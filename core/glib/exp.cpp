/////////////////////////////////////////////////
// Expression-Value
PExpVal TExpVal::UndefExpVal=NULL;
PExpVal TExpVal::ZeroExpVal=NULL;

bool TExpVal::operator==(const TExpVal& ExpVal) const {
  if (ValType!=ExpVal.ValType){return false;}
  switch (ValType){
    case evtUndef: return true;
    case evtFlt: return Flt==ExpVal.Flt;
    case evtStr: return Str==ExpVal.Str;
    case evtVec:{
      if (ValV.Len()!=ExpVal.ValV.Len()){return false;}
      for (int VecValN=0; VecValN<ExpVal.ValV.Len(); VecValN++){
        if (*ValV[VecValN]!=*ExpVal.ValV[VecValN]){return false;}}
      return true;}
    case evtLst:{
      if (ValL.Len()!=ExpVal.ValL.Len()){return false;}
      TExpValLN ValLN=ValL.First();
      TExpValLN ExpValLN=ExpVal.ValL.First();
      while (ValLN!=NULL){
        if (*ValLN->GetVal()!=*ExpValLN->GetVal()){return false;}
        ValLN=ValLN->Next();
        ExpValLN=ExpValLN->Next();
      }
      return true;}
    default: Fail; return false;
  }
}

bool TExpVal::operator<(const TExpVal& ExpVal) const {
  if (ValType!=ExpVal.ValType){
    return ValType<ExpVal.ValType;}
  switch (ValType){
    case evtUndef: return false;
    case evtFlt: return Flt<ExpVal.Flt;
    case evtStr: return Str<ExpVal.Str;
    case evtVec:{
      int VecValN=0;
      while ((VecValN<ValV.Len())&&(VecValN<ExpVal.ValV.Len())){
        if (*ValV[VecValN]<*ExpVal.ValV[VecValN]){return true;}
        else if (*ValV[VecValN]==*ExpVal.ValV[VecValN]){VecValN++;}
        else {return false;}
      }
      return ValV.Len()<ExpVal.ValV.Len();}
    case evtLst:{
      if (ValL.Len()!=ExpVal.ValL.Len()){return false;}
      TExpValLN ValLN=ValL.First();
      TExpValLN ExpValLN=ExpVal.ValL.First();
      while ((ValLN!=NULL)&&(ExpValLN!=NULL)){
        if (*ValLN->GetVal()<*ExpValLN->GetVal()){
          return true;}
        else if (*ValLN->GetVal()==*ExpValLN->GetVal()){
          ValLN=ValLN->Next(); ExpValLN=ExpValLN->Next();}
        else {return false;}
      }
      return ValLN==NULL;}
    default: Fail; return false;
  }
}

int TExpVal::GetFltValAsInt(const bool& ThrowExceptP) const {
  double Flt=GetFltVal();
  if ((Flt<double(TInt::Mn))&&(double(TInt::Mx)<Flt)){
    if (ThrowExceptP){TExcept::Throw("Float too big for integer.");}
    else {Flt=0;}
  }
  return int(Flt);
}

void TExpVal::SaveTxt(TOLx& Lx) const {
  TExpValType _ValType=TExpValType(int(ValType));
  switch (_ValType){
    case evtUndef: Lx.PutIdStr("Undef"); break;
    case evtFlt: Lx.PutFlt(Flt); break;
    case evtStr: Lx.PutQStr(Str); break;
    case evtVec:{
      Lx.PutSym(syLBrace);
      for (int ValN=0; ValN<ValV.Len(); ValN++){
        ValV[ValN]->SaveTxt(Lx);}
      Lx.PutSym(syRBrace);
      break;}
    case evtLst:{
      Lx.PutSym(syLBracket);
      TExpValLN ValLN=ValL.First();
      for (int ValN=0; ValN<ValL.Len(); ValN++){
        ValLN->GetVal()->SaveTxt(Lx); ValLN=ValLN->Next();}
      Lx.PutSym(syRBracket);
      break;}
    default: Fail;
  }
}

void TExpVal::SaveTxt(const PSOut& SOut) const {
  TOLx Lx(SOut, TFSet()|oloCmtAlw|oloSigNum|oloCsSens);
  SaveTxt(Lx);
}

TStr TExpVal::GetStr() const {
  PSOut SOut=TMOut::New(); TMOut& MOut=*(TMOut*)SOut();
  SaveTxt(SOut);
  TStr ExpValStr=TStr::LoadTxt(MOut.GetSIn());
  return ExpValStr;
}

PExpVal TExpVal::MkClone(const PExpVal& ExpVal){
  PExpVal CloneExpVal=TExpVal::New();
  CloneExpVal->ValType=ExpVal->ValType;
  CloneExpVal->Flt=ExpVal->Flt;
  CloneExpVal->Str=ExpVal->Str;
  CloneExpVal->ValV.Gen(ExpVal->ValV.Len(), 0);
  for (int VecValN=0; VecValN<ExpVal->ValV.Len(); VecValN++){
    CloneExpVal->ValV.Add(MkClone(ExpVal->ValV[VecValN]));
  }
  TExpValLN ExpValLN=ExpVal->ValL.First();
  while (ExpValLN!=NULL){
    CloneExpVal->ValL.AddBack(MkClone(ExpValLN->GetVal()));
    ExpValLN=ExpValLN->Next();
  }
  return CloneExpVal;
}

PExpVal TExpVal::GetUndefExpVal(){
  if (UndefExpVal.Empty()){
    UndefExpVal=TExpVal::New();}
  return UndefExpVal;
}

PExpVal TExpVal::GetZeroExpVal(){
  if (ZeroExpVal.Empty()){
    ZeroExpVal=TExpVal::New(double(0));}
  return ZeroExpVal;
}

/////////////////////////////////////////////////
// Expression-Environment
bool TExpEnv::IsFuncOk(
 const TStr& RqFuncNm, const TExpFuncArgType& RqFuncArgType,
 const TStr& FuncNm, const TExpValV& ArgValV){
  if (RqFuncNm.GetUc()!=FuncNm.GetUc()){return false;}
  switch (RqFuncArgType){
    case efatVoid: return ArgValV.Len()==0;
    case efatFlt:
      if (ArgValV.Len()!=1){return false;}
      if (ArgValV[0]->GetValType()!=evtFlt){return false;}
      return true;
    case efatStr:
      if (ArgValV.Len()!=1){return false;}
      if (ArgValV[0]->GetValType()!=evtStr){return false;}
      return true;
    case efatFltFlt:
      if (ArgValV.Len()!=2){return false;}
      if (ArgValV[0]->GetValType()!=evtFlt){return false;}
      if (ArgValV[1]->GetValType()!=evtFlt){return false;}
      return true;
    case efatFltStr:
      if (ArgValV.Len()!=2){return false;}
      if (ArgValV[0]->GetValType()!=evtFlt){return false;}
      if (ArgValV[1]->GetValType()!=evtStr){return false;}
      return true;
    case efatStrFlt:
      if (ArgValV.Len()!=2){return false;}
      if (ArgValV[0]->GetValType()!=evtStr){return false;}
      if (ArgValV[1]->GetValType()!=evtFlt){return false;}
      return true;
    case efatStrStr:
      if (ArgValV.Len()!=2){return false;}
      if (ArgValV[0]->GetValType()!=evtStr){return false;}
      if (ArgValV[1]->GetValType()!=evtStr){return false;}
      return true;
    case efatStrAny:
      if (ArgValV.Len()!=2){return false;}
      if (ArgValV[0]->GetValType()!=evtStr){return false;}
      return true;
    default: Fail; return false;
  }
}

