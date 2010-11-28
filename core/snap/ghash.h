/////////////////////////////////////////////////
// Graph Hash Table Key
class TGraphKey {
public:
  static const int RoundTo;
private:
public:
  TInt Nodes;
  TIntPrV EdgeV;  // renumbers the graph (node ids 0..nodes-1)
  TFltV SigV;     // signature (for hashing)
  TInt VariantId; // if graphs have same signature but are different
public:
  TGraphKey() : Nodes(-1), EdgeV(), SigV(), VariantId(0) { }
  TGraphKey(const TSFltV& GraphSigV);
  TGraphKey(const TIntV& GraphSigV);
  TGraphKey(const TFltV& GraphSigV);
  TGraphKey(const TGraphKey& GraphKey);
  TGraphKey(TSIn& SIn);
  void Save(TSOut& SOut) const;
  TGraphKey& operator = (const TGraphKey& GraphKey);
  bool operator == (const TGraphKey& GraphKey) const { return SigV==GraphKey.SigV && VariantId==GraphKey.VariantId; }

  int GetPrimHashCd() const { return abs(SigV.GetPrimHashCd() ^ VariantId); }
  int GetSecHashCd() const { return abs(SigV.GetSecHashCd() ^ VariantId<<8); }

  int GetNodes() const { return Nodes; }
  int GetEdges() const { return EdgeV.Len(); }
  int GetSigLen() const { return SigV.Len(); }
  int GetVariant() const { return VariantId; }
  void SetVariant(const int& Variant) { VariantId = Variant; }
  void SetEdgeV(const TIntPrV& EdgeIdV) { EdgeV = EdgeIdV; }

  PNGraph GetNGraph() const;
  void TakeGraph(const PNGraph& Graph); // renumbers the nodes
  void TakeGraph(const PNGraph& Graph, TIntPrV& NodeMap); // renumbers the nodes
  void TakeSig(const PNGraph& Graph, const int& MnSvdGraph, const int& MxSvdGraph); // create graph signature

  void SaveTxt(FILE *F) const;
  void SaveGViz(const TStr& OutFNm, const TStr& Desc = TStr(), const TStr& NodeAttrs="", const int& Size=-1) const;
  void PlotGViz(const TStr& OutFNm, const TStr& Desc = TStr(), const TStr& NodeAttrs="", const int& Size=-1) const;
  static bool IsIsomorph(const TGraphKey& Key1, const TGraphKey& Key2, const TIntV& NodeIdMap);
  static bool IsIsomorph(const TGraphKey& Key1, const TGraphKey& Key2, const TVec<TIntV>& NodeIdMapV);
  static bool IsIsomorph(const TGraphKey& Key1, const TGraphKey& Key2, const TVec<TIntV>& NodeIdMapV, int& IsoPermId);
};

/////////////////////////////////////////////////
// Graph Hash Table
template <class TDat>
class TGHash {
public:
  typedef typename THash<TGraphKey, TDat>::TIter TIter;
private:
  TInt MxIsoCheck;   // brute force graph isomorphism check
  TInt MxSvdGraph;   // SVD isomorphism check
  THash<TInt, TVec<TIntV> > GSzToPermH; // node permutations up to MxIsoCkeck nodes
  TBool HashOnlyTrees; // hashing only trees (exact isomorphism test)
  THash<TGraphKey, TDat> GraphH;
private:
  void InitPermutations();
  int IsGetKeyId(const PNGraph& Graph) const;
  int IsGetKeyId(const PNGraph& Graph, TGraphKey& GKey) const;
  int IsGetKeyId(const PNGraph& Graph, TGraphKey& GKey, TIntPrV& NodeMap) const;
public:
  TGHash(const bool& HashTrees, const int& MaxIsoCheck=8, const int& MaxSvdGraph=500);
  TGHash(TSIn& SIn);
  void Save(TSOut& SOut) const;

