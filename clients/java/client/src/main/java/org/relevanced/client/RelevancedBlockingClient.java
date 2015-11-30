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

import org.relevanced.client.protocol.Relevanced;
import org.relevanced.client.protocol.Language;
import org.relevanced.client.protocol.AddDocumentsToCentroidRequest;
import org.relevanced.client.protocol.AddDocumentsToCentroidResponse;
import org.relevanced.client.protocol.RemoveDocumentsFromCentroidRequest;
import org.relevanced.client.protocol.RemoveDocumentsFromCentroidResponse;
import org.relevanced.client.protocol.CreateCentroidRequest;
import org.relevanced.client.protocol.CreateCentroidResponse;
import org.relevanced.client.protocol.CreateDocumentResponse;
import org.relevanced.client.protocol.DeleteCentroidRequest;
import org.relevanced.client.protocol.DeleteCentroidResponse;
import org.relevanced.client.protocol.DeleteDocumentRequest;
import org.relevanced.client.protocol.DeleteDocumentResponse;
import org.relevanced.client.protocol.MultiDeleteDocumentsRequest;
import org.relevanced.client.protocol.MultiDeleteDocumentsResponse;
import org.relevanced.client.protocol.JoinCentroidRequest;
import org.relevanced.client.protocol.JoinCentroidResponse;
import org.relevanced.client.protocol.MultiJoinCentroidsRequest;
import org.relevanced.client.protocol.MultiJoinCentroidsResponse;
import org.relevanced.client.protocol.MultiCreateCentroidsRequest;
import org.relevanced.client.protocol.MultiCreateCentroidsResponse;
import org.relevanced.client.protocol.MultiDeleteCentroidsRequest;
import org.relevanced.client.protocol.MultiDeleteCentroidsResponse;
import org.relevanced.client.protocol.MultiSimilarityResponse;

public class RelevancedBlockingClient extends Relevanced.Client {
    public Relevanced.Client thriftClient_;
    public TTransport thriftTransport_;
    public TProtocol thriftProtocol_;
    public String hostname_;
    public int portNum_;

    public RelevancedBlockingClient(String host, int port, TProtocol protocol, TTransport transport) {
        super(protocol);
        hostname_ = host;
        portNum_ = port;
        thriftProtocol_ = protocol;
        thriftTransport_ = transport;
    }

    public AddDocumentsToCentroidResponse addDocumentsToCentroid(String centroidId, List<String> documentIds, Boolean ignoreAlreadyInCentroid) throws TException {
        AddDocumentsToCentroidRequest request = new AddDocumentsToCentroidRequest();
        request.centroidId = centroidId;
        request.documentIds = documentIds;
        request.ignoreAlreadyInCentroid = ignoreAlreadyInCentroid;
        return addDocumentsToCentroid(request);
    }

    public AddDocumentsToCentroidResponse addDocumentsToCentroid(String centroidId, List<String> docIds) throws TException {
        return addDocumentsToCentroid(centroidId, docIds, false);
    }

    public AddDocumentsToCentroidResponse addDocumentToCentroid(String centroidId, String documentId, Boolean ignoreAlreadyInCentroid) throws TException {
        List<String> documentIds = new ArrayList<String>();
        documentIds.add(documentId);
        return addDocumentsToCentroid(centroidId, documentIds, ignoreAlreadyInCentroid);
    }

    public AddDocumentsToCentroidResponse addDocumentToCentroid(String centroidId, String documentId) throws TException {
        return addDocumentToCentroid(centroidId, documentId, false);
    }

    public CreateCentroidResponse createCentroid(String id, Boolean ignoreExisting) throws TException {
        CreateCentroidRequest request = new CreateCentroidRequest();
        request.id = id;
        request.ignoreExisting = ignoreExisting;
        return createCentroid(request);
    }

    public CreateCentroidResponse createCentroid(String id) throws TException {
        return createCentroid(id, false);
    }

    public CreateDocumentResponse createDocument(String text) throws TException {
        return createDocument(text, Language.EN);
    }

    public CreateDocumentResponse createDocumentWithID(String id, String text) throws TException {
        return createDocumentWithID(id, text, Language.EN);
    }

    public DeleteCentroidResponse deleteCentroid(String id, Boolean ignoreMissing) throws TException {
        DeleteCentroidRequest request = new DeleteCentroidRequest();
        request.id = id;
        request.ignoreMissing = ignoreMissing;
        return deleteCentroid(request);
    }

    public DeleteCentroidResponse deleteCentroid(String id) throws TException {
        return deleteCentroid(id, false);
    }

