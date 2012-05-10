#!/cygdrive/c/Programs/Python/python.exe
###
# Zips all the files for GraphGarden release:
#   glib -- only files included from base.*
#   snap -- exclude amazon and linkedin
#   snap/examples -- *.cpp *.txt *.vcproj

import os, re, datetime

# simple regular expressions separated with |
def GetRegExpList(List) :
  ReV = []
  if len(List)== 0 : return ReV
  for ReStr in List.split('|') :
    ReStr = ReStr.replace('.','\.')
    ReStr = ReStr.replace('?','.?')
    ReStr = ReStr.replace('*','.*')
    ReObj = re.compile(ReStr)
    ReV.append(ReObj)
  return ReV

def IsReMatch(FNmStr, ReV) :
  for Re in ReV :
    if Re.match(FNmStr) :
      return True
  return False
  
def GetFileList(DirNm, Accept, Exclude) :
  AcceptReV = GetRegExpList(Accept)
  ExcludeReV = GetRegExpList(Exclude)
  FileV = []
  for FNm in os.listdir(DirNm) :
    if not os.path.isfile(DirNm+'/'+FNm) : continue
    if not IsReMatch(FNm, AcceptReV) or IsReMatch(FNm, ExcludeReV) :  continue
    FileV.append(DirNm+'/'+FNm)
  return FileV

# extracts includes from the FNm
def ExtractIncludes(Dir, FNm, ReStr) :
  F = open(Dir+'/'+FNm, 'rt')
  FileV = []
  RE = re.compile(ReStr)
  for line in F.readlines() :
    M = RE.search(line)
    if M :
      MStr = M.group(0)[1:-1]
      if not len(MStr) : continue
      FileV.append(Dir+'/'+MStr)
  F.close()
  return FileV

def Main() :
  FileList = ['README']
  # SNAP files
  FileList = FileList + GetFileList('snap', '*.cpp|*.h|Makefile|README', 'amzn*.*|linkedin.*|_tmp*')
  # GLib files extracted from base.*
  FileList = FileList + ExtractIncludes('glib', 'base.h', '\".*\.h\"')
  FileList = FileList + ExtractIncludes('glib', 'base.cpp', '\".*\.cpp\"')
  FileList = FileList + ['glib/base.h', 'glib/base.cpp', 'glib/stdafx.h', 'glib/linalg.*', 'glib/gnuplot.*', 'glib/Makefile', 'glib/README']

  # Example files
  FileList = FileList + GetFileList('examples', '*.sln|README|*.txt|Makefile|SnapExamples.sln', '')
  # Samples
  FileList = FileList + GetFileList('examples/cascades', '*.cpp|*.h|*.txt|*.vcproj|*.vcxproj|Makefile', '')
  FileList = FileList + GetFileList('examples/centrality', '*.cpp|*.h|*.txt|*.vcproj|*.vcxproj|Makefile', '')
  FileList = FileList + GetFileList('examples/cliques', '*.cpp|*.h|*.txt|*.vcproj|*.vcxproj|Makefile|*.png|*.dot', '')
  FileList = FileList + GetFileList('examples/community', '*.cpp|*.h|*.txt|*.vcproj|*.vcxproj|Makefile', '')
  FileList = FileList + GetFileList('examples/concomp', '*.cpp|*.h|*.txt|*.vcproj|*.vcxproj|Makefile', '')
  FileList = FileList + GetFileList('examples/forestfire', '*.cpp|*.h|*.txt|*.vcproj|*.vcxproj|Makefile', '')
  FileList = FileList + GetFileList('examples/graphgen', '*.cpp|*.h|*.txt|*.vcproj|*.vcxproj|Makefile', '')
  FileList = FileList + GetFileList('examples/kcores', '*.cpp|*.h|*.txt|*.vcproj|*.vcxproj|Makefile', '')  
  FileList = FileList + GetFileList('examples/kronfit', '*.cpp|*.h|*.txt|*.vcproj|*.vcxproj|Makefile', '')
  FileList = FileList + GetFileList('examples/krongen', '*.cpp|*.h|*.txt|*.vcproj|*.vcxproj|Makefile', '')
  FileList = FileList + GetFileList('examples/mkdatasets', '*.cpp|*.h|*.txt|*.vcproj|*.vcxproj|Makefile', '')
  FileList = FileList + GetFileList('examples/ncpplot', '*.cpp|*.h|*.txt|*.vcproj|*.vcxproj|Makefile', '')
  FileList = FileList + GetFileList('examples/netevol', '*.cpp|*.h|*.txt|*.vcproj|*.vcxproj|Makefile', '')
  FileList = FileList + GetFileList('examples/netstat', '*.cpp|*.h|*.txt|*.vcproj|*.vcxproj|Makefile', '')
  FileList = FileList + GetFileList('examples/testgraph', '*.cpp|*.h|*.txt|*.vcproj|*.vcxproj|Makefile', '')
  FileList = FileList + GetFileList('examples/motifs', '*.cpp|*.h|*.txt|*.vcproj|*.vcxproj|Makefile', '')
  
  print ('Zipping', len(FileList), 'files')
  F = open('zipGardenFiles.txt', 'wt')
  for file in FileList : F.write(file+'\n') 
  F.close()
  DateStr = datetime.date.today().isoformat()
  os.system('pkzip Snap-%s.zip -P @zipGardenFiles.txt' % DateStr[2:])
  os.remove('zipGardenFiles.txt')
  
Main()

