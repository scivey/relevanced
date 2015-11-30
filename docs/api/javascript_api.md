# Javascript API

---

All of the Javascript client's methods return ES6-style promises.

---

## Similarity Scoring

### `getTextSimilarity`

`(centroidId, text, language = RelevancedClient.Language.EN)`

`-> future[double]`


Transforms `text` into a normalized term-frequency vector on the server, using stopwords and tokenization rules given by `language`, and then computes its similarity against the given centroid.

Returns a future resolving to the cosine similarity score as a double.

Resolves to a `RelevancedClient.ECentroidDoesNotExist` exception if there is no centroid matching `centroidId`.

---
### `multiGetTextSimilarity`

`(centroidIds, text, language = RelevancedClient.Language.EN)`

`-> future[MultiSimilarityResponse(scores: map<string, double>)]`

Transforms `text` into a normalized term-frequency vector on the server, using stopwords and tokenization rules given by `language`, and then computes its similarity against each of the centroids in the list `centroid_ids`.

Returns a future resolving to a `MultiSimilarityResponse` object.  The `scores` property of this object is a mapping from centroid IDs to their corresponding similarity scores.

Resolves to a `RelevancedClient.ECentroidDoesNotExist` exception if any of the centroids specified by `centroid_ids` does not exist.

Note that the protocol does not distinguish between cases where a single centroid is missing and those where multiple centroids are missing.  These cases are all indicated by the exception `ECentroidDoesNotExist`.

---
### `getDocumentSimilarity`

`(centroidId, documentId)`

`-> future[double]`

Computes cosine similarity of a document that *already exists on the server* against the specified centroid.

Returns a future resolving to the cosine similarity score as a double.

This command has the same purpose as `getTextSimilarity`, but is much more efficient in cases where the document has already been added to the server (e.g. for addition to another centroid).  In these cases the server can skip the text-processing phase.

Resolves to a `RelevancedClient.ECentroidDoesNotExist` exception if there is no centroid matching `centroidId`.

Resolves to a `RelevancedClient.EDocumentDoesNotExist` exception if there is no document matching `documentId`.

Note that if both the centroid and document do not exist, only the `EDocumentDoesNotExist` exception will be reported.

---
### `multiGetDocumentSimilarity`

`(centroidIds, documentId)`

`-> future[MultiSimilarityResponse(scores: dict<String, double>)]`

Computes cosine similarity of the document with id `documentId` against each of the centroids specified by `centroid_ids`, in parallel.

Returns a future resolving to a `MultiSimilarityResponse` object.  The `scores` property of this object is a mapping from centroid IDs to their corresponding similarity scores.

Resolves to a `RelevancedClient.EDocumentDoesNotExist` exception if the specified document does not exist.

Resolves to a `RelevancedClient.ECentroidDoesNotExist` exception if any of the centroids specified by `centroid_ids` does not exist.

Note that the protocol does not distinguish between cases where a single centroid is missing and those where multiple centroids are missing.  These cases are all indicated by the exception `ECentroidDoesNotExist`.

Also note that if both the document and one or more centroids are missing, only the exception `EDocumentDoesNotExist` will be reported.

---
### `getCentroidSimilarity`

`(centroid1Id, centroid2Id)`

`-> future[double]`

Computes cosine similarity of two centroids.

Returns a future resolving to the score as a double.

Resolves to a `RelevancedClient.ECentroidDoesNotExist` exception if either centroid is missing.

---

## Document CRUD
### `createDocument`

`(text, language = RelevancedClient.Language.EN)`

`-> future[CreateDocumentResponse(id: string)]`

Sends a blob of unprocessed text to the server, which processes it and persists it.  Processing uses stopwords and tokenization rules according to the specified `language`.

Returns a future resolving to a `CreateDocumentResponse` object.  The `id` property of this object contains the UUID assigned to the processed document.

---
### `createDocumentWithID`

`(id, text, language = RelevancedClient.Language.EN)`

`-> future[CreateDocumentResponse(id: string)]`

Sends an ID and blob of unprocessed text to the server, which processes it and persists it under the given ID.  Processing uses stopwords and tokenization rules according to the specified `language`.

This command is similar to `createDocument`, but the server will use the provided `id` instead of generating a UUID for the document.

Resolves to a `RelevancedClient.EDocumentAlreadyExists` exception if a document with the specified ID already exists.

---
### `deleteDocument`

`(documentId)`

`-> future[DeleteDocumentResponse(id: string)]`

Deletes the document associated with the given ID.  The `id` property of the returned `DeleteDocumentResponse` contains the same ID.

Resolves to a `RelevancedClient.EDocumentDoesNotExist` exception if the document does not exist.

If the document is associated with any centroids, those associations are automatically invalidated.  Most of this invalidation work happens lazily during centroid recomputations.

---
### `multiDeleteDocuments`

`(documentIds, ignoreMissing=false)`

`-> MultiDeleteDocumentsResponse(ids: list<string>)`

Deletes multiple documents on the server.  See the `deleteDocument` command for details of document deletion.

