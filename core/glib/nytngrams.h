#ifndef nytngrams_h
#define nytngrams_h

/////////////////////////////////////////////////
// Includes
#include "base.h"

/////////////////////////////////////////////////
// NYTimes-NGram-Base
ClassTP(TNytNGramBs, PNytNGramBs)//{
private:
  TStrVH TermStrVH;
  UndefCopyAssign(TNytNGramBs);
public:
  TNytNGramBs(const int& ExpectedNGrams=0):
    TermStrVH(ExpectedNGrams){}
  static PNytNGramBs New(const int& ExpectedNGrams=0){
    return new TNytNGramBs(ExpectedNGrams);}
  ~TNytNGramBs(){}
  TNytNGramBs(TSIn& SIn):
    TermStrVH(SIn){}
  static PNytNGramBs Load(TSIn& SIn){return new TNytNGramBs(SIn);}
  void Save(TSOut& SOut){
    TermStrVH.Save(SOut);}

  bool IsNGram(const TStrV& TermStrV, int& NGramId) const;
  TStr GetNGramStr(const int& NGramId) const;
  void GetNGramStrV(const TStr& HtmlStr, TStrV& NGramStrV);
  
  // files
  static PNytNGramBs LoadNytNGramBs(const TStr& InNGramsFPath, const int& ExpectedNGrams);
  static PNytNGramBs LoadBin(const TStr& FNm){
    TFIn SIn(FNm); return Load(SIn);}
  void SaveBin(const TStr& FNm){
    TFOut SOut(FNm); Save(SOut);}
  void SaveTxt(const TStr& FNm);
};

#endif