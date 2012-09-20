/////////////////////////////////////////////////
// Environment
TEnv::TEnv(const int& _Args, char** _ArgV, const PNotify& _Notify):
    ArgV(), HdStr(), MnArgs(1), SilentP(false), Notify(_Notify) {
  for (int ArgN = 0; ArgN < _Args; ArgN++)
      ArgV.Add(TStr(_ArgV[ArgN]));
}

TEnv::TEnv(const TStr& _ArgStr, const PNotify& _Notify):
    ArgV(), HdStr(), MnArgs(1), SilentP(false), Notify(_Notify) {
  _ArgStr.SplitOnAllCh(' ', ArgV);
}

TStr TEnv::GetExeFNm() const {
  TStr ExeFNm=GetArg(0);
  if (ExeFNm.IsPrefix("//?")){ // observed on Win64 CGI
    ExeFNm=ExeFNm.GetSubStr(3, ExeFNm.Len());
  }
  return ExeFNm;
}

TStr TEnv::GetCmLn(const int& FromArgN) const {
  TChA CmLnChA;
  for (int ArgN=FromArgN; ArgN<GetArgs(); ArgN++){
    if (ArgN>FromArgN){CmLnChA+=' ';}
    CmLnChA+=GetArg(ArgN);
  }
  return CmLnChA;
}

int TEnv::GetPrefixArgN(const TStr& PrefixStr) const {
  int ArgN=0;
  while (ArgN<GetArgs()){
    if (GetArg(ArgN).GetSubStr(0, PrefixStr.Len()-1)==PrefixStr){return ArgN;}
    ArgN++;
  }
  return -1;
}

TStr TEnv::GetArgPostfix(const TStr& PrefixStr) const {
  int ArgN=GetPrefixArgN(PrefixStr); IAssert(ArgN!=-1);
  TStr ArgStr=GetArg(ArgN);
  return ArgStr.GetSubStr(PrefixStr.Len(), ArgStr.Len());
}

void TEnv::PrepArgs(const TStr& _HdStr, const int& _MnArgs, const bool& _SilentP){
  // put environment state
  HdStr=_HdStr;
  MnArgs=_MnArgs;
  // silence
  SilentP=true;
  SilentP=Env.GetIfArgPrefixBool("-silent:", false, "Silence");
  // start header
  if (!SilentP){
    // print header
    if (!HdStr.Empty()){
      // print header-string
      TStr DateStr=__DATE__;
      printf("%s [%s]\n", HdStr.CStr(), DateStr.CStr());
      // print header-line
      for (int ChN=0; ChN<HdStr.Len()+DateStr.Len()+3; ChN++){printf("=");}
      printf("\n");
    }
    // print start of 'usage' message if not enough arguments
    if (Env.GetArgs()<=MnArgs){
      TStr ExeFNm=Env.GetArg(0).GetFBase();
      printf("usage: %s\n", ExeFNm.CStr());
    }
  }
}

bool TEnv::IsEndOfRun() const {
  if (!SilentP){
    // print line in length of header-line
    if (HdStr.Empty()){
      printf("========================================\n");
    } else {
      for (int ChN=0; ChN<HdStr.Len(); ChN++){printf("=");}
      printf("\n");
    }
  }
  // return
  return Env.GetArgs()<=MnArgs;
}

bool TEnv::IsArgStr(const TStr& ArgStr) {
  int ArgN=0;
  while (ArgN<GetArgs()){
    if (GetArg(ArgN)==ArgStr){return true;}
    ArgN++;
  }
  return false;
}

TStr TEnv::GetIfArgPrefixStr(
 const TStr& PrefixStr, const TStr& DfVal, const TStr& DNm) const {
  if (Env.GetArgs()<=MnArgs){
    // 'usage' argument message
    if (!SilentP){
      printf("   %s%s (default:'%s')\n", PrefixStr.CStr(), DNm.CStr(), DfVal.CStr());}
    return DfVal;
  } else {
    // argument & value message
    TStr Val;
    if (Env.IsArgPrefix(PrefixStr)){
      Val=Env.GetArgPostfix(PrefixStr);
      if (Val.Len()>1){
        if ((Val[0]=='\"')&&(Val.LastCh()=='\"')){
          Val=Val.GetSubStr(1, Val.Len()-2);
        }
      }
    } else {
      Val=DfVal;
    }
    TStr MsgStr=DNm+" ("+PrefixStr+")="+Val;
    if (!SilentP){TNotify::OnStatus(Notify, MsgStr);}
    return Val;
  }
}

TStrV TEnv::GetIfArgPrefixStrV(
 const TStr& PrefixStr, TStrV& DfValV, const TStr& DNm) const {
  TStrV ArgValV;
  if (Env.GetArgs()<=MnArgs){
    // 'usage' argument message
    if (!SilentP){
      printf("   %s%s (default:", PrefixStr.CStr(), DNm.CStr());
      for (int DfValN=0; DfValN<DfValV.Len(); DfValN++){
        if (DfValN>0){printf(", ");}
        printf("'%s'", DfValV[DfValN].CStr());
      }
      printf(")\n");
    }
    return ArgValV;
  } else {
    // argument & value message
    TStr ArgValVChA;
    for (int ArgN=0; ArgN<GetArgs(); ArgN++){
      // get argument string
      TStr ArgStr=GetArg(ArgN);
      if (ArgStr.GetSubStr(0, PrefixStr.Len()-1)==PrefixStr){
        // extract & add argument value
        TStr ArgVal=ArgStr.GetSubStr(PrefixStr.Len(), ArgStr.Len());
        ArgValV.Add(ArgVal);
        // add to message string
        if (ArgValV.Len()>1){ArgValVChA+=", ";}
        ArgValVChA+=ArgValV.Last();
      }
    }
    if (ArgValV.Empty()){ArgValV=DfValV;}
    // output argument values
    TChA MsgChA;
    MsgChA+=DNm; MsgChA+=" ("; MsgChA+=PrefixStr; MsgChA+=")=";
    for (int ArgValN=0; ArgValN<ArgValV.Len(); ArgValN++){
      if (ArgValN>0){MsgChA+=", ";}
      MsgChA+="'"; MsgChA+=ArgValV[ArgValN]; MsgChA+="'";
    }
    if (!SilentP){TNotify::OnStatus(Notify, MsgChA);}
    return ArgValV;
  }
}

