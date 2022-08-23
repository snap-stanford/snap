#include "stdafx.h"
#include "hysgen.h"

void THysgenUtil::DumpCmtyVV(const TStr OutFNm, TVec<TIntV>& CmtyVV, TIntStrH& NIDNmH) {
  FILE* F = fopen(OutFNm.CStr(), "wt");
  for (int c = 0; c < CmtyVV.Len(); c++) {
    fprintf(F, "\n\n*** COMMUNITY #%d, Length=%d ***\n", c+1, CmtyVV[c].Len());
    for (int u = 0; u < CmtyVV[c].Len(); u++) {
      if (NIDNmH.IsKey(CmtyVV[c][u])){
        fprintf(F, "%s\t", NIDNmH.GetDat(CmtyVV[c][u]).CStr());
      }
      else {
        fprintf(F, "%d\t", (int) CmtyVV[c][u]);
      }
    }
    fprintf(F, "\n");
  }
  fclose(F);
}

void THysgenUtil::DumpCmtyVH(const TStr OutFNm, TVec<TIntFltH>& CmtyVH, TIntStrH& NIDNmH) {
  FILE* F = fopen(OutFNm.CStr(), "wt");
  int UId;
  for (int c = 0; c < CmtyVH.Len(); c++) {
    fprintf(F, "\n\n*** COMMUNITY #%d, Length=%d ***\n", c+1, CmtyVH[c].Len());
    for (int u = 0; u < CmtyVH[c].Len(); u++) {
      UId = CmtyVH[c].GetKey(u);
      if (NIDNmH.IsKey(UId)){
        fprintf(F, "%s: ", NIDNmH.GetDat(UId).CStr());
      }
      else {
        fprintf(F, "%d: ", UId);
      }
      fprintf(F, "%f,\t", (double) CmtyVH[c].GetDat(UId));
    }
    fprintf(F, "\n");
  }
  fclose(F);
}

PHGraph THysgenUtil::LoadEdgeList(const TStr& InFNm, TStrHash<TInt>& NodeNameH,
                                  TSsFmt SsFmt) {
  TSsParser Ss(InFNm, SsFmt);
  PHGraph PG = THGraph::New();
  THGraph& G = *PG;
  TStrHash<TInt> StrSet(Mega(1), true);
  int EIdCtr = 0;
  int RedunEdgesCnt = 0; 
  std::string Curr;
  while (Ss.Next()) {
    TIntSet NIdH;
    NIdH.Gen(Ss.Len());
    if (Ss.Len() == 0) { continue; }
    Curr = Ss[0];
    if (Curr.compare(0,1, "#") == 0) { continue; } // Comments are allowed in the file
    for (int i = 0; i < Ss.Len(); i++){
      int ENId = StrSet.AddKey(Ss[i]);
      if (! G.IsNode(ENId)) { G.AddNode(ENId, Ss[i]); }
      NIdH.AddKey(ENId);
    }
    if (G.AddEdge(NIdH,EIdCtr) >= 0) {
      EIdCtr++;
    } else {RedunEdgesCnt++; }
  }
  printf("\nNumber of redundant edges: %d:\n", RedunEdgesCnt);
  NodeNameH = StrSet;
  NodeNameH.Pack();
  G.Defrag();
  return PG;
}

template<class PHGraph>
double THysgenUtil::GetConductance(const PHGraph& Graph, const TIntSet& CmtyS,
                                   const int N2Edges) {
  TIntV NeiNV;
  int Vol = 0,  Cut = 0;
  double Phi = 0.0;
  for (int i = 0; i < CmtyS.Len(); i++) {
    if (! Graph->IsNode(CmtyS[i])) { continue; }
    THGraph::TNodeI  NI = Graph->GetNI(CmtyS[i]);
    NI.GetNbrNodes(NeiNV);
    for (int nei = 0; nei < NeiNV.Len(); nei++) {
      if (! CmtyS.IsKey(NeiNV[nei])) { Cut += 1; }
    }
    Vol += NI.Get2Edges();
  }
  int N2Edges2 = N2Edges * 2;
  if (Vol != N2Edges2) {
    if (N2Edges2 - Vol < Vol) { Phi = Cut / double (N2Edges2 - Vol); }
    else if (Vol == 0) { Phi = 0.0; }
    else { Phi = Cut / double(Vol); }
  } else {
    if (Vol == N2Edges2) { Phi = 1.0; }
  }
  return Phi;
}

template<class PHGraph>
void THysgenUtil::GetNbhCom(const PHGraph& Graph, const int NID, TIntSet& NBCmtyS) {
  THGraph::TNodeI NI = Graph->GetNI(NID);
  NBCmtyS.Gen(NI.GetNbrNodes() + 1);
  NBCmtyS.AddKey(NID);
  for (int e = 0; e < NI.GetNbrNodes(); e++) {
    NBCmtyS.AddKey(NI.GetNbrNId(e));
  }
}

template<class PHGraph>
void THysgenUtil::GetPhiNIdPrV(const PHGraph &G, TFltIntPrV &PhiNIdPrV, const int MinComSiz) {
  PhiNIdPrV.Gen(G->GetNodes(), 0);
  const int N2Edges = G->Get2Edges();
  TExeTm RunTm;
  //compute conductance of neighborhood community
  for (THGraph::TNodeI NI = G->BegNI(); NI < G->EndNI(); NI++) {
    TIntSet NBCmty(NI.GetNbrNodes() + 1);
    double Phi;
    if (NI.GetNbrNodes() < MinComSiz) { 
      Phi = 1.0;
    } else {
      THysgenUtil::GetNbhCom<PHGraph>(G, NI.GetId(), NBCmty);
      Phi = THysgenUtil::GetConductance(G, NBCmty, N2Edges);
    }
    PhiNIdPrV.Add(TFltIntPr(Phi, NI.GetId()));
  }
  PhiNIdPrV.Sort(true);
  printf("conductance computation completed [%s]\n", RunTm.GetTmStr());
  fflush(stdout);
}

