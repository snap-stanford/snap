/////////////////////////////////////////////////
// Graph Generators
namespace TSnap {

// Constant degree random graph
PUNGraph GenRndDegK(const int& Nodes, const int& NodeDeg, const int& NSwitch, TRnd& Rnd) {
  // create degree sequence
  TIntV DegV(Nodes, 0);
  int DegSum=0;
  for (int i = 0; i < Nodes; i++) {
    DegV.Add(NodeDeg);
    DegSum += NodeDeg;
  }
  IAssert(DegSum % 2 == 0);
  PUNGraph G = GenDegSeq(DegV, Rnd); // get some graph that obeys the degree sequnce
  return GenRewire(G, NSwitch, Rnd);  // make it random
}

PUNGraph GenRndPowerLaw(const int& Nodes, const double& PowerExp, const bool& ConfModel, TRnd& Rnd) {
  TIntV DegSeqV;
  uint DegSum=0;
  for (int n = 0; n < Nodes; n++) {
    const int Val = (int) TMath::Round(Rnd.GetPowerDev(PowerExp));
    if (! (Val >= 1 && Val < Nodes/2)) { n--; continue; } // skip nodes with too large degree
    DegSeqV.Add(Val);
    DegSum += Val;
  }
  printf("%d nodes, %u edges\n", Nodes, DegSum);
  if (DegSum % 2 == 1) { DegSeqV[0] += 1; }
  if (ConfModel) {
    // use configuration model -- fast but does not exactly obey the degree sequence
    return GenConfModel(DegSeqV, Rnd);
  } else {
    PUNGraph G = TSnap::GenDegSeq(DegSeqV, Rnd);
    return TSnap::GenRewire(G, 10, Rnd);
  }
}

// A graph with exact degree sequence (edges could be a bit biased). No self loops.
// The idea is to simulate Configuration Model but if a duplicate edge occurs,
// find a random edge, break it and reconnect
PUNGraph GenDegSeq(const TIntV& DegSeqV, TRnd& Rnd) {
  const int Nodes = DegSeqV.Len();
  PUNGraph GraphPt = TUNGraph::New();
  TUNGraph& Graph = *GraphPt;
  Graph.Reserve(Nodes, -1);
  TIntH DegH(DegSeqV.Len(), true);
  int DegSum=0, edge=0;
  for (int node = 0; node < Nodes; node++) {
    IAssert(Graph.AddNode(node) == node);
    DegH.AddDat(node, DegSeqV[node]);
    DegSum += DegSeqV[node];
  }
  IAssert(DegSum % 2 == 0);
  while (! DegH.Empty()) {
    // pick random nodes and connect
    const int NId1 = DegH.GetKey(DegH.GetRndKeyId(TInt::Rnd, 0.5));
    const int NId2 = DegH.GetKey(DegH.GetRndKeyId(TInt::Rnd, 0.5));
    IAssert(DegH.IsKey(NId1) && DegH.IsKey(NId2));
    if (NId1 == NId2) {
      if (DegH.GetDat(NId1) == 1) { continue; }
      // find rnd edge, break it, and connect the endpoints to the nodes
      const TIntPr Edge = GetRndEdgeNonAdjNode(GraphPt, NId1, -1);
      if (Edge.Val1==-1) { continue; }
      Graph.DelEdge(Edge.Val1, Edge.Val2);
      Graph.AddEdge(Edge.Val1, NId1);
      Graph.AddEdge(NId1, Edge.Val2);
      if (DegH.GetDat(NId1) == 2) { DegH.DelKey(NId1); }
      else { DegH.GetDat(NId1) -= 2; }
    } else {
      if (! Graph.IsEdge(NId1, NId2)) {
        Graph.AddEdge(NId1, NId2); }  // good edge
      else {
        // find rnd edge, break and cross-connect
        const TIntPr Edge = GetRndEdgeNonAdjNode(GraphPt, NId1, NId2);
        if (Edge.Val1==-1) {continue; }
        Graph.DelEdge(Edge.Val1, Edge.Val2);
        Graph.AddEdge(NId1, Edge.Val1);
        Graph.AddEdge(NId2, Edge.Val2);
      }
      if (DegH.GetDat(NId1)==1) { DegH.DelKey(NId1); }
      else { DegH.GetDat(NId1) -= 1; }
      if (DegH.GetDat(NId2)==1) { DegH.DelKey(NId2); }
      else { DegH.GetDat(NId2) -= 1; }
    }
    if (++edge % 1000 == 0) {
      printf("\r %dk / %dk", edge/1000, DegSum/2000); }
  }
  return GraphPt;
}


// Generate a random graph with a given degree sequence
// Configuration model: pick two spokes at random, link or discard, continue until done
// We ignore (discard) self loops and multiple edges
PUNGraph GenConfModel(const TIntV& DegSeqV, TRnd& Rnd) {
  const int Nodes = DegSeqV.Len();
  PUNGraph GraphPt = TUNGraph::New();
  TUNGraph& Graph = *GraphPt;
  Graph.Reserve(Nodes, -1);
  TIntV NIdDegV(DegSeqV.Len(), 0);
  int DegSum=0, edges=0;
  for (int node = 0; node < Nodes; node++) {
    Graph.AddNode(node);
    for (int d = 0; d < DegSeqV[node]; d++) { NIdDegV.Add(node); }
    DegSum += DegSeqV[node];
  }
  NIdDegV.Shuffle(Rnd);
  TIntPrSet EdgeH(DegSum/2); // set of all edges, is faster than graph edge lookup
  if (DegSum % 2 != 0) {
    printf("Seg seq is odd [%d]: ", DegSeqV.Len());
    for (int d = 0; d < TMath::Mn(100, DegSeqV.Len()); d++) { printf("  %d", (int)DegSeqV[d]); }
    printf("\n");
  }
  int u=0, v=0;
  for (int c = 0; NIdDegV.Len() > 1; c++) {
    u = Rnd.GetUniDevInt(NIdDegV.Len());
    while ((v = Rnd.GetUniDevInt(NIdDegV.Len())) == u) { }
    if (u > v) { Swap(u, v); }
    const int E1 = NIdDegV[u];
    const int E2 = NIdDegV[v];
    if (v == NIdDegV.Len()-1) { NIdDegV.DelLast(); } 
    else { NIdDegV[v] = NIdDegV.Last();  NIdDegV.DelLast(); }
    if (u == NIdDegV.Len()-1) { NIdDegV.DelLast(); } 
    else { NIdDegV[u] = NIdDegV.Last();  NIdDegV.DelLast(); }
    if (E1 == E2 || EdgeH.IsKey(TIntPr(E1, E2))) { continue; }
    EdgeH.AddKey(TIntPr(E1, E2));
    Graph.AddEdge(E1, E2);
    edges++;
    if (c % (DegSum/100+1) == 0) { printf("\r configuration model: iter %d: edges: %d, left: %d", c, edges, NIdDegV.Len()/2); }
  }
  printf("\n");
  return GraphPt;
}

// Rewire the network (while preserving node degrees): edge switching model
// See:  On the uniform generation of random graphs with prescribed degree sequences
// by R. Milo, N. Kashtan, S. Itzkovitz, M. E. J. Newman, U. Alon
// http://arxiv.org/abs/cond-mat/0312028
PUNGraph GenRewire(const PUNGraph& OrigGraph, const int& NSwitch, TRnd& Rnd) {
  const int Nodes = OrigGraph->GetNodes();
  const int Edges = OrigGraph->GetEdges();
  PUNGraph GraphPt = TUNGraph::New();
  TUNGraph& Graph = *GraphPt;
  Graph.Reserve(Nodes, -1);
  TExeTm ExeTm;
  // generate a graph that satisfies the constraints
  printf("Randomizing edges (%d, %d)...\n", Nodes, Edges);
  TIntPrSet EdgeSet(Edges);
  for (TUNGraph::TNodeI NI = OrigGraph->BegNI(); NI < OrigGraph->EndNI(); NI++) {
    const int NId = NI.GetId();
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      if (NId <= NI.GetOutNId(e)) { continue; }
      EdgeSet.AddKey(TIntPr(NId, NI.GetOutNId(e)));
    }
    Graph.AddNode(NI.GetId());
  }
  // edge switching
  uint skip=0;
  for (uint swps = 0; swps < 2*uint(Edges)*uint(NSwitch); swps++) {
    const int keyId1 = EdgeSet.GetRndKeyId(Rnd);
    const int keyId2 = EdgeSet.GetRndKeyId(Rnd);
    if (keyId1 == keyId2) { skip++; continue; }
    const TIntPr& E1 = EdgeSet[keyId1];
    const TIntPr& E2 = EdgeSet[keyId2];
    TIntPr NewE1(E1.Val1, E2.Val1), NewE2(E1.Val2, E2.Val2);
    if (NewE1.Val1 > NewE1.Val2) { Swap(NewE1.Val1, NewE1.Val2); }
    if (NewE2.Val1 > NewE2.Val2) { Swap(NewE2.Val1, NewE2.Val2); }
    if (NewE1!=NewE2 && NewE1.Val1!=NewE1.Val2 && NewE2.Val1!=NewE2.Val2 && ! EdgeSet.IsKey(NewE1) && ! EdgeSet.IsKey(NewE2)) {
      EdgeSet.DelKeyId(keyId1);  EdgeSet.DelKeyId(keyId2);
      EdgeSet.AddKey(TIntPr(NewE1));
      EdgeSet.AddKey(TIntPr(NewE2));
    } else { skip++; }
    if (swps % Edges == 0) {
      printf("\r  %uk/%uk: %uk skip [%s]", swps/1000u, 2*uint(Edges)*uint(NSwitch)/1000u, skip/1000u, ExeTm.GetStr());
      // time limit 2 hours
      if (ExeTm.GetSecs() > 2*3600) { printf(" *** Time limit!\n"); break; }
    }
  }
  printf("\r  total %uk switchings attempted, %uk skiped  [%s]\n", 2*uint(Edges)*uint(NSwitch)/1000u, skip/1000u, ExeTm.GetStr());
  for (int e = 0; e < EdgeSet.Len(); e++) {
    Graph.AddEdge(EdgeSet[e].Val1, EdgeSet[e].Val2);
  }
  return GraphPt;
}

