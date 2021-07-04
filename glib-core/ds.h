/////////////////////////////////////////////////
// Address-Pointer
template <class TRec>
class TAPt{
private:
  TRec* Addr;
public:
  TAPt(): Addr(NULL){}
  TAPt(const TAPt& Pt): Addr(Pt.Addr){}
  TAPt(TRec* _Addr): Addr(_Addr){}
  TAPt(TSIn&){Fail;}
  void Save(TSOut&) const {Fail;}

  TAPt& operator=(const TAPt& Pt){Addr=Pt.Addr; return *this;}
  TAPt& operator=(TRec* _Addr){Addr=_Addr; return *this;}
  bool operator==(const TAPt& Pt) const {return *Addr==*Pt.Addr;}
  bool operator!=(const TAPt& Pt) const {return *Addr!=*Pt.Addr;}
  bool operator<(const TAPt& Pt) const {return *Addr<*Pt.Addr;}

  TRec* operator->() const {Assert(Addr!=NULL); return Addr;}
  TRec& operator*() const {Assert(Addr!=NULL); return *Addr;}
  TRec& operator[](const int& RecN) const {
    Assert(Addr!=NULL); return Addr[RecN];}
  TRec* operator()() const {return Addr;}

  bool Empty() const {return Addr==NULL;}
};

/////////////////////////////////////////////////
// Pair
template <class TVal1, class TVal2>
class TPair{
public:
  TVal1 Val1;
  TVal2 Val2;
public:
  TPair(): Val1(), Val2(){}
  TPair(const TPair& Pair): Val1(Pair.Val1), Val2(Pair.Val2){}
  TPair(const TVal1& _Val1, const TVal2& _Val2): Val1(_Val1), Val2(_Val2){}
  explicit TPair(TSIn& SIn): Val1(SIn), Val2(SIn){}
  void Save(TSOut& SOut) const {
    Val1.Save(SOut); Val2.Save(SOut);}
  void Load(TSIn& SIn) {Val1.Load(SIn); Val2.Load(SIn);}
  void LoadXml(const PXmlTok& XmlTok, const TStr& Nm="");
  void SaveXml(TSOut& SOut, const TStr& Nm) const;

  TPair& operator=(const TPair& Pair){
    if (this!=&Pair){Val1=Pair.Val1; Val2=Pair.Val2;} return *this;}
  bool operator==(const TPair& Pair) const {
    return (Val1==Pair.Val1)&&(Val2==Pair.Val2);}
  bool operator<(const TPair& Pair) const {
    return (Val1<Pair.Val1)||((Val1==Pair.Val1)&&(Val2<Pair.Val2));}

  int GetMemUsed() const {return Val1.GetMemUsed()+Val2.GetMemUsed();}

  int GetPrimHashCd() const {return TPairHashImpl::GetHashCd(Val1.GetPrimHashCd(), Val2.GetPrimHashCd()); }
  int GetSecHashCd() const {return TPairHashImpl::GetHashCd(Val2.GetSecHashCd(), Val1.GetSecHashCd()); }

  void GetVal(TVal1& _Val1, TVal2& _Val2) const {_Val1=Val1; _Val2=Val2;}
  const TVal1& GetVal1() const { return Val1;}
  const TVal2& GetVal2() const { return Val2;}
  TStr GetStr() const {
    return TStr("Pair(")+Val1.GetStr()+", "+Val2.GetStr()+")";}
};

template <class TVal1, class TVal2, class TSizeTy>
void GetSwitchedPrV(const TVec<TPair<TVal1, TVal2>, TSizeTy>& SrcPrV, TVec<TPair<TVal2, TVal1>, TSizeTy>& DstPrV){
  const TSizeTy Prs = SrcPrV.Len();
  DstPrV.Gen(Prs, 0);
  for (TSizeTy PrN=0; PrN<Prs; PrN++){
    const TPair<TVal1, TVal2>& SrcPr=SrcPrV[PrN];
    DstPrV.Add(TPair<TVal2, TVal1>(SrcPr.Val2, SrcPr.Val1));
  }
}

typedef TPair<TBool, TCh> TBoolChPr;
typedef TPair<TBool, TFlt> TBoolFltPr;
typedef TPair<TUCh, TInt> TUChIntPr;
typedef TPair<TUCh, TUInt64> TUChUInt64Pr;
typedef TPair<TUCh, TStr> TUChStrPr;
typedef TPair<TInt, TBool> TIntBoolPr;
typedef TPair<TInt, TCh> TIntChPr;
typedef TPair<TInt, TInt> TIntPr;
typedef TPair<TInt, TUInt64> TIntUInt64Pr;
typedef TPair<TInt, TIntPr> TIntIntPrPr;
typedef TPair<TInt, TVec<TInt, int> > TIntIntVPr;
typedef TPair<TInt, TFlt> TIntFltPr;
typedef TPair<TInt, TStr> TIntStrPr;
typedef TPair<TInt, TStrV> TIntStrVPr;
typedef TPair<TIntPr, TInt> TIntPrIntPr;
typedef TPair<TUInt, TUInt> TUIntUIntPr;
typedef TPair<TUInt, TInt> TUIntIntPr;
typedef TPair<TUInt64, TInt> TUInt64IntPr;
typedef TPair<TUInt64, TUInt64> TUInt64Pr;
typedef TPair<TUInt64, TFlt> TUInt64FltPr;
typedef TPair<TUInt64, TStr> TUInt64StrPr;
typedef TPair<TFlt, TInt> TFltIntPr;
typedef TPair<TFlt, TUInt64> TFltUInt64Pr;
typedef TPair<TFlt, TFlt> TFltPr;
typedef TPair<TFlt, TStr> TFltStrPr;
typedef TPair<TAscFlt, TInt> TAscFltIntPr;
typedef TPair<TAscFlt, TAscFlt> TAscFltPr;
typedef TPair<TFlt, TStr> TFltStrPr;
typedef TPair<TAscFlt, TStr> TAscFltStrPr;
typedef TPair<TStr, TInt> TStrIntPr;
typedef TPair<TStr, TFlt> TStrFltPr;
typedef TPair<TStr, TStr> TStrPr;
typedef TPair<TStr, TStrV> TStrStrVPr;
typedef TPair<TStrV, TInt> TStrVIntPr;
typedef TPair<TInt, TIntPr> TIntIntPrPr;
typedef TPair<TInt, TStrPr> TIntStrPrPr;
typedef TPair<TFlt, TStrPr> TFltStrPrPr;
typedef TPair<TVec<TInt, int>, TVec<TFlt, int> > TIntVFltVPr;

/// Compares the pair by the second value.
template <class TVal1, class TVal2>
class TCmpPairByVal2 {
private:
  bool IsAsc;
public:
  TCmpPairByVal2(const bool& AscSort=true) : IsAsc(AscSort) { }
  bool operator () (const TPair<TVal1, TVal2>& P1, const TPair<TVal1, TVal2>& P2) const {
    if (IsAsc) { return P1.Val2 < P2.Val2; } else { return P2.Val2 < P1.Val2; }
  }
};

/////////////////////////////////////////////////
// Triple
template <class TVal1, class TVal2, class TVal3>
class TTriple{
public:
  TVal1 Val1;
  TVal2 Val2;
  TVal3 Val3;
public:
  TTriple(): Val1(), Val2(), Val3(){}
  TTriple(const TTriple& Triple):
    Val1(Triple.Val1), Val2(Triple.Val2), Val3(Triple.Val3){}
  TTriple(const TVal1& _Val1, const TVal2& _Val2, const TVal3& _Val3):
    Val1(_Val1), Val2(_Val2), Val3(_Val3){}
  explicit TTriple(TSIn& SIn): Val1(SIn), Val2(SIn), Val3(SIn){}
  void Save(TSOut& SOut) const {
    Val1.Save(SOut); Val2.Save(SOut); Val3.Save(SOut);}
  void LoadXml(const PXmlTok& XmlTok, const TStr& Nm="");
  void SaveXml(TSOut& SOut, const TStr& Nm) const;

  TTriple& operator=(const TTriple& Triple){
    if (this!=&Triple){Val1=Triple.Val1; Val2=Triple.Val2; Val3=Triple.Val3;}
    return *this;}
  bool operator==(const TTriple& Triple) const {
    return (Val1==Triple.Val1)&&(Val2==Triple.Val2)&&(Val3==Triple.Val3);}
  bool operator<(const TTriple& Triple) const {
    return (Val1<Triple.Val1)||((Val1==Triple.Val1)&&(Val2<Triple.Val2))||
     ((Val1==Triple.Val1)&&(Val2==Triple.Val2)&&(Val3<Triple.Val3));}

  int GetPrimHashCd() const {return  TPairHashImpl::GetHashCd(TPairHashImpl::GetHashCd(Val1.GetPrimHashCd(), Val2.GetPrimHashCd()), Val3.GetPrimHashCd()); }
  int GetSecHashCd() const {return TPairHashImpl::GetHashCd(TPairHashImpl::GetHashCd(Val2.GetSecHashCd(), Val3.GetSecHashCd()), Val1.GetSecHashCd()); }
  int GetMemUsed() const {return Val1.GetMemUsed()+Val2.GetMemUsed()+Val3.GetMemUsed();}

  void GetVal(TVal1& _Val1, TVal2& _Val2, TVal3& _Val3) const {
    _Val1=Val1; _Val2=Val2; _Val3=Val3;}

  const TVal1& GetVal1() const { return Val1;}
  const TVal2& GetVal2() const { return Val2;}
  const TVal3& GetVal3() const { return Val3;}
};

typedef TTriple<TCh, TCh, TCh> TChTr;
typedef TTriple<TCh, TInt, TInt> TChIntIntTr;
typedef TTriple<TUCh, TInt, TInt> TUChIntIntTr;
typedef TTriple<TInt, TInt, TInt> TIntTr;
typedef TTriple<TUInt64, TUInt64, TUInt64> TUInt64Tr;
typedef TTriple<TInt, TStr, TInt> TIntStrIntTr;
typedef TTriple<TInt, TInt, TStr> TIntIntStrTr;
typedef TTriple<TInt, TInt, TFlt> TIntIntFltTr;
typedef TTriple<TInt, TFlt, TInt> TIntFltIntTr;
typedef TTriple<TInt, TFlt, TFlt> TIntFltFltTr;
typedef TTriple<TInt, TStr, TStr> TIntStrStrTr;
typedef TTriple<TInt, TVec<TInt, int>, TInt> TIntIntVIntTr;
typedef TTriple<TInt, TInt, TVec<TInt, int> > TIntIntIntVTr;
typedef TTriple<TFlt, TFlt, TFlt> TFltTr;
typedef TTriple<TFlt, TInt, TInt> TFltIntIntTr;
typedef TTriple<TFlt, TFlt, TInt> TFltFltIntTr;
typedef TTriple<TFlt, TFlt, TStr> TFltFltStrTr;
typedef TTriple<TChA, TChA, TChA> TChATr;
typedef TTriple<TStr, TStr, TStr> TStrTr;
typedef TTriple<TStr, TInt, TInt> TStrIntIntTr;
typedef TTriple<TStr, TFlt, TFlt> TStrFltFltTr;
typedef TTriple<TStr, TStr, TInt> TStrStrIntTr;
typedef TTriple<TStr, TInt, TStrV> TStrIntStrVTr;

/// Compares the triple by the second value.
template <class TVal1, class TVal2, class TVal3>
class TCmpTripleByVal2 {
private:
  bool IsAsc;
public:
  TCmpTripleByVal2(const bool& AscSort=true) : IsAsc(AscSort) { }
  bool operator () (const TTriple<TVal1, TVal2, TVal3>& T1, const TTriple<TVal1, TVal2, TVal3>& T2) const {
    if (IsAsc) { return T1.Val2 < T2.Val2; } else { return T2.Val2 < T1.Val2; }
  }
};

/// Compares the triple by the third value.
template <class TVal1, class TVal2, class TVal3>
class TCmpTripleByVal3 {
private:
  bool IsAsc;
public:
  TCmpTripleByVal3(const bool& AscSort=true) : IsAsc(AscSort) { }
  bool operator () (const TTriple<TVal1, TVal2, TVal3>& T1, const TTriple<TVal1, TVal2, TVal3>& T2) const {
    if (IsAsc) { return T1.Val3 < T2.Val3; } else { return T2.Val3 < T1.Val3; }
  }
};

/////////////////////////////////////////////////
// Quad
template <class TVal1, class TVal2, class TVal3, class TVal4>
class TQuad{
public:
  TVal1 Val1;
  TVal2 Val2;
  TVal3 Val3;
  TVal4 Val4;
public:
  TQuad():
    Val1(), Val2(), Val3(), Val4(){}
  TQuad(const TQuad& Quad):
    Val1(Quad.Val1), Val2(Quad.Val2), Val3(Quad.Val3), Val4(Quad.Val4){}
  TQuad(const TVal1& _Val1, const TVal2& _Val2, const TVal3& _Val3, const TVal4& _Val4):
    Val1(_Val1), Val2(_Val2), Val3(_Val3), Val4(_Val4){}
  explicit TQuad(TSIn& SIn):
    Val1(SIn), Val2(SIn), Val3(SIn), Val4(SIn){}
  void Save(TSOut& SOut) const {
    Val1.Save(SOut); Val2.Save(SOut); Val3.Save(SOut); Val4.Save(SOut);}
  void LoadXml(const PXmlTok& XmlTok, const TStr& Nm="");
  void SaveXml(TSOut& SOut, const TStr& Nm) const;

  TQuad& operator=(const TQuad& Quad){
    if (this!=&Quad){
      Val1=Quad.Val1; Val2=Quad.Val2; Val3=Quad.Val3; Val4=Quad.Val4;}
    return *this;}
  bool operator==(const TQuad& Quad) const {
    return (Val1==Quad.Val1)&&(Val2==Quad.Val2)&&(Val3==Quad.Val3)&&(Val4==Quad.Val4);}
  bool operator<(const TQuad& Quad) const {
    return (Val1<Quad.Val1)||((Val1==Quad.Val1)&&(Val2<Quad.Val2))||
     ((Val1==Quad.Val1)&&(Val2==Quad.Val2)&&(Val3<Quad.Val3))||
     ((Val1==Quad.Val1)&&(Val2==Quad.Val2)&&(Val3==Quad.Val3)&&(Val4<Quad.Val4));}

  int GetPrimHashCd() const {return  TPairHashImpl::GetHashCd(TPairHashImpl::GetHashCd(Val1.GetPrimHashCd(), Val2.GetPrimHashCd()), TPairHashImpl::GetHashCd(Val3.GetPrimHashCd(), Val4.GetPrimHashCd())); }
  int GetSecHashCd() const {return TPairHashImpl::GetHashCd(TPairHashImpl::GetHashCd(Val2.GetSecHashCd(), Val3.GetSecHashCd()), TPairHashImpl::GetHashCd(Val4.GetSecHashCd(), Val1.GetSecHashCd())); }

  void GetVal(TVal1& _Val1, TVal2& _Val2, TVal3& _Val3, TVal4& _Val4) const {
    _Val1=Val1; _Val2=Val2; _Val3=Val3; _Val4=Val4;}
  const TVal1& GetVal1() const { return Val1;}
  const TVal2& GetVal2() const { return Val2;}
  const TVal3& GetVal3() const { return Val3;}
  const TVal4& GetVal4() const { return Val4;}
};

typedef TQuad<TStr, TStr, TInt, TInt> TStrStrIntIntQu;
typedef TQuad<TStr, TStr, TStr, TStr> TStrQu;
typedef TQuad<TInt, TInt, TInt, TInt> TIntQu;
typedef TQuad<TFlt, TFlt, TFlt, TFlt> TFltQu;
typedef TQuad<TFlt, TInt, TInt, TInt> TFltIntIntIntQu;
typedef TQuad<TInt, TStr, TInt, TInt> TIntStrIntIntQu;
typedef TQuad<TInt, TInt, TFlt, TFlt> TIntIntFltFltQu;

/////////////////////////////////////////////////
// Tuple
template<class TVal, int NVals>
class TTuple {
private:
  TVal ValV [NVals];
public:
  TTuple(){}
  TTuple(const TVal& InitVal) { for (int i=0; i<Len(); i++) ValV[i]=InitVal; }
  TTuple(const TTuple& Tup) { for (int i=0; i<Len(); i++) ValV[i]=Tup[i]; }
  TTuple(TSIn& SIn) { for (int i=0; i<Len(); i++) ValV[i].Load(SIn); }
  void Save(TSOut& SOut) const { for (int i=0; i<Len(); i++) ValV[i].Save(SOut); }
  void Load(TSIn& SIn) { for (int i=0; i<Len(); i++) ValV[i].Load(SIn); }

  int Len() const { return NVals; }
  TVal& operator[] (const int& ValN) { return ValV[ValN]; }
  const TVal& operator[] (const int& ValN) const { return ValV[ValN]; }
  TTuple& operator = (const TTuple& Tup) { if (this != & Tup) {
    for (int i=0; i<Len(); i++) ValV[i]=Tup[i]; } return *this; }
  bool operator == (const TTuple& Tup) const {
    if (Len()!=Tup.Len()) { return false; }  if (&Tup==this) { return true; }
    for (int i=0; i<Len(); i++) if(ValV[i]!=Tup[i]){return false;} return true; }
  bool operator < (const TTuple& Tup) const {
    if (Len() == Tup.Len()) { for (int i=0; i<Len(); i++) {
      if(ValV[i]<Tup[i]){return true;} else if(ValV[i]>Tup[i]){return false;} } return false; }
    else { return Len() < Tup.Len(); } }
  void Sort(const bool& Asc=true);
  int FindMx() const;
  int FindMn() const;
  int GetPrimHashCd() const { int hc = 0;
    for (int i = 0; i < NVals; i++) { hc = TPairHashImpl::GetHashCd(hc, ValV[i].GetPrimHashCd()); }
    return hc; }
  int GetSecHashCd() const { int hc = 0;
    for (int i = 1; i < NVals; i++) { hc = TPairHashImpl::GetHashCd(hc, ValV[i].GetSecHashCd()); }
    if (NVals > 0) { hc = TPairHashImpl::GetHashCd(hc, ValV[0].GetSecHashCd()); }
    return hc; }

  TStr GetStr() const { TChA ValsStr;
    for (int i=0; i<Len(); i++) { ValsStr+=" "+ValV[i].GetStr(); }
    return TStr::Fmt("Tuple(%d):", Len())+ValsStr; }
};

template<class TVal, int NVals>
void TTuple<TVal, NVals>::Sort(const bool& Asc) {
  TVec<TVal, int> V(NVals);
  for (int i=0; i<NVals; i++) { V.Add(ValV[i]); }
  V.Sort(Asc);
  for (int i=0; i<NVals; i++) { ValV[i] = V[i]; }
}

template<class TVal, int NVals>
int TTuple<TVal, NVals>::FindMx() const {
  TVal MxVal = ValV[0];
  int ValN = 0;
  for (int i = 1; i < NVals; i++) {
    if (MxVal<ValV[i]) {
      MxVal=ValV[i]; ValN=i;
    }
  }
  return ValN;
}

template<class TVal, int NVals>
int TTuple<TVal, NVals>::FindMn() const {
  TVal MnVal = ValV[0];
  int ValN = 0;
  for (int i = 1; i < NVals; i++) {
    if (MnVal>ValV[i]) {
      MnVal=ValV[i]; ValN=i;
    }
  }
  return ValN;
}

