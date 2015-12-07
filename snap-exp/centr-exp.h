namespace TSnap {

/////////////////////////////////////////////////
// Node centrality measures (See: http://en.wikipedia.org/wiki/Centrality)

/// PageRank
/// For more info see: http://en.wikipedia.org/wiki/PageRank
template<class PGraph> void GetPageRank_v2(const PGraph& Graph, TIntFltH& PRankH, const double& C=0.85, const double& Eps=1e-4, const int& MaxIter=100);
#ifdef USE_OPENMP
template<class PGraph> void GetPageRankMP1(const PGraph& Graph, TIntFltH& PRankH, const double& C=0.85, const double& Eps=1e-4, const int& MaxIter=100);
template<class PGraph> void GetPageRankMP2(const PGraph& Graph, TIntFltH& PRankH, const double& C=0.85, const double& Eps=1e-4, const int& MaxIter=100);
template<class PGraph> void GetPageRankMP3(const PGraph& Graph, TIntFltH& PRankH, const double& C=0.85, const double& Eps=1e-4, const int& MaxIter=100);
template<class PGraph> void GetPageRankMNetMP(const PGraph& Graph, TIntFltH& PRankH, const double& C, const double& Eps, const int& MaxIter);
#endif

/// Weighted PageRank (TODO: Use template)
#ifdef USE_OPENMP
int GetWeightedPageRankMP1(const PNEANet Graph, TIntFltH& PRankH, const TStr& Attr, const double& C=0.85, const double& Eps=1e-4, const int& MaxIter=100);
int GetWeightedPageRankMP2(const PNEANet Graph, TIntFltH& PRankH, const TStr& Attr, const double& C=0.85, const double& Eps=1e-4, const int& MaxIter=100);
#endif

/////////////////////////////////////////////////
// Implementation

// Page Rank -- there are two different implementations (uncomment the desired 2 lines):
//   Berkhin -- (the correct way) see Algorithm 1 of P. Berkhin, A Survey on PageRank Computing, Internet Mathematics, 2005
//   iGraph -- iGraph implementation(which treats leaked PageRank in a funny way)
// This implementation is an optimized version, it builds a vector and accesses nodes via the vector.
template<class PGraph>
void GetPageRank_v2(const PGraph& Graph, TIntFltH& PRankH, const double& C, const double& Eps, const int& MaxIter) {
  const int NNodes = Graph->GetNodes();
  TVec<typename PGraph::TObj::TNodeI> NV;
  PRankH.Gen(NNodes);
  int MxId = -1;
  //time_t t = time(0);
  //printf("%s", ctime(&t));
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NV.Add(NI);
    PRankH.AddDat(NI.GetId(), 1.0/NNodes);
    int Id = NI.GetId();
    if (Id > MxId) {
      MxId = Id;
    }
  }
  //t = time(0);
  //printf("%s", ctime(&t));

  TFltV PRankV(MxId+1);
  TIntV OutDegV(MxId+1);

  //#pragma omp parallel for schedule(dynamic,10000)
  for (int j = 0; j < NNodes; j++) {
    typename PGraph::TObj::TNodeI NI = NV[j];
    int Id = NI.GetId();
    PRankV[Id] = 1.0/NNodes;
    OutDegV[Id] = NI.GetOutDeg();
  }

  TFltV TmpV(NNodes);

  for (int iter = 0; iter < MaxIter; iter++) {
    //time_t t = time(0);
    //printf("%s%d\n", ctime(&t),iter);
    //#pragma omp parallel for schedule(dynamic,10000)
    for (int j = 0; j < NNodes; j++) {
      typename PGraph::TObj::TNodeI NI = NV[j];
      TFlt Tmp = 0;
      for (int e = 0; e < NI.GetInDeg(); e++) {
        const int InNId = NI.GetInNId(e);
        const int OutDeg = OutDegV[InNId];
        //if (OutDeg != OutDegV[InNId]) {
          //printf("*** ERROR *** InNId %d, OutDeg %d, OutDegV %d\n",
            //InNId, OutDeg, OutDegV[InNId].Val);
        //}
        if (OutDeg > 0) {
          Tmp += PRankV[InNId] / OutDeg;
        }
      }
      TmpV[j] =  C*Tmp; // Berkhin (the correct way of doing it)
    }
    double sum = 0;
    //#pragma omp parallel for reduction(+:sum) schedule(dynamic,10000)
    for (int i = 0; i < TmpV.Len(); i++) { sum += TmpV[i]; }
    const double Leaked = (1.0-sum) / double(NNodes);

    double diff = 0;
    //#pragma omp parallel for reduction(+:diff) schedule(dynamic,10000)
    for (int i = 0; i < NNodes; i++) {
      typename PGraph::TObj::TNodeI NI = NV[i];
      double NewVal = TmpV[i] + Leaked; // Berkhin
      int Id = NI.GetId();
      diff += fabs(NewVal-PRankV[Id]);
      PRankV[Id] = NewVal;
    }
    //printf("counts %d %d\n", hcount1, hcount2);
    if (diff < Eps) { break; }
  }

  //#pragma omp parallel for schedule(dynamic,10000)
  for (int i = 0; i < NNodes; i++) {
    typename PGraph::TObj::TNodeI NI = NV[i];
    PRankH[i] = PRankV[NI.GetId()];
  }
}

