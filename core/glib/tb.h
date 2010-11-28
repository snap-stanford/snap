/////////////////////////////////////////////////
// Forward
ClassHdTP(TTb, PTb);

/////////////////////////////////////////////////
// Table-Variable-Data-Type
typedef enum {
  tsvcUndef, tsvcIgnore, tsvcLabel, tsvcClass, tsvcAttr} TTbSufixVarCat;

ClassTPV(TTbVarType, PTbVarType, TTbVarTypeV)//{
private:
  TBool DefP;
  TB32Set AlwVTSet;
  TB32Set FixVTSet;
  TBool DscIntP, DscOrdP;
  TInt MnIntDsc, MxIntDsc;
  TStrV DscToNmV;
  TStrIntH NmToDscH;
  TFlt MnFlt, MxFlt;
public:
  TTbVarType():
    AlwVTSet(TB32Set().Fill()), FixVTSet(),
    DscIntP(false), DscOrdP(false),
    MnIntDsc(0), MxIntDsc(0),
    DscToNmV(), NmToDscH(10),
    MnFlt(TFlt::Mx), MxFlt(TFlt::Mn){}
  TTbVarType(TSIn& SIn):
    DefP(SIn),
    AlwVTSet(SIn), FixVTSet(SIn),
    DscIntP(SIn), DscOrdP(SIn),
    MnIntDsc(SIn), MxIntDsc(SIn),
    DscToNmV(SIn), NmToDscH(SIn),
    MnFlt(SIn), MxFlt(SIn){
    Assert(DefP);}
  static PTbVarType Load(TSIn& SIn){return new TTbVarType(SIn);}
  void Save(TSOut& SOut){
    Assert(DefP); DefP.Save(SOut),
    FixVTSet.Save(SOut); AlwVTSet.Save(SOut);
    DscIntP.Save(SOut); DscOrdP.Save(SOut);
    MnIntDsc.Save(SOut); MxIntDsc.Save(SOut);
    DscToNmV.Save(SOut); NmToDscH.Save(SOut);
    MnFlt.Save(SOut); MxFlt.Save(SOut);}

  void Def(){Assert(!DefP); DefP=true;}
  bool IsDef() const {return DefP;}

  TTbVarType& operator=(const TTbVarType& TbVarType){
    Assert(!DefP); DefP=TbVarType.DefP;
    FixVTSet=TbVarType.FixVTSet; AlwVTSet=TbVarType.AlwVTSet;
    DscIntP=TbVarType.DscIntP; DscOrdP=TbVarType.DscOrdP;
    MnIntDsc=TbVarType.MnIntDsc; MxIntDsc=TbVarType.MxIntDsc;
    DscToNmV=TbVarType.DscToNmV; NmToDscH=TbVarType.NmToDscH;
    MnFlt=TbVarType.MnFlt; MxFlt=TbVarType.MxFlt; return *this;}
  bool operator==(const TTbVarType& TbVarType) const {
    Assert(DefP);
    if (this==&TbVarType){return true;}
    return (DefP==TbVarType.DefP)&&
     (AlwVTSet==TbVarType.AlwVTSet)&&(FixVTSet==TbVarType.FixVTSet)&&
     (DscIntP==TbVarType.DscIntP)&&(DscOrdP==TbVarType.DscOrdP)&&
     (MnIntDsc==TbVarType.MnIntDsc)&&(MxIntDsc==TbVarType.MxIntDsc)&&
     (DscToNmV==TbVarType.DscToNmV)&&//(NmToDscH==TbVarType.NmToDscH)&&
     (MnFlt==TbVarType.MnFlt)&&(MxFlt==TbVarType.MxFlt);}

  TB32Set GetAlwVTSet() const {return AlwVTSet;}
  void SetAlwVTSet(const TB32Set& _AlwVTSet){
    Assert(!DefP); AlwVTSet=_AlwVTSet;}

  TB32Set GetFixVTSet() const {return FixVTSet;}
  void SetDscFix(const int& _MnIntDsc, const int& _MxIntDsc){
    Assert(!DefP && !FixVTSet.In(tvtDsc) && (_MnIntDsc<=_MxIntDsc));
    FixVTSet.Incl(tvtDsc);
    DscIntP=true; DscOrdP=true; MnIntDsc=_MnIntDsc; MxIntDsc=_MxIntDsc;}
  void SetDscFix(bool _DscOrd=false){
    Assert(!DefP && !FixVTSet.In(tvtDsc) && (DscToNmV.Len()==0));
    FixVTSet.Incl(tvtDsc);
    DscIntP=false; DscOrdP=_DscOrd; MnIntDsc=0; MxIntDsc=0;}
  int AddDscNm(const TStr& Str){
    Assert(!DefP && FixVTSet.In(tvtDsc) && !DscIntP);
    Assert(NmToDscH.GetKeyId(Str)==-1);
    return NmToDscH.AddDat(Str, TInt(DscToNmV.Add(Str)));}
  void SetFltFix(const double& _MnFlt, const double& _MxFlt){
    Assert(!DefP && !FixVTSet.In(tvtFlt) && (_MnFlt<=_MxFlt));
    FixVTSet.Incl(tvtFlt);
    MnFlt=_MnFlt; MxFlt=_MxFlt;}

  bool IsDsc() const {Assert(DefP);
    return AlwVTSet.In(tvtDsc) && (FixVTSet.In(tvtDsc) || DscToNmV.Len()>0);}
  bool IsDscInt() const {Assert(IsDsc()); return DscIntP;}
  bool IsDscSym() const {Assert(IsDsc()); return !DscIntP;}
  bool IsDscOrd() const {Assert(IsDsc()); return DscOrdP;}
  int GetDscs() const {Assert(IsDsc());
    if (DscIntP){return MxIntDsc-MnIntDsc+1;} else {return DscToNmV.Len();}}
  int GetMnIntDsc() const {Assert(IsDscInt()); return MnIntDsc;}
  int GetMxIntDsc() const {Assert(IsDscInt()); return MxIntDsc;}
  TStr GetDscStr(const int& Dsc) const {return DscToNmV[Dsc];}
  void GetDscStrV(TStrV& DscStrV) const {
    Assert(IsDscSym()); int Dscs=GetDscs(); DscStrV.Gen(Dscs, 0);
    for (int Dsc=0; Dsc<Dscs; Dsc++){DscStrV.Add(GetDscStr(Dsc));}}
  bool IsFlt() const {Assert(DefP);
    return AlwVTSet.In(tvtFlt) && (FixVTSet.In(tvtFlt) || (MnFlt<=MxFlt));}
  double GetFltRange() const {Assert(IsFlt()); return MxFlt-MnFlt;}
  double GetMnFlt() const {Assert(IsFlt()); return MnFlt;}
  double GetMxFlt() const {Assert(IsFlt()); return MxFlt;}
  TStr GetStr() const;

  bool IsValOk(const TTbVal& Val);
  TTbVal GetDscVal(const TStr& Str);
  TTbVal GetDscVal(const int& Dsc);
  TTbVal GetIntDscVal(const int& Int);
  TTbVal GetFltVal(const double& Flt);
  TTbVal GetVal(const TTbVal& Val);
  double GetNrmFlt(const TTbVal& Val);
  TStr GetValStr(const TTbVal& Val);

  // predefined data types
  static PTbVarType GetDscBoolVarType();
  static PTbVarType GetDscIntVarType(
   const int& MnIntDsc, const int& MxIntDsc);
  static PTbVarType GetDscNmVVarType(
   const TStrV& NmV, const bool& DscOrdP=false);
  static PTbVarType GetFltVarType(
   const double& MnFlt=TFlt::Mn, const double& MxFlt=TFlt::Mx);

  // name-suffix-category functions
  static TTbSufixVarCat GetSufixVarCat(const TStr& VarNm);
  static bool IsSuffixVarNm(const PTb& Tb);
};

