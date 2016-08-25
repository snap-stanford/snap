/////////////////////////////////////////////////
// Attribute Node Edge Network
bool TNEANet::HasFlag(const TGraphFlag& Flag) const {
  return HasGraphFlag(TNEANet::TNet, Flag);
}

bool TNEANet::TNodeI::IsInNId(const int& NId) const {
  const TNode& Node = NodeHI.GetDat();
  for (int edge = 0; edge < Node.GetInDeg(); edge++) {
    if (NId == Graph->GetEdge(Node.GetInEId(edge)).GetSrcNId())
      return true;
  }
  return false;
}

bool TNEANet::TNodeI::IsOutNId(const int& NId) const {
  const TNode& Node = NodeHI.GetDat();
  for (int edge = 0; edge < Node.GetOutDeg(); edge++) {
    if (NId == Graph->GetEdge(Node.GetOutEId(edge)).GetDstNId())
      return true;
  }
  return false;
}

void TNEANet::AttrNameNI(const TInt& NId, TStrIntPrH::TIter NodeHI, TStrV& Names) const {
  Names = TVec<TStr>();
  while (!NodeHI.IsEnd()) {
    if (!NodeAttrIsDeleted(NId, NodeHI)) {
      Names.Add(NodeHI.GetKey());
    }
    NodeHI++;
  }  
}

void TNEANet::AttrValueNI(const TInt& NId , TStrIntPrH::TIter NodeHI, TStrV& Values) const {
  Values = TVec<TStr>();
  while (!NodeHI.IsEnd()) {
    if (!NodeAttrIsDeleted(NId, NodeHI)) {
      Values.Add(GetNodeAttrValue(NId, NodeHI));
    }
    NodeHI++;
  }
}

void TNEANet::IntAttrNameNI(const TInt& NId, TStrIntPrH::TIter NodeHI, TStrV& Names) const {
  Names = TVec<TStr>();
  while (!NodeHI.IsEnd()) {
    if (NodeHI.GetDat().Val1 == IntType && !NodeAttrIsIntDeleted(NId, NodeHI)) {
      Names.Add(NodeHI.GetKey());
    }
    NodeHI++;
  }  
}

void TNEANet::IntAttrValueNI(const TInt& NId, TStrIntPrH::TIter NodeHI, TIntV& Values) const {
  Values = TVec<TInt>();
  while (!NodeHI.IsEnd()) {
    if (NodeHI.GetDat().Val1 == IntType && !NodeAttrIsIntDeleted(NId, NodeHI)) {
      TInt val = this->VecOfIntVecsN.GetVal(NodeHI.GetDat().Val2).GetVal(NodeH.GetKeyId(NId));
      Values.Add(val);
    }
    NodeHI++;
  }  
}

void TNEANet::IntVAttrNameNI(const TInt& NId, TStrIntPrH::TIter NodeHI, TStrV& Names) const {
  Names = TVec<TStr>();
  while (!NodeHI.IsEnd()) {
    if (NodeHI.GetDat().Val1 == IntVType) {
      Names.Add(NodeHI.GetKey());
    }
    NodeHI++;
  }  
}

void TNEANet::IntVAttrValueNI(const TInt& NId, TStrIntPrH::TIter NodeHI, TVec<TIntV>& Values) const {
  Values = TVec<TIntV>();
  while (!NodeHI.IsEnd()) {
    if (NodeHI.GetDat().Val1 == IntVType) {
      TIntV val = this->VecOfIntVecVecsN.GetVal(NodeHI.GetDat().Val2).GetVal(NodeH.GetKeyId(NId));
      Values.Add(val);
    }
    NodeHI++;
  }  
}

void TNEANet::StrAttrNameNI(const TInt& NId, TStrIntPrH::TIter NodeHI, TStrV& Names) const {
  Names = TVec<TStr>();
  while (!NodeHI.IsEnd()) {
    if (NodeHI.GetDat().Val1 == StrType && !NodeAttrIsStrDeleted(NId, NodeHI)) {
      Names.Add(NodeHI.GetKey());
    }
    NodeHI++;
  }  
}

void TNEANet::StrAttrValueNI(const TInt& NId, TStrIntPrH::TIter NodeHI, TStrV& Values) const {
  Values = TVec<TStr>();
  while (!NodeHI.IsEnd()) {
    if (NodeHI.GetDat().Val1 == StrType && !NodeAttrIsStrDeleted(NId, NodeHI)) {
      TStr val = this->VecOfStrVecsN.GetVal(NodeHI.GetDat().Val2).GetVal(NodeH.GetKeyId(NId));
      Values.Add(val);
    }
    NodeHI++;
  }  
}

void TNEANet::FltAttrNameNI(const TInt& NId, TStrIntPrH::TIter NodeHI, TStrV& Names) const {
  Names = TVec<TStr>();
  while (!NodeHI.IsEnd()) {
    if (NodeHI.GetDat().Val1 == FltType && !NodeAttrIsFltDeleted(NId, NodeHI)) {
      Names.Add(NodeHI.GetKey());
    }
    NodeHI++;
  }  
}

void TNEANet::FltAttrValueNI(const TInt& NId, TStrIntPrH::TIter NodeHI, TFltV& Values) const {
  Values = TVec<TFlt>();
  while (!NodeHI.IsEnd()) {
    if (NodeHI.GetDat().Val1 == FltType && !NodeAttrIsFltDeleted(NId, NodeHI)) {
      TFlt val = (this->VecOfFltVecsN.GetVal(NodeHI.GetDat().Val2).GetVal(NodeH.GetKeyId(NId)));
      Values.Add(val);
    }
    NodeHI++;
  }  
}

bool TNEANet::IsAttrDeletedN(const int& NId, const TStr& attr) const {
  bool IntDel = IsIntAttrDeletedN(NId, attr);
  bool StrDel = IsStrAttrDeletedN(NId, attr);
  bool FltDel = IsFltAttrDeletedN(NId, attr);
  bool IntVDel = IsIntVAttrDeletedN(NId, attr);
  return IntDel || StrDel || FltDel || IntVDel;
}

bool TNEANet::IsIntAttrDeletedN(const int& NId, const TStr& attr) const {
  return NodeAttrIsIntDeleted(NId, KeyToIndexTypeN.GetI(attr));
}

bool TNEANet::IsIntVAttrDeletedN(const int& NId, const TStr& attr) const {
  return NodeAttrIsIntVDeleted(NId, KeyToIndexTypeN.GetI(attr));
}

bool TNEANet::IsStrAttrDeletedN(const int& NId, const TStr& attr) const {
  return NodeAttrIsStrDeleted(NId, KeyToIndexTypeN.GetI(attr));
}

bool TNEANet::IsFltAttrDeletedN(const int& NId, const TStr& attr) const {
  return NodeAttrIsFltDeleted(NId, KeyToIndexTypeN.GetI(attr));
}

bool TNEANet::NodeAttrIsDeleted(const int& NId, const TStrIntPrH::TIter& NodeHI) const {
  bool IntDel = NodeAttrIsIntDeleted(NId, NodeHI);
  bool StrDel = NodeAttrIsStrDeleted(NId, NodeHI);
  bool FltDel = NodeAttrIsFltDeleted(NId, NodeHI);
  bool IntVDel = NodeAttrIsIntVDeleted(NId, NodeHI);
  return IntDel || StrDel || FltDel || IntVDel;
}

bool TNEANet::NodeAttrIsIntDeleted(const int& NId, const TStrIntPrH::TIter& NodeHI) const {
  if (NodeHI.GetDat().Val1 != IntType) {
    return false;
  }
  return (GetIntAttrDefaultN(NodeHI.GetKey()) == this->VecOfIntVecsN.GetVal(
    this->KeyToIndexTypeN.GetDat(NodeHI.GetKey()).Val2).GetVal(NodeH.GetKeyId(NId)));
}

bool TNEANet::NodeAttrIsIntVDeleted(const int& NId, const TStrIntPrH::TIter& NodeHI) const {
  if (NodeHI.GetDat().Val1 != IntVType) {
    return false;
  }
  return (TIntV() == this->VecOfIntVecVecsN.GetVal(
    this->KeyToIndexTypeN.GetDat(NodeHI.GetKey()).Val2).GetVal(NodeH.GetKeyId(NId)));
}

bool TNEANet::NodeAttrIsStrDeleted(const int& NId, const TStrIntPrH::TIter& NodeHI) const {
  if (NodeHI.GetDat().Val1 != StrType) {
    return false;
  }
  return (GetStrAttrDefaultN(NodeHI.GetKey()) == this->VecOfStrVecsN.GetVal(
    this->KeyToIndexTypeN.GetDat(NodeHI.GetKey()).Val2).GetVal(NodeH.GetKeyId(NId)));
}

bool TNEANet::NodeAttrIsFltDeleted(const int& NId, const TStrIntPrH::TIter& NodeHI) const {
  if (NodeHI.GetDat().Val1 != FltType) {
    return false;
  }
  return (GetFltAttrDefaultN(NodeHI.GetKey()) == this->VecOfFltVecsN.GetVal(
    this->KeyToIndexTypeN.GetDat(NodeHI.GetKey()).Val2).GetVal(NodeH.GetKeyId(NId)));
}

TStr TNEANet::GetNodeAttrValue(const int& NId, const TStrIntPrH::TIter& NodeHI) const {
  if (NodeHI.GetDat().Val1 == IntType) {
    return (this->VecOfIntVecsN.GetVal(
      this->KeyToIndexTypeN.GetDat(NodeHI.GetKey()).Val2).GetVal(NodeH.GetKeyId(NId))).GetStr();
  } else if(NodeHI.GetDat().Val1 == StrType) {
    return this->VecOfStrVecsN.GetVal(
    this->KeyToIndexTypeN.GetDat(NodeHI.GetKey()).Val2).GetVal(NodeH.GetKeyId(NId));
  } else if (NodeHI.GetDat().Val1 == FltType) {
    return (this->VecOfFltVecsN.GetVal(
      this->KeyToIndexTypeN.GetDat(NodeHI.GetKey()).Val2).GetVal(NodeH.GetKeyId(NId))).GetStr();
  }
  return TStr::GetNullStr();
}

void TNEANet::AttrNameEI(const TInt& EId, TStrIntPrH::TIter EdgeHI, TStrV& Names) const {
  Names = TVec<TStr>();
  while (!EdgeHI.IsEnd()) {
    if (!EdgeAttrIsDeleted(EId, EdgeHI)) {
      Names.Add(EdgeHI.GetKey());
    }
    EdgeHI++;
  }  
}

void TNEANet::AttrValueEI(const TInt& EId, TStrIntPrH::TIter EdgeHI, TStrV& Values) const {
  Values = TVec<TStr>();
  while (!EdgeHI.IsEnd()) {
    if (!EdgeAttrIsDeleted(EId, EdgeHI)) {
      Values.Add(GetEdgeAttrValue(EId, EdgeHI));
    }
    EdgeHI++;
  }  
}

void TNEANet::IntAttrNameEI(const TInt& EId, TStrIntPrH::TIter EdgeHI, TStrV& Names) const {
  Names = TVec<TStr>();
  while (!EdgeHI.IsEnd()) {
    if (EdgeHI.GetDat().Val1 == IntType && !EdgeAttrIsIntDeleted(EId, EdgeHI)) {
      Names.Add(EdgeHI.GetKey());
    }
    EdgeHI++;
  }  
}

void TNEANet::IntAttrValueEI(const TInt& EId, TStrIntPrH::TIter EdgeHI, TIntV& Values) const {
  Values = TVec<TInt>();
  while (!EdgeHI.IsEnd()) {
    if (EdgeHI.GetDat().Val1 == IntType && !EdgeAttrIsIntDeleted(EId, EdgeHI)) {
      TInt val = (this->VecOfIntVecsE.GetVal(EdgeHI.GetDat().Val2).GetVal(EId));
      Values.Add(val);
    }
    EdgeHI++;
  }  
}

void TNEANet::IntVAttrNameEI(const TInt& EId, TStrIntPrH::TIter EdgeHI, TStrV& Names) const {
  Names = TVec<TStr>();
  while (!EdgeHI.IsEnd()) {
    if (EdgeHI.GetDat().Val1 == IntVType) {
      Names.Add(EdgeHI.GetKey());
    }
    EdgeHI++;
  }  
}

void TNEANet::IntVAttrValueEI(const TInt& EId, TStrIntPrH::TIter EdgeHI, TVec<TIntV>& Values) const {
  Values = TVec<TIntV>();
  while (!EdgeHI.IsEnd()) {
    if (EdgeHI.GetDat().Val1 == IntVType) {
      TIntV val = (this->VecOfIntVecVecsE.GetVal(EdgeHI.GetDat().Val2).GetVal(EId));
      Values.Add(val);
    }
    EdgeHI++;
  }  
}

