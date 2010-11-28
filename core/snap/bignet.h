/////////////////////////////////////////////////
// Big Network
template <class TNodeData, bool IsDir>
class TBigNet {
public:
  typedef TNodeData TNodeDat;
  typedef TBigNet<TNodeData, IsDir> TNet;
  typedef TPt<TNet> PNet;
public:
  class TNode;
  //typedef TSparseHash<TInt, TNode> TNodeH;
  typedef THash<TInt, TNode> TNodeH; // use SaveToDisk to convert between the two hash table types
  typedef TPt<TBigNet<TNodeData, IsDir> > PBigNet;
  typedef TVecPool<TInt> TPool;
  typedef TPt<TPool> PPool;

  class TNode {
  public:
    TInt InVId, OutVId; // if undirected, InNIdV == OutNIdV
    TNodeDat Dat;
  public:
    TNode() : InVId(-1), OutVId(-1), Dat() { }
    TNode(const int& InVecId, const int& OutVecId) : InVId(InVecId), OutVId(OutVecId), Dat() { }
    TNode(const int& InVecId, const int& OutVecId, const TNodeDat& NodeDat) :
      InVId(InVecId), OutVId(OutVecId), Dat(NodeDat) { }
    TNode(const TNode& Node) : InVId(Node.InVId), OutVId(Node.OutVId), Dat(Node.Dat) { }
    TNode(TSIn& SIn) : InVId(SIn), OutVId(SIn), Dat(SIn) { }
    void Save(TSOut& SOut) const { SOut.Save(InVId); SOut.Save(OutVId); Dat.Save(SOut); }
    bool IsUnused() const { return InVId==-1 && OutVId==-1; }
  };

  class TNodeI {
  protected:
    typedef typename TNodeH::TIter THashIter;
    THashIter NodeHI;
    TPool *Pool;
    int InDeg, OutDeg, *InNIdV, *OutNIdV; // if undirected, InNIdV==OutNIdV
  public:
    inline void GetInOutNIdV();
    int GetInVId() const { return NodeHI->Dat.InVId; }
    int GetOutVId() const { return NodeHI->Dat.OutVId; }
  public:
    TNodeI() : NodeHI(), Pool(NULL), InDeg(0), OutDeg(0), InNIdV(NULL), OutNIdV(NULL) { }
    TNodeI(const THashIter& NodeHIter, TPool *PoolPt) : NodeHI(NodeHIter), Pool(PoolPt) { GetInOutNIdV(); }
    TNodeI(const TNodeI& NodeI) : NodeHI(NodeI.NodeHI), Pool(NodeI.Pool) { GetInOutNIdV(); }
    TNodeI& operator = (const TNodeI& NI) { NodeHI=NI.NodeHI; Pool=NI.Pool; GetInOutNIdV(); return *this; }
    TNodeI& operator++ (int) { NodeHI++; GetInOutNIdV(); return *this; }
    bool operator < (const TNodeI& NI) const { return NodeHI < NI.NodeHI; }
    bool operator == (const TNodeI& NI) const { return NodeHI == NI.NodeHI; }
    int GetId() const { return NodeHI->Key(); }
    int GetDeg() const { return GetInDeg()+(InNIdV!=OutNIdV?GetOutDeg():0); }
    int GetInDeg() const { return InDeg; }
    int GetOutDeg() const { return OutDeg; }
    int GetInNId(const int& NodeN) const { return InNIdV[NodeN]; }
    int GetOutNId(const int& NodeN) const { return OutNIdV[NodeN]; }
    int GetOutNbhId(const int& NodeN) const { return NodeN<OutDeg ? OutNIdV[NodeN]:InNIdV[NodeN-OutDeg]; }
    bool IsInNId(const int& NId) const { return BinSearch(InNIdV, InNIdV+InDeg, NId)!=NULL; }
    bool IsOutNId(const int& NId) const { return BinSearch(OutNIdV, OutNIdV+OutDeg, NId)!=NULL; }
    bool IsNbhNId(const int& NId) const { return IsOutNId(NId) || IsInNId(NId); }
    const TNodeData& operator () () const { return GetDat(); }
    TNodeData& operator () () { return GetDat(); }
    const TNodeData& GetDat() const { return NodeHI->Dat.Dat; }
    TNodeData& GetDat() { return NodeHI->Dat.Dat; }
    // requires pointer back to the network (see as in TNodeNet)
    //const TNodeData& GetInNDat(const int& NodeN) const { return Net->GetNDat(GetInNId(NodeN)); }
    //TNodeData& GetInNDat(const int& NodeN) { return Net->GetNDat(GetInNId(NodeN)); }
    //const TNodeData& GetOutNDat(const int& NodeN) const { return Net->GetNDat(GetOutNId(NodeN)); }
    //TNodeData& GetOutNDat(const int& NodeN) { return Net->GetNDat(GetOutNId(NodeN)); }
    //const TNodeData& GetNbhNDat(const int& NodeN) const { return Net->GetNDat(GetNbhNId(NodeN)); }
    //TNodeData& GetNbhNDat(const int& NodeN) { return Net->GetNDat(GetNbhNId(NodeN)); }
    void Dump() const;
    friend class TBigNet<TNodeData, IsDir>;
  };

  class TEdgeI {
  private:
    TNodeI CurNode, EndNode;
    int CurEdge;
  public:
    TEdgeI() : CurNode(), EndNode(), CurEdge(0) { }
    TEdgeI(const TNodeI& NodeI, const TNodeI& EndNodeI, const int& EdgeN=0) : CurNode(NodeI), EndNode(EndNodeI), CurEdge(0) { }
    TEdgeI(const TEdgeI& EdgeI) : CurNode(EdgeI.CurNode), EndNode(EdgeI.EndNode), CurEdge(EdgeI.CurEdge) { }
    TEdgeI& operator = (const TEdgeI& EdgeI) { if (this!=&EdgeI) { CurNode=EdgeI.CurNode;  EndNode=EdgeI.EndNode;  CurEdge=EdgeI.CurEdge; }  return *this; }
    TEdgeI& operator++ (int) { CurEdge++; if (CurEdge >= CurNode.GetOutDeg()) { CurEdge=0;  CurNode++;
      while (CurNode < EndNode && CurNode.GetOutDeg()==0) { CurNode++; } }  return *this; }
    bool operator < (const TEdgeI& EdgeI) const { return CurNode<EdgeI.CurNode || (CurNode==EdgeI.CurNode && CurEdge<EdgeI.CurEdge); }
    bool operator == (const TEdgeI& EdgeI) const { return CurNode == EdgeI.CurNode && CurEdge == EdgeI.CurEdge; }
    int GetId() const { return -1; }
    int GetSrcNId() const { return CurNode.GetId(); }
    int GetDstNId() const { return CurNode.GetOutNId(CurEdge); }
    const TNodeData& GetSrcNDat() const { return CurNode.GetNDat(); }
    TNodeData& GetDstNDat() { return CurNode.GetOutNDat(CurEdge); }
    friend class TNodeNet<TNodeData>;
  };
protected:
  bool IsNode(const int& NId, TNode& Node) const { return NodeH.IsKeyGetDat(NId, Node); }
  int* GetInNIdVPt(const int& NId) const { return (int *) Pool.GetValVPt(GetNode(NId).InVId); }
  int* GetOutNIdVPt(const int& NId) const { return (int *) Pool.GetValVPt(GetNode(NId).OutVId); }
  static void AddSorted(int* Beg, int* End, const int& Val);
  static const int* BinSearch(const int* Beg, const int* End, const int& Val);
  static const int* BinSearch2(const int* Beg, const int* End, const int& Val);
  const TNode& GetNode(const int& NId) const { return NodeH.GetDat(NId); }
  TNode& GetNode(const int& NId) { return NodeH.GetDat(NId); }
public:
  enum { DelNId = INT_MAX }; // id of a deleted node
protected:
  TCRef CRef;
  TInt MxNId;
  TB32Set Flags;
  TPool Pool;
  TNodeH NodeH;
public:
  TBigNet(const int& Nodes, const TSize& Edges, const bool& Sources=false);
  TBigNet(TSIn& SIn) : MxNId(SIn), Flags(SIn), Pool(SIn), NodeH(SIn) { TBool Dir(SIn); IAssert(Dir()==IsDir); }
  virtual ~TBigNet() { }
  virtual void Save(TSOut& SOut) const;
  static PBigNet New(const int& Nodes, const TSize& Edges, const bool& Sources=false) {
    return PBigNet(new TBigNet(Nodes, Edges, Sources)); }
  static PBigNet Load(TSIn& SIn) { return PBigNet(new TBigNet(SIn)); }
  TBigNet& operator = (const TBigNet& Net) { if (this!=&Net) {
    MxNId=Net.MxNId; Flags=Net.Flags; Pool=Net.Pool; NodeH=Net.NodeH; }  return *this; }

