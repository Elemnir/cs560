import sys

def main():
    idval = 0
    vals = {}
    for line in sys.stdin:
        word = line.rstrip().lower()
        if word not in vals:
            vals[word] = idval
            idval += 1

    for key in vals.keys():
        sys.stdout.write('{} {}\n'.format(key, vals[key]))

if __name__ == "__main__":
    main()
