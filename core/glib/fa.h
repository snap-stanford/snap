#ifndef fa_h
#define fa_h

/////////////////////////////////////////////////
// Includes
#include "base.h"

/////////////////////////////////////////////////
// Finite-Automata-Message
ClassTP(TFaMsg, PFaMsg)//{
private:
  TStr Nm;
  TStrKdV ArgNmValV;
public:
  TFaMsg(const TStr& _Nm): Nm(_Nm), ArgNmValV(){}
  static PFaMsg New(const TStr& Nm){
    return PFaMsg(new TFaMsg(Nm));}
  static PFaMsg New(const TStr& Nm, const TStr& ArgNm, const TStr& ArgVal){
    PFaMsg Msg=new TFaMsg(Nm); Msg->AddArgNmVal(ArgNm, ArgVal); return Msg;}
  ~TFaMsg(){}
  TFaMsg(TSIn& SIn):
    Nm(SIn), ArgNmValV(SIn){}
  static PFaMsg Load(TSIn& SIn){return new TFaMsg(SIn);}
  void Save(TSOut& SOut) const {
    Nm.Save(SOut); ArgNmValV.Save(SOut);}
  void LoadXml(const PXmlTok& XmlTok, const TStr& _Nm){
    XLoadHd(_Nm); XLoad(Nm); XLoad(ArgNmValV);}
  void SaveXml(TSOut& SOut, const TStr& _Nm){
    XSaveHd(_Nm); XSave(Nm); XSave(ArgNmValV);}

  TFaMsg& operator=(const TFaMsg& FaMsg){
    if (this!=&FaMsg){Nm=FaMsg.Nm; ArgNmValV=FaMsg.ArgNmValV;}
    return *this;}

  // message name
  TStr GetNm() const {return Nm;}

  // message arguments
  int GetArgs() const {return ArgNmValV.Len();}
  TStr GetArgNmVa(const int& ArgN) const {return ArgNmValV[ArgN].Key;}
  TStr GetArgVal(const int& ArgN) const {return ArgNmValV[ArgN].Dat;}
  void AddArgVal(const TStr& ArgVal){
    ArgNmValV.Add(TStrKd(TInt::GetStr(1+GetArgs(), "Arg%d"), ArgVal));}
  void AddArgNmVal(const TStr& ArgNm, const TStr& ArgVal){
    ArgNmValV.Add(TStrKd(ArgNm, ArgVal));}
};
typedef TQQueue<PFaMsg> TFaMsgQ;

/////////////////////////////////////////////////
// Finite-Automata-Transition
ClassTPV(TFaTrans, PFaTrans, TFaTransV)//{
private:
  TStr SrcStateNm, DstStateNm;
  TStr CondStr;
  TStr ScriptStr;
  TFltRect Rect;
public:
  TFaTrans(){}
  TFaTrans(
   const TStr& _SrcStateNm, const TStr& _DstStateNm,
   const TStr& _CondStr, const TStr& _ScriptStr=TStr()):
    SrcStateNm(_SrcStateNm), DstStateNm(_DstStateNm),
    CondStr(_CondStr), ScriptStr(_ScriptStr), Rect(){}
  static PFaTrans New(
   const TStr& SrcStateNm, const TStr& DstStateNm,
   const TStr& CondStr, const TStr& ScriptStr=TStr()){
    return PFaTrans(new TFaTrans(
     SrcStateNm, DstStateNm, CondStr, ScriptStr));}
  ~TFaTrans(){}
  TFaTrans(TSIn& SIn):
    SrcStateNm(SIn), DstStateNm(SIn),
    CondStr(SIn), ScriptStr(SIn), Rect(SIn){}
  static PFaTrans Load(TSIn& SIn){return new TFaTrans(SIn);}
  void Save(TSOut& SOut) const {
    SrcStateNm.Save(SOut); DstStateNm.Save(SOut);
    CondStr.Save(SOut); ScriptStr.Save(SOut); Rect.Save(SOut);}
  static PFaTrans LoadCustomXml(const PXmlTok& XmlTok);
  void LoadXml(const PXmlTok& XmlTok, const TStr& Nm){
    XLoadHd(Nm);
    XLoad(SrcStateNm); XLoad(DstStateNm);
    XLoad(CondStr); XLoad(ScriptStr); XLoad(Rect);}
  void SaveXml(TSOut& SOut, const TStr& Nm){
    XSaveHd(Nm);
    XSave(SrcStateNm); XSave(DstStateNm);
    XSave(CondStr); XSave(ScriptStr); XSave(Rect);}

  TFaTrans& operator=(const TFaTrans& FaTrans){
    if (this!=&FaTrans){
      SrcStateNm=FaTrans.SrcStateNm; DstStateNm=FaTrans.DstStateNm;
      CondStr=FaTrans.CondStr; ScriptStr=FaTrans.ScriptStr; Rect=FaTrans.Rect;}
    return *this;}

  // comoponent retrieval
  TStr GetSrcStateNm() const {return SrcStateNm;}
  TStr GetDstStateNm() const {return DstStateNm;}
  void _ChangeStateNm(const TStr& OldStateNm, const TStr& NewStateNm);

  // messages & conditions
  void PutCondStr(const TStr& _CondStr){CondStr=_CondStr;}
  TStr GetCondStr() const {return CondStr;}

  // scripts
  void PutScriptStr(const TStr& _ScriptStr){ScriptStr=_ScriptStr;}
  TStr GetScriptStr() const {return ScriptStr;}

  // coordinates
  void PutRect(const TFltRect& _Rect){Rect=_Rect;}
  TFltRect GetRect() const {return Rect;}
};

