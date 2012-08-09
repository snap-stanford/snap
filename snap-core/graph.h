/////////////////////////////////////////////////
// Undirected graphs
class TUNGraph;
class TBPGraph;
//TODO:class TUNEGraph; -- undirected multigraph

/// Pointer to an undirected graph (TUNGraph)
typedef TPt<TUNGraph> PUNGraph;
/// Pointer to a bipartitegraph graph (TBPGraph)
typedef TPt<TBPGraph> PBPGraph;

/////////////////////////////////////////////////
// Directed graphs
class TNGraph;
class TNEGraph;

/// Pointer to a directed graph (TNGraph)
typedef TPt<TNGraph> PNGraph;
/// Pointer to a directed multigraph (TNEGraph)
typedef TPt<TNEGraph> PNEGraph;

/////////////////////////////////////////////////
/// Undirected graph. ##Undirected_graph
class TUNGraph {
public:
  typedef TUNGraph TNet;
  typedef TPt<TUNGraph> PNet;
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
    void Save(TSOut& SOut) const { Id.Save(SOut);  NIdV.Save(SOut); }
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
    friend class TUNGraph;
    friend class TUNGraphMtx;
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
    TNodeI& operator = (const TNodeI& NodeI) { NodeHI = NodeI.NodeHI;  return *this; }

    /// Increment iterator.
    TNodeI& operator++ (int) { NodeHI++;  return *this; }

    bool operator < (const TNodeI& NodeI) const { return NodeHI < NodeI.NodeHI; }
    bool operator == (const TNodeI& NodeI) const { return NodeHI == NodeI.NodeHI; }

    /// Returns ID of the current node.
    int GetId() const { return NodeHI.GetDat().GetId(); }
    /// Returns degree of the current node.
    int GetDeg() const { return NodeHI.GetDat().GetDeg(); }
    /// Returns in-degree of the current node (returns same as value GetDeg() since the graph is undirected).
    int GetInDeg() const { return NodeHI.GetDat().GetInDeg(); }
    /// Returns out-degree of the current node (returns same as value GetDeg() since the graph is undirected).
    int GetOutDeg() const { return NodeHI.GetDat().GetOutDeg(); }
    /// Returns ID of NodeN-th in-node (the node pointing to the current node). ##TUNGraph::TNodeI::GetInNId
    int GetInNId(const int& NodeN) const { return NodeHI.GetDat().GetInNId(NodeN); }
    /// Returns ID of NodeN-th out-node (the node the current node points to). ##TUNGraph::TNodeI::GetOutNId
    int GetOutNId(const int& NodeN) const { return NodeHI.GetDat().GetOutNId(NodeN); }
    /// Returns ID of NodeN-th neighboring node. ##TUNGraph::TNodeI::GetNbrNId
    int GetNbrNId(const int& NodeN) const { return NodeHI.GetDat().GetNbrNId(NodeN); }
    /// Tests whether node with ID NId points to the current node.
    bool IsInNId(const int& NId) const { return NodeHI.GetDat().IsInNId(NId); }
    /// Tests whether the current node points to node with ID NId.
    bool IsOutNId(const int& NId) const { return NodeHI.GetDat().IsOutNId(NId); }
    /// Tests whether node with ID NId is a neighbor of the current node.
    bool IsNbrNId(const int& NId) const { return NodeHI.GetDat().IsNbrNId(NId); }
    friend class TUNGraph;
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
    TEdgeI& operator++ (int) { do { CurEdge++; if (CurEdge >= CurNode.GetOutDeg()) { CurEdge=0;  CurNode++; while (CurNode < EndNode && CurNode.GetOutDeg()==0) { CurNode++; } } } while (CurNode < EndNode && GetSrcNId()>GetDstNId()); return *this; }
    bool operator < (const TEdgeI& EdgeI) const { return CurNode<EdgeI.CurNode || (CurNode==EdgeI.CurNode && CurEdge<EdgeI.CurEdge); }
    bool operator == (const TEdgeI& EdgeI) const { return CurNode == EdgeI.CurNode && CurEdge == EdgeI.CurEdge; }
    /// Gets edge ID. Always returns -1 since only edges in multigraphs have explicit IDs.
    int GetId() const { return -1; }
    /// Gets the source of an edge. Since the graph is undirected this is the node with smaller ID of the edge endpoints.
    int GetSrcNId() const { return CurNode.GetId(); }
    /// Gets destination of an edge. Since the graph is undirected this is the node with greater ID of the edge endpoints.
    int GetDstNId() const { return CurNode.GetOutNId(CurEdge); }
    friend class TUNGraph;
  };
private:
  TCRef CRef;
  TInt MxNId;
  THash<TInt, TNode> NodeH;
private:
  TNode& GetNode(const int& NId) { return NodeH.GetDat(NId); }
  const TNode& GetNode(const int& NId) const { return NodeH.GetDat(NId); }
