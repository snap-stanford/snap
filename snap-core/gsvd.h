/////////////////////////////////////////////////
/// Directed Graph Adjacency Matrix represented as sparse {0,1} row matrix.
/// The class is used for computing spectral properties of graph adjacency matrices.
/// The class assumes that node IDs have the range 0...Nodes-1.
class TNGraphMtx : public TMatrix {
private:
  PNGraph Graph;
  bool CheckNodeIds();
public:
  TNGraphMtx(const PNGraph& GraphPt);
  TNGraphMtx(const TNGraphMtx& GraphMtx) : Graph(GraphMtx.Graph) { }
  TNGraphMtx& operator = (const TNGraphMtx& GraphMtx) { Graph=GraphMtx.Graph;  return *this; }
  int PGetRows() const { return Graph->GetNodes(); }
  int PGetCols() const { return Graph->GetNodes(); }
  // Result = A * B(:,ColId)
  void PMultiply(const TFltVV& B, int ColId, TFltV& Result) const;
  // Result = A * Vec
  void PMultiply(const TFltV& Vec, TFltV& Result) const;
  // Result = A' * B(:,ColId)
  void PMultiplyT(const TFltVV& B, int ColId, TFltV& Result) const;
  // Result = A' * Vec
  void PMultiplyT(const TFltV& Vec, TFltV& Result) const;
};

/////////////////////////////////////////////////
/// Undirected Graph Adjacency Matrix represented as sparse {0,1} row matrix.
/// The class is used for computing spectral properties of graph adjacency matrices.
/// The class assumes that node IDs have the range 0...Nodes-1.
class TUNGraphMtx : public TMatrix {
private:
  PUNGraph Graph;
  bool CheckNodeIds();
public:
  TUNGraphMtx(const PUNGraph& GraphPt);
  TUNGraphMtx(const TUNGraphMtx& GraphMtx) : Graph(GraphMtx.Graph) { }
  TUNGraphMtx& operator = (const TUNGraphMtx& GraphMtx) { Graph=GraphMtx.Graph;  return *this; }
  int PGetRows() const { return Graph->GetNodes(); }
  int PGetCols() const { return Graph->GetNodes(); }
  // Result = A * B(:,ColId)
  void PMultiply(const TFltVV& B, int ColId, TFltV& Result) const;
  // Result = A * Vec
  void PMultiply(const TFltV& Vec, TFltV& Result) const;
  // Result = A' * B(:,ColId)
  void PMultiplyT(const TFltVV& B, int ColId, TFltV& Result) const;
  // Result = A' * Vec
  void PMultiplyT(const TFltV& Vec, TFltV& Result) const;
};

/////////////////////////////////////////////////
// Graphs Singular Value Decomposition of Graph Adjacency Matrix
namespace TSnap {

/// Computes largest SngVals singular values of the adjacency matrix representing a directed Graph.
void GetSngVals(const PNGraph& Graph, const int& SngVals, TFltV& SngValV);
/// Computes the leading left and right singular vector of the adjacency matrix representing a directed Graph.
void GetSngVec(const PNGraph& Graph, TFltV& LeftSV, TFltV& RightSV);
/// Computes the singular values and left and right singular vectors of the adjacency matrix representing a directed Graph.
/// @param SngVecs Number of singular values/vectors to compute.
void GetSngVec(const PNGraph& Graph, const int& SngVecs, TFltV& SngValV, TVec<TFltV>& LeftSV, TVec<TFltV>& RightSV);

/// Computes top EigVals eigenvalues of the adjacency matrix representing a given undirected Graph.
void GetEigVals(const PUNGraph& Graph, const int& EigVals, TFltV& EigValV);
/// Computes the leading eigenvector of the adjacency matrix representing a given undirected Graph.
void GetEigVec(const PUNGraph& Graph, TFltV& EigVecV);
/// Computes top EigVecs eigenvalues and eigenvectors of the adjacency matrix representing a given undirected Graph.
void GetEigVec(const PUNGraph& Graph, const int& EigVecs, TFltV& EigValV, TVec<TFltV>& EigVecV);
/// Computes Inverse participation ratio of a given graph.
/// See Spectra of "real-world" graphs: Beyond the semicircle law by Farkas, Derenyi, Barabasi and Vicsek
void GetInvParticipRat(const PUNGraph& Graph, int MaxEigVecs, int TimeLimit, TFltPrV& EigValIprV);

namespace TSnapDetail {
double GetInvParticipRat(const TFltV& EigVec);
} // namespace TSnapDetail

}; // namespace TSnap