PExpEnv TExpEnv::DfExpEnv=PExpEnv(new TExpEnv());

/////////////////////////////////////////////////
// Expression-Built-Ins
TExpBi::TExpBi():
  ExpBiNmToIdH(100), ExpBiIdToArgTypeH(100){
  // constants
  AddBi("Undef", ebi_Undef);
  AddBi("True", ebi_True);
  AddBi("False", ebi_False);
  AddBi("E", ebi_E);
  AddBi("Pi", ebi_Pi);

  // trigonometric funcions
  AddBi("Sin", ebi_Sin, ebatFlt);
  AddBi("Cos", ebi_Cos, ebatFlt);
  AddBi("Tan", ebi_Tan, ebatFlt);
  AddBi("ASin", ebi_ASin, ebatFlt);
  AddBi("ACos", ebi_ACos, ebatFlt);
  AddBi("ATan", ebi_ATan, ebatFlt);
  AddBi("SinH", ebi_SinH, ebatFlt);
  AddBi("CosH", ebi_CosH, ebatFlt);
  AddBi("TanH", ebi_TanH, ebatFlt);

  // exponential functions
  AddBi("Pow", ebi_Pow, ebatFltFlt);
  AddBi("Exp", ebi_Exp, ebatFlt);
  AddBi("Sqr", ebi_Sqr, ebatFlt);
  AddBi("Sqrt", ebi_Sqrt, ebatFlt);
  AddBi("Log", ebi_Log, ebatFlt);
  AddBi("Log10", ebi_Log10, ebatFlt);

  // number manipulation functions
  AddBi("Ceil", ebi_Ceil, ebatFlt);
  AddBi("Floor", ebi_Floor, ebatFlt);
  AddBi("Int", ebi_Int, ebatFlt);
  AddBi("Frac", ebi_Frac, ebatFlt);
  AddBi("Abs", ebi_Abs, ebatFlt);

  // random deviates
  AddBi("UniDev", ebi_UniDev, ebatVoid);
  AddBi("NrmDev", ebi_NrmDev, ebatVoid);
  AddBi("ExpDev", ebi_ExpDev, ebatVoid);
  AddBi("GamDev", ebi_GamDev, ebatFlt);
  AddBi("PoiDev", ebi_PoiDev, ebatFlt);
  AddBi("BinDev", ebi_BinDev, ebatFltFlt);
  AddBi("UniDevStep", ebi_UniDevStep, ebatFltFlt);

  // assign values to constants
  Val_Undef=TExpVal::GetUndefExpVal();
  Val_True=TExpVal::New(double(1));
  Val_False=TExpVal::New(double(0));
  Val_E=TExpVal::New(TMath::E);
  Val_Pi=TExpVal::New(TMath::Pi);
}

void TExpBi::AddBi(const TStr& ExpBiNm, const TExpBiId& ExpBiId,
 const TExpBiArgType& ExpBiArgType){
  ExpBiNmToIdH.AddDat(ExpBiNm.GetUc(), TInt(int(ExpBiId)));
  ExpBiIdToArgTypeH.AddDat(TInt(int(ExpBiId)), TInt(int(ExpBiArgType)));
}

bool TExpBi::IsExpBiId(const TStr& ExpBiNm, TExpBiId& ExpBiId){
  int ExpBiIdP;
  if (ExpBiNmToIdH.IsKey(ExpBiNm.GetUc(), ExpBiIdP)){
    ExpBiId=TExpBiId(int(ExpBiNmToIdH[ExpBiIdP])); return true;
  } else {
    ExpBiId=ebi_Undef; return false;
  }
}

TExpBiArgType TExpBi::GetExpBiArgType(const TExpBiId& ExpBiId){
  TInt ExpBiArgType=ExpBiIdToArgTypeH.GetDat(TInt(int(ExpBiId)));
  return TExpBiArgType(int(ExpBiArgType));
}

void TExpBi::AssertArgs(const int& RqArgs, const int& ActArgs){
  if (RqArgs!=ActArgs){
    TExcept::Throw("Invalid number of arguments.");
  }
}

void TExpBi::AssertArgValType(
 const TExpValType& ExpValType, const PExpVal& ExpVal){
  if (ExpValType!=ExpVal->GetValType()){
    TExcept::Throw("Invalid type of argument.");
  }
}

PExpVal TExpBi::GetBiConstVal(const TExpBiId& ExpBiId){
  switch (ExpBiId){
    case ebi_Undef: return Val_Undef;
    case ebi_True: return Val_True;
    case ebi_False: return Val_False;
    case ebi_E: return Val_E;
    case ebi_Pi: return Val_Pi;
    default: TExcept::Throw("Invalid constant."); return Val_Undef;
  }
}

