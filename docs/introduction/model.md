# Text Similarity Model

**relevanced** follows the established vector space model for text similarity: documents are represented as length-adjusted term frequency vectors, groups of documents are centroids of those vectors, and cosine similarity determines relatedness.

As documents are added and removed from centroids, worker threads periodically recalculate the underlying models to keep them up to date.  This combines the accuracy and well-understood behavior of classic offline SVM with the convenience of its online approximations.

Processed centroids are kept in memory for fast comparison with new documents, while documents themselves are stored on disk in an embedded RocksDB database.  When a centroid is recalculated, its associated documents are streamed into the server in small batches.  This means that the number and size of documents which can be assigned to a single centroid is not constrained by memory, but only by disk space.  It also means that **relevanced** can maintain a relatively consistent memory footprint.

While the math underlying SVM is established and well understood, there are [many practical considerations](./motivations.md) to using such a model in a distributed production system that are not handled by existing open source software.  Any application dealing with text similarity ends up building its own home-grown solutions to these problems.

**relevanced** provides efficient, well-tested plumbing which addresses these issues.  It gives client applications access to the power of SVMs while shielding them from the complexity of updating, persisting and redeploying models.
