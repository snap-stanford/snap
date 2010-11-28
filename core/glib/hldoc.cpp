/////////////////////////////////////////////////
// Includes
#include "hldoc.h"
#include "yahooex.h"

/////////////////////////////////////////////////
// HyperLinkDocument-Word-Distribution
void THldWordDs::AddWordIdFq(const TIntFltKd& WordIdFqKd){
  double WordFq=WordIdFqKd.Dat;
  if (WordFq>0){
    int WordIdN=WordIdFqKdV.SearchBin(WordIdFqKd);
    SumWordFq+=WordFq;
    if (WordIdN==-1){
      WordIdFqKdV.AddSorted(WordIdFqKd);
    } else {
      WordIdFqKdV[WordIdN].Dat+=WordFq;
    }
  }
}

bool THldWordDs::FNextWordId(
 int& WordIdN, int& WordId, double& WordFq, double& WordPrb) const {
  WordIdN++;
  if (WordIdN<WordIdFqKdV.Len()){
    WordId=WordIdFqKdV[WordIdN].Key;
    WordFq=WordIdFqKdV[WordIdN].Dat;
    WordPrb=WordFq/SumWordFq;
    return true;
  } else {
    return false;
  }
}

void THldWordDs::NrToSumWordFq(const double& NrSumWordFq){
  if (SumWordFq!=NrSumWordFq){
    double NrRatio=(double(SumWordFq)==0) ? 0 : NrSumWordFq/SumWordFq;
    SumWordFq=NrSumWordFq;
    if (SumWordFq>0){
      for (int WordIdN=0; WordIdN<WordIdFqKdV.Len(); WordIdN++){
        WordIdFqKdV[WordIdN].Dat=WordIdFqKdV[WordIdN].Dat*NrRatio;}
    }
  }
}

void THldWordDs::PowWordFq(const double& Exp){
  if (SumWordFq>0){
    SumWordFq=0;
    for (int WordIdN=0; WordIdN<WordIdFqKdV.Len(); WordIdN++){
      SumWordFq+=WordIdFqKdV[WordIdN].Dat=pow(WordIdFqKdV[WordIdN].Dat, Exp);
    }
  }
}

void THldWordDs::SaveTxt(const PSOut& SOut, const PHldBs& HldBs){
  TFltIntKdV WordFqIdKdV(WordIdFqKdV.Len(), 0);
  for (int WordIdN=0; WordIdN<WordIdFqKdV.Len(); WordIdN++){
    int WordId=WordIdFqKdV[WordIdN].Key;
    double WordFq=WordIdFqKdV[WordIdN].Dat/SumWordFq;
    WordFqIdKdV.Add(TFltIntKd(WordFq, WordId));
  }
  WordFqIdKdV.Sort(false);
  IAssert(WordFqIdKdV.IsSorted(false));

  TOLx Lx(SOut, TFSet()|oloFrcEoln|oloSigNum);
  Lx.PutVarFlt("SumWordFq", SumWordFq);
  Lx.PutVar("WordIdFqKdV", true, true);
  {for (int WordIdN=0; WordIdN<WordFqIdKdV.Len(); WordIdN++){
    int WordId=WordFqIdKdV[WordIdN].Dat;
    double WordFq=WordFqIdKdV[WordIdN].Key;
    if (HldBs.Empty()){Lx.PutInt(WordId);}
    else {Lx.PutQStr(HldBs->GetWordStr(WordId));}
    Lx.PutFlt(WordFq); Lx.PutLn();
  }}
  Lx.PutSym(syRBracket); Lx.PutLn();
}

PHldWordDs THldWordDs::GetMerged(
 const PHldWordDs& LWordDs, const PHldWordDs& RWordDs){
  TIntFltKdV& LWordIdFqKdV=LWordDs->WordIdFqKdV;
  TIntFltKdV& RWordIdFqKdV=RWordDs->WordIdFqKdV;
  int LWordIds=LWordDs->GetWordIds();
  int RWordIds=RWordDs->GetWordIds();
  double LSumWordFq=LWordDs->GetSumWordFq();
  double RSumWordFq=RWordDs->GetSumWordFq();
  PHldWordDs WordDs=new THldWordDs(LWordIds+RWordIds);
  int LWordIdN=0; int RWordIdN=0;
  if ((LSumWordFq>0)&&(RSumWordFq>0)){
    while ((LWordIdN<LWordIds)&&(RWordIdN<RWordIds)){
      TIntFltKd& LWordIdFqKd=LWordIdFqKdV[LWordIdN];
      TIntFltKd& RWordIdFqKd=RWordIdFqKdV[RWordIdN];
      if (LWordIdFqKd==RWordIdFqKd){
        double LFq=LWordIdFqKd.Dat;
        double RFq=RWordIdFqKd.Dat;
        WordDs->AddWordIdFq(LWordIdFqKd.Key, LFq+RFq); LWordIdN++; RWordIdN++;
      } else
      if (LWordIdFqKd<RWordIdFqKd){
        double LFq=LWordIdFqKd.Dat;
        WordDs->AddWordIdFq(LWordIdFqKd.Key, LFq); LWordIdN++;
      } else {
        double RFq=RWordIdFqKd.Dat;
        WordDs->AddWordIdFq(RWordIdFqKd.Key, RFq); RWordIdN++;
      }
    }
  }
  if (LSumWordFq>0){
    while (LWordIdN<LWordIds){
      TIntFltKd& LWordIdFqKd=LWordIdFqKdV[LWordIdN];
      double LFq=LWordIdFqKd.Dat;
      WordDs->AddWordIdFq(LWordIdFqKd.Key, LFq);
      LWordIdN++;
    }
  }
  if (RSumWordFq>0){
    while (RWordIdN<RWordIds){
      TIntFltKd& RWordIdFqKd=RWordIdFqKdV[RWordIdN];
      double RFq=RWordIdFqKd.Dat;
      WordDs->AddWordIdFq(RWordIdFqKd.Key, RFq);
      RWordIdN++;
    }
  }
  return WordDs;
}

