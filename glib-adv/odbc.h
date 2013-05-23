#ifndef odbc_h
#define odbc_h

#include "base.h"

#include <sql.h>
#include <sqlext.h>

/////////////////////////////////////////////////
// Odbc-Database
ClassTP(TOdbcDb, POdbcDb)//{
private:
  bool Ok; TStr MsgStr;
  bool EnvHndP; SQLHENV EnvHnd;
  bool DbcHndP; SQLHENV DbcHnd;
  TStrStrH SchemaNmMapH;
  UndefCopyAssign(TOdbcDb);
public:
  TOdbcDb();
  static POdbcDb New(){
    return new TOdbcDb();}
  ~TOdbcDb();

  // metadata
  void GetDataSourceNmV(TStrV& DataSourceNmV) const;
  void GetTbNmV(TStrV& TbNmV) const;
  void GetColNmV(const TStr& TbNm, TStrV& ColNmV) const;

  // connection
  bool Connect(const TStr& SrvNm, const TStr& UsrNm, const TStr& PwdStr);
  bool Disconnect();
  bool IsConnected();

  // diagnostics handling
  bool IsOk() const {return Ok;}
  TStr GetMsgStr() const {return MsgStr;}
  void OdbcAssert(const SQLRETURN& RetCd);
  static TStr GetOdbcMsgStr(
   const SQLRETURN& RetCd, const SQLSMALLINT& HndType, const SQLHANDLE& Handle);

  // schema-name mapper
  void ClrSchemaNmMap(){SchemaNmMapH.Clr();}
  void AddSchemaNmMap(const TStr& FromNm, const TStr& ToNm){
    SchemaNmMapH.AddDat(FromNm, ToNm);}
  bool HasSchemaNmMap(const TStr& Nm) const {return SchemaNmMapH.IsKey(Nm);}
  TStr MapSchemaNm(const TStr& Nm) const {
    if (HasSchemaNmMap(Nm)){return SchemaNmMapH.GetDat(Nm);} else {return TStr();}}

  // null string
  static const TStr NullStr;

  friend class TOdbcQuery;
};

/////////////////////////////////////////////////
// Odbc-Query
ClassTP(TOdbcQuery, POdbcQuery)//{
private:
  bool Ok; bool OpOk; TStr MsgStr;
  POdbcDb Db;
  bool StmtHndP; SQLHENV StmtHnd;
  UndefCopyAssign(TOdbcQuery);
public:
  TOdbcQuery(const POdbcDb& _Db);
  static POdbcQuery New(const POdbcDb& Db){
    return new TOdbcQuery(Db);}
  ~TOdbcQuery();

  // statement execution
  bool ExeSql(const TStr& SqlStr, const TStrV& ParamStrV=TStrV());
  bool IsResultSet();
  int GetCols();
  SQLLEN GetRowsAffected();
  TStr GetColNm(const int& ColN);
  bool NextRow();
  TStr GetColVal(const int& ColN, const int& MxValStrLen=10000);

  // diagnostics handling
  bool IsOk() const {return Ok;}
  bool IsOpOk() const {return OpOk;}
  TStr GetMsgStr() const {return MsgStr;}
  void OpAssert(const SQLRETURN& RetCd);

  // escape characters
  static TStr GetSqlStr(const TStr& Str);
};

/////////////////////////////////////////////////
// Odbc-Table
ClassTP(TOdbcTb, POdbcTb)//{
private:
  bool Ok;
  TStr MsgStr;
  TStrIntH ColNmToNH, UcColNmToNH;
  TStrV ColNmV, UcColNmV;
  TVec<TStrV> ValVV;
  UndefCopyAssign(TOdbcTb);
public:
  TOdbcTb(const POdbcQuery& Query);
  static POdbcTb New(const POdbcQuery& Query=NULL){
    return new TOdbcTb(Query);}

  // data retrieval
  void GetData(const POdbcQuery& Query);

  // status
  bool IsOk() const {return Ok;}
  TStr GetMsgStr() const {return MsgStr;}

  // columns
  int GetCols() const {return ColNmV.Len();}
  TStr GetColNm(const int& ColN) const {return ColNmV[ColN];}
  TStr GetUcColNm(const int& ColN) const {return UcColNmV[ColN];}
  int GetColN(const TStr& ColNm) const;

  // rows & values
  bool Empty() const {return GetRows()==0;}
  int GetRows() const {return ValVV.Len();}
  TStr GetRowVal(const int& RowN, const int& ColN) const;
  TStr GetRowVal(const int& RowN, const TStr& ColNm) const {
    return GetRowVal(RowN, GetColN(ColNm));}
  TTm GetRowValTm(const int& RowN, const TStr& ColNm) const;

  // files
  void Dump() const;
};

/////////////////////////////////////////////////
// Input-Odbc
class TOdbcIn: public TSIn {
private:
  POdbcQuery Query;
  PSIn SIn;

  // UndefDefaultCopyAssign
  TOdbcIn(): TSBase("Odbc-Input-Stream") {}
  TOdbcIn(const TOdbcIn&): TSBase("Odbc-Input-Stream") {}
  TOdbcIn& operator=(const TOdbcIn&) {}
public:
  TOdbcIn(const POdbcQuery& _Query, const int& ColN, const int& MxValStrLen=10000):
   TSBase("Odbc-Input-Stream"), SIn(), Query(_Query)  {
    TStr Str=Query->GetColVal(ColN, MxValStrLen);
    SIn=TStrIn::New(Str);
  }
  static PSIn New(const POdbcQuery& Query, const int& ColN, const int& MxValStrLen=10000){
    return PSIn(new TOdbcIn(Query, ColN));}
  ~TOdbcIn(){}

  bool Eof() {return SIn->Eof();}
  int Len() const {return SIn->Len();}
  char GetCh(){return SIn->GetCh();}
  char PeekCh(){return SIn->PeekCh();}
  int GetBf(const void* LBf, const TSize& LBfL){
    return SIn->GetBf(LBf, LBfL);}
};

#endif