void THysgenUtil::GetBinLocs(const int& DecNum, TIntV& LocsV, const TIntV& NodMapV) {
  int MaxShift = ceil(TMath::Log2((double) DecNum+1));
  int NumCopy = DecNum;
  for (int j = 0; j < MaxShift; j++) {
    if ( NumCopy / 2.0 != floor(NumCopy / 2.0) ) {
      LocsV.Add(NodMapV[j]);
    }
    NumCopy = int(NumCopy / 2.0);
  }
}

void THysgen::SetGraph(const PHGraph& GraphPt) {
  HONEIdsV.Gen(GraphPt->GetNodes());
  HONNIdsV.Gen(GraphPt->GetNodes());
  HOKIDSV.Gen(GraphPt->GetNodes());
  TIntV NIDV;
  GraphPt->GetNIdV(NIDV);
  NIdToIdx.Gen(NIDV.Len());
  NIdToIdx.AddKeyV(NIDV);
  // check that nodes IDs are {0,1,..,Nodes-1}
  printf("rearrage nodes\n");
  G = TSnap::GetSubGraph(GraphPt, NIDV);
  for (int nid = 0; nid < G->GetNodes(); nid++) {
    IAssert(G->IsNode(nid));
  }
  
  PrNoCom = 1.0 / (double) G->GetNodes();
  if (1.0 / PrNoCom > sqrt(TFlt::Mx)) { PrNoCom = 0.99 / sqrt(TFlt::Mx); } // to prevent overflow
  NegWgt = 1.0;
}

void THysgen::LoadComInit(const TStr& InFNm, TSsFmt SsFmt) {
  TSsParser Ss(InFNm, SsFmt);
  TVec<TStrFltH> ComsVH;
  TStrSet NodsInFile(G->GetNodes());
  int CId = 0;
  std::string Curr;
  printf("Loading community initializations from file ...\n");
  while (Ss.Next()) {
    if (Ss.Len() == 0) { continue; }
    Curr = Ss[0];
    if (Curr.compare(0,1, "#") == 0 || Curr.compare(0,1, "*") == 0) { continue; } // Comments are allowed in the file
    TStrFltH CurComH;
    for (int i = 0; i < Ss.Len(); i++){
      Curr = Ss[i];
      TStr NName; double SN;
      int SepLoc = Curr.find_first_of(":");
      if (SepLoc == -1) {
        NName = Curr.c_str();
        SN = InitVal;
      } else {
        NName = Curr.substr(0, SepLoc).c_str();
        SN = std::strtod(Curr.substr(SepLoc + 2, 8).c_str(), NULL);
      }
      CurComH.AddDat(NName, SN);
      NodsInFile.AddKey(NName);
    }
    ComsVH.Add(CurComH);
  }
  for (int c = 0; c < ComsVH.Len(); c++) {
    for (THGraph::TNodeI NI = G->BegNI(); NI < G->EndNI(); NI++) {
      AddNCom(NI.GetId(), c, InitNullS);
      if (ComsVH[c].IsKey(NI.GetName())) {
        AddNCom(NI.GetId(), c, ComsVH[c].GetDat(NI.GetName()));
      }
    }
  }
  printf("\n");
  for (int c = 0; c < GetNumComs(); c++) {
    printf("comm %d:\t\t", c);
    for (THGraph::TNodeI NI = G->BegNI(); NI < G->EndNI(); NI++) {
      if (GetNCom(NI.GetId(),c) > InitNullS) {
        printf("%s,%0.2f\t",NI.GetName().CStr(), GetNCom(NI.GetId(),c));
      }
    }
    printf("\n");
  }
  printf("\n Likelihood of the initial community memberships from the file is: %f\n", Likelihood());
}

void THysgen::ComInit(const int InitComs, const int MinComSiz, const double PerturbDens) {
  IAssertR(InitVal>0.0 || InitNullS>0.0, "Error: At least one of -ic or -in parameters should be set > 0. Cannot start without any initalization.");
  S.Gen(G->GetNodes()); 
  time_t InitTime = time(NULL);
  printf("\n** Community Initializations **  Initializing the communities over %d nodes is started...\n", S.Len());
  NumCIdNV.Gen(InitComs);
  NumComs = InitComs;
  if (InitNullS > 0.0) {
    UniformComInit(InitComs);
    printf("\n** Community Initializations **  First step of initializing the null memberships to the communities is finished [%lu sec]\n", time(NULL) - InitTime);
    InitTime = time(NULL);
    InitEdgeProb();
    printf("\n** Community Initializations **  Taylor computation is finsihed [%lu sec]\n", time(NULL) - InitTime);
  }
  if (InitNullS != InitVal) {
    InitTime = time(NULL);
    NeighborComInit(MinComSiz, InitNullS==0.0);
    printf("\n** Community Initializations **  Informed initial community assignment is finished [%lu sec]\n", time(NULL) - InitTime);
    if (InitNullS==0.0) {
      InitTime = time(NULL);
      InitEdgeProb();
      printf("\n** Community Initializations **  Taylor computation is finsihed [%lu sec]\n", time(NULL) - InitTime);
    }
  }
  // If no informed initialization in practice, a random perturbation on the equal initial memberships
  if (InitNullS != InitVal) {
    RandomComPerturb(PerturbDens);
  } else { RandomComPerturb(); }
}
void THysgen::UniformComInit(const int InitComs) {
  for (int c=0; c < NumComs; c++) {
    for (THGraph::TNodeI NI = G->BegNI(); NI < G->EndNI(); NI++) {
      AddNCom(NI.GetId(), c, InitNullS, true);
    }
  }
}
void THysgen::RandomComPerturb(double Density) {
  if (Density <= 0) { return; }
  for (int c=0; c < NumComs; c++) {
    for (THGraph::TNodeI NI = G->BegNI(); NI < G->EndNI(); NI++) {
      if (GetNCom(NI.GetId(), c) == 0) { continue; } // Reserve for later usages
      if (Rnd.GetUniDev() < Density) { AddNCom(NI.GetId(), c, InitVal*Rnd.GetUniDev()); }
    }
  }
}
void THysgen::NeighborComInit(const int MinComSiz, const bool& IsInit) {
  TFltIntPrV PhiNIdPrV;
  THysgenUtil::GetPhiNIdPrV<PHGraph>(G, PhiNIdPrV, MinComSiz);
  NeighborComInit(PhiNIdPrV, IsInit);
}
void THysgen::NeighborComInit(TFltIntPrV& PhiNIdPrV, const bool& IsInit) {
  PhiNIdPrV.Sort(true);
  TIntV NIdV;
  TIntV EIdV;
  THGraph::TNodeI NI;
  THGraph::TEdgeI EI;
  TIntSet InvalidNIDS(S.Len());
  //choose nodes with local minimum in conductance
  TIntV NumCIdNDefV(NumComs, 0);
  int CurCID = 0;
  for (int ui = 0; ui < PhiNIdPrV.Len(); ui++) {
    if (PhiNIdPrV[ui].Val1 == 0.0) { // Isolated nodes are not of our interest for community seeds.
      continue;
    }
    int UID = PhiNIdPrV[ui].Val2;
    fflush(stdout);
    if (InvalidNIDS.IsKey(UID)) { continue; }
    //add the node and its neighbors to the current community
    AddNCom(UID, CurCID, InitVal, IsInit);
    InvalidNIDS.AddKey(UID);
    NI = G->GetNI(UID);
    fflush(stdout);
    NI.GetNbrNodes(NIdV);
    AddNCom(NIdV, CurCID, InitVal, IsInit);
    NumCIdNDefV[CurCID] = NIdV.Len()+1;
    InvalidNIDS.AddKeyV(NIdV);
    CurCID++;
    if (CurCID >= NumComs) { break; }
    fflush(stdout);
  }
  if (NumComs > CurCID) {
    printf("%d communities needed to fill randomly\n", NumComs - CurCID);
  }
  for (int c = 0; c < NumCIdNV.Len(); c++) {
    if (NumCIdNDefV[c] == 0) {
      printf("Empty comminities remained after initialization. Members are getting assigned randomly!!\n\n");
      int ComSz = 10;
      for (int u = 0; u < ComSz; u++) {
        int UID = Rnd.GetUniDevInt(G->GetNodes());
        AddNCom(UID, c, InitVal, IsInit);
      }
    }
  }
}

