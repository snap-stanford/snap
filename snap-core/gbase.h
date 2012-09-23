/////////////////////////////////////////////////
// Defines
#define Kilo(n) (1000*(n))
#define Mega(n) (1000*1000*(n))
#define Giga(n) (1000*1000*1000*(n))

/////////////////////////////////////////////////
/// Graph Flags, used for quick testing of graph types.
/// This is very useful for testing graph properties at compile time for partial template specialization as well as compile time assert (CAssert)
typedef enum {
  gfUndef=0,    ///< default value, no flags
  gfDirected,   ///< directed graph (TNGraph, TNEGraph), else graph is undirected(TUNGraph)
  gfMultiGraph, ///< have explicit edges (multigraph): TNEGraph, TNodeEdgeNet
  gfNodeDat,    ///< network with data on nodes
  gfEdgeDat,    ///< network with data on edges
  gfSources,    ///< nodes only store out-edges (but not in-edges). See TBigNet
  gfBipart,     ///< bipartite graph
  gfMx          ///< sentinel, last value for iteration
} TGraphFlag;

namespace TSnap {

/// Tests (at compile time) if the graph is directed.
template <class TGraph> struct IsDirected   { enum { Val = 0 }; };
/// Tests (at compile time) if the graph is a multigraph with multiple edges between the same nodes.
template <class TGraph> struct IsMultiGraph { enum { Val = 0 }; };
/// Tests (at compile time) if the graph is a network with data on nodes.
template <class TGraph> struct IsNodeDat    { enum { Val = 0 }; };
/// Tests (at compile time) if the graph is a network with data on edges.
template <class TGraph> struct IsEdgeDat    { enum { Val = 0 }; };
/// Tests (at compile time) if the nodes store only out-edges, but not in-edges.
template <class TGraph> struct IsSources    { enum { Val = 0 }; };
/// Tests (at compile time) if the graph is a bipartite graph type.
template <class TGraph> struct IsBipart     { enum { Val = 0 }; };

/// For quick testing of the properties of the graph/network object (see TGraphFlag).
#define HasGraphFlag(TGraph, Flag) \
  ((Flag)==gfDirected ? TSnap::IsDirected<TGraph::TNet>::Val : \
  (Flag)==gfMultiGraph ? TSnap::IsMultiGraph<TGraph::TNet>::Val : \
  (Flag)==gfNodeDat ? TSnap::IsNodeDat<TGraph::TNet>::Val : \
  (Flag)==gfEdgeDat ? TSnap::IsEdgeDat<TGraph::TNet>::Val : \
  (Flag)==gfSources ? TSnap::IsSources<TGraph::TNet>::Val : \
  (Flag)==gfBipart ? TSnap::IsBipart<TGraph::TNet>::Val : 0)

/// Tests (at complile time) whether TDerivClass is derived from TBaseClass
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

/////////////////////////////////////////////////
// Graph Base

/// Returns a string representation of a flag.
TStr GetFlagStr(const TGraphFlag& GraphFlag);
/// Prints basic graph statistics.
/// @param Fast true: only computes basic statistics (that can be computed fast). For more extensive information (and longer execution times) set Fast = false.
template <class PGraph> void PrintInfo(const PGraph& Graph, const TStr& Desc="", const TStr& OutFNm="", const bool& Fast=true);

/////////////////////////////////////////////////
// Implementation

// Forward declaration, definition in triad.h
template <class PGraph> int GetTriads(const PGraph& Graph, int& ClosedTriads, int& OpenTriads, int SampleNodes=-1);

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
    if (! Fast || Graph->GetNodes() < 1000) {
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
/// Fast Queue used by the TBreathFS (uses memcpy to move objects TVal around).
template <class TVal>
class TSnapQueue {
private:
  TInt MxFirst; // how often we move the queue to the start of the array
  TInt First, Last;
  TVec<TVal> ValV;
public:
  TSnapQueue() : MxFirst(1024), First(0), Last(0), ValV(MxFirst, 0) { }
  /// Constructor that reserves enough memory for a queue with MxVals elements.
  TSnapQueue(const int& MxVals) : MxFirst(1024+MxVals/10), First(0), Last(0), ValV(TInt::GetMx(MxFirst, MxVals), 0) { }
  TSnapQueue(const int& MxVals, const int& MaxFirst) : MxFirst(MaxFirst),
    First(0), Last(0), ValV(TInt::GetMx(MxFirst, MxVals), 0) { }
  TSnapQueue(const TSnapQueue& Queue) : MxFirst(Queue.MxFirst), First(Queue.First), Last(Queue.Last), ValV(Queue.ValV) { }
  /// Constructor that loads the queue from a (binary) stream SIn.
  explicit TSnapQueue(TSIn& SIn): MxFirst(SIn), First(SIn), Last(SIn), ValV(SIn) { }
  /// Saves the queue to a (binary) stream SOut.
  void Save(TSOut& SOut) const { MxFirst.Save(SOut); First.Save(SOut); Last.Save(SOut); ValV.Save(SOut); }

  TSnapQueue& operator=(const TSnapQueue& Queue) { if (this != &Queue) { MxFirst=Queue.MxFirst;
    First=Queue.First; Last=Queue.Last; ValV=Queue.ValV; } return *this; }
  /// Returns the value of the ValN element in the queue, but does not remove the element.
  const TVal& operator[](const int& ValN) const { return ValV[First+ValN]; }

  /// Deletes all elements from the queue.
  void Clr(const bool& DoDel=true) { ValV.Clr(DoDel);  First=Last=0; }
  void Gen(const int& MxVals, const int& MaxFirst=1024) {
    MxFirst=MaxFirst; First=Last=0; ValV.Gen(MxVals, 0); }

  /// Tests whether the queue is empty (contains no elements).
  bool Empty() const {return First==Last;}
  /// Returns the number of elements in the queue.
  int Len() const {return Last-First;}
  /// Returns the location of the first element in the queue.
  int GetFirst() const { return First; }
  /// Returns the location of the last element in the queue.
  int GetLast() const { return Last; }
  int Reserved() const { return ValV.Reserved(); }

  /// Returns the value of the first element in the queue, but does not remove the element.
  const TVal& Top() const { return ValV[First]; }
  /// Removes the first element from the queue.
  void Pop() { First++;
    if (First==Last) { ValV.Clr(false); First=Last=0; } }
  /// Adds an element at the end of the queue.
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
/// Union Find class (Disjoint-set data structure).
/// For more info see: http://en.wikipedia.org/wiki/Disjoint-set_data_structure)
class TUnionFind {
private:
  THash<TInt, TIntPr> KIdSetH; // key id to (parent, rank)
public:
  /// Returns the parent of element Key.
  TInt& Parent(const int& Key) { return KIdSetH.GetDat(Key).Val1; }
  /// Returns the rank of element Key.
  TInt& Rank(const int& Key) { return KIdSetH.GetDat(Key).Val2; }
public:
  TUnionFind() : KIdSetH() { }
  /// Constructor that reserves enough memory for ExpectKeys elements.
  TUnionFind(const int& ExpectKeys) : KIdSetH(ExpectKeys, true) { }
  TUnionFind(const TUnionFind& UnionFind) : KIdSetH(UnionFind.KIdSetH) { }
  TUnionFind& operator = (const TUnionFind& UF) { KIdSetH=UF.KIdSetH; return *this; }

  /// Returns the number of elements in the structure.
  int Len() const { return KIdSetH.Len(); }
  /// Returns true if the structure contains element Key.
  bool IsKey(const int& Key) const { return KIdSetH.IsKey(Key); }
  /// Returns the element KeyN.
  int GetKeyI(const int& KeyN) const { return KIdSetH.GetKey(KeyN); }
  /// Returns the set that contains element Key.
  int Find(const int& Key);
  /// Adds an element Key to the structure.
  int Add(const int& Key) { KIdSetH.AddDat(Key, TIntPr(-1, 0));  return Key; }
  /// Merges sets with elements Key1 and Key2.
  void Union(const int& Key1, const int& Key2);
  /// Returns true if elements Key1 and Key2 are in the same set.
  bool IsSameSet(const int& Key1, const int& Key2) {
    return Find(Key1) == Find(Key2); }
  /// Prints out the structure to standard output.
  void Dump();
};
