/////////////////////////////////////////////////
// Includes
#include "cyc.h"

/////////////////////////////////////////////////
// Cyc-Vertex
TStr TCycVrtx::GetCycVrtxFlagStr(const TCycVrtxFlag& CycVrtxFlag){
  switch (CycVrtxFlag){
    case cvfUndef: return "Undef";
    case cvfNum: return "Num";
    case cvfStr: return "Str";
    case cvfCycL: return "CycL";
    case cvfCycLConst: return "CycLConst"; 
    case cvfCycLExpr: return "CycLExpr";
    case cvfCycLPred: return "CycLPred";
    case cvfBackLink: return "BackLink";
    case cvfHumanOk: return "HumanOk";
    case cvfHumanRelevant: return "HumanRelevant";
    case cfvHumanIrrelevant: return "HumanIrrelevant";
    case cfvHumanClarifying: return "HumanClarifying";
    default: Fail; return "";
  }
}

TStr TCycVrtx::GetFlagStr() const {
  TChA ChA;
  ChA+='[';
  for (int FlagId=cvfMn; FlagId<cvfMx; FlagId++){
    if (IsFlag(FlagId)){
      if (ChA.Len()>1){ChA+=' ';}
      ChA+=GetCycVrtxFlagStr(TCycVrtxFlag(FlagId));
    }
  }
  ChA+=']';
  return ChA;
}

/////////////////////////////////////////////////
// Cyc-Base
int TCycBs::AddVNm(const TStr& VNm){
  int VId;
  if (VNmToVrtxH.IsKey(VNm.CStr(), VId)){
    return VId;
  } else {
    VId=VNmToVrtxH.AddKey(VNm);
    TCycVrtx& Vrtx=VNmToVrtxH[VId];
    Vrtx.PutVId(VId);
    // set flags based on vertex-name string
    if (VNm.IsPrefix("~")){
      Vrtx.SetFlag(cvfBackLink);
    } else
    if (VNm.IsPrefix("#$")&&(!VNm.IsChIn(' '))){
      Vrtx.SetFlag(cvfCycL);
      if (VNm.Len()>2){
        char Ch=VNm[2];
        if (('a'<=Ch)&&(Ch<='z')){
          Vrtx.SetFlag(cvfCycLPred);
        } else {
          Vrtx.SetFlag(cvfCycLConst);
        }
      }
    } else
    if (VNm.IsPrefix("(#$")){
      Vrtx.SetFlag(cvfCycL);
      Vrtx.SetFlag(cvfCycLExpr);
    } else {
      if (VNm.IsFlt()){
        Vrtx.SetFlag(cvfNum);
      } else {
        Vrtx.SetFlag(cvfStr);
      }
    }
    return VId;
  }
}

void TCycBs::GetRelNmV(TStrV& RelNmV){
  TStrH RelNmH;
  // traverse vertices
  for (int VId=0; VId<GetVIds(); VId++){
    TStr VNm=GetVNm(VId);
    TCycVrtx& Vrtx=GetVrtx(VId);
    // traverse edges per vertice
    for (int EdgeN=0; EdgeN<Vrtx.GetEdges(); EdgeN++){
      // extract relation name
      TCycEdge& Edge=Vrtx.GetEdge(EdgeN);
      TStr RelNm=GetVNm(Edge.GetRelId());
      // add relation name to the pool
      RelNmH.AddKey(RelNm);
    }
  }
  // extract relation-name-vector
  RelNmH.GetKeyV(RelNmV);
}

