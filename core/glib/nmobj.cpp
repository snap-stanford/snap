/////////////////////////////////////////////////
// Includes
#include "nmobj.h"

/////////////////////////////////////////////////
// Named-Objects
TNmObjBs::TNmObjBs(
 const TSwSetType& SwSetType, const PSIn& CustSwSetSIn,
 const PSIn& NrWordBsSIn, const PSIn& WordTypeBsSIn, const TStr& MteFNm):
  ChDef(),
  WordStrToNrH(),
  WordStrVToNmObjAttrSetH(),
  NmObjWordStrVToNrH(),
  NmObjWordStrVToDocIdVH(),
  DocNmToNmObjDocH(){
  // get character set
  ChDef=THtmlLxChDef::GetChDef();
  // standard-stopwords
  SwSet=TSwSet::GetSwSet(SwSetType);
  // custom-stopwords
  LoadCustSwSet(CustSwSetSIn);
  // load word normalizations
  LoadNrWordBs(NrWordBsSIn);
  // load phrase aliases & types
  LoadNmObjTypeBs(WordTypeBsSIn);
  // load MulTextEast
  if (!MteFNm.Empty()){
    /*MteLex=TMteLex::LoadBin(MteFNm);*/}
}

TStr TNmObjBs::PeriodTagStr="<.>";
TStr TNmObjBs::BreakTagStr="<br>";
TStr TNmObjBs::ParagraphTagStr="<p>";
TStr TNmObjBs::EofTagStr="<eof>";

void TNmObjBs::LoadCustSwSet(const PSIn& SIn){
  if (SIn.Empty()){return;}
  TILx Lx(SIn, TFSet(iloCmtAlw, iloRetEoln, iloExcept));
  // traverse lines
  Lx.GetSym(syLn, syEof);
  while (Lx.Sym!=syEof){
    // get stop-phrase string
    TStr WordStrVStr=Lx.Str;
    // split phrase to words
    TStrV WordStrV; WordStrVStr.SplitOnWs(WordStrV);
    if (!WordStrV.Empty()){
      // define phrase as stop-word
      WordStrVToNmObjAttrSetH.AddDat(WordStrV).Incl(noaIgnore);
    }
    // get next symbol
    Lx.GetSym(syLn, syEof);
  }
}

void TNmObjBs::LoadNrWordBs(const PSIn& SIn){
  if (SIn.Empty()){return;}
  TILx Lx(SIn, TFSet(iloCmtAlw, iloRetEoln, iloExcept));
  // traverse lines
  Lx.GetSym(syQStr, syEoln, syEof);
  while (Lx.Sym!=syEof){
    if (Lx.Sym==syQStr){
      // get normalized word
      TStr NrWordStr=Lx.Str;
      // get inflected words
      Lx.GetSym(syColon);
      Lx.GetSym(syQStr, syEoln);
      while (Lx.Sym!=syEoln){
        // get inflected word
        TStr WordStr=Lx.Str;
        // test if inflected word already exists
        if (WordStrToNrH.IsKey(WordStr)){
          printf("Word already normalized (%s)", WordStr.CStr());}
         // add inflected word and corresponding normalized word
        WordStrToNrH.AddDat(WordStr, NrWordStr);
        //printf("'%s' ->'%s'\n", WordStr.CStr(), NrWordStr.CStr());
        Lx.GetSym(syQStr, syEoln);
      }
      Lx.GetSym(syQStr, syEoln, syEof);
    } else
    if (Lx.Sym==syEoln){
      // empty line
      Lx.GetSym(syQStr, syEoln, syEof);
    } else {
      Fail;
    }
  }
}

TStr TNmObjBs::GetNrWordStr(const TStr& _WordStr) const {
  // remove underline
  TStr WordStr=_WordStr;
  if (WordStr.IsChIn('_')){
    WordStr.ChangeChAll('_', '$');
  }
  // check custom word-normalization table
  TStr NrWordStr;
  if (WordStrToNrH.IsKeyGetDat(WordStr, NrWordStr)){
    // normalization found
    return NrWordStr;
  } else
  if (MteLex.Empty()){
    return WordStr;
  } else {
    // check MulTextEast word-normalization table
    TStr UcWordStr=ChDef->GetUcStr(WordStr); // get upper-case word
    TStr NrUcWordStr;
    if (MteLex->IsInfWord(UcWordStr, NrUcWordStr)){
      // normalization found
      if (UcWordStr==NrUcWordStr){
        // normalization word same as inflected word
        return WordStr;
      } else
      if (IsAllCapWordStr(WordStr)){
        // original word is in upper-case
        return NrUcWordStr;
      } else
      if (IsFirstCapWordStr(WordStr)){
        // original word has capitalization
        TChA NrWordChA(NrUcWordStr);
        ChDef->GetLcChA(NrWordChA);
        if (NrWordChA.Len()>0){
          NrWordChA.PutCh(0, ChDef->GetUc(NrWordChA[0]));}
        return NrWordChA;
      } else {
        // original word is in lower-case
        TStr NrLcWordStr=ChDef->GetLcStr(NrUcWordStr);
        return NrLcWordStr;
      }
    } else {
      // no normalization
      return WordStr;
    }
  }
}

TNmObjAttr TNmObjBs::GetNmObjTypeFromStr(const TStr& Str){
  if (Str=="Ignore"){return noaIgnore;}
  if (Str=="Standalone"){return noaStandalone;}
  if (Str=="AsCapitalized"){return noaAsCapitalized;}
  if (Str=="Unperiod"){return noaUnperiod;}
  if (Str=="Acronym"){return noaAcronym;}
  if (Str=="FirstName"){return noaFirstName;}
  if (Str=="Person"){return noaPerson;}
  if (Str=="Company"){return noaCompany;}
  if (Str=="Organization"){return noaOrganization;}
  if (Str=="Country"){return noaCountry;}
  if (Str=="Geography"){return noaGeography;}
  TExcept::Throw("Invalid Named-Object name.", Str);
  return noaIgnore;
}

void TNmObjBs::LoadNmObjTypeBs(const PSIn& SIn){
  if (SIn.Empty()){return;}
  TILx Lx(SIn, TFSet(iloCmtAlw, iloRetEoln, iloExcept));
  // traverse lines
  Lx.GetSym(syQStr, syIdStr, syEoln, syEof);
  while (Lx.Sym!=syEof){
    if ((Lx.Sym==syQStr)||(Lx.Sym==syIdStr)){
      TVec<TStrV> NmObjWordStrVV;
      TB32Set NmObjAttrSet;
      while ((Lx.Sym==syQStr)||(Lx.Sym==syIdStr)){
        if (Lx.Sym==syQStr){
          // named-object word-string
          TStr WordStrVStr=Lx.Str;
          TStrV WordStrV; WordStrVStr.SplitOnWs(WordStrV);
          NmObjWordStrVV.Add(WordStrV);
        } else
        if (Lx.Sym==syIdStr){
          // named-object attribute
          TNmObjAttr NmObjAttr=TNmObjBs::GetNmObjTypeFromStr(Lx.Str);
          NmObjAttrSet.Incl(NmObjAttr);
        } else {
          Fail;
        }
        Lx.GetSym(syQStr, syIdStr, syEoln, syEof);
      }
      // assign 'defined' attribute if 'not ignore'
      if (!NmObjAttrSet.In(noaIgnore)){
        NmObjAttrSet.Incl(noaDefined);}
      // assign attribute-sets to word-vectors
      for (int NmObjN=0; NmObjN<NmObjWordStrVV.Len(); NmObjN++){
        WordStrVToNmObjAttrSetH.AddDat(NmObjWordStrVV[NmObjN])|=NmObjAttrSet;
      }
      // assign aliases
      {for (int NmObjN=1; NmObjN<NmObjWordStrVV.Len(); NmObjN++){
        NmObjWordStrVToNrH.AddDat(NmObjWordStrVV[NmObjN], NmObjWordStrVV[0]);
      }}
      // get eoln
    } else
    if (Lx.Sym==syEoln){
      // empty line
      Lx.GetSym(syQStr, syEoln, syEof);
    } else {
      Fail;
    }
  }
}