  const TDat& operator [] (const int& KeyId) const { return GraphH[KeyId]; }
  TDat& operator [] (const int& KeyId) { return GraphH[KeyId]; }
  const TDat& operator () (const TGraphKey& Key) const { return GraphH.GetDat(Key); }
  TDat& operator () (const TGraphKey& Key) { return GraphH.GetDat(Key); }
  TIter BegI() const { return GraphH.BegI(); }
  TIter EndI() const { return GraphH.EndI(); }
  TIter GetI(const int& KeyId) const  { return GraphH.GetI(KeyId); }

  bool HashTrees() const { return HashOnlyTrees; }

  void Gen(const int& Ports) { GraphH.Gen(Ports); }
  void Clr(const bool& DoDel=true, const int& NoDelLim=-1) { GraphH.Clr(DoDel, NoDelLim); }
  bool Empty() const { return GraphH.Empty(); }
  int Len() const {  return GraphH.Len(); }
  int GetPorts() const { return GraphH.GetPorts(); }
  bool IsAutoSize() const { return GraphH.IsAutoSize(); }
  int GetMxKeyIds() const { return GraphH.GetMxKeyIds(); }
  bool IsKeyIdEqKeyN() const { return GraphH.IsKeyIdEqKeyN(); }

  int AddKey(const PNGraph& Graph);
  TDat& AddDat(const PNGraph& Graph) { return GraphH[AddKey(Graph)]; }
  TDat& AddDat(const PNGraph& Graph, const TDat& Dat) { return GraphH[AddKey(Graph)] = Dat; }

  bool IsKey(const PNGraph& Graph) const { int k=IsGetKeyId(Graph); return k!=-1; }
  int GetKeyId(const PNGraph& Graph) const { int k=IsGetKeyId(Graph); IAssert(k!=-1); return k; }
  const TDat& GetDat(const PNGraph& Graph) const { return GraphH[GetKeyId(Graph)]; }
  TDat& GetDat(const PNGraph& Graph) { return GraphH[GetKeyId(Graph)]; }

  const TGraphKey& GetKey(const int& KeyId) const { return GraphH.GetKey(KeyId); }
  int GetKeyId(const TGraphKey& Key) const { return GraphH.GetKeyId(Key); }
  bool IsKey(const TGraphKey& Key) const { return GraphH.IsKey(Key); }
  bool IsKey(const TGraphKey& Key, int& KeyId) const { return GraphH.IsKey(Key, KeyId); }
  bool IsKeyId(const int& KeyId) const { return GraphH.IsKeyId(KeyId); }
  const TDat& GetDat(const TGraphKey& Key) const { return GraphH.GetDat(Key); }
  TDat& GetDat(const TGraphKey& Key) { return GraphH.GetDat(Key); }
  const TDat& GetDatId(const int& KeyId) const { return GraphH[KeyId]; }
  TDat& GetDatId(const int& KeyId) { return GraphH[KeyId]; }

  void GetKeyDat(const int& KeyId, TGraphKey& Key, TDat& Dat) const { GraphH.GetKeyDat(KeyId, Key, Dat); }
  bool IsKeyGetDat(const TGraphKey& Key, TDat& Dat) const { return GraphH.IsKeyGetDat(Key, Dat); }

  bool GetNodeMap(const PNGraph& Graph, TIntPrV& NodeMapV) const;
  bool GetNodeMap(const PNGraph& Graph, TIntPrV& NodeMapV, int& KeyId) const;

