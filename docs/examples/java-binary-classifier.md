## Binary Classifier (Java)

In this example, each of a small collection of Wikipedia articles is added to a relevanced server and assigned to the centroid `math`.  Similarity scores for another math-related article and an irrelevant article on Richard Gere are then calculated.

This example uses [jsoup](http://jsoup.org/) to crawl the Wikipedia pages and extract their text.

The slowest part of this code is actually fetching and crawling the Wikipedia pages, not interacting with the relevanced server.

For simplicity, this example ignores proper error handling.

```java
package org.relevanced.example;

import java.lang.StringBuffer;
import java.lang.System;
import java.io.IOException;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.select.Elements;

import org.relevanced.client.RelevancedBlockingClient;
import org.apache.thrift.TException;

public class Main {

    public static void main(String[] args) throws IOException, TException {
        RelevancedBlockingClient client = RelevancedBlockingClient.connect(
            "localhost", 8097
        );

        client.createCentroid("math");

        for (String url: mathUrls) {
            String urlText = fetchUrl(url);
            client.createDocumentWithID(url, urlText);
            client.addDocumentToCentroid("math", url);
        }

        client.joinCentroid("math");

        for (String url: testUrls) {
            String urlText = fetchUrl(url);
            Double similarity = client.getTextSimilarity("math", urlText);
            System.out.println(url + " : " + similarity);
        }
    }

    public static String fetchUrl(String url) throws IOException {
        Document doc = Jsoup.connect(url).get();
        Element mainText = doc.getElementById("mw-content-text");
        StringBuffer articleText = new StringBuffer();
        for (Element child: mainText.children()) {
            if (child.tagName() == "p") {
                articleText.append(child.text());
            }
        }
        return articleText.toString();
    }

    static final String mathUrls[] = {
        "https://en.wikipedia.org/wiki/Linear_programming",
        "https://en.wikipedia.org/wiki/Algorithm",
        "https://en.wikipedia.org/wiki/Scalar_multiplication",
        "https://en.wikipedia.org/wiki/Mathematical_structure",
        "https://en.wikipedia.org/wiki/Dot_product",
        "https://en.wikipedia.org/wiki/Analysis_of_algorithms",
        "https://en.wikipedia.org/wiki/Linear_algebra",
        "https://en.wikipedia.org/wiki/Criss-cross_algorithm",
        "https://en.wikipedia.org/wiki/Support_vector_machine",
        "https://en.wikipedia.org/wiki/Mathematics",
        "https://en.wikipedia.org/wiki/Rational_number",
        "https://en.wikipedia.org/wiki/Fraction_(mathematics)",
        "https://en.wikipedia.org/wiki/Square_root_of_2",
        "https://en.wikipedia.org/wiki/Mathematical_optimization",
        "https://en.wikipedia.org/wiki/Optimization_problem",
        "https://en.wikipedia.org/wiki/Candidate_solution",
        "https://en.wikipedia.org/wiki/Search_algorithm"
    };

    static final String testUrls[] = {
        "https://en.wikipedia.org/wiki/Matrix_multiplication",
        "https://en.wikipedia.org/wiki/Richard_Gere"
    };
}
```

As of 10/12/2015 (the article text may change), the result is:
```bash
[info] Running org.relevanced.example.Main
https://en.wikipedia.org/wiki/Matrix_multiplication : 0.4426147674201117
https://en.wikipedia.org/wiki/Richard_Gere : 0.07166738541810064
[success] Total time: 3 s, completed Oct 12, 2015 10:25:26 PM
```