void TNEANet::StrAttrNameEI(const TInt& EId, TStrIntPrH::TIter EdgeHI, TStrV& Names) const {
  Names = TVec<TStr>();
  while (!EdgeHI.IsEnd()) {
    if (EdgeHI.GetDat().Val1 == StrType && !EdgeAttrIsStrDeleted(EId, EdgeHI)) {
      Names.Add(EdgeHI.GetKey());
    }
    EdgeHI++;
  }  
}

void TNEANet::StrAttrValueEI(const TInt& EId, TStrIntPrH::TIter EdgeHI, TStrV& Values) const {
  Values = TVec<TStr>();
  while (!EdgeHI.IsEnd()) {
    if (EdgeHI.GetDat().Val1 == StrType && !EdgeAttrIsStrDeleted(EId, EdgeHI)) {
      TStr val = this->VecOfStrVecsE.GetVal(EdgeHI.GetDat().Val2).GetVal(EId);
      Values.Add(val);
    }
    EdgeHI++;
  }  
}

void TNEANet::FltAttrNameEI(const TInt& EId, TStrIntPrH::TIter EdgeHI, TStrV& Names) const {
  Names = TVec<TStr>();
  while (!EdgeHI.IsEnd()) {
    if (EdgeHI.GetDat().Val1 == FltType && !EdgeAttrIsFltDeleted(EId, EdgeHI)) {
      Names.Add(EdgeHI.GetKey());
    }
    EdgeHI++;
  }  
}

void TNEANet::FltAttrValueEI(const TInt& EId, TStrIntPrH::TIter EdgeHI, TFltV& Values) const {
  Values = TVec<TFlt>();
  while (!EdgeHI.IsEnd()) {
    if (EdgeHI.GetDat().Val1 == FltType && !EdgeAttrIsFltDeleted(EId, EdgeHI)) {
      TFlt val = (this->VecOfFltVecsE.GetVal(EdgeHI.GetDat().Val2).GetVal(EId));
      Values.Add(val);
    }
    EdgeHI++;
  }  
}

bool TNEANet::IsAttrDeletedE(const int& EId, const TStr& attr) const {
  bool IntDel = IsIntAttrDeletedE(EId, attr);
  bool IntVDel = IsIntVAttrDeletedE(EId, attr);
  bool StrDel = IsStrAttrDeletedE(EId, attr);
  bool FltDel = IsFltAttrDeletedE(EId, attr);
  return IntDel || StrDel || FltDel || IntVDel;
}

bool TNEANet::IsIntAttrDeletedE(const int& EId, const TStr& attr) const {
  return EdgeAttrIsIntDeleted(EId, KeyToIndexTypeE.GetI(attr));
}

bool TNEANet::IsIntVAttrDeletedE(const int& EId, const TStr& attr) const {
  return EdgeAttrIsIntVDeleted(EId, KeyToIndexTypeE.GetI(attr));
}

bool TNEANet::IsStrAttrDeletedE(const int& EId, const TStr& attr) const {
  return EdgeAttrIsStrDeleted(EId, KeyToIndexTypeE.GetI(attr));
}

bool TNEANet::IsFltAttrDeletedE(const int& EId, const TStr& attr) const {
  return EdgeAttrIsFltDeleted(EId, KeyToIndexTypeE.GetI(attr));
}

bool TNEANet::EdgeAttrIsDeleted(const int& EId, const TStrIntPrH::TIter& EdgeHI) const {
  bool IntDel = EdgeAttrIsIntDeleted(EId, EdgeHI);
  bool IntVDel = EdgeAttrIsIntVDeleted(EId, EdgeHI);
  bool StrDel = EdgeAttrIsStrDeleted(EId, EdgeHI);
  bool FltDel = EdgeAttrIsFltDeleted(EId, EdgeHI);
  return IntDel || StrDel || FltDel || IntVDel;
}

bool TNEANet::EdgeAttrIsIntDeleted(const int& EId, const TStrIntPrH::TIter& EdgeHI) const {
  return (EdgeHI.GetDat().Val1 == IntType &&
    GetIntAttrDefaultE(EdgeHI.GetKey()) == this->VecOfIntVecsE.GetVal(
    this->KeyToIndexTypeE.GetDat(EdgeHI.GetKey()).Val2).GetVal(EdgeH.GetKeyId(EId)));
}

bool TNEANet::EdgeAttrIsIntVDeleted(const int& EId, const TStrIntPrH::TIter& EdgeHI) const {
  return (EdgeHI.GetDat().Val1 == IntVType &&
    TIntV() == this->VecOfIntVecVecsE.GetVal(
    this->KeyToIndexTypeE.GetDat(EdgeHI.GetKey()).Val2).GetVal(EdgeH.GetKeyId(EId)));
}

bool TNEANet::EdgeAttrIsStrDeleted(const int& EId, const TStrIntPrH::TIter& EdgeHI) const {
  return (EdgeHI.GetDat().Val1 == StrType &&
    GetStrAttrDefaultE(EdgeHI.GetKey()) == this->VecOfStrVecsE.GetVal(
    this->KeyToIndexTypeE.GetDat(EdgeHI.GetKey()).Val2).GetVal(EdgeH.GetKeyId(EId)));
}

bool TNEANet::EdgeAttrIsFltDeleted(const int& EId, const TStrIntPrH::TIter& EdgeHI) const {
  return (EdgeHI.GetDat().Val1 == FltType &&
    GetFltAttrDefaultE(EdgeHI.GetKey()) == this->VecOfFltVecsE.GetVal(
    this->KeyToIndexTypeE.GetDat(EdgeHI.GetKey()).Val2).GetVal(EdgeH.GetKeyId(EId)));
}

TStr TNEANet::GetEdgeAttrValue(const int& EId, const TStrIntPrH::TIter& EdgeHI) const {
  if (EdgeHI.GetDat().Val1 == IntType) {
    return (this->VecOfIntVecsE.GetVal(
      this->KeyToIndexTypeE.GetDat(EdgeHI.GetKey()).Val2).GetVal(EdgeH.GetKeyId(EId))).GetStr();
  } else if(EdgeHI.GetDat().Val1 == StrType) {
    return this->VecOfStrVecsE.GetVal(
    this->KeyToIndexTypeE.GetDat(EdgeHI.GetKey()).Val2).GetVal(EdgeH.GetKeyId(EId));
  } else if (EdgeHI.GetDat().Val1 == FltType) {
    return (this->VecOfFltVecsE.GetVal(
      this->KeyToIndexTypeE.GetDat(EdgeHI.GetKey()).Val2).GetVal(EdgeH.GetKeyId(EId))).GetStr();
  }
  return TStr::GetNullStr();
}

int TNEANet::AddNode(int NId) {
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    MxNId = TMath::Mx(NId+1, MxNId());
  }
  NodeH.AddDat(NId, TNode(NId));
  AddAttributes(NId);
  return NId;
}

int TNEANet::AddNodeUnchecked(int NId) {
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    if (IsNode(NId)) { return -1;}
    MxNId = TMath::Mx(NId+1, MxNId());
  }
  NodeH.AddDat(NId, TNode(NId));
  AddAttributes(NId);
  return NId;
}

int TNEANet::AddAttributes(const int NId) {
  int i;
  // update attribute columns
  for (i = 0; i < VecOfIntVecsN.Len(); i++) {
    TVec<TInt>& IntVec = VecOfIntVecsN[i];
    int KeyId = NodeH.GetKeyId(NId);
    if (IntVec.Len() > KeyId) {
      IntVec[KeyId] = TInt::Mn;
    } else {
      IntVec.Ins(KeyId, TInt::Mn);
    }
  }
  TVec<TStr> DefIntVec = TVec<TStr>();
  IntDefaultsN.GetKeyV(DefIntVec);
  for (i = 0; i < DefIntVec.Len(); i++) {
    TStr attr = DefIntVec[i];
    TVec<TInt>& IntVec = VecOfIntVecsN[KeyToIndexTypeN.GetDat(DefIntVec[i]).Val2];
    IntVec[NodeH.GetKeyId(NId)] = GetIntAttrDefaultN(attr);
  } 
  for (i = 0; i < VecOfStrVecsN.Len(); i++) {
    TVec<TStr>& StrVec = VecOfStrVecsN[i];
    int KeyId = NodeH.GetKeyId(NId);
    if (StrVec.Len() > KeyId) {
      StrVec[KeyId] = TStr::GetNullStr();
    } else {
      StrVec.Ins(KeyId, TStr::GetNullStr());
    }
  }
  TVec<TStr> DefStrVec = TVec<TStr>();
  StrDefaultsN.GetKeyV(DefStrVec);
  for (i = 0; i < DefStrVec.Len(); i++) {
    TStr attr = DefStrVec[i];
    TVec<TStr>& StrVec = VecOfStrVecsN[KeyToIndexTypeN.GetDat(DefStrVec[i]).Val2];
    StrVec[NodeH.GetKeyId(NId)] = GetStrAttrDefaultN(attr);
  }
  for (i = 0; i < VecOfFltVecsN.Len(); i++) {
    TVec<TFlt>& FltVec = VecOfFltVecsN[i];
    int KeyId = NodeH.GetKeyId(NId);
    if (FltVec.Len() > KeyId) {
      FltVec[KeyId] = TFlt::Mn;
    } else {
      FltVec.Ins(KeyId, TFlt::Mn);
    }
  }
  TVec<TStr> DefFltVec = TVec<TStr>();
  FltDefaultsN.GetKeyV(DefFltVec);
  for (i = 0; i < DefFltVec.Len(); i++) {
    TStr attr = DefFltVec[i];
    TVec<TFlt>& FltVec = VecOfFltVecsN[KeyToIndexTypeN.GetDat(DefFltVec[i]).Val2];
    FltVec[NodeH.GetKeyId(NId)] = GetFltAttrDefaultN(attr);
  }
  for (i = 0; i < VecOfIntVecVecsN.Len(); i++) {
    TVec<TIntV>& IntVecV = VecOfIntVecVecsN[i];
    int KeyId = NodeH.GetKeyId(NId);
    if (IntVecV.Len() > KeyId) {
      IntVecV[KeyId] = TIntV();
    } else {
      IntVecV.Ins(KeyId, TIntV());
    }
  }
  return NId;
}

void TNEANet::DelNode(const int& NId) {
  int i;
  TInt Id(NId);
  SAttrN.DelSAttrId(Id);
  const TNode& Node = GetNode(NId);
  for (int out = 0; out < Node.GetOutDeg(); out++) {
    const int EId = Node.GetOutEId(out);
    const TEdge& Edge = GetEdge(EId);
    IAssert(Edge.GetSrcNId() == NId);
    GetNode(Edge.GetDstNId()).InEIdV.DelIfIn(EId);
    // Delete from Edge Attributes
    for (i = 0; i < VecOfIntVecsE.Len(); i++) {
      TVec<TInt>& IntVec = VecOfIntVecsE[i];
      IntVec[EdgeH.GetKeyId(EId)] = TInt::Mn;
    }
    for (i = 0; i < VecOfStrVecsE.Len(); i++) {
      TVec<TStr>& StrVec = VecOfStrVecsE[i];
      StrVec[EdgeH.GetKeyId(EId)] = TStr::GetNullStr();
    }
    for (i = 0; i < VecOfFltVecsE.Len(); i++) {
      TVec<TFlt>& FltVec = VecOfFltVecsE[i];
      FltVec[EdgeH.GetKeyId(EId)] = TFlt::Mn;
    }
    for (i = 0; i < VecOfIntVecVecsE.Len(); i++) {
      TVec<TIntV>& IntVecV = VecOfIntVecVecsE[i];
      IntVecV[EdgeH.GetKeyId(EId)] = TIntV();
    }
    EdgeH.DelKey(EId);
  }
  for (int in = 0; in < Node.GetInDeg(); in++) {
    const int EId = Node.GetInEId(in);
    const TEdge& Edge = GetEdge(EId);
    IAssert(Edge.GetDstNId() == NId);
    GetNode(Edge.GetSrcNId()).OutEIdV.DelIfIn(EId);
    // Delete from Edge Attributes
    for (i = 0; i < VecOfIntVecsE.Len(); i++) {
      TVec<TInt>& IntVec = VecOfIntVecsE[i];
      IntVec[EdgeH.GetKeyId(EId)] = TInt::Mn;
    }
    for (i = 0; i < VecOfStrVecsE.Len(); i++) {
      TVec<TStr>& StrVec = VecOfStrVecsE[i];
      StrVec[EdgeH.GetKeyId(EId)] = TStr::GetNullStr();
    }
    for (i = 0; i < VecOfFltVecsE.Len(); i++) {
      TVec<TFlt>& FltVec = VecOfFltVecsE[i];
      FltVec[EdgeH.GetKeyId(EId)] = TFlt::Mn;
    }
    for (i = 0; i < VecOfIntVecVecsE.Len(); i++) {
      TVec<TIntV>& IntVecV = VecOfIntVecVecsE[i];
      IntVecV[EdgeH.GetKeyId(EId)] = TIntV();
    }
    EdgeH.DelKey(EId);
  }

  for (i = 0; i < VecOfIntVecsN.Len(); i++) {
    TVec<TInt>& IntVec = VecOfIntVecsN[i];
    IntVec[NodeH.GetKeyId(NId)] = TInt::Mn;
  }
  for (i = 0; i < VecOfStrVecsN.Len(); i++) {
    TVec<TStr>& StrVec = VecOfStrVecsN[i];
    StrVec[NodeH.GetKeyId(NId)] = TStr::GetNullStr();
  }
  for (i = 0; i < VecOfFltVecsN.Len(); i++) {
    TVec<TFlt>& FltVec = VecOfFltVecsN[i];
    FltVec[NodeH.GetKeyId(NId)] = TFlt::Mn;
  }
  for (i = 0; i < VecOfIntVecVecsN.Len(); i++) {
    TVec<TIntV>& IntVecV = VecOfIntVecVecsN[i];
    IntVecV[NodeH.GetKeyId(NId)] = TIntV();
  }
  NodeH.DelKey(NId);
}

