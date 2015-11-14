#ifndef NETWORKMM_H
#define NETWORKMM_H


class TMMNet;


typedef TPt<TMMNet> PMMNet;

//NOTE: Removed inheritance from nodes and edges (for now)
//TODO: Renaming
//TODO: Change Type everywhere to Mode
//TODO: TMultiLink with the entire hash table

//A single mode in a multimodal directed attributed multigraph
class TMultiNet;

/// Pointer to a directed attribute multigraph (TNEANet)
typedef TPt<TMultiNet> PMultiNet;

//#//////////////////////////////////////////////
/// Directed multigraph with node edge attributes. ##TNEANet::Class
class TMultiNet : TNEANet {
public:
  typedef TMultiNet TNetMM;
  typedef TPt<TMultiNet> PNetMM;
public:
  class TNodeMM  {
  private:
    TInt Id;
    TIntV InEIdV, OutEIdV;
  public:
    TNodeMM() : Id(-1), InEIdV(), OutEIdV() { }
    TNodeMM(const int& NId) : Id(NId), InEIdV(), OutEIdV() { }
    TNodeMM(const TNode& Node) : Id(Node.Id), InEIdV(Node.InEIdV), OutEIdV(Node.OutEIdV) { }
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
    friend class TMultiNet;
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
    friend class TMultiNet;
  };

private:
  TInt NTypeId;
  PMMNet MMNet; //the parent MMNet
  THashSet<TStr> NeighborTypes;

  TStr GetNeighborLinkName(TStr& LinkName, bool isOutEdge);
public:
  //TODO: Update constructors with information from fields above.
  TMultiNet() : TNEANet(), TypeEdgeH(), OutEdgeV(), InEdgeV(), MxTypeEId(0), NTypeId(-1) { }
  TMultiNet(const int& TypeId) : TNEANet(), TypeEdgeH(), OutEdgeV(), InEdgeV(), MxTypeEId(0), NTypeId(TypeId) { }
  /// Constructor that reserves enough memory for a graph of nodes and edges.
  explicit TMultiNet(const int& Nodes, const int& Edges) : TNEANet(Nodes, Edges),
    TypeEdgeH(), OutEdgeV(), InEdgeV(), MxTypeEId(0), NTypeId(-1) { }
  explicit TMultiNet(const int& Nodes, const int& Edges, const int& TypeId) : TNEANet(Nodes, Edges),
    TypeEdgeH(), OutEdgeV(), InEdgeV(), MxTypeEId(0), NTypeId(TypeId) { }
  TMultiNet(const TMultiNet& Graph) :  TNEANet(Graph), TypeEdgeH(Graph.TypeEdgeH),
    OutEdgeV(Graph.OutEdgeV), InEdgeV(Graph.InEdgeV), MxTypeEId(Graph.MxTypeEId), NTypeId(Graph.NTypeId) { }
  /// Constructor for loading the graph from a (binary) stream SIn.
  TMultiNet(TSIn& SIn) : TNEANet(SIn), TypeEdgeH(SIn), OutEdgeV(SIn), InEdgeV(SIn), MxTypeEId(SIn), NTypeId(SIn) { }

  //Make these functions virtual

  /// Saves the graph to a (binary) stream SOut.
  void Save(TSOut& SOut) const {
    TNEANet::Save(SOut); TypeEdgeH.Save(SOut); OutEdgeV.Save(SOut);
    InEdgeV.Save(SOut); MxTypeEId.Save(SOut); NTypeId.Save(SOut); }
  /// Static cons returns pointer to graph. Ex: PMultiNet Graph=TMultiGraph::New().
  static PMultiNet New() { return PMultiNet(new TMultiNet()); }
  /// Static constructor that returns a pointer to the graph and reserves enough memory for Nodes nodes and Edges edges. ##TNEANet::New
  static PMultiNet New(const int& Nodes, const int& Edges) { return PMultiNet(new TMultiNet(Nodes, Edges)); }
  /// Static constructor that loads the graph from a stream SIn and returns a pointer to it.
  static PMultiNet Load(TSIn& SIn) { return PMultiNet(new TMultiNet(SIn)); }

