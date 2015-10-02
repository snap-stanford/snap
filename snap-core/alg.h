namespace TSnap {

/////////////////////////////////////////////////
// Node Degrees

/// Returns the number of nodes with in-degree NodeInDeg.
template <class PGraph> int CntInDegNodes(const PGraph& Graph, const int& NodeInDeg);
/// Returns the number of nodes with out-degree NodeOutDeg.
template <class PGraph> int CntOutDegNodes(const PGraph& Graph, const int& NodeOutDeg);
/// Returns the number of nodes with degree NodeDeg.
template <class PGraph> int CntDegNodes(const PGraph& Graph, const int& NodeDeg);
/// Returns the number of nodes with degree greater than 0.
template <class PGraph> int CntNonZNodes(const PGraph& Graph);
/// Returns the number of nodes in NodeSet that have an edge to the node NId.
template <class PGraph> int CntEdgesToSet(const PGraph& Graph, const int& NId, const TIntSet& NodeSet);

/// Returns a randomly chosen node from all the nodes with the maximum degree.
template <class PGraph> int GetMxDegNId(const PGraph& Graph);
/// Returns a randomly chosen node from all the nodes with the maximum in-degree.
template <class PGraph> int GetMxInDegNId(const PGraph& Graph);
/// Returns a randomly chosen node from all the nodes with the maximum out-degree.
template <class PGraph> int GetMxOutDegNId(const PGraph& Graph);

// degree histograms
/// Returns an in-degree histogram: a set of pairs <code>(in-degree, number of nodes of such in-degree)</code>
template <class PGraph> void GetInDegCnt(const PGraph& Graph, TIntPrV& DegToCntV);
/// Returns an in-degree histogram: a set of pairs <code>(in-degree, number of nodes of such in-degree)</code>
template <class PGraph> void GetInDegCnt(const PGraph& Graph, TFltPrV& DegToCntV);
/// Returns an out-degree histogram: a set of pairs <code>(out-degree, number of nodes of such out-degree)</code>
template <class PGraph> void GetOutDegCnt(const PGraph& Graph, TIntPrV& DegToCntV);
/// Returns an out-degree histogram: a set of pairs <code>(out-degree, number of nodes of such out-degree)</code>
template <class PGraph> void GetOutDegCnt(const PGraph& Graph, TFltPrV& DegToCntV);
/// Returns a degree histogram: a set of pairs <code>(degree, number of nodes of such degree)</code>
template <class PGraph> void GetDegCnt(const PGraph& Graph, TIntPrV& DegToCntV);
/// Returns a degree histogram: a set of pairs <code>(degree, number of nodes of such degree)</code>
template <class PGraph> void GetDegCnt(const PGraph& Graph, TFltPrV& DegToCntV);
/// Returns a degree sequence vector.
template <class PGraph> void GetDegSeqV(const PGraph& Graph, TIntV& DegV);
/// Returns an in- and out-degree sequence vectors.
template <class PGraph> void GetDegSeqV(const PGraph& Graph, TIntV& InDegV, TIntV& OutDegV);

/// Returns a vector of pairs <code>(node id, node in-degree)</code>
template <class PGraph> void GetNodeInDegV(const PGraph& Graph, TIntPrV& NIdInDegV);
/// Returns a vector of pairs <code>(node id, node out-degree)</code>
template <class PGraph> void GetNodeOutDegV(const PGraph& Graph, TIntPrV& NIdOutDegV);

/// Counts unique undirected edges in the graph \c Graph. Nodes <code>(u,v)<\code> (where <code>u!=v</code>) are connected via an undirected edge if there exists an edge in either direction <code>(u,v)</code> or <code>(v,u)</code>.
template <class PGraph> int CntUniqUndirEdges(const PGraph& Graph);
/// Counts unique directed edges in the graph \c Graph. Nodes <code>(u,v)<\code> (where <code>u!=v</code>) are connected via an directted edge if there exists a directed edge from node \c u to node \c v.
template <class PGraph> int CntUniqDirEdges(const PGraph& Graph);
/// Counts unique bidirectional edges in the graph \c Graph. Edge is bidirectional is there exist directed edges in both directions: <code>(u,v)</code> and <code>(v,u)</code>
template <class PGraph> int CntUniqBiDirEdges(const PGraph& Graph);
/// Counts the number fo of self-edges in a graph. Edge <code>(u,u)</code> is a self-edge.
template <class PGraph> int CntSelfEdges(const PGraph& Graph);

/////////////////////////////////////////////////
// Manipulation
/// Returs an undirected version of the graph. For every edge <code>(u,v)</code> an edge <code>(v,u)</code> is added (if it does not yet exist).
template <class PGraph> PGraph GetUnDir(const PGraph& Graph);
/// Makes the graph undirected. For every edge <code>(u,v)</code> an edge <code>(v,u)</code> is added (if it does not yet exist).
template <class PGraph> void MakeUnDir(const PGraph& Graph);
/// Adds a self-edge to every node in the graph.
template <class PGraph> void AddSelfEdges(const PGraph& Graph);
/// Removes all the self-edges from the graph.
template <class PGraph> void DelSelfEdges(const PGraph& Graph);

//TODO Implement:
//template <class PGraph> void DelBiDirEdges(const PGraph& Graph);

/// Removes nodes with ids stored in \c NIdV from the graph.
template <class PGraph> void DelNodes(PGraph& Graph, const TIntV& NIdV);
/// Removes all the zero-degree nodes, that isolated nodes, from the graph.
template <class PGraph> void DelZeroDegNodes(PGraph& Graph);
/// Removes all the node of out-degree \c OutDegK and all the nodes of in-degree \c InDegK from the graph.
template <class PGraph> void DelDegKNodes(PGraph& Graph, const int& OutDegK, const int& InDegK);

/////////////////////////////////////////////////
// Tree Routines
template <class PGraph> bool IsTree(const PGraph& Graph, int& RootNIdX);
template <class PGraph> int GetTreeRootNId(const PGraph& Graph) { int RootNId; bool Tree; Tree = IsTree(Graph, RootNId);  Assert(Tree);  return RootNId; }
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
int CntEdgesToSet(const PGraph& Graph, const int& NId, const TIntSet& NodeSet) {
  if (! Graph->IsNode(NId)) { return 0; }
  const bool IsDir = Graph->HasFlag(gfDirected);
  const typename PGraph::TObj::TNodeI NI = Graph->GetNI(NId);
  if (! IsDir) {
    int EdgesToSet = 0;
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      if (NodeSet.IsKey(NI.GetOutNId(e))) { EdgesToSet++; } }
    return EdgesToSet;
  } else {
    TIntSet Set(NI.GetDeg());
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      if (NodeSet.IsKey(NI.GetOutNId(e))) { Set.AddKey(NI.GetOutNId(e)); } }
    for (int e = 0; e < NI.GetInDeg(); e++) {
      if (NodeSet.IsKey(NI.GetInNId(e))) { Set.AddKey(NI.GetInNId(e)); } }
    return Set.Len();
  }
}

