#include <string.h>
#include "stdafx.h"
#include "rolx.h"

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("Rolx. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm;
  Try
  const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "graph.txt", "Input graph (one edge per line, tab/space separated)");
  const TStr OutFNm = Env.GetIfArgPrefixStr("-o:", "roles.txt", "Output file name prefix");
  const int MinRoles = Env.GetIfArgPrefixInt("-l:", 2, "Lower bound of the number of roles");
  const int MaxRoles = Env.GetIfArgPrefixInt("-u:", 3, "Upper bound of the number of roles");
  double Threshold = 1e-6;
  if (MinRoles > MaxRoles || MinRoles < 2) {
    printf("min roles and max roles should be integer and\n");
    printf("2 <= min roles <= max roles\n");
    exit(EXIT_SUCCESS);
  }
  printf("loading file...\n");
  PUNGraph Graph = TSnap::LoadEdgeList<PUNGraph>(InFNm, 0, 1);
  printf("extracting features...\n");
  TIntFtrH Features = ExtractFeatures(Graph);
  TIntIntH NodeIdMtxIdH = CreateNodeIdMtxIdxHash(Features);
  TFltVV V = ConvertFeatureToMatrix(Features, NodeIdMtxIdH);
  //printf("saving features...\n");
  //FPrintMatrix(V, "v.txt");
  printf("feature matrix is saved in v.txt\n");
  TFlt MnError = TFlt::Mx;
  TFltVV FinalG, FinalF;
  int NumRoles = -1;
  for (int r = MinRoles; r <= MaxRoles; ++r) {
    TFltVV G, F;
    printf("factorizing for %d roles...\n", r);
    CalcNonNegativeFactorization(V, r, G, F, Threshold);
    //FPrintMatrix(G, "g.txt");
    //FPrintMatrix(F, "f.txt");
    TFlt Error = CalcDescriptionLength(V, G, F);
    if (Error < MnError) {
      MnError = Error;
      FinalG = G;
      FinalF = F;
      NumRoles = r;
    }
  }
  //FPrintMatrix(FinalG, "final_g.txt");
  //FPrintMatrix(FinalF, "final_f.txt");
  printf("using %d roles, min error: %f\n", NumRoles, MnError());
  TIntIntH Roles = FindRoles(FinalG, NodeIdMtxIdH);
  FPrintRoles(Roles, OutFNm);
  //PlotRoles(Graph, Roles);
  Catch
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
  return 0;
}
