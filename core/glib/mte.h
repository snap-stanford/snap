#ifndef mte_h
#define mte_h

/////////////////////////////////////////////////
// Includes
#include "base.h"

/////////////////////////////////////////////////
// MulTextEast-Lexical-Chars
typedef enum {
  mtelcSpace, mtelcStr, mtelcEq, mtelcComma, mtelcEoln, mtelcEof} TMteLxChTy;

class TMteLxChDef{
private:
  TIntV ChTyV;
  TChV UcChV;
  void SetUcCh(const TStr& Str);
  void SetChTy(const TMteLxChTy& ChTy, const TStr& Str);
  UndefCopyAssign(TMteLxChDef);
public:
  TMteLxChDef();

  int GetChTy(const char& Ch) const {return ChTyV[Ch];}
  bool IsSpace(const char& Ch) const {return Ch<0?true:ChTyV[Ch]==mtelcSpace;}
  bool IsStr(const char& Ch) const {return ChTyV[Ch]==mtelcStr;}
  bool IsEq(const char& Ch) const {return ChTyV[Ch]==mtelcEq;}
  bool IsEoln(const char& Ch) const {return ChTyV[Ch]==mtelcEoln;}
  bool IsEof(const char& Ch) const {return ChTyV[Ch]==mtelcEof;}
  char GetUc(const char& Ch) const {return UcChV[Ch];}
};

/////////////////////////////////////////////////
// MulTextEast-Lexical
typedef enum {
  mtesyUndef, mtesyStr, mtesyEq, mtesyComma, mtesyEoln, mtesyEof} TMteLxSym;

class TMteLx{
private:
  TMteLxChDef ChDef;
  PSIn SIn;
  char Ch;
  char GetCh(){
    return (Ch=(SIn->Eof()) ? TCh::EofCh : SIn->GetCh());}
  //char GetCh(){
  //  Ch=(SIn->Eof()) ? TCh::EofCh : SIn->GetCh(); putchar(Ch); return Ch;}
public:
  TMteLxSym Sym;
  TChA ChA;
  TChA UcChA;
  UndefDefaultCopyAssign(TMteLx);
public:
  TMteLx(const PSIn& _SIn):
    ChDef(), SIn(_SIn), Ch(' '), Sym(mtesyUndef), ChA(), UcChA(){}

  TMteLxSym GetSym();
  TMteLxSym GetSym(const TMteLxSym& ESym){
    GetSym(); IAssert(Sym==ESym); return Sym;}
  TMteLxSym GetSym(const TMteLxSym& ESym1, const TMteLxSym& ESym2){
    GetSym(); IAssert((Sym==ESym1)||(Sym==ESym2)); return Sym;}
};

/////////////////////////////////////////////////
// MulTextEast-Lexicon
ClassTP(TMteLex, PMteLex)//{
private:
  TStrStrH InfWordToNrWordH;
  TStrIntVH NrWordToInfWordNVH;
  UndefDefaultCopyAssign(TMteLex);
public:
  TMteLex(const TStr& FNm, const int& Recs=-1);
  static PMteLex New(const TStr& FNm, const int& Recs=-1){
    return PMteLex(new TMteLex(FNm, Recs));}
  TMteLex(TSIn& SIn):
    InfWordToNrWordH(SIn), NrWordToInfWordNVH(SIn){}
  static PMteLex Load(TSIn& SIn){return new TMteLex(SIn);}
  void Save(TSOut& SOut){
    InfWordToNrWordH.Save(SOut); NrWordToInfWordNVH.Save(SOut);}

  // inflected words
  int GetInfWords() const {return InfWordToNrWordH.Len();}
  int GetInfWordN(const TStr& InfWordStr) const {
    return InfWordToNrWordH.GetKeyId(InfWordStr);}
  TStr GetInfWordStr(const int& InfWordN) const {
    return InfWordToNrWordH.GetKey(InfWordN);}
  bool IsInfWord(const TStr& InfWordStr) const {
    return InfWordToNrWordH.IsKey(InfWordStr);}
  bool IsInfWord(const TStr& InfWordStr, TStr& NrWordStr) const {
    return InfWordToNrWordH.IsKeyGetDat(InfWordStr, NrWordStr);}
  TStr GetNrWordFromInfWord(const TStr& InfWordStr) const {
    TStr NrWordStr;
    if (InfWordToNrWordH.IsKeyGetDat(InfWordStr, NrWordStr)){return NrWordStr;}
    else {return InfWordStr;}}
  void GetInfWordNrWordStr(
   const int& InfWordN, TStr& InfWordStr, TStr& NrWordStr) const {
    InfWordStr=InfWordToNrWordH.GetKey(InfWordN);
    NrWordStr=InfWordToNrWordH[InfWordN];}

  // normalized words
  int GetNrWords() const {return NrWordToInfWordNVH.Len();}
  int GetNrWordN(const TStr& NrWordStr) const {
    return NrWordToInfWordNVH.GetKeyId(NrWordStr);}
  TStr GetNrWordStr(const int& NrWordN) const {
    return NrWordToInfWordNVH.GetKey(NrWordN);}
  void GetInfWordNV(const TStr& NrWordStr, TIntV& InfWordNV) const {
    GetInfWordNV(GetNrWordN(NrWordStr), InfWordNV);}
  void GetInfWordNV(const int& NrWordN, TIntV& InfWordNV) const {
    InfWordNV=NrWordToInfWordNVH[NrWordN];}
  void GetSampleNrWordNV(
   TRnd& Rnd, const int& SampleLen, TIntV& SampleNrWordNV) const;

  // files
  static PMteLex LoadBin(const TStr& BinFNm);
  static PMteLex LoadBinOrTxt(const TStr& BinFNm, const TStr& TxtFNm);
  static PMteLex LoadBinOrTxt(const TStr& FPath){
    TStr NrFPath=TStr::GetNrFPath(FPath);
    return LoadBinOrTxt(NrFPath+"mte-sl.bin", NrFPath+"mte-sl.wfl");}

  // output
  void DumpWord(const int& NrWordN) const;
};

/////////////////////////////////////////////////
// MulTextEast-Stop-Words
ClassTP(TMteSw, PMteSw)//{
private:
  TStrH WordStrH;
  UndefDefaultCopyAssign(TMteSw);
public:
  TMteSw(const TStr& FNm);
  static PMteSw New(const TStr& FNm){return new TMteSw(FNm);}
  TMteSw(TSIn& SIn){Fail;}
  static PMteSw Load(TSIn& SIn){Fail; return NULL;}
  void Save(TSOut& SOut){Fail;}

  bool IsIn(const TStr& WordStr){return WordStrH.IsKey(WordStr);}

  void SaveTxt(const TStr& FNm, const bool& FmtCppP=false) const;
};

#endif
