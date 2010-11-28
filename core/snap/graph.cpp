/////////////////////////////////////////////////
// Undirected Graph
bool TUNGraph::HasFlag(const TGraphFlag& Flag) const {
  return HasGraphFlag(TUNGraph::TNet, Flag);
}

int TUNGraph::AddNode(int NId) {
  if (NId == -1) { NId = MxNId;  MxNId++; }
  else if (IsNode(NId)) { return NId; } // already a node
  else { MxNId = TMath::Mx(NId+1, MxNId()); }
  NodeH.AddDat(NId, TNode(NId));
  return NId;
}

// add a node with a list of neighbors
// (use TUNGraph::IsOk to check whether the graph is consistent)
int TUNGraph::AddNode(const int& NId, const TIntV& NbhNIdV) {
  IAssert(NId != -1);
  IAssertR(! IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
  MxNId = TMath::Mx(NId+1, MxNId());
  TNode& Node = NodeH.AddDat(NId);
  Node.Id = NId;
  Node.NIdV = NbhNIdV;
  Node.NIdV.Sort();
  return NId;
}

// add a node from a vector pool
// (use TUNGraph::IsOk to check whether the graph is consistent)
int TUNGraph::AddNode(const int& NId, const TVecPool<TInt>& Pool, const int& NIdVId) {
  IAssert(NId != -1);
  IAssertR(! IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
  MxNId = TMath::Mx(NId+1, MxNId());
  TNode& Node = NodeH.AddDat(NId);
  Node.Id = NId;
  Node.NIdV.GenExt(Pool.GetValVPt(NIdVId), Pool.GetVLen(NIdVId));
  Node.NIdV.Sort();
  return NId;
}

void TUNGraph::DelNode(const int& NId) {
  { TNode& Node = GetNode(NId);
  for (int e = 0; e < Node.GetDeg(); e++) {
	const int nbh = Node.GetNbhNId(e);
	if (nbh == NId) { continue; }
    TNode& N = GetNode(nbh);
    const int n = N.NIdV.SearchBin(NId);
    if (n!= -1) { N.NIdV.Del(n); }
  } }
  NodeH.DelKey(NId);
}

int TUNGraph::GetEdges() const {
  int Edges = 0;
  for (int N=NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    Edges += NodeH[N].GetDeg();
  }
  return Edges/2;
}

int TUNGraph::AddEdge(const int& SrcNId, const int& DstNId) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  //IAssert(! IsEdge(SrcNId, DstNId));
  if (IsEdge(SrcNId, DstNId)) { return -2; } // edge already exists
  GetNode(SrcNId).NIdV.AddSorted(DstNId);
  if (SrcNId!=DstNId) { // not a self edge
    GetNode(DstNId).NIdV.AddSorted(SrcNId); }
  return -1; // edge id
}

void TUNGraph::DelEdge(const int& SrcNId, const int& DstNId) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  { TNode& N = GetNode(SrcNId);
  int n = N.NIdV.SearchBin(DstNId);
  if (n!= -1) { N.NIdV.Del(n); } }
  if (SrcNId != DstNId) { // not a self edge
    TNode& N = GetNode(DstNId);
    int n = N.NIdV.SearchBin(SrcNId);
    if (n!= -1) { N.NIdV.Del(n); }
  }
}

bool TUNGraph::IsEdge(const int& SrcNId, const int& DstNId) const {
  if (! IsNode(SrcNId) || ! IsNode(DstNId)) return false;
  return GetNode(SrcNId).IsNbhNId(DstNId);
}

TUNGraph::TEdgeI TUNGraph::GetEI(const int& SrcNId, const int& DstNId) const {
  const TNodeI SrcNI = GetNI(SrcNId);
  const int NodeN = SrcNI.NodeHI.GetDat().NIdV.SearchBin(DstNId);
  IAssert(NodeN != -1);
  return TEdgeI(SrcNI, EndNI(), NodeN);
}

void TUNGraph::GetNIdV(TIntV& NIdV) const {
  NIdV.Gen(GetNodes(), 0);
  for (int N=NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    NIdV.Add(NodeH.GetKey(N)); }
}

void TUNGraph::Defrag(const bool& OnlyNodeLinks) {
  for (int n = NodeH.FFirstKeyId(); NodeH.FNextKeyId(n); ) {
    NodeH[n].NIdV.Pack();
  }
  if (! OnlyNodeLinks && ! NodeH.IsKeyIdEqKeyN()) {
    NodeH.Defrag();
  }
}

