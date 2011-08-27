/////////////////////////////////////////////////
// ODBC-Old Library
/////////////////////////////////////////////////

#include "odbcold.h"

/////////////////////////////////////////////////
// TODBCBase
bool TODBCBase::GetSQLState(const SQLSMALLINT& HandleType,
 const SQLHANDLE& Handle, TSQLState& SQLState) const {
  SQLSMALLINT L;
  if ((SQLReturn==SQL_SUCCESS_WITH_INFO)||(SQLReturn==SQL_ERROR)){
    SQLGetDiagField(HandleType, Handle, 1, SQL_DIAG_SQLSTATE,
     (SQLCHAR*)SQLState, sizeof(TSQLState), &L);
    return true;
  }
  return false;
}

TStr TODBCBase::GetErrMsg(const SQLSMALLINT& HandleType,
 const SQLHANDLE& Handle) const {
  static const TStr SQL_NoError="00000";
  TSQLState S;
  if (GetSQLState(HandleType, Handle, S)){return TStr(S);}
  else {return SQL_NoError;}
}

/////////////////////////////////////////////////
// TODBC

// ODBC attribute keywords
const TStr TODBC::oakDSN="DSN";
const TStr TODBC::oakUsername="UID";
const TStr TODBC::oakPassword="PWD";
const TStr TODBC::oakDatabase="DATABASE";

TODBC::TODBC(const int& ODBCVersion){
  SQLUINTEGER OV=SQL_OV_ODBC2;
  if (ODBCVersion==3){OV=SQL_OV_ODBC3;}
  SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &Handle);
  SQLSetEnvAttr(Handle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)OV, 0);
}

TODBC::~TODBC(){
  SQLFreeHandle(SQL_HANDLE_ENV, Handle);
}

// DSN Functions
bool TODBC::AddDataSource(const TStr& DSN, const TStr& DrvDesc,
 const TBool& User, const TStr& AttrStr,
 const TStr& Username, const TStr& Password, const TStr& Database) const {
  WORD Req=(User)?(ODBC_ADD_DSN):(ODBC_ADD_SYS_DSN);
  TMem A;
  A+=oakDSN; A+='='; A+=DSN; A+=TCh::NullCh;
  if (!Username.Empty()){A+=oakUsername; A+='='; A+=Username; A+=TCh::NullCh;}
  if (!Password.Empty()){A+=oakPassword; A+='='; A+=Password; A+=TCh::NullCh;}
  if (!Database.Empty()){A+=oakDatabase; A+='='; A+=Database; A+=TCh::NullCh;}
  if (!AttrStr.Empty()){A+=AttrStr; A+=TCh::NullCh;}
  A+=TCh::NullCh;
  return SQLConfigDataSource(NULL, Req, DrvDesc.CStr(), A())==TRUE;
}

bool TODBC::ModifyDataSource(const TStr& DSN, const TStr& DrvDesc,
 const TBool& User, const TStr& AttrStr,
 const TStr& Username, const TStr& Password, const TStr& Database) const {
  WORD Req=(User)?(ODBC_CONFIG_DSN):(ODBC_CONFIG_SYS_DSN);
  TMem A;
  A+=oakDSN; A+='='; A+=DSN; A+=TCh::NullCh;
  if (!Username.Empty()){A+=oakUsername; A+='='; A+=Username; A+=TCh::NullCh;}
  if (!Password.Empty()){A+=oakPassword; A+='='; A+=Password; A+=TCh::NullCh;}
  if (!Database.Empty()){A+=oakDatabase; A+='='; A+=Database; A+=TCh::NullCh;}
  if (!AttrStr.Empty()){A+=AttrStr; A+=TCh::NullCh;}
  A+=TCh::NullCh;
  return SQLConfigDataSource(NULL, Req, DrvDesc.CStr(), A())==TRUE;
}

bool TODBC::RemoveDataSource(const TStr& DSN, const TStr& DrvDesc,
 const TBool& User) const {
  WORD Req=(User)?(ODBC_REMOVE_DSN):(ODBC_REMOVE_SYS_DSN);
  TMem A;
  A+=oakDSN; A+='='; A+=DSN; A+=TCh::NullCh;
  A+=TCh::NullCh;
  return SQLConfigDataSource(NULL, Req, DrvDesc.CStr(), A())==TRUE;
}

