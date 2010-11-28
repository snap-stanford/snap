/////////////////////////////////////////////////
// Includes
#include "ts.h"

/////////////////////////////////////////////////
// Test-Model
void TTsMd::AddEst(const TTsMdEst& Est, const double& Val){
  IAssert(!EstActValPrV[Est].Val1);
  EstActValPrV[Est].Val1=true;
  EstActValPrV[Est].Val2=Val;
}

void TTsMd::DefDscEst(){
  double SumW=CorrValDs->GetSumW();
  if (SumW==0){return;}

  double AltSumW=0; // alternative summary weight
  TFltVV CorrXPredValWVV(CorrValDs->GetDscs(), CorrValDs->GetDscs());
    // CorrVal*PredVal weighted-sum
  TFltV CorrValWV(CorrValDs->GetDscs()); // CorrVal weighted-sum
  TFltV PredValWV(CorrValDs->GetDscs()); // PredVal weighted-sum
  double CorrEqPredValSumW=0; // weighted-sum when CorrVal==PredVal
  double CorrValPrbSumW=0; // CorrValPrb weighted-sum
  double CorrPosValPrbSumW=0; // CorrValPrb weighted-sum when CorrVal=1 && Vals==2
  double PredPosValPrbSumW=0; // PredValPrb weighted-sum when PredVal=1 && Vals==2
  double InfScoreSumW=0; // information-score-terms weighted-sum

  for (int PredN=0; PredN<PredV.Len(); PredN++){
    PTsPred Pred=PredV[PredN];
    PTbValDs PredValDs=Pred->GetPredValDs();
    TTbVal PredVal=PredValDs->GetMajorVal();
    TTbVal CorrVal=Pred->GetCorrVal();
    double Wgt=Pred->GetWgt();
    double CorrValPrb=PredValDs->GetPrb_RelFq(CorrVal);
    double CorrValPriorPrb=PriorValDs->GetPrb_RelFq(CorrVal);
    double PredValPrb=PredValDs->GetPrb_RelFq(PredVal);

    AltSumW+=Wgt;
    if ((CorrVal.GetValTag()==tvtDsc)&&(PredVal.GetValTag()==tvtDsc)){
      CorrXPredValWVV.At(CorrVal.GetDsc(), PredVal.GetDsc())+=Wgt;
      CorrValWV[CorrVal.GetDsc()]+=Wgt;
      PredValWV[PredVal.GetDsc()]+=Wgt;
    }
    if (CorrVal==PredVal){CorrEqPredValSumW+=Wgt;}
    CorrValPrbSumW+=Wgt*CorrValPrb;
    if (CorrValWV.Len()==2){
      if (CorrVal.GetDsc()==1){CorrPosValPrbSumW+=Wgt*CorrValPrb;}
      if (PredVal.GetDsc()==1){
        if (CorrVal==PredVal){PredPosValPrbSumW+=Wgt*PredValPrb;}
        else {PredPosValPrbSumW+=Wgt*(1-PredValPrb);}
      }
    }

    if (CorrValPrb>CorrValPriorPrb){
      InfScoreSumW+=Wgt*(log2(CorrValPrb)-log2(CorrValPriorPrb));
    } else {
      InfScoreSumW+=Wgt*(-log2(1-CorrValPrb)-log2(1-CorrValPriorPrb));
    }
  }
  IAssert(SumW==AltSumW);

  // Accuracy
  AddEst(tmeAcc, CorrEqPredValSumW/SumW);

  // Probability-Accuracy
  AddEst(tmePrbAcc, CorrValPrbSumW/SumW);

  // Information-Score
  AddEst(tmeInfScore, InfScoreSumW/SumW);

  // Geometric-Precision
  double GeoPrecision=1;
  for (int Dsc=0; Dsc<PredValWV.Len(); Dsc++){
    if (double(PredValWV[Dsc])==0){GeoPrecision*=1;} // by definition
    else {GeoPrecision*=CorrXPredValWVV.At(Dsc, Dsc)/PredValWV[Dsc];}
  }
  if (GeoPrecision>0){GeoPrecision=pow(GeoPrecision, 1/PredValWV.Len());}
  AddEst(tmeGeoPrecision, GeoPrecision);

  // Geometric-Recall
  double GeoRecall=1;
  {for (int Dsc=0; Dsc<CorrValWV.Len(); Dsc++){
    if (double(CorrValWV[Dsc])==0){GeoRecall*=1;} // by definition
    else {GeoRecall*=CorrXPredValWVV.At(Dsc, Dsc)/CorrValWV[Dsc];}
  }}
  if (GeoRecall>0){GeoRecall=pow(GeoRecall, 1/CorrValWV.Len());}
  AddEst(tmeGeoRecall, GeoRecall);

  if (CorrValWV.Len()==2){
    // Contingency-Table-Shortcuts
    double a=CorrXPredValWVV.At(1, 1);
    double b=CorrXPredValWVV.At(0, 1);
    double c=CorrXPredValWVV.At(1, 0);
//    double d=CorrXPredValWVV.At(0, 0); never used

    // Precision
    double Precision;
    if (a+b==0){Precision=1;} else {Precision=a/(a+b);}
    AddEst(tmePrecision, Precision);

    // Recall
    double Recall;
    if (a+c==0){Recall=1;} else {Recall=a/(a+c);}
    AddEst(tmeRecall, Recall);

    // Probability-Precision
    AddEst(tmePrbPrecision, PredPosValPrbSumW/PredValWV[1]);

    // Probability-Recall
    AddEst(tmePrbRecall, CorrPosValPrbSumW/CorrValWV[1]);

    // Square-Error-In-Counting
    AddEst(tmeSec, sqr(b-c));

    // F-Measure
    const double Beta=2; // 0.5, 1, 2
    double FMeasure;
    if ((a+b==0)&&(a+c==0)){FMeasure=1;}
    else if ((a+b==0)&&(a+c>0)){FMeasure=0;}
    else {FMeasure=((sqr(Beta)+1)*a)/((sqr(Beta)+1)*a+b+sqr(Beta)*c);}
    AddEst(tmeF2, FMeasure);
  }
}

