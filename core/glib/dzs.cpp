/////////////////////////////////////////////////
// Includes
#include "StdAfx.h"
#include "dzs.h"

/////////////////////////////////////////////////
// Dzs-Base-Document
void TDzsBsDoc::AddDataTokToChA(const PXmlTok& Tok, TChA& ChA){
  switch (Tok->GetSym()){
    case xsyWs:
    case xsyStr:
    case xsyQStr:
      ChA+=Tok->GetStr(); break;
    case xsyTag:{
      TStr TagNm=Tok->GetStr();
      if ((TagNm=="GUID")||(TagNm=="REFF")||(TagNm=="TYPE")||(TagNm=="FILE")){break;}
      if ((TagNm=="TITLE")||(TagNm=="TOPICTITLE")){break;}
      ChA+=' ';
      if ((TagNm=="TITLE")||(TagNm=="TOPICTITLE")){ChA+="<TITLE>";}
      for (int SubTokN=0; SubTokN<Tok->GetSubToks(); SubTokN++){
        AddDataTokToChA(Tok->GetSubTok(SubTokN), ChA);}
      if ((TagNm=="TITLE")||(TagNm=="TOPICTITLE")){ChA+="</TITLE>";}
      ChA+=' '; break;}
    default: Fail;
  }
}

TStr TDzsBsDoc::GetDataTokStr(const PXmlTok& Tok){
  TChA ChA;
  AddDataTokToChA(Tok, ChA);
  ChA.CompressWs();
  return ChA;
}

TStr TDzsBsDoc::GetDataTokVStr(const TXmlTokV& TokV, const TStr& SepStr){
  TChA ChA;
  for (int TokN=0; TokN<TokV.Len(); TokN++){
    if (TokN>0){ChA+=SepStr;}
    ChA+=GetDataTokStr(TokV[TokN]);
  }
  return ChA;
}

void TDzsBsDoc::GetDocParts(
 const TStr& FNm, const PXmlDoc& XmlDoc,
 const TStr& FPath, const TStr& WebAlias,
 bool& Ok, TStr& IdStr, TStr& TitleStr, TStr& DataStr, int& YearN){
  Ok=false;
  if (!XmlDoc->IsOk()){return;}
  // id
  IdStr=FNm;
  IdStr.ChangeStr(FPath, WebAlias);
//  PXmlTok IdTok;
//  if (XmlDoc->IsTagTok("term|metadata|identifier", IdTok)){
//    IdStr=IdTok->GetTokStr(false);}
//  else {return;}
  // title
  PXmlTok TitleTok;
  if (XmlDoc->IsTagTok("term|metadata|title", TitleTok)){
    TitleStr=TitleTok->GetTokStr(false);}
  else {return;}
  // timedata
  TXmlTokV TimeDataTokV;
  XmlDoc->GetTagTokV("term|data|frame|timedata|fromyear", TimeDataTokV);
  TStr TimeDataStr=TXmlTok::GetTokVStr(TimeDataTokV, false);
  if (TimeDataStr.IsInt(YearN)){} else {YearN=0;}
  // locdata
  TXmlTokV LocDataTokV;
  XmlDoc->GetTagTokV("term|data|frame|locdata", LocDataTokV);
  TStr LocDataStr=TXmlTok::GetTokVStr(LocDataTokV, true);
  // pages
  TXmlTokV PageTokV; XmlDoc->GetTagTokV("term|data|frame|page", PageTokV);
  DataStr=GetDataTokVStr(PageTokV, "\n")+" "+LocDataStr;
  // character-set transformation
  TitleStr=THtmlLxChDef::GetCSZFromWin1250(TitleStr);
  DataStr=THtmlLxChDef::GetCSZFromWin1250(DataStr);
  // success
  Ok=true;
}

PDzsBsDoc TDzsBsDoc::GetDzsBsDoc(
 const TStr& FNm, const PXmlDoc& XmlDoc,
 const TStr& FPath, const TStr& WebAlias){
  TStr TitleStr; TStr IdStr; TStr DataStr; int YearN;
  if (!XmlDoc->IsOk()){return NULL;}
  bool Ok;
  GetDocParts(FNm, XmlDoc, FPath, WebAlias, Ok, IdStr, TitleStr, DataStr, YearN);
  if (!Ok){return NULL;}
  PDzsBsDoc DzsBsDoc=TDzsBsDoc::New(IdStr, TitleStr, DataStr, YearN);
//  printf("---------------------\n");
//  printf("%s\n", TitleStr.CStr());
//  printf("%s\n", IdStr.CStr());
//  printf("%s\n", DataStr.CStr());
//  printf("%s\n", TInt::GetStr(YearN));
  return DzsBsDoc;
}

