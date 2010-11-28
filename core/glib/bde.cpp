/////////////////////////////////////////////////
// Includes
#include "bde.h"

/////////////////////////////////////////////////
// BDE-DataBase
int TBdeDb::LastDbId=0;

void TBdeDb::ConnDb(){
  try {
    Db->Connected=true; Ok=true;}
  catch (EDatabaseError& DbErr){
    ErrMsg=DbErr.Message.c_str(); Ok=false;}
  catch (...){
    ErrMsg="Unknown Error"; Ok=false;
  }
}

TBdeDb::TBdeDb():
  Db(new TDatabase(NULL)), Ok(false), ErrMsg(){
  Db->Connected=false;
  Db->KeepConnection=true;
  Db->LoginPrompt=false;
}

TBdeDb::~TBdeDb(){
  Db->Connected=false; delete Db;
}

/////////////////////////////////////////////////
// DBase-Data-Base
TDbfDb::TDbfDb(const TStr& FPath){
  Db->DatabaseName=(TStr("DbfDbId")+TInt::GetStr(GetDbId())).CStr();
  Db->DriverName="STANDARD";
  Db->Params->Clear();
  Db->Params->Add((TStr("PATH=")+FPath).CStr());
  Db->Params->Add("DEFAULT DRIVER=DBASE");
  Db->Params->Add("ENABLE BCD=FALSE");
  ConnDb();
}

/////////////////////////////////////////////////
// Paradox-Data-Base
TPxDb::TPxDb(const TStr& FPath){
  Db->DatabaseName=(TStr("PxDbId")+TInt::GetStr(GetDbId())).CStr();
  Db->DriverName="STANDARD";
  Db->Params->Clear();
  Db->Params->Add((TStr("PATH=")+FPath).CStr());
  Db->Params->Add("DEFAULT DRIVER=PARADOX");
  Db->Params->Add("ENABLE BCD=FALSE");
  ConnDb();
}

/////////////////////////////////////////////////
// Access-Data-Base
TAccDb::TAccDb(const TStr& OdbcDsnNm):
  TBdeDb(){
  Db->DatabaseName=(TStr("AccDbId")+TInt::GetStr(GetDbId())).CStr();
  Db->DriverName="Driver do Microsoft Access (*.m";
  Db->Params->Clear();
  Db->Params->Add("DATABASE NAME=");
  Db->Params->Add("USER NAME=");
  Db->Params->Add((TStr("ODBC DSN=")+OdbcDsnNm).CStr());
  Db->Params->Add("OPEN MODE=READ/WRITE");
  Db->Params->Add("SCHEMA CACHE SIZE=8");
  Db->Params->Add("SQLQRYMODE=");
  Db->Params->Add("LANGDRIVER=");
  Db->Params->Add("SQLPASSTHRU MODE=SHARED AUTOCOMMIT");
  Db->Params->Add("SCHEMA CACHE TIME=-1");
  Db->Params->Add("MAX ROWS=-1");
  Db->Params->Add("BATCH COUNT=200");
  Db->Params->Add("ENABLE SCHEMA CACHE=FALSE");
  Db->Params->Add("SCHEMA CACHE DIR=");
  Db->Params->Add("ENABLE BCD=FALSE");
  Db->Params->Add("ROWSET SIZE=20");
  Db->Params->Add("BLOBS TO CACHE=64");
  Db->Params->Add("PASSWORD=");
  ConnDb();
}

/////////////////////////////////////////////////
// InterBase-DataBase
TIbDb::TIbDb(const TStr& FNm, const TStr& UsrNm, const TStr& PwdStr):
  TBdeDb(){
  Db->DatabaseName=(TStr("IbDbId")+TInt::GetStr(GetDbId())).CStr();
  Db->DriverName="INTRBASE";
  Db->Params->Clear();
  Db->Params->Add((TStr("SERVER NAME=")+FNm).CStr());
  Db->Params->Add((TStr("USER NAME=")+UsrNm).CStr());
  Db->Params->Add("OPEN MODE=READ/WRITE");
  Db->Params->Add("SCHEMA CACHE SIZE=8");
  Db->Params->Add("LANGDRIVER=");
  Db->Params->Add("SQLQRYMODE=");
  Db->Params->Add("SQLPASSTHRU MODE=SHARED AUTOCOMMIT");
  Db->Params->Add("SCHEMA CACHE TIME=-1");
  Db->Params->Add("MAX ROWS=-1");
  Db->Params->Add("BATCH COUNT=200");
  Db->Params->Add("ENABLE SCHEMA CACHE=FALSE");
  Db->Params->Add("SCHEMA CACHE DIR=");
  Db->Params->Add("ENABLE BCD=FALSE");
  Db->Params->Add("BLOBS TO CACHE=64");
  Db->Params->Add("BLOB SIZE=32");
  Db->Params->Add((TStr("PASSWORD=")+PwdStr).CStr());
  ConnDb();
}

