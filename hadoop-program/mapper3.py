#!/usr/bin/python
"""A more advanced Mapper, using Python iterators and generators."""

import sys
from collections import Counter

def read_input(file):
    for line in file:
        # split the line into words
        yield line.split()


def read_word(word):
    c = Counter(word)
    for k,v in c.items():
        yield k,v

def main(separator='\t'):
    # input comes from STDIN (standard input)
    data = read_input(sys.stdin)
    for words in data:
        # write the results to STDOUT (standard output);
        # what we output here will be the input for the
        # Reduce step, i.e. the input for reducer.py
        #
        # tab-delimited; the trivial word count is 1
        for word in words:
            charcnt = read_word(word)
            for char,cnt in charcnt:
                print "%s%s%d" %(char, separator,cnt)
            # print '%s%s%d' % (word, separator, 1)

if __name__ == "__main__":
    main()