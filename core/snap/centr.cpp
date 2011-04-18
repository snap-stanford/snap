namespace TSnap {

/////////////////////////////////////////////////
// Node centrality measures
double GetDegreeCentr(const PUNGraph& Graph, const int& NId) {
  if (Graph->GetNodes() > 1) {
    return Graph->GetNI(NId).GetDeg()/double(Graph->GetNodes()-1); }
  else { return 0; }
}

double GetClosenessCentr(const PUNGraph& Graph, const int& NId) {
  TIntH NDistH(Graph->GetNodes());
  TSnap::GetShortPath<PUNGraph>(Graph, NId, NDistH, true, TInt::Mx);
  int sum = 0;
  for (TIntH::TIter I = NDistH.BegI(); I < NDistH.EndI(); I++) {
    sum += I->Dat();
  }
  if (NDistH.Len() > 1) { return sum/double(NDistH.Len()-1); }
  else { return 0.0; }
}

// Beetweenness Centrality. To get exact results we solve single-source shortest-path problem for every node.
// Solving it for a BtwNIdV subset of nodes gives centralitiy values that are about Graph->GetNodes()/BtwNIdV.Len() times lower than exact centrality.
// "A Faster Algorithm for Beetweenness Centrality", Ulrik Brandes, Journal of Mathematical Sociology, 2001
// "Centrality Estimation in Large Networks", Urlik Brandes and Christian Pich, 2006
void GetBetweennessCentr(const PUNGraph& Graph, const TIntV& BtwNIdV, TIntFltH& NodeBtwH, const bool& DoNodeCent, TIntPrFltH& EdgeBtwH, const bool& DoEdgeCent) {
  if (DoNodeCent) { NodeBtwH.Clr(); }
  if (DoEdgeCent) { EdgeBtwH.Clr(); }
  const int nodes = Graph->GetNodes();
  TIntS S(nodes);
  TIntQ Q(nodes);
  TIntIntVH P(nodes); // one vector for every node
  TIntFltH delta(nodes);
  TIntH sigma(nodes), d(nodes);
  // init  
  for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    if (DoNodeCent) { 
      NodeBtwH.AddDat(NI.GetId(), 0); }
    if (DoEdgeCent) {
      for (int e = 0; e < NI.GetOutDeg(); e++) { 
        if (NI.GetId() < NI.GetOutNId(e)) {
          EdgeBtwH.AddDat(TIntPr(NI.GetId(), NI.GetOutNId(e)), 0); }
      }
    }
    sigma.AddDat(NI.GetId(), 0);
    d.AddDat(NI.GetId(), -1);
    P.AddDat(NI.GetId(), TIntV());
    delta.AddDat(NI.GetId(), 0);
  }
  // calc betweeness
  for (int k=0; k < BtwNIdV.Len(); k++) {
    const TUNGraph::TNodeI NI = Graph->GetNI(BtwNIdV[k]);
    // reset
    for (int i = 0; i < sigma.Len(); i++) {
      sigma[i]=0;  d[i]=-1;  delta[i]=0;  P[i].Clr(false);
    }
    S.Clr(false);
    Q.Clr(false);
    sigma.AddDat(NI.GetId(), 1);
    d.AddDat(NI.GetId(), 0);
    Q.Push(NI.GetId());
    while (! Q.Empty()) {
      const int v = Q.Top();  Q.Pop();
      const TUNGraph::TNodeI NI2 = Graph->GetNI(v);
      S.Push(v);
      const int VDat = d.GetDat(v);
      for (int e = 0; e < NI2.GetOutDeg(); e++) {
        const int w = NI2.GetOutNId(e);
        if (d.GetDat(w) < 0) { // find w for the first time
          Q.Push(w);
          d.AddDat(w, VDat+1);
        }
        //shortest path to w via v ?
        if (d.GetDat(w) == VDat+1) {
          sigma.AddDat(w) += sigma.GetDat(v);
          P.GetDat(w).Add(v);
        }
      }
    }
    while (! S.Empty()) {
      const int w = S.Top();
      const double SigmaW = sigma.GetDat(w);
      const double DeltaW = delta.GetDat(w);
      const TIntV NIdV = P.GetDat(w);
      S.Pop();
      for (int i = 0; i < NIdV.Len(); i++) {
        const int nid = NIdV[i];
        const double c = (sigma.GetDat(nid)*1.0/SigmaW) * (1+DeltaW);
        delta.AddDat(nid) += c;
        if (DoEdgeCent) {
          EdgeBtwH.AddDat(TIntPr(TMath::Mn(nid, w), TMath::Mx(nid, w))) += c; }
      }
      if (DoNodeCent && w != NI.GetId()) {
        NodeBtwH.AddDat(w) += delta.GetDat(w)/2.0; }
    }
  }
}

