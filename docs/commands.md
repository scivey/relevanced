# Commands (language-agnostic)

Some details of the client commands vary between languages.  This is especially true for exception types, synchronous vs. asynchronous response patterns, and camel- vs. snake-casing.  You can find these differences detailed in the language-specific client pages.

Aside from those details, the fundamental commands for interfacing with **relevanced** are the same across all clients.  They're described below.

---

## `getTextSimilarity`
#### `(centroidId: string, text: string) -> double`
Transforms `text` into a normalized term-frequency vector on the server, and then computes its similarity against the given centroid.

If successful, higher-level clients return the cosine similarity score as double-precision floating point.  The underlying Thrift protocol returns this score in addition to the status code `OK`.

If there is no centroid matching `centroidId`, the protocol returns status code `CENTROID_DOES_NOT_EXIST`.  Higher-level clients throw an error or return a failed promise (depending on language).

---
## `multiGetTextSimilarity`
#### `(centroidIds: list<string>, text: string) -> map<string, double>`
Transforms `text` into a normalized term-frequency vector on the server, and then computes its similarity against each of the centroids specified by `centroidIds`.

If successful, higher-level clients return a language-appropriate `Map<string, double>` type which maps centroid ids to their double precision floating point cosine similarity score against the document.  The underlying Thrift protocol returns this map of scores in addition to the status code `OK`.

If any of the centroids specified by `centroidIds` do not exist, the entire command fails.  The protocol indicates this with status code `CENTROID_DOES_NOT_EXIST`; higher-level clients throw an error or return a failed promise.

Note that the protocol does not distinguish between cases where a single centroid is missing and those where multiple centroids are missing.  These cases are all indicated by the status code `CENTROID_DOES_NOT_EXIST`.

---
## `getDocumentSimilarity`
#### `(centroidId: string, documentId: string) -> double`
Computes cosine similarity of a document that *already exists on the server* against the specified centroid.

This command has the same purpose as `getSimilarityForText`, but is much more efficient in cases where the text has already been sent to the server for another reason.  In these cases the server can skip the text-processing phase.

If successful, higher-level clients return the cosine similarity score as double-precision floating point.  The underlying Thrift protocol returns this score in addition to the status code `OK`.

If there is no centroid matching `centroidId`, the protocol returns status code `CENTROID_DOES_NOT_EXIST`.  If there is no document matching `documentId`, it returns status code `DOCUMENT_DOES_NOT_EXIST`.  In either case, higher-level clients throw an error or return a failed promise (depending on language).

Note that if both the centroid and document do not exist, only the `CENTROID_DOES_NOT_EXIST` error condition will be reported.

---
## `getCentroidSimilarity`
#### `(centroidId1: string, centroidId2: string) -> double`

Computes cosine similarity of two centroids.

If successful, higher-level clients return the cosine similarity score as double-precision floating point.  The underlying Thrift protocol returns this score in addition to the status code `OK`.

If either centroid is missing, the protocol returns status code `CENTROID_DOES_NOT_EXIST` and higher-level clients throw an error or return a failed promise (depending on language).

Note that the protocol does not distinguish between cases where both centroids are missing and those where only one is missing.  In either case, the status is `CENTROID_DOES_NOT_EXIST`.

---
## `createDocument`
#### `(text: string) -> string`

Send a blob of unprocessed text to the server, which will process and persist it.  This command returns the UUID assigned to the processed document, which is needed for further commands involving that document.

---
## `createDocumentWithID`
#### `(id: string, text: string) -> string`

Like `createDocument` but also specifies an ID to use for the document, which the server will use instead of generating a UUID.

On success, returns a string equal to `id`.

Because document IDs must be unique, calling `createDocumentWithID` with an existing ID is an error.  The server reports this condition via the `DOCUMENT_ALREADY_EXISTS` status code in its underlying Thrift protocol; the higher-level clients indicate this condition in ways that integrate better with their host languages.  (The Python client throws `DocumentAlreadyExists`; the Javascript client returns a failed promise containing a similar error.)

---
## `deleteDocument`
#### `(id: string) -> bool`
If a document with `id` exists, deletes it and returns status code `OK` (Thrift protocol) or a variant of `True` or a successful promise (higher-level clients).

If the document does not exist, returns status code `DOCUMENT_DOES_NOT_EXIST` (Thrift protocol) or throws an exception / returns a failed promise (higher-level clients).

