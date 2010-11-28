namespace TSnap {

/////////////////////////////////////////////////
// Centrality measures (see: http://en.wikipedia.org/wiki/Centrality)

// Degree and Closeness centrality
double GetDegreeCentr(const PUNGraph& Graph, const int& NId);
double GetClosenessCentr(const PUNGraph& Graph, const int& NId);

// Betweenness centrality (exact and fast approximate calculations)
void GetBetweennessCentr(const PUNGraph& Graph, TIntFltH& NIdBtwH, const double& NodeFrac=1.0);
void GetBetweennessCentr(const PUNGraph& Graph, TIntPrFltH& EdgeBtwH, const double& NodeFrac=1.0);
void GetBetweennessCentr(const PUNGraph& Graph, TIntFltH& NodeBtwH, TIntPrFltH& EdgeBtwH, const double& NodeFrac=1.0);
void GetBetweennessCentr(const PUNGraph& Graph, const TIntV& BtwNIdV, TIntFltH& NodeBtwH, const bool& DoNodeCent, TIntPrFltH& EdgeBtwH, const bool& DoEdgeCent);

// Eigenvector centrality
void GetEigenVectorCentr(const PUNGraph& Graph, TIntFltH& EigenH, const double& Eps=1e-6, const int& MaxIter=100);

// Page Rank
template<class PGraph>
void GetPageRank(const PGraph& Graph, TIntFltH& PRankH, const double& C=0.15, const double& Eps=1e-4, const int& MaxIter=100) {
  const int NNodes = Graph->GetNodes();
  PRankH.Gen(NNodes);
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    PRankH.AddDat(NI.GetId(), 1.0/NNodes);
    IAssert(NI.GetId() == PRankH.GetKey(PRankH.Len()-1));
  }
  TFltV TmpV(NNodes);
  for (int iter = 0; iter < MaxIter; iter++) {
    int j = 0;
    for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++, j++) {
      TmpV[j] = 0;
      for (int e = 0; e < NI.GetInDeg(); e++) {
        TmpV[j] += PRankH.GetDat(NI.GetInNId(e)) / Graph->GetNI(NI.GetInNId(e)).GetOutDeg(); }
      TmpV[j] = C + (1.0-C)*TmpV[j];
    }
    double diff=0;
    for (int i = 0; i < PRankH.Len(); i++) {
      diff += fabs(PRankH[i] - TmpV[i]);
      PRankH[i] = TmpV[i]; 
    }
    if (diff < Eps) { break; }
  }
}

}; // namespace TSnap
