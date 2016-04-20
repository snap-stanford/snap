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
  //typedef PUNGraph PGraph; // undirected graph
  typedef PNGraph PGraph;  //   directed graph
  //typedef PNEGraph PGraph;  //   directed multigraph
  //typedef TPt<TNodeNet<TInt> > PGraph;
  //typedef TPt<TNodeEdgeNet<TInt, TInt> > PGraph;

  // this code is independent of what particular graph implementation/type we use
  printf("Creating graph for Live Journal\n");

  PGraph graph = TSnap::LoadEdgeList<PGraph>("../../../soc-LiveJournal1.txt", 0, 1);
  IAssert(graph->IsOk());
  // dump the graph
  printf("Graph (%d, %d)\n", graph->GetNodes(), graph->GetEdges());

  // Get current time
  struct timeval tv1, tv2;
  gettimeofday(&tv1, NULL);

  // BFS
  TBreathFS<PGraph> bfs(graph, false);
  bfs.DoBfs(0, true, false);

  gettimeofday(&tv2, NULL);
  double timeDiff = timeInSeconds(tv1, tv2);

  printf("Time spent: %f\n\n", timeDiff);

  return 0;
}