  bool OnlySources() const { return Flags.In(gfSources); }
  bool HasFlag(const TGraphFlag& Flag) const {
    return HasGraphFlag(typename TBigNet, Flag) || (Flag==gfSources && OnlySources()); }
  void DumpFlags() const;

  // vertices
  int GetNodes() const { return NodeH.Len(); }
  int GetMxNId() const { return MxNId; }
  int AddNode(int NId, const int& InDeg, const int& OutDeg);
  int AddNode(int NId, const int& InDeg, const int& OutDeg, const TNodeDat& NodeDat);
  int AddNode(int NId, const TIntV& InNIdV, const TIntV& OutNIdV);
  int AddNode(int NId, const TIntV& InNIdV, const TIntV& OutNIdV, const TNodeDat& NodeDat);
  int AddUndirNode(int NId, const int& Deg);
  int AddUndirNode(int NId, const int& Deg, const TNodeDat& NodeDat);
  int AddUndirNode(int NId, const TIntV& EdgeNIdV);
  int AddUndirNode(int NId, const TIntV& EdgeNIdV, const TNodeDat& NodeDat);
  void SetInNIdV(int NId, const TIntV& InNIdV);
  void SetOutNIdV(int NId, const TIntV& OutNIdV);
  void GetInNIdV(int NId, TIntV& OutNIdV) const;
  void GetOutNIdV(int NId, TIntV& OutNIdV) const;
  bool IsNode(const int& NId) const { return NodeH.IsKey(NId); }
  TNodeI BegNI() const { return TNodeI(NodeH.BegI(), (TPool *)&Pool); }
  TNodeI EndNI() const { return TNodeI(NodeH.EndI(), (TPool *)&Pool); }
  TNodeI GetNI(const int& NId) const { return TNodeI(NodeH.GetI(NId), (TPool *)&Pool); }
  TNodeDat& GetNDat(const int& NId) { return NodeH.GetDat(NId).Dat; }
  const TNodeDat& GetNDat(const int& NId) const { return NodeH.GetDat(NId).Dat; }
  // edges
  TEdgeI BegEI() const { TNodeI NI=BegNI();  while(NI<EndNI() && NI.GetOutDeg()==0) NI++;  return TEdgeI(NI, EndNI()); }
  TEdgeI EndEI() const { return TEdgeI(EndNI(), EndNI()); }
  TEdgeI GetEI(const int& EId) const; // not supported

  // delete nodes
  int IsolateNode(int NId); // isolate the node by setting edge endpoints to point to node id DelNId, IsNode(DelNId)==false)
  int DelNode(int NId); // set neigbors to point to DelNId, delete node from the node table
  bool IsIsoNode(const int& NId) const;
  uint GetDelEdges(); // the number deleted edges
  void CompactEdgePool(); // after nodes are isolated and deleted, we compact the empty space

  // edges
  ::TSize GetEdges() const { return Pool.GetVals(); }
  int AddEdge(const int& SrcNId, const int& DstNId);
  bool IsEdge(const int& SrcNId, const int& DstNId, const bool& Dir=true) const;

  void SortEdgeV();
  void InvertFromSources(uint ExpectNodes=0); // add missing nodes and in-links
  void Rewire(TRnd& Rnd = TInt::Rnd); // create a random network with same degree distribution (configuration model)

  // manipulation
  PNGraph GetNGraph(const bool& RenumberNodes=false) const;
  PNGraph GetSubNGraph(const TIntV& NIdV) const;
  PBigNet GetSubGraph(const TIntV& NIdV, const bool& RenumberNodes=false) const;
  void GetSubGraph(const TIntV& NIdV, TBigNet* NewNet, const bool& RenumberNodes=false) const;

  int GetRndNId(TRnd& Rnd=TInt::Rnd) const { return NodeH.GetKey(NodeH.GetRndKeyId(Rnd)); }
  TNodeI GetRndNI(TRnd& Rnd=TInt::Rnd) const { return GetNI(GetRndNId(Rnd)); }
  void GetNIdV(TIntV& NIdV) const;

  bool Empty() const { return GetNodes()==0; }
  void Clr(const bool& DoDel = true) { MxNId = 0;  NodeH.Clr(DoDel); Pool.Clr(DoDel); }
  void Reserve(const int& Nodes, const TSize& Edges);
  void Defrag(const bool& OnlyNodeLinks = false) { }
  bool IsOk() const;
  void Dump(const TStr& Desc = TStr()) const;
  void SaveForDisk(const TStr& OutFNm) const;

  static void LoadNodeDatH(const TStr& InFNm, TNodeH& NodeH);
  static void SaveToDisk(const TStr& InFNm, const TStr& OutFNm, const bool& SaveSparseHash);
  friend class TPt<TBigNet>;
};

// set flags
namespace TSnap {
template <class TNodeData, bool IsDir> struct IsDirected<TBigNet<TNodeData, IsDir> > { enum { Val = 0 }; };
template <class TNodeData> struct IsDirected<TBigNet<TNodeData, true> > { enum { Val = 1 }; };
template <class TNodeData, bool IsDir> struct IsNodeDat<TBigNet<TNodeData, IsDir> > { enum { Val = 1 }; };
}

template <class TNodeData, bool IsDir>
inline void TBigNet<TNodeData, IsDir>::TNodeI::GetInOutNIdV() {
  if (NodeHI.IsEnd()) return;
  const TNode& N = NodeHI->Dat;
  if (! Pool->IsVId(N.InVId)) {
    InDeg=0; OutDeg=0; }
  else {
    InDeg=Pool->GetVLen(N.InVId);
    OutDeg=Pool->GetVLen(N.OutVId);
    InNIdV=(int *)Pool->GetValVPt(N.InVId);
    OutNIdV=(int *)Pool->GetValVPt(N.OutVId);
  }
}

template <class TNodeData, bool IsDir>
void TBigNet<TNodeData, IsDir>::TNodeI::Dump() const {
  printf("NodeId: %d\n", GetId());
  printf("  I:%4d]", GetInDeg());
  for (int i = 0; i < GetInDeg(); i++) { printf("  %d", GetInNId(i)); }
  printf("\n  O:%4d]", GetOutDeg());
  for (int i = 0; i < GetOutDeg(); i++) { printf("  %d", GetOutNId(i)); }
  printf("\n");
}

