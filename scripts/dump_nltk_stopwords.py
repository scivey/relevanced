from nltk.corpus import stopwords
import sys
HEX_1 = 16**3
HEX_2 = 16**2
HEX_3 = 16
HEX_4 = 1
digit_to_hex = {i: k for i, k in enumerate('0123456789abcdef')}

def get_hex(an_int):
    out = []
    for place in (HEX_1, HEX_2, HEX_3, HEX_4):
        out.append(digit_to_hex[an_int / place])
        an_int = an_int % place
    return str("\\u" + ''.join(out))

def reencode_word(word):
    ords = map(ord, word)
    out = []
    for c in ords:
        if c < 128:
            out.append(chr(c))
        else:
            out.append(get_hex(c))

    return str(''.join(out))

if __name__ == '__main__':
    if len(sys.argv) < 3 or sys.argv[2] not in ('header', 'impl'):
        raise ValueError('usage: <script> $language header|impl')

    language = sys.argv[1]
    func_name = 'is%sStopword' % language.title()

    if sys.argv[2] == 'impl':
        words = map(reencode_word, stopwords.words(language))

        print '#include <string>'
        print '#include <unordered_set>'
        print '#include "stopwords/%s_stopwords.h"' % language
        print ''
        print 'using namespace std;'
        print ''
        print 'namespace relevanced {'
        print 'namespace stopwords {'
        print ''

        print 'const unordered_set<string> stopwordSet {'
        for i in xrange(len(words)):
            word = words[i]
            line = '  "%s"' % word
            if i < (len(words) - 1):
                line += ','
            print line
        print '};'

        print ''
        print 'bool %s(const string &word) {' % func_name
        print '  return stopwordSet.count(word) > 0;'
        print '}'

        print ''
        print '} // stopwords'
        print '} // relevanced'
        print ''
    elif sys.argv[2] == 'header':
        print '#include <string>'
        print ''
        print 'namespace relevanced {'
        print 'namespace stopwords {'
        print ''
        print 'bool %s(const std::string &word);' % func_name
        print ''
        print '} // stopwords'
        print '} // relevanced'
        print ''
