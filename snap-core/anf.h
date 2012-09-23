/////////////////////////////////////////////////
// Approximate Neighborhood Function.
namespace TSnap {
/// Approximate Neighborhood Function of a node: Returns the (approximate) number of nodes reachable from SrcNId in less than H hops.
/// @param SrcNId Starting node.
/// @param DistNbrsV Maps between the distance H (in hops) and the number of nodes reachable in <=H hops.
/// @param MxDist Maximum number of hops the algorithm spreads from SrcNId.
/// @param IsDir false: consider links as undirected (drop link directions).
/// @param NApprox Quality of approximation. See the ANF paper.
template <class PGraph> void GetAnf(const PGraph& Graph, const int& SrcNId, TIntFltKdV& DistNbrsV, const int& MxDist, const bool& IsDir, const int& NApprox=32); 
/// Approximate Neighborhood Function of a Graph: Returns the number of pairs of nodes reachable in less than H hops.
/// For example, DistNbrsV.GetDat(0) is the number of nodes in the graph, DistNbrsV.GetDat(1) is the number of nodes+edges and so on.
/// @param DistNbrsV Maps between the distance H (in hops) and the number of nodes reachable in <=H hops.
/// @param MxDist Maximum number of hops the algorithm spreads from SrcNId.
/// @param IsDir false: consider links as undirected (drop link directions).
/// @param NApprox Quality of approximation. See the ANF paper.
template <class PGraph> void GetAnf(const PGraph& Graph, TIntFltKdV& DistNbrsV, const int& MxDist, const bool& IsDir, const int& NApprox=32);
/// Returns a given Percentile of the shortest path length distribution of a Graph (based on a single run of ANF of approximation quality NApprox).
/// @param IsDir false: consider links as undirected (drop link directions).
template <class PGraph> double GetAnfEffDiam(const PGraph& Graph, const bool& IsDir, const double& Percentile, const int& NApprox);
/// Returns a 90-th percentile of the shortest path length distribution of a Graph (based on a NRuns runs of ANF of approximation quality NApprox).
/// @param IsDir false: consider links as undirected (drop link directions).
template <class PGraph> double GetAnfEffDiam(const PGraph& Graph, const int NRuns=1, int NApprox=-1);
} // namespace TSnap

/////////////////////////////////////////////////
/// Approximate Neighborhood Function.
/// Implements the algorithm for computing the diameter of a given graph.
/// The method is based on the approximate counting argument by Flajolet-Martin.
/// For more details see C. R. Palmer, P. B. Gibbons and C. Faloutsos, ANF: A Fast and Scalable Tool for Data Mining in Massive Graphs, KDD 2002 (http://www.cs.cmu.edu/~christos/PUBLICATIONS/kdd02-anf.ps.gz)
/// See TSnap::TestAnf() for example of how to use the class.
template <class PGraph>
class TGraphAnf {
private:
  typedef TVec<uint64> TAnfBitV;
  THash<TInt, uint64> NIdToBitPosH;  // NId to byte(!) offset in BitV
  TInt NApprox;                      // maintain N parallel approximations (multiple of 8)
  TInt NBits, MoreBits, ApproxBytes; // NBits=logNodes+MoreBits;  MoreBits: additional R bits;  ApproxBytes: Approx/8;
  PGraph Graph;
  TRnd Rnd;
private:
  UndefDefaultCopyAssign(TGraphAnf);
public:
  TGraphAnf(const PGraph& GraphPt, const int& Approx=32, const int& moreBits=5, const int& RndSeed=0) :
    NApprox(Approx), MoreBits(moreBits), Graph(GraphPt), Rnd(RndSeed) { IAssert(NApprox%8==0);  IAssert(sizeof(uint)==4); }
  uint64 GetNIdOffset(const int& NId) const { return NIdToBitPosH.GetDat(NId); }
  void InitAnfBits(TAnfBitV& BitV);
  void Union(TAnfBitV& BitV1, const uint64& NId1Offset, TAnfBitV& BitV2, const uint64& NId2Offset) const;
  double AvgLstZero(const TAnfBitV& BitV, const uint64& NIdOffset) const;
  double GetCount(const TAnfBitV& BitV, const uint64& NIdOffset) const {
    return pow(2.0, AvgLstZero(BitV, NIdOffset)) / 0.77351; }
  /// Returns the number of nodes reachable from SrcNId in less than H hops.
  /// @param SrcNId Starting node.
  /// @param DistNbrsV Maps between the distance H (in hops) and the number of nodes reachable in <=H hops.
  /// @param MxDist Maximum number of hops the algorithm spreads from SrcNId.
  /// @param IsDir false: consider links as undirected (drop link directions).
  void GetNodeAnf(const int& SrcNId, TIntFltKdV& DistNbrsV, const int& MxDist, const bool& IsDir);
  /// Returns the number of pairs of nodes reachable in less than H hops.
  /// For example, DistNbrsV.GetDat(0) is the number of nodes in the graph, DistNbrsV.GetDat(1) is the number of nodes+edges and so on.
  /// @param DistNbrsV Maps between the distance H (in hops) and the number of nodes reachable in <=H hops.
  /// @param MxDist Maximum number of hops the algorithm spreads from SrcNId.
  /// @param IsDir false: consider links as undirected (drop link directions).
  void GetGraphAnf(TIntFltKdV& DistNbrsV, const int& MxDist, const bool& IsDir);
};

