/////////////////////////////////////////////////
// Attribute-Estimator
PAttrEst TAttrEst::Load(TSIn& SIn){
  TStr TypeNm(SIn);
  if (TypeNm==TTypeNm<TAttrEstRnd>()){return new TAttrEstRnd(SIn);}
  else if (TypeNm==TTypeNm<TAttrEstIGain>()){return new TAttrEstIGain(SIn);}
  else if (TypeNm==TTypeNm<TAttrEstIGainNorm>()){return new TAttrEstIGainNorm(SIn);}
  else if (TypeNm==TTypeNm<TAttrEstIGainRatio>()){return new TAttrEstIGainRatio(SIn);}
  else if (TypeNm==TTypeNm<TAttrEstMantarasDist>()){return new TAttrEstMantarasDist(SIn);}
  else if (TypeNm==TTypeNm<TAttrEstMdl>()){return new TAttrEstMdl(SIn);}
  else if (TypeNm==TTypeNm<TAttrEstGStat>()){return new TAttrEstGStat(SIn);}
  else if (TypeNm==TTypeNm<TAttrEstChiSquare>()){return new TAttrEstChiSquare(SIn);}
  else if (TypeNm==TTypeNm<TAttrEstOrt>()){return new TAttrEstOrt(SIn);}
  else if (TypeNm==TTypeNm<TAttrEstGini>()){return new TAttrEstGini(SIn);}
  else if (TypeNm==TTypeNm<TAttrEstWgtEvd>()){return new TAttrEstWgtEvd(SIn);}
  else if (TypeNm==TTypeNm<TAttrEstTextWgtEvd>()){return new TAttrEstTextWgtEvd(SIn);}
  else if (TypeNm==TTypeNm<TAttrEstOddsRatio>()){return new TAttrEstOddsRatio(SIn);}
  else if (TypeNm==TTypeNm<TAttrEstWgtOddsRatio>()){return new TAttrEstWgtOddsRatio(SIn);}
  else if (TypeNm==TTypeNm<TAttrEstCondOddsRatio>()){return new TAttrEstCondOddsRatio(SIn);}
  else if (TypeNm==TTypeNm<TAttrEstLogPRatio>()){return new TAttrEstLogPRatio(SIn);}
  else if (TypeNm==TTypeNm<TAttrEstExpPDiff>()){return new TAttrEstExpPDiff(SIn);}
  else if (TypeNm==TTypeNm<TAttrEstMutInf>()){return new TAttrEstMutInf(SIn);}
  else if (TypeNm==TTypeNm<TAttrEstCrossEnt>()){return new TAttrEstCrossEnt(SIn);}
  else if (TypeNm==TTypeNm<TAttrEstTermFq>()){return new TAttrEstTermFq(SIn);}
  else {Fail; return NULL;}
}

PTbValDs TAttrEst::GetCSValDs(const int& AttrN, const int& SplitN,
 const PTbValSplit& ValSplit, const PDmDs& DmDs){
  PTbValDs CSValDs=new TTbValDs(DmDs->GetCDs()->GetDscs());
  for (int ValN=0; ValN<ValSplit->GetSplitVals(SplitN); ValN++){
    TTbVal Val=ValSplit->GetSplitVal(SplitN, ValN);
    for (int CDsc=0; CDsc<DmDs->GetCDs()->GetDscs(); CDsc++){
      double ValW=DmDs->GetCAVDs(CDsc, AttrN)->GetValW(Val);
      CSValDs->AddVal(CDsc, ValW);
    }
  }
  CSValDs->Def();
  return CSValDs;
}

PTbValDs TAttrEst::GetSValDs(const int& AttrN,
 const PTbValSplit& ValSplit, const PDmDs& DmDs){
  PTbValDs SValDs=new TTbValDs(ValSplit->GetSplits());
  for (int SplitN=0; SplitN<ValSplit->GetSplits(); SplitN++){
    for (int ValN=0; ValN<ValSplit->GetSplitVals(SplitN); ValN++){
      TTbVal Val=ValSplit->GetSplitVal(SplitN, ValN);
      double ValW=DmDs->GetAVDs(AttrN)->GetValW(Val);
      SValDs->AddVal(SplitN, ValW);
    }
  }
  SValDs->Def();
  return SValDs;
}