void TDzsBsDoc::GetDocParts(
 const TStr& FNm, const PHtmlDoc& HtmlDoc,
 const TStr& FPath, const TStr& WebAlias,
 bool& Ok, TStr& IdStr, TStr& TitleStr, TStr& DataStr, int& YearN){
  // id
  IdStr=FNm;
  IdStr.ChangeStr(FPath, WebAlias);
  // title & data
  TChA TitleChA; TChA DataChA;
  PHtmlTok Tok; THtmlLxSym Sym; TStr Str; bool InTitleTag=false;
  for (int TokN=0; TokN<HtmlDoc->GetToks(); TokN++){
    Tok=HtmlDoc->GetTok(TokN, Sym, Str);
    if ((Sym==hsyBTag)&&(Str==THtmlTok::TitleTagNm)){InTitleTag=true;}
    else if ((Sym==hsyETag)&&(Str==THtmlTok::TitleTagNm)){InTitleTag=false;}
    else if (Sym==hsyStr){
      if (InTitleTag){TitleChA+=' '; TitleChA+=Str;}
      else {DataChA+=' '; DataChA+=Str;}
    }
  }
  TitleStr=TitleChA;
  DataStr=DataChA;
  YearN=0;
  if (DataStr.Empty()){Ok=false; return;}
  // success
  Ok=true;
}

PDzsBsDoc TDzsBsDoc::GetDzsBsDoc(
 const TStr& FNm, const PHtmlDoc& HtmlDoc,
 const TStr& FPath, const TStr& WebAlias){
  TStr TitleStr; TStr IdStr; TStr DataStr; int YearN;
  bool Ok;
  GetDocParts(FNm, HtmlDoc, FPath, WebAlias, Ok, IdStr, TitleStr, DataStr, YearN);
  if (!Ok){return NULL;}
  PDzsBsDoc DzsBsDoc=TDzsBsDoc::New(IdStr, TitleStr, DataStr, YearN);
  return DzsBsDoc;
}

/////////////////////////////////////////////////
// DzsBs
bool TDzsBs::IsReg=TDzsBs::MkReg();

void TDzsBs::AddDoc(const PDzsBsDoc& DzsBsDoc){
  if (!DzsBsDoc.Empty()){
    int DzsBsDocId=DzsBsDocV.Add(DzsBsDoc);
    Bix->AddSec(DzsBsDocId, DzsBsDoc->GetTitleStr(), true);
    Bix->AddSec(DzsBsDocId, DzsBsDoc->GetDataStr(), false);
    if (DzsBsDoc->GetYearN()!=0){
      Bix->AddKeyVal(DzsBsDocId, "year", DzsBsDoc->GetYearN());}
  }
}

TDzsBs::TDzsBs(const TStr& FPath, const TStr& WebAlias):
  TBook("Dzs-Base", "Dzs", "Lexicon"),
  DzsBsDocV(), Bix(TBix::New()){
  TStrV FPathV; FPathV.Add(FPath);
  TStrV FExtV;
  FExtV.Add("xml"); FExtV.Add("html"); FExtV.Add("htm"); FExtV.Add("txt");
  TFFile FFile(FPathV, FExtV, true); TStr FNm; int FNmN=0;
  while (FFile.Next(FNm)){
    printf("%d\r", ++FNmN);
    PDzsBsDoc DzsBsDoc;
    if (FNm.GetFExt().GetUc()==".XML"){
      PXmlDoc XmlDoc=TXmlDoc::LoadTxt(FNm);
      DzsBsDoc=TDzsBsDoc::GetDzsBsDoc(FNm, XmlDoc, FPath, WebAlias);
    } else {
      PHtmlDoc HtmlDoc=THtmlDoc::LoadTxt(FNm, hdtAll, false);
      DzsBsDoc=TDzsBsDoc::GetDzsBsDoc(FNm, HtmlDoc, FPath, WebAlias);
    }
    AddDoc(DzsBsDoc);
  }
}

