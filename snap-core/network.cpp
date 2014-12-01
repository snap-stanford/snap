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
  return IntDel || StrDel || FltDel;
}

bool TNEANet::IsIntAttrDeletedN(const int& NId, const TStr& attr) const {
  return NodeAttrIsIntDeleted(NId, KeyToIndexTypeN.GetI(attr));
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
  return IntDel || StrDel || FltDel;
}

bool TNEANet::NodeAttrIsIntDeleted(const int& NId, const TStrIntPrH::TIter& NodeHI) const {
  if (NodeHI.GetDat().Val1 != IntType) {
    return false;
  }
  return (GetIntAttrDefaultN(NodeHI.GetKey()) == this->VecOfIntVecsN.GetVal(
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
  bool StrDel = IsStrAttrDeletedE(EId, attr);
  bool FltDel = IsFltAttrDeletedE(EId, attr);
  return IntDel || StrDel || FltDel;
}

bool TNEANet::IsIntAttrDeletedE(const int& EId, const TStr& attr) const {
  return EdgeAttrIsIntDeleted(EId, KeyToIndexTypeE.GetI(attr));
}

bool TNEANet::IsStrAttrDeletedE(const int& EId, const TStr& attr) const {
  return EdgeAttrIsStrDeleted(EId, KeyToIndexTypeE.GetI(attr));
}

bool TNEANet::IsFltAttrDeletedE(const int& EId, const TStr& attr) const {
  return EdgeAttrIsFltDeleted(EId, KeyToIndexTypeE.GetI(attr));
}

bool TNEANet::EdgeAttrIsDeleted(const int& EId, const TStrIntPrH::TIter& EdgeHI) const {
  bool IntDel = EdgeAttrIsIntDeleted(EId, EdgeHI);
  bool StrDel = EdgeAttrIsStrDeleted(EId, EdgeHI);
  bool FltDel = EdgeAttrIsFltDeleted(EId, EdgeHI);
  return IntDel || StrDel || FltDel;
}

bool TNEANet::EdgeAttrIsIntDeleted(const int& EId, const TStrIntPrH::TIter& EdgeHI) const {
  return (EdgeHI.GetDat().Val1 == IntType &&
    GetIntAttrDefaultE(EdgeHI.GetKey()) == this->VecOfIntVecsE.GetVal(
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
  int i;
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    MxNId = TMath::Mx(NId+1, MxNId());
  }
  // update attribute columns
  NodeH.AddDat(NId, TNode(NId));
  for (i = 0; i < VecOfIntVecsN.Len(); i++) {
    TVec<TInt>& IntVec = VecOfIntVecsN[i];
    IntVec.Ins(NodeH.GetKeyId(NId), TInt::Mn);
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
    StrVec.Ins(NodeH.GetKeyId(NId), TStr::GetNullStr());
  }
  TVec<TStr> DefStrVec = TVec<TStr>();
  IntDefaultsN.GetKeyV(DefStrVec);
  for (i = 0; i < DefStrVec.Len(); i++) {
    TStr attr = DefStrVec[i];
    TVec<TStr>& StrVec = VecOfStrVecsN[KeyToIndexTypeN.GetDat(DefStrVec[i]).Val2];
    StrVec[NodeH.GetKeyId(NId)] = GetStrAttrDefaultN(attr);
  }
  for (i = 0; i < VecOfFltVecsN.Len(); i++) {
    TVec<TFlt>& FltVec = VecOfFltVecsN[i];
    FltVec.Ins(NodeH.GetKeyId(NId), TFlt::Mn);
  }
  TVec<TStr> DefFltVec = TVec<TStr>();
  FltDefaultsN.GetKeyV(DefFltVec);
  for (i = 0; i < DefFltVec.Len(); i++) {
    TStr attr = DefFltVec[i];
    TVec<TFlt>& FltVec = VecOfFltVecsN[KeyToIndexTypeN.GetDat(DefFltVec[i]).Val2];
    FltVec[NodeH.GetKeyId(NId)] = GetFltAttrDefaultN(attr);
  }
  return NId;
}

void TNEANet::DelNode(const int& NId) {
  int i;
  
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
    IntVec.Ins(EdgeH.GetKeyId(EId), TInt::Mn);
  }
  TVec<TStr> DefIntVec = TVec<TStr>();
  IntDefaultsE.GetKeyV(DefIntVec);
  for (i = 0; i < DefIntVec.Len(); i++) {
    TStr attr = DefIntVec[i];
    TVec<TInt>& IntVec = VecOfIntVecsE[KeyToIndexTypeE.GetDat(DefIntVec[i]).Val2];
    IntVec[EdgeH.GetKeyId(EId)] = GetIntAttrDefaultE(attr);
  }

  for (i = 0; i < VecOfStrVecsE.Len(); i++) {
    TVec<TStr>& StrVec = VecOfStrVecsE[i];
    StrVec.Ins(EdgeH.GetKeyId(EId), TStr::GetNullStr());
  }
  TVec<TStr> DefStrVec = TVec<TStr>();
  IntDefaultsE.GetKeyV(DefStrVec);
  for (i = 0; i < DefStrVec.Len(); i++) {
    TStr attr = DefStrVec[i];
    TVec<TStr>& StrVec = VecOfStrVecsE[KeyToIndexTypeE.GetDat(DefStrVec[i]).Val2];
    StrVec[EdgeH.GetKeyId(EId)] = GetStrAttrDefaultE(attr);
  }

  for (i = 0; i < VecOfFltVecsE.Len(); i++) {
    TVec<TFlt>& FltVec = VecOfFltVecsE[i];
    FltVec.Ins(EdgeH.GetKeyId(EId), TFlt::Mn);
  }
  TVec<TStr> DefFltVec = TVec<TStr>();
  FltDefaultsE.GetKeyV(DefFltVec);
  for (i = 0; i < DefFltVec.Len(); i++) {
    TStr attr = DefFltVec[i];
    TVec<TFlt>& FltVec = VecOfFltVecsE[KeyToIndexTypeE.GetDat(DefFltVec[i]).Val2];
    FltVec[NodeH.GetKeyId(EId)] = GetFltAttrDefaultE(attr);
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
  EdgeH.DelKey(EId);

  for (i = 0; i < VecOfIntVecsE.Len(); i++) {
    TVec<TInt>& IntVec = VecOfIntVecsE[i];
    IntVec.Ins(EdgeH.GetKeyId(EId), TInt::Mn);
  }
  for (i = 0; i < VecOfStrVecsE.Len(); i++) {
    TVec<TStr>& StrVec = VecOfStrVecsE[i];
    StrVec.Ins(EdgeH.GetKeyId(EId), TStr::GetNullStr());
  }
  for (i = 0; i < VecOfFltVecsE.Len(); i++) {
    TVec<TFlt>& FltVec = VecOfFltVecsE[i];
    FltVec.Ins(EdgeH.GetKeyId(EId), TFlt::Mn);
  }
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
  fprintf(OutF, "-------------------------------------------------\nDirected Node-Edge Network: nodes: %d, edges: %d\n", GetNodes(), GetEdges());
  for (TNodeI NodeI = BegNI(); NodeI < EndNI(); NodeI++) {
    fprintf(OutF, "  %*d]\n", NodePlaces, NodeI.GetId());
    // load node attributes
    TIntV IntAttrN;
    IntAttrValueNI(NodeI.GetId(), IntAttrN);
    fprintf(OutF, "    nai[%d]", IntAttrN.Len());
    for (int i = 0; i < IntAttrN.Len(); i++) {
      fprintf(OutF, " %*i", NodePlaces, IntAttrN[i]()); }
    TStrV StrAttrN;
    StrAttrValueNI(NodeI.GetId(), StrAttrN);
    fprintf(OutF, "    nas[%d]", StrAttrN.Len());
    for (int i = 0; i < StrAttrN.Len(); i++) {
      fprintf(OutF, " %*s", NodePlaces, StrAttrN[i]()); }
    TFltV FltAttrN;
    FltAttrValueNI(NodeI.GetId(), FltAttrN);
    fprintf(OutF, "    naf[%d]", FltAttrN.Len());
    for (int i = 0; i < FltAttrN.Len(); i++) {
      fprintf(OutF, " %*f", NodePlaces, FltAttrN[i]()); }

    fprintf(OutF, "    in[%d]", NodeI.GetInDeg());
    for (int edge = 0; edge < NodeI.GetInDeg(); edge++) {
      fprintf(OutF, " %*d", EdgePlaces, NodeI.GetInEId(edge)); }
    fprintf(OutF, "\n");
    fprintf(OutF, "    out[%d]", NodeI.GetOutDeg());
    for (int edge = 0; edge < NodeI.GetOutDeg(); edge++) {
      fprintf(OutF, " %*d", EdgePlaces, NodeI.GetOutEId(edge)); }
    fprintf(OutF, "\n");
  }
  for (TEdgeI EdgeI = BegEI(); EdgeI < EndEI(); EdgeI++) {
    fprintf(OutF, "  %*d]  %*d  ->  %*d\n", EdgePlaces, EdgeI.GetId(), NodePlaces, EdgeI.GetSrcNId(), NodePlaces, EdgeI.GetDstNId());

    // load edge attributes
    TIntV IntAttrE;
    IntAttrValueEI(EdgeI.GetId(), IntAttrE);
    fprintf(OutF, "    eai[%d]", IntAttrE.Len());
    for (int i = 0; i < IntAttrE.Len(); i++) {
      fprintf(OutF, " %*i", EdgePlaces, IntAttrE[i]()); }
    TStrV StrAttrE;
    StrAttrValueEI(EdgeI.GetId(), StrAttrE);
    fprintf(OutF, "    eas[%d]", StrAttrE.Len());
    for (int i = 0; i < StrAttrE.Len(); i++) {
      fprintf(OutF, " %*s", EdgePlaces, StrAttrE[i]()); }
    TFltV FltAttrE;
    FltAttrValueEI(EdgeI.GetId(), FltAttrE);
    fprintf(OutF, "    eaf[%d]", FltAttrE.Len());
    for (int i = 0; i < FltAttrE.Len(); i++) {
      fprintf(OutF, " %*f", EdgePlaces, FltAttrE[i]()); }
  }
  fprintf(OutF, "\n");
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

TInt TNEANet::GetIntAttrDatN(const int& NId, const TStr& attr) {
  return VecOfIntVecsN[KeyToIndexTypeN.GetDat(attr).Val2][NodeH.GetKeyId(NId)];
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
  CurrLen = VecOfStrVecsN.Len();
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
  CurrLen = VecOfStrVecsE.Len();
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