public:
  TUNGraph() : CRef(), MxNId(0), NodeH() { }
  /// Constructor that reserves enough memory for a graph of Nodes nodes and Edges edges.
  explicit TUNGraph(const int& Nodes, const int& Edges) : MxNId(0) { Reserve(Nodes, Edges); }
  TUNGraph(const TUNGraph& Graph) : MxNId(Graph.MxNId), NodeH(Graph.NodeH) { }
  /// Constructor that loads the graph from a (binary) stream SIn.
  TUNGraph(TSIn& SIn) : MxNId(SIn), NodeH(SIn) { }
  /// Saves the graph to a (binary) stream SOut.
  void Save(TSOut& SOut) const { MxNId.Save(SOut);  NodeH.Save(SOut); }
  /// Static constructor that returns a pointer to the graph. Call: PUNGraph Graph = TUNGraph::New().
  static PUNGraph New() { return new TUNGraph(); }
  /// Static constructor that returns a pointer to the graph and reserves enough memory for Nodes nodes and Edges edges. ##TUNGraph::New
  static PUNGraph New(const int& Nodes, const int& Edges) { return new TUNGraph(Nodes, Edges); }
  /// Static constructor that loads the graph from a stream SIn and returns a pointer to it.
  static PUNGraph Load(TSIn& SIn) { return PUNGraph(new TUNGraph(SIn)); }
  /// Allows for run-time checking the type of the graph (see the TGraphFlag for flags).
  bool HasFlag(const TGraphFlag& Flag) const;
  TUNGraph& operator = (const TUNGraph& Graph) {
    if (this!=&Graph) { MxNId=Graph.MxNId;  NodeH=Graph.NodeH; }  return *this; }
  
  /// Returns the number of nodes in the graph.
  int GetNodes() const { return NodeH.Len(); }
  /// Adds a node of ID NId to the graph. ##TUNGraph::AddNode
  int AddNode(int NId = -1);
  /// Adds a node of ID NodeI.GetId() to the graph.
  int AddNode(const TNodeI& NodeI) { return AddNode(NodeI.GetId()); }
  /// Adds a node of ID NId to the graph and create edges to all nodes in vector NbrNIdV. ##TUNGraph::AddNode-1
  int AddNode(const int& NId, const TIntV& NbrNIdV);
  /// Adds a node of ID NId to the graph and create edges to all nodes in vector NIdVId in the vector pool Pool. ##TUNGraph::AddNode-2
  int AddNode(const int& NId, const TVecPool<TInt>& Pool, const int& NIdVId);
  /// Deletes node of ID NId from the graph. ##TUNGraph::DelNode
  void DelNode(const int& NId);
  /// Deletes node of ID NodeI.GetId() from the graph.
  void DelNode(const TNode& NodeI) { DelNode(NodeI.GetId()); }
  /// Tests whether ID NId is a node.
  bool IsNode(const int& NId) const { return NodeH.IsKey(NId); }
  /// Returns an iterator referring to the first node in the graph.
  TNodeI BegNI() const { return TNodeI(NodeH.BegI()); }
  /// Returns an iterator referring to the past-the-end node in the graph.
  TNodeI EndNI() const { return TNodeI(NodeH.EndI()); }
  /// Returns an iterator referring to the node of ID NId in the graph.
  TNodeI GetNI(const int& NId) const { return TNodeI(NodeH.GetI(NId)); }
  /// Returns the maximum id of a any node in the graph.
  int GetMxNId() const { return MxNId; }

  /// Returns the number of edges in the graph.
  int GetEdges() const;
  /// Adds an edge between node IDs SrcNId and DstNId to the graph. ##TUNGraph::AddEdge
  int AddEdge(const int& SrcNId, const int& DstNId);
  /// Adds an edge between EdgeI.GetSrcNId() and EdgeI.GetDstNId() to the graph.
  int AddEdge(const TEdgeI& EdgeI) { return AddEdge(EdgeI.GetSrcNId(), EdgeI.GetDstNId()); }
  /// Deletes an edge between node IDs SrcNId and DstNId from the graph. ##TUNGraph::DelEdge
  void DelEdge(const int& SrcNId, const int& DstNId);
  /// Tests whether an edge between node IDs SrcNId and DstNId exists in the graph.
  bool IsEdge(const int& SrcNId, const int& DstNId) const;
  /// Returns an iterator referring to the first edge in the graph.
  TEdgeI BegEI() const { TNodeI NI=BegNI(); while (NI<EndNI() && (NI.GetOutDeg()==0 || NI.GetId()>NI.GetOutNId(0))) { NI++; } return TEdgeI(NI, EndNI()); }
  /// Returns an iterator referring to the past-the-end edge in the graph.
  TEdgeI EndEI() const { return TEdgeI(EndNI(), EndNI()); }
  /// Not supported/implemented!
  TEdgeI GetEI(const int& EId) const;
  /// Returns an iterator referring to edge (SrcNId, DstNId) in the graph. ##TUNGraph::GetEI
  TEdgeI GetEI(const int& SrcNId, const int& DstNId) const;

  /// Returns an ID of a random node in the graph.
  int GetRndNId(TRnd& Rnd=TInt::Rnd) { return NodeH.GetKey(NodeH.GetRndKeyId(Rnd, 0.8)); }
  /// Returns an interator referring to a random node in the graph.
  TNodeI GetRndNI(TRnd& Rnd=TInt::Rnd) { return GetNI(GetRndNId(Rnd)); }
  /// Gets a vector IDs of all nodes in the graph.
  void GetNIdV(TIntV& NIdV) const;

  /// Tests whether the graph is empty (has zero nodes).
  bool Empty() const { return GetNodes()==0; }
  /// Deletes all nodes and edges from the graph.
  void Clr() { MxNId=0;  NodeH.Clr(); }
  /// Reserves memory for a graph of Nodes nodes and Edges edges.
  void Reserve(const int& Nodes, const int& Edges) { if (Nodes>0) NodeH.Gen(Nodes/2); }
  /// Reserves memory for node ID NId having Deg edges.
  void ReserveNIdDeg(const int& NId, const int& Deg) { GetNode(NId).NIdV.Reserve(Deg); }
  /// Defragments the graph. ##TUNGraph::Defrag
  void Defrag(const bool& OnlyNodeLinks=false);
  /// Checks the graph data structure for internal consistency. ##TUNGraph::IsOk
  bool IsOk(const bool& ThrowExcept=true) const;
  /// Print the graph in a human readable form to an output stream OutF.
  void Dump(FILE *OutF=stdout) const;
  /// Returns a small graph on 5 nodes and 5 edges. ##TUNGraph::GetSmallGraph
  static PUNGraph GetSmallGraph();

  friend class TUNGraphMtx;
  friend class TPt<TUNGraph>;
};

/////////////////////////////////////////////////
// Directed Node Graph