template <class TNodeData, bool IsDir>
void TBigNet<TNodeData, IsDir>::AddSorted(int* Beg, int* End, const int& Val) {
  // there is at least one free slot available for the Val
  IAssertR(*(End-1)==TInt::Mx, "Edge can not be added: no free space");
  // find position to insert
  int Half, Len = int(End-Beg);
  while (Len > 0) {
    Half = Len/2;
    int* Mid=Beg+Half;
    if (*Mid < Val) { Beg=Mid+1; Len=Len-Half-1; } else { Len=Half; } }
  IAssertR(*Beg != Val, "Value already existis");
  // insert
  memmove(Beg+1, Beg, sizeof(int)*(End-Beg-1));
  *Beg = Val;
}

template <class TNodeData, bool IsDir>
const int* TBigNet<TNodeData, IsDir>::BinSearch(const int* Beg, const int* End, const int& Val) {
  End--;  const int *Mid;
  while (Beg <= End) { Mid = Beg+(End-Beg)/2;
    if (*Mid == Val) { return Mid; }
    else if (Val < *Mid) { End=Mid-1; } else { Beg=Mid+1; }
  }
  return NULL;
}

template <class TNodeData, bool IsDir>
const int* TBigNet<TNodeData, IsDir>::BinSearch2(const int* Beg, const int* End, const int& Val) {
  const int* First = Beg;
  const int* Last = End;
  const int* Mid;
  TSize len = End-Beg, half;
  while (len > 0) {
    half = len>>1;  Mid=First+half;
    if (*Mid < Val) { First = Mid; First++; len=len-half-1; }
    else { len=half; }
  }
  return First==Last ? Last-1 : First;
}

template <class TNodeData, bool IsDir>
TBigNet<TNodeData, IsDir>::TBigNet(const int& Nodes, const TSize& Edges, const bool& Sources) :
CRef(), MxNId(0), Flags(), Pool(IsDir ? 2*Edges:Edges, 10000000, true, TInt::Mx), NodeH(Nodes) {
  //Flags.Incl(gfNodeGraph);
  //Flags.Incl(gfNetwork);
  //if (IsDir) { Flags.Incl(gfDirected); }
  if (Sources) {
    Flags.Incl(gfSources);
    IAssertR(! IsDir, "Jure: not clear what happens is graph is Undirected and has only sources.");
  }
  //DumpFlags();
}

template <class TNodeData, bool IsDir>
void TBigNet<TNodeData, IsDir>::Save(TSOut& SOut) const {
  MxNId.Save(SOut);
  Flags.Save(SOut);
  Pool.Save(SOut);
  NodeH.Save(SOut);
  TBool(IsDir).Save(SOut);
}

template <class TNodeData, bool IsDir>
void TBigNet<TNodeData, IsDir>::DumpFlags() const {
  for (int i = 1; i <int(gfMx); i++) {
    if (HasFlag(TGraphFlag(i))) { printf("  +"); }
    else { printf("  -"); }
    printf("%s", TSnap::GetFlagStr(TGraphFlag(i)).CStr());
  }
  printf("\n");
}

template <class TNodeData, bool IsDir>
int TBigNet<TNodeData, IsDir>::AddNode(int NId, const int& InDeg, const int& OutDeg) {
  CAssert(IsDir);
  if (NId == -1) { NId = MxNId;  MxNId.Val++; }
  else { MxNId = TMath::Mx(NId+1, MxNId()); }
  TNode& Node = NodeH.AddDat(NId);
  IAssertR(Node.IsUnused(), TStr::Fmt("NodeId %d already exists", NId));
  Node.InVId = Pool.AddEmptyV(InDeg);
  Node.OutVId = Pool.AddEmptyV(OutDeg);
  return NId;
}

template <class TNodeData, bool IsDir>
int TBigNet<TNodeData, IsDir>::AddNode(int NId, const int& InDeg, const int& OutDeg, const TNodeData& NodeDat) {
  CAssert(IsDir);
  if (NId == -1) { NId = MxNId;  MxNId.Val++; }
  else { MxNId = TMath::Mx(NId+1, MxNId()); }
  TNode& Node = NodeH.AddDat(NId);
  IAssertR(Node.IsUnused(), TStr::Fmt("NodeId %d already exists", NId));
  Node.InVId = Pool.AddEmptyV(InDeg);
  Node.OutVId = Pool.AddEmptyV(OutDeg);
  Node.Dat = NodeDat;
  return NId;
}

template <class TNodeData, bool IsDir>
int TBigNet<TNodeData, IsDir>::AddUndirNode(int NId, const int& Deg) {
  CAssert(! IsDir);
  if (NId == -1) { NId = MxNId;  MxNId.Val++; }
  else { MxNId = TMath::Mx(NId+1, MxNId()); }
  TNode& Node = NodeH.AddDat(NId);
  IAssertR(Node.IsUnused(), TStr::Fmt("NodeId %d already exists", NId));
  Node.InVId = Pool.AddEmptyV(Deg);
  Node.OutVId = Node.InVId; // same vector
  return NId;
}

template <class TNodeData, bool IsDir>
int TBigNet<TNodeData, IsDir>::AddUndirNode(int NId, const int& Deg, const TNodeData& NodeDat) {
  CAssert(! IsDir);
  if (NId == -1) { NId = MxNId;  MxNId.Val++; }
  else { MxNId = TMath::Mx(NId+1, MxNId()); }
  TNode& Node = NodeH.AddDat(NId);
  IAssertR(Node.IsUnused(), TStr::Fmt("NodeId %d already exists", NId));
  Node.InVId = Pool.AddEmptyV(Deg);
  Node.OutVId = Node.InVId; // same vector
  Node.Dat = NodeDat;
  return NId;
}

template <class TNodeData, bool IsDir>
int TBigNet<TNodeData, IsDir>::AddNode(int NId, const TIntV& InNIdV, const TIntV& OutNIdV) {
  CAssert(IsDir);
  IAssert(InNIdV.IsSorted() && OutNIdV.IsSorted());
  if (NId == -1) { NId = MxNId;  MxNId.Val++; }
  else { MxNId = TMath::Mx(NId+1, MxNId()); }
  TNode& Node = NodeH.AddDat(NId);
  IAssertR(Node.IsUnused(), TStr::Fmt("NodeId %d already exists", NId));
  Node.InVId = Pool.AddV(InNIdV);
  Node.OutVId = Pool.AddV(OutNIdV);
  return NId;
}

template <class TNodeData, bool IsDir>
int TBigNet<TNodeData, IsDir>::AddNode(int NId, const TIntV& InNIdV, const TIntV& OutNIdV, const TNodeData& NodeDat) {
  CAssert(IsDir);
  IAssert(InNIdV.IsSorted() && OutNIdV.IsSorted());
  if (NId == -1) { NId = MxNId;  MxNId.Val++; }
  else { MxNId = TMath::Mx(NId+1, MxNId()); }
  TNode& Node = NodeH.AddDat(NId);
  IAssertR(Node.IsUnused(), TStr::Fmt("NodeId %d already exists", NId));
  Node.InVId = Pool.AddV(InNIdV);
  Node.OutVId = Pool.AddV(OutNIdV);
  Node.Dat = NodeDat;
  return NId;
}

template <class TNodeData, bool IsDir>
int TBigNet<TNodeData, IsDir>::AddUndirNode(int NId, const TIntV& EdgeNIdV) {
  CAssert(! IsDir);
  IAssert(EdgeNIdV.IsSorted());
  if (NId == -1) { NId = MxNId;  MxNId.Val++; }
  else { MxNId = TMath::Mx(NId+1, MxNId()); }
  TNode& Node = NodeH.AddDat(NId);
  IAssertR(Node.IsUnused(), TStr::Fmt("NodeId %d already exists", NId));
  Node.InVId = Pool.AddV(EdgeNIdV);
  Node.OutVId = Node.InVId;
  return NId;
}

