/////////////////////////////////////////////////
// Includes
#include "webmb.h"
#include "webold.h"

/////////////////////////////////////////////////
// Web-Memory-Base
const TStr TWebMemBs::WebMemBsFNm="WebMemBs.Txt";
const int TWebMemBs::ExpectedWebPgs=10000;

void TWebMemBs::OnSave(const int& WebPgId, const PXWebPg& WebPg){
  for (int UrlStrN=0; UrlStrN<WebPg->GetUrls(); UrlStrN++){
    TStr UrlStr=WebPg->GetUrlStr(UrlStrN);
    if (IsWebPg(UrlStr)){
      int PrevWebPgId=UrlStrToWebPgIdH.GetDat(UrlStr);
      TStrV PrevUrlStrV=WebPgIdToUrlStrVH.GetDat(PrevWebPgId);
      for (int UrlStrN=0; UrlStrN<PrevUrlStrV.Len(); UrlStrN++){
        UrlStrToWebPgIdH.DelKey(PrevUrlStrV[UrlStrN]);}
      WebPgIdToUrlStrVH.DelKey(PrevWebPgId);
      if (WebPgIdToRefUrlStrVH.IsKey(PrevWebPgId)){
        WebPgIdToRefUrlStrVH.DelKey(PrevWebPgId);}
      TFile::Del(GetWebPgFNm(PrevWebPgId));
    }
  }
  {for (int UrlStrN=0; UrlStrN<WebPg->GetUrls(); UrlStrN++){
    UrlStrToWebPgIdH.AddDat(WebPg->GetUrlStr(UrlStrN), WebPgId);}}
  WebPgIdToUrlStrVH.AddDat(WebPgId, WebPg->GetUrlStrV());
  if (IsGenRef()){
    WebPgIdToRefUrlStrVH.AddDat(WebPgId, WebPg->GetRefUrlStrV());}

  PSOut SOut=new TFOut(GetWebPgFNm(WebPgId));
  WebPg->SaveTxt(SOut);
  Modified=true;
  if (GetWebPgs()%1000==0){SaveTxt();}
}

void TWebMemBs::OnFetch(const int& WebPgId, const PXWebPg& WebPg){
  bool DoSave;
  OnWebPg(WebPgId, WebPg, DoSave);
  if (DoSave){OnSave(WebPgId, WebPg);}
  if (IsCheckMode()){
    for (int UrlStrN=0; UrlStrN<WebPg->GetUrls(); UrlStrN++){
      TStr UrlStr=WebPg->GetUrlStr(UrlStrN);
      CheckedUrlStrToWebPgIdH.AddDat(UrlStr, WebPgId);
    }
  }
}

TWebMemBs::TWebMemBs(const TStr& _FPath, const bool& _CheckMode):
  TXWebBs(_FPath, _CheckMode),
  UrlStrToWebPgIdH(ExpectedWebPgs),
  CheckedUrlStrToWebPgIdH(IsCheckMode()?ExpectedWebPgs:0),
  WebPgIdToUrlStrVH(ExpectedWebPgs),
  WebPgIdToRefUrlStrVH(ExpectedWebPgs),
  Modified(false){
  PSIn SIn=new TFIn(GetFPath()+WebMemBsFNm);
  TILx Lx(SIn, TFSet()|iloRetEoln|iloSigNum);
  while (Lx.GetSym(syInt, syEof)==syInt){
    int WebPgId=Lx.Int;
    Lx.GetSym(syColon);
    while (Lx.GetSym(syQStr, sySemicolon)==syQStr){
      UrlStrToWebPgIdH.AddDat(Lx.Str, WebPgId);
      WebPgIdToUrlStrVH.AddDat(WebPgId).Add(Lx.Str);
    }
    WebPgIdToRefUrlStrVH.AddKey(WebPgId);
    while (Lx.GetSym(syQStr, syEoln)==syQStr){
      WebPgIdToRefUrlStrVH.GetDat(WebPgId).AddMerged(Lx.Str);
    }
  }
}

