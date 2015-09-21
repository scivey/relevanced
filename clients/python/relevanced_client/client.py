from __future__ import print_function
from thrift import Thrift
from thrift.protocol import TBinaryProtocol
from thrift.transport import TSocket, TTransport
from .gen_py.TextRelevance import Relevance

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

    def list_collections(self):
        return self.thrift_client.listCollections()

    def create_collection(self, name):
        return self.thrift_client.createCollection(name)

    def list_documents(self):
        return self.thrift_client.listDocuments()

    def add_positive_document_to_collection(self, collection_id, doc_id):
        return self.thrift_client.addPositiveDocumentToCollection(
            collection_id, doc_id
        )

    def add_negative_document_to_collection(self, collection_id, doc_id):
        return self.thrift_client.addNegativeDocumentToCollection(
            collection_id, doc_id
        )

    def remove_document_from_collection(self, collection_id, doc_id):
        return self.thrift_client.removeDocumentFromCollection(
            collection_id, doc_id
        )

    def create_document_with_id(self, ident, doc_text):
        return self.thrift_client.createDocumentWithID(
            ident.encode('utf-8'),
            doc_text.encode('utf-8')
        )

    def create_document(self, doc_text):
        return self.thrift_client.createDocument(doc_text.encode('utf-8'))

    def get_document(self, doc_id):
        return self.thrift_client.getDocument(doc_id)

    def delete_document(self, doc_id):
        return self.thrift_client.deleteDocument(doc_id)

    def delete_collection(self, collection_id):
        return self.thrift_client.deleteCollection(collection_id)

    def recompute(self, collection_id):
        return self.thrift_client.recompute(collection_id)

    def list_collection_documents(self, collection_id):
        return self.thrift_client.listCollectionDocuments(collection_id)

    def get_collection_size(self, collection_id):
        return self.thrift_client.getCollectionSize(collection_id)

    def get_relevance_for_text(self, collection_id, text):
        return self.thrift_client.getRelevanceForText(
            collection_id, text.encode('utf-8')
        )

    def get_relevance_for_doc(self, collection_id, doc_id):
        return self.thrift_client.getRelevanceForDoc(
            collection_id, doc_id
        )
