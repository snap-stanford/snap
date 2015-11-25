#ifndef NETWORKMM_H
#define NETWORKMM_H


class TMMNet;


typedef TPt<TMMNet> PMMNet;

//NOTE: Removed inheritance from nodes and edges (for now)
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
private:
  TInt NModeId;
  PMMNet MMNet; //the parent MMNet
  THashSet<TStr> NeighborTypes; //TODO: Is this necessary?

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


private:
  //method to add neighbors; will be called by TMMNet AddEdge function; outEdge == true iff NId(which is of the type of the TModeNet; i.e. it should refer to a node in this graph) is the source node.
  int AddNeighbor(const int& NId, const int& EId, const bool outEdge, const int linkId);
  int AddNeighbor(const int& NId, const int& EId, const bool outEdge, const TStr& linkName);
  int DelNeighbor(const int& NId, const int& EId, bool outEdge, const TStr& LinkName);
  int DelNeighbor(const int& NId, const int& EId, bool outEdge, const TInt& linkId);
  TStr GetNeighborLinkName(const TStr& LinkName, bool isOutEdge);
  void SetParentPointer( PMMNet parent);
public:

  ///When we create a new link type, we need to add a new neighbor type here.
  int AddNbrType(const TStr& LinkName);

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
  TCrossNet() : LinkH(), MxEId(0), Mode1(), Mode2(), LinkTypeId() {}
  TCrossNet(TInt MId1, TInt MId2, TInt LId) : LinkH(), MxEId(0), Mode1(MId1), Mode2(MId2), LinkTypeId(LId) {}
private:
  void SetParentPointer(PMMNet& parent);
public:
  //TODO: DelEdge or DelLink? Same with Add
  int AddEdge(const int& sourceNId, const int& destNId, const int& EId=-1);
  TCrossEdgeI GetEdgeI(const int& EId) const { return TCrossEdgeI(LinkH.GetI(EId), this); }
  TCrossEdgeI BegEdgeI() const { return TCrossEdgeI(LinkH.BegI(), this); }
  TCrossEdgeI EndEdgeI() const { return TCrossEdgeI(LinkH.EndI(), this); }
  int DelEdge(const int& EId);
  int GetMode1() const { return Mode1; }
  int GetMode2() const {return Mode2; }
  friend class TMMNet;
};

//The container class for a multimodal network

class TMMNet {


private:

  TInt MxModeId; //The number of modes
  TInt MxLinkTypeId;
  TVec<TModeNet> TModeNetV;
  THash<TInt, TCrossNet> TCrossNetH;

  THash<TInt,TStr> ModeIdToNameH;
  THash<TStr,TInt> ModeNameToIdH;

  THash<TInt,TStr> LinkIdToNameH;
  THash<TStr,TInt> LinkNameToIdH;

public:
  TCRef CRef; //Reference counter. Necessary for pointers.
  friend class TCrossNet;

public:
  TMMNet() : MxModeId(0), MxLinkTypeId(0), TModeNetV() {}
  TMMNet(const TMMNet& OtherTMMNet) : MxModeId(OtherTMMNet.MxModeId), MxLinkTypeId(OtherTMMNet.MxLinkTypeId), TModeNetV(OtherTMMNet.TModeNetV) {}
  TMMNet(TSIn& SIn) : MxModeId(SIn), MxLinkTypeId(SIn) {} //TODO
  int AddMode(const TStr& ModeName);
  int DelMode(const TInt& ModeId); // TODO(sramas15): finish implementing
  int DelMode(const TStr& ModeName);
  int AddLinkType(const TStr& ModeName1, const TStr& ModeName2, const TStr& LinkTypeName);
  int AddLinkType(const TInt& ModeId1, const TInt& ModeId2, const TStr& LinkTypeName);
  int DelLinkType(const TInt& LinkTypeId); // TODO(sramas15): finish implementing
  int DelLinkType(const TStr& LinkType);

  void Save(TSOut& SOut) const {TModeNetV.Save(SOut);} //TODO
  static PMMNet Load(TSIn& SIn) { return PMMNet(new TMMNet(SIn)); }

  //TODO: Add IAssertRs
  int GetModeId(const TStr& ModeName) const { return ModeNameToIdH.GetDat(ModeName);  }//TODO: Return type int or TInt?
  TStr GetModeName(const TInt& ModeId) const { return ModeIdToNameH.GetDat(ModeId); }
  int GetLinkId(const TStr& LinkName) const { return LinkNameToIdH.GetDat(LinkName);  }//TODO: Return type int or TInt?
  TStr GetLinkName(const TInt& LinkId) const { return LinkIdToNameH.GetDat(LinkId); }

  TModeNet GetTModeNet(const TStr& ModeName) const;
  TModeNet GetTModeNet(const TInt& ModeId) const;

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
