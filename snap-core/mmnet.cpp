////////////////////////////////////////////////
// Mutimodal Network

TStr TModeNet::GetNeighborCrossName(const TStr& CrossName, bool isOutEdge, const bool sameMode, bool isDir) const {
  TStr Cpy(CrossName);
  if (!isDir || !sameMode) { return Cpy; }
  if (isOutEdge) {
    Cpy += ":SRC";
  } else {
    Cpy += ":DST";
  }
  return Cpy;
}

void TModeNet::ClrNbr(const TStr& CrossNetName, const bool& outEdge, const bool& sameMode, bool& isDir) {
  TStr Name = GetNeighborCrossName(CrossNetName, outEdge, sameMode, isDir);
  TVec<TIntV> Attrs(MxNId);
  int index = KeyToIndexTypeN.GetDat(Name).Val2;
  VecOfIntVecVecsN[index] = Attrs;
}

void TModeNet::Clr() {
  TStrV CNets;
  NeighborTypes.GetKeyV(CNets);
  for (int i=0; i < CNets.Len(); i++) {
    MMNet->GetCrossNetByName(CNets[i]).Clr();
  }
  TNEANet::Clr();
}

int TModeNet::AddNeighbor(const int& NId, const int& EId, bool outEdge, const int linkId, const bool sameMode, bool isDir){
  TStr CrossName = MMNet->GetCrossName(linkId);
  return AddNeighbor(NId, EId, outEdge, CrossName, sameMode, isDir);
}

int TModeNet::AddNeighbor(const int& NId, const int& EId, bool outEdge, const TStr& CrossName, const bool sameMode, bool isDir){
  if (!NeighborTypes.IsKey(CrossName)) {
    AddNbrType(CrossName, sameMode, isDir);
  }
  TStr Name = GetNeighborCrossName(CrossName, outEdge, sameMode, isDir);
  return AppendIntVAttrDatN(NId, EId, Name); 
}

int TModeNet::DelNeighbor(const int& NId, const int& EId, bool outEdge, const TStr& CrossName, const bool sameMode, bool isDir){
  if (!NeighborTypes.IsKey(CrossName)) {
    return -1;
  }
  TStr Name = GetNeighborCrossName(CrossName, outEdge, sameMode, isDir);
  return DelFromIntVAttrDatN(NId, EId, Name);
}

int TModeNet::DelNeighbor(const int& NId, const int& EId, bool outEdge, const TInt& linkId, const bool sameMode, bool isDir){
  TStr CrossName = MMNet->GetCrossName(linkId);
  return DelNeighbor(NId, EId, outEdge, CrossName, sameMode, isDir);
}

void TModeNet::DelNode(const int& NId) {
  TStrV Names;
  GetCrossNetNames(Names);
  for (int i=0; i < Names.Len(); i++) {
    TCrossNet& Cross = MMNet->GetCrossNetByName(Names[i]);
    TIntV OutEIds;
    GetNeighborsByCrossNet(NId, Names[i], OutEIds, true);
    for (int j=0; j < OutEIds.Len(); j++) {
      Cross.DelEdge(OutEIds[j].Val);
    }
    if (Cross.IsDirect && Cross.Mode1 == Cross.Mode2) {
      TIntV InEIds;
      GetNeighborsByCrossNet(NId, Names[i], InEIds, false);
      for (int j=0; j < InEIds.Len(); j++) {
        Cross.DelEdge(InEIds[j].Val);
      }
    }
  }
  TNEANet::DelNode(NId);
}

void TModeNet::SetParentPointer(TMMNet* parent) {
  MMNet = parent;
}

int TModeNet::AddNbrType(const TStr& CrossName, const bool sameMode, bool isDir) {
  //IAssertR(!NeighborTypes.IsKey(CrossName),TStr::Fmt("Neighbor Cross Types already exists: %s", CrossName.CStr()));
  if (NeighborTypes.IsKey(CrossName)) { return -1; } //Don't want to add nbr type multiple times
  bool hasSingleVector = (!isDir || !sameMode);
  NeighborTypes.AddDat(CrossName, hasSingleVector);
  return 0;
}

int TModeNet::DelNbrType(const TStr& CrossName) {
  //IAssertR(!NeighborTypes.IsKey(CrossName),TStr::Fmt("Neighbor Cross Types already exists: %s", CrossName.CStr()));
  bool hasSingleVector = NeighborTypes.GetDat(CrossName);
  NeighborTypes.DelKey(CrossName);
  if (hasSingleVector) {
    return DelAttrN(CrossName);
  } else {
    TStr InName = GetNeighborCrossName(CrossName, true, true, true);
    TStr OutName = GetNeighborCrossName(CrossName, false, true, true);
    if (DelAttrN(InName) == -1 || DelAttrN(OutName) == -1) {
      return -1;
    }
  }
  return 0;
}

void TModeNet::GetNeighborsByCrossNet(const int& NId, TStr& Name, TIntV& Neighbors, const bool isOutEId) const{
  //IAssertR(NeighborTypes.IsKey(Name), TStr::Fmt("Cross Type does not exist: %s", Name));
  TBool hasSingleVector = NeighborTypes.GetDat(Name);
  if (hasSingleVector) {
    Neighbors = GetIntVAttrDatN(NId, Name);
  } else {
    TStr DirectionalName = GetNeighborCrossName(Name, isOutEId, true, true);
    Neighbors = GetIntVAttrDatN(NId, DirectionalName);
  }
}

int TModeNet::AddIntVAttrByVecN(const TStr& attr, TVec<TIntV>& Attrs){
  TInt CurrLen;
  TVec<TIntV> NewVec;
  CurrLen = VecOfIntVecVecsN.Len();
  KeyToIndexTypeN.AddDat(attr, TIntPr(IntVType, CurrLen));
  VecOfIntVecVecsN.Add(Attrs);
  return 0;
}

void TModeNet::RemoveCrossNets(TModeNet& Result, TStrV& CrossNets) {
  const TModeNet& self = *this;
  Result = TModeNet(self, false);
  for (TStrIntPrH::TIter it = KeyToIndexTypeN.BegI(); it < KeyToIndexTypeN.EndI(); it++) {
    TStr AttrName = it.GetKey();
    TInt AttrType = it.GetDat().GetVal1();
    TInt AttrIndex = it.GetDat().GetVal2();
    if (AttrType != IntVType) {
      Result.KeyToIndexTypeN.AddDat(AttrName, it.GetDat());
    } else {
      TStr WithoutSuffix = AttrName;
      bool removeSuffix = false;
      if (AttrName.IsSuffix(":SRC") || AttrName.IsSuffix(":DST")) {
        WithoutSuffix = AttrName.GetSubStr(0, AttrName.Len()-5);
        removeSuffix = true;
      }
      bool isSingleVNbrAttr = (NeighborTypes.IsKey(AttrName) && NeighborTypes.GetDat(AttrName));
      bool isMultiVNbrAttr = (removeSuffix && NeighborTypes.IsKey(WithoutSuffix) && !NeighborTypes.GetDat(WithoutSuffix));
      if (isSingleVNbrAttr || isMultiVNbrAttr) {
        TStr NbrName = isSingleVNbrAttr ? AttrName : WithoutSuffix;
        if (CrossNets.IsIn(NbrName)) {
          Result.AddNbrType(NbrName, removeSuffix, removeSuffix);
          TVec<TIntV>& Attrs = VecOfIntVecVecsN[AttrIndex];
          Result.AddIntVAttrByVecN(AttrName, Attrs);
        }
      } else {
        TVec<TIntV>& Attrs = VecOfIntVecVecsN[AttrIndex];
        Result.AddIntVAttrByVecN(AttrName, Attrs);
      }
    }
  }
}

int TModeNet::GetAttrTypeN(const TStr& attr) const {
  if (KeyToIndexTypeN.IsKey(attr)) {
    return KeyToIndexTypeN.GetDat(attr).Val1;
  }
  return -1;
}


int TCrossNet::GetAttrTypeE(const TStr& attr) const {
  if (KeyToIndexTypeE.IsKey(attr)) {
    return KeyToIndexTypeE.GetDat(attr).Val1;
  }
  return -1;
}

