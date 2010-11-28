#ifndef YahooBs_h
#define YahooBs_h

/////////////////////////////////////////////////
// Includes
#include "base.h"
#include "dmdata.h"
#include "stopword.h"
#include "webold.h"
#include "webmb.h"
#include "hldoc.h"

/////////////////////////////////////////////////
// Yahoo-Word
class TYBs;

class TYWord{
private:
  TYBs* YBs;
  TInt FirstChN;
public:
  TYWord(): YBs(NULL), FirstChN(-1){}
  TYWord(const TYWord& YWord): YBs(YWord.YBs), FirstChN(YWord.FirstChN){}
  TYWord(TYBs* _YBs, const TStr& Str);
  ~TYWord(){}
  TYWord(TSIn& SIn): YBs(NULL), FirstChN(SIn){}
  void Save(TSOut& SOut){FirstChN.Save(SOut);}

  TYWord& operator=(const TYWord& YWord){
    if (this!=&YWord){YBs=YWord.YBs; FirstChN=YWord.FirstChN;}
    return *this;}
  bool operator==(const TYWord& YWord) const {
    return (GetStr()==YWord.GetStr());}
  bool operator<(const TYWord& YWord) const {
    return GetStr()<YWord.GetStr();}

  TStr GetStr() const;
  int GetSegs() const;

  int GetPrimHashCd() const {return GetStr().GetPrimHashCd();}
  int GetSecHashCd() const {return GetStr().GetSecHashCd();}

  friend TYBs;
};

/////////////////////////////////////////////////
// Yahoo-Document
class TYDoc{
private:
  TCRef CRef;
private:
  TInt FirstPathCtgIdN;
  TInt FirstCtgIdN;
  TInt FirstReftoDocIdN;
  TInt FirstRefbyDocIdN;
  TInt FirstWordIdN;
  TInt Sects;
  TInt AbsSects;
public:
  TYDoc():
    FirstPathCtgIdN(-1), FirstCtgIdN(-1),
    FirstReftoDocIdN(-1), FirstRefbyDocIdN(-1),
    FirstWordIdN(-1), Sects(-1), AbsSects(-1){}
  ~TYDoc(){}
  TYDoc(TSIn& SIn):
    FirstPathCtgIdN(SIn), FirstCtgIdN(SIn),
    FirstReftoDocIdN(SIn), FirstRefbyDocIdN(SIn),
    FirstWordIdN(SIn), Sects(SIn), AbsSects(SIn){}
  static TPt<TYDoc> Load(TSIn& SIn){return new TYDoc(SIn);}
  void Save(TSOut& SOut){
    FirstPathCtgIdN.Save(SOut); FirstCtgIdN.Save(SOut);
    FirstReftoDocIdN.Save(SOut); FirstRefbyDocIdN.Save(SOut);
    FirstWordIdN.Save(SOut); Sects.Save(SOut); AbsSects.Save(SOut);}

  TYDoc& operator=(const TYDoc&){Fail; return *this;}

  friend TYBs;
  friend TPt<TYDoc>;
};
typedef TPt<TYDoc> PYDoc;

/////////////////////////////////////////////////
// Yahoo-Word-Distribution
class TYWordDs;
typedef TPt<TYWordDs> PYWordDs;

class TYWordDs{
private:
  TCRef CRef;
public:
  TInt Docs;
  TInt Sects;
  TFlt SumWordFq;
  TIntFltKdV WordIdFqKdV;
public:
  TYWordDs(const int& _Docs=0, const int& _Sects=0, const int& MxWordIds=100):
    Docs(_Docs), Sects(_Sects), SumWordFq(0), WordIdFqKdV(MxWordIds, 0){}
  ~TYWordDs(){}
  TYWordDs(TSIn& SIn):
    Docs(SIn),
    Sects(SIn),
    SumWordFq(SIn),
    WordIdFqKdV(SIn){}
  static PYWordDs Load(TSIn& SIn){return new TYWordDs(SIn);}
  void Save(TSOut& SOut){
    Docs.Save(SOut); Sects.Save(SOut); SumWordFq.Save(SOut);
    WordIdFqKdV.Save(SOut);}

