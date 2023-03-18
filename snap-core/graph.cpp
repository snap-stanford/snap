/////////////////////////////////////////////////
// Undirected Graph
bool TUNGraph::HasFlag(const TGraphFlag& Flag) const {
  return HasGraphFlag(TUNGraph::TNet, Flag);
}

// Add a node of ID NId to the graph.
int TUNGraph::AddNode(int NId) {
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    MxNId = TMath::Mx(NId+1, MxNId());
  }
  NodeH.AddDat(NId, TNode(NId));
  return NId;
}

// Add a node of ID NId to the graph.
int TUNGraph::AddNodeUnchecked(int NId) {
  if (IsNode(NId)) { return -1;}
  MxNId = TMath::Mx(NId+1, MxNId());
  NodeH.AddDat(NId, TNode(NId));
  return NId;
}

// Add a node of ID NId to the graph and create edges to all nodes in vector NbrNIdV.
int TUNGraph::AddNode(const int& NId, const TIntV& NbrNIdV) {
  int NewNId;
  if (NId == -1) {
    NewNId = MxNId;  MxNId++;
  } else {
    IAssertR(! IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    NewNId = NId;
    MxNId = TMath::Mx(NewNId+1, MxNId());
  }
  TNode& Node = NodeH.AddDat(NewNId);
  Node.Id = NewNId;
  Node.NIdV = NbrNIdV;
  Node.NIdV.Sort();
  NEdges += Node.GetDeg();
  for (int i = 0; i < NbrNIdV.Len(); i++) {
    GetNode(NbrNIdV[i]).NIdV.AddSorted(NewNId);
  }
  return NewNId;
}

// Add a node of ID NId to the graph and create edges to all nodes in the vector NIdVId in the vector pool Pool).
int TUNGraph::AddNode(const int& NId, const TVecPool<TInt>& Pool, const int& NIdVId) {
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
  Node.NIdV.GenExt(Pool.GetValVPt(NIdVId), Pool.GetVLen(NIdVId));
  Node.NIdV.Sort();
  NEdges += Node.GetDeg();
  return NewNId;
}

// Delete node of ID NId from the graph.
void TUNGraph::DelNode(const int& NId) {
  { AssertR(IsNode(NId), TStr::Fmt("NodeId %d does not exist", NId));
  TNode& Node = GetNode(NId);
  NEdges -= Node.GetDeg();
  for (int e = 0; e < Node.GetDeg(); e++) {
    const int nbr = Node.GetNbrNId(e);
    if (nbr == NId) { continue; }
    TNode& N = GetNode(nbr);
    const int n = N.NIdV.SearchBin(NId);
    IAssert(n != -1); // if NId points to N, then N also should point back
    if (n!= -1) { N.NIdV.Del(n); }
  } }
  NodeH.DelKey(NId);
}

int TUNGraph::GetEdges() const {
  //int Edges = 0;
  //for (int N=NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
  //  Edges += NodeH[N].GetDeg();
  //}
  //return Edges/2;
  return NEdges;
}

// Add an edge between SrcNId and DstNId to the graph.
int TUNGraph::AddEdge(const int& SrcNId, const int& DstNId) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  if (IsEdge(SrcNId, DstNId)) { return -2; } // edge already exists
  GetNode(SrcNId).NIdV.AddSorted(DstNId);
  if (SrcNId!=DstNId) { // not a self edge
    GetNode(DstNId).NIdV.AddSorted(SrcNId); }
  NEdges++;
  return -1; // no edge id
}

// Add an edge between SrcNId and DstNId to the graph.
int TUNGraph::AddEdgeUnchecked(const int& SrcNId, const int& DstNId) {
  GetNode(SrcNId).NIdV.Add(DstNId);
  if (SrcNId!=DstNId) { // not a self edge
    GetNode(DstNId).NIdV.Add(SrcNId); }
  NEdges++;
  return -1; // no edge id
}

// Add an edge between SrcNId and DstNId to the graph and create the nodes if they don't yet exist.
int TUNGraph::AddEdge2(const int& SrcNId, const int& DstNId) {
  if (! IsNode(SrcNId)) { AddNode(SrcNId); }
  if (! IsNode(DstNId)) { AddNode(DstNId); }
  if (GetNode(SrcNId).IsNbrNId(DstNId)) { return -2; } // edge already exists
  GetNode(SrcNId).NIdV.AddSorted(DstNId);
  if (SrcNId!=DstNId) { // not a self edge
    GetNode(DstNId).NIdV.AddSorted(SrcNId); }
  NEdges++;
  return -1; // no edge id
}

