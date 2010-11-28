/////////////////////////////////////////////////
// Includes
#include "hc.h"
#include "html.h"

/////////////////////////////////////////////////
// Html-Coded-Base
THcBase::THcBase():
  WordH(100000), DocH(1000),
  StopStrH(100), FlagTagH(100), ContTagH(100), SkipTagH(100), BreakTagH(100){
  // Stop-Strings
  for (char Ch='A'; Ch<='Z'; Ch++){GenStopStr(Ch);}
  GenStopStr("AN"); GenStopStr("AS"); GenStopStr("AT");
  GenStopStr("BE"); GenStopStr("BY"); GenStopStr("IT");
  GenStopStr("IF"); GenStopStr("OF"); GenStopStr("OR");
  GenStopStr("ON"); GenStopStr("IN"); GenStopStr("IS");
  GenStopStr("SO"); GenStopStr("TO");
  GenStopStr("AND"); GenStopStr("ARE"); GenStopStr("FOR");
  GenStopStr("THE");

  // Flag-Tags
  GenFlagTag(">STR<"); GenFlagTag(">DSTR<");
  GenFlagTag(">BTAG<"); GenFlagTag(">ETAG<"); GenFlagTag(">URL<");
  GenFlagTag("<HEAD>"); GenFlagTag("<BODY>");
  GenFlagTag("<TITLE>"); GenFlagTag("<A>");
  GenFlagTag("<H1>"); GenFlagTag("<H2>"); GenFlagTag("<H3>");
  GenFlagTag("<H4>"); GenFlagTag("<H5>"); GenFlagTag("<H6>");

  // Continuous-Tags
  GenContTag("<B>"); GenContTag("<BIG>"); GenContTag("<BLINK>");
  GenContTag("<EM>"); GenContTag("<FONT>"); GenContTag("<I>");
  GenContTag("<SMALL>"); GenContTag("<STRONG>"); GenContTag("<TT>");
  GenContTag("<U>");

  // Break-Tags
  GenBreakTag("<BR>"); GenBreakTag("<HR>"); GenBreakTag("<P>");
  GenBreakTag("<DL>"); GenBreakTag("<UL>"); GenBreakTag("<OL>");
  GenBreakTag("<LI>"); GenBreakTag("<DT>"); GenBreakTag("<DD>");
  GenBreakTag("<HEAD>"); GenBreakTag("<BODY>"); GenBreakTag("<TITLE>");
  GenFlagTag("<H1>"); GenFlagTag("<H2>"); GenFlagTag("<H3>");
  GenFlagTag("<H4>"); GenFlagTag("<H5>"); GenFlagTag("<H6>");

  // Skip-Tags
  GenSkipTag("<SCRIPT>");

  // Flag Shortcuts
  StrFN=FlagTagH.GetKeyId(TInt(WordH.AddKey(">STR<")));
  DStrFN=FlagTagH.GetKeyId(TInt(WordH.AddKey(">DSTR<")));
  BTagFN=FlagTagH.GetKeyId(TInt(WordH.AddKey(">BTAG<")));
  ETagFN=FlagTagH.GetKeyId(TInt(WordH.AddKey(">ETAG<")));
  UrlFN=FlagTagH.GetKeyId(TInt(WordH.AddKey(">URL<")));
  AnchFN=FlagTagH.GetKeyId(TInt(WordH.AddKey("<A>")));

  // Tag Shortcuts
  TitleTagId=WordH.AddKey("<TITLE>"); AnchTagId=WordH.AddKey("<A>");
  H1TagId=WordH.AddKey("<H1>"); H2TagId=WordH.AddKey("<H2>");
  H3TagId=WordH.AddKey("<H3>"); H4TagId=WordH.AddKey("<H4>");
  H5TagId=WordH.AddKey("<H5>"); H6TagId=WordH.AddKey("<H6>");

  // Fixed-Words
  FxWordIds=WordH.GetMxKeyIds();
}

THcBase::THcBase(TSIn& SIn):
  WordH(SIn), DocH(SIn),
  StopStrH(SIn), FlagTagH(SIn), ContTagH(SIn), SkipTagH(SIn), BreakTagH(SIn),
  StrFN(SIn), DStrFN(SIn), BTagFN(SIn), ETagFN(SIn), UrlFN(SIn), AnchFN(SIn),
  TitleTagId(SIn), AnchTagId(SIn),
  H1TagId(SIn), H2TagId(SIn), H3TagId(SIn),
  H4TagId(SIn), H5TagId(SIn), H6TagId(SIn),
  FxWordIds(SIn){}

