## Binary Classifier (Ruby)

In this example, each of a small collection of Wikipedia articles is added to a relevanced server and assigned to the centroid `math`.  Similarity scores for another math-related article and an irrelevant article on Richard Gere are then calculated.

This example uses [nokogiri](http://www.nokogiri.org/) to crawl the Wikipedia pages and extract their text.  Note that `open-uri` is part of the Ruby standard library.

Also note that by far the slowest part of the script is fetching and crawling the Wikipedia pages.

```ruby
require 'relevanced_client'
require 'nokogiri'
require 'open-uri'

def run()
    client = RelevancedClient::Client.new('localhost', 8097)
    client.create_centroid('math')

    MATH_URLS.each do |url|
        text = fetch_url(url)
        client.create_document_with_id(url, text)
        client.add_document_to_centroid('math', url)
    end

    client.join_centroid('math')

    TEST_URLS.each do |url|
        text = fetch_url(url)
        similarity = client.get_text_similarity('math', text)
        puts "#{url} -> #{similarity}"
    end
end

def fetch_url(url)
    page = Nokogiri::HTML(open(url))
    paragraphs = page.css('#mw-content-text > p').map { |x| x.text }
    paragraphs.join("\n")
end

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

TEST_URLS = [
    'https://en.wikipedia.org/wiki/Matrix_multiplication',
    'https://en.wikipedia.org/wiki/Richard_Gere'
]

begin
    run()
end
```

As of 10/12/2015 (the article text may change), the result is:
```bash
https://en.wikipedia.org/wiki/Matrix_multiplication -> 0.4271200925196943
https://en.wikipedia.org/wiki/Richard_Gere -> 0.07472419631570816
```
