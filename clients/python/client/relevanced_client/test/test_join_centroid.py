from __future__ import print_function

from .common import IsolatedTestCase
from .. import (
    ECentroidDoesNotExist
)

class TestJoinCentroid(IsolatedTestCase):
    def test_join_centroid_no_documents(self):
        self.client.create_centroid('some-new-centroid-1')
        result = self.client.join_centroid('some-new-centroid-1')
        self.assertFalse(result.recalculated)
        self.assertEqual('some-new-centroid-1', result.id)

    def test_join_centroid_out_of_date(self):
        self.client.create_centroid('some-new-centroid-2')
        self.client.create_document_with_id('doc-1', 'some text')
        self.client.add_document_to_centroid('some-new-centroid-2', 'doc-1')
        result = self.client.join_centroid('some-new-centroid-2')
        self.assertTrue(result.recalculated)
        self.assertEqual('some-new-centroid-2', result.id)