template <class PGraph>
int GetMxDegNId(const PGraph& Graph) {
  TIntV MxDegV;
  int MxDeg=-1;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    if (MxDeg < NI.GetDeg()) { MxDegV.Clr(); MxDeg = NI.GetDeg(); }
    if (MxDeg == NI.GetDeg()) { MxDegV.Add(NI.GetId()); }
  }
  EAssertR(! MxDegV.Empty(), "Input graph is empty!");
  return MxDegV[TInt::Rnd.GetUniDevInt(MxDegV.Len())];
}

template <class PGraph>
int GetMxInDegNId(const PGraph& Graph) {
  TIntV MxDegV;
  int MxDeg=-1;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    if (MxDeg < NI.GetInDeg()) { MxDegV.Clr(); MxDeg = NI.GetInDeg(); }
    if (MxDeg == NI.GetInDeg()) { MxDegV.Add(NI.GetId()); }
  }
  EAssertR(! MxDegV.Empty(), "Input graph is empty!");
  return MxDegV[TInt::Rnd.GetUniDevInt(MxDegV.Len())];
}

template <class PGraph>
int GetMxOutDegNId(const PGraph& Graph) {
  TIntV MxDegV;
  int MxDeg=-1;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    if (MxDeg < NI.GetOutDeg()) { MxDegV.Clr(); MxDeg = NI.GetOutDeg(); }
    if (MxDeg == NI.GetOutDeg()) { MxDegV.Add(NI.GetId()); }
  }
  EAssertR(! MxDegV.Empty(), "Input graph is empty!");
  return MxDegV[TInt::Rnd.GetUniDevInt(MxDegV.Len())];
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
void GetNodeInDegV(const PGraph& Graph, TIntPrV& NIdInDegV) {
  NIdInDegV.Reserve(Graph->GetNodes(), 0);
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NIdInDegV.Add(TIntPr(NI.GetId(), NI.GetInDeg()));
  }
}

