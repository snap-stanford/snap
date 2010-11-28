/////////////////////////////////////////////////
// Includes
#include "wordco.h"

/////////////////////////////////////////////////
// Word-Co-occurrence-Base
void TWCoBs::AssertOk() const {
  int Words=GetWords();
  for (int WordN=0; WordN<Words; WordN++){
    // get word string & frequency
    TStr WordStr=GetWordStr(WordN);
    int WordFq=GetWordFq(WordN);
    // get word
    PWCoWord Word=GetWord(WordN);
    // check components
    IAssert(WordN==Word->WordN);
    IAssert(WordStr==Word->WordStr);
    IAssert(WordFq==Word->Fq);
    // check related words
    int RelWords=Word->GetRelWords();
    int SumRelWordFq=0;
    for (int RelWordN=0; RelWordN<RelWords; RelWordN++){
      // get related word string & frequency
      TStr RelWordStr=Word->GetRelWordStr(RelWordN);
      int RelWordFq=Word->GetRelWordFq(RelWordN);
      // get word-id
      int NewRelWordN=GetWordN(RelWordStr);
      IAssert(RelWordStr==GetWordStr(NewRelWordN));
      // add to summary frequence
      SumRelWordFq+=RelWordFq;
    }
    IAssert(WordFq==SumRelWordFq);
  }
}

PBowDocBs TWCoBs::GetBowDocBs() const {
  // prepare document set
  PBowDocBs BowDocBs=TBowDocBs::New();
  // document & word definition
  int Words=GetWords();
  for (int WordN=0; WordN<Words; WordN++){
    // get word string & frequency
    TStr WordStr=GetWordStr(WordN);
    int WordFq=GetWordFq(WordN);
    // document definition
    BowDocBs->DocNmH.AddKey(WordStr);
    // word definition
    int WId=BowDocBs->WordStrToDescH.AddKey(WordStr);
    BowDocBs->WordStrToDescH[WId].Fq=WordFq;
  }
  // train data
  {for (int WordN=0; WordN<Words; WordN++){
    // get word string
    TStr WordStr=GetWordStr(WordN);
    /// get doc-id
    int DId=BowDocBs->GetDId(WordStr);
    // add doc-id to training documents
    BowDocBs->TrainDIdV.Add(DId);
    // create sparse vector
    PBowSpV SpV=TBowSpV::New(DId);
    int NewDId=BowDocBs->DocSpVV.Add(SpV);
    EAssert(DId==NewDId, "Document-Ids don't match.");
    PWCoWord Word=GetWord(WordN);
    int RelWords=Word->GetRelWords();
    for (int RelWordN=0; RelWordN<RelWords; RelWordN++){
      // get related word string & frequency
      TStr RelWordStr=Word->GetRelWordStr(RelWordN);
      int RelWordFq=Word->GetRelWordFq(RelWordN);
      // get word-id
      int RelWId=BowDocBs->GetWId(RelWordStr);
      // add word-id/freq pair to vector
      SpV->AddWIdWgt(RelWId, RelWordFq);
    }
    SpV->Trunc();
  }}
  // check correctness
  BowDocBs->AssertOk();
  // return result
  return BowDocBs;
}