void THysgen::InitEdgeProb(){
  TInt NId;
  TInt CIterKey;
  ProbEdgCommHH.Gen(G->GetEdges());
  ProbNotEdgH.Gen(G->GetEdges());
  ProbEdgH.Gen(G->GetEdges());
  ProbENoiseV.Gen(G->GetNodes(),1);
  ProbENoiseV[0] = 1;
  
  // Initializing the DP Matrix with zeros, to prevent new memory assignment in each use.
  int DPMatSize = NumComs + 1 + (SNoise>0); // If there is an active SNoise, must be counted in the matrix.
  AuxDPEdgVV.Gen(DPMatSize);
  for (int i = 0; i < DPMatSize; i++) {
    AuxDPEdgVV[i].Gen(DPMatSize);
    for (int j = 0; j < DPMatSize; j++) {
      AuxDPEdgVV[i][j] = 0;
    }
  }
  // INITIALIZING the SAVED EDGE PROBABILITIES (For lower computation)
  for (THGraph::TEdgeI EI = G->BegEI(); EI < G->EndEI(); EI++) {
    TIntV NumMmbrsInEdgV(NumComs);
    TIntFltH ProdH(NumComs);// S[v][1,..,C]
    TIntV NeiNV;
    EI.GetNodesV(NeiNV);
    for (int n = 0; n < NeiNV.Len(); n++) {
      NId = NeiNV[n];
      for (TIntFltH::TIter SNI = S[NId].BegI(); SNI < S[NId].EndI(); SNI++) {
        CIterKey = SNI.GetKey();
        if (!ProdH.IsKey(CIterKey)) { ProdH.AddDat(CIterKey, 1.0); }
        ProdH.AddDat(CIterKey, ProdH.GetDat(CIterKey)*SNI.GetDat());
        NumMmbrsInEdgV[CIterKey] ++;
      }
    }
    /// If at least one node in the edge has a ZERO membership to a
    /// community, the product of node memberships for that edge should
    /// be zero.
    /// Note that when NumMmbrsInEdgV[CId]==0, ProdH is not instantiated for
    /// that CId, so we shouldn't try to remove it.
    for (int CId = 0; CId < NumComs; CId++){
      if (NumMmbrsInEdgV[CId] > 0 && NumMmbrsInEdgV[CId] < NeiNV.Len()) {
        ProdH.DelKey(CId);
      }
    }
    AddECom(EI.GetId(), ProdH);
  }
  if (InitNullS > 0) { InitPrAllEdgesS(InitNullS, true);
  } else { InitPrAllEdgesS(InitVal, false); }
}

void THysgen::UpdateUEdgesProb(const int& UId, const int& CId, const double& SUNew){
  double SUOld = GetNCom(UId,CId);
  TIntV EIdV;
  TIntFltH ProbEH;
  double PrECOld;
  double PrECNew;
  double Denom;
  int EId;
  G->GetNI(UId).GetEIDs(EIdV);
  for (int e = 0; e < EIdV.Len(); e++) {
    EId = EIdV[e];
    PrECOld = GetECom(EId, CId);
    if (SUOld <= 0.0) { Denom = 1; } else { Denom = SUOld; }
    if (SUOld < 0.0) { printf("<<< OUT OF RANGE! LINE cpp 626 >>>\n"); } //todo debugTIntV ENodesV;
    PrECNew = PrECOld*SUNew/Denom;
    if (PrECNew > 0) {
      AddECom(EId, CId, PrECNew);
    } else {
      DelECom(EId, CId);
    }
  }
}