// NodeFrac ... calculate Beetweenness Centrality for a fraction of nodes. Gives approximate
// beetweenness centrality scores but scales to large networks
void GetBetweennessCentr(const PUNGraph& Graph, TIntFltH& NodeBtwH, const double& NodeFrac) {
  TIntPrFltH EdgeBtwH;
  TIntV NIdV;  Graph->GetNIdV(NIdV);
  if (NodeFrac < 1.0) { // calculate beetweenness centrality for a subset of nodes
    NIdV.Shuffle(TInt::Rnd);
    for (int i = int((1.0-NodeFrac)*NIdV.Len()); i > 0; i--) { 
      NIdV.DelLast(); }
  }
  GetBetweennessCentr(Graph, NIdV, NodeBtwH, true, EdgeBtwH, false);
}

void GetBetweennessCentr(const PUNGraph& Graph, TIntPrFltH& EdgeBtwH, const double& NodeFrac) {
  TIntFltH NodeBtwH;
  TIntV NIdV;  Graph->GetNIdV(NIdV);
  if (NodeFrac < 1.0) { // calculate beetweenness centrality for a subset of nodes
    NIdV.Shuffle(TInt::Rnd);
    for (int i = int((1.0-NodeFrac)*NIdV.Len()); i > 0; i--) { 
      NIdV.DelLast(); }
  }
  GetBetweennessCentr(Graph, NIdV, NodeBtwH, false, EdgeBtwH, true);
}

void GetBetweennessCentr(const PUNGraph& Graph, TIntFltH& NodeBtwH, TIntPrFltH& EdgeBtwH, const double& NodeFrac) {
  TIntV NIdV;  Graph->GetNIdV(NIdV);
  if (NodeFrac < 1.0) { // calculate beetweenness centrality for a subset of nodes
    NIdV.Shuffle(TInt::Rnd);
    for (int i = int((1.0-NodeFrac)*NIdV.Len()); i > 0; i--) { 
      NIdV.DelLast(); }
  }
  GetBetweennessCentr(Graph, NIdV, NodeBtwH, true, EdgeBtwH, true);
}

void GetEigenVectorCentr(const PUNGraph& Graph, TIntFltH& EigenH, const double& Eps, const int& MaxIter) {
  const int NNodes = Graph->GetNodes();
  EigenH.Gen(NNodes);
  for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    EigenH.AddDat(NI.GetId(), 1.0/NNodes);
    IAssert(NI.GetId() == EigenH.GetKey(EigenH.Len()-1));
  }
  TFltV TmpV(NNodes);
  double diff = TFlt::Mx;
  for (int iter = 0; iter < MaxIter; iter++) {
    int j = 0;
    for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++, j++) {
      TmpV[j] = 0;
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        TmpV[j] += EigenH.GetDat(NI.GetOutNId(e)); }
    }
    double sum = 0;
    for (int i = 0; i < TmpV.Len(); i++) {
      EigenH[i] = TmpV[i];
      sum += EigenH[i];
    }
    for (int i = 0; i < EigenH.Len(); i++) {
      EigenH[i] /= sum; }
    if (fabs(diff-sum) < Eps) { break; }
    //printf("\tdiff:%f\tsum:%f\n", fabs(diff-sum), sum);
    diff = sum;
  }
}

}; // namespace TSnap
