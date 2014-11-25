#ifndef MGRAPH_H_
#define MGRAPH_H_

template<class TNode>
class TMNet;

class TMVNode {
private:
  TInt TypeId; // Node type ID
  TInt Id; // Get global ID
  TIntV InEIdV, OutEIdV;

public:
  TMVNode() : TypeId(-1), Id(-1), InEIdV(), OutEIdV(){ }
  TMVNode(const int& NTypeId, const int& NId) : TypeId(NTypeId), Id(NId), InEIdV(), OutEIdV() { }
  TMVNode(const TMVNode& Node) : TypeId(Node.TypeId), Id(Node.Id), InEIdV(Node.InEIdV), OutEIdV(Node.OutEIdV) { }
  TMVNode(TSIn& SIn) : TypeId(SIn), Id(SIn), InEIdV(SIn), OutEIdV(SIn) { }
  void Save(TSOut& SOut) const { TypeId.Save(SOut); Id.Save(SOut); InEIdV.Save(SOut); OutEIdV.Save(SOut); }
  int GetTypeId() const { return TypeId; }
  int GetId() const { return Id; }
  int GetDeg() const { return GetInDeg() + GetOutDeg(); }
  int GetInDeg() const { return InEIdV.Len(); }
  int GetOutDeg() const { return OutEIdV.Len(); }
  int GetInEId(const int& EdgeN) const { return InEIdV[EdgeN]; }
  int GetOutEId(const int& EdgeN) const { return OutEIdV[EdgeN]; }
  int GetNbrEId(const int& EdgeN) const { return EdgeN<GetOutDeg()?GetOutEId(EdgeN):GetInEId(EdgeN-GetOutDeg()); }
  bool IsInEId(const int& EId) const { return InEIdV.SearchBin(EId) != -1; }
  bool IsOutEId(const int& EId) const { return OutEIdV.SearchBin(EId) != -1; }
  void AddInETypeIds(const TIntV& ETypeIds) { } // Do nothing.
  void AddOutETypeIds(const TIntV& ETypeIds) { } // Do nothing.
  void AddInNbr(const int& ETypeId, const int& EId) {
    InEIdV.AddSorted(EId);
  }
  void AddOutNbr(const int& ETypeId, const int& EId) {
    OutEIdV.AddSorted(EId);
  }
  friend class TMNet<TMVNode>;
};

