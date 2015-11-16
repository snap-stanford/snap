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

//#//////////////////////////////////////////////
/// Directed multigraph with node edge attributes. ##TNEANet::Class
class TModeNet : TNEANet {
public:
  typedef TModeNet TNetMM;
  typedef TPt<TModeNet> PNetMM;
public:
  class TNodeMM  {
  private:
    TInt Id;
    TIntV InEIdV, OutEIdV;
  public:
    TNodeMM() : Id(-1), InEIdV(), OutEIdV() { }
    TNodeMM(const int& NId) : Id(NId), InEIdV(), OutEIdV() { }
    TNodeMM(const TNodeMM& Node) : Id(Node.Id), InEIdV(Node.InEIdV), OutEIdV(Node.OutEIdV) { }
    TNodeMM(TSIn& SIn) : Id(SIn), InEIdV(SIn), OutEIdV(SIn) { }
    void Save(TSOut& SOut) const { Id.Save(SOut); InEIdV.Save(SOut); OutEIdV.Save(SOut); }
    int GetId() const { return Id; }
    int GetDeg() const { return GetInDeg() + GetOutDeg(); } //TODO: Reimplement
    int GetInDeg() const { return InEIdV.Len(); }
    int GetOutDeg() const { return OutEIdV.Len(); }
    int GetInEId(const int& EdgeN) const { return InEIdV[EdgeN]; }
    int GetOutEId(const int& EdgeN) const { return OutEIdV[EdgeN]; }
    int GetNbrEId(const int& EdgeN) const { return EdgeN<GetOutDeg()?GetOutEId(EdgeN):GetInEId(EdgeN-GetOutDeg()); }
    bool IsInEId(const int& EId) const { return InEIdV.SearchBin(EId) != -1; }
    bool IsOutEId(const int& EId) const { return OutEIdV.SearchBin(EId) != -1; }
    friend class TModeNet;
  };
  class TEdgeMM {
  private:
    TInt Id, SrcNId, DstNId, SrcNTypeId, DstNTypeId;
  public:
    TEdgeMM() : Id(-1), SrcNId(-1), DstNId(-1), SrcNTypeId(-1), DstNTypeId(-1) { }
    TEdgeMM(const int& EId, const int& SourceNId, const int& DestNId, const int& SourceNTypeId, const int& DestNTypeId) : 
      Id(EId), SrcNId(SourceNId), DstNId(DestNId), SrcNTypeId(SourceNTypeId), DstNTypeId(DestNTypeId) { }
    TEdgeMM(const TEdgeMM& EdgeMM) : Id(EdgeMM.Id), SrcNId(EdgeMM.SrcNId), DstNId(EdgeMM.DstNId), SrcNTypeId(EdgeMM.SrcNTypeId), DstNTypeId(EdgeMM.DstNTypeId) { }
    TEdgeMM(TSIn& SIn) : Id(SIn), SrcNId(SIn), DstNId(SIn), SrcNTypeId(SIn), DstNTypeId(SIn) { }
    void Save(TSOut& SOut) const { Id.Save(SOut); SrcNId.Save(SOut); DstNId.Save(SOut); SrcNTypeId(-1).Save(SOut); DstNTypeId.Save(SOut); }
    int GetId() const { return Id; }
    int GetSrcNId() const { return SrcNId; }
    int GetDstNId() const { return DstNId; }
    int GetSrcNTypeId() const { return SrcNTypeId; }
    int GetDstNTypeId() const { return DstNTypeId; }
    friend class TModeNet;
  };

private:
  TInt NTypeId;
  PMMNet MMNet; //the parent MMNet
  THashSet<TStr> NeighborTypes;

public:
  //TODO: Update constructors with information from fields above.
  TModeNet() : TNEANet(), TypeEdgeH(), OutEdgeV(), InEdgeV(), MxTypeEId(0), NTypeId(-1) { }
  TModeNet(const int& TypeId) : TNEANet(), TypeEdgeH(), OutEdgeV(), InEdgeV(), MxTypeEId(0), NTypeId(TypeId) { }
  /// Constructor that reserves enough memory for a graph of nodes and edges.
  explicit TModeNet(const int& Nodes, const int& Edges) : TNEANet(Nodes, Edges),
    TypeEdgeH(), OutEdgeV(), InEdgeV(), MxTypeEId(0), NTypeId(-1) { }
  explicit TModeNet(const int& Nodes, const int& Edges, const int& TypeId) : TNEANet(Nodes, Edges),
    TypeEdgeH(), OutEdgeV(), InEdgeV(), MxTypeEId(0), NTypeId(TypeId) { }
  TModeNet(const TModeNet& Graph) :  TNEANet(Graph), TypeEdgeH(Graph.TypeEdgeH),
    OutEdgeV(Graph.OutEdgeV), InEdgeV(Graph.InEdgeV), MxTypeEId(Graph.MxTypeEId), NTypeId(Graph.NTypeId) { }
  /// Constructor for loading the graph from a (binary) stream SIn.
  TModeNet(TSIn& SIn) : TNEANet(SIn), TypeEdgeH(SIn), OutEdgeV(SIn), InEdgeV(SIn), MxTypeEId(SIn), NTypeId(SIn) { }

