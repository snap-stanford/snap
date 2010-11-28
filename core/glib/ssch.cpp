/////////////////////////////////////////////////
// Includes
#include "sch.h"

/////////////////////////////////////////////////
// Schedule-Task
TSchTask::TSchTask(const int& _TaskId, const TStrV& _DimObjNmV,
 const TSecTm& _StartTm, const TSecTm& _EndTm):
  TaskId(_TaskId), DimObjNmV(_DimObjNmV), StartTm(_StartTm), EndTm(_EndTm){
  IAssert(StartTm<EndTm);
}

TStr TSchTask::GetStr() const {
  TChA ChA;
  for (int DimN=0; DimN<DimObjNmV.Len(); DimN++){
    if (DimN>0){ChA+=' ';}
    ChA+=DimObjNmV[DimN];
  }
  return ChA;
}

bool TSchTask::IsCons(const PSchTask& Task1, const PSchTask& Task2){
  return
   (Task1->GetEndTm()<=Task2->GetStartTm())||
   (Task2->GetEndTm()<=Task1->GetStartTm());
}

PSchTask TSchTask::LoadTxt(TILx& Lx){
  PSchTask SchTask=PSchTask(new TSchTask());
  Lx.GetVar("SchTask", true, true);
  SchTask->TaskId=Lx.GetVarInt("TaskId");
  Lx.GetVarStrV("DimObjNmV", SchTask->DimObjNmV);
  SchTask->StartTm=Lx.GetVarSecTm("StartTm");
  SchTask->EndTm=Lx.GetVarSecTm("EndTm");
  Lx.GetVarEnd(true, true);
  return SchTask;
}

void TSchTask::SaveTxt(TOLx& Lx) const {
  Lx.PutVar("SchTask", true, true);
  Lx.PutVarInt("TaskId", TaskId);
  Lx.PutVarStrV("DimObjNmV", DimObjNmV);
  Lx.PutVarSecTm("StartTm", StartTm);
  Lx.PutVarSecTm("EndTm", EndTm);
  Lx.PutVarEnd(true, true);
}

/////////////////////////////////////////////////
// Schedule-Object
void TSchObj::AddTask(const PSchTask& Task){
  IAssert(IsTaskOk(Task));
  TTmTaskKd TmTaskKd(Task->GetStartTm(), Task);
  int TaskN=TaskV.AddSorted(TmTaskKd);
  if (0<TaskN){
    PSchTask PrevTask=TaskV[TaskN-1].Dat;
    IAssert(TSchTask::IsCons(PrevTask, Task));
  }
  if (TaskN+1<TaskV.Len()){
    PSchTask NextTask=TaskV[TaskN+1].Dat;
    IAssert(TSchTask::IsCons(Task, NextTask));
  }
}

void TSchObj::DelTask(const PSchTask& Task){
  TTmTaskKd TmTaskKd(Task->GetStartTm(), Task);
  int TaskN=TaskV.SearchBin(TmTaskKd);
  IAssert(TaskN!=-1);
  IAssert(Task->GetTaskId()==TaskV[TaskN].Dat->GetTaskId());
  TaskV.Del(TaskN);
}

bool TSchObj::IsTaskAtTm(const TSecTm& Tm, PSchTask& Task) const {
  TTmTaskKd TmTaskKd(Tm);
  int InsTaskN; int TaskN=TaskV.SearchBin(TmTaskKd, InsTaskN);
  if (TaskN==-1){
    if (InsTaskN==0){
      Task=NULL; return false;
    } else
    if (TaskV[InsTaskN-1].Dat->IsTmIn(Tm)){
      Task=TaskV[InsTaskN-1].Dat; return true;
    } else {
      Task=NULL; return false;
    }
  } else {
    Task=TaskV[TaskN].Dat; return true;
  }
}

bool TSchObj::IsTaskInTm(
 const TSecTm& MnTm, const TSecTm& MxTm, PSchTask& Task) const {
  TTmTaskKd StartTmTaskKd(MnTm);
  TTmTaskKd EndTmTaskKd(MxTm);
  int StartInsTaskN; int EndInsTaskN;
  int StartTaskN=TaskV.SearchBin(StartTmTaskKd, StartInsTaskN);
  int EndTaskN=TaskV.SearchBin(EndTmTaskKd, EndInsTaskN);
  if ((StartTaskN==-1)&&(EndTaskN==-1)){
    if (StartInsTaskN==EndInsTaskN){
      if (StartInsTaskN==0){
        Task=NULL; return false;
      } else
      if (TaskV[StartInsTaskN-1].Dat->IsTmIn(MnTm)){
        Task=TaskV[StartInsTaskN-1].Dat; return true;
      } else {
        Task=NULL; return false;
      }
    } else {
      Task=TaskV[StartInsTaskN].Dat; return true;
    }
  } else {
    if (StartTaskN!=-1){IsTaskAtTm(MnTm, Task);}
    else if (EndTaskN!=-1){IsTaskAtTm(MxTm, Task);}
    else {Fail;}
    IAssert(!Task.Empty());
    return true;
  }
}

PSchObj TSchObj::LoadTxt(TILx& Lx){
  PSchObj SchObj=PSchObj(new TSchObj());
  Lx.GetVar("SchObj", true, true);
  // Nm
  SchObj->Nm=Lx.GetVarStr("Nm");
  // TaskV
  Lx.GetVar("TaskV", true, true);
  while (!Lx.PeekVarEnd(true, true)){
    PSchTask Task=TSchTask::LoadTxt(Lx);
    SchObj->TaskV.Add(TTmTaskKd(Task->GetStartTm(), Task));
  }
  Lx.GetVarEnd(true, true);
  Lx.GetVarEnd(true, true);
  return SchObj;
}

