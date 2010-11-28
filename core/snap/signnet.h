#ifndef snap_signed_network_h
#define snap_signed_network_h

#include "Snap.h"
#include "mxdag.h"

class TSignNet;
typedef TPt<TSignNet> PSignNet;

/////////////////////////////////////////////////
// Signed network (network with signed (+1/0/-1) edges)
class TSignNet : public TNodeEDatNet<TInt, TInt> {
public:
  TSignNet() { }
  TSignNet(TSIn& SIn) : TNet(SIn) { }
  static PSignNet New() { return new TSignNet; }
  static PSignNet Load(TSIn& SIn) { return new TSignNet(SIn); }
  PSignNet GetEdgeSubNet(const int& EDat1, const int& EDat2=TInt::Mn, const int& EDat3=TInt::Mn) const;

  void PermuteEdgeSigns();
  void RewireNetwork();
  
  void SimplifyNet();
  void FlipMinusEdges(const int&  OldSign=-1, const int& NewSign=1);
  PSignNet GetSubNet(const int& MinEdgeWgt) const { 
    return TSnap::GetEDatSubGraph<PSignNet, TInt>(PSignNet((TSignNet*) this), MinEdgeWgt, 1); }
  PSignNet GetSignSubNet(const int& Sign) const { 
    return TSnap::GetEDatSubGraph<PSignNet, TInt>(PSignNet((TSignNet*) this), Sign, 0); }
  
  void CountStructBalance() const;
  void CountBalUnBal(const int& NId1, const int& NId2, int& BalTriads, int& UnBalTriads) const;
  void SetNodePart(const int& PartId);
  void SetNodePart(TVec<TIntV>& PartNIdV);
  void GetPartStat(const TVec<TIntV>& PartNIdV, const TStr& Desc=TStr()) const;
  void MakeStatusConsistent();
  
  
  // signed triad 
  PSignNet GetTriad(const int& NId1, const int& NId2, const int& NId3) const;
  bool IsClosedTriad() const;
  bool IsBalanced() const;
  double GetTriadProb(const double& PlusProb) const;
  TIntPr GetNewEdge(const PSignNet& OldTriad) const;
  int GetEdgeSig(const int& NId1, const int& NId2) const;
  TChA GetEdgeStr(const int& NId1, const int& NId2) const;
  TIntTr GetTriadSig(const bool& Canonical=true) const;
  TChA GetTriadStr(const bool& Canonical=true) const;
  //static TIntPr GetSigNewEdge(const TIntTr& Sig1, const TIntTr& Sig2) const;
  //static int GetEdgeSigSign(const int& EdgeSig);
  static TChA GetEdgeStr(const int& EdgeSig);
  static TChA GetTriadStr(const TIntTr& TriadSig);
      
  void EdgeSignStat() const;
  void PlotSignCmnNbhs(const TStr& OutFNm) const;
  void CountSignedTriads(const TStr& OutFNm) const;
  void PlotGraphProp(const TStr& OutFNm) const;
  void PlotInOutPlusFrac(const TStr& OutFNm) const;
  void SaveTxt(const TStr& OutFNm) const;
  void SavePajek(const TStr& OutFNm);
  void DrawGraphViz(const TStr& OutFNm, const TStr& Desc, const bool& NodeLabels=false, const bool& EdgeLabels=false, const bool& SaveDir=true);
  void PrintInfo(const TStr& Desc=TStr()) const;
  void Dump() const;
  static int IsSameTriad(const PSignNet& Net1, const PSignNet& Net2);
  static void GetPartOverlap(const TVec<TIntV>& PartNIdV1, const TVec<TIntV>& PartNIdV2);
  static PSignNet LoadEpinions(const TStr& FNm);
  static PSignNet LoadSlashdot(const TStr& FNm);
  static PSignNet GetSmallNet();
  friend class TPt<TSignNet>;
};

/////////////////////////////////////////////////
// Hopfield energy minimization (find sets that satisfy structural balance)
class THopfield {
public:
  PSignNet Net;
  TIntH NIdPartH;
  THash<TInt, TTuple<TInt, 5> > NIdPartCnt;
  TIntH EnergDistr;
  TInt MxEnergy;
  TIntH BestPart;
public:
  THopfield(const PSignNet& _Net) : Net(_Net) { }

