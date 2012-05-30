#
#   make a copy of the source tree and process additional doxygen macros
#

import os
import shutil
import sys

# prefix added to the block lines
blockprefix = "/// "

def mkdirs(path):
    try:
        os.makedirs(path)
    except:
        pass

def convert(dirpath,srcname,dstname):

    lines = []

    # process the input file
    f = open(srcname)

    for l in f:
        words = l.split()

        block = False

        # find the \block command
        if len(words) >= 3  and  words[0] == "///"  and  words[1] == "\\block":
            block = True

        if not block:
            lines.append(l)
            continue

        # process a \block command
        bfile = os.path.join(dirpath,words[2])
        bname = " ".join(words[3:])
        #lines.append("*** " + words[0] + " " + bfile + " " + bname + "\n")

        insert(lines,bfile,bname)

    f.close()

    outline = "".join(lines)

    # write out the output
    f = open(dstname,"w")
    f.write(outline)
    f.close()

def insert(lines,bfile,bname):
    #print "*** .%s. .%s." % (bfile, bname)

    f = open(bfile)

    found = False
    for l in f:
        words = l.split()

        block = False

        # find block start ///
        if len(words) >= 2  and  words[0] == "///":
            block = True
        if not block:
            continue

        # process a block ///

        # find the block label
        lpar = l.find("[")
        rpar = l.find("]")

        # skip incorrectly formatted labels
        if lpar < 0  or  rpar < 0  or  rpar < lpar:
            continue

        # get the label match
        blabel = l[lpar+1:rpar]
        #print "+++ .%s. .%s." % (blabel, l)
        if bname == blabel:
            found = True
            break

    if not found:
        f.close()
        return

    lines.append("\n")
    for l in f:
        words = l.split()

        block = False

        # terminate at the block end ///
        if len(words) >= 1  and  words[0] == "///":
            break

        lines.append(blockprefix + l)

    f.close()

if __name__ == '__main__':

    if len(sys.argv) < 3:
        print "Usage: " + sys.argv[0] + " <srcdir> <dstdir>"
        sys.exit(1)

    srcdir = sys.argv[1]
    dstdir = sys.argv[2]

    workdir = os.getcwd()

    for (path, dirs, files) in os.walk(srcdir):

        dstpath = os.path.join(workdir,path.replace(srcdir,dstdir,1))
        for dir in dirs:
            mkdirs(os.path.join(dstpath,dir))

        srcpath = os.path.join(workdir, path)

        for file in files:
            # copy file from path/file to workdir/dstpath/file
            #shutil.copyfile(
            convert(srcpath,
                os.path.join(srcpath,file), os.path.join(dstpath,file))


