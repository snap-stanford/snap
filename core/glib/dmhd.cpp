/////////////////////////////////////////////////
// Domain-Header-Variable
PDmHdVar TDmHdVar::Load(TSIn& SIn){
  TStr TypeNm(SIn);
  if (TypeNm==TTypeNm<TGDmHdVar>()){return new TGDmHdVar(SIn);}
  else {Fail; return NULL;}
}

/////////////////////////////////////////////////
// Domain-Header
PDmHd TDmHd::Load(TSIn& SIn){
  TStr TypeNm(SIn);
  if (TypeNm==TTypeNm<TGDmHd>()){return new TGDmHd(SIn);}
  else {Fail; return NULL;}
}

bool TDmHd::operator==(const TDmHd& DmHd) const {
  if ((GetClasses()!=DmHd.GetClasses())||(GetAttrs()!=DmHd.GetAttrs())){
    return false;}
  for (int ClassN=0; ClassN<GetClasses(); ClassN++){
    if (*GetClass(ClassN)!=*DmHd.GetClass(ClassN)){return false;}}
  for (int AttrN=0; AttrN<GetAttrs(); AttrN++){
    if (*GetAttr(AttrN)!=*DmHd.GetAttr(AttrN)){return false;}}
  return true;
}

/////////////////////////////////////////////////
// General-Domain-Header
int TGDmHd::GetVarN(const TStr& Nm, const TDmHdVarV& VarV) const {
  int VarN=0;
  while ((VarN<VarV.Len())&&(VarV[VarN]->GetNm()!=Nm)){VarN++;}
  if (VarN<VarV.Len()){return VarN;} else {return -1;}
}