/////////////////////////////////////////////////
// Key-Data
template <class TKey, class TDat>
class TKeyDat{
public:
  TKey Key;
  TDat Dat;
public:
  TKeyDat(): Key(), Dat(){}
  TKeyDat(const TKeyDat& KeyDat): Key(KeyDat.Key), Dat(KeyDat.Dat){}
  explicit TKeyDat(const TKey& _Key): Key(_Key), Dat(){}
  TKeyDat(const TKey& _Key, const TDat& _Dat): Key(_Key), Dat(_Dat){}
  explicit TKeyDat(TSIn& SIn): Key(SIn), Dat(SIn){}
  void Save(TSOut& SOut) const {Key.Save(SOut); Dat.Save(SOut);}
  void LoadXml(const PXmlTok& XmlTok, const TStr& Nm="");
  void SaveXml(TSOut& SOut, const TStr& Nm) const;

  TKeyDat& operator=(const TKeyDat& KeyDat){
    if (this!=&KeyDat){Key=KeyDat.Key; Dat=KeyDat.Dat;} return *this;}
  bool operator==(const TKeyDat& KeyDat) const {return Key==KeyDat.Key;}
  bool operator<(const TKeyDat& KeyDat) const {return Key<KeyDat.Key;}

  int GetPrimHashCd() const {return Key.GetPrimHashCd();}
  int GetSecHashCd() const {return Key.GetSecHashCd();}
};

template <class TKey, class TDat>
void GetSwitchedKdV(const TVec<TKeyDat<TKey, TDat>, int>& SrcKdV, TVec<TKeyDat<TDat, TKey>, int>& DstKdV){
  const int Kds=SrcKdV.Len();
  DstKdV.Gen(Kds, 0);
  for (int KdN=0; KdN<Kds; KdN++){
    const TKeyDat<TKey, TDat>& SrcKd=SrcKdV[KdN];
    DstKdV.Add(TKeyDat<TDat, TKey>(SrcKd.Dat, SrcKd.Key));
  }
}

typedef TKeyDat<TInt, TInt> TIntKd;
typedef TKeyDat<TInt, TUInt64> TIntUInt64Kd;
typedef TKeyDat<TInt, TFlt> TIntFltKd;
typedef TKeyDat<TIntPr, TFlt> TIntPrFltKd;
typedef TKeyDat<TInt, TFltPr> TIntFltPrKd;
typedef TKeyDat<TInt, TSFlt> TIntSFltKd;
typedef TKeyDat<TInt, TStr> TIntStrKd;
typedef TKeyDat<TUInt, TInt> TUIntIntKd;
typedef TKeyDat<TUInt, TUInt> TUIntKd;
typedef TKeyDat<TUInt64, TInt> TUInt64IntKd;
typedef TKeyDat<TUInt64, TFlt> TUInt64FltKd;
typedef TKeyDat<TUInt64, TStr> TUInt64StrKd;
typedef TKeyDat<TFlt, TBool> TFltBoolKd;
typedef TKeyDat<TFlt, TInt> TFltIntKd;
typedef TKeyDat<TFlt, TUInt64> TFltUInt64Kd;
typedef TKeyDat<TFlt, TIntPr> TFltIntPrKd;
typedef TKeyDat<TFlt, TUInt> TFltUIntKd;
typedef TKeyDat<TFlt, TFlt> TFltKd;
typedef TKeyDat<TFlt, TStr> TFltStrKd;
typedef TKeyDat<TFlt, TBool> TFltBoolKd;
typedef TKeyDat<TFlt, TIntBoolPr> TFltIntBoolPrKd;
typedef TKeyDat<TAscFlt, TInt> TAscFltIntKd;
typedef TKeyDat<TStr, TBool> TStrBoolKd;
typedef TKeyDat<TStr, TInt> TStrIntKd;
typedef TKeyDat<TStr, TFlt> TStrFltKd;
typedef TKeyDat<TStr, TAscFlt> TStrAscFltKd;
typedef TKeyDat<TStr, TStr> TStrKd;

// Key-Data comparator

template <class TVal1, class TVal2>
class TCmpKeyDatByDat {
private:
  bool IsAsc;
public:
  TCmpKeyDatByDat(const bool& AscSort=true) : IsAsc(AscSort) { }
  bool operator () (const TKeyDat<TVal1, TVal2>& P1, const TKeyDat<TVal1, TVal2>& P2) const {
    if (IsAsc) { return P1.Dat < P2.Dat; } else { return P2.Dat < P1.Dat; }
  }
};

//#//////////////////////////////////////////////
/// Vector is a sequence \c TVal objects representing an array that can change in size. ##TVec
template <class TVal, class TSizeTy = int>
class TVec{
public:
  typedef TVal* TIter;  //!< Random access iterator to \c TVal.
protected:
  TSizeTy MxVals; //!< Vector capacity. Capacity is the size of allocated storage. If <tt>MxVals==-1</tt>, then \c ValT is not owned by the vector, and it won't free it at destruction.
  TSizeTy Vals;   //!< Vector length. Length is the number of elements stored in the vector.
  TVal* ValT;     //!< Pointer to the memory where the elements of the vector are stored.
  bool IsShM; //!< True if the vector array is in shared memory
  /// Resizes the vector so that it can store at least \c _MxVals.
  void Resize(const TSizeTy& _MxVals=-1);
  /// Constructs the out of bounds error message.
  TStr GetXOutOfBoundsErrMsg(const TSizeTy& ValN) const;
public:
  TVec(): MxVals(0), Vals(0), ValT(NULL), IsShM(false) {}
  TVec(const TVec<TVal, TSizeTy>& Vec);
  /// Constructs a vector (an array) of length \c _Vals.
  explicit TVec(const TSizeTy& _Vals){
    IsShM = false;
    IAssert(0<=_Vals); MxVals=Vals=_Vals;
    if (_Vals==0){ValT=NULL;} else {ValT=new TVal[_Vals];}}
  /// Constructs a vector (an array) of length \c _Vals, while reserving enough memory to store \c _MxVals elements.
  TVec(const TSizeTy& _MxVals, const TSizeTy& _Vals){
    IsShM = false;
    IAssert((0<=_Vals)&&(_Vals<=_MxVals)); MxVals=_MxVals; Vals=_Vals;
    if (_MxVals==0){ValT=NULL;} else {ValT=new TVal[_MxVals];}}
  /// Constructs a vector of \c _Vals elements of memory array \c _ValT. ##TVec::TVec
  explicit TVec(TVal *_ValT, const TSizeTy& _Vals):
    MxVals(-1), Vals(_Vals), ValT(_ValT), IsShM(false){}
  ~TVec() {if ((ValT!=NULL) && (MxVals!=-1)) {delete[] ValT;}}
  explicit TVec(TSIn& SIn): MxVals(0), Vals(0), ValT(NULL), IsShM(false) {Load(SIn);}
  /// Constructs the vector from a shared memory input ##TVec::LoadShM
  void LoadShM(TShMIn& ShMIn);
  /// Constructs vector from shared memory input passing in functor to initialize elements
  template <typename TLoadShMElem>
  void LoadShM(TShMIn& ShMIn, TLoadShMElem LoadFromShMFn) {
    if ((ValT!=NULL) && (MxVals!=-1)) {delete[] ValT;}
    ShMIn.Load(MxVals);
    ShMIn.Load(Vals);
    if (MxVals == 0) {
      ValT = NULL;
    } else {
        ValT=new TVal[MxVals];
        for (TSizeTy ValN=0; ValN<Vals; ValN++) {
          LoadFromShMFn(ValT+ValN, ShMIn);
        }
    }
    IsShM = false;
  }

  void Load(TSIn& SIn);
  void Save(TSOut& SOut) const;
  void LoadXml(const PXmlTok& XmlTok, const TStr& Nm="");
  void SaveXml(TSOut& SOut, const TStr& Nm) const;

  /// Assigns new contents to the vector, replacing its current content.
  TVec<TVal, TSizeTy>& operator=(const TVec<TVal, TSizeTy>& Vec);
  /// Appends value \c Val to the vector.
  TVec<TVal, TSizeTy>& operator+(const TVal& Val){Add(Val); return *this;}
  /// Checks that the two vectors have the same contents.
  bool operator==(const TVec<TVal, TSizeTy>& Vec) const;
  /// Lexicographically compares two vectors. ##TVec::Less
  bool operator<(const TVec<TVal, TSizeTy>& Vec) const;
  /// Returns a reference to the element at position \c ValN in the vector.
  const TVal& operator[](const TSizeTy& ValN) const {
    AssertR((0<=ValN)&&(ValN<Vals), GetXOutOfBoundsErrMsg(ValN));
    return ValT[ValN];}
  /// Returns a reference to the element at position \c ValN in the vector.
  TVal& operator[](const TSizeTy& ValN){
    AssertR((0<=ValN)&&(ValN<Vals), GetXOutOfBoundsErrMsg(ValN));
    return ValT[ValN];}
  /// Returns the memory footprint (the number of bytes) of the vector.
  TSizeTy GetMemUsed() const {
    return TSizeTy(2*sizeof(TSizeTy)+sizeof(TVal*)+MxVals*sizeof(TVal));}
  /// Returns the memory size (the number of bytes) of a binary representation.
  TSizeTy GetMemSize() const {
    return TSizeTy(2*sizeof(TVal)+sizeof(TSizeTy)*Vals);}

  /// Returns primary hash code of the vector. Used by \c THash.
  int GetPrimHashCd() const;
  /// Returns secondary hash code of the vector. Used by \c THash.
  int GetSecHashCd() const;

  /// Constructs a vector (an array) of \c _Vals elements.
  void Gen(const TSizeTy& _Vals){ IAssert(0<=_Vals);
    if (ValT!=NULL && MxVals!=-1){delete[] ValT;} MxVals=Vals=_Vals;
    if (MxVals==0){ValT=NULL;} else {ValT=new TVal[MxVals];}}
  /// Constructs a vector (an array) of \c _Vals elements, while reserving enough memory for \c _MxVals elements.
  void Gen(const TSizeTy& _MxVals, const TSizeTy& _Vals){ IAssert((0<=_Vals)&&(_Vals<=_MxVals));
    if (ValT!=NULL  && MxVals!=-1){delete[] ValT;} MxVals=_MxVals; Vals=_Vals;
    if (_MxVals==0){ValT=NULL;} else {ValT=new TVal[_MxVals];}}
  /// Constructs a vector of \c _Vals elements of memory array \c _ValT. ##TVec::GenExt
  void GenExt(TVal *_ValT, const TSizeTy& _Vals){
    if (ValT!=NULL && MxVals!=-1){delete[] ValT;}
    MxVals=-1; Vals=_Vals; ValT=_ValT;}
  /// Returns true if the vector was created using the \c GenExt(). ##TVec::IsExt
  bool IsExt() const {return MxVals==-1;}
  /// Reserves enough memory for the vector to store \c _MxVals elements.
  void Reserve(const TSizeTy& _MxVals){Resize(_MxVals);}
  /// Reserves enough memory for the vector to store \c _MxVals elements and sets its length to \c _Vals.
  void Reserve(const TSizeTy& _MxVals, const TSizeTy& _Vals){ IAssert((0<=_Vals)&&(_Vals<=_MxVals)); Resize(_MxVals); Vals=_Vals;}
  /// Clears the contents of the vector. ##TVec::Clr
  void Clr(const bool& DoDel=true, const TSizeTy& NoDelLim=-1);
  /// Truncates the vector's length and capacity to \c _Vals elements. ##TVec::Trunc
  void Trunc(const TSizeTy& _Vals=-1);
  /// Reduces the vector's length to \c _Vals elements, which must be less than the current length.
  void Reduce(const TSizeTy& _Vals=-1) {Vals = _Vals;}
  /// Reduces vector capacity (frees memory) to match its size.
  void Pack();
  /// Takes over the data and the capacity from \c Vec. ##TVec::MoveFrom
  void MoveFrom(TVec<TVal, TSizeTy>& Vec);
  /// Copy \c Sz values from \c Vec starting at \c Offset.
  void CopyUniqueFrom(TVec<TVal, TSizeTy>& Vec, TInt Offset, TInt Sz);
  /// Swaps the contents of the vector with \c Vec.
  void Swap(TVec<TVal, TSizeTy>& Vec);
  /// Tests whether the vector is empty. ##TVec::Empty
  bool Empty() const {return Vals==0;}
  /// Returns the number of elements in the vector. ##TVec::Len
  TSizeTy Len() const {return Vals;}
  /// Returns the size of allocated storage capacity.
  TSizeTy Reserved() const {return MxVals;}
  /// Returns a reference to the last element of the vector.
  const TVal& Last() const {return GetVal(Len()-1);}
  /// Returns a reference to the last element of the vector.
  TVal& Last(){return GetVal(Len()-1);}
  /// Returns the position of the last element.
  TSizeTy LastValN() const {return Len()-1;}
  /// Returns a reference to the one before last element of the vector.
  const TVal& LastLast() const { AssertR(1<Vals, GetXOutOfBoundsErrMsg(Vals-2)); return ValT[Vals-2];}
  /// Returns a reference to the one before last element of the vector.
  TVal& LastLast(){ AssertR(1<Vals, GetXOutOfBoundsErrMsg(Vals-2)); return ValT[Vals-2];}
  /// Returns a reference to a random element in the vector.
  const TVal& GetRndVal(TRnd& Rnd=TInt::Rnd) const { return GetVal(Rnd.GetUniDevInt(Len())); }
  /// Returns a reference to a random element in the vector.
  TVal& GetRndVal(TRnd& Rnd=TInt::Rnd) { return GetVal(Rnd.GetUniDevInt(Len())); }
  /// Returns an iterator pointing to the first element in the vector.
  TIter BegI() const {return ValT;}
  /// Returns an iterator referring to the past-the-end element in the vector.
  TIter EndI() const {return ValT+Vals;}
  /// Returns an iterator an element at position \c ValN.
  TIter GetI(const TSizeTy& ValN) const {return ValT+ValN;}

  /// Adds a new element at the end of the vector, after its current last element. ##TVec::Add
  TSizeTy Add(){ AssertR(MxVals!=-1, "This vector was obtained from TVecPool. Such vectors cannot change its size!");
    if (Vals==MxVals){Resize();} return Vals++;}

  /// Adds a new element at the end of the vector, after its current last element. ##TVec::Add1
  TSizeTy Add(const TVal& Val){ AssertR(MxVals!=-1, "This vector was obtained from TVecPool. Such vectors cannot change its size!");
    if (Vals==MxVals){Resize();} ValT[Vals]=Val; return Vals++;}
  TSizeTy Add(TVal& Val){ AssertR(MxVals!=-1, "This vector was obtained from TVecPool. Such vectors cannot change its size!");
    if (Vals==MxVals){Resize();} ValT[Vals]=Val; return Vals++;}
  /// Adds element \c Val at the end of the vector. #TVec::Add2
  TSizeTy Add(const TVal& Val, const TSizeTy& ResizeLen){ AssertR(MxVals!=-1, "This vector was obtained from TVecPool. Such vectors cannot change its size!");
    if (Vals==MxVals){Resize(MxVals+ResizeLen);} ValT[Vals]=Val; return Vals++;}
#ifdef USE_OPENMP
  /// Adds element \c Val at the end of the vector in a thread safe manner, returns the element index in the vector. #TVec::AddMP
  TSizeTy AddMP(const TVal& Val){ const int Idx = __sync_fetch_and_add(&Vals, 1);
     ValT[Idx]=Val; return Idx;}
  /// Reserves space after the current last element in a thread safe manner, returning the old vector size. ##TVec::MoveLastMP
  TSizeTy MoveLastMP(const TVal& Val, int Inc){ const int Idx = __sync_fetch_and_add(&Vals, Inc);
  return Idx;}
#endif
  /// Adds the elements of the vector \c ValV to the to end of the vector.
  TSizeTy AddV(const TVec<TVal, TSizeTy>& ValV);
  /// Adds element \c Val to a sorted vector. ##TVec::AddSorted
  TSizeTy AddSorted(const TVal& Val, const bool& Asc=true, const TSizeTy& _MxVals=-1);
  /// Adds element \c Val to a sorted vector. ##TVec::AddBackSorted
  TSizeTy AddBackSorted(const TVal& Val, const bool& Asc);
  /// Adds element \c Val to a sorted vector only if the element \c Val is not already in the vector. ##TVec::AddMerged
  TSizeTy AddMerged(const TVal& Val);
  /// Adds elements of \c ValV to a sorted vector only if a particular element is not already in the vector. ##TVec::AddMerged1
  TSizeTy AddVMerged(const TVec<TVal, TSizeTy>& ValV);
  /// Adds element \c Val to a vector only if the element \c Val is not already in the vector. ##TVec::AddUnique
  TSizeTy AddUnique(const TVal& Val);
  /// Returns a reference to the element at position \c ValN in the vector.
  const TVal& GetVal(const TSizeTy& ValN) const {return operator[](ValN);}
  /// Returns a reference to the element at position \c ValN in the vector.
  TVal& GetVal(const TSizeTy& ValN){return operator[](ValN);}
  /// Sets the value of element at position \c ValN to \c Val.
  void SetVal(const TSizeTy& ValN, const TVal& Val){
    EAssertR(!(IsShM && (MxVals == -1)), "Cannot write to shared memory");
    AssertR((0<=ValN)&&(ValN<Vals), GetXOutOfBoundsErrMsg(ValN)); ValT[ValN] = Val;}
  /// Fills \c ValV with elements at positions <tt>BValN...EValN</tt>.
  void GetSubValV(const TSizeTy& BValN, const TSizeTy& EValN, TVec<TVal, TSizeTy>& ValV) const;
  /// Inserts new element \c Val before the element at position \c ValN.
  void Ins(const TSizeTy& ValN, const TVal& Val);
  /// Removes the element at position \c ValN.
  void Del(const TSizeTy& ValN);
  /// Removes the elements at positions <tt>MnValN...MxValN</tt>.
  void Del(const TSizeTy& MnValN, const TSizeTy& MxValN);
  /// Removes the last element of the vector.
  void DelLast(){Del(Len()-1);}
  /// Removes the first occurrence of element \c Val.
  bool DelIfIn(const TVal& Val);
  /// Removes all occurrences of element \c Val.
  void DelAll(const TVal& Val);
  /// Sets all elements of the vector to value \c Val.
  void PutAll(const TVal& Val);

  /// Swaps elements at positions \c ValN1 and \c ValN2.
  void Swap(const TSizeTy& ValN1, const TSizeTy& ValN2){EAssertR(!(IsShM && (MxVals == -1)), "Cannot write to shared memory");
    const TVal Val=ValT[ValN1]; ValT[ValN1]=ValT[ValN2]; ValT[ValN2]=Val;}
  /// Swaps the elements that iterators \c LVal and \c RVal point to.
  static void SwapI(TIter LVal, TIter RVal){const TVal Val=*LVal; *LVal=*RVal; *RVal=Val;}

  /// Generates next permutation of the elements in the vector. ##TVec::NextPerm
  bool NextPerm();
  /// Generates previous permutation of the elements in the vector. ##TVec::PrevPerm
  bool PrevPerm();

