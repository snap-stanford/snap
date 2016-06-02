#ifndef MMNET_H
#define MMNET_H


class TMMNet;


typedef TPt<TMMNet> PMMNet;

///A single mode in a multimodal directed attributed multigraph
class TModeNet;

///A single cross net in a multimodal directed attributed multigraph
class TCrossNet;


//#//////////////////////////////////////////////
/// The nodes of one particular mode in a TMMNet, and their neighbor vectors as TIntV attributes ##TModeNet::Class
class TModeNet : public TNEANet {
public:
  typedef TModeNet TNetMM;
public:
  /// Node iterator. Only forward iteration (operator++) is supported.
  class TNodeI : public TNEANet::TNodeI {
  public:
    TNodeI() : TNEANet::TNodeI() { }
    TNodeI(const THashIter& NodeHIter, const TModeNet* GraphPt) : TNEANet::TNodeI(NodeHIter, GraphPt) { }
    TNodeI(const TNodeI& NodeI) : TNEANet::TNodeI(NodeI) { }
    TNodeI& operator = (const TNodeI& NodeI) { NodeHI = NodeI.NodeHI; Graph=NodeI.Graph; return *this; }
    /// Increments the iterator.
    TNodeI& operator++ (int) { NodeHI++; return *this; }
    bool operator < (const TNodeI& NodeI) const { return NodeHI < NodeI.NodeHI; }
    bool operator == (const TNodeI& NodeI) const { return NodeHI == NodeI.NodeHI; }
    /// Gets the neighbors (NIds) for the given node for the given CrossNet type.
    void GetNeighborsByCrossNet(TStr& Name, TIntV& Neighbors, const bool isOutEId=false) {
        const TModeNet *TMGraph = static_cast<const TModeNet *>(Graph); TMGraph->GetNeighborsByCrossNet(GetId(), Name, Neighbors, isOutEId); }
    /// Gets all the CrossNets that include the given node.
    void GetCrossNetNames(TStrV& Names) { const TModeNet *TMGraph = static_cast<const TModeNet *>(Graph); TMGraph->GetCrossNetNames(Names); }
    friend class TModeNet;
  };
private:
  TInt ModeId;
  TMMNet *MMNet; // A pointer to the parent MMNet
  THash<TStr, TBool> NeighborTypes; // Mapping of the Neighbor/CrossNets for this mode to whether it requires a one (or two) vectors to store neighbors

public:
  TModeNet() : TNEANet(), ModeId(-1), MMNet(), NeighborTypes() { }
  TModeNet(const int& TypeId) : TNEANet(), ModeId(TypeId), MMNet(), NeighborTypes() { }
  /// Constructor that reserves enough memory for a graph of nodes and edges.
  explicit TModeNet(const int& Nodes, const int& Edges) : TNEANet(Nodes, Edges),
    ModeId(-1), MMNet(), NeighborTypes(){ }
  explicit TModeNet(const int& Nodes, const int& Edges, const int& TypeId) : TNEANet(Nodes, Edges),
    ModeId(TypeId), MMNet(), NeighborTypes() { }
  TModeNet(const TModeNet& Graph) :  TNEANet(true, Graph), ModeId(Graph.ModeId), MMNet(Graph.MMNet), NeighborTypes(Graph.NeighborTypes) { }
  /// Constructor for loading the graph from a (binary) stream SIn.
  TModeNet(TSIn& SIn) : TNEANet(SIn), ModeId(SIn), MMNet(), NeighborTypes(SIn) { }
private:
  TModeNet(const TModeNet& Graph, bool isSubModeGraph) : TNEANet(Graph, isSubModeGraph), ModeId(Graph.ModeId), MMNet(), NeighborTypes() {}
public:
  /// Saves the graph to a (binary) stream SOut.
  void Save(TSOut& SOut) const {
    TNEANet::Save(SOut); ModeId.Save(SOut); NeighborTypes.Save(SOut); }

  /// Deletes the given node from this mode.
  void DelNode(const int& NId);
  /// Gets a list of CrossNets that have this Mode as either a source or destination type.
  void GetCrossNetNames(TStrV& Names) const { NeighborTypes.GetKeyV(Names); }
  /// For the given node, gets all the neighbors for the crossnet type. If this mode is both the source and dest type, use isOutEId to specify direction.
  void GetNeighborsByCrossNet(const int& NId, TStr& Name, TIntV& Neighbors, const bool isOutEId=false) const;

  /// Returns an iterator referring to the first node in the graph.
  TNodeI BegMMNI() const { return TNodeI(NodeH.BegI(), this); }
  /// Returns an iterator referring to the past-the-end node in the graph.
  TNodeI EndMMNI() const { return TNodeI(NodeH.EndI(), this); }
  /// Returns an iterator referring to the node of ID NId in the graph.
  TNodeI GetMMNI(const int& NId) const { return TNodeI(NodeH.GetI(NId), this); }
  /// Deletes all nodes from this mode and edges from associated crossnets.
  void Clr();


