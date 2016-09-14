#ifndef NETWORKMP_H
#define NETWORKMP_H

#ifdef GCC_ATOMIC

class TNEANetMP;
/// Pointer to a directed attribute multigraph (TNEANetMP)
typedef TPt<TNEANetMP> PNEANetMP;

//#//////////////////////////////////////////////
/// Directed multigraph with node edge attributes. ##TNEANetMP::Class
class TNEANetMP {
public:
  typedef TNEANetMP TNet;
  typedef TPt<TNEANetMP> PNet;
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
    friend class TNEANetMP;
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
    friend class TNEANetMP;
  };
  /// Node iterator. Only forward iteration (operator++) is supported.
  class TNodeI {
  private:
    typedef THashMP<TInt, TNode>::TIter THashIter;
    THashIter NodeHI;
    const TNEANetMP *Graph;
  public:
    TNodeI() : NodeHI(), Graph(NULL) { }
    TNodeI(const THashIter& NodeHIter, const TNEANetMP* GraphPt) : NodeHI(NodeHIter), Graph(GraphPt) { }
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
    /// Returns ID of EdgeN-th in-node (the node pointing to the current node). ##TNEANetMP::TNodeI::GetInNId
    int GetInNId(const int& EdgeN) const { return Graph->GetEdge(NodeHI.GetDat().GetInEId(EdgeN)).GetSrcNId(); }
    /// Returns ID of EdgeN-th out-node (the node the current node points to). ##TNEANetMP::TNodeI::GetOutNId
    int GetOutNId(const int& EdgeN) const { return Graph->GetEdge(NodeHI.GetDat().GetOutEId(EdgeN)).GetDstNId(); }
    /// Returns ID of EdgeN-th neighboring node. ##TNEANetMP::TNodeI::GetNbrNId
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
    /// Gets vector of str attribute names.
    void GetStrAttrNames(TStrV& Names) const { Graph->StrAttrNameNI(GetId(), Names); }
    /// Gets vector of str attribute values.
    void GetStrAttrVal(TStrV& Val) const { Graph->StrAttrValueNI(GetId(), Val); }
    /// Gets vector of flt attribute names.
    void GetFltAttrNames(TStrV& Names) const { Graph->FltAttrNameNI(GetId(), Names); }
    /// Gets vector of flt attribute values.
    void GetFltAttrVal(TFltV& Val) const { Graph->FltAttrValueNI(GetId(), Val); }
    friend class TNEANetMP;
  };
  /// Edge iterator. Only forward iteration (operator++) is supported.
  class TEdgeI {
  private:
    typedef THashMP<TInt, TEdge>::TIter THashIter;
    THashIter EdgeHI;
    const TNEANetMP *Graph;
  public:
    TEdgeI() : EdgeHI(), Graph(NULL) { }
    TEdgeI(const THashIter& EdgeHIter, const TNEANetMP *GraphPt) : EdgeHI(EdgeHIter), Graph(GraphPt) { }
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
    /// Gets vector of str attribute names.
    void GetStrAttrNames(TStrV& Names) const { Graph->StrAttrNameEI(GetId(), Names); }
    /// Gets vector of str attribute values.
    void GetStrAttrVal(TStrV& Val) const { Graph->StrAttrValueEI(GetId(), Val); }
    /// Gets vector of flt attribute names.
    void GetFltAttrNames(TStrV& Names) const { Graph->FltAttrNameEI(GetId(), Names); }
    /// Gets vector of flt attribute values.
    void GetFltAttrVal(TFltV& Val) const { Graph->FltAttrValueEI(GetId(), Val); }
    friend class TNEANetMP;
  };

  /// Node/edge integer attribute iterator. Iterates through all nodes/edges for one integer attribute.
  class TAIntI {
  private:
    typedef TIntV::TIter TIntVecIter;
    TIntVecIter HI;
    bool isNode;
    TStr attr;
    const TNEANetMP *Graph;
  public:
    TAIntI() : HI(), attr(), Graph(NULL) { }
    TAIntI(const TIntVecIter& HIter, TStr attribute, bool isEdgeIter, const TNEANetMP* GraphPt) : HI(HIter), attr(), Graph(GraphPt) { isNode = !isEdgeIter; attr = attribute; }
    TAIntI(const TAIntI& I) : HI(I.HI), attr(I.attr), Graph(I.Graph) { isNode = I.isNode; }
    TAIntI& operator = (const TAIntI& I) { HI = I.HI; Graph=I.Graph; isNode = I.isNode; attr = I.attr; return *this; }
    bool operator < (const TAIntI& I) const { return HI < I.HI; }
    bool operator == (const TAIntI& I) const { return HI == I.HI; }
    /// Returns an attribute of the node.
    TInt GetDat() const { return HI[0]; }
    /// Returns true if the attribute has been deleted.
    bool IsDeleted() const { return isNode ? GetDat() == Graph->GetIntAttrDefaultN(attr) : GetDat() == Graph->GetIntAttrDefaultE(attr); };
    TAIntI& operator++(int) { HI++; return *this; }
    friend class TNEANetMP;
  };

  /// Node/edge string attribute iterator. Iterates through all nodes/edges for one string attribute.
  class TAStrI {
  private:
    typedef TStrV::TIter TStrVecIter;
    TStrVecIter HI;
    bool isNode;
    TStr attr;
    const TNEANetMP *Graph;
  public:
    TAStrI() : HI(), attr(), Graph(NULL) { }
    TAStrI(const TStrVecIter& HIter, TStr attribute, bool isEdgeIter, const TNEANetMP* GraphPt) : HI(HIter), attr(), Graph(GraphPt) { isNode = !isEdgeIter; attr = attribute; }
    TAStrI(const TAStrI& I) : HI(I.HI), attr(I.attr), Graph(I.Graph) { isNode = I.isNode; }
    TAStrI& operator = (const TAStrI& I) { HI = I.HI; Graph=I.Graph; isNode = I.isNode; attr = I.attr; return *this; }
    bool operator < (const TAStrI& I) const { return HI < I.HI; }
    bool operator == (const TAStrI& I) const { return HI == I.HI; }
    /// Returns an attribute of the node.
    TStr GetDat() const { return HI[0]; }
    /// Returns true if the attribute has been deleted.
    bool IsDeleted() const { return isNode ? GetDat() == Graph->GetStrAttrDefaultN(attr) : GetDat() == Graph->GetStrAttrDefaultE(attr); };
    TAStrI& operator++(int) { HI++; return *this; }
    friend class TNEANetMP;
  };

  /// Node/edge float attribute iterator. Iterates through all nodes/edges for one float attribute.
  class TAFltI {
  private:
    typedef TFltV::TIter TFltVecIter;
    TFltVecIter HI;
    bool isNode;
    TStr attr;
    const TNEANetMP *Graph;
  public:
    TAFltI() : HI(), attr(), Graph(NULL) { }
    TAFltI(const TFltVecIter& HIter, TStr attribute, bool isEdgeIter, const TNEANetMP* GraphPt) : HI(HIter), attr(), Graph(GraphPt) { isNode = !isEdgeIter; attr = attribute; }
    TAFltI(const TAFltI& I) : HI(I.HI), attr(I.attr), Graph(I.Graph) { isNode = I.isNode; }
    TAFltI& operator = (const TAFltI& I) { HI = I.HI; Graph=I.Graph; isNode = I.isNode; attr = I.attr; return *this; }
    bool operator < (const TAFltI& I) const { return HI < I.HI; }
    bool operator == (const TAFltI& I) const { return HI == I.HI; }
    /// Returns an attribute of the node.
    TFlt GetDat() const { return HI[0]; }
    /// Returns true if the attribute has been deleted.
    bool IsDeleted() const { return isNode ? GetDat() == Graph->GetFltAttrDefaultN(attr) : GetDat() == Graph->GetFltAttrDefaultE(attr); };
    TAFltI& operator++(int) { HI++; return *this; }
    friend class TNEANetMP;
  };

