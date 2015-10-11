## Binary Classifier (Python)

In this example, each of a small collection of Wikipedia articles is added to a relevanced server and assigned to the centroid `math`.  Similarity scores for another math-related article and an irrelevant article on Richard Gere are then calculated.

This example uses [python-goose](https://github.com/grangier/python-goose) to crawl the Wikipedia pages and extract their text.

If you run this, note that by far the slowest part of the script is fetching and crawling HTML pages.  To get a better idea for relevanced's raw speed, you can run one of the [functional tests in the GitHub repository](https://github.com/scivey/relevanced/blob/master/func_test/main.py).  This test caches the crawled pages in a memcached instance running on localhost, so communication with relevanced is the only bottleneck after the first run.

```python
from __future__ import print_function
MATH_URLS = [
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
]

MATH_TEST_URL = 'https://en.wikipedia.org/wiki/Matrix_multiplication'
IRRELEVANT_TEST_URL = 'https://en.wikipedia.org/wiki/Richard_Gere'

from goose import Goose
from relevanced_client import Client

def main():
    client = Client('localhost', 8097)

    for url in MATH_URLS:
        article = Goose().extract(url=url)
        client.create_document_with_id(url, article.cleaned_text)

    client.create_centroid('math')
    for url in MATH_URLS:
        client.add_document_to_centroid('math', url)

    # the centroid will be automatically computed, but
    # for this example we want to be sure that it's up to date
    client.join_centroid('math')

    math_text = Goose().extract(url=MATH_TEST_URL).cleaned_text
    math_similarity = client.get_text_similarity('math', math_text)
    print("Similarity for math article: \t%r" % math_similarity)

    irrelevant_text = Goose().extract(url=IRRELEVANT_TEST_URL).cleaned_text
    irrelevant_similarity = client.get_text_similarity('math', irrelevant_text)
    print("Similarity for richard gere: \t%r" % irrelevant_similarity)

```

As of 10/01/2015 (the articles may be changed), the result is:
```bash
Similarity for math article:    0.4526966705497888
Similarity for richard gere:    0.07232063152615818
```