template <class PGraph>
void TGraphAnf<PGraph>::InitAnfBits(TAnfBitV& BitV) {
  const int NNodes = Graph->GetNodes();
  const int LogNodes = int(ceil(TMath::Log2(NNodes)));
  ApproxBytes = NApprox / 8;
  NBits = LogNodes + MoreBits; // bits per node
  const int BytesPerNd = ApproxBytes * NBits; // total bytes per node
  BitV.Gen((NNodes * BytesPerNd)/sizeof(uint)+1);  IAssert(BitV.BegI() != NULL);
  BitV.PutAll(0);
  int SetBit = 0;
  uint64 NodeOff = 0;
  uchar* BitVPt = (uchar *) BitV.BegI();
  // for each node: 1st bits of all approximations are at BitV[Offset+0], 2nd bits at BitV[Offset+NApprox/32], ...
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI != Graph->EndNI(); NI++) {
    NIdToBitPosH.AddDat(NI.GetId(), NodeOff);
    // init vertex bits
    for (int approx = 0; approx < NApprox; approx++) {
      const int RndNum = Rnd.GetUniDevInt();
      for (SetBit = 0; (RndNum & (1<<SetBit))==0 && SetBit < NBits; SetBit++) { }
      if (SetBit >= NBits) SetBit = NBits-1;
      const int BitPos = ApproxBytes * SetBit + approx / 8;
      *(BitVPt + NodeOff + BitPos) |= (1<<(approx % 8)); // magically works better than code below (see anf.c)
    }
    NodeOff += BytesPerNd;
  }
}

template <class PGraph>
void TGraphAnf<PGraph>::Union(TAnfBitV& BitV1, const uint64& NId1Offset, TAnfBitV& BitV2, const uint64& NId2Offset) const {
  uchar* DstI = (uchar *) BitV1.BegI() + NId1Offset;
  uchar* SrcI = (uchar *) BitV2.BegI() + NId2Offset;
  for (int b=0; b < ApproxBytes*NBits; b++, DstI++, SrcI++) { *DstI |= *SrcI; }
}

// Average least zero bit position (least significant zero)
template <class PGraph>
double TGraphAnf<PGraph>::AvgLstZero(const TAnfBitV& BitV, const uint64& NIdOffset) const { //average least zero bit position (least significant zero)
  int approx, bit, AvgBitPos = 0;
  uchar* BitVPt = (uchar *) BitV.BegI();
  for (approx = 0; approx < NApprox; approx++) {
    for (bit = 0; bit < NBits; bit++) {
      if ((*(BitVPt+NIdOffset + ApproxBytes*bit + approx/8) & (1<<(approx%8))) == 0) break; } // found zero
    if (bit > NBits) bit = NBits;
    AvgBitPos += bit;
  }
  return AvgBitPos/double(NApprox) ;
}

template <class PGraph>
void TGraphAnf<PGraph>::GetNodeAnf(const int& SrcNId, TIntFltKdV& DistNbrsV, const int& MxDist, const bool& IsDir) {
  const int NNodes = Graph->GetNodes();
  TAnfBitV CurBitsV, LastBitsV;
  InitAnfBits(CurBitsV);          IAssert(CurBitsV.BegI() != NULL);
  LastBitsV.Gen(CurBitsV.Len());  IAssert(LastBitsV.BegI() != NULL);
  DistNbrsV.Clr();
  DistNbrsV.Add(TIntFltKd(1, Graph->GetNI(SrcNId).GetOutDeg()));
  for (int dist = 1; dist < (MxDist==-1 ? TInt::Mx : MxDist); dist++) {
    memcpy(LastBitsV.BegI(), CurBitsV.BegI(), sizeof(uint)*CurBitsV.Len()); //LastBitsV = CurBitsV;
    for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
      const uint64 NIdOffset = GetNIdOffset(NI.GetId());
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        const uint64 NId2Offset = GetNIdOffset(NI.GetOutNId(e));
        Union(CurBitsV, NIdOffset,  LastBitsV, NId2Offset);
      }
      if (! IsDir) {
        for (int e = 0; e < NI.GetInDeg(); e++) {
          const uint64 NId2Offset = GetNIdOffset(NI.GetInNId(e));
          Union(CurBitsV, NIdOffset,  LastBitsV, NId2Offset);
        }
      }
    }
    DistNbrsV.Add(TIntFltKd(dist, GetCount(CurBitsV, GetNIdOffset(SrcNId))));
    if (DistNbrsV.Len() > 1 && DistNbrsV.Last().Dat < 1.001*DistNbrsV[DistNbrsV.Len()-2].Dat) break; // 0.1%  change
  }
}

