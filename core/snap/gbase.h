/////////////////////////////////////////////////
// Defines
#define Kilo(n) (1000*(n))
#define Mega(n) (1000*1000*(n))
#define Giga(n) (1000*1000*1000*(n))

/////////////////////////////////////////////////
// Graph Flags
typedef enum {
  gfUndef=0,
  gfDirected,   // directed graph (TNGraph, TNEGraph), else graph is undirected(TUNGraph)
  gfMultiGraph, // have explicit edges (multigraph): TNEGraph, TNodeEdgeNet
  gfNodeDat,    // network with data on nodes
  gfEdgeDat,    // network with data on edges
  gfSources,    // nodes only store out-edges (but not in-edges). See TBigNet
  gfMx
} TGraphFlag;

namespace TSnap {

template <class TGraph> struct IsDirected   { enum { Val = 0 }; };
template <class TGraph> struct IsMultiGraph { enum { Val = 0 }; };
template <class TGraph> struct IsNodeDat    { enum { Val = 0 }; };
template <class TGraph> struct IsEdgeDat    { enum { Val = 0 }; };
template <class TGraph> struct IsSources    { enum { Val = 0 }; };

template <class PGraph> int GetTriads(const PGraph& Graph, int& ClosedTriads, int& OpenTriads, int SampleNodes=-1);

// test whether TDerivClass is derived from TBaseClass
template<class TDerivClass, class TBaseClass>
class IsDerivedFrom {
private:
  struct Yes { char a[1]; };
  struct No { char a[10]; };
  static Yes Test(TBaseClass*);
  static No Test(...);         // undefined
public:
  enum { Val = sizeof(Test(static_cast<TDerivClass*>(0))) == sizeof(Yes) ? 1 : 0 };
};

//template<class TGraph>
//bool __HasFlag(const TGraphFlag& Flag) {
//  switch(Flag) {
//    case gfDirected :  return IsDirected<TGraph>::Val;
//    case gfMultiGraph : return IsMultiGraph<TGraph>::Val;
//    case gfNodeDat :   return IsNodeDat<TGraph>::Val;
//    case gfEdgeDat :   return IsEdgeDat<TGraph>::Val;
//    case gfSources :   return IsSources<TGraph>::Val;
//    default : return 0;
//  };
//}

} // namespace TSnap

#define HasGraphFlag(TGraph, Flag) \
  ((Flag)==gfDirected ? TSnap::IsDirected<TGraph::TNet>::Val : \
  (Flag)==gfMultiGraph ? TSnap::IsMultiGraph<TGraph::TNet>::Val : \
  (Flag)==gfNodeDat ? TSnap::IsNodeDat<TGraph::TNet>::Val : \
  (Flag)==gfEdgeDat ? TSnap::IsEdgeDat<TGraph::TNet>::Val : \
  (Flag)==gfSources ? TSnap::IsSources<TGraph::TNet>::Val : 0)

/////////////////////////////////////////////////
// Graph Base
namespace TSnap {

TStr GetFlagStr(const TGraphFlag& GraphFlag);
template <class PGraph> void PrintInfo(const PGraph& Graph, const TStr& Desc="", const TStr& OutFNm="", const bool& Fast=true);

/////////////////////////////////////////////////
// Implementation

// print basic graph info
template <class PGraph>
void PrintInfo(const PGraph& Graph, const TStr& Desc, const TStr& OutFNm, const bool& Fast) {
  int BiDirEdges=0, ZeroNodes=0, ZeroInNodes=0, ZeroOutNodes=0, SelfEdges=0, NonZIODegNodes=0;
  THash<TIntPr, TInt> UniqDirE, UniqUnDirE;
  FILE *F = stdout;
  if (! OutFNm.Empty()) F = fopen(OutFNm.CStr(), "wt");
  if (! Desc.Empty()) { fprintf(F, "%s:", Desc.CStr()); }
  else { fprintf(F, "Graph:"); }
  for (int f = gfUndef; f < gfMx; f++) {
    if (HasGraphFlag(typename PGraph::TObj, TGraphFlag(f))) {
      fprintf(F, " %s", TSnap::GetFlagStr(TGraphFlag(f)).CStr()); }
  }
  // calc stat
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    if (NI.GetDeg()==0) ZeroNodes++;
    if (NI.GetInDeg()==0) ZeroInNodes++;
    if (NI.GetOutDeg()==0) ZeroOutNodes++;
    if (NI.GetInDeg()!=0 && NI.GetOutDeg()!=0) NonZIODegNodes++;
    if (! Fast) {
      const int NId = NI.GetId();
      for (int edge = 0; edge < NI.GetOutDeg(); edge++) {
        const int DstNId = NI.GetOutNId(edge);
        if (Graph->IsEdge(DstNId, NId)) BiDirEdges++;
        if (NId == DstNId) SelfEdges++;
        UniqDirE.AddKey(TIntPr(NId, DstNId));
        UniqUnDirE.AddKey(TIntPr(TInt::GetMn(NId, DstNId), TInt::GetMx(NId, DstNId)));
      }
    }
  }
  int Closed=0, Open=0;
  if (! Fast) { TSnap::GetTriads(Graph, Closed, Open); }
  // print info
  fprintf(F, "\n");
  fprintf(F, "  Nodes:                    %d\n", Graph->GetNodes());
  fprintf(F, "  Edges:                    %d\n", Graph->GetEdges());
  fprintf(F, "  Zero Deg Nodes:           %d\n", ZeroNodes);
  fprintf(F, "  Zero InDeg Nodes:         %d\n", ZeroInNodes);
  fprintf(F, "  Zero OutDeg Nodes:        %d\n", ZeroOutNodes);
  fprintf(F, "  NonZero In-Out Deg Nodes: %d\n", NonZIODegNodes);
  if (! Fast) {
    fprintf(F, "  Unique directed edges:    %d\n", UniqDirE.Len());
    fprintf(F, "  Unique undirected edges:  %d\n", UniqUnDirE.Len());
    fprintf(F, "  Self Edges:               %d\n", SelfEdges);
    fprintf(F, "  BiDir Edges:              %d\n", BiDirEdges);
    fprintf(F, "  Closed triangles          %d\n", Closed);
    fprintf(F, "  Open triangles            %d\n", Open);
    fprintf(F, "  Frac. of closed triads    %f\n", Closed/double(Closed+Open));
  }
  if (! OutFNm.Empty()) { fclose(F); }
}

}  // namespace TSnap

