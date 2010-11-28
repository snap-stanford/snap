#ifndef HlDoc_h
#define HlDoc_h

/////////////////////////////////////////////////
// Includes
#include "base.h"
#include "stopword.h"
#include "webold.h"

/////////////////////////////////////////////////
// HyperLinkDocument-Word-Distribution
class THldBs;
typedef TPt<THldBs> PHldBs;

class THldWordDs{
private:
  TCRef CRef;
public:
  TFlt SumWordFq;
  TIntFltKdV WordIdFqKdV;
public:
  THldWordDs(const int& MxWordIds=0):
    SumWordFq(0), WordIdFqKdV(MxWordIds, 0){}
  ~THldWordDs(){}
  THldWordDs(TSIn& SIn):
    SumWordFq(SIn), WordIdFqKdV(SIn){}
  static TPt<THldWordDs> Load(TSIn& SIn){return new THldWordDs(SIn);}
  void Save(TSOut& SOut){
    SumWordFq.Save(SOut); WordIdFqKdV.Save(SOut);}

  THldWordDs& operator=(const THldWordDs& HldWordDs){
    if (this!=&HldWordDs){
      SumWordFq=HldWordDs.SumWordFq; WordIdFqKdV=HldWordDs.WordIdFqKdV;}
    return *this;}

  void AddWordIdFq(const TIntFltKd& WordIdFqKd);
  void AddWordIdFq(const int& WordId, const double& WordFq){
    AddWordIdFq(TIntFltKd(WordId, WordFq));}
  bool IsWordId(const int& WordId, double& WordFq) const {
    int WordIdFqKdN=WordIdFqKdV.SearchBin(TInt(WordId));
    if (WordIdFqKdN==-1){return false;}
    else {WordFq=WordIdFqKdV[WordIdFqKdN].Dat; return true;}}
  bool IsWordId(const int& WordId) const {double WordFq;
    return IsWordId(WordId, WordFq);}
  double GetWordFq(const int& WordId) const {
    return WordIdFqKdV[WordIdFqKdV.SearchBin(TInt(WordId))].Dat;}
  double GetWordPrb(const int& WordId) const {double WordFq;
    if (IsWordId(WordId, WordFq)){return WordFq/SumWordFq;} else {return 0;}}
  int GetWordIds() const {return WordIdFqKdV.Len();}
  double GetSumWordFq(){return SumWordFq;}
  int FFirstWordId() const {return 0-1;}
  bool FNextWordId(
   int& WordIdN, int& WordId, double& WordFq, double& WordPrb) const;

  void NrToSumWordFq(const double& NrSumWordFq);
  void PowWordFq(const double& Exp);

  void SaveTxt(const PSOut& SOut, const PHldBs& HldBs);

  static TPt<THldWordDs> GetMerged(
   const TPt<THldWordDs>& LWordDs, const TPt<THldWordDs>& RWordDs);
  static TPt<THldWordDs> GetTruncByMxWords(
   const TPt<THldWordDs>& WordDs, const int& MxWords);
  static double GetDist(
   const TPt<THldWordDs>& LWordDs, const TPt<THldWordDs>& RWordDs,
   const PHldBs& HldBs);
  static void GetHypTsEst(
   const TPt<THldWordDs>& RefWordDs, const TPt<THldWordDs>& HypWordDs,
   double& Precis, double& Recall, double& Dist, double& F2);

  friend TPt<THldWordDs>;
};
typedef TPt<THldWordDs> PHldWordDs;

/////////////////////////////////////////////////
// HyperLinkDocument-Word
class THldWord{
private:
  THldBs* HldBs;
  TInt FirstChN;
public:
  THldWord(): HldBs(NULL), FirstChN(-1){}
  THldWord(const THldWord& HldWord):
    HldBs(HldWord.HldBs), FirstChN(HldWord.FirstChN){}
  THldWord(THldBs* _HldBs, const TStr& Str);
  ~THldWord(){}
  THldWord(TSIn& SIn): HldBs(NULL), FirstChN(SIn){}
  void Save(TSOut& SOut){FirstChN.Save(SOut);}

  THldWord& operator=(const THldWord& HldWord){
    if (this!=&HldWord){HldBs=HldWord.HldBs; FirstChN=HldWord.FirstChN;}
    return *this;}
  bool operator==(const THldWord& HldWord) const {
    return (GetStr()==HldWord.GetStr());}
  bool operator<(const THldWord& HldWord) const {
    return GetStr()<HldWord.GetStr();}

  TStr GetStr() const;

  int GetPrimHashCd() const {return GetStr().GetPrimHashCd();}
  int GetSecHashCd() const {return GetStr().GetSecHashCd();}

  friend THldBs;
};

/////////////////////////////////////////////////
// HyperLink-Document
class THlDoc{
private:
  TInt HlId;
  TInt DocId;
  PHldWordDs HlWordDs;
public:
  THlDoc(): HlId(-1), DocId(-1), HlWordDs(NULL){}
  THlDoc(const int& _HlId, const int& _DocId, const PHldWordDs& _HlWordDs):
    HlId(_HlId), DocId(_DocId), HlWordDs(_HlWordDs){}
  ~THlDoc(){}
  THlDoc(TSIn& SIn):
    HlId(SIn), DocId(SIn), HlWordDs(SIn){}
  void Save(TSOut& SOut){
    HlId.Save(SOut); DocId.Save(SOut); HlWordDs.Save(SOut);}

