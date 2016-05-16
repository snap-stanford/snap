#include "stdafx.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
  typedef PNGraph PGraph; // directed graph

  printf("Creating graph for wikiTalk\n");

  PGraph graph = TSnap::LoadEdgeList<PGraph>("data/wiki-Talk.txt", 0, 1);
  IAssert(graph->IsOk());
  printf("Graph (%d, %d)\n", graph->GetNodes(), graph->GetEdges());

  printf("Getting max scc\n");
  PGraph maxScc = TSnap::GetMxScc(graph);
  printf("Scc (%d, %d)\n", maxScc->GetNodes(), maxScc->GetEdges());
  TFOut FOut("data/wikiTalk_scc.graph");
  maxScc->Save(FOut);

  return 0;
}