template <class TNodeData, bool IsDir>
int TBigNet<TNodeData, IsDir>::AddUndirNode(int NId, const TIntV& EdgeNIdV, const TNodeData& NodeDat) {
  CAssert(! IsDir);
  IAssert(EdgeNIdV.IsSorted());
  if (NId == -1) { NId = MxNId;  MxNId.Val++; }
  else { MxNId = TMath::Mx(NId+1, MxNId()); }
  TNode& Node = NodeH.AddDat(NId);
  IAssertR(Node.IsUnused(), TStr::Fmt("NodeId %d already exists", NId));
  Node.InVId = Pool.AddV(EdgeNIdV);
  Node.OutVId = Node.InVId;
  Node.Dat = NodeDat;
  return NId;
}

template <class TNodeData, bool IsDir>
void TBigNet<TNodeData, IsDir>::SetInNIdV(int NId, const TIntV& InNIdV) {
  TNode Node;  EAssert(IsNode(NId, Node));
  TIntV InNodesV;  Pool.GetV(Node.InVId, InNodesV);
  EAssert(InNIdV.Len() == InNodesV.Len());
  memcpy(InNodesV.BegI(), InNIdV.BegI(), sizeof(TInt)*InNIdV.Len());
}

template <class TNodeData, bool IsDir>
void TBigNet<TNodeData, IsDir>::SetOutNIdV(int NId, const TIntV& OutNIdV) {
  TNode Node;  EAssert(IsNode(NId, Node));
  TIntV OutNodesV;  Pool.GetV(Node.OutVId, OutNodesV);
  EAssert(OutNIdV.Len() == OutNodesV.Len());
  memcpy(OutNodesV.BegI(), OutNIdV.BegI(), sizeof(TInt)*OutNIdV.Len());
}

template <class TNodeData, bool IsDir>
void TBigNet<TNodeData, IsDir>::GetInNIdV(int NId, TIntV& InNIdV) const {
  TNode Node;  EAssertR(IsNode(NId, Node), TStr::Fmt("Not node: NId=%d\n", NId).CStr());
  Pool.GetV(Node.InVId, InNIdV);
}

template <class TNodeData, bool IsDir>
void TBigNet<TNodeData, IsDir>::GetOutNIdV(int NId, TIntV& OutNIdV) const {
  TNode Node;  EAssert(IsNode(NId, Node));
  Pool.GetV(Node.OutVId, OutNIdV);
}

// Node is deleted by setting edge endpoints to point to node id -1 (DelNId)
// No memory is freed
template <class TNodeData, bool IsDir>
int  TBigNet<TNodeData, IsDir>::IsolateNode(int NId) {
  TIntV OutV;
  int NDel = 0;
  // out-edges
  GetOutNIdV(NId, OutV);
  for (int i = 0; i < OutV.Len(); i++) {
    if (OutV[i] == DelNId) { break; } // because they are sorted
    // fast implementation
    const TNode& N = NodeH.GetDat(OutV[i]);
    int *InNIdV = (int *) Pool.GetValVPt(N.InVId);
    const int Deg = Pool.GetVLen(N.InVId);
    int* Val = (int *) BinSearch(InNIdV, InNIdV+Deg, NId);
    if (Val == NULL) {
      printf("BAD: Can't find: OUT: NId: %d -- OutNbhId: %d\n", NId, OutV[i]);
      continue;
    }
    IAssert(Val != 0);
    memcpy(Val, Val+1, sizeof(int)*int(InNIdV+Deg-Val));
    *(InNIdV+Deg-1) = DelNId;  NDel++;
  }
  OutV.PutAll(DelNId);
  // in-edges
  if (IsDir) {
    TIntV InV;
    GetInNIdV(NId, InV);
    for (int i = 0; i < InV.Len(); i++) {
      if (InV[i] == DelNId) { break; }
      // fast implementation
      const TNode& N = NodeH.GetDat(InV[i]);
      int *OutNIdV = (int *) Pool.GetValVPt(N.OutVId);
      const int Deg = Pool.GetVLen(N.OutVId);
      int* Val = (int *) BinSearch(OutNIdV, OutNIdV+Deg, NId);
      if (Val == NULL) {
        printf("IN: NId: %d -- InNbhId: %d\n", NId, OutV[i]);
        continue;
      }
      IAssert(Val != 0);
      memcpy(Val, Val+1, sizeof(int)*int(OutNIdV+Deg-Val));
      *(OutNIdV+Deg-1) = DelNId;  NDel++;
    }
    InV.PutAll(DelNId);
  }
  return NDel;
}

// set neigbors to point to DelNId, delete node from the node table
template <class TNodeData, bool IsDir>
int  TBigNet<TNodeData, IsDir>::DelNode(int NId) {
  const int DelEdges = IsolateNode(NId);
  NodeH.DelKey(NId);
  return DelEdges;
}

template <class TNodeData, bool IsDir>
bool TBigNet<TNodeData, IsDir>::IsIsoNode(const int& NId) const {
  if (NId == DelNId) { return true; }
  TIntV OutV;
  GetOutNIdV(NId, OutV);
  if (OutV.Empty() || OutV[0] == DelNId) { return true; }
  return false;
}

// the number deleted edges
template <class TNodeData, bool IsDir>
uint TBigNet<TNodeData, IsDir>::GetDelEdges() {
  uint DelEdges = 0;
  TIntV OutV;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    const int NId = NI.GetId();
    GetOutNIdV(NId, OutV);
    for (int i = 0; i < OutV.Len(); i++) {
      if (OutV[i] == DelNId) { DelEdges++; }
    }
  }
  return DelEdges;
}

template <class TNodeData, bool IsDir>
void TBigNet<TNodeData, IsDir>::CompactEdgePool() {
  Pool.CompactPool(DelNId);
}

template <class TNodeData, bool IsDir>
int TBigNet<TNodeData, IsDir>::AddEdge(const int& SrcNId, const int& DstNId) {
  TNode Src;  IAssert(IsNode(SrcNId, Src));
  int* OutV = (int *)Pool.GetValVPt(Src.OutVId);
  const int OutVLen = Pool.GetVLen(Src.OutVId);
  Assert(BinSearch(OutV, OutV+OutVLen, DstNId)==NULL); // no edge yet
  AddSorted(OutV, OutV+OutVLen, DstNId);
  if (! OnlySources()) {
    TNode Dst;  IAssert(IsNode(DstNId, Dst));
    int* InV = (int *)Pool.GetValVPt(Dst.InVId);
    const int InVLen = Pool.GetVLen(Dst.InVId);
    AddSorted(InV, InV+InVLen, SrcNId);
  }
  return -1; // edge id
}

template <class TNodeData, bool IsDir>
bool TBigNet<TNodeData, IsDir>::IsEdge(const int& SrcNId, const int& DstNId, const bool& Dir) const {
  TNode Src, Dst;
  if (! IsNode(SrcNId, Src)) { return false; } // no source node
  int* OutV1 = (int *)Pool.GetValVPt(Src.OutVId);
  const bool IsEdge = BinSearch(OutV1, OutV1+Pool.GetVLen(Src.OutVId), DstNId) != NULL;
  if (IsEdge && OnlySources()) { return true; }
  const bool IsDstNode = IsNode(DstNId, Dst);
  if (! IsDstNode) { return false; } // no destination node
  else if (IsEdge) { return true; } // destination and link found
  else if (! Dir) { // check for the undirected version of the edge
    int* OutV2 = (int *)Pool.GetValVPt(Dst.OutVId);
    return BinSearch(OutV2, OutV2+Pool.GetVLen(Dst.OutVId), SrcNId) != NULL; }
  else { return false; }
}