void TDzsBs::GetSecInfo(
 const int& SecId, TStr& SecIdStr, TStr& TitleStr, TStr& SecStr) const {
  PDzsBsDoc DzsBsDoc=DzsBsDocV[SecId];
  SecIdStr=DzsBsDoc->GetIdStr();
  TitleStr=DzsBsDoc->GetTitleStr();
  SecStr=DzsBsDoc->GetDataStr();
}

/////////////////////////////////////////////////
// Dzs-Base ActiveX-Interface
bool TDzsBsX::LoadBook(const TStr& FNm){
  BixRes=NULL;
  bool Opened; PSIn SIn=TFIn::New(FNm, Opened);
  if (!Opened){return false;}
  Book=TBook::Load(*SIn);
  return true;
}

void TDzsBsX::UnloadBook(){
  BixRes=NULL; Book=NULL;
}

int TDzsBsX::GetBookSecs(){
  if (Book.Empty()){return -1;}
  else {return Book->GetSecs();}
}

bool TDzsBsX::GetBookSecInfo(
 const int& SecId, TStr& SecIdStr, TStr& TitleStr, TStr& SecStr){
  if (Book.Empty()){return false;}
  if ((SecId<0)||(Book->GetSecs()<=SecId)){return false;}
  Book->GetSecInfo(SecId, SecIdStr, TitleStr, SecStr);
  return true;
}

bool TDzsBsX::SearchBook(const TStr& QueryStr){
  if (Book.Empty()){return false;}
  BixRes=Book->Search(QueryStr);
  return true;
}

bool TDzsBsX::IsResultOk(){
  if (BixRes.Empty()){return false;}
  return BixRes->IsOk();
}

bool TDzsBsX::GetResultMsgStr(TStr& MsgStr){
  if (BixRes.Empty()){return false;}
  MsgStr=BixRes->GetMsgStr();
  return true;
}

bool TDzsBsX::GetResultQueryStr(TStr& QueryStr){
  if (BixRes.Empty()){return false;}
  QueryStr=BixRes->GetQueryStr();
  return true;
}

int TDzsBsX::GetResultHits(){
  if (BixRes.Empty()){return -1;}
  if (!BixRes->IsOk()){return -1;}
  return BixRes->GetHits();
}

bool TDzsBsX::GetResultHitInfo(
 const int& HitN, const int& MxTitleLen, const int& MxCtxLen,
 TStr& SecIdStr, TStr& TitleStr, TStr& SecStr, TStr& CtxStr){
  if (BixRes.Empty()){return false;}
  if (!BixRes->IsOk()){return false;}
  if ((HitN<0)||(BixRes->GetHits()<=HitN)){return false;}
  BixRes->GetHitInfo(
   Book, HitN, MxTitleLen, MxCtxLen,
   SecIdStr, TitleStr, SecStr, CtxStr);
  return true;
}

/////////////////////////////////////////////////
// Dzs-Keyword-Base
TDzsKwBs::TDzsKwBs(const TStr& FNm):
  KwToSecIdVH(10000){
  PSIn SIn=TFIn::New(FNm);
  TILx Lx(SIn, TFSet()|iloTabSep);
  Lx.GetSym(syStr, syEoln, syEof);
  while (Lx.Sym!=syEof){
    if (Lx.Sym==syStr){
      KwToSecIdVH.AddDat(Lx.Str);
      while (Lx.GetSym()!=syEoln){}
    }
    Lx.GetSym(syStr, syEoln, syEof);
  }
}

