#ifndef NETWORKMM_H
#define NETWORKMM_H


class TMMNet;


typedef TPt<TMMNet> PMMNet;

//TODO: TCrossNet with the entire hash table

///A single mode in a multimodal directed attributed multigraph
class TModeNet;

/// Pointer to a directed attribute multigraph (TNEANet)
typedef TPt<TModeNet> PModeNet;

class TCrossNet;

typedef TPt<TCrossNet> PCrossNet;

//#//////////////////////////////////////////////
/// Directed multigraph with node edge attributes. ##TNEANet::Class
class TModeNet : TNEANet {
public:
  typedef TModeNet TNetMM;
  typedef TPt<TModeNet> PNetMM;
public:
    /// Node iterator. Only forward iteration (operator++) is supported.
  class TNodeI : TNEANet::TNodeI {
  public:
    TNodeI() : TNEANet::TNodeI() { }
    TNodeI(const THashIter& NodeHIter, const TModeNet* GraphPt) : TNEANet::TNodeI(NodeHIter, GraphPt) { }
    TNodeI(const TNodeI& NodeI) : TNEANet::TNodeI(NodeI) { }
    TNodeI& operator = (const TNodeI& NodeI) { NodeHI = NodeI.NodeHI; Graph=NodeI.Graph; return *this; }
    /// Increment iterator.
    TNodeI& operator++ (int) { NodeHI++; return *this; }
    bool operator < (const TNodeI& NodeI) const { return NodeHI < NodeI.NodeHI; }
    bool operator == (const TNodeI& NodeI) const { return NodeHI == NodeI.NodeHI; }
    void GetNeighborsByLinkType(TStr& Name, TIntV& Neighbors, const bool isOutEId=false) { const TModeNet *TMGraph = static_cast<const TModeNet *>(Graph); TMGraph->GetNeighborsByLinkType(GetId(), Name, Neighbors); }
    void GetLinkTypeNames(TStrV& Names) { const TModeNet *TMGraph = static_cast<const TModeNet *>(Graph); TMGraph->GetLinkTypeNames(Names); }
    friend class TModeNet;
  };
private:
  TInt NModeId;
  PMMNet MMNet; //the parent MMNet
  THash<TStr, TBool> NeighborTypes;

public:
  //TODO: Update constructors with information from fields above.
  TModeNet() : TNEANet(), NModeId(-1), MMNet(),NeighborTypes() { }
  TModeNet(const int& TypeId) : TNEANet(), NModeId(TypeId), MMNet(), NeighborTypes() { }
  /// Constructor that reserves enough memory for a graph of nodes and edges.
  explicit TModeNet(const int& Nodes, const int& Edges) : TNEANet(Nodes, Edges),
    NModeId(-1), MMNet(), NeighborTypes(){ }
  explicit TModeNet(const int& Nodes, const int& Edges, const int& TypeId) : TNEANet(Nodes, Edges),
    NModeId(-1), MMNet(), NeighborTypes() { }
  TModeNet(const TModeNet& Graph) :  TNEANet(Graph), NModeId(Graph.NModeId), MMNet(Graph.MMNet), NeighborTypes(Graph.NeighborTypes) { }
  /// Constructor for loading the graph from a (binary) stream SIn.
  TModeNet(TSIn& SIn) : TNEANet(SIn), NModeId(SIn), MMNet(SIn), NeighborTypes(SIn) { }

  //Make these functions virtual

  /// Saves the graph to a (binary) stream SOut.
  void Save(TSOut& SOut) const {
    TNEANet::Save(SOut); NModeId.Save(SOut); MMNet.Save(SOut); NeighborTypes.Save(SOut); }
  /// Static cons returns pointer to graph. Ex: PModeNet Graph=TMultiGraph::New().
  static PModeNet New() { return PModeNet(new TModeNet()); }
  /// Static constructor that returns a pointer to the graph and reserves enough memory for Nodes nodes and Edges edges. ##TNEANet::New
  static PModeNet New(const int& Nodes, const int& Edges) { return PModeNet(new TModeNet(Nodes, Edges)); }
  /// Static constructor that loads the graph from a stream SIn and returns a pointer to it.
  static PModeNet Load(TSIn& SIn) { return PModeNet(new TModeNet(SIn)); }