template <class TNodeData, bool IsDir>
void TBigNet<TNodeData, IsDir>::GetNIdV(TIntV& NIdV) const {
  NIdV.Reserve(GetNodes(), 0);
  for (typename TNodeH::TIter I = NodeH.BegI(); I < NodeH.EndI(); I++) {
    NIdV.Add(I->Key); }
}

template <class TNodeData, bool IsDir>
void TBigNet<TNodeData, IsDir>::SortEdgeV() {
  printf("Sorting Edges... ");
  TExeTm ExeTm;
  TIntV OutV, InV;
  int cnt = 0;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    const int NId = NI.GetId();
    GetOutNIdV(NId, OutV);
    OutV.Sort();
    if (IsDir) {
      GetInNIdV(NId, InV);
      InV.Sort();
    }
    if (++cnt % Mega(1) == 0) { printf("\r  sort:%dm  %s", cnt/Mega(1), ExeTm.GetStr()); }
  }
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    const int NId = NI.GetId();
    GetOutNIdV(NId, OutV);
    IAssert(OutV.IsSorted());
    GetInNIdV(NId, InV);
    IAssert(InV.IsSorted());
    if (++cnt % Mega(1) == 0) { printf("\r  check sorted:%dm  %s", cnt/Mega(1), ExeTm.GetStr()); }
  }
  printf("done. [%s]\n", ExeTm.GetStr());
}

// add missing nodes and in-links from a network of sources
template <class TNodeData, bool IsDir>
void TBigNet<TNodeData, IsDir>::InvertFromSources(uint ExpectNodes) {
  typedef THash<TInt, TInt> TDegHash;
  typedef int* TIntPt;
  if (ExpectNodes == 0) ExpectNodes=4*GetNodes();
  printf("\nInverting BigNet: reserved for %s nodes\n", TInt::GetMegaStr(ExpectNodes).CStr());
  CAssert(IsDir);
  IAssert(OnlySources());
  TDegHash InDegH(ExpectNodes);
  TSize NDest=0;
  // get node in-degree
  uint c=0;  TExeTm ExeTm;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++, c++) {
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      InDegH.AddDat(NI.GetOutNId(e)) += 1; }
    if (c%100000==0) printf("\r%s h:%d [%g]    ", TInt::GetMegaStr(c).CStr(), InDegH.Len(), ExeTm.GetSecs());
  }
  printf("\n Resizing NodePool: %lld -> %lld\n", uint64(Pool.Reserved()), uint64(GetEdges()));
  if (2*GetEdges() > Pool.Reserved()) {
    Pool.Reserve(2*GetEdges()); }
  // add nodes
  printf("NodeH: %s nodes, InDeg: %s nodes, Reserve: %s\n", TInt::GetMegaStr(NodeH.Len()).CStr(),
    TInt::GetMegaStr(InDegH.Len()).CStr(), TInt::GetMegaStr(ExpectNodes).CStr());
  NodeH.Reserve(ExpectNodes);
  for (TDegHash::TIter I = InDegH.BegI(); I < InDegH.EndI(); I++) {
    const int NId = I->Key, InDeg = I->Dat;
    if (! IsNode(NId)) {
      AddNode(NId, InDeg, 0); NDest++; } // new destination node, no out-links
    else {
      TNode& Node = GetNode(NId);
      IAssert(Node.InVId == 0); // no in-links
      Node.InVId = Pool.AddEmptyV(InDeg); }
  }
  InDegH.Clr(true);
  printf("Added: %lld destination nodes\n", uint64(NDest));
  printf("Graph nodes: %lld nodes\n", uint64(GetNodes()));
  // pointer to in-links vector
  THash<TInt, int*> NIdToPtH(GetNodes());
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++, c++)
    NIdToPtH.AddDat(NI.GetId(), (int *)Pool.GetValVPt(NI.GetInVId()));
  // add in-edges
  printf("Adding edges...\n");
  c = 0;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++, c++) {
    const int SrcNId = NI.GetId();
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      TIntPt& InV = NIdToPtH.GetDat(NI.GetOutNId(e));
      IAssert(*InV == TInt::Mx);
      *InV = SrcNId;  InV++;
    }
    if (c%100000==0) printf("\r%s [%g]   ", TInt::GetMegaStr(c).CStr(), ExeTm.GetSecs());
  }
  // sort in-links
  printf("\nSorting in-links...\n");
  TIntV InNIdV;  c = 0;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++, c++) {
    Pool.GetV(NI.GetInVId(), InNIdV);
    int nid = NI.GetId();
    InNIdV.Sort();
    if (c%100000==0) printf("\r%s [%g]    ", TInt::GetMegaStr(c).CStr(), ExeTm.GetSecs());
  }
  printf("\r...done [%g]\n", ExeTm.GetSecs());
  Flags.Excl(gfSources);
}

