/////////////////////////////////////////////////
// Domain
ClassTP(TDm, PDm)//{
private:
  TStr Nm;
public:
  TDm(const TStr& _Nm="Untitled-Domain"): Nm(_Nm){}
  TDm(const TDm& Dm): Nm(Dm.Nm){}
  virtual ~TDm(){}
  TDm(TSIn& SIn): Nm(SIn){}
  static PDm Load(TSIn& SIn);
  virtual void Save(TSOut& SOut){GetTypeNm(*this).Save(SOut); Nm.Save(SOut);}

  TDm& operator=(const TDm& Dm){
    Nm=Dm.Nm; return *this;}

  virtual void PutNm(const TStr& _Nm){Nm=_Nm;}
  virtual TStr GetNm() const {return Nm;}

  virtual int GetClasses() const=0;
  int GetClassN(const TStr& ClassNm) const;
  virtual PTbVar GetClass(const int& ClassN=0) const=0;
  virtual TTbVal GetClassVal(const int& ExN, const int& ClassN=0) const=0;
  virtual TStr GetClassValStr(const int& ExN, const int& ClassN=0) const=0;

  virtual int GetAttrs() const=0;
  int GetAttrN(const TStr& AttrNm) const;
  virtual PTbVar GetAttr(const int& AttrN) const=0;
  virtual TTbVal GetAttrVal(const int& ExN, const int& AttrN) const=0;
  virtual TStr GetAttrValStr(const int& ExN, const int& AttrN) const=0;
  double GetAttrNrmFlt(const int& ExN, const int& AttrN){
    return GetAttr(AttrN)->GetVarType()->GetNrmFlt(GetAttrVal(ExN, AttrN));}

  virtual int GetExs() const=0;
  virtual PExSet GetExSet() const=0;

  virtual PDmHd GetDmHd() const=0;

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm);
  static PDm New(const PPp& Pp, const PDm& _Dm=NULL);

  static PDm LoadFile(
   const TStr& FNm, const TStr& FType="", const TStr& MemRep="General");
};

/////////////////////////////////////////////////
// Class-Identification
class TClassId{
public:
  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm);
  static int New(const PPp& Pp, const PDm& Dm, const int& _ClassN=-1);
};

/////////////////////////////////////////////////
// Domain-Check
class TDmCheck{
private:
  PDm Dm;
public:
  TDmCheck(const PDm& _Dm): Dm(_Dm){}

  bool IsClass(const int& ClassN=0) const {
    return (0<=ClassN)&&(ClassN<Dm->GetClasses());}
  bool IsAttr(const int& AttrN) const {
    return (0<=AttrN)&&(AttrN<Dm->GetAttrs());}
  bool IsClassDsc(const int& ClassN=0) const {
    return (Dm->GetClass(ClassN)->GetActVTSet().In(tvtDsc));}
  bool IsAttrDsc(const int& AttrN) const {
    return (Dm->GetAttr(AttrN)->GetActVTSet().In(tvtDsc));}
  bool AreClassesDsc() const;
  bool AreAttrsDsc() const;
  bool IsDmDsc() const {
    return AreClassesDsc()&&AreAttrsDsc();}
  bool IsDmDsc(const int& ClassN=0) const {
    return IsClassDsc(ClassN)&&AreAttrsDsc();}
};

/////////////////////////////////////////////////
// General-Domain
class TGDm: public TDm{
private:
  PTb Tb;
  TIntV ClassToVarV;
  TIntV AttrToVarV;
  TIntV ExToTupV;
public:
  TGDm(const PTb& _Tb, const TStr& _Nm);
  TGDm(TSIn& SIn):
    TDm(SIn), Tb(TTb::Load(SIn)),
    ClassToVarV(SIn), AttrToVarV(SIn), ExToTupV(SIn){}
  void Save(TSOut& SOut){
    TDm::Save(SOut); Tb.Save(SOut);
    ClassToVarV.Save(SOut); AttrToVarV.Save(SOut); ExToTupV.Save(SOut);}

  TGDm& operator=(const TGDm& GDm){
    TDm::operator=(GDm); Tb=GDm.Tb;
    ClassToVarV=GDm.ClassToVarV; AttrToVarV=GDm.AttrToVarV;
    ExToTupV=GDm.ExToTupV; return *this;}