bool TEnv::GetIfArgPrefixBool(
 const TStr& PrefixStr, const bool& DfVal, const TStr& DNm) const {
  if (Env.GetArgs()<=MnArgs){
    // 'usage' argument message
    if (!SilentP){
      printf("   %s%s (default:'%s')\n",
       PrefixStr.CStr(), DNm.CStr(), TBool::GetStr(DfVal).CStr());}
    return DfVal;
  } else {
    // argument & value message
    bool Val;
    if ((PrefixStr.Len()>0)&&(PrefixStr.LastCh()==':')){
      if (Env.IsArgPrefix(PrefixStr)){
        // try to find one of boolean string value representations
        TStr ValStr=Env.GetArgPostfix(PrefixStr);
        Val=TBool::GetValFromStr(ValStr, DfVal);
      } else {
        // remove ':' and try to find option
        TStr RedPrefixStr=PrefixStr;
        RedPrefixStr.DelSubStr(PrefixStr.Len()-1, PrefixStr.Len()-1);
        if (Env.IsArgPrefix(RedPrefixStr)){Val=true;} else {Val=DfVal;}
      }
    } else {
      if (Env.IsArgPrefix(PrefixStr)){Val=true;} else {Val=DfVal;}
    }
    TStr MsgStr=DNm+" ("+PrefixStr+")="+TBool::GetYesNoStr(Val);
    if (!SilentP){TNotify::OnStatus(Notify, MsgStr);}
    return Val;
  }
}

int TEnv::GetIfArgPrefixInt(
 const TStr& PrefixStr, const int& DfVal, const TStr& DNm) const {
  if (Env.GetArgs()<=MnArgs){
    // 'usage' argument message
    if (!SilentP){
      printf("   %s%s (default:%d)\n", PrefixStr.CStr(), DNm.CStr(), DfVal);}
    return DfVal;
  } else {
    // argument & value message
    int Val;
    if (Env.IsArgPrefix(PrefixStr)){
      TStr ValStr=Env.GetArgPostfix(PrefixStr);
      Val=ValStr.GetInt(DfVal);
    } else {
      Val=DfVal;
    }
    TStr MsgStr=DNm+" ("+PrefixStr+")="+TInt::GetStr(Val);
    if (!SilentP){TNotify::OnStatus(Notify, MsgStr);}
    return Val;
  }
}

TIntV TEnv::GetIfArgPrefixIntV(
 const TStr& PrefixStr, TIntV& DfValV, const TStr& DNm) const {
  // convert default-integer-values to default-string-values
  TStrV DfValStrV;
  for (int ValN=0; ValN<DfValV.Len(); ValN++){
    DfValStrV.Add(TInt::GetStr(DfValV[ValN]));}
  // get string-values
  TStrV ValStrV=GetIfArgPrefixStrV(PrefixStr, DfValStrV, DNm);
  // convert string-values to integer-values
  TIntV ValV;
  for (int ValN=0; ValN<ValStrV.Len(); ValN++){
    int Val;
    if (ValStrV[ValN].IsInt(Val)){
      ValV.Add(Val);}
  }
  // return value-vector
  return ValV;
}

double TEnv::GetIfArgPrefixFlt(
 const TStr& PrefixStr, const double& DfVal, const TStr& DNm) const {
  if (Env.GetArgs()<=MnArgs){
    // 'usage' argument message
    if (!SilentP){
      printf("   %s%s (default:%g)\n", PrefixStr.CStr(), DNm.CStr(), DfVal);}
    return DfVal;
  } else {
    // argument & value message
    double Val;
    if (Env.IsArgPrefix(PrefixStr)){
      TStr ValStr=Env.GetArgPostfix(PrefixStr);
      Val=ValStr.GetFlt(DfVal);
    } else {
      Val=DfVal;
    }
    TStr MsgStr=DNm+" ("+PrefixStr+")="+TFlt::GetStr(Val);
    if (!SilentP){TNotify::OnStatus(Notify, MsgStr);}
    return Val;
  }
}

void TEnv::GetVarNmV(TStrV& VarNmV){
  VarNmV.Clr(); int VarN=0;
  while (_environ[VarN]!=NULL){
    TStr VarNmVal=_environ[VarN++];
    TStr VarNm; TStr VarVal; VarNmVal.SplitOnCh(VarNm, '=', VarVal);
    VarNmV.Add(VarNm);
  }
}

void TEnv::GetVarNmValV(TStrV& VarNmValV){
  VarNmValV.Clr(); int VarN=0;
  while (_environ[VarN]!=NULL){
    VarNmValV.Add(_environ[VarN++]);
  }
}

void TEnv::PutVarVal(const TStr& VarNm, const TStr& VarVal) {
  const int RetVal = putenv(TStr::Fmt("%s=%s", VarNm.CStr(), VarVal.CStr()).CStr());
  IAssert(RetVal==0);
}

TStr TEnv::GetVarVal(const TStr& VarNm) const {
  return TStr(getenv(VarNm.CStr()));
}

TEnv Env;