    public DeleteDocumentResponse deleteDocument(String id, Boolean ignoreMissing) throws TException {
        DeleteDocumentRequest request = new DeleteDocumentRequest();
        request.id = id;
        request.ignoreMissing = ignoreMissing;
        return deleteDocument(request);
    }

    public DeleteDocumentResponse deleteDocument(String id) throws TException {
        return deleteDocument(id, false);
    }

    public double getTextSimilarity(String centroidId, String text) throws TException {
        return getTextSimilarity(centroidId, text, Language.EN);
    }

    public JoinCentroidResponse joinCentroid(String id, Boolean ignoreMissing) throws TException {
        JoinCentroidRequest request = new JoinCentroidRequest();
        request.id = id;
        request.ignoreMissing = ignoreMissing;
        return joinCentroid(request);
    }

    public JoinCentroidResponse joinCentroid(String id) throws TException {
        return joinCentroid(id, false);
    }

    public MultiCreateCentroidsResponse multiCreateCentroids(List<String> centroidIds, Boolean ignoreExisting) throws TException {
        MultiCreateCentroidsRequest request = new MultiCreateCentroidsRequest();
        request.ids = centroidIds;
        request.ignoreExisting = ignoreExisting;
        return multiCreateCentroids(request);
    }

    public MultiCreateCentroidsResponse multiCreateCentroids(List<String> centroidIds) throws TException {
        return multiCreateCentroids(centroidIds, false);
    }

    public MultiDeleteCentroidsResponse multiDeleteCentroids(List<String> centroidIds, Boolean ignoreMissing) throws TException {
        MultiDeleteCentroidsRequest request = new MultiDeleteCentroidsRequest();
        request.ids = centroidIds;
        request.ignoreMissing = ignoreMissing;
        return multiDeleteCentroids(request);
    }

    public MultiDeleteCentroidsResponse multiDeleteCentroids(List<String> centroidIds) throws TException {
        return multiDeleteCentroids(centroidIds, false);
    }

    public MultiDeleteDocumentsResponse multiDeleteDocuments(List<String> documentIds, Boolean ignoreMissing) throws TException {
        MultiDeleteDocumentsRequest request = new MultiDeleteDocumentsRequest();
        request.ids = documentIds;
        request.ignoreMissing = ignoreMissing;
        return multiDeleteDocuments(request);
    }

    public MultiDeleteDocumentsResponse multiDeleteDocuments(List<String> documentIds) throws TException {
        return multiDeleteDocuments(documentIds, false);
    }

    public MultiSimilarityResponse multiGetTextSimilarity(List<String> centroidIds, String text) throws TException {
        return multiGetTextSimilarity(centroidIds, text, Language.EN);
    }

    public MultiJoinCentroidsResponse multiJoinCentroids(List<String> centroidIds, Boolean ignoreMissing) throws TException {
        MultiJoinCentroidsRequest request = new MultiJoinCentroidsRequest();
        request.ids = centroidIds;
        request.ignoreMissing = ignoreMissing;
        return multiJoinCentroids(request);
    }

    public MultiJoinCentroidsResponse multiJoinCentroids(List<String> centroidIds) throws TException {
        return multiJoinCentroids(centroidIds, false);
    }

    public RemoveDocumentsFromCentroidResponse removeDocumentsFromCentroid(String centroidId, List<String> docIds, Boolean ignoreNotInCentroid) throws TException {
        RemoveDocumentsFromCentroidRequest request = new RemoveDocumentsFromCentroidRequest();
        request.centroidId = centroidId;
        request.documentIds = docIds;
        request.ignoreNotInCentroid = ignoreNotInCentroid;
        return removeDocumentsFromCentroid(request);
    }

    public RemoveDocumentsFromCentroidResponse removeDocumentsFromCentroid(String centroidId, List<String> docIds) throws TException {
        return removeDocumentsFromCentroid(centroidId, docIds, false);
    }

    public RemoveDocumentsFromCentroidResponse removeDocumentFromCentroid(String centroidId, String documentId, Boolean ignoreNotInCentroid) throws TException {
        List<String> documentIds = new ArrayList<String>();
        documentIds.add(documentId);
        return removeDocumentsFromCentroid(centroidId, documentIds, ignoreNotInCentroid);
    }

    public RemoveDocumentsFromCentroidResponse removeDocumentFromCentroid(String centroidId, String documentId) throws TException {
        return removeDocumentFromCentroid(centroidId, documentId, false);
    }

    public static RelevancedBlockingClient connect(String host, int port) throws TException {
        try {
            TTransport transport = new TSocket(host, port);
            transport.open();
            TProtocol protocol = new TBinaryProtocol(transport);
            return new RelevancedBlockingClient(host, port, protocol, transport);
        } catch (TException err) {
            err.printStackTrace();
            throw err;
        }
    }

}