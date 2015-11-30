# Ruby API

---
## Similarity Scoring

### `get_text_similarity`

`(centroid_id, text, language = Language::EN)`

`-> double`


Transforms `text` into a normalized term-frequency vector on the server, using stopwords and tokenization rules given by `language`, and then computes its similarity against the given centroid.

Returns the cosine similarity score as a double.

Raises `ECentroidDoesNotExist` if there is no centroid matching `centroidId`.

---
### `multi_get_text_similarity`

`(centroid_ids, text, language = Language::EN)`

`-> MultiSimilarityResponse(scores: dict<string, double>)`

Transforms `text` into a normalized term-frequency vector on the server, using stopwords and tokenization rules given by `language`, and then computes its similarity against each of the centroids in the list `centroid_ids`.

The `scores` property of the returned `MultiSimilarityResponse` maps centroid IDs to their corresponding similarity scores.

Raises `ECentroidDoesNotExist` if any of the centroids specified by `centroid_ids` does not exist.

Note that the protocol does not distinguish between cases where a single centroid is missing and those where multiple centroids are missing.  These cases are all indicated by the exception `ECentroidDoesNotExist`.

---
### `get_document_similarity`

`(centroid_id, document_id)`

`-> double`

Computes cosine similarity of a document that *already exists on the server* against the specified centroid.

Returns the cosine similarity score as a double.

This command has the same purpose as `get_text_similarity`, but is much more efficient in cases where the document has already been added to the server (e.g. for addition to another centroid).  In these cases the server can skip the text-processing phase.

Raises `ECentroidDoesNotExist` if there is no centroid matching `centroidId`.

Raises `EDocumentDoesNotExist` if there is no document matching `documentId`.

Note that if both the centroid and document do not exist, only the `EDocumentDoesNotExist` exception will be reported.

---
### `multi_get_document_similarity`

`(centroid_ids, document_id)`

`-> MultiSimilarityResponse(scores: dict<String, double>)`

Computes cosine similarity of the document with id `documentId` against each of the centroids specified by `centroid_ids`, in parallel.

The `scores` property of the returned `MultiSimilarityResponse` is a dict mapping centroid IDs to their corresponding similarity scores.

Raises `EDocumentDoesNotExist` if there is no document matching `documentId`.

Raises `ECentroidDoesNotExist` if any of the centroids specified by `centroid_ids` does not exist.

Note that the protocol does not distinguish between cases where a single centroid is missing and those where multiple centroids are missing.  These cases are all indicated by the exception `ECentroidDoesNotExist`.

Also note that if both the document and one or more centroids are missing, only the exception `EDocumentDoesNotExist` will be reported.

---
### `get_centroid_similarity`

`(centroid_1_id, centroid_2_id)`

`-> double`

Computes cosine similarity of two centroids, returning the score as a double.

Raises `ECentroidDoesNotExist` if either centroid is missing.

---

## Document CRUD

### `create_document`

`(text, language = Language::EN)`

`-> CreateDocumentResponse(id: string)`

Sends a blob of unprocessed text to the server, which processes it and persists it.  Processing uses stopwords and tokenization rules according to the specified `language`.

The `id` property of the returned `CreateDocumentResponse` contains the UUID assigned to the processed document, which is needed for further commands involving that document.

---
### `create_document_with_id`

`(id, text, language = Language::EN)`

`-> CreateDocumentResponse(id: string)`

Sends an ID and blob of unprocessed text to the server, which processes it and persists it under the given ID.  Processing uses stopwords and tokenization rules according to the specified `language`.

This command is similar to `create_document`, but the server will use the provided `id` instead of generating a UUID for the document.

Raises `EDocumentAlreadyExists` if a document with the specified ID already exists.

---
### `delete_document`

`(id)`

`-> DeleteDocumentResponse(id: string)`

Deletes the document associated with the given ID.  The `id` property of the returned `DeleteDocumentResponse` contains the same ID.

Raises `EDocumentDoesNotExist` if the document does not exist.

If the document is associated with any centroids, those associations are automatically invalidated.  Most of this invalidation work happens lazily during centroid recomputations.

---

### `multi_delete_documents`

`(document_ids, ignore_missing=false)`

`-> MultiDeleteDocumentsResponse(ids: list<string>)`

Deletes multiple documents on the server.  See the `delete_document` command for details of document deletion.