#ifdef USE_OPENMP
// Page Rank -- there are two different implementations (uncomment the desired 2 lines):
//   Berkhin -- (the correct way) see Algorithm 1 of P. Berkhin, A Survey on PageRank Computing, Internet Mathematics, 2005
//   iGraph -- iGraph implementation(which treats leaked PageRank in a funny way)
// This is a parallel, non-optimized version.
template<class PGraph>
void GetPageRankMP1(const PGraph& Graph, TIntFltH& PRankH, const double& C, const double& Eps, const int& MaxIter) {
  const int NNodes = Graph->GetNodes();
  TVec<typename PGraph::TObj::TNodeI> NV;
  //const double OneOver = 1.0/double(NNodes);
  PRankH.Gen(NNodes);
  //time_t t = time(0);
  //printf("%s", ctime(&t));
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NV.Add(NI);
    PRankH.AddDat(NI.GetId(), 1.0/NNodes);
    //IAssert(NI.GetId() == PRankH.GetKey(PRankH.Len()-1));
  }

  TFltV TmpV(NNodes);

  //int hcount1 = 0;
  //int hcount2 = 0;
  for (int iter = 0; iter < MaxIter; iter++) {
    //time_t t = time(0);
    //printf("%s%d\n", ctime(&t),iter);
    //int j = 0;
    //for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++, j++) {
    #pragma omp parallel for schedule(dynamic,10000)
    for (int j = 0; j < NNodes; j++) {
      typename PGraph::TObj::TNodeI NI = NV[j];
      TmpV[j] = 0;
      for (int e = 0; e < NI.GetInDeg(); e++) {
        const int InNId = NI.GetInNId(e);
        //hcount1++;
        const int OutDeg = Graph->GetNI(InNId).GetOutDeg();
        if (OutDeg > 0) {
          //hcount2++;
          TmpV[j] += PRankH.GetDat(InNId) / OutDeg;
        }
      }
      TmpV[j] =  C*TmpV[j]; // Berkhin (the correct way of doing it)
      //TmpV[j] =  C*TmpV[j] + (1.0-C)*OneOver; // iGraph
    }
    double sum = 0;
    #pragma omp parallel for reduction(+:sum) schedule(dynamic,10000)
    for (int i = 0; i < TmpV.Len(); i++) { sum += TmpV[i]; }
    const double Leaked = (1.0-sum) / double(NNodes);

    double diff = 0;
    #pragma omp parallel for reduction(+:diff) schedule(dynamic,10000)
    for (int i = 0; i < PRankH.Len(); i++) { // re-instert leaked PageRank
      double NewVal = TmpV[i] + Leaked; // Berkhin
      //NewVal = TmpV[i] / sum;  // iGraph
      diff += fabs(NewVal-PRankH[i]);
      PRankH[i] = NewVal;
    }
    //printf("counts %d %d\n", hcount1, hcount2);
    if (diff < Eps) { break; }
  }
}

