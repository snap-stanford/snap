#ifndef wordco_h
#define wordco_h

/////////////////////////////////////////////////
// Includes
#include "base.h"
#include "tmine.h"

/////////////////////////////////////////////////
// Word-Co-occurrence-Word
ClassTPV(TWCoWord, PWCoWord, TWCoWordV)//{
public:
  TInt WordN;
  TStr WordStr;
  TInt Fq;
  TStrIntPrV RelWordStrFqPrV;
public:
  TWCoWord(): WordN(-1), WordStr(), Fq(), RelWordStrFqPrV(){}
  static PWCoWord New(){return PWCoWord(new TWCoWord());}
  TWCoWord(TSIn& SIn):
    WordN(SIn), WordStr(SIn), Fq(SIn), RelWordStrFqPrV(SIn){}
  static PWCoWord Load(TSIn& SIn){return new TWCoWord(SIn);}
  void Save(TSOut& SOut){
    WordN.Save(SOut); WordStr.Save(SOut); Fq.Save(SOut);
    RelWordStrFqPrV.Save(SOut);}

  TWCoWord& operator=(const TWCoWord&){Fail; return *this;}

  // word
  int GetWordN() const {return WordN;}
  TStr GetWordStr() const {return WordStr;}
  int GetWordFq() const {return Fq;}

  // related-words
  int GetRelWords() const {return RelWordStrFqPrV.Len();}
  TStr GetRelWordStr(const int& RelWordN) const {
    return RelWordStrFqPrV[RelWordN].Val1;}
  int GetRelWordFq(const int& RelWordN) const {
    return RelWordStrFqPrV[RelWordN].Val2;}
};

/////////////////////////////////////////////////
// Word-Co-occurrence-Base
ClassTP(TWCoBs, PWCoBs)//{
public:
  TStrIntPrV WordStrFqPrV;
  THash<TStr, PWCoWord> WordStrToWCoWordH;
  void AssertOk() const;
public:
  TWCoBs(): WordStrFqPrV(), WordStrToWCoWordH(){}
  static PWCoBs New(){return PWCoBs(new TWCoBs());}
  TWCoBs(TSIn& SIn):
    WordStrFqPrV(SIn), WordStrToWCoWordH(SIn){}
  static PWCoBs Load(TSIn& SIn){return new TWCoBs(SIn);}
  void Save(TSOut& SOut) const {
    WordStrFqPrV.Save(SOut); WordStrToWCoWordH.Save(SOut);}

  TWCoBs& operator=(const TWCoBs&){Fail; return *this;}

  // words
  int GetWords() const {return WordStrFqPrV.Len();}
  TStr GetWordStr(const int& WordN) const {return WordStrFqPrV[WordN].Val1;}
  int GetWordFq(const int& WordN) const {return WordStrFqPrV[WordN].Val2;}
  int GetWordN(const TStr& WordStr) const {
    return WordStrToWCoWordH.GetDat(WordStr)->GetWordN();}
  PWCoWord GetWord(const int& WordN) const {
    return WordStrToWCoWordH.GetDat(GetWordStr(WordN));}

  // bag-of-words
  PBowDocBs GetBowDocBs() const;

  // binary files
  static PWCoBs LoadBin(const TStr& FNm){
    TFIn SIn(FNm); return Load(SIn);}
  void SaveBin(const TStr& FNm) const {
    TFOut SOut(FNm); Save(SOut);}

  // NlpWin
  static void GetNlpWinWordRel(const PXmlTok& Tok, const TStr& NodeTypeNm,
   const int& Lev, TStrV& HeadStrV, TStrPrV& WordStrPrV, TStrTrV& WordStrTrV);
  static void GetNlpWinWordRel(const PXmlDoc& NlpWinXmlDoc,
   TStr& SentStr, TStrPrV& WordStrPrV, TStrTrV& WordStrTrV);
  static PXmlDoc GetNextNlpWinXmlDoc(const PSIn& NlpWinXmlSIn);
  static PWCoBs LoadNlpWinTxt(const TStr& FNm, const int& MxTrees=-1);
  static PWCoBs LoadNlpWinBinOrTxt(const TStr& BinFNm, const TStr& TxtFNm);

  // files
  void SaveTxt(const TStr& FNm) const;
  void SaveXml(const TStr& FNm) const;
};

#endif
