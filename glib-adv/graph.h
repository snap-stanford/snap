#ifndef graph_h
#define graph_h

/////////////////////////////////////////////////
// Includes
#include "base.h"
#include "gks.h"

/////////////////////////////////////////////////
// Forward
ClassHdTPV(TGraph, PGraph, TGraphV)

/////////////////////////////////////////////////
// Vertex
ClassTPV(TVrtx, PVrtx, TVrtxV)//{
protected: // constant strings
  static TStr VIdNm;
  static TStr PpNm;
  static TStr VNmNm;
  static TStr DNmNm;
  static TStr BriefNm;
  static TStr MarkNm;
  static TStr WgtNm;
  static TStr HitsHubWNm;
  static TStr HitsAuthWNm;
  static TStr PageRankWNm;
  static TStr XNm;
  static TStr YNm;
  static TStr ZNm;
  static TStr ColorNm;
  static TStr ShapeNm;
  static TStr RectNm;
  static TStr RectX1Nm, X1Nm;
  static TStr RectY1Nm, Y1Nm;
  static TStr RectX2Nm, X2Nm;
  static TStr RectY2Nm, Y2Nm;
  static TStr DmNm;
private:
  UndefCopyAssign(TVrtx);
public:
  TVrtx(){}
  virtual ~TVrtx(){}
  TVrtx(TSIn&){Fail;}
  static PVrtx Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  bool operator==(TVrtx&){Fail; return false;}

  virtual void DefVrtx(const PGraph& Graph)=0;
  virtual int GetVId() const=0;
  virtual PPp GetPp() const=0;

  // names: id-name, description-name, brief-description-name
  virtual TStr GetVNm() const=0;
  virtual void PutDNm(const TStr& DNm){ GetPp()->PutValStr(DNmNm, DNm); }
  virtual TStr GetDNm() const {return GetPp()->GetValStr(DNmNm);}
  virtual void PutBrief(const int& Brief){GetPp()->PutValInt(BriefNm, Brief);}
  virtual int GetBrief() const {return GetPp()->GetValInt(BriefNm);}

  // mark
  virtual bool IsMark() const {return GetPp()->GetValInt(MarkNm)!=0;}
  virtual void ClrMark(){GetPp()->PutValInt(MarkNm, 0);}
  virtual void SetMark(){GetPp()->PutValInt(MarkNm, 1);}
  virtual int GetMark() const {return GetPp()->GetValInt(MarkNm);}
  virtual void PutMark(const int& Mark){GetPp()->PutValInt(MarkNm, Mark);}

  // weight
  virtual void PutWgt(const double& Wgt){GetPp()->PutValFlt(WgtNm, Wgt);}
  virtual double GetWgt() const {return GetPp()->GetValFlt(WgtNm);}

  // hits, weights
  virtual void PutHitsHubW(const double& HitsHubW){GetPp()->PutValFlt(HitsHubWNm, HitsHubW);}
  virtual double GetHitsHubW() const {return GetPp()->GetValFlt(HitsHubWNm);}
  virtual void PutHitsAuthW(const double& HitsAuthW){GetPp()->PutValFlt(HitsAuthWNm, HitsAuthW);}
  virtual double GetHitsAuthW() const {return GetPp()->GetValFlt(HitsAuthWNm);}

  // page-rank weight
  virtual void PutPageRankW(const double& PageRankW){GetPp()->PutValFlt(PageRankWNm, PageRankW);}
  virtual double GetPageRankW() const {return GetPp()->GetValFlt(PageRankWNm);}

  // X, Y, Z coordinates
  virtual void PutX(const double& X){GetPp()->PutValFlt(XNm, X);}
  virtual double GetX() const {return GetPp()->GetValFlt(XNm);}
  virtual void PutY(const double& Y){GetPp()->PutValFlt(YNm, Y);}
  virtual double GetY() const {return GetPp()->GetValFlt(YNm);}
  virtual void PutZ(const double& Z){GetPp()->PutValFlt(ZNm, Z);}
  virtual double GetZ() const {return GetPp()->GetValFlt(ZNm);}
  virtual void PutXY(const double& X, const double& Y){PutX(X); PutY(Y);}

  // color
  virtual void PutColor(const int& Color){GetPp()->PutValInt(ColorNm, Color);}
  virtual int GetColor() const {return GetPp()->GetValInt(ColorNm);}

  // drawing shape & bounding rectangle
  virtual void PutShape(const TStr& Shape){GetPp()->PutValStr(ShapeNm, Shape);}
  virtual TStr GetShape() const {return GetPp()->GetValStr(ShapeNm);}
  virtual void PutRect(const TGksRect& Rect);
  virtual TGksRect GetRect() const;
  virtual bool IsInRect(const double& X, const double& Y) const;

  // edges
  virtual void DelEId(const int& EId)=0;

  // input-edges
  virtual void AddInEId(const int& EId)=0;
  virtual int GetInEIds() const=0;
  virtual int GetInEId(const int& InEIdN) const=0;
  virtual bool NoInEIds() const {return GetInEIds()==0;}

  // output-edges
  virtual void AddOutEId(const int& EId)=0;
  virtual int GetOutEIds() const=0;
  virtual int GetOutEId(const int& OutEIdN) const=0;
  virtual bool NoOutEIds() const {return GetOutEIds()==0;}

  //  properties
  virtual PPp GetDmPp(){return GetPp()->GetPp(DmNm);}
  static PPp GetDrawPp();

  // xml
  static PVrtx LoadXml(const PXmlTok& XmlTok);
  PXmlTok GetXmlTok() const;

  // saving/loading text
  static PVrtx LoadTxt(TILx& Lx);
  void SaveTxt(TOLx& Lx) const;
};

