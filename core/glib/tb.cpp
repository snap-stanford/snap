/////////////////////////////////////////////////
// Table-Variable-Data-Type
TStr TTbVarType::GetStr() const {
  TChA ChA;
  if (IsDsc()){
    ChA+="Discrete";
    ChA+=":[";
    if (IsDscOrd()){ChA+="Ordered";}
    ChA+=" Vals:"+TInt::GetStr(GetDscs());
    if (IsDscInt()){
      ChA+=" Integer";
      ChA+=":[";
      ChA+=" Mn:"+TInt::GetStr(GetMnIntDsc());
      ChA+=" Mx:"+TInt::GetStr(GetMxIntDsc());
      ChA+="]";
    }
    if (IsDscSym()){
      ChA+=" Symbolic";
      ChA+=":[";
      for (int Dsc=0; Dsc<GetDscs(); Dsc++){
        ChA+=" "; ChA+=GetDscStr(Dsc);
      }
      ChA+="]";
    }
    ChA+="]";
  }
  if (IsFlt()){
    ChA+="Continuos";
    ChA+=":[";
    ChA+="Range:"+TFlt::GetStr(GetFltRange());
    ChA+=" Mn:"+TFlt::GetStr(GetMnFlt());
    ChA+=" Mx:"+TFlt::GetStr(GetMxFlt());
    ChA+="]";
  }
  return ChA;
}

bool TTbVarType::IsValOk(const TTbVal& Val){
  TTbValTag ValTag=Val.GetValTag();
  if (!AlwVTSet.In(ValTag)){return false;}
  if (ValTag==tvtDsc){
    int Dsc=Val.GetDsc();
    return (0<=Dsc)&&(Dsc<GetDscs());
  } else
  if (ValTag==tvtFlt){
    double Flt=Val.GetFlt();
    if (DefP || FixVTSet.In(tvtFlt)){
      return (double(MnFlt)<=Flt)&&(Flt<=double(MxFlt));}
    else {
      MnFlt=TFlt::GetMn(MnFlt, Flt); MxFlt=TFlt::GetMx(MxFlt, Flt);
      return true;
    }
  } else {
    return true;
  }
}

TTbVal TTbVarType::GetDscVal(const TStr& Str){
  Assert(AlwVTSet.In(tvtDsc)&&!DscIntP);
  int KeyId=NmToDscH.GetKeyId(Str);
  if (KeyId==-1){
    Assert(!DefP && !FixVTSet.In(tvtDsc));
    return TTbVal(NmToDscH.AddDat(Str, TInt(DscToNmV.Add(Str))));}
  else {
    return TTbVal(NmToDscH[KeyId]);
  }
}

TTbVal TTbVarType::GetDscVal(const int& Dsc){
  Assert(AlwVTSet.In(tvtDsc)&&(0<=Dsc)&&(Dsc<GetDscs()));
  return TTbVal(Dsc);
}

TTbVal TTbVarType::GetIntDscVal(const int& Int){
  Assert(AlwVTSet.In(tvtDsc)&&DscIntP&&(MnIntDsc<=Int)&&(Int<=MxIntDsc));
  return TTbVal(Int-MnIntDsc);
}

TTbVal TTbVarType::GetFltVal(const double& Flt){
  Assert(AlwVTSet.In(tvtFlt));
  if (DefP || FixVTSet.In(tvtFlt)){
    Assert((double(MnFlt)<=Flt)&&(Flt<=double(MxFlt)));
  } else {
    MnFlt=TFlt::GetMn(MnFlt, Flt); MxFlt=TFlt::GetMx(MxFlt, Flt);
  }
  return TTbVal(Flt);
}

TTbVal TTbVarType::GetVal(const TTbVal& Val){
  Assert(AlwVTSet.In(Val.GetValTag()));
  switch (Val.GetValTag()){
    case tvtUnknw: case tvtUnimp: case tvtUnapp: return Val;
    case tvtDsc: return GetDscVal(Val.GetDsc());
    case tvtFlt: return GetFltVal(Val.GetFlt());
    default: Fail; return TTbVal();
  }
}

double TTbVarType::GetNrmFlt(const TTbVal& Val){
  Assert(DefP && IsValOk(Val));
  switch (Val.GetValTag()){
    case tvtDsc:
      Assert(DscOrdP);
      if (GetDscs()>1){return double(Val.GetDsc())/double(GetDscs()-1);}
      else {return 0;}
    case tvtFlt:
      Assert(GetFltRange()>0);
      return (Val.GetFlt()-MnFlt)/GetFltRange();
    default: Fail; return 0;
  }
}

