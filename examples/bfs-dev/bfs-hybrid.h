#include <vector>
#include <stdio.h>

/* Remove */
#define NUM_ITERS 100

/* Remove this */
struct ChildTypeCount {
  int claimedChildren;
  int failedChildren;
  int peers;
  int validParents;
};

//#//////////////////////////////////////////////
/// Breath-First-Search class.
/// The class is meant for executing many BFSs over a fixed graph. This means that the class can keep the hash tables and queues initialized between different calls of the DoBfs() function.
template<class PGraph>
class TBreathFS_Hybrid {
public:
  PGraph Graph;
  TInt StartNId;
  TIntV NIdDistV;
/* Remove this *///  std::vector<ChildTypeCount> childCounts;
/* Remove */  std::vector<double> timePerStep;
public:
  TBreathFS_Hybrid(const PGraph& GraphPt, const bool& InitBigV=true) :
    Graph(GraphPt), NIdDistV(Graph->GetMxNId()+1), InitBigV(InitBigV) {
    for (int i = 0; i < NIdDistV.Len(); i++) {
      NIdDistV.SetVal(i, -1);
    }
  }
  /// Performs BFS from node id StartNode for at maps MxDist steps by only following in-links (parameter FollowIn = true) and/or out-links (parameter FollowOut = true).
  int DoBfs_Hybrid(const int& StartNode, const bool& FollowOut, const bool& FollowIn, const int& TargetNId=-1, const int& MxDist=TInt::Mx);
private:
  bool InitBigV;
private:
  bool TopDownStep(TIntV *Frontier, TIntV *NextFrontier, int& MaxDist, const int& TargetNId);
  bool BottomUpStep(TIntV *Frontier, TIntV *NextFrontier, int& MaxDist, const int& TargetNId);
};

template<class PGraph>
int TBreathFS_Hybrid<PGraph>::DoBfs_Hybrid(const int& StartNode, const bool& FollowOut, const bool& FollowIn, const int& TargetNId, const int& MxDist) {
  StartNId = StartNode;
  IAssert(Graph->IsNode(StartNId));
  const typename PGraph::TObj::TNodeI StartNodeI = Graph->GetNI(StartNode);
  IAssertR(StartNodeI.GetOutDeg() > 0, TStr::Fmt("No neighbors from start node %d.", StartNode));

  TIntV *Frontier = new TIntV(InitBigV ? Graph->GetNodes() : 1024, 0);
  TIntV *NextFrontier = new TIntV(InitBigV ? Graph->GetNodes() : 1024, 0);

  NIdDistV.SetVal(StartNId, 0);
  Frontier->Add(StartNId);
  int MaxDist = -1;
  while (! Frontier->Empty()) {
    MaxDist += 1;
    /* Remove */
    struct timeval tv1, tv2;
    gettimeofday(&tv1, NULL);

    NextFrontier->Clr(false);
    if (MaxDist == MxDist) { break; } // max distance limit reached
    if (BottomUpStep(Frontier, NextFrontier, MaxDist, TargetNId)) { break; }

    /* Remove */
    gettimeofday(&tv2, NULL);
    double timeDiff = (tv2.tv_sec - tv1.tv_sec) + (double)(tv2.tv_usec - tv1.tv_usec) / 1000000;
    timePerStep.push_back(timeDiff);

    // swap Frontier and NextFrontier
    TIntV *temp = Frontier;
    Frontier = NextFrontier;
    NextFrontier = temp;

/*
    const int NId = Queue.Top();  Queue.Pop();
    const int Dist = NIdDistV[NId];
    if (Dist == MxDist) { break; } // max distance limit reached

    // for analyzing edges
    if (Dist >= (int)childCounts.size()) {
      struct ChildTypeCount c = {0, 0, 0, 0};
      childCounts.push_back(c);
      if (Dist != (int)childCounts.size() - 1) {
        printf("Error.....\n");
        exit(1); // sanity check
      }
    }

    const typename PGraph::TObj::TNodeI NodeI = Graph->GetNI(NId);
    if (FollowOut) { // out-links
      for (v = 0; v < NodeI.GetOutDeg(); v++) {  // out-links
        const int DstNId = NodeI.GetOutNId(v);
        if (NIdDistV[DstNId] == -1) {
          NIdDistV.SetVal(DstNId, Dist+1);
          MaxDist = TMath::Mx(MaxDist, Dist+1);
          if (DstNId == TargetNId) { return MaxDist; }
          Queue.Push(DstNId);
          // Claimed child
          childCounts[Dist].claimedChildren += 1;
        } else {
          // Node visited before
          const int nextDist = NIdDistV[DstNId];
          if (nextDist < Dist) {
            childCounts[Dist].validParents += 1;
          } else if (nextDist == Dist) {
            childCounts[Dist].peers += 1;
          } else {
            childCounts[Dist].failedChildren += 1;
          }
        }
      }
    }

    if (FollowIn) { // in-links
      for (v = 0; v < NodeI.GetInDeg(); v++) {
        const int DstNId = NodeI.GetInNId(v);
        if (NIdDistV[DstNId]) {
          NIdDistV.SetVal(DstNId, Dist+1);
          MaxDist = TMath::Mx(MaxDist, Dist+1);
          if (DstNId == TargetNId) { return MaxDist; }
          Queue.Push(DstNId);
        }
      }
    }
    */
  }
  
  delete Frontier;
  delete NextFrontier;

  return MaxDist;
}

template<class PGraph>
bool TBreathFS_Hybrid<PGraph>::TopDownStep(TIntV *Frontier, TIntV *NextFrontier, int& MaxDist, const int& TargetNId) {
  printf("Top Down\n");
  for (TIntV::TIter it = Frontier->BegI(); it != Frontier->EndI(); ++it) { // loop over frontier
    const int NId = *it;
    const int Dist = NIdDistV[NId];
    IAssert(Dist == MaxDist); // Must equal to MaxDist
    const typename PGraph::TObj::TNodeI NodeI = Graph->GetNI(NId);
    for (int v = 0; v < NodeI.GetOutDeg(); v++) {
      const int NeighborNId = NodeI.GetOutNId(v);
      if (NIdDistV[NeighborNId] == -1) {
        NIdDistV.SetVal(NeighborNId, Dist+1);
        if (NeighborNId == TargetNId) return true;
        NextFrontier->Add(NeighborNId);
      }
    }
  }
  return false;
}
}
template<class PGraph>
bool TBreathFS_Hybrid<PGraph>::BottomUpStep(TIntV *Frontier, TIntV *NextFrontier, int& MaxDist, const int& TargetNId) {
  printf("Bottom up\n");
  for (TNGraph::TNodeI NodeI = Graph->BegNI(); NodeI < Graph->EndNI(); NodeI++) {
    const int NId = NodeI.GetId();
    if (NIdDistV[NId] == -1) {
      for (int v = 0; v < NodeI.GetInDeg(); v++) {
        const int ParentNId = NodeI.GetInNId(v);
        if (NIdDistV[ParentNId] == MaxDist) {
          NIdDistV[NId] = MaxDist + 1;
          if (NId == TargetNId) return true;
          NextFrontier->Add(NId);
          break;
        }
      }
    }
  }
  return false;
}