private:
  TNode& GetNode(const int& NId) { return NodeH.GetDat(NId); }
  const TNode& GetNode(const int& NId) const { return NodeH.GetDat(NId); }
  TEdge& GetEdge(const int& EId) { return EdgeH.GetDat(EId); }
  const TEdge& GetEdge(const int& EId) const { return EdgeH.GetDat(EId); }

  /// Get Int node attribute val.  If not a proper attr, return default.
  TInt GetIntAttrDefaultN(const TStr& attribute) const { return IntDefaultsN.IsKey(attribute) ? IntDefaultsN.GetDat(attribute) : (TInt) TInt::Mn; }
  /// Get Str node attribute val.  If not a proper attr, return default.
  TStr GetStrAttrDefaultN(const TStr& attribute) const { return StrDefaultsN.IsKey(attribute) ? StrDefaultsN.GetDat(attribute) : (TStr) TStr::GetNullStr(); }
  /// Get Flt node attribute val.  If not a proper attr, return default.
  TFlt GetFltAttrDefaultN(const TStr& attribute) const { return FltDefaultsN.IsKey(attribute) ? FltDefaultsN.GetDat(attribute) : (TFlt) TFlt::Mn; }
  /// Get Int edge attribute val.  If not a proper attr, return default.
  TInt GetIntAttrDefaultE(const TStr& attribute) const { return IntDefaultsE.IsKey(attribute) ? IntDefaultsE.GetDat(attribute) : (TInt) TInt::Mn; }
  /// Get Str edge attribute val.  If not a proper attr, return default.
  TStr GetStrAttrDefaultE(const TStr& attribute) const { return StrDefaultsE.IsKey(attribute) ? StrDefaultsE.GetDat(attribute) : (TStr) TStr::GetNullStr(); }
  /// Get Flt edge attribute val.  If not a proper attr, return default.
  TFlt GetFltAttrDefaultE(const TStr& attribute) const { return FltDefaultsE.IsKey(attribute) ? FltDefaultsE.GetDat(attribute) : (TFlt) TFlt::Mn; }