  // Sorting functions
  /// Picks three random elements at positions <tt>LValN...RValN</tt> and returns the middle one.
  TSizeTy GetPivotValN(const TSizeTy& LValN, const TSizeTy& RValN) const;
  /// Bubble sorts the values between positions <tt>MnLValN...MxLValN</tt>. ##TVec::BSort
  void BSort(const TSizeTy& MnLValN, const TSizeTy& MxRValN, const bool& Asc);
  /// Insertion sorts the values between positions <tt>MnLValN...MxLValN</tt>. ##TVec::ISort
  void ISort(const TSizeTy& MnLValN, const TSizeTy& MxRValN, const bool& Asc);
  /// Partitions the values between positions <tt>MnLValN...MxLValN</tt>. ##TVec::Partition
  TSizeTy Partition(const TSizeTy& MnLValN, const TSizeTy& MxRValN, const bool& Asc);
  /// Quick sorts the values between positions <tt>MnLValN...MxLValN</tt>. ##TVec::QSort
  void QSort(const TSizeTy& MnLValN, const TSizeTy& MxRValN, const bool& Asc);
  /// Sorts the elements of the vector. ##TVec::Sort
  void Sort(const bool& Asc=true);
  /// Checks whether the vector is sorted in ascending (if \c Asc=true) or descending (if \c Asc=false) order.
  bool IsSorted(const bool& Asc=true) const;
  /// Randomly shuffles the elements of the vector.
  void Shuffle(TRnd& Rnd);
  /// Reverses the order of the elements in the vector.
  void Reverse();
  /// Reverses the order of elements between <tt>LValN...RValN</tt>.
  void Reverse(TSizeTy LValN, TSizeTy RValN){ Assert(LValN>=0 && RValN<Len()); while (LValN < RValN){Swap(LValN++, RValN--);} }
  /// Sorts the vector and only keeps a single element of each value.
  void Merge();

  /// Picks three random elements at positions <tt>BI...EI</tt> and returns the middle one under the comparator \c Cmp.
  template <class TCmp>
  static TIter GetPivotValNCmp(const TIter& BI, const TIter& EI, const TCmp& Cmp) {
    TSizeTy SubVals=TSizeTy(EI-BI); if (SubVals > TInt::Mx-1) { SubVals = TInt::Mx-1; }
    const TSizeTy ValN1=TInt::GetRnd(SubVals), ValN2=TInt::GetRnd(SubVals), ValN3=TInt::GetRnd(SubVals);
    const TVal& Val1 = *(BI+ValN1);  const TVal& Val2 = *(BI+ValN2); const TVal& Val3 = *(BI+ValN3);
    if (Cmp(Val1, Val2)) {
      if (Cmp(Val2, Val3)) return BI+ValN2;
      else if (Cmp(Val3, Val1)) return BI+ValN1;
      else return BI+ValN3;
    } else {
      if (Cmp(Val1, Val3)) return BI+ValN1;
      else if (Cmp(Val3, Val2)) return BI+ValN2;
      else return BI+ValN3; } }
  /// Partitions the values between positions <tt>BI...EI</tt> under the comparator \c Cmp.
  template <class TCmp>
  static TIter PartitionCmp(TIter BI, TIter EI, const TVal Pivot, const TCmp& Cmp) {
    forever {
      while (Cmp(*BI, Pivot)){++BI;}  --EI;
      while (Cmp(Pivot, *EI)){--EI;}
      if (!(BI<EI)){return BI;}  SwapI(BI, EI);  ++BI; } }
  /// Bubble sorts the values between positions <tt>BI...EI</tt> under the comparator \c Cmp.
  template <class TCmp>
  static void BSortCmp(TIter BI, TIter EI, const TCmp& Cmp) {
    for (TIter i = BI; i != EI; ++i) {
      for (TIter j = EI-1; j != i; --j) {
        if (Cmp(*j, *(j-1))) { SwapI(j, j-1); } } } }
  /// Insertion sorts the values between positions <tt>BI...EI</tt> under the comparator \c Cmp.
  template <class TCmp>
  static void ISortCmp(TIter BI, TIter EI, const TCmp& Cmp) {
    if (BI + 1 < EI) {
      for (TIter i = BI, j; i != EI; ++i) { TVal Tmp=*i;  j=i;
        while (j > BI && Cmp(Tmp, *(j-1))) { *j = *(j-1); --j; } *j=Tmp; } } }
  /// Quick sorts the values between positions <tt>BI...EI</tt> under the comparator \c Cmp.
  template <class TCmp>
  static void QSortCmp(TIter BI, TIter EI, const TCmp& Cmp) {
    if (BI + 1 < EI) {
      if (EI - BI < 20) { ISortCmp(BI, EI, Cmp); }
      else { const TVal Val = *GetPivotValNCmp(BI, EI, Cmp);
        TIter Split = PartitionCmp(BI, EI, Val, Cmp);
        QSortCmp(BI, Split, Cmp);  QSortCmp(Split, EI, Cmp); } } }
  /// Sorts the elements of the vector using the comparator \c Cmp.
  template <class TCmp>
  void SortCmp(const TCmp& Cmp){ QSortCmp(BegI(), EndI(), Cmp);}
  /// Checks whether the vector is sorted according to the comparator \c Cmp.
  template <class TCmp>
  bool IsSortedCmp(const TCmp& Cmp) const {
    if (EndI() == BegI()) return true;
    for (TIter i = BegI(); i != EndI()-1; ++i) {
      if (Cmp(*(i+1), *i)){return false;} } return true; }
  /// Sets this vector to its intersection with \c ValV. Assumes the vectors are sorted!
  void Intrs(const TVec<TVal, TSizeTy>& ValV);
  /// Sets this vector to its union with \c ValV. Assumes the vectors are sorted!
  void Union(const TVec<TVal, TSizeTy>& ValV);
  /// Subtracts \c ValV from this vector. Assumes the vectors are sorted! ##TVec::Diff
  void Diff(const TVec<TVal, TSizeTy>& ValV);
  /// Sets \c DstValV to the intersection of this vector and \c ValV. Assumes the vectors are sorted!
  void Intrs(const TVec<TVal, TSizeTy>& ValV, TVec<TVal, TSizeTy>& DstValV) const;
  /// Sets \c DstValV to the union of this vector and \c ValV. Assumes the vectors are sorted!
  void Union(const TVec<TVal, TSizeTy>& ValV, TVec<TVal, TSizeTy>& DstValV) const;
  /// Sets \c DstValV to the difference between this vector and \c ValV. Assumes the vectors are sorted! ##TVec::Diff1
  void Diff(const TVec<TVal, TSizeTy>& ValV, TVec<TVal, TSizeTy>& DstValV) const;
  /// Returns the size of the intersection of vectors \c this and \c ValV. Assumes the vectors are sorted!
  TSizeTy IntrsLen(const TVec<TVal, TSizeTy>& ValV) const;
  /// Returns the size of the union of vectors \c this and \c ValV. Assumes the vectors are sorted!
  TSizeTy UnionLen(const TVec<TVal, TSizeTy>& ValV) const;

  /// Counts the number of occurrences of \c Val in the vector.
  TSizeTy Count(const TVal& Val) const;
  /// Returns the position of an element with value \c Val. ##TVec::SearchBin
  TSizeTy SearchBin(const TVal& Val) const;
  /// Returns the position of an element with value \c Val. ##TVec::SearchBin1
  TSizeTy SearchBin(const TVal& Val, TSizeTy& InsValN) const;
  /// Returns the position of an element with value \c Val. ##TVec::SearchBinLeft
  TSizeTy SearchBinLeft(const TVal& Val, TSizeTy& InsValN) const;
  /// Returns the position of an element with value \c Val. ##TVec::SearchForw
  TSizeTy SearchForw(const TVal& Val, const TSizeTy& BValN=0) const;
  /// Returns the position of an element with value \c Val. ##TVec::SearchBack
  TSizeTy SearchBack(const TVal& Val) const;
  /// Returns the starting position of vector \c ValV. ##TVec::SearchVForw
  TSizeTy SearchVForw(const TVec<TVal, TSizeTy>& ValV, const TSizeTy& BValN=0) const;

  /// Checks whether element \c Val is a member of the vector.
  bool IsIn(const TVal& Val) const {return SearchForw(Val)!=-1;}
  /// Checks whether element \c Val is a member of the vector. ##TVec::IsIn
  bool IsIn(const TVal& Val, TSizeTy& ValN) const { ValN=SearchForw(Val); return ValN!=-1;}
  /// Checks whether element \c Val is a member of the vector. ##TVec::IsInBin
  bool IsInBin(const TVal& Val) const {return SearchBin(Val)!=-1;}
  /// Returns reference to the first occurrence of element \c Val.
  const TVal& GetDat(const TVal& Val) const { return GetVal(SearchForw(Val));}
  /// Returns reference to the first occurrence of element \c Val. ##TVec::GetAddDat
  TVal& GetAddDat(const TVal& Val){ AssertR(MxVals!=-1, "This vector was obtained from TVecPool. Such vectors cannot change its size!");
    const TSizeTy ValN=SearchForw(Val); if (ValN==-1){Add(Val); return Last();} else {return GetVal(ValN);}}
  /// Returns the position of the largest element in the vector.
  TSizeTy GetMxValN() const;

  /// Returns a vector on element \c Val1.
  static TVec<TVal, TSizeTy> GetV(const TVal& Val1){
    TVec<TVal, TSizeTy> V(1, 0); V.Add(Val1); return V;}
  /// Returns a vector on elements \c Val1, \c Val2.
  static TVec<TVal, TSizeTy> GetV(const TVal& Val1, const TVal& Val2){
    TVec<TVal, TSizeTy> V(2, 0); V.Add(Val1); V.Add(Val2); return V;}
  /// Returns a vector on elements <tt>Val1...Val3</tt>.
  static TVec<TVal, TSizeTy> GetV(const TVal& Val1, const TVal& Val2, const TVal& Val3){
    TVec<TVal, TSizeTy> V(3, 0); V.Add(Val1); V.Add(Val2); V.Add(Val3); return V;}
  /// Returns a vector on elements <tt>Val1...Val4</tt>.
  static TVec<TVal, TSizeTy> GetV(const TVal& Val1, const TVal& Val2, const TVal& Val3, const TVal& Val4){
    TVec<TVal, TSizeTy> V(4, 0); V.Add(Val1); V.Add(Val2); V.Add(Val3); V.Add(Val4); return V;}
  /// Returns a vector on elements <tt>Val1...Val5</tt>.
  static TVec<TVal, TSizeTy> GetV(const TVal& Val1, const TVal& Val2, const TVal& Val3, const TVal& Val4, const TVal& Val5){
    TVec<TVal, TSizeTy> V(5, 0); V.Add(Val1); V.Add(Val2); V.Add(Val3); V.Add(Val4); V.Add(Val5); return V;}
  /// Returns a vector on elements <tt>Val1...Val6</tt>.
  static TVec<TVal, TSizeTy> GetV(const TVal& Val1, const TVal& Val2, const TVal& Val3, const TVal& Val4, const TVal& Val5, const TVal& Val6){
    TVec<TVal, TSizeTy> V(6, 0); V.Add(Val1); V.Add(Val2); V.Add(Val3); V.Add(Val4); V.Add(Val5); V.Add(Val6); return V;}
  /// Returns a vector on elements <tt>Val1...Val7</tt>.
  static TVec<TVal, TSizeTy> GetV(const TVal& Val1, const TVal& Val2, const TVal& Val3, const TVal& Val4, const TVal& Val5, const TVal& Val6, const TVal& Val7){
    TVec<TVal, TSizeTy> V(7, 0); V.Add(Val1); V.Add(Val2); V.Add(Val3); V.Add(Val4); V.Add(Val5); V.Add(Val6); V.Add(Val7); return V;}
  /// Returns a vector on elements <tt>Val1...Val8</tt>.
  static TVec<TVal, TSizeTy> GetV(const TVal& Val1, const TVal& Val2, const TVal& Val3, const TVal& Val4, const TVal& Val5, const TVal& Val6, const TVal& Val7, const TVal& Val8){
    TVec<TVal, TSizeTy> V(8, 0); V.Add(Val1); V.Add(Val2); V.Add(Val3); V.Add(Val4); V.Add(Val5); V.Add(Val6); V.Add(Val7); V.Add(Val8); return V;}
  /// Returns a vector on elements <tt>Val1...Val9</tt>.
  static TVec<TVal, TSizeTy> GetV(const TVal& Val1, const TVal& Val2, const TVal& Val3, const TVal& Val4, const TVal& Val5, const TVal& Val6, const TVal& Val7, const TVal& Val8, const TVal& Val9){
    TVec<TVal, TSizeTy> V(9, 0); V.Add(Val1); V.Add(Val2); V.Add(Val3); V.Add(Val4); V.Add(Val5); V.Add(Val6); V.Add(Val7); V.Add(Val8); V.Add(Val9); return V;}
};

template <class TVal, class TSizeTy>
void TVec<TVal, TSizeTy>::Resize(const TSizeTy& _MxVals){
  IAssertR(MxVals!=-1 || IsShM, TStr::Fmt("Can not increase the capacity of the vector. %s. [Program failed to allocate more memory. Solution: Get a bigger machine and a 64-bit compiler.]", GetTypeNm(*this).CStr()).CStr());
  IAssertR(MxVals!=(TInt::Mx-1024), TStr::Fmt("Buffer size at maximum. %s. [Program refuses to allocate more memory. Solution-1: Send your test case to developers.]", GetTypeNm(*this).CStr()).CStr());
  TSizeTy OldMxVals = MxVals;
  if (MxVals == -1) {MxVals = Vals;}
  if (_MxVals==-1){
    if (Vals==0){MxVals=16;} else {MxVals*=2;}
  } else {
    if (_MxVals<=MxVals){return;} else {MxVals=_MxVals;}
  }
  if (MxVals < 0) {
    MxVals = TInt::Mx-1024;
  }
  if (ValT==NULL){
    try {
      ValT=new TVal[MxVals];
    }
    catch (std::exception Ex){
      FailR(TStr::Fmt("TVec::Resize: %s, Length:%s, Capacity:%s, New capacity:%s, Type:%s [Program failed to allocate more memory. Solution: Get a bigger machine and a 64-bit compiler.]",
        Ex.what(), TInt::GetStr(Vals).CStr(), TInt::GetStr(MxVals).CStr(), TInt::GetStr(_MxVals).CStr(), GetTypeNm(*this).CStr()).CStr());}
  } else {
    TVal* NewValT = NULL;
    try {
      NewValT=new TVal[MxVals];
    }
    catch (std::exception Ex){
      FailR(TStr::Fmt("TVec::Resize: %s, Length:%s, Capacity:%s, New capacity:%s, Type:%s [Program failed to allocate more memory. Solution-1: Get a bigger machine and a 64-bit compiler.]",
        Ex.what(), TInt::GetStr(Vals).CStr(), TInt::GetStr(MxVals).CStr(), TInt::GetStr(_MxVals).CStr(), GetTypeNm(*this).CStr()).CStr());}
    IAssert(NewValT!=NULL);
    for (TSizeTy ValN=0; ValN<Vals; ValN++){NewValT[ValN]=ValT[ValN];}
    if (OldMxVals != -1) {delete[] ValT;} ValT=NewValT;
  }
  IsShM = false;
}

template <class TVal, class TSizeTy>
TStr TVec<TVal, TSizeTy>::GetXOutOfBoundsErrMsg(const TSizeTy& ValN) const {
  return TStr()+
  "Index:"+TInt::GetStr(ValN)+
  " Vals:"+TInt::GetStr(Vals)+
  " MxVals:"+TInt::GetStr(MxVals)+
  " Type:"+GetTypeNm(*this);
}

template <class TVal, class TSizeTy>
TVec<TVal, TSizeTy>::TVec(const TVec<TVal, TSizeTy>& Vec){
  MxVals=Vec.MxVals;
  Vals=Vec.Vals;
  if (MxVals==0) {ValT=NULL;} else {ValT=new TVal[MxVals];}
  for (TSizeTy ValN=0; ValN<Vec.Vals; ValN++){ValT[ValN]=Vec.ValT[ValN];}
  IsShM = false;
}


template <class TVal, class TSizeTy>
void TVec<TVal, TSizeTy>::LoadShM(TShMIn& ShMIn) {
  if ((ValT!=NULL) && (MxVals!=-1)) {delete[] ValT;}
  ShMIn.Load(MxVals);
  MxVals = -1;
  ShMIn.Load(Vals);
  if (MxVals == 0) {
    ValT = NULL;
  } else {
      ValT = (TVal*)(ShMIn.AdvanceCursor(Vals*sizeof(TVal)));
      IsShM = true;
  }
}

template <class TVal, class TSizeTy>
void TVec<TVal, TSizeTy>::Load(TSIn& SIn){
  if ( (ValT!=NULL) && (MxVals!=-1)) {delete[] ValT;}
  SIn.Load(MxVals); SIn.Load(Vals); MxVals=Vals;
  if ( MxVals==0 ){ValT=NULL;} else {ValT=new TVal[MxVals];}
  for (TSizeTy ValN=0; ValN<Vals; ValN++){ValT[ValN]=TVal(SIn);}
}

template <class TVal, class TSizeTy>
void TVec<TVal, TSizeTy>::Save(TSOut& SOut) const {
  if (MxVals!=-1){SOut.Save(MxVals);} else {SOut.Save(Vals);}
  SOut.Save(Vals);
  for (TSizeTy ValN=0; ValN<Vals; ValN++){ValT[ValN].Save(SOut);}
}

template <class TVal, class TSizeTy>
TVec<TVal, TSizeTy>& TVec<TVal, TSizeTy>::operator=(const TVec<TVal, TSizeTy>& Vec){
  if (this!=&Vec){
    if ((ValT!=NULL)&&(MxVals!=-1)){delete[] ValT;}
    MxVals=Vals=Vec.Vals;
    if (MxVals==0){ValT=NULL;} else {ValT=new TVal[MxVals];}
    for (TSizeTy ValN=0; ValN<Vec.Vals; ValN++){ValT[ValN]=Vec.ValT[ValN];}
  }
  return *this;
}

template <class TVal, class TSizeTy>
bool TVec<TVal, TSizeTy>::operator==(const TVec<TVal, TSizeTy>& Vec) const {
  if (this==&Vec){return true;}
  if (Len()!=Vec.Len()){return false;}
  for (TSizeTy ValN=0; ValN<Vals; ValN++){
    if (ValT[ValN]!=Vec.ValT[ValN]){return false;}}
  return true;
}

template <class TVal, class TSizeTy>
bool TVec<TVal, TSizeTy>::operator<(const TVec<TVal, TSizeTy>& Vec) const {
  if (this==&Vec){return false;}
  if (Len()==Vec.Len()){
    for (TSizeTy ValN=0; ValN<Vals; ValN++){
      if (ValT[ValN]<Vec.ValT[ValN]){return true;}
      else if (ValT[ValN]>Vec.ValT[ValN]){return false;}
      else {}
    }
    return false;
  } else {
    return Len()<Vec.Len();
  }
}

// Improved hashing of vectors (Jure Apr 20 2013)
// This change makes binary representation of vectors incompatible with previous code.
// Previous hash functions are available for compatibility in class TVecHashF_OldGLib
template <class TVal, class TSizeTy>
int TVec<TVal, TSizeTy>::GetPrimHashCd() const {
  int hc = 0;
  for (TSizeTy i=0; i<Vals; i++){
    hc = TPairHashImpl::GetHashCd(hc, ValT[i].GetPrimHashCd());
  }
  return hc;
}

// Improved hashing of vectors (Jure Apr 20 2013)
// This change makes binary representation of vectors incompatible with previous code.
// Previous hash functions are available for compatibility in class TVecHashF_OldGLib
template <class TVal, class TSizeTy>
int TVec<TVal, TSizeTy>::GetSecHashCd() const {
  int hc = 0;
  for (TSizeTy i=0; i<Vals; i++){
    hc = TPairHashImpl::GetHashCd(hc, ValT[i].GetSecHashCd());
  }
  if (Vals > 0) {
    hc = TPairHashImpl::GetHashCd(hc, ValT[0].GetSecHashCd()); }
  return hc;
}

