#ifndef Sch_h
#define Sch_h

/////////////////////////////////////////////////
// Includes
#include "base.h"

/////////////////////////////////////////////////
// Forward
class TSchTask;
typedef TPt<TSchTask> PSchTask;

class TSchObj;
typedef TPt<TSchObj> PSchObj;

class TSchDim;
typedef TPt<TSchDim> PSchDim;

class TSchCal;
typedef TPt<TSchCal> PSchCal;

class TSchRep;
typedef TPt<TSchRep> PSchRep;

class TSch;
typedef TPt<TSch> PSch;

/////////////////////////////////////////////////
// Schedule-Task
class TSchTask{
private:
  TCRef CRef;
private:
  TInt TaskId;
  TVec<TStrV> DimObjVV;
  TSecTm StartTm, EndTm;
  TBool Forced;
public:
  TSchTask(): TaskId(-1), DimObjVV(), StartTm(), EndTm(), Forced(){}
  TSchTask(const int& _TaskId, const TVec<TStrV>& _DimObjVV,
   const TSecTm& _StartTm, const TSecTm& _EndTm, const bool& _Forced);
  TSchTask(const int& _TaskId, const TStrV& DimObjV,
   const TSecTm& _StartTm, const TSecTm& _EndTm, const bool& _Forced);
  ~TSchTask(){}
  TSchTask(TSIn& SIn):
    TaskId(SIn), DimObjVV(SIn), StartTm(SIn), EndTm(SIn), Forced(SIn){}
  void Save(TSOut& SOut){
    TaskId.Save(SOut); DimObjVV.Save(SOut);
    StartTm.Save(SOut); EndTm.Save(SOut); Forced.Save(SOut);}

  TSchTask& operator=(const TSchTask&){Fail; return *this;}

  // members
  int GetTaskId() const {return TaskId;}
  int GetDims() const {return DimObjVV.Len();}
  int GetDimObjs(const int& DimN) const {return DimObjVV[DimN].Len();}
  TStr GetDimObj(const int& DimN, const int& ObjN) const {
    return DimObjVV[DimN][ObjN];}
  TStr GetAllDimObjStr(const int& DimN) const;
  void GetDimObjVV(TVec<TStrV>& _DimObjVV) const {_DimObjVV=DimObjVV;}
  TSecTm GetStartTm() const {return StartTm;}
  TSecTm GetEndTm() const {return EndTm;}
  int GetTmSecs() const {return TSecTm::GetDSecs(GetStartTm(), GetEndTm());}
  bool IsForced() const {return Forced;}

  // operations
  bool IsTmIn(const TSecTm& Tm) const {
    return (StartTm<=Tm)&&(Tm<EndTm);}
  bool IsTmInts(const TSecTm& MnTm, const TSecTm& MxTm) const {
    IAssert(MnTm<=MxTm); return !((MxTm<StartTm)||(EndTm<=MnTm));}
  TStr GetStr() const;

  // consistency
  static bool IsCons(const PSchTask& Task1, const PSchTask& Task2);

  // text file
  static PSchTask LoadTxt(TILx& Lx);
  void SaveTxt(TOLx& Lx) const;

  friend PSchTask;
};
typedef TVec<PSchTask> TSchTaskV;

/////////////////////////////////////////////////
// Schedule-Object
class TSchObj{
private:
  TCRef CRef;
private:
  typedef TKeyDat<TSecTm, PSchTask> TTmTaskKd;
  TStr Nm;
  TVec<TTmTaskKd> TmTaskKdV;
  TSchTaskV FrcTaskV;
public:
  TSchObj(): Nm(), TmTaskKdV(), FrcTaskV(){}
  TSchObj(const TStr& _Nm): Nm(_Nm), TmTaskKdV(), FrcTaskV(){}
  ~TSchObj(){}
  TSchObj(TSIn& SIn):
    Nm(SIn), TmTaskKdV(SIn), FrcTaskV(SIn){}
  void Save(TSOut& SOut){
    Nm.Save(SOut); TmTaskKdV.Save(SOut); FrcTaskV.Save(SOut);}

  TSchObj& operator=(const TSchObj&){Fail; return *this;}

  // name
  TStr GetNm() const {return Nm;}

  // tasks
  void AddTask(const PSchTask& Task, const bool& OverlapCheckP);
  void DelTask(const PSchTask& Task);
  void GetTaskV(TSchTaskV& TaskV);
  int GetTasks() const;
  PSchTask GetTask(const int& TaskN) const;

