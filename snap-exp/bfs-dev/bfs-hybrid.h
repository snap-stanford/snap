//#//////////////////////////////////////////////
/// Breath-First-Search class.
/// The class is meant for executing many BFSs over a fixed graph. This means that the class can keep the hash tables and queues initialized between different calls of the DoBfs() function.
template<class PGraph>
class TBreathFS_Hybrid {
public:
  PGraph Graph;
  TInt StartNId;
  TIntH NIdDistH;
public:
  TBreathFS_Hybrid(const PGraph& GraphPt, const bool& InitBigV=true) :
    Graph(GraphPt), NIdDistH(Graph->GetNodes()), InitBigV(InitBigV) {}
  /// Performs BFS from node id StartNode for at maps MxDist steps by only following in-links (parameter FollowIn = true) and/or out-links (parameter FollowOut = true).
  int DoBfs_Hybrid(const int& StartNode, const bool& FollowOut, const bool& FollowIn, const int& TargetNId=-1, const int& MxDist=TInt::Mx);
private:
  bool InitBigV;
  int Stage; // 0, 2: top down, 1: bottom up
  static const unsigned int alpha = 100;
  static const unsigned int beta = 20;

  /* Private functions */
  bool TopDownStep(TIntV &NIdDistV, TIntV *Frontier, TIntV *NextFrontier, int& MaxDist, const int& TargetNId, const bool& FollowOut, const bool& FollowIn);
  bool BottomUpStep(TIntV &NIdDistV, TIntV *Frontier, TIntV *NextFrontier, int& MaxDist, const int& TargetNId, const bool& FollowOut, const bool& FollowIn);
};

template<class PGraph>
int TBreathFS_Hybrid<PGraph>::DoBfs_Hybrid(const int& StartNode, const bool& FollowOut, const bool& FollowIn, const int& TargetNId, const int& MxDist) {
  StartNId = StartNode;
  IAssert(Graph->IsNode(StartNId));
  if (TargetNId == StartNode) return 0;
  const typename PGraph::TObj::TNodeI StartNodeI = Graph->GetNI(StartNode);

  // Initialize vector
  TIntV NIdDistV(Graph->GetMxNId() + 1);
  for (int i = 0; i < NIdDistV.Len(); i++) {
    NIdDistV.SetVal(i, -1);
  }
  TIntV *Frontier = new TIntV(InitBigV ? Graph->GetNodes() : 1024, 0);
  TIntV *NextFrontier = new TIntV(InitBigV ? Graph->GetNodes() : 1024, 0);

  NIdDistV.SetVal(StartNId, 0);
  Frontier->Add(StartNId);
  Stage = 0;
  int MaxDist = -1;
  const unsigned int TotalNodes = Graph->GetNodes();
  unsigned int UnvisitedNodes = Graph->GetNodes();
  while (! Frontier->Empty()) {
    MaxDist += 1;
    NextFrontier->Clr(false);
    if (MaxDist == MxDist) { break; } // max distance limit reached

    UnvisitedNodes -= Frontier->Len();
    if (Stage == 0 && UnvisitedNodes / Frontier->Len() < alpha) {
      Stage = 1;
    } else if (Stage == 1 && TotalNodes / Frontier->Len() > beta) {
      Stage = 2;
    }

    // Top down or bottom up depending on stage
    bool targetFound = false;
    if (Stage == 0 || Stage == 2) {
      targetFound = TopDownStep(NIdDistV, Frontier, NextFrontier, MaxDist, TargetNId, FollowOut, FollowIn);
    } else {
      targetFound = BottomUpStep(NIdDistV, Frontier, NextFrontier, MaxDist, TargetNId, FollowOut, FollowIn);
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
  // Transform vector to hash table
  for (int NId = 0; NId < NIdDistV.Len(); NId++) {
    if (NIdDistV[NId] != -1) {
      NIdDistH.AddDat(NId, NIdDistV[NId]);
    }
  }
  return MaxDist;
}

template<class PGraph>
bool TBreathFS_Hybrid<PGraph>::TopDownStep(TIntV &NIdDistV, TIntV *Frontier, TIntV *NextFrontier, int& MaxDist, const int& TargetNId, const bool& FollowOut, const bool& FollowIn) {
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
        }
      }
    }
  }
  return false;
}

template<class PGraph>
bool TBreathFS_Hybrid<PGraph>::BottomUpStep(TIntV &NIdDistV, TIntV *Frontier, TIntV *NextFrontier, int& MaxDist, const int& TargetNId, const bool& FollowOut, const bool& FollowIn) {
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
            break;
          }
        }
      }
    }
  }
  return false;
}

