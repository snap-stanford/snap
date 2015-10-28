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
