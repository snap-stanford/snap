#include "stdafx.h"

#include "dnet.h"

/////////////////////////////////////////////////
// General
TStr TDNet::GetStr(System::String* DNStr){
  const int DNStrLen=DNStr->Length;
  TChA ChA(DNStrLen);
  for (int ChN=0; ChN<DNStrLen; ChN++){
    char Ch=char(DNStr->get_Chars(ChN));
    ChA+=Ch;
  }
  return ChA;
}

/////////////////////////////////////////////////
// Sql-Exception
TStr TSqlExcept::GetMsgStr(SqlException* Except){
  TChA MsgChA;
  for (int ErrN=0; ErrN<Except->Errors->Count; ErrN++){
    if (!MsgChA.Empty()){MsgChA+="; ";}
    MsgChA+='[';
    MsgChA+=TInt::GetStr(Except->Errors->Item[ErrN]->get_Number());
    MsgChA+=" / ";
    MsgChA+=TDNet::GetStr(Except->Errors->Item[ErrN]->Message);
    MsgChA+=" / ";
    MsgChA+=TDNet::GetStr(Except->Errors->Item[ErrN]->Source);
    MsgChA+=" / ";
    MsgChA+=TDNet::GetStr(Except->Errors->Item[ErrN]->Server);
    MsgChA+=']';
  }
  return MsgChA;
}

/////////////////////////////////////////////////
// Sql-Connection
TSqlConnState TSqlConn::GetState() const {
  switch (Conn->State){
    case 16: return scsBroken;
    case 0: return scsClosed;
    case 2: return scsConnecting;
    case 4: return scsExecuting;
    case 8: return scsFetching;
    case 1: return scsOpen;
    default: return scsUndef;
  }
}

PSqlStat TSqlConn::Open(){
  PSqlStat SqlStat=TSqlStat::GetOkStat();
  try {
    Conn->Open();
  }
  catch (SqlException* Except){
    SqlStat=TSqlStat::New(false, TSqlExcept::GetMsgStr(Except));}
  catch (...){
    SqlStat=TSqlStat::New(false, "[Unknown Database Error]");}
  return SqlStat;
}

TStr TSqlConn::GetConnStr(
 const TStr& SrvNm, const TStr& DbNm, const TStr& UsrNm, const TStr& PwdStr){
  return TStr()+
   "Server="+SrvNm+";"+"Database="+DbNm+";"+
   "User Id="+UsrNm+";"+"Password="+PwdStr;
}

/////////////////////////////////////////////////
// Sql-Reader
TStr TSqlRd::GetFldVal(const int& FldN) const {
  EAssert(Rd!=NULL);
  if (GetFldTypeNm(FldN)=="datetime"){
    return GetFldTm(FldN).GetWebLogDateTimeStr();
  } else {
    return TDNet::GetStr(Rd->Item[FldN]->ToString());
  }
}

TStr TSqlRd::GetFldVal(const TStr& FldNm) const {
  EAssert(Rd!=NULL); 
  if (GetFldTypeNm(GetFldN(FldNm))=="datetime"){
    return GetFldTm(FldNm).GetWebLogDateTimeStr();
  } else {
    return TDNet::GetStr(Rd->Item[FldNm.CStr()]->ToString());
  }
}

TTm TSqlRd::GetFldTm(const int& FldN) const {
  EAssert(Rd!=NULL);
  TTm Tm;
  try {
    System::DateTime DtTm=Rd->GetDateTime(FldN);
    Tm=TTm(
     DtTm.Year, DtTm.Month, DtTm.Day, DtTm.DayOfWeek, 
     DtTm.Hour, DtTm.Minute, DtTm.Second, DtTm.Millisecond);
  }
  catch (...){
  }
  return Tm;
}

/////////////////////////////////////////////////
// Sql-Command
PSqlCm TSqlCm::New(
 const PSqlConn& SqlConn, const TStr& CmStr, const TStrPrV& ParamNmValPrV){
  PSqlCm SqlCm=new TSqlCm();
  SqlCm->GetCm()->Connection=SqlConn->GetConn();
  SqlCm->GetCm()->CommandText=CmStr.CStr();
  for (int ParamN=0; ParamN<ParamNmValPrV.Len(); ParamN++){
    System::String* ParamNmDNetStr=ParamNmValPrV[ParamN].Val1.CStr();
    System::String* ParamValDNetStr=ParamNmValPrV[ParamN].Val2.CStr();
    SqlCm->GetCm()->Parameters->Add(ParamNmDNetStr, 
     System::Data::SqlDbType::VarChar, ParamValDNetStr->Length)->Value=ParamValDNetStr;
  }
  return SqlCm;
}

