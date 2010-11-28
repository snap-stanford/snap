/////////////////////////////////////////////////
// Includes
#include "google.h"

/////////////////////////////////////////////////
// Google-Scholar-Reference
TMd5Sig TGgSchRef::GetMd5Sig() const {
  TChA ChA;
  ChA+=TitleStr;
  for (int AuthN=0; AuthN<AuthNmV.Len(); AuthN++){
    ChA+=';'; ChA+=AuthNmV[AuthN];}
  ChA+=';'; ChA+=PubNm;
  ChA+=';'; ChA+=YearStr;
  return TMd5Sig(ChA);
}

void TGgSchRef::GetAuthNmVPubStr(
 const TStr& AuthNmVPubStr, TStrV& AuthNmV, TStr& PubNm, TStr& PubYearStr){
  // split input string into two parts
  TStr AuthNmVStr; TStr PubStr;
  AuthNmVPubStr.SplitOnStr(AuthNmVStr, " - ", PubStr);
  // author-names string
  AuthNmVStr.SplitOnAllCh(',', AuthNmV, true);
  for (int AuthN=0; AuthN<AuthNmV.Len(); AuthN++){
    AuthNmV[AuthN].ToTrunc();
  }
  if ((!AuthNmV.Empty())&&
   ((AuthNmV.Last().IsStrIn("..."))||(AuthNmV.Last().Len()<=2))){
    AuthNmV.DelLast();
  }
  // publication-name & publication-year string
  TStr OriginStr; TStr LinkStr;
  PubStr.SplitOnStr(OriginStr, " - ", LinkStr);
  OriginStr.SplitOnLastCh(PubNm, ',', PubYearStr);
  PubNm.ToTrunc(); PubYearStr.ToTrunc();
  if ((PubYearStr.Len()>=4)&&(PubYearStr.GetSubStr(0, 3).IsInt())){
    PubYearStr=PubYearStr.GetSubStr(0, 3);
  } else
  if ((PubNm.Len()>=4)&&(PubNm.GetSubStr(0, 3).IsInt())){
    PubYearStr=PubNm.GetSubStr(0, 3); PubNm="";
  } else {
    PubYearStr="";
  }
}

void TGgSchRef::SaveXml(FILE* fOut, const int& RefN){
  if (RefN==-1){
    fprintf(fOut, "    <Ref>\n");
  } else {
    fprintf(fOut, "    <Ref Num=\"%d\">\n", RefN);
  }
  fprintf(fOut, "    <MD5>%s</MD5>\n", GetMd5Sig().GetStr().CStr());
  fprintf(fOut, "      <Title>%s</Title>\n", TXmlLx::GetXmlStrFromPlainStr(TitleStr).CStr());
  if (AuthNmV.Len()>0){
    fprintf(fOut, "      <Authors>");
    for (int AuthN=0; AuthN<AuthNmV.Len(); AuthN++){
      fprintf(fOut, "<Author>%s</Author>", TXmlLx::GetXmlStrFromPlainStr(AuthNmV[AuthN]).CStr());
    }
    fprintf(fOut, "</Authors>\n");
  }
  if (!PubNm.Empty()){
    fprintf(fOut, "      <Pub>%s</Pub>\n", TXmlLx::GetXmlStrFromPlainStr(PubNm).CStr());}
  if (!YearStr.Empty()){
    fprintf(fOut, "      <Year>%s</Year>\n", TXmlLx::GetXmlStrFromPlainStr(YearStr).CStr());}
  fprintf(fOut, "      <Citations>%d</Citations>\n", Citations);
  if (!CitedByUrlStr.Empty()){
    fprintf(fOut, "      <CitedByUrl>%s</CitedByUrl>\n", TXmlLx::GetXmlStrFromPlainStr(CitedByUrlStr).CStr());}
  fprintf(fOut, "    </Ref>\n");
}

/////////////////////////////////////////////////
// Google-Scholar-Result-Set
PGgSchRSet TGgSchRSet::NewScholar(const TStr& UrlStr, const TStr& HtmlStr){
  // prepare object
  PGgSchRSet RSet=TGgSchRSet::New();

  // prepare html browsing
  PSIn HtmlSIn=TStrIn::New(HtmlStr);
  THtmlLx HtmlLx(HtmlSIn);

  // extract header info
  HtmlLx.MoveToStrOrEof("Results");
  TStr FromResultStr=HtmlLx.GetStrInTag("<B>", true);
  TStr ToResultStr=HtmlLx.GetStrInTag("<B>", true);
  TStr AllHitsStr=HtmlLx.GetStrInTag("<B>", true);
  AllHitsStr.DelChAll(',');
  TStr QueryStr=HtmlLx.GetStrInTag("<B>", true);

  // traverse hits
  HtmlLx.MoveToBTagOrEof("<P>");
  forever {
    if (!((HtmlLx.Sym==hsyBTag)&&(HtmlLx.UcChA=="<P>"))){break;}
    HtmlLx.GetSym();
    if (HtmlLx.Sym==hsyBTag){
      TStr FullBTagStr=HtmlLx.GetFullBTagStr();
      if (FullBTagStr=="<FONT SIZE=\"-2\">"){
        TStr PubTypeNm=HtmlLx.GetStrInTag("<B>", true);
      } else
      if (FullBTagStr=="<SPAN CLASS=\"w\">"){
      } else {
        break;
      }
      TStr TitleStr=HtmlLx.GetStrToBTag("<BR>", true).GetTrunc();
      if (TitleStr.IsPrefix("[PS] ")){
        TitleStr=TitleStr.GetSubStr(5, TitleStr.Len()).GetTrunc();}
      TStr AuthNmVPubStr=HtmlLx.GetStrToBTag("<BR>", true);
      TStrV AuthNmV; TStr PubNm; TStr PubYearStr;
      TGgSchRef::GetAuthNmVPubStr(AuthNmVPubStr, AuthNmV, PubNm, PubYearStr);

      TStr CitedByUrlStr; int Citations=0;
      HtmlLx.MoveToBTag3OrEof("<A>", "<P>", "<DIV>");
      if ((HtmlLx.Sym==hsyBTag)&&(HtmlLx.ChA=="<A>")){
        TStr CitedByRelUrlStr=HtmlLx.GetArg("HREF");
        TStr AStr=HtmlLx.GetStrToETag("<A>", true);
        if (AStr.IsPrefix("Cited by ")){
          PUrl CitedByUrl=TUrl::New(CitedByRelUrlStr, UrlStr);
          if (CitedByUrl->IsOk()){
            CitedByUrlStr=CitedByUrl->GetUrlStr();
            Citations=AStr.GetSubStr(TStr("Cited by ").Len(), AStr.Len()).GetInt(0);
          }
        }
        HtmlLx.MoveToBTag2OrEof("<P>", "<DIV>");
      }
      PGgSchRef Ref=
       TGgSchRef::New(TitleStr, AuthNmV, PubNm, PubYearStr, Citations, CitedByUrlStr);
      RSet->AddHit(Ref);
      //printf("%4s - Cit %d - %s (Auth %d)\n",
      // PubYearStr.CStr(), Citations, TitleStr.CStr(), AuthNmV.Len());
    } else {
      break;
    }
    //RSet->AddHit(HitUrlStr, HitTitleStr, HitSrcNm, HitCtxStr);
  }

  // extract footer info
  TStr NextUrlStr;
  if ((HtmlLx.Sym==hsyBTag)&&(HtmlLx.UcChA=="<DIV>")){
    TStr NextRelUrlStr=HtmlLx.GetHRefBeforeStr("Next");
    if (!NextRelUrlStr.Empty()){
      PUrl NextUrl=TUrl::New(NextRelUrlStr, UrlStr);
      if (NextUrl->IsOk()){
        NextUrlStr=NextUrl->GetUrlStr();
      }
    }
  }

  // put components
  RSet->PutUrlStr(UrlStr);
  RSet->PutNextUrlStr(NextUrlStr);
  RSet->PutQueryStr(QueryStr);
  RSet->PutAllHits(AllHitsStr.GetInt(-1));

  // return
  return RSet;
}

