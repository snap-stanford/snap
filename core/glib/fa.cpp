/////////////////////////////////////////////////
// Includes
#include "fa.h"

/////////////////////////////////////////////////
// Finite-Automata-Transition
PFaTrans TFaTrans::LoadCustomXml(const PXmlTok& XmlTok){
  IAssert(XmlTok->IsTag("Trans"));
  // collect transition values
  TStr MsgNm=XmlTok->GetArgVal("Msg");
  TStr DstStateNm=XmlTok->GetArgVal("DstState");
  TStr ScriptStr;
  if (XmlTok->IsSubTag("Script")){
    ScriptStr=XmlTok->GetTagTok("Script")->GetTokStr(false);}
  // create transition
  PFaTrans Trans=TFaTrans::New(MsgNm, DstStateNm, ScriptStr);
  // return result
  return Trans;
}

void TFaTrans::_ChangeStateNm(const TStr& OldStateNm, const TStr& NewStateNm){
  if (SrcStateNm==OldStateNm){SrcStateNm=NewStateNm;}
  if (DstStateNm==OldStateNm){DstStateNm=NewStateNm;}
}

/////////////////////////////////////////////////
// Finite-Automata-State
PFaState TFaState::LoadCustomXml(const PXmlTok& XmlTok){
  IAssert(XmlTok->IsTag("State"));
  // collect state values
  // name
  TStr Nm=XmlTok->GetArgVal("Nm");
  // script
  TStr ScriptStr;
  if (XmlTok->IsSubTag("Script")){
    ScriptStr=XmlTok->GetTagTok("Script")->GetTokStr(false);}
  // transitions
  TXmlTokV TransTokV; XmlTok->GetTagTokV("Trans", TransTokV);
  TFaTransV TransV;
  for (int TransN=0; TransN<TransTokV.Len(); TransN++){
    PFaTrans Trans=TFaTrans::LoadCustomXml(TransTokV[TransN]);
    TransV.Add(Trans);
  }
  // create transition
  PFaState State=TFaState::New(Nm, ScriptStr);
  for (int TransN=0; TransN<TransV.Len(); TransN++){
    State->AddTrans(TransV[TransN]);}
  // return result
  return State;
}

void TFaState::_ChangeStateNm(const TStr& OldStateNm, const TStr& NewStateNm){
  if (Nm==OldStateNm){Nm=NewStateNm;}
  for (int TransN=0; TransN<FaTransV.Len(); TransN++){
    FaTransV[TransN]->_ChangeStateNm(OldStateNm, NewStateNm);}
}

bool TFaState::IsTransTo(const TStr& StateNm) const {
  for (int TransN=0; TransN<FaTransV.Len(); TransN++){
    if (FaTransV[TransN]->GetDstStateNm()==StateNm){return true;}
  }
  return false;
}

void TFaState::DelTrans(const PFaTrans& Trans){
  for (int TransN=FaTransV.Len()-1; TransN>=0; TransN--){
    if (FaTransV[TransN]()==Trans()){
      FaTransV.Del(TransN);}
  }
}

void TFaState::DelTransIfDstState(const TStr& DstStateNm){
  for (int TransN=FaTransV.Len()-1; TransN>=0; TransN--){
    if (FaTransV[TransN]->GetDstStateNm()==DstStateNm){
      FaTransV.Del(TransN);}
  }
}

/////////////////////////////////////////////////
// Finite-Automata
TStr TFaDef::GetNewStateNm() const {
  int StateN=0;
  while (IsState(TInt::GetStr(StateN, "State%d"))){StateN++;}
  return TInt::GetStr(StateN, "State%d");
}