// Page Rank -- there are two different implementations (uncomment the desired 2 lines):
//   Berkhin -- (the correct way) see Algorithm 1 of P. Berkhin, A Survey on PageRank Computing, Internet Mathematics, 2005
//   iGraph -- iGraph implementation(which treats leaked PageRank in a funny way)
// This is a parallel, optimized version.
template<class PGraph>
void GetPageRankMP2(const PGraph& Graph, TIntFltH& PRankH, const double& C, const double& Eps, const int& MaxIter) {
  const int NNodes = Graph->GetNodes();
  //TIntV NV;
  TVec<typename PGraph::TObj::TNodeI> NV;
  //const double OneOver = 1.0/double(NNodes);
  PRankH.Gen(NNodes);

  //double t1 = omp_get_wtime();
  int MxId = -1;
  //time_t t = time(0);
  //printf("%s", ctime(&t));
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NV.Add(NI);
    PRankH.AddDat(NI.GetId(), 1.0/NNodes);
    int Id = NI.GetId();
    if (Id > MxId) {
      MxId = Id;
    }
    //IAssert(NI.GetId() == PRankH.GetKey(PRankH.Len()-1));
  }
  //t = time(0);
  //printf("%s", ctime(&t));
  //double t2 = omp_get_wtime();
  //printf("Pre-process %f\n", t2-t1);

  TFltV PRankV(MxId+1);
  TIntV OutDegV(MxId+1);
  //printf("MxId %d\n", MxId);

  #pragma omp parallel for schedule(dynamic,10000)
  for (int j = 0; j < NNodes; j++) {
    typename PGraph::TObj::TNodeI NI = NV[j];
    int Id = NI.GetId();
    PRankV[Id] = 1.0/NNodes;
    OutDegV[Id] = NI.GetOutDeg();
  }
  //double t3 = omp_get_wtime();
  //printf("Collect degrees %f\n", t3-t2);

  TFltV TmpV(NNodes);
  //double i1 = 0;
  //double i2 = 0;
  //double i3 = 0;
  for (int iter = 0; iter < MaxIter; iter++) {
    //time_t t = time(0);
    //printf("%s%d\n", ctime(&t),iter);
    //double ti1 = omp_get_wtime();
    #pragma omp parallel for schedule(dynamic,10000)
    for (int j = 0; j < NNodes; j++) {
      typename PGraph::TObj::TNodeI NI = NV[j];
      TFlt Tmp = 0;
      for (int e = 0; e < NI.GetInDeg(); e++) {
        const int InNId = NI.GetInNId(e);
        const int OutDeg = OutDegV[InNId];
        if (OutDeg > 0) {
          Tmp += PRankV[InNId] / OutDeg;
        }
      }
      TmpV[j] =  C*Tmp; // Berkhin (the correct way of doing it)
      //TmpV[j] =  C*TmpV[j]; // Berkhin (the correct way of doing it)
      ////TmpV[j] =  C*TmpV[j] + (1.0-C)*OneOver; // iGraph
    }
    //double ti2 = omp_get_wtime();
    //i1 += (ti2 - ti1);

    double sum = 0;
    #pragma omp parallel for reduction(+:sum) schedule(dynamic,10000)
    for (int i = 0; i < TmpV.Len(); i++) { sum += TmpV[i]; }
    const double Leaked = (1.0-sum) / double(NNodes);
    //double ti3 = omp_get_wtime();
    //i2 += (ti3 - ti2);

    double diff = 0;
    #pragma omp parallel for reduction(+:diff) schedule(dynamic,10000)
    for (int i = 0; i < NNodes; i++) {
      double NewVal = TmpV[i] + Leaked; // Berkhin
      //NewVal = TmpV[i] / sum;  // iGraph
      int Id = NV[i].GetId();
      diff += fabs(NewVal-PRankV[Id]);
      PRankV[Id] = NewVal;
    }
    //double ti4 = omp_get_wtime();
    //i3 += (ti4 - ti3);
    if (diff < Eps) { break; }
  }
  //double t4 = omp_get_wtime();
  //printf("Iterate %f\n", t4-t3);
  //printf("Compute new weights %f\n", i1);
  //printf("Compute sum %f\n", i2);
  //printf("Update weights %f\n", i3);

  #pragma omp parallel for schedule(dynamic,10000)
  for (int i = 0; i < NNodes; i++) {
    typename PGraph::TObj::TNodeI NI = NV[i];
    PRankH[i] = PRankV[NI.GetId()];
  }
  //double t5 = omp_get_wtime();
  //printf("Post-process %f\n", t5-t4);
}

