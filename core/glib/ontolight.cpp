/////////////////////////////////////////////////
// Light-Weight-Language-Base
void TLwLangBs::SaveTxt(FILE* fOut){
  fprintf(fOut, "===LwLangBs-Begin====================\n");
  for (int LangN=0; LangN<GetLangs(); LangN++){
    int LangId=GetLangId(LangN);
    fprintf(fOut, "N:%d Id:%d Nm:'%s' Desc:'%s'\n",
     LangN, LangId, GetLangNm(LangId).CStr(), GetLangDesc(LangId).CStr());
  }
  fprintf(fOut, "===LwLangBs-End======================\n");
}

/////////////////////////////////////////////////
// Light-Weight-Term-Type
void TLwTermType::SaveTxt(FILE* fOut){
  fprintf(fOut, "Id:%d Nm:'%s' LangId:%d Desc:'%s'\n",
    GetTermTypeId(), GetTermTypeNm().CStr(), GetLangId(), GetDescStr().CStr());
}

/////////////////////////////////////////////////
// Light-Weight-Term
void TLwTerm::SaveTxt(FILE* fOut){
  fprintf(fOut, "Id:%d Nm:'%s' LangId:%d TermTypeId:%d Desc:'%s'\n",
   GetTermId(), GetTermNm().CStr(), GetLangId(), GetTermTypeId(), GetDescStr().CStr());
}

/////////////////////////////////////////////////
// Light-Weight-Term-Type-Base
int TLwTermTypeBs::AddTermType(const PLwTermType& LwTermType){
  IAssert(!IsTermTypeId(LwTermType->GetTermTypeId()));
  IdToTermTypeVH.AddDat(LwTermType->GetTermTypeId()).Add(LwTermType);
  TStrIntPr TermTypeNmLangIdPr=
   TStrIntPr(LwTermType->GetTermTypeNm(), LwTermType->GetLangId());
  TermTypeNmLangIdToIdH.AddDat(TermTypeNmLangIdPr, LwTermType->GetTermTypeId());
  return LwTermType->GetTermTypeId();
}

void TLwTermTypeBs::SaveTxt(FILE* fOut){
  fprintf(fOut, "===LwTermTypeBs-Begin================\n");
  for (int TermTypeN=0; TermTypeN<GetTermTypes(); TermTypeN++){
    fprintf(fOut, "N:%d ", TermTypeN);
    GetTermType(GetTermTypeId(TermTypeN))->SaveTxt(fOut);
  }
  fprintf(fOut, "===LwTermTypeBs-End==================\n");
}

/////////////////////////////////////////////////
// Light-Weight-Term-Base
void TLwTermBs::AddTerm(const PLwTerm& Term){
  IAssert(Onto->GetLangBs()->IsLangId(Term->GetLangId()));
  if (Term->GetTermId()>MxTermId){MxTermId=Term->GetTermId();}
  IdToTermVH.AddDat(Term->GetTermId()).Add(Term);
  TermNmLangIdToToTermIdH.AddDat(
   TStrIntPr(Term->GetTermNm(), Term->GetLangId()), Term->GetTermId());
}

int TLwTermBs::AddTermGetTermId(const PLwTerm& Term){
  IAssert(Term->GetTermId()==-1);
  Term->PutTermId(MxTermId+1);
  AddTerm(Term);
  return Term->GetTermId();
}

PLwTerm TLwTermBs::GetTerm(const int& TermId){
  TLwTermV TermV=IdToTermVH.GetDat(TermId);
  if (Onto->GetLangBs()->IsLangNm("EN")){
    int EnLangId=Onto->GetLangBs()->GetLangId("EN");
    for (int TermN=0; TermN<TermV.Len(); TermN++){
      if (TermV[TermN]->GetLangId()==EnLangId){return TermV[TermN];}
    }
  }
  return TermV[0];
}

void TLwTermBs::SaveTxt(FILE* fOut){
  fprintf(fOut, "===LwTermBs-Begin====================\n");
  for (int TermN=0; TermN<GetTerms(); TermN++){
    TLwTermV TermV; GetTermV(GetTermId(TermN), TermV);
    for (int SubTermN=0; SubTermN<TermV.Len(); SubTermN++){
      if (TermV.Len()==1){fprintf(fOut, "N:%d ", TermN);}
      else {fprintf(fOut, "N:%d/%d ", TermN, SubTermN);}
      TermV[SubTermN]->SaveTxt(fOut);
    }
    Onto->GetLinkBs()->SaveTxt(fOut, GetTermId(TermN), "   ");
  }
  fprintf(fOut, "===LwTermBs-End======================\n");
}

/////////////////////////////////////////////////
// Light-Weight-Link-Type
void TLwLinkType::SaveTxt(FILE* fOut){
  fprintf(fOut, "Id:%d Nm:'%s' LangId:%d SDesc:'%s' Desc:'%s' RevLinkTypeId:%d ParentLinkTypeId:%d\n",
   GetLinkTypeId(), GetLinkTypeNm().CStr(), GetLangId(),
   GetSDescStr().CStr(), GetDescStr().CStr(),
   GetReverseLinkTypeId(), GetParentLinkTypeId());
}

/////////////////////////////////////////////////
// Light-Weight-Link-Type-Base
void TLwLinkTypeBs::AddLinkType(const PLwLinkType& LwLinkType){
  IdToLinkTypeVH.AddDat(LwLinkType->GetLinkTypeId()).Add(LwLinkType);
  TStrIntPr LinkTypeNmLangIdPr(LwLinkType->GetLinkTypeNm(), LwLinkType->GetLangId());
  IAssert(!LinkTypeNmLangIdToIdH.IsKey(LinkTypeNmLangIdPr));
  LinkTypeNmLangIdToIdH.AddDat(LinkTypeNmLangIdPr, LwLinkType->GetLinkTypeId());
}