class TCVNode {
public:
  static const int DEF_WEIGHT;
  static const int DEF_WEIGHT_COEFF;
  static const int DEF_EXPAND_RATIO;
private:
  static void RedistributeEIds(const TIntV& Weights, TIntV& EIdV, TIntV& TypeIndexV, TIntV& TypeDegV) {
    IAssertR(TypeIndexV.Len() == TypeDegV.Len(), TStr::Fmt("The node is in inconsistent state."));
    // Get new TypeIndex
    int NTypes = Weights.Len();
    TIntV NewTypeIndexV(NTypes); // number of types
    int CumSum = 0; // cumulative sum of weights
    for (int ETypeId = 0; ETypeId < NTypes; ETypeId++) {
      NewTypeIndexV[ETypeId] = CumSum;
      CumSum += Weights[ETypeId] * DEF_WEIGHT_COEFF;
    }
    TIntV NewEIdV(CumSum);
    // Copy data from old positions to new positions
    for (int ETypeId = TypeIndexV.Len() - 1; ETypeId >= 0; ETypeId--) {
      IAssertR(CumSum >= NewTypeIndexV[ETypeId] + TypeDegV[ETypeId], TStr::Fmt("The node is in inconsistent state."));
      for (int i = 0; i < TypeDegV[ETypeId]; i++) {
        NewEIdV[NewTypeIndexV[ETypeId] + i] = EIdV[TypeIndexV[ETypeId] + i];
      }
    }
    TypeDegV.Reserve(NTypes, NTypes);
    TypeIndexV = NewTypeIndexV;
    EIdV = NewEIdV;
  }
private:
  TInt TypeId; // Node type ID
  TInt Id; // Get global ID
  TIntV InEIdV, OutEIdV;
  TInt InDeg, OutDeg;
  TIntV InTypeIndexV, OutTypeIndexV;
  TIntV InTypeDegV, OutTypeDegV;
public:
  TCVNode() : TypeId(-1), Id(-1), InEIdV(), OutEIdV(), InDeg(0), OutDeg(0),
                                    InTypeIndexV(), OutTypeIndexV(), InTypeDegV(), OutTypeDegV() { }
  TCVNode(const int& NTypeId, const int& NId) : TypeId(NTypeId), Id(NId), InEIdV(), OutEIdV(), InDeg(0), OutDeg(0),
                                    InTypeIndexV(), OutTypeIndexV(), InTypeDegV(), OutTypeDegV() { }
  TCVNode(const TCVNode& Node) : TypeId(Node.TypeId), Id(Node.Id), InEIdV(Node.InEIdV), OutEIdV(Node.OutEIdV),
                                    InDeg(Node.InDeg), OutDeg(Node.OutDeg), InTypeIndexV(Node.InTypeIndexV), OutTypeIndexV(Node.OutTypeIndexV),
                                    InTypeDegV(Node.InTypeDegV), OutTypeDegV(Node.OutTypeDegV) { }
  TCVNode(TSIn& SIn) : TypeId(SIn), Id(SIn), InEIdV(SIn), OutEIdV(SIn), InDeg(SIn), OutDeg(SIn),
                                    InTypeIndexV(SIn), OutTypeIndexV(SIn), InTypeDegV(SIn), OutTypeDegV(SIn) { }
  void Save(TSOut& SOut) const { TypeId.Save(SOut); Id.Save(SOut); InEIdV.Save(SOut); OutEIdV.Save(SOut);
                                    InDeg.Save(SOut); OutDeg.Save(SOut); InTypeIndexV.Save(SOut); OutTypeIndexV.Save(SOut);
                                    InTypeDegV.Save(SOut); OutTypeDegV.Save(SOut); }
  int GetTypeId() const { return TypeId; }
  int GetId() const { return Id; }
  int GetDeg() const { return InDeg + OutDeg; }
  int GetInDeg() const { return InDeg; }
  int GetOutDeg() const { return OutDeg; }
  int GetInDeg(const int& ETypeId) const { return InTypeDegV[ETypeId]; }
  int GetOutDeg(const int& ETypeId) const { return OutTypeDegV[ETypeId]; }
  int GetInEId(const int& EdgeN) const {
    int CumSum = 0;
    int ETypeId = 0;
    for (; ETypeId < InTypeDegV.Len(); ETypeId++) {
      CumSum += InTypeDegV[ETypeId];
      if (CumSum > EdgeN) { CumSum -= InTypeDegV[ETypeId]; break; }
    }
    return InEIdV[InTypeIndexV[ETypeId] + EdgeN - CumSum];
  }
  int GetOutEId(const int& EdgeN) const {
    int CumSum = 0;
    int ETypeId = 0;
    for (; ETypeId < OutTypeDegV.Len(); ETypeId++) {
      CumSum += OutTypeDegV[ETypeId];
      if (CumSum > EdgeN) { CumSum -= OutTypeDegV[ETypeId]; break; }
    }
    return OutEIdV[OutTypeIndexV[ETypeId] + EdgeN - CumSum];
  }
  int GetNbrEId(const int& EdgeN) const { return EdgeN<GetOutDeg()?GetOutEId(EdgeN):GetInEId(EdgeN-GetOutDeg()); }
  //bool IsInEId(const int& EId) const { return InEIdV.SearchBin(EId) != -1; }
  //bool IsOutEId(const int& EId) const { return OutEIdV.SearchBin(EId) != -1; }
  void AddInETypeIds(const TIntV& ETypeIds) {
    if (ETypeIds.Len() == 0) { return; }

    int MxETypeId = InTypeIndexV.Len() - 1;
    for (TIntV::TIter iter = ETypeIds.BegI(); iter < ETypeIds.EndI(); iter++) {
      if (MxETypeId < *iter) { MxETypeId = *iter; }
    }
    TIntV InWeights(MxETypeId + 1);
    for (int ETypeId = 0; ETypeId < InTypeDegV.Len(); ETypeId++) {
      InWeights[ETypeId] = InTypeDegV[ETypeId];
    }
    for (TIntV::TIter iter = ETypeIds.BegI(); iter < ETypeIds.EndI(); iter++) {
      InWeights[*iter] = DEF_WEIGHT;
    }
    RedistributeEIds(InWeights, InEIdV, InTypeIndexV, InTypeDegV);
  }
  void AddOutETypeIds(const TIntV& ETypeIds) {
    if (ETypeIds.Len() == 0) { return; }

    int MxETypeId = OutTypeIndexV.Len() - 1;
    for (TIntV::TIter iter = ETypeIds.BegI(); iter < ETypeIds.EndI(); iter++) {
      if (MxETypeId < *iter) { MxETypeId = *iter; }
    }
    TIntV OutWeights(MxETypeId + 1);
    for (int ETypeId = 0; ETypeId < OutTypeDegV.Len(); ETypeId++) {
      OutWeights[ETypeId] = OutTypeDegV[ETypeId];
    }
    for (TIntV::TIter iter = ETypeIds.BegI(); iter < ETypeIds.EndI(); iter++) {
      OutWeights[*iter] = DEF_WEIGHT;
    }
    RedistributeEIds(OutWeights, OutEIdV, OutTypeIndexV, OutTypeDegV);
  }
  void AddInNbr(const int& ETypeId, const int& EId) {
    int Deg = InTypeDegV[ETypeId];
    int Capacity = (ETypeId == (InTypeIndexV.Len()-1)) ? InEIdV.Len() : InTypeIndexV[ETypeId+1].Val;
    Capacity -= InTypeIndexV[ETypeId];
    if (Deg >= Capacity) {
      IAssertR(Deg == Capacity, TStr::Fmt("The node is in inconsistent state."));
      TIntV Weights(InTypeDegV);
      Weights[ETypeId] = (Weights[ETypeId] + 4) * DEF_EXPAND_RATIO;
      RedistributeEIds(Weights, InEIdV, InTypeIndexV, InTypeDegV);
    }
    InEIdV[InTypeIndexV[ETypeId] + Deg] = EId;
    InTypeDegV[ETypeId]++;
    InDeg++;
  }
  void AddOutNbr(const int& ETypeId, const int& EId) {
    int Deg = OutTypeDegV[ETypeId];
    int Capacity = (ETypeId == (OutTypeIndexV.Len()-1)) ? OutEIdV.Len() : OutTypeIndexV[ETypeId+1].Val;
    Capacity -= OutTypeIndexV[ETypeId];
    if (Deg >= Capacity) {
      IAssertR(Deg == Capacity, TStr::Fmt("The node is in inconsistent state."));
      TIntV Weights(OutTypeDegV);
      Weights[ETypeId] = (Weights[ETypeId] + 4) * DEF_EXPAND_RATIO; // + 4 to avoid 0
      RedistributeEIds(Weights, OutEIdV, OutTypeIndexV, OutTypeDegV);
    }
    OutEIdV[OutTypeIndexV[ETypeId] + Deg] = EId;
    OutTypeDegV[ETypeId]++;
    OutDeg++;
  }
  friend class TMNet<TCVNode>;
};

