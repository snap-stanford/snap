namespace TSnap {

/////////////////////////////////////////////////
// Triads and clustering coefficient

/// Computes the average clustering coefficient as defined in Watts and Strogatz, Collective dynamics of 'small-world' networks.
template <class PGraph> double GetClustCf(const PGraph& Graph, int SampleNodes=-1);
/// Computes the distribution of average clustering coefficient.
/// @param DegToCCfV Vector of pairs (degree, avg. clustering coefficient of nodes of that degree).
/// @param SampleNodes If !=-1 then compute clustering coefficient only for a random sample of SampleNodes nodes. Useful for approximate but quick computations.
template <class PGraph> double GetClustCf(const PGraph& Graph, TFltPrV& DegToCCfV, int SampleNodes=-1);
/// Computes the distribution of average clustering coefficient as well as the number of open and closed triads in the graph.
/// @param DegToCCfV Vector of pairs (degree, avg. clustering coefficient of nodes of that degree).
/// @param SampleNodes If !=-1 then compute clustering coefficient only for a random sample of SampleNodes nodes. Useful for approximate but quick computations.
template <class PGraph> double GetClustCf(const PGraph& Graph, TFltPrV& DegToCCfV, int& ClosedTriads, int& OpenTriads, int SampleNodes=-1);
/// Returns clustering coefficient of a particular node.
template <class PGraph> double GetNodeClustCf(const PGraph& Graph, const int& NId);
/// Computes clustering coefficient of each node of the Graph.
template <class PGraph> void GetNodeClustCf(const PGraph& Graph, TIntFltH& NIdCCfH);

/// Returns the number of triangles in a graph. Function returns the number of unique triples of connected nodes (regardless of the number of edges between each pair of nodes).
/// @param SampleNodes If !=-1 then compute triads only for a random sample of SampleNodes nodes. Useful for approximate but quick computations.
template <class PGraph> int GetTriads(const PGraph& Graph, int SampleNodes=-1);
/// Computes the number of Closed and Open triads.
/// @param SampleNodes If !=-1 then compute triads only for a random sample of SampleNodes nodes. Useful for approximate but quick computations.
template <class PGraph> int GetTriads(const PGraph& Graph, int& ClosedTriads, int& OpenTriads, int SampleNodes);
/// Computes the number of open and close triads for every node of the network.
/// @param NIdCOTriadV Triple (node id, open triads: number of pairs of node's neighbors that are not connected, closed triads: number of pairs of node's neighbors that are connected between themselves).
/// @param SampleNodes If !=-1 then compute triads only for a random sample of SampleNodes nodes. Useful for approximate but quick computations.
template <class PGraph> void GetTriads(const PGraph& Graph, TIntTrV& NIdCOTriadV, int SampleNodes=-1);
/// Counts the number of edges that participate in at least one triad
/// @param SampleNodes If !=-1 then compute triads only for a random sample of SampleNodes nodes. Useful for approximate but quick computations.
template <class PGraph> int GetTriadEdges(const PGraph& Graph, int SampleEdges=-1);

/// Returns number of undirected triads a node NId participates in.
template <class PGraph> int GetNodeTriads(const PGraph& Graph, const int& NId);
/// Returns number of undirected Open and Closed triads a node NId participates in.
template <class PGraph> int GetNodeTriads(const PGraph& Graph, const int& NId, int& ClosedTriads, int& OpenTriads);
/// Returns the number of triads between a node NId and a subset of its neighbors GroupSet.
/// @param InGroupEdges  Number of triads triads (NId, g1, g2), where g1 and g2 are in GroupSet
/// @param InOutGroupEdges Number of triads (NId, g1, o1), where g1 in GroupSet and o1 not in GroupSet
template <class PGraph> int GetNodeTriads(const PUNGraph& Graph, const int& NId, const TIntSet& GroupSet, int& InGroupEdges, int& InOutGroupEdges);
/// Returns the number of triads between a node NId and a subset of its neighbors GroupSet.
/// @param InGroupEdges Number of triads (NId, g1, g2), where g1 and g2 are in GroupSet
/// @param InOutGroupEdges Number of triads (NId, g1, o1), where g1 in GroupSet and o1 not in GroupSet
/// @param OutGroupEdges Number of triads (NId, p1, o1), where o1 and o2 are not in GroupSet
template <class PGraph> int GetNodeTriads(const PUNGraph& Graph, const int& NId, const TIntSet& GroupSet, int& InGroupEdges, int& InOutGroupEdges, int& OutGroup);
/// Triangle Participation Ratio: For each node counts how many triangles it participates in and then returns a set of pairs (number of triangles, number of such nodes).
template <class PGraph> void GetTriadParticip(const PGraph& Graph, TIntPrV& TriadCntV);

/// Returns a number of shared neighbors between a pair of nodes NId1 and NId2.
template<class PGraph> int GetCmnNbrs(const PGraph& Graph, const int& NId1, const int& NId2);
/// Returns the shared neighbors between a pair of nodes NId1 and NId2.
template<class PGraph> int GetCmnNbrs(const PGraph& Graph, const int& NId1, const int& NId2, TIntV& NbrV);
/// Returns the number of length 2 directed paths between a pair of nodes NId1, NId2 (NId1 --> U --> NId2).
template<class PGraph> int GetLen2Paths(const PGraph& Graph, const int& NId1, const int& NId2);
/// Returns the 2 directed paths between a pair of nodes NId1, NId2 (NId1 --> U --> NId2). NbrV intermediary stores nodes U.
template<class PGraph> int GetLen2Paths(const PGraph& Graph, const int& NId1, const int& NId2, TIntV& NbrV);


/////////////////////////////////////////////////
// Implementation

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
    DegToCCfV.Add(TFltPr(DegSumCnt.GetKey(d), double(DegSumCnt[d].Val1()/DegSumCnt[d].Val2())));
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
  return GetTriads(Graph, ClosedTriads, OpenTriads, SampleNodes);
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

// Function pretends that the graph is undirected (count unique connected triples of nodes)
template <class PGraph>
void GetTriads(const PGraph& Graph, TIntTrV& NIdCOTriadV, int SampleNodes) {
  const bool IsDir = Graph->HasFlag(gfDirected);
  TIntSet NbrH;
  TIntV NIdV;
  TRnd Rnd(0);

  Graph->GetNIdV(NIdV);
  NIdV.Shuffle(Rnd);
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
    NbrH.Clr(false);
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      if (NI.GetOutNId(e) != NI.GetId()) {
        NbrH.AddKey(NI.GetOutNId(e)); }
    }
    if (IsDir) {
      for (int e = 0; e < NI.GetInDeg(); e++) {
        if (NI.GetInNId(e) != NI.GetId()) {
          NbrH.AddKey(NI.GetInNId(e)); }
      }
    }
    // count connected neighbors
    int OpenCnt=0, CloseCnt=0;
    for (int srcNbr = 0; srcNbr < NbrH.Len(); srcNbr++) {
      const typename PGraph::TObj::TNodeI SrcNode = Graph->GetNI(NbrH.GetKey(srcNbr));
      for (int dstNbr = srcNbr+1; dstNbr < NbrH.Len(); dstNbr++) {
        const int dstNId = NbrH.GetKey(dstNbr);
        if (SrcNode.IsNbrNId(dstNId)) { CloseCnt++; } // is edge
        else { OpenCnt++; }
      }
    }
    IAssert(2*(OpenCnt+CloseCnt) == NbrH.Len()*(NbrH.Len()-1));
    NIdCOTriadV.Add(TIntTr(NI.GetId(), CloseCnt, OpenCnt));
  }
}