  TYWordDs& operator=(const TYWordDs& YWordDs){
    if (this!=&YWordDs){
      Docs=YWordDs.Docs; Sects=YWordDs.Sects; SumWordFq=YWordDs.SumWordFq;
      WordIdFqKdV=YWordDs.WordIdFqKdV;}
    return *this;}

  int GetDocs() const {return Docs;}

  void AddSect(){Sects++;}
  void PutSects(const int& _Sects){Sects=_Sects;}
  int GetSects() const {return Sects;}

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
  bool FNextWordId(int& WordIdN, int& WordId, double& WordFq) const {
    double WordPrb; return FNextWordId(WordIdN, WordId, WordFq, WordPrb);}
  bool FNextWordId(int& WordIdN, int& WordId) const {
    double WordFq; double WordPrb;
    return FNextWordId(WordIdN, WordId, WordFq, WordPrb);}

  void NrToSumWordFq(const double& NrSumWordFq);
  void PowWordFq(const double& Exp);

  void SaveTxt(const PSOut& SOut, const TYBs* YBs=NULL, const int& DocId=-1);

  static PYWordDs GetMerged(const PYWordDs& LWordDs,
   const PYWordDs& RWordDs, const double& LWgt, const double& RWgt);
  static PYWordDs GetTruncByMnWordPrb(
   const PYWordDs& WordDs, const double& MnWordPrb);
  static PYWordDs GetTruncBySumWordPrb(
   const PYWordDs& WordDs, const double& SumWordPrb);

  friend PYWordDs;
};

/////////////////////////////////////////////////
// Yahoo-Section
class TYSect;
typedef TPt<TYSect> PYSect;
typedef TVec<PYSect> TYSectV;

class TYSect{
private:
  TCRef CRef;
private:
  TStr RefUrlStr;
  TStr UrlStr;
  PYWordDs WordDs;
  TStr TxtStr;
public:
  TYSect(const TStr& _RefUrlStr, const TStr& _UrlStr,
   const PYWordDs& _WordDs, const TStr& _TxtStr):
    RefUrlStr(_RefUrlStr), UrlStr(_UrlStr),
    WordDs(_WordDs), TxtStr(_TxtStr){}
  ~TYSect(){}
  TYSect(TSIn& SIn):
    RefUrlStr(SIn), UrlStr(SIn), WordDs(SIn), TxtStr(SIn){}
  static PYSect Load(TSIn& SIn){return new TYSect(SIn);}
  void Save(TSOut& SOut){
    RefUrlStr.Save(SOut); UrlStr.Save(SOut);
    WordDs.Save(SOut); TxtStr.Save(SOut);}

  TYSect& operator=(const TYSect&){Fail; return *this;}

  TStr GetRefUrlStr(){return RefUrlStr;}
  TStr GetUrlStr(){return UrlStr;}
  PYWordDs GetWordDs(){return WordDs;}
  TStr GetTxtStr(){return TxtStr;}

  friend PYSect;
};

