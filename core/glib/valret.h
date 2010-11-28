/////////////////////////////////////////////////
// Value-Retriever
ClassTP(TValRet, PValRet)//{
private:
  PDmHd DmHd;
  UndefDefaultCopyAssign(TValRet);
public:
  TValRet(const PDmHd& _DmHd): DmHd(_DmHd){}
  virtual ~TValRet(){}
  static PValRet Load(TSIn&){Fail; return NULL;}
  virtual void Save(TSOut&){Fail;}

  PDmHd GetDmHd() const {return DmHd;}
  int GetAttrs() const {return DmHd->GetAttrs();}

  virtual TTbVal GetVal(const int& AttrN) const=0;
  double GetNrmFlt(const int& AttrN) const {
    return DmHd->GetAttr(AttrN)->GetVarType()->GetNrmFlt(GetVal(AttrN));}

  virtual int FFirstAttrN() const=0;
  virtual bool FNextAttrN(int& AttrP, int& AttrN, TTbVal& AttrVal) const=0;
};

/////////////////////////////////////////////////
// Domain-Value-Retriever
class TDmValRet: public TValRet{
private:
  PDm Dm;
  int ExN;
public:
  TDmValRet(const PDmHd& _DmHd, const PDm& _Dm):
    TValRet(_DmHd), Dm(_Dm), ExN(-1){
    Assert(*GetDmHd()==*Dm->GetDmHd());}

  TTbVal GetVal(const int& AttrN) const {
    return Dm->GetAttrVal(ExN, AttrN);}

  int FFirstAttrN() const {return 0-1;}
  bool FNextAttrN(int& AttrP, int& AttrN, TTbVal& AttrVal) const {
    if (++AttrP<GetAttrs()){AttrN=AttrP; AttrVal=GetVal(AttrN); return true;} 
    else {return false;}
  }
    

  void SetExN(const int&  _ExN){ExN=_ExN;}
  int GetExN() const {return ExN;}
};

/////////////////////////////////////////////////
// Overloaded-Value-Retriever
class TOvlValRet: public TValRet{
private:
  PValRet ValRet;
  TIntV AttrToAttrV;
public:
  TOvlValRet(const PDmHd& _DmHd, const PValRet& _ValRet);

  TTbVal GetVal(const int& AttrN) const {
    return ValRet->GetVal(AttrToAttrV[AttrN]);}

  int FFirstAttrN() const {return ValRet->FFirstAttrN();}
  bool FNextAttrN(int& AttrP, int& AttrN, TTbVal& AttrVal) const {
    return ValRet->FNextAttrN(AttrP, AttrN, AttrVal);}
};

