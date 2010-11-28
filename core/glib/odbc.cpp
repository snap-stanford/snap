#include "odbc.h"

#define ODBC_STR_LEN 254+1

/////////////////////////////////////////////////
// Odbc-Database
TOdbcDb::TOdbcDb():
  Ok(true), MsgStr("Ok"),
  EnvHndP(false), EnvHnd(),
  DbcHndP(false), DbcHnd(),
  SchemaNmMapH(){
  if (Ok){ // allocate environment-handle
    OdbcAssert(SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &EnvHnd));
    EnvHndP=true;}
  if (Ok){ // set odbc-version
    OdbcAssert(SQLSetEnvAttr(
     EnvHnd, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0));}
  if (Ok){ // allocate database-connection-handle
    OdbcAssert(SQLAllocHandle(SQL_HANDLE_DBC, EnvHnd, &DbcHnd));
    DbcHndP=true;}
  if (Ok){ // set login-timeout
    OdbcAssert(SQLSetConnectAttr(DbcHnd, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0));}
}

TOdbcDb::~TOdbcDb(){
  if (DbcHndP){ // release environment-handle
    SQLFreeHandle(SQL_HANDLE_DBC, DbcHnd);}
  if (EnvHndP){ // release database-connection-handle
    SQLFreeHandle(SQL_HANDLE_ENV, EnvHnd);}
}

void TOdbcDb::GetTbNmV(TStrV& TbNmV) const {
  #define ODBC_STR_LEN 254+1
  SQLCHAR       szCatalog[ODBC_STR_LEN], szSchema[ODBC_STR_LEN];
  SQLCHAR       szTableName[ODBC_STR_LEN], szTableTypeName[ODBC_STR_LEN];
  SQLCHAR       szRemarks[ODBC_STR_LEN];
  SQLHSTMT      StmtHnd;
  SQLAllocHandle(SQL_HANDLE_STMT, DbcHnd, &StmtHnd);

  /* Declare buffers for bytes available to return */
  SQLLEN cbCatalog, cbSchema, cbTableName, cbTableTypeName, cbRemarks;

  SQLRETURN RetCd=SQLTables(StmtHnd,
   NULL, 0, /* All catalogs */
   NULL, 0, /* All schemas */
   NULL, 0, /* All tables */
   NULL, 0); /* All table-typescolumns */

  TbNmV.Clr();
  if (RetCd == SQL_SUCCESS || RetCd == SQL_SUCCESS_WITH_INFO){
    /* Bind columns in result set to buffers */
    SQLBindCol(StmtHnd, 1, SQL_C_CHAR, szCatalog, ODBC_STR_LEN, &cbCatalog);
    SQLBindCol(StmtHnd, 2, SQL_C_CHAR, szSchema, ODBC_STR_LEN, &cbSchema);
    SQLBindCol(StmtHnd, 3, SQL_C_CHAR, szTableName, ODBC_STR_LEN, &cbTableName);
    SQLBindCol(StmtHnd, 4, SQL_C_CHAR, szTableTypeName, ODBC_STR_LEN, &cbTableTypeName);
    SQLBindCol(StmtHnd, 5, SQL_C_CHAR, szRemarks, ODBC_STR_LEN, &cbRemarks);
    //
    forever{
      RetCd=SQLFetch(StmtHnd);
      if (RetCd==SQL_SUCCESS || RetCd==SQL_SUCCESS_WITH_INFO){
        TbNmV.Add((char*)szTableName);
        //printf("[%s]", szTableName);
      } else {
        break;
      }
    }
  }
  SQLFreeHandle(SQL_HANDLE_STMT, StmtHnd);
}