int TNEANet::AddEdge(const int& SrcNId, const int& DstNId, int EId) {
  int i;

  if (EId == -1) { EId = MxEId;  MxEId++; }
  else { MxEId = TMath::Mx(EId+1, MxEId()); }
  IAssertR(!IsEdge(EId), TStr::Fmt("EdgeId %d already exists", EId));
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  EdgeH.AddDat(EId, TEdge(EId, SrcNId, DstNId));
  GetNode(SrcNId).OutEIdV.AddSorted(EId);
  GetNode(DstNId).InEIdV.AddSorted(EId);

  // update attribute columns
  for (i = 0; i < VecOfIntVecsE.Len(); i++) {
    TVec<TInt>& IntVec = VecOfIntVecsE[i];
    int KeyId = EdgeH.GetKeyId(EId);
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
    IntVec[EdgeH.GetKeyId(EId)] = GetIntAttrDefaultE(attr);
  }

  for (i = 0; i < VecOfIntVecVecsE.Len(); i++) {
    TVec<TIntV>& IntVecV = VecOfIntVecVecsE[i];
    IntVecV.Ins(EdgeH.GetKeyId(EId), TIntV());
  }

  for (i = 0; i < VecOfStrVecsE.Len(); i++) {
    TVec<TStr>& StrVec = VecOfStrVecsE[i];
    int KeyId = EdgeH.GetKeyId(EId);
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
    StrVec[EdgeH.GetKeyId(EId)] = GetStrAttrDefaultE(attr);
  }

  for (i = 0; i < VecOfFltVecsE.Len(); i++) {
    TVec<TFlt>& FltVec = VecOfFltVecsE[i];
    int KeyId = EdgeH.GetKeyId(EId);
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
    FltVec[EdgeH.GetKeyId(EId)] = GetFltAttrDefaultE(attr);
  }
  return EId;
}

void TNEANet::DelEdge(const int& EId) {
  int i;

  IAssert(IsEdge(EId));
  const int SrcNId = GetEdge(EId).GetSrcNId();
  const int DstNId = GetEdge(EId).GetDstNId();
  GetNode(SrcNId).OutEIdV.DelIfIn(EId);
  GetNode(DstNId).InEIdV.DelIfIn(EId);
  TInt Id(EId);
  SAttrE.DelSAttrId(Id);

  for (i = 0; i < VecOfIntVecsE.Len(); i++) {
    TVec<TInt>& IntVec = VecOfIntVecsE[i];
    IntVec[EdgeH.GetKeyId(EId)] = TInt::Mn;
  }
  for (i = 0; i < VecOfStrVecsE.Len(); i++) {
    TVec<TStr>& StrVec = VecOfStrVecsE[i];
    StrVec[EdgeH.GetKeyId(EId)] = TStr::GetNullStr();
  }
  for (i = 0; i < VecOfFltVecsE.Len(); i++) {
    TVec<TFlt>& FltVec = VecOfFltVecsE[i];
    FltVec[EdgeH.GetKeyId(EId)] = TFlt::Mn;
  }

  EdgeH.DelKey(EId);
}

// delete all edges between the two nodes
void TNEANet::DelEdge(const int& SrcNId, const int& DstNId, const bool& IsDir) {
  int EId = 0;
  bool Edge = IsEdge(SrcNId, DstNId, EId, IsDir);
  IAssert(Edge); // there is at least one edge
  while (Edge) {
    DelEdge(EId);
    Edge = IsEdge(SrcNId, DstNId, EId, IsDir);
  }
}

bool TNEANet::IsEdge(const int& SrcNId, const int& DstNId, int& EId, const bool& IsDir) const {
  const TNode& SrcNode = GetNode(SrcNId);
  for (int edge = 0; edge < SrcNode.GetOutDeg(); edge++) {
    const TEdge& Edge = GetEdge(SrcNode.GetOutEId(edge));
    if (DstNId == Edge.GetDstNId()) {
      EId = Edge.GetId();
      return true;
    }
  }
  if (! IsDir) {
    for (int edge = 0; edge < SrcNode.GetInDeg(); edge++) {
      const TEdge& Edge = GetEdge(SrcNode.GetInEId(edge));
      if (DstNId == Edge.GetSrcNId()) {
        EId = Edge.GetId();
        return true;
      }
    }
  }
  return false;
}

void TNEANet::GetNIdV(TIntV& NIdV) const {
  NIdV.Gen(GetNodes(), 0);
  for (int N=NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    NIdV.Add(NodeH.GetKey(N));
  }
}

void TNEANet::GetEIdV(TIntV& EIdV) const {
  EIdV.Gen(GetEdges(), 0);
  for (int E=EdgeH.FFirstKeyId(); EdgeH.FNextKeyId(E); ) {
    EIdV.Add(EdgeH.GetKey(E));
  }
}

void TNEANet::Defrag(const bool& OnlyNodeLinks) {
  for (int kid = NodeH.FFirstKeyId(); NodeH.FNextKeyId(kid); ) {
    TNode& Node = NodeH[kid];
    Node.InEIdV.Pack();  Node.OutEIdV.Pack();
  }
  if (! OnlyNodeLinks && ! NodeH.IsKeyIdEqKeyN()) { NodeH.Defrag(); }
  if (! OnlyNodeLinks && ! EdgeH.IsKeyIdEqKeyN()) { EdgeH.Defrag(); }
}