void TCrossNet::Clr() {
  CrossH.Clr();
  MxEId=0;
  KeyToIndexTypeE.Clr();
  IntDefaultsE.Clr();
  StrDefaultsE.Clr();
  FltDefaultsE.Clr();
  VecOfIntVecsE.Clr();
  VecOfStrVecsE.Clr();
  VecOfFltVecsE.Clr(); 
  Net->ClrNbr(Mode1, CrossNetId, true, Mode1==Mode2, IsDirect.Val);
  Net->ClrNbr(Mode2, CrossNetId, false, Mode1==Mode2, IsDirect.Val); 
}

int TCrossNet::AddEdge(const int& sourceNId, const int& destNId, int EId){
  if (EId == -1) { EId = MxEId;  MxEId++; }
  else { MxEId = TMath::Mx(EId+1, MxEId()); }
  if (Net != NULL) {
    TModeNet& M1 = Net->TModeNetH.GetDat(this->Mode1);
    TModeNet& M2 = Net->TModeNetH.GetDat(this->Mode2);
    if (!M1.IsNode(sourceNId) || !M2.IsNode(destNId)) { return -1; }
    TStr ThisCrossName = Net->GetCrossName(this->CrossNetId);
    M1.AddNeighbor(sourceNId, EId, true, ThisCrossName, Mode1==Mode2, IsDirect);
    M2.AddNeighbor(destNId, EId, false, ThisCrossName, Mode1==Mode2, IsDirect);
  }
  TCrossNet::TCrossEdge newEdge(EId, sourceNId, destNId);
  CrossH.AddDat(EId, newEdge);
  int i;
  // update attribute columns
  for (i = 0; i < VecOfIntVecsE.Len(); i++) {
    TVec<TInt>& IntVec = VecOfIntVecsE[i];
    int KeyId = CrossH.GetKeyId(EId);
    if (IntVec.Len() > KeyId) {
      IntVec[KeyId] = TInt::Mn;
    } else {
      IntVec.Ins(KeyId, TInt::Mn);
    }
  }
  TVec<TStr> DefIntVec = TVec<TStr>();
  IntDefaultsE.GetKeyV(DefIntVec);
  for (i = 0; i < DefIntVec.Len(); i++) {
    TStr attr = DefIntVec[i];
    TVec<TInt>& IntVec = VecOfIntVecsE[KeyToIndexTypeE.GetDat(DefIntVec[i]).Val2];
    IntVec[CrossH.GetKeyId(EId)] = GetIntAttrDefaultE(attr);
  }

  for (i = 0; i < VecOfStrVecsE.Len(); i++) {
    TVec<TStr>& StrVec = VecOfStrVecsE[i];
    int KeyId = CrossH.GetKeyId(EId);
    if (StrVec.Len() > KeyId) {
      StrVec[KeyId] = TStr::GetNullStr();
    } else {
      StrVec.Ins(KeyId, TStr::GetNullStr());
    }
  }
  TVec<TStr> DefStrVec = TVec<TStr>();
  StrDefaultsE.GetKeyV(DefStrVec);
  for (i = 0; i < DefStrVec.Len(); i++) {
    TStr attr = DefStrVec[i];
    TVec<TStr>& StrVec = VecOfStrVecsE[KeyToIndexTypeE.GetDat(DefStrVec[i]).Val2];
    StrVec[CrossH.GetKeyId(EId)] = GetStrAttrDefaultE(attr);
  }

  for (i = 0; i < VecOfFltVecsE.Len(); i++) {
    TVec<TFlt>& FltVec = VecOfFltVecsE[i];
    int KeyId = CrossH.GetKeyId(EId);
    if (FltVec.Len() > KeyId) {
      FltVec[KeyId] = TFlt::Mn;
    } else {
      FltVec.Ins(KeyId, TFlt::Mn);
    }
  }
  TVec<TStr> DefFltVec = TVec<TStr>();
  FltDefaultsE.GetKeyV(DefFltVec);
  for (i = 0; i < DefFltVec.Len(); i++) {
    TStr attr = DefFltVec[i];
    TVec<TFlt>& FltVec = VecOfFltVecsE[KeyToIndexTypeE.GetDat(DefFltVec[i]).Val2];
    FltVec[CrossH.GetKeyId(EId)] = GetFltAttrDefaultE(attr);
  }
  return EId;
}

int TCrossNet::DelEdge(const int& EId) {
  TCrossEdge& Edge = CrossH.GetDat(EId);
  int srcNode = Edge.SrcNId;
  int dstNode = Edge.DstNId;
  TStr ThisCrossName = Net->GetCrossName(this->CrossNetId);
  Net->GetModeNetById(this->Mode1).DelNeighbor(srcNode, EId, true, ThisCrossName, Mode1==Mode2, IsDirect);
  Net->GetModeNetById(this->Mode2).DelNeighbor(dstNode, EId, false, ThisCrossName, Mode1==Mode2, IsDirect);
  int i;
  for (i = 0; i < VecOfIntVecsE.Len(); i++) {
    TVec<TInt>& IntVec = VecOfIntVecsE[i];
    IntVec[CrossH.GetKeyId(EId)] =  TInt::Mn;
  }
  for (i = 0; i < VecOfStrVecsE.Len(); i++) {
    TVec<TStr>& StrVec = VecOfStrVecsE[i];
    StrVec[CrossH.GetKeyId(EId)] =  TStr::GetNullStr();
  }
  for (i = 0; i < VecOfFltVecsE.Len(); i++) {
    TVec<TFlt>& FltVec = VecOfFltVecsE[i];
    FltVec[CrossH.GetKeyId(EId)] = TFlt::Mn;
  }
  CrossH.DelKey(EId);
  return 0;
}

void TCrossNet::SetParentPointer(TMMNet* parent) {
  Net = parent;
}

void TCrossNet::AttrNameEI(const TInt& EId, TStrIntPrH::TIter CrossHI, TStrV& Names) const {
  Names = TVec<TStr>();
  while (!CrossHI.IsEnd()) {
    if (!EdgeAttrIsDeleted(EId, CrossHI)) {
      Names.Add(CrossHI.GetKey());
    }
    CrossHI++;
  }  
}

void TCrossNet::AttrValueEI(const TInt& EId, TStrIntPrH::TIter CrossHI, TStrV& Values) const {
  Values = TVec<TStr>();
  while (!CrossHI.IsEnd()) {
    if (!EdgeAttrIsDeleted(EId, CrossHI)) {
      Values.Add(GetEdgeAttrValue(EId, CrossHI));
    }
    CrossHI++;
  }  
}

void TCrossNet::IntAttrNameEI(const TInt& EId, TStrIntPrH::TIter CrossHI, TStrV& Names) const {
  Names = TVec<TStr>();
  while (!CrossHI.IsEnd()) {
    if (CrossHI.GetDat().Val1 == IntType && !EdgeAttrIsIntDeleted(EId, CrossHI)) {
      Names.Add(CrossHI.GetKey());
    }
    CrossHI++;
  }  
}

void TCrossNet::IntAttrValueEI(const TInt& EId, TStrIntPrH::TIter CrossHI, TIntV& Values) const {
  Values = TVec<TInt>();
  while (!CrossHI.IsEnd()) {
    if (CrossHI.GetDat().Val1 == IntType && !EdgeAttrIsIntDeleted(EId, CrossHI)) {
      TInt val = (this->VecOfIntVecsE.GetVal(CrossHI.GetDat().Val2).GetVal(EId));
      Values.Add(val);
    }
    CrossHI++;
  }  
}

void TCrossNet::StrAttrNameEI(const TInt& EId, TStrIntPrH::TIter CrossHI, TStrV& Names) const {
  Names = TVec<TStr>();
  while (!CrossHI.IsEnd()) {
    if (CrossHI.GetDat().Val1 == StrType && !EdgeAttrIsStrDeleted(EId, CrossHI)) {
      Names.Add(CrossHI.GetKey());
    }
    CrossHI++;
  }  
}

void TCrossNet::StrAttrValueEI(const TInt& EId, TStrIntPrH::TIter CrossHI, TStrV& Values) const {
  Values = TVec<TStr>();
  while (!CrossHI.IsEnd()) {
    if (CrossHI.GetDat().Val1 == StrType && !EdgeAttrIsStrDeleted(EId, CrossHI)) {
      TStr val = this->VecOfStrVecsE.GetVal(CrossHI.GetDat().Val2).GetVal(EId);
      Values.Add(val);
    }
    CrossHI++;
  }  
}