private:
  TCRef CRef;
  TInt MxNId, MxEId;
  THashMP<TInt, TNode> NodeH;
  THashMP<TInt, TEdge> EdgeH;
  /// KeyToIndexType[N|E]: Key->(Type,Index).
  TStrIntPrH KeyToIndexTypeN, KeyToIndexTypeE;

  THashMP<TStr, TInt> IntDefaultsN, IntDefaultsE;
  THashMP<TStr, TStr> StrDefaultsN, StrDefaultsE;
  THashMP<TStr, TFlt> FltDefaultsN, FltDefaultsE;
  TVec<TIntV> VecOfIntVecsN, VecOfIntVecsE;
  TVec<TStrV> VecOfStrVecsN, VecOfStrVecsE;
  TVec<TFltV> VecOfFltVecsN, VecOfFltVecsE;
  enum { IntType, StrType, FltType };
public:
  TNEANetMP() : CRef(), MxNId(0), MxEId(0), NodeH(), EdgeH(),
    KeyToIndexTypeN(), KeyToIndexTypeE(), IntDefaultsN(), IntDefaultsE(),
    StrDefaultsN(), StrDefaultsE(), FltDefaultsN(), FltDefaultsE(),
    VecOfIntVecsN(), VecOfIntVecsE(), VecOfStrVecsN(), VecOfStrVecsE(),
    VecOfFltVecsN(), VecOfFltVecsE() { }
  /// Constructor that reserves enough memory for a graph of nodes and edges.
  explicit TNEANetMP(const int& Nodes, const int& Edges) : CRef(),
    MxNId(0), MxEId(0), NodeH(), EdgeH(), KeyToIndexTypeN(), KeyToIndexTypeE(),
    IntDefaultsN(), IntDefaultsE(), StrDefaultsN(), StrDefaultsE(),
    FltDefaultsN(), FltDefaultsE(), VecOfIntVecsN(), VecOfIntVecsE(),
    VecOfStrVecsN(), VecOfStrVecsE(), VecOfFltVecsN(), VecOfFltVecsE()
    { Reserve(Nodes, Edges); }
  TNEANetMP(const TNEANetMP& Graph) : MxNId(Graph.MxNId), MxEId(Graph.MxEId),
    NodeH(Graph.NodeH), EdgeH(Graph.EdgeH), KeyToIndexTypeN(), KeyToIndexTypeE(),
    IntDefaultsN(), IntDefaultsE(), StrDefaultsN(), StrDefaultsE(),
    FltDefaultsN(), FltDefaultsE(), VecOfIntVecsN(), VecOfIntVecsE(),
    VecOfStrVecsN(), VecOfStrVecsE(), VecOfFltVecsN(), VecOfFltVecsE() { }
  /// Constructor for loading the graph from a (binary) stream SIn.
  TNEANetMP(TSIn& SIn) : MxNId(SIn), MxEId(SIn), NodeH(SIn), EdgeH(SIn),
    KeyToIndexTypeN(SIn), KeyToIndexTypeE(SIn), IntDefaultsN(SIn), IntDefaultsE(SIn),
    StrDefaultsN(SIn), StrDefaultsE(SIn), FltDefaultsN(SIn), FltDefaultsE(SIn),
    VecOfIntVecsN(SIn), VecOfIntVecsE(SIn), VecOfStrVecsN(SIn),VecOfStrVecsE(SIn),
    VecOfFltVecsN(SIn), VecOfFltVecsE(SIn) { }
  /// Saves the graph to a (binary) stream SOut.
  void Save(TSOut& SOut) const {
    MxNId.Save(SOut); MxEId.Save(SOut); NodeH.Save(SOut); EdgeH.Save(SOut);
    KeyToIndexTypeN.Save(SOut); KeyToIndexTypeE.Save(SOut);
    IntDefaultsN.Save(SOut); IntDefaultsE.Save(SOut);
    StrDefaultsN.Save(SOut); StrDefaultsE.Save(SOut);
    FltDefaultsN.Save(SOut); FltDefaultsE.Save(SOut);
    VecOfIntVecsN.Save(SOut); VecOfIntVecsE.Save(SOut);
    VecOfStrVecsN.Save(SOut); VecOfStrVecsE.Save(SOut);
    VecOfFltVecsN.Save(SOut); VecOfFltVecsE.Save(SOut); }
  /// Static cons returns pointer to graph. Ex: PNEANetMP Graph=TNEANetMP::New().
  static PNEANetMP New() { return PNEANetMP(new TNEANetMP()); }
  /// Static constructor that returns a pointer to the graph and reserves enough memory for Nodes nodes and Edges edges. ##TNEANetMP::New
  static PNEANetMP New(const int& Nodes, const int& Edges) { return PNEANetMP(new TNEANetMP(Nodes, Edges)); }
  /// Static constructor that loads the graph from a stream SIn and returns a pointer to it.
  static PNEANetMP Load(TSIn& SIn) { return PNEANetMP(new TNEANetMP(SIn)); }
  /// Allows for run-time checking the type of the graph (see the TGraphFlag for flags).
  bool HasFlag(const TGraphFlag& Flag) const;
  TNEANetMP& operator = (const TNEANetMP& Graph) { if (this!=&Graph) {
    MxNId=Graph.MxNId; MxEId=Graph.MxEId; NodeH=Graph.NodeH; EdgeH=Graph.EdgeH; }
    return *this; }

  /// Returns the number of nodes in the graph.
  int GetNodes() const { return NodeH.Len(); }
  /// Sets the number of nodes in the graph.
  void SetNodes(const int& Length) { NodeH.SetLen(Length); }
  /// Adds a node of ID NId to the graph. ##TNEANetMP::AddNode
  int AddNode(int NId = -1);
  /// Adds a node of ID NodeI.GetId() to the graph.
  int AddNode(const TNodeI& NodeId) { return AddNode(NodeId.GetId()); }
  /// Adds a node along with its neighbor edges.
  void AddNodeWithEdges(const TInt& NId, TIntV& InEIdV, TIntV& OutEIdV);