bool TNEANet::IsOk(const bool& ThrowExcept) const {
  bool RetVal = true;
  for (int N = NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    const TNode& Node = NodeH[N];
    if (! Node.OutEIdV.IsSorted()) {
      const TStr Msg = TStr::Fmt("Out-edge list of node %d is not sorted.", Node.GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
    if (! Node.InEIdV.IsSorted()) {
      const TStr Msg = TStr::Fmt("In-edge list of node %d is not sorted.", Node.GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
    // check out-edge ids
    int prevEId = -1;
    for (int e = 0; e < Node.GetOutDeg(); e++) {
      if (! IsEdge(Node.GetOutEId(e))) {
        const TStr Msg = TStr::Fmt("Out-edge id %d of node %d does not exist.",  Node.GetOutEId(e), Node.GetId());
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      if (e > 0 && prevEId == Node.GetOutEId(e)) {
        const TStr Msg = TStr::Fmt("Node %d has duplidate out-edge id %d.", Node.GetId(), Node.GetOutEId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      prevEId = Node.GetOutEId(e);
    }
    // check in-edge ids
    prevEId = -1;
    for (int e = 0; e < Node.GetInDeg(); e++) {
      if (! IsEdge(Node.GetInEId(e))) {
      const TStr Msg = TStr::Fmt("Out-edge id %d of node %d does not exist.",  Node.GetInEId(e), Node.GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      if (e > 0 && prevEId == Node.GetInEId(e)) {
        const TStr Msg = TStr::Fmt("Node %d has duplidate out-edge id %d.", Node.GetId(), Node.GetInEId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      prevEId = Node.GetInEId(e);
    }
  }
  for (int E = EdgeH.FFirstKeyId(); EdgeH.FNextKeyId(E); ) {
    const TEdge& Edge = EdgeH[E];
    if (! IsNode(Edge.GetSrcNId())) {
      const TStr Msg = TStr::Fmt("Edge %d source node %d does not exist.", Edge.GetId(), Edge.GetSrcNId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
    if (! IsNode(Edge.GetDstNId())) {
      const TStr Msg = TStr::Fmt("Edge %d destination node %d does not exist.", Edge.GetId(), Edge.GetDstNId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
  }
  return RetVal;
}

void TNEANet::Dump(FILE *OutF) const {
  const int NodePlaces = (int) ceil(log10((double) GetNodes()));
  const int EdgePlaces = (int) ceil(log10((double) GetEdges()));
  fprintf(OutF, "-------------------------------------------------\nDirected Node-Edge Network with Attributes: nodes: %d, edges: %d\n", GetNodes(), GetEdges());
  for (TNodeI NodeI = BegNI(); NodeI < EndNI(); NodeI++) {
    fprintf(OutF, "  %*d:", NodePlaces, NodeI.GetId());
    fprintf(OutF, "    in[%d]", NodeI.GetInDeg());
    for (int edge = 0; edge < NodeI.GetInDeg(); edge++) {
      fprintf(OutF, " %*d", EdgePlaces, NodeI.GetInEId(edge)); }
    //fprintf(OutF, "\n");
    fprintf(OutF, "    out[%d]", NodeI.GetOutDeg());
    for (int edge = 0; edge < NodeI.GetOutDeg(); edge++) {
      fprintf(OutF, " %*d", EdgePlaces, NodeI.GetOutEId(edge)); }
    //fprintf(OutF, "\n");

    // load node attributes
    TIntV IntAttrN;
    IntAttrValueNI(NodeI.GetId(), IntAttrN);
    fprintf(OutF, "    nai[%d]", IntAttrN.Len());
    for (int i = 0; i < IntAttrN.Len(); i++) {
      fprintf(OutF, " %*i", NodePlaces, IntAttrN[i]()); }
    //fprintf(OutF, "\n");

    TStrV StrAttrN;
    StrAttrValueNI(NodeI.GetId(), StrAttrN);
    fprintf(OutF, "    nas[%d]", StrAttrN.Len());
    for (int i = 0; i < StrAttrN.Len(); i++) {
      fprintf(OutF, " %*s", NodePlaces, StrAttrN[i]()); }
    //fprintf(OutF, "\n");

    TFltV FltAttrN;
    FltAttrValueNI(NodeI.GetId(), FltAttrN);
    fprintf(OutF, "    naf[%d]", FltAttrN.Len());
    for (int i = 0; i < FltAttrN.Len(); i++) {
      fprintf(OutF, " %*f", NodePlaces, FltAttrN[i]()); }
    //fprintf(OutF, "\n");
    fprintf(OutF, "\n");
  }
  for (TEdgeI EdgeI = BegEI(); EdgeI < EndEI(); EdgeI++) {
    fprintf(OutF, "  %*d:  %*d  ->  %*d", EdgePlaces, EdgeI.GetId(), NodePlaces, EdgeI.GetSrcNId(), NodePlaces, EdgeI.GetDstNId());

    // load edge attributes
    TIntV IntAttrE;
    IntAttrValueEI(EdgeI.GetId(), IntAttrE);
    fprintf(OutF, "    eai[%d]", IntAttrE.Len());
    for (int i = 0; i < IntAttrE.Len(); i++) {
      fprintf(OutF, " %*i", EdgePlaces, IntAttrE[i]()); 
    }
    TStrV StrAttrE;
    StrAttrValueEI(EdgeI.GetId(), StrAttrE);
    fprintf(OutF, "    eas[%d]", StrAttrE.Len());
    for (int i = 0; i < StrAttrE.Len(); i++) {
      fprintf(OutF, " %*s", EdgePlaces, StrAttrE[i]()); 
    }
    TFltV FltAttrE;
    FltAttrValueEI(EdgeI.GetId(), FltAttrE);
    fprintf(OutF, "    eaf[%d]", FltAttrE.Len());
    for (int i = 0; i < FltAttrE.Len(); i++) {
      fprintf(OutF, " %*f", EdgePlaces, FltAttrE[i]()); 
    }
    fprintf(OutF, "\n");
  }
  //fprintf(OutF, "\n");
}

// Attribute related function

int TNEANet::AddIntAttrDatN(const int& NId, const TInt& value, const TStr& attr) {
  int i;
  TInt CurrLen;
  if (!IsNode(NId)) {
    // AddNode(NId);
    return -1;
  }
  if (KeyToIndexTypeN.IsKey(attr)) {
    TVec<TInt>& NewVec = VecOfIntVecsN[KeyToIndexTypeN.GetDat(attr).Val2];
    NewVec[NodeH.GetKeyId(NId)] = value;
  } else {
    CurrLen = VecOfIntVecsN.Len();
    KeyToIndexTypeN.AddDat(attr, TIntPr(IntType, CurrLen));
    TVec<TInt> NewVec = TVec<TInt>();
    for (i = 0; i < MxNId; i++) {
      NewVec.Ins(i, GetIntAttrDefaultN(attr));
    }
    NewVec[NodeH.GetKeyId(NId)] = value;
    VecOfIntVecsN.Add(NewVec);
  }
  return 0;
}

int TNEANet::AddIntVAttrDatN(const int& NId, const TIntV& value, const TStr& attr) {
  TInt CurrLen;
  if (!IsNode(NId)) {
    // AddNode(NId);
    return -1;
  }
  if (KeyToIndexTypeN.IsKey(attr)) {
    TVec<TIntV>& NewVec = VecOfIntVecVecsN[KeyToIndexTypeN.GetDat(attr).Val2];
    NewVec[NodeH.GetKeyId(NId)] = value;
  } else {
    CurrLen = VecOfIntVecVecsN.Len();
    KeyToIndexTypeN.AddDat(attr, TIntPr(IntVType, CurrLen));
    TVec<TIntV> NewVec = TVec<TIntV>(MxNId);
    NewVec[NodeH.GetKeyId(NId)] = value;
    VecOfIntVecVecsN.Add(NewVec);
  }
  return 0;
} 

int TNEANet::AppendIntVAttrDatN(const int& NId, const TInt& value, const TStr& attr) {
  TInt CurrLen;
  if (!IsNode(NId)) {
    // AddNode(NId);
    return -1;
  }
  if (KeyToIndexTypeN.IsKey(attr)) {
    TVec<TIntV>& NewVec = VecOfIntVecVecsN[KeyToIndexTypeN.GetDat(attr).Val2];
    NewVec[NodeH.GetKeyId(NId)].Add(value);
  } else {
    CurrLen = VecOfIntVecVecsN.Len();
    KeyToIndexTypeN.AddDat(attr, TIntPr(IntVType, CurrLen));
    TVec<TIntV> NewVec;
    VecOfIntVecVecsN.Add(NewVec);
    VecOfIntVecVecsN[CurrLen].Gen(MxNId);
    VecOfIntVecVecsN[CurrLen][NodeH.GetKeyId(NId)].Add(value);
  }
  return 0;
} 

int TNEANet::DelFromIntVAttrDatN(const int& NId, const TInt& value, const TStr& attr) {
  TInt CurrLen;
  if (!IsNode(NId)) {
    // AddNode(NId);
    return -1;
  }
  if (KeyToIndexTypeN.IsKey(attr)) {
    TVec<TIntV>& NewVec = VecOfIntVecVecsN[KeyToIndexTypeN.GetDat(attr).Val2];
    if (!NewVec[NodeH.GetKeyId(NId)].DelIfIn(value)) {
      return -1;
    }
  } else {
    return -1;
  }
  return 0;
} 

int TNEANet::AddStrAttrDatN(const int& NId, const TStr& value, const TStr& attr) {
  int i;
  TInt CurrLen;
  if (!IsNode(NId)) {
    // AddNode(NId);
    return -1;
  }
  if (KeyToIndexTypeN.IsKey(attr)) {
    TVec<TStr>& NewVec = VecOfStrVecsN[KeyToIndexTypeN.GetDat(attr).Val2];
    NewVec[NodeH.GetKeyId(NId)] = value;
  } else {
    CurrLen = VecOfStrVecsN.Len();
    KeyToIndexTypeN.AddDat(attr, TIntPr(StrType, CurrLen));
    TVec<TStr> NewVec = TVec<TStr>();
    for (i = 0; i < MxNId; i++) {
        NewVec.Ins(i, GetStrAttrDefaultN(attr));
    }
    NewVec[NodeH.GetKeyId(NId)] = value;
    VecOfStrVecsN.Add(NewVec);
  }
  return 0;
} 

int TNEANet::AddFltAttrDatN(const int& NId, const TFlt& value, const TStr& attr) {
  int i;
  TInt CurrLen;

  if (!IsNode(NId)) {
    // AddNode(NId);
    return -1;
  }
  if (KeyToIndexTypeN.IsKey(attr)) {
    TVec<TFlt>& NewVec = VecOfFltVecsN[KeyToIndexTypeN.GetDat(attr).Val2];
    NewVec[NodeH.GetKeyId(NId)] = value;
  } else {
    CurrLen = VecOfFltVecsN.Len();
    KeyToIndexTypeN.AddDat(attr, TIntPr(FltType, CurrLen));
    TVec<TFlt> NewVec = TVec<TFlt>();
    for (i = 0; i < MxNId; i++) {
      NewVec.Ins(i, GetFltAttrDefaultN(attr));
    }
    NewVec[NodeH.GetKeyId(NId)] = value;
    VecOfFltVecsN.Add(NewVec);
  }
  return 0;
} 

int TNEANet::AddIntAttrDatE(const int& EId, const TInt& value, const TStr& attr) {
  int i;
  TInt CurrLen;
  if (!IsEdge(EId)) {
    //AddEdge(EId);
     return -1;
  }
  if (KeyToIndexTypeE.IsKey(attr)) {
    TVec<TInt>& NewVec = VecOfIntVecsE[KeyToIndexTypeE.GetDat(attr).Val2];
    NewVec[EdgeH.GetKeyId(EId)] = value;
  } else {
    CurrLen = VecOfIntVecsE.Len();
    KeyToIndexTypeE.AddDat(attr, TIntPr(IntType, CurrLen));
    TVec<TInt> NewVec = TVec<TInt>();
    for (i = 0; i < MxEId; i++) {
      NewVec.Ins(i, GetIntAttrDefaultE(attr));
    }
    NewVec[EdgeH.GetKeyId(EId)] = value;
    VecOfIntVecsE.Add(NewVec);
  }
  return 0;
}

int TNEANet::AddIntVAttrDatE(const int& EId, const TIntV& value, const TStr& attr) {
  int i;
  TInt CurrLen;
  if (!IsEdge(EId)) {
    //AddEdge(EId);
     return -1;
  }
  if (KeyToIndexTypeE.IsKey(attr)) {
    TVec<TIntV>& NewVec = VecOfIntVecVecsE[KeyToIndexTypeE.GetDat(attr).Val2];
    NewVec[EdgeH.GetKeyId(EId)] = value;
  } else {
    CurrLen = VecOfIntVecVecsE.Len();
    KeyToIndexTypeE.AddDat(attr, TIntPr(IntVType, CurrLen));
    TVec<TIntV> NewVec = TVec<TIntV>();
    for (i = 0; i < MxEId; i++) {
      NewVec.Ins(i, TIntV());
    }
    NewVec[EdgeH.GetKeyId(EId)] = value;
    VecOfIntVecVecsE.Add(NewVec);
  }
  return 0;
} 

int TNEANet::AppendIntVAttrDatE(const int& EId, const TInt& value, const TStr& attr) {
  TInt CurrLen;
  if (!IsEdge(EId)) {
    //AddEdge(EId);
     return -1;
  }
  if (KeyToIndexTypeE.IsKey(attr)) {
    TVec<TIntV>& NewVec = VecOfIntVecVecsE[KeyToIndexTypeE.GetDat(attr).Val2];
    NewVec[EdgeH.GetKeyId(EId)].Add(value);
  } else {
    return -1;
  }
  return 0;
}

int TNEANet::AddStrAttrDatE(const int& EId, const TStr& value, const TStr& attr) {
  int i;
  TInt CurrLen;
  if (!IsEdge(EId)) {
    //AddEdge(EId);
     return -1;
  }
  if (KeyToIndexTypeE.IsKey(attr)) {
    TVec<TStr>& NewVec = VecOfStrVecsE[KeyToIndexTypeE.GetDat(attr).Val2];
    NewVec[EdgeH.GetKeyId(EId)] = value;
  } else {
    CurrLen = VecOfStrVecsE.Len();
    KeyToIndexTypeE.AddDat(attr, TIntPr(StrType, CurrLen));
    TVec<TStr> NewVec = TVec<TStr>();
    for (i = 0; i < MxEId; i++) {
      NewVec.Ins(i, GetStrAttrDefaultE(attr));
    }
    NewVec[EdgeH.GetKeyId(EId)] = value;
    VecOfStrVecsE.Add(NewVec);
  }
  return 0;
} 

int TNEANet::AddFltAttrDatE(const int& EId, const TFlt& value, const TStr& attr) {
  int i;
  TInt CurrLen;

  if (!IsEdge(EId)) {
    //AddEdge(EId);
     return -1;
  }
  if (KeyToIndexTypeE.IsKey(attr)) {
    TVec<TFlt>& NewVec = VecOfFltVecsE[KeyToIndexTypeE.GetDat(attr).Val2];
    NewVec[EdgeH.GetKeyId(EId)] = value;
  } else {
    CurrLen = VecOfFltVecsE.Len();
    KeyToIndexTypeE.AddDat(attr, TIntPr(FltType, CurrLen));
    TVec<TFlt> NewVec = TVec<TFlt>();
    for (i = 0; i < MxEId; i++) {
      NewVec.Ins(i, GetFltAttrDefaultE(attr));
    }
    NewVec[EdgeH.GetKeyId(EId)] = value;
    VecOfFltVecsE.Add(NewVec);
  }
  return 0;
}

TVec<TFlt>& TNEANet::GetFltAttrVecE(const TStr& attr) {
  return VecOfFltVecsE[KeyToIndexTypeE.GetDat(attr).Val2];
}

int TNEANet::GetFltKeyIdE(const int& EId) {
  return EdgeH.GetKeyId(EId);
}

TInt TNEANet::GetIntAttrDatN(const int& NId, const TStr& attr) {
  return VecOfIntVecsN[KeyToIndexTypeN.GetDat(attr).Val2][NodeH.GetKeyId(NId)];
}

TIntV TNEANet::GetIntVAttrDatN(const int& NId, const TStr& attr) const {
  return VecOfIntVecVecsN[KeyToIndexTypeN.GetDat(attr).Val2][NodeH.GetKeyId(NId)];
}

TStr TNEANet::GetStrAttrDatN(const int& NId, const TStr& attr) {
  return VecOfStrVecsN[KeyToIndexTypeN.GetDat(attr).Val2][NodeH.GetKeyId(NId)];
}

TFlt TNEANet::GetFltAttrDatN(const int& NId, const TStr& attr) {
  return VecOfFltVecsN[KeyToIndexTypeN.GetDat(attr).Val2][NodeH.GetKeyId(NId)];
}

TInt TNEANet::GetIntAttrIndDatN(const int& NId, const int& index) {
  return VecOfIntVecsN[index][NodeH.GetKeyId(NId)];
}

TStr TNEANet::GetStrAttrIndDatN(const int& NId, const int& index) {
  return VecOfStrVecsN[index][NodeH.GetKeyId(NId)];
}

TFlt TNEANet::GetFltAttrIndDatN(const int& NId, const int& index) {
  return VecOfFltVecsN[index][NodeH.GetKeyId(NId)];
}

int TNEANet::GetIntAttrIndN(const TStr& attr) {
  return KeyToIndexTypeN.GetDat(attr).Val2.Val;
}

int TNEANet::GetAttrIndN(const TStr& attr) {
  return KeyToIndexTypeN.GetDat(attr).Val2.Val;
}

TInt TNEANet::GetIntAttrDatE(const int& EId, const TStr& attr) {
  return VecOfIntVecsE[KeyToIndexTypeE.GetDat(attr).Val2][EdgeH.GetKeyId(EId)];
}

TIntV TNEANet::GetIntVAttrDatE(const int& EId, const TStr& attr) {
  return VecOfIntVecVecsE[KeyToIndexTypeE.GetDat(attr).Val2][EdgeH.GetKeyId(EId)];
}

TStr TNEANet::GetStrAttrDatE(const int& EId, const TStr& attr) {
  return VecOfStrVecsE[KeyToIndexTypeE.GetDat(attr).Val2][EdgeH.GetKeyId(EId)];
}

TFlt TNEANet::GetFltAttrDatE(const int& EId, const TStr& attr) {
  return VecOfFltVecsE[KeyToIndexTypeE.GetDat(attr).Val2][EdgeH.GetKeyId(EId)];
}

TInt TNEANet::GetIntAttrIndDatE(const int& EId, const int& index) {
  return VecOfIntVecsE[index][EdgeH.GetKeyId(EId)];
}

TStr TNEANet::GetStrAttrIndDatE(const int& EId, const int& index) {
  return VecOfStrVecsE[index][EdgeH.GetKeyId(EId)];
}

TFlt TNEANet::GetFltAttrIndDatE(const int& EId, const int& index) {
  return VecOfFltVecsE[index][EdgeH.GetKeyId(EId)];
}

int TNEANet::GetIntAttrIndE(const TStr& attr) {
  return KeyToIndexTypeE.GetDat(attr).Val2.Val;
}

int TNEANet::GetAttrIndE(const TStr& attr) {
  return KeyToIndexTypeE.GetDat(attr).Val2.Val;
}

int TNEANet::DelAttrDatN(const int& NId, const TStr& attr) {
  TInt vecType = KeyToIndexTypeN(attr).Val1;
  if (vecType == IntType) {
    VecOfIntVecsN[KeyToIndexTypeN.GetDat(attr).Val2][NodeH.GetKeyId(NId)] = GetIntAttrDefaultN(attr);
  } else if (vecType == StrType) {
    VecOfStrVecsN[KeyToIndexTypeN.GetDat(attr).Val2][NodeH.GetKeyId(NId)] = GetStrAttrDefaultN(attr);
  } else if (vecType == FltType) {
    VecOfFltVecsN[KeyToIndexTypeN.GetDat(attr).Val2][NodeH.GetKeyId(NId)] = GetFltAttrDefaultN(attr);
  } else if (vecType ==IntVType) {
    VecOfIntVecVecsN[KeyToIndexTypeN.GetDat(attr).Val2][NodeH.GetKeyId(NId)] = TIntV();
  } else {
    return -1;
  }
  return 0;
}
             
int TNEANet::DelAttrDatE(const int& EId, const TStr& attr) {
  // TODO(nkhadke): add error checking
  TInt vecType = KeyToIndexTypeE(attr).Val1;
  if (vecType == IntType) {
    VecOfIntVecsE[KeyToIndexTypeE.GetDat(attr).Val2][EdgeH.GetKeyId(EId)] = GetIntAttrDefaultE(attr);
  } else if (vecType == StrType) {
    VecOfStrVecsE[KeyToIndexTypeE.GetDat(attr).Val2][EdgeH.GetKeyId(EId)] = GetStrAttrDefaultE(attr);
  } else if (vecType == FltType) {
    VecOfFltVecsE[KeyToIndexTypeE.GetDat(attr).Val2][EdgeH.GetKeyId(EId)] = GetFltAttrDefaultE(attr);
  } else if (vecType == IntVType) {
    VecOfIntVecVecsE[KeyToIndexTypeE.GetDat(attr).Val2][EdgeH.GetKeyId(EId)] = TIntV();
  } else {
    return -1;
  }
  return 0;
}

int TNEANet::AddIntAttrN(const TStr& attr, TInt defaultValue){
  int i;
  TInt CurrLen;
  TVec<TInt> NewVec;
  CurrLen = VecOfIntVecsN.Len();
  KeyToIndexTypeN.AddDat(attr, TIntPr(IntType, CurrLen));
  NewVec = TVec<TInt>();
  for (i = 0; i < MxNId; i++) {
    NewVec.Ins(i, defaultValue);
  }
  VecOfIntVecsN.Add(NewVec);
  if (!IntDefaultsN.IsKey(attr)) {
    IntDefaultsN.AddDat(attr, defaultValue);
  } else {
    return -1;
  }
  return 0;
}

int TNEANet::AddIntVAttrN(const TStr& attr){
  TInt CurrLen;
  TVec<TIntV> NewVec;
  CurrLen = VecOfIntVecVecsN.Len();
  KeyToIndexTypeN.AddDat(attr, TIntPr(IntVType, CurrLen));
  NewVec = TVec<TIntV>(MxNId);
  VecOfIntVecVecsN.Add(NewVec);
  return 0;
}

int TNEANet::AddStrAttrN(const TStr& attr, TStr defaultValue) {
  int i;
  TInt CurrLen;
  TVec<TStr> NewVec;
  CurrLen = VecOfStrVecsN.Len();
  KeyToIndexTypeN.AddDat(attr, TIntPr(StrType, CurrLen));
  NewVec = TVec<TStr>();
  for (i = 0; i < MxNId; i++) {
    NewVec.Ins(i, defaultValue);
  }
  VecOfStrVecsN.Add(NewVec);
  if (!StrDefaultsN.IsKey(attr)) {
    StrDefaultsN.AddDat(attr, defaultValue);
  } else {
    return -1;
  }
  return 0;
}

int TNEANet::AddFltAttrN(const TStr& attr, TFlt defaultValue) {
  // TODO(nkhadke): add error checking
  int i;
  TInt CurrLen;
  TVec<TFlt> NewVec;
  CurrLen = VecOfFltVecsN.Len();
  KeyToIndexTypeN.AddDat(attr, TIntPr(FltType, CurrLen));
  NewVec = TVec<TFlt>();
  for (i = 0; i < MxNId; i++) {
    NewVec.Ins(i, defaultValue);
  }
  VecOfFltVecsN.Add(NewVec);
  if (!FltDefaultsN.IsKey(attr)) {
    FltDefaultsN.AddDat(attr, defaultValue);
  } else {
    return -1;
  }
  return 0;
}

int TNEANet::AddIntAttrE(const TStr& attr, TInt defaultValue){
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

int TNEANet::AddIntVAttrE(const TStr& attr){
  int i;
  TInt CurrLen;
  TVec<TIntV> NewVec;
  CurrLen = VecOfIntVecVecsE.Len();
  KeyToIndexTypeE.AddDat(attr, TIntPr(IntVType, CurrLen));
  NewVec = TVec<TIntV>();
  for (i = 0; i < MxEId; i++) {
    NewVec.Ins(i, TIntV());
  }
  VecOfIntVecVecsE.Add(NewVec);
  return 0;
}

int TNEANet::AddStrAttrE(const TStr& attr, TStr defaultValue) {
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

int TNEANet::AddFltAttrE(const TStr& attr, TFlt defaultValue) {
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

int TNEANet::DelAttrN(const TStr& attr) {
  TInt vecType = KeyToIndexTypeN(attr).Val1;
  if (vecType == IntType) {
    VecOfIntVecsN[KeyToIndexTypeN.GetDat(attr).Val2] = TVec<TInt>();
    if (IntDefaultsN.IsKey(attr)) {
      IntDefaultsN.DelKey(attr);
    }
  } else if (vecType == StrType) {
    VecOfStrVecsN[KeyToIndexTypeN.GetDat(attr).Val2] = TVec<TStr>();  
    if (StrDefaultsN.IsKey(attr)) {
      StrDefaultsN.DelKey(attr);
    }
  } else if (vecType == FltType) {
    VecOfFltVecsN[KeyToIndexTypeN.GetDat(attr).Val2] = TVec<TFlt>();
    if (FltDefaultsN.IsKey(attr)) {
      FltDefaultsN.DelKey(attr);
    }
  } else if (vecType == IntVType) {
    VecOfIntVecVecsN[KeyToIndexTypeN.GetDat(attr).Val2] = TVec<TIntV>();
  } else {
    return -1;
  }
  KeyToIndexTypeN.DelKey(attr);
  return 0;
}

int TNEANet::DelAttrE(const TStr& attr) {
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

// Return a small graph on 5 nodes and 6 edges.
PNEANet TNEANet::GetSmallGraph() {
  PNEANet Net = TNEANet::New();
  for (int i = 0; i < 5; i++) { Net->AddNode(i); }
  Net->AddEdge(0,1);  Net->AddEdge(0,2);
  Net->AddEdge(0,3);  Net->AddEdge(0,4);
  Net->AddEdge(1,2);  Net->AddEdge(1,2);
  return Net;
}

void TNEANet::GetAttrNNames(TStrV& IntAttrNames, TStrV& FltAttrNames, TStrV& StrAttrNames) const {
  for (TStrIntPrH::TIter it = KeyToIndexTypeN.BegI(); it < KeyToIndexTypeN.EndI(); it++) {
    if (it.GetDat().GetVal1() == IntType) {
      IntAttrNames.Add(it.GetKey());
    }
    if (it.GetDat().GetVal1() == FltType) {
      FltAttrNames.Add(it.GetKey());
    }
    if (it.GetDat().GetVal1() == StrType) {
      StrAttrNames.Add(it.GetKey());
    }
  }
}

void TNEANet::GetAttrENames(TStrV& IntAttrNames, TStrV& FltAttrNames, TStrV& StrAttrNames) const {
  for (TStrIntPrH::TIter it = KeyToIndexTypeE.BegI(); it < KeyToIndexTypeE.EndI(); it++) {
    if (it.GetDat().GetVal1() == IntType) {
      IntAttrNames.Add(it.GetKey());
    }
    if (it.GetDat().GetVal1() == FltType) {
      FltAttrNames.Add(it.GetKey());
    }
    if (it.GetDat().GetVal1() == StrType) {
      StrAttrNames.Add(it.GetKey());
    }
  }
}

TFlt TNEANet::GetWeightOutEdges(const TNodeI& NI, const TStr& attr) {
  TNode Node = GetNode(NI.GetId());
  TIntV OutEIdV = Node.OutEIdV;
  TFlt total = 0;
  int len = Node.OutEIdV.Len();
  for (int i = 0; i < len; i++) {
    total += GetFltAttrDatE(Node.OutEIdV[i], attr);
  }
  return total;
}

void TNEANet::GetWeightOutEdgesV(TFltV& OutWeights, const TFltV& AttrVal) {
  for (TEdgeI it = BegEI(); it < EndEI(); it++) {
    int EId = it.GetId();
    int SrcId = it.GetSrcNId();
    OutWeights[SrcId] +=AttrVal[GetFltKeyIdE(EId)];
  }
}

bool TNEANet::IsFltAttrE(const TStr& attr) {
  return (KeyToIndexTypeE.IsKey(attr) &&
    KeyToIndexTypeE.GetDat(attr).Val1 == FltType);
}

bool TNEANet::IsIntAttrE(const TStr& attr) {
  return (KeyToIndexTypeE.IsKey(attr) &&
    KeyToIndexTypeE.GetDat(attr).Val1 == IntType);
}

bool TNEANet::IsStrAttrE(const TStr& attr) {
  return (KeyToIndexTypeE.IsKey(attr) &&
    KeyToIndexTypeE.GetDat(attr).Val1 == StrType);
}

int TNEANet::AddSAttrDatN(const TInt& NId, const TStr& AttrName, const TInt& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrName, Val);
}

int TNEANet::AddSAttrDatN(const TInt& NId, const TInt& AttrId, const TInt& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrId, Val);
}

int TNEANet::AddSAttrDatN(const TInt& NId, const TStr& AttrName, const TFlt& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrName, Val);
}

int TNEANet::AddSAttrDatN(const TInt& NId, const TInt& AttrId, const TFlt& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrId, Val);
}

int TNEANet::AddSAttrDatN(const TInt& NId, const TStr& AttrName, const TStr& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrName, Val);
}

int TNEANet::AddSAttrDatN(const TInt& NId, const TInt& AttrId, const TStr& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrId, Val);
}

int TNEANet::GetSAttrDatN(const TInt& NId, const TStr& AttrName, TInt& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrName, Val);
}

int TNEANet::GetSAttrDatN(const TInt& NId, const TInt& AttrId, TInt& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrId, Val);
}

int TNEANet::GetSAttrDatN(const TInt& NId, const TStr& AttrName, TFlt& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrName, Val);
}

int TNEANet::GetSAttrDatN(const TInt& NId, const TInt& AttrId, TFlt& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrId, Val);
}

int TNEANet::GetSAttrDatN(const TInt& NId, const TStr& AttrName, TStr& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrName, Val);
}

int TNEANet::GetSAttrDatN(const TInt& NId, const TInt& AttrId, TStr& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrId, Val);
}

int TNEANet::DelSAttrDatN(const TInt& NId, const TStr& AttrName) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.DelSAttrDat(NId, AttrName);
}

int TNEANet::DelSAttrDatN(const TInt& NId, const TInt& AttrId) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.DelSAttrDat(NId, AttrId);
}

int TNEANet::GetSAttrVN(const TInt& NId, const TAttrType AttrType, TAttrPrV& AttrV) const {
  if (!IsNode(NId)) {
    return -1;
  }
  SAttrN.GetSAttrV(NId, AttrType, AttrV);
  return 0;
}

int TNEANet::GetIdVSAttrN(const TStr& AttrName, TIntV& IdV) const {
  return SAttrN.GetIdVSAttr(AttrName, IdV);
}

int TNEANet::GetIdVSAttrN(const TInt& AttrId, TIntV& IdV) const {
  return SAttrN.GetIdVSAttr(AttrId, IdV);
}

int TNEANet::AddSAttrN(const TStr& Name, const TAttrType& AttrType, TInt& AttrId) {
  return SAttrN.AddSAttr(Name, AttrType, AttrId);
}

int TNEANet::GetSAttrIdN(const TStr& Name, TInt& AttrId, TAttrType& AttrType) const {
  return SAttrN.GetSAttrId(Name, AttrId, AttrType);
}

int TNEANet::GetSAttrNameN(const TInt& AttrId, TStr& Name, TAttrType& AttrType) const {
  return SAttrN.GetSAttrName(AttrId, Name, AttrType);
}

int TNEANet::AddSAttrDatE(const TInt& EId, const TStr& AttrName, const TInt& Val) {
  if (!IsEdge(EId)) {
    return -1;
  }
  return SAttrE.AddSAttrDat(EId, AttrName, Val);
}

int TNEANet::AddSAttrDatE(const TInt& EId, const TInt& AttrId, const TInt& Val) {
  if (!IsEdge(EId)) {
    return -1;
  }
  return SAttrE.AddSAttrDat(EId, AttrId, Val);
}

int TNEANet::AddSAttrDatE(const TInt& EId, const TStr& AttrName, const TFlt& Val) {
  if (!IsEdge(EId)) {
    return -1;
  }
  return SAttrE.AddSAttrDat(EId, AttrName, Val);
}

int TNEANet::AddSAttrDatE(const TInt& EId, const TInt& AttrId, const TFlt& Val) {
  if (!IsEdge(EId)) {
    return -1;
  }
  return SAttrE.AddSAttrDat(EId, AttrId, Val);
}

int TNEANet::AddSAttrDatE(const TInt& EId, const TStr& AttrName, const TStr& Val) {
  if (!IsEdge(EId)) {
    return -1;
  }
  return SAttrE.AddSAttrDat(EId, AttrName, Val);
}

int TNEANet::AddSAttrDatE(const TInt& EId, const TInt& AttrId, const TStr& Val) {
  if (!IsEdge(EId)) {
    return -1;
  }
  return SAttrE.AddSAttrDat(EId, AttrId, Val);
}

int TNEANet::GetSAttrDatE(const TInt& EId, const TStr& AttrName, TInt& Val) const {
  if (!IsEdge(EId)) {
    return -1;
  }
  return SAttrE.GetSAttrDat(EId, AttrName, Val);
}

int TNEANet::GetSAttrDatE(const TInt& EId, const TInt& AttrId, TInt& Val) const {
  if (!IsEdge(EId)) {
    return -1;
  }
  return SAttrE.GetSAttrDat(EId, AttrId, Val);
}

int TNEANet::GetSAttrDatE(const TInt& EId, const TStr& AttrName, TFlt& Val) const {
  if (!IsEdge(EId)) {
    return -1;
  }
  return SAttrE.GetSAttrDat(EId, AttrName, Val);
}

int TNEANet::GetSAttrDatE(const TInt& EId, const TInt& AttrId, TFlt& Val) const {
  if (!IsEdge(EId)) {
    return -1;
  }
  return SAttrE.GetSAttrDat(EId, AttrId, Val);
}

int TNEANet::GetSAttrDatE(const TInt& EId, const TStr& AttrName, TStr& Val) const {
  if (!IsEdge(EId)) {
    return -1;
  }
  return SAttrE.GetSAttrDat(EId, AttrName, Val);
}

int TNEANet::GetSAttrDatE(const TInt& EId, const TInt& AttrId, TStr& Val) const {
  if (!IsEdge(EId)) {
    return -1;
  }
  return SAttrE.GetSAttrDat(EId, AttrId, Val);
}

int TNEANet::DelSAttrDatE(const TInt& EId, const TStr& AttrName) {
  if (!IsEdge(EId)) {
    return -1;
  }
  return SAttrE.DelSAttrDat(EId, AttrName);
}

int TNEANet::DelSAttrDatE(const TInt& EId, const TInt& AttrId) {
  if (!IsEdge(EId)) {
    return -1;
  }
  return SAttrE.DelSAttrDat(EId, AttrId);
}

int TNEANet::GetSAttrVE(const TInt& EId, const TAttrType AttrType, TAttrPrV& AttrV) const {
  if (!IsEdge(EId)) {
    return -1;
  }
  SAttrE.GetSAttrV(EId, AttrType, AttrV);
  return 0;
}

int TNEANet::GetIdVSAttrE(const TStr& AttrName, TIntV& IdV) const {
  return SAttrE.GetIdVSAttr(AttrName, IdV);
}

int TNEANet::GetIdVSAttrE(const TInt& AttrId, TIntV& IdV) const {
  return SAttrE.GetIdVSAttr(AttrId, IdV);
}

int TNEANet::AddSAttrE(const TStr& Name, const TAttrType& AttrType, TInt& AttrId) {
  return SAttrE.AddSAttr(Name, AttrType, AttrId);
}

int TNEANet::GetSAttrIdE(const TStr& Name, TInt& AttrId, TAttrType& AttrType) const {
  return SAttrE.GetSAttrId(Name, AttrId, AttrType);
}

int TNEANet::GetSAttrNameE(const TInt& AttrId, TStr& Name, TAttrType& AttrType) const {
  return SAttrE.GetSAttrName(AttrId, Name, AttrType);
}

/////////////////////////////////////////////////
// Undirected Graph
bool TUndirNet::HasFlag(const TGraphFlag& Flag) const {
  return HasGraphFlag(TUndirNet::TNet, Flag);
}

// Add a node of ID NId to the graph.
int TUndirNet::AddNode(int NId) {
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    MxNId = TMath::Mx(NId+1, MxNId());
  }
  NodeH.AddDat(NId, TNode(NId));
  return NId;
}

int TUndirNet::AddNodeUnchecked(int NId) {
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    if (IsNode(NId)) { return -1;}
    MxNId = TMath::Mx(NId+1, MxNId());
  }
  NodeH.AddDat(NId, TNode(NId));
  return NId;
}

// Add a node of ID NId to the graph and create edges to all nodes in vector NbrNIdV.
int TUndirNet::AddNode(const int& NId, const TIntV& NbrNIdV) {
  int NewNId;
  if (NId == -1) {
    NewNId = MxNId;  MxNId++;
  } else {
    IAssertR(! IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    NewNId = NId;
    MxNId = TMath::Mx(NewNId+1, MxNId());
  }
  TNode& Node = NodeH.AddDat(NewNId);
  Node.Id = NewNId;
  Node.NIdV = NbrNIdV;
  Node.NIdV.Sort();
  NEdges += Node.GetDeg();
  for (int i = 0; i < NbrNIdV.Len(); i++) {
    GetNode(NbrNIdV[i]).NIdV.AddSorted(NewNId);
  }
  return NewNId;
}

// Add a node of ID NId to the graph and create edges to all nodes in the vector NIdVId in the vector pool Pool).
int TUndirNet::AddNode(const int& NId, const TVecPool<TInt>& Pool, const int& NIdVId) {
  int NewNId;
  if (NId == -1) {
    NewNId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    NewNId = NId;
    MxNId = TMath::Mx(NewNId+1, MxNId()); 
  }
  TNode& Node = NodeH.AddDat(NewNId);
  Node.Id = NewNId;
  Node.NIdV.GenExt(Pool.GetValVPt(NIdVId), Pool.GetVLen(NIdVId));
  Node.NIdV.Sort();
  NEdges += Node.GetDeg();
  return NewNId;
}

// Delete node of ID NId from the graph.
void TUndirNet::DelNode(const int& NId) {
  { AssertR(IsNode(NId), TStr::Fmt("NodeId %d does not exist", NId));
  TInt Id(NId);
  SAttrN.DelSAttrId(Id);
  TNode& Node = GetNode(NId);
  NEdges -= Node.GetDeg();
  for (int e = 0; e < Node.GetDeg(); e++) {
    const int nbr = Node.GetNbrNId(e);
    if (nbr == NId) { continue; }
    TNode& N = GetNode(nbr);
    const int n = N.NIdV.SearchBin(NId);
    IAssert(n != -1); // if NId points to N, then N also should point back
    if (n!= -1) { N.NIdV.Del(n); }
  } }
  NodeH.DelKey(NId);
}

int TUndirNet::GetEdges() const {
  //int Edges = 0;
  //for (int N=NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
  //  Edges += NodeH[N].GetDeg();
  //}
  //return Edges/2;
  return NEdges;
}

// Add an edge between SrcNId and DstNId to the graph.
int TUndirNet::AddEdge(const int& SrcNId, const int& DstNId) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  if (IsEdge(SrcNId, DstNId)) { return -2; } // edge already exists
  GetNode(SrcNId).NIdV.AddSorted(DstNId);
  if (SrcNId!=DstNId) { // not a self edge
    GetNode(DstNId).NIdV.AddSorted(SrcNId); }
  NEdges++;
  return -1; // no edge id
}

// Add an edge between SrcNId and DstNId to the graph.
int TUndirNet::AddEdgeUnchecked(const int& SrcNId, const int& DstNId) {
  GetNode(SrcNId).NIdV.Add(DstNId);
  if (SrcNId!=DstNId) { // not a self edge
    GetNode(DstNId).NIdV.Add(SrcNId); }
  NEdges++;
  return -1; // no edge id
}

// Delete an edge between node IDs SrcNId and DstNId from the graph.
void TUndirNet::DelEdge(const int& SrcNId, const int& DstNId) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  { TNode& N = GetNode(SrcNId);
  const int n = N.NIdV.SearchBin(DstNId);
  TIntPr Id(SrcNId, DstNId);
  SAttrE.DelSAttrId(Id);
  if (n!= -1) { N.NIdV.Del(n);  NEdges--; } }
  if (SrcNId != DstNId) { // not a self edge
    TNode& N = GetNode(DstNId);
    const int n = N.NIdV.SearchBin(SrcNId);
    if (n!= -1) { N.NIdV.Del(n); }
  }
}

// Test whether an edge between node IDs SrcNId and DstNId exists the graph.
bool TUndirNet::IsEdge(const int& SrcNId, const int& DstNId) const {
  if (! IsNode(SrcNId) || ! IsNode(DstNId)) return false;
  return GetNode(SrcNId).IsNbrNId(DstNId);
}

// Return an iterator referring to edge (SrcNId, DstNId) in the graph.
TUndirNet::TEdgeI TUndirNet::GetEI(const int& SrcNId, const int& DstNId) const {
  const int MnNId = TMath::Mn(SrcNId, DstNId);
  const int MxNId = TMath::Mx(SrcNId, DstNId);
  const TNodeI SrcNI = GetNI(MnNId);
  const int NodeN = SrcNI.NodeHI.GetDat().NIdV.SearchBin(MxNId);
  IAssert(NodeN != -1);
  return TEdgeI(SrcNI, EndNI(), NodeN);
}


// Get a vector IDs of all nodes in the graph.
void TUndirNet::GetNIdV(TIntV& NIdV) const {
  NIdV.Gen(GetNodes(), 0);
  for (int N=NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    NIdV.Add(NodeH.GetKey(N)); }
}

// Defragment the graph.
void TUndirNet::Defrag(const bool& OnlyNodeLinks) {
  for (int n = NodeH.FFirstKeyId(); NodeH.FNextKeyId(n); ) {
    NodeH[n].NIdV.Pack();
  }
  if (! OnlyNodeLinks && ! NodeH.IsKeyIdEqKeyN()) {
    NodeH.Defrag();
  }
}

// Check the graph data structure for internal consistency.
bool TUndirNet::IsOk(const bool& ThrowExcept) const {
  bool RetVal = true;
  for (int N = NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    const TNode& Node = NodeH[N];
    if (! Node.NIdV.IsSorted()) {
      const TStr Msg = TStr::Fmt("Neighbor list of node %d is not sorted.", Node.GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); }
      RetVal=false;
    }
    int prevNId = -1;
    for (int e = 0; e < Node.GetDeg(); e++) {
      if (! IsNode(Node.GetNbrNId(e))) {
        const TStr Msg = TStr::Fmt("Edge %d --> %d: node %d does not exist.",
          Node.GetId(), Node.GetNbrNId(e), Node.GetNbrNId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); }
        RetVal=false;
      }
      if (e > 0 && prevNId == Node.GetNbrNId(e)) {
        const TStr Msg = TStr::Fmt("Node %d has duplicate edge %d --> %d.",
          Node.GetId(), Node.GetId(), Node.GetNbrNId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); }
        RetVal=false;
      }
      prevNId = Node.GetNbrNId(e);
    }
  }
  int EdgeCnt = 0;
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) { EdgeCnt++; }
  if (EdgeCnt != GetEdges()) {
    const TStr Msg = TStr::Fmt("Number of edges counter is corrupted: GetEdges():%d, EdgeCount:%d.", GetEdges(), EdgeCnt);
    if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); }
    RetVal=false;
  }
  return RetVal;
}

