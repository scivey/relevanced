from func_test_lib.main import get_client, init_documents
import sys

def delete_all_docs(client):
    docs = client.list_all_documents().documents
    for doc in docs:
        client.delete_document(doc)

if __name__ == '__main__':
    if len(sys.argv) == 1:
        cycles = 10
    else:
        cycles = int(sys.argv[1])
    client = get_client()
    for _ in xrange(cycles):
        delete_all_docs(client)
        init_documents(client)