void TLwLinkTypeBs::SaveTxt(FILE* fOut){
  fprintf(fOut, "===LwLinkTypeBs-Begin================\n");
  for (int LinkTypeN=0; LinkTypeN<GetLinkTypes(); LinkTypeN++){
    fprintf(fOut, "N:%d ", LinkTypeN);
    GetLinkType(GetLinkTypeId(LinkTypeN))->SaveTxt(fOut);
  }
  fprintf(fOut, "===LwLinkTypeBs-End==================\n");
}

/////////////////////////////////////////////////
// Light-Weight-Link-Base
int TLwLinkBs::GetLinks() const {
  int Links=0;
  int TermIds=FromTermIdToLinkTypeIdTermIdPrVH.Len();
  for (int TermIdN=0; TermIdN<TermIds; TermIdN++){
    Links+=FromTermIdToLinkTypeIdTermIdPrVH[TermIdN].Len();}
  return Links;
}

void TLwLinkBs::AddLink(const int TermId1, int LinkTypeId, int TermId2){
  IAssert(Onto->GetTermBs()->IsTermId(TermId1));
  IAssert(Onto->GetLinkTypeBs()->IsLinkTypeId(LinkTypeId));
  IAssert(Onto->GetTermBs()->IsTermId(TermId2));
  FromTermIdToLinkTypeIdTermIdPrVH.AddDat(TermId1).Add(TIntPr(LinkTypeId, TermId2));
  IntoTermIdToLinkTypeIdTermIdPrVH.AddDat(TermId2).Add(TIntPr(LinkTypeId, TermId1));
}

int TLwLinkBs::GetFromLinks(const int SrcTermId) const {
  if (FromTermIdToLinkTypeIdTermIdPrVH.IsKey(SrcTermId)){
    return FromTermIdToLinkTypeIdTermIdPrVH.GetDat(SrcTermId).Len();
  } else {return 0;}
}

void TLwLinkBs::GetFromLink(
 const int SrcTermId, const int& LinkN, int& LinkTypeId, int& DstTermId){
  TIntPr& LinkTypeIdDstTermIdPr=
   FromTermIdToLinkTypeIdTermIdPrVH.GetDat(SrcTermId)[LinkN];
  LinkTypeId=LinkTypeIdDstTermIdPr.Val1;
  DstTermId=LinkTypeIdDstTermIdPr.Val2;
}

int TLwLinkBs::GetIntoLinks(const int DstTermId) const {
  if (IntoTermIdToLinkTypeIdTermIdPrVH.IsKey(DstTermId)){
    return IntoTermIdToLinkTypeIdTermIdPrVH.GetDat(DstTermId).Len();
  } else {return 0;}
}

void TLwLinkBs::GetIntoLink(
 const int DstTermId, const int& LinkN, int& LinkTypeId, int& SrcTermId){
  TIntPr& LinkTypeIdSrcTermIdPr=
   IntoTermIdToLinkTypeIdTermIdPrVH.GetDat(DstTermId)[LinkN];
  LinkTypeId=LinkTypeIdSrcTermIdPr.Val1;
  SrcTermId=LinkTypeIdSrcTermIdPr.Val2;
}

void TLwLinkBs::SaveTxt(FILE* fOut, const int& TermId, const TStr& IndentStr){
  for (int LinkN=0; LinkN<GetFromLinks(TermId); LinkN++){
    int LinkTypeId; int DstTermId;
    GetFromLink(TermId, LinkN, LinkTypeId, DstTermId);
    TStr LinkTypeNm=Onto->GetLinkTypeBs()->GetLinkType(LinkTypeId)->GetLinkTypeNm();
    TStr DstTermNm=Onto->GetTermBs()->GetTerm(DstTermId)->GetTermNm();
    fprintf(fOut, "%s-->%s--> DstTermId:%d DstTermNm:'%s'\n", IndentStr.CStr(),
     LinkTypeNm.CStr(), DstTermId, DstTermNm.CStr());
  }
  for (int LinkN=0; LinkN<GetIntoLinks(TermId); LinkN++){
    int LinkTypeId; int SrcTermId;
    GetIntoLink(TermId, LinkN, LinkTypeId, SrcTermId);
    TStr LinkTypeNm=Onto->GetLinkTypeBs()->GetLinkType(LinkTypeId)->GetLinkTypeNm();
    TStr SrcTermNm=Onto->GetTermBs()->GetTerm(SrcTermId)->GetTermNm();
    fprintf(fOut, "%s<--%s<-- SrcTermId:%d SrcTermNm:'%s'\n", IndentStr.CStr(),
     LinkTypeNm.CStr(), SrcTermId, SrcTermNm.CStr());
  }
}

/////////////////////////////////////////////////
// Light-Weight-Ontology
void TLwOnto::SaveTxt(const TStr& FNm){
  TFOut FOut(FNm); FILE* fOut=FOut.GetFileId();
  fprintf(fOut, "===LwOnto-Begin======================\n");
  GetLangBs()->SaveTxt(fOut);
  fprintf(fOut, "\n");
  GetTermTypeBs()->SaveTxt(fOut);
  fprintf(fOut, "\n");
  GetLinkTypeBs()->SaveTxt(fOut);
  fprintf(fOut, "\n");
  GetTermBs()->SaveTxt(fOut);
  fprintf(fOut, "===LwOnto-End========================\n");
}

