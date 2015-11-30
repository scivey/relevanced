# Ruby Client

---

See the [detailed Ruby API documentation here.](../api/ruby_api.md)

---

## Installation

The Ruby client is available on RubyGems; you can install it by running:

```bash
gem install relevanced_client
```

You may need to run this command with `sudo`, depending on how and where you're installing it.

You can also install the `relevanced_client` gem with Bundler or any other Ruby dependency management tool.

## Basic Use

```ruby
require 'relevanced_client'
client = RelevancedClient::Client.new('localhost', 8097)
documents = client.list_all_documents()
centroids = client.list_all_centroids()
some_score = client.get_document_similarity(
    'some-centroid-id', 'some-document-id'
)

```
## Examples
* [Ruby binary classifier](../examples/ruby-binary-classifier.md)