PGgSchRSet TGgSchRSet::NewScholar(const PWebPg& WebPg){
  TStr UrlStr=WebPg->GetUrlStr();
  TStr HtmlStr=WebPg->GetHttpBodyAsStr();
  return TGgSchRSet::NewScholar(UrlStr, HtmlStr);
}

void TGgSchRSet::Merge(const PGgSchRSet& RSet){
  if (RSet.Empty()){return;}
  // create hash table of existing urls
  TStrH TitleStrH(GetHits());
  for (int HitN=0; HitN<GetHits(); HitN++){
    TitleStrH.AddKey(GetHit(HitN)->TitleStr);
  }
  // merge hits
  for (int HitN=0; HitN<RSet->GetHits(); HitN++){
    PGgSchRef Ref=RSet->GetHit(HitN);
    if (!TitleStrH.IsKey(Ref->TitleStr)){
      AddHit(Ref);
      TitleStrH.AddKey(Ref->TitleStr);
    }
  }
  // reset fixed fields
  PutNextUrlStr("");
  PutAllHits(-1);
}

PBowDocBs TGgSchRSet::GetBowDocBs() const {
  // prepare stop-words, stemming
  PSwSet SwSet=TSwSet::GetSwSet(swstEn523);
  PStemmer Stemmer=TStemmer::New(stmtPorter, true);
  // prepare n-grams
  TStrV HtmlStrV(GetHits(), 0);
  for (int HitN=0; HitN<GetHits(); HitN++){
    TStr HtmlStr=GetHit(HitN)->TitleStr;
    HtmlStrV.Add(HtmlStr);
  }
  PNGramBs NGramBs=TNGramBs::GetNGramBsFromHtmlStrV(
   HtmlStrV, 3, 3, SwSet, Stemmer);
  // create document-base
  printf("Create Bag-Of-Words Base ... ");
  PBowDocBs BowDocBs=TBowDocBs::New();
  BowDocBs->PutNGramBs(NGramBs);
  for (int HitN=0; HitN<GetHits(); HitN++){
    BowDocBs->AddHtmlDoc(TInt::GetStr(HitN), TStrV(), HtmlStrV[HitN]);
  }
  BowDocBs->AssertOk();
  printf("Done.\n");
  // return bag-of-words
  return BowDocBs;
}

void TGgSchRSet::SaveBin(const TStr& FNm, const PGgSchRSet& GgSchRSet){
  if (GgSchRSet.Empty()){
    PGgSchRSet RSet=TGgSchRSet::New();
    GgSchRSet->SaveBin(FNm);
  } else {
    GgSchRSet->SaveBin(FNm);
  }
}

void TGgSchRSet::SaveXml(const TStr& FNm, const PGgSchRSet& GgSchRSet){
  if (GgSchRSet.Empty()){
    TFOut FOut(FNm); FILE* fOut=FOut.GetFileId();
    fprintf(fOut, "<GgSchRSets>\n");
    fprintf(fOut, "  <Error/>\n");
    fprintf(fOut, "</GgSchRSets>");
  } else {
    GgSchRSet->SaveXml(FNm);
  }
}

void TGgSchRSet::SaveXml(const TStr& FNm){
  TFOut FOut(FNm); FILE* fOut=FOut.GetFileId();
  fprintf(fOut, "<RSet>\n");
  // fixed fields
  if (!GetUrlStr().Empty()){
    fprintf(fOut, "  <Url>%s</Url>\n", TXmlLx::GetXmlStrFromPlainStr(GetUrlStr()).CStr());}
  if (!GetNextUrlStr().Empty()){
    fprintf(fOut, "  <NextUrl>%s</NextUrl>\n", TXmlLx::GetXmlStrFromPlainStr(GetNextUrlStr()).CStr());}
  if (!GetQueryStr().Empty()){
    fprintf(fOut, "  <Query>%s</Query>\n", TXmlLx::GetXmlStrFromPlainStr(GetQueryStr()).CStr());}
  if (GetAllHits()!=-1){
    fprintf(fOut, "  <AllHits>%d</AllHits>\n", GetAllHits());}
  // hits
  fprintf(fOut, "  <Hits Size=\"%d\">\n", GetHits());
  for (int HitN=0; HitN<GetHits(); HitN++){
    PGgSchRef Ref=GetHit(HitN);
    Ref->SaveXml(fOut, 1+HitN);
  }
  fprintf(fOut, "  </Hits>\n");
  fprintf(fOut, "</RSet>");
}

void TGgSchRSet::LoadRSetsBin(const TStr& FNm, TGgSchRSetV& RSetV){
  TFIn SIn(FNm); RSetV.Clr(); int RSets=0;
  while (!SIn.Eof()){
    RSets++; if (RSets%100==0){printf("%d\r", RSets);}
    PGgSchRSet RSet(SIn);
    RSetV.Add(RSet);
  }
}

