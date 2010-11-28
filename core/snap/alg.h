namespace TSnap {

/////////////////////////////////////////////////
// Node Degrees
template <class PGraph> int CntInDegNodes(const PGraph& Graph, const int& NodeInDeg);
template <class PGraph> int CntOutDegNodes(const PGraph& Graph, const int& NodeOutDeg);
template <class PGraph> int CntDegNodes(const PGraph& Graph, const int& NodeDeg);
template <class PGraph> int CntNonZNodes(const PGraph& Graph);

template <class PGraph> void GetInDegCnt(const PGraph& Graph, TIntPrV& DegToCntV);
template <class PGraph> void GetInDegCnt(const PGraph& Graph, TFltPrV& DegToCntV);
template <class PGraph> void GetOutDegCnt(const PGraph& Graph, TIntPrV& DegToCntV);
template <class PGraph> void GetOutDegCnt(const PGraph& Graph, TFltPrV& DegToCntV);
template <class PGraph> void GetDegCnt(const PGraph& Graph, TIntPrV& DegToCntV);
template <class PGraph> void GetDegCnt(const PGraph& Graph, TFltPrV& DegToCntV);
template <class PGraph> void GetDegSeqV(const PGraph& Graph, TIntV& DegV);
template <class PGraph> void GetDegSeqV(const PGraph& Graph, TIntV& InDegV, TIntV& OutDegV);

template <class PGraph> int CntUniqUndirEdges(const PGraph& Graph);
template <class PGraph> int CntUniqDirEdges(const PGraph& Graph);
template <class PGraph> int CntUniqBiDirEdges(const PGraph& Graph);
template <class PGraph> int CntSelfEdges(const PGraph& Graph);

/////////////////////////////////////////////////
// Manipulation
template <class PGraph> PGraph GetUnDir(const PGraph& Graph);
template <class PGraph> void MakeUnDir(const PGraph& Graph);
template <class PGraph> void AddSelfEdges(const PGraph& Graph);
template <class PGraph> void DelSelfEdges(const PGraph& Graph);
template <class PGraph> void DelBiDirEdges(const PGraph& Graph);

template <class PGraph> void DelNodes(PGraph& Graph, const TIntV& NIdV);
template <class PGraph> void DelZeroDegNodes(PGraph& Graph);
template <class PGraph> void DelDegKNodes(PGraph& Graph, const int& OutDegK, const int& InDegK);

/////////////////////////////////////////////////
// Tree Routines
template <class PGraph> bool IsTree(const PGraph& Graph, int& RootNId);
template <class PGraph> int GetTreeRootNId(const PGraph& Graph) { int RootNId; Assert(IsTree(Graph, RootNId));  return RootNId; }
template <class PGraph> void GetTreeSig(const PGraph& Graph, const int& RootNId, TIntV& Sig);
template <class PGraph> void GetTreeSig(const PGraph& Graph, const int& RootNId, TIntV& Sig, TIntPrV& NodeMap);

/////////////////////////////////////////////////
// Implementation
template <class PGraph>
int CntInDegNodes(const PGraph& Graph, const int& NodeInDeg) {
  int Cnt = 0;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    if (NI.GetInDeg() == NodeInDeg) Cnt++;
  }
  return Cnt;
}

template <class PGraph>
int CntOutDegNodes(const PGraph& Graph, const int& NodeOutDeg) {
  int Cnt = 0;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    if (NI.GetOutDeg() == NodeOutDeg) Cnt++;
  }
  return Cnt;
}

template <class PGraph>
int CntDegNodes(const PGraph& Graph, const int& NodeDeg) {
  int Cnt = 0;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    if (NI.GetDeg() == NodeDeg) Cnt++;
  }
  return Cnt;
}

template <class PGraph>
int CntNonZNodes(const PGraph& Graph) {
  int Cnt = 0;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    if (NI.GetDeg() > 0) Cnt++;
  }
  return Cnt;
}

template <class PGraph>
void GetInDegCnt(const PGraph& Graph, TIntPrV& DegToCntV) {
  TIntH DegToCntH;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    DegToCntH.AddDat(NI.GetInDeg())++; }
  DegToCntV.Gen(DegToCntH.Len(), 0);
  for (int i = 0; i < DegToCntH.Len(); i++) {
    DegToCntV.Add(TIntPr(DegToCntH.GetKey(i), DegToCntH[i])); }
  DegToCntV.Sort();
}