template<class PGraph>
void GetPageRankMP3(const PGraph& Graph, TIntFltH& PRankH, const double& C, const double& Eps, const int& MaxIter) {
  const int NNodes = Graph->GetNodes();
  TVec<typename PGraph::TObj::TNodeI> NV;
  PRankH.Gen(NNodes);
  TIntIntH NIdH(NNodes);
  //double t1 = omp_get_wtime();
  int c = 0;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NV.Add(NI);
    PRankH.AddDat(NI.GetId(), 1.0/NNodes);
    int Id = NI.GetId();
    NIdH.AddDat(Id, c++);
  }
  //double t2 = omp_get_wtime();
  //printf("Preprocessing %f\n", t2-t1);

  TFltV PRankV(NNodes);
  TIntV OutDegV(NNodes);
  #pragma omp parallel for schedule(dynamic,10000)
  for (int j = 0; j < NNodes; j++) {
    int Id = NV[j].GetId();
    PRankV[NIdH.GetDat(Id)] = 1.0/NNodes;
    OutDegV[NIdH.GetDat(Id)] = NV[j].GetOutDeg();
  }
  //double t3 = omp_get_wtime();
  //printf("Collect degree %f\n", t3-t2);

  TFltV TmpV(NNodes);

  for (int iter = 0; iter < MaxIter; iter++) {
    #pragma omp parallel for schedule(dynamic,10000)
    for (int j = 0; j < NNodes; j++) {
      typename PGraph::TObj::TNodeI NI = NV[j];
      TFlt Tmp = 0;
      for (int e = 0; e < NI.GetInDeg(); e++) {
        const int InNId = NIdH.GetDat(NI.GetInNId(e));
        const int OutDeg = OutDegV[InNId];
        if (OutDeg > 0) {
          Tmp += PRankV[InNId] / OutDeg;
        }
      }
      TmpV[j] =  C*Tmp; // Berkhin (the correct way of doing it)
      ////TmpV[j] =  C*TmpV[j] + (1.0-C)*OneOver; // iGraph
    }
    double sum = 0;
    #pragma omp parallel for reduction(+:sum) schedule(dynamic,10000)
    for (int i = 0; i < TmpV.Len(); i++) { sum += TmpV[i]; }
    const double Leaked = (1.0-sum) / double(NNodes);

    double diff = 0;
    #pragma omp parallel for reduction(+:diff) schedule(dynamic,10000)
    for (int i = 0; i < NNodes; i++) {
      typename PGraph::TObj::TNodeI NI = NV[i];
      double NewVal = TmpV[i] + Leaked; // Berkhin
      //NewVal = TmpV[i] / sum;  // iGraph
      int Id = NIdH.GetDat(NI.GetId());
      diff += fabs(NewVal-PRankV[Id]);
      PRankV[Id] = NewVal;
    }
    if (diff < Eps) { break; }
  }
  //double t4 = omp_get_wtime();
  //printf("Iterate %f\n", t4-t3);

  #pragma omp parallel for schedule(dynamic,10000)
  for (int i = 0; i < NNodes; i++) {
    typename PGraph::TObj::TNodeI NI = NV[i];
    PRankH[i] = PRankV[NIdH.GetDat(NI.GetId())];
  }
  //double t5 = omp_get_wtime();
  //printf("Post-process %f\n", t5-t4);
}

