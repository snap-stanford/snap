/////////////////////////////////////////////////
// Property-Tag
TStr TPpTag::GetStr(const int& Tag){
  switch (Tag){
    case ptUndef: return "Undef";
    case ptBool: return "Bool";
    case ptInt: return "Int";
    case ptFlt: return "Flt";
    case ptStr: return "Str";
    case ptValV: return "ValV";
    case ptSel: return "Sel";
    case ptSet: return "Set";
    default: Fail; return TStr();
  }
}

TPpTagVal TPpTag::GetTag(const TStr& Str){
  if (Str=="Undef"){return ptUndef;}
  else if (Str=="Bool"){return ptBool;}
  else if (Str=="Int"){return ptInt;}
  else if (Str=="Flt"){return ptFlt;}
  else if (Str=="Str"){return ptStr;}
  else if (Str=="ValV"){return ptValV;}
  else if (Str=="Sel"){return ptSel;}
  else if (Str=="Set"){return ptSet;}
  else {Fail; return ptUndef;}
}

/////////////////////////////////////////////////
// Property-Value
bool TPpVal::operator==(const TPpVal& PpVal) const {
  if (Tag!=PpVal.Tag){return false;}
  switch (Tag){
    case ptUndef: return true;
    case ptBool: return Bool==PpVal.Bool;
    case ptInt: return Int==PpVal.Int;
    case ptFlt: return Flt==PpVal.Flt;
    case ptStr: return Str==PpVal.Str;
    case ptValV: return ValV==PpVal.ValV;
    case ptSel:
    case ptSet:
    default: Fail; return false;
  }
}

TStr TPpVal::GetValStr(const bool& DoAddTag) const {
  TChA ChA;
  if (DoAddTag){ChA+='<'; ChA+=TPpTag::GetStr(Tag); ChA+='>';}
  switch (Tag){
    case ptUndef: break;
    case ptBool: ChA+=TBool::GetStr(Bool); break;
    case ptInt: ChA+=TInt::GetStr(Int); break;
    case ptFlt: ChA+=TFlt::GetStr(Flt); break;
    case ptStr: ChA+='"'; ChA+=Str; ChA+='"'; break;
    case ptValV:{
      ChA+='[';
      for (int ValN=0; ValN<ValV.Len(); ValN++){
        if (ValN>0){ChA+=' ';}
        ChA+=ValV[ValN].GetValStr(DoAddTag);
      }
      ChA+=']'; break;}
    default: Fail;
  }
  return ChA;
}

TPpVal TPpVal::LoadTxt(TILx& Lx){
  TPpTagVal Tag=TPpTag::LoadTxt(Lx);
  if (Tag==ptUndef){
    return TPpVal();
  } else {
    Lx.GetSym(syColon);
    switch (Tag){
      case ptBool: return TPpVal(Lx.GetBool());
      case ptInt: return TPpVal(Lx.GetInt());
      case ptFlt: return TPpVal(Lx.GetFlt());
      case ptStr: return TPpVal(Lx.GetQStr());
      case ptValV:{
        TPpValV ValV;
        Lx.GetSym(syLBracket);
        while (Lx.PeekSym()==syIdStr){ValV.Add(LoadTxt(Lx));}
        Lx.GetSym(syRBracket);
        return TPpVal(ValV);}
      default: Fail; return TPpVal();
    }
  }
}

void TPpVal::SaveTxt(TOLx& Lx) const {
  Lx.PutIdStr(TPpTag::GetStr(Tag));
  if (int(Tag)!=ptUndef){
    Lx.PutSym(syColon);
    switch (Tag){
      case ptBool: Lx.PutBool(Bool); break;
      case ptInt: Lx.PutInt(Int); break;
      case ptFlt: Lx.PutFlt(Flt); break;
      case ptStr: Lx.PutQStr(Str); break;
      case ptValV:{
        Lx.PutSym(syLBracket);
        for (int ValN=0; ValN<ValV.Len(); ValN++){ValV[ValN].SaveTxt(Lx);}
        Lx.PutSym(syRBracket); break;}
      default: Fail;
    }
  }
}

/////////////////////////////////////////////////
// Property
TStr TPp::PpNm="Pp";

int TPp::GetPpN(const TStr& IdNm) const {
  TStr UcIdNm=IdNm.GetUc();
  for (int PpN=0; PpN<PpV.Len(); PpN++){
    if (PpV[PpN]->UcIdNm==UcIdNm){return PpN;}}
  return -1;
}

void TPp::GetChA(const int& Lev, const bool& Brief, TChA& ChA) const {
  for (int LevN=0; LevN<Lev; LevN++){ChA+="|  ";}
  if (!Brief){ChA+=TPpTag::GetStr(Tag);}
  ChA+=" '"; ChA+=IdNm; ChA+="' ("; ChA+=DescNm; ChA+=") = ";
  ChA+=Val.GetValStr(true); ChA+="\r\n";
  if ((int(Tag)==ptSel)||(int(Tag)==ptSet)){
    for (int PpN=0; PpN<PpV.Len(); PpN++){
      PpV[PpN]->GetChA(Lev+1, Brief, ChA);
    }
  }
}

