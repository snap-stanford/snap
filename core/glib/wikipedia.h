#ifndef wikipedia_h
#define wikipedia_h

/////////////////////////////////////////////////
// Includes
#include "mine.h"

/////////////////////////////////////////////////
// Wikipedia-Document
ClassTP(TWikiDoc, PWikiDoc)//{
private:
  UndefCopyAssign(TWikiDoc);
public:
  TWikiDoc(){}
  static PWikiDoc New(){
    return new TWikiDoc();}
  TWikiDoc(TSIn& SIn){}
  static PWikiDoc Load(TSIn& SIn){return new TWikiDoc(SIn);}
  void Save(TSOut& SOut) const {}

  // extraction
  static PWikiDoc LoadXml(const PXmlDoc& XmlDoc);

  // files
  static PWikiDoc LoadBin(const TStr& FNm);
  void SaveBin(const TStr& FNm);
};

/////////////////////////////////////////////////
// Wikipedia-Base
ClassTP(TWikiBs, PWikiBs)//{
private:
  UndefCopyAssign(TWikiBs);
public:
  TWikiBs(){}
  static PWikiBs New(){
    return new TWikiBs();}
  TWikiBs(TSIn& SIn){}
  static PWikiBs Load(TSIn& SIn){return new TWikiBs(SIn);}
  void Save(TSOut& SOut) const {}

  // files
  static PWikiBs LoadBin(const TStr& FNm);
  void SaveBin(const TStr& FNm);
  static PWikiBs LoadXml(const TStr& FNm);
};

#endif