PHldWordDs THldWordDs::GetTruncByMxWords(
 const PHldWordDs& WordDs, const int& MxWords){
  // prepare vector sorted by word frequency
  TFltIntKdV WordFqIdKdV(WordDs->WordIdFqKdV.Len(), 0);
  for (int WordIdN=0; WordIdN<WordDs->WordIdFqKdV.Len(); WordIdN++){
    int WordId=WordDs->WordIdFqKdV[WordIdN].Key;
    double WordFq=WordDs->WordIdFqKdV[WordIdN].Dat;
    WordFqIdKdV.Add(TFltIntKd(WordFq, WordId));
  }
  WordFqIdKdV.Sort(false);
  // generate truncated word distribution
  int TruncWordIds=TInt::GetMn(MxWords, WordDs->GetWordIds());
  PHldWordDs TruncWordDs=PHldWordDs(new THldWordDs(TruncWordIds));
  for (int WordIdN=0; WordIdN<TruncWordIds; WordIdN++){
    double WordFq=WordFqIdKdV[WordIdN].Key;
    int WordId=WordFqIdKdV[WordIdN].Dat;
    TruncWordDs->AddWordIdFq(WordId, WordFq);
  }
  return TruncWordDs;
}

double THldWordDs::GetDist(
 const PHldWordDs& LWordDs, const PHldWordDs& RWordDs,
 const PHldBs& /*HldBs*/){
//  return TFlt::GetRnd();
  TIntFltKdV& LWordIdFqKdV=LWordDs->WordIdFqKdV;
  TIntFltKdV& RWordIdFqKdV=RWordDs->WordIdFqKdV;
  int LWordIds=LWordIdFqKdV.Len();
  int RWordIds=RWordIdFqKdV.Len();
  double Dist=0;
  int LWordIdN=0; int RWordIdN=0;
  while ((LWordIdN<LWordIds)&&(RWordIdN<RWordIds)){
    TIntFltKd& LWordIdFqKd=LWordIdFqKdV[LWordIdN];
    TIntFltKd& RWordIdFqKd=RWordIdFqKdV[RWordIdN];
    if (LWordIdFqKd==RWordIdFqKd){
//      int WordId=LWordIdFqKd.Key;
//      TStr WordStr=HldBs->GetWordStr(WordId);
//      Dist-=1.0/HldBs->GetWordFq(WordId);
      Dist++;
      LWordIdN++; RWordIdN++;
    } else
    if (LWordIdFqKd<RWordIdFqKd){
      LWordIdN++;
    } else {
      RWordIdN++;
    }
  }
  Dist=1-Dist/(sqrt(LWordIdFqKdV.Len())*sqrt(RWordIdFqKdV.Len()));
  return Dist;
}

void THldWordDs::GetHypTsEst(
 const PHldWordDs& RefWordDs, const PHldWordDs& HypWordDs,
 double& Precis, double& Recall, double& Dist, double& F2){
  int RefWordIds=0; int IntsWordIds=0;
  int WordIdN=RefWordDs->FFirstWordId(); int WordId; double WordFq; double WordPrb;
  while (RefWordDs->FNextWordId(WordIdN, WordId, WordFq, WordPrb)){
    if (HypWordDs->IsWordId(WordId)){IntsWordIds++;}
    RefWordIds++;
  }
  int HypWordIds=HypWordDs->GetWordIds();
  PHldWordDs RefHypWordDs=THldWordDs::GetMerged(RefWordDs, HypWordDs);
//  int RefHypWordIds=RefHypWordDs->GetWordIds();
//  IAssert((RefWordIds>0)&&(HypWordIds>0)&&(RefHypWordIds>0));
  if ((RefWordIds==0)||(HypWordIds==0)){
    Precis=1; Recall=1; Dist=1;
  } else {
    Precis=IntsWordIds/double(HypWordIds);
    Recall=IntsWordIds/double(RefWordIds);
    Dist=1-IntsWordIds/(sqrt(HypWordIds)*sqrt(RefWordIds));
  }
  if (Precis+Recall>0){F2=(5*Precis*Recall)/(4*Precis+Recall);}
}