PExpVal TExpBi::GetBiFuncVal(
 const TExpBiId& ExpBiId, const TExpValV& ArgValV, const PExpEnv& ExpEnv){
  TExpBiArgType ExpBiArgType=TExpBi::GetExpBiArgType(ExpBiId);
  int Args=ArgValV.Len();
  double ArgFlt1=0; double ArgFlt2=0;
  switch (ExpBiArgType){
    case ebatUndef: Fail; break;
    case ebatVoid:
      AssertArgs(0, Args); break;
    case ebatFlt:
      AssertArgs(1, Args);
      AssertArgValType(evtFlt, ArgValV[0]);
      ArgFlt1=ArgValV[0]->GetFltVal(); break;
    case ebatFltFlt:
      AssertArgs(2, Args);
      AssertArgValType(evtFlt, ArgValV[0]);
      AssertArgValType(evtFlt, ArgValV[1]);
      ArgFlt1=ArgValV[0]->GetFltVal();
      ArgFlt2=ArgValV[1]->GetFltVal(); break;
    default: Fail;
  }
  PExpVal ExpVal;
  switch (ExpBiId){
    // trigonometric funcions
    case ebi_Sin: ExpVal=TExpVal::New(sin(ArgFlt1)); break;
    case ebi_Cos: ExpVal=TExpVal::New(cos(ArgFlt1)); break;
    case ebi_Tan: ExpVal=TExpVal::New(tan(ArgFlt1)); break;
    case ebi_ASin: ExpVal=TExpVal::New(asin(ArgFlt1)); break;
    case ebi_ACos: ExpVal=TExpVal::New(acos(ArgFlt1)); break;
    case ebi_ATan: ExpVal=TExpVal::New(atan(ArgFlt1)); break;
    case ebi_SinH: ExpVal=TExpVal::New(sinh(ArgFlt1)); break;
    case ebi_CosH: ExpVal=TExpVal::New(cosh(ArgFlt1)); break;
    case ebi_TanH: ExpVal=TExpVal::New(tanh(ArgFlt1)); break;

    // exponential functions
    case ebi_Pow: ExpVal=TExpVal::New(pow(ArgFlt1, ArgFlt2)); break;
    case ebi_Exp: ExpVal=TExpVal::New(exp(ArgFlt1)); break;
    case ebi_Sqr: ExpVal=TExpVal::New(TMath::Sqr(ArgFlt1)); break;
    case ebi_Sqrt: ExpVal=TExpVal::New(sqrt(ArgFlt1)); break;
    case ebi_Log: ExpVal=TExpVal::New(log(ArgFlt1)); break;
    case ebi_Log10: ExpVal=TExpVal::New(log10(ArgFlt1)); break;

    // number manipulation functions
    case ebi_Ceil: ExpVal=TExpVal::New(ceil(ArgFlt1)); break;
    case ebi_Floor: ExpVal=TExpVal::New(floor(ArgFlt1)); break;
    case ebi_Int:{
      double Int; modf(ArgFlt1, &Int);
      ExpVal=TExpVal::New(Int); break;}
    case ebi_Frac:{
      double Frac, Int; Frac=modf(ArgFlt1, &Int);
      ExpVal=TExpVal::New(Frac); break;}
    case ebi_Abs: ExpVal=TExpVal::New(fabs(ArgFlt1)); break;

    // random deviates
    case ebi_UniDev: ExpVal=TExpVal::New(ExpEnv->GetRnd().GetUniDev()); break;
    case ebi_NrmDev: ExpVal=TExpVal::New(ExpEnv->GetRnd().GetNrmDev()); break;
    case ebi_ExpDev: ExpVal=TExpVal::New(ExpEnv->GetRnd().GetExpDev()); break;
    case ebi_GamDev:{
      int ArgInt1=int(ArgFlt1);
      ExpVal=TExpVal::New(ExpEnv->GetRnd().GetGammaDev(ArgInt1)); break;}
    case ebi_PoiDev:{
      ExpVal=TExpVal::New(ExpEnv->GetRnd().GetPoissonDev(ArgFlt1)); break;}
    case ebi_BinDev:{
      int ArgInt2=int(ArgFlt2);
      ExpVal=TExpVal::New(ExpEnv->GetRnd().GetBinomialDev(ArgFlt1, ArgInt2)); break;}
    case ebi_UniDevStep:{
      int ArgInt1=int(ArgFlt1); if (ArgInt1<0){ArgInt1=0;}
      int ArgInt2=int(ArgFlt2);
      ExpVal=TExpVal::New(TRnd::GetUniDevStep(ArgInt1, ArgInt2)); break;}
    case ebi_NrmDevStep:{
      int ArgInt1=int(ArgFlt1); if (ArgInt1<0){ArgInt1=0;}
      int ArgInt2=int(ArgFlt2);
      ExpVal=TExpVal::New(TRnd::GetNrmDevStep(ArgInt1, ArgInt2)); break;}
    case ebi_ExpDevStep:{
      int ArgInt1=int(ArgFlt1); if (ArgInt1<0){ArgInt1=0;}
      int ArgInt2=int(ArgFlt2);
      ExpVal=TExpVal::New(TRnd::GetExpDevStep(ArgInt1, ArgInt2)); break;}

    default: TExcept::Throw("Invalid function.");
  }
  return ExpVal;
}

/////////////////////////////////////////////////
// Expression
TExpBi TExp::ExpBi;

const TFSet TExp::MulOpSymSet(syAsterisk, sySlash, syPercent, syHash, syAmpersand);
const TFSet TExp::UAddOpSymSet(syPlus, syMinus);
const TFSet TExp::AddOpSymSet(syPlus, syMinus, syVBar);
const TFSet TExp::RelOpSymSet(syEq, syNEq, syLss, syGtr, syLEq, syGEq);

const TFSet TExp::FactExpExpect(syFlt, syIdStr, syQStr, syLParen);
const TFSet TExp::MulExpExpect(FactExpExpect);
const TFSet TExp::AddExpExpect(MulExpExpect, UAddOpSymSet);
const TFSet TExp::RelExpExpect(AddExpExpect);
const TFSet TExp::ExpExpect(RelExpExpect);

