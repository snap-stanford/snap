#ifndef snap_link_pred
#define snap_link_pred

#include <Snap.h>

class TLpPredictor;

/////////////////////////////////////////////////
// Link prediction example
struct TNodeGrad { // (value, gradient) pair
  TFlt SumVal;       // value f_w(features)
  TFltV SumGradV;    // gradient of f_w
  TNodeGrad() : SumVal(0) { }
  TNodeGrad(const int& NAttr) : SumVal(0), SumGradV(NAttr) { }
  TNodeGrad(TSIn& SIn) : SumVal(SIn), SumGradV(SIn) { }
  void Save(TSOut& SOut) const { SumVal.Save(SOut); SumGradV.Save(SOut); }
  bool operator < (const TNodeGrad& VG) const  { return SumVal < VG.SumVal; }
}; 

class TEdgeGrad { 
public:
  TFlt Val;       // value f_w(features)
  TFltV GradV;    // gradient of f_w
  TFltV AttrV;    // attribute vector
public:
  TEdgeGrad() : Val(0) { }
  TEdgeGrad(const int& NAttr) : Val(0), GradV(NAttr) { }
  TEdgeGrad(const TFltV& EdgeAttrV) : Val(0), GradV(EdgeAttrV.Len()), AttrV(EdgeAttrV) { }
  TEdgeGrad(const TEdgeGrad& EG) : Val(EG.Val), GradV(EG.GradV), AttrV(EG.AttrV) { }
  TEdgeGrad(TSIn& SIn) : Val(SIn), GradV(SIn) { } 
  void Save(TSOut& SOut) const { Val.Save(SOut); GradV.Save(SOut); }
  bool operator < (const TEdgeGrad& EG) const  { return Val < EG.Val; }
};

// EdgeDat...gradient (function value, function gradient) of each edge
class TLpExample : public TNodeEDatNet<TInt, TEdgeGrad> {
private:
  typedef TPt<TNodeEDatNet<TInt, TEdgeGrad> > PNet;
private:
  TInt SrcNId;                         // source node
  TIntV DstNIdV, NolNIdV;              // destination and no-link nodes
  TInt DummyNId;                       // random jump node (last node)
  THash<TInt, TFlt> PRankH;
  THash<TInt, TFltV> PrGradH;
public:
  TLpExample() : SrcNId(-1), DummyNId(-1) { CRef.MkRef(); }
  ~TLpExample() { CRef.UnRef(); }
  int GetNAttr() const { return BegEI().GetDat().AttrV.Len(); }
  TNodeI DummyNI() const { return GetNI(DummyNId); }

  void GetNetAttrV(TVec<TFltV>& AttrV) const;   // (dstnid, class, features)
  void GetNodeAttrV(TVec<TFltV>& AttrV) const;  // (dstnid, class, features)
  void GetLen2PathAttrV(TVec<TFltV>& AttrV) const;  // (dstnid, class, features)
  void SaveAttrV(FILE *F, const bool& NetA, const bool& NodeA, const bool& PathA) const;

  void InitQGraph();
  void UpdateQGraph(const TLpPredictor& Predictor);
  void CalcPageRank();
  void GenSmallExample1();
  void GenSmallExample2();
  void SaveTxt(FILE *F) const;
  //void SaveNodeA(FILE *F) const;
  void Dump(const bool& DumpPRank=true, const bool& DumpGrad=false, const bool& DumpNet=false) const;
  static void LoadTxt(const TStr& FNm, TVec<TLpExample>& ExV);
};

/////////////////////////////////////////////////
// Link prediction Predictor
class TLpPredictor {
public:
  double Alpha;   // random jump prob
  double B;       // margin
  double Lambda;  // regularization parameter
  double Z;       // huber loss window width
  TFltV WgtV;     // this is what we try to learn
public:
  TLpPredictor() : Alpha(0.85), B(0.01), Lambda(0.01), Z(0.01) { }
  double GetFVal(const TFltV& AttrV) const;
  void GetFGrad(const TFltV& AttrV, TFltV& GradV) const;
  double GetLossGrad(const TLpExample& Example);
  double GetLoss(const double& Val) const;
  double GetLossGrad(const double& Val) const;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace __OLD__ {
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Link prediction example
class TLpExample {
public:
  PNGraph Graph;
  TInt SrcNId;
  TIntV DstNIdV, NolNIdV;
  THash<TIntPr, TFltV> EAttrV;
public:
  TLpExample() : Graph(), SrcNId(-1), DstNIdV(), NolNIdV(), EAttrV() { }
  PNGraph GetGraph() const { return Graph; }
  const TFltV& GetEAttrV(const TIntPr& Edge) const { return EAttrV.GetDat(Edge); }
  int Attrs() const { return EAttrV[0].Len(); }
  
  void GenSmallExample1();
  void GenSmallExample2();
};

/////////////////////////////////////////////////
// Link prediction Predictor
class TLpPredictor {
private:
  double Z; // huber loss window width
public:
  TLpPredictor() : Z(0.01) { }
  double GetFVal(const TFltV& WgtV, const TFltV& AttrV) const;
  void GetFGrad(const TFltV& WgtV, const TFltV& AttrV, TFltV& GradV) const;
  double GetLoss(const double& Val) const;
  double GetLossGrad(const double& Val) const;
};

/////////////////////////////////////////////////
// Stochastic transition matrix: each directed edge has value and gradient
typedef TPair<TFlt, TFltV> TEdgeGrad;

class TQGraph : public TNodeEDatNet<TInt, TEdgeGrad> {
public:
  typedef enum { lntUndef, lntSrc, lntDst, lntNoLink, lntDummy } TLpNodeTy;
private:
  int DummyNId;
  THash<TIntPr, TEdgeGrad> EdgeGradH;  // f and f' for each edge
  THash<TInt, TEdgeGrad> NormConstH;   // sum f and sum f' for each node
  THash<TInt, TFlt> PRankH;
  THash<TInt, TFltV> PrGradH;
private:
  void UpdateNormConst();
  void UpdateGrad(const double& Alpha, const TIntPr& Edge, TEdgeGrad& Grad);
public:
  TQGraph() : DummyNId(-1) { }
  int GetDummy() const { return DummyNId; }
  void MakeGraph(const TLpExample& Example);
  void UpdateGraph(const TLpExample& Example, const TLpPredictor& Prd, const TFltV& WgtV);
  void CalcPRankGrad();
  double GetGradient(const TLpExample& Example, const TLpPredictor& Prd, TFltV& GradV) const;

  void Dump(const bool& OnlyPRank=true) const;
};

/////////////////////////////////////////////////
// Link prediction Base
class TLpBase {
private:
  TLpExample LpData;
  TLpPredictor LpPred;
  TQGraph QGraph;
  TFlt B;      // margin
  TFlt Alpha;  // random jump prob
  TFlt Lambda; // regularization parameter
  TFltV WgtV;  // this is what we aim to learn

public:
  TLpBase() { }
  void CalcDeriv();
  void TestGrad();
};

}; // __OLD__

#endif