// Print the graph in a human readable form to an output stream OutF.
void TUndirNet::Dump(FILE *OutF) const {
  const int NodePlaces = (int) ceil(log10((double) GetNodes()));
  fprintf(OutF, "-------------------------------------------------\nUndirected Node Graph: nodes: %d, edges: %d\n", GetNodes(), GetEdges());
  for (int N = NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    const TNode& Node = NodeH[N];
    fprintf(OutF, "  %*d [%d] ", NodePlaces, Node.GetId(), Node.GetDeg());
    for (int edge = 0; edge < Node.GetDeg(); edge++) {
      fprintf(OutF, " %*d", NodePlaces, Node.GetNbrNId(edge)); }
    fprintf(OutF, "\n");
  }
  fprintf(OutF, "\n");
}

// Return a small graph on 5 nodes and 5 edges.
PUndirNet TUndirNet::GetSmallGraph() {
  PUndirNet Graph = TUndirNet::New();
  for (int i = 0; i < 5; i++) { Graph->AddNode(i); }
  Graph->AddEdge(0,1);  Graph->AddEdge(0,2);
  Graph->AddEdge(0,3);  Graph->AddEdge(0,4);
  Graph->AddEdge(1,2);
  return Graph;
}

int TUndirNet::AddSAttrDatN(const TInt& NId, const TStr& AttrName, const TInt& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrName, Val);
}

