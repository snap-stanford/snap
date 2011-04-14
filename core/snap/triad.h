namespace TSnap {

/////////////////////////////////////////////////
// Triads and clustering coefficient
template <class PGraph> double GetClustCf(const PGraph& Graph, int SampleNodes=-1);
template <class PGraph> double GetClustCf(const PGraph& Graph, TFltPrV& DegToCCfV, int SampleNodes=-1);
template <class PGraph> double GetClustCf(const PGraph& Graph, TFltPrV& DegToCCfV, int& ClosedTriads, int& OpenTriads, int SampleNodes=-1);

template <class PGraph> double GetNodeClustCf(const PGraph& Graph, const int& NId);
template <class PGraph> void GetNodeClustCf(const PGraph& Graph, TIntFltH& NIdCCfH);

// get triples of connected nodes (regardless of the number of edges between each pair of nodes)
template <class PGraph> int GetTriads(const PGraph& Graph, int SampleNodes=-1);
template <class PGraph> int GetTriads(const PGraph& Graph, int& ClosedTriads, int& OpenTriads, int SampleNodes);
template <class PGraph> void GetTriads(const PGraph& Graph, TIntTrV& NIdCOTriadV, int SampleNodes=-1);

template <class PGraph> int GetNodeTriads(const PGraph& Graph, const int& NId);
template <class PGraph> int GetNodeTriads(const PGraph& Graph, const int& NId, int& ClosedTriads, int& OpenTriads);
template <class PGraph> void GetTriadParticip(const PGraph& Graph, TIntPrV& TriadCntV);

template<class PGraph> int GetCmnNbhs(const PGraph& Graph, const int& NId1, const int& NId2);
template<class PGraph> int GetCmnNbhs(const PGraph& Graph, const int& NId1, const int& NId2, TIntV& NbhV);
template<class PGraph> int GetLen2Paths(const PGraph& Graph, const int& NId1, const int& NId2);
template<class PGraph> int GetLen2Paths(const PGraph& Graph, const int& NId1, const int& NId2, TIntV& NbhV);

/////////////////////////////////////////////////
// Implementation

// average clustering coefficient as defined in Watts and Strogatz
// in Collective dynamics of 'small-world' networks
template <class PGraph> double GetClustCf(const PGraph& Graph, int SampleNodes) {
  TIntTrV NIdCOTriadV;
  GetTriads(Graph, NIdCOTriadV, SampleNodes);
  if (NIdCOTriadV.Empty()) { return 0.0; }
  double SumCcf = 0.0;
  for (int i = 0; i < NIdCOTriadV.Len(); i++) {
    const double OpenCnt = NIdCOTriadV[i].Val2()+NIdCOTriadV[i].Val3();
    if (OpenCnt > 0) {
      SumCcf += NIdCOTriadV[i].Val2() / OpenCnt; }
  }
  IAssert(SumCcf>=0);
  return SumCcf / double(NIdCOTriadV.Len());
}

template <class PGraph> double GetClustCf(const PGraph& Graph, TFltPrV& DegToCCfV, int SampleNodes) {
  TIntTrV NIdCOTriadV;
  GetTriads(Graph, NIdCOTriadV, SampleNodes);
  THash<TInt, TFltPr> DegSumCnt;
  double SumCcf = 0.0;
  for (int i = 0; i < NIdCOTriadV.Len(); i++) {
    const int D = NIdCOTriadV[i].Val2()+NIdCOTriadV[i].Val3();
    const double Ccf = D!=0 ? NIdCOTriadV[i].Val2() / double(D) : 0.0;
    TFltPr& SumCnt = DegSumCnt.AddDat(Graph->GetNI(NIdCOTriadV[i].Val1).GetDeg());
    SumCnt.Val1 += Ccf;
    SumCnt.Val2 += 1;
    SumCcf += Ccf;
  }
  // get average clustering coefficient for each degree
  DegToCCfV.Gen(DegSumCnt.Len(), 0);
  for (int d = 0; d  < DegSumCnt.Len(); d++) {
    DegToCCfV.Add(TFltPr(DegSumCnt.GetKey(d), DegSumCnt[d].Val1()/DegSumCnt[d].Val2()));
  }
  DegToCCfV.Sort();
  return SumCcf / double(NIdCOTriadV.Len());
}

template <class PGraph>
double GetClustCf(const PGraph& Graph, TFltPrV& DegToCCfV, int& ClosedTriads, int& OpenTriads, int SampleNodes) {
  TIntTrV NIdCOTriadV;
  GetTriads(Graph, NIdCOTriadV, SampleNodes);
  THash<TInt, TFltPr> DegSumCnt;
  double SumCcf = 0.0;
  uint64 closedTriads = 0;
  uint64 openTriads = 0;
  for (int i = 0; i < NIdCOTriadV.Len(); i++) {
    const int D = NIdCOTriadV[i].Val2()+NIdCOTriadV[i].Val3();
    const double Ccf = D!=0 ? NIdCOTriadV[i].Val2() / double(D) : 0.0;
    closedTriads += NIdCOTriadV[i].Val2;
    openTriads += NIdCOTriadV[i].Val3;
    TFltPr& SumCnt = DegSumCnt.AddDat(Graph->GetNI(NIdCOTriadV[i].Val1).GetDeg());
    SumCnt.Val1 += Ccf;
    SumCnt.Val2 += 1;
    SumCcf += Ccf;
  }
  // get average clustering coefficient for each degree
  DegToCCfV.Gen(DegSumCnt.Len(), 0);
  for (int d = 0; d  < DegSumCnt.Len(); d++) {
    DegToCCfV.Add(TFltPr(DegSumCnt.GetKey(d).Val, DegSumCnt[d].Val1()/DegSumCnt[d].Val2()));
  }
  if(closedTriads/3 > (uint64) TInt::Mx) { WarnNotify(TStr::Fmt("[%s line %d] %g closed triads.\n", __FILE__, __LINE__, float(closedTriads/3)).CStr());  }
  if(openTriads > (uint64) TInt::Mx) { WarnNotify(TStr::Fmt("[%s line %d] %g open triads.\n", __FILE__, __LINE__, float(openTriads/3)).CStr());  }
  ClosedTriads = int(closedTriads/3); // each triad is counted 3 times
  OpenTriads = int(openTriads);
  DegToCCfV.Sort();
  return SumCcf / double(NIdCOTriadV.Len());
}

template <class PGraph> 
double GetNodeClustCf(const PGraph& Graph, const int& NId) {
  int Open, Closed;
  GetNodeTriads(Graph, NId, Open, Closed);
  const double Deg = Graph->GetNI(NId).GetDeg();
  return (Open+Closed)==0 ? 0 : double(Open)/double(Open+Closed);
}

template <class PGraph> 
void GetNodeClustCf(const PGraph& Graph, TIntFltH& NIdCCfH) {
  TIntTrV NIdCOTriadV;
  GetTriads(Graph, NIdCOTriadV);
  NIdCCfH.Clr(false);
  for (int i = 0; i < NIdCOTriadV.Len(); i++) {
    const int D = NIdCOTriadV[i].Val2()+NIdCOTriadV[i].Val3();
    const double CCf = D!=0 ? NIdCOTriadV[i].Val2() / double(D) : 0.0;
    NIdCCfH.AddDat(NIdCOTriadV[i].Val1, CCf);
  }
}

template <class PGraph>
int GetTriads(const PGraph& Graph, int SampleNodes) {
  int OpenTriads, ClosedTriads;
  return GetTriads(Graph, ClosedTriads, OpenTriads);
}

template <class PGraph>
int GetTriads(const PGraph& Graph, int& ClosedTriads, int& OpenTriads, int SampleNodes) {
  TIntTrV NIdCOTriadV;
  GetTriads(Graph, NIdCOTriadV, SampleNodes);
  uint64 closedTriads = 0;
  uint64 openTriads = 0;
  for (int i = 0; i < NIdCOTriadV.Len(); i++) {
    closedTriads += NIdCOTriadV[i].Val2;
    openTriads += NIdCOTriadV[i].Val3;
  }
  IAssert(closedTriads/3 < (uint64) TInt::Mx);
  IAssert(openTriads < (uint64) TInt::Mx);
  ClosedTriads = int(closedTriads/3); // each triad is counted 3 times
  OpenTriads = int(openTriads);
  return ClosedTriads;
}

/// Note: pretends that the graph is undirected (count unique connected triples of nodes)
template <class PGraph>
void GetTriads(const PGraph& Graph, TIntTrV& NIdCOTriadV, int SampleNodes) {
  const bool IsDir = Graph->HasFlag(gfDirected);
  TIntSet NbhH;
  TIntV NIdV;
  Graph->GetNIdV(NIdV);
  NIdV.Shuffle(TInt::Rnd);
  if (SampleNodes == -1) {
    SampleNodes = Graph->GetNodes(); }
  NIdCOTriadV.Clr(false);
  NIdCOTriadV.Reserve(SampleNodes);
  for (int node = 0; node < SampleNodes; node++) {
    typename PGraph::TObj::TNodeI NI = Graph->GetNI(NIdV[node]);
    if (NI.GetDeg() < 2) { 
      NIdCOTriadV.Add(TIntTr(NI.GetId(), 0, 0)); // zero triangles
      continue; 
    }
    // find neighborhood
    NbhH.Clr(false);
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      if (NI.GetOutNId(e) != NI.GetId()) {
        NbhH.AddKey(NI.GetOutNId(e)); }
    }
    if (IsDir) {
      for (int e = 0; e < NI.GetInDeg(); e++) {
        if (NI.GetInNId(e) != NI.GetId()) {
          NbhH.AddKey(NI.GetInNId(e)); }
      }
    }
    // count connected neighbors
    int OpenCnt=0, CloseCnt=0;
    for (int srcNbh = 0; srcNbh < NbhH.Len(); srcNbh++) {
      const typename PGraph::TObj::TNodeI SrcNode = Graph->GetNI(NbhH.GetKey(srcNbh));
      for (int dstNbh = srcNbh+1; dstNbh < NbhH.Len(); dstNbh++) {
        const int dstNId = NbhH.GetKey(dstNbh);
        if (SrcNode.IsNbhNId(dstNId)) { CloseCnt++; } // is edge
        else { OpenCnt++; }
      }
    }
    IAssert(2*(OpenCnt+CloseCnt) == NbhH.Len()*(NbhH.Len()-1));
    NIdCOTriadV.Add(TIntTr(NI.GetId(), CloseCnt, OpenCnt));
  }
}

