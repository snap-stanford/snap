/////////////////////////////////////////////////
// Includes
#include "skygrid.h"

/////////////////////////////////////////////////
// SkyGrid-Document
void TSkyGridBinDoc::SaveBinDocV(
 const TStr& InXmlFPath, const TStr& OutBinFNm, const int& MxDocs){
  printf("Processing SkyGrid-News-Xml files from '%s'...\n", InXmlFPath.CStr());
  TFOut SOut(OutBinFNm);
  TFFile FFile(InXmlFPath, true); TStr FNm;
  int Docs=0; int DateDocs=0; uint64 PrevTm=0;
  while (FFile.Next(FNm)){
    if ((MxDocs!=-1)&&(Docs>=MxDocs)){break;}
    //printf("  Processing '%s' ...", FNm.CStr());
    PXmlDoc XmlDoc=TXmlDoc::LoadTxt(FNm);
    PXmlTok ContentTok=XmlDoc->GetTagTok("item|content");
    TStr SwIdStr=ContentTok->GetTagTok("swid")->GetArgVal("value");
    TStr UrlStr=ContentTok->GetTagTok("url")->GetTokStr(false);
    TStr TitleStr=ContentTok->GetTagTok("title")->GetTokStr(false);
    TStr FetchedValStr=ContentTok->GetTagTok("fetched")->GetArgVal("value");
    TXmlTokV EntityTokV; ContentTok->GetTagTokV("annotations|entity", EntityTokV);
    TStr BodyStr=ContentTok->GetTagTok("body")->GetTokStr(false);
    // extract date
    TStr DateStr=SwIdStr.GetSubStr(0, 7);
    TStr YearStr=DateStr.GetSubStr(0, 3);
    TStr MonthStr=DateStr.GetSubStr(4, 5);
    TStr DayStr=DateStr.GetSubStr(6, 7);
    TTm DateTm(YearStr.GetInt(), MonthStr.GetInt(), DayStr.GetInt());
    uint64 Tm=TTm::GetMSecsFromTm(DateTm);
    // extract entities
    TStrIntH EntNmToFqH;
    for (int EntityTokN=0; EntityTokN<EntityTokV.Len(); EntityTokN++){
      PXmlTok EntityTok=EntityTokV[EntityTokN];
      if (!EntityTok->IsTag("entity")){continue;}
      TStr CanonicalNm=EntityTok->GetArgVal("canonical", "");
      TStr TextStr=EntityTok->GetArgVal("text", "");
      TStr TypeNm=EntityTok->GetArgVal("type", "");
      TStr EntNm=CanonicalNm.Empty() ? TextStr : CanonicalNm;
      EntNmToFqH.AddDat(EntNm)++;
    }
    TIntStrPrV FqEntNmPrV; EntNmToFqH.GetDatKeyPrV(FqEntNmPrV); FqEntNmPrV.Sort(false);
    // extract headline
    TChA HeadlineChA=BodyStr.GetSubStr(0, 250);
    while ((HeadlineChA.Len()>0)&&(HeadlineChA.LastCh()!=' ')){
      HeadlineChA.Trunc(HeadlineChA.Len()-1);}
    HeadlineChA+="...";
    // create document
    TSkyGridBinDoc Doc(SwIdStr, Tm, TitleStr, HeadlineChA, FqEntNmPrV);
    // save document
    Doc.Save(SOut);
    // screen log
    if (PrevTm!=Tm){
      if (PrevTm!=0){printf("\n");}
      PrevTm=Tm; DateDocs=0;
    }
    Docs++; DateDocs++;
    printf("  %s [Day:%d / All:%d]\r", DateStr.CStr(), DateDocs, Docs);
  }
  printf("\nDone.\n");
}

void TSkyGridBinDoc::LoadBinDocV(const TStr& InBinFNm){
  printf("Processing SkyGrid News-Binary file '%s'...\n", InBinFNm.CStr());
  TFIn SIn(InBinFNm); int Docs=0;
  while (!SIn.Eof()){
    TSkyGridBinDoc Doc(SIn);
    Docs++; printf("%d\r", Docs);
  }
  printf("\nDone.\n");
}