void TCrossNet::FltAttrNameEI(const TInt& EId, TStrIntPrH::TIter CrossHI, TStrV& Names) const {
  Names = TVec<TStr>();
  while (!CrossHI.IsEnd()) {
    if (CrossHI.GetDat().Val1 == FltType && !EdgeAttrIsFltDeleted(EId, CrossHI)) {
      Names.Add(CrossHI.GetKey());
    }
    CrossHI++;
  }  
}

void TCrossNet::FltAttrValueEI(const TInt& EId, TStrIntPrH::TIter CrossHI, TFltV& Values) const {
  Values = TVec<TFlt>();
  while (!CrossHI.IsEnd()) {
    if (CrossHI.GetDat().Val1 == FltType && !EdgeAttrIsFltDeleted(EId, CrossHI)) {
      TFlt val = (this->VecOfFltVecsE.GetVal(CrossHI.GetDat().Val2).GetVal(EId));
      Values.Add(val);
    }
    CrossHI++;
  }  
}

bool TCrossNet::IsAttrDeletedE(const int& EId, const TStr& attr) const {
  bool IntDel = IsIntAttrDeletedE(EId, attr);
  bool StrDel = IsStrAttrDeletedE(EId, attr);
  bool FltDel = IsFltAttrDeletedE(EId, attr);
  return IntDel || StrDel || FltDel;
}

bool TCrossNet::IsIntAttrDeletedE(const int& EId, const TStr& attr) const {
  return EdgeAttrIsIntDeleted(EId, KeyToIndexTypeE.GetI(attr));
}

bool TCrossNet::IsStrAttrDeletedE(const int& EId, const TStr& attr) const {
  return EdgeAttrIsStrDeleted(EId, KeyToIndexTypeE.GetI(attr));
}

bool TCrossNet::IsFltAttrDeletedE(const int& EId, const TStr& attr) const {
  return EdgeAttrIsFltDeleted(EId, KeyToIndexTypeE.GetI(attr));
}

bool TCrossNet::EdgeAttrIsDeleted(const int& EId, const TStrIntPrH::TIter& CrossHI) const {
  bool IntDel = EdgeAttrIsIntDeleted(EId, CrossHI);
  bool StrDel = EdgeAttrIsStrDeleted(EId, CrossHI);
  bool FltDel = EdgeAttrIsFltDeleted(EId, CrossHI);
  return IntDel || StrDel || FltDel;
}

bool TCrossNet::EdgeAttrIsIntDeleted(const int& EId, const TStrIntPrH::TIter& CrossHI) const {
  return (CrossHI.GetDat().Val1 == IntType &&
    GetIntAttrDefaultE(CrossHI.GetKey()) == this->VecOfIntVecsE.GetVal(
    this->KeyToIndexTypeE.GetDat(CrossHI.GetKey()).Val2).GetVal(CrossH.GetKeyId(EId)));
}

bool TCrossNet::EdgeAttrIsStrDeleted(const int& EId, const TStrIntPrH::TIter& CrossHI) const {
  return (CrossHI.GetDat().Val1 == StrType &&
    GetStrAttrDefaultE(CrossHI.GetKey()) == this->VecOfStrVecsE.GetVal(
    this->KeyToIndexTypeE.GetDat(CrossHI.GetKey()).Val2).GetVal(CrossH.GetKeyId(EId)));
}

bool TCrossNet::EdgeAttrIsFltDeleted(const int& EId, const TStrIntPrH::TIter& CrossHI) const {
  return (CrossHI.GetDat().Val1 == FltType &&
    GetFltAttrDefaultE(CrossHI.GetKey()) == this->VecOfFltVecsE.GetVal(
    this->KeyToIndexTypeE.GetDat(CrossHI.GetKey()).Val2).GetVal(CrossH.GetKeyId(EId)));
}

TStr TCrossNet::GetEdgeAttrValue(const int& EId, const TStrIntPrH::TIter& CrossHI) const {
  if (CrossHI.GetDat().Val1 == IntType) {
    return (this->VecOfIntVecsE.GetVal(
      this->KeyToIndexTypeE.GetDat(CrossHI.GetKey()).Val2).GetVal(CrossH.GetKeyId(EId))).GetStr();
  } else if(CrossHI.GetDat().Val1 == StrType) {
    return this->VecOfStrVecsE.GetVal(
    this->KeyToIndexTypeE.GetDat(CrossHI.GetKey()).Val2).GetVal(CrossH.GetKeyId(EId));
  } else if (CrossHI.GetDat().Val1 == FltType) {
    return (this->VecOfFltVecsE.GetVal(
      this->KeyToIndexTypeE.GetDat(CrossHI.GetKey()).Val2).GetVal(CrossH.GetKeyId(EId))).GetStr();
  }
  return TStr::GetNullStr();
}

int TCrossNet::AddIntAttrDatE(const int& EId, const TInt& value, const TStr& attr) {
  int i;
  TInt CurrLen;
  if (!IsEdge(EId)) {
    //AddEdge(EId);
     return -1;
  }
  if (KeyToIndexTypeE.IsKey(attr)) {
    TVec<TInt>& NewVec = VecOfIntVecsE[KeyToIndexTypeE.GetDat(attr).Val2];
    NewVec[CrossH.GetKeyId(EId)] = value;
  } else {
    CurrLen = VecOfIntVecsE.Len();
    KeyToIndexTypeE.AddDat(attr, TIntPr(IntType, CurrLen));
    TVec<TInt> NewVec = TVec<TInt>();
    for (i = 0; i < MxEId; i++) {
      NewVec.Ins(i, GetIntAttrDefaultE(attr));
    }
    NewVec[CrossH.GetKeyId(EId)] = value;
    VecOfIntVecsE.Add(NewVec);
  }
  return 0;
}

int TCrossNet::AddStrAttrDatE(const int& EId, const TStr& value, const TStr& attr) {
  int i;
  TInt CurrLen;
  if (!IsEdge(EId)) {
    //AddEdge(EId);
     return -1;
  }
  if (KeyToIndexTypeE.IsKey(attr)) {
    TVec<TStr>& NewVec = VecOfStrVecsE[KeyToIndexTypeE.GetDat(attr).Val2];
    NewVec[CrossH.GetKeyId(EId)] = value;
  } else {
    CurrLen = VecOfStrVecsE.Len();
    KeyToIndexTypeE.AddDat(attr, TIntPr(StrType, CurrLen));
    TVec<TStr> NewVec = TVec<TStr>();
    for (i = 0; i < MxEId; i++) {
      NewVec.Ins(i, GetStrAttrDefaultE(attr));
    }
    NewVec[CrossH.GetKeyId(EId)] = value;
    VecOfStrVecsE.Add(NewVec);
  }
  return 0;
} 

int TCrossNet::AddFltAttrDatE(const int& EId, const TFlt& value, const TStr& attr) {
  int i;
  TInt CurrLen;

  if (!IsEdge(EId)) {
    //AddEdge(EId);
     return -1;
  }
  if (KeyToIndexTypeE.IsKey(attr)) {
    TVec<TFlt>& NewVec = VecOfFltVecsE[KeyToIndexTypeE.GetDat(attr).Val2];
    NewVec[CrossH.GetKeyId(EId)] = value;
  } else {
    CurrLen = VecOfFltVecsE.Len();
    KeyToIndexTypeE.AddDat(attr, TIntPr(FltType, CurrLen));
    TVec<TFlt> NewVec = TVec<TFlt>();
    for (i = 0; i < MxEId; i++) {
      NewVec.Ins(i, GetFltAttrDefaultE(attr));
    }
    NewVec[CrossH.GetKeyId(EId)] = value;
    VecOfFltVecsE.Add(NewVec);
  }
  return 0;
}

TInt TCrossNet::GetIntAttrDatE(const int& EId, const TStr& attr) {
  return VecOfIntVecsE[KeyToIndexTypeE.GetDat(attr).Val2][CrossH.GetKeyId(EId)];
}

TStr TCrossNet::GetStrAttrDatE(const int& EId, const TStr& attr) {
  return VecOfStrVecsE[KeyToIndexTypeE.GetDat(attr).Val2][CrossH.GetKeyId(EId)];
}