  //Make these functions virtual

  /// Saves the graph to a (binary) stream SOut.
  void Save(TSOut& SOut) const {
    TNEANet::Save(SOut); TypeEdgeH.Save(SOut); OutEdgeV.Save(SOut);
    InEdgeV.Save(SOut); MxTypeEId.Save(SOut); NTypeId.Save(SOut); }
  /// Static cons returns pointer to graph. Ex: PModeNet Graph=TMultiGraph::New().
  static PModeNet New() { return PModeNet(new TModeNet()); }
  /// Static constructor that returns a pointer to the graph and reserves enough memory for Nodes nodes and Edges edges. ##TNEANet::New
  static PModeNet New(const int& Nodes, const int& Edges) { return PModeNet(new TModeNet(Nodes, Edges)); }
  /// Static constructor that loads the graph from a stream SIn and returns a pointer to it.
  static PModeNet Load(TSIn& SIn) { return PModeNet(new TModeNet(SIn)); }

  int DelNode (const int& NId); //TODO(sramas15): finish implementing

  // Returns the number of edges in the graph.
  //int GetEdges() const { return EdgeH.Len(); }
  /// Adds an edge with ID EId between node IDs SrcNId and DstNId to the graph. ##TNEANet::AddEdge

  int AddEdge(const int& CurrModeNId, const int& OtherModeNId, bool direction, const TStr& LinkTypeName, const TInt& EId=-1);
  int AddEdge(const int& CurrModeNId, const int& OtherModeNId, bool direction, const TInt& LinkTypeId, const TInt& EId=-1);

  int DelEdge(const TStr& LinkTypeName, const TInt& EId);
  int DelEdge(const TInt& LinkTypeId, const TInt& EId);


private:
  //method to add neighbors; will be called by TMMNet AddEdge function; outEdge == true iff NId(which is of the type of the TModeNet; i.e. it should refer to a node in this graph) is the source node.
  int AddNeighbor(const int& NId, const int& EId, const bool outEdge, const int linkId);
  int AddNeighbor(const int& NId, const int& EId, const bool outEdge, const TStr& linkName);
  int DelNeighbor(const int& NId, const int& EId, bool outEdge, const TStr& LinkName);
  int DelNeighbor(const int& NId, const int& EId, bool outEdge, const TInt& linkId);
  TStr GetNeighborLinkName(const TStr& LinkName, bool isOutEdge);
public:

  /// Deletes all nodes and edges from the graph.
  void Clr() { TNEANet::Clr(); TypeEdgeH.Clr(); OutEdgeV.Clr(); InEdgeV.Clr(); MxTypeEId = 0; NTypeId = -1;}

  /// Returns a small multigraph on 5 nodes and 6 edges. ##TNEANet::GetSmallGraph
  static PModeNet GetSmallGraph();
  friend class TPt<TModeNet>;
};

class TCrossNet;

typedef TPt<TCrossNet> PCrossNet;

//A class for each link table
//TODO: Attributes: how?
class TCrossNet {

public:
  class TCrossEdge {
  private:
    TInt EId;
    TInt Mode1NId, Mode2NId;
    TBool direction;
  public:
    TCrossEdge() : EId(-1), Mode1NId(-1), Mode2NId(-1), direction() { }
    TCrossEdge(const int& Id, const int& M1NId, const int& M2NId, const bool& direc) :
      EId(Id), Mode1NId(M1NId), Mode2NId(M2NId), direction(direc) { }
    TCrossEdge(const TCrossEdge& MultiEdge) : EId(MultiEdge.EId), Mode1NId(MultiEdge.Mode1NId),
        Mode2NId(MultiEdge.Mode2NId), direction(MultiEdge.direction) { }
    TCrossEdge(TSIn& SIn) : EId(SIn), Mode1NId(SIn), Mode2NId(SIn), direction(SIn) { }
    void Save(TSOut& SOut) const { EId.Save(SOut); Mode1NId.Save(SOut); Mode2NId.Save(SOut); direction.Save(SOut); }
    int GetId() const { return EId; }
    int GetSrcNId() const { if (direction) { return Mode1NId; } else { return Mode2NId; } }
    int GetDstNId() const { if (direction) { return Mode2NId; } else { return Mode1NId; } }
    bool GetDirection() const { return direction; }
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
    int GetSrcModeId() const { if (EdgeHI.GetDat().GetDirection()) { return Graph->GetMode1(); } else { return Graph->GetMode2(); } }
    /// Returns the destination of the edge.
    int GetDstModeId() const { if (EdgeHI.GetDat().GetDirection()) { return Graph->GetMode2(); } else { return Graph->GetMode1(); } }

