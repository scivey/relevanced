# Java API

---

### `getTextSimilarity`

`(String centroidId, String text, org.relevanced.client.protocol.Language language = Language::EN)`

`-> double`


Transforms `text` into a normalized term-frequency vector on the server, using stopwords and tokenization rules given by `language`, and then computes its similarity against the given centroid.

Returns the cosine similarity score as a double.

Raises `org.relevanced.client.protocol.ECentroidDoesNotExist` if there is no centroid matching `centroidId`.

---
### `multiGetTextSimilarity`

`(List<String> centroidIds, String text, org.relevanced.client.protocol.Language language = Language::EN)`

`-> MultiSimilarityResponse(scores: map<String, double>)`

Transforms `text` into a normalized term-frequency vector on the server, using stopwords and tokenization rules given by `language`, and then computes its similarity against each of the centroids in the list `centroidIds`.

The `scores` property of the returned `MultiSimilarityResponse` maps centroid IDs to their corresponding similarity scores.

Raises `org.relevanced.client.protocol.ECentroidDoesNotExist` if any of the centroids specified by `centroidIds` does not exist.

Note that the protocol does not distinguish between cases where a single centroid is missing and those where multiple centroids are missing.  These cases are all indicated by the exception `ECentroidDoesNotExist`.

---
### `getDocumentSimilarity`

`(String centroidId, String documentId)`

`-> double`

Computes cosine similarity of a document that *already exists on the server* against the specified centroid.

Returns the cosine similarity score as a double.

This command has the same purpose as `getTextSimilarity`, but is much more efficient in cases where the document has already been added to the server (e.g. for addition to another centroid).  In these cases the server can skip the text-processing phase.

Raises `org.relevanced.client.protocol.ECentroidDoesNotExist` if there is no centroid matching `centroidId`.

Raises `org.relevanced.client.protocol.EDocumentDoesNotExist` if there is no document matching `documentId`.

Note that if both the centroid and document do not exist, only the `EDocumentDoesNotExist` exception will be reported.

---
### `multiGetDocumentSimilarity`

`(List<String> centroidIds, String documentId)`

`-> MultiSimilarityResponse(scores: map<String, double>)`

Computes cosine similarity of the document with id `documentId` against each of the centroids specified by `centroidIds`, in parallel.

The `scores` property of the returned `MultiSimilarityResponse` is a dict mapping centroid IDs to their corresponding similarity scores.

Raises `org.relevanced.client.protocol.EDocumentDoesNotExist` if there is no document matching `documentId`.

Raises `org.relevanced.client.protocol.ECentroidDoesNotExist` if any of the centroids specified by `centroidIds` does not exist.

Note that the protocol does not distinguish between cases where a single centroid is missing and those where multiple centroids are missing.  These cases are all indicated by the exception `ECentroidDoesNotExist`.

Also note that if both the document and one or more centroids are missing, only the exception `EDocumentDoesNotExist` will be reported.

---
### `getCentroidSimilarity`

`(String centroid1Id, String centroid2Id)`

`-> double`

Computes cosine similarity of two centroids, returning the score as a double.

Raises `org.relevanced.client.protocol.ECentroidDoesNotExist` if either centroid is missing.

---
### `joinCentroid`

`(String centroidId)`

`-> JoinCentroidResponse(id: String, recalculated: bool)`

Ensures a centroid is up to date before proceeding.
The server automatically recalculates centroids as documents are added and removed.  `joinCentroid` provides a way to synchronize with that process.  The specific behavior is:

- If the centroid is up to date, returns immediately.
- If an update job is currently in progress, returns once that job has completed.
- If the centroid is not up to date and no job is executing (because of the cool-off period), immediately starts the update and returns once it is complete.

The `recalculated` property of the returned `JoinCentroidResponse` indicates whether a recalculation was actually performed.

Raises `org.relevanced.client.protocol.ECentroidDoesNotExist` if `centroidId` refers to a nonexistent centroid.

---
### `createDocument`

`(String text, org.relevanced.client.protocol.Language language = Language::EN)`

