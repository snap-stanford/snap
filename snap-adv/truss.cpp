#include "stdafx.h"
#include "truss.h"
#include <vector>
#include <set>
using namespace std;  // for std::vector, std::set

/// Get the trussness for each edge.
void GetTruss(const PUNGraph& Graph, TIntPrIntH& Truss){
  TSnap::DelSelfEdges(Graph);  // remove loops
  TIntPrIntH NumberOfTrianglesForEachEdge;
  GetNumberOfTrianglesForEachEdge(Graph, NumberOfTrianglesForEachEdge);
  GetTrussnessForEachEdge(Graph, NumberOfTrianglesForEachEdge, Truss);
}

/// Get all triangles from undirected graph. Adapted from temporalmotifs.cpp.
void GetNumberOfTrianglesForEachEdge(const PUNGraph& Graph,
  TIntPrIntH& NumberOfTrianglesForEachEdge) {
  // omp_set_num_threads(4);
  // Get the nodes.
  TIntV Nodes;
  GetAllNodes(Graph, Nodes);

  // Initialize NumberOfTrianglesForEachEdge: set the key of the edge pair,
  // and set the value to zero.
  for (TUNGraph::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    NumberOfTrianglesForEachEdge.AddDat(TIntPr(EI.GetSrcNId(),
    EI.GetDstNId())) = 0;
  }

  // Get degree ordering of the graph.
  int MxNodes = Graph->GetMxNId();
  TVec<TIntPr> Degrees(MxNodes);
  Degrees.PutAll(TIntPr(0, 0));
  // Set the degree of a node to be the number of nodes adjacent to the node
  // in the undirected graph.
  for (int node_id = 0; node_id < Nodes.Len(); node_id++) {
    int SrcNode = Nodes[node_id];
    TIntV Nbrs;
    GetAllNeighbors(Graph, SrcNode, Nbrs);
    Degrees[SrcNode] = TIntPr(Nbrs.Len(), SrcNode);
  }
  Degrees.Sort();
  TIntV Order = TIntV(MxNodes);

  for (int i = 0; i < Order.Len(); i++) {
    Order[Degrees[i].Val2] = i;
  }

  // Get triangles centered at a given node where that node is the smallest in
  // the degree ordering.
  for (int node_id = 0; node_id < Nodes.Len(); node_id++) {
    int SrcNode = Nodes[node_id];
    int SrcNodePos = Order[SrcNode];

    // Get all neighbors who come later in the ordering.
    TIntV Nbrs;
    GetAllNeighbors(Graph, SrcNode, Nbrs);
    TIntV NbrsHigher;
    for (int i = 0; i < Nbrs.Len(); i++) {
      int nbr = Nbrs[i];
      if (Order[nbr] > SrcNodePos) { NbrsHigher.Add(nbr); }
    }

    for (int ind1 = 0; ind1 < NbrsHigher.Len(); ind1++) {
      for (int ind2 = ind1 + 1; ind2 < NbrsHigher.Len(); ind2++) {
        int Dst1 = NbrsHigher[ind1];
        int Dst2 = NbrsHigher[ind2];
        // Check for triangle formation.
        if (Graph->IsEdge(Dst1, Dst2) || Graph->IsEdge(Dst2, Dst1)) {
          // Order the edge for the hash map.
          TIntPr Edge1 = SrcNode < Dst1 ? TIntPr(SrcNode, Dst1) :
          TIntPr(Dst1, SrcNode);
          TIntPr Edge2 = SrcNode < Dst2 ? TIntPr(SrcNode, Dst2) :
          TIntPr(Dst2, SrcNode);
          TIntPr Edge3 = Dst1 < Dst2 ? TIntPr(Dst1, Dst2) :
          TIntPr(Dst2, Dst1);

          // Increment support for each edge in this triangle.
          NumberOfTrianglesForEachEdge.GetDat(Edge1)++;
          NumberOfTrianglesForEachEdge.GetDat(Edge2)++;
          NumberOfTrianglesForEachEdge.GetDat(Edge3)++;
        }
      }
    }
  }
}