/// Return number of undirected triads a node participates in
template <class PGraph>
int GetNodeTriads(const PGraph& Graph, const int& NId) {
  int ClosedTriads=0, OpenTriads=0;
  return GetNodeTriads(Graph, NId, ClosedTriads, OpenTriads);
}

/// Return number of undirected triads a node participates in
template <class PGraph> 
int GetNodeTriads(const PGraph& Graph, const int& NId, int& ClosedTriads, int& OpenTriads) {
  const typename PGraph::TObj::TNodeI NI = Graph->GetNI(NId);
  ClosedTriads=0;  OpenTriads=0;
  if (NI.GetDeg() < 2) { return 0; }
  // find neighborhood
  TIntSet NbhSet(NI.GetDeg());
  for (int e = 0; e < NI.GetOutDeg(); e++) {
    if (NI.GetOutNId(e) != NI.GetId()) { // exclude self edges
      NbhSet.AddKey(NI.GetOutNId(e)); }
  }
  if (Graph->HasFlag(gfDirected)) {
    for (int e = 0; e < NI.GetInDeg(); e++) {
      if (NI.GetInNId(e) != NI.GetId()) { // exclude self edges
        NbhSet.AddKey(NI.GetInNId(e)); }
    }
  }
  // count connected neighbors
  for (int srcNbh = 0; srcNbh < NbhSet.Len(); srcNbh++) {
    const typename PGraph::TObj::TNodeI SrcNode = Graph->GetNI(NbhSet.GetKey(srcNbh));
    for (int dstNbh = srcNbh+1; dstNbh < NbhSet.Len(); dstNbh++) {
      const int dstNId = NbhSet.GetKey(dstNbh);
      if (SrcNode.IsNbhNId(dstNId)) { ClosedTriads++; }
      else { OpenTriads++; }
    }
  }
  return ClosedTriads;
}