/////////////////////////////////////////////////
// General-Vrtx
class TGVrtx: public TVrtx{
private:
  TInt VId;
  TStr VNm;
  TStr DNm;
  TInt Brief;
  TInt Mark;
  TFlt Wgt, HitsHubW, HitsAuthW, PageRankW;
  TFlt X, Y, Z;
  TInt Color;
  TStr Shape;
  TGksRect Rect;
  PPp Pp;
  TIntV InEIdV;
  TIntV OutEIdV;
public:
  TGVrtx(const int& _VId=-1, const TStr& _VNm=TStr(), const PPp& _Pp=NULL):
    TVrtx(), VId(_VId), VNm(_VNm), DNm(TStr()), Brief(0), Mark(0),
    Wgt(0), HitsHubW(0), HitsAuthW(0), PageRankW(0),
    X(0), Y(0), Z(0),
    Color(), Shape(), Rect(),
    Pp(_Pp), InEIdV(), OutEIdV(){}
  TGVrtx(const TStr& _VNm, const PPp& _Pp=NULL):
    TVrtx(), VId(-1), VNm(_VNm), DNm(TStr()), Brief(0), Mark(0),
    Wgt(0), HitsHubW(0), HitsAuthW(0), PageRankW(0),
    X(0), Y(0), Z(0),
    Color(), Shape(), Rect(),
    Pp(_Pp), InEIdV(), OutEIdV(){}
  static PVrtx New(const TStr& VNm, const PPp& Pp=NULL){
    return new TGVrtx(VNm, Pp);}
  static PVrtx New(const int& VId, const TStr& VNm, const PPp& Pp=NULL){
    return new TGVrtx(VId, VNm, Pp);}
  virtual ~TGVrtx(){}
  TGVrtx(TSIn&){Fail;}
  void Save(TSOut&){Fail;}

  virtual void DefVrtx(const PGraph& Graph);
  virtual int GetVId() const {return VId;}
  virtual PPp GetPp() const { return Pp;}

  virtual TStr GetVNm() const {
    if (VNm.Empty()){return TStr('V')+TInt::GetStr(VId);} else {return VNm;}}
  virtual TStr GetDNm() const { return DNm; }
  virtual void PutDNm(const TStr& _DNm) { DNm = _DNm; }
  virtual int GetBrief() const { return Brief; }
  virtual void PutBrief(const int& _Brief) { Brief = _Brief; }

  virtual bool IsMark() const {return Mark!=0;}
  virtual void ClrMark(){Mark=0;}
  virtual void SetMark(){Mark=1;}
  virtual int GetMark() const {return Mark;}
  virtual void PutMark(const int& _Mark){Mark=_Mark;}

