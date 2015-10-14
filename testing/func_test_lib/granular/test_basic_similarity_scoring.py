from __future__ import print_function

from .common import IsolatedTestCase
from relevanced_client import (
    EDocumentDoesNotExist,
    ECentroidDoesNotExist,
    EDocumentAlreadyInCentroid,
    EDocumentNotInCentroid
)

class TestBasicSimilarityScoring(IsolatedTestCase):
    def test_basic_text_similarity_1(self):
        self.client.create_centroid('centroid-1')
        self.client.create_document_with_id('doc-1', 'dog cat wolf wolf')
        self.client.add_document_to_centroid('centroid-1', 'doc-1')
        self.client.join_centroid('centroid-1')
        response = self.client.get_text_similarity('centroid-1', 'wolf wolf wolf')
        self.assertTrue(isinstance(response, float))
        print(response)
        self.assertTrue(response > 0.01)
        self.assertTrue(response < 1.0)