/////////////////////////////////////////////////
// SkyGrid-Entity
void TSkyGridEnt::GetDocIdV(const TSkyGridBs* SkyGridBs,
 const uint64& MnTm, const uint64& MxTm, TIntV& DocIdV) const {
  DocIdV.Gen(GetDocIds(), 0);
  for (int DocN=0; DocN<GetDocIds(); DocN++){
    int DocId=GetDocId(DocN);
    PSkyGridDoc Doc=SkyGridBs->GetDoc(DocId);
    uint64 DocTm=Doc->GetTm();
    if (((MnTm==0)||(MnTm<=DocTm))&&((MxTm==0)||(DocTm<MxTm))){
      DocIdV.Add(DocId);
    }
  }
}

void TSkyGridEnt::GetDocsPerDateV(
 const TSkyGridBs* SkyGridBs, TStrIntPrV& DateStrDocsPrV, int& Docs) const {
  TStrIntH DateStrToDocsH; Docs=0;
  for (int DocN=0; DocN<GetDocIds(); DocN++){
    int DocId=GetDocId(DocN);
    PSkyGridDoc Doc=SkyGridBs->GetDoc(DocId);
    uint64 DocTm=Doc->GetTm();
    TStr DocDateStr=TTm::GetTmFromMSecs(DocTm).GetWebLogDateStr();
    DateStrToDocsH.AddDat(DocDateStr)++; Docs++;
  }
  DateStrToDocsH.GetKeyDatPrV(DateStrDocsPrV);
  DateStrDocsPrV.Sort();
}

void TSkyGridEnt::GetDocCentroid(const TSkyGridBs* SkyGridBs,
 const int& TopWords, const double& TopWordsWgtSumPrc,
 TStrFltPrV& WordStrWgtPrV) const {
  // create bow
  PSwSet SwSet=TSwSet::GetSwSet(swstEn523);
  PStemmer Stemmer=TStemmer::GetStemmer(stmtPorter);
  PBowDocBs BowDocBs=TBowDocBs::New(SwSet, Stemmer, NULL);
  for (int DocN=0; DocN<GetDocIds(); DocN++){
    int DocId=GetDocId(DocN);
    PSkyGridDoc Doc=SkyGridBs->GetDoc(DocId);
    TStr DocStr=Doc->GetHeadlineStr();
    BowDocBs->AddHtmlDoc(TInt::GetStr(DocId), TStrV(), DocStr);
  }
  // create word-weights
  TBowWordWgtType WordWgtType=bwwtNrmTFIDF; // define weighting
  PBowDocWgtBs BowDocWgtBs=TBowDocWgtBs::New(BowDocBs, WordWgtType, 0, 0);
  // create concept vector
  PBowSim BowSim=TBowSim::New(bstCos); // similarity object
  TIntV AllDIdV; BowDocBs->GetAllDIdV(AllDIdV);
  PBowSpV ConceptSpV=TBowClust::GetConceptSpV(BowDocWgtBs, BowSim, AllDIdV);
  // get word-vector
  ConceptSpV->GetWordStrWgtPrV(BowDocBs, TopWords, TopWordsWgtSumPrc, WordStrWgtPrV);
}

void TSkyGridEnt::GetDocCentroid(const TSkyGridBs* SkyGridBs,
 const PBowDocBs& BowDocBs, const PBowDocWgtBs& BowDocWgtBs,
 const uint64& MnTm, const int& TopWords, const double& TopWordsWgtSumPrc,
 int& Docs, TStrFltPrV& WordStrWgtPrV) const {
  // get doc-ids
  TIntV DocIdV; GetDocIdV(SkyGridBs, MnTm, 0, DocIdV);
  TIntV BowDIdV(DocIdV.Len(), 0);
  for (int DocN=0; DocN<DocIdV.Len(); DocN++){
    int DocId=DocIdV[DocN];
    TStr BowDocNm=TInt::GetStr(DocId);
    int BowDId=BowDocBs->GetDId(BowDocNm);
    BowDIdV.Add(BowDId);
  }
  // create concept vector
  PBowSim BowSim=TBowSim::New(bstCos); // similarity object
  PBowSpV ConceptSpV=TBowClust::GetConceptSpV(BowDocWgtBs, BowSim, BowDIdV);
  // get docs & word-vector
  Docs=DocIdV.Len();
  ConceptSpV->GetWordStrWgtPrV(BowDocBs, TopWords, TopWordsWgtSumPrc, WordStrWgtPrV);
}