`-> CreateDocumentResponse(id: String)`

Sends a blob of unprocessed text to the server, which processes it and persists it.  Processing uses stopwords and tokenization rules according to the specified `language`.

The `id` property of the returned `CreateDocumentResponse` contains the UUID assigned to the processed document, which is needed for further commands involving that document.

---
### `CreateDocumentWithID`

`(String id, String text, org.relevanced.client.protocol.Language language = Language::EN)`

`-> CreateDocumentResponse(id: String)`

Sends an ID and blob of unprocessed text to the server, which processes it and persists it under the given ID.  Processing uses stopwords and tokenization rules according to the specified `language`.

This command is similar to `createDocument`, but the server will use the provided `id` instead of generating a UUID for the document.

Raises `org.relevanced.client.protocol.EDocumentAlreadyExists` if a document with the specified ID already exists.

---
### `deleteDocument`

`(String id)`

`-> DeleteDocumentResponse(id: String)`

Deletes the document associated with the given ID.  The `id` property of the returned `DeleteDocumentResponse` contains the same ID.

Raises `org.relevanced.client.protocol.EDocumentDoesNotExist` if the document does not exist.

If the document is associated with any centroids, those associations are automatically invalidated.  Most of this invalidation work happens lazily during centroid recomputations.

---
### `createCentroid`

`(String id)`

`-> CreateCentroidResponse(id: String)`

Creates a new centroid with the given ID.

The `id` property of the returned `CreateCentroidResponse` contains the same ID.

Raises `org.relevanced.client.protocol.ECentroidAlreadyExists` if a centroid with the given ID already exists.

---
### `deleteCentroid`

`(id)`

`-> DeleteCentroidResponse(id: string)`

Deletes the centroid associated with the given ID.  The `id` property of the returned `DeleteCentroidResponse` contains the same ID.

Raises `org.relevanced.client.protocol.ECentroidDoesNotExist` if the centroid does not exist.

If the centroid is associated with any documents, the related centroid->document links will be invalidated and removed from the database lazily.

Because documents are independent entities which can be added to many centroids, deleting a given centroid does *not* delete any of the documents it is associated with.  It only results in removal of any database entries linking them to the deleted centroid.

---
### `addDocumentsToCentroid`

`(String centroidId, List<String> documentIds, Boolean ignoreAlreadyInCentroid=False)`

`-> AddDocumentsToCentroidResponse`

Associates documents with a centroid.

Raises `org.relevanced.client.protocol.ECentroidDoesNotExist` if `centroidId` refers to a nonexistent centroid.

Raises `org.relevanced.client.protocol.EDocumentDoesNotExist` if any of the documents in `documentIds` does not exist.

Raises `org.relevanced.client.protocol.EDocumentAlreadyInCentroid` if any of the documents in `documentIds` is already in the centroid, unless `ignoreAlreadyInCentroid` is `true`.

If successful, the centroid is automatically recalculated after a cool-down interval.

---
### `removeDocumentsFromCentroid`

`(String centroidId, List<String> documentIds, Boolean ignoreNotInCentroid=False)`

`-> RemoveDocumentsFromCentroidResponse`

Unassociates documents from a centroid.

Raises `org.relevanced.client.protocol.ECentroidDoesNotExist` if `centroidId` refers to a nonexistent centroid.

Raises `org.relevanced.client.protocol.EDocumentDoesNotExist` if any of the documents in `documentIds` does not exist.

Raises `org.relevanced.client.protocol.EDocumentNotInCentroid` if any of the documents in `documentIds` is not currently associated with the centroid, unless `ignoreNotInCentroid` is `true`.

If successful, the centroid is automatically recalculated after a cool-down interval.

---
### `listAllCentroids`

`()`

`-> ListCentroidsResponse(centroids: List<String>)`

Lists the IDs of all centroids existing on the server.

This command has no error conditions.

---
### `listAllDocuments`

`()`

`-> ListDocumentsResponse(documents: List<String>)`

Lists the IDs of all documents existing on the server.

This command has no error conditions.

