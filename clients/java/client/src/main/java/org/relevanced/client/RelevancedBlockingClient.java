package org.relevanced.client;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.apache.thrift.protocol.TBinaryProtocol;
import org.apache.thrift.protocol.TProtocol;
import org.apache.thrift.TException;
import org.apache.thrift.transport.TSocket;
import org.apache.thrift.transport.TSSLTransportFactory.TSSLTransportParameters;
import org.apache.thrift.transport.TSSLTransportFactory;
import org.apache.thrift.transport.TTransport;

import org.relevanced.client.gen_thrift_protocol.CrudResponse;
import org.relevanced.client.gen_thrift_protocol.ListCentroidDocumentsResponse;
import org.relevanced.client.gen_thrift_protocol.MultiSimilarityResponse;
import org.relevanced.client.gen_thrift_protocol.Relevanced;
import org.relevanced.client.gen_thrift_protocol.SimilarityResponse;
import org.relevanced.client.gen_thrift_protocol.Status;
import org.relevanced.client.gen_thrift_protocol.StatusCode;

import org.relevanced.client.exceptions.CentroidAlreadyExists;
import org.relevanced.client.exceptions.CentroidDoesNotExist;
import org.relevanced.client.exceptions.ConnectionError;
import org.relevanced.client.exceptions.DocumentAlreadyExists;
import org.relevanced.client.exceptions.DocumentDoesNotExist;
import org.relevanced.client.exceptions.RelevancedException;
import org.relevanced.client.exceptions.UnknownException;

public class RelevancedBlockingClient {
    public Relevanced.Client thriftClient_;
    public TTransport thriftTransport_;
    public TProtocol thriftProtocol_;
    public String hostname_;
    public int portNum_;

    public RelevancedBlockingClient(String host, int port) {
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
        if (status.code == StatusCode.OK) {
            return;
        }
        switch (status.code) {
            case CENTROID_DOES_NOT_EXIST:
                throw new CentroidDoesNotExist(status.message);
            case CENTROID_ALREADY_EXISTS:
                throw new CentroidAlreadyExists(status.message);
            case DOCUMENT_DOES_NOT_EXIST:
                throw new DocumentDoesNotExist(status.message);
            case DOCUMENT_ALREADY_EXISTS:
                throw new DocumentAlreadyExists(status.message);
            default:
                throw new UnknownException(status.message);
        }
    }

    public Map<String, Double> multiGetTextSimilarity(List<String> centroidIds, String text) throws RelevancedException {
        MultiSimilarityResponse response;
        try {
            response = thriftClient_.multiGetTextSimilarity(centroidIds, text);
        } catch (TException err) {
            throw new ConnectionError(err);
        }
        handleResponseStatus(response.status);
        return response.scores;
    }

    public Double getTextSimilarity(String centroidId, String text) throws RelevancedException {
        SimilarityResponse response;
        try {
            response = thriftClient_.getTextSimilarity(centroidId, text);
        } catch (TException err) {
            throw new ConnectionError(err);
        }
        handleResponseStatus(response.status);
        return response.similarity;
    }

    public Double getDocumentSimilarity(String centroidId, String documentId) throws RelevancedException {
        SimilarityResponse response;
        try {
            response = thriftClient_.getDocumentSimilarity(centroidId, documentId);
        } catch (TException err) {
            throw new ConnectionError(err);
        }
        handleResponseStatus(response.status);
        return response.similarity;
    }

    public Double getCentroidSimilarity(String centroidId1, String centroidId2) throws RelevancedException {
        SimilarityResponse response;
        try {
            response = thriftClient_.getCentroidSimilarity(centroidId1, centroidId2);
        } catch (TException err) {
            throw new ConnectionError(err);
        }
        handleResponseStatus(response.status);
        return response.similarity;
    }

    public String createDocument(String text) throws RelevancedException {
        CrudResponse response;
        try {
            response = thriftClient_.createDocument(text);
        } catch (TException err) {
            throw new ConnectionError(err);
        }
        handleResponseStatus(response.status);
        return response.created;
    }

    public String createDocumentWithID(String id, String text) throws RelevancedException {
        CrudResponse response;
        try {
            response = thriftClient_.createDocumentWithID(id, text);
        } catch (TException err) {
            throw new ConnectionError(err);
        }
        handleResponseStatus(response.status);
        return response.created;
    }

    public Boolean deleteDocument(String documentId) throws RelevancedException {
        CrudResponse response;
        try {
            response = thriftClient_.deleteDocument(documentId);
        } catch (TException err) {
            throw new ConnectionError(err);
        }
        handleResponseStatus(response.status);
        return true;
    }

    public String createCentroid(String name) throws RelevancedException {
        CrudResponse response;
        try {
            response = thriftClient_.createCentroid(name);
        } catch (TException err) {
            throw new ConnectionError(err);
        }
        handleResponseStatus(response.status);
        return response.created;
    }

    public Boolean deleteCentroid(String name) throws RelevancedException {
        CrudResponse response;
        try {
            response = thriftClient_.deleteCentroid(name);
        } catch (TException err) {
            throw new ConnectionError(err);
        }
        handleResponseStatus(response.status);
        return true;
    }

    public Boolean addDocumentToCentroid(String centroidId, String documentId) throws RelevancedException {
        CrudResponse response;
        try {
            response = thriftClient_.addDocumentToCentroid(centroidId, documentId);
        } catch (TException err) {
            throw new ConnectionError(err);
        }
        handleResponseStatus(response.status);
        return true;
    }

    public Boolean removeDocumentFromCentroid(String centroidId, String documentId) throws RelevancedException {
        CrudResponse response;
        try {
            response = thriftClient_.removeDocumentFromCentroid(centroidId, documentId);
        } catch (TException err) {
            throw new ConnectionError(err);
        }
        handleResponseStatus(response.status);
        return true;
    }

    public List<String> listAllDocuments() throws RelevancedException {
        try {
            List<String> documents = thriftClient_.listAllDocuments();
            return documents;
        } catch (TException err) {
            throw new ConnectionError(err);
        }
    }

    public List<String> listAllCentroids() throws RelevancedException {
        try {
            List<String> centroids = thriftClient_.listAllCentroids();
            return centroids;
        } catch (TException err) {
            throw new ConnectionError(err);
        }
    }

    public List<String> listAllDocumentsForCentroid(String centroidId) throws RelevancedException {
        ListCentroidDocumentsResponse response;
        try {
            response = thriftClient_.listAllDocumentsForCentroid(centroidId);
        } catch (TException err) {
            throw new ConnectionError(err);
        }
        handleResponseStatus(response.status);
        return response.documents;
    }

}