from __future__ import print_function
import sys
import json
import time
from functools import wraps
from pprint import pprint
from relevanced_client import Client
from . import caching, urls, crawl

def init_documents(client):
    all_articles = load_math() + load_poli()
    articles_by_url = { d['url']: d for d in all_articles}
    existing_docs = set(client.list_documents())
    for url, doc in articles_by_url.iteritems():
        if url not in existing_docs:
            client.create_document_with_id(url, doc['text'])
    pprint(client.list_documents())

def init_collections(client):
    colls = client.list_collections()

    if 'wiki_math' not in colls:
        name = 'wiki_math'
        client.create_collection(name)
        for doc in load_math():
            print(client.add_positive_document_to_collection(
                name, doc['url']
            ))
        for doc in load_poli():
            print(client.add_negative_document_to_collection(
                name, doc['url']
            ))

    if 'wiki_poli' not in colls:
        name = 'wiki_poli'
        client.create_collection(name)
        for doc in load_math():
            print(client.add_negative_document_to_collection(
                name, doc['url']
            ))
        for doc in load_poli():
            print(client.add_positive_document_to_collection(
                name, doc['url']
            ))


def get_client():
    return Client('localhost', 8097)

def main_small():
    client = get_client()
    init_documents(client)
    init_collections(client)
    client.recompute('wiki_math')
    client.recompute('wiki_poli')

    print("math documents: %i" % client.get_collection_size('wiki_math'))
    print("poli documents: %i" % client.get_collection_size('wiki_poli'))

    print('math v math')
    for art in load_math():
        print(client.get_relevance_for_text('wiki_math', art['text']))
    print('math v polisci')
    for art in load_poli():
        print(client.get_relevance_for_text('wiki_math', art['text']))

    print('poli v poli')
    for art in load_poli():
        print(client.get_relevance_for_text('wiki_poli', art['text']))
    print('poli v math')
    for art in load_math():
        print(client.get_relevance_for_text('wiki_poli', art['text']))

@caching.memoize0
def load_large_math():
    return crawl.crawl_urls(urls.MATH)

@caching.memoize0
def load_large_poli():
    return crawl.crawl_urls(urls.POLITICS)

@caching.memoize0
def load_large_irrelevant():
    return crawl.crawl_urls(urls.IRRELEVANT)

def init_documents_large(client):
    existing_docs = set(client.list_documents())
    math_docs = list(load_large_math())
    articles_by_url = {}
    for url, doc in load_large_math().iteritems():
        articles_by_url[doc['url']] = doc['text']

    for url, doc in load_large_poli().iteritems():
        articles_by_url[doc['url']] = doc['text']

    for url, doc in load_large_irrelevant().iteritems():
        articles_by_url[doc['url']] = doc['text']

    for url, text in articles_by_url.iteritems():
        if url not in existing_docs:
            res = client.create_document_with_id(
                url, text
            )
            print('created : %s' % res)


def init_collections_large(client):
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

def main_large():
    client = get_client()
    # init_documents_large(client)
    # init_collections_large(client)
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


def bench(secs):
    client = get_client()
    init_documents(client)
    init_collections(client)
    doc = load_math()[0]['text'].encode('utf-8')
    start = time.time()
    count = 0
    while True:
        client.getRelevanceForText('wiki_math', doc)
        count += 1
        if time.time() - start >= secs:
            break
    return count

def memoize(func):
    closure_ = {}
    @wraps(func)
    def out():
        if 'result' not in closure_:
            closure_['result'] = func()
        return closure_['result']
    return out

@memoize
def _load_raw_data():
    with open('./test_data/text.json') as f:
        return f.read()

def load_math():
    return json.loads(_load_raw_data())['math']

def load_poli():
    return json.loads(_load_raw_data())['politics']

if __name__ == '__main__':
    if len(sys.argv) >= 2 and sys.argv[1] == 'bench':
        interval = 5.0
        count = bench(interval)
        print("%i relevance calls in %f secs" % (count, interval))
    else:
        main()