//class TMNet;
///// Pointer
//typedef TPt<TMNet> PMVNet;

//#//////////////////////////////////////////////
/// Directed multigraph with node edge attributes. ##TNEANet::Class
template<class TNode>
class TMNet {
public:
  typedef TMNet TNet;
  typedef TPt<TMNet> PNet;
public:
  class TEdge {
  private:
    TInt TypeId, Id, SrcNId, DstNId;
  public:
    TEdge() : TypeId(-1), Id(-1), SrcNId(-1), DstNId(-1) { }
    TEdge(const int& ETypeId, const int& EId, const int& SourceNId, const int& DestNId) : TypeId(ETypeId), Id(EId), SrcNId(SourceNId), DstNId(DestNId) { }
    TEdge(const TEdge& Edge) : TypeId(Edge.TypeId), Id(Edge.Id), SrcNId(Edge.SrcNId), DstNId(Edge.DstNId) { }
    TEdge(TSIn& SIn) : TypeId(SIn), Id(SIn), SrcNId(SIn), DstNId(SIn) { }
    void Save(TSOut& SOut) const { TypeId.Save(SOut), Id.Save(SOut); SrcNId.Save(SOut); DstNId.Save(SOut); }
    int GetTypeId() const { return TypeId; }
    int GetId() const { return Id; }
    int GetSrcNId() const { return SrcNId; }
    int GetDstNId() const { return DstNId; }
    friend class TMNet;
  };