PExpVal TExp::EvalExpOp(
 const PExpEnv& ExpEnv, const bool& DbgP, TChA& DbgChA){
  PExpVal OutExpVal;
  TExpOp _ExpOp=TExpOp(int(ExpOp));
  switch (_ExpOp){
    case eoUPlus:
    case eoUMinus:
    case eoNot:{
      PExpVal ExpVal=ArgExpV[0]->EvalExp(ExpEnv, DbgP, DbgChA);
      TExpValType ExpValType=ExpVal->GetValType();
      if (ExpValType==evtFlt){
        TFlt Flt;
        switch (_ExpOp){
          case eoUPlus: Flt=ExpVal->GetFltVal(); break;
          case eoUMinus: Flt=-ExpVal->GetFltVal(); break;
          case eoNot: Flt=double(ExpVal->GetFltValAsInt()==0);
          default: Fail; Flt=0;
        }
        OutExpVal=TExpVal::New(Flt);
      } else {
        TExcept::Throw("Bad argument types.");
      }
      break;}
    case eoPlus:
    case eoMinus:
    case eoMul:
    case eoDiv:
    case eoIDiv:
    case eoMod:
    case eoAnd:
    case eoOr:{
      PExpVal LExpVal=ArgExpV[0]->EvalExp(ExpEnv, DbgP, DbgChA);
      PExpVal RExpVal=ArgExpV[1]->EvalExp(ExpEnv, DbgP, DbgChA);
      TExpValType LExpValType=LExpVal->GetValType();
      TExpValType RExpValType=RExpVal->GetValType();
      if ((LExpValType==evtFlt)&&(RExpValType==evtFlt)){
        // check left expression
        double LVal=LExpVal->GetFltVal();
        int LValExpon; frexp(LVal, &LValExpon);
        if (LValExpon>150){LExpVal=TExpVal::GetZeroExpVal();}
        // check right expression
        double RVal=LExpVal->GetFltVal();
        int RValExpon; frexp(RVal, &RValExpon);
        if (RValExpon>150){RExpVal=TExpVal::GetZeroExpVal();}
        // calculate
        TFlt Flt;
        switch (_ExpOp){
          case eoPlus: Flt=LExpVal->GetFltVal()+RExpVal->GetFltVal(); break;
          case eoMinus: Flt=LExpVal->GetFltVal()-RExpVal->GetFltVal(); break;
          case eoMul: Flt=LExpVal->GetFltVal()*RExpVal->GetFltVal(); break;
          case eoDiv:
            if (RExpVal->GetFltVal()==0){TExcept::Throw("Division by zero.");}
            else {Flt=LExpVal->GetFltVal()/RExpVal->GetFltVal();}
            break;
          case eoIDiv:
            if (RExpVal->GetFltValAsInt()==0){TExcept::Throw("Division by zero.");}
            else {Flt=LExpVal->GetFltValAsInt()/RExpVal->GetFltValAsInt();}
            break;
          case eoMod:
            if (RExpVal->GetFltValAsInt()==0){TExcept::Throw("Division by zero.");}
            else {Flt=LExpVal->GetFltValAsInt()%RExpVal->GetFltValAsInt();}
            break;
          case eoAnd:
            Flt=(LExpVal->GetFltValAsInt()!=0)&&(RExpVal->GetFltValAsInt()!=0); break;
          case eoOr:
            Flt=(LExpVal->GetFltValAsInt()!=0)||(RExpVal->GetFltValAsInt()!=0); break;
          default: Fail; Flt=0;
        }
        OutExpVal=TExpVal::New(Flt);
      } else
      if ((_ExpOp==eoPlus)&&(LExpValType==evtStr)&&(RExpValType==evtStr)){
        TStr Str=LExpVal->GetStrVal()+RExpVal->GetStrVal();
        OutExpVal=TExpVal::New(Str);
      } else {
        TExcept::Throw("Bad argument types.");
      }
      break;}
    case eoEq:
    case eoNEq:
    case eoLss:
    case eoGtr:
    case eoLEq:
    case eoGEq:{
      PExpVal LExpVal=ArgExpV[0]->EvalExp(ExpEnv, DbgP, DbgChA);
      PExpVal RExpVal=ArgExpV[1]->EvalExp(ExpEnv, DbgP, DbgChA);
      TExpValType LExpValType=LExpVal->GetValType();
      TExpValType RExpValType=RExpVal->GetValType();
      if ((LExpValType==evtFlt)&&(RExpValType==evtFlt)){
        TFlt Flt;
        switch (_ExpOp){
          case eoEq: Flt=double(LExpVal->GetFltVal()==RExpVal->GetFltVal()); break;
          case eoNEq: Flt=double(LExpVal->GetFltVal()!=RExpVal->GetFltVal()); break;
          case eoLss: Flt=double(LExpVal->GetFltVal()<RExpVal->GetFltVal()); break;
          case eoGtr: Flt=double(LExpVal->GetFltVal()>RExpVal->GetFltVal()); break;
          case eoLEq: Flt=double(LExpVal->GetFltVal()<=RExpVal->GetFltVal()); break;
          case eoGEq: Flt=double(LExpVal->GetFltVal()>=RExpVal->GetFltVal()); break;
          default: Fail; Flt=0;
        }
        OutExpVal=TExpVal::New(Flt);
      } else
      if ((LExpValType==evtStr)&&(RExpValType==evtStr)){
        TFlt Flt;
        switch (_ExpOp){
          case eoEq: Flt=double(LExpVal->GetStrVal()==RExpVal->GetStrVal()); break;
          case eoNEq: Flt=double(LExpVal->GetStrVal()!=RExpVal->GetStrVal()); break;
          case eoLss: Flt=double(LExpVal->GetStrVal()<RExpVal->GetStrVal()); break;
          case eoGtr: Flt=double(LExpVal->GetStrVal()>RExpVal->GetStrVal()); break;
          case eoLEq: Flt=double(LExpVal->GetStrVal()<=RExpVal->GetStrVal()); break;
          case eoGEq: Flt=double(LExpVal->GetStrVal()>=RExpVal->GetStrVal()); break;
          default: Fail; Flt=0;
        }
        OutExpVal=TExpVal::New(Flt);
      } else {
        TExcept::Throw("Bad argument types.");
      }
      break;}
    case eoIf:{
      PExpVal CondExpVal=ArgExpV[0]->EvalExp(ExpEnv, DbgP, DbgChA);
      TExpValType CondExpValType=CondExpVal->GetValType();
      if (CondExpValType==evtFlt){
        PExpVal ExpVal;
        if (CondExpVal->GetFltVal()!=0){
          ExpVal=ArgExpV[1]->EvalExp(ExpEnv, DbgP, DbgChA);
        } else {
          ExpVal=ArgExpV[2]->EvalExp(ExpEnv, DbgP, DbgChA);
        }
        OutExpVal=ExpVal;
      } else {
        TExcept::Throw("Bad argument types.");
      }
      break;}
    default: Fail; OutExpVal=NULL;
  }
  if (DbgP){
    DbgChA+="['"; DbgChA+=TExp::GetExpOpStr(_ExpOp);
    DbgChA+="'='"; DbgChA+=OutExpVal->GetStr(); DbgChA+="'] ";
  }
  return OutExpVal;
}