TFlt TCrossNet::GetFltAttrDatE(const int& EId, const TStr& attr) {
  return VecOfFltVecsE[KeyToIndexTypeE.GetDat(attr).Val2][CrossH.GetKeyId(EId)];
}

int TCrossNet::DelAttrDatE(const int& EId, const TStr& attr) {
  // TODO(nkhadke): add error checking
  TInt vecType = KeyToIndexTypeE(attr).Val1;
  if (vecType == IntType) {
    VecOfIntVecsE[KeyToIndexTypeE.GetDat(attr).Val2][CrossH.GetKeyId(EId)] = GetIntAttrDefaultE(attr);
  } else if (vecType == StrType) {
    VecOfStrVecsE[KeyToIndexTypeE.GetDat(attr).Val2][CrossH.GetKeyId(EId)] = GetStrAttrDefaultE(attr);
  } else if (vecType == FltType) {
    VecOfFltVecsE[KeyToIndexTypeE.GetDat(attr).Val2][CrossH.GetKeyId(EId)] = GetFltAttrDefaultE(attr);
  } else {
    return -1;
  }
  return 0;
}

int TCrossNet::AddIntAttrE(const TStr& attr, TInt defaultValue){
  // TODO(nkhadke): add error checking
  int i;
  TInt CurrLen;
  TVec<TInt> NewVec;
  CurrLen = VecOfIntVecsE.Len();
  KeyToIndexTypeE.AddDat(attr, TIntPr(IntType, CurrLen));
  NewVec = TVec<TInt>();
  for (i = 0; i < MxEId; i++) {
    NewVec.Ins(i, defaultValue);
  }
  VecOfIntVecsE.Add(NewVec);
  if (!IntDefaultsE.IsKey(attr)) {
    IntDefaultsE.AddDat(attr, defaultValue);
  } else {
    return -1;
  }
  return 0;
}

int TCrossNet::AddStrAttrE(const TStr& attr, TStr defaultValue) {
  int i;
  TInt CurrLen;
  TVec<TStr> NewVec;
  CurrLen = VecOfStrVecsE.Len();
  KeyToIndexTypeE.AddDat(attr, TIntPr(StrType, CurrLen));
  NewVec = TVec<TStr>();
  for (i = 0; i < MxEId; i++) {
    NewVec.Ins(i, defaultValue);
  }
  VecOfStrVecsE.Add(NewVec);
  if (!StrDefaultsE.IsKey(attr)) {
    StrDefaultsE.AddDat(attr, defaultValue);
  } else {
    return -1;
  }
  return 0;
}

int TCrossNet::AddFltAttrE(const TStr& attr, TFlt defaultValue) {
  int i;
  TInt CurrLen;
  TVec<TFlt> NewVec;
  CurrLen = VecOfFltVecsE.Len();
  KeyToIndexTypeE.AddDat(attr, TIntPr(FltType, CurrLen));
  NewVec = TVec<TFlt>();
  for (i = 0; i < MxEId; i++) {
    NewVec.Ins(i, defaultValue);
  }
  VecOfFltVecsE.Add(NewVec);
  if (!FltDefaultsE.IsKey(attr)) {
    FltDefaultsE.AddDat(attr, defaultValue);
  } else {
    return -1;
  }
  return 0;
}

int TCrossNet::DelAttrE(const TStr& attr) {
  TInt vecType = KeyToIndexTypeE(attr).Val1;
  if (vecType == IntType) {
    VecOfIntVecsE[KeyToIndexTypeE.GetDat(attr).Val2] = TVec<TInt>();
    if (IntDefaultsE.IsKey(attr)) {
      IntDefaultsE.DelKey(attr);
    }
  } else if (vecType == StrType) {
    VecOfStrVecsE[KeyToIndexTypeE.GetDat(attr).Val2] = TVec<TStr>();
    if (StrDefaultsE.IsKey(attr)) {
      StrDefaultsE.DelKey(attr);
    }  
  } else if (vecType == FltType) {
    VecOfFltVecsE[KeyToIndexTypeE.GetDat(attr).Val2] = TVec<TFlt>();
    if (FltDefaultsE.IsKey(attr)) {
      FltDefaultsE.DelKey(attr);
    }
  } else {
    return -1;
  }
  KeyToIndexTypeE.DelKey(attr);
  return 0;
}


int TMMNet::AddModeNet(const TStr& ModeName) {
  if (ModeNameToIdH.IsKey(ModeName)) {
    return -1;
  }
  TInt ModeId = TInt(MxModeId);
  MxModeId++;
  ModeIdToNameH.AddDat(ModeId, ModeName);
  ModeNameToIdH.AddDat(ModeName, ModeId);

  TModeNet NewGraph(ModeId);
  NewGraph.SetParentPointer(this);
  TModeNetH.AddDat(ModeId, NewGraph);
  return ModeId;
}

int TMMNet::AddCrossNet(const TStr& ModeName1, const TStr& ModeName2, const TStr& CrossNetName, bool isDir) {
  TInt ModeId1 = GetModeId(ModeName1);
  TInt ModeId2 = GetModeId(ModeName2);
  return AddCrossNet(ModeId1, ModeId2, CrossNetName, isDir);
}

int TMMNet::AddCrossNet(const TInt& ModeId1, const TInt& ModeId2, const TStr& CrossNetName, bool isDir) {
  if (CrossNameToIdH.IsKey(CrossNetName)) {
    return -1;
  }
  TInt CrossNetId = TInt(MxCrossNetId);
  MxCrossNetId++;
  CrossIdToNameH.AddDat(CrossNetId, CrossNetName);
  CrossNameToIdH.AddDat(CrossNetName, CrossNetId);

  TCrossNet Cross = TCrossNet(ModeId1, ModeId2, isDir, CrossNetId);
  Cross.SetParentPointer(this);
  TCrossNetH.AddDat(CrossNetId, Cross);

  TModeNetH.GetDat(ModeId1).AddNbrType(CrossNetName, ModeId1==ModeId2, isDir);
  TModeNetH.GetDat(ModeId2).AddNbrType(CrossNetName, ModeId1==ModeId2, isDir);

  return CrossNetId;
}

int TMMNet::DelCrossNet(const TInt& CrossNetId) {
  return DelCrossNet(CrossIdToNameH.GetDat(CrossNetId));
}

int TMMNet::DelCrossNet(const TStr& CrossNet) {
  IAssertR(CrossNameToIdH.IsKey(CrossNet),TStr::Fmt("No such link type: %s", CrossNet.CStr()));
  TInt CrossNetId = CrossNameToIdH.GetDat(CrossNet);
  TInt Mode1 = GetCrossNetById(CrossNetId).Mode1;
  TInt Mode2 = GetCrossNetById(CrossNetId).Mode2;
  if (GetModeNetById(Mode1).DelNbrType(CrossNet) == -1 || (Mode1 != Mode2 && GetModeNetById(Mode2).DelNbrType(CrossNet) == -1)) {
    return -1;
  }
  CrossNameToIdH.DelKey(CrossNet);
  CrossIdToNameH.DelKey(CrossNetId);
  GetCrossNetById(CrossNetId).SetParentPointer(NULL);
  TCrossNetH.DelKey(CrossNetId);
  return 0;
}

int TMMNet::DelModeNet(const TInt& ModeId) {
  TStrV CrossNets;
  GetModeNetById(ModeId).GetCrossNetNames(CrossNets);
  for (int i = 0; i < CrossNets.Len(); i++) {
    if (DelCrossNet(CrossNets[i]) == -1) {
      return -1;
    }
  }
  TStr ModeName = ModeIdToNameH.GetDat(ModeId);
  ModeNameToIdH.DelKey(ModeName);
  ModeIdToNameH.DelKey(ModeId);
  GetModeNetById(ModeId).SetParentPointer(NULL);
  TModeNetH.DelKey(ModeId);
  return 0;
}

int TMMNet::DelModeNet(const TStr& ModeName) {
  IAssertR(ModeNameToIdH.IsKey(ModeName), TStr::Fmt("No such mode with name: %s", ModeName.CStr()));
  return DelModeNet(ModeNameToIdH.GetDat(ModeName));
}

TModeNet& TMMNet::GetModeNetByName(const TStr& ModeName) const {
  //IAssertR(ModeNameToIdH.IsKey(ModeName),TStr::Fmt("No such mode name: %s", ModeName.CStr()));
  return GetModeNetById(ModeNameToIdH.GetDat(ModeName));
}

