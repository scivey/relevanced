from __future__ import print_function
import heapq
from thrift import Thrift
from thrift.protocol import TBinaryProtocol
from thrift.transport import TSocket, TTransport
from .gen_py.TextRelevance import Relevance
from .gen_py.TextRelevance.ttypes import RelevanceStatus
from . import exceptions

def raise_unexpected(response_code):
    err_name = RelevanceStatus._VALUES_TO_NAME.get(response_code, 'UNKNOWN')
    msg = "UnexpectedResponse: [%i]: '%s'" % (response_code, err_name)
    raise exceptions.UnexpectedResponse(msg)

class Client(object):
    def __init__(self, host, port):
        self.host = host
        self.port = int(port)

    @property
    def thrift_client(self):
        if not hasattr(self, '_thrift_client'):
            sock = TSocket.TSocket(self.host, self.port)
            transport = TTransport.TBufferedTransport(sock)
            protocol = TBinaryProtocol.TBinaryProtocol(transport)
            self._thrift_client = Relevance.Client(protocol)
            transport.open()
        return self._thrift_client

    def list_all_centroids(self):
        return self.thrift_client.listAllCentroids()

    def create_centroid(self, name):
        res = self.thrift_client.createCentroid(name)
        if res.status != RelevanceStatus.OK:
            if res.status == RelevanceStatus.CENTROID_ALREADY_EXISTS:
                raise exceptions.CentroidAlreadyExists(name)
            raise_unexpected(res.status)
        return True

    def list_all_documents(self):
        return self.thrift_client.listAllDocuments()

    def _handle_centroid_document_crud_response(self, res, centroid_id, doc_id):
        if res.status != RelevanceStatus.OK:
            if res.status == RelevanceStatus.CENTROID_DOES_NOT_EXIST:
                raise exceptions.CentroidDoesNotExist(centroid_id)
            elif res.status == RelevanceStatus.DOCUMENT_DOES_NOT_EXIST:
                raise exceptions.DocumentDoesNotExist(doc_id)
            else:
                raise_unexpected(res.status)
        return True

    def add_document_to_centroid(self, centroid_id, doc_id):
        res = self.thrift_client.addDocumentToCentroid(
            centroid_id, doc_id
        )
        return self._handle_centroid_document_crud_response(
            res, centroid_id, doc_id
        )

    def remove_document_from_centroid(self, centroid_id, doc_id):
        res = self.thrift_client.removeDocumentFromCentroid(
            centroid_id, doc_id
        )
        return self._handle_centroid_document_crud_response(
            res, centroid_id, doc_id
        )

    def create_document_with_id(self, ident, doc_text):
        res = self.thrift_client.createDocumentWithID(
            ident.encode('utf-8'),
            doc_text.encode('utf-8')
        )
        if res.status != RelevanceStatus.OK:
            if res.status == RelevanceStatus.DOCUMENT_ALREADY_EXISTS:
                raise exceptions.DocumentAlreadyExists(ident)
            raise_unexpected(res.status)
        return res.created

    def create_document(self, doc_text):
        res = self.thrift_client.createDocument(doc_text.encode('utf-8'))
        if res.status != RelevanceStatus.OK:
            raise_unexpected(res.status)
        return res.created

    def get_document(self, doc_id):
        return self.thrift_client.getDocument(doc_id)

    def delete_document(self, doc_id):
        res = self.thrift_client.deleteDocument(doc_id)
        if res.status != RelevanceStatus.OK:
            if res.status == RelevanceStatus.DOCUMENT_DOES_NOT_EXIST:
                raise exceptions.DocumentDoesNotExist(doc_id)
            raise_unexpected(res.status)
        return True

    def delete_centroid(self, centroid_id):
        res = self.thrift_client.deleteCentroid(centroid_id)
        if res.status != RelevanceStatus.OK:
            if res.status == RelevanceStatus.CENTROID_DOES_NOT_EXIST:
                raise exceptions.CentroidDoesNotExist(centroid_id)
            raise_unexpected(res.status)
        return True

    def recompute_centroid(self, centroid_id):
        return self.thrift_client.recomputeCentroid(centroid_id)

    def list_all_documents_for_centroid(self, centroid_id):
        res = self.thrift_client.listAllDocumentsForCentroid(centroid_id)
        if res.status != RelevanceStatus.OK:
            if res.status == RelevanceStatus.CENTROID_DOES_NOT_EXIST:
                raise exceptions.CentroidDoesNotExist(centroid_id)
            raise_unexpected(res.status)
        return res.documents

    def get_centroid_similarity(self, centroid_1_id, centroid_2_id):
        return self.thrift_client.getCentroidSimilarity(centroid_1_id, centroid_2_id)

    def get_text_similarity(self, centroid_id, text):
        res = self.thrift_client.getTextSimilarity(
            centroid_id, text.encode('utf-8')
        )
        if res.status != RelevanceStatus.OK:
            if res.status == RelevanceStatus.CENTROID_DOES_NOT_EXIST:
                raise exceptions.CentroidDoesNotExist(centroid_id)
            raise_unexpected(res.status)
        return res.relevance

    def multi_get_text_similarity(self, centroid_ids, text):
        assert(not isinstance(centroid_ids, basestring))
        res = self.thrift_client.multiGetTextSimilarity(
            centroid_ids, text.encode('utf-8')
        )
        if res.status != RelevanceStatus.OK:
            if res.status == RelevanceStatus.CENTROID_DOES_NOT_EXIST:
                raise exceptions.CentroidDoesNotExist(centroid_id)
            raise_unexpected(res.status)
        return res.scores


    def get_document_similarity(self, centroid_id, doc_id):
        res = self.thrift_client.getDocumentSimilarity(
            centroid_id, doc_id
        )
        if res.status != RelevanceStatus.OK:
            if res.status == RelevanceStatus.CENTROID_DOES_NOT_EXIST:
                raise exceptions.CentroidDoesNotExist(centroid_id)
            elif res.status == RelevanceStatus.DOCUMENT_DOES_NOT_EXIST:
                raise exceptions.DocumentDoesNotExist(doc_id)
            raise_unexpected(res.status)
        return res.relevance

    def get_all_centroid_similarities(self):
        centroids = self.list_all_centroids()
        distances = {}
        for centroid_name in centroids:
            other_centroids = [name for name in centroids if name != centroid_name]
            for other_name in other_centroids:
                if (centroid_name, other_name) in distances:
                    continue
                current_dist = self.get_centroid_similarity(centroid_name, other_name)
                distances[(centroid_name, other_name)] = current_dist
                distances[(other_name, centroid_name)] = current_dist
        output = {}
        for centroid_name in centroids:
            other_distances = [item for item in distances.iteritems() if item[0][0] == centroid_name]
            other_distances = map(lambda x: (x[0][1], x[1]), other_distances)
            other_distances.sort(key=lambda x: x[1])
            output[centroid_name] = other_distances
        return output

    def get_all_centroid_distances(self):
        similarities = self.get_all_centroid_similarities()
        distances = {}
        for name, scores in similarities.iteritems():
            scores = map(lambda x: (x[0], 1 - x[1]), scores)
            scores.sort(key=lambda x: -x[1])
            distances[name] = scores
        return distances

    def get_centroid_k_neighbor_distances(self, k=3):
        distances = self.get_all_centroid_distances()
        k_neighbors = {}
        for name, distances in distances.iteritems():
            name[k_neighbors] = {other_name: dist for other_name, dist in distances[-k:]}
        return k_neighbors

    def get_manifold_regularized_distances(self, k=3):
        distances = self.get_centroid_k_neighbor_distances(k=3)
        all_names = distances.keys()
        all_shortest_paths = {}
        for current_name, neighbor_distances in distances.iteritems():
            shortest_paths = {}
            for other_name in all_names:
                if other_name == current_name:
                    continue
                if other_name in neighbor_distances:
                    shortest_paths[other_name] = neighbor_distances[other_name]
                    continue
                shortest_paths[other_name] = shortest_path(distances, current_name, other_name)
            all_shortest_paths[current_name] = shortest_paths
        return all_shortest_paths


class Path(object):
    def __init__(self, distance, edges):
        self.distance = distance
        self.edges = edges

    def __cmp__(self, other):
        return cmp(self.distance, other.distance)

    def push(self, edge_name, edge_dist):
        edges = tuple(list(self.edges) + [edge_name])
        dist = self.distance + edge_dist
        return Path(dist, edges)

def shortest_path(distances_dict, origin, destination):
    heap = heapq.heapify([])