If any of the documents does not exist, raises `EDocumentDoesNotExist` unless `ignore_missing=true` was passed.

The `ids` property of the returned response object lists the IDs of the documents that were deleted.  If `ignore_missing=true` was passed, this list will only include the documents that were really deleted (not missing).

---

### `list_all_documents`

`()`

`-> ListDocumentsResponse(documents: list<string>)`

Lists the IDs of all documents existing on the server.

This command has no error conditions.


---
### `list_document_range`

`(offset, count)`

`-> ListDocumentsResponse(documents: list<string>)`

Lists a range of IDs of documents existing on the server.  With keys sorted alphabetically, skips the first `offset` entries and returns up to `count` entries after that point.

This command has no error conditions.

---
### `list_document_range_from_id`

`(document_id, count)`

`-> ListDocumentsResponse(documents: list<string>)`

Lists a range of IDs of documents existing on the server.  With keys sorted alphabetically, skips to `document_id` and then continues to return up to `count` total entries.  This count includes the starting `document_id`.

A missing `document_id` is not an error condition for this command.  If `document_id` is not found, iteration will start from the first ID located after it alphabetically.

This command has no error conditions.

---

## Centroid CRUD
### `create_centroid`

`(id)`

`-> CreateCentroidResponse(id: string)`

Creates a new centroid with the given ID.

The `id` property of the returned `CreateCentroidResponse` contains the same ID.

Raises `ECentroidAlreadyExists` if a centroid with the given ID already exists.

---
### `multi_create_centroids`

`(centroid_ids, ignore_existing=false)`

`-> MultiCreateCentroidsResponse(created: list<string>)`

Created multiple new centroids on the server.

Raises `ECentroidAlreadyExists` if any of the centroids already exists, unless `ignore_existing=true` was passed.