  int DelNode ( const int& NId);

  // Returns the number of edges in the graph.
  //int GetEdges() const { return EdgeH.Len(); }
  /// Adds an edge with ID EId between node IDs SrcNId and DstNId to the graph. ##TNEANet::AddEdge

  int AddEdge(const int& SrcNId, const int& DstNId, const int& OtherModeId, const bool& OtherIsSrc, const TInt& EId=-1);
  int AddEdge(const int& SrcNId, const int& DstNId, const TStr& OtherMode, const bool& OtherIsSrc, const TInt& EId=-1);
  int AddEdge(const int& CurrModeNId, const int& OtherModeNId, bool direction, const TStr& LinkTypeName, const TInt& EId=-1);
  int AddEdge(const int& CurrModeNId, const int& OtherModeNId, bool direction, const TInt& LinkTypeId, const TInt& EId=-1);

  int DelEdge(const TStr& LinkTypeName, const TInt& EId);


  //method to add neighbors; will be called by TMMNet AddEdge function; outEdge == true iff NId(which is of the type of the TMultiNet; i.e. it should refer to a node in this graph) is the source node.
  int AddNeighbor(const int& NId, const int& EId, const bool outEdge, const int linkId);
  int AddNeighbor(const int& NId, const int& EId, const bool outEdge, const TStr& linkName);
  int DelNeighbor(const int& NId, const int& EId, bool outEdge);

  /// Deletes all nodes and edges from the graph.
  void Clr() { TNEANet::Clr(); TypeEdgeH.Clr(); OutEdgeV.Clr(); InEdgeV.Clr(); MxTypeEId = 0; NTypeId = -1;}

  /// Returns a small multigraph on 5 nodes and 6 edges. ##TNEANet::GetSmallGraph
  static PMultiNet GetSmallGraph();
  friend class TPt<TMultiNet>;
};

class TMultiLink;

typedef TPt<TMultiLink> PMultiLink;

//A class for each link table
//TODO: Attributes: how?
//TODO: Name this as multi link table?
class TMultiLink {

public:
  class TMultiEdge {
  private:
    TInt EId;
    TInt SrcNId, DstNId;
    TBool direction;
  public:
    TMultiEdge() : EId(-1), SrcNId(-1), DstNId(-1), direction() { }
    TMultiEdge(const int& EId, const int& SourceNId, const int& DestNId, const bool& direc) :
      Id(EId), SrcNId(SourceNId), DstNId(DestNId), direction(direc) { }
    TMultiEdge(const TMultiEdge& MultiEdge) : EId(MultiEdge.EId), SrcNId(MultiEdge.SrcNId),
        DstNId(MultiEdge.DstNId), direction(MuliEdge.direction) { }
    TMultiEdge(TSIn& SIn) : EId(SIn), SrcNId(SIn), DstNId(SIn), direction(SIn) { }
    void Save(TSOut& SOut) const { EId.Save(SOut); SrcNId.Save(SOut); DstNId.Save(SOut); direction.Save(SOut); }
    int GetId() const { return EId; }
    int GetSrcNId() const { if (direction) { return SrcNId; } else { return DstNId; } }
    int GetDstNId() const { if (direction) { return DstNId; } else { return SrcNId; } }
    bool GetDirection() const { return direction; }
    friend class TMultiLink;
  };
    /// Edge iterator. Only forward iteration (operator++) is supported.
  class TMultiEdgeI {
  private:
    typedef THash<TInt, TMultiEdge>::TIter THashIter;
    THashIter EdgeHI;
    const TMultiLink *Graph;
  public:
    TEdgeI() : EdgeHI(), Graph(NULL) { }
    TEdgeI(const THashIter& EdgeHIter, const TMultiLink *GraphPt) : EdgeHI(EdgeHIter), Graph(GraphPt) { }
    TEdgeI(const TMultiEdgeI& EdgeI) : EdgeHI(EdgeI.EdgeHI), Graph(EdgeI.Graph) { }
    TEdgeI& operator = (const TMultiEdgeI& EdgeI) { if (this!=&EdgeI) { EdgeHI=EdgeI.EdgeHI; Graph=EdgeI.Graph; }  return *this; }
    /// Increment iterator.
    TEdgeI& operator++ (int) { EdgeHI++; return *this; }
    bool operator < (const TMultiEdgeI& EdgeI) const { return EdgeHI < EdgeI.EdgeHI; }
    bool operator == (const TMultiEdgeI& EdgeI) const { return EdgeHI == EdgeI.EdgeHI; }
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

