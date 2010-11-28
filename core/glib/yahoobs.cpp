/////////////////////////////////////////////////
// Includes
#include "yahoobs.h"

/////////////////////////////////////////////////
// Yahoo-Word
TYWord::TYWord(TYBs* _YBs, const TStr& Str):
  YBs(_YBs), FirstChN(YBs->WordChAHeap.Len()){
    YBs->WordChAHeap+=Str; YBs->WordChAHeap+=TCh::CrCh;
}

TStr TYWord::GetStr() const {
  TChA ChA; int ChN=FirstChN;
  while (YBs->WordChAHeap[ChN]!=TCh::CrCh){
    ChA+=YBs->WordChAHeap[ChN++];}
  return ChA;
}

int TYWord::GetSegs() const {
  int Segs=1; int ChN=FirstChN;
  while (YBs->WordChAHeap[ChN]!=TCh::CrCh){
    if (YBs->WordChAHeap[ChN++]==' '){Segs++;}}
  return Segs;
}

/////////////////////////////////////////////////
// Yahoo-Word-Distribution
void TYWordDs::AddWordIdFq(const TIntFltKd& WordIdFqKd){
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

bool TYWordDs::FNextWordId(
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

void TYWordDs::NrToSumWordFq(const double& NrSumWordFq){
  if (SumWordFq!=NrSumWordFq){
    double NrRatio=(double(SumWordFq)==0) ? 0 : NrSumWordFq/SumWordFq;
    SumWordFq=NrSumWordFq;
    if (SumWordFq>0){
      for (int WordIdN=0; WordIdN<WordIdFqKdV.Len(); WordIdN++){
        WordIdFqKdV[WordIdN].Dat=WordIdFqKdV[WordIdN].Dat*NrRatio;}
    }
  }
}

void TYWordDs::PowWordFq(const double& Exp){
  if (SumWordFq>0){
    SumWordFq=0;
    for (int WordIdN=0; WordIdN<WordIdFqKdV.Len(); WordIdN++){
      SumWordFq+=WordIdFqKdV[WordIdN].Dat=pow(WordIdFqKdV[WordIdN].Dat, Exp);
    }
  }
}

void TYWordDs::SaveTxt(const PSOut& SOut, const TYBs* YBs, const int& DocId){
  TFltIntKdV WordFqIdKdV(WordIdFqKdV.Len(), 0);
  for (int WordIdN=0; WordIdN<WordIdFqKdV.Len(); WordIdN++){
    int WordId=WordIdFqKdV[WordIdN].Key;
    double WordFq=WordIdFqKdV[WordIdN].Dat/SumWordFq;
    WordFqIdKdV.Add(TFltIntKd(WordFq, WordId));
  }
  WordFqIdKdV.Sort(false);
  IAssert(WordFqIdKdV.IsSorted(false));

  TOLx Lx(SOut, TFSet()|oloFrcEoln|oloSigNum);
  if (DocId!=-1){Lx.PutVarStr("UrlStr", YBs->GetDocUrlStr(DocId));}
  Lx.PutVarInt("DocId", DocId);
  Lx.PutVarInt("Docs", Docs);
  Lx.PutVarInt("Sects", Sects);
  Lx.PutVarFlt("SumWordFq", SumWordFq);
  Lx.PutVar("WordIdFqKdV", true, true);
  {for (int WordIdN=0; WordIdN<WordFqIdKdV.Len(); WordIdN++){
    int WordId=WordFqIdKdV[WordIdN].Dat;
    double WordFq=WordFqIdKdV[WordIdN].Key;
    if (YBs==NULL){Lx.PutInt(WordId);}
    else {Lx.PutQStr(YBs->GetWordStr(WordId));}
    Lx.PutFlt(WordFq); Lx.PutLn();
  }}
  Lx.PutSym(syRBracket); Lx.PutLn();
}

PYWordDs TYWordDs::GetMerged(const PYWordDs& LWordDs, const PYWordDs& RWordDs,
 const double& LWgt, const double& RWgt){
  TIntFltKdV& LWordIdFqKdV=LWordDs->WordIdFqKdV;
  TIntFltKdV& RWordIdFqKdV=RWordDs->WordIdFqKdV;
  int LWordIds=LWordDs->GetWordIds();
  int RWordIds=RWordDs->GetWordIds();
  double LSumWordFq=LWordDs->GetSumWordFq();
  double RSumWordFq=RWordDs->GetSumWordFq();
  int Docs=LWordDs->GetDocs()+RWordDs->GetDocs();
  int Sects=LWordDs->GetSects()+RWordDs->GetSects();
  int WordIds=LWordIds+RWordIds;
  PYWordDs WordDs=new TYWordDs(Docs, Sects, WordIds);
  int LWordIdN=0; int RWordIdN=0;
  if ((LSumWordFq>0)&&(RSumWordFq>0)){
    while ((LWordIdN<LWordIds)&&(RWordIdN<RWordIds)){
      TIntFltKd& LWordIdFqKd=LWordIdFqKdV[LWordIdN];
      TIntFltKd& RWordIdFqKd=RWordIdFqKdV[RWordIdN];
      if (LWordIdFqKd==RWordIdFqKd){
        double LFq=LWgt*LWordIdFqKd.Dat/LSumWordFq;
        double RFq=RWgt*RWordIdFqKd.Dat/RSumWordFq;
        WordDs->AddWordIdFq(LWordIdFqKd.Key, LFq+RFq); LWordIdN++; RWordIdN++;
      } else
      if (LWordIdFqKd<RWordIdFqKd){
        double LFq=LWgt*LWordIdFqKd.Dat/LSumWordFq;
        WordDs->AddWordIdFq(LWordIdFqKd.Key, LFq); LWordIdN++;
      } else {
        double RFq=RWgt*RWordIdFqKd.Dat/RSumWordFq;
        WordDs->AddWordIdFq(RWordIdFqKd.Key, RFq); RWordIdN++;
      }
    }
  }
  if (LSumWordFq>0){
    while (LWordIdN<LWordIds){
      TIntFltKd& LWordIdFqKd=LWordIdFqKdV[LWordIdN];
      double LFq=LWgt*LWordIdFqKd.Dat/LSumWordFq;
      WordDs->AddWordIdFq(LWordIdFqKd.Key, LFq);
      LWordIdN++;
    }
  }
  if (RSumWordFq>0){
    while (RWordIdN<RWordIds){
      TIntFltKd& RWordIdFqKd=RWordIdFqKdV[RWordIdN];
      double RFq=RWgt*RWordIdFqKd.Dat/RSumWordFq;
      WordDs->AddWordIdFq(RWordIdFqKd.Key, RFq);
      RWordIdN++;
    }
  }
  return WordDs;
}

PYWordDs TYWordDs::GetTruncByMnWordPrb(
 const PYWordDs& WordDs, const double& MnWordPrb){
  double MnWordFq=MnWordPrb*WordDs->GetSumWordFq();
  int TruncWordIds=0;
  {int WordIdN=WordDs->FFirstWordId(); int WordId; double WordFq;
  while (WordDs->FNextWordId(WordIdN, WordId, WordFq)){
    if (WordFq>=MnWordFq){TruncWordIds++;}
  }}
  PYWordDs TruncYWordDs=new
   TYWordDs(WordDs->GetDocs(), WordDs->GetSects(), TruncWordIds);
  {int WordIdN=WordDs->FFirstWordId(); int WordId; double WordFq;
  while (WordDs->FNextWordId(WordIdN, WordId, WordFq)){
    if (WordFq>=MnWordFq){TruncYWordDs->AddWordIdFq(WordId, WordFq);}
  }}
  return TruncYWordDs;
}

PYWordDs TYWordDs::GetTruncBySumWordPrb(
 const PYWordDs& WordDs, const double& SumWordPrb){
  double TruncSumWordFq=SumWordPrb*WordDs->GetSumWordFq();
  TFltIntKdV WordFqIdKdV(WordDs->GetWordIds(), 0);
  {int WordIdN=WordDs->FFirstWordId(); int WordId; double WordFq;
  while (WordDs->FNextWordId(WordIdN, WordId, WordFq)){
    WordFqIdKdV.Add(TFltIntKd(WordFq, WordId));
  }}
  WordFqIdKdV.Sort(false); double CurSumWordFq=0;
  TIntFltKdV WordIdFqKdV;
  for (int WordIdN=0; WordIdN<WordFqIdKdV.Len(); WordIdN++){
    if (CurSumWordFq>TruncSumWordFq){break;}
    double WordFq=WordFqIdKdV[WordIdN].Key;
    int WordId=WordFqIdKdV[WordIdN].Dat;
    WordIdFqKdV.Add(TIntFltKd(WordId, WordFq));
    CurSumWordFq+=WordFq;
  }
  WordIdFqKdV.Sort();
  PYWordDs TruncYWordDs=new TYWordDs(WordDs->GetDocs(), WordDs->GetSects());
  {for (int WordIdN=0; WordIdN<WordIdFqKdV.Len(); WordIdN++){
    TruncYWordDs->AddWordIdFq(WordIdFqKdV[WordIdN]);
  }}
  return TruncYWordDs;
}

/////////////////////////////////////////////////
// Yahoo-Text-Base
const TStr TYBs::ParWordStr="/";

void TYBs::PutThisToWord(){
  int WordToFqP=WordToFqH.FFirstKeyId();
  while (WordToFqH.FNextKeyId(WordToFqP)){
    WordToFqH.GetKey(WordToFqP).YBs=this;
  }
}

TStr TYBs::GetStrQStr(const TStrQ& StrQ, const int& Strs){
  TChA ChA;
  int FirstStrN=(Strs==-1) ? 0 : StrQ.Len()-Strs;
  for (int StrN=FirstStrN; StrN<StrQ.Len(); StrN++){
    if (!ChA.Empty()){ChA+=' ';} ChA+=StrQ[StrN];
  }
  return ChA;
}

TYBs::TYBs(const PXWebBs& WebBs,
 const int& _MxNGram, const int& _MnWordFq,
 const TSwSetTy& SwSetTy, const TStrV& ExclUrlStrV,
 const PNotify& _Notify):
  Notify(_Notify),
  MxNGram(_MxNGram), MnWordFq(_MnWordFq), ParWordId(-1),
  RootDocId(-1), RootUrlStr(),
  Sects(0), AbsSects(0),
  SwSet(new TSwSet(SwSetTy)), CtgStrToFqH(30000),
  WordToFqH(100000), WordChAHeap(1000000),
  UrlStrToDocH(50000),
  PathCtgIdVHeap(10000, 0), CtgIdVHeap(10000, 0),
  ReftoDocIdVHeap(10000, 0), RefbyDocIdVHeap(),
  WordIdVHeap(100000, 0){
  TIntPrV DocIdRefbyDocIdPrV;

  // prepare sorted vector of url-s for exclusion
  TStrV SortedExclUrlStrV(ExclUrlStrV);
  SortedExclUrlStrV.Sort();

  // generate document objects
  {int WebPgP=WebBs->FFirstWebPg(); int WebPgId;
  while (WebBs->FNextWebPg(WebPgP, WebPgId)){
    TStr UrlStr=WebBs->GetUrlStr(WebPgId);
    PUrl Url=PUrl(new TUrl(UrlStr));
    static TStr TreeHtmlStr="tree.html";
    if (!Url->GetPathStr().IsSuffix(TreeHtmlStr)){
      UrlStrToDocH.AddDat(UrlStr, PYDoc(new TYDoc()));
      TStr PathStr=Url->GetPathStr();
      if (Url->GetPathStr()=="/"){
        IAssert(RootDocId==-1);
        RootDocId=UrlStrToDocH.GetKeyId(UrlStr);
        RootUrlStr=UrlStr;
      }
      for (int PathSegN=0; PathSegN<Url->GetPathSegs(); PathSegN++){
        TStr PathSegStr=Url->GetPathSeg(PathSegN);
        if (!PathSegStr.Empty()){CtgStrToFqH.AddDat(PathSegStr);}
      }
    }
  }}
  IAssert(RootDocId!=-1);
  TStr DocObjGenStr=
   TStr("Document Objects Generated (")+TInt::GetStr(UrlStrToDocH.Len())+")";
  TNotify::OnNotify(Notify, ntInfo, DocObjGenStr);

  TNotify::OnNotify(Notify, ntInfo, "Start Generation N-Grams");
  for (int NGram=1; NGram<=MxNGram; NGram++){
    // collect words and word frequencies
    {int WebPgP=WebBs->FFirstWebPg(); int WebPgId; int Docs=0;
    while (WebBs->FNextWebPg(WebPgP, WebPgId)){
      PXWebPg WebPg=WebBs->GetWebPg(WebPgId);
      if (UrlStrToDocH.IsKey(WebPg->GetUrlStr())){
        AddDoc(WebPg, false, NGram, SortedExclUrlStrV, DocIdRefbyDocIdPrV);}
      if (++Docs%100==0){
        TStr DocProcStr=
         TStr("...")+TInt::GetStr(Docs)+" Documents; "+
         TInt::GetStr(GetWords())+" N-Grams";
        TNotify::OnNotify(Notify, ntInfo, DocProcStr);
      }
    }}
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

  // generate special words
  AddWord(ParWordStr)=0; ParWordId=GetWordId(ParWordStr);

  // generate document contents
  TNotify::OnNotify(Notify, ntInfo, "Start Processing Documents");
  {int WebPgP=WebBs->FFirstWebPg(); int WebPgId; int Docs=0;
  while (WebBs->FNextWebPg(WebPgP, WebPgId)){
    PXWebPg WebPg=WebBs->GetWebPg(WebPgId);
    if (UrlStrToDocH.IsKey(WebPg->GetUrlStr())){
      AddDoc(WebPg, true, -1, SortedExclUrlStrV, DocIdRefbyDocIdPrV);}
    if (++Docs%100==0){
      TStr DocProcStr=TInt::GetStr(Docs)+" Documents Processed";
      TNotify::OnNotify(Notify, ntInfo, DocProcStr);
    }
  }}

  // generate refby references
  DocIdRefbyDocIdPrV.Sort();
  RefbyDocIdVHeap.Gen(DocIdRefbyDocIdPrV.Len()+UrlStrToDocH.Len(), 0);
  int PrevDocId=-1;
  for (int DocIdN=0; DocIdN<DocIdRefbyDocIdPrV.Len(); DocIdN++){
    int DocId=DocIdRefbyDocIdPrV[DocIdN].Val1;
    int RefbyDocId=DocIdRefbyDocIdPrV[DocIdN].Val2;
    if (PrevDocId!=DocId){
      if (PrevDocId!=-1){RefbyDocIdVHeap.Add(TInt(-1));}
      UrlStrToDocH[DocId]->FirstRefbyDocIdN=RefbyDocIdVHeap.Len();
      PrevDocId=DocId;
    }
    RefbyDocIdVHeap.Add(RefbyDocId);
  }
  RefbyDocIdVHeap.Add(TInt(-1));

  // check refby references
  {int DocId=FFirstDocId();
  while (FNextDocId(DocId)){
    if ((GetDocRefbys(DocId)==0)&&(DocId!=GetRootDocId())){
      TStr MsgStr=TStr("Unreferenced page: ")+GetDocUrlStr(DocId);
      TNotify::OnNotify(Notify, ntWarn, MsgStr);
    }
  }}

  // generate document categories
  {int DocId=FFirstDocId();
  while (FNextDocId(DocId)){
    PYDoc Doc=GetDoc(DocId);
    TIntV CtgIdV;
    TIntH DocIdH(20);
    TIntQ DocIdQ; DocIdQ.Push(DocId);
    while (!DocIdQ.Empty()){
      int DocId=DocIdQ.Top(); DocIdQ.Pop();
      if (!DocIdH.IsKey(DocId)){
        DocIdH.AddKey(DocId);
        int RefbyDocIdN=FFirstDocRefbyDocId(DocId); int RefbyDocId;
        while (FNextDocRefbyDocId(RefbyDocIdN, RefbyDocId)){
          DocIdQ.Push(RefbyDocId);}

        int CtgIdN=FFirstDocPathCtgId(DocId); int CtgId;
        while (FNextDocPathCtgId(CtgIdN, CtgId)){
          if (CtgIdV.SearchBin(CtgId)==-1){CtgIdV.AddSorted(CtgId);}}
      }
    }
    if (!CtgIdV.Empty()){
      Doc->FirstCtgIdN=CtgIdVHeap.Len();
      CtgIdVHeap.AddV(CtgIdV); CtgIdVHeap.Add(TInt(-1));
    }
  }}

  TStr DocProcStr=TStr("Documents Processing Finished (")+
   TInt::GetStr(UrlStrToDocH.Len())+")";
  TNotify::OnNotify(Notify, ntInfo, DocProcStr);
}

void TYBs::AddCtgStrWords(const TStr& CtgStr, TIntV& DocWordIdV){
  if (!CtgStr.Empty()){
    TStr UcCtgStr=CtgStr.GetUc();
    if (!UcCtgStr.IsChIn('_')){
      TStr WordStr=UcCtgStr;
      if ((WordStr.IsWord())&&(!SwSet->IsIn(WordStr))){
        AddWord(WordStr)++; DocWordIdV.Add(TInt(GetWordId(WordStr)));
      }
    } else {
      TChA WordChA; TStrV WordStrV; int ChN=0;
      while (ChN<=UcCtgStr.Len()){
        if ((ChN==UcCtgStr.Len())||(UcCtgStr[ChN]=='_')){
          if (!WordChA.Empty()){
            TStr WordStr=WordChA; WordChA.Clr();
            if ((WordStr.IsWord())&&(!SwSet->IsIn(WordStr))){
              WordStrV.Add(WordStr);}
          }
          if ((ChN+1<UcCtgStr.Len())&&(UcCtgStr[ChN+1]=='_')){
            WordStrV.Add(TStr()); ChN++;}
        } else {
          WordChA+=UcCtgStr[ChN];
        }
        ChN++;
      }
      WordStrV.Add(TStr());
      TChA NGramWordChA;
      for (int WordStrN=0; WordStrN<WordStrV.Len(); WordStrN++){
        TStr WordStr=WordStrV[WordStrN];
        if (WordStr.Empty()){
          if (!NGramWordChA.Empty()){
            TStr NGramWordStr=NGramWordChA; NGramWordChA.Clr();
            if (NGramWordStr.IsChIn(' ')){
              AddWord(NGramWordStr)++; DocWordIdV.Add(TInt(GetWordId(NGramWordStr)));
            }
          }
        } else {
          AddWord(WordStr)++; DocWordIdV.Add(TInt(GetWordId(WordStr)));
          if (!NGramWordChA.Empty()){NGramWordChA+=' ';}
          NGramWordChA+=WordStr;
        }
      }
    }
  }
}

void TYBs::AddDoc(
 const PXWebPg& WebPg, const bool& DoGenDoc,
 const int& NGram, const TStrV& ExclUrlStrV,
 TIntPrV& DocIdRefbyDocIdPrV){
  TStr DocUrlStr;
  int DocId=-1;
  PYDoc Doc;
  bool IsDocWhitePg=false;
  TIntV DocCtgIdV;
  TIntV DocReftoDocIdV;
  TIntV DocWordIdV;
  int DocSects=0;
  int DocAbsSects=0;

  if (DoGenDoc){
    DocUrlStr=WebPg->GetUrlStr();
    DocId=UrlStrToDocH.GetKeyId(DocUrlStr);
    Doc=UrlStrToDocH.AddDat(DocUrlStr);
    PUrl DocUrl=new TUrl(DocUrlStr);
    IsDocWhitePg=false;
    int DocPathSegs=DocUrl->GetPathSegs();
    if (DocPathSegs>=2){
      IsDocWhitePg=DocUrl->GetPathSeg(DocPathSegs-2)=="White_Pages";}
    DocCtgIdV.Gen(20, 0);
    DocReftoDocIdV.Gen(100, 0);
    DocWordIdV.Gen(1000, 0);
    DocSects=0;
    DocAbsSects=0;
  }

  if (DoGenDoc){
    PUrl DocUrl=new TUrl(DocUrlStr); IAssert(DocUrl->IsOk());
    for (int PathSegN=0; PathSegN<DocUrl->GetPathSegs(); PathSegN++){
      TStr PathSegStr=DocUrl->GetPathSeg(PathSegN);
      if (!PathSegStr.Empty()){
        CtgStrToFqH.GetDat(PathSegStr)++;
        DocCtgIdV.Add(TInt(CtgStrToFqH.GetKeyId(PathSegStr)));
        AddCtgStrWords(PathSegStr, DocWordIdV);
      }
    }
    if (DocUrl->GetPathSegs()>0){DocWordIdV.Add(ParWordId); DocSects++;}
  }

  PSIn SIn=TStrIn::New(WebPg->GetBodyStr());
  PHtmlDoc HtmlDoc=PHtmlDoc(new THtmlDoc(SIn, hdtAll));
  bool InLi=false; bool ActLi=false; TStrQ PrevStrQ;
  PHtmlTok Tok; THtmlLxSym Sym=hsyUndef; TStr Str; int TokN=0;
  // up to <ul> tag including .../White_Pages/wp_*.html within <center> tags
  while ((TokN<HtmlDoc->GetToks())&&((Sym!=hsyBTag)||(Str!=THtmlTok::UlTagNm))){
    Tok=HtmlDoc->GetTok(TokN++, Sym, Str);
    if (DoGenDoc&& !IsDocWhitePg){
      if ((Sym==hsyBTag)&&(Str==THtmlTok::CenterTagNm)){
        Tok=HtmlDoc->GetTok(TokN++, Sym, Str);
        if ((Sym==hsySSym)&&(Str=="[")){
          while ((TokN<HtmlDoc->GetToks())&&
           ((Sym!=hsyBTag)||(Str!=THtmlTok::UlTagNm))){
            Tok=HtmlDoc->GetTok(TokN++, Sym, Str);
            if ((Sym==hsyBTag)&&(Str==THtmlTok::ATagNm)){
              PUrl Url=new TUrl(Tok->GetArg(THtmlTok::HRefArgNm), DocUrlStr);
              if (Url->IsOk()){
                IAssert(Url->GetPathSeg(Url->GetPathSegs()-2)=="White_Pages");
                int ReftoDocId=UrlStrToDocH.GetKeyId(Url->GetUrlStr());
                if ((ReftoDocId!=-1)&&(ReftoDocId!=int(RootDocId))&&
                 (ReftoDocId!=DocId)){
                  DocReftoDocIdV.AddMerged(ReftoDocId);
                  DocIdRefbyDocIdPrV.AddMerged(TIntPr(ReftoDocId, DocId));
                }
              }
            }
          }
        }
      }
    }
  }
  // from <ul> to Eof
  while (TokN<HtmlDoc->GetToks()){
    Tok=HtmlDoc->GetTok(TokN++, Sym, Str);
    switch (Sym){
      case hsyStr:
        if (InLi&&ActLi){
          if (DoGenDoc){
            if (!SwSet->IsIn(Str)){
              int WordId=GetWordId(Str);
              if (WordId!=-1){
                DocWordIdV.Add(WordId);
                PrevStrQ.Push(Str);
                int NGram=2;
                while ((NGram<=MxNGram)&&(NGram<=PrevStrQ.Len())){
                  int WordId=GetWordId(GetStrQStr(PrevStrQ, NGram));
                  if (WordId==-1){
                    while (PrevStrQ.Len()>=NGram){PrevStrQ.Pop();} break;
                  } else {
                    DocWordIdV.Add(WordId);
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
        }
        break;
      case hsyNum:
        if (InLi&&ActLi){PrevStrQ.Clr();}
        break;
      case hsySSym:
        if (InLi&&ActLi){PrevStrQ.Clr();}
        break;
      case hsyBTag:
        if (Str==THtmlTok::LiTagNm){
          if (DoGenDoc&&(DocWordIdV.Len()>0)&&(DocWordIdV.Last()!=ParWordId)){
            DocWordIdV.Add(ParWordId); DocSects++;}
          InLi=true; ActLi=true; PrevStrQ.Clr();
        } else
        if (InLi&&ActLi&&(Str==THtmlTok::ATagNm)){
          TStr ItemUrlStr=Tok->GetArg(THtmlTok::HRefArgNm);
          if (ExclUrlStrV.SearchBin(ItemUrlStr)!=-1){
            ActLi=false;}
          if (DoGenDoc&&InLi&&ActLi){
            if (TUrl::IsAbs(ItemUrlStr)){
              DocAbsSects++;}
            PUrl ItemUrl=TUrl::New(ItemUrlStr, DocUrlStr);
            if (ItemUrl->IsOk()){
              int ReftoDocId=UrlStrToDocH.GetKeyId(ItemUrl->GetUrlStr());
              if ((ReftoDocId!=-1)&&(ReftoDocId!=int(RootDocId))&&
               (ReftoDocId!=DocId)){
                DocReftoDocIdV.AddMerged(ReftoDocId);
                DocIdRefbyDocIdPrV.AddMerged(TIntPr(ReftoDocId, DocId));
              }
            }
          }
        }
        break;
      case hsyETag:
        if (Str==THtmlTok::UlTagNm){
          if (DoGenDoc&&(DocWordIdV.Len()>0)&&(DocWordIdV.Last()!=ParWordId)){
            DocWordIdV.Add(ParWordId); DocSects++;
          }
          InLi=false; ActLi=false; PrevStrQ.Clr();
        }
        break;
      default:;
    }
  }
  if (DoGenDoc){
    if (!DocCtgIdV.Empty()){
      Doc->FirstPathCtgIdN=PathCtgIdVHeap.Len();
      PathCtgIdVHeap.AddV(DocCtgIdV); PathCtgIdVHeap.Add(TInt(-1));
    }
    if (!DocReftoDocIdV.Empty()){
      Doc->FirstReftoDocIdN=ReftoDocIdVHeap.Len();
      ReftoDocIdVHeap.AddV(DocReftoDocIdV); ReftoDocIdVHeap.Add(TInt(-1));
    }
    if (!DocWordIdV.Empty()){
      Doc->FirstWordIdN=WordIdVHeap.Len();
      WordIdVHeap.AddV(DocWordIdV); WordIdVHeap.Add(TInt(-1));
    }
    Doc->Sects=DocSects; Sects+=DocSects;
    Doc->AbsSects=DocAbsSects; AbsSects+=DocAbsSects;
  }
}

TStr TYBs::GetCtgIdVStr(const TIntV& CtgIdV){
  TChA ChA;
  for (int CtgIdN=0; CtgIdN<CtgIdV.Len(); CtgIdN++){
    if (CtgIdN>0){ChA+=' ';}
    ChA+=GetCtgStr(CtgIdV[CtgIdN]);
  }
  return ChA;
}

void TYBs::GetCtgStrV(const TIntV& CtgIdV, TStrV& CtgStrV){
  CtgStrV.Gen(CtgIdV.Len(), 0);
  for (int CtgIdN=0; CtgIdN<CtgIdV.Len(); CtgIdN++){
    TStr CtgStr=GetCtgStr(CtgIdV[CtgIdN]);
    CtgStrV.Add(CtgStr);
  }
}

TStr TYBs::GetCtgIdToWFqHStr(const TIntFltH& CtgIdToWFqH, const double& SumPrb){
  TFltIntKdV CtgWFqIdKdV(CtgIdToWFqH.Len(), 0);
  double SumCtgWFq=0;
  int CtgIdToWFqP=CtgIdToWFqH.FFirstKeyId();
  while (CtgIdToWFqH.FNextKeyId(CtgIdToWFqP)){
    int CtgId=CtgIdToWFqH.GetKey(CtgIdToWFqP);
    double CtgWFq=CtgIdToWFqH[CtgIdToWFqP];
    CtgWFqIdKdV.Add(TFltIntKd(CtgWFq, CtgId));
    SumCtgWFq+=CtgWFq;
  }
  CtgWFqIdKdV.Sort(false);
  SumCtgWFq*=SumPrb;
  TChA ChA;
  for (int CtgWFqIdKdN=0; CtgWFqIdKdN<CtgWFqIdKdV.Len(); CtgWFqIdKdN++){
    double CtgWFq=CtgWFqIdKdV[CtgWFqIdKdN].Key;
    int CtgId=CtgWFqIdKdV[CtgWFqIdKdN].Dat;
    SumCtgWFq-=CtgWFq;
    if (SumCtgWFq>=0){
      ChA+=GetCtgStr(CtgId);
//      ChA+="["; ChA+=TFlt::GetStr(CtgWFq, 0, 2); ChA+="]";
      ChA+=" ";
    }
  }
  return ChA;
}

TInt& TYBs::AddWord(const TStr& WordStr){
  int PrevLen=WordChAHeap.Len();
  TYWord Word(this, WordStr);
  int WordId=WordToFqH.GetKeyId(Word);
  if (WordId==-1){
    return WordToFqH.AddDat(Word);
  } else {
    WordChAHeap.Trunc(PrevLen);
    return WordToFqH[WordId];
  }
}

int TYBs::GetWordId(const TStr& WordStr){
  int PrevLen=WordChAHeap.Len();
  TYWord Word(this, WordStr);
  int WordId=WordToFqH.GetKeyId(Word);
  WordChAHeap.Trunc(PrevLen);
  return WordId;
}

void TYBs::GetLevDocIdV(const int& Lev, TIntV& DocIdV){
  DocIdV.Clr();
  int DocId=FFirstDocId();
  while (FNextDocId(DocId)){
    PUrl Url=new TUrl(GetDocUrlStr(DocId));
    if (Url->GetPathSegs()==Lev){
      DocIdV.Add(DocId);}
  }
}

void TYBs::GetCtgIdV(const int& DocId, TIntV& CtgIdV){
  CtgIdV.Clr();
  int CtgIdN=FFirstDocCtgId(DocId); int CtgId;
  while (FNextDocCtgId(CtgIdN, CtgId)){
    CtgIdV.Add(CtgId);}
}

void TYBs::GetParentDocIdV(const int& DocId, TIntV& DocIdV){
  DocIdV.Clr();
  TIntQ DocIdQ; DocIdQ.Push(DocId);
  while (!DocIdQ.Empty()){
    int DocId=DocIdQ.Top(); DocIdQ.Pop();
    if (DocIdV.SearchBin(DocId)==-1){
      DocIdV.AddSorted(DocId);
      int RefbyDocIdN=FFirstDocRefbyDocId(DocId); int RefbyDocId;
      while (FNextDocRefbyDocId(RefbyDocIdN, RefbyDocId)){
        DocIdQ.Push(RefbyDocId);}
    }
  }
}

int TYBs::GetDocDist(const int& LDocId, const int& RDocId){
  TIntIntH LDocIdToDistH(100);
  TIntPrQ LDocIdQ; LDocIdQ.Push(TIntPr(LDocId, TInt(0)));
  while (!LDocIdQ.Empty()){
    int DocId=LDocIdQ.Top().Val1; int Dist=LDocIdQ.Top().Val2; LDocIdQ.Pop();
    if (!LDocIdToDistH.IsKey(DocId)){
      LDocIdToDistH.AddDat(DocId, Dist);
      int RefbyDocIdN=FFirstDocRefbyDocId(DocId); int RefbyDocId;
      while (FNextDocRefbyDocId(RefbyDocIdN, RefbyDocId)){
        LDocIdQ.Push(TIntPr(RefbyDocId, TInt(Dist+1)));}
    }
  }

  TIntPrQ RDocIdQ; RDocIdQ.Push(TIntPr(RDocId, TInt(0)));
  while (!RDocIdQ.Empty()){
    int DocId=RDocIdQ.Top().Val1; int Dist=RDocIdQ.Top().Val2; RDocIdQ.Pop();
    if (LDocIdToDistH.IsKey(DocId)){
      return Dist+LDocIdToDistH.GetDat(DocId);
    } else {
      int RefbyDocIdN=FFirstDocRefbyDocId(DocId); int RefbyDocId;
      while (FNextDocRefbyDocId(RefbyDocIdN, RefbyDocId)){
        RDocIdQ.Push(TIntPr(RefbyDocId, TInt(Dist+1)));}
    }
  }
  return -1;
}

int TYBs::GetDocReftos(const int& DocId){
  int Reftos=0;
  int ReftoDocIdN=FFirstDocReftoDocId(DocId); int ReftoDocId;
  while (FNextDocReftoDocId(ReftoDocIdN, ReftoDocId)){Reftos++;}
  return Reftos;
}

int TYBs::GetDocRefbys(const int& DocId){
  int Refbys=0;
  int RefbyDocIdN=FFirstDocRefbyDocId(DocId); int RefbyDocId;
  while (FNextDocRefbyDocId(RefbyDocIdN, RefbyDocId)){Refbys++;}
  return Refbys;
}

PYWordDs TYBs::GetWordDs(const PSIn& SIn){
  PYWordDs WordDs=PYWordDs(new TYWordDs());
  PHtmlDoc HtmlDoc=new THtmlDoc(SIn, hdtAll);
  TStrQ PrevStrQ;
  PHtmlTok Tok; THtmlLxSym Sym; TStr Str;
  for (int TokN=0; TokN<HtmlDoc->GetToks(); TokN++){
    Tok=HtmlDoc->GetTok(TokN, Sym, Str);
    switch (Sym){
      case hsyStr:
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
      case hsyNum:
      case hsySSym:
        PrevStrQ.Clr(); break;
      case hsyBTag:
      case hsyETag:
        if (THtmlTok::IsBreakTag(Str)){PrevStrQ.Clr();} break;
      default:;
    }
  }
  return WordDs;
}

void TYBs::GetAbsSectV(
 const TStr& RefUrlStr, const PXWebPg& WebPg, TYSectV& YSectV){
  YSectV.Gen(100, 0);
  TChA TitleChA;
  PUrl RefUrl=PUrl(new TUrl(RefUrlStr)); IAssert(RefUrl->IsOk());
  TitleChA+=THtmlTok::TitleTagNm; TitleChA+=RefUrl->GetPathStr();
  TitleChA+=THtmlTok::TitleETagNm; TitleChA.ChangeCh('_', ' ');
  PSIn SIn=TStrIn::New(WebPg->GetBodyStr());
  PHtmlDoc HtmlDoc=new THtmlDoc(SIn, hdtUL);
  PHtmlTok Tok; THtmlLxSym Sym; TStr Str; int TokN=0;
  while (TokN<HtmlDoc->GetToks()){
    Tok=HtmlDoc->GetTok(TokN++, Sym, Str);
    if ((Sym==hsyBTag)&&(Str==THtmlTok::LiTagNm)){
      IAssert(TokN<HtmlDoc->GetToks());
      Tok=HtmlDoc->GetTok(TokN++, Sym, Str);
      IAssert((Sym==hsyBTag)&&(Str==THtmlTok::ATagNm));
      IAssert(Tok->IsArg(THtmlTok::HRefArgNm));
      TChA TxtChA; TxtChA+=TitleChA; TxtChA+=' '; TxtChA+=Tok->GetFullStr();
      TStr UrlStr=Tok->GetArg(THtmlTok::HRefArgNm);
      if (TUrl::IsAbs(UrlStr)){
        PUrl Url=PUrl(new TUrl(UrlStr));
        if (Url->IsOk()){
          PYWordDs WordDs=PYWordDs(new TYWordDs());
          TStrQ PrevStrQ; bool InSect=true;
          while ((InSect)&&(TokN<HtmlDoc->GetToks())){
            Tok=HtmlDoc->GetTok(TokN++, Sym, Str);
            switch (Sym){
              case hsyStr:
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
              case hsyNum: PrevStrQ.Clr(); break;
              case hsySSym: PrevStrQ.Clr(); break;
              case hsyBTag: InSect=(Str!=THtmlTok::LiTagNm); break;
              case hsyETag: InSect=(Str!=THtmlTok::UlTagNm); break;
              default:;
            }
            if (InSect){TxtChA+=' '; TxtChA+=Tok->GetFullStr();;}
          }
          PYSect YSect=new TYSect(RefUrlStr, Url->GetUrlStr(), WordDs, TxtChA);
          YSectV.Add(YSect);
        }
      }
    }
  }
}

void TYBs::GetAbsSectUrlStrV(const PXWebPg& WebPg, TStrV& UrlStrV){
  UrlStrV.Gen(100, 0);
  PSIn SIn=TStrIn::New(WebPg->GetBodyStr());
  PHtmlDoc HtmlDoc=new THtmlDoc(SIn, hdtUL);
  PHtmlTok Tok; THtmlLxSym Sym; TStr Str; int TokN=0;
  while (TokN<HtmlDoc->GetToks()){
    Tok=HtmlDoc->GetTok(TokN++, Sym, Str);
    if ((Sym==hsyBTag)&&(Str==THtmlTok::LiTagNm)){
      IAssert(TokN<HtmlDoc->GetToks());
      Tok=HtmlDoc->GetTok(TokN++, Sym, Str);
      IAssert((Sym==hsyBTag)&&(Str==THtmlTok::ATagNm));
      IAssert(Tok->IsArg(THtmlTok::HRefArgNm));
      TStr UrlStr=Tok->GetArg(THtmlTok::HRefArgNm);
      if (TUrl::IsAbs(UrlStr)){
        PUrl Url=new TUrl(UrlStr);
        if (Url->IsOk()){
          UrlStrV.Add(Url->GetUrlStr());}
      }
    }
  }
}

void TYBs::SaveTxt(const PSOut& SOut){
  TOLx Lx(SOut, TFSet()|oloFrcEoln|oloSigNum);
  int UrlStrToDocP=UrlStrToDocH.FFirstKeyId();
  while (UrlStrToDocH.FNextKeyId(UrlStrToDocP)){
    TStr UrlStr=UrlStrToDocH.GetKey(UrlStrToDocP);
    PYDoc Doc=UrlStrToDocH[UrlStrToDocP]; TChA ChA(10000);
    int DocId=UrlStrToDocP;
    Lx.PutVarStr("UrlStr", UrlStr);
    Lx.PutVarInt("DocId", DocId);
    Lx.PutVarInt("Sects", Doc->Sects);
    Lx.PutVarInt("AbsSects", Doc->AbsSects);

    TIntV CtgIdV; GetCtgIdV(DocId, CtgIdV);
    Lx.PutVarStr("CtgIdV", GetCtgIdVStr(CtgIdV));

    Lx.PutVar("ReftoDocIdV", true, false); 
    if (Doc->FirstReftoDocIdN!=-1){
      int ReftoDocIdN=Doc->FirstReftoDocIdN;
      while (ReftoDocIdVHeap[ReftoDocIdN]!=-1){
        Lx.PutInt(ReftoDocIdVHeap[ReftoDocIdN++]);}
    }
    Lx.PutSym(syRBracket); Lx.PutLn();

    Lx.PutVar("RefbyDocIdV", true, false);
    if (Doc->FirstRefbyDocIdN!=-1){
      int RefbyDocIdN=Doc->FirstRefbyDocIdN;
      while (RefbyDocIdVHeap[RefbyDocIdN]!=-1){
        Lx.PutInt(RefbyDocIdVHeap[RefbyDocIdN++]);}
    }
    Lx.PutSym(syRBracket); Lx.PutLn();

    Lx.PutVar("Doc", true, true);
    if (Doc->FirstWordIdN!=-1){
      int WordIdN=Doc->FirstWordIdN;
      while (WordIdVHeap[WordIdN]!=-1){
        int WordId=WordIdVHeap[WordIdN++];
        TStr WordStr=GetWordStr(WordId);
        int WordSegs=GetWordSegs(WordId);
        if (WordId==int(ParWordId)){
          Lx.PutQStr(ChA); Lx.PutLn(); ChA.Clr();
        } else {
          if (!ChA.Empty()){ChA+=' ';}
          if (WordStr.SearchCh(' ')==-1){
            ChA+=WordStr;
          } else {
            ChA+='('; ChA+='#'; ChA+=TInt::GetStr(WordSegs);
            ChA+=' '; ChA+=WordStr; ChA+=')';
          }
        }
      }
    }
    Lx.PutSym(syRBracket); Lx.PutLn(); Lx.PutLn();
  }
}

void TYBs::SaveTxtWords(const TStr& FNm){
  TIntStrKdV WordFqStrKdV;
  int WordToFqP=WordToFqH.FFirstKeyId();
  while (WordToFqH.FNextKeyId(WordToFqP)){
    TStr WordStr=GetWordStr(WordToFqP);
    int WordFq=WordToFqH[WordToFqP];
    WordFqStrKdV.Add(TIntStrKd(WordFq, WordStr));
  }
  WordFqStrKdV.Sort(false);
  PSOut WordSOut=PSOut(new TFOut(FNm));
  for (int WordFqStrKdN=0; WordFqStrKdN<WordFqStrKdV.Len(); WordFqStrKdN++){
    int WordFq=WordFqStrKdV[WordFqStrKdN].Key;
    TStr WordStr=WordFqStrKdV[WordFqStrKdN].Dat;
    WordSOut->PutStr(WordStr); WordSOut->PutStr(": ");
    WordSOut->PutStr(TInt::GetStr(WordFq)); WordSOut->PutLn();
  }
}

void TYBs::SaveTxtCtgs(const TStr& FNm){
  TIntStrKdV CtgFqStrKdV;
  int CtgStrToFqP=CtgStrToFqH.FFirstKeyId();
  while (CtgStrToFqH.FNextKeyId(CtgStrToFqP)){
    TStr CtgStr=CtgStrToFqH.GetKey(CtgStrToFqP);
    int CtgFq=CtgStrToFqH[CtgStrToFqP];
    CtgFqStrKdV.Add(TIntStrKd(CtgFq, CtgStr));
  }
  CtgFqStrKdV.Sort(false);
  PSOut CtgSOut=PSOut(new TFOut(FNm));
  for (int CtgFqStrKdN=0; CtgFqStrKdN<CtgFqStrKdV.Len(); CtgFqStrKdN++){
    int CtgFq=CtgFqStrKdV[CtgFqStrKdN].Key;
    TStr CtgStr=CtgFqStrKdV[CtgFqStrKdN].Dat;
    int CtgId=GetCtgId(CtgStr);
    CtgSOut->PutStr(CtgStr); CtgSOut->PutStr(": ");
    CtgSOut->PutStr(TInt::GetStr(CtgFq));
    CtgSOut->PutStr(TStr(" [Id:")+TInt::GetStr(CtgId)+"]");
    CtgSOut->PutLn();
  }
}

/////////////////////////////////////////////////
// Yahoo-Distribution-Base
int TYDsBs::GetMissDss(const PYBs& YBs, const int& DocId){
  int MissDss=0;
  int ReftoDocIdN=YBs->FFirstDocReftoDocId(DocId); int ReftoDocId;
  while (YBs->FNextDocReftoDocId(ReftoDocIdN, ReftoDocId)){
    if (!DocIdToWordDsH.IsKey(ReftoDocId)){MissDss++;}}
  return MissDss;
}

double TYDsBs::GetNrWgt(const TYDsBsNrType& NrType, const PYWordDs& WordDs){
  switch (NrType){
    case ydnConst: return 1;
    case ydnWords: return WordDs->GetSumWordFq();
    case ydnLnWords: return log10(1+WordDs->GetSumWordFq());
    case ydnSects: return 1+WordDs->GetSects();
    case ydnLnSects: return log10(1+WordDs->GetSects());
    case ydnDocs: return 1+WordDs->GetDocs();
    case ydnLnDocs: return log10(1+WordDs->GetDocs());
    default: Fail; return -1;
  }
}

TYDsBs::TYDsBs(const TYDsBsNrType& NrType, const double& WordFqExp,
 const double& MnWordPrb, const double& /*MnAllWordPrb*/,
 const PYBs& YBs, const PNotify& _Notify):
  Notify(_Notify), AllWordDs(), DocIdToWordDsH(10000){
  TNotify::OnNotify(Notify, ntInfo, "Start Creating Distributions");

  int RootDocId=YBs->GetRootDocId();
  int ParWordId=YBs->GetParWordId();
  int PrevDocs=0; int MxMissDss=0;
  TIntH RepDocIdH(1000); int RepDocIters=3;
  while ((DocIdToWordDsH.Len()<YBs->GetDocs())||(!RepDocIdH.Empty())){
    if ((DocIdToWordDsH.Len()>0)&&(PrevDocs==DocIdToWordDsH.Len())){
      if (PrevDocs==YBs->GetDocs()){MxMissDss=-1;} else {MxMissDss++;}
    }
    PrevDocs=DocIdToWordDsH.Len();
    int DocId=YBs->FFirstDocId();
    while (YBs->FNextDocId(DocId)){
      bool MkWordDs=false;
      if (MxMissDss==-1){
        if (RepDocIdH.IsKey(DocId)){
          MkWordDs=true;
          if (RepDocIters<=1){RepDocIdH.DelKey(DocId);}
        }
      } else {
        if (!DocIdToWordDsH.IsKey(DocId)){
          int MissDss=GetMissDss(YBs, DocId);
          if (MissDss<=MxMissDss){
            MkWordDs=true; if (MissDss>0){RepDocIdH.AddKey(DocId);}
          }
        }
      }
      if (MkWordDs){
        PYWordDs WordDs=new TYWordDs(1);
        int WordIdN=YBs->FFirstDocWordId(DocId); int WordId;
        while (YBs->FNextDocWordId(WordIdN, WordId)){
          if (WordId==ParWordId){WordDs->AddSect();}
          else {WordDs->AddWordIdFq(WordId, 1);}
        }
        IAssert(WordDs->GetSects()==YBs->GetDocSects(DocId));

        WordDs->NrToSumWordFq(GetNrWgt(NrType, WordDs));
        int ReftoDocIdN=YBs->FFirstDocReftoDocId(DocId); int ReftoDocId;
        while (YBs->FNextDocReftoDocId(ReftoDocIdN, ReftoDocId)){
          if (DocIdToWordDsH.IsKey(ReftoDocId)){
            PYWordDs ReftoWordDs=DocIdToWordDsH.GetDat(ReftoDocId);
            double ReftoWordDsW=GetNrWgt(NrType, ReftoWordDs);
            WordDs=TYWordDs::GetMerged(WordDs, ReftoWordDs,
             WordDs->GetSumWordFq(), ReftoWordDsW);
          }
        }
        WordDs->PowWordFq(WordFqExp);
        WordDs=TYWordDs::GetTruncByMnWordPrb(WordDs, MnWordPrb);
        DocIdToWordDsH.AddDat(DocId, WordDs);
        if (DocIdToWordDsH.Len()%1000==0){
          TNotify::OnNotify(Notify, ntInfo, TStr("...")+
           TInt::GetStr(DocIdToWordDsH.Len())+" Distrs.");
        }
      }
    }
    if (MxMissDss==-1){
      if (--RepDocIters==0){RepDocIdH.AddKey(RootDocId);}
    }
    TStr OnWordDsStr=TStr("Missing distrs.: ")+TInt::GetStr(MxMissDss)+";"+
     " Docs.: "+TInt::GetStr(DocIdToWordDsH.Len());
    TNotify::OnNotify(Notify, ntInfo, OnWordDsStr);
  }

  TNotify::OnNotify(Notify, ntInfo, "Create Overall Distribution");
  int AllDocs=GetWordDs(RootDocId)->GetDocs();
  int AllSects=GetWordDs(RootDocId)->GetSects();
  AllWordDs=PYWordDs(new TYWordDs(AllDocs, AllSects, YBs->GetWords()));
  {int WordId=YBs->FFirstWordId();
  while (YBs->FNextWordId(WordId)){
    AllWordDs->AddWordIdFq(WordId, YBs->GetWordFq(WordId));}}
  AllWordDs->PowWordFq(WordFqExp);
  AllWordDs=TYWordDs::GetTruncBySumWordPrb(AllWordDs, 0.9);
//  AllWordDs=TYWordDs::GetTruncByMnWordPrb(AllWordDs, MnAllWordPrb);

  TNotify::OnNotify(Notify, ntInfo, "Finished Creating Distributions");
}

void TYDsBs::SaveTxt(const PSOut& SOut, const PYBs& YBs){
  AllWordDs->SaveTxt(SOut, YBs(), -1);
  int DocIdToWordDsP=DocIdToWordDsH.FFirstKeyId();
  while (DocIdToWordDsH.FNextKeyId(DocIdToWordDsP)){
    int DocId=DocIdToWordDsH.GetKey(DocIdToWordDsP);
    PYWordDs WordDs=DocIdToWordDsH[DocIdToWordDsP];
    WordDs->SaveTxt(SOut, YBs(), DocId);
  }
}

TStr TYDsBs::GetNrTypeStr(const TYDsBsNrType& NrType){
  switch (NrType){
    case ydnConst: return "Const";
    case ydnWords: return "Words";
    case ydnLnWords: return "LnWords";
    case ydnSects: return "Sects";
    case ydnLnSects: return "LnSects";
    case ydnDocs: return "Docs";
    case ydnLnDocs: return "LnDocs";
    default: Fail; return TStr();
  }
}

/////////////////////////////////////////////////
// Yahoo-Test-Base
TYTsBs::TYTsBs(
 const double& AbsSectsPrb, const double& SectPrb, const TStr& TsWebBsFPath,
 const PXWebBs& RefWebBs, const PYBs& RefYBs,
 const PXWebTravelEvent& WebTravelEvent, const PNotify& Notify):
  TsRefUrlStrH(1000), WebBs(), WebTravel(){
  IAssert((0<=AbsSectsPrb)&&(AbsSectsPrb<=1));
  if (AbsSectsPrb==1){
    TRnd Rnd;
    int DocId=RefYBs->FFirstDocId();
    while (RefYBs->FNextDocId(DocId)){
      if (RefYBs->GetDocRefbys(DocId)==0){continue;}
      TStr RefUrlStr=RefYBs->GetDocUrlStr(DocId);
      PXWebPg WebPg=RefWebBs->GetWebPg(RefUrlStr);
      TStrV UrlStrV; TYBs::GetAbsSectUrlStrV(WebPg, UrlStrV);
      for (int UrlStrN=0; UrlStrN<UrlStrV.Len(); UrlStrN++){
        if (Rnd.GetUniDev()<SectPrb){
          TsRefUrlStrH.AddDat(UrlStrV[UrlStrN], RefUrlStr);
        }
      }
    }
  } else {
    int TsDocs=int(AbsSectsPrb*RefYBs->GetAbsSects());
    TRnd Rnd;
    while (TsRefUrlStrH.Len()<TsDocs){
      int DocId=Rnd.GetUniDevInt(RefYBs->GetDocs());
      if (RefYBs->GetDocRefbys(DocId)==0){continue;}
      TStr RefUrlStr=RefYBs->GetDocUrlStr(DocId);
      PXWebPg WebPg=RefWebBs->GetWebPg(RefUrlStr);
      TStrV UrlStrV; TYBs::GetAbsSectUrlStrV(WebPg, UrlStrV);
      if (UrlStrV.Len()==0){continue;}
      int UrlStrN=Rnd.GetUniDevInt(UrlStrV.Len());
      TStr TsUrlStr=UrlStrV[UrlStrN];
      TsRefUrlStrH.AddDat(TsUrlStr, RefUrlStr);
    }
  }
  TNotify::OnNotify(Notify, ntInfo,
   TStr("Loading ")+TInt::GetStr(TsRefUrlStrH.Len())+" Documents.");

  WebBs=PXWebBs(new TWebMemBs(100, TsWebBsFPath, true));
  WebTravel=PXWebTravel(new TXWebTravel(WebBs, WebTravelEvent));
  int TsSrcUrlStrP=TsRefUrlStrH.FFirstKeyId();
  while (TsRefUrlStrH.FNextKeyId(TsSrcUrlStrP)){
    TStr TsUrlStr=TsRefUrlStrH.GetKey(TsSrcUrlStrP);
    WebTravel->Go(TsUrlStr);
  }
}

TYTsBs::~TYTsBs(){
  int WebPgP=WebBs->FFirstWebPg(); int WebPgId;
  while (WebBs->FNextWebPg(WebPgP, WebPgId)){
    TStr TsUrlStr=WebBs->GetUrlStr(WebPgId);
    TStr RefUrlStr=TsRefUrlStrH.GetDat(TsUrlStr);
    WebBs->AddWebPgRef(TsUrlStr, RefUrlStr);
  }
}

/////////////////////////////////////////////////
// Yahoo-HyperLink-Context
TYHlCtx::TYHlCtx(
 const PXWebBs& RefWebBs, const PYBs& RefYBs, const PXWebBs& DocWebBs,
 const PNotify& /*Notify*/): HlCtxStrV(){
  int RefDocId=RefYBs->FFirstDocId();
  while (RefYBs->FNextDocId(RefDocId)){
    TStr RefUrlStr=RefYBs->GetDocUrlStr(RefDocId);
    PXWebPg WebPg=RefWebBs->GetWebPg(RefUrlStr);
    TYSectV YSectV; RefYBs->GetAbsSectV(RefUrlStr, WebPg, YSectV);
    for (int YSectN=0; YSectN<YSectV.Len(); YSectN++){
      PYSect YSect=YSectV[YSectN];
      if (DocWebBs->IsUrlStr(YSect->GetUrlStr())){
        HlCtxStrV.Add(YSect->GetTxtStr());}
    }
    if (HlCtxStrV.Len()>500){break;}//**
  }
}

