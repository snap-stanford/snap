//
//  randwalk.h
//  snap-core
//
//  Created by Peter Lofgren on 8/5/15.
//  Copyright (c) 2015 infolab. All rights reserved.
//
#ifndef snap_core_randwalk_h
#define snap_core_randwalk_h

#include <stdio.h>
#include "priorityqueue.h"
#include "Snap.h"

// use anonymous namespace to limit name to this file.
namespace {
  double WallClockTime() {
    // from http://stackoverflow.com/questions/588307/c-obtaining-milliseconds-time-on-linux-clock-doesnt-seem-to-work-properl/
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec + t.tv_usec / 1.0e6;
  }
  
  // Given a target nodeId, computes results in two maps Estimates and Residuals and one value MaxResidual.
  // Each value Estimates.GetDat(v)
  // approximates ppr(v, target) with additive error MaxResidual, and the residuals satisfy a
  // useful loop invariant.  See the paper "Local Computation of PageRank Contributions" for details.
  // This variant is for the balanced variant of bidirectional ppr.  It does reverse pushes, decreasing maxResidual incrementally,
  // until the time spent equals the remaining time required for forward walks, as estimated using ForwardSecondsRMaxRatio
  // (which is the ratio between the expected forward random walk time and the current MaxResidual that hasn't been pushed back).
  template <class PGraph>
  void ApproxContributionsBalanced(const PGraph& Graph,
                                   double JumpProb,
                                   int TargetNId,
                                   float ForwardSecondsRMaxRatio,
                                   TIntFltH& ResultEstimates,
                                   TIntFltH& ResultResiduals,
                                   float& ResultMaxResidual) {
    double startTime = WallClockTime();
    TMaxPriorityQueue<TInt> nodesByResidual;
    nodesByResidual.Insert(TargetNId, 1.0f);
    while (!nodesByResidual.IsEmpty() &&
           WallClockTime() - startTime < ForwardSecondsRMaxRatio * nodesByResidual.GetMaxPriority()) {
      float vPriority = nodesByResidual.GetMaxPriority();
      int vId = nodesByResidual.PopMax();
      ResultEstimates(vId) = ResultEstimates(vId) + JumpProb * vPriority;
      //printf("set estimate(%d) to %g\n", vId, double(ResultEstimates(vId)));
      TNGraph::TNodeI v = Graph->GetNI(vId);
      for (int i = 0; i < v.GetInDeg(); i++) {
        int uId = v.GetInNId(i);
        TNGraph::TNodeI u = Graph->GetNI(uId);
        float residualChange = (1.0 - JumpProb) * vPriority / u.GetOutDeg();
        nodesByResidual.SetPriority(uId, nodesByResidual.GetPriority(uId) + residualChange);
        //printf("set priority(%d) to %g\n", uId, double(nodesByResidual.GetPriority(uId)));
      }
    }
    
    // Copy residuals from nodesByResidual to ResultResiduals
    nodesByResidual.GetPriorities(ResultResiduals);
    ResultMaxResidual = nodesByResidual.IsEmpty() ? 0.0f : nodesByResidual.GetMaxPriority();
  }
}