  virtual double GetWgt() const {return Wgt;}
  virtual void PutWgt(const double& _Wgt){Wgt=_Wgt;}
  virtual double GetHitsHubW() const {return HitsHubW;}
  virtual void PutHitsHubW(const double& _HitsHubW){HitsHubW=_HitsHubW;}
  virtual double GetHitsAuthW() const {return HitsAuthW;}
  virtual void PutHitsAuthW(const double& _HitsAuthW){HitsAuthW=_HitsAuthW;}
  virtual double GetPageRankW() const {return PageRankW;}
  virtual void PutPageRankW(const double& _PageRankW){PageRankW=_PageRankW;}

  virtual void PutX(const double& _X){X=_X;}
  virtual double GetX() const {return X;}
  virtual void PutY(const double& _Y){Y=_Y;}
  virtual double GetY() const {return Y;}
  virtual void PutZ(const double& _Z){Z=_Z;}
  virtual double GetZ() const {return Z;}
  virtual void PutXY(const double& _X, const double& _Y){X=_X; Y=_Y;}

  virtual void PutColor(const int& _Color){Color=_Color;}
  virtual int GetColor() const {return Color;}
  virtual void PutShape(const TStr& _Shape){Shape=_Shape;}
  virtual TStr GetShape() const {return ShapeNm;}
  virtual void PutRect(const TGksRect& _Rect){Rect=_Rect;}
  virtual TGksRect GetRect() const {return Rect;}
  virtual bool IsInRect(const double& X, const double& Y) const {
    return Rect.IsIn(X, Y);}

  void DelEId(const int& EId){InEIdV.DelIfIn(EId); OutEIdV.DelIfIn(EId);}

  virtual void AddInEId(const int& EId){InEIdV.Add(EId);}
  virtual int GetInEIds() const {return InEIdV.Len();}
  virtual int GetInEId(const int& InEIdN) const {return InEIdV[InEIdN];}

  virtual void AddOutEId(const int& EId){OutEIdV.Add(EId);}
  virtual int GetOutEIds() const {return OutEIdV.Len();}
  virtual int GetOutEId(const int& OutEIdN) const {return OutEIdV[OutEIdN];}
};

/////////////////////////////////////////////////
// Edge
ClassTPV(TEdge, PEdge, TEdgeV)//{
protected: // constant strings
  static TStr EIdNm;
  static TStr VId1Nm;
  static TStr VId2Nm;
  static TStr PpNm;
  static TStr DirPNm;
  static TStr ENmNm;
  static TStr MarkNm;
  static TStr WgtNm;
  static TStr WidthNm;
  static TStr DNmNm;
  static TStr BriefNm;
  static TStr RectNm;
  static TStr RectX1Nm, X1Nm;
  static TStr RectY1Nm, Y1Nm;
  static TStr RectX2Nm, X2Nm;
  static TStr RectY2Nm, Y2Nm;
  static TStr DmNm;
private:
  UndefCopyAssign(TEdge);
public:
  TEdge(){}
  virtual ~TEdge(){}
  TEdge(TSIn&){Fail;}
  static PEdge Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  bool operator==(TEdge&){Fail; return false;}

  virtual void DefEdge(const PGraph& Graph)=0;
  virtual int GetEId() const=0;
  virtual int GetVId1() const=0;
  virtual int GetVId2() const=0;
  virtual int GetInVId() const {IAssert(IsDir()); return GetVId1();}
  virtual int GetOutVId() const {IAssert(IsDir()); return GetVId2();}
  virtual int GetOtherVId(const PVrtx& Vrtx) const {
    if (Vrtx->GetVId()==GetVId1()){return GetVId2();} else {return GetVId1();}}
  virtual bool IsDir() const=0;
  virtual TStr GetENm() const=0;
  virtual PPp GetPp() const=0;

  // mark
  virtual bool IsMark() const=0;
  virtual void ClrMark()=0;
  virtual void SetMark()=0;
  virtual int GetMark() const=0;
  virtual void PutMark(const int& _Mark)=0;

  // weights
  virtual double GetWgt() const=0;
  virtual void PutWgt(const double& _Wgt)=0;

  // width
  virtual int GetWidth() const=0;
  virtual void PutWidth(const double& _Width)=0;

  // draw
  virtual void PutDNm(const TStr& DNm){GetPp()->PutValStr(DNmNm, DNm);}
  virtual TStr GetDNm() const {return GetPp()->GetValStr(DNmNm);}
  virtual void PutBrief(const int& Brief){GetPp()->PutValInt(BriefNm, Brief);}
  virtual int GetBrief() const {return GetPp()->GetValInt(BriefNm);}
  virtual void PutRect(const TGksRect& Rect);
  virtual TGksRect GetRect() const;
  virtual bool IsInRect(const double& X, const double& Y) const;
  virtual PPp GetDmPp(){return GetPp()->GetPp(DmNm);}

  // properties
  static PPp GetDrawPp();

  // xml
  static PEdge LoadXml(const PXmlTok& XmlTok);
  PXmlTok GetXmlTok() const;

  // saving/loading text
  static PEdge LoadTxt(TILx& Lx);
  void SaveTxt(TOLx& Lx) const;
};

