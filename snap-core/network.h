#ifndef NETWORK_H
#define NETWORK_H

/////////////////////////////////////////////////
// Node Data
// TNodeData has to implement the following methods:
//  class TNodeData {
//  public:
//    TNodeData() { }
//    TNodeData(TSIn& SIn) { }
//    void Save(TSOut& SOut) const { }
//  };

//#//////////////////////////////////////////////
/// Node Network (directed graph, TNGraph with data on nodes only).
template <class TNodeData>
class TNodeNet {
public:
  typedef TNodeData TNodeDat;
  typedef TNodeNet<TNodeData> TNet;
  typedef TPt<TNet> PNet;
public:
  class TNode {
  private:
    TInt Id;
    TNodeData NodeDat;
    TIntV InNIdV, OutNIdV;
  public:
    TNode() : Id(-1), NodeDat(), InNIdV(), OutNIdV() { }
    TNode(const int& NId) : Id(NId), NodeDat(), InNIdV(), OutNIdV() { }
    TNode(const int& NId, const TNodeData& NodeData) : Id(NId), NodeDat(NodeData), InNIdV(), OutNIdV() { }
    TNode(const TNode& Node) : Id(Node.Id), NodeDat(Node.NodeDat), InNIdV(Node.InNIdV), OutNIdV(Node.OutNIdV) { }
    TNode(TSIn& SIn) : Id(SIn), NodeDat(SIn), InNIdV(SIn), OutNIdV(SIn) { }
    void Save(TSOut& SOut) const { Id.Save(SOut);  NodeDat.Save(SOut);  InNIdV.Save(SOut);  OutNIdV.Save(SOut); }
    int GetId() const { return Id; }
    int GetDeg() const { return GetInDeg() + GetOutDeg(); }
    int GetInDeg() const { return InNIdV.Len(); }
    int GetOutDeg() const { return OutNIdV.Len(); }
    const TNodeData& GetDat() const { return NodeDat; }
    TNodeData& GetDat() { return NodeDat; }
    int GetInNId(const int& NodeN) const { return InNIdV[NodeN]; }
    int GetOutNId(const int& NodeN) const { return OutNIdV[NodeN]; }
    int GetNbrNId(const int& NodeN) const { return NodeN<GetOutDeg() ? GetOutNId(NodeN):GetInNId(NodeN-GetOutDeg()); }
    bool IsInNId(const int& NId) const { return InNIdV.SearchBin(NId) != -1; }
    bool IsOutNId(const int& NId) const { return OutNIdV.SearchBin(NId) != -1; }
    bool IsNbrNId(const int& NId) const { return IsOutNId(NId) || IsInNId(NId); }
    void LoadShM(TShMIn& MStream) {
      Id = TInt(MStream);
      NodeDat = TNodeData(MStream);
      InNIdV.LoadShM(MStream);
      OutNIdV.LoadShM(MStream);
    }
    bool operator < (const TNode& Node) const { return NodeDat < Node.NodeDat; }
    friend class TNodeNet<TNodeData>;
  };

  /// Node iterator. Only forward iteration (operator++) is supported.
  class TNodeI {
  private:
    typedef typename THash<TInt, TNode>::TIter THashIter;
    THashIter NodeHI;
    TNodeNet *Net;
  public:
    TNodeI() : NodeHI(), Net(NULL) { }
    TNodeI(const THashIter& NodeHIter, const TNodeNet* NetPt) : NodeHI(NodeHIter), Net((TNodeNet *) NetPt) { }
    TNodeI(const TNodeI& NodeI) : NodeHI(NodeI.NodeHI), Net(NodeI.Net) { }
    TNodeI& operator = (const TNodeI& NodeI) { NodeHI=NodeI.NodeHI; Net=NodeI.Net; return *this; }
    /// Increment iterator.
    TNodeI& operator++ (int) { NodeHI++;  return *this; }
    bool operator < (const TNodeI& NodeI) const { return NodeHI < NodeI.NodeHI; }
    bool operator == (const TNodeI& NodeI) const { return NodeHI == NodeI.NodeHI; }

    /// Returns ID of the current node.
    int GetId() const { return NodeHI.GetDat().GetId(); }
    /// Returns degree of the current node.
    int GetDeg() const { return NodeHI.GetDat().GetDeg(); }
    /// Returns in-degree of the current node.
    int GetInDeg() const { return NodeHI.GetDat().GetInDeg(); }
    /// Returns out-degree of the current node.
    int GetOutDeg() const { return NodeHI.GetDat().GetOutDeg(); }
    /// Returns ID of NodeN-th in-node (the node pointing to the current node). ##TNodeNet::TNodeI::GetInNId
    int GetInNId(const int& NodeN) const { return NodeHI.GetDat().GetInNId(NodeN); }
    /// Returns ID of NodeN-th out-node (the node the current node points to). ##TNodeNet::TNodeI::GetOutNId
    int GetOutNId(const int& NodeN) const { return NodeHI.GetDat().GetOutNId(NodeN); }
    /// Returns ID of NodeN-th neighboring node. ##TNodeNet::TNodeI::GetNbrNId
    int GetNbrNId(const int& NodeN) const { return NodeHI.GetDat().GetNbrNId(NodeN); }
    /// Tests whether node with ID NId points to the current node.
    bool IsInNId(const int& NId) const { return NodeHI.GetDat().IsInNId(NId); }
    /// Tests whether the current node points to node with ID NId.
    bool IsOutNId(const int& NId) const { return NodeHI.GetDat().IsOutNId(NId); }
    /// Tests whether node with ID NId is a neighbor of the current node.
    bool IsNbrNId(const int& NId) const { return IsOutNId(NId) || IsInNId(NId); }
    const TNodeData& operator () () const { return NodeHI.GetDat().NodeDat; }
    TNodeData& operator () () { return NodeHI.GetDat().GetDat(); }
    const TNodeData& GetDat() const { return NodeHI.GetDat().GetDat(); }
    TNodeData& GetDat() { return NodeHI.GetDat().GetDat(); }
    const TNodeData& GetInNDat(const int& NodeN) const { return Net->GetNDat(GetInNId(NodeN)); }
    TNodeData& GetInNDat(const int& NodeN) { return Net->GetNDat(GetInNId(NodeN)); }
    const TNodeData& GetOutNDat(const int& NodeN) const { return Net->GetNDat(GetOutNId(NodeN)); }
    TNodeData& GetOutNDat(const int& NodeN) { return Net->GetNDat(GetOutNId(NodeN)); }
    const TNodeData& GetNbrNDat(const int& NodeN) const { return Net->GetNDat(GetNbrNId(NodeN)); }
    TNodeData& GetNbrNDat(const int& NodeN) { return Net->GetNDat(GetNbrNId(NodeN)); }
    friend class TNodeNet<TNodeData>;
  };

  /// Edge iterator. Only forward iteration (operator++) is supported.
  class TEdgeI {
  private:
    TNodeI CurNode, EndNode;
    int CurEdge;
  public:
    TEdgeI() : CurNode(), EndNode(), CurEdge(0) { }
    TEdgeI(const TNodeI& NodeI, const TNodeI& EndNodeI, const int& EdgeN=0) : CurNode(NodeI), EndNode(EndNodeI), CurEdge(EdgeN) { }
    TEdgeI(const TEdgeI& EdgeI) : CurNode(EdgeI.CurNode), EndNode(EdgeI.EndNode), CurEdge(EdgeI.CurEdge) { }
    TEdgeI& operator = (const TEdgeI& EdgeI) { if (this!=&EdgeI) { CurNode=EdgeI.CurNode;  EndNode=EdgeI.EndNode;  CurEdge=EdgeI.CurEdge; }  return *this; }
    /// Increment iterator.
    TEdgeI& operator++ (int) { CurEdge++; if (CurEdge >= CurNode.GetOutDeg()) { CurEdge=0;  CurNode++;
      while (CurNode < EndNode && CurNode.GetOutDeg()==0) { CurNode++; } }  return *this; }
    bool operator < (const TEdgeI& EdgeI) const { return CurNode<EdgeI.CurNode || (CurNode==EdgeI.CurNode && CurEdge<EdgeI.CurEdge); }
    bool operator == (const TEdgeI& EdgeI) const { return CurNode == EdgeI.CurNode && CurEdge == EdgeI.CurEdge; }
    /// Gets edge ID. Always returns -1 since only edges in multigraphs have explicit IDs.
    int GetId() const { return -1; }
    /// Gets the source node of an edge.
    int GetSrcNId() const { return CurNode.GetId(); }
    /// Gets the destination node of an edge.
    int GetDstNId() const { return CurNode.GetOutNId(CurEdge); }
    const TNodeData& GetSrcNDat() const { return CurNode.GetDat(); }
    TNodeData& GetDstNDat() { return CurNode.GetOutNDat(CurEdge); }
    friend class TNodeNet<TNodeData>;
  };

protected:
  TNode& GetNode(const int& NId) { return NodeH.GetDat(NId); }

protected:
  TCRef CRef;
  TInt MxNId;
  THash<TInt, TNode> NodeH;

private:
  class TNodeFunctor {
  public:
    TNodeFunctor() {}
    void operator() (TNode* n, TShMIn& ShMIn) { n->LoadShM(ShMIn);}
  };
private:
  void LoadNetworkShM(TShMIn& ShMIn) {
    MxNId = TInt(ShMIn);
    TNodeFunctor f;
    NodeH.LoadShM(ShMIn, f);
  }

public:
  TNodeNet() : CRef(), MxNId(0), NodeH() { }
  /// Constructor that reserves enough memory for a network of Nodes nodes and Edges edges.
  explicit TNodeNet(const int& Nodes, const int& Edges) : MxNId(0) { Reserve(Nodes, Edges); }
  TNodeNet(const TNodeNet& NodeNet) : MxNId(NodeNet.MxNId), NodeH(NodeNet.NodeH) { }
  /// Constructor that loads the network from a (binary) stream SIn.
  TNodeNet(TSIn& SIn) : MxNId(SIn), NodeH(SIn) { }
  virtual ~TNodeNet() { }
  /// Saves the network to a (binary) stream SOut.
  virtual void Save(TSOut& SOut) const { MxNId.Save(SOut);  NodeH.Save(SOut); }
  /// Static constructor that returns a pointer to the network. Call: TPt <TNodeNet<TNodeData> > Net = TNodeNet<TNodeData>::New().
  static PNet New() { return PNet(new TNodeNet()); }
  /// Static constructor that loads the network from a stream SIn and returns a pointer to it.
  static PNet Load(TSIn& SIn) { return PNet(new TNodeNet(SIn)); }
  /// Static constructor that loads the network from shared memory. ##TNodeNet::LoadShM(TShMIn& ShMIn)
  static PNet LoadShM(TShMIn& ShMIn) {
    TNodeNet* Network = new TNodeNet();
    Network->LoadNetworkShM(ShMIn);
    return PNet(Network);
  }
  /// Allows for run-time checking the type of the network (see the TGraphFlag for flags).
  bool HasFlag(const TGraphFlag& Flag) const;
  TNodeNet& operator = (const TNodeNet& NodeNet) {
    if (this!=&NodeNet) { NodeH=NodeNet.NodeH;  MxNId=NodeNet.MxNId; }  return *this; }
  // nodes
  /// Returns the number of nodes in the network.
  int GetNodes() const { return NodeH.Len(); }
  /// Adds a node of ID NId to the network. ##TNodeNet::AddNode
  int AddNode(int NId = -1);
  /// Adds a node of ID NId to the network, noop if the node already exists. ##TNodeNet::AddNodeUnchecked
  int AddNodeUnchecked(int NId = -1);
  /// Adds a node of ID NId and node data NodeDat to the network. ##TNodeNet::AddNode-1
  int AddNode(int NId, const TNodeData& NodeDat);
  /// Adds a node NodeI and its node data to the network.
  int AddNode(const TNodeI& NodeI) { return AddNode(NodeI.GetId(), NodeI.GetDat()); }
  /// Deletes node of ID NId from the network. ##TNodeNet::DelNode
  virtual void DelNode(const int& NId);
  /// Deletes node of ID NodeI.GetId() from the network.
  void DelNode(const TNode& NodeI) { DelNode(NodeI.GetId()); }
  /// Tests whether ID NId is a node.
  bool IsNode(const int& NId) const { return NodeH.IsKey(NId); }
  /// Returns an iterator referring to the first node in the network.
  TNodeI BegNI() const { return TNodeI(NodeH.BegI(), this); }
  /// Returns an iterator referring to the past-the-end node in the network.
  TNodeI EndNI() const { return TNodeI(NodeH.EndI(), this); }
  /// Returns an iterator referring to the node of ID NId in the network.
  TNodeI GetNI(const int& NId) const { return TNodeI(NodeH.GetI(NId), this); }
  /// Returns node element for the node of ID NId in the network.
  const TNode& GetNode(const int& NId) const { return NodeH.GetDat(NId); }
  /// Sets node data for the node of ID NId in the network.
  void SetNDat(const int& NId, const TNodeData& NodeDat);
  /// Returns node data for the node of ID NId in the network.
  TNodeData& GetNDat(const int& NId) { return NodeH.GetDat(NId).NodeDat; }
  /// Returns node data for the node of ID NId in the network.
  const TNodeData& GetNDat(const int& NId) const { return NodeH.GetDat(NId).NodeDat; }
  /// Returns an ID that is larger than any node ID in the network.
  int GetMxNId() const { return MxNId; }

  // edges
  /// Returns the number of edges in the network.
  int GetEdges() const;
  /// Adds an edge from node SrcNId to node DstNId to the network. ##TNodeNet::AddEdge
  int AddEdge(const int& SrcNId, const int& DstNId);
  /// Adds an edge from EdgeI.GetSrcNId() to EdgeI.GetDstNId() to the network.
  int AddEdge(const TEdgeI& EdgeI) { return AddEdge(EdgeI.GetSrcNId(), EdgeI.GetDstNId()); }
  /// Deletes an edge from node IDs SrcNId to DstNId from the network. ##TNodeNet::DelEdge
  void DelEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true);
  /// Tests whether an edge from node IDs SrcNId to DstNId exists in the network.
  bool IsEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true) const;
  /// Returns an iterator referring to the first edge in the network.
  TEdgeI BegEI() const { TNodeI NI=BegNI();  while(NI<EndNI() && NI.GetOutDeg()==0) NI++;  return TEdgeI(NI, EndNI()); }
  /// Returns an iterator referring to the past-the-end edge in the network.
  TEdgeI EndEI() const { return TEdgeI(EndNI(), EndNI()); }
  /// Not supported/implemented!
  TEdgeI GetEI(const int& EId) const;
  /// Returns an iterator referring to edge (SrcNId, DstNId) in the network.
  TEdgeI GetEI(const int& SrcNId, const int& DstNId) const;

  /// Returns an ID of a random node in the network.
  int GetRndNId(TRnd& Rnd=TInt::Rnd) { return NodeH.GetKey(NodeH.GetRndKeyId(Rnd, 0.8)); }
  /// Returns an interator referring to a random node in the network.
  TNodeI GetRndNI(TRnd& Rnd=TInt::Rnd) { return GetNI(GetRndNId(Rnd)); }
  /// Gets a vector IDs of all nodes in the network.
  void GetNIdV(TIntV& NIdV) const;

  /// Tests whether the network is empty (has zero nodes).
  bool Empty() const { return GetNodes()==0; }
  /// Deletes all nodes and edges from the network.
  void Clr(const bool& DoDel=true, const bool& ResetDat=true) {
    MxNId = 0;  NodeH.Clr(DoDel, -1, ResetDat); }
  /// Reserves memory for a network of Nodes nodes and Edges edges.
  void Reserve(const int& Nodes, const int& Edges) { if (Nodes>0) { NodeH.Gen(Nodes/2); } }
  /// Sorts nodes by node IDs.
  void SortNIdById(const bool& Asc=true) { NodeH.SortByKey(Asc); }
  /// Sorts nodes by node data.
  void SortNIdByDat(const bool& Asc=true) { NodeH.SortByDat(Asc); }
  /// Defragments the network. ##TNodeNet::Defrag
  void Defrag(const bool& OnlyNodeLinks=false);
  /// Checks the network data structure for internal consistency. ##TNodeNet::IsOk
  bool IsOk(const bool& ThrowExcept=true) const;

  friend class TPt<TNodeNet<TNodeData> >;
};

// set flags
namespace TSnap {
template <class TNodeData> struct IsDirected<TNodeNet<TNodeData> > { enum { Val = 1 }; };
template <class TNodeData> struct IsNodeDat<TNodeNet<TNodeData> > { enum { Val = 1 }; };
}

template <class TNodeData>
bool TNodeNet<TNodeData>::HasFlag(const TGraphFlag& Flag) const {
  return HasGraphFlag(typename TNet, Flag);
}

template <class TNodeData>
int TNodeNet<TNodeData>::AddNode(int NId) {
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    MxNId = TMath::Mx(NId+1, MxNId());
  }
  NodeH.AddDat(NId, TNode(NId));
  return NId;
}

template <class TNodeData>
int TNodeNet<TNodeData>::AddNodeUnchecked(int NId) {
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    if (IsNode(NId)) { return -1;}
    MxNId = TMath::Mx(NId+1, MxNId());
  }
  NodeH.AddDat(NId, TNode(NId));
  return NId;
}

template <class TNodeData>
int TNodeNet<TNodeData>::AddNode(int NId, const TNodeData& NodeDat) {
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    MxNId = TMath::Mx(NId+1, MxNId());
  }
  NodeH.AddDat(NId, TNode(NId, NodeDat));
  return NId;
}

template <class TNodeData>
void TNodeNet<TNodeData>::DelNode(const int& NId) {
  { TNode& Node = GetNode(NId);
  for (int e = 0; e < Node.GetOutDeg(); e++) {
  const int nbr = Node.GetOutNId(e);
  if (nbr == NId) { continue; }
    TNode& N = GetNode(nbr);
    int n = N.InNIdV.SearchBin(NId);
    if (n!= -1) { N.InNIdV.Del(n); }
  }
  for (int e = 0; e < Node.GetInDeg(); e++) {
  const int nbr = Node.GetInNId(e);
  if (nbr == NId) { continue; }
    TNode& N = GetNode(nbr);
    int n = N.OutNIdV.SearchBin(NId);
    if (n!= -1) { N.OutNIdV.Del(n); }
  } }
  NodeH.DelKey(NId);
}

template <class TNodeData>
void TNodeNet<TNodeData>::SetNDat(const int& NId, const TNodeData& NodeDat) {
  IAssertR(IsNode(NId), TStr::Fmt("NodeId %d does not exist.", NId).CStr());
  NodeH.GetDat(NId).NodeDat = NodeDat;
}

template <class TNodeData>
int TNodeNet<TNodeData>::GetEdges() const {
  int edges=0;
  for (int N=NodeH.FFirstKeyId(); NodeH.FNextKeyId(N);) {
    edges+=NodeH[N].GetOutDeg(); }
  return edges;
}

template <class TNodeData>
int TNodeNet<TNodeData>::AddEdge(const int& SrcNId, const int& DstNId) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  if (IsEdge(SrcNId, DstNId)) { return -2; }
  GetNode(SrcNId).OutNIdV.AddSorted(DstNId);
  GetNode(DstNId).InNIdV.AddSorted(SrcNId);
  return -1; // no edge id
}

template <class TNodeData>
void TNodeNet<TNodeData>::DelEdge(const int& SrcNId, const int& DstNId, const bool& IsDir) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  GetNode(SrcNId).OutNIdV.DelIfIn(DstNId);
  GetNode(DstNId).InNIdV.DelIfIn(SrcNId);
  if (! IsDir) {
    GetNode(DstNId).OutNIdV.DelIfIn(SrcNId);
    GetNode(SrcNId).InNIdV.DelIfIn(DstNId);
  }
}

template <class TNodeData>
bool TNodeNet<TNodeData>::IsEdge(const int& SrcNId, const int& DstNId, const bool& IsDir) const {
  if (! IsNode(SrcNId) || ! IsNode(DstNId)) { return false; }
  if (IsDir) { return GetNode(SrcNId).IsOutNId(DstNId); }
  else { return GetNode(SrcNId).IsOutNId(DstNId) || GetNode(DstNId).IsOutNId(SrcNId); }
}

template <class TNodeData>
void TNodeNet<TNodeData>::GetNIdV(TIntV& NIdV) const {
  NIdV.Reserve(GetNodes(), 0);
  for (int N=NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    NIdV.Add(NodeH.GetKey(N)); }
}

template <class TNodeData>
typename TNodeNet<TNodeData>::TEdgeI  TNodeNet<TNodeData>::GetEI(const int& SrcNId, const int& DstNId) const {
  const TNodeI SrcNI = GetNI(SrcNId);
  const int NodeN = SrcNI.NodeHI.GetDat().OutNIdV.SearchBin(DstNId);
  if (NodeN == -1) { return EndEI(); }
  return TEdgeI(SrcNI, EndNI(), NodeN);
}

template <class TNodeData>
void TNodeNet<TNodeData>::Defrag(const bool& OnlyNodeLinks) {
  for (int n = NodeH.FFirstKeyId(); NodeH.FNextKeyId(n); ) {
    TNode& Node = NodeH[n];
    Node.InNIdV.Pack();  Node.OutNIdV.Pack();
  }
  if (! OnlyNodeLinks && ! NodeH.IsKeyIdEqKeyN()) {
    NodeH.Defrag(); }
}

