#ifndef Snap_GraphCounter
#define Snap_GraphCounter

#include "Snap.h"

/////////////////////////////////////////////////
// Directed 3 or 4 graph counter
class TD34GraphCounter {
public:
  TD34GraphCounter(int GraphSz); // IAssert(GraphSz==3 || GraphSz==4);
public:
  void operator()(const PNGraph &G, const TIntV &sg);
  //THash<TInt,TUInt64> &GraphCounters() { return m_graphCounters; }
  int Len() const  { return m_graphCounters.Len(); }
  int GetId(const int& i) const { return m_graphCounters.GetKey(i); }
  uint64 GetCnt(const  int& GraphId) const { return m_graphCounters.GetDat(GraphId); }
  PNGraph GetGraph(const int& GraphId) const;
private:
  THash<TInt,TInt> m_graphMaps;
  THash<TInt,TUInt64> m_graphCounters;
  int m_subGraphSize;
};

/////////////////////////////////////////////////
// Directed graph counter
class TDGraphCounter{
public:
  void operator()(const PNGraph &G, const TIntV &sg);
  THash<TUInt64,TUInt64> &GraphCounters() { return m_graphCounters; }
private:
  THash<TUInt64,TUInt64> m_graphMaps;
  THash<TUInt64,TUInt64> m_graphCounters;
};

/////////////////////////////////////////////////
// Directed ghash graph counter
class TDGHashGraphCounter{
public:
  TDGHashGraphCounter() : m_graphs(false) {}
public:
  void operator()(const PNGraph &G, const TIntV &sg);
  TGHash<TUInt64> &Graphs() { return m_graphs; }
private:
  TGHash<TUInt64> m_graphs;
};

/////////////////////////////////////////////////
// Directed 3 graph
class TD3Graph {
public:
  static inline int getId(const PNGraph &G, const TIntV &sg);
public:
  static int m_numOfGraphs;
  static int m_graphIds[];
};

/////////////////////////////////////////////////
// Directed 4 graph
class TD4Graph {
public:
  static inline int getId(const PNGraph &G, const TIntV &sg);
public:
  static int m_numOfGraphs;
  static int m_graphIds[];
};

/////////////////////////////////////////////////
// TSNodeTest
class TSNodeTest{
public:
  void operator()(const PNGraph &G, const TIntV &sg) {
    for(int i=0; i<sg.Len(); i++) printf("%d, ", sg[i].Val);
    printf("\n");
  }
};

/////////////////////////////////////////////////
// TGraphEnumUtils
class TGraphEnumUtils{
private:
  static void GetNormalizedMap(const PNGraph &G, THash<TInt,TInt> &map);
  static void GetPermutations(TIntV &v, int start, TVec<TIntV> &perms);
public:
  static inline bool IsEdge(const PNGraph &G, int SrcNId, int DstNId) { return G->GetNodeC(SrcNId).IsOutNId(DstNId); }
  static void GetEdges(uint64 graphId, int nodes, TVec<TPair<int,int> > &edges);
public:
  static void GetNormalizedGraph(const PNGraph &G, PNGraph &nG);
  static void GetIndGraph(const PNGraph &G, const TIntV &sg, PNGraph &indG);
  static void GetGraph(uint64 graphId, int nodes, PNGraph &G);
public:
  static void GetIsoGraphs(uint64 graphId, int nodes, TVec<PNGraph> &isoG);
  static void GetIsoGraphs(const PNGraph &G, TVec<PNGraph> &isoG);
public:
  static uint64 GraphId(const PNGraph &G);
  static uint64 GraphId(const PNGraph &G, const TIntV &sg);
  static uint64 GetMinAndGraphIds(const TVec<PNGraph> &isoG, TVec<uint64> &graphIds);
};

#endif
