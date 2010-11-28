/////////////////////////////////////////////////
// Includes
#include "nyta.h"

/////////////////////////////////////////////////
// NYT-Archive-Base
void TNytaBs::LoadGeoNames(const TStr& FPath){
  printf("Loading Geonames from '%s' ... ", FPath.CStr());
  // countries
  TStr CountriesFNm=TStr::GetNrFPath(FPath)+"countryInfo.txt";
  PSs CountriesSs=TSs::LoadTxt(ssfTabSep, CountriesFNm, NULL, false, -1, TIntV(), false);
  TStrStrH CountryCdToNmH;
  for (int Y=38; Y<CountriesSs->GetYLen(); Y++){
    // extract components
    TStr CountryCdNm=CountriesSs->GetVal(0, Y);
    TStr CountryNm=CountriesSs->GetVal(4, Y);
    TStr ContinetCdNm=CountriesSs->GetVal(8, Y);
    // save country-code to country-name relationship
    CountryCdToNmH.AddDat(CountryCdNm, CountryNm);
    // convert continent-code to continent-name
    TStr ContinentNm=ContinetCdNm;
    if (ContinetCdNm=="AF"){ContinentNm="Africa";}
    if (ContinetCdNm=="AS"){ContinentNm="Asia";}
    if (ContinetCdNm=="EU"){ContinentNm="Europe";}
    if (ContinetCdNm=="NA"){ContinentNm="North America";}
    if (ContinetCdNm=="OC"){ContinentNm="Oceania";}
    if (ContinetCdNm=="SA"){ContinentNm="South America";}
    if (ContinetCdNm=="AN"){ContinentNm="Antarctica";}
    // create token list
    TStrV CountryNmTokStrV; THtmlLx::GetTokStrV(CountryNm, CountryNmTokStrV);
    // add topic
    AddTopicNm(CountryNmTokStrV, "gn:Country", TStr("gn:in-continent:")+ContinetCdNm);
  }

  // cities
  TStr CitiesFNm=TStr::GetNrFPath(FPath)+"cities15000.txt";
  PSs CitiesSs=TSs::LoadTxt(ssfTabSep, CitiesFNm, NULL, false, -1, TIntV(), false);
  for (int Y=0; Y<CitiesSs->GetYLen(); Y++){
    // extract components
    TStr CityNm=CitiesSs->GetVal(2, Y);
    TStr GeoFeatureCdNm=CitiesSs->GetVal(7, Y);
    TStr CountryCdNm=CitiesSs->GetVal(8, Y);
    // convert country-code to country-name
    TStr CountryNm=CountryCdNm;
    if (CountryCdToNmH.IsKey(CountryCdNm)){
      CountryNm=CountryCdToNmH.GetDat(CountryCdNm);}
    // create token list
    TStrV CityNmTokStrV; THtmlLx::GetTokStrV(CityNm, CityNmTokStrV);
    // add topic
    AddTopicNm(CityNmTokStrV, "gn:City",
     TStr("gn:feature:")+GeoFeatureCdNm, TStr("gn:in-country:")+CountryNm);
  }

  // other geo-entities
  TStr GeoEntityFNm=TStr::GetNrFPath(FPath)+"null.txt";
  PSs GeoEntitySs=TSs::LoadTxt(ssfTabSep, GeoEntityFNm, NULL, false, -1, TIntV(), false);
  for (int Y=0; Y<GeoEntitySs->GetYLen(); Y++){
    // extract components
    TStr GeoEntityNm=GeoEntitySs->GetVal(2, Y);
    TStr GeoFeatureCdNm=GeoEntitySs->GetVal(7, Y);
    TStr CountryNm=GeoEntitySs->GetVal(8, Y);
    // convert country-code to country-name
    TStr CountryCdNm=CountryNm;
    if (CountryCdToNmH.IsKey(CountryCdNm)){
      CountryNm=CountryCdToNmH.GetDat(CountryCdNm);}
    // create token list
    TStrV GeoEntityNmTokStrV; THtmlLx::GetTokStrV(GeoEntityNm, GeoEntityNmTokStrV);
    // add topic
    if (CountryNm.Empty()){
      AddTopicNm(GeoEntityNmTokStrV, "gn:GeoEntity",
       TStr("gn:feature:")+GeoFeatureCdNm);
    } else {
      AddTopicNm(GeoEntityNmTokStrV, "gn:GeoEntity",
       TStr("gn:feature:")+GeoFeatureCdNm, TStr("gn:in-country:")+CountryNm);
    }
  }
  printf("Done.\n");
}

