#ifndef snap_motifcluster_h
#define snap_motifcluster_h
#include "Snap.h"

// TODO: Implement algorithm for motif adjacency matrix for
//       1. semi-clique
//       2. undirected triangles
enum MotifType {
  M1,         // u  --> v, v  --> w, w  --> u
  M2,         // u <--> v, v  --> w, w  --> u
  M3,         // u <--> v, v <--> w, w  --> u
  M4,         // u <--> v, v <--> w, w <--> u
  M5,         // u  --> v, v  --> w, u  --> w
  M6,         // u <--> v, w  --> u, w  --> v
  M7,         // u <--> v, u  --> w, v  --> w
  M8,         // u  --> v, u  --> w
  M9,         // u  --> v, w  --> u
  M10,        // v  --> u, w  --> u
  M11,        // u <--> v, u  --> w
  M12,        // u <--> v, w  --> u
  M13,        // u <--> v, u <--> w
  bifan,      // u  --> w, u  --> x, v  --> w, v  --> x
  semiclique, // u  --  v, u  --  w, u  --  x, v  --  w, v  --  x
};

// Container for sweep cut data.
class TSweepCut {
public:
  TIntV cluster;        // Set of indices forming cluster
  double cond;          // conductance of the cluster
  double eig;           // Eigenvalue of Fiedler vector
  TFltV sweep_profile;  // Sweep profile: kth entry is conductance(Sk)
  TCnCom component;     // connected component that the cut runs on
};

class MotifCluster {
 public:
  // For a directed graph, fills the weights vector with the weights in the
  // motif adjacency matrix.  Specifically, weights[i][j] is the number of times
  // i and j co-occurr in an instance of the motif for any i < j (only stores
  // the lower triangular part of the matrix).
  void MotifAdjacency(PNGraph graph, MotifType motif,
		      TVec< THash<TInt, TInt> >& weights);

  // Given a weighted network, compute a cut of the graph using the Fiedler vector
  // and a sweep cut.  Results are stored in the sweepcut data structure.  The
  // variables tol and maxiter control the stopping tolerance and maximum number
  // of iterations used by the eigensolver.
  void SpectralCut(const TVec< THash<TInt, TInt> >& weights, TSweepCut& sweepcut,
		   double tol=1e-14, int maxiter=300);


  // Compute the Fiedler vector for the nonnegative matrix W and store the result
  // in fvec using a power method.  The algorithm terminates when the current
  // residual || Ax^{k} - \lambda^{(k)} x^{(k)} || < tol or when maxiter
  // iterations of the power method have completed.
  double FiedlerVector(const TSparseColMatrix& W, TFltV& fvec,
		       double tol=1e-14, int maxiter=300);

  // Check if three nodes form an instance of a directed triangle motif.
  bool IsMotifM1(PNGraph graph, int u, int v, int w);
  bool IsMotifM2(PNGraph graph, int u, int v, int w);
  bool IsMotifM3(PNGraph graph, int u, int v, int w);
  bool IsMotifM4(PNGraph graph, int u, int v, int w);
  bool IsMotifM5(PNGraph graph, int u, int v, int w);
  bool IsMotifM6(PNGraph graph, int u, int v, int w);
  bool IsMotifM7(PNGraph graph, int u, int v, int w);
  // Check if three nodes form a directed wedge motif with a specified center.
  bool IsMotifM8(PNGraph graph, int center, int v, int w);
  bool IsMotifM9(PNGraph graph, int center, int v, int w);
  bool IsMotifM10(PNGraph graph, int center, int v, int w);
  bool IsMotifM11(PNGraph graph, int center, int v, int w);
  bool IsMotifM12(PNGraph graph, int center, int v, int w);
  bool IsMotifM13(PNGraph graph, int center, int v, int w);  

  // Check if u --> v is a unidirectional edge (u --> v but no v --> u).
  bool IsUnidirEdge(PNGraph graph, int u, int v);

  // Check if u and v form a bidirectional edge (u <--> v).
  bool IsBidirEdge(PNGraph graph, int u, int v);

  // Check if there is no edge between u and v
  bool IsNoEdge(PNGraph graph, int u, int v);  

 private:
  // Fills order vector so that order[i] < order[j] implies that
  //    degree(i) <= degree(j),
  // where degree is the number of unique incoming and outgoing
  // neighbors (reciprocated edge neighbors are only counted once).
  void DegreeOrdering(PNGraph graph, TIntV& order);

  // Handles MotifAdjacency() functionality for directed triangle motifs.
  void TriangleMotifAdjacency(PNGraph graph, MotifType motif,
                              TVec< THash<TInt, TInt> >& weights);

  // Handles MotifAdjacency() functionality for directed wedges.
  void WedgeMotifAdjacency(PNGraph graph, MotifType motif,
			   TVec< THash<TInt, TInt> >& weights);  

  // Handles MotifAdjacency() functionality for the bifan motif.
  void BifanMotifAdjacency(PNGraph graph, TVec< THash<TInt, TInt> >& weights);

  // Handles MotifAdjacency() functionality for the semi-clique.  
  void SemicliqueAdjacency(PUNGraph graph, TVec< THash<TInt, TInt> >& weights);

  // Increments weight on (i, j) by one
  void IncrementWeight(int i, int j, TVec< THash<TInt, TInt> >& weights);
};

#endif
