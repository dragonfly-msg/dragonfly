#!/usr/bin/python

import os.path
import sys
from ctypeslib.h2xml import main as h2xml
from ctypeslib.xml2py import main as xml2py

files = sys.argv[1:]

h2op = ["", "-c"]
x2op = [""]

for f in files:
    fabs = os.path.abspath(f)
    if not os.path.isfile(fabs):
        print "File not found: %s" % (fabs)
        continue
    print "Processing %s" % (fabs)

    # set h2xml output file
    basename = os.path.splitext(os.path.basename(fabs))[0]
    xmlfile = '%s.xml' % basename
    h2op.append("-o%s" % (xmlfile))

    # set h2xml input file
    h2op.append(fabs)

    # run h2xml
    h2xml(h2op)

    # set xml2py output file
    pyfile = "%s/%s.py" % (os.getcwd(), basename)
    x2op.append("-o%s" % pyfile)
    
    # set xml2py input file
    x2op.append(xmlfile)
    xml2py(x2op)
