package org.relevanced.client;

import org.apache.thrift.TException;
import org.apache.thrift.transport.TSSLTransportFactory;
import org.apache.thrift.transport.TTransport;
import org.apache.thrift.transport.TSocket;
import org.apache.thrift.transport.TSSLTransportFactory.TSSLTransportParameters;
import org.apache.thrift.protocol.TBinaryProtocol;
import org.apache.thrift.protocol.TProtocol;
import org.relevanced.client.gen_thrift_protocol.Relevanced;
import org.relevanced.client.gen_thrift_protocol.SimilarityResponse;
import org.relevanced.client.gen_thrift_protocol.CrudResponse;
import org.relevanced.client.gen_thrift_protocol.Status;
import org.relevanced.client.gen_thrift_protocol.StatusCode;
import org.relevanced.client.exceptions.RelevancedException;
import org.relevanced.client.exceptions.CentroidDoesNotExist;
import org.relevanced.client.exceptions.DocumentDoesNotExist;
import org.relevanced.client.exceptions.CentroidAlreadyExists;
import org.relevanced.client.exceptions.DocumentAlreadyExists;
import org.relevanced.client.exceptions.UnspecifiedException;

public class RelevancedBlockingClient {
    public Relevanced.Client thriftClient_;
    public TTransport thriftTransport_;
    public TProtocol thriftProtocol_;
    public String hostname_;
    public Int portNum_;

    public RelevancedBlockingClient(String host, Int port) {
        hostname_ = host;
        portNum_ = port;
        try {
            thriftTransport_ = new TSocket(hostname_, portNum_);
            thriftTransport_.open();
            thriftProtocol_ = new TBinaryProtocol(thriftTransport_);
            thriftClient_ = new Relevanced.Client(thriftProtocol_);
        } catch (TException err) {
            err.printStackTrace();
        }
    }

    protected void handleResponseStatus(Status status) throws RelevancedException {
        if (status.code == OK) {
            return;
        }
        switch (status.code) {
            case CENTROID_DOES_NOT_EXIST:
                throw new CentroidDoesNotExist(status.message);
                break;
            case CENTROID_ALREADY_EXISTS:
                throw new CentroidAlreadyExists(status.message);
                break;
            case DOCUMENT_DOES_NOT_EXIST:
                throw new DocumentDoesNotExist(status.message);
                break;
            case DOCUMENT_ALREADY_EXISTS:
                throw new DocumentAlreadyExists(status.message);
                break;
            default:
                throw new UnspecifiedException(status.message);
                break;
        }
    }

    public Map<String, Double> multiGetTextSimilarity(List<String> centroidIds, String text) {
        SimilarityResponse response = thriftClient_.multiGetTextSimilarity(centroidId, documentId);
        handleResponseStatus(response.status);
        return response.scores;
    }

    public Double getTextSimilarity(String centroidId, String text) {
        SimilarityResponse response = thriftClient_.getTextSimilarity(centroidId, text);
        handleResponseStatus(response.status);
        return response.similarity;
    }

    public Double getDocumentSimilarity(String centroidId, String documentId) {
        SimilarityResponse response = thriftClient_.getDocumentSimilarity(centroidId, documentId);
        handleResponseStatus(response.status);
        return response.similarity;
    }

    public Double getCentroidSimilarity(String centroidId1, String centroidId2) {
        SimilarityResponse response = thriftClient_.getCentroidSimilarity(centroidId1, centroidId2);
        handleResponseStatus(response.status);
        return response.similarity;
    }

    public String createDocument(String text) {
        CrudResponse response = thriftClient_.createDocument(text);
        handleResponseStatus(response.status);
        return response.created;
    }

    public String createDocumentWithID(String id, String text) {
        CrudResponse response = thriftClient_.createDocumentWithID(id, text);
        handleResponseStatus(response.status);
        return response.created;
    }

    public Boolean deleteDocument(String documentId) {
        CrudResponse response = thriftClient_.deleteDocument(documentId);
        handleResponseStatus(response.status);
        return true;
    }

    public String createCentroid(String name) {
        CrudResponse response = thriftClient_.createCentroid(name);
        handleResponseStatus(response.status);
        return response.created;
    }

    public Boolean deleteCentroid(String name) {
        CrudResponse response = thriftClient_.deleteCentroid(name);
        handleResponseStatus(response.status);
        return true;
    }

    public Boolean addDocumentToCentroid(String centroidId, String documentId) {
        CrudResponse response = thriftClient_.addDocumentToCentroid(centroidId, documentId);
        handleResponseStatus(response.status);
        return true;
    }

    public Boolean removeDocumentFromCentroid(String centroidId, String documentId) {
        CrudResponse response = thriftClient_.removeDocumentFromCentroid(centroidId, documentId);
        handleResponseStatus(response.status);
        return true;
    }

    public List<String> listAllDocuments() {
        List<String> documents = thriftClient_.listAllDocuments();
        return documents;
    }

    public List<String> listAllCentroids() {
        List<String> centroids = thriftClient_.listAllCentroids();
        return centroids;
    }

    public List<String> listAllDocumentsForCentroid(String centroidId) {
        List<String> documents = thriftClient_.listAllDocumentsForCentroid(centroidId);
        return documents;
    }

}