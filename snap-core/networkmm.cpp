////////////////////////////////////////////////
// Mutimodal Network

int TMMNet::AddMode(const TStr& ModeName, TInt& ModeId) {

  //Book-keeping for new mode id and the hash lookups
  ModeId = TInt(MxModeId);
  MxModeId++;
  ModeIdToNameH.AddDat(ModeId, ModeName);
  ModeNameToIdH.AddDat(ModeName, ModeId);

  PNEANetMM NewGraph = TNEANetMM.New();
  PNEANetMMV.Append(NewGraph);
  //Create a new TNEANetMM object.

  //Add the edge hash tables to every existing TNEANetMM object

  return ModeId;
}

int AddLinkType(const TStr& ModeName1, const TStr& ModeName2, const TStr& EdgeTypeName, TInt& EdgeTypeId) {
  if (!ModeNameToIdH.IsKey(ModeName1) || !ModeNameToIdH.IsKey(ModeName2)) { return -1; }
  TInt ModeId1 = GetModeId(ModeName1);
  TInt ModeId2 = GetModeId(ModeName2);
  return AddLink(ModeId1, ModeId2, EdgeTypeName, EdgeTypeId)
}
int AddLinkType(const TInt& ModeId1, const TInt& ModeId2, const TStr& EdgeTypeName, TInt& EdgeTypeId) {
  if (!ModeIdToNameH.IsKey(ModeId1) || !ModeIdToNameH.IsKey(ModeId2)) { return -1; }
  EdgeTypeId = TInt(MxLinkTypeId);
  MxLinkTypeId++;
  LinkIdToNameH.AddDat(EdgeTypeId, EdgeTypeName);
  LinkNameToIdH.AddDat(EdgeTypeName, EdgeTypeId);

  TMultiLink Link = TMultiLink();
  TMultiLinkV.Append(Link)
  return 0;
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
  IAssertR(LinkH.IsKey(EId),TStr::FMT("No link with id %d exists", Eid));
  return LinkH.GetDat(EId);
}