  class TNodeType {
  private:
    TInt Id;
    TStr Name;
    TInt MxNId;
    THash<TInt, TNode> NodeH;
  public:
    TNodeType() : Id(-1), Name(), MxNId(0), NodeH(){ }
    TNodeType(const int& NTypeId, const TStr& NTypeName) : Id(NTypeId), Name(NTypeName), MxNId(0), NodeH(){ }
    TNodeType(const TNodeType& NodeType) : Id(NodeType.Id), Name(NodeType.Name), MxNId(NodeType.MxNId), NodeH(NodeType.NodeH) { }
    TNodeType(TSIn& SIn) : Id(SIn), Name(SIn), MxNId(SIn), NodeH(SIn) { }
    void Save(TSOut& SOut) const { Id.Save(SOut); Name.Save(SOut); MxNId.Save(SOut); NodeH.Save(SOut); }
    int GetId() const { return Id; }
    TStr GetName() const { return Name; }
    int GetMxNId() const { return MxNId; }
    friend class TMNet;
  };

  /// Node iterator. Only forward iteration (operator++) is supported.
  template<class TEdge>
  class TMNodeI {
  private:
    typedef typename THash<TInt, TNode>::TIter THashIter;
    typedef typename TVec<TNodeType>::TIter TTypeIter;
    TTypeIter VecI;
    TTypeIter VecEndI;
    THashIter HashI;
    const TMNet *Graph;
  private:
    THashIter VecElemBegI() {
      return (*VecI).NodeH.BegI();
    }
    void FindNextNonEmptyHashI() {
      while (HashI.IsEnd() && VecI < VecEndI) {
        VecI++;
        HashI = VecElemBegI();
      }
    }
  public:
    TMNodeI() : VecI(), VecEndI(), HashI(), Graph(NULL) { }
    TMNodeI(const TTypeIter& TypeIter, const THashIter& NodeIter, const TMNet* GraphPt)
              : VecI(TypeIter), VecEndI(GraphPt->TypeNodeV.EndI()), HashI(NodeIter), Graph(GraphPt) { }
    TMNodeI(const TTypeIter& TypeIter, const TMNet* GraphPt) : VecI(TypeIter), VecEndI(GraphPt->TypeNodeV.EndI()), Graph(GraphPt) {
      if (VecI < VecEndI) {
        HashI = VecElemBegI();
        FindNextNonEmptyHashI();
      } else {
        HashI = THashIter();
      }
    }
    TMNodeI(const TMNodeI& NodeI) : VecI(NodeI.VecI), VecEndI(NodeI.VecEndI), HashI(NodeI.HashI), Graph(NodeI.Graph) { }
    TMNodeI& operator = (const TMNodeI& NodeI) { VecI=NodeI.VecI; VecEndI=NodeI.VecEndI; HashI=NodeI.HashI; Graph=NodeI.Graph; return *this; }
    /// Increment iterator.
    TMNodeI& operator++ (int) {
      HashI++;
      FindNextNonEmptyHashI();
      return *this;
    }
    bool operator < (const TMNodeI& NodeI) const { return VecI < NodeI.VecI || HashI < NodeI.HashI; }
    bool operator == (const TMNodeI& NodeI) const { return VecI == NodeI.VecI && HashI == NodeI.HashI; }

