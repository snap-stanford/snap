namespace TSnap {

/////////////////////////////////////////////////
// BFS and DFS
/// Returns a directed Breadth-First-Search tree rooted at StartNId. ##GetBfsTree1
template <class PGraph> PNGraph GetBfsTree(const PGraph& Graph, const int& StartNId, const bool& FollowOut, const bool& FollowIn);
/// Returns the BFS tree size (number of nodes) and depth (number of levels) by following in-links (parameter FollowIn = true) and/or out-links (parameter FollowOut = true) of node StartNId.
template <class PGraph> int GetSubTreeSz(const PGraph& Graph, const int& StartNId, const bool& FollowOut, const bool& FollowIn, int& TreeSzX, int& TreeDepthX);
/// Finds IDs of all nodes that are at distance Hop from node StartNId. ##GetSubTreeSz
template <class PGraph> int GetNodesAtHop(const PGraph& Graph, const int& StartNId, const int& Hop, TIntV& NIdV, const bool& IsDir=false);
/// Returns the number of nodes at each hop distance from the starting node StartNId. ##GetNodesAtHops
template <class PGraph> int GetNodesAtHops(const PGraph& Graph, const int& StartNId, TIntPrV& HopCntV, const bool& IsDir=false);

/////////////////////////////////////////////////
// Shortest paths
/// Returns the length of the shortest path from node SrcNId to node DstNId. ##GetShortPath1
template <class PGraph> int GetShortPath(const PGraph& Graph, const int& SrcNId, const int& DstNId, const bool& IsDir=false);
/// Returns the length of the shortest path from node SrcNId to all other nodes in the network. ##GetShortPath2
template <class PGraph> int GetShortPath(const PGraph& Graph, const int& SrcNId, TIntH& NIdToDistH, const bool& IsDir=false, const int& MaxDist=TInt::Mx);

/////////////////////////////////////////////////
// Diameter

/// Returns the (approximation of the) Diameter (maximum shortest path length) of a graph (by performing BFS from NTestNodes random starting nodes). ##GetBfsFullDiam
template <class PGraph> int GetBfsFullDiam(const PGraph& Graph, const int& NTestNodes, const bool& IsDir=false);
/// Returns the (approximation of the) Effective Diameter (90-th percentile of the distribution of shortest path lengths) of a graph (by performing BFS from NTestNodes random starting nodes). ##GetBfsEffDiam1
template <class PGraph> double GetBfsEffDiam(const PGraph& Graph, const int& NTestNodes, const bool& IsDir=false);
/// Returns the (approximation of the) Effective Diameter and the Diameter of a graph (by performing BFS from NTestNodes random starting nodes). ##GetBfsEffDiam2
template <class PGraph> double GetBfsEffDiam(const PGraph& Graph, const int& NTestNodes, const bool& IsDir, double& EffDiamX, int& FullDiamX);
/// Returns the (approximation of the) Effective Diameter, the Diameter and the Average Shortest Path length in a graph (by performing BFS from NTestNodes random starting nodes). ##GetBfsEffDiam3
template <class PGraph> double GetBfsEffDiam(const PGraph& Graph, const int& NTestNodes, const bool& IsDir, double& EffDiamX, int& FullDiamX, double& AvgSPLX);
/// Returns the (approximation of the) Effective Diameter, the Diameter and the Average Shortest Path length in a graph (by performing BFS from NTestNodes random starting nodes). ##GetBfsEffDiamAll
template <class PGraph> double GetBfsEffDiamAll(const PGraph& Graph, const int& NTestNodes, const bool& IsDir, double& EffDiamX, int& FullDiamX, double& AvgSPLX);
/// Use the whole graph (all edges) to measure the shortest path lengths but only report the path lengths between nodes in the SubGraphNIdV. ##GetBfsEffDiam4
template <class PGraph> double GetBfsEffDiam(const PGraph& Graph, const int& NTestNodes, const TIntV& SubGraphNIdV, const bool& IsDir, double& EffDiamX, int& FullDiamX);

// TODO: Implement in the future
//template <class PGraph> int GetRangeDist(const PGraph& Graph, const int& SrcNId, const int& DstNId, const bool& IsDir=false);
//template <class PGraph> int GetShortPath(const PGraph& Graph, const int& SrcNId, TIntH& NIdToDistH, const bool& IsDir=false, const int& MaxDist=1000);
//template <class PGraph> int GetShortPath(const PGraph& Graph, const int& SrcNId, const TIntSet& TargetSet, const bool& IsDir, TIntV& PathNIdV);
//template <class PGraph> int GetShortPath(TIntH& NIdPrnH, TCcQueue<int>& NIdQ, const PGraph& Graph, const int& SrcNId, const TIntSet& TargetSet, const bool& IsDir, TIntV& PathNIdV);
//template <class PGraph> int GetMxShortDist(const PGraph& Graph, const int& SrcNId, const bool& IsDir=false);
//template <class PGraph> int GetMxShortDist(const PGraph& Graph, const int& SrcNId, const bool& IsDir, int& MxDistNId);
//template <class PGraph> int GetMxShortDist(const PGraph& Graph, const int& SrcNId, const bool& IsDir, int& MxDistNId, TCcQueue<int>& NIdQ, TCcQueue<int>& DistQ, TIntSet& VisitedH);
//template <class PGraph> int GetMxGreedyDist(const PGraph& Graph, const int& SrcNId, const bool& IsDir=false);
//template <class PGraph> int GetMxGreedyDist(const PGraph& Graph, const int& SrcNId, const bool& IsDir, TCcQueue<int>& NIdQ, TCcQueue<int>& DistQ, TIntSet& VisitedH);
//template <class PGraph> PNGraph GetShortPathsSubGraph(const PGraph& Graph, const TIntV& SubGraphNIdV);
//template <class PGraph> PGraph GetWccPathsSubGraph(const PGraph& Graph, const TIntV& NIdV);
//template <class PGraph> void GetSubTreeSz(const PGraph& Graph, const int& StartNId, const bool& FollowOutEdges, int& TreeSz, int& TreeDepth);

} // namespace TSnap

