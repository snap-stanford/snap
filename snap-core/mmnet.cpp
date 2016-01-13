////////////////////////////////////////////////
// Mutimodal Network

//TODO Sheila-check: I've removed the ModeId argument here. Is it really necessary?
// Sheila-response: with the attribute code, we used the return value as a way to signal if there are errors
// and made the real return values (i.e. the mode id, etc) pass by reference variables. I was just following
// that standard.
int TMMNet::AddMode(const TStr& ModeName) {
  //Book-keeping for new mode id and the hash lookups
  TInt ModeId = TInt(MxModeId);
  MxModeId++;
  ModeIdToNameH.AddDat(ModeId, ModeName);
  ModeNameToIdH.AddDat(ModeName, ModeId);

  TModeNet NewGraph(ModeId);
  NewGraph.SetParentPointer(this);
  TModeNetH.AddDat(ModeId, NewGraph);
  return ModeId;
}

int TMMNet::AddLinkType(const TStr& ModeName1, const TStr& ModeName2, const TStr& LinkTypeName) {
  //IAssertR(ModeNameToIdH.IsKey(ModeName1), TStr::Fmt("No such mode name: %s", ModeName1.CStr()));
  //IAssertR(ModeNameToIdH.IsKey(ModeName2), TStr::Fmt("No such mode name: %s", ModeName2.CStr()));

  TInt ModeId1 = GetModeId(ModeName1);
  TInt ModeId2 = GetModeId(ModeName2);
  return AddLinkType(ModeId1, ModeId2, LinkTypeName);
}


int TMMNet::AddLinkType(const TInt& ModeId1, const TInt& ModeId2, const TStr& LinkTypeName) {
  //IAssertR(ModeIdToNameH.IsKey(ModeId1), TStr::Fmt("No mode with id %d exists", ModeId1));
  //IAssertR(ModeIdToNameH.IsKey(ModeId2), TStr::Fmt("No mode with id %d exists", ModeId2));


  TInt LinkTypeId = TInt(MxLinkTypeId);
  MxLinkTypeId++;
  LinkIdToNameH.AddDat(LinkTypeId, LinkTypeName);
  LinkNameToIdH.AddDat(LinkTypeName, LinkTypeId);
  TCrossNet Link = TCrossNet(ModeId1, ModeId2, LinkTypeId);
  Link.SetParentPointer(this);
  TCrossNetH.AddDat(LinkTypeId, Link);

  TModeNetH.GetDat(ModeId1).AddNbrType(LinkTypeName, ModeId1==ModeId2, true); //can't assume it is directed
  TModeNetH.GetDat(ModeId2).AddNbrType(LinkTypeName, ModeId1==ModeId2, true);
  return LinkTypeId;
}

int TMMNet::DelLinkType(const TInt& LinkTypeId) {
  //TODO: delete all neighbors from the modes involved in the link type
  return -1;
}

int TMMNet::DelLinkType(const TStr& LinkType) {
  IAssertR(LinkNameToIdH.IsKey(LinkType),TStr::Fmt("No such link type: %s", LinkType.CStr()));
  return DelLinkType(LinkNameToIdH.GetDat(LinkType));
}

int TMMNet::DelMode(const TInt& ModeId) {
  //TODO: figure out what to put in the vector TModeNetH when the mode is deleted
  //TODO: delete all edges in the mode
  return -1;
}
int TMMNet::DelMode(const TStr& ModeName) {
  IAssertR(ModeNameToIdH.IsKey(ModeName), TStr::Fmt("No such mode with name: %s", ModeName.CStr()));
  return DelMode(ModeNameToIdH.GetDat(ModeName));
}

int TMMNet::AddEdge(const TStr& LinkTypeName, int& NId1, int& NId2, int EId){
  //IAssertR(LinkNameToIdH.IsKey(LinkTypeName),TStr::Fmt("No such link name: %s",LinkTypeName.CStr()));
  return AddEdge(LinkNameToIdH.GetDat(LinkTypeName), NId1, NId2, EId);
}
int TMMNet::AddEdge(const TInt& LinkTypeId, int& NId1, int& NId2, int EId){
  //IAssertR(LinkIdToNameH.IsKey(LinkTypeId),TStr::Fmt("No link with id %d exists",LinkTypeId));
  return TCrossNetH.GetDat(LinkTypeId).AddEdge(NId1, NId2, EId);
}