  // task time interval testing
  bool IsRegTaskAtTm(const TSecTm& Tm, PSchTask& Task) const;
  bool IsRegTaskInTm(
   const TSecTm& MnTm, const TSecTm& MxTm, PSchTask& Task) const;
  bool IsFrcTaskInTm(
   const TSecTm& MnTm, const TSecTm& MxTm, PSchTask& Task) const;
  bool IsTaskInTm(
   const TSecTm& MnTm, const TSecTm& MxTm, PSchTask& Task) const;

  // task verification
  bool IsTaskOk(const PSchTask& Task, PSchTask& IcTask,
   const bool& OverlapCheckP) const {
    if (OverlapCheckP){
     return (Task->IsForced())||
      (!IsRegTaskInTm(Task->GetStartTm(), Task->GetEndTm()-1, IcTask));}
    else {return true;}}
  bool IsTaskOk(const PSchTask& Task, const bool& OverlapCheckP) const {
    PSchTask IcTask; return IsTaskOk(Task, IcTask, OverlapCheckP);}

  // text file
  static PSchObj LoadTxt(TILx& Lx);
  void SaveTxt(TOLx& Lx) const;

  friend PSchObj;
};

/////////////////////////////////////////////////
// Schedule-Dimension
class TSchDim{
private:
  TCRef CRef;
private:
  typedef THash<TStr, PSchObj> TNmToObjH;
  TInt DimN;
  TStr Nm;
  TBool OverlapCheckP;
  TNmToObjH NmToObjH;
  TStrStrVH CObjNmToObjNmVH;
public:
  TSchDim():
    DimN(-1), Nm(), OverlapCheckP(true), NmToObjH(), CObjNmToObjNmVH(){}
  TSchDim(const int& _DimN, const int& ExpObjs):
    DimN(_DimN), Nm(), OverlapCheckP(true),
    NmToObjH(ExpObjs), CObjNmToObjNmVH(ExpObjs){}
  ~TSchDim(){}
  TSchDim(TSIn& SIn):
    DimN(SIn), Nm(SIn), OverlapCheckP(SIn),
    NmToObjH(SIn), CObjNmToObjNmVH(SIn){}
  void Save(TSOut& SOut){
    DimN.Save(SOut); Nm.Save(SOut); OverlapCheckP.Save(SOut);
    NmToObjH.Save(SOut); CObjNmToObjNmVH.Save(SOut);}

  TSchDim& operator=(const TSchDim&){Fail; return *this;}

  // dim & name
  int GetDimN() const {return DimN;}
  void PutNm(const TStr& _Nm){Nm=_Nm;}
  TStr GetNm() const {return Nm;}

  // consistency check
  bool IsOverlapCheck() const {return OverlapCheckP;}
  void SetOverlapCheck(const bool& _OverlapCheckP){OverlapCheckP=_OverlapCheckP;}

  // objects
  bool IsObj(const TStr& ObjNm) const {
    return NmToObjH.IsKey(ObjNm);}
  bool IsObjV(const TStrV& ObjNmV) const {
    for (int ObjNmN=0; ObjNmN<ObjNmV.Len(); ObjNmN++){
      if (!IsObj(ObjNmV[ObjNmN])){return false;}}
    return true;}
  void AddObj(const TStr& ObjNm);
  void DelObj(const TStr& ObjNm){
    IAssert(GetObj(ObjNm)->GetTasks()==0); NmToObjH.DelKey(ObjNm);}
  PSchObj GetObj(const TStr& ObjNm) const {
    return NmToObjH.GetDat(ObjNm);}
  int GetObjs() const {return NmToObjH.Len();}
  TStrV GetObjNmV() const;
  void GetObjTaskV(const TStr& ObjNm, TSchTaskV& TaskV) const {
    GetObj(ObjNm)->GetTaskV(TaskV);}

  // composed objects
  bool IsCObj(const TStr& CObjNm) const {
    return CObjNmToObjNmVH.IsKey(CObjNm);}
  int AddCObj(const TStr& CObjNm, const TStrV& ObjNmV);
  void DelCObj(const TStr& CObjNm){
    CObjNmToObjNmVH.DelKey(CObjNm);}
  TStrV GetCObj_ObjNmV(const TStr& CObjNm) const {
    TStrV ObjNmV=CObjNmToObjNmVH.GetDat(CObjNm); ObjNmV.Sort(); return ObjNmV;}
  TStrV GetCObjNmV() const {
    TStrV NmV; CObjNmToObjNmVH.GetKeyV(NmV); return NmV;}
  TStrV GetAllObjNmV() const {
    TStrV NmV; NmV.AddV(GetCObjNmV()); NmV.AddV(GetObjNmV()); return NmV;}
  void GetCObjFromDesc(const TStr& DescStr, TStr& CObjNm, TStrV& ObjNmV) const;
  TStrV GetAllObjDescV() const;