void TWCoBs::GetNlpWinWordRel(const PXmlTok& Tok, const TStr& NodeTypeNm,
 const int& Lev, TStrV& HeadStrV, TStrPrV& WordStrPrV, TStrTrV& WordStrTrV){
  HeadStrV.Clr();
  if (Tok->IsTag("Head")){
    PXmlTok LemmaTok=Tok->GetTagTok("node|Lemma");
    if (LemmaTok.Empty()){/*printf("***");*/ return;}
    TStr HeadStr=LemmaTok->GetTokStr(false);
    TStr NodeTypeNm=Tok->GetTagTok("node")->GetArgVal("Nodetype");
    if (NodeTypeNm=="PREP"){HeadStr="";}
    else if (NodeTypeNm=="CONJ"){HeadStr="";}
    else if ((HeadStr=="the")||(HeadStr=="a")){HeadStr="";}
    else if ((HeadStr=="this")||(HeadStr=="it")){HeadStr="";}
    else if ((HeadStr=="be")||(HeadStr=="will")){HeadStr="";}
    else if (HeadStr.IsFlt()){HeadStr="_NUM";}
    else if ((HeadStr.Len()>0)&&(TCh::IsNum(HeadStr[0]))){HeadStr="_NUM";}
    else if (Tok->IsTagTok("node|Factrecs")){
      PXmlTok FactTok=Tok->GetTagTok("node|Factrecs");
      for (int SubTokN=0; SubTokN<FactTok->GetSubToks(); SubTokN++){
        PXmlTok SubTok=FactTok->GetSubTok(SubTokN);
        if (!SubTok->IsTag("node")){continue;}
        TStr NodeTypeNm=SubTok->GetArgVal("Nodetype");
        /*if (NodeTypeNm=="ADJ"){continue;}
        if ((NodeTypeNm=="NAME")||(NodeTypeNm=="FIRSTNAME")||(NodeTypeNm=="LASTNAME")||(NodeTypeNm=="TITLE")){HeadStr="_NAME";}
        else if ((NodeTypeNm=="MONTH")||(NodeTypeNm=="DAY")||(NodeTypeNm=="DATE")){HeadStr="_DATE";}
        else if ((NodeTypeNm=="AMOUNT")||(NodeTypeNm=="MONEY")){HeadStr="_AMOUNT";}
        else if ((NodeTypeNm=="RANGE")||(NodeTypeNm=="PORTION")){HeadStr="_AMOUNT";}
        else if ((NodeTypeNm=="NBR")||(NodeTypeNm=="NUMERTR")||(NodeTypeNm=="QUANT")){HeadStr="_NUM";}
        else if ((NodeTypeNm=="CAPTOID")||((NodeTypeNm=="COMPNAME"))){HeadStr="_NAME";}
        else if ((NodeTypeNm=="PLACE")||(NodeTypeNm=="PLACETYPE")||(NodeTypeNm=="PLACENAME")){HeadStr="_GEONAME";}
        else if ((NodeTypeNm=="STNAME")||(NodeTypeNm=="CITY")){HeadStr="_GEONAME";}
        else if ((NodeTypeNm=="STATE")||(NodeTypeNm=="COUNTRY")){HeadStr="_GEONAME";}
        else {/*printf("'%s'", NodeTypeNm());}*/
        if ((!HeadStr.Empty())&&(HeadStr[0]=='_')){break;}
      }
    }
    //if (!HeadStr.Empty()){HeadStrV.Add(HeadStr);}
    if (!HeadStr.Empty()){HeadStrV.Add(HeadStr+":"+NodeTypeNm);}
  } else {
    TStr NodeTypeNm;
    if (Tok->IsTag("node")){
      //if (!Tok->IsArg("Nodetype")){printf("***");}
      NodeTypeNm=Tok->GetArgVal("Nodetype", "");}
    int HeadTagN=-1;
    int SubToks=Tok->GetSubToks();
    for (int SubTokN=0; SubTokN<SubToks; SubTokN++){
      PXmlTok SubTok=Tok->GetSubTok(SubTokN);
      TStr SubTokStr=SubTok->GetStr();
      if (!SubTok->IsTag()){continue;}
      TStrV SubHeadStrV;
      GetNlpWinWordRel(SubTok, NodeTypeNm, Lev+1, SubHeadStrV, WordStrPrV, WordStrTrV);
      if (!SubHeadStrV.Empty()){
        HeadStrV.AddV(SubHeadStrV);
        if (SubTok->IsTag("Head")){
          IAssert(HeadTagN==-1);
          IAssert(SubHeadStrV.Len()==1);
          HeadTagN=HeadStrV.Len()-1;
        }
      }
    }
    if (HeadTagN!=-1){
      TStr HeadStr=HeadStrV[HeadTagN];
      // X R Y
      for (int PrModN=0; PrModN<HeadTagN; PrModN++){
        for (int PsModN=HeadTagN+1; PsModN<HeadStrV.Len(); PsModN++){
          TStrTr WordStrTr(HeadStrV[PrModN].CStr(), HeadStr.CStr(), HeadStrV[PsModN]);
          WordStrTrV.Add(WordStrTr);
        }
      }
      // X R and R Y
      {for (int PrModN=0; PrModN<HeadTagN; PrModN++){
        TStrPr WordStrPr(HeadStr, HeadStrV[PrModN]);
        WordStrPrV.Add(WordStrPr);
      }}
      for (int PsModN=HeadTagN+1; PsModN<HeadStrV.Len(); PsModN++){
        TStrPr WordStrPr(HeadStr, HeadStrV[PsModN]);
        WordStrPrV.Add(WordStrPr);
      }
      // assign new head
      HeadStrV.Clr(); HeadStrV.Add(HeadStr);
    }
  }
}

