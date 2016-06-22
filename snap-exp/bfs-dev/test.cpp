#include "stdafx.h"
#include <stdio.h>
#include <sys/time.h>
#include <ctime> // for random generator

double timeInSeconds(struct timeval &tv1, struct timeval &tv2) {
  double diff = 0;
  diff += (double)(tv2.tv_sec - tv1.tv_sec);
  diff += (double)(tv2.tv_usec - tv1.tv_usec) / 1000000;
  return diff;
}

template <class PGraph>
bool checkResults(TBreathFS<PGraph> &bfs_hybrid, TBreathFS<PGraph> &bfs) {
  int num = 0;
  if (bfs.NIdDistH.Len() != bfs_hybrid.NIdDistH.Len()) {
    printf("NIdDistH length does not match.\n");
    return false;
  }
  for (TIntH::TIter it = bfs.NIdDistH.BegI(); it < bfs.NIdDistH.EndI(); it++) {
    num++;
    int key = it.GetKey();
    if (bfs.NIdDistH.GetDat(key) != bfs_hybrid.NIdDistH.GetDat(key)) {
      printf("Incorrect! NId: %d, Original: %d, Hybrid: %d\n", key, bfs.NIdDistH.GetDat(key)(), bfs_hybrid.NIdDistH.GetDat(key)());
      return false;
    }
  }
  IAssert(num == bfs.Graph->GetNodes());
  return true;
}

template <class PGraph>
bool test(PGraph &graph, bool followOut, bool followIn) {
  printf("\n================================\nFollowOut: %d, FollowIn: %d\n", followOut, followIn);
  int iters = 10;
  for (int k = 0; k < iters; k++) {
    TRnd rnd = TRnd((int)time(0));
    int start = graph->GetRndNId(rnd);
    rnd.PutSeed(0);
//    int target = graph->GetRndNId(rnd);
//    printf("Start node: %d, target node: %d\n", start, target);
    int target = -1;
    printf("Start node: %d\n", start);

    struct timeval tv1, tv2;
    gettimeofday(&tv1, NULL);

    /* Hybrid */
    TBreathFS<PGraph> bfs_hybrid(graph, true);
    int maxDist_hybrid = bfs_hybrid.DoBfsHybrid(start, followOut, followIn, target);

    gettimeofday(&tv2, NULL);
    double time_hybrid = timeInSeconds(tv1, tv2);

    /* Original */
    gettimeofday(&tv1, NULL);

    TBreathFS<PGraph> bfs(graph, true);
    int maxDist = bfs.DoBfs(start, followOut, followIn, target);

    gettimeofday(&tv2, NULL);
    double time = timeInSeconds(tv1, tv2);

    /* Check results */
    if (maxDist_hybrid != maxDist) {
      printf("MaxDist incorrect.\n");
      return false;
    }
    if (target == -1) {
      if (!checkResults<PGraph>(bfs_hybrid, bfs)) {
        printf("NIdDistH values incorrect!\n");
        return false;
      }
    }

    printf("Execution times: Original: %.2f, Hybrid: %.2f\n", time, time_hybrid);
  }
  return true;
}

int main(int argc, char* argv[]) {
  typedef PNGraph PGraph;  //   directed graph

  printf("Creating graph for Livejournal\n");

  TFIn FIn("data/livejournal_scc.graph");
  PGraph graph = TNGraph::Load(FIn);
  IAssert(graph->IsOk());
  printf("Nodes: %d\n", graph->GetNodes());

  test<PGraph>(graph, true, false);
  test<PGraph>(graph, false, true);
  test<PGraph>(graph, true, true);


  return 0;
}

