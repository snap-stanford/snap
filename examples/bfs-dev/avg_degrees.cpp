#include "stdafx.h"
#include <stdio.h>
#include "bfs-hybrid.h"

int main(int argc, char* argv[]) {
  typedef PNGraph PGraph;  //   directed graph

  printf("Creating graph for Twitter\n");

  TFIn FIn("twitter_scc.graph");
  PGraph graph = TNGraph::Load(FIn);
  IAssert(graph->IsOk());
  printf("Graph (%d, %d)\n", graph->GetNodes(), graph->GetEdges());

  int outDegrees = 0;
  int inDegrees = 0;
  for (TNGraph::TNodeI NodeI = graph->BegNI(); NodeI < graph->EndNI(); NodeI++) {
    outDegrees += NodeI.GetOutDeg();
    inDegrees += NodeI.GetInDeg();
  }

  float avgOut = (float)outDegrees / graph->GetNodes();
  float avgIn = (float)inDegrees / graph->GetNodes();

  printf("Average out degrees: %f\n", avgOut);
  printf("Average in degrees: %f\n", avgIn);
  
  return 0;
}