  TModeNet& operator = (const TModeNet& Graph) { 
    if (this!=&Graph) {
      MxNId=Graph.MxNId; MxEId=Graph.MxEId; NodeH=Graph.NodeH; EdgeH=Graph.EdgeH;
      KeyToIndexTypeN=Graph.KeyToIndexTypeN; KeyToIndexTypeE=Graph.KeyToIndexTypeE;
      IntDefaultsN=Graph.IntDefaultsN; IntDefaultsE=Graph.IntDefaultsE; StrDefaultsN=Graph.StrDefaultsN; StrDefaultsE=Graph.StrDefaultsE;
      FltDefaultsN=Graph.FltDefaultsN; FltDefaultsE=Graph.FltDefaultsE; VecOfIntVecsN=Graph.VecOfIntVecsN; VecOfIntVecsE=Graph.VecOfIntVecsE;
      VecOfStrVecsN=Graph.VecOfStrVecsN; VecOfStrVecsE=Graph.VecOfStrVecsE; VecOfFltVecsN=Graph.VecOfFltVecsN; VecOfFltVecsE=Graph.VecOfFltVecsE;
      VecOfIntVecVecsN=Graph.VecOfIntVecVecsN; VecOfIntVecVecsE=Graph.VecOfIntVecVecsE; SAttrN=Graph.SAttrN; SAttrE=Graph.SAttrE;
      ModeId=Graph.ModeId; MMNet=Graph.MMNet; NeighborTypes=Graph.NeighborTypes;
    }
    return *this; 
  }

private:
  //method to add neighbors; will be called by TMMNet AddEdge function; outEdge == true iff NId(which is of the type of the TModeNet; i.e. it should refer to a node in this graph) is the source node.
  int AddNeighbor(const int& NId, const int& EId, const bool outEdge, const int linkId, const bool sameMode, bool isDir);
  int AddNeighbor(const int& NId, const int& EId, const bool outEdge, const TStr& linkName, const bool sameMode, bool isDir);
  int DelNeighbor(const int& NId, const int& EId, bool outEdge, const TStr& CrossName, const bool sameMode, bool isDir);
  int DelNeighbor(const int& NId, const int& EId, bool outEdge, const TInt& linkId, const bool sameMode, bool isDir);
  TStr GetNeighborCrossName(const TStr& CrossName, bool isOutEdge, const bool sameMode, bool isDir) const;
  void SetParentPointer(TMMNet* parent);
  int AddNbrType(const TStr& CrossName, const bool sameMode, bool isDir);
  /// Adds a new TIntV node attribute to the hashmap.
  int AddIntVAttrByVecN(const TStr& attr, TVec<TIntV>& Attrs);
  void RemoveCrossNets(TModeNet& Result, TStrV& CrossNets);
  int DelNbrType(const TStr& CrossName);
  int GetAttrTypeN(const TStr& attr) const;
  void ClrNbr(const TStr& CrossNetName, const bool& outEdge, const bool& sameMode, bool& isDir);
public:
  friend class TMMNet;
  friend class TCrossNet;
};


//#///////////////////////////////////////////////
///Implements a single CrossNet consisting of edges between two TModeNets (could be the same TModeNet) ##TCrossNet::Class
class TCrossNet {

public:
  //#///////////////////////////////////////////////
  /// A single edge in the cross net. Has an Edge Id, and the source and destination node ids. (Mode ids are implicit from TCrossNet)
  class TCrossEdge {
  private:
    TInt EId;
    TInt SrcNId, DstNId;
  public:
    TCrossEdge() : EId(-1), SrcNId(-1), DstNId(-1) { }
    TCrossEdge(const int& Id, const int& SourceNId, const int& DestNId) :
      EId(Id), SrcNId(SourceNId), DstNId(DestNId) { }
    TCrossEdge(const TCrossEdge& MultiEdge) : EId(MultiEdge.EId), SrcNId(MultiEdge.SrcNId),
        DstNId(MultiEdge.DstNId) { }
    TCrossEdge(TSIn& SIn) : EId(SIn), SrcNId(SIn), DstNId(SIn) { }
    void Save(TSOut& SOut) const { EId.Save(SOut); SrcNId.Save(SOut); DstNId.Save(SOut); }
    int GetId() const { return EId; }
    int GetSrcNId() const { return SrcNId; }
    int GetDstNId() const { return DstNId; }
    size_t GetMemUsed() const { return EId.GetMemUsed() + SrcNId.GetMemUsed() + DstNId.GetMemUsed(); }
    friend class TCrossNet;
  };
   /// Edge iterator. Only forward iteration (operator++) is supported.
  class TCrossEdgeI {
  private:
    typedef THash<TInt, TCrossEdge>::TIter THashIter;
    THashIter CrossHI;
    const TCrossNet *Graph;
  public:
    TCrossEdgeI() : CrossHI(), Graph(NULL) { }
    TCrossEdgeI(const THashIter& CrossHIter, const TCrossNet *GraphPt) : CrossHI(CrossHIter), Graph(GraphPt) { }
    TCrossEdgeI(const TCrossEdgeI& EdgeI) : CrossHI(EdgeI.CrossHI), Graph(EdgeI.Graph) { }
    TCrossEdgeI& operator = (const TCrossEdgeI& EdgeI) { if (this!=&EdgeI) { CrossHI=EdgeI.CrossHI; Graph=EdgeI.Graph; }  return *this; }
    /// Increment iterator.
    TCrossEdgeI& operator++ (int) { CrossHI++; return *this; }
    bool operator < (const TCrossEdgeI& EdgeI) const { return CrossHI < EdgeI.CrossHI; }
    bool operator == (const TCrossEdgeI& EdgeI) const { return CrossHI == EdgeI.CrossHI; }
    /// Returns edge ID.
    int GetId() const { return CrossHI.GetDat().GetId(); }
    /// Returns the source of the edge.
    int GetSrcNId() const { return CrossHI.GetDat().GetSrcNId(); }
    /// Returns the destination of the edge.
    int GetDstNId() const { return CrossHI.GetDat().GetDstNId(); }

