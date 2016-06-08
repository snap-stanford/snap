#include "stdafx.h"
#include "motifcluster.h"

#include <algorithm>

// debugging
#include <cassert>
#include <iostream>

/////////////////////////////////////////////////
// Subgraph type checking
bool MotifCluster::IsUnidirEdge(PNGraph graph, int u, int v) {
  return graph->IsEdge(u, v) && !graph->IsEdge(v, u);
}

bool MotifCluster::IsBidirEdge(PNGraph graph, int u, int v) {
  return graph->IsEdge(u, v) && graph->IsEdge(v, u);
}

bool MotifCluster::IsMotifM1(PNGraph graph, int u, int v, int w) {
  return ((IsUnidirEdge(graph, u, v) && IsUnidirEdge(graph, v, w) && IsUnidirEdge(graph, w, u)) ||
	  (IsUnidirEdge(graph, u, w) && IsUnidirEdge(graph, w, v) && IsUnidirEdge(graph, v, u)));
}

bool MotifCluster::IsMotifM2(PNGraph graph, int u, int v, int w) {
  return ((IsBidirEdge(graph, u, v) && IsUnidirEdge(graph, u, w) && IsUnidirEdge(graph, w, v)) ||
	  (IsBidirEdge(graph, u, v) && IsUnidirEdge(graph, w, u) && IsUnidirEdge(graph, v, w)) ||
	  (IsBidirEdge(graph, u, w) && IsUnidirEdge(graph, u, v) && IsUnidirEdge(graph, v, w)) ||
	  (IsBidirEdge(graph, u, w) && IsUnidirEdge(graph, v, u) && IsUnidirEdge(graph, w, v)) ||
	  (IsBidirEdge(graph, v, w) && IsUnidirEdge(graph, v, u) && IsUnidirEdge(graph, u, w)) ||
	  (IsBidirEdge(graph, v, w) && IsUnidirEdge(graph, u, v) && IsUnidirEdge(graph, w, u)));
}

bool MotifCluster::IsMotifM3(PNGraph graph, int u, int v, int w) {
  if ((IsBidirEdge( graph, u, v) && IsBidirEdge( graph, v, w)) &&
      (IsUnidirEdge(graph, u, w) || IsUnidirEdge(graph, w, u))) { return true; }
  if ((IsBidirEdge( graph, u, w) && IsBidirEdge( graph, w, v)) &&
      (IsUnidirEdge(graph, u, v) || IsUnidirEdge(graph, v, u))) { return true; }
  if ((IsBidirEdge( graph, w, u) && IsBidirEdge( graph, u, v)) &&
      (IsUnidirEdge(graph, w, v) || IsUnidirEdge(graph, v, w))) { return true; }
  return false;
}

bool MotifCluster::IsMotifM4(PNGraph graph, int u, int v, int w) {
  return IsBidirEdge(graph, u, v) && IsBidirEdge(graph, u, w) && IsBidirEdge(graph, v, w);
}

bool MotifCluster::IsMotifM5(PNGraph graph, int u, int v, int w) {
  if ((IsUnidirEdge(graph, u, v) && IsUnidirEdge(graph, u, w)) && 
      (IsUnidirEdge(graph, v, w) || IsUnidirEdge(graph, w, v))) { return true; }
  if ((IsUnidirEdge(graph, v, u) && IsUnidirEdge(graph, v, w)) && 
      (IsUnidirEdge(graph, u, w) || IsUnidirEdge(graph, w, u))) { return true; }
  if ((IsUnidirEdge(graph, w, v) && IsUnidirEdge(graph, w, u)) && 
      (IsUnidirEdge(graph, v, u) || IsUnidirEdge(graph, u, v))) { return true; }
  return false;
}

bool MotifCluster::IsMotifM6(PNGraph graph, int u, int v, int w) {
  return ((IsUnidirEdge(graph, u, v) && IsUnidirEdge(graph, u, w) && IsBidirEdge(graph, v, w)) ||
	  (IsUnidirEdge(graph, v, u) && IsUnidirEdge(graph, v, w) && IsBidirEdge(graph, u, w)) ||
	  (IsUnidirEdge(graph, w, u) && IsUnidirEdge(graph, w, v) && IsBidirEdge(graph, u, v)));
}

bool MotifCluster::IsMotifM7(PNGraph graph, int u, int v, int w) {
  return ((IsUnidirEdge(graph, v, u) && IsUnidirEdge(graph, w, u) && IsBidirEdge(graph, v, w)) ||
	  (IsUnidirEdge(graph, u, v) && IsUnidirEdge(graph, w, v) && IsBidirEdge(graph, u, w)) ||
	  (IsUnidirEdge(graph, u, w) && IsUnidirEdge(graph, v, w) && IsBidirEdge(graph, u, v)));
}

