package org.relevanced.client;

import java.lang.System;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Random;
import org.apache.thrift.TException;

public class Main {
    public static void main(String [] args) {
        System.out.println("start");
        try {
            RelevancedBlockingClient client = new RelevancedBlockingClient("localhost", 8097);
            client.connect();
            List<String> centroids = client.listAllCentroids();
            System.out.println("number of centroids: " + centroids.size());
            System.out.println(centroids);
            List<String> documents = client.listAllDocuments();
            System.out.println("number of documents: " + documents.size());
            int docIndex = new Random().nextInt(documents.size());
            String docId = documents.get(docIndex);
            String centroidId = centroids.get(0);
            System.out.println("comparing " + docId + " against " + centroidId + "...");
            double similarity = client.getDocumentSimilarity(centroidId, docId);
            System.out.println("result: " + similarity);

        } catch (TException err) {
            err.printStackTrace();
        }
        System.out.println("end");
    }
}