void TGgSchRSet::SaveRSetsBin(const TStr& FNm, TGgSchRSetV& RSetV){
  TFOut SOut(FNm);
  for (int RSetN=0; RSetN<RSetV.Len(); RSetN++){
    RSetV[RSetN].Save(SOut);
  }
}

/////////////////////////////////////////////////
// Google-Scholar-Base
void TGgSchBs::AddRef(const PGgSchRef& Ref){
  // ger reference md5
  TMd5Sig Sig=Ref->GetMd5Sig();
  // add reference if not exists
  if (!RefMd5ToRefH.IsKey(Sig)){
    // add reference
    RefMd5ToRefH.AddDat(Sig)=Ref;
    RefMd5ToRefCiteCrawlPH.AddDat(Sig)=false;
    // add authors
    for (int AuthN=0; AuthN<Ref->AuthNmV.Len(); AuthN++){
      TStr LcAuthNm=Ref->AuthNmV[AuthN].GetLc();
      if ((!LcAuthNm.Empty())&&(!AuthNmToCrawlPH.IsKey(LcAuthNm))){
        AuthNmToCrawlPH.AddDat(LcAuthNm)=false;
      }
    }
    // add publication
    TStr LcPubNm=Ref->PubNm.GetLc();
    if ((!Ref->PubNm.Empty())&&(!AuthNmToCrawlPH.IsKey(LcPubNm))){
      PubNmToCrawlPH.AddDat(LcPubNm)=false;
    }
  }
}

void TGgSchBs::AddRSet(const PGgSchRSet& RSet){
  // save references
  for (int HitN=0; HitN<RSet->GetHits(); HitN++){
    PGgSchRef Ref=RSet->GetHit(HitN);
    AddRef(Ref);
  }
}

TStr TGgSchBs::GetAuthNmToCrawl() const {
  TStr AuthNm;
  for (int AuthN=0; AuthN<AuthNmToCrawlPH.Len(); AuthN++){
    if (!AuthNmToCrawlPH[AuthN]){
      AuthNm=AuthNmToCrawlPH.GetKey(AuthN); break;
    }
  }
  return AuthNm;
}

void TGgSchBs::SetAuthCrawled(const TStr& AuthNm){
  AuthNmToCrawlPH.AddDat(AuthNm.GetLc())=true;
}

void TGgSchBs::SaveXml(const TStr& FNm){
  TFOut FOut(FNm); FILE* fOut=FOut.GetFileId();
  fprintf(fOut, "<GgSchBs>\n");
  for (int RefN=0; RefN<GetRefs(); RefN++){
    PGgSchRef Ref=GetRef(RefN);
    Ref->SaveXml(fOut, 1+RefN);
  }
  fprintf(fOut, "</GgSchBs>");
}

/////////////////////////////////////////////////
// Google-Result-Set
PRSet TRSet::NewWeb(const TStr& UrlStr, const TStr& HtmlStr){
  // prepare object
  PRSet RSet=TRSet::New();

  // prepare html browsing
  PSIn HtmlSIn=TStrIn::New(HtmlStr);
  THtmlLx HtmlLx(HtmlSIn);

  // extract header info
  HtmlLx.MoveToStrOrEof("Results");
  TStr FromResultStr=HtmlLx.GetStrInTag("<B>", true);
  TStr ToResultStr=HtmlLx.GetStrInTag("<B>", true);
  TStr AllHitsStr=HtmlLx.GetStrInTag("<B>", true);
  AllHitsStr.DelChAll(',');
  TStr QueryStr=HtmlLx.GetStrInTag("<B>", true);

  // traverse hits
  forever {
    HtmlLx.MoveToBTagOrEof("<P>", "CLASS", "g", "<DIV>", "CLASS", "n");
    if (!((HtmlLx.Sym==hsyBTag)&&(HtmlLx.UcChA=="<P>"))){break;}
    HtmlLx.MoveToBTagOrEof("<A>");
    if (HtmlLx.Sym!=hsyBTag){break;}
    TStr HitUrlStr=HtmlLx.GetArg("HREF");
    TStr HitTitleStr=HtmlLx.GetStrToETag("<A>", true);
    HtmlLx.MoveToBTagOrEof("<FONT>");
    TStr HitCtxStr=HtmlLx.GetStrToBTag("<FONT>", "COLOR", "#008000", true);
    RSet->AddHit(HitUrlStr, HitTitleStr, "", HitCtxStr);
  }

  // extract footer info
  TStr NextUrlStr;
  if ((HtmlLx.Sym==hsyBTag)&&(HtmlLx.UcChA=="<DIV>")){
    TStr NextRelUrlStr=HtmlLx.GetHRefBeforeStr("Next");
    if (!NextRelUrlStr.Empty()){
      PUrl NextUrl=TUrl::New(NextRelUrlStr, UrlStr);
      if (NextUrl->IsOk()){
        NextUrlStr=NextUrl->GetUrlStr();
      }
    }
  }

  // put components
  RSet->PutUrlStr(UrlStr);
  RSet->PutNextUrlStr(NextUrlStr);
  RSet->PutQueryStr(QueryStr);
  RSet->PutAllHits(AllHitsStr.GetInt(-1));

  // return
  return RSet;
}

PRSet TRSet::NewWeb(const PWebPg& WebPg){
  TStr UrlStr=WebPg->GetUrlStr();
  TStr HtmlStr=WebPg->GetHttpBodyAsStr();
  return TRSet::NewWeb(UrlStr, HtmlStr);
}