PLwOnto TLwOnto::LoadAgroVoc(const TStr& FPath){
  // normalize path
  TStr NrFPath=TStr::GetNrFPath(FPath);
  // create ontology
  PLwOnto LwOnto=TLwOnto::New();

  // languages
  {TStr AvLangFNm=NrFPath+"language.txt";
  printf("Loading '%s' ...", AvLangFNm.CStr());
  PSs AvLangSs=TSs::LoadTxt(ssfCommaSep, AvLangFNm);
  int AvLangSs_LangNmFldN=AvLangSs->GetFldX("languagecode");
  int AvLangSs_LangDescFldN=AvLangSs->GetFldX("name");
  for (int Y=1; Y<AvLangSs->GetYLen(); Y++){
    // extract language info
    TStr LangNm=AvLangSs->GetVal(AvLangSs_LangNmFldN, Y);
    TStr LangDesc=AvLangSs->GetVal(AvLangSs_LangDescFldN, Y);
    // create language object
    LwOnto->GetLangBs()->AddLang(LangNm, LangDesc);
  }
  printf(" Done. (%d)\n", AvLangSs->GetYLen()-1);}

  // terms
  {TStr AvTermFNm=NrFPath+"agrovocterm.txt";
  printf("Loading '%s' ...", AvTermFNm.CStr());
  PSs AvTermSs=TSs::LoadTxt(ssfCommaSep, AvTermFNm);
  int AvTermSs_TermIdFldN=AvTermSs->GetFldX("termcode");
  int AvTermSs_LangNmFldN=AvTermSs->GetFldX("languagecode");
  int AvTermSs_TermSpellFldN=AvTermSs->GetFldX("termspell");
  for (int Y=1; Y<AvTermSs->GetYLen(); Y++){
    int TermId=AvTermSs->GetVal(AvTermSs_TermIdFldN, Y).GetInt();
    TStr LangNm=AvTermSs->GetVal(AvTermSs_LangNmFldN, Y);
    TStr TermNm=AvTermSs->GetVal(AvTermSs_TermSpellFldN, Y);
    int LangId=LwOnto->GetLangBs()->GetLangId(LangNm);
    PLwTerm Term=TLwTerm::New(TermId, TermNm, LangId);
    LwOnto->GetTermBs()->AddTerm(Term);
  }
  printf(" Done. (%d)\n", AvTermSs->GetYLen()-1);}

  // link-types
  {TStr AvLinkTypeFNm=NrFPath+"linktype.txt";
  printf("Loading '%s' ...", AvLinkTypeFNm.CStr());
  PSs AvLinkTypeSs=TSs::LoadTxt(ssfCommaSep, AvLinkTypeFNm);
  int AvLinkTypeSs_LinkTypeIdFldN=AvLinkTypeSs->GetFldX("linktypeid");
  int AvLinkTypeSs_LangIdFldN=AvLinkTypeSs->GetFldX("languagecode");
  int AvLinkTypeSs_LinkSDescFldN=AvLinkTypeSs->GetFldX("linkdesc");
  int AvLinkTypeSs_LinkNmFldN=AvLinkTypeSs->GetFldX("linkabr");
  int AvLinkTypeSs_LinkDescFldN=AvLinkTypeSs->GetFldX("linkdescription");
  int AvLinkTypeSs_ReverseLinkIdFldN=AvLinkTypeSs->GetFldX("rlinkcode");
  int AvLinkTypeSs_ParentLinkTypeIdFldN=AvLinkTypeSs->GetFldX("parentlinktypeid");
  for (int Y=1; Y<AvLinkTypeSs->GetYLen(); Y++){
    // extract link-type ifo
    int LinkTypeId=AvLinkTypeSs->GetVal(AvLinkTypeSs_LinkTypeIdFldN, Y).GetInt();
    TStr LinkTypeNm=AvLinkTypeSs->GetVal(AvLinkTypeSs_LinkNmFldN, Y);
    TStr LangNm=AvLinkTypeSs->GetVal(AvLinkTypeSs_LangIdFldN, Y);
    TStr SDescStr=AvLinkTypeSs->GetVal(AvLinkTypeSs_LinkSDescFldN, Y);
    TStr DescStr=AvLinkTypeSs->GetVal(AvLinkTypeSs_LinkDescFldN, Y);
    int ReverseLinkTypeId=AvLinkTypeSs->GetVal(AvLinkTypeSs_ReverseLinkIdFldN, Y).GetInt(-1);
    int ParentLinkTypeId=AvLinkTypeSs->GetVal(AvLinkTypeSs_ParentLinkTypeIdFldN, Y).GetInt(-1);
    // create link-type object
    int LangId=LwOnto->GetLangBs()->GetLangId(LangNm);
    PLwLinkType LwLinkType=TLwLinkType::New();
    LwLinkType->PutLinkTypeId(LinkTypeId);
    LwLinkType->PutLinkTypeNm(LinkTypeNm);
    LwLinkType->PutLangId(LangId);
    LwLinkType->PutSDescStr(SDescStr);
    LwLinkType->PutDescStr(DescStr);
    LwLinkType->PutReverseLinkTypeId(ReverseLinkTypeId);
    LwLinkType->PutParentLinkTypeId(ParentLinkTypeId);
    LwOnto->GetLinkTypeBs()->AddLinkType(LwLinkType);
  }
  printf(" Done. (%d)\n", AvLinkTypeSs->GetYLen()-1);}

  // links
  {TStr AvTermLinkFNm=NrFPath+"termlink.txt";
  printf("Loading '%s' ...", AvTermLinkFNm.CStr());
  PSs AvTermLinkSs=TSs::LoadTxt(ssfCommaSep, AvTermLinkFNm);
  int AvTermLinkSs_TermId1FldN=AvTermLinkSs->GetFldX("termcode1");
  int AvTermLinkSs_TermId2FldN=AvTermLinkSs->GetFldX("termcode2");
  int AvTermLinkSs_LinkTypeIdFldN=AvTermLinkSs->GetFldX("linktypeid");
  for (int Y=1; Y<AvTermLinkSs->GetYLen(); Y++){
    int TermId1=AvTermLinkSs->GetVal(AvTermLinkSs_TermId1FldN, Y).GetInt();
    int TermId2=AvTermLinkSs->GetVal(AvTermLinkSs_TermId2FldN, Y).GetInt();
    int LinkTypeId=AvTermLinkSs->GetVal(AvTermLinkSs_LinkTypeIdFldN, Y).GetInt();
    if (!LwOnto->GetTermBs()->IsTermId(TermId1)){printf("#"); continue;}
    if (!LwOnto->GetTermBs()->IsTermId(TermId2)){printf("*"); continue;}
    LwOnto->GetLinkBs()->AddLink(TermId1, LinkTypeId, TermId2);
  }
  printf(" Done. (%d)\n", AvTermLinkSs->GetYLen()-1);}

  // return ontology
  return LwOnto;
}