void THcBase::Save(TSOut& SOut){
  WordH.Save(SOut); DocH.Save(SOut);
  StopStrH.Save(SOut); FlagTagH.Save(SOut); ContTagH.Save(SOut);
  SkipTagH.Save(SOut); BreakTagH.Save(SOut);
  StrFN.Save(SOut); DStrFN.Save(SOut); BTagFN.Save(SOut);
  ETagFN.Save(SOut); UrlFN.Save(SOut); AnchFN.Save(SOut);
  TitleTagId.Save(SOut), AnchTagId.Save(SOut),
  H1TagId.Save(SOut), H2TagId.Save(SOut), H3TagId.Save(SOut),
  H4TagId.Save(SOut), H5TagId.Save(SOut), H6TagId.Save(SOut),
  FxWordIds.Save(SOut);
}

void THcBase::AddDoc(const TStr& FNm, const TStr& _Nm){
  THtmlLx Lx(PSIn(new TFIn(FNm)));
  TStr Nm=_Nm; if (Nm.Empty()){Nm=FNm;}
  PHcDoc Doc=DocH.AddDat(Nm, PHcDoc(new THcDoc()));
  TB32Set TagSet; TB32Set StrFSet(StrFN), DStrFSet(DStrFN);
  TB32Set BTagFSet(BTagFN), ETagFSet(ETagFN); TB32Set UrlFSet(UrlFN);
  THash<TInt, TInt> DocWordsH(1000);
  int PStrWordId=-1;
  while (Lx.GetSym()!=hlsyEof){
    switch (Lx.Sym){
      case hlsyStr: case hlsyNum:{
        int StrWordId=WordH.AddKey(Lx.Str);
        if (!StopStrH.IsKey(StrWordId)){
          DocWordsH.AddDat(StrWordId)++;
          Doc->Add(THcWord(TFSet(StrFSet)|TagSet, StrWordId));
          if (PStrWordId!=-1){
            int DStrWordId=AddDStrKey(PStrWordId, StrWordId);
            DocWordsH.AddDat(DStrWordId)++;
            Doc->Add(THcWord(TFSet(StrFSet)|DStrFSet|TagSet, DStrWordId));
          }
          PStrWordId=StrWordId;
        } break;}
      case hlsySSym:
        break;
      case hlsyUrl:{
        int UrlWordId=WordH.AddKey(Lx.Str);
        DocWordsH.AddDat(UrlWordId)++;
        Doc->Add(THcWord(TFSet(UrlFSet)|TagSet, UrlWordId));
        PStrWordId=-1;
        break;}
      case hlsyBTag:{
        int TagId=WordH.AddKey(Lx.Str);
        int FlagN=FlagTagH.GetKeyId(TagId);
        if (FlagN!=-1){TagSet.Incl(FlagN);}
        if (!ContTagH.IsKey(TagId)){PStrWordId=-1;}
        static TStr AltStr("ALT");
        if (Lx.IsArg(AltStr)){
          Lx.PutStr(Lx.GetArg(AltStr)); Lx.PutCh(' ');}
        if (SkipTagH.IsKey(TagId)){
          do {Lx.GetSym();} while ((Lx.Sym!=hlsyEof)&&
           ((Lx.Sym!=hlsyETag)||(WordH.AddKey(Lx.Str)!=TagId)));
          if (Lx.Sym!=hlsyEof){Lx.GetSym();}
        } else {
          DocWordsH.AddDat(TagId)++;
          Doc->Add(THcWord(TFSet(BTagFSet)|TagSet, TagId));
          if (TagId==AnchTagId){
            static TStr HRefStr("HREF");
            if (Lx.IsArg(HRefStr)){
              int UrlWordId=WordH.AddKey(Lx.GetArg(HRefStr));
              DocWordsH.AddDat(UrlWordId)++;
              Doc->Add(THcWord(TFSet(UrlFSet)|TagSet, UrlWordId));
            }
          }
        } break;}
      case hlsyETag:{
        int TagId=WordH.AddKey(Lx.Str);
        int FlagN=FlagTagH.GetKeyId(TagId);
        if (FlagN!=-1){TagSet.Excl(FlagN);}
        if (!ContTagH.IsKey(TagId)){PStrWordId=-1;}
        DocWordsH.AddDat(TagId)++;
        Doc->Add(THcWord(TFSet(ETagFSet)|TagSet, TagId));
        break;}
      default:;// printf("Undef "); //getchar();
    }
  }

  int DocWordsP=DocWordsH.FFirstKeyId();
  while (DocWordsH.FNextKeyId(DocWordsP)){
    int WordId=DocWordsH.GetKey(DocWordsP);
    WordH[WordId].Val1++;
    WordH[WordId].Val2+=DocWordsH[DocWordsP];
  }
}