void TDzsKwBs::AddHits(const PBook& Book, const TStr& FNm){
  PSOut SOut=TFOut::New(FNm);
  TOLx Lx(SOut, TFSet()|oloFrcEoln|oloSigNum|oloTabSep);
  int KwToSecIdVP=KwToSecIdVH.FFirstKeyId();
  while (KwToSecIdVH.FNextKeyId(KwToSecIdVP)){
    TChA QueryChA=KwToSecIdVH.GetKey(KwToSecIdVP);
    for (int QueryChN=0; QueryChN<QueryChA.Len(); QueryChN++){
      if ((' '<QueryChA[QueryChN])&&(QueryChA[QueryChN]<='~')){
        if (QueryChA[QueryChN]!='*'){
          if (!TCh::IsAlNum(QueryChA[QueryChN])){
            QueryChA.PutCh(QueryChN, ' ');}
        }
      }
    }
    TStr QueryStr=QueryChA;
    PBixRes BixRes=Book->Search(QueryStr);
    if (BixRes->IsOk()){
      Lx.PutUQStr(QueryStr); Lx.PutUQStr(TInt::GetStr(BixRes->GetHits()));
      printf("%s - %d\n", QueryStr.CStr(), BixRes->GetHits());
      int Hits=BixRes->GetHits(); // TInt::GetMn(BixRes->GetHits(), 10);
      for (int HitN=0; HitN<Hits; HitN++){
        TStr SecIdStr; TStr TitleStr; TStr SecStr; TStr CtxStr;
        BixRes->GetHitInfo(
         Book, HitN, -1, 100, SecIdStr, TitleStr, SecStr, CtxStr);
        KwToSecIdVH[KwToSecIdVP].Add(SecIdStr);
        Lx.PutUQStr(SecIdStr);
      }
      Lx.PutLn();
    } else {
      Lx.PutUQStr(QueryStr); Lx.PutUQStr("Error"); Lx.PutStr(BixRes->GetMsgStr());
      printf("%s - Error - %s\n", QueryStr.CStr(), BixRes->GetMsgStr().CStr());
      Lx.PutLn();
    }
  }
}

/////////////////////////////////////////////////
// Dzs-Hierarchical-Keyword
TDzsHKw::TDzsHKw(const TStr& _KwNm, const int& _Lev, const TStrV& _QueryStrV):
  KwNm(_KwNm), Lev(_Lev), QueryStrV(_QueryStrV), SecIdStrV(){
  if (QueryStrV.Empty()){
    TChA QueryChA=KwNm;
    for (int QueryChN=0; QueryChN<QueryChA.Len(); QueryChN++){
      if ((' '<QueryChA[QueryChN])&&(QueryChA[QueryChN]<='~')){
        if (QueryChA[QueryChN]!='*'){
          if (!TCh::IsAlNum(QueryChA[QueryChN])){
            QueryChA.PutCh(QueryChN, ' ');}
        }
      }
    }
    QueryStrV.Add(QueryChA);
  }
}

/////////////////////////////////////////////////
// Dzs-Hierarchical-Keyword-Base
bool TDzsHKwBs::IsTopicOk(const PXmlTok& TopicTok){
  // <topic> is expected
  if (!TopicTok->IsTag("topic")){return false;}
  // <title> is mandatory subtag
  if (!TopicTok->IsSubTag("title")){return false;}
  // recurse
  for (int SubTokN=0; SubTokN<TopicTok->GetSubToks(); SubTokN++){
    PXmlTok CurTok=TopicTok->GetSubTok(SubTokN);
    if (CurTok->IsTag("topic")){
      if (!IsTopicOk(CurTok)){return false;}
    }
  }
  return true;
}

void TDzsHKwBs::AddHits(const PXmlTok& TopicTok, const PBook& Book){
  IAssert(TopicTok->IsTag("topic"));
  // execute query & add hits
  if (TopicTok->IsSubTag("query")){
    // get query string
    TStr QueryStr=TopicTok->GetTagTok("query")->GetTokStr(false);
    // search
    PBixRes BixRes=Book->Search(QueryStr);
    if (BixRes->IsOk()){
      printf("%s - %d\n", QueryStr.CStr(), BixRes->GetHits());
      int Hits=BixRes->GetHits();
      // retrieve or create <refs> subtag
      PXmlTok RefsTok;
      if (Hits>0){
        if (TopicTok->IsSubTag("refs")){
          RefsTok=TopicTok->GetTagTok("refs");
        } else {
          RefsTok=TXmlTok::New(xsyTag, "refs");
          TopicTok->AddSubTok(RefsTok);
        }
      }
      // add hits
      for (int HitN=0; HitN<Hits; HitN++){
        // get hit info
        TStr SecIdStr; TStr TitleStr; TStr SecStr; TStr CtxStr;
        BixRes->GetHitInfo(
         Book, HitN, -1, 100, SecIdStr, TitleStr, SecStr, CtxStr);
        TStr TitleXmlStr=TXmlLx::GetXmlFromPlainStr(TitleStr);
        TStr UrlXmlStr=TXmlLx::GetXmlFromPlainStr(SecIdStr);
        // create <ref>
        PXmlTok RefTok=TXmlTok::New(xsyTag, "ref");
        //RefTok->AddArg("title", TitleXmlStr);
        RefsTok->AddSubTok(RefTok);
        // create <title>
        PXmlTok TitleTok=TXmlTok::New(xsyTag, "title");
        RefTok->AddSubTok(TitleTok);
        PXmlTok TitleStrTok=TXmlTok::New(xsyStr, TitleXmlStr);
        TitleTok->AddSubTok(TitleStrTok);
        // create <url>
        PXmlTok UrlTok=TXmlTok::New(xsyTag, "url");
        RefTok->AddSubTok(UrlTok);
        PXmlTok UrlStrTok=TXmlTok::New(xsyStr, UrlXmlStr);
        UrlTok->AddSubTok(UrlStrTok);
      }
    }
  }
  // recurse
  for (int SubTokN=0; SubTokN<TopicTok->GetSubToks(); SubTokN++){
    PXmlTok CurTok=TopicTok->GetSubTok(SubTokN);
    if (CurTok->IsTag("topic")){
      AddHits(CurTok, Book);
    }
  }
}

