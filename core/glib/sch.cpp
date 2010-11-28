/////////////////////////////////////////////////
// Includes
#include "sch.h"

/////////////////////////////////////////////////
// Schedule-Task
TSchTask::TSchTask(
 const int& _TaskId, const TVec<TStrV>& _DimObjVV,
 const TSecTm& _StartTm, const TSecTm& _EndTm, const bool& _Forced):
  TaskId(_TaskId), DimObjVV(_DimObjVV),
  StartTm(_StartTm), EndTm(_EndTm), Forced(_Forced){
  IAssert(StartTm<EndTm);
}

TSchTask::TSchTask(
 const int& _TaskId, const TStrV& DimObjV,
 const TSecTm& _StartTm, const TSecTm& _EndTm, const bool& _Forced):
  TaskId(_TaskId), DimObjVV(),
  StartTm(_StartTm), EndTm(_EndTm), Forced(_Forced){
  IAssert(StartTm<EndTm);
  for (int DimObjN=0; DimObjN<DimObjV.Len(); DimObjN++){
    DimObjVV.Add(); DimObjVV.Last().Add(DimObjV[DimObjN]);
  }
}

TStr TSchTask::GetAllDimObjStr(const int& DimN) const {
  TChA ChA;
  for (int ObjN=0; ObjN<GetDimObjs(DimN); ObjN++){
    if (ObjN>0){ChA+=", ";}
    ChA+=GetDimObj(DimN, ObjN);
  }
  return ChA;
}

TStr TSchTask::GetStr() const {
  TChA ChA;
  if (Forced){ChA+="F:";}// else {ChA+="R:";}
  for (int DimN=0; DimN<GetDims(); DimN++){
    if (DimN>0){ChA+=' ';}
    ChA+='[';
    for (int ObjN=0; ObjN<GetDimObjs(DimN); ObjN++){
      if (ObjN>0){ChA+=' ';}
      ChA+=DimObjVV[DimN][ObjN];
    }
    ChA+=']';
  }
  return ChA;
}

bool TSchTask::IsCons(const PSchTask& Task1, const PSchTask& Task2){
  IAssert((!Task1->IsForced())&&(!Task2->IsForced()));
  return
   (Task1->GetEndTm()<=Task2->GetStartTm())||
   (Task2->GetEndTm()<=Task1->GetStartTm());
}

PSchTask TSchTask::LoadTxt(TILx& Lx){
  PSchTask SchTask=PSchTask(new TSchTask());
  Lx.GetVar("SchTask", true, true);
  SchTask->TaskId=Lx.GetVarInt("TaskId");
  Lx.GetVarStrVV("DimObjVV", SchTask->DimObjVV);
  SchTask->StartTm=Lx.GetVarSecTm("StartTm");
  SchTask->EndTm=Lx.GetVarSecTm("EndTm");
  SchTask->Forced=Lx.GetVarBool("Forced");
  Lx.GetVarEnd(true, true);
  return SchTask;
}

void TSchTask::SaveTxt(TOLx& Lx) const {
  Lx.PutVar("SchTask", true, true);
  Lx.PutVarInt("TaskId", TaskId);
  Lx.PutVarStrVV("DimObjVV", DimObjVV);
  Lx.PutVarSecTm("StartTm", StartTm);
  Lx.PutVarSecTm("EndTm", EndTm);
  Lx.PutVarBool("Forced", Forced);
  Lx.PutVarEnd(true, true);
}

/////////////////////////////////////////////////
// Schedule-Object
void TSchObj::AddTask(const PSchTask& Task, const bool& OverlapCheckP){
  IAssert(IsTaskOk(Task, OverlapCheckP));
  if (Task->IsForced()){
    FrcTaskV.Add(Task);
  } else {
    TTmTaskKd TmTaskKd(Task->GetStartTm(), Task);
    int TaskN=TmTaskKdV.AddSorted(TmTaskKd);
    if (0<TaskN){
      PSchTask PrevTask=TmTaskKdV[TaskN-1].Dat;
      if (OverlapCheckP){IAssert(TSchTask::IsCons(PrevTask, Task));}
    }
    if (TaskN+1<TmTaskKdV.Len()){
      PSchTask NextTask=TmTaskKdV[TaskN+1].Dat;
      if (OverlapCheckP){IAssert(TSchTask::IsCons(Task, NextTask));}
    }
  }
}

void TSchObj::DelTask(const PSchTask& Task){
  if (Task->IsForced()){
    int FrcTaskN=0;
    while ((FrcTaskN<FrcTaskV.Len())&&
     (FrcTaskV[FrcTaskN]->GetTaskId()!=Task->GetTaskId())){FrcTaskN++;}
    IAssert(FrcTaskN<FrcTaskV.Len());
    FrcTaskV.Del(FrcTaskN);
  } else {
    TTmTaskKd TmTaskKd(Task->GetStartTm(), Task);
    int TaskN=TmTaskKdV.SearchBin(TmTaskKd);
    IAssert(TaskN!=-1);
    IAssert(Task->GetTaskId()==TmTaskKdV[TaskN].Dat->GetTaskId());
    TmTaskKdV.Del(TaskN);
  }
}

void TSchObj::GetTaskV(TSchTaskV& TaskV){
  TaskV.Clr();
  for (int TaskN=0; TaskN<GetTasks(); TaskN++){
    TaskV.Add(GetTask(TaskN));}
}

int TSchObj::GetTasks() const {
  return TmTaskKdV.Len()+FrcTaskV.Len();
}

PSchTask TSchObj::GetTask(const int& TaskN) const {
  if (TaskN<TmTaskKdV.Len()){
    return TmTaskKdV[TaskN].Dat;
  } else {
    return FrcTaskV[TaskN-TmTaskKdV.Len()];
  }
}

bool TSchObj::IsRegTaskAtTm(const TSecTm& Tm, PSchTask& Task) const {
  TTmTaskKd TmTaskKd(Tm);
  int InsTaskN; int TaskN=TmTaskKdV.SearchBin(TmTaskKd, InsTaskN);
  if (TaskN==-1){
    if (InsTaskN==0){
      Task=NULL; return false;
    } else
    if (TmTaskKdV[InsTaskN-1].Dat->IsTmIn(Tm)){
      Task=TmTaskKdV[InsTaskN-1].Dat; return true;
    } else {
      Task=NULL; return false;
    }
  } else {
    Task=TmTaskKdV[TaskN].Dat; return true;
  }
}