TStr TTbVarType::GetValStr(const TTbVal& Val){
  Assert(IsValOk(Val));
  if (Val.GetValTag()==tvtDsc){
    if (DscIntP){return TInt::GetStr(Val.GetDsc()+MnIntDsc);}
    else {return DscToNmV[Val.GetDsc()];}
  } else {
    return Val.GetStr();
  }
}

PTbVarType TTbVarType::GetDscBoolVarType(){
  static PTbVarType BoolVarType=NULL;
  if (BoolVarType==NULL){
    BoolVarType=PTbVarType(new TTbVarType());
    BoolVarType->SetDscFix(true);
    BoolVarType->AddDscNm(TBool::GetStr(false));
    BoolVarType->AddDscNm(TBool::GetStr(true));
    BoolVarType->SetAlwVTSet(TB32Set(tvtDsc));
    BoolVarType->Def();
  }
  return BoolVarType;
}

PTbVarType TTbVarType::GetDscIntVarType(const int& MnIntDsc, const int& MxIntDsc){
  static const int IntVarTypes=10;
  static TTbVarTypeV DscIntVarTypeV;
  if (DscIntVarTypeV.Len()!=IntVarTypes){
    for (int IntVarTypeN=0; IntVarTypeN<IntVarTypes; IntVarTypeN++){
      PTbVarType VarType=new TTbVarType();
      VarType->SetDscFix(0, IntVarTypeN);
      VarType->SetAlwVTSet(TB32Set(tvtDsc));
      VarType->Def();
      DscIntVarTypeV.Add(VarType);
    }
  }
  if ((MnIntDsc==0)&&(0<=MxIntDsc)&&(MxIntDsc<DscIntVarTypeV.Len())){
    return DscIntVarTypeV[MxIntDsc];
  } else {
    PTbVarType VarType=new TTbVarType();
    VarType->SetDscFix(MnIntDsc, MxIntDsc);
    VarType->SetAlwVTSet(TB32Set(tvtDsc));
    VarType->Def();
    return VarType;
  }
}

PTbVarType TTbVarType::GetDscNmVVarType(const TStrV& NmV, const bool& DscOrdP){
  PTbVarType VarType=new TTbVarType();
  VarType->SetDscFix(DscOrdP);
  for (int NmN=0; NmN<NmV.Len(); NmN++){VarType->AddDscNm(NmV[NmN]);}
  VarType->SetAlwVTSet(TB32Set(tvtDsc));
  VarType->Def();
  return VarType;
}

PTbVarType TTbVarType::GetFltVarType(const double& MnFlt, const double& MxFlt){
  static PTbVarType GFltVarType=NULL;
  if (GFltVarType.Empty()){
    GFltVarType=PTbVarType(new TTbVarType());
    GFltVarType->SetFltFix(TFlt::Mn, TFlt::Mx);
    GFltVarType->SetAlwVTSet(TB32Set(tvtFlt));
    GFltVarType->Def();
  }
  if ((MnFlt==TFlt::Mn)&&(MxFlt==TFlt::Mx)){
    return GFltVarType;
  } else {
    PTbVarType VarType=new TTbVarType();
    VarType->SetFltFix(MnFlt, MxFlt);
    VarType->SetAlwVTSet(TB32Set(tvtFlt));
    VarType->Def();
    return VarType;
  }
}

TTbSufixVarCat TTbVarType::GetSufixVarCat(const TStr& VarNm){
  TStr UcVarNm=VarNm.GetUc();
  if (UcVarNm.IsSuffix(":I")){return tsvcIgnore;}
  else if (UcVarNm.IsSuffix(":L")){return tsvcLabel;}
  else if (UcVarNm.IsSuffix(":C")){return tsvcClass;}
  else if (UcVarNm.IsSuffix(":A")){return tsvcAttr;}
  else if (UcVarNm=="CLASS"){return tsvcClass;}
  else {return tsvcUndef;}
}

bool TTbVarType::IsSuffixVarNm(const PTb& Tb){
  for (int VarN=0; VarN<Tb->GetVars(); VarN++){
    TStr VarNm=Tb->GetVar(VarN)->GetNm();
    if (GetSufixVarCat(VarNm)!=tsvcUndef){return true;}
  }
  return false;
}

