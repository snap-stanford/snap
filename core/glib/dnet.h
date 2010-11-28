#ifndef dnet_h
#define dnet_h

#include <vcclr.h>

using namespace System::Data::SqlClient;

#include "base.h"

/////////////////////////////////////////////////
// General
class TDNet{
public:
  static TStr GetStr(System::String* DNStr);
};

/////////////////////////////////////////////////
// Sql-Exception
class TSqlExcept{
public:
  static TStr GetMsgStr(SqlException* Except);
};

/////////////////////////////////////////////////
// Sql-Status
ClassTP(TSqlStat, PSqlStat)//{
private:
  bool Ok;
  TStr MsgStr;
  UndefDefaultCopyAssign(TSqlStat);
public:
  TSqlStat(const bool& _Ok, const TStr& _MsgStr): 
    Ok(_Ok), MsgStr(_MsgStr){}
  static PSqlStat New(const bool& Ok, const TStr& MsgStr){
    return new TSqlStat(Ok, MsgStr);}

  bool IsOk() const {return Ok;}
  TStr GetMsgStr() const {return MsgStr;}

  static PSqlStat GetOkStat(){
    return New(true, "Ok");}
};

/////////////////////////////////////////////////
// Sql-Connection
typedef enum {scsUndef, scsBroken, scsClosed, scsConnecting, 
 scsExecuting, scsFetching, scsOpen} TSqlConnState;

ClassTP(TSqlConn, PSqlConn)//{
private:
  gcroot<SqlConnection*> Conn;
  UndefCopyAssign(TSqlConn);
public:
  TSqlConn(): 
    Conn(new SqlConnection()){}
  static PSqlConn New(const TStr& ConnStr){
    PSqlConn SqlConn=new TSqlConn();
    SqlConn->GetConn()->ConnectionString=ConnStr.CStr();
    return SqlConn;}
  ~TSqlConn(){Close();}

  SqlConnection* GetConn() const {return Conn;}
  TSqlConnState GetState() const;
  TStr GetSrvVerStr() const {return TDNet::GetStr(Conn->ServerVersion);}
  
  PSqlStat Open();
  void Close(){Conn->Close();}

  static TStr GetConnStr(
    const TStr& SrvNm, const TStr& DbNm, const TStr& UsrNm, const TStr& PwdStr);
};

/////////////////////////////////////////////////
// Sql-Reader
ClassTP(TSqlRd, PSqlRd)//{
private:
  PSqlStat SqlStat;
  gcroot<SqlDataReader*> Rd;
  UndefDefaultCopyAssign(TSqlRd);
public:
  TSqlRd(const PSqlStat& _SqlStat, SqlDataReader* _Rd=NULL): 
    SqlStat(_SqlStat), Rd(_Rd){}
   static PSqlRd New(const PSqlStat& SqlStat, SqlDataReader* Rd=NULL){
     return new TSqlRd(SqlStat, Rd);}
  ~TSqlRd(){Close();}

  // retrieve components
  PSqlStat GetSqlStat() const {return SqlStat;}
  SqlDataReader* GetRd() const {return Rd;}
  
  // status
  bool IsOk() const {return SqlStat->IsOk();}
  TStr GetMsgStr() const {return SqlStat->GetMsgStr();}
  
  // result-set
  void Close() const {
    if (Rd!=NULL){Rd->Close();}}
  bool HasRows() const {
    EAssert(Rd!=NULL); return Rd->HasRows;}
  int GetFlds() const {
    EAssert(Rd!=NULL); return Rd->FieldCount;}
  bool NextRow() const {
    EAssert(Rd!=NULL); return Rd->Read();}
  int GetFldN(const TStr& FldNm) const {
    EAssert(Rd!=NULL); return Rd->GetOrdinal(FldNm.CStr());}
  TStr GetFldNm(const int& FldN) const {
    EAssert(Rd!=NULL); return TDNet::GetStr(Rd->GetName(FldN));}
  TStr GetFldTypeNm(const int& FldN) const {
    EAssert(Rd!=NULL); return TDNet::GetStr(Rd->GetDataTypeName(FldN));}
  TStr GetFldVal(const int& FldN) const;
  TStr GetFldVal(const TStr& FldNm) const;
  TTm GetFldTm(const int& FldN) const;
  TTm GetFldTm(const TStr& FldNm) const {
    return GetFldTm(GetFldN(FldNm));}
};

