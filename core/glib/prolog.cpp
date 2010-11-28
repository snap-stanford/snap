/////////////////////////////////////////////////
// Prolog-Value
TStr TPlVal::GetStr(TPlBs* PlBs) const {
  TChA ChA;
  if (IsInt()){ChA+=TInt::GetStr(GetInt());}
  else if (IsFlt()){ChA+=TFlt::GetStr(GetFlt());}
  else if (IsAtom()){ChA+=PlBs->GetAtomQStr(GetAtomId());}
  else if (IsTup()){PPlTup Tup=PlBs->GetTup(GetTupId()); ChA+=Tup->GetStr(PlBs);}
  else {Fail;}
  return ChA;
}

/////////////////////////////////////////////////
// Prolog-Tuple
TStr TPlTup::GetStr(TPlBs* PlBs) const {
  TChA ChA;
  TStr RelFuncStr=PlBs->GetAtomQStr(GetFuncId());
  ChA+=TStr::Fmt("%s(", RelFuncStr.CStr());
  int Vals=GetVals();
  for (int ValN=0; ValN<Vals; ValN++){
    if (ValN>0){ChA+=", ";}
    TStr ValStr=GetVal(ValN).GetStr(PlBs);
    ChA+=ValStr.CStr();
  }
  ChA+=")";
  return ChA;
}

/////////////////////////////////////////////////
// Prolog-Base
const TFSet TPlBs::TupExpect(syStr);
const TFSet TPlBs::ExpExpect(syInt, syFlt, syStr);

TPlVal TPlBs::ParseTup(TILx& Lx, const TFSet& Expect, const PPlBs& PlBs){
  IAssert(TupExpect.In(Lx.Sym));
  int FuncId=PlBs->AtomH.AddKey(Lx.Str);
  TPlValV ValV;
  Lx.GetSym(syLParen);
  Lx.GetSym(ExpExpect);
  TPlVal Val=ParseExp(Lx, TFSet()|syComma|syRParen, PlBs);
  ValV.Add(Val);
  while (Lx.Sym==syComma){
    Lx.GetSym(ExpExpect);
    Val=ParseExp(Lx, TFSet()|syComma|syRParen, PlBs);
    ValV.Add(Val);
  }
  Lx.GetSym(Expect);
  // create and store tuple
  PPlTup Tup=TPlTup::New(FuncId, ValV);
  int TupId=PlBs->AddTup(Tup);
  TPlVal TupVal; TupVal.PutTupId(TupId);
  return TupVal;
}

TPlVal TPlBs::ParseExp(TILx& Lx, const TFSet& Expect, const PPlBs& PlBs){
  IAssert(ExpExpect.In(Lx.Sym));
  TPlVal PlVal;
  if (Lx.Sym==syInt){
    PlVal.PutInt(Lx.Int);
    Lx.GetSym(Expect);
  } else
  if (Lx.Sym==syFlt){
    PlVal.PutFlt(Lx.Flt);
    Lx.GetSym(Expect);
  } else
  if (Lx.Sym==syStr){
    if (Lx.PeekSym()==syLParen){
      Fail;
    } else {
      int AtomId=PlBs->AtomH.AddKey(Lx.Str);
      PlVal.PutAtomId(AtomId);
      Lx.GetSym(Expect);
    }
  } else {
    Fail;
  }
  return PlVal;
}

PPlBs TPlBs::LoadTxtPl(const TStr& FNmWc){
  PPlBs PlBs=TPlBs::New();
  printf("Loading Prolog from '%s'...\n", FNmWc.CStr());
  TFFile FFile(FNmWc); TStr FNm;
  while (FFile.Next(FNm)){
    printf("  ...loading '%s'\n", FNm.CStr());
    // process prolog-file
    PSIn SIn=TFIn::New(FNm);
    TILx Lx(SIn, TFSet()|/*iloList|*/iloCmtAlw|iloCsSens|iloUniStr|iloExcept);
    Lx.GetSym(TFSet(TupExpect)|syEof);
    while (Lx.Sym!=syEof){
      TPlVal TupVal=TPlBs::ParseTup(Lx, TFSet()|syPeriod, PlBs);
      int FuncId=PlBs->TupV[TupVal.GetTupId()]->GetFuncId();
      int Arity=PlBs->TupV[TupVal.GetTupId()]->GetVals();
      PlBs->FuncIdArityPrToTupIdVH.
       AddDat(TIntPr(FuncId, Arity)).Add(TupVal.GetTupId());
      Lx.GetSym(TFSet(TupExpect)|syEof);
    }
    //break;
  }
  printf("Done.\n");
  return PlBs;
}

void TPlBs::SaveTxt(const TStr& FNm){
  TFOut FOut(FNm); FILE* fOut=FOut.GetFileId();
  for (int RelId=0; RelId<GetRels(); RelId++){
    for (int TupN=0; TupN<GetRelTups(RelId); TupN++){
      int TupId=GetRelTupId(RelId, TupN);
      PPlTup Tup=GetTup(TupId);
      TStr TupStr=Tup->GetStr(this);
      fprintf(fOut, "%s.\n", TupStr.CStr());
    }
    if (RelId+1<GetRels()){fprintf(fOut, "\n");}
  }
}

