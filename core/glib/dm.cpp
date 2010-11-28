/////////////////////////////////////////////////
// Domain
PDm TDm::Load(TSIn& SIn){
  TStr TypeNm(SIn);
  if (TypeNm==TTypeNm<TGDm>()){return new TGDm(SIn);}
  else {Fail; return NULL;}
}

int TDm::GetClassN(const TStr& ClassNm) const {
  for (int ClassN=0; ClassN<GetClasses(); ClassN++){
    if (GetClass(ClassN)->GetNm()==ClassNm){return ClassN;}}
  return -1;
}

int TDm::GetAttrN(const TStr& AttrNm) const {
  for (int AttrN=0; AttrN<GetAttrs(); AttrN++){
    if (GetAttr(AttrN)->GetNm()==AttrNm){return AttrN;}}
  return -1;
}

const TStr TDm::DNm("Domain");

PPp TDm::GetPp(const TStr& Nm, const TStr& DNm){
  PPp FNmPp=new TPp("FNm", "File-Name", ptStr);
  FNmPp->PutDfVal("../domain.txt");

  PPp FTypePp=new TPp("FType", "File-Type", ptStr);
  FTypePp->AddCcVal("Text"); FTypePp->PutDfVal("Text");

  PPp MemRepPp=new TPp("MemRep", "Memory-Rep.", ptStr);
  MemRepPp->AddCcVal("General"); MemRepPp->PutDfVal("General");

  PPp Pp=new TPp(Nm, DNm, ptSet);
  Pp->AddPp(FNmPp);
  Pp->AddPp(FTypePp);
  Pp->AddPp(MemRepPp);
  return Pp;
}

PDm TDm::New(const PPp& Pp, const PDm& _Dm){
  if (!_Dm.Empty()){return _Dm;}
  TStr FNm=Pp->GetValStr("FNm");
  TStr FType=Pp->GetValStr("FType");
  TStr MemRep=Pp->GetValStr("MemRep");
  return LoadFile(FNm, FType, MemRep);
}

PDm TDm::LoadFile(const TStr& FNm, const TStr& FType, const TStr& MemRep){
  if (MemRep=="General"){return TGDm::LoadFile(FNm, FType);}
  else {Fail; return NULL;}
}

/////////////////////////////////////////////////
// Class-Identification
const TStr TClassId::DNm("Class-Name");

PPp TClassId::GetPp(const TStr& Nm, const TStr& DNm){
  PPp Pp=new TPp(Nm, DNm, ptStr);
  Pp->PutDfVal(TPpVal(TStr()));
  return Pp;
}

int TClassId::New(const PPp& Pp, const PDm& Dm, const int& _ClassN){
  TDmCheck DmCheck(Dm);
  if (_ClassN!=-1){
    IAssert(DmCheck.IsClass(_ClassN)); return _ClassN;}

  TStr ClassNm=Pp->GetValStr();
  if (ClassNm.Empty()){
    IAssert(DmCheck.IsClass(0)); return 0;
  } else {
    int ClassN=Dm->GetClassN(ClassNm);
    IAssert(ClassN); return ClassN;
  }
}

/////////////////////////////////////////////////
// Domain-Check
bool TDmCheck::AreClassesDsc() const {
  if (Dm->GetClasses()==0){return false;}
  for (int ClassN=0; ClassN<Dm->GetClasses(); ClassN++){
    if (!IsClassDsc(ClassN)){return false;}}
  return true;
}

bool TDmCheck::AreAttrsDsc() const {
  if (Dm->GetAttrs()==0){return false;}
  for (int AttrN=0; AttrN<Dm->GetAttrs(); AttrN++){
    if (!IsAttrDsc(AttrN)){return false;}}
  return true;
}

/////////////////////////////////////////////////
// General-Domain
TGDm::TGDm(const PTb& _Tb, const TStr& _Nm):
  TDm(_Nm), Tb(_Tb),
  ClassToVarV(), AttrToVarV(), ExToTupV(){

  if (TTbVarType::IsSuffixVarNm(Tb)){
    // approximate (most probable) allocation
    ClassToVarV.Gen(1, 0); AttrToVarV.Gen(Tb->GetVars()-1, 0);
    for (int VarN=0; VarN<Tb->GetVars(); VarN++){
      TStr VarNm=Tb->GetVar(VarN)->GetNm();
      TTbSufixVarCat TbSufixVarCat=TTbVarType::GetSufixVarCat(VarNm);
      switch (TbSufixVarCat){
        case tsvcClass:
          ClassToVarV.Add(VarN); break;
        case tsvcAttr:
        case tsvcUndef:
          AttrToVarV.Add(VarN); break;
        default: Fail; //J: not all values are handled in switch statement
      }
    }
  } else {
    int Classes=1;
    ClassToVarV.Gen(Classes, 0);
    AttrToVarV.Gen(Tb->GetVars()-Classes, 0);
    for (int VarN=0; VarN<Classes; VarN++){ClassToVarV.Add(VarN);}
    {for (int VarN=Classes; VarN<Tb->GetVars(); VarN++){AttrToVarV.Add(VarN);}}
  }
  // examples
  ExToTupV.Gen(Tb->GetTups(), 0);
  for (int TupN=0; TupN<Tb->GetTups(); TupN++){ExToTupV.Add(TupN);}
}

PExSet TGDm::GetExSet() const {
  int Exs=GetExs();
  PExSet ExSet=TGExSet::New(Exs);
  for (int ExN=0; ExN<Exs; ExN++){ExSet->AddEx(ExN);}
  return ExSet;
}

PDmHd TGDm::GetDmHd() const {
  TGDmHd& GDmHd=*new TGDmHd(); PDmHd DmHd(&GDmHd);
  for (int ClassN=0; ClassN<ClassToVarV.Len(); ClassN++){
    GDmHd.AddClass(GetClass(ClassN));}
  for (int AttrN=0; AttrN<AttrToVarV.Len(); AttrN++){
    GDmHd.AddAttr(GetAttr(AttrN));}
  return DmHd;
}

PDm TGDm::LoadFile(const TStr& FNm, const TStr& FType){
  PTb Tb=TTb::LoadFile(FNm, FType, "General");
  return PDm(new TGDm(Tb, FNm));
}

/////////////////////////////////////////////////
// Multiple-Domain
PExSet TMDm::GetExSet() const{
  int Exs=GetExs();
  PExSet ExSet=TGExSet::New(Exs);
  for (int ExN=0; ExN<Exs; ExN++){ExSet->AddEx(ExN, 1);}
  return ExSet;
}

PDmHd TMDm::GetDmHd() const {
  TGDmHd& GDmHd=*new TGDmHd(); PDmHd DmHd(&GDmHd);
  for (int ClassN=0; ClassN<ClassToVarV.Len(); ClassN++){
    GDmHd.AddClass(GetClass(ClassN));}
  for (int AttrN=0; AttrN<AttrToVarV.Len(); AttrN++){
    GDmHd.AddAttr(GetAttr(AttrN));}
  return DmHd;
}

