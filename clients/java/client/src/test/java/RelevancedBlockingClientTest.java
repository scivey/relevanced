import static org.junit.Assert.assertEquals;
import org.junit.Test;
import org.relevanced.client.RelevancedBlockingClient;
import org.apache.thrift.TException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.Random;

public class RelevancedBlockingClientTest {
    private void eraseEverything(RelevancedBlockingClient client) throws TException {
        List<String> documentIds = client.listAllDocuments().documents;
        List<String> centroidIds = client.listAllCentroids().centroids;
        Boolean ignoreMissing = true;
        client.multiDeleteCentroids(centroidIds, ignoreMissing);
        client.multiDeleteDocuments(documentIds, ignoreMissing);
    }

    @Test
    public void testCentroidCRUD() throws TException {
        RelevancedBlockingClient client = RelevancedBlockingClient.connect("localhost", 8097);
        eraseEverything(client);
        client.createCentroid("something");
        List<String> centroids = client.listAllCentroids().centroids;
        List<String> expected = new ArrayList<String>();
        expected.add("something");
        assertEquals(expected, centroids);

        List<String> toCreate = new ArrayList<String>();
        toCreate.add("c1");
        toCreate.add("c2");
        toCreate.add("c3");
        toCreate.add("c4");
        client.multiCreateCentroids(toCreate);

        centroids = client.listAllCentroids().centroids;
        expected.clear();
        expected.add("c1");
        expected.add("c2");
        expected.add("c3");
        expected.add("c4");
        expected.add("something");
        assertEquals(expected, centroids);

        client.deleteCentroid("c2");
        centroids = client.listAllCentroids().centroids;
        expected.clear();
        expected.add("c1");
        expected.add("c3");
        expected.add("c4");
        expected.add("something");
        assertEquals(expected, centroids);

        List<String> toDelete = new ArrayList<String>();
        toDelete.add("c3");
        toDelete.add("something");
        client.multiDeleteCentroids(toDelete);

        centroids = client.listAllCentroids().centroids;
        expected.clear();
        expected.add("c1");
        expected.add("c4");
        assertEquals(expected, centroids);
    }

    @Test
    public void testDocumentCRUD() throws TException {
        RelevancedBlockingClient client = RelevancedBlockingClient.connect("localhost", 8097);
        eraseEverything(client);
        client.createDocumentWithID("doc-1", "some text");
        String createdID = client.createDocument("some other text").id;
        List<String> docs = client.listAllDocuments().documents;
        List<String> expected = new ArrayList<String>();
        expected.add("doc-1");
        expected.add(createdID);
        Collections.sort(expected);
        assertEquals(expected, docs);

        client.deleteDocument(createdID);
        expected.clear();
        expected.add("doc-1");
        docs = client.listAllDocuments().documents;
        assertEquals(expected, docs);

        client.createDocumentWithID("doc-2", "more text");
        client.createDocumentWithID("doc-3", "more text");
        client.createDocumentWithID("doc-4", "more text");

        docs = client.listAllDocuments().documents;
        expected.clear();
        expected.add("doc-1");
        expected.add("doc-2");
        expected.add("doc-3");
        expected.add("doc-4");
        assertEquals(expected, docs);

        List<String> toDelete = new ArrayList<String>();
        toDelete.add("doc-2");
        toDelete.add("doc-4");
        client.multiDeleteDocuments(toDelete);

        docs = client.listAllDocuments().documents;
        expected.clear();
        expected.add("doc-1");
        expected.add("doc-3");
        assertEquals(expected, docs);
    }

    @Test
    public void testCentroidDocumentOperations() throws TException {
        RelevancedBlockingClient client = RelevancedBlockingClient.connect("localhost", 8097);
        eraseEverything(client);
        client.createCentroid("c1");
        client.createCentroid("c2");
        client.createDocumentWithID("d1", "some text");
        client.createDocumentWithID("d2", "some text");
        client.createDocumentWithID("d3", "some text");

        List<String> expectedCentroids = new ArrayList<String>();
        List<String> expectedDocs = new ArrayList<String>();
        expectedCentroids.add("c1");
        expectedCentroids.add("c2");
        expectedDocs.add("d1");
        expectedDocs.add("d2");
        expectedDocs.add("d3");
        assertEquals(expectedCentroids, client.listAllCentroids().centroids);
        assertEquals(expectedDocs, client.listAllDocuments().documents);

        client.addDocumentToCentroid("c1", "d2");
        expectedDocs.clear();
        expectedDocs.add("d2");
        assertEquals(expectedDocs, client.listAllDocumentsForCentroid("c1").documents);
    }
}
