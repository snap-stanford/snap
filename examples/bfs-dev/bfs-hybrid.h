#include <vector>
#include <stdio.h>

/* Remove */
#define NUM_ITERS 100

/* Remove this */
struct ChildTypeCount {
  int claimed;
  int failed;
};

struct NodeCount {
  int frontier;
  int unvisited;
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
/* Remove this */  std::vector<ChildTypeCount> childCounts;
/* Remove this */  std::vector<NodeCount> nodeCounts;
/* Remove */ // std::vector<double> timePerStep;
public:
  TBreathFS_Hybrid(const PGraph& GraphPt, const bool& InitBigV=true) :
    Graph(GraphPt), NIdDistV(Graph->GetMxNId()+1), InitBigV(InitBigV) {
    for (int i = 0; i < NIdDistV.Len(); i++) {
      NIdDistV.SetVal(i, -1);
    }
  }
  /// Performs BFS from node id StartNode for at maps MxDist steps by only following in-links (parameter FollowIn = true) and/or out-links (parameter FollowOut = true).
  int DoBfs_Hybrid(const int& StartNode, const bool& FollowOut, const bool& FollowIn, const int switch1=-1, const int switch2=0, const int& TargetNId=-1, const int& MxDist=TInt::Mx);
private:
  bool InitBigV;
  /* Private functions */
  bool TopDownStep(TIntV *Frontier, TIntV *NextFrontier, int& MaxDist, const int& TargetNId);
  bool BottomUpStep(TIntV *Frontier, TIntV *NextFrontier, int& MaxDist, const int& TargetNId);
};

template<class PGraph>
int TBreathFS_Hybrid<PGraph>::DoBfs_Hybrid(const int& StartNode, const bool& FollowOut, const bool& FollowIn, const int switch1, const int switch2, const int& TargetNId, const int& MxDist) {
  StartNId = StartNode;
  IAssert(Graph->IsNode(StartNId));
  const typename PGraph::TObj::TNodeI StartNodeI = Graph->GetNI(StartNode);
  IAssertR(StartNodeI.GetOutDeg() > 0, TStr::Fmt("No neighbors from start node %d.", StartNode));
  // Remove
  IAssertR(switch1 < switch2, TStr::Fmt("Invalid switches: %d, %d.", switch1, switch2));

  TIntV *Frontier = new TIntV(InitBigV ? Graph->GetNodes() : 1024, 0);
  TIntV *NextFrontier = new TIntV(InitBigV ? Graph->GetNodes() : 1024, 0);

  NIdDistV.SetVal(StartNId, 0);
  Frontier->Add(StartNId);
  int MaxDist = -1;
  int totalNodes = Graph->GetNodes();
  int visited = 0;
  while (! Frontier->Empty()) {
    visited += Frontier->Len();
    /* Remove */
    struct NodeCount nc = {Frontier->Len(), totalNodes - visited};
    nodeCounts.push_back(nc);
    /* Remove */
    struct ChildTypeCount c = {0, 0};
    childCounts.push_back(c);
    MaxDist += 1;
    /* Remove */
//    struct timeval tv1, tv2;
//    gettimeofday(&tv1, NULL);

    NextFrontier->Clr(false);
    if (MaxDist == MxDist) { break; } // max distance limit reached

    bool targetFound = false;
    if (MaxDist < switch1 || MaxDist >= switch2) {
      targetFound = TopDownStep(Frontier, NextFrontier, MaxDist, TargetNId);
    } else {
      targetFound = BottomUpStep(Frontier, NextFrontier, MaxDist, TargetNId);
    }
    if (targetFound) break;

//    if (TopDownStep(Frontier, NextFrontier, MaxDist, TargetNId)) { break; }

    /* Remove */
//    gettimeofday(&tv2, NULL);
//    double timeDiff = (tv2.tv_sec - tv1.tv_sec) + (double)(tv2.tv_usec - tv1.tv_usec) / 1000000;
//    timePerStep.push_back(timeDiff);

    // swap Frontier and NextFrontier
    TIntV *temp = Frontier;
    Frontier = NextFrontier;
    NextFrontier = temp;
  }
  
  delete Frontier;
  delete NextFrontier;

  return MaxDist;
}

template<class PGraph>
bool TBreathFS_Hybrid<PGraph>::TopDownStep(TIntV *Frontier, TIntV *NextFrontier, int& MaxDist, const int& TargetNId) {
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
        /* Remove */
        childCounts[MaxDist].claimed += 1;
      } else {
        childCounts[MaxDist].failed += 1;
      }
    }
  }
  return false;
}

template<class PGraph>
bool TBreathFS_Hybrid<PGraph>::BottomUpStep(TIntV *Frontier, TIntV *NextFrontier, int& MaxDist, const int& TargetNId) {
  for (TNGraph::TNodeI NodeI = Graph->BegNI(); NodeI < Graph->EndNI(); NodeI++) {
    const int NId = NodeI.GetId();
    if (NIdDistV[NId] == -1) {
      for (int v = 0; v < NodeI.GetInDeg(); v++) {
        const int ParentNId = NodeI.GetInNId(v);
        if (NIdDistV[ParentNId] == MaxDist) {
          NIdDistV[NId] = MaxDist + 1;
          if (NId == TargetNId) return true;
          NextFrontier->Add(NId);
          /* Remove */
          childCounts[MaxDist].claimed += 1;
          break;
        } else {
          childCounts[MaxDist].failed += 1;
        }
      }
    }
  }
  return false;
}