TModeNet& TMMNet::GetModeNetById(const TInt& ModeId) const {
//  IAssertR(ModeId < TModeNetH.Len(), TStr::Fmt("Mode with id %d does not exist", ModeId));
  TModeNet &Net = (const_cast<TMMNet *>(this))->TModeNetH.GetDat(ModeId);
  return Net;
}
TCrossNet& TMMNet::GetCrossNetByName(const TStr& CrossName) const{
  //IAssertR(CrossNameToIdH.IsKey(CrossName),TStr::Fmt("No such link name: %s", CrossName.CStr()));
  return GetCrossNetById(CrossNameToIdH.GetDat(CrossName));
}
TCrossNet& TMMNet::GetCrossNetById(const TInt& CrossId) const{
  //IAssertR(CrossIdToNameH.IsKey(CrossId),TStr::Fmt("No link with id %d exists", CrossId));
  TCrossNet& CrossNet = (const_cast<TMMNet *>(this))->TCrossNetH.GetDat(CrossId);
  return CrossNet;
}

int TMMNet::AddMode(const TStr& ModeName, const TInt& ModeId, const TModeNet& ModeNet) {
  ModeIdToNameH.AddDat(ModeId, ModeName);
  ModeNameToIdH.AddDat(ModeName, ModeId);

  TModeNetH.AddDat(ModeId, ModeNet);
  TModeNetH[ModeId].SetParentPointer(this);
  return ModeId;

}
int TMMNet::AddCrossNet(const TStr& CrossNetName, const TInt& CrossNetId, const TCrossNet& CrossNet) {
  CrossIdToNameH.AddDat(CrossNetId, CrossNetName);
  CrossNameToIdH.AddDat(CrossNetName, CrossNetId);

  TCrossNetH.AddDat(CrossNetId, CrossNet);
  TCrossNetH[CrossNetId].SetParentPointer(this);
  return CrossNetId;
}

void TMMNet::ClrNbr(const TInt& ModeId, const TInt& CrossNetId, const bool& outEdge, const bool& sameMode, bool& isDir) {
  TStr CrossNetName = CrossIdToNameH[CrossNetId];
  TModeNetH[ModeId].ClrNbr(CrossNetName, outEdge, sameMode, isDir);
}

PMMNet TMMNet::GetSubgraphByCrossNet(TStrV& CrossNetTypes) {
  PMMNet Result = New();
  TInt MxMode = 0;
  TInt MxCross = 0;
  TIntH ModeH;
  for(int i = 0; i < CrossNetTypes.Len(); i++) {
    TStr CrossName = CrossNetTypes[i];
    TInt OldId = CrossNameToIdH.GetDat(CrossName);
    TInt NewId = MxCross++;
    TCrossNet NewCrossNet(TCrossNetH.GetDat(OldId));
    TInt OldModeId1 = NewCrossNet.Mode1;
    TInt OldModeId2 = NewCrossNet.Mode2;
    TInt NewModeId1, NewModeId2;
    if (ModeH.IsKey(OldModeId1)) {
      NewModeId1 = ModeH.GetDat(OldModeId1);
    } else {
      NewModeId1 = MxMode++;
      ModeH.AddDat(OldModeId1, NewModeId1);
    }
    if (ModeH.IsKey(OldModeId2)) {
      NewModeId2 = ModeH.GetDat(OldModeId2);
    } else {
      NewModeId2 = MxMode++;
      ModeH.AddDat(OldModeId2, NewModeId2);
    }
    NewCrossNet.Mode1 = NewModeId1;
    NewCrossNet.Mode2 = NewModeId2;
    NewCrossNet.CrossNetId = NewId;
    Result->AddCrossNet(CrossName, NewId, NewCrossNet);
  }
  for(TIntH::TIter it = ModeH.BegI(); it < ModeH.EndI(); it++) {
    TStr ModeName = ModeIdToNameH.GetDat(it.GetKey());
    TInt NewModeId = it.GetDat();
    TModeNet NewModeNet;
    TModeNetH.GetDat(it.GetKey()).RemoveCrossNets(NewModeNet, CrossNetTypes);
    NewModeNet.ModeId = NewModeId;
    Result->AddMode(ModeName, NewModeId, NewModeNet);
  }
  Result->MxModeId = MxMode;
  Result->MxCrossNetId = MxCross;
  return Result;
}

PMMNet TMMNet::GetSubgraphByModeNet(TStrV& ModeNetTypes) {
  THash<TInt, TBool> ModeTypeIds;
  for (int i = 0; i < ModeNetTypes.Len(); i++) {
    ModeTypeIds.AddDat(ModeNameToIdH.GetDat(ModeNetTypes[i]), true);
  }
  TStrV CrossNetTypes;
  for (THash<TInt, TCrossNet>::TIter it = TCrossNetH.BegI(); it < TCrossNetH.EndI(); it++) {
    TCrossNet& CrossNet = it.GetDat();
    if (ModeTypeIds.IsKey(CrossNet.Mode1) && ModeTypeIds.IsKey(CrossNet.Mode2)) {
      CrossNetTypes.Add(CrossIdToNameH.GetDat(it.GetKey()));
      ModeTypeIds[CrossNet.Mode1] = false;
      ModeTypeIds[CrossNet.Mode2] = false;
    }
  }

  PMMNet Result = GetSubgraphByCrossNet(CrossNetTypes);
  TInt MxMode = Result->MxModeId;
  TStrV EmptyCrossNetTypes;
  for (THash<TInt, TBool>::TIter it = ModeTypeIds.BegI(); it < ModeTypeIds.EndI(); it++) {
    if (it.GetDat().Val) {
      TStr ModeName = ModeIdToNameH.GetDat(it.GetKey());
      TInt NewModeId = MxMode++;
      TModeNet NewModeNet;
      TModeNetH.GetDat(it.GetKey()).RemoveCrossNets(NewModeNet, EmptyCrossNetTypes);
      NewModeNet.ModeId = NewModeId;
      Result->AddMode(ModeName, NewModeId, NewModeNet);
    }
  }
  Result->MxModeId = MxMode;
  return Result;
}

