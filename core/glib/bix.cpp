/////////////////////////////////////////////////
// Includes
#include "StdAfx.h"
#include "bix.h"
#include "book.h"

/////////////////////////////////////////////////
// Book-Index-Context
TStr TBixCtx::GetStr() const {
  TChA ChA;
  ChA+=TInt::GetStr(SecId);
  ChA+='['; ChA+=FSet.GetStr(); ChA+=']';
  return ChA;
}

int TBixCtx::TitleFlagN=0;
int TBixCtx::ContinentFlagN=1;
int TBixCtx::AreaFlagN=2;

/////////////////////////////////////////////////
// Book-Index-Context-Set
PTqlCtxSet TBixCtxSet::GetAndCtxSet(const PTqlCtxSet& CtxSet){
  TBixCtxSet* DstCtxSet=new TBixCtxSet();
  TBixCtxV& V1=BixCtxV;
  TBixCtxV& V2=((TBixCtxSet*)CtxSet())->BixCtxV;
  TBixCtxV& DstV=DstCtxSet->BixCtxV;

  int N1=0; int N2=0;
  while ((N1<V1.Len())&&(N2<V2.Len())){
    TBixCtx& Val1=V1[N1];
    while ((N2<V2.Len())&&(Val1>V2[N2])){
      N2++;}
    if ((N2<V2.Len())&&(Val1==V2[N2])){
      TBixCtx Val(Val1); Val.Merge(V2[N2]);
      DstV.Add(Val); N2++;
    }
    N1++;
  }
  return PTqlCtxSet(DstCtxSet);
}

PTqlCtxSet TBixCtxSet::GetOrCtxSet(const PTqlCtxSet& CtxSet){
  TBixCtxSet* DstCtxSet=new TBixCtxSet();
  TBixCtxV& V1=BixCtxV;
  TBixCtxV& V2=((TBixCtxSet*)CtxSet())->BixCtxV;
  TBixCtxV& DstV=DstCtxSet->BixCtxV;

  int N1=0; int N2=0;
  while ((N1<V1.Len())&&(N2<V2.Len())){
    TBixCtx& Val1=V1[N1];
    TBixCtx& Val2=V2[N2];
    if (Val1<Val2){DstV.Add(Val1); N1++;}
    else if (Val1>Val2){DstV.Add(Val2); N2++;}
    else {TBixCtx Val(Val1); Val.Merge(Val2); DstV.Add(Val); N1++; N2++;}
  }
  for (int RestN1=N1; RestN1<V1.Len(); RestN1++){DstV.Add(V1[RestN1]);}
  for (int RestN2=N2; RestN2<V2.Len(); RestN2++){DstV.Add(V2[RestN2]);}
  return PTqlCtxSet(DstCtxSet);
}

PTqlCtxSet TBixCtxSet::GetTagCtxSet(const TStr& TagNm){
  int FlagN=-1;
  if (TagNm=="title"){FlagN=TBixCtx::TitleFlagN;}
  if (TagNm=="continent"){FlagN=TBixCtx::ContinentFlagN;}
  if (TagNm=="area"){FlagN=TBixCtx::AreaFlagN;}
  if (FlagN==-1){
    return this;
  } else {
    TBixCtxSet* DstCtxSet=new TBixCtxSet();
    TBixCtxV& V=BixCtxV;
    TBixCtxV& DstV=DstCtxSet->BixCtxV;
    for (int N=0; N<V.Len(); N++){
      if (V[N].IsFlag(FlagN)){
        DstV.Add(V[N]);}
    }
    return PTqlCtxSet(DstCtxSet);
  }
}

PTqlCtxSet TBixCtxSet::GetTitleCtxSet(){
  TBixCtxSet* DstCtxSet=new TBixCtxSet();
  TBixCtxV& V=BixCtxV;
  TBixCtxV& DstV=DstCtxSet->BixCtxV;
  for (int N=0; N<V.Len(); N++){
    if (V[N].IsFlag(TBixCtx::TitleFlagN)){
      DstV.Add(V[N]);}
  }
  return PTqlCtxSet(DstCtxSet);
}