bool TNmObjBs::IsNmObjAttr(const TStr& WordStr, const TNmObjAttr& NmObjAttr) const {
  TStrV WordStrV; WordStrV.Add(WordStr);
  return IsNmObjAttr(WordStrV, NmObjAttr);
}

bool TNmObjBs::IsNmObjAttr(const TStrV& WordStrV, const TNmObjAttr& NmObjAttr) const {
  int WordStrVToNmObjAttrSetP=WordStrVToNmObjAttrSetH.GetKeyId(WordStrV);
  if (WordStrVToNmObjAttrSetP==-1){return false;}
  else {return WordStrVToNmObjAttrSetH[WordStrVToNmObjAttrSetP].In(NmObjAttr);}
}

bool TNmObjBs::IsFirstCapWordStr(const TStr& Str) const {
  if (Str.Empty()){return false;}
  char FirstCh=Str[0];
  if (!ChDef->IsAlpha(FirstCh)){return false;}
  if (!ChDef->IsUc(FirstCh)){return false;}
  return true;
}

bool TNmObjBs::IsAllCapWordStr(const TStr& Str) const {
  TChA ChA=Str;
  for (int ChN=0; ChN<ChA.Len(); ChN++){
    if (!ChDef->IsUc(ChA[ChN])){return false;}
  }
  return true;
}

bool TNmObjBs::IsNumStr(const TStr& Str) const {
  if (Str.Empty()){return false;}
  char FirstCh=Str[0];
  if (!ChDef->IsNum(FirstCh)){return false;}
  return true;
}

bool TNmObjBs::IsTagStr(const TStr& Str) const {
  return (!Str.Empty())&&(Str[0]=='<');
}

bool TNmObjBs::IsMatchPfx(
 const TStr& Str1, const TStr& Str2,
 const int& MnPfxLen, const int& MxSfxLen) const {
  // copy strings to character-array
  TChA ChA1=Str1; TChA ChA2=Str2;
  // match prefix of strings
  int ChN=0;
  forever {
    if (ChN>=ChA1.Len()){break;}
    if (ChN>=ChA2.Len()){break;}
    if (ChA1[ChN]!=ChA2[ChN]){break;}
    ChN++;
  }
  if (ChN+1<=MnPfxLen){return false;}
  //
  if (ChA1.Len()-ChN+1>MxSfxLen){return false;}
  if (ChA2.Len()-ChN+1>MxSfxLen){return false;}
  //printf("[%s]=[%s]\n", ChA1.CStr(), ChA2.CStr());
  return true;
}

int TNmObjBs::GetNmObjId(const TStrV& WordStrV, const bool& DefP){
  int NmObjId=-1;
  if (DefP){
    NmObjId=NmObjWordStrVToDocIdVH.AddKey(WordStrV);
  } else {
    NmObjId=NmObjWordStrVToDocIdVH.GetKeyId(WordStrV);
  }
  return NmObjId;
}

TStr TNmObjBs::GetWordStrVStr(const TStrV& WordStrV, const char& SepCh) const {
  TChA WordChA;
  for (int WordStrN=0; WordStrN<WordStrV.Len(); WordStrN++){
    if (WordStrN>0){WordChA+=SepCh;}
    WordChA+=WordStrV[WordStrN];
  }
  return WordChA;
}

void TNmObjBs::GetNrNmObjStrV(const TStrV& NmObjStrV, TStrV& NrNmObjStrV) const {
  int NmObjWordStrVToNrP=NmObjWordStrVToNrH.GetKeyId(NmObjStrV);
  if (NmObjWordStrVToNrP==-1){NrNmObjStrV=NmObjStrV;}
  else {NrNmObjStrV=NmObjWordStrVToNrH[NmObjWordStrVToNrP];}
}

void TNmObjBs::ExtrCandWordStrV(
 const TStr& HtmlStr, TStrV& CandWordStrV, const bool& DumpP){
  // prepare named-object vector
  CandWordStrV.Clr();
  // prepare html parsing
  PSIn HtmlSIn=TStrIn::New(HtmlStr);
  PHtmlDoc HtmlDoc=THtmlDoc::New(HtmlSIn, hdtAll, false);
  PHtmlTok Tok;
  THtmlLxSym Sym; TStr Str; TStr NrStr;
  CandWordStrV.Add(PeriodTagStr);
  bool InTitle=false; bool InScript=false; int LastNmObjTokN=-1;
  // traverse html tokens
  if (DumpP){printf("Tokens: ");}
  for (int TokN=0; TokN<HtmlDoc->GetToks(); TokN++){
    PHtmlTok Tok=HtmlDoc->GetTok(TokN);
    HtmlDoc->GetTok(TokN, Sym, Str);
    switch (Sym){
      case hsyUndef: break;
      case hsyStr:
      case hsyNum:
        if (InTitle){break;}
        if (InScript){break;}
        NrStr=GetNrWordStr(Str);
        if (DumpP){
          if (Str==NrStr){printf("%s ", Str.CStr());}
          else {printf("%s(%s) ", Str.CStr(), NrStr.CStr());}
        }
        if (IsFirstCapWordStr(NrStr)||IsNmObjAttr(NrStr, noaAsCapitalized)){
          if ((LastNmObjTokN!=-1)&&(LastNmObjTokN<TokN-1)){
            if (CandWordStrV.Last()!=PeriodTagStr){
              CandWordStrV.Add(BreakTagStr);
            }
          }
          CandWordStrV.Add(NrStr); LastNmObjTokN=TokN;
        }
        break;
      case hsySSym:
        if (InTitle){break;}
        if (InScript){break;}
        if (DumpP){
          printf("%s ", Str.CStr());}
        if (
         (Str==".")||(Str=="!")||(Str=="?")||
         (Str=="\"")||(Str=="-")||(Str=="/")||
         (Str==":")||(Str==";")){
          if (CandWordStrV.Last()!=PeriodTagStr){
            CandWordStrV.Add(PeriodTagStr);
          }
        }
        break;
      case hsyBTag:
      case hsyETag:
        if (Str=="<TITLE>"){
          InTitle=(Sym==hsyBTag);
        } else
        if (Str=="<SCRIPT>"){
          InScript=(Sym==hsyBTag);
        } else
        if (Str=="<P>"){
          if ((!CandWordStrV.Empty())&&(CandWordStrV.Last()!=ParagraphTagStr)){
            CandWordStrV.Add(ParagraphTagStr);
            CandWordStrV.Add(PeriodTagStr);
          }
        } else
        if ((Str=="<TD>")||(Str=="<BR>")){
          CandWordStrV.Add(PeriodTagStr);
        }
        break;
      case hsyEof: break;
      default: break;
    }
  }
  CandWordStrV.Add(EofTagStr);
  if (DumpP){printf("\n");}
  if (DumpP){
    printf("Candidates: ");
    for (int CandWordStrN=0; CandWordStrN<CandWordStrV.Len(); CandWordStrN++){
      printf("%s ", CandWordStrV[CandWordStrN].CStr());}
    printf("\n");
  }
}