void THcBase::DelDoc(const TStr& DocNm){
  THash<TInt, TInt> DocWordsH(1000);
  PHcDoc Doc=DocH.GetDat(DocNm);
  for (int WordN=0; WordN<Doc->Len(); WordN++){
    DocWordsH.AddDat(Doc->Get(WordN).GetWordId())++;}

  int DocWordsP=DocWordsH.FFirstKeyId();
  while (DocWordsH.FNextKeyId(DocWordsP)){
    int WordId=DocWordsH.GetKey(DocWordsP);
    WordH[WordId].Val1--;
    WordH[WordId].Val2-=DocWordsH[DocWordsP];
    Assert((WordH[WordId].Val1>=0)&&(WordH[WordId].Val2>=0));
    if (WordH[WordId].Val1==0){Assert(WordH[WordId].Val2==0);
      if (FxWordIds<=WordId){WordH.DelKeyId(WordId);}}
  }
}

void THcBase::DelDocs(){
  int HcDocN=DocH.FFirstKeyId();
  while (DocH.FNextKeyId(HcDocN)){DelDoc(DocH.GetKey(HcDocN));}

  int WordId=WordH.FFirstKeyId();
  while (WordH.FNextKeyId(WordId)){Assert(WordH[WordId].Val2==0);}
}

void THcBase::ClrAnchFlag(const TVec<THcWord>& HcWordV){
  for (int HcWordN=0; HcWordN<HcWordV.Len(); HcWordN++){
    HcWordV[HcWordN].SetFlag(AnchFN, false);}
}

TVec<PHcDoc> THcBase::GetHLDocV(const int& DocId){
  TVec<PHcDoc> HLDocV;
  TVec<THcWord> TitleWordV; bool IsTitleOpen=false;
  TVec<THcWord> H1WordV; bool IsH1Open=false;
  TVec<THcWord> H2WordV; bool IsH2Open=false;
  TVec<THcWord> H3WordV; bool IsH3Open=false;
  TVec<THcWord> H4WordV; bool IsH4Open=false;
  TVec<THcWord> H5WordV; bool IsH5Open=false;
  TVec<THcWord> H6WordV; bool IsH6Open=false;
  bool IsAOpen=false;
  PHcDoc Doc=GetDoc(DocId);
  for (int WordN=0; WordN<Doc->Len(); WordN++){
    THcWord Word=Doc->Get(WordN); int WordId=Word.GetWordId();
    if (Word.IsFlag(BTagFN)){
      if (WordId==TitleTagId){TitleWordV.Clr(); IsTitleOpen=true;}
      if (WordId==AnchTagId){IsAOpen=true;}
      if (WordId==H1TagId){H1WordV.Clr(); IsH1Open=true;}
      if (WordId==H2TagId){H2WordV.Clr(); IsH2Open=true;}
      if (WordId==H3TagId){H3WordV.Clr(); IsH3Open=true;}
      if (WordId==H4TagId){H4WordV.Clr(); IsH4Open=true;}
      if (WordId==H5TagId){H5WordV.Clr(); IsH5Open=true;}
      if (WordId==H6TagId){H6WordV.Clr(); IsH6Open=true;}
    } else if (Word.IsFlag(ETagFN)){
      if (WordId==TitleTagId){IsTitleOpen=false;}
      if (WordId==AnchTagId){IsAOpen=false;}
      if (WordId==H1TagId){IsH1Open=false;}
      if (WordId==H2TagId){IsH2Open=false;}
      if (WordId==H3TagId){IsH3Open=false;}
      if (WordId==H4TagId){IsH4Open=false;}
      if (WordId==H5TagId){IsH5Open=false;}
      if (WordId==H6TagId){IsH6Open=false;}
    } else if (Word.IsFlag(StrFN)){
      if (IsTitleOpen){TitleWordV.Add(Word);}
      if (IsH1Open){H1WordV.Add(Word);}
      if (IsH2Open){H2WordV.Add(Word);}
      if (IsH3Open){H3WordV.Add(Word);}
      if (IsH4Open){H4WordV.Add(Word);}
      if (IsH5Open){H5WordV.Add(Word);}
      if (IsH6Open){H6WordV.Add(Word);}
    } else if (Word.IsFlag(UrlFN)){
      TVec<THcWord> AWordV; int AWordN=WordN+1;
      if (IsAOpen){
        while ((AWordN<Doc->Len())&&
         ((!Doc->Get(AWordN).IsFlag(ETagFN))||
         (Doc->Get(AWordN).GetWordId()!=AnchTagId))){
          if (Doc->Get(AWordN).IsFlag(StrFN)){
            AWordV.Add(Doc->Get(AWordN));} AWordN++;
         }
         IsAOpen=false;
      }

      TVec<THcWord> PrevWordV; int PrevWordN=WordN-1;
      while ((PrevWordN>=0)&&(PrevWordV.Len()<10)&&
       (!BreakTagH.IsKey(Doc->Get(PrevWordN).GetWordId()))){
        if (Doc->Get(PrevWordN).IsFlag(StrFN)){
          PrevWordV.Add(Doc->Get(PrevWordN));}
        PrevWordN--;
      }

      TVec<THcWord> NextWordV; int NextWordN=AWordN+1;
      while ((NextWordN<Doc->Len())&&(NextWordV.Len()<10)&&
       (!BreakTagH.IsKey(Doc->Get(NextWordN).GetWordId()))){
        if (Doc->Get(NextWordN).IsFlag(StrFN)){
          NextWordV.Add(Doc->Get(NextWordN));}
        NextWordN++;
      }

      ClrAnchFlag(TitleWordV);
      ClrAnchFlag(H1WordV); ClrAnchFlag(H2WordV); ClrAnchFlag(H3WordV);
      ClrAnchFlag(H4WordV); ClrAnchFlag(H5WordV); ClrAnchFlag(H6WordV);
      ClrAnchFlag(PrevWordV); ClrAnchFlag(NextWordV);

      int HLDocN=HLDocV.Add(PHcDoc(new THcDoc()));
      HLDocV[HLDocN]->Add(TitleWordV);
      HLDocV[HLDocN]->Add(H1WordV);
      HLDocV[HLDocN]->Add(H2WordV);
      HLDocV[HLDocN]->Add(H3WordV);
      HLDocV[HLDocN]->Add(H4WordV);
      HLDocV[HLDocN]->Add(H5WordV);
      HLDocV[HLDocN]->Add(H6WordV);
      HLDocV[HLDocN]->Add(PrevWordV);
      HLDocV[HLDocN]->Add(AWordV);
      HLDocV[HLDocN]->Add(NextWordV);
      WrDoc(WordH.GetKey(WordId), HLDocV[HLDocN]);
    }
  }
  return HLDocV;
}