// Catalog Functions
bool TODBC::GetDataSources(TStrV& DSNV, TStrV& DrvDescV){
  DSNV.Clr(); DrvDescV.Clr();

  const int MxNmLen=128; const int MxDrvDescLen=128;
  char SrvNm[MxNmLen], DrvDesc[MxDrvDescLen];
  SQLSMALLINT SrvNmLen, DrvDescSize;
  SQLUSMALLINT Direction=SQL_FETCH_FIRST;

  forever{
    SQLReturn=SQLDataSources(GetHandle(), Direction,
     (SQLCHAR*)SrvNm, sizeof(SrvNm), &SrvNmLen,
     (SQLCHAR*)DrvDesc, sizeof(DrvDesc),&DrvDescSize);
    if (SQLReturn==SQL_NO_DATA){return true;}
    else if (!SQLSuccess(SQLReturn)){return false;}

    SrvNm[MxNmLen-1]=0; DrvDesc[MxDrvDescLen-1]=0;
    DSNV.Add(TStr(SrvNm)); DrvDescV.Add(TStr(DrvDesc));
    Direction=SQL_FETCH_NEXT;
  }
}   // GetDataSources

bool TODBC::GetDrivers(TStrV& DrvDescV, TStrV& DrvAttrV){
  DrvDescV.Clr(); DrvAttrV.Clr();

  const int MxDrvDescLen=128; const int MxAttrLen=1024;

  char DrvDesc[MxDrvDescLen], DrvAttr[MxAttrLen];
  SQLSMALLINT DrvDescSize, DrvAttrSize;
  SQLUSMALLINT Direction=SQL_FETCH_FIRST;

  forever{
    SQLReturn=SQLDrivers(GetHandle(), Direction,
     (SQLCHAR*)DrvDesc, sizeof(DrvDesc), &DrvDescSize,
     (SQLCHAR*)DrvAttr, sizeof(DrvAttr), &DrvAttrSize);
    if (SQLReturn==SQL_NO_DATA){return true;}
    else if (!SQLSuccess(SQLReturn)){return false;}

    DrvDesc[MxDrvDescLen-1]=0; DrvAttr[MxAttrLen-1]=0;
    TStr Attr; char* AttrP=DrvAttr;
    while (DrvAttrSize>0){
      Attr+=AttrP;
      DrvAttrSize-=strlen(AttrP)+1;
      AttrP+=strlen(AttrP)+1;
      if (DrvAttrSize>0){Attr+="\n";}
    }
    DrvDescV.Add(TStr(DrvDesc)); DrvAttrV.Add(Attr);
    Direction=SQL_FETCH_NEXT;
  }
}   // GetDrivers

/////////////////////////////////////////////////
// TDB
TDB::TDB(const TODBC& ODBC, 
 const TStr& _DSN, const TStr& _Username, const TStr& _Password):
 DSN(_DSN), Username(_Username), Password(_Password){
  SQLUINTEGER LoginTimeout=30;   // default 30 seconds
  SQLAllocHandle(SQL_HANDLE_DBC, ODBC.GetHandle(), &Handle);
  SQLSetConnectAttr(Handle, SQL_ATTR_LOGIN_TIMEOUT, (SQLPOINTER)LoginTimeout,0);
  SQLReturn=::SQLConnect(Handle, (SQLCHAR*)DSN.CStr(), SQL_NTS, 
   (SQLCHAR*)Username.CStr(), SQL_NTS, (SQLCHAR*)Password.CStr(), SQL_NTS);
}

TDB::~TDB(){
  SQLDisconnect(Handle);
  SQLFreeHandle(SQL_HANDLE_DBC, Handle);
}

/////////////////////////////////////////////////
// TQuery
TQuery::TQuery(const TDB& DB): rsColNmH(64){
  SQLAllocHandle(SQL_HANDLE_STMT, DB.GetHandle(), &Handle);
  rsColData=0;
  rsNCols=0;
  QueryReady=true;
}