  int DelNode (const int& NId); //TODO(sramas15): finish implementing
  void GetLinkTypeNames(TStrV& Names) const { NeighborTypes.GetKeyV(Names); }
  void GetNeighborsByLinkType(const int& NId, TStr& Name, TIntV& Neighbors, const bool isOutEId=false) const;

  /// Returns an iterator referring to the first node in the graph.
  TNodeI BegMMNI() const { return TNodeI(NodeH.BegI(), this); }
  /// Returns an iterator referring to the past-the-end node in the graph.
  TNodeI EndMMNI() const { return TNodeI(NodeH.EndI(), this); }
  /// Returns an iterator referring to the node of ID NId in the graph.
  TNodeI GetMMNI(const int& NId) const { return TNodeI(NodeH.GetI(NId), this); }


private:
  //method to add neighbors; will be called by TMMNet AddEdge function; outEdge == true iff NId(which is of the type of the TModeNet; i.e. it should refer to a node in this graph) is the source node.
  int AddNeighbor(const int& NId, const int& EId, const bool outEdge, const int linkId, const bool sameMode, bool isDir);
  int AddNeighbor(const int& NId, const int& EId, const bool outEdge, const TStr& linkName, const bool sameMode, bool isDir);
  int DelNeighbor(const int& NId, const int& EId, bool outEdge, const TStr& LinkName, const bool sameMode, bool isDir);
  int DelNeighbor(const int& NId, const int& EId, bool outEdge, const TInt& linkId, const bool sameMode, bool isDir);
  TStr GetNeighborLinkName(const TStr& LinkName, bool isOutEdge, const bool sameMode, bool isDir) const;
  void SetParentPointer(PMMNet parent);
  int AddNbrType(const TStr& LinkName, const bool sameMode, bool isDir);
public:

  ///When we create a new link type, we need to add a new neighbor type here.

  /// Deletes all nodes and edges from the graph.
  void Clr() { TNEANet::Clr(); NModeId = -1; MMNet.Clr(); NeighborTypes.Clr(); }

  friend class TPt<TModeNet>;
  friend class TMMNet;
  friend class TCrossNet;
};

class TCrossNet {

public:
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
    friend class TCrossNet;
  };
    /// Edge iterator. Only forward iteration (operator++) is supported.
  class TCrossEdgeI {
  private:
    typedef THash<TInt, TCrossEdge>::TIter THashIter;
    THashIter EdgeHI;
    const TCrossNet *Graph;
  public:
    TCrossEdgeI() : EdgeHI(), Graph(NULL) { }
    TCrossEdgeI(const THashIter& EdgeHIter, const TCrossNet *GraphPt) : EdgeHI(EdgeHIter), Graph(GraphPt) { }
    TCrossEdgeI(const TCrossEdgeI& EdgeI) : EdgeHI(EdgeI.EdgeHI), Graph(EdgeI.Graph) { }
    TCrossEdgeI& operator = (const TCrossEdgeI& EdgeI) { if (this!=&EdgeI) { EdgeHI=EdgeI.EdgeHI; Graph=EdgeI.Graph; }  return *this; }
    /// Increment iterator.
    TCrossEdgeI& operator++ (int) { EdgeHI++; return *this; }
    bool operator < (const TCrossEdgeI& EdgeI) const { return EdgeHI < EdgeI.EdgeHI; }
    bool operator == (const TCrossEdgeI& EdgeI) const { return EdgeHI == EdgeI.EdgeHI; }
    /// Returns edge ID.
    int GetId() const { return EdgeHI.GetDat().GetId(); }
    /// Returns the source of the edge.
    int GetSrcNId() const { return EdgeHI.GetDat().GetSrcNId(); }
    /// Returns the destination of the edge.
    int GetDstNId() const { return EdgeHI.GetDat().GetDstNId(); }

    /// Returns the source of the edge.
    int GetSrcModeId() const { return Graph->GetMode1(); }
    /// Returns the destination of the edge.
    int GetDstModeId() const { return Graph->GetMode2(); }
    /// Returns whether the edge is directed.
//    int IsDirected() const { return Graph->IsDirected(); } //TODO

    friend class TCrossNet;
  };