bool TSchObj::IsRegTaskInTm(
 const TSecTm& MnTm, const TSecTm& MxTm, PSchTask& Task) const {
  TTmTaskKd StartTmTaskKd(MnTm);
  TTmTaskKd EndTmTaskKd(MxTm);
  int StartInsTaskN; int EndInsTaskN;
  int StartTaskN=TmTaskKdV.SearchBin(StartTmTaskKd, StartInsTaskN);
  int EndTaskN=TmTaskKdV.SearchBin(EndTmTaskKd, EndInsTaskN);
  if ((StartTaskN==-1)&&(EndTaskN==-1)){
    if (StartInsTaskN==EndInsTaskN){
      if (StartInsTaskN==0){
        Task=NULL; return false;
      } else
      if (TmTaskKdV[StartInsTaskN-1].Dat->IsTmIn(MnTm)){
        Task=TmTaskKdV[StartInsTaskN-1].Dat; return true;
      } else {
        Task=NULL; return false;
      }
    } else {
      Task=TmTaskKdV[StartInsTaskN].Dat; return true;
    }
  } else {
    if (StartTaskN!=-1){IsRegTaskAtTm(MnTm, Task);}
    else if (EndTaskN!=-1){IsRegTaskAtTm(MxTm, Task);}
    else {Fail;}
    IAssert(!Task.Empty());
    return true;
  }
}

bool TSchObj::IsFrcTaskInTm(
 const TSecTm& MnTm, const TSecTm& MxTm, PSchTask& Task) const {
  for (int FrcTaskN=0; FrcTaskN<FrcTaskV.Len(); FrcTaskN++){
    if (FrcTaskV[FrcTaskN]->IsTmInts(MnTm, MxTm)){
      Task=FrcTaskV[FrcTaskN]; return true;
    }
  }
  return false;
}

bool TSchObj::IsTaskInTm(
 const TSecTm& MnTm, const TSecTm& MxTm, PSchTask& Task) const {
  return IsRegTaskInTm(MnTm, MxTm, Task) || IsFrcTaskInTm(MnTm, MxTm, Task);
}

PSchObj TSchObj::LoadTxt(TILx& Lx){
  PSchObj SchObj=PSchObj(new TSchObj());
  Lx.GetVar("SchObj", true, true);
  // Nm
  SchObj->Nm=Lx.GetVarStr("Nm");
  // TmTaskKdV
  Lx.GetVar("TmTaskKdV", true, true);
  while (!Lx.PeekVarEnd(true, true)){
    PSchTask Task=TSchTask::LoadTxt(Lx);
    SchObj->TmTaskKdV.Add(TTmTaskKd(Task->GetStartTm(), Task));
  }
  Lx.GetVarEnd(true, true);
  // FrcTaskV
  Lx.GetVar("FrcTaskV", true, true);
  while (!Lx.PeekVarEnd(true, true)){
    PSchTask Task=TSchTask::LoadTxt(Lx);
    SchObj->FrcTaskV.Add(Task);
  }
  Lx.GetVarEnd(true, true);
  Lx.GetVarEnd(true, true);
  return SchObj;
}

void TSchObj::SaveTxt(TOLx& Lx) const {
  Lx.PutVar("SchObj", true, true);
  // Nm
  Lx.PutVarStr("Nm", Nm);
  // TmTaskKdV
  Lx.PutVar("TmTaskKdV", true, true);
  for (int TaskN=0; TaskN<TmTaskKdV.Len(); TaskN++){
    TmTaskKdV[TaskN].Dat->SaveTxt(Lx);}
  Lx.PutVarEnd(true, true);
  // FrcTaskV
  Lx.PutVar("FrcTaskV", true, true);
  for (int FrcTaskN=0; FrcTaskN<FrcTaskV.Len(); FrcTaskN++){
    FrcTaskV[FrcTaskN]->SaveTxt(Lx);}
  Lx.PutVarEnd(true, true);
  Lx.PutVarEnd(true, true);
}

/////////////////////////////////////////////////
// Schedule-Dimension
void TSchDim::AddObj(const TStr& ObjNm){
  IAssert((!IsObj(ObjNm))&&(!IsCObj(ObjNm)));
  PSchObj Obj=PSchObj(new TSchObj(ObjNm));
  NmToObjH.AddDat(ObjNm, Obj);
  NmToObjH.GetKeyId(ObjNm);
}

TStrV TSchDim::GetObjNmV() const {
  TStrV ObjNmV(NmToObjH.Len(), 0);
  int NmToObjP=NmToObjH.FFirstKeyId();
  while (NmToObjH.FNextKeyId(NmToObjP)){
    ObjNmV.Add(NmToObjH[NmToObjP]->GetNm());}
  ObjNmV.Sort();
  return ObjNmV;
}

int TSchDim::AddCObj(const TStr& CObjNm, const TStrV& ObjNmV){
  IAssert((!IsObj(CObjNm))&&(!IsCObj(CObjNm)));
  for (int ObjNmN=0; ObjNmN<ObjNmV.Len(); ObjNmN++){
    IAssert(IsObj(ObjNmV[ObjNmN]));}
  CObjNmToObjNmVH.AddDat(CObjNm, ObjNmV);
  return NmToObjH.GetKeyId(CObjNm);
}

void TSchDim::GetCObjFromDesc(
 const TStr& DescStr, TStr& CObjNm, TStrV& ObjNmV) const {
  if (DescStr.IsChIn(':')){
    TStr ObjNmVStr; DescStr.SplitOnCh(CObjNm, ':', ObjNmVStr);
    CObjNm=CObjNm.GetTrunc(); ObjNmV.Clr();
    TChA ObjNm;
    for (int ChN=0; ChN<=ObjNmVStr.Len(); ChN++){
      if ((ChN==ObjNmVStr.Len())||(ObjNmVStr[ChN]==',')){
        ObjNm.Trunc();
        if (!ObjNm.Empty()){ObjNmV.Add(ObjNm);}
        ObjNm.Clr();
      } else {
        ObjNm+=ObjNmVStr[ChN];
      }
    }
    ObjNmV.Sort();
  } else {
    CObjNm=DescStr.GetTrunc(); ObjNmV.Clr();
  }
}

