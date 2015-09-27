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

    def list_classifiers(self):
        return self.thrift_client.listClassifiers()

    def create_classifier(self, name):
        res = self.thrift_client.createClassifier(name)
        if res.status != RelevanceStatus.OK:
            if res.status == RelevanceStatus.CLASSIFIER_ALREADY_EXISTS:
                raise exceptions.ClassifierAlreadyExists(name)
            raise_unexpected(res.status)
        return True

    def list_documents(self):
        return self.thrift_client.listDocuments()

    def _handle_classifier_document_crud_response(self, res, classifier_id, doc_id):
        if res.status != RelevanceStatus.OK:
            if res.status == RelevanceStatus.CLASSIFIER_DOES_NOT_EXIST:
                raise exceptions.ClassifierDoesNotExist(classifier_id)
            elif res.status == RelevanceStatus.DOCUMENT_DOES_NOT_EXIST:
                raise exceptions.DocumentDoesNotExist(doc_id)
            else:
                raise_unexpected(res.status)
        return True

    def add_positive_document_to_classifier(self, classifier_id, doc_id):
        res = self.thrift_client.addPositiveDocumentToClassifier(
            classifier_id, doc_id
        )
        return self._handle_classifier_document_crud_response(
            res, classifier_id, doc_id
        )

    def add_negative_document_to_classifier(self, classifier_id, doc_id):
        res = self.thrift_client.addNegativeDocumentToClassifier(
            classifier_id, doc_id
        )
        return self._handle_classifier_document_crud_response(
            res, classifier_id, doc_id
        )

    def remove_document_from_classifier(self, classifier_id, doc_id):
        res = self.thrift_client.removeDocumentFromClassifier(
            classifier_id, doc_id
        )
        return self._handle_classifier_document_crud_response(
            res, classifier_id, doc_id
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

    def delete_classifier(self, classifier_id):
        res = self.thrift_client.deleteClassifier(classifier_id)
        if res.status != RelevanceStatus.OK:
            if res.status == RelevanceStatus.CLASSIFIER_DOES_NOT_EXIST:
                raise exceptions.ClassifierDoesNotExist(classifier_id)
            raise_unexpected(res.status)
        return True

    def recompute(self, classifier_id):
        return self.thrift_client.recompute(classifier_id)

    def list_all_classifier_documents(self, classifier_id):
        res = self.thrift_client.listAllClassifierDocuments(classifier_id)
        if res.status != RelevanceStatus.OK:
            if res.status == RelevanceStatus.CLASSIFIER_DOES_NOT_EXIST:
                raise exceptions.ClassifierDoesNotExist(classifier_id)
            raise_unexpected(res.status)
        return res.documents

    def get_classifier_size(self, classifier_id):
        res = self.thrift_client.getClassifierSize(classifier_id)
        if res.status != RelevanceStatus.OK:
            if res.status == RelevanceStatus.CLASSIFIER_DOES_NOT_EXIST:
                raise exceptions.ClassifierDoesNotExist(classifier_id)
            raise_unexpected(res.status)
        return res.size

    def get_relevance_for_text(self, classifier_id, text):
        res = self.thrift_client.getRelevanceForText(
            classifier_id, text.encode('utf-8')
        )
        if res.status != RelevanceStatus.OK:
            if res.status == RelevanceStatus.CLASSIFIER_DOES_NOT_EXIST:
                raise exceptions.ClassifierDoesNotExist(classifier_id)
            raise_unexpected(res.status)
        return res.relevance

    def get_relevance_for_doc(self, classifier_id, doc_id):
        res = self.thrift_client.getRelevanceForDoc(
            classifier_id, doc_id
        )
        if res.status != RelevanceStatus.OK:
            if res.status == RelevanceStatus.CLASSIFIER_DOES_NOT_EXIST:
                raise exceptions.ClassifierDoesNotExist(classifier_id)
            elif res.status == RelevanceStatus.DOCUMENT_DOES_NOT_EXIST:
                raise exceptions.DocumentDoesNotExist(doc_id)
            raise_unexpected(res.status)
        return res.relevance