int TUndirNet::AddSAttrDatN(const TInt& NId, const TInt& AttrId, const TInt& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrId, Val);
}

int TUndirNet::AddSAttrDatN(const TInt& NId, const TStr& AttrName, const TFlt& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrName, Val);
}

int TUndirNet::AddSAttrDatN(const TInt& NId, const TInt& AttrId, const TFlt& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrId, Val);
}

int TUndirNet::AddSAttrDatN(const TInt& NId, const TStr& AttrName, const TStr& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrName, Val);
}

int TUndirNet::AddSAttrDatN(const TInt& NId, const TInt& AttrId, const TStr& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrId, Val);
}

int TUndirNet::GetSAttrDatN(const TInt& NId, const TStr& AttrName, TInt& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrName, Val);
}

int TUndirNet::GetSAttrDatN(const TInt& NId, const TInt& AttrId, TInt& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrId, Val);
}

int TUndirNet::GetSAttrDatN(const TInt& NId, const TStr& AttrName, TFlt& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrName, Val);
}

int TUndirNet::GetSAttrDatN(const TInt& NId, const TInt& AttrId, TFlt& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrId, Val);
}

int TUndirNet::GetSAttrDatN(const TInt& NId, const TStr& AttrName, TStr& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrName, Val);
}

int TUndirNet::GetSAttrDatN(const TInt& NId, const TInt& AttrId, TStr& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrId, Val);
}