template <class PGraph>
void GetInDegCnt(const PGraph& Graph, TFltPrV& DegToCntV) {
  TIntH DegToCntH;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    DegToCntH.AddDat(NI.GetInDeg())++; }
  DegToCntV.Gen(DegToCntH.Len(), 0);
  for (int i = 0; i < DegToCntH.Len(); i++) {
    DegToCntV.Add(TFltPr(DegToCntH.GetKey(i).Val, DegToCntH[i].Val)); }
  DegToCntV.Sort();
}

template <class PGraph>
void GetOutDegCnt(const PGraph& Graph, TIntPrV& DegToCntV) {
  TIntH DegToCntH;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    DegToCntH.AddDat(NI.GetOutDeg())++; }
  DegToCntV.Gen(DegToCntH.Len(), 0);
  for (int i = 0; i < DegToCntH.Len(); i++) {
    DegToCntV.Add(TIntPr(DegToCntH.GetKey(i), DegToCntH[i])); }
  DegToCntV.Sort();
}

template <class PGraph>
void GetOutDegCnt(const PGraph& Graph, TFltPrV& DegToCntV) {
  TIntH DegToCntH;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    DegToCntH.AddDat(NI.GetOutDeg())++; }
  DegToCntV.Gen(DegToCntH.Len(), 0);
  for (int i = 0; i < DegToCntH.Len(); i++) {
    DegToCntV.Add(TFltPr(DegToCntH.GetKey(i).Val, DegToCntH[i].Val)); }
  DegToCntV.Sort();
}

template <class PGraph>
void GetDegCnt(const PGraph& Graph, TIntPrV& DegToCntV) {
  TIntH DegToCntH;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    DegToCntH.AddDat(NI.GetDeg())++; }
  DegToCntV.Gen(DegToCntH.Len(), 0);
  for (int i = 0; i < DegToCntH.Len(); i++) {
    DegToCntV.Add(TIntPr(DegToCntH.GetKey(i), DegToCntH[i])); }
  DegToCntV.Sort();
}

template <class PGraph>
void GetDegCnt(const PGraph& Graph, TFltPrV& DegToCntV) {
  TIntH DegToCntH;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    DegToCntH.AddDat(NI.GetDeg())++; }
  DegToCntV.Gen(DegToCntH.Len(), 0);
  for (int i = 0; i < DegToCntH.Len(); i++) {
    DegToCntV.Add(TFltPr(DegToCntH.GetKey(i).Val, DegToCntH[i].Val)); }
  DegToCntV.Sort();
}

template <class PGraph> 
void GetDegSeqV(const PGraph& Graph, TIntV& DegV) {
  DegV.Gen(Graph->GetNodes(), 0);
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    DegV.Add(NI.GetDeg()); 
  }
}

template <class PGraph> 
void GetDegSeqV(const PGraph& Graph, TIntV& InDegV, TIntV& OutDegV) {
  InDegV.Gen(Graph->GetNodes(), 0);
  OutDegV.Gen(Graph->GetNodes(), 0);
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    InDegV.Add(NI.GetInDeg()); 
    OutDegV.Add(NI.GetOutDeg());
  }
}

template <class PGraph>
int GetUniqUndirEdges(const PGraph& Graph) {
  TIntSet NbhSet;
  int Cnt = 0;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NbhSet.Clr(false);
    for (int e = 0; e < NI.GetDeg(); e++) { // unqiue neighbors of a node
      NbhSet.AddKey(NI.GetNbhNId(e));
    }
    Cnt += NbhSet.Len();
  }
  return Cnt / 2;
}

template <class PGraph>
int GetUniqDirEdges(const PGraph& Graph) {
  TIntSet NbhSet;
  int Cnt = 0;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NbhSet.Clr(false);
    for (int e = 0; e < NI.GetOutDeg(); e++) { // unqiue out-neighbors of a node
      NbhSet.AddKey(NI.GetOutNId(e));
    }
    Cnt += NbhSet.Len();
  }
  return Cnt;
}

