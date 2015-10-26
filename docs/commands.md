# Commands (language-agnostic)

Some details of the client commands vary between languages.  This is especially true for exception types, synchronous vs. asynchronous response patterns, and camel- vs. snake-casing.  You can find these differences detailed in the language-specific client pages.

Aside from those details, the fundamental commands for interfacing with **relevanced** are the same across all clients.  They're described below.

---

## `getTextSimilarity`
#### `(centroidId: string, text: string) -> double`
Transforms `text` into a normalized term-frequency vector on the server, and then computes its similarity against the given centroid.

Returns the cosine similarity score as double-precision floating point.

If there is no centroid matching `centroidId`, the command fails with the exception `ECentroidDoesNotExist`.

---
## `multiGetTextSimilarity`
#### `(centroidIds: list<string>, text: string) -> MultiSimilarityResponse`
#### `{scores: map<string, double>}`

Transforms `text` into a normalized term-frequency vector on the server, and then computes its similarity against each of the centroids specified by `centroidIds`.

The `scores` property of the returned `MultiSimilarityResponse` is a `Map<string, double>` of centroid IDs to their corresponding similarity scores.

If any of the centroids specified by `centroidIds` do not exist, the entire command fails with the exception `ECentroidDoesNotExist`.

Note that the protocol does not distinguish between cases where a single centroid is missing and those where multiple centroids are missing.  These cases are all indicated by the exception `ECentroidDoesNotExist`.

---
## `getDocumentSimilarity`
#### `(centroidId: string, documentId: string) -> double`
Computes cosine similarity of a document that *already exists on the server* against the specified centroid.

Returns the cosine similarity score as double-precision floating point.

This command has the same purpose as `getSimilarityForText`, but is much more efficient in cases where the text has already been sent to the server for another reason.  In these cases the server can skip the text-processing phase.

If there is no centroid matching `centroidId`, the command fails with the exception `ECentroidDoesNotExist`.

If there is no document matching `documentId`, the command fails with the exception `EDocumentDoesNotExist`.

Note that if both the centroid and document do not exist, only the `ECentroidDoesNotExist` exception will be reported.

---
## `multiGetDocumentSimilarity`
#### `(centroidIds: list<string>, documentId: string) -> MultiSimilarityResponse`
#### `{scores: map<string, double>}`

Computes cosine similarity of the document with id `documentId` against each of the centroids specified by `centroidIds`, in parallel.

The `scores` property of the returned `MultiSimilarityResponse` is a `Map<string, double>` of centroid IDs to their corresponding similarity scores.

If the document does not exist, the command fails with the exception `EDocumentDoesNotExist`.

If any of the centroids specified by `centroidIds` do not exist, the entire command fails with the exception `ECentroidDoesNotExist`.

Note that the protocol does not distinguish between cases where a single centroid is missing and those where multiple centroids are missing.  These cases are all indicated by the exception `ECentroidDoesNotExist`.

Also note that if both the document and one or more centroids are missing, only the exception `ECentroidDoesNotExist` will be reported.


---
## `getCentroidSimilarity`
#### `(centroidId1: string, centroidId2: string) -> double`

Computes cosine similarity of two centroids.

Returns the cosine similarity score as double-precision floating point.

If either centroid is missing, the command fails with the exception `ECentroidDoesNotExist`.

Note that the protocol does not distinguish between cases where both centroids are missing and those where only one is missing.  In either case the `ECentroidDoesNotExist` exception will be returned, and it will only indicate the absence of a single centroid.

---
## `joinCentroid`
#### `(centroidId: string) -> JoinCentroidResponse`
#### `{id: string, recalculated: bool}`
Ensures a centroid is up to date before proceeding.
The server automatically recalculates centroids as documents are added and removed, so this is really a way of synchronizing with that process.  The specific behavior is:

- If the centroid is up to date, returns immediately.
- If an update job is currently in progress, returns once that job has completed.
- If the centroid is not up to date and no job is executing (because of the cool-off period), immediately starts the update and returns once it is complete.

The `recalculated` property of the returned `JoinCentroidResponse` indicates whether a recalculation was actually performed.

If the centroid given by `centroidId` does not exist, the command fails with the exception `ECentroidDoesNotExist`.

---
## `createDocument`
#### `(text: string) -> CreateDocumentResponse`
#### `{id: string}`

Sends a blob of unprocessed text to the server, which processes it and persists it.

The `id` property of the returned `CreateDocumentResponse` contains the UUID assigned to the processed document, which is needed for further commands involving that document.

---
## `createDocumentWithID`
#### `(id: string, text: string) -> CreateDocumentResponse`
#### `{id: string}`

Sends an ID and blob of unprocessed text to the server, which processes it and persists it under the given ID.

Like `createDocument` but also specifies an ID to use for the document, which the server will use instead of generating a UUID.

