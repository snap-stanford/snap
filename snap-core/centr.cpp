namespace TSnap {

/////////////////////////////////////////////////
// Node centrality measures
double GetDegreeCentr(const PUNGraph& Graph, const int& NId) {
  if (Graph->GetNodes() > 1) {
    return double(Graph->GetNI(NId).GetDeg())/double(Graph->GetNodes()-1); }
  else { return 0.0; }
}

double GetFarnessCentr(const PUNGraph& Graph, const int& NId) {
  TIntH NDistH(Graph->GetNodes());
  TSnap::GetShortPath<PUNGraph>(Graph, NId, NDistH, true, TInt::Mx);
  double sum = 0;
  for (TIntH::TIter I = NDistH.BegI(); I < NDistH.EndI(); I++) {
    sum += I->Dat();
  }
  if (NDistH.Len() > 1) { return sum/double(NDistH.Len()-1); }
  else { return 0.0; }
}

double GetClosenessCentr(const PUNGraph& Graph, const int& NId) {
  const double Farness = GetFarnessCentr(Graph, NId);
  if (Farness != 0.0) { return 1.0/Farness; }
  else { return 0.0; }
}

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

// Approximate beetweenness centrality scores. The implementation scales to large networks.
// NodeFrac ... calculate Beetweenness Centrality for a fraction of nodes. Gives approximate.
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

void GetEigenVectorCentr(const PUNGraph& Graph, TIntFltH& NIdEigenH, const double& Eps, const int& MaxIter) {
  const int NNodes = Graph->GetNodes();
  NIdEigenH.Gen(NNodes);
  // initialize vector values
  for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NIdEigenH.AddDat(NI.GetId(), 1.0/NNodes);
    IAssert(NI.GetId() == NIdEigenH.GetKey(NIdEigenH.Len()-1));
  }
  TFltV TmpV(NNodes);
  for (int iter = 0; iter < MaxIter; iter++) {
    int j = 0;
    // add neighbor values
    for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++, j++) {
      TmpV[j] = 0;
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        TmpV[j] += NIdEigenH.GetDat(NI.GetOutNId(e)); }
    }

    // normalize
    double sum = 0;
    for (int i = 0; i < TmpV.Len(); i++) {
      sum += (TmpV[i]*TmpV[i]);
    }
    sum = sqrt(sum);
    for (int i = 0; i < TmpV.Len(); i++) {
      TmpV[i] /= sum;
    }

    // compute difference
    double diff = 0.0;
    j = 0;
    for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++, j++) {
      diff += fabs(NIdEigenH.GetDat(NI.GetId())-TmpV[j]);
    }

    // set new values
    j = 0;
    for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++, j++) {
      NIdEigenH.AddDat(NI.GetId(), TmpV[j]);
    }

    if (diff < Eps) {
      break;
    }
  }
}