// For each node count how many triangles it participates in
template <class PGraph>
void GetTriadParticip(const PGraph& Graph, TIntPrV& TriadCntV) {
  TIntH TriadCntH;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    const int Triads = GetNodeTriads(Graph, NI.GetId());
    TriadCntH.AddDat(Triads) += 1;
  }
  TriadCntH.GetKeyDatPrV(TriadCntV);
  TriadCntV.Sort();
}

template<class PGraph>
int GetCmnNbhs(const PGraph& Graph, const int& NId1, const int& NId2) {
  TIntV NbhV;
  return GetCmnNbhs(Graph, NId1, NId2, NbhV);
}

// Get common neighbors between a pair of nodes (undirected)
template<class PGraph>
int GetCmnNbhs(const PGraph& Graph, const int& NId1, const int& NId2, TIntV& NbhV) {
  if (! Graph->IsNode(NId1) || ! Graph->IsNode(NId2)) { NbhV.Clr(false); return 0; }
  typename PGraph::TObj::TNodeI NI1 = Graph->GetNI(NId1);
  typename PGraph::TObj::TNodeI NI2 = Graph->GetNI(NId2);
  NbhV.Clr(false);
  NbhV.Reserve(TMath::Mn(NI1.GetDeg(), NI2.GetDeg()));
  TIntSet NSet1(NI1.GetDeg()), NSet2(NI2.GetDeg());
  for (int i = 0; i < NI1.GetDeg(); i++) {
    const int nid = NI1.GetNbhNId(i);
    if (nid!=NId1 && nid!=NId2) {
      NSet1.AddKey(nid); }
  }
  for (int i = 0; i < NI2.GetDeg(); i++) {
    const int nid = NI2.GetNbhNId(i);
    if (NSet1.IsKey(nid)) {
      NSet2.AddKey(nid);
    }
  }
  NSet2.GetKeyV(NbhV);
  return NbhV.Len();
}