/////////////////////////////////////////////////
// Triangle weighting
void MotifCluster::DegreeOrdering(PNGraph graph, TIntV& order) {
  // Note: this works the best when the nodes are numbered 0, 1, ..., num_nodes - 1.
  int max_nodes = graph->GetMxNId() + 1;
  TVec< TKeyDat<TInt, TInt> > degrees(max_nodes);
  degrees.PutAll(TKeyDat<TInt, TInt>(0, 0));
  // Set the degree of a node to be the number of nodes adjacent to the node in
  // the undirected graph.
  for (TNGraph::TNodeI NI = graph->BegNI(); NI < graph->EndNI(); NI++) {
    int src = NI.GetId();
    int num_nbrs = NI.GetOutDeg();
    // For each incoming edge that is not an outgoing edge, include it.
    for (int i = 0; i < NI.GetInDeg(); ++i) {
      int dst = NI.GetInNId(i);
      if (!NI.IsOutNId(dst)) {
	++num_nbrs;
      }
    }
    degrees[src] = TKeyDat<TInt, TInt>(num_nbrs, src);
  }

  degrees.Sort();
  order = TIntV(max_nodes);
  for (int i = 0; i < order.Len(); ++i) {
    order[degrees[i].Dat] = i;
  }
}

void MotifCluster::IncrementWeight(int i, int j, TVec< THash<TInt, TInt> >& weights) {
  int minval = std::min(i, j);
  int maxval = std::max(i, j);
  THash<TInt, TInt>& edge_weights = weights[minval];
  edge_weights(maxval) += 1;
}

void MotifCluster::TriangleMotifAdjacency(PNGraph graph, MotifType motif,
					  TVec< THash<TInt, TInt> >& weights) {
  std::cout << "About to get order..." << std::endl;
  TIntV order;
  DegreeOrdering(graph, order);
  std::cout << "Got order... " << order.Len() << std::endl;

  weights = TVec< THash<TInt, TInt> >(order.Len());
  for (TNGraph::TNodeI NI = graph->BegNI(); NI < graph->EndNI(); NI++) {
    int src = NI.GetId();
    int src_pos = order[src];
    
    // Get all neighbors who come later in the ordering
    TIntV neighbors_higher;
    for (int i = 0; i < NI.GetOutDeg(); i++) {
      int nbr = NI.GetOutNId(i);
      if (order[nbr] > src_pos) {
	neighbors_higher.Add(nbr);
      }
    }
    for (int i = 0; i < NI.GetInDeg(); i++) {
      int nbr = NI.GetInNId(i);
      if (!NI.IsOutNId(nbr) && order[nbr] > src_pos) {
	neighbors_higher.Add(nbr);
      }
    }

    for (int ind1 = 0; ind1 < neighbors_higher.Len(); ind1++) {
      for (int ind2 = ind1 + 1; ind2 < neighbors_higher.Len(); ind2++) {
	int dst1 = neighbors_higher[ind1];
	int dst2 = neighbors_higher[ind2];
	// Check for triangle formation
	if (graph->IsEdge(dst1, dst2) || graph->IsEdge(dst2, dst1)) {
	  bool motif_occurs = false;
	  switch (motif) {
	  case M1:
	    motif_occurs = IsMotifM1(graph, src, dst1, dst2);
	    break;
	  case M2:
	    motif_occurs = IsMotifM2(graph, src, dst1, dst2);
	    break;
	  case M3:
	    motif_occurs = IsMotifM3(graph, src, dst1, dst2);
	    break;
	  case M4:
	    motif_occurs = IsMotifM4(graph, src, dst1, dst2);
	    break;
	  case M5:
	    motif_occurs = IsMotifM5(graph, src, dst1, dst2);
	    break;
	  case M6:
	    motif_occurs = IsMotifM6(graph, src, dst1, dst2);
	    break;
	  case M7:
	    motif_occurs = IsMotifM7(graph, src, dst1, dst2);
	    break;
	  default:
	    TExcept::Throw("Unknown directed triangle motif");
	  }
	  // Increment weights of the triad (src, dst1, dst2) if it occurs.
	  if (motif_occurs) {
	    IncrementWeight(src,  dst1, weights);
	    IncrementWeight(src,  dst2, weights);
	    IncrementWeight(dst1, dst2, weights);
	  }
	}
      }
    }
  }
}

/////////////////////////////////////////////////
// Motif adjacency formation
void MotifCluster::MotifAdjacency(PNGraph graph, MotifType motif,
				  TVec< THash<TInt, TInt> >& weights) {
  switch (motif) {
  case M1:
  case M2:
  case M3:
  case M4:
  case M5:
  case M6:
  case M7:
    TriangleMotifAdjacency(graph, motif, weights);
    break;
  default:
    TExcept::Throw("Unknown motif type");
  }
}