/// Directed graph. ##Directed_graph
class TNGraph {
public:
  typedef TNGraph TNet;
  typedef TPt<TNGraph> PNet;
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
    void Save(TSOut& SOut) const { Id.Save(SOut);  InNIdV.Save(SOut);  OutNIdV.Save(SOut);  }
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
    friend class TNGraph;
    friend class TNGraphMtx;
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
    TNodeI& operator = (const TNodeI& NodeI) { NodeHI = NodeI.NodeHI;  return *this; }
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
    /// Returns ID of NodeN-th in-node (the node pointing to the current node). ##TNGraph::TNodeI::GetInNId
    int GetInNId(const int& NodeN) const { return NodeHI.GetDat().GetInNId(NodeN); }
    /// Returns ID of NodeN-th out-node (the node the current node points to). ##TNGraph::TNodeI::GetOutNId
    int GetOutNId(const int& NodeN) const { return NodeHI.GetDat().GetOutNId(NodeN); }
    /// Returns ID of NodeN-th neighboring node. ##TNGraph::TNodeI::GetNbrNId
    int GetNbrNId(const int& NodeN) const { return NodeHI.GetDat().GetNbrNId(NodeN); }
    /// Tests whether node with ID NId points to the current node.
    bool IsInNId(const int& NId) const { return NodeHI.GetDat().IsInNId(NId); }
    /// Tests whether the current node points to node with ID NId.
    bool IsOutNId(const int& NId) const { return NodeHI.GetDat().IsOutNId(NId); }
    /// Tests whether node with ID NId is a neighbor of the current node.
    bool IsNbrNId(const int& NId) const { return IsOutNId(NId) || IsInNId(NId); }
    friend class TNGraph;
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
    /// Gets destination node of an edge.
    int GetDstNId() const { return CurNode.GetOutNId(CurEdge); }
    friend class TNGraph;
  };
private:
  TCRef CRef;
  TInt MxNId;
  THash<TInt, TNode> NodeH;
private:
  TNode& GetNode(const int& NId) { return NodeH.GetDat(NId); }
  const TNode& GetNode(const int& NId) const { return NodeH.GetDat(NId); }
public:
  TNGraph() : CRef(), MxNId(0), NodeH() { }
  /// Constructor that reserves enough memory for a graph of Nodes nodes and Edges edges.
  explicit TNGraph(const int& Nodes, const int& Edges) : MxNId(0) { Reserve(Nodes, Edges); }
  TNGraph(const TNGraph& Graph) : MxNId(Graph.MxNId), NodeH(Graph.NodeH) { }
  /// Constructor that loads the graph from a (binary) stream SIn.
  TNGraph(TSIn& SIn) : MxNId(SIn), NodeH(SIn) { }
  /// Saves the graph to a (binary) stream SOut.
  void Save(TSOut& SOut) const { MxNId.Save(SOut);  NodeH.Save(SOut); }
  /// Static constructor that returns a pointer to the graph. Call: PNGraph Graph = TNGraph::New().
  static PNGraph New() { return new TNGraph(); }
  /// Static constructor that returns a pointer to the graph and reserves enough memory for Nodes nodes and Edges edges. ##TNGraph::New
  static PNGraph New(const int& Nodes, const int& Edges) { return new TNGraph(Nodes, Edges); }
  /// Static constructor that loads the graph from a stream SIn and returns a pointer to it.
  static PNGraph Load(TSIn& SIn) { return PNGraph(new TNGraph(SIn)); }
  /// Allows for run-time checking the type of the graph (see the TGraphFlag for flags).
  bool HasFlag(const TGraphFlag& Flag) const;
  TNGraph& operator = (const TNGraph& Graph) {
    if (this!=&Graph) { MxNId=Graph.MxNId;  NodeH=Graph.NodeH; }  return *this; }
  
  /// Returns the number of nodes in the graph.
  int GetNodes() const { return NodeH.Len(); }
  /// Adds a node of ID NId to the graph. ##TNGraph::AddNode
  int AddNode(int NId = -1);
  /// Adds a node of ID NodeI.GetId() to the graph.
  int AddNode(const TNodeI& NodeId) { return AddNode(NodeId.GetId()); }
  /// Adds a node of ID NId to the graph, creates edges to the node from all nodes in vector InNIdV, creates edges from the node to all nodes in vector OutNIdV. ##TNGraph::AddNode-1
  int AddNode(const int& NId, const TIntV& InNIdV, const TIntV& OutNIdV);
  /// Adds a node of ID NId to the graph, creates edges to the node from all nodes in vector InNIdV in the vector pool Pool, creates edges from the node to all nodes in vector OutNIdVin the vector pool Pool . ##TNGraph::AddNode-2
  int AddNode(const int& NId, const TVecPool<TInt>& Pool, const int& SrcVId, const int& DstVId);
  /// Deletes node of ID NId from the graph. ##TNGraph::DelNode
  void DelNode(const int& NId);
  /// Deletes node of ID NodeI.GetId() from the graph.
  void DelNode(const TNode& NodeI) { DelNode(NodeI.GetId()); }
  /// Tests whether ID NId is a node.
  bool IsNode(const int& NId) const { return NodeH.IsKey(NId); }
  /// Returns an iterator referring to the first node in the graph.
  TNodeI BegNI() const { return TNodeI(NodeH.BegI()); }
  /// Returns an iterator referring to the past-the-end node in the graph.
  TNodeI EndNI() const { return TNodeI(NodeH.EndI()); }
  /// Returns an iterator referring to the node of ID NId in the graph.
  TNodeI GetNI(const int& NId) const { return TNodeI(NodeH.GetI(NId)); }
  // GetNodeC() has been commented out. It was a quick shortcut, do not use.
  //const TNode& GetNodeC(const int& NId) const { return NodeH.GetDat(NId); }
  /// Returns the maximum id of a any node in the graph.
  int GetMxNId() const { return MxNId; }

  /// Returns the number of edges in the graph.
  int GetEdges() const;
  /// Adds an edge from node IDs SrcNId to node DstNId to the graph. ##TNGraph::AddEdge
  int AddEdge(const int& SrcNId, const int& DstNId);
  /// Adds an edge from EdgeI.GetSrcNId() to EdgeI.GetDstNId() to the graph.
  int AddEdge(const TEdgeI& EdgeI) { return AddEdge(EdgeI.GetSrcNId(), EdgeI.GetDstNId()); }
  /// Deletes an edge from node IDs SrcNId to DstNId from the graph. ##TNGraph::DelEdge
  void DelEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true);
  /// Tests whether an edge from node IDs SrcNId to DstNId exists in the graph.
  bool IsEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true) const;
  /// Returns an iterator referring to the first edge in the graph.
  TEdgeI BegEI() const { TNodeI NI=BegNI();  while(NI<EndNI() && NI.GetOutDeg()==0) NI++;  return TEdgeI(NI, EndNI()); }
  /// Returns an iterator referring to the past-the-end edge in the graph.
  TEdgeI EndEI() const { return TEdgeI(EndNI(), EndNI()); }
  /// Not supported/implemented!
  TEdgeI GetEI(const int& EId) const; // not supported
  /// Returns an iterator referring to edge (SrcNId, DstNId) in the graph.
  TEdgeI GetEI(const int& SrcNId, const int& DstNId) const;

  /// Returns an ID of a random node in the graph.
  int GetRndNId(TRnd& Rnd=TInt::Rnd) { return NodeH.GetKey(NodeH.GetRndKeyId(Rnd, 0.8)); }
  /// Returns an interator referring to a random node in the graph.
  TNodeI GetRndNI(TRnd& Rnd=TInt::Rnd) { return GetNI(GetRndNId(Rnd)); }
  /// Gets a vector IDs of all nodes in the graph.
  void GetNIdV(TIntV& NIdV) const;

  /// Tests whether the graph is empty (has zero nodes).
  bool Empty() const { return GetNodes()==0; }
  /// Deletes all nodes and edges from the graph.
  void Clr() { MxNId=0;  NodeH.Clr(); }
  /// Reserves memory for a graph of Nodes nodes and Edges edges.
  void Reserve(const int& Nodes, const int& Edges) { if (Nodes>0) { NodeH.Gen(Nodes/2); } }
  /// Reserves memory for node ID NId having InDeg in-edges.
  void ReserveNIdInDeg(const int& NId, const int& InDeg) { GetNode(NId).InNIdV.Reserve(InDeg); }
  /// Reserves memory for node ID NId having OutDeg out-edges.
  void ReserveNIdOutDeg(const int& NId, const int& OutDeg) { GetNode(NId).OutNIdV.Reserve(OutDeg); }
  /// Defragments the graph. ##TNGraph::Defrag
  void Defrag(const bool& OnlyNodeLinks=false);
  /// Checks the graph data structure for internal consistency. ##TNGraph::IsOk
  bool IsOk(const bool& ThrowExcept=true) const;
  /// Print the graph in a human readable form to an output stream OutF.
  void Dump(FILE *OutF=stdout) const;
  /// Returns a small graph on 5 nodes and 6 edges. ##TNGraph::GetSmallGraph
  static PNGraph GetSmallGraph();
  friend class TPt<TNGraph>;
  friend class TNGraphMtx;
};

