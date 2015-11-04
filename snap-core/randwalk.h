//
//  personalizedpagerank.h
//  snap-adv
//
//  Created by Peter Lofgren on 8/5/15.
//  Copyright (c) 2015 infolab. All rights reserved.
//
#ifndef snap_core_personalizedpagerank_h
#define snap_core_personalizedpagerank_h

#include <stdio.h>
#include "Snap.h"
#include "priorityqueue.h"

namespace TSnap {
  
// Returns the endpoint of a random walk sampled from a random start node in StartNIdV.  The walk has expected length 1/JumpProb, and restarts if it reaches a dead-end node (one with no out-neighbors).
template <class PGraph>
int SamplePersonalizedPageRank(const PGraph& Graph, const double& JumpProb, const TIntV& StartNIdV, TRnd& Rnd);

// Returns the Personalized PageRank from given vector of start node ids to the given target.  This is equal to the probability
// that a random walk from a source uniformly sampled from the given start node ids stops at the given target node id, where
// the walk stops after each stop with probability JumpProb.  As long as the true PPR is greater than MinProbability (which defaults
//  to 1/n on an n node graph), the mean relative error will be approximately the given RelativeError.  If provableRelativeError is set to
// true, the result will provably (with high probability) have at most the given RelativeError, but this comes at the cost of significantly greater
//  running time.  Uses the algorithm presented in "Personalized PageRank Estimation and Search: A Bidirectional Approach" by Lofgren, Banerjee, and Goel.
template <class PGraph>
float GetPersonalizedPageRankBidirectional(const PGraph& Graph, const double& JumpProb, const TIntV& StartNIdV,
                                             const int& TargetNId, float MinProbability = -1.0f, const float& RelativeError = 0.1f,
                                             const bool provableRelativeError = false);
  
// Convenience method that takes a single startNId and converts it to a vector of length 1 before calling GetPersonalizedPageRankBidirectional.
template <class PGraph>
inline float GetRndWalkRestartBidirectional(const PGraph& Graph, const double& JumpProb, const int& StartNId,
 const int& TargetNId, float minProbability = -1.0f, const float& relativeError = 0.1f,
 const bool proveRelativeError = false) {
  return GetPersonalizedPageRankBidirectional(Graph, JumpProb, TIntV::GetV(StartNId), TargetNId,
                                              minProbability, relativeError, proveRelativeError);
}

}; // namespace TSnap

#endif