void TFaDef::ChangeStateNm(const TStr& OldStateNm, const TStr& NewStateNm){
  // return if no change
  if (OldStateNm==NewStateNm){return;}
  // get state
  PFaState State=GetState(OldStateNm);
  // assert new name doesn't exist yet and is not start or end state
  IAssert(!IsState(NewStateNm));
  IAssert(State()!=GetStartState()());
  IAssert(State()!=GetEndState()());
  // change references to state in states and transitions
  TFaStateV StateV; GetStateV(StateV);
  for (int StateN=0; StateN<StateV.Len(); StateN++){
    StateV[StateN]->_ChangeStateNm(OldStateNm, NewStateNm);}
  for (int TransN=0; TransN<GlobalTransV.Len(); TransN++){
    GlobalTransV[TransN]->_ChangeStateNm(OldStateNm, NewStateNm);}
  // reinsert the state
  NmToFaStateH.DelKey(OldStateNm);
  NmToFaStateH.AddDat(State->GetNm(), State);
}

void TFaDef::DelState(const TStr& StateNm){
  // cannot delete start & end state
  IAssert(StateNm!=GetStartState()->GetNm());
  IAssert(StateNm!=GetEndState()->GetNm());
  // get state
  PFaState State=GetState(StateNm);
  // delete local state transitions
  TFaStateV StateV; GetStateV(StateV);
  for (int StateN=0; StateN<StateV.Len(); StateN++){
    StateV[StateN]->DelTransIfDstState(StateNm);}
  // delete global transitions
  for (int TransN=GlobalTransV.Len()-1; TransN>=0; TransN--){
    PFaTrans Trans=GlobalTransV[TransN];
    if ((Trans->GetSrcStateNm()==StateNm)||(Trans->GetDstStateNm()==StateNm)){
      GlobalTransV.Del(TransN);}
  }
  // delete state from hash table
  NmToFaStateH.DelKey(StateNm);
}

TStr TFaDef::GetStateNmAtXY(const double& X, const double& Y) const {
  TFaStateV StateV; GetStateV(StateV);
  for (int StateN=StateV.Len()-1; StateN>=0; StateN--){
    if (StateV[StateN]->GetRect().IsXYIn(X, Y)){
      return StateV[StateN]->GetNm();}
  }
  return "";
}

void TFaDef::DelTrans(const PFaTrans& Trans){
  // delete local state transition
  TFaStateV StateV; GetStateV(StateV);
  for (int StateN=0; StateN<StateV.Len(); StateN++){
    StateV[StateN]->DelTrans(Trans);}
  // delete global transitions
  for (int TransN=GlobalTransV.Len()-1; TransN>=0; TransN--){
    if (GlobalTransV[TransN]()==Trans()){
      GlobalTransV.Del(TransN);}
  }
}

PFaTrans TFaDef::GetTransAtXY(const double& X, const double& Y) const {
  // get states
  TFaStateV StateV; GetStateV(StateV);
  // traverse states
  for (int StateN=0; StateN<StateV.Len(); StateN++){
    // get state data
    PFaState SrcFaState=StateV[StateN];
    // traverse transitions
    for (int TransN=0; TransN<SrcFaState->GetTranss(); TransN++){
      // get transition
      PFaTrans FaTrans=SrcFaState->GetTrans(TransN);
      // check point being in the transition rectangle
      if (FaTrans->GetRect().IsXYIn(X, Y)){return FaTrans;}
    }
  }
  // no transition found
  return NULL;
}

const TStr TFaDef::FaDefVerStr="Automaton Definition / 09.03.2004";
const TStr TFaDef::DfFNm="Automaton.Xml";
const TStr TFaDef::FExt=".Xml";

PFaDef TFaDef::LoadBin(const TStr& FNm){
  PSIn SIn=TFIn::New(FNm);
  char* VerCStr;
  SIn->Load(VerCStr, FaDefVerStr.Len(), FaDefVerStr.Len());
  if (FaDefVerStr!=VerCStr){
    TExcept::Throw("Invalid version of Faulation Definition file.");}
  return TFaDef::Load(*SIn);
}

void TFaDef::SaveBin(const TStr& FNm) const {
  PSOut SOut=TFOut::New(FNm);
  SOut->Save(FaDefVerStr.CStr(), FaDefVerStr.Len());
  Save(*SOut);
}

PFaDef TFaDef::LoadXml(const TStr& FNm){
  PFaDef FaDef=TFaDef::New();
  XLoadFromFile(FNm, "FaDef", *FaDef);
  return FaDef;
}

