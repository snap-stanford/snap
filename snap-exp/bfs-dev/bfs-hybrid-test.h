//#//////////////////////////////////////////////
/// Breath-First-Search class.
/// The class is meant for executing many BFSs over a fixed graph. This means that the class can keep the hash tables and queues initialized between different calls of the DoBfs() function.
template<class PGraph>
class TBreathFS_Hybrid_Test {
public:
  PGraph Graph;
  TInt StartNId;
  TIntV NIdDistV;
public:
  TBreathFS_Hybrid_Test(const PGraph& GraphPt, const bool& InitBigV=true) :
    Graph(GraphPt), NIdDistV(Graph->GetMxNId()+1), InitBigV(InitBigV) {
    for (int i = 0; i < NIdDistV.Len(); i++) {
      NIdDistV.SetVal(i, -1);
    }
  }
  /// Performs BFS from node id StartNode for at maps MxDist steps by only following in-links (parameter FollowIn = true) and/or out-links (parameter FollowOut = true).
  int DoBfs_Hybrid_Test(const int& StartNode, const bool& FollowOut, const bool& FollowIn, const int& TargetNId=-1, const int& MxDist=TInt::Mx);
private:
  bool InitBigV;
  int Stage; // 0, 2: top down, 1: bottom up
  unsigned int FrontierOutDegrees;
  unsigned int UnvisitedInDegrees;
  static const unsigned int alpha = 10;
  static const unsigned int beta = 20;

  /* Private functions */
  bool TopDownStep(TIntV *Frontier, TIntV *NextFrontier, int& MaxDist, const int& TargetNId, const bool& FollowOut, const bool& FollowIn);
  bool BottomUpStep(TIntV *Frontier, TIntV *NextFrontier, int& MaxDist, const int& TargetNId, const bool& FollowOut, const bool& FollowIn);
};