void TNmObjBs::FilterCandToNmObjIdV(
 const TStrV& CandWordStrV, TIntV& NmObjIdV, const bool& DumpP){
  // prepare candidate traversal
  TVec<TStrV> NmObjIdWordStrVV;
  int CandWordStrN=0; int CandWordStrs=CandWordStrV.Len();
  while (CandWordStrN<CandWordStrs){
    // get candidate
    TStr WordStr=CandWordStrV[CandWordStrN];
    //printf("%s ", WordStr.CStr());
    // simple filters
    if (WordStr.Len()<=1){CandWordStrN++; continue;}
    if (WordStr==ParagraphTagStr){CandWordStrN++; continue;}
    if (WordStr==BreakTagStr){CandWordStrN++; continue;}
    if (WordStr==EofTagStr){CandWordStrN++; break;}
    if (IsNumStr(WordStr)){CandWordStrN++; continue;}
    TStr UcWordStr=ChDef->GetUcStr(WordStr);
    //if (SwSet->IsIn(UcWordStr, true)){
    //  CandWordStrN++; continue;}
    if ((WordStr==UcWordStr)&&((WordStr.Len()>4)&&(!IsNmObjAttr(WordStr, noaAcronym)))){
      CandWordStrN++; continue;}
    // unperiod
    if (IsNmObjAttr(WordStr, noaUnperiod)&&(CandWordStrV[CandWordStrN+1]==PeriodTagStr)){
      CandWordStrN+=1;
    }
    // period
    if (WordStr==PeriodTagStr){
      CandWordStrN++; WordStr=CandWordStrV[CandWordStrN];
      if (IsTagStr(WordStr)){continue;}
      if (IsNmObjAttr(WordStr, noaDefined)){
        continue;
      } else 
      if ((CandWordStrN>1)&&(IsNmObjAttr(CandWordStrV[CandWordStrN-2], noaUnperiod))){
        continue;
      } else {
        TStr NextWordStr=CandWordStrV[CandWordStrN+1];
        if (IsFirstCapWordStr(NextWordStr)||IsNmObjAttr(NextWordStr, noaAsCapitalized)){
          continue;
        } else 
        if (!IsNmObj(WordStr)){
          CandWordStrN++; continue;
        }
      }
    }
//    if (WordStr=="British"){
//      printf("");}
    // ignore
    if (IsNmObjAttr(WordStr, noaIgnore)){
      CandWordStrN++; continue;
    } 
    // collect named-object words
    TStrV WordStrV;
    forever {
      WordStrV.Add(WordStr);
      CandWordStrN++; WordStr=CandWordStrV[CandWordStrN];
      if (IsTagStr(WordStr)){break;}
      if (WordStr.Len()<=1){break;}
      if (IsNmObjAttr(WordStr, noaIgnore)){CandWordStrN++; break;}
      if (IsNmObjAttr(WordStr, noaStandalone)){break;}
      if (IsNmObjAttr(WordStrV, noaStandalone)){break;}
    }
    // get normalized version of named-object
    TStrV NrWordStrV; GetNrNmObjStrV(WordStrV, NrWordStrV);
    // simple filters
    if (IsNmObjAttr(NrWordStrV, noaIgnore)){continue;}
    if (IsNmObjAttr(NrWordStrV, noaFirstName)){continue;}
    if (NrWordStrV.Len()>5){
      while (NrWordStrV.Len()>2){NrWordStrV.Del(0);}}
    if (NrWordStrV.Len()==1){
      TStr UcWordStr=ChDef->GetUcStr(NrWordStrV[0]);
      if (SwSet->IsIn(UcWordStr, true)){continue;}
    }
    // add named object
    NmObjIdWordStrVV.Add(NrWordStrV);
  }
  // merge similar words
  for (int NmObjN=0; NmObjN<NmObjIdWordStrVV.Len(); NmObjN++){
    TStrV& WordStrV=NmObjIdWordStrVV[NmObjN];
    if (WordStrV.Len()==1){
      // merge single words
      for (int SubNmObjN=0; SubNmObjN<NmObjIdWordStrVV.Len(); SubNmObjN++){
        TStrV& SubWordStrV=NmObjIdWordStrVV[SubNmObjN];
        if (SubWordStrV.Len()==1){
          if (WordStrV[0]!=SubWordStrV[0]){
            if (IsMatchPfx(WordStrV[0], SubWordStrV[0], 3, 4)){
              // normalize to shorter string
              if (WordStrV[0].Len()<SubWordStrV[0].Len()){SubWordStrV=WordStrV;}
              else {WordStrV=SubWordStrV;}
            }
          }
        }
      }
    } else
    if (WordStrV.Len()>=2){
      TStr LastNm=WordStrV.Last();
      for (int SubNmObjN=0; SubNmObjN<NmObjIdWordStrVV.Len(); SubNmObjN++){
        TStrV& SubWordStrV=NmObjIdWordStrVV[SubNmObjN];
        if (SubWordStrV.Len()==1){
          // merge last-name with [first-name,last-name] pairs
          TStr SubLastNm=SubWordStrV[0];
          if (LastNm!=SubLastNm){
            if (IsMatchPfx(LastNm, SubLastNm, 3, 4)){
              if (LastNm.Len()<SubLastNm.Len()){SubWordStrV=WordStrV;} 
              else {WordStrV=SubWordStrV;}
            }
          }
        } else
        if (false&&(SubWordStrV.Len()==2)){
          // merge [first-name,last-name] with [first-name,last-name] pairs
          if ((WordStrV[0]!=SubWordStrV[0])||(WordStrV[1]!=SubWordStrV[1])){
            if ((IsMatchPfx(WordStrV[0], SubWordStrV[0], 3, 4))&&
             (IsMatchPfx(WordStrV[1], SubWordStrV[1], 3, 4))){
              // normalize to shorter string (first word)
              if (WordStrV[0].Len()<SubWordStrV[0].Len()){
                SubWordStrV[0]=WordStrV[0];}
              else {WordStrV[0]=SubWordStrV[0];}
              // normalize to shorter string (second word)
              if (WordStrV[1].Len()<SubWordStrV[1].Len()){
                SubWordStrV[1]=WordStrV[1];}
              else {WordStrV[1]=SubWordStrV[1];}
            }
          }
        }
      }
    }
  }
  // get named-objects-ids
  NmObjIdV.Gen(NmObjIdWordStrVV.Len(), 0);
  {for (int NmObjN=0; NmObjN<NmObjIdWordStrVV.Len(); NmObjN++){
    TStrV& NmObjWordStrV=NmObjIdWordStrVV[NmObjN];
    int NmObjId=GetNmObjId(NmObjWordStrV, true);
    NmObjIdV.Add(NmObjId);
  }}
  // dump
  if (DumpP){
    printf("Named-Objects: ");
    for (int NmObjN=0; NmObjN<NmObjIdV.Len(); NmObjN++){
      int NmObjId=NmObjIdV[NmObjN];
      TStr NmObjStr=GetNmObjStr(NmObjId);
      printf("%s ", NmObjStr.CStr());
    }
    printf("\n");
  }
}

