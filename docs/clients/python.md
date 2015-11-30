# Python Client

---

See the [detailed Python API documentation here.](../api/python_api.md)

---


## Installation

The Python client is available on PyPI; you can install it with `pip` by running:

```bash
pip install relevanced-client
```
You may need to run this command with `sudo`, depending on how and where you're installing it.

## Basic Use

```python
from relevanced_client import Client
client = Client('localhost', 8097)
documents = client.list_all_documents()
centroids = client.list_all_centroids()
some_score = client.get_document_similarity(
    'some-centroid-id', 'some-document-id'
)

```

## Examples

* [Python binary classifier](../examples/python-binary-classifier.md)
* [Python multiclass classifier](../examples/python-multiclass-classifier.md)
