/////////////////////////////////////////////////
// Community detection algorithms
namespace TSnap {


namespace TSnapDetail {
// GIRVAN-NEWMAN algorithm
//	1. The betweenness of all existing edges in the network is calculated first.
//	2. The edge with the highest betweenness is removed.
//	3. The betweenness of all edges affected by the removal is recalculated.
//	4. Steps 2 and 3 are repeated until no edges remain.
//  Girvan M. and Newman M. E. J., Community structure in social and biological networks, Proc. Natl. Acad. Sci. USA 99, 7821-7826 (2002)
// Keep removing edges from Graph until one of the connected components of Graph splits into two.
void CmtyGirvanNewmanStep(PUNGraph& Graph, TIntV& Cmty1, TIntV& Cmty2) {
  TIntPrFltH BtwEH;
  TBreathFS<PUNGraph> BFS(Graph);
  Cmty1.Clr(false);  Cmty2.Clr(false);
  while (true) {
    TSnap::GetBetweennessCentr(Graph, BtwEH);
    BtwEH.SortByDat(false);
    if (BtwEH.Empty()) { return; }
    const int NId1 = BtwEH.GetKey(0).Val1;
    const int NId2 = BtwEH.GetKey(0).Val2;
    Graph->DelEdge(NId1, NId2);
    BFS.DoBfs(NId1, true, false, NId2, TInt::Mx);
    if (BFS.GetHops(NId1, NId2) == -1) { // two components
      TSnap::GetNodeWcc(Graph, NId1, Cmty1);
      TSnap::GetNodeWcc(Graph, NId2, Cmty2);
      return;
    }
  }
}

// Connected components of a graph define clusters
// OutDegH and OrigEdges stores node degrees and number of edges in the original graph
double _GirvanNewmanGetModularity(const PUNGraph& G, const TIntH& OutDegH, const int& OrigEdges, TCnComV& CnComV) {
  TSnap::GetWccs(G, CnComV); // get communities
  double Mod = 0;
  for (int c = 0; c < CnComV.Len(); c++) {
    const TIntV& NIdV = CnComV[c]();
    double EIn=0, EEIn=0;
    for (int i = 0; i < NIdV.Len(); i++) {
      TUNGraph::TNodeI NI = G->GetNI(NIdV[i]);
      EIn += NI.GetOutDeg();
      EEIn += OutDegH.GetDat(NIdV[i]);
    }
    Mod += (EIn-EEIn*EEIn/(2.0*OrigEdges));
  }
  if (Mod == 0) { return 0; }
  else { return Mod/(2.0*OrigEdges); }
}

TIntFltH MapEquationNew2Modules(PUNGraph& Graph, TIntH& Module, TIntFltH& Qi, int a, int b){
  TIntFltH Qi1;
  Qi1 = Qi;	
  float InModule=0.0, OutModule=0.0, Val;
  int Mds[2] = {a,b};
  for (int i=0; i<2; i++) {
    InModule=0.0, OutModule=0.0;
    if (Qi1.IsKey(Mds[i])){
      int CentralModule = Mds[i];
      for (TUNGraph::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
        if (Module.GetDat(EI.GetSrcNId()) == Module.GetDat(EI.GetDstNId()) && Module.GetDat(EI.GetDstNId()) == CentralModule) {
          InModule += 1.0;
        } else if ((Module.GetDat(EI.GetSrcNId()) == CentralModule && Module.GetDat(EI.GetDstNId()) != CentralModule) || (Module.GetDat(EI.GetSrcNId()) != CentralModule && Module.GetDat(EI.GetDstNId()) == CentralModule)) {
          OutModule +=1.0;
        }
	    }
      Val = 0.0;
      if (InModule+OutModule > 0) {
        Val = OutModule/(InModule+OutModule);
      }
      Qi1.DelKey(Mds[i]);
      Qi1.AddDat(Mds[i],Val);
    } else {
      Qi1.DelKey(Mds[i]);
      Qi1.AddDat(Mds[i],0.0);
    }
  }
	
  return Qi1;
}

float Equation(PUNGraph& Graph, TIntFltH& PAlpha,float& SumPAlphaLogPAlpha, TIntFltH& Qi){
  float SumPAlpha = 1.0, SumQi = 0.0, SumQiLogQi=0.0, SumQiSumPAlphaLogQiSumPAlpha = 0.0;
  for (int i=0; i<Qi.Len(); i++) {
    SumQi += Qi[i];
    SumQiLogQi += Qi[i]*log(Qi[i]);
    SumQiSumPAlphaLogQiSumPAlpha += (Qi[i]+SumPAlpha)*log(Qi[i]+SumPAlpha);
  }
  return (SumQi*log(SumQi)-2*SumQiLogQi-SumPAlphaLogPAlpha+SumQiSumPAlphaLogQiSumPAlpha);
}

} // namespace TSnapDetail

// Maximum modularity clustering by Girvan-Newman algorithm (slow)
//  Girvan M. and Newman M. E. J., Community structure in social and biological networks, Proc. Natl. Acad. Sci. USA 99, 7821-7826 (2002)
double CommunityGirvanNewman(PUNGraph& Graph, TCnComV& CmtyV) {
  TIntH OutDegH;
  const int NEdges = Graph->GetEdges();
  for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    OutDegH.AddDat(NI.GetId(), NI.GetOutDeg());
  }
  double BestQ = -1; // modularity
  TCnComV CurCmtyV;
  CmtyV.Clr();
  TIntV Cmty1, Cmty2;
  while (true) {
    TSnapDetail::CmtyGirvanNewmanStep(Graph, Cmty1, Cmty2);
    const double Q = TSnapDetail::_GirvanNewmanGetModularity(Graph, OutDegH, NEdges, CurCmtyV);
    //printf("current modularity: %f\n", Q);
    if (Q > BestQ) {
      BestQ = Q; 
      CmtyV.Swap(CurCmtyV);
    }
    if (Cmty1.Len()==0 || Cmty2.Len() == 0) { break; }
  }
  return BestQ;
}