PRSet TRSet::NewNews(const TStr& UrlStr, const TStr& HtmlStr){
  // prepare object
  PRSet RSet=TRSet::New();

  // prepare html browsing
  PSIn HtmlSIn=TStrIn::New(HtmlStr);
  THtmlLx HtmlLx(HtmlSIn);

  // extract header info
  HtmlLx.MoveToStrOrEof("Results");
  TStr FromResultStr=HtmlLx.GetStrInTag("<B>", true);
  TStr ToResultStr=HtmlLx.GetStrInTag("<B>", true);
  TStr AllHitsStr=HtmlLx.GetStrInTag("<B>", true);
  AllHitsStr.DelChAll(',');
  TStr QueryStr=HtmlLx.GetStrInTag("<B>", true);

  // traverse hits
  forever {
    HtmlLx.MoveToBTagOrEof("<TABLE>", "WIDTH", "75%", "<DIV>", "CLASS", "n");
    if (!((HtmlLx.Sym==hsyBTag)&&(HtmlLx.UcChA=="<TABLE>"))){break;}
    HtmlLx.MoveToBTagOrEof("<A>");
    if (HtmlLx.Sym!=hsyBTag){break;}
    TStr HitUrlStr=HtmlLx.GetArg("HREF");
    TStr IdStr=HtmlLx.GetArg("ID");
    // if image
    if ((!IdStr.Empty())&&(IdStr.LastCh()=='i')){
      HtmlLx.MoveToBTagOrEof("<A>");
      if (HtmlLx.Sym!=hsyBTag){break;}
      HitUrlStr=HtmlLx.GetArg("HREF");
    }
    TStr HitTitleStr=HtmlLx.GetStrToETag("<A>", true);
    TStr HitSrcNm=HtmlLx.GetStrToBTag("<NOBR>", true);
    if (HitSrcNm.IsSuffix(" -")){
      HitSrcNm=HitSrcNm.GetSubStr(0, HitSrcNm.Len()-3);}
    HtmlLx.MoveToETagOrEof("<NOBR>");
    TStr HitCtxStr=HtmlLx.GetStrToETag("<TABLE>", true);
    RSet->AddHit(HitUrlStr, HitTitleStr, HitSrcNm, HitCtxStr);
  }

  // extract footer info
  TStr NextUrlStr;
  if ((HtmlLx.Sym==hsyBTag)&&(HtmlLx.UcChA=="<DIV>")){
    TStr NextRelUrlStr=HtmlLx.GetHRefBeforeStr("Next");
    if (!NextRelUrlStr.Empty()){
      PUrl NextUrl=TUrl::New(NextRelUrlStr, UrlStr);
      if (NextUrl->IsOk()){
        NextUrlStr=NextUrl->GetUrlStr();
      }
    }
  }

  // put components
  RSet->PutUrlStr(UrlStr);
  RSet->PutNextUrlStr(NextUrlStr);
  RSet->PutQueryStr(QueryStr);
  RSet->PutAllHits(AllHitsStr.GetInt(-1));

  // return
  return RSet;
}

PRSet TRSet::NewNews(const PWebPg& WebPg){
  TStr UrlStr=WebPg->GetUrlStr();
  TStr HtmlStr=WebPg->GetHttpBodyAsStr();
  return TRSet::NewNews(UrlStr, HtmlStr);
}

int TRSet::GetHitN(const TStr& UrlStr, const bool& LcP) const {
  int Hits=GetHits();
  TStr LcUrlStr=UrlStr.GetLc();
  for (int HitN=0; HitN<Hits; HitN++){
    if (LcP){
      if (GetHitUrlStr(HitN).GetLc()==LcUrlStr){return HitN;}
    } else {
      if (GetHitUrlStr(HitN)==UrlStr){return HitN;}
    }
  }
  return -1;
}

void TRSet::Merge(const PRSet& RSet){
  if (RSet.Empty()){return;}
  // create hash table of existing urls
  TStrH UrlStrH(GetHits());
  for (int HitN=0; HitN<GetHits(); HitN++){
    UrlStrH.AddKey(GetHitUrlStr(HitN));
  }
  // merge hits
  for (int HitN=0; HitN<RSet->GetHits(); HitN++){
    TStr HitUrlStr; TStr HitTitleStr; TStr HitSrcNm; TStr HitCtxStr;
    RSet->GetHit(HitN, HitUrlStr, HitTitleStr, HitSrcNm, HitCtxStr);
    if (!UrlStrH.IsKey(HitUrlStr)){
      AddHit(HitUrlStr, HitTitleStr, HitSrcNm, HitCtxStr);
      UrlStrH.AddKey(HitUrlStr);
    }
  }
  // reset fixed fields
  PutNextUrlStr("");
  PutAllHits(-1);
}

PBowDocBs TRSet::GetBowDocBs() const {
  // prepare stop-words, stemming
  PSwSet SwSet=TSwSet::GetSwSet(swstEn523);
  PStemmer Stemmer=TStemmer::New(stmtPorter, true);
  // prepare n-grams
  TStrV HtmlStrV(GetHits(), 0);
  for (int HitN=0; HitN<GetHits(); HitN++){
    TStr TitleStr=GetHitTitleStr(HitN);
    TStr CtxStr=GetHitCtxStr(HitN);
    TStr HtmlStr=TitleStr+". "+CtxStr;
    HtmlStrV.Add(HtmlStr);
  }
  PNGramBs NGramBs=TNGramBs::GetNGramBsFromHtmlStrV(
   HtmlStrV, 3, 3, SwSet, Stemmer);
  // create document-base
  printf("Create Bag-Of-Words Base ... ");
  PBowDocBs BowDocBs=TBowDocBs::New();
  BowDocBs->PutNGramBs(NGramBs);
  for (int HitN=0; HitN<GetHits(); HitN++){
    BowDocBs->AddHtmlDoc(GetHitUrlStr(HitN), TStrV(), HtmlStrV[HitN]);
  }
  BowDocBs->AssertOk();
  printf("Done.\n");
  // return bag-of-words
  return BowDocBs;
}

void TRSet::SaveBin(const TStr& FNm, const PRSet& RSet){
  if (RSet.Empty()){
    PRSet RSet=TRSet::New();
    RSet->SaveBin(FNm);
  } else {
    RSet->SaveBin(FNm);
  }
}

void TRSet::SaveXml(const TStr& FNm, const PRSet& RSet){
  if (RSet.Empty()){
    TFOut FOut(FNm); FILE* fOut=FOut.GetFileId();
    fprintf(fOut, "<RSets>\n");
    fprintf(fOut, "  <Error/>\n");
    fprintf(fOut, "</RSets>");
  } else {
    RSet->SaveXml(FNm);
  }
}