// for each node check that their neighbors are also nodes
bool TUNGraph::IsOk(const bool& ThrowExcept) const {
  bool RetVal = true;
  for (int N = NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    const TNode& Node = NodeH[N];
    if (! Node.NIdV.IsSorted()) {
      const TStr Msg = TStr::Fmt("Neighbor list of node %d is not sorted.", Node.GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); }
      RetVal=false;
    }
    int prevNId = -1;
    for (int e = 0; e < Node.GetDeg(); e++) {
      if (! IsNode(Node.GetNbhNId(e))) {
        const TStr Msg = TStr::Fmt("Edge %d --> %d: node %d does not exist.",
          Node.GetId(), Node.GetNbhNId(e), Node.GetNbhNId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); }
        RetVal=false;
      }
      if (e > 0 && prevNId == Node.GetNbhNId(e)) {
        const TStr Msg = TStr::Fmt("Node %d has duplidate edge %d --> %d.",
          Node.GetId(), Node.GetId(), Node.GetNbhNId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); }
        RetVal=false;
      }
      prevNId = Node.GetNbhNId(e);
    }
  }
  return RetVal;
}

void TUNGraph::Dump(FILE *OutF) const {
  const int NodePlaces = (int) ceil(log10((double) GetNodes()));
  fprintf(OutF, "-------------------------------------------------\nUndirected Node Graph: nodes: %d, edges: %d\n", GetNodes(), GetEdges());
  for (int N = NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    const TNode& Node = NodeH[N];
    fprintf(OutF, "  %*d [%d] ", NodePlaces, Node.GetId(), Node.GetDeg());
    for (int edge = 0; edge < Node.GetDeg(); edge++) {
      fprintf(OutF, " %*d", NodePlaces, Node.GetNbhNId(edge)); }
    fprintf(OutF, "\n");
  }
  fprintf(OutF, "\n");
}

//Graph:   3--0--4
//           /|
//          1-2
PUNGraph TUNGraph::GetSmallGraph() {
  PUNGraph Graph = TUNGraph::New();
  for (int i = 0; i < 5; i++) { Graph->AddNode(i); }
  Graph->AddEdge(0,1);  Graph->AddEdge(0,2);
  Graph->AddEdge(0,3);  Graph->AddEdge(0,4);
  Graph->AddEdge(1,2);
  return Graph;
}

/////////////////////////////////////////////////
// Directed Node Graph
bool TNGraph::HasFlag(const TGraphFlag& Flag) const {
  return HasGraphFlag(TNGraph::TNet, Flag);
}

int TNGraph::AddNode(int NId) {
  if (NId == -1) { NId = MxNId;  MxNId++; }
  else if (IsNode(NId)) { return NId; } // already a node
  else { MxNId = TMath::Mx(NId+1, MxNId()); }
  NodeH.AddDat(NId, TNode(NId));
  return NId;
}

