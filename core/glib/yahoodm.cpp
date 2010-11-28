/////////////////////////////////////////////////
// Includes
#include "yahoodm.h"

/////////////////////////////////////////////////
// Yahoo-Table
TYTb::TYTb(const PYBs& _YBs, const PYDsBs& _YDsBs):
  YBs(_YBs), YDsBs(_YDsBs),
  VarNToWordIdV(YBs->GetWords()), WordIdToVarNV(YBs->GetMxWordIds()),
  TupNToDocIdV(YBs->GetDocs()), DocIdToTupNV(YBs->GetMxDocIds()){

  int WordId=YBs->FFirstWordId(); int VarN=0-1;
  while (YBs->FNextWordId(WordId)){VarN++;
    VarNToWordIdV[VarN]=WordId; WordIdToVarNV[WordId]=VarN;}

  int DocId=YBs->FFirstDocId(); int TupN=0-1;
  while (YBs->FNextDocId(DocId)){TupN++;
    TupNToDocIdV[TupN]=DocId; DocIdToTupNV[DocId]=TupN;}
}

TTbVal TYTb::GetVal(const int& TupN, const int& VarN){
  int DocId=TupNToDocIdV[TupN];
  int WordId=VarNToWordIdV[VarN];
  double WordFq;
  if (YDsBs->GetWordDs(DocId)->IsWordId(WordId, WordFq)){return WordFq;}
  else {return double(0);}
}

/////////////////////////////////////////////////
// Yahoo-Domain-Distribution
TYDmDs::TYDmDs(
 const bool& _DoPriorDmDs, const int& ClassN,
 const int& _YNegDsType, const int& _YPriorType,
 const PYBs& _YBs, const PYDsBs& _YDsBs, const PDmHd& _DmHd):
  DoPriorDmDs(_DoPriorDmDs),
  YNegDsType(_YNegDsType), YPriorType(_YPriorType),
  YBs(_YBs), YDsBs(_YDsBs), DmHd(_DmHd),
  NegWordDs(), PosWordDs(),
  CValDs(), NegCValPrb(), PosCValPrb(){

  NegWordDs=GetNegWordDs(YNegDsType, YBs, YDsBs);
  PosWordDs=YDsBs->GetWordDs(ClassN);

  CValDs=GetPriorValDs(YPriorType, NegWordDs, PosWordDs);
  NegCValPrb=CValDs->GetPrb_RelFq(TTbVal::NegVal);
  PosCValPrb=CValDs->GetPrb_RelFq(TTbVal::PosVal);
}

double TYDmDs::GetSumW(){
  return CValDs->GetSumW();
}

PTbValDs TYDmDs::GetCDs(){
  return CValDs;
}

PTbValDs TYDmDs::GetAVDs(const int& AttrN){
  if (DoPriorDmDs){
    return TTbValDs::GetBoolValDs(-1, NegWordDs->GetWordPrb(AttrN));
  } else {
    return TTbValDs::GetBoolValDs(-1,
     NegCValPrb*NegWordDs->GetWordPrb(AttrN)+
     PosCValPrb*PosWordDs->GetWordPrb(AttrN));
  }
}

PTbValDs TYDmDs::GetCAVDs(const TTbVal& CVal, const int& AttrN){
  if (DoPriorDmDs){
    return TTbValDs::GetBoolValDs(-1, NegWordDs->GetWordPrb(AttrN));
  } else {
    if (CVal==TTbVal::NegVal){
      return TTbValDs::GetBoolValDs(-1, NegWordDs->GetWordPrb(AttrN));
    } else
    if (CVal==TTbVal::PosVal){
      return TTbValDs::GetBoolValDs(-1, PosWordDs->GetWordPrb(AttrN));
    } else {
      Fail; return NULL;
    }
  }
}

