#include "stdafx.h"
#include "../../snap/cliques.h"

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("k-cores. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm;
  Try
  
  const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "../as20graph.txt", "Input undirected graph file (single directed edge per line)");
  const bool Save = Env.GetIfArgPrefixBool("-s:", true, "Save the k-cores");
  TStr OutFNm = Env.GetIfArgPrefixStr("-o:", "", "Output file prefix");
  if (OutFNm.Empty()) { OutFNm = InFNm.GetFMid(); }

  PUNGraph G;
  // load graph
  if (InFNm.GetFExt().GetLc()==".ungraph") {
    TFIn FIn(InFNm);  G=TUNGraph::Load(FIn); }
  else if (InFNm.GetFExt().GetLc()==".ngraph") {
    TFIn FIn(InFNm);  G=TSnap::ConvertGraph<PUNGraph>(TNGraph::Load(FIn), false); }
  else {
    G = TSnap::LoadEdgeList<PUNGraph>(InFNm, 0, 1); }
  
  TKCore<PUNGraph> KCore(G);
  TIntPrV KCoreV;
  while (KCore.GetNextCore()!=0) {
    KCoreV.Add(TIntPr(KCore.GetCurK(), KCore.GetCoreNodes()));
    if (Save) {
      TSnap::SaveEdgeList(KCore.GetCoreG(), TStr::Fmt("%s-core%02d.txt", OutFNm.CStr(), KCore.GetCurK()), 
        TStr::Fmt("%d-core of graph %s", KCore.GetCurK(), InFNm.CStr()));
    }
  }
  TGnuPlot::PlotValV(KCoreV, "kcore-"+OutFNm, TStr::Fmt("K-core decomposition of graph %s (%d, %d)",
    OutFNm.CStr(), G->GetNodes(), G->GetEdges()), "K-core", "Number of nodes in k-core", gpsLog);
    
  Catch
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
  return 0;
}