//#//////////////////////////////////////////////
/// Breath-First-Search class.
/// The class is meant for executing many BFSs over a fixed graph. This means that the class can keep the hash tables and queues initialized between different calls of the DoBfs() function.
template<class PGraph>
class TBreathFS {
public:
  PGraph Graph;
  TSnapQueue<int> Queue;
  TInt StartNId;
  TIntH NIdDistH;
public:
  TBreathFS(const PGraph& GraphPt, const bool& InitBigQ=true) :
    Graph(GraphPt), Queue(InitBigQ?Graph->GetNodes():1024), NIdDistH(InitBigQ?Graph->GetNodes():1024) { }
  /// Sets the graph to be used by the BFS to GraphPt and resets the data structures.
  void SetGraph(const PGraph& GraphPt);
  /// Performs BFS from node id StartNode for at maps MxDist steps by only following in-links (parameter FollowIn = true) and/or out-links (parameter FollowOut = true).
  int DoBfs(const int& StartNode, const bool& FollowOut, const bool& FollowIn, const int& TargetNId=-1, const int& MxDist=TInt::Mx);
  /// Same functionality as DoBfs with better performance.
  int DoBfsHybrid(const int& StartNode, const bool& FollowOut, const bool& FollowIn, const int& TargetNId=-1, const int& MxDist=TInt::Mx);
  /// Returns the number of nodes visited/reached by the BFS.
  int GetNVisited() const { return NIdDistH.Len(); }
  /// Returns the IDs of the nodes visited/reached by the BFS.
  void GetVisitedNIdV(TIntV& NIdV) const { NIdDistH.GetKeyV(NIdV); }
  /// Returns the shortst path distance between SrcNId and DistNId.
  /// Note you have to first call DoBFs(). SrcNId must be equal to StartNode, otherwise return value is -1.
  int GetHops(const int& SrcNId, const int& DstNId) const;
  /// Returns a random shortest path from SrcNId to DstNId.
  /// Note you have to first call DoBFs(). SrcNId must be equal to StartNode, otherwise return value is -1.
  int GetRndPath(const int& SrcNId, const int& DstNId, TIntV& PathNIdV) const;

/* Private variables and functions for DoBfsHybrid */
private:
  int Stage; // 0, 2: top down, 1: bottom up
  static const unsigned int alpha = 100;
  static const unsigned int beta = 20;
  /* Private functions */
  bool TopDownStep(TIntV &NIdDistV, TIntV *Frontier, TIntV *NextFrontier, int& MaxDist, const int& TargetNId, const bool& FollowOut, const bool& FollowIn);
  bool BottomUpStep(TIntV &NIdDistV, TIntV *Frontier, TIntV *NextFrontier, int& MaxDist, const int& TargetNId, const bool& FollowOut, const bool& FollowIn);
};

