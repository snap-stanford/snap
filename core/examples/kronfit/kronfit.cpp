#include "stdafx.h"
#include "../../snap/kronecker.h"

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("Kronecker graphs. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm;
  Try
  Env = TEnv(argc, argv, TNotify::StdNotify);
  const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "../as20graph.txt", "Input graph file (single directed edge per line)");
  TStr OutFNm = Env.GetIfArgPrefixStr("-o:", "", "Output file prefix");
  const TInt NZero = Env.GetIfArgPrefixInt("-n0:", 2, "Innitiator matrix size");
  const TStr InitMtx = Env.GetIfArgPrefixStr("-m:", "0.9 0.7; 0.5 0.2", "Init Gradient Descent Matrix (R=random)").GetLc();
  const TStr Perm = Env.GetIfArgPrefixStr("-p:", "d", "Initial node permutation: d:Degree, r:Random, o:Order").GetLc();
  const TInt GradIter = Env.GetIfArgPrefixInt("-gi:", 50, "Gradient descent iterations");
  const TFlt LrnRate = Env.GetIfArgPrefixFlt("-l:", 1e-5, "Learning rate");
  const TFlt MnStep = Env.GetIfArgPrefixFlt("-mns:", 0.005, "Minimum gradient step");
  const TFlt MxStep = Env.GetIfArgPrefixFlt("-mxs:", 0.05, "Maximum gradient step");
  const TInt WarmUp =  Env.GetIfArgPrefixInt("-w:", 10000, "Samples to warm up");
  const TInt NSamples = Env.GetIfArgPrefixInt("-s:", 100000, "Samples per gradient estimation");
  //const TInt GradType = Env.GetIfArgPrefixInt("-gt:", 1, "1:Grad1, 2:Grad2");
  const bool ScaleInitMtx = Env.GetIfArgPrefixBool("-sim:", true, "Scale the initiator to match the number of edges");
  const TFlt PermSwapNodeProb = Env.GetIfArgPrefixFlt("-nsp:", 1.0, "Probability of using NodeSwap (vs. EdgeSwap) MCMC proposal distribution");
  if (OutFNm.Empty()) { OutFNm = TStr::Fmt("%s-fit%d", InFNm.GetFMid().CStr(), NZero()); }
  // load graph
  PNGraph G;
  if (InFNm.GetFExt().GetLc()==".ungraph") {
    TFIn FIn(InFNm);  G=TSnap::ConvertGraph<PNGraph>(TUNGraph::Load(FIn), true); }
  else if (InFNm.GetFExt().GetLc()==".ngraph") {
    TFIn FIn(InFNm);  G=TNGraph::Load(FIn); }
  else {
    G = TSnap::LoadEdgeList<PNGraph>(InFNm, 0, 1);
  }
  // fit
  TKronMtx InitKronMtx = InitMtx=="r" ? TKronMtx::GetRndMtx(NZero, 0.1) : TKronMtx::GetMtx(InitMtx);
  InitKronMtx.Dump("INIT PARAM", true);
  TKroneckerLL KronLL(G, InitKronMtx, PermSwapNodeProb);
  if (ScaleInitMtx) {
    InitKronMtx.SetForEdges(G->GetNodes(), G->GetEdges()); }
  KronLL.InitLL(G, InitKronMtx);
  InitKronMtx.Dump("SCALED PARAM", true);
  KronLL.SetPerm(Perm.GetCh(0));
  double LogLike = 0;
  //if (GradType == 1) {
  LogLike = KronLL.GradDescent(GradIter, LrnRate, MnStep, MxStep, WarmUp, NSamples);
  //} else if (GradType == 2) {
  //  LogLike = KronLL.GradDescent2(GradIter, LrnRate, MnStep, MxStep, WarmUp, NSamples); }
  //else{ Fail; }
  const TKronMtx& FitMtx = KronLL.GetProbMtx();
  FILE *F = fopen(TStr::Fmt("KronFit-%s.tab", InFNm.GetFMid().CStr()).CStr(), "at");
  fprintf(F, "Input\t%s\n", InFNm.CStr());
  TStrV ParamV; Env.GetCmLn().SplitOnAllCh(' ', ParamV);
  fprintf(F, "Command line options\n");
  for (int i = 0; i < ParamV.Len(); i++) {
    fprintf(F, "\t%s\n", ParamV[i].CStr()+(ParamV[i][0]=='-'?1:0)); }
  fprintf(F, "Loglikelihood\t%10.2f\n", LogLike);
  fprintf(F, "Absolute error (based on expected number of edges)\t%f\n", KronLL.GetAbsErr());
  fprintf(F, "RunTime\t%g\n", ExeTm.GetSecs());
  fprintf(F, "Estimated initiator\t%s\n", FitMtx.GetMtxStr().CStr());
  fclose(F);

  Catch
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
  return 0;
}
