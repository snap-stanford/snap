#include "stdafx.h"
#include "agmfast.h"
#include "agmdirected.h"
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
  const int IsUndirected = Env.GetIfArgPrefixInt("-g:", 0, "Input graph type. 0:directed, 1:undirected");
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
  PNGraph G;
  TIntStrH NIDNameH;
  if (IsUndirected == 1) {
    PUNGraph UG;
    if (InFNm.IsSuffix(".ungraph")) {
      TFIn GFIn(InFNm);
      UG = TUNGraph::Load(GFIn);
      
    } else {
      UG = TAGMUtil::LoadEdgeListStr<PUNGraph>(InFNm, NIDNameH);
    }
    G = TSnap::ConvertGraph<PNGraph, PUNGraph>(UG);
  } else {
    if (InFNm.IsSuffix(".ngraph")) {
      TFIn GFIn(InFNm);
      G = TNGraph::Load(GFIn);
      
    } else {
      G = TAGMUtil::LoadEdgeListStr<PNGraph>(InFNm, NIDNameH);
    }
  }
  if (LabelFNm.Len() > 0) {
    TSsParser Ss(LabelFNm, ssfTabSep);
    while (Ss.Next()) {
      if (Ss.Len() > 0) { NIDNameH.AddDat(Ss.GetInt(0), Ss.GetFld(1)); }
    }
  }
  printf("Graph: %d Nodes %d Edges\n", G->GetNodes(), G->GetEdges());
  
  TVec<TIntV> EstCmtyVVIn, EstCmtyVVOut;
  TExeTm RunTm;
  TCoda CD(G, 10, 10);
  
  if (OptComs == -1) {
    printf("finding number of communities\n");
    OptComs = CD.FindComsByCV(NumThreads, MaxComs, MinComs, DivComs, OutFPrx, StepAlpha, StepBeta);
  }

  CD.NeighborComInit(OptComs);
  if (NumThreads == 1 || G->GetEdges() < 1000) {
    CD.MLEGradAscent(0.0001, 1000 * G->GetNodes(), "", StepAlpha, StepBeta);
  } else {
    CD.MLEGradAscentParallel(0.0001, 1000, NumThreads, "", StepAlpha, StepBeta);
  }
  CD.GetCmtyVV(EstCmtyVVOut, EstCmtyVVIn);
  TAGMUtil::DumpCmtyVV(OutFPrx + "cmtyvv.out.txt", EstCmtyVVOut, NIDNameH);
  TAGMUtil::DumpCmtyVV(OutFPrx + "cmtyvv.in.txt", EstCmtyVVIn, NIDNameH);

  Catch

  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());

  return 0;
}
