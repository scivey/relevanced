from __future__ import print_function

from .common import IsolatedTestCase
from relevanced_client import (
    ECentroidDoesNotExist
)

class TestJoinCentroid(IsolatedTestCase):
    def test_join_centroid_no_documents(self):
        self.client.create_centroid('centroid-1')
        result = self.client.join_centroid('centroid-1')
        self.assertTrue(result.recalculated)
        self.assertEqual('centroid-1', result.id)

    def test_join_centroid_out_of_date(self):
        self.client.create_centroid('centroid-1')
        self.client.create_document_with_id('doc-1', 'some text')
        self.client.add_document_to_centroid('centroid-1', 'doc-1')
        result = self.client.join_centroid('centroid-1')
        self.assertTrue(result.recalculated)
        self.assertEqual('centroid-1', result.id)