/////////////////////////////////////////////////
// Table-Variable
ClassTPV(TTbVar, PTbVar, TTbVarV)//{
private:
  UndefCopy(TTbVar);
public:
  TTbVar(){}
  virtual ~TTbVar(){}
  TTbVar(TSIn&){}
  static PTbVar Load(TSIn&){Fail; return NULL;}
  virtual void Save(TSOut&){}

  TTbVar& operator=(const TTbVar&){return *this;}
  bool operator==(const TTbVar& TbVar) const {
    return (GetNm()==TbVar.GetNm())&&(GetVarType()==TbVar.GetVarType())&&
     (GetActVTSet()==TbVar.GetActVTSet());}

  virtual TStr GetNm() const=0;
  virtual PTbVarType GetVarType() const=0;
  virtual TB32Set GetActVTSet() const=0;
  virtual void DefVarType()=0;

  virtual TTbVal GetDscVal(const TStr& Str)=0;
  virtual TTbVal GetDscVal(const int& Dsc)=0;
  virtual TTbVal GetFltVal(const double& Flt)=0;
  virtual TTbVal GetVal(const TTbVal& Val)=0;
};

/////////////////////////////////////////////////
// Table-Tuple
ClassTPV(TTbTup, PTbTup, TTbTupV)//{
private:
  UndefCopy(TTbTup);
