from bs4 import BeautifulSoup
import random
import requests
from .caching import FuncCache

def _get_html(url):
    res = requests.get(url)
    assert(res.status_code < 300)
    return res.content

_html_cache = FuncCache(
    prefix='relevanced_func_test_raw_html',
    ttl=60*60*72,
    func=_get_html
)

def get_html(url):
    return _html_cache.get(url)

def select_n(n, elems):
    elems = list(elems)
    if n > len(elems):
        n = len(elems) - 1
    if n <= 0:
        return []
    indices = set([])
    get_next = lambda: random.randrange(0, len(elems))
    while True:
        current = get_next()
        if current not in indices:
            indices.add(current)
            if len(indices) >= n:
                break
    indices = list(indices)
    indices.sort()
    return [elems[i] for i in indices]

def get_wiki_links(url):
    html = get_html(url)
    soup = BeautifulSoup(html)
    links = soup.findAll('a')
    bad_words = (
        'List_of',
        'Portal:',
        '#',
        'Special:',
        'Template:',
        'Talk:',
        'Template_talk:',
        'Wikipedia:',
        'File:',
        'Category:',
        'Help:',
        'Book:'
    )
    for link in links:
        href = link.attrs.get('href', '')
        if href.startswith('/wiki/'):
            any_bad = False
            for bad in bad_words:
                if bad in href:
                    any_bad = True
                    break
            if not any_bad:
                yield href

def suggest_urls(existing_url_list):
    existing_url_list = list(existing_url_list)
    wiki_offset = existing_url_list[0].index('/wiki')
    existing_set = set(map(lambda url: url[wiki_offset:], existing_url_list))
    urls = select_n(10, existing_url_list)
    suggested = []
    for url in urls:
        for link in get_wiki_links(url):
            if link not in existing_set:
                suggested.append(link)
    suggested = list(set(suggested))
    random.shuffle(suggested)
    return suggested[:200]

def suggest_full_urls(url_list):
    suggestions = suggest_urls(url_list)
    return ['https://en.wikipedia.org%s' % url for url in suggestions]

def suggest_math_urls():
    from . import urls
    return suggest_urls(urls.MATH)

def suggest_poli_urls():
    from . import urls
    return suggest_urls(urls.POLITICS)

def poli_crawl(start, stop):
    from . import urls
    from .crawl import crawl_url
    for url in urls.POLITICS[start:stop]:
        print url
        crawl_url(url)

def math_crawl(start, stop):
    from . import urls
    from .crawl import crawl_url
    for url in urls.MATH[start:stop]:
        print url
        crawl_url(url)

def _suggest_relevant_urls(collection_name, existing_urls):
    from . import main, crawl
    client = main.get_client()
    suggestions = suggest_full_urls(existing_urls)[:50]
    crawled = crawl.crawl_urls(suggestions)
    relevant_suggestions = []
    for url, data in crawled.iteritems():
        if client.get_relevance_for_text(collection_name, data['text']) > 0.25:
            relevant_suggestions.append(url)
    return relevant_suggestions

def suggest_relevant_math_urls():
    from . import urls
    return _suggest_relevant_urls('wiki_math', urls.MATH)

def suggest_relevant_poli_urls():
    from . import urls
    return _suggest_relevant_urls('wiki_poli', urls.POLITICS)


def _extract_irrelevant_urls(url_list):
    from . import main, crawl
    crawled = crawl.crawl_urls(url_list)
    client = main.get_client()
    irrelevant = []
    for url, data in crawled.iteritems():
        is_irrelevant = True
        for collection in ('wiki_math', 'wiki_poli'):
            if client.get_relevance_for_text(collection, data['text']) > 0.1:
                is_irrelevant = False
                break
        if is_irrelevant:
            irrelevant.append(url)
    return irrelevant

def _suggest_irrelevant_urls(*url_lists):
    suggestions = []
    for url_list in url_lists:
        suggestions.extend(suggest_full_urls(url_list))
    random.shuffle(suggestions)
    return _extract_irrelevant_urls(suggestions)

def _crawl_irrelevant_urls(*url_lists):
    from . import urls
    start = []
    for url_list in url_lists:
        start.extend(url_list)
    seen_set = set([])
    for url in start:
        seen_set.add(url)
    initial_suggestions = suggest_full_urls(start)
    second_pass = set([])
    for url in initial_suggestions:
        if url not in seen_set:
            seen_set.add(url)
            second_pass.add(url)
    second_pass = list(second_pass)
    random.shuffle(second_pass)
    third_pass = set([])
    second_suggestions = suggest_full_urls(second_pass[:50])
    for url in second_suggestions:
        if url not in seen_set:
            seen_set.add(url)
            third_pass.add(url)
    third_pass = list(third_pass)
    random.shuffle(third_pass)
    return _extract_irrelevant_urls(third_pass[:50])

def suggest_wide_irrelevant_urls():
    from . import urls
    return _crawl_irrelevant_urls(urls.IRRELEVANT, urls.MATH, urls.POLITICS)

def suggest_narrow_irrelevant_urls():
    from . import urls
    return _crawl_irrelevant_urls(urls.IRRELEVANT)