  // tasks
  void AddTask(const PSchTask& Task);
  void DelTask(const PSchTask& Task);
  bool IsTaskOk(const PSchTask& Task, PSchTask& IcTask) const;
  bool IsTaskOk(const PSchTask& Task) const {
    PSchTask IcTask; return IsTaskOk(Task, IcTask);}
  bool IsTaskInTm(const TStr& ObjNm,
   const TSecTm& MnTm, const TSecTm& MxTm, PSchTask& Task) const;

  // text file
  static PSchDim LoadTxt(TILx& Lx);
  void SaveTxt(TOLx& Lx) const;

  friend PSchDim;
};
typedef TVec<PSchDim> TSchDimV;

/////////////////////////////////////////////////
// Schedule-Report
class TSchRep{
private:
  TCRef CRef;
private:
  TSecTmV DayTmV;
  TSecTmV DtTmV;
  TVec<TStrV> TaskNmVV;
public:
  TSchRep(){}
  ~TSchRep(){}
  TSchRep(TSIn&){Fail;}
  void Save(TSOut&){Fail;}

  TSchRep& operator=(const TSchRep&){Fail; return *this;}

  static PSchRep New(
   const PSch& Sch, const TStr& DimNm, const TStr& ObjNm, const int& ResMins,
   const bool& AllTmP, const TSecTm& StartDtTm, const TSecTm& EndDtTm);

  int GetDayTms() const {return DayTmV.Len();}
  TStr GetDayTmStr(const int& DayTmN) const {
    return DayTmV[DayTmN].GetTmMinStr();}

  int GetDtTms() const {return DtTmV.Len();}
  TSecTm GetDtTm(const int& DtTmN) const {return DtTmV[DtTmN];}
  TStr GetDtTmStr(const int& DtTmN) const {return DtTmV[DtTmN].GetDtStr();}

  TStr GetTaskStr(const int& DtTmN, const int& DayTmN) const {
    return TaskNmVV[DtTmN][DayTmN];}

  void SaveHtml(const PSOut& SOut) const;
  void SaveHtml(const TStr& FNm) const;
  void SaveTxt(const PSOut& SOut) const;
  void SaveTxt(const TStr& FNm) const;
  static void SaveStat(const TStr& FNm, const PSch& Sch);

  friend PSchRep;
};

/////////////////////////////////////////////////
// Schedule-Calendar
typedef enum {ssUnlim, ssLim, ssWeek} TSchScope;

class TSchCal{
private:
  TCRef CRef;
private:
  TInt Scope;
  TSecTm StartTm, EndTm;
  TSecTmStrKdV BadDay_DtTmDescStrKdV;
public:
  TSchCal(): Scope(ssUnlim), StartTm(), EndTm(){}
  static PSchCal New(){return PSchCal(new TSchCal());}
  ~TSchCal(){}
  TSchCal(TSIn& SIn):
    Scope(SIn), StartTm(SIn), EndTm(SIn){}
  void Save(TSOut& SOut){
    Scope.Save(SOut); StartTm.Save(SOut); EndTm.Save(SOut);}

  TSchCal& operator=(const TSchCal&){Fail; return *this;}

  // time boundaries
  void PutScope(const TSchScope& Scope,
   const TSecTm& _StartTm=TSecTm(), const TSecTm& _EndTm=TSecTm());
  TSchScope GetScope() const {return TSchScope(int(Scope));}
  TSecTm GetStartTm() const {return StartTm;}
  TSecTm GetEndTm() const {return EndTm;}

  // bad days
  void AddBadDay(const TSecTm& DtTm, const TStr& DescStr);
  void DelBadDay(const TSecTm& DtTm);
  bool IsBadDay(const TSecTm& DtTm) const;
  int GetBadDays() const {return BadDay_DtTmDescStrKdV.Len();}
  TSecTm GetBadDay_DtTm(const int& BadDayN) const {
    return BadDay_DtTmDescStrKdV[BadDayN].Key;}
  TStr GetBadDay_DescStr(const int& BadDayN) const {
    return BadDay_DtTmDescStrKdV[BadDayN].Dat;}