TStr TDzsHKwBs::GetQueryStr(const TStr& TxtStr){
  TChA QueryChA=TXmlLx::GetPlainFromXmlStr(TxtStr);
  QueryChA.ChangeCh('(', '|');
  QueryChA.ChangeCh(')', ' ');
  QueryChA.ChangeCh('.', ' ');
  QueryChA.ChangeCh('/', ' ');
  QueryChA.ChangeCh('-', ' ');
  return QueryChA;
}

TStr TDzsHKwBs::GetFPathSegStr(const TStr& TxtStr){
  TChA FPathChA=TXmlLx::GetUsAsciiFromXmlStr(TxtStr);
  // extract alpha-num chars; change others to '_'; stop at '('
  for (int ChN=0; ChN<FPathChA.Len(); ChN++){
    char Ch=FPathChA[ChN];
    if (Ch=='('){FPathChA.Trunc(ChN); break;}
    if (!TCh::IsAlNum(Ch)){FPathChA.PutCh(ChN, ' ');}
  }
  FPathChA.CompressWs();
  FPathChA.ChangeCh(' ', '_');
  FPathChA.Trunc(16);
  return FPathChA;
}

PDzsHKwBs TDzsHKwBs::LoadTxt(const TStr& FNm){
  TSStack<PXmlTok> TokS;
  PXmlTok RootTok;
  PSIn SIn=TFIn::New(FNm);
  TILx Lx(SIn, TFSet()|iloTabSep);
  Lx.GetSym(syTab, syStr, syEoln, syEof);
  int PrevLev=-1;
  while (Lx.Sym!=syEof){
    printf(".");
    int Lev=0; while (Lx.Sym==syTab){Lev++; Lx.GetSym(syTab, syStr);}
    IAssert(Lx.Sym==syStr);
    for (int ChN=0; ChN<Lx.Str.Len(); ChN++){
      uchar Ch=Lx.Str[ChN];
      if (Ch>127){printf("%c", Ch); Fail;}}
    // create topic
    PXmlTok TopicTok=TXmlTok::New(xsyTag, "topic");
    //TopicTok->AddArg("title", Lx.Str);
    // topic title
    TStr TitleStr=TXmlLx::GetChRefFromYuEntRef(Lx.Str);
    PXmlTok TitleTok=TXmlTok::New(xsyTag, "title");
    TopicTok->AddSubTok(TitleTok);
    PXmlTok TitleStrTok=TXmlTok::New(xsyStr, TitleStr);
    TitleTok->AddSubTok(TitleStrTok);
    // topic query
    TStr QueryStr=GetQueryStr(TitleStr);
    PXmlTok QueryTok=TXmlTok::New(xsyTag, "query");
    TopicTok->AddSubTok(QueryTok);
    PXmlTok QueryStrTok=TXmlTok::New(xsyStr, QueryStr);
    QueryTok->AddSubTok(QueryStrTok);
    // insert topic into tree
    if (Lev==0){
      IAssert(RootTok.Empty());
      RootTok=TopicTok;
      TokS.Push(RootTok);
    } else
    if (PrevLev+1==Lev){
      TokS.Top()->AddSubTok(TopicTok);
      TokS.Push(TopicTok);
    } else
    if (PrevLev==Lev){
      TokS.Pop();
      TokS.Top()->AddSubTok(TopicTok);
      TokS.Push(TopicTok);
    } else
    if (PrevLev>Lev){
      int UpLev=PrevLev;
      while (UpLev>Lev){UpLev--; TokS.Pop();}
      TokS.Pop();
      TokS.Top()->AddSubTok(TopicTok);
      TokS.Push(TopicTok);
    } else {
      Fail;
    }
    while (Lx.GetSym()!=syEoln){}
    Lx.GetSym(syTab, syStr, syEoln, syEof);
    PrevLev=Lev;
    //if (Lx.SymLnN>100){break;}
  }
  PXmlDoc XmlDoc=TXmlDoc::New(RootTok);
  PDzsHKwBs HKwBs=TDzsHKwBs::New(XmlDoc);
  return HKwBs;
}