public:
  TTbTup(){}
  TTbTup(TSIn&){}
  virtual ~TTbTup(){}
  static PTbTup Load(TSIn&){Fail; return NULL;}
  virtual void Save(TSOut&){}

  TTbTup& operator=(const TTbTup&){return *this;}

  virtual void PutNm(const TStr& _Nm)=0;
  virtual TStr GetNm()=0;

  virtual void PutVal(int VarN, const TTbVal& Val)=0;
  virtual TTbVal GetVal(int VarN)=0;
};

/////////////////////////////////////////////////
// Table
ClassTPV(TTb, PTb, TTbV)//{
private:
  TStr Nm;
  UndefCopyAssign(TTb);
public:
  TTb(): Nm("Untitled-Table"){}
  virtual ~TTb(){}
  TTb(TSIn& SIn): Nm(SIn){}
  static PTb Load(TSIn& SIn);
  virtual void Save(TSOut& SOut){GetTypeNm(*this).Save(SOut); Nm.Save(SOut);}

  void PutNm(const TStr& _Nm){Nm=_Nm;}
  TStr GetNm(){return Nm;}

  void AddTb(const PTb& Tb);
  PTb GetSubTb(const TIntV& TupNV, const TIntV& VarNV, const PTb& Tb);

  virtual int AddVar(const PTbVar& TbVar)=0;
  virtual void DefVarTypes()=0;
  virtual int GetVars()=0;
  virtual PTbVar GetVar(const int& VarN)=0;
  virtual int GetVarN(const TStr& Nm)=0;

  virtual int AddTup(const TStr& Nm)=0;
  int AddTup(){return AddTup(TStr());}
  virtual int GetTups()=0;
  virtual TStr GetTupNm(const int& TupN)=0;
  virtual int GetTupN(const TStr& Nm)=0;

  virtual void PutDsc(const int& TupN, const int& VarN, const TStr& Str)=0;
  virtual void PutDsc(const int& TupN, const int& VarN, const int& Dsc)=0;
  virtual void PutFlt(const int& TupN, const int& VarN, const double& Flt)=0;
  virtual void PutVal(const int& TupN, const int& VarN, const TTbVal& Val)=0;
  virtual TTbVal GetVal(const int& TupN, const int& VarN)=0;
  virtual TStr GetValStr(const int& TupN, const int& VarN)=0;

  static PTb LoadFile(const TStr& FNm, const TStr& FType, const TStr& MemRep);
  static PTb LoadTxtSpc(const TStr& FNm, const PTb& Tb);
  static PTb LoadTxtTab(const TStr& FNm, const PTb& Tb);
  static PTb LoadTxtCsv(const TStr& FNm, const PTb& Tb);
  void SaveTxt(const TStr& FNm, const bool& SaveTupNm=true);
  void SaveAssis(const TStr& FNm);
};