  // text file
  static PSchCal LoadTxt(TILx& Lx);
  void SaveTxt(TOLx& Lx) const;

  friend PSchCal;
};

/////////////////////////////////////////////////
// Schedule
class TSch{
private:
  TCRef CRef;
private:
  typedef THash<TInt, PSchTask> TIdToTaskH;
  typedef THash<TStr, PSch> TNmToPatH;
  TStr Nm;
  PSchCal Cal;
  TSchDimV DimV;
  TInt LastTaskId;
  PSchTask CpbTask;
  TIdToTaskH IdToTaskH;
  TNmToPatH NmToPatH;
  int GetNewTaskId(){return LastTaskId++;}
public:
  TSch():
    Nm(), Cal(TSchCal::New()),
    DimV(), LastTaskId(-1), CpbTask(),
    IdToTaskH(), NmToPatH(){}
  static PSch New(){return PSch(new TSch());}
  TSch(const TStr& _Nm, const int& Dims,
   const int& ExpObjs=100, const int& ExpTasks=1000);
  static PSch New(const TStr& Nm, const int& Dims){
    return PSch(new TSch(Nm, Dims));}
  ~TSch(){}
  TSch(TSIn& SIn):
    Nm(SIn), Cal(SIn),
    DimV(SIn), LastTaskId(SIn), CpbTask(SIn),
    IdToTaskH(SIn), NmToPatH(SIn){}
  void Save(TSOut& SOut){
    Nm.Save(SOut); Cal.Save(SOut);
    DimV.Save(SOut); LastTaskId.Save(SOut); CpbTask.Save(SOut);
    IdToTaskH.Save(SOut); NmToPatH.Save(SOut);}

  TSch& operator=(const TSch&){Fail; return *this;}

  // name
  void PutNm(const TStr& _Nm){Nm=_Nm;}
  TStr GetNm() const {return Nm;}

  // time boundaries
  PSchCal GetCal() const {return Cal;}

  // dimensions
  int GetDims() const {return DimV.Len();}
  TStr GetDimNm(const int& DimN) const {return DimV[DimN]->GetNm();}
  TStrV GetDimNmV() const;
  int GetDimN(const TStr& DimNm) const;
  void PutDimNm(const int& DimN, const TStr& DimNm) const {
    DimV[DimN]->PutNm(DimNm);}
  void PutDimOverlapCheck(const int& DimN, const bool& OverlapCheckP) const {
    DimV[DimN]->SetOverlapCheck(OverlapCheckP);}

  // objects
  bool IsObj(const int& DimN, const TStr& ObjNm) const {
    return DimV[DimN]->IsObj(ObjNm);}
  bool IsObjV(const int& DimN, const TStrV& ObjNmV) const {
    return DimV[DimN]->IsObjV(ObjNmV);}
  void AddObj(const int& DimN, const TStr& ObjNm) const {
    DimV[DimN]->AddObj(ObjNm); SyncPatVObj();}
  void AddObjV(const int& DimN, const TStrV& ObjNmV) const {
    for (int ObjNmN=0; ObjNmN<ObjNmV.Len(); ObjNmN++){
      AddObj(DimN, ObjNmV[ObjNmN]);}}
  void AddObj(const TStr& DimNm, const TStr& ObjNm) const {
    AddObj(GetDimN(DimNm), ObjNm);}
  void DelObj(const int& DimN, const TStr& ObjNm);
  int GetObjs(const int& DimN) const {
    return DimV[DimN]->GetObjs();}
  TStrV GetObjNmV(const int& DimN) const {
    return DimV[DimN]->GetObjNmV();}
  void GetObjTaskV(const int& DimN, const TStr& ObjNm, TSchTaskV& TaskV) const {
    DimV[DimN]->GetObjTaskV(ObjNm, TaskV);}

