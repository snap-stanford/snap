#include "stdafx.h"
#include <stdio.h>
#include <sys/time.h>
#include <ctime> // for random generator
#include <vector>
#include "bfs-hybrid.h"

double timeInSeconds(struct timeval &tv1, struct timeval &tv2) {
  double diff = 0;
  diff += (double)(tv2.tv_sec - tv1.tv_sec);
  diff += (double)(tv2.tv_usec - tv1.tv_usec) / 1000000;
  return diff;
}

template <class PGraph>
bool checkResults(TBreathFS_Hybrid<PGraph> &bfs_hybrid, TBreathFS_Test<PGraph> &bfs) {
  for (int i = 0; i < bfs.NIdDistV.Len(); i++) {
    if (bfs.NIdDistV[i] != bfs_hybrid.NIdDistV[i]) {
      printf("Incorrect! Index: %d, BFS: %d, Hybrid: %d\n", i, bfs.NIdDistV[i](), bfs_hybrid.NIdDistV[i]());
      return false;
    }
  }
  return true;
}

template <class PGraph>
bool test(PGraph &graph, bool followOut, bool followIn) {
  printf("\n================================\nFollowOut: %d, FollowIn: %d\n", followOut, followIn);
  int iters = 5;
  for (int k = 0; k < iters; k++) {
    TRnd rnd = TRnd((int)time(0));
    int start = graph->GetRndNId(rnd);
    rnd.PutSeed(0);
//    int target = graph->GetRndNId(rnd);
//    printf("Start node: %d, target node: %d\n", start, target);
    int target = -1;
    printf("Start node: %d\n", start);

    /* Hybrid (nodes) */
    struct timeval tv1, tv2;
    gettimeofday(&tv1, NULL);

    TBreathFS_Hybrid<PGraph> bfs_hybrid(graph, true);
    int maxDist_hybrid = bfs_hybrid.DoBfs_Hybrid(start, followOut, followIn, target);

    gettimeofday(&tv2, NULL);
    double time_hybrid = timeInSeconds(tv1, tv2);

    /* Hybrid (edges) */
    gettimeofday(&tv1, NULL);

    TBreathFS_Hybrid_Test<PGraph> bfs_test(graph, true);
    int maxDist = bfs_test.DoBfs_Hybrid_Test(start, followOut, followIn, target);

    gettimeofday(&tv2, NULL);
    double time_test = timeInSeconds(tv1, tv2);

    /* Transform */
    TIntH NIdDistH(graph->GetNodes());
    for (int NId = 0; NId < bfs_hybrid.NIdDistV.Len(); NId++) {
      if (bfs_hybrid.NIdDistV[NId] != -1) {
        NIdDistH.AddDat(NId, bfs_hybrid.NIdDistV[NId]);
      }
    }
    gettimeofday(&tv1, NULL);
    double time_transform = timeInSeconds(tv2, tv1);

    /* Check results */
    if (maxDist_hybrid != maxDist) {
      printf("No!!! MaxDist incorrect!\n");
      return false;
    }
    if (target == -1) {
      if (!checkResults<PGraph>(bfs_hybrid, bfs)) {
        printf("No!!! Results incorrect!\n");
        return false;
      }
    }

    printf("Hybrid (nodes): %.2f, Hybrid (edges): %.2f, Transform: %.3f\n", time_hybrid, time_test, time_transform);
  }
  return true;
}

int main(int argc, char* argv[]) {
  typedef PNGraph PGraph;  //   directed graph

  printf("Creating graph for Livejournal\n");

  TFIn FIn("data/livejournal_scc.graph");
  PGraph graph = TNGraph::Load(FIn);
  IAssert(graph->IsOk());

  test<PGraph>(graph, true, false);
  test<PGraph>(graph, false, true);
  test<PGraph>(graph, true, true);


  return 0;
}

