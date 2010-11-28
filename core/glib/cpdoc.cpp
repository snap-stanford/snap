/////////////////////////////////////////////////
// Compact-Document
TStr TCpDoc::GetTxtStr() const {
  TChA ChA;
  ChA+=GetTitleStr(); ChA+=". ";
  for (int ParN=0; ParN<GetPars(); ParN++){
    if (ParN>0){ChA+=' ';}
    ChA+=GetParStr(ParN);
  }
  return ChA;
}

TStr TCpDoc::GetHtmlStr() const {
  TChA ChA;
  ChA+="<HTML>\n";
  ChA+="<HEAD><TITLE>"; ChA+=GetTitleStr(); ChA+="</TITLE></HEAD>\n";
  ChA+="<BODY>\n";
  ChA+="<H1>"; ChA+=GetTitleStr(); ChA+="</H1>\n";
  for (int ParN=0; ParN<GetPars(); ParN++){
    ChA+="<P>";
    ChA+=GetParStr(ParN);
    ChA+="</P>\n";
  }
  ChA+="</BODY>\n";
  ChA+="</HTML>\n";
  return ChA;
}

TStr TCpDoc::GetAllParStr() const {
  TChA ChA;
  for (int ParN=0; ParN<GetPars(); ParN++){
    if (ParN>0){ChA+=' ';}
    ChA+=GetParStr(ParN);
  }
  return ChA;
}

PSIn TCpDoc::FFirstCpd(const TStr& FNm){
  PSIn SIn=TFIn::New(FNm);
  return SIn;
}

PSIn TCpDoc::FFirstCpd(const PSIn& SIn){
  return SIn;
}

bool TCpDoc::FNextCpd(const PSIn& SIn, PCpDoc& CpDoc){
  if (SIn->Eof()){
    CpDoc=NULL; return false;
  } else {
    CpDoc=TCpDoc::Load(*SIn); return true;
  }
}

PCpDoc TCpDoc::LoadHtmlDoc(const TStr& FNm){
  // create compact-document
  PCpDoc CpDoc=TCpDoc::New();
  // load document string
  PSIn HtmlSIn=TFIn::New(FNm);
  TStr HtmlStr=TStr::LoadTxt(HtmlSIn);
  CpDoc->DocNm=FNm;
  CpDoc->ParStrV.Add(HtmlStr);
  // return compact-document
  return CpDoc;
}

void TCpDoc::SaveHtmlToCpd(
 const TStr& InHtmlFPath, const TStr& OutCpdFNm,
 const bool& /*RecurseDirP*/, const int& MxDocs){
  // create output file
  PSOut SOut=TFOut::New(OutCpdFNm);
  // prepare file-directory traversal
  TStrV FPathV; FPathV.Add(InHtmlFPath);
  TStrV FExtV; FExtV.Add("html"); FExtV.Add("htm"); FExtV.Add("xml");
  TFFile FFile(FPathV, FExtV, "", true); TStr FNm;
  // traverse files
  printf("Processing '%s' ...\n", InHtmlFPath.CStr());
  int Docs=0;
  while (FFile.Next(FNm)){
    // get file-name
    if ((MxDocs!=-1)&&(FFile.GetFNmN()>MxDocs)){break;}
    Docs++; if (FFile.GetFNmN()%10==0){printf("%d\r", Docs);}
    // load html
    PCpDoc CpDoc=TCpDoc::LoadHtmlDoc(FNm);
    // save cpd document
    CpDoc->Save(*SOut);
  }
  printf("%d\nDone.\n", Docs);
}

void TCpDoc::SaveTBsToCpd(
 const TStr& InTBsFNm, const TStr& OutCpdFNm, const int& /*MxDocs*/){
  // open input text-base
  TStr TxtBsNm=InTBsFNm.GetFBase();
  TStr TxtBsFPath=InTBsFNm.GetFPath();
  PTxtBs TxtBs=TTxtBs::New(TxtBsNm, TxtBsFPath, faRdOnly);
  // create output file
  PSOut SOut=TFOut::New(OutCpdFNm);
  // traverse input documents
  TBlobPt TrvBlobPt=TxtBs->FFirstDocId(); TBlobPt DocId;
  int DocN=0; TStr DocNm; TStr DocStr;
  while (TxtBs->FNextDocId(TrvBlobPt, DocId)){
    DocN++; if (DocN%100==0){printf("%d docs\r", DocN);}
    // get document data
    TxtBs->GetDocNmStr(DocId, DocNm, DocStr);
    // create cpd document
    PCpDoc CpDoc=TCpDoc::New();
    CpDoc->DocNm=DocNm;
    CpDoc->ParStrV.Add(DocStr, 1);
    // save cpd document
    CpDoc->Save(*SOut);
  }
}