// add a node with a list of neighbors
// (use TNGraph::IsOk to check whether the graph is consistent)
int TNGraph::AddNode(const int& NId, const TIntV& InNIdV, const TIntV& OutNIdV) {
  IAssert(NId != -1);
  IAssertR(! IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
  MxNId = TMath::Mx(NId+1, MxNId());
  TNode& Node = NodeH.AddDat(NId);
  Node.Id = NId;
  Node.InNIdV = InNIdV;
  Node.OutNIdV = OutNIdV;
  Node.InNIdV.Sort();
  Node.OutNIdV.Sort();
  return NId;
}

// add a node from a vector pool
// (use TNGraph::IsOk to check whether the graph is consistent)
int TNGraph::AddNode(const int& NId, const TVecPool<TInt>& Pool, const int& SrcVId, const int& DstVId) {
  IAssert(NId!=-1);
  IAssertR(! IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
  MxNId = TMath::Mx(NId+1, MxNId());
  TNode& Node = NodeH.AddDat(NId);
  Node.Id = NId;
  Node.InNIdV.GenExt(Pool.GetValVPt(SrcVId), Pool.GetVLen(SrcVId));
  Node.OutNIdV.GenExt(Pool.GetValVPt(DstVId), Pool.GetVLen(DstVId));
  Node.InNIdV.Sort();
  Node.OutNIdV.Sort();
  return NId;
}

void TNGraph::DelNode(const int& NId) {
  { TNode& Node = GetNode(NId);
  for (int e = 0; e < Node.GetOutDeg(); e++) {
	const int nbh = Node.GetOutNId(e);
	if (nbh == NId) { continue; }
    TNode& N = GetNode(nbh);
    int n = N.InNIdV.SearchBin(NId);
    if (n!= -1) { N.InNIdV.Del(n); }
  }
  for (int e = 0; e < Node.GetInDeg(); e++) {
	const int nbh = Node.GetInNId(e);
	if (nbh == NId) { continue; }
    TNode& N = GetNode(nbh);
    int n = N.OutNIdV.SearchBin(NId);
    if (n!= -1) { N.OutNIdV.Del(n); }
  } }
  NodeH.DelKey(NId);
}

int TNGraph::GetEdges() const {
  int edges=0;
  for (int N=NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    edges+=NodeH[N].GetOutDeg();
  }
  return edges;
}

int TNGraph::AddEdge(const int& SrcNId, const int& DstNId) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  //IAssert(! IsEdge(SrcNId, DstNId));
  if (IsEdge(SrcNId, DstNId)) { return -2; }
  GetNode(SrcNId).OutNIdV.AddSorted(DstNId);
  GetNode(DstNId).InNIdV.AddSorted(SrcNId);
  return -1; // edge id
}

void TNGraph::DelEdge(const int& SrcNId, const int& DstNId, const bool& Dir) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  { TNode& N = GetNode(SrcNId);
  int n = N.OutNIdV.SearchBin(DstNId);
  if (n!= -1) { N.OutNIdV.Del(n); } }
  { TNode& N = GetNode(DstNId);
  int n = N.InNIdV.SearchBin(SrcNId);
  if (n!= -1) { N.InNIdV.Del(n); } }
  if (! Dir) {
    { TNode& N = GetNode(SrcNId);
    int n = N.InNIdV.SearchBin(DstNId);
    if (n!= -1) { N.InNIdV.Del(n); } }
    { TNode& N = GetNode(DstNId);
    int n = N.OutNIdV.SearchBin(SrcNId);
    if (n!= -1) { N.OutNIdV.Del(n); } }
  }
}

bool TNGraph::IsEdge(const int& SrcNId, const int& DstNId, const bool& Dir) const {
  if (! IsNode(SrcNId) || ! IsNode(DstNId)) { return false; }
  if (Dir) { return GetNode(SrcNId).IsOutNId(DstNId); }
  else { return GetNode(SrcNId).IsOutNId(DstNId) || GetNode(DstNId).IsOutNId(SrcNId); }
}

TNGraph::TEdgeI TNGraph::GetEI(const int& SrcNId, const int& DstNId) const {
  const TNodeI SrcNI = GetNI(SrcNId);
  const int NodeN = SrcNI.NodeHI.GetDat().OutNIdV.SearchBin(DstNId);
  IAssert(NodeN != -1);
  return TEdgeI(SrcNI, EndNI(), NodeN);
}

void TNGraph::GetNIdV(TIntV& NIdV) const {
  NIdV.Gen(GetNodes(), 0);
  for (int N=NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    NIdV.Add(NodeH.GetKey(N)); }
}

void TNGraph::Defrag(const bool& OnlyNodeLinks) {
  for (int n = NodeH.FFirstKeyId(); NodeH.FNextKeyId(n); ) {
    TNode& Node = NodeH[n];
    Node.InNIdV.Pack();  Node.OutNIdV.Pack();
  }
  if (! OnlyNodeLinks && ! NodeH.IsKeyIdEqKeyN()) { NodeH.Defrag(); }
}