template <class TVal, class TSizeTy>
void TVec<TVal, TSizeTy>::Clr(const bool& DoDel, const TSizeTy& NoDelLim){
  if ((DoDel)||((!DoDel)&&(NoDelLim!=-1)&&(MxVals>NoDelLim))){
    if ((ValT!=NULL)&&(MxVals!=-1)){delete[] ValT;}
    MxVals=Vals=0; ValT=NULL;
  } else {
    IAssertR(MxVals!=-1 || IsShM, "This vector was obtained from TVecPool. Such vectors cannot change its size!");
    Vals=0;
  }
}

template <class TVal, class TSizeTy>
void TVec<TVal, TSizeTy>::Trunc(const TSizeTy& _Vals){
  EAssertR(!(MxVals==-1 && IsShM), "Cannot truncate a shared memory vector");
  IAssertR(MxVals!=-1, "This vector was obtained from TVecPool. Such vectors cannot change its size!");
  IAssert((_Vals==-1)||(_Vals>=0));
  if ((_Vals!=-1)&&(_Vals>=Vals)){
    return;
  } else
    if (((_Vals==-1)&&(Vals==0))||(_Vals==0)){
      if (ValT!=NULL){delete[] ValT;}
      MxVals=Vals=0; ValT=NULL;
    } else {
      if (_Vals==-1){
        if (MxVals==Vals){return;} else {MxVals=Vals;}
      } else {
        MxVals=Vals=_Vals;
      }
      TVal* NewValT=new TVal[MxVals];
      IAssert(NewValT!=NULL);
      for (TSizeTy ValN=0; ValN<Vals; ValN++){NewValT[ValN]=ValT[ValN];}
      delete[] ValT; ValT=NewValT;
    }
}

template <class TVal, class TSizeTy>
void TVec<TVal, TSizeTy>::Pack(){
  EAssertR(!(IsShM && (MxVals == -1)), "Cannot pack accessed shared memory");
  IAssertR(MxVals!=-1, "This vector was obtained from TVecPool. Such vectors cannot change its size!");
  if (Vals==0){
    if (ValT!=NULL){delete[] ValT;} ValT=NULL;
  } else
    if (Vals<MxVals){
      MxVals=Vals;
      TVal* NewValT=new TVal[MxVals];
      IAssert(NewValT!=NULL);
      for (TSizeTy ValN=0; ValN<Vals; ValN++){NewValT[ValN]=ValT[ValN];}
      delete[] ValT; ValT=NewValT;
    }
}

template <class TVal, class TSizeTy>
void TVec<TVal, TSizeTy>::MoveFrom(TVec<TVal, TSizeTy>& Vec){
  if (this!=&Vec){
    if (ValT!=NULL && MxVals!=-1){delete[] ValT;}
    MxVals=Vec.MxVals; Vals=Vec.Vals; ValT=Vec.ValT;
    Vec.MxVals=0; Vec.Vals=0; Vec.ValT=NULL;
  }
}

template <class TVal, class TSizeTy>
void TVec<TVal, TSizeTy>::CopyUniqueFrom(TVec<TVal, TSizeTy>& Vec, TInt Offset, TInt Sz){
  EAssertR(!(IsShM && (MxVals == -1)), "Cannot write to shared memory");
  if (this!=&Vec){
    if (ValT!=NULL && MxVals!=-1 && MxVals < Sz){
      delete[] ValT;
      ValT=new TVal[Sz];
    }
    if (Sz == 0) { Vals = 0; return; }
    ValT[0] = Vec.ValT[Offset];
    Vals = 1;
    for (TSizeTy ValN=1; ValN<Sz; ValN++){
      if (ValT[Vals-1] != Vec.ValT[Offset+ValN]) {
        ValT[Vals++] = Vec.ValT[Offset+ValN];
      }
    }
  }
}

template <class TVal, class TSizeTy>
void TVec<TVal, TSizeTy>::Swap(TVec<TVal, TSizeTy>& Vec){
  if (this!=&Vec){
    ::Swap(MxVals, Vec.MxVals);
    ::Swap(Vals, Vec.Vals);
    ::Swap(ValT, Vec.ValT);
  }
}

template <class TVal, class TSizeTy>
TSizeTy TVec<TVal, TSizeTy>::AddV(const TVec<TVal, TSizeTy>& ValV){
  AssertR(MxVals!=-1 || IsShM, "This vector was obtained from TVecPool. Such vectors cannot change its size!");
  for (TSizeTy ValN=0; ValN<ValV.Vals; ValN++){Add(ValV[ValN]);}
  return Len();
}

template <class TVal, class TSizeTy>
TSizeTy TVec<TVal, TSizeTy>::AddSorted(const TVal& Val, const bool& Asc, const TSizeTy& _MxVals){
  EAssertR(!(IsShM && (MxVals == -1)), "Cannot write to shared memory");
  AssertR(MxVals!=-1, "This vector was obtained from TVecPool. Such vectors cannot change its size!");
  TSizeTy ValN=Add(Val);
  if (Asc){
    while ((ValN>0)&&(ValT[ValN]<ValT[ValN-1])){
      Swap(ValN, ValN-1); ValN--;}
  } else {
    while ((ValN>0)&&(ValT[ValN]>ValT[ValN-1])){
      Swap(ValN, ValN-1); ValN--;}
  }
  if ((_MxVals!=-1)&&(Len()>_MxVals)){Del(_MxVals, Len()-1);}
  return ValN;
}

template <class TVal, class TSizeTy>
TSizeTy TVec<TVal, TSizeTy>::AddBackSorted(const TVal& Val, const bool& Asc){
  EAssertR(!(IsShM && (MxVals == -1)), "Cannot write to shared memory");
  AssertR(MxVals!=-1, "This vector was obtained from TVecPool. Such vectors cannot change its size!");
  Add();
  TSizeTy ValN=Vals-2;
  while ((ValN>=0)&&((Asc&&(Val<ValT[ValN]))||(!Asc&&(Val>ValT[ValN])))){
    ValT[ValN+1]=ValT[ValN]; ValN--;}
  ValT[ValN+1]=Val;
  return ValN+1;
}

template <class TVal, class TSizeTy>
TSizeTy TVec<TVal, TSizeTy>::AddMerged(const TVal& Val){
  EAssertR(!(IsShM && (MxVals == -1)), "Cannot write to shared memory");
  AssertR(MxVals!=-1, "This vector was obtained from TVecPool. Such vectors cannot change its size!");
  TSizeTy ValN=SearchBin(Val);
  if (ValN==-1){return AddSorted(Val);}
  else {GetVal(ValN)=Val; return -1;}
}

template <class TVal, class TSizeTy>
TSizeTy TVec<TVal, TSizeTy>::AddVMerged(const TVec<TVal, TSizeTy>& ValV){
  EAssertR(!(IsShM && (MxVals == -1)), "Cannot write to shared memory");
  AssertR(MxVals!=-1, "This vector was obtained from TVecPool. Such vectors cannot change its size!");
  TVec<TVal> ValV2;
  for (TSizeTy ValN=0; ValN<ValV.Len(); ValN++){if (!IsInBin(ValV[ValN])) {ValV2.Add(ValV[ValN]);}} ValV2.Merge();
  TVec<TSizeTy> ValNV; TSizeTy ValN=0;
  for (TSizeTy ValN2 = 0; ValN2 < ValV2.Len(); ValN2++){
      for (; ValN<Len(); ValN++){if (GetVal(ValN) > ValV2[ValN2]){ValNV.Add(ValN); break;}}}
  TSizeTy EndValN = Len()-1; TSizeTy OutValN = ValV2.Len() - ValNV.Len();
  for (TSizeTy N = 0; N < ValV2.Len() - OutValN; N++){Add();}
  for (TSizeTy N = 0; N < OutValN; N++){Add(ValV2[ValNV.Len()+N]);}
  for (TSizeTy ValN2 = ValNV.Len()-1; ValN2 >= 0; ValN2--) {
      const TSizeTy & BegValN = ValNV[ValN2]; const TSizeTy OffSet = ValN2+1;
      for (; EndValN >= BegValN; EndValN--){GetVal(EndValN+OffSet) = GetVal(EndValN);}
      EndValN = BegValN + OffSet - 1; GetVal(EndValN) = ValV2[ValN2]; EndValN = BegValN - 1;}
  return Len();
}

template <class TVal, class TSizeTy>
TSizeTy TVec<TVal, TSizeTy>::AddUnique(const TVal& Val){
  AssertR(MxVals!=-1 || IsShM, "This vector was obtained from TVecPool. Such vectors cannot change its size!");
  TSizeTy ValN=SearchForw(Val);
  if (ValN==-1){return Add(Val);}
  else {GetVal(ValN)=Val; return -1;}
}

template <class TVal, class TSizeTy>
void TVec<TVal, TSizeTy>::GetSubValV(const TSizeTy& _BValN, const TSizeTy& _EValN, TVec<TVal, TSizeTy>& SubValV) const {
  const TSizeTy BValN=TInt::GetInRng(_BValN, 0, Len()-1);
  const TSizeTy EValN=TInt::GetInRng(_EValN, 0, Len()-1);
  const TSizeTy SubVals=TInt::GetMx(0, EValN-BValN+1);
  SubValV.Gen(SubVals, 0);
  for (TSizeTy ValN=BValN; ValN<=EValN; ValN++){
    SubValV.Add(GetVal(ValN));}
}

template <class TVal, class TSizeTy>
void TVec<TVal, TSizeTy>::Ins(const TSizeTy& ValN, const TVal& Val){
  EAssertR(!(IsShM && (MxVals == -1)), "Cannot write to shared memory");
  AssertR(MxVals!=-1, "This vector was obtained from TVecPool. Such vectors cannot change its size!");
  Add();  Assert((0<=ValN)&&(ValN<Vals));
  for (TSizeTy MValN=Vals-2; MValN>=ValN; MValN--){ValT[MValN+1]=ValT[MValN];}
  ValT[ValN]=Val;
}

template <class TVal, class TSizeTy>
void TVec<TVal, TSizeTy>::Del(const TSizeTy& ValN){
  EAssertR(!(IsShM && (MxVals == -1)), "Cannot write to shared memory");
  AssertR(MxVals!=-1, "This vector was obtained from TVecPool. Such vectors cannot change its size!");
  Assert((0<=ValN)&&(ValN<Vals));
  for (TSizeTy MValN=ValN+1; MValN<Vals; MValN++){
    ValT[MValN-1]=ValT[MValN];}
  ValT[--Vals]=TVal();
}

template <class TVal, class TSizeTy>
void TVec<TVal, TSizeTy>::Del(const TSizeTy& MnValN, const TSizeTy& MxValN){
  EAssertR(!(IsShM && (MxVals == -1)), "Cannot write to shared memory");
  AssertR(MxVals!=-1, "This vector was obtained from TVecPool. Such vectors cannot change its size!");
  Assert((0<=MnValN)&&(MnValN<Vals)&&(0<=MxValN)&&(MxValN<Vals));
  Assert(MnValN<=MxValN);
  for (TSizeTy ValN=MxValN+1; ValN<Vals; ValN++){
    ValT[MnValN+ValN-MxValN-1]=ValT[ValN];}
  for (TSizeTy ValN=Vals-MxValN+MnValN-1; ValN<Vals; ValN++){
    ValT[ValN]=TVal();}
  Vals-=MxValN-MnValN+1;
}

template <class TVal, class TSizeTy>
bool TVec<TVal, TSizeTy>::DelIfIn(const TVal& Val){
  EAssertR(!(IsShM && (MxVals == -1)), "Cannot write to shared memory");
  AssertR(MxVals!=-1, "This vector was obtained from TVecPool. Such vectors cannot change its size!");
  TSizeTy ValN=SearchForw(Val);
  if (ValN!=-1){Del(ValN); return true;}
  else {return false;}
}

template <class TVal, class TSizeTy>
void TVec<TVal, TSizeTy>::DelAll(const TVal& Val){
  EAssertR(!(IsShM && (MxVals == -1)), "Cannot write to shared memory");
  AssertR(MxVals!=-1, "This vector was obtained from TVecPool. Such vectors cannot change its size!");
  TSizeTy ValN;
  while ((ValN=SearchForw(Val))!=-1){Del(ValN);}
}

template <class TVal, class TSizeTy>
void TVec<TVal, TSizeTy>::PutAll(const TVal& Val){
  EAssertR(!(IsShM && (MxVals == -1)), "Cannot write to shared memory");
  for (TSizeTy ValN=0; ValN<Vals; ValN++){ValT[ValN]=Val;}
}

template <class TVal, class TSizeTy>
void TVec<TVal, TSizeTy>::BSort(const TSizeTy& MnLValN, const TSizeTy& MxRValN, const bool& Asc){
  for (TSizeTy ValN1=MnLValN; ValN1<=MxRValN; ValN1++){
    for (TSizeTy ValN2=MxRValN; ValN2>ValN1; ValN2--){
      if (Asc){
        if (ValT[ValN2]<ValT[ValN2-1]){Swap(ValN2, ValN2-1);}
      } else {
        if (ValT[ValN2]>ValT[ValN2-1]){Swap(ValN2, ValN2-1);}
      }
    }
  }
}

template <class TVal, class TSizeTy>
void TVec<TVal, TSizeTy>::ISort(const TSizeTy& MnLValN, const TSizeTy& MxRValN, const bool& Asc){
  if (MnLValN<MxRValN){
    for (TSizeTy ValN1=MnLValN+1; ValN1<=MxRValN; ValN1++){
      TVal Val=ValT[ValN1]; TSizeTy ValN2=ValN1;
      if (Asc){
        while ((ValN2>MnLValN)&&(ValT[ValN2-1]>Val)){
          ValT[ValN2]=ValT[ValN2-1]; ValN2--;}
      } else {
        while ((ValN2>MnLValN)&&(ValT[ValN2-1]<Val)){
          ValT[ValN2]=ValT[ValN2-1]; ValN2--;}
      }
      ValT[ValN2]=Val;
    }
  }
}

template <class TVal, class TSizeTy>
TSizeTy TVec<TVal, TSizeTy>::GetPivotValN(const TSizeTy& LValN, const TSizeTy& RValN) const {
  TSizeTy SubVals=RValN-LValN+1;
  if (SubVals > TInt::Mx-1) { SubVals = TInt::Mx-1; }
  const TSizeTy ValN1=LValN+TInt::GetRnd(int(SubVals));
  const TSizeTy ValN2=LValN+TInt::GetRnd(int(SubVals));
  const TSizeTy ValN3=LValN+TInt::GetRnd(int(SubVals));
  const TVal& Val1=ValT[ValN1];
  const TVal& Val2=ValT[ValN2];
  const TVal& Val3=ValT[ValN3];
  if (Val1<Val2){
    if (Val2<Val3){return ValN2;}
    else if (Val3<Val1){return ValN1;}
    else {return ValN3;}
  } else {
    if (Val1<Val3){return ValN1;}
    else if (Val3<Val2){return ValN2;}
    else {return ValN3;}
  }
}

template <class TVal, class TSizeTy>
TSizeTy TVec<TVal, TSizeTy>::Partition(const TSizeTy& MnLValN, const TSizeTy& MxRValN, const bool& Asc){
  TSizeTy PivotValN=GetPivotValN(MnLValN, MxRValN);
  Swap(PivotValN, MnLValN);
  TVal PivotVal=ValT[MnLValN];
  TSizeTy LValN=MnLValN-1;  TSizeTy RValN=MxRValN+1;
  forever {
    if (Asc){
      do {RValN--;} while (ValT[RValN]>PivotVal);
      do {LValN++;} while (ValT[LValN]<PivotVal);
    } else {
      do {RValN--;} while (ValT[RValN]<PivotVal);
      do {LValN++;} while (ValT[LValN]>PivotVal);
    }
    if (LValN<RValN){Swap(LValN, RValN);}
    else {return RValN;}
  };
}

template <class TVal, class TSizeTy>
void TVec<TVal, TSizeTy>::QSort(const TSizeTy& MnLValN, const TSizeTy& MxRValN, const bool& Asc){
  if (MnLValN<MxRValN){
    if (MxRValN-MnLValN<20){
      ISort(MnLValN, MxRValN, Asc);
    } else {
      TSizeTy SplitValN=Partition(MnLValN, MxRValN, Asc);
      QSort(MnLValN, SplitValN, Asc);
      QSort(SplitValN+1, MxRValN, Asc);
    }
  }
}

template <class TVal, class TSizeTy>
void TVec<TVal, TSizeTy>::Sort(const bool& Asc){
  QSort(0, Len()-1, Asc);
}

template <class TVal, class TSizeTy>
bool TVec<TVal, TSizeTy>::IsSorted(const bool& Asc) const {
  if (Asc){
    for (TSizeTy ValN=0; ValN<Vals-1; ValN++){
      if (ValT[ValN]>ValT[ValN+1]){return false;}}
  } else {
    for (TSizeTy ValN=0; ValN<Vals-1; ValN++){
      if (ValT[ValN]<ValT[ValN+1]){return false;}}
  }
  return true;
}

template <class TVal, class TSizeTy>
void TVec<TVal, TSizeTy>::Shuffle(TRnd& Rnd){
  if (Len() < TInt::Mx) {
    for (TSizeTy ValN=0; ValN<Vals-1; ValN++){
      const int Range = int(Vals-ValN);
      Swap(ValN, ValN+Rnd.GetUniDevInt(Range));
    }
  } else {
    for (TSizeTy ValN=0; ValN<Vals-1; ValN++){
      const TSizeTy Range = Vals-ValN;
      Swap(ValN, TSizeTy(ValN+Rnd.GetUniDevInt64(Range)));
    }
  }
}

template <class TVal, class TSizeTy>
void TVec<TVal, TSizeTy>::Reverse(){
  for (TSizeTy ValN=0; ValN<Vals/2; ValN++){
    Swap(ValN, Vals-ValN-1);}
}

template <class TVal, class TSizeTy>
void TVec<TVal, TSizeTy>::Merge(){
  IAssertR(!(IsShM && (MxVals == -1)), "Cannot write to shared memory");
  AssertR(MxVals!=-1, "This vector was obtained from TVecPool. Such vectors cannot change its size!");
  TVec<TVal, TSizeTy> SortedVec(*this); SortedVec.Sort();
  Clr();
  for (TSizeTy ValN=0; ValN<SortedVec.Len(); ValN++){
    if ((ValN==0)||(SortedVec[ValN-1]!=SortedVec[ValN])){
      Add(SortedVec[ValN]);}
  }
}

template <class TVal, class TSizeTy>
bool TVec<TVal, TSizeTy>::NextPerm() {
  // start with a sorted sequence to obtain all permutations
  TSizeTy First = 0, Last = Len(), Next = Len()-1;
  if (Last < 2) return false;
  for(; ; ) {
    // find rightmost element smaller than successor
    TSizeTy Next1 = Next;
    if (GetVal(--Next) < GetVal(Next1)) { // swap with rightmost element that's smaller, flip suffix
      TSizeTy Mid = Last;
      for (; GetVal(Next) >= GetVal(--Mid); ) { }
      Swap(Next, Mid);
      Reverse(Next1, Last-1);
      return true;
    }
    if (Next == First) { // pure descending, flip all
      Reverse();
      return false;
    }
  }
}