PLwOnto TLwOnto::LoadEuroVoc(const TStr& FPath){
  // normalize path
  TStr NrFPath=TStr::GetNrFPath(FPath);
  // create ontology
  PLwOnto LwOnto=TLwOnto::New();

  // load languages & terms
  TStr EvTermFNm=NrFPath+"listMultiLg_All.txt";
  printf("Loading '%s' ...\n", EvTermFNm.CStr());
  PSs EvTermSs=TSs::LoadTxt(ssfTabSep, EvTermFNm);
  int EvTermSs_TermIdFldN=EvTermSs->GetFldX("ID"); IAssert(EvTermSs_TermIdFldN==0);
  int XLen=EvTermSs->GetXLen(EvTermSs_TermIdFldN);

  // collect languages
  for (int X=1; X<XLen; X++){
    TStr LangNm=EvTermSs->GetVal(X, 0); // extract language info
    LwOnto->GetLangBs()->AddLang(LangNm, LangNm); // create language object
  }
  int EnLangId=LwOnto->GetLangBs()->GetLangId("EN");

  // create term-types
  PLwTermType D_TermType=TLwTermType::New(0, "Descriptor", EnLangId);
  PLwTermType ND_TermType=TLwTermType::New(1, "Non-descriptor", EnLangId);
  LwOnto->GetTermTypeBs()->AddTermType(D_TermType);
  LwOnto->GetTermTypeBs()->AddTermType(ND_TermType);

  // collect multilingual terms
  for (int Y=1; Y<EvTermSs->GetYLen(); Y++){
    printf("%d/%d\r", Y, EvTermSs->GetYLen());
    int TermId=EvTermSs->GetVal(EvTermSs_TermIdFldN, Y).GetInt();
    for (int X=1; X<XLen; X++){
      TStr LangNm=EvTermSs->GetVal(X, 0);
      TStr TermNm=EvTermSs->GetVal(X, Y);
      int LangId=LwOnto->GetLangBs()->GetLangId(LangNm);
      PLwTerm Term=TLwTerm::New(TermId, TermNm, LangId, D_TermType->GetTermTypeId());
      LwOnto->GetTermBs()->AddTerm(Term);
    }
  }
  printf("\nDone. (%d)\n", EvTermSs->GetYLen()-1);

  // link-types
  {PLwLinkType BT_LinkType=TLwLinkType::New(0, "BT", EnLangId, "Broader-Term");
  PLwLinkType NT_LinkType=TLwLinkType::New(1, "NT", EnLangId, "Narrower-Term");
  PLwLinkType MT_LinkType=TLwLinkType::New(2, "MT", EnLangId, "Micro-Theory");
  PLwLinkType RT_LinkType=TLwLinkType::New(3, "RT", EnLangId, "Related-Term");
  PLwLinkType UF_LinkType=TLwLinkType::New(4, "UF", EnLangId, "Used-For");
  LwOnto->GetLinkTypeBs()->AddLinkType(BT_LinkType);
  LwOnto->GetLinkTypeBs()->AddLinkType(NT_LinkType);
  LwOnto->GetLinkTypeBs()->AddLinkType(MT_LinkType);
  LwOnto->GetLinkTypeBs()->AddLinkType(RT_LinkType);
  LwOnto->GetLinkTypeBs()->AddLinkType(UF_LinkType);}

  // links
  {TStr EvLinkFNm=NrFPath+"eurovoc.txt";
  printf("Loading '%s' ...", EvLinkFNm.CStr());
  PSs EvLinkSs=TSs::LoadTxt(ssfTabSep, EvLinkFNm);
  int EvLinkSs_TermId1FldN=EvLinkSs->GetFldX("term");
  int EvLinkSs_LinkTypeNmFldN=EvLinkSs->GetFldX("link");
  int EvLinkSs_TermId2FldN=EvLinkSs->GetFldX("subterm");
  int EvLinkSs_TermNmFldN=EvLinkSs->GetFldX("name");
  int EnLangId=LwOnto->GetLangBs()->GetLangId("EN");
  for (int Y=1; Y<EvLinkSs->GetYLen(); Y++){
    // extract link info
    int TermId1=EvLinkSs->GetVal(EvLinkSs_TermId1FldN, Y).GetInt();
    TStr LinkTypeNm=EvLinkSs->GetVal(EvLinkSs_LinkTypeNmFldN, Y);
    int TermId2=EvLinkSs->GetVal(EvLinkSs_TermId2FldN, Y).GetInt();
    TStr TermNm=EvLinkSs->GetVal(EvLinkSs_TermNmFldN, Y);
    // add link
    if (LinkTypeNm=="UF"){
      IAssert(TermId2==-1);
      PLwTerm Term=TLwTerm::New(-1, TermNm, EnLangId, ND_TermType->GetTermTypeId());
      TermId2=LwOnto->GetTermBs()->AddTermGetTermId(Term);
      int LinkTypeId=LwOnto->GetLinkTypeBs()->GetLinkTypeId(LinkTypeNm, EnLangId);
      LwOnto->GetLinkBs()->AddLink(TermId1, LinkTypeId, TermId2);
    } else {
      if (LinkTypeNm=="NT1"){LinkTypeNm="NT";}
      if (LinkTypeNm=="BT1"){LinkTypeNm="BT";}
      if ((LinkTypeNm=="BT")||(LinkTypeNm=="NT")||(LinkTypeNm=="RT")){
        int LinkTypeId=LwOnto->GetLinkTypeBs()->GetLinkTypeId(LinkTypeNm, EnLangId);
        LwOnto->GetLinkBs()->AddLink(TermId1, LinkTypeId, TermId2);
      }
    }
  }
  printf(" Done. (%d)\n", EvLinkSs->GetYLen()-1);}

  // return ontology
  return LwOnto;
}