void TWCoBs::GetNlpWinWordRel(const PXmlDoc& NlpWinXmlDoc,
 TStr& SentStr, TStrPrV& WordStrPrV, TStrTrV& WordStrTrV){
  SentStr=NlpWinXmlDoc->GetTagTokStr("MSNLP|Parse|Portrait|node|String");
  //printf("\n%s\n", SentStr.CStr());
  TStrV HeadStrV; WordStrPrV.Clr(); WordStrTrV.Clr();
  GetNlpWinWordRel(NlpWinXmlDoc->GetTok(), "", 0, HeadStrV, WordStrPrV, WordStrTrV);
  // write pairs
  for (int PrN=0; PrN<WordStrPrV.Len(); PrN++){
    TStr WordStr1=WordStrPrV[PrN].Val1;
    TStr WordStr2=WordStrPrV[PrN].Val2;
    //printf("RX: '%s' -> '%s'\n", WordStr1.CStr(), WordStr2.CStr());
  }
  // write triples
  for (int TrN=0; TrN<WordStrTrV.Len(); TrN++){
    TStr WordStr1=WordStrTrV[TrN].Val1;
    TStr WordStr2=WordStrTrV[TrN].Val2;
    TStr WordStr3=WordStrTrV[TrN].Val3;
    //printf("XRY: '%s' <- '%s' -> '%s'\n",
    // WordStr1.CStr(), WordStr2.CStr(), WordStr3.CStr());
  }
}

PXmlDoc TWCoBs::GetNextNlpWinXmlDoc(const PSIn& NlpWinXmlSIn){
  forever {
    while ((!NlpWinXmlSIn->Eof())&&(NlpWinXmlSIn->GetCh()!='>')){}
    if (NlpWinXmlSIn->Eof()){return NULL;}
    if (NlpWinXmlSIn->GetCh()!='>'){continue;}
    if (NlpWinXmlSIn->GetCh()!='>'){continue;}
    while (NlpWinXmlSIn->GetCh()!='\n'){}
    if ((NlpWinXmlSIn->GetCh())==TCh::NullCh){continue;}
    while (NlpWinXmlSIn->GetCh()!='\n'){}
    while (NlpWinXmlSIn->GetCh()!='\n'){}
    // read xml document
    PXmlDoc NlpWinXmlDoc=TXmlDoc::LoadTxt(NlpWinXmlSIn);
    // return xml-doc if ok or return NULL if eof
    if (NlpWinXmlDoc->IsOk()){return NlpWinXmlDoc;}
    else if (NlpWinXmlSIn->Eof()){return NULL;}
  }
}

PWCoBs TWCoBs::LoadNlpWinTxt(const TStr& FNm, const int& MxTrees){
  // open file with NlpWin trees
  PSIn NlpWinXmlSIn=TFIn::New(FNm);
  // traverse trees
  TDbStrIntH WordDbStrToFqH; int NlpWinXmlN=0;
  forever {
    while ((!NlpWinXmlSIn->Eof())&&(NlpWinXmlSIn->GetCh()!='>')){}
    if (NlpWinXmlSIn->Eof()){break;}
    if (NlpWinXmlSIn->GetCh()!='>'){continue;}
    if (NlpWinXmlSIn->GetCh()!='>'){continue;}
    while (NlpWinXmlSIn->GetCh()!='\n'){}
    if ((NlpWinXmlSIn->GetCh())==TCh::NullCh){continue;}
    while (NlpWinXmlSIn->GetCh()!='\n'){}
    while (NlpWinXmlSIn->GetCh()!='\n'){}
    PXmlDoc NlpWinXmlDoc=TXmlDoc::LoadTxt(NlpWinXmlSIn);
    if (NlpWinXmlDoc->IsOk()){
      TStr SentStr; TStrPrV WordStrPrV; TStrTrV WordStrTrV;
      GetNlpWinWordRel(NlpWinXmlDoc, SentStr, WordStrPrV, WordStrTrV);
      for (int PrN=0; PrN<WordStrPrV.Len(); PrN++){
        TStr WordStr1=WordStrPrV[PrN].Val1;
        TStr WordStr2=WordStrPrV[PrN].Val2;
        if (WordStr1>WordStr2){
          TStr SwapWordStr=WordStr1; WordStr1=WordStr2; WordStr2=SwapWordStr;}
        WordDbStrToFqH.AddDat(TDbStr(WordStr1))++;
        if (WordStr1!=WordStr2){
          WordDbStrToFqH.AddDat(TDbStr(WordStr2))++;}
        WordDbStrToFqH.AddDat(TDbStr(WordStr1, WordStr2))++;
      }
      NlpWinXmlN++; if ((MxTrees!=-1)&&(NlpWinXmlN>MxTrees)){break;}
      printf("%d\r", NlpWinXmlN);
    } else {
      printf("#");
    }
  }
  printf("\n");
  // create wco base
  PWCoBs WCoBs=TWCoBs::New();
  // collect words
  TStrIntPrV& WordStrFqPrV=WCoBs->WordStrFqPrV;
  for (int WordStrP=0; WordStrP<WordDbStrToFqH.Len(); WordStrP++){
    TDbStr& WordDbStr=WordDbStrToFqH.GetKey(WordStrP);
    int Fq=WordDbStrToFqH[WordStrP];
    if (WordDbStr.Str2.Empty()){
      WordStrFqPrV.Add(TStrIntPr(WordDbStr.Str1, Fq));}
  }
  WordStrFqPrV.Sort();
  // collect cooccurrences
  for (int WordStrN=0; WordStrN<WordStrFqPrV.Len(); WordStrN++){
    // get word & frequency
    TStr WordStr=WordStrFqPrV[WordStrN].Val1;
    int Fq=WordStrFqPrV[WordStrN].Val2;
    printf("%d/%d\r", WordStrN, WordStrFqPrV.Len());
    // get related words
    TStrIntPrV RelWordStrFqPrV;
    for (int WordStrP=0; WordStrP<WordDbStrToFqH.Len(); WordStrP++){
      TDbStr& WordDbStr=WordDbStrToFqH.GetKey(WordStrP);
      int Fq=WordDbStrToFqH[WordStrP];
      TStr SubWordStr;
      if (WordDbStr.Str1==WordStr){SubWordStr=WordDbStr.Str2;}
      else if (WordDbStr.Str2==WordStr){SubWordStr=WordDbStr.Str1;}
      if (!SubWordStr.Empty()){
        RelWordStrFqPrV.Add(TStrIntPr(SubWordStr, Fq));}
    }
    RelWordStrFqPrV.Sort();
    // create wco word
    PWCoWord WCoWord=TWCoWord::New();
    WCoWord->WordN=WordStrN;
    WCoWord->WordStr=WordStr;
    WCoWord->Fq=Fq;
    WCoWord->RelWordStrFqPrV=RelWordStrFqPrV;
    WCoBs->WordStrToWCoWordH.AddDat(WordStr, WCoWord);
  }
  // check correctness
  WCoBs->AssertOk();
  // return wco base
  return WCoBs;
}

