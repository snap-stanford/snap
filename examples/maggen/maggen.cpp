#include "stdafx.h"
#include "mag.h"

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("MAG graphs. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm;
  Try
  const TStr InFNm  = Env.GetIfArgPrefixStr("-i:", "graph.config", "Input graph configuration file name (except for Simplified MAG)"); 
  const TStr OutFNm  = Env.GetIfArgPrefixStr("-o:", "graph.txt", "Output graph file name"); 
  const TInt Seed = Env.GetIfArgPrefixInt("-s:", 1, "Random seed (0 - time seed)");
  const TInt Nodes = Env.GetIfArgPrefixInt("-n:", 10240, "The number of nodes");
  const TBool Simplified = Env.GetIfArgPrefixBool("-simple:", true, "Simplified MAG option");
  TInt NAttrs = Env.GetIfArgPrefixInt("-l:", 8, "The number of attributes (only for Simplified MAG)");
  const TFlt Mu = Env.GetIfArgPrefixFlt("-mu:", 0.45, "Equalized Bernoulli parameter for every node attribute (only for Simplified MAG)");
  const TStr MtxStr = Env.GetIfArgPrefixStr("-m:", "0.8 0.35;0.35 0.15", "Equalized affinity matrix for every node attribute (only for Simplified MAG)");

  TIntVV AttrVV;
  PNGraph Graph;
  TStr GraphLabel;

  if(Simplified) {
	  printf("\n*** Simplified MAG:\n");
	  printf("    N = %d  L = %d  Mu = %.4f  AffMtx = [%s]\n", Nodes(), NAttrs(), Mu(), MtxStr.CStr());
	  GraphLabel = TStr::Fmt("Simplified MAG(%d, %d, %.4f, [%s])", Nodes(), NAttrs(), Mu(), MtxStr.CStr());

	  TMAGNodeSimple SimNode(NAttrs, Mu);
	  TMAGParam<TMAGNodeSimple> SimMAG(Nodes, NAttrs);
	  SimMAG.SetMtxAll(TMAGAffMtx::GetMtx(MtxStr));
	  SimMAG.SetNodeAttr(SimNode);
	  
	  printf("\n*** MAG generating... ");
	  Graph = SimMAG.GenMAG(AttrVV, false, Seed);
	  printf("%d edges created...\n", Graph->GetEdges());

  } else {  // general MAG with Bernoulli node attributes
	  TMAGParam<TMAGNodeBern> MAG(Nodes, InFNm);
	  
	  printf("\n*** MAG generating... ");
	  Graph = MAG.GenMAG(AttrVV, false, Seed);
	  printf("%d edges created...\n", Graph->GetEdges());
  }
  
  // save edge list
  TSnap::SaveEdgeList(Graph, OutFNm, GraphLabel);
  Catch
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
  return 0;
}