PLwOnto TLwOnto::LoadAsfaVoc(const TStr& FPath){
  // normalize path
  TStr NrFPath=TStr::GetNrFPath(FPath);
  // create ontology
  PLwOnto LwOnto=TLwOnto::New();

  // create language object
  int EnLangId=LwOnto->GetLangBs()->AddLang("EN", "English");

  // create term-types
  {PLwTermType D_TermType=TLwTermType::New(0, "Descriptor", EnLangId);
  PLwTermType ND_TermType=TLwTermType::New(1, "Non-descriptor", EnLangId);
  LwOnto->GetTermTypeBs()->AddTermType(D_TermType);
  LwOnto->GetTermTypeBs()->AddTermType(ND_TermType);}

  // create link-types
  {PLwLinkType BT_LinkType=TLwLinkType::New(0, "BT", EnLangId, "Broader-Term");
  PLwLinkType NT_LinkType=TLwLinkType::New(1, "NT", EnLangId, "Narrower-Term");
  PLwLinkType RT_LinkType=TLwLinkType::New(2, "RT", EnLangId, "Related-Term");
  PLwLinkType UF_LinkType=TLwLinkType::New(3, "UF", EnLangId, "Used-For");
  PLwLinkType USE_LinkType=TLwLinkType::New(4, "USE", EnLangId, "Used-By");
  LwOnto->GetLinkTypeBs()->AddLinkType(BT_LinkType);
  LwOnto->GetLinkTypeBs()->AddLinkType(NT_LinkType);
  LwOnto->GetLinkTypeBs()->AddLinkType(RT_LinkType);
  LwOnto->GetLinkTypeBs()->AddLinkType(UF_LinkType);
  LwOnto->GetLinkTypeBs()->AddLinkType(USE_LinkType);}

  // load ontology file
  TStr AsfaOntoFNm=NrFPath+"asfa_xml_20060522.xml";
  printf("Loading '%s' ...", AsfaOntoFNm.CStr());
  PXmlDoc AsfaXmlDoc=TXmlDoc::LoadTxt(AsfaOntoFNm);
  IAssert(AsfaXmlDoc->IsOk());
  TXmlTokV ConceptXmlTokV;
  AsfaXmlDoc->GetTagTokV("THESAURUS|CONCEPT", ConceptXmlTokV);
  printf(" Done.\n");

  // create terms
  {printf("Creating terms ...");
  for (int ConceptN=0; ConceptN<ConceptXmlTokV.Len(); ConceptN++){
    PXmlTok ConceptXmlTok=ConceptXmlTokV[ConceptN];
    // term-name
    TStr TermNm;
    if (ConceptXmlTok->IsSubTag("NON-DESCRIPTOR")){
      TermNm=ConceptXmlTok->GetTagTokStr("NON-DESCRIPTOR");}
    else if (ConceptXmlTok->IsSubTag("DESCRIPTOR")){
      TermNm=ConceptXmlTok->GetTagTokStr("DESCRIPTOR");}
    // term-type
    TStr TermTypeNm=ConceptXmlTok->GetTagTokStr("TYP");
    int TermTypeId=LwOnto->GetTermTypeBs()->GetTermTypeId(TermTypeNm, EnLangId);
    // description
    TStr DescStr;
    if (ConceptXmlTok->IsSubTag("SN")){
      DescStr=ConceptXmlTok->GetTagTokStr("SN");
      DescStr.ChangeChAll('\r', ' '); DescStr.ChangeChAll('\n', ' ');
      DescStr.ChangeStrAll("  ", " "); DescStr.ToTrunc();
    }
    // create term
    PLwTerm Term=TLwTerm::New(-1, TermNm, EnLangId, TermTypeId, DescStr);
    LwOnto->GetTermBs()->AddTermGetTermId(Term);
  }
  printf(" Done. (%d)\n", LwOnto->GetTermBs()->GetTerms());}

  // create links
  {printf("Creating links ...");
  for (int ConceptN=0; ConceptN<ConceptXmlTokV.Len(); ConceptN++){
    PXmlTok ConceptXmlTok=ConceptXmlTokV[ConceptN];
    // source-term-name
    TStr TermNm1;
    if (ConceptXmlTok->IsSubTag("NON-DESCRIPTOR")){
      TermNm1=ConceptXmlTok->GetTagTokStr("NON-DESCRIPTOR");}
    else if (ConceptXmlTok->IsSubTag("DESCRIPTOR")){
      TermNm1=ConceptXmlTok->GetTagTokStr("DESCRIPTOR");}
    int TermId1=LwOnto->GetTermBs()->GetTermId(TermNm1, EnLangId);
    // links
    for (int SubTokN=0; SubTokN<ConceptXmlTok->GetSubToks(); SubTokN++){
      PXmlTok SubTok=ConceptXmlTok->GetSubTok(SubTokN);
      if (SubTok->IsTag()){
        TStr LinkTypeNm=SubTok->GetTagNm();
        if (LwOnto->GetLinkTypeBs()->IsLinkType(LinkTypeNm, EnLangId)){
          // destination-term-name
          TStr TermNm2=ConceptXmlTok->GetTagTokStr(LinkTypeNm);
          int TermId2=LwOnto->GetTermBs()->GetTermId(TermNm2, EnLangId);
          int LinkTypeId=LwOnto->GetLinkTypeBs()->GetLinkTypeId(LinkTypeNm, EnLangId);
          LwOnto->GetLinkBs()->AddLink(TermId1, LinkTypeId, TermId2);
        }
      }
    }
  }
  printf(" Done. (%d)\n", LwOnto->GetLinkBs()->GetLinks());}

  // return ontology
  return LwOnto;
}