  int MinEnergy(const int& NPart);
  int FindStableSet(const int& NPart, const int& NRuns);
  void GetStableSet(const int& Tresh, TVec<TIntV>& PartNIdV);
  void PlotPartStab(const TStr& FNm, TStr Desc=TStr()) const;
  void DumpPartStat() const;
};

/////////////////////////////////////////////////
// SignNet-Micro-Evolution
class TSignMicroEvol {
public:
  // transition from open to close triad
  struct TTransStat {
    TInt Cnt;    // number of + edges
    TFlt SumWgt; // sum_+ p
    TFlt VarSq;  // sum_+ p*(1-p)
    TTransStat() { }
    TTransStat(TSIn& SIn) : Cnt(SIn), SumWgt(SIn), VarSq(SIn) { }
    void Save(TSOut& SOut) const { Cnt.Save(SOut); SumWgt.Save(SOut); VarSq.Save(SOut); }
    bool operator < (const TTransStat& S) const { return Cnt<S.Cnt; }
  };
  // triad closing statistics (for +/- closures)
  struct TCloseStat {
    TInt PlsCnt, MnsCnt;        // number of + edges
    TFlt OutSumWgt, OutVarSq;   // sum p+, sum p*(1-p) (out plus prob)
    TFlt InSumWgt, InVarSq;     // sum p+, sum p*(1-p) (in plus prob)
    TIntH OutPlusPH, InPlusPH;  // histogram of out-/in- plus probability
    TCloseStat() { }
    TCloseStat(TSIn& SIn) : PlsCnt(SIn), MnsCnt(SIn), OutSumWgt(SIn), OutVarSq(SIn), InSumWgt(SIn), InVarSq(SIn), OutPlusPH(SIn), InPlusPH(SIn) { }
    void Save(TSOut& SOut) const { PlsCnt.Save(SOut); MnsCnt.Save(SOut); OutSumWgt.Save(SOut); OutVarSq.Save(SOut); InSumWgt.Save(SOut); InVarSq.Save(SOut); OutPlusPH.Save(SOut); InPlusPH.Save(SOut); }
    TStr GetMajorSign() const { return PlsCnt>=MnsCnt?"+":"-"; }
    double GetMajorFrac() const { return TMath::Mx(PlsCnt,MnsCnt)/(PlsCnt+MnsCnt); }
    double GetPlsFrac() const { return PlsCnt/double(PlsCnt+MnsCnt); }
    //old: double GetPlsSurp() const { return (PlsCnt-double(PlsWgt+(MnsCnt-MnsWgt)))/sqrt(PlsVarSq+MnsVarSq); }
    double GetOutPlsSurp() const { return (PlsCnt - OutSumWgt)/sqrt(OutVarSq); }
    double GetInPlsSurp() const { return  (PlsCnt - InSumWgt)/sqrt(InVarSq); }
    void SaveHist(const TStr& OutFNm) const { 
      TGnuPlot::PlotValCntH(OutPlusPH, OutFNm+"_Out", "", "Probability of plus out", "Count");
      TGnuPlot::PlotValCntH(InPlusPH, OutFNm+"_In", "", "Probability of plus in", "Count"); }
  };
  typedef TNodeEDatNet<TInt, TTransStat> TTransitionNet;
  typedef TPt<TTransitionNet> PTransitionNet;
  THash<TInt, TVec<TIntV> > PermH;
  void BuildPerms();
public:
  PSignNet Network;
  // transition from a directed open to a closed triad
  TVec<PSignNet> TriadIdV;  // non-isomorphic open and closed triads
  TVec<PSignNet> OpnTriadV; // open triads (also includes isomorphic ones). (0,1) is the closing edge
  PTransitionNet TransNet;  // transition statistics between open -> closed triads (TriadIdV)
  THash<TInt, TCloseStat> CloseTriadH;  // OpnTriadId -> 0->1 +/- edges
  THash<TIntPr, TCloseStat> Triad3to4H; // 3-edge to 4-edge triad (edge (0,1) is the new edge)
  // reciprocal links in triads
  THash<TTuple<TInt, 16>, TIntPr> TriadVecV;
  THash<TIntPr, TFltPr> RepLinkCntH;    // reciprocal links
public:
  TSignMicroEvol(PSignNet NetPt);
  TSignMicroEvol(TSIn& SIn);
  void Save(TSOut& SOut) const;
  PSignNet GetSubGraph(const int& N1, const int& N2);
  PSignNet GetSubGraph(const int& N1, const int& N2, const int& N3);
  