void TSkyGridEnt::GetEntClustV(const TSkyGridBs* SkyGridBs,
 const uint64& MnTm, const int& MnDocs, const int& MxDocs, const int& Clusts,
 TVec<TStrFltPrV>& EntNmWgtPrVV) const {
  EntNmWgtPrVV.Clr();
  // create bow
  PBowDocBs BowDocBs=TBowDocBs::New();
  // collect documents
  TIntV DocIdV; GetDocIdV(SkyGridBs, MnTm, 0, DocIdV);
  DocIdV.Reverse(); DocIdV.Shuffle(TRnd(1)); DocIdV.Trunc(MxDocs);
  if (DocIdV.Len()<MnDocs){return;}
  for (int DocN=0; DocN<DocIdV.Len(); DocN++){
    int DocId=DocIdV[DocN];
    PSkyGridDoc Doc=SkyGridBs->GetDoc(DocId);
    // create vector of entity-weights
    TIntFltPrV WIdWgtPrV;
    for (int EntN=0; EntN<Doc->GetEnts(); EntN++){
      int EntId; int EntFq; Doc->GetEntNmFq(EntN, EntId, EntFq);
      TStr EntNm=SkyGridBs->GetEntNm(EntId);
      int EntWId=BowDocBs->AddWordStr(EntNm);
      WIdWgtPrV.Add(TIntFltPr(EntWId, EntFq));
    }
    // create bow-document
    int DId=BowDocBs->AddDoc(TInt::GetStr(DocId), TStrV(), WIdWgtPrV);
    TStr DocDescStr=Doc->GetTitleStr();
    BowDocBs->PutDocDescStr(DId, DocDescStr);
  }
  // k-means clustering
  PBowSim BowSim=TBowSim::New(bstCos); // similarity object
  TBowWordWgtType WordWgtType=bwwtNrmTFIDF; // define weighting
  PBowDocPart BowDocPart=TBowClust::GetKMeansPart(
   TNotify::StdNotify, // log output
   BowDocBs, // document data
   BowSim, // similarity function
   TRnd(1), // random generator
   Clusts, // number of clusters
   1, // trials per k-means
   1, // convergence epsilon for k-means
   1, // min. documents per cluster
   WordWgtType, // word weighting
   0, // cut-word-weights percentage
   0); // minimal word frequency
  EntNmWgtPrVV.Clr();
  for (int ClustN=0; ClustN<BowDocPart->GetClusts(); ClustN++){
    PBowDocPartClust Clust=BowDocPart->GetClust(ClustN);
    TStrFltPrV WordStrWgtPrV;
    Clust->GetTopWordStrWgtPrV(BowDocBs, 25, 0.5, WordStrWgtPrV);
    EntNmWgtPrVV.Add(WordStrWgtPrV);
  }
  //BowDocPart->SaveTxt("Clusts.Txt", BowDocBs, true, 25, 0.5, false);
}

void TSkyGridEnt::GetSorted_LinkWgtDstEntIdPrV(
 const uint64& MnTm, const double& TopWgtSumPrc, TIntPrV& LinkWgtDstEntIdPrV) const {
  double AllLinkWgtSum=0;
  TIntIntH DstEntIdLinkWgtH;
  int LinkEnts=GetLinkEnts();
  for (int LinkEntN=0; LinkEntN<LinkEnts; LinkEntN++){
    int DstEntId=GetLinkEntId(LinkEntN);
    int EntLinks=GetEntLinks(LinkEntN);
    int EntLinkWgtSum=0;
    for (int EntLinkN=0; EntLinkN<EntLinks; EntLinkN++){
      const TSkyGridEntLinkCtx& EntLinkCtx=GetEntLinkCtx(LinkEntN, EntLinkN);
      if (EntLinkCtx.Tm>=MnTm){
        EntLinkWgtSum+=EntLinkCtx.LinkWgt;}
    }
    DstEntIdLinkWgtH.AddDat(DstEntId, EntLinkWgtSum);
    AllLinkWgtSum+=EntLinkWgtSum;
  }
  LinkWgtDstEntIdPrV.Clr(); DstEntIdLinkWgtH.GetDatKeyPrV(LinkWgtDstEntIdPrV);
  LinkWgtDstEntIdPrV.Sort(false);
  // cut long-tail
  if ((TopWgtSumPrc>0.0)&&(LinkWgtDstEntIdPrV.Len()>0)){
    int TopLinkWgt=LinkWgtDstEntIdPrV[0].Val1;
    if (TopLinkWgt>(3*AllLinkWgtSum)/LinkWgtDstEntIdPrV.Len()){
      double CutWgtSum=AllLinkWgtSum*(1-TopWgtSumPrc);
      int LastValN=LinkWgtDstEntIdPrV.Len()-1;
      while ((LastValN>0)&&(CutWgtSum>0)){
        CutWgtSum-=LinkWgtDstEntIdPrV[LastValN].Val1;
        LastValN--;
      }
      LinkWgtDstEntIdPrV.Trunc(LastValN+1);
    }
  }
}