PNEANet TMMNet::ToNetwork(TIntV& CrossNetTypes, TIntStrStrTrV& NodeAttrMap, TVec<TTriple<TInt, TStr, TStr> >& EdgeAttrMap) {
  TIntPrIntH NodeMap;
  THash<TIntPr, TIntPr> EdgeMap;
  THashSet<TInt> Modes;
  PNEANet NewNet = TNEANet::New();
  //Add nodes and edges
  for (int i = 0; i < CrossNetTypes.Len(); i++) {
    TCrossNet& CrossNet = GetCrossNetById(CrossNetTypes[i]);
    TInt Mode1 = CrossNet.GetMode1();
    TInt Mode2 = CrossNet.GetMode2();
    Modes.AddKey(Mode1);
    Modes.AddKey(Mode2);
    bool isDirected = CrossNet.IsDirected();
    for(TCrossNet::TCrossEdgeI EdgeI = CrossNet.BegEdgeI(); EdgeI != CrossNet.EndEdgeI(); EdgeI++) {
      int srcNode = EdgeI.GetSrcNId();
      int dstNode = EdgeI.GetDstNId();
      TIntPr SrcNodeMapping(Mode1, srcNode);
      int srcId = 0;
      if (NodeMap.IsKey(SrcNodeMapping)) {
        srcId = NodeMap.GetDat(SrcNodeMapping);
      } else {
        srcId = NewNet->AddNode();
        NodeMap.AddDat(SrcNodeMapping, srcId);
      }
      TIntPr DstNodeMapping(Mode2, dstNode);
      int dstId = 0;
      if (NodeMap.IsKey(DstNodeMapping)) {
        dstId = NodeMap.GetDat(DstNodeMapping);
      } else {
        dstId = NewNet->AddNode();
        NodeMap.AddDat(DstNodeMapping, dstId);
      }
      int edgeId = EdgeI.GetId();
      TIntPr EdgeMapping(CrossNetTypes[i], edgeId);
      int newEId = NewNet->AddEdge(srcId, dstId);
      int otherEId = -1;
      if (!isDirected) {
        otherEId = NewNet->AddEdge(dstId, srcId);
      }
      EdgeMap.AddDat(EdgeMapping, TIntPr(newEId, otherEId));
    }
  }

  for (THashSet<TInt>::TIter it = Modes.BegI(); it != Modes.EndI(); it++) {
    TModeNet &ModeNet = GetModeNetById(it.GetKey());
    TInt ModeId = it.GetKey();
    for(TModeNet::TNodeI NodeIt = ModeNet.BegMMNI(); NodeIt != ModeNet.EndMMNI(); NodeIt++) {
      TIntPr NodeKey(ModeId, NodeIt.GetId());
      if (!NodeMap.IsKey(NodeKey)) {
        int newId = NewNet->AddNode();
        NodeMap.AddDat(NodeKey, newId);
      }
    }
  }

  //Add attributes
  NewNet->AddIntAttrN(TStr("Mode"));
  NewNet->AddIntAttrN(TStr("Id"));
  NewNet->AddIntAttrE(TStr("CrossNet"));
  NewNet->AddIntAttrE(TStr("Id"));
  for(TIntPrIntH::TIter it = NodeMap.BegI(); it != NodeMap.EndI(); it++) {
    NewNet->AddIntAttrDatN(it.GetDat(), it.GetKey().GetVal1(), TStr("Mode"));
    NewNet->AddIntAttrDatN(it.GetDat(), it.GetKey().GetVal2(), TStr("Id"));
  }
  for(THash<TIntPr, TIntPr>::TIter it = EdgeMap.BegI(); it != EdgeMap.EndI(); it++) {
    NewNet->AddIntAttrDatE(it.GetDat().GetVal1(), it.GetKey().GetVal1(), TStr("CrossNet"));
    NewNet->AddIntAttrDatE(it.GetDat().GetVal1(), it.GetKey().GetVal2(), TStr("Id"));
    if (it.GetDat().GetVal2() != -1) {
      NewNet->AddIntAttrDatE(it.GetDat().GetVal2(), it.GetKey().GetVal1(), TStr("CrossNet"));
      NewNet->AddIntAttrDatE(it.GetDat().GetVal2(), it.GetKey().GetVal2(), TStr("Id"));
    }
  }

  for (int i = 0; i < NodeAttrMap.Len(); i++) {
    //mode, orig attr, new attr
    TInt ModeId = NodeAttrMap[i].Val1;
    TStr OrigAttr = NodeAttrMap[i].Val2;
    TStr NewAttr = NodeAttrMap[i].Val3;
    TModeNet& Net = GetModeNetById(ModeId);
    int type = Net.GetAttrTypeN(OrigAttr);
    if (type == TModeNet::IntType) {
      NewNet->AddIntAttrN(NewAttr, Net.GetIntAttrDefaultN(OrigAttr));
      for(TModeNet::TNodeI it = Net.BegMMNI(); it != Net.EndMMNI(); it++) {
        TIntPr OldNId(ModeId, it.GetId());
        int NewId = NodeMap.GetDat(OldNId);
        int Val = Net.GetIntAttrDatN(it.GetId(), OrigAttr);
        NewNet->AddIntAttrDatN(NewId, Val, NewAttr);
      }
    } else if (type == TModeNet::FltType) {
      NewNet->AddFltAttrN(NewAttr, Net.GetFltAttrDefaultN(OrigAttr));
      for(TModeNet::TNodeI it = Net.BegMMNI(); it != Net.EndMMNI(); it++) {
        TIntPr OldNId(ModeId, it.GetId());
        int NewId = NodeMap.GetDat(OldNId);
        TFlt Val = Net.GetFltAttrDatN(it.GetId(), OrigAttr);
        NewNet->AddFltAttrDatN(NewId, Val, NewAttr);
      }

    } else if (type == TModeNet::StrType) {
      NewNet->AddStrAttrN(NewAttr, Net.GetStrAttrDefaultN(OrigAttr));
      for(TModeNet::TNodeI it = Net.BegMMNI(); it != Net.EndMMNI(); it++) {
        TIntPr OldNId(ModeId, it.GetId());
        int NewId = NodeMap.GetDat(OldNId);
        TStr Val = Net.GetStrAttrDatN(it.GetId(), OrigAttr);
        NewNet->AddStrAttrDatN(NewId, Val, NewAttr);
      }
    } else if (type == TModeNet::IntVType) {
      NewNet->AddIntVAttrN(NewAttr);
      for(TModeNet::TNodeI it = Net.BegMMNI(); it != Net.EndMMNI(); it++) {
        TIntPr OldNId(ModeId, it.GetId());
        int NewId = NodeMap.GetDat(OldNId);
        TIntV Val = Net.GetIntVAttrDatN(it.GetId(), OrigAttr);
        NewNet->AddIntVAttrDatN(NewId, Val, NewAttr);
      }
    }
  }

  for (int i = 0; i < EdgeAttrMap.Len(); i++) {
    //mode, orig attr, new attr
    TInt CrossId = EdgeAttrMap[i].Val1;
    TStr OrigAttr = EdgeAttrMap[i].Val2;
    TStr NewAttr = EdgeAttrMap[i].Val3;
    TCrossNet& Net = GetCrossNetById(CrossId);
    int type = Net.GetAttrTypeE(OrigAttr);
    if (type == TCrossNet::IntType) {
      NewNet->AddIntAttrE(NewAttr, Net.GetIntAttrDefaultE(OrigAttr));
      for(TCrossNet::TCrossEdgeI it = Net.BegEdgeI(); it != Net.EndEdgeI(); it++) {
        TIntPr OldNId(CrossId, it.GetId());
        TIntPr NewId = EdgeMap.GetDat(OldNId);
        int Val = Net.GetIntAttrDatE(it.GetId(), OrigAttr);
        NewNet->AddIntAttrDatE(NewId.Val1, Val, NewAttr);
        if (NewId.Val2 != -1) {
          NewNet->AddIntAttrDatE(NewId.Val2, Val, NewAttr);
        }
      }
    } else if (type == TCrossNet::FltType) {
      NewNet->AddFltAttrE(NewAttr, Net.GetFltAttrDefaultE(OrigAttr));
      for(TCrossNet::TCrossEdgeI it = Net.BegEdgeI(); it != Net.EndEdgeI(); it++) {
        TIntPr OldNId(CrossId, it.GetId());
        TIntPr NewId = EdgeMap.GetDat(OldNId);
        TFlt Val = Net.GetFltAttrDatE(it.GetId(), OrigAttr);
        NewNet->AddFltAttrDatE(NewId.Val1, Val, NewAttr);
        if (NewId.Val2 != -1) {
          NewNet->AddFltAttrDatE(NewId.Val2, Val, NewAttr);
        }
      }

    } else if (type == TCrossNet::StrType) {
      NewNet->AddStrAttrE(NewAttr, Net.GetStrAttrDefaultE(OrigAttr));
      for(TCrossNet::TCrossEdgeI it = Net.BegEdgeI(); it != Net.EndEdgeI(); it++){
        TIntPr OldNId(CrossId, it.GetId());
        TIntPr NewId = EdgeMap.GetDat(OldNId);
        TStr Val = Net.GetStrAttrDatE(it.GetId(), OrigAttr);
        NewNet->AddStrAttrDatE(NewId.Val1, Val, NewAttr);
        if (NewId.Val2 != -1) {
          NewNet->AddStrAttrDatE(NewId.Val2, Val, NewAttr);
        }
      }
    }
  }
  return NewNet;
}