/////////////////////////////////////////////////
// Finite-Automata-State
ClassTPV(TFaState, PFaState, TFaStateV)//{
private:
  TStr Nm;
  TStr ScriptStr;
  TFaTransV FaTransV;
  TFlt X, Y;
  TFltRect Rect;
public:
  TFaState(const TStr& _Nm=TStr(), const TStr& _ScriptStr=TStr()):
    Nm(_Nm), ScriptStr(_ScriptStr), FaTransV(),
    X(0), Y(0), Rect(){}
  static PFaState New(const TStr& Nm=TStr(), const TStr& ScriptStr=TStr()){
    return PFaState(new TFaState(Nm, ScriptStr));}
  ~TFaState(){}
  TFaState(TSIn& SIn):
    Nm(SIn), ScriptStr(SIn), FaTransV(SIn),
    X(SIn), Y(SIn), Rect(SIn){}
  static PFaState Load(TSIn& SIn){return new TFaState(SIn);}
  void Save(TSOut& SOut) const {
    Nm.Save(SOut); ScriptStr.Save(SOut); FaTransV.Save(SOut);
    X.Save(SOut); Y.Save(SOut); Rect.Save(SOut);}
  static PFaState LoadCustomXml(const PXmlTok& XmlTok);
  void LoadXml(const PXmlTok& XmlTok, const TStr& _Nm){
    XLoadHd(_Nm); XLoad(Nm); XLoad(ScriptStr); XLoad(FaTransV);
    XLoad(X); XLoad(Y); XLoad(Rect);}
  void SaveXml(TSOut& SOut, const TStr& _Nm){
    XSaveHd(_Nm); XSave(Nm); XSave(ScriptStr); XSave(FaTransV);
    XSave(X); XSave(Y); XSave(Rect);}

  TFaState& operator=(const TFaState& FaState){
    if (this!=&FaState){
      Nm=FaState.Nm; ScriptStr=FaState.ScriptStr; FaTransV=FaState.FaTransV;
      X=FaState.X; Y=FaState.Y; Rect=FaState.Rect;}
    return *this;}

  // component retrieval
  TStr GetNm() const {return Nm;}
  void _ChangeStateNm(const TStr& OldStateNm, const TStr& NewStateNm);

  // scripts
  void PutScriptStr(const TStr& _ScriptStr){ScriptStr=_ScriptStr;}
  TStr GetScriptStr() const {return ScriptStr;}

  // transitions
  int GetTranss() const {return FaTransV.Len();}
  PFaTrans GetTrans(const int& TransN) const {return FaTransV[TransN];}
  PFaTrans AddTrans(const PFaTrans& Trans){
    FaTransV.Add(Trans); return Trans;}
  bool IsTransTo(const TStr& StateNm) const;
  void DelTrans(const PFaTrans& Trans);
  void DelTransIfDstState(const TStr& DstStateNm);

  // coordinates
  void PutXY(const double& _X, const double& _Y){X=_X; Y=_Y;}
  double GetX() const {return X;}
  double GetY() const {return Y;}
  void PutRect(const TFltRect& _Rect){Rect=_Rect;}
  TFltRect GetRect() const {return Rect;}
};

