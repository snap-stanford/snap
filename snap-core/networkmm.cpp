////////////////////////////////////////////////
// Mutimodal Network

int TMMNet::AddMode(const TStr& ModeName, TInt& ModeId) {
  if (PModeNetV.Len() != MxModeId) {
    return -1;
  }
  //Book-keeping for new mode id and the hash lookups
  ModeId = TInt(MxModeId);
  MxModeId++;
  ModeIdToNameH.AddDat(ModeId, ModeName);
  ModeNameToIdH.AddDat(ModeName, ModeId);

  PModeNet NewGraph = TModeNet.New();
  PModeNetV.Append(NewGraph); //TODO: Where is Append?

  return ModeId;
}

int TMMNet::AddLinkType(const TStr& ModeName1, const TStr& ModeName2, const TStr& EdgeTypeName, TInt& EdgeTypeId) {
  if (!ModeNameToIdH.IsKey(ModeName1) || !ModeNameToIdH.IsKey(ModeName2)) { return -1; }
  TInt ModeId1 = GetModeId(ModeName1);
  TInt ModeId2 = GetModeId(ModeName2);
  return AddLinkType(ModeId1, ModeId2, EdgeTypeName, EdgeTypeId);
}


int TMMNet::AddLinkType(const TInt& ModeId1, const TInt& ModeId2, const TStr& EdgeTypeName, TInt& EdgeTypeId) {
  if (!ModeIdToNameH.IsKey(ModeId1) || !ModeIdToNameH.IsKey(ModeId2)) { return -1; }
  EdgeTypeId = TInt(MxLinkTypeId);
  MxLinkTypeId++;
  LinkIdToNameH.AddDat(EdgeTypeId, EdgeTypeName);
  LinkNameToIdH.AddDat(EdgeTypeName, EdgeTypeId);
  TCrossNet Link = TCrossNet(ModeId1, ModeId2, EdgeTypeId);
  TCrossNetH.AddDat(EdgeTypeId, Link);
  return 0;
}

int TMMNet::DelLinkType(const TInt& EdgeTypeId) {
  //TODO: delete all neighbors from the modes involved in the link type
  return -1;
}

int TMMNet::DelLinkType(const TStr& EdgeType) {
  if (!LinkNameToIdH.IsKey(EdgeType)) {
    return -1;
  }
  return DelLinkType(LinkNameToIdH.GetDat(EdgeType));
}

int TMMNet::DelMode(const TInt& ModeId) {
  //TODO: figure out what to put in the vector when the mode is deleted
  //TODO: delete all edges in the mode
  return -1;
}
int TMMNet::DelMode(const TStr& ModeName) {
  if (!ModeNameToIdH.IsKey(ModeName)) {
    return -1;
  }
  return DelMode(ModeNameToIdH.GetDat(ModeName));
}


TIntPr TMMNet::GetOrderedLinkPair(const TStr& Mode1, const TStr& Mode2) {
  TInt ModeId1 = GetModeId(Mode1);
  TInt ModeId2 = GetModeId(Mode2);
  return GetOrderedLinkPair(ModeId1, ModeId2);
}

TIntPr TMMNet::GetOrderedLinkPair(const TInt& Mode1, const TInt& Mode2) {
  if (Mode1 < Mode2) {
    return TIntPr(Mode1, Mode2);
  }
  return TIntPr(Mode2, Mode1);
}

int TMMNet::AddEdge(int& NId, int& NId2, bool& direction, int& ModeId1, TStr& LinkTypeName, TInt& EId) {
  if (!LinkNameToIdH.IsKey(LinkTypeName)) {
    return -1;
  }
  return AddEdge(NId, NId2, direction, ModeId1, LinkNameToIdH.GetDat(LinkTypeName), EId);
}
int TMMNet::AddEdge(int& NId, int& NId2, bool& direction, int& ModeId1, TInt& LinkTypeId, TInt& EId) {
  if (!TCrossNetH.IsKey(LinkTypeId)) {
    return -1;
  }
  TCrossNet& CrossNet = TCrossNetH.GetDat(LinkTypeId);
  TInt ModeId2;
  if (ModeId1 == CrossNet.GetMode1()) {
    EId = CrossNet.AddLink(NId, NId2, direction, EId);
    ModeId2 = CrossNet.GetMode2();
  } else {
    EId = CrossNet.AddLink(NId2, NId, !direction, EId);
    ModeId2 = CrossNet.GetMode1();
  }
  PModeNet Net1 = PModeNetV[ModeId1];
  Net1->AddNeighbor(NId, EId, direction, LinkTypeId);
  PModeNet Net2 = PModeNetV[ModeId2];
  Net2->AddNeighbor(NId2, EId, !direction, LinkTypeId);
  return EId;
}


