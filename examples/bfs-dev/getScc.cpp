#include "stdafx.h"
#include <stdio.h>
#include <sys/time.h>

double timeInSeconds(struct timeval &tv1, struct timeval &tv2) {
  double diff = 0;
  diff += (double)(tv2.tv_sec - tv1.tv_sec);
  diff += (double)(tv2.tv_usec - tv1.tv_usec) / 1000000;
  return diff;
}

int main(int argc, char* argv[]) {
  typedef PNGraph PGraph; // directed graph

  printf("Creating graph for Twitter\n");

  PGraph graph = TSnap::LoadEdgeList<PGraph>("/dfs/ilfs2/0/ringo/benchmarks/twitter_rv.txt", 0, 1);
  IAssert(graph->IsOk());
  printf("Graph (%d, %d)\n", graph->GetNodes(), graph->GetEdges());

  printf("Getting max scc\n");
  PGraph maxScc = TSnap::GetMxScc(graph);
  printf("Scc (%d, %d)\n", maxScc->GetNodes(), maxScc->GetEdges());
  TFOut FOut("twitter_scc.graph");
  maxScc->Save(FOut);

  return 0;
}