PNGraph GenRewire(const PNGraph& OrigGraph, const int& NSwitch, TRnd& Rnd) {
  const int Nodes = OrigGraph->GetNodes();
  const int Edges = OrigGraph->GetEdges();
  PNGraph GraphPt = TNGraph::New();
  TNGraph& Graph = *GraphPt;
  Graph.Reserve(Nodes, -1);
  TExeTm ExeTm;
  // generate a graph that satisfies the constraints
  printf("Randomizing edges (%d, %d)...\n", Nodes, Edges);
  TIntPrSet EdgeSet(Edges);
  for (TNGraph::TNodeI NI = OrigGraph->BegNI(); NI < OrigGraph->EndNI(); NI++) {
    const int NId = NI.GetId();
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      EdgeSet.AddKey(TIntPr(NId, NI.GetOutNId(e))); }
    Graph.AddNode(NI);
  }
  // edge switching
  uint skip=0;
  for (uint swps = 0; swps < 2*uint(Edges)*uint(NSwitch); swps++) {
    const int keyId1 = EdgeSet.GetRndKeyId(Rnd);
    const int keyId2 = EdgeSet.GetRndKeyId(Rnd);
    if (keyId1 == keyId2) { skip++; continue; }
    const TIntPr& E1 = EdgeSet[keyId1];
    const TIntPr& E2 = EdgeSet[keyId2];
    TIntPr NewE1(E1.Val1, E2.Val1), NewE2(E1.Val2, E2.Val2);
    if (NewE1.Val1!=NewE2.Val1 && NewE1.Val2!=NewE2.Val1 && NewE1.Val2!=NewE2.Val1 && NewE1.Val2!=NewE2.Val2 && ! EdgeSet.IsKey(NewE1) && ! EdgeSet.IsKey(NewE2)) {
      EdgeSet.DelKeyId(keyId1);  EdgeSet.DelKeyId(keyId2);
      EdgeSet.AddKey(TIntPr(NewE1));
      EdgeSet.AddKey(TIntPr(NewE2));
    } else { skip++; }
    if (swps % Edges == 0) {
      printf("\r  %uk/%uk: %uk skip [%s]", swps/1000u, 2*uint(Edges)*uint(NSwitch)/1000u, skip/1000u, ExeTm.GetStr());
      // time limit 2 hours
      if (ExeTm.GetSecs() > 2*3600) { printf(" *** Time limit!\n"); break; }
    }
  }
  printf("\r  total %uk switchings attempted, %uk skiped  [%s]\n", 2*uint(Edges)*uint(NSwitch)/1000u, skip/1000u, ExeTm.GetStr());
  for (int e = 0; e < EdgeSet.Len(); e++) {
    Graph.AddEdge(EdgeSet[e].Val1, EdgeSet[e].Val2);
  }
  return GraphPt;
}