    /// Returns the source mode of the crossnet
    int GetSrcModeId() const { return Graph->GetMode1(); }
    /// Returns the destination mode of the crossnet
    int GetDstModeId() const { return Graph->GetMode2(); }
    /// Returns whether the edge is directed.
    bool IsDirected() const { return Graph->IsDirected(); }


    friend class TCrossNet;
  };

  /// Node/edge integer attribute iterator. Iterates through all nodes/edges for one integer attribute.
  class TAIntI {
  private:
    typedef TIntV::TIter TIntVecIter;
    TIntVecIter HI;
    TStr attr;
    const TCrossNet *Graph;
  public:
    TAIntI() : HI(), attr(), Graph(NULL) { }
    TAIntI(const TIntVecIter& HIter, TStr attribute, const TCrossNet* GraphPt) : HI(HIter), attr(), Graph(GraphPt) { attr = attribute; }
    TAIntI(const TAIntI& I) : HI(I.HI), attr(I.attr), Graph(I.Graph) { }
    TAIntI& operator = (const TAIntI& I) { HI = I.HI; Graph=I.Graph; attr = I.attr; return *this; }
    bool operator < (const TAIntI& I) const { return HI < I.HI; }
    bool operator == (const TAIntI& I) const { return HI == I.HI; }
    /// Returns an attribute of the node.
    TInt GetDat() const { return HI[0]; }
    /// Returns true if the attribute has been deleted.
    bool IsDeleted() const { return GetDat() == Graph->GetIntAttrDefaultE(attr); };
    TAIntI& operator++(int) { HI++; return *this; }
    friend class TCrossNet;
  };

  /// Node/edge string attribute iterator. Iterates through all nodes/edges for one string attribute.
  class TAStrI {
  private:
    typedef TStrV::TIter TStrVecIter;
    TStrVecIter HI;
    TStr attr;
    const TCrossNet *Graph;
  public:
    TAStrI() : HI(), attr(), Graph(NULL) { }
    TAStrI(const TStrVecIter& HIter, TStr attribute, const TCrossNet* GraphPt) : HI(HIter), attr(), Graph(GraphPt) { attr = attribute; }
    TAStrI(const TAStrI& I) : HI(I.HI), attr(I.attr), Graph(I.Graph) { }
    TAStrI& operator = (const TAStrI& I) { HI = I.HI; Graph=I.Graph; attr = I.attr; return *this; }
    bool operator < (const TAStrI& I) const { return HI < I.HI; }
    bool operator == (const TAStrI& I) const { return HI == I.HI; }
    /// Returns an attribute of the node.
    TStr GetDat() const { return HI[0]; }
    /// Returns true if the attribute has been deleted.
    bool IsDeleted() const { return GetDat() == Graph->GetStrAttrDefaultE(attr); };
    TAStrI& operator++(int) { HI++; return *this; }
    friend class TCrossNet;
  };

  /// Node/edge float attribute iterator. Iterates through all nodes/edges for one float attribute.
  class TAFltI {
  private:
    typedef TFltV::TIter TFltVecIter;
    TFltVecIter HI;
    TStr attr;
    const TCrossNet *Graph;
  public:
    TAFltI() : HI(), attr(), Graph(NULL) { }
    TAFltI(const TFltVecIter& HIter, TStr attribute, const TCrossNet* GraphPt) : HI(HIter), attr(), Graph(GraphPt) { attr = attribute; }
    TAFltI(const TAFltI& I) : HI(I.HI), attr(I.attr), Graph(I.Graph) { }
    TAFltI& operator = (const TAFltI& I) { HI = I.HI; Graph=I.Graph; attr = I.attr; return *this; }
    bool operator < (const TAFltI& I) const { return HI < I.HI; }
    bool operator == (const TAFltI& I) const { return HI == I.HI; }
    /// Returns an attribute of the node.
    TFlt GetDat() const { return HI[0]; }
    /// Returns true if the attribute has been deleted.
    bool IsDeleted() const { return GetDat() == Graph->GetFltAttrDefaultE(attr); };
    TAFltI& operator++(int) { HI++; return *this; }
    friend class TCrossNet;
  };
private:
  THash<TInt,TCrossEdge> CrossH; /// The HashTable from Edge id to the corresponding Edge
  TInt MxEId;
  TInt Mode1; ///The first mode. In the case of directed crossnets, this is implicitly understood to be the source mode.
  TInt Mode2; ///The second mode. In the case of directed crossnets, this is implicitly understood to be the destination mode.
  TBool IsDirect;
  TInt CrossNetId;
  TMMNet* Net;
  TStrIntPrH KeyToIndexTypeE;
  THash<TStr, TInt> IntDefaultsE;
  THash<TStr, TStr> StrDefaultsE;
  THash<TStr, TFlt> FltDefaultsE;
  TVec<TIntV> VecOfIntVecsE;
  TVec<TStrV> VecOfStrVecsE;
  TVec<TFltV> VecOfFltVecsE;
  enum { IntType, StrType, FltType };
public:
  TCrossNet() : CrossH(), MxEId(0), Mode1(-1), Mode2(-1), IsDirect(), CrossNetId(), Net(), KeyToIndexTypeE(), IntDefaultsE(), StrDefaultsE(),
    FltDefaultsE(), VecOfIntVecsE(), VecOfStrVecsE(), VecOfFltVecsE() {}
  TCrossNet(TInt MId1, TInt MId2, TInt LId) : CrossH(), MxEId(0), Mode1(MId1), Mode2(MId2), IsDirect(true),CrossNetId(LId), Net(),
    KeyToIndexTypeE(), IntDefaultsE(), StrDefaultsE(), FltDefaultsE(), VecOfIntVecsE(), VecOfStrVecsE(), VecOfFltVecsE() {}
  TCrossNet(TInt MId1, TInt MId2, TBool IsDir, TInt LId) : CrossH(), MxEId(0), Mode1(MId1), Mode2(MId2), IsDirect(IsDir),CrossNetId(LId), Net(),
    KeyToIndexTypeE(), IntDefaultsE(), StrDefaultsE(), FltDefaultsE(), VecOfIntVecsE(), VecOfStrVecsE(), VecOfFltVecsE() {}
  TCrossNet(TSIn& SIn) : CrossH(SIn), MxEId(SIn), Mode1(SIn), Mode2(SIn), IsDirect(SIn), CrossNetId(SIn), Net(),
    KeyToIndexTypeE(SIn), IntDefaultsE(SIn), StrDefaultsE(SIn), FltDefaultsE(SIn), VecOfIntVecsE(SIn), VecOfStrVecsE(SIn), VecOfFltVecsE(SIn) {}
  TCrossNet(const TCrossNet& OtherTCrossNet) : CrossH(OtherTCrossNet.CrossH), MxEId(OtherTCrossNet.MxEId), Mode1(OtherTCrossNet.Mode1),
    Mode2(OtherTCrossNet.Mode2), IsDirect(OtherTCrossNet.IsDirect), CrossNetId(OtherTCrossNet.CrossNetId),Net(OtherTCrossNet.Net), KeyToIndexTypeE(OtherTCrossNet.KeyToIndexTypeE), 
    IntDefaultsE(OtherTCrossNet.IntDefaultsE), StrDefaultsE(OtherTCrossNet.StrDefaultsE), FltDefaultsE(OtherTCrossNet.FltDefaultsE), VecOfIntVecsE(OtherTCrossNet.VecOfIntVecsE),
    VecOfStrVecsE(OtherTCrossNet.VecOfStrVecsE), VecOfFltVecsE(OtherTCrossNet.VecOfFltVecsE) {}

