## Binary Classifier (Javascript)

In this example, each of a small collection of Wikipedia articles is added to a relevanced server and assigned to the centroid `math`.  Similarity scores for another math-related article and an irrelevant article on Richard Gere are then calculated.

The slowest part of this script is actually fetching and crawling the Wikipedia pages, not interacting with the relevanced server.

This example uses [request](https://github.com/request/request) to fetch the HTML documents from Wikipedia.  It then uses [cheerio](https://github.com/cheeriojs/cheerio), a server-side version of jQuery, to extract the main article text from those documents.  [bluebird](https://github.com/petkaantonov/bluebird)'s Promise implementation is used for general async coordination.


```javascript
var RelevancedClient = require('relevanced-client').RelevancedClient;
var Promise = require('bluebird');
var request = require('request');
var cheerio = require('cheerio');

var MATH_URLS = [
    'https://en.wikipedia.org/wiki/Linear_programming',
    'https://en.wikipedia.org/wiki/Algorithm',
    'https://en.wikipedia.org/wiki/Scalar_multiplication',
    'https://en.wikipedia.org/wiki/Mathematical_structure',
    'https://en.wikipedia.org/wiki/Dot_product',
    'https://en.wikipedia.org/wiki/Analysis_of_algorithms',
    'https://en.wikipedia.org/wiki/Linear_algebra',
    'https://en.wikipedia.org/wiki/Criss-cross_algorithm',
    'https://en.wikipedia.org/wiki/Support_vector_machine',
    'https://en.wikipedia.org/wiki/Mathematics',
    'https://en.wikipedia.org/wiki/Rational_number',
    'https://en.wikipedia.org/wiki/Fraction_(mathematics)',
    'https://en.wikipedia.org/wiki/Square_root_of_2',
    'https://en.wikipedia.org/wiki/Mathematical_optimization',
    'https://en.wikipedia.org/wiki/Optimization_problem',
    'https://en.wikipedia.org/wiki/Candidate_solution',
    'https://en.wikipedia.org/wiki/Search_algorithm'
];

var MATH_TEST_URL = 'https://en.wikipedia.org/wiki/Matrix_multiplication';
var IRRELEVANT_TEST_URL = 'https://en.wikipedia.org/wiki/Richard_Gere';

var main = function() {
    var client = new RelevancedClient('localhost', 8097);
    client.connect().then(function() {

        // create the centroid
        var createCentroid = client.createCentroid('math');

        // extract article text from each Wikipedia URL
        // and add it to the relevanced server
        var createDocuments = Promise
            .all(MATH_URLS)
            .map(getWikipediaArticle)
            .map(function(article) {
                return client.createDocumentWithID(
                    article.url, article.text
                ).then(function() {
                    return article.url;
                });
            });

        return Promise.all([createCentroid, createDocuments]);
    }).then(function(results) {
        // add all the new documents to the `math` centroid
        var docIds = results[1];
        return Promise.all(docIds).map(function(id) {
            return client.addDocumentToCentroid('math', id);
        });
    }).then(function() {
        // make sure the centroid is up to date
        // after all the recent insertions.
        return client.joinCentroid('math');
    }).then(function() {
        // extract article text for the two test articles
        return Promise.all([MATH_TEST_URL, IRRELEVANT_TEST_URL])
            .map(getWikipediaArticle);
    }).each(function(testArticle) {
        // get + print similarity score for each test
        // article against the `math` centroid.
        return client
            .getTextSimilarity('math', testArticle.text)
            .then(function(score) {
                console.log(testArticle.url, ' : ', score);
            });
    }).then(function() {
        client.disconnect();
    });
};

var getWikipediaArticle = function(url) {
    return new Promise(function(resolve, reject) {
        request(url, function(err, response, body) {
            if (err) return reject(err);
            var $ = cheerio.load(body);
            var $mainText = $('#mw-content-text');
            var $paras = $mainText.children('p');
            var paragraphs = [];
            $paras.each(function(elem) {
                var $this = $(this);
                paragraphs.push($this.text());
            });
            return resolve({
                url: url,
                text: paragraphs.join('\n')
            });
        });
    });
};

main();

```

As of 10/11/2015 (the article text may change), the result is:
```bash
https://en.wikipedia.org/wiki/Matrix_multiplication  :  0.4440614023877249
https://en.wikipedia.org/wiki/Richard_Gere  :  0.07171829911780646
```
