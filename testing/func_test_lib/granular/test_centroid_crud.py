from __future__ import print_function

from .common import IsolatedTestCase
from relevanced_client import (
    ECentroidDoesNotExist,
    ECentroidAlreadyExists
)

class TestCentroidCrud(IsolatedTestCase):
    def test_centroid_creation_1(self):
        self.client.create_centroid('centroid-1')
        centroids = self.client.list_all_centroids().centroids
        self.assertEqual(['centroid-1'], centroids)

    def test_centroid_creation_deletion(self):
        self.client.create_centroid('centroid-1')
        centroids = self.client.list_all_centroids().centroids
        self.assertEqual(['centroid-1'], centroids)

        self.client.create_centroid('centroid-2')
        self.client.create_centroid('centroid-3')

        centroids = self.client.list_all_centroids().centroids
        self.assertEqual(set(['centroid-1', 'centroid-2', 'centroid-3']), set(centroids))

        self.client.delete_centroid('centroid-2')

        centroids = self.client.list_all_centroids().centroids
        self.assertEqual(set(['centroid-1', 'centroid-3']), set(centroids))

    def test_centroid_creation_already_exists(self):
        self.client.create_centroid('centroid-1')
        with self.assertRaises(ECentroidAlreadyExists):
            self.client.create_centroid('centroid-1')

    def test_centroid_deletion_does_not_exist(self):
        with self.assertRaises(ECentroidDoesNotExist):
            self.client.delete_centroid('centroid-1')