TQuery::~TQuery(){
  SQLFreeHandle(SQL_HANDLE_STMT, Handle);
  if (rsColData!=0){free(rsColData); rsColData=0;}
  QueryReady=false;
}

// Catalog Functions
bool TQuery::GetTables(){
  if (!QueryReady){return false;}
  SQLReturn=SQLTables(Handle, NULL, 0, NULL, 0, NULL, 0, NULL, 0);
  if (!SQLSuccess(SQLReturn)){return false;}
  rsBindCols();
  QueryReady=false;
  return true;
}

bool TQuery::GetColumns(const TStr& TableNm){
  if (!QueryReady){return false;}
  if (TableNm.Empty()){
    SQLReturn=SQLColumns(Handle, NULL, 0, NULL, 0, NULL, 0, NULL, 0);
  } else {
    SQLReturn=SQLColumns(Handle, NULL, 0, NULL, 0, 
     (SQLCHAR*)TableNm.CStr(), SQL_NTS, NULL, 0);
  }
  if (!SQLSuccess(SQLReturn)){return false;}
  rsBindCols();
  QueryReady=false;
  return true;
}

// SQL Statement
bool TQuery::Execute(const TStr& SQLQuery){
  if (!QueryReady){return false;}
  SQLReturn=SQLExecDirect(Handle, (SQLCHAR*)SQLQuery.CStr(), SQL_NTS);
  if (!SQLSuccess(SQLReturn)){return false;}
  rsBindCols();
  QueryReady=false;
  return true;
}

void TQuery::MakeReady(){
  SQLCloseCursor(Handle);
  SQLFreeStmt(Handle, SQL_UNBIND);
  QueryReady=true;
}

// Result Set
bool TQuery::Fetch(){
  if (QueryReady){return false;}
  SQLReturn=SQLFetch(Handle);
  if (SQLReturn==SQL_NO_DATA){MakeReady();}
  return SQLSuccess(SQLReturn);
}

SQLSMALLINT TQuery::rsCType(const SQLINTEGER& Type, const TStr& TyNm) const {
  switch(Type){
  case SQL_BIT:
		return SQL_C_BIT;
	case SQL_TINYINT:
		return SQL_C_UTINYINT;
	case SQL_SMALLINT:
		return SQL_C_SSHORT;
	case SQL_INTEGER:
		return SQL_C_SLONG;
	case SQL_REAL:
		return SQL_C_FLOAT;
	case SQL_FLOAT:
	case SQL_DOUBLE:
		return SQL_C_DOUBLE;
  case SQL_DATE:
	case SQL_TIME:
	case SQL_TIMESTAMP:
  case SQL_TYPE_DATE:
  case SQL_TYPE_TIME:
  case SQL_TYPE_TIMESTAMP:
    return SQL_C_TIMESTAMP;
	case SQL_NUMERIC:
	case SQL_DECIMAL:
	case SQL_BIGINT:
	case SQL_CHAR:
	case SQL_VARCHAR:
	case SQL_LONGVARCHAR:
		return SQL_C_CHAR;
	case SQL_BINARY:
	case SQL_VARBINARY:
	case SQL_LONGVARBINARY:
		return SQL_C_BINARY;
  case SQL_WCHAR:
  case SQL_WVARCHAR:
  case SQL_WLONGVARCHAR:
    // not supported: #ifdef UNICODE return SQL_C_WCHAR;
    return SQL_C_CHAR;
  default:
    /*
    if (TyNm=="INTEGER"){return SQL_C_SLONG;}
    else if (TyNm=="SMALLINT"){return SQL_C_SSHORT;}
    else if (TyNm=="CHAR"){return SQL_C_CHAR;}
    else if (TyNm=="VARCHAR"){return SQL_C_CHAR;}
    else if (TyNm=="BLOB SUB_TYPE 1"){return SQL_C_BINARY;}
    */
    WarnNotify("SQL Type Not Supported ["+TyNm+"]");
    return SQL_C_CHAR;
  }
}