PTbValDs TAttrEst::GetSCValDs(const int& CDsc, const int& AttrN,
 const PTbValSplit& ValSplit, const PDmDs& DmDs){
  PTbValDs SCValDs=new TTbValDs(ValSplit->GetSplits());
  for (int SplitN=0; SplitN<ValSplit->GetSplits(); SplitN++){
    for (int ValN=0; ValN<ValSplit->GetSplitVals(SplitN); ValN++){
      TTbVal Val=ValSplit->GetSplitVal(SplitN, ValN);
      double ValW=DmDs->GetCAVDs(CDsc, AttrN)->GetValW(Val);
      SCValDs->AddVal(SplitN, ValW);
    }
  }
  SCValDs->Def();
  return SCValDs;
}

double TAttrEst::GetCEntropy(
 const PDmDs& DmDs, const PDmDs& PriorDmDs, const PPrbEst& PrbEst){
  double CEntropy=0;
  for (int CDsc=0; CDsc<DmDs->GetCDs()->GetDscs(); CDsc++){
    double CPrb=PrbEst->GetCPrb(CDsc, DmDs, PriorDmDs);
    if (CPrb>0){CEntropy-=CPrb*TMath::Log2(CPrb);}
  }
  return CEntropy;
}

double TAttrEst::GetAEntropy(const int& AttrN, const PTbValSplit& ValSplit,
 const PDmDs& DmDs, const PDmDs& PriorDmDs, const PPrbEst& PrbEst){
  PTbValDs SValDs=GetSValDs(AttrN, ValSplit, DmDs);
  PTbValDs PriorSValDs=GetSValDs(AttrN, ValSplit, PriorDmDs);
  double AEntropy=0;
  for (int SplitN=0; SplitN<ValSplit->GetSplits(); SplitN++){
    double SPrb=PrbEst->GetVPrb(SplitN, SValDs, PriorSValDs);
    if (SPrb>0){AEntropy-=SPrb*TMath::Log2(SPrb);}
  }
  return AEntropy;
}

double TAttrEst::GetCAEntropy(const int& AttrN, const PTbValSplit& ValSplit,
 const PDmDs& DmDs, const PDmDs& PriorDmDs, const PPrbEst& PrbEst){
  double CAEntropy=0;
  for (int SplitN=0; SplitN<ValSplit->GetSplits(); SplitN++){
    PTbValDs CSValDs=GetCSValDs(AttrN, SplitN, ValSplit, DmDs);
    double SPrb=CSValDs->GetSumPrb(DmDs->GetSumW());
    double SEntropy=0;
    for (int CDsc=0; CDsc<DmDs->GetCDs()->GetDscs(); CDsc++){
      double CAPrb=SPrb*PrbEst->GetVPrb(CDsc, CSValDs, PriorDmDs->GetCDs());
      if (CAPrb>0){SEntropy-=CAPrb*TMath::Log2(CAPrb);}
    }
    CAEntropy+=SEntropy;
  }
  return CAEntropy;
}

PPrbEst TAttrEst::GetPrbEst(const PPrbEst& PrbEst){
  if (!PrbEst.Empty()){return PrbEst;}
  else {return PPrbEst(new TPrbEstRelFq());}
}

PPp TAttrEst::GetPrbEstPp(const TStr& Nm, const TStr& DNm){
  PPp Pp=new TPp(Nm, DNm, ptSet);
  Pp->AddPp(TPrbEst::GetPp(TTypeNm<TPrbEst>(), TPrbEst::DNm));
  return Pp;
}

const TStr TAttrEst::DNm("Attribute Estimate");

