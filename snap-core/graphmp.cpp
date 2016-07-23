/////////////////////////////////////////////////
#ifdef GCC_ATOMIC

/////////////////////////////////////////////////
// Directed Node Graph MP
bool TNGraphMP::HasFlag(const TGraphFlag& Flag) const {
  return HasGraphFlag(TNGraphMP::TNet, Flag);
}

int TNGraphMP::AddNode(int NId) {
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    MxNId = TMath::Mx(NId+1, MxNId());
  }
  NodeH.AddDat(NId, TNode(NId));
  return NId;
}

int TNGraphMP::AddNodeUnchecked(int NId) {
  if (IsNode(NId)) { return NId;}
  MxNId = TMath::Mx(NId+1, MxNId());
  NodeH.AddDat(NId, TNode(NId));
  return NId;
}

// add a node with a list of neighbors
// (use TNGraphMP::IsOk to check whether the graph is consistent)
int TNGraphMP::AddNode(const int& NId, const TIntV& InNIdV, const TIntV& OutNIdV) {
  int NewNId;
  if (NId == -1) {
    NewNId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    NewNId = NId;
    MxNId = TMath::Mx(NewNId+1, MxNId());
  }
  TNode& Node = NodeH.AddDat(NewNId);
  Node.Id = NewNId;
  Node.InNIdV = InNIdV;
  Node.OutNIdV = OutNIdV;
  Node.InNIdV.Sort();
  Node.OutNIdV.Sort();
  return NewNId;
}

// add a node from a vector pool
// (use TNGraphMP::IsOk to check whether the graph is consistent)
int TNGraphMP::AddNode(const int& NId, const TVecPool<TInt>& Pool, const int& SrcVId, const int& DstVId) {
  int NewNId;
  if (NId == -1) {
    NewNId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    NewNId = NId;
    MxNId = TMath::Mx(NewNId+1, MxNId());
  }
  TNode& Node = NodeH.AddDat(NewNId);
  Node.Id = NewNId;
  Node.InNIdV.GenExt(Pool.GetValVPt(SrcVId), Pool.GetVLen(SrcVId));
  Node.OutNIdV.GenExt(Pool.GetValVPt(DstVId), Pool.GetVLen(DstVId));
  Node.InNIdV.Sort();
  Node.OutNIdV.Sort();
  return NewNId;
}

void TNGraphMP::DelNode(const int& NId) {
#if 0
  { TNode& Node = GetNode(NId);
  for (int e = 0; e < Node.GetOutDeg(); e++) {
  const int nbr = Node.GetOutNId(e);
  if (nbr == NId) { continue; }
    TNode& N = GetNode(nbr);
    const int n = N.InNIdV.SearchBin(NId);
    if (n!= -1) { N.InNIdV.Del(n); }
  }
  for (int e = 0; e < Node.GetInDeg(); e++) {
  const int nbr = Node.GetInNId(e);
  if (nbr == NId) { continue; }
    TNode& N = GetNode(nbr);
    const int n = N.OutNIdV.SearchBin(NId);
    if (n!= -1) { N.OutNIdV.Del(n); }
  } }
  NodeH.DelKey(NId);
#endif
}

int TNGraphMP::GetEdges() const {
  int edges=0;
  for (int N=NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    edges+=NodeH[N].GetOutDeg();
  }
  return edges;
}

int TNGraphMP::AddEdge(const int& SrcNId, const int& DstNId) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  //IAssert(! IsEdge(SrcNId, DstNId));
  if (IsEdge(SrcNId, DstNId)) { return -2; }
  GetNode(SrcNId).OutNIdV.AddSorted(DstNId);
  GetNode(DstNId).InNIdV.AddSorted(SrcNId);
  return -1; // edge id
}

int TNGraphMP::AddEdgeUnchecked(const int& SrcNId, const int& DstNId) {
  GetNode(SrcNId).OutNIdV.Add(DstNId);
  GetNode(DstNId).InNIdV.Add(SrcNId);
  return -1; // edge id
}

