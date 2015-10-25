from func_test_lib.main import get_client, load_large_math, load_large_poli
import sys
if __name__ == '__main__':
    client = get_client()
    name = sys.argv[1]
    num = int(sys.argv[2])
    loader = {
        'poli': load_large_poli,
        'math': load_large_math
    }[name]
    centroid_name = '%s_%i' % (name, num)
    client.create_centroid(centroid_name)
    for url, doc in loader().iteritems():
        doc_id = '%s-%i' % (url, num)
        client.add_document_to_centroid(centroid_name, doc_id)