template<class PGraph>
void TBreathFS<PGraph>::SetGraph(const PGraph& GraphPt) {
  Graph=GraphPt;
  const int N=GraphPt->GetNodes();
  if (Queue.Reserved() < N) { Queue.Gen(N); }
  if (NIdDistH.GetReservedKeyIds() < N) { NIdDistH.Gen(N); }
}

template<class PGraph>
int TBreathFS<PGraph>::DoBfs(const int& StartNode, const bool& FollowOut, const bool& FollowIn, const int& TargetNId, const int& MxDist) {
  StartNId = StartNode;
  IAssert(Graph->IsNode(StartNId));
//  const typename PGraph::TObj::TNodeI StartNodeI = Graph->GetNI(StartNode);
//  IAssertR(StartNodeI.GetOutDeg() > 0, TStr::Fmt("No neighbors from start node %d.", StartNode));
  NIdDistH.Clr(false);  NIdDistH.AddDat(StartNId, 0);
  Queue.Clr(false);  Queue.Push(StartNId);
  int v, MaxDist = 0;
  while (! Queue.Empty()) {
    const int NId = Queue.Top();  Queue.Pop();
    const int Dist = NIdDistH.GetDat(NId);
    if (Dist == MxDist) { break; } // max distance limit reached
    const typename PGraph::TObj::TNodeI NodeI = Graph->GetNI(NId);
    if (FollowOut) { // out-links
      for (v = 0; v < NodeI.GetOutDeg(); v++) {  // out-links
        const int DstNId = NodeI.GetOutNId(v);
        if (! NIdDistH.IsKey(DstNId)) {
          NIdDistH.AddDat(DstNId, Dist+1);
          MaxDist = TMath::Mx(MaxDist, Dist+1);
          if (DstNId == TargetNId) { return MaxDist; }
          Queue.Push(DstNId);
        }
      }
    }
    if (FollowIn) { // in-links
      for (v = 0; v < NodeI.GetInDeg(); v++) {
        const int DstNId = NodeI.GetInNId(v);
        if (! NIdDistH.IsKey(DstNId)) {
          NIdDistH.AddDat(DstNId, Dist+1);
          MaxDist = TMath::Mx(MaxDist, Dist+1);
          if (DstNId == TargetNId) { return MaxDist; }
          Queue.Push(DstNId);
        }
      }
    }
  }
  return MaxDist;
}

template<class PGraph>
int TBreathFS<PGraph>::DoBfsHybrid(const int& StartNode, const bool& FollowOut, const bool& FollowIn, const int& TargetNId, const int& MxDist) {
  StartNId = StartNode;
  IAssert(Graph->IsNode(StartNId));
  if (TargetNId == StartNode) return 0;
  const typename PGraph::TObj::TNodeI StartNodeI = Graph->GetNI(StartNode);

  // Initialize vector
  TIntV NIdDistV(Graph->GetMxNId() + 1);
  for (int i = 0; i < NIdDistV.Len(); i++) {
    NIdDistV.SetVal(i, -1);
  }
  TIntV *Frontier = new TIntV(Graph->GetNodes(), 0);
  TIntV *NextFrontier = new TIntV(Graph->GetNodes(), 0);

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
  NIdDistH.Clr(false);
  for (int NId = 0; NId < NIdDistV.Len(); NId++) {
    if (NIdDistV[NId] != -1) {
      NIdDistH.AddDat(NId, NIdDistV[NId]);
    }
  }
  return MaxDist;
}

