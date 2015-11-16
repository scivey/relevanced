from collections import namedtuple
import sys
from pprint import pprint

import requests
from bs4 import BeautifulSoup

UnicodeBlock = namedtuple('UnicodeBlock', ['name', 'start', 'end'])


HEX_CHAR_MAP = {k: i for i, k in enumerate('0123456789abcdef')}

def int_of_hex_char(hex_char):
    return HEX_CHAR_MAP[hex_char]

def int_of_hex_str(hex_str):
    hex_digits = map(int_of_hex_char, hex_str.lower())
    char_count = len(hex_digits)
    for i in xrange(0, char_count):
        hex_digits[i] = hex_digits[i] * (16 ** ((char_count - 1) - i))
    return sum(hex_digits)

def codify_name(name):
    return name.upper().replace('-', '_').replace(' ', '_')

def extract_row(row):
    cells = row.findAll('td')
    range_cell = cells[1]
    title_cell = cells[2]

    if 'U+' not in range_cell.text:
        # some rows have range as first cell,
        # some as second
        range_cell = cells[0]
        title_cell = cells[1]

    title = title_cell.text
    if '[' in title:
        title = title[:title.index('[')]
    try:
        range_text = range_cell.text[range_cell.text.index('U+'):]
    except ValueError:
        print range_cell.text
        raise
    start, end = range_text.split('..')

    # remove leading 'U+', leaving just the hex code
    start = start[2:]
    end = end[2:]

    return UnicodeBlock(
        name=title,
        start=int_of_hex_str(start),
        end=int_of_hex_str(end)
    )

def split_list_in_2(a_list):
    index = len(a_list) / 2
    return a_list[:index], a_list[index:]

def split_list_in_4(a_list):
    index = len(a_list) / n_parts
    return a_list[:index], a_list[index:]


def generate_enum(blocks):
    blocks.sort(key=lambda x: x.name)
    print '\n\n'
    print 'enum class UnicodeBlock {'
    for block in blocks:
        print '  %s,' % codify_name(block.name)
    print '  UNKNOWN'
    print '};\n'

    print '\n\n'


def generate_string_of_block(blocks):
    blocks.sort(key=lambda x: x.name)
    print 'std::string stringOfUnicodeBlock(UnicodeBlock block) {'
    print '  switch(block) {'
    for block in blocks:
        name = codify_name(block.name)
        print '    case UnicodeBlock::%s : return "%s";' % (name, name)
    print '    case UnicodeBlock::UNKNOWN : return "UNKNOWN";'
    print '  }'
    print '  return "UNKNOWN";'
    print '}'
    print '\n\n'


def generate_get_block(blocks):
    def emit_linear_blocks(block_set, indentation):
        assert(len(block_set) >= 2)
        indentation = ''.join([' ' for _ in xrange(indentation)])
        lines = []
        lines.append('%sif (cp <= %s) {' % (indentation, block_set[0].end))
        lines.append('%s  return UnicodeBlock::%s;' % (indentation, codify_name(block_set[0].name)))
        lines.append('%s}' % indentation)
        last_block_index = len(block_set) - 1
        for i in xrange(1, len(block_set)):
            block = block_set[i]
            if i == last_block_index:
                lines[-1] += ' else {'
            else:
                lines[-1] += (' else if (cp <= %s) {' % block.end)
            lines.append('%s  return UnicodeBlock::%s;' % (indentation, codify_name(block.name)))
            lines.append('%s}' % indentation)
        return lines

    def print_blocks_rec(block_set, indentation):
        if len(block_set) < 4:
            lines = emit_linear_blocks(block_set, indentation=indentation)
            return lines
        first_half, second_half = split_list_in_2(block_set)
        lines = []
        indent_space = ''.join([' ' for _ in xrange(indentation)])
        lines.append('%sif (cp < %s) {' % (indent_space, second_half[0].start))
        lines.extend(print_blocks_rec(first_half, indentation=indentation+2))
        lines.append('%s} else {' % indent_space)
        lines.extend(print_blocks_rec(second_half, indentation=indentation+2))
        lines.append('%s}' % indent_space)
        return lines

    blocks.sort(key=lambda x: x.start)

    print 'UnicodeBlock getUnicodeBlock(uint32_t codePoint) {'
    print '  auto cp = codePoint;'

    basic_latin_blocks = blocks[:4]
    other_blocks = blocks[4:]

    # check for basic latin first
    print '  if (cp < %s) {' % other_blocks[0].start
    print '\n'.join(print_blocks_rec(basic_latin_blocks, indentation=4))
    print '  }\n'

    print '\n'.join(print_blocks_rec(other_blocks, indentation=2))
    print '  return UnicodeBlock::UNKNOWN;'
    print '}\n\n'


def throw_usage(args, allowed_keys):
    pprint(args)
    keys = '|'.join(sorted(allowed_keys))
    message = 'usage: <script> %s' % allowed_keys
    raise ValueError(message)

if __name__ == '__main__':
    funcs = {
        'get_block': generate_get_block,
        'string_of': generate_string_of_block,
        'enum': generate_enum
    }
    args = sys.argv
    if len(args) < 2:
        throw_usage(args, funcs.keys())
    if args[1] not in funcs.keys():
        throw_usage(args, funcs.keys())
    page = requests.get('https://en.wikipedia.org/wiki/Unicode_block')
    soup = BeautifulSoup(page.content)
    table = soup.select('.wikitable')[0]

    # 3 header rows at top, 1 footnote row at bottom
    rows = table.findAll('tr')[3:-1]
    blocks = map(extract_row, rows)
    funcs[sys.argv[1]](blocks)