void TTsMd::DefFltEst(){
}

void TTsMd::Def(){
  IAssert(!DefP); DefP=true;
  CorrValDs->Def();
  if (CorrValDs->IsDsc()){DefDscEst();}
  else if (CorrValDs->IsFlt()){DefFltEst();}
}

TStr TTsMd::GetTsMdEstDNm(const TTsMdEst& Est){
  switch (Est){
    case tmeAcc: return "Accuracy";
    case tmePrbAcc: return "Probability-Accuracy";
    case tmeInfScore: return "Information-Score";
    case tmeGeoPrecision: return "Geometric-Precision";
    case tmeGeoRecall: return "Geometric-Recall";
    case tmePrecision: return "Precision";
    case tmeRecall: return "Recall";
    case tmePrbPrecision: return "Probability-Precision";
    case tmePrbRecall: return "Probability-Recall";
    case tmeSec: return "Square-Error-In-Counting";
    case tmeF2: return "F-Measure";
    default: Fail; return TStr();
  }
}

PTsMd TTsMd::GetTsMd(
 const PMd& Md, const PDm& Dm, const int& ClassN, const PExSet& ExSet){
  PTsMd TsMd=new TTsMd(Md->GetPriorValDs(ClassN));
  TDmValRet& DmValRet=*new TDmValRet(Md->GetDmHd(), Dm); PValRet ValRet(&DmValRet);
  int ExP; ExSet->FFirstExP(ExP);
  while (ExSet->FNextExP(ExP)){
    int ExN=ExSet->GetExN(ExP); double ExW=ExSet->GetExW(ExP);
    DmValRet.SetExN(ExN);
    PTbValDs PostrValDs=Md->GetPostrValDs(ValRet, ClassN);
    TTbVal CorrVal=Dm->GetClassVal(ExN, ClassN);
    PTsPred Pred=new TTsPred(PostrValDs, CorrVal, ExW);
    TsMd->AddPred(Pred);
  }
  TsMd->Def();
  return TsMd;
}

/////////////////////////////////////////////////
// Test-Model-Generator
TPt<TTsMg> TTsMg::Load(TSIn& SIn){
  TStr TypeNm(SIn);
  if (TypeNm==TTypeNm<TTsMgCV>()){return new TTsMgCV(SIn);}
  else if (TypeNm==TTypeNm<TTsMgHO>()){return new TTsMgHO(SIn);}
  else {Fail; return NULL;}
}

const TStr TTsMg::DNm("Test-Model-Generator");

PPp TTsMg::GetPp(const TStr& Nm, const TStr& DNm){
  PPp Pp=new TPp(Nm, DNm, pvtSel);
  Pp->AddPp(TTsMgCV::GetPp(TTypeNm<TTsMgCV>(), TTsMgCV::DNm));
  Pp->AddPp(TTsMgHO::GetPp(TTypeNm<TTsMgHO>(), TTsMgHO::DNm));
  return Pp;
}

PTsMg TTsMg::New(const PPp& Pp, const PDm& Dm, const int& ClassN){
  if (Pp->GetVal()==TPpVal(TTypeNm<TTsMgCV>())){
    return new TTsMgCV(Pp->GetSelPp(), Dm, ClassN);}
  else if (Pp->GetVal()==TPpVal(TTypeNm<TTsMgHO>())){
    return new TTsMgHO(Pp->GetSelPp(), Dm, ClassN);}
  else {Fail; return NULL;}
}