// Rosvall-Bergstrom community detection algorithm based on information theoretic approach.
// See: Rosvall M., Bergstrom C. T., Maps of random walks on complex networks reveal community structure, Proc. Natl. Acad. Sci. USA 105, 1118-1123 (2008)
double Infomap(PUNGraph& Graph, TCnComV& CmtyV){	
  TIntH DegH; 
  TIntFltH PAlpha; // probability of visiting node alpha
  TIntH Module; // module of each node
  TIntFltH Qi; // probability of leaving each module
  float SumPAlphaLogPAlpha = 0.0;
  int Br = 0;
  const int e = Graph->GetEdges(); 

  // initial values
  for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    DegH.AddDat(NI.GetId(), NI.GetDeg());
    float d = ((float)NI.GetDeg()/(float)(2*e));
    PAlpha.AddDat(NI.GetId(), d);
    SumPAlphaLogPAlpha += d*log(d);
    Module.AddDat(NI.GetId(),Br);
    Qi.AddDat(Module[Br],1.0);
    Br+=1;
  }

  float MinCodeLength = TSnapDetail::Equation(Graph,PAlpha,SumPAlphaLogPAlpha,Qi);
  float NewCodeLength, PrevIterationCodeLength = 0.0;
  int OldModule, NewModule;

  do {
    PrevIterationCodeLength = MinCodeLength;
      for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
        MinCodeLength = TSnapDetail::Equation(Graph, PAlpha, SumPAlphaLogPAlpha, Qi);
        for (int i=0; i<DegH.GetDat(NI.GetId()); i++) {
          OldModule = Module.GetDat(NI.GetId());
          NewModule = Module.GetDat(NI.GetNbrNId(i));
          if (OldModule!=NewModule){
            Module.DelKey(NI.GetId()); 
            Module.AddDat(NI.GetId(),NewModule);
            Qi = TSnapDetail::MapEquationNew2Modules(Graph,Module,Qi,OldModule, NewModule);
            NewCodeLength = TSnapDetail::Equation(Graph,PAlpha,SumPAlphaLogPAlpha, Qi);
            if (NewCodeLength<MinCodeLength) {
              MinCodeLength = NewCodeLength;
              OldModule = NewModule;
            } else {
              Module.DelKey(NI.GetId());
              Module.AddDat(NI.GetId(),OldModule);
            }
          }
       }
     }
   } while (MinCodeLength<PrevIterationCodeLength);

  Module.SortByDat(true);
  int Mod=-1;
  for (int i=0; i<Module.Len(); i++) {
    if (Module[i]>Mod){
      Mod = Module[i];
      TCnCom t;
      for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++){
        if (Module.GetDat(NI.GetId())==Mod)
        t.Add(NI.GetId());
      }
      CmtyV.Add(t);
    }
  }

  return MinCodeLength;
}

