//#//////////////////////////////////////////////
/// Small Directed Graphs. ##TGraphKey
class TGraphKey {
public:
  static const int RoundTo;
private:
public:
  TInt Nodes;
  TIntPrV EdgeV;  // renumbers the graph (node Ids 0..nodes-1)
  TFltV SigV;     // signature (for hashing)
  TInt VariantId; // graphs can have the same signature but are not-isomorphic. VariantId starts with 1.
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

  /// Returns the number of nodes in the graph.
  int GetNodes() const { return Nodes; }
  /// Returns the number of edges in the graph.
  int GetEdges() const { return EdgeV.Len(); }
  /// Returns the length of the signature vector of a graph. ##TGraphKey::GetSigLen
  int GetSigLen() const { return SigV.Len(); }
  /// Returns the graph variant Id. ##TGraphKey::GetVariant
  int GetVariant() const { return VariantId; }
  /// Sets the Variant Id of a given graph.
  void SetVariant(const int& Variant) { VariantId = Variant; }
  /// Returns a vector of directed edges of a graph.
  void SetEdgeV(const TIntPrV& EdgeIdV) { EdgeV = EdgeIdV; }

  /// Returns the directed graph stored in the GraphKey object.
  PNGraph GetNGraph() const;
  /// Creates a key from a given directed graph. ##TGraphKey::TakeGraph
  void TakeGraph(const PNGraph& Graph);
  /// Creates a key from a given directed graph. ##TGraphKey::TakeGraph-1
  void TakeGraph(const PNGraph& Graph, TIntPrV& NodeMap);
  /// Creates a signature for a given directed graph. ##TGraphKey::TakeSig
  void TakeSig(const PNGraph& Graph, const int& MnSvdGraph, const int& MxSvdGraph);
  
  /// Saves the graph as a list of edges.
  void SaveTxt(FILE *F) const;
  /// Saves the graph to the .DOT file format used by GraphViz. ##TGraphKey::SaveGViz
  void SaveGViz(const TStr& OutFNm, const TStr& Desc = TStr(), const TStr& NodeAttrs="", const int& Size=-1) const;
  /// Saves the graph to the .DOT file format and calls GraphViz to draw it. ##TGraphKey::DrawGViz
  void DrawGViz(const TStr& OutFNm, const TStr& Desc = TStr(), const TStr& NodeAttrs="", const int& Size=-1) const;

  /// Checks whether directed graph Key1 is isomorphic to the directed graph Key2 under node Id permutation NodeIdMap. ##TGraphKey::IsIsomorph
  static bool IsIsomorph(const TGraphKey& Key1, const TGraphKey& Key2, const TIntV& NodeIdMap);
  /// Checks whether directed graph Key1 is isomorphic to the directed graph Key2 under all the permutations of node Ids stored in NodeIdMapV.
  static bool IsIsomorph(const TGraphKey& Key1, const TGraphKey& Key2, const TVec<TIntV>& NodeIdMapV);
  /// Checks whether directed graph Key1 is isomorphic to the directed graph Key2 under all the permutations of node Ids stored in NodeIdMapV and returns the Id of the permutation of node Ids (IsoPermId) which makes the two graphs isomorphic.
  static bool IsIsomorph(const TGraphKey& Key1, const TGraphKey& Key2, const TVec<TIntV>& NodeIdMapV, int& IsoPermId);
};

//#//////////////////////////////////////////////
/// Graph Hash Table. ##TGHash
template <class TDat>
class TGHash {
public:
  typedef typename THash<TGraphKey, TDat>::TIter TIter;
private:
  TInt MxIsoCheck;     // maximum graph size for which we perform brute force graph isomorphism check
  TInt MxSvdGraph;     // maximum graph size for which we perform SVD-based approximate isomorphism check
  THash<TInt, TVec<TIntV> > GSzToPermH; // Graph size to a vector of all node permutations (for graphs of up to MxIsoCkeck nodes)
  TBool HashOnlyTrees; // hashing only trees (exact isomorphism test)
  THash<TGraphKey, TDat> GraphH;
private:
  void InitPermutations();
  int IsGetKeyId(const PNGraph& Graph) const;
  int IsGetKeyId(const PNGraph& Graph, TGraphKey& GKey) const;
  int IsGetKeyId(const PNGraph& Graph, TGraphKey& GKey, TIntPrV& NodeMap) const;
public:
  /// Default contructor. ##TGHash::TGHash
  TGHash(const bool& HashTrees, const int& MaxIsoCheck=8, const int& MaxSvdGraph=500);
  TGHash(TSIn& SIn);
  void Save(TSOut& SOut) const;

