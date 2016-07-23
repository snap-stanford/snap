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

  TFIn FIn("data/twitter_scc.graph");
  PGraph graph = TNGraph::Load(FIn);
  IAssert(graph->IsOk());
  printf("Graph (%d, %d)\n", graph->GetNodes(), graph->GetEdges());

  int start = 59102400;

  struct timeval tv1, tv2;
  gettimeofday(&tv1, NULL);

  TBreathFS_Hybrid<PGraph> bfs(graph, true);
  int maxDist = bfs.DoBfs_Hybrid(start, true, false);
  IAssert(maxDist == 15);

//  for (int i = 0; i <= maxDist; i++) {
//    int frontier = bfs.edgeCounts[i].frontier;
//    int unvisited = bfs.edgeCounts[i].unvisited;
//    printf("Step %d: Frontier edges: %d, Unvisited edges: %d, Unvisied/Frontier = %f\n", i, frontier, unvisited, (float)unvisited / frontier);
//  }

  gettimeofday(&tv2, NULL);
  double timeDiff = timeInSeconds(tv1, tv2);

  printf("Start node: %d\nMax Distance: %d\nTime spent: %f\n", start, maxDist, timeDiff);

  return 0;
}