TStr TCycBs::GetDocStrFromCycLConstStr(const TStr& CycLConstStr){
  TChA ChA=CycLConstStr;
  TChA DstChA;
  if ((ChA.Len()>2)&&(ChA[0]=='#')&&(ChA[1]=='$')){
    int ChN=2;
    while (ChN<ChA.Len()){
      if (DstChA.Len()>0){DstChA+=' ';}
      if ((ChA[ChN]=='-')||(ChA[ChN]=='_')||(ChA[ChN]==':')){
        DstChA+="-"; ChN++;
      } else
      if (('0'<=ChA[ChN])&&(ChA[ChN]<='9')){
        do {
          DstChA+=ChA[ChN]; ChN++;
        } while ((ChN<ChA.Len())&&('0'<=ChA[ChN])&&(ChA[ChN]<='9'));
      } else
      if (('a'<=ChA[ChN])&&(ChA[ChN]<='z')){
        do {
          DstChA+=ChA[ChN]; ChN++;
        } while ((ChN<ChA.Len())&&('a'<=ChA[ChN])&&(ChA[ChN]<='z'));
      } else
      if (('A'<=ChA[ChN])&&(ChA[ChN]<='Z')){
        if ((ChN+1<ChA.Len())&&('a'<=ChA[ChN+1])&&(ChA[ChN+1]<='z')){
          do {
            DstChA+=ChA[ChN]; ChN++;
          } while ((ChN<ChA.Len())&&('a'<=ChA[ChN])&&(ChA[ChN]<='z'));
        } else {
          do {
            DstChA+=ChA[ChN]; ChN++;
          } while (((ChN==ChA.Len()-1)&&('A'<=ChA[ChN])&&(ChA[ChN]<='Z'))||
            ((ChN+1<ChA.Len())&&('A'<=ChA[ChN+1])&&(ChA[ChN+1]<='Z')));
        }
      } else {
        DstChA=""; break;
      }
    }
  } else {
    DstChA="";
  }
  return DstChA;
}

PLwOnto TCycBs::LoadCycVoc(const TStr& CycBsFNm, const bool& HumanOkOnlyP){
  // load cyc-base
  printf("Loading Cyc-Base from Binary-File '%s' ... ", CycBsFNm.CStr());
  PCycBs CycBs=TCycBs::LoadBin(CycBsFNm);
  int VIds=CycBs->GetVIds();
  printf("Done.\n");

  // create ontology
  PLwOnto LwOnto=TLwOnto::New();

  // create language object
  int EnLangId=LwOnto->GetLangBs()->AddLang("EN", "English");

  // create term-types
  PLwTermType C_TermType=TLwTermType::New(0, "Class", EnLangId);
  PLwTermType L_TermType=TLwTermType::New(1, "Lexical", EnLangId);
  int C_TermTypeId=LwOnto->GetTermTypeBs()->AddTermType(C_TermType);
  int L_TermTypeId=LwOnto->GetTermTypeBs()->AddTermType(L_TermType);

  // create link-types
  {printf("Creating link-types ...");
  TStrV RelNmV; CycBs->GetRelNmV(RelNmV);
  for (int RelNmN=0; RelNmN<RelNmV.Len(); RelNmN++){
    TStr RelNm=RelNmV[RelNmN];
    if (RelNm[0]=='~'){continue;}
    PLwLinkType LinkType=TLwLinkType::New(RelNmN, RelNm, EnLangId, "");
    LwOnto->GetLinkTypeBs()->AddLinkType(LinkType);
  }
  printf(" Done. (%d)\n", LwOnto->GetLinkTypeBs()->GetLinkTypes());}

  // create terms
  {printf("Creating terms ...\n");
  for (int VId=0; VId<VIds; VId++){
    if (VId%1000==0){printf("%d/%d\r", VId, VIds);}
    TCycVrtx& Vrtx=CycBs->GetVrtx(VId);
    // skip predicates, non-strings, non-human classes
    if (Vrtx.IsFlag(cvfCycLPred)){continue;}
    if ((!Vrtx.IsFlag(cvfStr))&&
     (HumanOkOnlyP&&(!Vrtx.IsFlag(cvfHumanOk)))){continue;}
    // create term
    TStr VNm=CycBs->GetVNm(VId);
    int TermTypeId=(Vrtx.IsFlag(cvfStr)) ? L_TermTypeId : C_TermTypeId;
    PLwTerm Term=TLwTerm::New(VId, VNm, EnLangId, TermTypeId);
    LwOnto->GetTermBs()->AddTerm(Term);
  }
  printf("Done. (%d)\n", LwOnto->GetTermBs()->GetTerms());}

  {printf("Creating links ...\n");
  for (int VId=0; VId<VIds; VId++){
    if (VId%1000==0){printf("%d/%d\r", VId, VIds);}
    if (!LwOnto->GetTermBs()->IsTermId(VId)){continue;}
    TCycVrtx& Vrtx=CycBs->GetVrtx(VId);
    for (int EdgeN=0; EdgeN<Vrtx.GetEdges(); EdgeN++){
      TCycEdge& Edge=Vrtx.GetEdge(EdgeN);
      int DstVId=Edge.GetDstVId();
      if (!LwOnto->GetTermBs()->IsTermId(DstVId)){continue;}
      TStr RelNm=CycBs->GetVNm(Edge.GetRelId());
      if (LwOnto->GetLinkTypeBs()->IsLinkType(RelNm, EnLangId)){
        int LinkTypeId=LwOnto->GetLinkTypeBs()->GetLinkTypeId(RelNm, EnLangId);
        LwOnto->GetLinkBs()->AddLink(VId, LinkTypeId, DstVId);
      }
    }
  }
  printf("Done. (%d)\n", LwOnto->GetLinkBs()->GetLinks());}

  // return ontology
  return LwOnto;
}

