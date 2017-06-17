#ifndef snap_truss_h
#define snap_truss_h

#include "Snap.h"

void GetTruss(const PUNGraph& Graph, TIntPrIntH& Truss);
void GetNumberOfTrianglesForEachEdge(const PUNGraph& Graph, TIntPrIntH&
  numberOfTrianglesForEachEdge);
void GetTrussnessForEachEdge(const PUNGraph& Graph,
    TIntPrIntH& numberOfTrianglesForEachEdge, TIntPrIntH& truss);
void GetAllNodes(const PUNGraph& Graph, TIntV& nodes);
void GetAllNeighbors(const PUNGraph& Graph, int node, TIntV& nbrs);

#endif // snap_truss_h
