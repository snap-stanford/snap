#include "stdafx.h"
#include <stdio.h>
#include <sys/time.h>
#include <ctime> // for random generator
#include "bfs-hybrid.h"
#include "bfs-parallel.h"

using namespace std;

double timeInSeconds(struct timeval &tv1, struct timeval &tv2) {
  double diff = 0;
  diff += (double)(tv2.tv_sec - tv1.tv_sec);
  diff += (double)(tv2.tv_usec - tv1.tv_usec) / 1000000;
  return diff;
}

template <class PGraph>
bool checkResults(TBreathFS_Hybrid<PGraph> &bfs_hybrid, TBreathFS_Parallel<PGraph> &bfs) {
  int num = 0;
  for (TIntH::TIter it = bfs.NIdDistH.BegI(); it < bfs.NIdDistH.EndI(); it++) {
    num++;
    int key = it.GetKey();
    if (bfs.NIdDistH.GetDat(key) != bfs_hybrid.NIdDistH.GetDat(key)) {
      printf("Incorrect! NId: %d, Parallel: %d, Hybrid: %d\n", key, bfs.NIdDistH.GetDat(key)(), bfs_hybrid.NIdDistH.GetDat(key)());
      return false;
    }
  }
  IAssert(num == bfs.Graph->GetNodes());
  return true;
}

template <class PGraph>
bool test(PGraph &graph, bool followOut, bool followIn) {
  printf("\n================================\nFollowOut: %d, FollowIn: %d\n", followOut, followIn);
  int iters = 1;
  for (int k = 0; k < iters; k++) {
    TRnd rnd = TRnd((int)time(0));
    int start = graph->GetRndNId(rnd);
    rnd.PutSeed(0);
//    int target = graph->GetRndNId(rnd);
//    printf("Start node: %d, target node: %d\n", start, target);
    int target = -1;
    printf("Start node: %d\n", start);

    /* Hybrid */
    struct timeval tv1, tv2;
    gettimeofday(&tv1, NULL);

    TBreathFS_Hybrid<PGraph> bfs_hybrid(graph, true);
    int maxDist_hybrid = bfs_hybrid.DoBfs_Hybrid(start, followOut, followIn, target);

    gettimeofday(&tv2, NULL);
    double time_hybrid = timeInSeconds(tv1, tv2);

    /* Parallel */
    gettimeofday(&tv1, NULL);

    TBreathFS_Parallel<PGraph> bfs_parallel(graph, true);
    int maxDist = bfs_parallel.DoBfs_Parallel(start, followOut, followIn, target);

    gettimeofday(&tv2, NULL);
    double time = timeInSeconds(tv1, tv2);

    /* Check results */
    if (maxDist_hybrid != maxDist) {
      printf("No!!! MaxDist incorrect!\n");
      return false;
    }
    if (target == -1) {
      if (!checkResults<PGraph>(bfs_hybrid, bfs_parallel)) {
        printf("No!!! Results incorrect!\n");
        return false;
      }
    }

    printf("Parallel: %.2f, Hybrid: %.2f\n", time, time_hybrid);
  }
  return true;
}

int main(int argc, char* argv[]) {
  typedef PNGraph PGraph;  //   directed graph

  printf("Creating graph for Twitter\n");

  TFIn FIn("data/twitter_scc.graph");
  PGraph graph = TNGraph::Load(FIn);
  IAssert(graph->IsOk());
  printf("Nodes: %d\n", graph->GetNodes());

  test<PGraph>(graph, true, false);
//  test<PGraph>(graph, false, true);
//  test<PGraph>(graph, true, true);


  return 0;
}

