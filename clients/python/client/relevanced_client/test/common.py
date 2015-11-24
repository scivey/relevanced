from __future__ import print_function
import unittest

from ..client import Client

class IsolatedTestCase(unittest.TestCase):
    def setUp(self):
        self.client = Client('localhost', 8097)

    def tearDown(self):
        print("tearDown: erasing everything...")
        documents = self.client.list_all_documents().documents
        centroids = self.client.list_all_centroids().centroids
        self.client.multi_delete_centroids(
            centroids, ignore_missing=True
        )
        self.client.multi_delete_documents(
            documents, ignore_missing=True
        )
