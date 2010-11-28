#ifndef sgraph_h
#define sgraph_h

/////////////////////////////////////////////////
// Includes
#include "base.h"
#include "mine.h"

/////////////////////////////////////////////////
// Simple-Vertex
class TSVrtx{
public:
  TInt VNmStrX;
  TIntFltPrV VrtxNWgtPrV;
  TFlt X, Y;
public:
  TSVrtx(): VNmStrX(-1), VrtxNWgtPrV(), X(0), Y(0){}
  TSVrtx(const TSVrtx& SVrtx):
    VNmStrX(SVrtx.VNmStrX), VrtxNWgtPrV(SVrtx.VrtxNWgtPrV),
    X(SVrtx.X), Y(SVrtx.Y){}
  TSVrtx(const int& _VNmStrX): VNmStrX(_VNmStrX), VrtxNWgtPrV(), X(0), Y(0){}
  explicit TSVrtx(TSIn& SIn):
    VNmStrX(SIn), VrtxNWgtPrV(SIn), X(SIn), Y(SIn){}
  void Save(TSOut& SOut) const {
    VNmStrX.Save(SOut); VrtxNWgtPrV.Save(SOut); X.Save(SOut); Y.Save(SOut);}

  TSVrtx& operator=(const TSVrtx& SVrtx){
    if (this!=&SVrtx){
      VNmStrX=SVrtx.VNmStrX; VrtxNWgtPrV=SVrtx.VrtxNWgtPrV;
      X=SVrtx.X; Y=SVrtx.Y;}
    return *this;}

  int GetEdges() const {return VrtxNWgtPrV.Len();}
  int GetEdgeDstVrtxN(const int& EdgeN) const {
    return VrtxNWgtPrV[EdgeN].Val1;}
  double GetEdgeWgt(const int& EdgeN) const {
    return VrtxNWgtPrV[EdgeN].Val2;}
};
typedef TVec<TSVrtx> TSVrtxV;

/////////////////////////////////////////////////
// Simple-Graph
ClassTPV(TSGraph, PSGraph, TSGraphV)//{
private:
  TStrPool VrtxStrPool;
  TSVrtxV VrtxV;
public:
  TSGraph(): VrtxStrPool(), VrtxV(){}
  static PSGraph New(){return PSGraph(new TSGraph());}
  ~TSGraph(){}
  TSGraph(TSIn& SIn): VrtxStrPool(SIn), VrtxV(SIn){}
  static PSGraph Load(TSIn& SIn){return new TSGraph(SIn);}
  void Save(TSOut& SOut) const {VrtxStrPool.Save(SOut); VrtxV.Save(SOut);}

  TSGraph& operator=(const TSGraph& SGraph){
    if (this!=&SGraph){VrtxStrPool=SGraph.VrtxStrPool; VrtxV=SGraph.VrtxV;}
    return *this;}

  // vertices
  int AddVrtx(const TStr& VNm){
    int VNmX=VrtxStrPool.AddStr(VNm); return VrtxV.Add(TSVrtx(VNmX));}
  int GetVrtxs() const {return VrtxV.Len();}
  TStr GetVrtxNm(const int& VrtxN) const {
    return VrtxStrPool.GetStr(VrtxV[VrtxN].VNmStrX);}
  void PutVrtxXY(const int& VrtxN, const double& X, const double& Y){
    VrtxV[VrtxN].X=X; VrtxV[VrtxN].Y=Y;}
  void GetVrtxXY(const int& VrtxN, double& X, double& Y) const {
    X=VrtxV[VrtxN].X; Y=VrtxV[VrtxN].Y;}

  // edges
  void ReserveVrtxEdges(const int& VrtxN, const int& Edges){
    VrtxV[VrtxN].VrtxNWgtPrV.Reserve(Edges);}
  void AddEdge(const int& SrcVrtxN, const int& DstVrtxN,
   const bool& DirP, const double Wgt=1){
    VrtxV[SrcVrtxN].VrtxNWgtPrV.Add(TIntFltPr(DstVrtxN, Wgt));
    if (!DirP){VrtxV[DstVrtxN].VrtxNWgtPrV.Add(TIntFltPr(SrcVrtxN, Wgt));}}
  void AddEdgeOrIncWgt(const int& SrcVrtxN, const int& DstVrtxN,
   const bool& DirP, const double Wgt=1);
  int GetEdges(const int& VrtxN) const {
    return VrtxV[VrtxN].VrtxNWgtPrV.Len();}
  int GetEdgeDstVrtxN(const int& VrtxN, const int& EdgeN) const {
    return VrtxV[VrtxN].VrtxNWgtPrV[EdgeN].Val1;}
  double GetEdgeWgt(const int& VrtxN, const int& EdgeN) const {
    return VrtxV[VrtxN].VrtxNWgtPrV[EdgeN].Val2;}
  void IncEdgeWgt(const int& VrtxN, const int& EdgeN, const double& Wgt=1){
    VrtxV[VrtxN].VrtxNWgtPrV[EdgeN].Val2+=Wgt;}

  // rescaling
  void _GetMnMxXY(double& MnX, double& MnY, double& MxX, double& MxY);
  void _AddBorders(const double& BorderFac,
   double& MnX, double& MnY, double& MxX, double& MxY);
  void RescaleXY(const double& BorderFac);
  void RescaleXY(const double& MnX, const double& MnY,
   const double& MxX, const double& MxY);

  // alternative representations
  PBowDocBs GetNhoodV(const int& MxVrtxLev, TIntH& DIdVrtxNH,
   const TStrStrH& BowDocNmToCatNmH=TStrStrH(), const int& MnEAddrDegree=0) const;

  // files
  static PSGraph LoadBin(const TStr& FNm){
    PSIn SIn=TFIn::New(FNm); return TSGraph::Load(*SIn);}
  void SaveBin(const TStr FNm) const {
    PSOut SOut=TFOut::New(FNm); Save(*SOut);}
  void SaveTxt(const TStr& FNm) const;
  void SavePajek(const TStr& FNm) const;

  // file formats
  static PSGraph GetSGraphFromAmazon(const TStr& FNm, const int& Recs=-1);
  static PSGraph GetSGraphFromFP5IST(const TStr& FNm);
  static PSGraph GetSGraphFromTest();
};

#endif
