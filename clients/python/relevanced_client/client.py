from __future__ import print_function
from thrift import Thrift
from thrift.protocol import TBinaryProtocol
from thrift.transport import TSocket, TTransport
from .gen_py.TextRelevance import Relevance
from .gen_py.TextRelevance.ttypes import RelevanceStatus
from . import exceptions

def raise_unexpected(response_code):
    err_name = RelevanceStatus._VALUES_TO_NAME.get(response_code, 'UNKNOWN')
    msg = "UnexpectedResponse: [%i]: '%s'" % (response_code, err_name)
    raise exceptions.UnexpectedResponse(msg)

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
            self._thrift_client = Relevance.Client(protocol)
            transport.open()
        return self._thrift_client

    def list_all_centroids(self):
        return self.thrift_client.listAllCentroids()

    def create_centroid(self, name):
        res = self.thrift_client.createCentroid(name)
        if res.status != RelevanceStatus.OK:
            if res.status == RelevanceStatus.CENTROID_ALREADY_EXISTS:
                raise exceptions.CentroidAlreadyExists(name)
            raise_unexpected(res.status)
        return True

    def list_all_documents(self):
        return self.thrift_client.listAllDocuments()

    def _handle_centroid_document_crud_response(self, res, centroid_id, doc_id):
        if res.status != RelevanceStatus.OK:
            if res.status == RelevanceStatus.CENTROID_DOES_NOT_EXIST:
                raise exceptions.CentroidDoesNotExist(centroid_id)
            elif res.status == RelevanceStatus.DOCUMENT_DOES_NOT_EXIST:
                raise exceptions.DocumentDoesNotExist(doc_id)
            else:
                raise_unexpected(res.status)
        return True

    def add_document_to_centroid(self, centroid_id, doc_id):
        res = self.thrift_client.addDocumentToCentroid(
            centroid_id, doc_id
        )
        return self._handle_centroid_document_crud_response(
            res, centroid_id, doc_id
        )

    def remove_document_from_centroid(self, centroid_id, doc_id):
        res = self.thrift_client.removeDocumentFromCentroid(
            centroid_id, doc_id
        )
        return self._handle_centroid_document_crud_response(
            res, centroid_id, doc_id
        )

    def create_document_with_id(self, ident, doc_text):
        res = self.thrift_client.createDocumentWithID(
            ident.encode('utf-8'),
            doc_text.encode('utf-8')
        )
        if res.status != RelevanceStatus.OK:
            if res.status == RelevanceStatus.DOCUMENT_ALREADY_EXISTS:
                raise exceptions.DocumentAlreadyExists(ident)
            raise_unexpected(res.status)
        return res.created

    def create_document(self, doc_text):
        res = self.thrift_client.createDocument(doc_text.encode('utf-8'))
        if res.status != RelevanceStatus.OK:
            raise_unexpected(res.status)
        return res.created

    def get_document(self, doc_id):
        return self.thrift_client.getDocument(doc_id)

    def delete_document(self, doc_id):
        res = self.thrift_client.deleteDocument(doc_id)
        if res.status != RelevanceStatus.OK:
            if res.status == RelevanceStatus.DOCUMENT_DOES_NOT_EXIST:
                raise exceptions.DocumentDoesNotExist(doc_id)
            raise_unexpected(res.status)
        return True

    def delete_centroid(self, centroid_id):
        res = self.thrift_client.deleteCentroid(centroid_id)
        if res.status != RelevanceStatus.OK:
            if res.status == RelevanceStatus.CENTROID_DOES_NOT_EXIST:
                raise exceptions.CentroidDoesNotExist(centroid_id)
            raise_unexpected(res.status)
        return True

    def recompute_centroid(self, centroid_id):
        return self.thrift_client.recomputeCentroid(centroid_id)

    def list_all_documents_for_centroid(self, centroid_id):
        res = self.thrift_client.listAllDocumentsForCentroid(centroid_id)
        if res.status != RelevanceStatus.OK:
            if res.status == RelevanceStatus.CENTROID_DOES_NOT_EXIST:
                raise exceptions.CentroidDoesNotExist(centroid_id)
            raise_unexpected(res.status)
        return res.documents

    def get_text_similarity(self, centroid_id, text):
        res = self.thrift_client.getTextSimilarity(
            centroid_id, text.encode('utf-8')
        )
        if res.status != RelevanceStatus.OK:
            if res.status == RelevanceStatus.CENTROID_DOES_NOT_EXIST:
                raise exceptions.CentroidDoesNotExist(centroid_id)
            raise_unexpected(res.status)
        return res.relevance

    def multi_get_text_similarity(self, centroid_ids, text):
        assert(not isinstance(centroid_ids, basestring))
        res = self.thrift_client.multiGetTextSimilarity(
            centroid_ids, text.encode('utf-8')
        )
        if res.status != RelevanceStatus.OK:
            if res.status == RelevanceStatus.CENTROID_DOES_NOT_EXIST:
                raise exceptions.CentroidDoesNotExist(centroid_id)
            raise_unexpected(res.status)
        return res.scores


    def get_document_similarity(self, centroid_id, doc_id):
        res = self.thrift_client.getDocumentSimilarity(
            centroid_id, doc_id
        )
        if res.status != RelevanceStatus.OK:
            if res.status == RelevanceStatus.CENTROID_DOES_NOT_EXIST:
                raise exceptions.CentroidDoesNotExist(centroid_id)
            elif res.status == RelevanceStatus.DOCUMENT_DOES_NOT_EXIST:
                raise exceptions.DocumentDoesNotExist(doc_id)
            raise_unexpected(res.status)
        return res.relevance

