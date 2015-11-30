# Motivations

### Efficient similarity scoring against calculated centroids
Given that we have already calculated a centroid representing a large collection of documents, we want an efficient way to score new documents against the model.

A centroid consists of a large `string -> float` map.  This map is generally not *massive*, but it is large enough that loading and deserialization is a relatively expensive process.  If we need to score a stream of new documents against this model, loading it on demand for each request will significantly reduce throughput and increase latency.

The solution to this is to load the centroid into memory when we first need it, and keep it there while we process the incoming documents.  This is a good approach, but there a number of implementation issues:

- In a system of distributed worker machines, each worker will have to maintain its own copy of the model in memory.
- This is magnified by the (very likely) possibility that individual worker processes on a given machine do not have efficient access to shared memory, in which case each process will again load its own copy of the model into memory.
- Worker processes frequently die.  Sometimes this is due to an unhandled exception.  Often it is an intentional action by a supervising process, either to enforce task time limits or to reduce the impact of possible memory leaks.  Each time a worker process is killed and restarted in this manner, it will again have to obtain and deserialize the model.

These problems can be acceptable for a simple application, but they scale with the number of models in use.  A larger application may need to score hundreds or thousands of separate document streams against a corresponding number of centroids.  Alternately, it may need to score one document stream against many centroids for [multi-label](https://en.wikipedia.org/wiki/Multi-label_classification) or [multi-class](https://en.wikipedia.org/wiki/Multiclass_classification) classification.  In these cases, the simpler approach's inefficiencies can reach unacceptable levels.

**relevanced** addresses this by taking on the role of maintainer of the application's SVM models.  It keeps a single copy of each centroid loaded in memory, and knows to reload this copy whenever an update occurs.  Because it is written in well-profiled native code, it can compute scores against these centroids much more efficiently than workers written in most other languages would be able to.

### Serialization and persistence
Given a calculated centroid representing a collection of documents, we need a way to serialize it and a place to store its serialized representation so that it can be used by other processes.

Serialization may be into [libsvm format](https://www.csie.ntu.edu.tw/~cjlin/libsvmtools/datasets/), a given programming language's built-in serialization mechanism, JSON, or another format.  A given choice of format has implications on worker processes' ability to make use of the data: data in Python's `pickle` format will be difficult to use from a Java worker, and a given language may not have good libraries available for libsvm format.  Serialization format will also impact performance (JSON is relatively slow).

The serialized model may be stored on disk, in which case it will be unavailable to processes on other machines.  Alternately, it could be stored in a SQL `BLOB` column or in a key-value store like Redis.  These are sensible choices in the absence of other options, but they result in increased complexity and ongoing maintenance overhead.

Whenever a centroid is recomputed, **relevanced** serializes it in a compact binary format and stores it on disk in an embedded RocksDB database.  If the server is restarted, these models are again loaded into memory to improve similarity scoring latency.  Client applications don't ever need to deal with the server's internal format, as they can ask it for similarity scores directly.

### Updating centroids as documents are added or removed
Building a classic SVM model is an offline operation: the documents involved are known before we start, and the algorithm iterates over them to calculate their center in the vector space.

There a number of online approaches to approximating SVM, with widely varying accuracy and performance characteristics.  These algorithms attempt to calculate a centroid through an initial batch process and then update it as new documents are added, avoiding the expense of rebuilding the entire model.

While there is interesting work being done in this area, online variants are not nearly as well established or understood as the offline batch-processing approach.  They are also not really necessary for similarity scoring against even moderately-sized document collections.

A centroid labelled `fish` might consist of 10,000 wikipedia articles on various kinds of fish.  During a distributed web crawl, worker processes may add an additional ten documents to this centroid.  These ten additions will cause a relatively small change in a given document's cosine similarity with `fish`, so estimating the impact of each addition in real-time is wasted effort.

What matters is not that the model is a 100% accurate representation of the documents assigned to it at an instant in time, but that it is relatively up to date and regularly recalculated as documents are added and removed.  Assuming we have decided to go with the batch processing approach, then, we have several new requirements:

- We need to determine our tolerance for "staleness" of the model, and schedule update jobs as needed to keep the model within that tolerance.
- If "staleness" is defined in terms of additions or deletions, then we have to track that metadata somewhere.
- Alternately, if staleness is defined in terms of a last-update timestamp, then we still need to track that metadata somewhere.
- When we determine that an update is needed, some kind of worker system needs to be available to actually do the recalculation.
- Once the model is recalculated, we need a way to deploy it so that new similarity comparisons are done against the latest version of the model.  Any process that has loaded the model into memory needs to be notified that new data is available, at which point it should reload its stored copy.

When a change is made to a centroid's document membership, **relevanced** makes sure that a corresponding update job has been scheduled.  Update scheduling is throttled / debounced on a time interval, so adding 10,000 documents to a centroid over one minute does not cause 10,000 separate update jobs to be scheduled.  After the first update is scheduled, additional changes within the interval will not trigger additional updates *until the end of the interval*, at which point a final update job is kicked off to see if anything has been missed.  This keeps load reasonable during a burst of activity while ensuring that models are kept up to date after the burst ends.

### Caching intermediate results
When we regularly recompute a centroid to keep it up to date, many of the documents involved will have already been seen during previous update cycles.

There is a relatively expensive part of the computation which must be performed for each document: turning its associated blob of text into a term frequency vector.  To do this, we have to:

- Tokenize the text
- Remove stopwords `("a", "the", "him")`
- Stem the tokens `("slowly" -> "slow")`
- Normalize term scores against overall document length.

Caching these intermediate results can be a significant optimization to centroid recalculation.  However, this requires:

- A datastore
- A way to retrieve the cached results for a given document
- An updating job which is aware of the cache and can efficiently access it

These all imply additional infrastructure and additional code.  Many systems avoid the added complexity by just leaving this optimization on the table.

**relevanced** computes a normalized term-frequency vector of each document when it is added to the server, meaning centroid recomputation jobs skip the text processing phase and go directly to finding the center of the documents' vector representations.  This approach significantly speeds up recomputation, and also makes it cheap to add a single document to many different centroids.  The heavy text processing is only done once.
