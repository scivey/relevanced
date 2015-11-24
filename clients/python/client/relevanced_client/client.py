from __future__ import print_function, absolute_import
from collections import defaultdict
from thrift.protocol import TBinaryProtocol
from thrift.transport import TSocket, TTransport
from relevanced_client.gen_py.RelevancedProtocol import Relevanced
from relevanced_client.gen_py.RelevancedProtocol.ttypes import (
    CreateCentroidRequest,
    DeleteDocumentRequest,
    DeleteCentroidRequest,
    MultiDeleteCentroidsRequest,
    MultiDeleteDocumentsRequest,
    MultiCreateCentroidsRequest,
    JoinCentroidRequest,
    MultiJoinCentroidsRequest,
    AddDocumentsToCentroidRequest,
    RemoveDocumentsFromCentroidRequest,
    Language
)

class Client(object):
    def __init__(self, host, port):
        self.host = host
        self.port = int(port)

    @property
    def thrift_client(self):
        if not hasattr(self, '_thrift_client'):
            sock = TSocket.TSocket(self.host, self.port)
            transport = TTransport.TBufferedTransport(sock)
            protocol = TBinaryProtocol.TBinaryProtocol(transport)
            self._thrift_client = Relevanced.Client(protocol)
            transport.open()
        return self._thrift_client

    def list_all_centroids(self):
        """
        List the IDs of all centroids defined on the server.

        Returns a `ListCentroidsResponse`.  The `centroids`
        property of this response object contains an array
        of IDs.
        """
        return self.thrift_client.listAllCentroids()

    def list_centroid_range(self, offset, count):
        """
        Returns up to `count` ids of centroids existing
        on the server, skipping the first `offset` ordered
        ids.

        Returns a `ListCentroidsResponse`.  The `centroids`
        property of this response object contains an array
        of IDs.
        """
        return self.thrift_client.listCentroidRange(offset, count)

    def list_centroid_range_from_id(self, centroid_id, count):
        """
        Returns up to `count` ids of centroids existing
        on the server, starting from `centroid_id`.
        If `centroid_id` does not exist, the returned list
        will start at the next largest centroid id after
        `centroid_id`.

        Returns a `ListCentroidsResponse`.  The `centroids`
        property of this response object contains an array
        of IDs.
        """
        return self.thrift_client.listCentroidRangeFromID(centroid_id, count)

    def create_centroid(self, id, ignore_existing=False):
        """
        Create a new centroid on the server.

        On success, returns a `CreateCentroidResponse`.
        The response's ID equals the original
        ID specified.

        If a centroid already exists with ID `id`,
        raises `ECentroidAlreadyExists`.

        If `ignore_existing` is True, this exception
        will not be raised.
        """
        request = CreateCentroidRequest()
        request.id = id
        request.ignoreExisting = ignore_existing
        return self.thrift_client.createCentroid(request)

    def multi_create_centroids(self, ids, ignore_existing=False):
        """
        Create multiple new centroids on the server.

        On success, returns a `MultiCreateCentroidsResponse`.
        The `created` property of this object is an array
        containing the IDs of all centroids created.

        If a centroid already exists with one of the IDs in
        `ids`, raises an `ECentroidAlreadyExists` exception
        unless `ignore_existing=True` is passed.
        """
        request = MultiCreateCentroidsRequest()
        request.ids = ids
        request.ignoreExisting = ignore_existing
        return self.thrift_client.multiCreateCentroids(request)

    def list_all_documents(self):
        """
        List the IDs of all documents defined on the server.

        Returns a `ListDocumentsResponse`.  The `documents`
        property of this response object contains an array
        of IDs.
        """
        return self.thrift_client.listAllDocuments()

    def list_document_range(self, offset, count):
        """
        Returns up to `count` ids of documents existing on
        the server, skipping the first `offset` ids.

        Returns a `ListDocumentsResponse`.  The `documents`
        property of this response object contains an array
        of IDs.
        """
        return self.thrift_client.listDocumentRange(offset, count)

    def list_document_range_from_id(self, document_id, count):
        """
        Returns up to `count` ids of documents existing on
        the server, starting from `document_id`.
        If `document_id` does not exist, the returned list
        will start at the next largest document id after
        `document_id`.

        Returns a `ListDocumentsResponse`.  The `documents`
        property of this response object contains an array
        of IDs.
        """
        return self.thrift_client.listDocumentRangeFromID(document_id, count)

    def add_documents_to_centroid(self, centroid_id, document_ids, ignore_missing_document=False, ignore_already_in_centroid=False):
        request = AddDocumentsToCentroidRequest()
        request.centroidId = centroid_id
        request.documentIds = document_ids
        request.ignoreMissingDocument = ignore_missing_document
        request.ignoreAlreadyInCentroid = ignore_already_in_centroid
        return self.thrift_client.addDocumentsToCentroid(request)

    def add_document_to_centroid(self, centroid_id, document_id, ignore_missing_document=False, ignore_already_in_centroid=False):
        """
        Add the document with id `document_id` to the centroid with
        id `centroid_id`.

        Returns an `AddDocumentToCentroidResponse` with
        corresponding `centroidId` and `documentId` properties.

        If no centroid exists with id `centroid_id`, raises
        `ECentroidDoesNotExist`.

        If no document exists with id `document_id`, raises
        `EDocumentDoesNotExist`.

        If the document has already been added to the centroid,
        raises `EDocumentAlreadyInCentroid`.
        """
        return self.add_documents_to_centroid(
            centroid_id=centroid_id,
            document_ids=[document_id],
            ignore_missing_document=ignore_missing_document,
            ignore_already_in_centroid=ignore_already_in_centroid
        )

    def remove_documents_from_centroid(self, centroid_id, document_ids, ignore_missing_document=False, ignore_not_in_centroid=False):
        request = RemoveDocumentsFromCentroidRequest()
        request.centroidId = centroid_id
        request.documentIds = document_ids
        request.ignoreMissingDocument = ignore_missing_document
        request.ignoreNotInCentroid = ignore_not_in_centroid
        return self.thrift_client.removeDocumentsFromCentroid(request)

    def remove_document_from_centroid(self, centroid_id, document_id, ignore_missing_document=False, ignore_not_in_centroid=False):
        """
        Remove the document with id `document_id` from the centroid with
        id `centroid_id`.

        Returns a `RemoveDocumentFromCentroidResponse` with
        corresponding `centroidId` and `documentId` properties.

        If no centroid exists with id `centroid_id`, raises
        `ECentroidDoesNotExist`.

        If no document exists with id `document_id`, raises
        `EDocumentDoesNotExist`.

        If the document is not in the given centroid,
        raises `EDocumentNotInCentroid`.
        """
        return self.remove_documents_from_centroid(
            centroid_id=centroid_id,
            document_ids=[document_id],
            ignore_missing_document=ignore_missing_document,
            ignore_not_in_centroid=ignore_not_in_centroid
        )

    def create_document_with_id(self, document_id, document_text, lang=Language.EN):
        """
        Create a new document on the server with ID `document_id`
        and text `document_text`.

        Takes an optional `lang` argument which defaults to
        `Language.EN` (English).

        Returns a `CreateDocumentResponse` with `id` property
        equal to `document_id`.

        If a document already exists with the given ID, raises
        `EDocumentAlreadyExists`.
        """

        return self.thrift_client.createDocumentWithID(
            document_id.encode('utf-8'),
            document_text.encode('utf-8'),
            lang
        )

    def create_document(self, document_text, lang=Language.EN):
        """
        Create a new document with text `document_text`. The server
        will generate a UUID for the new document's ID.

        Takes an optional `lang` argument which defaults to
        `Language.EN` (English).

        Returns a `CreateDocumentResponse`.  Its `id` property
        gives the server-generated UUID of the new document.
        """
        return self.thrift_client.createDocument(
            document_text.encode('utf-8'), lang
        )

    def delete_document(self, document_id, ignore_missing=False):
        """
        Delete the document with id = `document_id`.
        This is not reversible.

        Returns a `DeleteDocumentResponse` with `id` property
        equal to `document_id`.

        The document is automatically removed from any
        centroids it has been added to.

        If no document exists with the given ID, raises
        `EDocumentDoesNotExist` unless `ignore_missing=True`
        was passed.
        """
        request = DeleteDocumentRequest()
        request.ignoreMissing = ignore_missing
        request.id = document_id
        return self.thrift_client.deleteDocument(request)

    def multi_delete_documents(self, document_ids, ignore_missing=False):
        request = MultiDeleteDocumentsRequest()
        request.ids = document_ids
        request.ignoreMissing = ignore_missing
        return self.thrift_client.multiDeleteDocuments(request)

    def delete_centroid(self, centroid_id, ignore_missing=False):
        """
        Delete the centroid with id = `centroid_id`.
        This is not reversible.

        Returns a `DeleteCentroidResponse` with `id` property
        equal to `centroid_id`.

        Does not delete any documents contained in
        the centroid, but does remove any record of
        which documents it was associated with.

        If no centroid exists with the given ID, raises
        `ECentroidDoesNotExist`.
        """
        request = DeleteCentroidRequest()
        request.id = centroid_id
        request.ignoreMissing = ignore_missing

        return self.thrift_client.deleteCentroid(request)

    def multi_delete_centroids(self, centroid_ids, ignore_missing=False):
        request = MultiDeleteCentroidsRequest()
        request.ids = centroid_ids
        request.ignoreMissing = ignore_missing
        return self.thrift_client.multiDeleteCentroids(request)

    def join_centroid(self, centroid_id, ignore_missing=False):
        """
        Synchronize with the server-side centroid
        recalculation cycle.

        - If centroid is up to date, returns.
        - If centroid is not up to date and an update is
        in progress, returns once that update is complete.
        - If centroid is not up to date and an update is not
        yet running due to the cool-down period, immediately
        performs the update and then returns.

        Returns a `JoinCentroidResponse`.  The `recalculated`
        property of this response object indicates whether
        a recalculation was actually performed.

        If no centroid exists with the given ID, raises
        `ECentroidDoesNotExist` unless `ignore_missing=True`
        is passed.

        This command is not necessary under most circumstances,
        as the server automatically recalculates centroids
        as documents are added and removed.  It mainly exists
        to aid development and testing.
        """
        request = JoinCentroidRequest()
        request.id = centroid_id
        request.ignoreMissing = ignore_missing
        return self.thrift_client.joinCentroid(request)

    def multi_join_centroids(self, centroid_ids, ignore_missing=False):
        """
        Synchronize with the server-side centroid
        recalculation cycle for multiple centroids.

        See the documentation on `join_centroid` for
        details.
        `multi_join_centroids` takes a list of centroid IDs
        instead of a single ID.
        """
        request = MultiJoinCentroidsRequest()
        request.ids = centroid_ids
        request.ignoreMissing = ignore_missing
        return self.thrift_client.multiJoinCentroids(request)

    def list_all_documents_for_centroid(self, centroid_id):
        """
        List the IDs of all documents associated with the
        centroid `centroid_id`.

        Returns a `ListCentroidDocumentsResponse`.  The
        `documents` property of this response object
        contains the IDs.

        If no centroid exists with the given ID, raises
        `ECentroidDoesNotExist`.

        If the centroid exists but is not associated with
        any documents, the response's `documents` property
        will be an empty list. (This is not considered
        an error condition.)
        """
        return self.thrift_client.listAllDocumentsForCentroid(centroid_id)

    def list_centroid_document_range(self, centroid_id, offset, count):
        """
        Lists up to `count` IDs of documents associated
        with the centroid `centroid_id`, skipping the
        first `offset` documents.

        Returns a `ListCentroidDocumentsResponse`.  The
        `documents` property of this response object
        contains the IDs.

        If no centroid exists with the given ID, raises
        `ECentroidDoesNotExist`.
        """
        return self.thrift_client.listCentroidDocumentRange(
            centroid_id, offset, count
        )

    def list_centroid_document_range_from_id(self, centroid_id, document_id, count):
        """
        Lists up to `count` IDs of documents associated
        with the centroid `centroid_id`, starting from
        `document_id`.

        If `document_id` does not exist or is not
        associated with `centroid_id`, the returned list
        will start at the next largest document id after
        `document_id`.

        Returns a `ListCentroidDocumentsResponse`.  The
        `documents` property of this response object
        contains the IDs.

        If no centroid exists with the given ID, raises
        `ECentroidDoesNotExist`.
        """
        return self.thrift_client.listCentroidDocumentRangeFromID(
            centroid_id, document_id, count
        )


    def get_centroid_similarity(self, centroid_1_id, centroid_2_id):
        """
        Return the cosine similarity score of `centroid_1_id` against
        `centroid_2_id`, as double-precision floating point.

        If either centroid does not exist, raises
        `ECentroidDoesNotExist`.
        """

        return self.thrift_client.getCentroidSimilarity(centroid_1_id, centroid_2_id)

    def get_text_similarity(self, centroid_id, text, lang=Language.EN):
        """
        Return cosine similarity of raw text `text` against the centroid
        given by `centroid_id`, as double-precision floating point.

        The server must convert `text` into a term-frequency vector
        to perform this comparison.  For cases where the text has
        already been added to the server as a document, prefer
        `get_document_similarity` to avoid this extra work.

        If the centroid does not exist, raises
        `ECentroidDoesNotExist`.
        """
        return self.thrift_client.getTextSimilarity(
            centroid_id, text.encode('utf-8'), lang
        )

    def multi_get_text_similarity(self, centroid_ids, text, lang=Language.EN):
        """
        Calculate the cosine similarity of raw text `text`
        against multiple centroids in parallel.

        Returns a `MultiSimilarityResponse`.  The `scores`
        property of this response object is a dict mapping
        each centroid ID to its corresponding cosine similarity
        against `text`.

        If any of the centroids do not exist, raises
        `ECentroidDoesNotExist`.

        Example:
            response = client.multi_get_text_similarity(
                ['centroid1', 'centroid2'],
                'this is some text'
            )
            pprint(response.scores)
            # {"centroid1": 0.08731412, "centroid2": 0.3921579}
        """

        if isinstance(centroid_ids, basestring):
            centroid_ids = [centroid_ids]

        return self.thrift_client.multiGetTextSimilarity(
            centroid_ids, text.encode('utf-8'), lang
        )

    def multi_get_document_similarity(self, centroid_ids, document_id):
        """
        Calculate the cosine similarity of document with id
        `document_id` against multiple centroids in parallel.

        The document must already exist on the server.

        Returns a `MultiSimilarityResponse`.  The `scores`
        property of this response object is a dict mapping
        each centroid ID to its corresponding cosine similarity
        against `text`.

        If the document does not exist, raises
        `EDocumentDoesNotExist`.

        If any of the centroids do not exist, raises
        `ECentroidDoesNotExist`.

        Example:
            response = client.multi_get_document_similarity(
                ['centroid1', 'centroid2'],
                'some-document-id'
            )
            pprint(response.scores)
            # {"centroid1": 0.08731412, "centroid2": 0.3921579}
        """

        if isinstance(centroid_ids, basestring):
            centroid_ids = [centroid_ids]

        return self.thrift_client.multiGetDocumentSimilarity(
            centroid_ids, text.encode('utf-8')
        )

    def get_document_similarity(self, centroid_id, document_id):
        """
        Return cosine similarity of document with ID `document_id`
        against the centroid given by `centroid_id`, as
        double-precision floating point.

        Because the server can use its cached term-frequency vector
        representing `document_id` for the comparison, this command
        should be preferred over `get_text_similarity` for cases
        where the document has already been saved on the server.

        If the centroid does not exist, raises
        `ECentroidDoesNotExist`.

        If the document does not exist, raises
        `EDocumentDoesNotExist`.
        """

        return self.thrift_client.getDocumentSimilarity(
            centroid_id, document_id
        )

    def get_server_metadata(self):
        """
        Return server metadata.  Includes release version,
        git revision, build timestamp, etc.
        """
        return self.thrift_client.getServerMetadata()

    def list_unused_documents(self, limit):
        """
        Return a list of up to `limit` IDs of documents
        which are not in any centroid.
        """
        return self.thrift_client.listUnusedDocuments(limit)
