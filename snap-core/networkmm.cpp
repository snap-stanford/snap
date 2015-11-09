////////////////////////////////////////////////
// Mutimodal Network

int TMMNet::AddMode(const TStr& ModeName, TInt& ModeId) {

  //Book-keeping for new mode id and the hash lookups
  ModeId = TInt(MxModeId);
  MxModeId++;
  ModeIdToNameH.AddDat(ModeId, ModeName);
  ModeNameToIdH.AddDat(ModeName, ModeId);

  PMultiNet NewGraph = TMultiNet.New();
  PMultiNetV.Append(NewGraph);

  return ModeId;
}

int TMMNet::AddLinkType(const TStr& ModeName1, const TStr& ModeName2, const TStr& EdgeTypeName, TInt& EdgeTypeId) {
  if (!ModeNameToIdH.IsKey(ModeName1) || !ModeNameToIdH.IsKey(ModeName2)) { return -1; }
  TInt ModeId1 = GetModeId(ModeName1);
  TInt ModeId2 = GetModeId(ModeName2);
  return AddLinkType(ModeId1, ModeId2, EdgeTypeName, EdgeTypeId)
}
int TMMNet::AddLinkType(const TInt& ModeId1, const TInt& ModeId2, const TStr& EdgeTypeName, TInt& EdgeTypeId) {
  if (!ModeIdToNameH.IsKey(ModeId1) || !ModeIdToNameH.IsKey(ModeId2)) { return -1; }
  EdgeTypeId = TInt(MxLinkTypeId);
  MxLinkTypeId++;
  LinkIdToNameH.AddDat(EdgeTypeId, EdgeTypeName);
  LinkNameToIdH.AddDat(EdgeTypeName, EdgeTypeId);
  TIntPr Pairing(ModeId1, ModeId2);
  LinkNameToPrH.AddDat(EdgeTypeName, Pairing);
  LinkPrToNameH.AddDat(Pairing, EdgeTypeName);

  TMultiLink Link = TMultiLink();
  TMultiLinkV.Append(Link)
  return 0;
}


int TMultiLink::AddLink (const int& sourceNId, const int& sourceNModeId, const int& destNId, const int& destNModeId, const int& EId) {
  int type1NId;
  int type2NId;
  bool direction = true;
  if (Mode1 == sourceNModeId && Mode2 == destNModeId) {
    type1NId = sourceNId;
    type2NId = destNId;
  } else if (Mode2 == sourceNModeId && Mode1 == destNModeId) {
    type1NId = destNId;
    type2NId = sourceNId;
    direction = false;
  } else {
    return -1;
  }
  return AddLink(type1NId, type2NId, direction, EId);
}

int TMultiLink::AddLink (const int& NIdType1, const int& NIdType2, const bool& direction, const int& EId) {
  if (EId == -1) {
    MxEId++;
    int newEId = MxEId;
  }
  else {
    IAssertR(!LinkH.IsKey(EId), TStr::Fmt("The edge with id %d already exists", EId ));
    int newEId = EId;
    if (newEId > MxEId) MxEId = newEId; //TODO: Figure this out
  }
  TMultiEdge newEdge(newEId, NIdType1, NIdType2, direction);
  LinkH.AddDat(newEid, newEdge);
  return 0;

}

int TMultiLink::DelLink(const int& EId) {
  if (LinkH.DelIfKey(EId)) { return 0; }
  return -1;
}

TMultiEdge TMultiLink::GetLink (const int& EId) const {
  IAssertR(LinkH.IsKey(EId),TStr::Fmt("No link with id %d exists", EId));
  return LinkH.GetDat(EId);
}

int TMultiNet::AddEdge(const int& SrcNId, const int& DstNId, const int& DstModeId, const TInt& EId=-1){
  IAssertR(NodeH.IsKey(SrcNId),TStr::Fmt("No node with id %d exists", SrcNId));
  return this.AddEdge(SrcNId, DstNId, true, DstModeId, EId);
}

int TMultiNet::AddEdge(const int& NId, const int& OtherTypeNId, bool direction, const int& DstModeId, const TInt& EId=-1){
  return MMNet.AddEdge(NId, OtherTypeNId, direction,DstModeId,Eid);
}
int TMultiNet::AddEdge(const int& NId, const int& OtherTypeNId, bool direction, const TStr& LinkTypeName, const TInt& EId=-1){
  return MMNet.AddEdge(NId, OtherTypeNId, direction,DstModeId,Eid);

}
int TMultiNet::DelEdge(const TStr& LinkTypeName, const TInt& EId){
}


//TODO
//method to add neighbors; will be called by TMMNet AddEdge function; outEdge == true iff NId(which is of the type of the TMultiNet; i.e. it should refer to a node in this graph) is the source node.
int TMultiNet::AddNeighbor(const int& NId, const int& EId, bool outEdge){
  if (outEdge) {
    //IntVV attribute.AddDat()
  }
  else {

  }

}
int TMultiNet::DelNeighbor(const int& NId, const int& EId, bool outEdge){
  if (outEdge ) {

  }
  else{

  }

}

//TODO: Does TMMNet implement any of the logic for this?
int TMultiNet::DelNode ( const int& NId){

}
int TMMNet::AddEdge(int& NId, int& OtherTypeNId, bool& direction, TStr& LinkTypeName, TInt& EId) {
  int index = LinkNameToIdH.GetDat(LinkTypeName);
  TMultiLink& Link = TMultiLinkV[index];
  Link.AddLink(NId, OtherTypeNId, direction, EId);
  //TODO: Finish this method
}