/////////////////////////////////////////////////
// SkyGrid-Base
void TSkyGridBs::GetSorted_DocsEntIdPrV(TIntPrV& DocsEntIdPrV){
  TIntIntH EntIdToDocsH;
  for (int EntId=0; EntId<GetEnts(); EntId++){
    int Docs=GetEnt(EntId).GetDocIds();
    EntIdToDocsH.AddDat(EntId, Docs);
  }
  DocsEntIdPrV.Clr(); EntIdToDocsH.GetDatKeyPrV(DocsEntIdPrV);
  DocsEntIdPrV.Sort(false);
}

int TSkyGridBs::AddDoc(
 const TStr& SwIdStr, const uint64& Tm,
 const TStr& TitleStr, const TStr& HeadlineStr,
 const TIntStrPrV FqEntNmPrV){
  // create entity-id vector
  TIntPrV EntIdFqPrV(FqEntNmPrV.Len(), 0);
  for (int EntN=0; EntN<FqEntNmPrV.Len(); EntN++){
    TStr EntNm=FqEntNmPrV[EntN].Val2;
    int EntFq=FqEntNmPrV[EntN].Val1;
    if (EntFq>=GetMnEntFqPerDoc()){
      int EntId=AddEntNm(EntNm);
      EntIdFqPrV.Add(TIntPr(EntId, EntFq));
    }
  }
  // check entity-id vector lenght
  if (EntIdFqPrV.Len()<GetMnEntsPerDoc()){
    return -1;}
  // create document
  PSkyGridDoc Doc=
   TSkyGridDoc::New(SwIdStr, Tm, TitleStr, HeadlineStr, EntIdFqPrV);
  // add document to base
  int DocId=GetNewDocId();
  IdToDocH.AddDat(DocId, Doc);
  // get entity
  for (int EntN=0; EntN<EntIdFqPrV.Len(); EntN++){
    int EntId=EntIdFqPrV[EntN].Val1;
    TSkyGridEnt& Ent=GetEnt(EntId);
    Ent.PushDocId(DocId);
  }
  // create link
  int EntLinkWgtSum=0;
  for (int EntN1=0; EntN1<EntIdFqPrV.Len(); EntN1++){
    int EntId1=EntIdFqPrV[EntN1].Val1;
    TSkyGridEnt& Ent1=GetEnt(EntId1);
    int EntWgt1=EntIdFqPrV[EntN1].Val2;
    for (int EntN2=0; EntN2<EntIdFqPrV.Len(); EntN2++){
      if (EntN1==EntN2){continue;}
      int EntId2=EntIdFqPrV[EntN2].Val1;
      int EntWgt2=EntIdFqPrV[EntN2].Val2;
      // entity-link-weight
      int EntLinkWgt=EntWgt1*EntWgt2;
      EntLinkWgtSum+=EntLinkWgt;
      // create entity-link-context
      TSkyGridEntLinkCtx LinkCtx(EntLinkWgt, DocId, Tm);
      Ent1.AddLink(EntId2, LinkCtx);
    }
  }

  // return doc-id
  return DocId;
}

