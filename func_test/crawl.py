from __future__ import print_function
from goose import Goose
from .caching import FuncCache

def _crawl_url(url):
    print('crawling: %s' % url)
    extracted = Goose().extract(url=url)
    return {
        'title': extracted.title,
        'text': extracted.cleaned_text,
        'url': url
    }

_crawl_cache = FuncCache(
    prefix='relevanced_func_crawled_pages',
    ttl=60*60*24*14,
    func=_crawl_url
)

def crawl_url(url):
    return _crawl_cache.get(url)

def crawl_urls(urls):
    return _crawl_cache.get_multi(urls)
