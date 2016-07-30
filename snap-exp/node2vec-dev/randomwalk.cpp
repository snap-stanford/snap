//Preprocess alias sampling method
void GetNodeAlias(TFltV& PTblV, TIntVFltVPr& NTTable) {
  int N = PTblV.Len();
  TIntV KTbl(N);
  TFltV UTbl(N);
  for (int i = 0; i < N; i++) {
    KTbl[i]=0;
    UTbl[i]=0;
  }
  TIntV UnderV;
  TIntV OverV;
  for (int i = 0; i < N; i++) {
    UTbl[i] = PTblV[i]*N;
    if (UTbl[i] < 1) {
      UnderV.Add(i);
    } else {
      OverV.Add(i);
    }
  }
  while (UnderV.Len() > 0 && OverV.Len() > 0) {
    int Small = UnderV.Last();
    int Large = OverV.Last();
    UnderV.DelLast();
    OverV.DelLast();
    KTbl[Small] = Large;
    UTbl[Large] = UTbl[Large] + UTbl[Small] - 1;
    if (UTbl[Large] < 1) {
      UnderV.Add(Large);
    } else {
      OverV.Add(Large);
    }
  }
  NTTable = TIntVFltVPr(KTbl,UTbl);
}
//Get random element using alias sampling method
int AliasDrawInt(TIntVFltVPr& NTTable, TRnd& Rnd) {
  int N = NTTable.GetVal1().Len();
  TInt X = static_cast<int>(Rnd.GetUniDev()*N);
  double Y = Rnd.GetUniDev();
  return Y < NTTable.GetVal2()[X] ? X : NTTable.GetVal1()[X];
}
//Preprocess transition probabilities for each path t->v->x
void PreprocessTransitionProbs(PWNet& InNet, double& ParamP, double& ParamQ) {
  for (TWNet::TNodeI NI = InNet->BegNI(); NI < InNet->EndNI(); NI++) {
    InNet->SetNDat(NI.GetId(),TIntIntVFltVPrH());
  }
  for (TWNet::TNodeI NI = InNet->BegNI(); NI < InNet->EndNI(); NI++) {
    //for each node t
    THash <TInt, TBool> NbrH;//Neighbors of t
    for (int i = 0; i < NI.GetOutDeg(); i++) {
      NbrH.AddKey(NI.GetNbrNId(i));
    } 
    for (int i = 0; i < NI.GetOutDeg(); i++) {
      TWNet::TNodeI CurrI = InNet->GetNI(NI.GetNbrNId(i));//for each node v
      double Psum = 0;
      TFltV PTable;//Probability distribution table
      for (int j = 0; j < CurrI.GetOutDeg(); j++) {//for each node x
        int FId = CurrI.GetNbrNId(j);
        TFlt Weight;
        if (!(InNet->GetEDat(CurrI.GetId(),FId,Weight))){ continue; }
        if (FId==NI.GetId()) {
          PTable.Add(Weight / ParamP);
          Psum += Weight / ParamP;
        } else if (NbrH.IsKey(FId)) {
          PTable.Add(Weight);
          Psum += Weight;
        } else {
          PTable.Add(Weight / ParamQ);
          Psum += Weight / ParamQ;
        }
      }
      //Normalizing table
      for (int j = 0; j < CurrI.GetOutDeg(); j++) {
        PTable[j] /= Psum;
      }
      TIntVFltVPr NTTable;
      GetNodeAlias(PTable,NTTable);
      CurrI.GetDat().AddDat(NI.GetId(),NTTable);
    }
  }
}
//Simulates a random walk
void SimulateWalk(PWNet& InNet, int StartNId, int& WalkLen, TRnd& Rnd, TIntV& WalkV) {
  WalkV.Add(StartNId);
  if (WalkLen == 1) { return; }
  if (InNet->GetNI(StartNId).GetDeg() == 0) { return; }
  WalkV.Add(InNet->GetNI(StartNId).GetNbrNId(Rnd.GetUniDevInt(InNet->GetNI(StartNId).GetDeg())));
  while (WalkV.Len() < WalkLen) {
    int Dst = WalkV.Last();
    int Src = WalkV.LastLast();
    if (InNet->GetNI(Dst).GetDeg() == 0) { return; }
    int Next = AliasDrawInt(InNet->GetNDat(Dst).GetDat(Src),Rnd);
    WalkV.Add(InNet->GetNI(Dst).GetNbrNId(Next));
  }
}