private:
  THash<TInt,TCrossEdge> LinkH;
  TInt MxEId;
  TInt Mode1;
  TInt Mode2;
  TInt LinkTypeId;
  PMMNet Net;
  //Constructors

public:

  TCRef CRef; //Reference counter. Necessary for pointers.

  TCrossNet() : CRef(),LinkH(), MxEId(0), Mode1(), Mode2(), LinkTypeId(), Net() {}
  TCrossNet(TInt MId1, TInt MId2, TInt LId) : LinkH(), MxEId(0), Mode1(MId1), Mode2(MId2), LinkTypeId(LId), Net() {}
  TCrossNet(TSIn& SIn) : LinkH(SIn), MxEId(SIn), Mode1(SIn), Mode2(SIn), LinkTypeId(SIn), Net() {}
  TCrossNet(const TCrossNet& OtherTCrossNet) :LinkH(OtherTCrossNet.LinkH), MxEId(OtherTCrossNet.MxEId), Mode1(OtherTCrossNet.Mode1),
    Mode2(OtherTCrossNet.Mode2),LinkTypeId(OtherTCrossNet.LinkTypeId),Net(OtherTCrossNet.Net){}
  TCrossNet& operator=(const TCrossNet& OtherTCrossNet) {
    LinkH = OtherTCrossNet.LinkH;
    MxEId = OtherTCrossNet.MxEId;
    Mode1 = OtherTCrossNet.Mode1;
    Mode2 = OtherTCrossNet.Mode2;
    LinkTypeId = OtherTCrossNet.LinkTypeId;
    Net = OtherTCrossNet.Net;
    return *this;
  }
private:
  void SetParentPointer(PMMNet parent);
public:
  //TODO: DelEdge or DelLink? Same with Add
  int AddEdge(const int& sourceNId, const int& destNId, int EId=-1);
  TCrossEdgeI GetEdgeI(const int& EId) const { return TCrossEdgeI(LinkH.GetI(EId), this); }
  TCrossEdgeI BegEdgeI() const { return TCrossEdgeI(LinkH.BegI(), this); }
  TCrossEdgeI EndEdgeI() const { return TCrossEdgeI(LinkH.EndI(), this); }
  int DelEdge(const int& EId);
  int GetMode1() const { return Mode1; }
  int GetMode2() const {return Mode2; }
  void Save(TSOut& SOut) const { LinkH.Save(SOut); MxEId.Save(SOut); Mode1.Save(SOut); Mode2.Save(SOut); LinkTypeId.Save(SOut); } //TODO
  friend class TMMNet;
};


//The container class for a multimodal network
class TMMNet {

public:
  /// Node iterator. Only forward iteration (operator++) is supported.
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
    //PModeNet GetModeNet() {}
    friend class TMMNet;
  };

private:

  TInt MxModeId; //The number of modes
  TInt MxLinkTypeId;
  THash<TInt, TModeNet> TModeNetH;
  THash<TInt, TCrossNet> TCrossNetH;

  THash<TInt,TStr> ModeIdToNameH;
  THash<TStr,TInt> ModeNameToIdH;

  THash<TInt,TStr> LinkIdToNameH;
  THash<TStr,TInt> LinkNameToIdH;

