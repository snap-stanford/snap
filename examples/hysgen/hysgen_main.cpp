#include "hysgen.h"
#include "agm.h"

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("HySGen. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm;
  
  Try
  
  const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "./synthetic_data/synthetic.hyperedges", "Input [hyper]edgelist file url.");
  const TStr OutFPrx = Env.GetIfArgPrefixStr("-o:", "./synthetic_res", "Output file url + name prefix for the discovered communities.");
  int OptComs = Env.GetIfArgPrefixInt("-c:", 2, "The number of communities to detect.");
  const TStr OutPlt = Env.GetIfArgPrefixStr("-op:", "", "Output file performance plot (empty for no plot).");
  const TStr InitComFNm = Env.GetIfArgPrefixStr("-ci:", "", "Community initialization file url.");
  const TStr LabelFNm = Env.GetIfArgPrefixStr("-l:", "", "Input file name for node names (Node ID, Node label).");
  const int MinComSize = Env.GetIfArgPrefixInt("-mc:", 3, "Minimum size of the communities.");
  const int RndSeed = Env.GetIfArgPrefixInt("-rs:", 0, "Random Seed.");
  int MaxIter = Env.GetIfArgPrefixInt("-xi:", 1000, "Maximum number of iterations.");
  const double InitComS = Env.GetIfArgPrefixFlt("-ic:", 0.1, "Initial membership value for the initially assigned communities.");
  const double InitNulS = Env.GetIfArgPrefixFlt("-in:", 0.0, "The default membership value of each node to all the communities.");
  double PerturbDensity = Env.GetIfArgPrefixFlt("-rp:", 0.0, "Ratio of initial memberships to be randomly perturbed.");
  const double RegCoef = Env.GetIfArgPrefixFlt("-rw:", 0.005, "Weight for l-1 regularization on learning the model parameters.");
  const double StepSize = Env.GetIfArgPrefixFlt("-sz:", 1.0, "Initial step size for backtracking line search.");
  const double StepCtrlParam = Env.GetIfArgPrefixFlt("-sa:", 1.0, "Control parameter for backtracking line search.");
  const double StepReductionRatio = Env.GetIfArgPrefixFlt("-sr:", 0.5, "Step-size reduction ratio for backtracking line search.");
  const double Threshold = Env.GetIfArgPrefixFlt("-th:", MAX(0.01, RegCoef), "Cut-off threshold for the final community membership values.");


  PHGraph G;
  TIntStrH NIDNameH, NIDEdgelistnameH;
  TStrIntH NameNIdH, EdgelistnameNIdH;
  TStrHash<TInt> NodeNameH;
  TVec<TFltV> WckVV;
  TVec<TIntFltH> EstCmtyVH;
  TVec<TIntV> EstCmtyVV;
  if (InFNm.IsSuffix(".hgraph")) {
    TFIn GFIn(InFNm);
    G = THGraph::Load(GFIn);
  } else {
    G = THysgenUtil::LoadEdgeList(InFNm, NodeNameH);
    NIDNameH.Gen(NodeNameH.Len()); NIDEdgelistnameH.Gen(NodeNameH.Len());
    NameNIdH.Gen(NodeNameH.Len()); EdgelistnameNIdH.Gen(NodeNameH.Len());
    for (int s = 0; s < NodeNameH.Len(); s++) {
      NIDNameH.AddDat(s, NodeNameH.GetKey(s));
      NIDEdgelistnameH.AddDat(s, NodeNameH.GetKey(s));
      NameNIdH.AddDat(NodeNameH.GetKey(s), s);
      EdgelistnameNIdH.AddDat(NodeNameH.GetKey(s), s);
    }
  }
  if (LabelFNm.Len() > 0) {
    TSsParser Ss(LabelFNm, ssfTabSep);
    while (Ss.Next()) {
      if (Ss.Len() > 1) {NIDNameH.AddDat(NameNIdH.GetDat(Ss[0]), Ss.GetFld(1)); }
    }
  }
  printf("HyperGraph: %d Nodes %d Edges\n", G->GetNodes(), G->GetEdges());
  
  TIntV NIDV;
  G->GetNIdV(NIDV);
  
  TExeTm RunTm;
  THysgen Optimizer(G, RndSeed, InitComS, InitNulS);
  Optimizer.ComInit(OptComs, MinComSize, PerturbDensity);
  if (InitComFNm.Len() > 0) {
    Optimizer.LoadComInit(InitComFNm);
  }
  Optimizer.SetRegCoef(RegCoef);
  
  Optimizer.GetCmtyVV(EstCmtyVH, EstCmtyVV, WckVV, InitNulS, MinComSize);
  THysgenUtil::DumpCmtyVH(OutFPrx + "_cmtyvv_init.txt", EstCmtyVH, NIDNameH, THysgenUtil::Alphabetical);
  
  Optimizer.MLEGradAscent(1.0, MaxIter * G->GetNodes(), OutPlt, StepSize, StepCtrlParam, StepReductionRatio);
  Optimizer.GetCmtyVV(EstCmtyVH, EstCmtyVV, WckVV, Threshold, MinComSize);
  
  THysgenUtil::DumpCmtyVH(OutFPrx + "_cmty_SrtById_IdValues.txt", EstCmtyVH, NIDEdgelistnameH, THysgenUtil::Alphabetical);
  THysgenUtil::DumpCmtyVH(OutFPrx + "_cmty_SrtByVals_IdValues.txt", EstCmtyVH, NIDEdgelistnameH, THysgenUtil::Value);
  THysgenUtil::DumpCmtyVH(OutFPrx + "_cmty_SrtByVals_NameValues.txt", EstCmtyVH, NIDNameH, THysgenUtil::Value);
  THysgenUtil::DumpCmtyVV(OutFPrx + "_cmty_SrtByName_Names.txt", EstCmtyVV, NIDNameH);

  Catch

  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());

  return 0;
}