    friend class TCrossNet;
  };

private:
  THash<TInt,TCrossEdge> LinkH;
  TInt MxEId;
  TInt Mode1;
  TInt Mode2;
  TInt LinkId;
  PMMNet Net;
  //Constructors
public:
  TCrossNet() : LinkH(), MxEId(-1), Mode1(), Mode2(), LinkId() {}
  TCrossNet(TInt MId1, TInt MId2, TInt LId) : LinkH(), MxEId(-1), Mode1(MId1), Mode2(MId2), LinkId(LId) {}
private:
  int AddLink (const int& sourceNId, const int& sourceNModeId, const int& destNId, const int& EId=-1);
  int AddLink (const int& NIdType1, const int& NIdType2, const bool& direction, const int& EId=-1);
  int DelLink(const int& EId) const;
public:
  int AddEdge (const int& sourceNId, const int& sourceNModeId, const int& destNId, const int& destNModeId, const int& EId=-1);
  int AddEdge (const int& NIdType1, const int& NIdType2, const bool& direction, const int& EId=-1);
  TCrossEdgeI GetLinkI(const int& EId) const { return TCrossEdgeI(LinkH.GetI(EId), this); }
  TCrossEdgeI BegLinkI() const { return TCrossEdgeI(LinkH.BegI(), this); }
  TCrossEdgeI EndLinkI() const { return TCrossEdgeI(LinkH.EndI(), this); }
  int DelEdge(const int& EId);
  int GetMode1() const { return Mode1; }
  int GetMode2() const { return Mode2; }
  friend class TMMNet;
};

//The container class for a multimodal network

class TMMNet {


private:

  TInt MxModeId; //The number of modes
  TInt MxLinkTypeId;
  TVec<PModeNet> PModeNetV; //The vector of TNEANetMM's this contains. TODO: Decide whether this is vec or hash
  THash<TInt, TCrossNet> TCrossNetH;

  THash<TInt,TStr> ModeIdToNameH;
  THash<TStr,TInt> ModeNameToIdH;

  THash<TInt,TStr> LinkIdToNameH;
  THash<TStr,TInt> LinkNameToIdH;

public:
  TCRef CRef; //Reference counter. Necessary for pointers.

public:
  TMMNet() : MxModeId(0), PModeNetV() {}
  TMMNet(const TMMNet& OtherTMMNet) : MxModeId(OtherTMMNet.MxModeId), PModeNetV(OtherTMMNet.PModeNetV) {}
  int AddMode(const TStr& ModeName, TInt& ModeId);
  int DelMode(const TInt& ModeId); // TODO(sramas15): finish implementing
  int DelMode(const TStr& ModeName);
  int AddLinkType(const TStr& ModeName1, const TStr& ModeName2, const TStr& EdgeTypeName, TInt& EdgeTypeId);
  int AddLinkType(const TInt& ModeId1, const TInt& ModeId2, const TStr& EdgeTypeName, TInt& EdgeTypeId);
  int DelLinkType(const TInt& EdgeTypeId); // TODO(sramas15): finish implementing
  int DelLinkType(const TStr& EdgeType);

  int GetModeId(const TStr& ModeName) const { return ModeNameToIdH.GetDat(ModeName);  }//TODO: Return type int or TInt?
  TStr GetModeName(const TInt& ModeId) const { return ModeIdToNameH.GetDat(ModeId); }
  int GetLinkId(const TStr& LinkName) const { return LinkNameToIdH.GetDat(LinkName);  }//TODO: Return type int or TInt?
  TStr GetLinkName(const TInt& LinkId) const { return LinkIdToNameH.GetDat(LinkId); }

private:
  TIntPr GetOrderedLinkPair(const TStr& Mode1, const TStr& Mode2);
  TIntPr GetOrderedLinkPair(const TInt& Mode1, const TInt& Mode2);
  int AddEdge(int& NId, int& NId2, bool& direction, int& ModeId1, TStr& LinkTypeName, TInt& EId=-1);
  int AddEdge(int& NId, int& NId2, bool& direction, int& ModeId1, TInt& LinkTypeId, TInt& EId=-1);
  int DelEdge(const TInt& LinkTypeId, const TInt& EId);
};

// set flags
namespace TSnap {
template <> struct IsMultiGraph<TModeNet> { enum { Val = 1 }; };
template <> struct IsDirected<TModeNet> { enum { Val = 1 }; };
}
#endif // NETWORKMM_H