/////////////////////////////////////////////////
// Finite-Automata-Definition
ClassTPV(TFaDef, PFaDef, TFaDefV)//{
private:
  TStr Nm;
  TStrKdV ParamNmValKdV;
  TStr StartStateNm, EndStateNm;
  THash<TStr, PFaState> NmToFaStateH;
  TFaTransV GlobalTransV;
public:
  TFaDef():
    Nm(), ParamNmValKdV(),
    StartStateNm(), EndStateNm(),
    NmToFaStateH(), GlobalTransV(){}
  static PFaDef New(){return PFaDef(new TFaDef());}
  ~TFaDef(){}
  TFaDef(TSIn& SIn):
    Nm(SIn), ParamNmValKdV(SIn),
    StartStateNm(SIn), EndStateNm(SIn),
    NmToFaStateH(SIn), GlobalTransV(SIn){}
  static PFaDef Load(TSIn& SIn){return new TFaDef(SIn);}
  void Save(TSOut& SOut) const {
    Nm.Save(SOut); ParamNmValKdV.Save(SOut);
    StartStateNm.Save(SOut); EndStateNm.Save(SOut);
    NmToFaStateH.Save(SOut); GlobalTransV.Save(SOut);}
  void LoadXml(const PXmlTok& XmlTok, const TStr& _Nm){
    XLoadHd(_Nm);
    XLoad(Nm); XLoad(ParamNmValKdV);
    XLoad(StartStateNm); XLoad(EndStateNm);
    XLoad(NmToFaStateH); XLoad(GlobalTransV);}
  void SaveXml(TSOut& SOut, const TStr& _Nm){
    XSaveHd(_Nm);
    XSave(Nm); XSave(ParamNmValKdV);
    XSave(StartStateNm); XSave(EndStateNm);
    XSave(NmToFaStateH); XSave(GlobalTransV);}

  TFaDef& operator=(const TFaDef& FaDef){
    if (this!=&FaDef){
      Nm=FaDef.Nm; ParamNmValKdV=FaDef.ParamNmValKdV;
      StartStateNm=FaDef.StartStateNm; EndStateNm=FaDef.EndStateNm;
      NmToFaStateH=FaDef.NmToFaStateH; GlobalTransV=FaDef.GlobalTransV;}
    return *this;}

  // global
  bool IsOk(TStr& MsgStr) const {MsgStr="Ok"; return true;}

  // name & parameters
  void PutNm(const TStr& _Nm){Nm=_Nm;}
  TStr GetNm() const {return Nm;}
  void PutParamNmValKdV(const TStrKdV& _ParamNmValKdV){ParamNmValKdV=_ParamNmValKdV;}
  int GetParams() const {return ParamNmValKdV.Len();}
  TStr GetParamNm(const int& ParamN) const {return ParamNmValKdV[ParamN].Key;}
  TStr GetParamVal(const int& ParamN) const {return ParamNmValKdV[ParamN].Dat;}

  // start & end states
  void PutStartState(const PFaState& State){
    IAssert(StartStateNm.Empty()); IAssert(IsState(State->GetNm()));
    StartStateNm=State->GetNm();}
  PFaState GetStartState() const {
    return NmToFaStateH.GetDat(StartStateNm);}
  void PutEndState(const PFaState& State){
    IAssert(EndStateNm.Empty()); IAssert(IsState(State->GetNm()));
    EndStateNm=State->GetNm();}
  PFaState GetEndState() const {
    return NmToFaStateH.GetDat(EndStateNm);}

  // states
  bool IsState(const TStr& StateNm) const {
    return NmToFaStateH.IsKey(StateNm);}
  TStr GetNewStateNm() const;
  void ChangeStateNm(const TStr& OldStateNm, const TStr& NewStateNm);
  PFaState AddState(const PFaState& State){
    IAssert(!IsState(State->GetNm()));
    NmToFaStateH.AddDat(State->GetNm(), State); return State;}
  void DelState(const TStr& StateNm);
  PFaState GetState(const TStr& StateNm){
    return NmToFaStateH.GetDat(StateNm);}
  void GetStateV(TFaStateV& StateV) const {
    return NmToFaStateH.GetDatV(StateV);}
  TStr GetStateNmAtXY(const double& X, const double& Y) const;

  // global transitions
  int GetGlobalTranss() const {return GlobalTransV.Len();}
  PFaTrans GetGlobalTrans(const int& TransN) const {
    return GlobalTransV[TransN];}
  PFaTrans AddGlobalTrans(const PFaTrans& Trans){
    GlobalTransV.Add(Trans); return Trans;}

  // transitions
  PFaTrans GetTrans(const PFaState& State, const PFaMsg& Msg);
  void DelTrans(const PFaTrans& Trans);
  PFaTrans GetTransAtXY(const double& X, const double& Y) const;

  // files
  static const TStr FaDefVerStr;
  static const TStr DfFNm;
  static const TStr FExt;
  static PFaDef LoadBin(const TStr& FNm);
  void SaveBin(const TStr& FNm) const;
  static PFaDef LoadXml(const TStr& FNm);
  void SaveXml(const TStr& FNm);
  static PFaDef LoadCustomXml(const TStr& FNm);
  static PFaDef LoadFile(const TStr& FNm){return LoadXml(FNm);}
  void SaveFile(const TStr& FNm){SaveXml(FNm);}
};

