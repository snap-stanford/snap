/////////////////////////////////////////////////
// Includes
#include "tnt.h"

/////////////////////////////////////////////////
// Tnt-Document
void TTntBinDoc::SaveBinDocV(
 const TStr& InXmlFPath, const TStr& OutBinFNm, const int& MxDocs){
  printf("Processing Tnt-News-Xml files from '%s'...\n", InXmlFPath.CStr());
  TFOut SOut(OutBinFNm);
  TFFile FFile(InXmlFPath, true); TStr FNm;
  int Docs=0; int DateDocs=0; uint64 PrevTm=0;
  while (FFile.Next(FNm)){
    if ((MxDocs!=-1)&&(Docs>=MxDocs)){break;}
    //printf("  Processing '%s' ...", FNm.CStr());
    PXmlDoc XmlDoc=TXmlDoc::LoadTxt(FNm);
    PXmlTok ContentTok=XmlDoc->GetTagTok("item|content");
    TStr DocNm=ContentTok->GetTagTok("swid")->GetArgVal("value");
    TStr UrlStr=ContentTok->GetTagTok("url")->GetTokStr(false);
    TStr SubjStr=ContentTok->GetTagTok("title")->GetTokStr(false);
    TStr FetchedValStr=ContentTok->GetTagTok("fetched")->GetArgVal("value");
    TXmlTokV EntityTokV; ContentTok->GetTagTokV("annotations|entity", EntityTokV);
    TStr BodyStr=ContentTok->GetTagTok("body")->GetTokStr(false);
    // extract date
    TStr DateStr=DocNm.GetSubStr(0, 7);
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
    TTntBinDoc Doc(DocNm, Tm, SubjStr, HeadlineChA, FqEntNmPrV);
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

void TTntBinDoc::LoadBinDocV(const TStr& InBinFNm){
  printf("Processing Tnt News-Binary file '%s'...\n", InBinFNm.CStr());
  TFIn SIn(InBinFNm); int Docs=0;
  while (!SIn.Eof()){
    TTntBinDoc Doc(SIn);
    Docs++; printf("%d\r", Docs);
  }
  printf("\nDone.\n");
}

/////////////////////////////////////////////////
// Tnt-Entity
void TTntEnt::GetDocIdV(const TTntBs* TntBs,
 const uint64& MnTm, const uint64& MxTm, TIntV& DocIdV) const {
  DocIdV.Gen(GetDocIds(), 0);
  for (int DocN=0; DocN<GetDocIds(); DocN++){
    int DocId=GetDocId(DocN);
    PTntDoc Doc=TntBs->GetDoc(DocId);
    uint64 DocTm=Doc->GetTm();
    if (((MnTm==0)||(MnTm<=DocTm))&&((MxTm==0)||(DocTm<MxTm))){
      DocIdV.Add(DocId);
    }
  }
}

void TTntEnt::GetDocsPerDateV(
 const TTntBs* TntBs, TStrIntPrV& DateStrDocsPrV, int& Docs) const {
  TStrIntH DateStrToDocsH; Docs=0;
  for (int DocN=0; DocN<GetDocIds(); DocN++){
    int DocId=GetDocId(DocN);
    PTntDoc Doc=TntBs->GetDoc(DocId);
    uint64 DocTm=Doc->GetTm();
    TStr DocDateStr=TTm::GetTmFromMSecs(DocTm).GetWebLogDateStr();
    DateStrToDocsH.AddDat(DocDateStr)++; Docs++;
  }
  DateStrToDocsH.GetKeyDatPrV(DateStrDocsPrV);
  DateStrDocsPrV.Sort();
}

void TTntEnt::GetDocCentroid(const TTntBs* TntBs,
 const int& TopWords, const double& TopWordsWgtSumPrc,
 TStrFltPrV& WordStrWgtPrV) const {
  // create bow
  PSwSet SwSet=TSwSet::GetSwSet(swstEn523);
  PStemmer Stemmer=TStemmer::GetStemmer(stmtPorter);
  PBowDocBs BowDocBs=TBowDocBs::New(SwSet, Stemmer, NULL);
  for (int DocN=0; DocN<GetDocIds(); DocN++){
    int DocId=GetDocId(DocN);
    PTntDoc Doc=TntBs->GetDoc(DocId);
    TStr DocStr=Doc->GetContStr();
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

void TTntEnt::GetDocCentroid(const TTntBs* TntBs,
 const PBowDocBs& BowDocBs, const PBowDocWgtBs& BowDocWgtBs,
 const uint64& MnTm, const TStr& QKwStr, 
 const int& TopWords, const double& TopWordsWgtSumPrc,
 int& Docs, TStrFltPrV& WordStrWgtPrV) const {
  // get doc-ids
  TIntV DocIdV; GetDocIdV(TntBs, MnTm, 0, DocIdV);
  if (DocIdV.Len()>0){

  }
  TIntV BowDIdV(DocIdV.Len(), 0);
  for (int DocN=0; DocN<DocIdV.Len(); DocN++){
    int DocId=DocIdV[DocN];
    TStr BowDocNm=TInt::GetStr(DocId);
    int BowDId=BowDocBs->GetDId(BowDocNm);
    if (QKwStr.Empty()){
      BowDIdV.Add(BowDId);
    } else {
      if (BowDocBs->IsDocWordStr(BowDId, QKwStr)){
        BowDIdV.Add(BowDId);
      }
    }
  }
  // create concept vector
  PBowSim BowSim=TBowSim::New(bstCos); // similarity object
  PBowSpV ConceptSpV=TBowClust::GetConceptSpV(BowDocWgtBs, BowSim, BowDIdV);
  // get docs & word-vector
  Docs=DocIdV.Len();
  ConceptSpV->GetWordStrWgtPrV(BowDocBs, TopWords, TopWordsWgtSumPrc, WordStrWgtPrV);
}

void TTntEnt::GetEntClustV(const TTntBs* TntBs,
 const uint64& MnTm, const int& MnDocs, const int& MxDocs, const int& Clusts,
 TVec<TStrFltPrV>& EntNmWgtPrVV) const {
  EntNmWgtPrVV.Clr();
  // create bow
  PBowDocBs BowDocBs=TBowDocBs::New();
  // collect documents
  TIntV DocIdV; GetDocIdV(TntBs, MnTm, 0, DocIdV);
  DocIdV.Reverse();
  TRnd Rnd(1);
  DocIdV.Shuffle(Rnd);
  DocIdV.Trunc(MxDocs);
  if (DocIdV.Len()<MnDocs){return;}
  for (int DocN=0; DocN<DocIdV.Len(); DocN++){
    int DocId=DocIdV[DocN];
    PTntDoc Doc=TntBs->GetDoc(DocId);
    // create vector of entity-weights
    TIntFltPrV WIdWgtPrV;
    for (int EntN=0; EntN<Doc->GetEnts(); EntN++){
      int EntId; int EntFq; Doc->GetEntNmFq(EntN, EntId, EntFq);
      TStr EntNm=TntBs->GetEntNm(EntId);
      int EntWId=BowDocBs->AddWordStr(EntNm);
      WIdWgtPrV.Add(TIntFltPr(EntWId, EntFq));
    }
    // create bow-document
    int DId=BowDocBs->AddDoc(TInt::GetStr(DocId), TStrV(), WIdWgtPrV);
    TStr DocDescStr=Doc->GetSubjStr();
    BowDocBs->PutDocDescStr(DId, DocDescStr);
  }
  // k-means clustering
  PBowSim BowSim=TBowSim::New(bstCos); // similarity object
  TBowWordWgtType WordWgtType=bwwtNrmTFIDF; // define weighting
  PBowDocPart BowDocPart=TBowClust::GetKMeansPart(
   TNotify::StdNotify, // log output
   BowDocBs, // document data
   BowSim, // similarity function
   Rnd, // random generator
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

void TTntEnt::GetSorted_LinkWgtDstEntIdPrV(
 const uint64& MnTm, const double& TopWgtSumPrc, TIntPrV& LinkWgtDstEntIdPrV) const {
  double AllLinkWgtSum=0;
  TIntIntH DstEntIdLinkWgtH;
  int LinkEnts=GetLinkEnts();
  for (int LinkEntN=0; LinkEntN<LinkEnts; LinkEntN++){
    int DstEntId=GetLinkEntId(LinkEntN);
    int EntLinks=GetEntLinks(LinkEntN);
    int EntLinkWgtSum=0;
    for (int EntLinkN=0; EntLinkN<EntLinks; EntLinkN++){
      const TTntEntLinkCtx& EntLinkCtx=GetEntLinkCtx(LinkEntN, EntLinkN);
      if (EntLinkCtx.Tm>=MnTm){
        EntLinkWgtSum+=EntLinkCtx.LinkWgt;}
    }
    if (EntLinkWgtSum>0){
      DstEntIdLinkWgtH.AddDat(DstEntId, EntLinkWgtSum);}
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
// Tnt-Base
void TTntBs::GetSorted_DocsEntIdPrV(TIntPrV& DocsEntIdPrV){
  TIntIntH EntIdToDocsH;
  for (int EntId=0; EntId<GetEnts(); EntId++){
    int Docs=GetEnt(EntId)->GetDocIds();
    EntIdToDocsH.AddDat(EntId, Docs);
  }
  DocsEntIdPrV.Clr(); EntIdToDocsH.GetDatKeyPrV(DocsEntIdPrV);
  DocsEntIdPrV.Sort(false);
}

int TTntBs::AddDoc(
 const TStr& DocNm, const uint64& Tm,
 const TStr& SubjStr, const TStr& ContStr,
 const TIntStrPrV FqEntNmPrV){
  // create entity-id vector
  TIntPrV EntIdFqPrV(FqEntNmPrV.Len(), 0);
  for (int EntN=0; EntN<FqEntNmPrV.Len(); EntN++){
    TStr EntNm=FqEntNmPrV[EntN].Val2;
    int EntFq=FqEntNmPrV[EntN].Val1;
    if (EntFq>=GetMnEntFqPerDoc()){
      int EntId=AddEnt(EntNm);
      EntIdFqPrV.Add(TIntPr(EntId, EntFq));
    }
  }
  // check entity-id vector lenght
  if (EntIdFqPrV.Len()<GetMnEntsPerDoc()){
    return -1;}
  // create document
  PTntDoc Doc=
   TTntDoc::New(DocNm, Tm, SubjStr, ContStr, EntIdFqPrV);
  // add document to base
  int DocId=GetNewDocId();
  IdToDocH.AddDat(DocId, Doc);
  // get entity
  for (int EntN=0; EntN<EntIdFqPrV.Len(); EntN++){
    int EntId=EntIdFqPrV[EntN].Val1;
    GetEnt(EntId)->PushDocId(DocId);
  }
  // create link
  int EntLinkWgtSum=0;
  for (int EntN1=0; EntN1<EntIdFqPrV.Len(); EntN1++){
    int EntId1=EntIdFqPrV[EntN1].Val1;
    PTntEnt Ent1=GetEnt(EntId1);
    int EntWgt1=EntIdFqPrV[EntN1].Val2;
    for (int EntN2=0; EntN2<EntIdFqPrV.Len(); EntN2++){
      if (EntN1==EntN2){continue;}
      int EntId2=EntIdFqPrV[EntN2].Val1;
      int EntWgt2=EntIdFqPrV[EntN2].Val2;
      // entity-link-weight
      int EntLinkWgt=EntWgt1*EntWgt2;
      EntLinkWgtSum+=EntLinkWgt;
      // create entity-link-context
      TTntEntLinkCtx LinkCtx(EntLinkWgt, DocId, Tm);
      Ent1->AddLink(EntId2, LinkCtx);
    }
  }

  // return doc-id
  return DocId;
}

void TTntBs::GetMnMxDocTm(uint64& MnDocTm, uint64& MxDocTm) const {
  MnDocTm=TUInt64::Mx; MxDocTm=TUInt64::Mn;
  TTntIdDocPrV IdDocPrV; GetIdDocPrV(IdDocPrV); int Docs=IdDocPrV.Len();
  for (int DocN=0; DocN<Docs; DocN++){
    uint64 Tm=IdDocPrV[DocN].Val2->GetTm();
    if (Tm<MnDocTm){MnDocTm=Tm;}
    if (Tm>MxDocTm){MxDocTm=Tm;}
  }
}

PBowDocBs TTntBs::GetBowDocBs(
 const int& MxNGramLen, const int& MnNGramFq) const {
  // prepare stop-words
  PSwSet SwSet=TSwSet::GetSwSet(swstEn523);
  // prepare stemmer
  PStemmer Stemmer=TStemmer::GetStemmer(stmtPorter);
  // create ngrams
  PNGramBs NGramBs;
  if (!((MxNGramLen==1)&&(MnNGramFq==1))){
    TStrV HtmlStrV;
    TTntIdDocPrV IdDocPrV; GetIdDocPrV(IdDocPrV);
    for (int DocN=0; DocN<IdDocPrV.Len(); DocN++){
      PTntDoc Doc=IdDocPrV[DocN].Val2;
      TStr DocStr=Doc->GetContStr();
      HtmlStrV.Add(DocStr);
    }
    NGramBs=TNGramBs::GetNGramBsFromHtmlStrV(
     HtmlStrV, MxNGramLen, MnNGramFq, SwSet, Stemmer);
  }
  // create bow
  PBowDocBs BowDocBs=TBowDocBs::New(SwSet, Stemmer, NGramBs);
  TTntIdDocPrV IdDocPrV; GetIdDocPrV(IdDocPrV);
  for (int DocN=0; DocN<IdDocPrV.Len(); DocN++){
    int DocId=IdDocPrV[DocN].Val1;
    PTntDoc Doc=IdDocPrV[DocN].Val2;
    TStr DocStr=Doc->GetContStr();
    BowDocBs->AddHtmlDoc(TInt::GetStr(DocId), TStrV(), DocStr);
  }
  // return bow
  return BowDocBs;
}

PBowDocWgtBs TTntBs::GetBowDocWgtBs(const PBowDocBs& BowDocBs) const {
  // define weighting
  TBowWordWgtType WordWgtType=bwwtNrmTFIDF;
  // create word-weights
  PBowDocWgtBs BowDocWgtBs=TBowDocWgtBs::New(BowDocBs, WordWgtType, 0, 0);
  // return bow-weights
  return BowDocWgtBs;
}

void TTntBs::GetWordStrWgtPrVDiff(
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

void TTntBs::GetWordStrWgtPrV(const TStrFltPrV& WordStrWgtPrV, 
 TChA& WordStrWgtPrVChA, TChA& WordStrWgtPrVXmlChA){
  WordStrWgtPrVChA.Clr(); WordStrWgtPrVXmlChA.Clr();
  for (int WordN=0; WordN<WordStrWgtPrV.Len(); WordN++){
    TStr WStr=WordStrWgtPrV[WordN].Val1;
    TStr XmlWStr=TXmlLx::GetXmlStrFromPlainStr(WStr);
    double WWgt=WordStrWgtPrV[WordN].Val2;
    if (WordN>0){WordStrWgtPrVChA+=' ';}
    WordStrWgtPrVChA+=TStr::Fmt("['%s':%.3f]", WStr.CStr(), WWgt);
    WordStrWgtPrVXmlChA+=TStr::Fmt("<Kw str=\"%s\" wgt=\"%.3f\"/>", XmlWStr.CStr(), WWgt);
  }
}

void TTntBs::GetLinkWgtDstEntIdPrVDiff(
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

void TTntBs::GetLinkWgtDstEntNmPrV(const TIntPrV& LinkWgtDstEntIdPrV, 
 TStrIntPrV& LinkWgtDstEntNmPrV, TChA& LinkWgtDstEntIdPrVChA, TChA& LinkWgtDstEntIdPrVXmlChA){
  LinkWgtDstEntNmPrV.Clr(); LinkWgtDstEntIdPrVChA.Clr(); LinkWgtDstEntIdPrVXmlChA.Clr();
  for (int DstEntN=0; DstEntN<LinkWgtDstEntIdPrV.Len(); DstEntN++){
    // prepare values
    int DstEntId=LinkWgtDstEntIdPrV[DstEntN].Val2;
    TStr DstEntNm=GetEntNm(DstEntId);
    TStr XmlDstEntNm=TXmlLx::GetXmlStrFromPlainStr(DstEntNm);
    int LinkWgt=LinkWgtDstEntIdPrV[DstEntN].Val1;
    // update vectors
    LinkWgtDstEntNmPrV.Add(TStrIntPr(DstEntNm, LinkWgt));
    if (DstEntN>0){LinkWgtDstEntIdPrVChA+=' ';}
    LinkWgtDstEntIdPrVChA+=TStr::Fmt("['%s':%d]", DstEntNm.CStr(), LinkWgt);
    LinkWgtDstEntIdPrVXmlChA+=TStr::Fmt("<Ent name=\"%s\" wgt=\"%d\"/>", XmlDstEntNm.CStr(), LinkWgt);
  }
}

void TTntBs::GetSchTmV(const TStr& SchNm, const uint64& PivotTm, TUInt64V& SchTmMSecsV){
  SchTmMSecsV.Clr();
  if (SchNm=="dayexp"){
    SchTmMSecsV.Add(PivotTm-0*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-1*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-2*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-4*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-8*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-16*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-32*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-64*TTmInfo::GetDayMSecs());
  } else 
  if (SchNm=="day"){
    SchTmMSecsV.Add(PivotTm-0*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-1*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-2*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-3*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-4*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-5*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-6*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-7*TTmInfo::GetDayMSecs());
  } else 
  if (SchNm=="week"){
    SchTmMSecsV.Add(PivotTm-0*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-7*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-14*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-21*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-28*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-35*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-42*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-49*TTmInfo::GetDayMSecs());
  } else 
  if (SchNm=="month"){
    SchTmMSecsV.Add(PivotTm-0*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-30*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-60*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-90*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-120*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-150*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-180*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-210*TTmInfo::GetDayMSecs());
  } else 
	  if (SchNm=="year"){
    SchTmMSecsV.Add(PivotTm-0*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-364*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-728*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-1092*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-1457*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-1821*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-2185*TTmInfo::GetDayMSecs());
    SchTmMSecsV.Add(PivotTm-2550*TTmInfo::GetDayMSecs());
  } else {
    FailR("Invalid Time Schedule");
  }
}

void TTntBs::Search(
 const PBowDocBs& TntBsBowDocBs, const PBowDocWgtBs& TntBsBowDocWgtBs, 
 const TStr& QTmStr, const TStr& QSchNm, 
 const TStr& QWcEntNm, const TStr& QKwStr,
 PTntRSet& TntRSet, const TStr& OutXmlFNm, const TStr& OutTxtFNm){
  // time-query
  TTm QTm=TTm::GetTmFromWebLogDateTimeStr(QTmStr);
  if (!QTm.IsDef()){
    uint64 MnDocTm; uint64 MxDocTm; GetMnMxDocTm(MnDocTm, MxDocTm);
    QTm=TTm::GetTmFromMSecs(MxDocTm);
  }
  TTm RawPivotTm=QTm;
  TStr RawPivotTmStr=RawPivotTm.GetWebLogDateTimeStr();
  TTm PivotTm=TTm::GetTmFromWebLogDateTimeStr(RawPivotTm.GetWebLogDateStr());
  TStr PivotTmStr=PivotTm.GetWebLogDateTimeStr();
  uint64 PivotTmMSecs=TTm::GetMSecsFromTm(PivotTm);
  TUInt64V SchTmMSecsV; GetSchTmV(QSchNm, PivotTmMSecs, SchTmMSecsV);
  // keyword-query
  TStr NrQKwStr=QKwStr.GetUc();

  // create result-set
  TntRSet=TTntRSet::New();
  TntRSet->PutQStrs(QTmStr, QSchNm, QWcEntNm, QKwStr);
  TntRSet->PutPivotTm(RawPivotTm, PivotTm);
    
  // get bow
  //PBowDocBs BowDocBs=GetBowDocBs(3, 5);
  //PBowDocBs BowDocBs=GetBowDocBs();
  //PBowDocWgtBs BowDocWgtBs=GetBowDocWgtBs(BowDocBs);

  // open files
  TFOut XmlFOut(OutXmlFNm); FILE* fXmlOut=XmlFOut.GetFileId();
  fprintf(fXmlOut, "<TntProfile>\n");
  TFOut TxtFOut(OutTxtFNm); FILE* fTxtOut=TxtFOut.GetFileId();
  // get docs-entities sorted vector
  TIntPrV DocsEntIdPrV; GetSorted_DocsEntIdPrV(DocsEntIdPrV);
  // traverse entities
  for (int EntN=0; EntN<DocsEntIdPrV.Len(); EntN++){
    int EntId=DocsEntIdPrV[EntN].Val2;
    TStr EntNm=GetEntNm(EntId);
    if (!QWcEntNm.Empty()&&(!EntNm.IsWcMatch(QWcEntNm))){continue;}

    // create entity-result-set
    PTntEntRSet EntRSet=TTntEntRSet::New();
    EntRSet->PutEntNm(EntNm);
    TntRSet->AddEntRSet(EntRSet);

    // output entity-header
    int EntDocs=DocsEntIdPrV[EntN].Val1;
    PTntEnt Ent=GetEnt(EntId);
    int LinkEnts=Ent->GetLinkEnts();
    fprintf(fXmlOut, "<Entity name=\"%s\" docs=\"%d\" ents=\"%d\">\n", EntNm.CStr(), EntDocs, LinkEnts);
    fprintf(fTxtOut, "'%s' [%d docs] [%d ents]\n", EntNm.CStr(), EntDocs, LinkEnts);

    // output docs over dates
    {TStrIntPrV DateStrDocsPrV; int _EntDocs;
    Ent->GetDocsPerDateV(this, DateStrDocsPrV, _EntDocs);
    fprintf(fXmlOut, "<DatesDocs docs=\"%d\">\n", _EntDocs);
    fprintf(fTxtOut, "   Docs per Date (%d docs):", _EntDocs);
    for (int DateN=0; DateN<DateStrDocsPrV.Len(); DateN++){
      TStr DateStr=DateStrDocsPrV[DateN].Val1;
      int Docs=DateStrDocsPrV[DateN].Val2;
      fprintf(fXmlOut, "  <DateDoc date=\"%s\" docs=\"%d\"/>\n", DateStr.CStr(), Docs);
      fprintf(fTxtOut, " [%s:%d]", DateStr.CStr(), Docs);
    }
    fprintf(fXmlOut, "</DatesDocs>\n");
    fprintf(fTxtOut, "\n");}

    fprintf(fXmlOut, "<TimeSlices start-time=\"%s\">\n", RawPivotTmStr.CStr());
    fprintf(fTxtOut, "   [Now: %s]\n", RawPivotTmStr.CStr());
    TIntPrV PrevLinkWgtDstEntIdPrV;
    TStrFltPrV PrevWordStrWgtPrV;
    for (int SchTmN=0; SchTmN<SchTmMSecsV.Len(); SchTmN++){
      // start-time
      uint64 StartTmMSecs=SchTmMSecsV[SchTmN];
      TStr StartTmStr=TTm::GetTmFromMSecs(StartTmMSecs).GetWebLogDateTimeStr();
      double PastDays=-1*((PivotTmMSecs-StartTmMSecs)/double(TTmInfo::GetDayMSecs()));
      // end-time
      TStr EndTmStr;
      if (SchTmN==0){EndTmStr=RawPivotTmStr;} 
      else {EndTmStr=TTm::GetTmFromMSecs(SchTmMSecsV[SchTmN-1]).GetWebLogDateTimeStr();}
      uint64 EndTmMSecs=TTm::GetMSecsFromTm(TTm::GetTmFromWebLogDateTimeStr(EndTmStr));
      // crate time-slice
      PTntEntRSet_TmSlice TmSlice=TTntEntRSet_TmSlice::New();
      TmSlice->PutStartEndTm(StartTmMSecs, EndTmMSecs);
      EntRSet->AddTmSlice(TmSlice);
      // open-time-point
      fprintf(fXmlOut, "<TimeSlice start-time=\"%s\" end-time=\"%s\" past-days=\"%.1f\">\n", 
       StartTmStr.CStr(), EndTmStr.CStr(), PastDays);
      // get linked entities
      TIntPrV LinkWgtDstEntIdPrV;
      Ent->GetSorted_LinkWgtDstEntIdPrV(StartTmMSecs, 0.9, LinkWgtDstEntIdPrV);
      fprintf(fXmlOut, "<SocialProfile>\n");
      // output linked entities
      int TopLinkEnts=LinkWgtDstEntIdPrV.Len();
      TStrIntPrV LinkWgtDstEntNmPrV; TChA LinkWgtDstEntIdPrVChA; TChA LinkWgtDstEntIdPrVXmlChA;
      GetLinkWgtDstEntNmPrV(LinkWgtDstEntIdPrV, 
       LinkWgtDstEntNmPrV, LinkWgtDstEntIdPrVChA, LinkWgtDstEntIdPrVXmlChA);
      TmSlice->PutLinkWgtDstEntNmPrV(LinkWgtDstEntNmPrV);
      fprintf(fXmlOut, "  <Entities ents=\"%d\">%s</Entities>\n", TopLinkEnts, LinkWgtDstEntIdPrVXmlChA.CStr());
      fprintf(fTxtOut, "      Entities (%d ents): %s\n", TopLinkEnts, LinkWgtDstEntIdPrVChA.CStr());
      // output difference between previous and current centroid
      if (SchTmN>0){
        TIntPrV NegDiffLinkWgtDstEntIdPrV; TIntPrV PosDiffLinkWgtDstEntIdPrV;
        GetLinkWgtDstEntIdPrVDiff(LinkWgtDstEntIdPrV, PrevLinkWgtDstEntIdPrV,
         NegDiffLinkWgtDstEntIdPrV, PosDiffLinkWgtDstEntIdPrV);
        // output positive change
        TStrIntPrV PosDiffLinkWgtDstEntNmPrV; TChA PosDiffLinkWgtDstEntIdPrVChA; TChA PosDiffLinkWgtDstEntIdPrVXmlChA;
        GetLinkWgtDstEntNmPrV(PosDiffLinkWgtDstEntIdPrV, 
         PosDiffLinkWgtDstEntNmPrV, PosDiffLinkWgtDstEntIdPrVChA, PosDiffLinkWgtDstEntIdPrVXmlChA);
        TmSlice->PutPosDiffLinkWgtDstEntNmPrV(PosDiffLinkWgtDstEntNmPrV);
        fprintf(fXmlOut, "  <RisingEntities>%s</RisingEntities>\n", PosDiffLinkWgtDstEntIdPrVXmlChA.CStr());
        fprintf(fTxtOut, "         Pos-Diff: %s\n", PosDiffLinkWgtDstEntIdPrVChA.CStr());
        // output negative change
        TStrIntPrV NegDiffLinkWgtDstEntNmPrV; TChA NegDiffLinkWgtDstEntIdPrVChA; TChA NegDiffLinkWgtDstEntIdPrVXmlChA;
        GetLinkWgtDstEntNmPrV(NegDiffLinkWgtDstEntIdPrV, 
         NegDiffLinkWgtDstEntNmPrV, NegDiffLinkWgtDstEntIdPrVChA, NegDiffLinkWgtDstEntIdPrVXmlChA);
        TmSlice->PutNegDiffLinkWgtDstEntNmPrV(NegDiffLinkWgtDstEntNmPrV);
        fprintf(fXmlOut, "  <FallingEntities>%s</FallingEntities>\n", NegDiffLinkWgtDstEntIdPrVXmlChA.CStr());
        fprintf(fTxtOut, "         Neg-Diff: %s\n", NegDiffLinkWgtDstEntIdPrVChA.CStr());
      }
      PrevLinkWgtDstEntIdPrV=LinkWgtDstEntIdPrV;
      fprintf(fXmlOut, "</SocialProfile>\n");
      // get text centroid
      int CtrDocs; TStrFltPrV WordStrWgtPrV;
      Ent->GetDocCentroid(this, TntBsBowDocBs, TntBsBowDocWgtBs, StartTmMSecs, NrQKwStr, 150, 0.9, CtrDocs, WordStrWgtPrV);
      fprintf(fXmlOut, "<ContentProfile>\n");
      // output centroid
      TChA WordStrWgtPrVChA; TChA WordStrWgtPrVXmlChA; 
      GetWordStrWgtPrV(WordStrWgtPrV, WordStrWgtPrVChA, WordStrWgtPrVXmlChA);
      TmSlice->PutWordStrWgtPrV(WordStrWgtPrV);
      fprintf(fXmlOut, "  <Keywords docs=\"%d\" kwords=\"%d\">%s</Keywords>\n", 
       CtrDocs, WordStrWgtPrV.Len(), WordStrWgtPrVXmlChA.CStr());
      fprintf(fTxtOut, "      Centroid (%d docs, %d words): %s\n",
       CtrDocs, WordStrWgtPrV.Len(), WordStrWgtPrVChA.CStr());
      // output difference between previous and current centroid
      if (SchTmN>0){
        TStrFltPrV NegDiffWordStrWgtPrV; TStrFltPrV PosDiffWordStrWgtPrV;
        GetWordStrWgtPrVDiff(WordStrWgtPrV, PrevWordStrWgtPrV,
         NegDiffWordStrWgtPrV, PosDiffWordStrWgtPrV);
        // output positive change
        TChA PosDiffWordStrWgtPrVChA; TChA PosDiffWordStrWgtPrVXmlChA; 
        GetWordStrWgtPrV(PosDiffWordStrWgtPrV, PosDiffWordStrWgtPrVChA, PosDiffWordStrWgtPrVXmlChA);
        TmSlice->PutPosDiffWordStrWgtPrV(PosDiffWordStrWgtPrV);
        fprintf(fXmlOut, "  <RisingKeywords>%s</RisingKeywords>\n", PosDiffWordStrWgtPrVXmlChA.CStr());
        fprintf(fTxtOut, "         Pos-Diff: %s\n", PosDiffWordStrWgtPrVChA.CStr());
        // output negative change
        TChA NegDiffWordStrWgtPrVChA; TChA NegDiffWordStrWgtPrVXmlChA; 
        GetWordStrWgtPrV(NegDiffWordStrWgtPrV, NegDiffWordStrWgtPrVChA, NegDiffWordStrWgtPrVXmlChA);
        TmSlice->PutNegDiffWordStrWgtPrV(NegDiffWordStrWgtPrV);
        fprintf(fXmlOut, "  <FallingKeywords>%s</FallingKeywords>\n", NegDiffWordStrWgtPrVXmlChA.CStr());
        fprintf(fTxtOut, "         Neg-Diff: %s\n", NegDiffWordStrWgtPrVChA.CStr());
      }
      PrevWordStrWgtPrV=WordStrWgtPrV;
      fprintf(fXmlOut, "</ContentProfile>\n");
      // close time-point
      fprintf(fXmlOut, "</TimeSlice>\n");
      fprintf(fTxtOut, "   [%.1f days: %s]\n", PastDays, StartTmStr.CStr());
    }
    fprintf(fXmlOut, "</TimeSlices>\n");

    // entity clustering
    TVec<TStrFltPrV> EntNmWgtPrVV;
    Ent->GetEntClustV(this, SchTmMSecsV.Last(), 100, 1000, 15, EntNmWgtPrVV);
    TntRSet->PutClust(EntNmWgtPrVV);
    fprintf(fXmlOut, "<SocialClusters>\n");
    for (int ClustN=0; ClustN<EntNmWgtPrVV.Len(); ClustN++){
      TStrFltPrV& EntNmWgtPrV=EntNmWgtPrVV[ClustN];
      fprintf(fXmlOut, "<SocialCluster num=\"%d\">\n", ClustN);
      fprintf(fTxtOut, "   Clust-%d:", ClustN);
      for (int EntN=0; EntN<EntNmWgtPrV.Len(); EntN++){
        TStr EntNm=EntNmWgtPrV[EntN].Val1;
        double Wgt=EntNmWgtPrV[EntN].Val2;
        fprintf(fXmlOut, "<Entity name=\"%s\" wgt=\"%.3f\"/>", EntNm.CStr(), Wgt);
        fprintf(fTxtOut, " ['%s':%.3f]", EntNm.CStr(), Wgt);
      }
      fprintf(fXmlOut, "</SocialCluster>\n");
      fprintf(fTxtOut, "\n");
    }
    fprintf(fXmlOut, "</SocialClusters>\n");
    // close
    fprintf(fXmlOut, "</Entity>\n");
    fprintf(fTxtOut, "\n");
  }
  // closing files
  fprintf(fXmlOut, "</TntProfile>\n");
}