PExpVal TExp::EvalExp(
 const PExpEnv& ExpEnv, const bool& DbgP, TChA& DbgChA){
  PExpVal OutExpVal;
  TExpType _ExpType=TExpType(int(ExpType));
  switch (_ExpType){
    case etUndef:
      OutExpVal=TExpVal::GetUndefExpVal();
      break;
    case etVal:
      OutExpVal=ExpVal;
      break;
    case etVec:{
      PExpVal ExpVal=TExpVal::New(evtVec);
      for (int ArgExpN=0; ArgExpN<ArgExpV.Len(); ArgExpN++){
        PExpVal ArgExpVal=
         PExpVal(ArgExpV[ArgExpN]->EvalExp(ExpEnv, DbgP, DbgChA));
        ExpVal->AddToVec(ArgExpVal);
      }
      OutExpVal=ExpVal;
      break;}
    case etLst:{
      PExpVal ExpVal=TExpVal::New(evtLst);
      for (int ArgExpN=0; ArgExpN<ArgExpV.Len(); ArgExpN++){
        PExpVal ArgExpVal=
         PExpVal(ArgExpV[ArgExpN]->EvalExp(ExpEnv, DbgP, DbgChA));
        ExpVal->AddToLst(ArgExpVal);
      }
      OutExpVal=ExpVal;
      break;}
    case etOp:
      OutExpVal=EvalExpOp(ExpEnv, DbgP, DbgChA); break;
    case etVar:{
      bool IsVar=false;
      PExpVal ExpVal=ExpEnv->GetVarVal(ExpNm.GetUc(), IsVar);
      if (!IsVar){TExcept::Throw(TStr("Variable not defined (")+ExpNm+").");}
      OutExpVal=ExpVal;
      break;}
    case etBiConst:
      OutExpVal=ExpBi.GetBiConstVal(TExpBiId(int(ExpBiId)));
      break;
    case etFunc:
    case etBiFunc:{
      TExpValV ArgExpValV(ArgExpV.Len(), 0);
      for (int ArgExpN=0; ArgExpN<ArgExpV.Len(); ArgExpN++){
        PExpVal ArgExpVal=
         PExpVal(ArgExpV[ArgExpN]->EvalExp(ExpEnv, DbgP, DbgChA));
        ArgExpValV.Add(ArgExpVal);
      }
      switch (_ExpType){
        case etFunc:{
          bool IsFunc=false;
          PExpVal ExpVal=ExpEnv->GetFuncVal(ExpNm.GetUc(), ArgExpValV, IsFunc);
          if (!IsFunc){
            TExcept::Throw(TStr("Function not defined (")+ExpNm+").");}
          OutExpVal=ExpVal;
          break;}
        case etBiFunc:
          OutExpVal=ExpBi.GetBiFuncVal(TExpBiId(int(ExpBiId)), ArgExpValV, ExpEnv);
          break;
        default: Fail; OutExpVal=NULL;
      }
      break;}
    default: Fail; OutExpVal=NULL;
  }
  if (DbgP){
    switch (_ExpType){
      case etVal:
      case etOp:
        break;
      case etUndef:
      case etVec:
      case etLst:{
        TStr ExpTypeStr=TExp::GetExpTypeStr(_ExpType);
        DbgChA+='['; DbgChA+=ExpTypeStr; DbgChA+='=';
        DbgChA+=OutExpVal->GetStr(); DbgChA+="] ";
        break;}
      case etVar:
      case etBiConst:
      case etFunc:
      case etBiFunc:
        DbgChA+='['; DbgChA+=ExpNm; DbgChA+='=';
        DbgChA+=OutExpVal->GetStr(); DbgChA+="] ";
        break;
      default: Fail;
    }
  }
  return OutExpVal;
}

TExpOp TExp::GetExpOpFromLxSym(const TLxSym& LxSym){
  switch (LxSym){
    case syPlus: return eoPlus;
    case syMinus: return eoMinus;
    case syAsterisk: return eoMul;
    case sySlash: return eoDiv;
    case syPercent: return eoMod;
    case syExclamation: return eoNot;
    case syVBar: return eoOr;
    case syAmpersand: return eoAnd;
    case syQuestion: return eoIf;
    case syHash: return eoIDiv;
    case syEq: return eoEq;
    case syNEq: return eoNEq;
    case syLss: return eoLss;
    case syGtr: return eoGtr;
    case syLEq: return eoLEq;
    case syGEq: return eoGEq;
    default: Fail; return eoUndef;
  }
}

TLxSym TExp::GetLxSymFromExpOp(const TExpOp& ExpOp){
  switch (ExpOp){
    case eoUPlus: return syPlus;
    case eoUMinus: return syMinus;
    case eoNot: return syExclamation;
    case eoPlus: return syPlus;
    case eoMinus: return syMinus;
    case eoMul: return syAsterisk;
    case eoDiv: return sySlash;
    case eoIDiv: return syHash;
    case eoMod: return syPercent;
    case eoAnd: return syAmpersand;
    case eoOr: return syVBar;
    case eoEq: return syEq;
    case eoNEq: return syNEq;
    case eoLss: return syLss;
    case eoGtr: return syGtr;
    case eoLEq: return syLEq;
    case eoGEq: return syGEq;
    case eoIf: return syQuestion;
    default: Fail; return syUndef;
  }
}

PExp TExp::LoadTxtFact(TILx& Lx, const TFSet& Expect){
  PExp Exp;
  switch (Lx.Sym){
    case syFlt:{
      PExpVal ExpVal=TExpVal::New(Lx.Flt);
      Exp=PExp(new TExp(ExpVal));
      Lx.GetSym(Expect);
      break;}
    case syIdStr:{
      TStr ExpNm=Lx.Str;
      Lx.GetSym(TFSet(Expect)|syLParen);
      if (Lx.Sym==syLParen){
        TExpV ArgExpV;
        Lx.GetSym(TFSet(ExpExpect)|syRParen);
        while (Lx.Sym!=syRParen){
          if (Lx.Sym==syComma){Lx.GetSym(ExpExpect);}
          PExp ArgExp=LoadTxtExp(Lx, TFSet()|syComma|syRParen);
          ArgExpV.Add(ArgExp);
        }
        Lx.GetSym(Expect);
        Exp=PExp(new TExp(ExpNm, ArgExpV));
      } else {
        Exp=PExp(new TExp(ExpNm));
      }
      break;}
    case syQStr:{
      PExpVal ExpVal=TExpVal::New(Lx.Str);
      Exp=PExp(new TExp(ExpVal));
      Lx.GetSym(Expect);
      break;}
    case syLParen:{
      Lx.GetSym(ExpExpect);
      Exp=LoadTxtExp(Lx, TFSet()|syRParen);
      Exp->IsParen=true;
      Lx.GetSym(Expect);
      break;}
    default: Fail;
  }
  return Exp;
}

PExp TExp::LoadTxtMulExp(TILx& Lx, const TFSet& Expect){
  PExp Exp=LoadTxtFact(Lx, TFSet(Expect)|MulOpSymSet);
  while (MulOpSymSet.In(Lx.Sym)){
    TExpOp ExpOp=GetExpOpFromLxSym(Lx.Sym);
    Lx.GetSym(FactExpExpect);
    PExp RExp=LoadTxtFact(Lx, TFSet(Expect)|MulOpSymSet);
    Exp=PExp(new TExp(ExpOp, Exp, RExp));
  }
  return Exp;
}

PExp TExp::LoadTxtAddExp(TILx& Lx, const TFSet& Expect){
  TExpOp PrefExpOp=eoUndef;
  if (Lx.Sym==syPlus){PrefExpOp=eoUPlus; Lx.GetSym(MulExpExpect);}
  else if (Lx.Sym==syMinus){PrefExpOp=eoUMinus; Lx.GetSym(MulExpExpect);}
  PExp Exp=LoadTxtMulExp(Lx, TFSet(Expect)|AddOpSymSet);
  if (PrefExpOp!=eoUndef){
    Exp=PExp(new TExp(PrefExpOp, Exp));}
  while (AddOpSymSet.In(Lx.Sym)){
    TExpOp ExpOp=GetExpOpFromLxSym(Lx.Sym);
    Lx.GetSym(MulExpExpect);
    PExp RExp=LoadTxtMulExp(Lx, TFSet(Expect)|AddOpSymSet);
    Exp=PExp(new TExp(ExpOp, Exp, RExp));
  }
  return Exp;
}