The `id` property of the returned `CreateDocumentResponse` contains the specified ID.

Document IDs must be unique.  Calling `createDocumentWithID` with an existing ID will result in an `EDocumentAlreadyExists` exception.

---
## `deleteDocument`
#### `(id: string) -> DeleteDocumentResponse`
#### `{id: string}`

Deletes the document associated with the given ID.  The `id` property of the returned `DeleteDocumentResponse` contains the same ID.

Throws an `EDocumentDoesNotExist` exception if the document does not exist.

If the document is associated with any centroids, those associations are automatically invalidated.  Most of this invalidation work (e.g. removing `RocksDB` entries) happens lazily during centroid recomputations.

---
## `createCentroid`
#### `(id: string) -> CreateCentroidResponse`
#### `{id: string}`

Creates a new centroid with the given ID.

The `id` property of the returned `CreateCentroidResponse` contains the same ID.

Centroid IDs must be unique.  Calling `createCentroid` with an existing centroid ID will result in an `ECentroidAlreadyExists` exception.

---
## `deleteCentroid`
#### `(id: string) -> DeleteCentroidResponse`
#### `{id: string}`

Deletes the centroid associated with the given ID.  The `id` property of the returned `DeleteCentroidResponse` contains the same ID.

Throws an `ECentroidDoesNotExist` exception if the centroid does not exist.

If the centroid is associated with any documents, the related centroid->document links will be invalidated and removed from the database lazily.

Because documents are independent entities which can be added to many centroids, deleting a given centroid does *not* delete any of the documents it is associated with.  It only results in removal of any database entries linking them to the deleted centroid.

---
## `addDocumentToCentroid`
#### `(centroidId: string, documentId: string) -> AddDocumentToCentroidResponse`
#### `{centroidId: string, documentId: string}`

Associates a document with a centroid.

The `centroidId` and `documentId` properties of the returned `AddDocumentToCentroidResponse` contain the specified centroid and document IDs, respectively.

If the centroid does not exist, throws an `ECentroidDoesNotExist` exception.
If the document does not exist, throws an `EDocumentDoesNotExist` exception.
If the document has already been added to the centroid, throws an `EDocumentAlreadyInCentroid` exception.

Note that if both the centroid and document do not exist, only the `ECentroidDoesNotExist` exception is reported.

If this command is successful, it initiates a background recomputation of the affected centroid.  This background job is intelligently scheduled with debounce and cool-off intervals, so making `N` calls to `addDocumentToCentroid` with the same `centroidId` will not result in `N` update jobs running on the server: it will result in one or a small number, depending on the exact calling pattern.  Once the centroid has been updated, its position will include the influence of the newly linked document.

---
## `removeDocumentFromCentroid`
#### `(centroidId: string, documentId: string) -> RemoveDocumentFromCentroidResponse`
#### `{centroidId: string, documentId: string}`
Unassociates a document from a centroid.

The `centroidId` and `documentId` properties of the returned `RemoveDocumentFromCentroidResponse` contain the specified centroid and document IDs, respectively.

If the centroid does not exist, throws an `ECentroidDoesNotExist` exception.
If the document does not exist, throws an `EDocumentDoesNotExist` exception.
If the document is not in the given centroid, throws an `EDocumentNotInCentroid` exception.

Note that if both the centroid and document do not exist, only the `ECentroidDoesNotExist` exception is reported.

If this command is successful, it initiates a background recomputation of the affected centroid.  This background job is intelligently scheduled with debounce and cool-off intervals, so making `N` calls to `addDocumentToCentroid` with the same `centroidId` will not result in `N` update jobs running on the server: it will result in one or a small number, depending on the exact calling pattern.  Once the centroid has been updated, its position will no longer be influenced by that unlinked document.

---
## `listAllCentroids`
#### `() -> ListCentroidsResponse`
#### `{centroids: list<string>}`
Lists the IDs of all centroids existing on the server.  The `centroids` property of the returned `ListCentroidsResponse` contains a `list<string>` of the IDs.

This command has no error conditions.

---
## `listAllDocuments`
#### `() -> ListDocumentsResponse`
#### `{documents: list<string>}`
Lists the IDs of all documents existing on the server.  The `documents` property of the returned `ListDocumentsResponse` contains a `list<string>` of the IDs.

This command has no error conditions.

---
## `listAllDocumentsForCentroid`
#### `(centroidId: string) -> ListCentroidDocumentsResponse`
#### `{documents: list<string>}`
Lists all document IDs associated with the given `centroidId`.

The `documents` property of the returned `ListCentroidDocumentsResponse` contains a `list<string>` of the document IDs.

If the centroid does not exist, throws an `ECentroidDoesNotExist` exception.

If the centroid exists but has no documents assigned to it, the `documents` property is an empty `list<string>` (i.e. this scenario is not considered an error).

