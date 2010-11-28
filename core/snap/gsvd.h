/////////////////////////////////////////////////
// Directed Graph Matrix -- sparse {0,1} row matrix 
// (node IDs have to be 0..Nodes-1)
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
// Undirected Graph Matrix -- sparse {0,1} row matrix 
// (node IDs have to be 0..Nodes-1)
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

void GetSngVals(const PNGraph& Graph, const int& SngVals, TFltV& SngValV);
void GetSngVec(const PNGraph& Graph, TFltV& LeftSV, TFltV& RightSV);
void GetSngVec(const PNGraph& Graph, const int& SngVecs, TFltV& SngValV, TVec<TFltV>& LeftSV, TVec<TFltV>& RightSV);

void GetEigVals(const PUNGraph& Graph, const int& EigVals, TFltV& EigValV);
void GetEigVec(const PUNGraph& Graph, TFltV& EigVecV);
void GetEigVec(const PUNGraph& Graph, const int& EigVecs, TFltV& EigValV, TVec<TFltV>& EigVecV);
void GetInvParticipRat(const PUNGraph& Graph, int MaxEigVecs, int TimeLimit, TFltPrV& EigValIprV);
double GetInvParticipRat(const TFltV& EigVec);
}; // namespace TSnap
