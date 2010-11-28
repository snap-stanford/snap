/////////////////////////////////////////////////
// Graph K-Cores
// get a subgraph where every node has degree at least K
template<class PGraph>
class TKCore {
private:
  PGraph Graph;
  TIntH DegH;
  TInt CurK;
  TIntV NIdV;
private:
  void Init();
public:
  TKCore(const PGraph& _Graph) : Graph(_Graph) { Init(); }
  int GetCurK() const { return CurK; } // currrent value value of k
  int GetNextCore();                   // start with k=1 -core and proceed (until GetCoreNodes()==0)
  int GetCoreK(const int& K);          // get k-core directly
  int GetCoreNodes() const { return NIdV.Len(); }
  int GetCoreEdges() const;
  const TIntV& GetNIdV() const { return NIdV; } // nodes in the current core GetK()
  PGraph GetCoreG() const { return TSnap::GetSubGraph(Graph, NIdV); }
};

template<class PGraph>
void TKCore<PGraph>::Init() {
  DegH.Gen(Graph->GetNodes());
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    //DegH.AddDat(NI.GetId(), NI.GetOutDeg());
    DegH.AddDat(NI.GetId(), NI.GetDeg());
  }
  CurK = 0;
}

template<class PGraph>
int TKCore<PGraph>::GetCoreEdges() const {
  int CoreEdges = 0;
  for (int k = DegH.FFirstKeyId(); DegH.FNextKeyId(k); ) {
    CoreEdges += DegH[k];
  }
  return CoreEdges/2;
}

template<class PGraph>
int TKCore<PGraph>::GetNextCore() {
  TIntV DelV;
  int NDel=-1, Pass=1, AllDeg=0;
  TExeTm ExeTm;
  CurK++;
  printf("Get K-core: %d\n", CurK());
  while (NDel != 0) {
    NDel = 0;
    for (int k = DegH.FFirstKeyId(); DegH.FNextKeyId(k); ) {
      const int NId = DegH.GetKey(k);
      const int Deg = DegH[k];
      if (Deg < CurK) {
        const typename PGraph::TObj::TNodeI NI = Graph->GetNI(NId);
        for (int e = 0; e < NI.GetDeg(); e++) {
          const int n = NI.GetNbhNId(e);
          const int nk = DegH.GetKeyId(n);
          if (nk != -1) { DegH[nk] -= 1; } // mark node deleted
        }
        DegH.DelKeyId(k);
        NDel++;  AllDeg++;
      }
    }
    printf("\r  pass %d]  %d deleted,  %d all deleted  [%s]", Pass++, NDel, AllDeg, ExeTm.GetStr());
  }
  printf("\n");
  DegH.Defrag();
  DegH.GetKeyV(NIdV);
  NIdV.Sort();
  return NIdV.Len(); // all nodes in the current core
}

template<class PGraph>
int TKCore<PGraph>::GetCoreK(const int& K) {
  Init();
  CurK = K-1;
  return GetNextCore();
}

/////////////////////////////////////////////////
// Snap
namespace TSnap {

template<class PGraph>
PGraph GetKCore(const PGraph& Graph, const int& K) {
  TKCore<PGraph> KCore(Graph);
  KCore.GetCoreK(K);
  return TSnap::GetSubGraph(Graph, KCore.GetNIdV());
}

} // namespace TSnap
