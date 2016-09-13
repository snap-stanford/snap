#ifndef GRAPHMP_H
#define GRAPHMP_H

#ifdef GCC_ATOMIC
//#//////////////////////////////////////////////
/// Directed graphs
class TNGraphMP;

typedef TPt<TNGraphMP> PNGraphMP;

//#//////////////////////////////////////////////
/// Directed graph for multi-threaded operations. ##TNGraphMP::Class
class TNGraphMP {
public:
  typedef TNGraphMP TNet;
  typedef TPt<TNGraphMP> PNet;
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
    friend class TNGraphMP;
  };
  /// Node iterator. Only forward iteration (operator++) is supported.
  class TNodeI {
  private:
    typedef THashMP<TInt, TNode>::TIter THashIter;
    THashIter NodeHI;
  public:
    TNodeI() : NodeHI() { }
    TNodeI(const THashIter& NodeHIter) : NodeHI(NodeHIter) { }
    TNodeI(const TNodeI& NodeI) : NodeHI(NodeI.NodeHI) { }
    TNodeI& operator = (const TNodeI& NodeI) { NodeHI = NodeI.NodeHI; return *this; }
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
    /// Sorts the adjacency lists of the current node.
    void SortNIdV() { NodeHI.GetDat().SortNIdV(); }
    /// Returns ID of NodeN-th in-node (the node pointing to the current node). ##TNGraphMP::TNodeI::GetInNId
    int GetInNId(const int& NodeN) const { return NodeHI.GetDat().GetInNId(NodeN); }
    /// Returns ID of NodeN-th out-node (the node the current node points to). ##TNGraphMP::TNodeI::GetOutNId
    int GetOutNId(const int& NodeN) const { return NodeHI.GetDat().GetOutNId(NodeN); }
    /// Returns ID of NodeN-th neighboring node. ##TNGraphMP::TNodeI::GetNbrNId
    int GetNbrNId(const int& NodeN) const { return NodeHI.GetDat().GetNbrNId(NodeN); }
    /// Tests whether node with ID NId points to the current node.
    bool IsInNId(const int& NId) const { return NodeHI.GetDat().IsInNId(NId); }
    /// Tests whether the current node points to node with ID NId.
    bool IsOutNId(const int& NId) const { return NodeHI.GetDat().IsOutNId(NId); }
    /// Tests whether node with ID NId is a neighbor of the current node.
    bool IsNbrNId(const int& NId) const { return IsOutNId(NId) || IsInNId(NId); }
    friend class TNGraphMP;
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
    /// Gets edge ID. Always returns -1 since only edges in multigraphs have explicit IDs.
    int GetId() const { return -1; }
    /// Gets the source node of an edge.
    int GetSrcNId() const { return CurNode.GetId(); }
    /// Gets destination node of an edge.
    int GetDstNId() const { return CurNode.GetOutNId(CurEdge); }
    friend class TNGraphMP;
  };
private:
  TCRef CRef;
  TInt MxNId;
  THashMP<TInt, TNode> NodeH;
private:
  TNode& GetNode(const int& NId) { return NodeH.GetDat(NId); }
  const TNode& GetNode(const int& NId) const { return NodeH.GetDat(NId); }