template <class TVal, class TSizeTy>
bool TVec<TVal, TSizeTy>::PrevPerm() {
  TSizeTy First = 0, Last = Len(), Next = Len()-1;
  if (Last < 2) return false;
  for(; ; ) {
    // find rightmost element not smaller than successor
    TSizeTy Next1 = Next;
    if (GetVal(--Next) >= GetVal(Next1)) { // swap with rightmost element that's not smaller, flip suffix
      TSizeTy Mid = Last;
      for (; GetVal(Next) < GetVal(--Mid); ) { }
      Swap(Next, Mid);
      Reverse(Next1, Last);
      return true;
    }
    if (Next == First) { // pure descending, flip all
      Reverse();
      return false;
    }
  }
}

template <class TVal, class TSizeTy>
void TVec<TVal, TSizeTy>::Intrs(const TVec<TVal, TSizeTy>& ValV){
  TVec<TVal, TSizeTy> IntrsVec;
  Intrs(ValV, IntrsVec);
  MoveFrom(IntrsVec);
}

template <class TVal, class TSizeTy>
void TVec<TVal, TSizeTy>::Union(const TVec<TVal, TSizeTy>& ValV){
  TVec<TVal, TSizeTy> UnionVec;
  Union(ValV, UnionVec);
  MoveFrom(UnionVec);
}

template <class TVal, class TSizeTy>
void TVec<TVal, TSizeTy>::Diff(const TVec<TVal, TSizeTy>& ValV){
  TVec<TVal, TSizeTy> DiffVec;
  Diff(ValV, DiffVec);
  MoveFrom(DiffVec);
}

template <class TVal, class TSizeTy>
void TVec<TVal, TSizeTy>::Intrs(const TVec<TVal, TSizeTy>& ValV, TVec<TVal, TSizeTy>& DstValV) const {
  DstValV.Clr();
  TSizeTy ValN1=0, ValN2=0;
  while ((ValN1<Len())&&(ValN2<ValV.Len())){
    const TVal& Val1=GetVal(ValN1);
    while ((ValN2<ValV.Len())&&(Val1>ValV.GetVal(ValN2))){
      ValN2++;}
    if ((ValN2<ValV.Len())&&(Val1==ValV.GetVal(ValN2))){
      DstValV.Add(Val1); ValN2++;}
    ValN1++;
  }
}

template <class TVal, class TSizeTy>
void TVec<TVal, TSizeTy>::Union(const TVec<TVal, TSizeTy>& ValV, TVec<TVal, TSizeTy>& DstValV) const {
  DstValV.Gen(TInt::GetMx(Len(), ValV.Len()), 0);
  TSizeTy ValN1=0, ValN2=0;
  while ((ValN1<Len())&&(ValN2<ValV.Len())){
    const TVal& Val1=GetVal(ValN1);
    const TVal& Val2=ValV.GetVal(ValN2);
    if (Val1<Val2){DstValV.Add(Val1); ValN1++;}
    else if (Val1>Val2){DstValV.Add(Val2); ValN2++;}
    else {DstValV.Add(Val1); ValN1++; ValN2++;}
  }
  for (TSizeTy RestValN1=ValN1; RestValN1<Len(); RestValN1++){
    DstValV.Add(GetVal(RestValN1));}
  for (TSizeTy RestValN2=ValN2; RestValN2<ValV.Len(); RestValN2++){
    DstValV.Add(ValV.GetVal(RestValN2));}
}

template <class TVal, class TSizeTy>
void TVec<TVal, TSizeTy>::Diff(const TVec<TVal, TSizeTy>& ValV, TVec<TVal, TSizeTy>& DstValV) const {
  DstValV.Clr();
  TSizeTy ValN1=0, ValN2=0;
  while (ValN1<Len() && ValN2<ValV.Len()) {
    const TVal& Val1 = GetVal(ValN1);
    while (ValN2<ValV.Len() && Val1>ValV.GetVal(ValN2)) ValN2++;
    if (ValN2<ValV.Len()) {
      if (Val1!=ValV.GetVal(ValN2)) { DstValV.Add(Val1); }
      ValN1++;
    }
  }
  for (TSizeTy RestValN1=ValN1; RestValN1<Len(); RestValN1++){
    DstValV.Add(GetVal(RestValN1));}
}

template <class TVal, class TSizeTy>
TSizeTy TVec<TVal, TSizeTy>::IntrsLen(const TVec<TVal, TSizeTy>& ValV) const {
  TSizeTy Cnt=0, ValN1=0, ValN2=0;
  while ((ValN1<Len())&&(ValN2<ValV.Len())){
    const TVal& Val1=GetVal(ValN1);
    while ((ValN2<ValV.Len())&&(Val1>ValV.GetVal(ValN2))){
      ValN2++;}
    if ((ValN2<ValV.Len())&&(Val1==ValV.GetVal(ValN2))){
      ValN2++; Cnt++;}
    ValN1++;
  }
  return Cnt;
}

template <class TVal, class TSizeTy>
TSizeTy TVec<TVal, TSizeTy>::UnionLen(const TVec<TVal, TSizeTy>& ValV) const {
  TSizeTy Cnt = 0, ValN1 = 0, ValN2 = 0;
  while ((ValN1 < Len()) && (ValN2 < ValV.Len())) {
    const TVal& Val1 = GetVal(ValN1);
    const TVal& Val2 = ValV.GetVal(ValN2);
    if (Val1 < Val2) {
      Cnt++; ValN1++;
    } else if (Val1 > Val2) {
      Cnt++; ValN2++;
    } else {
      Cnt++; ValN1++; ValN2++;
    }
  }
  Cnt += (Len() - ValN1) + (ValV.Len() - ValN2);
  return Cnt;
}

template <class TVal, class TSizeTy>
TSizeTy TVec<TVal, TSizeTy>::Count(const TVal& Val) const {
  TSizeTy Count = 0;
  for (TSizeTy i = 0; i < Len(); i++){
    if (Val == ValT[i]){Count++;}}
  return Count;
}

template <class TVal, class TSizeTy>
TSizeTy TVec<TVal, TSizeTy>::SearchBin(const TVal& Val) const {
  TSizeTy LValN=0, RValN=Len()-1;
  while (RValN>=LValN){
    TSizeTy ValN=(LValN+RValN)/2;
    if (Val==ValT[ValN]){return ValN;}
    if (Val<ValT[ValN]){RValN=ValN-1;} else {LValN=ValN+1;}
  }
  return -1;
}

template <class TVal, class TSizeTy>
TSizeTy TVec<TVal, TSizeTy>::SearchBin(const TVal& Val, TSizeTy& InsValN) const {
  TSizeTy LValN=0, RValN=Len()-1;
  while (RValN>=LValN){
    TSizeTy ValN=(LValN+RValN)/2;
    if (Val==ValT[ValN]){InsValN=ValN; return ValN;}
    if (Val<ValT[ValN]){RValN=ValN-1;} else {LValN=ValN+1;}
  }
  InsValN=LValN; return -1;
}

template <class TVal, class TSizeTy>
TSizeTy TVec<TVal, TSizeTy>::SearchBinLeft(const TVal& Val, TSizeTy& InsValN) const {
  TSizeTy LValN=0, RValN=Len()-1;
  while (RValN>=LValN){
    TSizeTy ValN=(LValN+RValN)/2;
    if (Val==ValT[ValN]){InsValN=ValN; return ValN;}
    if (Val<ValT[ValN]){RValN=ValN-1;} else {LValN=ValN+1;}
  }
  InsValN=RValN; return -1;
}

template <class TVal, class TSizeTy>
TSizeTy TVec<TVal, TSizeTy>::SearchForw(const TVal& Val, const TSizeTy& BValN) const {
  for (TSizeTy ValN=BValN; ValN<Vals; ValN++){
    if (Val==ValT[ValN]){return ValN;}}
  return -1;
}

template <class TVal, class TSizeTy>
TSizeTy TVec<TVal, TSizeTy>::SearchBack(const TVal& Val) const {
  for (TSizeTy ValN=Vals-1; ValN>=0; ValN--){
    if (Val==ValT[ValN]){return ValN;}}
  return -1;
}

template <class TVal, class TSizeTy>
TSizeTy TVec<TVal, TSizeTy>::SearchVForw(const TVec<TVal, TSizeTy>& ValV, const TSizeTy& BValN) const {
  TSizeTy ValVLen=ValV.Len();
  for (TSizeTy ValN=BValN; ValN<Vals-ValVLen+1; ValN++){
    bool Found=true;
    for (TSizeTy SubValN=0; SubValN<ValVLen; SubValN++){
      if (ValV[SubValN]!=GetVal(ValN+SubValN)){Found=false; break;}
    }
    if (Found){return ValN;}
  }
  return -1;
}

template <class TVal, class TSizeTy>
TSizeTy TVec<TVal, TSizeTy>::GetMxValN() const {
  if (Vals==0){return -1;}
  TSizeTy MxValN=0;
  for (TSizeTy ValN=1; ValN<Vals; ValN++){
    if (ValT[ValN]>ValT[MxValN]){MxValN=ValN;}
  }
  return MxValN;
}

/////////////////////////////////////////////////
// Common-Vector-Types
typedef TVec<TBool> TBoolV;
typedef TVec<TCh> TChV;
typedef TVec<TUCh> TUChV;
typedef TVec<TUInt> TUIntV;
typedef TVec<TInt> TIntV;
typedef TVec<TUInt64> TUInt64V;
typedef TVec<TFlt> TFltV;
typedef TVec<TSFlt> TSFltV;
typedef TVec<TAscFlt> TAscFltV;
typedef TVec<TStr> TStrV;
typedef TVec<TChA> TChAV;
typedef TVec<TIntPr> TIntPrV;
typedef TVec<TIntTr> TIntTrV;
typedef TVec<TIntQu> TIntQuV;
typedef TVec<TFltPr> TFltPrV;
typedef TVec<TFltTr> TFltTrV;
typedef TVec<TIntKd> TIntKdV;
typedef TVec<TUChIntPr> TUChIntPrV;
typedef TVec<TUChUInt64Pr> TUChUInt64PrV;
typedef TVec<TIntUInt64Pr> TIntUInt64PrV;
typedef TVec<TIntUInt64Kd> TIntUInt64KdV;
typedef TVec<TIntFltPr> TIntFltPrV;
typedef TVec<TIntFltPrKd> TIntFltPrKdV;
typedef TVec<TFltIntPr> TFltIntPrV;
typedef TVec<TFltUInt64Pr> TFltUInt64PrV;
typedef TVec<TFltStrPr> TFltStrPrV;
typedef TVec<TAscFltStrPr> TAscFltStrPrV;
typedef TVec<TIntStrPr> TIntStrPrV;
typedef TVec<TIntIntStrTr> TIntIntStrTrV;
typedef TVec<TIntIntFltTr> TIntIntFltTrV;
typedef TVec<TIntFltIntTr> TIntFltIntTrV;
typedef TVec<TIntStrIntTr> TIntStrIntTrV;
typedef TVec<TIntStrStrTr> TIntStrStrTrV;
typedef TVec<TIntKd> TIntKdV;
typedef TVec<TUIntIntKd> TUIntIntKdV;
typedef TVec<TIntFltKd> TIntFltKdV;
typedef TVec<TIntPrFltKd> TIntPrFltKdV;
typedef TVec<TIntStrKd> TIntStrKdV;
typedef TVec<TIntStrPrPr> TIntStrPrPrV;
typedef TVec<TIntStrVPr> TIntStrVPrV;
typedef TVec<TIntIntVIntTr> TIntIntVIntTrV;
typedef TVec<TIntIntIntVTr> TIntIntIntVTrV;
typedef TVec<TUInt64IntPr> TUInt64IntPrV;
typedef TVec<TUInt64FltPr> TUInt64FltPrV;
typedef TVec<TUInt64StrPr> TUInt64StrPrV;
typedef TVec<TUInt64IntKd> TUInt64IntKdV;
typedef TVec<TUInt64FltKd> TUInt64FltKdV;
typedef TVec<TUInt64StrKd> TUInt64StrKdV;
typedef TVec<TFltBoolKd> TFltBoolKdV;
typedef TVec<TFltIntKd> TFltIntKdV;
typedef TVec<TFltUInt64Kd> TFltUInt64KdV;
typedef TVec<TFltIntPrKd> TFltIntPrKdV;
typedef TVec<TFltKd> TFltKdV;
typedef TVec<TFltStrKd> TFltStrKdV;
typedef TVec<TFltStrPrPr> TFltStrPrPrV;
typedef TVec<TFltIntIntTr> TFltIntIntTrV;
typedef TVec<TFltFltStrTr> TFltFltStrTrV;
typedef TVec<TAscFltIntPr> TAscFltIntPrV;
typedef TVec<TAscFltIntKd> TAscFltIntKdV;
typedef TVec<TStrPr> TStrPrV;
typedef TVec<TStrIntPr> TStrIntPrV;
typedef TVec<TStrFltPr> TStrFltPrV;
typedef TVec<TStrIntKd> TStrIntKdV;
typedef TVec<TStrFltKd> TStrFltKdV;
typedef TVec<TStrAscFltKd> TStrAscFltKdV;
typedef TVec<TStrTr> TStrTrV;
typedef TVec<TStrQu> TStrQuV;
typedef TVec<TStrFltFltTr> TStrFltFltTrV;
typedef TVec<TStrStrIntTr> TStrStrIntTrV;
typedef TVec<TStrKd> TStrKdV;
typedef TVec<TStrStrVPr> TStrStrVPrV;
typedef TVec<TStrVIntPr> TStrVIntPrV;
typedef TVec<TFltIntIntIntQu> TFltIntIntIntQuV;
typedef TVec<TIntStrIntIntQu> TIntStrIntIntQuV;
typedef TVec<TIntIntPrPr> TIntIntPrPrV;
typedef TVec<TFltV> TFltVFltV;

//#//////////////////////////////////////////////
/// Vector Pool. ##TVecPool
template <class TVal, class TSizeTy=int>
class TVecPool {
public:
  typedef TPt<TVecPool<TVal, TSizeTy> > PVecPool;
  typedef TVec<TVal, TSizeTy> TValV;
private:
  TCRef CRef;
  TBool FastCopy;
  TSize GrowBy, MxVals, Vals;
  TVal EmptyVal;                // Empty value/vector
  TVal *ValBf;                  // Buffer for storing all the values
  TVec<uint64, int> IdToOffV;   // Id to one past last (Vector starts at [id-1]). Vector length is IdToOff[id]-IdToOff[id-1]
private:
  void Resize(const TSize& _MxVals);
public:
  /// Vector pool constructor. ##TVecPool::TVecPool
  TVecPool(const TSize& ExpectVals=0, const TSize& _GrowBy=1000000, const bool& _FastCopy=false, const TVal& _EmptyVal=TVal());
  TVecPool(const TVecPool<TVal, TSizeTy>& Pool);
  TVecPool(TSIn& SIn);
  ~TVecPool() { if (ValBf != NULL) { delete [] ValBf; } ValBf=NULL; }
  static PVecPool New(const TSize& ExpectVals=0, const TSize& GrowBy=1000000, const bool& FastCopy=false) {
    return new TVecPool(ExpectVals, GrowBy, FastCopy); }
  static PVecPool Load(TSIn& SIn) { return new TVecPool(SIn); }
  static PVecPool Load(const TStr& FNm) { TFIn FIn(FNm); return Load(FIn); }
  void Save(TSOut& SOut) const;
  TVecPool& operator = (const TVecPool& Pool);

  /// Returns the total number of vectors stored in the vector pool.
  int GetVecs() const { return IdToOffV.Len(); }
  /// Returns the total number of values stored in the vector pool.
  TSize GetVals() const { return Vals; }
  /// Tests whether vector of id \c VId is in the pool.
  bool IsVId(const int& VId) const { return (0 <= VId) && (VId < IdToOffV.Len()); }
  /// Returns the total capacity of the pool.
  uint64 Reserved() const { return MxVals; }
  /// Reserves enough capacity for the pool to store \c MxVals elements.
  void Reserve(const TSize& MxVals) { Resize(MxVals); }
  /// Returns the reference to an empty value.
  const TVal& GetEmptyVal() const { return EmptyVal; }
  /// Sets the empty value.
  void SetEmptyVal(const TVal& _EmptyVal) { EmptyVal = _EmptyVal; }
  /// Returns the total memory footprint (in bytes) of the pool.
  uint64 GetMemUsed() const {
    return sizeof(TCRef)+sizeof(TBool)+3*sizeof(TSize)+sizeof(TVal*)+MxVals*sizeof(TVal);}

  /// Adds vector \c ValV to the pool and returns its id.
  int AddV(const TValV& ValV);
  /// Adds a vector of length \c ValVLen to the pool and returns its id. ##TVecPool::AddEmptyV
  int AddEmptyV(const int& ValVLen);
  /// Returns the number of elements in the vector with id \c VId.
  int GetVLen(const int& VId) const { if (VId==0){return 0;} else {return int(IdToOffV[VId]-IdToOffV[VId-1]);}}
  /// Returns pointer to the first element of the vector with id \c VId.
  TVal* GetValVPt(const int& VId) const {
    if (GetVLen(VId)==0){return (TVal*)&EmptyVal;}
    else {return ValBf+IdToOffV[VId-1];}}
  /// Returns \c ValV which is a reference (not a copy) to vector with id \c VId. ##TVecPool::GetV
  void GetV(const int& VId, TValV& ValV) const {
    if (GetVLen(VId)==0){ValV.Clr();}
    else { ValV.GenExt(GetValVPt(VId), GetVLen(VId)); } }
  /// Sets the values of vector \c VId with those in \c ValV.
  void PutV(const int& VId, const TValV& ValV) {
    IAssert(IsVId(VId) && GetVLen(VId) == ValV.Len());
    if (FastCopy) {
      memcpy(GetValVPt(VId), ValV.BegI(), sizeof(TVal)*ValV.Len()); }
    else { TVal* ValPt = GetValVPt(VId);
      for (::TSize ValN=0; ValN < ::TSize(ValV.Len()); ValN++, ValPt++) { *ValPt=ValV[ValN]; }
    } }
  /// Deletes all elements of value \c DelVal from all vectors. ##TVecPool::CompactPool
  void CompactPool(const TVal& DelVal);
  /// Shuffles the order of all elements in the pool. ##TVecPool::ShuffleAll
  void ShuffleAll(TRnd& Rnd=TInt::Rnd);

  /// Clears the contents of the pool. ##TVecPool::Clr
  void Clr(bool DoDel = true) {
    IdToOffV.Clr(DoDel);  MxVals=0;  Vals=0;
    if (DoDel && ValBf!=NULL) { delete [] ValBf; ValBf=NULL;}
    if (! DoDel) { PutAll(EmptyVal); } }
  /// Sets the values of all elements in the pool to \c Val.
  void PutAll(const TVal& Val) {
    for (TSize ValN = 0; ValN < MxVals; ValN++) { ValBf[ValN]=Val; } }
  friend class TPt<TVecPool<TVal> >;
};