template <class PGraph>
int GetUniqBiDirEdges(const PGraph& Graph) {
  if (! Graph->HasFlag(gfDirected)) { // graph is undirected
    return GetUniqUndirEdges(Graph);  // then every edge is bi-directional
  }
  TIntSet NbhSet;
  int Cnt = 0;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    const int SrcId = NI.GetId();
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      const int DstId = NI.GetOutNId(e);
      if (DstId <= SrcId) { continue; } // count each un-dir edge only once
      if (Graph->IsEdge(DstId, SrcId)) { Cnt++; }
    }
  }
  return 2*Cnt;
}

template <class PGraph> 
int CntSelfEdges(const PGraph& Graph) {
  int Cnt = 0;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    if (Graph->IsEdge(NI.GetId(), NI.GetId())) { Cnt++; }
  }
  return Cnt;
}

template <class PGraph>
PGraph GetUnDir(const PGraph& Graph) {
  PGraph NewGraphPt = PGraph::New();
  *NewGraphPt = *Graph;
  MakeUnDir(NewGraphPt);
  return NewGraphPt;
}

template <class PGraph>
void MakeUnDir(const PGraph& Graph) {
  CAssert(HasGraphFlag(typename PGraph::TObj, gfDirected)); // graph has to be directed
  TIntPrV EdgeV;
  for (typename PGraph::TObj::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    const int SrcNId = EI.GetSrcNId();
    const int DstNId = EI.GetDstNId();
    if (! Graph->IsEdge(DstNId, SrcNId)) {
      EdgeV.Add(TIntPr(DstNId, SrcNId));
    }
  }
  for (int i = 0; i < EdgeV.Len(); i++) {
    Graph->AddEdge(EdgeV[i].Val1, EdgeV[i].Val2);
  }
}

template <class PGraph>
void AddSelfEdges(const PGraph& Graph) {
  TIntV EdgeV;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    const int NId = NI.GetId();
    if (! Graph->IsEdge(NId, NId)) {
      EdgeV.Add(NId);
    }
  }
  for (int i = 0; i < EdgeV.Len(); i++) {
    Graph->AddEdge(EdgeV[i], EdgeV[i]);
  }
}

namespace TSnapDetail {

template <class PGraph, bool IsMultiGraph>
struct TDelSelfEdges { // not a multigraph graph
  static void Do(const PGraph& Graph) {
    TIntV EdgeV;
    // node graph, no explicit edge ids
    for (typename PGraph::TObj::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
      if (EI.GetSrcNId() == EI.GetDstNId()) {
        EdgeV.Add(EI.GetSrcNId());
      }
    }
    for (int i = 0; i < EdgeV.Len(); i++) {
      Graph->DelEdge(EdgeV[i], EdgeV[i]);
    }
  }
};

template <class PGraph>
struct TDelSelfEdges<PGraph, true> { // mutligraph specialization
  static void Do(const PGraph& Graph) {
    TIntV EdgeV;
    for (typename PGraph::TObj::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
      if (EI.GetSrcNId() == EI.GetDstNId()) {
        IAssert(EI.GetId() >= 0); // real edge id
        EdgeV.Add(EI.GetId());
      }
    }
    for (int i = 0; i < EdgeV.Len(); i++) { // delete all edges between a pair of nodes
      Graph->DelEdge(EdgeV[i]);
    }
  }
};

} // namespace TSnapDetail

template <class PGraph>
void DelSelfEdges(const PGraph& Graph) {
  TSnapDetail::TDelSelfEdges<PGraph, HasGraphFlag(typename PGraph::TObj, gfMultiGraph)>
    ::Do(Graph);
}

template <class PGraph>
void DelNodes(PGraph& Graph, const TIntV& NIdV) {
  for (int n = 0; n < NIdV.Len(); n++) {
    Graph->DelNode(NIdV[n]);
  }
}

template <class PGraph>
void DelZeroDegNodes(PGraph& Graph) {
  TIntV DelNIdV;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    if (NI.GetDeg() == 0) {
      DelNIdV.Add(NI.GetId());
    }
  }
  for (int i = 0; i < DelNIdV.Len(); i++) {
    Graph->DelNode(DelNIdV[i]);
  }
}

template <class PGraph>
void DelDegKNodes(PGraph& Graph, const int& OutDegK, const int& InDegK) {
  TIntV DelNIdV;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    if (NI.GetOutDeg() == OutDegK || NI.GetInDeg() == InDegK) {
      DelNIdV.Add(NI.GetId());
    }
  }
  for (int i = 0; i < DelNIdV.Len(); i++) {
    Graph->DelNode(DelNIdV[i]);
  }
}

