## relevanced

*relevanced* provides persistent, named binary document classifiers.

It maintains an in-memory centroid for each collection, enabling rapid classification with low memory requirements.

Documents are stored on disk in a preprocessed form, and streamed into memory during recalculation of the centroid.

Centroids are recalculated in batches with a standard offline algorithm.  This allows for relatively up-to-date models while avoiding the memory overhead and complexity of online variants.

*relevanced*'s interface is specified as a Thrift service.  Clients can be easily generated in many languages.