PPp TAttrEst::GetPp(const TStr& Nm, const TStr& DNm){
  PPp Pp=new TPp(Nm, DNm, ptSel);
  Pp->AddPp(TAttrEstRnd::GetPp(TTypeNm<TAttrEstRnd>(), TAttrEstRnd::DNm));
  Pp->AddPp(TAttrEstIGain::GetPp(TTypeNm<TAttrEstIGain>(), TAttrEstIGain::DNm));
  Pp->AddPp(TAttrEstIGainNorm::GetPp(TTypeNm<TAttrEstIGainNorm>(), TAttrEstIGainNorm::DNm));
  Pp->AddPp(TAttrEstIGainRatio::GetPp(TTypeNm<TAttrEstIGainRatio>(), TAttrEstIGainRatio::DNm));
  Pp->AddPp(TAttrEstMantarasDist::GetPp(TTypeNm<TAttrEstMantarasDist>(), TAttrEstMantarasDist::DNm));
  Pp->AddPp(TAttrEstMdl::GetPp(TTypeNm<TAttrEstMdl>(), TAttrEstMdl::DNm));
  Pp->AddPp(TAttrEstGStat::GetPp(TTypeNm<TAttrEstGStat>(), TAttrEstGStat::DNm));
  Pp->AddPp(TAttrEstChiSquare::GetPp(TTypeNm<TAttrEstChiSquare>(), TAttrEstChiSquare::DNm));
  Pp->AddPp(TAttrEstOrt::GetPp(TTypeNm<TAttrEstOrt>(), TAttrEstOrt::DNm));
  Pp->AddPp(TAttrEstGini::GetPp(TTypeNm<TAttrEstGini>(), TAttrEstGini::DNm));
  Pp->AddPp(TAttrEstWgtEvd::GetPp(TTypeNm<TAttrEstWgtEvd>(), TAttrEstWgtEvd::DNm));
  Pp->AddPp(TAttrEstTextWgtEvd::GetPp(TTypeNm<TAttrEstTextWgtEvd>(), TAttrEstTextWgtEvd::DNm));
  Pp->AddPp(TAttrEstOddsRatio::GetPp(TTypeNm<TAttrEstOddsRatio>(), TAttrEstOddsRatio::DNm));
  Pp->AddPp(TAttrEstWgtOddsRatio::GetPp(TTypeNm<TAttrEstWgtOddsRatio>(), TAttrEstWgtOddsRatio::DNm));
  Pp->AddPp(TAttrEstCondOddsRatio::GetPp(TTypeNm<TAttrEstCondOddsRatio>(), TAttrEstCondOddsRatio::DNm));
  Pp->AddPp(TAttrEstLogPRatio::GetPp(TTypeNm<TAttrEstLogPRatio>(), TAttrEstLogPRatio::DNm));
  Pp->AddPp(TAttrEstExpPDiff::GetPp(TTypeNm<TAttrEstExpPDiff>(), TAttrEstExpPDiff::DNm));
  Pp->AddPp(TAttrEstMutInf::GetPp(TTypeNm<TAttrEstMutInf>(), TAttrEstMutInf::DNm));
  Pp->AddPp(TAttrEstCrossEnt::GetPp(TTypeNm<TAttrEstCrossEnt>(), TAttrEstCrossEnt::DNm));
  Pp->AddPp(TAttrEstTermFq::GetPp(TTypeNm<TAttrEstTermFq>(), TAttrEstTermFq::DNm));
  Pp->PutDfVal(TTypeNm<TAttrEstIGain>());
  return Pp;
}

PAttrEst TAttrEst::New(const PPp& Pp){
  if (Pp->GetVal()==TPpVal(TTypeNm<TAttrEstRnd>())){
    return new TAttrEstRnd(Pp->GetSelPp());}
  else if (Pp->GetVal()==TPpVal(TTypeNm<TAttrEstIGain>())){
    return new TAttrEstIGain(Pp->GetSelPp());}
  else if (Pp->GetVal()==TPpVal(TTypeNm<TAttrEstIGainNorm>())){
    return new TAttrEstIGainNorm(Pp->GetSelPp());}
  else if (Pp->GetVal()==TPpVal(TTypeNm<TAttrEstIGainRatio>())){
    return new TAttrEstIGainRatio(Pp->GetSelPp());}
  else if (Pp->GetVal()==TPpVal(TTypeNm<TAttrEstMantarasDist>())){
    return new TAttrEstMantarasDist(Pp->GetSelPp());}
  else if (Pp->GetVal()==TPpVal(TTypeNm<TAttrEstMdl>())){
    return new TAttrEstMdl(Pp->GetSelPp());}
  else if (Pp->GetVal()==TPpVal(TTypeNm<TAttrEstGStat>())){
    return new TAttrEstGStat(Pp->GetSelPp());}
  else if (Pp->GetVal()==TPpVal(TTypeNm<TAttrEstChiSquare>())){
    return new TAttrEstChiSquare(Pp->GetSelPp());}
  else if (Pp->GetVal()==TPpVal(TTypeNm<TAttrEstOrt>())){
    return new TAttrEstOrt(Pp->GetSelPp());}
  else if (Pp->GetVal()==TPpVal(TTypeNm<TAttrEstGini>())){
    return new TAttrEstGini(Pp->GetSelPp());}
  else if (Pp->GetVal()==TPpVal(TTypeNm<TAttrEstWgtEvd>())){
    return new TAttrEstWgtEvd(Pp->GetSelPp());}
  else if (Pp->GetVal()==TPpVal(TTypeNm<TAttrEstTextWgtEvd>())){
    return new TAttrEstTextWgtEvd(Pp->GetSelPp());}
  else if (Pp->GetVal()==TPpVal(TTypeNm<TAttrEstOddsRatio>())){
    return new TAttrEstOddsRatio(Pp->GetSelPp());}
  else if (Pp->GetVal()==TPpVal(TTypeNm<TAttrEstWgtOddsRatio>())){
    return new TAttrEstWgtOddsRatio(Pp->GetSelPp());}
  else if (Pp->GetVal()==TPpVal(TTypeNm<TAttrEstCondOddsRatio>())){
    return new TAttrEstCondOddsRatio(Pp->GetSelPp());}
  else if (Pp->GetVal()==TPpVal(TTypeNm<TAttrEstLogPRatio>())){
    return new TAttrEstLogPRatio(Pp->GetSelPp());}
  else if (Pp->GetVal()==TPpVal(TTypeNm<TAttrEstExpPDiff>())){
    return new TAttrEstExpPDiff(Pp->GetSelPp());}
  else if (Pp->GetVal()==TPpVal(TTypeNm<TAttrEstMutInf>())){
    return new TAttrEstMutInf(Pp->GetSelPp());}
  else if (Pp->GetVal()==TPpVal(TTypeNm<TAttrEstCrossEnt>())){
    return new TAttrEstCrossEnt(Pp->GetSelPp());}
  else if (Pp->GetVal()==TPpVal(TTypeNm<TAttrEstTermFq>())){
    return new TAttrEstTermFq(Pp->GetSelPp());}
  else {Fail; return NULL;}
}