  TCrossNet& operator=(const TCrossNet& OtherTCrossNet) {
    CrossH = OtherTCrossNet.CrossH;
    MxEId = OtherTCrossNet.MxEId;
    Mode1 = OtherTCrossNet.Mode1;
    Mode2 = OtherTCrossNet.Mode2;
    CrossNetId = OtherTCrossNet.CrossNetId;
    IsDirect = OtherTCrossNet.IsDirect;
    Net = OtherTCrossNet.Net;
    KeyToIndexTypeE = OtherTCrossNet.KeyToIndexTypeE;
    IntDefaultsE = OtherTCrossNet.IntDefaultsE;
    StrDefaultsE = OtherTCrossNet.StrDefaultsE;
    FltDefaultsE = OtherTCrossNet.FltDefaultsE;
    VecOfIntVecsE = OtherTCrossNet.VecOfIntVecsE;
    VecOfStrVecsE = OtherTCrossNet.VecOfStrVecsE;
    VecOfFltVecsE = OtherTCrossNet.VecOfFltVecsE;
    return *this;
  }

private:
  void SetParentPointer(TMMNet* parent);
  /// Gets Int edge attribute val.  If not a proper attr, return default.
  TInt GetIntAttrDefaultE(const TStr& attribute) const { return IntDefaultsE.IsKey(attribute) ? IntDefaultsE.GetDat(attribute) : (TInt) TInt::Mn; }
  /// Gets Str edge attribute val.  If not a proper attr, return default.
  TStr GetStrAttrDefaultE(const TStr& attribute) const { return StrDefaultsE.IsKey(attribute) ? StrDefaultsE.GetDat(attribute) : (TStr) TStr::GetNullStr(); }
  /// Gets Flt edge attribute val.  If not a proper attr, return default.
  TFlt GetFltAttrDefaultE(const TStr& attribute) const { return FltDefaultsE.IsKey(attribute) ? FltDefaultsE.GetDat(attribute) : (TFlt) TFlt::Mn; }
  int GetAttrTypeE(const TStr& attr) const;
  TCrossEdge& GetEdge(int eid) { return CrossH[eid]; }
public:
  /// Tests whether an edge with edge ID EId exists in the graph.
  bool IsEdge(const int& EId) const { return CrossH.IsKey(EId); }

  int GetMxEId() const { return MxEId; }
  /// Returns the number of edges in the graph.
  int GetEdges() const { return CrossH.Len(); }
  /// Deletes all nodes and edges from the graph.
  void Clr();

  /// Adds an edge to the CrossNet; Mode1 NId should be the sourceNId always, regardless of whether edge is directed.
  int AddEdge(const int& sourceNId, const int& destNId, int EId=-1);
  /// Edge iterators.
  TCrossEdgeI GetEdgeI(const int& EId) const { return TCrossEdgeI(CrossH.GetI(EId), this); }
  TCrossEdgeI BegEdgeI() const { return TCrossEdgeI(CrossH.BegI(), this); }
  TCrossEdgeI EndEdgeI() const { return TCrossEdgeI(CrossH.EndI(), this); }
  /// Deletes an edge by its id.
  int DelEdge(const int& EId);
  /// Gets the id of the src mode.
  int GetMode1() const { return Mode1; }
  /// Gets the id of the dst mode.
  int GetMode2() const {return Mode2; }
  /// Saves the TCrossNet to the binary stream.
  void Save(TSOut& SOut) const { CrossH.Save(SOut); MxEId.Save(SOut); Mode1.Save(SOut); Mode2.Save(SOut); IsDirect.Save(SOut); CrossNetId.Save(SOut); 
    KeyToIndexTypeE.Save(SOut); IntDefaultsE.Save(SOut); StrDefaultsE.Save(SOut); FltDefaultsE.Save(SOut); VecOfIntVecsE.Save(SOut);
    VecOfStrVecsE.Save(SOut); VecOfFltVecsE.Save(SOut); }

