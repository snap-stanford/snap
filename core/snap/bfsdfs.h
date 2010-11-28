namespace TSnap {

/////////////////////////////////////////////////
// BFS and DFS
template <class PGraph> PNGraph GetBfsTree(const PGraph& Graph, const int& StartNId, const bool& FollowOut, const bool& FollowIn);
template <class PGraph> int GetSubTreeSz(const PGraph& Graph, const int& StartNId, const bool& FollowOut, const bool& FollowIn, int& TreeSz, int& TreeDepth);
template <class PGraph> int GetNodesAtHop(const PGraph& Graph, const int& StartNId, const int& Hop, TIntV& NIdV, const bool& Dir=false);
template <class PGraph> int GetNodesAtHops(const PGraph& Graph, const int& StartNId, TIntPrV& HopCntV, const bool& Dir=false);

/////////////////////////////////////////////////
// Paths
template <class PGraph> int GetShortPath(const PGraph& Graph, const int& SrcNId, TIntH& NIdToDistH, const bool& Dir=false, const int& MaxDist=TInt::Mx);
template <class PGraph> int GetShortPath(const PGraph& Graph, const int& SrcNId, const int& DstNId, const bool& Dir=false);

/////////////////////////////////////////////////
// Diameter
template <class PGraph> int GetBfsFullDiam(const PGraph& Graph, const int& TestNodes, const bool& Dir=false);
template <class PGraph> double GetBfsEffDiam(const PGraph& Graph, const int& TestNodes, const bool& Dir=false);
template <class PGraph> double GetBfsEffDiam(const PGraph& Graph, const int& TestNodes, const bool& Dir, double& EffDiam, int& FullDiam);
template <class PGraph> double GetBfsEffDiam(const PGraph& Graph, const int& TestNodes, const bool& Dir, double& EffDiam, int& FullDiam, double& AvgDiam);
template <class PGraph> double GetBfsEffDiam(const PGraph& Graph, const int& TestNodes, const TIntV& SubGraphNIdV, const bool& Dir, double& EffDiam, int& FullDiam);

//template <class PGraph> int GetShortPathDist(const PGraph& Graph, const int& SrcNId, const int& DstNId, const bool& Dir=false);
//template <class PGraph> int GetRangeDist(const PGraph& Graph, const int& SrcNId, const int& DstNId, const bool& Dir=false);
//template <class PGraph> int GetShortPath(const PGraph& Graph, const int& SrcNId, TIntH& NIdToDistH, const bool& Dir=false, const int& MaxDist=1000);
//template <class PGraph> int GetShortPath(const PGraph& Graph, const int& SrcNId, const TIntSet& TargetSet, const bool& Dir, TIntV& PathNIdV);
//template <class PGraph> int GetShortPath(TIntH& NIdPrnH, TCcQueue<int>& NIdQ, const PGraph& Graph, const int& SrcNId, const TIntSet& TargetSet, const bool& Dir, TIntV& PathNIdV);
//template <class PGraph> int GetMxShortDist(const PGraph& Graph, const int& SrcNId, const bool& Dir=false);
//template <class PGraph> int GetMxShortDist(const PGraph& Graph, const int& SrcNId, const bool& Dir, int& MxDistNId);
//template <class PGraph> int GetMxShortDist(const PGraph& Graph, const int& SrcNId, const bool& Dir, int& MxDistNId, TCcQueue<int>& NIdQ, TCcQueue<int>& DistQ, TIntSet& VisitedH);
//template <class PGraph> int GetMxGreedyDist(const PGraph& Graph, const int& SrcNId, const bool& Dir=false);
//template <class PGraph> int GetMxGreedyDist(const PGraph& Graph, const int& SrcNId, const bool& Dir, TCcQueue<int>& NIdQ, TCcQueue<int>& DistQ, TIntSet& VisitedH);

//template <class PGraph> PNGraph GetBfsTree(const PGraph& Graph, const int& StartNId, TIntH& TargetNIdH, const bool& IsDir);
//template <class PGraph> PNGraph GetShortPathsSubGraph(const PGraph& Graph, const TIntV& SubGraphNIdV);
//template <class PGraph> PGraph GetWccPathsSubGraph(const PGraph& Graph, const TIntV& NIdV);
//template <class PGraph> void GetSubTreeSz(const PGraph& Graph, const int& StartNId, const bool& FollowOutEdges, int& TreeSz, int& TreeDepth);
//template <class PGraph> PNGraph GetDfsTree(const PGraph& Graph, const int& StartNId, const bool& IsDir);
//template <class PGraph> void GetDfsNumber(const PGraph& Graph, THash<TInt, TIntPr>& DfsBegEndH);
//template <class PGraph, class TVisitor> void GetDfsVisitor(const PGraph& Graph, TVisitor& Visitor);

}; // namespace TSnap