  /// Accesses the data at hash table position index KeyId.
  const TDat& operator [] (const int& KeyId) const { return GraphH[KeyId]; }
  /// Accesses the data at hash table position index KeyId.
  TDat& operator [] (const int& KeyId) { return GraphH[KeyId]; }
  /// Accesses the data of graph-key Key.
  const TDat& operator () (const TGraphKey& Key) const { return GraphH.GetDat(Key); }
  /// Accesses the data of graph-key Key.
  TDat& operator () (const TGraphKey& Key) { return GraphH.GetDat(Key); }
  /// Returns iterator to the first element of the hash table.
  TIter BegI() const { return GraphH.BegI(); }
  /// Returns iterator to one past the last element of the hash table.
  TIter EndI() const { return GraphH.EndI(); }
  /// Returns iterator to a key at position index KeyId.
  TIter GetI(const int& KeyId) const  { return GraphH.GetI(KeyId); }

  /// Returns whether the hash table only hashes trees and not arbitrary directed graphs.
  bool HashTrees() const { return HashOnlyTrees; }

  /// Initializes the hash table for the expected number of keys ExpectVals.
  void Gen(const int& ExpectVals) { GraphH.Gen(ExpectVals); }
  /// Removes all the elements from the hash table. ##TGHash::Clr
  void Clr(const bool& DoDel=true, const int& NoDelLim=-1) { GraphH.Clr(DoDel, NoDelLim); }
  /// Tests whether the hash table is empty.
  bool Empty() const { return GraphH.Empty(); }
  /// Returns the number of keys in the hash table.
  int Len() const {  return GraphH.Len(); }
  /// Returns the number of ports in the hash table.
  int GetPorts() const { return GraphH.GetPorts(); }
  /// Tests whether the hash table automatically adjusts the number of ports based on the number of keys.
  bool IsAutoSize() const { return GraphH.IsAutoSize(); }
  /// Returns the maximum key Id of any element in the hash table.
  int GetMxKeyIds() const { return GraphH.GetMxKeyIds(); }
  /// Tests whether there are any unused slots in the hash table. ##TGHash::IsKeyIdEqKeyN
  bool IsKeyIdEqKeyN() const { return GraphH.IsKeyIdEqKeyN(); }

  /// Adds a key Graph to the table and returns its KeyId. ##TGHash::AddKey
  int AddKey(const PNGraph& Graph);
  /// Adds a key Graph to the table and returns its data value. ##TGHash::AddDat
  TDat& AddDat(const PNGraph& Graph) { return GraphH[AddKey(Graph)]; }
  /// Adds a key Graph to the table, sets its data value to value of Dat and returns Dat. ##TGHash::AddDat-1
  TDat& AddDat(const PNGraph& Graph, const TDat& Dat) { return GraphH[AddKey(Graph)] = Dat; }

  /// Test whether Graph is an existing key in the hash table.
  bool IsKey(const PNGraph& Graph) const { int k=IsGetKeyId(Graph); return k!=-1; }
  /// Returns the KeyId (position index) of key Graph. ##TGHash::GetKeyId
  int GetKeyId(const PNGraph& Graph) const { return IsGetKeyId(Graph); }
  /// Returns the data associated with key Graph. ##TGHash::GetDat
  const TDat& GetDat(const PNGraph& Graph) const { return GraphH[GetKeyId(Graph)]; }
  /// Returns the data associated with key Graph. ##TGHash::GetDat-1
  TDat& GetDat(const PNGraph& Graph) { return GraphH[GetKeyId(Graph)]; }

  /// Returns the GraphKey with position index KeyId.
  const TGraphKey& GetKey(const int& KeyId) const { return GraphH.GetKey(KeyId); }
  /// Returns the KeyId for a given Key. ##TGHash::GetKeyId
  int GetKeyId(const TGraphKey& Key) const { return GraphH.GetKeyId(Key); }
  /// Tests whether a given Key exists in the hash table.
  bool IsKey(const TGraphKey& Key) const { return GraphH.IsKey(Key); }
  /// Tests whether a given Key exists in the hash table.
  bool IsKey(const TGraphKey& Key, int& KeyId) const { return GraphH.IsKey(Key, KeyId); }
  /// Tests whether there exists a key at given position index KeyId.
  bool IsKeyId(const int& KeyId) const { return GraphH.IsKeyId(KeyId); }
  /// Returns data with a given graph Key. ##TGHash::GetDat-2
  const TDat& GetDat(const TGraphKey& Key) const { return GraphH.GetDat(Key); }
  /// Returns data with a given graph Key. ##TGHash::GetDat-3
  TDat& GetDat(const TGraphKey& Key) { return GraphH.GetDat(Key); }
  /// Returns data at a given position index KeyId. ##TGHash::GetDatId
  const TDat& GetDatId(const int& KeyId) const { return GraphH[KeyId]; }
  /// Returns data at a given position index KeyId. ##TGHash::GetDatId-1
  TDat& GetDatId(const int& KeyId) { return GraphH[KeyId]; }

