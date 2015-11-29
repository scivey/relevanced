# Python API

---

### `get_text_similarity`

`(centroid_id, text, language = relevanced_client.Language.EN)`

`-> double`


Transforms `text` into a normalized term-frequency vector on the server, using stopwords and tokenization rules given by `language`, and then computes its similarity against the given centroid.

Returns the cosine similarity score as a double.

Raises `relevanced_client.ECentroidDoesNotExist` if there is no centroid matching `centroidId`.

---
### `multi_get_text_similarity`

`(centroid_ids, text, language = relevanced_client.Language.EN)`

`-> MultiSimilarityResponse(scores: dict<string, double>)`

Transforms `text` into a normalized term-frequency vector on the server, using stopwords and tokenization rules given by `language`, and then computes its similarity against each of the centroids in the list `centroid_ids`.

The `scores` property of the returned `MultiSimilarityResponse` maps centroid IDs to their corresponding similarity scores.

Raises `relevanced_client.ECentroidDoesNotExist` if any of the centroids specified by `centroid_ids` does not exist.

Note that the protocol does not distinguish between cases where a single centroid is missing and those where multiple centroids are missing.  These cases are all indicated by the exception `ECentroidDoesNotExist`.

---
### `get_document_similarity`

`(centroid_id, document_id)`

`-> double`

Computes cosine similarity of a document that *already exists on the server* against the specified centroid.

Returns the cosine similarity score as a double.

This command has the same purpose as `get_text_similarity`, but is much more efficient in cases where the document has already been added to the server (e.g. for addition to another centroid).  In these cases the server can skip the text-processing phase.

Raises `relevanced_client.ECentroidDoesNotExist` if there is no centroid matching `centroidId`.

Raises `relevanced_client.EDocumentDoesNotExist` if there is no document matching `documentId`.

Note that if both the centroid and document do not exist, only the `EDocumentDoesNotExist` exception will be reported.

---
### `multi_get_document_similarity`

`(centroid_ids, document_id)`

`-> MultiSimilarityResponse(scores: dict<String, double>)`

Computes cosine similarity of the document with id `documentId` against each of the centroids specified by `centroid_ids`, in parallel.

The `scores` property of the returned `MultiSimilarityResponse` is a dict mapping centroid IDs to their corresponding similarity scores.

Raises `relevanced_client.EDocumentDoesNotExist` if there is no document matching `documentId`.

Raises `relevanced_client.ECentroidDoesNotExist` if any of the centroids specified by `centroid_ids` does not exist.

Note that the protocol does not distinguish between cases where a single centroid is missing and those where multiple centroids are missing.  These cases are all indicated by the exception `ECentroidDoesNotExist`.

Also note that if both the document and one or more centroids are missing, only the exception `EDocumentDoesNotExist` will be reported.

---
### `get_centroid_similarity`

`(centroid_1_id, centroid_2_id)`

`-> double`

Computes cosine similarity of two centroids, returning the score as a double.

Raises `relevanced_client.ECentroidDoesNotExist` if either centroid is missing.

---
### `join_centroid`

`(centroid_id)`

`-> JoinCentroidResponse(id: string, recalculated: bool)`

Ensures a centroid is up to date before proceeding.
The server automatically recalculates centroids as documents are added and removed.  `join_centroid` provides a way to synchronize with that process.  The specific behavior is:

- If the centroid is up to date, returns immediately.
- If an update job is currently in progress, returns once that job has completed.
- If the centroid is not up to date and no job is executing (because of the cool-off period), immediately starts the update and returns once it is complete.

The `recalculated` property of the returned `JoinCentroidResponse` indicates whether a recalculation was actually performed.

Raises `relevanced_client.ECentroidDoesNotExist` if `centroid_id` refers to a nonexistent centroid.

---
### `create_document`

`(text, language = relevanced_client.Language.EN)`

`-> CreateDocumentResponse(id: string)`

Sends a blob of unprocessed text to the server, which processes it and persists it.  Processing uses stopwords and tokenization rules according to the specified `language`.

The `id` property of the returned `CreateDocumentResponse` contains the UUID assigned to the processed document, which is needed for further commands involving that document.

---
### `create_document_with_id`

`(id, text, language = relevanced_client.Language.EN)`

`-> CreateDocumentResponse(id: string)`

Sends an ID and blob of unprocessed text to the server, which processes it and persists it under the given ID.  Processing uses stopwords and tokenization rules according to the specified `language`.

This command is similar to `create_document`, but the server will use the provided `id` instead of generating a UUID for the document.

Raises `relevanced_client.EDocumentAlreadyExists` if a document with the specified ID already exists.

---
### `delete_document`

`(id)`

`-> DeleteDocumentResponse(id: string)`

Deletes the document associated with the given ID.  The `id` property of the returned `DeleteDocumentResponse` contains the same ID.

Raises `relevanced_client.EDocumentDoesNotExist` if the document does not exist.

If the document is associated with any centroids, those associations are automatically invalidated.  Most of this invalidation work happens lazily during centroid recomputations.

---
### `create_centroid`

`(id)`

`-> CreateCentroidResponse(id: string)`

Creates a new centroid with the given ID.

The `id` property of the returned `CreateCentroidResponse` contains the same ID.

Raises `relevanced_client.ECentroidAlreadyExists` if a centroid with the given ID already exists.

---
### `delete_centroid`

`(id)`

`-> DeleteCentroidResponse(id: string)`

Deletes the centroid associated with the given ID.  The `id` property of the returned `DeleteCentroidResponse` contains the same ID.

Raises `relevanced_client.ECentroidDoesNotExist` if the centroid does not exist.

If the centroid is associated with any documents, the related centroid->document links will be invalidated and removed from the database lazily.

Because documents are independent entities which can be added to many centroids, deleting a given centroid does *not* delete any of the documents it is associated with.  It only results in removal of any database entries linking them to the deleted centroid.

---
### `add_documents_to_centroid`

`(centroid_id, document_ids, ignore_already_in_centroid=False)`

`-> AddDocumentsToCentroidResponse`

Associates multiple documents with a centroid.

Raises `relevanced_client.ECentroidDoesNotExist` if `centroid_id` refers to a nonexistent centroid.

Raises `relevanced_client.EDocumentDoesNotExist` if any of the documents in `document_ids` does not exist.

Raises `relevanced_client.EDocumentAlreadyInCentroid` if any of the documents in `document_ids` is already in the centroid, unless `ignore_already_in_centroid=True` is passed.

If successful, the centroid is automatically recalculated after a cool-down interval.

---
### `remove_documents_from_centroid`

`(centroid_id, document_ids, ignore_not_in_centroid=False)`

`-> RemoveDocumentsFromCentroidResponse`

Unassociates multiple documents from a centroid.

Raises `relevanced_client.ECentroidDoesNotExist` if `centroid_id` refers to a nonexistent centroid.

Raises `relevanced_client.EDocumentDoesNotExist` if any of the documents in `document_ids` does not exist.

Raises `relevanced_client.EDocumentNotInCentroid` if any of the documents in `document_ids` is not currently associated with the centroid, unless `ignore_not_in_centroid=True` is passed.

If successful, the centroid is automatically recalculated after a cool-down interval.

---
### `list_all_centroids`

`()`

`-> ListCentroidsResponse(centroids: list<string>)`

Lists the IDs of all centroids existing on the server.

This command has no error conditions.

---
### `list_all_documents`

`()`

`-> ListDocumentsResponse(documents: list<string>)`

Lists the IDs of all documents existing on the server.

This command has no error conditions.