PNEANet TMMNet::ToNetwork2(TIntV& CrossNetTypes, TIntStrPrVH& NodeAttrMap, THash<TInt, TVec<TPair<TStr, TStr> > >& EdgeAttrMap) {
  TIntPrIntH NodeMap;
  THashSet<TInt> Modes;
  PNEANet NewNet = TNEANet::New();
  NewNet->AddIntAttrN(TStr("Mode"));
  NewNet->AddIntAttrN(TStr("Id"));
  NewNet->AddIntAttrE(TStr("CrossNet"));
  NewNet->AddIntAttrE(TStr("Id"));
  
  //Add nodes and edges
  for (int i = 0; i < CrossNetTypes.Len(); i++) {
    TCrossNet& CrossNet = GetCrossNetById(CrossNetTypes[i]);
    TStrPrV CNetAttrs;
    if (EdgeAttrMap.IsKey(CrossNetTypes[i])) {
      CNetAttrs = EdgeAttrMap.GetDat(CrossNetTypes[i]);
    }
    TInt Mode1 = CrossNet.GetMode1();
    TInt Mode2 = CrossNet.GetMode2();
    TModeNet& Mode1Net = GetModeNetById(Mode1);
    TModeNet& Mode2Net = GetModeNetById(Mode2);
    TStrPrV Mode1Attrs;
    if (NodeAttrMap.IsKey(Mode1)) {
      Mode1Attrs = NodeAttrMap.GetDat(Mode1);
    }
    TStrPrV Mode2Attrs;
    if (NodeAttrMap.IsKey(Mode2)) {
      Mode2Attrs = NodeAttrMap.GetDat(Mode2);
    } 
    Modes.AddKey(Mode1);
    Modes.AddKey(Mode2);
    bool isDirected = CrossNet.IsDirected();
    for(TCrossNet::TCrossEdgeI EdgeI = CrossNet.BegEdgeI(); EdgeI != CrossNet.EndEdgeI(); EdgeI++) {
      int srcNode = EdgeI.GetSrcNId();
      int dstNode = EdgeI.GetDstNId();
      TIntPr SrcNodeMapping(Mode1, srcNode);
      int srcId = 0;
      if (NodeMap.IsKey(SrcNodeMapping)) {
        srcId = NodeMap.GetDat(SrcNodeMapping);
      } else {
        srcId = NewNet->AddNode();
        NodeMap.AddDat(SrcNodeMapping, srcId);
        NewNet->AddIntAttrDatN(srcId, srcNode, TStr("Id"));
        NewNet->AddIntAttrDatN(srcId, Mode1, TStr("Mode"));
        AddNodeAttributes(NewNet, Mode1Net, Mode1Attrs, Mode1, srcNode, srcId);
      }
      TIntPr DstNodeMapping(Mode2, dstNode);
      int dstId = 0;
      if (NodeMap.IsKey(DstNodeMapping)) {
        dstId = NodeMap.GetDat(DstNodeMapping);
      } else {
        dstId = NewNet->AddNode();
        NodeMap.AddDat(DstNodeMapping, dstId);
        NewNet->AddIntAttrDatN(dstId, dstNode, TStr("Id"));
        NewNet->AddIntAttrDatN(dstId, Mode2, TStr("Mode"));
        AddNodeAttributes(NewNet, Mode2Net, Mode2Attrs, Mode2, dstNode, dstId);
      }
      int edgeId = EdgeI.GetId();
      int newEId = NewNet->AddEdge(srcId, dstId);
      NewNet->AddIntAttrDatE(newEId, edgeId, TStr("Id"));
      NewNet->AddIntAttrDatE(newEId, CrossNetTypes[i], TStr("CrossNet"));
      AddEdgeAttributes(NewNet, CrossNet, CNetAttrs, CrossNetTypes[i], edgeId, newEId);
      if (!isDirected) {
        int otherEId = NewNet->AddEdge(dstId, srcId);
        NewNet->AddIntAttrDatE(otherEId, edgeId, TStr("Id"));
        NewNet->AddIntAttrDatE(otherEId, CrossNetTypes[i], TStr("CrossNet"));
        AddEdgeAttributes(NewNet, CrossNet, CNetAttrs, CrossNetTypes[i], edgeId, otherEId);
      }
    }
  }

  for (THashSet<TInt>::TIter it = Modes.BegI(); it != Modes.EndI(); it++) {
    TInt ModeId = it.GetKey();
    TModeNet &ModeNet = GetModeNetById(ModeId);
    TStrPrV ModeAttrs = NodeAttrMap.GetDat(ModeId);
    for(TModeNet::TNodeI NodeIt = ModeNet.BegMMNI(); NodeIt != ModeNet.EndMMNI(); NodeIt++) {
      TIntPr NodeKey(ModeId, NodeIt.GetId());
      if (!NodeMap.IsKey(NodeKey)) {
        int newId = NewNet->AddNode();
        NodeMap.AddDat(NodeKey, newId);
        AddNodeAttributes(NewNet, ModeNet, ModeAttrs, ModeId, NodeIt.GetId(), newId);
      }
    }
  }

  return NewNet;
}

void TMMNet::GetPartitionRanges(TIntPrV& Partitions, const TInt& NumPartitions, const TInt& MxLen) const {

  TInt PartitionSize = MxLen/NumPartitions;
  TInt CurrStart = 0;
  bool done = false;
  while (!done) {
    TInt CurrEnd = CurrStart + PartitionSize;
    if (MxLen - CurrEnd < PartitionSize) {
      CurrEnd = MxLen;
      done = true;
    }
    Partitions.Add(TIntPr(CurrStart, CurrEnd));
    CurrStart = CurrEnd;
  }
}

#ifdef GCC_ATOMIC