/////////////////////////////////////////////////
// Table
PTb TTb::Load(TSIn& SIn){
  TStr TypeNm(SIn);
  if (TypeNm==TTypeNm<TGTb>()){return new TGTb(SIn);}
  else {Fail; return NULL;}
}

void TTb::AddTb(const PTb& Tb){
  Assert(GetVars()==Tb->GetVars());
  for (int VarN=0; VarN<GetVars(); VarN++){
    Assert(*GetVar(VarN)==*(Tb->GetVar(VarN)));
  }
  for (int SrcTupN=0; SrcTupN<Tb->GetTups(); SrcTupN++){
    int DstTupN=AddTup(Tb->GetTupNm(SrcTupN));
    for (int VarN=0; VarN<Tb->GetVars(); VarN++){
      PutVal(DstTupN, VarN, Tb->GetVal(SrcTupN, VarN));}
  }
}

PTb TTb::GetSubTb(const TIntV& TupNV, const TIntV& VarNV, const PTb& Tb){
  if ((TupNV.Len()==0)||(VarNV.Len()==0)){return NULL;}
  for (int VarNN=0; VarNN<VarNV.Len(); VarNN++){
    Tb->AddVar(GetVar(VarNV[VarNN]));}
  for (int TupNN=0; TupNN<TupNV.Len(); TupNN++){
    int TupN=TupNV[TupNN]; int NewTupN=Tb->AddTup(GetTupNm(TupN));
    for (int VarNN=0; VarNN<VarNV.Len(); VarNN++){
      Tb->PutVal(NewTupN, VarNN, GetVal(TupN, VarNV[VarNN]));}
  }
  Tb->DefVarTypes();
  return Tb;
}

PTb TTb::LoadFile(const TStr& FNm, const TStr& FType, const TStr& MemRep){
  // memory representation
  PTb Tb;
  if (MemRep==""){Tb=PTb(new TGTb());}
  else if (MemRep=="General"){Tb=PTb(new TGTb());}
  else {Fail; return NULL;}
  // file type
  if (FType==""){
    // infer file-type from file-extension
    TStr FExt=FNm.GetFExt().GetUc();
    if (FExt==".TXT"){LoadTxtSpc(FNm, Tb);}
    else if (FExt==".TAB"){LoadTxtTab(FNm, Tb);}
    else if (FExt==".CSV"){LoadTxtCsv(FNm, Tb);}
    else {Fail;}
  } else {
    // use file-type info
    if (FType=="Txt"){LoadTxtSpc(FNm, Tb);}
    else if (FType=="TxtTab"){LoadTxtTab(FNm, Tb);}
    else if (FType=="TxtCsv"){LoadTxtCsv(FNm, Tb);}
    else {Fail;}
  }
  return Tb;
}

PTb TTb::LoadTxtSpc(const TStr& FNm, const PTb& Tb){
  // open file
  PSIn SIn=TFIn::New(FNm);
  TILx Lx(SIn, TFSet()|iloRetEoln|iloSigNum|iloUniStr);

  // read variable names
  Lx.GetStr();
  bool IsTupNm=(Lx.UcStr=="NAME")||(Lx.UcStr=="ID");
  if (IsTupNm){Lx.GetStr();}
  do {
    PTbVarType VarType=PTbVarType(new TTbVarType());
    PTbVar Var=PTbVar(new TGTbVar(Lx.Str, VarType));
    Tb->AddVar(Var);
  } while (Lx.GetSym(syStr, syEoln, syEof)==syStr);

  // read tuples
  TFSet ValSymSet=TFSet()|syQuestion|syAsterisk|sySlash|syFlt|syStr;
  if (Lx.Sym!=syEof){
    Lx.GetSym(TFSet(ValSymSet)|syEof);}
  while (Lx.Sym!=syEof){
    int TupN;
    if (IsTupNm){TupN=Tb->AddTup(Lx.GetSymStr()); Lx.GetSym(ValSymSet);}
    else {TupN=Tb->AddTup();}
    for (int VarN=0; VarN<Tb->GetVars(); VarN++){
      if (VarN>0){Lx.GetSym(ValSymSet);}
      switch (Lx.Sym){
        case syQuestion: Tb->PutVal(TupN, VarN, TTbVal(tvtUnknw)); break;
        case syAsterisk: Tb->PutVal(TupN, VarN, TTbVal(tvtUnimp)); break;
        case sySlash: Tb->PutVal(TupN, VarN, TTbVal(tvtUnapp)); break;
        case syStr: Tb->PutDsc(TupN, VarN, Lx.Str); break;
        case syFlt: Tb->PutFlt(TupN, VarN, Lx.Flt); break;
        default: Fail;
      }
    }
    Lx.GetSym(syEoln, syEof);
    while (Lx.Sym==syEoln){Lx.GetSym(TFSet(ValSymSet)|syEoln|syEof);}
  }
  Tb->DefVarTypes();
  return Tb;
}

