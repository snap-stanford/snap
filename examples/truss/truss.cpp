#include "stdafx.h"
#include "truss.h"
#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "../as20graph.txt", "Input un/directed graph");

  FILE *F = fopen("truss-output.txt", "wt");     // file for output
  fprintf(F, "FromNodeId\tToNodeId\tTruss\n");

  printf("Loading %s...", InFNm.CStr());
  PNGraph Graph = TSnap::LoadEdgeList<PNGraph>(InFNm);  // load graph from file
  PUNGraph UGraph = TSnap::ConvertGraph<PUNGraph>(Graph); // undirected version of the input graph from file
  printf("\n#Nodes:%d, #Edges:%d\n", UGraph->GetNodes(), UGraph->GetEdges());

  // Compute truss.
  TExeTm tm1;  // timing
  TIntPrIntH Truss;
  GetTruss(UGraph, Truss);
  printf("Compute truss: %f seconds\n", tm1.GetSecs());

  // Save truss output to file.
  for (TIntPrIntH::TIter it = Truss.BegI(); it < Truss.EndI(); it++) {
    fprintf(F, "%d\t%d\t%d\n", it.GetKey().GetVal1(), it.GetKey().GetVal2(),
      it.GetDat());
  }

  printf("Truss output results have been written to file.\n");

  return 0;
}
