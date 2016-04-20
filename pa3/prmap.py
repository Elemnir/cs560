#!/usr/bin/env python
import sys

def main():
    for line in sys.stdin:
        elements = line.rstrip().split()
        try:
            pgid, pr, links = int(elements[0]), float(elements[1]), elements[2:]
        except Exception:
            continue

        for link in links:
            if link.isdigit():
                sys.stdout.write("{}\t{}\n".format(link, pr / len(links))

if __name__ == "__main__":
    main()