void TNGraphMP::DelEdge(const int& SrcNId, const int& DstNId, const bool& IsDir) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  { TNode& N = GetNode(SrcNId);
  const int n = N.OutNIdV.SearchBin(DstNId);
  if (n!= -1) { N.OutNIdV.Del(n); } }
  { TNode& N = GetNode(DstNId);
  const int n = N.InNIdV.SearchBin(SrcNId);
  if (n!= -1) { N.InNIdV.Del(n); } }
  if (! IsDir) {
    { TNode& N = GetNode(SrcNId);
    const int n = N.InNIdV.SearchBin(DstNId);
    if (n!= -1) { N.InNIdV.Del(n); } }
    { TNode& N = GetNode(DstNId);
    const int n = N.OutNIdV.SearchBin(SrcNId);
    if (n!= -1) { N.OutNIdV.Del(n); } }
  }
}

bool TNGraphMP::IsEdge(const int& SrcNId, const int& DstNId, const bool& IsDir) const {
  if (! IsNode(SrcNId) || ! IsNode(DstNId)) { return false; }
  if (IsDir) { return GetNode(SrcNId).IsOutNId(DstNId); }
  else { return GetNode(SrcNId).IsOutNId(DstNId) || GetNode(DstNId).IsOutNId(SrcNId); }
}

TNGraphMP::TEdgeI TNGraphMP::GetEI(const int& SrcNId, const int& DstNId) const {
  const TNodeI SrcNI = GetNI(SrcNId);
  const int NodeN = SrcNI.NodeHI.GetDat().OutNIdV.SearchBin(DstNId);
  IAssert(NodeN != -1);
  return TEdgeI(SrcNI, EndNI(), NodeN);
}

void TNGraphMP::GetNIdV(TIntV& NIdV) const {
  NIdV.Gen(GetNodes(), 0);
  for (int N=NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    NIdV.Add(NodeH.GetKey(N)); }
}

void TNGraphMP::Defrag(const bool& OnlyNodeLinks) {
#if 0
  for (int n = NodeH.FFirstKeyId(); NodeH.FNextKeyId(n); ) {
    TNode& Node = NodeH[n];
    Node.InNIdV.Pack();  Node.OutNIdV.Pack();
  }
  if (! OnlyNodeLinks && ! NodeH.IsKeyIdEqKeyN()) { NodeH.Defrag(); }
#endif
}