// Count the number of edges that participate in at least one triad
template <class PGraph>
int GetTriadEdges(const PGraph& Graph, int SampleEdges) {
  const bool IsDir = Graph->HasFlag(gfDirected);
  TIntSet NbrH;
  int TriadEdges = 0;
  for(typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    NbrH.Clr(false);
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      if (NI.GetOutNId(e) != NI.GetId()) {
        NbrH.AddKey(NI.GetOutNId(e)); }
    }
    if (IsDir) {
      for (int e = 0; e < NI.GetInDeg(); e++) {
        if (NI.GetInNId(e) != NI.GetId()) {
          NbrH.AddKey(NI.GetInNId(e)); }
      }
    }
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      if (!IsDir && NI.GetId()<NI.GetOutNId(e)) { continue; } // for undirected graphs count each edge only once
      const typename PGraph::TObj::TNodeI SrcNode = Graph->GetNI(NI.GetOutNId(e));
      bool Triad=false;
      for (int e1 = 0; e1 < SrcNode.GetOutDeg(); e1++) {
        if (NbrH.IsKey(SrcNode.GetOutNId(e1))) { Triad=true; break; }
      }
      if (IsDir && ! Triad) {
        for (int e1 = 0; e1 < SrcNode.GetInDeg(); e1++) {
          if (NbrH.IsKey(SrcNode.GetInNId(e1))) { Triad=true; break; }
        }
      }
      if (Triad) { TriadEdges++; }
    }
  }
  return TriadEdges;
}