Resolves to a `RelevancedClient.EDocumentDoesNotExist` exception if any of the documents does not exist, unless `ignoreMissing` is `true.

The `ids` property of the returned response object lists the IDs of the documents that were deleted.  If `ignoreMissing=true` was passed, this list will only include the documents that were really deleted (not missing).


---
### `listAllDocuments`

`()`

`-> future[ListDocumentsResponse(documents: list<string>)]`

Lists the IDs of all documents existing on the server.

This command has no error conditions.

---

## Centroid CRUD
### `createCentroid`

`(centroidId)`

`-> future[CreateCentroidResponse(id: string)]`

Creates a new centroid with the given ID.

The `id` property of the returned `CreateCentroidResponse` contains the same ID.

Resolves to a `RelevancedClient.ECentroidAlreadyExists` exception if a centroid with the given ID already exists.

---
### `multiCreateCentroids`

`(centroidIds, ignoreExisting=false)`

`-> MultiCreateCentroidsResponse(created: list<string>)`

Created multiple new centroids on the server.

Resolves to a `RelevancedClient.ECentroidAlreadyExists` exception if any of the centroids already exists, unless `ignoreExisting=true` was passed.

The `created` property of the returned `MultiCreateCentroidsResponse` contains a list of IDs of the created centroids.  If `ignoreExisting=True` was passed, this list will only include the centroids that were really created (didn't already exist).


---
### `deleteCentroid`

`(centroidId)`

`-> future[DeleteCentroidResponse(id: string)]`

Deletes the centroid associated with the given ID.  The `id` property of the returned `DeleteCentroidResponse` contains the same ID.

Resolves to a `RelevancedClient.ECentroidDoesNotExist` exception if the centroid does not exist.

If the centroid is associated with any documents, the related centroid->document links will be invalidated and removed from the database lazily.

Because documents are independent entities which can be added to many centroids, deleting a given centroid does *not* delete any of the documents it is associated with.  It only results in removal of any database entries linking them to the deleted centroid.

---
### `multiDeleteCentroids`
`(centroidIds, ignoreMissing=false)`

`-> MultiDeleteCentroidsResponse(ids: list<string>)`

Deletes multiple centroids on the server.  See the `deleteCentroid` command for details of centroid deletion.

Resolves to a `RelevancedClient.ECentroidDoesNotExist` exception if any of the centroids does not exist, unless `ignoreMissing=true` was passed.

The `ids` property of the returned response object lists the IDs of the centroids that were deleted.  If `ignoreMissing=true` was passed, this list will only include the centroids that were really deleted (not missing).

---
### `listAllCentroids`

`()`

`-> future[ListCentroidsResponse(centroids: list<string>)]`

Lists the IDs of all centroids existing on the server.

This command has no error conditions.

---
## Centroid-Document CRUD
### `addDocumentsToCentroid`

`(centroidId, documentIds, ignoreAlreadyInCentroid=false)`

`-> future[AddDocumentsToCentroidResponse]`

Associates multiple documents with a centroid.

Resolves to a `RelevancedClient.ECentroidDoesNotExist` exception if `centroidId` refers to a nonexistent centroid.

Resolves to a `RelevancedClient.EDocumentDoesNotExist` exception if any of the documents in `documentIds` does not exist.

Resolves to a `RelevancedClient.EDocumentAlreadyInCentroid` exception if any of the documents in `documentIds` is already in the centroid, unless `ignoreAlreadyInCentroid` is `true`.

If successful, the centroid is automatically recalculated after a cool-down interval.

---
### `removeDocumentsFromCentroid`

`(centroidId, documentIds, ignoreNotInCentroid=false)`

`-> future[RemoveDocumentsFromCentroidResponse]`

Unassociates multiple documents from a centroid.

Resolves to a `RelevancedClient.ECentroidDoesNotExist` exception if `centroidId` refers to a nonexistent centroid.

Resolves to a `RelevancedClient.EDocumentDoesNotExist` exception if any of the documents in `documentIds` does not exist.

Resolves to a `RelevancedClient.EDocumentNotInCentroid` exception if any of the documents in `documentIds` is not currently associated with the centroid, unless `ignoreNotInCentroid` is `true`.

If successful, the centroid is automatically recalculated after a cool-down interval.


---

## Synchronization
### `joinCentroid`

`(centroidId)`

`-> future[JoinCentroidResponse(id: string, recalculated: bool)]`

Ensures a centroid is up to date before proceeding.
The server automatically recalculates centroids as documents are added and removed.  `joinCentroid` provides a way to synchronize with that process.  The specific behavior is:

- If the centroid is up to date, returns immediately.
- If an update job is currently in progress, returns once that job has completed.
- If the centroid is not up to date and no job is executing (because of the cool-off period), immediately starts the update and returns once it is complete.

The `recalculated` property of the returned `JoinCentroidResponse` indicates whether a recalculation was actually performed.

Raises `RelevancedClient.ECentroidDoesNotExist` if `centroidId` refers to a nonexistent centroid.

---
### `multiJoinCentroids`

`(centroidIds)`

`-> future[MultiJoinCentroidsResponse(ids: list<string>, recalculated: list<bool>)]`

Like `joinCentroid`, but runs against multiple centroids in a single request.

Resolves to a `RelevancedClient.ECentroidDoesNotExist` exception if any of the `centroidIds` refers to a nonexistent centroid.