  int FFirstKeyId() const { return 0-1; }
  bool FNextKeyId(int& KeyId) const { return GraphH.FNextKeyId(KeyId); }
  void GetKeyV(TVec<TGraphKey>& KeyV) const { GraphH.GetKeyV(KeyV); }
  void GetDatV(TVec<TDat>& DatV) const { GraphH.GetDatV(DatV); }
  void GetKeyIdByDat(TIntV& KeyIdV, const bool& Asc = true) const; // order keyIds by data
  void GetKeyIdByGSz(TIntV& KeyIdV, const bool& Asc = true) const; // order keyIds by graph size
  void GetKeyDatPrV(TVec<TPair<TGraphKey, TDat> >& KeyDatPrV) const { GraphH.GetKeyDatPrV(KeyDatPrV); }
  void GetDatKeyPrV(TVec<TPair<TDat, TGraphKey> >& DatKeyPrV) const { GraphH.GetDatKeyPrV(DatKeyPrV); }

  void Defrag() { GraphH.Defrag(); }
  void Pack() { GraphH.Pack(); }

  void PlotGViz(const int& KeyId, const TStr& OutFNmPref, const TStr& OutputType = "gif", TStr Desc="") const;
  void PlotGViz(const TIntV& KeyIdV, const TStr& OutFNmPref, const TStr& OutputType = "gif") const;
  void SaveTxt(const TStr& OutFNm, const TStr& Desc, const TStr& DatColNm, const bool& SortByKeyVal=true) const;
  void SaveDetailTxt(const TStr& OutFNm, const TStr& Desc, const TStr& DatColNm) const;
};

template <class TDat>
void TGHash<TDat>::InitPermutations() {
  GSzToPermH.Clr();
  for (int nodes = 2; nodes <= MxIsoCheck; nodes++) {
    TVec<TIntV> NodePermutationV;
    TIntV NodeIdV(nodes, 0);
    for (int i = 0; i < nodes; i++)  NodeIdV.Add(i);
    NodeIdV.Pack();
    NodePermutationV.Add(NodeIdV);
    while (NodeIdV.NextPerm()) {
      NodePermutationV.Add(NodeIdV);
    }
    NodePermutationV.Pack();
    GSzToPermH.AddDat(nodes, NodePermutationV);
  }
}

template <class TDat>
TGHash<TDat>::TGHash(const bool& HashTrees, const int& MaxIsoCheck, const int& MaxSvdGraph) :
 MxIsoCheck(MaxIsoCheck), MxSvdGraph(MaxSvdGraph), GSzToPermH(), HashOnlyTrees(HashTrees), GraphH() {
  if (! HashTrees) {
    InitPermutations();
  }
}

template <class TDat>
TGHash<TDat>::TGHash(TSIn& SIn) : MxIsoCheck(SIn), MxSvdGraph(SIn), GSzToPermH(), HashOnlyTrees(SIn), GraphH(SIn) {
  if (! HashOnlyTrees) {
    InitPermutations();
  }
}

template <class TDat>
void TGHash<TDat>::Save(TSOut& SOut) const {
  MxIsoCheck.Save(SOut);
  MxSvdGraph.Save(SOut);
  HashOnlyTrees.Save(SOut);
  GraphH.Save(SOut);
}

template <class TDat>
int TGHash<TDat>::AddKey(const PNGraph& Graph) {
  if (HashOnlyTrees) {
    int RootNId;  IAssert(TSnap::IsTree(Graph, RootNId));
    TIntV TreeSig;  TSnap::GetTreeSig(Graph, RootNId, TreeSig);
    TGraphKey GKey(TreeSig);
    const int KeyId = GraphH.GetKeyId(GKey);
    if (KeyId == -1) {
      GKey.TakeGraph(Graph);
      return GraphH.AddKey(GKey);
    }
    return KeyId;
  } else {
    TGraphKey GKey;
    GKey.TakeSig(Graph, MxIsoCheck+1, MxSvdGraph); // get signature
    const int Nodes = GKey.GetNodes();
    if (Nodes > 2 && Nodes <= MxIsoCheck) {
      GKey.TakeGraph(Graph);
      // check all variants with same signature
      for (int variant = 1; ; variant++) {
        GKey.SetVariant(variant);
        int KeyId = GraphH.GetKeyId(GKey);
        if (KeyId == -1) {
          KeyId = GraphH.AddKey(GKey);
          //printf("  new variant: %d (%d)\n", KeyId, variant);
          return KeyId;

        }
        if (TGraphKey::IsIsomorph(GKey, GraphH.GetKey(KeyId), GSzToPermH.GetDat(Nodes))) {
          //printf("  isomorphic to: %d\n", KeyId);
          return KeyId;
        } // found isomorphic graph
      }
    } else {
      const int KeyId = GraphH.GetKeyId(GKey);
      if (KeyId == -1) {
        GKey.TakeGraph(Graph);
        return GraphH.AddKey(GKey);
      }
      return KeyId;
    }
  }
  Fail;
  return -1;
}

