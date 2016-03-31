#!/usr/bin/env python
from itertools import groupby
from operator import itemgetter
import sys

def read_mapper_output(file, separator=' '):
    for line in file:
        yield line.rstrip().split(separator)

def main(separator=' '):
    data = read_mapper_output(sys.stdin, separator=separator)
    for word, group in groupby(data, itemgetter(0)):
        sys.stdout.write(word)
        sys.stdout.write(separator)
        for filehash, subgroup in groupby(group, itemgetter(1)):
            sys.stdout.write(filehash)
            sys.stdout.write(':')
            for linenum, subsubgroup in groupby(subgroup, itemgetter(2)):
                sys.stdout.write(linenum)
                sys.stdout.write(',')
            sys.stdout.write(';')
        sys.stdout.write(separator)
        sys.stdout.write('\n')

if __name__ == "__main__":
    main()
