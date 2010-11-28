/////////////////////////////////////////////////
// Example-Set
PExSet TExSet::New(const TStr& TypeNm){
  if (TypeNm==TTypeNm<TGExSet>()){return new TGExSet();}
  else {Fail; return NULL;}
}

PExSet TExSet::Load(TSIn& SIn){
  TStr TypeNm(SIn);
  if (TypeNm==TTypeNm<TGExSet>()){return new TGExSet(SIn);}
  else {Fail; return NULL;}
}

/////////////////////////////////////////////////
// Generic-Example-Set
void TGExSet::RecalcExsWgt(){
  ExsWgt=0; int Exs=GetExs();
  for (int ExP=0; ExP<Exs; ExP++){
    ExsWgt+=GetExWgt(ExP);}
}

void TGExSet::NewV(const int& ExSets, TExSetV& ExSetV){
  ExSetV.Gen(ExSets);
  for (int ExSetN=0; ExSetN<ExSets; ExSetN++){
    ExSetV[ExSetN]=TGExSet::New();}
}

void TGExSet::SplitPerPrb(
 const double& LPrb, TRnd& Rnd, PExSet& LExSet, PExSet& RExSet){
  IAssert((0<=LPrb)&&(LPrb<=1));
  int Exs=GetExs();
  // prepare example-numbers shuffle
  TIntV ExPV(Exs, 0);
  for (int ExPN=0; ExPN<Exs; ExPN++){ExPV.Add(ExPN);}
  ExPV.Shuffle(Rnd);
  // prepare left & right example-sets
  int LExs=TFlt::Round(LPrb*Exs); int RExs=Exs-LExs;
  TGExSet* LGExSet=new TGExSet(LExs); LExSet=PExSet(LGExSet);
  TGExSet* RGExSet=new TGExSet(RExs); RExSet=PExSet(RGExSet);
  // left example-set
  for (int LExPN=0; LExPN<LExs; LExPN++){
    TIntFltKd& ExIdWgtKd=ExIdWgtKdV[ExPV[LExPN]];
    LGExSet->ExIdWgtKdV.Add(ExIdWgtKd);
    LGExSet->ExsWgt+=ExIdWgtKd.Dat;
  }
  LGExSet->ExIdWgtKdV.Sort();
  // right example-set
  for (int RExPN=LExs; RExPN<Exs; RExPN++){
    TIntFltKd& ExIdWgtKd=ExIdWgtKdV[ExPV[RExPN]];
    RGExSet->ExIdWgtKdV.Add(ExIdWgtKd);
    RGExSet->ExsWgt+=ExIdWgtKd.Dat;
  }
  RGExSet->ExIdWgtKdV.Sort();
}

void TGExSet::SplitToFolds(const int& Folds, TRnd& Rnd, TExSetV& FoldV){
  IAssert(Folds>0);
  int Exs=GetExs();
  // prepare example-numbers shuffle
  TIntV ExPV(Exs, 0);
  for (int ExPN=0; ExPN<Exs; ExPN++){ExPV.Add(ExPN);}
  ExPV.Shuffle(Rnd);
  // prepare folds
  FoldV.Gen(Folds, 0);
  for (int FoldN=0; FoldN<Folds; FoldN++){
    // create fold
    TGExSet* GExSet=new TGExSet(Exs/Folds+1); PExSet ExSet=PExSet(GExSet);
    FoldV.Add(ExSet);
    // prepare example-set
    int ExPN=FoldN;
    while (ExPN<Exs){
      TIntFltKd& ExIdWgtKd=ExIdWgtKdV[ExPV[ExPN]];
      GExSet->ExIdWgtKdV.Add(ExIdWgtKd);
      GExSet->ExsWgt+=ExIdWgtKd.Dat;
      ExPN+=Folds;
    }
  }
}

PExSet TGExSet::GetIntrs(const PExSet& ExSet) const {
  TGExSet* GExSet=dynamic_cast<TGExSet*>(ExSet());
  TGExSet* DstGExSet=new TGExSet(); PExSet DstExSet=PExSet(DstGExSet);
  ExIdWgtKdV.Intrs(GExSet->ExIdWgtKdV, DstGExSet->ExIdWgtKdV);
  DstGExSet->RecalcExsWgt();
  return DstExSet;
}

/////////////////////////////////////////////////
// Binary-Example-Set
bool TBExSet::FNextExP(int& ExP) const {
  int ExIds=ExIdBSet->GetBits();
  do {
    ExP++;
    if (ExIdBSet->In(ExP)){return true;}
  } while (ExP<ExIds);
  return false;
}

void TBExSet::SplitPerPrb(
 const double& LPrb, TRnd& /*Rnd*/, PExSet& /*LExSet*/, PExSet& /*RExSet*/){
  IAssert((0<=LPrb)&&(LPrb<=1));
  Fail;
}

void TBExSet::SplitToFolds(const int& Folds, TRnd& /*Rnd*/, TExSetV& /*FoldV*/){
  IAssert(Folds>0);
  Fail;
}

PExSet TBExSet::GetIntrs(const PExSet& ExSet) const {
  TBExSet* BExSet=dynamic_cast<TBExSet*>(ExSet());
  TBExSet* DstBExSet=new TBExSet(ExIdBSet->GetBits());
  PExSet DstExSet=PExSet(DstBExSet);
  *DstBExSet->ExIdBSet=*ExIdBSet & *BExSet->ExIdBSet;
  return DstExSet;
}