void THysgen::InitPrAllEdgesS(const double& DefVal, const bool& IsEqualComms){
  if (SumPrPsblEdgesPow_nVV.Len()==0) {
    SumPrPsblEdgesPow_nVV.Gen(NumCIdNV.Len(), 0);
  } else {
    SumPrPsblEdgesPow_nVV.Clr();
    SumPrPsblEdgesPow_nVV.Gen(NumCIdNV.Len(), 0);
  }
  TFlt SToN = 1.0;
  for (int c=0; c<NumCIdNV.Len(); c++){
    // 0 index is For computation only. Will not be counted (Our formulation considers n>=1)
    TFltV PsiV;
    PsiV.Add(1.0);
    SToN = 1.0;
    for (int n=1; (n <= MinTayN && PsiV[n-1] > 0.0) || (n<=TayN && PsiV[n-1]>TayThresh) ; n++){ //n: Taylor coefficient order
      SToN *= DefVal;
      PsiV.Add(pow(SToN+1.0, NumCIdNV[c]) - 1);
    }
    SumPrPsblEdgesPow_nVV.Add(PsiV);
    if (IsEqualComms && NumCIdNV.Len()>1) {
      for (int c=1; c<NumCIdNV.Len(); c++) {
        TFltV PsiVCopy(SumPrPsblEdgesPow_nVV[0]);
        SumPrPsblEdgesPow_nVV.Add(PsiVCopy);
      }
      printf("\n Done the same initialization for the rest of %d communities...\n",NumComs()-1);
      break;
    }
  }
}

void THysgen::UpdatePrAllEdgesS(const int &UID, const int &CID, const double& SNNew){
  TFltV PsiV(SumPrPsblEdgesPow_nVV[CID].Len());
  UpdatePrAllEdgesS(PsiV, UID, CID, SNNew, true);
}

void THysgen::UpdatePrAllEdgesS(TFltV& PsiV, const int& UID, const int& CID, const double& SNNew, const bool IsApplyChange){
  PsiV[0] = 1.0; /// The 0th index is meaningless and is never used
  double SNOld = GetNCom(UID,CID);
  double SToNOld = 1.0, SToNNew = 1.0;
  int nLast;
  for (int n=1; (n <= MinTayN && PsiV[n-1] > 0.0) ||
                (n <= TayN    && PsiV[n-1]>TayThresh); n++){
    if (n >= PsiV.Len()) {
      PsiV.Add(0.0);
    }
    SToNOld *= SNOld; SToNNew *= SNNew;
    PsiV[n] = ((SToNNew+1)/(SToNOld+1)) *
                                    (SumPrPsblEdgesPow_nVV[CID][n]-SToNOld) + SToNNew; 
    nLast = n;
  }
  if (IsApplyChange) {
    for (int n = 1; n <= nLast; n++) {
      if (n >= SumPrPsblEdgesPow_nVV[CID].Len()) {
        SumPrPsblEdgesPow_nVV[CID].Add(-1);
      }
      SumPrPsblEdgesPow_nVV[CID][n] = PsiV[n];
    }
    nLast++;
    while (nLast < SumPrPsblEdgesPow_nVV[CID].Len()) {
      SumPrPsblEdgesPow_nVV[CID][nLast] = 0.0;
      nLast++;
    }
  }
}

double THysgen::PredictAllCEdgesS(const int &UID, const int &CID,
                                  const bool IsLikelihood, const bool Verbose){
  double SCoef = 1.0;
  double LikCoef = 1.0;
  double S_uc = GetNCom(UID,CID);
  double SPowN = 1.0;
  if (!IsLikelihood){
    if (S_uc <= 0.0) { return SumPrPsblEdgesPow_nVV[CID][1]; }
    else { SCoef = 1/S_uc; }
  }
  if (S_uc<=0.0 && IsLikelihood) { return 0.0; }
  double Val = 0.0;
  double Val_n;
  if (Verbose) {
    printf("### PredictAllCEdgesS for node %d and comm %d :\n", UID, CID);
  }
  for (int n=1; n<SumPrPsblEdgesPow_nVV[CID].Len() &&
                SumPrPsblEdgesPow_nVV[CID][n]>TayThresh; n++){
    if (IsLikelihood) { LikCoef = -n; }
    SPowN *= S_uc;
    Val_n = SCoef * (SPowN/(1+SPowN)) * (SumPrPsblEdgesPow_nVV[CID][n] - SPowN) / LikCoef;
    Val += Val_n;
    if(Verbose){
      printf("\t --- (n=%d): %f\n", n , Val_n);
    }
  }
  return Val;
}

double THysgen::PredictAllCEdgesS(const int &UID, const int &CID, const double& SNNew,
                                  const bool IsLikelihood, const bool Verbose){
  IAssertR(IsLikelihood,"This Overload of function is only designed for comuting the likelihood, not gradient!\n");
  double SCoef = 1.0;
  double LikCoef;
  double SPowN = 1.0;
  if (SNNew <= 0.0) { return 0.0; } // Just for easier computation and clearer presentation of the logic in code.
  double Val = 0.0;
  double Val_n;
  if (Verbose) {
    printf("### PredictAllCEdgesS for node %d and comm %d :\n", UID, CID);
  }
  TFltV PsiV(SumPrPsblEdgesPow_nVV[CID].Len());
  UpdatePrAllEdgesS(PsiV, UID, CID, SNNew, false);
  for (int n=1; n<PsiV.Len() && PsiV[n]>TayThresh; n++){ // n=0 is voided in the formulation
    LikCoef = -n;
    SPowN *= SNNew;
    Val_n = SCoef * (SPowN/(1+SPowN)) * (PsiV[n] - SPowN) / LikCoef;
    Val += Val_n;
    if(Verbose){
      printf("\t --- (n=%d): %f\n", n , Val_n);
    }
  }
  return Val;
}