PNmObjBs TNmObjBs::GetFromStrQuV(
 const TStrQuV& IdTitleSrcHtmlQuV,
 const TSwSetType& SwSetType, const TStr& CustSwSetFNm,
 const TStr& NrWordBsFNm, const TStr& WordTypeBsFNm, const TStr& MteFNm,
 const int& MxDocs, const bool& DumpP){
  // custom-stop-words
  PSIn CustSwSetSIn;
  if (!CustSwSetFNm.Empty()&&(TFile::Exists(CustSwSetFNm))){
    CustSwSetSIn=TFIn::New(CustSwSetFNm);}
  // normalized-words
  PSIn NrWordBsSIn;
  if (!NrWordBsFNm.Empty()&&(TFile::Exists(NrWordBsFNm))){
    NrWordBsSIn=TFIn::New(NrWordBsFNm);}
  // word-types
  PSIn WordTypeBsSIn;
  if (!WordTypeBsFNm.Empty()&&(TFile::Exists(WordTypeBsFNm))){
    WordTypeBsSIn=TFIn::New(WordTypeBsFNm);}
  // create named-objects-base
  PNmObjBs NmObjBs=
   TNmObjBs::New(SwSetType, CustSwSetSIn, NrWordBsSIn, WordTypeBsSIn, MteFNm);
  // traverse compact-documents
  for (int DocN=0; DocN<IdTitleSrcHtmlQuV.Len(); DocN++){
    if ((MxDocs!=-1)&&(DocN>MxDocs)){break;}
    TStr DocNm=IdTitleSrcHtmlQuV[DocN].Val1;
    TStr TitleStr=IdTitleSrcHtmlQuV[DocN].Val2;
    TStr HtmlStr=IdTitleSrcHtmlQuV[DocN].Val4;
    //if (!DocNm.IsStrIn("1998-06-18-z1b9.")){continue;}
    if (DumpP){
      printf("===============================================\n");
      printf("%s\n", DocNm.CStr());
    }
    // extract candidate named-objects
    TStrV CandWordStrV; NmObjBs->ExtrCandWordStrV(HtmlStr, CandWordStrV, DumpP);
    // extract final named-objects from candidates
    TIntV NmObjIdV; NmObjBs->FilterCandToNmObjIdV(CandWordStrV, NmObjIdV, DumpP);
    // create document and add named-objects
    NmObjBs->AddDoc(DocNm, "", NmObjIdV);
  }
  // get merged named-objects
  TIntV NewNmObjIdV; NmObjBs->GetMergedNmObj(NewNmObjIdV);
  // apply merged named-objects
  NmObjBs->PutMergedNmObj(NewNmObjIdV);
  // return named-objects-base
  return NmObjBs;
}

PNmObjBs TNmObjBs::GetFromCpd(
 const PSIn& _CpdSIn,
 const TSwSetType& SwSetType, const PSIn& CustSwSetSIn,
 const PSIn& NrWordBsSIn, const PSIn& WordTypeBsSIn, const TStr& MteFNm,
 const int& MxDocs, const bool& DumpP){
  // create named-objects-base
  PNmObjBs NmObjBs=
   TNmObjBs::New(SwSetType, CustSwSetSIn, NrWordBsSIn, WordTypeBsSIn, MteFNm);
  // traverse compact-documents
  PSIn CpdSIn=TCpDoc::FFirstCpd(_CpdSIn); PCpDoc CpDoc; int Docs=0;
  while (TCpDoc::FNextCpd(CpdSIn, CpDoc)){
    if ((MxDocs!=-1)&&(Docs>MxDocs)){break;}
    TStr DocNm=CpDoc->GetDocNm();
    TStr DateStr=CpDoc->GetDateStr();
    TStr HtmlStr=CpDoc->GetTxtStr();
    Docs++; printf("%d %s\r", Docs, DocNm.CStr());
    //if (!DocNm.IsStrIn("1998-06-18-z1b9.")){continue;}
    if (DumpP){
      printf("===============================================\n");
      printf("%s\n", DocNm.CStr());
    }
    // extract candidate named-objects
    TStrV CandWordStrV; NmObjBs->ExtrCandWordStrV(HtmlStr, CandWordStrV, DumpP);
    // extract final named-objects from candidates
    TIntV NmObjIdV; NmObjBs->FilterCandToNmObjIdV(CandWordStrV, NmObjIdV, DumpP);
    // create document and add named-objects
    NmObjBs->AddDoc(DocNm, DateStr, NmObjIdV);
  }
  // get merged named-objects
  TIntV NewNmObjIdV; NmObjBs->GetMergedNmObj(NewNmObjIdV);
  // apply merged named-objects
  NmObjBs->PutMergedNmObj(NewNmObjIdV);
  // return named-objects-base
  return NmObjBs;
}

PNmObjBs TNmObjBs::GetFromCpd(
 const TStr& CpdFNm,
 const TSwSetType& SwSetType, const TStr& CustSwSetFNm,
 const TStr& NrWordBsFNm, const TStr& WordTypeBsFNm, const TStr& MteFNm,
 const int& MxDocs, const bool& DumpP){
  // cpd file
  PSIn CpdSIn=TFIn::New(CpdFNm);
  // custom-stop-words
  PSIn CustSwSetSIn;
  if (!CustSwSetFNm.Empty()&&(TFile::Exists(CustSwSetFNm))){
    CustSwSetSIn=TFIn::New(CustSwSetFNm);}
  // normalized-words
  PSIn NrWordBsSIn;
  if (!NrWordBsFNm.Empty()&&(TFile::Exists(NrWordBsFNm))){
    NrWordBsSIn=TFIn::New(NrWordBsFNm);}
  // word-types
  PSIn WordTypeBsSIn;
  if (!WordTypeBsFNm.Empty()&&(TFile::Exists(WordTypeBsFNm))){
    WordTypeBsSIn=TFIn::New(WordTypeBsFNm);}

  return TNmObjBs::GetFromCpd(
   CpdSIn, SwSetType, CustSwSetSIn, NrWordBsSIn, WordTypeBsSIn, MteFNm,
   MxDocs, DumpP);
}