TStr TBixCtxSet::GetStr() const {
  TChA ChA;
  for (int CtxN=0; CtxN<BixCtxV.Len(); CtxN++){
    if (CtxN>0){ChA+=' ';}
    ChA+=BixCtxV[CtxN].GetStr();
  }
  return ChA;
}

/////////////////////////////////////////////////
// Book-Inverted-Index
void TBix::MergeBixCtx(const TStr& WordStr, const TBixCtx& BixCtx){
  // make this more efficient on per document basis //**
  int WordToCtxVP;
  if (WordToCtxVH.IsKey(WordStr, WordToCtxVP)){
    TBixCtxV& BixCtxV=WordToCtxVH[WordToCtxVP];
    int BixCtxN;
    if (BixCtxV.IsIn(BixCtx, BixCtxN)){
      BixCtxV[BixCtxN].Merge(BixCtx); // if context already exists
    } else {
      WordToCtxVH.AddDat(WordStr).AddMerged(BixCtx);
    }
  } else {
    WordToCtxVH.AddDat(WordStr).AddMerged(BixCtx);
  }
}

PTqlCtxSet TBix::GetEmptyCtxSet() const {
  return PTqlCtxSet(new TBixCtxSet());
}

PTqlCtxSet TBix::GetCtxSet(
 const TStr& WordStr, const bool& IsWc, const int& MxWcWords) const {
  IAssert(Access==faRdOnly);
  TStr NrWordStr=ChDef->GetUcStr(WordStr);
  PTqlCtxSet CtxSet;
  if (IsWc){
    // wildcard word
    int FirstWordN; int WordN=WordV.SearchBin(NrWordStr, FirstWordN);
    if (WordN==-1){
      CtxSet=PTqlCtxSet(new TBixCtxSet()); WordN=FirstWordN;
    } else {
      CtxSet=PTqlCtxSet(new TBixCtxSet(WordToCtxVH.GetDat(NrWordStr))); WordN++;
    }
    int WcWords=0;
    while ((WordN<WordV.Len())&&(WordV[WordN].IsPrefix(NrWordStr))&&
     (WcWords<MxWcWords)){
      TStr PfxNrWordStr=WordV[WordN];
      PTqlCtxSet SubCtxSet=
       PTqlCtxSet(new TBixCtxSet(WordToCtxVH.GetDat(PfxNrWordStr)));
      CtxSet=CtxSet->GetOrCtxSet(SubCtxSet);
      WordN++; WcWords++;
    }
  } else {
    // exact match
    int WordP;
    if (WordToCtxVH.IsKey(NrWordStr, WordP)){
      CtxSet=PTqlCtxSet(new TBixCtxSet(WordToCtxVH[WordP]));
    } else {
      CtxSet=PTqlCtxSet(new TBixCtxSet());
    }
  }
  return CtxSet;
}

PTqlCtxSet TBix::GetCtxSet(
 const TStr& TagNm, const TRelOp& RelOp, const TStr& ArgValStr,
 const int& MxWcWords) const {
  IAssert(Access==faRdOnly);
  int ArgVal;
  if (ArgValStr.IsInt(ArgVal)){
    TBixCtxV BixCtxV;
    for (int KeyN=0; KeyN<SecIdKeyNmValTrV.Len(); KeyN++){
      if (SecIdKeyNmValTrV[KeyN].Val2==TagNm){
        int KeyVal=SecIdKeyNmValTrV[KeyN].Val3;
        if (Cmp(RelOp, KeyVal, ArgVal)){
          int SecId=SecIdKeyNmValTrV[KeyN].Val1;
          BixCtxV.Add(TBixCtx(SecId));
        }
      }
      if ((MxWcWords!=-1)&&(BixCtxV.Len()>MxWcWords)){break;}
    }
    BixCtxV.Merge();
    PTqlCtxSet CtxSet=PTqlCtxSet(new TBixCtxSet(BixCtxV));
    return CtxSet;
  } else {
    return PTqlCtxSet(new TBixCtxSet());
  }
}