public:
  TCRef CRef; //Reference counter. Necessary for pointers.
  friend class TCrossNet;
  friend class TModeNet;

public:
  TMMNet() : CRef(), MxModeId(0), MxLinkTypeId(0), TModeNetH(), TCrossNetH(), ModeIdToNameH(), ModeNameToIdH(), LinkIdToNameH(), LinkNameToIdH() {}
  TMMNet(const TMMNet& OtherTMMNet) : MxModeId(OtherTMMNet.MxModeId), MxLinkTypeId(OtherTMMNet.MxLinkTypeId), TModeNetH(OtherTMMNet.TModeNetH), 
    TCrossNetH(OtherTMMNet.TCrossNetH), ModeIdToNameH(OtherTMMNet.ModeIdToNameH), ModeNameToIdH(OtherTMMNet.ModeNameToIdH), LinkIdToNameH(OtherTMMNet.LinkIdToNameH), LinkNameToIdH(OtherTMMNet.LinkNameToIdH) {}
  TMMNet(TSIn& SIn) : MxModeId(SIn), MxLinkTypeId(SIn), TModeNetH(SIn), TCrossNetH(SIn), ModeIdToNameH(SIn), ModeNameToIdH(SIn), LinkIdToNameH(SIn), LinkNameToIdH(SIn) {}
  int AddMode(const TStr& ModeName);
  int DelMode(const TInt& ModeId); // TODO(sramas15): finish implementing
  int DelMode(const TStr& ModeName);
  int AddLinkType(const TStr& ModeName1, const TStr& ModeName2, const TStr& LinkTypeName);
  int AddLinkType(const TInt& ModeId1, const TInt& ModeId2, const TStr& LinkTypeName);
  int DelLinkType(const TInt& LinkTypeId); // TODO(sramas15): finish implementing
  int DelLinkType(const TStr& LinkType);

  void Save(TSOut& SOut) const {MxModeId.Save(SOut); MxLinkTypeId.Save(SOut); TModeNetH.Save(SOut); 
    TCrossNetH.Save(SOut); ModeIdToNameH.Save(SOut); ModeNameToIdH.Save(SOut); LinkIdToNameH.Save(SOut);
    LinkNameToIdH.Save(SOut); } //TODO
  static PMMNet Load(TSIn& SIn) { return PMMNet(new TMMNet(SIn)); }
  static PMMNet New() { return PMMNet(new TMMNet()); }

  int GetModeId(const TStr& ModeName) const { return ModeNameToIdH.GetDat(ModeName);  }
  TStr GetModeName(const TInt& ModeId) const { return ModeIdToNameH.GetDat(ModeId); }
  int GetLinkId(const TStr& LinkName) const { return LinkNameToIdH.GetDat(LinkName);  }
  TStr GetLinkName(const TInt& LinkId) const { return LinkIdToNameH.GetDat(LinkId); }

  PModeNet GetModeNet(const TStr& ModeName) const;
  PModeNet GetModeNet(const TInt& ModeId) const;

  PCrossNet GetCrossNet(const TStr& LinkName) const;
  PCrossNet GetCrossNet(const TInt& LinkId) const;

private:
  TIntPr GetOrderedLinkPair(const TStr& Mode1, const TStr& Mode2);
  TIntPr GetOrderedLinkPair(const TInt& Mode1, const TInt& Mode2);
  int AddEdge(const TStr& LinkTypeName, int& NId1, int& NId2, int EId=-1);
  int AddEdge(const TInt& LinkTypeId, int& NId1, int& NId2, int EId=-1);
  int DelEdge(const TStr& LinkTypeName, const TInt& EId);
  int DelEdge(const TInt& LinkTypeId, const TInt& EId);
};

// set flags
namespace TSnap {
template <> struct IsMultiGraph<TModeNet> { enum { Val = 1 }; };
template <> struct IsDirected<TModeNet> { enum { Val = 1 }; };
}
#endif // NETWORKMM_H
