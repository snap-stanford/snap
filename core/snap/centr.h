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
void GetEigenVectorCentr(const PUNGraph& Graph, TIntFltH& EigenH, const double& Eps=1e-4, const int& MaxIter=100);

// Page Rank -- there are two different implementations (uncomment the desired 2 lines):
//   Berkhin -- see Algorithm 1 of P. Berkhin, A Survey on PageRank Computing, Internet Mathematics, 2005
//   iGraph -- the way iGraph does it (which is a different way of treating leaked PageRank)
template<class PGraph>
void GetPageRank(const PGraph& Graph, TIntFltH& PRankH, const double& C=0.85, const double& Eps=1e-4, const int& MaxIter=100) {
  const int NNodes = Graph->GetNodes();
  const double OneOver = 1.0/double(NNodes);
  PRankH.Gen(NNodes);
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    PRankH.AddDat(NI.GetId(), 1.0/NNodes);
    //IAssert(NI.GetId() == PRankH.GetKey(PRankH.Len()-1));
  }
  TFltV TmpV(NNodes);
  for (int iter = 0; iter < MaxIter; iter++) {
    int j = 0;
    for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++, j++) {
      TmpV[j] = 0;
      for (int e = 0; e < NI.GetInDeg(); e++) {
        const int InNId = NI.GetInNId(e);
        const int OutDeg = Graph->GetNI(InNId).GetOutDeg();
        if (OutDeg > 0) {
          TmpV[j] += PRankH.GetDat(InNId) / OutDeg; }
      }
      TmpV[j] =  C*TmpV[j]; // Berkhin
      //TmpV[j] =  C*TmpV[j] + (1.0-C)*OneOver; // iGraph
    }
    double diff=0, sum=0, NewVal;
    for (int i = 0; i < TmpV.Len(); i++) { sum += TmpV[i]; }
    const double Leaked = (1.0-sum) / double(NNodes);
    for (int i = 0; i < PRankH.Len(); i++) { // re-instert leaked PageRank
      NewVal = TmpV[i] + Leaked; // Berkhin
      //NewVal = TmpV[i] / sum;  // iGraph
      diff += fabs(NewVal-PRankH[i]);
      PRankH[i] = NewVal;
    }
    //printf("  diff: %f\tleaked: %f\n", diff, 1-sum);
    if (diff < Eps) { break; }
  }
  //for (int i = 0; i < PRankH.Len(); i++) { printf("%d\t%f\n", PRankH.GetKey(i), PRankH[i]); }
}

}; // namespace TSnap
