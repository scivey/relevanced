## Commands (language-agnostic)

Some details of the client commands vary between languages.  This is especially true for exception types, synchronous vs. asynchronous response patterns, and camel- vs. snake-casing.  You can find these differences detailed in the language-specific client pages.

Aside from those details, the fundamental commands for interfacing with **relevanced** are the same across all clients.  They're described below.

### CRUD commands
These commands create or delete centroids or documents, or associate or unassociating particular documents with particular centroids.

#### `createDocument`
Takes: `text: string` 

Send a blob of unprocessed text to the server, which will process and persist it.  This command returns the UUID assigned to the processed document, which you need for further commands involving that document.

#### `createDocumentWithID`
Takes: `id: string`, `text: string`

Like `createDocument` but also specifies an ID to use for the document, which the server will use instead of generating a UUID.  Because document IDs must be unique, calling `createDocumentWithID` with an existing ID is an error.  The server reports this condition via the `DOCUMENT_ALREADY_EXISTS` status code in its underlying Thrift protocol; the higher-level clients indicate this condition in ways that integrate better with their host languages.  (The Python client throws `DocumentAlreadyExists`; the Javascript client returns a failed promise containing a similar error.)

#### `deleteDocument`
Takes: `id: string`
If a document with `id` exists, deletes it and returns status code `OK` (Thrift protocol) or a variant of `True` or a successful promise (higher-level clients).

If the document does not exist, returns status code `DOCUMENT_DOES_NOT_EXIST` (Thrift protocol) or throws an exception / returns a failed promise (higher-level clients).

If the document is associated with any centroids, those associations are automatically invalidated.  Most of this invalidation work (e.g. removing `RocksDB` entries) happens lazily during centroid recomputations.


#### `createCentroid`
Takes: `id: string`

Creates a new centroid.  `id` must be unique among all other centroids on the server.

If the centroid is successfully created, returns status code `OK` (Thrift protocol) or a variant of `True` or a successful promise (higher-level clients).

If `id` refers to an existing centroid, returns status code `CENTROID_ALREADY_EXISTS` (Thrift protocol) or throws an exception / returns a failed promise (higher-level clients).


#### `deleteCentroid`
Takes: `id: string`

Deletes a centroid.  `id` must refer to the id of a centroid that exists on the server.

If the centroid is successfully deleted, returns status code `OK` (Thrift protocol) or a variant of `True` or a successful promise (higher-level clients).

If `id` does not refer to any centroid on the server, returns status code `CENTROID_DOES_NOT_EXIST` (Thrift protocol) or throws an exception / returns a failed promise (higher-level clients).

If the centroid is associated with any documents, the related centroid->document links will be invalidated and removed from the database lazily.

Because documents in **relevanced** are independent entities which can be added to many centroids, deleting a given centroid does *not* delete any of the documents it is associated with.  It only results in deletion of database entries describing the links.

#### `addDocumentToCentroid`
Takes: `centroidId: string`, `documentId: string`
Associates a document with a centroid.

If the centroid and document both exist, returns status code `OK` (Thrift protocol) or a variant of `True` or a successful promise (higher-level clients).

If this command is successful, it initiates a background recomputation of the affected centroid.  This background job is intelligently scheduled with debounce and cool-off intervals, so making `N` calls to `addDocumentToCentroid` with the same `centroidId` will not result in `N` update jobs running on the server: it will result in one or a small number, depending on the exact calling pattern.  Once the centroid has been updated, its position will include the influence of the newly linked document.

If the centroid does not exist, the Thrift protocol returns status code `CENTROID_DOES_NOT_EXIST`.  If the document does not exist, returns status code `DOCUMENT_DOES_NOT_EXIST`.  Higher-level clients report either status by throwing an exception or returning a failed promise, depending on host language.

Note that if both the centroid and document do not exist, only the `CENTROID_DOES_NOT_EXIST` error condition is reported.

#### `removeDocumentFromCentroid`
Takes: `centroidId: string`, `documentId: string`
Unassociates a document from a centroid.

If the centroid and document both exist and they are currently linked, returns status code `OK` (Thrift protocol) or a variant of `True` or a successful promise (higher-level clients).

If this command is successful, it initiates a background recomputation of the affected centroid.  This background job is intelligently scheduled with debounce and cool-off intervals, so making `N` calls to `addDocumentToCentroid` with the same `centroidId` will not result in `N` update jobs running on the server: it will result in one or a small number, depending on the exact calling pattern.  Once the centroid has been updated, its position will no longer be influenced by that unlinked document.

If the centroid does not exist, the Thrift protocol returns status code `CENTROID_DOES_NOT_EXIST`.  If the document does not exist, returns status code `DOCUMENT_DOES_NOT_EXIST`.  Higher-level clients report either status by throwing an exception or returning a failed promise, depending on host language.

Note that if both the centroid and document do not exist, only the `CENTROID_DOES_NOT_EXIST` error condition is reported.

#### `listAllCentroids`
Takes: (no arguments)

Returns a list of all centroid IDs currently defined on the server.
This command has no error conditions.


#### `listAllDocuments`
Takes: (no arguments)

Returns a list of the IDs of all documents existing on the server.
This command has no error conditions.

#### `listAllDocumentsForCentroid`
# TODO


### Similarity scoring commands
Commands for comparing the similarities of documents against centroids, text against centroids, centroids against centroids.

#### `getSimilarityForText`
Takes: `centroidId: string`, `text: string`
Transforms `text` into a normalized term-frequency vector on the server, and then computes its similarity against the given centroid.

If successful, higher-level clients return the floating point cosine similarity score.  The underlying Thrift protocol returns this score in addition to the status code `OK`.



