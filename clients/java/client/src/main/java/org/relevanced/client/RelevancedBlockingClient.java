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

import org.relevanced.client.gen_thrift_protocol.Relevanced;
import org.relevanced.client.gen_thrift_protocol.Language;
import org.relevanced.client.gen_thrift_protocol.CreateDocumentResponse;
import org.relevanced.client.gen_thrift_protocol.MultiSimilarityResponse;


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

    public double getTextSimilarity(String centroidId, String text) throws TException {
        return getTextSimilarity(centroidId, text, Language.EN);
    }

    public MultiSimilarityResponse multiGetTextSimilarity(List<String> centroidIds, String text) throws TException {
        return multiGetTextSimilarity(centroidIds, text, Language.EN);
    }

    public CreateDocumentResponse createDocument(String text) throws TException {
        return createDocument(text, Language.EN);
    }

    public CreateDocumentResponse createDocumentWithID(String id, String text) throws TException {
        return createDocumentWithID(id, text, Language.EN);
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