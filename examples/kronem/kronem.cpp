#include "stdafx.h"
#include "kronecker.h"

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("Kronecker graphs. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm, IterTm;

  Try
  Env = TEnv(argc, argv, TNotify::StdNotify);
  const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "../as20graph.txt", "Input graph file (single directed edge per line)");
  TStr OutFNm = Env.GetIfArgPrefixStr("-o:", "", "Output file prefix");
  const TInt NZero = Env.GetIfArgPrefixInt("-n0:", 2, "Initiator matrix size");
  const TStr InitMtx = Env.GetIfArgPrefixStr("-m:", "0.9 0.7; 0.5 0.2", "Init Gradient Descent Matrix (R=random)").GetLc();
  const TInt GradIter = Env.GetIfArgPrefixInt("-gi:", 5, "Gradient descent iterations for M-step");
  const TInt EMIter = Env.GetIfArgPrefixInt("-ei:", 30, "EM iterations");
  const TFlt LrnRate = Env.GetIfArgPrefixFlt("-l:", 1e-5, "Learning rate");
  const TFlt MnStep = Env.GetIfArgPrefixFlt("-mns:", 0.001, "Minimum gradient step for M-step");
  const TFlt MxStep = Env.GetIfArgPrefixFlt("-mxs:", 0.008, "Maximum gradient step for M-step");
  const TInt WarmUp =  Env.GetIfArgPrefixInt("-w:", 8000, "Samples for MCMC warm-up");
  const TInt NSamples = Env.GetIfArgPrefixInt("-s:", 2000, "Samples per gradient estimation");
  const bool ScaleInitMtx = Env.GetIfArgPrefixBool("-sim:", false, "Scale the initiator to match the number of edges");
  const TFlt PermSwapNodeProb = Env.GetIfArgPrefixFlt("-nsp:", 0.6, "Probability of using NodeSwap (vs. EdgeSwap) MCMC proposal");
  const TBool Debug = Env.GetIfArgPrefixBool("-debug:", false, "Debug mode");
//  const TStr Perm = Env.GetIfArgPrefixStr("-p:", "r", "Permutation: d:Degree, r:Random, o:Order, b:BestMatched").GetLc();
//  const TStr TypeStr = Env.GetIfArgPrefixStr("-t:", "d", "KronEM type: d:denoise, o:denoise with ordered removal p:process e:edges").GetLc();
  
  if (OutFNm.Empty()) { OutFNm = TStr::Fmt("%s-kronem%d", InFNm.GetFMid().CStr(), NZero()); }
  
  // Load graph
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
//  KronLL.SetPerm(Perm.GetCh(0));
  KronLL.SetDebug(Debug);

  KronLL.RunKronEM(EMIter, GradIter, LrnRate, MnStep, MxStep, WarmUp, WarmUp, NSamples);

  const TKronMtx& FitMtx = KronLL.GetProbMtx();
  FILE *F = fopen(TStr::Fmt("KronEM-%s.tab", InFNm.GetFMid().CStr()).CStr(), "at");
  fprintf(F, "Input\t%s\n", InFNm.CStr());
  TStrV ParamV; Env.GetCmLn().SplitOnAllCh(' ', ParamV);
  fprintf(F, "Command line options\n");
  for (int i = 0; i < ParamV.Len(); i++) {
    fprintf(F, "\t%s\n", ParamV[i].CStr()+(ParamV[i][0]=='-'?1:0)); }
  fprintf(F, "Loglikelihood\t%10.2f\n", KronLL.CalcApxGraphLL());
//  fprintf(F, "Absolute error (based on expected number of edges)\t%f\n", KronLL.GetAbsErr());
  fprintf(F, "RunTime\t%g\n", ExeTm.GetSecs());
  fprintf(F, "Estimated initiator\t%s\n", FitMtx.GetMtxStr().CStr());
  fclose(F);

  Catch
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
  return 0;
}