PExp TExp::LoadTxtRelExp(TILx& Lx, const TFSet& Expect){
  PExp Exp=LoadTxtAddExp(Lx, TFSet(Expect)|RelOpSymSet);
  if (RelOpSymSet.In(Lx.Sym)){
    TExpOp ExpOp=GetExpOpFromLxSym(Lx.Sym);
    Lx.GetSym(AddExpExpect);
    PExp RExp=LoadTxtAddExp(Lx, Expect);
    Exp=PExp(new TExp(ExpOp, Exp, RExp));
  }
  return Exp;
}

PExp TExp::LoadTxtExp(TILx& Lx, const TFSet& Expect){
  PExp Exp=LoadTxtRelExp(Lx, TFSet(Expect)|syQuestion);
  if (Lx.Sym==syQuestion){
    TExpOp ExpOp=GetExpOpFromLxSym(Lx.Sym);
    Lx.GetSym(ExpExpect);
    PExp ThenExp=LoadTxtExp(Lx, TFSet()|syColon);
    Lx.GetSym(ExpExpect);
    PExp ElseExp=LoadTxtExp(Lx, Expect);
    Exp=PExp(new TExp(ExpOp, Exp, ThenExp, ElseExp));
  }
  return Exp;
}

void TExp::SaveTxtOp(TOLx& Lx) const {
  IAssert(TExpType(static_cast<int>(ExpType))==etOp);
  TExpOp _ExpOp=TExpOp(int(ExpOp));
  TLxSym OpSym=GetLxSymFromExpOp(_ExpOp);
  switch (_ExpOp){
    case eoUPlus:
    case eoUMinus:
    case eoNot:
      Lx.PutSym(OpSym); ArgExpV[0]->SaveTxt(Lx);
      break;
    case eoPlus: case eoMinus:
    case eoMul: case eoDiv:
    case eoIDiv: case eoMod:
    case eoAnd: case eoOr:
    case eoEq: case eoNEq:
    case eoLss: case eoGtr:
    case eoLEq: case eoGEq:
      ArgExpV[0]->SaveTxt(Lx); Lx.PutSym(OpSym); ArgExpV[1]->SaveTxt(Lx);
      break;
    case eoIf:
      ArgExpV[0]->SaveTxt(Lx); Lx.PutSym(OpSym);
      ArgExpV[1]->SaveTxt(Lx); Lx.PutSym(syColon); ArgExpV[2]->SaveTxt(Lx);
      break;
    default: Fail;
  }
}

TExp::TExp(const TExpOp& _ExpOp,
 const PExp& Exp1, const PExp& Exp2, const PExp& Exp3):
  ExpType(etOp), IsParen(false),
  ExpVal(), ExpNm(), ExpOp(_ExpOp), ExpBiId(), ArgExpV(){
  ArgExpV.Add(Exp1);
  if (!Exp2.Empty()){ArgExpV.Add(Exp2);}
  if (!Exp3.Empty()){ArgExpV.Add(Exp3);}
}

TExp::TExp(const PExpVal& _ExpVal):
  ExpType(etVal), IsParen(false),
  ExpVal(_ExpVal), ExpNm(), ExpOp(), ExpBiId(), ArgExpV(){}

TExp::TExp(const TStr& _VarNm):
  ExpType(), IsParen(false),
  ExpVal(), ExpNm(_VarNm), ExpOp(), ExpBiId(), ArgExpV(){
  TExpBiId _ExpBiId;
  if (ExpBi.IsExpBiId(ExpNm, _ExpBiId)){
    ExpType=etBiConst;
    ExpBiId=TInt(int(_ExpBiId));
  } else {
    ExpType=etVar;
  }
}

TExp::TExp(const TStr& _FuncNm, const TExpV& _ArgExpV):
  ExpType(), IsParen(false),
  ExpVal(), ExpNm(_FuncNm), ExpOp(), ExpBiId(), ArgExpV(_ArgExpV){
  TExpBiId _ExpBiId;
  if (ExpBi.IsExpBiId(ExpNm, _ExpBiId)){
    ExpType=etBiFunc;
    ExpBiId=TInt(int(_ExpBiId));
  } else {
    ExpType=etFunc;
  }
}

PExp TExp::LoadTxt(
 const PSIn& SIn, bool& Ok, TStr& MsgStr, const TFSet& Expect){
  TILx Lx(SIn, TFSet()|iloCmtAlw|iloCsSens|iloExcept);
  PExp Exp; Ok=true; MsgStr="Ok";
  try {
    Lx.GetSym(ExpExpect);
    Exp=LoadTxtExp(Lx, Expect);
  }
  catch (PExcept Except){
    Ok=false; MsgStr=Except->GetMsgStr();
    Exp=PExp(new TExp(etUndef));
  }
  return Exp;
}

void TExp::SaveTxt(TOLx& Lx) const {
  if (IsParen){Lx.PutSym(syLParen);}
  TExpType _ExpType=TExpType(int(ExpType));
  switch (_ExpType){
    case etVal:
      ExpVal->SaveTxt(Lx); break;
    case etVec:{
      Lx.PutSym(syLBrace);
      for (int ArgExpN=0; ArgExpN<ArgExpV.Len(); ArgExpN++){
        if (ArgExpN>0){Lx.PutSym(syComma);}
        ArgExpV[ArgExpN]->SaveTxt(Lx);
      }
      Lx.PutSym(syRBrace);
      break;}
    case etLst:{
      Lx.PutSym(syLBracket);
      for (int ArgExpN=0; ArgExpN<ArgExpV.Len(); ArgExpN++){
        if (ArgExpN>0){Lx.PutSym(syComma);}
        ArgExpV[ArgExpN]->SaveTxt(Lx);
      }
      Lx.PutSym(syRBracket);
      break;}
    case etOp:
      SaveTxtOp(Lx); break;
    case etVar:
    case etBiConst:
      Lx.PutIdStr(ExpNm); break;
    case etFunc:
    case etBiFunc:{
      Lx.PutIdStr(ExpNm);
      Lx.PutSym(syLParen);
      for (int ArgExpN=0; ArgExpN<ArgExpV.Len(); ArgExpN++){
        if (ArgExpN>0){Lx.PutSym(syComma);}
        ArgExpV[ArgExpN]->SaveTxt(Lx);
      }
      Lx.PutSym(syRParen);
      break;}
    default: Fail;
  }
  if (IsParen){Lx.PutSym(syRParen);}
}

TStr TExp::GetStr() const {
  PSOut SOut=TMOut::New(); TMOut& MOut=*(TMOut*)SOut();
  SaveTxt(SOut);
  TStr ExpStr=TStr::LoadTxt(MOut.GetSIn());
  return ExpStr;
}