// for each node check that their neighbors are also nodes
bool TNGraph::IsOk(const bool& ThrowExcept) const {
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

void TNGraph::Dump(FILE *OutF) const {
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

PNGraph TNGraph::GetSmallGraph() {
  PNGraph G = TNGraph::New();
  for (int i = 0; i < 5; i++) { G->AddNode(i); }
  G->AddEdge(0,1); G->AddEdge(1,2); G->AddEdge(0,2);
  G->AddEdge(1,3);
  G->AddEdge(3,4);
  G->AddEdge(2,3);
  return G;
}

/////////////////////////////////////////////////
// Node Edge Graph
bool TNEGraph::HasFlag(const TGraphFlag& Flag) const {
  return HasGraphFlag(TNEGraph::TNet, Flag);
}

bool TNEGraph::TNodeI::IsInNId(const int& NId) const {
  const TNode& Node = NodeHI.GetDat();
  for (int edge = 0; edge < Node.GetInDeg(); edge++) {
    if (NId == Graph->GetEdge(Node.GetInEId(edge)).GetSrcNId())
      return true;
  }
  return false;
}

bool TNEGraph::TNodeI::IsOutNId(const int& NId) const {
  const TNode& Node = NodeHI.GetDat();
  for (int edge = 0; edge < Node.GetOutDeg(); edge++) {
    if (NId == Graph->GetEdge(Node.GetOutEId(edge)).GetDstNId())
      return true;
  }
  return false;
}

int TNEGraph::AddNode(int NId) {
  if (NId == -1) { NId = MxNId;  MxNId++; }
  else if (IsNode(NId)) { return NId; } // already a node
  else { MxNId = TMath::Mx(NId+1, MxNId()); }
  NodeH.AddDat(NId, TNode(NId));
  return NId;
}

void TNEGraph::DelNode(const int& NId) {
  const TNode& Node = GetNode(NId);
  for (int out = 0; out < Node.GetOutDeg(); out++) {
    const int EId = Node.GetOutEId(out);
    const TEdge& Edge = GetEdge(EId);
    IAssert(Edge.GetSrcNId() == NId);
    GetNode(Edge.GetDstNId()).InEIdV.DelIfIn(EId);
    EdgeH.DelKey(EId);
  }
  for (int in = 0; in < Node.GetInDeg(); in++) {
    const int EId = Node.GetInEId(in);
    const TEdge& Edge = GetEdge(EId);
    IAssert(Edge.GetDstNId() == NId);
    GetNode(Edge.GetSrcNId()).OutEIdV.DelIfIn(EId);
    EdgeH.DelKey(EId);
  }
  NodeH.DelKey(NId);
}

int TNEGraph::AddEdge(const int& SrcNId, const int& DstNId, int EId) {
  if (EId == -1) { EId = MxEId;  MxEId++; }
  else { MxEId = TMath::Mx(EId+1, MxEId()); }
  IAssert(! IsEdge(EId));
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  EdgeH.AddDat(EId, TEdge(EId, SrcNId, DstNId));
  GetNode(SrcNId).OutEIdV.AddSorted(EId);
  GetNode(DstNId).InEIdV.AddSorted(EId);
  return EId;
}

void TNEGraph::DelEdge(const int& EId) {
  IAssert(IsEdge(EId));
  const int SrcNId = GetEdge(EId).GetSrcNId();
  const int DstNId = GetEdge(EId).GetDstNId();
  GetNode(SrcNId).OutEIdV.DelIfIn(EId);
  GetNode(DstNId).InEIdV.DelIfIn(EId);
  EdgeH.DelKey(EId);
}

// delete all edges between the two nodes
void TNEGraph::DelEdge(const int& SrcNId, const int& DstNId, const bool& Dir) {
  int EId;
  IAssert(IsEdge(SrcNId, DstNId, EId, Dir)); // there is at least one edge
  while (IsEdge(SrcNId, DstNId, EId, Dir)) {
    GetNode(SrcNId).OutEIdV.DelIfIn(EId);
    GetNode(DstNId).InEIdV.DelIfIn(EId);
  }
  EdgeH.DelKey(EId);
}

bool TNEGraph::IsEdge(const int& SrcNId, const int& DstNId, int& EId, const bool& Dir) const {
  const TNode& SrcNode = GetNode(SrcNId);
  for (int edge = 0; edge < SrcNode.GetOutDeg(); edge++) {
    const TEdge& Edge = GetEdge(SrcNode.GetOutEId(edge));
    if (DstNId == Edge.GetDstNId()) {
      EId = Edge.GetId();  return true; }
  }
  if (! Dir) {
    for (int edge = 0; edge < SrcNode.GetInDeg(); edge++) {
    const TEdge& Edge = GetEdge(SrcNode.GetInEId(edge));
    if (DstNId == Edge.GetSrcNId()) {
      EId = Edge.GetId();  return true; }
    }
  }
  return false;
}

void TNEGraph::GetNIdV(TIntV& NIdV) const {
  NIdV.Gen(GetNodes(), 0);
  for (int N=NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    NIdV.Add(NodeH.GetKey(N)); }
}

void TNEGraph::GetEIdV(TIntV& EIdV) const {
  EIdV.Gen(GetEdges(), 0);
  for (int E=EdgeH.FFirstKeyId(); EdgeH.FNextKeyId(E); ) {
    EIdV.Add(EdgeH.GetKey(E));
  }
}

void TNEGraph::Defrag(const bool& OnlyNodeLinks) {
  for (int kid = NodeH.FFirstKeyId(); NodeH.FNextKeyId(kid); ) {
    TNode& Node = NodeH[kid];
    Node.InEIdV.Pack();  Node.OutEIdV.Pack();
  }
  if (! OnlyNodeLinks && ! NodeH.IsKeyIdEqKeyN()) { NodeH.Defrag(); }
  if (! OnlyNodeLinks && ! EdgeH.IsKeyIdEqKeyN()) { EdgeH.Defrag(); }
}

bool TNEGraph::IsOk(const bool& ThrowExcept) const {
bool RetVal = true;
  for (int N = NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    const TNode& Node = NodeH[N];
    if (! Node.OutEIdV.IsSorted()) {
      const TStr Msg = TStr::Fmt("Out-edge list of node %d is not sorted.", Node.GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
    if (! Node.InEIdV.IsSorted()) {
      const TStr Msg = TStr::Fmt("In-edge list of node %d is not sorted.", Node.GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
    // check out-edge ids
    int prevEId = -1;
    for (int e = 0; e < Node.GetOutDeg(); e++) {
      if (! IsEdge(Node.GetOutEId(e))) {
        const TStr Msg = TStr::Fmt("Out-edge id %d of node %d does not exist.",  Node.GetOutEId(e), Node.GetId());
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      if (e > 0 && prevEId == Node.GetOutEId(e)) {
        const TStr Msg = TStr::Fmt("Node %d has duplidate out-edge id %d.", Node.GetId(), Node.GetOutEId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      prevEId = Node.GetOutEId(e);
    }
    // check in-edge ids
    prevEId = -1;
    for (int e = 0; e < Node.GetInDeg(); e++) {
      if (! IsEdge(Node.GetInEId(e))) {
        const TStr Msg = TStr::Fmt("Out-edge id %d of node %d does not exist.",  Node.GetInEId(e), Node.GetId());
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      if (e > 0 && prevEId == Node.GetInEId(e)) {
        const TStr Msg = TStr::Fmt("Node %d has duplidate out-edge id %d.", Node.GetId(), Node.GetInEId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      prevEId = Node.GetInEId(e);
    }
  }
  for (int E = EdgeH.FFirstKeyId(); EdgeH.FNextKeyId(E); ) {
    const TEdge& Edge = EdgeH[E];
    if (! IsNode(Edge.GetSrcNId())) {
      const TStr Msg = TStr::Fmt("Edge %d source node %d does not exist.", Edge.GetId(), Edge.GetSrcNId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
    if (! IsNode(Edge.GetDstNId())) {
      const TStr Msg = TStr::Fmt("Edge %d destination node %d does not exist.", Edge.GetId(), Edge.GetDstNId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
  }
  return RetVal;
}

void TNEGraph::Dump(FILE *OutF) const {
  const int NodePlaces = (int) ceil(log10((double) GetNodes()));
  const int EdgePlaces = (int) ceil(log10((double) GetEdges()));
  fprintf(OutF, "-------------------------------------------------\nDirected Node-Edge Graph: nodes: %d, edges: %d\n", GetNodes(), GetEdges());
  for (TNodeI NodeI = BegNI(); NodeI < EndNI(); NodeI++) {
    fprintf(OutF, "  %*d]\n", NodePlaces, NodeI.GetId());
    fprintf(OutF, "    in[%d]", NodeI.GetInDeg());
    for (int edge = 0; edge < NodeI.GetInDeg(); edge++) {
      fprintf(OutF, " %*d", EdgePlaces, NodeI.GetInEId(edge)); }
    fprintf(OutF, "\n    out[%d]", NodeI.GetOutDeg());
    for (int edge = 0; edge < NodeI.GetOutDeg(); edge++) {
      fprintf(OutF, " %*d", EdgePlaces, NodeI.GetOutEId(edge)); }
    fprintf(OutF, "\n");
  }
  for (TEdgeI EdgeI = BegEI(); EdgeI < EndEI(); EdgeI++) {
    fprintf(OutF, "  %*d]  %*d  ->  %*d\n", EdgePlaces, EdgeI.GetId(), NodePlaces, EdgeI.GetSrcNId(), NodePlaces, EdgeI.GetDstNId());
  }
  fprintf(OutF, "\n");
}