double THysgen::PredictAllCEdgesS_direct(const int &UID, const int &CID,
                                         const bool IsLikelihood, const bool Verbose) {
  double Psi_c = 0.0;
  double S_uc = GetNCom(UID,CID);
  if (S_uc <= 0.0 && IsLikelihood) { return 0; }
  TIntV CNodesV(S.Len(),0);
  for (int nod=0; nod < S.Len(); nod++) {
    if (GetNCom(nod, CID) > 0 || nod==UID) {CNodesV.Add(nod);}
  }
  TInt NPsbEdges = pow(2.0, CNodesV.Len());
  for (int e=0; e<NPsbEdges; e++) {
    TIntV ENodesV;
    THysgenUtil::GetBinLocs(e, ENodesV, CNodesV);
    if (!ENodesV.IsIn(UID) || ENodesV.Len() < 2) { continue; }
    double ECProb = 1;
    for (int i = 0; i < ENodesV.Len(); i++) {
      if (Verbose) {
        printf("%d, ", ENodesV[i]()); }
      if (ENodesV[i] != UID) { ECProb *= GetNCom(ENodesV[i], CID); }
    }
    if (Verbose) {
      printf(" --\t ECProb=%f, ", ECProb);
    }
    if (ECProb == 1.0 && S_uc == 1.0) { continue; }
    if (!IsLikelihood) {
      Psi_c += (ECProb / (1 - ECProb*S_uc));
    } else {
      Psi_c += log(1 - ECProb*S_uc);
    }
    if (Verbose) {
      printf(" *Islikelihood=%d*, Psi_c=%f\n", IsLikelihood, Psi_c); }
  }
  return Psi_c;
}

void THysgen::SetCmtyVV(const TVec<TIntV>& CmtyVV) {
  S.Gen(G->GetNodes());
  NumCIdNV.Gen(CmtyVV.Len());
  NumComs = CmtyVV.Len();
  for (int c = 0; c < CmtyVV.Len(); c++) {
    for (int u = 0; u < CmtyVV[c].Len(); u++) {
      int UID = CmtyVV[c][u];
      if (! NIdToIdx.IsKey(UID)) { continue; }
      AddNCom(NIdToIdx.GetKeyId(UID), c, 1.0);
    }
  }
}

double THysgen::Likelihood() {
  TExeTm ExeTm;
  double L = 0.0;
  for (int u = 0; u < S.Len(); u++) {
    double LU = LikelihoodForRow(u);
    L += LU;
  }
  return L ;
}

double THysgen::Likelihood(const int UID, const TIntFltH& SU) {
  TExeTm ExeTm;
  double L = 0.0;
  for (int u = 0; u < S.Len(); u++) {
    double LU;
    if (u == UID) { LU = LikelihoodForRow(u, SU);
    } else { LU = LikelihoodForRow(u); }
    L += LU;
  }
  return L;
}

double THysgen::LikelihoodForRow(const int UID) {
  return LikelihoodForRow(UID, S[UID]);
}
double THysgen::LikelihoodForRow(const int UID, const TIntFltH& SU, const bool CmprDirct_vs_Taylor) {
  bool IsSUpdated = false;
  if (S[UID].Len() != SU.Len()) {
    IsSUpdated = true;
  } else {
    for (int CID = 0; CID < NumComs; CID++){
      if (!S[UID].IsKey(CID) && !SU.IsKey(CID)) {continue;}
      if ( (S[UID].IsKey(CID) && !SU.IsKey(CID)) ||
          (!S[UID].IsKey(CID) && SU.IsKey(CID)) ||
          S[UID].GetDat(CID) != SU.GetDat(CID) ) {
        IsSUpdated = true;
        break;
      }
    }
  }
  double L = 0.0;
  const bool IsLikelihood = true;
  THGraph::TNodeI NI = G->GetNI(UID);
  TIntV EIdV; 
  NI.GetEIDs(EIdV);
  TFlt SumLgPrNotUEdges;
  for (int e = 0; e < NI.GetDeg(); e++) {
    int EId = EIdV[e];
    TIntFltH PrENewCH;
    if (HONNIdsV[UID].IsKey(EId)) { continue; }
    SumLgPrNotUEdges = 0.0; // For the NOT side, Noise source has no effect, no need to be included.
    if (IsSUpdated){
      L += log(GetPrE(EId, UID, PrENewCH, SU));
      for (TIntFltH::TIter PrECI = PrENewCH.BegI(); PrECI < PrENewCH.EndI(); PrECI++){
        SumLgPrNotUEdges += log(1.0 - PrECI.GetDat());
      }
    } else {
      L += log(GetPrE(EId));
      for (TIntFltH::TIter UCI = SU.BegI(); UCI < SU.EndI(); UCI++){
        SumLgPrNotUEdges += log(1.0 - GetECom(EId, UCI.GetKey()));
      }
    }
  }
  TFlt SumAllPsbl = 0.0;
  for (TIntFltH::TIter UCI = SU.BegI(); UCI < SU.EndI(); UCI++) {
    if (IsSUpdated) {
      SumAllPsbl += PredictAllCEdgesS(UID, UCI.GetKey(), UCI.GetDat(), IsLikelihood);
    } else {
      SumAllPsbl += PredictAllCEdgesS(UID, UCI.GetKey(), IsLikelihood);
    }
  }
  if (CmprDirct_vs_Taylor && !IsSUpdated) {
    TFlt SumAllPsbl_direct = 0.0;
    for (TIntFltH::TIter UCI = SU.BegI(); UCI < SU.EndI(); UCI++) {
      SumAllPsbl_direct += PredictAllCEdgesS_direct(UID, UCI.GetKey(), IsLikelihood,
                                                    false);
    }
  }
  L += NegWgt * (SumAllPsbl - SumLgPrNotUEdges);
  
  // Add regularization
  if (RegCoef > 0.0) { //L1
    L -= RegCoef * Sum(SU);
  }
  if (RegCoef < 0.0) { //L2
    L += RegCoef * Norm2(SU);
  }
  return L;
}