// Returns number of undirected triads a node participates in
template <class PGraph>
int GetNodeTriads(const PGraph& Graph, const int& NId) {
  int ClosedTriads=0, OpenTriads=0;
  return GetNodeTriads(Graph, NId, ClosedTriads, OpenTriads);
}

// Return number of undirected triads a node participates in
template <class PGraph>
int GetNodeTriads(const PGraph& Graph, const int& NId, int& ClosedTriads, int& OpenTriads) {
  const typename PGraph::TObj::TNodeI NI = Graph->GetNI(NId);
  ClosedTriads=0;  OpenTriads=0;
  if (NI.GetDeg() < 2) { return 0; }
  // find neighborhood
  TIntSet NbrSet(NI.GetDeg());
  for (int e = 0; e < NI.GetOutDeg(); e++) {
    if (NI.GetOutNId(e) != NI.GetId()) { // exclude self edges
      NbrSet.AddKey(NI.GetOutNId(e)); }
  }
  if (Graph->HasFlag(gfDirected)) {
    for (int e = 0; e < NI.GetInDeg(); e++) {
      if (NI.GetInNId(e) != NI.GetId()) { // exclude self edges
        NbrSet.AddKey(NI.GetInNId(e)); }
    }
  }
  // count connected neighbors
  for (int srcNbr = 0; srcNbr < NbrSet.Len(); srcNbr++) {
    const typename PGraph::TObj::TNodeI SrcNode = Graph->GetNI(NbrSet.GetKey(srcNbr));
    for (int dstNbr = srcNbr+1; dstNbr < NbrSet.Len(); dstNbr++) {
      const int dstNId = NbrSet.GetKey(dstNbr);
      if (SrcNode.IsNbrNId(dstNId)) { ClosedTriads++; }
      else { OpenTriads++; }
    }
  }
  return ClosedTriads;
}