PYWordDs TYDmDs::GetNegWordDs(
 const int& YNegDsType, const PYBs& YBs, const PYDsBs& YDsBs){
  PYWordDs NegWordDs;
  switch (TYNegDsType(YNegDsType)){
    case yndtEmpty: NegWordDs=PYWordDs(new TYWordDs(0, 0, 0)); break;
    case yndtRoot: NegWordDs=YDsBs->GetWordDs(YBs->GetRootDocId()); break;
    case yndtAll: NegWordDs=YDsBs->GetAllWordDs(); break;
    default: Fail;
  }
  return NegWordDs;
}

PTbValDs TYDmDs::GetPriorValDs(const int& YPriorType,
 const PYWordDs& NegWordDs, const PYWordDs& PosWordDs){
  double PosW; double AllW;
  switch (TYPriorType(YPriorType)){
    case yptWords:
      PosW=PosWordDs->GetSumWordFq(); AllW=NegWordDs->GetSumWordFq(); break;
    case yptSects:
      PosW=PosWordDs->GetSects(); AllW=NegWordDs->GetSects(); break;
    case yptDocs:
      PosW=PosWordDs->GetDocs(); AllW=NegWordDs->GetDocs(); break;
    default: Fail;
  }
  return TTbValDs::GetBoolValDs(AllW, PosW);
}

TStr TYDmDs::GetYNegDsTypeStr(const TYNegDsType& YNegDsType){
  switch (YNegDsType){
    case yndtEmpty: return "Empty";
    case yndtRoot: return "Root";
    case yndtAll: return "All";
    default: Fail; return TStr();
  }
}

TStr TYDmDs::GetYPriorTypeStr(const TYPriorType& YPriorType){
  switch (YPriorType){
    case yptWords: return "Words";
    case yptSects: return "Sects";
    case yptDocs: return "Docs";
    default: Fail; return TStr();
  }
}