void TFaDef::SaveXml(const TStr& FNm){
  PSOut SOut=TFOut::New(FNm);
  SaveXml(*SOut, "FaDef");
}

PFaDef TFaDef::LoadCustomXml(const TStr& FNm){
  // create finite automaton
  PFaDef FaDef=TFaDef::New();

  // load xml file
  PXmlDoc XmlDoc=TXmlDoc::LoadTxt(FNm);

  // global transitions
  TXmlTokV GlobalTransTokV;
  XmlDoc->GetTagTokV("FinAut|Trans", GlobalTransTokV);
  for (int TransN=0; TransN<GlobalTransTokV.Len(); TransN++){
    PFaTrans Trans=TFaTrans::LoadCustomXml(GlobalTransTokV[TransN]);
    FaDef->AddGlobalTrans(Trans);
  }

  // states
  TXmlTokV StateTokV; XmlDoc->GetTagTokV("FinAut|State", StateTokV);
  for (int StateN=0; StateN<StateTokV.Len(); StateN++){
    PFaState State=TFaState::LoadCustomXml(StateTokV[StateN]);
    FaDef->AddState(State);
  }

  // start & end state
  TStr StartStateNm=XmlDoc->GetTagVal("StartState", false);
  FaDef->PutStartState(FaDef->GetState(StartStateNm));
  TStr EndStateNm=XmlDoc->GetTagVal("EndState", false);
  FaDef->PutEndState(FaDef->GetState(EndStateNm));

  // return finite automaton
  return FaDef;
}

/////////////////////////////////////////////////
// Finite-Automata-Expression-Environment
void TFaExpEnv::PutVarVal(const TStr& VarNm, const PExpVal& ExpVal){
  VarNmToValH.AddDat(VarNm.GetUc(), ExpVal);
}

PExpVal TFaExpEnv::GetVarVal(const TStr& VarNm, bool& IsVar){
  int VarNmToValP;
  if (VarNmToValH.IsKey(VarNm.GetUc(), VarNmToValP)){
    IsVar=true; return VarNmToValH[VarNmToValP];
  } else {
    printf("Variable '%s' does not exist\n", VarNm.CStr());
    IsVar=false; return TExpVal::GetUndefExpVal();
  }
}

