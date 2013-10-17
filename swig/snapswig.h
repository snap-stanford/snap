class TNGraphNodeI {
private:
  TNGraph::TNodeI NI;
public:
  TNGraphNodeI() : NI() { }
  TNGraphNodeI(const TNGraph::TNodeI& NodeI) : NI(NodeI) { }
  TNGraphNodeI& operator = (const TNGraph::TNodeI& NodeI) { NI = NodeI; return *this; }
  /// Increment iterator.
  TNGraphNodeI& operator++ (int) { NI++; return *this; }
  TNGraphNodeI& Next() { NI++; return *this; }
  bool operator < (const TNGraphNodeI& NodeI) const { return NI < NodeI.NI; }
  bool operator == (const TNGraphNodeI& NodeI) const { return NI == NodeI.NI; }
  /// Returns ID of the current node.
  int GetId() const { return NI.GetId(); }
  /// Returns degree of the current node, the sum of in-degree and out-degree.
  int GetDeg() const { return NI.GetDeg(); }
  /// Returns in-degree of the current node.
  int GetInDeg() const { return NI.GetInDeg(); }
  /// Returns out-degree of the current node.
  int GetOutDeg() const { return NI.GetOutDeg(); }
  /// Returns ID of NodeN-th in-node (the node pointing to the current node). ##TNGraph::TNodeI::GetInNId
  int GetInNId(const int& NodeN) const { return NI.GetInNId(NodeN); }
  /// Returns ID of NodeN-th out-node (the node the current node points to). ##TNGraph::TNodeI::GetOutNId
  int GetOutNId(const int& NodeN) const { return NI.GetOutNId(NodeN); }
  /// Returns ID of NodeN-th neighboring node. ##TNGraph::TNodeI::GetNbrNId
  int GetNbrNId(const int& NodeN) const { return NI.GetNbrNId(NodeN); }
  /// Tests whether node with ID NId points to the current node.
  bool IsInNId(const int& NId) const { return NI.IsInNId(NId); }
  /// Tests whether the current node points to node with ID NId.
  bool IsOutNId(const int& NId) const { return NI.IsOutNId(NId); }
  /// Tests whether node with ID NId is a neighbor of the current node.
  bool IsNbrNId(const int& NId) const { return NI.IsOutNId(NId) || NI.IsInNId(NId); }
};

/// Edge iterator. Only forward iteration (operator++) is supported.
class TNGraphEdgeI {
private:
  TNGraph::TEdgeI EI;
public:
  TNGraphEdgeI() : EI() { }
  TNGraphEdgeI(const TNGraph::TEdgeI& EdgeI) : EI(EdgeI) { }
  TNGraphEdgeI& operator = (const TNGraph::TEdgeI& EdgeI) { EI = EdgeI; return *this; }
  /// Increment iterator.
  TNGraphEdgeI& operator++ (int) { EI++; return *this; }
  TNGraphEdgeI& Next() { EI++; return *this; }
  bool operator < (const TNGraphEdgeI& EdgeI) const { return EI < EdgeI.EI; }
  bool operator == (const TNGraphEdgeI& EdgeI) const { return EI == EdgeI.EI; }
  /// Gets edge ID. Always returns -1 since only edges in multigraphs have explicit IDs.
  int GetId() const { return EI.GetId(); }
  /// Gets the source of an edge. Since the graph is undirected this is the node with smaller ID of the edge endpoints.
  int GetSrcNId() const { return EI.GetSrcNId(); }
  /// Gets destination of an edge. Since the graph is undirected this is the node with greater ID of the edge endpoints.
  int GetDstNId() const { return EI.GetDstNId(); }
};

class TUNGraphNodeI {
private:
  TUNGraph::TNodeI NI;
public:
  TUNGraphNodeI() : NI() { }
  TUNGraphNodeI(const TUNGraph::TNodeI& NodeI) : NI(NodeI) { }
  TUNGraphNodeI& operator = (const TUNGraph::TNodeI& NodeI) { NI = NodeI; return *this; }
  /// Increment iterator.
  TUNGraphNodeI& operator++ (int) { NI++; return *this; }
  TUNGraphNodeI& Next() { NI++; return *this; }
  bool operator < (const TUNGraphNodeI& NodeI) const { return NI < NodeI.NI; }
  bool operator == (const TUNGraphNodeI& NodeI) const { return NI == NodeI.NI; }
  /// Returns ID of the current node.
  int GetId() const { return NI.GetId(); }
  /// Returns degree of the current node, the sum of in-degree and out-degree.
  int GetDeg() const { return NI.GetDeg(); }
  /// Returns in-degree of the current node.
  int GetInDeg() const { return NI.GetInDeg(); }
  /// Returns out-degree of the current node.
  int GetOutDeg() const { return NI.GetOutDeg(); }
  /// Returns ID of NodeN-th in-node (the node pointing to the current node). ##TUNGraph::TNodeI::GetInNId
  int GetInNId(const int& NodeN) const { return NI.GetInNId(NodeN); }
  /// Returns ID of NodeN-th out-node (the node the current node points to). ##TUNGraph::TNodeI::GetOutNId
  int GetOutNId(const int& NodeN) const { return NI.GetOutNId(NodeN); }
  /// Returns ID of NodeN-th neighboring node. ##TUNGraph::TNodeI::GetNbrNId
  int GetNbrNId(const int& NodeN) const { return NI.GetNbrNId(NodeN); }
  /// Tests whether node with ID NId points to the current node.
  bool IsInNId(const int& NId) const { return NI.IsInNId(NId); }
  /// Tests whether the current node points to node with ID NId.
  bool IsOutNId(const int& NId) const { return NI.IsOutNId(NId); }
  /// Tests whether node with ID NId is a neighbor of the current node.
  bool IsNbrNId(const int& NId) const { return NI.IsOutNId(NId) || NI.IsInNId(NId); }
};

