# Ruby Client

## Installation

The Ruby client is available on RubyGems; you can install it by running:

```bash
gem install relevanced_client
```

You may need to run this command with `sudo`, depending on how and where you're installing it.

You can also install the `relevanced_client` gem with Bundler or any other Ruby dependency management tool.

## API

The Ruby client API has the same commands described in the [language-agnostic command overview](../commands.md), but they are converted to snake-case to be more Ruby-ish.  The mapping is identical to that used in the [Python client](./python.md), e.g.:

```
    getTextSimilarity(centroid, text) -> get_text_similarity(centroid, text)
    createCentroid(id)                -> create_centroid(id)
    listAllCentroids()                -> list_all_centroids()
```

Basic client use:
```ruby
require 'relevanced_client'
client = RelevancedClient::Client.new('localhost', 8097)
documents = client.list_all_documents()
centroids = client.list_all_centroids()
some_score = client.get_document_similarity(
    'some-centroid-id', 'some-document-id'
)

```

Also see the Ruby [binary classifier](../examples/ruby-binary-classifier.md) example for more practical use.