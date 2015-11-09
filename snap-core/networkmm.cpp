////////////////////////////////////////////////
// Mutimodal Network

int TMMNet::AddMode(const TStr& ModeName) {

  //Book-keeping for new mode id and the hash lookups
  TInt NewModeId = MxModeId;
  MxModeId++;
  ModeIdToNameH.AddDat(NewModeId, ModeName);
  ModeNameToIdH.AddDat(ModeName, NewModeId);

  //Create a new TNEANetMM object.

  //Add the edge hash tables to every existing TNEANetMM object

  return NewModeId; //TODO: Is this necessary?
}

void TMultiLink::AddLink (const int& sourceNId, const int& sourceNModeId, const int& destNId, const int& destNModeId, const int& EId = -1) {

  if (EId == -1) {
    MxEId++;
    int newEId = MxEId;
  }
  else {
    IAssertR(!LinkH.IsKey(EId), TStr::Fmt("The edge with id %d already exists", EId ));
    int newEId = EId;
    if (newEId > MxEId) MxEId = newEId; //TODO: Figure this out
  }
  MultiEdge newEdge(newEId, sourceNId, destNId, sourceNModeId, destNModeId);
  LinkH.AddDat(newEid, newEdge);

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
