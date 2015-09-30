**Efficient cosine similarity scores**.  Centroids are kept in memory to speed up similarity score requests.  As with all of **relevanced**, relevance scoring and document vectorization is written in C++ for performance.

**Efficient scoring against multiple centroids**.  Common approaches to [multi-label](https://en.wikipedia.org/wiki/Multi-label_classification) and [multi-class](https://en.wikipedia.org/wiki/Multiclass_classification) classification score a given document against a number of different centroids.  In multiclass classification, we are interested in determining which of `N` centroids is most similar to the given document.  In the case of multilabel classification, we decide whether to assign each of `N` different labels to a given document based on its similarity scores against `N` corresponding centroids.  In its `multi_get_text_similarity` operation, **relevanced** makes the following optimizations for multi-centroid scoring:

- The document is only vectorized once, after which the same term frequency vector is used for each centroid comparison.
- Reduces `N` network calls to 1.
- Computes similarity against the requested centroids in parallel across multiple worker threads.