/////////////////////////////////////////////////
// Generic-Table-Variable
class TGTbVar: public TTbVar{
private:
  TStr Nm;
  PTbVarType VarType;
  TB32Set ActVTSet;
public:
  TGTbVar(): TTbVar(), Nm(), VarType(), ActVTSet(){}
  TGTbVar(const TStr& _Nm, const PTbVarType& _VarType):
    TTbVar(), Nm(_Nm), VarType(_VarType), ActVTSet(){}
  TGTbVar(TSIn& SIn):
    TTbVar(SIn), Nm(SIn), VarType(SIn), ActVTSet(SIn){}
  void Save(TSOut& SOut){
    TTbVar::Save(SOut); Nm.Save(SOut); VarType.Save(SOut); ActVTSet.Save(SOut);}

  TGTbVar& operator=(const TGTbVar& GTbVar){
    TTbVar::operator=(GTbVar);
    Nm=GTbVar.Nm; ActVTSet=GTbVar.ActVTSet; VarType=GTbVar.VarType;
    return *this;}

  TStr GetNm() const {return Nm;}
  PTbVarType GetVarType() const {return VarType;}
  TB32Set GetActVTSet() const {return ActVTSet;}
  void DefVarType(){VarType->Def();}

  TTbVal GetDscVal(const TStr& Str){
    ActVTSet.Incl(tvtDsc); return VarType->GetDscVal(Str);}
  TTbVal GetDscVal(const int& Dsc){
    ActVTSet.Incl(tvtDsc); return VarType->GetDscVal(Dsc);}
  TTbVal GetFltVal(const double& Flt){
    ActVTSet.Incl(tvtFlt); return VarType->GetFltVal(Flt);}
  TTbVal GetVal(const TTbVal& Val){
    ActVTSet.Incl(Val.GetValTag()); return VarType->GetVal(Val);}
};

/////////////////////////////////////////////////
// Generic-Table-Tuple
class TGTbTup: public TTbTup{
  TStr Nm;
  TTbValV ValV;
public:
  TGTbTup(): TTbTup(), Nm(), ValV(){}
  TGTbTup(const TStr& _Nm, const int& Vars): TTbTup(), Nm(_Nm), ValV(Vars){}
  TGTbTup(TSIn& SIn): TTbTup(SIn), Nm(SIn), ValV(SIn){}
  virtual ~TGTbTup(){}
  void Save(TSOut& SOut){TTbTup::Save(SOut); Nm.Save(SOut); ValV.Save(SOut);}

  TGTbTup& operator=(const TGTbTup& GTbTup){
    TTbTup::operator=(GTbTup); Nm=GTbTup.Nm; ValV=GTbTup.ValV; return *this;}

  void PutNm(const TStr& _Nm){Nm=_Nm;}
  TStr GetNm(){return Nm;}

  void PutVal(int VarN, const TTbVal& Val){ValV[VarN]=Val;}
  TTbVal GetVal(int VarN){return ValV[VarN];}
};

/////////////////////////////////////////////////
// Generic-Table
class TGTb: public TTb{
private:
  TTbVarV VarV;
  TStrIntH VarNmToNH;
  TTbTupV TupV;
  TStrIntH TupNmToNH;
public:
  TGTb(const int& MxVars=0, const int& MxTups=0):
    TTb(), VarV(MxVars, 0), VarNmToNH(MxVars>0?MxVars:100),
    TupV(MxTups, 0), TupNmToNH(MxTups>0?MxTups:100){}
  TGTb(TSIn& SIn):
    TTb(SIn), VarV(SIn), VarNmToNH(SIn),
    TupV(SIn), TupNmToNH(SIn){SIn.LoadCs();}
  void Save(TSOut& SOut){
    TTb::Save(SOut); VarV.Save(SOut); VarNmToNH.Save(SOut);
    TupV.Save(SOut); SOut.SaveCs();}

