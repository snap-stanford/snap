//
//  personalizedpagerank.cpp
//  snap-core
//
//  Created by Peter Lofgren on 10/30/15.
//  Copyright © 2015 infolab. All rights reserved.
//

#include "randwalk.h"

// use anonymous namespace to limit name to this file.
namespace {
  double wallClockTime() {
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
  void ApproxContributionsBalanced(const PNGraph& Graph, const double& JumpProb, const int& TargetNId,
                                   const float& ForwardSecondsRMaxRatio, TIntFltH& ResultEstimates, TIntFltH& ResultResiduals, float& ResultMaxResidual) {
    double startTime = wallClockTime();
    MaxPriorityQueue nodesByResidual;
    nodesByResidual.Insert(TargetNId, 1.0f);
    while (!nodesByResidual.IsEmpty() &&
           wallClockTime() - startTime < ForwardSecondsRMaxRatio * nodesByResidual.GetMaxPriority()) {
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

  int SamplePersonalizedPageRank(const PNGraph& Graph, const double& JumpProb, const TIntV& StartNIdV, TRnd& Rnd) {
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

  
  float GetPersonalizedPageRankBidirectional(const PNGraph& Graph, const double& JumpProb, const TIntV& StartNIdV,
                                             const int& TargetNId, float MinProbability, const float& RelativeError,
                                             const bool provableRelativeError) {
    const bool PrintTimeForTuning = false;
    if (MinProbability <= 0.0f) { // Check if minProbability not set.
      MinProbability = 1.0f / Graph->GetNodes();
    }
    // In experiments, when relativeError = 0.1, a chernoff constant of 0.07 gave mean relative error less than 0.1 on several realistic graphs.
    float kChernoffConstant = provableRelativeError ? 12 * exp(1) * log(2 / 1.0e-9) : 0.07;
    float kSecondsPerWalk = 2.0e-7; // The time required to generate a random walk. Can be tuned so that forward and reverse running times are equal, to improve running time
    float WalkCountRMaxRatio = kChernoffConstant / (RelativeError * RelativeError) / MinProbability;
    float ForwardSecondsRMaxRatio = kSecondsPerWalk * WalkCountRMaxRatio;
    
    
    double startTime = wallClockTime();
    // Results from ApproxContributionsBalanced are set by reference:
    TIntFltH Estimates, Residuals;
    float MaxResidual;
    ApproxContributionsBalanced(Graph, JumpProb, TargetNId, ForwardSecondsRMaxRatio, Estimates, Residuals, MaxResidual);
    
    double forwardTime = wallClockTime() - startTime;
    startTime = wallClockTime();
    
    float Estimate = 0.0f;
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
    double reverseTime = wallClockTime() - startTime;
    if (PrintTimeForTuning) printf("forwardTime reverseTime %g %g\n", forwardTime, reverseTime);
    
    return Estimate;
  }

}
