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

// PageRank and HITS (Hubs and Authorities)
template<class PGraph> void GetPageRank(const PGraph& Graph, TIntFltH& PRankH, const double& C=0.85, const double& Eps=1e-4, const int& MaxIter=100);
template<class PGraph> void GetHits(const PGraph& Graph, TIntFltH& NIdHubH, TIntFltH& NIdAuthH, const int& MaxIter=20);


/////////////////////////////////////////////////
// Page Rank -- there are two different implementations (uncomment the desired 2 lines):
//   Berkhin -- (the correct way) see Algorithm 1 of P. Berkhin, A Survey on PageRank Computing, Internet Mathematics, 2005
//   iGraph -- iGraph implementation(which treats leaked PageRank in a funny way)
template<class PGraph>
void GetPageRank(const PGraph& Graph, TIntFltH& PRankH, const double& C, const double& Eps, const int& MaxIter) {
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
    if (diff < Eps) { break; }
  }
}

// HITS: Hubs and Authorities (by J. Kleinberg, see http://en.wikipedia.org/wiki/HITS_algorithm)
template<class PGraph>
void GetHits(const PGraph& Graph, TIntFltH& NIdHubH, TIntFltH& NIdAuthH, const int& MaxIter) {
  const int NNodes = Graph->GetNodes();
  NIdHubH.Gen(NNodes);
  NIdAuthH.Gen(NNodes);
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NIdHubH.AddDat(NI.GetId(), 1.0);
    NIdAuthH.AddDat(NI.GetId(), 1.0);
  }
  double Norm=0;
  for (int iter = 0; iter < MaxIter; iter++) {
    // update authority scores
    Norm = 0;
    for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
      double& Auth = NIdAuthH.GetDat(NI.GetId()).Val;
      Auth = 0;
      for (int e = 0; e < NI.GetInDeg(); e++) {
        Auth +=  NIdHubH.GetDat(NI.GetInNId(e)); }
      Norm += Auth*Auth;
    }
    Norm = sqrt(Norm);
    for (int i = 0; i < NIdAuthH.Len(); i++) { NIdAuthH[i] /= Norm; }
    // update hub scores
    for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
      double& Hub = NIdHubH.GetDat(NI.GetId()).Val;
      Hub = 0; 
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        Hub += NIdAuthH.GetDat(NI.GetOutNId(e)); }
      Norm += Hub*Hub;
    }
    Norm = sqrt(Norm);
    for (int i = 0; i < NIdHubH.Len(); i++) { NIdHubH[i] /= Norm; }
  }
}

}; // namespace TSnap
