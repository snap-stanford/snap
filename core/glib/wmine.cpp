/////////////////////////////////////////////////
// Includes
#include "wmine.h"

/////////////////////////////////////////////////
// Web-Log
void TWebLog::GetUsrUrlRankStep(
 const double& TaxFact, const TIntPrV& LRIdPrV,
 TFltIntKdV& WgtLIdKdV, TFltIntKdV& WgtRIdKdV, double& WgtDiff){
  // create new right-weights
  int WgtRIdKds=WgtRIdKdV.Len();
  TFltIntKdV NewWgtRIdKdV(WgtRIdKds, 0);
  for (int NewRIdN=0; NewRIdN<WgtRIdKds; NewRIdN++){
    NewWgtRIdKdV.Add(TFltIntKd(0, WgtRIdKdV[NewRIdN].Dat));}
  // traverse edges from left to right ids
  double TaxWealth=0;
  int LRIdKdN=0; int LRIdKds=LRIdPrV.Len(); double WgtSum=0;
  while (LRIdKdN<LRIdKds){
    // get user-id & weight
    int LId=LRIdPrV[LRIdKdN].Val1;
    double LWgt=(1-TaxFact)*WgtLIdKdV[LId].Key;
    TaxWealth+=TaxFact*WgtLIdKdV[LId].Key;
    // count urls per user
    int RIds=0;
    while ((LRIdKdN+RIds<LRIdKds)&&
     (LRIdPrV[LRIdKdN+RIds].Val1==LId)){RIds++;}
    // distribute user weight
    for (int RIdN=0; RIdN<RIds; RIdN++){
      int RId=LRIdPrV[LRIdKdN+RIdN].Val2;
      NewWgtRIdKdV[RId].Key+=LWgt/RIds;
      WgtSum+=LWgt/RIds;
    }
    // move to next user
    LRIdKdN+=RIds;
  }
  // get new right-side-weights, weight-difference & redistribute tax wealth
  for (int RIdWgtKdN=0; RIdWgtKdN<WgtRIdKds; RIdWgtKdN++){
    WgtDiff+=fabs(NewWgtRIdKdV[RIdWgtKdN].Key-WgtRIdKdV[RIdWgtKdN].Key);
    WgtRIdKdV[RIdWgtKdN].Key=0;
    WgtRIdKdV[RIdWgtKdN].Key+=NewWgtRIdKdV[RIdWgtKdN].Key;
    //WgtRIdKdV[RIdWgtKdN].Key+=TaxWealth/WgtRIdKds;
  }
}

void TWebLog::GetUsrUrlRank(
 TFltIntKdV& WgtUsrIdKdV, TFltIntKdV& WgtUrlIdKdV) const {
  // prepare weights
  int Usrs=GetUsrs(); int Urls=GetUrls();
  WgtUsrIdKdV.Gen(Usrs, 0); WgtUrlIdKdV.Gen(Urls, 0);
  for (int UsrId=0; UsrId<Usrs; UsrId++){WgtUsrIdKdV.Add(TFltIntKd(1, UsrId));}
  for (int UrlId=0; UrlId<Urls; UrlId++){WgtUrlIdKdV.Add(TFltIntKd(0, UrlId));}
  double TaxFact=0.15; int IterN=0; double KdevWgtDiff=0; double WgtDiff=0;
  forever {
    // user to url propagation
    GetUsrUrlRankStep(TaxFact, UsrIdUrlIdPrV, WgtUsrIdKdV, WgtUrlIdKdV, WgtDiff);
    // url to user propagation
    GetUsrUrlRankStep(TaxFact, UrlIdUsrIdPrV, WgtUrlIdKdV, WgtUsrIdKdV, WgtDiff);
    // stopping-criterium
    IterN++;
    printf("%d: %g\n", IterN, fabs(WgtDiff-KdevWgtDiff));
    if (fabs(WgtDiff-KdevWgtDiff)<0.001){break;}
    KdevWgtDiff=WgtDiff;
  }
  WgtUrlIdKdV.Sort(false); WgtUsrIdKdV.Sort(false);
}

