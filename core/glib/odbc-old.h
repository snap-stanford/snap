/////////////////////////////////////////////////
// ODBC-Old Library
/////////////////////////////////////////////////

#ifndef odbcold_h
#define odbcold_h

#include "base.h"

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
//#include <sqlucode.h>
#include <odbcinst.h>


/////////////////////////////////////////////////
// TODBCBase
class TODBCBase{
public:
  typedef char TSQLState[6];
protected:
  SQLHANDLE Handle;
  SQLRETURN SQLReturn;
public:
  SQLHANDLE GetHandle() const {return Handle;}
  SQLRETURN GetSQLReturn() const {return SQLReturn;}

  static bool SQLSuccess(SQLRETURN R){
   return (R==SQL_SUCCESS)||(R==SQL_SUCCESS_WITH_INFO);}
  bool IsOK() const {return SQLSuccess(SQLReturn);}

  bool GetSQLState(const SQLSMALLINT& HandleType, const SQLHANDLE& Handle,
   TSQLState& SQLState) const;
  TStr GetErrMsg(const SQLSMALLINT& HandleType, const SQLHANDLE& Handle) const;

  virtual bool GetSQLState(TSQLState& SQLState) const=0;
  virtual TStr GetErrMsg() const=0;
};

/////////////////////////////////////////////////
// TODBC
class TODBC;
typedef TPt<TODBC> PODBC;
class TODBC: public TODBCBase{
private:
  TCRef CRef;
private:
  // ODBC attribute keywords
  static const TStr oakDSN;
  static const TStr oakUsername;
  static const TStr oakPassword;
  static const TStr oakDatabase;
public:
  // CD
  TODBC(const int& ODBCVersion=3);   // only versions 2 and 3 are supported
  ~TODBC();

  // DSN Functions
  bool AddDataSource(const TStr& DSN, const TStr& DrvDesc,
   const TBool& User=true, const TStr& AttrStr=TStr(),
   const TStr& Username=TStr(), const TStr& Password=TStr(),
   const TStr& Database=TStr()) const;
  bool ModifyDataSource(const TStr& DSN, const TStr& DrvDesc,
   const TBool& User=true, const TStr& AttrStr=TStr(),
   const TStr& Username=TStr(), const TStr& Password=TStr(),
   const TStr& Database=TStr()) const;
  bool RemoveDataSource(const TStr& DSN, const TStr& DrvDesc,
   const TBool& User=true) const;

  // Catalog Functions
  bool GetDataSources(TStrV& DSNV, TStrV& DrvDescV);
  bool GetDrivers(TStrV& DrvDescV, TStrV& DrvAttrV);

  // Housekeeping
  bool GetSQLState(TSQLState& SQLState) const {
    return TODBCBase::GetSQLState(SQL_HANDLE_ENV, Handle, SQLState);}
  TStr GetErrMsg() const {return TODBCBase::GetErrMsg(SQL_HANDLE_ENV, Handle);}

  friend PODBC;
};

/////////////////////////////////////////////////
// TDB
class TDB;
typedef TPt<TDB> PDB;
class TDB: public TODBCBase{
private:
  TCRef CRef;
private:
  TStr DSN, Username, Password;
public:
  // CD
  TDB(const TODBC& ODBC,
   const TStr& _DSN, const TStr& _Username, const TStr& _Password);
  ~TDB();

  TStr GetDSN() const {return DSN;}
  TStr GetUsername() const {return Username;}
  TStr GetPassword() const {return Password;}

  // Housekeeping
  bool GetSQLState(TSQLState& SQLState) const {
    return TODBCBase::GetSQLState(SQL_HANDLE_DBC, Handle, SQLState);}
  TStr GetErrMsg() const {return TODBCBase::GetErrMsg(SQL_HANDLE_DBC, Handle);}

  friend PDB;
};

/////////////////////////////////////////////////
// TQuery
enum TODBCTy {otUndef, otNull, otInt, otFlt, otStr, otTm, otOther};
class TQuery;
typedef TPt<TQuery> PQuery;
class TQuery: public TODBCBase{
private:
  TCRef CRef;
private:
  bool QueryReady;

  // Result Set Internals
  int rsNCols;
  TStrV rsColNmV;
  TIntV rsColOfsV, rsColLenV;
  TVec<SQLSMALLINT> rsColTypeV;
  char* rsColData;
  TStrH rsColNmH;

  SQLSMALLINT rsCType(const SQLINTEGER& Type, const TStr& TyNm) const;
  void rsBindCols();
public:
  // CD
  TQuery(const TDB& DB);
  ~TQuery();

  // Catalog Functions
  bool GetTables();
  bool GetColumns(const TStr& TableNm);

  // SQL Statement
  bool Execute(const TStr& SQLQuery);

  // The above methods fill in the query's result set in order to be processed
  // with Fetch and Get* methods. Use MakeReady to release the half-used result
  // set at any time and make the query ready for use with another Execute or a
  // catalog function.
  void MakeReady();
  bool IsReady() const {return QueryReady;}

  // Result Set
  bool Fetch();

  int GetNFlds() const {return rsNCols;}
  TStr GetFldNm(const int& FldN) const {return rsColNmV[FldN];}
  TODBCTy GetFldTy(const int& FldN) const;
  short GetFldSQLTy(const int& FldN) const;

  bool GetValBool(const int& FldN) const;
  bool GetValBool(const TStr& FldNm) const {
    return GetValBool(rsColNmH.GetDat(FldNm));}
  int GetValInt(const int& FldN) const;
  int GetValInt(const TStr& FldNm) const {
    return GetValInt(rsColNmH.GetDat(FldNm));}
  double GetValFlt(const int& FldN) const;
  double GetValFlt(const TStr& FldNm) const {
    return GetValFlt(rsColNmH.GetDat(FldNm));}
  TTm GetValTm(const int& FldN) const;
  TTm GetValTm(const TStr& FldNm) const {
    return GetValTm(rsColNmH.GetDat(FldNm));}
  TStr GetValStr(const int& FldN) const;
  TStr GetValStr(const TStr& FldNm) const {
    return GetValStr(rsColNmH.GetDat(FldNm));}

  // Housekeeping
  bool GetSQLState(TSQLState& SQLState) const {
    return TODBCBase::GetSQLState(SQL_HANDLE_STMT, Handle, SQLState);}
  TStr GetErrMsg() const {return TODBCBase::GetErrMsg(SQL_HANDLE_STMT, Handle);}

  friend PQuery;
};

#endif
