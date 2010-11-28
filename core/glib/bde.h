#ifndef bde_h
#define bde_h

/////////////////////////////////////////////////
// Includes

#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\DB.hpp>
#include <vcl\DBTables.hpp>

#include "base.h"

/////////////////////////////////////////////////
// BDE-DataBase
ClassTP(TBdeDb, PBdeDb)//{
protected:
  static int LastDbId;
  TDatabase *Db;
  bool Ok;
  TStr ErrMsg;
  int GetDbId(){return ++LastDbId;}
  void ConnDb();
public:
  TBdeDb();
  virtual ~TBdeDb();
  TBdeDb(TSIn&){Fail;}
  static PBdeDb Load(TSIn& SIn){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  bool IsOk() const {return Ok;}
  TStr GetErrMsg() const {return ErrMsg;}

  TStr GetNm() const {return Db->DatabaseName.c_str();}
  void Open() const {Db->Open();}
  void Close() const {Db->Close();}
};

/////////////////////////////////////////////////
// DBase-Data-Base
class TDbfDb: public TBdeDb{
public:
  TDbfDb(const TStr& FPath);
};

/////////////////////////////////////////////////
// Paradox-Data-Base
class TPxDb: public TBdeDb{
public:
  TPxDb(const TStr& FPath);
};

/////////////////////////////////////////////////
// Access-Data-Base
class TAccDb: public TBdeDb{
public:
  TAccDb(const TStr& OdbcDsnNm);
};

/////////////////////////////////////////////////
// InterBase-Data-Base
class TIbDb: public TBdeDb{
public:
  TIbDb(const TStr& FNm, const TStr& UsrNm="SYSDBA", const TStr& PwdStr="masterkey");
};

/////////////////////////////////////////////////
// SqlServer-Data-Base
class TSqlSrvDb: public TBdeDb{
public:
  TSqlSrvDb(const TStr& DbNm, const TStr& UsrNm, const TStr& PwdStr);
  static PBdeDb New(const TStr& DbNm, const TStr& UsrNm, const TStr& PwdStr){
    return new TSqlSrvDb(DbNm, UsrNm, PwdStr);}
  TSqlSrvDb(const TStr& AliasNm);
  static PBdeDb New(const TStr& AliasNm){
    return new TSqlSrvDb(AliasNm);}
};

/////////////////////////////////////////////////
// Oracle-DataBase
class TOracleDb: public TBdeDb{
public:
  TOracleDb(
   const TStr& OdbcDsnNm, const TStr& UsrNm=TStr(), const TStr& PwdStr=TStr());
};

/////////////////////////////////////////////////
// Bde-Query
ClassTP(TBdeQuery, PBdeQuery)//{
private:
  TQuery* Query;
  bool Ok;
  TStr ErrMsg;
  UndefDefaultCopyAssign(TBdeQuery);
public:
  TBdeQuery(const PBdeDb& BdeDb);
  static PBdeQuery New(const PBdeDb& BdeDb){
    return PBdeQuery(new TBdeQuery(BdeDb));}
  ~TBdeQuery();
  static PBdeQuery Load(TSIn& SIn){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TQuery* GetQuery(){return Query;}
  bool IsOk(){return Ok;}
  TStr GetErrMsg(){return ErrMsg;}

  void Execute(const TStr& QueryStr);
  void Select(const TStr& QueryStr);

  int GetRecs(){return Query->RecordCount;}
  void First(){Query->First();}
  void Next(){Query->Next();}
  bool Eof(){return Query->Eof;}

  int GetFlds(){
    return Query->FieldCount;}
  TStr GetFldNm(const int& FldN){
    return Query->Fields->Fields[FldN]->FieldName.c_str();}
  void GetFldNmV(TStrV& FldNmV){
    int Flds=GetFlds(); FldNmV.Gen(Flds, 0);
    for (int FldN=0; FldN<Flds; FldN++){FldNmV.Add(GetFldNm(FldN));}}

  TStr GetValStr(const int& FldN){
    return Query->Fields->Fields[FldN]->AsString.c_str();}
  void GetValStrV(TStrV& ValStrV){
    int Flds=GetFlds(); ValStrV.Gen(Flds, 0);
    for (int FldN=0; FldN<Flds; FldN++){ValStrV.Add(GetValStr(FldN));}}
  TStr GetValStr(const TStr& FldNm){
    return Query->FieldByName(FldNm.CStr())->AsString.c_str();}
  bool GetValBool(const TStr& FldNm){
    return Query->FieldByName(FldNm.CStr())->AsInteger!=0;}
  int GetValInt(const int& FldN){
    return Query->Fields->Fields[FldN]->AsInteger;}
  int GetValInt(const TStr& FldNm){
    return Query->FieldByName(FldNm.CStr())->AsInteger;}
  double GetValFlt(const TStr& FldNm){
    return Query->FieldByName(FldNm.CStr())->AsFloat;}
};

/////////////////////////////////////////////////
// Bde-Table
ClassTP(TBdeTable, PBdeTable)//{
private:
  TTable* Table;
  UndefDefaultCopyAssign(TBdeTable);
public:
  TBdeTable(const PBdeDb& BdeDb, const TStr& TableNm);
  static PBdeTable New(const PBdeDb& BdeDb, const TStr& TableNm){
    return PBdeTable(new TBdeTable(BdeDb, TableNm));}
  ~TBdeTable();
  static PBdeTable Load(TSIn& SIn){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TTable* GetTable() const {return Table;}
  void Open() const {Table->Open();}
  void Close() const {Table->Close();}

  void First() const {Table->First();}
  void Next() const {Table->Next();}
  bool Eof() const {return Table->Eof;}
  int GetRecs() const {return Table->RecordCount;}
  void Edit() const {Table->Edit();}
  void Post() const {Table->Post();}

  int GetFlds() const {
    return Table->FieldCount;}
  TStr GetFldNm(const int& FldN) const {
    return Table->Fields->Fields[FldN]->FieldName.c_str();}

  TStr GetValStr(const int& FldN) const {
    return Table->Fields->Fields[FldN]->AsString.c_str();}
  void PutValStr(const TStr& FldNm, const TStr& FldVal) const {
    Table->FieldByName(FldNm.CStr())->AsString=FldVal.CStr();}
  TStr GetValStr(const TStr& FldNm) const {
    return Table->FieldByName(FldNm.CStr())->AsString.c_str();}
  bool GetValBool(const TStr& FldNm) const {
    return Table->FieldByName(FldNm.CStr())->AsInteger!=0;}
  int GetValInt(const TStr& FldNm) const {
    return Table->FieldByName(FldNm.CStr())->AsInteger;}
  double GetValFlt(const TStr& FldNm) const {
    return Table->FieldByName(FldNm.CStr())->AsFloat;}
};

#endif
