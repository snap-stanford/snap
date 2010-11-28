#ifndef tql_h
#define tql_h

/////////////////////////////////////////////////
// Includes
#include "base.h"

/////////////////////////////////////////////////
// Tql-Lexical-Chars
typedef enum {tlcSpace, tlcAlpha, tlcNum, tlcSym, tlcEof} TTqlLxChTy;

class TTqlChDef{
private:
  TIntV ChTyV;
  void SetChTy(const TTqlLxChTy& ChTy, const TStr& Str);
public:
  TTqlChDef();

  TTqlChDef& operator=(const TTqlChDef&){Fail; return *this;}

  int GetChTy(const char& Ch) const {return ChTyV[Ch-TCh::Mn];}
  bool IsSpace(const char& Ch) const {return ChTyV[Ch-TCh::Mn]==tlcSpace;}
  bool IsAlpha(const char& Ch) const {return ChTyV[Ch-TCh::Mn]==tlcAlpha;}
  bool IsNum(const char& Ch) const {return ChTyV[Ch-TCh::Mn]==tlcNum;}
  bool IsAlNum(const char& Ch) const {return IsAlpha(Ch)||IsNum(Ch);}
  bool IsSym(const char& Ch) const {return ChTyV[Ch-TCh::Mn]==tlcSym;}
};

/////////////////////////////////////////////////
// Tql-Lexical-Reserved-Words
typedef enum {
  trwDomain, trwDepth, trwFeature, trwOutgoingUrl, trwTitle,
  trwAfter, trwBefore, trwWithin} TTqlRw;

class TTqlRwMap{
private:
  TStrIntH StrToRwH;
public:
  TTqlRwMap();

  TTqlRwMap& operator=(const TTqlRwMap&){Fail; return *this;}

  bool IsRw(const TStr& RwStr, TTqlRw& Rw) const;
};

/////////////////////////////////////////////////
// Tql-Lexical
typedef enum {
  tsyUndef, tsyWord, tsyNum, tsySSym, // general symbols
  tsyDQuote, tsyColon, // auxilary symbols
  tsyLParen, tsyRParen, tsyLBracket, tsyRBracket, // parenthesis
  tsyOr, tsyAnd, tsyNot, // logical operators
  tsyIncl, tsyExcl, tsyTitle, tsyWCard, // extra operators
  tsyLss, tsyLEq, tsyEq, tsyNEq, tsyGEq, tsyGtr, // relational operator
  tsyEof} TTqlLxSym;

class TTqlLx{
private:
  static TTqlChDef ChDef;
  PSIn SIn;
  char Ch;
  void GetCh(){Ch=(SIn->Eof()) ? TCh::EofCh : SIn->GetCh();}
public:
  TTqlLxSym Sym;
  TChA Str;
public:
  TTqlLx(const TStr& ExpStr);

  TTqlLx& operator=(const TTqlLx&){Fail; return *this;}

  TTqlLxSym GetSym(const TFSet& Expect);
  TTqlLxSym GetSym(){return GetSym(TFSet());}
  TTqlLxSym GetSym(const TTqlLxSym& Sym){return GetSym(TFSet()|Sym);}
  TTqlLxSym GetSym(const TTqlLxSym& Sym1, const TTqlLxSym& Sym2){
    return GetSym(TFSet()|Sym1|Sym2);}
  TTqlLxSym GetSym(const TTqlLxSym& Sym1, const TTqlLxSym& Sym2,
   const TTqlLxSym& Sym3){
    return GetSym(TFSet()|Sym1|Sym2|Sym3);}
};

/////////////////////////////////////////////////
// Tql-Context-Set
class TTqlCtxSet;
typedef TPt<TTqlCtxSet> PTqlCtxSet;

class TTqlCtxSet{
private:
  TCRef CRef;
public:
  TTqlCtxSet(){}
  virtual ~TTqlCtxSet(){}
  TTqlCtxSet(TSIn&){}
  static PTqlCtxSet Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){}

  TTqlCtxSet& operator=(const TTqlCtxSet&){return *this;}

  virtual int GetCtxs() const=0;
  virtual int GetCtxSecId(const int& CtxN) const=0;

  virtual PTqlCtxSet GetAndCtxSet(const PTqlCtxSet& CtxSet)=0;
  virtual PTqlCtxSet GetOrCtxSet(const PTqlCtxSet& CtxSet)=0;
  virtual PTqlCtxSet GetTagCtxSet(const TStr& TagNm)=0;
  virtual PTqlCtxSet GetTitleCtxSet()=0;

  friend PTqlCtxSet;
};