void TSchObj::SaveTxt(TOLx& Lx) const {
  Lx.PutVar("SchObj", true, true);
  // Nm
  Lx.PutVarStr("Nm", Nm);
  // TaskV
  Lx.PutVar("TaskV", true, true);
  for (int TaskN=0; TaskN<TaskV.Len(); TaskN++){
    TaskV[TaskN].Dat->SaveTxt(Lx);}
  Lx.PutVarEnd(true, true);
  Lx.PutVarEnd(true, true);
}

/////////////////////////////////////////////////
// Schedule-Dimension
int TSchDim::AddObj(const TStr& Nm){
  IAssert(!NmToObjH.IsKey(Nm));
  PSchObj Obj=PSchObj(new TSchObj(Nm));
  NmToObjH.AddDat(Nm, Obj);
  return NmToObjH.GetKeyId(Nm);
}

TStrV TSchDim::GetObjNmV() const {
  TStrV NmV(NmToObjH.Len(), 0);
  int NmToObjP=NmToObjH.FFirstKeyId();
  while (NmToObjH.FNextKeyId(NmToObjP)){
    NmV.Add(NmToObjH[NmToObjP]->GetNm());}
  return NmV;
}

void TSchDim::AddTask(const PSchTask& Task){
  PSchObj Obj=GetObj(Task->GetDimObjNm(DimN));
  Obj->AddTask(Task);
}

void TSchDim::DelTask(const PSchTask& Task){
  PSchObj Obj=GetObj(Task->GetDimObjNm(DimN));
  Obj->DelTask(Task);
}

bool TSchDim::IsTaskOk(const PSchTask& Task, PSchTask& IcTask) const {
  PSchObj Obj=GetObj(Task->GetDimObjNm(DimN));
  return Obj->IsTaskOk(Task, IcTask);
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
  // NmToObjH
  Lx.GetVar("NmToObjH", true, true);
  int Ports=Lx.GetVarInt("Ports");
  SchDim->NmToObjH=TNmToObjH(Ports);
  while (!Lx.PeekVarEnd(true, true)){
    PSchObj Obj=TSchObj::LoadTxt(Lx);
    SchDim->NmToObjH.AddDat(Obj->GetNm(), Obj);
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
  // NmToObjH
  Lx.PutVar("NmToObjH", true, true);
  int NmToObjP=NmToObjH.FFirstKeyId();
  Lx.PutVarInt("Ports", NmToObjH.GetPorts());
  while (NmToObjH.FNextKeyId(NmToObjP)){
    NmToObjH[NmToObjP]->SaveTxt(Lx);}
  Lx.PutVarEnd(true, true);
  Lx.PutVarEnd(true, true);
}

/////////////////////////////////////////////////
// Schedule
TSch::TSch(const TStr& _Nm, const int& Dims,
 const int& ExpObjs, const int& ExpTasks):
  Nm(_Nm), DimV(Dims), LastTaskId(0), IdToTaskH(ExpTasks){
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

int TSch::AddTask(
 const TStrV& DimObjNmV, const TSecTm& StartTm, const TSecTm& EndTm){
  IAssert(DimObjNmV.Len()==GetDims());
  int TaskId=GetNewTaskId();
  PSchTask Task=PSchTask(new TSchTask(TaskId, DimObjNmV, StartTm, EndTm));
  IdToTaskH.AddDat(TaskId, Task);
  for (int DimN=0; DimN<GetDims(); DimN++){
    DimV[DimN]->AddTask(Task);}
  return TaskId;
}

int TSch::AddTask(
 const TStr& DimObjNm1, const TStr& DimObjNm2, const TStr& DimObjNm3,
 const TSecTm& StartTm, const TSecTm& EndTm){
  TStrV DimObjNmV(3, 0);
  DimObjNmV.Add(DimObjNm1);
  DimObjNmV.Add(DimObjNm2);
  DimObjNmV.Add(DimObjNm3);
  return AddTask(DimObjNmV, StartTm, EndTm);
}

void TSch::DelTask(const int& TaskId){
  PSchTask Task=IdToTaskH.GetDat(TaskId);
  for (int DimN=0; DimN<GetDims(); DimN++){
    DimV[DimN]->DelTask(Task);}
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

PSch TSch::LoadTxt(TILx& Lx){
  PSch Sch=PSch(new TSch());
  Lx.GetVar("Sch", true, true);
  // Nm
  Sch->Nm=Lx.GetVarStr("Nm");
  // DimV
  Lx.GetVar("DimV", true, true);
  while (!Lx.PeekVarEnd(true, true)){
    Sch->DimV.Add(TSchDim::LoadTxt(Lx));}
  Lx.GetVarEnd(true, true);
  // LastTaskId
  Sch->LastTaskId=Lx.GetVarInt("LastTaskId");
  // IdToTaskH
  Lx.GetVar("IdToTaskH", true, true);
  int Ports=Lx.GetVarInt("Ports");
  Sch->IdToTaskH=TIdToTaskH(Ports);
  while (!Lx.PeekVarEnd(true, true)){
    PSchTask Task=TSchTask::LoadTxt(Lx);
    Sch->IdToTaskH.AddDat(Task->GetTaskId(), Task);
  }
  Lx.GetVarEnd(true, true);
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
  Lx.PutVarEnd(true, true);
}

void TSch::SaveTxt(const TStr& FNm) const {
  PSOut SOut=PSOut(new TFOut(FNm));
  TOLx Lx(SOut, TFSet()|oloFrcEoln|oloSigNum|oloCsSens|oloVarIndent);
  SaveTxt(Lx);
}