PLwOnto TLwOnto::LoadDMozVoc(const TStr& DMozBinFBase, const TStr& FPath){
  // load DMoz-Base
  PDMozBs DMozBs=TDMozBs::LoadBin(DMozBinFBase, FPath);
  int Cats=DMozBs->GetCats();

  // create ontology
  PLwOnto LwOnto=TLwOnto::New();

  // create language object
  int EnLangId=LwOnto->GetLangBs()->AddLang("EN", "English");

  // create term-types
  PLwTermType Cat_TermType=TLwTermType::New(0, "Category", EnLangId);
  LwOnto->GetTermTypeBs()->AddTermType(Cat_TermType);

  // create link-types
  PLwLinkType NT_LinkType=TLwLinkType::New(0, "NT", EnLangId, "Narrow");
  PLwLinkType RT_LinkType=TLwLinkType::New(1, "RT", EnLangId, "Related");
  PLwLinkType SL_LinkType=TLwLinkType::New(2, "SL", EnLangId, "Symbolic");
  LwOnto->GetLinkTypeBs()->AddLinkType(NT_LinkType);
  LwOnto->GetLinkTypeBs()->AddLinkType(RT_LinkType);
  LwOnto->GetLinkTypeBs()->AddLinkType(SL_LinkType);

  // create terms
  {printf("Creating terms ...\n");
  for (int CatId=0; CatId<Cats; CatId++){
    if (CatId%1000==0){printf("%d/%d\r", CatId, Cats);}
    TStr TermNm=DMozBs->GetCatNm(CatId);
    // create term
    PLwTerm Term=TLwTerm::New(CatId, TermNm, EnLangId, Cat_TermType->GetTermTypeId());
    LwOnto->GetTermBs()->AddTerm(Term);
  }
  printf("Done. (%d)\n", LwOnto->GetTermBs()->GetTerms());}

  // create links
  {printf("Creating links ...\n"); 
  for (int CatId=0; CatId<Cats; CatId++){
    TStr CatNm=DMozBs->GetCatNm(CatId);
    if (CatId%1000==0){printf("%d/%d\r", CatId, Cats);}
    // get linked-categories & link-types
    TUIntV SubCatTypeIdPrV; DMozBs->GetSubCatTypeIdPrV(CatId, SubCatTypeIdPrV);
    for (int SubCatN=0; SubCatN<SubCatTypeIdPrV.Len(); SubCatN++){
      // get subcategory data
      int SubCatId; TDMozSubCatType SubCatType;
      TDMozBs::GetSubCatTypeCatId(SubCatTypeIdPrV[SubCatN], SubCatType, SubCatId);
      TStr SubCatNm=DMozBs->GetCatNm(SubCatId);
      // create link
      int LinkTypeId=-1;
      switch (SubCatType){
        case dmsctNarrow: LinkTypeId=NT_LinkType->GetLinkTypeId(); break;
        case dmsctRelated: LinkTypeId=RT_LinkType->GetLinkTypeId(); break;
        case dmsctSymbolic: LinkTypeId=SL_LinkType->GetLinkTypeId(); break;
        default: LinkTypeId=-1;
      }
      if (LinkTypeId!=-1){
        LwOnto->GetLinkBs()->AddLink(CatId, LinkTypeId, SubCatId);
      }
    }
  }
  printf(" Done. (%d)\n", LwOnto->GetLinkBs()->GetLinks());}

  // return ontology
  return LwOnto;
}

PLwOnto TLwOnto::LoadVoc(const TStr& FNm) {
  // create ontology
  PLwOnto LwOnto = TLwOnto::New();
  // create language object
  int EnLangId = LwOnto->GetLangBs()->AddLang("EN", "English");
  // create term-types
  PLwTermType TermType = TLwTermType::New(0, "Term", EnLangId);
  LwOnto->GetTermTypeBs()->AddTermType(TermType);
  // create terms
  {printf("Creating terms ...\n");
  // load terms from file
  TStr VocFileStr = TStr::LoadTxt(FNm); 
  VocFileStr.DelChAll('\r');
  TStrV TermNmV; VocFileStr.SplitOnAllCh('\n', TermNmV);
  // add terms to base
  const int Terms = TermNmV.Len();
  for (int TermId = 0; TermId<Terms; TermId++){
    if (TermId%1000==0){printf("%d/%d\r", TermId, Terms);}
    TStr TermNm = TermNmV[TermId];
    // create term
    PLwTerm Term=TLwTerm::New(TermId, TermNm, EnLangId, TermType->GetTermTypeId());
    LwOnto->GetTermBs()->AddTerm(Term);
  }
  printf("Done. (%d)\n", LwOnto->GetTermBs()->GetTerms());}
  // return ontology
  return LwOnto;
}

/////////////////////////////////////////////////
// Light-Weight-Ontology-Grounding
void TLwOntoGround::SaveTxt(const TStr& FNm){
  // shortcuts
  PLwTermBs TermBs=LwOnto->GetTermBs();
  // save data
  TFOut FOut(FNm); FILE* fOut=FOut.GetFileId();
  fprintf(fOut, "===LwOnto-Begin======================\n");
  for (int TermN=0; TermN<TermBs->GetTerms(); TermN++){
    int TermId=TermBs->GetTermId(TermN);
    TStr TermNm=TermBs->GetTerm(TermId)->GetTermNm();
    fprintf(fOut, "TermId:%d TermNm:'%s'", TermId, TermNm.CStr());
    if (TermIdToConceptSpVH.IsKey(TermId)){
      PBowSpV ConceptSpV=TermIdToConceptSpVH.GetDat(TermId);
      TStr ConceptSpVStr=ConceptSpV->GetStr(BowDocBs, 15, 0.66);
      fprintf(fOut, " ConceptSpV:%s", ConceptSpVStr.CStr());
    }
    fprintf(fOut, "\n");
  }
}

