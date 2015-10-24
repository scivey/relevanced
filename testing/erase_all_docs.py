from __future__ import print_function
if __name__ == '__main__':
    from func_test_lib.main import get_client
    client = get_client()
    docs = client.list_all_documents().documents
    count = len(docs)
    print('Deleting %i docs' % count)
    for d in docs:
        client.delete_document(d)
