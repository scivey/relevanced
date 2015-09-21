from __future__ import print_function
import sys
import json
import time
from functools import wraps
from pprint import pprint

sys.path.append('./clients/python')
from relevanced_client import Client

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
            client.add_positive_document_to_collection(
                name, doc['url']
            )
        for doc in load_poli():
            client.add_negative_document_to_collection(
                name, doc['url']
            )
        client.recompute(name)

    if 'wiki_poli' not in colls:
        name = 'wiki_poli'
        client.create_collection(name)
        for doc in load_math():
            client.add_negative_document_to_collection(
                name, doc['url']
            )
        for doc in load_poli():
            client.add_positive_document_to_collection(
                name, doc['url']
            )
        client.recompute(name)

    assert(2 <= len(client.list_collections()))

def main():
    client = Client('localhost', 8097)
    init_documents(client)
    init_collections(client)

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
