#include "stdafx.h"

int main(int argc, char* argv[]) {
  printf("ForestFire. build: %s, %s. Start time: %s\n\n", __TIME__, __DATE__, TExeTm::GetCurTm());
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs("Forest Fire");
  TExeTm ExeTm;
  Try
  const TStr OutFNm = Env.GetIfArgPrefixStr("-o:", "graph.txt", "Output graph file name");
  const int NNodes = Env.GetIfArgPrefixInt("-n:", 10000, "Number of nodes (size of the generated graph)");
  const double FwdProb = Env.GetIfArgPrefixFlt("-f:", 0.35, "Forward burning probability");
  const double BckProb = Env.GetIfArgPrefixFlt("-b:", 0.32, "Backward burning probability");
  const int StartNodes = Env.GetIfArgPrefixInt("-s:", 1, "Start graph with S isolated nodes");
  const double Take2AmbProb = Env.GetIfArgPrefixFlt("-a:", 0.0, "Probability of a new node choosing 2 ambassadors");
  const double OrphanProb = Env.GetIfArgPrefixFlt("-op:", 0.0, "Probability of a new node being an orphan (node with zero out-degree)");
  // generate forest fire graph
  TFfGGen::TimeLimitSec = -1;
  TFfGGen FF(false, StartNodes, FwdProb, BckProb, 1.0, Take2AmbProb, OrphanProb);
  FF.GenGraph(NNodes, false);
  TSnap::SaveEdgeList(FF.GetGraph(), OutFNm, FF.GetParamStr());
  Catch
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
  return 0;
}