/////////////////////////////////////////////////
// General-Edge
class TGEdge: public TEdge{
private:
  TInt EId;
  TInt VId1;
  TInt VId2;
  TBool DirP;
  TStr ENm;
  TStr DNm; //B:
  TInt Mark;
  TFlt Wgt;
  TInt Width;
  PPp Pp;
public:
  TGEdge(const int& _EId, const int& _VId1, const int& _VId2,
   const bool& _DirP, const TStr& _ENm=TStr(), const PPp& _Pp=NULL);
  TGEdge(const PVrtx& Vrtx1, const PVrtx& Vrtx2,
   const TStr& _ENm=TStr(), const bool& _DirP=true, const PPp& _Pp=NULL);
  static PEdge New(const PVrtx& Vrtx1, const PVrtx& Vrtx2,
   const TStr& ENm=TStr(), const bool& DirP=true, const PPp& Pp=NULL){
    return new TGEdge(Vrtx1, Vrtx2, ENm, DirP, Pp);}
  virtual ~TGEdge(){}
  TGEdge(TSIn&){Fail;}
  static PEdge Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  virtual void DefEdge(const PGraph& Graph);
  virtual int GetEId() const {return EId;}
  virtual int GetVId1() const {return VId1;}
  virtual int GetVId2() const {return VId2;}
  virtual bool IsDir() const {return DirP;}
  virtual TStr GetENm() const {
    if (ENm.Empty()){return TStr('E')+TInt::GetStr(EId);} else {return ENm;}}
  virtual PPp GetPp() const {return Pp;}

  virtual bool IsMark() const {return Mark!=0;}
  virtual void ClrMark(){Mark=0;}
  virtual void SetMark(){Mark=1;}
  virtual int GetMark() const {return Mark;}
  virtual void PutMark(const int& _Mark){Mark=_Mark;}

  virtual double GetWgt() const {return Wgt;}
  virtual void PutWgt(const double& _Wgt){Wgt=_Wgt;}

  virtual TStr GetDNm() const { return DNm; }
  virtual void PutDNm(const TStr& _DNm) { DNm = _DNm; }

  virtual int GetWidth() const {return Width;}
  virtual void PutWidth(const double& _Width){Width=int(_Width);}
};

/////////////////////////////////////////////////
// Graph
ClassTPV(TGraph, PGraph, TGraphV)//{
public: // constant strings
  static TStr PpNm;
  static TStr VrtxNm;
  static TStr EdgeNm;
  static TStr GNmNm;
  static TStr DNmNm;
  static TStr BriefNm;
protected: // registration of extended types
  // default constructor
  typedef PGraph (*TGraphNew)();
  typedef TFunc<TGraphNew> TGraphNewF;
  static THash<TStr, TGraphNewF> TypeToNewFH;
  // load constructor
  typedef PGraph (*TGraphLoad)(TSIn& SIn);
  typedef TFunc<TGraphLoad> TGraphLoadF;
  static THash<TStr, TGraphLoadF> TypeToLoadFH;
private:
  UndefCopyAssign(TGraph);
public: // vertex & edge ids
  TInt LastVId;
  TInt LastEId;
  int GetNewVId() const {return LastVId;}
  int GetNewEId() const {return LastEId;}
  void DefVId(const int& VId){if (VId>=LastVId){LastVId=VId+1;}}
  void DefEId(const int& EId){if (EId>=LastEId){LastEId=EId+1;}}