void TOdbcDb::GetColNmV(const TStr& TbNm, TStrV& ColNmV) const {
  /* Declare buffers for result set data */
  SQLCHAR       szCatalog[ODBC_STR_LEN], szSchema[ODBC_STR_LEN];
  SQLCHAR       szTableName[ODBC_STR_LEN], szColumnName[ODBC_STR_LEN];
  SQLCHAR       szTypeName[ODBC_STR_LEN], szRemarks[ODBC_STR_LEN];
  SQLCHAR       szColumnDefault[ODBC_STR_LEN], szIsNullable[ODBC_STR_LEN];
  SQLINTEGER    ColumnSize, BufferLength, CharOctetLength, OrdinalPosition;
  SQLSMALLINT   DataType, DecimalDigits, NumPrecRadix, Nullable;
  SQLSMALLINT   SQLDataType, DatetimeSubtypeCode;
  SQLRETURN     RetCd;
  SQLHSTMT      StmtHnd;
  SQLAllocHandle(SQL_HANDLE_STMT, DbcHnd, &StmtHnd);

  /* Declare buffers for bytes available to return */
  SQLLEN cbCatalog, cbSchema, cbTableName, cbColumnName;
  SQLLEN cbDataType, cbTypeName, cbColumnSize, cbBufferLength;
  SQLLEN cbDecimalDigits, cbNumPrecRadix, cbNullable, cbRemarks;
  SQLLEN cbColumnDefault, cbSQLDataType, cbDatetimeSubtypeCode, cbCharOctetLength;
  SQLLEN cbOrdinalPosition, cbIsNullable;


  RetCd=SQLColumns(StmtHnd,
   NULL, 0, /* All catalogs */
   NULL, 0, /* All schemas */
   (uchar*)TbNm.CStr(), SQL_NTS, /* table-name */
   NULL, 0); /* All columns */

  ColNmV.Clr();
  if (RetCd == SQL_SUCCESS || RetCd == SQL_SUCCESS_WITH_INFO){
    /* Bind columns in result set to buffers */
    SQLBindCol(StmtHnd, 1, SQL_C_CHAR, szCatalog, ODBC_STR_LEN, &cbCatalog);
    SQLBindCol(StmtHnd, 2, SQL_C_CHAR, szSchema, ODBC_STR_LEN, &cbSchema);
    SQLBindCol(StmtHnd, 3, SQL_C_CHAR, szTableName, ODBC_STR_LEN, &cbTableName);
    SQLBindCol(StmtHnd, 4, SQL_C_CHAR, szColumnName, ODBC_STR_LEN, &cbColumnName);
    SQLBindCol(StmtHnd, 5, SQL_C_SSHORT, &DataType, 0, &cbDataType);
    SQLBindCol(StmtHnd, 6, SQL_C_CHAR, szTypeName, ODBC_STR_LEN, &cbTypeName);
    SQLBindCol(StmtHnd, 7, SQL_C_SLONG, &ColumnSize, 0, &cbColumnSize);
    SQLBindCol(StmtHnd, 8, SQL_C_SLONG, &BufferLength, 0, &cbBufferLength);
    SQLBindCol(StmtHnd, 9, SQL_C_SSHORT, &DecimalDigits, 0, &cbDecimalDigits);
    SQLBindCol(StmtHnd, 10, SQL_C_SSHORT, &NumPrecRadix, 0, &cbNumPrecRadix);
    SQLBindCol(StmtHnd, 11, SQL_C_SSHORT, &Nullable, 0, &cbNullable);
    SQLBindCol(StmtHnd, 12, SQL_C_CHAR, szRemarks, ODBC_STR_LEN, &cbRemarks);
    SQLBindCol(StmtHnd, 13, SQL_C_CHAR, szColumnDefault, ODBC_STR_LEN, &cbColumnDefault);
    SQLBindCol(StmtHnd, 14, SQL_C_SSHORT, &SQLDataType, 0, &cbSQLDataType);
    SQLBindCol(StmtHnd, 15, SQL_C_SSHORT, &DatetimeSubtypeCode, 0, &cbDatetimeSubtypeCode);
    SQLBindCol(StmtHnd, 16, SQL_C_SLONG, &CharOctetLength, 0, &cbCharOctetLength);
    SQLBindCol(StmtHnd, 17, SQL_C_SLONG, &OrdinalPosition, 0, &cbOrdinalPosition);
    SQLBindCol(StmtHnd, 18, SQL_C_CHAR, szIsNullable, ODBC_STR_LEN, &cbIsNullable);
    forever{
      RetCd=SQLFetch(StmtHnd);
      if (RetCd==SQL_SUCCESS || RetCd==SQL_SUCCESS_WITH_INFO){
        ColNmV.Add((char*)szColumnName);
        //printf("[%s]", szColumnName);
      } else {
        break;
      }
    }
  }
  SQLFreeHandle(SQL_HANDLE_STMT, StmtHnd);
}

