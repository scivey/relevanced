## Binary Classifier (Scala)

In this example, each of a small collection of Wikipedia articles is added to a relevanced server and assigned to the centroid `math`.  Similarity scores for another math-related article and an irrelevant article on Richard Gere are then calculated.

This example uses [jsoup](http://jsoup.org/) to crawl the Wikipedia pages and extract their text.

The slowest part of this code is actually fetching and crawling the Wikipedia pages, not interacting with the relevanced server.

For simplicity, this example uses the synchronous client instead of diving into Scala async internals.

```scala
package org.relevanced.example
import scala.collection.JavaConverters._
import org.relevanced.client.RelevancedBlockingClient
import org.jsoup.Jsoup

object Main {

  def main(args: Array[String]) {
    val client = RelevancedBlockingClient.connect("localhost", 8097)
    client.createCentroid("math")

    val urlTextPairs = mathUrls.zip(mathUrls.map(ArticleFetcher.fetch))

    for (pair <- urlTextPairs) {
      val (url, text) = pair
      client.createDocumentWithID(url, text)
      client.addDocumentToCentroid("math", url)
    }

    client.joinCentroid("math")

    for(url <- testUrls) {
      val text = ArticleFetcher.fetch(url)
      val similarity = client.getTextSimilarity("math", text)
      println(url + " : " + similarity)
    }
  }

  val mathUrls = Seq(
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
  )

  val testUrls = Seq(
    "https://en.wikipedia.org/wiki/Matrix_multiplication",
    "https://en.wikipedia.org/wiki/Richard_Gere"
  )

}

object ArticleFetcher {
  def fetch(url: String): String = {
    val doc = Jsoup.connect(url).get()
    doc
      .getElementById("mw-content-text")
      .children
      .iterator.asScala
      .filter { elem => elem.tagName == "p" }
      .map { p => p.text }
      .toSeq
      .mkString("\n")
  }
}
```

As of 10/12/2015 (the article text may change), the result is:
```bash
[info] Running org.relevanced.example.Main
https://en.wikipedia.org/wiki/Matrix_multiplication : 0.44406084104930565
https://en.wikipedia.org/wiki/Richard_Gere : 0.07172603262303778
[success] Total time: 3 s, completed Oct 12, 2015 7:59:32 PM
```
