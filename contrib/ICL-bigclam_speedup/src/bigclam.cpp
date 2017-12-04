// bigclam.cpp : Example program for BigClam - Cluster Affiliation Model
//               for Big Networks
//
#include "stdafx.h"
#include "agmfast.h"
#include "agm.h"
#ifndef NOMP
#include <omp.h>
#endif
#include <inttypes.h>

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(
    TStr::Fmt("Bigclam - cluster affiliation model for big networks.\n"
              "Executable built at: %s, %s.\n"
              "Start time: %s",
              __TIME__, __DATE__, TExeTm::GetCurTm())
  );
  TExeTm ExeTm;

  TStr OutFPrx = 
    Env.GetIfArgPrefixStr("-o:", "", "Output community affiliation data prefix");
  const TStr InFNm =
    Env.GetIfArgPrefixStr("-i:", "./data/com-amazon.ungraph.txt", 
                          "Input edgelist file name");
  int OptComs = 
    Env.GetIfArgPrefixInt("-c:", 100, "Number of communities to detect");
  const int NumThreads = 
    Env.GetIfArgPrefixInt("-nt:", 4, "Number of threads for parallelization");

#ifndef NOMP
  omp_set_num_threads(NumThreads);
#endif
  PUNGraph G;
  TIntStrH NIDNameH;
  
  G = TAGMUtil::LoadEdgeListStr<PUNGraph>(InFNm, NIDNameH);

  printf("\nSolving graph with %d nodes & %d edges...\n", 
         G->GetNodes(), G->GetEdges());
  
  TVec<TIntV> EstCmtyVV;
  TExeTm RunTm;
  TAGMFast RAGM(G, 10, 10);
  
  // For each of the three stages, we record the start time, run the
  // method w.r.t. the stage, and record the end time to obtain the time
  // spent in each stage
  
  // Community initialisation - conductance test stage
  uint64 STime = TSecTm::GetCurTm().GetAbsSecs();
  RAGM.NeighborComInit(OptComs);
  uint64 ConductanceTestRunTime = TSecTm::GetCurTm().GetAbsSecs() - STime;
  
  // Gradient ascent stage
  STime = TSecTm::GetCurTm().GetAbsSecs();
  RAGM.MLEGradAscentParallel(0.0001, 1000, NumThreads, "", 0.05, 0.3);
  uint64 GradientAscentRunTime = TSecTm::GetCurTm().GetAbsSecs() - STime;
 
  // Community association stage
  printf("\nExtracting community affiliations from matrix...");
  STime = TSecTm::GetCurTm().GetAbsSecs();
  RAGM.GetCmtyVV(EstCmtyVV);
  uint64 CommunityAssociationRunTime = TSecTm::GetCurTm().GetAbsSecs() - STime;
  printf("completed.\n");
  
  // Dump list of community memberships
  TAGMUtil::DumpCmtyVV(OutFPrx + "cmtyvv.txt", EstCmtyVV, NIDNameH);
  
  printf("\nRuntime breakdown (in full seconds elapsed):\n"
         "- Conductance test stage:\t %" PRIu64 " second(s)\n"
         "- Gradient ascent stage:\t %" PRIu64 " second(s)\n"
         "- Community association stage:\t %" PRIu64 " second(s)\n",
         ConductanceTestRunTime, GradientAscentRunTime, 
         CommunityAssociationRunTime
        );
  
  printf("End time: %s\n", TExeTm::GetCurTm());

  return 0;
}
