#!/usr/bin/env python

import sys

damping = 0.85

def main():
    sums = {}
    for line in sys.stdin:
        key, value = line.rstrip().split('\t', 1)
        
        try:
            value = float(value)
        except Exception:
            continue

        if key in sums.keys():
            sums[key] += value
        else:
            sums[key] = value

    for pgid in sums.keys():
        pr = (1 - damping) + damping * sums[pgid]
        sys.stdout.write("{} {}\n".format(pgid, pr)

if __name__ == "__main__":
    main()
