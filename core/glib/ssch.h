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

class TSch;
typedef TPt<TSch> PSch;

/////////////////////////////////////////////////
// Schedule-Task
class TSchTask{
private:
  TCRef CRef;
private:
  TInt TaskId;
  TStrV DimObjNmV;
  TSecTm StartTm, EndTm;
public:
  TSchTask(): TaskId(-1), DimObjNmV(), StartTm(), EndTm(){}
  TSchTask(const int& _TaskId, const TStrV& _DimObjNmV,
   const TSecTm& _StartTm, const TSecTm& _EndTm);
  ~TSchTask(){}
  TSchTask(TSIn& SIn):
    TaskId(SIn), DimObjNmV(SIn), StartTm(SIn), EndTm(SIn){}
  void Save(TSOut& SOut){
    TaskId.Save(SOut); DimObjNmV.Save(SOut);
    StartTm.Save(SOut); EndTm.Save(SOut); }

  TSchTask& operator=(const TSchTask&){Fail; return *this;}

  // members
  int GetTaskId() const {return TaskId;}
  int GetDims() const {return DimObjNmV.Len();}
  TStr GetDimObjNm(const int& DimN) const {return DimObjNmV[DimN];}
  TSecTm GetStartTm() const {return StartTm;}
  TSecTm GetEndTm() const {return EndTm;}

  // operations
  bool IsTmIn(const TSecTm& Tm) const {
    return (StartTm<=Tm)&&(Tm<EndTm);}
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
  TVec<TTmTaskKd> TaskV;
public:
  TSchObj(): Nm(), TaskV(){}
  TSchObj(const TStr& _Nm): Nm(_Nm), TaskV(){}
  ~TSchObj(){}
  TSchObj(TSIn& SIn): Nm(SIn), TaskV(SIn){}
  void Save(TSOut& SOut){Nm.Save(SOut); TaskV.Save(SOut);}

  TSchObj& operator=(const TSchObj&){Fail; return *this;}

  // name
  TStr GetNm() const {return Nm;}

  // tasks
  void AddTask(const PSchTask& Task);
  void DelTask(const PSchTask& Task);
  bool IsTaskAtTm(const TSecTm& Tm, PSchTask& Task) const;
  bool IsTaskInTm(const TSecTm& MnTm, const TSecTm& MxTm, PSchTask& Task) const;
  bool IsTaskOk(const PSchTask& Task, PSchTask& IcTask) const {
    return !IsTaskInTm(Task->GetStartTm(), Task->GetEndTm()-1, IcTask);}
  bool IsTaskOk(const PSchTask& Task) const {
    PSchTask IcTask; return IsTaskOk(Task, IcTask);}

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
  TNmToObjH NmToObjH;
public:
  TSchDim(): DimN(-1), Nm(), NmToObjH(){}
  TSchDim(const int& _DimN, const int& ExpObjs):
    DimN(_DimN), Nm(), NmToObjH(ExpObjs){}
  ~TSchDim(){}
  TSchDim(TSIn& SIn): DimN(SIn), Nm(SIn), NmToObjH(SIn){}
  void Save(TSOut& SOut){DimN.Save(SOut); Nm.Save(SOut); NmToObjH.Save(SOut);}

  TSchDim& operator=(const TSchDim&){Fail; return *this;}

  // dim & name
  int GetDimN() const {return DimN;}
  void PutNm(const TStr& _Nm){Nm=_Nm;}
  TStr GetNm() const {return Nm;}

  // objects
  bool IsObj(const TStr& ObjNm) const {
    return NmToObjH.IsKey(ObjNm);}
  int AddObj(const TStr& Nm);
  PSchObj GetObj(const TStr& Nm) const {
    return NmToObjH.GetDat(Nm);}
  TStrV GetObjNmV() const;

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
typedef TVec<PSchDim> TSchDefV;

/////////////////////////////////////////////////
// Schedule
class TSch{
private:
  TCRef CRef;
private:
  typedef THash<TInt, PSchTask> TIdToTaskH;
  TStr Nm;
  TSchDefV DimV;
  TInt LastTaskId;
  TIdToTaskH IdToTaskH;
  int GetNewTaskId(){return LastTaskId++;}
public:
  TSch(): Nm(), DimV(), LastTaskId(-1), IdToTaskH(){}
  TSch(const TStr& _Nm, const int& Dims,
   const int& ExpObjs=100, const int& ExpTasks=1000);
  ~TSch(){}
  TSch(TSIn& SIn):
    Nm(SIn), DimV(SIn), LastTaskId(SIn), IdToTaskH(SIn){}
  void Save(TSOut& SOut){
    Nm.Save(SOut); DimV.Save(SOut);
    LastTaskId.Save(SOut); IdToTaskH.Save(SOut);}

  TSch& operator=(const TSch&){Fail; return *this;}

  // name
  void PutNm(const TStr& _Nm){Nm=_Nm;}
  TStr GetNm() const {return Nm;}

  // dimensions
  int GetDims() const {return DimV.Len();}
  TStr GetDimNm(const int& DimN) const {return DimV[DimN]->GetNm();}
  TStrV GetDimNmV() const;
  int GetDimN(const TStr& DimNm) const;
  void PutDimNm(const int& DimN, const TStr& DimNm) const {
    DimV[DimN]->PutNm(DimNm);}

  // objects
  bool IsObj(const int& DimN, const TStr& ObjNm) const {
    return DimV[DimN]->IsObj(ObjNm);}
  int AddObj(const int& DimN, const TStr& ObjNm) const {
    return DimV[DimN]->AddObj(ObjNm);}
  int AddObj(const TStr& DimNm, const TStr& ObjNm) const {
    return AddObj(DimNm, ObjNm);}
  TStrV GetObjNmV(const int& DimN) const {
    return DimV[DimN]->GetObjNmV();}

  // tasks
  int AddTask(
   const TStrV& DimObjNmV, const TSecTm& StartTm, const TSecTm& EndTm);
  int AddTask(
   const TStr& DimObjNm1, const TStr& DimObjNm2, const TStr& DimObjNm3,
   const TSecTm& StartTm, const TSecTm& EndTm);
  void DelTask(const int& TaskId);
  bool IsTask(const int& TaskId, PSchTask& Task) const;
  bool IsTaskOk(const PSchTask& Task, TSchTaskV& DimIcTaskV) const;
  bool IsTaskOk(const PSchTask& Task) const {
    TSchTaskV DimIcTaskV; return IsTaskOk(Task, DimIcTaskV);}
  bool IsTaskInTm(const TStr& DimNm, const TStr& ObjNm,
   const TSecTm& MnTm, const TSecTm& MxTm, PSchTask& Task) const {
    int DimN=GetDimN(DimNm);
    return DimV[DimN]->IsTaskInTm(ObjNm, MnTm, MxTm, Task);}

  // text file
  static PSch LoadTxt(TILx& Lx);
  static PSch LoadTxt(const TStr& FNm);
  void SaveTxt(TOLx& Lx) const;
  void SaveTxt(const TStr& FNm) const;

  friend PSch;
};

#endif

