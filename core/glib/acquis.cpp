/////////////////////////////////////////////////
// Acquis-Base
void TAcquisBs::SaveAcquisToCpd(
 const TStr& InAcquisFPath, const TStrV& AcquisLangNmV, const TStr& OutCpdFNm){
  // create output file
  printf("Saving to %s ...\n", OutCpdFNm.CStr());
  PSOut SOut=TFOut::New(OutCpdFNm);
  // traverse language-directories
  TFFile FPathFFile(TStr::GetNrFPath(InAcquisFPath)+"*"); TStr FPath;
  while (FPathFFile.Next(FPath)){
    if (!FPathFFile.IsDir()){continue;}
    TStr LangNm=FPath.GetFBase();
    if (!AcquisLangNmV.Empty()&&(!AcquisLangNmV.IsIn(LangNm))){continue;}
    printf("%s\n", FPath.CStr());
    // traverse year-directories
    TFFile YearFPathFFile(TStr::GetNrFPath(FPath)+"*"); TStr YearFPath;
    while (YearFPathFFile.Next(YearFPath)){
      if (!YearFPathFFile.IsDir()){continue;}
      TStr YearStr=YearFPath.GetFBase();
      printf("%s\n", YearFPath.CStr());
      // traverse acquis-documents
      TFFile FFile(TStr::GetNrFPath(YearFPath)+"*", true); TStr XmlFNm;
      while (FFile.Next(XmlFNm)){
        //printf("%s\n", XmlFNm.CStr());
        // read document
        PXmlDoc XmlDoc=TXmlDoc::LoadTxt(XmlFNm);
        IAssert(XmlDoc->IsOk());
        // extract document-name
        TStr DocNm=XmlDoc->GetTok()->GetArgVal("id");
        // extract eurovoc-categories
        TStrV EuroVocCatNmV;
        PXmlTok EuroVocXmlTok=XmlDoc->GetTagTok("TEI.2|teiHeader|profileDesc|textClass");
        if (!EuroVocXmlTok.Empty()){
          for (int TokN=0; TokN<EuroVocXmlTok->GetSubToks(); TokN++){
            PXmlTok SubXmlTok=EuroVocXmlTok->GetSubTok(TokN);
            if (SubXmlTok->IsTag("classCode")){
              IAssert(SubXmlTok->GetArgVal("scheme")=="eurovoc");
              TStr EuroVocCatNm=SubXmlTok->GetTagVal(SubXmlTok->GetTagNm(), false);
              EuroVocCatNmV.Add(EuroVocCatNm);
            }
          }
        }
        // extract paragraphs
        TStrV ParStrV; TStr TitleStr;
        PXmlTok BodyXmlTok=XmlDoc->GetTagTok("TEI.2|text|body");
        for (int TokN=0; TokN<BodyXmlTok->GetSubToks(); TokN++){
          PXmlTok SubXmlTok=BodyXmlTok->GetSubTok(TokN);
          if (SubXmlTok->IsTag("head")||SubXmlTok->IsTag("p")){
            if (SubXmlTok->IsTag("head")){
              TitleStr=SubXmlTok->GetTagVal(SubXmlTok->GetTagNm(), false);}
            int ParN=SubXmlTok->GetIntArgVal("n", -1);
            if ((ParStrV.Len()==0)&&(ParN==2)){ParStrV.Add();}
            IAssert((ParN!=-1)||(ParN-1==ParStrV.Len()));
            TStr ParStr=SubXmlTok->GetTagVal(SubXmlTok->GetTagNm(), false);
            ParStrV.Add(ParStr);
          }
        }
        // create & save cpd document
        PCpDoc CpDoc=TCpDoc::New();
        CpDoc->DocNm=DocNm;
        CpDoc->DateStr=YearStr;
        CpDoc->TitleStr=TitleStr;
        CpDoc->ParStrV=ParStrV;
        CpDoc->TopCdNmV=EuroVocCatNmV;
        CpDoc->Save(*SOut);
      }
    }
  }
  printf("\nDone.\n");
}