// set flags
namespace TSnap {
template <> struct IsDirected<TNGraph> { enum { Val = 1 }; };
}

/////////////////////////////////////////////////
// Node Edge Graph

// TODO TNEGraph describe time complexity for basic operations
/// Directed multigraph. ##Directed_multigraph
class TNEGraph {
public:
  typedef TNEGraph TNet;
  typedef TPt<TNEGraph> PNet;
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
    void Save(TSOut& SOut) const { Id.Save(SOut);  InEIdV.Save(SOut);  OutEIdV.Save(SOut);  }
    int GetId() const { return Id; }
    int GetDeg() const { return GetInDeg() + GetOutDeg(); }
    int GetInDeg() const { return InEIdV.Len(); }
    int GetOutDeg() const { return OutEIdV.Len(); }
    int GetInEId(const int& EdgeN) const { return InEIdV[EdgeN]; }
    int GetOutEId(const int& EdgeN) const { return OutEIdV[EdgeN]; }
    int GetNbrEId(const int& EdgeN) const { return EdgeN<GetOutDeg()?GetOutEId(EdgeN):GetInEId(EdgeN-GetOutDeg()); }
    bool IsInEId(const int& EId) const { return InEIdV.SearchBin(EId) != -1; }
    bool IsOutEId(const int& EId) const { return OutEIdV.SearchBin(EId) != -1; }
    friend class TNEGraph;
  };
  class TEdge {
  private:
    TInt Id, SrcNId, DstNId;
  public:
    TEdge() : Id(-1), SrcNId(-1), DstNId(-1) { }
    TEdge(const int& EId, const int& SourceNId, const int& DestNId) : Id(EId), SrcNId(SourceNId), DstNId(DestNId) { }
    TEdge(const TEdge& Edge) : Id(Edge.Id), SrcNId(Edge.SrcNId), DstNId(Edge.DstNId) { }
    TEdge(TSIn& SIn) : Id(SIn), SrcNId(SIn), DstNId(SIn) { }
    void Save(TSOut& SOut) const { Id.Save(SOut);  SrcNId.Save(SOut);  DstNId.Save(SOut); }
    int GetId() const { return Id; }
    int GetSrcNId() const { return SrcNId; }
    int GetDstNId() const { return DstNId; }
    friend class TNEGraph;
  };
  /// Node iterator. Only forward iteration (operator++) is supported.
  class TNodeI {
  private:
    typedef THash<TInt, TNode>::TIter THashIter;
    THashIter NodeHI;
    const TNEGraph *Graph;
  public:
    TNodeI() : NodeHI(), Graph(NULL) { }
    TNodeI(const THashIter& NodeHIter, const TNEGraph* GraphPt) : NodeHI(NodeHIter), Graph(GraphPt) { }
    TNodeI(const TNodeI& NodeI) : NodeHI(NodeI.NodeHI), Graph(NodeI.Graph) { }
    TNodeI& operator = (const TNodeI& NodeI) { NodeHI = NodeI.NodeHI;  Graph=NodeI.Graph;  return *this; }
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
    /// Returns ID of EdgeN-th in-node (the node pointing to the current node). ##TNEGraph::TNodeI::GetInNId
    int GetInNId(const int& EdgeN) const { return Graph->GetEdge(NodeHI.GetDat().GetInEId(EdgeN)).GetSrcNId(); }
    /// Returns ID of EdgeN-th out-node (the node the current node points to). ##TNEGraph::TNodeI::GetOutNId
    int GetOutNId(const int& EdgeN) const { return Graph->GetEdge(NodeHI.GetDat().GetOutEId(EdgeN)).GetDstNId(); }
    /// Returns ID of EdgeN-th neighboring node. ##TNEGraph::TNodeI::GetNbrNId
    int GetNbrNId(const int& EdgeN) const { const TEdge& E = Graph->GetEdge(NodeHI.GetDat().GetNbrEId(EdgeN));
      return GetId()==E.GetSrcNId() ? E.GetDstNId():E.GetSrcNId(); }
    /// Tests whether node with ID NId points to the current node.
    bool IsInNId(const int& NId) const;
    /// Tests whether the current node points to node with ID NId.
    bool IsOutNId(const int& NId) const;
    /// Tests whether node with ID NId is a neighbor of the current node.
    bool IsNbrNId(const int& NId) const { return IsOutNId(NId) || IsInNId(NId); }
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
    bool IsNbrEId(const int& EId) const { return IsInEId(EId) || IsOutEId(EId); }
    friend class TNEGraph;
  };
  /// Edge iterator. Only forward iteration (operator++) is supported.
  class TEdgeI {
  private:
    typedef THash<TInt, TEdge>::TIter THashIter;
    THashIter EdgeHI;
    const TNEGraph *Graph;
  public:
    TEdgeI() : EdgeHI(), Graph(NULL) { }
    TEdgeI(const THashIter& EdgeHIter, const TNEGraph *GraphPt) : EdgeHI(EdgeHIter), Graph(GraphPt) { }
    TEdgeI(const TEdgeI& EdgeI) : EdgeHI(EdgeI.EdgeHI), Graph(EdgeI.Graph) { }
    TEdgeI& operator = (const TEdgeI& EdgeI) { if (this!=&EdgeI) { EdgeHI=EdgeI.EdgeHI;  Graph=EdgeI.Graph; }  return *this; }
    /// Increment iterator.
    TEdgeI& operator++ (int) { EdgeHI++;  return *this; }
    bool operator < (const TEdgeI& EdgeI) const { return EdgeHI < EdgeI.EdgeHI; }
    bool operator == (const TEdgeI& EdgeI) const { return EdgeHI == EdgeI.EdgeHI; }
    /// Gets edge ID.
    int GetId() const { return EdgeHI.GetDat().GetId(); }
    /// Gets the source of an edge.
    int GetSrcNId() const { return EdgeHI.GetDat().GetSrcNId(); }
    /// Gets destination of an edge.
    int GetDstNId() const { return EdgeHI.GetDat().GetDstNId(); }
    friend class TNEGraph;
  };