PWCoBs TWCoBs::LoadNlpWinBinOrTxt(const TStr& BinFNm, const TStr& TxtFNm){
  printf("Loading files ...\n");
  PWCoBs WCoBs;
  if (TFile::Exists(BinFNm)){
    WCoBs=LoadBin(BinFNm);
  } else {
    WCoBs=LoadNlpWinTxt(TxtFNm);
    WCoBs->SaveBin(BinFNm);
  }
  printf("Done.\n");
  WCoBs->AssertOk();
  return WCoBs;
}

void TWCoBs::SaveTxt(const TStr& FNm) const {
  TFOut SOut(FNm); TFileId FId=SOut.GetFileId();
  for (int WordStrN=0; WordStrN<WordStrFqPrV.Len(); WordStrN++){
    TStr WordStr=WordStrFqPrV[WordStrN].Val1;
    int Fq=WordStrFqPrV[WordStrN].Val2;
    printf("%d/%d\r", WordStrN, WordStrFqPrV.Len());
    fprintf(FId, "%s[%d]:", WordStr.CStr(), Fq);
    TStrIntPrV& RelWordStrFqPrV=
     WordStrToWCoWordH.GetDat(WordStr)->RelWordStrFqPrV;
    for (int RelWordStrN=0; RelWordStrN<RelWordStrFqPrV.Len(); RelWordStrN++){
      TStr RelWordStr=RelWordStrFqPrV[RelWordStrN].Val1;
      int Fq=RelWordStrFqPrV[RelWordStrN].Val2;
      fprintf(FId, " %s[%d]", RelWordStr.CStr(), Fq);
    }
    fprintf(FId, "\n");
  }
}

void TWCoBs::SaveXml(const TStr& FNm) const {
  TFOut SOut(FNm); TFileId FId=SOut.GetFileId();
  fprintf(FId, "<WordCo>\n");
  for (int WordStrN=0; WordStrN<WordStrFqPrV.Len(); WordStrN++){
    TStr WordStr=WordStrFqPrV[WordStrN].Val1;
    int Fq=WordStrFqPrV[WordStrN].Val2;
    printf("%d/%d\r", WordStrN, WordStrFqPrV.Len());
    fprintf(FId, "<Word Str=\"%s\" Fq=\"%d\">", WordStr.CStr(), Fq);
    TStrIntPrV& RelWordStrFqPrV=
     WordStrToWCoWordH.GetDat(WordStr)->RelWordStrFqPrV;
    for (int RelWordStrN=0; RelWordStrN<RelWordStrFqPrV.Len(); RelWordStrN++){
      TStr RelWordStr=RelWordStrFqPrV[RelWordStrN].Val1;
      int Fq=RelWordStrFqPrV[RelWordStrN].Val2;
      fprintf(FId, " <RelWord Str=\"%s\" Fq=\"%d\"/>", RelWordStr.CStr(), Fq);
    }
    fprintf(FId, " </Word>\n");
  }
  fprintf(FId, "</WordCo>\n");
}