TStr TExp::GetTopObjNm() const {
  TStr TopObjNm;
  TExpType _ExpType=TExpType(int(ExpType));
  switch (_ExpType){
    case etOp:{
      TExpOp _ExpOp=TExpOp(int(ExpOp));
      TopObjNm=GetExpOpStr(_ExpOp);
      break;}
    case etVar:
    case etBiConst:
    case etFunc:
    case etBiFunc:{
      TopObjNm=ExpNm; break;}
    default: break;
  }
  return TopObjNm;
}

int TExp::GetArgExps() const {
  return ArgExpV.Len();
}

TStr TExp::GetArgExpStr(const int& ArgExpN) const {
  return ArgExpV[ArgExpN]->GetStr();
}

PExpVal TExp::Eval(
 bool& Ok, TStr& MsgStr, const bool& DbgP, TStr& DbgStr, const PExpEnv& ExpEnv){
  Ok=true; MsgStr="Ok";
  PExpVal ExpVal; TChA DbgChA;
  if (DbgP){DbgChA+="Debug Expression: ";}
  try {
    ExpVal=EvalExp(ExpEnv, DbgP, DbgChA);
  }
  catch (PExcept E){
    Ok=false; MsgStr=E->GetMsgStr();
  }
  if (!Ok){return TExpVal::GetUndefExpVal();}
  if (DbgP){
    DbgChA+='['; DbgChA+=GetStr(); DbgChA+=" -> ";
    DbgChA+=ExpVal->GetStr(); DbgChA+="] "; DbgChA+=MsgStr;
    DbgStr=DbgChA;
  }
  return ExpVal;
}

PExpVal TExp::LoadAndEvalExpL(
 const TStr& ExpLStr, bool& Ok, TStr& MsgStr, const PExpEnv& ExpEnv){
  // create final expression value
  PExpVal ExpVal;
  // transform exp. str. to input stream
  PSIn SIn=TStrIn::New(ExpLStr);
  // create lexical
  TILx Lx(SIn, TFSet()|iloCmtAlw|iloCsSens|iloExcept);
  TFSet Expect=TFSet()|sySemicolon|syEof;
  // load & evaluate expression separated by semicolon
  while (Lx.Sym!=syEof){
    // create expression
    PExp Exp; Ok=true; MsgStr="Ok";
    try {
      Lx.GetSym(ExpExpect);
      Exp=LoadTxtExp(Lx, Expect);
    }
    catch (PExcept Except){
      Ok=false; MsgStr=Except->GetMsgStr();
      Exp=PExp(new TExp(etUndef));
    }
    // evaluate expression
    if (Ok){
      ExpVal=Exp->Eval(Ok, MsgStr, ExpEnv);
      //printf("%s\n", ExpVal->GetStr().CStr());
      if (!Ok){
        return NULL;}
    } else {
      return NULL;
    }
  }
  return ExpVal;
}

TStr TExp::GetExpTypeStr(const TExpType& ExpType){
  switch (ExpType){
    case etUndef: return "Undef";
    case etVal: return "Val";
    case etVec: return "Vec";
    case etLst: return "Lst";
    case etOp: return "Op";
    case etVar: return "Var";
    case etBiConst: return "BiConst";
    case etFunc: return "Func";
    case etBiFunc: return "BiFunc";
    default: Fail; return "";
  }
}

void TExp::GetBiDescV(TStrPrV& BiDescV){
  BiDescV.Clr();
  // constants
  BiDescV.Add(TStrPr("True", "Logical 'True' == 1."));
  BiDescV.Add(TStrPr("False", "Logical 'False' == 0."));
  BiDescV.Add(TStrPr("E", "Nat. logarithm basis (2.7182...)."));
  BiDescV.Add(TStrPr("Pi", "Constant pi (3.1415...)."));

  // trigonometric funcions
  BiDescV.Add(TStrPr("Sin(X)", "Sine of angle in radians."));
  BiDescV.Add(TStrPr("Cos(X)", "Cosine of angle in radians."));
  BiDescV.Add(TStrPr("Tan(X)", "Tangent of angle in radians."));
  BiDescV.Add(TStrPr("ASin(X)", "Arc sine of (-1..+1)."));
  BiDescV.Add(TStrPr("ACos(X)", "Arc cosine of (-1..+1)."));
  BiDescV.Add(TStrPr("ATan(X)", "Arc tangent of (-inf..+inf)."));
  BiDescV.Add(TStrPr("SinH(X)", "Hyperbolic sine."));
  BiDescV.Add(TStrPr("CosH(X)", "Hyperbolic cosine."));
  BiDescV.Add(TStrPr("TanH(X)", "Hyperbolic tangent."));

  // exponential functions
  BiDescV.Add(TStrPr("Pow(X, Y)", "X to the power of Y."));
  BiDescV.Add(TStrPr("Exp(X)", "Exponential E to the power of X."));
  BiDescV.Add(TStrPr("Sqr(X)", "X squared."));
  BiDescV.Add(TStrPr("Sqrt(X)", "Positive square root."));
  BiDescV.Add(TStrPr("Log(X)", "Natural logarithm."));
  BiDescV.Add(TStrPr("Log10(X)", "Base 10 logarithm."));

  // number manipulation functions
  BiDescV.Add(TStrPr("Ceil(X)", "The smallest integer not less than X."));
  BiDescV.Add(TStrPr("Floor(X)", "The largest integer not greater than X."));
  BiDescV.Add(TStrPr("Int(X)", "Integer part of X."));
  BiDescV.Add(TStrPr("Frac(X)", "Fractional part of X."));
  BiDescV.Add(TStrPr("Abs(X)", "Absolute value of X."));

  // random deviates
  BiDescV.Add(TStrPr("UniDev()", "Uniform deviate (0..1)."));
  BiDescV.Add(TStrPr("NrmDev()", "Normal deviate (0, 1)."));
  BiDescV.Add(TStrPr("ExpDev()", "Exponential deviate."));
  BiDescV.Add(TStrPr("GamDev(Order)", "Gamma deviate of Order."));
  BiDescV.Add(TStrPr("PoiDev(Mean)", "Poisson deviate."));
  BiDescV.Add(TStrPr("BinDev(Prb, Trials)", "Binomial deviate."));

  // operators
  BiDescV.Add(TStrPr("+N", "Unary plus."));
  BiDescV.Add(TStrPr("-N", "Unary minus."));
  BiDescV.Add(TStrPr("!L", "Not."));
  BiDescV.Add(TStrPr("N1+N2", "Plus."));
  BiDescV.Add(TStrPr("N1-N2", "Minus."));
  BiDescV.Add(TStrPr("N1*N2", "Multiply."));
  BiDescV.Add(TStrPr("N1/N2", "Division."));
  BiDescV.Add(TStrPr("N1#N2", "Integer division."));
  BiDescV.Add(TStrPr("N1%N2", "Modulo."));
  BiDescV.Add(TStrPr("L1&L2", "And."));
  BiDescV.Add(TStrPr("L1|L2", "Or."));
  BiDescV.Add(TStrPr("E1=E2", "Equal."));
  BiDescV.Add(TStrPr("E1<>E2", "Not equal."));
  BiDescV.Add(TStrPr("E1<E2", "Less."));
  BiDescV.Add(TStrPr("E1>E2", "Greater."));
  BiDescV.Add(TStrPr("E1<=E2", "Less or equal."));
  BiDescV.Add(TStrPr("E1>=E2", "Greater or equal."));
  BiDescV.Add(TStrPr("L?E1:E2", "If L then return E1 else return E2."));
}