void TRSet::SaveXml(const TStr& FNm){
  TFOut FOut(FNm); FILE* fOut=FOut.GetFileId();
  fprintf(fOut, "<RSet>\n");
  // fixed fields
  if (!GetUrlStr().Empty()){
    fprintf(fOut, "  <Url>%s</Url>\n", TXmlLx::GetXmlStrFromPlainStr(GetUrlStr()).CStr());}
  if (!GetNextUrlStr().Empty()){
    fprintf(fOut, "  <NextUrl>%s</NextUrl>\n", TXmlLx::GetXmlStrFromPlainStr(GetNextUrlStr()).CStr());}
  if (!GetQueryStr().Empty()){
    fprintf(fOut, "  <Query>%s</Query>\n", TXmlLx::GetXmlStrFromPlainStr(GetQueryStr()).CStr());}
  if (GetAllHits()!=-1){
    fprintf(fOut, "  <AllHits>%d</AllHits>\n", GetAllHits());}
  // hits
  fprintf(fOut, "  <Hits Size=\"%d\">\n", GetHits());
  for (int HitN=0; HitN<GetHits(); HitN++){
    TStr HitUrlStr; TStr HitTitleStr; TStr HitSrcNm; TStr HitCtxStr;
    GetHit(HitN, HitUrlStr, HitTitleStr, HitSrcNm, HitCtxStr);
    fprintf(fOut, "    <Hit Num=\"%d\">\n", 1+HitN);
    fprintf(fOut, "      <Url>%s</Url>\n", TXmlLx::GetXmlStrFromPlainStr(HitUrlStr).CStr());
    fprintf(fOut, "      <Title>%s</Title>\n", TXmlLx::GetXmlStrFromPlainStr(HitTitleStr).CStr());
    if (!HitSrcNm.Empty()){
      fprintf(fOut, "      <Source>%s</Source>\n", TXmlLx::GetXmlStrFromPlainStr(HitSrcNm).CStr());}
    fprintf(fOut, "      <Snippet>%s</Snippet>\n", TXmlLx::GetXmlStrFromPlainStr(HitCtxStr).CStr());
    fprintf(fOut, "    </Hit>\n");
  }
  fprintf(fOut, "  </Hits>\n");
  fprintf(fOut, "</RSet>");
}

void TRSet::LoadRSetsBin(const TStr& FNm, TRSetV& RSetV){
  TFIn SIn(FNm); RSetV.Clr(); int RSets=0;
  while (!SIn.Eof()){
    RSets++; if (RSets%100==0){printf("%d\r", RSets);}
    PRSet RSet(SIn);
    RSetV.Add(RSet);
  }
}

void TRSet::SaveRSetsBin(const TStr& FNm, TRSetV& RSetV){
  TFOut SOut(FNm);
  for (int RSetN=0; RSetN<RSetV.Len(); RSetN++){
    RSetV[RSetN].Save(SOut);
  }
}

/////////////////////////////////////////////////
// Google-Web-Fetch-Saver
void TGgWebFetchSaver::OnFetch(const int&, const PWebPg& WebPg){
  printf("Fetched [Wait:%d Conn.:%d]: %s\n",
   GetWaitUrls(), GetConnUrls(), WebPg->GetUrlStr().CStr());
  WebPgV.Add(WebPg);
  if (Empty()){
    TSysMsg::Quit();}
}

void TGgWebFetchSaver::OnError(const int&, const TStr& MsgStr){
  printf("Error [Wait:%d Conn.:%d]: %s\n",
   GetWaitUrls(), GetConnUrls(), MsgStr.CStr());
  if (Empty()){
    TSysMsg::Quit();}
}

PWebPg TGgWebFetchSaver::GetWebPg(const TStr& UrlStr) const {
  for (int WebPgN=0; WebPgN<GetWebPgs(); WebPgN++){
    if (GetWebPg(WebPgN)->GetUrlStr(0)==UrlStr){
      return GetWebPg(WebPgN);}
  }
  return NULL;
}

/////////////////////////////////////////////////
// Google-Focused-Crawl
PGgFCrawl TGgFCrawl::GetFCrawl(
 const TStr& SrcUrlStr, const int& MxCands, const TStr& ProxyStr){
  // collect related urls
  printf("Expand source URL: %s\n", SrcUrlStr.CStr());
  PRSet SrcUrlRSet=
   TGg::WebSearch(TStr("related:")+SrcUrlStr, -1, TNotify::NullNotify, ProxyStr);
  // create & prepare focused-crawl
  PGgFCrawl FCrawl=TGgFCrawl::New();
  FCrawl->SrcUrlStr=SrcUrlStr;
  FCrawl->DstRSet=TRSet::New(SrcUrlRSet);
  // fill hits
  for (int HitN=0; HitN<SrcUrlRSet->GetHits(); HitN++){
    if ((MxCands!=-1)&&(FCrawl->DstRSet->GetHits()>MxCands)){break;}
    TStr HitUrlStr=SrcUrlRSet->GetHitUrlStr(HitN);
    printf("Expand URL: %s\n", HitUrlStr.CStr());
    PRSet RelUrlRSet=
     TGg::WebSearch(TStr("related:")+HitUrlStr, -1, TNotify::NullNotify, ProxyStr);
    FCrawl->DstRSet->Merge(RelUrlRSet);
  }
  // save related urls
  //TRSet::SaveXml(DstRSet, OutXmlUrlFNm);

  // collect related web-pages
  TGgWebFetchSaver WebFetchSaver(100);
  WebFetchSaver.PutProxyStr(ProxyStr);
  // get source-url web-page
  {bool Ok; TStr MsgStr;
  TWebFetchBlocking::GetWebPg(
   SrcUrlStr, Ok, MsgStr, FCrawl->SrcWebPg, NULL, ProxyStr);
  if (!Ok){FCrawl->SrcWebPg=NULL;}}
  // get related-urls web-page
  int FetchHits=FCrawl->DstRSet->GetHits();
  if ((MxCands!=-1)&&(MxCands<FetchHits)){FetchHits=MxCands;}
  for (int HitN=0; HitN<FetchHits; HitN++){
    TStr HitUrlStr=FCrawl->DstRSet->GetHitUrlStr(HitN);
    WebFetchSaver.FetchUrl(HitUrlStr);
  }
  TSysMsg::Loop();

  // save crawled web-pages
  for (int WebPgN=0; WebPgN<WebFetchSaver.GetWebPgs(); WebPgN++){
    PWebPg WebPg=WebFetchSaver.GetWebPg(WebPgN);
    FCrawl->UrlStrToWebPgH.AddDat(WebPg->GetUrlStr(), WebPg);
  }

  // create bag-of-words
  FCrawl->BowDocBs=TBowDocBs::New();
  FCrawl->SrcDId=FCrawl->BowDocBs->AddHtmlDoc(
   SrcUrlStr, TStrV(), FCrawl->SrcWebPg->GetHttpBodyAsStr());
  for (int WebPgN=0; WebPgN<WebFetchSaver.GetWebPgs(); WebPgN++){
    PWebPg WebPg=WebFetchSaver.GetWebPg(WebPgN);
    FCrawl->BowDocBs->AddHtmlDoc(
     WebPg->GetUrlStr(0), TStrV(), WebPg->GetHttpBodyAsStr());
  }

  // calculate similarities to the source document
  PBowDocWgtBs BowDocWgtBs=TBowDocWgtBs::New(FCrawl->BowDocBs, bwwtNrmTFIDF);
  PBowSim BowSim=TBowSim::New(bstCos);
  FCrawl->SimDIdKdV; FCrawl->SumSim=0;
  for (int DIdN=0; DIdN<BowDocWgtBs->GetDocs(); DIdN++){
    int DId=BowDocWgtBs->GetDId(DIdN);
    if (DId!=FCrawl->SrcDId){
      double Sim=BowSim->GetSim(
       BowDocWgtBs->GetSpV(FCrawl->SrcDId), BowDocWgtBs->GetSpV(DId));
      FCrawl->SimDIdKdV.Add(TFltIntKd(Sim, DId));
      FCrawl->SumSim+=Sim;
    }
  }
  FCrawl->SimDIdKdV.Sort(false);
  // set crawl ok
  FCrawl->Ok=true;
  // return focused-crawl
  return FCrawl;
}