void TWebLog::SaveTxtUURank(const PSOut& SOut,
 const TFltIntKdV& WgtUsrIdKdV, const TFltIntKdV& WgtUrlIdKdV,
 const int& TopRecs) const {
  // urls
  int TopUrls=TopRecs;
  if ((TopUrls==-1)||(TopUrls>GetUrls())){TopUrls=GetUrls();}
  SOut->PutStr("================================="); SOut->PutLn();
  SOut->PutStr("Best Urls"); SOut->PutLn();
  for (int UrlN=0; UrlN<TopUrls; UrlN++){
    double Wgt=WgtUrlIdKdV[UrlN].Key;
    int UrlId=WgtUrlIdKdV[UrlN].Dat;
    TStr UrlStr=GetUrlNm(UrlId);
    TChA OutLn;
    OutLn+=TInt::GetStr(UrlN+1, "%d.");
    OutLn+=TFlt::GetStr(Wgt, " (%g)");
    OutLn+=TStr::GetStr(UrlStr, " '%s'");
    SOut->PutStr(OutLn); SOut->PutLn();
  }
  // users
  int TopUsrs=TopRecs;
  if ((TopUsrs==-1)||(TopUsrs>GetUsrs())){TopUsrs=GetUsrs();}
  SOut->PutStr("================================="); SOut->PutLn();
  SOut->PutStr("Best Users"); SOut->PutLn();
  for (int UsrN=0; UsrN<TopUsrs; UsrN++){
    double Wgt=WgtUsrIdKdV[UsrN].Key;
    int UsrId=WgtUsrIdKdV[UsrN].Dat;
    TStr UsrStr=GetUsrNm(UsrId);
    TChA OutLn;
    OutLn+=TInt::GetStr(UsrN+1, "%d.");
    OutLn+=TFlt::GetStr(Wgt, " (%g)");
    OutLn+=TStr::GetStr(UsrStr, " '%s'");
    SOut->PutStr(OutLn); SOut->PutLn();
  }
}

void TWebLog::SaveXmlUURank(const PSOut& SOut,
 const TFltIntKdV& WgtUsrIdKdV, const TFltIntKdV& WgtUrlIdKdV,
 const int& TopRecs) const {
  // open top-tag
  SOut->PutStr("<UrlUserRank>"); SOut->PutLn();
  // urls
  int TopUrls=TopRecs;
  if ((TopUrls==-1)||(TopUrls>GetUrls())){TopUrls=GetUrls();}
  SOut->PutStr("<UrlRank>"); SOut->PutLn();
  for (int UrlN=0; UrlN<TopUrls; UrlN++){
    double Wgt=WgtUrlIdKdV[UrlN].Key;
    int UrlId=WgtUrlIdKdV[UrlN].Dat;
    TStr UrlStr=GetUrlNm(UrlId);
    TChA BTagChA;
    BTagChA+="<Rank";
    BTagChA+=TInt::GetStr(UrlN+1, " Place=\"%d\"");
    BTagChA+=TFlt::GetStr(Wgt, " Weigth=\"%g\"");
    BTagChA+=">";
    SOut->PutStr(BTagChA);
    TStr UrlXmlStr=TXmlLx::GetXmlStrFromPlainStr(UrlStr);
    SOut->PutStr(UrlXmlStr);
    SOut->PutStr("</Rank>");
    SOut->PutLn();
  }
  SOut->PutStr("</UrlRank>"); SOut->PutLn();
  // users
  int TopUsrs=TopRecs;
  if ((TopUsrs==-1)||(TopUsrs>GetUsrs())){TopUsrs=GetUsrs();}
  SOut->PutStr("<UserRank>"); SOut->PutLn();
  for (int UsrN=0; UsrN<TopUsrs; UsrN++){
    double Wgt=WgtUsrIdKdV[UsrN].Key;
    int UsrId=WgtUsrIdKdV[UsrN].Dat;
    TStr UsrStr=GetUsrNm(UsrId);
    TChA BTagChA;
    BTagChA+="<Rank";
    BTagChA+=TInt::GetStr(UsrN+1, " Place=\"%d\"");
    BTagChA+=TFlt::GetStr(Wgt, " Weigth=\"%g\"");
    BTagChA+=">";
    SOut->PutStr(BTagChA);
    TStr UsrXmlStr=TXmlLx::GetXmlStrFromPlainStr(UsrStr);
    SOut->PutStr(UsrXmlStr);
    SOut->PutStr("</Rank>");
    SOut->PutLn();
  }
  SOut->PutStr("</UserRank>"); SOut->PutLn();
  // close top tag
  SOut->PutStr("</UrlUserRank>"); SOut->PutLn();
}