  /// Returns Key and Data at a given position index KeyId.
  void GetKeyDat(const int& KeyId, TGraphKey& Key, TDat& Dat) const { GraphH.GetKeyDat(KeyId, Key, Dat); }
  /// Test whether Key exists and sets its data to Dat.
  bool IsKeyGetDat(const TGraphKey& Key, TDat& Dat) const { return GraphH.IsKeyGetDat(Key, Dat); }

  /// Returns the mapping of node Ids of the Graph to those of the graph-key in the hash table. ##TGHash::GetNodeMap
  bool GetNodeMap(const PNGraph& Graph, TIntPrV& NodeMapV) const;
  /// Returns the mapping of node Ids of the Graph to those of the graph-key in the hash table and the Graph KeyId. ##TGHash::GetNodeMap-1
  bool GetNodeMap(const PNGraph& Graph, TIntPrV& NodeMapV, int& KeyId) const;

  /// Finds first KeyId. ##TGHash::FFirstKeyId
  int FFirstKeyId() const { return 0-1; }
  /// Finds next KeyId. ##TGHash::FNextKeyId
  bool FNextKeyId(int& KeyId) const { return GraphH.FNextKeyId(KeyId); }
  /// Returns a vector of keys stored in the hash table.
  void GetKeyV(TVec<TGraphKey>& KeyV) const { GraphH.GetKeyV(KeyV); }
  /// Returns a vector of data elements stored in the hash table.
  void GetDatV(TVec<TDat>& DatV) const { GraphH.GetDatV(DatV); }
  /// Returns a vector of KeyIds of hash table elements sorted by their data value. ##TGHash::GetKeyIdByDat
  void GetKeyIdByDat(TIntV& KeyIdV, const bool& Asc = true) const;
  /// Returns a vector of KeyIds of hash table elements sorted by their graph size. ##TGHash::GetKeyIdByGSz
  void GetKeyIdByGSz(TIntV& KeyIdV, const bool& Asc = true) const;
  /// Returns a vector of pairs (Key, Data) elements stored in the hash table.
  void GetKeyDatPrV(TVec<TPair<TGraphKey, TDat> >& KeyDatPrV) const { GraphH.GetKeyDatPrV(KeyDatPrV); }
  /// Returns a vector of pairs (Data, Key) elements stored in the hash table.
  void GetDatKeyPrV(TVec<TPair<TDat, TGraphKey> >& DatKeyPrV) const { GraphH.GetDatKeyPrV(DatKeyPrV); }

  /// Removes unused slots from the hash table. ##TGHash::Defrag
  void Defrag() { GraphH.Defrag(); }
  /// Frees the unused memory by the hash table.
  void Pack() { GraphH.Pack(); }