TStrV TSchDim::GetAllObjDescV() const {
  TStrV DescStrV;
  DescStrV.AddV(GetObjNmV());
  TStrV CObjNmV=GetCObjNmV();
  for (int CObjNmN=0; CObjNmN<CObjNmV.Len(); CObjNmN++){
    TStrV ObjNmV=GetCObj_ObjNmV(CObjNmV[CObjNmN]);
    TChA DescChA; DescChA+=CObjNmV[CObjNmN]; DescChA+=':';
    for (int ObjNmN=0; ObjNmN<ObjNmV.Len(); ObjNmN++){
      if (ObjNmN>0){DescChA+=", ";} DescChA+=ObjNmV[ObjNmN];}
    DescStrV.Add(DescChA);
  }
  DescStrV.Sort();
  return DescStrV;
}

void TSchDim::AddTask(const PSchTask& Task){
  for (int DimObjN=0; DimObjN<Task->GetDimObjs(DimN); DimObjN++){
    PSchObj Obj=GetObj(Task->GetDimObj(DimN, DimObjN));
    Obj->AddTask(Task, OverlapCheckP);
  }
}

void TSchDim::DelTask(const PSchTask& Task){
  for (int DimObjN=0; DimObjN<Task->GetDimObjs(DimN); DimObjN++){
    PSchObj Obj=GetObj(Task->GetDimObj(DimN, DimObjN));
    Obj->DelTask(Task);
  }
}

bool TSchDim::IsTaskOk(const PSchTask& Task, PSchTask& IcTask) const {
  for (int DimObjN=0; DimObjN<Task->GetDimObjs(DimN); DimObjN++){
    PSchObj Obj=GetObj(Task->GetDimObj(DimN, DimObjN));
    if (!Obj->IsTaskOk(Task, IcTask, OverlapCheckP)){return false;}
  }
  return true;
}

bool TSchDim::IsTaskInTm(const TStr& ObjNm,
 const TSecTm& MnTm, const TSecTm& MxTm, PSchTask& Task) const {
  PSchObj Obj=GetObj(ObjNm);
  return Obj->IsTaskInTm(MnTm, MxTm, Task);
}

PSchDim TSchDim::LoadTxt(TILx& Lx){
  PSchDim SchDim=PSchDim(new TSchDim());
  Lx.GetVar("SchDim", true, true);
  // DimN
  SchDim->DimN=Lx.GetVarInt("DimN");
  // Nm
  SchDim->Nm=Lx.GetVarStr("Nm");
  // ConsCheckP
  SchDim->OverlapCheckP=Lx.GetVarBool("OverlapCheckP");
  // NmToObjH
  Lx.GetVar("NmToObjH", true, true);
  int NmToObjH_Ports=Lx.GetVarInt("Ports");
  SchDim->NmToObjH=TNmToObjH(NmToObjH_Ports);
  while (!Lx.PeekVarEnd(true, true)){
    PSchObj Obj=TSchObj::LoadTxt(Lx);
    SchDim->NmToObjH.AddDat(Obj->GetNm(), Obj);
  }
  Lx.GetVarEnd(true, true);
  // CObjNmToObjNmVH
  Lx.GetVar("CObjNmToObjNmVH", true, true);
  int CObjNmToObjNmVH_Ports=Lx.GetVarInt("Ports");
  SchDim->CObjNmToObjNmVH=TStrStrVH(CObjNmToObjNmVH_Ports);
  while (!Lx.PeekVarEnd(true, true)){
    Lx.GetVar("CObj", true, true);
    TStr CObjNm=Lx.GetVarStr("CObjNm");
    TStrV ObjNmV; Lx.GetVarStrV("ObjNmV", ObjNmV);
    SchDim->CObjNmToObjNmVH.AddDat(CObjNm, ObjNmV);
    Lx.GetVarEnd(true, true);
  }
  Lx.GetVarEnd(true, true);
  Lx.GetVarEnd(true, true);
  return SchDim;
}

void TSchDim::SaveTxt(TOLx& Lx) const {
  Lx.PutVar("SchDim", true, true);
  // DimN
  Lx.PutVarInt("DimN", DimN);
  // Nm
  Lx.PutVarStr("Nm", Nm);
  // OverlapCheckP
  Lx.PutVarBool("OverlapCheckP", OverlapCheckP);
  // NmToObjH
  Lx.PutVar("NmToObjH", true, true);
  int NmToObjP=NmToObjH.FFirstKeyId();
  Lx.PutVarInt("Ports", NmToObjH.GetPorts());
  while (NmToObjH.FNextKeyId(NmToObjP)){
    NmToObjH[NmToObjP]->SaveTxt(Lx);}
  Lx.PutVarEnd(true, true);
  // CObjNmToObjNmVH
  Lx.PutVar("CObjNmToObjNmVH", true, true);
  int CObjNmToObjNmVP=CObjNmToObjNmVH.FFirstKeyId();
  Lx.PutVarInt("Ports", CObjNmToObjNmVH.GetPorts());
  while (CObjNmToObjNmVH.FNextKeyId(CObjNmToObjNmVP)){
    Lx.PutVar("CObj", true, true);
    Lx.PutVarStr("CObjNm", CObjNmToObjNmVH.GetKey(CObjNmToObjNmVP));
    Lx.PutVarStrV("ObjNmV", CObjNmToObjNmVH[CObjNmToObjNmVP]);
    Lx.PutVarEnd(true, true);
  }
  Lx.PutVarEnd(true, true);
  Lx.PutVarEnd(true, true);
}