  /// Whether edges in the crossnet are directed.
  bool IsDirected() const { return IsDirect;}
  /// Returns a vector of attr names for edge EId.
  void AttrNameEI(const TInt& EId, TStrV& Names) const {
    AttrNameEI(EId, KeyToIndexTypeE.BegI(), Names);}
  void AttrNameEI(const TInt& EId, TStrIntPrH::TIter CrossHI, TStrV& Names) const;
  /// Returns a vector of attr values for edge EId.
  void AttrValueEI(const TInt& EId, TStrV& Values) const {
    AttrValueEI(EId, KeyToIndexTypeE.BegI(), Values);}
  void AttrValueEI(const TInt& EId, TStrIntPrH::TIter CrossHI, TStrV& Values) const;
  /// Returns a vector of int attr names for edge EId.
  void IntAttrNameEI(const TInt& EId, TStrV& Names) const {
    IntAttrNameEI(EId, KeyToIndexTypeE.BegI(), Names);}
  void IntAttrNameEI(const TInt& EId, TStrIntPrH::TIter CrossHI, TStrV& Names) const;
  /// Returns a vector of attr values for edge EId.
  void IntAttrValueEI(const TInt& EId, TIntV& Values) const {
    IntAttrValueEI(EId, KeyToIndexTypeE.BegI(), Values);}
  void IntAttrValueEI(const TInt& EId, TStrIntPrH::TIter CrossHI, TIntV& Values) const;


  /// Returns a vector of str attr names for node NId.
  void StrAttrNameEI(const TInt& EId, TStrV& Names) const {
    StrAttrNameEI(EId, KeyToIndexTypeE.BegI(), Names);}
  void StrAttrNameEI(const TInt& EId, TStrIntPrH::TIter CrossHI, TStrV& Names) const;
  /// Returns a vector of attr values for node NId.
  void StrAttrValueEI(const TInt& EId, TStrV& Values) const {
    StrAttrValueEI(EId, KeyToIndexTypeE.BegI(), Values);}
  void StrAttrValueEI(const TInt& EId, TStrIntPrH::TIter CrossHI, TStrV& Values) const;
  /// Returns a vector of int attr names for node NId.
  void FltAttrNameEI(const TInt& EId, TStrV& Names) const {
    FltAttrNameEI(EId, KeyToIndexTypeE.BegI(), Names);}
  void FltAttrNameEI(const TInt& EId, TStrIntPrH::TIter CrossHI, TStrV& Names) const;
  /// Returns a vector of attr values for node NId.
  void FltAttrValueEI(const TInt& EId, TFltV& Values) const {
    FltAttrValueEI(EId, KeyToIndexTypeE.BegI(), Values);}
  void FltAttrValueEI(const TInt& EId, TStrIntPrH::TIter CrossHI, TFltV& Values) const;

  /// Attribute based add function for attr to Int value.
  int AddIntAttrDatE(const TCrossEdgeI& EdgeI, const TInt& value, const TStr& attr) { return AddIntAttrDatE(EdgeI.GetId(), value, attr); }
  int AddIntAttrDatE(const int& EId, const TInt& value, const TStr& attr);
  /// Attribute based add function for attr to Str value.
  int AddStrAttrDatE(const TCrossEdgeI& EdgeI, const TStr& value, const TStr& attr) { return AddStrAttrDatE(EdgeI.GetId(), value, attr); }
  int AddStrAttrDatE(const int& EId, const TStr& value, const TStr& attr);
  /// Attribute based add function for attr to Flt value.
  int AddFltAttrDatE(const TCrossEdgeI& EdgeI, const TFlt& value, const TStr& attr) { return AddFltAttrDatE(EdgeI.GetId(), value, attr); }
  int AddFltAttrDatE(const int& EId, const TFlt& value, const TStr& attr);

  /// Gets the value of int attr from the edge attr value vector.
  TInt GetIntAttrDatE(const TCrossEdgeI& EdgeI, const TStr& attr) { return GetIntAttrDatE(EdgeI.GetId(), attr); }
  TInt GetIntAttrDatE(const int& EId, const TStr& attr);
  /// Gets the value of str attr from the edge attr value vector.
  TStr GetStrAttrDatE(const TCrossEdgeI& EdgeI, const TStr& attr) { return GetStrAttrDatE(EdgeI.GetId(), attr); }
  TStr GetStrAttrDatE(const int& EId, const TStr& attr);
  /// Gets the value of flt attr from the edge attr value vector.
  TFlt GetFltAttrDatE(const TCrossEdgeI& EdgeI, const TStr& attr) { return GetFltAttrDatE(EdgeI.GetId(), attr); }
  TFlt GetFltAttrDatE(const int& EId, const TStr& attr);

