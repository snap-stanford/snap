#include "stdafx.h"
#include "../../snap/kronecker.h"

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("Kronecker graphs. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm;
  Try
  const TStr OutFNm  = Env.GetIfArgPrefixStr("-o:", "graph.txt", "Output graph file name"); 
  const TStr MtxNm = Env.GetIfArgPrefixStr("-m:", "0.9 0.5; 0.5 0.1", "Matrix (in Maltab notation)");
  const int NIter = Env.GetIfArgPrefixInt("-i:", 5, "Iterations of Kronecker product");
  const int Seed = Env.GetIfArgPrefixInt("-s:", 0, "Random seed (0 - time seed)");

  TKronMtx SeedMtx = TKronMtx::GetMtx(MtxNm);
  printf("\n*** Seed matrix:\n");
  SeedMtx.Dump();
  printf("\n*** Kronecker:\n");
  // slow but exact O(n^2) algorightm
  //PNGraph Graph = TKronMtx::GenKronecker(SeedMtx, NIter, true, Seed); 
  // fast O(e) approximate algorithm
  PNGraph Graph = TKronMtx::GenFastKronecker(SeedMtx, NIter, true, Seed); 
  // save edge list
  TSnap::SaveEdgeList(Graph, OutFNm, TStr::Fmt("Kronecker Graph: seed matrix [%s]", MtxNm.CStr()));
  Catch
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
  return 0;
}