void TCpDoc::SaveLnDocToCpd(
 const TStr& LnDocFNm, const TStr& OutCpdFNm, const bool& NamedP, const int& MxDocs){
  printf("Saving Line-Document '%s' to '%s' ...\n", LnDocFNm.CStr(), OutCpdFNm.CStr());
  // create output file
  PSOut SOut=TFOut::New(OutCpdFNm);
  // open line-doc file
  TFIn FIn(LnDocFNm); char Ch=' '; int Docs=0;
  while (!FIn.Eof()){
    Docs++; if ((MxDocs!=-1)&&(Docs>=MxDocs)){break;}
    printf("%d\r", Docs);
    // document name
    TChA DocNm;
    if (NamedP){
      Ch=FIn.GetCh();
      while ((!FIn.Eof())&&(Ch!='\r')&&(Ch!='\n')&&(Ch!=' ')){
        DocNm+=Ch; Ch=FIn.GetCh();}
      DocNm.Trunc();
      if (DocNm.Empty()){Docs--; continue;}
    }
    // categories
    TStrV CatNmV;
    forever {
      while ((!FIn.Eof())&&(Ch==' ')){Ch=FIn.GetCh();}
      if (Ch=='!'){
        if (!FIn.Eof()){Ch=FIn.GetCh();}
        TChA CatNm;
        while ((!FIn.Eof())&&(Ch!='\r')&&(Ch!='\n')&&(Ch!=' ')){
          CatNm+=Ch; Ch=FIn.GetCh();}
        if (!CatNm.Empty()){CatNmV.Add(CatNm);}
      } else {
        break;
      }
    }
    // document text
    TChA DocChA;
    while ((!FIn.Eof())&&(Ch!='\r')&&(Ch!='\n')){
      DocChA+=Ch; Ch=FIn.GetCh();}
    // skip empty documents (empty lines)
    if (DocNm.Empty()&&DocChA.Empty()){
      continue;}
    // create & save cpd document
    PCpDoc CpDoc=TCpDoc::New();
    CpDoc->DocNm=DocNm;
    CpDoc->ParStrV.Add(DocChA, 1);
    for (int CatNmN=0; CatNmN<CatNmV.Len(); CatNmN++){
      CpDoc->TopCdNmV.Add(CatNmV[CatNmN]);}
    CpDoc->Save(*SOut);
  }
  printf("\nDone.\n");
}

const TStr TCpDoc::Reuters21578CpdFBase="Reuters21578.Cpd";

void TCpDoc::SaveReuters21578ToCpd(
 const TStr& InFPath, const TStr& OutCpdFNm, const int& MxDocs){
  // create output file
  PSOut SOut=TFOut::New(OutCpdFNm);
  // traverse directory with .sgm files
  TFFile FFile(InFPath, ".SGM", false); TStr FNm; int Docs=0;
  while (FFile.Next(FNm)){
    printf("Processing file '%s'\n", FNm.CStr());
    TXmlDocV LDocV; TXmlDoc::LoadTxt(FNm, LDocV);
    for (int LDocN=0; LDocN<LDocV.Len(); LDocN++){
      Docs++; if (Docs%100==0){printf("%d\r", Docs);}
      if ((MxDocs!=-1)&&(Docs>MxDocs)){break;}
      // create reuters document
      PCpDoc CpDoc=TCpDoc::New();
      // load xml document
      PXmlDoc Doc=LDocV[LDocN];
      PXmlTok DocTok=Doc->GetTok();
      // document id
      CpDoc->DocNm=DocTok->GetArgVal("NEWID");
      // date
      CpDoc->DateStr=Doc->GetTagTok("REUTERS|DATE")->GetTokStr(false);
      // document title
      PXmlTok TitleTok=Doc->GetTagTok("REUTERS|TEXT|TITLE");
      if (!TitleTok.Empty()){
        CpDoc->TitleStr=TitleTok->GetTokStr(false);}
      // dateline
      PXmlTok DatelineTok=Doc->GetTagTok("REUTERS|TEXT|DATELINE");
      if (!DatelineTok.Empty()){
        CpDoc->DatelineStr=DatelineTok->GetTokStr(false);}
      // get text string
      TStr TextStr;
      PXmlTok BodyTok=Doc->GetTagTok("REUTERS|TEXT|BODY");
      if (!BodyTok.Empty()){
        TextStr=BodyTok->GetTokStr(false);
      } else {
        // if <BODY> doesn't exist, take the whole <TEXT>
        PXmlTok TextTok=Doc->GetTagTok("REUTERS|TEXT");
        if (!TextTok.Empty()){
          TextStr=TextTok->GetTokStr(false);
        }
      }
      CpDoc->ParStrV.Add(TextStr, 1);
      // topic categories
      TXmlTokV TopCatTokV; Doc->GetTagTokV("REUTERS|TOPICS|D", TopCatTokV);
      for (int TokN=0; TokN<TopCatTokV.Len(); TokN++){
        TStr CatNm=TopCatTokV[TokN]->GetTokStr(false);
        CpDoc->TopCdNmV.Add(CatNm);
      }
      // save cpd document
      CpDoc->Save(*SOut);
    }
    if ((MxDocs!=-1)&&(Docs>MxDocs)){break;}
  }
}

