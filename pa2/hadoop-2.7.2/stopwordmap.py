#!/usr/bin/env python
import os
import re
import sys

def read_input(file):
    for line in file:
        yield line.split()

def main(separator=' '):
    data = read_input(sys.stdin)
    for words in data:
        for word in words:
            word = re.sub('[^a-zA-Z]', '', word).lower()
            if word != '':
                print '%s%s%d' % (word, separator, 1)

if __name__ == "__main__":
    main()
