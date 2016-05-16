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

template <class PGraph>
bool checkResults(TBreathFS_Hybrid<PGraph> &bfs_hybrid, TBreathFS<PGraph> &bfs) {
  for (TIntH::TIter it = bfs.NIdDistH.BegI(); it < bfs.NIdDistH.EndI(); it++) {
    int key = it.GetKey();
    if (bfs.NIdDistH[key]() != bfs_hybrid.NIdDistV[key]()) {
      printf("Key: %d, BFS: %d, Hybrid: %d\n", key, bfs.NIdDistH[key](), bfs_hybrid.NIdDistV[key]());
      return false;
    }
  }
  return true;
}

int main(int argc, char* argv[]) {
  typedef PNGraph PGraph;  //   directed graph

  printf("Creating graph for Livejournal\n");

  TFIn FIn("data/livejournal_scc.graph");
  PGraph graph = TNGraph::Load(FIn);
  IAssert(graph->IsOk());

  int iters = 1;
  for (int k = 0; k < iters; k++) {
    int start = graph->GetRndNId();
    printf("Start node: %d\n", start);

    /* Hybrid */
    struct timeval tv1, tv2;
    gettimeofday(&tv1, NULL);

    TBreathFS_Hybrid<PGraph> bfs_hybrid(graph, true);
    int maxDist_hybrid = bfs_hybrid.DoBfs_Hybrid(start, true, false);

    gettimeofday(&tv2, NULL);
    double time_hybrid = timeInSeconds(tv1, tv2);

    /* Original */
    gettimeofday(&tv1, NULL);

    TBreathFS<PGraph> bfs(graph, true);
    int maxDist = bfs.DoBfs(start, true, false);
    int d = bfs.NIdDistH[start]();
    printf("Start distance: %d\n", d);

    gettimeofday(&tv2, NULL);
    double time = timeInSeconds(tv1, tv2);

    /* Check results */
    if (maxDist_hybrid != maxDist) {
      printf("No!!! MaxDist incorrect!\n");
      break;
    }
    if (!checkResults<PGraph>(bfs_hybrid, bfs)) {
      printf("No!!! Results incorrect!\n");
      break;
    }

    printf("Original: %.4f, Hybrid: %.4f\n", time, time_hybrid);

  }

  return 0;
}