PNEANetMP TMMNet::ToNetworkMP(TStrV& CrossNetNames) {
 
  TStrIntH CrossNetStart;
  THashSet<TInt> ModeSet;
  int offset = 0;
  int NumEdges = 0;
  for (int i=0; i < CrossNetNames.Len(); i++) {
    CrossNetStart.AddDat(CrossNetNames[i], offset);
    TCrossNet& CrossNet = GetCrossNetByName(CrossNetNames[i]);
    int factor = CrossNet.IsDirected() ? 1 : 2;
    offset += (CrossNet.GetMxEId() * factor);
    NumEdges += (CrossNet.GetEdges() * factor);
    ModeSet.AddKey(CrossNet.GetMode1());
    ModeSet.AddKey(CrossNet.GetMode2());
  }
  int MxEId = offset;

  int NumNodes = 0;
  for (THashSet<TInt>::TIter MI = ModeSet.BegI(); MI < ModeSet.EndI(); MI++) {
    TModeNet& ModeNet = GetModeNetById(MI.GetKey());
    NumNodes += ModeNet.GetNodes();
  }

  THashMP<TIntPr, TInt> NodeMap(NumNodes);
  THashMP<TIntPr, TIntPr> EdgeMap(NumEdges);
  PNEANetMP NewNet = TNEANetMP::New(NumNodes, NumEdges);

  int num_threads = omp_get_max_threads();
  offset = 0;
  for (THashSet<TInt>::TIter MI = ModeSet.BegI(); MI < ModeSet.EndI(); MI++) {
    TInt ModeId = MI.GetKey();
    TModeNet& ModeNet = GetModeNetById(ModeId);
    TIntV KeyIds;
    ModeNet.NodeH.GetKeyV(KeyIds);

    TIntPrV NodePartitions;
    GetPartitionRanges(NodePartitions, num_threads, KeyIds.Len());

    int curr_nid;
    #pragma omp parallel for schedule(static) private(curr_nid)
    for (int i = 0; i < NodePartitions.Len(); i++) {
      TInt CurrStart = NodePartitions[i].GetVal1();
      TInt CurrEnd = NodePartitions[i].GetVal2();
      curr_nid = offset + CurrStart;
      for (int idx = CurrStart; idx < CurrEnd ; idx++) {
        int n_i = KeyIds[idx];
        if (ModeNet.IsNode(n_i)) {
          //Collect neighbors
          TIntV InNbrs;
          TIntV OutNbrs;
          for (int j=0; j < CrossNetNames.Len(); j++) {
            if (ModeNet.NeighborTypes.IsKey(CrossNetNames[j])) {
              if (ModeNet.NeighborTypes.GetDat(CrossNetNames[j])) {
                
                TIntV Neighbors;
                ModeNet.GetNeighborsByCrossNet(n_i, CrossNetNames[j], Neighbors);
                int edge_offset = CrossNetStart.GetDat(CrossNetNames[j]);
                TCrossNet CrossNet = GetCrossNetByName(CrossNetNames[j]);
                bool isDir = CrossNet.IsDirected();
                bool isOutNbr = CrossNet.GetMode1() == ModeId;
                int factor = isDir ? 1 : 2;

                int id_offset = isDir || isOutNbr ? 0 : 1;
                if (!isDir && CrossNet.GetMode1() == CrossNet.GetMode2()) {
                  id_offset = n_i == CrossNet.GetEdge(n_i).GetSrcNId() ? 0 : 1;
                }

                for (int k = 0; k < Neighbors.Len(); k++) {
                  if (isOutNbr && id_offset == 0) {
                    OutNbrs.Add(edge_offset + Neighbors[k]*factor + id_offset);
                  } else {
                    InNbrs.Add(edge_offset + Neighbors[k]*factor + id_offset);
                  }
                  if (!isDir) {
                    int opp_offset = id_offset == 1 ? 0 : 1;
                    if (isOutNbr && id_offset == 0) {
                      InNbrs.Add(edge_offset + Neighbors[k]*factor + opp_offset);
                    } else {
                      OutNbrs.Add(edge_offset + Neighbors[k]*factor + opp_offset);
                    }
                  }
                }
              } else {
                TIntV TempOut;
                ModeNet.GetNeighborsByCrossNet(n_i, CrossNetNames[j], TempOut, true);
                OutNbrs.AddV(TempOut);
                TIntV TempIn;
                ModeNet.GetNeighborsByCrossNet(n_i, CrossNetNames[j], TempIn, false);
                InNbrs.AddV(TempIn);
              }
            }
          }

          NewNet->AddNodeWithEdges(curr_nid, InNbrs, OutNbrs);
          TIntPr NodeKey(MI.GetKey(), n_i);
          NodeMap.AddDat(NodeKey, curr_nid);
          curr_nid++;
        }
      }
    }
    offset += KeyIds.Len();
  }
  NewNet->SetNodes(offset);

  for (int j=0; j < CrossNetNames.Len(); j++) {
    TStr CrossNetName = CrossNetNames[j];
    TCrossNet& CrossNet = GetCrossNetByName(CrossNetName);
    TInt CrossNetId = GetCrossId(CrossNetName);
    TInt Mode1 = CrossNet.GetMode1();
    TInt Mode2 = CrossNet.GetMode2();

    TIntPrV EdgePartitions;
    GetPartitionRanges(EdgePartitions, num_threads, CrossNet.MxEId);
    int curr_eid;
    offset = CrossNetStart.GetDat(CrossNetNames[j]);
    int factor = CrossNet.IsDirected() ? 1 : 2;
    #pragma omp parallel for schedule(static) private(curr_eid)
    for (int i = 0; i < EdgePartitions.Len(); i++) {
      TInt CurrStart = EdgePartitions[i].GetVal1();
      TInt CurrEnd = EdgePartitions[i].GetVal2();
      for (int e_i = CurrStart; e_i < CurrEnd ; e_i++) {
        curr_eid = offset + factor*e_i;
        if (CrossNet.IsEdge(e_i)) {
          int new_eid = curr_eid;
          TIntPr EdgeKey(CrossNetId, e_i);
          TCrossNet::TCrossEdgeI edge = CrossNet.GetEdgeI(e_i);
          int srcNode = edge.GetSrcNId();
          int dstNode = edge.GetDstNId();
          TIntPr NodeKeySrc(Mode1, srcNode);
          TIntPr NodeKeyDst(Mode2, dstNode);
          int newSrc = NodeMap.GetDat(NodeKeySrc);
          int newDst = NodeMap.GetDat(NodeKeyDst);
          NewNet->AddEdgeUnchecked(curr_eid, newSrc, newDst);
          curr_eid++;
          int otherEId = -1;
          if (!CrossNet.IsDirected()) {
            otherEId = curr_eid;
            NewNet->AddEdgeUnchecked(otherEId, newDst, newSrc);
          }
          EdgeMap.AddDat(EdgeKey, TIntPr(new_eid, otherEId));
        }
      }
    }
  }
  NewNet->SetEdges(MxEId);
  NewNet->ReserveAttr(2, 0, 0, 2, 0, 0);

  //Add attributes
  NewNet->AddIntAttrN(TStr("Mode"));
  NewNet->AddIntAttrN(TStr("Id"));
  NewNet->AddIntAttrE(TStr("CrossNet"));
  NewNet->AddIntAttrE(TStr("Id"));

  TIntPrV NewNodeIds;
  NodeMap.GetKeyV(NewNodeIds);

  #pragma omp parallel for schedule(static)
  for(int i = 0; i < NewNodeIds.Len(); i++) {
    NewNet->AddIntAttrDatN(NodeMap.GetDat(NewNodeIds[i]), NewNodeIds[i].GetVal1(), TStr("Mode"));
    NewNet->AddIntAttrDatN(NodeMap.GetDat(NewNodeIds[i]), NewNodeIds[i].GetVal2(), TStr("Id"));
  }


  TIntPrV NewEdgeIds;
  EdgeMap.GetKeyV(NewEdgeIds);
  #pragma omp parallel for schedule(static)
  for(int i = 0; i < NewEdgeIds.Len(); i++) {
    NewNet->AddIntAttrDatE(EdgeMap.GetDat(NewEdgeIds[i]).GetVal1(), NewEdgeIds[i].GetVal2(), TStr("Id"));
    NewNet->AddIntAttrDatE(EdgeMap.GetDat(NewEdgeIds[i]).GetVal1(), NewEdgeIds[i].GetVal1(), TStr("CrossNet"));
    if (EdgeMap.GetDat(NewEdgeIds[i]).GetVal2() != -1) {
      NewNet->AddIntAttrDatE(EdgeMap.GetDat(NewEdgeIds[i]).GetVal2(), NewEdgeIds[i].GetVal1(), TStr("CrossNet"));
      NewNet->AddIntAttrDatE(EdgeMap.GetDat(NewEdgeIds[i]).GetVal2(), NewEdgeIds[i].GetVal2(), TStr("Id"));
    }
  }
  return NewNet;
}

#endif // GCC_ATOMIC

int TMMNet::AddNodeAttributes(PNEANet& NewNet, TModeNet& Net, TVec<TPair<TStr, TStr> >& Attrs, int ModeId, int oldId, int NId) {
  for (int i = 0; i < Attrs.Len(); i++) {
    //mode, orig attr, new attr
    TStr OrigAttr = Attrs[i].Val1;
    TStr NewAttr = Attrs[i].Val2;
    int type = Net.GetAttrTypeN(OrigAttr);
    if (type == TModeNet::IntType) {
      TIntPr OldNId(ModeId, oldId);
      TInt Val = Net.GetIntAttrDatN(oldId, OrigAttr);
      NewNet->AddIntAttrDatN(NId, Val, NewAttr);
    } else if (type == TModeNet::FltType) {
      TIntPr OldNId(ModeId, oldId);
      TFlt Val = Net.GetFltAttrDatN(oldId, OrigAttr);
      NewNet->AddFltAttrDatN(NId, Val, NewAttr);
    } else if (type == TModeNet::StrType) {
      TIntPr OldNId(ModeId, oldId);
      TStr Val = Net.GetStrAttrDatN(oldId, OrigAttr);
      NewNet->AddStrAttrDatN(NId, Val, NewAttr);
    } else if (type == TModeNet::IntVType) {
      TIntPr OldNId(ModeId, oldId);
      TIntV Val = Net.GetIntVAttrDatN(oldId, OrigAttr);
      NewNet->AddIntVAttrDatN(NId, Val, NewAttr);
    }
  }
  return 0;
}

int TMMNet::AddEdgeAttributes(PNEANet& NewNet, TCrossNet& Net, TVec<TPair<TStr, TStr> >& Attrs, int CrossId, int oldId, int EId) {
  for (int i = 0; i < Attrs.Len(); i++) {
    //mode, orig attr, new attr
    TStr OrigAttr = Attrs[i].Val1;
    TStr NewAttr = Attrs[i].Val2;
    int type = Net.GetAttrTypeE(OrigAttr);
    if (type == TCrossNet::IntType) {
      TIntPr OldNId(CrossId, oldId);
      TInt Val = Net.GetIntAttrDatE(oldId, OrigAttr);
      NewNet->AddIntAttrDatE(EId, Val, NewAttr);
    } else if (type == TCrossNet::FltType) {
      TIntPr OldNId(CrossId, oldId);
      TFlt Val = Net.GetFltAttrDatE(oldId, OrigAttr);
      NewNet->AddFltAttrDatE(EId, Val, NewAttr);
    } else if (type == TCrossNet::StrType) {
      TIntPr OldNId(CrossId, oldId);
      TStr Val = Net.GetStrAttrDatE(oldId, OrigAttr);
      NewNet->AddStrAttrDatE(EId, Val, NewAttr);
    }
  }
  return 0;
}