/////////////////////////////////////////////////
// Schedule-Report
PSchRep TSchRep::New(
 const PSch& Sch, const TStr& DimNm, const TStr& ObjNm, const int& ResMins,
 const bool& AllTmP, const TSecTm& StartDtTm, const TSecTm& EndDtTm){
  // create result
  PSchRep SchRep=PSchRep(new TSchRep());

  // generate day times at given resolution
  TSecTm EndDayTm=TSecTm::GetZeroTm().AddDays(1);
  {TSecTm CurDayTm=TSecTm::GetZeroTm();
  while (CurDayTm<EndDayTm){
    SchRep->DayTmV.Add(CurDayTm); CurDayTm.AddMins(ResMins);}}
  // normalize start & end dates
  TSecTm NrStartDtTm; TSecTm NrEndDtTm;
  if (AllTmP){
    NrStartDtTm=Sch->GetCal()->GetStartTm();
    NrEndDtTm=Sch->GetCal()->GetEndTm();
  } else {
    NrStartDtTm=StartDtTm; NrEndDtTm=EndDtTm;
  }
  if (NrStartDtTm>NrEndDtTm){NrEndDtTm=NrStartDtTm;}
  if (NrStartDtTm<Sch->GetCal()->GetStartTm()){
    NrStartDtTm=Sch->GetCal()->GetStartTm();}
  if (NrEndDtTm>Sch->GetCal()->GetEndTm()){
    NrEndDtTm=Sch->GetCal()->GetEndTm();}

  // generate data for all days
  TSecTm CurDtTm=NrStartDtTm;
  PSchTask PrevTask; int TaskN=0-1; int TaskSegN=-1;
  while (CurDtTm<NrEndDtTm){
    SchRep->DtTmV.Add(CurDtTm);
    SchRep->TaskNmVV.Add();
    TSecTm StartTaskTm=CurDtTm;
    TSecTm EndTaskTm=TSecTm(StartTaskTm).AddMins(ResMins);
    for (int DayTmN=0; DayTmN<SchRep->DayTmV.Len(); DayTmN++){
      PSchTask Task;
      if (Sch->IsTaskInTm(DimNm, ObjNm, StartTaskTm, EndTaskTm-1, Task)){
        if (PrevTask==Task){TaskSegN++;} else {TaskN++; TaskSegN=1;}
        TStr TaskStr=Task->GetStr()+" ("+TInt::GetStr(TaskSegN)+")";
        SchRep->TaskNmVV.Last().Add(TaskStr);
      } else {
        SchRep->TaskNmVV.Last().Add();
      }
      PrevTask=Task;
      StartTaskTm.AddMins(ResMins); EndTaskTm.AddMins(ResMins);
    }
    CurDtTm.AddDays(1);
  }

  // return result
  return SchRep;
}

void TSchRep::SaveHtml(const PSOut& SOut) const {
  int CurDtTmN=0;
  while (CurDtTmN<GetDtTms()){
    // determine first & last day in current table
    int MnDtTmN=CurDtTmN;
    while ((CurDtTmN<GetDtTms())&&
     (GetDtTm(CurDtTmN).GetDayOfWeekN()!=TTmInfo::SunN)){CurDtTmN++;}
    int MxDtTmN=CurDtTmN;
    CurDtTmN++;

    SOut->PutStr("<table border=1 cellpadding=3>\n");
    // header line
    SOut->PutStr("<tr>");
    SOut->PutStr("<td>");
    SOut->PutStr("Time");
    SOut->PutStr("</td>");
    for (int DtTmN=MnDtTmN; DtTmN<MxDtTmN; DtTmN++){
      SOut->PutStr("<td>");
      SOut->PutStr(GetDtTmStr(DtTmN));
      SOut->PutStr("</td>");
    }
    SOut->PutStr("</tr>"); SOut->PutLn();
    // data lines
    for (int DayTmN=0; DayTmN<GetDayTms(); DayTmN++){
      SOut->PutStr("</tr>");
      SOut->PutStr("<td>");
      SOut->PutStr(GetDayTmStr(DayTmN));
      SOut->PutStr("</td>");
      for (int DtTmN=MnDtTmN; DtTmN<MxDtTmN; DtTmN++){
        SOut->PutStr("<td>");
        if (GetTaskStr(DtTmN, DayTmN).Empty()){
          SOut->PutStr("&nbsp;");
        } else {
          SOut->PutStr(GetTaskStr(DtTmN, DayTmN));
        }
        SOut->PutStr("</td>");
      }
      SOut->PutStr("</tr>"); SOut->PutLn();
    }
    SOut->PutStr("</table>\n");
  }
}

void TSchRep::SaveHtml(const TStr& FNm) const {
  PSOut SOut=TFOut::New(FNm);
  SOut->PutStr("<html>\n");
  SOut->PutStr("<head><title>Urnik</title></head>\n");
  SOut->PutStr("<body>\n");
  SaveHtml(SOut);
  SOut->PutStr("</body>\n");
  SOut->PutStr("</html>\n");
}

void TSchRep::SaveTxt(const PSOut& SOut) const {
  // header line
  SOut->PutStr("Time");
  for (int DtTmN=0; DtTmN<GetDtTms(); DtTmN++){
    SOut->PutCh(TabCh); SOut->PutStr(GetDtTmStr(DtTmN));}
  SOut->PutLn();
  // data lines
  for (int DayTmN=0; DayTmN<GetDayTms(); DayTmN++){
    SOut->PutStr(GetDayTmStr(DayTmN));
    for (int DtTmN=0; DtTmN<GetDtTms(); DtTmN++){
      SOut->PutCh(TabCh); SOut->PutStr(GetTaskStr(DtTmN, DayTmN));}
    SOut->PutLn();
  }
}

void TSchRep::SaveTxt(const TStr& FNm) const {
  PSOut SOut=TFOut::New(FNm);
  SaveTxt(SOut);
}

void TSchRep::SaveStat(const TStr& FNm, const PSch& Sch){
  PSOut SOut=TFOut::New(FNm);
  TStr StartTmStr=Sch->GetCal()->GetStartTm().GetDtStr(lSi);
  TStr EndTmStr=Sch->GetCal()->GetEndTm().GetDtStr(lSi);
  SOut->PutStr("Zacetek: "); SOut->PutStr(StartTmStr); SOut->PutLn();
  SOut->PutStr("Konec: "); SOut->PutStr(EndTmStr); SOut->PutLn();
  for (int DimN=0; DimN<Sch->GetDims(); DimN++){
    SOut->PutStr("----------------------"); SOut->PutLn();
    SOut->PutStr(TStr("Dimenzija: ")+Sch->GetDimNm(DimN)); SOut->PutLn();
    TStrV ObjNmV=Sch->GetObjNmV(DimN);
    for (int ObjNmN=0; ObjNmN<ObjNmV.Len(); ObjNmN++){
      TStr ObjNm=ObjNmV[ObjNmN];
      SOut->PutStr(ObjNm+": ");
      TSchTaskV TaskV; Sch->GetObjTaskV(DimN, ObjNm, TaskV);
      int ObjSecs=0;
      for (int TaskN=0; TaskN<TaskV.Len(); TaskN++){
        ObjSecs+=TaskV[TaskN]->GetTmSecs();}
      SOut->PutStr(TInt::GetStr(ObjSecs/3600)+"h ");
      SOut->PutStr(TInt::GetStr((ObjSecs%3600)/60)+"m ");
      SOut->PutStr(TInt::GetStr((ObjSecs%3600*60)/60)+"s");
      SOut->PutLn();
    }
  }
}

