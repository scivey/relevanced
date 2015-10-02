import os
import random
from pprint import pprint

def get_max_key(scores):
    pairs = scores.items()
    pairs.sort(key=lambda x: x[1])
    return pairs[-1][0]

def _rel_dir(file_name):
    base = os.path.dirname(os.path.realpath(__file__))
    return os.path.join(base, file_name)

def _in_newsgroup_dir(file_name):
    news_dir = _rel_dir('../tmp/20news-18828')
    return os.path.join(news_dir, file_name)

def _read_newsgroup_file_lines(file_name):
    line_num = 0
    with open(file_name) as f:
        for line in f:
            line_num += 1
            if line_num <= 2:
                continue
            while line.startswith('>') and len(line) > 1:
                line = line[1:]
            if len(line) < 2 or (len(line) and line[0] == '>'):
                continue
            if line.startswith('In article <') or line.startswith('wrote in response to'):
                continue
            yield line

def read_newsgroup_file(file_name):
    return '\n'.join(list(_read_newsgroup_file_lines(file_name)))

def load_one_newsgroup(name):
    group_dir = _in_newsgroup_dir(name)
    files = os.listdir(group_dir)
    output = {}
    for item in files:
        item_name = '%s_%s' % (name, item)
        text = read_newsgroup_file(_in_newsgroup_dir('%s/%s' % (name, item)))
        output[item_name] = text
    return output

def load_newsgroups():
    group_names = os.listdir(_in_newsgroup_dir('.'))
    groups = {}
    for name in group_names:
        groups[name] = load_one_newsgroup(name)
    return groups


def insert_data():
    from .main import get_client
    group_data = load_newsgroups()
    client = get_client()
    print 'creating centroids'
    for name, _ in group_data.iteritems():
        try:
            client.create_centroid(name)
        except Exception:
            pass

    print 'creating documents'
    for _, items in group_data.iteritems():
        for ident, text in items.iteritems():
            try:
                client.create_document_with_id(ident, text)
            except Exception as e:
                print 'error creating document (%s): %r' % (ident, e)

    print 'associating documents'
    for group_name, items in group_data.iteritems():
        for ident, _ in items.iteritems():
            try:
                client.add_document_to_centroid(group_name, ident)
            except Exception as e:
                print 'error associating document (%s): %s' % (ident, e)


def test():
    from .main import get_client
    group_data = load_newsgroups()
    client = get_client()
    all_groups = [name for name, _ in group_data.iteritems()]
    for group_name, items in group_data.iteritems():
        item_key = items.keys()[random.randint(0, len(items) - 1)]
        item_id = '%s_%s' % (group_name, item_key)
        item_text = items[item_key]
        item_scores = client.multi_get_text_similarity(
            all_groups, item_text
        )
        closest_group = get_max_key(item_scores)
        print '%s \t\t -> %s \t\t (%f)' % (item_key, closest_group, item_scores[closest_group])
