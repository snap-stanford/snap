/////////////////////////////////////////////////
// Approximate Neighborhood Function
// C. R. Palmer, P. B. Gibbons and C. Faloutsos,
// ANF: A Fast and Scalable Tool for Data Mining in Massive Graphs,
// KDD 2002 http://www.cs.cmu.edu/~christos/PUBLICATIONS/kdd02-anf.ps.gz

template <class PGraph>
class TGraphAnf {
private:
  typedef TVec<uint64> TAnfBitV;
  THash<TInt, uint64> NIdToBitPosH; // NId to byte(!) offset in BitV
  TInt NApprox; // maintain N parallel approximations (multiple of 8)
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
    return pow(2.0, AvgLstZero(BitV, NIdOffset)) / 0.77351;
  }
  void GetNodeAnf(const int& SrcNId, TIntFltKdV& DistNbhsV, const int& MxDist, const bool& IsDir);
  void GetGraphAnf(TIntFltKdV& DistNbhsV, const int& MxDist, const bool& IsDir);
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

//average least zero bit position (least significant zero)
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
void TGraphAnf<PGraph>::GetNodeAnf(const int& SrcNId, TIntFltKdV& DistNbhsV, const int& MxDist, const bool& IsDir) {
  const int NNodes = Graph->GetNodes();
  TAnfBitV CurBitsV, LastBitsV;
  InitAnfBits(CurBitsV);          IAssert(CurBitsV.BegI() != NULL);
  LastBitsV.Gen(CurBitsV.Len());  IAssert(LastBitsV.BegI() != NULL);
  DistNbhsV.Clr();
  DistNbhsV.Add(TIntFltKd(1, Graph->GetNI(SrcNId).GetOutDeg()));
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
    DistNbhsV.Add(TIntFltKd(dist, GetCount(CurBitsV, GetNIdOffset(SrcNId))));
    if (DistNbhsV.Len() > 1 && DistNbhsV.Last().Dat < 1.001*DistNbhsV[DistNbhsV.Len()-2].Dat) break; // 0.1%  change
  }
}

template <class PGraph>
void TGraphAnf<PGraph>::GetGraphAnf(TIntFltKdV& DistNbhsV, const int& MxDist, const bool& IsDir) {
  TAnfBitV CurBitsV, LastBitsV;
  InitAnfBits(CurBitsV);          IAssert(CurBitsV.BegI() != NULL);
  LastBitsV.Gen(CurBitsV.Len());  IAssert(LastBitsV.BegI() != NULL);
  int v, e;
  double NPairs = 0.0;
  DistNbhsV.Clr();
  DistNbhsV.Add(TIntFltKd(0, Graph->GetNodes()));
  DistNbhsV.Add(TIntFltKd(1, Graph->GetEdges()));
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
    DistNbhsV.Add(TIntFltKd(dist, NPairs));
    //printf("pairs: %g  %s\n", NPairs, ExeTm.GetTmStr());
    if (NPairs == 0) { break; }
    if (DistNbhsV.Len() > 1 && NPairs < 1.001*DistNbhsV[DistNbhsV.Len()-2].Dat) { break; } // 0.1%  change
    //TGnuPlot::SaveTs(DistNbhsV, "hops.tab", "HOPS, REACHABLE PAIRS");
  }
}
/////////////////////////////////////////////////
// Approximate Neighborhood Function
namespace TAnf {
double CalcEffDiam(const TIntFltKdV& DistNbhsCdfV, const double& Percentile=0.9);
double CalcEffDiam(const TFltPrV& DistNbhsCdfV, const double& Percentile=0.9);
double CalcEffDiamPdf(const TIntFltKdV& DistNbhsPdfV, const double& Percentile=0.9);
double CalcEffDiamPdf(const TFltPrV& DistNbhsPdfV, const double& Percentile=0.9);
double CalcAvgDiamPdf(const TIntFltKdV& DistNbhsPdfV);
double CalcAvgDiamPdf(const TFltPrV& DistNbhsPdfV);

template <class PGraph> void Test() {
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
    TIntFltKdV DistToNbhsV;
    Anf.GetGraphAnf(DistToNbhsV, 5, true);
    printf("\n--seed: %d---------------------\n", t+1);
    for (int i = 0; i < DistToNbhsV.Len(); i++) {
      printf("dist: %d\t hops:%f\n", DistToNbhsV[i].Key(), DistToNbhsV[i].Dat());
    }
    AnfV.Add(DistToNbhsV.Last().Dat);
  }
  TMom Mom(AnfV);
  printf("-----------\nAvgAnf: %f  StDev:  %f\n", Mom.GetMean(), Mom.GetSDev());//*/
  const int NApprox = 32;
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

}; // namespace TAnf

/////////////////////////////////////////////////
// Snap functions
namespace TSnap {

template <class PGraph> void GetAnf(const PGraph& Graph, const int& SrcNId, TIntFltKdV& DistNbhsV, const int& MxDist, const bool& IsDir, const int& NApprox=32); // number of reachable nodes at distance Dist
template <class PGraph> void GetAnf(const PGraph& Graph, TIntFltKdV& DistNbhsV, const int& MxDist, const bool& IsDir, const int& NApprox=32); // number of pairs of reachable nodes at distance Dist
template <class PGraph> double GetAnfEffDiam(const PGraph& Graph, const bool& IsDir, const double& Percentile, const int& NApprox);
template <class PGraph> double GetAnfEffDiam(const PGraph& Graph, const int NRuns=1, int NApprox=-1);

template <class PGraph>
void GetAnf(const PGraph& Graph, const int& SrcNId, TIntFltKdV& DistNbhsV, const int& MxDist, const bool& IsDir, const int& NApprox) {
  TGraphAnf<PGraph> Anf(Graph, NApprox, 5, 0);
  Anf.GetNodeAnf(SrcNId, DistNbhsV, MxDist, IsDir);
}

template <class PGraph>
void GetAnf(const PGraph& Graph, TIntFltKdV& DistNbhsV, const int& MxDist, const bool& IsDir, const int& NApprox) {
  TGraphAnf<PGraph> Anf(Graph, NApprox, 5, 0);
  Anf.GetGraphAnf(DistNbhsV, MxDist, IsDir);
}

template <class PGraph>
double GetAnfEffDiam(const PGraph& Graph, const bool& IsDir, const double& Percentile, const int& NApprox) {
  TIntFltKdV DistNbhsV;
  TGraphAnf<PGraph> Anf(Graph, NApprox, 5, 0);
  Anf.GetGraphAnf(DistNbhsV, -1, IsDir);
  return TAnf::CalcEffDiam(DistNbhsV, Percentile);
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

} // namespace TSnap
