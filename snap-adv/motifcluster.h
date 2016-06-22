#ifndef snap_motifcluster_h
#define snap_motifcluster_h
#include "Snap.h"

const double kDefaultTol = 1e-12;
const int kMaxIter = 300;

typedef TVec< THash<TInt, TInt> > WeightVH;

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
  triangle,   // undirected cliques
  clique3,    //       |
  clique4,    //       |
  clique5,    //       |
  clique6,    //       |
  clique7,    //       |
  clique8,    //       |
  clique9,    //       |
  edge,       // (undirected) edges
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

// Wrapper around ARPACK for computing the smallest algebraic eigenvalues of a
// matrix A.  A is the matrix, nev is the number of eigenvectors to compute, tol
// is the stopping tolerance, and maxiter is the maximum number of iterations.
// This routine stores the eigenvalues in evals and the eigenvectors in evecs,
// sorted from smallest to largest eigenvalue.
void SymeigsSmallest(const TSparseColMatrix& A, int nev, TFltV& evals,
		     TFullColMatrix& evecs, double tol=kDefaultTol,
		     int maxiter=kMaxIter);

class MotifCluster {
 public:
  // Given a graph and a motif type, uses the motif spectral clustering to find
  // a motif-based cluster in the graph.  The result of the method is stored in
  // the sweepcut data structure.  Optional parameters include the tolerance and
  // maximum number of iterations for the underlying eigenvector routine.
  static void GetMotifCluster(PNGraph graph, MotifType motif,
			      TSweepCut& sweepcut, double tol=kDefaultTol,
			      int maxiter=kMaxIter);
  static void GetMotifCluster(PUNGraph graph, MotifType motif,
			      TSweepCut& sweepcut, double tol=kDefaultTol,
			      int maxiter=kMaxIter);  

  // For a given graph, fill the weights vector with the weights in the motif
  // adjacency matrix.  Specifically, weights[i][j] is the number of times i and
  // j co-occurr in an instance of the motif for any i < j (only stores the
  // lower triangular part of the matrix).
  static void MotifAdjacency(PNGraph graph, MotifType motif, WeightVH& weights);
  static void MotifAdjacency(PUNGraph graph, MotifType motif, WeightVH& weights);
  
  // Given a weighted network, compute a cut of the graph using the Fiedler
  // vector and a sweep cut.  Results are stored in the sweepcut data structure.
  // The variables tol and maxiter control the stopping tolerance and maximum
  // number of iterations used by the eigensolver.
  static void SpectralCut(const WeightVH& weights, TSweepCut& sweepcut,
			  double tol=kDefaultTol, int maxiter=kMaxIter);

  // Compute the normalized Fiedler vector for the normalized Laplacian of the
  // graph corresponding to the nonnegative matrix W and store the result in
  // fvec.  The normalized Fiedler vector is the eigenvector corresponding to
  // the second smallest eigenvalue of the normalized Laplacian, scaled by the
  // inverse square root of the node degrees.
  static double NFiedlerVector(const TSparseColMatrix& W, TFltV& fvec,
			       double tol=kDefaultTol, int maxiter=kMaxIter);

  // Given a string representation of a motif name, parse it to a MotifType.
  static MotifType ParseMotifType(const TStr& motif);

  // Check if three nodes form an instance of a directed triangle motif.
  static bool IsMotifM1(PNGraph graph, int u, int v, int w);
  static bool IsMotifM2(PNGraph graph, int u, int v, int w);
  static bool IsMotifM3(PNGraph graph, int u, int v, int w);
  static bool IsMotifM4(PNGraph graph, int u, int v, int w);
  static bool IsMotifM5(PNGraph graph, int u, int v, int w);
  static bool IsMotifM6(PNGraph graph, int u, int v, int w);
  static bool IsMotifM7(PNGraph graph, int u, int v, int w);
  // Check if three nodes form a directed wedge motif with a specified center.
  static bool IsMotifM8(PNGraph graph, int center, int v, int w);
  static bool IsMotifM9(PNGraph graph, int center, int v, int w);
  static bool IsMotifM10(PNGraph graph, int center, int v, int w);
  static bool IsMotifM11(PNGraph graph, int center, int v, int w);
  static bool IsMotifM12(PNGraph graph, int center, int v, int w);
  static bool IsMotifM13(PNGraph graph, int center, int v, int w);  

  // Check if u --> v is a unidirectional edge (u --> v but no v --> u).
  static bool IsUnidirEdge(PNGraph graph, int u, int v);

  // Check if u and v form a bidirectional edge (u <--> v).
  static bool IsBidirEdge(PNGraph graph, int u, int v);

  // Check if there is no edge between u and v
  static bool IsNoEdge(PNGraph graph, int u, int v);

  // Fills order vector so that order[i] < order[j] implies that
  //    degree(i) <= degree(j),
  // where degree is the number of unique incoming and outgoing neighbors
  // (reciprocated edge neighbors are only counted once).
  static void DegreeOrdering(PNGraph graph, TIntV& order);

 private:
  // Handles MotifAdjacency() functionality for simple edges..
  static void EdgeMotifAdjacency(PNGraph graph, WeightVH& weights);
  static void EdgeMotifAdjacency(PUNGraph graph, WeightVH& weights);  

  
  // Handles MotifAdjacency() functionality for directed triangle motifs.
  static void TriangleMotifAdjacency(PNGraph graph, MotifType motif,
                              WeightVH& weights);

  // Handles MotifAdjacency() functionality for directed wedges.
  static void WedgeMotifAdjacency(PNGraph graph, MotifType motif,
				  WeightVH& weights);  

  // Handles MotifAdjacency() functionality for the bifan motif.
  static void BifanMotifAdjacency(PNGraph graph, WeightVH& weights);

  // Handles MotifAdjacency() functionality for the semi-clique.  
  static void SemicliqueMotifAdjacency(PUNGraph graph, WeightVH& weights);

  // Handles MotifAdjacency() functionality for undirected cliques.
  static void CliqueMotifAdjacency(PUNGraph graph, int clique_size,
				   WeightVH& weights);
};

// Helper Class for doing undirected clique adjacency matrix weighting.  Uses
// the Chiba & Nishizeki algorithm with (k-1)-core preprocessing.  See:
//
// Chiba, Norishige, and Takao Nishizeki. "Arboricity and subgraph listing
// algorithms." SIAM Journal on Computing 14.1 (1985): 210-223.
class ChibaNishizekiWeighter {
 public:
 ChibaNishizekiWeighter(PUNGraph graph) : orig_graph_(graph) {}
  
  // Form motif adjacency matrix for cliques of size k
  void Run(int k);
  
  // Get the weight vector
  WeightVH& weights() { return weights_; }
  
 private:
  // Get the order of nodes given by the subgraph induced by U
  void SubgraphDegreeOrder(int k, const TIntV& U, TIntV& order);
  
  // Update the weights for all nodes participating in the clique
  void UpdateWeights(const TIntV& clique);

  // Write out all of the cliques (nodes in U + the stack)
  void FlushCliques(const TIntV& U);
  
  // Main recursive function
  void CliqueEnum(int k, const TIntV& U);

  // Adjust neighbors with label k
  void AdjustLabels(int kcurr, int klast, const TIntV& U);

  void Initialize(int k);

  TVec < TVec<TIntV> > graph_;
  TIntV labels_;
  TIntV C_;
  int k_;  // size of clique
  PUNGraph orig_graph_;
  WeightVH weights_;  // motif adjacency weights
};

#endif  // snap_motifcluster_h