/////////////////////////////////////////////////
// Fast Queue (uses memcpy to move object around)
template <class TVal>
class TSnapQueue {
private:
  TInt MxFirst; // how often we move the queue to the start of the array
  TInt First, Last;
  TVec<TVal> ValV;
public:
  TSnapQueue() : MxFirst(1024), First(0), Last(0), ValV(MxFirst, 0) { }
  TSnapQueue(const int& MxVals) : MxFirst(1024+MxVals/10), First(0), Last(0), ValV(TInt::GetMx(MxFirst, MxVals), 0) { }
  TSnapQueue(const int& MxVals, const int& MaxFirst) : MxFirst(MaxFirst),
    First(0), Last(0), ValV(TInt::GetMx(MxFirst, MxVals), 0) { }
  TSnapQueue(const TSnapQueue& Queue) : MxFirst(Queue.MxFirst), First(Queue.First), Last(Queue.Last), ValV(Queue.ValV) { }
  explicit TSnapQueue(TSIn& SIn): MxFirst(SIn), First(SIn), Last(SIn), ValV(SIn) { }
  void Save(TSOut& SOut) const { MxFirst.Save(SOut); First.Save(SOut); Last.Save(SOut); ValV.Save(SOut); }

  TSnapQueue& operator=(const TSnapQueue& Queue) { if (this != &Queue) { MxFirst=Queue.MxFirst;
    First=Queue.First; Last=Queue.Last; ValV=Queue.ValV; } return *this; }
  const TVal& operator[](const int& ValN) const { return ValV[First+ValN]; }

  void Clr(const bool& DoDel=true) { ValV.Clr(DoDel);  First=Last=0; }
  void Gen(const int& MxVals, const int& MaxFirst=1024) {
    MxFirst=MaxFirst; First=Last=0; ValV.Gen(MxVals, 0); }

  bool Empty() const {return First==Last;}
  int Len() const {return Last-First;}
  int GetFirst() const { return First; }
  int GetLast() const { return Last; }
  int Reserved() const { return ValV.Reserved(); }

  const TVal& Top() const { return ValV[First]; }
  void Pop() { First++;
    if (First==Last) { ValV.Clr(false); First=Last=0; } }
  void Push(const TVal& Val) {
    if (First>0 && (First > MxFirst || ValV.Len() == ValV.Reserved()) && ! ValV.Empty()) {
      //printf("[move cc queue.Len:%d-->%d]", ValV.Len(),Len()); TExeTm Tm;
      memmove(ValV.BegI(), ValV.GetI(First), sizeof(TVal)*Len());
      ValV.Del(Len(), ValV.Len()-1);  Last-=First;  First=0;
      //printf("[%s]\n", Tm.GetStr()); fflush(stdout);
    }
    //if (ValV.Len() == ValV.Reserved()){ printf("[resizeCCQ]"); fflush(stdout); }
    Last++;  ValV.Add(Val);
  }
};

/////////////////////////////////////////////////
// Union Find (Disjoint-set data structure)
// http://en.wikipedia.org/wiki/Disjoint-set_data_structure)
class TUnionFind {
private:
  THash<TInt, TIntPr> KIdSetH; // key id to (parent, rank)
public:
  TInt& Parent(const int& Key) { return KIdSetH.GetDat(Key).Val1; }
  TInt& Rank(const int& Key) { return KIdSetH.GetDat(Key).Val2; }
public:
  TUnionFind() : KIdSetH() { }
  TUnionFind(const int& ExpectKeys) : KIdSetH(ExpectKeys, true) { }
  TUnionFind(const TUnionFind& UnionFind) : KIdSetH(UnionFind.KIdSetH) { }
  TUnionFind& operator = (const TUnionFind& UF) { KIdSetH=UF.KIdSetH; return *this; }

  int Len() const { return KIdSetH.Len(); }
  bool IsKey(const int& Key) const { return KIdSetH.IsKey(Key); }
  int GetKeyI(const int& KeyN) const { return KIdSetH.GetKey(KeyN); }
  int Find(const int& Key);
  int Add(const int& Key) { KIdSetH.AddDat(Key, TIntPr(-1, 0));  return Key; }
  void Union(const int& Key1, const int& Key2);
  bool IsSameSet(const int& Key1, const int& Key2) {
    return Find(Key1) == Find(Key2); }
  void Dump();
};