void TNmObjBs::GetMergedNmObj(TIntV& NewNmObjIdV){
  // matching constraints
  int MnPfxLen=3; int MxSfxLen=2;

  // create transformation vector
  int NmObjs=NmObjWordStrVToDocIdVH.Len();
  NewNmObjIdV.Gen(NmObjs); NewNmObjIdV.PutAll(-1);

  // merging single words
  // merging statistics
  {int SingleWords=0; int ReducedSingleWords=0;
  // collect single words according to prefix
  TStrIntVH PfxStrToNmObjIdVH;
  for (int NmObjId=0; NmObjId<NmObjs; NmObjId++){
    if (NewNmObjIdV[NmObjId]!=-1){continue;}
    const TStrV& WordStrV=NmObjWordStrVToDocIdVH.GetKey(NmObjId);
    if (WordStrV.Len()==1){
      TStr PfxStr=WordStrV[0].GetSubStr(0, 2);
      PfxStrToNmObjIdVH.AddDat(PfxStr).Add(NmObjId);
      SingleWords++;
    }
  }
  // traverse word-groups with the same prefix
  int Pfxs=PfxStrToNmObjIdVH.Len();
  for (int PfxId=0; PfxId<Pfxs; PfxId++){
    // get & traverse word-group
    TIntV& NmObjIdV=PfxStrToNmObjIdVH[PfxId];
    for (int NmObjIdN=0; NmObjIdN<NmObjIdV.Len(); NmObjIdN++){
      int NmObjId=NmObjIdV[NmObjIdN];
      if (NewNmObjIdV[NmObjId]!=-1){continue;}
      NewNmObjIdV[NmObjId]=NmObjId;
      const TStr& WordStr=NmObjWordStrVToDocIdVH.GetKey(NmObjId)[0];
      int Fq=NmObjWordStrVToDocIdVH[NmObjId].Len();
      TIntPrV FqNmObjIdPrV(NmObjIdV.Len(), 0);
      FqNmObjIdPrV.Add(TIntPr(Fq, NmObjId));
      // traverse rest of the word-group for matching words
      for (int SubNmObjIdN=NmObjIdN+1; SubNmObjIdN<NmObjIdV.Len(); SubNmObjIdN++){
        int SubNmObjId=NmObjIdV[SubNmObjIdN];
        if (NewNmObjIdV[SubNmObjId]!=-1){continue;}
        const TStr& SubWordStr=NmObjWordStrVToDocIdVH.GetKey(SubNmObjId)[0];
        // test matching
        if (IsMatchPfx(WordStr, SubWordStr, MnPfxLen, MxSfxLen)){
          NewNmObjIdV[SubNmObjId]=NmObjId;
          int SubFq=NmObjWordStrVToDocIdVH[SubNmObjId].Len();
          FqNmObjIdPrV.Add(TIntPr(SubFq, SubNmObjId));
          //printf("%s -> %s\n", WordStr.CStr(), SubWordStr.CStr());
        }
      }
      // increment number of equivalence word-groups
      ReducedSingleWords++;
      // collapse matching words into most frequent word
      if (FqNmObjIdPrV.Len()>1){
        FqNmObjIdPrV.Sort(false);
        int MainNmObjId=FqNmObjIdPrV[0].Val2;
        NewNmObjIdV[MainNmObjId]=MainNmObjId;
        TStr MainWordStr=NmObjWordStrVToDocIdVH.GetKey(MainNmObjId)[0];
        //printf("[%s:", MainWordStr.CStr());
        for (int FqNmObjIdPrN=1; FqNmObjIdPrN<FqNmObjIdPrV.Len(); FqNmObjIdPrN++){
          int SubNmObjId=FqNmObjIdPrV[FqNmObjIdPrN].Val2;
          NewNmObjIdV[SubNmObjId]=MainNmObjId;
          //TStr& SubWordStr=NmObjWordStrVToDocIdVH.GetKey(SubNmObjId)[0];
          //printf(" %s", SubWordStr.CStr());
        }
        //printf("]\n");
      }
    }
  }
  // print statistics
  //printf("SingleWords:%d ReducedSingleWords:%d\n",
  // SingleWords, ReducedSingleWords);
  }

  // merging double words
  // merging statistics
  {int DoubleWords=0; int ReducedDoubleWords=0;
  // collect double words according to prefix
  TStrIntVH PfxStrToNmObjIdVH;
  for (int NmObjId=0; NmObjId<NmObjs; NmObjId++){
    if (NewNmObjIdV[NmObjId]!=-1){continue;}
    const TStrV& WordStrV=NmObjWordStrVToDocIdVH.GetKey(NmObjId);
    if (WordStrV.Len()==2){
      TStr PfxStr=WordStrV[0].GetSubStr(0, 2)+WordStrV[1].GetSubStr(0, 2);
      PfxStrToNmObjIdVH.AddDat(PfxStr).Add(NmObjId);
      DoubleWords++;
    }
  }
  // traverse word-groups with the same prefix
  int Pfxs=PfxStrToNmObjIdVH.Len();
  for (int PfxId=0; PfxId<Pfxs; PfxId++){
    // get & traverse word-group
    TIntV& NmObjIdV=PfxStrToNmObjIdVH[PfxId];
    for (int NmObjIdN=0; NmObjIdN<NmObjIdV.Len(); NmObjIdN++){
      int NmObjId=NmObjIdV[NmObjIdN];
      if (NewNmObjIdV[NmObjId]!=-1){continue;}
      NewNmObjIdV[NmObjId]=NmObjId;
      const TStr& WordStr1=NmObjWordStrVToDocIdVH.GetKey(NmObjId)[0];
      const TStr& WordStr2=NmObjWordStrVToDocIdVH.GetKey(NmObjId)[1];
      int Fq=NmObjWordStrVToDocIdVH[NmObjId].Len();
      TIntPrV FqNmObjIdPrV(NmObjIdV.Len(), 0);
      FqNmObjIdPrV.Add(TIntPr(Fq, NmObjId));
      // traverse rest of the word-group for matching words
      for (int SubNmObjIdN=NmObjIdN+1; SubNmObjIdN<NmObjIdV.Len(); SubNmObjIdN++){
        int SubNmObjId=NmObjIdV[SubNmObjIdN];
        if (NewNmObjIdV[SubNmObjId]!=-1){continue;}
        const TStr& SubWordStr1=NmObjWordStrVToDocIdVH.GetKey(SubNmObjId)[0];
        const TStr& SubWordStr2=NmObjWordStrVToDocIdVH.GetKey(SubNmObjId)[1];
        // test matching
        if (IsMatchPfx(WordStr1, SubWordStr1, MnPfxLen, MxSfxLen+1)&&
         IsMatchPfx(WordStr2, SubWordStr2, MnPfxLen, MxSfxLen+1)){
          NewNmObjIdV[SubNmObjId]=NmObjId;
          int SubFq=NmObjWordStrVToDocIdVH[SubNmObjId].Len();
          FqNmObjIdPrV.Add(TIntPr(SubFq, SubNmObjId));
          //printf("%s_%s -> %s_%s\n",
          // WordStr1.CStr(), WordStr2.CStr(),
          // SubWordStr1.CStr(), SubWordStr2.CStr());
        }
      }
      // increment number of equivalence word-groups
      ReducedDoubleWords++;
      // collapse matching words into most frequent word
      if (FqNmObjIdPrV.Len()>1){
        FqNmObjIdPrV.Sort(false);
        int MainNmObjId=FqNmObjIdPrV[0].Val2;
        NewNmObjIdV[MainNmObjId]=MainNmObjId;
        TStr MainWordStr1=NmObjWordStrVToDocIdVH.GetKey(MainNmObjId)[0];
        TStr MainWordStr2=NmObjWordStrVToDocIdVH.GetKey(MainNmObjId)[1];
        //printf("[%s_%s:", MainWordStr1.CStr(), MainWordStr2.CStr());
        for (int FqNmObjIdPrN=1; FqNmObjIdPrN<FqNmObjIdPrV.Len(); FqNmObjIdPrN++){
          int SubNmObjId=FqNmObjIdPrV[FqNmObjIdPrN].Val2;
          NewNmObjIdV[SubNmObjId]=MainNmObjId;
          //TStr& SubWordStr1=NmObjWordStrVToDocIdVH.GetKey(SubNmObjId)[0];
          //TStr& SubWordStr2=NmObjWordStrVToDocIdVH.GetKey(SubNmObjId)[1];
          //printf(" %s_%s", SubWordStr1.CStr(), SubWordStr2.CStr());
        }
        //printf("]\n");
      }
    }
  }
  // print statistics
  //printf("DoubleWords:%d ReducedDoubleWords:%d\n",
  // DoubleWords, ReducedDoubleWords);
  }

  // merging triples to doubles
  // ... (prefix, first-name, last-name) to (first-name, last-name)
  // merging statistics
  {int TripleWords=0; int ReducedTripleWords=0;
  // collect single words according to prefix
  TStrIntVH PfxStrToNmObjIdVH;
  for (int NmObjId=0; NmObjId<NmObjs; NmObjId++){
    if (NewNmObjIdV[NmObjId]!=-1){continue;}
    const TStrV& WordStrV=NmObjWordStrVToDocIdVH.GetKey(NmObjId);
    if (WordStrV.Len()==3){
      TripleWords++;
      TStrV DbWordStrV(2, 0);
      DbWordStrV.Add(WordStrV[1]); DbWordStrV.Add(WordStrV[2]);
      int DbNmObjId=NmObjWordStrVToDocIdVH.GetKeyId(DbWordStrV);
      if (DbNmObjId!=-1){
        ReducedTripleWords++;
        int NewDbNmObjId=NewNmObjIdV[DbNmObjId];
        NewNmObjIdV[NmObjId]=NewDbNmObjId;
        //TStr NmObjStr=GetNmObjStr(NmObjId);
        //TStr DbNmObjStr=GetNmObjStr(DbNmObjId);
        //TStr NewDbNmObjStr=GetNmObjStr(NewDbNmObjId);
        //printf("%s -> %s -> %s\n",
        // NmObjStr.CStr(), DbNmObjStr.CStr(), NewDbNmObjStr.CStr());
      }
    }
  }
  //printf("TripleWords:%d ReducedTripleWords:%d\n",
  // TripleWords, ReducedTripleWords);
  }

  // merging triple words
  // merging statistics
  {int TripleWords=0; int ReducedTripleWords=0;
  // collect triple words according to prefix
  TStrIntVH PfxStrToNmObjIdVH;
  for (int NmObjId=0; NmObjId<NmObjs; NmObjId++){
    if (NewNmObjIdV[NmObjId]!=-1){continue;}
    const TStrV& WordStrV=NmObjWordStrVToDocIdVH.GetKey(NmObjId);
    if (WordStrV.Len()==3){
      TStr PfxStr=WordStrV[0].GetSubStr(0, 2)+WordStrV[1].GetSubStr(0, 2)+WordStrV[2].GetSubStr(0, 2);
      PfxStrToNmObjIdVH.AddDat(PfxStr).Add(NmObjId);
      TripleWords++;
    }
  }
  // traverse word-groups with the same prefix
  int Pfxs=PfxStrToNmObjIdVH.Len();
  for (int PfxId=0; PfxId<Pfxs; PfxId++){
    // get & traverse word-group
    TIntV& NmObjIdV=PfxStrToNmObjIdVH[PfxId];
    for (int NmObjIdN=0; NmObjIdN<NmObjIdV.Len(); NmObjIdN++){
      int NmObjId=NmObjIdV[NmObjIdN];
      if (NewNmObjIdV[NmObjId]!=-1){continue;}
      NewNmObjIdV[NmObjId]=NmObjId;
      const TStr& WordStr1=NmObjWordStrVToDocIdVH.GetKey(NmObjId)[0];
      const TStr& WordStr2=NmObjWordStrVToDocIdVH.GetKey(NmObjId)[1];
      const TStr& WordStr3=NmObjWordStrVToDocIdVH.GetKey(NmObjId)[2];
      int Fq=NmObjWordStrVToDocIdVH[NmObjId].Len();
      TIntPrV FqNmObjIdPrV(NmObjIdV.Len(), 0);
      FqNmObjIdPrV.Add(TIntPr(Fq, NmObjId));
      // traverse rest of the word-group for matching words
      for (int SubNmObjIdN=NmObjIdN+1; SubNmObjIdN<NmObjIdV.Len(); SubNmObjIdN++){
        int SubNmObjId=NmObjIdV[SubNmObjIdN];
        if (NewNmObjIdV[SubNmObjId]!=-1){continue;}
        const TStr& SubWordStr1=NmObjWordStrVToDocIdVH.GetKey(SubNmObjId)[0];
        const TStr& SubWordStr2=NmObjWordStrVToDocIdVH.GetKey(SubNmObjId)[1];
        const TStr& SubWordStr3=NmObjWordStrVToDocIdVH.GetKey(SubNmObjId)[2];
        // test matching
        if (IsMatchPfx(WordStr1, SubWordStr1, MnPfxLen, MxSfxLen+1)&&
         IsMatchPfx(WordStr2, SubWordStr2, MnPfxLen, MxSfxLen+1)&&
         IsMatchPfx(WordStr3, SubWordStr3, MnPfxLen, MxSfxLen+1)){
          NewNmObjIdV[SubNmObjId]=NmObjId;
          int SubFq=NmObjWordStrVToDocIdVH[SubNmObjId].Len();
          FqNmObjIdPrV.Add(TIntPr(SubFq, SubNmObjId));
          //printf("%s_%s_%s -> %s_%s_%s\n",
          // WordStr1.CStr(), WordStr2.CStr(), WordStr3.CStr(),
          // SubWordStr1.CStr(), SubWordStr2.CStr(), SubWordStr3.CStr());
        }
      }
      // increment number of equivalence word-groups
      ReducedTripleWords++;
      // collapse matching words into most frequent word
      if (FqNmObjIdPrV.Len()>1){
        FqNmObjIdPrV.Sort(false);
        int MainNmObjId=FqNmObjIdPrV[0].Val2;
        NewNmObjIdV[MainNmObjId]=MainNmObjId;
        TStr MainWordStr1=NmObjWordStrVToDocIdVH.GetKey(MainNmObjId)[0];
        TStr MainWordStr2=NmObjWordStrVToDocIdVH.GetKey(MainNmObjId)[1];
        TStr MainWordStr3=NmObjWordStrVToDocIdVH.GetKey(MainNmObjId)[2];
        //printf("[%s_%s_%s:", MainWordStr1.CStr(), MainWordStr2.CStr(), MainWordStr3.CStr());
        for (int FqNmObjIdPrN=1; FqNmObjIdPrN<FqNmObjIdPrV.Len(); FqNmObjIdPrN++){
          int SubNmObjId=FqNmObjIdPrV[FqNmObjIdPrN].Val2;
          NewNmObjIdV[SubNmObjId]=MainNmObjId;
          //TStr& SubWordStr1=NmObjWordStrVToDocIdVH.GetKey(SubNmObjId)[0];
          //TStr& SubWordStr2=NmObjWordStrVToDocIdVH.GetKey(SubNmObjId)[1];
          //TStr& SubWordStr3=NmObjWordStrVToDocIdVH.GetKey(SubNmObjId)[2];
          //printf(" %s_%s_%s", SubWordStr1.CStr(), SubWordStr2.CStr(), SubWordStr3.CStr());
        }
        //printf("]\n");
      }
    }
  }
  // print statistics
  //printf("TripleWords:%d ReducedTripleWords:%d\n",
  // TripleWords, ReducedTripleWords);
  }
}

