#
#   generate tempaltes for SNAP typedefs
#       input file format: <typedef> <type> <name>;
#

import os
import sys

if __name__ == '__main__':

    if len(sys.argv) < 2:
        print "Usage: " + sys.argv[0] + " <typedefs>"
        sys.exit(1)

    fname = sys.argv[1]
    f = open(fname, "r")
    for nline in f:
        line = nline.split("\n")[0].strip()
        print line

        # remove typedef
        line1 = line.split(" ", 1)[1]
        print line1
        # remove ';'
        line2 = line1.replace(";","")
        print line2
        # get definition, name
        w = line2.rsplit(" ",1)
        if len(w) < 2:
            print "*** Error: incorrect line format: ", line
            sys.exit(1)
        tdef = w[0]
        tname = w[1]
        print "%%template(%s) %s;" % (tname, tdef)

    f.close()