    friend class TMultiLink;
  };

private:
  THash<TInt,TMultiEdge> LinkH;
  TInt MxEId;
  TInt Mode1;
  TInt Mode2;
  TInt LinkId;
  PMMnet Net;
  //Constructors
public:
  TMultiLink() : MxEId(-1), LinkH() {}
public:
  int AddLink (const int& sourceNId, const int& sourceNModeId, const int& destNId, const int& destNModeId, const int& EId=-1);
  int AddLink (const int& NIdType1, const int& NIdType2, const bool& direction, const int& EId=-1);
  TMultiEdgeI GetLinkI(const int& EId) const { return TMultiEdgeI(LinkH.GetI(EId), this); }
  TMultiEdgeI BegLinkI() const { return TMultiEdgeI(LinkH.BegI(), this); }
  TMultiEdgeI EndLinkI() const { return TMultiEdgeI(LinkH.EndI(), this); }
  int DelLink(const int& EId) const;
  int GetMode1() const { return Mode1; }
  int GetMode2() const { return Mode2; }
  friend class TMMNet;
};

//The container class for a multimodal network

class TMMNet {


private:
  TInt MxModeId; //The number of modes
  TInt MxLinkTypeId;
  TVec<PMultiNet> PMultiNetV; //The vector of TNEANetMM's this contains. TODO: Decide whether this is vec or hash
  THash<TInt, TMultiLink> TMultiLinkH;

  THash<TInt,TStr> ModeIdToNameH;
  THash<TStr,TInt> ModeNameToIdH;

  THash<TInt,TStr> LinkIdToNameH;
  THash<TStr,TInt> LinkNameToIdH;

public:
  TMMNet() : MxModeId(0), TMultiNetV() {}
  TMMNet(const TMMNet& OtherTMMNet) : MxModeId(OtherTMMNet.MxModeId), TNEANetMMV(OtherTMMNet.TNEANetMMV) {}
  int AddMode(const TStr& ModeName, TInt& ModeId); //TODO: Implement. Decide return type.
  int DelMode(const TInt& ModeId);
  int DelMode(const TStr& ModeName);
  int AddLinkType(const TStr& ModeName1, const TStr& ModeName2, const TStr& EdgeTypeName, TInt& EdgeTypeId);
  int AddLinkType(const TInt& ModeId1, const TInt& ModeId2, const TStr& EdgeTypeName, TInt& EdgeTypeId);
  int DelLinkType(const TInt& EdgeTypeId);



  //Interface
  int GetModeId(const TStr& ModeName) const { return ModeNameToIdH.GetDat(ModeName);  }//TODO: Return type int or TInt?
  TStr GetModeName(const TInt& ModeId) const { return ModeIdToNameH.GetDat(ModeId); }

  int GetLinkId(const TStr& LinkName) const { return LinkNameToIdH.GetDat(LinkName);  }//TODO: Return type int or TInt?
  TStr GetLinkName(const TInt& LinkId) const { return LinkIdToNameH.GetDat(LinkId); }

private:
  TIntPr GetOrderedLinkPair(const TStr& Mode1, const TStr& Mode2);
  TIntPr GetOrderedLinkPair(const TInt& Mode1, const TInt& Mode2);
  int AddEdge(int& NId, int& NId2, bool& direction, int& ModeId1, TStr& LinkTypeName, TInt& EId=-1);
  int AddEdge(int& NId, int& NId2, bool& direction, int& ModeId1, TInt& LinkTypeId, TInt& EId=-1);
};

// set flags
namespace TSnap {
template <> struct IsMultiGraph<TMultiNet> { enum { Val = 1 }; };
template <> struct IsDirected<TMultiNet> { enum { Val = 1 }; };
}
#endif // NETWORKMM_H