void THysgen::GradientForRow(const int UId, TIntFltH& GradNod, const TIntSet& CIDSet, const bool CmprDirct_vs_Taylor) {
  GradNod.Gen(CIDSet.Len());
  THGraph::TNodeI NI = G->GetNI(UId);
  int NDeg = NI.GetDeg();
  TIntV EIdV; 
  NI.GetEIDs(EIdV);
  TIntFltH PredEH(NDeg);
  TFltV PredECMinusUV(CIDSet.Len());
  TFltV GradV(CIDSet.Len());
  TIntV CIDV(CIDSet.Len());
  TIntV NIdV;
  TInt NId;
  TInt EId;
  for (int e = 0; e < NDeg; e++) {
    EId = EIdV[e];
    if (HONEIdsV[UId].IsKey(EId)) { continue; }
    PredEH.AddDat(EId, GetPrE(EId)); 
  }
  for (int CId = 0; CId < NumComs; CId++) { 
    double Val = 0.0;
    for (int e = 0; e < NDeg; e++) {
      EId = EIdV[e];
      if (HONEIdsV[UId].IsKey(EId)) { continue; }
      if (GetNCom(UId, CId) > 0.0) {
        PredECMinusUV[CId] =
          GetECom(EId, CId) / GetNCom(UId, CId); 
      } else {
        PredECMinusUV[CId] = 1;
        G->GetEI(EId).GetNodesV(NIdV);
        for (int m = 0; m < NIdV.Len(); m++) {
          NId = NIdV[m];
          if (NId != UId) { PredECMinusUV[CId] *= GetNCom(NId, CId); }
        }
      }
      
      double PrENoise = GetENoiseProb(G->GetEI(EId).Len());
      if (GetECom(EId, CId) >= 1.0 || (1 - GetECom(EId, CId) <= 0.0)) {
        double PredENotCId = 1 - PrENoise;
        for (int c2 = 0; c2 < NumComs; c2++) {
          if (c2 == CId) { continue; }
          PredENotCId *= 1 - GetECom(EId, c2);
        }
        Val += PredECMinusUV[CId] * PredENotCId / PredEH.GetDat(EId); 
      }
      else {
        double PrNotENotC = (1 - PredEH.GetDat(EId)) / (1 - GetECom(EId, CId)); // Precision safe!
        Val +=
          PredECMinusUV[CId] * PrNotENotC / PredEH.GetDat(EId);
      }
      Val += NegWgt * (PredECMinusUV[CId] /
        (1 - GetECom(EId, CId)));
    }
    Val -= NegWgt * PredictAllCEdgesS(UId, CId, false);
    CIDV[CId] = CId;
    GradV[CId] = Val;
  }
  for (int c = 0; c < GradV.Len(); c++) {
    GradNod.AddDat(CIDV[c], GradV[CIDV[c]]);
  }

}

void THysgen::GetCmtyVV(TVec<TIntFltH>& CmtyVH, TVec<TIntV>& CmtyVV, TVec<TFltV>& WckVV,
                        const double Thres, const int MinSz) {
  CmtyVH.Gen(NumComs, 0);
  CmtyVV.Gen(NumComs, 0);
  for (int CId = 0; CId < NumComs; CId++) {
    TIntFltH NIDSucH(S.Len());
    TIntV CmtyV;
    for (int u = 0; u < S.Len(); u++) {
      int NID = NIdToIdx[u];
      if (GetNCom(u, CId) > Thres) {
        NIDSucH.AddDat(NID, GetNCom(u, CId));
      }
    }
    NIDSucH.SortByDat(false);
    NIDSucH.GetKeyV(CmtyV);
    if (CmtyV.Len() < MinSz) { continue; }
    CmtyVH.Add(NIDSucH);
    CmtyVV.Add(CmtyV);
  }
  if ( NumComs != CmtyVV.Len()) {
    printf("Sort-less Community vector generated. %d communities are ommitted\n", NumComs.Val - CmtyVV.Len());
  }
}

void THysgen::GetCmtyVVUnSorted(TVec<TIntV>& CmtyVV) {
  GetCmtyVVUnSorted(CmtyVV, sqrt(2.0 * (double) G->GetEdges() / G->GetNodes() / G->GetNodes()), 3);
}

void THysgen::GetCmtyVVUnSorted(TVec<TIntV>& CmtyVV, const double Thres, const int MinSz) {
  CmtyVV.Gen(NumComs, 0);
  for (int c = 0; c < NumComs; c++) {
    TIntV CmtyV;
    for (int u = 0; u < G->GetNodes(); u++) {
      if (GetNCom(u, c) > Thres) { CmtyV.Add(NIdToIdx[u]); }
    }
    if (CmtyV.Len() >= MinSz) { CmtyVV.Add(CmtyV); }
  }
  if ( NumComs != CmtyVV.Len()) {
    printf("*Sorted* Community vector generated. %d communities are ommitted\n", NumComs.Val - CmtyVV.Len());
  }
}


/// Armijo–Goldstein backtracking line search
double THysgen::GetStepSizeByLineSearch(const int UID, const TIntFltH &DeltaH,
                                        TIntFltH &SearchVecH,
                                        const double &stepSize,
                                        const double &CtrlParam,
                                        const double &ReductionRatio,
                                        const int MaxIter) {
  double StepSize = stepSize;
  double NewVal;
  bool IsEligible = false;
  for (TIntFltH::TIter CI = DeltaH.BegI(); CI < DeltaH.EndI(); CI++) {
    int CID = CI.GetKey();
    if (!((GetNCom(UID, CID) <= MinVal && CI.GetDat() < 0.0) ||
          (GetNCom(UID, CID) >= MaxVal && CI.GetDat() > 0.0))) {
      IsEligible = true;
      break;
    }
  }
  if (!IsEligible) { return 0.0; }
  bool ShouldReduce = true;
  while (ShouldReduce) {
    for (TIntFltH::TIter CI = DeltaH.BegI(); CI < DeltaH.EndI(); CI++) {
      int CID = CI.GetKey();
      NewVal = GetNCom(UID, CID) + StepSize * CI.GetDat();
      if (!(NewVal < MinVal || NewVal > MaxVal)) {
        ShouldReduce = false;
        break;
      }
    }
    if (ShouldReduce) {
      StepSize *= ReductionRatio;
    }
  }
  
  double InitLikelihood = LikelihoodForRow(UID);
  double FinalLikelihood = 0.0;
  TIntFltH NewVarH;
  double ChangeVal;
  GetNCom(NewVarH, UID);
  for(int iter = 0; iter < MaxIter; iter++) {
    for (TIntFltH::TIter CI = DeltaH.BegI(); CI < DeltaH.EndI(); CI++) {
      NewVal = GetNCom(UID, CI.GetKey()) + StepSize * CI.GetDat();
      ChangeVal = StepSize * CI.GetDat();
      if (NewVal < MinVal) { NewVal = MinVal; ChangeVal = MinVal - GetNCom(UID, CI.GetKey()); }
      if (NewVal > MaxVal) { NewVal = MaxVal - TayThresh; ChangeVal = MaxVal - TayThresh - GetNCom(UID, CI.GetKey()); }
      NewVarH.AddDat(CI.GetKey(), NewVal); SearchVecH.AddDat(CI.GetKey(), ChangeVal);
    }
    FinalLikelihood = LikelihoodForRow(UID, NewVarH, false);
    if (FinalLikelihood < InitLikelihood + CtrlParam * StepSize * DotProduct(SearchVecH, SearchVecH)
        || isinf(FinalLikelihood)) {
      // ***** True Armijo Rule:
      StepSize *= ReductionRatio;
    } else {
      break;
    }
    if (iter == MaxIter - 1) {
      StepSize = 0.0;
      break;
    }
  }
  return StepSize;
}