/////////////////////////////////////////////////
// Yahoo-Base
class TYBs{
private:
  TCRef CRef;
private:
  static const TStr ParWordStr;
  PNotify Notify;
  TInt MxNGram;
  TInt MnWordFq;
  TInt ParWordId;
  TInt RootDocId;
  TStr RootUrlStr;
  TInt Sects;
  TInt AbsSects;
  PSwSet SwSet;
  TStrIntH CtgStrToFqH;
  THash<TYWord, TInt> WordToFqH;
  TChA WordChAHeap;
  THash<TStr, PYDoc> UrlStrToDocH;
  TIntV PathCtgIdVHeap;
  TIntV CtgIdVHeap;
  TIntV ReftoDocIdVHeap;
  TIntV RefbyDocIdVHeap;
  TIntV WordIdVHeap;
  void PutThisToWord();
  TStr GetStrQStr(const TStrQ& StrQ, const int& Strs=-1);
public:
  TYBs(const PXWebBs& WebBs,
   const int& _MxNGram=1, const int& _MnWordFq=4,
   const TSwSetTy& SwSetTy=swstNone, const TStrV& ExclUrlStrV=TStrV(),
   const PNotify& _Notify=NULL);
  ~TYBs(){}
  TYBs(TSIn& SIn, const PNotify& _Notify=NULL):
    Notify(_Notify),
    MxNGram(SIn), MnWordFq(SIn), ParWordId(SIn),
    RootDocId(SIn), RootUrlStr(SIn),
    Sects(SIn), AbsSects(SIn),
    SwSet(SIn), CtgStrToFqH(SIn),
    WordToFqH(SIn), WordChAHeap(SIn),
    UrlStrToDocH(SIn),
    PathCtgIdVHeap(SIn), CtgIdVHeap(SIn),
    ReftoDocIdVHeap(SIn), RefbyDocIdVHeap(SIn),
    WordIdVHeap(SIn){SIn.LoadCs(); PutThisToWord();}
  static TPt<TYBs> Load(TSIn& SIn){return new TYBs(SIn);}
  void Save(TSOut& SOut){
    MxNGram.Save(SOut); MnWordFq.Save(SOut); ParWordId.Save(SOut);
    RootDocId.Save(SOut); RootUrlStr.Save(SOut);
    Sects.Save(SOut); AbsSects.Save(SOut);
    SwSet.Save(SOut); CtgStrToFqH.Save(SOut);
    WordToFqH.Save(SOut); WordChAHeap.Save(SOut);
    UrlStrToDocH.Save(SOut);
    PathCtgIdVHeap.Save(SOut); CtgIdVHeap.Save(SOut);
    ReftoDocIdVHeap.Save(SOut); RefbyDocIdVHeap.Save(SOut);
    WordIdVHeap.Save(SOut); SOut.SaveCs();}
  void Save(const TStr& FNm){PSOut SOut=TFOut::New(FNm); Save(*SOut);}

  TYBs& operator=(const TYBs&){Fail; return *this;}

  bool IsIdEqN(){
    return CtgStrToFqH.IsKeyIdEqKeyN()&&
     WordToFqH.IsKeyIdEqKeyN()&&
     UrlStrToDocH.IsKeyIdEqKeyN();}

  void AddCtgStrWords(const TStr& CtgStr, TIntV& DocWordIdV);
  void AddDoc(
   const PXWebPg& WebPg, const bool& DoGenDoc,
   const int& NGram, const TStrV& ExclUrlStrV,
   TIntPrV& DocIdRefbyDocIdPrV);

  int GetCtgs(){return CtgStrToFqH.Len();}
  int GetCtgId(const TStr& CtgStr){return CtgStrToFqH.GetKeyId(CtgStr);}
  TStr GetCtgStr(const int& CtgId){return CtgStrToFqH.GetKey(CtgId);}
  int GetCtgFq(const int& CtgId){return CtgStrToFqH[CtgId];}
  TStr GetCtgIdVStr(const TIntV& CtgIdV);
  void GetCtgStrV(const TIntV& CtgIdV, TStrV& CtgStrV);
  TStr GetCtgIdToWFqHStr(const TIntFltH& CtgIdToWFqH, const double& SumPrb=1);
  int FFirstCtg(){return CtgStrToFqH.FFirstKeyId();}
  bool FNextCtg(int& CtgId){return CtgStrToFqH.FNextKeyId(CtgId);}

  int GetWords(){return WordToFqH.Len();}
  int GetMxWordIds(){return WordToFqH.GetMxKeyIds();}
  TInt& AddWord(const TStr& WordStr);
  int GetWordId(const TStr& WordStr);
  TStr GetWordStr(const int& WordId) const {
    return WordToFqH.GetKey(WordId).GetStr();}
  int GetWordFq(const int& WordId){return WordToFqH[WordId];}
  int GetWordSegs(const int& WordId){return WordToFqH.GetKey(WordId).GetSegs();}
  int GetParWordId(){return ParWordId;}
  int FFirstWordId(){return WordToFqH.FFirstKeyId();}
  bool FNextWordId(int& WordId){return WordToFqH.FNextKeyId(WordId);}