//  /// Deletes node of ID NId from the graph. ##TNEANetMP::DelNode
//  void DelNode(const int& NId);
//  /// Deletes node of ID NodeI.GetId() from the graph.
//  void DelNode(const TNode& NodeI) { DelNode(NodeI.GetId()); }
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

  /// Returns the number of nodes reserved in the network.
  int Reserved() const {return NodeH.GetReservedKeyIds();}
  /// Returns the number of edges reserved in the network.
  int ReservedE() const {return EdgeH.GetReservedKeyIds();}

  /// Returns the number of edges in the graph.
  int GetEdges() const { return EdgeH.Len(); }
  /// Sets the number of edges in the graph.
  void SetEdges(const int& Length) { EdgeH.SetLen(Length); }
  /// Sets the MaxEId. Used since AddEdgeUnchecked doesn't affect EId for efficiency.
  void SetMxEId(const TInt& Id) { MxEId = Id; }
  /// Adds an edge with ID EId between node IDs SrcNId and DstNId to the graph. ##TNEANetMP::AddEdge
  int AddEdge(const int& SrcNId, const int& DstNId, int EId  = -1);
  /// Adds an edge between EdgeI.GetSrcNId() and EdgeI.GetDstNId() to the graph.
  int AddEdge(const TEdgeI& EdgeI) { return AddEdge(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), EdgeI.GetId()); }
  /// Adds an edge without checking its adjacent nodes' neighborhood
  void AddEdgeUnchecked(const TInt& EId, const int SrcNId, const int DstNId);