void TGgFCrawl::SaveXml(const TStr& FNm, const bool& SaveDocP){
  if (!Ok){return;}
  PSOut SOut=TFOut::New(FNm);
  FILE* fOut=SOut->GetFileId();
  fprintf(fOut, "<FocusedCrawl>\n");
  fprintf(fOut, "  <SourceWebPage>\n");
  fprintf(fOut, "    <Url>%s</Url>\n", SrcUrlStr.CStr());
  if (SaveDocP){
    THtmlDoc::SaveHtmlToXml(SrcWebPg->GetHttpBodyAsStr(), SOut,
     SrcUrlStr, false, false, true, false, false);
  }
  fprintf(fOut, "  </SourceWebPage>\n");
  fprintf(fOut, "  <FocusedWebPages>\n", SimDIdKdV.Len());
  double SumSimSF=0;
  for (int DIdN=0; DIdN<SimDIdKdV.Len(); DIdN++){
    double Sim=SimDIdKdV[DIdN].Key;
    SumSimSF+=Sim; if ((SumSim==0)||(SumSimSF>SumSim*0.99)){break;}
    int DId=SimDIdKdV[DIdN].Dat;
    TStr UrlStr=BowDocBs->GetDocNm(DId);
    printf("%d. %.3f %s\n", 1+DIdN, Sim, UrlStr.CStr());
    int HitN=DstRSet->GetHitN(UrlStr); IAssert(HitN!=-1);
    fprintf(fOut, "    <WebPage Rank=\"%d\" Sim=\"%.3f\">\n", 1+DIdN, Sim, UrlStr.CStr());
    fprintf(fOut, "      <Url>%s</Url>\n", UrlStr.CStr());
    fprintf(fOut, "      <Title>%s</Title>\n", TXmlLx::GetXmlStrFromPlainStr(DstRSet->GetHitTitleStr(HitN)).CStr());
    fprintf(fOut, "      <Context>%s</Context>\n", TXmlLx::GetXmlStrFromPlainStr(DstRSet->GetHitCtxStr(HitN)).CStr());
    if (SaveDocP){
      //flush(fOut);
      PWebPg WebPg=UrlStrToWebPgH.GetDat(UrlStr);
      THtmlDoc::SaveHtmlToXml(WebPg->GetHttpBodyAsStr(), SOut,
       SrcUrlStr, false, false, true, false, false);
      //SOut->Flush();
    }
    fprintf(fOut, "    </WebPage>\n");
  }
  printf("\n");
  fprintf(fOut, "  </FocusedWebPages>\n");
  fprintf(fOut, "</FocusedCrawl>\n");
}

void TGgFCrawl::SaveTxt(const TStr& FNm){
  PSOut SOut=TFOut::New(FNm);
  FILE* fOut=SOut->GetFileId();
  if (Ok){
    fprintf(fOut, "<SourceWebPage>\n");
    fprintf(fOut, "%s\n", SrcUrlStr.CStr());
    double SumSimSF=0;
    for (int DIdN=0; DIdN<SimDIdKdV.Len(); DIdN++){
      double Sim=SimDIdKdV[DIdN].Key;
      SumSimSF+=Sim; if ((SumSim==0)||(SumSimSF>SumSim*0.99)){break;}
      int DId=SimDIdKdV[DIdN].Dat;
      TStr UrlStr=BowDocBs->GetDocNm(DId);
      printf("%d. %.3f %s\n", 1+DIdN, Sim, UrlStr.CStr());
      int HitN=DstRSet->GetHitN(UrlStr); IAssert(HitN!=-1);
      TStr TitleStr=DstRSet->GetHitTitleStr(HitN);
      TitleStr.ChangeChAll('\n', ' ');
      TStr CtxStr=DstRSet->GetHitCtxStr(HitN);
      CtxStr.ChangeChAll('\n', ' ');
      fprintf(fOut, "<ResultWebPage>\n");
      fprintf(fOut, "%s\n", UrlStr.CStr());
      fprintf(fOut, "%s\n", TitleStr.CStr());
      fprintf(fOut, "%s\n", CtxStr.CStr());
    }
  } else {
    fprintf(fOut, "<Error>\n");
  }
  fprintf(fOut, "<End>\n");
}

/////////////////////////////////////////////////
// Google-Web-Context-Graph
PGgCtxGraph TGgCtxGraph::GetCtxGraph(const TStr& FocusUrlStr){
  // create context-graph
  PGgCtxGraph CtxGraph=TGgCtxGraph::New();
  CtxGraph->Ok=false;
  // get focus-web-page
  CtxGraph->FocusUrlStr=FocusUrlStr;
  {bool Ok; TStr MsgStr;
  TWebFetchBlocking::GetWebPg(
   CtxGraph->FocusUrlStr, Ok, MsgStr, CtxGraph->FocusWebPg, TNotify::StdNotify);
  if (!Ok){return CtxGraph;}}
  // get 'In' set of links
  PRSet InRSet=
   TGg::WebSearch(TStr("link:")+CtxGraph->FocusUrlStr, -1, TNotify::StdNotify);
  for (int HitN=0; HitN<InRSet->GetHits(); HitN++){
    TStr UrlStr; TStr TitleStr; TStr SrcNm; TStr CtxStr;
    InRSet->GetHit(HitN, UrlStr, TitleStr, SrcNm, CtxStr);
    TitleStr.ChangeChAll('\n', ' ');
    CtxGraph->InUrlCtxStrPrV.Add(TStrPr(UrlStr, TitleStr));
  }
  // get Out set
  TStrKdV OutDescUrlStrKdV;
  CtxGraph->FocusWebPg->GetOutDescUrlStrKdV(OutDescUrlStrKdV);
  for (int UrlN=0; UrlN<OutDescUrlStrKdV.Len(); UrlN++){
    OutDescUrlStrKdV[UrlN].Key.ChangeChAll('\n', ' ');
    CtxGraph->OutUrlCtxStrPrV.Add(
     TStrPr(OutDescUrlStrKdV[UrlN].Dat, OutDescUrlStrKdV[UrlN].Key));
  }
  // set context-graph successful
  CtxGraph->Ok=true;
  // return context-graph
  return CtxGraph;
}