void TCycBs::SaveTxt(const TStr& FNm){
  TFOut FOut(FNm); FILE* fOut=FOut.GetFileId();
  for (int VId=0; VId<GetVIds(); VId++){
    TStr VNm=GetVNm(VId);
    TCycVrtx& Vrtx=GetVrtx(VId);
    TStr FlagStr=Vrtx.GetFlagStr();
    fprintf(fOut, "(%d) %s - %s\n", VId, VNm.CStr(), FlagStr.CStr());
    for (int EdgeN=0; EdgeN<Vrtx.GetEdges(); EdgeN++){
      TCycEdge& Edge=Vrtx.GetEdge(EdgeN);
      TStr RelNm=GetVNm(Edge.GetRelId());
      TStr DstVNm=GetVNm(Edge.GetDstVId());
      fprintf(fOut, "     %d. [%s] --> %s\n", 1+EdgeN, RelNm.CStr(), DstVNm.CStr());
    }
  }
}

void TCycBs::_SaveTaxonomyTxt(FILE* fOut, 
 const int& Lev, TIntPrV& RelIdVIdPrV, TIntIntH& VIdToLevH){
  for (int VidN=0; VidN<RelIdVIdPrV.Len(); VidN++){
    int FromRelId=RelIdVIdPrV[VidN].Val1;
    int SrcVId=RelIdVIdPrV[VidN].Val2;
    TStr SrcVNm=GetVNm(SrcVId);
    TCycVrtx& SrcVrtx=GetVrtx(SrcVId);
    if (!SrcVrtx.IsFlag(cvfHumanOk)){continue;}
    TStr FlagStr=SrcVrtx.GetFlagStr();
    if (FromRelId==-1){
      if (Lev>0){fprintf(fOut, "===upper");} else {fprintf(fOut, "===lower");}
      fprintf(fOut, "=======================================================\n");
      fprintf(fOut, "%s - %s\n", SrcVNm.CStr(), FlagStr.CStr());
    } else {
      TStr FromRelNm=GetVNm(FromRelId);
      fprintf(fOut, "%*c[%s] --> %s\n", (Lev-1)*5, ' ', FromRelNm.CStr(), SrcVNm.CStr());
    }
    TIntPrV UpRelIdVIdPrV;
    for (int EdgeN=0; EdgeN<SrcVrtx.GetEdges(); EdgeN++){
      TCycEdge& Edge=SrcVrtx.GetEdge(EdgeN);
      int RelId=Edge.GetRelId();
      int DstVId=Edge.GetDstVId();
      TStr RelNm=GetVNm(RelId);
      TStr DstVNm=GetVNm(DstVId);
      if (Lev>0){
        // upper taxonomy
        if ((RelNm=="#$isa")||(RelNm=="#$genls")){
          if (!VIdToLevH.IsKey(DstVId)){
            VIdToLevH.AddDat(DstVId, Lev+1);
            UpRelIdVIdPrV.Add(TIntPr(RelId, DstVId));
          }
        }
      } else {
        // lower taxonomy
        if ((RelNm=="~#$isa")||(RelNm=="~#$genls")){
          if (!VIdToLevH.IsKey(DstVId)){
            VIdToLevH.AddDat(DstVId, Lev-1);
            UpRelIdVIdPrV.Add(TIntPr(RelId, DstVId));
          }
        }
      }
    } 
    // recursive call
    if (Lev>0){
      _SaveTaxonomyTxt(fOut, Lev+1, UpRelIdVIdPrV, VIdToLevH);
    } else {
      _SaveTaxonomyTxt(fOut, Lev-1, UpRelIdVIdPrV, VIdToLevH);
    }
  }
}