  int GetClasses() const {return ClassToVarV.Len();}
  PTbVar GetClass(const int& ClassN) const {
    return Tb->GetVar(ClassToVarV[ClassN]);}
  TTbVal GetClassVal(const int& ExN, const int& ClassN) const {
    return Tb->GetVal(ExToTupV[ExN], ClassToVarV[ClassN]);}
  TStr GetClassValStr(const int& ExN, const int& ClassN) const {
    return Tb->GetValStr(ExToTupV[ExN], ClassToVarV[ClassN]);}

  int GetAttrs() const {return AttrToVarV.Len();}
  PTbVar GetAttr(const int& AttrN) const {
    return Tb->GetVar(AttrToVarV[AttrN]);}
  TTbVal GetAttrVal(const int& ExN, const int& AttrN) const {
    return Tb->GetVal(ExToTupV[ExN], AttrToVarV[AttrN]);}
  TStr GetAttrValStr(const int& ExN, const int& AttrN) const {
    return Tb->GetValStr(ExToTupV[ExN], AttrToVarV[AttrN]);}

  int GetExs() const {return ExToTupV.Len();}
  PExSet GetExSet() const;

  PDmHd GetDmHd() const;

  static PDm LoadFile(const TStr& FNm, const TStr& FType="");
};

/////////////////////////////////////////////////
// Multiple-Domain
class TMDm: public TDm{
private:
  TTbV TbV;
  TIntPrV ClassToVarV;
  TIntPrV AttrToVarV;
  TIntV ExToTupV;
public:
  TMDm(const TStr& _Nm):
    TDm(_Nm), TbV(), ClassToVarV(), AttrToVarV(), ExToTupV(){}
  TMDm(TSIn& SIn):
    TDm(SIn), TbV(SIn), ClassToVarV(SIn), AttrToVarV(SIn), ExToTupV(SIn){}
  void Save(TSOut& SOut){
    TDm::Save(SOut); TbV.Save(SOut);
    ClassToVarV.Save(SOut); AttrToVarV.Save(SOut); ExToTupV.Save(SOut);}

  TDm& operator=(const TMDm& MDm){
    TDm::operator=(MDm); TbV=MDm.TbV;
    ClassToVarV=MDm.ClassToVarV; AttrToVarV=MDm.AttrToVarV;
    ExToTupV=MDm.ExToTupV; return *this;}

  int AddTb(const PTb& Tb){return TbV.Add(Tb);}
  int AddClass(const int& TbN, const int& VarN){
    return ClassToVarV.Add(TIntPr(TbN, VarN));}
  int AddAttr(const int& TbN, const int& VarN){
    return AttrToVarV.Add(TIntPr(TbN, VarN));}

  int GetClasses() const {return ClassToVarV.Len();}
  PTbVar GetClass(const int& ClassN) const {
    return TbV[ClassToVarV[ClassN].Val1]->GetVar(ClassToVarV[ClassN].Val2);}
  TTbVal GetClassVal(const int& ExN, const int& ClassN) const {
    return TbV[ClassToVarV[ClassN].Val1]->
     GetVal(ExToTupV[ExN], ClassToVarV[ClassN].Val2);}
  TStr GetClassValStr(const int& ExN, const int& ClassN) const {
    return TbV[ClassToVarV[ClassN].Val1]->
     GetValStr(ExToTupV[ExN], ClassToVarV[ClassN].Val2);}

  int GetAttrs() const {return AttrToVarV.Len();}
  PTbVar GetAttr(const int& AttrN) const {
    return TbV[AttrToVarV[AttrN].Val1]->GetVar(AttrToVarV[AttrN].Val2);}
  TTbVal GetAttrVal(const int& ExN, const int& AttrN) const {
    return TbV[AttrToVarV[AttrN].Val1]->
      GetVal(ExToTupV[ExN], AttrToVarV[AttrN].Val2);}
  TStr GetAttrValStr(const int& ExN, const int& AttrN) const {
    return TbV[AttrToVarV[AttrN].Val1]->
      GetValStr(ExToTupV[ExN], AttrToVarV[AttrN].Val2);}

  int GetExs() const {return ExToTupV.Len();}
  PExSet GetExSet() const;

  PDmHd GetDmHd() const;
};

