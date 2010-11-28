/////////////////////////////////////////////////
// Value-Distribution
void TTbValDs::AddVal(const TTbVal& Val, const double& Wgt){
  Assert(!DefP);
  SumW+=Wgt;
  ValTagWV[Val.GetValTag()]+=Wgt;
  switch (Val.GetValTag()){
    case tvtDsc: DscWV[Val.GetDsc()]+=Wgt; break;
    case tvtFlt: FltMom->Add(Val.GetFlt(), Wgt); break;
    default:;
  }
}

void TTbValDs::ExpW(){
  Assert(!DefP);
  int MxWDsc=0;
  for (int Dsc=1; Dsc<DscWV.Len(); Dsc++){
    if (DscWV[Dsc]>DscWV[MxWDsc]){MxWDsc=Dsc;}}
  double MxDscW=DscWV[MxWDsc]; SumW=0;
  {for (int Dsc=0; Dsc<DscWV.Len(); Dsc++){
    DscWV[Dsc]=exp(DscWV[Dsc]-MxDscW); SumW+=DscWV[Dsc];}}
}

double TTbValDs::GetValW(const TTbVal& Val) const {
  Assert(DefP);
  switch (Val.GetValTag()){
    case tvtDsc: return DscWV[Val.GetDsc()];
    case tvtFlt: Fail; return 0;
    default: return ValTagWV[Val.GetValTag()];
  }
}

bool TTbValDs::IsOneVal() const {
  Assert(DefP);
  if (IsEmpty()){return false;}
  if (IsDsc()){
    for (int Dsc=0; Dsc<DscWV.Len(); Dsc++){
      if (DscWV[Dsc]==SumW){return true;}}
    return false;
  } else
  if (IsFlt()){
    return FltMom->GetSDev()==0;
  } else {
    return false;
  }
}

TTbVal TTbValDs::GetMajorVal() const {
  IAssert(DefP&&!IsEmpty());
  if (IsDsc()){
    int MajorDsc=0;
    for (int Dsc=1; Dsc<DscWV.Len(); Dsc++){
      if (DscWV[Dsc]>DscWV[MajorDsc]){MajorDsc=Dsc;}}
    return TTbVal(MajorDsc);
  } else
  if (IsFlt()){
    return TTbVal(FltMom->GetMean());
  } else {
    Fail; return TTbVal();
  }
}

double TTbValDs::GetPrb_RelFq(const TTbVal& Val) const {
  IAssert(DefP);
  if (double(SumW)==0){return 0;}
  return GetValW(Val)/SumW;
}

double TTbValDs::GetPrb_Laplace(const TTbVal& Val) const {
  IAssert(DefP);
  if (SumW+GetDscs()==0){return 0;}
  return (GetValW(Val)+1)/(SumW+GetDscs());
}

double TTbValDs::GetPrb_QuinLaplace(
 const TTbVal& Val, const double& PriorPrb) const {
  IAssert(DefP);
  IAssert((0<=PriorPrb)&&(PriorPrb<=1));
  if (PriorPrb==1){return PriorPrb;}
  double PriorW=1.0/(1-PriorPrb);
  if (SumW+PriorW==0){return 0;}
  return (GetValW(Val)+PriorW*PriorPrb)/(SumW+PriorW);
}

double TTbValDs::GetPrb_MEst(
 const TTbVal& Val, const double& MParam, const double& PriorPrb) const {
  IAssert(DefP);
  IAssert((0<=PriorPrb)&&(PriorPrb<=1));
  if (SumW+MParam==0){return 0;}
  return (GetValW(Val)+MParam*PriorPrb)/(SumW+MParam);
}

TStr TTbValDs::GetStr(const PTbVarType& VarType) const {
  TChA ChA;
  if (IsDsc()){
    ChA+='[';
    for (int ValN=0; ValN<GetDscs(); ValN++){
      if (ValN>0){ChA+=' ';}
      TTbVal Val(ValN);
      ChA+=VarType->GetValStr(Val); ChA+=':';
      ChA+=TFlt::GetStr(GetValW(Val));
    }
    ChA+=']';
  }
  if (IsFlt()){
    ChA+=GetFltMom()->GetStr();
  }
  return ChA;
}

