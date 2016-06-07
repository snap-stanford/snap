// bigclam.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "agmfast.h"
#include "agm.h"
#ifndef NOMP
#include <omp.h>
#endif

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("bigclam. build: %s, %s. Time: %s",
                          __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm;

  TStr OutFPrx = 
    Env.GetIfArgPrefixStr("-o:", "", "Output Graph data prefix");
  const TStr InFNm =
    Env.GetIfArgPrefixStr("-i:", "./data/com-amazon.ungraph.txt", 
                          "Input edgelist file name");
  int OptComs = 
    Env.GetIfArgPrefixInt("-c:", 100, "The number of communities to detect "
                                      "(-1: detect automatically)");
  const int MinComs = 
    Env.GetIfArgPrefixInt("-mc:", 5, "Minimum number of communities to try");
  const int MaxComs = 
    Env.GetIfArgPrefixInt("-xc:", 100, "Maximum number of communities to try");
  const int DivComs = 
    Env.GetIfArgPrefixInt("-nc:", 10, "How many trials for the number "
                                      "of communities");
  const int NumThreads = 
    Env.GetIfArgPrefixInt("-nt:", 4, "Number of threads for parallelization");
  const double StepAlpha = 
    Env.GetIfArgPrefixFlt("-sa:", 0.05, "Alpha for backtracking line search");
  const double StepBeta = 
    Env.GetIfArgPrefixFlt("-sb:", 0.3, "Beta for backtracking line search");

#ifndef NOMP
  omp_set_num_threads(NumThreads);
#endif
  PUNGraph G;
  TIntStrH NIDNameH;
  
  G = TAGMUtil::LoadEdgeListStr<PUNGraph>(InFNm, NIDNameH);

  printf("Graph: %d Nodes %d Edges\n", G->GetNodes(), G->GetEdges());
  
  TVec<TIntV> EstCmtyVV;
  TExeTm RunTm;
  TAGMFast RAGM(G, 10, 10);
  
  if (OptComs == -1) {
    printf("finding number of communities\n");
    OptComs = RAGM.FindComsByCV(NumThreads, MaxComs, MinComs, DivComs, 
                                OutFPrx, StepAlpha, StepBeta);
  }
  
  // For each of the three stages, we record the start time, run the
  // method w.r.t. the stage, and record the end time to obtain the time
  // spent in each stage
  
  // Community initialisation - conductance test stage
  uint64 stime = TSecTm::GetCurTm().GetAbsSecs();
  RAGM.NeighborComInit(OptComs);
  double conductanceTestRunTime =
    (double) (TSecTm::GetCurTm().GetAbsSecs() - stime);
  
  // Gradient ascent stage
  stime = TSecTm::GetCurTm().GetAbsSecs();
  RAGM.MLEGradAscentParallel(0.0001, 1000, 
                             NumThreads, "", 
                             StepAlpha, StepBeta);
  double gradientAscentRunTime =
    (double) (TSecTm::GetCurTm().GetAbsSecs() - stime);
 
  // Community association stage
  stime = TSecTm::GetCurTm().GetAbsSecs();
  RAGM.GetCmtyVV(EstCmtyVV);
  double communityAssociationRunTime =
    (double) (TSecTm::GetCurTm().GetAbsSecs() - stime);
  
  // Dump list of community memmberships
  TAGMUtil::DumpCmtyVV(OutFPrx + "cmtyvv.txt", EstCmtyVV, NIDNameH);
  
  printf("\nRuntime breakdown (in seconds):\n"
         "Conductance test stage: %f seconds\n"
         "Gradient ascent stage: %f seconds\n"
         "Community association stage: %f seconds\n",
         conductanceTestRunTime, gradientAscentRunTime, 
         communityAssociationRunTime
        );
  
  printf("\nrun time: %s (%s)\n", 
         ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());

  return 0;
}
