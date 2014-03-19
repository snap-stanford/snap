#include <string.h>
#include "stdafx.h"
#include "rolx.h"

int main(int argc, char* argv[]) {
  if (argc < 4) {
    printf("Usage: ./prototype <dataset file> <min roles> <max roles>\n");
    exit(EXIT_SUCCESS);
  }
  int min_roles = atoi(argv[2]);
  int max_roles = atoi(argv[3]);
  if (min_roles > max_roles || min_roles < 2) {
    printf("min roles and max roles should be integer and\n");
	printf("2 <= min roles <= max roles\n");
	exit(EXIT_SUCCESS);
  }
  printf("loading file...\n");
  PUNGraph Graph = TSnap::LoadEdgeList<PUNGraph>(argv[1], 0, 1);
  printf("extracting features...\n");
  TIntFtrH Features = ExtractFeatures(Graph);
  TIntIntH NodeIdMtxIdH = CreateNodeIdMtxIdxHash(Features);
  TFltVV V = ConvertFeatureToMatrix(Features, NodeIdMtxIdH);
  printf("saving features...\n");
  FPrintMatrix(V, "v.txt");
  printf("feature matrix is saved in v.txt\n");
  TFlt MnError = TFlt::Mx;
  TFltVV FinalG, FinalF;
  int NumRoles = -1;
  for (int r = min_roles; r <= max_roles; ++r) {
    TFltVV G, F;
	printf("factorizing for %d roles...\n", r);
    time_t t = time(NULL);
    CalcNonNegativeFactorization(V, r, G, F);
    printf("Factorization uses %f seconds\n", difftime(time(NULL), t));
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
  FPrintRoles(Roles, "roles.txt");
  //PlotRoles(Graph, Roles);
  return 0;
}
