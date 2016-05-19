#include "stdafx.h"
#include <stdio.h>
#include <sys/time.h>
#include "bfs-hybrid.h"
#include <vector>

double timeInSeconds(struct timeval &tv1, struct timeval &tv2) {
  double diff = 0;
  diff += (double)(tv2.tv_sec - tv1.tv_sec);
  diff += (double)(tv2.tv_usec - tv1.tv_usec) / 1000000;
  return diff;
}

int main(int argc, char* argv[]) {
  typedef PNGraph PGraph;  //   directed graph

  printf("Creating graph for Live Journal\n");

  TFIn FIn("data/livejournal_scc.graph");
  PGraph graph = TNGraph::Load(FIn);
  IAssert(graph->IsOk());
  printf("Graph (%d, %d)\n", graph->GetNodes(), graph->GetEdges());

  int start = 4529257;
  const int iter = 10;

  struct timeval tv1, tv2;
  gettimeofday(&tv1, NULL);

  for (int i = 0; i < iter; i++) {
    TBreathFS_Hybrid<PGraph> bfs(graph, true);
    int maxDist = bfs.DoBfs_Hybrid(start, true, false);
    IAssert(maxDist == 15);
  }

  gettimeofday(&tv2, NULL);
  double timeDiff = timeInSeconds(tv1, tv2) / iter;
  printf("Start node: %d\nTime spent: %f\n", start, timeDiff);

  return 0;
}