PSqlStat TSqlCm::ExeNonQuery(){
  PSqlStat SqlStat=TSqlStat::GetOkStat();
  try {
    Cm->ExecuteNonQuery();
  }
  catch (SqlException* Except){
    SqlStat=TSqlStat::New(false, TSqlExcept::GetMsgStr(Except));}
  catch (...){
    SqlStat=TSqlStat::New(false, "[Unknown Database Error]");}
  return SqlStat;
}

PSqlRd TSqlCm::ExeQuery(){
  PSqlStat SqlStat=TSqlStat::GetOkStat();
  SqlDataReader* Rd=NULL;
  try {
    Rd=Cm->ExecuteReader();
  }
  catch (SqlException* Except){
    SqlStat=TSqlStat::New(false, TSqlExcept::GetMsgStr(Except));}
  catch (...){
    SqlStat=TSqlStat::New(false, "[Unknown Database Error]");}
  return TSqlRd::New(SqlStat, Rd);
}

/////////////////////////////////////////////////
// Sql-Query
PSqlStat TSqlQuery::ExeNonQuery(const TStr& CmStr, const TStrPrV& ParamNmValPrV){
  // close previous sql-reader
  CloseSqlRd();
  // prepare sql-connection
  if (SqlConn->GetState()!=scsOpen){
    SqlConn->Close();
    SqlStat=SqlConn->Open();
    if (!SqlStat->IsOk()){return SqlStat;}
  }
  // prepare sql-command
  SqlCm=TSqlCm::New(SqlConn, CmStr, ParamNmValPrV);
  // execute non-query command
  SqlStat=SqlCm->ExeNonQuery();
  // return status
  return SqlStat;
}

PSqlStat TSqlQuery::ExeQuery(const TStr& CmStr, const TStrPrV& ParamNmValPrV){
  // close previous sql-reader
  CloseSqlRd();
  // prepare sql-connection
  if (SqlConn->GetState()!=scsOpen){
    SqlConn->Close();
    SqlStat=SqlConn->Open();
    if (!SqlStat->IsOk()){return SqlStat;}
  }
  // prepare sql-command
  SqlCm=TSqlCm::New(SqlConn, CmStr, ParamNmValPrV);
  // execute non-query command
  SqlRd=SqlCm->ExeQuery();
  SqlStat=SqlRd->GetSqlStat();
  // return status
  return SqlStat;
}

PSqlStat TSqlQuery::ExeQuery(
 const TStr& CmStr, const TStr& ParamNm, const TStr& ParamVal){
  TStrPrV ParamNmValPrV(1, 0);
  ParamNmValPrV.Add(TStrPr(ParamNm, ParamVal));
  return ExeQuery(CmStr, ParamNmValPrV);
}

PSqlStat TSqlQuery::ExeQuery(const TStr& CmStr,
 const TStr& ParamNm1, const TStr& ParamVal1,
 const TStr& ParamNm2, const TStr& ParamVal2){
  TStrPrV ParamNmValPrV(2, 0);
  ParamNmValPrV.Add(TStrPr(ParamNm1, ParamVal1));
  ParamNmValPrV.Add(TStrPr(ParamNm2, ParamVal2));
  return ExeQuery(CmStr, ParamNmValPrV);
}

void TSqlQuery::Dump(const PSqlQuery& SqlQuery){
  if (SqlQuery->IsOk()){
    int Flds=SqlQuery->GetFlds();
    for (int FldN=0; FldN<Flds; FldN++){
      TStr FldNm=SqlQuery->GetFldNm(FldN);
      TStr FldTypeNm=SqlQuery->GetFldTypeNm(FldN);
      printf("[%s:%s] ", FldNm.CStr(), FldTypeNm.CStr());
    }
    printf("\n");
    while (SqlQuery->NextRow()){
      for (int FldN=0; FldN<Flds; FldN++){
        TStr FldNm=SqlQuery->GetFldNm(FldN);
        TStr FldVal=SqlQuery->GetFldVal(FldN);
        printf("[%s=%s] ", FldNm.CStr(), FldVal.CStr());
      }
      printf("\n");
    }
  } else {
    printf("%s", SqlQuery->GetMsgStr().CStr());
  }
}