/////////////////////////////////////////////////
// Test-Model-Generator-With-Cross-Validation
void TTsMgCV::GetTsMdV(const int& Folds, const PMg& Mg,
 const PDm& Dm, const int& ClassN){
  PExSet DmExSet=Dm->GetExSet();
  TVec<PExSet> FoldV(Folds, 0);        
  DmExSet->SplitToFolds(Folds, FoldV);
  for (int TsFoldN=0; TsFoldN<Folds; TsFoldN++){
    PExSet TrExSet=TExSet::New(GetTypeNm(*DmExSet));
    for (int TrFoldN=0; TrFoldN<Folds; TrFoldN++){
      if (TrFoldN!=TsFoldN){TrExSet->AddExSet(FoldV[TrFoldN]);}}
    TrExSet->Def();
    PMd Md=Mg->GetMd(TrExSet);
    PTsMd TsMd=TTsMd::GetTsMd(Md, Dm, ClassN, FoldV[TsFoldN]);
    TsMdV.Add(TsMd);
  }
  Def();
}

TTsMgCV::TTsMgCV(const PPp& Pp, const PDm& _Dm, const int& _ClassN):
  TTsMg(), TsMdV(){
  int Folds=Pp->GetValInt("Folds");
  PDm Dm=TDm::New(Pp->GetPp(TTypeNm<TDm>()), _Dm);
  int ClassN=TClassId::New(Pp->GetPp(TTypeNm<TClassId>()), Dm, _ClassN);
  PMg Mg=TMg::New(Pp->GetPp(TTypeNm<TMg>()), Dm, ClassN);
  GetTsMdV(Folds, Mg, Dm, ClassN);
}

const TStr TTsMgCV::DNm("Cross-Validation");

PPp TTsMgCV::GetPp(const TStr& Nm, const TStr& DNm){
  PPp Pp=new TPp(Nm, DNm, pvtSet);
  Pp->AddPpInt("Folds", "Folds", 2, TInt::Mx, 10);
  Pp->AddPp(TDm::GetPp(TTypeNm<TDm>(), TDm::DNm));
  Pp->AddPp(TClassId::GetPp(TTypeNm<TClassId>(), TClassId::DNm));
  Pp->AddPp(TMg::GetPp(TTypeNm<TMg>(), TMg::DNm));
  return Pp;
}

void TTsMgCV::Wr(){
  for (int TsMdN=0; TsMdN<TsMdV.Len(); TsMdN++){
    printf("%d: %f\n", TsMdN, TsMdV[TsMdN]->GetEstVal(tmeAcc));}
}

/////////////////////////////////////////////////
// Test-Model-Generator-With-Hold-Out
void TTsMgHO::GetTsMdV(const int& Trials, const double& TrnPrc, const PMg& Mg,
 const PDm& Dm, const int& ClassN){
  PExSet DmExSet=Dm()->GetExSet();
  for (int TrialN=0; TrialN<Trials; TrialN++){
    PExSet TrExSet, TsExSet;
    DmExSet->SplitPerPrb(TrnPrc/100, TrExSet, TsExSet);
    PMd Md=Mg->GetMd(TrExSet);
    PTsMd TsMd=TTsMd::GetTsMd(Md, Dm, ClassN, TsExSet);
    TsMdV.Add(TsMd);
  }
  Def();
}

TTsMgHO::TTsMgHO(const PPp& Pp, const PDm& _Dm, const int& _ClassN):
  TTsMg(), TsMdV(){
  int Trials=Pp->GetValInt("Trials");
  double TrnPrc=Pp->GetValFlt("TrnPrc");
  PDm Dm=TDm::New(Pp->GetPp(TTypeNm<TDm>()), _Dm);
  int ClassN=TClassId::New(Pp->GetPp(TTypeNm<TClassId>()), Dm, _ClassN);
  PMg Mg=TMg::New(Pp->GetPp(TTypeNm<TMg>()), Dm, ClassN);
  GetTsMdV(Trials, TrnPrc, Mg, Dm, ClassN);
}

const TStr TTsMgHO::DNm("Hold-Out");

PPp TTsMgHO::GetPp(const TStr& Nm, const TStr& DNm){
  PPp Pp=new TPp(Nm, DNm, pvtSet);
  Pp->AddPpInt("Trials", "Trials", 1, TInt::Mx, 10);
  Pp->AddPpFlt("TrnPrc", "Train-Examples(%)", 0, 100, 70);
  Pp->AddPp(TDm::GetPp(TTypeNm<TDm>(), TDm::DNm));
  Pp->AddPp(TClassId::GetPp(TTypeNm<TClassId>(), TClassId::DNm));
  Pp->AddPp(TMg::GetPp(TTypeNm<TMg>(), TMg::DNm));
  return Pp;
}

void TTsMgHO::Wr(){
  for (int TsMdN=0; TsMdN<TsMdV.Len(); TsMdN++){
    printf("%d: %f\n", TsMdN, TsMdV[TsMdN]->GetEstVal(tmeAcc));}
}