/////////////////////////////////////////////////
// Attribute-Estimator-Random
const TStr TAttrEstRnd::DNm("Random");

PPp TAttrEstRnd::GetPp(const TStr& Nm, const TStr& DNm){
  PPp Pp=new TPp(Nm, DNm, ptSet);
  Pp->AddPp(TPrbEst::GetPp(TTypeNm<TPrbEst>(), TPrbEst::DNm));
  Pp->AddPpInt("Seed", "Random-Seed", 0, TInt::Mx, 1);
  return Pp;
}

/////////////////////////////////////////////////
// Attribute-Estimator-Information-Gain
const TStr TAttrEstIGain::DNm("Inf-Gain");

/////////////////////////////////////////////////
// Attribute-Estimator-Information-Gain-Normalized
const TStr TAttrEstIGainNorm::DNm("Inf-Gain-Normalized");

/////////////////////////////////////////////////
// Attribute-Estimator-Information-Gain-Ratio
const TStr TAttrEstIGainRatio::DNm("Inf-Gain-Ratio");

/////////////////////////////////////////////////
// Attribute-Estimator-Mantaras-Distance
const TStr TAttrEstMantarasDist::DNm("Mantaras-Distance");

/////////////////////////////////////////////////
// Attribute-Estimator-MDL
double TAttrEstMdl::GetAttrQ(
 const int& AttrN, const PTbValSplit& ValSplit,
 const PDmDs& DmDs, const PDmDs& PriorDmDs){
  double SumW=DmDs->GetSumW();
  int CDscs=DmDs->GetCDs()->GetDscs();

  double IGainAttrQ=IGain.GetAttrQ(AttrN, ValSplit, DmDs, PriorDmDs);
  double LnPart=TSpecFunc::LnComb(TFlt::Round(SumW)+CDscs-1, CDscs-1);
  for (int SplitN=0; SplitN<ValSplit->GetSplits(); SplitN++){
    double SSumW=0;
    for (int ValN=0; ValN<ValSplit->GetSplitVals(SplitN); ValN++){
      TTbVal Val=ValSplit->GetSplitVal(SplitN, ValN);
      double ValW=DmDs->GetAVDs(AttrN)->GetValW(Val);
      SSumW+=ValW;
    }
    LnPart-=TSpecFunc::LnComb(TFlt::Round(SSumW+CDscs-1), CDscs-1);
  }
  return IGainAttrQ+LnPart/SumW;
}

const TStr TAttrEstMdl::DNm("MDL");

/////////////////////////////////////////////////
// Attribute-Estimator-G-Statistics
const TStr TAttrEstGStat::DNm("G-Statistics");

/////////////////////////////////////////////////
// Attribute-Estimator-Chi-Square
double TAttrEstChiSquare::GetAttrQ(
 const int& AttrN, const PTbValSplit& ValSplit,
 const PDmDs& DmDs, const PDmDs&){
  double ChiSquare=0;
  for (int SplitN=0; SplitN<ValSplit->GetSplits(); SplitN++){
    PTbValDs CSValDs=GetCSValDs(AttrN, SplitN, ValSplit, DmDs);
    double SPrb=CSValDs->GetSumPrb(DmDs->GetSumW());
    for (int CDsc=0; CDsc<DmDs->GetCDs()->GetDscs(); CDsc++){
      double Frac=SPrb*DmDs->GetCDs()->GetValW(CDsc);
      if (Frac>0){ChiSquare+=TMath::Sqr(Frac-CSValDs->GetValW(CDsc))/Frac;}
    }
  }
  return ChiSquare;
}