template <class TNodeData, bool IsDir>
void TBigNet<TNodeData, IsDir>::Rewire(TRnd& Rnd) {
  uint64 NDup=0, NResolve=0, NAddit=0, cnt = 0;
  TExeTm ExeTm;
  IAssertR(! IsDir, "Only undirected networks are supported");
  printf("Rewiring the network... 1");
  Pool.ShuffleAll(Rnd);  printf("[%s]\n", ExeTm.GetStr());
  //Pool.ShuffleAll(Rnd);  printf(" done [%s]\n", ExeTm.GetStr());
  printf("  sorting edges...\n");
  SortEdgeV(); // sort individual edge vectors
  printf(" done [%s]\n", ExeTm.GetStr());
  // remove duplicates and self edges
  printf("  removing duplicates...\n");
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++, cnt++) {
    const int VId = NI.GetOutVId();
    int Len = Pool.GetVLen(VId);
    int* V = (int *)Pool.GetValVPt(VId);
    for (int i = 0; i < Len-1 && V[i]!=DelNId; i++) {
      if (V[i] == V[i+1] || V[i]==NI.GetId()) {
        memcpy(V+i, V+i+1, sizeof(int)*(Len-i-1)); i--;
        V[Len-1] = DelNId;  NDup++;
      }
    }
    if (Len > 0 && V[Len-1]==NI.GetId()) { V[Len-1] = DelNId;  NDup++; }
    if (cnt % Mega(10) == 0) { printf(".");  fflush(stdout); }
  }
  printf("\n    %I64u duplicate edges removed [%s]\n", NDup, ExeTm.GetStr());
  if (OnlySources()) { return; }
  // resolve one way edges
  printf("  resolving one way edges...\n"); cnt=0; fflush(stdout);
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++, cnt++) {
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      if (NI.GetOutNId(e) == DelNId) { continue; } // skip deleted nodes
      const int InVId = GetNode(NI.GetOutNId(e)).InVId;
      const int InVLen = Pool.GetVLen(InVId); IAssert(InVLen>=0 && InVLen < Mega(3));
      int* InV = (int *) Pool.GetValVPt(InVId);
      int* Val = (int *) BinSearch2(InV, InV+InVLen, NI.GetId());
      if (*Val == NI.GetId()) { continue; } // points back, everything is ok
      if (InVLen>0 && InV[InVLen-1] == DelNId) { // there is space at the end, insert
        IAssert((InVLen-(Val-InV)-1) >= 0);
        memmove(Val+1, Val, sizeof(int)*(InVLen-(Val-InV)-1));
        *Val = NI.GetId();
      } else { // the other end could point at us but it doesn't
        memmove(NI.OutNIdV+e, NI.OutNIdV+e+1, sizeof(int)*(NI.GetOutDeg()-e-1));
        NI.OutNIdV[NI.GetOutDeg()-1]=DelNId;  e--;
      }
      NResolve++;
    }
    if (cnt % Mega(10) == 0) { printf(".");  fflush(stdout); }
  }
  printf("\n    %I64u resolved edges [%s]\n", NResolve, ExeTm.GetStr());
  // check if there are two nodes that still have space and are not yet connected and connect them
  printf("  filling empty edge slots...\n");
  TIntPrV SlotNIdV;
  THash<TInt, TInt> SlotNIdH;
  int CumSlots=0;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    if (NI.GetOutNId(NI.GetOutDeg()-1) == DelNId) {
      int NSlots = 0;
      for (int s = NI.GetOutDeg()-1; s >= 0; s--) {
        if (NI.GetOutNId(s) == DelNId) { NSlots++; }
        else { break; }
      }
      SlotNIdV.Add(TIntPr(NI.GetId(), NSlots));
      SlotNIdH.AddDat(NI.GetId(), NSlots);
      CumSlots+=NSlots;
    }
  }
  printf("    %d nodes, with %d spokes available, %d edges\n", SlotNIdH.Len(), CumSlots, CumSlots/2);
  TIntV NIdV;  SlotNIdH.GetKeyV(NIdV);
  for (int ni1 = 0; ni1 < NIdV.Len(); ni1++) {
    const int nid = NIdV[ni1];
    if (! SlotNIdH.IsKey(nid) || SlotNIdH.GetDat(nid) == 0) { continue; }
    const int NI1Slots = SlotNIdH.GetDat(nid);
    if ((SlotNIdH.GetMxKeyIds() - SlotNIdH.Len())/double(SlotNIdH.GetMxKeyIds()) > 0.5) { SlotNIdH.Defrag(); }
    TNodeI NI  = GetNI(nid);
    for (int NTries = 0; NTries < 4*NI1Slots && NI.GetOutNId(NI.GetOutDeg()-1) == DelNId; NTries++) {
      const int nid2 = SlotNIdH.GetKey(SlotNIdH.GetRndKeyId(Rnd));
      if (nid == nid2) { continue; }
      TNodeI NI2  = GetNI(nid2);
      // insert the edge
      if (NI2.GetOutNId(NI2.GetOutDeg()-1)==DelNId && ! NI2.IsInNId(NI.GetId())) {
        int *V1 = (int *) BinSearch2(NI.OutNIdV, NI.OutNIdV+NI.GetOutDeg(), NI2.GetId());
        int *V2 = (int *) BinSearch2(NI2.InNIdV, NI2.InNIdV+NI2.GetInDeg(), NI.GetId());
        if (*V1 != DelNId) { memmove(V1+1, V1, sizeof(int)*(NI.GetOutDeg()-(V1-NI.OutNIdV)-1)); }
        if (*V2 != DelNId) { memmove(V2+1, V2, sizeof(int)*(NI2.GetInDeg()-(V2-NI2.InNIdV)-1)); }
        *V1 = NI2.GetId();  *V2 = NI.GetId();
        NAddit++;
        SlotNIdH.GetDat(nid)--;  SlotNIdH.GetDat(nid2)--;
      }
      if (SlotNIdH.GetDat(nid2) == 0) { SlotNIdH.DelKey(nid2); continue; }
      if (SlotNIdH.GetDat(nid) == 0) { SlotNIdH.DelKey(nid); break; }
    }
    if (ni1 % Mega(1) == 0) { printf(".");  fflush(stdout); }
  }
  printf("    %I64u edges added.\nDONE. TOTAL REWIRE TIME [%s]\n\n", NAddit, ExeTm.GetStr());
}

template <class TNodeData, bool IsDir>
PNGraph TBigNet<TNodeData, IsDir>::GetNGraph(const bool& RenumberNodes) const {
  IAssert(RenumberNodes == false);
  PNGraph Graph = TNGraph::New();
  Graph->Reserve(GetNodes(), 0);
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    Graph->AddNode(NI.GetId(), Pool, NI.GetInVId(), NI.GetOutVId());
  }
  return Graph;
}

template <class TNodeData, bool IsDir>
PNGraph TBigNet<TNodeData, IsDir>::GetSubNGraph(const TIntV& NIdV) const {
  PNGraph Graph = TNGraph::New(NIdV.Len(), 0);
  // add nodes
  for (int i = 0; i < NIdV.Len(); i++) {
    Graph->AddNode(NIdV[i]); }
  // reserve node in- and out-degree
  for (int i = 0; i < NIdV.Len(); i++) {
    const int SrcNId = NIdV[i];
    const TNodeI NI = GetNI(SrcNId);
    int InDeg=0, OutDeg=0;
    for (int e = 0; e < NI.GetInDeg(); e++) { if (Graph->IsNode(NI.GetInNId(e))) InDeg++; }
    for (int e = 0; e < NI.GetOutDeg(); e++) { if (Graph->IsNode(NI.GetOutNId(e))) OutDeg++; }
    Graph->ReserveNIdInDeg(SrcNId, InDeg);
    Graph->ReserveNIdOutDeg(SrcNId, OutDeg);
  }
  // add edges
  for (int i = 0; i < NIdV.Len(); i++) {
    const int SrcNId = NIdV[i];
    const TNodeI NI = GetNI(SrcNId);
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      const int DstNId = NI.GetOutNId(e);
      if (Graph->IsNode(DstNId)) {
        Graph->AddEdge(SrcNId, DstNId); }
    }
  }
  return Graph;
}

template <class TNodeData, bool IsDir>
TPt<TBigNet<TNodeData, IsDir> > TBigNet<TNodeData, IsDir>::GetSubGraph(const TIntV& NIdV, const bool& RenumberNodes) const {
  const bool isDir = IsDir, onlySources = HasFlag(gfSources);
  TSize Edges=0;
  // find in- and out-degree
  TSparseHash<TInt, TIntPr> NIdDegH(NIdV.Len());
  for (int i = 0; i < NIdV.Len(); i++) { NIdDegH.AddDat(NIdV[i]); }
  for (int i = 0; i < NIdV.Len(); i++) {
    const TNodeI NI = GetNI(NIdV[i]);
    int InDeg=0, OutDeg=0;
    for (int n = 0; n < NI.GetOutDeg(); n++) {
      if (NIdDegH.IsKey(NI.GetOutNId(n))) OutDeg++; }
    if (! onlySources && isDir) {
      for (int n = 0; n < NI.GetInDeg(); n++) {
        if (NIdDegH.IsKey(NI.GetInNId(n))) InDeg++; }
    }
    Edges += OutDeg;
    NIdDegH.GetDat(NIdV[i]) = TIntPr(InDeg, OutDeg);
  }
  // make network
  typedef TBigNet<TNodeData, IsDir> TBNet;
  TPt<TBNet> NewNetPt = TBNet::New(NIdV.Len(), Edges, HasFlag(gfDirected));
  TBNet& NewNet = *NewNetPt;
  NewNet.Flags = Flags;
  // add nodes
  if (isDir || onlySources) {
    for (int i = 0; i < NIdV.Len(); i++) {
      const TIntPr& Deg = NIdDegH.GetDat(NIdV[i]);
      NewNet.AddNode(NIdV[i], Deg.Val1, Deg.Val2, GetNDat(NIdV[i])); } // also copy the node data
  } else {
    for (int i = 0; i < NIdV.Len(); i++) {
      const TIntPr& Deg = NIdDegH.GetDat(NIdV[i]);
      NewNet.AddUndirNode(NIdV[i], Deg.Val2, GetNDat(NIdV[i])); } // also copy the node data
  }
  // add edges
  for (int i = 0; i < NIdV.Len(); i++) {
    int NId = NIdV[i];
    const TNodeI NI = GetNI(NId);
    int *NIdVPt = (int *) NewNet.GetOutNIdVPt(NId);
    int deg = 0;
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      const int Dst = NI.GetOutNId(e);
      if (NewNet.IsNode(Dst)) {
        *NIdVPt = Dst;  NIdVPt++;  deg++; }
    }
    EAssert(deg == NIdDegH.GetDat(NId).Val2);
    if (isDir && ! onlySources) {
      EAssert((NI.GetInVId()==NI.GetOutVId() && NI.GetInVId()==0)
        || (NI.GetInVId() != NI.GetOutVId()));
      int * NIdVPt = (int *) NewNet.GetInNIdVPt(NId);
      int deg = 0;
      for (int e = 0; e < NI.GetInDeg(); e++) {
        const int Dst = NI.GetInNId(e);
        if (NewNet.IsNode(Dst)) {
          *NIdVPt = Dst;  NIdVPt++;  deg++; }
      }
      EAssert(deg == NIdDegH.GetDat(NId).Val1);
    }
  }
  return NewNetPt;
}

