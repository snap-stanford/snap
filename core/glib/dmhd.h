/////////////////////////////////////////////////
// Domain-Header-Variable
ClassTPV(TDmHdVar, PDmHdVar, TDmHdVarV)//{
private:
  UndefCopy(TDmHdVar);
public:
  TDmHdVar(){}
  virtual ~TDmHdVar(){}
  TDmHdVar(TSIn&){}
  static PDmHdVar Load(TSIn& SIn);
  virtual void Save(TSOut& SOut){GetTypeNm(*this).Save(SOut);}

  TDmHdVar& operator=(const TDmHdVar&){return *this;}
  bool operator==(const TDmHdVar&) const {return true;}

  virtual TStr GetNm() const=0;
  virtual PTbVarType GetVarType() const=0;
};

/////////////////////////////////////////////////
// Domain-Header
ClassTP(TDmHd, PDmHd)//{
private:
  UndefCopy(TDmHd);
public:
  TDmHd(){}
  virtual ~TDmHd(){}
  TDmHd(TSIn&){}
  static PDmHd Load(TSIn&);
  virtual void Save(TSOut& SOut){GetTypeNm(*this).Save(SOut);}

  TDmHd& operator=(const TDmHd&){return *this;}
  bool operator==(const TDmHd& DmHd) const;

  virtual int GetClasses() const=0;
  virtual int GetClassN(const TStr& Nm) const=0;
  virtual TStr GetClassNm(const int& ClassN) const=0;
  virtual PDmHdVar GetClass(const int& ClassN) const=0;
  PDmHdVar GetClass(const TStr& Nm) const {return GetClass(GetClassN(Nm));}

  virtual int GetAttrs() const=0;
  virtual int GetAttrN(const TStr& Nm) const=0;
  virtual TStr GetAttrNm(const int& AttrN) const=0;
  virtual PDmHdVar GetAttr(const int& AttrN) const=0;
  PDmHdVar GetAttr(const TStr& Nm) const {return GetAttr(GetAttrN(Nm));}
};

/////////////////////////////////////////////////
// General-Domain-Header-Variable
class TGDmHdVar: public TDmHdVar{
private:
  TStr Nm;
  PTbVarType VarType;
public:
  TGDmHdVar(const TStr& _Nm, const PTbVarType& _VarType):
    TDmHdVar(), Nm(_Nm), VarType(_VarType){}
  TGDmHdVar(TSIn& SIn): TDmHdVar(SIn), Nm(SIn), VarType(SIn){}
  void Save(TSOut& SOut){
    TDmHdVar::Save(SOut); Nm.Save(SOut); VarType.Save(SOut);}

  TGDmHdVar& operator=(const TGDmHdVar& Var){
    TDmHdVar::operator=(Var); Nm=Var.Nm; VarType=Var.VarType; return *this;}
  bool operator==(const TGDmHdVar& Var) const {
    return (TDmHdVar::operator==(Var))&&(Nm==Var.Nm)&&(*VarType==*Var.VarType);}

  TStr GetNm() const {return Nm;}
  PTbVarType GetVarType() const {return VarType;}
};

/////////////////////////////////////////////////
// General-Domain-Header
class TGDmHd: public TDmHd{
private:
  TDmHdVarV ClassVarV;
  TDmHdVarV AttrVarV;
  int GetVarN(const TStr& Nm, const TDmHdVarV& VarV) const;
public:
  TGDmHd(): TDmHd(), ClassVarV(), AttrVarV(){}
  TGDmHd(TSIn& SIn): TDmHd(SIn), ClassVarV(SIn), AttrVarV(SIn){}
  void Save(TSOut& SOut){
    TDmHd::Save(SOut); ClassVarV.Save(SOut); AttrVarV.Save(SOut);}

  TGDmHd& operator=(const TGDmHd& GDmHd){TDmHd::operator=(GDmHd);
    ClassVarV=GDmHd.ClassVarV; AttrVarV=GDmHd.AttrVarV; return *this;}

  void AddClass(const PTbVar& Var){
    ClassVarV.Add(PDmHdVar(new TGDmHdVar(Var->GetNm(), Var->GetVarType())));}
  void AddAttr(const PTbVar& Var){
    AttrVarV.Add(PDmHdVar(new TGDmHdVar(Var->GetNm(), Var->GetVarType())));}

  int GetClasses() const {return ClassVarV.Len();}
  int GetClassN(const TStr& Nm) const {return GetVarN(Nm, ClassVarV);}
  TStr GetClassNm(const int& ClassN) const {return GetClass(ClassN)->GetNm();}
  PDmHdVar GetClass(const int& ClassN) const {return ClassVarV[ClassN];}

  int GetAttrs() const {return AttrVarV.Len();}
  int GetAttrN(const TStr& Nm) const {return GetVarN(Nm, AttrVarV);}
  TStr GetAttrNm(const int& AttrN) const {return GetAttr(AttrN)->GetNm();}
  PDmHdVar GetAttr(const int& AttrN) const {return AttrVarV[AttrN];}
};