PBowDocBs TWebLog::GetBowDocBs(
 const TStrIntH& DocNmToFqH, const TIntPrV& DIdWIdPrV,
 const TStrIntH& WordStrToFqH, const int& MnFq) const {
  printf("Generating Bag-Of-Words...\n");
  // create bag-of-words
  PBowDocBs BowDocBs=TBowDocBs::New();
  // generate data
  int DIdWIdPrs=DIdWIdPrV.Len();
  int PrevDId=-1; TStr DocNm; TStrV WordStrV;
  for (int DIdWIdPrN=0; DIdWIdPrN<DIdWIdPrs; DIdWIdPrN++){
    if (DIdWIdPrN%100==0){printf("%d/%d\r", DIdWIdPrN, DIdWIdPrs);}
    // get document & word ids
    int DId=DIdWIdPrV[DIdWIdPrN].Val1;
    int WId=DIdWIdPrV[DIdWIdPrN].Val2;
    // check for new document
    if ((PrevDId!=-1)&&(PrevDId!=DId)){
      //printf("[%d]", WordStrV.Len());
      //printf("[%s:%d]", DocNm.CStr(), WordStrV.Len());
      if (WordStrV.Len()>=MnFq){
        BowDocBs->AddDoc(DocNm, TStrV(), WordStrV, "");}
      DocNm=DocNmToFqH.GetKey(DId);
      WordStrV.Clr(false);
    }
    // add words to a current document
    WordStrV.Add(WordStrToFqH.GetKey(WId));
    // save doc-id
    PrevDId=DId;
  }
  if (!WordStrV.Empty()){
    if (WordStrV.Len()>=MnFq){
      BowDocBs->AddDoc(DocNm, TStrV(), WordStrV, "");}
  }
  // return bag-of-words
  BowDocBs->AssertOk();
  printf("\nDone.\n");
  return BowDocBs;
}

PBowDocBs TWebLog::GetUsrBowDocBsFromUrl(const int& MnFq) const {
  return GetBowDocBs(UsrNmToClicksH, UsrIdUrlIdPrV, UrlNmToClicksH, MnFq);
}

PBowDocBs TWebLog::GetUrlBowDocBsFromUsr(const int& MnFq) const {
  return GetBowDocBs(UrlNmToClicksH, UrlIdUsrIdPrV, UsrNmToClicksH, MnFq);
}

