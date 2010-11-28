#ifndef cyc_h
#define cyc_h

/////////////////////////////////////////////////
// Includes
#include "net.h"
#include "mine.h"
#include "google.h"

/////////////////////////////////////////////////
// Cyc-Edge
ClassTV(TCycEdge, TCycEdgeV)//{
public:
  TInt RelId;
  TInt DstVId;
public:
  TCycEdge(): RelId(), DstVId(){}
  TCycEdge(const TCycEdge& CycEdge):
    RelId(CycEdge.RelId), DstVId(CycEdge.DstVId){}
  TCycEdge(const int& _RelId, const int& _DstVId):
    RelId(_RelId), DstVId(_DstVId){}
  TCycEdge(TSIn& SIn): 
    RelId(SIn), DstVId(SIn){}
  void Save(TSOut& SOut) const {
    RelId.Save(SOut); DstVId.Save(SOut);}

  TCycEdge& operator=(const TCycEdge& CycEdge){
    if (this!=&CycEdge){RelId=CycEdge.RelId; DstVId=CycEdge.DstVId;} return *this;}
  bool operator==(const TCycEdge& CycEdge) const {
    return (RelId==CycEdge.RelId)&&(DstVId==CycEdge.DstVId);}
  bool operator<(const TCycEdge& CycEdge) const {
    return (RelId<CycEdge.RelId)||((RelId==CycEdge.RelId)&&(DstVId<CycEdge.DstVId));}

  // retrieve components
  int GetRelId() const {return RelId;}
  int GetDstVId() const {return DstVId;}
};

/////////////////////////////////////////////////
// Cyc-Vertex
typedef enum {
  cvfUndef, cvfMn=0,
  cvfNum, cvfStr, 
  cvfCycL, cvfCycLConst, cvfCycLExpr, cvfCycLPred, cvfBackLink,
  cvfHumanOk, cvfHumanRelevant, cfvHumanIrrelevant, cfvHumanClarifying,
  cvfMx} TCycVrtxFlag;

class TCycVrtx{
public:
  TInt VId;
  TB32Set FlagSet;
  TCycEdgeV EdgeV;
public:
  TCycVrtx(): VId(), FlagSet(), EdgeV(){}
  TCycVrtx(const TCycVrtx& CycVrtx):
    VId(CycVrtx.VId), FlagSet(CycVrtx.FlagSet), EdgeV(CycVrtx.EdgeV){}
  TCycVrtx(const TInt& _VId, const TB32Set& _FlagSet, const TCycEdgeV& _EdgeV):
    VId(_VId), FlagSet(_FlagSet), EdgeV(_EdgeV){}
  TCycVrtx(TSIn& SIn):
    VId(SIn), FlagSet(SIn), EdgeV(SIn){}
  void Save(TSOut& SOut) const {
    VId.Save(SOut); FlagSet.Save(SOut); EdgeV.Save(SOut);}

  TCycVrtx& operator=(const TCycVrtx& CycVrtx){
    if (this!=&CycVrtx){
      VId=CycVrtx.VId; FlagSet=CycVrtx.FlagSet; EdgeV=CycVrtx.EdgeV;}
    return *this;}

  // vertex-id
  void PutVId(const int& _VId){VId=_VId;}
  int GetVId() const {return VId;}

  // flags
  void SetFlag(const int& FlagId, const bool& Val=true){
    FlagSet.SetBit(FlagId, Val);}
  bool IsFlag(const int& FlagId) const {
    return FlagSet.In(FlagId);}

  // edges
  int AddEdge(const TCycEdge& Edge){return EdgeV.AddUnique(Edge);}
  int GetEdges() const {return EdgeV.Len();}
  TCycEdge& GetEdge(const int& EdgeN){return EdgeV[EdgeN];}

  // flag strings
  static TStr GetCycVrtxFlagStr(const TCycVrtxFlag& CycVrtxFlag);
  TStr GetFlagStr() const;
};

/////////////////////////////////////////////////
// Cyc-Base
ClassTP(TCycBs, PCycBs)//{
private:
  TStrHash<TCycVrtx> VNmToVrtxH; // cyc-graph
  UndefCopyAssign(TCycBs);
public:
  TCycBs():
    VNmToVrtxH(){}
  static PCycBs New(){return new TCycBs();}
  TCycBs(TSIn& SIn):
    VNmToVrtxH(SIn){SIn.LoadCs();}
  static PCycBs Load(TSIn& SIn){return new TCycBs(SIn);}
  void Save(TSOut& SOut){
    VNmToVrtxH.Save(SOut); SOut.SaveCs();}

  // cyc-graph-vertices
  int AddVNm(const TStr& VNm);
  int GetVIds() const {return VNmToVrtxH.Len();}
  bool IsVNm(const TStr& VNm) const {return VNmToVrtxH.IsKey(VNm);}
  int GetVId(const TStr& VNm) const {return VNmToVrtxH.GetKeyId(VNm);}
  TStr GetVNm(const int& VId) const {return VNmToVrtxH.GetKey(VId);}
  TCycVrtx& GetVrtx(const TStr& VNm){return VNmToVrtxH[GetVId(VNm)];}
  TCycVrtx& GetVrtx(const int& VId){return VNmToVrtxH[VId];}

  // cyc-graph-edges
  void AddEdge(const int& SrcVId, const int& RelId, const int& DstVId){
    VNmToVrtxH[SrcVId].AddEdge(TCycEdge(RelId, DstVId));}
  void AddEdge(const TStr& SrcVNm, const TStr& RelNm, const TStr& DstVNm){
    AddEdge(AddVNm(SrcVNm), AddVNm(RelNm), AddVNm(DstVNm));}
  void GetRelNmV(TStrV& RelNmV);

  // cycl
  static TStr GetDocStrFromCycLConstStr(const TStr& CycLConstStr);

  // onto-light
  static PLwOnto LoadCycVoc(const TStr& CycBsFNm, const bool& HumanOkOnlyP);

  // files
  static PCycBs LoadBin(const TStr& FNm){
    TFIn SIn(FNm); return Load(SIn);}
  void SaveBin(const TStr& FNm){
    TFOut SOut(FNm); Save(SOut);}
  void SaveTxt(const TStr& FNm);
  void _SaveTaxonomyTxt(
   FILE* fOut, const int& Lev, TIntPrV& RelIdVIdPrV, TIntIntH& VIdToLevH);
  void SaveTaxonomyTxt(const TStr& FNm);
  void SaveStatTxt(const TStr& FNm);

  // loading from cyc-xml-dump files
  static PCycBs LoadCycXmlDump(const TStr& FPath);
};

#endif