template <class TNodeData, bool IsDir>
void TBigNet<TNodeData, IsDir>::GetSubGraph(const TIntV& NIdV, TBigNet* NewNetPt, const bool& RenumberNodes) const {
  printf("Set subgraph on %d nodes\n", NIdV.Len()); TExeTm ExeTm;
  const bool isDir = IsDir, onlySources = HasFlag(gfSources);
  TSize Edges=0;
  // find in- and out-degree
  THash<TInt, TIntPr> NIdDegH(NIdV.Len());
  for (int i = 0; i < NIdV.Len(); i++) { NIdDegH.AddDat(NIdV[i]); }
  for (int i = 0; i < NIdV.Len(); i++) {
    const TNodeI NI = GetNI(NIdV[i]);
    int InDeg=0, OutDeg=0;
    for (int n = 0; n < NI.GetOutDeg(); n++) {
      if (NIdDegH.IsKey(NI.GetOutNId(n))) OutDeg++; }
    if (! onlySources && isDir) {
      for (int n = 0; n < NI.GetInDeg(); n++) {
        if (NIdDegH.IsKey(NI.GetInNId(n))) InDeg++; }
    }
    Edges += OutDeg;
    NIdDegH.GetDat(NIdV[i]) = TIntPr(InDeg, OutDeg);
  }
  // make network
  //typedef TBigNet<TNodeData, IsDir> TBNet;
  //TPt<TBNet> NewNetPt = TBNet::New(NIdV.Len(), Edges, HasFlag(gfDirected));
  NewNetPt->Reserve(NIdV.Len(), Edges);
  TBigNet<TNodeData, IsDir>& NewNet = *NewNetPt;
  NewNet.Flags = Flags;
  TIntSet NIdMap;
  // add nodes
  if (! RenumberNodes) {
    if (isDir || onlySources) {
      for (int i = 0; i < NIdV.Len(); i++) {
        const TIntPr& Deg = NIdDegH.GetDat(NIdV[i]);
        NewNet.AddNode(NIdV[i], Deg.Val1, Deg.Val2, GetNDat(NIdV[i])); } // also copy the node data
    } else {
      for (int i = 0; i < NIdV.Len(); i++) {
        const TIntPr& Deg = NIdDegH.GetDat(NIdV[i]);
        NewNet.AddUndirNode(NIdV[i], Deg.Val2, GetNDat(NIdV[i])); } // also copy the node data
    }
  } else { // renumber nodes
    NIdMap.Gen(NIdV.Len());
    for (int i = 0; i < NIdV.Len(); i++) { NIdMap.AddKey(NIdV[i]);  }
    if (isDir || onlySources) {
      for (int i = 0; i < NIdV.Len(); i++) {
        const TIntPr& Deg = NIdDegH.GetDat(NIdV[i]);
        NewNet.AddNode(NIdMap.GetKeyId(NIdV[i]), Deg.Val1, Deg.Val2, GetNDat(NIdV[i])); } // also copy the node data
    } else {
      for (int i = 0; i < NIdV.Len(); i++) {
        const TIntPr& Deg = NIdDegH.GetDat(NIdV[i]);
        NewNet.AddUndirNode(NIdMap.GetKeyId(NIdV[i]), Deg.Val2, GetNDat(NIdV[i])); } // also copy the node data
    }
  }
  // add edges
  for (int i = 0; i < NIdV.Len(); i++) {
    int NId = NIdV[i];
    const TNodeI NI = GetNI(NId);
    int *NIdVPt = (int *) NewNet.GetOutNIdVPt(RenumberNodes?NIdMap.GetKeyId(NId):NId);
    int deg = 0;
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      const int Dst = RenumberNodes?NIdMap.GetKeyId(NI.GetOutNId(e)):NI.GetOutNId(e);
      if (NewNet.IsNode(Dst)) {
        *NIdVPt = Dst;  NIdVPt++;  deg++; }
    }
    EAssert(deg == NIdDegH.GetDat(NId).Val2);
    if (isDir && ! onlySources) {
      EAssert((NI.GetInVId()==NI.GetOutVId() && NI.GetInVId()==0)
        || (NI.GetInVId() != NI.GetOutVId()));
      int * NIdVPt = (int *) NewNet.GetInNIdVPt(RenumberNodes?NIdMap.GetKeyId(NId):NId);
      int deg = 0;
      for (int e = 0; e < NI.GetInDeg(); e++) {
        const int Dst = RenumberNodes?NIdMap.GetKeyId(NI.GetInNId(e)):NI.GetInNId(e);
        if (NewNet.IsNode(Dst)) {
          *NIdVPt = Dst;  NIdVPt++;  deg++; }
      }
      EAssert(deg == NIdDegH.GetDat(NId).Val1);
    }
  }
  printf("  %u edges [%s]\n", uint(Edges), ExeTm.GetStr());
}

template <class TNodeData, bool IsDir>
void TBigNet<TNodeData, IsDir>::Reserve(const int& Nodes, const TSize& Edges) {
  NodeH.Gen(TMath::Mx(int(1.1*Nodes), 100));
  Pool = TPool(TMath::Mx(Edges, (TSize) Mega(1)), Mega(100), true);
}