const TStr TAttrEstChiSquare::DNm("Chi-Square");

/////////////////////////////////////////////////
// Attribute-Estimator-ORT
double TAttrEstOrt::GetAttrQ(
 const int& AttrN, const PTbValSplit& ValSplit,
 const PDmDs& DmDs, const PDmDs& PriorDmDs){
  IAssert(ValSplit->GetSplits()==2); // **make exception
  PTbValDs CSValDs1=GetCSValDs(AttrN, 0, ValSplit, DmDs);
  PTbValDs CSValDs2=GetCSValDs(AttrN, 1, ValSplit, DmDs);
  double Cos=0; double Norm1=0; double Norm2=0;
  for (int CDsc=0; CDsc<DmDs->GetCDs()->GetDscs(); CDsc++){
    double CSPrb1=PrbEst->GetVPrb(CDsc, CSValDs1, PriorDmDs->GetCDs());
    double CSPrb2=PrbEst->GetVPrb(CDsc, CSValDs2, PriorDmDs->GetCDs());
    Cos+=CSPrb1*CSPrb2; Norm1+=TMath::Sqr(CSPrb1); Norm2+=TMath::Sqr(CSPrb2);
  }
  if ((Norm1==0)||(Norm2==0)){Cos=1;}
  else {Cos=Cos/(sqrt(Norm1)*sqrt(Norm2));}
  return 1-Cos;
}

const TStr TAttrEstOrt::DNm("ORT");

/////////////////////////////////////////////////
// Attribute-Estimator-Gini
double TAttrEstGini::GetAttrQ(
 const int& AttrN, const PTbValSplit& ValSplit,
 const PDmDs& DmDs, const PDmDs& PriorDmDs){
  double Gini=0;
  for (int SplitN=0; SplitN<ValSplit->GetSplits(); SplitN++){
    PTbValDs CSValDs=GetCSValDs(AttrN, SplitN, ValSplit, DmDs);
    double SPrb=CSValDs->GetSumPrb(DmDs->GetSumW());
    double Sum=0;
    for (int CDsc=0; CDsc<DmDs->GetCDs()->GetDscs(); CDsc++){
      Sum+=TMath::Sqr(PrbEst->GetVPrb(CDsc, CSValDs, PriorDmDs->GetCDs()));}
    Gini+=SPrb*Sum;
  }
  for (int CDsc=0; CDsc<DmDs->GetCDs()->GetDscs(); CDsc++){
    double CPrb=PrbEst->GetCPrb(CDsc, DmDs, PriorDmDs);
    Gini-=TMath::Sqr(CPrb);
  }
  return Gini;
}

const TStr TAttrEstGini::DNm("Gini-Index");

/////////////////////////////////////////////////
// Attribute-Estimator-Weight-Of-Evidence
double TAttrEstWgtEvd::GetAttrQ(
 const int& AttrN, const PTbValSplit& ValSplit,
 const PDmDs& DmDs, const PDmDs& PriorDmDs){
  double PriorSumW=PriorDmDs->GetSumW();
  if (PriorSumW==0){return 0;}

  double WgtEvd=0;
  for (int SplitN=0; SplitN<ValSplit->GetSplits(); SplitN++){
    PTbValDs CSValDs=GetCSValDs(AttrN, SplitN, ValSplit, DmDs);
    double SPrb=CSValDs->GetSumPrb(DmDs->GetSumW());
    for (int CDsc=0; CDsc<DmDs->GetCDs()->GetDscs(); CDsc++){
      double OrigCPrb=PrbEst->GetCPrb(CDsc, DmDs, PriorDmDs);
      double CPrb=OrigCPrb;
      if (CPrb==0){CPrb=1/TMath::Sqr(PriorSumW);}
      if (CPrb==1){CPrb=1-(1/TMath::Sqr(PriorSumW));}
      double OddsC=CPrb/(1-CPrb);

      double CSPrb=PrbEst->GetVPrb(CDsc, CSValDs, PriorDmDs->GetCDs());
      if (CSPrb==0){CSPrb=1/TMath::Sqr(PriorSumW);}
      if (CSPrb==1){CSPrb=1-(1/TMath::Sqr(PriorSumW));}
      double OddsCS=CSPrb/(1-CSPrb);

      WgtEvd+=OrigCPrb*SPrb*fabs(log(OddsCS/OddsC));
    }
  }
  return WgtEvd;
}

const TStr TAttrEstWgtEvd::DNm("Weight-Of-Evidence");