int TUndirNet::DelSAttrDatN(const TInt& NId, const TStr& AttrName) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.DelSAttrDat(NId, AttrName);
}

int TUndirNet::DelSAttrDatN(const TInt& NId, const TInt& AttrId) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.DelSAttrDat(NId, AttrId);
}

int TUndirNet::GetSAttrVN(const TInt& NId, const TAttrType AttrType, TAttrPrV& AttrV) const {
  if (!IsNode(NId)) {
    return -1;
  }
  SAttrN.GetSAttrV(NId, AttrType, AttrV);
  return 0;
}

int TUndirNet::GetIdVSAttrN(const TStr& AttrName, TIntV& IdV) const {
  return SAttrN.GetIdVSAttr(AttrName, IdV);
}

int TUndirNet::GetIdVSAttrN(const TInt& AttrId, TIntV& IdV) const {
  return SAttrN.GetIdVSAttr(AttrId, IdV);
}

int TUndirNet::AddSAttrN(const TStr& Name, const TAttrType& AttrType, TInt& AttrId) {
  return SAttrN.AddSAttr(Name, AttrType, AttrId);
}

int TUndirNet::GetSAttrIdN(const TStr& Name, TInt& AttrId, TAttrType& AttrType) const {
  return SAttrN.GetSAttrId(Name, AttrId, AttrType);
}

int TUndirNet::GetSAttrNameN(const TInt& AttrId, TStr& Name, TAttrType& AttrType) const {
  return SAttrN.GetSAttrName(AttrId, Name, AttrType);
}

TIntPr TUndirNet::OrderEdgeNodes(const int& SrcNId, const int& DstNId) const{
  if (SrcNId < DstNId) {
    return TIntPr(SrcNId, DstNId);
  }
  return TIntPr(DstNId, SrcNId);
}

int TUndirNet::AddSAttrDatE(const int& SrcNId, const int& DstNId, const TStr& AttrName, const TInt& Val) {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TIntPr EId = OrderEdgeNodes(SrcNId, DstNId);
  return SAttrE.AddSAttrDat(EId, AttrName, Val);
}

int TUndirNet::AddSAttrDatE(const int& SrcNId, const int& DstNId, const TInt& AttrId, const TInt& Val) {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TIntPr EId = OrderEdgeNodes(SrcNId, DstNId);
  return SAttrE.AddSAttrDat(EId, AttrId, Val);
}

int TUndirNet::AddSAttrDatE(const int& SrcNId, const int& DstNId, const TStr& AttrName, const TFlt& Val) {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TIntPr EId = OrderEdgeNodes(SrcNId, DstNId);
  return SAttrE.AddSAttrDat(EId, AttrName, Val);
}

int TUndirNet::AddSAttrDatE(const int& SrcNId, const int& DstNId, const TInt& AttrId, const TFlt& Val) {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TIntPr EId = OrderEdgeNodes(SrcNId, DstNId);
  return SAttrE.AddSAttrDat(EId, AttrId, Val);
}

int TUndirNet::AddSAttrDatE(const int& SrcNId, const int& DstNId, const TStr& AttrName, const TStr& Val) {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TIntPr EId = OrderEdgeNodes(SrcNId, DstNId);
  return SAttrE.AddSAttrDat(EId, AttrName, Val);
}

int TUndirNet::AddSAttrDatE(const int& SrcNId, const int& DstNId, const TInt& AttrId, const TStr& Val) {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TIntPr EId = OrderEdgeNodes(SrcNId, DstNId);
  return SAttrE.AddSAttrDat(EId, AttrId, Val);
}

int TUndirNet::GetSAttrDatE(const int& SrcNId, const int& DstNId, const TStr& AttrName, TInt& Val) const {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TIntPr EId = OrderEdgeNodes(SrcNId, DstNId);
  return SAttrE.GetSAttrDat(EId, AttrName, Val);
}

int TUndirNet::GetSAttrDatE(const int& SrcNId, const int& DstNId, const TInt& AttrId, TInt& Val) const {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TIntPr EId = OrderEdgeNodes(SrcNId, DstNId);
  return SAttrE.GetSAttrDat(EId, AttrId, Val);
}

int TUndirNet::GetSAttrDatE(const int& SrcNId, const int& DstNId, const TStr& AttrName, TFlt& Val) const {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TIntPr EId = OrderEdgeNodes(SrcNId, DstNId);
  return SAttrE.GetSAttrDat(EId, AttrName, Val);
}

int TUndirNet::GetSAttrDatE(const int& SrcNId, const int& DstNId, const TInt& AttrId, TFlt& Val) const {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TIntPr EId = OrderEdgeNodes(SrcNId, DstNId);
  return SAttrE.GetSAttrDat(EId, AttrId, Val);
}

int TUndirNet::GetSAttrDatE(const int& SrcNId, const int& DstNId, const TStr& AttrName, TStr& Val) const {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TIntPr EId = OrderEdgeNodes(SrcNId, DstNId);
  return SAttrE.GetSAttrDat(EId, AttrName, Val);
}

int TUndirNet::GetSAttrDatE(const int& SrcNId, const int& DstNId, const TInt& AttrId, TStr& Val) const {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TIntPr EId = OrderEdgeNodes(SrcNId, DstNId);
  return SAttrE.GetSAttrDat(EId, AttrId, Val);
}

int TUndirNet::DelSAttrDatE(const int& SrcNId, const int& DstNId, const TStr& AttrName) {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TIntPr EId = OrderEdgeNodes(SrcNId, DstNId);
  return SAttrE.DelSAttrDat(EId, AttrName);
}

int TUndirNet::DelSAttrDatE(const int& SrcNId, const int& DstNId, const TInt& AttrId) {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TIntPr EId = OrderEdgeNodes(SrcNId, DstNId);
  return SAttrE.DelSAttrDat(EId, AttrId);
}

int TUndirNet::GetSAttrVE(const int& SrcNId, const int& DstNId, const TAttrType AttrType, TAttrPrV& AttrV) const {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TIntPr EId = OrderEdgeNodes(SrcNId, DstNId);
  SAttrE.GetSAttrV(EId, AttrType, AttrV);
  return 0;
}

int TUndirNet::GetIdVSAttrE(const TStr& AttrName, TIntPrV& IdV) const {
  return SAttrE.GetIdVSAttr(AttrName, IdV);
}

int TUndirNet::GetIdVSAttrE(const TInt& AttrId, TIntPrV& IdV) const {
  return SAttrE.GetIdVSAttr(AttrId, IdV);
}

int TUndirNet::AddSAttrE(const TStr& Name, const TAttrType& AttrType, TInt& AttrId) {
  return SAttrE.AddSAttr(Name, AttrType, AttrId);
}

int TUndirNet::GetSAttrIdE(const TStr& Name, TInt& AttrId, TAttrType& AttrType) const {
  return SAttrE.GetSAttrId(Name, AttrId, AttrType);
}

int TUndirNet::GetSAttrNameE(const TInt& AttrId, TStr& Name, TAttrType& AttrType) const {
  return SAttrE.GetSAttrName(AttrId, Name, AttrType);
}

/////////////////////////////////////////////////
// Directed Node Graph
bool TDirNet::HasFlag(const TGraphFlag& Flag) const {
  return HasGraphFlag(TDirNet::TNet, Flag);
}

int TDirNet::AddNode(int NId) {
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    MxNId = TMath::Mx(NId+1, MxNId());
  }
  NodeH.AddDat(NId, TNode(NId));
  return NId;
}

int TDirNet::AddNodeUnchecked(int NId) {
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    if (IsNode(NId)) { return -1;}
    MxNId = TMath::Mx(NId+1, MxNId());
  }
  NodeH.AddDat(NId, TNode(NId));
  return NId;
}

// add a node with a list of neighbors
// (use TDirNet::IsOk to check whether the graph is consistent)
int TDirNet::AddNode(const int& NId, const TIntV& InNIdV, const TIntV& OutNIdV) {
  int NewNId;
  if (NId == -1) {
    NewNId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    NewNId = NId;
    MxNId = TMath::Mx(NewNId+1, MxNId());
  }
  TNode& Node = NodeH.AddDat(NewNId);
  Node.Id = NewNId;
  Node.InNIdV = InNIdV;
  Node.OutNIdV = OutNIdV;
  Node.InNIdV.Sort();
  Node.OutNIdV.Sort();
  return NewNId;
}

// add a node from a vector pool
// (use TDirNet::IsOk to check whether the graph is consistent)
int TDirNet::AddNode(const int& NId, const TVecPool<TInt>& Pool, const int& SrcVId, const int& DstVId) {
  int NewNId;
  if (NId == -1) {
    NewNId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    NewNId = NId;
    MxNId = TMath::Mx(NewNId+1, MxNId());
  }
  TNode& Node = NodeH.AddDat(NewNId);
  Node.Id = NewNId;
  Node.InNIdV.GenExt(Pool.GetValVPt(SrcVId), Pool.GetVLen(SrcVId));
  Node.OutNIdV.GenExt(Pool.GetValVPt(DstVId), Pool.GetVLen(DstVId));
  Node.InNIdV.Sort();
  Node.OutNIdV.Sort();
  return NewNId;
}

void TDirNet::DelNode(const int& NId) {
  { TNode& Node = GetNode(NId);
  TInt Id(NId);
  SAttrN.DelSAttrId(Id);
  for (int e = 0; e < Node.GetOutDeg(); e++) {
  const int nbr = Node.GetOutNId(e);
  if (nbr == NId) { continue; }
    TNode& N = GetNode(nbr);
    const int n = N.InNIdV.SearchBin(NId);
    if (n!= -1) { N.InNIdV.Del(n); }
  }
  for (int e = 0; e < Node.GetInDeg(); e++) {
  const int nbr = Node.GetInNId(e);
  if (nbr == NId) { continue; }
    TNode& N = GetNode(nbr);
    const int n = N.OutNIdV.SearchBin(NId);
    if (n!= -1) { N.OutNIdV.Del(n); }
  } }
  NodeH.DelKey(NId);
}

int TDirNet::GetEdges() const {
  int edges=0;
  for (int N=NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    edges+=NodeH[N].GetOutDeg();
  }
  return edges;
}

int TDirNet::AddEdge(const int& SrcNId, const int& DstNId) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  //IAssert(! IsEdge(SrcNId, DstNId));
  if (IsEdge(SrcNId, DstNId)) { return -2; }
  GetNode(SrcNId).OutNIdV.AddSorted(DstNId);
  GetNode(DstNId).InNIdV.AddSorted(SrcNId);
  return -1; // no edge id
}

int TDirNet::AddEdgeUnchecked(const int& SrcNId, const int& DstNId) {
  GetNode(SrcNId).OutNIdV.Add(DstNId);
  GetNode(DstNId).InNIdV.Add(SrcNId);
  return -1; // no edge id
}

void TDirNet::DelEdge(const int& SrcNId, const int& DstNId, const bool& IsDir) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  { TNode& N = GetNode(SrcNId);
  TIntPr Id(SrcNId, DstNId);
  SAttrE.DelSAttrId(Id);
  const int n = N.OutNIdV.SearchBin(DstNId);
  if (n!= -1) { N.OutNIdV.Del(n); } }
  { TNode& N = GetNode(DstNId);
  const int n = N.InNIdV.SearchBin(SrcNId);
  if (n!= -1) { N.InNIdV.Del(n); } }
  if (! IsDir) {
    { TNode& N = GetNode(SrcNId);
    const int n = N.InNIdV.SearchBin(DstNId);
    if (n!= -1) { N.InNIdV.Del(n); } }
    { TNode& N = GetNode(DstNId);
    const int n = N.OutNIdV.SearchBin(SrcNId);
    if (n!= -1) { N.OutNIdV.Del(n); } }
  }
}