// Delete an edge between node IDs SrcNId and DstNId from the graph.
void TUNGraph::DelEdge(const int& SrcNId, const int& DstNId) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  { TNode& N = GetNode(SrcNId);
  const int n = N.NIdV.SearchBin(DstNId);
  if (n!= -1) { N.NIdV.Del(n);  NEdges--; } }
  if (SrcNId != DstNId) { // not a self edge
    TNode& N = GetNode(DstNId);
    const int n = N.NIdV.SearchBin(SrcNId);
    if (n!= -1) { N.NIdV.Del(n); }
  }
}

// Test whether an edge between node IDs SrcNId and DstNId exists the graph.
bool TUNGraph::IsEdge(const int& SrcNId, const int& DstNId) const {
  if (! IsNode(SrcNId) || ! IsNode(DstNId)) return false;
  return GetNode(SrcNId).IsNbrNId(DstNId);
}

// Return an iterator referring to edge (SrcNId, DstNId) in the graph.
TUNGraph::TEdgeI TUNGraph::GetEI(const int& SrcNId, const int& DstNId) const {
  const int MnNId = TMath::Mn(SrcNId, DstNId);
  const int MxNId = TMath::Mx(SrcNId, DstNId);
  const TNodeI SrcNI = GetNI(MnNId);
  const int NodeN = SrcNI.NodeHI.GetDat().NIdV.SearchBin(MxNId);
  IAssert(NodeN != -1);
  return TEdgeI(SrcNI, EndNI(), NodeN);
}

// Get a vector IDs of all nodes in the graph.
void TUNGraph::GetNIdV(TIntV& NIdV) const {
  NIdV.Gen(GetNodes(), 0);
  for (int N=NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    NIdV.Add(NodeH.GetKey(N)); }
}

// Defragment the graph.
void TUNGraph::Defrag(const bool& OnlyNodeLinks) {
  for (int n = NodeH.FFirstKeyId(); NodeH.FNextKeyId(n); ) {
    NodeH[n].NIdV.Pack();
  }
  if (! OnlyNodeLinks && ! NodeH.IsKeyIdEqKeyN()) {
    NodeH.Defrag();
  }
}

// Check the graph data structure for internal consistency.
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
      if (! IsNode(Node.GetNbrNId(e))) {
        const TStr Msg = TStr::Fmt("Edge %d --> %d: node %d does not exist.",
          Node.GetId(), Node.GetNbrNId(e), Node.GetNbrNId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); }
        RetVal=false;
      }
      if (e > 0 && prevNId == Node.GetNbrNId(e)) {
        const TStr Msg = TStr::Fmt("Node %d has duplicate edge %d --> %d.",
          Node.GetId(), Node.GetId(), Node.GetNbrNId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); }
        RetVal=false;
      }
      prevNId = Node.GetNbrNId(e);
    }
  }
  int EdgeCnt = 0;
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) { EdgeCnt++; }
  if (EdgeCnt != GetEdges()) {
    const TStr Msg = TStr::Fmt("Number of edges counter is corrupted: GetEdges():%d, EdgeCount:%d.", GetEdges(), EdgeCnt);
    if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); }
    RetVal=false;
  }
  return RetVal;
}

void TUNGraph::GetNeighbors(const int& NId, TIntV& NeighborsIdV){
  //check if the node exists
  AssertR(IsNode(NId), TStr::Fmt("NodeId %d does not exist", NId));

  //get the snap node corresponding to nodeId
  TNode& Node = GetNode(NId);

  int neighborCount = Node.GetDeg();

  NeighborsIdV.Gen(neighborCount, 0);
  
  //go through the entire degree (as it is undirected) of this node
  //NEdges -= Node.GetDeg();

  for (int e = 0; e < Node.GetDeg(); e++) {
    
    const int nbr = Node.GetNbrNId(e);
    
    if (nbr == NId) 
    { 
        continue; 
    }
    
    TNode& N = GetNode(nbr);
    
    const int n = N.NIdV.SearchBin(NId);
    
    IAssert(n != -1); // if NId points to N, then N also should point back
    
    //add neighbor to neighbors list
    if (n != -1) { 
      NeighborsIdV.Add(nbr); 
    }

  } 
}

// Print the graph in a human readable form to an output stream OutF.
void TUNGraph::Dump(FILE *OutF) const {
  const int NodePlaces = (int) ceil(log10((double) GetNodes()));
  fprintf(OutF, "-------------------------------------------------\nUndirected Node Graph: nodes: %d, edges: %d\n", GetNodes(), GetEdges());
  for (int N = NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    const TNode& Node = NodeH[N];
    fprintf(OutF, "  %*d [%d] ", NodePlaces, Node.GetId(), Node.GetDeg());
    for (int edge = 0; edge < Node.GetDeg(); edge++) {
      fprintf(OutF, " %*d", NodePlaces, Node.GetNbrNId(edge)); }
    fprintf(OutF, "\n");
  }
  fprintf(OutF, "\n");
}