/////////////////////////////////////////////////
// Attribute-Estimator-Text-Weight-Of-Evidence
double TAttrEstTextWgtEvd::GetAttrQ(
 const int& AttrN, const PTbValSplit& ValSplit,
 const PDmDs& DmDs, const PDmDs& PriorDmDs){
  double PriorSumW=PriorDmDs->GetSumW();
  if (PriorSumW==0){return 0;}

  double WgtEvd=0;
  PTbValDs CS1ValDs=GetCSValDs(AttrN, 1, ValSplit, DmDs);
  double S1Prb=CS1ValDs->GetSumPrb(DmDs->GetSumW());
  for (int CDsc=0; CDsc<DmDs->GetCDs()->GetDscs(); CDsc++){
    double OrigCPrb=PrbEst->GetCPrb(CDsc, DmDs, PriorDmDs);
    double CPrb=OrigCPrb;
    if (CPrb==0){CPrb=1/TMath::Sqr(PriorSumW);}
    if (CPrb==1){CPrb=1-(1/TMath::Sqr(PriorSumW));}
    double OddsC=CPrb/(1-CPrb);

    double CS1Prb=PrbEst->GetVPrb(CDsc, CS1ValDs, PriorDmDs->GetCDs());
    if (CS1Prb==0){CS1Prb=1/TMath::Sqr(PriorSumW);}
    if (CS1Prb==1){CS1Prb=1-(1/TMath::Sqr(PriorSumW));}
    double OddsCS1=CS1Prb/(1-CS1Prb);

    WgtEvd+=OrigCPrb*S1Prb*fabs(log(OddsCS1/OddsC));
  }
  return WgtEvd;
}

const TStr TAttrEstTextWgtEvd::DNm("Text-Weight-Of-Evidence");

/////////////////////////////////////////////////
// Attribute-Estimator-Odds-Ratio
double TAttrEstOddsRatio::GetAttrQ(
 const int& AttrN, const PTbValSplit& ValSplit,
 const PDmDs& DmDs, const PDmDs& PriorDmDs){
  IAssert(DmDs->GetCDs()->GetDscs()==2); // **make exception
  IAssert(ValSplit->GetSplits()==2); // **make exception

  double PriorSumW=PriorDmDs->GetSumW();
  if (PriorSumW==0){return TFlt::Mn;}

  // split-number-0: false; split-number-1: true
  PTbValDs SC0ValDs=GetSCValDs(0, AttrN, ValSplit, DmDs);
  PTbValDs SC1ValDs=GetSCValDs(1, AttrN, ValSplit, DmDs);
  PTbValDs PriorSValDs=GetSValDs(AttrN, ValSplit, PriorDmDs);
  double S1C0Prb=PrbEst->GetVPrb(TTbVal::PosVal, SC0ValDs, PriorSValDs);
  double S1C1Prb=PrbEst->GetVPrb(TTbVal::PosVal, SC1ValDs, PriorSValDs);

  if (S1C0Prb==0){S1C0Prb=1/TMath::Sqr(PriorSumW);}
  if (S1C0Prb==1){S1C0Prb=1-(1/TMath::Sqr(PriorSumW));}
  double OddsS1C0=S1C0Prb/(1-S1C0Prb);

  if (S1C1Prb==0){S1C1Prb=1/TMath::Sqr(PriorSumW);}
  if (S1C1Prb==1){S1C1Prb=1-(1/TMath::Sqr(PriorSumW));}
  double OddsS1C1=S1C1Prb/(1-S1C1Prb);

  double OddsRatio=log(OddsS1C1/OddsS1C0);
  return OddsRatio;
}

const TStr TAttrEstOddsRatio::DNm("Odds-Ratio");

/////////////////////////////////////////////////
// Attribute-Estimator-Weighted-Odds-Ratio
double TAttrEstWgtOddsRatio::GetAttrQ(
 const int& AttrN, const PTbValSplit& ValSplit,
 const PDmDs& DmDs, const PDmDs& PriorDmDs){
  IAssert(DmDs->GetCDs()->GetDscs()==2); // **make exception
  IAssert(ValSplit->GetSplits()==2); // **make exception

  PTbValDs CSValDs=GetCSValDs(AttrN, 1, ValSplit, DmDs);
  double SPrb=CSValDs->GetSumPrb(DmDs->GetSumW());

  double WgtOddsRatio=SPrb*OddsRatio.GetAttrQ(AttrN, ValSplit, DmDs, PriorDmDs);
  return WgtOddsRatio;
}

const TStr TAttrEstWgtOddsRatio::DNm("Weighted-Odds-Ratio");