void TNytaBs::LoadNytTopics(const TStr& FPath){
  printf("Loading NYT-Topics from '%s' ...\n", FPath.CStr());
  TFFile FFile(TStr::GetNrFPath(FPath)+"*.html"); TStr FNm;
  while (FFile.Next(FNm)){
    printf("  Processing '%s' ...", FNm.CStr());
    PSIn SIn=TFIn::New(FNm);
    THtmlLx Lx(SIn);
    Lx.GetSym();
    while (Lx.Sym!=hsyEof){
      Lx.MoveToBTagArg2OrEof("<FONT>", "COLOR", "#000099", "SIZE", "2");
      if (Lx.Sym!=hsyEof){
        // extract topic-type
        Lx.GetSym(); IAssert((Lx.Sym==hsyBTag)&&(Lx.UcChA=="<A>"));
        TStr UrlStr=Lx.GetArg("HREF");
        PUrl Url=TUrl::New(UrlStr);
        TStr TopicTagNm=Url->GetPathSeg(3);
        if (TopicTagNm=="diseasesconditionsandhealthtopics"){TopicTagNm="health";}
        TopicTagNm=TStr("nytt:")+TopicTagNm;
        // extract topic-name
        TStr TopicNm=Lx.GetStrToETag("<FONT>", true);
        TopicNm.ToTrunc();
        TopicNm=TXmlLx::GetXmlStrFromPlainStr(TopicNm);
        // remove content within parenthesis
        if (TopicNm.IsStrIn(" (")){
          int BChN=TopicNm.SearchStr(" (");
          int EChN=TopicNm.SearchStr(")");
          TopicNm.DelSubStr(BChN, EChN);
        }
        // reverse comma separated parts
        if (TopicNm.IsStrIn(", ")){
          TStr LeftStr; TStr RightStr;
          TopicNm.SplitOnStr(LeftStr, ", ", RightStr);
          if (RightStr=="Inc."){
            TopicNm=LeftStr+" "+RightStr;
          } else {
            TopicNm=RightStr+" "+LeftStr;
          }
        }
        // create token-string-vector
        PSIn TopicNmSIn=TStrIn::New(TopicNm);
        THtmlLx TopicNmLx(TopicNmSIn);
        TopicNmLx.GetSym(); IAssert(TopicNmLx.Sym!=hsyEof);
        TStrV TopicNmTokStrV;
        while (TopicNmLx.Sym!=hsyEof){
          TopicNmTokStrV.Add(TopicNmLx.ChA);
          TopicNmLx.GetSym();
        }
        // add topic
        AddTopicNm(TopicNmTokStrV, TopicTagNm);
      }
    }
    printf(" Done.\n");
  }
  printf("Done.\n");
}

void TNytaBs::SaveNytTopicsTxt(const TStr& FNm){
  TFOut FOut(FNm); FILE* fOut=FOut.GetFileId();
  for (int TopicId=0; TopicId<GetTopicNms(); TopicId++){
    TStr TopicNm=GetTopicNm(TopicId);
    fprintf(fOut, "'%s' (", TopicNm.CStr());
    for (int TopicTagN=0; TopicTagN<GetTopicTags(TopicId); TopicTagN++){
      TStr TopicTagNm=GetTopicTagNm(TopicId, TopicTagN);
      if (TopicTagN>0){fprintf(fOut, ", ");}
      fprintf(fOut, "'%s'", TopicTagNm.CStr());
    }
    fprintf(fOut, "):");
    for (int DocIdN=0; DocIdN<GetTopicDocIds(TopicId); DocIdN++){
      int DocId=GetTopicDocId(TopicId, DocIdN);
      fprintf(fOut, " %d", DocId);
    }
    fprintf(fOut, "\n");
  }
}