const TStr TCpDoc::Reuters2000CpdFBase="Reuters2000.Cpd";

void TCpDoc::LoadReuters2000DocFromXml(const TStr& FNm,
 TStr& DocId, TStr& DateStr, TStr& TitleStr,
 TStr& HeadlineStr, TStr& BylineStr, TStr& DatelineStr,
 TStrV& ParStrV,
 TStrV& TopCdNmV, TStrV& GeoCdNmV, TStrV& IndCdNmV){
  PXmlDoc Doc=TXmlDoc::LoadTxt(FNm);
  // get text strings
  // general document data
  DocId=Doc->GetTagTok("newsitem")->GetArgVal("itemid");
  DateStr=Doc->GetTagTok("newsitem")->GetArgVal("date");
  TitleStr=Doc->GetTagTok("newsitem|title")->GetTokStr(false);
  HeadlineStr=Doc->GetTagTok("newsitem|headline")->GetTokStr(false);
  BylineStr=""; PXmlTok BylineTok;
  if (Doc->IsTagTok("newsitem|byline", BylineTok)){
    BylineStr=BylineTok->GetTokStr(false);}
  DatelineStr=""; PXmlTok DatelineTok;
  if (Doc->IsTagTok("newsitem|dateline", DatelineTok)){
    DatelineStr=DatelineTok->GetTokStr(false);}
  // text paragraphs
  ParStrV.Clr(); TXmlTokV ParTokV; Doc->GetTagTokV("newsitem|text|p", ParTokV);
  for (int ParTokN=0; ParTokN<ParTokV.Len(); ParTokN++){
    TStr ParStr=ParTokV[ParTokN]->GetTokStr(false);
    ParStrV.Add(ParStr);
  }
  // codes
  TopCdNmV.Clr(); GeoCdNmV.Clr(); IndCdNmV.Clr();
  TXmlTokV CdsTokV; Doc->GetTagTokV("newsitem|metadata|codes", CdsTokV);
  for (int CdsTokN=0; CdsTokN<CdsTokV.Len(); CdsTokN++){
    PXmlTok CdsTok=CdsTokV[CdsTokN];
    TXmlTokV CdTokV; CdsTok->GetTagTokV("code", CdTokV);
    if (CdsTok->GetArgVal("class")=="bip:topics:1.0"){
      for (int CdTokN=0; CdTokN<CdTokV.Len(); CdTokN++){
        TStr CdNm=CdTokV[CdTokN]->GetArgVal("code");
        TopCdNmV.Add(CdNm);
      }
    } else
    if (CdsTok->GetArgVal("class")=="bip:countries:1.0"){
      for (int CdTokN=0; CdTokN<CdTokV.Len(); CdTokN++){
        TStr CdNm=CdTokV[CdTokN]->GetArgVal("code");
        GeoCdNmV.Add(CdNm);
      }
    } else
    if (CdsTok->GetArgVal("class")=="bip:industries:1.0"){
      for (int CdTokN=0; CdTokN<CdTokV.Len(); CdTokN++){
        TStr CdNm=CdTokV[CdTokN]->GetArgVal("code");
        IndCdNmV.Add(CdNm);
      }
    } else {
      Fail;
    }
  }
}