PBowDocBs TWebLog::GetUsrBowDocBsFromHtml(
 const int& MnFq, const TStr& WebRootFPath){
  printf("Generating Bag-Of-Words...\n");
  // create bag-of-words object
  PBowDocBs BowDocBs=TBowDocBs::New();
  // aliases
  TStrIntH& DocNmToFqH=UsrNmToClicksH;
  TIntPrV& DIdWIdPrV=UsrIdUrlIdPrV;
  TStrIntH& WordStrToFqH=UrlNmToClicksH;
  // prepare web-root-filepath
  TStr WebRootNrFPath=TStr::GetNrFPath(WebRootFPath);
  if ((WebRootNrFPath.Len()>0)&&(WebRootNrFPath.LastCh()=='/')){
    WebRootNrFPath.DelSubStr(WebRootNrFPath.Len()-1, WebRootNrFPath.Len()-1);}
  // prepare stop-words
  PSwSet SwSet=TSwSet::GetSwSet(swstEn523);
  // generate data
  int DIdWIdPrs=DIdWIdPrV.Len();
  int PrevDId=-1; TStr DocNm; TStrV WordStrV; int NotFoundDocs=0;
  for (int DIdWIdPrN=0; DIdWIdPrN<DIdWIdPrs; DIdWIdPrN++){
    if (DIdWIdPrN%10==0){
      printf("%d/%d (%d missed)\r", DIdWIdPrN, DIdWIdPrs, NotFoundDocs);}
    // get document & word ids
    int DId=DIdWIdPrV[DIdWIdPrN].Val1;
    int WId=DIdWIdPrV[DIdWIdPrN].Val2;
    // check for new document
    if ((PrevDId!=-1)&&(PrevDId!=DId)){
      //printf("[%d]", WordStrV.Len());
      //printf("[%s:%d]", DocNm.CStr(), WordStrV.Len());
      if (WordStrV.Len()>=MnFq){
        BowDocBs->AddDoc(DocNm, TStrV(), WordStrV, "");}
      DocNm=DocNmToFqH.GetKey(DId);
      WordStrV.Clr(false);
    }
    // add words to current document
    TStr HtmlFNm=WebRootNrFPath+WordStrToFqH.GetKey(WId);
    if (TFile::Exists(HtmlFNm)){
      PSIn HtmlSIn=TFIn::New(HtmlFNm);
      THtmlLx HtmlLx(HtmlSIn);
      while (HtmlLx.Sym!=hsyEof){
        if (HtmlLx.Sym==hsyStr){
          TStr WordStr=HtmlLx.UcChA;
          if (!SwSet->IsIn(WordStr)){
            //WordStr=TPorterStemmer::Stem(WordStr);
            WordStrV.Add(WordStr);
          }
        }
        HtmlLx.GetSym();
      }
    } else {
      NotFoundDocs++;
    }
    // save doc-id
    PrevDId=DId;
  }
  if (!WordStrV.Empty()){
    if (WordStrV.Len()>=MnFq){
      BowDocBs->AddDoc(DocNm, TStrV(), WordStrV, "");}
  }
  // return bag-of-words
  BowDocBs->AssertOk();
  printf("\nDone.\n");
  return BowDocBs;
}

PWebLog TWebLog::LoadTxt(const TStr& FNm, const int& MxRecs,
 const TSsFmt& SsFmt, const bool& HdLnP){
  printf("Loading web-log '%s' ...\n", FNm.CStr());
  // create web-log
  PWebLog WebLog=TWebLog::New();
  // open log file
  PSIn SIn=TFIn::New(FNm);
  // get field-numbers
  int UsrIdFldN=0; int UrlIdFldN=1;
  if (HdLnP){
    char Ch=' '; TStrV FldNmV; TSs::LoadTxtFldV(SsFmt, SIn, Ch, FldNmV);
    UsrIdFldN=FldNmV.SearchForw("UserID");
    UrlIdFldN=FldNmV.SearchForw("URLID");
  }
  // traverse & load file
  TStrV FldValV; int Recs=0;
  while (!SIn->Eof()){
    if ((MxRecs!=-1)&&(Recs>MxRecs)){break;}
    Recs++; if (Recs%100==0){printf("%d\r", Recs);}
    char Ch=' '; TSs::LoadTxtFldV(SsFmt, SIn, Ch, FldValV);
    TStr UsrNm=FldValV[UsrIdFldN].GetLc();
    TStr UrlNm=FldValV[UrlIdFldN].GetLc();
    int UsrId=WebLog->UsrNmToClicksH.AddKey(UsrNm);
    int UrlId=WebLog->UrlNmToClicksH.AddKey(UrlNm);
    WebLog->UsrNmToClicksH[UsrId]++;
    WebLog->UrlNmToClicksH[UrlId]++;
    WebLog->UsrIdUrlIdPrV.Add(TIntPr(UsrId, UrlId));
    WebLog->UrlIdUsrIdPrV.Add(TIntPr(UrlId, UsrId));
  }
  // sort duplicate hits
  WebLog->UsrIdUrlIdPrV.Sort();
  WebLog->UrlIdUsrIdPrV.Sort();
  // return weblog
  printf("   Users:%d   Urls:%d   Clicks:%d\n",
   WebLog->GetUsrs(), WebLog->GetUrls(), WebLog->GetClicks());
  printf("Done.\n");
  return WebLog;
}