/// Edge iterator. Only forward iteration (operator++) is supported.
class TUNGraphEdgeI {
private:
  TUNGraph::TEdgeI EI;
public:
  TUNGraphEdgeI() : EI() { }
  TUNGraphEdgeI(const TUNGraph::TEdgeI& EdgeI) : EI(EdgeI) { }
  TUNGraphEdgeI& operator = (const TUNGraph::TEdgeI& EdgeI) { EI = EdgeI; return *this; }
  /// Increment iterator.
  TUNGraphEdgeI& operator++ (int) { EI++; return *this; }
  TUNGraphEdgeI& Next() { EI++; return *this; }
  bool operator < (const TUNGraphEdgeI& EdgeI) const { return EI < EdgeI.EI; }
  bool operator == (const TUNGraphEdgeI& EdgeI) const { return EI == EdgeI.EI; }
  /// Gets edge ID. Always returns -1 since only edges in multigraphs have explicit IDs.
  int GetId() const { return EI.GetId(); }
  /// Gets the source of an edge. Since the graph is undirected this is the node with smaller ID of the edge endpoints.
  int GetSrcNId() const { return EI.GetSrcNId(); }
  /// Gets destination of an edge. Since the graph is undirected this is the node with greater ID of the edge endpoints.
  int GetDstNId() const { return EI.GetDstNId(); }
};
    
class TNEANetNodeI {
private:
  TNEANet::TNodeI NI;
public:
  TNEANetNodeI() : NI() { }
  TNEANetNodeI(const TNEANet::TNodeI& NodeI) : NI(NodeI) { }
  TNEANetNodeI& operator = (const TNEANet::TNodeI& NodeI) { NI = NodeI; return *this; }
  /// Increment iterator.
  TNEANetNodeI& operator++ (int) { NI++; return *this; }
  TNEANetNodeI& Next() { NI++; return *this; }
  bool operator < (const TNEANetNodeI& NodeI) const { return NI < NodeI.NI; }
  bool operator == (const TNEANetNodeI& NodeI) const { return NI == NodeI.NI; }
  /// Returns ID of the current node.
  int GetId() const { return NI.GetId(); }
  /// Returns degree of the current node, the sum of in-degree and out-degree.
  int GetDeg() const { return NI.GetDeg(); }
  /// Returns in-degree of the current node.
  int GetInDeg() const { return NI.GetInDeg(); }
  /// Returns out-degree of the current node.
  int GetOutDeg() const { return NI.GetOutDeg(); }
  /// Returns ID of NodeN-th in-node (the node pointing to the current node). ##TNEANet::TNodeI::GetInNId
  int GetInNId(const int& NodeN) const { return NI.GetInNId(NodeN); }
  /// Returns ID of NodeN-th out-node (the node the current node points to). ##TNEANet::TNodeI::GetOutNId
  int GetOutNId(const int& NodeN) const { return NI.GetOutNId(NodeN); }
  /// Returns ID of NodeN-th neighboring node. ##TNEANet::TNodeI::GetNbrNId
  int GetNbrNId(const int& NodeN) const { return NI.GetNbrNId(NodeN); }
  /// Tests whether node with ID NId points to the current node.
  bool IsInNId(const int& NId) const { return NI.IsInNId(NId); }
  /// Tests whether the current node points to node with ID NId.
  bool IsOutNId(const int& NId) const { return NI.IsOutNId(NId); }
  /// Tests whether node with ID NId is a neighbor of the current node.
  bool IsNbrNId(const int& NId) const { return NI.IsOutNId(NId) || NI.IsInNId(NId); }
};

