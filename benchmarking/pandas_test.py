import numpy as np
import pandas as pd
import time

folder = '/lfs/local/0/ringo/testfiles/'

class Timer:
  def __init__(self):
    self.cumul = 0
    self.start = time.time()
  def gettime(self):
    duration = time.time()-self.start
    self.start = time.time()
    self.cumul += duration
    return duration
  def showtime(self,op):
    print op + ":\t" + "{0:.3f}".format(self.gettime())
  def showresults(self,op,df):
    print op + ":\t" + "{0:.3f}".format(self.gettime()) + " (working table has " + str(len(df)) + " rows)"

def rename(df,col,name):
  newcolumns = []
  for c in list(df.columns):
    if c == col:
      newcolumns.append(name)
    elif c == name:
      newcolumns.append('<undefined>')
    else:
      newcolumns.append(c)
  df.columns = newcolumns

def QAGraph(s):
  print "\n" + str(s*1000) + " rows:"
  timer = Timer()
  posts = pd.read_csv(folder + 'posts_' + str(s) + '.hashed.tsv',sep='\t',error_bad_lines=False)
  timer.showresults("Read", posts)
  total = Timer()
  posts1 = posts[posts['PostTypeId'] == 2]
  timer.showresults("Select", posts1)
  rename(posts1,"ParentId","Id")
  posts2 = pd.merge(posts1,posts,on='Id')
  timer.showresults("Join", posts2)
  posts3 = posts2.groupby(['OwnerUserId_x','OwnerUserId_y']).size()
  timer.showresults("Group", posts3)
  total.showtime("Total (without Read)")

def CommentsGraph(s):
  print "\n" + str(s*1000) + " rows:"
  timer = Timer()
  comments = pd.read_csv(folder + 'comments_' + str(s) + '.hashed.tsv',sep='\t',error_bad_lines=False)
  timer.showresults("Read", comments)
  total = Timer()
  comments1 = pd.merge(comments,comments,on='PostId')
  timer.showresults("Join", comments1)
  comments2 = comments1[comments1['UserId_x'] != comments1['UserId_y']]
  timer.showresults("Select", comments2)
  comments3 = comments2.groupby(['UserId_x','UserId_y']).size()
  timer.showresults("Group", comments3)
  total.showtime("Total (without Read)")

sizes = [10,30,100,300,1000,3000,10000]
print "\nQ&A Graph:"
for s in sizes:
  QAGraph(s)
print "\nCommon Comments Graph:"
for s in sizes:
  CommentsGraph(s)
