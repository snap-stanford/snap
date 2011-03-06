#include "stdafx.h"
#include "../../snap/cliques.h"

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("Clique Percolation Method. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm;
  Try
  
  const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "../as20graph.txt", "Input undirected graph file (single directed edge per line)");
  const int OverlapSz = Env.GetIfArgPrefixInt("-k:", 2, "Min clique overlap");
  TStr OutFNm = Env.GetIfArgPrefixStr("-o:", "", "Output file prefix");
  if (OutFNm.Empty()) { OutFNm = InFNm.GetFMid(); }

  PUNGraph G;
  if (InFNm == "DEMO") { // small demo graph
    G = TUNGraph::New();
    for (int i = 1; i < 8; i++) { G->AddNode(i); }
    G->AddEdge(1,2);
    G->AddEdge(2,3); G->AddEdge(2,4);
    G->AddEdge(3,4);
    G->AddEdge(4,5); G->AddEdge(4,7);
    G->AddEdge(5,6); G->AddEdge(5,7);
    G->AddEdge(6,7);
    // draw the small graph using GraphViz
    TGraphViz::Plot(G, gvlNeato, "small_graph.png", "", true); 
  }
  // load graph
  else if (InFNm.GetFExt().GetLc()==".ungraph") {
    TFIn FIn(InFNm);  G=TUNGraph::Load(FIn); }
  else if (InFNm.GetFExt().GetLc()==".ngraph") {
    TFIn FIn(InFNm);  G=TSnap::ConvertGraph<PUNGraph>(TNGraph::Load(FIn), false); }
  else {
    G = TSnap::LoadEdgeList<PUNGraph>(InFNm, 0, 1); }
  // find communities
  TVec<TIntV> CmtyV;
  TCliqueOverlap::GetCPMCommunities(G, OverlapSz+1, CmtyV);
  // save result
  FILE *F = fopen(TStr::Fmt("cpm-%s.txt", OutFNm.CStr()).CStr(), "wt");
  fprintf(F, "# %d Overlapping Clique Percolation Communities (min clique overlap %d)\n", CmtyV.Len(), OverlapSz);
  fprintf(F, "# Each line contains nodes belonging to the same community community\n");
  for (int i = 0; i < CmtyV.Len(); i++) {
    fprintf(F, "%d", CmtyV[i][0].Val);
    for (int j = 1; j < CmtyV[i].Len(); j++) {
      fprintf(F, "\t%d", CmtyV[i][j].Val);
    }
    fprintf(F, "\n");
  }
  Catch
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
  return 0;
}