//  /// Deletes an edge with edge ID EId from the graph.
//  void DelEdge(const int& EId);
//  /// Deletes all edges between node IDs SrcNId and DstNId from the graph. ##TNEANetMP::DelEdge
//  void DelEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true);
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
  void Clr() { MxNId=0; MxEId=0; NodeH.Clr(); EdgeH.Clr(),
    KeyToIndexTypeN.Clr(), KeyToIndexTypeE.Clr(), IntDefaultsN.Clr(), IntDefaultsE.Clr(),
    StrDefaultsN.Clr(), StrDefaultsE.Clr(), FltDefaultsN.Clr(), FltDefaultsE.Clr(),
    VecOfIntVecsN.Clr(), VecOfIntVecsE.Clr(), VecOfStrVecsN.Clr(), VecOfStrVecsE.Clr(),
    VecOfFltVecsN.Clr(), VecOfFltVecsE.Clr();}
  /// Reserves memory for a graph of Nodes nodes and Edges edges.
  void Reserve(const int& Nodes, const int& Edges) {
    if (Nodes>0) { NodeH.Gen(Nodes); } if (Edges>0) { EdgeH.Gen(Edges); } }
  /// Reserves memory for dense attributes.
  void ReserveAttr(const int& NIntAttr, const int& NFltAttr, const int& NStrAttr, const int& EIntAttr,
       const int& EFltAttr, const int& EStrAttr) {
    if (NIntAttr > 0) { IntDefaultsN.Gen(NIntAttr); }
    if (NFltAttr > 0) { FltDefaultsN.Gen(NFltAttr); }
    if (NStrAttr > 0) { StrDefaultsN.Gen(NStrAttr); }
    if (EIntAttr > 0) { IntDefaultsE.Gen(EIntAttr); }
    if (EFltAttr > 0) { FltDefaultsE.Gen(EFltAttr); }
    if (EStrAttr > 0) { StrDefaultsE.Gen(EStrAttr); }
  }
