namespace TSnap {

#ifdef USE_OPENMP

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

#endif // USE_OPENMP

}; // namespace TSnap