The `created` property of the returned `MultiCreateCentroidsResponse` contains a list of IDs of the created centroids.  If `ignore_existing=true` was passed, this list will only include the centroids that were really created (didn't already exist).


---
### `delete_centroid`

`(id)`

`-> DeleteCentroidResponse(id: string)`

Deletes the centroid associated with the given ID.  The `id` property of the returned `DeleteCentroidResponse` contains the same ID.

Raises `ECentroidDoesNotExist` if the centroid does not exist.

If the centroid is associated with any documents, the related centroid->document links will be invalidated and removed from the database lazily.

Because documents are independent entities which can be added to many centroids, deleting a given centroid does *not* delete any of the documents it is associated with.  It only results in removal of any database entries linking them to the deleted centroid.

---
### `multi_delete_centroids`

`(centroid_ids, ignore_missing=false)`

`-> MultiDeleteCentroidsResponse(ids: list<string>)`

Deletes multiple centroids on the server.  See the `delete_centroid` command for details of centroid deletion.

If any of the centroids does not exist, raises `ECentroidDoesNotExist` unless `ignore_missing=true` was passed.

The `ids` property of the returned response object lists the IDs of the centroids that were deleted.  If `ignore_missing=true` was passed, this list will only include the centroids that were really deleted (not missing).


---
### `list_all_centroids`

`()`

`-> ListCentroidsResponse(centroids: list<string>)`

Lists the IDs of all centroids existing on the server.

This command has no error conditions.

---

### `list_centroid_range`

`(offset, count)`

`-> ListCentroidsResponse(centroids: list<string>)`

Lists a range of IDs of centroids existing on the server.  With keys sorted alphabetically, skips the first `offset` entries and returns up to `count` entries after that point.

This command has no error conditions.

---

### `list_centroid_range_from_id`

`(centroid_id, count)`

`-> ListCentroidsResponse(centroids: list<string>)`

Lists a range of IDs of centroids existing on the server.  With keys sorted alphabetically, skips to `centroid_id` and then continues to return up to `count` total entries.  This count includes the starting `centroid_id`.

A missing `centroid_id` is not an error condition for this command.  If `centroid_id` is not found, iteration will start from the first ID located after it alphabetically.

This command has no error conditions.

---

## Centroid-Document CRUD

### `add_document_to_centroid`

`(centroid_id, document_id, ignore_already_in_centroid=false)`

`-> AddDocumentsToCentroidResponse`

Associates a single document with a centroid.

Raises `ECentroidDoesNotExist` if `centroid_id` refers to a nonexistent centroid.

Raises `EDocumentDoesNotExist` if `document_id` refers to a nonexistent document.

Raises `EDocumentAlreadyInCentroid` the document is already in the centroid, unless `ignore_already_in_centroid=true` is passed.

If successful, the centroid is automatically recalculated after a cool-down interval.

---
### `add_documents_to_centroid`

`(centroid_id, document_ids, ignore_already_in_centroid=false)`

`-> AddDocumentsToCentroidResponse`

Associates multiple documents with a centroid.

Raises `ECentroidDoesNotExist` if `centroid_id` refers to a nonexistent centroid.

Raises `EDocumentDoesNotExist` if any of the documents in `document_ids` does not exist.

Raises `EDocumentAlreadyInCentroid` if any of the documents in `document_ids` is already in the centroid, unless `ignore_already_in_centroid=true` is passed.

If successful, the centroid is automatically recalculated after a cool-down interval.

---

### `remove_document_from_centroid`

`(centroid_id, document_id, ignore_not_in_centroid=false)`

`-> RemoveDocumentsFromCentroidResponse`

Unassociates a single document from a centroid.

Raises `ECentroidDoesNotExist` if `centroid_id` refers to a nonexistent centroid.

Raises `EDocumentDoesNotExist` if `document_id` refers to a nonexistent document.

Raises `EDocumentNotInCentroid` if the document is not currently associated with the centroid, unless `ignore_not_in_centroid=true` is passed.

If successful, the centroid is automatically recalculated after a cool-down interval.

---

### `remove_documents_from_centroid`

`(centroid_id, document_ids, ignore_not_in_centroid=false)`

`-> RemoveDocumentsFromCentroidResponse`

Unassociates multiple documents from a centroid.

Raises `ECentroidDoesNotExist` if `centroid_id` refers to a nonexistent centroid.

Raises `EDocumentDoesNotExist` if any of the documents in `document_ids` does not exist.

Raises `EDocumentNotInCentroid` if any of the documents in `document_ids` is not currently associated with the centroid, unless `ignore_not_in_centroid=true` is passed.

If successful, the centroid is automatically recalculated after a cool-down interval.

---

### `list_all_documents_for_centroid`

`(centroid_id)`

`-> ListCentroidDocumentsResponse(documents: list<string>)`

Lists IDs of all documents associated with the given centroid.

Raises `ECentroidDoesNotExist` if `centroid_id` refers to a nonexistent centroid.

---
### `list_centroid_document_range`

`(centroid_id, offset, count)`

`-> ListCentroidDocumentsResponse(documents: list<string>)`

Lists a range of IDs of documents associated with the given centroid.  With keys sorted alphabetically, skips the first `offset` entries and returns up to `count` entries after that point.

Raises `ECentroidDoesNotExist` if `centroid_id` refers to a nonexistent centroid.

---
### `list_centroid_document_range_from_id`

`(centroid_id, document_id, count)`

`-> ListCentroidDocumentsResponse(documents: list<string>)`



Lists a range of IDs of documents associated with the given centroid.  With keys sorted alphabetically, skips to `document_id` and then continues to return up to `count` total entries.  This count includes the starting `document_id`.

A missing `document_id` is not an error condition for this command.  If `document_id` is not found, iteration will start from the ID of the next associated document located after it alphabetically.

Raises `ECentroidDoesNotExist` if `centroid_id` refers to a nonexistent centroid.

---

## Synchronization
### `join_centroid`

`(centroid_id)`

`-> JoinCentroidResponse(id: string, recalculated: bool)`

Ensures a centroid is up to date before proceeding.
The server automatically recalculates centroids as documents are added and removed.  `join_centroid` provides a way to synchronize with that process.  The specific behavior is:

- If the centroid is up to date, returns immediately.
- If an update job is currently in progress, returns once that job has completed.
- If the centroid is not up to date and no job is executing (because of the cool-off period), immediately starts the update and returns once it is complete.

The `recalculated` property of the returned `JoinCentroidResponse` indicates whether a recalculation was actually performed.

Raises `ECentroidDoesNotExist` if `centroid_id` refers to a nonexistent centroid.

---
### `multi_join_centroids`

`(centroid_ids)`

`-> MultiJoinCentroidsResponse(ids: list<string>, recalculated: list<bool>)`

Like `join_centroid`, but runs against multiple centroids in a single request.

Raises `ECentroidDoesNotExist` if any of the `centroid_ids` refers to a nonexistent centroid.

