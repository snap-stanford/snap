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
  PModeNetV.Append(NewGraph);

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
  return -1;
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
  if (EId == -1) {
    MxEId++;
    int newEId = MxEId;
  }
  else {
    IAssertR(!LinkH.IsKey(EId), TStr::Fmt("The edge with id %d already exists", EId ));
    int newEId = EId;
    if (newEId > MxEId) MxEId = newEId; //TODO: Figure this out
  }
  TCrossEdge newEdge(newEId, NIdType1, NIdType2, direction);
  LinkH.AddDat(newEid, newEdge);
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







int TModeNet::AddEdge(const int& CurrModeNId, const int& OtherModeNId, bool direction, const TStr& LinkTypeName, const TInt& EId=-1) {

  return MMNet->AddEdge(CurrModeNId, OtherModeNId, direction, LinkTypeName, EId);
}

int TModeNet::AddEdge(const int& CurrModeNId, const int& OtherModeNId, bool direction, const TInt& LinkTypeId, const TInt& EId=-1) {
  return MMNet->AddEdge(CurrModeNId, OtherModeNId, direction, LinkTypeId, EId);
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
  return AddNeighbor(NId, EId, outEdge, linkName);
}

int TModeNet::AddNeighbor(const int& NId, const int& EId, bool outEdge, const TStr& LinkName){
  TStr Name = GetNeighborLinkName(LinkName, outEdge);
  if (!NeighborTypes.IsKey(Name)) {
    NeighborTypes.AddKey(name);
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

//TODO: Does TMMNet implement any of the logic for this? 
int TModeNet::DelNode ( const int& NId){
  // loop through all neighbor vectors, call delete edge on each of these
  TNEANet::DelNode(NId);
}

int TModeNet::DelEdge(const TStr& LinkTypeName, const TInt& EId) {
  TInt LinkTypeId = MMNet->GetLinkId(LinkTypeName);
  return DelEdge(LinkTypeId, EId);
}
int TModeNet::DelEdge(const TInt& LinkTypeId, const TInt& EId) {
  return MMNet->DelEdge(LinkTypeId, EId);
}