void TGgCtxGraph::SaveTxt(const TStr& FNm){
  PSOut SOut=TFOut::New(FNm);
  FILE* fOut=SOut->GetFileId();
  if (Ok){
    fprintf(fOut, "<FocusWebPage>\n");
    fprintf(fOut, "%s\n", FocusUrlStr.CStr());
    for (int UrlN=0; UrlN<InUrlCtxStrPrV.Len(); UrlN++){
      fprintf(fOut, "<InWebPage>\n");
      fprintf(fOut, "%s\n", InUrlCtxStrPrV[UrlN].Val1.CStr());
      fprintf(fOut, "%s\n", InUrlCtxStrPrV[UrlN].Val2.CStr());
    }
    for (int UrlN=0; UrlN<OutUrlCtxStrPrV.Len(); UrlN++){
      fprintf(fOut, "<OutWebPage>\n");
      fprintf(fOut, "%s\n", OutUrlCtxStrPrV[UrlN].Val1.CStr());
      fprintf(fOut, "%s\n", OutUrlCtxStrPrV[UrlN].Val2.CStr());
    }
  } else {
    fprintf(fOut, "<Error>\n");
  }
  fprintf(fOut, "<End>\n");
}

/////////////////////////////////////////////////
// Google
TStr TGg::GetWebSearchUrlStr(const TStr& QueryStr){
  TStr SearchUrlStr=
   "http://www.google.com/search?num=100&q="+
   TUrl::GetUrlSearchStr(QueryStr);
  return SearchUrlStr;
}

TStr TGg::GetNewsSearchUrlStr(const TStr& QueryStr){
  TStr SearchUrlStr=
//   "http://news.google.com/news?num=100&scoring=d&q="+
   "http://news.google.com/news?num=100&q="+
   TUrl::GetUrlSearchStr(QueryStr);
  return SearchUrlStr;
}

TStr TGg::GetScholarSearchUrlStr(const TStr& QueryStr){
  TStr SearchUrlStr=
   "http://scholar.google.com/scholar?num=100&hl=en&lr=&q="+
   TUrl::GetUrlSearchStr(QueryStr);
  return SearchUrlStr;
}

TStr TGg::GetScholarAuthorSearchUrlStr(const TStr& QueryStr){
  TStr AuthorQueryStr=TStr("author:\"")+QueryStr+"\"";
  TStr SearchUrlStr=
   "http://scholar.google.com/scholar?num=100&hl=en&lr=&q="+
   TUrl::GetUrlSearchStr(AuthorQueryStr);
  return SearchUrlStr;
}

TStr TGg::GetScholarPublicationSearchUrlStr(const TStr& QueryStr){
  TStr PublicationQueryStr=TStr("\"")+QueryStr+"\"";
  TStr SearchUrlStr=
   "http://scholar.google.com/scholar?num=100&hl=en&lr=&q=&as_publication="+
   TUrl::GetUrlSearchStr(PublicationQueryStr);
  return SearchUrlStr;
}

PRSet TGg::WebSearch(const TStr& QueryStr, const int& MxHits,
 const PNotify& Notify, const TStr& ProxyStr){
  // prepare search url
  TStr SearchUrlStr=TGg::GetWebSearchUrlStr(QueryStr);
  // fetch page from google
  bool Ok; TStr MsgStr; PWebPg WebPg;
  TWebFetchBlocking::GetWebPg(
   SearchUrlStr, Ok, MsgStr, WebPg, Notify, ProxyStr);
  // postprocess results
  if (Ok){
    // process search results
    PRSet RSet=TRSet::NewWeb(WebPg);
    // fetch next result pages
    TStr NextSearchUrlStr=RSet->GetNextUrlStr();
    while (!NextSearchUrlStr.Empty()){
      if ((MxHits!=-1)&&(RSet->GetHits()>=MxHits)){break;}
      // fetch page from google
      bool NextOk; TStr NextMsgStr; PWebPg NextWebPg;
      TWebFetchBlocking::GetWebPg(
       NextSearchUrlStr, NextOk, NextMsgStr, NextWebPg, Notify, ProxyStr);
      if (NextOk){
        // process search results
        PRSet NextRSet=TRSet::NewWeb(NextWebPg);
        // merge next-result-set into final result-set
        RSet->Merge(NextRSet);
        // prepare next-url
        NextSearchUrlStr=NextRSet->GetNextUrlStr();
      } else {
        NextSearchUrlStr="";
      }
    }
    RSet->Trunc(MxHits);
    RSet->PutQueryStr(QueryStr);
    // return results
    return RSet;
  } else {
    // return empty results
    return TRSet::New();
  }
}

PRSet TGg::WebSearchExternal(
 const TStr& QueryStr, const int& MxHits, const PNotify& Notify, const TStr& ProxyStr){
  TStr ExeFPath=Env.GetExeFPath();
  TStr ExeFNm=TStr::GetNrFPath(ExeFPath)+"Google2RSet.exe";
  TStr RSetFNm=TStr::GetNrFPath(ExeFPath)+TTm::GetCurUniTm().GetIdStr()+".RSet";
  TStr CmLn=
   TStr::GetStr(QueryStr, " -iwq:%s")+
   TInt::GetStr(MxHits, " -hits:%d")+
   TStr::GetStr(RSetFNm, " -obin:\"%s\" -oxml: -obow: -ssilent")+
   TStr::GetStr(ProxyStr, " -proxy:%s");
  if (TSysProc::ExeProc(ExeFNm, CmLn)){
    PRSet RSet;
    {TFIn RSetFIn(RSetFNm);
    RSet=TRSet::Load(RSetFIn);}
    TFile::Del(RSetFNm);
    return RSet;
  } else {
    return TRSet::New();
  }

  /*TStr ExeStr=ExeFNm+CmLn;
  int ErrCd=system(ExeStr.CStr());
  if (ErrCd==-1){
    return TRSet::New();
  } else {
    PRSet RSet;
    {TFIn RSetFIn(RSetFNm);
    RSet=TRSet::Load(RSetFIn);}
    TFile::Del(RSetFNm);
    return RSet;
  }*/
}