  THlDoc& operator=(const THlDoc& HlDoc){
    HlId=HlDoc.HlId; DocId=HlDoc.DocId; HlWordDs=HlDoc.HlWordDs; return *this;}

  int GetHlId(){return HlId;}
  int GetDocId(){return DocId;}
  PHldWordDs GetHlWordDs(){return HlWordDs;}
};

/////////////////////////////////////////////////
// HyperLinkDocument-Base
class THldBs{
private:
  TCRef CRef;
private:
  TInt MxNGram;
  TInt MnWordFq;
  PSwSet SwSet;
  THash<THldWord, TInt> WordToFqH;
  TChA WordChAHeap;
  TIntV WordIdVHeap;
  THash<TInt, TStr> DocIdToUrlStrH;
  TVec<THlDoc> HlDocV;
  PNotify Notify;
  void PutThisToWord();
  TStr GetStrQStr(const TStrQ& StrQ, const int& Strs=-1);
public:
  THldBs(
   const TStrV& HlCtxStrV, const PXWebBs& WebBs,
   const int& _MxNGram, const int& _MnWordFq,
   const TSwSetTy& SwSetTy, const PNotify& _Notify);
  ~THldBs(){}
  THldBs(TSIn& SIn, const PNotify& _Notify=NULL):
    MxNGram(SIn), MnWordFq(SIn), SwSet(SIn),
    WordToFqH(SIn), WordChAHeap(SIn), WordIdVHeap(SIn),
    DocIdToUrlStrH(SIn), HlDocV(SIn), Notify(_Notify){
    SIn.LoadCs(); PutThisToWord();}
  static TPt<THldBs> Load(TSIn& SIn){return new THldBs(SIn);}
  void Save(TSOut& SOut){
    MxNGram.Save(SOut); MnWordFq.Save(SOut); SwSet.Save(SOut);
    WordToFqH.Save(SOut); WordChAHeap.Save(SOut); WordIdVHeap.Save(SOut);
    DocIdToUrlStrH.Save(SOut); HlDocV.Save(SOut); SOut.SaveCs();}

  THldBs& operator=(const THldBs&){Fail; return *this;}

  int GetWords(){return WordToFqH.Len();}
  TInt& AddWord(const TStr& WordStr);
  int GetWordId(const TStr& WordStr);
  TStr GetWordStr(const int& WordId){return WordToFqH.GetKey(WordId).GetStr();}
  int GetWordFq(const int& WordId){return WordToFqH[WordId];}

  int AddTxt(const TStr& TxtStr, const bool& DoGenDoc, const int& NGram);
  int GetTxtLen(const int& TxtId);
  PHldWordDs GetTxtWordDs(const int& TxtId);

  PHldWordDs GetWordDs(const PSIn& SIn);
  PHldWordDs GetWordDs(const TStr& Str){
    PSIn SIn=TStrIn::New(Str); return GetWordDs(SIn);}

  int GetHlDocs(){return HlDocV.Len();}
  int GetHlId(const int& HlDocN){return HlDocV[HlDocN].GetHlId();}
  int GetDocId(const int& HlDocN){return HlDocV[HlDocN].GetDocId();}
  PHldWordDs GetHlWordDs(const int& HlDocN){
    return HlDocV[HlDocN].GetHlWordDs();}
  PHldWordDs GetDocWordDs(const int& HlDocN){
    return GetTxtWordDs(HlDocV[HlDocN].GetDocId());}
  TStr GetDocUrlStr(const int& HlDocN){
    return DocIdToUrlStrH.GetDat(TInt(HlDocV[HlDocN].GetDocId()));}

  void GetDistHlDocNV(
   const int& KNNbrs, const bool& RndSel,
   const PHldWordDs& RefHlWordDs, const TStrV& SelUrlStrV,
   TFltIntKdV& DistHlDocNKdV);
  PHldWordDs GetHypDocWordDs(const TFltIntKdV& DistHlDocNKdV);

  void SaveTxt(const PSOut& SOut);

  static TStr GetUrlStrFromHlCtxStr(const TStr& HlCtxStr);

  friend THldWord;
  friend TPt<THldBs>;
};
typedef TPt<THldBs> PHldBs;

/////////////////////////////////////////////////
// HyperLinkDocument-Experiment
class THldExpr{
public:
  static void PutMomHd(TOLx& Lx, const TStr& VarNm);
  static void PutMomVal(TOLx& Lx, const PMom& Mom);
  static void Go(
   const PSOut& SOut, const bool& PutHd, const bool& SaveTmp,
   const TStr& HldDmNm, const int& TsRuns,
   const TStrV& AllHlCtxStrV, const PXWebBs& DocWebBs,
   const int& MxNGram, const int& MnWordFq, const TSwSetTy& SwSetTy,
   const int& KNNbrs, const bool& RndSel,
   const PNotify& Notify);
};

#endif