// Barabasi-Albert model
//   See: Emergence of scaling in random networks
//   http://arxiv.org/abs/cond-mat/9910332
PUNGraph GenPrefAttach(const int& Nodes, const int& NodeOutDeg, TRnd& Rnd) {
  PUNGraph GraphPt = PUNGraph::New();
  TUNGraph& Graph = *GraphPt;
  Graph.Reserve(Nodes, NodeOutDeg*Nodes);
  TIntV NIdV(NodeOutDeg*Nodes, 0);
  // first edge
  Graph.AddNode(0);  Graph.AddNode(1);
  NIdV.Add(0);  NIdV.Add(1);
  Graph.AddEdge(0, 1);
  TIntSet NodeSet;
  for (int node = 2; node < Nodes; node++) {
    NodeSet.Clr(false);
    while (NodeSet.Len() < NodeOutDeg && NodeSet.Len() < node) {
      NodeSet.AddKey(NIdV[TInt::Rnd.GetUniDevInt(NIdV.Len())]);
    }
    const int N = Graph.AddNode();
    for (int i = 0; i < NodeSet.Len(); i++) {
      Graph.AddEdge(N, NodeSet[i]);
      NIdV.Add(N);
      NIdV.Add(NodeSet[i]);
    }
  }
  return GraphPt;
}