// for each node check that their neighbors are also nodes
bool TNGraphMP::IsOk(const bool& ThrowExcept) const {
  bool RetVal = true;
  for (int N = NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    const TNode& Node = NodeH[N];
    if (! Node.OutNIdV.IsSorted()) {
      const TStr Msg = TStr::Fmt("Out-neighbor list of node %d is not sorted.", Node.GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
    if (! Node.InNIdV.IsSorted()) {
      const TStr Msg = TStr::Fmt("In-neighbor list of node %d is not sorted.", Node.GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
    // check out-edges
    int prevNId = -1;
    for (int e = 0; e < Node.GetOutDeg(); e++) {
      if (! IsNode(Node.GetOutNId(e))) {
        const TStr Msg = TStr::Fmt("Out-edge %d --> %d: node %d does not exist.",
          Node.GetId(), Node.GetOutNId(e), Node.GetOutNId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      if (e > 0 && prevNId == Node.GetOutNId(e)) {
        const TStr Msg = TStr::Fmt("Node %d has duplidate out-edge %d --> %d.",
          Node.GetId(), Node.GetId(), Node.GetOutNId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      prevNId = Node.GetOutNId(e);
    }
    // check in-edges
    prevNId = -1;
    for (int e = 0; e < Node.GetInDeg(); e++) {
      if (! IsNode(Node.GetInNId(e))) {
        const TStr Msg = TStr::Fmt("In-edge %d <-- %d: node %d does not exist.",
          Node.GetId(), Node.GetInNId(e), Node.GetInNId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      if (e > 0 && prevNId == Node.GetInNId(e)) {
        const TStr Msg = TStr::Fmt("Node %d has duplidate in-edge %d <-- %d.",
          Node.GetId(), Node.GetId(), Node.GetInNId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      prevNId = Node.GetInNId(e);
    }
  }
  return RetVal;
}

void TNGraphMP::Dump(FILE *OutF) const {
  const int NodePlaces = (int) ceil(log10((double) GetNodes()));
  fprintf(OutF, "-------------------------------------------------\nDirected Node Graph: nodes: %d, edges: %d\n", GetNodes(), GetEdges());
  for (int N = NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    const TNode& Node = NodeH[N];
    fprintf(OutF, "  %*d]\n", NodePlaces, Node.GetId());
    fprintf(OutF, "    in [%d]", Node.GetInDeg());
    for (int edge = 0; edge < Node.GetInDeg(); edge++) {
      fprintf(OutF, " %*d", NodePlaces, Node.GetInNId(edge)); }
    fprintf(OutF, "\n    out[%d]", Node.GetOutDeg());
    for (int edge = 0; edge < Node.GetOutDeg(); edge++) {
      fprintf(OutF, " %*d", NodePlaces, Node.GetOutNId(edge)); }
    fprintf(OutF, "\n");
  }
  fprintf(OutF, "\n");
}

PNGraphMP TNGraphMP::GetSmallGraph() {
  PNGraphMP G = TNGraphMP::New();
  for (int i = 0; i < 5; i++) { G->AddNode(i); }
  G->AddEdge(0,1); G->AddEdge(1,2); G->AddEdge(0,2);
  G->AddEdge(1,3);
  G->AddEdge(3,4);
  G->AddEdge(2,3);
  return G;
}


int TNGraphMP::AddOutEdge1(int& SrcIdx, const int& SrcNId, const int& DstNId) {
  bool Found;
  int SrcKeyId;

  //SrcKeyId = NodeH.AddKey2(SrcIdx, SrcNId, Found);
  SrcKeyId = NodeH.AddKey12(SrcIdx, SrcNId, Found);
  if (!Found) {
    NodeH[SrcKeyId] = TNode(SrcNId);
    //MxNId = TMath::Mx(SrcNId+1, MxNId());
  }
  //GetNode(SrcNId).OutNIdV.Add(DstNId);
  //NodeH[SrcKeyId].OutNIdV.Add1(DstNId);

  // TODO:RS, edge lists need to be sorted at the end

  SrcIdx = SrcKeyId;
  return Found;
}

int TNGraphMP::AddInEdge1(int& DstIdx, const int& SrcNId, const int& DstNId) {
  bool Found;
  int DstKeyId;

  //DstKeyId = NodeH.AddKey2(DstIdx, DstNId, Found);
  DstKeyId = NodeH.AddKey12(DstIdx, DstNId, Found);
  if (!Found) {
    NodeH[DstKeyId] = TNode(DstNId);
    //MxNId = TMath::Mx(DstNId+1, MxNId());
  }
  //GetNode(DstNId).InNIdV.Add(SrcNId);
  //NodeH[DstKeyId].InNIdV.Add1(SrcNId);

  // TODO:RS, edge lists need to be sorted at the end

  DstIdx = DstKeyId;
  return Found;
}

void TNGraphMP::AddOutEdge2(const int& SrcNId, const int& DstNId) {
  NodeH[NodeH.GetKeyId(SrcNId)].OutNIdV.AddMP(DstNId);
}

void TNGraphMP::AddInEdge2(const int& SrcNId, const int& DstNId) {
  NodeH[NodeH.GetKeyId(DstNId)].InNIdV.AddMP(SrcNId);
}

// add a node with a list of neighbors
// (use TNGraphMP::IsOk to check whether the graph is consistent)
void TNGraphMP::AddNodeWithEdges(const TInt& NId, TIntV& InNIdV, TIntV& OutNIdV) {
  int NodeIdx = abs((NId.GetPrimHashCd()) % Reserved());
  int NodeKeyId = NodeH.AddKey13(NodeIdx, NId);
  NodeH[NodeKeyId] = TNode(NId);
  NodeH[NodeKeyId].InNIdV.MoveFrom(InNIdV);
  NodeH[NodeKeyId].OutNIdV.MoveFrom(OutNIdV);  
}
#endif // GCC_ATOMIC

