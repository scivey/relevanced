from __future__ import print_function
import unittest

from ..main import get_client

class IsolatedTestCase(unittest.TestCase):
    def setUp(self):
        self.client = get_client()

    def tearDown(self):
        print("tearDown: erasing everything...")
        self.client.thrift_client.debugEraseAllData()