void TWebMemBs::SaveTxt(){
  TXWebBs::SaveTxt();
  PSOut SOut=new TFOut(GetFPath()+WebMemBsFNm);
  TOLx Lx(SOut, TFSet()|oloFrcEoln|oloSigNum);
  int WebPgIdToUrlStrVP=WebPgIdToUrlStrVH.FFirstKeyId();
  while (WebPgIdToUrlStrVH.FNextKeyId(WebPgIdToUrlStrVP)){
    int WebPgId=WebPgIdToUrlStrVH.GetKey(WebPgIdToUrlStrVP);
    TStrV UrlStrV=WebPgIdToUrlStrVH.GetDat(WebPgId);
    TStrV RefUrlStrV;
    if (WebPgIdToRefUrlStrVH.IsKey(WebPgId)){
      RefUrlStrV=WebPgIdToRefUrlStrVH.GetDat(WebPgId);}
    Lx.PutInt(WebPgId); Lx.PutSym(syColon);
    for (int UrlStrN=0; UrlStrN<UrlStrV.Len(); UrlStrN++){
      Lx.PutQStr(UrlStrV[UrlStrN]);}
    Lx.PutSym(sySemicolon);
    for (int RefUrlStrN=0; RefUrlStrN<RefUrlStrV.Len(); RefUrlStrN++){
      Lx.PutQStr(RefUrlStrV[RefUrlStrN]);}
    Lx.PutLn();
  }
}

int TWebMemBs::AddWebPg(const TStr& RelUrlStr, const TStr& BaseUrlStr){
  PUrl Url=new TUrl(RelUrlStr, BaseUrlStr);
  if (Url->IsOk(usHttp)){
    if (IsWebPg(Url->GetUrlStr(), IsCheckMode())){
      AddWebPgRef(Url->GetUrlStr(), BaseUrlStr); return -1;
    } else {
      return FetchWebPg(RelUrlStr, BaseUrlStr);
    }
  } else {
    TStr ErrStr=TStr("Invalid URL [")+RelUrlStr+"] + ["+BaseUrlStr+"]";
    OnError(-1, ErrStr);
    return -1;
  }
}

bool TWebMemBs::IsWebPg(const TStr& UrlStr){
  return UrlStrToWebPgIdH.IsKey(UrlStr);
}

bool TWebMemBs::IsWebPg(const TStr& UrlStr, const bool& Check){
  if (Check){return CheckedUrlStrToWebPgIdH.IsKey(UrlStr);}
  else {return IsWebPg(UrlStr);}
}

PXWebPg TWebMemBs::GetWebPg(const int& WebPgId){
  TStrV UrlStrV=WebPgIdToUrlStrVH.GetDat(WebPgId);
  TStrV RefUrlStrV;
  if (WebPgIdToRefUrlStrVH.IsKey(WebPgId)){
    RefUrlStrV=WebPgIdToRefUrlStrVH.GetDat(WebPgId);}
  PSIn SIn=new TFIn(GetWebPgFNm(WebPgId));
  return TXWebPg::LoadTxt(SIn, UrlStrV, RefUrlStrV);
}

PXWebPg TWebMemBs::GetWebPg(const TStr& UrlStr){
  int WebPgId=UrlStrToWebPgIdH.GetDat(UrlStr);
  return GetWebPg(WebPgId);
}

void TWebMemBs::AddWebPgRef(const TStr& UrlStr, const TStr& RefUrlStr){
  if (IsGenRef()){
    int WebPgId=UrlStrToWebPgIdH.GetDat(UrlStr);
    PUrl RefUrl=new TUrl(RefUrlStr); IAssert(RefUrl->IsOk(usHttp));
    WebPgIdToRefUrlStrVH.GetDat(WebPgId).AddMerged(RefUrl->GetUrlStr());
    Modified=true;
  }
}

bool TWebMemBs::DoStillFetchUrl(const TStr& UrlStr){
  if (IsCheckMode()){
    return !CheckedUrlStrToWebPgIdH.IsKey(UrlStr);
  } else {
    return !UrlStrToWebPgIdH.IsKey(UrlStr);
  }
}

bool TWebMemBs::DoFetchRedirUrl(const PUrl& RedirUrl, const TUrlV& UrlV){
  TStr RedirUrlStr=RedirUrl->GetUrlStr();
  if (UrlStrToWebPgIdH.IsKey(RedirUrlStr)){
    int WebPgId=UrlStrToWebPgIdH.GetDat(RedirUrlStr);
    for (int UrlN=0; UrlN<UrlV.Len(); UrlN++){
      TStr UrlStr=UrlV[UrlN]->GetUrlStr();
      UrlStrToWebPgIdH.AddDat(UrlStr, WebPgId);
      WebPgIdToUrlStrVH.GetDat(WebPgId).AddMerged(UrlStr);
    }
    return false;
  } else {
    return true;
  }
}

int TWebMemBs::FFirstWebPg(){
  return WebPgIdToUrlStrVH.FFirstKeyId();
}

bool TWebMemBs::FNextWebPg(int& WebPgP, int& WebPgId){
  if (WebPgIdToUrlStrVH.FNextKeyId(WebPgP)){
    WebPgId=WebPgIdToUrlStrVH.GetKey(WebPgP);
    return true;
  } else {
    return false;
  }
}

