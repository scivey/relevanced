## Multiclass Classifier (Python)

Given a new document, we want to determine which one of out of several possible labels to apply to it.  This can be handled with a simple extension of the [binary classifier example](python-binary-classifier.md): by comparing the document's scores against each of a family of binary classifiers and comparing the scores, we can get an idea for which group the new document is closest to.

**relevanced** provides a `multi_get_text_similarity` command which makes this kind of processing more efficient.  This command performs multiple similarity comparisons in a single network round-trip.  The document only has to be vectorized once and the individual comparisons are run in parallel on the server, both further improving throughput.

In this example we have three small collections of Wikipedia articles, chosen to represent the subjects `math`, `politics` and `medicine`.  We save each of these documents in **relevanced** and add them to their corresponding centroids.  After this we force the server to recompute all three centroids.  (The server will recompute the centroids on its own in a minute or two, but we're impatient.)

Once the centroids are calculated, we take three previously unseen documents from the same categories and determine the best label for each based on its similarity scores against the centroids.

This example uses [python-goose](https://github.com/grangier/python-goose) to crawl the Wikipedia pages and extract their text.

If you run this, note that by far the slowest part of the script is fetching and crawling HTML pages.  To get a better idea for relevanced's raw speed, you can run one of the [functional tests in the GitHub repository](https://github.com/scivey/relevanced/blob/master/func_test/main.py).  This test caches the crawled pages in a memcached instance running on localhost, so communication with relevanced is the only bottleneck after the first run.

```python
from __future__ import print_function
from pprint import pprint

MATH_TEST_URL = 'https://en.wikipedia.org/wiki/Matrix_multiplication'
POLITICAL_TEST_URL = 'https://en.wikipedia.org/wiki/Constitutional_republic'
MEDICAL_TEST_URL = 'https://en.wikipedia.org/wiki/Vitamin_B12_deficiency'

from goose import Goose
from relevanced_client import Client

def get_max_key(scores):
    pairs = scores.items()
    pairs.sort(key=lambda x: x[1])
    return pairs[-1][0]

def main():
    client = Client('localhost', 8097)
    client.create_centroid('math')
    client.create_centroid('politics')
    client.create_centroid('medicine')

    centroid_url_pairs = [
        ('math', MATH_URLS),
        ('politics', POLITICAL_URLS),
        ('medicine', MEDICAL_URLS)
    ]

    for centroid_name, urls in centroid_url_pairs:
        for url in urls:
            text = Goose().extract(url=url).cleaned_text
            client.create_document_with_id(url, text)
            client.add_document_to_centroid(centroid_name, url)

    for centroid_name, _ in centroid_url_pairs:
        # not normally necessary, but with a new
        # centroid we want to be sure it's up to date
        client.recompute_centroid(centroid_name)

    for url in [MATH_TEST_URL, POLITICAL_TEST_URL, MEDICAL_TEST_URL]:
        text = Goose().extract(url=url).cleaned_text
        scores = client.multi_get_text_similarity(
            ['math', 'politics', 'medicine'], text
        )
        label = get_max_key(scores)
        print('Scores for %s :  -->  %s' % (url, label))
        pprint(scores)


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

POLITICAL_URLS = [
    'https://en.wikipedia.org/wiki/Sovereign_state',
    'https://en.wikipedia.org/wiki/Executive_(government)',
    'https://en.wikipedia.org/wiki/Government',
    'https://en.wikipedia.org/wiki/Politics',
    'https://en.wikipedia.org/wiki/Federalism',
    'https://en.wikipedia.org/wiki/Separation_of_powers',
    'https://en.wikipedia.org/wiki/Autocracy',
    'https://en.wikipedia.org/wiki/Democracy',
    'https://en.wikipedia.org/wiki/Centralized_government',
    'https://en.wikipedia.org/wiki/Citizenship',
    'https://en.wikipedia.org/wiki/Public_interest',
    'https://en.wikipedia.org/wiki/Republic',
    'https://en.wikipedia.org/wiki/Political_philosophy'
]

MEDICAL_URLS = [
    'https://en.wikipedia.org/wiki/Cardiovascular_disease',
    'https://en.wikipedia.org/wiki/Antibiotics',
    'https://en.wikipedia.org/wiki/Aortic_aneurysm',
    'https://en.wikipedia.org/wiki/Medical_ultrasound',
    'https://en.wikipedia.org/wiki/Atherosclerosis',
    'https://en.wikipedia.org/wiki/Myocardial_infarction',
    'https://en.wikipedia.org/wiki/Coronary_artery_disease',
    'https://en.wikipedia.org/wiki/Diabetes_mellitus',
    'https://en.wikipedia.org/wiki/Metabolic_disorder',
    'https://en.wikipedia.org/wiki/Mitochondrial_disease',
    'https://en.wikipedia.org/wiki/Anemia'
]

if __name__ == '__main__':
    main()
```

This outputs:
```bash
Scores for https://en.wikipedia.org/wiki/Matrix_multiplication :  -->  math
{'math': 0.4525650024744206,
 'medicine': 0.13516317937169092,
 'politics': 0.12693132752530528}
Scores for https://en.wikipedia.org/wiki/Constitutional_republic :  -->  politics
{'math': 0.17118084395264888,
 'medicine': 0.16685493347197253,
 'politics': 0.6667040730757361}
Scores for https://en.wikipedia.org/wiki/Vitamin_B12_deficiency :  -->  medicine
{'math': 0.1475778049040104,
 'medicine': 0.4441391114192581,
 'politics': 0.15670094485386332}

```