void TNytaBs::AnnotateWithNytTopics(){
  printf("Annotating with NYT-Topics ...\n");
  for (int DocId=0; DocId<NytaDocV.Len(); DocId++){
    printf("%d/%d\r", 1+DocId, NytaDocV.Len());
    // get document
    PNytaDoc NytaDoc=GetDoc(DocId);
    TStr SummaryXmlStr=NytaDoc->SummaryXmlStr;

    // save document tokens
    typedef TTriple<TInt, TStr, TStr> TSymStrPreTr;
    TVec<TSymStrPreTr> SymStrPreTrV;
    PSIn SIn=TStrIn::New(SummaryXmlStr);
    THtmlLx Lx(SIn);
    Lx.GetSym();
    while (Lx.Sym!=hsyEof){
      TSymStrPreTr SymStrPreTr(TSymStrPreTr(Lx.Sym, Lx.SymChA, Lx.PreSpaceChA));
      SymStrPreTrV.Add(SymStrPreTr);
      Lx.GetSym();
    }

    // annotate
    TChA AnnDocChA;
    int SymN=0;
    while (SymN<SymStrPreTrV.Len()){
      // get symbol strings
      TStr SymStr=SymStrPreTrV[SymN].Val2;
      TStr PreSpaceStr=SymStrPreTrV[SymN].Val3;
      // check if possibility for a topic-name
      if (TopicNmFirstTokStrToToksVH.IsKey(SymStr)){
        const TIntV& ToksV=TopicNmFirstTokStrToToksVH.GetDat(SymStr);
        if ((ToksV.Len()==1)&&(ToksV[0]==1)){
          // single-token topic-name
          TStrV TokStrV; TokStrV.Add(SymStr);
          int TopicId=GetTopicId(TokStrV);
          AddTopicDocId(TopicId, DocId);
          AnnDocChA+=PreSpaceStr;
          AnnDocChA+="<ent>"; AnnDocChA+=SymStr; AnnDocChA+="</ent>";
          NytaDoc->AddTopicId(TopicId);
          SymN++;
        } else {
          // multiple-tokens topic-name
          int MxToks=ToksV.Last(); TStrV TokStrV(MxToks, 0);
          for (int TokN=0; TokN<MxToks; TokN++){
            if (SymN+TokN>=SymStrPreTrV.Len()){break;}
            TokStrV.Add(SymStrPreTrV[SymN+TokN].Val2);
          }
          int TopicId;
          for (int ToksN=ToksV.Len()-1; ToksN>=0; ToksN--){
            int Toks=ToksV[ToksN];
            TokStrV.Trunc(Toks);
            if (IsTopic(TokStrV, TopicId)){
              break;}
          }
          if (IsTopic(TokStrV, TopicId)){
            AddTopicDocId(TopicId, DocId);
            AnnDocChA+=PreSpaceStr;
            AnnDocChA+="<ent>"; AnnDocChA+=GetTopicNm(TopicId); AnnDocChA+="</ent>";
            NytaDoc->AddTopicId(TopicId);
            SymN+=TokStrV.Len();
          } else {
            AnnDocChA+=PreSpaceStr; AnnDocChA+=SymStr; SymN++;
          }
        }
      } else {
        // copy token
        AnnDocChA+=PreSpaceStr; AnnDocChA+=SymStr; SymN++;
      }
    }
    if (!(SummaryXmlStr==AnnDocChA.CStr())){
      //printf("%s\n%s\n", SummaryStr.CStr(), AnnDocChA.CStr());
    }
    NytaDoc->SummaryXmlStr=AnnDocChA;
  }
  printf("\nDone.\n");
}

int TNytaBs::AddTopicNm(const TStrV& TopicNmTokStrV,
 const TStr& TopicTagNm1, const TStr& TopicTagNm2, const TStr& TopicTagNm3){
  TopicNmFirstTokStrToToksVH.AddDat(TopicNmTokStrV[0]).AddUnique(TopicNmTokStrV.Len());
  int TopicId=TokStrVToTopicH.AddKey(TopicNmTokStrV);
  if (!TopicTagNm1.Empty()){
    TokStrVToTopicH[TopicId].AddTopicTag(TopicTagNm1);}
  if (!TopicTagNm2.Empty()){
    TokStrVToTopicH[TopicId].AddTopicTag(TopicTagNm2);}
  if (!TopicTagNm3.Empty()){
    TokStrVToTopicH[TopicId].AddTopicTag(TopicTagNm3);}
  return TopicId;
}