PTb TTb::LoadTxtTab(const TStr& FNm, const PTb& Tb){
  // open file
  PSIn SIn=TFIn::New(FNm);
  TILx Lx(SIn, TFSet()|iloTabSep);

  // read variable names
  Lx.GetStr();
  bool IsTupNm=(Lx.UcStr=="NAME")||(Lx.UcStr=="ID");
  if (IsTupNm){Lx.GetSym(syTab); Lx.GetStr();}
  do {
    PTbVarType VarType=PTbVarType(new TTbVarType());
    PTbVar Var=PTbVar(new TGTbVar(Lx.Str, VarType));
    Tb->AddVar(Var);
    if (Lx.GetSym(syTab, syEoln, syEof)==syTab){
      Lx.GetSym(syStr);}
  } while (Lx.Sym==syStr);

  // read tuples
  if (Lx.Sym!=syEof){
    Lx.GetSym(syStr, syEof);}
  while (Lx.Sym!=syEof){
    int TupN;
    if (IsTupNm){
      TupN=Tb->AddTup(Lx.GetSymStr());
      Lx.GetSym(syTab); Lx.GetStr();}
    else {TupN=Tb->AddTup();}
    for (int VarN=0; VarN<Tb->GetVars(); VarN++){
      if (VarN>0){Lx.GetSym(syTab); Lx.GetStr();}
      if (Lx.Str=='?'){Tb->PutVal(TupN, VarN, TTbVal(tvtUnknw));}
      else if (Lx.Str=='*'){Tb->PutVal(TupN, VarN, TTbVal(tvtUnimp));}
      else if (Lx.Str=='/'){Tb->PutVal(TupN, VarN, TTbVal(tvtUnapp));}
      else {
        double Flt;
        if (TStr(Lx.Str).IsFlt(Flt)){
          Tb->PutFlt(TupN, VarN, Flt);
        } else {
          Tb->PutDsc(TupN, VarN, Lx.Str);
        }
      }
    }
    //printf("Tups: %d\r", Tb->GetTups());
    Lx.GetSym(syEoln, syEof);
    while (Lx.Sym==syEoln){Lx.GetSym(syStr, syEoln, syEof);}
  }
  Tb->DefVarTypes();
  return Tb;
}

PTb TTb::LoadTxtCsv(const TStr& FNm, const PTb& Tb){
  // open file
  PSIn SIn=TFIn::New(FNm);
  TILx Lx(SIn, TFSet()|iloRetEoln|iloSigNum|iloUniStr);

  // read variable names
  Lx.GetStr();
  bool IsTupNm=(Lx.UcStr=="NAME")||(Lx.UcStr=="ID");
  if (IsTupNm){Lx.GetSym(syComma); Lx.GetStr();}
  do {
    PTbVarType VarType=PTbVarType(new TTbVarType());
    PTbVar Var=PTbVar(new TGTbVar(Lx.Str, VarType));
    Tb->AddVar(Var);
    if (Lx.GetSym(syComma, syEoln, syEof)==syComma){
      Lx.GetSym(syStr);}
  } while (Lx.Sym==syStr);

  // read tuples
  TFSet ValSymSet=TFSet()|syQuestion|syAsterisk|sySlash|syFlt|syStr;
  if (Lx.Sym!=syEof){
    Lx.GetSym(TFSet(ValSymSet)|syEof);}
  while (Lx.Sym!=syEof){
    int TupN;
    if (IsTupNm){
      TupN=Tb->AddTup(Lx.GetSymStr());
      Lx.GetSym(syComma); Lx.GetSym(ValSymSet);}
    else {TupN=Tb->AddTup();}
    for (int VarN=0; VarN<Tb->GetVars(); VarN++){
      if (VarN>0){Lx.GetSym(syComma); Lx.GetSym(ValSymSet);}
      switch (Lx.Sym){
        case syQuestion: Tb->PutVal(TupN, VarN, TTbVal(tvtUnknw)); break;
        case syAsterisk: Tb->PutVal(TupN, VarN, TTbVal(tvtUnimp)); break;
        case sySlash: Tb->PutVal(TupN, VarN, TTbVal(tvtUnapp)); break;
        case syStr: Tb->PutDsc(TupN, VarN, Lx.Str); break;
        case syFlt: Tb->PutFlt(TupN, VarN, Lx.Flt); break;
        default: Fail;
      }
    }
    Lx.GetSym(syEoln, syEof);
    while (Lx.Sym==syEoln){Lx.GetSym(TFSet(ValSymSet)|syEoln|syEof);}
  }
  Tb->DefVarTypes();
  return Tb;
}