private:
  TNode& GetNode(const int& NId) { return NodeH.GetDat(NId); }
  const TNode& GetNode(const int& NId) const { return NodeH.GetDat(NId); }
  TEdge& GetEdge(const int& EId) { return EdgeH.GetDat(EId); }
  const TEdge& GetEdge(const int& EId) const { return EdgeH.GetDat(EId); }
private:
  TCRef CRef;
  TInt MxNId, MxEId;
  THash<TInt, TNode> NodeH;
  THash<TInt, TEdge> EdgeH;
public:
  TNEGraph() : CRef(), MxNId(0), MxEId(0) { }
  /// Constructor that reserves enough memory for a graph of Nodes nodes and Edges edges.
  explicit TNEGraph(const int& Nodes, const int& Edges) : CRef(), MxNId(0), MxEId(0) { Reserve(Nodes, Edges); }
  TNEGraph(const TNEGraph& Graph) : MxNId(Graph.MxNId), MxEId(Graph.MxEId), NodeH(Graph.NodeH), EdgeH(Graph.EdgeH) { }
  /// Constructor for loading the graph from a (binary) stream SIn.
  TNEGraph(TSIn& SIn) : MxNId(SIn), MxEId(SIn), NodeH(SIn), EdgeH(SIn) { }
  /// Saves the graph to a (binary) stream SOut.
  void Save(TSOut& SOut) const { MxNId.Save(SOut);  MxEId.Save(SOut);  NodeH.Save(SOut);  EdgeH.Save(SOut); }
  /// Static constructor that returns a pointer to the graph. Call: PNEGraph Graph = TNEGraph::New().
  static PNEGraph New() { return PNEGraph(new TNEGraph()); }
  /// Static constructor that returns a pointer to the graph and reserves enough memory for Nodes nodes and Edges edges. ##TNEGraph::New
  static PNEGraph New(const int& Nodes, const int& Edges) { return PNEGraph(new TNEGraph(Nodes, Edges)); }
  /// Static constructor that loads the graph from a stream SIn and returns a pointer to it.
  static PNEGraph Load(TSIn& SIn) { return PNEGraph(new TNEGraph(SIn)); }
  /// Allows for run-time checking the type of the graph (see the TGraphFlag for flags).
  bool HasFlag(const TGraphFlag& Flag) const;
  TNEGraph& operator = (const TNEGraph& Graph) { if (this!=&Graph) {
    MxNId=Graph.MxNId; MxEId=Graph.MxEId; NodeH=Graph.NodeH; EdgeH=Graph.EdgeH; }  return *this; }

  /// Returns the number of nodes in the graph.
  int GetNodes() const { return NodeH.Len(); }
  /// Adds a node of ID NId to the graph. ##TNEGraph::AddNode
  int AddNode(int NId = -1);
  /// Adds a node of ID NodeI.GetId() to the graph.
  int AddNode(const TNodeI& NodeId) { return AddNode(NodeId.GetId()); }
  /// Deletes node of ID NId from the graph. ##TNEGraph::DelNode
  void DelNode(const int& NId);
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
  /// Returns the maximum id of a any node in the graph.
  int GetMxNId() const { return MxNId; }

  /// Returns the number of edges in the graph.
  int GetEdges() const { return EdgeH.Len(); }
  /// Adds an edge with ID EId between node IDs SrcNId and DstNId to the graph. ##TNEGraph::AddEdge
  int AddEdge(const int& SrcNId, const int& DstNId, int EId  = -1);
  /// Adds an edge between EdgeI.GetSrcNId() and EdgeI.GetDstNId() to the graph.
  int AddEdge(const TEdgeI& EdgeI) { return AddEdge(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), EdgeI.GetId()); }
  /// Deletes an edge with edge ID EId from the graph.
  void DelEdge(const int& EId);
  /// Deletes all edges between node IDs SrcNId and DstNId from the graph. ##TNEGraph::DelEdge
  void DelEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true);
  /// Tests whether an edge with edge ID EId exists in the graph.
  bool IsEdge(const int& EId) const { return EdgeH.IsKey(EId); }
  /// Tests whether an edge between node IDs SrcNId and DstNId exists in the graph.
  bool IsEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true) const { int EId;  return IsEdge(SrcNId, DstNId, EId, IsDir); }
  /// Tests whether an edge between node IDs SrcNId and DstNId exists in the graph. if an edge exists, return its edge ID in EId
  bool IsEdge(const int& SrcNId, const int& DstNId, int& EId, const bool& IsDir = true) const;
  /// Returns an edge ID between node IDs SrcNId and DstNId, if such an edge exists. Otherwise, return -1.
  int GetEId(const int& SrcNId, const int& DstNId) const { int EId; return IsEdge(SrcNId, DstNId, EId)?EId:-1; }
  /// Returns an iterator referring to the first edge in the graph.
  TEdgeI BegEI() const { return TEdgeI(EdgeH.BegI(), this); }
  /// Returns an iterator referring to the past-the-end edge in the graph.
  TEdgeI EndEI() const { return TEdgeI(EdgeH.EndI(), this); }
  // TODO document TNEGraph::GetEI()
  TEdgeI GetEI(const int& EId) const { return TEdgeI(EdgeH.GetI(EId), this); }
  // TODO document TNEGraph::GetEI()
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
  void Clr() { MxNId=0;  MxEId=0;  NodeH.Clr();  EdgeH.Clr(); }
  /// Reserves memory for a graph of Nodes nodes and Edges edges.
  void Reserve(const int& Nodes, const int& Edges) {
    if (Nodes>0) NodeH.Gen(Nodes/2);  if (Edges>0) EdgeH.Gen(Edges/2); }
  /// Defragments the graph. ##TNEGraph::Defrag
  void Defrag(const bool& OnlyNodeLinks=false);
  /// Checks the graph data structure for internal consistency. ##TNEGraph::IsOk
  bool IsOk(const bool& ThrowExcept=true) const;
  /// Print the graph in a human readable form to an output stream OutF.
  void Dump(FILE *OutF=stdout) const;
  // TODO implement and document TNEGraph::GetSmallGraph()
  static PNEGraph GetSmallGraph();
  friend class TPt<TNEGraph>;
};