TStr TNytaBs::GetTopicNm(const int& TopicId) const {
  const TStrV& TokStrV=TokStrVToTopicH.GetKey(TopicId);
  TChA TopicChA;
  for (int TokN=0; TokN<TokStrV.Len(); TokN++){
    TStr TokStr=TokStrV[TokN];
    if (TokN>0){
      TStr NonSpaceChStr=".,-)";
      if ((TokStr.Len()==1)&&(NonSpaceChStr).IsChIn(TokStr[0])){
      } else
      if ((TopicChA.Len()>0)&&
       ((TopicChA.LastCh()=='-')||(TopicChA.LastCh()=='('))){
      } else {
        TopicChA+=' ';
      }
    }
    TopicChA+=TokStrV[TokN];
  }
  return TopicChA;
}

void TNytaBs::SaveR2KXml(const TStr& FNm){
  TFOut FOut(FNm); FILE* fOut=FOut.GetFileId();
  for (int DocId=0; DocId<NytaDocV.Len(); DocId++){
    // get documents
    PNytaDoc NytaDoc=GetDoc(DocId);
    // get components in xml format
    TStr UrlXmlStr=TXmlLx::GetXmlStrFromPlainStr(NytaDoc->UrlStr);
    TStr TitleXmlStr=NytaDoc->TitleXmlStr;
    TStr SummaryXmlStr=NytaDoc->SummaryXmlStr;
    TStr YMDDateXmlStr=NytaDoc->GetYMDDateStr();

    // output document in Reuters format
    fprintf(fOut, "<?xml version=\"1.0\" encoding=\"iso-8859-1\" ?>\n");
    fprintf(fOut, "<newsitem itemid=\"%d\" id=\"root\" date=\"%s\" xml:lang=\"en\">\n", DocId, YMDDateXmlStr.CStr());
    fprintf(fOut, "<title>%s</title>\n", TitleXmlStr.CStr());
    //fprintf(fOut, "<headline>%s</headline>\n", SummaryXmlStr.CStr());
    fprintf(fOut, "<dateline>%s</dateline>\n", YMDDateXmlStr.CStr());
    fprintf(fOut, "<text>\n");
    fprintf(fOut, "<p>%s</p>\n", SummaryXmlStr.CStr());
    fprintf(fOut, "</text>\n");
    fprintf(fOut, "<url>%s</url>\n", UrlXmlStr.CStr());
    fprintf(fOut, "<copyright>(c) New York Times 2008</copyright>\n");
    fprintf(fOut, "<metadata>\n");
    if (NytaDoc->GetTopicIds()>0){
      fprintf(fOut, "  <codes class=\"nyt-quint:1.0\">\n");
      for (int TopicIdN=0; TopicIdN<NytaDoc->GetTopicIds(); TopicIdN++){
        int TopicId=NytaDoc->GetTopicId(TopicIdN);
        TStr TopicNm=GetTopicNm(TopicId);
        TStr XmlTopicNm=TXmlLx::GetXmlStrFromPlainStr(TopicNm);
        fprintf(fOut, "    <code code=\"%s\">", XmlTopicNm.CStr());
        for (int TopicTagN=0; TopicTagN<GetTopicTags(TopicId); TopicTagN++){
          TStr TopicTagNm=GetTopicTagNm(TopicId, TopicTagN);
          TStr XmlTopicTagNm=TXmlLx::GetXmlStrFromPlainStr(TopicTagNm);
          fprintf(fOut, "<tag>%s</tag>", XmlTopicTagNm.CStr());
        }
        fprintf(fOut, "</code>\n");
      }
      fprintf(fOut, "  </codes>\n");
    }
    fprintf(fOut, "  <dc element=\"dc.publisher\" value=\"New York Times Inc\"/>\n");
    fprintf(fOut, "  <dc element=\"dc.date.published\" value=\"%s\"/>\n", YMDDateXmlStr.CStr());
    fprintf(fOut, "  <dc element=\"dc.conversion\" value=\"Quintelligence d.o.o.\"/>\n");
    fprintf(fOut, "</metadata>\n");
    fprintf(fOut, "</newsitem>\n");
    fprintf(fOut, "\n");
  }
}

