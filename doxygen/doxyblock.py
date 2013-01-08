#
#   make a copy of the source tree and process additional doxygen macros
#

import os
import shutil
import sys

# brief doc prefix
briefprefix = "///"

# detailed doc prefix
detailprefix = "##"

# block prefix in the existing documentation
blockprefix = "///"

# subdirectory with the documentation
docdir = "doc"

# documentation file suffix
docsuffix = ".txt"

# prefix added to the block lines in the output
docprefix = "/// "

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
        # find the comments
        brief = l.find(briefprefix)
        detailed = l.find(detailprefix)

        # no comments found, add the line to the output and continue
        if brief < 0  or  detailed < 0  or  detailed < brief:
            lines.append(l)
            continue

        # process the comment

        # append the content before the detailed doc
        lines.append(l[:detailed] + "\n")

        # get the macro file name and block name
        bfile = os.path.join(dirpath, docdir,
                            os.path.basename(srcname) + docsuffix)

        words = l[detailed+len(detailprefix):].split()
        if len(words) <= 0:
            continue

        bname = words[0].strip()
        if len(bname) <= 0:
            continue

        #lines.append("*** " + bfile + " " + bname + "\n")

        insert(lines,bfile,bname)

    f.close()

    outline = "".join(lines)

    # write out the output
    f = open(dstname,"w")
    f.write(outline)
    f.close()

def insert(lines,bfile,bname):
    #print "*** .%s. .%s." % (bfile, bname)

    if not os.path.exists(bfile):
        print "*** Error: file not found - file:.%s." % (bfile)
        sys.exit(1)

    f = open(bfile)

    found = False
    for l in f:
        words = l.split()

        block = False

        # find block start
        if len(words) >= 2  and  words[0] == blockprefix:
            block = True
        if not block:
            continue

        # get the label match
        blabel = words[1]
        #print "+++ .%s. .%s." % (blabel, l)
        if bname == blabel:
            found = True
            break

    if not found:
        f.close()
        print "*** Error: block not found - file:.%s., block:.%s." % (bfile, bname)
        sys.exit(1)
        return

    lines.append("\n")
    for l in f:
        words = l.split()

        block = False

        # terminate at the block end
        if len(words) >= 1  and  words[0] == blockprefix:
            break

        lines.append(docprefix + l)

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

            # skip files that start with "."
            if file[0] == ".":
                continue

            # skip files that do not end with ".cpp" or ".h"
            fname, fext = os.path.splitext(file)
            if not fext in [ ".cpp", ".h", ".txt" ]:
                continue

            # copy file from path/file to workdir/dstpath/file
            #shutil.copyfile(
            convert(srcpath,
                os.path.join(srcpath,file), os.path.join(dstpath,file))