  int AddVar(const PTbVar& TbVar);
  void DefVarTypes();
  int GetVars(){return VarV.Len();}
  PTbVar GetVar(const int& VarN){return VarV[VarN];}
  int GetVarN(const TStr& Nm){return VarNmToNH.GetDat(Nm);}

  int AddTup(const TStr& Nm);
  int GetTups(){return TupV.Len();}
  TStr GetTupNm(const int& TupN){return TupV[TupN]->GetNm();}
  int GetTupN(const TStr& Nm){return TupNmToNH.GetDat(Nm);}

  void PutDsc(const int& TupN, const int& VarN, const TStr& Str){
    TupV[TupN]->PutVal(VarN, VarV[VarN]->GetDscVal(Str));}
  void PutDsc(const int& TupN, const int& VarN, const int& Dsc){
    TupV[TupN]->PutVal(VarN, VarV[VarN]->GetDscVal(Dsc));}
  void PutFlt(const int& TupN, const int& VarN, const double& Flt){
    TupV[TupN]->PutVal(VarN, VarV[VarN]->GetFltVal(Flt));}
  void PutVal(const int& TupN, const int& VarN, const TTbVal& Val){
    TupV[TupN]->PutVal(VarN, VarV[VarN]->GetVal(Val));}
  TTbVal GetVal(const int& TupN, const int& VarN){
    return TupV[TupN]->GetVal(VarN);}
  TStr GetValStr(const int& TupN, const int& VarN);
};

/////////////////////////////////////////////////
// Binary-Table
class TBTb: public TTb{
private:
  TTbVarV VarV;
  TStrIntH VarNmToNH;
  TTbTupV TupV;
  TStrIntH TupNmToNH;
public:
  TBTb(const int& MxVars=0, const int& MxTups=0):
    TTb(), VarV(MxVars, 0), VarNmToNH(MxVars>0?MxVars:100),
    TupV(MxTups, 0), TupNmToNH(MxTups>0?MxTups:100){}
  TBTb(TSIn& SIn):
    TTb(SIn), VarV(SIn), VarNmToNH(SIn),
    TupV(SIn), TupNmToNH(SIn){SIn.LoadCs();}
  void Save(TSOut& SOut){
    TTb::Save(SOut); VarV.Save(SOut); VarNmToNH.Save(SOut);
    TupV.Save(SOut); SOut.SaveCs();}

  int AddVar(const PTbVar& TbVar);
  void DefVarTypes(){}
  int GetVars(){return VarNmToNH.Len();}
  PTbVar GetVar(const int& /*VarN*/){Fail; return NULL;}
  int GetVarN(const TStr& Nm){return VarNmToNH.GetKeyId(Nm);}

  int AddTup(const TStr& Nm);
  int GetTups(){return TupV.Len();}
  TStr GetTupNm(const int& TupN){return TupV[TupN]->GetNm();}
  int GetTupN(const TStr& Nm){return TupNmToNH.GetDat(Nm);}

  void PutDsc(const int& TupN, const int& VarN, const TStr& Str){
    TupV[TupN]->PutVal(VarN, VarV[VarN]->GetDscVal(Str));}
  void PutDsc(const int& TupN, const int& VarN, const int& Dsc){
    TupV[TupN]->PutVal(VarN, VarV[VarN]->GetDscVal(Dsc));}
  void PutFlt(const int& TupN, const int& VarN, const double& Flt){
    TupV[TupN]->PutVal(VarN, VarV[VarN]->GetFltVal(Flt));}
  void PutVal(const int& TupN, const int& VarN, const TTbVal& Val){
    TupV[TupN]->PutVal(VarN, VarV[VarN]->GetVal(Val));}
  TTbVal GetVal(const int& TupN, const int& VarN){
    return TupV[TupN]->GetVal(VarN);}
  TStr GetValStr(const int& TupN, const int& VarN);
};

