#ifndef tagcloud_h
#define tagcloud_h

/////////////////////////////////////////////////
// Includes
#include "mine.h"
#include "gks.h"

/////////////////////////////////////////////////
// Tag-Cloud
ClassTP(TTagCloud, PTagCloud)//{
private:
  TStrFltPrV WordStrWgtPrV;
  TFltQu RectV;
  UndefCopyAssign(TTagCloud);
public:
  TTagCloud():
    WordStrWgtPrV(){}
  static PTagCloud TTagCloud::New(){return new TTagCloud();}
  ~TTagCloud(){}
  TTagCloud(TSIn& SIn):
    WordStrWgtPrV(SIn){}
  static PTagCloud Load(TSIn& SIn){return new TTagCloud(SIn);}
  void Save(TSOut& SOut) const {
    WordStrWgtPrV.Save(SOut);}

  // create
  static PTagCloud GetFromDocStrWgtPrV(const TStrFltPrV& DocStrWgtPrV, 
   const int& TopWords, const double& TopWordsWgtSumPrc);
  void PlaceWords();
   
  // files
  static PTagCloud LoadBin(const TStr& FNm){
    TFIn SIn(FNm); return Load(SIn);}
  void SaveBin(const TStr& FNm){
    TFOut SOut(FNm); Save(SOut);}
  void Dump();
};

#endif