/////////////////////////////////////////////////
// Schedule-Calendar
void TSchCal::PutScope(const TSchScope& _Scope,
 const TSecTm& _StartTm, const TSecTm& _EndTm){
  Scope=_Scope; StartTm=_StartTm; EndTm=_EndTm;
  switch (Scope){
    case ssUnlim: IAssert((!StartTm.IsDef())&&(!EndTm.IsDef())); break;
    case ssLim: IAssert((StartTm.IsDef())&&(EndTm.IsDef())&&(StartTm<=EndTm)); break;
    case ssWeek:
      IAssert((!StartTm.IsDef())&&(!EndTm.IsDef()));
      StartTm=EndTm=TSecTm::GetZeroWeekTm(); EndTm.AddDays(7); break;
    default: Fail;
  }
}

void TSchCal::AddBadDay(const TSecTm& DtTm, const TStr& DescStr){
  TSecTm NrDtTm=TSecTm::GetDtTm(DtTm);
  int BadDayN;
  if (BadDay_DtTmDescStrKdV.IsIn(NrDtTm, BadDayN)){
    BadDay_DtTmDescStrKdV.Del(BadDayN);}
  BadDay_DtTmDescStrKdV.AddSorted(TSecTmStrKd(DtTm, DescStr));
}

void TSchCal::DelBadDay(const TSecTm& DtTm){
  TSecTm NrDtTm=TSecTm::GetDtTm(DtTm);
  int BadDayN;
  if (BadDay_DtTmDescStrKdV.IsIn(NrDtTm, BadDayN)){
    BadDay_DtTmDescStrKdV.Del(BadDayN);}
}

bool TSchCal::IsBadDay(const TSecTm& DtTm) const {
  TSecTm NrDtTm=TSecTm::GetDtTm(DtTm);
  return BadDay_DtTmDescStrKdV.IsIn(NrDtTm);
}

PSchCal TSchCal::LoadTxt(TILx& Lx){
  PSchCal SchCal=TSchCal::New();
  Lx.GetVar("SchCal", true, true);
  // Scope StartTm, EndTm
  SchCal->Scope=TSchScope(Lx.GetVarInt("Scope"));
  SchCal->StartTm=Lx.GetVarSecTm("StartTm");
  SchCal->EndTm=Lx.GetVarSecTm("EndTm");
  // BadDay_DtTmDescStrKdV
  Lx.GetVar("BadDay_DtTmDescStrKdV", true, true);
  while (!Lx.PeekVarEnd(true, true)){
    TSecTm DtTm=Lx.GetVarSecTm("DtTm");
    TStr DescStr=Lx.GetVarStr("DescStr");
    SchCal->BadDay_DtTmDescStrKdV.Add(TSecTmStrKd(DtTm, DescStr));
  }
  Lx.GetVarEnd(true, true);
  Lx.GetVarEnd(true, true);
  return SchCal;
}

void TSchCal::SaveTxt(TOLx& Lx) const {
  Lx.PutVar("SchCal", true, true);
  // Scope, StartTm, EndTm
  Lx.PutVarInt("Scope", int(Scope));
  Lx.PutVarSecTm("StartTm", StartTm);
  Lx.PutVarSecTm("EndTm", EndTm);
  // BadDay_DtTmDescStrKdV
  Lx.PutVar("BadDay_DtTmDescStrKdV", true, true);
  for (int BadDayN=0; BadDayN<BadDay_DtTmDescStrKdV.Len(); BadDayN++){
    Lx.PutVarSecTm("DtTm", BadDay_DtTmDescStrKdV[BadDayN].Key);
    Lx.PutVarStr("DescStr", BadDay_DtTmDescStrKdV[BadDayN].Dat);
  }
  Lx.PutVarEnd(true, true);
  Lx.PutVarEnd(true, true);
}

/////////////////////////////////////////////////
// Schedule
TSch::TSch(const TStr& _Nm, const int& Dims,
 const int& ExpObjs, const int& ExpTasks):
  Nm(_Nm), Cal(TSchCal::New()),
  DimV(Dims), LastTaskId(0), CpbTask(),
  IdToTaskH(ExpTasks), NmToPatH(10){
  for (int DimN=0; DimN<Dims; DimN++){
    DimV[DimN]=PSchDim(new TSchDim(DimN, ExpObjs));}
}

TStrV TSch::GetDimNmV() const {
  TStrV DimNmV(GetDims(), 0);
  for (int DimN=0; DimN<GetDims(); DimN++){
    DimNmV.Add(GetDimNm(DimN));}
  return DimNmV;
}

int TSch::GetDimN(const TStr& DimNm) const {
  for (int DimN=0; DimN<GetDims(); DimN++){
    if (GetDimNm(DimN)==DimNm){return DimN;}}
  return -1;
}

void TSch::DelObj(const int& DimN, const TStr& ObjNm){
  TSchTaskV TaskV; GetObjTaskV(DimN, ObjNm, TaskV);
  for (int TaskN=0; TaskN<TaskV.Len(); TaskN++){
    DelTask(TaskV[TaskN]->GetTaskId());}
  DimV[DimN]->DelObj(ObjNm);
  SyncPatVObj();
}

bool TSch::IsOkForAddTask(TStr& MsgStr) const {
  if (GetDims()==0){MsgStr="Dimenzije niso definirane."; return false;}
  for (int DimN=0; DimN<GetDims(); DimN++){
    if (GetObjs(DimN)==0){
      MsgStr="Dimenzija nima objektov."; return false;}
  }
  MsgStr="Ok."; return true;
}