namespace TSnapDetail {
//   sample random point from d-dimensional unit sphere
void GetSphereDev(const int& Dim, TRnd& Rnd, TFltV& ValV) {
  if (ValV.Len() != Dim) { ValV.Gen(Dim); }
  double Length = 0.0;
  for (int i = 0; i < Dim; i++) {
    ValV[i] = Rnd.GetNrmDev();
    Length += TMath::Sqr(ValV[i]); }
  Length = 1.0 / sqrt(Length);
  for (int i = 0; i < Dim; i++) {
    ValV[i] *= Length;
  }
}
} // namespace TSnapDetail

// Geometric Preferential Attachment model by Flexman, Frieze and Vera, WAW 2004
PUNGraph GenGeoPrefAttach(const int& Nodes, const int& OutDeg, const double& Beta, TRnd& Rnd) {
  PUNGraph G = TUNGraph::New(Nodes, Nodes*OutDeg);
  TFltTrV PointV(Nodes, 0);
  TFltV ValV;
  // points on a sphere of radius 1/(2*pi)
  const double Rad = 0.5 * TMath::Pi;
  for (int i = 0; i < Nodes; i++) {
    TSnapDetail::GetSphereDev(3, Rnd, ValV);
    PointV.Add(TFltTr(Rad*ValV[0], Rad*ValV[1], Rad*ValV[2]));
  }
  const double R2 = TMath::Sqr(log((double) Nodes) / (pow((double) Nodes, 0.5-Beta)));
  TIntV DegV, NIdV;
  int SumDeg;
  for (int t = 0; t < Nodes; t++) {
    const int pid = t;
    const TFltTr& P1 = PointV[pid];
    // add node
    if (! G->IsNode(pid)) { G->AddNode(pid); }
    // find neighborhood
    DegV.Clr(false);  NIdV.Clr(false);  SumDeg=0;
    for (int p = 0; p < t; p++) {
      const TFltTr& P2 = PointV[p];
      if (TMath::Sqr(P1.Val1-P2.Val1)+TMath::Sqr(P1.Val2-P2.Val2)+TMath::Sqr(P1.Val3-P2.Val3) < R2) {
        NIdV.Add(p);
        DegV.Add(G->GetNI(p).GetDeg()+1);
        SumDeg += DegV.Last();
      }
    }
    // add edges
    for (int m = 0; m < OutDeg; m++) {
      const int rnd = Rnd.GetUniDevInt(SumDeg);
      int sum = 0, dst = -1;
      for (int s = 0; s < DegV.Len(); s++) {
        sum += DegV[s];
        if (rnd < sum) { dst=s;  break; }
      }
      if (dst != -1) {
        G->AddEdge(pid, NIdV[dst]);
        SumDeg -= DegV[dst];
        NIdV.Del(dst);  DegV.Del(dst);
      }
    }
  }
  return G;
}