PCpDoc TCpDoc::LoadReuters2000DocFromXml(const TStr& FNm){
  PCpDoc CpDoc=TCpDoc::New();
  LoadReuters2000DocFromXml(FNm,
   CpDoc->DocNm, CpDoc->DateStr, CpDoc->TitleStr,
   CpDoc->HeadlineStr, CpDoc->BylineStr, CpDoc->DatelineStr,
   CpDoc->ParStrV,
   CpDoc->TopCdNmV, CpDoc->GeoCdNmV, CpDoc->IndCdNmV);
  return CpDoc;
}

void TCpDoc::SaveReuters2000ToCpd(
 const TStr& InFPath, const TStr& OutCpdFNm, const int& MxDocs){
  // create output file 
  PSOut SOut=TFOut::New(OutCpdFNm);
  // processing xml files
  TStrStrH DocIdToDateStrH;
  TFFile FFile(InFPath, ".XML", true); TStr FNm;
  while (FFile.Next(FNm)){
    if ((MxDocs!=-1)&&(FFile.GetFNmN()>=MxDocs)){break;}
    if ((1+FFile.GetFNmN())%100==0){
      printf("Processing file '%s' (%d)\r", FNm.CStr(), 1+FFile.GetFNmN());}
    PXmlDoc Doc=TXmlDoc::LoadTxt(FNm);
    // get document
    PCpDoc CpDoc=TCpDoc::New();
    LoadReuters2000DocFromXml(FNm,
     CpDoc->DocNm, CpDoc->DateStr, CpDoc->TitleStr,
     CpDoc->HeadlineStr, CpDoc->BylineStr, CpDoc->DatelineStr,
     CpDoc->ParStrV,
     CpDoc->TopCdNmV, CpDoc->GeoCdNmV, CpDoc->IndCdNmV);
    // save cpd document
    CpDoc->Save(*SOut);
  }
  printf("\n");
}

const TStr TCpDoc::AcmTechNewsCpdFBase="AcmTechNews.Cpd";

void TCpDoc::SaveAcmTechNewsToCpd(
 const TStr& InFPath, const TStr& OutCpdFNm, const int& MxDocs){
  // create output file
  PSOut SOut=TFOut::New(OutCpdFNm);
  // processing xml files
  TFFile FFile(TStrV()+InFPath, TStrV()+".Html"+".Htm", "", true);
  TStr FNm; int Docs=0;
  while (FFile.Next(FNm)){
    printf("Processing file '%s'\r", FNm.CStr());
    PSIn SIn=TFIn::New(FNm);
    THtmlLx Lx(SIn);
    while (Lx.GetSym()!=hsyEof){
      //printf("%d\r", Docs);
      if ((MxDocs!=-1)&&(Docs>MxDocs)){break;}
      Lx.MoveToBTagOrEof("<SPAN>");
      if (Lx.GetArg("CLASS")!="title"){continue;}
      Lx.MoveToBTagOrEof("<A>");
      TStr TitleStr=Lx.GetStrToETag("<A>", false); TitleStr="";
      Lx.MoveToETagOrEof("<SPAN>");
      Lx.MoveToBTagOrEof("<P>");
      TStr ParStr=Lx.GetStrToETag("<P>", false);
      if (!ParStr.Empty()){
        Docs++;
        PCpDoc CpDoc=TCpDoc::New(TInt::GetStr(Docs), TitleStr, ParStr);
        CpDoc->Save(*SOut);
      }
    }
  }
  printf("\n");
}

const TStr TCpDoc::CiaWFBCpdFBase="CiaWFB.Cpd";

void TCpDoc::SaveCiaWFBToCpd(
 const TStr& InFPath, const TStr& OutCpdFNm, const int& MxDocs){
  // load CiaWFB documents into base
  PCiaWFBBs CiaWFBBs=TCiaWFBBs::LoadHtml(InFPath);
  // create output file
  PSOut SOut=TFOut::New(OutCpdFNm);
  // processing countries from base
  for (int CountryN=0; CountryN<CiaWFBBs->GetCountries(); CountryN++){
    PCiaWFBCountry CiaWFBCountry=CiaWFBBs->GetCountry(CountryN);
    TStr CountryNm=CiaWFBCountry->GetCountryNm();
    TStr DescStr=CiaWFBCountry->GetDescStr();
    PCpDoc CpDoc=TCpDoc::New(CountryNm, CountryNm, DescStr);
    CpDoc->Save(*SOut);
  }
}

