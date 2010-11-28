#include "stdafx.h"

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("Network over time. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm;
  Try
  const TStr InFNm  = Env.GetIfArgPrefixStr("-i:", "graph*.txt", "Input graphs (each file is a graph snapshot, or \"DEMO\")");
  const TStr OutFNm  = Env.GetIfArgPrefixStr("-o:", "over-time", "Output file name prefix");
  const TStr Desc = Env.GetIfArgPrefixStr("-t:", "", "Description");
  const int TakeStat = Env.GetIfArgPrefixInt("-s:", 6, "How much statistics to calculate?\n\
    1:basic, 2:degrees, 3:no-diameter, 4:no-distributions, 5:no-svd, 6:all-statistics");
  // for each graph snapshot we keep full statistics
  TGStat::NDiamRuns = 1; // number of times the diameter algorithm is run
  PGStatVec GStatVec = TGStatVec::New(tmu1Sec, TGStat::BasicStat());
  switch (TakeStat) {
    case 1 : break;
    case 2 : GStatVec->SetTakeStat(TGStat::DegDStat()); break;
    case 3 : GStatVec->SetTakeStat(TGStat::NoDiamStat()); break;
    case 4 : GStatVec->SetTakeStat(TGStat::NoDistrStat()); break;
    case 5 : GStatVec->SetTakeStat(TGStat::NoSvdStat()); break;
    case 6 : GStatVec->SetTakeStat(TGStat::AllStat()); break;
    default: FailR("Bad -s: parameter.");
  }
  if (InFNm == "DEMO") {
    TFfGGen FF(false, 1, 0.36, 0.33, 1.0, 0.0, 0.0);
    FF.GenGraph(1000, GStatVec, true);
  } else {
    TFFile FFile(InFNm);  TStr FNm;
    for (int t = 0; FFile.Next(FNm); t++) {
      printf("Loading graph: %d: %s\n", t+1, FNm.CStr());
      PNGraph Graph = TSnap::LoadEdgeList<PNGraph>(InFNm, 0, 1);
      printf("nodes:%d  edges:%d\nCreating plots...\n", Graph->GetNodes(), Graph->GetEdges());
      GStatVec->Add(Graph, TSecTm(t+1), FNm.GetFBase());
    }
  }
  if (GStatVec->Len() == 0) { printf("No graph loaded.\n"); return 0; }
  // plot all temporal statistics
  GStatVec->PlotAllVsX(gsvTime, OutFNm, Desc);
  GStatVec->PlotAllVsX(gsvNodes, OutFNm, Desc);
  // plot properties of last graph in the sequence
  GStatVec->Last()->PlotAll(OutFNm, Desc);
  Catch
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
  return 0;
}
