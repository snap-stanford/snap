/////////////////////////////////////////////////
// Wix-Expression-Lexical-Chars
typedef enum {
  welctSpace, welctAlpha, welctNum, welctSym, welctEof} TWixExpLxChTy;

class TWixExpLxChDef{
private:
  TIntV ChTyV;
  void SetChTy(const TWixExpLxChTy& ChTy, const TStr& Str);
public:
  TWixExpLxChDef();

  TWixExpLxChDef& operator=(const TWixExpLxChDef&){Fail; return *this;}

  int GetChTy(const char& Ch){return ChTyV[Ch-TCh::Mn];}
  bool IsSpace(const char& Ch){return ChTyV[Ch-TCh::Mn]==welctSpace;}
  bool IsAlpha(const char& Ch){return ChTyV[Ch-TCh::Mn]==welctAlpha;}
  bool IsNum(const char& Ch){return ChTyV[Ch-TCh::Mn]==welctNum;}
  bool IsAlNum(const char& Ch){return IsAlpha(Ch)||IsNum(Ch);}
  bool IsSym(const char& Ch){return ChTyV[Ch-TCh::Mn]==welctSym;}
};

/////////////////////////////////////////////////
// Wix-Expression-Lexical-Keywords
typedef enum {
  wekDomain, wekDepth, wekFeature, wekOutgoingUrl, wekTitle,
  wekAfter, wekBefore, wekWithin
} TWixExpKw;

class TWixExpKwMap{
private:
  TStrIntH StrToKwH;
public:
  TWixExpKwMap();

  TWixExpKwMap& operator=(const TWixExpKwMap&){Fail; return *this;}

  bool IsExpKw(const TStr& ExpKwStr, TWixExpKw& ExpKw);
};

/////////////////////////////////////////////////
// Wix-Expression-Lexical
typedef enum {
  wesyUndef, wesyWord, wesyNum, wesySSym, // general symbols
  wesyDQuote, wesyColon, wesyLParen, wesyRParen, // auxilary symbols
  wesyOr, wesyAnd, wesyNot, // logical operators
  wesyIncl, wesyExcl, wesyWCard, // extra operators
  wesyEof} TWixExpLxSym;

class TWixExpLx{
private:
  static TWixExpLxChDef ChDef;
  PSIn SIn;
  char Ch;
  void GetCh(){Ch=(SIn->Eof()) ? TCh::EofCh : SIn->GetCh();}
public:
  TWixExpLxSym Sym;
  TChA Str;
public:
  TWixExpLx(const TStr& ExpStr);

  TWixExpLx& operator=(const TWixExpLx&){Fail; return *this;}

  TWixExpLxSym GetSym(const TFSet& Expect);
  TWixExpLxSym GetSym(){return GetSym(TFSet());}
  TWixExpLxSym GetSym(const TWixExpLxSym& Sym){return GetSym(TFSet()|Sym);}
  TWixExpLxSym GetSym(const TWixExpLxSym& Sym1, const TWixExpLxSym& Sym2){
    return GetSym(TFSet()|Sym1|Sym2);}
  TWixExpLxSym GetSym(const TWixExpLxSym& Sym1, const TWixExpLxSym& Sym2,
   const TWixExpLxSym& Sym3){
    return GetSym(TFSet()|Sym1|Sym2|Sym3);}
};

/////////////////////////////////////////////////
// Word-Inverted-Index-Document-Id-Set
ClassTP(TWixExpDocIdSet, PWixExpDocIdSet)//{
private:
  TBlobPtV DocIdV;
  bool NegatedP;
public:
  TWixExpDocIdSet():
    DocIdV(), NegatedP(false){}
  TWixExpDocIdSet(const TBlobPtV& _DocIdV, const bool& _NegatedP=false):
    DocIdV(_DocIdV), NegatedP(_NegatedP){}
  ~TWixExpDocIdSet(){}
  TWixExpDocIdSet(TSIn&){Fail;}
  static PWixExpDocIdSet Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){}

  TWixExpDocIdSet& operator=(const TWixExpDocIdSet&){Fail; return *this;}

  TBlobPtV& GetDocIdV(){return DocIdV;}
  int GetDocIds() const {return DocIdV.Len();}
  TBlobPt GetDocId(const int& DocIdN) const {return DocIdV[DocIdN];}
  void PutNegated(const bool& _NegatedP){NegatedP=_NegatedP;}
  bool IsNegated() const {return NegatedP;}

  void GetDocIdV(TBlobPtV& _DocIdV){_DocIdV=DocIdV;}
  PWixExpDocIdSet AndDocSet(const PWixExpDocIdSet& DocSet);
  PWixExpDocIdSet OrDocSet(const PWixExpDocIdSet& DocSet);
};

/////////////////////////////////////////////////
// Wix-Expression-Item
typedef enum {
  weitUndef,
  weitOr, weitAnd, weitNot, weitIncl, weitExcl, weitWCard,
  weitWord, weitPhrase, weitMeta} TWixExpItemType;

ClassTPV(TWixExpItem, PWixExpItem, TWixExpItemV)//{
private:
  TWixExpItemType Type;
  TWixExpItemV ExpItemV;
  TStrV WordStrV;
public:
  TWixExpItem():
    Type(weitUndef), ExpItemV(), WordStrV(){}
  TWixExpItem(const TWixExpItemType& _Type,
   const PWixExpItem& ExpItem1, const PWixExpItem& ExpItem2=NULL);
  TWixExpItem(const TStr& WordStr);
  TWixExpItem(const TStrV& WordStrV);
  TWixExpItem(const TStr& MetaTag, const TStrV& ArgStrV);
  ~TWixExpItem(){}
  TWixExpItem(TSIn&){Fail;}
  static PWixExpItem Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TWixExpItem& operator=(const TWixExpItem&){Fail; return *this;}

  TWixExpItemType GetType(){return Type;}
  PWixExpItem GetExpItem(const int& ExpItemN=0);
  TStr GetWordStr();
  int GetPhraseLen();
  TStr GetPhraseWordStr(const int& WordStrN);

  PWixExpDocIdSet Eval(const PWix& Wix);
};

/////////////////////////////////////////////////
// Wix-Expression
ClassTP(TWixExp, PWixExp)//{
private:
  TStr ExpStr;
  bool Ok;
  TStr ErrMsg;
  PWixExpItem ExpItem;
private:
  TFSet FactExpect, TermExpect, ExpExpect;
  PWixExpItem ParseFact(TWixExpLx& Lx, const TFSet& Expect);
  PWixExpItem ParseTerm(TWixExpLx& Lx, const TFSet& Expect);
  PWixExpItem ParseExp(TWixExpLx& Lx, const TFSet& Expect);
public:
  TWixExp(const TWixExp&);
  TWixExp(const TStr& _ExpStr);
  static PWixExp New(const TStr& ExpStr){
    return PWixExp(new TWixExp(ExpStr));}
  ~TWixExp(){}
  TWixExp(TSIn&){Fail;}
  static PWixExp Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TWixExp& operator=(const TWixExp&){Fail; return *this;}

  bool IsOk() const {return Ok;}
  TStr GetErrMsg() const {return ErrMsg;}

  PWixExpDocIdSet Eval(const PWix& Wix);
};