int THysgen::MLEGradAscent(const double& Thres, const int& MaxIter, const TStr PlotNm,
                           const double StepSize, const double StepCtrlParam, const double StepReductionRatio) {
  int NegLikPermits = 0;
  int MaxIterLineSearch = 100;
  time_t InitTime = time(NULL);
  time_t InitIterTime;
  TExeTm ExeTm, CheckTm; //Execution time
  int iter = 0; 
  TIntFltPrV IterLV; 
  THGraph::TNodeI UI; 
  double PrevL = Likelihood(), CurL = 0.0, DiffL; 
  printf("\n0 iterations (iter/#nodes = 0) {[INITIAL] Likelihood: %.4e}\n",PrevL);
  TIntV NIdxV(S.Len(), 0);  
  for (int i = 0; i < S.Len(); i++) { NIdxV.Add(i); }
  TIntFltH GradUH;
  TIntSet CIDSet(NumComs);
  for (int c = 0; c < NumComs; c++) { CIDSet.AddKey(c); }
  while(iter < MaxIter) {
    InitIterTime = time(NULL);
    DebugPrcCmpCnt = 0;
    DebugPrcApproxCmpCnt = 0;
    DebugPrcCmpCntH.Gen(5);
    NIdxV.Shuffle(Rnd);
    for (int ui = 0; ui < S.Len(); ui++, iter++) {
      int u = NIdxV[ui]; 
      GradientForRow(u, GradUH, CIDSet, false);
      if (Norm2(GradUH) < 1e-4) { continue; }
      TIntFltH GradAdjustedH;
      TIntFltH SearchVecH;
      NormalizeIfLarge(GradUH, GradAdjustedH);
      double LearnRate = GetStepSizeByLineSearch(u, GradAdjustedH,
                                                 SearchVecH, StepSize,
                                                 StepCtrlParam,
                                                 StepReductionRatio, MaxIterLineSearch);
      if (LearnRate < ThreshLearnRate) { continue; }
      TIntFltH SNew;
      GetUpdatedNodP(SNew, u, GradAdjustedH, LearnRate);
      if (!AcceptStepSA(u, SNew, iter, MaxIter, 1.0)) { continue; }
      for (int ci = 0; ci < GradAdjustedH.Len(); ci++) {
        int CID = GradAdjustedH.GetKey(ci);
        double NewSuc = SNew.GetDat(CID);
        if (NewSuc <= 0.0) {
          DelNCom(u, CID);
        } else {
          AddNCom(u, CID, NewSuc);
        }
      }
      /// Add regularization
      if (RegCoef > 0.0) { //L1
        for (int c = 0; c < GetNumComs(); c++) {
          if (GetNCom(u, c) > RegCoef) {
            AddNCom(u, c, GetNCom(u, c) - RegCoef);
          } else { DelNCom(u, c); }
        }
      }
      if (RegCoef < 0.0) { //L2
        for (int c = 0; c < GetNumComs(); c++) {
          if (GetNCom(u, c) > - 2 * RegCoef * GetNCom(u, c)) {
            AddNCom(u, c, GetNCom(u, c) + 2 * RegCoef * GetNCom(u, c));
          } else { DelNCom(u, c); }
        }
      }
      if (! PlotNm.Empty() && (iter + 1) % G->GetNodes() == 0) {
        IterLV.Add(TIntFltPr(iter, Likelihood()));
      }
    }
    CurL = Likelihood();
    DiffL = CurL - PrevL;
    printf("High Precision computation is used %d times as exact and %d times as "
           "approximate during this iteration.\n\tMoreover, Below is the per line of "
           "call statistics of precise computation:\n",DebugPrcCmpCnt(), DebugPrcApproxCmpCnt());
    
    for (TIntIntH::TIter LI=DebugPrcCmpCntH.BegI(); LI<DebugPrcCmpCntH.EndI(); LI++) {
      printf("\t\tLine# %d: \t%d\n", LI.GetKey()(), LI.GetDat()());
    }
    if (fabs(CurL) < 1e9) {
      if (fabs(DiffL) < 1e9) {
        printf("\r%d iterations (iter/#nodes = %d) {Likelihood: %.4f, Diff: %.4f} [%lu/%lu sec]\n",
               iter, iter/G->GetNodes(), CurL, DiffL, time(NULL) - InitIterTime, time(NULL) - InitTime);
      } else {
        printf("\r%d iterations (iter/#nodes = %d) {Likelihood: %.4f, Diff: %.4e} [%lu/%lu sec]\n",
               iter, iter/G->GetNodes(), CurL, DiffL, time(NULL) - InitIterTime, time(NULL) - InitTime);
      }
    } else {
      if (fabs(DiffL) < 1e9) {
        printf("\r%d iterations (iter/#nodes = %d) {Likelihood: %.4e, Diff: %.4f} [%lu/%lu sec]\n",
               iter, iter/G->GetNodes(), CurL, DiffL, time(NULL) - InitIterTime, time(NULL) - InitTime);
      } else {
        printf("\r%d iterations (iter/#nodes = %d) {Likelihood: %.4e, Diff: %.4e} [%lu/%lu sec]\n",
               iter, iter/G->GetNodes(), CurL, DiffL, time(NULL) - InitIterTime, time(NULL) - InitTime);
      }
    }
    printf("\n");
    fflush(stdout);
    if (DiffL < 0) { // If the likelihood goes negative several times, then we are actually converged, so stop!
      if (NegLikPermits <= 0) { break; }
      NegLikPermits --;
    }
    if (fabs(DiffL) <= Thres) { break; }
    else { PrevL = CurL; }
  }
  printf("\n");
  printf("MLE for Lambda completed with %d iterations(%s)\n", iter, ExeTm.GetTmStr());
  if (! PlotNm.Empty()) {
    TGnuPlot::PlotValV(IterLV, PlotNm + ".likelihood_Q");
  }
  return iter;
}