  /// Returns an iterator referring to the first edge's int attribute.
  TAIntI BegEAIntI(const TStr& attr) const {
    return TAIntI(VecOfIntVecsE[KeyToIndexTypeE.GetDat(attr).Val2].BegI(), attr, this);
  }
  /// Returns an iterator referring to the past-the-end edge's attribute.
  TAIntI EndEAIntI(const TStr& attr) const {
    return TAIntI(VecOfIntVecsE[KeyToIndexTypeE.GetDat(attr).Val2].EndI(), attr, this);
  }
  /// Returns an iterator referring to the edge of ID EId in the graph.
  TAIntI GetEAIntI(const TStr& attr, const int& EId) const {
    return TAIntI(VecOfIntVecsE[KeyToIndexTypeE.GetDat(attr).Val2].GetI(CrossH.GetKeyId(EId)), attr, this);
  }

  /// Returns an iterator referring to the first edge's str attribute.
  TAStrI BegEAStrI(const TStr& attr) const {
    return TAStrI(VecOfStrVecsE[KeyToIndexTypeE.GetDat(attr).Val2].BegI(), attr, this);   }
  /// Returns an iterator referring to the past-the-end edge's attribute.
  TAStrI EndEAStrI(const TStr& attr) const {
    return TAStrI(VecOfStrVecsE[KeyToIndexTypeE.GetDat(attr).Val2].EndI(), attr, this);
  }
  /// Returns an iterator referring to the edge of ID EId in the graph.
  TAStrI GetEAStrI(const TStr& attr, const int& EId) const {
    return TAStrI(VecOfStrVecsE[KeyToIndexTypeE.GetDat(attr).Val2].GetI(CrossH.GetKeyId(EId)), attr, this);
  }
  /// Returns an iterator referring to the first edge's flt attribute.
  TAFltI BegEAFltI(const TStr& attr) const {
    return TAFltI(VecOfFltVecsE[KeyToIndexTypeE.GetDat(attr).Val2].BegI(), attr, this);
  }
  /// Returns an iterator referring to the past-the-end edge's attribute.
  TAFltI EndEAFltI(const TStr& attr) const {
    return TAFltI(VecOfFltVecsE[KeyToIndexTypeE.GetDat(attr).Val2].EndI(), attr, this);
  }
  /// Returns an iterator referring to the edge of ID EId in the graph.
  TAFltI GetEAFltI(const TStr& attr, const int& EId) const {
    return TAFltI(VecOfFltVecsE[KeyToIndexTypeE.GetDat(attr).Val2].GetI(CrossH.GetKeyId(EId)), attr, this);
  }

  /// Deletes the edge attribute for NodeI.
  int DelAttrDatE(const TCrossEdgeI& EdgeI, const TStr& attr) { return DelAttrDatE(EdgeI.GetId(), attr); } 
  int DelAttrDatE(const int& EId, const TStr& attr); 

  /// Adds a new Int edge attribute to the hashmap.
  int AddIntAttrE(const TStr& attr, TInt defaultValue=TInt::Mn);
  /// Adds a new Str edge attribute to the hashmap.
  int AddStrAttrE(const TStr& attr, TStr defaultValue=TStr::GetNullStr());
  /// Adds a new Flt edge attribute to the hashmap.
  int AddFltAttrE(const TStr& attr, TFlt defaultValue=TFlt::Mn);

  /// Removes all the values for edge  attr.
  int DelAttrE(const TStr& attr);

  // Returns true if \c attr exists for edge \c EId and has default value.
  bool IsAttrDeletedE(const int& EId, const TStr& attr) const;
  // Returns true if Int \c attr exists for edge \c EId and has default value.
  bool IsIntAttrDeletedE(const int& EId, const TStr& attr) const;
  // Returns true if Str \c attr exists for edge \c NId and has default value.
  bool IsStrAttrDeletedE(const int& EId, const TStr& attr) const;
  // Returns true if Flt \c attr exists for edge \c NId and has default value.
  bool IsFltAttrDeletedE(const int& EId, const TStr& attr) const;

  // Returns true if EId attr deleted for current edge attr iterator.
  bool EdgeAttrIsDeleted(const int& EId, const TStrIntPrH::TIter& CrossHI) const;
  // Returns true if EId attr deleted for current edge int attr iterator.
  bool EdgeAttrIsIntDeleted(const int& EId, const TStrIntPrH::TIter& CrossHI) const;
  // Returns true if EId attr deleted for current edge str attr iterator.
  bool EdgeAttrIsStrDeleted(const int& EId, const TStrIntPrH::TIter& CrossHI) const;
  // Returns true if EId attr deleted for current edge flt attr iterator.
  bool EdgeAttrIsFltDeleted(const int& EId, const TStrIntPrH::TIter& CrossHI) const;

  // Returns edge attribute value, converted to Str type.
  TStr GetEdgeAttrValue(const int& EId, const TStrIntPrH::TIter& CrossHI) const;

  friend class TMMNet;
  friend class TModeNet;
};