// Return a small graph on 5 nodes and 5 edges.
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
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    MxNId = TMath::Mx(NId+1, MxNId());
  }
  NodeH.AddDat(NId, TNode(NId));
  return NId;
}

int TNGraph::AddNodeUnchecked(int NId) {
  if (IsNode(NId)) { return NId;}
  MxNId = TMath::Mx(NId+1, MxNId());
  NodeH.AddDat(NId, TNode(NId));
  return NId;
}

// add a node with a list of neighbors
// (use TNGraph::IsOk to check whether the graph is consistent)
int TNGraph::AddNode(const int& NId, const TIntV& InNIdV, const TIntV& OutNIdV) {
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
// (use TNGraph::IsOk to check whether the graph is consistent)
int TNGraph::AddNode(const int& NId, const TVecPool<TInt>& Pool, const int& SrcVId, const int& DstVId) {
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

void TNGraph::DelNode(const int& NId) {
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
  return -1; // no edge id
}

int TNGraph::AddEdgeUnchecked(const int& SrcNId, const int& DstNId) {
  GetNode(SrcNId).OutNIdV.Add(DstNId);
  GetNode(DstNId).InNIdV.Add(SrcNId);
  return -1; // no edge id
}

int TNGraph::AddEdge2(const int& SrcNId, const int& DstNId) {
  if (! IsNode(SrcNId)) { AddNode(SrcNId); }
  if (! IsNode(DstNId)) { AddNode(DstNId); }
  if (GetNode(SrcNId).IsOutNId(DstNId)) { return -2; } // edge already exists
  GetNode(SrcNId).OutNIdV.AddSorted(DstNId);
  GetNode(DstNId).InNIdV.AddSorted(SrcNId);
  return -1; // no edge id
}

void TNGraph::DelEdge(const int& SrcNId, const int& DstNId, const bool& IsDir) {
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

bool TNGraph::IsEdge(const int& SrcNId, const int& DstNId, const bool& IsDir) const {
  if (! IsNode(SrcNId) || ! IsNode(DstNId)) { return false; }
  if (IsDir) { return GetNode(SrcNId).IsOutNId(DstNId); }
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

void TNGraph::GetNeighbors(const int& NId, TIntV& NeighborsIdV, bool outDegreeNeighbors){
  //check if the node exists
  AssertR(IsNode(NId), TStr::Fmt("NodeId %d does not exist", NId));

  //get the snap node corresponding to nodeId
  TNode& Node = GetNode(NId);

  int neighborCount = 0;

  if(outDegreeNeighbors){
      neighborCount = Node.GetOutDeg();
  }
  else{
      neighborCount = Node.GetInDeg();
  }

  NeighborsIdV.Gen(neighborCount, 0);
  
  //go through the entire degree (as it is undirected) of this node
  for (int e = 0; e < neighborCount; e++) {
    int nbr; 
    
    if(outDegreeNeighbors){
      nbr = Node.GetOutNId(e);
    }
    else{
      nbr = Node.GetInNId(e);
    }

    if (nbr == NId) 
    { 
        continue; 
    }
    
    TNode& N = GetNode(nbr);
    
    if(outDegreeNeighbors){
        const int n = N.InNIdV.SearchBin(NId);
        if (n != -1) { 
          NeighborsIdV.Add(nbr);
        }
    }
    else{
        const int n = N.OutNIdV.SearchBin(NId);
        if (n != -1) { 
          NeighborsIdV.Add(nbr);
        }
    }
  }
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
  G->AddEdge(1,3); G->AddEdge(3,4); G->AddEdge(2,3);
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
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    MxNId = TMath::Mx(NId+1, MxNId());
  }
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
  IAssertR(!IsEdge(EId), TStr::Fmt("EdgeId %d already exists", EId));
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
void TNEGraph::DelEdge(const int& SrcNId, const int& DstNId, const bool& IsDir) {
  int EId;
  IAssert(IsEdge(SrcNId, DstNId, EId, IsDir)); // there is at least one edge
  while (IsEdge(SrcNId, DstNId, EId, IsDir)) {
    GetNode(SrcNId).OutEIdV.DelIfIn(EId);
    GetNode(DstNId).InEIdV.DelIfIn(EId);
  }
  EdgeH.DelKey(EId);
}

bool TNEGraph::IsEdge(const int& SrcNId, const int& DstNId, int& EId, const bool& IsDir) const {
  const TNode& SrcNode = GetNode(SrcNId);
  for (int edge = 0; edge < SrcNode.GetOutDeg(); edge++) {
    const TEdge& Edge = GetEdge(SrcNode.GetOutEId(edge));
    if (DstNId == Edge.GetDstNId()) {
      EId = Edge.GetId();  return true; }
  }
  if (! IsDir) {
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

// Return a small graph on 5 nodes and 6 edges.
PNEGraph TNEGraph::GetSmallGraph() {
  PNEGraph Graph = TNEGraph::New();
  for (int i = 0; i < 5; i++) { Graph->AddNode(i); }
  Graph->AddEdge(0,1);  Graph->AddEdge(0,2);
  Graph->AddEdge(0,3);  Graph->AddEdge(0,4);
  Graph->AddEdge(1,2);  Graph->AddEdge(1,2);
  return Graph;
}

/////////////////////////////////////////////////
// Bipartite graph
int TBPGraph::AddNode(int NId, const bool& LeftNode) {
  if (NId == -1) { NId = MxNId;  MxNId++; }
  else if (IsLNode(NId)) { IAssertR(LeftNode, TStr::Fmt("Node with id %s already exists on the 'left'.", NId));  return NId; }
  else if (IsRNode(NId)) { IAssertR(! LeftNode, TStr::Fmt("Node with id %s already exists on the 'right'.", NId));  return NId; }
  else { MxNId = TMath::Mx(NId+1, MxNId()); }
  if (LeftNode) { LeftH.AddDat(NId, TNode(NId)); }
  else { RightH.AddDat(NId, TNode(NId)); }
  return NId;
}

// Delete node of ID NId from the bipartite graph.
void TBPGraph::DelNode(const int& NId) {
  AssertR(IsNode(NId), TStr::Fmt("NodeId %d does not exist", NId));
  THash<TInt, TNode>& SrcH = IsLNode(NId) ? LeftH : RightH;
  THash<TInt, TNode>& DstH = IsLNode(NId) ? RightH : LeftH;
  { TNode& Node = SrcH.GetDat(NId);
  for (int e = 0; e < Node.GetOutDeg(); e++) {
    const int nbr = Node.GetOutNId(e);
    IAssertR(nbr != NId, "Bipartite graph has a loop!");
    TNode& N = DstH.GetDat(nbr);
    const int n = N.NIdV.SearchBin(NId);
    IAssert(n!= -1); 
    N.NIdV.Del(n);
  } }
  SrcH.DelKey(NId);
}

int TBPGraph::GetEdges() const {
  int Edges = 0;
  for (int N=LeftH.FFirstKeyId(); LeftH.FNextKeyId(N); ) {
    Edges += LeftH[N].GetDeg(); }
  return Edges;
}

int TBPGraph::AddEdge(const int& LeftNId, const int& RightNId) {
  const bool IsLL = IsLNode(LeftNId), IsLR = IsRNode(LeftNId);
  const bool IsRL = IsLNode(RightNId), IsRR = IsRNode(RightNId);
  IAssertR((IsLL||IsLR)&&(IsRL||IsRR), TStr::Fmt("%d or %d is not a node.", LeftNId, RightNId).CStr());
  IAssertR(LeftNId!=RightNId, "No self-edges are allowed."); 
  IAssertR((IsLL&&!IsLR&&!IsRL&&IsRR)||(!IsLL&&IsLR&&IsRL&&!IsRR), "One node should be on the 'left' and the other on the 'right'.");
  const int LNId = IsLL ? LeftNId : RightNId; // the real left node
  const int RNId = IsLL ? RightNId : LeftNId; // the real right node
  if (LeftH.GetDat(LNId).IsOutNId(RNId)) { return -2; } // edge already exists
  LeftH.GetDat(LNId).NIdV.AddSorted(RNId);
  RightH.GetDat(RNId).NIdV.AddSorted(LNId);
  return -1; // no edge id
}

void TBPGraph::DelEdge(const int& LeftNId, const int& RightNId) {
  const bool IsLL = IsLNode(LeftNId), IsLR = IsRNode(LeftNId);
  const bool IsRL = IsLNode(RightNId), IsRR = IsRNode(RightNId);
  IAssertR((IsLL||IsLR)&&(IsRL||IsRR), TStr::Fmt("%d or %d is not a node.", LeftNId, RightNId).CStr());
  IAssertR(LeftNId!=RightNId, "No self-edges are allowed."); 
  IAssertR((IsLL&&!IsLR&&!IsRL&&IsRR)||(!IsLL&&IsLR&&IsRL&&!IsRR), "One node should be on the 'left' and the other on the 'right'.");
  const int LNId = IsLL ? LeftNId : RightNId; // the real left node
  const int RNId = IsLL ? RightNId : LeftNId; // the real right node
  { TIntV& NIdV = LeftH.GetDat(LNId).NIdV;
  const int n = NIdV.SearchBin(RNId);
  if (n != -1) { NIdV.Del(n); } }
  { TIntV& NIdV = RightH.GetDat(RNId).NIdV;
  const int n = NIdV.SearchBin(LNId);
  if (n != -1) { NIdV.Del(n); } }
}

bool TBPGraph::IsEdge(const int& LeftNId, const int& RightNId) const {
  if (! IsNode(LeftNId) || ! IsNode(RightNId)) { return false; }
  return IsLNode(LeftNId) ? LeftH.GetDat(LeftNId).IsOutNId(RightNId) : RightH.GetDat(LeftNId).IsOutNId(RightNId);
}

TBPGraph::TEdgeI TBPGraph::GetEI(const int& LeftNId, const int& RightNId) const {
  const bool IsLL = IsLNode(LeftNId), IsLR = IsRNode(LeftNId);
  const bool IsRL = IsLNode(RightNId), IsRR = IsRNode(RightNId);
  IAssertR((IsLL||IsLR)&&(IsRL||IsRR), TStr::Fmt("%d or %d is not a node.", LeftNId, RightNId).CStr());
  IAssertR(LeftNId!=RightNId, "No self-edges are allowed."); 
  IAssertR((IsLL&&!IsLR&&!IsRL&&IsRR)||(!IsLL&&IsLR&&IsRL&&!IsRR), "One node should be on the 'left' and the other on the 'right'.");
  const int LNId = IsLL ? LeftNId : RightNId; // the real left node
  const int RNId = IsLL ? RightNId : LeftNId; // the real right node
  const TNodeI SrcNI = GetNI(LNId);
  const int NodeN = SrcNI.LeftHI.GetDat().NIdV.SearchBin(RNId);
  IAssertR(NodeN != -1, "Right edge endpoint does not exists!");
  return TEdgeI(SrcNI, EndNI(), NodeN);
}

int TBPGraph::GetRndNId(TRnd& Rnd) { 
  const int NNodes = GetNodes();
  if (Rnd.GetUniDevInt(NNodes) < GetLNodes()) {
    return GetRndLNId(Rnd); }
  else {
    return GetRndRNId(Rnd); }
}

int TBPGraph::GetRndLNId(TRnd& Rnd) { 
  return LeftH.GetKey(LeftH.GetRndKeyId(Rnd, 0.8)); 
}

int TBPGraph::GetRndRNId(TRnd& Rnd) { 
  return RightH.GetKey(RightH.GetRndKeyId(Rnd, 0.8)); 
}

void TBPGraph::GetNIdV(TIntV& NIdV) const {
  NIdV.Gen(GetNodes(), 0);
  for (int N=LeftH.FFirstKeyId(); LeftH.FNextKeyId(N); ) {
    NIdV.Add(LeftH.GetKey(N)); }
  for (int N=RightH.FFirstKeyId(); RightH.FNextKeyId(N); ) {
    NIdV.Add(RightH.GetKey(N)); }
}

void TBPGraph::GetLNIdV(TIntV& NIdV) const {
  NIdV.Gen(GetLNodes(), 0);
  for (int N=LeftH.FFirstKeyId(); LeftH.FNextKeyId(N); ) {
    NIdV.Add(LeftH.GetKey(N)); }
}

void TBPGraph::GetRNIdV(TIntV& NIdV) const {
  NIdV.Gen(GetRNodes(), 0);
  for (int N=RightH.FFirstKeyId(); RightH.FNextKeyId(N); ) {
    NIdV.Add(RightH.GetKey(N)); }
}

void TBPGraph::Reserve(const int& Nodes, const int& Edges) { 
  if (Nodes>0) { LeftH.Gen(Nodes/2); RightH.Gen(Nodes/2); } 
}

void TBPGraph::Defrag(const bool& OnlyNodeLinks) {
  for (int n = LeftH.FFirstKeyId(); LeftH.FNextKeyId(n); ) {
    LeftH[n].NIdV.Pack(); }
  for (int n = RightH.FFirstKeyId(); RightH.FNextKeyId(n); ) {
    RightH[n].NIdV.Pack(); }
  if (! OnlyNodeLinks && ! LeftH.IsKeyIdEqKeyN()) { LeftH.Defrag(); }
  if (! OnlyNodeLinks && ! RightH.IsKeyIdEqKeyN()) { RightH.Defrag(); }
}

bool TBPGraph::IsOk(const bool& ThrowExcept) const {
  bool RetVal = false;
  // edge lists are sorted
  for (int n = LeftH.FFirstKeyId(); LeftH.FNextKeyId(n); ) {
    if (! LeftH[n].NIdV.IsSorted()) {
      const TStr Msg = TStr::Fmt("Neighbor list of node %d is not sorted.", LeftH[n].GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false; }
  }
  for (int n = RightH.FFirstKeyId(); RightH.FNextKeyId(n); ) {
    if (! RightH[n].NIdV.IsSorted()) {
      const TStr Msg = TStr::Fmt("Neighbor list of node %d is not sorted.", RightH[n].GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false; }
  }
  // nodes only appear on one side
  for (int n = LeftH.FFirstKeyId(); LeftH.FNextKeyId(n); ) {
    if (RightH.IsKey(LeftH[n].GetId())) {
      const TStr Msg = TStr::Fmt("'Left' node %d also appears on the 'right'.", LeftH[n].GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false; }
  } 
  for (int n = RightH.FFirstKeyId(); RightH.FNextKeyId(n); ) {
    if (LeftH.IsKey(RightH[n].GetId())) {
      const TStr Msg = TStr::Fmt("'Right' node %d also appears on the 'left'.", RightH[n].GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false; }
  }
  // edges only point from left to right and right to left
  for (int n = LeftH.FFirstKeyId(); LeftH.FNextKeyId(n); ) {
    for (int e = 0; e < LeftH[n].NIdV.Len(); e++) {
      if (! RightH.IsKey(LeftH[n].NIdV[e]) || ! RightH.GetDat(LeftH[n].NIdV[e]).NIdV.IsIn(LeftH[n].GetId())) {
        const TStr Msg = TStr::Fmt("'Left' node %d does not point to the 'right' node %d.", LeftH[n].GetId(), LeftH[n].NIdV[e]());
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false; }
    }
  }
  for (int n = RightH.FFirstKeyId(); RightH.FNextKeyId(n); ) {
    for (int e = 0; e < RightH[n].NIdV.Len(); e++) {
      if (! LeftH.IsKey(RightH[n].NIdV[e]) || ! LeftH.GetDat(RightH[n].NIdV[e]).NIdV.IsIn(RightH[n].GetId())) {
        const TStr Msg = TStr::Fmt("'Left' node %d does not point to the 'right' node %d.", RightH[n].GetId(), RightH[n].NIdV[e]());
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false; }
    }
  }
  return RetVal;
}

void TBPGraph::Dump(FILE *OutF) const {
  const int NodePlaces = (int) ceil(log10((double) GetNodes()));
  fprintf(OutF, "-------------------------------------------------\nBipartite Graph: nodes: %d+%d=%d, edges: %d\n", GetLNodes(), GetRNodes(), GetNodes(), GetEdges());
  for (int N = LeftH.FFirstKeyId(); LeftH.FNextKeyId(N); ) {
    const TNode& Node = LeftH[N];
    fprintf(OutF, "  %*d [%d] ", NodePlaces, Node.GetId(), Node.GetDeg());
    for (int edge = 0; edge < Node.GetDeg(); edge++) {
      fprintf(OutF, " %*d", NodePlaces, Node.GetNbrNId(edge)); }
    fprintf(OutF, "\n");
  }
  fprintf(OutF, "\n");
  /*// Also dump the 'right' side
  fprintf(OutF, "\n");
  for (int N = RightH.FFirstKeyId(); RightH.FNextKeyId(N); ) {
    const TNode& Node = RightH[N];
    fprintf(OutF, "  %*d [%d] ", NodePlaces, Node.GetId(), Node.GetDeg());
    for (int edge = 0; edge < Node.GetDeg(); edge++) {
      fprintf(OutF, " %*d", NodePlaces, Node.GetNbrNId(edge)); }
    fprintf(OutF, "\n");
  }
  fprintf(OutF, "\n"); //*/
}

PBPGraph TBPGraph::GetSmallGraph() {
  PBPGraph BP = TBPGraph::New();
  BP->AddNode(0, true);
  BP->AddNode(1, true);
  BP->AddNode(2, false);
  BP->AddNode(3, false);
  BP->AddNode(4, false);
  BP->AddEdge(0, 2);
  BP->AddEdge(0, 3);
  BP->AddEdge(1, 2);
  BP->AddEdge(1, 3);
  BP->AddEdge(1, 4);
  return BP;
}

/////////////////////////////////////////////////
// Hypergraph
bool THGraph::HasFlag(const TGraphFlag& Flag) const {
  return HasGraphFlag(THGraph::TNet, Flag);
}

// Add a node of ID NId to the graph.
int THGraph::AddNode(int NId, TStr NName) {
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    MxNId = TMath::Mx(NId+1, MxNId());
  }
  NodeH.AddDat(NId, TNode(NId, NName));
  return NId;
}

// Add a node of ID NId to the graph.
int THGraph::AddNodeUnchecked(int NId, TStr NName) {
  if (IsNode(NId)) { return -1;}
  MxNId = TMath::Mx(NId+1, MxNId());
  NodeH.AddDat(NId, TNode(NId, NName));
  return NId;
}

// Delete node of ID NId from the graph.
void THGraph::DelNode(const int& NId) {
  AssertR(IsNode(NId), TStr::Fmt("NodeId %d does not exist", NId));
  TNode& Node = GetNode(NId);
  int EId = Node.EIdV.GetVal(0);
  int NumNei;
  TIntSet& ENIdsHS = GetEdge(EId).NeiNIdSH;
  for (int e = 0; e < Node.GetDeg(); e++) {
    // Remove node from neighboring nodes' neighbor list:
    EId = Node.EIdV.GetVal(e);
    ENIdsHS = GetEdge(EId).NeiNIdSH;
    int iKey;
    for (int i=0; i < ENIdsHS.Len(); i++) {
      iKey = ENIdsHS.GetKey(i);
      if (iKey == NId) { continue; }
      GetNode(iKey).DelNeighbor(NId);
    }
    /// Adjust N2Edges
    NumNei = ENIdsHS.Len();
    N2Edges = N2Edges - (NumNei*(NumNei-1))/2 + ((NumNei-1)*(NumNei-2))/2;
    // Remove NId from contributing edges/Delete edges with NId with size of 2:
    if (ENIdsHS.Len() > 2) {
      GetEdge(EId).NeiNIdSH.DelKey(NId);
    } else { DelEdge(EId); }
    // Remove node from HGraph (list of nodes):
    NodeH.DelKey(NId);
  }
  delete &Node;
}

void THGraph::TNode::UpdEInfo(const int& EId, const TIntSet& ENodesHS){
  if (! EIdV.IsIn(EId)) {
    EIdV.Add(EId);
    for (int j = 0; j < ENodesHS.Len(); j++) {
      if (Id == ENodesHS.GetKey(j)) { continue; }
      if (! NbrNIdENumH.IsKey(ENodesHS.GetKey(j))) {
        NbrNIdENumH.AddDat(ENodesHS.GetKey(j), 1);
      } else {
        NbrNIdENumH.AddDat(ENodesHS.GetKey(j), 1 + NbrNIdENumH.GetDat(ENodesHS.GetKey(j)));
      }
    }
  }
}

inline THGraph::TEdge::TEdge( THGraph* GraphPt, const TIntSet& NodeIdsHS) {
  TInt EIdCandidate = GraphPt->NEdges;
  while (GraphPt->EdgeH.IsKey(EIdCandidate)) { EIdCandidate++; }
  TEdge(EIdCandidate, NodeIdsHS, GraphPt);
}

inline THGraph::TEdge::TEdge( THGraph* GraphPt, const TIntV& NodeIdsV) {
  TInt EIdCandidate = GraphPt->NEdges;
  while (GraphPt->EdgeH.IsKey(EIdCandidate)) { EIdCandidate++; }
  TEdge(EIdCandidate, NodeIdsV, GraphPt);
}

void THGraph::TEdge::UpdNEInfo(const TIntSet& ENodesHS){
  for (int i = 0; i < ENodesHS.Len(); i++) {
    Graph->GetNode(ENodesHS.GetKey(i)).UpdEInfo(Id, ENodesHS);
  }
}

bool THGraph::IsEdge(const TIntSet& NIdH) {
  if (NIdH.Len() < 2) { return false; }
  if (! IsNode(NIdH.GetKey(0))) { return false; }
  int NId = NIdH.GetKey(0);
  TIntSet SharedEIdsH;
  NodeH.GetDat(NId).GetEIDs(SharedEIdsH);
  for (int n = 1; n< NIdH.Len(); n++){
    int N1Id = NIdH.GetKey(n);
    TIntSet NeiEIdH;
    NodeH.GetDat(N1Id).GetEIDs(NeiEIdH);
    TIntersect(SharedEIdsH, NeiEIdH);
    if (SharedEIdsH.Len()==0) { return false; }
  }
  for (THashSetKeyI<TInt> e = SharedEIdsH.BegI(); e < SharedEIdsH.EndI(); e++) {
    if(GetEI(e.GetKey()).Len() == NIdH.Len()) {
      return true;
    }
  }
  return false;
}

/// Add an edge between the nodes in NodeIdsHS set.
int THGraph::AddEdge(const TIntSet& NIdH, int& EId) {
  if (IsEdge(NIdH)) { return -1; }
  EId = TMath::Mx(EId, MxEId());
  MxEId = EId + 1;
  IAssertR(!IsEdgeId(EId), TStr::Fmt("EdgeId %d already exists", EId));
  EdgeH.AddDat(EId, TEdge(EId, NIdH, this));
  EdgeH.GetDat(EId).UpdNEInfo(NIdH);
  NEdges++;
  N2Edges += (NIdH.Len() * (NIdH.Len()-1))/2;
  return EId;
}

int THGraph::AssertNodes(const TIntSet& NodesIS) {
  int NKey;
  for (int N=0; N < NodesIS.Len(); N++) {
    NKey = NodesIS.GetKey(N);
    if (! IsNode(NKey)) {return NKey;}
  }
  return -1;
}

void THGraph::DelEdge(const int& EId) {
  IAssertR(IsEdgeId(EId), TStr::Fmt("EdgeId %d not found", EId));
  TIntSet NodeIdsHS = GetEdge(EId).NeiNIdSH;
  int iKey, jKey;
  for (int i=NodeIdsHS.FFirstKeyId(); NodeIdsHS.FNextKeyId(i); ) {
    iKey = NodeIdsHS.GetKey(i);
    for (int j=NodeIdsHS.FFirstKeyId(); NodeIdsHS.FNextKeyId(j); ) {
      if (i==j) { continue; }
      jKey = NodeIdsHS.GetKey(j);
      GetNode(iKey).DelNeighbor(jKey);
    }
  }
  EdgeH.DelKey(EId);
  int ESize = GetEdge(EId).NeiNIdSH.Len();
  NEdges--;
  N2Edges -= (ESize*(ESize-1))/2;
  delete &GetEdge(EId);
}

// Get a vector IDs of all nodes in the graph.
void THGraph::GetNIdV(TIntV& NIdV) const {
  NIdV.Gen(GetNodes(), 0);
  for (int N=NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    NIdV.Add(NodeH.GetKey(N)); }
}

// Defragment the graph.
void THGraph::Defrag(const bool& OnlyNodeLinks) {
  int nKey;
  for (int n = EdgeH.FFirstKeyId(); EdgeH.FNextKeyId(n); ) {
    nKey = EdgeH.GetKey(n);
    if (! EdgeH.GetDat(nKey).NeiNIdSH.IsKeyIdEqKeyN()) {
      EdgeH.GetDat(nKey).NeiNIdSH.Defrag();
    }
  }
  for (int n = NodeH.FFirstKeyId(); NodeH.FNextKeyId(n); ) {
    nKey = NodeH.GetKey(n);
    NodeH.GetDat(nKey).EIdV.Pack();
    if (! NodeH.GetDat(nKey).NbrNIdENumH.IsKeyIdEqKeyN()) {
      NodeH.GetDat(nKey).NbrNIdENumH.Defrag();
    }
  }
  if (! OnlyNodeLinks) {
    if (! NodeH.IsKeyIdEqKeyN()){ NodeH.Defrag(); }
    if (! EdgeH.IsKeyIdEqKeyN()){ EdgeH.Defrag(); }
  }
}

void THGraph::PrintEdge(const int EId) {
  if (! EdgeH.IsKey(EId)) {
    printf("\nEdge Not Found!\n");
  }
  TIntV NV;
  TStr EStr("Edge nodes: ");
  EdgeH.GetDat(EId).GetNodesV(NV);
  for (int i = 0; i < NV.Len(); i++) {
    EStr += (" " + NV[i].GetStr());
  }
  EStr += "\n";
  printf(EStr.GetCStr());
}

// Print the graph in a human readable form to an output stream OutF.
void THGraph::Dump(FILE *OutF) const {
  const int NodePlaces = (int) ceil(log10((double) GetNodes()));
  int NKey;
  for (int N = NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    NKey = NodeH.GetKey(N);
    const TNode& Node = NodeH.GetDat(NKey);
    fprintf(OutF, "  %*d [%d] ", NodePlaces, Node.GetId(), Node.GetDeg());
    for (int edge = 0; edge < Node.GetDeg(); edge++) {
      fprintf(OutF, " %*d", NodePlaces, Node.GetNbrNId(edge)); }
    fprintf(OutF, "\n");
  }
  fprintf(OutF, "\n");
}

// Return a small hypergraph on 5 nodes and 5 edges.
PHGraph THGraph::GetSmallGraph() {
  PHGraph Graph = THGraph::New();
  for (int i = 0; i < 5; i++) { Graph->AddNode(i); }
  TIntV EV(3);
  int el[] = {0,1,3};
  for (int i = 0; i < 3; i++)
    EV.Add(el[i]);
  Graph->AddEdge(EV);
  return Graph;
}