  /// Saves a given graph with key Id KeyId in DOT format and calls the GraphViz to draw it.
  void DrawGViz(const int& KeyId, const TStr& OutFNmPref, const TStr& OutputType = "gif", TStr Desc="") const;
  /// Saves a set of graphs with key Ids KeyIdV in DOT format and calls the GraphViz to draw them.
  void DrawGViz(const TIntV& KeyIdV, const TStr& OutFNmPref, const TStr& OutputType = "gif") const;
  /// Saves all graphs stored in the hash table into a text file.
  void SaveTxt(const TStr& OutFNm, const TStr& Desc, const TStr& DatColNm, const bool& SortByKeyVal=true) const;
  /// Saves all graphs stored in the hash table into a text file and include additional information.
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
      // Check all variants with same signature
      for (int variant = 1; ; variant++) {
        GKey.SetVariant(variant);
        int KeyId = GraphH.GetKeyId(GKey);
        if (KeyId == -1) { // Key of such signature and variant does not exist yet.
          KeyId = GraphH.AddKey(GKey);
          return KeyId;
        }
        if (TGraphKey::IsIsomorph(GKey, GraphH.GetKey(KeyId), GSzToPermH.GetDat(Nodes))) { // Graph isomorphism test
          return KeyId;  // Found isomorphic graph.
        }
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
    // For trees we perform exact isomorshism test based on graph signatures
    int RootNId;  IAssert(TSnap::IsTree(Graph, RootNId));
    TIntV TreeSig;  TSnap::GetTreeSig(Graph, RootNId, TreeSig);
    GKey = TGraphKey(TreeSig);
    const int KeyId = GraphH.GetKeyId(GKey);
    return KeyId;
  } else {
    // For small graphs  of less than MxIsoCheck nodes we perform brute force isomorphism checking
    GKey.TakeSig(Graph, MxIsoCheck+1, MxSvdGraph);
    const int Nodes = GKey.GetNodes();
    if (Nodes > 2 && Nodes <= MxIsoCheck) {
      GKey.TakeGraph(Graph);
      for (int variant = 1; ; variant++) {
        GKey.SetVariant(variant);
        int KeyId = GraphH.GetKeyId(GKey); // Is there a graph of the same signature and same VariantId
        if (KeyId == -1) { return -1; }
        if (TGraphKey::IsIsomorph(GKey, GraphH.GetKey(KeyId), GSzToPermH.GetDat(Nodes))) { return KeyId; } // perform brute force isomorphism check
      }
    } else {
      // For all other graphs we perform approximate graph isomorphism checking
      const int KeyId = GraphH.GetKeyId(GKey);
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
        if (KeyId == -1) { return false; }
        if (TGraphKey::IsIsomorph(GKey, GraphH.GetKey(KeyId), GSzToPermH.GetDat(Nodes), IsoPermId)) {
          const TIntV& K1K2Perm = GSzToPermH.GetDat(Nodes)[IsoPermId];
          // map from graph to key1 to key2
          for  (int i = 0; i < NodeMapV.Len(); i++) {
            NodeMapV[i].Val2 = K1K2Perm[NodeMapV[i].Val2]; }
          return true;
        }
      }
      return false;
    } else {
      return false; // graph too big to find the mapping
    }
  }
  Fail;
  return false;
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
void TGHash<TDat>::DrawGViz(const int& KeyId, const TStr& OutFNmPref, const TStr& OutputType, TStr Desc) const {
  IAssert(OutputType == "ps" || OutputType == "gif" || OutputType == "png");
  const TGraphKey& GKey = GetKey(KeyId);
  const TStr Desc1 = TStr::Fmt("%s (%d, %d)", Desc.CStr(), GKey.GetNodes(), GKey.GetEdges());
  GKey.SaveGViz(OutFNmPref+".dot", Desc1);
  TSnap::TSnapDetail::GVizDoLayout(OutFNmPref+".dot", OutFNmPref+"."+OutputType, gvlDot);
}

template <class TDat>
void TGHash<TDat>::DrawGViz(const TIntV& KeyIdV, const TStr& OutFNmPref, const TStr& OutputType) const {
  IAssert(OutputType == "ps" || OutputType == "gif" || OutputType == "png");
  TExeTm ExeTm;
  printf("Plotting %d graphs\n", KeyIdV.Len());
  for (int i = 0; i < KeyIdV.Len(); i++) {
    const TStr FNm = TStr::Fmt("%s.%03d.key%d.", OutFNmPref.CStr(), i+1, KeyIdV[i]());
    const TStr Desc = TStr::Fmt("KeyId:%d", KeyIdV[i]());
    const TGraphKey& GKey = GetKey(KeyIdV[i]);
    printf("\r  %d  g(%d, %d)    ", i, GKey.GetNodes(), GKey.GetEdges());
    GKey.SaveGViz(FNm+"dot", Desc);
    TSnap::TSnapDetail::GVizDoLayout(FNm+"dot", FNm+OutputType, gvlDot);
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
    fprintf(F, "%d\t%d\t%d\t%d\t%s\n", i+1, KeyIdV[i](), Key.GetNodes(), Key.GetEdges(),
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
    fprintf(F, "\n\n[%5d]\tRank: %d\n", KeyIdV[i](), i+1);
    fprintf(F, "Dat:  %s\n", GetDat(KeyIdV[i]).GetStr().CStr());
    GetDatId(KeyIdV[i]).SaveTxt(F);
  }
  fclose(F);
}

//#//////////////////////////////////////////////
/// Simple directed/undirected graph defined by its edges.
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

//#//////////////////////////////////////////////
/// Connected Sub-graph Enumeration.
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