void TNmObjBs::PutMergedNmObj(const TIntV& NewNmObjIdV){
  // create temporary table of new named-objects
  TStrVIntVH NewNmObjWordStrVToDocIdVH;
  for (int NmObjId=0; NmObjId<NewNmObjIdV.Len(); NmObjId++){
    if (NewNmObjIdV[NmObjId]!=NmObjId){continue;}
    // take data for new named-object from old definition
    const TStrV& WordStrV=NmObjWordStrVToDocIdVH.GetKey(NmObjId);
    // define new named-object
    NewNmObjWordStrVToDocIdVH.AddDat(WordStrV);
  }
  //printf("Old Named-Objects: %d\n", NmObjWordStrVToDocIdVH.Len());
  //printf("New Named-Objects: %d\n", NewNmObjWordStrVToDocIdVH.Len());
  // obsolete named-object define as aliases
  {for (int NmObjId=0; NmObjId<NewNmObjIdV.Len(); NmObjId++){
    if (NewNmObjIdV[NmObjId]==NmObjId){continue;}
    // take data for obsolete named-object from old definition
    const TStrV& WordStrV=NmObjWordStrVToDocIdVH.GetKey(NmObjId);
    // define alias for obsolete named-object
    int NrNmObjId=NewNmObjIdV[NmObjId];
    if (NrNmObjId!=-1){
      const TStrV& NrWordStrV=NmObjWordStrVToDocIdVH.GetKey(NrNmObjId);
      NmObjWordStrVToNrH.AddDat(WordStrV, NrWordStrV);
    }
  }}
  // redefine documents
  int Docs=GetDocs();
  for (int DocId=0; DocId<Docs; DocId++){
    TIntPrV& NmObjIdFqPrV=GetDoc_NmObjIdFqPrV(DocId);
    // create temporary-document: new-named-object to frequency table
    TIntIntH NewNmObjIdToFqH(NmObjIdFqPrV.Len());
    for (int NmObjN=0; NmObjN<NmObjIdFqPrV.Len(); NmObjN++){
      // get obsolete named-object data
      int NmObjId=NmObjIdFqPrV[NmObjN].Val1;
      int Fq=NmObjIdFqPrV[NmObjN].Val2;
      // get named-document-id for normalized named-object
      int NrNmObjId=NewNmObjIdV[NmObjId];
      if (NrNmObjId!=-1){
        // get normalized version of word-vector
        const TStrV& NrWordStrV=NmObjWordStrVToDocIdVH.GetKey(NrNmObjId);
        // get new named-object-id
        int NewNmObjId=NewNmObjWordStrVToDocIdVH.GetKeyId(NrWordStrV);
        // add new named-object-id and term-frequency to temporary-document
        NewNmObjIdToFqH.AddDat(NewNmObjId)+=Fq;
      }
    }
    // transfere new-named-object data to document
    NmObjIdFqPrV.Gen(NewNmObjIdToFqH.Len(), 0);
    for (int NmObjP=0; NmObjP<NewNmObjIdToFqH.Len(); NmObjP++){
      int NewNmObjId=NewNmObjIdToFqH.GetKey(NmObjP);
      int Fq=NewNmObjIdToFqH[NmObjP];
      // add named-object and increment by term-frequency
      NmObjIdFqPrV.Add(TIntPr(NewNmObjId, Fq));
      // merge document-ids
      NewNmObjWordStrVToDocIdVH[NewNmObjId].Add(DocId);
    }
    NmObjIdFqPrV.Sort();
  }
  // assign new named-objects
  NmObjWordStrVToDocIdVH=NewNmObjWordStrVToDocIdVH;
}