TModeNet& TMMNet::GetModeNet(const TStr& ModeName) const {
  //IAssertR(ModeNameToIdH.IsKey(ModeName),TStr::Fmt("No such mode name: %s", ModeName.CStr()));
  return GetModeNet(ModeNameToIdH.GetDat(ModeName));
}

TModeNet& TMMNet::GetModeNet(const TInt& ModeId) const {
//  IAssertR(ModeId < TModeNetH.Len(), TStr::Fmt("Mode with id %d does not exist", ModeId));
  //TODO: figure out if there is a better way to ensure the non-const version of GetDat called.
  TModeNet &Net = (const_cast<TMMNet *>(this))->TModeNetH.GetDat(ModeId);
  return Net;
}
TCrossNet& TMMNet::GetCrossNet(const TStr& LinkName) const{
  //IAssertR(LinkNameToIdH.IsKey(LinkName),TStr::Fmt("No such link name: %s", LinkName.CStr()));
  return GetCrossNet(LinkNameToIdH.GetDat(LinkName));
}
TCrossNet& TMMNet::GetCrossNet(const TInt& LinkId) const{
  //IAssertR(LinkIdToNameH.IsKey(LinkId),TStr::Fmt("No link with id %d exists", LinkId));
  //TODO: figure out if there is a better way to ensure the non-const version of GetDat called.
  TCrossNet& CrossNet = (const_cast<TMMNet *>(this))->TCrossNetH.GetDat(LinkId);
  return CrossNet;
}



int TCrossNet::AddEdge(const int& sourceNId, const int& destNId, int EId){
  if (EId == -1) { EId = MxEId;  MxEId++; }
  else { MxEId = TMath::Mx(EId+1, MxEId()); }
  TCrossNet::TCrossEdge newEdge (EId, sourceNId, destNId);
  LinkH.AddDat(EId, newEdge);
  TStr ThisLinkName = Net->GetLinkName(this->LinkTypeId);
  Net->TModeNetH.GetDat(this->Mode1).AddNeighbor(sourceNId, EId, true, ThisLinkName, Mode1==Mode2, true); // TODO: can't assume it is directed
  Net->TModeNetH.GetDat(this->Mode2).AddNeighbor(destNId, EId, false, ThisLinkName, Mode1==Mode2, true);
  int i;
  // update attribute columns
  for (i = 0; i < VecOfIntVecsE.Len(); i++) {
    TVec<TInt>& IntVec = VecOfIntVecsE[i];
    IntVec.Ins(LinkH.GetKeyId(EId), TInt::Mn);
  }
  TVec<TStr> DefIntVec = TVec<TStr>();
  IntDefaultsE.GetKeyV(DefIntVec);
  for (i = 0; i < DefIntVec.Len(); i++) {
    TStr attr = DefIntVec[i];
    TVec<TInt>& IntVec = VecOfIntVecsE[KeyToIndexTypeE.GetDat(DefIntVec[i]).Val2];
    IntVec[LinkH.GetKeyId(EId)] = GetIntAttrDefaultE(attr);
  }
  for (i = 0; i < VecOfStrVecsE.Len(); i++) {
    TVec<TStr>& StrVec = VecOfStrVecsE[i];
    StrVec.Ins(LinkH.GetKeyId(EId), TStr::GetNullStr());
  }
  TVec<TStr> DefStrVec = TVec<TStr>();
  StrDefaultsE.GetKeyV(DefStrVec);
  for (i = 0; i < DefStrVec.Len(); i++) {
    TStr attr = DefStrVec[i];
    TVec<TStr>& StrVec = VecOfStrVecsE[KeyToIndexTypeE.GetDat(DefStrVec[i]).Val2];
    StrVec[LinkH.GetKeyId(EId)] = GetStrAttrDefaultE(attr);
  }

  for (i = 0; i < VecOfFltVecsE.Len(); i++) {
    TVec<TFlt>& FltVec = VecOfFltVecsE[i];
    FltVec.Ins(LinkH.GetKeyId(EId), TFlt::Mn);
  }
  TVec<TStr> DefFltVec = TVec<TStr>();
  FltDefaultsE.GetKeyV(DefFltVec);
  for (i = 0; i < DefFltVec.Len(); i++) {
    TStr attr = DefFltVec[i];
    TVec<TFlt>& FltVec = VecOfFltVecsE[KeyToIndexTypeE.GetDat(DefFltVec[i]).Val2];
    FltVec[LinkH.GetKeyId(EId)] = GetFltAttrDefaultE(attr);
  }
  return EId;
}

