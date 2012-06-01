/////////////////////////////////////////////////
// Undirected graphs
class TUNGraph;

/// Pointer to an undirected graph (TUNGraph)
typedef TPt<TUNGraph> PUNGraph;
//class TUNEGraph;  typedef TPt<TUNEGraph> PUNEGraph;

/////////////////////////////////////////////////
// Directed graphs

/// Pointer to a directed graph (TNGraph)
class TNGraph;  typedef TPt<TNGraph> PNGraph;
/// Pointer to a directed multigraph (TNEGraph)
class TNEGraph; typedef TPt<TNEGraph> PNEGraph;

/// Undirected graph. ## Undirected_graph
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
    int GetInNId(const int& NodeN) const { return GetNbhNId(NodeN); }
    int GetOutNId(const int& NodeN) const { return GetNbhNId(NodeN); }
    int GetNbhNId(const int& NodeN) const { return NIdV[NodeN]; }
    bool IsNbhNId(const int& NId) const { return NIdV.SearchBin(NId)!=-1; }
    bool IsInNId(const int& NId) const { return IsNbhNId(NId); }
    bool IsOutNId(const int& NId) const { return IsNbhNId(NId); }
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

    /// Return ID of the current node.
    int GetId() const { return NodeHI.GetDat().GetId(); }
    /// Return degree of the current node.
    int GetDeg() const { return NodeHI.GetDat().GetDeg(); }
    /// Return in-degree of the current node (returns same as value GetDeg() since the graph is undirected).
    int GetInDeg() const { return NodeHI.GetDat().GetInDeg(); }
    /// Return out-degree of the current node (returns same as value GetDeg() since the graph is undirected).
    int GetOutDeg() const { return NodeHI.GetDat().GetOutDeg(); }
    /// Return ID of NodeN-th in-node (the node pointing to the current node). ## TNodeI::GetInNId
    int GetInNId(const int& NodeN) const { return NodeHI.GetDat().GetInNId(NodeN); }
    /// Return ID of NodeN-th out-node (the node the current node points to). ## TNodeI::GetOutNId
    int GetOutNId(const int& NodeN) const { return NodeHI.GetDat().GetOutNId(NodeN); }
    /// Return ID of NodeN-th neighboring node. ## TNodeI::GetNbhNId
    int GetNbhNId(const int& NodeN) const { return NodeHI.GetDat().GetNbhNId(NodeN); }
    /// Test whether node with ID NId points to the current node.
    bool IsInNId(const int& NId) const { return NodeHI.GetDat().IsInNId(NId); }
    /// Test whether the current node points to node with ID NId.
    bool IsOutNId(const int& NId) const { return NodeHI.GetDat().IsOutNId(NId); }
    /// Test whether node with ID NId is a neighbor of the current node.
    bool IsNbhNId(const int& NId) const { return NodeHI.GetDat().IsNbhNId(NId); }
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
    /// Get edge ID. Always returns -1 since only edges in multigraphs have explicit IDs.
    int GetId() const { return -1; }
    /// Get the source of an edge. Since the graph is undirected this is the node with smaller ID of the edge endpoints.
    int GetSrcNId() const { return CurNode.GetId(); }
    /// Get destination of an edge. Since the graph is undirected this is the node with greater ID of the edge endpoints.
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
  TUNGraph() : CRef(), MxNId(0), NodeH() { }
  /// Constructor that reserves enough memory for a graph of Nodes nodes and Edges edges.
  explicit TUNGraph(const int& Nodes, const int& Edges) : MxNId(0) { Reserve(Nodes, Edges); }
  TUNGraph(const TUNGraph& Graph) : MxNId(Graph.MxNId), NodeH(Graph.NodeH) { }
  /// Constructor for loading the graph from a (binary) stream SIn.
  TUNGraph(TSIn& SIn) : MxNId(SIn), NodeH(SIn) { }
  /// Saves the graph to a (binary) stream SOut.
  void Save(TSOut& SOut) const { MxNId.Save(SOut);  NodeH.Save(SOut); }
  /// Static constructor that returns a pointer to the graph. Call: PUNGraph Graph = TUNGraph::New();
  static PUNGraph New() { return new TUNGraph(); }
  /// Static constructor that returns a pointer to the graph and reserves enough memory for Nodes nodes and Edges edges. ## TUNGraph::New
  static PUNGraph New(const int& Nodes, const int& Edges) { return new TUNGraph(Nodes, Edges); }
  /// Static constructor that loads the graph from a stream SIn and returns a pointer to it.
  static PUNGraph Load(TSIn& SIn) { return PUNGraph(new TUNGraph(SIn)); }
  /// Allows for run-time checking the type of the graph (see the TGraphFlag for flags).
  bool HasFlag(const TGraphFlag& Flag) const;
  TUNGraph& operator = (const TUNGraph& Graph) {
    if (this!=&Graph) { MxNId=Graph.MxNId;  NodeH=Graph.NodeH; }  return *this; }
  // nodes
  /// Returns the number of nodes in the graph.
  int GetNodes() const { return NodeH.Len(); }
  /// Add a node of ID NId to the graph. ## TUNGraph::AddNode
  int AddNode(int NId = -1);
  /// Add a node of ID NodeI.GetId() to the graph.
  int AddNode(const TNodeI& NodeI) { return AddNode(NodeI.GetId()); }
  /// Add a node of ID NId to the graph and create edges to all nodes in vector NbhNIdV. ## TUNGraph::AddNode-1
  int AddNode(const int& NId, const TIntV& NbhNIdV);
  /// Add a node of ID NId to the graph and create edges to all nodes in vector NIdVId in the vector pool Pool. ## TUNGraph::AddNode-2
  int AddNode(const int& NId, const TVecPool<TInt>& Pool, const int& NIdVId);
  /// Delete node of ID NId from the graph. ## TUNGraph::DelNode
  void DelNode(const int& NId);
  /// Delete node of ID NodeI.GetId() from the graph.
  void DelNode(const TNode& NodeI) { DelNode(NodeI.GetId()); }
  /// Test whether ID NId is a node.
  bool IsNode(const int& NId) const { return NodeH.IsKey(NId); }
  /// Return an iterator referring to the first node in the graph.
  TNodeI BegNI() const { return TNodeI(NodeH.BegI()); }
  /// Return an iterator referring to the past-the-end node in the graph.
  TNodeI EndNI() const { return TNodeI(NodeH.EndI()); }
  /// Return an iterator referring to the node of ID NId in the graph.
  TNodeI GetNI(const int& NId) const { return TNodeI(NodeH.GetI(NId)); }
  // edges
  /// Return the number of edges in the graph.
  int GetEdges() const;
  /// Add an edge between node IDs SrcNId and DstNId to the graph. ## TUNGraph::AddEdge
  int AddEdge(const int& SrcNId, const int& DstNId);
  /// Add an edge between EdgeI.GetSrcNId() and EdgeI.GetDstNId() to the graph.
  int AddEdge(const TEdgeI& EdgeI) { return AddEdge(EdgeI.GetSrcNId(), EdgeI.GetDstNId()); }
  /// Delete an edge between node IDs SrcNId and DstNId from the graph. ## TUNGraph::DelEdge
  void DelEdge(const int& SrcNId, const int& DstNId);
  /// Test whether an edge between node IDs SrcNId and DstNId exists the graph.
  bool IsEdge(const int& SrcNId, const int& DstNId) const;
  /// Return an iterator referring to the first edge in the graph.
  TEdgeI BegEI() const { TNodeI NI=BegNI(); while (NI<EndNI() && (NI.GetOutDeg()==0 || NI.GetId()>NI.GetOutNId(0))) { NI++; } return TEdgeI(NI, EndNI()); }
  /// Return an iterator referring to the past-the-end edge in the graph.
  TEdgeI EndEI() const { return TEdgeI(EndNI(), EndNI()); }
  /// Not supported/implemented!
  TEdgeI GetEI(const int& EId) const;
  /// Return an iterator referring to edge (SrcNId, DstNId) in the graph. ## TUNGraph::GetEI
  TEdgeI GetEI(const int& SrcNId, const int& DstNId) const;

  /// Return an ID of a random node in the graph.
  int GetRndNId(TRnd& Rnd=TInt::Rnd) { return NodeH.GetKey(NodeH.GetRndKeyId(Rnd, 0.8)); }
  /// Return an interator referring to a random node in the graph.
  TNodeI GetRndNI(TRnd& Rnd=TInt::Rnd) { return GetNI(GetRndNId(Rnd)); }
  /// Get a vector IDs of all nodes in the graph.
  void GetNIdV(TIntV& NIdV) const;

  /// Test whether the graph is empty (has zero nodes).
  bool Empty() const { return GetNodes()==0; }
  /// Delete all nodes and edges from the graph.
  void Clr() { MxNId=0;  NodeH.Clr(); }
  /// Reserve memory for a graph of Nodes nodes and Edges edges.
  void Reserve(const int& Nodes, const int& Edges) { if (Nodes>0) NodeH.Gen(Nodes/2); }
  /// Reserve memory for node ID NId having Deg edges.
  void ReserveNIdDeg(const int& NId, const int& Deg) { GetNode(NId).NIdV.Reserve(Deg); }
  /// Defragment the graph. ## TUNGraph::Defrag
  void Defrag(const bool& OnlyNodeLinks=false);
  /// Check the graph data structure for internal consistency. ## TUNGraph::IsOk
  bool IsOk(const bool& ThrowExcept=true) const;
  /// Print the graph in a human readable form to an output stream OutF.
  void Dump(FILE *OutF=stdout) const;
  /// Return a small graph on 5 nodes and 4 edges. ## TUNGraph::GetSmallGraph
  static PUNGraph GetSmallGraph();

  friend class TUNGraphMtx;
  friend class TPt<TUNGraph>;
};