#ifdef MIHA_BLAZ_DOTNET_PROXY
PRSet TGg::WebSearchProxy(const TStr& QueryStr, const int& MxHits,
 const PNotify& Notify, const TStr& ProxyStr){
  // prepare search url
  TStr SearchUrlStr=TGg::GetWebSearchUrlStr(QueryStr);
  // fetch page from google
  //printf("Proxy: '%s'\n", ProxyStr.CStr());
  char* WebPgHtmlCStr = fetchWebPage(SearchUrlStr.CStr(), ProxyStr.CStr());
  TStr WebPgHtmlStr = TStr(WebPgHtmlCStr); GlobalFree(WebPgHtmlCStr);
  bool Ok = (WebPgHtmlCStr != NULL);
  // postprocess results
  if (Ok){
    // process search results
    PRSet RSet=TRSet::NewWeb(SearchUrlStr, WebPgHtmlStr);
    // fetch next result pages
    TStr NextSearchUrlStr=RSet->GetNextUrlStr();
    while (!NextSearchUrlStr.Empty()){
      if ((MxHits!=-1)&&(RSet->GetHits()>=MxHits)){break;}
      // fetch page from google
      char* NextWebPgHtmlCStr = fetchWebPage(NextSearchUrlStr.CStr(), ProxyStr.CStr());
      TStr NextWebPgHtmlStr = TStr(NextWebPgHtmlCStr); GlobalFree(NextWebPgHtmlCStr);
      bool NextOk = (NextWebPgHtmlCStr != NULL);
      if (NextOk){
        // process search results
        PRSet NextRSet=TRSet::NewWeb(NextSearchUrlStr, NextWebPgHtmlStr);
        // merge next-result-set into final result-set
        RSet->Merge(NextRSet);
        // prepare next-url
        NextSearchUrlStr=NextRSet->GetNextUrlStr();
      } else {
        NextSearchUrlStr="";
      }
    }
    RSet->Trunc(MxHits);
    RSet->PutQueryStr(QueryStr);
    // return results
    return RSet;
  } else {
    // return empty results
    return TRSet::New();
  }
}
#endif

PRSet TGg::NewsSearch(const TStr& QueryStr, const PNotify& Notify){
  // prepare search url
  TStr SearchUrlStr=TGg::GetNewsSearchUrlStr(QueryStr);
  // fetch page from google
  bool Ok; TStr MsgStr; PWebPg WebPg;
  TWebFetchBlocking::GetWebPg(
   SearchUrlStr, Ok, MsgStr, WebPg, Notify);
  // postprocess results
  if (Ok){
    // process search results
    PRSet RSet=TRSet::NewNews(WebPg);
    // fetch next result pages
    TStr NextSearchUrlStr=RSet->GetNextUrlStr();
    while (!NextSearchUrlStr.Empty()){
      // fetch page from google
      bool NextOk; TStr NextMsgStr; PWebPg NextWebPg;
      TWebFetchBlocking::GetWebPg(
       NextSearchUrlStr, NextOk, NextMsgStr, NextWebPg, Notify);
      if (NextOk){
        // process search results
        PRSet NextRSet=TRSet::NewNews(NextWebPg);
        // merge next-result-set into final result-set
        RSet->Merge(NextRSet);
        // prepare next-url
        NextSearchUrlStr=NextRSet->GetNextUrlStr();
      } else {
        NextSearchUrlStr="";
      }
    }
    // return results
    return RSet;
  } else {
    // return empty results
    return TRSet::New();
  }
}

PGgSchRSet TGg::_ScholarSearch(
 const TStr& SearchUrlStr, const PNotify& Notify){
  // fetch page from google
  bool Ok; TStr MsgStr; PWebPg WebPg;
  TWebFetchBlocking::GetWebPg(
   SearchUrlStr, Ok, MsgStr, WebPg, Notify);
  // postprocess results
  if (Ok){
    // process search results
    PGgSchRSet GgSchRSet=TGgSchRSet::NewScholar(WebPg);
    // fetch next result pages
    TStr NextSearchUrlStr=GgSchRSet->GetNextUrlStr();
    TRnd Rnd(0);
    while (!NextSearchUrlStr.Empty()){
      int WaitMSecs=int(1*1000*(Rnd.GetUniDev()+0.5));
      TSysProc::Sleep(WaitMSecs);
      // fetch page from google
      bool NextOk; TStr NextMsgStr; PWebPg NextWebPg;
      TWebFetchBlocking::GetWebPg(
       NextSearchUrlStr, NextOk, NextMsgStr, NextWebPg, Notify);
      if (NextOk){
        // process search results
        PGgSchRSet NextRSet=TGgSchRSet::NewScholar(NextWebPg);
        // merge next-result-set into final result-set
        GgSchRSet->Merge(NextRSet);
        // prepare next-url
        NextSearchUrlStr=NextRSet->GetNextUrlStr();
      } else {
        NextSearchUrlStr="";
      }
    }
    // return results
    return GgSchRSet;
  } else {
    // return empty results
    return TGgSchRSet::New();
  }
}

PGgSchRSet TGg::ScholarSearch(const TStr& QueryStr, const PNotify& Notify){
  TStr SearchUrlStr=GetScholarSearchUrlStr(QueryStr);
  return _ScholarSearch(SearchUrlStr, Notify);
}

PGgSchRSet TGg::ScholarAuthorSearch(const TStr& AuthorNm, const PNotify& Notify){
  TStr SearchUrlStr=GetScholarAuthorSearchUrlStr(AuthorNm);
  return _ScholarSearch(SearchUrlStr, Notify);
}

PGgSchRSet TGg::ScholarPublicationSearch(const TStr& PublicationNm, const PNotify& Notify){
  TStr SearchUrlStr=GetScholarPublicationSearchUrlStr(PublicationNm);
  return _ScholarSearch(SearchUrlStr, Notify);
}