  void CountTriadClose(const int& SrcNId, const int& DstNId, const int& Sign, const double& SrcOutPlusProb, 
    const double& DstInPlusProb, const bool& OnlySimpleTriads=false);
  void CountTriad3to4Edges(const int& SrcNId, const int& DstNId, const int& Sign, const double& SrcOutPlusProb, 
    const double& DstInPlusProb);
  void CountTriadCloseVec(const int& SrcNId, const int& DstNId, const int& Sign);
  
  int IsSameOpnTriad(const PSignNet& Net1, const PSignNet& Net2);
  int GetOpnTriadId(const PSignNet& Net1);
  bool IsSameTriad(const PSignNet& Net1, const PSignNet& Net2, const TIntV& Perm) const;
  bool IsSameTriad(const PSignNet& Net1, const PSignNet& Net2, int& PermId) const;
  int GetTriadId(const PSignNet& Net1);
  void GetEdgeDiff(const PSignNet& Net1, const PSignNet& Net2, int& SrcNId, int& DstNId, int& Sign);

  static double GetStatusDelta(const PSignNet& Net, const int& NId1, const int& NId2);
  static double GetStatus(const PSignNet& Net, const int& NId);
  bool IsStatusFrac(const int& OpnTriadId, const bool& DstNode=true) const;
  bool IsStatusSurp(const int& OpnTriadId, const bool& DstNode=true) const;
  bool IsBalanceFrac(const int& OpnTriadId) const;
  bool IsBalanceSurp(const int& OpnTriadId) const;
  
  void PrintInfo() const;
  void SaveCloseTriadTxt(const TStr& FNmPref, const bool& DrawNets=true, const bool& PlotHists=true);
  void DrawCloseTriad(const TStr& FNmPref, const int& OpnTriadId, const TStr& Label="");
  void SaveTriad3to4Edges(const TStr& FNmPref);
  void SaveTriadCloseVec(const TStr& FNmPref);
  static char GetEdgeChar(const PSignNet& Net, const int& n1, const int& n2);
  static void EpinionsTriadEvolution(const bool& OnlySimpleTriads=false);
  //static void WikiVoteTriadEvolution(const bool& OnlySimpleTriads=false);
  static void EpinionsStat();
};

