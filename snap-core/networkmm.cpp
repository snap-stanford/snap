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
  IAssertR(LinkH.IsKey(EId),TStr::FMT("No link with id %d exists", Eid));
  return LinkH.GetDat(EId);
}