// set flags
namespace TSnap {
template <> struct IsMultiGraph<TNEGraph> { enum { Val = 1 }; };
template <> struct IsDirected<TNEGraph> { enum { Val = 1 }; };
}

/////////////////////////////////////////////////
/// Bipartite graph. ##Bipartite_graph
class TBPGraph {
public:
  typedef TBPGraph TNet;
  typedef TPt<TBPGraph> PNet;
  typedef enum { bgsUndef, bgsLeft, bgsRight, bgsBoth } TNodeTy; // left or right hand side node
public:
  class TNode {
  private:
    TInt Id;
    TIntV NIdV;
    TNodeTy NodeTy; // remove
  public:
    TNode() : Id(-1), NIdV(), NodeTy(bgsUndef) { }
    TNode(const int& NId) : Id(NId), NIdV(), NodeTy(true?bgsLeft:bgsRight) { }
    TNode(const TNode& Node) : Id(Node.Id), NIdV(Node.NIdV), NodeTy(Node.NodeTy) { }
    TNode(TSIn& SIn) : Id(SIn), NIdV(SIn), NodeTy(bgsUndef) { TInt Ty(SIn);  NodeTy=(TNodeTy)Ty.Val; }
    void Save(TSOut& SOut) const { Id.Save(SOut);  NIdV.Save(SOut);  TInt(NodeTy).Save(SOut); }
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
    friend class TBPGraph;
  };
  /// Node iterator. Only forward iteration (operator++) is supported.
  class TNodeI {
  private:
    typedef THash<TInt, TNode>::TIter THashIter;
    THashIter LeftHI, RightHI; // iterator over left and right hand-side nodes
  private:
    inline THashIter HI() const { return ! LeftHI.IsEnd()?LeftHI:RightHI; }
  public:
    TNodeI() : LeftHI(), RightHI() { }
    TNodeI(const THashIter& LeftHIter, const THashIter& RightHIter) : LeftHI(LeftHIter), RightHI(RightHIter) { }
    TNodeI(const TNodeI& NodeI) : LeftHI(NodeI.LeftHI), RightHI(NodeI.RightHI) { }
    TNodeI& operator = (const TNodeI& NodeI) { LeftHI = NodeI.LeftHI;  RightHI=NodeI.RightHI;  return *this; }
    /// Increment iterator.
    TNodeI& operator++ (int) { 
      if (! LeftHI.IsEnd()) { 
        LeftHI++; } 
      else if (! RightHI.IsEnd()) { 
        RightHI++; } 
      return *this; }
    bool operator < (const TNodeI& NodeI) const { return LeftHI < NodeI.LeftHI || (LeftHI==NodeI.LeftHI && RightHI < NodeI.RightHI); }
    bool operator == (const TNodeI& NodeI) const { return LeftHI==NodeI.LeftHI && RightHI==NodeI.RightHI; }
    /// Returns ID of the current node.
    int GetId() const { return HI().GetDat().GetId(); }
    /// Tests whether the node is left hand side node.
    bool IsLeft() const { return ! LeftHI.IsEnd(); }
    /// Tests whether the node is right hand side node.
    bool IsRight() const { return ! IsLeft(); }
    /// Returns degree of the current node.
    int GetDeg() const { return HI().GetDat().GetDeg(); }
    /// Returns in-degree of the current node (returns same as value GetDeg() since the graph is undirected).
    int GetInDeg() const { return HI().GetDat().GetInDeg(); }
    /// Returns out-degree of the current node (returns same as value GetDeg() since the graph is undirected).
    int GetOutDeg() const { return HI().GetDat().GetOutDeg(); }
    /// Returns ID of NodeN-th in-node (the node pointing to the current node). ##TBPGraph::TNodeI::GetInNId
    int GetInNId(const int& NodeN) const { return HI().GetDat().GetInNId(NodeN); }
    /// Returns ID of NodeN-th out-node (the node the current node points to). ##TBPGraph::TNodeI::GetOutNId
    int GetOutNId(const int& NodeN) const { return HI().GetDat().GetOutNId(NodeN); }
    /// Returns ID of NodeN-th neighboring node. ##TBPGraph::TNodeI::GetNbrNId
    int GetNbrNId(const int& NodeN) const { return HI().GetDat().GetNbrNId(NodeN); }
    /// Tests whether node with ID NId points to the current node.
    bool IsInNId(const int& NId) const { return HI().GetDat().IsInNId(NId); }
    /// Tests whether the current node points to node with ID NId.
    bool IsOutNId(const int& NId) const { return HI().GetDat().IsOutNId(NId); }
    /// Tests whether node with ID NId is a neighbor of the current node.
    bool IsNbrNId(const int& NId) const { return HI().GetDat().IsNbrNId(NId); }
    friend class TBPGraph;
  };
  /// Edge iterator. Only forward iteration (operator++) is supported.
  class TEdgeI {
  private:
    TNodeI CurNode, EndNode; // end node on the 'left'
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
    /// Gets the source ('left' side) of an edge. Since the graph is undirected this is the node with smaller ID of the edge endpoints.
    int GetSrcNId() const { return CurNode.GetId(); }
    /// Gets destination ('right' side) of an edge. Since the graph is undirected this is the node with greater ID of the edge endpoints.
    int GetDstNId() const { return CurNode.GetOutNId(CurEdge); }
    /// Gets the id of the node on the 'left' side of the edge.
    int GetLNId() const { return GetSrcNId(); }
    /// Gets the id of the node on the 'right' side of the edge.
    int GetRNId() const { return GetDstNId(); }
    friend class TBPGraph;
  };
private:
  TCRef CRef;
  TInt MxNId;                 // maximum node id in the graph
  THash<TInt, TNode> LeftH;   // 'left' nodes
  THash<TInt, TNode> RightH;  // 'right' nodes
private:
  //TNode& GetNode(const int& NId) { return NodeH.GetDat(NId); }
  //const TNode& GetNode(const int& NId) const { return NodeH.GetDat(NId); }
public:
  TBPGraph() : CRef(), MxNId(0), LeftH(), RightH() { }
  /// Constructor that reserves enough memory for a graph of Nodes (LeftNodes+RightNodes) nodes and Edges edges.
  explicit TBPGraph(const int& Nodes, const int& Edges) : MxNId(0) { } //!!! Reserve(Nodes, Edges); }
  TBPGraph(const TBPGraph& BPGraph) : MxNId(BPGraph.MxNId), LeftH(BPGraph.LeftH), RightH(BPGraph.RightH) { }
  /// Constructor for loading the graph from a (binary) stream SIn.
  TBPGraph(TSIn& SIn) : MxNId(SIn), LeftH(SIn), RightH(SIn) { }
  /// Saves the graph to a (binary) stream SOut.
  void Save(TSOut& SOut) const { MxNId.Save(SOut);  LeftH.Save(SOut);  RightH.Save(SOut); }
  /// Static constructor that returns a pointer to the graph. Call: PBPGraph BPGraph = TBPGraph::New();
  static PBPGraph New() { return new TBPGraph(); }
  /// Static constructor that returns a pointer to the graph and reserves enough memory for Nodes nodes and Edges edges. ##TBPGraph::New
  static PBPGraph New(const int& Nodes, const int& Edges) { return new TBPGraph(Nodes, Edges); }
  /// Static constructor that loads the graph from a stream SIn and returns a pointer to it.
  static PBPGraph Load(TSIn& SIn) { return PBPGraph(new TBPGraph(SIn)); }
  /// Allows for run-time checking the type of the graph (see the TGraphFlag for flags).
  bool HasFlag(const TGraphFlag& Flag) const;
  TBPGraph& operator = (const TBPGraph& BPGraph) {
    if (this!=&BPGraph) { MxNId=BPGraph.MxNId;  LeftH=BPGraph.LeftH;  RightH=BPGraph.RightH; }  return *this; }
  
