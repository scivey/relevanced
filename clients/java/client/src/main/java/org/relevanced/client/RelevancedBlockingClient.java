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

public class RelevancedBlockingClient {
    public Relevanced.Client thriftClient_;
    public TTransport thriftTransport_;
    public TProtocol thriftProtocol_;
    public String hostname_;
    public int portNum_;

    public RelevancedBlockingClient(String host, int port) {
        hostname_ = host;
        portNum_ = port;
    }

    public void connect() throws TException {
        try {
            thriftTransport_ = new TSocket(hostname_, portNum_);
            thriftTransport_.open();
            thriftProtocol_ = new TBinaryProtocol(thriftTransport_);
            thriftClient_ = new Relevanced.Client(thriftProtocol_);
        } catch (TException err) {
            err.printStackTrace();
            throw err;
        }
    }

    public Map<String, Double> multiGetTextSimilarity(List<String> centroidIds, String text) throws TException {
        MultiSimilarityResponse response;
        response = thriftClient_.multiGetTextSimilarity(centroidIds, text);
        return response.scores;
    }

    public Double getTextSimilarity(String centroidId, String text) throws TException {
        SimilarityResponse response;
        response = thriftClient_.getTextSimilarity(centroidId, text);
        return response.similarity;
    }

    public Double getDocumentSimilarity(String centroidId, String documentId) throws TException {
        SimilarityResponse response;
        response = thriftClient_.getDocumentSimilarity(centroidId, documentId);
        return response.similarity;
    }

    public Double getCentroidSimilarity(String centroidId1, String centroidId2) throws TException {
        SimilarityResponse response;
        response = thriftClient_.getCentroidSimilarity(centroidId1, centroidId2);
        return response.similarity;
    }

    public String createDocument(String text) throws TException {
        CrudResponse response;
        response = thriftClient_.createDocument(text);
        return response.id;
    }

    public String createDocumentWithID(String id, String text) throws TException {
        CrudResponse response;
        response = thriftClient_.createDocumentWithID(id, text);
        return response.id;
    }

    public String deleteDocument(String documentId) throws TException {
        CrudResponse response;
        response = thriftClient_.deleteDocument(documentId);
        return response.id;
    }

    public String createCentroid(String name) throws TException {
        CrudResponse response;
        response = thriftClient_.createCentroid(name);
        return response.id;
    }

    public String deleteCentroid(String name) throws TException {
        CrudResponse response;
        response = thriftClient_.deleteCentroid(name);
        return response.id;
    }

    public Boolean addDocumentToCentroid(String centroidId, String documentId) throws TException {
        CrudResponse response;
        response = thriftClient_.addDocumentToCentroid(centroidId, documentId);
        return true;
    }

    public Boolean removeDocumentFromCentroid(String centroidId, String documentId) throws TException {
        CrudResponse response;
        response = thriftClient_.removeDocumentFromCentroid(centroidId, documentId);
        return true;
    }

    public List<String> listAllDocuments() throws TException {
        List<String> documents = thriftClient_.listAllDocuments();
        return documents;
    }

    public List<String> listAllCentroids() throws TException {
        return thriftClient_.listAllCentroids();
    }

    public List<String> listAllDocumentsForCentroid(String centroidId) throws TException {
        ListCentroidDocumentsResponse response;
        response = thriftClient_.listAllDocumentsForCentroid(centroidId);
        return response.documents;
    }

    public Map<String, String> getServerMetadata() throws TException {
        return thriftClient_.getServerMetadata();
    }

}