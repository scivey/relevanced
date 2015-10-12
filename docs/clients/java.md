# Java Client

## Installation

The Java client [exists](https://github.com/scivey/relevanced/tree/master/clients/java/client), but is not yet easily accessible.  It will be available from [Maven Central](http://search.maven.org/) as soon as Sonatype OSSRH approves creation of the `org.relevanced.client` repository.

## API

The Java client API has the same commands described in the [language-agnostic command overview](../commands.md), and follows the same `camelCase` naming convention.  In the case of the synchronous client (`RelevancedBlockingClient`), the semantics are exactly identical to the description in that document.

Basic use of the blocking client:

```java
package org.relevanced.example;
import java.lang.System;
import org.apache.thrift.TException;
import org.relevanced.client.RelevancedBlockingClient;

public class Main {
    public static void main(String [] args) {
        try {
            RelevancedBlockingClient client = RelevancedBlockingClient.connect(
                "localhost", 8097
            );
            client.createCentroid("centroid-id");
            client.createDocumentWithId("doc-id", "This is some relevant text");
            client.addDocumentToCentroid("centroid-id", "doc-id");
            client.joinCentroid("centroid-id");
            double textSimilarity = client.getTextSimilarity("centroid-id",
                "This is a potentially relevant document."
            );
            System.out.println("similarity: " + textSimilarity);
        } catch (TException err) {
            err.printStackTrace();
        }
    }
}
```