/////////////////////////////////////////////////
// HyperLinkDocument-Word
THldWord::THldWord(THldBs* _HldBs, const TStr& Str):
  HldBs(_HldBs), FirstChN(HldBs->WordChAHeap.Len()){
  HldBs->WordChAHeap+=Str; HldBs->WordChAHeap+=CrCh;
}

TStr THldWord::GetStr() const {
  TChA ChA; int ChN=FirstChN;
  while (HldBs->WordChAHeap[ChN]!=CrCh){
    ChA+=HldBs->WordChAHeap[ChN++];}
  return ChA;
}

/////////////////////////////////////////////////
// HyperLinkDocument-Base
void THldBs::PutThisToWord(){
  int WordToFqP=WordToFqH.FFirstKeyId();
  while (WordToFqH.FNextKeyId(WordToFqP)){
    WordToFqH.GetKey(WordToFqP).HldBs=this;
  }
}

TStr THldBs::GetStrQStr(const TStrQ& StrQ, const int& Strs){
  TChA ChA;
  int FirstStrN=(Strs==-1) ? 0 : StrQ.Len()-Strs;
  for (int StrN=FirstStrN; StrN<StrQ.Len(); StrN++){
    if (!ChA.Empty()){ChA+=' ';} ChA+=StrQ[StrN];
  }
  return ChA;
}

THldBs::THldBs(
 const TStrV& HlCtxStrV, const PXWebBs& WebBs,
 const int& _MxNGram, const int& _MnWordFq,
 const TSwSetTy& SwSetTy, const PNotify& _Notify):
  MxNGram(_MxNGram), MnWordFq(_MnWordFq),
  SwSet(new TSwSet(SwSetTy)),
  WordToFqH(100000), WordChAHeap(1000000),
  WordIdVHeap(100000, 0), DocIdToUrlStrH(50000),
  HlDocV(), Notify(_Notify){
  // generate hyperlink-document pairs
  TIntPrV HlCtxWebPgPrV(HlCtxStrV.Len(), 0);
  for (int HlCtxStrN=0; HlCtxStrN<HlCtxStrV.Len(); HlCtxStrN++){
    TStr UrlStr=GetUrlStrFromHlCtxStr(HlCtxStrV[HlCtxStrN]);
    IAssert(WebBs->IsUrlStr(UrlStr));
    int WebPgId=WebBs->GetWebPgId(UrlStr);
    HlCtxWebPgPrV.Add(TIntPr(HlCtxStrN, WebPgId));
  }

  TNotify::OnNotify(Notify, ntInfo, "Start Generation N-Grams");
  for (int NGram=1; NGram<=MxNGram; NGram++){
    // collect words and word frequencies
    int Hlds=0;
    for (int HlCtxWebPgN=0; HlCtxWebPgN<HlCtxWebPgPrV.Len(); HlCtxWebPgN++){
      int HlCtxStrN=HlCtxWebPgPrV[HlCtxWebPgN].Val1;
      int WebPgId=HlCtxWebPgPrV[HlCtxWebPgN].Val2;
      TStr HlCtxStr=HlCtxStrV[HlCtxStrN];
      TStr WebPgStr=WebBs->GetBodyStr(WebPgId);
      AddTxt(HlCtxStr, false, NGram);
      AddTxt(WebPgStr, false, NGram);
      if (++Hlds%100==0){
        TStr DocProcStr=TStr("...")+TInt::GetStr(Hlds)+" Documents; "+
         TInt::GetStr(GetWords())+" N-Grams";
        TNotify::OnNotify(Notify, ntInfo, DocProcStr);
      }
    }
    TStr NGramGenStr=TInt::GetStr(NGram)+"-Grams Generated ("+
     TInt::GetStr(GetWords())+")";
    TNotify::OnNotify(Notify, ntInfo, NGramGenStr);

    // remove words with low frequency
    TStrIntPrV WordStrFqPrV(WordToFqH.Len(), 0);
    int WordId=WordToFqH.FFirstKeyId();
    while (WordToFqH.FNextKeyId(WordId)){
      if (WordToFqH[WordId]>=MnWordFq){
        WordStrFqPrV.Add(TStrIntPr(GetWordStr(WordId), TInt(GetWordFq(WordId))));}
    }
    WordToFqH.Clr(); WordChAHeap.Clr();
    for (int WordN=0; WordN<WordStrFqPrV.Len(); WordN++){
      AddWord(WordStrFqPrV[WordN].Val1)=WordStrFqPrV[WordN].Val2;}
    TStr NGramReduceStr=TInt::GetStr(NGram)+"-Grams Reduced ("+
     TInt::GetStr(GetWords())+")";
    TNotify::OnNotify(Notify, ntInfo, NGramReduceStr);
  }
  TStr NGramGenStr=TInt::GetStr(MxNGram)+"-Grams Generation Finished ("+
   TInt::GetStr(GetWords())+")";
  TNotify::OnNotify(Notify, ntInfo, NGramGenStr);

  // generate document contents
  TNotify::OnNotify(Notify, ntInfo, "Start Processing Documents");
  HlDocV.Gen(HlCtxWebPgPrV.Len(), 0);
  int Hlds=0;
  THash<TStr, TInt> UrlStrToDocIdH(HlCtxWebPgPrV.Len());
  for (int HlCtxWebPgN=0; HlCtxWebPgN<HlCtxWebPgPrV.Len(); HlCtxWebPgN++){
    int HlCtxStrN=HlCtxWebPgPrV[HlCtxWebPgN].Val1;
    int WebPgId=HlCtxWebPgPrV[HlCtxWebPgN].Val2;
    TStr HlCtxStr=HlCtxStrV[HlCtxStrN];
//    PWebPg WebPg=WebBs->GetWebPg(WebPgId);
//    TStrV RefUrlStrV=WebPg->GetRefUrlStrV();
//    for (int RefUrlStrN=0; RefUrlStrN<RefUrlStrV.Len(); RefUrlStrN++){
//      TNotify::OnNotify(Notify, ntInfo, RefUrlStrV[RefUrlStrN]);}
    TStr WebPgStr=WebBs->GetBodyStr(WebPgId);
    TStr UrlStr=WebBs->GetUrlStr(WebPgId);
    int HlId=AddTxt(HlCtxStr, true, -1);
    int DocId;
    if (UrlStrToDocIdH.IsKey(UrlStr)){
      DocId=UrlStrToDocIdH.GetDat(UrlStr);
    } else {
      DocId=AddTxt(WebPgStr, true, -1);
      UrlStrToDocIdH.AddDat(UrlStr, DocId);
      DocIdToUrlStrH.AddDat(DocId, UrlStr);
    }
    PHldWordDs HlWordDs=GetTxtWordDs(HlId);
    HlDocV.Add(THlDoc(HlId, DocId, HlWordDs));
    if (++Hlds%100==0){
      TStr DocProcStr=TInt::GetStr(Hlds)+" Documents Processed";
      TNotify::OnNotify(Notify, ntInfo, DocProcStr);
    }
  }

  TStr DocProcStr=TStr("HyperLinkDocument Processing Finished (")+
   TInt::GetStr(HlDocV.Len())+")";
  TNotify::OnNotify(Notify, ntInfo, DocProcStr);
}