/////////////////////////////////////////////////
// Breath First Search
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
  void SetGraph(const PGraph& GraphPt);
  int DoBfs(const int& StartNode, const bool& FollowOut, const bool& FollowIn, const int& TargetNId=-1, const int& MxDist=TInt::Mx);
  int GetNVisited() const { return NIdDistH.Len(); }
  void GetVisitedNIdV(TIntV& NIdV) const { NIdDistH.GetKeyV(NIdV); }
  int GetHops(const int& SrcNId, const int& DstNId) const;
  int GetRndPath(const int& SrcNId, const int& DstNId, TIntV& PathNIdV) const;
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
int TBreathFS<PGraph>::GetHops(const int& SrcNId, const int& DstNId) const {
  TInt Dist;
  if (SrcNId!=StartNId) { return -1; }
  if (! NIdDistH.IsKeyGetDat(DstNId, Dist)) { return -1; }
  return Dist.Val;
}

// get random shortest path from SrcNId to DstNId
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
      const int Next = NI.GetNbhNId(e);
      IAssert(NIdDistH.IsKeyGetDat(Next, NextDist));
      if (NextDist == CurDist-1) { CloserNIdV.Add(Next); }
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

// parent points to childer
template <class PGraph>
PNGraph GetBfsTree(const PGraph& Graph, const int& StartNId, const bool& FollowOut, const bool& FollowIn) {
  TBreathFS<PGraph> BFS(Graph, false);
  BFS.DoBfs(StartNId, FollowOut, FollowIn, -1, TInt::Mx);
  PNGraph Tree = TNGraph::New();
  BFS.NIdDistH.SortByDat();
  Tree->AddNode(StartNId);
  for (int i = 0; i < BFS.NIdDistH.Len(); i++) {
    const int NId = BFS.NIdDistH.GetKey(i);
    const int Dist = BFS.NIdDistH[i];
    typename PGraph::TObj::TNodeI NI = Graph->GetNI(NId);
    Tree->AddNode(NId);
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

// get IDs of nodes that are at distance Hop from StartNId
template <class PGraph>
int GetNodesAtHop(const PGraph& Graph, const int& StartNId, const int& Hop, TIntV& NIdV, const bool& Dir) {
  TBreathFS<PGraph> BFS(Graph);
  BFS.DoBfs(StartNId, true, !Dir, -1, Hop);
  NIdV.Clr(false);
  for (int i = 0; i < BFS.NIdDistH.Len(); i++) {
    if (BFS.NIdDistH[i] == Hop) {
      NIdV.Add(BFS.NIdDistH.GetKey(i)); }
  }
  return NIdV.Len();
}

// return the number of nodes at each hop distance from StartNId
template <class PGraph>
int GetNodesAtHops(const PGraph& Graph, const int& StartNId, TIntPrV& HopCntV, const bool& Dir) {
  TBreathFS<PGraph> BFS(Graph);
  BFS.DoBfs(StartNId, true, !Dir, -1, TInt::Mx);
  TIntH HopCntH;
  for (int i = 0; i < BFS.NIdDistH.Len(); i++) {
    HopCntH.AddDat(BFS.NIdDistH[i]) += 1;
  }
  HopCntH.GetKeyDatPrV(HopCntV);
  HopCntV.Sort();
  return HopCntV.Len();
}

template <class PGraph>
int GetShortPath(const PGraph& Graph, const int& SrcNId, TIntH& NIdToDistH, const bool& Dir, const int& MaxDist) {
  TBreathFS<PGraph> BFS(Graph);
  BFS.DoBfs(SrcNId, true, ! Dir, -1, TInt::Mx);
  NIdToDistH.Clr();
  NIdToDistH.Swap(BFS.NIdDistH);
  return NIdToDistH[NIdToDistH.Len()-1];
}

template <class PGraph>
int GetBfsFullDiam(const PGraph& Graph, const int& TestNodes, const bool& Dir) {
  int FullDiam;
  double EffDiam;
  GetBfsEffDiam(Graph, TestNodes, Dir, EffDiam, FullDiam);
  return FullDiam;
}

template <class PGraph>
double GetBfsEffDiam(const PGraph& Graph, const int& TestNodes, const bool& Dir) {
  int FullDiam;
  double EffDiam;
  GetBfsEffDiam(Graph, TestNodes, Dir, EffDiam, FullDiam);
  return EffDiam;
}

// returns Effective Diameter and the Diameter of a graph (by performing BFS from TestNodes random nodes)
template <class PGraph>
double GetBfsEffDiam(const PGraph& Graph, const int& TestNodes, const bool& Dir, double& EffDiam, int& FullDiam) {
  EffDiam = -1;
  FullDiam = -1;
  TIntFltH DistToCntH;
  TBreathFS<PGraph> BFS(Graph);
  // shotest paths
  TIntV NodeIdV;
  Graph->GetNIdV(NodeIdV);  NodeIdV.Shuffle(TInt::Rnd);
  for (int tries = 0; tries < TMath::Mn(TestNodes, Graph->GetNodes()); tries++) {
    const int NId = NodeIdV[tries]; //Graph->GetRndNId();
    BFS.DoBfs(NId, true, ! Dir, -1, TInt::Mx);
    for (int i = 0; i < BFS.NIdDistH.Len(); i++) {
      DistToCntH.AddDat(BFS.NIdDistH[i]) += 1; }
  }
  TIntFltKdV DistNbhsPdfV;
  for (int i = 0; i < DistToCntH.Len(); i++) {
    DistNbhsPdfV.Add(TIntFltKd(DistToCntH.GetKey(i), DistToCntH[i]));
  }
  DistNbhsPdfV.Sort();
  EffDiam = TAnf::CalcEffDiamPdf(DistNbhsPdfV, 0.9);
  FullDiam = DistNbhsPdfV.Last().Key;
  return EffDiam;
}

// returns Effective Diameter, the Diameter and the Average Shortest Path Lenght of a graph
// (by performing BFS from TestNodes random starting nodes)
template <class PGraph>
double GetBfsEffDiam(const PGraph& Graph, const int& TestNodes, const bool& Dir, double& EffDiam, int& FullDiam, double& AvgDiam) {
  EffDiam = -1;
  FullDiam = -1;
  AvgDiam = -1;
  TIntFltH DistToCntH;
  TBreathFS<PGraph> BFS(Graph);
  // shotest paths
  TIntV NodeIdV;
  Graph->GetNIdV(NodeIdV);  NodeIdV.Shuffle(TInt::Rnd);
  for (int tries = 0; tries < TMath::Mn(TestNodes, Graph->GetNodes()); tries++) {
    const int NId = NodeIdV[tries]; //Graph->GetRndNId();
    BFS.DoBfs(NId, true, ! Dir, -1, TInt::Mx);
    for (int i = 0; i < BFS.NIdDistH.Len(); i++) {
      DistToCntH.AddDat(BFS.NIdDistH[i]) += 1; }
  }
  TIntFltKdV DistNbhsPdfV;
  double SumPathL=0, PathCnt=0;
  for (int i = 0; i < DistToCntH.Len(); i++) {
    DistNbhsPdfV.Add(TIntFltKd(DistToCntH.GetKey(i), DistToCntH[i]));
    SumPathL += DistToCntH.GetKey(i) * DistToCntH[i];
    PathCnt += DistToCntH[i];
  }
  DistNbhsPdfV.Sort();
  EffDiam = TAnf::CalcEffDiamPdf(DistNbhsPdfV, 0.9); // effective diameter (90-th percentile)
  FullDiam = DistNbhsPdfV.Last().Key; // full diameter (max shortest path length)
  AvgDiam = SumPathL/PathCnt; // average shortest path length
  return EffDiam;
}

// use whole graph (all links) to measure the distances but report the path lengths only between points in SubGraphNIdV
template <class PGraph>
double GetBfsEffDiam(const PGraph& Graph, const int& TestNodes, const TIntV& SubGraphNIdV, const bool& Dir, double& EffDiam, int& FullDiam) {
  EffDiam = -1;
  FullDiam = -1;
  TIntFltH DistToCntH;
  TBreathFS<PGraph> BFS(Graph);
  // shotest paths
  TIntV NodeIdV(SubGraphNIdV);  NodeIdV.Shuffle(TInt::Rnd);
  TInt Dist;
  for (int tries = 0; tries < TMath::Mn(TestNodes, Graph->GetNodes()); tries++) {
    const int NId = NodeIdV[tries];
    BFS.DoBfs(NId, true, ! Dir, -1, TInt::Mx);
    for (int i = 0; i < SubGraphNIdV.Len(); i++) {
      if (BFS.NIdDistH.IsKeyGetDat(SubGraphNIdV[i], Dist)) {
        DistToCntH.AddDat(Dist) += 1; }
    }
  }
  TIntFltKdV DistNbhsPdfV;
  for (int i = 0; i < DistToCntH.Len(); i++) {
    DistNbhsPdfV.Add(TIntFltKd(DistToCntH.GetKey(i), DistToCntH[i]));
  }
  DistNbhsPdfV.Sort();
  EffDiam = TAnf::CalcEffDiamPdf(DistNbhsPdfV, 0.9);
  FullDiam = DistNbhsPdfV.Last().Key;
  return EffDiam;
}

} // namespace TSnap
