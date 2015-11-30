# JVM Clients

---

See the [detailed Java API documentation here.](../api/java_api.md)

---

## Installation

Relevanced has a Java client which can also be used from any other JVM language.  It's hosted on Sonatype OSS.

To add it to a Maven project, add the following dependency to your `pom.xml` file:
```xml
<dependency>
  <groupId>org.relevanced</groupId>
  <artifactId>client</artifactId>
  <version>0.9.5-SNAPSHOT</version>
</dependency>
```

To add it to an SBT project (Java or Scala), add the same dependency to the `libraryDependencies` in `build.sbt`.  You will also need to add the Sonatype OSS Snapshots repository to your `resolvers`:

```scala
resolvers +=
  "Sonatype OSS Snapshots" at "https://oss.sonatype.org/content/repositories/snapshots"

libraryDependencies ++= Seq(
   "org.relevanced" % "client" % "0.9.5-SNAPSHOT"
)

```

## Basic Use

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

## Examples

* [Java binary classifier](../examples/java-binary-classifier.md)
* [Java multi-class classifier](../examples/java-multiclass-classifier.md)
* [Scala binary classifier](../examples/scala-binary-classifier.md)

