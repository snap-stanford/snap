#include "time.h"

#include "Snap.h"

void test1();

int main(int argc, char* argv[]){
  TTableContext Context;

  // create scheme
  Schema PostS;
  PostS.Add(TPair<TStr,TAttrType>("Id", atInt));
  PostS.Add(TPair<TStr,TAttrType>("OwnerUserId", atInt));
  PostS.Add(TPair<TStr,TAttrType>("AcceptedAnswerId", atInt));
  PostS.Add(TPair<TStr,TAttrType>("CreationDate", atInt));
  PostS.Add(TPair<TStr,TAttrType>("Score", atInt));
  TIntV RelevantCols;
  RelevantCols.Add(0); RelevantCols.Add(1); RelevantCols.Add(2); RelevantCols.Add(3); RelevantCols.Add(4);

  PTable P = TTable::LoadSS(PostS, "/dfs/ilfs2/0/ringo/StackOverflow_2/posts.tsv", Context, RelevantCols);
  printf("Load done\n");

  TStrV cols;
  cols.Add("OwnerUserId");

  struct timeval begin, end;
  gettimeofday(&begin, NULL);
  PTable Q = P->IsNextK("CreationDate", 1, "OwnerUserId");
  gettimeofday(&end, NULL);

  double diff = (end.tv_sec * 1000000 + end.tv_usec) - (begin.tv_sec * 1000000 + begin.tv_usec);
  printf("Elapsed time:%.3lfs\n", diff / 1000000);
  
  if (atoi(argv[1]) == 0) return 0;

  Q->SaveSS("tests/p3.txt");

  return 0;
}