  /// Returns the total number of nodes in the graph.
  int GetNodes() const { return GetLNodes() + GetRNodes(); }
  /// Returns the number of nodes on the 'left' side of the biparite graph.
  int GetLNodes() const { return LeftH.Len(); }
  /// Returns the number of nodes on the 'right' side of the biparite graph.
  int GetRNodes() const { return RightH.Len(); }
  /// Adds a node of ID NId to the graph. ##TBPGraph::AddNode
  int AddNode(int NId = -1, const bool& LeftNode=true);
  /// Adds a node of ID NodeI.GetId() to the graph.
  int AddNode(const TNodeI& NodeI) { return AddNode(NodeI.GetId(), NodeI.IsLeft()); }
  /// Deletes node of ID NId from the graph. ##TBPGraph::DelNode
  void DelNode(const int& NId);
  /// Deletes node of ID NodeI.GetId() from the graph.
  void DelNode(const TNode& NodeI) { DelNode(NodeI.GetId()); }
  /// Tests whether ID NId is a node.
  bool IsNode(const int& NId) const { return IsLNode(NId) || IsRNode(NId); }
  /// Tests whether ID NId is a 'left' side node.
  bool IsLNode(const int& NId) const { return LeftH.IsKey(NId); }
  /// Tests whether ID NId is a 'right' side node.
  bool IsRNode(const int& NId) const { return RightH.IsKey(NId); }
  /// Returns the maximum id of a any node in the graph.
  int GetMxNId() const { return MxNId; }
    