int TCrossNet::DelEdge(const int& EId) {
  TCrossEdge& Edge = LinkH.GetDat(EId);
  int srcNode = Edge.SrcNId;
  int dstNode = Edge.DstNId;
  TStr ThisLinkName = Net->GetLinkName(this->LinkTypeId);
  Net->TModeNetH.GetDat(this->Mode1).DelNeighbor(srcNode, EId, true, ThisLinkName, Mode1==Mode2, true); // TODO: can't assume it is directed
  Net->TModeNetH.GetDat(this->Mode2).DelNeighbor(dstNode, EId, false, ThisLinkName, Mode1==Mode2, true);
  int i;
  for (i = 0; i < VecOfIntVecsE.Len(); i++) {
    TVec<TInt>& IntVec = VecOfIntVecsE[i];
    IntVec[LinkH.GetKeyId(EId)] =  TInt::Mn;
  }
  for (i = 0; i < VecOfStrVecsE.Len(); i++) {
    TVec<TStr>& StrVec = VecOfStrVecsE[i];
    StrVec[LinkH.GetKeyId(EId)] =  TStr::GetNullStr();
  }
  for (i = 0; i < VecOfFltVecsE.Len(); i++) {
    TVec<TFlt>& FltVec = VecOfFltVecsE[i];
    FltVec[LinkH.GetKeyId(EId)] = TFlt::Mn;
  }
  return 0;
}

void TCrossNet::SetParentPointer(TMMNet* parent) {
  Net = parent;
}

void TCrossNet::AttrNameEI(const TInt& EId, TStrIntPrH::TIter LinkHI, TStrV& Names) const {
  Names = TVec<TStr>();
  while (!LinkHI.IsEnd()) {
    if (!EdgeAttrIsDeleted(EId, LinkHI)) {
      Names.Add(LinkHI.GetKey());
    }
    LinkHI++;
  }  
}

void TCrossNet::AttrValueEI(const TInt& EId, TStrIntPrH::TIter LinkHI, TStrV& Values) const {
  Values = TVec<TStr>();
  while (!LinkHI.IsEnd()) {
    if (!EdgeAttrIsDeleted(EId, LinkHI)) {
      Values.Add(GetEdgeAttrValue(EId, LinkHI));
    }
    LinkHI++;
  }  
}

void TCrossNet::IntAttrNameEI(const TInt& EId, TStrIntPrH::TIter LinkHI, TStrV& Names) const {
  Names = TVec<TStr>();
  while (!LinkHI.IsEnd()) {
    if (LinkHI.GetDat().Val1 == IntType && !EdgeAttrIsIntDeleted(EId, LinkHI)) {
      Names.Add(LinkHI.GetKey());
    }
    LinkHI++;
  }  
}

void TCrossNet::IntAttrValueEI(const TInt& EId, TStrIntPrH::TIter LinkHI, TIntV& Values) const {
  Values = TVec<TInt>();
  while (!LinkHI.IsEnd()) {
    if (LinkHI.GetDat().Val1 == IntType && !EdgeAttrIsIntDeleted(EId, LinkHI)) {
      TInt val = (this->VecOfIntVecsE.GetVal(LinkHI.GetDat().Val2).GetVal(EId));
      Values.Add(val);
    }
    LinkHI++;
  }  
}

void TCrossNet::StrAttrNameEI(const TInt& EId, TStrIntPrH::TIter LinkHI, TStrV& Names) const {
  Names = TVec<TStr>();
  while (!LinkHI.IsEnd()) {
    if (LinkHI.GetDat().Val1 == StrType && !EdgeAttrIsStrDeleted(EId, LinkHI)) {
      Names.Add(LinkHI.GetKey());
    }
    LinkHI++;
  }  
}