/////////////////////////////////////////////////
// Sql-Command
ClassTP(TSqlCm, PSqlCm)//{
private:
  gcroot<SqlCommand*> Cm;
  UndefCopyAssign(TSqlCm);
public:
  TSqlCm():
    Cm(new SqlCommand()){}
  static PSqlCm New(
   const PSqlConn& SqlConn, const TStr& CmStr, const TStrPrV& ParamNmValPrV);
  ~TSqlCm(){}

  SqlCommand* GetCm() const {return Cm;}

  PSqlStat ExeNonQuery();
  PSqlRd ExeQuery();
};

/////////////////////////////////////////////////
// Sql-Query
ClassTP(TSqlQuery, PSqlQuery)//{
private:
  PSqlConn SqlConn;
  PSqlCm SqlCm;
  PSqlRd SqlRd;
  PSqlStat SqlStat;
  void CloseSqlRd(){if (!SqlRd.Empty()){SqlRd->Close();}}
  UndefDefaultCopyAssign(TSqlQuery);
public:
  TSqlQuery(const TStr& ConnStr):
    SqlConn(TSqlConn::New(ConnStr)), SqlCm(), SqlRd(),
    SqlStat(TSqlStat::GetOkStat()){}
  static PSqlQuery New(
   const TStr& SrvNm, const TStr& DbNm, const TStr& UsrNm, const TStr& PwdStr){
    return new TSqlQuery(TSqlConn::GetConnStr(SrvNm, DbNm, UsrNm, PwdStr));}

  // execution
  PSqlStat ExeNonQuery(const TStr& CmStr, const TStrPrV& ParamNmValPrV=TStrPrV());
  PSqlStat ExeQuery(const TStr& CmStr, const TStrPrV& ParamNmValPrV=TStrPrV());
  PSqlStat ExeQuery(const TStr& CmStr, 
   const TStr& ParamNm, const TStr& ParamVal);
  PSqlStat ExeQuery(const TStr& CmStr, 
   const TStr& ParamNm1, const TStr& ParamVal1, 
   const TStr& ParamNm2, const TStr& ParamVal2);

  // status
  PSqlStat GetSqlStat() const {return SqlStat;}
  bool IsOk() const {return SqlStat->IsOk();}
  TStr GetMsgStr() const {SqlStat->GetMsgStr();}
  
  // reader
  bool HasRows() const {return SqlRd->HasRows();}
  int GetFlds() const {return SqlRd->GetFlds();}
  bool NextRow() const {return SqlRd->NextRow();}
  int GetFldN(const TStr& FldNm) const {return SqlRd->GetFldN(FldNm);}
  TStr GetFldNm(const int& FldN) const {return SqlRd->GetFldNm(FldN);}
  TStr GetFldTypeNm(const int& FldN) const {return SqlRd->GetFldTypeNm(FldN);}
  TStr GetFldVal(const int& FldN) const {return SqlRd->GetFldVal(FldN);}
  TStr GetFldVal(const TStr& FldNm) const {return SqlRd->GetFldVal(FldNm);}
  TTm GetFldTm(const int& FldN) const {return SqlRd->GetFldTm(FldN);}
  TTm GetFldTm(const TStr& FldNm) const {return SqlRd->GetFldTm(FldNm);}

  // files  
  static void Dump(const PSqlQuery& SqlQuery);
};

#endif
