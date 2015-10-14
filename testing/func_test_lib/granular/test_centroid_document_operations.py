from __future__ import print_function

from .common import IsolatedTestCase
from relevanced_client import (
    EDocumentDoesNotExist,
    ECentroidDoesNotExist,
    EDocumentAlreadyInCentroid,
    EDocumentNotInCentroid
)

class TestCentroidDocumentOperations(IsolatedTestCase):
    def test_add_document_1(self):
        self.client.create_centroid('centroid-1')
        self.client.create_document_with_id('doc-1', 'doc 1 text')
        self.client.add_document_to_centroid('centroid-1', 'doc-1')
        documents = self.client.list_all_documents_for_centroid('centroid-1').documents
        self.assertEqual(['doc-1'], documents)

    def test_add_remove_documents(self):
        self.client.create_centroid('centroid-1')
        self.client.create_document_with_id('doc-1', 'doc 1 text')
        self.client.add_document_to_centroid('centroid-1', 'doc-1')

        documents = self.client.list_all_documents_for_centroid('centroid-1').documents
        self.assertEqual(['doc-1'], documents)

        self.client.create_document_with_id('doc-2', 'doc 2 text')
        self.client.create_document_with_id('doc-3', 'doc 3 text')
        self.client.add_document_to_centroid('centroid-1', 'doc-2')
        self.client.add_document_to_centroid('centroid-1', 'doc-3')

        documents = self.client.list_all_documents_for_centroid('centroid-1').documents
        self.assertEqual(set(['doc-1', 'doc-2', 'doc-3']), set(documents))

        self.client.remove_document_from_centroid('centroid-1', 'doc-2')

        documents = self.client.list_all_documents_for_centroid('centroid-1').documents
        self.assertEqual(set(['doc-1', 'doc-3']), set(documents))

    def test_add_document_missing_centroid(self):
        self.client.create_centroid('good-centroid')
        self.client.create_document_with_id('doc-1', 'some text')
        with self.assertRaises(ECentroidDoesNotExist):
            self.client.add_document_to_centroid('bad-centroid', 'doc-1')

    def test_add_document_missing_document(self):
        self.client.create_centroid('good-centroid')
        with self.assertRaises(EDocumentDoesNotExist):
            self.client.add_document_to_centroid('good-centroid', 'doc-1')

    def test_add_document_already_in_centroid(self):
        self.client.create_centroid('centroid-1')
        self.client.create_document_with_id('doc-1', 'some text')
        self.client.add_document_to_centroid('centroid-1', 'doc-1')
        with self.assertRaises(EDocumentAlreadyInCentroid):
            self.client.add_document_to_centroid('centroid-1', 'doc-1')

    def test_remove_document_not_in_centroid(self):
        self.client.create_centroid('centroid-1')
        self.client.create_document_with_id('doc-1', 'some text')
        self.client.create_document_with_id('doc-2', 'more text')
        self.client.add_document_to_centroid('centroid-1', 'doc-1')
        with self.assertRaises(EDocumentNotInCentroid):
            self.client.remove_document_from_centroid('centroid-1', 'doc-2')

    def test_add_document_missing_document_and_centroid(self):
        with self.assertRaises(ECentroidDoesNotExist):
            self.client.add_document_to_centroid('some-centroid', 'some-doc')

    def test_remove_document_missing_document_and_centroid(self):
        with self.assertRaises(ECentroidDoesNotExist):
            self.client.remove_document_from_centroid('some-centroid', 'some-doc')

