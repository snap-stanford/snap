#
#   generate dispatch tables for instantiated polymorphic SNAP templates
#       input file format: <func> <type>
#

import os
import sys

f_header     = "def %s(tspec, *args):" 
f_consbody   = "    if tspec == %-8s: return %s_%s(*args)"
f_funcbody   = "    if type(tspec) == %-8s: return %s_%s(tspec, *args)"
f_end        = "    return None" 

def gendispatch(item, df):
    flist = df[item]["cpp"]

    print f_header % (item)
    for ftype in flist:
        if df[item].has_key("type")  and  df[item]["type"] == "__cons__":
            print f_consbody % (ftype, item, ftype)
        else:
            print f_funcbody % (ftype, item, ftype)
    print f_end

if __name__ == '__main__':

    if len(sys.argv) < 3:
        print "Usage: " + sys.argv[0] + " <type> <dispatch>"
        sys.exit(1)

    df = {}

    ftname = sys.argv[1]
    f = open(ftname, "r")
    for nline in f:
        line = nline.split("\n")[0]
        w = line.split("\t")

        # skip empty lines, comments, malformed lines
        if len(w) < 2  or  len(w[0]) <= 0  or  w[0] == "#":
            continue

        ftype   = w[0]
        fname   = w[1]

        if not df.has_key(fname):
            df[fname] = {}
            df[fname]["cpp"] = []
            df[fname]["type"] = ftype

    f.close()

    fdname = sys.argv[2]
    f = open(fdname, "r")
    for nline in f:
        line = nline.split("\n")[0]
        w = line.split("\t")

        # skip empty lines, comments, malformed lines
        if len(w) < 2  or  len(w[0]) <= 0  or  w[0] == "#":
            continue

        cpptype = w[0]
        fname   = w[1]

        if not df.has_key(fname):
            df[fname] = {}
            df[fname]["cpp"] = []

        df[fname]["cpp"].append(cpptype)

    f.close()

    for item in df:
        #print item, df[item]
        gendispatch(item,df)