//#///////////////////////////////////////////////
/// Multimodal networks. ##TMMNet::Class
class TMMNet {

public:
  /// TModeNet iterator. Only forward iteration (operator++) is supported.
  class TModeNetI {
  protected:
    typedef THash<TInt, TModeNet>::TIter THashIter;
    THashIter ModeNetHI;
    const TMMNet *Graph;
  public:
    TModeNetI() : ModeNetHI(), Graph(NULL) { }
    TModeNetI(const THashIter& ModeNetHIter, const TMMNet* GraphPt) : ModeNetHI(ModeNetHIter), Graph(GraphPt) { }
    TModeNetI(const TModeNetI& ModeNetI) : ModeNetHI(ModeNetI.ModeNetHI), Graph(ModeNetI.Graph) { }
    TModeNetI& operator = (const TModeNetI& ModeNetI) { ModeNetHI = ModeNetI.ModeNetHI; Graph=ModeNetI.Graph; return *this; }
    /// Increment iterator.
    TModeNetI& operator++ (int) { ModeNetHI++; return *this; }
    bool operator < (const TModeNetI& ModeNetI) const { return ModeNetHI < ModeNetI.ModeNetHI; }
    bool operator == (const TModeNetI& ModeNetI) const { return ModeNetHI == ModeNetI.ModeNetHI; }
    /// Gets the mode id.
    int GetModeId() { return ModeNetHI.GetKey(); }
    /// Gets the name of the mode.
    TStr GetModeName() { return Graph->GetModeName(ModeNetHI.GetKey()); }
    /// Gets a reference to the modenet.
    TModeNet& GetModeNet() { return Graph->GetModeNetById(GetModeId()); }
    friend class TMMNet;
  };

  /// TCrossNet iterator. Only forward iteration (operator++) is supported.
  class TCrossNetI {
  protected:
    typedef THash<TInt, TCrossNet>::TIter THashIter;
    THashIter CrossNetHI;
    const TMMNet *Graph;
  public:
    TCrossNetI() : CrossNetHI(), Graph(NULL) { }
    TCrossNetI(const THashIter& CrossNetHIter, const TMMNet* GraphPt) : CrossNetHI(CrossNetHIter), Graph(GraphPt) { }
    TCrossNetI(const TCrossNetI& CrossNetI) : CrossNetHI(CrossNetI.CrossNetHI), Graph(CrossNetI.Graph) { }
    TCrossNetI& operator = (const TCrossNetI& CrossNetI) { CrossNetHI = CrossNetI.CrossNetHI; Graph=CrossNetI.Graph; return *this; }
    /// Increments iterator.
    TCrossNetI& operator++ (int) { CrossNetHI++; return *this; }
    bool operator < (const TCrossNetI& CrossNetI) const { return CrossNetHI < CrossNetI.CrossNetHI; }
    bool operator == (const TCrossNetI& CrossNetI) const { return CrossNetHI == CrossNetI.CrossNetHI; }
    /// Gets the crossnet id.
    int GetCrossId() { return CrossNetHI.GetKey(); }
    /// Gets the name of the crossnet.
    TStr GetCrossName() { return Graph->GetCrossName(CrossNetHI.GetKey()); }
    /// Gets a reference to the crossnet.
    TCrossNet& GetCrossNet() { return Graph->GetCrossNetById(GetCrossId()); }
    friend class TMMNet;
  };

public:
  TCRef CRef; //Reference counter. Necessary for pointers.

private:

  TInt MxModeId; /// Keeps track of the max mode id.
  TInt MxCrossNetId; /// Keeps track of the max crossnet id
  THash<TInt, TModeNet> TModeNetH;
  THash<TInt, TCrossNet> TCrossNetH;

  THash<TInt,TStr> ModeIdToNameH;
  THash<TStr,TInt> ModeNameToIdH;

  THash<TInt,TStr> CrossIdToNameH;
  THash<TStr,TInt> CrossNameToIdH;

public:
  friend class TCrossNet;
  friend class TModeNet;

public:
  TMMNet() : CRef(), MxModeId(0), MxCrossNetId(0), TModeNetH(), TCrossNetH(), ModeIdToNameH(), ModeNameToIdH(), CrossIdToNameH(), CrossNameToIdH() {}
  TMMNet(const TMMNet& OtherTMMNet) : MxModeId(OtherTMMNet.MxModeId), MxCrossNetId(OtherTMMNet.MxCrossNetId), TModeNetH(OtherTMMNet.TModeNetH), 
    TCrossNetH(OtherTMMNet.TCrossNetH), ModeIdToNameH(OtherTMMNet.ModeIdToNameH), ModeNameToIdH(OtherTMMNet.ModeNameToIdH), CrossIdToNameH(OtherTMMNet.CrossIdToNameH), CrossNameToIdH(OtherTMMNet.CrossNameToIdH) {}
  TMMNet(TSIn& SIn) : MxModeId(SIn), MxCrossNetId(SIn), TModeNetH(SIn), TCrossNetH(SIn), ModeIdToNameH(SIn), ModeNameToIdH(SIn), CrossIdToNameH(SIn), CrossNameToIdH(SIn) { 
    for (THash<TInt, TModeNet>::TIter it = TModeNetH.BegI(); it < TModeNetH.EndI(); it++) {
      it.GetDat().SetParentPointer(this);
    }
    for (THash<TInt, TCrossNet>::TIter it = TCrossNetH.BegI(); it < TCrossNetH.EndI(); it++) {
      it.GetDat().SetParentPointer(this);
    }
  }
  /// Adds a mode to the multimodal network.
  int AddModeNet(const TStr& ModeName);
  /// Deletes a mode from the multimodal network.
  int DelModeNet(const TInt& ModeId); 
  int DelModeNet(const TStr& ModeName);
  /// Adds a crossnet to the multimodal network. Specify modes by id or names; by default, crossnet is directed
  int AddCrossNet(const TStr& ModeName1, const TStr& ModeName2, const TStr& CrossNetName, bool isDir=true);
  int AddCrossNet(const TInt& ModeId1, const TInt& ModeId2, const TStr& CrossNetName, bool isDir=true);
  /// Deletes a crossnet from the multimodal network.
  int DelCrossNet(const TInt& CrossNetId);
  int DelCrossNet(const TStr& CrossNet);