/////////////////////////////////////////////////
// SignNet Edge Sign Prediction
class TEdgeSignPred {
public:
  PSignNet Network;
  THash<TStr, TIntQu> ResH; // method id --> (+Ok, +Bad, -Ok, -Bad)
  int MinCmnNbrs;
public:
  TEdgeSignPred() : Network(TSignNet::New()), MinCmnNbrs(0) { }
  void AddEdge(const int& SrcNId, const int& DstNId, const int& TrueSign) {
    if (DstNId == SrcNId || Network->IsEdge(DstNId, SrcNId)) { return; }
    Network->AddNode(SrcNId);  Network->AddNode(DstNId);
    Network->AddEdge(SrcNId, DstNId, TrueSign);
    if (MinCmnNbrs>0 && MinCmnNbrs>TSnap::GetCmnNbhs(Network, SrcNId, DstNId)) { return; }
    PredictRandom(SrcNId, DstNId, TrueSign);
    PredictOutSign(SrcNId, DstNId, TrueSign);
    PredictInSign(SrcNId, DstNId, TrueSign);
    PredictStatus(SrcNId, DstNId, TrueSign);
    PredictBalance(SrcNId, DstNId, TrueSign);
  }
  void PredictOnStaticNet(const bool& SameNPlusNMinus) {
    printf("\nPredict static network: MinCmnNbrs  %d\n", MinCmnNbrs);
    ResH.Clr();
    TIntTrV EdgeV;
    int NegCnt=0;
    for (TSignNet::TEdgeI EI = Network->BegEI(); EI < Network->EndEI(); EI++) {
      if (MinCmnNbrs>0 && MinCmnNbrs>TSnap::GetCmnNbhs(Network, EI.GetSrcNId(), EI.GetDstNId())) { continue; }
      EdgeV.Add(TIntTr(EI.GetSrcNId(), EI.GetDstNId(), EI()));
      if (EI() == -1) { NegCnt++; }
    }
    int PlusCnt=0, MinusCnt=0;
    EdgeV.Shuffle(TInt::Rnd);
    for (int e = 0; e < EdgeV.Len(); e++) {
      const int SrcNId = EdgeV[e].Val1;
      const int DstNId = EdgeV[e].Val2;
      const int TrueSign = EdgeV[e].Val3;
      if (SameNPlusNMinus) {
        if (TrueSign==1 && PlusCnt>=NegCnt) { continue; }
        if (TrueSign==-1 && MinusCnt>=NegCnt) { continue; }
      }
      if (TrueSign==1) { PlusCnt++; } else { MinusCnt++; }
      Network->DelEdge(SrcNId, DstNId);
      PredictRandom(SrcNId, DstNId, TrueSign);
      PredictOutSign(SrcNId, DstNId, TrueSign);
      PredictInSign(SrcNId, DstNId, TrueSign);
      PredictStatus(SrcNId, DstNId, TrueSign);
      PredictBalance(SrcNId, DstNId, TrueSign);
      Network->AddEdge(SrcNId, DstNId, TrueSign);
    }
    printf("%d %d\n", PlusCnt, MinusCnt);
    PrintRes();
  }
  void AddPred(const TStr& Method, const int& PredSign, const int& TrueSign) {
    TIntQu& Q = ResH.AddDat(Method);
    if (TrueSign == 1) { if (PredSign == 1) { Q.Val1++; } else { Q.Val2++; } }
    else { if (PredSign == 1) { Q.Val3++; } else { Q.Val4++; } }
  }
  void PredictRandom(const int& SrcNId, const int& DstNId, const int& TrueSign) {
    AddPred("Random", 1, TrueSign);
  }
  void PredictOutSign(const int& SrcNId, const int& DstNId, const int& TrueSign) {
    TSignNet::TNodeI NI = Network->GetNI(SrcNId);
    int plusCnt=0;
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      plusCnt += NI.GetOutEDat(e); }
    if (plusCnt >= 0) { AddPred("OutSign", 1, TrueSign); }
    else { AddPred("OutSign", -1, TrueSign); }
  }
  void PredictInSign(const int& SrcNId, const int& DstNId, const int& TrueSign) {
    TSignNet::TNodeI NI = Network->GetNI(DstNId);
    int plusCnt=0;
    for (int e = 0; e < NI.GetInDeg(); e++) {
      plusCnt += NI.GetInEDat(e); }
    if (plusCnt >= 0) { AddPred("InSign", 1, TrueSign); }
    else { AddPred("InSign", -1, TrueSign); }
  }
  void PredictStatus(const int& SrcNId, const int& DstNId, const int& TrueSign) {
    TIntV NbhV;
    TSnap::GetCmnNbhs(Network, SrcNId, DstNId, NbhV);
    double SrcS=0, DstS=0;
    for (int n = 0; n < NbhV.Len(); n++) {
      PSignNet TriadNet = Network->GetTriad(SrcNId, DstNId, NbhV[n]); // (src,dst) == (0,1)
      SrcS += TSignMicroEvol::GetStatus(TriadNet, 0)>=0 ? 1:-1;
      DstS += TSignMicroEvol::GetStatus(TriadNet, 1)>=0 ? 1:-1;
    }
    //if (SrcS == DstS) { return; }
    if (SrcS > 0) { AddPred("SrcStat", -1, TrueSign); }
    else { AddPred("SrcStat", 1, TrueSign); }
    if (DstS > 0) { AddPred("DstStat", 1, TrueSign); }
    else { AddPred("DstStat", -1, TrueSign); }
    if (SrcS > DstS) { AddPred("StatDif", -1, TrueSign); }
    else { AddPred("StatDif", 1, TrueSign); }
  }
  void PredictBalance(const int& SrcNId, const int& DstNId, const int& TrueSign) {
    TIntV NbhV;
    TSnap::GetCmnNbhs(Network, SrcNId, DstNId, NbhV);
    int Bal=0;
    for (int n = 0; n < NbhV.Len(); n++) {
      //PSignNet TriadNet = Network->GetTriad(SrcNId, DstNId, NbhV[n]); // (src,dst) == (0,1)
      //Bal += TriadNet->IsBalanced()? 1:0;
      const int E1 = Network->IsEdge(SrcNId, NbhV[n])? Network->GetEDat(SrcNId, NbhV[n]) : Network->GetEDat(NbhV[n], SrcNId);
      const int E2 = Network->IsEdge(DstNId, NbhV[n])? Network->GetEDat(DstNId, NbhV[n]) : Network->GetEDat(NbhV[n], DstNId);
      if (E1*E2 == 1) { Bal++; }
    }
    //if (Bal >= NbhV.Len()/2) { AddPred("Balance", TrueSign, TrueSign); }
    //else { AddPred("Balance", -TrueSign, TrueSign); }
    if (Bal >= NbhV.Len()/2) { AddPred("Balance", +1, TrueSign); }
    else { AddPred("Balance", -1, TrueSign); }
  }
  void PrintRes() const {
    printf("method\t++\t+-\t-+\t--\tacc\n");
    for (int i = 0; i < ResH.Len(); i++) {
      const TIntQu& Q = ResH[i];
      printf("%s\t%d\t%d\t%d\t%d\t%f\n", ResH.GetKey(i).CStr(),
        Q.Val1, Q.Val2, Q.Val3, Q.Val4, double(Q.Val1+Q.Val4)/double(Q.Val1+Q.Val2+Q.Val3+Q.Val4));
    }
    printf("\n");
  }
  // 
  bool SaveEdgeAttrs(FILE *F, const int& SrcNId,const int& DstNId, const bool& SaveEol=true) {  
    TIntV NbhV;
    bool DelEdge = false;
    const int TrueSign = Network->IsEdge(SrcNId, DstNId) ? Network->GetEDat(SrcNId, DstNId) : 0;
    //if (MinCmnNbrs > TSnap::GetCmnNbhs(Network, SrcNId, DstNId, NbhV)) { return false; }
    TSnap::GetCmnNbhs(Network, SrcNId, DstNId, NbhV);
    if (Network->IsEdge(SrcNId, DstNId)) { Network->DelEdge(SrcNId, DstNId); DelEdge=true; }
    TSignNet::TNodeI SrcNI = Network->GetNI(SrcNId);
    TSignNet::TNodeI DstNI = Network->GetNI(DstNId);
    int srcOutPlus=0, dstInPlus=0;
    for (int e = 0; e < SrcNI.GetOutDeg(); e++) { srcOutPlus += SrcNI.GetOutEDat(e)>0 ? 1:0; }
    for (int e = 0; e < DstNI.GetInDeg(); e++) { dstInPlus += DstNI.GetInEDat(e)>0 ? 1:0; }
    fprintf(F, "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d", TrueSign==1?1:0, SrcNI.GetOutDeg(), srcOutPlus, 
      SrcNI.GetOutDeg()-srcOutPlus, DstNI.GetInDeg(), dstInPlus, DstNI.GetInDeg()-dstInPlus, NbhV.Len());
    THash<TIntPr, TInt> H;
    for (int i = 1; i <=2; i++) {
      for (int j = 1; j <=2; j++) { 
        H.AddDat(TIntPr(i,j)); H.AddDat(TIntPr(i,-j)); H.AddDat(TIntPr(-i,j)); H.AddDat(TIntPr(-i,-j)); 
    } }
    // predict edge A-->B
    for (int n = 0; n < NbhV.Len(); n++) {
      int AC=0, CB=0; // edge desc: +,- 1:AC/CB, 2:CA/BC : FB+- = (1, -2)
      const int Mid = NbhV[n];
      if (Network->IsEdge(SrcNId, Mid)) { AC = 1 * Network->GetEDat(SrcNId, Mid); }
      else  { AC = 2 * Network->GetEDat(Mid, SrcNId); }
      if (Network->IsEdge(Mid, DstNId)) { CB = 1 * Network->GetEDat(Mid, DstNId); }
      else  { CB = 2 * Network->GetEDat(DstNId, Mid); }
      if (! (AC != 0 && CB != 0)) {
        Network->PrintInfo();
        IAssert(Network->IsNode(Mid));
        printf("\nES: %d\t%d\n", (int)Network->IsEdge(SrcNId, Mid), (int)Network->IsEdge(Mid, SrcNId));
        printf("\nED: %d\t%d\n", (int)Network->IsEdge(DstNId, Mid), (int)Network->IsEdge(Mid, DstNId));
        if (Network->IsEdge(SrcNId, Mid)) { printf("S1: %d\n", Network->GetEDat(SrcNId, Mid)); }
        else  { printf("S2: %d\n", Network->GetEDat(Mid, SrcNId)); }
        if (Network->IsEdge(Mid, DstNId)) { printf("D1: %d\n", Network->GetEDat(Mid, DstNId)); }
        else  { printf("D2: %d\n", Network->GetEDat(DstNId, Mid)); }
      }
      IAssert(Network->IsEdge(SrcNId, Mid) || Network->IsEdge(Mid, SrcNId));
      IAssert(Network->IsEdge(DstNId, Mid) || Network->IsEdge(Mid, DstNId));
      IAssert(AC != 0 && CB != 0);
      H.AddDat(TIntPr(AC, CB)) += 1;
    }
    fprintf(F, "\t%d\t%d\t%d\t%d", H.GetDat(TIntPr(1,1)), H.GetDat(TIntPr(1,-1)), H.GetDat(TIntPr(-1,1)), H.GetDat(TIntPr(-1,-1)));
    fprintf(F, "\t%d\t%d\t%d\t%d", H.GetDat(TIntPr(1,2)), H.GetDat(TIntPr(1,-2)), H.GetDat(TIntPr(-1,2)), H.GetDat(TIntPr(-1,-2)));
    fprintf(F, "\t%d\t%d\t%d\t%d", H.GetDat(TIntPr(2,1)), H.GetDat(TIntPr(2,-1)), H.GetDat(TIntPr(-2,1)), H.GetDat(TIntPr(-2,-1)));
    fprintf(F, "\t%d\t%d\t%d\t%d", H.GetDat(TIntPr(2,2)), H.GetDat(TIntPr(2,-2)), H.GetDat(TIntPr(-2,2)), H.GetDat(TIntPr(-2,-2)));
    if (SaveEol) { fprintf(F, "\n"); }
    if (DelEdge) { Network->AddEdge(SrcNId, DstNId, TrueSign); }
    return true;
  }
  // save features for edge sign prediction
  void SaveEdgeSignPredFeatures(const TStr& OutFNm) {
    FILE *F = fopen(TStr::Fmt("tr16pred-%s-nbh%d.tab", OutFNm.CStr(), MinCmnNbrs).CStr(), "wt");
    //FILE *F = fopen(TStr::Fmt("tr16pred-%s-nbh%d.tab", OutFNm.CStr(), MinCmnNbrs).CStr(), "wt");
    fprintf(F, "Edge\tAOutDeg\tAOutPlus\tAOutMinus\tBInDeg\tBInPlus\tBInMinus\tCmnNbrs");
    fprintf(F, "\tFFpp\tFFpm\tFFmp\tFFmm"); // feed forward 
    fprintf(F, "\tFBpp\tFBpm\tFBmp\tFBmm"); // collision
    fprintf(F, "\tBFpp\tBFpm\tBFmp\tBFmm"); // split
    fprintf(F, "\tBBpp\tBBpm\tBBmp\tBBmm\n"); // feed backward
    TIntV NbhV;
    int NegEdgeCnt=0;
    //Network->MakeStatusConsistent();
    TIntPrV EdgeV;
    for (TSignNet::TEdgeI EI = Network->BegEI(); EI < Network->EndEI(); EI++) {
      if (EI()==-1) { NegEdgeCnt++; }
      EdgeV.Add(TIntPr(EI.GetSrcNId(), EI.GetDstNId()));
    }
    EdgeV.Shuffle(TInt::Rnd);
    NegEdgeCnt *= 2;
    for (int edge = 0; edge < EdgeV.Len(); edge++) {
      const int SrcNId = EdgeV[edge].Val1;
      const int DstNId = EdgeV[edge].Val2;
      const int TrueSign = Network->GetEDat(SrcNId, DstNId);
      const int CmnNbrs = TSnap::GetCmnNbhs(Network, SrcNId, DstNId);
      if (CmnNbrs < MinCmnNbrs) { continue; }
      if (TrueSign==1 && NegEdgeCnt <= 0) { continue; }
      if (SaveEdgeAttrs(F, SrcNId, DstNId) && TrueSign==1) { NegEdgeCnt--; }
    }
    fclose(F);
  }
  bool HasMinusEdge(const int& NId) const {
    const TSignNet::TNodeI NI = Network->GetNI(NId);
    for (int i = 0; i < NI.GetOutDeg(); i++) { 
      if (NI.GetOutEDat(i)==-1) { return true; } }
    for (int i = 0; i < NI.GetInDeg(); i++) { 
      if (NI.GetInEDat(i)==-1) { return true; } }
    return false;
  }
  // save features for trust prediction (is there a trust edge vs. there is no edge), (A,B) vs (A,D)
  void SaveEdgePredFeatures(const TStr& OutFNm, const int& SavePairs) {
    FILE *F = fopen(TStr::Fmt("edgePred-%s-nbh%d.tab", OutFNm.CStr(), MinCmnNbrs).CStr(), "wt");
    fprintf(F, "Edge\tAOutDeg\tAOutPlus\tAOutMinus\tBInDeg\tBInPlus\tBInMinus\tCmnNbrs");
    fprintf(F, "\tFFpp\tFFpm\tFFmp\tFFmm"); // feed forward 
    fprintf(F, "\tFBpp\tFBpm\tFBmp\tFBmm"); // collision
    fprintf(F, "\tBFpp\tBFpm\tBFmp\tBFmm"); // split
    fprintf(F, "\tBBpp\tBBpm\tBBmp\tBBmm\tCmnPlus\n"); // feed backward
    TIntPrV EdgeV, NoEdgeV;
    TIntV NbhV, Hop2NIdV;
    TIntH EmbeddH;
    PSignNet PlusNet = TSnap::GetEDatSubGraph(Network, 1, 0);
    PlusNet->PrintInfo();
    for (TSignNet::TEdgeI EI = PlusNet->BegEI(); EI < PlusNet->EndEI(); EI++) {
      EdgeV.Add(TIntPr(EI.GetSrcNId(), EI.GetDstNId()));
    }
    EdgeV.Shuffle(TInt::Rnd);
    int SaveTrust=0, SaveNoEdge=0;
    for (int e = 0; e < EdgeV.Len(); e++) {
      int Src = EdgeV[e].Val1, Dst = EdgeV[e].Val2;
      if (! HasMinusEdge(Src) || ! HasMinusEdge(Dst)) { continue; }
      Hop2NIdV.Clr(false);
      TSnap::GetNodesAtHop(PlusNet, Src, 2, Hop2NIdV);
      const int CmnNbrs = TSnap::GetCmnNbhs(PlusNet, Src, Dst, NbhV);
      if (CmnNbrs >= MinCmnNbrs && SaveTrust<SavePairs) { 
        SaveEdgeAttrs(F, Src, Dst, false); 
        fprintf(F, "\t%d\n", CmnNbrs);
        EmbeddH.AddDat(CmnNbrs) += 1;
        printf(" %d", CmnNbrs);
        SaveTrust++;
      }
      for (int n = 0; n < Hop2NIdV.Len(); n++) {
        const int CmnNbrs2 = TSnap::GetCmnNbhs(PlusNet, Src, Hop2NIdV[n], NbhV);
        if(EmbeddH.IsKey(CmnNbrs2) && EmbeddH.GetDat(CmnNbrs2) > 0 && SaveNoEdge<SavePairs) {
          SaveEdgeAttrs(F, Src, Hop2NIdV[n], false);
          fprintf(F, "\t%d\n", CmnNbrs2);
          printf("x");
          EmbeddH.GetDat(CmnNbrs2)--;
          SaveNoEdge++;
        }
      }
      if (SaveTrust==SavePairs && SaveNoEdge==SavePairs) {
        printf("%d pairs of nodes saved\ndone.\n", SaveNoEdge);
        break;
      }
    }
    fclose(F);
  }
  // (A,B) vs (C,D) -- trust edge vs. no edge
  void SaveEdgePredFeatures2(const TStr& OutFNm, const int& SavePairs) {
    FILE *F = fopen(TStr::Fmt("edgePred2-%s-nbh%d.tab", OutFNm.CStr(), MinCmnNbrs).CStr(), "wt");
    fprintf(F, "Edge\tAOutDeg\tAOutPlus\tAOutMinus\tBInDeg\tBInPlus\tBInMinus\tCmnNbrs");
    fprintf(F, "\tFFpp\tFFpm\tFFmp\tFFmm"); // feed forward 
    fprintf(F, "\tFBpp\tFBpm\tFBmp\tFBmm"); // collision
    fprintf(F, "\tBFpp\tBFpm\tBFmp\tBFmm"); // split
    fprintf(F, "\tBBpp\tBBpm\tBBmp\tBBmm\tCmnPlus\n"); // feed backward
    TIntPrV EdgeV, NoEdgeV;
    TIntV NbhV, Hop2NIdV;
    THash<TInt, TIntPrV> EmbedEdgeVH;
    TIntH EmbedH;
    PSignNet PlusNet = TSnap::GetEDatSubGraph(Network, 1, 0);
    PlusNet->PrintInfo();
    for (TSignNet::TEdgeI EI = PlusNet->BegEI(); EI < PlusNet->EndEI(); EI++) {
      const int Cmn = TSnap::GetCmnNbhs(PlusNet, EI.GetSrcNId(), EI.GetDstNId(), NbhV);
      EmbedEdgeVH.AddDat(Cmn).Add(TIntPr(EI.GetSrcNId(), EI.GetDstNId()));
      EmbedH.AddDat(Cmn) += 1;
    }
    for (int e = 0; e < EmbedEdgeVH.Len(); e++) {
      EmbedEdgeVH[e].Shuffle(TInt::Rnd);
    }
    for (int Saved = 0; Saved < SavePairs; ) {
      const int SrcNId = Network->GetRndNId();
      const int DstNId = Network->GetRndNId();
      if (SrcNId == DstNId || Network->IsEdge(SrcNId, DstNId, false)) { continue; }
      const int Cmn = TSnap::GetCmnNbhs(PlusNet, SrcNId, DstNId, NbhV);
      if (Cmn < MinCmnNbrs) { continue; }
      if (! EmbedH.IsKey(Cmn) || EmbedH.GetDat(Cmn)==0) { continue; }
      const int i = EmbedH.GetDat(Cmn)-1;
      //IAssert(TSnap::GetCmnNbhs(PlusNet, SrcNId, DstNId, NbhV) == TSnap::GetCmnNbhs(PlusNet, EmbedEdgeVH.GetDat(Cmn)[i].Val1, EmbedEdgeVH.GetDat(Cmn)[i].Val2, NbhV));
      SaveEdgeAttrs(F, EmbedEdgeVH.GetDat(Cmn)[i].Val1, EmbedEdgeVH.GetDat(Cmn)[i].Val2, false);  fprintf(F, "\t%d\n", Cmn);
      SaveEdgeAttrs(F, SrcNId, DstNId, false);  fprintf(F, "\t%d\n", Cmn);
      printf(".");
      EmbedH.GetDat(Cmn)--;
      Saved++;
    }
    fclose(F);
  }
};

#endif
