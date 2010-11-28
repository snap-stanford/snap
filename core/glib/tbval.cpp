/////////////////////////////////////////////////
// Table-Value
bool TTbVal::operator==(const TTbVal& TbVal) const {
  if (ValTag!=TbVal.ValTag){
    return false;
  } else {
    switch (ValTag){
      case tvtDsc: return Val.Dsc==TbVal.Val.Dsc;
      case tvtFlt: return Val.Flt==TbVal.Val.Flt;
      default: return true;
    }
  }
}

bool TTbVal::operator<(const TTbVal& TbVal) const {
  if (ValTag<TbVal.ValTag){
    return false;
  } else {
    switch (ValTag){
      case tvtDsc: return Val.Dsc<TbVal.Val.Dsc;
      case tvtFlt: return Val.Flt<TbVal.Val.Flt;
      default: return true;
    }
  }
}

TStr TTbVal::GetValTagStr(const int& ValTag){
  switch (ValTag){
    case tvtUndef: return "<Undef>";
    case tvtUnknw: return "<Unknw>";
    case tvtUnimp: return "<Unimp>";
    case tvtUnapp: return "<Unapp>";
    case tvtDsc: return "<Dsc>";
    case tvtFlt: return "<Flt>";
    default: Fail; return TStr();
  }
}

TStr TTbVal::GetStr() const {
  switch (ValTag){
    case tvtUndef: Fail; return TStr();
    case tvtUnknw: return TStr('?');
    case tvtUnimp: return TStr('*');
    case tvtUnapp: return TStr('/');
    case tvtDsc: return TInt::GetStr(Val.Dsc);
    case tvtFlt: return TFlt::GetStr(Val.Flt);
    default: Fail; return TStr();
  }
}

const TTbVal TTbVal::NegVal(false);
const TTbVal TTbVal::PosVal(true);
const int TTbVal::BoolVals=2;

/////////////////////////////////////////////////
// Table-Value-Set
PTbValSet TTbValSet::Load(TSIn& SIn){
  TStr TypeNm(SIn);
  if (TypeNm==TTypeNm<TTbValSetExp>()){return new TTbValSetExp(SIn);}
  else if (TypeNm==TTypeNm<TTbValSetSel>()){return new TTbValSetSel(SIn);}
  else {Fail; return NULL;}
}

/////////////////////////////////////////////////
// Table-Value-Set-Expression
bool TTbValSetExp::In(const TTbVal& Val){
  switch (LOp){
    case loNot: Assert(TbValSetV.Len()==1); return !TbValSetV[0]->In(Val);
    case loAnd:{
      bool Ok=true; int TbValSetN=0;
      while (Ok&&(TbValSetN<TbValSetV.Len())){
        Ok=TbValSetV[TbValSetN++]->In(Val);}
      return Ok;}
    case loOr:{
      bool Ok=false; int TbValSetN=0;
      while (!Ok&&(TbValSetN<TbValSetV.Len())){
        Ok=TbValSetV[TbValSetN++]->In(Val);}
      return Ok;}
    default: Fail; return false;
  }
}

int TTbValSetExp::InN(const TTbVal& Val){
  Assert(LOp==loOr);
  bool Ok=false; int TbValSetN=0;
  while (!Ok&&(TbValSetN<TbValSetV.Len())){Ok=TbValSetV[TbValSetN++]->In(Val);}
  if (TbValSetN<TbValSetV.Len()){return TbValSetN;} else {return -1;}
}

/////////////////////////////////////////////////
// Table-Value-Split
void TTbValSplit::AddBinSplitDscValsPerBits(const int& Dscs, const TB32Set& BSet){
  uint DscBSet=BSet.GetUInt();
  for (int Dsc=0; Dsc<Dscs; Dsc++){
    if (DscBSet%2){AddSplitVal(0, TTbVal(Dsc));}
    else {AddSplitVal(1, TTbVal(Dsc));}
    DscBSet/=2;
  }
}

int TTbValSplit::GetSplitN(const TTbVal& TbVal) const {
  int Splits=GetSplits();
  for (int SplitN=0; SplitN<Splits; SplitN++){
    if (TbValVV[SplitN].IsIn(TbVal)){return SplitN;}
  }
  return -1;
}

TStr TTbValSplit::GetStr() const {
  TChA ChA;
  ChA+="Split"; ChA+=":[";
  int Splits=GetSplits();
  for (int SplitN=0; SplitN<Splits; SplitN++){
    if (SplitN>0){ChA+=' ';}
    ChA+='S'; ChA+=TInt::GetStr(SplitN); ChA+=":[";
    int SplitVals=GetSplitVals(SplitN);
    for (int ValN=0; ValN<SplitVals; ValN++){
      TTbVal TbVal=GetSplitVal(SplitN, ValN);
      if (ValN>0){ChA+=' ';}
      ChA+='V'; ChA+=TbVal.GetStr();
    }
    ChA+="]";
  }
  ChA+="]";
  return ChA;
}

PTbValSplit TTbValSplit::GetBoolValSplit(){
  static PTbValSplit BoolValSplit;
  if (BoolValSplit.Empty()){
    BoolValSplit=PTbValSplit(new TTbValSplit(2));
    BoolValSplit->AddSplitVal(0, TTbVal::NegVal);
    BoolValSplit->AddSplitVal(1, TTbVal::PosVal);
  }
  return BoolValSplit;
}