template <class TDat>
int TGHash<TDat>::IsGetKeyId(const PNGraph& Graph) const {
  TGraphKey GKey;
  return IsGetKeyId(Graph, GKey);
}

template <class TDat>
int TGHash<TDat>::IsGetKeyId(const PNGraph& Graph, TGraphKey& GKey) const {
  if (HashOnlyTrees) {
    int RootNId;  IAssert(TSnap::IsTree(Graph, RootNId));
    TIntV TreeSig;  TSnap::GetTreeSig(Graph, RootNId, TreeSig);
    GKey = TGraphKey(TreeSig);
    const int KeyId = GraphH.GetKeyId(GKey);
    //IAssert(KeyId != -1);
    return KeyId;
  } else {
    GKey.TakeSig(Graph, MxIsoCheck+1, MxSvdGraph);
    const int Nodes = GKey.GetNodes();
    if (Nodes > 2 && Nodes <= MxIsoCheck) {
      GKey.TakeGraph(Graph);
      for (int variant = 1; ; variant++) {
        GKey.SetVariant(variant);
        int KeyId = GraphH.GetKeyId(GKey);
        //IAssert(KeyId != -1);
        if (TGraphKey::IsIsomorph(GKey, GraphH.GetKey(KeyId), GSzToPermH.GetDat(Nodes))) { return KeyId; }
      }
      return -1;
    } else {
      const int KeyId = GraphH.GetKeyId(GKey);
      //IAssert(KeyId != -1);
      return KeyId;
    }
  }
  Fail;
  return -1;
}

template <class TDat>
bool TGHash<TDat>::GetNodeMap(const PNGraph& Graph, TIntPrV& NodeMapV) const {
  int KeyId;
  return GetNodeMap(Graph, NodeMapV, KeyId);
}



template <class TDat>
bool TGHash<TDat>::GetNodeMap(const PNGraph& Graph, TIntPrV& NodeMapV, int& KeyId) const {
  NodeMapV.Clr(false);
  if (HashOnlyTrees) {
    int RootNId;  IAssert(TSnap::IsTree(Graph, RootNId));
    TIntV TreeSig;  TSnap::GetTreeSig(Graph, RootNId, TreeSig, NodeMapV);
    TGraphKey GKey(TreeSig);
    KeyId = GraphH.GetKeyId(GKey);
    return KeyId != -1;
  } else {
    const int Nodes = Graph->GetNodes();
    int IsoPermId = -1;
    NodeMapV.Clr(false);
    if (Nodes == 0) { return true; }
    else if (Nodes == 1) {
      NodeMapV.Add(TIntPr(Graph->BegNI().GetId(), 0));  return true; }
    else if (Nodes <= MxIsoCheck) {
      TGraphKey GKey;
      GKey.TakeSig(Graph, MxIsoCheck+1, MxSvdGraph);
      GKey.TakeGraph(Graph, NodeMapV);
      for (int variant = 1; ; variant++) {
        GKey.SetVariant(variant);
        KeyId = GraphH.GetKeyId(GKey);
        if (KeyId == -1) return false;
        if (TGraphKey::IsIsomorph(GKey, GraphH.GetKey(KeyId), GSzToPermH.GetDat(Nodes), IsoPermId)) {
          const TIntV& K1K2Perm = GSzToPermH.GetDat(Nodes)[IsoPermId];
          // map from graph to key1 to key2
          for  (int i = 0; i < NodeMapV.Len(); i++) {
            NodeMapV[i].Val2 = K1K2Perm[NodeMapV[i].Val2];
          }
          return true;
        }
      }
      return false;
    } else {
      return false; // graph too big to find the mapping
    }
  }
  Fail;  return false;
}

