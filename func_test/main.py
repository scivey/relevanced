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

@caching.memoize0
def load_large_irrelevant():
    return crawl.crawl_urls(urls.IRRELEVANT)

def init_documents(client):
    existing_docs = set(client.list_documents())
    math_docs = list(load_large_math())
    articles_by_url = {}
    urls_by_length = []
    for url, doc in load_large_math().iteritems():
        articles_by_url[doc['url']] = doc['text']
        urls_by_length.append((len(doc['text']), doc['url']))
    for url, doc in load_large_poli().iteritems():
        articles_by_url[doc['url']] = doc['text']
        urls_by_length.append((len(doc['text']), doc['url']))
    for url, doc in load_large_irrelevant().iteritems():
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
            print('created : %s' % res)

def init_collections(client):
    existing_collections = set(client.list_collections())
    for coll in ('wiki_math', 'wiki_poli'):
        if coll not in existing_collections:
            client.create_collection(coll)

    for url in load_large_math().keys():
        client.add_positive_document_to_collection(
            'wiki_math', url
        )
        client.add_negative_document_to_collection(
            'wiki_poli', url
        )

    for url in load_large_poli().keys():
        client.add_negative_document_to_collection(
            'wiki_math', url
        )
        client.add_positive_document_to_collection(
            'wiki_poli', url
        )

    for url in load_large_irrelevant().keys():
        for coll in ('wiki_math', 'wiki_poli'):
            client.add_negative_document_to_collection(coll, url)

def main():
    client = get_client()
    # init_documents(client)
    # init_collections(client)
    # client.recompute('wiki_math')
    # client.recompute('wiki_poli')
    print('math -> math')
    for doc in load_large_math().values()[:10]:
        print(doc['title'])
        print(client.get_relevance_for_text('wiki_math', doc['text']))

    print('poli -> math')
    for doc in load_large_math().values()[:10]:
        print(doc['title'])
        print(client.get_relevance_for_text('wiki_poli', doc['text']))