int TMMNet::DelEdge(const TInt& LinkTypeId, const TInt& EId) {
  if (!TCrossNetH.IsKey(LinkTypeId)) {
    return -1;
  }
  TCrossNet& CrossNet = TCrossNetH.GetDat(LinkTypeId);
  TCrosNet::TCrossEdgeI EI = CrossNet.GetI(EId);
  TInt SrcMId = EI.GetSrcModeId();
  TInt SrcNId = EI.GetSrcNId();
  TInt DstMId = EI.GetDstModeId();
  TInt DstNId = EI.GetDstNId();
  PModeNet Net1 = PModeNetV[SrcMId];
  Net1->DelNeighbor(SrcNId, EId, true, LinkTypeId);
  PModeNet Net2 = PModeNetV[DstMId];
  Net2->DelNeighbor(DstNId2, EId, false, LinkTypeId);
  return CrossNet.DelLink(EId);
}



int TCrossNet::AddLink (const int& sourceNId, const int& sourceNModeId, const int& destNId, const int& EId) {
  int type1NId;
  int type2NId;
  bool direction = true;
  if (Mode1 == sourceNModeId) {
    type1NId = sourceNId;
    type2NId = destNId;
  } else if (Mode2 == sourceNModeId) {
    type1NId = destNId;
    type2NId = sourceNId;
    direction = false;
  } else {
    return -1;
  }
  return AddLink(type1NId, type2NId, direction, EId);
}

int TCrossNet::AddLink (const int& NIdType1, const int& NIdType2, const bool& direction, const int& EId) {
  int newEId;
  if (EId == -1) {
    MxEId++;
    newEId = MxEId;
  }
  else {
    IAssertR(!LinkH.IsKey(EId), TStr::Fmt("The edge with id %d already exists", EId ));
    newEId = EId;
    if (newEId > MxEId) MxEId = newEId; //TODO: Figure this out
  }
  TCrossEdge newEdge(newEId, NIdType1, NIdType2, direction);
  LinkH.AddDat(newEId, newEdge);
  return EId;

}

int TCrossNet::AddEdge (const int& sourceNId, const int& sourceNModeId, const int& destNId, const int& destNModeId, const int& EId) {
  if (!(sourceNModeId == Mode1 && destNModeId == Mode2) && !(sourceNModeId == Mode2 && destNModeId == Mode1)) {
    return -1;
  }
  return Net->AddEdge(NIdType1, NIdType2, true, sourceNModeId, LinkId, EId);
}

int TCrossNet::AddEdge (const int& NIdType1, const int& NIdType2, const bool& direction, const int& EId) {
  return Net->AddEdge(NIdType1, NIdType2, direction, Mode1, LinkId, EId);
}

int TCrossNet::DelLink(const int& EId) {
  if (LinkH.DelIfKey(EId)) { return 0; }
  return -1;
}

int TCrossNet::DelEdge(const int& EId) {
  return Net->DelEdge(LinkId, EId);
}



TStr TModeNet::GetNeighborLinkName(TStr& LinkName, bool isOutEdge) {
  TStr Cpy(LinkName);
  if (isOutEdge) {
    Cpy += ":OUT";
  } else {
    Cpy += ":IN";
  }
  return Cpy;
}

int TModeNet::AddNeighbor(const int& NId, const int& EId, bool outEdge, const int linkId){
  TStr LinkName = MMNet->GetLinkName(linkId);
  return AddNeighbor(NId, EId, outEdge, LinkName);
}

int TModeNet::AddNeighbor(const int& NId, const int& EId, bool outEdge, const TStr& LinkName){
  TStr Name = GetNeighborLinkName(LinkName, outEdge);
  if (!NeighborTypes.IsKey(Name)) {
    NeighborTypes.AddKey(Name);
  }
  return AppendIntVAttrDatN(NId, EId, Name); 
}

int TModeNet::DelNeighbor(const int& NId, const int& EId, bool outEdge, const TStr& LinkName){
  TStr Name = GetNeighborLinkName(LinkName, outEdge);
  if (!NeighborTypes.IsKey(Name)) {
    return -1;
  }
  return DelFromIntVAttrDatN(NId, EId, Name);
}

int TModeNet::DelNeighbor(const int& NId, const int& EId, bool outEdge, const TInt& linkId){
  TStr LinkName = MMNet->GetLinkName(linkId);
  return DelNeighbor(NId, EId, outEdge, LinkName);
}

//TODO: Finish implementing
int TModeNet::DelNode ( const int& NId){
  // loop through all neighbor vectors, call delete edge on each of these
  TNEANet::DelNode(NId);
}

void TModeNet::SetParentPointer(PMMNet& parent) {
  MMNet = parent;
}