template <class PGraph>
void TGraphAnf<PGraph>::GetGraphAnf(TIntFltKdV& DistNbrsV, const int& MxDist, const bool& IsDir) {
  TAnfBitV CurBitsV, LastBitsV;
  InitAnfBits(CurBitsV);          IAssert(CurBitsV.BegI() != NULL);
  LastBitsV.Gen(CurBitsV.Len());  IAssert(LastBitsV.BegI() != NULL);
  int v, e;
  double NPairs = 0.0;
  DistNbrsV.Clr();
  DistNbrsV.Add(TIntFltKd(0, Graph->GetNodes()));
  DistNbrsV.Add(TIntFltKd(1, Graph->GetEdges()));
  //TExeTm ExeTm;
  for (int dist = 2; dist < (MxDist==-1 ? TInt::Mx : MxDist); dist++) {
    //printf("ANF dist %d...", dist);  ExeTm.Tick();
    memcpy(LastBitsV.BegI(), CurBitsV.BegI(), sizeof(uint)*CurBitsV.Len()); //LastBitsV = CurBitsV;
    for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
      const uint64 NIdOffset = GetNIdOffset(NI.GetId());
      for (e = 0; e < NI.GetOutDeg(); e++) {
        const uint64 NId2Offset = GetNIdOffset(NI.GetOutNId(e));
        Union(CurBitsV, NIdOffset,  LastBitsV, NId2Offset);
      }
      if (! IsDir) {
        for (e = 0; e < NI.GetInDeg(); e++) {
          const uint64 NId2Offset = GetNIdOffset(NI.GetInNId(e));
          Union(CurBitsV, NIdOffset,  LastBitsV, NId2Offset);
        }
      }
    }
    NPairs = 0.0;
    for (v = NIdToBitPosH.FFirstKeyId(); NIdToBitPosH.FNextKeyId(v); ) {
      NPairs += GetCount(CurBitsV, NIdToBitPosH[v]);
    }
    DistNbrsV.Add(TIntFltKd(dist, NPairs));
    //printf("pairs: %g  %s\n", NPairs, ExeTm.GetTmStr());
    if (NPairs == 0) { break; }
    if (DistNbrsV.Len() > 1 && NPairs < 1.001*DistNbrsV.LastLast().Dat) { break; } // 0.1%  change
    //TGnuPlot::SaveTs(DistNbrsV, "hops.tab", "HOPS, REACHABLE PAIRS");
  }
}
/////////////////////////////////////////////////
// Approximate Neighborhood Function
namespace TSnap {

namespace TSnapDetail {
/// Helper function for computing a given Percentile of a (unnormalized) cumulative distribution function.
double CalcEffDiam(const TIntFltKdV& DistNbrsCdfV, const double& Percentile=0.9);
/// Helper function for computing a given Percentile of a (unnormalized) cumulative distribution function.
double CalcEffDiam(const TFltPrV& DistNbrsCdfV, const double& Percentile=0.9);
/// Helper function for computing a given Percentile of a (unnormalized) probability distribution function.
double CalcEffDiamPdf(const TIntFltKdV& DistNbrsPdfV, const double& Percentile=0.9);
/// Helper function for computing a given Percentile of a (unnormalized) probability distribution function.
double CalcEffDiamPdf(const TFltPrV& DistNbrsPdfV, const double& Percentile=0.9);
/// Helper function for computing the mean of a (unnormalized) probability distribution function.
double CalcAvgDiamPdf(const TIntFltKdV& DistNbrsPdfV);
/// Helper function for computing the mean of a (unnormalized) probability distribution function.
double CalcAvgDiamPdf(const TFltPrV& DistNbrsPdfV);
} // TSnapDetail

template <class PGraph>
void GetAnf(const PGraph& Graph, const int& SrcNId, TIntFltKdV& DistNbrsV, const int& MxDist, const bool& IsDir, const int& NApprox) {
  TGraphAnf<PGraph> Anf(Graph, NApprox, 5, 0);
  Anf.GetNodeAnf(SrcNId, DistNbrsV, MxDist, IsDir);
}

template <class PGraph>
void GetAnf(const PGraph& Graph, TIntFltKdV& DistNbrsV, const int& MxDist, const bool& IsDir, const int& NApprox) {
  TGraphAnf<PGraph> Anf(Graph, NApprox, 5, 0);
  Anf.GetGraphAnf(DistNbrsV, MxDist, IsDir);
}

template <class PGraph>
double GetAnfEffDiam(const PGraph& Graph, const bool& IsDir, const double& Percentile, const int& NApprox) {
  TIntFltKdV DistNbrsV;
  TGraphAnf<PGraph> Anf(Graph, NApprox, 5, 0);
  Anf.GetGraphAnf(DistNbrsV, -1, IsDir);
  return TSnap::TSnapDetail::CalcEffDiam(DistNbrsV, Percentile);
}

template<class PGraph>
double GetAnfEffDiam(const PGraph& Graph, const int NRuns, int NApprox) {
  //return TSnap::GetEffDiam(Graph, IsDir, 0.9, 32);
  TMom Mom;
  if (NApprox == -1) {
    if (Graph->GetNodes() < 100000) { NApprox = 64; }
    else if (Graph->GetNodes() < 1000000) { NApprox = 32; }
    else { NApprox = 16; }
  }
  const bool IsDir = false;
  for (int r = 0; r < NRuns; r++) {
    Mom.Add(TSnap::GetAnfEffDiam(Graph, IsDir, 0.9, NApprox));
  }
  Mom.Def();
  return Mom.GetMean();
}

template <class PGraph> void TestAnf() {
  PGraph Graph = PGraph::TObj::New();
  //Graph:
  //  0    2 ----> 3
  //       ^       |
  //       |       |
  //       |       ^
  //  1    5 <---- 4
  for (int v = 0; v < 6; v++) { Graph->AddNode(v); }
  Graph->AddEdge(2, 3);
  Graph->AddEdge(3, 4);
  Graph->AddEdge(4, 5);
  Graph->AddEdge(5, 2);
  TFltV AnfV;
  for (int t = 0; t < 10; t++) {
    TGraphAnf<PGraph> Anf(Graph, 128, 5, t+1);
    TIntFltKdV DistToNbrsV;
    Anf.GetGraphAnf(DistToNbrsV, 5, true);
    printf("\n--seed: %d---------------------\n", t+1);
    for (int i = 0; i < DistToNbrsV.Len(); i++) {
      printf("dist: %d\t hops:%f\n", DistToNbrsV[i].Key(), DistToNbrsV[i].Dat());
    }
    AnfV.Add(DistToNbrsV.Last().Dat);
  }
  TMom Mom(AnfV);
  printf("-----------\nAvgAnf: %f  StDev:  %f\n", Mom.GetMean(), Mom.GetSDev());//*/
  // const int NApprox = 32;
  /*printf("\nANF vs. SAMPLE diam test (10 runs of ANF, NApprox=%d):\n", NApprox);
  //Graph = TGGen<PGraph>::GenGrid(20, 20);
  Graph = TGAlg::GetMxWcc(TGGen<PGraph>::GenRnd(1000, 10000));
  TFltV FullAnf, EffAnf;
  for (int tryn = 0; tryn < 10; tryn++) {
    FullAnf.Add(GetEffDiam(Graph, false, 1.0, NApprox));
    EffAnf.Add(GetEffDiam(Graph, false, 0.9, NApprox));
  }
  TMom FullMom(FullAnf);
  TMom AnfMom(EffAnf);
  printf("  Sample FullDiam:      %d\n", TGAlg::GetBfsFullDiam(Graph, 100, false));
  printf("  Anf    FullDiam:      %f  [%f]\n", FullMom.GetMean(), FullMom.GetSDev());
  printf("  Sample EffDiam [90%%]: %f\n", TGAlg::GetBfsEffDiam(Graph, 100, false));
  printf("  Anf    EffDiam [90%%]: %f  [%f]\n", AnfMom.GetMean(), AnfMom.GetSDev());
  // epinions
  printf("\nEpinions graph:\n");
  { typedef PNGraph PGraph;
  PGraph G = TGGen<PGraph>::GenEpinions();
  TIntFltKdV DistToPairsV;
  GetAnf(G, DistToPairsV, 50, true);
  for(int i = 0; i < DistToPairsV.Len(); i++) {
    printf("\t%d\t%f\n", DistToPairsV[i].Key, DistToPairsV[i].Dat); }
  printf("\nUndir\n");
  TAnf<PGraph>::GetAnf(G, DistToPairsV, 50, false);
  for(int j = 0; j < DistToPairsV.Len(); j++) {
    printf("\t%d\t%f\n", DistToPairsV[j].Key, DistToPairsV[j].Dat); }
  }//*/
}

} // namespace TSnap
