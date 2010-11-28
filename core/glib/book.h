#ifndef book_h
#define book_h

/////////////////////////////////////////////////
// Includes
#include "base.h"
#include "bix.h"

/////////////////////////////////////////////////
// Book-Table-Of-Contents
class TBookToc;
typedef TPt<TBookToc> PBookToc;

class TBookToc{
private:
  TCRef CRef;
private:
  TStrV LevNmV;
  TIntStrIntTrV LevTitleSecIdV;
public:
  TBookToc(){}
  static PBookToc New(){return PBookToc(new TBookToc());}
  ~TBookToc(){}
  TBookToc(TSIn&){}
  static PBookToc Load(TSIn&){Fail; return NULL;}
  virtual void Save(TSOut&){Fail;}

  TBookToc& operator=(const TBookToc&){Fail; return *this;}

  // hierarcical levels
  void AddLevNm(const TStr& LevNm){LevNmV.Add(LevNm);}
  int GetLevs() const {return LevNmV.Len();}
  TStr GetLevNm(const int& LevN) const {return LevNmV[LevN];}

  // toc items
  void AddItem(const int& LevN, const TStr& TitleNm, const int& SecId){
    IAssert((0<=LevN)&&(LevN<LevNmV.Len()));
    LevTitleSecIdV.Add(TIntStrIntTr(LevN, TitleNm, SecId));}
  int GetItems() const {return LevTitleSecIdV.Len();}
  void GetItem(const int ItemN, int& LevN, TStr& TitleNm, int& SecId) const {
    LevN=LevTitleSecIdV[ItemN].Val1;
    TitleNm=LevTitleSecIdV[ItemN].Val2;
    SecId=LevTitleSecIdV[ItemN].Val3;
  }

  friend PBookToc;
};

/////////////////////////////////////////////////
// Book
class TBook;
typedef TPt<TBook> PBook;

class TBook{
private:
  TCRef CRef;
private:
  typedef PBook (*TBookLoad)(TSIn& SIn);
  typedef TFunc<TBookLoad> TBookLoadF;
  static THash<TStr, TBookLoadF> TypeToLoadFH;
private:
  TStr TitleStr;
  TStr AuthorNm;
  TStr CtgNm;
public:
  TBook(const TStr& _TitleStr, const TStr& _AuthorNm, const TStr& _CtgNm):
    TitleStr(_TitleStr), AuthorNm(_AuthorNm), CtgNm(_CtgNm){}
  virtual ~TBook(){}
  TBook(TSIn& SIn):
    TitleStr(SIn), AuthorNm(SIn), CtgNm(SIn){}
  static PBook Load(TSIn& SIn);
  static PBook Load(const TStr& FNm){
    PSIn SIn=TFIn::New(FNm); return TBook::Load(*SIn);}
  virtual void Save(TSOut& SOut){
    GetTypeNm(*this).Save(SOut);
    TitleStr.Save(SOut); AuthorNm.Save(SOut); CtgNm.Save(SOut);}
  void Save(const TStr& FNm){
    PSOut SOut=TFOut::New(FNm); Save(*SOut);}
  static bool Reg(const TStr& TypeNm, const TBookLoadF& LoadF);

  TBook& operator=(const TBook& Book){
    TitleStr=Book.TitleStr; AuthorNm=Book.AuthorNm; CtgNm=Book.CtgNm;
    return *this;}

  TStr GetTitleStr() const {return TitleStr;}
  TStr GetAuthorNm() const {return AuthorNm;}
  TStr GetCtgNm() const {return CtgNm;}

  virtual PBookToc GetBookToc() const=0;

  virtual int GetSecs() const=0;
  virtual void GetSecInfo(
   const int& SecId, TStr& SecIdStr, TStr& TitleStr, TStr& SecStr) const=0;
  virtual PBixRes Search(const TStr& QueryStr) const=0;

  friend PBook;
};

/////////////////////////////////////////////////
// Bible-Psalm
class TBiblePsalm{
private:
  TStr ChpNm;
  TInt SecN;
  TInt SSecN;
  TStr SSecStr;
public:
  TBiblePsalm(): ChpNm(), SecN(0), SSecN(0), SSecStr(){}
  TBiblePsalm(const TStr _ChpNm, const int& _SecN, const int& _SSecN,
   const TStr& _SSecStr):
    ChpNm(_ChpNm), SecN(_SecN), SSecN(_SSecN), SSecStr(_SSecStr){}
  TBiblePsalm(TSIn& SIn):
    ChpNm(SIn), SecN(SIn), SSecN(SIn), SSecStr(SIn){}
  void Save(TSOut& SOut){
    ChpNm.Save(SOut); SecN.Save(SOut); SSecN.Save(SOut); SSecStr.Save(SOut);}

  TBiblePsalm& operator=(const TBiblePsalm& Psalm){
    ChpNm=Psalm.ChpNm; SecN=Psalm.SecN; SSecN=Psalm.SSecN;
    SSecStr=Psalm.SSecStr; return *this;}

  TStr GetChpNm() const {return ChpNm;}
  int GetSecN() const {return SecN;}
  int GetSSecN() const {return SSecN;}
  TStr GetSSecStr() const {return SSecStr;}
};
typedef TVec<TBiblePsalm> TBiblePsalmV;

/////////////////////////////////////////////////
// Bible
class TBible: public TBook{
private:
  static bool IsReg;
  static bool MkReg(){
    return TBook::Reg(TTypeNm<TBible>(), TBookLoadF(&Load));}
private:
  TStrIntKdV ChpNmSecIdKdV;
  TBiblePsalmV PsalmV;
  PBix Bix;
  void AddPsalm(
   const TStr& ChpNm, const int& SecN, const int& SSecN, const TStr& SSecStr);
public:
  TBible(const TStr& FNm);
  TBible(TSIn& SIn):
    TBook(SIn), ChpNmSecIdKdV(SIn), PsalmV(SIn), Bix(SIn){}
  static PBook Load(TSIn& SIn){return PBook(new TBible(SIn));}
  void Save(TSOut& SOut){
    TBook::Save(SOut); ChpNmSecIdKdV.Save(SOut); PsalmV.Save(SOut);
    Bix.Save(SOut);}

  TBible& operator=(const TBible&){Fail; return *this;}

  PBookToc GetBookToc() const;
  int GetSecs() const {return PsalmV.Len();}
  void GetSecInfo(
   const int& SecId, TStr& SecIdStr, TStr& TitleStr, TStr& SecStr) const;
  PBixRes Search(const TStr& QueryStr) const {
    return Bix->Search(QueryStr);}
};

#endif

