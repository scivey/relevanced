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

def main():
    client = get_client()
    # client.createCollection('wiki_math')
    # client.createCollection('foobar')
    colls = client.listCollections()
    pprint(colls)
    if 'wiki_math' not in colls:
        print('initializing')
        client.createCollection('wiki_math')
        for art in load_math():
            client.addPositiveToCollection('wiki_math', art['url'].encode('utf-8'), art['text'].encode('utf-8'))
        for art in load_poli():
            client.addNegativeToCollection('wiki_math', art['url'].encode('utf-8'), art['text'].encode('utf-8'))
        client.recompute('wiki_math')

    if 'wiki_poli' not in colls:
        client.createCollection('wiki_poli')
        for art in load_math():
            client.addNegativeToCollection('wiki_poli', art['url'].encode('utf-8'), art['text'].encode('utf-8'))
        for art in load_poli():
            client.addPositiveToCollection('wiki_poli', art['url'].encode('utf-8'), art['text'].encode('utf-8'))
        client.recompute('wiki_poli')
    # client.recompute()

    pprint(client.listCollections())
    # print(client.recompute('wiki_math'))
    # print('math')
    print(client.getCollectionSize('wiki_math'))
    print('math v math')
    for art in load_math():
        print(client.getRelevance('wiki_math', art['text'].encode('utf-8')))
    print('math v polisci')
    for art in load_poli():
        print(client.getRelevance('wiki_math', art['text'].encode('utf-8')))

    print('poli v poli')
    for art in load_poli():
        print(client.getRelevance('wiki_poli', art['text'].encode('utf-8')))
    print('poli v math')
    for art in load_math():
        print(client.getRelevance('wiki_poli', art['text'].encode('utf-8')))

    # for art in load_math():
    #     # print(client.getRelevance('wiki_math', art['text'].encode('utf-8')))
    #     client.addPositiveToCollection('wiki_math', art['url'].encode('utf-8'), art['text'].encode('utf-8'))
    # # print('poli')
    # for art in load_poli():
    # #     print(client.getRelevance('wiki_math', art['text'].encode('utf-8')))
    #     client.addNegativeToCollection('wiki_math', art['url'].encode('utf-8'), art['text'].encode('utf-8'))

    # # print('math scores')


    # sock = TSocket.TSocket('localhost', 8097)
    # transport = TTransport.TBufferedTransport(sock)
    # protocol = TBinaryProtocol.TBinaryProtocol(transport)
    # client = Relevance.Client(protocol)
    # transport.open()
    # result = client.createCollection("wiki_math")
    # print(result)

if __name__ == '__main__':
    main()