void TCycBs::SaveTaxonomyTxt(const TStr& FNm){
  TFOut FOut(FNm); FILE* fOut=FOut.GetFileId();
  for (int VId=0; VId<GetVIds(); VId++){
    printf("%d/%d (%.1f%%)\r", 1+VId, GetVIds(), 100.0*(1+VId)/GetVIds());
    //if (VId>10){break;}
    // upper taxonomy
    {int Lev=0;
    TIntIntH VIdToLevH; VIdToLevH.AddDat(VId, Lev);
    TIntPrV UpRelIdVIdPrV; UpRelIdVIdPrV.Add(TIntPr(-1, VId));
    _SaveTaxonomyTxt(fOut, Lev+1, UpRelIdVIdPrV, VIdToLevH);}
    // lower taxonomy
    {int Lev=0;
    TIntIntH VIdToLevH; VIdToLevH.AddDat(VId, Lev);
    TIntPrV UpRelIdVIdPrV; UpRelIdVIdPrV.Add(TIntPr(-1, VId));
    _SaveTaxonomyTxt(fOut, Lev-1, UpRelIdVIdPrV, VIdToLevH);}
  }
  printf("\n");
}

void TCycBs::SaveStatTxt(const TStr& FNm){
  // open statistics objects
  PMom CommentLenMom=TMom::New();
  PMom StrsLenMom=TMom::New();
  PMom HumanOk_StrsLenMom=TMom::New();
  PMom ClassesPerStrMom=TMom::New();
  PMom StrsPerClassMom=TMom::New();
  PMom HumanOk_StrsPerClassMom=TMom::New();
  PMom IsasPerClassMom=TMom::New();
  PMom HumanOk_IsasPerClassMom=TMom::New();
  PMom GenlsPerClassMom=TMom::New();
  PMom HumanOk_GenlsPerClassMom=TMom::New();
  PMom IsasGenlsPerClassMom=TMom::New();
  PMom HumanOk_IsasGenlsPerClassMom=TMom::New();
  int Classes=0;
  int HumanOk_Classes=0;
  int Comments=0;
  int HumanOk_Comments=0;

  // collect statistics
  for (int VId=0; VId<GetVIds(); VId++){
    TStr VNm=GetVNm(VId);
    TCycVrtx& Vrtx=GetVrtx(VId);
    TStr FlagStr=Vrtx.GetFlagStr();
    // local counters
    int StrsLen=0;
    int ClassesPerStrInstance=0;
    int NmStrs=0; int BackLinkNmStrs=0;
    int Isas=0; int Genls=0;
    // traverse edges
    for (int EdgeN=0; EdgeN<Vrtx.GetEdges(); EdgeN++){
      TCycEdge& Edge=Vrtx.GetEdge(EdgeN);
      TStr RelNm=GetVNm(Edge.GetRelId());
      TStr DstVNm=GetVNm(Edge.GetDstVId());
      TCycVrtx& DstVrtx=GetVrtx(Edge.GetDstVId());
      // comments
      if (RelNm=="#$comment"){
        CommentLenMom->Add(DstVNm.Len());
        Comments++; 
        if (Vrtx.IsFlag(cvfHumanOk)){HumanOk_Comments++;}
      }
      // counters
      if (DstVrtx.IsFlag(cvfStr)){StrsLen+=DstVNm.Len();}
      if (Vrtx.IsFlag(cvfStr)){ClassesPerStrInstance++;}
      if (RelNm=="#$nameString"){NmStrs++;}
      if (RelNm=="~#$nameString"){BackLinkNmStrs++;}
      if (RelNm=="#$isa"){Isas++;}
      if (RelNm=="#$genls"){Genls++;}
    }
    // update statistics
    if (Vrtx.IsFlag(cvfCycL)){Classes++;}
    if (Vrtx.IsFlag(cvfCycL)){StrsLenMom->Add(StrsLen);}
    if (Vrtx.IsFlag(cvfStr)){ClassesPerStrMom->Add(BackLinkNmStrs);}
    if (Vrtx.IsFlag(cvfCycL)){StrsPerClassMom->Add(NmStrs);}
    if (Vrtx.IsFlag(cvfCycL)){IsasPerClassMom->Add(Isas);}
    if (Vrtx.IsFlag(cvfCycL)){GenlsPerClassMom->Add(Genls);}
    if (Vrtx.IsFlag(cvfCycL)){IsasGenlsPerClassMom->Add(Isas+Genls);}
    if (Vrtx.IsFlag(cvfHumanOk)){
      if (Vrtx.IsFlag(cvfCycL)){HumanOk_Classes++;}
      if (Vrtx.IsFlag(cvfCycL)){HumanOk_StrsLenMom->Add(StrsLen);}
      if (Vrtx.IsFlag(cvfCycL)){HumanOk_StrsPerClassMom->Add(NmStrs);}
      if (Vrtx.IsFlag(cvfCycL)){HumanOk_IsasPerClassMom->Add(Isas);}
      if (Vrtx.IsFlag(cvfCycL)){HumanOk_GenlsPerClassMom->Add(Genls);}
      if (Vrtx.IsFlag(cvfCycL)){HumanOk_IsasGenlsPerClassMom->Add(Isas+Genls);}
    }
  }
  // close statistics objects
  CommentLenMom->Def();
  StrsLenMom->Def();
  HumanOk_StrsLenMom->Def();
  ClassesPerStrMom->Def();
  StrsPerClassMom->Def();
  HumanOk_StrsPerClassMom->Def();
  IsasPerClassMom->Def();
  HumanOk_IsasPerClassMom->Def();
  GenlsPerClassMom->Def();
  HumanOk_GenlsPerClassMom->Def();
  IsasGenlsPerClassMom->Def();
  HumanOk_IsasGenlsPerClassMom->Def();

  // save statistics to file
  {TFOut FOut(FNm); FILE* fOut=FOut.GetFileId();
  fprintf(fOut, "CommentLen: %s\n", CommentLenMom->GetStr().CStr());
  fprintf(fOut, "StrsLen: %s\n", StrsLenMom->GetStr().CStr());
  fprintf(fOut, "HumanOk_StrsLen: %s\n", HumanOk_StrsLenMom->GetStr().CStr());
  fprintf(fOut, "ClassesPerStr: %s\n", ClassesPerStrMom->GetStr().CStr());
  fprintf(fOut, "StrsPerClass: %s\n", StrsPerClassMom->GetStr().CStr());
  fprintf(fOut, "HumanOk_StrsPerClass: %s\n", HumanOk_StrsPerClassMom->GetStr().CStr());
  fprintf(fOut, "IsasPerClass: %s\n", IsasPerClassMom->GetStr().CStr());
  fprintf(fOut, "HumanOk_IsasPerClass: %s\n", HumanOk_IsasPerClassMom->GetStr().CStr());
  fprintf(fOut, "GenlsPerClass: %s\n", GenlsPerClassMom->GetStr().CStr());
  fprintf(fOut, "HumanOk_GenlsPerClass: %s\n", HumanOk_GenlsPerClassMom->GetStr().CStr());
  fprintf(fOut, "IsasGenlsPerClass: %s\n", IsasGenlsPerClassMom->GetStr().CStr());
  fprintf(fOut, "HumanOk_IsasGenlsPerClass: %s\n", HumanOk_IsasGenlsPerClassMom->GetStr().CStr());
  fprintf(fOut, "Classes: %d\n", Classes);
  fprintf(fOut, "HumanOk_Classes: %d\n", HumanOk_Classes);
  fprintf(fOut, "Comments: %d\n", Comments);
  fprintf(fOut, "HumanOk_Comments: %d\n", HumanOk_Comments);
  fprintf(fOut, "End");}
}