namespace TSnap {
// Returns the endpoint of a random walk sampled from a random start node in StartNIdV.  The walk has expected length 1/JumpProb, and restarts if it reaches a dead-end node (one with no out-neighbors).
template <class PGraph>
int SamplePersonalizedPageRank(const PGraph& Graph, double JumpProb, const TIntV& StartNIdV, TRnd& Rnd) {
  int locationId = StartNIdV.GetRndVal(Rnd);
  //printf("starting walk at %d\n", locationId);
  while (Rnd.GetUniDev() >= JumpProb) {
    TNGraph::TNodeI location = Graph->GetNI(locationId);
    int d = location.GetOutDeg();
    if (d > 0)
      locationId = location.GetOutNId(Rnd.GetUniDevInt(d));
    else
      locationId = StartNIdV.GetRndVal(Rnd);
  }
  return locationId;
}
  
// Returns the Personalized PageRank from given vector of start node ids to the given target.  This is equal to the probability
// that a random walk from a source uniformly sampled from the given start node ids stops at the given target node id, where
// the walk stops after each stop with probability JumpProb.  As long as the true PPR is greater than MinProbability (which defaults
//  to 1/n on an n node graph), the mean relative error will be approximately the given RelativeError.  If provableRelativeError is set to
// true, the result will provably (with high probability) have at most the given RelativeError, but this comes at the cost of significantly greater
//  running time.  Uses the algorithm presented in "Personalized PageRank Estimation and Search: A Bidirectional Approach" by Lofgren, Banerjee, and Goel.
template <class PGraph>
  double GetPersonalizedPageRankBidirectional(const PGraph& Graph,
                                              double JumpProb,
                                              const TIntV& StartNIdV,
                                              int TargetNId,
                                              double MinProbability = -1.0,
                                              double RelativeError = 0.1,
                                              bool provableRelativeError = false,
                                              bool PrintTimeForTuning = false) {
  if (MinProbability <= 0.0) { // Check if minProbability not set.
    MinProbability = 1.0 / Graph->GetNodes();
  }
  // In experiments, when relativeError = 0.1, a chernoff constant of 0.07 gave mean relative error less than 0.1 on several realistic graphs.
  float kChernoffConstant = provableRelativeError ? 12 * exp((double) 1) * log(2 / 1.0e-9) : 0.07;
  float kSecondsPerWalk = 4.0e-7; // The time required to generate a random walk. Can be tuned so that forward and reverse running times are equal, to improve running time
  float WalkCountRMaxRatio = kChernoffConstant / (RelativeError * RelativeError) / MinProbability;
  float ForwardSecondsRMaxRatio = kSecondsPerWalk * WalkCountRMaxRatio;
  
  
  double startTime = WallClockTime();
  // Results from ApproxContributionsBalanced are set by reference:
  TIntFltH Estimates, Residuals;
  float MaxResidual;
  ApproxContributionsBalanced(Graph, JumpProb, TargetNId, ForwardSecondsRMaxRatio, Estimates, Residuals, MaxResidual);
  
  double reverseTime = WallClockTime() - startTime;
  startTime = WallClockTime();
  
  double Estimate = 0.0;
  // First incorporate the average Estimates value for starting nodes
  for (int i = 0; i < StartNIdV.Len(); i++) {
    Estimate += Estimates.GetDatWithDefault(StartNIdV[i], 0.0) / StartNIdV.Len();
  }
  
  int RandomWalkCount = static_cast<int>(WalkCountRMaxRatio * MaxResidual);
  TRnd Rnd(0); // 0 means seed from clock. We use an explicit Rnd for thread safety.
  for (int i = 0; i < RandomWalkCount; i++) {
    int vId = SamplePersonalizedPageRank(Graph, JumpProb, StartNIdV, Rnd);
    Estimate += Residuals.GetDatWithDefault(vId, 0.0) / RandomWalkCount;
  }
  double forwardTime = WallClockTime() - startTime;
  if (PrintTimeForTuning) printf("forwardTime reverseTime %g %g\n", forwardTime, reverseTime);
  
  return Estimate;
}
  
// Convenience method that takes a single startNId and converts it to a vector of length 1 before calling GetPersonalizedPageRankBidirectional.
template <class PGraph>
  double GetRndWalkRestartBidirectional(const PGraph& Graph,
                                        double JumpProb,
                                        int StartNId,
                                        int TargetNId,
                                        double minProbability = -1.0,
                                        double relativeError = 0.1,
                                        bool proveRelativeError = false,
                                        bool PrintTimeForTuning = false) {
    return GetPersonalizedPageRankBidirectional(Graph, JumpProb, TIntV::GetV(StartNId), TargetNId,
                                                minProbability, relativeError, proveRelativeError, PrintTimeForTuning);
  }

}; // namespace TSnap

#endif