  int GetDocs(){return UrlStrToDocH.Len();}
  int GetMxDocIds(){return UrlStrToDocH.GetMxKeyIds();}
  int GetDocId(const TStr& UrlStr){return UrlStrToDocH.GetKeyId(UrlStr);}
  TStr GetDocUrlStr(const int& DocId) const {return UrlStrToDocH.GetKey(DocId);}
  PYDoc GetDoc(const int& DocId){return UrlStrToDocH[DocId];}
  int GetSects(){return Sects;}
  int GetAbsSects(){return AbsSects;}
  int GetDocSects(const int& DocId){return UrlStrToDocH[DocId]->Sects;}
  int GetDocAbsSects(const int& DocId){return UrlStrToDocH[DocId]->AbsSects;}
  int GetRootDocId(){return RootDocId;}
  TStr GetRootUrlStr(){return RootUrlStr;}
  void GetLevDocIdV(const int& Lev, TIntV& DocIdV);
  void GetCtgIdV(const int& DocId, TIntV& CtgIdV);
  void GetParentDocIdV(const int& DocId, TIntV& DocIdV);
  int GetDocDist(const int& LDocId, const int& RDocId);
  int FFirstDocId(){return UrlStrToDocH.FFirstKeyId();}
  bool FNextDocId(int& DocId){return UrlStrToDocH.FNextKeyId(DocId);}

  int FFirstDocPathCtgId(const int& DocId){
    return GetDoc(DocId)->FirstPathCtgIdN;}
  bool FNextDocPathCtgId(int& CtgIdN, int& CtgId){
    if (CtgIdN==-1){return false;}
    else {CtgId=PathCtgIdVHeap[CtgIdN++]; return CtgId!=-1;}}

  int FFirstDocCtgId(const int& DocId){
    return GetDoc(DocId)->FirstCtgIdN;}
  bool FNextDocCtgId(int& CtgIdN, int& CtgId){
    if (CtgIdN==-1){return false;}
    else {CtgId=CtgIdVHeap[CtgIdN++]; return CtgId!=-1;}}

  int FFirstDocWordId(const int& DocId){
    return GetDoc(DocId)->FirstWordIdN;}
  bool FNextDocWordId(int& WordIdN, int& WordId){
    if (WordIdN==-1){return false;}
    else {WordId=WordIdVHeap[WordIdN++]; return WordId!=-1;}}

  int GetDocReftos(const int& DocId);
  int FFirstDocReftoDocId(const int& DocId){
    return GetDoc(DocId)->FirstReftoDocIdN;}
  bool FNextDocReftoDocId(int& ReftoDocIdN, int& ReftoDocId){
    if (ReftoDocIdN==-1){return false;}
    else {ReftoDocId=ReftoDocIdVHeap[ReftoDocIdN++]; return ReftoDocId!=-1;}}

  int GetDocRefbys(const int& DocId);
  int FFirstDocRefbyDocId(const int& DocId){
    return GetDoc(DocId)->FirstRefbyDocIdN;}
  bool FNextDocRefbyDocId(int& RefbyDocIdN, int& RefbyDocId){
    if (RefbyDocIdN==-1){return false;}
    else {RefbyDocId=RefbyDocIdVHeap[RefbyDocIdN++]; return RefbyDocId!=-1;}}

  PYWordDs GetWordDs(const PSIn& SIn);
  PYWordDs GetWordDs(const TStr& Str){
    PSIn SIn=TStrIn::New(Str);
    return GetWordDs(SIn);}
  PYWordDs GetWordDs(const PXWebPg& WebPg){
    PSIn SIn=TStrIn::New(WebPg->GetHttpResp()->GetBodyAsStr());
    return GetWordDs(SIn);}

  void GetAbsSectV(
   const TStr& RefUrlStr, const PXWebPg& WebPg, TYSectV& YSectV);
  static void GetAbsSectUrlStrV(const PXWebPg& WebPg, TStrV& UrlStrV);

  void SaveTxt(const PSOut& SOut);
  void SaveTxt(const TStr& FNm){SaveTxt(PSOut(new TFOut(FNm)));}