void TBix::AddSec(const int& SecId, const TStr& SecStr, const bool& TitleP){
  IAssert(Access==faCreate);
  PSIn HtmlSIn=TStrIn::New(SecStr);
  THtmlLx Lx(HtmlSIn, false);
  bool InTitleTag=TitleP; bool InContinentTag=false; bool InAreaTag=false;
  while (Lx.GetSym()!=hsyEof){
    if ((Lx.Sym==hsyStr)||(Lx.Sym==hsyNum)){
      TStr WordStr(Lx.UcChA);
      if (!SwSet->IsIn(WordStr)){
        TBixCtx BixCtx(SecId);
        if (InTitleTag){BixCtx.SetFlag(TBixCtx::TitleFlagN);}
        if (InContinentTag){BixCtx.SetFlag(TBixCtx::ContinentFlagN);}
        if (InAreaTag){BixCtx.SetFlag(TBixCtx::AreaFlagN);}
        MergeBixCtx(WordStr, BixCtx);
      }
    } else
    if ((Lx.Sym==hsyBTag)||(Lx.Sym==hsyETag)){
      TStr TagNm=Lx.UcChA;
      if (TagNm==THtmlTok::TitleTagNm){InTitleTag=(Lx.Sym==hsyBTag);}
      else if (TagNm==ContinentTagNm){InContinentTag=(Lx.Sym==hsyBTag);}
      else if (TagNm==AreaTagNm){InAreaTag=(Lx.Sym==hsyBTag);}
    }
  }
}

PBixRes TBix::Search(const TStr& QueryStr){
  PTqlExp TqlExp=TTqlExp::New(QueryStr);
  PBixRes BixRes;
  if (TqlExp->IsOk()){
    PTqlCtxSet CtxSet=TqlExp->Eval(*this);
    BixRes=PBixRes(new TBixRes(this, QueryStr, CtxSet));
  } else {
    BixRes=PBixRes(new TBixRes(TqlExp->GetErrMsg()));
  }
  return BixRes;
}

/////////////////////////////////////////////////
// Book-Index-Results
TBixRes::TBixRes(
 const PBix& _Bix, const TStr& _QueryStr, const PTqlCtxSet& _CtxSet):
  Ok(true), MsgStr(TStr::OkStr),
  Bix(_Bix), QueryStr(_QueryStr), CtxSet(_CtxSet){
  // hit scoring code
}

void TBixRes::GetHitInfo(
 const PBook& Book,
 const int& HitN, const int& MxTitleLen, const int& MxCtxLen,
 TStr& SecIdStr, TStr& TitleStr, TStr& SecStr, TStr& CtxStr) const {
  // get section data
  int SecId=CtxSet->GetCtxSecId(HitN);
  Book->GetSecInfo(SecId, SecIdStr, TitleStr, SecStr);

  // convert document string to html
  PSIn HtmlSIn=TStrIn::New(SecStr);
  THtmlLx Lx(HtmlSIn);

  // construct title and document-context
  TChA TitleChA; bool IsTitleComplete=true;
  TChA CtxChA;
  TLxSym PrevLxSym=syUndef; TLxSym CurLxSym=syUndef;
  bool InTitleTag=false;
  while (((MxCtxLen==-1)||(CtxChA.Len()<MxCtxLen))&&
   (Lx.GetSym()!=hsyEof)){
    if ((Lx.Sym==hsyStr)||(Lx.Sym==hsyNum)||(Lx.Sym==hsySSym)){
      PrevLxSym=CurLxSym;
      CurLxSym=THtmlDoc::GetLxSym(Lx.Sym, Lx.UcChA);
      if (InTitleTag){
        if ((MxTitleLen==-1)||(TitleChA.Len()<MxTitleLen)){
          if (TLxSymStr::IsSep(PrevLxSym, CurLxSym)){TitleChA+=' ';}
          TitleChA+=Lx.ChA;
        } else {
          IsTitleComplete=false;
        }
      } else {
        if (TLxSymStr::IsSep(PrevLxSym, CurLxSym)){CtxChA+=' ';}
        CtxChA+=Lx.ChA;
      }
    } else
    if ((Lx.Sym==hsyBTag)||(Lx.Sym==hsyETag)){
      if (Lx.UcChA==THtmlTok::TitleTagNm){
        InTitleTag=(Lx.Sym==hsyBTag); PrevLxSym=syUndef;
      } else
      if (Lx.UcChA==THtmlTok::CardTagNm){
        TitleChA=Lx.GetArg(THtmlTok::TitleArgNm);
      }
    }
  }

  // adjustments
  // title
  if (TitleChA.Empty()){TitleChA=TitleStr;}
  else {if (!IsTitleComplete){TitleChA+=" ...";}}
  TitleStr=THtmlLx::GetEscapedStr(TitleChA);
  // context
  if (Lx.Sym!=hsyEof){CtxChA+=" ...";}
  CtxStr=THtmlLx::GetEscapedStr(CtxChA);
  // section
  SecStr=THtmlLx::GetEscapedStr(SecStr);
}