/// Get the trussness for each edge (from number of triangles for each edge).
void GetTrussnessForEachEdge(const PUNGraph& Graph,
  TIntPrIntH& NumberOfTrianglesForEachEdge, TIntPrIntH& Truss) {
  // Get max possible truss value = max entry in NumberOfTrianglesForEachEdge.
  int MxTri = 0;
  for (TIntPrIntH::TIter it = NumberOfTrianglesForEachEdge.BegI();
  it < NumberOfTrianglesForEachEdge.EndI(); it++) {
    int Value = it.GetDat();
    if (MxTri < Value) { MxTri = Value; }
  }

  // The vector of sets. Each level of the vector is a set of edges initially
  // at that level of support, where support is # of triangles the edge is in.
  vector< set<TIntPr> > Sets(MxTri + 1);

  // Add each edge to its appropriate level of support.
  for (TIntPrIntH::TIter it = NumberOfTrianglesForEachEdge.BegI();
  it < NumberOfTrianglesForEachEdge.EndI(); it++) {
    TIntPr Edge = it.GetKey();  // the edge
    int Value = it.GetDat();
    Sets[Value].insert(Edge);
  }

  // Record the trussness of edges at level 0.
  for (set<TIntPr>::iterator it=Sets[0].begin(); it != Sets[0].end(); ++it) {
    Truss.AddDat(*it, 0);
  }

  // Initialize number and set of completed edges.
  int NumCompleted = Sets[0].size();
  set<TIntPr> Completed;  // set of completed edges

  // Move through the sets, one level at a time, starting at first level.
  for (int level = 1; level <= MxTri; level++) {
    while (!Sets[level].empty()) {
      TIntPr Seed = *Sets[level].begin();  // pull out the first edge
      Sets[level].erase(Seed);  // remove the first element

      // Find neighbors.
      int FromNId = Seed.GetVal1();
      int ToNId = Seed.GetVal2();
      TIntV CmnNbrs;
      TSnap::GetCmnNbrs(Graph, FromNId, ToNId, CmnNbrs);

      for (int i = 0; i < CmnNbrs.Len(); i++) {
        int n = CmnNbrs[i];  // the common neighbor
        TIntPr Edge1 = FromNId < n ? TIntPr(FromNId, n) : TIntPr(n, FromNId);
        TIntPr Edge2 = ToNId < n ? TIntPr(ToNId, n) : TIntPr(n, ToNId);
        bool Edge1Complete = (Completed.find(Edge1) != Completed.end());
        bool Edge2Complete = (Completed.find(Edge2) != Completed.end());
        if (!Edge1Complete && !Edge2Complete) {
          // Demote this edge, if higher than current level.
          if (NumberOfTrianglesForEachEdge.GetDat(Edge1) > level) {
            int newLevel = NumberOfTrianglesForEachEdge.GetDat(Edge1)--;
            Sets[newLevel].insert(Edge1);
            Sets[newLevel + 1].erase(Edge1);  // the old level
          }
          // Demote this edge, if higher than current level.
          if (NumberOfTrianglesForEachEdge.GetDat(Edge2) > level) {
            int newLevel = NumberOfTrianglesForEachEdge.GetDat(Edge2)--;
            Sets[newLevel].insert(Edge2);
            Sets[newLevel + 1].erase(Edge2);  // the old level
          }
        }
      }
      // Record this edge; its level is its trussness.
      Truss.AddDat(Seed, level);
      Completed.insert(Seed);
      NumCompleted++;
    }  // end while for a single level
  }  // end for-loop over the levels
}

/// Get all the Nodes (undirected). Adapted from temporalmotifs.cpp.
void GetAllNodes(const PUNGraph& Graph, TIntV& Nodes) {
  for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    Nodes.Add(NI.GetId());
  }
}

/// Get all the neighbors (undirected). Adapted from temporalmotifs.cpp.
void GetAllNeighbors(const PUNGraph& Graph, int node, TIntV& Nbrs) {
  TUNGraph::TNodeI NI = Graph->GetNI(node);
  for (int i = 0; i < NI.GetDeg(); i++) {
    Nbrs.Add(NI.GetNbrNId(i));
  }
}
