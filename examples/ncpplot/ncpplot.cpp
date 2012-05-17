#include "stdafx.h"
#include "ncp.h"

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("Network Community Profile Plot. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm;
  Try
  const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "../as20graph.txt", "Input undirected graph (one edge per line)");
  TStr OutFNm = Env.GetIfArgPrefixStr("-o:", "", "Output file name");
  TStr Desc = Env.GetIfArgPrefixStr("-d:", "", "Description");
  const int DrawWhisk = Env.GetIfArgPrefixInt("-d:", -1, "Draw largest D whiskers");
  const bool TakeCore = Env.GetIfArgPrefixBool("-k:", false, "Take core (strip away whiskers)");
  const bool DoWhisk = Env.GetIfArgPrefixBool("-w:", false, "Do bag of whiskers");
  const bool DoRewire = Env.GetIfArgPrefixBool("-r:", false, "Do rewired network");
  const bool SaveInfo = Env.GetIfArgPrefixBool("-s:", false, "Save info file (for each size store conductance, modulariy, ...)");
  const int KMin = Env.GetIfArgPrefixInt("-kmin:", 10, "minimum K (volume)");
  const int KMax = Env.GetIfArgPrefixInt("-kmax:", Mega(100), "maximum K (volume)");
  const int Coverage = Env.GetIfArgPrefixInt("-c:", 10, "coverage (so that every node is covered C times)");
  TLocClust::Verbose = Env.GetIfArgPrefixBool("-v:", true, "Verbose (plot intermediate output)");
  if (OutFNm.Empty()) { OutFNm = InFNm.GetFMid(); }
  if (Desc.Empty()) { Desc = OutFNm; }
  PUNGraph Graph = TSnap::GetMxWcc(TSnap::LoadEdgeList<PUNGraph>(InFNm,0,1));
  if (DrawWhisk > 0) {
    printf("*** Drawing whiskers (must have GraphViz installed)\n");
    TLocClust::DrawWhiskers(Graph, OutFNm, DrawWhisk);
  }
  if (TakeCore) {
    printf("Take bi-connected core: (%d, %d)  -->", Graph->GetNodes(), Graph->GetEdges());
    Graph = TSnap::GetMxBiCon(Graph);
    printf("  (%d, %d)\n", Graph->GetNodes(), Graph->GetEdges());
  }
  printf("*** Plotting network community profile (NCP)\n");
  //TLocClust::PlotNCP(Graph, OutFNm, , DoWhisk, DoRewire, KMin, KMax, Coverage, SaveInfo);
  TLocClust::PlotNCP(Graph, OutFNm, "Graph: "+InFNm+" -> "+OutFNm+" "+Desc,
    DoWhisk, DoRewire, KMin, KMax, Coverage, SaveInfo);
  Catch
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
  
   
  /* 
  // find communities around node id 0
  PUNGraph G = TUNGraph::GetSmallGraph();
  G->Dump();
  TLocClust LC(G, 0.001);
  LC.FindBestCut(0, 1000, 0.1);
  const TIntV& V = LC.GetNIdV();
  for (int i = 0; i < V.Len(); i++) {
    printf("%d\t%d\t%d\t%f\n", V[i], LC.GetVolV()[i], LC.GetCutV()[i], LC.GetPhiV()[i]);
  }
  //*/
  
  return 0;
}