template <class TVal, class TSizeTy>
void TVecPool<TVal, TSizeTy>::Resize(const TSize& _MxVals){
  if (_MxVals <= MxVals){ return; } else { MxVals = _MxVals; }
  if (ValBf == NULL) {
    try { ValBf = new TVal [MxVals]; }
    catch (std::exception Ex) {
      FailR(TStr::Fmt("TVecPool::Resize 1: %s, MxVals: %s. [Program failed to allocate more memory. Solution: Get a bigger machine and a 64-bit compiler.]", Ex.what(), TInt::GetStr(uint64(_MxVals)).CStr()).CStr()); }
    IAssert(ValBf != NULL);
    if (EmptyVal != TVal()) { PutAll(EmptyVal); }
  } else {
    // printf("*** Resize vector pool: %llu -> %llu\n", uint64(Vals), uint64(MxVals));
    TVal* NewValBf = NULL;
    try { NewValBf = new TVal [MxVals]; }
    catch (std::exception Ex) {
      FailR(TStr::Fmt("TVecPool::Resize 1: %s, MxVals: %s. [Program failed to allocate more memory. Solution: Get a bigger machine and a 64-bit compiler.]", Ex.what(), TInt::GetStr(uint64(_MxVals)).CStr()).CStr()); }
    IAssert(NewValBf != NULL);
    if (FastCopy) {
      memcpy(NewValBf, ValBf, Vals*sizeof(TVal)); }
    else {
      for (TSize ValN = 0; ValN < Vals; ValN++){ NewValBf[ValN] = ValBf[ValN]; } }
    if (EmptyVal != TVal()) { // init empty values
      for (TSize ValN = Vals; ValN < MxVals; ValN++) { NewValBf[ValN] = EmptyVal; }
    }
    delete [] ValBf;
    ValBf = NewValBf;
  }
}

template <class TVal, class TSizeTy>
TVecPool<TVal, TSizeTy>::TVecPool(const TSize& ExpectVals, const TSize& _GrowBy, const bool& _FastCopy, const TVal& _EmptyVal) : GrowBy(_GrowBy), MxVals(0), Vals(0), EmptyVal(_EmptyVal), ValBf(NULL) {
  IdToOffV.Add(0);
  Resize(ExpectVals);
}

template <class TVal, class TSizeTy>
TVecPool<TVal, TSizeTy>::TVecPool(const TVecPool& Pool) : FastCopy(Pool.FastCopy), GrowBy(Pool.GrowBy), MxVals(Pool.MxVals), Vals(Pool.Vals), EmptyVal(Pool.EmptyVal), IdToOffV(Pool.IdToOffV) {
  try {
    ValBf = new TVal [MxVals]; }
  catch (std::exception Ex) {
    FailR(TStr::Fmt("TVecPool::TVecPool: %s, MxVals: %s. [Program failed to allocate memory. Solution: Get a bigger machine and a 64-bit compiler.]", Ex.what(), TInt::GetStr(uint64(MxVals)).CStr()).CStr()); }
  IAssert(ValBf != NULL);
  if (FastCopy) {
    memcpy(ValBf, Pool.ValBf, MxVals*sizeof(TVal)); }
  else {
    for (TSize ValN = 0; ValN < MxVals; ValN++){ ValBf[ValN] = Pool.ValBf[ValN]; } }
}

template <class TVal, class TSizeTy>
TVecPool<TVal, TSizeTy>::TVecPool(TSIn& SIn) : FastCopy(SIn) {
  uint64 _GrowBy, _MxVals, _Vals;
  SIn.Load(_GrowBy); SIn.Load(_MxVals);  SIn.Load(_Vals);
  IAssertR(_GrowBy<TSizeMx && _MxVals<TSizeMx && _Vals<TSizeMx, "This is a 64-bit vector pool. Use a 64-bit compiler.");
  GrowBy=TSize(_GrowBy);  MxVals=TSize(_Vals);  Vals=TSize(_Vals); //note MxVals==Vals
  EmptyVal = TVal(SIn);
  if (MxVals==0) { ValBf = NULL; } else { ValBf = new TVal [MxVals]; }
  for (TSize ValN = 0; ValN < Vals; ValN++) { ValBf[ValN] = TVal(SIn); }
  { TInt MxVals(SIn), Vals(SIn);
    IdToOffV.Gen(Vals);
    for (int ValN = 0; ValN < Vals; ValN++) {
      uint64 Offset;  SIn.Load(Offset);  IAssert(Offset < TSizeMx);
      IdToOffV[ValN]=TSize(Offset);
    } }
}

template <class TVal, class TSizeTy>
void TVecPool<TVal, TSizeTy>::Save(TSOut& SOut) const {
  SOut.Save(FastCopy);
  uint64 _GrowBy=GrowBy, _MxVals=MxVals, _Vals=Vals;
  SOut.Save(_GrowBy); SOut.Save(_MxVals);  SOut.Save(_Vals);
  SOut.Save(EmptyVal);
  for (TSize ValN = 0; ValN < Vals; ValN++) { ValBf[ValN].Save(SOut); }
  { SOut.Save(IdToOffV.Len());  SOut.Save(IdToOffV.Len());
    for (int ValN = 0; ValN < IdToOffV.Len(); ValN++) {
      const uint64 Offset=IdToOffV[ValN];  SOut.Save(Offset);
    } }
}

template <class TVal, class TSizeTy>
TVecPool<TVal, TSizeTy>& TVecPool<TVal, TSizeTy>::operator = (const TVecPool& Pool) {
  if (this!=&Pool) {
    FastCopy = Pool.FastCopy;
    GrowBy = Pool.GrowBy;
    MxVals = Pool.MxVals;
    Vals = Pool.Vals;
    EmptyVal = Pool.EmptyVal;
    IdToOffV=Pool.IdToOffV;
    try {
      ValBf = new TVal [MxVals]; }
    catch (std::exception Ex) {
      FailR(TStr::Fmt("TVecPool::operator=: %s, MxVals: %s. [Program failed to allocate memory. Solution: Get a bigger machine and a 64-bit compiler.]", Ex.what(), TInt::GetStr(uint64(MxVals)).CStr()).CStr()); }
    IAssert(ValBf != NULL);
    if (FastCopy) {
      memcpy(ValBf, Pool.ValBf, Vals*sizeof(TVal)); }
    else {
      for (TSize ValN = 0; ValN < Vals; ValN++){ ValBf[ValN] = Pool.ValBf[ValN]; } }
  }
  return *this;
}

template <class TVal, class TSizeTy>
int TVecPool<TVal, TSizeTy>::AddV(const TValV& ValV) {
  const TSize ValVLen = ValV.Len();
  if (ValVLen == 0) { return 0; }
  if (MxVals < Vals+ValVLen) { Resize(Vals+MAX(ValVLen, GrowBy)); }
  if (FastCopy) { memcpy(ValBf+Vals, ValV.BegI(), sizeof(TVal)*ValV.Len()); }
  else { for (TSize ValN=0; ValN < ValVLen; ValN++) { ValBf[Vals+ValN]=ValV[ValN]; } }
  Vals+=ValVLen;  IdToOffV.Add(Vals);
  return IdToOffV.Len()-1;
}

template <class TVal, class TSizeTy>
int TVecPool<TVal, TSizeTy>::AddEmptyV(const int& ValVLen) {
  if (ValVLen==0){return 0;}
  if (MxVals < Vals+ValVLen){Resize(Vals+MAX(TSize(ValVLen), GrowBy)); }
  Vals+=ValVLen; IdToOffV.Add(Vals);
  return IdToOffV.Len()-1;
}

// Delete all elements of value DelVal from all vectors. Empty space is left at the end of the pool.
template <class TVal, class TSizeTy>
void TVecPool<TVal, TSizeTy>::CompactPool(const TVal& DelVal) {
  ::TSize TotalDel=0, NDel=0;
  // printf("Compacting %d vectors\n", IdToOffV.Len());
  for (int vid = 1; vid < IdToOffV.Len(); vid++) {
    // if (vid % 10000000 == 0) { printf(" %dm", vid/1000000);  fflush(stdout); }
    const uint Len = GetVLen(vid);
    TVal* ValV = GetValVPt(vid);
    if (TotalDel > 0) { IdToOffV[vid-1] -= TotalDel; } // update end of vector
    if (Len == 0) { continue; }
    NDel = 0;
    for (TVal* v = ValV; v < ValV+Len-NDel; v++) {
      if (*v == DelVal) {
        TVal* Beg = v;
        while (*v == DelVal && v < ValV+Len) { v++; NDel++; }
        memcpy(Beg, v, sizeof(TVal)*int(Len - ::TSize(v - ValV)));
        v -= NDel;
      }
    }
    memcpy(ValV-TotalDel, ValV, sizeof(TVal)*Len);  // move data
    TotalDel += NDel;
  }
  IdToOffV.Last() -= TotalDel;
  for (::TSize i = Vals-TotalDel; i < Vals; i++) { ValBf[i] = EmptyVal; }
  Vals -= TotalDel;
  // printf("  deleted %llu elements from the pool\n", TotalDel);
}

// shuffles all the order of elements in the pool (does not respect vector boundaries)
template <class TVal, class TSizeTy>
void TVecPool<TVal, TSizeTy>::ShuffleAll(TRnd& Rnd) {
  for (::TSize n = Vals-1; n > 0; n--) {
    const ::TSize k = ::TSize(((uint64(Rnd.GetUniDevInt())<<32) | uint64(Rnd.GetUniDevInt())) % (n+1));
    const TVal Tmp = ValBf[n];
    ValBf[n] = ValBf[k];
    ValBf[k] = Tmp;
  }
}


/////////////////////////////////////////////////
// Below are old 32-bit implementations of TVec and other classes.
// Old TVec takes at most 2G elements.
// The new vector class supports 64-bits for the number of elements,
// but also allows 32-bits for backward compatibility.
// by Jure (Jan 2013)
namespace TGLib_OLD {
/////////////////////////////////////////////////
// Vector Pool
template<class TVal>
class TVecPool {
public:
  typedef TPt<TVecPool<TVal> > PVecPool;
  typedef TVec<TVal> TValV;
private:
  TCRef CRef;
  TBool FastCopy;
  ::TSize GrowBy, MxVals, Vals;
  TVal EmptyVal;           // empty vector
  TVal *ValBf;             // buffer storing all the values
  TVec< ::TSize> IdToOffV; // id to one past last (vector starts at [id-1])
private:
  void Resize(const ::TSize& _MxVals);
public:
  TVecPool(const ::TSize& ExpectVals=0, const ::TSize& _GrowBy=1000000, const bool& _FastCopy=false, const TVal& _EmptyVal=TVal());
  TVecPool(const TVecPool& Pool);
  TVecPool(TSIn& SIn);
  ~TVecPool() { if (ValBf != NULL) { delete [] ValBf; } ValBf=NULL; }
  static PVecPool New(const ::TSize& ExpectVals=0, const ::TSize& GrowBy=1000000, const bool& FastCopy=false) {
    return new TVecPool(ExpectVals, GrowBy, FastCopy); }
  static PVecPool Load(TSIn& SIn) { return new TVecPool(SIn); }
  static PVecPool Load(const TStr& FNm) { TFIn FIn(FNm); return Load(FIn); }
  void Save(TSOut& SOut) const;

  TVecPool& operator = (const TVecPool& Pool);

  ::TSize GetVals() const { return Vals; }
  ::TSize GetVecs() const { return IdToOffV.Len(); }
  bool IsVId(const int& VId) const { return (0 <= VId) && (VId < IdToOffV.Len()); }
  ::TSize Reserved() const { return MxVals; }
  void Reserve(const ::TSize& MxVals) { Resize(MxVals); }
  const TVal& GetEmptyVal() const { return EmptyVal; }
  void SetEmptyVal(const TVal& _EmptyVal) { EmptyVal = _EmptyVal; }
  ::TSize GetMemUsed() const {
    return sizeof(TCRef)+sizeof(TBool)+3*sizeof(TSize)+sizeof(TVal*)+MxVals*sizeof(TVal);}

  int AddV(const TValV& ValV);
  int AddEmptyV(const int& ValVLen);
  uint GetVLen(const int& VId) const {
    if (VId==0){return 0;}
    else {return uint(IdToOffV[VId]-IdToOffV[VId-1]);}}
  TVal* GetValVPt(const int& VId) const {
    if (GetVLen(VId)==0){return (TVal*)&EmptyVal;}
    else {return ValBf+IdToOffV[VId-1];}}
  void GetV(const int& VId, TValV& ValV) const {
    if (GetVLen(VId)==0){ValV.Clr();}
    else { ValV.GenExt(GetValVPt(VId), GetVLen(VId)); } }
  void PutV(const int& VId, const TValV& ValV) {
    IAssert(IsVId(VId) && GetVLen(VId) == ValV.Len());
    if (FastCopy) {
      memcpy(GetValVPt(VId), ValV.BegI(), sizeof(TVal)*ValV.Len()); }
    else { TVal* ValPt = GetValVPt(VId);
      for (uint ValN=0; ValN < uint(ValV.Len()); ValN++, ValPt++) { *ValPt=ValV[ValN]; }
    }
  }
  void CompactPool(const TVal& DelVal); // delete all elements of value DelVal from all vectors
  void ShuffleAll(TRnd& Rnd=TInt::Rnd); // shuffles all the order of elements in the Pool (does not respect vector boundaries)

  //bool HasIdMap() const { return ! IdToOffV.Empty(); }
  //void ClrIdMap() { IdToOffV.Clr(true); }
  void Clr(bool DoDel = true) {
    IdToOffV.Clr(DoDel);  MxVals=0;  Vals=0;
    if (DoDel && ValBf!=NULL) { delete [] ValBf; ValBf=NULL;}
    if (! DoDel) { PutAll(EmptyVal); }
  }
  void PutAll(const TVal& Val) {
    for (TSize ValN = 0; ValN < MxVals; ValN++) { ValBf[ValN]=Val; } }

  friend class TPt<TVecPool<TVal> >;
};

template <class TVal>
void TVecPool<TVal>::Resize(const ::TSize& _MxVals){
  if (_MxVals <= MxVals){ return; } else { MxVals = _MxVals; }
  if (ValBf == NULL) {
    try { ValBf = new TVal [MxVals]; }
    catch (std::exception Ex) {
      FailR(TStr::Fmt("TVecPool::Resize 1: %s, MxVals: %d. [Program failed to allocate more memory. Solution: Get a bigger machine and a 64-bit compiler.]", Ex.what(), _MxVals).CStr()); }
    IAssert(ValBf != NULL);
    if (EmptyVal != TVal()) { PutAll(EmptyVal); }
  } else {
    // printf("*** Resize vector pool: %llu -> %llu\n", uint64(Vals), uint64(MxVals));
    TVal* NewValBf = NULL;
    try { NewValBf = new TVal [MxVals]; }
    catch (std::exception Ex) { FailR(TStr::Fmt("TVecPool::Resize 2: %s, MxVals: %d. [Program failed to allocate more memory. Solution: Get a bigger machine and a 64-bit compiler.]", Ex.what(), _MxVals).CStr()); }
    IAssert(NewValBf != NULL);
    if (FastCopy) {
      memcpy(NewValBf, ValBf, Vals*sizeof(TVal)); }
    else {
      for (TSize ValN = 0; ValN < Vals; ValN++){ NewValBf[ValN] = ValBf[ValN]; } }
    if (EmptyVal != TVal()) { // init empty values
      for (TSize ValN = Vals; ValN < MxVals; ValN++) { NewValBf[ValN] = EmptyVal; }
    }
    delete [] ValBf;
    ValBf = NewValBf;
  }
}

template <class TVal>
TVecPool<TVal>::TVecPool(const ::TSize& ExpectVals, const ::TSize& _GrowBy, const bool& _FastCopy, const TVal& _EmptyVal) :
  GrowBy(_GrowBy), MxVals(0), Vals(0), EmptyVal(_EmptyVal), ValBf(NULL) {
  IdToOffV.Add(0);
  Resize(ExpectVals);
}

template <class TVal>
TVecPool<TVal>::TVecPool(const TVecPool& Pool):
  FastCopy(Pool.FastCopy), GrowBy(Pool.GrowBy),
  MxVals(Pool.MxVals), Vals(Pool.Vals), EmptyVal(Pool.EmptyVal), IdToOffV(Pool.IdToOffV) {
  try { ValBf = new TVal [MxVals]; }
  catch (std::exception Ex) { FailR(TStr::Fmt("TVecPool::TVecPool: %s, MxVals: %d", Ex.what(), MxVals).CStr()); }
  IAssert(ValBf != NULL);
  if (FastCopy) {
    memcpy(ValBf, Pool.ValBf, MxVals*sizeof(TVal)); }
  else {
    for (TSize ValN = 0; ValN < MxVals; ValN++){ ValBf[ValN] = Pool.ValBf[ValN]; } }
}

template <class TVal>
TVecPool<TVal>::TVecPool(TSIn& SIn):
  FastCopy(SIn) {
  uint64 _GrowBy, _MxVals, _Vals;
  SIn.Load(_GrowBy); SIn.Load(_MxVals);  SIn.Load(_Vals);
  IAssert(_GrowBy<TSizeMx && _MxVals<TSizeMx && _Vals<TSizeMx);
  GrowBy=TSize(_GrowBy);  MxVals=TSize(_Vals);  Vals=TSize(_Vals); //note MxVals==Vals
  EmptyVal = TVal(SIn);
  if (MxVals==0) { ValBf = NULL; } else { ValBf = new TVal [MxVals]; }
  for (TSize ValN = 0; ValN < Vals; ValN++) { ValBf[ValN] = TVal(SIn); }
  { TInt MxVals(SIn), Vals(SIn);
  IdToOffV.Gen(Vals);
  for (int ValN = 0; ValN < Vals; ValN++) {
    uint64 Offset;  SIn.Load(Offset);  IAssert(Offset < TSizeMx);
    IdToOffV[ValN]=TSize(Offset);
  } }
}

template <class TVal>
void TVecPool<TVal>::Save(TSOut& SOut) const {
  SOut.Save(FastCopy);
  uint64 _GrowBy=GrowBy, _MxVals=MxVals, _Vals=Vals;
  SOut.Save(_GrowBy); SOut.Save(_MxVals);  SOut.Save(_Vals);
  SOut.Save(EmptyVal);
  for (TSize ValN = 0; ValN < Vals; ValN++) { ValBf[ValN].Save(SOut); }
  { SOut.Save(IdToOffV.Len());  SOut.Save(IdToOffV.Len());
  for (int ValN = 0; ValN < IdToOffV.Len(); ValN++) {
    const uint64 Offset=IdToOffV[ValN];  SOut.Save(Offset);
  } }
}

template <class TVal>
TVecPool<TVal>& TVecPool<TVal>::operator = (const TVecPool& Pool) {
  if (this!=&Pool) {
    FastCopy = Pool.FastCopy;
    GrowBy = Pool.GrowBy;
    MxVals = Pool.MxVals;
    Vals = Pool.Vals;
    EmptyVal = Pool.EmptyVal;
    IdToOffV=Pool.IdToOffV;
    try { ValBf = new TVal [MxVals]; }
    catch (std::exception Ex) { FailR(TStr::Fmt("TVec::operator= : %s, MxVals: %d", Ex.what(), MxVals).CStr()); }
    IAssert(ValBf != NULL);
    if (FastCopy) {
      memcpy(ValBf, Pool.ValBf, Vals*sizeof(TVal)); }
    else {
      for (uint64 ValN = 0; ValN < Vals; ValN++){ ValBf[ValN] = Pool.ValBf[ValN]; } }
  }
  return *this;
}

template<class TVal>
int TVecPool<TVal>::AddV(const TValV& ValV) {
  const ::TSize ValVLen = ValV.Len();
  if (ValVLen == 0) { return 0; }
  if (MxVals < Vals+ValVLen) { Resize(Vals+MAX(ValVLen, GrowBy)); }
  if (FastCopy) { memcpy(ValBf+Vals, ValV.BegI(), sizeof(TVal)*ValV.Len()); }
  else { for (uint ValN=0; ValN < ValVLen; ValN++) { ValBf[Vals+ValN]=ValV[ValN]; } }
  Vals+=ValVLen;  IdToOffV.Add(Vals);
  return IdToOffV.Len()-1;
}

template<class TVal>
int TVecPool<TVal>::AddEmptyV(const int& ValVLen) {
  if (ValVLen==0){return 0;}
  if (MxVals < Vals+ValVLen){Resize(Vals+MAX(TSize(ValVLen), GrowBy)); }
  Vals+=ValVLen; IdToOffV.Add(Vals);
  return IdToOffV.Len()-1;
}

// delete all elements of value DelVal from all vectors
// empty space is left at the end of the pool
template<class TVal>
void TVecPool<TVal>::CompactPool(const TVal& DelVal) {
  ::TSize TotalDel=0, NDel=0;
  // printf("Compacting %d vectors\n", IdToOffV.Len());
  for (int vid = 1; vid < IdToOffV.Len(); vid++) {
    // if (vid % 10000000 == 0) { printf(" %dm", vid/1000000);  fflush(stdout); }
    const uint Len = GetVLen(vid);
    TVal* ValV = GetValVPt(vid);
    if (TotalDel > 0) { IdToOffV[vid-1] -= TotalDel; } // update end of vector
    if (Len == 0) { continue; }
    NDel = 0;
    for (TVal* v = ValV; v < ValV+Len-NDel; v++) {
      if (*v == DelVal) {
        TVal* Beg = v;
        while (*v == DelVal && v < ValV+Len) { v++; NDel++; }
        memcpy(Beg, v, sizeof(TVal)*int(Len - ::TSize(v - ValV)));
        v -= NDel;
      }
    }
    memcpy(ValV-TotalDel, ValV, sizeof(TVal)*Len);  // move data
    TotalDel += NDel;
  }
  IdToOffV.Last() -= TotalDel;
  for (::TSize i = Vals-TotalDel; i < Vals; i++) { ValBf[i] = EmptyVal; }
  Vals -= TotalDel;
  // printf("  deleted %llu elements from the pool\n", TotalDel);
}

// shuffles all the order of elements in the pool (does not respect vector boundaries)
template<class TVal>
void TVecPool<TVal>::ShuffleAll(TRnd& Rnd) {
  for (::TSize n = Vals-1; n > 0; n--) {
    const ::TSize k = ::TSize(((uint64(Rnd.GetUniDevInt())<<32) | uint64(Rnd.GetUniDevInt())) % (n+1));
    const TVal Tmp = ValBf[n];
    ValBf[n] = ValBf[k];
    ValBf[k] = Tmp;
  }
}

}; // namespace TGLib_OLD