void TQuery::rsBindCols(){
  SQLSMALLINT N;
  SQLNumResultCols(Handle, &N);
  rsNCols=N;
  if (rsNCols<=0){return;}   // No Result Set

  rsColNmV.Gen(rsNCols); 
  rsColOfsV.Gen(rsNCols); 
  rsColLenV.Gen(rsNCols); 
  rsColTypeV.Gen(rsNCols);

  int Ofs=0;
  for (int ColN=1; ColN<=rsNCols; ++ColN){
    SQLCHAR ColNm[128];
    SQLSMALLINT ColNmLen;
    SQLColAttribute(Handle, ColN, 
     SQL_DESC_NAME, ColNm, sizeof(ColNm), &ColNmLen, NULL);
    rsColNmV[ColN-1]=(char*)ColNm;
    rsColNmH.AddDat((char*)ColNm, ColN-1);

    SQLCHAR ColTypeNm[128];
    SQLSMALLINT ColTypeNmLen;
    SQLColAttribute(Handle, ColN, 
     SQL_DESC_TYPE_NAME, ColTypeNm, sizeof(ColTypeNm), &ColTypeNmLen, NULL);

    SQLINTEGER ColType;
    SQLRETURN R=SQLColAttribute(Handle, ColN, 
     SQL_DESC_CONCISE_TYPE, NULL, 0,NULL,(SQLPOINTER)&ColType);
    rsColTypeV[ColN-1]=rsCType(ColType, (char*)ColTypeNm);

    SQLINTEGER ColLen=256;   // version 2 does not support SQL_DESC_OCTET_LENGTH
    const SQLINTEGER MxTextLen=1024;
    SQLColAttribute(Handle, ColN, 
     SQL_DESC_OCTET_LENGTH, NULL, 0, NULL, (SQLPOINTER)&ColLen);
    if (ColType==SQL_LONGVARCHAR){if(ColLen>MxTextLen){ColLen=MxTextLen;}}
    if (rsColTypeV[ColN-1]==SQL_C_CHAR){ColLen++;}
    rsColLenV[ColN-1]=(int)ColLen;

    SQLINTEGER ColUpdatable;
    SQLColAttribute(Handle, ColN, 
     SQL_DESC_UPDATABLE, NULL, 0, NULL, (SQLPOINTER)&ColUpdatable);

    rsColOfsV[ColN-1]=Ofs;
    Ofs+=(int)ColLen+sizeof(SQLINTEGER);

    /*
    printf("%d %s %s (%d) %d %d\n", 
     ColN, ColNm, ColTypeNm, ColType, ColLen, ColUpdatable);
    */
  }

  if (rsColData!=0){free(rsColData); rsColData=0;}
  rsColData=(char*)malloc(Ofs);

  {for (int ColN=0; ColN<rsNCols; ++ColN){
    SQLBindCol(Handle, ColN+1, rsColTypeV[ColN],
     (SQLPOINTER)&rsColData[rsColOfsV[ColN]], rsColLenV[ColN],
     (SQLINTEGER*)&rsColData[rsColOfsV[ColN]+rsColLenV[ColN]]);
  }}
}

TODBCTy TQuery::GetFldTy(const int& FldN) const {
  SQLINTEGER LID=*(SQLINTEGER*)&rsColData[rsColOfsV[FldN]+rsColLenV[FldN]];
  if (LID==SQL_NULL_DATA){return otNull;}

  switch(rsColTypeV[FldN]){
  case SQL_C_BIT:{return otOther;}
  case SQL_C_UTINYINT:
  case SQL_C_SSHORT:
  case SQL_C_SLONG:{return otInt;}
  case SQL_C_FLOAT:
  case SQL_C_DOUBLE:{return otFlt;}
  case SQL_C_TIMESTAMP:{return otTm;}
  case SQL_C_CHAR:{return otStr;}
  case SQL_C_BINARY:{return otOther;}
  default:{return otUndef;}
  }
}

short TQuery::GetFldSQLTy(const int& FldN) const {return rsColTypeV[FldN];}

