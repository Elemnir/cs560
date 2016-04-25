import os
import re
import sys

from xml.etree import ElementTree

def main():
    tree = ElementTree.parse(sys.argv[1])
    sys.stderr.write('Tree Parsed\n')
    root = tree.getroot()

    for page in root.findall('page'):
        title = page.findtext('title')
        text = page.find('revision').findtext('text')
        if title and text:
            links = re.findall(r'\[\[(?!File:|Special:)(.+?)\]\]', text)
            sys.stdout.write('{}\t\t'.format(title))
            for link in links:
                sys.stdout.write('{}\t'.format(link))
            sys.stdout.write('\n')

if __name__ == "__main__":
    main()