/////////////////////////////////////////////////
// Yahoo-Feature-Selection
TYFSelBs::TYFSelBs(
 const TYFSelType& FSelType, const double& FSels,
 const bool& FSelPosWords, const PAttrEst& AttrEst,
 const TYNegDsType& _YNegDsType, const TYPriorType& YPriorType,
 const PYBs& YBs, const PYDsBs& YDsBs, const PNotify& Notify):
  YNegDsType(_YNegDsType), DocIdToWordIdEstVV(YBs->GetDocs()){
  TNotify::OnNotify(Notify, ntInfo, "Start Feature Selection");

  PDmHd DmHd=new TYDmHd(YBs, YDsBs);
  PYWordDs NegWordDs=TYDmDs::GetNegWordDs(YNegDsType, YBs, YDsBs);
  PTbValSplit BoolValSplit=TTbValSplit::GetBoolValSplit();

  int DocId=YBs->FFirstDocId(); int DocIds=0;
  while (YBs->FNextDocId(DocId)){
    PYWordDs PosWordDs=YDsBs->GetWordDs(DocId); DocIds++;

    int SelWordIds;
    switch (FSelType){
      case yfstFix: SelWordIds=int(FSels); break;
      case yfstPosPrc:
        SelWordIds=int(FSels*double(PosWordDs->GetWordIds())); break;
      case yfstUnionPrc:{
        PYWordDs UnionWordDs=TYWordDs::GetMerged(PosWordDs, NegWordDs, 1, 1);
        SelWordIds=int(FSels*double(UnionWordDs->GetWordIds())); break;}
      default: Fail; SelWordIds=0;
    }
    if (SelWordIds<=0){SelWordIds=1;}

    PDmDs DmDs=PDmDs(new TYDmDs(
     false, DocId, YNegDsType, YPriorType, YBs, YDsBs, DmHd));
    PDmDs PriorDmDs=PDmDs(new TYDmDs(
     true, DocId, yndtAll, yptDocs, YBs, YDsBs, DmHd));
    PYWordDs WordDs; PYWordDs TrvWordDs;
    TIntH SelWordIdH(SelWordIds);
    TFltIntKdV WordEstIdKdV(SelWordIds, 0);
    for (int CDsc=0; CDsc<TTbVal::BoolVals; CDsc++){
      switch (CDsc){
        case 0: WordDs=NegWordDs; break;
        case 1: WordDs=PosWordDs; break;
        default: Fail;
      }
      if (FSelPosWords){TrvWordDs=PosWordDs;} else {TrvWordDs=WordDs;}
      int WordIdN=TrvWordDs->FFirstWordId(); int WordId;
      while (TrvWordDs->FNextWordId(WordIdN, WordId)){
        if (SelWordIdH.IsKey(WordId)){continue;}
        double WordEst;
        if (AttrEst.Empty()){
          // Shortcut: Odds-Ratio
          double PriorSumW=YBs->GetDocs();
//          double PriorSumW=PosWordDs->GetDocs()+NegWordDs->GetDocs();
          IAssert(PriorSumW>0);
          double S1C0Prb=NegWordDs->GetWordPrb(WordId);
          double S1C1Prb=PosWordDs->GetWordPrb(WordId);

          if (S1C0Prb==0){S1C0Prb=1/sqr(PriorSumW);}
          if (S1C0Prb==1){S1C0Prb=1-(1/sqr(PriorSumW));}
          double OddsS1C0=S1C0Prb/(1-S1C0Prb);

          if (S1C1Prb==0){S1C1Prb=1/sqr(PriorSumW);}
          if (S1C1Prb==1){S1C1Prb=1-(1/sqr(PriorSumW));}
          double OddsS1C1=S1C1Prb/(1-S1C1Prb);

          WordEst=log(OddsS1C1/OddsS1C0);
        } else {
          WordEst=AttrEst->GetAttrQ(WordId, BoolValSplit, DmDs, PriorDmDs);
        }
        WordEstIdKdV.AddSorted(TFltIntKd(WordEst, WordId), false, SelWordIds);
        SelWordIdH.AddKey(WordId);
      }
    }
    TIntFltKdV& WordIdEstKdV=DocIdToWordIdEstVV[DocId];
    WordIdEstKdV.Gen(WordEstIdKdV.Len(), 0);
    for (int WordIdN=0; WordIdN<WordEstIdKdV.Len(); WordIdN++){
      double WordEst=WordEstIdKdV[WordIdN].Key;
      int WordId=WordEstIdKdV[WordIdN].Dat;
      WordIdEstKdV.Add(TIntFltKd(WordId, WordEst));
    }
    WordIdEstKdV.Sort();
    if (DocIds%100==0){
      TNotify::OnNotify(Notify, ntInfo,
       TStr("...")+TInt::GetStr(DocIds)+" Selections.");}
  }
  TNotify::OnNotify(Notify, ntInfo,
   TStr("Feature Selection Finished (")+ TInt::GetStr(DocIds)+").");
}

void TYFSelBs::GetBestWordIdV(
 const int& DocId, const double& EstExp, const double& SumEstPrb,
 const PYWordDs& IntrsWordDs, TIntV& BestWordIdV){
  TIntFltKdV& WordIdEstKdV=DocIdToWordIdEstVV[DocId];
  TFltIntKdV WordEstIdKdV(WordIdEstKdV.Len(), 0);
  double MnWordEst=TFlt::Mx;
  for (int WordIdN=0; WordIdN<WordIdEstKdV.Len(); WordIdN++){
    int WordId=WordIdEstKdV[WordIdN].Key;
    double WordEst=pow(WordIdEstKdV[WordIdN].Dat, EstExp);
    if (IntrsWordDs->IsWordId(WordId)){
      WordEstIdKdV.Add(TFltIntKd(WordEst, WordId));
      MnWordEst=TFlt::GetMn(WordEst, MnWordEst);
    }
  }
  double SumWordEst=0;
  {for (int WordIdN=0; WordIdN<WordEstIdKdV.Len(); WordIdN++){
    SumWordEst+=(WordEstIdKdV[WordIdN].Key-=MnWordEst);}}
  WordEstIdKdV.Sort(false);

  {BestWordIdV.Gen(WordEstIdKdV.Len(), 0);
  SumWordEst*=SumEstPrb; int WordIdN=0;
  while ((SumWordEst>=0)&&(WordIdN<WordEstIdKdV.Len())){
    double WordEst=WordEstIdKdV[WordIdN].Key;
    int WordId=WordEstIdKdV[WordIdN].Dat;
    SumWordEst-=WordEst;
    BestWordIdV.Add(WordId);
    WordIdN++;
  }}
}