template<class PGraph>
void GetPageRankMNetMP(const PGraph& Graph, TIntFltH& PRankH, const double& C, const double& Eps, const int& MaxIter) {
  const int NNodes = Graph->GetNodes();
  TVec<typename PGraph::TObj::TNodeI> NV;
  PRankH.Gen(NNodes);

  //double t1 = omp_get_wtime();
  int MxNTypeId = Graph->GetMxNTypeId();
  TVec<TFltV> PRankVV(MxNTypeId);
  TVec<TIntV> OutDegVV(MxNTypeId);

  for (int NTypeId = 0; NTypeId < MxNTypeId; NTypeId++) {
    int MxNId = Graph->GetMxNId(NTypeId);
    PRankVV[NTypeId] = TFltV(MxNId);
    OutDegVV[NTypeId] = TIntV(MxNId);
  }
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NV.Add(NI);
    PRankH.AddKey(NI.GetId()); // Put a placeholder
  }
  #pragma omp parallel for schedule(dynamic,10000)
  for (int j = 0; j < NNodes; j++) {
    typename PGraph::TObj::TNodeI NI = NV[j];
    int Id = NI.GetId();
    int NTypeId = Graph->GetNTypeId(Id);
    int LocalNId = Graph->GetLocalNId(Id);
    PRankVV[NTypeId][LocalNId] = 1.0/NNodes;
    OutDegVV[NTypeId][LocalNId] = NI.GetOutDeg();
  }
  //double t2 = omp_get_wtime();
  //printf("Pre-process %f\n", t2-t1);

  TFltV TmpV(NNodes);
  //double i1 = 0;
  //double i2 = 0;
  //double i3 = 0;
  for (int iter = 0; iter < MaxIter; iter++) {
    //double ti1 = omp_get_wtime();
    #pragma omp parallel for schedule(dynamic,10000)
    for (int j = 0; j < NNodes; j++) {
      typename PGraph::TObj::TNodeI NI = NV[j];
      TFlt Tmp = 0;
      for (int e = 0; e < NI.GetInDeg(); e++) {
        const int InNId = NI.GetInNId(e);
        const int InNTypeId = Graph->GetNTypeId(InNId);
        const int InLocalNId = Graph->GetLocalNId(InNId);
        const int OutDeg = OutDegVV[InNTypeId][InLocalNId];
        Tmp += (OutDeg > 0) ? (PRankVV[InNTypeId][InLocalNId] / OutDeg) : 0;
      }
      TmpV[j] =  C*Tmp; // Berkhin (the correct way of doing it)
    }
    //double ti2 = omp_get_wtime();
    //i1 += (ti2 - ti1);

    double sum = 0;
    #pragma omp parallel for reduction(+:sum) schedule(dynamic,10000)
    for (int i = 0; i < TmpV.Len(); i++) { sum += TmpV[i]; }
    const double Leaked = (1.0-sum) / double(NNodes);
    //double ti3 = omp_get_wtime();
    //i2 += (ti3 - ti2);

    double diff = 0;
    #pragma omp parallel for reduction(+:diff) schedule(dynamic,10000)
    for (int i = 0; i < NNodes; i++) {
      double NewVal = TmpV[i] + Leaked; // Berkhin
      //NewVal = TmpV[i] / sum;  // iGraph
      int Id = NV[i].GetId();
      int NTypeId = Graph->GetNTypeId(Id);
      int LocalNId = Graph->GetLocalNId(Id);
      diff += fabs(NewVal-PRankVV[NTypeId][LocalNId]);
      PRankVV[NTypeId][LocalNId] = NewVal;
    }
    //double ti4 = omp_get_wtime();
    //i3 += (ti4 - ti3);
    //printf("counts %d %d\n", hcount1, hcount2);
    if (diff < Eps) { break; }
  }
  //double t3 = omp_get_wtime();
  //printf("Iterate %f\n", t3-t2);
  //printf("Compute new weights %f\n", i1);
  //printf("Compute sum %f\n", i2);
  //printf("Update weights %f\n", i3);

  #pragma omp parallel for schedule(dynamic,10000)
  for (int i = 0; i < NNodes; i++) {
    typename PGraph::TObj::TNodeI NI = NV[i];
    int NTypeId = NI.GetTypeId();
    int LocalNId = NI.GetLocalId();
    PRankH[i] = PRankVV[NTypeId][LocalNId];
  }
  //double t4 = omp_get_wtime();
  //printf("Post-process %f\n", t4-t3);
}
#endif // USE_OPENMP

}; // namespace TSnap