int TSch::AddTask(const PSchTask& Task){
  IAssert(Task->GetDims()==GetDims());
  IdToTaskH.AddDat(Task->GetTaskId(), Task);
  for (int DimN=0; DimN<GetDims(); DimN++){
    DimV[DimN]->AddTask(Task);}
  return Task->GetTaskId();
}

int TSch::AddTask(
 const TVec<TStrV>& DimObjVV,
 const TSecTm& StartTm, const TSecTm& EndTm, const bool& Forced){
  int TaskId=GetNewTaskId();
  PSchTask Task=PSchTask(new TSchTask(TaskId, DimObjVV, StartTm, EndTm, Forced));
  return AddTask(Task);
}

int TSch::AddTask(
 const TStr& DimObj1, const TStr& DimObj2, const TStr& DimObj3,
 const TSecTm& StartTm, const TSecTm& EndTm, const bool& Forced){
  TVec<TStrV> DimObjVV(3, 0);
  DimObjVV.Add(); DimObjVV.Last().Add(DimObj1);
  DimObjVV.Add(); DimObjVV.Last().Add(DimObj2);
  DimObjVV.Add(); DimObjVV.Last().Add(DimObj3);
  return AddTask(DimObjVV, StartTm, EndTm, Forced);
}

int TSch::AddTask(
 const TStrV& DimObjV,
 const TSecTm& StartTm, const TSecTm& EndTm, const bool& Forced){
  TVec<TStrV> DimObjVV(3, 0);
  DimObjVV.Add(); DimObjVV.Last().Add(DimObjV[0]);
  DimObjVV.Add(); DimObjVV.Last().Add(DimObjV[1]);
  DimObjVV.Add(); DimObjVV.Last().Add(DimObjV[2]);
  return AddTask(DimObjVV, StartTm, EndTm, Forced);
}

int TSch::AddTask(
 const TStrV& DimObjV1, const TStrV& DimObjV2, const TStrV& DimObjV3,
 const TSecTm& StartTm, const TSecTm& EndTm, const bool& Forced){
  TVec<TStrV> DimObjVV(3, 0);
  DimObjVV.Add(DimObjV1);
  DimObjVV.Add(DimObjV2);
  DimObjVV.Add(DimObjV3);
  return AddTask(DimObjVV, StartTm, EndTm, Forced);
}

void TSch::DelTask(const int& TaskId){
  PSchTask Task=IdToTaskH.GetDat(TaskId);
  for (int DimN=0; DimN<GetDims(); DimN++){
    DimV[DimN]->DelTask(Task);}
  IdToTaskH.DelKey(TaskId);
}

void TSch::GetDayTaskV(const TSecTm& DtTm, TIntV& TaskIdV) const {
  TaskIdV.Clr();
  int IdToTaskP=IdToTaskH.FFirstKeyId();
  while (IdToTaskH.FNextKeyId(IdToTaskP)){
    PSchTask Task=IdToTaskH[IdToTaskP];
    TSecTm TaskDtTm=TSecTm::GetDtTm(Task->GetStartTm());
    if (TaskDtTm==DtTm){
      TaskIdV.Add(Task->GetTaskId());}
  }
}

bool TSch::IsTask(const int& TaskId, PSchTask& Task) const {
  int IdToTaskP;
  if (IdToTaskH.IsKey(TaskId, IdToTaskP)){
    Task=IdToTaskH[IdToTaskP]; return true;}
  else {return false;}
}

bool TSch::IsTaskOk(const PSchTask& Task, TSchTaskV& DimIcTaskV) const {
  DimIcTaskV.Gen(GetDims(), GetDims());
  bool Ok=true;
  for (int DimN=0; DimN<GetDims(); DimN++){
    PSchTask IcTask;
    if (!DimV[DimN]->IsTaskOk(Task, IcTask)){
      DimIcTaskV[DimN]=IcTask; Ok=false;
    }
  }
  return Ok;
}

bool TSch::IsTaskSim(const PSchTask& Task, PSchTask& SimTask) const {
  TVec<TStrV> DimObjVV; Task->GetDimObjVV(DimObjVV);
  int IdToTaskP=IdToTaskH.FFirstKeyId();
  while (IdToTaskH.FNextKeyId(IdToTaskP)){
    SimTask=IdToTaskH[IdToTaskP];
    if (
     (Task->GetStartTm()==SimTask->GetStartTm())&&
     (Task->GetEndTm()==SimTask->GetEndTm())&&
     (Task->IsForced()==SimTask->IsForced())){
      TVec<TStrV> SimDimObjVV; SimTask->GetDimObjVV(SimDimObjVV);
      if (DimObjVV==SimDimObjVV){return true;}
    }
  }
  return false;
}

PSch TSch::AddPat(const TStr& PatNm){
  IAssert(!IsPat(PatNm));
  PSch Pat=New(PatNm, GetDims());
  Pat->PutNm(PatNm);
  Pat->GetCal()->PutScope(ssWeek);
  for (int DimN=0; DimN<GetDims(); DimN++){
    Pat->PutDimNm(DimN, GetDimNm(DimN));
    Pat->AddObjV(DimN, GetObjNmV(DimN));
    TStrV CObjNmV=GetCObjNmV(DimN);
    for (int CObjNmN=0; CObjNmN<CObjNmV.Len(); CObjNmN++){
      TStr CObjNm=CObjNmV[CObjNmN];
      TStrV ObjNmV=GetCObj_ObjNmV(DimN, CObjNm);
      Pat->AddCObj(DimN, CObjNm, ObjNmV);
    }
  }
  NmToPatH.AddDat(PatNm, Pat);
  return Pat;
}