    /// Returns ID of the current node.
    int GetId() const { return HashI.GetDat().GetId(); }
    /// Returns type ID of the current node.
    int GetTypeId() const { return HashI.GetDat().GetTypeId(); }
    /// Returns degree of the current node, the sum of in-degree and out-degree.
    int GetDeg() const { return HashI.GetDat().GetDeg(); }
    /// Returns in-degree of the current node.
    int GetInDeg() const { return HashI.GetDat().GetInDeg(); }
    /// Returns out-degree of the current node.
    int GetOutDeg() const { return HashI.GetDat().GetOutDeg(); }
    /// Returns ID of EdgeN-th in-node (the node pointing to the current node). ##TNEANet::TNodeI::GetInNId
    int GetInNId(const int& EdgeN) const { return Graph->GetEdge(HashI.GetDat().GetInEId(EdgeN)).GetSrcNId(); }
    /// Returns ID of EdgeN-th out-node (the node the current node points to). ##TNEANet::TNodeI::GetOutNId
    int GetOutNId(const int& EdgeN) const { return Graph->GetEdge(HashI.GetDat().GetOutEId(EdgeN)).GetDstNId(); }
    /// Returns ID of EdgeN-th neighboring node. ##TNEANet::TNodeI::GetNbrNId
    int GetNbrNId(const int& EdgeN) const { const TEdge& E = Graph->GetEdge(HashI.GetDat().GetNbrEId(EdgeN)); return GetId()==E.GetSrcNId() ? E.GetDstNId():E.GetSrcNId(); }
    /// Tests whether node with ID NId points to the current node.
    bool IsInNId(const int& NId) const;
    /// Tests whether the current node points to node with ID NId.
    bool IsOutNId(const int& NId) const;
    /// Tests whether node with ID NId is a neighbor of the current node.
    bool IsNbrNId(const int& NId) const { return IsOutNId(NId) || IsInNId(NId); }
    /// Returns ID of EdgeN-th in-edge.
    int GetInEId(const int& EdgeN) const { return HashI.GetDat().GetInEId(EdgeN); }
    /// Returns ID of EdgeN-th out-edge.
    int GetOutEId(const int& EdgeN) const { return HashI.GetDat().GetOutEId(EdgeN); }
    /// Returns ID of EdgeN-th in or out-edge.
    int GetNbrEId(const int& EdgeN) const { return HashI.GetDat().GetNbrEId(EdgeN); }
    /// Tests whether the edge with ID EId is an in-edge of current node.
    bool IsInEId(const int& EId) const { return HashI.GetDat().IsInEId(EId); }
    /// Tests whether the edge with ID EId is an out-edge of current node.
    bool IsOutEId(const int& EId) const { return HashI.GetDat().IsOutEId(EId); }
    /// Tests whether the edge with ID EId is an in or out-edge of current node.
    bool IsNbrEId(const int& EId) const { return IsInEId(EId) || IsOutEId(EId); }
    /*
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
    */
  };

  typedef TMNodeI<TEdge> TNodeI;