/////////////////////////////////////////////////
// Expression-Help-Object
TStr TExpHelpObj::GetHdArgNmStr() const {
  TChA ChA;
  switch (Type){
    case ehotOp: {// operator
      TStr OpStr=HdItem->GetNm();
      if (ArgItemV.Len()==1){
        ChA+=OpStr; ChA+=" "; ChA+=ArgItemV[0]->GetNm();
      } else
      if (ArgItemV.Len()==2){
        ChA+=ArgItemV[0]->GetNm();
        ChA+=" "; ChA+=OpStr; ChA+=" ";
        ChA+=ArgItemV[1]->GetNm();
      } else
      if (ArgItemV.Len()==3){
        ChA+=ArgItemV[0]->GetNm();
        ChA+=" "; ChA+=OpStr; ChA+=" ";
        ChA+=ArgItemV[1]->GetNm();
        ChA+=" "; ChA+=":"; ChA+=" ";
        ChA+=ArgItemV[2]->GetNm();
      } else {
        Fail;
      }
      break;}
    case ehotVar: // variable
      ChA+=HdItem->GetNm(); break;
    case ehotFunc: // function
      ChA+=HdItem->GetTypeStr(); ChA+=" <- ";
      ChA+=HdItem->GetNm();
      ChA+="(";
      {for (int ArgN=0; ArgN<ArgItemV.Len(); ArgN++){
        if (ArgN>0){ChA+=", ";}
        ChA+=ArgItemV[ArgN]->GetNm();
      }}
      ChA+=")";
      break;
    case ehotTempl: // template
      ChA+=HdItem->GetTypeStr(); break;
    default: Fail;
  }
  return ChA;
}

TExpHelpObjType TExpHelpObj::GetObjTypeFromStr(const TStr& TypeStr){
  if (TypeStr=="Op"){return ehotOp;}
  else if (TypeStr=="Var"){return ehotVar;}
  else if (TypeStr=="Func"){return ehotFunc;}
  else if (TypeStr=="Templ"){return ehotTempl;}
  else {TExcept::Throw("Invalid object type.", TypeStr); return ehotUndef;}
}

/////////////////////////////////////////////////
// Expression-Help
PExpHelp TExpHelp::LoadXml(const PSIn& SIn){
  // create expression help
  PExpHelp ExpHelp=TExpHelp::New();
  // load xml with expression help
  PXmlDoc Doc=TXmlDoc::LoadTxt(SIn);
  // retrieve objects
  TXmlTokV ObjTokV; Doc->GetTagTokV("ExpHelp|Obj", ObjTokV);
  for (int ObjTokN=0; ObjTokN<ObjTokV.Len(); ObjTokN++){
    PXmlTok ObjTok=ObjTokV[ObjTokN];
    // type
    TStr TypeStr=ObjTok->GetTagTok("Type")->GetTokStr(false);
    // category
    TStr CatNm=ObjTok->GetTagTok("Cat")->GetTokStr(false);
    // header
    TStr HdNm=ObjTok->GetTagTok("Head|Name")->GetTokStr(false);
    TStr HdTypeStr=ObjTok->GetTagTok("Head|Type")->GetTokStr(false);
    TStr HdDescStr=ObjTok->GetTagTok("Head|Desc")->GetTokStr(false);
    PExpHelpItem HdItem=
     TExpHelpItem::New(HdNm, HdTypeStr, HdDescStr, "");
    // arguments
    TXmlTokV ArgTokV; ObjTok->GetTagTokV("Args|Arg", ArgTokV);
    TExpHelpItemV ArgItemV;
    for (int ArgTokN=0; ArgTokN<ArgTokV.Len(); ArgTokN++){
      PXmlTok ArgTok=ArgTokV[ArgTokN];
      // argument
      TStr ArgNm=ArgTok->GetTagTok("Name")->GetTokStr(false);
      TStr ArgTypeStr=ArgTok->GetTagTok("Type")->GetTokStr(false);
      TStr ArgDescStr=ArgTok->GetTagTok("Desc")->GetTokStr(false);
      TStr ArgDfValStr=ArgTok->GetTagTok("Default")->GetTokStr(false);
      PExpHelpItem ArgItem=
       TExpHelpItem::New(ArgNm, ArgTypeStr, ArgDescStr, ArgDfValStr);
      ArgItemV.Add(ArgItem);
    }
    // create & add object
    TExpHelpObjType Type=TExpHelpObj::GetObjTypeFromStr(TypeStr);
    PExpHelpObj Obj=TExpHelpObj::New(Type, CatNm, HdItem, ArgItemV);
    ExpHelp->AddObj(Obj);
  }
  // return result
  return ExpHelp;
}

void TExpHelp::GetCatNmV(TStrV& CatNmV) const {
  CatNmV.Clr();
  for (int ObjN=0; ObjN<ObjV.Len(); ObjN++){
    TStr CatNm=ObjV[ObjN]->GetCatNm();
    CatNmV.AddUnique(CatNm);
  }
  CatNmV.Ins(0, "All");
}

void TExpHelp::GetObjHdNmV(const TStr& CatNm, TStrV& ObjHdNmV) const {
  ObjHdNmV.Clr();
  for (int ObjN=0; ObjN<ObjV.Len(); ObjN++){
    TStr ObjCatNm=ObjV[ObjN]->GetCatNm();
    TStr ObjHdNm=ObjV[ObjN]->GetHdItem()->GetNm();
    if ((CatNm.Empty())||(CatNm=="All")||(CatNm==ObjCatNm)){
      ObjHdNmV.AddUnique(ObjHdNm);}
  }
  ObjHdNmV.Sort();
}

PExpHelpObj TExpHelp::GetObj(const TStr& ObjNm) const {
  PExpHelpObj Obj;
  for (int ObjN=0; ObjN<ObjV.Len(); ObjN++){
    if (ObjV[ObjN]->GetHdItem()->GetNm().GetUc()==ObjNm.GetUc()){
      return ObjV[ObjN];}
  }
  return NULL;
}