template<class PGraph>
bool TBreathFS<PGraph>::TopDownStep(TIntV &NIdDistV, TIntV *Frontier, TIntV *NextFrontier, int& MaxDist, const int& TargetNId, const bool& FollowOut, const bool& FollowIn) {
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
bool TBreathFS<PGraph>::BottomUpStep(TIntV &NIdDistV, TIntV *Frontier, TIntV *NextFrontier, int& MaxDist, const int& TargetNId, const bool& FollowOut, const bool& FollowIn) {
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

template<class PGraph>
int TBreathFS<PGraph>::GetHops(const int& SrcNId, const int& DstNId) const {
  TInt Dist;
  if (SrcNId!=StartNId) { return -1; }
  if (! NIdDistH.IsKeyGetDat(DstNId, Dist)) { return -1; }
  return Dist.Val;
}

template<class PGraph>
int TBreathFS<PGraph>::GetRndPath(const int& SrcNId, const int& DstNId, TIntV& PathNIdV) const {
  PathNIdV.Clr(false);
  if (SrcNId!=StartNId || ! NIdDistH.IsKey(DstNId)) { return -1; }
  PathNIdV.Add(DstNId);
  TIntV CloserNIdV;
  int CurNId = DstNId;
  TInt CurDist, NextDist;
  while (CurNId != SrcNId) {
    typename PGraph::TObj::TNodeI NI = Graph->GetNI(CurNId);
    IAssert(NIdDistH.IsKeyGetDat(CurNId, CurDist));
    CloserNIdV.Clr(false);
    for (int e = 0; e < NI.GetDeg(); e++) {
      const int Next = NI.GetNbrNId(e);
      if (NIdDistH.IsKeyGetDat(Next, NextDist)) {
        if (NextDist == CurDist-1) { CloserNIdV.Add(Next); }
      }
    }
    IAssert(! CloserNIdV.Empty());
    CurNId = CloserNIdV[TInt::Rnd.GetUniDevInt(CloserNIdV.Len())];
    PathNIdV.Add(CurNId);
  }
  PathNIdV.Reverse();
  return PathNIdV.Len()-1;
}

/////////////////////////////////////////////////
// Implementation
namespace TSnap {

template <class PGraph>
PNGraph GetBfsTree(const PGraph& Graph, const int& StartNId, const bool& FollowOut, const bool& FollowIn) {
  TBreathFS<PGraph> BFS(Graph);
  BFS.DoBfs(StartNId, FollowOut, FollowIn, -1, TInt::Mx);
  PNGraph Tree = TNGraph::New();
  BFS.NIdDistH.SortByDat();
  for (int i = 0; i < BFS.NIdDistH.Len(); i++) {
    const int NId = BFS.NIdDistH.GetKey(i);
    const int Dist = BFS.NIdDistH[i];
    typename PGraph::TObj::TNodeI NI = Graph->GetNI(NId);
    if (!Tree->IsNode(NId)) {
      Tree->AddNode(NId);
    }
    if (FollowOut) {
      for (int e = 0; e < NI.GetInDeg(); e++) {
        const int Prev = NI.GetInNId(e);
        if (Tree->IsNode(Prev) && BFS.NIdDistH.GetDat(Prev)==Dist-1) {
          Tree->AddEdge(Prev, NId); }
      }
    }
    if (FollowIn) {
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        const int Prev = NI.GetOutNId(e);
        if (Tree->IsNode(Prev) && BFS.NIdDistH.GetDat(Prev)==Dist-1) {
          Tree->AddEdge(Prev, NId); }
      }
    }
  }
  return Tree;
}

template <class PGraph>
int GetSubTreeSz(const PGraph& Graph, const int& StartNId, const bool& FollowOut, const bool& FollowIn, int& TreeSz, int& TreeDepth) {
  TBreathFS<PGraph> BFS(Graph);
  BFS.DoBfs(StartNId, FollowOut, FollowIn, -1, TInt::Mx);
  TreeSz = BFS.NIdDistH.Len();
  TreeDepth = 0;
  for (int i = 0; i < BFS.NIdDistH.Len(); i++) {
    TreeDepth = TMath::Mx(TreeDepth, BFS.NIdDistH[i].Val);
  }
  return TreeSz;
}

template <class PGraph>
int GetNodesAtHop(const PGraph& Graph, const int& StartNId, const int& Hop, TIntV& NIdV, const bool& IsDir) {
  TBreathFS<PGraph> BFS(Graph);
  BFS.DoBfs(StartNId, true, !IsDir, -1, Hop);
  NIdV.Clr(false);
  for (int i = 0; i < BFS.NIdDistH.Len(); i++) {
    if (BFS.NIdDistH[i] == Hop) {
      NIdV.Add(BFS.NIdDistH.GetKey(i)); }
  }
  return NIdV.Len();
}

template <class PGraph>
int GetNodesAtHops(const PGraph& Graph, const int& StartNId, TIntPrV& HopCntV, const bool& IsDir) {
  TBreathFS<PGraph> BFS(Graph);
  BFS.DoBfs(StartNId, true, !IsDir, -1, TInt::Mx);
  TIntH HopCntH;
  for (int i = 0; i < BFS.NIdDistH.Len(); i++) {
    HopCntH.AddDat(BFS.NIdDistH[i]) += 1;
  }
  HopCntH.GetKeyDatPrV(HopCntV);
  HopCntV.Sort();
  return HopCntV.Len();
}

template <class PGraph>
int GetShortPath(const PGraph& Graph, const int& SrcNId, TIntH& NIdToDistH, const bool& IsDir, const int& MaxDist) {
  TBreathFS<PGraph> BFS(Graph);
  BFS.DoBfs(SrcNId, true, ! IsDir, -1, MaxDist);
  NIdToDistH.Clr();
  NIdToDistH.Swap(BFS.NIdDistH);
  return NIdToDistH[NIdToDistH.Len()-1];
}

template <class PGraph>
int GetShortPath(const PGraph& Graph, const int& SrcNId, const int& DstNId, const bool& IsDir) {
  TBreathFS<PGraph> BFS(Graph);
  BFS.DoBfs(SrcNId, true, ! IsDir, DstNId, TInt::Mx);
  return BFS.GetHops(SrcNId, DstNId);
}

template <class PGraph>
int GetBfsFullDiam(const PGraph& Graph, const int& NTestNodes, const bool& IsDir) {
  int FullDiam;
  double EffDiam;
  GetBfsEffDiam(Graph, NTestNodes, IsDir, EffDiam, FullDiam);
  return FullDiam;
}

template <class PGraph>
double GetBfsEffDiam(const PGraph& Graph, const int& NTestNodes, const bool& IsDir) {
  int FullDiam;
  double EffDiam;
  GetBfsEffDiam(Graph, NTestNodes, IsDir, EffDiam, FullDiam);
  return EffDiam;
}

template <class PGraph>
double GetBfsEffDiam(const PGraph& Graph, const int& NTestNodes, const bool& IsDir, double& EffDiam, int& FullDiam) {
  double AvgDiam;
  EffDiam = -1;  FullDiam = -1;
  return GetBfsEffDiam(Graph, NTestNodes, IsDir, EffDiam, FullDiam, AvgDiam);
}

template <class PGraph>
double GetBfsEffDiam(const PGraph& Graph, const int& NTestNodes, const bool& IsDir, double& EffDiam, int& FullDiam, double& AvgSPL) {
  EffDiam = -1;  FullDiam = -1;  AvgSPL = -1;
  TIntFltH DistToCntH;
  TBreathFS<PGraph> BFS(Graph);
  // shotest paths
  TIntV NodeIdV;
  Graph->GetNIdV(NodeIdV);  NodeIdV.Shuffle(TInt::Rnd);
  for (int tries = 0; tries < TMath::Mn(NTestNodes, Graph->GetNodes()); tries++) {
    const int NId = NodeIdV[tries];
    BFS.DoBfs(NId, true, ! IsDir, -1, TInt::Mx);
    for (int i = 0; i < BFS.NIdDistH.Len(); i++) {
      DistToCntH.AddDat(BFS.NIdDistH[i]) += 1; }
  }
  TIntFltKdV DistNbrsPdfV;
  double SumPathL=0, PathCnt=0;
  for (int i = 0; i < DistToCntH.Len(); i++) {
    DistNbrsPdfV.Add(TIntFltKd(DistToCntH.GetKey(i), DistToCntH[i]));
    SumPathL += DistToCntH.GetKey(i) * DistToCntH[i];
    PathCnt += DistToCntH[i];
  }
  DistNbrsPdfV.Sort();
  EffDiam = TSnap::TSnapDetail::CalcEffDiamPdf(DistNbrsPdfV, 0.9); // effective diameter (90-th percentile)
  FullDiam = DistNbrsPdfV.Last().Key;                // approximate full diameter (max shortest path length over the sampled nodes)
  AvgSPL = SumPathL/PathCnt;                        // average shortest path length
  return EffDiam;
}

template <class PGraph>
double GetBfsEffDiamAll(const PGraph& Graph, const int& NTestNodes, const bool& IsDir, double& EffDiam, int& FullDiam, double& AvgSPL) {
  return GetBfsEffDiam(Graph, NTestNodes, IsDir, EffDiam, FullDiam, AvgSPL);
}

template <class PGraph>
double GetBfsEffDiam(const PGraph& Graph, const int& NTestNodes, const TIntV& SubGraphNIdV, const bool& IsDir, double& EffDiam, int& FullDiam) {
  EffDiam = -1;
  FullDiam = -1;

  TIntFltH DistToCntH;
  TBreathFS<PGraph> BFS(Graph);
  // shotest paths
  TIntV NodeIdV(SubGraphNIdV);  NodeIdV.Shuffle(TInt::Rnd);
  TInt Dist;
  for (int tries = 0; tries < TMath::Mn(NTestNodes, SubGraphNIdV.Len()); tries++) {
    const int NId = NodeIdV[tries];
    BFS.DoBfs(NId, true, ! IsDir, -1, TInt::Mx);
    for (int i = 0; i < SubGraphNIdV.Len(); i++) {
      if (BFS.NIdDistH.IsKeyGetDat(SubGraphNIdV[i], Dist)) {
        DistToCntH.AddDat(Dist) += 1;
      }
    }
  }
  TIntFltKdV DistNbrsPdfV;
  for (int i = 0; i < DistToCntH.Len(); i++) {
    DistNbrsPdfV.Add(TIntFltKd(DistToCntH.GetKey(i), DistToCntH[i]));
  }
  DistNbrsPdfV.Sort();
  EffDiam = TSnap::TSnapDetail::CalcEffDiamPdf(DistNbrsPdfV, 0.9);  // effective diameter (90-th percentile)
  FullDiam = DistNbrsPdfV.Last().Key;                 // approximate full diameter (max shortest path length over the sampled nodes)
  return EffDiam;                                     // average shortest path length
}

template <class PGraph>
int GetShortestDistances(const PGraph& Graph, const int& StartNId, const bool& FollowOut, const bool& FollowIn, TIntV& ShortestDists) {
  PSOut StdOut = TStdOut::New();
  int MxNId = Graph->GetMxNId();
  int NonNodeDepth = 2147483647; // INT_MAX
  int InfDepth = 2147483646; // INT_MAX - 1
  ShortestDists.Gen(MxNId);
  for (int NId = 0; NId < MxNId; NId++) {
    if (Graph->IsNode(NId)) { ShortestDists[NId] = InfDepth; }
    else { ShortestDists[NId] = NonNodeDepth; }
  }

  TIntV Vec1(MxNId, 0); // ensure enough capacity
  TIntV Vec2(MxNId, 0); // ensure enough capacity

  ShortestDists[StartNId] = 0;
  TIntV* PCurV = &Vec1;
  PCurV->Add(StartNId);
  TIntV* PNextV = &Vec2;
  int Depth = 0; // current depth
  while (!PCurV->Empty()) {
    Depth++; // increase depth
    for (int i = 0; i < PCurV->Len(); i++) {
      int NId = PCurV->GetVal(i);
      typename PGraph::TObj::TNodeI NI = Graph->GetNI(NId);
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        const int OutNId = NI.GetOutNId(e);
        if (ShortestDists[OutNId].Val == InfDepth) {
          ShortestDists[OutNId] = Depth;
          PNextV->Add(OutNId);
        }
      }
    }
    // swap pointer, no copying
    TIntV* Tmp = PCurV;
    PCurV = PNextV;
    PNextV = Tmp;
    // clear next
    PNextV->Reduce(0); // reduce length, does not initialize new array
  }
  return Depth-1;
}

#ifdef USE_OPENMP
template <class PGraph>
int GetShortestDistancesMP2(const PGraph& Graph, const int& StartNId, const bool& FollowOut, const bool& FollowIn, TIntV& ShortestDists) {
  int MxNId = Graph->GetMxNId();
  int NonNodeDepth = 2147483647; // INT_MAX
  int InfDepth = 2147483646; // INT_MAX - 1
  ShortestDists.Gen(MxNId);
  #pragma omp parallel for schedule(dynamic,10000)
  for (int NId = 0; NId < MxNId; NId++) {
    if (Graph->IsNode(NId)) { ShortestDists[NId] = InfDepth; }
    else { ShortestDists[NId] = NonNodeDepth; }
  }

  TIntV Vec1(MxNId, 0); // ensure enough capacity
  TIntV Vec2(MxNId, 0); // ensure enough capacity

  ShortestDists[StartNId] = 0;
  TIntV* PCurV = &Vec1;
  PCurV->Add(StartNId);
  TIntV* PNextV = &Vec2;
  int Depth = 0; // current depth

  while (!PCurV->Empty()) {
    Depth++; // increase depth
    #pragma omp parallel for schedule(dynamic,10000)
    for (int i = 0; i < PCurV->Len(); i++) {
      int NId = PCurV->GetVal(i);
      typename PGraph::TObj::TNodeI NI = Graph->GetNI(NId);
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        const int OutNId = NI.GetOutNId(e);
        if (__sync_bool_compare_and_swap(&(ShortestDists[OutNId].Val), InfDepth, Depth)) {
          PNextV->AddMP(OutNId);
        }
      }
    }
//      #pragma omp parallel for schedule(dynamic,10000)
//      for (int NId = 0; NId < MxNId; NId++) {
//        if (ShortestDists[NId] == InfDepth) {
//          typename PGraph::TObj::TNodeI NI = Graph->GetNI(NId);
//          for (int e = 0; e < NI.GetInDeg(); e++) {
//            const int InNId = NI.GetInNId(e);
//            if (ShortestDists[InNId] < Depth) {
//              ShortestDists[NId] = Depth;
//              PNextV->AddMP(NId);
//              break;
//            }
//          }
//        }
//      }
    // swap pointer, no copying
    TIntV* Tmp = PCurV;
    PCurV = PNextV;
    PNextV = Tmp;
    // clear next
    PNextV->Reduce(0); // reduce length, does not initialize new array
  }
  return Depth-1;
}
#endif // USE_OPENMP

} // namespace TSnap
