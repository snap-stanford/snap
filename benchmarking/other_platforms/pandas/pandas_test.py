import numpy as np
import pandas as pd
import time
import sys
import pdb

folder = '/lfs/local/0/ringo/testfiles/'
#folder = 'testfiles/'
ITERATIONS = 10

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
  def updatetime(self,op,store):
    store.update(op,self.gettime())

class OpTimes:
  def __init__(self,length):
    self.tottimes = {}
    self.operations = []
    self.length = length
  def update(self,op,time):
    if not op in self.tottimes:
      self.tottimes[op] = 0
      self.operations.append(op)
    self.tottimes[op] += time
  def gettime(self,op):
    return self.tottimes[op]/self.length
  def printResults(self):
    for op in self.operations:
      print op + ": " + "{0:.3f}".format(self.gettime(op)) + " seconds"


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

def QAGraph(s, qa_times):
  timer = Timer()
  posts = pd.read_csv(folder + 'posts_' + str(s) + '.hashed.tsv',sep='\t',error_bad_lines=False)
  timer.updatetime("Read", qa_times)
  #timer.showresults("Read", posts)
  total = Timer()
  posts1 = posts[posts['PostTypeId'] == 2]
  timer.updatetime("Select", qa_times)
  #timer.showresults("Select", posts1)
  rename(posts1,"ParentId","Id")
  posts2 = pd.merge(posts1,posts,on='Id')
  timer.updatetime("Join", qa_times)
  #timer.showresults("Join", posts2)
  posts3 = pd.DataFrame({'Count' : posts2.groupby( [ "OwnerUserId_x", "OwnerUserId_y"] ).size()}).reset_index()
  #posts3 = posts2.groupby(['OwnerUserId_x','OwnerUserId_y']).unique()
  timer.updatetime("Group", qa_times)
  #timer.showresults("Group", posts3)
  total.updatetime("Total", qa_times)
  #total.showtime("Total (without Read)")
  #posts3[['OwnerUserId_x','OwnerUserId_y']].to_csv('QA_out_' + str(s) + '.tsv', sep='\t',header = True, index = False)

def CommentsGraph(s, comm_times):
  timer = Timer()
  comments = pd.read_csv(folder + 'comments_' + str(s) + '.hashed.tsv',sep='\t',error_bad_lines=False)
  timer.updatetime("Read", comm_times)
  #timer.showresults("Read", comments)
  total = Timer()
  comments1 = pd.merge(comments,comments,on='PostId')
  timer.updatetime("Join", comm_times)
  #timer.showresults("Join", comments1)
  comments2 = comments1[comments1['UserId_x'] != comments1['UserId_y']]
  timer.updatetime("Select", comm_times)
  #timer.showresults("Select", comments2)
  comments3 = pd.DataFrame({'Count' : comments2.groupby(['UserId_x','UserId_y']).size()}).reset_index()
  timer.updatetime("Group", comm_times)
  #timer.showresults("Group", comments3)
  total.updatetime("Total", comm_times)
  #total.showtime("Total (without Read)")
  #comments3.to_csv('Comm_out_' + str(s) + '.tsv', sep='\t',header = True, index = False)

def main():
  sizes = [10,30,100,300,1000,3000,10000]
  print "\nQ&A Graph:"
  for s in sizes:
    qa_times = OpTimes(len(sizes))
    sys.stdout.write(str(s*1000) + " rows")
    for i in xrange(ITERATIONS):
      sys.stdout.write('.')
      sys.stdout.flush()
      QAGraph(s, qa_times)
    print ""
    qa_times.printResults()
  print "\nCommon Comments Graph:"
  for s in sizes:
    comm_times = OpTimes(len(sizes))
    sys.stdout.write(str(s*1000) + " rows")
    for i in xrange(ITERATIONS):
      sys.stdout.write('.')
      sys.stdout.flush()
      CommentsGraph(s, comm_times)
    print ""
    comm_times.printResults()

if __name__ == "__main__":
  main()
