#include <string.h>
#include "stdafx.h"
#include "rolx.h"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Usage: ./prototype <dataset file>\n");
    exit(EXIT_SUCCESS);
  }

  PUNGraph Graph = TSnap::LoadEdgeList<PUNGraph>(
      argv[1], 0, 1);
  printf("finish loading file\n");

  TIntFtrH Features = ExtractFeatures(Graph);
  printf("finish feature extraction\n");
  printf("--features--\n");
  //PrintFeatures(Features);
  TIntIntH NodeIdMtxIdH = CreateNodeIdMtxIdxHash(Features);
  printf("--finish create (node ID -> Mtx ID) hash--\n");
  TFltVV V = ConvertFeatureToMatrix(Features, NodeIdMtxIdH); 
  printf("--finish convert feature to matrix--\n");
  //printf("--feature matrix--\n");
  //FPrintMatrix(V, "v.txt");

  TFlt MnError = TFlt::Mx;
  TFltVV FinalG, FinalF;
  int NumRoles = -1;
  for (int r = 3; r < 4; ++r) {
    TFltVV G, F;
    time_t t = time(NULL);
    CalcNonNegativeFactorization(V, r, G, F);
    printf("Factorization use %f seconds\n", difftime(time(NULL), t));
    printf("finish factorization for r=%d\n", r);
    //PrintMatrix(G);
    TFlt Error = ComputeDescriptionLength(V, G, F);
    if (Error < MnError) {
      MnError = Error;
      FinalG = G;
      FinalF = F;
      NumRoles = r;
    }
  }
  //printf("--FinalG--\n");
  //PrintMatrix(FinalG);
  //printf("--FinalF--\n");
  //PrintMatrix(FinalF);
  printf("using %d roles, min error: %f\n", NumRoles, MnError());

  TIntIntH Roles = FindRoles(FinalG, NodeIdMtxIdH);
  FPrintRoles(Roles, "roles.txt");
  //PlotRoles(Graph, Roles);

  return 0;
}