  // composed objects
  bool IsCObj(const int& DimN, const TStr& CObjNm) const {
    return DimV[DimN]->IsCObj(CObjNm);}
  int AddCObj(const int& DimN, const TStr& CObjNm, const TStrV& ObjNmV) const {
    return DimV[DimN]->AddCObj(CObjNm, ObjNmV);}
  int AddCObj(const TStr& DimNm, const TStr& CObjNm, const TStrV& ObjNmV) const {
    return AddCObj(GetDimN(DimNm), CObjNm, ObjNmV);}
  void DelCObj(const int& DimN, const TStr& CObjNm) const {
    DimV[DimN]->DelCObj(CObjNm);}
  TStrV GetCObj_ObjNmV(const int& DimN, const TStr& CObjNm) const {
    return DimV[DimN]->GetCObj_ObjNmV(CObjNm);}
  TStrV GetCObjNmV(const int& DimN) const {
    return DimV[DimN]->GetCObjNmV();}
  TStrV GetAllObjNmV(const int& DimN) const {
    return DimV[DimN]->GetAllObjNmV();}
  void GetCObjFromDesc(
   const int& DimN, const TStr& DescStr, TStr& CObjNm, TStrV& ObjNmV) const {
    DimV[DimN]->GetCObjFromDesc(DescStr, CObjNm, ObjNmV);}
  TStrV GetAllObjDescV(const int& DimN) const {
    return DimV[DimN]->GetAllObjDescV();}

  // clipboard task
  bool IsCpbTask(PSchTask& _CpbTask) const {
    if (CpbTask.Empty()){return false;}
    else {_CpbTask=CpbTask; return true;}}
  bool IsCpbTask() const {
    PSchTask _CpbTask; return IsCpbTask(_CpbTask);}
  void PutCpbTask(const PSchTask& _CpbTask){
    CpbTask=_CpbTask;}

  // tasks
  bool IsOkForAddTask(TStr& MsgStr) const;
  int AddTask(const PSchTask& Task);
  int AddTask(
   const TVec<TStrV>& DimObjVV,
   const TSecTm& StartTm, const TSecTm& EndTm, const bool& Forced);
  int AddTask(
   const TStr& DimObj1, const TStr& DimObj2, const TStr& DimObj3,
   const TSecTm& StartTm, const TSecTm& EndTm, const bool& Forced);
  int AddTask(
   const TStrV& DimObjV,
   const TSecTm& StartTm, const TSecTm& EndTm, const bool& Forced);
  int AddTask(
   const TStrV& DimObjV1, const TStrV& DimObjV2, const TStrV& DimObjV3,
   const TSecTm& StartTm, const TSecTm& EndTm, const bool& Forced);
  void DelTask(const int& TaskId);
  PSchTask GetTask(const int& TaskId) const {return IdToTaskH.GetDat(TaskId);}
  void GetDayTaskV(const TSecTm& DtTm, TIntV& TaskIdV) const;
  bool IsTask(const int& TaskId, PSchTask& Task) const;
  bool IsTaskOk(const PSchTask& Task, TSchTaskV& DimIcTaskV) const;
  bool IsTaskOk(const PSchTask& Task) const {
    TSchTaskV DimIcTaskV; return IsTaskOk(Task, DimIcTaskV);}
  bool IsTaskInTm(const TStr& DimNm, const TStr& ObjNm,
   const TSecTm& MnTm, const TSecTm& MxTm, PSchTask& Task) const {
    int DimN=GetDimN(DimNm);
    return DimV[DimN]->IsTaskInTm(ObjNm, MnTm, MxTm, Task);}
  bool IsTaskSim(const PSchTask& Task, PSchTask& SimTask) const;

  // patterns
  bool IsPat(const TStr& PatNm) const {
    return NmToPatH.IsKey(PatNm);}
  bool IsPat(const TStr& PatNm, PSch& Pat) const {
    return NmToPatH.IsKeyGetDat(PatNm, Pat);}
  PSch AddPat(const TStr& PatNm);
  void DelPat(const TStr& PatNm){
    NmToPatH.DelKey(PatNm);}
  PSch GetPat(const TStr& PatNm) const {
    return NmToPatH.GetDat(PatNm);}
  TStrV GetPatNmV() const {
    TStrV PatNmV; NmToPatH.GetKeyV(PatNmV); return PatNmV;}
  void SyncPatObj(const TStr& PatNm) const;
  void SyncPatVObj() const;

  // apply pattern
  void ApplyPatAdd(
   const PSch& Pat, const TSecTm& StartDtTm, const TSecTm& EndDtTm,
   const bool& TestOnlyP, TStrV& LogStrV);
  void ApplyPatDel(
   const PSch& Pat, const TSecTm& StartDtTm, const TSecTm& EndDtTm,
   const bool& TestOnlyP, TStrV& LogStrV);

  // text file
  static PSch LoadTxt(TILx& Lx);
  static PSch LoadTxt(const TStr& FNm);
  void SaveTxt(TOLx& Lx) const;
  void SaveTxt(const TStr& FNm) const;

  friend PSch;
};

#endif