PWebLog TWebLog::LoadBinOrTxt(
 const TStr& BinFNm, const TStr& TxtFNm, const int& MxRecs){
  PWebLog WebLog;
  if (TFile::Exists(BinFNm)){
    printf("Loading binary file '%s' ... ", BinFNm.CStr());
    WebLog=TWebLog::LoadBin(BinFNm);
    printf("Done.\n");
  } else {
    printf("Loading text file '%s' ... ", TxtFNm.CStr());
    WebLog=LoadTxt(TxtFNm, MxRecs);
    printf("Done.\n");
    if (!BinFNm.Empty()){
      printf("Saving binary file '%s'... ", BinFNm.CStr());
      WebLog->SaveBin(BinFNm);
      printf("Done.\n");
    }
  }
  return WebLog;
}

PWebLog TWebLog::LoadYbTxt(const TStr& FNm, const int& MxRecs){
  printf("Loading Yield-Broker Web-Log '%s' ...\n", FNm.CStr());
  // create web-log
  PWebLog WebLog=TWebLog::New();
  // open log file
  PSIn SIn=TFIn::New(FNm);
  // get field-numbers
  char Ch=' '; TStrV FldNmV; TSs::LoadTxtFldV(ssfCommaSep, SIn, Ch, FldNmV);
  int UsrIdFldN=FldNmV.SearchForw("UserID");
  int UrlIdFldN=FldNmV.SearchForw("URLID");
  // traverse & load file
  TStrV FldValV; int Recs=0;
  while (!SIn->Eof()){
    if ((MxRecs!=-1)&&(Recs>MxRecs)){break;}
    Recs++; if (Recs%100==0){printf("%d\r", Recs);}
    char Ch=' '; TSs::LoadTxtFldV(ssfCommaSep, SIn, Ch, FldValV);
    TStr UsrNm=FldValV[UsrIdFldN].GetLc();
    TStr UrlNm=FldValV[UrlIdFldN].GetLc();
    int UsrId=WebLog->UsrNmToClicksH.AddKey(UsrNm);
    int UrlId=WebLog->UrlNmToClicksH.AddKey(UrlNm);
    WebLog->UsrNmToClicksH[UsrId]++;
    WebLog->UrlNmToClicksH[UrlId]++;
    WebLog->UsrIdUrlIdPrV.Add(TIntPr(UsrId, UrlId));
    WebLog->UrlIdUsrIdPrV.Add(TIntPr(UrlId, UsrId));
  }
  // sort duplicate hits
  WebLog->UsrIdUrlIdPrV.Sort();
  WebLog->UrlIdUsrIdPrV.Sort();
  // return weblog
  printf("   Users:%d   Urls:%d   Clicks:%d\n",
   WebLog->GetUsrs(), WebLog->GetUrls(), WebLog->GetClicks());
  printf("Done.\n");
  return WebLog;
}