TInt& THldBs::AddWord(const TStr& WordStr){
  int PrevLen=WordChAHeap.Len();
  THldWord Word(this, WordStr);
  int WordId=WordToFqH.GetKeyId(Word);
  if (WordId==-1){
    return WordToFqH.AddDat(Word);
  } else {
    WordChAHeap.Trunc(PrevLen);
    return WordToFqH[WordId];
  }
}

int THldBs::GetWordId(const TStr& WordStr){
  int PrevLen=WordChAHeap.Len();
  THldWord Word(this, WordStr);
  int WordId=WordToFqH.GetKeyId(Word);
  WordChAHeap.Trunc(PrevLen);
  return WordId;
}

int THldBs::AddTxt(const TStr& TxtStr, const bool& DoGenDoc, const int& NGram){
  int TxtId=-1;
  TIntV TxtWordIdV;

  if (DoGenDoc){
    IAssert(NGram==-1);
    TxtWordIdV.Gen(1000, 0);
  }

  PSIn SIn=TStrIn::New(TxtStr);
  PHtmlDoc HtmlDoc=PHtmlDoc(new THtmlDoc(SIn, hdtAll));
  PHtmlTok Tok; THtmlLxSym Sym=hlsyUndef; TStr Str; TStrQ PrevStrQ;
  for (int TokN=0; TokN<HtmlDoc->GetToks(); TokN++){
    Tok=HtmlDoc->GetTok(TokN, Sym, Str);
    switch (Sym){
      case hlsyStr:
        if (DoGenDoc){
          if (!SwSet->IsIn(Str)){
            int WordId=GetWordId(Str);
            if (WordId!=-1){
              TxtWordIdV.Add(WordId);
              PrevStrQ.Push(Str);
              int NGram=2;
              while ((NGram<=MxNGram)&&(NGram<=PrevStrQ.Len())){
                int WordId=GetWordId(GetStrQStr(PrevStrQ, NGram));
                if (WordId==-1){
                  while (PrevStrQ.Len()>=NGram){PrevStrQ.Pop();} break;
                } else {
                  TxtWordIdV.Add(WordId);
                }
                NGram++;
              }
            } else {
              PrevStrQ.Clr();
            }
          }
        } else {
          if (NGram==1){
            if (!SwSet->IsIn(Str)){AddWord(Str)++;}
          } else {
            if (!SwSet->IsIn(Str)){
              int WordId=GetWordId(Str);
              if (WordId!=-1){
                TStr WordStr=GetWordStr(WordId);
                if (PrevStrQ.Len()+1==NGram){
                  if (GetWordId(GetStrQStr(PrevStrQ))!=-1){
                    PrevStrQ.Push(WordStr);
                    AddWord(GetStrQStr(PrevStrQ))++;
                    PrevStrQ.Pop();
                  } else {
                    PrevStrQ.Push(WordStr); PrevStrQ.Pop();
                  }
                } else {
                  PrevStrQ.Push(WordStr);
                }
              } else {
                PrevStrQ.Clr();
              }
            }
          }
        }
        break;
      case hlsyNum:
      case hlsySSym:
        PrevStrQ.Clr(); break;
      case hlsyBTag:
      case hlsyETag:
        if (THtmlTok::IsBreakTag(Str)){PrevStrQ.Clr();} break;
      default:;
    }
  }
  if (DoGenDoc){
    TxtId=WordIdVHeap.Len();
    WordIdVHeap.AddV(TxtWordIdV); WordIdVHeap.Add(TInt(-1));
  }
  return TxtId;
}

