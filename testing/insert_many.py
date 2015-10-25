from __future__ import print_function
import sys
from func_test_lib.main import get_client, load_large_math, load_large_poli

def throw_usage():
    raise ValueError('usage: <name.py> start:int count:int')

if __name__ == '__main__':
    if len(sys.argv) < 3:
        throw_usage()
    try:
        start = int(sys.argv[1])
        count = int(sys.argv[2])
    except Exception:
        throw_usage()

    client = get_client()
    existing = set(client.list_all_documents().documents)
    math = load_large_math()
    poli = load_large_poli()
    for i in xrange(start, start+count):
        for collection in (math, poli):
            for url, doc in collection.iteritems():
                url = '%s-%i' % (url, i)
                print('creating: %s' % url)
                client.create_document_with_id(url, doc['text'])