void TYFSelBs::SaveTxt(
 const PSOut& SOut, const PYBs& YBs, const PYDsBs& YDsBs){
  PYWordDs NegWordDs=TYDmDs::GetNegWordDs(YNegDsType, YBs, YDsBs);
  TOLx Lx(SOut, TFSet()|oloFrcEoln|oloSigNum);
  for (int DocId=0; DocId<DocIdToWordIdEstVV.Len(); DocId++){
    TIntFltKdV& WordIdEstKdV=DocIdToWordIdEstVV[DocId];
    TFltIntKdV WordEstIdKdV(WordIdEstKdV.Len(), 0);
    for (int WordIdN=0; WordIdN<WordIdEstKdV.Len(); WordIdN++){
      int WordId=WordIdEstKdV[WordIdN].Key;
      double WordEst=WordIdEstKdV[WordIdN].Dat;
      WordEstIdKdV.Add(TFltIntKd(WordEst, WordId));
    }
    WordEstIdKdV.Sort(false);

    Lx.PutVarStr("UrlStr", YBs->GetDocUrlStr(DocId));
    Lx.PutVarInt("DocId", DocId);
    PYWordDs WordDs=YDsBs->GetWordDs(DocId);
    Lx.PutVar("WordIdEstKdV", true, true); 
    {for (int WordIdN=0; WordIdN<WordEstIdKdV.Len(); WordIdN++){
      double WordEst=WordEstIdKdV[WordIdN].Key;
      int WordId=WordEstIdKdV[WordIdN].Dat;
      TStr WordStr=YBs->GetWordStr(WordId);
      double PosWordPrb=WordDs->GetWordPrb(WordId);
      double NegWordPrb=NegWordDs->GetWordPrb(WordId);
      Lx.PutQStr(WordStr);
      Lx.PutTab(); Lx.PutFlt(WordEst);
      Lx.PutIndent(1); Lx.PutSym(syLBracket);
      Lx.PutFlt(PosWordPrb); Lx.PutFlt(NegWordPrb);
      Lx.PutSym(syRBracket);
      Lx.PutLn();
    }}
    Lx.PutSym(syRBracket); Lx.PutLn();
  }
}

TStr TYFSelBs::GetYFSelTypeStr(const TYFSelType& YFSelType){
  switch (YFSelType){
    case yfstFix: return "Fix";
    case yfstPosPrc: return "PosPrc";
    case yfstUnionPrc: return "NegPrc";
    default: Fail; return TStr();
  }
}

