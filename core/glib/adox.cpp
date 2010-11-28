/////////////////////////////////////////////////
// Includes
#include "adox.h"

/////////////////////////////////////////////////
// Ado-DataBase
int TAdoDb::LastDbId=0;

void TAdoDb::ConnDb(const TStr& ConnStr){
  try {
    Connection->ConnectionString=ConnStr.CStr();
    Connection->Connected=true; Ok=true;}
  catch (EDatabaseError& DbErr){
    ErrMsg=DbErr.Message.c_str(); Ok=false;}
  catch (...){
    ErrMsg="Unknown Error"; Ok=false;
  }
}

TAdoDb::TAdoDb():
  Connection(NULL), Ok(false), ErrMsg(){
  Connection=new TADOConnection(NULL);
  Connection->Connected=false;
  Connection->LoginPrompt=false;
}

TAdoDb::~TAdoDb(){
  Connection->Connected=false;
  delete Connection;
}

/////////////////////////////////////////////////
// SqlServer-DataBase
PAdoDb TSqlSrvAdoDb::New(
 const TStr& AliasNm, const TStr& UsrNm, const TStr& PwdStr){
  PAdoDb Db=PAdoDb(new TSqlSrvAdoDb());
  TStr ConnStr=TStr()+
   "Data Source="+AliasNm+";"
   "User ID="+UsrNm+";"
   "Password="+PwdStr+";";
  Db->ConnDb(ConnStr);
  return Db;
}

/////////////////////////////////////////////////
// Ado-Query
TAdoQuery::TAdoQuery(const PAdoDb& AdoDb):
  Query(new TADOQuery(NULL)), Ok(false), ErrMsg(), RecN(0){
  Query->Active=false;
  Query->Connection=AdoDb->GetConnection();
}

TAdoQuery::~TAdoQuery(){
  Query->Active=false;
  delete Query;
}

void TAdoQuery::Execute(const TStr& QueryStr){
  Query->Active=false;
  Query->SQL->Clear();
  Query->SQL->Add(QueryStr.CStr());
  try {
    Query->ExecSQL(); Ok=true;}
  catch (EADOError& ADOError){
    ErrMsg=ADOError.Message.c_str(); Ok=false;}
  catch (...){
    ErrMsg="Unknown Query Error"; Ok=false;
  }
}

void TAdoQuery::Select(const TStr& QueryStr){
  Query->Active=false;
  Query->SQL->Clear();
  Query->SQL->Add(QueryStr.CStr());
  try {
    Query->Open(); Ok=true;}
  catch (EADOError& ADOError){
    ErrMsg=ADOError.Message.c_str(); Ok=false;}
  catch (...){
    ErrMsg="Unknown Query Error"; Ok=false;
  }
}

/////////////////////////////////////////////////
// Ado-Table
TAdoTable::TAdoTable(const PAdoDb& AdoDb, const TStr& TableNm):
  Table(new TADOTable(NULL)), RecN(0){
  Table->Active=false;
  Table->Connection=AdoDb->GetConnection();
  Table->TableName=TableNm.CStr();
  Table->Active=true;
}

TAdoTable::~TAdoTable(){
  Table->Active=false;
  delete Table;
}

