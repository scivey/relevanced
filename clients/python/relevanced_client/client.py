from __future__ import print_function
import heapq
from collections import defaultdict
from thrift import Thrift
from thrift.protocol import TBinaryProtocol
from thrift.transport import TSocket, TTransport
from .gen_py.RelevancedProtocol import Relevanced

from .gen_py.RelevancedProtocol.ttypes import StatusCode, Status
from . import exceptions

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
    def exception_map(self):
        if not hasattr(self, '_exception_map'):
            self._exception_map = _get_exception_map()
        return self._exception_map

    def _handle_response_status(self, response):
        status = None
        if isinstance(response, Status):
            status = response
        else:
            if hasattr(response, 'status') and hasattr(response.status, 'code'):
                status = response.status
        if not status:
            return

        ErrType = self.exception_map[status.code]
        if ErrType:
            message = getattr(status, 'message', 'Exception')
            raise ErrType(message)

    def list_all_centroids(self):
        return self.thrift_client.listAllCentroids()

    def create_centroid(self, name):
        res = self.thrift_client.createCentroid(name)
        self._handle_response_status(res)
        return res.created

    def list_all_documents(self):
        return self.thrift_client.listAllDocuments()

    def add_document_to_centroid(self, centroid_id, doc_id):
        res = self.thrift_client.addDocumentToCentroid(
            centroid_id, doc_id
        )
        self._handle_response_status(res)
        return True

    def remove_document_from_centroid(self, centroid_id, doc_id):
        res = self.thrift_client.removeDocumentFromCentroid(
            centroid_id, doc_id
        )
        self._handle_response_status(res)
        return True

    def create_document_with_id(self, ident, doc_text):
        res = self.thrift_client.createDocumentWithID(
            ident.encode('utf-8'),
            doc_text.encode('utf-8')
        )
        self._handle_response_status(res)
        return res.created

    def create_document(self, doc_text):
        res = self.thrift_client.createDocument(doc_text.encode('utf-8'))
        self._handle_response_status(res)
        return res.created

    def get_document(self, doc_id):
        return self.thrift_client.getDocument(doc_id)

    def delete_document(self, doc_id):
        res = self.thrift_client.deleteDocument(doc_id)
        self._handle_response_status(res)
        return response.created

    def delete_centroid(self, centroid_id):
        res = self.thrift_client.deleteCentroid(centroid_id)
        self._handle_response_status(res)
        return res.created

    def recompute_centroid(self, centroid_id):
        res = self.thrift_client.recomputeCentroid(centroid_id)
        self._handle_response_status(res)
        return centroid_id

    def list_all_documents_for_centroid(self, centroid_id):
        res = self.thrift_client.listAllDocumentsForCentroid(centroid_id)
        self._handle_response_status(res)
        return res.documents

    def get_centroid_similarity(self, centroid_1_id, centroid_2_id):
        res = self.thrift_client.getCentroidSimilarity(centroid_1_id, centroid_2_id)
        self._handle_response_status(res)
        return res.similarity

    def get_text_similarity(self, centroid_id, text):
        res = self.thrift_client.getTextSimilarity(
            centroid_id, text.encode('utf-8')
        )
        self._handle_response_status(res)
        return res.similarity

    def multi_get_text_similarity(self, centroid_ids, text):
        assert(not isinstance(centroid_ids, basestring))
        res = self.thrift_client.multiGetTextSimilarity(
            centroid_ids, text.encode('utf-8')
        )
        self._handle_response_status(res)
        return res.scores

    def get_document_similarity(self, centroid_id, doc_id):
        res = self.thrift_client.getDocumentSimilarity(
            centroid_id, doc_id
        )
        self._handle_response_status(res)
        return res.similarity