/////////////////////////////////////////////////
// Yahoo-Inverted-Index
TYInvIx::TYInvIx(
 const double& EstExp, const double& SumEstPrb,
 const PYBs& YBs, const PYDsBs& YDsBs,
 const PYFSelBs& YFSelBs, const PNotify& Notify):
  WordIdToFirstDocIdNH(YBs->GetWords()/2),
  DocIdVHeap(), AllDocIdV(YBs->GetDocs(), 0){
  TNotify::OnNotify(Notify, ntInfo, "Start Creating Inverted Index");
  TIntPrV WordIdDocIdPrV(YBs->GetDocs(), 0);
  TIntV BestWordIdV;
  int DocId=YBs->FFirstDocId();
  while (YBs->FNextDocId(DocId)){
    AllDocIdV.Add(DocId);
    PYWordDs PosWordDs=YDsBs->GetWordDs(DocId);
    YFSelBs->GetBestWordIdV(DocId, EstExp, SumEstPrb, PosWordDs, BestWordIdV);
    for (int WordIdN=0; WordIdN<BestWordIdV.Len(); WordIdN++){
      int WordId=BestWordIdV[WordIdN];
      WordIdDocIdPrV.Add(TIntPr(WordId, DocId));
    }
  }
  WordIdDocIdPrV.Sort();
  DocIdVHeap.Gen(WordIdDocIdPrV.Len()+YBs->GetDocs(), 0);
  int PrevWordId=-1;
  for (int WordIdN=0; WordIdN<WordIdDocIdPrV.Len(); WordIdN++){
    int WordId=WordIdDocIdPrV[WordIdN].Val1;
    int DocId=WordIdDocIdPrV[WordIdN].Val2;
    if (PrevWordId!=WordId){
      if (PrevWordId!=-1){DocIdVHeap.Add(TInt(-1));}
      PrevWordId=WordId;
      WordIdToFirstDocIdNH.AddDat(TInt(WordId), TInt(DocIdVHeap.Len()));
    }
    DocIdVHeap.Add(DocId);
  }
  DocIdVHeap.Add(TInt(-1));
  TNotify::OnNotify(Notify, ntInfo, "End Creating Inverted Index");
}

void TYInvIx::GetDocIdV(
 const PYWordDs& WordDs, const int& MnDocFq, TIntV& DocIdV){
  IAssert(MnDocFq>=0);
  if (MnDocFq==0){
    DocIdV=AllDocIdV;
  } else {
    TIntIntH DocIdFqH(100); int MxDocFq=0;
    int WordIdN=WordDs->FFirstWordId(); int WordId; double WordFq;
    while (WordDs->FNextWordId(WordIdN, WordId, WordFq)){
      if (WordIdToFirstDocIdNH.IsKey(WordId)){
        int DocIdN=FFirstDocId(WordId); int DocId;
        while (FNextWordId(DocIdN, DocId)){
          DocIdFqH.AddDat(DocId)+=int(WordFq);
          MxDocFq=TInt::GetMx(MxDocFq, DocIdFqH.GetDat(DocId));
        }
      }
    }
    int NewMnDocFq=(MnDocFq<=MxDocFq) ? MnDocFq : MxDocFq-3;
    DocIdV.Gen(DocIdFqH.Len(), 0);
    int DocIdP=DocIdFqH.FFirstKeyId();
    while (DocIdFqH.FNextKeyId(DocIdP)){
      int DocId=DocIdFqH.GetKey(DocIdP);
      int DocFq=DocIdFqH[DocIdP];
      if (DocFq>=NewMnDocFq){DocIdV.Add(DocId);}
    }
  }
}

void TYInvIx::SaveTxt(const PSOut& SOut, const PYBs& YBs){
  TOLx Lx(SOut, TFSet()|oloFrcEoln|oloSigNum);
  int WordIdToFirstDocIdNP=WordIdToFirstDocIdNH.FFirstKeyId();
  while (WordIdToFirstDocIdNH.FNextKeyId(WordIdToFirstDocIdNP)){
    int WordId=WordIdToFirstDocIdNH.GetKey(WordIdToFirstDocIdNP);
    TStr WordStr;
    if (YBs.Empty()){WordStr=TInt::GetStr(WordId);}
    else {WordStr=YBs->GetWordStr(WordId);}
    Lx.PutStr(WordStr); Lx.PutSym(syColon); Lx.PutSym(syLBracket);
    int DocIdN=FFirstDocId(WordId); int DocId;
    while (FNextWordId(DocIdN, DocId)){
      Lx.PutInt(DocId);}
    Lx.PutSym(syRBracket); Lx.PutLn();
  }
}