public:
  TGraph(): LastVId(0), LastEId(0){}
  virtual ~TGraph(){}
  static PGraph New(const TStr& TypeNm=TStr());
  TGraph(TSIn&){Fail;}
  static PGraph Load(TSIn& SIn);
  void Save(TSOut&){Fail;}
  static bool Reg(
   const TStr& TypeNm, const TGraphNewF& NewF, const TGraphLoadF& LoadF);

  virtual TStr GetTypeNm() const=0;
  virtual void Defrag()=0;

  // properties
  virtual PPp GetPp() const=0;
  virtual int AddPp(const PPp& Pp)=0;
  virtual int GetPps() const=0;
  virtual PPp GetPp(const int& PpN) const=0;

  // vertices
  virtual int AddVrtx(const PVrtx& Vrtx)=0;
  virtual void DelVrtx(const int& VId)=0;
  virtual void DelVrtx(const PVrtx& Vrtx)=0;
  virtual int GetVrtxs() const=0;
  virtual PVrtx GetVrtx(const int& VId) const=0;
  virtual PVrtx GetVrtx(const TStr& VNm) const=0;
  virtual PVrtx GetRndVrtx(TRnd& Rnd) const=0;
  bool IsVrtx(const TStr& VNm) const {
    PVrtx Vrtx; return IsVrtx(VNm, Vrtx);}
  virtual bool IsVrtx(const TStr& VNm, PVrtx& Vrtx) const=0;
  virtual int FFirstVrtx() const=0;
  virtual bool FNextVrtx(int& VrtxP, PVrtx& Vrtx) const=0;
  virtual void ClrVrtxMarks() const=0;

  // edges
  virtual int AddEdge(const PEdge& Edge)=0;
  virtual void DelEdge(const int& EId)=0;
  virtual void DelEdge(const PEdge& Edge)=0;
  virtual int GetEdges() const=0;
  virtual PEdge GetEdge(const int& EId) const=0;
  virtual PEdge GetEdge(const TStr& ENm) const=0;
  virtual bool IsEdge(const TStr& ENm, PEdge& Edge) const=0;
  virtual int FFirstEdge() const=0;
  virtual bool FNextEdge(int& EdgeP, PEdge& Edge) const=0;
  virtual bool IsVrtxsEdge(
   const int& VId1, const int& VId2, const bool& DirP, PEdge& Edge) const=0;
  bool IsVrtxsEdge(
   const int& VId1, const int& VId2, const bool& DirP) const {
    PEdge Edge; return IsVrtxsEdge(VId1, VId2, DirP, Edge);}

  // properties
  virtual void PutGNm(const TStr& GNm){GetPp()->PutValStr(GNmNm, GNm);}
  virtual TStr GetGNm() const {return GetPp()->GetValStr(GNmNm);}
  virtual void PutDNm(const TStr& DNm){GetPp()->PutValStr(DNmNm, DNm);}
  virtual TStr GetDNm() const {return GetPp()->GetValStr(DNmNm);}
  virtual void PutBrief(const int& Brief){GetPp()->PutValInt(BriefNm, Brief);}
  virtual int GetBrief() const {return GetPp()->GetValInt(BriefNm);}

  // rescaling
  void _GetMnMxXY(double& MnX, double& MnY, double& MxX, double& MxY) const;
  void _AddBorders(const double& BorderFac,
   double& MnX, double& MnY, double& MxX, double& MxY) const;
  void RescaleXY(const double& BorderFac, const PVrtx& CenterVrtx=NULL) const;
  void RescaleXY(const double& MnX, const double& MnY,
   const double& MxX, const double& MxY) const;

  // algorithms
  virtual int _IsEdgeXYInts(
   const PVrtx& Vrtx0, const PVrtx& Vrtx1, const PVrtx& Vrtx2) const;
  virtual bool IsEdgeXYInts(const PEdge& Edge1, const PEdge& Edge2) const;
  virtual double GetVrtxXYLen(const PVrtx& Vrtx1, const PVrtx& Vrtx2) const;
  virtual double GetEdgeXYLen(const PEdge& Edge) const;
  virtual double GetSumEdgeXYLen() const;
  virtual void PlaceRndXY(TRnd& Rnd) const;
  virtual void PlaceBestRndXY(const int& Tries, TRnd& Rnd) const;
  virtual double GetSimAnnXYEnergy(
   const double& NodeDistrWgt, const double& BorderWgt,
   const double& EdgeLenWgt, const double& EdgeCrossWgt,
   const double& NodeEdgeDistWgt, const bool& UseNodeEdgeDist) const;
  virtual double GetSimAnnXYEnergy(
   const PXmlDoc& CfgXmlDoc=NULL, const bool& UseNodeEdgeDist=true) const;
  virtual void PlaceSimAnnXY(
   TRnd& Rnd, const PGks& Gks=NULL,
   const PXmlDoc& CfgXmlDoc=NULL, const PNotify& Notify=NULL,
   const int& _TempStages=100, const int& _TempStageTrys=100,
   const int& Secs=-1) const;
  virtual void PlaceFineTuning(
   TRnd& Rnd, const PGks& Gks, const PNotify& Notify=NULL,
   const int& Secs=-1) const;
  virtual void PlaceCircle() const;
  virtual void PlaceTreeAsStar(
   const PVrtx& HomeVrtx, const int& Lev,
   const double& HomeX, const double& HomeY,
   const double& MnAng, const double& MxAng) const;
  virtual void PlaceTreeAsStar() const;
  virtual void PlaceGraphAsStar(
   const PVrtx& HomeVrtx, const TStr& DrawENm) const;
  virtual void SetEdgeWidth(const int& Intvs) const;
  virtual void Draw(const PGks& Gks, const bool& SimpleP=false,
   const bool& DrawEdgesP=true, const int& FontSize=8, 
   const int& EdgeFontSize=8, const int& MxLnLen=-1) const;
  virtual void _DrawTxtTree(const PVrtx& Vrtx, TChAV& LnV) const;
  virtual void DrawTxtTree(const PSOut& SOut) const;
  virtual void DrawTxtTree(const TStr& FNm) const {
    PSOut SOut=TFOut::New(FNm); DrawTxtTree(SOut);}
  virtual bool IsVrtxAt(
   const PGks& Gks, const int& X, const int& Y, PVrtx& Vrtx) const;
  virtual bool IsEdgeAt(
   const PGks& Gks, const int& X, const int& Y, PEdge& Edge) const;
  virtual bool IsDir() const;
  virtual bool _IsDag(const PVrtx& Vrtx, const int& Mark) const;
  virtual bool IsDag() const;
  virtual bool IsTree(PVrtx& RootVrtx) const;
  virtual PVrtx GetTreeRoot() const {
    PVrtx RootVrtx; IAssert(IsTree(RootVrtx)); return RootVrtx;}
  virtual void GetLeafVIdV(const int& RootVId, TIntV& VIdV) const;
  virtual void GetHitsW() const;
  virtual void GetTopHitsVrtxV(TVrtxV& TopHubVrtxV, TVrtxV& TopAuthVrtxV) const;
  virtual void GetConnVrtxV(PVrtx& RootVrtx, TVrtxV& ConnVrtxV) const;
  static PGraph GetRndGraph(
   TRnd& Rnd, const int& Vrtxs, const int& Edges,
   const bool& DirEdgeP=true, const int& EdgeToSameVrtxP=false);

  // xml
  static PGraph LoadXml(const PXmlDoc& XmlDoc, const TStr& TypeNm=TStr());
  static PGraph LoadXml(const PSIn& SIn, const TStr& TypeNm=TStr()){
    PXmlDoc XmlDoc=TXmlDoc::LoadTxt(SIn); return LoadXml(XmlDoc, TypeNm);}
  static PGraph LoadXml(const TStr& FNm, const TStr& TypeNm=TStr()){
    PSIn SIn=TFIn::New(FNm); return LoadXml(SIn, TypeNm);}
  PXmlDoc GetXmlDoc() const;
  void SaveXml(const PSOut& SOut) const {
    PXmlDoc XmlDoc=GetXmlDoc(); XmlDoc->SaveTxt(SOut);}
  void SaveXml(const TStr& FNm) const {
    PSOut SOut=TFOut::New(FNm); SaveXml(SOut);}

  // text files
  static const TStr FExt;
  static const TStr HitsFExt;
  static const TStr PageRankFExt;
  static PGraph LoadTxt(const PSIn& SIn, const TStr& TypeNm=TStr());
  static PGraph LoadTxt(const TStr& FNm, const TStr& TypeNm=TStr()){
    PSIn SIn=TFIn::New(FNm); return LoadTxt(SIn, TypeNm);}
  void SaveTxt(const PSOut& SOut) const;
  void SaveTxt(const TStr& FNm) const {
    PSOut SOut=TFOut::New(FNm); SaveTxt(SOut);}
  void SaveTxtTab(const TStr& FNm) const;
  void SaveTxtPajek(const TStr& FNm) const;
  void SaveTxtMatlab(const TStr& FNm) const;
  void SaveTxtHits(const TStr& FNm, const int& TopN=-1) const;
  void _SaveTxtSparseNbrs(const int& SrcVId,
   const int& Lev, const int& MxLev, TIntFltH& VIdToWgtH) const;
  void SaveTxtSparseNbrs(
   const TStr& VarIdFNm, const TStr& TupIdFNm, const TStr& TupVarValFNm,
   const int& MxLev) const;
};