public:
  TNGraphMP() : CRef(), MxNId(0), NodeH() { }
  /// Constructor that reserves enough memory for a graph of Nodes nodes and Edges edges.
  explicit TNGraphMP(const int& Nodes, const int& Edges) : MxNId(0) { Reserve(Nodes, Edges); }
  TNGraphMP(const TNGraphMP& Graph) : MxNId(Graph.MxNId), NodeH(Graph.NodeH) { }
  /// Constructor that loads the graph from a (binary) stream SIn.
  TNGraphMP(TSIn& SIn) : MxNId(SIn), NodeH(SIn) { }
  /// Saves the graph to a (binary) stream SOut.
  void Save(TSOut& SOut) const { MxNId.Save(SOut); NodeH.Save(SOut); }
  /// Static constructor that returns a pointer to the graph. Call: PNGraphMP Graph = TNGraphMP::New().
  static PNGraphMP New() { return new TNGraphMP(); }
  /// Static constructor that returns a pointer to the graph and reserves enough memory for Nodes nodes and Edges edges. ##TNGraphMP::New
  static PNGraphMP New(const int& Nodes, const int& Edges) { return new TNGraphMP(Nodes, Edges); }
  /// Static constructor that loads the graph from a stream SIn and returns a pointer to it.
  static PNGraphMP Load(TSIn& SIn) { return PNGraphMP(new TNGraphMP(SIn)); }
  /// Allows for run-time checking the type of the graph (see the TGraphFlag for flags).
  bool HasFlag(const TGraphFlag& Flag) const;
  TNGraphMP& operator = (const TNGraphMP& Graph) {
    if (this!=&Graph) { MxNId=Graph.MxNId; NodeH=Graph.NodeH; }  return *this; }
  
  /// Returns the number of nodes in the graph.
  int GetNodes() const { return NodeH.Len(); }
  /// Sets the number of nodes in the graph.
  void SetNodes(const int& Length) { NodeH.SetLen(Length); }
  /// Adds a node of ID NId to the graph. ##TNGraphMP::AddNode
  int AddNode(int NId = -1);
  /// Adds a node of ID NId to the graph without performing checks.
  int AddNodeUnchecked(int NId = -1);
  /// Adds a node of ID NodeI.GetId() to the graph.
  int AddNode(const TNodeI& NodeId) { return AddNode(NodeId.GetId()); }
  /// Adds a node of ID NId to the graph, creates edges to the node from all nodes in vector InNIdV, creates edges from the node to all nodes in vector OutNIdV. ##TNGraphMP::AddNode-1
  int AddNode(const int& NId, const TIntV& InNIdV, const TIntV& OutNIdV);
  /// Adds a node of ID NId to the graph, creates edges to the node from all nodes in vector InNIdV in the vector pool Pool, creates edges from the node to all nodes in vector OutNIdVin the vector pool Pool . ##TNGraphMP::AddNode-2
  int AddNode(const int& NId, const TVecPool<TInt>& Pool, const int& SrcVId, const int& DstVId);
  /// Deletes node of ID NId from the graph. ##TNGraphMP::DelNode
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
  int Reserved() const {return NodeH.GetReservedKeyIds();}

  /// Returns the number of edges in the graph.
  int GetEdges() const;
  /// Adds an edge from node IDs SrcNId to node DstNId to the graph. ##TNGraphMP::AddEdge
  int AddEdge(const int& SrcNId, const int& DstNId);
  /// Adds an edge from node IDs SrcNId to node DstNId to the graph without performing checks.
  int AddEdgeUnchecked(const int& SrcNId, const int& DstNId);
  /// Adds an edge from EdgeI.GetSrcNId() to EdgeI.GetDstNId() to the graph.
  int AddEdge(const TEdgeI& EdgeI) { return AddEdge(EdgeI.GetSrcNId(), EdgeI.GetDstNId()); }
  int AddOutEdge1(int& SrcIdx, const int& SrcNId, const int& DstNId);
  int AddInEdge1(int& DstIdx, const int& SrcNId, const int& DstNId);
  void AddOutEdge2(const int& SrcNId, const int& DstNId);
  void AddInEdge2(const int& SrcNId, const int& DstNId);

  /// Adds a node of ID NId to the graph, creates edges to the node from all nodes in vector InNIdV, creates edges from the node to all nodes in vector OutNIdV.
  void AddNodeWithEdges(const TInt& NId, TIntV& InNIdV, TIntV& OutNIdV);

  /// Deletes an edge from node IDs SrcNId to DstNId from the graph. ##TNGraphMP::DelEdge
  void DelEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true);
  /// Tests whether an edge from node IDs SrcNId to DstNId exists in the graph.
  bool IsEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true) const;
  /// Returns an iterator referring to the first edge in the graph.
  TEdgeI BegEI() const { TNodeI NI=BegNI(); while(NI<EndNI() && NI.GetOutDeg()==0){NI++;} return TEdgeI(NI, EndNI()); }
  /// Returns an iterator referring to the past-the-end edge in the graph.
  TEdgeI EndEI() const { return TEdgeI(EndNI(), EndNI()); }
  // /// Not supported/implemented!
  // TEdgeI GetEI(const int& EId) const; // not supported
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
  void Clr() { MxNId=0; NodeH.Clr(); }
  /// Reserves memory for a graph of Nodes nodes and Edges edges.
  void Reserve(const int& Nodes, const int& Edges) { if (Nodes>0) { NodeH.Gen(Nodes); } }
 /// Reserves memory for node Idx having InDeg in-edges and OutDeg out-edges.
  void ReserveNodeDegs(const int& Idx, const int& InDeg, const int& OutDeg) { if (InDeg > 0) NodeH[Idx].InNIdV.Reserve(InDeg); if (OutDeg > 0) NodeH[Idx].OutNIdV.Reserve(OutDeg); }
  /// Reserves memory for node ID NId having InDeg in-edges.
  void ReserveNIdInDeg(const int& NId, const int& InDeg) { GetNode(NId).InNIdV.Reserve(InDeg); }
  /// Reserves memory for node ID NId having OutDeg out-edges.
  void ReserveNIdOutDeg(const int& NId, const int& OutDeg) { GetNode(NId).OutNIdV.Reserve(OutDeg); }
  /// Sorts in-edges and out-edges.
  void SortEdges(const int& Idx, const int& InDeg, const int& OutDeg) { if (InDeg > 0) NodeH[Idx].InNIdV.Sort(); if (OutDeg > 0) NodeH[Idx].OutNIdV.Sort(); }
  /// Sorts the adjacency lists of each node
  void SortNodeAdjV() { for (TNodeI NI = BegNI(); NI < EndNI(); NI++) { NI.SortNIdV();} }
  /// Defragments the graph. ##TNGraphMP::Defrag
  void Defrag(const bool& OnlyNodeLinks=false);
  /// Checks the graph data structure for internal consistency. ##TNGraphMP::IsOk
  bool IsOk(const bool& ThrowExcept=true) const;
  /// Print the graph in a human readable form to an output stream OutF.
  void Dump(FILE *OutF=stdout) const;
  /// Returns a small graph on 5 nodes and 6 edges. ##TNGraphMP::GetSmallGraph
  static PNGraphMP GetSmallGraph();
  friend class TPt<TNGraphMP>;
  friend class TNGraphMPMtx;
};

// set flags
namespace TSnap {
template <> struct IsDirected<TNGraphMP> { enum { Val = 1 }; };
}

#else

// substitute TNGraph for TNGraphMP on non-gcc platforms
//typedef TNGraph TNGraphMP;
//typedef TPt<TNGraphMP> PNGraphMP;

#endif // GCC_ATOMIC

#endif // GRAPHMP_H