PTbValDs TTbValDs::GetCValDs(
 const int& ClassN, const PDm& Dm, const PExSet& ExSet){
  PTbVarType CVarType=Dm->GetClass(ClassN)->GetVarType();
  PTbValDs ValDs=TTbValDs::New(CVarType->GetDscs());

  int ExP=ExSet->FFirstExP();
  while (ExSet->FNextExP(ExP)){
    int ExId=ExSet->GetExId(ExP); double ExWgt=ExSet->GetExWgt(ExP);
    ValDs->AddVal(Dm->GetClassVal(ExId, ClassN), ExWgt);
  }
  ValDs->Def();
  return ValDs;
}

PTbValDs TTbValDs::GetBoolValDs(const double& NegValW, const double& PosValW){
  PTbValDs ValDs=TTbValDs::New(2);
  if ((NegValW==-1)||(PosValW==-1)){
    double NegValPrb=(NegValW==-1) ? 1-PosValW : NegValW;
    double PosValPrb=(PosValW==-1) ? 1-NegValW : PosValW;
    IAssert((NegValPrb<=1)&&(PosValPrb<=1));
    ValDs->AddVal(TTbVal::NegVal, NegValPrb);
    ValDs->AddVal(TTbVal::PosVal, PosValPrb);
  } else {
    ValDs->AddVal(TTbVal::NegVal, NegValW);
    ValDs->AddVal(TTbVal::PosVal, PosValW);
  }
  ValDs->Def();
  return ValDs;
}

/////////////////////////////////////////////////
// Domain-Distribution
PDmDs TDmDs::Load(TSIn& SIn){
  TStr TypeNm(SIn);
  if (TypeNm==TTypeNm<TGDmDs>()){return new TGDmDs(SIn);}
  else {Fail; return NULL;}
}

/////////////////////////////////////////////////
// General-Domain-Distribution
void TGDmDs::AddVal(
 const TTbVal& CVal, const int& AttrN, const TTbVal& AVal, const double& Wgt){
  IAssert(CVal.IsDsc());
  SumW+=Wgt;
  CDs->AddVal(CVal, Wgt);
  AVDsV[AttrN]->AddVal(AVal, Wgt);
  CAVDsVV.At(CVal.GetDsc(), AttrN)->AddVal(AVal, Wgt);
}

TGDmDs::TGDmDs(const PDm& Dm, const int& ClassN, const PExSet& ExSet):
  TDmDs(), SumW(), CDs(), AVDsV(), CAVDsVV(){
  //Assert(TDmCheck(Dm).IsClassDsc(ClassN));
  int CDscs=Dm->GetClass(ClassN)->GetVarType()->GetDscs();

  SumW=0;
  CDs=TTbValDs::New(CDscs);
  AVDsV.Gen(Dm->GetAttrs());
  CAVDsVV.Gen(CDscs, Dm->GetAttrs());
  for (int AttrN=0; AttrN<Dm->GetAttrs(); AttrN++){
    int ADscs=Dm->GetAttr(AttrN)->GetVarType()->GetDscs();
    AVDsV[AttrN]=TTbValDs::New(ADscs);
    for (int CDsc=0; CDsc<CDscs; CDsc++){
      CAVDsVV.At(CDsc, AttrN)=TTbValDs::New(ADscs);}
  }

  int ExP=ExSet->FFirstExP();
  while (ExSet->FNextExP(ExP)){
    int ExId=ExSet->GetExId(ExP); double ExWgt=ExSet->GetExWgt(ExP);
    TTbVal CVal=Dm->GetClassVal(ExId, ClassN);
    for (int AttrN=0; AttrN<Dm->GetAttrs(); AttrN++){
      AddVal(CVal, AttrN, Dm->GetAttrVal(ExId, AttrN), ExWgt);}
  }

  CDs->Def();
  {for (int AttrN=0; AttrN<Dm->GetAttrs(); AttrN++){
    AVDsV[AttrN]->Def();
    for (int CDsc=0; CDsc<CDscs; CDsc++){CAVDsVV.At(CDsc, AttrN)->Def();}
  }}
}

