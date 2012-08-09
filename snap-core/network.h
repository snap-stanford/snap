/////////////////////////////////////////////////
// Node Data
// TNodeData has to implement the following methods:
//  class TNodeData {
//  public:
//    TNodeData() { }
//    TNodeData(TSIn& SIn) { }
//    void Save(TSOut& SOut) const { }
//  };

/////////////////////////////////////////////////
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
  /// Allows for run-time checking the type of the network (see the TGraphFlag for flags).
  bool HasFlag(const TGraphFlag& Flag) const;
  TNodeNet& operator = (const TNodeNet& NodeNet) {
    if (this!=&NodeNet) { NodeH=NodeNet.NodeH;  MxNId=NodeNet.MxNId; }  return *this; }
  // nodes
  /// Returns the number of nodes in the network.
  int GetNodes() const { return NodeH.Len(); }
  /// Adds a node of ID NId to the network. ##TNodeNet::AddNode
  int AddNode(int NId = -1);
  /// Adds a node of ID NId and node data NodeDat to the network. ##TNodeNet::AddNode-1
  int AddNode(int NId, const TNodeData& NodeDat);
  /// Adds a node NodeId and its node data to the network.
  int AddNode(const TNodeI& NodeId) { return AddNode(NodeId.GetId(), NodeId.GetDat()); }
  /// Deletes node of ID NId from the network. ##TNodeNet::DelNode
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
  /// Returns the maximum id of a any node in the network.
  int GetMxNId() const { return MxNId; }

  // edges
  /// Returns the number of edges in the network.
  int GetEdges() const;
  /// Adds an edge from node IDs SrcNId to node DstNId to the network. ##TNodeNet::AddEdge
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
  return -1; // edge id
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

/////////////////////////////////////////////////
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
  /// Allows for run-time checking the type of the network (see the TGraphFlag for flags).
  bool HasFlag(const TGraphFlag& Flag) const;
  TNodeEDatNet& operator = (const TNodeEDatNet& NodeNet) { if (this!=&NodeNet) {
    NodeH=NodeNet.NodeH;  MxNId=NodeNet.MxNId; }  return *this; }
  // nodes
  /// Returns the number of nodes in the network.
  int GetNodes() const { return NodeH.Len(); }
  /// Adds a node of ID NId to the network. ##TNodeEDatNet::AddNode
  int AddNode(int NId = -1);
  /// Adds a node of ID NId and node data NodeDat to the network. ##TNodeEDatNet::AddNode-1
  int AddNode(int NId, const TNodeData& NodeDat);
  /// Adds a node NodeId and its node data to the network.
  int AddNode(const TNodeI& NodeId) { return AddNode(NodeId.GetId(), NodeId.GetDat()); }
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
  /// Returns the maximum id of a any node in the network.
  int GetMxNId() const { return MxNId; }

  // edges
  /// Returns the number of edges in the network.
  int GetEdges() const;
  /// Adds an edge from node IDs SrcNId to node DstNId to the network. ##TNodeEDatNet::AddEdge
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
  return -1; // edge id
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

/////////////////////////////////////////////////
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
    TNodeData& GetInNDat(const int& EdgeN) { return Net->GetNodeDat(GetInNId(EdgeN)); }
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
  /// Static constructor that returns a pointer to the network. Call: TPt <TNodeEdgeNet<TNodeData, TEdgeData> > Net = TNodeEdgeNet<TNodeData, TEdgeData>::New().
  static PNet New() { return PNet(new TNet()); }
  /// Static constructor that loads the network from a stream SIn and returns a pointer to it.
  static PNet Load(TSIn& SIn) { return PNet(new TNet(SIn)); }
  /// Allows for run-time checking the type of the network (see the TGraphFlag for flags).
  bool HasFlag(const TGraphFlag& Flag) const;
  TNodeEdgeNet& operator = (const TNodeEdgeNet& Net) {
    if (this!=&Net) { NodeH=Net.NodeH; EdgeH=Net.EdgeH; MxNId=Net.MxNId; MxEId=Net.MxEId; }  return *this; }
  // nodes
  /// Returns the number of nodes in the network.
  int GetNodes() const { return NodeH.Len(); }
  /// Adds a node of ID NId to the network. ##TNodeEdgeNet::AddNode
  int AddNode(int NId = -1);
  /// Adds node data to node with ID NId. ##TNodeEdgeNet::AddNode-1
  int AddNode(int NId, const TNodeData& NodeDat);
  /// Adds a node NodeId and its node data to the network.
  int AddNode(const TNodeI& NodeId) { return AddNode(NodeId.GetId(), NodeId.GetDat()); }
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
  /// Returns the maximum id of a any node in the network.
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
  // TODO document TNodeEdgeNet::GetEI()
  TEdgeI GetEI(const int& EId) const { return TEdgeI(EdgeH.GetI(EId), this); }
  // TODO document TNodeEdgeNet::GetEI()
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