void THcBase::WrDoc(const TStr& UrlStr, const PHcDoc& Doc){
  printf("'%s' -->", UrlStr.CStr());
  for (int WordN=0; WordN<Doc->Len(); WordN++){
    THcWord Word=Doc->Get(WordN); int WordId=Word.GetWordId();
    printf("[");
    if (Word.IsFlag(FlagTagH.GetKeyId(TInt(WordH.GetKeyId("<TITLE>"))))){printf("T");}
    if (Word.IsFlag(FlagTagH.GetKeyId(TInt(WordH.GetKeyId("<A>"))))){printf("A");}
    if (Word.IsFlag(FlagTagH.GetKeyId(TInt(WordH.GetKeyId("<H1>"))))){printf("H1");}
    if (Word.IsFlag(FlagTagH.GetKeyId(TInt(WordH.GetKeyId("<H2>"))))){printf("H2");}
    if (Word.IsFlag(FlagTagH.GetKeyId(TInt(WordH.GetKeyId("<H3>"))))){printf("H3");}
    if (Word.IsFlag(FlagTagH.GetKeyId(TInt(WordH.GetKeyId("<H4>"))))){printf("H4");}
    if (Word.IsFlag(FlagTagH.GetKeyId(TInt(WordH.GetKeyId("<H5>"))))){printf("H5");}
    if (Word.IsFlag(FlagTagH.GetKeyId(TInt(WordH.GetKeyId("<H6>"))))){printf("H6");}
    printf(":'%s'] ", WordH.GetKey(WordId).CStr());
  }
  printf("\n");
}

void THcBase::Wr(){
  const int MxOcc=100;
  int OccT[MxOcc];
  for (int Occ=0; Occ<MxOcc; Occ++){OccT[Occ]=0;}
  printf("Words:%d\n", WordH.Len());
  int Lns=0;
  int WordId=WordH.FFirstKeyId();
  while (WordH.FNextKeyId(WordId)){
    int Occ=WordH[WordId].Val2;
    if (Occ<MxOcc){OccT[Occ]++;}
//    if ((Occ>50)&&(strstr(WordV.GetKey(WordId).CStr(), " "))){
    if (Occ>100){
      printf("%6d '%s'\n", Occ, WordH.GetKey(WordId).CStr());
      if (++Lns%20==0) getchar();
    }
  }
  {for (int Occ=0; Occ<MxOcc; Occ++){printf("(%d:%d) ", Occ, OccT[Occ]);}}
}


