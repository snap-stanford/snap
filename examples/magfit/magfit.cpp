#include "stdafx.h"
#include "mag.h"

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  TExeTm ExeTm;
  Try
  const TStr InFNm  = Env.GetIfArgPrefixStr("-i:", "../as20graph.txt", "Input graph file name"); 
  const TStr ParamFNm  = Env.GetIfArgPrefixStr("-pi:", "init.config", "Initial parameter config file name"); 
  const int Seed = Env.GetIfArgPrefixInt("-s:", 1, "Random seed (0 - time seed)");
  const double LrnRate = Env.GetIfArgPrefixFlt("-lrn:", 0.0001, "Learning rate for Theta parameter");
  const double Lambda = Env.GetIfArgPrefixFlt("-lambda:", 0.1, "Regularization coefficient");
  const double MaxGrad = Env.GetIfArgPrefixFlt("-max:", 0.02, "Max gradient step");
  const double ReInit = Env.GetIfArgPrefixFlt("-ri:", 0.1, "Reinitialization rate");
  TStr OutFNm = Env.GetIfArgPrefixStr("-o:", "", "Output file name");
  const int Nstep = Env.GetIfArgPrefixInt("-ns:", 15, "# of EM iterations");
  const int NEstep = Env.GetIfArgPrefixInt("-es:", 5, "# of iterations in E-step");
  const int NMstep = Env.GetIfArgPrefixInt("-ms:", 5, "# of iterations in M-step");
  const bool Debug = Env.GetIfArgPrefixBool("-debug:", false, "Turns on the debugging option");
  const bool FastEstep = Env.GetIfArgPrefixBool("-fe:", true, "Turns on the fast E-step");
  const bool FastMstep = Env.GetIfArgPrefixBool("-fm:", true, "Turns on the fast M-step");
  
  if (OutFNm.Empty()) { OutFNm = TStr::Fmt("%s-magfit", InFNm.GetFMid().CStr()); }
  
  PNGraph Graph;
  if (InFNm.GetFExt().GetLc()==".ungraph") {
	  TFIn FIn(InFNm);
	  Graph = TSnap::ConvertGraph<PNGraph>(TUNGraph::Load(FIn), true);
  } else if (InFNm.GetFExt().GetLc()==".ngraph") {
	  TFIn FIn(InFNm);
	  Graph = TNGraph::Load(FIn);
  } else {
	  Graph = TSnap::LoadEdgeList<PNGraph>(InFNm, 0, 1);
  }
  TIntV NIdV;  Graph->GetNIdV(NIdV);
  Graph = TSnap::GetSubGraph(Graph, NIdV, true);
  // const int Nodes = Graph->GetNodes();

  ExeTm.Tick();
  printf("\n*** MAGfit starting... \n");
  printf("%d nodes %d edges ...\n", Graph->GetNodes(), Graph->GetEdges());
	  
  TMAGFitBern MAGFit(Graph, ParamFNm);
  TMAGNodeBern::Rnd.PutSeed(Seed);
  
  MAGFit.SetAlgConf(FastEstep, FastMstep);
  MAGFit.SetDebug(Debug);
  
  MAGFit.DoEMAlg(Nstep, NEstep, NMstep, LrnRate, MaxGrad, Lambda, ReInit);
  MAGFit.SaveTxt(OutFNm);
  
  printf("Running time = %.0f\n", ExeTm.GetSecs());
  
  Catch
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
  return 0;
}