bool TQuery::GetValBool(const int& FldN) const {
  SQLINTEGER LID=*(SQLINTEGER*)&rsColData[rsColOfsV[FldN]+rsColLenV[FldN]];
  if (LID==SQL_NULL_DATA){return false;}

  switch(rsColTypeV[FldN]){
  case SQL_C_BIT: WarnNotify("SQL_C_BIT -> Bool Not Supported"); return false;
  case SQL_C_UTINYINT:{
    uchar I=*(uchar*)&rsColData[rsColOfsV[FldN]];
    return I!=0;}
  case SQL_C_SSHORT:{
    short I=*(short*)&rsColData[rsColOfsV[FldN]];
    return I!=0;}
  case SQL_C_SLONG:{
    long I=*(long*)&rsColData[rsColOfsV[FldN]];
    return I!=0;}
  case SQL_C_FLOAT: WarnNotify("SQL_C_FLOAT -> Bool Not Supported"); return false;
  case SQL_C_DOUBLE: WarnNotify("SQL_C_DOUBLE -> Bool Not Supported"); return false;
  case SQL_C_TIMESTAMP: WarnNotify("SQL_C_TIMESTAMP -> Bool Not Supported"); 
    return false;
  case SQL_C_CHAR: WarnNotify("SQL_C_CHAR -> Bool Not Supported"); return false;
  case SQL_C_BINARY: WarnNotify("SQL_C_BINARY -> Bool Not Supported"); return false;
  default: WarnNotify("Internal - Unknown SQL_C Type"); return false;
  }
}

int TQuery::GetValInt(const int& FldN) const {
  SQLINTEGER LID=*(SQLINTEGER*)&rsColData[rsColOfsV[FldN]+rsColLenV[FldN]];
  if (LID==SQL_NULL_DATA){return -1;}

  switch(rsColTypeV[FldN]){
  case SQL_C_BIT: WarnNotify("SQL_C_BIT -> Int Not Supported"); return -1;
  case SQL_C_UTINYINT:{
    uchar I=*(uchar*)&rsColData[rsColOfsV[FldN]];
    return I;}
  case SQL_C_SSHORT:{
    short I=*(short*)&rsColData[rsColOfsV[FldN]];
    return I;}
  case SQL_C_SLONG:{
    long I=*(long*)&rsColData[rsColOfsV[FldN]];
    return I;}
  case SQL_C_FLOAT:{
    float D=*(float*)&rsColData[rsColOfsV[FldN]];
    return D;}
  case SQL_C_DOUBLE:{
    double D=*(double*)&rsColData[rsColOfsV[FldN]];
    return D;}
  case SQL_C_TIMESTAMP: WarnNotify("SQL_C_TIMESTAMP -> Int Not Supported"); return -1;
  case SQL_C_CHAR: return atoi(&rsColData[rsColOfsV[FldN]]);
  case SQL_C_BINARY: WarnNotify("SQL_C_BINARY -> Int Not Supported"); return -1;
  default: WarnNotify("Internal - Unknown SQL_C Type"); return -1;
  }
}

double TQuery::GetValFlt(const int& FldN) const {
  SQLINTEGER LID=*(SQLINTEGER*)&rsColData[rsColOfsV[FldN]+rsColLenV[FldN]];
  if (LID==SQL_NULL_DATA){return -1;}

  switch(rsColTypeV[FldN]){
  case SQL_C_BIT: WarnNotify("SQL_C_BIT -> Flt Not Supported"); return -1;
  case SQL_C_UTINYINT:{
    uchar I=*(uchar*)&rsColData[rsColOfsV[FldN]];
    return I;}
  case SQL_C_SSHORT:{
    short I=*(short*)&rsColData[rsColOfsV[FldN]];
    return I;}
  case SQL_C_SLONG:{
    long I=*(long*)&rsColData[rsColOfsV[FldN]];
    return I;}
  case SQL_C_FLOAT:{
    float D=*(float*)&rsColData[rsColOfsV[FldN]];
    return D;}
  case SQL_C_DOUBLE:{
    double D=*(double*)&rsColData[rsColOfsV[FldN]];
    return D;}
  case SQL_C_TIMESTAMP: WarnNotify("SQL_C_TIMESTAMP -> Flt Not Supported"); return -1;
  case SQL_C_CHAR: return atof(&rsColData[rsColOfsV[FldN]]);
  case SQL_C_BINARY: WarnNotify("SQL_C_BINARY -> Flt Not Supported"); return -1;
  default: WarnNotify("Internal - Unknown SQL_C Type"); return -1;
  }
}