template<class PGraph>
int TBreathFS_Hybrid_Test<PGraph>::DoBfs_Hybrid_Test(const int& StartNode, const bool& FollowOut, const bool& FollowIn, const int& TargetNId, const int& MxDist) {
  StartNId = StartNode;
  IAssert(Graph->IsNode(StartNId));
  if (TargetNId == StartNode) return 0;
  const typename PGraph::TObj::TNodeI StartNodeI = Graph->GetNI(StartNode);

  TIntV *Frontier = new TIntV(InitBigV ? Graph->GetNodes() : 1024, 0);
  TIntV *NextFrontier = new TIntV(InitBigV ? Graph->GetNodes() : 1024, 0);

  FrontierOutDegrees = 0;
  UnvisitedInDegrees = 0;
  if (FollowOut) {
    FrontierOutDegrees += StartNodeI.GetOutDeg();
    UnvisitedInDegrees += Graph->GetEdges() - StartNodeI.GetInDeg();
  }
  if (FollowIn) {
    FrontierOutDegrees += StartNodeI.GetInDeg();
    UnvisitedInDegrees += Graph->GetEdges() - StartNodeI.GetOutDeg();
  }

  NIdDistV.SetVal(StartNId, 0);
  Frontier->Add(StartNId);
  Stage = 0;
  int MaxDist = -1;
  const unsigned int TotalNodes = Graph->GetNodes();
  while (! Frontier->Empty()) {
    MaxDist += 1;
    NextFrontier->Clr(false);
    if (MaxDist == MxDist) { break; } // max distance limit reached

    if (Stage == 0 && UnvisitedInDegrees / FrontierOutDegrees < alpha) {
      Stage = 1;
    } else if (Stage == 1 && TotalNodes / Frontier->Len() > beta) {
      Stage = 2;
    }

    // Top down or bottom up depending on stage
    FrontierOutDegrees = 0;
    bool targetFound = false;
    if (Stage == 0 || Stage == 2) {
      targetFound = TopDownStep(Frontier, NextFrontier, MaxDist, TargetNId, FollowOut, FollowIn);
    } else {
      targetFound = BottomUpStep(Frontier, NextFrontier, MaxDist, TargetNId, FollowOut, FollowIn);
    }
    if (targetFound) {
      MaxDist = NIdDistV[TargetNId];
      break;
    }

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
bool TBreathFS_Hybrid_Test<PGraph>::TopDownStep(TIntV *Frontier, TIntV *NextFrontier, int& MaxDist, const int& TargetNId, const bool& FollowOut, const bool& FollowIn) {
  for (TIntV::TIter it = Frontier->BegI(); it != Frontier->EndI(); ++it) { // loop over frontier
    const int NId = *it;
    const int Dist = NIdDistV[NId];
    IAssert(Dist == MaxDist); // Must equal to MaxDist
    const typename PGraph::TObj::TNodeI NodeI = Graph->GetNI(NId);
    if (FollowOut) {
      for (int v = 0; v < NodeI.GetOutDeg(); v++) {
        const int NeighborNId = NodeI.GetOutNId(v);
        if (NIdDistV[NeighborNId] == -1) {
          NIdDistV.SetVal(NeighborNId, Dist+1);
          if (NeighborNId == TargetNId) return true;
          NextFrontier->Add(NeighborNId);
          // update FrontierOutDegrees, UnvisitedInDegrees
          if (Stage == 0) {
            typename PGraph::TObj::TNodeI NeighborNodeI = Graph->GetNI(NeighborNId);
            FrontierOutDegrees += NeighborNodeI.GetOutDeg();
            UnvisitedInDegrees -= NeighborNodeI.GetInDeg();
          }
        }
      }
    }
    if (FollowIn) {
      for (int v = 0; v < NodeI.GetInDeg(); v++) {
        const int NeighborNId = NodeI.GetInNId(v);
        if (NIdDistV[NeighborNId] == -1) {
          NIdDistV.SetVal(NeighborNId, Dist+1);
          if (NeighborNId == TargetNId) return true;
          NextFrontier->Add(NeighborNId);
          // update FrontierOutDegrees, UnvisitedInDegrees
          if (Stage == 0) {
            typename PGraph::TObj::TNodeI NeighborNodeI = Graph->GetNI(NeighborNId);
            FrontierOutDegrees += NeighborNodeI.GetInDeg();
            UnvisitedInDegrees -= NeighborNodeI.GetOutDeg();
          }
        }
      }
    }
  }
  return false;
}

template<class PGraph>
bool TBreathFS_Hybrid_Test<PGraph>::BottomUpStep(TIntV *Frontier, TIntV *NextFrontier, int& MaxDist, const int& TargetNId, const bool& FollowOut, const bool& FollowIn) {
  for (typename PGraph::TObj::TNodeI NodeI = Graph->BegNI(); NodeI < Graph->EndNI(); NodeI++) {
    const int NId = NodeI.GetId();
    if (NIdDistV[NId] == -1) {
      if (FollowOut) {
        for (int v = 0; v < NodeI.GetInDeg(); v++) {
          const int ParentNId = NodeI.GetInNId(v);
          if (NIdDistV[ParentNId] == MaxDist) {
            NIdDistV[NId] = MaxDist + 1;
            if (NId == TargetNId) return true;
            NextFrontier->Add(NId);
            // update FrontierOutDegrees, UnvisitedInDegrees
            if (Stage == 0) {
              FrontierOutDegrees += NodeI.GetOutDeg();
              UnvisitedInDegrees -= NodeI.GetInDeg();
            }
            break;
          }
        }
      }
      if (FollowIn && NIdDistV[NId] == -1) {
        for (int v = 0; v < NodeI.GetOutDeg(); v++) {
          const int ParentNId = NodeI.GetOutNId(v);
          if (NIdDistV[ParentNId] == MaxDist) {
            NIdDistV[NId] = MaxDist + 1;
            if (NId == TargetNId) return true;
            NextFrontier->Add(NId);
            // update FrontierOutDegrees, UnvisitedInDegrees
            if (Stage == 0) {
              FrontierOutDegrees += NodeI.GetInDeg();
              UnvisitedInDegrees -= NodeI.GetOutDeg();
            }
            break;
          }
        }
      }
    }
  }
  return false;
}