namespace TSnapDetail {
/// Clauset-Newman-Moore community detection method.
/// At every step two communities that contribute maximum positive value to global modularity are merged.
/// See: Finding community structure in very large networks, A. Clauset, M.E.J. Newman, C. Moore, 2004
class TCNMQMatrix {
private:
  struct TCmtyDat {
    double DegFrac;
    TIntFltH NIdQH;
    int MxQId;
    TCmtyDat() : MxQId(-1) { }
    TCmtyDat(const double& NodeDegFrac, const int& OutDeg) : 
      DegFrac(NodeDegFrac), NIdQH(OutDeg), MxQId(-1) { }
    void AddQ(const int& NId, const double& Q) { NIdQH.AddDat(NId, Q);
      if (MxQId==-1 || NIdQH[MxQId]<Q) { MxQId=NIdQH.GetKeyId(NId); } }
    void UpdateMaxQ() { MxQId=-1; 
      for (int i = -1; NIdQH.FNextKeyId(i); ) { 
        if (MxQId==-1 || NIdQH[MxQId]< NIdQH[i]) { MxQId=i; } } }
    void DelLink(const int& K) { const int NId=GetMxQNId(); 
      NIdQH.DelKey(K); if (NId==K) { UpdateMaxQ(); }  }
    int GetMxQNId() const { return NIdQH.GetKey(MxQId); }
    double GetMxQ() const { return NIdQH[MxQId]; }
  };
private:
  THash<TInt, TCmtyDat> CmtyQH;
  THeap<TFltIntIntTr> MxQHeap;
  TUnionFind CmtyIdUF;
  double Q;
public:
  TCNMQMatrix(const PUNGraph& Graph) : CmtyQH(Graph->GetNodes()), 
    MxQHeap(Graph->GetNodes()), CmtyIdUF(Graph->GetNodes()) { Init(Graph); }
  void Init(const PUNGraph& Graph) {
    const double M = 0.5/Graph->GetEdges(); // 1/2m
    Q = 0.0;
    for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
      CmtyIdUF.Add(NI.GetId());
      const int OutDeg = NI.GetOutDeg();
      if (OutDeg == 0) { continue; }
      TCmtyDat& Dat = CmtyQH.AddDat(NI.GetId(), TCmtyDat(M * OutDeg, OutDeg));
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        const int DstNId = NI.GetOutNId(e);
        const double DstMod = 2 * M * (1.0 - OutDeg * Graph->GetNI(DstNId).GetOutDeg() * M);
        Dat.AddQ(DstNId, DstMod);
      }
      Q += -1.0*TMath::Sqr(OutDeg*M);
      if (NI.GetId() < Dat.GetMxQNId()) {
        MxQHeap.Add(TFltIntIntTr(Dat.GetMxQ(), NI.GetId(), Dat.GetMxQNId())); }
    }
    MxQHeap.MakeHeap();
  }
  TFltIntIntTr FindMxQEdge() {
    while (true) {
      if (MxQHeap.Empty()) { break; }
      const TFltIntIntTr TopQ = MxQHeap.PopHeap();
      if (! CmtyQH.IsKey(TopQ.Val2) || ! CmtyQH.IsKey(TopQ.Val3)) { continue; }
      if (TopQ.Val1!=CmtyQH.GetDat(TopQ.Val2).GetMxQ() && TopQ.Val1!=CmtyQH.GetDat(TopQ.Val3).GetMxQ()) { continue; }
      return TopQ;
    }
    return TFltIntIntTr(-1, -1, -1);
  }
  bool MergeBestQ() {
    const TFltIntIntTr TopQ = FindMxQEdge();
    if (TopQ.Val1 <= 0.0) { return false; }
    // joint communities
    const int I = TopQ.Val3;
    const int J = TopQ.Val2;
    CmtyIdUF.Union(I, J); // join
    Q += TopQ.Val1;
    TCmtyDat& DatJ = CmtyQH.GetDat(J);
    { TCmtyDat& DatI = CmtyQH.GetDat(I);
    DatI.DelLink(J);  DatJ.DelLink(I);
    for (int i = -1; DatJ.NIdQH.FNextKeyId(i); ) {
      const int K = DatJ.NIdQH.GetKey(i);
      TCmtyDat& DatK = CmtyQH.GetDat(K);
      double NewQ = DatJ.NIdQH[i];
      if (DatI.NIdQH.IsKey(K)) { NewQ = NewQ+DatI.NIdQH.GetDat(K);  DatK.DelLink(I); }     // K connected to I and J
      else { NewQ = NewQ-2*DatI.DegFrac*DatK.DegFrac; }  // K connected to J not I
      DatJ.AddQ(K, NewQ);
      DatK.AddQ(J, NewQ);
      MxQHeap.PushHeap(TFltIntIntTr(NewQ, TMath::Mn(J,K), TMath::Mx(J,K)));
    }
    for (int i = -1; DatI.NIdQH.FNextKeyId(i); ) {
      const int K = DatI.NIdQH.GetKey(i);
      if (! DatJ.NIdQH.IsKey(K)) { // K connected to I not J
        TCmtyDat& DatK = CmtyQH.GetDat(K);
        const double NewQ = DatI.NIdQH[i]-2*DatJ.DegFrac*DatK.DegFrac; 
        DatJ.AddQ(K, NewQ);
        DatK.DelLink(I);
        DatK.AddQ(J, NewQ);
        MxQHeap.PushHeap(TFltIntIntTr(NewQ, TMath::Mn(J,K), TMath::Mx(J,K)));
      }
    } 
    DatJ.DegFrac += DatI.DegFrac; }
    if (DatJ.NIdQH.Empty()) { CmtyQH.DelKey(J); } // isolated community (done)
    CmtyQH.DelKey(I);
    return true;
  }
  static double CmtyCMN(const PUNGraph& Graph, TCnComV& CmtyV) {
    TCNMQMatrix QMatrix(Graph);
    // maximize modularity
    while (QMatrix.MergeBestQ()) { }
    // reconstruct communities
    THash<TInt, TIntV> IdCmtyH;
    for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
      IdCmtyH.AddDat(QMatrix.CmtyIdUF.Find(NI.GetId())).Add(NI.GetId()); 
    }
    CmtyV.Gen(IdCmtyH.Len());
    for (int j = 0; j < IdCmtyH.Len(); j++) {
      CmtyV[j].NIdV.Swap(IdCmtyH[j]);
    }
    return QMatrix.Q;
  }
};

} // namespace TSnapDetail

double CommunityCNM(const PUNGraph& Graph, TCnComV& CmtyV) {
  return TSnapDetail::TCNMQMatrix::CmtyCMN(Graph, CmtyV);
}

}; //namespace TSnap