TTm TQuery::GetValTm(const int& FldN) const {
  SQLINTEGER LID=*(SQLINTEGER*)&rsColData[rsColOfsV[FldN]+rsColLenV[FldN]];
  if (LID==SQL_NULL_DATA){return TTm();}

  switch(rsColTypeV[FldN]){
  case SQL_C_BIT: 
    WarnNotify("SQL_C_BIT -> Tm Not Supported"); return TTm();
  case SQL_C_UTINYINT:
    WarnNotify("SQL_C_UTINYINT -> Tm Not Supported"); return TTm();
  case SQL_C_SSHORT:
    WarnNotify("SQL_C_SSHORT -> Tm Not Supported"); return TTm();
  case SQL_C_SLONG:
    WarnNotify("SQL_C_SLONG -> Tm Not Supported"); return TTm();
  case SQL_C_FLOAT:
    WarnNotify("SQL_C_FLOAT -> Tm Not Supported"); return TTm();
  case SQL_C_DOUBLE: 
    WarnNotify("SQL_C_DOUBLE -> Tm Not Supported"); return TTm();
  case SQL_C_TIMESTAMP: 
    {TIMESTAMP_STRUCT Time=*(TIMESTAMP_STRUCT*)&rsColData[rsColOfsV[FldN]];
    return TTm(Time.year, Time.month, Time.day, 1, Time.hour, Time.minute,
     Time.second, Time.fraction);}
  case SQL_C_CHAR: 
    WarnNotify("SQL_C_CHAR -> Tm Not Supported"); return TTm();
  case SQL_C_BINARY: 
    WarnNotify("SQL_C_BINARY -> Tm Not Supported"); return TTm();
  default: 
    WarnNotify("Internal - Unknown SQL_C Type"); return TTm();
  }
}

TStr TQuery::GetValStr(const int& FldN) const {
  SQLINTEGER LID=*(SQLINTEGER*)&rsColData[rsColOfsV[FldN]+rsColLenV[FldN]];
  if (LID==SQL_NULL_DATA){return "NULL";}

  char S[32];
  switch(rsColTypeV[FldN]){
  case SQL_C_BIT: WarnNotify("SQL_C_BIT -> Str Not Supported"); return "";
  case SQL_C_UTINYINT:{
    uchar I=*(uchar*)&rsColData[rsColOfsV[FldN]];
    sprintf(S, "%hu", I);
    return TStr(S);}
  case SQL_C_SSHORT:{
    short I=*(short*)&rsColData[rsColOfsV[FldN]];
    sprintf(S, "%d", I);
    return TStr(S);}
  case SQL_C_SLONG:{
    long I=*(long*)&rsColData[rsColOfsV[FldN]];
    sprintf(S, "%ld", I);
    return TStr(S);}
  case SQL_C_FLOAT:{
    float D=*(float*)&rsColData[rsColOfsV[FldN]];
    sprintf(S, "%g", D);
    return TStr(S);}
  case SQL_C_DOUBLE:{
    double D=*(double*)&rsColData[rsColOfsV[FldN]];
    sprintf(S, "%g", D);
    return TStr(S);}
  case SQL_C_TIMESTAMP:{
    TIMESTAMP_STRUCT Time=*(TIMESTAMP_STRUCT*)&rsColData[rsColOfsV[FldN]];
    TTm Tm=TTm(Time.year, Time.month, Time.day, -1, Time.hour, Time.minute,
     Time.second, Time.fraction);
    return TInt::GetStr(Tm.GetYear())+"-"+TInt::GetStr(Tm.GetMonth(), "%02d")
     +"-"+TInt::GetStr(Tm.GetDay(), "%02d")+" "
     +TInt::GetStr(Tm.GetHour(), "%02d")+":"+TInt::GetStr(Tm.GetMin(), "%02d")
     +":"+TInt::GetStr(Tm.GetSec(), "%02d");}
  case SQL_C_CHAR: return &rsColData[rsColOfsV[FldN]];
  case SQL_C_BINARY: WarnNotify("SQL_C_BINARY -> Str  Not Supported"); return "";
  default: WarnNotify("Internal - Unknown SQL_C Type"); return "";
  }
}