  /// Returns an iterator referring to the first node in the graph.
  TNodeI BegNI() const { return TNodeI(LeftH.BegI(), RightH.BegI()); }
  /// Returns an iterator referring to the past-the-end node in the graph.
  TNodeI EndNI() const { return TNodeI(LeftH.EndI(), RightH.EndI()); }
  /// Returns an iterator referring to the node of ID NId in the graph.
  TNodeI GetNI(const int& NId) const { return IsLNode(NId) ? TNodeI(LeftH.GetI(NId), RightH.EndI()) : TNodeI(LeftH.EndI(), RightH.GetI(NId)); }
  /// Returns an iterator referring to the first 'left' node in the graph.
  TNodeI BegLNI() const { return TNodeI(LeftH.BegI(), RightH.EndI()); }
  /// Returns an iterator referring to the past-the-end 'left' node in the graph.
  TNodeI EndLNI() const { return EndNI(); }
  /// Returns an iterator referring to the first 'right' node in the graph.
  TNodeI BegRNI() const { return TNodeI(LeftH.EndI(), RightH.BegI()); }
  /// Returns an iterator referring to the past-the-end 'right' node in the graph.
  TNodeI EndRNI() const { return EndNI(); }

  /// Returns the number of edges in the graph.
  int GetEdges() const;
  /// Adds an edge between a node LeftNId on the left and a node RightNId on the right side of the bipartite graph. ##TBPGraph::AddEdge
  int AddEdge(const int& LeftNId, const int& RightNId);
  /// Adds an edge between EdgeI.GetLNId() and EdgeI.GetRNId() to the graph.
  int AddEdge(const TEdgeI& EdgeI) { return AddEdge(EdgeI.GetSrcNId(), EdgeI.GetDstNId()); }
  /// Deletes an edge between a node LeftNId on the left and a node RightNId on the right side of the bipartite graph. ##TBPGraph::DelEdge
  void DelEdge(const int& LeftNId, const int& RightNId);
  /// Tests whether an edge between node IDs LeftNId and RightNId exists in the graph.
  bool IsEdge(const int& LeftNId, const int& RightNId) const;
  /// Returns an iterator referring to the first edge in the graph.
  TEdgeI BegEI() const { TNodeI NI=BegLNI(); while (NI<EndLNI() && (NI.GetOutDeg()==0 || NI.GetId()>NI.GetOutNId(0))) { NI++; } return TEdgeI(NI, EndLNI()); }
  /// Returns an iterator referring to the past-the-end edge in the graph.
  TEdgeI EndEI() const { return TEdgeI(EndNI(), EndNI()); }
  /// Not supported/implemented!
  TEdgeI GetEI(const int& EId) const;
  /// Returns an iterator referring to edge (LeftNId, RightNId) in the graph. ##TBPGraph::GetEI
  TEdgeI GetEI(const int& LeftNId, const int& RightNId) const;
    
  /// Returns an ID of a random node in the graph.
  int GetRndNId(TRnd& Rnd=TInt::Rnd);
  /// Returns an ID of a random 'left' node in the graph.
  int GetRndLNId(TRnd& Rnd=TInt::Rnd);
  /// Returns an ID of a random 'right' node in the graph.
  int GetRndRNId(TRnd& Rnd=TInt::Rnd);
  /// Returns an interator referring to a random node in the graph.
  TNodeI GetRndNI(TRnd& Rnd=TInt::Rnd) { return GetNI(GetRndNId(Rnd)); }
  /// Gets a vector IDs of all nodes in the bipartite graph.
  void GetNIdV(TIntV& NIdV) const;
  /// Gets a vector IDs of all 'left' nodes in the bipartite graph.
  void GetLNIdV(TIntV& NIdV) const;
  /// Gets a vector IDs of all 'right' nodes in the bipartite graph.
  void GetRNIdV(TIntV& NIdV) const;

  /// Tests whether the bipartite graph is empty (has zero nodes).
  bool Empty() const { return GetNodes()==0; }
  /// Deletes all nodes and edges from the bipartite graph.
  void Clr() { MxNId=0;  LeftH.Clr();  RightH.Clr(); }
  /// Reserves memory for a biparite graph of Nodes nodes and Edges edges.
  void Reserve(const int& Nodes, const int& Edges);
  /// Defragments the biparite graph. ##TBPGraph::Defrag
  void Defrag(const bool& OnlyNodeLinks=false);
  /// Checks the bipartite graph data structure for internal consistency. ##TBPGraph::IsOk
  bool IsOk(const bool& ThrowExcept=true) const;
  /// Print the biparite graph in a human readable form to an output stream OutF.
  void Dump(FILE *OutF=stdout) const;
  /// Returns a small graph on 2 'left', 3 'right' nodes and 4 edges. ##TBPGraph::GetSmallGraph
  static PBPGraph GetSmallGraph();

  friend class TPt<TBPGraph>;
};

// set flags
namespace TSnap {
template <> struct IsBipart<TBPGraph> { enum { Val = 1 }; };
}
