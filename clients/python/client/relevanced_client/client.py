from __future__ import print_function, absolute_import
from collections import defaultdict
from thrift.protocol import TBinaryProtocol
from thrift.transport import TSocket, TTransport
from relevanced_client.gen_py.RelevancedProtocol import Relevanced

from relevanced_client.gen_py.RelevancedProtocol.ttypes import StatusCode, Status
from relevanced_client import exceptions

def _get_exception_map():
    known = {
        StatusCode.OK: None,
        StatusCode.CENTROID_DOES_NOT_EXIST: exceptions.CentroidDoesNotExist,
        StatusCode.DOCUMENT_DOES_NOT_EXIST: exceptions.DocumentDoesNotExist,
        StatusCode.CENTROID_ALREADY_EXISTS: exceptions.CentroidAlreadyExists,
        StatusCode.DOCUMENT_ALREADY_EXISTS: exceptions.DocumentAlreadyExists,
        StatusCode.UNKNOWN_EXCEPTION: exceptions.UnexpectedResponse
    }
    mapping = defaultdict(lambda: exceptions.UnexpectedResponse)
    for k, v in known.iteritems():
        mapping[k] = v
    return mapping

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

    @property
    def _exception_map(self):
        # two underscores == twice as sneaky
        if not hasattr(self, '__exception_map'):
            self.__exception_map = _get_exception_map()
        return self.__exception_map

    def _handle_response_status(self, response):
        status = None
        if isinstance(response, Status):
            status = response
        else:
            if hasattr(response, 'status') and hasattr(response.status, 'code'):
                status = response.status
        if not status:
            return

        ErrType = self._exception_map[status.code]
        if ErrType:
            message = getattr(status, 'message', 'Exception')
            raise ErrType(message)

    def list_all_centroids(self):
        """
        Return a list of all centroid IDs defined on the server.
        """
        return self.thrift_client.listAllCentroids()

    def create_centroid(self, name):
        """
        Create a new centroid on the server.

        On success, returns the string `name`.

        If a centroid already exists with identifier `name`,
        raises `.exceptions.CentroidAlreadyExists`.
        """
        res = self.thrift_client.createCentroid(name)
        self._handle_response_status(res)
        return res.created

    def list_all_documents(self):
        """
        Return a list of all document IDs defined on the server.
        """
        return self.thrift_client.listAllDocuments()

    def add_document_to_centroid(self, centroid_id, document_id):
        """
        Add the document with id `document_id` to the centroid with
        id `centroid_id`.

        On success, returns `True`.

        If no centroid exists with id `centroid_id`, raises
        `.exceptions.CentroidDoesNotExist`.

        If no document exists with id `document_id`, raises
        `.exceptions.DocumentDoesNotExist`.
        """
        res = self.thrift_client.addDocumentToCentroid(
            centroid_id, document_id
        )
        self._handle_response_status(res)
        return True

    def remove_document_from_centroid(self, centroid_id, document_id):
        """
        Remove the document with id `document_id` from the centroid with
        id `centroid_id`.

        On success, returns `True`.

        If no centroid exists with id `centroid_id`, raises
        `.exceptions.CentroidDoesNotExist`.

        If no document exists with id `document_id`, raises
        `.exceptions.DocumentDoesNotExist`.
        """

        res = self.thrift_client.removeDocumentFromCentroid(
            centroid_id, document_id
        )
        self._handle_response_status(res)
        return True

    def create_document_with_id(self, document_id, document_text):
        """
        Create a new document on the server with ID `document_id`
        and text `document_text`.

        On success, returns `document_id`.

        If a document already exists with the given ID, raises
        `.exceptions.DocumentAlreadyExists`.
        """

        res = self.thrift_client.createDocumentWithID(
            document_id.encode('utf-8'),
            document_text.encode('utf-8')
        )
        self._handle_response_status(res)
        return res.created

    def create_document(self, document_text):
        """
        Create a new document with text `document_text`. The server
        will generate a UUID for the new document's ID.

        Returns the UUID of the new document.
        """
        res = self.thrift_client.createDocument(document_text.encode('utf-8'))
        self._handle_response_status(res)
        return res.created

    def delete_document(self, document_id):
        """
        Delete the document with id = `document_id`.
        This is not reversible.

        On success, returns `True`.

        The document is automatically removed from any
        centroids it has been added to.

        If no document exists with the given ID, raises
        `.exceptions.DocumentDoesNotExist`.
        """
        res = self.thrift_client.deleteDocument(document_id)
        self._handle_response_status(res)
        return True

    def delete_centroid(self, centroid_id):
        """
        Delete the centroid with id = `centroid_id`.
        This is not reversible.

        On success, returns `True`.

        Does not delete any documents contained in
        the centroid, but does remove any record of
        which documents it was associated with.

        If no centroid exists with the given ID, raises
        `.exceptions.CentroidDoesNotExist`.
        """
        res = self.thrift_client.deleteCentroid(centroid_id)
        self._handle_response_status(res)
        return True

    def recompute_centroid(self, centroid_id):
        """
        Force a recomputation of the centroid with id = `centroid_id`.

        If no centroid exists with the given ID, raises
        `.exceptions.CentroidDoesNotExist`.

        This command is not necessary under ordinary circumstances,
        as the server automatically recalculates centroids
        as documents are added and removed.  It is mainly used for testing.

        This call does not return until recomputation has completed
        on the server side, so it can potentially block for seconds to
        tens of seconds. (Up to a minute or two, for very large centroids).
        """
        res = self.thrift_client.recomputeCentroid(centroid_id)
        self._handle_response_status(res)
        return centroid_id

    def list_all_documents_for_centroid(self, centroid_id):
        """
        Return a list of IDs of all documents associated with the centroid
        `centroid_id`.

        If no centroid exists with the given ID, raises
        `.exceptions.CentroidDoesNotExist`.

        If the centroid exists but no documents have been added to it,
        returns an empty list. (I.e. this is not considered an error condition).
        """
        res = self.thrift_client.listAllDocumentsForCentroid(centroid_id)
        self._handle_response_status(res)
        return res.documents

    def get_centroid_similarity(self, centroid_1_id, centroid_2_id):
        """
        Return the cosine similarity score of `centroid_1_id` against
        `centroid_2_id`, as double-precision floating point.

        If either centroid does not exist, raises
        `.exceptions.CentroidDoesNotExist`.
        """

        res = self.thrift_client.getCentroidSimilarity(centroid_1_id, centroid_2_id)
        self._handle_response_status(res)
        return res.similarity

    def get_text_similarity(self, centroid_id, text):
        """
        Return cosine similarity of raw text `text` against the centroid
        given by `centroid_id`, as double-precision floating point.

        The server must convert `text` into a term-frequency vector
        to perform this comparison.  For cases where the text has
        already been added to the server as a document, prefer
        `get_document_similarity` to avoid this extra work.

        If the centroid does not exist, raises
        `.exceptions.CentroidDoesNotExist`.
        """
        res = self.thrift_client.getTextSimilarity(
            centroid_id, text.encode('utf-8')
        )
        self._handle_response_status(res)
        return res.similarity

    def multi_get_text_similarity(self, centroid_ids, text):
        """
        Return a dict mapping each of the centroids in the list
        `centroid_ids` to its cosine similarity against `text`.

        If any of the centroids do not exist, raises
        `.exceptions.CentroidDoesNotExist`.

        Example:
            scores = client.multi_get_text_similarity(
                ['centroid1', 'centroid2'],
                'this is some text'
            )
            pprint(scores)
            # {"centroid1": 0.08731412, "centroid2": 0.3921579}
        """

        if isinstance(centroid_ids, basestring):
            centroid_ids = [centroid_ids]

        res = self.thrift_client.multiGetTextSimilarity(
            centroid_ids, text.encode('utf-8')
        )
        self._handle_response_status(res)
        return res.scores

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
        `.exceptions.CentroidDoesNotExist`.

        If the document does not exist, raises
        `.exceptions.DocumentDoesNotExist`.
        """

        res = self.thrift_client.getDocumentSimilarity(
            centroid_id, document_id
        )
        self._handle_response_status(res)
        return res.similarity

