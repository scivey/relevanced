from __future__ import print_function, absolute_import
from collections import defaultdict
from thrift.protocol import TBinaryProtocol
from thrift.transport import TSocket, TTransport
from relevanced_client.gen_py.RelevancedProtocol import Relevanced

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

    def create_centroid(self, id):
        """
        Create a new centroid on the server.

        On success, returns a `CreateCentroidResponse`.
        The response's ID equals the original
        ID specified.

        If a centroid already exists with ID `id`,
        raises `ECentroidAlreadyExists`.
        """
        return self.thrift_client.createCentroid(id)

    def list_all_documents(self):
        """
        List the IDs of all documents defined on the server.

        Returns a `ListDocumentsResponse`.  The `documents`
        property of this response object contains an array
        of IDs.
        """
        return self.thrift_client.listAllDocuments()

    def add_document_to_centroid(self, centroid_id, document_id):
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
        return self.thrift_client.addDocumentToCentroid(
            centroid_id, document_id
        )

    def remove_document_from_centroid(self, centroid_id, document_id):
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
        return self.thrift_client.removeDocumentFromCentroid(
            centroid_id, document_id
        )

    def create_document_with_id(self, document_id, document_text):
        """
        Create a new document on the server with ID `document_id`
        and text `document_text`.

        Returns a `CreateDocumentResponse` with `id` property
        equal to `document_id`.

        If a document already exists with the given ID, raises
        `EDocumentAlreadyExists`.
        """

        return self.thrift_client.createDocumentWithID(
            document_id.encode('utf-8'),
            document_text.encode('utf-8')
        )

    def create_document(self, document_text):
        """
        Create a new document with text `document_text`. The server
        will generate a UUID for the new document's ID.

        Returns a `CreateDocumentResponse`.  Its `id` property
        gives the server-generated UUID of the new document.
        """
        return self.thrift_client.createDocument(document_text.encode('utf-8'))

    def delete_document(self, document_id):
        """
        Delete the document with id = `document_id`.
        This is not reversible.

        Returns a `DeleteDocumentResponse` with `id` property
        equal to `document_id`.

        The document is automatically removed from any
        centroids it has been added to.

        If no document exists with the given ID, raises
        `EDocumentDoesNotExist`.
        """
        return self.thrift_client.deleteDocument(document_id)

    def delete_centroid(self, centroid_id):
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
        return self.thrift_client.deleteCentroid(centroid_id)

    def join_centroid(self, centroid_id):
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
        `ECentroidDoesNotExist`.

        This command is not necessary under most circumstances,
        as the server automatically recalculates centroids
        as documents are added and removed.  It mainly exists
        to aid development and testing.
        """
        return self.thrift_client.joinCentroid(centroid_id)

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

    def get_centroid_similarity(self, centroid_1_id, centroid_2_id):
        """
        Return the cosine similarity score of `centroid_1_id` against
        `centroid_2_id`, as double-precision floating point.

        If either centroid does not exist, raises
        `ECentroidDoesNotExist`.
        """

        return self.thrift_client.getCentroidSimilarity(centroid_1_id, centroid_2_id)

    def get_text_similarity(self, centroid_id, text):
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
            centroid_id, text.encode('utf-8')
        )

    def multi_get_text_similarity(self, centroid_ids, text):
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
