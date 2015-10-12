# Python Client

The Python client is available on PyPI; you can install it with `pip` by running:

```bash
pip install relevanced_client
```

The Python client API has the same commands described in the [language-agnostic command overview](../commands.md), but they are converted to snake-case to be more Pythonic.  For example:

```
    getTextSimilarity(centroid, text) -> get_text_similarity(centroid, text)
    createCentroid(id)                -> create_centroid(id)
    listAllCentroids()                -> list_all_centroids()
```

Basic client use:
```python
from relevanced_client import Client
client = Client('localhost', 8097)
documents = client.list_all_documents()
centroids = client.list_all_centroids()
some_score = client.get_document_similarity(
    'some-centroid-id', 'some-document-id'
)

```

Also see the Python [binary classifier](../examples/python-binary-classifier.md) and [multiclass classifier](../examples/python-multiclass-classifier.md) examples for more practical use.