  /// Saves the TMMNet to binary stream.
  void Save(TSOut& SOut) const {MxModeId.Save(SOut); MxCrossNetId.Save(SOut); TModeNetH.Save(SOut); 
    TCrossNetH.Save(SOut); ModeIdToNameH.Save(SOut); ModeNameToIdH.Save(SOut); CrossIdToNameH.Save(SOut);
    CrossNameToIdH.Save(SOut); }
  /// Loads the TMMNet from binary stream.
  static PMMNet Load(TSIn& SIn) { return PMMNet(new TMMNet(SIn)); }
  static PMMNet New() { return PMMNet(new TMMNet()); }

  /// Gets the mode id from the mode name.
  int GetModeId(const TStr& ModeName) const { if (ModeNameToIdH.IsKey(ModeName)) { return ModeNameToIdH.GetDat(ModeName); } else { return -1; }  }
  /// Gets the mode name from the mode id.
  TStr GetModeName(const TInt& ModeId) const { if (ModeIdToNameH.IsKey(ModeId)) { return ModeIdToNameH.GetDat(ModeId); } else {return TStr::GetNullStr();} }
  /// Gets the crossnet id from the crossnet name.
  int GetCrossId(const TStr& CrossName) const { if (CrossNameToIdH.IsKey(CrossName)) { return CrossNameToIdH.GetDat(CrossName); } else { return -1; }   }
  /// Gets the crossnet name from the crossnet id.
  TStr GetCrossName(const TInt& CrossId) const { if (CrossIdToNameH.IsKey(CrossId)) { return CrossIdToNameH.GetDat(CrossId); } else { return TStr::GetNullStr(); }  }

  /// Gets a reference to the modenet.
  TModeNet& GetModeNetByName(const TStr& ModeName) const;
  TModeNet& GetModeNetById(const TInt& ModeId) const;

  /// Gets a reference to the crossnet.
  TCrossNet& GetCrossNetByName(const TStr& CrossName) const;
  TCrossNet& GetCrossNetById(const TInt& CrossId) const;

  /// Iterator over all crossnets.
  TCrossNetI GetCrossNetI(const int& Id) const { return TCrossNetI(TCrossNetH.GetI(Id), this); }
  TCrossNetI BegCrossNetI() const { return TCrossNetI(TCrossNetH.BegI(), this); }
  TCrossNetI EndCrossNetI() const { return TCrossNetI(TCrossNetH.EndI(), this); }
 
  /// Iterator over all modenets.
  TModeNetI GetModeNetI(const int& Id) const { return TModeNetI(TModeNetH.GetI(Id), this); }
  TModeNetI BegModeNetI() const { return TModeNetI(TModeNetH.BegI(), this); }
  TModeNetI EndModeNetI() const { return TModeNetI(TModeNetH.EndI(), this); }

  /// Returns the number of modes in the multimodal network.
  int GetModeNets() { return TModeNetH.Len(); }
  /// Returns the number of crossnets in the multimodal network.
  int GetCrossNets() { return TCrossNetH.Len(); }

  ///Gets the induced subgraph given a vector of crossnet type names.
  PMMNet GetSubgraphByCrossNet(TStrV& CrossNetTypes);
  ///Gets the induced subgraph given a vector of mode type names.
  PMMNet GetSubgraphByModeNet(TStrV& ModeNetTypes);

  /// Converts multimodal network to TNEANet; as attr names can collide, AttrMap specifies the (Mode/Cross Id, old att name, new attr name)
  PNEANet ToNetwork(TIntV& CrossNetTypes, TIntStrStrTrV& NodeAttrMap, TVec<TTriple<TInt, TStr, TStr> >& EdgeAttrMap);
  /// Converts multimodal network to TNEANet; as attr names can collide, AttrMap specifies the Mode/Cross Id -> vec of pairs (old att name, new attr name)
  PNEANet ToNetwork2(TIntV& CrossNetTypes, TIntStrPrVH& NodeAttrMap, THash<TInt, TVec<TPair<TStr, TStr> > >& EdgeAttrMap);

  #ifdef GCC_ATOMIC
  PNEANetMP ToNetworkMP(TStrV& CrossNetNames);
  #endif // GCC_ATOMIC

private:
  void ClrNbr(const TInt& ModeId, const TInt& CrossNetId, const bool& outEdge, const bool& sameMode, bool& isDir);
  int AddMode(const TStr& ModeName, const TInt& ModeId, const TModeNet& ModeNet);
  int AddCrossNet(const TStr& CrossNetName, const TInt& CrossNetId, const TCrossNet& CrossNet);
  int AddNodeAttributes(PNEANet& NewNet, TModeNet& Net, TVec<TPair<TStr, TStr> >& Attrs, int ModeId, int oldId, int NId);
  int AddEdgeAttributes(PNEANet& NewNet, TCrossNet& Net, TVec<TPair<TStr, TStr> >& Attrs, int CrossId, int oldId, int EId);
  void GetPartitionRanges(TIntPrV& Partitions, const TInt& NumPartitions, const TInt& MxVal) const;
};

// set flags
namespace TSnap {
template <> struct IsMultiGraph<TModeNet> { enum { Val = 1 }; };
template <> struct IsDirected<TModeNet> { enum { Val = 1 }; };
}
#endif // MMNET_H