PCycBs TCycBs::LoadCycXmlDump(const TStr& FPath){
  // file-names
  TStr NrFPath=TStr::GetNrFPath(FPath);
  TStr CycLexiconFNm=NrFPath+"lexicon-dump.xml";
  TStr CycTaxonomyFNm=NrFPath+"taxonomy-dump.xml";
  TStr CycRelevanceFNm=NrFPath+"relevance-dump.xml";
  TStr CycKBaseFNm=NrFPath+"kb-dump.xml";                                                 

  // create cyc-base
  PCycBs CycBs=TCycBs::New();

  // lexicon
  {printf("Processing Lexicon %s ...\n", CycLexiconFNm.CStr());
  PSIn CycLexiconSIn=TFIn::New(CycLexiconFNm);
  PXmlDoc XmlDoc; int XmlDocs=0;
  TStr PrevCycWStr; TStr PrevCycLStr;
  forever{
    // statistics
    XmlDocs++; if (XmlDocs%1000==0){printf("%d Docs\r", XmlDocs);}
    // load xml-tree
    XmlDoc=TXmlDoc::LoadTxt(CycLexiconSIn);
    if (!XmlDoc->IsOk()){
      printf("%s - %s\n", PrevCycWStr.CStr(), PrevCycLStr.CStr());
      Fail;
    }
    // extract fields from xml-tree
    PXmlTok TopTok=XmlDoc->GetTok();
    if (TopTok->IsTag("end")){break;}
    IAssert(TopTok->IsTag("word"));
    TStr CycWStr=TopTok->GetArgVal("string");
    TStr CycLStr=TopTok->GetArgVal("cycl");
    PrevCycWStr=CycWStr; PrevCycLStr;
    // insert data
    CycBs->AddEdge(CycLStr, "#$nameString", CycWStr);
    CycBs->AddEdge(CycWStr, "~#$nameString", CycLStr);
  }
  printf("%d Docs\nDone.\n", XmlDocs);}

  // taxonomy
  {printf("Processing Taxonomy %s ...\n", CycTaxonomyFNm.CStr());
  PSIn CycTaxonomySIn=TFIn::New(CycTaxonomyFNm);
  PXmlDoc XmlDoc; int XmlDocs=0;
  TStr PrevSrcCycLStr;
  forever{
    // statistics
    XmlDocs++;
    if (XmlDocs%1000==0){
      printf("%d Docs\r", XmlDocs);}
    // load xml-tree
    XmlDoc=TXmlDoc::LoadTxt(CycTaxonomySIn);
    if (!XmlDoc->IsOk()){
      printf("%s\n", PrevSrcCycLStr.CStr());
      Fail;
    }
    // extract fields from xml-tree
    PXmlTok TopTok=XmlDoc->GetTok();
    if (TopTok->IsTag("end")){break;}
    IAssert(TopTok->IsTag("term"));
    TStr SrcCycLStr=TopTok->GetArgVal("cycl");
    PrevSrcCycLStr=SrcCycLStr;
    for (int SubTokN=0; SubTokN<TopTok->GetSubToks(); SubTokN++){
      PXmlTok SubTok=TopTok->GetSubTok(SubTokN);
      TStr DstCycLStr=SubTok->GetTagNm();
      if (SubTok->IsTag("isa")){
        DstCycLStr=SubTok->GetArgVal("value");
        CycBs->AddEdge(SrcCycLStr, "#$isa", DstCycLStr);
        CycBs->AddEdge(DstCycLStr, "~#$isa", SrcCycLStr);
      } else
      if (SubTok->IsTag("genl")){
        DstCycLStr=SubTok->GetArgVal("value");
        CycBs->AddEdge(SrcCycLStr, "#$genls", DstCycLStr);
        CycBs->AddEdge(DstCycLStr, "~#$genls", SrcCycLStr);
      } else {
        Fail;
      }
    }
  }
  printf("%d Docs\nDone.\n", XmlDocs);}

  // relevance
  {printf("Processing Relevance %s ...\n", CycRelevanceFNm.CStr());
  PSIn CycRelevanceSIn=TFIn::New(CycRelevanceFNm);
  PXmlDoc XmlDoc; int XmlDocs=0;
  TStr PrevCycStr;
  forever{
    // statistics
    XmlDocs++;
    if (XmlDocs%1000==0){
      printf("%d Docs\r", XmlDocs);}
    // load xml-tree
    XmlDoc=TXmlDoc::LoadTxt(CycRelevanceSIn);
    if (!XmlDoc->IsOk()){
      printf("%s\n", PrevCycStr.CStr());
      Fail;
    }
    // extract fields from xml-tree
    PXmlTok TopTok=XmlDoc->GetTok();
    if (TopTok->IsTag("end")){break;}
    IAssert(TopTok->IsTag("term"));
    TStr CycStr=TopTok->GetArgVal("cyc");
    PrevCycStr=CycStr;
    //IAssert(CycBs->IsVNm(CycStr));
    if (CycBs->IsVNm(CycStr)){
      if (TopTok->GetArgVal("thcl")=="T"){
        CycBs->GetVrtx(CycStr).SetFlag(cvfHumanRelevant, true);}
      if (TopTok->GetArgVal("irrel")=="T"){
        CycBs->GetVrtx(CycStr).SetFlag(cfvHumanIrrelevant, true);}
      if (TopTok->GetArgVal("clarifying")=="T"){
        CycBs->GetVrtx(CycStr).SetFlag(cfvHumanClarifying, true);}
      if ((TopTok->GetArgVal("thcl")=="T")||(TopTok->GetArgVal("clarifying")=="T")){
        CycBs->GetVrtx(CycStr).SetFlag(cvfHumanOk, true);}
    } else {
      //printf("%s\n", CycStr.CStr());
    }
  }
  printf("%d Docs\nDone.\n", XmlDocs);}

  // knowledge-base
  {printf("Processing KBase %s ...\n", CycKBaseFNm.CStr());
  PSIn CycKBaseSIn=TFIn::New(CycKBaseFNm);
  PXmlDoc XmlDoc; int XmlDocs=0;
  TStr PrevCycLStr; TStrV PrevArgCycLStrV;
  TStrIntH HdCycLToFq;
  forever{
    // statistics
    XmlDocs++;
    if (XmlDocs%1000==0){
      printf("%d Docs\r", XmlDocs);}
    //if (XmlDocs>10000){break;}
    // load xml-tree
    XmlDoc=TXmlDoc::LoadTxt(CycKBaseSIn);
    if (!XmlDoc->IsOk()){
      printf("%s\n", PrevCycLStr.CStr());
      for (int ArgN=0; ArgN<PrevArgCycLStrV.Len(); ArgN++){
        printf(" [%s]", PrevArgCycLStrV[ArgN].CStr());}
      printf("\n");
      Fail;
    }
    // extract fields from xml-tree
    PXmlTok TopTok=XmlDoc->GetTok();
    if (TopTok->IsTag("end")){break;}
    IAssert(TopTok->IsTag("sentence"));
    TStr CycLStr=TopTok->GetArgVal("cycl");
    TXmlTokV ArgXmlTokV; XmlDoc->GetTagTokV("sentence|arg", ArgXmlTokV);
    TStrV ArgCycLStrV;
    for (int ArgN=0; ArgN<ArgXmlTokV.Len(); ArgN++){
      PXmlTok Tok=ArgXmlTokV[ArgN];
      IAssert(Tok->IsTag("arg"));
      if (Tok->IsArg("cycl")){
        TStr ArgCycLStr=Tok->GetArgVal("cycl");
        ArgCycLStrV.Add(ArgCycLStr);
      } else {
        ArgCycLStrV.Add("Empty");
      }
    }
    PrevCycLStr=CycLStr;
    PrevArgCycLStrV=ArgCycLStrV;
    if (ArgCycLStrV.Len()>0){
      HdCycLToFq.AddDat(ArgCycLStrV[0]+" - "+TInt::GetStr(ArgCycLStrV.Len()-1))++;}
    // insert
    if (ArgCycLStrV.Len()==3){
      TStr PredNm=ArgCycLStrV[0];
      if ((PredNm!="#$isa")&&(PredNm!="#$termOfUnit")&&(PredNm!="#$genls")){
        TStr BackLinkPredNm=TStr("~")+PredNm;
        TStr Arg1=ArgCycLStrV[1];
        TStr Arg2=ArgCycLStrV[2];
        CycBs->AddEdge(Arg1, PredNm, Arg2);
        CycBs->AddEdge(Arg2, BackLinkPredNm, Arg1);
      }
    }
  }
  // output top cycl relations
  {TFOut CycLSOut("CycKB-CycLFq.Stat.Txt"); FILE* fCycL=CycLSOut.GetFileId();
  TIntStrPrV FqCycLStrPrV; HdCycLToFq.GetDatKeyPrV(FqCycLStrPrV); 
  FqCycLStrPrV.Sort(false);
  for (int CycLN=0; CycLN<FqCycLStrPrV.Len(); CycLN++){
    fprintf(fCycL, "%6d. %s\n", 1+FqCycLStrPrV[CycLN].Val1, FqCycLStrPrV[CycLN].Val2.CStr());
  }}
  printf("%d Docs\nDone.\n", XmlDocs);}

  // return cyc-base
  return CycBs;
}

