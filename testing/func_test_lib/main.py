from __future__ import print_function
import sys
import json
import time
from functools import wraps
from pprint import pprint
from relevanced_client import Client
from . import caching, urls, crawl

def get_client():
    return Client('localhost', 8097)

@caching.memoize0
def load_large_math():
    return crawl.crawl_urls(urls.MATH)

@caching.memoize0
def load_large_poli():
    return crawl.crawl_urls(urls.POLITICS)

def init_documents(client):
    existing_docs = set(client.list_all_documents().documents)
    math_docs = list(load_large_math())
    articles_by_url = {}
    urls_by_length = []
    for url, doc in load_large_math().iteritems():
        articles_by_url[doc['url']] = doc['text']
        urls_by_length.append((len(doc['text']), doc['url']))
    for url, doc in load_large_poli().iteritems():
        articles_by_url[doc['url']] = doc['text']
        urls_by_length.append((len(doc['text']), doc['url']))

    urls_by_length.sort(key=lambda x: x[0])
    urls_by_length.reverse()
    for _, url in urls_by_length:
        text = articles_by_url[url]
        if url not in existing_docs:
            res = client.create_document_with_id(
                url, text
            )
            print('created : %s' % res.id)

def init_centroids(client):
    existing_centroids = set(client.list_all_centroids().centroids)
    for coll in ('wiki_math', 'wiki_poli'):
        if coll not in existing_centroids:
            client.create_centroid(coll)
    existing_math_docs = set(client.list_all_documents_for_centroid('wiki_math').documents)
    existing_poli_docs = set(client.list_all_documents_for_centroid('wiki_poli').documents)

    missing_math = [url for url in load_large_math().keys() if url not in existing_math_docs]
    client.add_documents_to_centroid(
        'wiki_math', missing_math
    )
    # for url in load_large_math().keys():
    #     if url not in existing_math_docs:
    #         client.add_document_to_centroid(
    #             'wiki_math', url
    #         )

    missing_poli = [url for url in load_large_poli().keys() if url not in existing_poli_docs]
    client.add_documents_to_centroid(
        'wiki_poli', missing_poli
    )
    # for url in load_large_poli().keys():
    #     if url not in existing_poli_docs:
    #         client.add_document_to_centroid(
    #             'wiki_poli', url
    #         )

def main():
    client = get_client()
    init_documents(client)
    if not len(client.list_all_centroids().centroids) >= 2:
        init_centroids(client)
    client.multi_join_centroids(['wiki_math', 'wiki_poli'])
    print('\n\n')
    print('math vs math')
    for doc in load_large_math().values()[:10]:
        print(doc['title'])
        print(client.get_text_similarity('wiki_math', doc['text']))

    print('\n\n')
    print('math vs poli')
    for doc in load_large_math().values()[:10]:
        print(doc['title'])
        print(client.get_text_similarity('wiki_poli', doc['text']))