//  /// Defragments the graph. ##TNEANetMP::Defrag
  void Defrag(const bool& OnlyNodeLinks=false);
  /// Checks the graph data structure for internal consistency. ##TNEANetMP::IsOk
  bool IsOk(const bool& ThrowExcept=true) const;
  /// Print the graph in a human readable form to an output stream OutF.
  void Dump(FILE *OutF=stdout) const;

  /// Attribute based add function for attr to Int value. ##TNEANetMP::AddIntAttrDatN
  int AddIntAttrDatN(const TNodeI& NodeId, const TInt& value, const TStr& attr) { return AddIntAttrDatN(NodeId.GetId(), value, attr); }
  int AddIntAttrDatN(const int& NId, const TInt& value, const TStr& attr);
  /// Attribute based add function for attr to Str value. ##TNEANetMP::AddStrAttrDatN
  int AddStrAttrDatN(const TNodeI& NodeId, const TStr& value, const TStr& attr) { return AddStrAttrDatN(NodeId.GetId(), value, attr); }
  int AddStrAttrDatN(const int& NId, const TStr& value, const TStr& attr);
  /// Attribute based add function for attr to Flt value. ##TNEANetMP::AddFltAttrDatN
  int AddFltAttrDatN(const TNodeI& NodeId, const TFlt& value, const TStr& attr) { return AddFltAttrDatN(NodeId.GetId(), value, attr); }
  int AddFltAttrDatN(const int& NId, const TFlt& value, const TStr& attr);

  /// Attribute based add function for attr to Int value. ##TNEANetMP::AddIntAttrDatE
  int AddIntAttrDatE(const TEdgeI& EdgeId, const TInt& value, const TStr& attr) { return AddIntAttrDatE(EdgeId.GetId(), value, attr); }
  int AddIntAttrDatE(const int& EId, const TInt& value, const TStr& attr);
  /// Attribute based add function for attr to Str value. ##TNEANetMP::AddStrAttrDatE
  int AddStrAttrDatE(const TEdgeI& EdgeId, const TStr& value, const TStr& attr) { return AddStrAttrDatE(EdgeId.GetId(), value, attr); }
  int AddStrAttrDatE(const int& EId, const TStr& value, const TStr& attr);
  /// Attribute based add function for attr to Flt value. ##TNEANetMP::AddFltAttrDatE
  int AddFltAttrDatE(const TEdgeI& EdgeId, const TFlt& value, const TStr& attr) { return AddFltAttrDatE(EdgeId.GetId(), value, attr); }
  int AddFltAttrDatE(const int& EId, const TFlt& value, const TStr& attr);

  /// Gets the value of int attr from the node attr value vector.
  TInt GetIntAttrDatN(const TNodeI& NodeId, const TStr& attr) { return GetIntAttrDatN(NodeId.GetId(), attr); }
  TInt GetIntAttrDatN(const int& NId, const TStr& attr);

  /// Gets the value of str attr from the node attr value vector.
  TStr GetStrAttrDatN(const TNodeI& NodeId, const TStr& attr) { return GetStrAttrDatN(NodeId.GetId(), attr); }
  TStr GetStrAttrDatN(const int& NId, const TStr& attr);
  /// Gets the value of flt attr from the node attr value vector.
  TFlt GetFltAttrDatN(const TNodeI& NodeId, const TStr& attr) { return GetFltAttrDatN(NodeId.GetId(), attr); }
  TFlt GetFltAttrDatN(const int& NId, const TStr& attr);

  /// Gets the index of the node attr value vector specified by str attr.
  int GetIntAttrIndN(const TStr& attr);
  /// Gets the value of node int attr specified by the attr index.
  TInt GetIntAttrIndDatN(const TNodeI& NodeId, const int& index) { return GetIntAttrIndDatN(NodeId.GetId(), index); }
  TInt GetIntAttrIndDatN(const int& NId, const int& index);

  /// Gets the value of int attr from the edge attr value vector.
  TInt GetIntAttrDatE(const TEdgeI& EdgeId, const TStr& attr) { return GetIntAttrDatE(EdgeId.GetId(), attr); }
  TInt GetIntAttrDatE(const int& EId, const TStr& attr);
  /// Gets the value of str attr from the edge attr value vector.
  TStr GetStrAttrDatE(const TEdgeI& EdgeId, const TStr& attr) { return GetStrAttrDatE(EdgeId.GetId(), attr); }
  TStr GetStrAttrDatE(const int& EId, const TStr& attr);
  /// Gets the value of flt attr from the edge attr value vector.
  TFlt GetFltAttrDatE(const TEdgeI& EdgeId, const TStr& attr) { return GetFltAttrDatE(EdgeId.GetId(), attr); }
  TFlt GetFltAttrDatE(const int& EId, const TStr& attr);

  /// Gets the index of the edge attr value vector specified by str attr.
  int GetIntAttrIndE(const TStr& attr);
  /// Gets the value of edge int attr specified by the attr index.
  TInt GetIntAttrIndDatE(const TEdgeI& EdgeId, const int &index) { return GetIntAttrIndDatE(EdgeId.GetId(), index); }
  TInt GetIntAttrIndDatE(const int& EId, const int& index);

  /// Deletes the node attribute for NodeId.
  int DelAttrDatN(const TNodeI& NodeId, const TStr& attr) { return DelAttrDatN(NodeId.GetId(), attr); }
  int DelAttrDatN(const int& NId, const TStr& attr);
  /// Deletes the edge attribute for NodeId.
  int DelAttrDatE(const TEdgeI& EdgeId, const TStr& attr) { return DelAttrDatE(EdgeId.GetId(), attr); }
  int DelAttrDatE(const int& EId, const TStr& attr);

  /// Adds a new Int node attribute to the hashmap.
  int AddIntAttrN(const TStr& attr, TInt defaultValue=TInt::Mn);
  /// Adds a new Str node attribute to the hashmap.
  int AddStrAttrN(const TStr& attr, TStr defaultValue=TStr::GetNullStr());
  /// Adds a new Flt node attribute to the hashmap.
  int AddFltAttrN(const TStr& attr, TFlt defaultValue=TFlt::Mn);

  /// Adds a new Int edge attribute to the hashmap.
  int AddIntAttrE(const TStr& attr, TInt defaultValue=TInt::Mn);
  /// Adds a new Str edge attribute to the hashmap.
  int AddStrAttrE(const TStr& attr, TStr defaultValue=TStr::GetNullStr());
  /// Adds a new Flt edge attribute to the hashmap.
  int AddFltAttrE(const TStr& attr, TFlt defaultValue=TFlt::Mn);

