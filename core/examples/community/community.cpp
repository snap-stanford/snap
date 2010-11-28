#include "stdafx.h"

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("Network community detection. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm;
  Try
  const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "graph.txt", "Input graph (undirected graph)");
  const TStr OutFNm = Env.GetIfArgPrefixStr("-o:", "communities.txt", "Output file");
  const int CmtyAlg = Env.GetIfArgPrefixInt("-a:", 2, "Algorithm: 1:Girvan-Newman, 2:Clauset-Newman-Moore");

  PUNGraph Graph = TSnap::LoadEdgeList<PUNGraph>(InFNm, false);
  //PUNGraph Graph = TSnap::LoadEdgeList<PUNGraph>("../as20graph.txt", false);
  //PUNGraph Graph = TSnap::GenRndGnm<PUNGraph>(5000, 10000); // generate a random graph

  TSnap::DelSelfEdges(Graph);
  TCnComV CmtyV;
  double Q = 0.0;
  TStr CmtyAlgStr;
  if (CmtyAlg == 1) {
    CmtyAlgStr = "Girvan-Newman";
    Q = TSnap::CommunityGirvanNewman(Graph, CmtyV); }
  else if (CmtyAlg == 2) {
    CmtyAlgStr = "Cluset-Newman-Moore";
    Q = TSnap::CommunityCNM(Graph, CmtyV); }
  else { Fail; }

  FILE *F = fopen(OutFNm.CStr(), "wt");
  fprintf(F, "# Input: %s\n", InFNm.CStr());
  fprintf(F, "# Nodes: %d    Edges: %d\n", Graph->GetNodes(), Graph->GetEdges());
  fprintf(F, "# Algoritm: %s\n", CmtyAlgStr.CStr());
  fprintf(F, "# Modularity: %f\n", Q);
  fprintf(F, "# Communities: %d\n", CmtyV.Len());
  fprintf(F, "# NId\tCommunityId\n");
  for (int c = 0; c < CmtyV.Len(); c++) {
    for (int i = 0; i < CmtyV[c].Len(); i++) {
      fprintf(F, "%d\t%d\n", CmtyV[c][i].Val, c);
    }
  }
  fclose(F);

  Catch
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
  return 0;
}