int THldBs::GetTxtLen(const int& TxtId){
  int WordIdN=TxtId; int TxtLen=0;
  while (WordIdVHeap[WordIdN]!=-1){WordIdN++; TxtLen++;}
  return TxtLen;
}

PHldWordDs THldBs::GetTxtWordDs(const int& TxtId){
  PHldWordDs WordDs=PHldWordDs(new THldWordDs(GetTxtLen(TxtId)));
  int WordIdN=TxtId;
  while (WordIdVHeap[WordIdN]!=-1){
    int WordId=WordIdVHeap[WordIdN++];
    WordDs->AddWordIdFq(WordId, 1);
  }
  return WordDs;
}

PHldWordDs THldBs::GetWordDs(const PSIn& SIn){
  PHldWordDs WordDs=PHldWordDs(new THldWordDs());
  PHtmlDoc HtmlDoc=new THtmlDoc(SIn, hdtAll);
  PHtmlTok Tok; THtmlLxSym Sym; TStr Str; TStrQ PrevStrQ;
  for (int TokN=0; TokN<HtmlDoc->GetToks(); TokN++){
    Tok=HtmlDoc->GetTok(TokN, Sym, Str);
    switch (Sym){
      case hlsyStr:
        if (!SwSet->IsIn(Str)){
          int WordId=GetWordId(Str);
          if (WordId!=-1){
            WordDs->AddWordIdFq(WordId, 1);
            PrevStrQ.Push(Str);
            int NGram=2;
            while ((NGram<=MxNGram)&&(NGram<=PrevStrQ.Len())){
              int WordId=GetWordId(GetStrQStr(PrevStrQ, NGram));
              if (WordId==-1){
                while (PrevStrQ.Len()>=NGram){PrevStrQ.Pop();} break;
              } else {
                WordDs->AddWordIdFq(WordId, 1);
              }
              NGram++;
            }
          } else {
            PrevStrQ.Clr();
          }
        }
        break;
      case hlsyNum:
      case hlsySSym:
        PrevStrQ.Clr(); break;
      case hlsyBTag:
      case hlsyETag:
        if (THtmlTok::IsBreakTag(Str)){PrevStrQ.Clr();} break;
      default:;
    }
  }
  return WordDs;
}

void THldBs::GetDistHlDocNV(
 const int& KNNbrs, const bool& RndSel,
 const PHldWordDs& RefHlWordDs, const TStrV& SelUrlStrV,
 TFltIntKdV& DistHlDocNKdV){
  DistHlDocNKdV.Clr();
  for (int HlDocN=0; HlDocN<GetHlDocs(); HlDocN++){
    if (SelUrlStrV.Len()>0){
      if (SelUrlStrV.SearchBin(GetDocUrlStr(HlDocN))==-1){continue;}}
    PHldWordDs HlWordDs=GetHlWordDs(HlDocN);
    double Dist=THldWordDs::GetDist(RefHlWordDs, HlWordDs, this);
    if (RndSel){
      DistHlDocNKdV.Add(TFltIntKd(Dist, HlDocN));
    } else {
      DistHlDocNKdV.AddSorted(TFltIntKd(Dist, HlDocN), true, KNNbrs);
    }
  }
  if (RndSel){
    TRnd Rnd(TRnd::RndSeed);
    DistHlDocNKdV.Shuffle(Rnd);
    int Docs=TInt::GetMn(DistHlDocNKdV.Len(), KNNbrs);
    DistHlDocNKdV.Reserve(Docs, Docs);
  }
}

PHldWordDs THldBs::GetHypDocWordDs(const TFltIntKdV& DistHlDocNKdV){
  PHldWordDs HypWordDs=PHldWordDs(new THldWordDs());
  PMom HypDocLenMom=PMom(new TMom());
  for (int DistHlDocNKdN=0; DistHlDocNKdN<DistHlDocNKdV.Len(); DistHlDocNKdN++){
//    double Dist=DistHlDocNKdV[DistHlDocNKdN].Key;
    int HlDocN=DistHlDocNKdV[DistHlDocNKdN].Dat;
    PHldWordDs DocWordDs=GetDocWordDs(HlDocN);
    HypWordDs=THldWordDs::GetMerged(HypWordDs, DocWordDs);
    HypDocLenMom->Add(GetTxtLen(GetDocId(HlDocN)));
  }
  HypDocLenMom->Def();
  int HypDocLen=int(HypDocLenMom->GetMean());
  HypWordDs=THldWordDs::GetTruncByMxWords(HypWordDs, HypDocLen);
  return HypWordDs;
}

