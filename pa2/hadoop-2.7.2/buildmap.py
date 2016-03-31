#!/usr/bin/env python
import hashlib
import os
import re
import sys

# Read in the books
def read_input(file):
    linenum = 0
    for line in file:
        linenum += 1
        yield line.split(), os.path.basename(os.environ['mapreduce_map_input_file']), linenum

def main(separator=' '):
    # First read in the stop words
    stopwords = {}
    if os.path.isfile('stopwords.txt'):
        stopwordfile = open('stopwords.txt', 'r')
        for line in stopwordfile:
            match = re.match('^(?P<word>[a-z]+) (\d+)', line)
            if match is not None:
                stopwords[match.group('word')] = match.group(1)

    data = read_input(sys.stdin)
    for words, filehash, linenum in data:
        for word in words:
            word = re.sub('[^a-zA-Z]', '', word).lower()
            if (word != '') and (word not in stopwords):
                print '%s%s%s%s%d' % (word, separator, filehash, separator, linenum)

if __name__ == "__main__":
    main()
