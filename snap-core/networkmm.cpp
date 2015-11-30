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
  TModeNetH.AddDat(ModeId, NewGraph);
  return ModeId;
}

//TODO Sheila-check: I've removed the EdgeTypeId argument here and in the next function.
//Sheila-response: ok, then we should return the edge type id instead of just 0.
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
//  IAssertR(LinkNameToIdH.IsKey(LinkType),TStr::Fmt("No such link type: %s", LinkType));
  return DelLinkType(LinkNameToIdH.GetDat(LinkType));
}

int TMMNet::DelMode(const TInt& ModeId) {
  //TODO: figure out what to put in the vector TModeNetH when the mode is deleted
  //TODO: delete all edges in the mode
  return -1;
}
int TMMNet::DelMode(const TStr& ModeName) {
  // IAssertR(ModeNameToIdH.IsKey(ModeName), TStr::Fmt("No such mode with name: %s", ModeName))
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
  return AddEdge(LinkNameToIdH.GetDat(LinkTypeName), NId1, NId2, EId);
}
int TMMNet::AddEdge(const TInt& LinkTypeId, int& NId1, int& NId2, int EId){
//   IAssertR(LinkIdToNameH.IsKey(LinkTypeId),TStr::Fmt("No link with id %d exists",LinkTypeId));
  return TCrossNetH.GetDat(LinkTypeId).AddEdge(NId1, NId2, EId);
}

PModeNet TMMNet::GetTModeNet(const TStr& ModeName) const {
//  IAssertR(ModeNameToIdH.IsKey(ModeName),TStr::Fmt("No such mode name: %s", ModeName));
  return PModeNet(GetTModeNet(ModeNameToIdH.GetDat(ModeName)));
}
PModeNet TMMNet::GetTModeNet(const TInt& ModeId) const {
//  IAssertR(ModeId < TModeNetH.Len(), TStr::Fmt("Mode with id %d does not exist", ModeId));
  TModeNet Net = TModeNetH.GetDat(ModeId);
  return PModeNet(&Net);
}


int TCrossNet::AddEdge(const int& sourceNId, const int& destNId, int EId){
  if (EId == -1) { EId = MxEId;  MxEId++; }
  else { MxEId = TMath::Mx(EId+1, MxEId()); }
  TCrossNet::TCrossEdge newEdge (EId, sourceNId, destNId);
  LinkH.AddDat(EId, newEdge);
  TStr ThisLinkName = Net->GetLinkName(this->LinkTypeId);
  Net->TModeNetH.GetDat(this->Mode1).AddNeighbor(sourceNId, EId, true, ThisLinkName, Mode1==Mode2, true); // TODO: can't assume it is directed
  Net->TModeNetH.GetDat(this->Mode2).AddNeighbor(destNId, EId, false, ThisLinkName, Mode1==Mode2, true);
  return EId;
}

int TCrossNet::DelEdge(const int& EId) {
  TCrossEdge& Edge = LinkH.GetDat(EId);
  int srcNode = Edge.SrcNId;
  int dstNode = Edge.DstNId;
  TStr ThisLinkName = Net->GetLinkName(this->LinkTypeId);
  Net->TModeNetH.GetDat(this->Mode1).DelNeighbor(srcNode, EId, true, ThisLinkName, Mode1==Mode2, true); // TODO: can't assume it is directed
  Net->TModeNetH.GetDat(this->Mode2).DelNeighbor(dstNode, EId, false, ThisLinkName, Mode1==Mode2, true);
  return 0;
}

void TCrossNet::SetParentPointer(PMMNet parent) {
  Net = parent;
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
  TStr Name = GetNeighborLinkName(LinkName, outEdge, sameMode, isDir);
  if (!NeighborTypes.IsKey(Name)) {
    NeighborTypes.AddKey(Name);
  }
  return AppendIntVAttrDatN(NId, EId, Name); 
}

int TModeNet::DelNeighbor(const int& NId, const int& EId, bool outEdge, const TStr& LinkName, const bool sameMode, bool isDir){
  TStr Name = GetNeighborLinkName(LinkName, outEdge, sameMode, isDir);
  if (!NeighborTypes.IsKey(Name)) {
    return -1;
  }
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

void TModeNet::SetParentPointer(PMMNet parent) {
  MMNet = parent;
}

int TModeNet::AddNbrType(const TStr& LinkName, const bool sameMode, bool isDir) {
//  IAssertR(!NeighborTypes.IsKey(LinkName),TStr::Fmt("Neighbor Link Types already exists: %s", LinkName));
  if (NeighborTypes.IsKey(LinkName)) { return -1; } //Don't want to add nbr type multiple times
  bool hasSingleVector = (!isDir || !sameMode);
  NeighborTypes.AddDat(LinkName, hasSingleVector);
  return 0;
}

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