// Node NId and a subset of its neighbors GroupSet
//   InGroupEdges ... triads (NId, g1, g2), where g1 and g2 are in GroupSet
//   InOutGroupEdges ... triads (NId, g1, o1), where g1 in GroupSet and o1 not in GroupSet
template <class PGraph>
int GetNodeTriads(const PGraph& Graph, const int& NId, const TIntSet& GroupSet, int& InGroupEdges, int& InOutGroupEdges) {
  const typename PGraph::TObj::TNodeI NI = Graph->GetNI(NId);
  const bool IsDir = Graph->HasFlag(gfDirected);
  InGroupEdges=0;   InOutGroupEdges=0;
  if (NI.GetDeg() < 2 || GroupSet.Empty()) { return 0; }
  // find neighborhood
  TIntSet NbrSet(NI.GetDeg());
  for (int e = 0; e < NI.GetOutDeg(); e++) {
    if (NI.GetOutNId(e) != NI.GetId()) { // exclude self edges
      NbrSet.AddKey(NI.GetOutNId(e)); }
  }
  if (IsDir) {
    for (int e = 0; e < NI.GetInDeg(); e++) {
      if (NI.GetInNId(e) != NI.GetId()) {
        NbrSet.AddKey(NI.GetInNId(e)); }
    }
  }
  // count connected neighbors
  for (int srcGrp = 0; srcGrp < GroupSet.Len(); srcGrp++) {
    IAssert(NbrSet.IsKey(GroupSet[srcGrp]));
    const typename PGraph::TObj::TNodeI SrcNode = Graph->GetNI(GroupSet.GetKey(srcGrp));
    for (int i = 0; i < SrcNode.GetOutDeg(); i++) {
      const int dst = SrcNode.GetOutNId(i);
      if (! NbrSet.IsKey(dst)) { continue; } // not triangle
      if (GroupSet.IsKey(dst)) { InGroupEdges++; }
      else { InOutGroupEdges++; }
    }
  }
  return InGroupEdges;
}

