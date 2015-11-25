////////////////////////////////////////////////
// Mutimodal Network

//TODO Sheila-check: I've removed the ModeId argument here. Is it really necessary?
int TMMNet::AddMode(const TStr& ModeName) {
  //TODO: Add Assertions.
  if (TModeNetV.Len() != MxModeId) {
    return -1;
  }
  //Book-keeping for new mode id and the hash lookups
  TInt ModeId = TInt(MxModeId);
  MxModeId++;
  ModeIdToNameH.AddDat(ModeId, ModeName);
  ModeNameToIdH.AddDat(ModeName, ModeId);

  PModeNet NewGraph = TModeNet::New();
  TModeNetV.Add(*NewGraph);
  return ModeId;
}

//TODO Sheila-check: I've removed the EdgeTypeId argument here and in the next function.
int TMMNet::AddLinkType(const TStr& ModeName1, const TStr& ModeName2, const TStr& LinkTypeName) {

//  IAssertR(ModeNameToIdH.IsKey(ModeName1), TStr::Fmt("No such mode name: %s", ModeName1));
//  IAssertR(ModeNameToIdH.IsKey(ModeName2), TStr::Fmt("No such mode name: %s", ModeName2));

  TInt ModeId1 = GetModeId(ModeName1);
  TInt ModeId2 = GetModeId(ModeName2);
  return AddLinkType(ModeId1, ModeId2, LinkTypeName);
}


int TMMNet::AddLinkType(const TInt& ModeId1, const TInt& ModeId2, const TStr& LinkTypeName) {

//  IAssertR(ModeIdToNameH.IsKey(ModeId1), TStr::Fmt("No mode with id %d exists", ModeId1));
//  IAssertR(ModeIdToNameH.IsKey(ModeId2), TStr::Fmt("No mode with id %d exists", ModeId2));


  TInt LinkTypeId = TInt(MxLinkTypeId);
  MxLinkTypeId++;
  LinkIdToNameH.AddDat(LinkTypeId, LinkTypeName);
  LinkNameToIdH.AddDat(LinkTypeName, LinkTypeId);
  TCrossNet Link = TCrossNet(ModeId1, ModeId2, LinkTypeId);
//  Link.SetParentPointer(PMMNet(this)); //TODO
  TCrossNetH.AddDat(LinkTypeId, Link);

  TModeNetV[ModeId1].AddNbrType(LinkTypeName);
  TModeNetV[ModeId2].AddNbrType(LinkTypeName);
  return 0;
}

int TMMNet::DelLinkType(const TInt& LinkTypeId) {
  //TODO: delete all neighbors from the modes involved in the link type
  return -1;
}

int TMMNet::DelLinkType(const TStr& LinkType) {
//  IAssertR(LinkNameToIdH.IsKey(LinkType),TStr::Fmt("No such link type: %s", LinkType));
  return DelLinkType(LinkNameToIdH.GetDat(LinkType));
}

int TMMNet::DelMode(const TInt& ModeId) {
  //TODO: figure out what to put in the vector when the mode is deleted
  //TODO: delete all edges in the mode
  return -1;
}
int TMMNet::DelMode(const TStr& ModeName) {
  //TODO: Rewrite using IAssertR
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

int TMMNet::AddEdge(const TStr& LinkTypeName, int& NId1, int& NId2, int EId){
//  IAssertR(LinkNameToIdH.IsKey(LinkTypeName),TStr::Fmt("No such link name: %s",LinkTypeName));
  return AddEdge(LinkNameToIdH.GetDat(LinkTypeName),NId1,NId2,EId);
}
int TMMNet::AddEdge(const TInt& LinkTypeId, int& NId1, int& NId2, int EId){
//   IAssertR(LinkIdToNameH.IsKey(LinkTypeId),TStr::Fmt("No link with id %d exists",LinkTypeId));
  return TCrossNetH.GetDat(LinkTypeId).AddEdge(NId1,NId2, EId);
}
int TCrossNet::AddEdge(const int& sourceNId, const int& destNId, const int& EId){

  TInt newEId = TInt(MxEId); //TODO: Make this a function of the argument EId
  MxEId++;
  TCrossNet::TCrossEdge newEdge (newEId, sourceNId, destNId);
  LinkH.AddDat(newEId, newEdge);
  TStr ThisLinkName = Net->GetLinkName(this->LinkTypeId);
  Net->TModeNetV[this->Mode1].AddNeighbor(sourceNId, EId,true, ThisLinkName);
  Net->TModeNetV[this->Mode2].AddNeighbor(destNId, EId,false, ThisLinkName);
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


TStr TModeNet::GetNeighborLinkName(const TStr& LinkName, bool isOutEdge) {
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

void TModeNet::SetParentPointer(PMMNet parent) {
  MMNet = parent;
}

TModeNet TMMNet::GetTModeNet(const TStr& ModeName) const {
//  IAssertR(ModeNameToIdH.IsKey(ModeName),TStr::Fmt("No such mode name: %s", ModeName));
  return GetTModeNet(ModeNameToIdH.GetDat(ModeName));
}
TModeNet TMMNet::GetTModeNet(const TInt& ModeId) const {
//  IAssertR(ModeId < TModeNetV.Len(), TStr::Fmt("Mode with id %d does not exist", ModeId));
  return TModeNetV.GetVal(ModeId);
}

int TModeNet::AddNbrType(const TStr& LinkName) {
//  IAssertR(!NeighborTypes.IsKey(LinkName),TStr::Fmt("Neighbor Link Types already exists: %s", LinkName));
  NeighborTypes.AddKey(LinkName);
  return 0;
}
