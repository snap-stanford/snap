#ifndef snap_motifcluster_h
#define snap_motifcluster_h
#include "Snap.h"

// TODO: Implement algorithm for motif adjacency matrix for
//       1. directed wedges
//       2. bi-fan
//       3. semi-clique
//       4. undirected triangles
enum MotifType {
  M1,  // u  --> v, v  --> w, w  --> u
  M2,  // u <--> v, v  --> w, w  --> u
  M3,  // u <--> v, v <--> w, w  --> u
  M4,  // u <--> v, v <--> w, w <--> u
  M5,  // u  --> v, v  --> w, u  --> w
  M6,  // u <--> v, w  --> u, w  --> v
  M7,  // u <--> v, u  --> w, v  --> w
};

class MotifCluster {
 public:
  // For a directed graph, fills the weights vector with the weights in the
  // motif adjacency matrix.  Specifically, weights[i][j] is the number of times
  // i and j co-occurr in an instance of the motif for any i < j (only stores
  // the lower triangular part of the matrix).
  void MotifAdjacency(PNGraph graph, MotifType motif,
		      TVec< THash<TInt, TInt> >& weights);

 private:
  // Check if u --> v is a unidirectional edge (u --> v but no v --> u).
  bool IsUnidirEdge(PNGraph graph, int u, int v);

  // Check if u and v form a bidirectional edge (u <--> v).
  bool IsBidirEdge(PNGraph graph, int u, int v);

  // Check if three nodes form an instance of a particular motif.
  bool IsMotifM1(PNGraph graph, int u, int v, int w);
  bool IsMotifM2(PNGraph graph, int u, int v, int w);
  bool IsMotifM3(PNGraph graph, int u, int v, int w);
  bool IsMotifM4(PNGraph graph, int u, int v, int w);
  bool IsMotifM5(PNGraph graph, int u, int v, int w);
  bool IsMotifM6(PNGraph graph, int u, int v, int w);
  bool IsMotifM7(PNGraph graph, int u, int v, int w);

  // Fills order vector so that order[i] < order[j] implies that
  //    degree(i) <= degree(j),
  // where degree is the number of unique incoming and outgoing
  // neighbors (reciprocated edge neighbors are only counted once).
  void DegreeOrdering(PNGraph graph, TIntV& order);

  // Handles MotifAdjacency() functionality for directed triangle motifs.
  void TriangleMotifAdjacency(PNGraph graph, MotifType motif,
                              TVec< THash<TInt, TInt> >& weights);

  // Increments weight on (i, j) by one
  void IncrementWeight(int i, int j, TVec< THash<TInt, TInt> >& weights);
};

#endif
