#include "stdafx.h"

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("Connected components. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm;
  Try
  const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "as20graph.txt", "Input file");
  const TStr OutFNm = Env.GetIfArgPrefixStr("-o:", "graph", "Output file prefix");
  if (Env.IsEndOfRun()) { return 0; }
  TExeTm ExeTm;
  PNGraph Graph;
  printf("Loading...\n");
  Graph = TSnap::LoadEdgeList<PNGraph>(InFNm);
  printf("Graph: %d nodes, %d edges\n", Graph->GetNodes(), Graph->GetEdges());
  TCnComV CnComV;
  // weakly connected components
  printf("Wccs...\n");
  TSnap::GetWccs(Graph, CnComV);
  TCnCom::SaveTxt(CnComV, TStr::Fmt("%s.wcc.txt", OutFNm.CStr()), "Weakly connected components");
  CnComV.Clr();
  // strongly connected components
  printf("Sccs...\n");
  TSnap::GetSccs(Graph, CnComV);
  TCnCom::SaveTxt(CnComV, TStr::Fmt("%s.scc.txt", OutFNm.CStr()), "Strongly connected components");
  CnComV.Clr();
  // biconnected connected components (components that remain connected if any single edge is removed)
  //   in other works: any pair of nodes can be connected by 2 disjoint paths
  printf("Biconnected components...\n");
  PUNGraph UG = TSnap::ConvertGraph<PUNGraph>(Graph); // convert graph to undirected
  TSnap::GetBiCon(UG, CnComV);
  TCnCom::SaveTxt(CnComV, TStr::Fmt("%s.bcc.txt", OutFNm.CStr()), "Biconnected connected components");
  // Articulation points (cut vertices) -- vertices that if removed disconnect the network
  printf("Articulation points...\n");
  { TIntV NIdV;
  TSnap::GetArtPoints(UG, NIdV);
  FILE *F=fopen(TStr::Fmt("%s.art.txt", OutFNm.CStr()).CStr(), "wt");
  fprintf(F, "# Articulation points (cut vertices)\n");
  for (int i = 0; i < NIdV.Len(); i++) {
    fprintf(F, "%d\n", NIdV[i].Val); }
  fclose(F); }
  // Bridge edges -- edges that if removed disconnect as component from a graph
  printf("Bridge edges...\n");
  { TIntPrV EdgeV;
  TSnap::GetEdgeBridges(UG, EdgeV);
  FILE *F=fopen(TStr::Fmt("%s.bridges.txt", OutFNm.CStr()).CStr(), "wt");
  fprintf(F, "# Bridge edges\n");
  for (int i = 0; i < EdgeV.Len(); i++) {
    fprintf(F, "%d\t%d\n", EdgeV[i].Val1(), EdgeV[i].Val2()); }
  fclose(F); }
  Catch
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
  return 0;
}