/////////////////////////////////////////////////
// SqlServer-DataBase
TSqlSrvDb::TSqlSrvDb(const TStr& DsnNm, const TStr& UsrNm, const TStr& PwdStr):
  TBdeDb(){
  Db->DatabaseName=(TStr("SqlSrvDbId")+TInt::GetStr(GetDbId())).CStr();
  Db->DriverName="SQL Server";
  Db->Params->Clear();
  Db->Params->Add("DATABASE NAME=");
  Db->Params->Add((TStr("USER NAME=")+UsrNm).CStr());
  Db->Params->Add((TStr("ODBC DSN=")+DsnNm).CStr());
  Db->Params->Add("OPEN MODE=READ/WRITE");
  Db->Params->Add("BATCH COUNT=200");
  Db->Params->Add("LANGDRIVER=");
  Db->Params->Add("MAX ROWS=-1");
  Db->Params->Add("SCHEMA CACHE DIR=");
  Db->Params->Add("SCHEMA CACHE SIZE=8");
  Db->Params->Add("SCHEMA CACHE TIME=-1");
  Db->Params->Add("SQLPASSTHRU MODE=SHARED AUTOCOMMIT");
  Db->Params->Add("SQLQRYMODE=");
  Db->Params->Add("ENABLE SCHEMA CACHE=FALSE");
  Db->Params->Add("ENABLE BCD=FALSE");
  Db->Params->Add("ROWSET SIZE=20");
  Db->Params->Add("BLOBS TO CACHE=64");
  Db->Params->Add("BLOB SIZE=32");
  Db->Params->Add((TStr("PASSWORD=")+PwdStr).CStr());
  ConnDb();
}

TSqlSrvDb::TSqlSrvDb(const TStr& AliasNm):
  TBdeDb(){
  Db->DatabaseName=(TStr("SqlSrvDbId")+TInt::GetStr(GetDbId())).CStr();
  Db->AliasName=AliasNm.CStr();
  ConnDb();
}

/////////////////////////////////////////////////
// Oracle-DataBase
TOracleDb::TOracleDb(
 const TStr& OdbcDsnNm, const TStr& UsrNm, const TStr& PwdStr):
  TBdeDb(){
  Db->DatabaseName=(TStr("OracleDbId")+TInt::GetStr(GetDbId())).CStr();
  Db->DriverName="Microsoft ODBC for Oracle";
  Db->Params->Clear();
  Db->Params->Add((TStr("ODBC DSN=")+OdbcDsnNm).CStr());
  Db->Params->Add((TStr("USER NAME=")+UsrNm).CStr());
  Db->Params->Add((TStr("PASSWORD=")+PwdStr).CStr());
  ConnDb();
}

/////////////////////////////////////////////////
// Bde-Query
TBdeQuery::TBdeQuery(const PBdeDb& BdeDb):
  Query(new TQuery(NULL)), Ok(false), ErrMsg(){
  Query->Active=false;
  Query->DatabaseName=BdeDb->GetNm().CStr();
}

TBdeQuery::~TBdeQuery(){
  Query->Active=false;
  delete Query;
}

void TBdeQuery::Execute(const TStr& QueryStr){
  Query->Active=false;
  Query->SQL->Clear();
  Query->SQL->Add(QueryStr.CStr());
  try {
    Query->ExecSQL(); Ok=true;}
  catch (EDatabaseError& DbErr){
    ErrMsg=DbErr.Message.c_str(); Ok=false;}
  catch (...){
    ErrMsg="Unknown Query Error"; Ok=false;
  }
}

void TBdeQuery::Select(const TStr& QueryStr){
  Query->Active=false;
  Query->SQL->Clear();
  Query->SQL->Add(QueryStr.CStr());
  try {
    Query->Open(); Ok=true;}
  catch (EDatabaseError& DbErr){
    ErrMsg=DbErr.Message.c_str(); Ok=false;}
  catch (...){
    ErrMsg="Unknown Query Error"; Ok=false;
  }
}

/////////////////////////////////////////////////
// Bde-Table
TBdeTable::TBdeTable(const PBdeDb& BdeDb, const TStr& TableNm):
  Table(new TTable(NULL)){
  Table->Active=false;
  Table->DatabaseName=BdeDb->GetNm().CStr();
  Table->TableName=TableNm.CStr();
  Table->Active=true;
}

TBdeTable::~TBdeTable(){
  Table->Active=false;
  delete Table;
}