void TSch::SyncPatObj(const TStr& PatNm) const {
  PSch Pat=GetPat(PatNm);
  for (int DimN=0; DimN<GetDims(); DimN++){
    // add new main objects
    {TStrV ObjNmV=GetObjNmV(DimN);
    for (int ObjNmN=0; ObjNmN<ObjNmV.Len(); ObjNmN++){
      if (!Pat->IsObj(DimN, ObjNmV[ObjNmN])){
        Pat->AddObj(DimN, ObjNmV[ObjNmN]);}
    }}
    // add new main composed objects
    {TStrV CObjNmV=GetCObjNmV(DimN);
    for (int CObjNmN=0; CObjNmN<CObjNmV.Len(); CObjNmN++){
      TStrV ObjNmV=GetCObj_ObjNmV(DimN, CObjNmV[CObjNmN]);
      if (!Pat->IsCObj(DimN, CObjNmV[CObjNmN])){
        Pat->AddCObj(DimN, CObjNmV[CObjNmN], ObjNmV);
      } else {
        TStrV PatObjNmV=Pat->GetCObj_ObjNmV(DimN, CObjNmV[CObjNmN]);
        if (ObjNmV!=PatObjNmV){
          Pat->DelCObj(DimN, CObjNmV[CObjNmN]);
          Pat->AddCObj(DimN, CObjNmV[CObjNmN], ObjNmV);
        }
      }
    }}
    // del old pattern objects
    {TStrV PatObjNmV=Pat->GetObjNmV(DimN);
    for (int PatObjNmN=0; PatObjNmN<PatObjNmV.Len(); PatObjNmN++){
      if (!IsObj(DimN, PatObjNmV[PatObjNmN])){
        Pat->DelObj(DimN, PatObjNmV[PatObjNmN]);}
    }}
    // del old pattern composed objects
    {TStrV PatCObjNmV=Pat->GetCObjNmV(DimN);
    for (int PatCObjNmN=0; PatCObjNmN<PatCObjNmV.Len(); PatCObjNmN++){
      TStrV PatObjNmV=Pat->GetCObj_ObjNmV(DimN, PatCObjNmV[PatCObjNmN]);
      if (!IsCObj(DimN, PatCObjNmV[PatCObjNmN])){
        Pat->DelCObj(DimN, PatCObjNmV[PatCObjNmN]);
      }
    }}
  }
}

void TSch::SyncPatVObj() const {
  TStrV PatNmV=GetPatNmV();
  for (int PatNmN=0; PatNmN<PatNmV.Len(); PatNmN++){
    SyncPatObj(PatNmV[PatNmN]);}
}

void TSch::ApplyPatAdd(
 const PSch& Pat, const TSecTm& StartDtTm, const TSecTm& EndDtTm,
 const bool& TestOnlyP, TStrV& LogStrV){
  // prepare log strings
  LogStrV.Clr();
  LogStrV.Add(TStr("Adding pattern: ")+Pat->GetNm());
  LogStrV.Add(TStr("Start date: ")+StartDtTm.GetDtStr());
  LogStrV.Add(TStr("End date: ")+EndDtTm.GetDtStr());
  LogStrV.Add(TStr("Test only: ")+TBool::GetYesNoStr(TestOnlyP));
  LogStrV.Add("------------------------");
  // prepare schedule date counter
  TSecTm CurSchDtTm=TSecTm::GetDtTm(StartDtTm);
  if (CurSchDtTm<GetCal()->GetStartTm()){CurSchDtTm=GetCal()->GetStartTm();}
  // prepare pattern date counter
  TSecTm CurPatDtTm=TSecTm::GetDtTm(Pat->GetCal()->GetStartTm());
  while (CurPatDtTm.GetDayOfWeekN()!=CurSchDtTm.GetDayOfWeekN()){
    CurPatDtTm.AddDays(1);}
  // apply pattern for all days
  while ((CurSchDtTm<GetCal()->GetStartTm())&&(CurSchDtTm<=EndDtTm)){
    // retrieve tasks for a current pattern day
    TIntV PatTaskIdV; Pat->GetDayTaskV(CurPatDtTm, PatTaskIdV);
    // convert & (try to) add tasks to the current schedule day
    for (int PatTaskIdN=0; PatTaskIdN<PatTaskIdV.Len(); PatTaskIdN++){
      PSchTask PatTask=Pat->GetTask(PatTaskIdV[PatTaskIdN]);
      // calculate start & end time for schedule task
      TSecTm SchTaskStartTm=
       CurSchDtTm+PatTask->GetStartTm()-TSecTm::GetDtTm(PatTask->GetStartTm());
      TSecTm SchTaskEndTm=
       CurSchDtTm+PatTask->GetEndTm()-TSecTm::GetDtTm(PatTask->GetEndTm());
      // construct schedule task
      int SchTaskId=GetNewTaskId();
      TVec<TStrV> SchTaskDimObjVV; PatTask->GetDimObjVV(SchTaskDimObjVV);
      bool SchTaskForced=PatTask->IsForced();
      PSchTask SchTask=PSchTask(new TSchTask(SchTaskId,
       SchTaskDimObjVV, SchTaskStartTm, SchTaskEndTm, SchTaskForced));
      // check task correctness
      if (IsTaskOk(SchTask)){
        if (!TestOnlyP){
          AddTask(SchTask);} // add task to schedule
        // add schedule task to log strings
        TChA LogChA;
        LogChA+="Add ";
        LogChA+='['; LogChA+=SchTaskStartTm.GetStr(); LogChA+=']';
        LogChA+=" - ";
        LogChA+='['; LogChA+=SchTaskEndTm.GetStr(); LogChA+=']'; LogChA+="  ";
        LogChA+=SchTask->GetStr();
        LogStrV.Add(LogChA);
      }
    }
    // increment schedule date
    CurSchDtTm.AddDays(1);
    // increment pattern date
    CurPatDtTm.AddDays(1);
    if (CurPatDtTm>=Pat->GetCal()->GetEndTm()){
      CurPatDtTm=Pat->GetCal()->GetStartTm();}
  }
}