void TNmObjBs::GetNmObjStrFqPrV(TStrIntPrV& NmObjStrFqPrV, const int& MnFq) const {
  int NmObjs=GetNmObjs();
  NmObjStrFqPrV.Gen(NmObjs, 0);
  for (int NmObjId=0; NmObjId<NmObjs; NmObjId++){
    TStrIntPr NmObjStrFqPr;
    NmObjStrFqPr.Val1=GetNmObjStr(NmObjId);
    NmObjStrFqPr.Val2=GetNmObjDocs(NmObjId);
    if (NmObjStrFqPr.Val2>=MnFq){
      NmObjStrFqPrV.Add(NmObjStrFqPr);}
  }
}

void TNmObjBs::GetNmObjFqStrPrV(TIntStrPrV& NmObjFqStrPrV, const int& MnFq) const {
  int NmObjs=GetNmObjs();
  NmObjFqStrPrV.Gen(NmObjs, 0);
  for (int NmObjId=0; NmObjId<NmObjs; NmObjId++){
    TIntStrPr NmObjFqStrPr;
    NmObjFqStrPr.Val1=GetNmObjDocs(NmObjId);
    NmObjFqStrPr.Val2=GetNmObjStr(NmObjId);
    if (NmObjFqStrPr.Val1>=MnFq){
      NmObjFqStrPrV.Add(NmObjFqStrPr);}
  }
}

void TNmObjBs::GetNmObjDIdV(
 const PBowDocBs& BowDocBs, TIntV& BowDIdV, 
 const TStr& NmObjStr1, const TStr& NmObjStr2) const {
  // get first named-object-id
  int NmObjId1=GetNmObjId(NmObjStr1);
  TIntV NmObjDocIdV1; GetNmObjDocIdV(NmObjId1, NmObjDocIdV1);
  NmObjDocIdV1.Sort();
  // get second named-object-id
  TIntV NmObjDocIdV2;
  if (!NmObjStr2.Empty()){
    int NmObjId2=GetNmObjId(NmObjStr2);
    GetNmObjDocIdV(NmObjId2, NmObjDocIdV2);
    NmObjDocIdV2.Sort();
  }
  // create joint doc-id-vector
  TIntV NmObjDocIdV;
  if (NmObjDocIdV2.Empty()){
    NmObjDocIdV=NmObjDocIdV1;
  } else {
    NmObjDocIdV1.Intrs(NmObjDocIdV2, NmObjDocIdV);
  }
  // traverse named-object-documents to collect bow-document-ids
  BowDIdV.Gen(NmObjDocIdV.Len(), 0);
  for (int NmObjDocIdN=0; NmObjDocIdN<NmObjDocIdV.Len(); NmObjDocIdN++){
    TStr DocNm=GetDocNm(NmObjDocIdV[NmObjDocIdN]);
    int DId=BowDocBs->GetDId(DocNm);
    if (DId!=-1){
      BowDIdV.Add(DId);
    } 
  }
}

PBowSpV TNmObjBs::GetNmObjConcept(
 const PBowDocBs& BowDocBs, const PBowDocWgtBs& BowDocWgtBs,
 const TStr& NmObjStr1, const TStr& NmObjStr2) const {
  // get doc-ids
  TIntV BowDIdV; GetNmObjDIdV(BowDocBs, BowDIdV, NmObjStr1, NmObjStr2);
  // get concept vector
  PBowSpV ConceptSpV=TBowClust::GetConceptSpV(BowDocWgtBs, NULL, BowDIdV);
  // return concept vector
  return ConceptSpV;
}

void TNmObjBs::GetFqNmObjIdPrV(
 const TStr& TargetNmObjStr, TIntPrV& FqNmObjIdPrV) const {
  //printf("Searching %s ...", TargetNmObjStr.CStr());
  // get target named-object-id
  int TargetNmObjId=GetNmObjId(TargetNmObjStr);
  // collect target named-object frequencies
  TIntIntH NmObjIdToFqH;
  // traverse target named-object documents
  int NmObjDocs=GetNmObjDocs(TargetNmObjId);
  for (int NmObjDocIdN=0; NmObjDocIdN<NmObjDocs; NmObjDocIdN++){
    // get document-id
    int DocId=GetNmObjDocId(TargetNmObjId, NmObjDocIdN);
    // traverse named-object in document
    int DocNmObjs=GetDocNmObjs(DocId);
    for (int DocNmObjN=0; DocNmObjN<DocNmObjs; DocNmObjN++){
      // get named-object & frequency
      int NmObjId; int TermFq;
      GetDocNmObjId(DocId, DocNmObjN, NmObjId, TermFq);
      // increment named-object document frequency
      NmObjIdToFqH.AddDat(NmObjId)++;
    }
  }
  // get & sort frequency table
  FqNmObjIdPrV.Clr(); NmObjIdToFqH.GetDatKeyPrV(FqNmObjIdPrV);
  FqNmObjIdPrV.Sort(false);
}

int TNmObjBs::AddDoc(const TStr& DocNm, const TStr& DateStr, const TIntV& NmObjIdV){
  // create named-object-id to frequency table
  TIntIntH NmObjIdToFqH(NmObjIdV.Len());
  for (int NmObjN=0; NmObjN<NmObjIdV.Len(); NmObjN++){
    int NmObjId=NmObjIdV[NmObjN];
    NmObjIdToFqH.AddDat(NmObjId)++;
  }
  // create document
  IAssert(!IsDoc(DocNm));
  int DocId=DocNmToNmObjDocH.AddKey(DocNm);
  DocNmToNmObjDocH[DocId]=TNmObjDoc::New();
  DocNmToNmObjDocH[DocId]->DateStr=DateStr;
  TIntPrV& NmObjIdFqPrV=GetDoc_NmObjIdFqPrV(DocId);
  for (int NmObjP=0; NmObjP<NmObjIdToFqH.Len(); NmObjP++){
    int NmObjId=NmObjIdToFqH.GetKey(NmObjP);
    int Fq=NmObjIdToFqH[NmObjP];
    NmObjIdFqPrV.Add(TIntPr(NmObjId, Fq));
    NmObjWordStrVToDocIdVH[NmObjId].AddSorted(DocId);
  }
  NmObjIdFqPrV.Sort();
  // return doc-id
  return DocId;
}

