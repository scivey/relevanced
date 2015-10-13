// package org.relevanced.client;

// import java.lang.System;
// import java.util.ArrayList;
// import java.util.List;
// import java.util.Map;
// import java.util.Random;
// import org.apache.thrift.TException;
// import org.relevanced.client.gen_thrift_protocol.ECentroidDoesNotExist;
// import org.relevanced.client.gen_thrift_protocol.EDocumentDoesNotExist;

// public class Main {
//     public static void main(String [] args) {
//         System.out.println("start");
//         try {
//             RelevancedBlockingClient client = RelevancedBlockingClient.connect("localhost", 8097);
//             List<String> centroids = client.listAllCentroids().centroids;
//             System.out.println("number of centroids: " + centroids.size());
//             System.out.println(centroids);
//             List<String> documents = client.listAllDocuments().documents;
//             System.out.println("number of documents: " + documents.size());
//             int docIndex = new Random().nextInt(documents.size());
//             String docId = documents.get(docIndex);
//             String centroidId = centroids.get(0);
//             System.out.println("comparing " + docId + " against " + centroidId + "...");
//             double similarity = client.getDocumentSimilarity(centroidId, docId);
//             System.out.println("result: " + similarity);
//         } catch (TException err) {
//             err.printStackTrace();
//         }
//         System.out.println("end");
//     }
// }