void TBixRes::GetHitSetMnMxHitN(
 const int& HitSetN, const int& HitSetSecs, int& MnHitN, int& MxHitN) const {
  // general checks
  IAssert(IsOk());
  IAssert((HitSetN>0)&&(HitSetSecs>0));
  // min. & max. hit numbers in current hit-set
  MnHitN=(HitSetN-1)*HitSetSecs;
  MxHitN=MnHitN+HitSetSecs-1;
  MxHitN=TInt::GetMn(MxHitN, GetHits()-1);
}

void TBixRes::GetHitSet(
 const int& HitSetN, const int& HitSetSecs, const int& TocHitSets,
 const TStr& HitSetUrlFldNm, const PUrlSearchEnv& UrlSearchEnv,
 TStr& PrevTocUrlStr, TStrPrV& TocNmUrlStrPrV, TStr& NextTocUrlStr) const {
  // prepare output values
  PrevTocUrlStr.Clr(); TocNmUrlStrPrV.Clr(); NextTocUrlStr.Clr();

  // general checks
  IAssert(IsOk());
  IAssert((HitSetN>0)&&(HitSetSecs>0)&&(TocHitSets>0));
  if (GetHits()==0){return;}

  // min. & max. global hit-set numbers
  int MnHitSetN=1;
  int MxHitSetN=(GetHits()-1)/HitSetSecs+1;
  if ((HitSetN<MnHitSetN)||(MnHitSetN>MxHitSetN)){return;}

  // min. & max. hit-set numbers in current string
  int MnTocHitSetN=((HitSetN-1)/TocHitSets)*TocHitSets+1;
  int MxTocHitSetN=MnTocHitSetN+TocHitSets-1;
  MxTocHitSetN=TInt::GetMn(MxTocHitSetN, MxHitSetN);
  PUrlSearchEnv HitSetUSE=TUrlSearchEnv::MkClone(UrlSearchEnv);

  // previous hit-set
  if (HitSetN!=MnHitSetN){
    HitSetUSE->AddKeyVal(HitSetUrlFldNm, TInt::GetStr(HitSetN-1));
    PrevTocUrlStr=TStr("/")+HitSetUSE->GetStr();
  }

  // hit-set numbers
  for (int TocHitSetN=MnTocHitSetN; TocHitSetN<=MxTocHitSetN; TocHitSetN++){
    TStr Nm=TInt::GetStr(TocHitSetN);
    TStr UrlStr;
    if (TocHitSetN!=HitSetN){
      HitSetUSE->AddKeyVal(HitSetUrlFldNm, TInt::GetStr(TocHitSetN));
      UrlStr=TStr("/")+HitSetUSE->GetStr();
    }
    TocNmUrlStrPrV.Add(TStrPr(Nm, UrlStr));
  }

  // next hit-set
  if (HitSetN!=MxHitSetN){
    HitSetUSE->AddKeyVal(HitSetUrlFldNm, TInt::GetStr(HitSetN+1));
    NextTocUrlStr=TStr("/")+HitSetUSE->GetStr();
  }
}

TStr TBix::ContinentTagNm="<CONTINENT>";
TStr TBix::AreaTagNm="<AREA>";