/////////////////////////////////////////////////
// Yahoo-Value-Retriever
bool TYValRet::FNextAttrN(int& AttrP, int& AttrN, TTbVal& AttrVal) const {
  bool IsFNext; double WordFq; double WordPrb;
  do {
    IsFNext=WordDs->FNextWordId(AttrP, AttrN, WordFq, WordPrb);
    if ((IsFNext)&&(WordPrb>MnWordPrb)){AttrVal=TTbVal(WordFq); return true;}
  } while (IsFNext);
  return false;
}

/////////////////////////////////////////////////
// Model-Yahoo-Bayes
TMdYBayes::TMdYBayes(
 const TYNegDsType& _YNegDsType, const TYPriorType& _YPriorType,
 const PYBs& _YBs, const PYDsBs& _YDsBs,
 const PYFSelBs& _YFSelBs, const PYInvIx& _YInvIx):
  TMd(PDmHd(new TYDmHd(_YBs, _YDsBs))),
  YNegDsType(_YNegDsType), YPriorType(_YPriorType),
  YBs(_YBs), YDsBs(_YDsBs),
  YFSelBs(_YFSelBs), YInvIx(_YInvIx),
  NegWordDs(TYDmDs::GetNegWordDs(YNegDsType, YBs, YDsBs)){
  Def();
}

PMd TMdYBayes::Load(TSIn& SIn){
  TStr TypeNm(SIn); 
  IAssert(TypeNm==TTypeNm<TMdYBayes>());
  TYNegDsType YNegDsType=TYNegDsType(int(TInt(SIn)));
  TYPriorType YPriorType=TYPriorType(int(TInt(SIn)));
  PYBs YBs(SIn);
  PYDsBs YDsBs(SIn);
  PYFSelBs YFSelBs(SIn);
  PYInvIx YInvIx(SIn);
  PYWordDs NegWordDs(SIn); SIn.LoadCs();
  PMd Md=PMd(new TMdYBayes(
   YNegDsType, YPriorType, YBs, YDsBs, YFSelBs, YInvIx));
  return Md;
}

void TMdYBayes::Save(TSOut& SOut){
  GetTypeNm(*this).Save(SOut);
  YNegDsType.Save(SOut); YPriorType.Save(SOut);
  YBs.Save(SOut);
  YDsBs.Save(SOut);
  YFSelBs.Save(SOut);
  YInvIx.Save(SOut);
  NegWordDs.Save(SOut);
  SOut.SaveCs();
}

PTbValDs TMdYBayes::GetPostrValDs(
 const PValRet& ValRet, const int& ClassN) const {
  PYWordDs PosWordDs=YDsBs->GetWordDs(ClassN);
  PTbValDs PriorValDs=GetPriorValDs(ClassN);
  TIntFltKdV& WordIdEstKdV=YFSelBs->GetWordIdEstV(ClassN);

  double LnSumW=log(PriorValDs->GetSumW());
  PTbValDs ValDs=new TTbValDs(TTbVal::BoolVals);

  double NegPriorCPrb=PriorValDs->GetPrb_RelFq(TTbVal::NegVal);
  double PosPriorCPrb=PriorValDs->GetPrb_RelFq(TTbVal::PosVal);

  double NegLnPriorCPrb=0; double NegLnPostrCPrb=0;
  double PosLnPriorCPrb=0; double PosLnPostrCPrb=0;
  if (NegPriorCPrb!=0){NegLnPostrCPrb=NegLnPriorCPrb=log(NegPriorCPrb);}
  if (PosPriorCPrb!=0){PosLnPostrCPrb=PosLnPriorCPrb=log(PosPriorCPrb);}

  int AttrP=ValRet->FFirstAttrN(); int AttrN; TTbVal AVal;
  while (ValRet->FNextAttrN(AttrP, AttrN, AVal)){
    int WordId=AttrN; double WordFq=AVal.GetFlt();
    if (YFSelBs->IsWordId(WordIdEstKdV, WordId)){
      if (NegPriorCPrb!=0){
        double NegAValPrb=NegWordDs->GetWordPrb(WordId);
        if (NegAValPrb==0){NegLnPostrCPrb+=NegLnPriorCPrb-LnSumW;}
        else {NegLnPostrCPrb+=log(WordFq*NegAValPrb);}
      }
      if (PosPriorCPrb!=0){
        double PosAValPrb=PosWordDs->GetWordPrb(WordId);
        if (PosAValPrb==0){PosLnPostrCPrb+=PosLnPriorCPrb-LnSumW;}
        else {PosLnPostrCPrb+=log(WordFq*PosAValPrb);}
      }
    }
  }
  if (NegPriorCPrb!=0){ValDs->AddVal(TTbVal::NegVal, NegLnPostrCPrb);}
  if (PosPriorCPrb!=0){ValDs->AddVal(TTbVal::PosVal, PosLnPostrCPrb);}
  ValDs->ExpW(); ValDs->Def();
  return ValDs;
}

