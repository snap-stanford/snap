#ifndef NETWORKMM_H
#define NETWORKMM_H


class TMMNet;


typedef TPt<TMMNet> PMMNet;

//NOTE: Removed inheritance from nodes and edges (for now)
//TODO: Renaming
//TODO: Change Type everywhere to Mode
//TODO: TMultiLink with the entire hash table

//A single mode in a multimodal directed attributed multigraph
class TNEANetMM;

/// Pointer to a directed attribute multigraph (TNEANet)
typedef TPt<TNEANetMM> PNEANetMM;

//#//////////////////////////////////////////////
/// Directed multigraph with node edge attributes. ##TNEANet::Class
class TNEANetMM : TNEANet {
public:
  typedef TNEANetMM TNetMM;
  typedef TPt<TNEANetMM> PNetMM;
private:
//  TVec<TVec<TIntV> > OutEdgeV;
//  TVec<TVec<TIntV> > InEdgeV;
  TInt NTypeId;
  THash<TStr, TInt> EdgeTypeToId;
public:
  //TODO: Update constructors with information from fields above.
  TNEANetMM() : TNEANet(), TypeEdgeH(), OutEdgeV(), InEdgeV(), MxTypeEId(0), NTypeId(-1) { }
  TNEANetMM(const int& TypeId) : TNEANet(), TypeEdgeH(), OutEdgeV(), InEdgeV(), MxTypeEId(0), NTypeId(TypeId) { }
  /// Constructor that reserves enough memory for a graph of nodes and edges.
  explicit TNEANetMM(const int& Nodes, const int& Edges) : TNEANet(Nodes, Edges),
    TypeEdgeH(), OutEdgeV(), InEdgeV(), MxTypeEId(0), NTypeId(-1) { }
  explicit TNEANetMM(const int& Nodes, const int& Edges, const int& TypeId) : TNEANet(Nodes, Edges),
    TypeEdgeH(), OutEdgeV(), InEdgeV(), MxTypeEId(0), NTypeId(TypeId) { }
  TNEANetMM(const TNEANetMM& Graph) :  TNEANet(Graph), TypeEdgeH(Graph.TypeEdgeH),
    OutEdgeV(Graph.OutEdgeV), InEdgeV(Graph.InEdgeV), MxTypeEId(Graph.MxTypeEId), NTypeId(Graph.NTypeId) { }
  /// Constructor for loading the graph from a (binary) stream SIn.
  TNEANetMM(TSIn& SIn) : TNEANet(SIn), TypeEdgeH(SIn), OutEdgeV(SIn), InEdgeV(SIn), MxTypeEId(SIn), NTypeId(SIn) { }

  //Make these functions virtual

  /// Saves the graph to a (binary) stream SOut.
  void Save(TSOut& SOut) const {
    TNEANet::Save(SOut); TypeEdgeH.Save(SOut); OutEdgeV.Save(SOut);
    InEdgeV.Save(SOut); MxTypeEId.Save(SOut); NTypeId.Save(SOut); }
  /// Static cons returns pointer to graph. Ex: PNEANet Graph=TNEANet::New().
  static PNEANetMM New() { return PNEANetMM(new TNEANetMM()); }
  /// Static constructor that returns a pointer to the graph and reserves enough memory for Nodes nodes and Edges edges. ##TNEANet::New
  static PNEANetMM New(const int& Nodes, const int& Edges) { return PNEANetMM(new TNEANetMM(Nodes, Edges)); }
  /// Static constructor that loads the graph from a stream SIn and returns a pointer to it.
  static PNEANetMM Load(TSIn& SIn) { return PNEANetMM(new TNEANetMM(SIn)); }



  /// Returns an ID that is larger than any edge ID in the network.
  int GetMxTypeEId() const { return MxTypeEId; }