// check that in- and out-edges matchs, the table is sorted and so on
template <class TNodeData, bool IsDir>
bool TBigNet<TNodeData, IsDir>::IsOk() const {
  // check the node pool
  TIntV ValV; TExeTm ExeTm;
  printf("Is ok network:\n  Check Vec...");
  for (uint id = 1; id < Pool.GetVecs(); id++) {
    Pool.GetV(id, ValV);
    if (! ValV.Empty()) {
      EAssert((0<=ValV[0] && ValV[0]<MxNId) || ValV[0]==DelNId);
      try {
      for (int i = 1; i < ValV.Len(); i++) {
        //if (ValV[i]==DelNId) { continue; }
        // sorted & no duplicates & no empty values (can have deleted nodes)
        EAssertR((ValV[i-1]<ValV[i]) || (ValV[i-1]==ValV[i] && ValV[i]==DelNId),
          TStr::Fmt("nid1: %d nid2:%d", ValV[i-1],ValV[i]));
        EAssertR((0<=ValV[i] && ValV[i]<MxNId) || ValV[i]==DelNId,
          TStr::Fmt("nid1: %dm mxNId: %d", ValV[i], MxNId));
        if (! OnlySources()) {
          EAssertR(IsNode(ValV[i]) || ValV[i]==DelNId,
            TStr::Fmt("nid1: %dm mxNId: %d", ValV[i])); } // every link is a node
      }
      } catch (PExcept Except){
        printf("  %s\n", Except->GetStr().CStr());
        printf("  vec id: %d, lenght: %d\n", id, ValV.Len());
        for (int i = 1; i < ValV.Len(); i++) {
          printf("  %d", ValV[i]);
          if (!((ValV[i-1]<ValV[i]) || (ValV[i-1]==ValV[i] && ValV[i]==DelNId))) { printf("*"); }
        }
        printf("\n");
        return false;
      }
    }
    if (id % 10000 == 0) {
      printf("\r  %dk / %dk [%s]", id/1000, Pool.GetVecs()/1000, ExeTm.GetStr()); }
  }
  printf("[%s]\n  Check Edges...\n", ExeTm.GetStr());
  // check edges
  int ErrCnt = 0;
  if (! OnlySources()) {
    int Cnt=0;
    for (TNodeI NI = BegNI(); NI < EndNI(); NI++, Cnt++) {
      // nodes that point to NI, have it on out-list
      for (int e = 0; e < NI.GetInDeg(); e++) {
        if (NI.GetInNId(e) == DelNId) { continue; } // skip deleted nodes
    if (! IsEdge(NI.GetInNId(e), NI.GetId())) {
      printf("\nno edge: %d --> %d", NI.GetInNId(e), NI.GetId());
      printf("NId: %d   deg %d,%d\n", NI.GetId(), NI.GetOutDeg(), NI.GetInDeg());
      for (int i = 0; i < NI.GetInDeg(); i++) { printf("  %d", NI.GetInNId(i)); }
      TNodeI NI2 = GetNI(NI.GetInNId(e));
      printf("\nNId2: %d   deg %d,%d\n", NI2.GetId(), NI2.GetOutDeg(), NI2.GetInDeg());
      for (int j = 0; j < NI2.GetOutDeg(); j++) { printf("  %d", NI2.GetOutNId(j)); }
      printf("\n");
      ErrCnt++;
    }
        //EAssertR(IsEdge(NI.GetInNId(e), NI.GetId()),
        //  TStr::Fmt("no edge: %d --> %d", NI.GetInNId(e), NI.GetId()));
    }
      // nodes NI points to, have it on in-list
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        if (NI.GetOutNId(e) == DelNId) { continue; }
        const int InVId = GetNode(NI.GetOutNId(e)).InVId;
        int* DstInV = (int *)Pool.GetValVPt(InVId);
    if (BinSearch(DstInV, DstInV+Pool.GetVLen(InVId), NI.GetId()) == NULL) {
      printf("no edge: %d --> %d", NI.GetId(), NI.GetInNId(e));
      printf("NId: %d   deg %d,%d\n", NI.GetId(), NI.GetOutDeg(), NI.GetInDeg());
      for (int i = 0; i < NI.GetOutDeg(); i++) { printf("  %d", NI.GetOutNId(i)); }
      TNodeI NI2 = GetNI(NI.GetOutNId(e));
      printf("\nNId2: %d   deg %d,%d\n", NI2.GetId(), NI2.GetOutDeg(), NI2.GetInDeg());
      for (int j = 0; j < NI2.GetInDeg(); j++) { printf("  %d", NI2.GetInNId(j)); }
      printf("\n");  ErrCnt++;
    }
        //EAssertR(BinSearch(DstInV, DstInV+Pool.GetVLen(InVId), NI.GetId()) != NULL,
        //  TStr::Fmt("no edge: %d --> %d", NI.GetId(), NI.GetInNId(e)));
    }
    if (ErrCnt > 5) { FailR("error count too large!"); }
      if (Cnt % 100000 == 0) {
        printf("\r%s [%s]", TInt::GetMegaStr(Cnt).CStr(), ExeTm.GetStr()); }
    }
    printf("\r%s [%s]\n", TInt::GetMegaStr(Cnt).CStr(), ExeTm.GetStr());
  }
  return true;
}

template <class TNodeData, bool IsDir>
void TBigNet<TNodeData, IsDir>::Dump(const TStr& Desc) const {
  if (! Desc.Empty()) { printf("\n%s (%d, %u):\n", Desc.CStr(), GetNodes(), GetEdges()); }
  else { printf("\nNodes: %d,  Edges: %u\n", GetNodes(), GetEdges()); }
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    printf("%d]\n  IN %d]", NI.GetId(), NI.GetInDeg());
    for (int i=0; i<NI.GetInDeg(); i++) { printf(" %d", NI.GetInNId(i)); }
    if (IsDir) {
      printf("\n  OUT %d]", NI.GetOutDeg());
      for (int i=0; i<NI.GetOutDeg(); i++) { printf(" %d", NI.GetOutNId(i)); }
    }
    printf("\n");
  }
}

// header: <Nodes, Edges, IsDir>
// format: undirected <NId, Dat, OutDeg, OutNodeV>
// format: directed <NId, Dat, OutDeg, OutNodeV, InDeg, InNodeV>
template <class TNodeData, bool IsDir>
void TBigNet<TNodeData, IsDir>::SaveForDisk(const TStr& OutFNm) const {
  const bool IsDirected = IsDir;
  TFOut FOut(OutFNm);
  FOut.Save(GetNodes());
  FOut.Save(GetEdges());
  FOut.Save(IsDirected);
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    FOut.Save(NI.GetId());
    NI.GetDat().Save(FOut);
    FOut.Save(NI.GetOutDeg());
    for (int i = 0; i < NI.GetOutDeg(); i++) {
      FOut.Save(NI.GetOutNId(i)); }
    if (IsDirected) {
      FOut.Save(NI.GetInDeg());
      for (int i = 0; i < NI.GetInDeg(); i++) {
        FOut.Save(NI.GetInNId(i)); }
    }
  }
}

// skip the edge pool and only load the node data hash table
template <class TNodeData, bool IsDir>
void TBigNet<TNodeData, IsDir>::LoadNodeDatH(const TStr& InFNm, TNodeH& NodeH) {
  TFIn SIn(InFNm);
  TInt MxNId(SIn);
  TB32Set Flags(SIn);
  printf("skipping Pool...\n");
  TBool FastCopy(SIn);
  uint64 _GrowBy, _MxVals, _Vals;
  SIn.Load(_GrowBy); SIn.Load(_MxVals);  SIn.Load(_Vals);
  TInt EmptyVal(SIn);
  int Tmp;
  for (TSize ValN = 0; ValN < _Vals; ValN++) { SIn.Load(Tmp); }
  TInt MxVals(SIn), Vals(SIn);
  uint64 Offset;
  for (int ValN = 0; ValN < Vals; ValN++) { SIn.Load(Offset); }
  printf("loading Hode hash table...\n");
  NodeH.Load(SIn);
}

// Save the network without loading it. Save the node hash table as THash or TSparseHash
template <class TNodeData, bool IsDir>
void TBigNet<TNodeData, IsDir>::SaveToDisk(const TStr& InFNm, const TStr& OutFNm, const bool& SaveSparseHash) {
  TFIn FIn(InFNm);
  TFOut FOut(OutFNm);
  { TInt MxNId(FIn);  MxNId.Save(FOut);
  TB32Set Flags(FIn);  Flags.Save(FOut);
  TPool Pool(FIn);  Pool.Save(FOut); }
  { TNodeH NodeH(FIn);
  if (! SaveSparseHash) {
    THash<TInt, TNode> NewH(NodeH.Len(), true);
    for (typename TNodeH::TIter I = NodeH.BegI(); I < NodeH.EndI(); I++) {
      NewH.AddDat(I->Key, I->Dat);
    }
    NewH.Save(FOut);
  } else {
    FailR("Not implemented");
  } }
}