TStr TNmObjBs::GetDoc_NmObjStrVStr(const int& DocId, const char& SepCh) const {
  TChA NmObjStrVChA;
  TIntPrV& NmObjIdFqPrV=GetDoc_NmObjIdFqPrV(DocId);
  for (int NmObjIdN=0; NmObjIdN<NmObjIdFqPrV.Len(); NmObjIdN++){
    if (NmObjIdN>0){NmObjStrVChA+=SepCh;}
    int NmObjId=NmObjIdFqPrV[NmObjIdN].Val1;
    TStr NmObjStr=GetNmObjStr(NmObjId);
    NmObjStrVChA+=NmObjStr;
  }
  return NmObjStrVChA;
}

PBowDocBs TNmObjBs::GetBowDocBs(const int& MnNmObjFq) const {
  printf("Generating Bag-Of-Words...\n");
  // create bag-of-words
  PBowDocBs BowDocBs=TBowDocBs::New();
  // traverse documents
  for (int DocId=0; DocId<GetDocs(); DocId++){
    if (DocId%100==0){printf("%d\r", DocId);}
    TStr DocNm=GetDocNm(DocId);
    TStr DateStr=GetDocDateStr(DocId);
    TStrV WordStrV;
    int DocNmObjs=GetDocNmObjs(DocId);
    for (int DocNmObjN=0; DocNmObjN<DocNmObjs; DocNmObjN++){
      int NmObjId; int TermFq; GetDocNmObjId(DocId, DocNmObjN, NmObjId, TermFq);
      if ((MnNmObjFq==-1)||(GetNmObjDocs(NmObjId)>=MnNmObjFq)){
        TStr NmObjStr=GetNmObjStr(NmObjId);
        for (int TermOccN=0; TermOccN<TermFq; TermOccN++){
          WordStrV.Add(NmObjStr);
        }
      }
    }
    if (!WordStrV.Empty()){
      int DId=BowDocBs->AddDoc(DocNm, TStrV(), WordStrV);
      BowDocBs->PutDateStr(DId, DateStr);
   }
  }
  // return bag-of-words
  BowDocBs->AssertOk();
  printf("\nDone.\n");
  return BowDocBs;
}

PBowDocBs TNmObjBs::GetNmBowDocBs(
 const PBowDocBs& BowDocBs, const PBowDocWgtBs& BowDocWgtBs, const int& MnNmObjFq) const {
  int NmObjs=GetNmObjs();
  for (int NmObjId=0; NmObjId<NmObjs; NmObjId++){
    printf("%d/%d\r", (1+NmObjId), NmObjs);
    TStr NmObjStr=GetNmObjStr(NmObjId);
    PBowSpV BowSpV=GetNmObjConcept(BowDocBs, BowDocWgtBs, NmObjStr);
  }
  printf("\n");
  return NULL;
}

PBowDocBs TNmObjBs::GetRelBowDocBs(const PBowDocBs& BowDocBs, const PBowDocWgtBs& BowDocWgtBs, const int& MnNmObjFq) const {
  return NULL;
}

void TNmObjBs::SaveTxtNmObj(const TStr& FqFNm, const TStr& SwFNm,
 const TStr& AbcFNm, const TStr& DocFNm) const {
  // get sorted frequency/named-object vector
  TIntStrVPrV FqWordStrVPrV(NmObjWordStrVToDocIdVH.Len(), 0);
  TStrVIntPrV WordStrVFqPrV(NmObjWordStrVToDocIdVH.Len(), 0);
  for (int NmObjId=0; NmObjId<NmObjWordStrVToDocIdVH.Len(); NmObjId++){
    int Fq=NmObjWordStrVToDocIdVH[NmObjId].Len();
    const TStrV& WordStrV=NmObjWordStrVToDocIdVH.GetKey(NmObjId);
    FqWordStrVPrV.Add(TIntStrVPr(Fq, WordStrV));
    WordStrVFqPrV.Add(TStrVIntPr(WordStrV, Fq));
  }
  FqWordStrVPrV.Sort(false);
  WordStrVFqPrV.Sort();
  // save by frequency
  if (!FqFNm.Empty()){
    printf("Saving by frequency to '%s' ...", FqFNm.CStr());
    TFOut SOut(FqFNm); FILE* fOut=SOut.GetFileId();
    for (int PrN=0; PrN<FqWordStrVPrV.Len(); PrN++){
      int Fq=FqWordStrVPrV[PrN].Val1;
      TStrV& WordStrV=FqWordStrVPrV[PrN].Val2;
      TStr WordStrVStr=GetWordStrVStr(WordStrV);
      fprintf(fOut, "%d - %s\n", Fq, WordStrVStr.CStr());
    }
    printf(" Done.\n");
  }
  // save for stopword
  if (!SwFNm.Empty()){
    printf("Saving by frequency for stop-words proposal to '%s' ...", SwFNm.CStr());
    TFOut SOut(SwFNm); FILE* fOut=SOut.GetFileId();
    for (int PrN=0; PrN<FqWordStrVPrV.Len(); PrN++){
      TStrV& WordStrV=FqWordStrVPrV[PrN].Val2;
      TStr WordStrVStr=GetWordStrVStr(WordStrV, ' ');
      fprintf(fOut, "%s\n", WordStrVStr.CStr());
    }
    printf(" Done.\n");
  }
  // save by alphabet
  if (!AbcFNm.Empty()){
    printf("Saving by alphabet to '%s' ...", AbcFNm.CStr());
    TFOut SOut(AbcFNm); FILE* fOut=SOut.GetFileId();
    for (int PrN=0; PrN<WordStrVFqPrV.Len(); PrN++){
      TStrV& WordStrV=WordStrVFqPrV[PrN].Val1;
      int Fq=WordStrVFqPrV[PrN].Val2;
      TStr WordStrVStr=GetWordStrVStr(WordStrV);
      fprintf(fOut, "%s - %d [", WordStrVStr.CStr(), Fq);
      int NmObjId=GetNmObjId(WordStrV);
      TIntV DocIdV;
      GetNmObjDocIdV(NmObjId, DocIdV);
      for (int DocIdN=0; DocIdN<DocIdV.Len(); DocIdN++){
        TStr DocNm=GetDocNm(DocIdV[DocIdN]);
        fprintf(fOut, "'%s' ", DocNm.CStr());
      }
      fprintf(fOut, "]\n");
    }
    printf(" Done.\n");
  }
  // save by documents
  if (!DocFNm.Empty()){
    printf("Saving by documents to '%s' ...", DocFNm.CStr());
    TFOut SOut(DocFNm); FILE* fOut=SOut.GetFileId();
    for (int DocId=0; DocId<GetDocs(); DocId++){
      TStr DocNm=GetDocNm(DocId);
      fprintf(fOut, "'%s'(%d):", DocNm.CStr(), DocId);
      TStr DateStr=GetDocDateStr(DocId);
      if (!DateStr.Empty()){
        fprintf(fOut, "[%s]", DateStr.CStr());}
      int DocNmObjs=GetDocNmObjs(DocId);
      for (int DocNmObjN=0; DocNmObjN<DocNmObjs; DocNmObjN++){
        int NmObjId; int TermFq; GetDocNmObjId(DocId, DocNmObjN, NmObjId, TermFq);
        TStr NmObjStr=GetNmObjStr(NmObjId);
        fprintf(fOut, " [%s:%d]", NmObjStr.CStr(), TermFq);
      }
      fprintf(fOut, "\n");
    }
    printf(" Done.\n");
  }
}

