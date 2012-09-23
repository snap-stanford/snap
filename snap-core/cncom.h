/////////////////////////////////////////////////
// Connected Components
class TCnCom;
typedef TVec<TCnCom> TCnComV;

namespace TSnap {

/// Returns (via output parameter CnCom) all nodes that are in the same connected component as node NId.
template <class PGraph> void GetNodeWcc(const PGraph& Graph, const int& NId, TIntV& CnCom);
/// Tests whether the Graph is (weakly) connected.
template <class PGraph> bool IsConnected(const PGraph& Graph);
/// Tests whether the Graph is weakly connected.
template <class PGraph> bool IsWeaklyConn(const PGraph& Graph);
/// Returns a distribution of weakly connected component sizes.
/// @param WccSzCnt returns a set of pairs (number of nodes in the component, number of such components)
template <class PGraph> void GetWccSzCnt(const PGraph& Graph, TIntPrV& WccSzCnt);
/// Returns all weakly connected components in a Graph.
/// @param CnComV is a vector of connected components. Each component is defined by the IDs of its member nodes.
template <class PGraph> void GetWccs(const PGraph& Graph, TCnComV& CnComV);
/// Returns a distribution of strongly connected component sizes.
/// @param SccSzCnt returns a set of pairs (number of nodes in the component, number of such components)
template <class PGraph> void GetSccSzCnt(const PGraph& Graph, TIntPrV& SccSzCnt);
/// Returns all strongly connected components in a Graph.
/// @param CnComV is a vector of connected components. Each component is defined by the IDs of its member nodes.
template <class PGraph> void GetSccs(const PGraph& Graph, TCnComV& CnComV);
/// Returns the fraction of nodes in the largest weakly connected component of a Graph.
template <class PGraph> double GetMxWccSz(const PGraph& Graph);

/// Returns a graph representing the largest weakly connected component on an input Graph.
/// A directed/undirected graph is connected if there exist an undirected path between any pair of nodes.
/// See http://en.wikipedia.org/wiki/Connected_component_(graph_theory)
template <class PGraph> PGraph GetMxWcc(const PGraph& Graph);
/// Returns a graph representing the largest strongly connected component on an input Graph.
/// A directed graph is strongly connected if there exists a directed path from any vertex to any other vertex in the graph.
/// See http://en.wikipedia.org/wiki/Strongly_connected_component
template <class PGraph> PGraph GetMxScc(const PGraph& Graph);
/// Returns a graph representing the largest bi-connected component on an input Graph.
/// An undirected graph is bi-connected if by removing any single node does not disconnect the graph.
/// http://en.wikipedia.org/wiki/Biconnected_component
template <class PGraph> PGraph GetMxBiCon(const PGraph& Graph);

/// Returns a distribution of bi-connected component sizes.
/// @param SzCntV returns a set of pairs (number of nodes in the bi-component, number of such components)
void GetBiConSzCnt(const PUNGraph& Graph, TIntPrV& SzCntV);
/// Returns all bi-connected components of a Graph.
/// @param BiCnComV is a vector of bi-connected components. Each component is defined by the IDs of its member nodes.
void GetBiCon(const PUNGraph& Graph, TCnComV& BiCnComV);
/// Returns articulartion points of a Graph.
/// Articulation point (or a cut vertex) is any node that when removed increases the number of connected components.
void GetArtPoints(const PUNGraph& Graph, TIntV& ArtNIdV);
/// Returns bridge edges of a Graph.
/// Edge is a bridge if when removed increases the number of connected components.
/// See http://en.wikipedia.org/wiki/Bridge_(graph_theory)
void GetEdgeBridges(const PUNGraph& Graph, TIntPrV& EdgeV);
/// Distribution of sizes of 1-components, maximal components of that can be disconnected from the Graph by removing a single edge.
/// We find such components as follows: Find all bridge edges, remove them from the Graph, find largest component K and
/// add back all bridges that do not touch K. Now, find the connected components of this graph.
void Get1CnComSzCnt(const PUNGraph& Graph, TIntPrV& SzCntV);
/// Returns 1-components: maximal connected components of that can be disconnected from the Graph by removing a single edge.
/// We find such components as follows: Find all bridge edges, remove them from the Graph, find largest component K and
/// add back all bridges that do not touch K. Now, find the connected components of this graph.
void Get1CnCom(const PUNGraph& Graph, TCnComV& Cn1ComV);
/// Returns a graph representing the largest bi-connected component on an undirected Graph.
/// An undirected graph is bi-connected if by removing any single node does not disconnect the graph.
/// http://en.wikipedia.org/wiki/Biconnected_component
/// @param RenumberNodes if true, then node IDs of the returned graph will not match those of Graph, instead they will have a range 0...N-1.
PUNGraph GetMxBiCon(const PUNGraph& Graph, const bool& RenumberNodes=false);

}; // namespace TSnap