  /// Adds an edge with ID EId between node IDs SrcNId and DstNId to the graph. ##TNEANet::AddEdge
  int AddEdge(const int& SrcNId, const int& SrcNTypeId, const int& DstNId, const int& DstNTypeId, int EId=-1);
  /// Deletes an edge with edge ID EId from the graph.
  void DelEdge(const int& EId, const TStr& EdgeType);
  /// Deletes all edges between node IDs SrcNId and DstNId from the graph. ##TNEANet::DelEdge
  void DelEdge(const int& SrcNId, const int& SrcNTypeId, const int& DstNId, const int& DstNTypeId);
  /// Tests whether an edge with edge ID EId exists in the graph.
  //bool IsEdge(const int& EId) const { return EdgeH.IsKey(EId); }
  /// Tests whether an edge between node IDs SrcNId and DstNId exists in the graph.
  //bool IsEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true) const { int EId; return IsEdge(SrcNId, DstNId, EId, IsDir); }
  /// Tests whether an edge between node IDs SrcNId and DstNId exists in the graph. if an edge exists, return its edge ID in EId
  //bool IsEdge(const int& SrcNId, const int& DstNId, int& EId, const bool& IsDir = true) const;
  /// Returns an edge ID between node IDs SrcNId and DstNId, if such an edge exists. Otherwise, return -1.
  //int GetEId(const int& SrcNId, const int& DstNId) const { int EId; return IsEdge(SrcNId, DstNId, EId)?EId:-1; }
  /// Returns an iterator referring to the first edge in the graph.
  //TEdgeI BegEI() const { return TEdgeI(EdgeH.BegI(), this); }
  /// Returns an iterator referring to the past-the-end edge in the graph.
  //TEdgeI EndEI() const { return TEdgeI(EdgeH.EndI(), this); }
  /// Returns an iterator referring to edge with edge ID EId.
  //TEdgeI GetEI(const int& EId) const { return TEdgeI(EdgeH.GetI(EId), this); }
  /// Returns an iterator referring to edge (SrcNId, DstNId) in the graph.
  //TEdgeI GetEI(const int& SrcNId, const int& DstNId) const { return GetEI(GetEId(SrcNId, DstNId)); }

  /// Gets a vector IDs of all edges in the graph.
  //void GetEIdV(TIntV& EIdV) const;

  /// Deletes all nodes and edges from the graph.
  void Clr() { TNEANet::Clr(); TypeEdgeH.Clr(); OutEdgeV.Clr(); InEdgeV.Clr(); MxTypeEId = 0; NTypeId = -1;}

  /// Returns a small multigraph on 5 nodes and 6 edges. ##TNEANet::GetSmallGraph
  static PNEANetMM GetSmallGraph();
  friend class TPt<TNEANetMM>;
private:
  int AddNeighbor(const int& SrcNId, const int& EId, const TStr& EdgeType);
  int DelNeighbor(const int& SrcNId, const int& EId, const TStr& EdgeType);

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
  TVec<PNEANetMM> PNEANetMMV; //The vector of TNEANetMM's this contains. TODO: Decide whether this is vec or hash
  TVec<TMultiLink> TMultiLinkV;

  THash<TInt,TStr> ModeIdToNameH;
  THash<TStr,TInt> ModeNameToIdH;

  THash<TInt,TStr> LinkIdToNameH;
  THash<TStr,TInt> LinkNameToIdH;

  THash<TIntPr, TStr> LinkPrToNameH;
  THash<TStr, TIntPr> LinkNameToPrH;

public:
  TMMNet() : MxModeId(0), TNEANetMMV() {}
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

private:
  int AddEdge(const TInt& SrcNModeId, const TInt& DstNModeId, const TInt& SrcNId, const TInt& DstNId, TInt EId=-1); //TODO: Implement. Decide order of arguments.
  int AddEdge(int& NId, int& OtherTypeNId, bool& direction, TStr& LinkTypeName, TInt& EId=-1);

};

// set flags
namespace TSnap {
template <> struct IsMultiGraph<TNEANetMM> { enum { Val = 1 }; };
template <> struct IsDirected<TNEANetMM> { enum { Val = 1 }; };
}
#endif // NETWORKMM_H