bool TDirNet::IsEdge(const int& SrcNId, const int& DstNId, const bool& IsDir) const {
  if (! IsNode(SrcNId) || ! IsNode(DstNId)) { return false; }
  if (IsDir) { return GetNode(SrcNId).IsOutNId(DstNId); }
  else { return GetNode(SrcNId).IsOutNId(DstNId) || GetNode(DstNId).IsOutNId(SrcNId); }
}

TDirNet::TEdgeI TDirNet::GetEI(const int& SrcNId, const int& DstNId) const {
  const TNodeI SrcNI = GetNI(SrcNId);
  const int NodeN = SrcNI.NodeHI.GetDat().OutNIdV.SearchBin(DstNId);
  IAssert(NodeN != -1);
  return TEdgeI(SrcNI, EndNI(), NodeN);
}

void TDirNet::GetNIdV(TIntV& NIdV) const {
  NIdV.Gen(GetNodes(), 0);
  for (int N=NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    NIdV.Add(NodeH.GetKey(N)); }
}

void TDirNet::Defrag(const bool& OnlyNodeLinks) {
  for (int n = NodeH.FFirstKeyId(); NodeH.FNextKeyId(n); ) {
    TNode& Node = NodeH[n];
    Node.InNIdV.Pack();  Node.OutNIdV.Pack();
  }
  if (! OnlyNodeLinks && ! NodeH.IsKeyIdEqKeyN()) { NodeH.Defrag(); }
}

// for each node check that their neighbors are also nodes
bool TDirNet::IsOk(const bool& ThrowExcept) const {
  bool RetVal = true;
  for (int N = NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    const TNode& Node = NodeH[N];
    if (! Node.OutNIdV.IsSorted()) {
      const TStr Msg = TStr::Fmt("Out-neighbor list of node %d is not sorted.", Node.GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
    if (! Node.InNIdV.IsSorted()) {
      const TStr Msg = TStr::Fmt("In-neighbor list of node %d is not sorted.", Node.GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
    // check out-edges
    int prevNId = -1;
    for (int e = 0; e < Node.GetOutDeg(); e++) {
      if (! IsNode(Node.GetOutNId(e))) {
        const TStr Msg = TStr::Fmt("Out-edge %d --> %d: node %d does not exist.",
          Node.GetId(), Node.GetOutNId(e), Node.GetOutNId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      if (e > 0 && prevNId == Node.GetOutNId(e)) {
        const TStr Msg = TStr::Fmt("Node %d has duplidate out-edge %d --> %d.",
          Node.GetId(), Node.GetId(), Node.GetOutNId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      prevNId = Node.GetOutNId(e);
    }
    // check in-edges
    prevNId = -1;
    for (int e = 0; e < Node.GetInDeg(); e++) {
      if (! IsNode(Node.GetInNId(e))) {
        const TStr Msg = TStr::Fmt("In-edge %d <-- %d: node %d does not exist.",
          Node.GetId(), Node.GetInNId(e), Node.GetInNId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      if (e > 0 && prevNId == Node.GetInNId(e)) {
        const TStr Msg = TStr::Fmt("Node %d has duplidate in-edge %d <-- %d.",
          Node.GetId(), Node.GetId(), Node.GetInNId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      prevNId = Node.GetInNId(e);
    }
  }
  return RetVal;
}

void TDirNet::Dump(FILE *OutF) const {
  const int NodePlaces = (int) ceil(log10((double) GetNodes()));
  fprintf(OutF, "-------------------------------------------------\nDirected Node Graph: nodes: %d, edges: %d\n", GetNodes(), GetEdges());
  for (int N = NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    const TNode& Node = NodeH[N];
    fprintf(OutF, "  %*d]\n", NodePlaces, Node.GetId());
    fprintf(OutF, "    in [%d]", Node.GetInDeg());
    for (int edge = 0; edge < Node.GetInDeg(); edge++) {
      fprintf(OutF, " %*d", NodePlaces, Node.GetInNId(edge)); }
    fprintf(OutF, "\n    out[%d]", Node.GetOutDeg());
    for (int edge = 0; edge < Node.GetOutDeg(); edge++) {
      fprintf(OutF, " %*d", NodePlaces, Node.GetOutNId(edge)); }
    fprintf(OutF, "\n");
  }
  fprintf(OutF, "\n");
}

PDirNet TDirNet::GetSmallGraph() {
  PDirNet G = TDirNet::New();
  for (int i = 0; i < 5; i++) { G->AddNode(i); }
  G->AddEdge(0,1); G->AddEdge(1,2); G->AddEdge(0,2);
  G->AddEdge(1,3); G->AddEdge(3,4); G->AddEdge(2,3);
  return G;
}

int TDirNet::AddSAttrDatN(const TInt& NId, const TStr& AttrName, const TInt& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrName, Val);
}

int TDirNet::AddSAttrDatN(const TInt& NId, const TInt& AttrId, const TInt& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrId, Val);
}

int TDirNet::AddSAttrDatN(const TInt& NId, const TStr& AttrName, const TFlt& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrName, Val);
}

int TDirNet::AddSAttrDatN(const TInt& NId, const TInt& AttrId, const TFlt& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrId, Val);
}

int TDirNet::AddSAttrDatN(const TInt& NId, const TStr& AttrName, const TStr& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrName, Val);
}

int TDirNet::AddSAttrDatN(const TInt& NId, const TInt& AttrId, const TStr& Val) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.AddSAttrDat(NId, AttrId, Val);
}

int TDirNet::GetSAttrDatN(const TInt& NId, const TStr& AttrName, TInt& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrName, Val);
}

int TDirNet::GetSAttrDatN(const TInt& NId, const TInt& AttrId, TInt& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrId, Val);
}

int TDirNet::GetSAttrDatN(const TInt& NId, const TStr& AttrName, TFlt& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrName, Val);
}

int TDirNet::GetSAttrDatN(const TInt& NId, const TInt& AttrId, TFlt& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrId, Val);
}

int TDirNet::GetSAttrDatN(const TInt& NId, const TStr& AttrName, TStr& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrName, Val);
}

int TDirNet::GetSAttrDatN(const TInt& NId, const TInt& AttrId, TStr& Val) const {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.GetSAttrDat(NId, AttrId, Val);
}

int TDirNet::DelSAttrDatN(const TInt& NId, const TStr& AttrName) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.DelSAttrDat(NId, AttrName);
}

int TDirNet::DelSAttrDatN(const TInt& NId, const TInt& AttrId) {
  if (!IsNode(NId)) {
    return -1;
  }
  return SAttrN.DelSAttrDat(NId, AttrId);
}

int TDirNet::GetSAttrVN(const TInt& NId, const TAttrType AttrType, TAttrPrV& AttrV) const {
  if (!IsNode(NId)) {
    return -1;
  }
  SAttrN.GetSAttrV(NId, AttrType, AttrV);
  return 0;
}

int TDirNet::GetIdVSAttrN(const TStr& AttrName, TIntV& IdV) const {
  return SAttrN.GetIdVSAttr(AttrName, IdV);
}

int TDirNet::GetIdVSAttrN(const TInt& AttrId, TIntV& IdV) const {
  return SAttrN.GetIdVSAttr(AttrId, IdV);
}

int TDirNet::AddSAttrN(const TStr& Name, const TAttrType& AttrType, TInt& AttrId) {
  return SAttrN.AddSAttr(Name, AttrType, AttrId);
}

int TDirNet::GetSAttrIdN(const TStr& Name, TInt& AttrId, TAttrType& AttrType) const {
  return SAttrN.GetSAttrId(Name, AttrId, AttrType);
}

int TDirNet::GetSAttrNameN(const TInt& AttrId, TStr& Name, TAttrType& AttrType) const {
  return SAttrN.GetSAttrName(AttrId, Name, AttrType);
}

int TDirNet::AddSAttrDatE(const int& SrcNId, const int& DstNId, const TStr& AttrName, const TInt& Val) {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TIntPr EId(SrcNId, DstNId);
  return SAttrE.AddSAttrDat(EId, AttrName, Val);
}

int TDirNet::AddSAttrDatE(const int& SrcNId, const int& DstNId, const TInt& AttrId, const TInt& Val) {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TIntPr EId(SrcNId, DstNId);
  return SAttrE.AddSAttrDat(EId, AttrId, Val);
}

int TDirNet::AddSAttrDatE(const int& SrcNId, const int& DstNId, const TStr& AttrName, const TFlt& Val) {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TIntPr EId(SrcNId, DstNId);
  return SAttrE.AddSAttrDat(EId, AttrName, Val);
}

int TDirNet::AddSAttrDatE(const int& SrcNId, const int& DstNId, const TInt& AttrId, const TFlt& Val) {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TIntPr EId(SrcNId, DstNId);
  return SAttrE.AddSAttrDat(EId, AttrId, Val);
}

int TDirNet::AddSAttrDatE(const int& SrcNId, const int& DstNId, const TStr& AttrName, const TStr& Val) {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TIntPr EId(SrcNId, DstNId);
  return SAttrE.AddSAttrDat(EId, AttrName, Val);
}

int TDirNet::AddSAttrDatE(const int& SrcNId, const int& DstNId, const TInt& AttrId, const TStr& Val) {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TIntPr EId(SrcNId, DstNId);
  return SAttrE.AddSAttrDat(EId, AttrId, Val);
}

int TDirNet::GetSAttrDatE(const int& SrcNId, const int& DstNId, const TStr& AttrName, TInt& Val) const {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TIntPr EId(SrcNId, DstNId);
  return SAttrE.GetSAttrDat(EId, AttrName, Val);
}

int TDirNet::GetSAttrDatE(const int& SrcNId, const int& DstNId, const TInt& AttrId, TInt& Val) const {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TIntPr EId(SrcNId, DstNId);
  return SAttrE.GetSAttrDat(EId, AttrId, Val);
}

int TDirNet::GetSAttrDatE(const int& SrcNId, const int& DstNId, const TStr& AttrName, TFlt& Val) const {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TIntPr EId(SrcNId, DstNId);
  return SAttrE.GetSAttrDat(EId, AttrName, Val);
}

int TDirNet::GetSAttrDatE(const int& SrcNId, const int& DstNId, const TInt& AttrId, TFlt& Val) const {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TIntPr EId(SrcNId, DstNId);
  return SAttrE.GetSAttrDat(EId, AttrId, Val);
}

int TDirNet::GetSAttrDatE(const int& SrcNId, const int& DstNId, const TStr& AttrName, TStr& Val) const {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TIntPr EId(SrcNId, DstNId);
  return SAttrE.GetSAttrDat(EId, AttrName, Val);
}

int TDirNet::GetSAttrDatE(const int& SrcNId, const int& DstNId, const TInt& AttrId, TStr& Val) const {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TIntPr EId(SrcNId, DstNId);
  return SAttrE.GetSAttrDat(EId, AttrId, Val);
}

int TDirNet::DelSAttrDatE(const int& SrcNId, const int& DstNId, const TStr& AttrName) {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TIntPr EId(SrcNId, DstNId);
  return SAttrE.DelSAttrDat(EId, AttrName);
}

int TDirNet::DelSAttrDatE(const int& SrcNId, const int& DstNId, const TInt& AttrId) {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TIntPr EId(SrcNId, DstNId);
  return SAttrE.DelSAttrDat(EId, AttrId);
}

int TDirNet::GetSAttrVE(const int& SrcNId, const int& DstNId, const TAttrType AttrType, TAttrPrV& AttrV) const {
  if (!IsEdge(SrcNId, DstNId)) {
    return -1;
  }
  TIntPr EId(SrcNId, DstNId);
  SAttrE.GetSAttrV(EId, AttrType, AttrV);
  return 0;
}

int TDirNet::GetIdVSAttrE(const TStr& AttrName, TIntPrV& IdV) const {
  return SAttrE.GetIdVSAttr(AttrName, IdV);
}

int TDirNet::GetIdVSAttrE(const TInt& AttrId, TIntPrV& IdV) const {
  return SAttrE.GetIdVSAttr(AttrId, IdV);
}

int TDirNet::AddSAttrE(const TStr& Name, const TAttrType& AttrType, TInt& AttrId) {
  return SAttrE.AddSAttr(Name, AttrType, AttrId);
}

int TDirNet::GetSAttrIdE(const TStr& Name, TInt& AttrId, TAttrType& AttrType) const {
  return SAttrE.GetSAttrId(Name, AttrId, AttrType);
}

int TDirNet::GetSAttrNameE(const TInt& AttrId, TStr& Name, TAttrType& AttrType) const {
  return SAttrE.GetSAttrName(AttrId, Name, AttrType);
}
