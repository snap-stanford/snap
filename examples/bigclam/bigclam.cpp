// bigclam.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "agmfast.h"
#include "agm.h"
#ifdef USE_OPENMP
#include <omp.h>
#endif

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("ragm. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm;
  Try
  TStr OutFPrx = Env.GetIfArgPrefixStr("-o:", "", "Output Graph data prefix");
  const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "../as20graph.txt", "Input edgelist file name");
  const TStr LabelFNm = Env.GetIfArgPrefixStr("-l:", "", "Input file name for node names (Node ID, Node label) ");
  int OptComs = Env.GetIfArgPrefixInt("-c:", 100, "The number of communities to detect (-1: detect automatically)");
  const int MinComs = Env.GetIfArgPrefixInt("-mc:", 5, "Minimum number of communities to try");
  const int MaxComs = Env.GetIfArgPrefixInt("-xc:", 100, "Maximum number of communities to try");
  const int DivComs = Env.GetIfArgPrefixInt("-nc:", 10, "How many trials for the number of communities");
  const int NumThreads = Env.GetIfArgPrefixInt("-nt:", 4, "Number of threads for parallelization");
  const double StepAlpha = Env.GetIfArgPrefixFlt("-sa:", 0.05, "Alpha for backtracking line search");
  const double StepBeta = Env.GetIfArgPrefixFlt("-sb:", 0.3, "Beta for backtracking line search");

#ifdef USE_OPENMP
  omp_set_num_threads(NumThreads);
#endif
  PUNGraph G;
  TIntStrH NIDNameH;
  if (InFNm.IsSuffix(".ungraph")) {
    TFIn GFIn(InFNm);
    G = TUNGraph::Load(GFIn);
  } else if (LabelFNm.Len() > 0) {
    G = TSnap::LoadEdgeList<PUNGraph>(InFNm);
    TSsParser Ss(LabelFNm, ssfTabSep);
    while (Ss.Next()) {
      if (Ss.Len() > 0) { NIDNameH.AddDat(Ss.GetInt(0), Ss.GetFld(1)); }
    }
  } else {
    G = TAGMUtil::LoadEdgeListStr<PUNGraph>(InFNm, NIDNameH);
  }
  printf("Graph: %d Nodes %d Edges\n", G->GetNodes(), G->GetEdges());
  
  TVec<TIntV> EstCmtyVV;
  TExeTm RunTm;
  TAGMFast RAGM(G, 10, 10);
  
  if (OptComs == -1) {
    printf("finding number of communities\n");
    OptComs = RAGM.FindComsByCV(NumThreads, MaxComs, MinComs, DivComs, OutFPrx, StepAlpha, StepBeta);
  }

  RAGM.NeighborComInit(OptComs);
  if (NumThreads == 1 || G->GetEdges() < 1000) {
    RAGM.MLEGradAscent(0.0001, 1000 * G->GetNodes(), "", StepAlpha, StepBeta);
  } else {
    RAGM.MLEGradAscentParallel(0.0001, 1000, NumThreads, "", StepAlpha, StepBeta);
  }
  RAGM.GetCmtyVV(EstCmtyVV);
  TAGMUtil::DumpCmtyVV(OutFPrx + "cmtyvv.txt", EstCmtyVV, NIDNameH);
  TAGMUtil::SaveGephi(OutFPrx + "graph.gexf", G, EstCmtyVV, 1.5, 1.5, NIDNameH);

  Catch

  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());

  return 0;
}
