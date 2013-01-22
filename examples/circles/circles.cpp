#include "stdafx.h"
#include "circles.h"

int main(int argc, char** argv) {
  // Input Parameters
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("Circles. build: %s, %s. Time: %s",
                         __TIME__, __DATE__, TExeTm::GetCurTm()));
  const TStr InEdges =
  Env.GetIfArgPrefixStr("-e:", "fb1.edges", "Input graph (undirected)");
  const TStr INFeatures =
  Env.GetIfArgPrefixStr("-f:", "fb1.feat", "Input node Features");
  const TStr InGT =
  Env.GetIfArgPrefixStr("-g:", "fb1.circles", "Groundtruth circles");
  const TInt K =
  Env.GetIfArgPrefixInt("-K:", 4, "Number of circles to detect");
  const TFlt Lambda =
  Env.GetIfArgPrefixFlt("-l:", 1, "Regularization Parameter Lambda");

  // Load an example graph
  PUNGraph Graph = TSnap::LoadEdgeList<PUNGraph>(InEdges);
  // Load attributes for that graph
  PGraphAttributes PGA = new TGraphAttributes(Graph, INFeatures.CStr(),
                                              InGT.CStr());
  // Predict K circles
  PCluster PC = new TCluster(PGA, K, Lambda);
  // Train for 50 iterations of coordinate ascent,
  // with 100 iterations of gradient ascent, and 100 iterations of MCMC
  PC->Train(25, 100, 100);
  // Get the predicted circles
  TVec<TIntSet> Circles = PC->GetCircles();
  for (int k = 0; k < K; k ++) {
    printf("Circle %d:", k + 1);
    for (THashSetKeyI<TInt> it = Circles[k].BegI(); it != Circles[k].EndI();
         it ++) {
      int c = it.GetKey();
      printf(" %d", c);
    }
    printf("\n");
  }
  
  // printf("Loss = %f\n", (double) TotalLoss(Circles, PGA->GroundTruth,
  //                                 PGA->NodeIDs.Len(), balancedError));

  return 0;
}