PTbValDs TMdYBayes::GetPostrValDs(
 const PValRet& ValRet, const int& ClassN, TFltIntKdV& WordPrbIdV) const {
  // same as GetPostrValDs/2 except WordPrbIdV
  WordPrbIdV.Clr();
  PYWordDs PosWordDs=YDsBs->GetWordDs(ClassN);
  PTbValDs PriorValDs=GetPriorValDs(ClassN);
  TIntFltKdV& WordIdEstKdV=YFSelBs->GetWordIdEstV(ClassN);

  double LnSumW=log(PriorValDs->GetSumW());
  PTbValDs ValDs=new TTbValDs(TTbVal::BoolVals);

  double NegPriorCPrb=PriorValDs->GetPrb_RelFq(TTbVal::NegVal);
  double PosPriorCPrb=PriorValDs->GetPrb_RelFq(TTbVal::PosVal);

  double NegLnPriorCPrb=0; double NegLnPostrCPrb=0;
  double PosLnPriorCPrb=0; double PosLnPostrCPrb=0;
  if (NegPriorCPrb!=0){NegLnPostrCPrb=NegLnPriorCPrb=log(NegPriorCPrb);}
  if (PosPriorCPrb!=0){PosLnPostrCPrb=PosLnPriorCPrb=log(PosPriorCPrb);}

  int AttrP=ValRet->FFirstAttrN(); int AttrN; TTbVal AVal;
  while (ValRet->FNextAttrN(AttrP, AttrN, AVal)){
    int WordId=AttrN; double WordFq=AVal.GetFlt();
    if (YFSelBs->IsWordId(WordIdEstKdV, WordId)){
      if (NegPriorCPrb!=0){
        double NegAValPrb=NegWordDs->GetWordPrb(WordId);
        if (NegAValPrb==0){NegLnPostrCPrb+=NegLnPriorCPrb-LnSumW;}
        else {NegLnPostrCPrb+=log(WordFq*NegAValPrb);}
        if (NegAValPrb!=0){
          WordPrbIdV.Add(TFltIntKd(-/*WordFq**/NegAValPrb, WordId));}
      }
      if (PosPriorCPrb!=0){
        double PosAValPrb=PosWordDs->GetWordPrb(WordId);
        if (PosAValPrb==0){PosLnPostrCPrb+=PosLnPriorCPrb-LnSumW;}
        else {PosLnPostrCPrb+=log(WordFq*PosAValPrb);}
        if (PosAValPrb!=0){
          WordPrbIdV.Add(TFltIntKd(/*WordFq**/PosAValPrb, WordId));}
      }
    }
  }
  if (NegPriorCPrb!=0){ValDs->AddVal(TTbVal::NegVal, NegLnPostrCPrb);}
  if (PosPriorCPrb!=0){ValDs->AddVal(TTbVal::PosVal, PosLnPostrCPrb);}
  ValDs->ExpW(); ValDs->Def();
  WordPrbIdV.Sort(false);
  return ValDs;
}