/////////////////////////////////////////////////
// Attribute-Estimator-Conditional-Odds-Ratio
double TAttrEstCondOddsRatio::GetAttrQ(
 const int& AttrN, const PTbValSplit& ValSplit,
 const PDmDs& DmDs, const PDmDs& PriorDmDs){
  IAssert(DmDs->GetCDs()->GetDscs()==2); // **make exception
  IAssert(ValSplit->GetSplits()==2); // **make exception

  double PriorSumW=PriorDmDs->GetSumW();
  if (PriorSumW==0){return TFlt::Mn;}

  // split-number-0: false; split-number-1: true
  PTbValDs SC0ValDs=GetSCValDs(0, AttrN, ValSplit, DmDs);
  PTbValDs SC1ValDs=GetSCValDs(1, AttrN, ValSplit, DmDs);
  PTbValDs PriorSValDs=GetSValDs(AttrN, ValSplit, PriorDmDs);
  double S1C0Prb=PrbEst->GetVPrb(TTbVal::PosVal, SC0ValDs, PriorSValDs);
  double S1C1Prb=PrbEst->GetVPrb(TTbVal::PosVal, SC1ValDs, PriorSValDs);

  if (S1C0Prb==0){S1C0Prb=1/TMath::Sqr(PriorSumW);}
  if (S1C0Prb==1){S1C0Prb=1-(1/TMath::Sqr(PriorSumW));}
  double OddsS1C0=S1C0Prb/(1-S1C0Prb);

  if (S1C1Prb==0){S1C1Prb=1/TMath::Sqr(PriorSumW);}
  if (S1C1Prb==1){S1C1Prb=1-(1/TMath::Sqr(PriorSumW));}
  double OddsS1C1=S1C1Prb/(1-S1C1Prb);

  double CondOddsRatio;
  if (S1C0Prb-S1C1Prb>InvTsh){
    CondOddsRatio=InvWgt*log(OddsS1C0/OddsS1C1);
  } else {
    CondOddsRatio=log(OddsS1C1/OddsS1C0);
  }
  return CondOddsRatio;
}

const TStr TAttrEstCondOddsRatio::DNm("Conditional-Odds-Ratio");

PPp TAttrEstCondOddsRatio::GetPp(const TStr& Nm, const TStr& DNm){
  PPp Pp=new TPp(Nm, DNm, ptSet);
  Pp->AddPp(TPrbEst::GetPp(TTypeNm<TPrbEst>(), TPrbEst::DNm));
  Pp->AddPpFlt("InvTsh", "Inversion-Treshold", 0, 1, 0.9);
  Pp->AddPpFlt("InvWgt", "Inversion-Weight", 0, 1, 0.1);
  return Pp;
}

/////////////////////////////////////////////////
// Attribute-Estimator-Log-Probability-Ratio
double TAttrEstLogPRatio::GetAttrQ(
 const int& AttrN, const PTbValSplit& ValSplit,
 const PDmDs& DmDs, const PDmDs& PriorDmDs){
  IAssert(DmDs->GetCDs()->GetDscs()==2); // **make exception
  IAssert(ValSplit->GetSplits()==2); // **make exception

  double PriorSumW=PriorDmDs->GetSumW();
  if (PriorSumW==0){return TFlt::Mn;}

  // split-number-0: false; split-number-1: true
  PTbValDs SC0ValDs=GetSCValDs(0, AttrN, ValSplit, DmDs);
  PTbValDs SC1ValDs=GetSCValDs(1, AttrN, ValSplit, DmDs);
  PTbValDs PriorSValDs=GetSValDs(AttrN, ValSplit, PriorDmDs);
  double S1C0Prb=PrbEst->GetVPrb(TTbVal::PosVal, SC0ValDs, PriorSValDs);
  double S1C1Prb=PrbEst->GetVPrb(TTbVal::PosVal, SC1ValDs, PriorSValDs);

  if (S1C0Prb==0){S1C0Prb=1/TMath::Sqr(PriorSumW);}
  if (S1C1Prb==0){S1C1Prb=1/TMath::Sqr(PriorSumW);}

  double LogPRatio=log(S1C1Prb/S1C0Prb);
  return LogPRatio;
}

const TStr TAttrEstLogPRatio::DNm("Log-Probability-Ratio");