/////////////////////////////////////////////////
/// Connected Component.
/// Connected component is defined by a vector of its node IDs.
class TCnCom {
public:
  TIntV NIdV;
public:
  TCnCom() : NIdV() { }
  TCnCom(const TIntV& NodeIdV) : NIdV(NodeIdV) { }
  TCnCom(const TCnCom& CC) : NIdV(CC.NIdV) { }
  TCnCom(TSIn& SIn) : NIdV(SIn) { }
  void Save(TSOut& SOut) const { NIdV.Save(SOut); }
  TCnCom& operator = (const TCnCom& CC) { if (this != &CC) NIdV = CC.NIdV;  return *this; }
  bool operator == (const TCnCom& CC) const { return NIdV == CC.NIdV; }
  bool operator < (const TCnCom& CC) const { return NIdV < CC.NIdV; }

  int Len() const { return NIdV.Len(); }
  bool Empty() const { return NIdV.Empty(); }
  void Clr() { NIdV.Clr(); }
  void Add(const int& NodeId) { NIdV.Add(NodeId); }
  const TInt& operator [] (const int& NIdN) const { return NIdV[NIdN]; }
  const TIntV& operator () () const { return NIdV; }
  TIntV& operator () () { return NIdV; }
  void Sort(const bool& Asc = true) { NIdV.Sort(Asc); }
  bool IsNIdIn(const int& NId) const { return NIdV.SearchBin(NId) != -1; }
  const TInt& GetRndNId() const { return NIdV[TInt::Rnd.GetUniDevInt(Len())]; }
  static void Dump(const TCnComV& CnComV, const TStr& Desc=TStr());
  static void SaveTxt(const TCnComV& CnComV, const TStr& FNm, const TStr& Desc=TStr());
  /// Depth-First-Search.
  /// Depending on the stage of DFS a different member function of Visitor class is called.
  /// See source code for details.
  template <class PGraph, class TVisitor>
  static void GetDfsVisitor(const PGraph& Graph, TVisitor& Visitor);
};

template <class PGraph, class TVisitor>
void TCnCom::GetDfsVisitor(const PGraph& Graph, TVisitor& Visitor) {
  const int Nodes = Graph->GetNodes();
  TSStack<TIntTr> Stack(Nodes);
  int edge=0, Deg=0, U=0;
  TIntH ColorH(Nodes);
  typename PGraph::TObj::TNodeI NI, UI;
  for (NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    U = NI.GetId();
    if (! ColorH.IsKey(U)) {         // is unvisited node
      ColorH.AddDat(U, 1); 
      Visitor.DiscoverNode(U);       // discover
      Stack.Push(TIntTr(U, 0, Graph->GetNI(U).GetOutDeg()));
      while (! Stack.Empty()) {
        const TIntTr& Top = Stack.Top();
        U=Top.Val1; edge=Top.Val2; Deg=Top.Val3;
        typename PGraph::TObj::TNodeI UI = Graph->GetNI(U);
        Stack.Pop();
        while (edge != Deg) {
          const int V = UI.GetOutNId(edge);
          Visitor.ExamineEdge(U, V); // examine edge
          if (! ColorH.IsKey(V)) {
            Visitor.TreeEdge(U, V);  // tree edge
            Stack.Push(TIntTr(U, ++edge, Deg));
            U = V;
            ColorH.AddDat(U, 1); 
            Visitor.DiscoverNode(U); // discover
            UI = Graph->GetNI(U);
            edge = 0;  Deg = UI.GetOutDeg();
          }
          else if (ColorH.GetDat(V) == 1) {
            Visitor.BackEdge(U, V);  // edge upward
            ++edge; }
          else {
            Visitor.FwdEdge(U, V);   // edge downward
            ++edge; }
        }
        ColorH.AddDat(U, 2); 
        Visitor.FinishNode(U);       // finish
      }
    }
  }
}

