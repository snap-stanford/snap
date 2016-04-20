#include "stdafx.h"
#include <omp.h>
#include <stdio.h>
#include <sys/time.h>
#include <vector>

#define NUM_START_NODES 20

using namespace std;

double timeInSeconds(struct timeval &tv1, struct timeval &tv2) {
  double diff = 0;
  diff += (double)(tv2.tv_sec - tv1.tv_sec);
  diff += (double)(tv2.tv_usec - tv1.tv_usec) / 1000000;
  return diff;
}

int main(int argc, char* argv[]) {
  //typedef PUNGraph PGraph; // undirected graph
  typedef PNGraph PGraph;  //   directed graph
  //typedef PNEGraph PGraph;  //   directed multigraph

  printf("Creating graph for Live Journal\n");

  TFIn FIn("livejournal_scc.graph");
  PGraph graph = TNGraph::Load(FIn);
  IAssert(graph->IsOk());
  printf("Graph (%d, %d)\n", graph->GetNodes(), graph->GetEdges());
  printf("Start nodes: ");

  vector<int> startNodes(NUM_START_NODES, 0);
  for (int i = 0; i < NUM_START_NODES; i++) {
    int id = graph->GetRndNId();
    printf(" %d", id);
    startNodes[i] = id;
  }
  printf("\n\n");

  #pragma omp parallel for
  for (int i = 0; i < NUM_START_NODES; i++) {
    int start = startNodes[i];
    // Get current time
    struct timeval tv1, tv2;
    gettimeofday(&tv1, NULL);

    // BFS
    TBreathFS<PGraph> bfs(graph, false);
    int maxDist = bfs.DoBfs(start, true, false);

    gettimeofday(&tv2, NULL);
    double timeDiff = timeInSeconds(tv1, tv2);

    printf("Start node: %d\nMax Dist (Returned by DoBfs): %d\nTime spent: %f\n\n", start, maxDist, timeDiff);
  }

  return 0;
}