/////////////////////////////////////////////////
// Tql-Index
class TTqlIx{
public:
  TTqlIx(){}
  virtual ~TTqlIx(){}
  TTqlIx(TSIn&){}
  void Save(TSOut&){}

  TTqlIx& operator=(const TTqlIx&){return *this;}

  virtual PTqlCtxSet GetEmptyCtxSet() const=0;
  virtual PTqlCtxSet GetCtxSet(
   const TStr& WordStr, const bool& IsWc, const int& MxWcWords) const=0;
  virtual PTqlCtxSet GetCtxSet(
   const TStr& TagNm, const TRelOp& RelOp, const TStr& ArgStr,
   const int& MxWcWords) const=0;
};

/////////////////////////////////////////////////
// Tql-Item
class TTqlItem;
typedef TPt<TTqlItem> PTqlItem;
typedef TVec<PTqlItem> TTqlItemV;

typedef enum {
  titUndef,
  titOr, titAnd, titNot, titIncl, titExcl, titInTag, titTagRel,
  titTitle, titWCard, titWord, titPhrase, titMeta} TTqlItemType;

class TTqlItem{
private:
  TCRef CRef;
private:
  TTqlItemType Type;
  TTqlItemV ItemV;
  TStrV WordStrV;
public:
  TTqlItem():
    Type(titUndef), ItemV(), WordStrV(){}
  TTqlItem(const TTqlItemType& _Type,
   const PTqlItem& Item1, const PTqlItem& Item2=NULL);
  TTqlItem(const TStr& TagNm, const PTqlItem& Item);
  TTqlItem(const TStr& TagNm, const TStr& RelOpStr, const TStr& ArgStr);
  TTqlItem(const TStr& WordStr);
  TTqlItem(const TStrV& WordStrV);
  TTqlItem(const TStr& MetaTag, const TStrV& ArgStrV);
  ~TTqlItem(){}
  TTqlItem(TSIn&){Fail;}
  static PTqlItem Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TTqlItem& operator=(const TTqlItem&){Fail; return *this;}

  TTqlItemType GetType() const {return Type;}
  PTqlItem GetItem(const int& ItemN=0) const;
  TStr GetTagNm() const;
  TRelOp GetRelOp() const;
  TStr GetRelArgStr() const;
  TStr GetWordStr() const;
  int GetPhraseLen() const;
  TStr GetPhraseWordStr(const int& WordStrN) const;

  PTqlCtxSet Eval(const TTqlIx& TqlIx) const;

  friend PTqlItem;
};

/////////////////////////////////////////////////
// Tql-Expression
class TTqlExp;
typedef TPt<TTqlExp> PTqlExp;

class TTqlExp{
private:
  TCRef CRef;
private:
  TStr ExpStr;
  bool Ok;
  TStr ErrMsg;
  PTqlItem Item;
private:
  TFSet RelOpSet, FactExpect, TermExpect, ExpExpect;
  PTqlItem ParseFact(TTqlLx& Lx, const TFSet& Expect);
  PTqlItem ParseTerm(TTqlLx& Lx, const TFSet& Expect);
  PTqlItem ParseExp(TTqlLx& Lx, const TFSet& Expect);
public:
  TTqlExp(const TStr& _ExpStr);
  ~TTqlExp(){}
  TTqlExp(TSIn&){Fail;}
  static PTqlExp Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TTqlExp& operator=(const TTqlExp&){Fail; return *this;}

  bool IsOk() const {return Ok;}
  TStr GetErrMsg() const {return ErrMsg;}

  PTqlCtxSet Eval(const TTqlIx& TqlIx) const {
    IAssert(IsOk()); return Item->Eval(TqlIx);}

  static PTqlExp New(const TStr& ExpStr){
    return PTqlExp(new TTqlExp(ExpStr));}

  friend PTqlExp;
};

#endif
