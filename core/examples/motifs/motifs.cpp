#include "stdafx.h"
#include "../../snap/subgraphenum.h"
#include "../../snap/graphcounter.h"

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("Motifs. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm;
  Try
  
  const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "../as20graph.txt", "Input directed graph file (single directed edge per line)");
  const int MotifSz = Env.GetIfArgPrefixInt("-m:", 3, "Motif size (has to be 3 or 4)");
  const bool DrawMotifs = Env.GetIfArgPrefixBool("-d:", true, "Draw motif shapes (requires GraphViz)");
  TStr OutFNm = Env.GetIfArgPrefixStr("-o:", "", "Output file prefix");
  if (OutFNm.Empty()) { OutFNm = InFNm.GetFMid(); }
  EAssert(MotifSz==3 || MotifSz==4);

  // load graph
  PNGraph G;
  if (InFNm.GetFExt().GetLc()==".ungraph") {
    TFIn FIn(InFNm);  G=TSnap::ConvertGraph<PNGraph>(TUNGraph::Load(FIn), true); }
  else if (InFNm.GetFExt().GetLc()==".ngraph") {
    TFIn FIn(InFNm);  G=TNGraph::Load(FIn); }
  else {
    G = TSnap::LoadEdgeList<PNGraph>(InFNm, 0, 1); }
  bool IsOk = true;
  for (int nid = 0; nid < G->GetNodes(); nid++) {
    if (! G->IsNode(nid)) { IsOk=false; break; } }
  if (! IsOk) {
    printf("Nodes of the input graph have to be numbered 0...N-1\nRenumbering nodes...\n"); 
    PNGraph OG = G; G = TNGraph::New();
    TGraphEnumUtils::GetNormalizedGraph(OG, G);
  }
  // G = TSnap::GenRndGnm<PNGraph>(100, Kilo(1));
  
  // count frequency of connected subgraphs in G that have MotifSz nodes
  TD34GraphCounter GraphCounter(MotifSz);
  TSubGraphEnum<TD34GraphCounter> GraphEnum;
  GraphEnum.GetSubGraphs(G, MotifSz, GraphCounter);
  FILE *F = fopen(TStr::Fmt("%s-counts.tab", OutFNm.CStr()).CStr(), "wt");
  fprintf(F, "MotifId\tNodes\tEdges\tCount\n");
  for (int i = 0; i < GraphCounter.Len(); i++) {
    const int gid = GraphCounter.GetId(i);
    PNGraph SG = GraphCounter.GetGraph(gid);
    if (DrawMotifs) {
      TGraphViz::Plot(SG, gvlNeato, TStr::Fmt("%s-motif%03d.gif", OutFNm.CStr(), i), 
        TStr::Fmt("GId:%d  Count: %llu", gid, GraphCounter.GetCnt(gid)));
    }
    fprintf(F, "%d\t%d\t%d\t%llu\n", gid, SG->GetNodes(), SG->GetEdges(), GraphCounter.GetCnt(gid));
  }
  printf("done.");
  fclose(F); 
  
  Catch
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
  return 0;
}