PBowDocBs TSkyGridBs::GetBowDocBs(
 const int& MxNGramLen, const int& MnNGramFq) const {
  // prepare stop-words
  PSwSet SwSet=TSwSet::GetSwSet(swstEn523);
  // prepare stemmer
  PStemmer Stemmer=TStemmer::GetStemmer(stmtPorter);
  // create ngrams
  PNGramBs NGramBs;
  if (!((MxNGramLen==1)&&(MnNGramFq==1))){
    TStrV HtmlStrV;
    TSkyGridIdDocPrV IdDocPrV; GetIdDocPrV(IdDocPrV);
    for (int DocN=0; DocN<IdDocPrV.Len(); DocN++){
      PSkyGridDoc Doc=IdDocPrV[DocN].Val2;
      TStr DocStr=Doc->GetHeadlineStr();
      HtmlStrV.Add(DocStr);
    }
    NGramBs=TNGramBs::GetNGramBsFromHtmlStrV(
     HtmlStrV, MxNGramLen, MnNGramFq, SwSet, Stemmer);
  }
  // create bow
  PBowDocBs BowDocBs=TBowDocBs::New(SwSet, Stemmer, NGramBs);
  TSkyGridIdDocPrV IdDocPrV; GetIdDocPrV(IdDocPrV);
  for (int DocN=0; DocN<IdDocPrV.Len(); DocN++){
    int DocId=IdDocPrV[DocN].Val1;
    PSkyGridDoc Doc=IdDocPrV[DocN].Val2;
    TStr DocStr=Doc->GetHeadlineStr();
    BowDocBs->AddHtmlDoc(TInt::GetStr(DocId), TStrV(), DocStr);
  }
  // return bow
  return BowDocBs;
}

PBowDocWgtBs TSkyGridBs::GetBowDocWgtBs(const PBowDocBs& BowDocBs) const {
  // define weighting
  TBowWordWgtType WordWgtType=bwwtNrmTFIDF;
  // create word-weights
  PBowDocWgtBs BowDocWgtBs=TBowDocWgtBs::New(BowDocBs, WordWgtType, 0, 0);
  // return bow-weights
  return BowDocWgtBs;
}

void TSkyGridBs::GetWordStrWgtPrVDiff(
 const TStrFltPrV& OldWordStrWgtPrV, const TStrFltPrV& NewWordStrWgtPrV,
 TStrFltPrV& NegDiffWordStrWgtPrV, TStrFltPrV& PosDiffWordStrWgtPrV){
  TStrFltH WordStrToWgtH;
  // set previous-vector
  for (int WordN=0; WordN<NewWordStrWgtPrV.Len(); WordN++){
    TStr WStr=NewWordStrWgtPrV[WordN].Val1;
    double WWgt=NewWordStrWgtPrV[WordN].Val2;
    WordStrToWgtH.AddDat(WStr, WWgt);
  }
  // diff current-vector
  for (int WordN=0; WordN<OldWordStrWgtPrV.Len(); WordN++){
    TStr WStr=OldWordStrWgtPrV[WordN].Val1;
    double WWgt=OldWordStrWgtPrV[WordN].Val2;
    double CurWWgt=WordStrToWgtH.AddDat(WStr);
    WordStrToWgtH.AddDat(WStr, CurWWgt-WWgt);
  }
  // extract vector
  TFltStrPrV DiffWordWgtStrPrV; WordStrToWgtH.GetDatKeyPrV(DiffWordWgtStrPrV);
  // positive-vector
  DiffWordWgtStrPrV.Sort(true);
  NegDiffWordStrWgtPrV.Gen(DiffWordWgtStrPrV.Len(), 0);
  for (int WordN=0; WordN<DiffWordWgtStrPrV.Len(); WordN++){
    TStr WStr=DiffWordWgtStrPrV[WordN].Val2;
    double WWgt=DiffWordWgtStrPrV[WordN].Val1;
    if (WWgt!=0){NegDiffWordStrWgtPrV.Add(TStrFltPr(WStr, WWgt));}
  }
  // negative-vector
  DiffWordWgtStrPrV.Sort(false);
  PosDiffWordStrWgtPrV.Gen(DiffWordWgtStrPrV.Len(), 0);
  for (int WordN=0; WordN<DiffWordWgtStrPrV.Len(); WordN++){
    TStr WStr=DiffWordWgtStrPrV[WordN].Val2;
    double WWgt=DiffWordWgtStrPrV[WordN].Val1;
    if (WWgt!=0){PosDiffWordStrWgtPrV.Add(TStrFltPr(WStr, WWgt));}
  }
}

