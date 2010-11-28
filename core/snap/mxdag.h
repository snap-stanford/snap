#ifndef snap_max_dag_h
#define snap_max_dag_h

namespace TSnap {

/////////////////////////////////////////////////
// Maximum Directed Acyclic Graph
int EvalMxDagOrdering(const PNGraph& Graph, const TIntV& NIdV);
int EvalMxDagOrdering(const PNGraph& Graph, const TIntH& NIdPosH);
int EvalMxDagNodeSwap(const PNGraph& Graph, const TIntH& NIdPosH, const int& NId1, const int& NId2);
int GetGreedyMxDag(const PNGraph& Graph, const int& NIters, TIntV& NIdOrderV);
int GetGreedyMxDag(const PNGraph& Graph, const int& NIters=Mega(1));

} // namespace TSnap

#endif