PLwOntoGround TLwOntoGround::GetOntoGround(
 const PLwOnto& LwOnto, const PBowDocBs& BowDocBs,
 const TStr& LangNm, const bool& DocCatIsTermIdP,
 const double& CutWordWgtSumPrc){
  printf("Generating Ontology-Classifier...\n");
  // shortcuts
  PLwTermBs TermBs=LwOnto->GetTermBs();
  int Terms=TermBs->GetTerms();
  PLwLinkBs LinkBs=LwOnto->GetLinkBs();
  PLwLinkTypeBs LinkTypeBs=LwOnto->GetLinkTypeBs();
  int LangId=LwOnto->GetLangBs()->GetLangId(LangNm);
  int Docs=BowDocBs->GetDocs();
  // create tfidf
  printf("  Creating BowDocWgtBs ...");
  PBowDocWgtBs BowDocWgtBs=TBowDocWgtBs::New(BowDocBs, bwwtNrmTFIDF);
  PBowSim BowSim=TBowSim::New(bstCos);
  printf(" Done.\n");
  // collect documents per ontology-term
  printf("  Collecting documents per ontology-term ...\n");
  TIntIntVH TermIdToDIdVH; int PosCats=0; int NegCats=0;
  for (int DId=0; DId<Docs; DId++){
    printf("    Docs:%d/%d Pos:%d Neg:%d\r", 1+DId, Docs, PosCats, NegCats);
    for (int DocCIdN=0; DocCIdN<BowDocBs->GetDocCIds(DId); DocCIdN++){
      // get document-category
      int CId=BowDocBs->GetDocCId(DId, DocCIdN);
      TStr CatNm=BowDocBs->GetCatNm(CId);
      // get term-id
      if (DocCatIsTermIdP){
        int TermId=CatNm.GetInt();
        if (TermBs->IsTermId(TermId)){
          TermIdToDIdVH.AddDat(TermId).Add(DId); PosCats++;
        } else {NegCats++;}
      } else {
        if (TermBs->IsTermId(CatNm, LangId)){
          int TermId=TermBs->GetTermId(CatNm, LangId);
          TermIdToDIdVH.AddDat(TermId).Add(DId); PosCats++;
        } else {NegCats++;}
      }
    }
  }
  printf("    Docs:%d/%d Pos:%d Neg:%d\n", Docs, Docs, PosCats, NegCats);
  printf("  Done.\n");
  // create sub-terms & up-terms vectors
  printf("  Creating sub-terms & up-terms vectors ...");
  TIntIntVH Const_TermIdToSubTermIdVH;
  TIntIntVH TermIdToSubTermIdVH;
  TIntIntVH TermIdToUpTermIdVH;
  for (int TermN=0; TermN<Terms; TermN++){
    int TermId=TermBs->GetTermId(TermN);
    for (int LinkN=0; LinkN<LinkBs->GetFromLinks(TermId); LinkN++){
      int LinkTypeId; int DstTermId;
      LinkBs->GetFromLink(TermId, LinkN, LinkTypeId, DstTermId);
      TStr LinkTypeNm=LinkTypeBs->GetLinkType(LinkTypeId)->GetLinkTypeNm();
      if (LinkTypeNm=="NT"){
        Const_TermIdToSubTermIdVH.AddDat(TermId).Add(DstTermId);
        TermIdToSubTermIdVH.AddDat(TermId).Add(DstTermId);
        TermIdToUpTermIdVH.AddDat(DstTermId).Add(TermId);
      }
    }
  }
  printf("   Done.\n");
  // create centroids
  printf("  Creating centroids ...\n");
  THash<TInt, PBowSpV> TermIdToConceptSpVH;
  TIntIntVH TermIdToSubTermDIdVH;
  TIntH ProcTermIdH;
  int PrevActiveTerms=-1;
  forever{
    // count active nodes for processing
    int ActiveTerms=0;
    for (int TermN=0; TermN<Terms; TermN++){
      int TermId=TermBs->GetTermId(TermN);
      if ((TermIdToSubTermIdVH.IsKey(TermId))&&
       (TermIdToSubTermIdVH.GetDat(TermId).Len()>0)){
        ActiveTerms++;
      }
    }
    // stop if no change from previous round
    printf("    Active-Terms:%d\n", ActiveTerms);
    if (ActiveTerms==PrevActiveTerms){break;}
    PrevActiveTerms=ActiveTerms;
    // reduce active-nodes with zero-ancestors
    for (int TermN=0; TermN<Terms; TermN++){
      int TermId=TermBs->GetTermId(TermN);
      if (ProcTermIdH.IsKey(TermId)){continue;}
      if ((!TermIdToSubTermIdVH.IsKey(TermId))||
       (TermIdToSubTermIdVH.GetDat(TermId).Len()==0)){
        printf("    %d/%d\r", 1+TermN, Terms);
        ProcTermIdH.AddKey(TermId);
        // collect document-ids
        TIntV TermDIdV;
        if (TermIdToDIdVH.IsKey(TermId)){
          TermDIdV.AddV(TermIdToDIdVH.GetDat(TermId));}
        if (TermIdToSubTermDIdVH.IsKey(TermId)){
          TermDIdV.AddV(TermIdToSubTermDIdVH.GetDat(TermId));}
        // create concept-vector if any documents
        if (TermDIdV.Len()>0){
          PBowSpV ConceptSpV=
           TBowClust::GetConceptSpV(BowDocWgtBs, BowSim, TermDIdV, CutWordWgtSumPrc);
          TermIdToConceptSpVH.AddDat(TermId, ConceptSpV);
        }
        // correct upper-term
        if (TermIdToUpTermIdVH.IsKey(TermId)){
          TIntV& UpTermIdV=TermIdToUpTermIdVH.GetDat(TermId);
          for (int UpTermIdN=0; UpTermIdN<UpTermIdV.Len(); UpTermIdN++){
            int UpTermId=UpTermIdV[UpTermIdN];
            TermIdToSubTermIdVH.GetDat(UpTermId).DelIfIn(TermId);
            if (TermDIdV.Len()>0){
              TermIdToSubTermDIdVH.AddDat(UpTermId).AddV(TermDIdV);}
          }
        }
      }
    }
  }
  printf("  Done.\n");
  // create & return classifier
  PLwOntoGround OntoGround=
   TLwOntoGround::New(LwOnto, BowDocBs, BowDocWgtBs, TermIdToConceptSpVH);
  printf("Done.\n");
  return OntoGround;
}