/////////////////////////////////////////////////
// General-Graph
class TGGraph: public TGraph{
private: // registration
  static bool IsReg;
  static bool MkReg(){
    return TGraph::Reg(TTypeNm<TGGraph>(), TGraphNewF(&New), TGraphLoadF(&Load));}
private: // graph data
  int MxEId;
  PPp Pp;
  THash<TInt, PVrtx> VIdToVrtxH;
  THash<TInt, PEdge> EIdToEdgeH;
  TStrIntH VNmToVIdH;
  TStrIntH ENmToEIdH;
public:
  TGGraph(const int& EstVrtxs=1000, const int& EstEdges=1000):
    TGraph(), MxEId(0), Pp(TPp::GetSetPp(PpNm)),
    VIdToVrtxH(EstVrtxs), EIdToEdgeH(EstEdges),
    VNmToVIdH(EstVrtxs), ENmToEIdH(EstEdges){}
  virtual ~TGGraph(){}
  static PGraph New(){
    return PGraph(new TGGraph());}
  TGGraph(TSIn&){Fail;}
  static PGraph Load(TSIn& SIn){return PGraph(new TGGraph(SIn));}
  void Save(TSOut&){Fail;}

  virtual TStr GetTypeNm() const {return TTypeNm<TGGraph>();}
  virtual void Defrag(){
    VIdToVrtxH.Defrag(); EIdToEdgeH.Defrag();
    VNmToVIdH.Defrag(); ENmToEIdH.Defrag();}