/////////////////////////////////////////////////
// Directed Node Graph
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
    int GetNbhNId(const int& NodeN) const { return NodeN<GetOutDeg()?GetOutNId(NodeN):GetInNId(NodeN-GetOutDeg()); }
    bool IsInNId(const int& NId) const { return InNIdV.SearchBin(NId) != -1; }
    bool IsOutNId(const int& NId) const { return OutNIdV.SearchBin(NId) != -1; }
    bool IsNbhNId(const int& NId) const { return IsOutNId(NId) || IsInNId(NId); }
    void PackOutNIdV() { OutNIdV.Pack(); }
    void PackNIdV() { InNIdV.Pack(); }
    friend class TNGraph;
    friend class TNGraphMtx;
  };
  class TNodeI {
  private:
    typedef THash<TInt, TNode>::TIter THashIter;
    THashIter NodeHI;
  public:
    TNodeI() : NodeHI() { }
    TNodeI(const THashIter& NodeHIter) : NodeHI(NodeHIter) { }
    TNodeI(const TNodeI& NodeI) : NodeHI(NodeI.NodeHI) { }
    TNodeI& operator = (const TNodeI& NodeI) { NodeHI = NodeI.NodeHI;  return *this; }
    TNodeI& operator++ (int) { NodeHI++;  return *this; }
    bool operator < (const TNodeI& NodeI) const { return NodeHI < NodeI.NodeHI; }
    bool operator == (const TNodeI& NodeI) const { return NodeHI == NodeI.NodeHI; }
    int GetId() const { return NodeHI.GetDat().GetId(); }
    int GetDeg() const { return NodeHI.GetDat().GetDeg(); }
    int GetInDeg() const { return NodeHI.GetDat().GetInDeg(); }
    int GetOutDeg() const { return NodeHI.GetDat().GetOutDeg(); }
    int GetInNId(const int& NodeN) const { return NodeHI.GetDat().GetInNId(NodeN); }
    int GetOutNId(const int& NodeN) const { return NodeHI.GetDat().GetOutNId(NodeN); }
    int GetNbhNId(const int& NodeN) const { return NodeHI.GetDat().GetNbhNId(NodeN); }
    bool IsInNId(const int& NId) const { return NodeHI.GetDat().IsInNId(NId); }
    bool IsOutNId(const int& NId) const { return NodeHI.GetDat().IsOutNId(NId); }
    bool IsNbhNId(const int& NId) const { return IsOutNId(NId) || IsInNId(NId); }
    friend class TNGraph;
  };
  class TEdgeI {
  private:
    TNodeI CurNode, EndNode;
    int CurEdge;
  public:
    TEdgeI() : CurNode(), EndNode(), CurEdge(0) { }
    TEdgeI(const TNodeI& NodeI, const TNodeI& EndNodeI, const int& EdgeN=0) : CurNode(NodeI), EndNode(EndNodeI), CurEdge(EdgeN) { }
    TEdgeI(const TEdgeI& EdgeI) : CurNode(EdgeI.CurNode), EndNode(EdgeI.EndNode), CurEdge(EdgeI.CurEdge) { }
    TEdgeI& operator = (const TEdgeI& EdgeI) { if (this!=&EdgeI) { CurNode=EdgeI.CurNode;  EndNode=EdgeI.EndNode;  CurEdge=EdgeI.CurEdge; }  return *this; }
    TEdgeI& operator++ (int) { CurEdge++; if (CurEdge >= CurNode.GetOutDeg()) { CurEdge=0;  CurNode++;
      while (CurNode < EndNode && CurNode.GetOutDeg()==0) { CurNode++; } }  return *this; }
    bool operator < (const TEdgeI& EdgeI) const { return CurNode<EdgeI.CurNode || (CurNode==EdgeI.CurNode && CurEdge<EdgeI.CurEdge); }
    bool operator == (const TEdgeI& EdgeI) const { return CurNode == EdgeI.CurNode && CurEdge == EdgeI.CurEdge; }
    int GetId() const { return -1; }
    int GetSrcNId() const { return CurNode.GetId(); }
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
  explicit TNGraph(const int& Nodes, const int& Edges) : MxNId(0) { Reserve(Nodes, Edges); }
  TNGraph(const TNGraph& Graph) : MxNId(Graph.MxNId), NodeH(Graph.NodeH) { }
  TNGraph(TSIn& SIn) : MxNId(SIn), NodeH(SIn) { }
  void Save(TSOut& SOut) const { MxNId.Save(SOut);  NodeH.Save(SOut); }
  static PNGraph New() { return new TNGraph(); }
  static PNGraph New(const int& Nodes, const int& Edges) { return new TNGraph(Nodes, Edges); }
  static PNGraph Load(TSIn& SIn) { return PNGraph(new TNGraph(SIn)); }
  bool HasFlag(const TGraphFlag& Flag) const;
  TNGraph& operator = (const TNGraph& Graph) {
    if (this!=&Graph) { MxNId=Graph.MxNId;  NodeH=Graph.NodeH; }  return *this; }
  // nodes
  int GetNodes() const { return NodeH.Len(); }
  int AddNode(int NId = -1);
  int AddNode(const TNodeI& NodeId) { return AddNode(NodeId.GetId()); }
  int AddNode(const int& NId, const TIntV& InNIdV, const TIntV& OutNIdV);
  int AddNode(const int& NId, const TVecPool<TInt>& Pool, const int& SrcVId, const int& DstVId);
  void DelNode(const int& NId);
  void DelNode(const TNode& NodeI) { DelNode(NodeI.GetId()); }
  bool IsNode(const int& NId) const { return NodeH.IsKey(NId); }
  TNodeI BegNI() const { return TNodeI(NodeH.BegI()); }
  TNodeI EndNI() const { return TNodeI(NodeH.EndI()); }
  TNodeI GetNI(const int& NId) const { return TNodeI(NodeH.GetI(NId)); }
  const TNode& GetNodeC(const int& NId) const { return NodeH.GetDat(NId); }
  // edges
  int GetEdges() const;
  int AddEdge(const int& SrcNId, const int& DstNId);
  int AddEdge(const TEdgeI& EdgeI) { return AddEdge(EdgeI.GetSrcNId(), EdgeI.GetDstNId()); }
  void DelEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true);
  bool IsEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true) const;
  TEdgeI BegEI() const { TNodeI NI=BegNI();  while(NI<EndNI() && NI.GetOutDeg()==0) NI++;  return TEdgeI(NI, EndNI()); }
  TEdgeI EndEI() const { return TEdgeI(EndNI(), EndNI()); }
  TEdgeI GetEI(const int& EId) const; // not supported
  TEdgeI GetEI(const int& SrcNId, const int& DstNId) const;

  int GetRndNId(TRnd& Rnd=TInt::Rnd) { return NodeH.GetKey(NodeH.GetRndKeyId(Rnd, 0.8)); }
  TNodeI GetRndNI(TRnd& Rnd=TInt::Rnd) { return GetNI(GetRndNId(Rnd)); }
  void GetNIdV(TIntV& NIdV) const;

  bool Empty() const { return GetNodes()==0; }
  void Clr() { MxNId=0;  NodeH.Clr(); }
  void Reserve(const int& Nodes, const int& Edges) { if (Nodes>0) { NodeH.Gen(Nodes/2); } }
  void ReserveNIdInDeg(const int& NId, const int& InDeg) { GetNode(NId).InNIdV.Reserve(InDeg); }
  void ReserveNIdOutDeg(const int& NId, const int& OutDeg) { GetNode(NId).OutNIdV.Reserve(OutDeg); }
  void Defrag(const bool& OnlyNodeLinks=false);
  bool IsOk(const bool& ThrowExcept=true) const;
  void Dump(FILE *OutF=stdout) const;

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
    int GetNbhEId(const int& EdgeN) const { return EdgeN<GetOutDeg()?GetOutEId(EdgeN):GetInEId(EdgeN-GetOutDeg()); }
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
  // iterators
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
    TNodeI& operator++ (int) { NodeHI++;  return *this; }
    bool operator < (const TNodeI& NodeI) const { return NodeHI < NodeI.NodeHI; }
    bool operator == (const TNodeI& NodeI) const { return NodeHI == NodeI.NodeHI; }
    int GetId() const { return NodeHI.GetDat().GetId(); }
    int GetDeg() const { return NodeHI.GetDat().GetDeg(); }
    int GetInDeg() const { return NodeHI.GetDat().GetInDeg(); }
    int GetOutDeg() const { return NodeHI.GetDat().GetOutDeg(); }
    int GetInNId(const int& EdgeN) const { return Graph->GetEdge(NodeHI.GetDat().GetInEId(EdgeN)).GetSrcNId(); }
    int GetOutNId(const int& EdgeN) const { return Graph->GetEdge(NodeHI.GetDat().GetOutEId(EdgeN)).GetDstNId(); }
    int GetNbhNId(const int& EdgeN) const { const TEdge& E = Graph->GetEdge(NodeHI.GetDat().GetNbhEId(EdgeN));
      return GetId()==E.GetSrcNId() ? E.GetDstNId():E.GetSrcNId(); }
    bool IsInNId(const int& NId) const;
    bool IsOutNId(const int& NId) const;
    bool IsNbhNId(const int& NId) const { return IsOutNId(NId) || IsInNId(NId); }
    // edges
    int GetInEId(const int& EdgeN) const { return NodeHI.GetDat().GetInEId(EdgeN); }
    int GetOutEId(const int& EdgeN) const { return NodeHI.GetDat().GetOutEId(EdgeN); }
    int GetNbhEId(const int& EdgeN) const { return NodeHI.GetDat().GetNbhEId(EdgeN); }
    bool IsInEId(const int& EId) const { return NodeHI.GetDat().IsInEId(EId); }
    bool IsOutEId(const int& EId) const { return NodeHI.GetDat().IsOutEId(EId); }
    bool IsNbhEId(const int& EId) const { return IsInEId(EId) || IsOutEId(EId); }
    friend class TNEGraph;
  };
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
    TEdgeI& operator++ (int) { EdgeHI++;  return *this; }
    bool operator < (const TEdgeI& EdgeI) const { return EdgeHI < EdgeI.EdgeHI; }
    bool operator == (const TEdgeI& EdgeI) const { return EdgeHI == EdgeI.EdgeHI; }
    int GetId() const { return EdgeHI.GetDat().GetId(); }
    int GetSrcNId() const { return EdgeHI.GetDat().GetSrcNId(); }
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
  explicit TNEGraph(const int& Nodes, const int& Edges) : CRef(), MxNId(0), MxEId(0) { Reserve(Nodes, Edges); }
  TNEGraph(const TNEGraph& Graph) : MxNId(Graph.MxNId), MxEId(Graph.MxEId), NodeH(Graph.NodeH), EdgeH(Graph.EdgeH) { }
  TNEGraph(TSIn& SIn) : MxNId(SIn), MxEId(SIn), NodeH(SIn), EdgeH(SIn) { }
  void Save(TSOut& SOut) const { MxNId.Save(SOut);  MxEId.Save(SOut);  NodeH.Save(SOut);  EdgeH.Save(SOut); }
  static PNEGraph New() { return PNEGraph(new TNEGraph()); }
  static PNEGraph New(const int& Nodes, const int& Edges) { return PNEGraph(new TNEGraph(Nodes, Edges)); }
  static PNEGraph Load(TSIn& SIn) { return PNEGraph(new TNEGraph(SIn)); }
  bool HasFlag(const TGraphFlag& Flag) const;
  TNEGraph& operator = (const TNEGraph& Graph) { if (this!=&Graph) {
    MxNId=Graph.MxNId; MxEId=Graph.MxEId; NodeH=Graph.NodeH; EdgeH=Graph.EdgeH; }  return *this; }
  // nodes
  int GetNodes() const { return NodeH.Len(); }
  int AddNode(int NId = -1);
  int AddNode(const TNodeI& NodeId) { return AddNode(NodeId.GetId()); }
  void DelNode(const int& NId);
  void DelNode(const TNode& NodeI) { DelNode(NodeI.GetId()); }
  bool IsNode(const int& NId) const { return NodeH.IsKey(NId); }
  TNodeI BegNI() const { return TNodeI(NodeH.BegI(), this); }
  TNodeI EndNI() const { return TNodeI(NodeH.EndI(), this); }
  TNodeI GetNI(const int& NId) const { return TNodeI(NodeH.GetI(NId), this); }
  // edges
  int GetEdges() const { return EdgeH.Len(); }
  int AddEdge(const int& SrcNId, const int& DstNId, int EId  = -1);
  int AddEdge(const TEdgeI& EdgeI) { return AddEdge(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), EdgeI.GetId()); }
  void DelEdge(const int& EId);
  void DelEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true); // deletes all edges between the two nodes
  bool IsEdge(const int& EId) const { return EdgeH.IsKey(EId); }
  bool IsEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true) const { int EId;  return IsEdge(SrcNId, DstNId, EId, IsDir); }
  bool IsEdge(const int& SrcNId, const int& DstNId, int& EId, const bool& IsDir = true) const;
  int GetEId(const int& SrcNId, const int& DstNId) const { int EId; return IsEdge(SrcNId, DstNId, EId)?EId:-1; }
  TEdgeI BegEI() const { return TEdgeI(EdgeH.BegI(), this); }
  TEdgeI EndEI() const { return TEdgeI(EdgeH.EndI(), this); }
  TEdgeI GetEI(const int& EId) const { return TEdgeI(EdgeH.GetI(EId), this); }
  TEdgeI GetEI(const int& SrcNId, const int& DstNId) const { return GetEI(GetEId(SrcNId, DstNId)); }

  int GetRndNId(TRnd& Rnd=TInt::Rnd) { return NodeH.GetKey(NodeH.GetRndKeyId(Rnd, 0.8)); }
  TNodeI GetRndNI(TRnd& Rnd=TInt::Rnd) { return GetNI(GetRndNId(Rnd)); }
  int GetRndEId(TRnd& Rnd=TInt::Rnd) { return EdgeH.GetKey(EdgeH.GetRndKeyId(Rnd, 0.8)); }
  TEdgeI GetRndEI(TRnd& Rnd=TInt::Rnd) { return GetEI(GetRndEId(Rnd)); }
  void GetNIdV(TIntV& NIdV) const;
  void GetEIdV(TIntV& EIdV) const;

  bool Empty() const { return GetNodes()==0; }
  void Clr() { MxNId=0;  MxEId=0;  NodeH.Clr();  EdgeH.Clr(); }
  void Reserve(const int& Nodes, const int& Edges) {
    if (Nodes>0) NodeH.Gen(Nodes/2);  if (Edges>0) EdgeH.Gen(Edges/2); }
  void Defrag(const bool& OnlyNodeLinks=false);
  bool IsOk(const bool& ThrowExcept=true) const;
  void Dump(FILE *OutF=stdout) const;

  static PNEGraph GetSmallGraph();
  friend class TPt<TNEGraph>;
};

// set flags
namespace TSnap {
template <> struct IsMultiGraph<TNEGraph> { enum { Val = 1 }; };
template <> struct IsDirected<TNEGraph> { enum { Val = 1 }; };
}
