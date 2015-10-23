
# make a namespace more nesty
codemod.py -m --extensions cpp,h 'namespace persistence \{(.*)\}([ \/a-z]*)' 'namespace relevanced {\nnamespace persistence {\1}\2\n} // relevanced\n'

# update a using:: statement
codemod.py --extensions cpp,h 'using stemmer::(.*)$' 'using relevanced::stemmer::\1'