void THldBs::SaveTxt(const PSOut& SOut){
  TOLx Lx(SOut, TFSet()|oloFrcEoln|oloSigNum);
  for (int HlDocN=0; HlDocN<HlDocV.Len(); HlDocN++){
    int HlId=HlDocV[HlDocN].GetHlId();
    int DocId=HlDocV[HlDocN].GetDocId();
    PHldWordDs HlWordDs=HlDocV[HlDocN].GetHlWordDs();
    TStr UrlStr=DocIdToUrlStrH.GetDat(DocId);
    Lx.PutVarStr("UrlStr", UrlStr);
    // write hyperlink
    Lx.PutVar("Hl", true, true);
    int HlWordIdN=HlId; TChA HlWordLn;
    while (WordIdVHeap[HlWordIdN]!=-1){
      TStr WordStr=GetWordStr(WordIdVHeap[HlWordIdN++]);
      if (WordStr.SearchCh(' ')==-1){HlWordLn+=WordStr;}
      else {HlWordLn+='('; HlWordLn+=WordStr; HlWordLn+=')';}
      HlWordLn+=' ';
      if (HlWordLn.Len()>50){
        Lx.PutQStr(HlWordLn); Lx.PutLn(); HlWordLn.Clr();}
    }
    if (!HlWordLn.Empty()){Lx.PutQStr(HlWordLn); Lx.PutLn();}
    Lx.PutSym(syRBracket); Lx.PutLn();
    // write document
    Lx.PutVar("Doc", true, true);
    int DocWordIdN=DocId; TChA DocWordLn;
    while (WordIdVHeap[DocWordIdN]!=-1){
      TStr WordStr=GetWordStr(WordIdVHeap[DocWordIdN++]);
      if (WordStr.SearchCh(' ')==-1){DocWordLn+=WordStr;}
      else {DocWordLn+='('; DocWordLn+=WordStr; DocWordLn+=')';}
      DocWordLn+=' ';
      if (DocWordLn.Len()>50){
        Lx.PutQStr(DocWordLn); Lx.PutLn(); DocWordLn.Clr();}
    }
    if (!DocWordLn.Empty()){Lx.PutQStr(DocWordLn); Lx.PutLn();}
    Lx.PutSym(syRBracket); Lx.PutLn();
    // write hyperlink word distribution
    HlWordDs->SaveTxt(SOut, this);
  }
  // write words
  Lx.PutVar("WordFqIdKdV", true, true);
  TIntKdV WordFqIdKdV;
  int WordToFqP=WordToFqH.FFirstKeyId();
  while (WordToFqH.FNextKeyId(WordToFqP)){
    int WordId=WordToFqP;
    int WordFq=WordToFqH[WordToFqP];
    WordFqIdKdV.Add(TIntKd(WordFq, WordId));
  }
  WordFqIdKdV.Sort(false);
  for (int WordFqIdKdN=0; WordFqIdKdN<WordFqIdKdV.Len(); WordFqIdKdN++){
    int WordFq=WordFqIdKdV[WordFqIdKdN].Key;
    TStr WordStr=GetWordStr(WordFqIdKdV[WordFqIdKdN].Dat);
    Lx.PutStr(WordStr); Lx.PutSym(syColon); Lx.PutInt(WordFq); Lx.PutLn();
  }
  Lx.PutSym(syRBracket); Lx.PutLn();
}

TStr THldBs::GetUrlStrFromHlCtxStr(const TStr& HlCtxStr){
  PSIn SIn=TStrIn::New(HlCtxStr);
  PHtmlDoc HtmlDoc=THtmlDoc::New(SIn, hdtA);
  PHtmlTok Tok=HtmlDoc->GetTok(0);
  IAssert((Tok->GetSym()==hlsyBTag)&&(Tok->GetStr()==THtmlTok::ATagNm));
  TStr HRefStr=Tok->GetArg(THtmlTok::HRefArgNm);
  PUrl Url=TUrl::New(HRefStr);
  return Url->GetUrlStr();
}

/////////////////////////////////////////////////
// HyperLinkDocument-Experiment
void THldExpr::PutMomHd(TOLx& Lx, const TStr& VarNm){
  Lx.PutUQStr(VarNm+"Mean"); Lx.PutUQStr(VarNm+"SDev");
  Lx.PutUQStr(VarNm+"SErr"); Lx.PutUQStr(VarNm+"Med");
  Lx.PutUQStr(VarNm+"Q1"); Lx.PutUQStr(VarNm+"Q3");
}

void THldExpr::PutMomVal(TOLx& Lx, const PMom& Mom){
  if (Mom->IsUsable()){
    Lx.PutFlt(Mom->GetMean()); Lx.PutFlt(Mom->GetSDev());
    Lx.PutFlt(Mom->GetSErr()); Lx.PutFlt(Mom->GetMedian());
    Lx.PutFlt(Mom->GetQuart1()); Lx.PutFlt(Mom->GetQuart3());
  } else {
    for (int MomN=0; MomN<6; MomN++){Lx.PutFlt(-1);}
  }
}