// Small-World mode by Watts and Strogatz: Collective dynamics of 'small-world' networks.
// We assume a circle where each node out-links to NodeOutDeg other nodes (so, a node has 2*NodeOutDeg edges)
PUNGraph GenSmallWorld(const int& Nodes, const int& NodeOutDeg, const double& RewireProb, TRnd& Rnd) {
  THashSet<TIntPr> EdgeSet(Nodes*NodeOutDeg);
  for (int node = 0; node < Nodes; node++) {
    const int src = node;
    for (int edge = 1; edge <= NodeOutDeg; edge++) {
      int dst = (node+edge) % Nodes;      // edge to next neighbor
      if (Rnd.GetUniDev() < RewireProb) { // random edge
        dst = Rnd.GetUniDevInt(Nodes);
        while (dst == src || EdgeSet.IsKey(TIntPr(src, dst))) {
          dst = Rnd.GetUniDevInt(Nodes); }
      }
      EdgeSet.AddKey(TIntPr(src, dst));
    }
  }
  PUNGraph GraphPt = TUNGraph::New();
  TUNGraph& Graph = *GraphPt;
  Graph.Reserve(Nodes, EdgeSet.Len());
  int node;
  for (node = 0; node < Nodes; node++) {
    IAssert(Graph.AddNode(node) == node);
  }
  for (int edge = 0; edge < EdgeSet.Len(); edge++) {
    Graph.AddEdge(EdgeSet[edge].Val1, EdgeSet[edge].Val2);
  }
  Graph.Defrag();
  return GraphPt;
}

PNGraph GenForestFire(const int& Nodes, const double& FwdProb, const double& BckProb) {
  return TForestFire::GenGraph(Nodes, FwdProb, BckProb);
}

// Copying Model as defined in Kleinberg, Kumar, Raghavan, Rajagopalan, Tomkins:
//   The Web as a Graph: Measurements, Models, and Methods. COCOON 1999.
// Node u comes, selects a random v, and with prob Beta it links to v,
// with 1-Beta links u links to neighbor of v. Power-law degree slope is -1/(1-Beta)
PNGraph GenCopyModel(const int& Nodes, const double& Beta, TRnd& Rnd) {
  PNGraph GraphPt = TNGraph::New();
  TNGraph& Graph = *GraphPt;
  Graph.Reserve(Nodes, Nodes);
  const int startNId = Graph.AddNode();
  Graph.AddEdge(startNId, startNId);
  for (int n = 1; n < Nodes; n++) {
    const int rnd = Graph.GetRndNId();
    const int NId = Graph.AddNode();
    if (Rnd.GetUniDev() < Beta) {
      Graph.AddEdge(NId, rnd); }
    else {
      const TNGraph::TNodeI NI = Graph.GetNI(rnd);
      const int rnd2 = Rnd.GetUniDevInt(NI.GetOutDeg());
      Graph.AddEdge(NId, NI.GetOutNId(rnd2));
    }
  }
  return GraphPt;
}