void TOdbcDb::GetDataSourceNmV(TStrV& DataSourceNmV) const {
  DataSourceNmV.Clr();
  if (Ok){
    // server & description strings & string-lengths
    SQLCHAR SrvNm[SQL_MAX_DSN_LENGTH+1];
    SQLSMALLINT SrvNmLen;
    SQLCHAR DescStr[SQL_MAX_DSN_LENGTH+1];
    SQLSMALLINT DescStrLen;
    // get first data-source
    SQLRETURN RetCd=SQLDataSources(EnvHnd, SQL_FETCH_FIRST,
     SrvNm, SQL_MAX_DSN_LENGTH+1, &SrvNmLen,
     DescStr, SQL_MAX_DSN_LENGTH+1, &DescStrLen);
    while ((RetCd==SQL_SUCCESS)||(RetCd==SQL_SUCCESS_WITH_INFO)){
      DataSourceNmV.Add((char*)SrvNm);
      RetCd=SQLDataSources(EnvHnd, SQL_FETCH_NEXT,
       SrvNm, SQL_MAX_DSN_LENGTH+1, &SrvNmLen,
       DescStr, SQL_MAX_DSN_LENGTH+1, &DescStrLen);
    }
  }
}

bool TOdbcDb::Connect(
 const TStr& SrvNm, const TStr& UsrNm, const TStr& PwdStr){
  if (Ok){
    OdbcAssert(SQLConnect(DbcHnd,
     (SQLCHAR*)SrvNm.CStr(), SQL_NTS,
     (SQLCHAR*)UsrNm.CStr(), SQL_NTS,
     (SQLCHAR*)PwdStr.CStr(), SQL_NTS));
  }
  return Ok;
}

bool TOdbcDb::Disconnect(){
  if (DbcHndP){ // release environment-handle
    SQLFreeHandle(SQL_HANDLE_DBC, DbcHnd);
    DbcHndP=false;
  }
  if (EnvHndP){ // release database-connection-handle
    SQLFreeHandle(SQL_HANDLE_ENV, EnvHnd);
    EnvHndP=false;
  }
  return true;
}

bool TOdbcDb::IsConnected(){
  if (Ok){
    SQLINTEGER ConnP;
    OdbcAssert(SQLGetConnectAttr(DbcHnd,
     SQL_ATTR_CONNECTION_DEAD, (SQLPOINTER)&ConnP, 0, NULL));
    if (Ok){
      return ConnP==SQL_CD_FALSE;
    }
  }
  return Ok;
}

void TOdbcDb::OdbcAssert(const SQLRETURN& RetCd){
  if ((RetCd==SQL_SUCCESS)||(RetCd==SQL_SUCCESS_WITH_INFO)){
    Ok=true; MsgStr="Ok";
  } else {
    Ok=false; MsgStr=TStr::Fmt("Odbc-Db Error: %d", int(RetCd));
  }
}

TStr TOdbcDb::GetOdbcMsgStr(
 const SQLRETURN& RetCd, const SQLSMALLINT& HndType, const SQLHANDLE& Handle){
  // prepare message-string;
  TChA MsgChA;
  // if message is waiting
  if ((RetCd==SQL_ERROR)||(RetCd==SQL_SUCCESS_WITH_INFO)){
    // parameters for SQLGetDiagRec
    SQLSMALLINT RecN=0;
    SQLCHAR SqlState[6];
    SQLINTEGER NativeError;
    SQLCHAR MsgCStr[SQL_MAX_MESSAGE_LENGTH];
    SQLSMALLINT MsgCStrLen;
    // traverse errors
    SQLRETURN DiagRetCd;
    do {
      RecN++;
      DiagRetCd=SQLGetDiagRec(
       HndType, Handle, RecN, SqlState, &NativeError,
       MsgCStr, sizeof(MsgCStr), &MsgCStrLen);
      // if message was retrieved
      if (DiagRetCd==SQL_SUCCESS){
        if (!MsgChA.Empty()){MsgChA+=' ';}
        MsgChA+='['; MsgChA+=(char*)MsgCStr; MsgChA+=']';
      }
    } while (DiagRetCd!=SQL_NO_DATA);
  }
  // if empty message-string, assign 'ok'
  if (MsgChA.Empty()){
    MsgChA="Ok";}
  // return message-string
  return MsgChA;
}