void TSch::ApplyPatDel(
 const PSch& Pat, const TSecTm& StartDtTm, const TSecTm& EndDtTm,
 const bool& TestOnlyP, TStrV& LogStrV){
  // prepare log strings
  LogStrV.Clr();
  LogStrV.Add(TStr("Deleting pattern: ")+Pat->GetNm());
  LogStrV.Add(TStr("Start date: ")+StartDtTm.GetDtStr());
  LogStrV.Add(TStr("End date: ")+EndDtTm.GetDtStr());
  LogStrV.Add(TStr("Test only: ")+TBool::GetYesNoStr(TestOnlyP));
  LogStrV.Add("------------------------");
  // prepare schedule date counter
  TSecTm CurSchDtTm=TSecTm::GetDtTm(StartDtTm);
  if (CurSchDtTm<GetCal()->GetStartTm()){CurSchDtTm=GetCal()->GetStartTm();}
  // prepare pattern date counter
  TSecTm CurPatDtTm=TSecTm::GetDtTm(Pat->GetCal()->GetStartTm());
  while (CurPatDtTm.GetDayOfWeekN()!=CurSchDtTm.GetDayOfWeekN()){
    CurPatDtTm.AddDays(1);}
  // apply pattern for all days
  while ((CurSchDtTm<GetCal()->GetEndTm())&&(CurSchDtTm<=EndDtTm)){
    // retrieve tasks for a current pattern day
    TIntV PatTaskIdV; Pat->GetDayTaskV(CurPatDtTm, PatTaskIdV);
    // convert & (try to) add tasks to the current schedule day
    for (int PatTaskIdN=0; PatTaskIdN<PatTaskIdV.Len(); PatTaskIdN++){
      PSchTask PatTask=Pat->GetTask(PatTaskIdV[PatTaskIdN]);
      // calculate start & end time for schedule task
      TSecTm SchTaskStartTm=
       CurSchDtTm+PatTask->GetStartTm()-TSecTm::GetDtTm(PatTask->GetStartTm());
      TSecTm SchTaskEndTm=
       CurSchDtTm+PatTask->GetEndTm()-TSecTm::GetDtTm(PatTask->GetEndTm());
      // construct schedule task
      int SchTaskId=GetNewTaskId();
      TVec<TStrV> SchTaskDimObjVV; PatTask->GetDimObjVV(SchTaskDimObjVV);
      bool SchTaskForced=PatTask->IsForced();
      PSchTask SchTask=PSchTask(new TSchTask(SchTaskId,
       SchTaskDimObjVV, SchTaskStartTm, SchTaskEndTm, SchTaskForced));
      // check task correctness
      PSchTask SimSchTask;
      if (IsTaskSim(SchTask, SimSchTask)){
        if (!TestOnlyP){
          DelTask(SimSchTask->GetTaskId()); // delete task from schedule
        }
        // add schedule task to log strings
        TChA LogChA;
        LogChA+="Del ";
        LogChA+='['; LogChA+=SchTaskStartTm.GetStr(); LogChA+=']';
        LogChA+=" - ";
        LogChA+='['; LogChA+=SchTaskEndTm.GetStr(); LogChA+=']'; LogChA+="  ";
        LogChA+=SchTask->GetStr();
        LogStrV.Add(LogChA);
      }
    }
    // increment schedule date
    CurSchDtTm.AddDays(1);
    // increment pattern date
    CurPatDtTm.AddDays(1);
    if (CurPatDtTm>=Pat->GetCal()->GetEndTm()){
      CurPatDtTm=Pat->GetCal()->GetStartTm();}
  }
}

PSch TSch::LoadTxt(TILx& Lx){
  PSch Sch=PSch(new TSch());
  Lx.GetVar("Sch", true, true);
  // Nm
  Sch->Nm=Lx.GetVarStr("Nm");
  // Cal
  Sch->Cal=TSchCal::LoadTxt(Lx);
  // DimV
  Lx.GetVar("DimV", true, true);
  while (!Lx.PeekVarEnd(true, true)){
    Sch->DimV.Add(TSchDim::LoadTxt(Lx));}
  Lx.GetVarEnd(true, true);
  // LastTaskId
  Sch->LastTaskId=Lx.GetVarInt("LastTaskId");
  // IdToTaskH
  {Lx.GetVar("IdToTaskH", true, true);
  int Ports=Lx.GetVarInt("Ports");
  Sch->IdToTaskH=TIdToTaskH(Ports);
  while (!Lx.PeekVarEnd(true, true)){
    PSchTask Task=TSchTask::LoadTxt(Lx);
    Sch->IdToTaskH.AddDat(Task->GetTaskId(), Task);
  }
  Lx.GetVarEnd(true, true);}
  // NmToPatH
  {Lx.GetVar("NmToPatH", true, true);
  int Ports=Lx.GetVarInt("Ports");
  Sch->NmToPatH=TNmToPatH(Ports);
  while (!Lx.PeekVarEnd(true, true)){
    PSch Pat=TSch::LoadTxt(Lx);
    Sch->NmToPatH.AddDat(Pat->GetNm(), Pat);
  }
  Lx.GetVarEnd(true, true);}
  Lx.GetVarEnd(true, true);
  return Sch;
}

PSch TSch::LoadTxt(const TStr& FNm){
  PSIn SIn=PSIn(new TFIn(FNm));
  TILx Lx(SIn, TFSet()|iloRetEoln|iloSigNum|iloCsSens);
  return LoadTxt(Lx);
}

void TSch::SaveTxt(TOLx& Lx) const {
  Lx.PutVar("Sch", true, true);
  // Nm
  Lx.PutVarStr("Nm", Nm);
  // Cal
  Cal->SaveTxt(Lx);
  // DimV
  Lx.PutVar("DimV", true, true);
  for (int DimN=0; DimN<DimV.Len(); DimN++){
    DimV[DimN]->SaveTxt(Lx);}
  Lx.PutVarEnd(true, true);
  // LastTaskId
  Lx.PutVarInt("LastTaskId", LastTaskId);
  // IdToTaskH
  Lx.PutVar("IdToTaskH", true, true);
  Lx.PutVarInt("Ports", IdToTaskH.GetPorts());
  int IdToTaskP=IdToTaskH.FFirstKeyId();
  while (IdToTaskH.FNextKeyId(IdToTaskP)){
    IdToTaskH[IdToTaskP]->SaveTxt(Lx);}
  Lx.PutVarEnd(true, true);
  // NmToPatH
  Lx.PutVar("NmToPatH", true, true);
  Lx.PutVarInt("Ports", NmToPatH.GetPorts());
  int NmToPatP=NmToPatH.FFirstKeyId();
  while (NmToPatH.FNextKeyId(NmToPatP)){
    NmToPatH[NmToPatP]->SaveTxt(Lx);}
  Lx.PutVarEnd(true, true);
  Lx.PutVarEnd(true, true);
}

void TSch::SaveTxt(const TStr& FNm) const {
  PSOut SOut=PSOut(new TFOut(FNm));
  TOLx Lx(SOut, TFSet()|oloFrcEoln|oloSigNum|oloCsSens|oloVarIndent);
  SaveTxt(Lx);
}


