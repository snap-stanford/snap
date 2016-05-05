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

  printf("Creating graph for Twitter\n");

  TFIn FIn("twitter_scc.graph");
  PGraph graph = TNGraph::Load(FIn);
  IAssert(graph->IsOk());
  printf("Graph (%d, %d)\n", graph->GetNodes(), graph->GetEdges());

  int start = 59102400;

  for (int i = 0; i < 16; i++) {
    for (int j = i + 1; j < 17; j++) {
//      struct timeval tv1, tv2;
//      gettimeofday(&tv1, NULL);

      TBreathFS_Hybrid<PGraph> bfs(graph, true);
      int maxDist = bfs.DoBfs_Hybrid(start, true, false, i, j);
      IAssert(maxDist == 15);

      printf("Switch points: %d, %d\n", i, j);
      int totalClaimed = 0;
      int totalFailed = 0;
      for (int step = 0; step <= maxDist; step++) {
        int claimed = bfs.childCounts[step].claimed;
        int failed = bfs.childCounts[step].failed;
        printf("Step %d: Claimed: %f%% (%d, %d)\n", step, (double)claimed / (claimed + failed) * 100, claimed, failed);
        totalClaimed += claimed;
        totalFailed += failed;
      }
      printf("Total: Claimed: %f%% (%d, %d)\n\n", (double)totalClaimed / (totalClaimed + totalFailed) * 100, totalClaimed, totalFailed);

//      gettimeofday(&tv2, NULL);
//      double timeDiff = timeInSeconds(tv1, tv2);
    }
  }

//  printf("Start node: %d\nMax Distance: %d\nTime spent: %f\n", start, maxDist, timeDiff);

  return 0;
}