  /// Edge iterator. Only forward iteration (operator++) is supported.
  class TEdgeI {
  private:
    typedef typename THash<TInt, TEdge>::TIter THashIter;
    THashIter EdgeHI;
    const TMNet *Graph;
  public:
    TEdgeI() : EdgeHI(), Graph(NULL) { }
    TEdgeI(const THashIter& EdgeHIter, const TMNet *GraphPt) : EdgeHI(EdgeHIter), Graph(GraphPt) { }
    TEdgeI(const TEdgeI& EdgeI) : EdgeHI(EdgeI.EdgeHI), Graph(EdgeI.Graph) { }
    TEdgeI& operator = (const TEdgeI& EdgeI) { if (this!=&EdgeI) { EdgeHI=EdgeI.EdgeHI; Graph=EdgeI.Graph; }  return *this; }
    /// Increment iterator.
    TEdgeI& operator++ (int) { EdgeHI++; return *this; }
    bool operator < (const TEdgeI& EdgeI) const { return EdgeHI < EdgeI.EdgeHI; }
    bool operator == (const TEdgeI& EdgeI) const { return EdgeHI == EdgeI.EdgeHI; }
    /// Returns edge ID.
    int GetId() const { return EdgeHI.GetDat().GetId(); }
    /// Returns edge's type ID
    int GetTypeId() const { return EdgeHI.GetDat().GetTypeId(); }
    /// Returns the source of the edge.
    int GetSrcNId() const { return EdgeHI.GetDat().GetSrcNId(); }
    /// Returns the destination of the edge.
    int GetDstNId() const { return EdgeHI.GetDat().GetDstNId(); }
    /*
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
    */
    friend class TMNet;
  };

private:
  static const int NID_NBITS = 26;
  static const int NID_FLAG = (1 << NID_NBITS) - 1;
  static int GetGlobalNId(const int& NTypeId, const int& NId) { return (NTypeId << NID_NBITS) + NId;}
  static int GetLocalNId(const int& GlobalNId) { return GlobalNId & NID_FLAG; }

private:
  TCRef CRef;
  TInt MxEId;
  THash<TStr, int> NTypeH;
  THash<TStr, int> ETypeH;
  TVec<TNodeType> TypeNodeV;
  THash<TInt, TEdge> EdgeH;
  int Sz;
  TVec<TIntV> InETypes;
  TVec<TIntV> OutETypes;
  /// KeyToIndexType[N|E]: Key->(Type,Index).
  TStrIntPrH KeyToIndexTypeN, KeyToIndexTypeE;
  enum { IntType, StrType, FltType };

private:
  TNode& GetNode(const int&NId) {
    int NTypeId = GetNTypeId(NId);
    int LocalNId = GetLocalNId(NId);
    return GetNode(NTypeId, LocalNId);
  }
  const TNode& GetNode(const int&NId) const {
    int NTypeId = GetNTypeId(NId);
    int LocalNId = GetLocalNId(NId);
    return GetNode(NTypeId, LocalNId);
  }
  TNode& GetNode(const int& NTypeId, const int& NId) { return TypeNodeV[NTypeId].NodeH.GetDat(NId); }
  const TNode& GetNode(const int& NTypeId, const int& NId) const { return TypeNodeV[NTypeId].NodeH.GetDat(NId); }
  TEdge& GetEdge(const int& EId) { return EdgeH.GetDat(EId); }
  const TEdge& GetEdge(const int& EId) const { return EdgeH.GetDat(EId); }
  void AssertNTypeId(const int NTypeId) const {
    IAssertR(IsNTypeId(NTypeId), TStr::Fmt("NodeTypeId %d does not exist", NTypeId));
  }

public:
  TMNet() : CRef(), MxEId(0), NTypeH(), ETypeH(), TypeNodeV(), EdgeH(), Sz(0), InETypes(), OutETypes(),
    KeyToIndexTypeN(), KeyToIndexTypeE() { }

  TMNet(const TMNet& Graph) : MxEId(Graph.MxEId),
    NTypeH(Graph.NTypeH), ETypeH(Graph.ETypeH), TypeNodeV(Graph.TypeNodeV), EdgeH(Graph.EdgeH), Sz(Graph.Sz),
    InETypes(Graph.InETypes), OutETypes(Graph.OutETypes), KeyToIndexTypeN(), KeyToIndexTypeE() { }
  /// Static cons returns pointer to graph. Ex: PNEANet Graph=TNEANet::New().
  static TPt<TMNet<TNode> > New() {
    return TPt<TMNet<TNode> >(new TMNet());
  }
  TMNet& operator = (const TMNet& Graph) { if (this!=&Graph) {
    MxEId=Graph.MxEId; NTypeH=Graph.NTypeH; ETypeH=Graph.ETypeH; TypeNodeV=Graph.TypeNodeV; EdgeH=Graph.EdgeH;
    Sz=Graph.Sz; InETypes=Graph.InETypes; OutETypes=Graph.OutETypes;
    KeyToIndexTypeN=Graph.KeyToIndexTypeN; KeyToIndexTypeE=Graph.KeyToIndexTypeE;}
    return *this; }

  /// Gets the NTypeId
  static int GetNTypeId(const int& NId) { return NId >> NID_NBITS; } // Assuming that GlobaLNId is positive here