int GetWeightedPageRank(const PNEANet Graph, TIntFltH& PRankH, const TStr& Attr, const double& C, const double& Eps, const int& MaxIter) {
  if (!Graph->IsFltAttrE(Attr)) return -1;

  TFltV Weights = Graph->GetFltAttrVecE(Attr);

  int mxid = Graph->GetMxNId();
  TFltV OutWeights(mxid);
  Graph->GetWeightOutEdgesV(OutWeights, Weights);
  /*for (TNEANet::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    OutWeights[NI.GetId()] = Graph->GetWeightOutEdges(NI, Attr);
  }*/


  /*TIntFltH Weights;
  for (TNEANet::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    Weights.AddDat(NI.GetId(), Graph->GetWeightOutEdges(NI, Attr));
  }*/

  const int NNodes = Graph->GetNodes();
  //const double OneOver = 1.0/double(NNodes);
  PRankH.Gen(NNodes);
  for (TNEANet::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    PRankH.AddDat(NI.GetId(), 1.0/NNodes);
    //IAssert(NI.GetId() == PRankH.GetKey(PRankH.Len()-1));
  }
  TFltV TmpV(NNodes);
  for (int iter = 0; iter < MaxIter; iter++) {
    int j = 0;
    for (TNEANet::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++, j++) {
      TmpV[j] = 0;
      for (int e = 0; e < NI.GetInDeg(); e++) {
        const int InNId = NI.GetInNId(e);
        const TFlt OutWeight = OutWeights[InNId];
        int EId = Graph->GetEId(InNId, NI.GetId());
        const TFlt Weight = Weights[Graph->GetFltKeyIdE(EId)];
        if (OutWeight > 0) {
          TmpV[j] += PRankH.GetDat(InNId) * Weight / OutWeight; }
      }
      TmpV[j] =  C*TmpV[j]; // Berkhin (the correct way of doing it)
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
  return 0;
}

#ifdef _OPENMP

int GetWeightedPageRankMP1(const PNEANet Graph, TIntFltH& PRankH, const TStr& Attr, const double& C, const double& Eps, const int& MaxIter) {
  if (!Graph->IsFltAttrE(Attr)) return -1;

  TFltV Weights = Graph->GetFltAttrVecE(Attr);

  int mxid = Graph->GetMxNId();
  TFltV OutWeights(mxid);
  Graph->GetWeightOutEdgesV(OutWeights, Weights);
  /*for (TNEANet::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    OutWeights[NI.GetId()] = Graph->GetWeightOutEdges(NI, Attr);
  }*/


  /*TIntFltH Weights;
  for (TNEANet::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    Weights.AddDat(NI.GetId(), Graph->GetWeightOutEdges(NI, Attr));
  }*/

  const int NNodes = Graph->GetNodes();
  TVec<TNEANet::TNodeI> NV;
  //const double OneOver = 1.0/double(NNodes);
  PRankH.Gen(NNodes);
  for (TNEANet::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NV.Add(NI);
    PRankH.AddDat(NI.GetId(), 1.0/NNodes);
    //IAssert(NI.GetId() == PRankH.GetKey(PRankH.Len()-1));
  }
  TFltV TmpV(NNodes);
  for (int iter = 0; iter < MaxIter; iter++) {
    #pragma omp parallel for schedule(dynamic,10000)
    for (int j = 0; j < NNodes; j++) {
      TNEANet::TNodeI NI = NV[j];
      TmpV[j] = 0;
      for (int e = 0; e < NI.GetInDeg(); e++) {
        const int InNId = NI.GetInNId(e);
        const TFlt OutWeight = OutWeights[InNId];
        int EId = Graph->GetEId(InNId, NI.GetId());
        const TFlt Weight = Weights[Graph->GetFltKeyIdE(EId)];
        if (OutWeight > 0) {
          TmpV[j] += PRankH.GetDat(InNId) * Weight / OutWeight; }
      }
      TmpV[j] =  C*TmpV[j]; // Berkhin (the correct way of doing it)
      //TmpV[j] =  C*TmpV[j] + (1.0-C)*OneOver; // iGraph
    }
    double diff=0, sum=0, NewVal;
    #pragma omp parallel for reduction(+:sum) schedule(dynamic,10000)
    for (int i = 0; i < TmpV.Len(); i++) { sum += TmpV[i]; }
    const double Leaked = (1.0-sum) / double(NNodes);
    #pragma omp parallel for reduction(+:diff) schedule(dynamic,10000)
    for (int i = 0; i < PRankH.Len(); i++) { // re-instert leaked PageRank
      NewVal = TmpV[i] + Leaked; // Berkhin
      //NewVal = TmpV[i] / sum;  // iGraph
      diff += fabs(NewVal-PRankH[i]);
      PRankH[i] = NewVal;
    }
    if (diff < Eps) { break; }
  }
  return 0;
}

int GetWeightedPageRankMP2(const PNEANet Graph, TIntFltH& PRankH, const TStr& Attr, const double& C, const double& Eps, const int& MaxIter) {
  if (!Graph->IsFltAttrE(Attr)) return -1;
  const int NNodes = Graph->GetNodes();
  TVec<TNEANet::TNodeI> NV;

  //const double OneOver = 1.0/double(NNodes);
  PRankH.Gen(NNodes);
  int MxId = 0;

  for (TNEANet::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NV.Add(NI);
    PRankH.AddDat(NI.GetId(), 1.0/NNodes);
    int Id = NI.GetId();
    if (Id > MxId) {
      MxId = Id;
    }
  }

  TFltV PRankV(MxId+1);
  TFltV OutWeights(MxId+1);

  TFltV Weights = Graph->GetFltAttrVecE(Attr);

  #pragma omp parallel for schedule(dynamic,10000)
  for (int j = 0; j < NNodes; j++) {
    TNEANet::TNodeI NI = NV[j];
    int Id = NI.GetId();
    OutWeights[Id] = Graph->GetWeightOutEdges(NI, Attr);
    PRankV[Id] = 1/NNodes;
  }

  TFltV TmpV(NNodes);
  for (int iter = 0; iter < MaxIter; iter++) {

    #pragma omp parallel for schedule(dynamic,10000)
    for (int j = 0; j < NNodes; j++) {
      TNEANet::TNodeI NI = NV[j];
      TFlt Tmp = 0;
      for (int e = 0; e < NI.GetInDeg(); e++) {
        const int InNId = NI.GetInNId(e);

        const TFlt OutWeight = OutWeights[InNId];

        int EId = Graph->GetEId(InNId, NI.GetId());
        const TFlt Weight = Weights[Graph->GetFltKeyIdE(EId)];

        if (OutWeight > 0) {
          Tmp += PRankH.GetDat(InNId) * Weight / OutWeight; 
        }
      }
      TmpV[j] =  C*Tmp; // Berkhin (the correct way of doing it)
      //TmpV[j] =  C*TmpV[j] + (1.0-C)*OneOver; // iGraph
    }

    double sum = 0;
    #pragma omp parallel for reduction(+:sum) schedule(dynamic,10000)
    for (int i = 0; i < TmpV.Len(); i++) { sum += TmpV[i]; }
    const double Leaked = (1.0-sum) / double(NNodes);

    double diff = 0;
    #pragma omp parallel for reduction(+:diff) schedule(dynamic,10000)
    for (int i = 0; i < NNodes; i++) {
      TNEANet::TNodeI NI = NV[i];
      double NewVal = TmpV[i] + Leaked; // Berkhin
      //NewVal = TmpV[i] / sum;  // iGraph
      int Id = NI.GetId();
      diff += fabs(NewVal-PRankV[Id]);
      PRankV[Id] = NewVal;
    }
    if (diff < Eps) { break; }
  }

  #pragma omp parallel for schedule(dynamic,10000)
  for (int i = 0; i < NNodes; i++) {
    TNEANet::TNodeI NI = NV[i];
    PRankH[i] = PRankV[NI.GetId()];
  }

  return 0;
}

#endif

}; // namespace TSnap