  void SaveTxtWords(const TStr& FNm);
  void SaveTxtCtgs(const TStr& FNm);

  friend TYWord;
  friend TYDoc;
  friend TPt<TYBs>;
};
typedef TPt<TYBs> PYBs;

/////////////////////////////////////////////////
// Yahoo-Distribution-Base
typedef enum {ydnConst, ydnWords, ydnLnWords, ydnSects, ydnLnSects,
 ydnDocs, ydnLnDocs} TYDsBsNrType;

class TYDsBs{
private:
  TCRef CRef;
private:
  PYWordDs AllWordDs;
  THash<TInt, PYWordDs> DocIdToWordDsH;
  PNotify Notify;
  int GetMissDss(const PYBs& YBs, const int& DocId);
  double GetNrWgt(const TYDsBsNrType& NrType, const PYWordDs& WordDs);
public:
  TYDsBs(const TYDsBsNrType& NrType, const double& WordFqExp,
   const double& MnWordPrb, const double& MnAllWordPrb,
   const PYBs& YBs, const PNotify& _Notify);
  ~TYDsBs(){}
  TYDsBs(TSIn& SIn, const PNotify& _Notify=NULL):
    AllWordDs(SIn), DocIdToWordDsH(SIn), Notify(_Notify){SIn.LoadCs();}
  static TPt<TYDsBs> Load(TSIn& SIn){return new TYDsBs(SIn);}
  void Save(TSOut& SOut){
    AllWordDs.Save(SOut); DocIdToWordDsH.Save(SOut); SOut.SaveCs();}
  void Save(const TStr& FNm){PSOut SOut=TFOut::New(FNm); Save(*SOut);}

  TYDsBs& operator=(const TYDsBs&){Fail; return *this;}

  PYWordDs GetAllWordDs(){return AllWordDs;}
  PYWordDs GetWordDs(const int& DocId){return DocIdToWordDsH.GetDat(DocId);}

  void SaveTxt(const PSOut& SOut, const PYBs& YBs);
  static TStr GetNrTypeStr(const TYDsBsNrType& NrType);

  friend TPt<TYDsBs>;
};
typedef TPt<TYDsBs> PYDsBs;

/////////////////////////////////////////////////
// Yahoo-Test-Base
class TYTsBs{
private:
  TCRef CRef;
private:
  TStrStrH TsRefUrlStrH;
  PXWebBs WebBs;
  PXWebTravel WebTravel;
public:
  TYTsBs(
   const double& AllDocsPrb, const double& SectPrb, const TStr& TsWebBsFPath,
   const PXWebBs& RefWebBs, const PYBs& RefYBs,
   const PXWebTravelEvent& WebTravelEvent, const PNotify& Notify);
  ~TYTsBs();
  TYTsBs(TSIn& SIn){SIn.LoadCs();}
  static TPt<TYTsBs> Load(TSIn& SIn){return new TYTsBs(SIn);}
  void Save(TSOut& SOut){SOut.SaveCs();}

  TYTsBs& operator=(const TYTsBs&){Fail; return *this;}

  void SaveTxt(const PSOut& SOut, const PYBs& YBs);

  friend TPt<TYTsBs>;
};
typedef TPt<TYTsBs> PYTsBs;

/////////////////////////////////////////////////
// Yahoo-HyperLink-Context
class TYHlCtx{
private:
  TCRef CRef;
private:
  TStrV HlCtxStrV;
public:
  TYHlCtx(
   const PXWebBs& RefWebBs, const PYBs& RefYBs, const PXWebBs& DocWebBs,
   const PNotify& Notify);
  ~TYHlCtx(){}
  TYHlCtx(TSIn& SIn){SIn.LoadCs();}
  static TPt<TYHlCtx> Load(TSIn& SIn){return new TYHlCtx(SIn);}
  void Save(TSOut& SOut){SOut.SaveCs();}

  TYHlCtx& operator=(const TYHlCtx&){Fail; return *this;}

  TStrV& GetHlCtxStrV(){return HlCtxStrV;}

  friend TPt<TYHlCtx>;
};
typedef TPt<TYHlCtx> PYHlCtx;

#endif