void TCrossNet::StrAttrValueEI(const TInt& EId, TStrIntPrH::TIter LinkHI, TStrV& Values) const {
  Values = TVec<TStr>();
  while (!LinkHI.IsEnd()) {
    if (LinkHI.GetDat().Val1 == StrType && !EdgeAttrIsStrDeleted(EId, LinkHI)) {
      TStr val = this->VecOfStrVecsE.GetVal(LinkHI.GetDat().Val2).GetVal(EId);
      Values.Add(val);
    }
    LinkHI++;
  }  
}

void TCrossNet::FltAttrNameEI(const TInt& EId, TStrIntPrH::TIter LinkHI, TStrV& Names) const {
  Names = TVec<TStr>();
  while (!LinkHI.IsEnd()) {
    if (LinkHI.GetDat().Val1 == FltType && !EdgeAttrIsFltDeleted(EId, LinkHI)) {
      Names.Add(LinkHI.GetKey());
    }
    LinkHI++;
  }  
}

void TCrossNet::FltAttrValueEI(const TInt& EId, TStrIntPrH::TIter LinkHI, TFltV& Values) const {
  Values = TVec<TFlt>();
  while (!LinkHI.IsEnd()) {
    if (LinkHI.GetDat().Val1 == FltType && !EdgeAttrIsFltDeleted(EId, LinkHI)) {
      TFlt val = (this->VecOfFltVecsE.GetVal(LinkHI.GetDat().Val2).GetVal(EId));
      Values.Add(val);
    }
    LinkHI++;
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

bool TCrossNet::EdgeAttrIsDeleted(const int& EId, const TStrIntPrH::TIter& LinkHI) const {
  bool IntDel = EdgeAttrIsIntDeleted(EId, LinkHI);
  bool StrDel = EdgeAttrIsStrDeleted(EId, LinkHI);
  bool FltDel = EdgeAttrIsFltDeleted(EId, LinkHI);
  return IntDel || StrDel || FltDel;
}

bool TCrossNet::EdgeAttrIsIntDeleted(const int& EId, const TStrIntPrH::TIter& LinkHI) const {
  return (LinkHI.GetDat().Val1 == IntType &&
    GetIntAttrDefaultE(LinkHI.GetKey()) == this->VecOfIntVecsE.GetVal(
    this->KeyToIndexTypeE.GetDat(LinkHI.GetKey()).Val2).GetVal(LinkH.GetKeyId(EId)));
}

bool TCrossNet::EdgeAttrIsStrDeleted(const int& EId, const TStrIntPrH::TIter& LinkHI) const {
  return (LinkHI.GetDat().Val1 == StrType &&
    GetStrAttrDefaultE(LinkHI.GetKey()) == this->VecOfStrVecsE.GetVal(
    this->KeyToIndexTypeE.GetDat(LinkHI.GetKey()).Val2).GetVal(LinkH.GetKeyId(EId)));
}

bool TCrossNet::EdgeAttrIsFltDeleted(const int& EId, const TStrIntPrH::TIter& LinkHI) const {
  return (LinkHI.GetDat().Val1 == FltType &&
    GetFltAttrDefaultE(LinkHI.GetKey()) == this->VecOfFltVecsE.GetVal(
    this->KeyToIndexTypeE.GetDat(LinkHI.GetKey()).Val2).GetVal(LinkH.GetKeyId(EId)));
}

TStr TCrossNet::GetEdgeAttrValue(const int& EId, const TStrIntPrH::TIter& LinkHI) const {
  if (LinkHI.GetDat().Val1 == IntType) {
    return (this->VecOfIntVecsE.GetVal(
      this->KeyToIndexTypeE.GetDat(LinkHI.GetKey()).Val2).GetVal(LinkH.GetKeyId(EId))).GetStr();
  } else if(LinkHI.GetDat().Val1 == StrType) {
    return this->VecOfStrVecsE.GetVal(
    this->KeyToIndexTypeE.GetDat(LinkHI.GetKey()).Val2).GetVal(LinkH.GetKeyId(EId));
  } else if (LinkHI.GetDat().Val1 == FltType) {
    return (this->VecOfFltVecsE.GetVal(
      this->KeyToIndexTypeE.GetDat(LinkHI.GetKey()).Val2).GetVal(LinkH.GetKeyId(EId))).GetStr();
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
    NewVec[LinkH.GetKeyId(EId)] = value;
  } else {
    CurrLen = VecOfIntVecsE.Len();
    KeyToIndexTypeE.AddDat(attr, TIntPr(IntType, CurrLen));
    TVec<TInt> NewVec = TVec<TInt>();
    for (i = 0; i < MxEId; i++) {
      NewVec.Ins(i, GetIntAttrDefaultE(attr));
    }
    NewVec[LinkH.GetKeyId(EId)] = value;
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
    NewVec[LinkH.GetKeyId(EId)] = value;
  } else {
    CurrLen = VecOfStrVecsE.Len();
    KeyToIndexTypeE.AddDat(attr, TIntPr(StrType, CurrLen));
    TVec<TStr> NewVec = TVec<TStr>();
    for (i = 0; i < MxEId; i++) {
      NewVec.Ins(i, GetStrAttrDefaultE(attr));
    }
    NewVec[LinkH.GetKeyId(EId)] = value;
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
    NewVec[LinkH.GetKeyId(EId)] = value;
  } else {
    CurrLen = VecOfFltVecsE.Len();
    KeyToIndexTypeE.AddDat(attr, TIntPr(FltType, CurrLen));
    TVec<TFlt> NewVec = TVec<TFlt>();
    for (i = 0; i < MxEId; i++) {
      NewVec.Ins(i, GetFltAttrDefaultE(attr));
    }
    NewVec[LinkH.GetKeyId(EId)] = value;
    VecOfFltVecsE.Add(NewVec);
  }
  return 0;
}

TInt TCrossNet::GetIntAttrDatE(const int& EId, const TStr& attr) {
  return VecOfIntVecsE[KeyToIndexTypeE.GetDat(attr).Val2][LinkH.GetKeyId(EId)];
}

TStr TCrossNet::GetStrAttrDatE(const int& EId, const TStr& attr) {
  return VecOfStrVecsE[KeyToIndexTypeE.GetDat(attr).Val2][LinkH.GetKeyId(EId)];
}

TFlt TCrossNet::GetFltAttrDatE(const int& EId, const TStr& attr) {
  return VecOfFltVecsE[KeyToIndexTypeE.GetDat(attr).Val2][LinkH.GetKeyId(EId)];
}

int TCrossNet::DelAttrDatE(const int& EId, const TStr& attr) {
  // TODO(nkhadke): add error checking
  TInt vecType = KeyToIndexTypeE(attr).Val1;
  if (vecType == IntType) {
    VecOfIntVecsE[KeyToIndexTypeE.GetDat(attr).Val2][LinkH.GetKeyId(EId)] = GetIntAttrDefaultE(attr);
  } else if (vecType == StrType) {
    VecOfStrVecsE[KeyToIndexTypeE.GetDat(attr).Val2][LinkH.GetKeyId(EId)] = GetStrAttrDefaultE(attr);
  } else if (vecType == FltType) {
    VecOfFltVecsE[KeyToIndexTypeE.GetDat(attr).Val2][LinkH.GetKeyId(EId)] = GetFltAttrDefaultE(attr);
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

//int TMMNet::DelEdge(const TInt& LinkTypeId, const TInt& EId) {
//  if (!TCrossNetH.IsKey(LinkTypeId)) {
//    return -1;
//  }
//  TCrossNet& CrossNet = TCrossNetH.GetDat(LinkTypeId);
//  TCrossNet::TCrossEdgeI EI = CrossNet.GetEdgeI(EId);
//  TInt SrcMId = EI.GetSrcModeId();
//  TInt SrcNId = EI.GetSrcNId();
//  TInt DstMId = EI.GetDstModeId();
//  TInt DstNId = EI.GetDstNId();
//  TModeNet Net1 = TModeNetV[SrcMId];
//  Net1.DelNeighbor(SrcNId, EId, true, LinkTypeId);
//  TModeNet Net2 = TModeNetV[DstMId];
//  Net2.DelNeighbor(DstNId, EId, false, LinkTypeId);
//  return CrossNet.DelEdge(EId);
//}


//
//int TCrossNet::AddLink (const int& sourceNId, const int& sourceNModeId, const int& destNId, const int& EId) {
//  int type1NId;
//  int type2NId;
//  bool direction = true;
//  if (Mode1 == sourceNModeId) {
//    type1NId = sourceNId;
//    type2NId = destNId;
//  } else if (Mode2 == sourceNModeId) {
//    type1NId = destNId;
//    type2NId = sourceNId;
//    direction = false;
//  } else {
//    return -1;
//  }
//  return AddLink(type1NId, type2NId, direction, EId);
//}
//
//int TCrossNet::AddLink (const int& NIdType1, const int& NIdType2, const bool& direction, const int& EId) {
//  int newEId;
//  if (EId == -1) {
//    MxEId++;
//    newEId = MxEId;
//  }
//  else {
////    IAssertR(!LinkH.IsKey(EId), TStr::Fmt("The edge with id %d already exists", EId ));
//    newEId = EId;
//    if (newEId > MxEId) MxEId = newEId; //TODO: Figure this out
//  }
//  TCrossEdge newEdge(newEId, NIdType1, NIdType2);
//  LinkH.AddDat(newEId, newEdge);
//  return EId;
//
//}


TStr TModeNet::GetNeighborLinkName(const TStr& LinkName, bool isOutEdge, const bool sameMode, bool isDir) const {
  TStr Cpy(LinkName);
  if (!isDir || !sameMode) { return Cpy; }
  if (isOutEdge) {
    Cpy += ":SRC";
  } else {
    Cpy += ":DST";
  }
  return Cpy;
}

int TModeNet::AddNeighbor(const int& NId, const int& EId, bool outEdge, const int linkId, const bool sameMode, bool isDir){
  TStr LinkName = MMNet->GetLinkName(linkId);
  return AddNeighbor(NId, EId, outEdge, LinkName, sameMode, isDir);
}

int TModeNet::AddNeighbor(const int& NId, const int& EId, bool outEdge, const TStr& LinkName, const bool sameMode, bool isDir){
  if (!NeighborTypes.IsKey(LinkName)) {
    AddNbrType(LinkName, sameMode, isDir);
  }
  TStr Name = GetNeighborLinkName(LinkName, outEdge, sameMode, isDir);
  return AppendIntVAttrDatN(NId, EId, Name); 
}

int TModeNet::DelNeighbor(const int& NId, const int& EId, bool outEdge, const TStr& LinkName, const bool sameMode, bool isDir){
  if (!NeighborTypes.IsKey(LinkName)) {
    return -1;
  }
  TStr Name = GetNeighborLinkName(LinkName, outEdge, sameMode, isDir);
  return DelFromIntVAttrDatN(NId, EId, Name);
}

int TModeNet::DelNeighbor(const int& NId, const int& EId, bool outEdge, const TInt& linkId, const bool sameMode, bool isDir){
  TStr LinkName = MMNet->GetLinkName(linkId);
  return DelNeighbor(NId, EId, outEdge, LinkName, sameMode, isDir);
}

//TODO: Finish implementing
int TModeNet::DelNode ( const int& NId){
  // loop through all neighbor vectors, call delete edge on each of these
  TNEANet::DelNode(NId);
  return -1;
}

void TModeNet::SetParentPointer(TMMNet* parent) {
  MMNet = parent;
}

int TModeNet::AddNbrType(const TStr& LinkName, const bool sameMode, bool isDir) {
  //IAssertR(!NeighborTypes.IsKey(LinkName),TStr::Fmt("Neighbor Link Types already exists: %s", LinkName.CStr()));
  if (NeighborTypes.IsKey(LinkName)) { return -1; } //Don't want to add nbr type multiple times
  bool hasSingleVector = (!isDir || !sameMode);
  NeighborTypes.AddDat(LinkName, hasSingleVector);
  return 0;
}

/*int TModeNet::AddNbrType(const TStr& LinkName, bool hasSingleVector, TVec<TIntV>& Neighbors) {
  //IAssertR(!NeighborTypes.IsKey(LinkName),TStr::Fmt("Neighbor Link Types already exists: %s", LinkName.CStr()));
  if (NeighborTypes.IsKey(LinkName)) { return -1; } //Don't want to add nbr type multiple times
  NeighborTypes.AddDat(LinkName, hasSingleVector);
  if (hasSingleVector) {
    AddIntVAttrN(LinkName);
  } else {
    TStr Name = GetNeighborLinkName(LinkName, true, sameMode, isDir);
    AddIntVAttrN(Name);
    Name = GetNeighborLinkName(LinkName, false, sameMode, isDir);
    AddIntVAttrN(Name);
  }
  return 0;
}*/

void TModeNet::GetNeighborsByLinkType(const int& NId, TStr& Name, TIntV& Neighbors, const bool isOutEId) const{
  // TODO: check if need the suffix
  //IAssertR(NeighborTypes.IsKey(Name), TStr::Fmt("Link Type does not exist: %s", Name));
  TBool hasSingleVector = NeighborTypes.GetDat(Name);
  if (hasSingleVector) {
    Neighbors = GetIntVAttrDatN(NId, Name);
  } else {
    TStr DirectionalName = GetNeighborLinkName(Name, isOutEId, true, true);
    Neighbors = GetIntVAttrDatN(NId, DirectionalName);
  }
}

int TModeNet::AddIntAttrN(const TStr& attr, TIntV& Attrs, TInt defaultValue){
  TInt CurrLen;
  TVec<TInt> NewVec;
  CurrLen = VecOfIntVecsN.Len();
  KeyToIndexTypeN.AddDat(attr, TIntPr(IntType, CurrLen));
  NewVec = TVec<TInt>(Attrs);
  VecOfIntVecsN.Add(NewVec);
  if (!IntDefaultsN.IsKey(attr)) {
    IntDefaultsN.AddDat(attr, defaultValue);
  } else {
    return -1;
  }
  return 0;
}

int TModeNet::AddStrAttrN(const TStr& attr, TStrV& Attrs, TStr defaultValue) {
  TInt CurrLen;
  TVec<TStr> NewVec;
  CurrLen = VecOfStrVecsN.Len();
  KeyToIndexTypeN.AddDat(attr, TIntPr(StrType, CurrLen));
  NewVec = TVec<TStr>(Attrs);
  VecOfStrVecsN.Add(NewVec);
  if (!StrDefaultsN.IsKey(attr)) {
    StrDefaultsN.AddDat(attr, defaultValue);
  } else {
    return -1;
  }
  return 0;
}

int TModeNet::AddFltAttrN(const TStr& attr, TFltV& Attrs, TFlt defaultValue) {
  TInt CurrLen;
  TVec<TFlt> NewVec;
  CurrLen = VecOfFltVecsN.Len();
  KeyToIndexTypeN.AddDat(attr, TIntPr(FltType, CurrLen));
  NewVec = TVec<TFlt>(Attrs);
  VecOfFltVecsN.Add(NewVec);
  if (!FltDefaultsN.IsKey(attr)) {
    FltDefaultsN.AddDat(attr, defaultValue);
  } else {
    return -1;
  }
  return 0;
}

int TModeNet::AddIntVAttrN(const TStr& attr, TVec<TIntV>& Attrs){
  TInt CurrLen;
  TVec<TIntV> NewVec;
  CurrLen = VecOfIntVecVecsN.Len();
  KeyToIndexTypeN.AddDat(attr, TIntPr(IntVType, CurrLen));
  NewVec = TVec<TIntV>(Attrs);
  VecOfIntVecVecsN.Add(NewVec);
  return 0;
}

//copy everything but node attributes
// iterate over all node attribute types
// if not intv, just add it back
// if is intv; check if it is a intv type in list of attributes
//
void TModeNet::RemoveLinkTypes(TModeNet& Result, TStrV& LinkTypes) {
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
      if (!NeighborTypes.IsKey(AttrName) && (!removeSuffix || !NeighborTypes.IsKey(WithoutSuffix))) {
        TVec<TIntV>& Attrs = VecOfIntVecVecsN[AttrIndex];
        Result.AddIntVAttrN(AttrName, Attrs);
      } else {
        TStr NbrName = AttrName;
        if (removeSuffix) {
          NbrName = WithoutSuffix;
        }
        if (LinkTypes.IsIn(NbrName)) {
          Result.AddNbrType(NbrName, removeSuffix, removeSuffix);
          TVec<TIntV>& Attrs = VecOfIntVecVecsN[AttrIndex];
          Result.AddIntVAttrN(AttrName, Attrs);
        }
      }
    }
  }
}

int TMMNet::AddMode(const TStr& ModeName, const TInt& ModeId, const TModeNet& ModeNet) {
  ModeIdToNameH.AddDat(ModeId, ModeName);
  ModeNameToIdH.AddDat(ModeName, ModeId);

  TModeNetH.AddDat(ModeId, ModeNet);
  return ModeId;

}
int TMMNet::AddLinkType(const TStr& LinkTypeName, const TInt& LinkTypeId, const TCrossNet& CrossNet) {
  LinkIdToNameH.AddDat(LinkTypeId, LinkTypeName);
  LinkNameToIdH.AddDat(LinkTypeName, LinkTypeId);

  TCrossNetH.AddDat(LinkTypeId, CrossNet);
  return LinkTypeId;
}


/*PMMNet TMMNet::GetSubgraphByCrossNet(TStrV& CrossNetTypes) {
  PMMNet NewGraph = TMMNet::New();
  // Get a set of modes
  THashSet<TInt> Modes;
  // go through, only add cross nets specified in CrossNetTypes; add modes in types to Modes
  // go through, only add modes specified in Modes; call RemoveLinkTypes function
  return NewGraph;
}*/

PMMNet TMMNet::GetSubgraphByCrossNet(TStrV& CrossNetTypes) {
  PMMNet Result = New();
  //iterate over crossnet types, get list of mode ints
  //THash<TStr,TInt> LinkNameToIdH;
  TInt MxMode = 0;
  TInt MxCross = 0;
  TIntH ModeH;
  for(int i = 0; i < CrossNetTypes.Len(); i++) {
    TStr CrossName = CrossNetTypes[i];
    TInt OldId = LinkNameToIdH.GetDat(CrossName);
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
      NewModeId1 = ModeH.GetDat(OldModeId2);
    } else {
      NewModeId2 = MxMode++;
      ModeH.AddDat(OldModeId2, NewModeId2);
    }
    NewCrossNet.SetParentPointer(this);
    NewCrossNet.Mode1 = NewModeId1;
    NewCrossNet.Mode2 = NewModeId2;
    NewCrossNet.LinkTypeId = NewId;
    Result->AddLinkType(CrossName, NewId, NewCrossNet);
  }
  for(TIntH::TIter it = ModeH.BegI(); it < ModeH.EndI(); it++) {
    TStr ModeName = ModeIdToNameH.GetDat(it.GetKey());
    TInt NewModeId = it.GetDat();
    TModeNet NewModeNet;
    TModeNetH.GetDat(it.GetKey()).RemoveLinkTypes(NewModeNet, CrossNetTypes);
    NewModeNet.SetParentPointer(this);
    NewModeNet.NModeId = NewModeId;
    Result->AddMode(ModeName, NewModeId, NewModeNet);
  }
  Result->MxModeId = MxMode;
  Result->MxLinkTypeId = MxCross;
  return Result;
}

PMMNet TMMNet::GetSubgraphByModeNet(TStrV& ModeNetTypes) {
  THashSet<TInt> ModeTypeIds;
  for (int i = 0; i < ModeNetTypes.Len(); i++) {
    ModeTypeIds.AddKey(ModeNameToIdH.GetDat(ModeNetTypes[i]));
  }
  TStrV CrossNetTypes;
  for (THash<TInt, TCrossNet>::TIter it = TCrossNetH.BegI(); it < TCrossNetH.EndI(); it++) {
    TCrossNet& CrossNet = it.GetDat();
    if (ModeTypeIds.IsKey(CrossNet.Mode1) && ModeTypeIds.IsKey(CrossNet.Mode2)) {
      CrossNetTypes.Add(LinkIdToNameH.GetDat(it.GetKey()));
    }
  }
  return GetSubgraphByCrossNet(CrossNetTypes);
  //for each mode in ModeNetTypes, get a list of it's crossnet types; check if both modes are in ModeNetTypes; if is, add to list of crossnets
    //then, call RemoveLinkTypes with working list of crossnet types, add new mode to NewGraph
  //for each CrossNet in list generated in above step, add to NewGraph.
  //return NewGraph;
}