/////////////////////////////////////////////////
/// Articulation point Depth-First-Search visitor class.
class TArtPointVisitor {
public:
  THash<TInt, TIntPr> VnLowH;
  THash<TInt, TInt> ParentH;
  TIntSet ArtSet;
  TInt Time;
public:
  TArtPointVisitor() { }
  TArtPointVisitor(const int& Nodes) : VnLowH(Nodes), ParentH(Nodes)  { }
  void DiscoverNode(int NId) { Time++; VnLowH.AddDat(NId, TIntPr(Time, Time)); }
  void FinishNode(const int& NId) {
    if (! ParentH.IsKey(NId)) { return; }  const int Prn = ParentH.GetDat(NId);
    VnLowH.GetDat(Prn).Val2 = TMath::Mn(VnLowH.GetDat(Prn).Val2, VnLowH.GetDat(NId).Val2);
    if (VnLowH.GetDat(Prn).Val1==1 && VnLowH.GetDat(NId).Val1!=2) { ArtSet.AddKey(Prn); }
    if (VnLowH.GetDat(Prn).Val1!=1 && VnLowH.GetDat(NId).Val2>=VnLowH.GetDat(Prn).Val1) { ArtSet.AddKey(Prn); } }
  void ExamineEdge(const int& NId1, const int& NId2) { }
  void TreeEdge(const int& NId1, const int& NId2) { ParentH.AddDat(NId2, NId1); }
  void BackEdge(const int& NId1, const int& NId2) {
    if (ParentH.IsKey(NId1) && ParentH.GetDat(NId1)!=NId2) {
      VnLowH.GetDat(NId1).Val2 = TMath::Mn(VnLowH.GetDat(NId1).Val2, VnLowH.GetDat(NId2).Val1); } }
  void FwdEdge(const int& NId1, const int& NId2) {
    VnLowH.GetDat(NId1).Val2 = TMath::Mn(VnLowH.GetDat(NId1).Val2, VnLowH.GetDat(NId2).Val1); }
};

/////////////////////////////////////////////////
/// Biconnected componetns Depth-First-Search visitor class.
class TBiConVisitor {
public:
  THash<TInt, TIntPr> VnLowH;
  THash<TInt, TInt> ParentH;
  TSStack<TIntPr> Stack;
  TCnComV CnComV;
  TIntSet NSet;
  TInt Time;
public:
  TBiConVisitor() { }
  TBiConVisitor(const int& Nodes) : VnLowH(Nodes), ParentH(Nodes), Stack(Nodes) { }
  void DiscoverNode(int NId) { Time++; VnLowH.AddDat(NId, TIntPr(Time, Time)); }
  void FinishNode(const int& NId) {
    if (! ParentH.IsKey(NId)) { return; }  const int Prn = ParentH.GetDat(NId);
    VnLowH.GetDat(Prn).Val2 = TMath::Mn(VnLowH.GetDat(Prn).Val2, VnLowH.GetDat(NId).Val2);
    if (VnLowH.GetDat(NId).Val2 >= VnLowH.GetDat(Prn).Val1) {
      NSet.Clr(false);
      while (! Stack.Empty() && Stack.Top() != TIntPr(Prn, NId)) {
        const TIntPr& Top = Stack.Top();
        NSet.AddKey(Top.Val1);  NSet.AddKey(Top.Val2); Stack.Pop();  }
      if (! Stack.Empty()) {
        const TIntPr& Top = Stack.Top();
        NSet.AddKey(Top.Val1);  NSet.AddKey(Top.Val2); Stack.Pop(); }
      TIntV NIdV; NSet.GetKeyV(NIdV);  NIdV.Sort();
      CnComV.Add(NIdV); } }
  void ExamineEdge(const int& NId1, const int& NId2) { }
  void TreeEdge(const int& NId1, const int& NId2) {
    ParentH.AddDat(NId2, NId1);
    Stack.Push(TIntPr(NId1, NId2)); }
  void BackEdge(const int& NId1, const int& NId2) {
    if (ParentH.IsKey(NId1) && ParentH.GetDat(NId1)!=NId2) {
      Stack.Push(TIntPr(NId1, NId2));
      VnLowH.GetDat(NId1).Val2 = TMath::Mn(VnLowH.GetDat(NId1).Val2, VnLowH.GetDat(NId2).Val1); } }
  void FwdEdge(const int& NId1, const int& NId2) { }
};