void TPp::PutVal(const TPpVal& _Val){
  Val=_Val;
  if (Val.GetTag()!=ptUndef){
    switch (Tag){
      case ptUndef: Fail; break;
      case ptBool: IAssert(Val.GetTag()==int(Tag)); break;
      case ptInt:
      case ptFlt:
        IAssert(Val.GetTag()==int(Tag));
        if (MnVal.GetTag()!=ptUndef){IAssert(MnVal<=Val);}
        if (MxVal.GetTag()!=ptUndef){IAssert(Val<=MxVal);}
        break;
      case ptStr:
        if (Val.GetTag()==ptInt){Val=CcValV[Val.GetInt()];}
        IAssert(Val.GetTag()==ptStr);
        IAssert((CcValV.Len()==0)||(CcValV.SearchForw(Val)!=-1)); break;
      case ptValV:{
        IAssert(Val.GetTag()==int(Tag));
        Fail; //**TPpValV ValV=Val.GetValV();
        //**for (int ValN=0; ValN<ValV.Len(); ValN++){
          //**IAssert(ValV[ValN].GetTag()==int(ValVTag));}
        break;}
      case ptSel:
        if (Val.GetTag()==ptInt){Val=TPpVal(PpV[Val.GetInt()]->GetIdNm());}
        IAssert((Val.GetTag()==ptStr)&&(IsPp(Val.GetStr()))); break;
      case ptSet: Fail; break;
      default: Fail;
    }
  }
}

PPp TPp::GetPp(const TStr& IdPath) const {
  IAssertSubPp();
  TStr IdNm; TStr RestIdPath;
  IdPath.SplitOnCh(IdNm, '|', RestIdPath);
  PPp Pp=PpV[GetPpN(IdNm)];
  while (!RestIdPath.Empty()){
    RestIdPath.SplitOnCh(IdNm, '|', RestIdPath);
    Pp=Pp->PpV[Pp->GetPpN(IdNm)];
  }
  IAssert(!Pp.Empty()); return Pp;
}

PPp TPp::LoadTxt(TILx& Lx){
  Lx.GetSym(syLBracket);
  TStr IdNm=Lx.GetIdStr();
  TStr DescNm;
  if (Lx.PeekSym()==syLBracket){
    Lx.GetSym(syLBracket); DescNm=Lx.GetQStr(); Lx.GetSym(syRBracket);
  }
  Lx.GetSym(syColon);
  TPpTagVal Tag=TPpTag::LoadTxt(Lx);
  TPpTagVal ValVTag=ptUndef;
  if (Tag==ptValV){
    Lx.GetSym(syLBracket); ValVTag=TPpTag::LoadTxt(Lx); Lx.GetSym(syRBracket);}
  TPpVal Val;
  if (Lx.PeekSym()==syEq){
    Lx.GetSym(syEq); Val=TPpVal::LoadTxt(Lx);}
  TPpVal DfVal;
  if (Lx.PeekSym()==syLBracket){
    Lx.GetSym(syLBracket); DfVal=TPpVal::LoadTxt(Lx); Lx.GetSym(syRBracket);}
  bool IsMnMxValDef=false; TPpVal MnVal, MxVal;
  if (Lx.PeekSym()==syLss){
    Lx.GetSym(syLss);
    IsMnMxValDef=true; MnVal=TPpVal::LoadTxt(Lx); MxVal=TPpVal::LoadTxt(Lx);
    Lx.GetSym(syGtr);
  }
  TPpValV CcValV;
  if (Lx.PeekSym()==syLBrace){
    Lx.GetSym(syLBrace);
    while (Lx.PeekSym()!=syRBrace){CcValV.Add(TPpVal::LoadTxt(Lx));}
    Lx.GetSym(syRBrace);
  }
  TPpV PpV;
  if ((Tag==ptSel)||(Tag==ptSet)){
    while (Lx.IsVar(PpNm)){
      Lx.GetVar(PpNm); PpV.Add(LoadTxt(Lx));}
  }
  Lx.GetSym(syRBracket);
  // construct property
  PPp Pp=PPp(new TPp(IdNm, DescNm, Tag, ValVTag));
  Pp->AddPpV(PpV);
  Pp->PutCcValV(CcValV);
  if (IsMnMxValDef){Pp->PutMnMxVal(MnVal, MxVal);}
  Pp->PutDfVal(DfVal);
  Pp->PutVal(Val);
  // return property
  return Pp;
}

void TPp::SaveTxt(TOLx& Lx, const int& Lev) const {
  Lx.PutSym(syLBracket);
  //Lx.PutIndent(Lev);
  Lx.PutIdStr(IdNm);
  if (!DescNm.Empty()){
    Lx.PutSym(syLBracket); Lx.PutQStr(DescNm); Lx.PutSym(syRBracket);}
  Lx.PutSym(syColon);
  TPpTag::SaveTxt(Lx, Tag);
  if (int(Tag)==ptValV){
    Lx.PutSym(syLBracket); TPpTag::SaveTxt(Lx, ValVTag); Lx.PutSym(syRBracket);}
  if (Val.GetTag()!=ptUndef){
    Lx.PutSym(syEq); Val.SaveTxt(Lx);
  }
  if (DfVal.GetTag()!=ptUndef){
    Lx.PutSym(syLBracket); DfVal.SaveTxt(Lx); Lx.PutSym(syRBracket);}
  if ((MnVal.GetTag()!=ptUndef)||(MxVal.GetTag()!=ptUndef)){
    Lx.PutSym(syLss); MnVal.SaveTxt(Lx); MxVal.SaveTxt(Lx); Lx.PutSym(syGtr);
  }
  if (CcValV.Len()>0){
    Lx.PutSym(syLBrace);
    for (int CcValN=0; CcValN<CcValV.Len(); CcValN++){
      CcValV[CcValN].SaveTxt(Lx);}
    Lx.PutSym(syRBrace);
  }
  if ((int(Tag)==ptSel)||(int(Tag)==ptSet)){
    for (int PpN=0; PpN<PpV.Len(); PpN++){
      Lx.PutVar(PpNm); PpV[PpN]->SaveTxt(Lx, Lev+1);}
  }
  Lx.PutSym(syRBracket);
}