template<>
inline int GetCmnNbhs<PUNGraph>(const PUNGraph& Graph, const int& NId1, const int& NId2, TIntV& NbhV) {
  if (! Graph->IsNode(NId1) || ! Graph->IsNode(NId2)) { NbhV.Clr(false); return 0; }
  const TUNGraph::TNodeI NI1 = Graph->GetNI(NId1);
  const TUNGraph::TNodeI NI2 = Graph->GetNI(NId2);
  int i=0, j=0;
  NbhV.Clr(false);
  NbhV.Reserve(TMath::Mn(NI1.GetDeg(), NI2.GetDeg()));
  while (i < NI1.GetDeg() && j < NI2.GetDeg()) {
    const int nid = NI1.GetNbhNId(i);
    while (j < NI2.GetDeg() && NI2.GetNbhNId(j) < nid) { j++; }
    if (j < NI2.GetDeg() && nid==NI2.GetNbhNId(j) && nid!=NId1 && nid!=NId2) {
      IAssert(NbhV.Empty() || NbhV.Last() < nid);
      NbhV.Add(nid);
      j++;
    }
    i++;
  }
  return NbhV.Len();
}

// get number of length 2 directed paths between a pair of nodes
// for a pair of nodes (i,j): |{u: (i,u) and (u,j) }|
template<class PGraph>
int GetLen2Paths(const PGraph& Graph, const int& NId1, const int& NId2) {
  TIntV NbhV;
  return GetLen2Paths(Graph, NId1, NId2, NbhV);
}