// R-MAT generator model: A Recursive Model for Graph Mining
// by D. Chakrabarti, Y. Zhan and C. Faloutsos, in SIAM Data Mining 2004
// http://www.cs.cmu.edu/~deepay/mywww/papers/siam04.pdf
PNGraph GenRMat(const int& Nodes, const int& Edges, const double& A, const double& B, const double& C, TRnd& Rnd) {
  PNGraph GraphPt = TNGraph::New();
  TNGraph& Graph = *GraphPt;
  Graph.Reserve(Nodes, Edges);
  IAssert(A+B+C < 1.0);
  int rngX, rngY, offX, offY;
  int Depth=0, Collisions=0, Cnt=0, PctDone=0;
  const int EdgeGap = Edges / 100 + 1;
  // sum of parameters (probabilities)
  TVec<double> sumA(128, 0), sumAB(128, 0), sumAC(128, 0), sumABC(128, 0);  // up to 2^128 vertices ~ 3.4e38
  for (int i = 0; i < 128; i++) {
    const double a = A * (Rnd.GetUniDev() + 0.5);
    const double b = B * (Rnd.GetUniDev() + 0.5);
    const double c = C * (Rnd.GetUniDev() + 0.5);
    const double d = (1.0 - (A+B+C)) * (Rnd.GetUniDev() + 0.5);
    const double abcd = a+b+c+d;
    sumA.Add(a / abcd);
    sumAB.Add((a+b) / abcd);
    sumAC.Add((a+c) / abcd);
    sumABC.Add((a+b+c) / abcd);
  }
  // nodes
  for (int node = 0; node < Nodes; node++) {
    IAssert(Graph.AddNode(-1) == node);
  }
  // edges
  for (int edge = 0; edge < Edges; ) {
    rngX = Nodes;  rngY = Nodes;  offX = 0;  offY = 0;
    Depth = 0;
    // recurse the matrix
    while (rngX > 1 || rngY > 1) {
      const double RndProb = Rnd.GetUniDev();
      if (rngX>1 && rngY>1) {
        if (RndProb < sumA[Depth]) { rngX/=2; rngY/=2; }
        else if (RndProb < sumAB[Depth]) { offX+=rngX/2;  rngX-=rngX/2;  rngY/=2; }
        else if (RndProb < sumABC[Depth]) { offY+=rngY/2;  rngX/=2;  rngY-=rngY/2; }
        else { offX+=rngX/2;  offY+=rngY/2;  rngX-=rngX/2;  rngY-=rngY/2; }
      } else
      if (rngX>1) { // row vector
        if (RndProb < sumAC[Depth]) { rngX/=2; rngY/=2; }
        else { offX+=rngX/2;  rngX-=rngX/2;  rngY/=2; }
      } else
      if (rngY>1) { // column vector
        if (RndProb < sumAB[Depth]) { rngX/=2; rngY/=2; }
        else { offY+=rngY/2;  rngX/=2;  rngY-=rngY/2; }
      } else { Fail; }
      Depth++;
    }
    // add edge
    const int NId1 = offX;
    const int NId2 = offY;
    if (NId1 != NId2 && ! Graph.IsEdge(NId1, NId2)) {
      Graph.AddEdge(NId1, NId2);
      if (++Cnt > EdgeGap) {
        Cnt=0;  printf("\r  %d%% edges", ++PctDone); }
      edge++;
    } else {
      Collisions++; }
  }
  printf("\r  RMat: nodes:%d, edges:%d, Iterations:%d, Collisions:%d (%.1f%%).\n", Nodes, Edges,
    Edges+Collisions, Collisions, 100*Collisions/double(Edges+Collisions));
  Graph.Defrag();
  return GraphPt;
}

PNGraph GenRMatEpinions() {
  return GenRMat(75888, 508837, 0.550, 0.228, 0.212);
}

} // namespace TSnap
