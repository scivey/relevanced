from .celery_conf import test_app
from ..main import get_client
from ..crawl import crawl_url

@test_app.task
def crawl_and_insert_urls(centroid_id, url_list):
    client = get_client()
    for url in url_list:
        text = crawl_url(url)['text']
        client.create_document_with_id(url, text)
        client.add_document_to_centroid(centroid_id, url)

@test_app.task
def connect_repeatedly(n):
    for _ in xrange(n):
        client = get_client()
        client.ping()

@test_app.task
def join_centroid_repeatedly(centroid_id, n):
    client = get_client()
    for _ in xrange(n):
        client.join_centroid(centroid_id)

@test_app.task
def remove_documents_from_centroid(centroid_id, document_ids):
    client = get_client()
    for doc_id in document_id_list:
        client.remove_document_from_centroid(centroid_id, doc_id)