const TStr TOdbcDb::NullStr="NULL";

/////////////////////////////////////////////////
// Odbc-Query
TOdbcQuery::TOdbcQuery(const POdbcDb& _Db):
  Ok(false), OpOk(false), MsgStr("NoOp"),
  Db(_Db),
  StmtHndP(false), StmtHnd(){
  if (Db->IsOk()){ // allocate statement-handle
    OpAssert(SQLAllocHandle(SQL_HANDLE_STMT, Db->DbcHnd, &StmtHnd));
    if (OpOk){Ok=true; StmtHndP=true;} else {Ok=false;}
  }
}

TOdbcQuery::~TOdbcQuery(){
  if (StmtHndP){ // release statement-handle
    SQLFreeHandle(SQL_HANDLE_STMT, StmtHnd);}
}

bool TOdbcQuery::ExeSql(const TStr& SqlStr, const TStrV& ParamStrV){
  if (Ok){
    // prepare sql-statement
    OpAssert(SQLPrepare(StmtHnd, (SQLCHAR*)SqlStr.CStr(), SQL_NTS));
    // bind parameter-values
    for (int ParamN=0; ParamN<ParamStrV.Len(); ParamN++){
      TStr ParamStr=ParamStrV[ParamN];
      // bind parameter-value
      if (OpOk){
        if (ParamStr.Empty()){
          // put the NULL value 
          SQLLEN StrLenOrInd=SQL_NULL_DATA;
          OpAssert(SQLBindParameter(
           StmtHnd, 1+ParamN, SQL_PARAM_INPUT,
           SQL_C_CHAR, SQL_CHAR,
           1, 0, NULL, 0, &StrLenOrInd));
        } else
        if (TTm::GetTmFromWebLogDateTimeStr(ParamStr).IsDef()){
          // if the value is date
          SQLLEN StrLenOrInd=SQL_NTS;
          OpAssert(SQLBindParameter(
           StmtHnd, 1+ParamN, SQL_PARAM_INPUT,
           SQL_C_CHAR, SQL_TYPE_TIMESTAMP,
           (SQLLEN)ParamStr.Len(), 0,
           (SQLCHAR*)ParamStr.CStr(),
           (SQLLEN)ParamStr.Len(),
           &StrLenOrInd));
        } else {
          // if the value is of any other type
          SQLLEN StrLenOrInd=SQL_NTS;
          OpAssert(SQLBindParameter(
           StmtHnd, 1+ParamN, SQL_PARAM_INPUT,
           SQL_C_CHAR, SQL_CHAR,
           (SQLLEN)ParamStr.Len(), 0,
           (SQLCHAR*)ParamStr.CStr(),
           (SQLLEN)ParamStr.Len(),
           &StrLenOrInd));
        }
      }
    }
    // execute sql-statement
    if (OpOk){
      OpAssert(SQLExecute(StmtHnd));
    }
    // return success
    return OpOk;
  } else {
    OpOk=false;
    return false;
  }
}

bool TOdbcQuery::IsResultSet(){
  if (Ok){
    int ResultCols=GetCols();
    if (OpOk){return ResultCols>0;} else {return false;}
  } else {
    OpOk=false;
    return false;
  }
}

int TOdbcQuery::GetCols(){
  if (Ok){
    SQLSMALLINT Cols;
    OpAssert(SQLNumResultCols(StmtHnd, &Cols));
    if (OpOk){return Cols;} else {return -1;}
  } else {
    OpOk=false;
    return -1;
  }
}

SQLLEN TOdbcQuery::GetRowsAffected(){
  if (Ok){
    SQLLEN RowCount;
    OpAssert(SQLRowCount(StmtHnd, &RowCount));
    if (OpOk){return RowCount;} else {return -1;}
  } else {
    OpOk=false;
    return -1;
  }
}