  /// Adds a new type with the given string into the graph.
  int AddNType(const TStr& NTypeName) {
    int KeyId = NTypeH.GetKeyId(NTypeName);
    // Has the type been added?
    if (KeyId == -1) {
      // Not added.
      int NTypeId = TypeNodeV.Len();
      NTypeH.AddDat(NTypeName, NTypeId);
      TypeNodeV.Add(TNodeType(NTypeId, NTypeName));
      IAssertR(NTypeId == InETypes.Len(), TStr::Fmt("InETypes has inconsistent length."));
      IAssertR(NTypeId == OutETypes.Len(), TStr::Fmt("OutETypes has inconsistent length."));
      InETypes.Add(TIntV());
      OutETypes.Add(TIntV());
      return NTypeId;
    } else {
      // Added. Return the stored id.
      TStr TempKey;
      int NTypeId;
      NTypeH.GetKeyDat(KeyId, TempKey, NTypeId);
      return NTypeId;
    }
  }
  /// Gets the typeId of a type
  int GetNTypeId(const TStr& NTypeStr) { return NTypeH.GetDat(NTypeStr); }
  /// Gets the type name
  TStr GetNTypeName(const int NTypeId) {
    AssertNTypeId(NTypeId);
    return TypeNodeV[NTypeId].Name;
  }
  /// Validates the TypeId
  bool IsNTypeId(const int NTypeId) const { return NTypeId >= 0 && NTypeId < TypeNodeV.Len(); }

  /// Returns the number of nodes in the graph.
  int GetNodes() const { return Sz; }
  /// Adds a node of ID NId to the graph. ##TNEANet::AddNode
  int AddNode(const int& NTypeId, int NId = -1) {
    AssertNTypeId(NTypeId);
    TNodeType* NodeType = &TypeNodeV[NTypeId];
    if (NId == -1) {
      NId = NodeType->MxNId; NodeType->MxNId++;
    } else {
      IAssertR(!IsNode(NTypeId, NId), TStr::Fmt("NodeId %d with type %d already exists", NId, NTypeId));
      NodeType->MxNId = TMath::Mx(NId+1, NodeType->GetMxNId());
    }
    TNode NewNode(NTypeId, GetGlobalNId(NTypeId, NId));
    NewNode.AddInETypeIds(InETypes[NTypeId]);
    NewNode.AddOutETypeIds(OutETypes[NTypeId]);
    NodeType->NodeH.AddDat(NId, NewNode);
    Sz++;
    return GetGlobalNId(NTypeId, NId);
  }
  /// Adds a node of ID NodeI.GetId() to the graph.
  int AddNode(const TNodeI& NodeId) { return AddNode(NodeId.GetTypeId(), NodeId.GetId()); }
  /// Validates the global NId
  bool IsNode(const int& NId) const { return IsNode(GetNTypeId(NId), GetLocalNId(NId)); }
  /// Validates the NTypeId and NId
  bool IsNode(const int& NTypeId, const int& NId) const {
    if (!IsNTypeId(NTypeId)) { return false; }
    return TypeNodeV[NTypeId].NodeH.IsKey(NId);
  }

/*
  /// Deletes node of ID NId from the graph. ##TNEANet::DelNode
  void DelNode(const int& TypeId, const int& NId);
  /// Deletes node of ID NodeI.GetId() from the graph.
  void DelNode(const TNode& NodeI) { DelNode(NodeI.GetTypeId(), NodeI.GetId()); }
*/
  /// Returns an iterator referring to the first node in the graph.
  TNodeI BegNI() const {
    return TNodeI(TypeNodeV.BegI(), this);
  }
  /// Returns an iterator referring to the past-the-end node in the graph.
  TNodeI EndNI() const { return TNodeI(TypeNodeV.EndI(), this); }
  /// Returns an iterator referring to the node of ID NId in the graph.
  TNodeI GetNI(const int& NId) const {
    int NTypeId = GetNTypeId(NId);
    int LocalNId = GetLocalNId(NId);
    return GetNI(NTypeId, LocalNId);
  }
  TNodeI GetNI(const int& NTypeId, const int& NId) const {
    return TNodeI(TypeNodeV.GetI(NTypeId), TypeNodeV[NTypeId].NodeH.GetI(NId), this);
  }

  /// Returns an ID that is larger than any node ID in the network.
  int GetMxNId(const int& NTypeId) const {
    AssertNTypeId(NTypeId);
    return TypeNodeV[NTypeId].MxNId;
  }