void TSkyGridBs::GetWordStrWgtPrVChA(
 const TStrFltPrV& WordStrWgtPrV, TChA& WordStrWgtPrVChA){
  WordStrWgtPrVChA.Clr();
  for (int WordN=0; WordN<WordStrWgtPrV.Len(); WordN++){
    TStr WStr=WordStrWgtPrV[WordN].Val1;
    double WWgt=WordStrWgtPrV[WordN].Val2;
    if (WordN>0){WordStrWgtPrVChA+=' ';}
    WordStrWgtPrVChA+=TStr::Fmt("['%s':%.3f]", WStr.CStr(), WWgt);
  }
}

void TSkyGridBs::GetLinkWgtDstEntIdPrVDiff(
 const TIntPrV& OldLinkWgtDstEntIdPrV, const TIntPrV& NewLinkWgtDstEntIdPrV,
 TIntPrV& NegDiffLinkWgtDstEntIdPrV, TIntPrV& PosDiffLinkWgtDstEntIdPrV){
  TIntIntH DstEntIdToLinkWgtH;
  // set previous-vector
  for (int WordN=0; WordN<NewLinkWgtDstEntIdPrV.Len(); WordN++){
    int LinkWgt=NewLinkWgtDstEntIdPrV[WordN].Val1;
    int DstEntId=NewLinkWgtDstEntIdPrV[WordN].Val2;
    DstEntIdToLinkWgtH.AddDat(DstEntId, LinkWgt);
  }
  // diff current-vector
  for (int WordN=0; WordN<OldLinkWgtDstEntIdPrV.Len(); WordN++){
    int LinkWgt=OldLinkWgtDstEntIdPrV[WordN].Val1;
    int DstEntId=OldLinkWgtDstEntIdPrV[WordN].Val2;
    int CurLinkWgt=DstEntIdToLinkWgtH.AddDat(DstEntId);
    DstEntIdToLinkWgtH.AddDat(DstEntId, CurLinkWgt-LinkWgt);
  }
  // extract vector
  TIntPrV _DiffLinkWgtDstEntIdPrV;
  DstEntIdToLinkWgtH.GetDatKeyPrV(_DiffLinkWgtDstEntIdPrV);
  // clean zeros
  TIntPrV DiffLinkWgtDstEntIdPrV(_DiffLinkWgtDstEntIdPrV.Len(), 0);
  for (int EntN=0; EntN<_DiffLinkWgtDstEntIdPrV.Len(); EntN++){
    int LinkWgt=_DiffLinkWgtDstEntIdPrV[EntN].Val1;
    if (LinkWgt!=0){
      DiffLinkWgtDstEntIdPrV.Add(_DiffLinkWgtDstEntIdPrV[EntN]);}
  }
  // positive-vector
  DiffLinkWgtDstEntIdPrV.Sort(true);
  NegDiffLinkWgtDstEntIdPrV=DiffLinkWgtDstEntIdPrV;
  // negative-vector
  DiffLinkWgtDstEntIdPrV.Sort(false);
  PosDiffLinkWgtDstEntIdPrV=DiffLinkWgtDstEntIdPrV;
}

void TSkyGridBs::GetLinkWgtDstEntIdPrVChA(
 const TIntPrV& LinkWgtDstEntIdPrV, TChA& LinkWgtDstEntIdPrVChA){
  LinkWgtDstEntIdPrVChA.Clr();
  for (int DstEntN=0; DstEntN<LinkWgtDstEntIdPrV.Len(); DstEntN++){
    int DstEntId=LinkWgtDstEntIdPrV[DstEntN].Val2;
    TStr DstEntNm=GetEntNm(DstEntId);
    int LinkWgt=LinkWgtDstEntIdPrV[DstEntN].Val1;
    if (DstEntN>0){LinkWgtDstEntIdPrVChA+=' ';}
    LinkWgtDstEntIdPrVChA+=TStr::Fmt("['%s':%d]", DstEntNm.CStr(), LinkWgt);
  }
}