  // properties
  virtual PPp GetPp() const {return Pp;}
  virtual int AddPp(const PPp& _Pp){return Pp->AddPp(_Pp);}
  virtual int GetPps() const {return Pp->GetPps();}
  virtual PPp GetPp(const int& PpN) const {return Pp->GetPp(PpN);}

  // vertices
  virtual int AddVrtx(const PVrtx& Vrtx);
  virtual void DelVrtx(const int& VId){DelVrtx(GetVrtx(VId));}
  virtual void DelVrtx(const PVrtx& Vrtx);
  virtual int GetVrtxs() const {return VIdToVrtxH.Len();}
  virtual PVrtx GetVrtx(const int& VId) const {return VIdToVrtxH.GetDat(VId);}
  virtual PVrtx GetVrtx(const TStr& VNm) const {
    return GetVrtx(VNmToVIdH.GetDat(VNm));}
  virtual PVrtx GetRndVrtx(TRnd& Rnd) const {
    return VIdToVrtxH[VIdToVrtxH.GetRndKeyId(Rnd)];}
  virtual bool IsVrtx(const TStr& VNm, PVrtx& Vrtx) const;
  virtual int FFirstVrtx() const {return VIdToVrtxH.FFirstKeyId();}
  virtual bool FNextVrtx(int& VrtxP, PVrtx& Vrtx) const;
  virtual void ClrVrtxMarks() const;

  // edges
  virtual int AddEdge(const PEdge& Edge);
  virtual void DelEdge(const int& EId){DelEdge(GetEdge(EId));}
  virtual void DelEdge(const PEdge& Edge);
  virtual int GetEdges() const {return EIdToEdgeH.Len();}
  virtual PEdge GetEdge(const int& EId) const {return EIdToEdgeH.GetDat(EId);}
  virtual PEdge GetEdge(const TStr& ENm) const {
    return GetEdge(ENmToEIdH.GetDat(ENm));}
  virtual bool IsEdge(const TStr& ENm, PEdge& Edge) const;
  virtual int FFirstEdge() const {return EIdToEdgeH.FFirstKeyId();}
  virtual bool FNextEdge(int& EdgeP, PEdge& Edge) const;
  virtual bool IsVrtxsEdge(
   const int& VId1, const int& VId2, const bool& DirP, PEdge& Edge) const;
};

#endif
