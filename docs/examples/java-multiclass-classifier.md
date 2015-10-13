## Multiclass Classifier (Java)

Given a new document, we want to determine which one of out of several possible labels to apply to it.  This can be handled with a simple extension of the [binary classifier example](java-binary-classifier.md): by comparing the document's scores against each of a family of binary classifiers and comparing the scores, we can get an idea for which group the new document is closest to.

**relevanced** provides a `multiGetTextSimilarity` command which makes this kind of processing more efficient.  This command performs multiple similarity comparisons in a single network round-trip.  The document only has to be vectorized once and the individual comparisons are run in parallel on the server, both further improving throughput.

In this example we have three small collections of Wikipedia articles, chosen to represent the subjects `math`, `politics` and `medicine`.  We save each of these documents in **relevanced** and add them to their corresponding centroids.  After this we force the server to recompute all three centroids.  (The server will recompute the centroids on its own in a minute or two, but we're impatient.)

Once the centroids are calculated, we take three previously unseen documents from the same categories and determine the best label for each based on its similarity scores against the centroids.

This example uses [jsoup](http://jsoup.org/) to crawl the Wikipedia pages and extract their text.  The slowest part of this code is actually fetching and crawling the Wikipedia pages, not interacting with the relevanced server.

For simplicity, this example ignores proper error handling.

```java
package org.relevanced.example;

import java.lang.StringBuffer;
import java.lang.System;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.Arrays;
import java.util.List;
import java.util.ArrayList;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.select.Elements;

import org.relevanced.client.RelevancedBlockingClient;
import org.relevanced.client.gen_thrift_protocol.MultiSimilarityResponse;

import org.apache.thrift.TException;

public class Main {

    public static void main(String[] args) throws IOException, TException {
        Map<String, String[]> centroidUrls = new HashMap<String, String[]>();
        centroidUrls.put("math", mathUrls);
        centroidUrls.put("politics", politicalUrls);
        centroidUrls.put("medicine", medicalUrls);

        RelevancedBlockingClient client = RelevancedBlockingClient.connect(
            "localhost", 8097
        );

        for (Map.Entry<String, String[]> entry: centroidUrls.entrySet()) {
            String centroidId = entry.getKey();
            client.createCentroid(centroidId);
            for (String url: entry.getValue()) {
                String urlText = fetchUrl(url);
                client.createDocumentWithID(url, urlText);
                client.addDocumentToCentroid(centroidId, url);
            }
        }

        List<String> centroidNames = new ArrayList<String>(
            Arrays.asList("math", "politics", "medicine")
        );

        for (String key: centroidNames) {
            client.joinCentroid(key);
        }

        for (String url: testUrls) {
            String urlText = fetchUrl(url);
            MultiSimilarityResponse response = client.multiGetTextSimilarity(
                centroidNames, urlText
            );
            String closestCentroid = maxScoreKey(response.scores);
            System.out.println("Scores for '" + url + "' : -> " + closestCentroid);
            for (Map.Entry<String, Double> scoreEntry: response.scores.entrySet()) {
                System.out.println("\t" + scoreEntry.getKey() + " : " + scoreEntry.getValue());
            }
            System.out.println("---");
        }
    }

    public static String maxScoreKey(Map<String, Double> scores) {
        double currentMaxVal = 0.0;
        String currentMaxKey = "";
        for (Map.Entry<String, Double> entry: scores.entrySet()) {
            if (entry.getValue() > currentMaxVal) {
                currentMaxVal = entry.getValue();
                currentMaxKey = entry.getKey();
            }
        }
        return currentMaxKey;
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

    static final String politicalUrls[] = {
        "https://en.wikipedia.org/wiki/Sovereign_state",
        "https://en.wikipedia.org/wiki/Executive_(government)",
        "https://en.wikipedia.org/wiki/Government",
        "https://en.wikipedia.org/wiki/Politics",
        "https://en.wikipedia.org/wiki/Federalism",
        "https://en.wikipedia.org/wiki/Separation_of_powers",
        "https://en.wikipedia.org/wiki/Autocracy",
        "https://en.wikipedia.org/wiki/Democracy",
        "https://en.wikipedia.org/wiki/Centralized_government",
        "https://en.wikipedia.org/wiki/Citizenship",
        "https://en.wikipedia.org/wiki/Public_interest",
        "https://en.wikipedia.org/wiki/Republic",
        "https://en.wikipedia.org/wiki/Political_philosophy"
    };

    static final String medicalUrls[] = {
        "https://en.wikipedia.org/wiki/Cardiovascular_disease",
        "https://en.wikipedia.org/wiki/Antibiotics",
        "https://en.wikipedia.org/wiki/Aortic_aneurysm",
        "https://en.wikipedia.org/wiki/Medical_ultrasound",
        "https://en.wikipedia.org/wiki/Atherosclerosis",
        "https://en.wikipedia.org/wiki/Myocardial_infarction",
        "https://en.wikipedia.org/wiki/Coronary_artery_disease",
        "https://en.wikipedia.org/wiki/Diabetes_mellitus",
        "https://en.wikipedia.org/wiki/Metabolic_disorder",
        "https://en.wikipedia.org/wiki/Mitochondrial_disease",
        "https://en.wikipedia.org/wiki/Anemia"
    };

    static final String testUrls[] = {
        "https://en.wikipedia.org/wiki/Matrix_multiplication",
        "https://en.wikipedia.org/wiki/Constitutional_republic",
        "https://en.wikipedia.org/wiki/Vitamin_B12_deficiency"
    };

}

```


```bash
[info] Running org.relevanced.example.Main
Scores for 'https://en.wikipedia.org/wiki/Matrix_multiplication' : -> math
    politics : 0.12419293183746599
    medicine : 0.12986056258156387
    math : 0.4426147674201117
---
Scores for 'https://en.wikipedia.org/wiki/Constitutional_republic' : -> politics
    politics : 0.6546655597699288
    medicine : 0.16418704912570423
    math : 0.16907599961042782
---
Scores for 'https://en.wikipedia.org/wiki/Vitamin_B12_deficiency' : -> medicine
    politics : 0.12580253811958586
    medicine : 0.3630783617149443
    math : 0.11827175582711216
---
[success] Total time: 4 s, completed Oct 12, 2015 10:59:16 PM

```