void TSkyGridBs::SaveTxt(const TStr& FNm, const uint64& CurTm){
  // time-limit
  TStr CurTmStr=TTm::GetTmFromMSecs(CurTm).GetWebLogDateTimeStr();
  uint64 CurDateTm=TTm::GetMSecsFromTm(TTm::GetTmFromWebLogDateTimeStr(TTm::GetTmFromMSecs(CurTm).GetWebLogDateStr()));
  TStr CurDateTmStr=TTm::GetTmFromMSecs(CurDateTm).GetWebLogDateTimeStr();
  TUInt64V MnTmV;
  MnTmV.Add(CurDateTm-0*TTmInfo::GetDayMSecs());
  MnTmV.Add(CurDateTm-1*TTmInfo::GetDayMSecs());
  MnTmV.Add(CurDateTm-2*TTmInfo::GetDayMSecs());
  MnTmV.Add(CurDateTm-4*TTmInfo::GetDayMSecs());
  MnTmV.Add(CurDateTm-8*TTmInfo::GetDayMSecs());
  MnTmV.Add(CurDateTm-16*TTmInfo::GetDayMSecs());
  MnTmV.Add(CurDateTm-32*TTmInfo::GetDayMSecs());

  // get bow
  //PBowDocBs BowDocBs=GetBowDocBs(3, 5);
  PBowDocBs BowDocBs=GetBowDocBs();
  PBowDocWgtBs BowDocWgtBs=GetBowDocWgtBs(BowDocBs);

  // open file
  TFOut FOut(FNm); FILE* fOut=FOut.GetFileId();
  // get docs-entities sorted vector
  TIntPrV DocsEntIdPrV; GetSorted_DocsEntIdPrV(DocsEntIdPrV);
  // traverse entities
  for (int EntN=0; EntN<DocsEntIdPrV.Len(); EntN++){
    int EntId=DocsEntIdPrV[EntN].Val2;
    TStr EntNm=GetEntNm(EntId);
    int EntDocs=DocsEntIdPrV[EntN].Val1;
    TSkyGridEnt& Ent=GetEnt(EntId);
    int LinkEnts=Ent.GetLinkEnts();
    fprintf(fOut, "'%s' [%d docs] [%d ents]\n", EntNm.CStr(), EntDocs, LinkEnts);

    // output docs over dates
    {TStrIntPrV DateStrDocsPrV; int _EntDocs;
    Ent.GetDocsPerDateV(this, DateStrDocsPrV, _EntDocs);
    fprintf(fOut, "   Docs per Date (%d docs):", _EntDocs);
    for (int DateN=0; DateN<DateStrDocsPrV.Len(); DateN++){
      TStr DateStr=DateStrDocsPrV[DateN].Val1;
      int Docs=DateStrDocsPrV[DateN].Val2;
      fprintf(fOut, " [%s:%d]", DateStr.CStr(), Docs);
    }
    fprintf(fOut, "\n");}

    fprintf(fOut, "   [Now: %s]\n", CurTmStr.CStr());
    TIntPrV PrevLinkWgtDstEntIdPrV;
    TStrFltPrV PrevWordStrWgtPrV;
    for (int MnTmN=0; MnTmN<MnTmV.Len(); MnTmN++){
      uint64 MnTm=MnTmV[MnTmN];
      double PastDays=(CurDateTm-MnTm)/double(TTmInfo::GetDayMSecs());
      TStr MnTmStr=TTm::GetTmFromMSecs(MnTm).GetWebLogDateTimeStr();
      // get linked entities
      TIntPrV LinkWgtDstEntIdPrV;
      Ent.GetSorted_LinkWgtDstEntIdPrV(MnTm, 0.9, LinkWgtDstEntIdPrV);
      // output difference between previous and current centroid
      if (MnTmN>0){
        TIntPrV NegDiffLinkWgtDstEntIdPrV; TIntPrV PosDiffLinkWgtDstEntIdPrV;
        GetLinkWgtDstEntIdPrVDiff(LinkWgtDstEntIdPrV, PrevLinkWgtDstEntIdPrV,
         NegDiffLinkWgtDstEntIdPrV, PosDiffLinkWgtDstEntIdPrV);
        // output positive change
        TChA PosDiffLinkWgtDstEntIdPrVChA;
        GetLinkWgtDstEntIdPrVChA(PosDiffLinkWgtDstEntIdPrV, PosDiffLinkWgtDstEntIdPrVChA);
        fprintf(fOut, "         Pos-Diff: %s\n", PosDiffLinkWgtDstEntIdPrVChA.CStr());
        // output negative change
        TChA NegDiffLinkWgtDstEntIdPrVChA;
        GetLinkWgtDstEntIdPrVChA(NegDiffLinkWgtDstEntIdPrV, NegDiffLinkWgtDstEntIdPrVChA);
        fprintf(fOut, "         Neg-Diff: %s\n", NegDiffLinkWgtDstEntIdPrVChA.CStr());
      }
      PrevLinkWgtDstEntIdPrV=LinkWgtDstEntIdPrV;
      // output linked entities
      int TopLinkEnts=LinkWgtDstEntIdPrV.Len();
      TChA LinkWgtDstEntIdPrVChA;
      GetLinkWgtDstEntIdPrVChA(LinkWgtDstEntIdPrV, LinkWgtDstEntIdPrVChA);
      fprintf(fOut, "      Entities (%d ents): %s\n",
       TopLinkEnts, LinkWgtDstEntIdPrVChA.CStr());
      // get text centroid
      int CtrDocs; TStrFltPrV WordStrWgtPrV;
      Ent.GetDocCentroid(this, BowDocBs, BowDocWgtBs, MnTm, 150, 0.9, CtrDocs, WordStrWgtPrV);
      // output difference between previous and current centroid
      if (MnTmN>0){
        TStrFltPrV NegDiffWordStrWgtPrV; TStrFltPrV PosDiffWordStrWgtPrV;
        GetWordStrWgtPrVDiff(WordStrWgtPrV, PrevWordStrWgtPrV,
         NegDiffWordStrWgtPrV, PosDiffWordStrWgtPrV);
        // output positive change
        TChA PosDiffWordStrWgtPrVChA; GetWordStrWgtPrVChA(PosDiffWordStrWgtPrV, PosDiffWordStrWgtPrVChA);
        fprintf(fOut, "         Pos-Diff: %s\n", PosDiffWordStrWgtPrVChA.CStr());
        // output negative change
        TChA NegDiffWordStrWgtPrVChA; GetWordStrWgtPrVChA(NegDiffWordStrWgtPrV, NegDiffWordStrWgtPrVChA);
        fprintf(fOut, "         Neg-Diff: %s\n", NegDiffWordStrWgtPrVChA.CStr());
      }
      PrevWordStrWgtPrV=WordStrWgtPrV;
      // output centroid
      TChA WordStrWgtPrVChA; GetWordStrWgtPrVChA(WordStrWgtPrV, WordStrWgtPrVChA);
      fprintf(fOut, "      Centroid (%d docs, %d words): %s\n",
       CtrDocs, WordStrWgtPrV.Len(), WordStrWgtPrVChA.CStr());
      // output time
      fprintf(fOut, "   [-%.1f days: %s]\n", PastDays, MnTmStr.CStr());
    }
    // entity clustering
    /*TVec<TStrFltPrV> EntNmWgtPrVV;
    Ent.GetEntClustV(this, MnTmV.Last(), 100, 1000, 10, EntNmWgtPrVV);
    for (int ClustN=0; ClustN<EntNmWgtPrVV.Len(); ClustN++){
      TStrFltPrV& EntNmWgtPrV=EntNmWgtPrVV[ClustN];
      fprintf(fOut, "   Clust-%d:", ClustN);
      for (int EntN=0; EntN<EntNmWgtPrV.Len(); EntN++){
        TStr EntNm=EntNmWgtPrV[EntN].Val1;
        double Wgt=EntNmWgtPrV[EntN].Val2;
        fprintf(fOut, " ['%s':%.3f]", EntNm.CStr(), Wgt);
      }
      fprintf(fOut, "\n");
    }*/
    fprintf(fOut, "\n");
  }
}