/////////////////////////////////////////////////
/// Strongly connected componetns Depht-First-Search visitor class.
template <class PGraph, bool OnlyCount = false>
class TSccVisitor {
public:
  PGraph Graph;
  THash<TInt, TIntPr> TmRtH;
  TSStack<TInt> Stack;
  TInt Time;
  TIntH SccCntH;
  TCnComV CnComV;
public:
  TSccVisitor(const PGraph& _Graph) :
      Graph(_Graph), TmRtH(Graph->GetNodes()), Stack(Graph->GetNodes()) { }
  void DiscoverNode(int NId) {
    Time++; TmRtH.AddDat(NId, TIntPr(-Time, NId)); // negative time -- node not yet in any SCC
    Stack.Push(NId); }
  void FinishNode(const int& NId) {
    typename PGraph::TObj::TNodeI NI = Graph->GetNI(NId);
    TIntPr& TmRtN = TmRtH.GetDat(NId);
    int W = -1, Cnt = 0;
    for (int i = 0; i < NI.GetOutDeg(); i++) {
      W = NI.GetOutNId(i);
      const TIntPr& TmRtW = TmRtH.GetDat(W);
      if (TmRtW.Val1 < 0) { // node not yet in any SCC
        TmRtN.Val2 = GetMinDiscTm(TmRtN.Val2, TmRtW.Val2); } }
    if (TmRtN.Val2 == NId) {
      if (! OnlyCount) { CnComV.Add(); }
      do { W = Stack.Top();  Stack.Pop();
      if (OnlyCount) { Cnt++; } else { CnComV.Last().Add(W); }
      TmRtH.GetDat(W).Val1 = abs(TmRtH.GetDat(W).Val1); // node is in SCC
      } while (W != NId);
      if (OnlyCount) { SccCntH.AddDat(Cnt) += 1; } } }
  void ExamineEdge(const int& NId1, const int& NId2) { }
  void TreeEdge(const int& NId1, const int& NId2) { }
  void BackEdge(const int& NId1, const int& NId2) { }
  void FwdEdge(const int& NId1, const int& NId2) { }
  int GetMinDiscTm(const int& NId1, const int& NId2) const {
    return abs(TmRtH.GetDat(NId1).Val1) < abs(TmRtH.GetDat(NId2).Val1) ? NId1 : NId2; }
};