template <class PGraph>
void GetNodeOutDegV(const PGraph& Graph, TIntPrV& NIdOutDegV) {
  NIdOutDegV.Reserve(Graph->GetNodes(), 0);
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NIdOutDegV.Add(TIntPr(NI.GetId(), NI.GetOutDeg()));
  }
}

template <class PGraph>
int CntUniqUndirEdges(const PGraph& Graph) {
  TIntSet NbrSet;
  TIntSet SelfNbrSet;
  int Cnt = 0;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NbrSet.Clr(false);
    for (int e = 0; e < NI.GetDeg(); e++) { // unique neighbors of a node
      const int NbrId = NI.GetNbrNId(e);
      if (NbrId == NI.GetId()) { // remember self-edges
        SelfNbrSet.AddKey(NbrId);
      } else {
        NbrSet.AddKey(NbrId);
      }
    }
    Cnt += NbrSet.Len();
  }
  // OP RS 2014/06/11 self-edges are currently not used 
  //return Cnt / 2 + SelfNbrSet.Len();
  return Cnt / 2;
}

template <class PGraph>
int CntUniqDirEdges(const PGraph& Graph) {
  TIntSet NbrSet;
  int Cnt = 0;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NbrSet.Clr(false);
    for (int e = 0; e < NI.GetOutDeg(); e++) { // unique out-neighbors of a node
      if (NI.GetOutNId(e) != NI.GetId()) { // skip self-edges
        NbrSet.AddKey(NI.GetOutNId(e)); }
    }
    Cnt += NbrSet.Len();
  }
  return Cnt;
}

template <class PGraph>
int CntUniqBiDirEdges(const PGraph& Graph) {
  if (! Graph->HasFlag(gfDirected)) { // graph is undirected
    return CntUniqUndirEdges(Graph);  // then every edge is bi-directional
  }
  TIntSet NbrSet;
  int Cnt = 0;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    const int SrcId = NI.GetId();
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      const int DstId = NI.GetOutNId(e);
      if (DstId <= SrcId) { continue; } // count each un-dir edge only once
      if (Graph->IsEdge(DstId, SrcId)) { Cnt++; }
    }
  }
  return Cnt;
}

template <class PGraph>
int CntSelfEdges(const PGraph& Graph) {
  int Cnt = 0;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      if (NI.GetId() == NI.GetOutNId(e)) { Cnt++; }
    }
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
  if (Graph->GetNodes() == 1 && Graph->GetEdges() == 0) {
    RootNId = Graph->BegNI().GetId();
    return true;
  }
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
  if (NZeroOutDeg==1) {
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