PExpVal TFaExpEnv::GetFuncVal(
 const TStr& FuncNm, const TExpValV& ArgValV, bool& IsFunc){
  IsFunc=true; PExpVal ExpVal=TExpVal::GetUndefExpVal();
  //printf("Env. function call %s/%d\n", FuncNm.CStr(), ArgValV.Len());
  if (TExpEnv::IsFuncOk("Assign", efatStrAny, FuncNm, ArgValV)){
    // assign value to variable
    TStr VarNm=ArgValV[0]->GetStrVal();
    PutVarVal(VarNm, ArgValV[1]);
  } else
  if (TExpEnv::IsFuncOk("OnProb", efatFlt, FuncNm, ArgValV)){
    // returns true if probability in (_Prb, _PrbUsed) fires
    double RqPrb=ArgValV[0]->GetFltVal();
    double Prb=GetVarVal("_Prb")->GetFltVal();
    double PrbUsed=GetVarVal("_PrbUsed")->GetFltVal();
    bool Ok=(PrbUsed<=Prb)&&(Prb<PrbUsed+RqPrb);
    PutVarVal("_PrbUsed", TExpVal::New(PrbUsed+RqPrb));
    ExpVal=TExpVal::New(double(Ok)); IsFunc=true;
  } else
  if (TExpEnv::IsFuncOk("AfterTime", efatFlt, FuncNm, ArgValV)){
    // returns true if in state longer then arg. number of secons
    double MxSecs=ArgValV[0]->GetFltVal();
    TSecTm StartStateTm=FaExe->GetActStateStartTm();
    bool Ok=TSecTm::GetDSecs(StartStateTm, TSecTm::GetCurTm())>MxSecs;
    ExpVal=TExpVal::New(double(Ok)); IsFunc=true;
  } else
  if (TExpEnv::IsFuncOk("SendMsg", efatStr, FuncNm, ArgValV)){
    // send message created from Arg0
    TStr MsgNm=ArgValV[0]->GetStrVal();
    PFaMsg Msg=TFaMsg::New(MsgNm);
    FaExe->PushMsg(Msg);
  } else
  if (TExpEnv::IsFuncOk("SaveMsgArg", efatStrFlt, FuncNm, ArgValV)){
    // saves message on the index Arg1 to the variable from Arg0
    TStr VarNm=ArgValV[0]->GetStrVal();
    int ArgN=ArgValV[1]->GetFltValAsInt()-1;
    PFaMsg Msg=FaExe->GetLastMsg();
    if ((!Msg.Empty())&&(0<=ArgN)&&(ArgN<Msg->GetArgs())){
      TStr MsgArgValStr=Msg->GetArgVal(ArgN);
      PExpVal ExpVal=TExpVal::New(MsgArgValStr);
      PutVarVal(VarNm, ExpVal);
      printf("Assign '%s'='%s'\n", VarNm.CStr(), MsgArgValStr.CStr());
    }
  } else
  if (TExpEnv::IsFuncOk("SaveMsgArg", efatStr, FuncNm, ArgValV)){
    // saves message on the index 1 to the variable from Arg0
    TStr VarNm=ArgValV[0]->GetStrVal();
    int ArgN=0;
    PFaMsg Msg=FaExe->GetLastMsg();
    if ((!Msg.Empty())&&(0<=ArgN)&&(ArgN<Msg->GetArgs())){
      TStr MsgArgValStr=Msg->GetArgVal(ArgN);
      PExpVal ExpVal=TExpVal::New(MsgArgValStr);
      PutVarVal(VarNm, ExpVal);
      printf("Assign '%s'='%s'\n", VarNm.CStr(), MsgArgValStr.CStr());
    }
  } else
  if (TExpEnv::IsFuncOk("PlayIntro", efatStr, FuncNm, ArgValV)){
    // play introduction for number in Arg0
    TStr CallNumStr=ArgValV[0]->GetStrVal();
    printf(".....Playing Intro for '%s'.....\n", CallNumStr.CStr());
  } else
  if (TExpEnv::IsFuncOk("PlayCm", efatStrStr, FuncNm, ArgValV)){
    // play command
    TStr CallNumStr=ArgValV[0]->GetStrVal();
    TStr CmNm=ArgValV[1]->GetStrVal();
    printf(".....Playing Command '%s' for '%s'.....\n",
     CmNm.CStr(), CallNumStr.CStr());
  } else {
    printf("Bad env. function call %s/%d\n", FuncNm.CStr(), ArgValV.Len());
    IsFunc=false;
  }
  return ExpVal;
}

/////////////////////////////////////////////////
// Finite-Automata-Execution
PExpVal TFaExe::EvalExpStr(const TStr& ExpStr){
  bool Ok; TStr MsgStr; bool DbgP; TStr DbgStr; PExpVal ExpVal;
  PExp Exp=TExp::LoadTxt(ExpStr, Ok, MsgStr);
  if (Ok){
    ExpVal=Exp->Eval(Ok, MsgStr, DbgP, DbgStr, ExpEnv);
    if (!Ok){Notify->OnNotify(ntErr, MsgStr);}
  } else {
    Notify->OnNotify(ntErr, MsgStr);
    ExpVal=TExpVal::GetUndefExpVal();
  }
  return ExpVal;
}

TFaExe::TFaExe(const PFaDef& _FaDef, const PNotify& _Notify):
  FaDef(_FaDef), Notify(_Notify),
  ActState(FaDef->GetStartState()), ActTrans(),
  ActStateStartTm(TSecTm::GetCurTm()), MsgQ(),
  ExpEnv(TFaExpEnv::New(this)),
  LastMsg(){
  Notify->OnNotify(ntInfo, TStr("Starting at state: ")+ActState->GetNm());
  // execute start-state script
  if (!ActState->GetScriptStr().Empty()){
    EvalExpStr(ActState->GetScriptStr());
  }
}

