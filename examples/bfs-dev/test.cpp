#include "stdafx.h"
#include <stdio.h>
#include <sys/time.h>
#include "bfs-vector.h"
#include "bfs-hybrid.h"

double timeInSeconds(struct timeval &tv1, struct timeval &tv2) {
  double diff = 0;
  diff += (double)(tv2.tv_sec - tv1.tv_sec);
  diff += (double)(tv2.tv_usec - tv1.tv_usec) / 1000000;
  return diff;
}

bool checkBfsResults(TIntV &v1, TIntV &v2) {
  if (v1.Len() != v2.Len()) return false;
  for (int i = 0; i < v1.Len(); i++) {
    if (v1[i] != v2[i]) {
      return false;
    }
  }
  return true;
}

int main(int argc, char* argv[]) {
  typedef PNGraph PGraph;  //   directed graph

  printf("Creating graph for Live Journal\n");

  TFIn FIn("livejournal_scc.graph");
  PGraph graph = TNGraph::Load(FIn);
  IAssert(graph->IsOk());
  printf("Graph (%d, %d)\n", graph->GetNodes(), graph->GetEdges());

  int start = 4529257;

  printf("====== Original vector BFS ======\n");

  struct timeval tv1, tv2;
  gettimeofday(&tv1, NULL);

  TBreathFS_Test<PGraph> bfsVector(graph, true);
  int maxDist = bfsVector.DoBfs_test(start, true, false);

  gettimeofday(&tv2, NULL);
  double timeDiff = timeInSeconds(tv1, tv2);
  printf("Start node: %d\nMax Distance: %d\nTime spent: %f\n", start, maxDist, timeDiff);

  /* Hybrid */
  printf("\n\n====== Hybrid BFS ======\n");

  gettimeofday(&tv1, NULL);

  TBreathFS_Hybrid<PGraph> bfsHybrid(graph, true);
  maxDist = bfsHybrid.DoBfs_Hybrid(start, true, false);

  gettimeofday(&tv2, NULL);
  timeDiff = timeInSeconds(tv1, tv2);
  printf("Start node: %d\nMax Distance: %d\nTime spent: %f\n", start, maxDist, timeDiff);

  /* Check results */
  printf("\n\n===== Check results =====\n"); 
  if (checkBfsResults(bfsVector.NIdDistV, bfsHybrid.NIdDistV)) {
    printf("Same!!\n");
  }

  return 0;
}