// tree has directed edges -- so root is a well-defined
// children point to a parent
template <class PGraph>
bool IsTree(const PGraph& Graph, int& RootNId) {
  RootNId = -1;
  if (Graph->GetNodes() != Graph->GetEdges()+1) { return false; }
  int NZeroOutDeg = 0;
  int ZeroOutDegN = -1;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    if (NI.GetOutDeg() == 0) {
      ZeroOutDegN = NI.GetId();  NZeroOutDeg++;
    }
    if (NI.GetDeg() == 0) { return false; } // isolated nodes
  }
  if (ZeroOutDegN==1) {
    if (! TSnap::IsConnected(Graph)) { return false; }
    RootNId = ZeroOutDegN;  return true;
  }
  return false;
}

// tree signature -- for each level we sort the node in-degrees and concatenate them into a vector
template <class PGraph>
void GetTreeSig(const PGraph& Graph, const int& RootNId, TIntV& Sig) {
  CAssert(HasGraphFlag(typename PGraph::TObj, gfDirected));
  Sig.Gen(Graph->GetNodes(), 0);
  TSnapQueue<int> NIdQ(Graph->GetNodes());
  NIdQ.Push(RootNId);
  int LastPos = 0, NodeCnt = 1;
  while (! NIdQ.Empty()) {
    const typename PGraph::TObj::TNodeI Node = Graph->GetNI(NIdQ.Top());  NIdQ.Pop();
    IAssert(Node.GetInDeg()==0 || Node.GetOutDeg()==0); // child points or is-pointed to by the parent
    if (Node.GetInDeg() != 0) {
      for (int e = 0; e < Node.GetInDeg(); e++) {
        NIdQ.Push(Node.GetInNId(e)); }
    } else if (Node.GetOutDeg() != 0) {
      for (int e = 0; e < Node.GetOutDeg(); e++) {
        NIdQ.Push(Node.GetOutNId(e)); }
    }
    Sig.Add(Node.GetInDeg());
    if (--NodeCnt == 0) {
      for (int i = LastPos; i < Sig.Len(); i++) NodeCnt += Sig[i];
      Sig.QSort(LastPos, Sig.Len()-1, false);
      LastPos = Sig.Len();
    }
  }
}

// tree signature -- for each level we sort the node in-degrees and concatenate them into a vector
template <class PGraph>
void GetTreeSig(const PGraph& Graph, const int& RootNId, TIntV& Sig, TIntPrV& NodeMap) {
  CAssert(HasGraphFlag(typename PGraph::TObj, gfDirected));
  NodeMap.Gen(Graph->GetNodes(), 0);
  Sig.Gen(Graph->GetNodes(), 0);
  TSnapQueue<int> NIdQ(Graph->GetNodes());
  NIdQ.Push(RootNId);
  int LastPos = 0, NodeCnt = 1;
  while (! NIdQ.Empty()) {
    const typename PGraph::TObj::TNodeI Node = Graph->GetNI(NIdQ.Top());  NIdQ.Pop();
    IAssert(Node.GetInDeg()==0 || Node.GetOutDeg()==0); // child points or is-pointed to by the parent
    if (Node.GetInDeg() != 0) {
      for (int e = 0; e < Node.GetInDeg(); e++) {
        NIdQ.Push(Node.GetInNId(e)); }
      NodeMap.Add(TIntPr(Node.GetInDeg(), Node.GetId()));
    } else if (Node.GetOutDeg() != 0) {
      for (int e = 0; e < Node.GetOutDeg(); e++) {
        NIdQ.Push(Node.GetOutNId(e)); }
      NodeMap.Add(TIntPr(Node.GetOutDeg(), Node.GetId()));
    }
    if (--NodeCnt == 0) {
      for (int i = LastPos; i < NodeMap.Len(); i++) {
        NodeCnt += NodeMap[i].Val1; }
      NodeMap.QSort(LastPos, NodeMap.Len()-1, false);
      LastPos = NodeMap.Len();
    }
  }
  for (int i = 0; i < NodeMap.Len(); i++) {
    Sig.Add(NodeMap[i].Val1); // degree dignature
    NodeMap[i].Val1 = NodeMap[i].Val2;
    NodeMap[i].Val2 = i;
  }
}

}; // namespace TSnap