double THysgen::GetPrE(const int &EId, const int &UId, TIntFltH &PrEOutCH,
              const TIntFltH &SU) {
  double PrENoise = GetENoiseProb(G->GetEI(EId).Len());
  bool DirectCompValid = true;
  double SUOld;
  double SUNew;
  double PrECOld;
  double PrENew = 1 - PrENoise; // Noise source is the first community to be counted.
  int SCId;
  for (TIntFltH::TIter SCI = SU.BegI(); SCI < SU.EndI(); SCI++) {
    SCId = SCI.GetKey();
    SUNew = SCI.GetDat();
    SUOld = GetNCom(UId, SCId);
    if (SUOld <= 0.0) {
      TIntV ENodesV;
      G->GetEI(EId).GetNbrNodes(ENodesV);
      double PrEC = SUNew;
      for (int n = 0; n < ENodesV.Len(); n++) {
        if (ENodesV[n] == UId) { continue; }
        PrEC *= GetNCom(ENodesV[n], SCId);
      }
      PrEOutCH.AddDat(SCId, PrEC);
    } else {
      PrECOld = GetECom(EId, SCId);
      PrEOutCH.AddDat(SCId, PrECOld * SU.GetDat(SCId) / SUOld);
    }
    PrENew *= 1 - PrEOutCH.GetDat(SCId);
    if (PrENew >= 1 && PrEOutCH.GetDat(SCId) > 0) { DirectCompValid = false; }
  }
  if (DirectCompValid) { PrENew = 1 - PrENew; }
  else {
    PrENew = GetPrEPrecisionApprox(PrEOutCH, AuxDPEdgVV, PrENoise);
  }
  return PrENew;
}

double THysgen::GetPrEPrecisionApprox(const TIntFltH& ECH, TVec<TFltV>& DPMatVV,
                             const double PrENoise) {
  DebugPrcApproxCmpCnt ++;
  int ApproxLevel = 2;
  int NumEC = ECH.Len();
  TFltV ECV(NumEC);
  ECH.GetDatV(ECV);
  if (PrENoise > 0) { ECV.Add(PrENoise); NumEC++; }
  for (int i=0; i<NumEC+1; i++) {
    DPMatVV[0][i] = 1.0;
  }
  for (int i=0; i<=ApproxLevel; i++) {
    DPMatVV[i][NumEC] = 0.0;
  }
  for (int i=1; i<=ApproxLevel; i++) {
    for (int j=NumEC-1; j>=i-1; j--) {
      DPMatVV[i][j] = (ECV[j-i+1] * DPMatVV[i-1][j]) + DPMatVV[i][j+1];
    }
  }
  double prob = 0;
  double SignCoef = 1;
  for (int i=1; i<=ApproxLevel; i++) {
    if (i==ApproxLevel)  { prob += SignCoef * DPMatVV[i][i-1] / 2;
    } else               { prob += SignCoef * DPMatVV[i][i-1]; }
    SignCoef *= -1;
  }
  return prob;
}

double THysgen::GetPrEPrecision(const TIntFltH& ECH, TVec<TFltV>& DPMatVV, const double PrENoise, TInt LineNo) {
  DebugPrcCmpCnt ++;
  if (!DebugPrcCmpCntH.IsKey(LineNo)) { DebugPrcCmpCntH.AddDat(LineNo, 0); }
  DebugPrcCmpCntH.AddDat(LineNo, DebugPrcCmpCntH.GetDat(LineNo)+1);
  int NumEC = ECH.Len();
  TFltV ECV(NumEC);
  ECH.GetDatV(ECV);
  if (PrENoise > 0) { ECV.Add(PrENoise); NumEC++; }
  for (int i=0; i<NumEC+1; i++) {
    DPMatVV[0][i] = 1.0;
  }
  for (int i=0; i<NumEC+1; i++) {
    DPMatVV[i][NumEC] = 0.0;
  }
  for (int i=1; i<=NumEC; i++) {
    for (int j=NumEC-1; j>=i-1; j--) {
      DPMatVV[i][j] = ECV[j-i+1] * DPMatVV[i-1][j] + DPMatVV[i][j+1];
    }
  }
  double prob = 0;
  double SignCoef = 1;
  for (int i=1; i<=NumEC; i++) {
    prob += SignCoef * DPMatVV[i][i-1];
    SignCoef *= -1;
  }
  return prob;
}

void THysgen::GetUpdatedNodP(TIntFltH &SNew, const int &UID, const TIntFltH GradUH,
                        double &StepSize) {
  for (int CID = 0; CID < NumComs; CID++) {
    if (!(GetNCom(UID, CID) > 0 || GradUH.IsKey(CID))) { continue; }
    double Change = StepSize * GradUH.GetDat(CID);
    double NewSuc = GetNCom(UID, CID) + Change;
    if (NewSuc <= MinVal) {
      NewSuc = MinVal;
    } else if (NewSuc >= MaxVal){
      NewSuc = MaxVal - TayThresh;
    }
    SNew.AddDat(CID, NewSuc);
  }
}

bool THysgen::AcceptStepSA(const int &UID, const TIntFltH &SNew, const int &Iter,
                      const int &MaxIter, const double &SAParamK) {
  double T0 = 100.0;
  double T = T0/((double)Iter+1.0);
  
  double OldLikelihood = LikelihoodForRow(UID);
  double NewLikelihood = LikelihoodForRow(UID,SNew);
  double DeltaE = - (NewLikelihood-OldLikelihood);
  double PrAccept = exp(-DeltaE/(SAParamK*T));
  if (Rnd.GetUniDev() < PrAccept) { return true; }
  return false;
} 
