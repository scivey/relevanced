from __future__ import print_function
from func_test_lib.main import load_large_math, get_client

if __name__ == '__main__':
    client = get_client()
    existing = set(client.list_all_documents().documents)
    to_create = []
    for url, doc in load_large_math().iteritems():
        if url in existing:
            continue
        if len(to_create) > 5:
            break
        to_create.append((url, doc))
    for url, doc in to_create:
        print('creating %s' % url)
        client.create_document_with_id(url, doc['text'])