If the document is associated with any centroids, those associations are automatically invalidated.  Most of this invalidation work (e.g. removing `RocksDB` entries) happens lazily during centroid recomputations.

---
## `createCentroid`
#### `(id: string) -> string`

Creates a new centroid.  `id` must be unique among all centroids on the server.

If the centroid is successfully created, returns status code `OK` (Thrift protocol) or a string equal to `id` (higher-level clients).

If `id` refers to an existing centroid, returns status code `CENTROID_ALREADY_EXISTS` (Thrift protocol) or throws an exception / returns a failed promise (higher-level clients).

---
## `deleteCentroid`
#### `(id: string) -> bool`

Deletes a centroid.  `id` must refer to the id of a centroid that exists on the server.

If the centroid is successfully deleted, returns status code `OK` (Thrift protocol) or a variant of `True` or a successful promise (higher-level clients).

If `id` does not refer to any centroid on the server, returns status code `CENTROID_DOES_NOT_EXIST` (Thrift protocol) or throws an exception / returns a failed promise (higher-level clients).

If the centroid is associated with any documents, the related centroid->document links will be invalidated and removed from the database lazily.

Because documents in **relevanced** are independent entities which can be added to many centroids, deleting a given centroid does *not* delete any of the documents it is associated with.  It only results in deletion of database entries describing the links.

---
## `addDocumentToCentroid`
#### `(centroidId: string, documentId: string) -> void`
Associates a document with a centroid.

If the centroid and document both exist, returns status code `OK` (Thrift protocol) or a variant of `True` or a successful promise (higher-level clients).

If this command is successful, it initiates a background recomputation of the affected centroid.  This background job is intelligently scheduled with debounce and cool-off intervals, so making `N` calls to `addDocumentToCentroid` with the same `centroidId` will not result in `N` update jobs running on the server: it will result in one or a small number, depending on the exact calling pattern.  Once the centroid has been updated, its position will include the influence of the newly linked document.

If the centroid does not exist, the Thrift protocol returns status code `CENTROID_DOES_NOT_EXIST`.  If the document does not exist, returns status code `DOCUMENT_DOES_NOT_EXIST`.  Higher-level clients report either status by throwing an exception or returning a failed promise, depending on host language.

Note that if both the centroid and document do not exist, only the `CENTROID_DOES_NOT_EXIST` error condition is reported.

---
## `removeDocumentFromCentroid`
#### `(centroidId: string, documentId: string) -> void`
Unassociates a document from a centroid.

If the centroid and document both exist and they are currently linked, returns status code `OK` (Thrift protocol) or a variant of `True` or a successful promise (higher-level clients).

If this command is successful, it initiates a background recomputation of the affected centroid.  This background job is intelligently scheduled with debounce and cool-off intervals, so making `N` calls to `addDocumentToCentroid` with the same `centroidId` will not result in `N` update jobs running on the server: it will result in one or a small number, depending on the exact calling pattern.  Once the centroid has been updated, its position will no longer be influenced by that unlinked document.

If the centroid does not exist, the Thrift protocol returns status code `CENTROID_DOES_NOT_EXIST`.  If the document does not exist, returns status code `DOCUMENT_DOES_NOT_EXIST`.  Higher-level clients report either status by throwing an exception or returning a failed promise, depending on host language.

Note that if both the centroid and document do not exist, only the `CENTROID_DOES_NOT_EXIST` error condition is reported.

---
## `listAllCentroids`
#### `() -> list<string>`

Returns a list of all centroid IDs currently defined on the server.
This command has no error conditions.

---
## `listAllDocuments`
#### `() -> list<string>`

Returns a list of the IDs of all documents existing on the server.
This command has no error conditions.

---
## `listAllDocumentsForCentroid`
#### `(centroidId: string) -> list<string>`

Returns a list of all document IDs associated with the given `centroidId`.

Higher-level clients receive this list directly (possibly asynchronously); the underlying Thrift protocol returns this list together with a status.

When successful, higher-level clients receive the list of document IDs directly (possible asynchronously).  The underlying Thrift protocol returns a status code alongside this list, which is set to `OK` on success.

If there is no centroid matching `centroidId`, the protocol returns status code `CENTROID_DOES_NOT_EXIST`.  Higher-level clients throw an error or return a failed promise (depending on language).

If the centroid exists but there are no documents assigned to it, the result is just an empty list (i.e. this scenario is not considered an error).