void TDzsHKwBs::SaveHtml(const PXmlTok& TopicTok, const TStr& FPath){
  TStr NrFPath=TStr::GetNrFPath(FPath);
  TStr TitleStr=TopicTok->GetTagTok("title")->GetTokStr(false);
  TStr FPathSeg=GetFPathSegStr(TitleStr);
  printf("%s\n", TitleStr.CStr());
  TDir::GenDir(NrFPath+FPathSeg);
  PSOut SOut=TFOut::New(NrFPath+FPathSeg+"/"+"default.htm");
  SOut->PutStr("<html>"); SOut->PutDosLn();
  SOut->PutDosLn();
  // head
  SOut->PutStr("<head>"); SOut->PutDosLn();
  SOut->PutStr("<title>"); SOut->PutStr(TitleStr); SOut->PutStr("</title>"); SOut->PutDosLn();
  SOut->PutStr("</head>"); SOut->PutDosLn();
  SOut->PutDosLn();
  // body-start
  SOut->PutStr("<body>"); SOut->PutDosLn();
  SOut->PutStr("<center><h3>");
  SOut->PutStr(TitleStr);
  SOut->PutStr("</h3></center"); SOut->PutDosLn();
  // subtrees
  SOut->PutStr("<ul>"); SOut->PutDosLn();
  for (int SubTokN=0; SubTokN<TopicTok->GetSubToks(); SubTokN++){
    PXmlTok CurTok=TopicTok->GetSubTok(SubTokN);
    if (CurTok->IsTag("topic")){
      TStr SubTitleStr=CurTok->GetTagTok("title")->GetTokStr(false);
      TStr SubFPathSeg=GetFPathSegStr(SubTitleStr);
      SOut->PutStr("<li>");
      SOut->PutStr(TStr("<a href=\"")+SubFPathSeg+"/\">");
      SOut->PutStr("<b>"); SOut->PutStr(SubTitleStr); SOut->PutStr("</b>");
      SOut->PutStr("</a>");
      SOut->PutStr("</li>"); SOut->PutDosLn();
      SaveHtml(CurTok, NrFPath+FPathSeg);
    }
  }
  SOut->PutStr("</ul>"); SOut->PutDosLn();
  // references
  if (TopicTok->IsSubTag("refs")){
    SOut->PutDosLn(); SOut->PutStr("<hr>"); SOut->PutDosLn(); SOut->PutDosLn();
    PXmlTok RefsTok=TopicTok->GetTagTok("refs");
    SOut->PutStr("<ul>"); SOut->PutDosLn();
    for (int RefTokN=0; RefTokN<RefsTok->GetSubToks(); RefTokN++){
      // get <ref> data
      PXmlTok RefTok=RefsTok->GetSubTok(RefTokN);
      TStr RefTitleStr=RefTok->GetTagTok("title")->GetTokStr(false);
      TStr RefUrlStr=RefTok->GetTagTok("url")->GetTokStr(false);
      // generate <li>
      SOut->PutStr("<li>");
      SOut->PutStr(TStr("<a href=\"")+RefUrlStr+"\">");
      SOut->PutStr(RefTitleStr);
      SOut->PutStr("</a>");
      SOut->PutStr("</li>"); SOut->PutDosLn();
    }
    SOut->PutStr("</ul>"); SOut->PutDosLn();
  }
  // footer
  SOut->PutStr("<center>"); SOut->PutDosLn();
  SOut->PutStr("<small>");
  SOut->PutStr("Copyright DZS");
  SOut->PutStr("</small>"); SOut->PutDosLn();
  SOut->PutStr("</center>"); SOut->PutDosLn();
  // body-end
  SOut->PutStr("</body>"); SOut->PutDosLn();
  SOut->PutStr("</html>"); SOut->PutDosLn();
}