/// Edge iterator. Only forward iteration (operator++) is supported.
class TNEANetEdgeI {
private:
  TNEANet::TEdgeI EI;
public:
  TNEANetEdgeI() : EI() { }
  TNEANetEdgeI(const TNEANet::TEdgeI& EdgeI) : EI(EdgeI) { }
  TNEANetEdgeI& operator = (const TNEANet::TEdgeI& EdgeI)
                            { EI = EdgeI; return *this; }
  /// Increment iterator.
  TNEANetEdgeI& operator++ (int) { EI++; return *this; }
  TNEANetEdgeI& Next() { EI++; return *this; }
  bool operator < (const TNEANetEdgeI& EdgeI) const { return EI < EdgeI.EI; }
  bool operator == (const TNEANetEdgeI& EdgeI) const { return EI == EdgeI.EI; }
  /// Gets edge ID. Always returns -1 since only edges in multigraphs have explicit IDs.
  int GetId() const { return EI.GetId(); }
  /// Gets the source of an edge. Since the graph is undirected this is the node with smaller ID of the edge endpoints.
  int GetSrcNId() const { return EI.GetSrcNId(); }
  /// Gets destination of an edge. Since the graph is undirected this is the node with greater ID of the edge endpoints.
  int GetDstNId() const { return EI.GetDstNId(); }
};

typedef TIntV::TIter TIntVecIter;

/// Node/Edge Attr iterator. Iterate through all node for one attr value.
class TNEANetAIntI {
private:
  TNEANet::TAIntI IntAI;
public:
  TNEANetAIntI() : IntAI() { }
  TNEANetAIntI(const TIntVecIter& HIter, TStr attribute, bool isEdgeIter,
               const TNEANet* GraphPt) :
              IntAI(HIter, attribute, isEdgeIter, GraphPt) { }
  TNEANetAIntI(const TNEANet::TAIntI& I) : IntAI(I) { }
  TNEANetAIntI& operator = (const TNEANetAIntI& I)
              { IntAI = I.IntAI; return *this; }
  TNEANetAIntI& Next() { IntAI++; return *this; }
  bool operator < (const TNEANetAIntI& I) const { return IntAI < I.IntAI; }
  bool operator == (const TNEANetAIntI& I) const { return IntAI == I.IntAI; }
  /// Returns an attribute of the node.
  int GetDat() const { return IntAI.GetDat().Val; }
  /// Returns true if node or edge has been deleted.
  bool IsDeleted() const { return IntAI.IsDeleted(); };
  TNEANetAIntI& operator++(int) { IntAI++; return *this; }
//  friend class TNEANet;
};

typedef TStrV::TIter TStrVecIter;

/// Node/Edge Attr iterator. Iterate through all node for one attr value.
class TNEANetAStrI {
private:
  TNEANet::TAStrI StrAI;
public:
  TNEANetAStrI() : StrAI() { }
  TNEANetAStrI(const TStrVecIter& HIter, TStr attribute, bool isEdgeIter,
               const TNEANet* GraphPt) :
  StrAI(HIter, attribute, isEdgeIter, GraphPt) { }
  TNEANetAStrI(const TNEANet::TAStrI& I) : StrAI(I) { }
  TNEANetAStrI& operator = (const TNEANetAStrI& I)
  { StrAI = I.StrAI; return *this; }
  TNEANetAStrI& Next() { StrAI++; return *this; }
  bool operator < (const TNEANetAStrI& I) const { return StrAI < I.StrAI; }
  bool operator == (const TNEANetAStrI& I) const { return StrAI == I.StrAI; }
  /// Returns an attribute of the node.
  char * GetDat() const { return StrAI.GetDat().CStr(); }
  /// Returns true if node or edge has been deleted.
  bool IsDeleted() const { return StrAI.IsDeleted(); };
  TNEANetAStrI& operator++(int) { StrAI++; return *this; }
  //  friend class TNEANet;
};


typedef TFltV::TIter TFltVecIter;

/// Node/Edge Attr iterator. Iterate through all node for one attr value.
class TNEANetAFltI {
private:
  TNEANet::TAFltI FltAI;
public:
  TNEANetAFltI() : FltAI() { }
  TNEANetAFltI(const TFltVecIter& HIter, TStr attribute, bool isEdgeIter,
               const TNEANet* GraphPt) :
  FltAI(HIter, attribute, isEdgeIter, GraphPt) { }
  TNEANetAFltI(const TNEANet::TAFltI& I) : FltAI(I) { }
  TNEANetAFltI& operator = (const TNEANetAFltI& I)
  { FltAI = I.FltAI; return *this; }
  TNEANetAFltI& Next() { FltAI++; return *this; }
  bool operator < (const TNEANetAFltI& I) const { return FltAI < I.FltAI; }
  bool operator == (const TNEANetAFltI& I) const { return FltAI == I.FltAI; }
  /// Returns an attribute of the node.
  double GetDat() const { return FltAI.GetDat().Val; }
  /// Returns true if node or edge has been deleted.
  bool IsDeleted() const { return FltAI.IsDeleted(); };
  TNEANetAFltI& operator++(int) { FltAI++; return *this; }
  //  friend class TNEANet;
};