void TTb::SaveTxt(const TStr& FNm, const bool& SaveTupNm){
  TOLx Lx(PSOut(new TFOut(FNm)), TFSet()|oloFrcEoln|oloSigNum|oloUniStr|oloTabSep);
  if (SaveTupNm){Lx.PutStr("Name");}
  for (int VarN=0; VarN<GetVars(); VarN++){
    Lx.PutStr(GetVar(VarN)->GetNm());}
  Lx.PutLn();

  for (int TupN=0; TupN<GetTups(); TupN++){
    if (SaveTupNm){Lx.PutStr(GetTupNm(TupN));}
    for (int VarN=0; VarN<GetVars(); VarN++){
      TTbVal Val=GetVal(TupN, VarN);
      switch (Val.GetValTag()){
        case tvtUnknw: Lx.PutSym(syQuestion); break;
        case tvtUnimp: Lx.PutSym(syAsterisk); break;
        case tvtUnapp: Lx.PutSym(sySlash); break;
        case tvtDsc: Lx.PutStr(GetVar(VarN)->GetVarType()->GetValStr(Val)); break;
        case tvtFlt: Lx.PutFlt(Val.GetFlt()); break;
        default: Fail;
      }
    }
    Lx.PutLn();
  }
}

void TTb::SaveAssis(const TStr& FNm){
  TStr DoFNm=FNm.GetFPath()+"AsDo"+FNm.GetFMid().GetSubStr(0, 3)+".Dat";
  TStr DaFNm=FNm.GetFPath()+"AsDa"+FNm.GetFMid().GetSubStr(0, 3)+".Dat";

  TOLx DoLx(PSOut(new TFOut(DoFNm)), TFSet()|oloFrcEoln|oloSigNum|oloUniStr);
  int Dscs=GetVar(0)->GetVarType()->GetDscs(); Assert(Dscs>0);
  DoLx.PutInt(Dscs); DoLx.PutDosLn();
  for (int DscN=0; DscN<Dscs; DscN++){
    TTbVal Val=GetVar(0)->GetVarType()->GetVal(DscN);
    DoLx.PutStr(GetVar(0)->GetVarType()->GetValStr(Val)); DoLx.PutDosLn();
  }
  DoLx.PutInt(TInt(GetVars()-1)); DoLx.PutDosLn();
  for (int VarN=1; VarN<GetVars(); VarN++){
    DoLx.PutStr(GetVar(VarN)->GetNm()); DoLx.PutDosLn();
    int Dscs=GetVar(VarN)->GetVarType()->GetDscs();
    if (Dscs>0){
      DoLx.PutInt(Dscs); DoLx.PutDosLn();
      for (int DscN=0; DscN<Dscs; DscN++){
        TTbVal Val=GetVar(VarN)->GetVarType()->GetVal(DscN);
        DoLx.PutStr(GetVar(VarN)->GetVarType()->GetValStr(DscN)); DoLx.PutDosLn();}
    } else {
      DoLx.PutInt(TInt(0)); DoLx.PutInt(TInt(100)); DoLx.PutDosLn();
    }
  }

  TOLx DaLx(PSOut(new TFOut(DaFNm)), TFSet()|oloFrcEoln|oloSigNum|oloUniStr);
  for (int TupN=0; TupN<GetTups(); TupN++){
    for (int VarN=0; VarN<GetVars(); VarN++){
      TTbVal Val=GetVal(TupN, VarN);
      switch (Val.GetValTag()){
        case tvtUnknw: DaLx.PutSym(syQuestion); break;
        case tvtUnimp: DaLx.PutSym(syAsterisk); break;
        case tvtUnapp: DaLx.PutSym(syAsterisk); break;
        case tvtDsc: DaLx.PutInt(TInt(1+Val.GetDsc())); break;
        case tvtFlt: DaLx.PutFlt(Val.GetFlt()); break;
        default: Fail;
      }
    }
    DaLx.PutDosLn();
  }
}

