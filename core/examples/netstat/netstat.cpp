#include "stdafx.h"

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("GraphInfo. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm;
  Try
  const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "graph.txt", "Input graph (one edge per line, tab/space separated)");
  const TBool IsDir = Env.GetIfArgPrefixBool("-d:", true, "Directed graph");
  const TStr OutFNm = Env.GetIfArgPrefixStr("-o:", "graph", "Output file prefix");
  const TStr Desc = Env.GetIfArgPrefixStr("-t:", "", "Title (description)");
  const TStr Plot = Env.GetIfArgPrefixStr("-p:", "cdhwsCvV", "What statistics to plot string:"
    "\n\tc: cummulative degree distribution"
    "\n\td: degree distribution"
    "\n\th: hop plot (diameter)"
    "\n\tw: distribution of weakly connected components"
    "\n\ts: distribution of strongly connected components"
    "\n\tC: clustering coefficient"
    "\n\tv: singular values"
    "\n\tV: left and right singular vector\n\t");
  bool PlotDD, PlotCDD, PlotHop, PlotWcc, PlotScc, PlotSVal, PlotSVec, PlotClustCf;
  PlotDD = Plot.SearchCh('d') != -1;
  PlotCDD = Plot.SearchCh('c') != -1;
  PlotHop = Plot.SearchCh('h') != -1;
  PlotWcc = Plot.SearchCh('w') != -1;
  PlotScc = Plot.SearchCh('s') != -1;
  PlotClustCf = Plot.SearchCh('C') != -1;
  PlotSVal = Plot.SearchCh('v') != -1;
  PlotSVec = Plot.SearchCh('V') != -1;
  if (Env.IsEndOfRun()) { return 0; }
  //PNGraph G = TGGen<PNGraph>::GenRMat(1000, 3000, 0.40, 0.25, 0.2);
  //G->SaveEdgeList("graph.txt", "RMat graph (a:0.40, b:0.25, c:0.20)");
  printf("Loading...");
  PNGraph Graph = TSnap::LoadEdgeList<PNGraph>(InFNm);
  if (! IsDir) { TSnap::MakeUnDir(Graph); }
  printf("nodes:%d  edges:%d\nCreating plots...\n", Graph->GetNodes(), Graph->GetEdges());
  const int Vals = Graph->GetNodes()/2 > 200 ? 200 : Graph->GetNodes()/2;
  if (PlotDD) {
    TSnap::PlotOutDegDistr(Graph, OutFNm, Desc, false, false);
    TSnap::PlotInDegDistr(Graph, OutFNm, Desc, false, false);
  }
  if (PlotCDD) {
    TSnap::PlotOutDegDistr(Graph, OutFNm, Desc, true, false);
    TSnap::PlotInDegDistr(Graph, OutFNm, Desc, true, false);
  }
  if (PlotHop) {
    TSnap::PlotHops(Graph, OutFNm, Desc, false, 32);
  }
  if (PlotWcc) {
    TSnap::PlotWccDistr(Graph, OutFNm, Desc);
  }
  if (PlotScc) {
    TSnap::PlotSccDistr(Graph, OutFNm, Desc);
  }
  if (PlotClustCf) {
    TSnap::PlotClustCf(Graph, OutFNm, Desc);
  }
  if (PlotSVal) {
    TSnap::PlotSngValRank(Graph, Vals, OutFNm, Desc);
  }
  if(PlotSVec) {
    TSnap::PlotSngVec(Graph, OutFNm, Desc);
  }
  Catch
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
  return 0;
}

