# relevanced

*relevanced* provides persistent, named binary document classifiers.

It maintains an in-memory centroid for each collection, enabling rapid classification with low memory requirements.

Documents are stored on disk in a preprocessed form, and streamed into memory during recalculation of the centroid.

Centroids are recalculated in batches with a standard offline algorithm.  This allows for relatively up-to-date models while avoiding the memory overhead and complexity of online variants.

*relevanced*'s interface is specified as a Thrift service.  Clients can be easily generated in many languages.

## Python example
Using the following list of URLs, and using [python-goose](https://github.com/grangier/python-goose) for content extraction:
```python
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

IRRELEVANT_URLS = [
  'https://en.wikipedia.org/wiki/Sovereign_state',
  'https://en.wikipedia.org/wiki/Autocracy',
  'https://en.wikipedia.org/wiki/Centralized_government',
  'https://en.wikipedia.org/wiki/Federation',
  'https://en.wikipedia.org/wiki/Goat',
  'https://en.wikipedia.org/wiki/Richard_Gere',
  'https://en.wikipedia.org/wiki/Mud',
  'https://en.wikipedia.org/wiki/Jazz',
  'https://en.wikipedia.org/wiki/Irish_Potato_Famine',
  'https://en.wikipedia.org/wiki/Empire',
  'https://en.wikipedia.org/wiki/Charles_Baudelaire',
  'https://en.wikipedia.org/wiki/Means_of_production'
]
```

```python
from relevanced_client import Client
from goose import Goose
client = Client('localhost', 8097)
client.create_collection('math')

for url in MATH_URLS:
    if client.get_document(url) != url:
        content = Goose().extract(url=url).cleaned_text
        client.create_document_with_id(url, content)
    client.add_positive_document_to_collection('math', url)

for url in IRRELEVANT_URLS:
    if client.get_document(url) != url:
        content = Goose().extract(url=url).cleaned_text
        client.create_document_with_id(url, content)
    client.add_negative_document_to_collection('math', url)

client.recompute('math')

math_test = Goose().extract(url='https://en.wikipedia.org/wiki/Simplex_Algorithm').cleaned_text
print client.get_relevance_for_text('math', math_test)
# 0.475308829932

politics_test = Goose().extract(url='https://en.wikipedia.org/wiki/Democracy').cleaned_text
print client.get_relevance_for_text('math', politics_test)
# 0.28290160811
```