template <class TDat>
void TGHash<TDat>::GetKeyIdByDat(TIntV& KeyIdV, const bool& Asc) const {
  TVec<TQuad<TDat, TInt,TInt, TInt> > DatKeyIdV(Len(), 0); // <TDat,Nodes,Edges,KeyId>
  for (int i = FFirstKeyId(); FNextKeyId(i); ) {
    DatKeyIdV.Add(TQuad<TDat, TInt,TInt, TInt>(GetDatId(i), GetKey(i).GetNodes(), GetKey(i).GetEdges(), i));
  }
  DatKeyIdV.Sort(Asc);
  KeyIdV.Gen(Len(), 0);
  for (int i = 0; i < Len(); i++) {
    KeyIdV.Add(DatKeyIdV[i].Val4);
  }
}

template <class TDat>
void TGHash<TDat>::GetKeyIdByGSz(TIntV& KeyIdV, const bool& Asc) const {
  TVec<TQuad<TInt,TInt, TDat, TInt> > DatKeyIdV(Len(), 0); // <Nodes,Edges,TDat,KeyId>
  for (int i = FFirstKeyId(); FNextKeyId(i); ) {
    DatKeyIdV.Add(TQuad< TInt,TInt, TDat, TInt>(GetKey(i).GetNodes(), GetKey(i).GetEdges(), GetDatId(i), i));
  }
  DatKeyIdV.Sort(Asc);
  KeyIdV.Gen(Len(), 0);
  for (int i = 0; i < Len(); i++) {
    KeyIdV.Add(DatKeyIdV[i].Val4);
  }
}

template <class TDat>
void TGHash<TDat>::PlotGViz(const int& KeyId, const TStr& OutFNmPref, const TStr& OutputType, TStr Desc) const {
  IAssert(OutputType == "ps" || OutputType == "gif" || OutputType == "png");
  const TGraphKey& GKey = GetKey(KeyId);
  const TStr Desc1 = TStr::Fmt("%s (%d, %d)", Desc.CStr(), GKey.GetNodes(), GKey.GetEdges());
  GKey.SaveGViz(OutFNmPref+".dot", Desc1);
  TGraphViz::DoLayout(OutFNmPref+".dot", OutFNmPref+"."+OutputType, gvlDot);
}

template <class TDat>
void TGHash<TDat>::PlotGViz(const TIntV& KeyIdV, const TStr& OutFNmPref, const TStr& OutputType) const {
  IAssert(OutputType == "ps" || OutputType == "gif" || OutputType == "png");
  TExeTm ExeTm;
  printf("Plotting %d graphs\n", KeyIdV.Len());
  for (int i = 0; i < KeyIdV.Len(); i++) {
    const TStr FNm = TStr::Fmt("%s.%03d.key%d.", OutFNmPref.CStr(), i+1, KeyIdV[i]);
    const TStr Desc = TStr::Fmt("KeyId:%d", KeyIdV[i]);
    const TGraphKey& GKey = GetKey(KeyIdV[i]);
    printf("\r  %d  g(%d, %d)    ", i, GKey.GetNodes(), GKey.GetEdges());
    GKey.SaveGViz(FNm+"dot", Desc);
    TGraphViz::DoLayout(FNm+"dot", FNm+OutputType, gvlDot);
  }
  printf("done [%s].\n", ExeTm.GetTmStr());
}