TFaExe::~TFaExe(){
  Notify->OnNotify(ntInfo, TStr("Terminating at state: ")+ActState->GetNm());
}

void TFaExe::ExeStep(){
  // get probability random
  double Prb=ExpEnv->GetRnd().GetUniDev();
  ExpEnv->PutVarVal("_Prb", TExpVal::New(Prb));
  ExpEnv->PutVarVal("_PrbUsed", TExpVal::New(0));
  // split transitions into conditions and empty
  TFaTransV EmptyTransV; TFaTransV CondTransV;
  for (int TransN=0; TransN<ActState->GetTranss(); TransN++){
    PFaTrans Trans=ActState->GetTrans(TransN);
    if (Trans->GetCondStr().GetTrunc().Empty()){EmptyTransV.Add(Trans);}
    else {CondTransV.Add(Trans);}
  }
  // evaluate true conditions
  PFaState NewActState; ActTrans=NULL;
  for (int CondTransN=0; CondTransN<CondTransV.Len(); CondTransN++){
    PFaTrans CondTrans=CondTransV[CondTransN];
    PExpVal ExpVal=EvalExpStr(CondTrans->GetCondStr());
    if (ExpVal->GetFltVal()!=0){
      ActTrans=CondTrans;
      TStr NewActStateNm=CondTransV[CondTransN]->GetDstStateNm();
      if (FaDef->IsState(NewActStateNm)){
        NewActState=FaDef->GetState(NewActStateNm);
      } else {
        Notify->OnNotify(ntErr, TStr("Invalid state name (")+NewActStateNm+")");
      }
      break;
    }
  }
  // if no conditions applied
  if (NewActState.Empty()){
    if (EmptyTransV.Len()>0){
      ActTrans=EmptyTransV[0];
      TStr NewActStateNm=ActTrans->GetDstStateNm();
      if (FaDef->IsState(NewActStateNm)){
        NewActState=FaDef->GetState(NewActStateNm);
      } else {
        Notify->OnNotify(ntErr, TStr("Invalid state name (")+NewActStateNm+")");
      }
    }
  }
  // get new active state and execute its script
  TStr OldStateNm=ActState->GetNm();
  if (!NewActState.Empty()){
    ActState=NewActState; ActStateStartTm=TSecTm::GetCurTm();
    if (!ActState->GetScriptStr().Empty()){
      EvalExpStr(ActState->GetScriptStr());
    }
    TStr NewStateNm=ActState->GetNm();
    Notify->OnNotify(ntInfo, TStr("Transition: ")+OldStateNm+" -> "+NewStateNm);
  }
}

void TFaExe::SendMsg(const PFaMsg& Msg){
  if (!Msg.Empty()){
    MsgQ.Push(Msg);}
  while ((ActState()!=FaDef->GetEndState()())&&(!MsgQ.Empty())){
    // process messages
    if (!MsgQ.Empty()){
      LastMsg=MsgQ.Top(); MsgQ.Pop();
      printf("Msg: '%s'\n", LastMsg->GetNm()());
      PFaTrans Trans;//**=FaDef->GetTrans(State, LastMsg);
      if (!Trans.Empty()){
        // execute transition script
        if (!Trans->GetScriptStr().Empty()){
          bool Ok; TStr MsgStr;
          TExp::LoadAndEvalExpL(Trans->GetScriptStr(), Ok, MsgStr, ExpEnv);
        }
        // get new state
        ActState=FaDef->GetState(Trans->GetDstStateNm());
        // execute state script
        if (!ActState->GetScriptStr().Empty()){
          bool Ok; TStr MsgStr;
          TExp::LoadAndEvalExpL(ActState->GetScriptStr(), Ok, MsgStr, ExpEnv);
        }
      } else {
        printf("No transition for Msg: '%s'\n", LastMsg->GetNm()());
      }
    }
    printf("State: '%s'\n", ActState->GetNm().CStr());
  }
}