typedef TVecPool<TInt> TIntVecPool;
typedef TPt<TIntVecPool> PIntVecPool;

/////////////////////////////////////////////////
// Vector-Pointer
template <class TVal>
class PVec{
private:
  TCRef CRef;
public:
  TVec<TVal> V;
public:
  PVec<TVal>(): V(){}
  PVec<TVal>(const PVec<TVal>& Vec): V(Vec.V){}
  static TPt<PVec<TVal> > New(){
    return new PVec<TVal>();}
  PVec<TVal>(const int& MxVals, const int& Vals): V(MxVals, Vals){}
  static TPt<PVec<TVal> > New(const int& MxVals, const int& Vals){
    return new PVec<TVal>(MxVals, Vals);}
  PVec<TVal>(const TVec<TVal>& _V): V(_V){}
  static TPt<PVec<TVal> > New(const TVec<TVal>& V){
    return new PVec<TVal>(V);}
  explicit PVec<TVal>(TSIn& SIn): V(SIn){}
  static TPt<PVec<TVal> > Load(TSIn& SIn){return new PVec<TVal>(SIn);}
  void Save(TSOut& SOut) const {V.Save(SOut);}

  PVec<TVal>& operator=(const PVec<TVal>& Vec){
    if (this!=&Vec){V=Vec.V;} return *this;}
  bool operator==(const PVec<TVal>& Vec) const {return V==Vec.V;}
  bool operator<(const PVec<TVal>& Vec) const {return V<Vec.V;}
  TVal& operator[](const int& ValN) const {return V[ValN];}

  bool Empty() const {return V.Empty();}
  int Len() const {return V.Len();}
  TVal GetVal(const int& ValN) const {return V[ValN];}

  int Add(const TVal& Val){return V.Add(Val);}

  friend class TPt<PVec<TVal> >;
};

/////////////////////////////////////////////////
// Common-Vector-Pointer-Types
typedef PVec<TFlt> TFltVP;
typedef TPt<TFltVP> PFltV;
typedef PVec<TAscFlt> TAscFltVP;
typedef TPt<TAscFltVP> PAscFltV;
typedef PVec<TStr> TStrVP;
typedef TPt<TStrVP> PStrV;

/////////////////////////////////////////////////
// 2D-Vector
template <class TVal, class TSizeTy = int>
class TVVec{
private:
  TInt64 XDim, YDim;
  TVec<TVal, TSizeTy> ValV;
public:
  TVVec(): XDim(), YDim(), ValV(){}
  TVVec(const TVVec& Vec):
    XDim(Vec.XDim), YDim(Vec.YDim), ValV(Vec.ValV){}
  TVVec(const TSizeTy& _XDim, const TSizeTy& _YDim):
    XDim(), YDim(), ValV(){Gen(_XDim, _YDim);}
  explicit TVVec(const TVec<TVal,TSizeTy>& _ValV, const TSizeTy& _XDim, const TSizeTy& _YDim):
    XDim(_XDim), YDim(_YDim), ValV(_ValV){ IAssert(ValV.Len()==XDim*YDim); }
  explicit TVVec(TSIn& SIn) {Load(SIn);}
  void Load(TSIn& SIn){XDim.Load(SIn); YDim.Load(SIn); ValV.Load(SIn);}
  void Save(TSOut& SOut) const {
    XDim.Save(SOut); YDim.Save(SOut); ValV.Save(SOut);}

  TVVec<TVal, TSizeTy>& operator=(const TVVec<TVal, TSizeTy>& Vec){
    if (this!=&Vec){XDim=Vec.XDim; YDim=Vec.YDim; ValV=Vec.ValV;} return *this;}
  bool operator==(const TVVec& Vec) const {
    return (XDim==Vec.XDim)&&(YDim==Vec.YDim)&&(ValV==Vec.ValV);}

  bool Empty() const {return ValV.Len()==0;}
  void Clr(){XDim=0; YDim=0; ValV.Clr();}
  void Gen(const TSizeTy& _XDim, const TSizeTy& _YDim){
    Assert((_XDim>=0)&&(_YDim>=0));
    XDim=_XDim; YDim=_YDim; ValV.Gen(XDim*YDim);}
  TSizeTy GetXDim() const {return XDim;}
  TSizeTy GetYDim() const {return YDim;}
  TSizeTy GetRows() const {return XDim;}
  TSizeTy GetCols() const {return YDim;}
  TVec<TVal, TSizeTy>& Get1DVec(){return ValV;}

  const TVal& At(const TSizeTy& X, const TSizeTy& Y) const {
    Assert((0<=X)&&(X<TSizeTy(XDim))&&(0<=Y)&&(Y<TSizeTy(YDim)));
    return ValV[X*YDim+Y];}
  TVal& At(const TSizeTy& X, const TSizeTy& Y){
    Assert((0<=X)&&(X<TSizeTy(XDim))&&(0<=Y)&&(Y<TSizeTy(YDim)));
    return ValV[X*YDim+Y];}
  TVal& operator()(const TSizeTy& X, const TSizeTy& Y){
    return At(X, Y);}
  const TVal& operator()(const TSizeTy& X, const TSizeTy& Y) const {
    return At(X, Y);}

  void PutXY(const TSizeTy& X, const TSizeTy& Y, const TVal& Val){At(X, Y)=Val;}
  void PutAll(const TVal& Val){ValV.PutAll(Val);}
  void PutX(const TSizeTy& X, const TVal& Val){
    for (TSizeTy Y=0; Y<TSizeTy(YDim); Y++){At(X, Y)=Val;}}
  void PutY(const TSizeTy& Y, const TVal& Val){
    for (TSizeTy X=0; X<TSizeTy(XDim); X++){At(X, Y)=Val;}}
  TVal GetXY(const TSizeTy& X, const TSizeTy& Y) const {
    Assert((0<=X)&&(X<TSizeTy(XDim))&&(0<=Y)&&(Y<TSizeTy(YDim)));
    return ValV[X*YDim+Y];}
  void GetRow(const TSizeTy& RowN, TVec<TVal, TSizeTy>& Vec) const;
  void GetCol(const TSizeTy& ColN, TVec<TVal, TSizeTy>& Vec) const;

  void SwapX(const TSizeTy& X1, const TSizeTy& X2);
  void SwapY(const TSizeTy& Y1, const TSizeTy& Y2);
  void Swap(TVVec<TVal, TSizeTy>& Vec);

  void ShuffleX(TRnd& Rnd);
  void ShuffleY(TRnd& Rnd);
  void GetMxValXY(TSizeTy& X, TSizeTy& Y) const;

  void CopyFrom(const TVVec<TVal, TSizeTy>& VVec);
  void AddXDim();
  void AddYDim();
  void DelX(const TSizeTy& X);
  void DelY(const TSizeTy& Y);
};

template <class TVal, class TSizeTy>
void TVVec<TVal, TSizeTy>::SwapX(const TSizeTy& X1, const TSizeTy& X2){
  for (TSizeTy Y=0; Y<TSizeTy(YDim); Y++){
    TVal Val=At(X1, Y); At(X1, Y)=At(X2, Y); At(X2, Y)=Val;}
}

template <class TVal, class TSizeTy>
void TVVec<TVal, TSizeTy>::SwapY(const TSizeTy& Y1, const TSizeTy& Y2){
  for (TSizeTy X=0; X<TSizeTy(XDim); X++){
    TVal Val=At(X, Y1); At(X, Y1)=At(X, Y2); At(X, Y2)=Val;}
}

template <class TVal, class TSizeTy>
void TVVec<TVal, TSizeTy>::Swap(TVVec<TVal, TSizeTy>& Vec){  //J:
  if (this!=&Vec){
    ::Swap(XDim, Vec.XDim);
    ::Swap(YDim, Vec.YDim);
    ValV.Swap(Vec.ValV);
  }
}

template <class TVal, class TSizeTy>
void TVVec<TVal, TSizeTy>::ShuffleX(TRnd& Rnd){
  for (TSizeTy X=0; X<XDim-1; X++){SwapX(X, X+Rnd.GetUniDevInt(XDim-X));}
}

template <class TVal, class TSizeTy>
void TVVec<TVal, TSizeTy>::ShuffleY(TRnd& Rnd){
  for (TSizeTy Y=0; Y<YDim-1; Y++){SwapY(Y, Y+Rnd.GetUniDevInt(YDim-Y));}
}

template <class TVal, class TSizeTy>
void TVVec<TVal, TSizeTy>::GetMxValXY(TSizeTy& X, TSizeTy& Y) const {
  TSizeTy MxValN=ValV.GetMxValN();
  Y=MxValN%YDim;
  X=MxValN/YDim;
}

template <class TVal, class TSizeTy>
void TVVec<TVal, TSizeTy>::CopyFrom(const TVVec<TVal, TSizeTy>& VVec){
  TSizeTy CopyXDim = (GetXDim() < VVec.GetXDim()) ? GetXDim() : VVec.GetXDim();
  TSizeTy CopyYDim = (GetYDim() < VVec.GetYDim()) ? GetYDim() : VVec.GetYDim();
  for (TSizeTy X=0; X<CopyXDim; X++){
    for (TSizeTy Y=0; Y<CopyYDim; Y++){
      At(X, Y)=VVec.At(X, Y);
    }
  }
}

template <class TVal, class TSizeTy>
void TVVec<TVal, TSizeTy>::AddXDim(){
  TVVec<TVal, TSizeTy> NewVVec(XDim+1, YDim);
  NewVVec.CopyFrom(*this);
  *this=NewVVec;
}

template <class TVal, class TSizeTy>
void TVVec<TVal, TSizeTy>::AddYDim(){
  TVVec<TVal, TSizeTy> NewVVec(XDim, YDim+1);
  NewVVec.CopyFrom(*this);
  *this=NewVVec;
}

template <class TVal, class TSizeTy>
void TVVec<TVal, TSizeTy>::DelX(const TSizeTy& X){
  TVVec<TVal, TSizeTy> NewVVec(XDim-1, YDim);
  for (TSizeTy Y=0; Y<YDim; Y++){
    for (TSizeTy LX=0; LX<X; LX++){
      NewVVec.At(LX, Y)=At(LX, Y);}
    for (TSizeTy RX=X+1; RX<XDim; RX++){
      NewVVec.At(RX-1, Y)=At(RX, Y);}
  }
  *this=NewVVec;
}

template <class TVal, class TSizeTy>
void TVVec<TVal, TSizeTy>::DelY(const TSizeTy& Y){
  TVVec<TVal, TSizeTy> NewVVec(XDim, YDim-1);
  for (TSizeTy X=0; X<XDim; X++){
    for (TSizeTy LY=0; LY<Y; LY++){
      NewVVec.At(X, LY)=At(X, LY);}
    for (TSizeTy RY=Y+1; RY<YDim; RY++){
      NewVVec.At(X, RY-1)=At(X, RY);}
  }
  *this=NewVVec;
}

template <class TVal, class TSizeTy >
void TVVec<TVal, TSizeTy>::GetRow(const TSizeTy& RowN, TVec<TVal, TSizeTy>& Vec) const {
  Vec.Gen(GetCols(), 0);
  for (TSizeTy col = 0; col < GetCols(); col++) {
    Vec.Add(At(RowN, col));
  }
}

template <class TVal, class TSizeTy>
void TVVec<TVal, TSizeTy>::GetCol(const TSizeTy& ColN, TVec<TVal, TSizeTy>& Vec) const {
  Vec.Gen(GetRows(), 0);
  for (TSizeTy row = 0; row < GetRows(); row++) {
    Vec.Add(At(row, ColN));
  }
}

/////////////////////////////////////////////////
// Common-2D-Vector-Types
typedef TVVec<TBool> TBoolVV;
typedef TVVec<TCh> TChVV;
typedef TVVec<TInt> TIntVV;
typedef TVVec<TSFlt> TSFltVV;
typedef TVVec<TFlt> TFltVV;
typedef TVVec<TStr> TStrVV;
typedef TVVec<TIntPr> TIntPrVV;

/////////////////////////////////////////////////
// 3D-Vector
template <class TVal, class TSizeTy = int>
class TVVVec{
private:
  TInt64 XDim, YDim, ZDim;
  TVec<TVal, TSizeTy> ValV;
public:
  TVVVec(): XDim(), YDim(), ZDim(), ValV(){}
  TVVVec(const TVVVec& Vec):
    XDim(Vec.XDim), YDim(Vec.YDim), ZDim(Vec.ZDim), ValV(Vec.ValV){}
  TVVVec(const TSizeTy& _XDim, const TSizeTy& _YDim, const TSizeTy& _ZDim):
    XDim(), YDim(), ZDim(), ValV(){Gen(_XDim, _YDim, _ZDim);}
  explicit TVVVec(TSIn& SIn):
    XDim(SIn), YDim(SIn), ZDim(SIn), ValV(SIn){}
  void Save(TSOut& SOut) const {
    XDim.Save(SOut); YDim.Save(SOut); ZDim.Save(SOut); ValV.Save(SOut);}

  TVVVec<TVal, TSizeTy>& operator=(const TVVVec<TVal, TSizeTy>& Vec){
    XDim=Vec.XDim; YDim=Vec.YDim; ZDim=Vec.ZDim; ValV=Vec.ValV;
    return *this;
  }
  bool operator==(const TVVVec& Vec) const {
    return (XDim==Vec.XDim)&&(YDim==Vec.YDim)&&(ZDim==Vec.ZDim)&&
     (ValV==Vec.ValV);}

  bool Empty() const {return ValV.Len()==0;}
  void Clr(){XDim=0; YDim=0; ZDim=0; ValV.Clr();}
  void Gen(const TSizeTy& _XDim, const TSizeTy& _YDim, const TSizeTy& _ZDim){
    Assert((_XDim>=0)&&(_YDim>=0)&&(_ZDim>=0));
    XDim=_XDim; YDim=_YDim; ZDim=_ZDim; ValV.Gen(XDim*YDim*ZDim);}
  TVal& At(const TSizeTy& X, const TSizeTy& Y, const TSizeTy& Z){
    Assert((0<=X)&&(X<TSizeTy(XDim))&&(0<=Y)&&(Y<TSizeTy(YDim))&&(0<=Z)&&(Z<TSizeTy(ZDim)));
    return ValV[X*YDim*ZDim+Y*ZDim+Z];}
  const TVal& At(const TSizeTy& X, const TSizeTy& Y, const TSizeTy& Z) const {
    Assert((0<=X)&&(X<TSizeTy(XDim))&&(0<=Y)&&(Y<TSizeTy(YDim))&&(0<=Z)&&(Z<TSizeTy(ZDim)));
    return ValV[X*YDim*ZDim+Y*ZDim+Z];}
  TVal& operator()(const TSizeTy& X, const TSizeTy& Y, const TSizeTy& Z){
    return At(X, Y, Z);}
  const TVal& operator()(const TSizeTy& X, const TSizeTy& Y, const TSizeTy& Z) const {
    return At(X, Y, Z);}
  TSizeTy GetXDim() const {return XDim;}
  TSizeTy GetYDim() const {return YDim;}
  TSizeTy GetZDim() const {return ZDim;}
};

/////////////////////////////////////////////////
// Common-3D-Vector-Types
typedef TVVVec<TInt> TIntVVV;
typedef TVVVec<TFlt> TFltVVV;

/////////////////////////////////////////////////
// Tree
template <class TVal>
class TTree{
private:
  TVec<TTriple<TInt, TIntV, TVal> > NodeV; // (ParentNodeId, ChildNodeIdV, NodeVal)
public:
  TTree(): NodeV(){}
  TTree(const TTree& Tree): NodeV(Tree.NodeV){}
  explicit TTree(TSIn& SIn): NodeV(SIn){}
  void Save(TSOut& SOut) const {NodeV.Save(SOut);}
  void LoadXml(const PXmlTok& XmlTok, const TStr& Nm="");
  void SaveXml(TSOut& SOut, const TStr& Nm) const;

  TTree& operator=(const TTree& Tree){if (this!=&Tree){NodeV=Tree.NodeV;} return *this;}
  bool operator==(const TTree& Tree) const {return NodeV==Tree.NodeV;}
  bool operator<(const TTree& Tree) const {return false;}

  int GetPrimHashCd() const {return NodeV.GetPrimHashCd();}
  int GetSecHashCd() const {return NodeV.GetSecHashCd();}

  int GetMemUsed() const {return NodeV.GetMemUsed();}

  void Clr(){NodeV.Clr();}

  int AddNode(const int& ParentNodeId, const TVal& NodeVal=TVal()){
    IAssert(((ParentNodeId==-1)&&(NodeV.Len()==0))||(NodeV.Len()>0));
    if (ParentNodeId!=-1){NodeV[ParentNodeId].Val2.Add(NodeV.Len());}
    return NodeV.Add(TTriple<TInt, TIntV, TVal>(ParentNodeId, TIntV(), NodeVal));}
  int AddRoot(const TVal& NodeVal=TVal()){
    return AddNode(-1, NodeVal);}

