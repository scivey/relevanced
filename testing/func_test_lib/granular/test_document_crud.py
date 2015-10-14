from __future__ import print_function

from .common import IsolatedTestCase
from relevanced_client import (
    EDocumentDoesNotExist,
    EDocumentAlreadyExists
)

class DocumentCrudTests(IsolatedTestCase):
    def test_document_creation_1(self):
        self.client.create_document_with_id('doc-1', 'this is text')
        documents = self.client.list_all_documents().documents
        self.assertEqual(['doc-1'], documents)

        self.client.create_document_with_id('doc-2', 'doc 2 text')
        self.client.create_document_with_id('doc-3', 'doc 3 text')
        documents = self.client.list_all_documents().documents
        self.assertEqual(set(['doc-1', 'doc-2', 'doc-3']), set(documents))

    def test_document_creation_deletion(self):
        self.client.create_document_with_id('doc-1', 'doc 1 text')
        self.client.create_document_with_id('doc-2', 'doc 2 text')
        documents = self.client.list_all_documents().documents
        self.assertEqual(set(['doc-1', 'doc-2']), set(documents))

        self.client.delete_document('doc-2')
        documents = self.client.list_all_documents().documents
        self.assertEqual(['doc-1'], documents)

    def test_document_creation_already_exists(self):
        self.client.create_document_with_id('doc-1', 'doc 1 text')
        with self.assertRaises(EDocumentAlreadyExists):
            self.client.create_document_with_id('doc-1', 'more text')

    def test_document_deletion_does_not_exist(self):
        with self.assertRaises(EDocumentDoesNotExist):
            self.client.delete_document('doc-1')