TStr TOdbcQuery::GetColNm(const int& ColN){
  if (Ok){
    // parameters
    SQLCHAR ColNm[SQL_MAX_COLUMN_NAME_LEN+1];
    SQLSMALLINT ColNmLen;
    SQLSMALLINT DataType;
    SQLULEN ColumnSize;
    SQLSMALLINT DecimalDigits;
    SQLSMALLINT Nullable;
    // get column-data
    OpAssert(SQLDescribeCol(
     StmtHnd, (SQLSMALLINT)(1+ColN),
     ColNm, SQL_MAX_COLUMN_NAME_LEN+1, &ColNmLen,
     &DataType, &ColumnSize, &DecimalDigits, &Nullable));
    // return result
    if (OpOk){
      TStr Nm=(char*)ColNm;
      if (Db->HasSchemaNmMap(Nm)){Nm=Db->MapSchemaNm(Nm);}
      return Nm;
    } else {return "";}
  } else {
    OpOk=false;
    return "";
  }
}

bool TOdbcQuery::NextRow(){
  if (Ok){
    // fetch next-row
    SQLRETURN RetCd=SQLFetch(StmtHnd);
    // return result
    if ((RetCd==SQL_SUCCESS)||(RetCd==SQL_SUCCESS_WITH_INFO)){
      OpOk=true;
      return true;
    } else
    if (RetCd==SQL_NO_DATA){
      OpOk=true;
      return false;
    } else {
      OpOk=false;
      MsgStr=TOdbcDb::GetOdbcMsgStr(RetCd, SQL_HANDLE_STMT, StmtHnd);
      return false;
    }
  } else {
    OpOk=false;
    return false;
  }
}

TStr TOdbcQuery::GetColVal(const int& ColN, const int& MxValStrLen){
  if (Ok){
    /* identification of data type
    {SQLSMALLINT MxColNmLen=255;
    SQLSMALLINT ColNmLen=0;
    TChA ColNmChA(MxColNmLen);
    SQLSMALLINT DataType;
    SQLUINTEGER ColSize;
    SQLSMALLINT DecDigits;
    SQLSMALLINT Nullable;
    SQLDescribeCol(
     StmtHnd,
     (SQLSMALLINT)(1+ColN),
     ColNmChA.CStr(),
     MxColNmLen,
     &ColNmLen,
     &DataType,
     &ColSize,
     &DecDigits,
     &Nullable);
   if ((2<=DataType)&&(DataType<=8)&&(DataType!=3)){
    printf("[%s:%d:%d]\n", ColNmChA.CStr(), int(DataType), SQL_NUMERIC);
   }
   } */

    // parameters
    TChA ValChA(MxValStrLen);
    SQLLEN StrLenOrInd;
    // get column-value
    SQLRETURN RetCd=SQLGetData(
     StmtHnd, (SQLSMALLINT)(1+ColN),
     SQL_C_CHAR, ValChA.CStr(), (SQLINTEGER)MxValStrLen, &StrLenOrInd);
    // return result
    if ((RetCd==SQL_SUCCESS)||(RetCd==SQL_SUCCESS_WITH_INFO)){
      OpOk=true;
      if (StrLenOrInd==SQL_NULL_DATA){
        ValChA=TOdbcDb::NullStr;}
      return ValChA;
    } else
    if (RetCd==SQL_NO_DATA){
      OpOk=true;
      return "";
    } else {
      OpOk=false;
      MsgStr=TOdbcDb::GetOdbcMsgStr(RetCd, SQL_HANDLE_STMT, StmtHnd);
      return "";
    }
  } else {
    OpOk=false;
    return "";
  }
}

void TOdbcQuery::OpAssert(const SQLRETURN& RetCd){
  if ((RetCd==SQL_SUCCESS)||(RetCd==SQL_SUCCESS_WITH_INFO)){
    OpOk=true; MsgStr="Ok";
  } else {
    OpOk=false;
    if (StmtHndP){
      MsgStr=TOdbcDb::GetOdbcMsgStr(RetCd, SQL_HANDLE_STMT, StmtHnd);
    } else {
      MsgStr=TStr::Fmt("Odbc-Query Error: %d", int(RetCd));
    }
    SaveToErrLog(MsgStr.CStr());
  }
}