// get number of length 2 directed paths between a pair of nodes
// for a pair of nodes (i,j): {u: (i,u) and (u,j) }
template<class PGraph>
int GetLen2Paths(const PGraph& Graph, const int& NId1, const int& NId2, TIntV& NbhV) {
  const typename PGraph::TNodeI NI = Graph->GetNI(NId1);
  NbhV.Clr(false);
  NbhV.Reserve(NI.GetOutDeg());
  for (int e = 0; e < NI.GetOutDeg(); e++) {
    const typename PGraph::TNodeI MidNI = GetNI(NI.GetOutNId(e));
    if (MidNI.IsOutNId(NId2)) {
      NbhV.Add(MidNI.GetId());
    }
  }
  return NbhV.Len();
}

}; // mamespace TSnap

/////////////////////////////////////////////////
// Node and Edge Network Constraint (by Ron Burt)
// works for directed and undirected graphs (but not for multigraphs)
template <class PGraph>
class TNetConstraint {
public:
  PGraph Graph;
  THash<TIntPr, TFlt> NodePrCH; // pairs of nodes that have non-zero network constraint
public:
  TNetConstraint(const PGraph& GraphPt, const bool& CalcaAll=true);
  int Len() const { return NodePrCH.Len(); }
  double GetC(const int& ConstraintN) const { return NodePrCH[ConstraintN]; }
  TIntPr GetNodePr(const int& ConstraintN) const { return NodePrCH.GetKey(ConstraintN); }
  double GetEdgeC(const int& NId1, const int& NId2) const;
  double GetNodeC(const int& NId) const;
  void AddConstraint(const int& NId1, const int& NId2);
  void CalcConstraints();
  void CalcConstraints(const int& NId);
  void Dump() const;
  static void Test();
};

template <class PGraph>
TNetConstraint<PGraph>::TNetConstraint(const PGraph& GraphPt, const bool& CalcaAll) : Graph(GraphPt) {
  CAssert(! HasGraphFlag(typename PGraph::TObj, gfMultiGraph)); // must not be multigraph
  if (CalcaAll) {
    CalcConstraints();
  }
}

template <class PGraph>
double TNetConstraint<PGraph>::GetEdgeC(const int& NId1, const int& NId2) const {
  if (NodePrCH.IsKey(TIntPr(NId1, NId2))) {
    return NodePrCH.GetDat(TIntPr(NId1, NId2)); }
  else {
    return 0.0; }
}

template <class PGraph>
double TNetConstraint<PGraph>::GetNodeC(const int& NId) const {
  typename PGraph::TObj::TNodeI NI1 = Graph->GetNI(NId);
  if (NI1.GetOutDeg() == 0) { return 0.0; }
  int KeyId = -1;
  for (int k = 0; k<NI1.GetOutDeg(); k++) { 
    KeyId = NodePrCH.GetKeyId(TIntPr(NI1.GetId(), NI1.GetOutNId(k)));
    if (KeyId > -1) { break; }
  }
  if (KeyId < 0) { return 0.0; }
  double Constraint = NodePrCH[KeyId];
  for (int i = KeyId-1; i >-1 && NodePrCH.GetKey(i).Val1()==NId; i--) {
    Constraint += NodePrCH[i];
  }
  for (int i = KeyId+1; i < NodePrCH.Len() && NodePrCH.GetKey(i).Val1()==NId; i++) {
    Constraint += NodePrCH[i];
  }
  return Constraint;
}