//  /// Removes all the values for node attr.
//  int DelAttrN(const TStr& attr);
//  /// Removes all the values for edge  attr.
//  int DelAttrE(const TStr& attr);

  // Returns true if NId deleted for current node attr iterator.
  bool NodeAttrIsDeleted(const int& NId, const TStrIntPrH::TIter& NodeHI) const;
  // Returns true if NId deleted for current node int attr iterator.
  bool NodeAttrIsIntDeleted(const int& NId, const TStrIntPrH::TIter& NodeHI) const;
  // Returns true if NId deleted for current node str attr iterator.
  bool NodeAttrIsStrDeleted(const int& NId, const TStrIntPrH::TIter& NodeHI) const;
  // Returns true if NId deleted for current node flt attr iterator.
  bool NodeAttrIsFltDeleted(const int& NId, const TStrIntPrH::TIter& NodeHI) const;

  // Returns true if EId deleted for current edge attr iterator.
  bool EdgeAttrIsDeleted(const int& EId, const TStrIntPrH::TIter& EdgeHI) const;
  // Returns true if EId deleted for current edge int attr iterator.
  bool EdgeAttrIsIntDeleted(const int& EId, const TStrIntPrH::TIter& EdgeHI) const;
  // Returns true if EId deleted for current edge str attr iterator.
  bool EdgeAttrIsStrDeleted(const int& EId, const TStrIntPrH::TIter& EdgeHI) const;
  // Returns true if EId deleted for current edge flt attr iterator.
  bool EdgeAttrIsFltDeleted(const int& EId, const TStrIntPrH::TIter& EdgeHI) const;

  // Returns node attribute value, converted to Str type.
  TStr GetNodeAttrValue(const int& NId, const TStrIntPrH::TIter& NodeHI) const;
  // Returns edge attribute value, converted to Str type.
  TStr GetEdgeAttrValue(const int& EId, const TStrIntPrH::TIter& EdgeHI) const;

  // Get the sum of the weights of all the outgoing edges of the node.
  TFlt GetWeightOutEdges(const TNodeI& NI, const TStr& attr);
  // Check if there is an edge attribute with name attr.
  bool IsFltAttrE(const TStr& attr);
  // Check if there is an edge attribute with name attr.
  bool IsIntAttrE(const TStr& attr);
  // Check if there is an edge attribute with name attr.
  bool IsStrAttrE(const TStr& attr);
  // Get Vector for the Flt Attribute attr.
  TVec<TFlt>& GetFltAttrVecE(const TStr& attr);
  // Get keyid for edge with id EId.
  int GetFltKeyIdE(const int& EId);
  //Fills OutWeights with the outgoing weight from each node.
  void GetWeightOutEdgesV(TFltV& OutWeights, const TFltV& AttrVal) ;

  friend class TPt<TNEANetMP>;
};

// set flags
namespace TSnap {
template <> struct IsMultiGraph<TNEANetMP> { enum { Val = 1 }; };
template <> struct IsDirected<TNEANetMP> { enum { Val = 1 }; };
}

#else

// substitute TNEANet for TNEANetMP on non-gcc platforms
//typedef TNEANet TNEANetMP;
//typedef TPt<TNEANetMP> PNEANetMP;

#endif // GCC_ATOMIC

#endif // NETWORKMP_H
