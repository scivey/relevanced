from __future__ import print_function
import sys
import json
from functools import wraps
sys.path.append('./gen-py')
from pprint import pprint
from TextRelevance import Relevance
from thrift import Thrift
from thrift.protocol import TBinaryProtocol
from thrift.transport import TSocket, TTransport

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

def get_client():
    sock = TSocket.TSocket('localhost', 8097)
    transport = TTransport.TBufferedTransport(sock)
    protocol = TBinaryProtocol.TBinaryProtocol(transport)
    client = Relevance.Client(protocol)
    transport.open()
    return client


def init_documents(client):
    all_articles = load_math() + load_poli()
    articles_by_url = { d['url']: d for d in all_articles}

    # load documents
    existing_docs = set(client.listDocuments())
    for url, doc in articles_by_url.iteritems():
        if url not in existing_docs:
            client.createDocumentWithID(url.encode('utf-8'), doc['text'].encode('utf-8'))

    assert(len(all_articles) == len(client.listDocuments()))

def init_collections(client):
    colls = client.listCollections()

    if 'wiki_math' not in colls:
        name = 'wiki_math'
        client.createCollection(name)
        for doc in load_math():
            client.addPositiveDocumentToCollection(name, doc['url'].encode('utf-8'))
        for doc in load_poli():
            client.addNegativeDocumentToCollection(name, doc['url'].encode('utf-8'))
        client.recompute(name)

    if 'wiki_poli' not in colls:
        name = 'wiki_poli'
        client.createCollection(name)
        for doc in load_math():
            client.addNegativeDocumentToCollection(name, doc['url'].encode('utf-8'))
        for doc in load_poli():
            client.addPositiveDocumentToCollection(name, doc['url'].encode('utf-8'))
        client.recompute(name)

    assert(2 <= len(client.listCollections()))

def main():
    client = get_client()
    init_documents(client)
    init_collections(client)

    print("math documents: %i" % client.getCollectionSize('wiki_math'))
    print("poli documents: %i" % client.getCollectionSize('wiki_poli'))

    print('math v math')
    for art in load_math():
        print(client.getRelevanceForText('wiki_math', art['text'].encode('utf-8')))
    print('math v polisci')
    for art in load_poli():
        print(client.getRelevanceForText('wiki_math', art['text'].encode('utf-8')))

    print('poli v poli')
    for art in load_poli():
        print(client.getRelevanceForText('wiki_poli', art['text'].encode('utf-8')))
    print('poli v math')
    for art in load_math():
        print(client.getRelevanceForText('wiki_poli', art['text'].encode('utf-8')))

if __name__ == '__main__':
    main()
