#include "stdafx.h"
#include "agmfast.h"
#include "agmattr.h"
#include "agm.h"
#ifdef USE_OPENMP
#include <omp.h>
#endif

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("cesna. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm;
  Try
  TStr OutFPrx = Env.GetIfArgPrefixStr("-o:", "", "Output Graph data prefix");
  const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "./1912.edges", "Input edgelist file name");
  const TStr LabelFNm = Env.GetIfArgPrefixStr("-l:", "", "Input file name for node names (Node ID, Node label) ");
  const TStr AttrFNm = Env.GetIfArgPrefixStr("-a:", "./1912.nodefeat", "Input node attribute file name");
  const TStr ANameFNm = Env.GetIfArgPrefixStr("-n:", "./1912.nodefeatnames", "Input file name for node attribute names");
  int OptComs = Env.GetIfArgPrefixInt("-c:", 10, "The number of communities to detect (-1: detect automatically)");
  const int MinComs = Env.GetIfArgPrefixInt("-mc:", 3, "Minimum number of communities to try");
  const int MaxComs = Env.GetIfArgPrefixInt("-xc:", 20, "Maximum number of communities to try");
  const int DivComs = Env.GetIfArgPrefixInt("-nc:", 5, "How many trials for the number of communities");
  const int NumThreads = Env.GetIfArgPrefixInt("-nt:", 4, "Number of threads for parallelization");
  const double AttrWeight = Env.GetIfArgPrefixFlt("-aw:", 0.5, "We maximize (1 - aw) P(Network) + aw * P(Attributes)");
  const double LassoWeight = Env.GetIfArgPrefixFlt("-lw:", 1.0, "Weight for l-1 regularization on learning the logistic model parameters");
  const double StepAlpha = Env.GetIfArgPrefixFlt("-sa:", 0.05, "Alpha for backtracking line search");
  const double StepBeta = Env.GetIfArgPrefixFlt("-sb:", 0.3, "Beta for backtracking line search");
  const double MinFeatFrac = Env.GetIfArgPrefixFlt("-mf:", 0.0, "If the fraction of nodes with positive values for an attribute is smaller than this, we ignore that attribute");

#ifdef USE_OPENMP
  omp_set_num_threads(NumThreads);
#endif
  PUNGraph G;
  TIntStrH NIDNameH;
  TStrHash<TInt> NodeNameH;
  TVec<TFltV> Wck;
  TVec<TIntV> EstCmtyVV;
  if (InFNm.IsSuffix(".ungraph")) {
    TFIn GFIn(InFNm);
    G = TUNGraph::Load(GFIn);
  } else {
    G = TAGMUtil::LoadEdgeListStr<PUNGraph>(InFNm, NodeNameH);
    NIDNameH.Gen(NodeNameH.Len());
    for (int s = 0; s < NodeNameH.Len(); s++) { NIDNameH.AddDat(s, NodeNameH.GetKey(s)); }

  }
  if (LabelFNm.Len() > 0) {
    TSsParser Ss(LabelFNm, ssfTabSep);
    while (Ss.Next()) {
      if (Ss.Len() > 0) { NIDNameH.AddDat(Ss.GetInt(0), Ss.GetFld(1)); }
    }
  }
  printf("Graph: %d Nodes %d Edges\n", G->GetNodes(), G->GetEdges());

  //load attribute
  TIntV NIDV;
  G->GetNIdV(NIDV);
  THash<TInt, TIntV> RawNIDAttrH, NIDAttrH;
  TIntStrH RawFeatNameH, FeatNameH;
  if (ANameFNm.Len() > 0) {
    TSsParser Ss(ANameFNm, ssfTabSep);
    while (Ss.Next()) {
      if (Ss.Len() > 0) { RawFeatNameH.AddDat(Ss.GetInt(0), Ss.GetFld(1)); }
    }
  }

  TCesnaUtil::LoadNIDAttrHFromNIDKH(NIDV, AttrFNm, RawNIDAttrH, NodeNameH);
  TCesnaUtil::FilterLowEntropy(RawNIDAttrH, NIDAttrH, RawFeatNameH, FeatNameH, MinFeatFrac);

  TExeTm RunTm;
  TCesna CS(G, NIDAttrH, 10, 10);
  
  if (OptComs == -1) {
    printf("finding number of communities\n");
    OptComs = CS.FindComs(NumThreads, MaxComs, MinComs, DivComs, "", false, 0.1, StepAlpha, StepBeta);
  }

  CS.NeighborComInit(OptComs);
  CS.SetWeightAttr(AttrWeight);
  CS.SetLassoCoef(LassoWeight);
  if (NumThreads == 1 || G->GetEdges() < 1000) {
    CS.MLEGradAscent(0.0001, 1000 * G->GetNodes(), "", StepAlpha, StepBeta);
  } else {
    CS.MLEGradAscentParallel(0.0001, 1000, NumThreads, "", StepAlpha, StepBeta);
  }
  CS.GetCmtyVV(EstCmtyVV, Wck);
  TAGMUtil::DumpCmtyVV(OutFPrx + "cmtyvv.txt", EstCmtyVV, NIDNameH);
  FILE* F = fopen((OutFPrx + "weights.txt").CStr(), "wt");
  if (FeatNameH.Len() == Wck[0].Len()) {
    fprintf(F, "#");
    for (int k = 0; k < FeatNameH.Len(); k++) {
      fprintf(F, "%s", FeatNameH[k].CStr());
      if (k < FeatNameH.Len() - 1) { fprintf(F, "\t"); }
    }
    fprintf(F, "\n");
  }
  for (int c = 0; c < Wck.Len(); c++) {
    for (int k = 0; k < Wck[c].Len(); k++) {
      fprintf(F, "%f", Wck[c][k].Val);
      if (k < Wck[c].Len() - 1) { fprintf(F, "\t"); }
    }
    fprintf(F, "\n");
  }
  fclose(F);

  Catch

  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());

  return 0;
}