// Node NId and a subset of its neighbors GroupSet
//   InGroupEdges ... triads (NId, g1, g2), where g1 and g2 are in GroupSet
//   InOutGroupEdges ... triads (NId, g1, o1), where g1 in GroupSet and o1 not in GroupSet
//   OutGroupEdges ... triads (NId, p1, o1), where o1 and o2 are not in GroupSet
template <class PGraph>
int GetNodeTriads(const PGraph& Graph, const int& NId, const TIntSet& GroupSet, int& InGroupEdges, int& InOutGroupEdges, int& OutGroupEdges) {
  const typename PGraph::TObj::TNodeI NI = Graph->GetNI(NId);
  const bool IsDir = Graph->HasFlag(gfDirected);
  InGroupEdges=0;  InOutGroupEdges=0;  OutGroupEdges=0;
  if (NI.GetDeg() < 2 || GroupSet.Empty()) { return 0; }
  // find neighborhood
  TIntSet NbrSet(NI.GetDeg());
  for (int e = 0; e < NI.GetOutDeg(); e++) {
    if (NI.GetOutNId(e) != NI.GetId()) { // exclude self edges
      NbrSet.AddKey(NI.GetOutNId(e)); }
  }
  if (IsDir) {
    for (int e = 0; e < NI.GetInDeg(); e++) {
      if (NI.GetInNId(e) != NI.GetId()) {
        NbrSet.AddKey(NI.GetInNId(e)); }
    }
  }
  // count connected neighbors
  for (int srcNbr = 0; srcNbr < NbrSet.Len(); srcNbr++) {
    const int NbrId = NbrSet.GetKey(srcNbr);
    const bool NbrIn = GroupSet.IsKey(NbrId);
    const typename PGraph::TObj::TNodeI SrcNode = Graph->GetNI(NbrId);
    for (int dstNbr = srcNbr+1; dstNbr < NbrSet.Len(); dstNbr++) {
      const int DstNId = NbrSet.GetKey(dstNbr);
      if (SrcNode.IsNbrNId(DstNId)) { // triad (NId, NbrId, DstNid)
        bool DstIn = GroupSet.IsKey(DstNId);
        if (NbrIn && DstIn) { InGroupEdges++; }
        else if (NbrIn || DstIn) { InOutGroupEdges++; }
        else { OutGroupEdges++; }
      }
    }
  }
  return InGroupEdges;
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
int GetCmnNbrs(const PGraph& Graph, const int& NId1, const int& NId2) {
  TIntV NbrV;
  return GetCmnNbrs(Graph, NId1, NId2, NbrV);
}

// Get common neighbors between a pair of nodes (undirected)
template<class PGraph>
int GetCmnNbrs(const PGraph& Graph, const int& NId1, const int& NId2, TIntV& NbrV) {
  if (! Graph->IsNode(NId1) || ! Graph->IsNode(NId2)) { NbrV.Clr(false); return 0; }
  typename PGraph::TObj::TNodeI NI1 = Graph->GetNI(NId1);
  typename PGraph::TObj::TNodeI NI2 = Graph->GetNI(NId2);
  NbrV.Clr(false);
  NbrV.Reserve(TMath::Mn(NI1.GetDeg(), NI2.GetDeg()));
  TIntSet NSet1(NI1.GetDeg()), NSet2(NI2.GetDeg());
  for (int i = 0; i < NI1.GetDeg(); i++) {
    const int nid = NI1.GetNbrNId(i);
    if (nid!=NId1 && nid!=NId2) {
      NSet1.AddKey(nid); }
  }
  for (int i = 0; i < NI2.GetDeg(); i++) {
    const int nid = NI2.GetNbrNId(i);
    if (NSet1.IsKey(nid)) {
      NSet2.AddKey(nid);
    }
  }
  NSet2.GetKeyV(NbrV);
  return NbrV.Len();
}

template<>
inline int GetCmnNbrs<PUNGraph>(const PUNGraph& Graph, const int& NId1, const int& NId2, TIntV& NbrV) {
  if (! Graph->IsNode(NId1) || ! Graph->IsNode(NId2)) { NbrV.Clr(false); return 0; }
  const TUNGraph::TNodeI NI1 = Graph->GetNI(NId1);
  const TUNGraph::TNodeI NI2 = Graph->GetNI(NId2);
  int i=0, j=0;
  NbrV.Clr(false);
  NbrV.Reserve(TMath::Mn(NI1.GetDeg(), NI2.GetDeg()));
  while (i < NI1.GetDeg() && j < NI2.GetDeg()) {
    const int nid = NI1.GetNbrNId(i);
    while (j < NI2.GetDeg() && NI2.GetNbrNId(j) < nid) { j++; }
    if (j < NI2.GetDeg() && nid==NI2.GetNbrNId(j) && nid!=NId1 && nid!=NId2) {
      IAssert(NbrV.Empty() || NbrV.Last() < nid);
      NbrV.Add(nid);
      j++;
    }
    i++;
  }
  return NbrV.Len();
}

// get number of length 2 directed paths between a pair of nodes
// for a pair of nodes (i,j): |{u: (i,u) and (u,j) }|
template<class PGraph>
int GetLen2Paths(const PGraph& Graph, const int& NId1, const int& NId2) {
  TIntV NbrV;
  return GetLen2Paths(Graph, NId1, NId2, NbrV);
}

// get number of length 2 directed paths between a pair of nodes
// for a pair of nodes (i,j): {u: (i,u) and (u,j) }
template<class PGraph>
int GetLen2Paths(const PGraph& Graph, const int& NId1, const int& NId2, TIntV& NbrV) {
  const typename PGraph::TNodeI NI = Graph->GetNI(NId1);
  NbrV.Clr(false);
  NbrV.Reserve(NI.GetOutDeg());
  for (int e = 0; e < NI.GetOutDeg(); e++) {
    const typename PGraph::TNodeI MidNI = GetNI(NI.GetOutNId(e));
    if (MidNI.IsOutNId(NId2)) {
      NbrV.Add(MidNI.GetId());
    }
  }
  return NbrV.Len();
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
      const int NId1 = NI.GetNbrNId(i);
      for (int j = 0; j < NI.GetDeg();  j++) {
        const int NId2 = NI.GetNbrNId(j);
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

