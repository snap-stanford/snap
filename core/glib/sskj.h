#ifndef sskj_h
#define sskj_h

/////////////////////////////////////////////////
// Includes
#include "base.h"

/////////////////////////////////////////////////
// Sskj-Lexical-Chars
typedef enum {
  hlctSpace, hlctAlpha, hlctNum, hlctSym,
  hlctLTag, hlctRTag, hlctEof} TSskjLxChTy;

class TSskjLxChDef{
private:
  TIntV ChTyV;
  TChV UcChV;
  TStrStrH EscStrH;
  void SetUcCh(const TStr& Str);
  void SetChTy(const TSskjLxChTy& ChTy, const TStr& Str);
  void SetEscStr(const TStr& SrcStr, const TStr& DstStr);
public:
  TSskjLxChDef();

  TSskjLxChDef& operator=(const TSskjLxChDef&){Fail; return *this;}

  int GetChTy(const char& Ch){return ChTyV[Ch-TCh::Mn];}
  bool IsEoln(const char& Ch){return (Ch==CrCh)||(Ch==LfCh);}
  bool IsWs(const char& Ch){
    return (Ch==' ')||(Ch==TabCh)||(Ch==CrCh)||(Ch==LfCh);}
  bool IsSpace(const char& Ch){return ChTyV[Ch-TCh::Mn]==hlctSpace;}
  bool IsAlpha(const char& Ch){return ChTyV[Ch-TCh::Mn]==hlctAlpha;}
  bool IsNum(const char& Ch){return ChTyV[Ch-TCh::Mn]==hlctNum;}
  bool IsAlNum(const char& Ch){
    return (ChTyV[Ch-TCh::Mn]==hlctAlpha)||(ChTyV[Ch-TCh::Mn]==hlctNum);}
  bool IsSym(const char& Ch){return ChTyV[Ch-TCh::Mn]==hlctSym;}
  char GetUc(const char& Ch){return UcChV[Ch-TCh::Mn];}
  TStr GetEscStr(const TStr& Str);
};

/////////////////////////////////////////////////
// Sskj-Lexical
typedef enum {
  hlsyUndef, hlsyStr, hlsyNum, hlsySSym,
  hlsyTag, hlsyEof} TSskjLxSym;

class TSskjLx{
private:
  static TSskjLxChDef ChDef;
  PSIn SIn;
  TChA ChStack;
  char Ch;
  void PutCh(const char& _Ch){ChStack.Push(Ch); Ch=_Ch;}
  void PutStr(const TStr& Str){
    for (int ChN=Str.Len()-1; ChN>=0; ChN--){PutCh(Str[ChN]);}}
  void GetCh();
public:
  TSskjLxSym Sym;
  TChA Str;
  TChA UcStr;
public:
  TSskjLx(const PSIn& _SIn):
    SIn(_SIn), ChStack(), Ch(' '),
    Sym(hlsyUndef), Str(), UcStr(){}

  TSskjLx& operator=(const TSskjLx&){Fail; return *this;}

  TSskjLxSym GetSym();
};

#endif