/////////////////////////////////////////////////
// Implementation
namespace TSnap {

template <class PGraph> 
void GetNodeWcc(const PGraph& Graph, const int& NId, TIntV& CnCom) {
  typename PGraph::TObj::TNodeI NI;
  THashSet<TInt> VisitedNId(Graph->GetNodes()+1);
  TSnapQueue<int> NIdQ(Graph->GetNodes()+1);
  VisitedNId.AddKey(NId);
  NIdQ.Push(NId);
  while (! NIdQ.Empty()) {
    const typename PGraph::TObj::TNodeI Node = Graph->GetNI(NIdQ.Top());  NIdQ.Pop();
    if (HasGraphFlag(typename PGraph::TObj, gfDirected)) {
      for (int e = 0; e < Node.GetInDeg(); e++) {
        const int InNId = Node.GetInNId(e);
        if (! VisitedNId.IsKey(InNId)) {
          NIdQ.Push(InNId);  VisitedNId.AddKey(InNId); }
      }
    }
    for (int e = 0; e < Node.GetOutDeg(); e++) {
      const int OutNId = Node.GetOutNId(e);
      if (! VisitedNId.IsKey(OutNId)) {
        NIdQ.Push(OutNId);  VisitedNId.AddKey(OutNId); }
    }
  }
  CnCom.Gen(VisitedNId.Len(), 0);
  for (int i = 0; i < VisitedNId.Len(); i++) {
    CnCom.Add(VisitedNId.GetKey(i));
  }
}

template <class PGraph> 
bool IsConnected(const PGraph& Graph) {
  return IsWeaklyConn(Graph);
}

template <class PGraph>
bool IsWeaklyConn(const PGraph& Graph) {
  THashSet<TInt> VisitedNId(Graph->GetNodes());
  TSnapQueue<int> NIdQ(Graph->GetNodes()+1);
  typename PGraph::TObj::TNodeI NI;
  // the rest of the nodes
  NIdQ.Push(Graph->BegNI().GetId());
  while (! NIdQ.Empty()) {
    const typename PGraph::TObj::TNodeI Node = Graph->GetNI(NIdQ.Top());  NIdQ.Pop();
    if (HasGraphFlag(typename PGraph::TObj, gfDirected)) {
      for (int e = 0; e < Node.GetInDeg(); e++) {
        const int InNId = Node.GetInNId(e);
        if (! VisitedNId.IsKey(InNId)) { NIdQ.Push(InNId);  VisitedNId.AddKey(InNId); }
      }
    }
    for (int e = 0; e < Node.GetOutDeg(); e++) {
      const int OutNId = Node.GetOutNId(e);
      if (! VisitedNId.IsKey(OutNId)) { NIdQ.Push(OutNId);  VisitedNId.AddKey(OutNId); }
    }
  }
  if (VisitedNId.Len() < Graph->GetNodes()) { return false; }
  return true;
}

template <class PGraph>
void GetWccSzCnt(const PGraph& Graph, TIntPrV& WccSzCnt) {
  THashSet<TInt> VisitedNId(Graph->GetNodes());
  TIntH SzToCntH;
  TSnapQueue<int> NIdQ(Graph->GetNodes()+1);
  typename PGraph::TObj::TNodeI NI;
  int Cnt = 0;
  // zero degree nodes
  for (NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    if (NI.GetDeg() == 0) { Cnt++;  VisitedNId.AddKey(NI.GetId()); }
  }
  if (Cnt > 0) SzToCntH.AddDat(1, Cnt);
  // the rest of the nodes
  for (NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    if (! VisitedNId.IsKey(NI.GetId())) {
      VisitedNId.AddKey(NI.GetId());
      NIdQ.Clr(false);  NIdQ.Push(NI.GetId());
      Cnt = 0;
      while (! NIdQ.Empty()) {
        const typename PGraph::TObj::TNodeI Node = Graph->GetNI(NIdQ.Top());  NIdQ.Pop();
        if (HasGraphFlag(typename PGraph::TObj, gfDirected)) {
          for (int e = 0; e < Node.GetInDeg(); e++) {
            const int InNId = Node.GetInNId(e);
            if (! VisitedNId.IsKey(InNId)) { NIdQ.Push(InNId);  VisitedNId.AddKey(InNId); }
          }
        }
        for (int e = 0; e < Node.GetOutDeg(); e++) {
          const int OutNId = Node.GetOutNId(e);
          if (! VisitedNId.IsKey(OutNId)) { NIdQ.Push(OutNId);  VisitedNId.AddKey(OutNId); }
        }
        Cnt++;
      }
      SzToCntH.AddDat(Cnt) += 1;
    }
  }
  SzToCntH.GetKeyDatPrV(WccSzCnt);
  WccSzCnt.Sort(true);
}

template <class PGraph>
void GetWccs(const PGraph& Graph, TCnComV& CnComV) {
  typename PGraph::TObj::TNodeI NI;
  THashSet<TInt> VisitedNId(Graph->GetNodes()+1);
  TSnapQueue<int> NIdQ(Graph->GetNodes()+1);
  TIntV CcNIdV;
  CnComV.Clr();  CcNIdV.Gen(1);
  // zero degree nodes
  for (NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    if (NI.GetDeg() == 0) {
      const int NId = NI.GetId();
      VisitedNId.AddKey(NId);
      CcNIdV[0] = NId;  CnComV.Add(CcNIdV);
    }
  }
  // the rest of the nodes
  for (NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    const int NId = NI.GetId();
    if (! VisitedNId.IsKey(NId)) {
      VisitedNId.AddKey(NId);
      NIdQ.Clr(false);  NIdQ.Push(NId);
      CcNIdV.Clr();  CcNIdV.Add(NId);
      while (! NIdQ.Empty()) {
        const typename PGraph::TObj::TNodeI Node = Graph->GetNI(NIdQ.Top());  NIdQ.Pop();
        if (HasGraphFlag(typename PGraph::TObj, gfDirected)) {
          for (int e = 0; e < Node.GetInDeg(); e++) {
            const int InNId = Node.GetInNId(e);
            if (! VisitedNId.IsKey(InNId)) {
              NIdQ.Push(InNId);  VisitedNId.AddKey(InNId);  CcNIdV.Add(InNId); }
          }
        }
        for (int e = 0; e < Node.GetOutDeg(); e++) {
          const int OutNId = Node.GetOutNId(e);
          if (! VisitedNId.IsKey(OutNId)) {
            NIdQ.Push(OutNId);  VisitedNId.AddKey(OutNId);  CcNIdV.Add(OutNId); }
        }
      }
      CcNIdV.Sort(true);
      CnComV.Add(TCnCom(CcNIdV)); // add wcc comoponent
    }
  }
  CnComV.Sort(false);
}

template <class PGraph>
void GetSccSzCnt(const PGraph& Graph, TIntPrV& SccSzCnt) {
  TSccVisitor<PGraph, true> Visitor(Graph);
  TCnCom::GetDfsVisitor(Graph, Visitor);
  Visitor.SccCntH.GetKeyDatPrV(SccSzCnt);
  SccSzCnt.Sort(true);
}

template <class PGraph>
void GetSccs(const PGraph& Graph, TCnComV& CnComV) {
  TSccVisitor<PGraph, false> Visitor(Graph);
  TCnCom::GetDfsVisitor(Graph, Visitor);
  CnComV = Visitor.CnComV;
}

template <class PGraph> 
double GetMxWccSz(const PGraph& Graph) {
  TCnComV CnComV;
  GetWccs(Graph, CnComV);
  if (Graph->GetNodes() == 0) { return 0; }
  else { return CnComV[0].Len() / double(Graph->GetNodes()); }
}

template <class PGraph>
PGraph GetMxWcc(const PGraph& Graph) {
  TCnComV CnComV;
  GetWccs(Graph, CnComV);
  if (CnComV.Empty()) { return PGraph::TObj::New(); }
  int CcId = 0, MxSz = 0;
  for (int i = 0; i < CnComV.Len(); i++) {
    if (MxSz < CnComV[i].Len()) {
      MxSz=CnComV[i].Len();  CcId=i; }
  }
  if (CnComV[CcId].Len()==Graph->GetNodes()) { 
    return Graph; }
  else { 
    return TSnap::GetSubGraph(Graph, CnComV[CcId]()); 
  }
}

template <class PGraph>
PGraph GetMxScc(const PGraph& Graph) {
  TCnComV CnComV;
  GetSccs(Graph, CnComV);
  if (CnComV.Empty()) { return PGraph::TObj::New(); }
  int CcId = 0, MxSz = 0;
  for (int i = 0; i < CnComV.Len(); i++) {
    if (MxSz < CnComV[i].Len()) {
      MxSz=CnComV[i].Len();  CcId=i; }
  }
  if (CnComV[CcId].Len()==Graph->GetNodes()) { 
    return Graph; }
  else { 
    return TSnap::GetSubGraph(Graph, CnComV[CcId]()); 
  }
}

template <class PGraph>
PGraph GetMxBiCon(const PGraph& Graph) {
  TCnComV CnComV;
  GetBiCon(TSnap::ConvertGraph<PUNGraph, PGraph>(Graph), CnComV);
  if (CnComV.Empty()) { return PGraph::TObj::New(); }
  int CcId = 0, MxSz = 0;
  for (int i = 0; i < CnComV.Len(); i++) {
    if (MxSz < CnComV[i].Len()) {
      MxSz=CnComV[i].Len();  CcId=i; }
  }
  if (CnComV[CcId].Len()==Graph->GetNodes()) { 
    return Graph; }
  else { 
    return TSnap::GetSubGraph(Graph, CnComV[CcId]()); 
  }
}

} // namespace TSnap