  int AddEType(const TStr& ETypeName, const TStr& SrcNTypeName, const TStr& DstNTypeName) {
    int KeyId = ETypeH.GetKeyId(ETypeName);
    // Has the type been added?
    if (KeyId == -1) {
      // Not added.
      int ETypeId = ETypeH.Len();
      ETypeH.AddDat(ETypeName, ETypeId);
      InETypes[GetNTypeId(DstNTypeName)].Add(ETypeId);
      OutETypes[GetNTypeId(SrcNTypeName)].Add(ETypeId);
      return ETypeId;
    } else {
      // Added. Return the stored id.
      TStr TempKey;
      int ETypeId;
      ETypeH.GetKeyDat(KeyId, TempKey, ETypeId);
      return ETypeId;
    }
  }

  /// Returns an ID that is larger than any edge ID in the network.
  int GetMxEId() const { return MxEId; }
  /// Returns the number of edges in the graph.
  int GetEdges() const { return EdgeH.Len(); }
  /// Adds an edge with ID EId between node IDs SrcNId and DstNId to the graph. ##TNEANet::AddEdge
  int AddEdge(const int& SrcNId, const int& DstNId, const int& ETypeId, int EId  = -1) {
    if (EId == -1) { EId = MxEId;  MxEId++; }
    else { MxEId = TMath::Mx(EId+1, MxEId()); }

    IAssertR(!IsEdge(EId), TStr::Fmt("EdgeId %d already exists", EId));
    IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
    EdgeH.AddDat(EId, TEdge(ETypeId, EId, SrcNId, DstNId));
    GetNode(SrcNId).AddOutNbr(ETypeId, EId);
    GetNode(DstNId).AddInNbr(ETypeId, EId);

    return EId;
  }
  /// Adds an edge between EdgeI.GetSrcNId() and EdgeI.GetDstNId() to the graph.
  int AddEdge(const TEdgeI& EdgeI) { return AddEdge(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), EdgeI.GetTypeId(), EdgeI.GetId()); }
/*
  /// Deletes an edge with edge ID EId from the graph.
  void DelEdge(const int& EId);
  /// Deletes all edges between node IDs SrcNId and DstNId from the graph. ##TNEANet::DelEdge
  void DelEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true);
*/
  /// Tests whether an edge with edge ID EId exists in the graph.
  bool IsEdge(const int& EId) const { return EdgeH.IsKey(EId); }
  /// Tests whether an edge between node IDs SrcNId and DstNId exists in the graph.
  bool IsEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true) const { int EId; return IsEdge(SrcNId, DstNId, EId, IsDir); }
  /// Tests whether an edge between node IDs SrcNId and DstNId exists in the graph. if an edge exists, return its edge ID in EId
  bool IsEdge(const int& SrcNId, const int& DstNId, int& EId, const bool& IsDir = true) const {
    const TNode& SrcNode = GetNode(SrcNId);
    for (int edge = 0; edge < SrcNode.GetOutDeg(); edge++) {
      const TEdge& Edge = GetEdge(SrcNode.GetOutEId(edge));
      if (DstNId == Edge.GetDstNId()) {
        EId = Edge.GetId();
        return true;
      }
    }
    if (! IsDir) {
      for (int edge = 0; edge < SrcNode.GetInDeg(); edge++) {
        const TEdge& Edge = GetEdge(SrcNode.GetInEId(edge));
        if (DstNId == Edge.GetSrcNId()) {
          EId = Edge.GetId();
          return true;
        }
      }
    }
    return false;
  }

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
/*
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
    if (Nodes>0) { NodeH.Gen(Nodes/2); } if (Edges>0) { EdgeH.Gen(Edges/2); } }
  /// Defragments the graph. ##TNEANet::Defrag
  void Defrag(const bool& OnlyNodeLinks=false);
  /// Checks the graph data structure for internal consistency. ##TNEANet::IsOk
  bool IsOk(const bool& ThrowExcept=true) const;
  /// Print the graph in a human readable form to an output stream OutF.
  void Dump(FILE *OutF=stdout) const;

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
*/
  friend class TPt<TMNet>;
};

typedef TMNet<TMVNode> TMVNet;
typedef TPt<TMVNet> PMVNet;
typedef TMNet<TCVNode> TCVNet;
typedef TPt<TCVNet> PCVNet;
#endif // MGRAPH_H_