/////////////////////////////////////////////////
// Finite-Automata-Expression-Environment
class TFaExe;

class TFaExpEnv: public TExpEnv{
private:
  THash<TStr, PExpVal> VarNmToValH;
  TFaExe* FaExe;
public:
  TFaExpEnv(TFaExe* _FaExe):
    VarNmToValH(), FaExe(_FaExe){}
  static PExpEnv New(TFaExe* FaExe){
    return PExpEnv(new TFaExpEnv(FaExe));}
  ~TFaExpEnv(){}

  void PutVarVal(const TStr& VarNm, const PExpVal& ExpVal);
  PExpVal GetVarVal(const TStr& VarNm, bool& IsVar);
  PExpVal GetVarVal(const TStr& VarNm){
    bool IsVar; return GetVarVal(VarNm, IsVar);}
  int GetVars() const {return VarNmToValH.Len();}
  void GetVarNmVal(const int& VarN, TStr& VarNm, PExpVal& VarVal) const {
    VarNm=VarNmToValH.GetKey(VarN); VarVal=VarNmToValH[VarN];}

  PExpVal GetFuncVal(
   const TStr& FuncNm, const TExpValV& ArgValV, bool& IsFunc);
};

/////////////////////////////////////////////////
// Finite-Automata-Execution
ClassTPV(TFaExe, PFaExe, TFaExeV)//{
private:
  PFaDef FaDef;
  PNotify Notify;
  PFaState ActState;
  PFaTrans ActTrans;
  TSecTm ActStateStartTm;
  TFaMsgQ MsgQ;
  PExpEnv ExpEnv;
  PFaMsg LastMsg;
  PExpVal EvalExpStr(const TStr& ExpStr);
  UndefDefaultCopyAssign(TFaExe);
public:
  TFaExe(const PFaDef& _Fa, const PNotify& _Notify);
  static PFaExe New(const PFaDef& FaDef, const PNotify& Notify){
    return PFaExe(new TFaExe(FaDef, Notify));}
  ~TFaExe();
  TFaExe(TSIn& SIn){}
  static PFaExe Load(TSIn& SIn){return new TFaExe(SIn);}
  void Save(TSOut& SOut) const {}

  // component retrieval
  PFaDef GetFaDef() const {return FaDef;}
  PNotify GetNotify() const {return Notify;}
  PFaState GetActState() const {return ActState;}
  PFaTrans GetActTrans() const {return ActTrans;}
  TSecTm GetActStateStartTm() const {return ActStateStartTm;}
  PFaMsg GetLastMsg() const {return LastMsg;}

  // execution
  void ExeStep();
  int GetVars() const {return ExpEnv->GetVars();}
  void GetVarNmVal(const int& VarN, TStr& VarNm, PExpVal& VarVal) const {
    ExpEnv->GetVarNmVal(VarN, VarNm, VarVal);}

  // message handling
  void PushMsg(const PFaMsg& Msg){MsgQ.Push(Msg);}
  void SendMsg(const PFaMsg& Msg=NULL);
  void SendMsg(const TStr& MsgNm){
    SendMsg(TFaMsg::New(MsgNm));}
  void SendMsg(const TStr& MsgNm, const TStr& ArgNm, const TStr& ArgVal){
    SendMsg(TFaMsg::New(MsgNm, ArgNm, ArgVal));}
  void SendMsg(const TStr& MsgNm, const TStr& ArgVal){
    SendMsg(TFaMsg::New(MsgNm, "", ArgVal));}
};

#endif