/////////////////////////////////////////////////
// Attribute-Estimator-Exp-Probability-Difference
double TAttrEstExpPDiff::GetAttrQ(
 const int& AttrN, const PTbValSplit& ValSplit,
 const PDmDs& DmDs, const PDmDs& PriorDmDs){
  IAssert(DmDs->GetCDs()->GetDscs()==2); // **make exception
  IAssert(ValSplit->GetSplits()==2); // **make exception

  double SumW=DmDs->GetSumW();
  if (SumW==0){return TFlt::Mn;}

  // split-number-0: false; split-number-1: true
  PTbValDs SC0ValDs=GetSCValDs(0, AttrN, ValSplit, DmDs);
  PTbValDs SC1ValDs=GetSCValDs(1, AttrN, ValSplit, DmDs);
  PTbValDs PriorSValDs=GetSValDs(AttrN, ValSplit, PriorDmDs);
  double S1C0Prb=PrbEst->GetVPrb(TTbVal::PosVal, SC0ValDs, PriorSValDs);
  double S1C1Prb=PrbEst->GetVPrb(TTbVal::PosVal, SC1ValDs, PriorSValDs);

  double ExpPDiff=exp(S1C1Prb-S1C0Prb);
  return ExpPDiff;
}

const TStr TAttrEstExpPDiff::DNm("Exp-Probability-Difference");

/////////////////////////////////////////////////
// Attribute-Estimator-Mutual-Information
double TAttrEstMutInf::GetAttrQ(
 const int& AttrN, const PTbValSplit& ValSplit,
 const PDmDs& DmDs, const PDmDs& PriorDmDs){
  IAssert(ValSplit->GetSplits()==2); // **make exception

  // split-number-0: false; split-number-1: true
  PTbValDs SValDs=GetSValDs(AttrN, ValSplit, DmDs);
  PTbValDs PriorSValDs=GetSValDs(AttrN, ValSplit, PriorDmDs);
  double S1Prb=PrbEst->GetVPrb(TTbVal::PosVal, SValDs, PriorSValDs);
  if (S1Prb==0){return TFlt::Mn;}

  double MutInf=0;
  for (int CDsc=0; CDsc<DmDs->GetCDs()->GetDscs(); CDsc++){
    double CPrb=PrbEst->GetCPrb(CDsc, DmDs, PriorDmDs);
    PTbValDs SCValDs=GetSCValDs(CDsc, AttrN, ValSplit, DmDs);
    double S1CPrb=PrbEst->GetVPrb(TTbVal::PosVal, SCValDs, PriorSValDs);
    if (S1CPrb==0){S1CPrb=1/TMath::Sqr(DmDs->GetSumW());}
    MutInf+=CPrb*log(S1CPrb/S1Prb);
  }
  return MutInf;
}

const TStr TAttrEstMutInf::DNm("Mutual-Information");

/////////////////////////////////////////////////
// Attribute-Estimator-Cross-Entropy
double TAttrEstCrossEnt::GetAttrQ(
 const int& AttrN, const PTbValSplit& ValSplit,
 const PDmDs& DmDs, const PDmDs& PriorDmDs){
  IAssert(ValSplit->GetSplits()==2); // **make exception

  // split-number-0: false; split-number-1: true
  PTbValDs CS1ValDs=GetCSValDs(AttrN, 1, ValSplit, DmDs);
  PTbValDs SValDs=GetSValDs(AttrN, ValSplit, DmDs);
  PTbValDs PriorSValDs=GetSValDs(AttrN, ValSplit, PriorDmDs);
  double S1Prb=PrbEst->GetVPrb(TTbVal::PosVal, SValDs, PriorSValDs);
  if (S1Prb==0){return TFlt::Mn;}

  double CrossEnt=0;
  for (int CDsc=0; CDsc<DmDs->GetCDs()->GetDscs(); CDsc++){
    double CPrb=PrbEst->GetCPrb(CDsc, DmDs, PriorDmDs);
    double CS1Prb=PrbEst->GetVPrb(CDsc, CS1ValDs, PriorDmDs->GetCDs());
    if (CS1Prb>0){Assert(CPrb>0);
      CrossEnt+=CS1Prb*log(CS1Prb/CPrb);}
  }
  CrossEnt*=S1Prb;
  return CrossEnt;
}

const TStr TAttrEstCrossEnt::DNm("Cross-Entropy");

/////////////////////////////////////////////////
// Attribute-Estimator-Term-Frequency
double TAttrEstTermFq::GetAttrQ(
 const int& AttrN, const PTbValSplit& ValSplit,
 const PDmDs& DmDs, const PDmDs&){
  IAssert(ValSplit->GetSplits()==2); // **make exception

  PTbValDs CSValDs=GetCSValDs(AttrN, 1, ValSplit, DmDs);
  return CSValDs->GetSumW();
}

const TStr TAttrEstTermFq::DNm("Term-Frequency");