/////////////////////////////////////////////////
// Generic-Table
int TGTb::AddVar(const PTbVar& TbVar){
  Assert(GetTups()==0);
  // generate unique name
  TStr UniqNm=TbVar->GetNm();
  if (UniqNm.Empty()){UniqNm=TStr("A")+TInt::GetStr(VarV.Len());}
  if (VarNmToNH.IsKey(UniqNm)){
    int CopyN=2;
    while (VarNmToNH.IsKey(UniqNm+" ("+TInt::GetStr(CopyN)+")")){CopyN++;}
    UniqNm=UniqNm+" ("+TInt::GetStr(CopyN)+")";
  }
  Assert(!VarNmToNH.IsKey(UniqNm));
  int VarN=VarV.Add(PTbVar(new TGTbVar(UniqNm, TbVar->GetVarType())));
  VarNmToNH.AddDat(UniqNm, VarN);
  return VarN;
}

void TGTb::DefVarTypes(){
  for (int VarN=0; VarN<GetVars(); VarN++){VarV[VarN]->DefVarType();}
}

int TGTb::AddTup(const TStr& Nm){
  Assert(GetVars()>0);
  TStr UniqNm=Nm;
  if (UniqNm.Empty()){UniqNm=TStr("E")+TInt::GetStr(TupV.Len());}
  if (TupNmToNH.IsKey(UniqNm)){
    int CopyN=2;
    while (TupNmToNH.IsKey(UniqNm+" ("+TInt::GetStr(CopyN)+")")){CopyN++;}
    UniqNm=UniqNm+" ("+TInt::GetStr(CopyN)+")";
  }
  Assert(!TupNmToNH.IsKey(UniqNm));
  int TupN=TupV.Add(PTbTup(new TGTbTup(UniqNm, GetVars())));
  TupNmToNH.AddDat(UniqNm, TupN);
  return TupN;
}

TStr TGTb::GetValStr(const int& TupN, const int& VarN){
  return VarV[VarN]->GetVarType()->GetValStr(TupV[TupN]->GetVal(VarN));
}

/////////////////////////////////////////////////
// Binary-Table
int TBTb::AddVar(const PTbVar& TbVar){
  Assert(GetTups()==0);
  // generate unique name
  TStr UniqNm=TbVar->GetNm();
  if (UniqNm.Empty()){UniqNm=TStr("A")+TInt::GetStr(VarNmToNH.Len());}
  if (VarNmToNH.IsKey(UniqNm)){
    int CopyN=2;
    while (VarNmToNH.IsKey(UniqNm+" ("+TInt::GetStr(CopyN)+")")){CopyN++;}
    UniqNm=UniqNm+" ("+TInt::GetStr(CopyN)+")";
  }
  Assert(!VarNmToNH.IsKey(UniqNm));
  // check binary type
  IAssert(TbVar->GetVarType()==TTbVarType::GetDscBoolVarType());
  // create variable
  int VarN=VarNmToNH.Len();
  VarNmToNH.AddDat(UniqNm, VarN);
  return VarN;
}

int TBTb::AddTup(const TStr& Nm){
  Assert(GetVars()>0);
  TStr UniqNm=Nm;
  if (UniqNm.Empty()){UniqNm=TStr("E")+TInt::GetStr(TupV.Len());}
  if (TupNmToNH.IsKey(UniqNm)){
    int CopyN=2;
    while (TupNmToNH.IsKey(UniqNm+" ("+TInt::GetStr(CopyN)+")")){CopyN++;}
    UniqNm=UniqNm+" ("+TInt::GetStr(CopyN)+")";
  }
  Assert(!TupNmToNH.IsKey(UniqNm));
  int TupN=0;//TupV.Add(PTbTup(new TBTbTup(UniqNm, GetVars())));
  TupNmToNH.AddDat(UniqNm, TupN);
  return TupN;
}

TStr TBTb::GetValStr(const int& TupN, const int& VarN){
  return VarV[VarN]->GetVarType()->GetValStr(TupV[TupN]->GetVal(VarN));
}

