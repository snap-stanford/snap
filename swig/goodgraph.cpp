#include "Snap.h"

// TODO: move declarations to goodgraph.h
// RS 2013/08/19, this code should go to snap-core

using namespace TSnap;
  
typedef TVec<TInt, int> TIntV;
typedef TVec<TIntV, int> TIntIntVV;

TUNGraph TPrGraph(PUNGraph G) {
  return *G;
};

//int accept_array(int array[]) {
//  
//  for (int i=0; i < 10; i++)
//    printf("array[%d] = %d\n", i, array[i]);
//
//  return 0;
//}

template<class PGraph>
double PercentDegree(const PGraph& Graph, const int Threshold=0) {

    int Cnt = 0;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++)
  {
    if (NI.GetDeg() >= Threshold) Cnt++;
  }

  return (double)Cnt / (double) Graph->GetNodes();
}

template<class PGraph>
int NodesGTEDegree(const PGraph& Graph, const int Threshold=0) {
  
  int Cnt = 0;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI();
       NI++)
  {
    if (NI.GetDeg() >= Threshold) Cnt++;
  }
  
  return Cnt;
}

template<class PGraph>
int MxDegree(const PGraph& Graph) {
  
  int MaxDeg = 0;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    if (NI.GetDeg() > MaxDeg) {
      MaxDeg = NI.GetDeg();
    }
  }
  
  return MaxDeg;
}

template<class PGraph>
double PercentMxWcc(const PGraph& Graph) {
  
  return GetMxWccSz(Graph);
}

template<class PGraph>
double PercentMxScc(const PGraph& Graph) {
  
  PGraph MxSccSz = GetMxScc(Graph);
  
  return (double) MxSccSz->GetNodes() / (double) Graph->GetNodes();
}