  int GetNodes() const {return NodeV.Len();}
  void GetNodeIdV(TIntV& NodeIdV, const int& NodeId=0);
  int GetParentNodeId(const int& NodeId) const {return NodeV[NodeId].Val1;}
  int GetChildren(const int& NodeId) const {return NodeV[NodeId].Val2.Len();}
  int GetChildNodeId(const int& NodeId, const int& ChildN) const {return NodeV[NodeId].Val2[ChildN];}
  TVal& GetNodeVal(const int& NodeId){return NodeV[NodeId].Val3;}

  void GenRandomTree(const int& Nodes, TRnd& Rnd);

  void DelNode(const int& NodeId);
  void CopyTree(const int& SrcNodeId, TTree& DstTree, const int& DstParentNodeId=-1);

  void WrTree(const int& NodeId=0, const int& Lev=0);
};

template <class TVal>
void TTree<TVal>::GetNodeIdV(TIntV& NodeIdV, const int& NodeId){
  if (NodeId==0){NodeIdV.Clr(); if (GetNodes()==0){return;}}
  else if (GetParentNodeId(NodeId)==-1){return;}
  NodeIdV.Add(NodeId);
  for (int ChildN=0; ChildN<GetChildren(NodeId); ChildN++){
    int ChildNodeId=GetChildNodeId(NodeId, ChildN);
    if (ChildNodeId!=-1){
      GetNodeIdV(NodeIdV, ChildNodeId);
    }
  }
}

template <class TVal>
void TTree<TVal>::GenRandomTree(const int& Nodes, TRnd& Rnd){
  Clr();
  if (Nodes>0){
    AddRoot(TVal());
    for (int NodeN=1; NodeN<Nodes; NodeN++){
      int ParentNodeId=Rnd.GetUniDevInt(0, GetNodes()-1);
      AddNode(ParentNodeId, TVal());
    }
  }
}

template <class TVal>
void TTree<TVal>::DelNode(const int& NodeId){
  if (NodeId==0){
    Clr();
  } else {
    TIntV& ChildNodeIdV=NodeV[GetParentNodeId(NodeId)].Val2;
    int ChildNodeIdN=ChildNodeIdV.SearchForw(NodeId);
    ChildNodeIdV[ChildNodeIdN]=-1;
  }
}

template <class TVal>
void TTree<TVal>::CopyTree(const int& SrcNodeId, TTree& DstTree, const int& DstParentNodeId){
  int DstNodeId=DstTree.AddNode(DstParentNodeId, GetNodeVal(SrcNodeId));
  for (int ChildN=0; ChildN<GetChildren(SrcNodeId); ChildN++){
    int ChildNodeId=GetChildNodeId(SrcNodeId, ChildN);
    if (ChildNodeId!=-1){
      CopyTree(ChildNodeId, DstTree, DstNodeId);
    }
  }
}

template <class TVal>
void TTree<TVal>::WrTree(const int& NodeId, const int& Lev){
  for (int LevN=0; LevN<Lev; LevN++){printf("| ");}
  printf("%d (%d)\n", NodeId, GetChildren(NodeId));
  for (int ChildN=0; ChildN<GetChildren(NodeId); ChildN++){
    int ChildNodeId=GetChildNodeId(NodeId, ChildN);
    if (ChildNodeId!=-1){
      WrTree(ChildNodeId, Lev+1);
    }
  }
}

/////////////////////////////////////////////////
// Common-Tree-Types
typedef TTree<TInt> TIntTree;
typedef TTree<TFlt> TFltTree;
typedef TTree<TStr> TStrTree;
typedef TTree<TStrIntPr> TStrIntPrTree;
typedef TTree<TStrIntStrVTr> TStrIntStrVTrTree;

/////////////////////////////////////////////////
// Stack
template <class TVal>
class TSStack{
private:
  TVec<TVal> ValV;
public:
  TSStack(): ValV(){}
  TSStack(const int& MxVals): ValV(MxVals, 0){}
  TSStack(const TSStack& Stack): ValV(Stack.ValV){}
  explicit TSStack(TSIn& SIn): ValV(SIn){}
  void Save(TSOut& SOut) const {ValV.Save(SOut);}

  TSStack& operator=(const TSStack& Stack){
    if (this!=&Stack){ValV=Stack.ValV;} return *this;}
  bool operator==(const TSStack& Stack) const {return this==&Stack;}
  const TVal& operator[](const int& ValN) const {return ValV[ValV.Len()-ValN-1];}
  TVal& operator[](const int& ValN) {return ValV[ValV.Len()-ValN-1];}

  bool Empty(){return ValV.Len()==0;}
  void Clr(const bool& DoDel=false) {ValV.Clr(DoDel);}
  bool IsIn(const TVal& Val) const {return ValV.IsIn(Val);}
  int Len(){return ValV.Len();}
  TVal& Top(){Assert(0<ValV.Len()); return ValV.Last();}
  const TVal& Top() const {Assert(0<ValV.Len()); return ValV.Last();}
  void Push(){ValV.Add();}
  void Push(const TVal& Val){ValV.Add(Val);}
  void Pop(){Assert(0<ValV.Len()); ValV.DelLast();}
};

/////////////////////////////////////////////////
// Common-Stack-Types
typedef TSStack<TInt> TIntS;
typedef TSStack<TBoolChPr> TBoolChS;

/////////////////////////////////////////////////
// Queue
template <class TVal>
class TQQueue{
private:
  TInt MxLast, MxLen;
  TInt First, Last;
  TVec<TVal> ValV;
public:
  TQQueue(const int& _MxLast=64, const int& _MxLen=-1):
    MxLast(_MxLast), MxLen(_MxLen), First(0), Last(0), ValV(){
    Assert(int(MxLast)>0); Assert((MxLen==-1)||(int(MxLen)>0));}
  TQQueue(const TQQueue& Queue):
    MxLast(Queue.MxLast), MxLen(Queue.MxLen),
    First(Queue.First), Last(Queue.Last), ValV(Queue.ValV){}
  explicit TQQueue(TSIn& SIn):
    MxLast(SIn), MxLen(SIn), First(SIn), Last(SIn), ValV(SIn){}
  void Save(TSOut& SOut) const {
    MxLast.Save(SOut); MxLen.Save(SOut);
    First.Save(SOut); Last.Save(SOut); ValV.Save(SOut);}

  TQQueue& operator=(const TQQueue& Queue){
    if (this!=&Queue){MxLast=Queue.MxLast; MxLen=Queue.MxLen;
      First=Queue.First; Last=Queue.Last; ValV=Queue.ValV;}
    return *this;}
  const TVal& operator[](const int& ValN) const {Assert((0<=ValN)&&(ValN<Len()));
    return ValV[Last+ValN];}

  void Clr(const bool& DoDel=true){ValV.Clr(DoDel); First=Last=0;}
  void Gen(const int& _MxLast=64, const int& _MxLen=-1){
    MxLast=_MxLast; MxLen=_MxLen; First=0; Last=0; ValV.Clr();}
  void GetSubValV(const int& _BValN, const int& _EValN, TVec<TVal>& SubValV) const {
    int BValN=TInt::GetMx(0, _BValN);
    int EValN=TInt::GetMn(Len()-1, _EValN);
    SubValV.Gen(EValN-BValN+1);
    for (int ValN=BValN; ValN<=EValN; ValN++){
      SubValV[ValN-BValN]=ValV[Last+ValN];}
  }

  bool Empty() const {return First==Last;}
  int Len() const {return First-Last;}
  const TVal& Top() const {
    Assert(First!=Last); return ValV[Last];}
  void Pop(){
    IAssert(First!=Last); Last++;
    if (First==Last){ValV.Clr(); First=Last=0;}}
  void Push(const TVal& Val){
    if (Last>MxLast){ValV.Del(0, Last-1); First-=Last; Last=0;}
    if ((MxLen!=-1)&&(MxLen==Len())){Pop();}
    First++; ValV.Add(Val);}

  void Shuffle(TRnd& Rnd){
    TVec<TVal> ValV(Len(), 0); while (!Empty()){ValV.Add(Top()); Pop();}
    ValV.Shuffle(Rnd); Clr();
    for (int ValN=0; ValN<ValV.Len(); ValN++){Push(ValV[ValN]);}}
};

/////////////////////////////////////////////////
// Common-Queue-Types
typedef TQQueue<TInt> TIntQ;
typedef TQQueue<TFlt> TFltQ;
typedef TQQueue<TStr> TStrQ;
typedef TQQueue<TIntPr> TIntPrQ;
typedef TQQueue<TIntStrPr> TIntStrPrQ;
typedef TQQueue<TFltV> TFltVQ;
typedef TQQueue<TAscFltV> TAscFltVQ;
typedef TVec<TQQueue<TInt> > TIntQV;

/////////////////////////////////////////////////
// List-Node
template <class TVal>
class TLstNd{
public:
  TLstNd* PrevNd;
  TLstNd* NextNd;
  TVal Val;
public:
  TLstNd(): PrevNd(NULL), NextNd(NULL), Val(){}
  TLstNd(const TLstNd&);
  TLstNd(TLstNd* _PrevNd, TLstNd* _NextNd, const TVal& _Val):
    PrevNd(_PrevNd), NextNd(_NextNd), Val(_Val){}

  TLstNd& operator=(const TLstNd&);

  bool IsPrev() const {return (PrevNd != NULL); }
  bool IsNext() const {return (NextNd != NULL); }
  TLstNd* Prev() const {Assert(this!=NULL); return PrevNd;}
  TLstNd* Next() const {Assert(this!=NULL); return NextNd;}
  TVal& GetVal(){Assert(this!=NULL); return Val;}
  const TVal& GetVal() const {Assert(this!=NULL); return Val;}
};

/////////////////////////////////////////////////
// List
template <class TVal>
class TLst{
public:
  typedef TLstNd<TVal>* PLstNd;
private:
  int Nds;
  PLstNd FirstNd;
  PLstNd LastNd;
public:
  TLst(): Nds(0), FirstNd(NULL), LastNd(NULL){}
  TLst(const TLst&);
  ~TLst(){Clr();}
  explicit TLst(TSIn& SIn);
  void Save(TSOut& SOut) const;

  TLst& operator=(const TLst&);

  void Clr(){
    PLstNd Nd=FirstNd;
    while (Nd!=NULL){PLstNd NextNd=Nd->NextNd; delete Nd; Nd=NextNd;}
    Nds=0; FirstNd=NULL; LastNd=NULL;}

  bool Empty() const {return Nds==0;}
  int Len() const {return Nds;}
  PLstNd First() const {return FirstNd;}
  PLstNd Last() const {return LastNd;}
  TVal& FirstVal() const {return FirstNd->GetVal();}
  TVal& LastVal() const {return LastNd->GetVal();}

  PLstNd AddFront(const TVal& Val);
  PLstNd AddBack(const TVal& Val);
  PLstNd AddFrontSorted(const TVal& Val, const bool& Asc=true);
  PLstNd AddBackSorted(const TVal& Val, const bool& Asc=true);
  void PutFront(const PLstNd& Nd);
  void PutBack(const PLstNd& Nd);
  PLstNd Ins(const PLstNd& Nd, const TVal& Val);
  void Del(const TVal& Val);
  void Del(const PLstNd& Nd);
  void DelFirst() { PLstNd DelNd = FirstNd; Del(DelNd); }
  void DelLast() { PLstNd DelNd = LastNd; Del(DelNd); }

  PLstNd SearchForw(const TVal& Val);
  PLstNd SearchBack(const TVal& Val);

  friend class TLstNd<TVal>;
};

template <class TVal>
TLst<TVal>::TLst(TSIn& SIn):
  Nds(0), FirstNd(NULL), LastNd(NULL){
  int CheckNds=0; SIn.Load(CheckNds);
  for (int NdN=0; NdN<CheckNds; NdN++){AddBack(TVal(SIn));}
  Assert(Nds==CheckNds);
}

template <class TVal>
void TLst<TVal>::Save(TSOut& SOut) const {
  SOut.Save(Nds);
  PLstNd Nd=FirstNd; int CheckNds=0;
  while (Nd!=NULL){
    Nd->Val.Save(SOut); Nd=Nd->NextNd; CheckNds++;}
  IAssert(Nds==CheckNds);
}

template <class TVal>
TLstNd<TVal>* TLst<TVal>::AddFront(const TVal& Val){
  PLstNd Nd=new TLstNd<TVal>(NULL, FirstNd, Val);
  if (FirstNd!=NULL){FirstNd->PrevNd=Nd; FirstNd=Nd;}
  else {FirstNd=Nd; LastNd=Nd;}
  Nds++; return Nd;
}

template <class TVal>
TLstNd<TVal>* TLst<TVal>::AddBack(const TVal& Val){
  PLstNd Nd=new TLstNd<TVal>(LastNd, NULL, Val);
  if (LastNd!=NULL){LastNd->NextNd=Nd; LastNd=Nd;}
  else {FirstNd=Nd; LastNd=Nd;}
  Nds++; return Nd;
}

template <class TVal>
TLstNd<TVal>* TLst<TVal>::AddFrontSorted(const TVal& Val, const bool& Asc){
  PLstNd Nd=First();
  if (Nd==NULL){
    return Ins(Nd, Val);
  } else {
    while ((Nd!=NULL)&&((Asc&&(Val>Nd()))||(!Asc&&(Val<Nd())))){
      Nd=Nd->Next();}
    if (Nd==NULL){return Ins(Nd->Last(), Val);}
    else {return Ins(Nd->Prev(), Val);}
  }
}

template <class TVal>
TLstNd<TVal>* TLst<TVal>::AddBackSorted(const TVal& Val, const bool& Asc){
  PLstNd Nd=Last();
  while ((Nd!=NULL)&&((Asc&&(Val<Nd->Val))||(!Asc&&(Val>Nd->Val)))){
    Nd=Nd->Prev();}
  return Ins(Nd, Val);
}

template <class TVal>
void TLst<TVal>::PutFront(const PLstNd& Nd){
  Assert(Nd!=NULL);
  // unchain
  if (Nd->PrevNd==NULL){FirstNd=Nd->NextNd;}
  else {Nd->PrevNd->NextNd=Nd->NextNd;}
  if (Nd->NextNd==NULL){LastNd=Nd->PrevNd;}
  else {Nd->NextNd->PrevNd=Nd->PrevNd;}
  // add to front
  Nd->PrevNd=NULL; Nd->NextNd=FirstNd;
  if (FirstNd!=NULL){FirstNd->PrevNd=Nd; FirstNd=Nd;}
  else {FirstNd=Nd; LastNd=Nd;}
}

template <class TVal>
void TLst<TVal>::PutBack(const PLstNd& Nd){
  Assert(Nd!=NULL);
  // unchain
  if (Nd->PrevNd==NULL){FirstNd=Nd->NextNd;}
  else {Nd->PrevNd->NextNd=Nd->NextNd;}
  if (Nd->NextNd==NULL){LastNd=Nd->PrevNd;}
  else {Nd->NextNd->PrevNd=Nd->PrevNd;}
  // add to back
  Nd->PrevNd=LastNd; Nd->NextNd=NULL;
  if (LastNd!=NULL){LastNd->NextNd=Nd; LastNd=Nd;}
  else {FirstNd=Nd; LastNd=Nd;}
}

template <class TVal>
TLstNd<TVal>* TLst<TVal>::Ins(const PLstNd& Nd, const TVal& Val){
  if (Nd==NULL){return AddFront(Val);}
  else if (Nd->NextNd==NULL){return AddBack(Val);}
  else {
    PLstNd NewNd=new TLstNd<TVal>(Nd, Nd->NextNd, Val);
    Nd->NextNd=NewNd; NewNd->NextNd->PrevNd=Nd;
    Nds++; return Nd;
  }
}

template <class TVal>
void TLst<TVal>::Del(const TVal& Val){
  PLstNd Nd=SearchForw(Val);
  if (Nd!=NULL){Del(Nd);}
}

template <class TVal>
void TLst<TVal>::Del(const PLstNd& Nd){
  Assert(Nd!=NULL);
  if (Nd->PrevNd==NULL){FirstNd=Nd->NextNd;}
  else {Nd->PrevNd->NextNd=Nd->NextNd;}
  if (Nd->NextNd==NULL){LastNd=Nd->PrevNd;}
  else {Nd->NextNd->PrevNd=Nd->PrevNd;}
  Nds--; delete Nd;
}

template <class TVal>
TLstNd<TVal>* TLst<TVal>::SearchForw(const TVal& Val){
  PLstNd Nd=First();
  while (Nd!=NULL){
    if (Nd->GetVal()==Val){return Nd;}
    Nd=Nd->Next();
  }
  return NULL;
}

template <class TVal>
TLstNd<TVal>* TLst<TVal>::SearchBack(const TVal& Val){
  PLstNd Nd=Last();
  while (Nd!=NULL){
    if (Nd->GetVal()==Val){return Nd;}
    Nd=Nd->Prev();
  }
  return NULL;
}

/////////////////////////////////////////////////
// Common-List-Types
typedef TLst<TInt> TIntL;
typedef TLstNd<TInt>* PIntLN;
typedef TLst<TIntKd> TIntKdL;
typedef TLstNd<TIntKd>* PIntKdLN;
typedef TLst<TFlt> TFltL;
typedef TLstNd<TFlt>* PFltLN;
typedef TLst<TFltIntKd> TFltIntKdL;
typedef TLstNd<TFltIntKd>* PFltIntKdLN;
typedef TLst<TAscFltIntKd> TAscFltIntKdL;
typedef TLstNd<TAscFltIntKd>* PAscFltIntKdLN;
typedef TLst<TStr> TStrL;
typedef TLstNd<TStr>* PStrLN;

/////////////////////////////////////////////////
// Record-File
template <class THd, class TRec>
class TFRec{
private:
  PFRnd FRnd;
public:
  TFRec(const TStr& FNm, const TFAccess& FAccess, const bool& CreateIfNo):
    FRnd(PFRnd(new TFRnd(FNm, FAccess, CreateIfNo, sizeof(THd), sizeof(TRec)))){}
  TFRec(const TFRec&);

  TFRec& operator=(const TFRec&);

  void SetRecN(const int& RecN){FRnd->SetRecN(RecN);}
  int GetRecN(){return FRnd->GetRecN();}
  int GetRecs(){return FRnd->GetRecs();}

  void GetHd(THd& Hd){FRnd->GetHd(&Hd);}
  void PutHd(const THd& Hd){FRnd->PutHd(&Hd);}
  void GetRec(TRec& Rec, const int& RecN=-1){FRnd->GetRec(&Rec, RecN);}
  void PutRec(const TRec& Rec, const int& RecN=-1){FRnd->PutRec(&Rec, RecN);}
};

/////////////////////////////////////////////////
// Function
template <class TFuncPt>
class TFunc{
private:
  TFuncPt FuncPt;
public:
  TFunc(): FuncPt(NULL){}
  TFunc(const TFunc& Func): FuncPt(Func.FuncPt){}
  TFunc(const TFuncPt& _FuncPt): FuncPt(_FuncPt){}
  TFunc(TSIn&){Fail;}
  void Save(TSOut&) const {Fail;}

  TFunc& operator=(const TFunc& Func){
    if (this!=&Func){FuncPt=Func.FuncPt;} return *this;}
  bool operator==(const TFunc& Func) const {
    return FuncPt==Func.FuncPt;}
  bool operator<(const TFunc&) const {
    Fail; return false;}
  TFuncPt operator()() const {return FuncPt;}
};