TStr TOdbcQuery::GetSqlStr(const TStr& Str) {
  TChA SqlChA; const int Chs = Str.Len();
  for (int ChN = 0; ChN < Str.Len(); ChN++) {
    const char Ch = Str[ChN];
    switch (Ch) {
      case '\'': 
      case '"':
      case '\\':
      case '%':
      case '_':
        SqlChA += '\\';
    }
    SqlChA += Ch;
  }
  return SqlChA;
}

/////////////////////////////////////////////////
// Odbc-Table
TOdbcTb::TOdbcTb(const POdbcQuery& Query):
  Ok(false), MsgStr(), ColNmToNH(), UcColNmToNH(), ColNmV(), UcColNmV(), ValVV(){
  GetData(Query);
}

void TOdbcTb::GetData(const POdbcQuery& Query){
  // reset data
  Ok=false; MsgStr="NoOp";
  ColNmV.Clr(); UcColNmV.Clr(); ColNmToNH.Clr(); UcColNmToNH.Clr(); ValVV.Clr();
  // get table
  if ((!Query.Empty())&&(Query->IsResultSet())){
    if (!Query->IsOpOk()){MsgStr=Query->GetMsgStr(); return;}
    // get column-names
    int Cols=Query->GetCols();
    if (!Query->IsOpOk()){MsgStr=Query->GetMsgStr(); return;}
    ColNmV.Gen(Cols, 0); UcColNmV.Gen(Cols, 0);
    for (int ColN=0; ColN<Cols; ColN++){
      TStr ColNm=Query->GetColNm(ColN);
      TStr UcColNm=ColNm.GetUc();
      if (!Query->IsOpOk()){MsgStr=Query->GetMsgStr(); return;}
      ColNmV.Add(ColNm); ColNmToNH.AddDat(ColNm, ColN);
      UcColNmV.Add(ColNm); UcColNmToNH.AddDat(UcColNm, ColN);
    }
    // get rows
    int RowN=0;
    while (Query->NextRow()){
      if (!Query->IsOpOk()){MsgStr=Query->GetMsgStr(); return;}
      ValVV.Add(); ValVV.Last().Gen(Cols, 0);
      for (int ColN=0; ColN<Cols; ColN++){
        TStr ColVal=Query->GetColVal(ColN);
        if (!Query->IsOpOk()){MsgStr=Query->GetMsgStr(); return;}
        if (ColVal!=TOdbcDb::NullStr){ValVV.Last().Add(ColVal);}
        else {ValVV.Last().Add("");}
      }
      RowN++;
    }
    Ok=true;
  }
}

int TOdbcTb::GetColN(const TStr& ColNm) const {
  TInt ColN;
  if (ColNmToNH.IsKeyGetDat(ColNm, ColN)){
    return ColN;
  } else {
    return UcColNmToNH.GetDat(ColNm.GetUc());
  }
}

TStr TOdbcTb::GetRowVal(const int& RowN, const int& ColN) const {
  TStr ValStr=ValVV[RowN][ColN];
  /*double Val;
  if (ValStr.IsFlt(false, 0, 0, Val, ',')){
    ValStr.ChangeChAll(',', '.');}*/
  return ValStr;
}

TTm TOdbcTb::GetRowValTm(const int& RowN, const TStr& ColNm) const {
  TStr TmStr=GetRowVal(RowN, GetColN(ColNm));
  TTm Tm=TTm::GetTmFromWebLogDateTimeStr(TmStr);
  return Tm;
}

void TOdbcTb::Dump() const {
  if (Ok){
    for (int ColN=0; ColN<GetCols(); ColN++){
      printf("C%d:[%s]", ColN, GetColNm(ColN).CStr());}
    printf("\n");
    for (int RowN=0; RowN<GetRows(); RowN++){
      printf("R%d:", RowN);
      for (int ColN=0; ColN<GetCols(); ColN++){
        TStr ColVal=GetRowVal(RowN, ColN);
        printf(" C%d:[%s]", ColN, ColVal.CStr());
      }
      printf("\n");
    }
  } else {
    printf("Error\n");
  }
}