template <class PGraph>
void TNetConstraint<PGraph>::AddConstraint(const int& NId1, const int& NId2) {
  if (NId1==NId2 || NodePrCH.IsKey(TIntPr(NId1, NId2))) {
    return;
  }
  typename PGraph::TObj::TNodeI NI1 = Graph->GetNI(NId1);
  double Constraint = 0.0;
  if (NI1.IsOutNId(NId2)) { // is direct edge
    Constraint += 1.0/(double) NI1.GetOutDeg();
  }
  const double SrcC = 1.0/(double) NI1.GetOutDeg();
  for (int e = 0; e < NI1.GetOutDeg(); e++) {
    const int MidNId = NI1.GetOutNId(e);
    if (MidNId == NId1 || MidNId == NId2) { continue; }
    const typename PGraph::TObj::TNodeI MidNI = Graph->GetNI(MidNId);
    if (MidNI.IsOutNId(NId2)) {
      Constraint += SrcC * (1.0/(double)MidNI.GetOutDeg());
    }
  }
  if (Constraint==0) { return; }
  Constraint = TMath::Sqr(Constraint);
  NodePrCH.AddDat(TIntPr(NId1, NId2), Constraint);
}

template <class PGraph>
void TNetConstraint<PGraph>::CalcConstraints() {
  // add edges
  for (typename PGraph::TObj::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++) {
    AddConstraint(EI.GetSrcNId(), EI.GetDstNId());
    AddConstraint(EI.GetDstNId(), EI.GetSrcNId());
  }
  // add open triads
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    for (int i = 0; i < NI.GetDeg();  i++) {
      const int NId1 = NI.GetNbhNId(i);
      for (int j = 0; j < NI.GetDeg();  j++) {
        const int NId2 = NI.GetNbhNId(j);
        AddConstraint(NId1, NId2);
      }
    }
  }
  NodePrCH.SortByKey();
}

// calculate constraints around a node id
template <class PGraph>
void TNetConstraint<PGraph>::CalcConstraints(const int& NId) {
  typename PGraph::TObj::TNodeI StartNI = Graph->GetNI(NId);
  TIntSet SeenSet;
  for (int e = 0; e < StartNI.GetOutDeg(); e++) {
    typename PGraph::TObj::TNodeI MidNI = Graph->GetNI(StartNI.GetOutNId(e));
    AddConstraint(NId, MidNI.GetId());
    for (int i = 0; i < MidNI.GetOutDeg();  i++) {
      const int EndNId = MidNI.GetOutNId(i);
      if (! SeenSet.IsKey(EndNId)) {
        AddConstraint(NId, EndNId);
        SeenSet.AddKey(EndNId);
      }
    }
  }
}

template <class PGraph>
void TNetConstraint<PGraph>::Dump() const {
  printf("Edge network constraint: (%d, %d)\n", Graph->GetNodes(), Graph->GetEdges());
  for (int e = 0; e < NodePrCH.Len(); e++) {
    printf("  %4d %4d  :  %f\n", NodePrCH.GetKey(e).Val1(), NodePrCH.GetKey(e).Val2(), NodePrCH[e].Val);
  }
  printf("\n");
}

// example from page 56 of Structural Holes by Ronald S. Burt
// (http://www.amazon.com/Structural-Holes-Social-Structure-Competition)
template <class PGraph>
void TNetConstraint<PGraph>::Test() {
  PUNGraph G = TUNGraph::New();
  G->AddNode(0); G->AddNode(1); G->AddNode(2); G->AddNode(3);
  G->AddNode(4); G->AddNode(5); G->AddNode(6);
  G->AddEdge(0,1); G->AddEdge(0,2); G->AddEdge(0,3); G->AddEdge(0,4); G->AddEdge(0,5); G->AddEdge(0,6);
  G->AddEdge(1,2); G->AddEdge(1,5);  G->AddEdge(1,6);
  G->AddEdge(2,4);
  TNetConstraint<PUNGraph> NetConstraint(G, true);
  // NetConstraint.CalcConstraints(0);
  NetConstraint.Dump();
  printf("middle node network constraint: %f\n", NetConstraint.GetNodeC(0));
}