template <class TNodeData>
bool TNodeNet<TNodeData>::IsOk(const bool& ThrowExcept) const {
  bool RetVal = true;
  for (int N = NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    const TNode& Node = NodeH[N];
    if (! Node.OutNIdV.IsSorted()) {
      const TStr Msg = TStr::Fmt("Out-neighbor list of node %d is not sorted.", Node.GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
    if (! Node.InNIdV.IsSorted()) {
      const TStr Msg = TStr::Fmt("In-neighbor list of node %d is not sorted.", Node.GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
    // check out-edges
    int prevNId = -1;
    for (int e = 0; e < Node.GetOutDeg(); e++) {
      if (! IsNode(Node.GetOutNId(e))) {
        const TStr Msg = TStr::Fmt("Out-edge %d --> %d: node %d does not exist.",
          Node.GetId(), Node.GetOutNId(e), Node.GetOutNId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      if (e > 0 && prevNId == Node.GetOutNId(e)) {
        const TStr Msg = TStr::Fmt("Node %d has duplidate out-edge %d --> %d.",
          Node.GetId(), Node.GetId(), Node.GetOutNId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      prevNId = Node.GetOutNId(e);
    }
    // check in-edges
    prevNId = -1;
    for (int e = 0; e < Node.GetInDeg(); e++) {
      if (! IsNode(Node.GetInNId(e))) {
        const TStr Msg = TStr::Fmt("In-edge %d <-- %d: node %d does not exist.",
          Node.GetId(), Node.GetInNId(e), Node.GetInNId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      if (e > 0 && prevNId == Node.GetInNId(e)) {
        const TStr Msg = TStr::Fmt("Node %d has duplidate in-edge %d <-- %d.",
          Node.GetId(), Node.GetId(), Node.GetInNId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      prevNId = Node.GetInNId(e);
    }
  }
  return RetVal;
}

/////////////////////////////////////////////////
// Common network types
typedef TNodeNet<TInt> TIntNNet;
typedef TPt<TIntNNet> PIntNNet;
typedef TNodeNet<TFlt> TFltNNet;
typedef TPt<TFltNNet> PFltNNet;
typedef TNodeNet<TStr> TStrNNet;
typedef TPt<TStrNNet> PStrNNet;

//#//////////////////////////////////////////////
/// Node Edge Network (directed graph, TNGraph with data on nodes and edges).
template <class TNodeData, class TEdgeData>
class TNodeEDatNet {
public:
  typedef TNodeData TNodeDat;
  typedef TEdgeData TEdgeDat;
  typedef TNodeEDatNet<TNodeData, TEdgeData> TNet;
  typedef TPt<TNet> PNet;
  typedef TVec<TPair<TInt, TEdgeData> > TNIdDatPrV;
public:
  class TNode {
  private:
    TInt  Id;
    TNodeData NodeDat;
    TIntV InNIdV;
    TNIdDatPrV OutNIdV;
  public:
    TNode() : Id(-1), NodeDat(), InNIdV(), OutNIdV() { }
    TNode(const int& NId) : Id(NId), NodeDat(), InNIdV(), OutNIdV() { }
    TNode(const int& NId, const TNodeData& NodeData) : Id(NId), NodeDat(NodeData), InNIdV(), OutNIdV() { }
    TNode(const TNode& Node) : Id(Node.Id), NodeDat(Node.NodeDat), InNIdV(Node.InNIdV), OutNIdV(Node.OutNIdV) { }
    TNode(TSIn& SIn) : Id(SIn), NodeDat(SIn), InNIdV(SIn), OutNIdV(SIn) { }
    void Save(TSOut& SOut) const { Id.Save(SOut);  NodeDat.Save(SOut);  InNIdV.Save(SOut);  OutNIdV.Save(SOut); }
    int GetId() const { return Id; }
    int GetDeg() const { return GetInDeg() + GetOutDeg(); }
    int GetInDeg() const { return InNIdV.Len(); }
    int GetOutDeg() const { return OutNIdV.Len(); }
    const TNodeData& GetDat() const { return NodeDat; }
    TNodeData& GetDat() { return NodeDat; }
    int GetInNId(const int& EdgeN) const { return InNIdV[EdgeN]; }
    int GetOutNId(const int& EdgeN) const { return OutNIdV[EdgeN].Val1; }
    int GetNbrNId(const int& EdgeN) const { return EdgeN<GetOutDeg() ? GetOutNId(EdgeN):GetInNId(EdgeN-GetOutDeg()); }
    TEdgeData& GetOutEDat(const int& EdgeN) { return OutNIdV[EdgeN].Val2; }
    const TEdgeData& GetOutEDat(const int& EdgeN) const { return OutNIdV[EdgeN].Val2; }
    bool IsInNId(const int& NId) const { return InNIdV.SearchBin(NId)!=-1; }
    bool IsOutNId(const int& NId) const { return TNodeEDatNet::GetNIdPos(OutNIdV, NId)!=-1; }
    bool IsNbrNId(const int& NId) const { return IsOutNId(NId) || IsInNId(NId); }
    void LoadShM(TShMIn& MStream) {
      Id = TInt(MStream);
      NodeDat = TNodeData(MStream);
      InNIdV.LoadShM(MStream);
      OutNIdV.LoadShM(MStream);
    }
    bool operator < (const TNode& Node) const { return NodeDat < Node.NodeDat; }
    friend class TNodeEDatNet<TNodeData, TEdgeData>;
  };

  /// Node iterator. Only forward iteration (operator++) is supported.
  class TNodeI {
  private:
    typedef typename THash<TInt, TNode>::TIter THashIter;
    THashIter NodeHI;
    TNodeEDatNet *Net;
  public:
    TNodeI() : NodeHI(), Net(NULL) { }
    TNodeI(const THashIter& NodeHIter, const TNodeEDatNet* NetPt) : NodeHI(NodeHIter), Net((TNodeEDatNet *) NetPt) { }
    TNodeI(const TNodeI& NodeI) : NodeHI(NodeI.NodeHI), Net(NodeI.Net) { }
    TNodeI& operator = (const TNodeI& NodeI) { NodeHI=NodeI.NodeHI; Net=NodeI.Net; return *this; }
    /// Increment iterator.
    TNodeI& operator++ (int) { NodeHI++;  return *this; }
    bool operator < (const TNodeI& NodeI) const { return NodeHI < NodeI.NodeHI; }
    bool operator == (const TNodeI& NodeI) const { return NodeHI == NodeI.NodeHI; }

    /// Returns ID of the current node.
    int GetId() const { return NodeHI.GetDat().GetId(); }
    /// Returns degree of the current node.
    int GetDeg() const { return NodeHI.GetDat().GetDeg(); }
    /// Returns in-degree of the current node.
    int GetInDeg() const { return NodeHI.GetDat().GetInDeg(); }
    /// Returns out-degree of the current node.
    int GetOutDeg() const { return NodeHI.GetDat().GetOutDeg(); }
    /// Returns ID of NodeN-th in-node (the node pointing to the current node). ##TNodeEDatNet::TNodeI::GetInNId
    int GetInNId(const int& NodeN) const { return NodeHI.GetDat().GetInNId(NodeN); }
    /// Returns ID of NodeN-th out-node (the node the current node points to). ##TNodeEDatNet::TNodeI::GetOutNId
    int GetOutNId(const int& NodeN) const { return NodeHI.GetDat().GetOutNId(NodeN); }
    /// Returns ID of NodeN-th neighboring node. ##TNodeEDatNet::TNodeI::GetNbrNId
    int GetNbrNId(const int& NodeN) const { return NodeHI.GetDat().GetNbrNId(NodeN); }
    /// Tests whether node with ID NId points to the current node.
    bool IsInNId(const int& NId) const { return NodeHI.GetDat().IsInNId(NId); }
    /// Tests whether the current node points to node with ID NId.
    bool IsOutNId(const int& NId) const { return NodeHI.GetDat().IsOutNId(NId); }
    /// Tests whether node with ID NId is a neighbor of the current node.
    bool IsNbrNId(const int& NId) const { return IsOutNId(NId) || IsInNId(NId); }
    // node data
    const TNodeData& operator () () const { return NodeHI.GetDat().NodeDat; }
    TNodeData& operator () () { return NodeHI.GetDat().GetDat(); }
    const TNodeData& GetDat() const { return NodeHI.GetDat().GetDat(); }
    TNodeData& GetDat() { return NodeHI.GetDat().GetDat(); }
    const TNodeData& GetOutNDat(const int& NodeN) const { return Net->GetNDat(GetOutNId(NodeN)); }
    TNodeData& GetOutNDat(const int& NodeN) { return Net->GetNDat(GetOutNId(NodeN)); }
    const TNodeData& GetInNDat(const int& NodeN) const { return Net->GetNDat(GetInNId(NodeN)); }
    TNodeData& GetInNDat(const int& NodeN) { return Net->GetNDat(GetInNId(NodeN)); }
    const TNodeData& GetNbrNDat(const int& NodeN) const { return Net->GetNDat(GetNbrNId(NodeN)); }
    TNodeData& GetNbrNDat(const int& NodeN) { return Net->GetNDat(GetNbrNId(NodeN)); }
    // edge data
    TEdgeData& GetOutEDat(const int& EdgeN) { return NodeHI.GetDat().GetOutEDat(EdgeN); }
    const TEdgeData& GetOutEDat(const int& EdgeN) const { return NodeHI.GetDat().GetOutEDat(EdgeN); }
    TEdgeData& GetInEDat(const int& EdgeN) { return Net->GetEDat(GetInNId(EdgeN), GetId()); }
    const TEdgeData& GetInEDat(const int& EdgeN) const { return Net->GetEDat(GetInNId(EdgeN), GetId()); }
    TEdgeData& GetNbrEDat(const int& EdgeN) { return EdgeN<GetOutDeg() ? GetOutEDat(EdgeN) : GetInEDat(EdgeN-GetOutDeg()); }
    const TEdgeData& GetNbrEDat(const int& EdgeN) const { return EdgeN<GetOutDeg() ? GetOutEDat(EdgeN) : GetInEDat(EdgeN-GetOutDeg()); }
    friend class TNodeEDatNet<TNodeData, TEdgeData>;
  };

  /// Edge iterator. Only forward iteration (operator++) is supported.
  class TEdgeI {
  private:
    TNodeI CurNode, EndNode;
    int CurEdge;
  public:
    TEdgeI() : CurNode(), EndNode(), CurEdge(0) { }
    TEdgeI(const TNodeI& NodeI, const TNodeI& EndNodeI, const int& EdgeN=0) : CurNode(NodeI), EndNode(EndNodeI), CurEdge(EdgeN) { }
    TEdgeI(const TEdgeI& EdgeI) : CurNode(EdgeI.CurNode), EndNode(EdgeI.EndNode), CurEdge(EdgeI.CurEdge) { }
    TEdgeI& operator = (const TEdgeI& EdgeI) { if (this!=&EdgeI) { CurNode=EdgeI.CurNode;  EndNode=EdgeI.EndNode;  CurEdge=EdgeI.CurEdge; }  return *this; }
    /// Increment iterator.
    TEdgeI& operator++ (int) { CurEdge++; if (CurEdge >= CurNode.GetOutDeg()) { CurEdge=0;  CurNode++;
      while (CurNode < EndNode && CurNode.GetOutDeg()==0) { CurNode++; } }  return *this; }
    bool operator < (const TEdgeI& EdgeI) const { return CurNode<EdgeI.CurNode || (CurNode==EdgeI.CurNode && CurEdge<EdgeI.CurEdge); }
    bool operator == (const TEdgeI& EdgeI) const { return CurNode == EdgeI.CurNode && CurEdge == EdgeI.CurEdge; }
    /// Gets edge ID. Always returns -1 since only edges in multigraphs have explicit IDs.
    int GetId() const { return -1; }
    /// Gets the source node of an edge.
    int GetSrcNId() const { return CurNode.GetId(); }
    /// Gets the destination node of an edge.
    int GetDstNId() const { return CurNode.GetOutNId(CurEdge); }
    TEdgeData& operator () () { return CurNode.GetOutEDat(CurEdge); }
    const TEdgeData& operator () () const { return CurNode.GetOutEDat(CurEdge); }
    TEdgeData& GetDat() { return CurNode.GetOutEDat(CurEdge); }
    const TEdgeData& GetDat() const { return CurNode.GetOutEDat(CurEdge); }
    TNodeData& GetSrcNDat() { return CurNode(); }
    const TNodeData& GetSrcNDat() const { return CurNode(); }
    TNodeData& GetDstNDat() { return CurNode.GetOutNDat(CurEdge); }
    const TNodeData& GetDstNDat() const { return CurNode.GetOutNDat(CurEdge); }
    friend class TNodeEDatNet<TNodeData, TEdgeData>;
  };

protected:
  TNode& GetNode(const int& NId) { return NodeH.GetDat(NId); }
  static int GetNIdPos(const TVec<TPair<TInt, TEdgeData> >& NIdV, const int& NId);
protected:
  TCRef CRef;
  TInt MxNId;
  THash<TInt, TNode> NodeH;
private:
  class TNodeFunctor {
  public:
    TNodeFunctor() {}
    void operator() (TNode* n, TShMIn& ShMIn) { n->LoadShM(ShMIn);}
  };
private:
  void LoadNetworkShM(TShMIn& ShMIn) {
    MxNId = TInt(ShMIn);
    TNodeFunctor f;
    NodeH.LoadShM(ShMIn, f);
  }
public:
  TNodeEDatNet() : CRef(), MxNId(0), NodeH() { }
  /// Constructor that reserves enough memory for a network of Nodes nodes and Edges edges.
  explicit TNodeEDatNet(const int& Nodes, const int& Edges) : MxNId(0) { Reserve(Nodes, Edges); }
  TNodeEDatNet(const TNodeEDatNet& NodeNet) : MxNId(NodeNet.MxNId), NodeH(NodeNet.NodeH) { }
  /// Constructor that loads the network from a (binary) stream SIn.
  TNodeEDatNet(TSIn& SIn) : MxNId(SIn), NodeH(SIn) { }
  virtual ~TNodeEDatNet() { }
  /// Saves the network to a (binary) stream SOut.
  virtual void Save(TSOut& SOut) const { MxNId.Save(SOut);  NodeH.Save(SOut); }
  /// Static constructor that returns a pointer to the network. Call: TPt <TNodeEDatNet<TNodeData, TEdgeData> > Net = TNodeEDatNet<TNodeData, TEdgeData>::New().
  static PNet New() { return PNet(new TNet()); }
  /// Static constructor that loads the network from a stream SIn and returns a pointer to it.
  static PNet Load(TSIn& SIn) { return PNet(new TNet(SIn)); }
  /// Static constructor that loads the network from shared memory. ##TNodeEDatNet::LoadShM(TShMIn& ShMIn)
  static PNet LoadShM(TShMIn& ShMIn) {
    TNet* Network = new TNet();
    Network->LoadNetworkShM(ShMIn);
    return PNet(Network);
  }
  /// Allows for run-time checking the type of the network (see the TGraphFlag for flags).
  bool HasFlag(const TGraphFlag& Flag) const;
  TNodeEDatNet& operator = (const TNodeEDatNet& NodeNet) { if (this!=&NodeNet) {
    NodeH=NodeNet.NodeH;  MxNId=NodeNet.MxNId; }  return *this; }
  // nodes
  /// Returns the number of nodes in the network.
  int GetNodes() const { return NodeH.Len(); }
  /// Adds a node of ID NId to the network. ##TNodeEDatNet::AddNode
  int AddNode(int NId = -1);
  /// Adds a node of ID NId to the network, noop if the node already exists. ##TNodeEDatNet::AddNodeUnchecked
  int AddNodeUnchecked(int NId = -1);
  /// Adds a node of ID NId and node data NodeDat to the network. ##TNodeEDatNet::AddNode-1
  int AddNode(int NId, const TNodeData& NodeDat);
  /// Adds a node NodeI and its node data to the network.
  int AddNode(const TNodeI& NodeI) { return AddNode(NodeI.GetId(), NodeI.GetDat()); }
  /// Deletes node of ID NId from the network. ##TNodeEDatNet::DelNode
  void DelNode(const int& NId);
  /// Deletes node of ID NodeI.GetId() from the network.
  void DelNode(const TNode& NodeI) { DelNode(NodeI.GetId()); }
  /// Tests whether ID NId is a node.
  bool IsNode(const int& NId) const { return NodeH.IsKey(NId); }
  /// Returns an iterator referring to the first node in the network.
  TNodeI BegNI() const { return TNodeI(NodeH.BegI(), this); }
  /// Returns an iterator referring to the past-the-end node in the network.
  TNodeI EndNI() const { return TNodeI(NodeH.EndI(), this); }
  /// Returns an iterator referring to the node of ID NId in the network.
  TNodeI GetNI(const int& NId) const { return TNodeI(NodeH.GetI(NId), this); }
  /// Returns node element for the node of ID NId in the network.
  const TNode& GetNode(const int& NId) const { return NodeH.GetDat(NId); }
  /// Sets node data for the node of ID NId in the network.
  void SetNDat(const int& NId, const TNodeData& NodeDat);
  /// Returns node data for the node of ID NId in the network.
  TNodeData& GetNDat(const int& NId) { return NodeH.GetDat(NId).NodeDat; }
  /// Returns node data for the node of ID NId in the network.
  const TNodeData& GetNDat(const int& NId) const { return NodeH.GetDat(NId).NodeDat; }
  /// Returns an ID that is larger than any node ID in the network.
  int GetMxNId() const { return MxNId; }

  // edges
  /// Returns the number of edges in the network.
  int GetEdges() const;
  /// Adds an edge from node SrcNId to node DstNId to the network. ##TNodeEDatNet::AddEdge
  int AddEdge(const int& SrcNId, const int& DstNId);
  /// Adds an edge and edge data from node IDs SrcNId to node DstNId. ##TNodeEDatNet::AddEdge-1
  int AddEdge(const int& SrcNId, const int& DstNId, const TEdgeData& EdgeDat);
  /// Adds an edge from EdgeI.GetSrcNId() to EdgeI.GetDstNId() and its edge data to the network.
  int AddEdge(const TEdgeI& EdgeI) { return AddEdge(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), EdgeI()); }
  /// Deletes an edge from node IDs SrcNId to DstNId from the network. ##TNodeEDatNet::DelEdge
  void DelEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true);
  /// Tests whether an edge from node IDs SrcNId to DstNId exists in the network.
  bool IsEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true) const;
  /// Returns an iterator referring to the first edge in the network.
  TEdgeI BegEI() const { TNodeI NI=BegNI();  while(NI<EndNI() && NI.GetOutDeg()==0) NI++; return TEdgeI(NI, EndNI()); }
  /// Returns an iterator referring to the past-the-end edge in the network.
  TEdgeI EndEI() const { return TEdgeI(EndNI(), EndNI()); }
  /// Not supported/implemented!
  TEdgeI GetEI(const int& EId) const; // not supported
  /// Returns an iterator referring to edge (SrcNId, DstNId) in the network.
  TEdgeI GetEI(const int& SrcNId, const int& DstNId) const;
  /// Sets edge data for the edge between nodes SrcNId and DstNId in the network.
  void SetEDat(const int& SrcNId, const int& DstNId, const TEdgeData& EdgeDat);
  /// Returns edge data in Data for the edge from node IDs SrcNId to DstNId. ##TNodeEDatNet::GetEDat
  bool GetEDat(const int& SrcNId, const int& DstNId, TEdgeData& EdgeDat) const;
  /// Returns edge data for the edge from node IDs SrcNId to DstNId.
  TEdgeData& GetEDat(const int& SrcNId, const int& DstNId);
  /// Returns edge data for the edge from node IDs SrcNId to DstNId.
  const TEdgeData& GetEDat(const int& SrcNId, const int& DstNId) const;
  /// Sets edge data for all the edges in the network to EDat.
  void SetAllEDat(const TEdgeData& EdgeDat);

  /// Returns an ID of a random node in the network.
  int GetRndNId(TRnd& Rnd=TInt::Rnd) { return NodeH.GetKey(NodeH.GetRndKeyId(Rnd, 0.8)); }
  /// Returns an interator referring to a random node in the network.
  TNodeI GetRndNI(TRnd& Rnd=TInt::Rnd) { return GetNI(GetRndNId(Rnd)); }
  /// Gets a vector IDs of all nodes in the network.
  void GetNIdV(TIntV& NIdV) const;

  /// Tests whether the network is empty (has zero nodes).
  bool Empty() const { return GetNodes()==0; }
  /// Deletes all nodes and edges from the network.
  void Clr(const bool& DoDel=true, const bool& ResetDat=true) {
    MxNId = 0;  NodeH.Clr(DoDel, -1, ResetDat); }
  /// Reserves memory for a network of Nodes nodes and Edges edges.
  void Reserve(const int& Nodes, const int& Edges) { if (Nodes>0) { NodeH.Gen(Nodes/2); } }
  /// Sorts nodes by node IDs.
  void SortNIdById(const bool& Asc=true) { NodeH.SortByKey(Asc); }
  /// Sorts nodes by node data.
  void SortNIdByDat(const bool& Asc=true) { NodeH.SortByDat(Asc); }
  /// Defragments the network. ##TNodeEDatNet::Defrag
  void Defrag(const bool& OnlyNodeLinks=false);
  /// Checks the network data structure for internal consistency. ##TNodeEDatNet::IsOk
  bool IsOk(const bool& ThrowExcept=true) const;

  friend class TPt<TNodeEDatNet<TNodeData, TEdgeData> >;
};

// set flags
namespace TSnap {
template <class TNodeData, class TEdgeData> struct IsDirected<TNodeEDatNet<TNodeData, TEdgeData> > { enum { Val = 1 }; };
template <class TNodeData, class TEdgeData> struct IsNodeDat<TNodeEDatNet<TNodeData, TEdgeData> > { enum { Val = 1 }; };
template <class TNodeData, class TEdgeData> struct IsEdgeDat<TNodeEDatNet<TNodeData, TEdgeData> > { enum { Val = 1 }; };
}

template <class TNodeData, class TEdgeData>
bool TNodeEDatNet<TNodeData, TEdgeData>::HasFlag(const TGraphFlag& Flag) const {
  return HasGraphFlag(typename TNet, Flag);
}

template <class TNodeData, class TEdgeData>
int TNodeEDatNet<TNodeData, TEdgeData>::GetNIdPos(const TVec<TPair<TInt, TEdgeData> >& NIdV, const int& NId) {
  int LValN=0, RValN = NIdV.Len()-1;
  while (RValN >= LValN) {
    const int ValN = (LValN+RValN)/2;
    const int CurNId = NIdV[ValN].Val1;
    if (NId == CurNId) { return ValN; }
    if (NId < CurNId) { RValN=ValN-1; }
    else { LValN=ValN+1; }
  }
  return -1;
}

template <class TNodeData, class TEdgeData>
int TNodeEDatNet<TNodeData, TEdgeData>::AddNode(int NId) {
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    MxNId = TMath::Mx(NId+1, MxNId());
  }
  NodeH.AddDat(NId, TNode(NId));
  return NId;
}

template <class TNodeData, class TEdgeData>
int TNodeEDatNet<TNodeData, TEdgeData>::AddNodeUnchecked(int NId) {
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    if (IsNode(NId)) { return -1;}
    MxNId = TMath::Mx(NId+1, MxNId());
  }
  NodeH.AddDat(NId, TNode(NId));
  return NId;
}

template <class TNodeData, class TEdgeData>
int TNodeEDatNet<TNodeData, TEdgeData>::AddNode(int NId, const TNodeData& NodeDat) {
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    MxNId = TMath::Mx(NId+1, MxNId());
  }
  NodeH.AddDat(NId, TNode(NId, NodeDat));
  return NId;
}

template <class TNodeData, class TEdgeData>
void TNodeEDatNet<TNodeData, TEdgeData>::SetNDat(const int& NId, const TNodeData& NodeDat) {
  IAssertR(IsNode(NId), TStr::Fmt("NodeId %d does not exist.", NId).CStr());
  NodeH.GetDat(NId).NodeDat = NodeDat;
}

template <class TNodeData, class TEdgeData>
void TNodeEDatNet<TNodeData, TEdgeData>::DelNode(const int& NId) {
  const TNode& Node = GetNode(NId);
  for (int out = 0; out < Node.GetOutDeg(); out++) {
    const int nbr = Node.GetOutNId(out);
    if (nbr == NId) { continue; }
    TIntV& NIdV = GetNode(nbr).InNIdV;
    const int pos = NIdV.SearchBin(NId);
    if (pos != -1) { NIdV.Del(pos); }
  }
  for (int in = 0; in < Node.GetInDeg(); in++) {
    const int nbr = Node.GetInNId(in);
    if (nbr == NId) { continue; }
    TNIdDatPrV& NIdDatV = GetNode(nbr).OutNIdV;
    const int pos = GetNIdPos(NIdDatV, NId);
    if (pos != -1) { NIdDatV.Del(pos); }
  }
  NodeH.DelKey(NId);
}

template <class TNodeData, class TEdgeData>
int TNodeEDatNet<TNodeData, TEdgeData>::GetEdges() const {
  int edges=0;
  for (int N=NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    edges+=NodeH[N].GetOutDeg(); }
  return edges;
}

template <class TNodeData, class TEdgeData>
int TNodeEDatNet<TNodeData, TEdgeData>::AddEdge(const int& SrcNId, const int& DstNId) {
  return AddEdge(SrcNId, DstNId, TEdgeData());
}

template <class TNodeData, class TEdgeData>
int TNodeEDatNet<TNodeData, TEdgeData>::AddEdge(const int& SrcNId, const int& DstNId, const TEdgeData& EdgeDat) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  //IAssert(! IsEdge(SrcNId, DstNId));
  if (IsEdge(SrcNId, DstNId)) {
    GetEDat(SrcNId, DstNId) = EdgeDat;
    return -2;
  }
  GetNode(SrcNId).OutNIdV.AddSorted(TPair<TInt, TEdgeData>(DstNId, EdgeDat));
  GetNode(DstNId).InNIdV.AddSorted(SrcNId);
  return -1; // no edge id
}

template <class TNodeData, class TEdgeData>
void TNodeEDatNet<TNodeData, TEdgeData>::DelEdge(const int& SrcNId, const int& DstNId, const bool& IsDir) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  int pos = GetNIdPos(GetNode(SrcNId).OutNIdV, DstNId);
  if (pos != -1) { GetNode(SrcNId).OutNIdV.Del(pos); }
  pos = GetNode(DstNId).InNIdV.SearchBin(SrcNId);
  if (pos != -1) { GetNode(DstNId).InNIdV.Del(pos); }
  if (! IsDir) {
    pos = GetNIdPos(GetNode(DstNId).OutNIdV, SrcNId);
    if (pos != -1) { GetNode(DstNId).OutNIdV.Del(pos); }
    pos = GetNode(SrcNId).InNIdV.SearchBin(DstNId);
    if (pos != -1) { GetNode(SrcNId).InNIdV.Del(pos); }
  }
}

template <class TNodeData, class TEdgeData>
bool TNodeEDatNet<TNodeData, TEdgeData>::IsEdge(const int& SrcNId, const int& DstNId, const bool& IsDir) const {
  if (! IsNode(SrcNId) || ! IsNode(DstNId)) { return false; }
  if (IsDir) { return GetNode(SrcNId).IsOutNId(DstNId); }
  else { return GetNode(SrcNId).IsOutNId(DstNId) || GetNode(DstNId).IsOutNId(SrcNId); }
}

template <class TNodeData, class TEdgeData>
void TNodeEDatNet<TNodeData, TEdgeData>::SetEDat(const int& SrcNId, const int& DstNId, const TEdgeData& EdgeDat) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  IAssertR(IsEdge(SrcNId, DstNId), TStr::Fmt("Edge between %d and %d does not exist.", SrcNId, DstNId).CStr());
  GetEDat(SrcNId, DstNId) = EdgeDat;
}

template <class TNodeData, class TEdgeData>
bool TNodeEDatNet<TNodeData, TEdgeData>::GetEDat(const int& SrcNId, const int& DstNId, TEdgeData& EdgeDat) const {
  if (! IsEdge(SrcNId, DstNId)) { return false; }
  const TNode& N = GetNode(SrcNId);
  EdgeDat = N.GetOutEDat(GetNIdPos(N.OutNIdV, DstNId));
  return true;
}

template <class TNodeData, class TEdgeData>
TEdgeData& TNodeEDatNet<TNodeData, TEdgeData>::GetEDat(const int& SrcNId, const int& DstNId) {
  Assert(IsEdge(SrcNId, DstNId));
  TNode& N = GetNode(SrcNId);
  return N.GetOutEDat(GetNIdPos(N.OutNIdV, DstNId));
}

template <class TNodeData, class TEdgeData>
const TEdgeData& TNodeEDatNet<TNodeData, TEdgeData>::GetEDat(const int& SrcNId, const int& DstNId) const {
  Assert(IsEdge(SrcNId, DstNId));
  const TNode& N = GetNode(SrcNId);
  return N.GetOutEDat(GetNIdPos(N.OutNIdV, DstNId));
}

template <class TNodeData, class TEdgeData>
void TNodeEDatNet<TNodeData, TEdgeData>::SetAllEDat(const TEdgeData& EdgeDat) {
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    EI() = EdgeDat;
  }
}

template <class TNodeData, class TEdgeData>
typename TNodeEDatNet<TNodeData, TEdgeData>::TEdgeI  TNodeEDatNet<TNodeData, TEdgeData>::GetEI(const int& SrcNId, const int& DstNId) const {
  const TNodeI SrcNI = GetNI(SrcNId);
  int NodeN = -1;
  //SrcNI.NodeHI.GetDat().OutNIdV.SearchBin(DstNId);
  const TNIdDatPrV& NIdDatV = SrcNI.NodeHI.GetDat().OutNIdV;
  int LValN=0, RValN=NIdDatV.Len()-1;
  while (RValN>=LValN){
    int ValN=(LValN+RValN)/2;
    if (DstNId==NIdDatV[ValN].Val1){ NodeN=ValN; break; }
    if (DstNId<NIdDatV[ValN].Val1){RValN=ValN-1;} else {LValN=ValN+1;}
  }
  if (NodeN == -1) { return EndEI(); }
  else { return TEdgeI(SrcNI, EndNI(), NodeN); }
}

template <class TNodeData, class TEdgeData>
void TNodeEDatNet<TNodeData, TEdgeData>::GetNIdV(TIntV& NIdV) const {
  NIdV.Reserve(GetNodes(), 0);
  for (int N=NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    NIdV.Add(NodeH.GetKey(N)); }
}

template <class TNodeData, class TEdgeData>
void TNodeEDatNet<TNodeData, TEdgeData>::Defrag(const bool& OnlyNodeLinks) {
  for (int n = NodeH.FFirstKeyId(); NodeH.FNextKeyId(n);) {
    TNode& Node = NodeH[n];
    Node.InNIdV.Pack();  Node.OutNIdV.Pack();
  }
  if (! OnlyNodeLinks && ! NodeH.IsKeyIdEqKeyN()) {
    NodeH.Defrag();
  }
}

template <class TNodeData, class TEdgeData>
bool TNodeEDatNet<TNodeData, TEdgeData>::IsOk(const bool& ThrowExcept) const {
  bool RetVal = true;
  for (int N = NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    const TNode& Node = NodeH[N];
    if (! Node.OutNIdV.IsSorted()) {
      const TStr Msg = TStr::Fmt("Out-neighbor list of node %d is not sorted.", Node.GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
    if (! Node.InNIdV.IsSorted()) {
      const TStr Msg = TStr::Fmt("In-neighbor list of node %d is not sorted.", Node.GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
    // check out-edges
    int prevNId = -1;
    for (int e = 0; e < Node.GetOutDeg(); e++) {
      if (! IsNode(Node.GetOutNId(e))) {
        const TStr Msg = TStr::Fmt("Out-edge %d --> %d: node %d does not exist.",
          Node.GetId(), Node.GetOutNId(e), Node.GetOutNId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      if (e > 0 && prevNId == Node.GetOutNId(e)) {
        const TStr Msg = TStr::Fmt("Node %d has duplidate out-edge %d --> %d.",
          Node.GetId(), Node.GetId(), Node.GetOutNId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      prevNId = Node.GetOutNId(e);
    }
    // check in-edges
    prevNId = -1;
    for (int e = 0; e < Node.GetInDeg(); e++) {
      if (! IsNode(Node.GetInNId(e))) {
        const TStr Msg = TStr::Fmt("In-edge %d <-- %d: node %d does not exist.",
          Node.GetId(), Node.GetInNId(e), Node.GetInNId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      if (e > 0 && prevNId == Node.GetInNId(e)) {
        const TStr Msg = TStr::Fmt("Node %d has duplidate in-edge %d <-- %d.",
          Node.GetId(), Node.GetId(), Node.GetInNId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      prevNId = Node.GetInNId(e);
    }
  }
  return RetVal;
}

/////////////////////////////////////////////////
// Common network types
typedef TNodeEDatNet<TInt, TInt> TIntNEDNet;
typedef TPt<TIntNEDNet> PIntNEDNet;
typedef TNodeEDatNet<TInt, TFlt> TIntFltNEDNet;
typedef TPt<TIntFltNEDNet> PIntFltNEDNet;
typedef TNodeEDatNet<TStr, TInt> TStrIntNEDNet;
typedef TPt<TStrIntNEDNet> PStrIntNEDNet;

//#//////////////////////////////////////////////
/// Node Edge Network (directed multigraph, TNEGraph with data on nodes and edges).
template <class TNodeData, class TEdgeData>
class TNodeEdgeNet {
public:
  typedef TNodeData TNodeDat;
  typedef TEdgeData TEdgeDat;
  typedef TNodeEdgeNet<TNodeData, TEdgeData> TNet;
  typedef TPt<TNet> PNet;
public:
  class TNode {
  private:
    TInt Id;
    TIntV InEIdV, OutEIdV;
    TNodeData NodeDat;
  public:
    TNode() : Id(-1), InEIdV(), OutEIdV(), NodeDat() { }
    TNode(const int& NId) : Id(NId), InEIdV(), OutEIdV(), NodeDat()  { }
    TNode(const int& NId, const TNodeData& NodeData) : Id(NId), InEIdV(), OutEIdV(), NodeDat(NodeData) { }
    TNode(const TNode& Node) : Id(Node.Id), InEIdV(Node.InEIdV), OutEIdV(Node.OutEIdV), NodeDat(Node.NodeDat) { }
    TNode(TSIn& SIn) : Id(SIn), InEIdV(SIn), OutEIdV(SIn), NodeDat(SIn) { }
    void Save(TSOut& SOut) const { Id.Save(SOut);  InEIdV.Save(SOut);  OutEIdV.Save(SOut);  NodeDat.Save(SOut); }
    bool operator < (const TNode& Node) const { return NodeDat < Node.NodeDat; }
    int GetId() const { return Id; }
    int GetDeg() const { return GetInDeg() + GetOutDeg(); }
    int GetInDeg() const { return InEIdV.Len(); }
    int GetOutDeg() const { return OutEIdV.Len(); }
    const TNodeData& GetDat() const { return NodeDat; }
    TNodeData& GetDat() { return NodeDat; }
    int GetInEId(const int& NodeN) const { return InEIdV[NodeN]; }
    int GetOutEId(const int& NodeN) const { return OutEIdV[NodeN]; }
    int GetNbrEId(const int& EdgeN) const { return EdgeN<GetOutDeg()?GetOutEId(EdgeN):GetInEId(EdgeN-GetOutDeg()); }
    bool IsInEId(const int& EId) const { return InEIdV.SearchBin(EId) != -1; }
    bool IsOutEId(const int& EId) const { return OutEIdV.SearchBin(EId) != -1; }
    bool IsNbrEId(const int& EId) const { return IsInEId(EId) || IsOutEId(EId); }
    void LoadShM(TShMIn& MStream) {
      Id = TInt(MStream);
      InEIdV.LoadShM(MStream);
      OutEIdV.LoadShM(MStream);
      NodeDat = TNodeData(MStream);
    }
    friend class TNodeEdgeNet<TNodeData, TEdgeData>;
  };

  class TEdge {
  private:
    TInt Id, SrcNId, DstNId;
    TEdgeData EdgeDat;
  public:
    TEdge() : Id(-1), SrcNId(-1), DstNId(-1), EdgeDat() { }
    TEdge(const int& EId, const int& SourceNId, const int& DestNId) : Id(EId), SrcNId(SourceNId), DstNId(DestNId), EdgeDat() { }
    TEdge(const int& EId, const int& SourceNId, const int& DestNId, const TEdgeData& EdgeData) : Id(EId), SrcNId(SourceNId), DstNId(DestNId), EdgeDat(EdgeData) { }
    TEdge(const TEdge& Edge) : Id(Edge.Id), SrcNId(Edge.SrcNId), DstNId(Edge.DstNId), EdgeDat(Edge.EdgeDat) { }
    TEdge(TSIn& SIn) : Id(SIn), SrcNId(SIn), DstNId(SIn), EdgeDat(SIn) { }
    void Save(TSOut& SOut) const { Id.Save(SOut);  SrcNId.Save(SOut);  DstNId.Save(SOut);  EdgeDat.Save(SOut); }
    bool operator < (const TEdge& Edge) const { return EdgeDat < Edge.EdgeDat; }
    int GetId() const { return Id; }
    int GetSrcNId() const { return SrcNId; }
    int GetDstNId() const { return DstNId; }
    void Load(TSIn& InStream) {
      Id = TInt(InStream);
      SrcNId = TInt(InStream);
      DstNId = TInt(InStream);
      EdgeDat = TEdgeData(InStream);

    }
    const TEdgeData& GetDat() const { return EdgeDat; }
    TEdgeData& GetDat() { return EdgeDat; }
    friend class TNodeEdgeNet;
  };

  /// Node iterator. Only forward iteration (operator++) is supported.
  class TNodeI {
  private:
    typedef typename THash<TInt, TNode>::TIter THashIter;
    THashIter NodeHI;
    TNodeEdgeNet *Net;
  public:
    TNodeI() : NodeHI(), Net(NULL) { }
    TNodeI(const THashIter& NodeHIter, const TNodeEdgeNet* NetPt) : NodeHI(NodeHIter), Net((TNodeEdgeNet *)NetPt) { }
    TNodeI(const TNodeI& NodeI) : NodeHI(NodeI.NodeHI), Net(NodeI.Net) { }
    TNodeI& operator = (const TNodeI& NodeI) { NodeHI = NodeI.NodeHI;  Net=NodeI.Net;  return *this; }
    /// Increment iterator.
    TNodeI& operator++ (int) { NodeHI++;  return *this; }
    bool operator < (const TNodeI& NodeI) const { return NodeHI < NodeI.NodeHI; }
    bool operator == (const TNodeI& NodeI) const { return NodeHI == NodeI.NodeHI; }
    /// Returns ID of the current node.
    int GetId() const { return NodeHI.GetDat().GetId(); }
    /// Returns degree of the current node, the sum of in-degree and out-degree.
    int GetDeg() const { return NodeHI.GetDat().GetDeg(); }
    /// Returns in-degree of the current node.
    int GetInDeg() const { return NodeHI.GetDat().GetInDeg(); }
    /// Returns out-degree of the current node.
    int GetOutDeg() const { return NodeHI.GetDat().GetOutDeg(); }
    /// Returns ID of EdgeN-th in-node (the node pointing to the current node). ##TNodeEdgeNet::TNodeI::GetInNId
    int GetInNId(const int& EdgeN) const { return Net->GetEdge(NodeHI.GetDat().GetInEId(EdgeN)).GetSrcNId(); }
    /// Returns ID of EdgeN-th out-node (the node the current node points to). ##TNodeEdgeNet::TNodeI::GetOutNId
    int GetOutNId(const int& EdgeN) const { return Net->GetEdge(NodeHI.GetDat().GetOutEId(EdgeN)).GetDstNId(); }
    /// Returns ID of EdgeN-th neighboring node. ##TNodeEdgeNet::TNodeI::GetNbrNId
    int GetNbrNId(const int& EdgeN) const { const TEdge& E = Net->GetEdge(NodeHI.GetDat().GetNbrEId(EdgeN));
      return GetId()==E.GetSrcNId() ? E.GetDstNId():E.GetSrcNId(); }
    /// Tests whether node with ID NId points to the current node.
    bool IsInNId(const int& NId) const;
    /// Tests whether the current node points to node with ID NId.
    bool IsOutNId(const int& NId) const;
    /// Tests whether node with ID NId is a neighbor of the current node.
    bool IsNbrNId(const int& NId) const { return IsOutNId(NId) || IsInNId(NId); }
    // node data
    const TNodeData& operator () () const { return NodeHI.GetDat().GetDat(); }
    TNodeData& operator () () { return NodeHI.GetDat().GetDat(); }
    const TNodeData& GetDat() const { return NodeHI.GetDat().GetDat(); }
    TNodeData& GetDat() { return NodeHI.GetDat().GetDat(); }
    const TNodeData& GetInNDat(const int& EdgeN) const { return Net->GetNDat(GetInNId(EdgeN)); }
    TNodeData& GetInNDat(const int& EdgeN) { return Net->GetNDat(GetInNId(EdgeN)); }
    const TNodeData& GetOutNDat(const int& EdgeN) const { return Net->GetNDat(GetOutNId(EdgeN)); }
    TNodeData& GetOutNDat(const int& EdgeN) { return Net->GetNDat(GetOutNId(EdgeN)); }
    const TNodeData& GetNbrNDat(const int& EdgeN) const { return Net->GetNDat(GetNbrNId(EdgeN)); }
    TNodeData& GetNbrNDat(const int& EdgeN) { return Net->GetNDat(GetNbrNId(EdgeN)); }
    // edges
    /// Returns ID of EdgeN-th in-edge.
    int GetInEId(const int& EdgeN) const { return NodeHI.GetDat().GetInEId(EdgeN); }
    /// Returns ID of EdgeN-th out-edge.
    int GetOutEId(const int& EdgeN) const { return NodeHI.GetDat().GetOutEId(EdgeN); }
    /// Returns ID of EdgeN-th in or out-edge.
    int GetNbrEId(const int& EdgeN) const { return NodeHI.GetDat().GetNbrEId(EdgeN); }
    /// Tests whether the edge with ID EId is an in-edge of current node.
    bool IsInEId(const int& EId) const { return NodeHI.GetDat().IsInEId(EId); }
    /// Tests whether the edge with ID EId is an out-edge of current node.
    bool IsOutEId(const int& EId) const { return NodeHI.GetDat().IsOutEId(EId); }
    /// Tests whether the edge with ID EId is an in or out-edge of current node.
    bool IsNbrEId(const int& EId) const { return NodeHI.GetDat().IsNbrEId(EId); }
    // edge data
    TEdgeDat& GetInEDat(const int& EdgeN) { return Net->GetEDat(GetInEId(EdgeN)); }
    const TEdgeDat& GetInEDat(const int& EdgeN) const { return Net->GetEDat(GetInEId(EdgeN)); }
    TEdgeDat& GetOutEDat(const int& EdgeN) { return Net->GetEDat(GetOutEId(EdgeN)); }
    const TEdgeDat& GetOutEDat(const int& EdgeN) const { return Net->GetEDat(GetOutEId(EdgeN)); }
    TEdgeDat& GetNbrEDat(const int& EdgeN) { return Net->GetEDat(GetNbrEId(EdgeN)); }
    const TEdgeDat& GetNbrEDat(const int& EdgeN) const { return Net->GetEDat(GetNbrEId(EdgeN)); }
    friend class TNodeEdgeNet;
  };

  /// Edge iterator. Only forward iteration (operator++) is supported.
  class TEdgeI {
  private:
    typedef typename THash<TInt, TEdge>::TIter THashIter;
    THashIter EdgeHI;
    TNodeEdgeNet *Net;
  public:
    TEdgeI() : EdgeHI(), Net(NULL) { }
    TEdgeI(const THashIter& EdgeHIter, const TNodeEdgeNet *NetPt) : EdgeHI(EdgeHIter), Net((TNodeEdgeNet *) NetPt) { }
    TEdgeI(const TEdgeI& EdgeI) : EdgeHI(EdgeI.EdgeHI), Net(EdgeI.Net) { }
    TEdgeI& operator = (const TEdgeI& EdgeI) { if (this!=&EdgeI) { EdgeHI=EdgeI.EdgeHI;  Net=EdgeI.Net; }  return *this; }
    TEdgeI& operator++ (int) { EdgeHI++;  return *this; }
    bool operator < (const TEdgeI& EdgeI) const { return EdgeHI < EdgeI.EdgeHI; }
    bool operator == (const TEdgeI& EdgeI) const { return EdgeHI == EdgeI.EdgeHI; }
    /// Gets edge ID.
    int GetId() const { return EdgeHI.GetDat().GetId(); }
    /// Gets the source of an edge.
    int GetSrcNId() const { return EdgeHI.GetDat().GetSrcNId(); }
    /// Gets destination of an edge.
    int GetDstNId() const { return EdgeHI.GetDat().GetDstNId(); }
    const TEdgeData& operator () () const { return EdgeHI.GetDat().GetDat(); }
    TEdgeData& operator () () { return EdgeHI.GetDat().GetDat(); }
    const TEdgeData& GetDat() const { return EdgeHI.GetDat().GetDat(); }
    TEdgeData& GetDat() { return EdgeHI.GetDat().GetDat(); }
    const TNodeData& GetSrcNDat() const { return Net->GetNDat(GetSrcNId()); }
    TNodeData& GetSrcNDat() { return Net->GetNDat(GetSrcNId()); }
    const TNodeData& GetDstNDat() const { return Net->GetNDat(GetDstNId()); }
    TNodeData& GetDstNDat() { return Net->GetNDat(GetDstNId()); }
    friend class TNodeEdgeNet;
  };

private:
  TNode& GetNode(const int& NId) { return NodeH.GetDat(NId); }
  const TNode& GetNode(const int& NId) const { return NodeH.GetDat(NId); }
  const TNode& GetNodeKId(const int& NodeKeyId) const { return NodeH[NodeKeyId]; }
  TEdge& GetEdge(const int& EId) { return EdgeH.GetDat(EId); }
  const TEdge& GetEdge(const int& EId) const { return EdgeH.GetDat(EId); }
  const TEdge& GetEdgeKId(const int& EdgeKeyId) const { return EdgeH[EdgeKeyId]; }
protected:
  TCRef CRef;
  TInt MxNId, MxEId;
  THash<TInt, TNode> NodeH;
  THash<TInt, TEdge> EdgeH;
private:
  class LoadTNodeFunctor {
  public:
    LoadTNodeFunctor() {}
    void operator() (TNode* n, TShMIn& ShMIn) { n->LoadShM(ShMIn);}
  };
private:
  void LoadNetworkShM(TShMIn& ShMIn) {
    MxNId = TInt(ShMIn);
    MxEId = TInt(ShMIn);
    LoadTNodeFunctor fn;
    NodeH.LoadShM(ShMIn, fn);
    EdgeH.LoadShM(ShMIn);
  }
public:
  TNodeEdgeNet() : CRef(), MxNId(0), MxEId(0) { }
  /// Constructor that reserves enough memory for a network of Nodes nodes and Edges edges.
  explicit TNodeEdgeNet(const int& Nodes, const int& Edges) : CRef(), MxNId(0), MxEId(0) { Reserve(Nodes, Edges); }
  TNodeEdgeNet(const TNodeEdgeNet& Net) : MxNId(Net.MxNId), MxEId(Net.MxEId), NodeH(Net.NodeH), EdgeH(Net.EdgeH) { }
  /// Constructor that loads the network from a (binary) stream SIn.
  TNodeEdgeNet(TSIn& SIn) : MxNId(SIn), MxEId(SIn), NodeH(SIn), EdgeH(SIn) { }
  virtual ~TNodeEdgeNet() { }
  /// Saves the network to a (binary) stream SOut.
  virtual void Save(TSOut& SOut) const { MxNId.Save(SOut);  MxEId.Save(SOut);  NodeH.Save(SOut);  EdgeH.Save(SOut); }
  /// Static constructor that returns a pointer to the network. ##TNodeEdgeNet::New()
  static PNet New() { return PNet(new TNet()); }
  /// Static constructor that loads the network from a stream SIn and returns a pointer to it.
  static PNet Load(TSIn& SIn) { return PNet(new TNet(SIn)); }
  /// Static constructor that loads the network from memory. ##TNodeEdgeNet::LoadShM(TShMIn& ShMIn)
  static PNet LoadShM(TShMIn& ShMIn) {
    TNet* Network = new TNet();
    Network->LoadNetworkShM(ShMIn);
    return PNet(Network);
  }
  /// Allows for run-time checking the type of the network (see the TGraphFlag for flags).
  bool HasFlag(const TGraphFlag& Flag) const;
  TNodeEdgeNet& operator = (const TNodeEdgeNet& Net) {
    if (this!=&Net) { NodeH=Net.NodeH; EdgeH=Net.EdgeH; MxNId=Net.MxNId; MxEId=Net.MxEId; }  return *this; }
  // nodes
  /// Returns the number of nodes in the network.
  int GetNodes() const { return NodeH.Len(); }
  /// Adds a node of ID NId to the network. ##TNodeEdgeNet::AddNode
  int AddNode(int NId = -1);
  /// Adds a node of ID NId to the network, noop if the node already exists. ##TNodeEdgeNet::AddNodeUnchecked
  int AddNodeUnchecked(int NId = -1);
  /// Adds node data to node with ID NId. ##TNodeEdgeNet::AddNode-1
  int AddNode(int NId, const TNodeData& NodeDat);
  /// Adds a node NodeI and its node data to the network.
  friend class TCrossNet;
  int AddNode(const TNodeI& NodeI) { return AddNode(NodeI.GetId(), NodeI.GetDat()); }
  /// Deletes node of ID NId from the network. ##TNodeEdgeNet::DelNode
  void DelNode(const int& NId);
  /// Deletes node of ID NodeI.GetId() from the network.
  void DelNode(const TNode& NodeI) { DelNode(NodeI.GetId()); }
  /// Tests whether ID NId is a node.
  bool IsNode(const int& NId) const { return NodeH.IsKey(NId); }
  /// Returns an iterator referring to the first node in the network.
  TNodeI BegNI() const { return TNodeI(NodeH.BegI(), this); }
  /// Returns an iterator referring to the past-the-end node in the network.
  TNodeI EndNI() const { return TNodeI(NodeH.EndI(), this); }
  /// Returns an iterator referring to the node of ID NId in the network.
  TNodeI GetNI(const int& NId) const { return TNodeI(NodeH.GetI(NId), this); }
  /// Sets node data for the node of ID NId in the network.
  void SetNDat(const int& NId, const TNodeData& NodeDat);
  /// Returns node data for the node of ID NId in the network.
  TNodeData& GetNDat(const int& NId) { return NodeH.GetDat(NId).NodeDat; }
  /// Returns node data for the node of ID NId in the network.
  const TNodeData& GetNDat(const int& NId) const { return NodeH.GetDat(NId).NodeDat; }
  /// Returns an ID that is larger than any node ID in the network.
  int GetMxNId() const { return MxNId; }

  // edges
  /// Returns the number of edges in the network.
  int GetEdges() const { return EdgeH.Len(); }
  /// Returns the number of edges in the network with a unique pair of nodes.
  int GetUniqEdges(const bool& IsDir=true) const;
  /// Adds an edge between node IDs SrcNId and DstNId to the graph. ##TNodeEdgeNet::AddEdge
  int AddEdge(const int& SrcNId, const int& DstNId, int EId = -1);
  /// Adds an edge and edge data from node IDs SrcNId to node DstNId. ##TNodeEdgeNet::AddEdge-1
  int AddEdge(const int& SrcNId, const int& DstNId, int EId, const TEdgeData& EdgeDat);
  /// Adds an edge from EdgeI.GetSrcNId() to EdgeI.GetDstNId() and its edge data to the network.
  int AddEdge(const TEdgeI& EdgeI) { return AddEdge(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), EdgeI.GetId(), EdgeI.GetDat()); }
  /// Deletes an edge with ID EId from the network.
  void DelEdge(const int& EId);
  /// Deletes an edge from node IDs SrcNId to DstNId from the network. ##TNodeEDatNet::DelEdge
  void DelEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true);
  /// Tests whether an edge with ID EId exists in the network.
  bool IsEdge(const int& EId) const { return EdgeH.IsKey(EId); }
  /// Tests whether an edge from node IDs SrcNId to DstNId exists in the network.
  bool IsEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true) const { int EId;  return IsEdge(SrcNId, DstNId, EId, IsDir); }
  /// Tests whether an edge from node IDs SrcNId to DstNId with edge ID EId exists in the network.
  bool IsEdge(const int& SrcNId, const int& DstNId, int& EId, const bool& IsDir = true) const;
  int GetEId(const int& SrcNId, const int& DstNId) const { int EId; return IsEdge(SrcNId, DstNId, EId)?EId:-1; }
  /// Returns an iterator referring to the first edge in the network.
  TEdgeI BegEI() const { return TEdgeI(EdgeH.BegI(), this); }
  /// Returns an iterator referring to the past-the-end edge in the network.
  TEdgeI EndEI() const { return TEdgeI(EdgeH.EndI(), this); }
  /// Not supported/implemented!
  TEdgeI GetEI(const int& EId) const { return TEdgeI(EdgeH.GetI(EId), this); }
  /// Returns an iterator referring to edge (SrcNId, DstNId) in the graph.
  TEdgeI GetEI(const int& SrcNId, const int& DstNId) const { return GetEI(GetEId(SrcNId, DstNId)); }
  /// Sets edge data for the edge of ID NId in the network.
  void SetEDat(const int& EId, const TEdgeData& EdgeDat);
  /// Returns edge data for the edge with ID EId.
  TEdgeData& GetEDat(const int& EId) { return EdgeH.GetDat(EId).EdgeDat; }
  /// Returns edge data for the edge with ID EId.
  const TEdgeData& GetEDat(const int& EId) const { return EdgeH.GetDat(EId).EdgeDat; }
  /// Sets edge data for all the edges in the network to EDat.
  void SetAllEDat(const TEdgeData& EdgeDat);

  /// Returns an ID of a random node in the network.
  int GetRndNId(TRnd& Rnd=TInt::Rnd) { return NodeH.GetKey(NodeH.GetRndKeyId(Rnd, 0.8)); }
  /// Returns an interator referring to a random node in the network.
  TNodeI GetRndNI(TRnd& Rnd=TInt::Rnd) { return GetNI(GetRndNId(Rnd)); }
  /// Returns an ID of a random edge in the network.
  int GetRndEId(TRnd& Rnd=TInt::Rnd) { return EdgeH.GetKey(EdgeH.GetRndKeyId(Rnd, 0.8)); }
  /// Returns an interator referring to a random edge in the network.
  TEdgeI GetRndEI(TRnd& Rnd=TInt::Rnd) { return GetEI(GetRndEId(Rnd)); }
  /// Returns a vector of all node IDs in the network.
  void GetNIdV(TIntV& NIdV) const;
  /// Returns a vector of all edge IDs in the network.
  void GetEIdV(TIntV& EIdV) const;

  /// Tests whether the network is empty (has zero nodes).
  bool Empty() const { return GetNodes()==0; }
  /// Deletes all nodes and edges from the network.
  void Clr() { MxNId=0;  MxEId=0;  NodeH.Clr();  EdgeH.Clr(); }
  /// Reserves memory for a network of Nodes nodes and Edges edges.
  void Reserve(const int& Nodes, const int& Edges) {
    if (Nodes>0) { NodeH.Gen(Nodes/2); }  if (Edges>0) { EdgeH.Gen(Edges/2); } }
  /// Sorts nodes by node IDs.
  void SortNIdById(const bool& Asc=true) { NodeH.SortByKey(Asc); }
  /// Sorts nodes by node data.
  void SortNIdByDat(const bool& Asc=true) { NodeH.SortByDat(Asc); }
  /// Sorts edges by edge IDs.
  void SortEIdById(const bool& Asc=true) { EdgeH.SortByKey(Asc); }
  /// Sorts edges by edge data.
  void SortEIdByDat(const bool& Asc=true) { EdgeH.SortByDat(Asc); }
  /// Defragments the network. ##TNodeEdgeNet::Defrag
  void Defrag(const bool& OnlyNodeLinks=false);
  /// Checks the network data structure for internal consistency. ##TNodeEdgeNet::IsOk
  bool IsOk(const bool& ThrowExcept=true) const;

  friend class TPt<TNodeEdgeNet<TNodeData, TEdgeData> >;
};

// set flags
namespace TSnap {
template <class TNodeData, class TEdgeData> struct IsMultiGraph<TNodeEdgeNet<TNodeData, TEdgeData> > { enum { Val = 1 }; };
template <class TNodeData, class TEdgeData> struct IsDirected<TNodeEdgeNet<TNodeData, TEdgeData> > { enum { Val = 1 }; };
template <class TNodeData, class TEdgeData> struct IsNodeDat<TNodeEdgeNet<TNodeData, TEdgeData> > { enum { Val = 1 }; };
template <class TNodeData, class TEdgeData> struct IsEdgeDat<TNodeEdgeNet<TNodeData, TEdgeData> > { enum { Val = 1 }; };
}

template <class TNodeData, class TEdgeData>
bool TNodeEdgeNet<TNodeData, TEdgeData>::HasFlag(const TGraphFlag& Flag) const {
  return HasGraphFlag(typename TNet, Flag);
}

template <class TNodeData, class TEdgeData>
bool TNodeEdgeNet<TNodeData, TEdgeData>::TNodeI::IsInNId(const int& NId) const {
  const TNode& Node = NodeHI.GetDat();
  for (int edge = 0; edge < Node.GetInDeg(); edge++) {
    if (NId == Net->GetEdge(Node.GetInEId(edge)).GetSrcNId())
      return true;
  }
  return false;
}

template <class TNodeData, class TEdgeData>
bool TNodeEdgeNet<TNodeData, TEdgeData>::TNodeI::IsOutNId(const int& NId) const {
  const TNode& Node = NodeHI.GetDat();
  for (int edge = 0; edge < Node.GetOutDeg(); edge++) {
    if (NId == Net->GetEdge(Node.GetOutEId(edge)).GetDstNId())
      return true;
  }
  return false;
}

template <class TNodeData, class TEdgeData>
int TNodeEdgeNet<TNodeData, TEdgeData>::AddNode(int NId) {
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    MxNId = TMath::Mx(NId+1, MxNId());
  }
  NodeH.AddDat(NId, TNode(NId));
  return NId;
}

template <class TNodeData, class TEdgeData>
int TNodeEdgeNet<TNodeData, TEdgeData>::AddNodeUnchecked(int NId) {
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    if (IsNode(NId)) { return -1;}
    MxNId = TMath::Mx(NId+1, MxNId());
  }
  NodeH.AddDat(NId, TNode(NId));
  return NId;
}

template <class TNodeData, class TEdgeData>
int TNodeEdgeNet<TNodeData, TEdgeData>::AddNode(int NId, const TNodeData& NodeDat) {
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    MxNId = TMath::Mx(NId+1, MxNId());
  }
  NodeH.AddDat(NId, TNode(NId, NodeDat));
  return NId;
}

template <class TNodeData, class TEdgeData>
void TNodeEdgeNet<TNodeData, TEdgeData>::DelNode(const int& NId) {
  const TNode& Node = GetNode(NId);
  for (int out = 0; out < Node.GetOutDeg(); out++) {
    const int EId = Node.GetOutEId(out);
    const TEdge& Edge = GetEdge(EId);
    IAssert(Edge.GetSrcNId() == NId);
    GetNode(Edge.GetDstNId()).InEIdV.DelIfIn(EId);
    EdgeH.DelKey(EId);
  }
  for (int in = 0; in < Node.GetInDeg(); in++) {
    const int EId = Node.GetInEId(in);
    const TEdge& Edge = GetEdge(EId);
    IAssert(Edge.GetDstNId() == NId);
    GetNode(Edge.GetSrcNId()).OutEIdV.DelIfIn(EId);
    EdgeH.DelKey(EId);
  }
  NodeH.DelKey(NId);
}

template <class TNodeData, class TEdgeData>
void TNodeEdgeNet<TNodeData, TEdgeData>::SetNDat(const int& NId, const TNodeData& NodeDat) {
  IAssertR(IsNode(NId), TStr::Fmt("NodeId %d does not exist.", NId).CStr());
  NodeH.GetDat(NId).NodeDat = NodeDat;
}

template <class TNodeData, class TEdgeData>
int TNodeEdgeNet<TNodeData, TEdgeData>::GetUniqEdges(const bool& IsDir) const {
  TIntPrSet UniqESet(GetEdges());
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    const int Src = EI.GetSrcNId();
    const int Dst = EI.GetDstNId();
    if (IsDir) { UniqESet.AddKey(TIntPr(Src, Dst)); }
    else { UniqESet.AddKey(TIntPr(TMath::Mn(Src, Dst), TMath::Mx(Src, Dst))); }
  }
  return UniqESet.Len();
}

template <class TNodeData, class TEdgeData>
int TNodeEdgeNet<TNodeData, TEdgeData>::AddEdge(const int& SrcNId, const int& DstNId, int EId) {
  if (EId == -1) { EId = MxEId;  MxEId++; }
  else { MxEId = TMath::Mx(EId+1, MxEId()); }
  IAssertR(!IsEdge(EId), TStr::Fmt("EdgeId %d already exists", EId));
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  EdgeH.AddDat(EId, TEdge(EId, SrcNId, DstNId));
  GetNode(SrcNId).OutEIdV.AddSorted(EId);
  GetNode(DstNId).InEIdV.AddSorted(EId);
  return EId;
}

template <class TNodeData, class TEdgeData>
int TNodeEdgeNet<TNodeData, TEdgeData>::AddEdge(const int& SrcNId, const int& DstNId, int EId, const TEdgeData& EdgeDat) {
  if (EId == -1) { EId = MxEId;  MxEId++; }
  else { MxEId = TMath::Mx(EId+1, MxEId()); }
  IAssertR(!IsEdge(EId), TStr::Fmt("EdgeId %d already exists", EId));
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  EdgeH.AddDat(EId, TEdge(EId, SrcNId, DstNId, EdgeDat));
  GetNode(SrcNId).OutEIdV.AddSorted(EId);
  GetNode(DstNId).InEIdV.AddSorted(EId);
  return EId;
}

template <class TNodeData, class TEdgeData>
void TNodeEdgeNet<TNodeData, TEdgeData>::DelEdge(const int& EId) {
  IAssert(IsEdge(EId));
  const int SrcNId = GetEdge(EId).GetSrcNId();
  const int DstNId = GetEdge(EId).GetDstNId();
  GetNode(SrcNId).OutEIdV.DelIfIn(EId);
  GetNode(DstNId).InEIdV.DelIfIn(EId);
  EdgeH.DelKey(EId);
}

template <class TNodeData, class TEdgeData>
void TNodeEdgeNet<TNodeData, TEdgeData>::DelEdge(const int& SrcNId, const int& DstNId, const bool& IsDir) {
  int EId;
  IAssert(IsEdge(SrcNId, DstNId, EId, IsDir));
  GetNode(SrcNId).OutEIdV.DelIfIn(EId);
  GetNode(DstNId).InEIdV.DelIfIn(EId);
  EdgeH.DelKey(EId);
}

template <class TNodeData, class TEdgeData>
bool TNodeEdgeNet<TNodeData, TEdgeData>::IsEdge(const int& SrcNId, const int& DstNId, int& EId, const bool& IsDir) const {
  if (! IsNode(SrcNId)) { return false; }
  if (! IsNode(DstNId)) { return false; }
  const TNode& SrcNode = GetNode(SrcNId);
  for (int edge = 0; edge < SrcNode.GetOutDeg(); edge++) {
    const TEdge& Edge = GetEdge(SrcNode.GetOutEId(edge));
    if (DstNId == Edge.GetDstNId()) {
      EId = Edge.GetId();  return true; }
  }
  if (! IsDir) {
    for (int edge = 0; edge < SrcNode.GetInDeg(); edge++) {
    const TEdge& Edge = GetEdge(SrcNode.GetInEId(edge));
    if (DstNId == Edge.GetSrcNId()) {
      EId = Edge.GetId();  return true; }
    }
  }
  return false;
}

template <class TNodeData, class TEdgeData>
void TNodeEdgeNet<TNodeData, TEdgeData>::SetEDat(const int& EId, const TEdgeData& EdgeDat) {
  IAssertR(IsEdge(EId), TStr::Fmt("EdgeId %d does not exist.", EId).CStr());
  GetEI(EId).GetDat() = EdgeDat;
}

template <class TNodeData, class TEdgeData>
void TNodeEdgeNet<TNodeData, TEdgeData>::SetAllEDat(const TEdgeData& EdgeDat) {
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    EI() = EdgeDat;
  }
}


template <class TNodeData, class TEdgeData>
void TNodeEdgeNet<TNodeData, TEdgeData>::GetNIdV(TIntV& NIdV) const {
  NIdV.Gen(GetNodes(), 0);
  for (int N=NodeH.FFirstKeyId(); NodeH.FNextKeyId(N);) {
    NIdV.Add(NodeH.GetKey(N));
  }
}

template <class TNodeData, class TEdgeData>
void TNodeEdgeNet<TNodeData, TEdgeData>::GetEIdV(TIntV& EIdV) const {
  EIdV.Gen(GetEdges(), 0);
  for (int E=EdgeH.FFirstKeyId(); EdgeH.FNextKeyId(E);) {
    EIdV.Add(EdgeH.GetKey(E));
  }
}

template <class TNodeData, class TEdgeData>
void TNodeEdgeNet<TNodeData, TEdgeData>::Defrag(const bool& OnlyNodeLinks) {
  for (int kid = NodeH.FFirstKeyId(); NodeH.FNextKeyId(kid);) {
    TNode& Node = NodeH[kid];
    Node.InEIdV.Pack();  Node.OutEIdV.Pack();
  }
  if (! OnlyNodeLinks && ! NodeH.IsKeyIdEqKeyN()) { NodeH.Defrag(); }
  if (! OnlyNodeLinks && ! EdgeH.IsKeyIdEqKeyN()) { EdgeH.Defrag(); }
}

template <class TNodeData, class TEdgeData>
bool TNodeEdgeNet<TNodeData, TEdgeData>::IsOk(const bool& ThrowExcept) const {
  bool RetVal = true;
  for (int N = NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    const TNode& Node = NodeH[N];
    if (! Node.OutEIdV.IsSorted()) {
      const TStr Msg = TStr::Fmt("Out-edge list of node %d is not sorted.", Node.GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
    if (! Node.InEIdV.IsSorted()) {
      const TStr Msg = TStr::Fmt("In-edge list of node %d is not sorted.", Node.GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
    // check out-edge ids
    int prevEId = -1;
    for (int e = 0; e < Node.GetOutDeg(); e++) {
      if (! IsEdge(Node.GetOutEId(e))) {
        const TStr Msg = TStr::Fmt("Out-edge id %d of node %d does not exist.",  Node.GetOutEId(e), Node.GetId());
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      if (e > 0 && prevEId == Node.GetOutEId(e)) {
        const TStr Msg = TStr::Fmt("Node %d has duplidate out-edge id %d.", Node.GetId(), Node.GetOutEId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      prevEId = Node.GetOutEId(e);
    }
    // check in-edge ids
    prevEId = -1;
    for (int e = 0; e < Node.GetInDeg(); e++) {
      if (! IsEdge(Node.GetInEId(e))) {
        const TStr Msg = TStr::Fmt("Out-edge id %d of node %d does not exist.",  Node.GetInEId(e), Node.GetId());
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      if (e > 0 && prevEId == Node.GetInEId(e)) {
        const TStr Msg = TStr::Fmt("Node %d has duplidate out-edge id %d.", Node.GetId(), Node.GetInEId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      prevEId = Node.GetInEId(e);
    }
  }
  for (int E = EdgeH.FFirstKeyId(); EdgeH.FNextKeyId(E); ) {
    const TEdge& Edge = EdgeH[E];
    if (! IsNode(Edge.GetSrcNId())) {
      const TStr Msg = TStr::Fmt("Edge %d source node %d does not exist.", Edge.GetId(), Edge.GetSrcNId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
    if (! IsNode(Edge.GetDstNId())) {
      const TStr Msg = TStr::Fmt("Edge %d destination node %d does not exist.", Edge.GetId(), Edge.GetDstNId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
  }
  return RetVal;
}

/////////////////////////////////////////////////
// Common Node-Edge Network Types
typedef TNodeEdgeNet<TInt, TInt> TIntNENet;
typedef TPt<TIntNENet> PIntNENet;
typedef TNodeEdgeNet<TFlt, TFlt> TFltNENet;
typedef TPt<TFltNENet> PFltNENet;

class TNEANet;
/// Pointer to a directed attribute multigraph (TNEANet)
typedef TPt<TNEANet> PNEANet;

//#//////////////////////////////////////////////
/// Directed multigraph with node edge attributes. ##TNEANet::Class
class TNEANet {
public:
  typedef TNEANet TNet;
  typedef TPt<TNEANet> PNet;
public:
  class TNode {
  private:
    TInt Id;
    TIntV InEIdV, OutEIdV;
  public:
    TNode() : Id(-1), InEIdV(), OutEIdV() { }
    TNode(const int& NId) : Id(NId), InEIdV(), OutEIdV() { }
    TNode(const TNode& Node) : Id(Node.Id), InEIdV(Node.InEIdV), OutEIdV(Node.OutEIdV) { }
    TNode(TSIn& SIn) : Id(SIn), InEIdV(SIn), OutEIdV(SIn) { }
    void Save(TSOut& SOut) const { Id.Save(SOut); InEIdV.Save(SOut); OutEIdV.Save(SOut); }
    int GetId() const { return Id; }
    int GetDeg() const { return GetInDeg() + GetOutDeg(); }
    int GetInDeg() const { return InEIdV.Len(); }
    int GetOutDeg() const { return OutEIdV.Len(); }
    int GetInEId(const int& EdgeN) const { return InEIdV[EdgeN]; }
    int GetOutEId(const int& EdgeN) const { return OutEIdV[EdgeN]; }
    int GetNbrEId(const int& EdgeN) const { return EdgeN<GetOutDeg()?GetOutEId(EdgeN):GetInEId(EdgeN-GetOutDeg()); }
    bool IsInEId(const int& EId) const { return InEIdV.SearchBin(EId) != -1; }
    bool IsOutEId(const int& EId) const { return OutEIdV.SearchBin(EId) != -1; }
    void LoadShM(TShMIn& MStream) {
      Id = TInt(MStream);
      InEIdV.LoadShM(MStream);
      OutEIdV.LoadShM(MStream);
    }
    friend class TNEANet;
  };
  class TEdge {
  private:
    TInt Id, SrcNId, DstNId;
  public:
    TEdge() : Id(-1), SrcNId(-1), DstNId(-1) { }
    TEdge(const int& EId, const int& SourceNId, const int& DestNId) : Id(EId), SrcNId(SourceNId), DstNId(DestNId) { }
    TEdge(const TEdge& Edge) : Id(Edge.Id), SrcNId(Edge.SrcNId), DstNId(Edge.DstNId) { }
    TEdge(TSIn& SIn) : Id(SIn), SrcNId(SIn), DstNId(SIn) { }
    void Save(TSOut& SOut) const { Id.Save(SOut); SrcNId.Save(SOut); DstNId.Save(SOut); }
    int GetId() const { return Id; }
    int GetSrcNId() const { return SrcNId; }
    int GetDstNId() const { return DstNId; }
    void Load(TSIn& InStream) {
      Id = TInt(InStream);
      SrcNId = TInt(InStream);
      DstNId = TInt(InStream);
    }
    friend class TNEANet;
  };
  /// Node iterator. Only forward iteration (operator++) is supported.
  class TNodeI {
  protected:
    typedef THash<TInt, TNode>::TIter THashIter;
    THashIter NodeHI;
    const TNEANet *Graph;
  public:
    TNodeI() : NodeHI(), Graph(NULL) { }
    TNodeI(const THashIter& NodeHIter, const TNEANet* GraphPt) : NodeHI(NodeHIter), Graph(GraphPt) { }
    TNodeI(const TNodeI& NodeI) : NodeHI(NodeI.NodeHI), Graph(NodeI.Graph) { }
    TNodeI& operator = (const TNodeI& NodeI) { NodeHI = NodeI.NodeHI; Graph=NodeI.Graph; return *this; }
    /// Increment iterator.
    TNodeI& operator++ (int) { NodeHI++; return *this; }
    bool operator < (const TNodeI& NodeI) const { return NodeHI < NodeI.NodeHI; }
    bool operator == (const TNodeI& NodeI) const { return NodeHI == NodeI.NodeHI; }
    /// Returns ID of the current node.
    int GetId() const { return NodeHI.GetDat().GetId(); }
    /// Returns degree of the current node, the sum of in-degree and out-degree.
    int GetDeg() const { return NodeHI.GetDat().GetDeg(); }
    /// Returns in-degree of the current node.
    int GetInDeg() const { return NodeHI.GetDat().GetInDeg(); }
    /// Returns out-degree of the current node.
    int GetOutDeg() const { return NodeHI.GetDat().GetOutDeg(); }
    /// Returns ID of EdgeN-th in-node (the node pointing to the current node). ##TNEANet::TNodeI::GetInNId
    int GetInNId(const int& EdgeN) const { return Graph->GetEdge(NodeHI.GetDat().GetInEId(EdgeN)).GetSrcNId(); }
    /// Returns ID of EdgeN-th out-node (the node the current node points to). ##TNEANet::TNodeI::GetOutNId
    int GetOutNId(const int& EdgeN) const { return Graph->GetEdge(NodeHI.GetDat().GetOutEId(EdgeN)).GetDstNId(); }
    /// Returns ID of EdgeN-th neighboring node. ##TNEANet::TNodeI::GetNbrNId
    int GetNbrNId(const int& EdgeN) const { const TEdge& E = Graph->GetEdge(NodeHI.GetDat().GetNbrEId(EdgeN)); return GetId()==E.GetSrcNId() ? E.GetDstNId():E.GetSrcNId(); }
    /// Tests whether node with ID NId points to the current node.
    bool IsInNId(const int& NId) const;
    /// Tests whether the current node points to node with ID NId.
    bool IsOutNId(const int& NId) const;
    /// Tests whether node with ID NId is a neighbor of the current node.
    bool IsNbrNId(const int& NId) const { return IsOutNId(NId) || IsInNId(NId); }
    /// Returns ID of EdgeN-th in-edge.
    int GetInEId(const int& EdgeN) const { return NodeHI.GetDat().GetInEId(EdgeN); }
    /// Returns ID of EdgeN-th out-edge.
    int GetOutEId(const int& EdgeN) const { return NodeHI.GetDat().GetOutEId(EdgeN); }
    /// Returns ID of EdgeN-th in or out-edge.
    int GetNbrEId(const int& EdgeN) const { return NodeHI.GetDat().GetNbrEId(EdgeN); }
    /// Tests whether the edge with ID EId is an in-edge of current node.
    bool IsInEId(const int& EId) const { return NodeHI.GetDat().IsInEId(EId); }
    /// Tests whether the edge with ID EId is an out-edge of current node.
    bool IsOutEId(const int& EId) const { return NodeHI.GetDat().IsOutEId(EId); }
    /// Tests whether the edge with ID EId is an in or out-edge of current node.
    bool IsNbrEId(const int& EId) const { return IsInEId(EId) || IsOutEId(EId); }
    /// Gets vector of attribute names.
    void GetAttrNames(TStrV& Names) const { Graph->AttrNameNI(GetId(), Names); }
    /// Gets vector of attribute values.
    void GetAttrVal(TStrV& Val) const { Graph->AttrValueNI(GetId(), Val); }
    /// Gets vector of int attribute names.
    void GetIntAttrNames(TStrV& Names) const { Graph->IntAttrNameNI(GetId(), Names); }
    /// Gets vector of int attribute values.
    void GetIntAttrVal(TIntV& Val) const { Graph->IntAttrValueNI(GetId(), Val); }
    /// Gets vector of int attribute names.
    void GetIntVAttrNames(TStrV& Names) const { Graph->IntVAttrNameNI(GetId(), Names); }
    /// Gets vector of int attribute values.
    void GetIntVAttrVal(TVec<TIntV>& Val) const { Graph->IntVAttrValueNI(GetId(), Val); }
    /// Gets vector of str attribute names.
    void GetStrAttrNames(TStrV& Names) const { Graph->StrAttrNameNI(GetId(), Names); }
    /// Gets vector of str attribute values.
    void GetStrAttrVal(TStrV& Val) const { Graph->StrAttrValueNI(GetId(), Val); }
    /// Gets vector of flt attribute names.
    void GetFltAttrNames(TStrV& Names) const { Graph->FltAttrNameNI(GetId(), Names); }
    /// Gets vector of flt attribute values.
    void GetFltAttrVal(TFltV& Val) const { Graph->FltAttrValueNI(GetId(), Val); }
    friend class TNEANet;
  };
  /// Edge iterator. Only forward iteration (operator++) is supported.
  class TEdgeI {
  private:
    typedef THash<TInt, TEdge>::TIter THashIter;
    THashIter EdgeHI;
    const TNEANet *Graph;
  public:
    TEdgeI() : EdgeHI(), Graph(NULL) { }
    TEdgeI(const THashIter& EdgeHIter, const TNEANet *GraphPt) : EdgeHI(EdgeHIter), Graph(GraphPt) { }
    TEdgeI(const TEdgeI& EdgeI) : EdgeHI(EdgeI.EdgeHI), Graph(EdgeI.Graph) { }
    TEdgeI& operator = (const TEdgeI& EdgeI) { if (this!=&EdgeI) { EdgeHI=EdgeI.EdgeHI; Graph=EdgeI.Graph; }  return *this; }
    /// Increment iterator.
    TEdgeI& operator++ (int) { EdgeHI++; return *this; }
    bool operator < (const TEdgeI& EdgeI) const { return EdgeHI < EdgeI.EdgeHI; }
    bool operator == (const TEdgeI& EdgeI) const { return EdgeHI == EdgeI.EdgeHI; }
    /// Returns edge ID.
    int GetId() const { return EdgeHI.GetDat().GetId(); }
    /// Returns the source of the edge.
    int GetSrcNId() const { return EdgeHI.GetDat().GetSrcNId(); }
    /// Returns the destination of the edge.
    int GetDstNId() const { return EdgeHI.GetDat().GetDstNId(); }
    /// Gets vector of attribute names.
    void GetAttrNames(TStrV& Names) const { Graph->AttrNameEI(GetId(), Names); }
    /// Gets vector of attribute values.
    void GetAttrVal(TStrV& Val) const { Graph->AttrValueEI(GetId(), Val); }
    /// Gets vector of int attribute names.
    void GetIntAttrNames(TStrV& Names) const { Graph->IntAttrNameEI(GetId(), Names); }
    /// Gets vector of int attribute values.
    void GetIntAttrVal(TIntV& Val) const { Graph->IntAttrValueEI(GetId(), Val); }
    /// Gets vector of int attribute names.
    void GetIntVAttrNames(TStrV& Names) const { Graph->IntVAttrNameEI(GetId(), Names); }
    /// Gets vector of int attribute values.
    void GetIntVAttrVal(TVec<TIntV>& Val) const { Graph->IntVAttrValueEI(GetId(), Val); }
    /// Gets vector of str attribute names.
    void GetStrAttrNames(TStrV& Names) const { Graph->StrAttrNameEI(GetId(), Names); }
    /// Gets vector of str attribute values.
    void GetStrAttrVal(TStrV& Val) const { Graph->StrAttrValueEI(GetId(), Val); }
    /// Gets vector of flt attribute names.
    void GetFltAttrNames(TStrV& Names) const { Graph->FltAttrNameEI(GetId(), Names); }
    /// Gets vector of flt attribute values.
    void GetFltAttrVal(TFltV& Val) const { Graph->FltAttrValueEI(GetId(), Val); }
    friend class TNEANet;
  };

  /// Node/edge integer attribute iterator. Iterates through all nodes/edges for one integer attribute.
  class TAIntI {
  private:
    typedef TIntV::TIter TIntVecIter;
    TIntVecIter HI;
    bool isNode;
    TStr attr;
    const TNEANet *Graph;
  public:
    TAIntI() : HI(), attr(), Graph(NULL) { }
    TAIntI(const TIntVecIter& HIter, TStr attribute, bool isEdgeIter, const TNEANet* GraphPt) : HI(HIter), attr(), Graph(GraphPt) { isNode = !isEdgeIter; attr = attribute; }
    TAIntI(const TAIntI& I) : HI(I.HI), attr(I.attr), Graph(I.Graph) { isNode = I.isNode; }
    TAIntI& operator = (const TAIntI& I) { HI = I.HI; Graph=I.Graph; isNode = I.isNode; attr = I.attr; return *this; }
    bool operator < (const TAIntI& I) const { return HI < I.HI; }
    bool operator == (const TAIntI& I) const { return HI == I.HI; }
    /// Returns an attribute of the node.
    TInt GetDat() const { return HI[0]; }
    /// Returns true if the attribute has been deleted.
    bool IsDeleted() const { return isNode ? GetDat() == Graph->GetIntAttrDefaultN(attr) : GetDat() == Graph->GetIntAttrDefaultE(attr); };
    TAIntI& operator++(int) { HI++; return *this; }
    friend class TNEANet;
  };

  class TAIntVI {
  private:
    typedef TVec<TIntV>::TIter TIntVVecIter;
    TIntVVecIter HI;
    bool IsDense;
    typedef THash<TInt, TIntV>::TIter TIntHVecIter;
    TIntHVecIter HHI;
    bool isNode;
    TStr attr;
    const TNEANet *Graph;
  public:
    TAIntVI() : HI(), IsDense(), HHI(), attr(), Graph(NULL) { }
    TAIntVI(const TIntVVecIter& HIter, const TIntHVecIter& HHIter, TStr attribute, bool isEdgeIter, const TNEANet* GraphPt, bool is_dense) : HI(HIter), IsDense(is_dense), HHI(HHIter), attr(), Graph(GraphPt) {
      isNode = !isEdgeIter; attr = attribute;
    }
    TAIntVI(const TAIntVI& I) : HI(I.HI), IsDense(I.IsDense), HHI(I.HHI), attr(I.attr), Graph(I.Graph) { isNode = I.isNode; }
    TAIntVI& operator = (const TAIntVI& I) { HI = I.HI; HHI = I.HHI, Graph=I.Graph; isNode = I.isNode; attr = I.attr; return *this; }
    bool operator < (const TAIntVI& I) const { return HI == I.HI ? HHI < I.HHI : HI < I.HI; }
    bool operator == (const TAIntVI& I) const { return HI == I.HI && HHI == I.HHI; }
    /// Returns an attribute of the node.
    TIntV GetDat() const { return IsDense? HI[0] : HHI.GetDat(); }
    TAIntVI& operator++(int) { if (IsDense) {HI++;} else {HHI++;} return *this; }
    friend class TNEANet;
  };

  /// Node/edge string attribute iterator. Iterates through all nodes/edges for one string attribute.
  class TAStrI {
  private:
    typedef TStrV::TIter TStrVecIter;
    TStrVecIter HI;
    bool isNode;
    TStr attr;
    const TNEANet *Graph;
  public:
    TAStrI() : HI(), attr(), Graph(NULL) { }
    TAStrI(const TStrVecIter& HIter, TStr attribute, bool isEdgeIter, const TNEANet* GraphPt) : HI(HIter), attr(), Graph(GraphPt) { isNode = !isEdgeIter; attr = attribute; }
    TAStrI(const TAStrI& I) : HI(I.HI), attr(I.attr), Graph(I.Graph) { isNode = I.isNode; }
    TAStrI& operator = (const TAStrI& I) { HI = I.HI; Graph=I.Graph; isNode = I.isNode; attr = I.attr; return *this; }
    bool operator < (const TAStrI& I) const { return HI < I.HI; }
    bool operator == (const TAStrI& I) const { return HI == I.HI; }
    /// Returns an attribute of the node.
    TStr GetDat() const { return HI[0]; }
    /// Returns true if the attribute has been deleted.
    bool IsDeleted() const { return isNode ? GetDat() == Graph->GetStrAttrDefaultN(attr) : GetDat() == Graph->GetStrAttrDefaultE(attr); };
    TAStrI& operator++(int) { HI++; return *this; }
    friend class TNEANet;
  };

  /// Node/edge float attribute iterator. Iterates through all nodes/edges for one float attribute.
  class TAFltI {
  private:
    typedef TFltV::TIter TFltVecIter;
    TFltVecIter HI;
    bool isNode;
    TStr attr;
    const TNEANet *Graph;
  public:
    TAFltI() : HI(), attr(), Graph(NULL) { }
    TAFltI(const TFltVecIter& HIter, TStr attribute, bool isEdgeIter, const TNEANet* GraphPt) : HI(HIter), attr(), Graph(GraphPt) { isNode = !isEdgeIter; attr = attribute; }
    TAFltI(const TAFltI& I) : HI(I.HI), attr(I.attr), Graph(I.Graph) { isNode = I.isNode; }
    TAFltI& operator = (const TAFltI& I) { HI = I.HI; Graph=I.Graph; isNode = I.isNode; attr = I.attr; return *this; }
    bool operator < (const TAFltI& I) const { return HI < I.HI; }
    bool operator == (const TAFltI& I) const { return HI == I.HI; }
    /// Returns an attribute of the node.
    TFlt GetDat() const { return HI[0]; }
    /// Returns true if the attribute has been deleted.
    bool IsDeleted() const { return isNode ? GetDat() == Graph->GetFltAttrDefaultN(attr) : GetDat() == Graph->GetFltAttrDefaultE(attr); };
    TAFltI& operator++(int) { HI++; return *this; }
    friend class TNEANet;
  };

protected:
  TNode& GetNode(const int& NId) { return NodeH.GetDat(NId); }
  const TNode& GetNode(const int& NId) const { return NodeH.GetDat(NId); }
  TEdge& GetEdge(const int& EId) { return EdgeH.GetDat(EId); }
  const TEdge& GetEdge(const int& EId) const { return EdgeH.GetDat(EId); }
  int AddAttributes(const int NId);

protected:
  /// Gets Int node attribute val.  If not a proper attr, return default.
  TInt GetIntAttrDefaultN(const TStr& attribute) const { return IntDefaultsN.IsKey(attribute) ? IntDefaultsN.GetDat(attribute) : (TInt) TInt::Mn; }
  /// Gets Str node attribute val.  If not a proper attr, return default.
  TStr GetStrAttrDefaultN(const TStr& attribute) const { return StrDefaultsN.IsKey(attribute) ? StrDefaultsN.GetDat(attribute) : (TStr) TStr::GetNullStr(); }
  /// Gets Flt node attribute val.  If not a proper attr, return default.
  TFlt GetFltAttrDefaultN(const TStr& attribute) const { return FltDefaultsN.IsKey(attribute) ? FltDefaultsN.GetDat(attribute) : (TFlt) TFlt::Mn; }
  /// Gets Int edge attribute val.  If not a proper attr, return default.
  TInt GetIntAttrDefaultE(const TStr& attribute) const { return IntDefaultsE.IsKey(attribute) ? IntDefaultsE.GetDat(attribute) : (TInt) TInt::Mn; }
  /// Gets Str edge attribute val.  If not a proper attr, return default.
  TStr GetStrAttrDefaultE(const TStr& attribute) const { return StrDefaultsE.IsKey(attribute) ? StrDefaultsE.GetDat(attribute) : (TStr) TStr::GetNullStr(); }
  /// Gets Flt edge attribute val.  If not a proper attr, return default.
  TFlt GetFltAttrDefaultE(const TStr& attribute) const { return FltDefaultsE.IsKey(attribute) ? FltDefaultsE.GetDat(attribute) : (TFlt) TFlt::Mn; }
public:
  TCRef CRef;
protected:
  TInt MxNId, MxEId;
  THash<TInt, TNode> NodeH;
  THash<TInt, TEdge> EdgeH;
  /// KeyToIndexType[N|E]: Key->(Type,Index).
  TStrIntPrH KeyToIndexTypeN, KeyToIndexTypeE;
  /// KeyToDense[N|E]: Key->(True if Vec, False if Hash)
  THash<TStr, TBool> KeyToDenseN, KeyToDenseE;

  THash<TStr, TInt> IntDefaultsN, IntDefaultsE;
  THash<TStr, TStr> StrDefaultsN, StrDefaultsE;
  THash<TStr, TFlt> FltDefaultsN, FltDefaultsE;
  TVec<TIntV> VecOfIntVecsN, VecOfIntVecsE;
  TVec<TStrV> VecOfStrVecsN, VecOfStrVecsE;
  TVec<TFltV> VecOfFltVecsN, VecOfFltVecsE;
  TVec<TVec<TIntV> > VecOfIntVecVecsN, VecOfIntVecVecsE;
  TVec<THash<TInt, TIntV> > VecOfIntHashVecsN, VecOfIntHashVecsE;
  enum { IntType, StrType, FltType, IntVType };

  TAttr SAttrN;
  TAttr SAttrE;
private:
  class LoadTNodeFunctor {
  public:
    LoadTNodeFunctor() {}
    void operator() (TNode* n, TShMIn& ShMIn) { n->LoadShM(ShMIn);}
  };
  class LoadVecFunctor {
  public:
    LoadVecFunctor() {}
    template<typename TElem>
    void operator() (TVec<TElem>* n, TShMIn& ShMIn) {
      n->LoadShM(ShMIn);
    }
  };
  class LoadVecOfVecFunctor {
  public:
    LoadVecOfVecFunctor() {}
    template<typename TElem>
    void operator() (TVec<TVec<TElem> >* n, TShMIn& ShMIn) {
      LoadVecFunctor f;
      n->LoadShM(ShMIn, f);
    }
  };

  class LoadHashOfVecFunctor {
  public:
    LoadHashOfVecFunctor() {}
    template<typename TElem>
    void operator() (THash<TInt, TVec<TElem> >* n, TShMIn& ShMIn) {
      LoadVecFunctor f;
      n->LoadShM(ShMIn, f);
    }
  };

protected:
  /// Return 1 if in Dense, 0 if in Sparse, -1 if neither 
  TInt CheckDenseOrSparseN(const TStr& attr) const {
    if (!KeyToDenseN.IsKey(attr)) return -1;
    if (KeyToDenseN.GetDat(attr)) return 1;
    return 0;
  }

  TInt CheckDenseOrSparseE(const TStr& attr) const {
    if (!KeyToDenseE.IsKey(attr)) return -1;
    if (KeyToDenseE.GetDat(attr)) return 1;
    return 0;
  }
  

public:
  TNEANet() : CRef(), MxNId(0), MxEId(0), NodeH(), EdgeH(),
    KeyToIndexTypeN(), KeyToIndexTypeE(), KeyToDenseN(), KeyToDenseE(), IntDefaultsN(), IntDefaultsE(),
    StrDefaultsN(), StrDefaultsE(), FltDefaultsN(), FltDefaultsE(),
    VecOfIntVecsN(), VecOfIntVecsE(), VecOfStrVecsN(), VecOfStrVecsE(),
    VecOfFltVecsN(), VecOfFltVecsE(),  VecOfIntVecVecsN(), VecOfIntVecVecsE(),
    VecOfIntHashVecsN(), VecOfIntHashVecsE(), SAttrN(), SAttrE(){ }
  /// Constructor that reserves enough memory for a graph of nodes and edges.
  explicit TNEANet(const int& Nodes, const int& Edges) : CRef(),
    MxNId(0), MxEId(0), NodeH(), EdgeH(), KeyToIndexTypeN(), KeyToIndexTypeE(), KeyToDenseN(), KeyToDenseE(),
    IntDefaultsN(), IntDefaultsE(), StrDefaultsN(), StrDefaultsE(),
    FltDefaultsN(), FltDefaultsE(), VecOfIntVecsN(), VecOfIntVecsE(),
    VecOfStrVecsN(), VecOfStrVecsE(), VecOfFltVecsN(), VecOfFltVecsE(), VecOfIntVecVecsN(), VecOfIntVecVecsE(),
    VecOfIntHashVecsN(), VecOfIntHashVecsE(), SAttrN(), SAttrE()
    { Reserve(Nodes, Edges); }
  TNEANet(const TNEANet& Graph) : MxNId(Graph.MxNId), MxEId(Graph.MxEId),
    NodeH(Graph.NodeH), EdgeH(Graph.EdgeH), KeyToIndexTypeN(), KeyToIndexTypeE(), KeyToDenseN(), KeyToDenseE(),
    IntDefaultsN(), IntDefaultsE(), StrDefaultsN(), StrDefaultsE(),
    FltDefaultsN(), FltDefaultsE(), VecOfIntVecsN(), VecOfIntVecsE(),
    VecOfStrVecsN(), VecOfStrVecsE(), VecOfFltVecsN(), VecOfFltVecsE(), VecOfIntVecVecsN(), VecOfIntVecVecsE(),
    VecOfIntHashVecsN(), VecOfIntHashVecsE(), SAttrN(), SAttrE() { }
  /// Constructor for loading the graph from a (binary) stream SIn.
  TNEANet(TSIn& SIn) : MxNId(SIn), MxEId(SIn), NodeH(SIn), EdgeH(SIn),
    KeyToIndexTypeN(SIn), KeyToIndexTypeE(SIn), KeyToDenseN(SIn), KeyToDenseE(SIn), IntDefaultsN(SIn), IntDefaultsE(SIn),
    StrDefaultsN(SIn), StrDefaultsE(SIn), FltDefaultsN(SIn), FltDefaultsE(SIn),
    VecOfIntVecsN(SIn), VecOfIntVecsE(SIn), VecOfStrVecsN(SIn),VecOfStrVecsE(SIn),
    VecOfFltVecsN(SIn), VecOfFltVecsE(SIn), VecOfIntVecVecsN(SIn), VecOfIntVecVecsE(SIn), VecOfIntHashVecsN(SIn), VecOfIntHashVecsE(SIn),
    SAttrN(SIn), SAttrE(SIn) { }
protected:
  TNEANet(const TNEANet& Graph, bool modeSubGraph) : MxNId(Graph.MxNId), MxEId(Graph.MxEId),
    NodeH(Graph.NodeH), EdgeH(Graph.EdgeH), KeyToIndexTypeN(), KeyToIndexTypeE(Graph.KeyToIndexTypeE), KeyToDenseN(), KeyToDenseE(Graph.KeyToDenseE),
    IntDefaultsN(Graph.IntDefaultsN), IntDefaultsE(Graph.IntDefaultsE), StrDefaultsN(Graph.StrDefaultsN), StrDefaultsE(Graph.StrDefaultsE),
    FltDefaultsN(Graph.FltDefaultsN), FltDefaultsE(Graph.FltDefaultsE), VecOfIntVecsN(Graph.VecOfIntVecsN), VecOfIntVecsE(Graph.VecOfIntVecsE),
    VecOfStrVecsN(Graph.VecOfStrVecsN), VecOfStrVecsE(Graph.VecOfStrVecsE), VecOfFltVecsN(Graph.VecOfFltVecsN), VecOfFltVecsE(Graph.VecOfFltVecsE),
    VecOfIntVecVecsN(), VecOfIntVecVecsE(Graph.VecOfIntVecVecsE), VecOfIntHashVecsN(), VecOfIntHashVecsE(Graph.VecOfIntHashVecsE) { }
  TNEANet(bool copyAll, const TNEANet& Graph) : MxNId(Graph.MxNId), MxEId(Graph.MxEId),
    NodeH(Graph.NodeH), EdgeH(Graph.EdgeH), KeyToIndexTypeN(Graph.KeyToIndexTypeN), KeyToIndexTypeE(Graph.KeyToIndexTypeE), KeyToDenseN(Graph.KeyToDenseN), KeyToDenseE(Graph.KeyToDenseE),
    IntDefaultsN(Graph.IntDefaultsN), IntDefaultsE(Graph.IntDefaultsE), StrDefaultsN(Graph.StrDefaultsN), StrDefaultsE(Graph.StrDefaultsE),
    FltDefaultsN(Graph.FltDefaultsN), FltDefaultsE(Graph.FltDefaultsE), VecOfIntVecsN(Graph.VecOfIntVecsN), VecOfIntVecsE(Graph.VecOfIntVecsE),
    VecOfStrVecsN(Graph.VecOfStrVecsN), VecOfStrVecsE(Graph.VecOfStrVecsE), VecOfFltVecsN(Graph.VecOfFltVecsN), VecOfFltVecsE(Graph.VecOfFltVecsE),
    VecOfIntVecVecsN(Graph.VecOfIntVecVecsN), VecOfIntVecVecsE(Graph.VecOfIntVecVecsE), VecOfIntHashVecsN(Graph.VecOfIntHashVecsN), VecOfIntHashVecsE(Graph.VecOfIntHashVecsE), SAttrN(Graph.SAttrN), SAttrE(Graph.SAttrE) { }
  // virtual ~TNEANet() { }
public:
  /// Saves the graph to a (binary) stream SOut. Expects data structures for sparse attributes.
  void Save(TSOut& SOut) const {
    MxNId.Save(SOut); MxEId.Save(SOut); NodeH.Save(SOut); EdgeH.Save(SOut);
    KeyToIndexTypeN.Save(SOut); KeyToIndexTypeE.Save(SOut);
    KeyToDenseN.Save(SOut); KeyToDenseE.Save(SOut);
    IntDefaultsN.Save(SOut); IntDefaultsE.Save(SOut);
    StrDefaultsN.Save(SOut); StrDefaultsE.Save(SOut);
    FltDefaultsN.Save(SOut); FltDefaultsE.Save(SOut);
    VecOfIntVecsN.Save(SOut); VecOfIntVecsE.Save(SOut);
    VecOfStrVecsN.Save(SOut); VecOfStrVecsE.Save(SOut);
    VecOfFltVecsN.Save(SOut); VecOfFltVecsE.Save(SOut);
    VecOfIntVecVecsN.Save(SOut); VecOfIntVecVecsE.Save(SOut);
    VecOfIntHashVecsN.Save(SOut); VecOfIntHashVecsE.Save(SOut); 
    SAttrN.Save(SOut); SAttrE.Save(SOut); }
  /// Saves the graph to a (binary) stream SOut. Available for backwards compatibility.
  void Save_V1(TSOut& SOut) const {
    MxNId.Save(SOut); MxEId.Save(SOut); NodeH.Save(SOut); EdgeH.Save(SOut);
    KeyToIndexTypeN.Save(SOut); KeyToIndexTypeE.Save(SOut);
    IntDefaultsN.Save(SOut); IntDefaultsE.Save(SOut);
    StrDefaultsN.Save(SOut); StrDefaultsE.Save(SOut);
    FltDefaultsN.Save(SOut); FltDefaultsE.Save(SOut);
    VecOfIntVecsN.Save(SOut); VecOfIntVecsE.Save(SOut);
    VecOfStrVecsN.Save(SOut); VecOfStrVecsE.Save(SOut);
    VecOfFltVecsN.Save(SOut); VecOfFltVecsE.Save(SOut); }
  /// Saves the graph without any sparse data structures. Available for backwards compatibility
  void Save_V2(TSOut& SOut) const {
    MxNId.Save(SOut); MxEId.Save(SOut); NodeH.Save(SOut); EdgeH.Save(SOut);
    KeyToIndexTypeN.Save(SOut); KeyToIndexTypeE.Save(SOut);
    IntDefaultsN.Save(SOut); IntDefaultsE.Save(SOut);
    StrDefaultsN.Save(SOut); StrDefaultsE.Save(SOut);
    FltDefaultsN.Save(SOut); FltDefaultsE.Save(SOut);
    VecOfIntVecsN.Save(SOut); VecOfIntVecsE.Save(SOut);
    VecOfStrVecsN.Save(SOut); VecOfStrVecsE.Save(SOut);
    VecOfFltVecsN.Save(SOut); VecOfFltVecsE.Save(SOut);
    VecOfIntVecVecsN.Save(SOut); VecOfIntVecVecsE.Save(SOut); 
    SAttrN.Save(SOut); SAttrE.Save(SOut); }
  /// Static cons returns pointer to graph. Ex: PNEANet Graph=TNEANet::New().
  static PNEANet New() { return PNEANet(new TNEANet()); }
  /// Static constructor that returns a pointer to the graph and reserves enough memory for Nodes nodes and Edges edges. ##TNEANet::New
  static PNEANet New(const int& Nodes, const int& Edges) { return PNEANet(new TNEANet(Nodes, Edges)); }
  /// Static constructor that loads the graph from a stream SIn and returns a pointer to it.
  static PNEANet Load(TSIn& SIn) { return PNEANet(new TNEANet(SIn)); }
  /// Static constructor that loads the graph from a stream SIn and returns a pointer to it. Backwards compatible.
  static PNEANet Load_V1(TSIn& SIn) {
    PNEANet Graph = PNEANet(new TNEANet());
    Graph->MxNId.Load(SIn); Graph->MxEId.Load(SIn);
    Graph->NodeH.Load(SIn); Graph->EdgeH.Load(SIn);
    Graph->KeyToIndexTypeN.Load(SIn); Graph->KeyToIndexTypeE.Load(SIn);
    Graph->IntDefaultsN.Load(SIn); Graph->IntDefaultsE.Load(SIn);
    Graph->StrDefaultsN.Load(SIn); Graph->StrDefaultsE.Load(SIn);
    Graph->FltDefaultsN.Load(SIn); Graph->FltDefaultsE.Load(SIn);
    Graph->VecOfIntVecsN.Load(SIn); Graph->VecOfIntVecsE.Load(SIn);
    Graph->VecOfStrVecsN.Load(SIn); Graph->VecOfStrVecsE.Load(SIn);
    Graph->VecOfFltVecsN.Load(SIn); Graph->VecOfFltVecsE.Load(SIn);
    return Graph;
  }

  /// Static constructor that loads the graph from a stream SIn and returns a pointer to it. Backwards compatible without Sparse
  static PNEANet Load_V2(TSIn& SIn) {
    PNEANet Graph = PNEANet(new TNEANet());
    Graph->MxNId.Load(SIn); Graph->MxEId.Load(SIn);
    Graph->NodeH.Load(SIn); Graph->EdgeH.Load(SIn);
    Graph->KeyToIndexTypeN.Load(SIn); Graph->KeyToIndexTypeE.Load(SIn);
    Graph->IntDefaultsN.Load(SIn); Graph->IntDefaultsE.Load(SIn);
    Graph->StrDefaultsN.Load(SIn); Graph->StrDefaultsE.Load(SIn);
    Graph->FltDefaultsN.Load(SIn); Graph->FltDefaultsE.Load(SIn);
    Graph->VecOfIntVecsN.Load(SIn); Graph->VecOfIntVecsE.Load(SIn);
    Graph->VecOfStrVecsN.Load(SIn); Graph->VecOfStrVecsE.Load(SIn);
    Graph->VecOfFltVecsN.Load(SIn); Graph->VecOfFltVecsE.Load(SIn);
    Graph->VecOfIntVecVecsN.Load(SIn); Graph->VecOfIntVecVecsE.Load(SIn);
    Graph->SAttrN.Load(SIn); Graph->SAttrE.Load(SIn);
    return Graph;
  }

  /// load network from shared memory for this network
  void LoadNetworkShM(TShMIn& ShMIn);
  /// Static constructor that loads the network from memory. ##TNEANet::LoadShM(TShMIn& ShMIn)
  static PNEANet LoadShM(TShMIn& ShMIn) {
    TNEANet* Network = new TNEANet();
    Network->LoadNetworkShM(ShMIn);
    return PNEANet(Network);
  }

  void ConvertToSparse() {
    TInt VecLength = VecOfIntVecVecsN.Len();
    THash<TStr, TIntPr>::TIter iter;
    if (VecLength != 0) {
      VecOfIntHashVecsN = TVec<THash<TInt, TIntV> >(VecLength);
      for (iter = KeyToIndexTypeN.BegI(); !iter.IsEnd(); iter=iter.Next()) {
        if (iter.GetDat().Val1 == IntVType) {
          TStr attribute = iter.GetKey();
          TInt index = iter.GetDat().Val2();
          for (int i=0; i<VecOfIntVecVecsN[index].Len(); i++) {
            if(VecOfIntVecVecsN[index][i].Len() > 0) {
              VecOfIntHashVecsN[index].AddDat(TInt(i), VecOfIntVecVecsN[index][i]);
            }
          }
          KeyToDenseN.AddDat(attribute, TBool(false));
        }
      }
    }
    VecOfIntVecVecsN.Clr();

    VecLength = VecOfIntVecVecsE.Len();
    if (VecLength != 0) {
      VecOfIntHashVecsE = TVec<THash<TInt, TIntV> >(VecLength);
      for (iter = KeyToIndexTypeE.BegI(); !iter.IsEnd(); iter=iter.Next()) {
        if (iter.GetDat().Val1 == IntVType) {
          TStr attribute = iter.GetKey();
          TInt index = iter.GetDat().Val2();
          for (int i=0; i<VecOfIntVecVecsE[index].Len(); i++) {
            if(VecOfIntVecVecsE[index][i].Len() > 0) {
              VecOfIntHashVecsE[index].AddDat(TInt(i), VecOfIntVecVecsE[index][i]);
            }
          }
          KeyToDenseE.AddDat(attribute, TBool(false));
        }
      }
    }
    VecOfIntVecVecsE.Clr();
  }


  /// Allows for run-time checking the type of the graph (see the TGraphFlag for flags).
  bool HasFlag(const TGraphFlag& Flag) const;
  
  TNEANet& operator = (const TNEANet& Graph) { if (this!=&Graph) {
    MxNId=Graph.MxNId; MxEId=Graph.MxEId; NodeH=Graph.NodeH; EdgeH=Graph.EdgeH; }
    return *this; }

  /// Returns the number of nodes in the network.
  int GetNodes() const { return NodeH.Len(); }
  /// Adds a node of ID NId to the network. ##TNEANet::AddNode
  int AddNode(int NId = -1);
  /// Adds a node of ID NId to the network, noop if the node already exists. ##TNEANet::AddNodeUnchecked
  int AddNodeUnchecked(int NId = -1);
  /// Adds a node of ID NodeI.GetId() to the graph.
  int AddNode(const TNodeI& NodeI) { return AddNode(NodeI.GetId()); }
  /// Deletes node of ID NId from the graph. ##TNEANet::DelNode
  virtual void DelNode(const int& NId);
  /// Deletes node of ID NodeI.GetId() from the graph.
  void DelNode(const TNode& NodeI) { DelNode(NodeI.GetId()); }
  /// Tests whether ID NId is a node.
  bool IsNode(const int& NId) const { return NodeH.IsKey(NId); }
  /// Returns an iterator referring to the first node in the graph.
  TNodeI BegNI() const { return TNodeI(NodeH.BegI(), this); }
  /// Returns an iterator referring to the past-the-end node in the graph.
  TNodeI EndNI() const { return TNodeI(NodeH.EndI(), this); }
  /// Returns an iterator referring to the node of ID NId in the graph.
  TNodeI GetNI(const int& NId) const { return TNodeI(NodeH.GetI(NId), this); }
  /// Returns an iterator referring to the first node's int attribute.
  TAIntI BegNAIntI(const TStr& attr) const {
    return TAIntI(VecOfIntVecsN[KeyToIndexTypeN.GetDat(attr).Val2].BegI(), attr, false, this); }
  /// Returns an iterator referring to the past-the-end node's attribute.
  TAIntI EndNAIntI(const TStr& attr) const {
    return TAIntI(VecOfIntVecsN[KeyToIndexTypeN.GetDat(attr).Val2].EndI(), attr, false, this); }
  /// Returns an iterator referring to the node of ID NId in the graph.
  TAIntI GetNAIntI(const TStr& attr, const int& NId) const {
    return TAIntI(VecOfIntVecsN[KeyToIndexTypeN.GetDat(attr).Val2].GetI(NodeH.GetKeyId(NId)), attr, false, this); }

  /// Returns an iterator referring to the first node's int attribute.
  TAIntVI BegNAIntVI(const TStr& attr) const {
    TVec<TIntV>::TIter HI = NULL;
    THash<TInt, TIntV>::TIter HHI;
    TInt location = CheckDenseOrSparseN(attr);
    TBool IsDense = true;
    if (location != -1) {
      TInt index = KeyToIndexTypeN.GetDat(attr).Val2;
      if (location == 1) {
        HI = VecOfIntVecVecsN[index].BegI();
      } else {
        IsDense = false;
        HHI = VecOfIntHashVecsN[index].BegI();
      }
    }
    return TAIntVI(HI, HHI, attr, false, this, IsDense);
  }
  /// Returns an iterator referring to the past-the-end node's attribute.
  TAIntVI EndNAIntVI(const TStr& attr) const {
    TVec<TIntV>::TIter HI = NULL;
    THash<TInt, TIntV>::TIter HHI;
    TInt location = CheckDenseOrSparseN(attr);
    TBool IsDense = true;
    if (location != -1) {
      TInt index = KeyToIndexTypeN.GetDat(attr).Val2;
      if (location == 1) {
        HI = VecOfIntVecVecsN[index].EndI();
      } else {
        IsDense = false;
        HHI = VecOfIntHashVecsN[index].EndI();
      }
    }
    return TAIntVI(HI, HHI, attr, false, this, IsDense);
  }


  /// Returns an iterator referring to the node of ID NId in the graph.
  TAIntVI GetNAIntVI(const TStr& attr, const int& NId) const {
    TVec<TIntV>::TIter HI = NULL;
    THash<TInt, TIntV>::TIter HHI;
    TInt location = CheckDenseOrSparseN(attr);
    TBool IsDense = true;
    if (location != -1) {
      TInt index = KeyToIndexTypeN.GetDat(attr).Val2;
      if (location == 1) {
        HI = VecOfIntVecVecsN[index].GetI(NodeH.GetKeyId(NId));
      } else {
        IsDense = false;
        HHI = VecOfIntHashVecsN[index].GetI(NodeH.GetKeyId(NId));
      }
    }
    return TAIntVI(HI, HHI, attr, false, this, IsDense);
  }



  /// Returns an iterator referring to the first node's str attribute.
  TAStrI BegNAStrI(const TStr& attr) const {

    return TAStrI(VecOfStrVecsN[KeyToIndexTypeN.GetDat(attr).Val2].BegI(), attr, false, this); }
  /// Returns an iterator referring to the past-the-end node's attribute.
  TAStrI EndNAStrI(const TStr& attr) const {
    return TAStrI(VecOfStrVecsN[KeyToIndexTypeN.GetDat(attr).Val2].EndI(), attr, false, this); }
  /// Returns an iterator referring to the node of ID NId in the graph.
  TAStrI GetNAStrI(const TStr& attr, const int& NId) const {
    return TAStrI(VecOfStrVecsN[KeyToIndexTypeN.GetDat(attr).Val2].GetI(NodeH.GetKeyId(NId)), attr, false, this); }
  /// Returns an iterator referring to the first node's flt attribute.
  TAFltI BegNAFltI(const TStr& attr) const {
    return TAFltI(VecOfFltVecsN[KeyToIndexTypeN.GetDat(attr).Val2].BegI(), attr, false, this); }
  /// Returns an iterator referring to the past-the-end node's attribute.
  TAFltI EndNAFltI(const TStr& attr) const {
    return TAFltI(VecOfFltVecsN[KeyToIndexTypeN.GetDat(attr).Val2].EndI(), attr, false, this); }
  /// Returns an iterator referring to the node of ID NId in the graph.
  TAFltI GetNAFltI(const TStr& attr, const int& NId) const {
    return TAFltI(VecOfFltVecsN[KeyToIndexTypeN.GetDat(attr).Val2].GetI(NodeH.GetKeyId(NId)), attr, false, this); }

  /// Returns a vector of attr names for node NId.
  void AttrNameNI(const TInt& NId, TStrV& Names) const {
    AttrNameNI(NId, KeyToIndexTypeN.BegI(), Names);}
  void AttrNameNI(const TInt& NId, TStrIntPrH::TIter NodeHI, TStrV& Names) const;
  /// Returns a vector of attr values for node NId.
  void AttrValueNI(const TInt& NId, TStrV& Values) const {
    AttrValueNI(NId, KeyToIndexTypeN.BegI(), Values);}
  void AttrValueNI(const TInt& NId, TStrIntPrH::TIter NodeHI, TStrV& Values) const;

  /// Returns a vector of int attr names for node NId.
  void IntAttrNameNI(const TInt& NId, TStrV& Names) const {
    IntAttrNameNI(NId, KeyToIndexTypeN.BegI(), Names);}
  void IntAttrNameNI(const TInt& NId, TStrIntPrH::TIter NodeHI, TStrV& Names) const;
  /// Returns a vector of attr values for node NId.
  void IntAttrValueNI(const TInt& NId, TIntV& Values) const {
    IntAttrValueNI(NId, KeyToIndexTypeN.BegI(), Values);}
  void IntAttrValueNI(const TInt& NId, TStrIntPrH::TIter NodeHI, TIntV& Values) const;


  /// Returns a vector of int attr names for node NId.
  void IntVAttrNameNI(const TInt& NId, TStrV& Names) const {
    IntVAttrNameNI(NId, KeyToIndexTypeN.BegI(), Names);}
  void IntVAttrNameNI(const TInt& NId, TStrIntPrH::TIter NodeHI, TStrV& Names) const;
  /// Returns a vector of attr values for node NId.
  void IntVAttrValueNI(const TInt& NId, TVec<TIntV>& Values) const {
    IntVAttrValueNI(NId, KeyToIndexTypeN.BegI(), Values);}
  void IntVAttrValueNI(const TInt& NId, TStrIntPrH::TIter NodeHI, TVec<TIntV>& Values) const;


  /// Returns a vector of str attr names for node NId.
  void StrAttrNameNI(const TInt& NId, TStrV& Names) const {
    StrAttrNameNI(NId, KeyToIndexTypeN.BegI(), Names);}
  void StrAttrNameNI(const TInt& NId, TStrIntPrH::TIter NodeHI, TStrV& Names) const;
  /// Returns a vector of attr values for node NId.
  void StrAttrValueNI(const TInt& NId, TStrV& Values) const {
    StrAttrValueNI(NId, KeyToIndexTypeN.BegI(), Values);}
  void StrAttrValueNI(const TInt& NId, TStrIntPrH::TIter NodeHI, TStrV& Values) const;
  /// Returns a vector of int attr names for node NId.
  void FltAttrNameNI(const TInt& NId, TStrV& Names) const {
    FltAttrNameNI(NId, KeyToIndexTypeN.BegI(), Names);}
  void FltAttrNameNI(const TInt& NId, TStrIntPrH::TIter NodeHI, TStrV& Names) const;
  /// Returns a vector of attr values for node NId.
  void FltAttrValueNI(const TInt& NId, TFltV& Values) const {
    FltAttrValueNI(NId, KeyToIndexTypeN.BegI(), Values);}
  void FltAttrValueNI(const TInt& NId, TStrIntPrH::TIter NodeHI, TFltV& Values) const;

  /// Returns a vector of attr names for edge EId.
  void AttrNameEI(const TInt& EId, TStrV& Names) const {
    AttrNameEI(EId, KeyToIndexTypeE.BegI(), Names);}
  void AttrNameEI(const TInt& EId, TStrIntPrH::TIter EdgeHI, TStrV& Names) const;
  /// Returns a vector of attr values for edge EId.
  void AttrValueEI(const TInt& EId, TStrV& Values) const {
    AttrValueEI(EId, KeyToIndexTypeE.BegI(), Values);}
  void AttrValueEI(const TInt& EId, TStrIntPrH::TIter EdgeHI, TStrV& Values) const;
  /// Returns a vector of int attr names for edge EId.
  void IntAttrNameEI(const TInt& EId, TStrV& Names) const {
    IntAttrNameEI(EId, KeyToIndexTypeE.BegI(), Names);}
  void IntAttrNameEI(const TInt& EId, TStrIntPrH::TIter EdgeHI, TStrV& Names) const;
  /// Returns a vector of attr values for edge EId.
  void IntAttrValueEI(const TInt& EId, TIntV& Values) const {
    IntAttrValueEI(EId, KeyToIndexTypeE.BegI(), Values);}
  void IntAttrValueEI(const TInt& EId, TStrIntPrH::TIter EdgeHI, TIntV& Values) const;


  /// Returns a vector of int attr names for edge EId.
  void IntVAttrNameEI(const TInt& EId, TStrV& Names) const {
    IntVAttrNameEI(EId, KeyToIndexTypeE.BegI(), Names);}
  void IntVAttrNameEI(const TInt& EId, TStrIntPrH::TIter EdgeHI, TStrV& Names) const;
  /// Returns a vector of attr values for edge EId.
  void IntVAttrValueEI(const TInt& EId, TVec<TIntV>& Values) const {
    IntVAttrValueEI(EId, KeyToIndexTypeE.BegI(), Values);}
  void IntVAttrValueEI(const TInt& EId, TStrIntPrH::TIter EdgeHI, TVec<TIntV>& Values) const;


  /// Returns a vector of str attr names for node NId.
  void StrAttrNameEI(const TInt& EId, TStrV& Names) const {
    StrAttrNameEI(EId, KeyToIndexTypeE.BegI(), Names);}
  void StrAttrNameEI(const TInt& EId, TStrIntPrH::TIter EdgeHI, TStrV& Names) const;
  /// Returns a vector of attr values for node NId.
  void StrAttrValueEI(const TInt& EId, TStrV& Values) const {
    StrAttrValueEI(EId, KeyToIndexTypeE.BegI(), Values);}
  void StrAttrValueEI(const TInt& EId, TStrIntPrH::TIter EdgeHI, TStrV& Values) const;
  /// Returns a vector of int attr names for node NId.
  void FltAttrNameEI(const TInt& EId, TStrV& Names) const {
    FltAttrNameEI(EId, KeyToIndexTypeE.BegI(), Names);}
  void FltAttrNameEI(const TInt& EId, TStrIntPrH::TIter EdgeHI, TStrV& Names) const;
  /// Returns a vector of attr values for node NId.
  void FltAttrValueEI(const TInt& EId, TFltV& Values) const {
    FltAttrValueEI(EId, KeyToIndexTypeE.BegI(), Values);}
  void FltAttrValueEI(const TInt& EId, TStrIntPrH::TIter EdgeHI, TFltV& Values) const;

  /// Returns an iterator referring to the first edge's int attribute.
  TAIntI BegEAIntI(const TStr& attr) const {
    return TAIntI(VecOfIntVecsE[KeyToIndexTypeE.GetDat(attr).Val2].BegI(), attr, true, this);
  }
  /// Returns an iterator referring to the past-the-end edge's attribute.
  TAIntI EndEAIntI(const TStr& attr) const {
    return TAIntI(VecOfIntVecsE[KeyToIndexTypeE.GetDat(attr).Val2].EndI(), attr, true, this);
  }
  /// Returns an iterator referring to the edge of ID EId in the graph.
  TAIntI GetEAIntI(const TStr& attr, const int& EId) const {
    return TAIntI(VecOfIntVecsE[KeyToIndexTypeE.GetDat(attr).Val2].GetI(EdgeH.GetKeyId(EId)), attr, true, this);
  }

  /// Returns an iterator referring to the first edge's int attribute.
  TAIntVI BegEAIntVI(const TStr& attr) const {
    TVec<TIntV>::TIter HI = NULL;
    THash<TInt, TIntV>::TIter HHI;
    TInt location = CheckDenseOrSparseE(attr);
    TBool IsDense = true;
    if (location != -1) {
      TInt index = KeyToIndexTypeE.GetDat(attr).Val2;
      if (location == 1) {
        HI = VecOfIntVecVecsE[index].BegI();
      } else {
        IsDense = false;
        HHI = VecOfIntHashVecsE[index].BegI();
      }
    }
    return TAIntVI(HI, HHI, attr, true, this, IsDense);
  }
  /// Returns an iterator referring to the past-the-end edge's attribute.
  TAIntVI EndEAIntVI(const TStr& attr) const {
    TVec<TIntV>::TIter HI = NULL;
    THash<TInt, TIntV>::TIter HHI;
    TInt location = CheckDenseOrSparseE(attr);
    TBool IsDense = true;
    if (location != -1) {
      TInt index = KeyToIndexTypeE.GetDat(attr).Val2;
      if (location == 1) {
        HI = VecOfIntVecVecsE[index].EndI();
      } else {
        IsDense = false;
        HHI = VecOfIntHashVecsE[index].EndI();
      }
    }
    return TAIntVI(HI, HHI, attr, true, this, IsDense);
  }
  /// Returns an iterator referring to the edge of ID EId in the graph.
  TAIntVI GetEAIntVI(const TStr& attr, const int& EId) const {
    TVec<TIntV>::TIter HI = NULL;
    THash<TInt, TIntV>::TIter HHI;
    TInt location = CheckDenseOrSparseE(attr);
    TBool IsDense = true;
    if (location != -1) {
      TInt index = KeyToIndexTypeE.GetDat(attr).Val2;
      if (location == 1) {
        HI = VecOfIntVecVecsE[index].GetI(EdgeH.GetKeyId(EId));
      } else {
        IsDense = false;
        HHI = VecOfIntHashVecsE[index].GetI(EdgeH.GetKeyId(EId));
      }
    }
    return TAIntVI(HI, HHI, attr, true, this, IsDense);
  }

  /// Returns an iterator referring to the first edge's str attribute.
  TAStrI BegEAStrI(const TStr& attr) const {
    return TAStrI(VecOfStrVecsE[KeyToIndexTypeE.GetDat(attr).Val2].BegI(), attr, true, this);   }
  /// Returns an iterator referring to the past-the-end edge's attribute.
  TAStrI EndEAStrI(const TStr& attr) const {
    return TAStrI(VecOfStrVecsE[KeyToIndexTypeE.GetDat(attr).Val2].EndI(), attr, true, this);
  }
  /// Returns an iterator referring to the edge of ID EId in the graph.
  TAStrI GetEAStrI(const TStr& attr, const int& EId) const {
    return TAStrI(VecOfStrVecsE[KeyToIndexTypeE.GetDat(attr).Val2].GetI(EdgeH.GetKeyId(EId)), attr, true, this);
  }
  /// Returns an iterator referring to the first edge's flt attribute.
  TAFltI BegEAFltI(const TStr& attr) const {
    return TAFltI(VecOfFltVecsE[KeyToIndexTypeE.GetDat(attr).Val2].BegI(), attr, true, this);
  }
  /// Returns an iterator referring to the past-the-end edge's attribute.
  TAFltI EndEAFltI(const TStr& attr) const {
    return TAFltI(VecOfFltVecsE[KeyToIndexTypeE.GetDat(attr).Val2].EndI(), attr, true, this);
  }
  /// Returns an iterator referring to the edge of ID EId in the graph.
  TAFltI GetEAFltI(const TStr& attr, const int& EId) const {
    return TAFltI(VecOfFltVecsE[KeyToIndexTypeE.GetDat(attr).Val2].GetI(EdgeH.GetKeyId(EId)), attr, true, this);
  }
  /// Returns an ID that is larger than any node ID in the network.
  int GetMxNId() const { return MxNId; }
  /// Returns an ID that is larger than any edge ID in the network.
  int GetMxEId() const { return MxEId; }

  /// Returns the number of edges in the graph.
  int GetEdges() const { return EdgeH.Len(); }
  /// Adds an edge with ID EId between node IDs SrcNId and DstNId to the graph. ##TNEANet::AddEdge
  int AddEdge(const int& SrcNId, const int& DstNId, int EId  = -1);
  /// Adds an edge between EdgeI.GetSrcNId() and EdgeI.GetDstNId() to the graph.
  int AddEdge(const TEdgeI& EdgeI) { return AddEdge(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), EdgeI.GetId()); }
  /// Deletes an edge with edge ID EId from the graph.
  void DelEdge(const int& EId);
  /// Deletes all edges between node IDs SrcNId and DstNId from the graph. ##TNEANet::DelEdge
  void DelEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true);
  /// Tests whether an edge with edge ID EId exists in the graph.
  bool IsEdge(const int& EId) const { return EdgeH.IsKey(EId); }
  /// Tests whether an edge between node IDs SrcNId and DstNId exists in the graph.
  bool IsEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true) const { int EId; return IsEdge(SrcNId, DstNId, EId, IsDir); }
  /// Tests whether an edge between node IDs SrcNId and DstNId exists in the graph. if an edge exists, return its edge ID in EId
  bool IsEdge(const int& SrcNId, const int& DstNId, int& EId, const bool& IsDir = true) const;
  /// Returns an edge ID between node IDs SrcNId and DstNId, if such an edge exists. Otherwise, return -1.
  int GetEId(const int& SrcNId, const int& DstNId) const { int EId; return IsEdge(SrcNId, DstNId, EId)?EId:-1; }
  /// Returns an iterator referring to the first edge in the graph.
  TEdgeI BegEI() const { return TEdgeI(EdgeH.BegI(), this); }
  /// Returns an iterator referring to the past-the-end edge in the graph.
  TEdgeI EndEI() const { return TEdgeI(EdgeH.EndI(), this); }
  /// Returns an iterator referring to edge with edge ID EId.
  TEdgeI GetEI(const int& EId) const { return TEdgeI(EdgeH.GetI(EId), this); }
  /// Returns an iterator referring to edge (SrcNId, DstNId) in the graph.
  TEdgeI GetEI(const int& SrcNId, const int& DstNId) const { return GetEI(GetEId(SrcNId, DstNId)); }

  /// Returns an ID of a random node in the graph.
  int GetRndNId(TRnd& Rnd=TInt::Rnd) { return NodeH.GetKey(NodeH.GetRndKeyId(Rnd, 0.8)); }
  /// Returns an interator referring to a random node in the graph.
  TNodeI GetRndNI(TRnd& Rnd=TInt::Rnd) { return GetNI(GetRndNId(Rnd)); }
  /// Returns an ID of a random edge in the graph.
  int GetRndEId(TRnd& Rnd=TInt::Rnd) { return EdgeH.GetKey(EdgeH.GetRndKeyId(Rnd, 0.8)); }
  /// Returns an interator referring to a random edge in the graph.
  TEdgeI GetRndEI(TRnd& Rnd=TInt::Rnd) { return GetEI(GetRndEId(Rnd)); }
  /// Gets a vector IDs of all nodes in the graph.
  void GetNIdV(TIntV& NIdV) const;
  /// Gets a vector IDs of all edges in the graph.
  void GetEIdV(TIntV& EIdV) const;

  /// Tests whether the graph is empty (has zero nodes).
  bool Empty() const { return GetNodes()==0; }
  /// Deletes all nodes and edges from the graph.
  void Clr() { MxNId=0; MxEId=0; NodeH.Clr(); EdgeH.Clr();
    KeyToIndexTypeN.Clr(); KeyToIndexTypeE.Clr(); IntDefaultsN.Clr(); IntDefaultsE.Clr();
    StrDefaultsN.Clr(); StrDefaultsE.Clr(); FltDefaultsN.Clr(); FltDefaultsE.Clr();
    VecOfIntVecsN.Clr(); VecOfIntVecsE.Clr(); VecOfStrVecsN.Clr(); VecOfStrVecsE.Clr();
    VecOfFltVecsN.Clr(); VecOfFltVecsE.Clr(); VecOfIntVecVecsN.Clr(); VecOfIntVecVecsE.Clr(); 
    SAttrN.Clr(); SAttrE.Clr();}
  /// Reserves memory for a graph of Nodes nodes and Edges edges.
  void Reserve(const int& Nodes, const int& Edges) {
    if (Nodes>0) { NodeH.Gen(Nodes/2); } if (Edges>0) { EdgeH.Gen(Edges/2); } }
  /// Defragments the graph. ##TNEANet::Defrag
  void Defrag(const bool& OnlyNodeLinks=false);
  /// Checks the graph data structure for internal consistency. ##TNEANet::IsOk
  bool IsOk(const bool& ThrowExcept=true) const;
  /// Print the graph in a human readable form to an output stream OutF.
  void Dump(FILE *OutF=stdout) const;

  /// Attribute based add function for attr to Int value. ##TNEANet::AddIntAttrDatN
  int AddIntAttrDatN(const TNodeI& NodeI, const TInt& value, const TStr& attr) { return AddIntAttrDatN(NodeI.GetId(), value, attr); }
  int AddIntAttrDatN(const int& NId, const TInt& value, const TStr& attr);
  /// Attribute based add function for attr to Str value. ##TNEANet::AddStrAttrDatN
  int AddStrAttrDatN(const TNodeI& NodeI, const TStr& value, const TStr& attr) { return AddStrAttrDatN(NodeI.GetId(), value, attr); }
  int AddStrAttrDatN(const int& NId, const TStr& value, const TStr& attr);
  /// Attribute based add function for attr to Flt value. ##TNEANet::AddFltAttrDatN
  int AddFltAttrDatN(const TNodeI& NodeI, const TFlt& value, const TStr& attr) { return AddFltAttrDatN(NodeI.GetId(), value, attr); }
  int AddFltAttrDatN(const int& NId, const TFlt& value, const TStr& attr);
  /// Attribute based add function for attr to IntV value. ##TNEANet::AddIntVAttrDatN
  int AddIntVAttrDatN(const TNodeI& NodeI, const TIntV& value, const TStr& attr) { return AddIntVAttrDatN(NodeI.GetId(), value, attr); }
  int AddIntVAttrDatN(const int& NId, const TIntV& value, const TStr& attr, TBool UseDense=true);
  /// Appends value onto the TIntV attribute for the given node.
  int AppendIntVAttrDatN(const TNodeI& NodeI, const TInt& value, const TStr& attr) { return AppendIntVAttrDatN(NodeI.GetId(), value, attr); }
  int AppendIntVAttrDatN(const int& NId, const TInt& value, const TStr& attr, TBool UseDense=true);
  /// Deletes value from the TIntV attribute for the given node.
  int DelFromIntVAttrDatN(const TNodeI& NodeI, const TInt& value, const TStr& attr) { return DelFromIntVAttrDatN(NodeI.GetId(), value, attr); }
  int DelFromIntVAttrDatN(const int& NId, const TInt& value, const TStr& attr);
  /// Attribute based add function for attr to Int value. ##TNEANet::AddIntAttrDatE
  int AddIntAttrDatE(const TEdgeI& EdgeI, const TInt& value, const TStr& attr) { return AddIntAttrDatE(EdgeI.GetId(), value, attr); }
  int AddIntAttrDatE(const int& EId, const TInt& value, const TStr& attr);
  /// Attribute based add function for attr to Str value. ##TNEANet::AddStrAttrDatE
  int AddStrAttrDatE(const TEdgeI& EdgeI, const TStr& value, const TStr& attr) { return AddStrAttrDatE(EdgeI.GetId(), value, attr); }
  int AddStrAttrDatE(const int& EId, const TStr& value, const TStr& attr);
  /// Attribute based add function for attr to Flt value. ##TNEANet::AddFltAttrDatE
  int AddFltAttrDatE(const TEdgeI& EdgeI, const TFlt& value, const TStr& attr) { return AddFltAttrDatE(EdgeI.GetId(), value, attr); }
  int AddFltAttrDatE(const int& EId, const TFlt& value, const TStr& attr);
  /// Attribute based add function for attr to IntV value. ##TNEANet::AddIntVAttrDatE
  int AddIntVAttrDatE(const TEdgeI& EdgeI, const TIntV& value, const TStr& attr) { return AddIntVAttrDatE(EdgeI.GetId(), value, attr); }
  int AddIntVAttrDatE(const int& EId, const TIntV& value, const TStr& attr, TBool UseDense=true);
  /// Appends value onto the TIntV attribute for the given node.
  int AppendIntVAttrDatE(const TEdgeI& EdgeI, const TInt& value, const TStr& attr) { return AppendIntVAttrDatE(EdgeI.GetId(), value, attr); }
  int AppendIntVAttrDatE(const int& EId, const TInt& value, const TStr& attr, TBool UseDense=true);
  /// Gets the value of int attr from the node attr value vector.
  TInt GetIntAttrDatN(const TNodeI& NodeI, const TStr& attr) { return GetIntAttrDatN(NodeI.GetId(), attr); }
  TInt GetIntAttrDatN(const int& NId, const TStr& attr);

  /// Gets the value of str attr from the node attr value vector.
  TStr GetStrAttrDatN(const TNodeI& NodeI, const TStr& attr) { return GetStrAttrDatN(NodeI.GetId(), attr); }
  TStr GetStrAttrDatN(const int& NId, const TStr& attr);
  /// Gets the value of flt attr from the node attr value vector.
  TFlt GetFltAttrDatN(const TNodeI& NodeI, const TStr& attr) { return GetFltAttrDatN(NodeI.GetId(), attr); }
  TFlt GetFltAttrDatN(const int& NId, const TStr& attr);
  /// Gets the value of the intv attr from the node attr value vector.
  TIntV GetIntVAttrDatN(const TNodeI& NodeI, const TStr& attr) const { return GetIntVAttrDatN(NodeI.GetId(), attr); }
  TIntV GetIntVAttrDatN(const int& NId, const TStr& attr) const;

  /// Gets the index of the node attr value vector specified by \c attr (same as GetAttrIndN for compatibility reasons).
  int GetIntAttrIndN(const TStr& attr);
  /// Gets the index of the node attr value vector specified by \c attr.
  int GetAttrIndN(const TStr& attr);

  /// Gets the value of an int node attr specified by node iterator \c NodeI and the attr \c index.
  TInt GetIntAttrIndDatN(const TNodeI& NodeI, const int& index) { return GetIntAttrIndDatN(NodeI.GetId(), index); }
  /// Gets the value of an int node attr specified by node ID \c NId and the attr \c index.
  TInt GetIntAttrIndDatN(const int& NId, const int& index);

  /// Gets the value of a string node attr specified by node iterator \c NodeI and the attr \c index.
  TStr GetStrAttrIndDatN(const TNodeI& NodeI, const int& index) { return GetStrAttrIndDatN(NodeI.GetId(), index); }
  /// Gets the value of a string node attr specified by node ID \c NId and the attr \c index.
  TStr GetStrAttrIndDatN(const int& NId, const int& index);

  /// Gets the value of a float node attr specified by node iterator \c NodeI and the attr \c index.
  TFlt GetFltAttrIndDatN(const TNodeI& NodeI, const int& index) { return GetFltAttrIndDatN(NodeI.GetId(), index); }
  /// Gets the value of a float node attr specified by node ID \c NId and the attr \c index.
  TFlt GetFltAttrIndDatN(const int& NId, const int& index);

  /// Gets the value of int attr from the edge attr value vector.
  TInt GetIntAttrDatE(const TEdgeI& EdgeI, const TStr& attr) { return GetIntAttrDatE(EdgeI.GetId(), attr); }
  TInt GetIntAttrDatE(const int& EId, const TStr& attr);
  /// Gets the value of str attr from the edge attr value vector.
  TStr GetStrAttrDatE(const TEdgeI& EdgeI, const TStr& attr) { return GetStrAttrDatE(EdgeI.GetId(), attr); }
  TStr GetStrAttrDatE(const int& EId, const TStr& attr);
  /// Gets the value of flt attr from the edge attr value vector.
  TFlt GetFltAttrDatE(const TEdgeI& EdgeI, const TStr& attr) { return GetFltAttrDatE(EdgeI.GetId(), attr); }
  TFlt GetFltAttrDatE(const int& EId, const TStr& attr);
  /// Gets the value of the intv attr from the edge attr value vector.
  TIntV GetIntVAttrDatE(const TEdgeI& EdgeI, const TStr& attr) { return GetIntVAttrDatE(EdgeI.GetId(), attr); }
  TIntV GetIntVAttrDatE(const int& EId, const TStr& attr);

  /// Gets the index of the edge attr value vector specified by \c attr (same as GetAttrIndE for compatibility reasons).
  int GetIntAttrIndE(const TStr& attr);
  /// Gets the index of the edge attr value vector specified by \c attr.
  int GetAttrIndE(const TStr& attr);

  /// Gets the value of an int edge attr specified by edge iterator \c EdgeI and the attr \c index.
  TInt GetIntAttrIndDatE(const TEdgeI& EdgeI, const int& index) { return GetIntAttrIndDatE(EdgeI.GetId(), index); }
  /// Gets the value of an int edge attr specified by edge ID \c EId and the attr \c index.
  TInt GetIntAttrIndDatE(const int& EId, const int& index);
 
  /// Gets the value of a float edge attr specified by edge iterator \c EdgeI and the attr \c index.
  TFlt GetFltAttrIndDatE(const TEdgeI& EdgeI, const int& index) { return GetFltAttrIndDatE(EdgeI.GetId(), index); }
  /// Gets the value of an int edge attr specified by edge ID \c EId and the attr \c index.
  TFlt GetFltAttrIndDatE(const int& EId, const int& index);
 
  /// Gets the value of a string edge attr specified by edge iterator \c EdgeI and the attr \c index.
  TStr GetStrAttrIndDatE(const TEdgeI& EdgeI, const int& index) { return GetStrAttrIndDatE(EdgeI.GetId(), index); }
  /// Gets the value of an int edge attr specified by edge ID \c EId and the attr \c index.
  TStr GetStrAttrIndDatE(const int& EId, const int& index);
 
  /// Deletes the node attribute for NodeI.
  int DelAttrDatN(const TNodeI& NodeI, const TStr& attr) { return DelAttrDatN(NodeI.GetId(), attr); } 
  int DelAttrDatN(const int& NId, const TStr& attr); 
  /// Deletes the edge attribute for NodeI.
  int DelAttrDatE(const TEdgeI& EdgeI, const TStr& attr) { return DelAttrDatE(EdgeI.GetId(), attr); } 
  int DelAttrDatE(const int& EId, const TStr& attr); 

  /// Adds a new Int node attribute to the hashmap.
  int AddIntAttrN(const TStr& attr, TInt defaultValue=TInt::Mn);
  /// Adds a new Str node attribute to the hashmap.
  int AddStrAttrN(const TStr& attr, TStr defaultValue=TStr::GetNullStr());
  /// Adds a new Flt node attribute to the hashmap.
  int AddFltAttrN(const TStr& attr, TFlt defaultValue=TFlt::Mn);
  /// Adds a new IntV node attribute to the hashmap.
  int AddIntVAttrN(const TStr& attr, TBool UseDense=true);

  /// Adds a new Int edge attribute to the hashmap.
  int AddIntAttrE(const TStr& attr, TInt defaultValue=TInt::Mn);
  /// Adds a new Str edge attribute to the hashmap.
  int AddStrAttrE(const TStr& attr, TStr defaultValue=TStr::GetNullStr());
  /// Adds a new Flt edge attribute to the hashmap.
  int AddFltAttrE(const TStr& attr, TFlt defaultValue=TFlt::Mn);
  /// Adds a new IntV edge attribute to the hashmap.
  int AddIntVAttrE(const TStr& attr, TBool UseDense=true);

  /// Removes all the values for node attr.
  int DelAttrN(const TStr& attr);
  /// Removes all the values for edge  attr.
  int DelAttrE(const TStr& attr);

  /// Returns true if \c attr exists for node \c NId and has default value.
  bool IsAttrDeletedN(const int& NId, const TStr& attr) const;
  /// Returns true if Int \c attr exists for node \c NId and has default value.
  bool IsIntAttrDeletedN(const int& NId, const TStr& attr) const;
  /// Returns true if IntV \c attr exists for node \c NId and is an empty vector.
  bool IsIntVAttrDeletedN(const int& NId, const TStr& attr) const;
  /// Returns true if Str \c attr exists for node \c NId and has default value.
  bool IsStrAttrDeletedN(const int& NId, const TStr& attr) const;
  /// Returns true if Flt \c attr exists for node \c NId and has default value.
  bool IsFltAttrDeletedN(const int& NId, const TStr& attr) const;

  /// Returns true if NId attr deleted for current node attr iterator.
  bool NodeAttrIsDeleted(const int& NId, const TStrIntPrH::TIter& NodeHI) const;
  /// Returns true if NId attr deleted value for current node int attr iterator.
  bool NodeAttrIsIntDeleted(const int& NId, const TStrIntPrH::TIter& NodeHI) const;
  /// Returns true if NId attr deleted value for current node int vector attr iterator.
  bool NodeAttrIsIntVDeleted(const int& NId, const TStrIntPrH::TIter& NodeHI) const;
  /// Returns true if NId attr deleted value for current node str attr iterator.
  bool NodeAttrIsStrDeleted(const int& NId, const TStrIntPrH::TIter& NodeHI) const;
  /// Returns true if NId attr deleted value for current node flt attr iterator.
  bool NodeAttrIsFltDeleted(const int& NId, const TStrIntPrH::TIter& NodeHI) const;

  /// Returns true if \c attr exists for edge \c EId and has default value.
  bool IsAttrDeletedE(const int& EId, const TStr& attr) const;
  /// Returns true if Int \c attr exists for edge \c EId and has default value.
  bool IsIntAttrDeletedE(const int& EId, const TStr& attr) const;
  /// Returns true if IntV \c attr exists for edge \c EId and is an empty vector.
  bool IsIntVAttrDeletedE(const int& EId, const TStr& attr) const;
  /// Returns true if Str \c attr exists for edge \c NId and has default value.
  bool IsStrAttrDeletedE(const int& EId, const TStr& attr) const;
  /// Returns true if Flt \c attr exists for edge \c NId and has default value.
  bool IsFltAttrDeletedE(const int& EId, const TStr& attr) const;

  /// Returns true if EId attr deleted for current edge attr iterator.
  bool EdgeAttrIsDeleted(const int& EId, const TStrIntPrH::TIter& EdgeHI) const;
  /// Returns true if EId attr deleted for current edge int attr iterator.
  bool EdgeAttrIsIntDeleted(const int& EId, const TStrIntPrH::TIter& EdgeHI) const;
  /// Returns true if EId attr deleted for current edge int vector attr iterator.
  bool EdgeAttrIsIntVDeleted(const int& EId, const TStrIntPrH::TIter& EdgeHI) const;
  /// Returns true if EId attr deleted for current edge str attr iterator.
  bool EdgeAttrIsStrDeleted(const int& EId, const TStrIntPrH::TIter& EdgeHI) const;
  /// Returns true if EId attr deleted for current edge flt attr iterator.
  bool EdgeAttrIsFltDeleted(const int& EId, const TStrIntPrH::TIter& EdgeHI) const;

  /// Returns node attribute value, converted to Str type.
  TStr GetNodeAttrValue(const int& NId, const TStrIntPrH::TIter& NodeHI) const;
  /// Returns edge attribute value, converted to Str type.
  TStr GetEdgeAttrValue(const int& EId, const TStrIntPrH::TIter& EdgeHI) const;

  /// Gets the sum of the weights of all the outgoing edges of the node.
  TFlt GetWeightOutEdges(const TNodeI& NI, const TStr& attr);
  /// Checks if there is an edge attribute with name attr.
  bool IsFltAttrE(const TStr& attr);
  /// Checks if there is an edge attribute with name attr.
  bool IsIntAttrE(const TStr& attr);
  /// Checks if there is an edge attribute with name attr.
  bool IsStrAttrE(const TStr& attr);
  /// Gets Vector for the Flt Attribute attr.
  TVec<TFlt>& GetFltAttrVecE(const TStr& attr);
  /// Gets keyid for edge with id EId.
  int GetFltKeyIdE(const int& EId);

  /// Fills OutWeights with the outgoing weight from each node.
  void GetWeightOutEdgesV(TFltV& OutWeights, const TFltV& AttrVal) ;
  /// Fills each of the vectors with the names of node attributes of the given type.
  void GetAttrNNames(TStrV& IntAttrNames, TStrV& FltAttrNames, TStrV& StrAttrNames) const;
  /// Fills each of the vectors with the names of edge attributes of the given type.
  void GetAttrENames(TStrV& IntAttrNames, TStrV& FltAttrNames, TStrV& StrAttrNames) const;


  /// Adds Int sparse attribute with name \c AttrName to the given node with id \c NId.
  int AddSAttrDatN(const TInt& NId, const TStr& AttrName, const TInt& Val);
  /// Adds Int sparse attribute with id \c AttrId to the given node with id \c NId.
  int AddSAttrDatN(const TInt& NId, const TInt& AttrId, const TInt& Val);

  /// Adds Int sparse attribute with name \c AttrName to \c NodeI.
  int AddSAttrDatN(const TNodeI& NodeI, const TStr& AttrName, const TInt& Val) {
    return AddSAttrDatN(NodeI.GetId(), AttrName, Val);
  }
  /// Adds Int sparse attribute with id \c AttrId to \c NodeI.
  int AddSAttrDatN(const TNodeI& NodeI, const TInt& AttrId, const TInt& Val) {
    return AddSAttrDatN(NodeI.GetId(), AttrId, Val);
  }

  /// Adds Flt sparse attribute with name \c AttrName to the given node with id \c NId.
  int AddSAttrDatN(const TInt& NId, const TStr& AttrName, const TFlt& Val);
  /// Adds Flt sparse attribute with id \c AttrId to the given node with id \c NId.
  int AddSAttrDatN(const TInt& NId, const TInt& AttrId, const TFlt& Val);

  /// Adds Flt sparse attribute with name \c AttrName to \c NodeI.
  int AddSAttrDatN(const TNodeI& NodeI, const TStr& AttrName, const TFlt& Val) {
    return AddSAttrDatN(NodeI.GetId(), AttrName, Val);
  }
  /// Adds Flt sparse attribute with id \c AttrId to \c NodeI.
  int AddSAttrDatN(const TNodeI& NodeI, const TInt& AttrId, const TFlt& Val) {
    return AddSAttrDatN(NodeI.GetId(), AttrId, Val);
  }

  /// Adds Str sparse attribute with name \c AttrName to the given node with id \c NId.
  int AddSAttrDatN(const TInt& NId, const TStr& AttrName, const TStr& Val);
  /// Adds Str sparse attribute with id \c AttrId to the given node with id \c NId. 
  int AddSAttrDatN(const TInt& NId, const TInt& AttrId, const TStr& Val);

  /// Adds Str sparse attribute with name \c AttrName to \c NodeI.
  int AddSAttrDatN(const TNodeI& NodeI, const TStr& AttrName, const TStr& Val) {
    return AddSAttrDatN(NodeI.GetId(), AttrName, Val);
  }
  /// Adds Str sparse attribute with id \c AttrId to \c NodeI.
  int AddSAttrDatN(const TNodeI& NodeI, const TInt& AttrId, const TStr& Val) {
    return AddSAttrDatN(NodeI.GetId(), AttrId, Val);
  }

  /// Gets Int sparse attribute with name \c AttrName from node with id \c NId.
  int GetSAttrDatN(const TInt& NId, const TStr& AttrName, TInt& ValX) const;
  /// Gets Int sparse attribute with id \c AttrId from node with id \c NId. 
  int GetSAttrDatN(const TInt& NId, const TInt& AttrId, TInt& ValX) const;

  /// Gets Int sparse attribute with name \c AttrName from \c NodeI.
  int GetSAttrDatN(const TNodeI& NodeI, const TStr& AttrName, TInt& ValX) const {
    return GetSAttrDatN(NodeI.GetId(), AttrName, ValX);
  }
  /// Gets Int sparse attribute with id \c AttrId from \c NodeI.
  int GetSAttrDatN(const TNodeI& NodeI, const TInt& AttrId, TInt& ValX) const {
    return GetSAttrDatN(NodeI.GetId(), AttrId, ValX);
  }

  /// Gets Flt sparse attribute with name \c AttrName from node with id \c NId.
  int GetSAttrDatN(const TInt& NId, const TStr& AttrName, TFlt& ValX) const;
  /// Gets Flt sparse attribute with id \c AttrId from node with id \c NId.
  int GetSAttrDatN(const TInt& NId, const TInt& AttrId, TFlt& ValX) const;

  /// Gets Flt sparse attribute with name \c AttrName from \c NodeI.
  int GetSAttrDatN(const TNodeI& NodeI, const TStr& AttrName, TFlt& ValX) const {
    return GetSAttrDatN(NodeI.GetId(), AttrName, ValX);
  } 
  /// Gets Flt sparse attribute with id \c AttrId from \c NodeI.
  int GetSAttrDatN(const TNodeI& NodeI, const TInt& AttrId, TFlt& ValX) const {
    return GetSAttrDatN(NodeI.GetId(), AttrId, ValX);
  }

  /// Gets Str sparse attribute with name \c AttrName from node with id \c NId.
  int GetSAttrDatN(const TInt& NId, const TStr& AttrName, TStr& ValX) const;
  /// Gets Str sparse attribute with id \c AttrId from node with id \c NId.
  int GetSAttrDatN(const TInt& NId, const TInt& AttrId, TStr& ValX) const;

  /// Gets Str sparse attribute with name \c AttrName from \c NodeI.
  int GetSAttrDatN(const TNodeI& NodeI, const TStr& AttrName, TStr& ValX) const {
    return GetSAttrDatN(NodeI.GetId(), AttrName, ValX);
  }
  /// Gets Str sparse attribute with id \c AttrId from \c NodeI.
  int GetSAttrDatN(const TNodeI& NodeI, const TInt& AttrId, TStr& ValX) const {
    return GetSAttrDatN(NodeI.GetId(), AttrId, ValX);
  }

  /// Deletes sparse attribute with name \c AttrName from node with id \c NId.
  int DelSAttrDatN(const TInt& NId, const TStr& AttrName);
  /// Deletes sparse attribute with id \c AttrId from node with id \c NId.
  int DelSAttrDatN(const TInt& NId, const TInt& AttrId);

  /// Deletes sparse attribute with name \c AttrName from \c NodeI.
  int DelSAttrDatN(const TNodeI& NodeI, const TStr& AttrName) {
    return DelSAttrDatN(NodeI.GetId(), AttrName);
  }
  /// Deletes sparse attribute with id \c AttrId from \c NodeI.
  int DelSAttrDatN(const TNodeI& NodeI, const TInt& AttrId) {
    return DelSAttrDatN(NodeI.GetId(), AttrId);
  }

  /// Gets a list of all sparse attributes of type \c AttrType for node with id \c NId.
  int GetSAttrVN(const TInt& NId, const TAttrType AttrType, TAttrPrV& AttrV) const;
  /// Gets a list of all sparse attributes of type \c AttrType for \c NodeI.
  int GetSAttrVN(const TNodeI& NodeI, const TAttrType AttrType, TAttrPrV& AttrV) const {
    return GetSAttrVN(NodeI.GetId(), AttrType, AttrV);
  }

  /// Gets a list of all nodes that have a sparse attribute with name \c AttrName.
  int GetIdVSAttrN(const TStr& AttrName, TIntV& IdV) const;
  /// Gets a list of all nodes that have a sparse attribute with id \c AttrId.
  int GetIdVSAttrN(const TInt& AttrId, TIntV& IdV) const;

  /// Adds mapping for sparse attribute with name \c Name and type \c AttrType.
  int AddSAttrN(const TStr& Name, const TAttrType& AttrType, TInt& AttrId);

  /// Gets id and type for attribute with name \c Name.
  int GetSAttrIdN(const TStr& Name, TInt& AttrIdX, TAttrType& AttrTypeX) const;
  /// Gets name and type for attribute with id \c AttrId.
  int GetSAttrNameN(const TInt& AttrId, TStr& NameX, TAttrType& AttrTypeX) const;

  /// Adds Int sparse attribute with name \c AttrName to the given edge with id \c EId.
  int AddSAttrDatE(const TInt& EId, const TStr& AttrName, const TInt& Val);
  /// Adds Int sparse attribute with id \c AttrId to the given edge with id \c EId. 
  int AddSAttrDatE(const TInt& EId, const TInt& AttrId, const TInt& Val);

  /// Adds Int sparse attribute with name \c AttrName to \c EdgeI.
  int AddSAttrDatE(const TEdgeI& EdgeI, const TStr& AttrName, const TInt& Val) {
    return AddSAttrDatE(EdgeI.GetId(), AttrName, Val);
  }
  /// Adds Int sparse attribute with id \c AttrId to \c EdgeI.
  int AddSAttrDatE(const TEdgeI& EdgeI, const TInt& AttrId, const TInt& Val) {
    return AddSAttrDatE(EdgeI.GetId(), AttrId, Val);
  }

  /// Adds Flt sparse attribute with name \c AttrName to the given edge with id \c EId.
  int AddSAttrDatE(const TInt& EId, const TStr& AttrName, const TFlt& Val);
  /// Adds Flt sparse attribute with id \c AttrId to the given edge with id \c EId.
  int AddSAttrDatE(const TInt& EId, const TInt& AttrId, const TFlt& Val);

  /// Adds Flt sparse attribute with name \c AttrName to \c EdgeI.
  int AddSAttrDatE(const TEdgeI& EdgeI, const TStr& AttrName, const TFlt& Val) {
    return AddSAttrDatE(EdgeI.GetId(), AttrName, Val);
  }
  /// Adds Flt sparse attribute with id \c AttrId to \c EdgeI.
  int AddSAttrDatE(const TEdgeI& EdgeI, const TInt& AttrId, const TFlt& Val){
    return AddSAttrDatE(EdgeI.GetId(), AttrId, Val);
  }

  /// Adds Str sparse attribute with name \c AttrName to the given edge with id \c EId.
  int AddSAttrDatE(const TInt& EId, const TStr& AttrName, const TStr& Val);
  /// Adds Str sparse attribute with id \c AttrId to the given edge with id \c EId.
  int AddSAttrDatE(const TInt& EId, const TInt& AttrId, const TStr& Val);

  /// Adds Str sparse attribute with name \c AttrName to \c EdgeI.
  int AddSAttrDatE(const TEdgeI& EdgeI, const TStr& AttrName, const TStr& Val) {
    return AddSAttrDatE(EdgeI.GetId(), AttrName, Val);
  }
  /// Adds Str sparse attribute with id \c AttrId to \c EdgeI.
  int AddSAttrDatE(const TEdgeI& EdgeI, const TInt& AttrId, const TStr& Val) {
    return AddSAttrDatE(EdgeI.GetId(), AttrId, Val);
  }

  /// Gets Int sparse attribute with name \c AttrName from edge with id \c EId.
  int GetSAttrDatE(const TInt& EId, const TStr& AttrName, TInt& ValX) const;
  /// Gets Int sparse attribute with id \c AttrId from edge with id \c EId.
  int GetSAttrDatE(const TInt& EId, const TInt& AttrId, TInt& ValX) const;

  /// Gets Int sparse attribute with name \c AttrName from \c EdgeI.
  int GetSAttrDatE(const TEdgeI& EdgeI, const TStr& AttrName, TInt& ValX) const {
    return GetSAttrDatE(EdgeI.GetId(), AttrName, ValX);
  }
  /// Gets Int sparse attribute with id \c AttrId from \c EdgeI.
  int GetSAttrDatE(const TEdgeI& EdgeI, const TInt& AttrId, TInt& ValX) const {
    return GetSAttrDatE(EdgeI.GetId(), AttrId, ValX);
  } 

  /// Gets Flt sparse attribute with name \c AttrName from edge with id \c EId.
  int GetSAttrDatE(const TInt& EId, const TStr& AttrName, TFlt& ValX) const; 
  /// Gets Flt sparse attribute with id \c AttrId from edge with id \c EId.
  int GetSAttrDatE(const TInt& EId, const TInt& AttrId, TFlt& ValX) const;

  /// Gets Flt sparse attribute with name \c AttrName from \c EdgeI.
  int GetSAttrDatE(const TEdgeI& EdgeI, const TStr& AttrName, TFlt& ValX) const {
    return GetSAttrDatE(EdgeI.GetId(), AttrName, ValX);
  }
  /// Gets Flt sparse attribute with id \c AttrId from \c EdgeI.
  int GetSAttrDatE(const TEdgeI& EdgeI, const TInt& AttrId, TFlt& ValX) const {
    return GetSAttrDatE(EdgeI.GetId(), AttrId, ValX);
  } 

  /// Gets Str sparse attribute with name \c AttrName from edge with id \c EId.
  int GetSAttrDatE(const TInt& EId, const TStr& AttrName, TStr& ValX) const;
  /// Gets Str sparse attribute with id \c AttrId from edge with id \c EId.
  int GetSAttrDatE(const TInt& EId, const TInt& AttrId, TStr& ValX) const;

  /// Gets Str sparse attribute with name \c AttrName from \c EdgeI.
  int GetSAttrDatE(const TEdgeI& EdgeI, const TStr& AttrName, TStr& ValX) const {
    return GetSAttrDatE(EdgeI.GetId(), AttrName, ValX);
  }
  /// Gets Str sparse attribute with id \c AttrId from \c EdgeI.
  int GetSAttrDatE(const TEdgeI& EdgeI, const TInt& AttrId, TStr& ValX) const {
    return GetSAttrDatE(EdgeI.GetId(), AttrId, ValX);
  }

  /// Deletes sparse attribute with name \c AttrName from edge with id \c EId.
  int DelSAttrDatE(const TInt& EId, const TStr& AttrName);
  /// Deletes sparse attribute with id \c AttrId from edge with id \c EId.
  int DelSAttrDatE(const TInt& EId, const TInt& AttrId);

  /// Deletes sparse attribute with name \c AttrName from \c EdgeI.
  int DelSAttrDatE(const TEdgeI& EdgeI, const TStr& AttrName) {
    return DelSAttrDatE(EdgeI.GetId(), AttrName);
  }
  /// Deletes sparse attribute with id \c AttrId from \c EdgeI.
  int DelSAttrDatE(const TEdgeI& EdgeI, const TInt& AttrId) {
    return DelSAttrDatE(EdgeI.GetId(), AttrId);
  } 
  /// Gets a list of all sparse attributes of type \c AttrType for edge with id \c EId.
  int GetSAttrVE(const TInt& EId, const TAttrType AttrType, TAttrPrV& AttrV) const;
  /// Gets a list of all sparse attributes of type \c AttrType for \c EdgeI.
  int GetSAttrVE(const TEdgeI& EdgeI, const TAttrType AttrType, TAttrPrV& AttrV) const {
    return GetSAttrVE(EdgeI.GetId(), AttrType, AttrV);
  }

  /// Gets a list of all edges that have a sparse attribute with name \c AttrName.
  int GetIdVSAttrE(const TStr& AttrName, TIntV& IdV) const;
  /// Gets a list of all edges that have a sparse attribute with id \c AttrId.
  int GetIdVSAttrE(const TInt& AttrId, TIntV& IdV) const;

  /// Adds mapping for sparse attribute with name \c Name and type \c AttrType.
  int AddSAttrE(const TStr& Name, const TAttrType& AttrType, TInt& AttrId);

  /// Gets id and type for attribute with name \c Name.
  int GetSAttrIdE(const TStr& Name, TInt& AttrIdX, TAttrType& AttrTypeX) const;
  /// Gets name and type for attribute with id \c AttrId.
  int GetSAttrNameE(const TInt& AttrId, TStr& NameX, TAttrType& AttrTypeX) const;

  /// Returns a small multigraph on 5 nodes and 6 edges. ##TNEANet::GetSmallGraph
  static PNEANet GetSmallGraph();
  friend class TPt<TNEANet>;
};

// set flags
namespace TSnap {
template <> struct IsMultiGraph<TNEANet> { enum { Val = 1 }; };
template <> struct IsDirected<TNEANet> { enum { Val = 1 }; };
}

 //#//////////////////////////////////////////////
/// Undirected networks

class TUndirNet;

/// Pointer to an undirected network (TUndirNet)
typedef TPt<TUndirNet> PUndirNet;

//#//////////////////////////////////////////////
/// Directed networks
class TDirNet;

/// Pointer to a directed network (TDirNet)
typedef TPt<TDirNet> PDirNet;

//#//////////////////////////////////////////////
/// Undirected network. ##TUndirNet::Class
class TUndirNet {
public:
  typedef TUndirNet TNet;
  typedef TPt<TUndirNet> PNet;
public:
  class TNode {
  private:
    TInt Id;
    TIntV NIdV;
  public:
    TNode() : Id(-1), NIdV() { }
    TNode(const int& NId) : Id(NId), NIdV() { }
    TNode(const TNode& Node) : Id(Node.Id), NIdV(Node.NIdV) { }
    TNode(TSIn& SIn) : Id(SIn), NIdV(SIn) { }
    void Save(TSOut& SOut) const { Id.Save(SOut); NIdV.Save(SOut); }
    int GetId() const { return Id; }
    int GetDeg() const { return NIdV.Len(); }
    int GetInDeg() const { return GetDeg(); }
    int GetOutDeg() const { return GetDeg(); }
    int GetInNId(const int& NodeN) const { return GetNbrNId(NodeN); }
    int GetOutNId(const int& NodeN) const { return GetNbrNId(NodeN); }
    int GetNbrNId(const int& NodeN) const { return NIdV[NodeN]; }
    bool IsNbrNId(const int& NId) const { return NIdV.SearchBin(NId)!=-1; }
    bool IsInNId(const int& NId) const { return IsNbrNId(NId); }
    bool IsOutNId(const int& NId) const { return IsNbrNId(NId); }
    void PackOutNIdV() { NIdV.Pack(); }
    void PackNIdV() { NIdV.Pack(); }
    void SortNIdV() { NIdV.Sort();}
    void LoadShM(TShMIn& MStream) {
      Id = TInt(MStream);
      NIdV.LoadShM(MStream);
    }
    friend class TUndirNet;
    friend class TUndirNetMtx;
  };
  /// Node iterator. Only forward iteration (operator++) is supported.
  class TNodeI {
  private:
    typedef THash<TInt, TNode>::TIter THashIter;
    THashIter NodeHI;
  public:
    TNodeI() : NodeHI() { }
    TNodeI(const THashIter& NodeHIter) : NodeHI(NodeHIter) { }
    TNodeI(const TNodeI& NodeI) : NodeHI(NodeI.NodeHI) { }
    TNodeI& operator = (const TNodeI& NodeI) { NodeHI = NodeI.NodeHI; return *this; }

    /// Increment iterator.
    TNodeI& operator++ (int) { NodeHI++; return *this; }
    /// Decrement iterator.
    TNodeI& operator-- (int) { NodeHI--; return *this; }


    bool operator < (const TNodeI& NodeI) const { return NodeHI < NodeI.NodeHI; }
    bool operator == (const TNodeI& NodeI) const { return NodeHI == NodeI.NodeHI; }

    /// Returns ID of the current node.
    int GetId() const { return NodeHI.GetDat().GetId(); }
    /// Returns degree of the current node.
    int GetDeg() const { return NodeHI.GetDat().GetDeg(); }
    /// Returns in-degree of the current node (returns same as value GetDeg() since the network is undirected).
    int GetInDeg() const { return NodeHI.GetDat().GetInDeg(); }
    /// Returns out-degree of the current node (returns same as value GetDeg() since the network is undirected).
    int GetOutDeg() const { return NodeHI.GetDat().GetOutDeg(); }
    /// Sorts the adjacency lists of the current node.
    void SortNIdV() { NodeHI.GetDat().SortNIdV(); }
    /// Returns ID of NodeN-th in-node (the node pointing to the current node). ##TUndirNet::TNodeI::GetInNId
    int GetInNId(const int& NodeN) const { return NodeHI.GetDat().GetInNId(NodeN); }
    /// Returns ID of NodeN-th out-node (the node the current node points to). ##TUndirNet::TNodeI::GetOutNId
    int GetOutNId(const int& NodeN) const { return NodeHI.GetDat().GetOutNId(NodeN); }
    /// Returns ID of NodeN-th neighboring node. ##TUndirNet::TNodeI::GetNbrNId
    int GetNbrNId(const int& NodeN) const { return NodeHI.GetDat().GetNbrNId(NodeN); }
    /// Tests whether node with ID NId points to the current node.
    bool IsInNId(const int& NId) const { return NodeHI.GetDat().IsInNId(NId); }
    /// Tests whether the current node points to node with ID NId.
    bool IsOutNId(const int& NId) const { return NodeHI.GetDat().IsOutNId(NId); }
    /// Tests whether node with ID NId is a neighbor of the current node.
    bool IsNbrNId(const int& NId) const { return NodeHI.GetDat().IsNbrNId(NId); }
    friend class TUndirNet;
  };
  /// Edge iterator. Only forward iteration (operator++) is supported.
  class TEdgeI {
  private:
    TNodeI CurNode, EndNode;
    int CurEdge;
  public:
    TEdgeI() : CurNode(), EndNode(), CurEdge(0) { }
    TEdgeI(const TNodeI& NodeI, const TNodeI& EndNodeI, const int& EdgeN=0) : CurNode(NodeI), EndNode(EndNodeI), CurEdge(EdgeN) { }
    TEdgeI(const TEdgeI& EdgeI) : CurNode(EdgeI.CurNode), EndNode(EdgeI.EndNode), CurEdge(EdgeI.CurEdge) { }
    TEdgeI& operator = (const TEdgeI& EdgeI) { if (this!=&EdgeI) { CurNode=EdgeI.CurNode; EndNode=EdgeI.EndNode; CurEdge=EdgeI.CurEdge; } return *this; }
    /// Increment iterator.
    TEdgeI& operator++ (int) { do { CurEdge++; if (CurEdge >= CurNode.GetOutDeg()) { CurEdge=0; CurNode++; while (CurNode < EndNode && CurNode.GetOutDeg()==0) { CurNode++; } } } while (CurNode < EndNode && GetSrcNId()>GetDstNId()); return *this; }
    bool operator < (const TEdgeI& EdgeI) const { return CurNode<EdgeI.CurNode || (CurNode==EdgeI.CurNode && CurEdge<EdgeI.CurEdge); }
    bool operator == (const TEdgeI& EdgeI) const { return CurNode == EdgeI.CurNode && CurEdge == EdgeI.CurEdge; }
    /// Returns edge ID. Always returns -1 since only edges in multigraphs have explicit IDs.
    int GetId() const { return -1; }
    /// Returns the source of the edge. Since the network is undirected, this is the node with a smaller ID of the edge endpoints.
    int GetSrcNId() const { return CurNode.GetId(); }
    /// Returns the destination of the edge. Since the network is undirected, this is the node with a greater ID of the edge endpoints.
    int GetDstNId() const { return CurNode.GetOutNId(CurEdge); }
    friend class TUndirNet;
  };
private:
  TCRef CRef;
  TInt MxNId, NEdges;
  THash<TInt, TNode> NodeH;

  TAttr SAttrN;
  TAttrPair SAttrE;
private:
  TNode& GetNode(const int& NId) { return NodeH.GetDat(NId); }
  const TNode& GetNode(const int& NId) const { return NodeH.GetDat(NId); }
  TIntPr OrderEdgeNodes(const int& SrcNId, const int& DstNId) const;
private:
  class LoadTNodeFunctor {
  public:
    LoadTNodeFunctor() {}
    void operator() (TNode* n, TShMIn& ShMIn) {n->LoadShM(ShMIn);}
  };
private:
  void LoadNetworkShM(TShMIn& ShMIn) {
    MxNId = TInt(ShMIn);
    NEdges = TInt(ShMIn);
    LoadTNodeFunctor NodeFn;
    NodeH.LoadShM(ShMIn, NodeFn);
    SAttrN.Load(ShMIn);
    SAttrE = TAttrPair(ShMIn);
  }
public:
  TUndirNet() : CRef(), MxNId(0), NEdges(0), NodeH(), SAttrN(), SAttrE() { }
  /// Constructor that reserves enough memory for a network of Nodes nodes and Edges edges.
  explicit TUndirNet(const int& Nodes, const int& Edges) : MxNId(0), NEdges(0), SAttrN(), SAttrE() { Reserve(Nodes, Edges); }
  TUndirNet(const TUndirNet& Graph) : MxNId(Graph.MxNId), NEdges(Graph.NEdges), NodeH(Graph.NodeH),
    SAttrN(), SAttrE() { }
  /// Constructor that loads the network from a (binary) stream SIn.
  TUndirNet(TSIn& SIn) : MxNId(SIn), NEdges(SIn), NodeH(SIn), SAttrN(SIn), SAttrE(SIn) { }
  /// Saves the network to a (binary) stream SOut. Expects data structures for sparse attributes.
  void Save(TSOut& SOut) const { MxNId.Save(SOut); NEdges.Save(SOut); NodeH.Save(SOut);
    SAttrN.Save(SOut); SAttrE.Save(SOut); }
  /// Saves the network to a (binary) stream SOut. Available for backwards compatibility.
  void Save_V1(TSOut& SOut) const { MxNId.Save(SOut); NEdges.Save(SOut); NodeH.Save(SOut); }
  /// Static constructor that returns a pointer to the network. Call: PUndirNet Graph = TUndirNet::New().
  static PUndirNet New() { return new TUndirNet(); }
  /// Static constructor that returns a pointer to the network and reserves enough memory for Nodes nodes and Edges edges. ##TUndirNet::New
  static PUndirNet New(const int& Nodes, const int& Edges) { return new TUndirNet(Nodes, Edges); }
  /// Static constructor that loads the network from a stream SIn and returns a pointer to it.
  static PUndirNet Load(TSIn& SIn) { return PUndirNet(new TUndirNet(SIn)); }
  /// Static constructor that loads the network from a stream SIn and returns a pointer to it. Backwards compatible.
  static PUndirNet Load_V1(TSIn& SIn) { PUndirNet Graph = PUndirNet(new TUndirNet());
    Graph->MxNId.Load(SIn); Graph->NEdges.Load(SIn); Graph->NodeH.Load(SIn); return Graph;
  }

  /// Static constructor that loads the network from memory. ##TUndirNet::LoadShM(TShMIn& ShMIn)
  static PUndirNet LoadShM(TShMIn& ShMIn) {
    TUndirNet* Network = new TUndirNet();
    Network->LoadNetworkShM(ShMIn);
    return PUndirNet(Network);
  }

  /// Allows for run-time checking the type of the network (see the TGraphFlag for flags).
  bool HasFlag(const TGraphFlag& Flag) const;
  TUndirNet& operator = (const TUndirNet& Graph) {
    if (this!=&Graph) { MxNId=Graph.MxNId; NEdges=Graph.NEdges; NodeH=Graph.NodeH; } return *this; }
  
  /// Returns the number of nodes in the network.
  int GetNodes() const { return NodeH.Len(); }
  /// Adds a node of ID NId to the network. ##TUndirNet::AddNode
  int AddNode(int NId = -1);
  /// Adds a node of ID NId to the network, noop if the node already exists. ##TUndirNet::AddNodeUnchecked
  int AddNodeUnchecked(int NId = -1);
  /// Adds a node of ID NodeI.GetId() to the network.
  int AddNode(const TNodeI& NodeI) { return AddNode(NodeI.GetId()); }
  /// Adds a node of ID NId to the network and create edges to all nodes in vector NbrNIdV. ##TUndirNet::AddNode-1
  int AddNode(const int& NId, const TIntV& NbrNIdV);
  /// Adds a node of ID NId to the network and create edges to all nodes in vector NIdVId in the vector pool Pool. ##TUndirNet::AddNode-2
  int AddNode(const int& NId, const TVecPool<TInt>& Pool, const int& NIdVId);
  /// Deletes node of ID NId from the network. ##TUndirNet::DelNode
  void DelNode(const int& NId);
  /// Deletes node of ID NodeI.GetId() from the network.
  void DelNode(const TNode& NodeI) { DelNode(NodeI.GetId()); }
  /// Tests whether ID NId is a node.
  bool IsNode(const int& NId) const { return NodeH.IsKey(NId); }
  /// Returns an iterator referring to the first node in the network.
  TNodeI BegNI() const { return TNodeI(NodeH.BegI()); }
  /// Returns an iterator referring to the past-the-end node in the network.
  TNodeI EndNI() const { return TNodeI(NodeH.EndI()); }
  /// Returns an iterator referring to the node of ID NId in the network.
  TNodeI GetNI(const int& NId) const { return TNodeI(NodeH.GetI(NId)); }
  /// Returns an ID that is larger than any node ID in the network.
  int GetMxNId() const { return MxNId; }

  /// Returns the number of edges in the network.
  int GetEdges() const;
  /// Adds an edge between node IDs SrcNId and DstNId to the network. ##TUndirNet::AddEdge
  int AddEdge(const int& SrcNId, const int& DstNId);
  /// Adds an edge between node IDs SrcNId and DstNId to the network. ##TUndirNet::AddEdgeUnchecked
  int AddEdgeUnchecked(const int& SrcNId, const int& DstNId);
  /// Adds an edge between EdgeI.GetSrcNId() and EdgeI.GetDstNId() to the network.
  int AddEdge(const TEdgeI& EdgeI) { return AddEdge(EdgeI.GetSrcNId(), EdgeI.GetDstNId()); }
  /// Deletes an edge between node IDs SrcNId and DstNId from the network. ##TUndirNet::DelEdge
  void DelEdge(const int& SrcNId, const int& DstNId);
  /// Tests whether an edge between node IDs SrcNId and DstNId exists in the network.
  bool IsEdge(const int& SrcNId, const int& DstNId) const;
  /// Returns an iterator referring to the first edge in the network.
  TEdgeI BegEI() const { TNodeI NI = BegNI(); TEdgeI EI(NI, EndNI(), 0); if (GetNodes() != 0 && (NI.GetOutDeg()==0 || NI.GetId()>NI.GetOutNId(0))) { EI++; } return EI; }
  /// Returns an iterator referring to the past-the-end edge in the network.
  TEdgeI EndEI() const { return TEdgeI(EndNI(), EndNI()); }
  /// Not supported/implemented!
  TEdgeI GetEI(const int& EId) const;
  /// Returns an iterator referring to edge (SrcNId, DstNId) in the network. ##TUndirNet::GetEI
  TEdgeI GetEI(const int& SrcNId, const int& DstNId) const;

  /// Returns an ID of a random node in the network.
  int GetRndNId(TRnd& Rnd=TInt::Rnd) { return NodeH.GetKey(NodeH.GetRndKeyId(Rnd, 0.8)); }
  /// Returns an interator referring to a random node in the network.
  TNodeI GetRndNI(TRnd& Rnd=TInt::Rnd) { return GetNI(GetRndNId(Rnd)); }
  /// Gets a vector IDs of all nodes in the network.
  void GetNIdV(TIntV& NIdV) const;

  /// Tests whether the network is empty (has zero nodes).
  bool Empty() const { return GetNodes()==0; }
  /// Deletes all nodes and edges from the network.
  void Clr() { MxNId=0; NEdges=0; NodeH.Clr(); SAttrN.Clr(); SAttrE.Clr(); }
  /// Reserves memory for a network of Nodes nodes and Edges edges.
  void Reserve(const int& Nodes, const int& Edges) { if (Nodes>0) NodeH.Gen(Nodes/2); }
  /// Reserves memory for node ID NId having Deg edges.
  void ReserveNIdDeg(const int& NId, const int& Deg) { GetNode(NId).NIdV.Reserve(Deg); }
  /// Sorts the adjacency lists of each node.
  void SortNodeAdjV() { for (TNodeI NI = BegNI(); NI < EndNI(); NI++) { NI.SortNIdV();} }
  /// Defragments the network. ##TUndirNet::Defrag
  void Defrag(const bool& OnlyNodeLinks=false);
  /// Checks the network data structure for internal consistency. ##TUndirNet::IsOk
  bool IsOk(const bool& ThrowExcept=true) const;
  /// Print the network in a human readable form to an output stream OutF.
  void Dump(FILE *OutF=stdout) const;
  /// Returns a small network on 5 nodes and 5 edges. ##TUndirNet::GetSmallGraph
  static PUndirNet GetSmallGraph();

  /// Adds Int sparse attribute with name \c AttrName to the given node with id \c NId.
  int AddSAttrDatN(const TInt& NId, const TStr& AttrName, const TInt& Val);
  /// Adds Int sparse attribute with id \c AttrId to the given node with id \c NId.
  int AddSAttrDatN(const TInt& NId, const TInt& AttrId, const TInt& Val);

  /// Adds Int sparse attribute with name \c AttrName to \c NodeI.
  int AddSAttrDatN(const TNodeI& NodeI, const TStr& AttrName, const TInt& Val) {
    return AddSAttrDatN(NodeI.GetId(), AttrName, Val);
  }
  /// Adds Int sparse attribute with id \c AttrId to \c NodeI.
  int AddSAttrDatN(const TNodeI& NodeI, const TInt& AttrId, const TInt& Val) {
    return AddSAttrDatN(NodeI.GetId(), AttrId, Val);
  }

  /// Adds Flt sparse attribute with name \c AttrName to the given node with id \c NId.
  int AddSAttrDatN(const TInt& NId, const TStr& AttrName, const TFlt& Val);
  /// Adds Flt sparse attribute with id \c AttrId to the given node with id \c NId.
  int AddSAttrDatN(const TInt& NId, const TInt& AttrId, const TFlt& Val);

  /// Adds Flt sparse attribute with name \c AttrName to \c NodeI.
  int AddSAttrDatN(const TNodeI& NodeI, const TStr& AttrName, const TFlt& Val) {
    return AddSAttrDatN(NodeI.GetId(), AttrName, Val);
  }
  /// Adds Flt sparse attribute with id \c AttrId to \c NodeI.
  int AddSAttrDatN(const TNodeI& NodeI, const TInt& AttrId, const TFlt& Val) {
    return AddSAttrDatN(NodeI.GetId(), AttrId, Val);
  }

  /// Adds Str sparse attribute with name \c AttrName to the given node with id \c NId.
  int AddSAttrDatN(const TInt& NId, const TStr& AttrName, const TStr& Val);
  /// Adds Str sparse attribute with id \c AttrId to the given node with id \c NId. 
  int AddSAttrDatN(const TInt& NId, const TInt& AttrId, const TStr& Val);

  /// Adds Str sparse attribute with name \c AttrName to \c NodeI.
  int AddSAttrDatN(const TNodeI& NodeI, const TStr& AttrName, const TStr& Val) {
    return AddSAttrDatN(NodeI.GetId(), AttrName, Val);
  }
  /// Adds Str sparse attribute with id \c AttrId to \c NodeI.
  int AddSAttrDatN(const TNodeI& NodeI, const TInt& AttrId, const TStr& Val) {
    return AddSAttrDatN(NodeI.GetId(), AttrId, Val);
  }

  /// Gets Int sparse attribute with name \c AttrName from node with id \c NId.
  int GetSAttrDatN(const TInt& NId, const TStr& AttrName, TInt& ValX) const;
  /// Gets Int sparse attribute with id \c AttrId from node with id \c NId. 
  int GetSAttrDatN(const TInt& NId, const TInt& AttrId, TInt& ValX) const;

  /// Gets Int sparse attribute with name \c AttrName from \c NodeI.
  int GetSAttrDatN(const TNodeI& NodeI, const TStr& AttrName, TInt& ValX) const {
    return GetSAttrDatN(NodeI.GetId(), AttrName, ValX);
  }
  /// Gets Int sparse attribute with id \c AttrId from \c NodeI.
  int GetSAttrDatN(const TNodeI& NodeI, const TInt& AttrId, TInt& ValX) const {
    return GetSAttrDatN(NodeI.GetId(), AttrId, ValX);
  }

  /// Gets Flt sparse attribute with name \c AttrName from node with id \c NId.
  int GetSAttrDatN(const TInt& NId, const TStr& AttrName, TFlt& ValX) const;
  /// Gets Flt sparse attribute with id \c AttrId from node with id \c NId.
  int GetSAttrDatN(const TInt& NId, const TInt& AttrId, TFlt& ValX) const;

  /// Gets Flt sparse attribute with name \c AttrName from \c NodeI.
  int GetSAttrDatN(const TNodeI& NodeI, const TStr& AttrName, TFlt& ValX) const {
    return GetSAttrDatN(NodeI.GetId(), AttrName, ValX);
  } 
  /// Gets Flt sparse attribute with id \c AttrId from \c NodeI.
  int GetSAttrDatN(const TNodeI& NodeI, const TInt& AttrId, TFlt& ValX) const {
    return GetSAttrDatN(NodeI.GetId(), AttrId, ValX);
  }

  /// Gets Str sparse attribute with name \c AttrName from node with id \c NId.
  int GetSAttrDatN(const TInt& NId, const TStr& AttrName, TStr& ValX) const;
  /// Gets Str sparse attribute with id \c AttrId from node with id \c NId.
  int GetSAttrDatN(const TInt& NId, const TInt& AttrId, TStr& ValX) const;

  /// Gets Str sparse attribute with name \c AttrName from \c NodeI.
  int GetSAttrDatN(const TNodeI& NodeI, const TStr& AttrName, TStr& ValX) const {
    return GetSAttrDatN(NodeI.GetId(), AttrName, ValX);
  }
  /// Gets Str sparse attribute with id \c AttrId from \c NodeI.
  int GetSAttrDatN(const TNodeI& NodeI, const TInt& AttrId, TStr& ValX) const {
    return GetSAttrDatN(NodeI.GetId(), AttrId, ValX);
  }

  /// Deletes sparse attribute with name \c AttrName from node with id \c NId.
  int DelSAttrDatN(const TInt& NId, const TStr& AttrName);
  /// Deletes sparse attribute with id \c AttrId from node with id \c NId.
  int DelSAttrDatN(const TInt& NId, const TInt& AttrId);

  /// Deletes sparse attribute with name \c AttrName from \c NodeI.
  int DelSAttrDatN(const TNodeI& NodeI, const TStr& AttrName) {
    return DelSAttrDatN(NodeI.GetId(), AttrName);
  }
  /// Deletes sparse attribute with id \c AttrId from \c NodeI.
  int DelSAttrDatN(const TNodeI& NodeI, const TInt& AttrId) {
    return DelSAttrDatN(NodeI.GetId(), AttrId);
  }

  /// Gets a list of all sparse attributes of type \c AttrType for node with id \c NId.
  int GetSAttrVN(const TInt& NId, const TAttrType AttrType, TAttrPrV& AttrV) const;
  /// Gets a list of all sparse attributes of type \c AttrType for \c NodeI.
  int GetSAttrVN(const TNodeI& NodeI, const TAttrType AttrType, TAttrPrV& AttrV) const {
    return GetSAttrVN(NodeI.GetId(), AttrType, AttrV);
  }

  /// Gets a list of all nodes that have a sparse attribute with name \c AttrName.
  int GetIdVSAttrN(const TStr& AttrName, TIntV& IdV) const;
  /// Gets a list of all nodes that have a sparse attribute with id \c AttrId.
  int GetIdVSAttrN(const TInt& AttrId, TIntV& IdV) const;

  /// Adds mapping for sparse attribute with name \c Name and type \c AttrType.
  int AddSAttrN(const TStr& Name, const TAttrType& AttrType, TInt& AttrId);

  /// Gets id and type for attribute with name \c Name.
  int GetSAttrIdN(const TStr& Name, TInt& AttrIdX, TAttrType& AttrTypeX) const;
  /// Gets name and type for attribute with id \c AttrId.
  int GetSAttrNameN(const TInt& AttrId, TStr& NameX, TAttrType& AttrTypeX) const;

  /// Adds Int sparse attribute with name \c AttrName to the given edge with ids \c SrcId and \c DstId.
  int AddSAttrDatE(const int& SrcNId, const int& DstNId, const TStr& AttrName, const TInt& Val);
  /// Adds Int sparse attribute with id \c AttrId to the given edge with ids \c SrcId and \c DstId.
  int AddSAttrDatE(const int& SrcNId, const int& DstNId, const TInt& AttrId, const TInt& Val);

  /// Adds Int sparse attribute with name \c AttrName to \c EdgeI.
  int AddSAttrDatE(const TEdgeI& EdgeI, const TStr& AttrName, const TInt& Val) {
    return AddSAttrDatE(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), AttrName, Val);
  }
  /// Adds Int sparse attribute with id \c AttrId to \c EdgeI.
  int AddSAttrDatE(const TEdgeI& EdgeI, const TInt& AttrId, const TInt& Val) {
    return AddSAttrDatE(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), AttrId, Val);
  }

  /// Adds Flt sparse attribute with name \c AttrName to the given edge with ids \c SrcId and \c DstId.
  int AddSAttrDatE(const int& SrcNId, const int& DstNId, const TStr& AttrName, const TFlt& Val);
  /// Adds Flt sparse attribute with id \c AttrId to the given edge with ids \c SrcId and \c DstId.
  int AddSAttrDatE(const int& SrcNId, const int& DstNId, const TInt& AttrId, const TFlt& Val);

  /// Adds Flt sparse attribute with name \c AttrName to \c EdgeI.
  int AddSAttrDatE(const TEdgeI& EdgeI, const TStr& AttrName, const TFlt& Val) {
    return AddSAttrDatE(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), AttrName, Val);
  }
  /// Adds Flt sparse attribute with id \c AttrId to \c EdgeI.
  int AddSAttrDatE(const TEdgeI& EdgeI, const TInt& AttrId, const TFlt& Val){
    return AddSAttrDatE(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), AttrId, Val);
  }

  /// Adds Str sparse attribute with name \c AttrName to the given edge with ids \c SrcId and \c DstId.
  int AddSAttrDatE(const int& SrcNId, const int& DstNId, const TStr& AttrName, const TStr& Val);
  /// Adds Str sparse attribute with id \c AttrId to the given edge with ids \c SrcId and \c DstId.
  int AddSAttrDatE(const int& SrcNId, const int& DstNId, const TInt& AttrId, const TStr& Val);

  /// Adds Str sparse attribute with name \c AttrName to \c EdgeI.
  int AddSAttrDatE(const TEdgeI& EdgeI, const TStr& AttrName, const TStr& Val) {
    return AddSAttrDatE(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), AttrName, Val);
  }
  /// Adds Str sparse attribute with id \c AttrId to \c EdgeI.
  int AddSAttrDatE(const TEdgeI& EdgeI, const TInt& AttrId, const TStr& Val) {
    return AddSAttrDatE(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), AttrId, Val);
  }

  /// Gets Int sparse attribute with name \c AttrName from edge with ids \c SrcId and \c DstId.
  int GetSAttrDatE(const int& SrcNId, const int& DstNId, const TStr& AttrName, TInt& ValX) const;
  /// Gets Int sparse attribute with id \c AttrId from edge with ids \c SrcId and \c DstId.
  int GetSAttrDatE(const int& SrcNId, const int& DstNId, const TInt& AttrId, TInt& ValX) const;

  /// Gets Int sparse attribute with name \c AttrName from \c EdgeI.
  int GetSAttrDatE(const TEdgeI& EdgeI, const TStr& AttrName, TInt& ValX) const {
    return GetSAttrDatE(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), AttrName, ValX);
  }
  /// Gets Int sparse attribute with id \c AttrId from \c EdgeI.
  int GetSAttrDatE(const TEdgeI& EdgeI, const TInt& AttrId, TInt& ValX) const {
    return GetSAttrDatE(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), AttrId, ValX);
  } 

  /// Gets Flt sparse attribute with name \c AttrName from edge with ids \c SrcId and \c DstId.
  int GetSAttrDatE(const int& SrcNId, const int& DstNId, const TStr& AttrName, TFlt& ValX) const; 
  /// Gets Flt sparse attribute with id \c AttrId from edge with ids \c SrcId and \c DstId.
  int GetSAttrDatE(const int& SrcNId, const int& DstNId, const TInt& AttrId, TFlt& ValX) const;

  /// Gets Flt sparse attribute with name \c AttrName from \c EdgeI.
  int GetSAttrDatE(const TEdgeI& EdgeI, const TStr& AttrName, TFlt& ValX) const {
    return GetSAttrDatE(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), AttrName, ValX);
  }
  /// Gets Flt sparse attribute with id \c AttrId from \c EdgeI.
  int GetSAttrDatE(const TEdgeI& EdgeI, const TInt& AttrId, TFlt& ValX) const {
    return GetSAttrDatE(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), AttrId, ValX);
  } 

  /// Gets Str sparse attribute with name \c AttrName from edge with ids \c SrcId and \c DstId.
  int GetSAttrDatE(const int& SrcNId, const int& DstNId, const TStr& AttrName, TStr& ValX) const;
  /// Gets Str sparse attribute with id \c AttrId from edge with ids \c SrcId and \c DstId.
  int GetSAttrDatE(const int& SrcNId, const int& DstNId, const TInt& AttrId, TStr& ValX) const;

  /// Gets Str sparse attribute with name \c AttrName from \c EdgeI.
  int GetSAttrDatE(const TEdgeI& EdgeI, const TStr& AttrName, TStr& ValX) const {
    return GetSAttrDatE(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), AttrName, ValX);
  }
  /// Gets Str sparse attribute with id \c AttrId from \c EdgeI.
  int GetSAttrDatE(const TEdgeI& EdgeI, const TInt& AttrId, TStr& ValX) const {
    return GetSAttrDatE(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), AttrId, ValX);
  }

  /// Deletes sparse attribute with name \c AttrName from edge with ids \c SrcId and \c DstId.
  int DelSAttrDatE(const int& SrcNId, const int& DstNId, const TStr& AttrName);
  /// Deletes sparse attribute with id \c AttrId from edge with ids \c SrcId and \c DstId.
  int DelSAttrDatE(const int& SrcNId, const int& DstNId, const TInt& AttrId);

  /// Deletes sparse attribute with name \c AttrName from \c EdgeI.
  int DelSAttrDatE(const TEdgeI& EdgeI, const TStr& AttrName) {
    return DelSAttrDatE(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), AttrName);
  }
  /// Deletes sparse attribute with id \c AttrId from \c EdgeI.
  int DelSAttrDatE(const TEdgeI& EdgeI, const TInt& AttrId) {
    return DelSAttrDatE(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), AttrId);
  } 
  /// Gets a list of all sparse attributes of type \c AttrType for edge with ids \c SrcId and \c DstId.
  int GetSAttrVE(const int& SrcNId, const int& DstNId, const TAttrType AttrType, TAttrPrV& AttrV) const;
  /// Gets a list of all sparse attributes of type \c AttrType for \c EdgeI.
  int GetSAttrVE(const TEdgeI& EdgeI, const TAttrType AttrType, TAttrPrV& AttrV) const {
    return GetSAttrVE(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), AttrType, AttrV);
  }

  /// Gets a list of all edges that have a sparse attribute with name \c AttrName.
  int GetIdVSAttrE(const TStr& AttrName, TIntPrV& IdV) const;
  /// Gets a list of all edges that have a sparse attribute with id \c AttrId.
  int GetIdVSAttrE(const TInt& AttrId, TIntPrV& IdV) const;

  /// Adds mapping for sparse attribute with name \c Name and type \c AttrType.
  int AddSAttrE(const TStr& Name, const TAttrType& AttrType, TInt& AttrId);

  /// Gets id and type for attribute with name \c Name.
  int GetSAttrIdE(const TStr& Name, TInt& AttrIdX, TAttrType& AttrTypeX) const;
  /// Gets name and type for attribute with id \c AttrId.
  int GetSAttrNameE(const TInt& AttrId, TStr& NameX, TAttrType& AttrTypeX) const;

  friend class TUndirNetMtx;
  friend class TPt<TUndirNet>;
};

//#//////////////////////////////////////////////
/// Directed network. ##TDirNet::Class
class TDirNet {
public:
  typedef TDirNet TNet;
  typedef TPt<TDirNet> PNet;
public:
  class TNode {
  private:
    TInt Id;
    TIntV InNIdV, OutNIdV;
  public:
    TNode() : Id(-1), InNIdV(), OutNIdV() { }
    TNode(const int& NId) : Id(NId), InNIdV(), OutNIdV() { }
    TNode(const TNode& Node) : Id(Node.Id), InNIdV(Node.InNIdV), OutNIdV(Node.OutNIdV) { }
    TNode(TSIn& SIn) : Id(SIn), InNIdV(SIn), OutNIdV(SIn) { }
    void Save(TSOut& SOut) const { Id.Save(SOut); InNIdV.Save(SOut); OutNIdV.Save(SOut); }
    int GetId() const { return Id; }
    int GetDeg() const { return GetInDeg() + GetOutDeg(); }
    int GetInDeg() const { return InNIdV.Len(); }
    int GetOutDeg() const { return OutNIdV.Len(); }
    int GetInNId(const int& NodeN) const { return InNIdV[NodeN]; }
    int GetOutNId(const int& NodeN) const { return OutNIdV[NodeN]; }
    int GetNbrNId(const int& NodeN) const { return NodeN<GetOutDeg()?GetOutNId(NodeN):GetInNId(NodeN-GetOutDeg()); }
    bool IsInNId(const int& NId) const { return InNIdV.SearchBin(NId) != -1; }
    bool IsOutNId(const int& NId) const { return OutNIdV.SearchBin(NId) != -1; }
    bool IsNbrNId(const int& NId) const { return IsOutNId(NId) || IsInNId(NId); }
    void PackOutNIdV() { OutNIdV.Pack(); }
    void PackNIdV() { InNIdV.Pack(); }
    void SortNIdV() { InNIdV.Sort(); OutNIdV.Sort();}
    void LoadShM(TShMIn& MStream) {
      Id = TInt(MStream);
      InNIdV.LoadShM(MStream);
      OutNIdV.LoadShM(MStream);
    }
    friend class TDirNet;
    friend class TDirNetMtx;
  };
  /// Node iterator. Only forward iteration (operator++) is supported.
  class TNodeI {
  private:
    typedef THash<TInt, TNode>::TIter THashIter;
    THashIter NodeHI;
  public:
    TNodeI() : NodeHI() { }
    TNodeI(const THashIter& NodeHIter) : NodeHI(NodeHIter) { }
    TNodeI(const TNodeI& NodeI) : NodeHI(NodeI.NodeHI) { }
    TNodeI& operator = (const TNodeI& NodeI) { NodeHI = NodeI.NodeHI; return *this; }
    /// Increment iterator.
    TNodeI& operator++ (int) { NodeHI++; return *this; }
    /// Decrement iterator.
    TNodeI& operator-- (int) { NodeHI--; return *this; }

    bool operator < (const TNodeI& NodeI) const { return NodeHI < NodeI.NodeHI; }
    bool operator == (const TNodeI& NodeI) const { return NodeHI == NodeI.NodeHI; }
    /// Returns ID of the current node.
    int GetId() const { return NodeHI.GetDat().GetId(); }
    /// Returns degree of the current node, the sum of in-degree and out-degree.
    int GetDeg() const { return NodeHI.GetDat().GetDeg(); }
    /// Returns in-degree of the current node.
    int GetInDeg() const { return NodeHI.GetDat().GetInDeg(); }
    /// Returns out-degree of the current node.
    int GetOutDeg() const { return NodeHI.GetDat().GetOutDeg(); }
    /// Sorts the adjacency lists of the current node.
    void SortNIdV() { NodeHI.GetDat().SortNIdV(); }
    /// Returns ID of NodeN-th in-node (the node pointing to the current node). ##TDirNet::TNodeI::GetInNId
    int GetInNId(const int& NodeN) const { return NodeHI.GetDat().GetInNId(NodeN); }
    /// Returns ID of NodeN-th out-node (the node the current node points to). ##TDirNet::TNodeI::GetOutNId
    int GetOutNId(const int& NodeN) const { return NodeHI.GetDat().GetOutNId(NodeN); }
    /// Returns ID of NodeN-th neighboring node. ##TDirNet::TNodeI::GetNbrNId
    int GetNbrNId(const int& NodeN) const { return NodeHI.GetDat().GetNbrNId(NodeN); }
    /// Tests whether node with ID NId points to the current node.
    bool IsInNId(const int& NId) const { return NodeHI.GetDat().IsInNId(NId); }
    /// Tests whether the current node points to node with ID NId.
    bool IsOutNId(const int& NId) const { return NodeHI.GetDat().IsOutNId(NId); }
    /// Tests whether node with ID NId is a neighbor of the current node.
    bool IsNbrNId(const int& NId) const { return IsOutNId(NId) || IsInNId(NId); }
    friend class TDirNet;
  };
  /// Edge iterator. Only forward iteration (operator++) is supported.
  class TEdgeI {
  private:
    TNodeI CurNode, EndNode;
    int CurEdge;
  public:
    TEdgeI() : CurNode(), EndNode(), CurEdge(0) { }
    TEdgeI(const TNodeI& NodeI, const TNodeI& EndNodeI, const int& EdgeN=0) : CurNode(NodeI), EndNode(EndNodeI), CurEdge(EdgeN) { }
    TEdgeI(const TEdgeI& EdgeI) : CurNode(EdgeI.CurNode), EndNode(EdgeI.EndNode), CurEdge(EdgeI.CurEdge) { }
    TEdgeI& operator = (const TEdgeI& EdgeI) { if (this!=&EdgeI) { CurNode=EdgeI.CurNode; EndNode=EdgeI.EndNode; CurEdge=EdgeI.CurEdge; }  return *this; }
    /// Increment iterator.
    TEdgeI& operator++ (int) { CurEdge++; if (CurEdge >= CurNode.GetOutDeg()) { CurEdge=0; CurNode++;
      while (CurNode < EndNode && CurNode.GetOutDeg()==0) { CurNode++; } }  return *this; }
    bool operator < (const TEdgeI& EdgeI) const { return CurNode<EdgeI.CurNode || (CurNode==EdgeI.CurNode && CurEdge<EdgeI.CurEdge); }
    bool operator == (const TEdgeI& EdgeI) const { return CurNode == EdgeI.CurNode && CurEdge == EdgeI.CurEdge; }
    /// Returns edge ID. Always returns -1 since only edges in multigraphs have explicit IDs.
    int GetId() const { return -1; }
    /// Returns the source node of the edge.
    int GetSrcNId() const { return CurNode.GetId(); }
    /// Returns the destination node of the edge.
    int GetDstNId() const { return CurNode.GetOutNId(CurEdge); }
    friend class TDirNet;
  };
private:
  TCRef CRef;
  TInt MxNId;
  THash<TInt, TNode> NodeH;
  TAttr SAttrN;
  TAttrPair SAttrE;
private:
  TNode& GetNode(const int& NId) { return NodeH.GetDat(NId); }
  const TNode& GetNode(const int& NId) const { return NodeH.GetDat(NId); }
private:
  class TNodeFunctor {
  public:
    TNodeFunctor() {}
    void operator() (TNode* n, TShMIn& ShMIn) { n->LoadShM(ShMIn);}
  };
private:
  void LoadNetworkShM(TShMIn& ShMIn) {
    MxNId = TInt(ShMIn);
    TNodeFunctor f;
    NodeH.LoadShM(ShMIn, f);
    SAttrN.Load(ShMIn);
    SAttrE = TAttrPair(ShMIn);
  }
public:
  TDirNet() : CRef(), MxNId(0), NodeH(), SAttrN(), SAttrE() { }
  /// Constructor that reserves enough memory for a network of Nodes nodes and Edges edges.
  explicit TDirNet(const int& Nodes, const int& Edges) : MxNId(0), SAttrN(), SAttrE() { Reserve(Nodes, Edges); }
  TDirNet(const TDirNet& Graph) : MxNId(Graph.MxNId), NodeH(Graph.NodeH), SAttrN(), SAttrE() { }
  /// Constructor that loads the network from a (binary) stream SIn.
  TDirNet(TSIn& SIn) : MxNId(SIn), NodeH(SIn), SAttrN(SIn), SAttrE(SIn) { }
  /// Saves the network to a (binary) stream SOut. Expects data structures for sparse attributes.
  void Save(TSOut& SOut) const { MxNId.Save(SOut); NodeH.Save(SOut); SAttrN.Save(SOut); SAttrE.Save(SOut); }
  /// Saves the network to a (binary) stream SOut. Available for backwards compatibility.
  void Save_V1(TSOut& SOut) const { MxNId.Save(SOut); NodeH.Save(SOut); }
  /// Static constructor that returns a pointer to the network. Call: PDirNet Graph = TDirNet::New().
  static PDirNet New() { return new TDirNet(); }
  /// Static constructor that returns a pointer to the network and reserves enough memory for Nodes nodes and Edges edges. ##TDirNet::New
  static PDirNet New(const int& Nodes, const int& Edges) { return new TDirNet(Nodes, Edges); }
  /// Static constructor that loads the network from a stream SIn and returns a pointer to it.
  static PDirNet Load(TSIn& SIn) { return PDirNet(new TDirNet(SIn)); }
  /// Static constructor that loads the network from a stream SIn and returns a pointer to it. Backwards compatible.
  static PDirNet Load_V1(TSIn& SIn) { PDirNet Graph = PDirNet(new TDirNet());
    Graph->MxNId.Load(SIn); Graph->NodeH.Load(SIn); return Graph;
  }
  /// Static constructor that loads the network from memory. ##TDirNet::LoadShM(TShMIn& ShMIn)
  static PDirNet LoadShM(TShMIn& ShMIn) {
    TDirNet* Network = new TDirNet();
    Network->LoadNetworkShM(ShMIn);
    return PDirNet(Network);
  }
  /// Allows for run-time checking the type of the network (see the TGraphFlag for flags).
  bool HasFlag(const TGraphFlag& Flag) const;
  TDirNet& operator = (const TDirNet& Graph) {
    if (this!=&Graph) { MxNId=Graph.MxNId; NodeH=Graph.NodeH; }  return *this; }
  
  /// Returns the number of nodes in the network.
  int GetNodes() const { return NodeH.Len(); }
  /// Adds a node of ID NId to the network. ##TDirNet::AddNode
  int AddNode(int NId = -1);
  /// Adds a node of ID NId to the network, noop if the node already exists. ##TDirNet::AddNodeUnchecked
  int AddNodeUnchecked(int NId = -1);
  /// Adds a node of ID NodeI.GetId() to the network.
  int AddNode(const TNodeI& NodeId) { return AddNode(NodeId.GetId()); }
  /// Adds a node of ID NId to the network, creates edges to the node from all nodes in vector InNIdV, creates edges from the node to all nodes in vector OutNIdV. ##TDirNet::AddNode-1
  int AddNode(const int& NId, const TIntV& InNIdV, const TIntV& OutNIdV);
  /// Adds a node of ID NId to the network, creates edges to the node from all nodes in vector InNIdV in the vector pool Pool, creates edges from the node to all nodes in vector OutNIdVin the vector pool Pool . ##TDirNet::AddNode-2
  int AddNode(const int& NId, const TVecPool<TInt>& Pool, const int& SrcVId, const int& DstVId);
  /// Deletes node of ID NId from the network. ##TDirNet::DelNode
  void DelNode(const int& NId);
  /// Deletes node of ID NodeI.GetId() from the network.
  void DelNode(const TNode& NodeI) { DelNode(NodeI.GetId()); }
  /// Tests whether ID NId is a node.
  bool IsNode(const int& NId) const { return NodeH.IsKey(NId); }
  /// Returns an iterator referring to the first node in the network.
  TNodeI BegNI() const { return TNodeI(NodeH.BegI()); }
  /// Returns an iterator referring to the past-the-end node in the network.
  TNodeI EndNI() const { return TNodeI(NodeH.EndI()); }
  /// Returns an iterator referring to the node of ID NId in the network.
  TNodeI GetNI(const int& NId) const { return TNodeI(NodeH.GetI(NId)); }
  // GetNodeC() has been commented out. It was a quick shortcut, do not use.
  //const TNode& GetNodeC(const int& NId) const { return NodeH.GetDat(NId); }
  /// Returns an ID that is larger than any node ID in the network.
  int GetMxNId() const { return MxNId; }

  /// Returns the number of edges in the network.
  int GetEdges() const;
  /// Adds an edge from node SrcNId to node DstNId to the network. ##TDirNet::AddEdge
  int AddEdge(const int& SrcNId, const int& DstNId);
  /// Adds an edge from node SrcNId to node DstNId to the network. ##TDirNet::AddEdgeUnchecked
  int AddEdgeUnchecked(const int& SrcNId, const int& DstNId);
  // Adds an edge from EdgeI.GetSrcNId() to EdgeI.GetDstNId() to the network.
  int AddEdge(const TEdgeI& EdgeI) { return AddEdge(EdgeI.GetSrcNId(), EdgeI.GetDstNId()); }
  /// Deletes an edge from node IDs SrcNId to DstNId from the network. ##TDirNet::DelEdge
  void DelEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true);
  /// Tests whether an edge from node IDs SrcNId to DstNId exists in the network.
  bool IsEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true) const;
  /// Returns an iterator referring to the first edge in the network.
  TEdgeI BegEI() const { TNodeI NI=BegNI(); while(NI<EndNI() && NI.GetOutDeg()==0){NI++;} return TEdgeI(NI, EndNI()); }
  /// Returns an iterator referring to the past-the-end edge in the network.
  TEdgeI EndEI() const { return TEdgeI(EndNI(), EndNI()); }
  /// Not supported/implemented!
  TEdgeI GetEI(const int& EId) const; // not supported
  /// Returns an iterator referring to edge (SrcNId, DstNId) in the network.
  TEdgeI GetEI(const int& SrcNId, const int& DstNId) const;

  /// Returns an ID of a random node in the network.
  int GetRndNId(TRnd& Rnd=TInt::Rnd) { return NodeH.GetKey(NodeH.GetRndKeyId(Rnd, 0.8)); }
  /// Returns an interator referring to a random node in the network.
  TNodeI GetRndNI(TRnd& Rnd=TInt::Rnd) { return GetNI(GetRndNId(Rnd)); }
  /// Gets a vector IDs of all nodes in the network.
  void GetNIdV(TIntV& NIdV) const;

  /// Tests whether the network is empty (has zero nodes).
  bool Empty() const { return GetNodes()==0; }
  /// Deletes all nodes and edges from the network.
  void Clr() { MxNId=0; NodeH.Clr(); SAttrN.Clr(); SAttrE.Clr(); }
  /// Reserves memory for a network of Nodes nodes and Edges edges.
  void Reserve(const int& Nodes, const int& Edges) { if (Nodes>0) { NodeH.Gen(Nodes/2); } }
  /// Reserves memory for node ID NId having InDeg in-edges.
  void ReserveNIdInDeg(const int& NId, const int& InDeg) { GetNode(NId).InNIdV.Reserve(InDeg); }
  /// Reserves memory for node ID NId having OutDeg out-edges.
  void ReserveNIdOutDeg(const int& NId, const int& OutDeg) { GetNode(NId).OutNIdV.Reserve(OutDeg); }
  /// Sorts the adjacency lists of each node.
  void SortNodeAdjV() { for (TNodeI NI = BegNI(); NI < EndNI(); NI++) { NI.SortNIdV();} }
  /// Defragments the network. ##TDirNet::Defrag
  void Defrag(const bool& OnlyNodeLinks=false);
  /// Checks the network data structure for internal consistency. ##TDirNet::IsOk
  bool IsOk(const bool& ThrowExcept=true) const;
  /// Print the network in a human readable form to an output stream OutF.
  void Dump(FILE *OutF=stdout) const;
  /// Returns a small network on 5 nodes and 6 edges. ##TDirNet::GetSmallGraph
  static PDirNet GetSmallGraph();

  /// Adds Int sparse attribute with name \c AttrName to the given node with id \c NId.
  int AddSAttrDatN(const TInt& NId, const TStr& AttrName, const TInt& Val);
  /// Adds Int sparse attribute with id \c AttrId to the given node with id \c NId.
  int AddSAttrDatN(const TInt& NId, const TInt& AttrId, const TInt& Val);

  /// Adds Int sparse attribute with name \c AttrName to \c NodeI.
  int AddSAttrDatN(const TNodeI& NodeI, const TStr& AttrName, const TInt& Val) {
    return AddSAttrDatN(NodeI.GetId(), AttrName, Val);
  }
  /// Adds Int sparse attribute with id \c AttrId to \c NodeI.
  int AddSAttrDatN(const TNodeI& NodeI, const TInt& AttrId, const TInt& Val) {
    return AddSAttrDatN(NodeI.GetId(), AttrId, Val);
  }

  /// Adds Flt sparse attribute with name \c AttrName to the given node with id \c NId.
  int AddSAttrDatN(const TInt& NId, const TStr& AttrName, const TFlt& Val);
  /// Adds Flt sparse attribute with id \c AttrId to the given node with id \c NId.
  int AddSAttrDatN(const TInt& NId, const TInt& AttrId, const TFlt& Val);

  /// Adds Flt sparse attribute with name \c AttrName to \c NodeI.
  int AddSAttrDatN(const TNodeI& NodeI, const TStr& AttrName, const TFlt& Val) {
    return AddSAttrDatN(NodeI.GetId(), AttrName, Val);
  }
  /// Adds Flt sparse attribute with id \c AttrId to \c NodeI.
  int AddSAttrDatN(const TNodeI& NodeI, const TInt& AttrId, const TFlt& Val) {
    return AddSAttrDatN(NodeI.GetId(), AttrId, Val);
  }

  /// Adds Str sparse attribute with name \c AttrName to the given node with id \c NId.
  int AddSAttrDatN(const TInt& NId, const TStr& AttrName, const TStr& Val);
  /// Adds Str sparse attribute with id \c AttrId to the given node with id \c NId. 
  int AddSAttrDatN(const TInt& NId, const TInt& AttrId, const TStr& Val);

  /// Adds Str sparse attribute with name \c AttrName to \c NodeI.
  int AddSAttrDatN(const TNodeI& NodeI, const TStr& AttrName, const TStr& Val) {
    return AddSAttrDatN(NodeI.GetId(), AttrName, Val);
  }
  /// Adds Str sparse attribute with id \c AttrId to \c NodeI.
  int AddSAttrDatN(const TNodeI& NodeI, const TInt& AttrId, const TStr& Val) {
    return AddSAttrDatN(NodeI.GetId(), AttrId, Val);
  }

  /// Gets Int sparse attribute with name \c AttrName from node with id \c NId.
  int GetSAttrDatN(const TInt& NId, const TStr& AttrName, TInt& ValX) const;
  /// Gets Int sparse attribute with id \c AttrId from node with id \c NId. 
  int GetSAttrDatN(const TInt& NId, const TInt& AttrId, TInt& ValX) const;

  /// Gets Int sparse attribute with name \c AttrName from \c NodeI.
  int GetSAttrDatN(const TNodeI& NodeI, const TStr& AttrName, TInt& ValX) const {
    return GetSAttrDatN(NodeI.GetId(), AttrName, ValX);
  }
  /// Gets Int sparse attribute with id \c AttrId from \c NodeI.
  int GetSAttrDatN(const TNodeI& NodeI, const TInt& AttrId, TInt& ValX) const {
    return GetSAttrDatN(NodeI.GetId(), AttrId, ValX);
  }

  /// Gets Flt sparse attribute with name \c AttrName from node with id \c NId.
  int GetSAttrDatN(const TInt& NId, const TStr& AttrName, TFlt& ValX) const;
  /// Gets Flt sparse attribute with id \c AttrId from node with id \c NId.
  int GetSAttrDatN(const TInt& NId, const TInt& AttrId, TFlt& ValX) const;

  /// Gets Flt sparse attribute with name \c AttrName from \c NodeI.
  int GetSAttrDatN(const TNodeI& NodeI, const TStr& AttrName, TFlt& ValX) const {
    return GetSAttrDatN(NodeI.GetId(), AttrName, ValX);
  } 
  /// Gets Flt sparse attribute with id \c AttrId from \c NodeI.
  int GetSAttrDatN(const TNodeI& NodeI, const TInt& AttrId, TFlt& ValX) const {
    return GetSAttrDatN(NodeI.GetId(), AttrId, ValX);
  }

  /// Gets Str sparse attribute with name \c AttrName from node with id \c NId.
  int GetSAttrDatN(const TInt& NId, const TStr& AttrName, TStr& ValX) const;
  /// Gets Str sparse attribute with id \c AttrId from node with id \c NId.
  int GetSAttrDatN(const TInt& NId, const TInt& AttrId, TStr& ValX) const;

  /// Gets Str sparse attribute with name \c AttrName from \c NodeI.
  int GetSAttrDatN(const TNodeI& NodeI, const TStr& AttrName, TStr& ValX) const {
    return GetSAttrDatN(NodeI.GetId(), AttrName, ValX);
  }
  /// Gets Str sparse attribute with id \c AttrId from \c NodeI.
  int GetSAttrDatN(const TNodeI& NodeI, const TInt& AttrId, TStr& ValX) const {
    return GetSAttrDatN(NodeI.GetId(), AttrId, ValX);
  }

  /// Deletes sparse attribute with name \c AttrName from node with id \c NId.
  int DelSAttrDatN(const TInt& NId, const TStr& AttrName);
  /// Deletes sparse attribute with id \c AttrId from node with id \c NId.
  int DelSAttrDatN(const TInt& NId, const TInt& AttrId);

  /// Deletes sparse attribute with name \c AttrName from \c NodeI.
  int DelSAttrDatN(const TNodeI& NodeI, const TStr& AttrName) {
    return DelSAttrDatN(NodeI.GetId(), AttrName);
  }
  /// Deletes sparse attribute with id \c AttrId from \c NodeI.
  int DelSAttrDatN(const TNodeI& NodeI, const TInt& AttrId) {
    return DelSAttrDatN(NodeI.GetId(), AttrId);
  }

  /// Gets a list of all sparse attributes of type \c AttrType for node with id \c NId.
  int GetSAttrVN(const TInt& NId, const TAttrType AttrType, TAttrPrV& AttrV) const;
  /// Gets a list of all sparse attributes of type \c AttrType for \c NodeI.
  int GetSAttrVN(const TNodeI& NodeI, const TAttrType AttrType, TAttrPrV& AttrV) const {
    return GetSAttrVN(NodeI.GetId(), AttrType, AttrV);
  }

  /// Gets a list of all nodes that have a sparse attribute with name \c AttrName.
  int GetIdVSAttrN(const TStr& AttrName, TIntV& IdV) const;
  /// Gets a list of all nodes that have a sparse attribute with id \c AttrId.
  int GetIdVSAttrN(const TInt& AttrId, TIntV& IdV) const;

  /// Adds mapping for sparse attribute with name \c Name and type \c AttrType.
  int AddSAttrN(const TStr& Name, const TAttrType& AttrType, TInt& AttrId);

  /// Gets id and type for attribute with name \c Name.
  int GetSAttrIdN(const TStr& Name, TInt& AttrIdX, TAttrType& AttrTypeX) const;
  /// Gets name and type for attribute with id \c AttrId.
  int GetSAttrNameN(const TInt& AttrId, TStr& NameX, TAttrType& AttrTypeX) const;

  /// Adds Int sparse attribute with name \c AttrName to the given edge with ids \c SrcId and \c DstId.
  int AddSAttrDatE(const int& SrcNId, const int& DstNId, const TStr& AttrName, const TInt& Val);
  /// Adds Int sparse attribute with id \c AttrId to the given edge with ids \c SrcId and \c DstId.
  int AddSAttrDatE(const int& SrcNId, const int& DstNId, const TInt& AttrId, const TInt& Val);

  /// Adds Int sparse attribute with name \c AttrName to \c EdgeI.
  int AddSAttrDatE(const TEdgeI& EdgeI, const TStr& AttrName, const TInt& Val) {
    return AddSAttrDatE(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), AttrName, Val);
  }
  /// Adds Int sparse attribute with id \c AttrId to \c EdgeI.
  int AddSAttrDatE(const TEdgeI& EdgeI, const TInt& AttrId, const TInt& Val) {
    return AddSAttrDatE(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), AttrId, Val);
  }

  /// Adds Flt sparse attribute with name \c AttrName to the given edge with ids \c SrcId and \c DstId.
  int AddSAttrDatE(const int& SrcNId, const int& DstNId, const TStr& AttrName, const TFlt& Val);
  /// Adds Flt sparse attribute with id \c AttrId to the given edge with ids \c SrcId and \c DstId.
  int AddSAttrDatE(const int& SrcNId, const int& DstNId, const TInt& AttrId, const TFlt& Val);

  /// Adds Flt sparse attribute with name \c AttrName to \c EdgeI.
  int AddSAttrDatE(const TEdgeI& EdgeI, const TStr& AttrName, const TFlt& Val) {
    return AddSAttrDatE(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), AttrName, Val);
  }
  /// Adds Flt sparse attribute with id \c AttrId to \c EdgeI.
  int AddSAttrDatE(const TEdgeI& EdgeI, const TInt& AttrId, const TFlt& Val){
    return AddSAttrDatE(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), AttrId, Val);
  }

  /// Adds Str sparse attribute with name \c AttrName to the given edge with ids \c SrcId and \c DstId.
  int AddSAttrDatE(const int& SrcNId, const int& DstNId, const TStr& AttrName, const TStr& Val);
  /// Adds Str sparse attribute with id \c AttrId to the given edge with ids \c SrcId and \c DstId.
  int AddSAttrDatE(const int& SrcNId, const int& DstNId, const TInt& AttrId, const TStr& Val);

  /// Adds Str sparse attribute with name \c AttrName to \c EdgeI.
  int AddSAttrDatE(const TEdgeI& EdgeI, const TStr& AttrName, const TStr& Val) {
    return AddSAttrDatE(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), AttrName, Val);
  }
  /// Adds Str sparse attribute with id \c AttrId to \c EdgeI.
  int AddSAttrDatE(const TEdgeI& EdgeI, const TInt& AttrId, const TStr& Val) {
    return AddSAttrDatE(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), AttrId, Val);
  }

  /// Gets Int sparse attribute with name \c AttrName from edge with ids \c SrcId and \c DstId.
  int GetSAttrDatE(const int& SrcNId, const int& DstNId, const TStr& AttrName, TInt& ValX) const;
  /// Gets Int sparse attribute with id \c AttrId from edge with ids \c SrcId and \c DstId.
  int GetSAttrDatE(const int& SrcNId, const int& DstNId, const TInt& AttrId, TInt& ValX) const;

  /// Gets Int sparse attribute with name \c AttrName from \c EdgeI.
  int GetSAttrDatE(const TEdgeI& EdgeI, const TStr& AttrName, TInt& ValX) const {
    return GetSAttrDatE(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), AttrName, ValX);
  }
  /// Gets Int sparse attribute with id \c AttrId from \c EdgeI.
  int GetSAttrDatE(const TEdgeI& EdgeI, const TInt& AttrId, TInt& ValX) const {
    return GetSAttrDatE(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), AttrId, ValX);
  } 

  /// Gets Flt sparse attribute with name \c AttrName from edge with ids \c SrcId and \c DstId.
  int GetSAttrDatE(const int& SrcNId, const int& DstNId, const TStr& AttrName, TFlt& ValX) const; 
  /// Gets Flt sparse attribute with id \c AttrId from edge with ids \c SrcId and \c DstId.
  int GetSAttrDatE(const int& SrcNId, const int& DstNId, const TInt& AttrId, TFlt& ValX) const;

  /// Gets Flt sparse attribute with name \c AttrName from \c EdgeI.
  int GetSAttrDatE(const TEdgeI& EdgeI, const TStr& AttrName, TFlt& ValX) const {
    return GetSAttrDatE(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), AttrName, ValX);
  }
  /// Gets Flt sparse attribute with id \c AttrId from \c EdgeI.
  int GetSAttrDatE(const TEdgeI& EdgeI, const TInt& AttrId, TFlt& ValX) const {
    return GetSAttrDatE(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), AttrId, ValX);
  } 

  /// Gets Str sparse attribute with name \c AttrName from edge with ids \c SrcId and \c DstId.
  int GetSAttrDatE(const int& SrcNId, const int& DstNId, const TStr& AttrName, TStr& ValX) const;
  /// Gets Str sparse attribute with id \c AttrId from edge with ids \c SrcId and \c DstId.
  int GetSAttrDatE(const int& SrcNId, const int& DstNId, const TInt& AttrId, TStr& ValX) const;

  /// Gets Str sparse attribute with name \c AttrName from \c EdgeI.
  int GetSAttrDatE(const TEdgeI& EdgeI, const TStr& AttrName, TStr& ValX) const {
    return GetSAttrDatE(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), AttrName, ValX);
  }
  /// Gets Str sparse attribute with id \c AttrId from \c EdgeI.
  int GetSAttrDatE(const TEdgeI& EdgeI, const TInt& AttrId, TStr& ValX) const {
    return GetSAttrDatE(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), AttrId, ValX);
  }

  /// Deletes sparse attribute with name \c AttrName from edge with ids \c SrcId and \c DstId.
  int DelSAttrDatE(const int& SrcNId, const int& DstNId, const TStr& AttrName);
  /// Deletes sparse attribute with id \c AttrId from edge with ids \c SrcId and \c DstId.
  int DelSAttrDatE(const int& SrcNId, const int& DstNId, const TInt& AttrId);

  /// Deletes sparse attribute with name \c AttrName from \c EdgeI.
  int DelSAttrDatE(const TEdgeI& EdgeI, const TStr& AttrName) {
    return DelSAttrDatE(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), AttrName);
  }
  /// Deletes sparse attribute with id \c AttrId from \c EdgeI.
  int DelSAttrDatE(const TEdgeI& EdgeI, const TInt& AttrId) {
    return DelSAttrDatE(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), AttrId);
  } 
  /// Gets a list of all sparse attributes of type \c AttrType for edge with ids \c SrcId and \c DstId.
  int GetSAttrVE(const int& SrcNId, const int& DstNId, const TAttrType AttrType, TAttrPrV& AttrV) const;
  /// Gets a list of all sparse attributes of type \c AttrType for \c EdgeI.
  int GetSAttrVE(const TEdgeI& EdgeI, const TAttrType AttrType, TAttrPrV& AttrV) const {
    return GetSAttrVE(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), AttrType, AttrV);
  }

  /// Gets a list of all edges that have a sparse attribute with name \c AttrName.
  int GetIdVSAttrE(const TStr& AttrName, TIntPrV& IdV) const;
  /// Gets a list of all edges that have a sparse attribute with id \c AttrId.
  int GetIdVSAttrE(const TInt& AttrId, TIntPrV& IdV) const;

  /// Adds mapping for sparse attribute with name \c Name and type \c AttrType.
  int AddSAttrE(const TStr& Name, const TAttrType& AttrType, TInt& AttrId);

  /// Gets id and type for attribute with name \c Name.
  int GetSAttrIdE(const TStr& Name, TInt& AttrIdX, TAttrType& AttrTypeX) const;
  /// Gets name and type for attribute with id \c AttrId.
  int GetSAttrNameE(const TInt& AttrId, TStr& NameX, TAttrType& AttrTypeX) const;

  friend class TPt<TDirNet>;
  friend class TDirNetMtx;
};

// set flags
namespace TSnap {
template <> struct IsDirected<TDirNet> { enum { Val = 1 }; };
}
#endif // NETWORK_H
