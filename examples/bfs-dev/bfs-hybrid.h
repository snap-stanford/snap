#include <vector>
#include <stdio.h>

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
  TSnapQueue<int> Queue;
  TInt StartNId;
  TIntV NIdDistV;
  TIntV *Frontier;
  TIntV *NextFrontier;
  std::vector<ChildTypeCount> childCounts;
public:
  TBreathFS_Hybrid(const PGraph& GraphPt, const bool& InitBigQ=true) :
    Graph(GraphPt), Queue(InitBigQ?Graph->GetNodes():1024), NIdDistV(InitBigQ?Graph->GetMxNId()+1:1024) {
    Frontier = new TIntV(InitBigQ ? Graph->GetNodes() : 1024, 0);
    NextFrontier = new TIntV(InitBigQ ? Graph->GetNodes() : 1024, 0);
    for (int i = 0; i < NIdDistV.Len(); i++) {
      NIdDistV.SetVal(i, -1);
    }
  }
  /// Performs BFS from node id StartNode for at maps MxDist steps by only following in-links (parameter FollowIn = true) and/or out-links (parameter FollowOut = true).
  int DoBfs_Hybrid(const int& StartNode, const bool& FollowOut, const bool& FollowIn, const int& TargetNId=-1, const int& MxDist=TInt::Mx);
};

template<class PGraph>
int TBreathFS_Hybrid<PGraph>::DoBfs_Hybrid(const int& StartNode, const bool& FollowOut, const bool& FollowIn, const int& TargetNId, const int& MxDist) {
  StartNId = StartNode;
  IAssert(Graph->IsNode(StartNId));
//  const typename PGraph::TObj::TNodeI StartNodeI = Graph->GetNI(StartNode);
//  IAssertR(StartNodeI.GetOutDeg() > 0, TStr::Fmt("No neighbors from start node %d.", StartNode));
  NIdDistV.SetVal(StartNId, 0);
  Frontier->Add(StartNId);
  int v, MaxDist = 0;
  while (! Frontier->Empty()) {
/* Remove this */    IAssert(NextFrontier->Empty());
    if (MaxDist == MxDist) { break; } // max distance limit reached
    for (TIntV::TIter it = Frontier->BegI(); it != Frontier->EndI(); ++it) { // loop over frontier
      const int NId = *it;
      const int Dist = NIdDistV[NId];
      const typename PGraph::TObj::TNodeI NodeI = Graph->GetNI(NId);
      for (v = 0; v < NodeI.GetOutDeg(); v++) {
        const int NeighborNId = NodeI.GetOutNId(v);
        if (NIdDistV[NeighborNId] == -1) {
          NIdDistV.SetVal(NeighborNId, Dist+1);
          MaxDist = Dist + 1;
          if (NeighborNId == TargetNId) { return MaxDist; }
          NextFrontier->Add(NeighborNId);
        }
      }
    }
    // swap Frontier and NextFrontier
    TIntV *temp = Frontier;
    Frontier = NextFrontier;
    NextFrontier = temp;
    NextFrontier->Clr(false);

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
  return MaxDist;
}