void THldExpr::Go(
 const PSOut& SOut, const bool& PutHd, const bool& SaveTmp,
 const TStr& HldDmNm, const int& TsRuns,
 const TStrV& AllHlCtxStrV, const PXWebBs& DocWebBs,
 const int& MxNGram, const int& MnWordFq, const TSwSetTy& SwSetTy,
 const int& KNNbrs, const bool& RndSel,
 const PNotify& Notify){
  // open log file
  PSOut LogSOut=new TFOut("log.t");
  TOLx LogLx(LogSOut, TFSet()|oloFrcEoln|oloSigNum);

  // write result file header
  TOLx Lx(SOut, TFSet()|oloFrcEoln|oloSigNum|oloTabSep);
  if (PutHd){
    Lx.PutUQStr("DmNm"); Lx.PutUQStr("TsRunN");
    Lx.PutUQStr("KNNbrs");
    Lx.PutUQStr("RndSel");
    PutMomHd(Lx, "HldPrecis");
    PutMomHd(Lx, "HldRecall");
    PutMomHd(Lx, "HldDist");
    PutMomHd(Lx, "HldF2");
    PutMomHd(Lx, "HlLen");
    PutMomHd(Lx, "DocLen");
    PutMomHd(Lx, "HypDocLen");
    PutMomHd(Lx, "YHldPrecis");
    PutMomHd(Lx, "YHldRecall");
    Lx.PutLn();
  }

  TRnd Rnd;
  TStrV ShHlCtxStrV=AllHlCtxStrV; ShHlCtxStrV.Shuffle(Rnd);
  for (int TsRunN=0; TsRunN<TsRuns; TsRunN++){
    // prepare training and testing example set
    TStrV TrHlCtxStrV; TStrV TsHlCtxStrV;
    for (int ShHlCtxStrN=0; ShHlCtxStrN<ShHlCtxStrV.Len(); ShHlCtxStrN++){
      if (ShHlCtxStrN%TsRuns==TsRunN){
        TsHlCtxStrV.Add(ShHlCtxStrV[ShHlCtxStrN]);
      } else {
        TrHlCtxStrV.Add(ShHlCtxStrV[ShHlCtxStrN]);
      }
    }
    // construct hld base
    PHldBs HldBs=PHldBs(new THldBs(
     TrHlCtxStrV, DocWebBs, MxNGram, MnWordFq, SwSetTy, Notify));
    if (SaveTmp){
      TFOut FOut("HldBsRef.Dat");
      HldBs->Save(FOut);
      HldBs->SaveTxt(PSOut(new TFOut("hldRef.t")));
    }
/*    PMd Md=TYExpr::GenMd(
     HldDmNm, false, // YDmNm, DoSave
     2, 4, swtEnglish523, // MxNGram, MnWordFq, SwSetTy
     ydnLnSects, 1, // YDsBsNrType, YDsBsWordFqExp
     0.001, 0.5, // YDsBsMnDocWordPrb, YDsBsSumAllWordPrb
     yfstPosPrc, 3.0, NULL, // YFSelType, YFSelsV, YFSelAttrEst
     yndtAll, yptSects, // YNegDsType, YPriorType
     1.00, 0.7, // YInvIxEstExp, YInvIxSumEstPrb
     Notify); // ShowTopHits, Notify
    TMdYBayes& YMd=*(TMdYBayes*)Md();
    PYBs YBs=YMd.GetYBs();
    PYInvIx YInvIx=YMd.GetYInvIx();
    PXWebBs YWebBs=new TWebMemBs("Yahoo"+HldDmNm);*/

    // iterate through testing set
    PMom HldPrecisMom=PMom(new TMom());
    PMom HldRecallMom=PMom(new TMom());
    PMom HldDistMom=PMom(new TMom());
    PMom HldF2Mom=PMom(new TMom());
    PMom HlLenMom=PMom(new TMom());
    PMom DocLenMom=PMom(new TMom());
    PMom HypDocLenMom=PMom(new TMom());
    PMom YHldPrecisMom=PMom(new TMom());
    PMom YHldRecallMom=PMom(new TMom());
    for (int TsHlCtxStrN=0; TsHlCtxStrN<TsHlCtxStrV.Len(); TsHlCtxStrN++){
      TStr TsHlCtxStr=TsHlCtxStrV[TsHlCtxStrN];
      LogLx.PutVarStr("TsHlCtxStr", TsHlCtxStr);

      {TStr UrlStr=THldBs::GetUrlStrFromHlCtxStr(TsHlCtxStr);
      TStr WebPgStr=DocWebBs->GetBodyStr(UrlStr);
      PHldWordDs TsHlWordDs=HldBs->GetWordDs(TsHlCtxStr);
      PHldWordDs TsDocWordDs=HldBs->GetWordDs(WebPgStr);
      TFltIntKdV DistHlDocNKdV;
      HldBs->GetDistHlDocNV(KNNbrs, RndSel, TsHlWordDs, TStrV(), DistHlDocNKdV);
      if (SaveTmp){
        for (int DistHlDocNKdN=0; DistHlDocNKdN<DistHlDocNKdV.Len(); DistHlDocNKdN++){
          double Dist=DistHlDocNKdV[DistHlDocNKdN].Key;
          TStr UrlStr=HldBs->GetDocUrlStr(DistHlDocNKdV[DistHlDocNKdN].Dat);
          LogLx.PutVarStr("UrlStr", TFlt::GetStr(Dist)+" "+UrlStr);
        }
      }

      PHldWordDs HypDocWordDs=HldBs->GetHypDocWordDs(DistHlDocNKdV);
//      HypWordDs->SaveTxt(SOut, HldBs);
      double Precis; double Recall; double Dist; double F2;
      THldWordDs::GetHypTsEst(TsDocWordDs, HypDocWordDs, Precis, Recall, Dist, F2);
      HldPrecisMom->Add(Precis); HldRecallMom->Add(Recall);
      HldDistMom->Add(Dist); HldF2Mom->Add(F2);
      double HlLen=TsHlWordDs->GetWordIds(); HlLenMom->Add(HlLen);
      double DocLen=TsDocWordDs->GetWordIds(); DocLenMom->Add(DocLen);
      double HypDocLen=HypDocWordDs->GetWordIds(); HypDocLenMom->Add(HypDocLen);
      if (SaveTmp){
        LogLx.PutVarFlt("Precis", Precis);
        LogLx.PutVarFlt("Recall", Recall);
        LogLx.PutVarFlt("Dist", Dist);
        LogLx.PutVarFlt("F2", F2);
        LogLx.PutVarFlt("HlLen", HlLen);
        LogLx.PutVarFlt("DocLen", DocLen);
        LogLx.PutVarFlt("HypDocLen", HypDocLen);
      }}

/*      PYWordDs TsYWordDs=YBs->GetWordDs(TsHlCtxStr);
      double MnTsWordPrb=0.01; int MnDocFq=3;
      PValRet ValRet=new TYValRet(MnTsWordPrb, YMd.GetDmHd(), YBs, TsYWordDs);
      TIntV ClassNV; YInvIx->GetDocIdV(TsYWordDs, MnDocFq, ClassNV);
      TFltStrKdV PrbUrlStrKdV(ClassNV.Len(), 0);
      for (int ClassNN=0; ClassNN<ClassNV.Len(); ClassNN++){
        int ClassN=ClassNV[ClassNN];
        TStr UrlStr=Md->GetDmHd()->GetClassNm(ClassN);
        PTbValDs ValDs=Md->GetPostrValDs(ValRet, ClassN);
        double ClassPrb=ValDs->GetPrb_RelFq(TTbVal(true));
        PrbUrlStrKdV.Add(TFltStrKd(ClassPrb, UrlStr));
      }
      PrbUrlStrKdV.Sort(false);

      int YUrlStrN=0; TStrV SelUrlStrV;
      while ((YUrlStrN<10)&&(YUrlStrN<PrbUrlStrKdV.Len())){
        TStr YUrlStr=PrbUrlStrKdV[YUrlStrN++].Dat;
        PWebPg YWebPg=YWebBs->GetWebPg(YUrlStr);
        TStrV YWebPgUrlStrV; TYBs::GetAbsSectUrlStrV(YWebPg, YWebPgUrlStrV);
        SelUrlStrV.AddV(YWebPgUrlStrV);
      }
      SelUrlStrV.Sort();

      {TStr UrlStr=THldBs::GetUrlStrFromHlCtxStr(TsHlCtxStr);
      TStr WebPgStr=DocWebBs->GetBodyStr(UrlStr);
      PHldWordDs TsHlWordDs=HldBs->GetWordDs(TsHlCtxStr);
      PHldWordDs TsDocWordDs=HldBs->GetWordDs(WebPgStr);
      TFltIntKdV DistHlDocNKdV;
      HldBs->GetDistHlDocNV(100*KNNbrs, TsHlWordDs, SelUrlStrV, DistHlDocNKdV);
      PHldWordDs HypDocWordDs=HldBs->GetHypDocWordDs(DistHlDocNKdV);
//      HypWordDs->SaveTxt(SOut, HldBs);
      double Acc; double Precis; double Recall;
      THldWordDs::GetHypTsEst(TsDocWordDs, HypDocWordDs, Precis, Recall, Dist);
      YHldPrecisMom->Add(Precis); YHldRecallMom->Add(Recall);}*/
      if (SaveTmp){LogLx.PutLn();}
//      break;
    }
    HldPrecisMom->Def();
    HldRecallMom->Def();
    HldDistMom->Def();
    HldF2Mom->Def();
    HlLenMom->Def();
    DocLenMom->Def();
    HypDocLenMom->Def();
    YHldPrecisMom->Def();
    YHldRecallMom->Def();

    // writing to result file
    Lx.PutStr(HldDmNm); Lx.PutInt(TsRunN);
    Lx.PutInt(KNNbrs);
    Lx.PutBool(RndSel);
    PutMomVal(Lx, HldPrecisMom);
    PutMomVal(Lx, HldRecallMom);
    PutMomVal(Lx, HldDistMom);
    PutMomVal(Lx, HldF2Mom);
    PutMomVal(Lx, HlLenMom);
    PutMomVal(Lx, DocLenMom);
    PutMomVal(Lx, HypDocLenMom);
    PutMomVal(Lx, YHldPrecisMom);
    PutMomVal(Lx, YHldRecallMom);
    Lx.PutLn();
    SOut->Flush();
    LogSOut->Flush();
//    break;
  }
}