const TStr TCpDoc::AsfaFBase="Asfa.Cpd";

void TCpDoc::SaveAsfaToCpd(const TStr& InFPath, const TStr& OutCpdFNm){
  // create output file
  PSOut SOut=TFOut::New(OutCpdFNm);
  // traverse files
  TStrH AccessionIdH;
  TFFile FFile(TStr::GetNrFPath(InFPath)+"*.Asfa"); TStr AsfaFNm;
  while (FFile.Next(AsfaFNm)){
    printf("Processing file '%s'\n", AsfaFNm.CStr());
    PSIn SIn=TFIn::New(AsfaFNm);
    TILx Lx(SIn, TFSet(iloRetEoln, iloExcept));
    Lx.GetSym(syLn, syEof);
    while (Lx.Sym!=syEof){
      // Query Line
      TStr QueryLnStr=Lx.Str;
      TStrV QueryStrV; QueryLnStr.SplitOnAllCh('\t', QueryStrV, false);
      IAssert(QueryStrV[0]=="Query");
      // RecordNo Line
      Lx.GetSym(syLn); TStr RecNoLnStr=Lx.Str;
      TStrV RecNoStrV; RecNoLnStr.SplitOnAllCh('\t', RecNoStrV, false);
      IAssert(RecNoStrV[0]=="RecordNo");
      //int RecN=RecNoStrV[1].GetInt();
      // fields (format: Short-Name Tab Long-Name Tab Value-String)
      TStr TitleStr, AbstractStr, PublicationYearStr, AccessionId;
      TStrV AuthorNmV; TStrV TermNmV1, TermNmV2;
      while (true){
        Lx.GetSym(syLn); TStr FldLnStr=Lx.Str;
        TStrV FldStrV; FldLnStr.SplitOnAllCh('\t', FldStrV, false);
        if (FldStrV[0]=="----"){
          if (!AccessionIdH.IsKey(AccessionId)){
            AccessionIdH.AddKey(AccessionId);
            // create & save cpd document
            PCpDoc CpDoc=TCpDoc::New();
            CpDoc->DocNm=AccessionId;
            CpDoc->DateStr=PublicationYearStr;
            CpDoc->TitleStr=TitleStr;
            CpDoc->ParStrV.Add(AbstractStr);
            CpDoc->TopCdNmV=TermNmV1;
            CpDoc->GeoCdNmV=TermNmV2;
            CpDoc->IndCdNmV=AuthorNmV;
            CpDoc->Save(*SOut);
          } else {/*printf("[%s]", AccessionId.CStr());*/}
          break;
        } else
        if (FldStrV[0]=="TI"){
          TitleStr=FldStrV[2];
        } else if (FldStrV[0]=="TI"){
          TitleStr=FldStrV[2];
        } else if (FldStrV[0]=="AU"){
          FldStrV[2].SplitOnAllCh(';', AuthorNmV);
          for (int StrN=0; StrN<AuthorNmV.Len(); StrN++){AuthorNmV[StrN].ToTrunc();}
        } else if (FldStrV[0]=="AB"){
          AbstractStr=FldStrV[2];
        } else if (FldStrV[0]=="PY"){
          PublicationYearStr=FldStrV[2];
        } else if (FldStrV[0]=="DE"){
          FldStrV[2].SplitOnAllCh(';', TermNmV1);
          for (int StrN=0; StrN<TermNmV1.Len(); StrN++){TermNmV1[StrN].ToTrunc();}
        } else if (FldStrV[0]=="CL"){
          FldStrV[2].SplitOnAllCh(';', TermNmV2);
          for (int StrN=0; StrN<TermNmV2.Len(); StrN++){TermNmV2[StrN].ToTrunc();}
        } else if (FldStrV[0]=="AN"){
          AccessionId=FldStrV[2];
        }
      }
      printf("%d\r", AccessionIdH.Len());
      Lx.GetSym(syLn, syEof);
    }
  }
}

void TCpDoc::SaveForNlpWin(const PSOut& SOut){
  SOut->PutStr("DocId:");
  SOut->PutStr(DocNm); SOut->PutLn();
  SOut->PutStr(HeadlineStr); SOut->PutLn();
  for (int ParStrN=0; ParStrN<ParStrV.Len(); ParStrN++){
    SOut->PutStr(ParStrV[ParStrN]); SOut->PutLn();
  }
}