PLwOntoGround TLwOntoGround::GetOntoGroundNN(const PLwOnto& LwOnto, 
        const PBowDocBs& BowDocBs, const TStr& LangNm) {

    printf("Generating Ontology-Classifier...\n");
    // shortcuts
    PLwTermBs TermBs=LwOnto->GetTermBs();
    const int Terms = TermBs->GetTerms();
    const int LangId = LwOnto->GetLangBs()->GetLangId(LangNm);
    const int Words = BowDocBs->GetWords();
    // create tfidf
    printf("  Creating BowDocWgtBs ...");
    PBowDocWgtBs BowDocWgtBs=TBowDocWgtBs::New(BowDocBs, bwwtNrmTFIDF);
    PBowSim BowSim=TBowSim::New(bstCos);
    printf(" Done.\n");
    // collect documents per ontology-term
    printf("  Collecting documents per ontology-term ... ");
    THash<TInt, PBowSpV> TermIdToConceptSpVH;
    for (int TermN = 0; TermN < Terms; TermN++){
        int TermId = TermBs->GetTermId(TermN);
        PLwTerm Term = TermBs->GetTerm(TermId);
        if (Term->GetLangId() != LangId) { continue; }
        // do nearest neighbour search
        PBowSpV TermSpV = BowDocBs->GetSpVFromHtmlStr(
            Term->GetTermNm(), BowDocWgtBs);
        TFltIntKdV SimDIdKdV;
        BowDocWgtBs->GetSimDIdV(TermSpV, BowSim, SimDIdKdV, false);
        TFltV TermV(Words); TermV.PutAll(0.0);
        for (int SimDIdKdN = 0; SimDIdKdN < SimDIdKdV.Len(); SimDIdKdN++) {
            PBowSpV DocSpV = BowDocWgtBs->GetSpV(SimDIdKdV[SimDIdKdN].Dat);
            const double Sim = SimDIdKdV[SimDIdKdN].Key;
            TBowLinAlg::AddVec(Sim, DocSpV, TermV);
        }
        TermIdToConceptSpVH.AddDat(TermId, TBowSpV::New(-1, TermV, TFlt::Eps));
    }
    printf("  Done.\n");
    // create & return classifier
    PLwOntoGround OntoGround = TLwOntoGround::New(LwOnto, 
        BowDocBs, BowDocWgtBs, TermIdToConceptSpVH);
    printf("Done.\n");
    return OntoGround;
}

void TLwOntoGround::ClassifySpV(
 const PBowSpV& QueryBowSpV, TSimTermIdPrV& SimTermIdPrV) const {
  PBowSim BowSim=TBowSim::New(bstCos);
  SimTermIdPrV.Clr();
  for (int TermN=0; TermN<TermIdToConceptSpVH.Len(); TermN++){
    int TermId=TermIdToConceptSpVH.GetKey(TermN);
    PBowSpV ConceptSpV=TermIdToConceptSpVH[TermN];
    double Sim=BowSim->GetSim(QueryBowSpV, ConceptSpV);
    TStr TermNm = GetLwOnto()->GetTermBs()->GetTerm(TermId)->GetTermNm();
    if (Sim > 0.0) { SimTermIdPrV.Add(TFltIntPr(Sim, TermId)); } //B: >0
  }
  SimTermIdPrV.Sort(false);
}

void TLwOntoGround::ClassifyStr(
 const TStr& QueryStr, TSimTermIdPrV& SimTermIdPrV) const {
  PBowDocBs BowDocBs=GetBowDocBs();
  PBowDocWgtBs BowDocWgtBs=GetBowDocWgtBs();
  PBowSpV QueryBowSpV=
   BowDocBs->GetSpVFromHtmlStr(QueryStr, BowDocWgtBs);
  ClassifySpV(QueryBowSpV, SimTermIdPrV);
}

void TLwOntoGround::ClassifyHtmlFNm(
 const TStr& QueryHtmlFNm, TSimTermIdPrV& SimTermIdPrV) const {
  PBowDocBs BowDocBs=GetBowDocBs();
  PBowDocWgtBs BowDocWgtBs=GetBowDocWgtBs();
  PBowSpV QueryBowSpV=
   BowDocBs->GetSpVFromHtmlFile(QueryHtmlFNm, BowDocWgtBs);
  ClassifySpV(QueryBowSpV, SimTermIdPrV);
}

void TLwOntoGround::SaveCfySetTxt(
 FILE* fTxt, TSimTermIdPrV& SimTermIdPrV, const int& MxRank){
  int Terms=(MxRank==-1) ? SimTermIdPrV.Len() : MxRank;
  for (int TermN=0; TermN<Terms; TermN++){
    double Sim=SimTermIdPrV[TermN].Val1;
    int TermId=SimTermIdPrV[TermN].Val2;
    TStr TermNm=LwOnto->GetTermBs()->GetTerm(TermId)->GetTermNm();
    fprintf(fTxt, "%2d. %.3f %s\n", 1+TermN, Sim, TermNm.CStr());
  }
}

void TLwOntoGround::SaveCfySetXml(
 FILE* fXml, TSimTermIdPrV& SimTermIdPrV, const int& MxRank){
  fprintf(fXml, "<OntoClassify>\n");
  int Terms=(MxRank==-1) ? SimTermIdPrV.Len() : MxRank;
  for (int TermN=0; TermN<Terms; TermN++){
    double Sim=SimTermIdPrV[TermN].Val1;
    int TermId=SimTermIdPrV[TermN].Val2;
    TStr TermNm=LwOnto->GetTermBs()->GetTerm(TermId)->GetTermNm();
    fprintf(fXml, "<Term Rank=\"%d\" Sim=\"%.3f\">%s</Term>\n",
     1+TermN, Sim, TermNm.CStr());
  }
  fprintf(fXml, "</OntoClassify>\n");
}