template <class TDat>
void TGHash<TDat>::SaveTxt(const TStr& OutFNm, const TStr& Desc, const TStr& DatColNm, const bool& SortByKeyVal) const {
  TIntV KeyIdV;
  if (SortByKeyVal) GetKeyIdByDat(KeyIdV, false);
  else GetKeyIdByGSz(KeyIdV, true);
  FILE *F = fopen(OutFNm.CStr(), "wt");
  fprintf(F, "Graph-Hash-Table");
  fprintf(F, "%s\n", Desc.CStr());
  fprintf(F, "%d graphs\n", KeyIdV.Len());
  fprintf(F, "Rank\tKeyId\tNodes\tEdges\t%s\n", DatColNm.CStr());
  for (int i = 0; i < KeyIdV.Len(); i++) {
    const TGraphKey& Key = GetKey(KeyIdV[i]);
    fprintf(F, "%d\t%d\t%d\t%d\t%s\n", i+1, KeyIdV[i], Key.GetNodes(), Key.GetEdges(),
      GetDatId(KeyIdV[i]).GetStr().CStr());
  }
  fclose(F);
}

template <class TDat>
void TGHash<TDat>::SaveDetailTxt(const TStr& OutFNm, const TStr& Desc, const TStr& DatColNm) const {
  TIntV KeyIdV;  GetKeyIdByDat(KeyIdV, false);
  FILE *F = fopen(OutFNm.CStr(), "wt");
  fprintf(F, "Graph-Hash-Table\n");
  fprintf(F, "%s\n", Desc.CStr());
  fprintf(F, "%d graphs", KeyIdV.Len());
  for (int i = 0; i < KeyIdV.Len(); i++) {
    fprintf(F, "\n\n[%5d]\tRank: %d\n", KeyIdV[i], i+1);
    fprintf(F, "Dat:  %s\n", GetDat(KeyIdV[i]).GetStr().CStr());
    GetDatId(KeyIdV[i]).SaveTxt(F);
  }
  fclose(F);
}

/////////////////////////////////////////////////
// Simple Edge Graph
class TSimpleGraph {
private:
  TIntPrV EdgeV;
public:
  TSimpleGraph() { }
  TSimpleGraph(const TIntPrV& GEdgeV) : EdgeV(GEdgeV) { }
  bool operator == (const TSimpleGraph& Graph) const { return EdgeV == Graph.EdgeV; }
  bool operator < (const TSimpleGraph& Graph) const { return EdgeV < Graph.EdgeV; }

  int GetEdges() const { return EdgeV.Len(); }
  void AddEdge(const int& SrcNId, const int& DstNId) { EdgeV.Add(TIntPr(SrcNId, DstNId)); }
  bool Join(const TSimpleGraph& G1, const TSimpleGraph& G2);
  TIntPrV& GetEdgeV() { return EdgeV; }
  TIntPrV& operator () () { return EdgeV; }

  void Dump(const TStr& Desc = TStr()) const;
};
typedef TVec<TSimpleGraph> TSimpleGraphV;

/////////////////////////////////////////////////
// Connected Sub-graph Enumeration
class TSubGraphsEnum {
private:
  TSimpleGraphV SgV, NextSgV;
  THash<TIntPr, TIntH> EdgeH;
  PNGraph NGraph;
public:
  TSubGraphsEnum(PNGraph Graph) : NGraph(Graph) { }

  void Gen2Graphs();
  void EnumSubGraphs(const int& MaxEdges);
  void RecurBfs(const int& MxDepth);
  void RecurBfs(const int& NId, const int& Depth, TSimpleGraph& PrevG);
  void RecurBfs1(const int& MxDepth);
  void RecurBfs1(const int& NId, const int& Depth);
  //void RecurBfs(const int& NId, const int& Depth, const THash<TIntPr, TInt>& EdgeH);
};


