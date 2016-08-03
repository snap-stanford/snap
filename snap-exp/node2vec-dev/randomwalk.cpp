//Preprocess alias sampling method
void GetNodeAlias(TFltV& PTblV, TIntVFltVPr& NTTable) {
  int N = PTblV.Len();
  TIntV& KTbl = NTTable.Val1;//.GetVal1();//(N);
  TFltV& UTbl = NTTable.Val2;//.GetVal2();//(N);
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
  //NTTable = TIntVFltVPr(KTbl,UTbl);
}
//Get random element using alias sampling method
int AliasDrawInt(TIntVFltVPr& NTTable, TRnd& Rnd) {
  int N = NTTable.GetVal1().Len();
  TInt X = static_cast<int>(Rnd.GetUniDev()*N);
  double Y = Rnd.GetUniDev();
  return Y < NTTable.GetVal2()[X] ? X : NTTable.GetVal1()[X];
}

struct TTransPDat {//for multithreading purposes
  PWNet& InNet;
  double& ParamP;
  double& ParamQ;
  TWNet::TNodeI FirstNI;
  TWNet::TNodeI LastNI;
  int& NCnt;
  TTransPDat (PWNet& InNet, double& ParamP, double& ParamQ, TWNet::TNodeI FirstNI,
   TWNet::TNodeI LastNI, int& NCnt) : InNet(InNet), ParamP(ParamP), ParamQ(ParamQ),
   FirstNI(FirstNI), LastNI(LastNI), NCnt(NCnt) {}
};

void* PreprocessThread (void* ThrDat) {
  TTransPDat* Dat = (TTransPDat*) ThrDat;
  PWNet& InNet = Dat->InNet;
  double& ParamP = Dat->ParamP;
  double& ParamQ = Dat->ParamQ;
  for (TWNet::TNodeI NI = Dat->FirstNI; NI < Dat->LastNI; NI++) {
    if (Dat->NCnt%100 == 0) {
      printf("%cPreprocessing progress: %.2lf%% ",13,(double)Dat->NCnt*100/(double)(InNet->GetNodes()));fflush(stdout);
    }
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
      GetNodeAlias(PTable,CurrI.GetDat().GetDat(NI.GetId()));
      //CurrI.GetDat().AddDat(NI.GetId(),NTTable);
    }
    Dat->NCnt++;
  }
  pthread_exit(NULL);
}

//Preprocess transition probabilities for each path t->v->x
void PreprocessTransitionProbs(PWNet& InNet, double& ParamP, double& ParamQ, int& Workers) {
  for (TWNet::TNodeI NI = InNet->BegNI(); NI < InNet->EndNI(); NI++) {
    InNet->SetNDat(NI.GetId(),TIntIntVFltVPrH());
  }
  for (TWNet::TNodeI NI = InNet->BegNI(); NI < InNet->EndNI(); NI++) {
    for (int i = 0; i < NI.GetOutDeg(); i++) {//allocating space in advance to avoid issues with multithreading
      TWNet::TNodeI CurrI = InNet->GetNI(NI.GetNbrNId(i));
      CurrI.GetDat().AddDat(NI.GetId(),TPair<TIntV,TFltV>(TIntV(CurrI.GetOutDeg()),TFltV(CurrI.GetOutDeg())));
    }
  }
  pthread_t Threads[Workers];
  TTransPDat* ThreadDat[Workers];
  TWNet::TNodeI Last = InNet->BegNI();
  int NCnt = 0, NScheduled = 0, CurrWorker = 0;
  for (TWNet::TNodeI NI = InNet->BegNI(); NI <= InNet->EndNI(); NI++) {
    NScheduled++;
    if((CurrWorker+1)*InNet->GetNodes()/Workers<=NScheduled){
      ThreadDat[CurrWorker] = new TTransPDat(InNet, ParamP, ParamQ, Last, NI, NCnt);
      Last = NI;
      CurrWorker++;
    }
  }
  for (int i = 0; i < Workers; i++) {
    pthread_create(&Threads[i], NULL, PreprocessThread, (void *)ThreadDat[i]);
  }
  for (int i = 0; i < Workers; i++) { pthread_join(Threads[i],NULL); }
  printf("\n");
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
