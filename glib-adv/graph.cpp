/////////////////////////////////////////////////
// Includes
#include "stdafx.h"
#include "graph.h"

/////////////////////////////////////////////////
// Vertex
TStr TVrtx::VIdNm="VId";
TStr TVrtx::PpNm="Pp";
TStr TVrtx::VNmNm="VNm";
TStr TVrtx::DNmNm="DNm";
TStr TVrtx::BriefNm="Brief";
TStr TVrtx::MarkNm="Mark";
TStr TVrtx::WgtNm="Wgt";
TStr TVrtx::HitsHubWNm="HitsHubW";
TStr TVrtx::HitsAuthWNm="HitsAuthW";
TStr TVrtx::PageRankWNm="PageRankW";
TStr TVrtx::XNm="X";
TStr TVrtx::YNm="Y";
TStr TVrtx::ZNm="Z";
TStr TVrtx::ColorNm="Color";
TStr TVrtx::ShapeNm="Shape";
TStr TVrtx::RectNm="Rect";
TStr TVrtx::RectX1Nm="Rect|X1"; TStr TVrtx::X1Nm="X1";
TStr TVrtx::RectY1Nm="Rect|Y1"; TStr TVrtx::Y1Nm="Y1";
TStr TVrtx::RectX2Nm="Rect|X2"; TStr TVrtx::X2Nm="X2";
TStr TVrtx::RectY2Nm="Rect|Y2"; TStr TVrtx::Y2Nm="Y2";
TStr TVrtx::DmNm="Dm";

void TVrtx::PutRect(const TGksRect& Rect){
  GetPp()->PutValInt(RectX1Nm, Rect.GetX1());
  GetPp()->PutValInt(RectY1Nm, Rect.GetY1());
  GetPp()->PutValInt(RectX2Nm, Rect.GetX2());
  GetPp()->PutValInt(RectY2Nm, Rect.GetY2());
}

TGksRect TVrtx::GetRect() const {
  int X1=GetPp()->GetValInt(RectX1Nm);
  int Y1=GetPp()->GetValInt(RectY1Nm);
  int X2=GetPp()->GetValInt(RectX2Nm);
  int Y2=GetPp()->GetValInt(RectY2Nm);
  return TGksRect(X1, Y1, X2, Y2);
}

bool TVrtx::IsInRect(const double& X, const double& Y) const {
  TGksRect Rect=TVrtx::GetRect();
  return Rect.IsIn(int(X), int(Y));
}

PPp TVrtx::GetDrawPp(){
  PPp Pp=TPp::GetSetPp("Draw");
  Pp->AddPpStr(DNmNm, "", "");
  Pp->AddPpFlt(XNm, "", TFlt::Mn, TFlt::Mx, 0);
  Pp->AddPpFlt(YNm, "", TFlt::Mn, TFlt::Mx, 0);
  Pp->AddPpInt(BriefNm, "", TInt::Mn, TInt::Mx, 0);
  //Pp->AddPpInt(ColorNm, 0, 0);
  Pp->AddPpStr(ShapeNm, "", "");
  PPp RectPp=TPp::GetSetPp(RectNm);
  RectPp->AddPpInt(X1Nm, "", TInt::Mn, TInt::Mx, 0);
  RectPp->AddPpInt(Y1Nm, "", TInt::Mn, TInt::Mx, 0);
  RectPp->AddPpInt(X2Nm, "", TInt::Mn, TInt::Mx, 0);
  RectPp->AddPpInt(Y2Nm, "", TInt::Mn, TInt::Mx, 0);
  Pp->AddPp(RectPp);
  return Pp;
}

PVrtx TVrtx::LoadXml(const PXmlTok& XmlTok){
  // collect attributes
  int VId=XmlTok->GetIntArgVal(VIdNm);
  TStr VNm=XmlTok->GetStrArgVal(VNmNm, "");
  int Mark=XmlTok->GetIntArgVal(MarkNm, 0);
  double Wgt=XmlTok->GetFltArgVal(WgtNm, 0);
  double HitsHubW=XmlTok->GetFltArgVal(HitsHubWNm, 0);
  double HitsAuthW=XmlTok->GetFltArgVal(HitsAuthWNm, 0);
  double PageRankW=XmlTok->GetFltArgVal(PageRankWNm, 0);
  double X=XmlTok->GetFltArgVal(XNm, 0);
  double Y=XmlTok->GetFltArgVal(YNm, 0);
  double Z=XmlTok->GetFltArgVal(ZNm, 0);
  PPp Pp;
  // create vertex
  PVrtx Vrtx=PVrtx(new TGVrtx(VId, VNm, Pp));
  Vrtx->PutMark(Mark); Vrtx->PutWgt(Wgt);
  Vrtx->PutHitsHubW(HitsHubW); Vrtx->PutHitsAuthW(HitsAuthW);
  Vrtx->PutPageRankW(PageRankW);
  Vrtx->PutX(X); Vrtx->PutY(Y); Vrtx->PutZ(Z);
  return Vrtx;
}

PXmlTok TVrtx::GetXmlTok() const {
  PXmlTok VrtxTok=TXmlTok::New(TGraph::VrtxNm);
  VrtxTok->AddArg(VIdNm, GetVId());
  if (!GetVNm().Empty()){VrtxTok->AddArg(VNmNm, GetVNm());}
  if (GetMark()!=0){VrtxTok->AddArg(MarkNm, GetMark());}
  if (GetWgt()!=0){VrtxTok->AddArg(WgtNm, GetWgt());}
  if (GetHitsHubW()!=0){VrtxTok->AddArg(HitsHubWNm, GetHitsHubW());}
  if (GetHitsAuthW()!=0){VrtxTok->AddArg(HitsAuthWNm, GetHitsAuthW());}
  if (GetPageRankW()!=0){VrtxTok->AddArg(PageRankWNm, GetPageRankW());}
  if (GetX()!=0){VrtxTok->AddArg(XNm, GetX());}
  if (GetY()!=0){VrtxTok->AddArg(YNm, GetY());}
  if (GetZ()!=0){VrtxTok->AddArg(ZNm, GetZ());}
  return VrtxTok;
}

PVrtx TVrtx::LoadTxt(TILx& Lx){
  Lx.GetSym(syLBracket);
  int VId=Lx.GetVarInt(VIdNm, false);
  TStr VNm; int Mark=0; double Wgt=0;
  double HitsHubW=0; double HitsAuthW=0; double PageRankW=0;
  double X=0; double Y=0; double Z=0; PPp Pp;
  if (Lx.IsVar(VNmNm)){VNm=Lx.GetVarStr(VNmNm, false);}
  VNm=TXmlLx::GetPlainStrFromXmlStr(VNm);
  if (Lx.IsVar(MarkNm)){Mark=Lx.GetVarInt(MarkNm, false);}
  if (Lx.IsVar(WgtNm)){Wgt=Lx.GetVarFlt(WgtNm, false);}
  if (Lx.IsVar(HitsHubWNm)){HitsHubW=Lx.GetVarFlt(HitsHubWNm, false);}
  if (Lx.IsVar(HitsAuthWNm)){HitsAuthW=Lx.GetVarFlt(HitsAuthWNm, false);}
  if (Lx.IsVar(PageRankWNm)){PageRankW=Lx.GetVarFlt(PageRankWNm, false);}
  if (Lx.IsVar(XNm)){X=Lx.GetVarFlt(XNm, false);}
  if (Lx.IsVar(YNm)){Y=Lx.GetVarFlt(YNm, false);}
  if (Lx.IsVar(ZNm)){Z=Lx.GetVarFlt(ZNm, false);}
  if (Lx.IsVar(PpNm)){
    Lx.GetIdStr(PpNm); Lx.GetSym(syColon); Pp=TPp::LoadTxt(Lx);}
  PVrtx Vrtx=PVrtx(new TGVrtx(VId, VNm, Pp));
  Vrtx->PutMark(Mark); Vrtx->PutWgt(Wgt);
  Vrtx->PutHitsHubW(HitsHubW); Vrtx->PutHitsAuthW(HitsAuthW);
  Vrtx->PutPageRankW(PageRankW);
  Vrtx->PutX(X); Vrtx->PutY(Y); Vrtx->PutZ(Z);
  Lx.GetSym(syRBracket);
  return Vrtx;
}

void TVrtx::SaveTxt(TOLx& Lx) const {
  Lx.PutSym(syLBracket);
  Lx.PutVarInt(VIdNm, GetVId(), false);
  TStr XmlVNm=TXmlLx::GetXmlStrFromPlainStr(GetVNm());
  if (!GetVNm().Empty()){Lx.PutVarStr(VNmNm, XmlVNm, false);}
  if (GetMark()!=0){Lx.PutVarInt(MarkNm, GetMark(), false);}
  if (GetWgt()!=0){Lx.PutVarFlt(WgtNm, GetWgt(), false);}
  if (GetHitsHubW()!=0){Lx.PutVarFlt(HitsHubWNm, GetHitsHubW(), false);}
  if (GetHitsAuthW()!=0){Lx.PutVarFlt(HitsAuthWNm, GetHitsAuthW(), false);}
  if (GetPageRankW()!=0){Lx.PutVarFlt(PageRankWNm, GetPageRankW(), false);}
  if (GetX()!=0){Lx.PutVarFlt(XNm, GetX(), false);}
  if (GetY()!=0){Lx.PutVarFlt(YNm, GetY(), false);}
  if (GetZ()!=0){Lx.PutVarFlt(ZNm, GetZ(), false);}
  if (!GetPp().Empty()){
    Lx.PutIdStr(PpNm); Lx.PutSym(syColon); GetPp()->SaveTxt(Lx);}
  Lx.PutSym(syRBracket);
}

/////////////////////////////////////////////////
// General-Vrtx
void TGVrtx::DefVrtx(const PGraph& Graph){
  if (VId==-1){VId=Graph->GetNewVId();}
  Graph->DefVId(VId);
}

/////////////////////////////////////////////////
// Edge
TStr TEdge::EIdNm="EId";
TStr TEdge::VId1Nm="VId1";
TStr TEdge::VId2Nm="VId2";
TStr TEdge::PpNm="Pp";
TStr TEdge::DirPNm="Dir";
TStr TEdge::ENmNm="ENm";
TStr TEdge::MarkNm="Mark";
TStr TEdge::WgtNm="Wgt";
TStr TEdge::WidthNm="Width";
TStr TEdge::DNmNm="DNm";
TStr TEdge::BriefNm="Brief";
TStr TEdge::RectNm="Rect";
TStr TEdge::RectX1Nm="Rect|X1"; TStr TEdge::X1Nm="X1";
TStr TEdge::RectY1Nm="Rect|Y1"; TStr TEdge::Y1Nm="Y1";
TStr TEdge::RectX2Nm="Rect|X2"; TStr TEdge::X2Nm="X2";
TStr TEdge::RectY2Nm="Rect|Y2"; TStr TEdge::Y2Nm="Y2";
TStr TEdge::DmNm="Dm";

void TEdge::PutRect(const TGksRect& Rect){
  GetPp()->PutValInt(RectX1Nm, Rect.GetX1());
  GetPp()->PutValInt(RectY1Nm, Rect.GetY1());
  GetPp()->PutValInt(RectX2Nm, Rect.GetX2());
  GetPp()->PutValInt(RectY2Nm, Rect.GetY2());
}

TGksRect TEdge::GetRect() const {
  int X1=GetPp()->GetValInt(RectX1Nm);
  int Y1=GetPp()->GetValInt(RectY1Nm);
  int X2=GetPp()->GetValInt(RectX2Nm);
  int Y2=GetPp()->GetValInt(RectY2Nm);
  return TGksRect(X1, Y1, X2, Y2);
}

bool TEdge::IsInRect(const double& X, const double& Y) const {
  TGksRect Rect=TEdge::GetRect();
  return Rect.IsIn(int(X), int(Y));
}

PPp TEdge::GetDrawPp(){
  PPp Pp=TPp::GetSetPp("Draw");
  Pp->AddPpStr(DNmNm, "", "");
  Pp->AddPpInt(BriefNm, "", TInt::Mn, TInt::Mx, 0);
  PPp RectPp=TPp::GetSetPp(RectNm);
  RectPp->AddPpInt(X1Nm, "", TInt::Mn, TInt::Mx, 0);
  RectPp->AddPpInt(Y1Nm, "", TInt::Mn, TInt::Mx, 0);
  RectPp->AddPpInt(X2Nm, "", TInt::Mn, TInt::Mx, 0);
  RectPp->AddPpInt(Y2Nm, "", TInt::Mn, TInt::Mx, 0);
  Pp->AddPp(RectPp);
  return Pp;
}

PEdge TEdge::LoadXml(const PXmlTok& XmlTok){
  // collect attributes
  int EId=XmlTok->GetIntArgVal(EIdNm);
  int VId1=XmlTok->GetIntArgVal(VId1Nm);
  int VId2=XmlTok->GetIntArgVal(VId2Nm);
  bool DirP=XmlTok->GetBoolArgVal(DirPNm, true);
  TStr ENm=XmlTok->GetStrArgVal(ENmNm, "");
  int Mark=XmlTok->GetIntArgVal(MarkNm, 0);
  double Wgt=XmlTok->GetFltArgVal(WgtNm, 0);
  int Width=int(XmlTok->GetFltArgVal(WidthNm, 0));
  PPp Pp;
  // create edge
  PEdge Edge=PEdge(new TGEdge(EId, VId1, VId2, DirP, ENm, Pp));
  Edge->PutMark(Mark);
  Edge->PutWgt(Wgt);
  Edge->PutWidth(Width);
  return Edge;
}

PXmlTok TEdge::GetXmlTok() const {
  PXmlTok EdgeTok=TXmlTok::New(TGraph::EdgeNm);
  EdgeTok->AddArg(EIdNm, GetEId());
  EdgeTok->AddArg(VId1Nm, GetVId1());
  EdgeTok->AddArg(VId2Nm, GetVId2());
  if (!IsDir()){EdgeTok->AddArg(DirPNm, IsDir());}
  if (!GetENm().Empty()){EdgeTok->AddArg(ENmNm, GetENm());}
  if (GetMark()!=0){EdgeTok->AddArg(MarkNm, GetMark());}
  if (GetWgt()!=0){EdgeTok->AddArg(WgtNm, GetWgt());}
  if (GetWidth()!=0){EdgeTok->AddArg(WidthNm, GetWidth());}
  return EdgeTok;
}

PEdge TEdge::LoadTxt(TILx& Lx){
  Lx.GetSym(syLBracket);
  int EId=Lx.GetVarInt(EIdNm, false);
  int VId1=Lx.GetVarInt(VId1Nm, false);
  int VId2=Lx.GetVarInt(VId2Nm, false);
  bool DirP=true; TStr ENm; int Mark=0; double Wgt=0; int Width=0; PPp Pp;
  if (Lx.IsVar(DirPNm)){DirP=Lx.GetVarBool(DirPNm, false);}
  if (Lx.IsVar(ENmNm)){ENm=Lx.GetVarStr(ENmNm, false);}
  ENm=TXmlLx::GetPlainStrFromXmlStr(ENm);
  if (Lx.IsVar(MarkNm)){Mark=Lx.GetVarInt(MarkNm, false);}
  if (Lx.IsVar(WgtNm)){Wgt=Lx.GetVarFlt(WgtNm, false);}
  if (Lx.IsVar(WidthNm)){Width=Lx.GetVarInt(WidthNm, false);}
  if (Lx.IsVar(PpNm)){
    Lx.GetIdStr(PpNm); Lx.GetSym(syColon); Pp=TPp::LoadTxt(Lx);}
  PEdge Edge=PEdge(new TGEdge(EId, VId1, VId2, DirP, ENm, Pp));
  Edge->PutMark(Mark); Edge->PutWgt(Wgt); Edge->PutWidth(Width);
  Lx.GetSym(syRBracket);
  return Edge;
}

void TEdge::SaveTxt(TOLx& Lx) const {
  Lx.PutSym(syLBracket);
  Lx.PutVarInt(EIdNm, GetEId(), false);
  Lx.PutVarInt(VId1Nm, GetVId1(), false);
  Lx.PutVarInt(VId2Nm, GetVId2(), false);
  if (!IsDir()){Lx.PutVarBool(DirPNm, IsDir(), false);}
  TStr XmlENm=TXmlLx::GetXmlStrFromPlainStr(GetENm());
  if (!GetENm().Empty()){Lx.PutVarStr(ENmNm, XmlENm(), false);}
  if (GetMark()!=0){Lx.PutVarInt(MarkNm, GetMark(), false);}
  if (GetWgt()!=0){Lx.PutVarFlt(WgtNm, GetWgt(), false);}
  if (GetWidth()!=0){Lx.PutVarInt(WidthNm, GetWidth(), false);}
  if (!GetPp().Empty()){GetPp()->SaveTxt(Lx);}
  Lx.PutSym(syRBracket);
}

/////////////////////////////////////////////////
// General-Edge
TGEdge::TGEdge(const int& _EId, const int& _VId1, const int& _VId2,
 const bool& _DirP, const TStr& _ENm, const PPp& _Pp):
  TEdge(), EId(_EId), VId1(_VId1), VId2(_VId2),
  DirP(_DirP), ENm(_ENm), DNm(TStr()), Mark(0), Wgt(0), Width(0), Pp(_Pp){
}

TGEdge::TGEdge(const PVrtx& Vrtx1, const PVrtx& Vrtx2,
 const TStr& _ENm, const bool& _DirP, const PPp& _Pp):
  TEdge(), EId(-1), VId1(Vrtx1->GetVId()), VId2(Vrtx2->GetVId()),
  DirP(_DirP), ENm(_ENm), DNm(TStr()), Mark(0), Wgt(0), Width(0), Pp(_Pp){
}

void TGEdge::DefEdge(const PGraph& Graph){
  if (EId==-1){EId=Graph->GetNewEId();}
  Graph->DefEId(EId);
}

/////////////////////////////////////////////////
// Graph

// constant strings
TStr TGraph::PpNm="Pp";
TStr TGraph::VrtxNm="Vrtx";
TStr TGraph::EdgeNm="Edge";
TStr TGraph::GNmNm="GNm";
TStr TGraph::DNmNm="DNm";
TStr TGraph::BriefNm="Brief";

// file extension
const TStr TGraph::FExt=".Graph";
const TStr TGraph::HitsFExt=".Hits";
const TStr TGraph::PageRankFExt=".PageRank";

// registration of extended types
THash<TStr, TGraph::TGraphNewF> TGraph::TypeToNewFH(10);
THash<TStr, TGraph::TGraphLoadF> TGraph::TypeToLoadFH(10);

PGraph TGraph::New(const TStr& TypeNm){
  TStr FinalTypeNm=TypeNm;
  if (TypeNm.Empty()){FinalTypeNm=TTypeNm<TGGraph>();}
  TGraphNewF NewF=TypeToNewFH.GetDat(FinalTypeNm);
  return (*NewF())();
}

PGraph TGraph::Load(TSIn& SIn){
  TStr TypeNm(SIn);
  TGraphLoadF LoadF=TypeToLoadFH.GetDat(TypeNm);
  return (*LoadF())(SIn);
}

bool TGraph::Reg(
 const TStr& TypeNm, const TGraphNewF& NewF, const TGraphLoadF& LoadF){
  IAssert(!TypeToNewFH.IsKey(TypeNm));
  IAssert(!TypeToLoadFH.IsKey(TypeNm));
  TypeToNewFH.AddDat(TypeNm, NewF);
  TypeToLoadFH.AddDat(TypeNm, LoadF);
  return true;
}

void TGraph::_GetMnMxXY(
 double& MnX, double& MnY, double& MxX, double& MxY) const {
  // determine min. & max. coordinates
  MnX=0; MxX=0; MnY=0; MxY=0;
  int VrtxP=FFirstVrtx(); PVrtx Vrtx; bool First=true;
  while (FNextVrtx(VrtxP, Vrtx)){
    if (First){
      MnX=Vrtx->GetX(); MxX=Vrtx->GetX();
      MnY=Vrtx->GetY(); MxY=Vrtx->GetY();
      First=false;
    } else {
      if (Vrtx->GetX()<MnX){MnX=Vrtx->GetX();}
      if (Vrtx->GetX()>MxX){MxX=Vrtx->GetX();}
      if (Vrtx->GetY()<MnY){MnY=Vrtx->GetY();}
      if (Vrtx->GetY()>MxY){MxY=Vrtx->GetY();}
    }
  }
}

void TGraph::_AddBorders(const double& BorderFac,
 double& MnX, double& MnY, double& MxX, double& MxY) const {
  // add borders
  if (MxX-MnX==0){MnX-=BorderFac; MxX+=BorderFac;}
  else {MnX-=(MxX-MnX)*BorderFac; MxX+=(MxX-MnX)*BorderFac;}
  if (MxY-MnY==0){MnY-=BorderFac; MxY+=BorderFac;}
  else {MnY-=(MxY-MnY)*BorderFac; MxY+=(MxY-MnY)*BorderFac;}
}

void TGraph::RescaleXY(const double& BorderFac, const PVrtx& CenterVrtx) const {
  // determine min. & max. coordinates
  double MnX; double MnY; double MxX; double MxY;
  _GetMnMxXY(MnX, MnY, MxX, MxY);
  // use center vertex
  if (!CenterVrtx.Empty()){
    double CenterX=CenterVrtx->GetX();
    double CenterY=CenterVrtx->GetY();
    if (MxX-CenterX>CenterX-MnX){MnX=CenterX-(MxX-CenterX);}
    else {MxX=CenterX+(CenterX-MnX);}
    if (MxY-CenterY>CenterY-MnY){MnY=CenterY-(MxY-CenterY);}
    else {MxY=CenterY+(CenterY-MnY);}
  }
  // add borders
  _AddBorders(BorderFac, MnX, MnY, MxX, MxY);
  // rescale coordinates
  int VrtxP=FFirstVrtx(); PVrtx Vrtx;
  while (FNextVrtx(VrtxP, Vrtx)){
    Vrtx->PutX((Vrtx->GetX()-MnX)/(MxX-MnX));
    Vrtx->PutY((Vrtx->GetY()-MnY)/(MxY-MnY));
  }
}

void TGraph::RescaleXY(const double& MnX, const double& MnY,
 const double& MxX, const double& MxY) const {
  if ((fabs(MxX-MnX)<0.01)||(fabs(MxY-MnY)<0.01)){return;}
  // rescale coordinates
  int VrtxP=FFirstVrtx(); PVrtx Vrtx;
  while (FNextVrtx(VrtxP, Vrtx)){
    Vrtx->PutX((Vrtx->GetX()-MnX)/(MxX-MnX));
    Vrtx->PutY((Vrtx->GetY()-MnY)/(MxY-MnY));
  }
}

int TGraph::_IsEdgeXYInts(
 const PVrtx& Vrtx0, const PVrtx& Vrtx1, const PVrtx& Vrtx2) const {
  double dx1=Vrtx1->GetX()-Vrtx0->GetX();
  double dy1=Vrtx1->GetY()-Vrtx0->GetY();
  double dx2=Vrtx2->GetX()-Vrtx0->GetX();
  double dy2=Vrtx2->GetY()-Vrtx0->GetY();
  if (dx1*dy2>dy1*dx2){return +1;}
  if (dx1*dy2<dy1*dx2){return -1;}
  if ((dx1*dx2<0)||(dy1*dy2<0)){return -1;}
  if ((dx1*dx1+dy1*dy1)<(dx2*dx2+dy2*dy2)){return +1;}
  return 0;
}

bool TGraph::IsEdgeXYInts(const PEdge& Edge1, const PEdge& Edge2) const {
  PVrtx V11=GetVrtx(Edge1->GetVId1());
  PVrtx V12=GetVrtx(Edge1->GetVId2());
  PVrtx V21=GetVrtx(Edge2->GetVId1());
  PVrtx V22=GetVrtx(Edge2->GetVId2());
  return
   ((_IsEdgeXYInts(V11, V12, V21)*_IsEdgeXYInts(V11, V12, V22))<=0)&&
   ((_IsEdgeXYInts(V21, V22, V11)*_IsEdgeXYInts(V21, V22, V12))<=0);
}

double TGraph::GetVrtxXYLen(const PVrtx& Vrtx1, const PVrtx& Vrtx2) const {
  double Len=
   TMath::Sqr(Vrtx2->GetX()-Vrtx1->GetX())+
   TMath::Sqr(Vrtx2->GetY()-Vrtx1->GetY());
  Len=sqrt(Len);
  return Len;
}

double TGraph::GetEdgeXYLen(const PEdge& Edge) const {
  PVrtx Vrtx1=GetVrtx(Edge->GetVId1());
  PVrtx Vrtx2=GetVrtx(Edge->GetVId2());
  return GetVrtxXYLen(Vrtx1, Vrtx2);
}

double TGraph::GetSumEdgeXYLen() const {
  double SumLen=0;
  int EdgeP=FFirstEdge(); PEdge Edge;
  while (FNextEdge(EdgeP, Edge)){
    SumLen+=GetEdgeXYLen(Edge);
  }
  return SumLen;
}

void TGraph::PlaceRndXY(TRnd& Rnd) const {
  int VrtxP=FFirstVrtx(); PVrtx Vrtx;
  while (FNextVrtx(VrtxP, Vrtx)){
    Vrtx->PutX(Rnd.GetUniDev());
    Vrtx->PutY(Rnd.GetUniDev());
  }
}

void TGraph::PlaceBestRndXY(const int& Tries, TRnd& Rnd) const {
  int BestSeed; double BestLen=-1;
  for (int TryN=0; TryN<Tries; TryN++){
    int CurSeed=Rnd.GetSeed();
    PlaceRndXY(Rnd);
    double CurLen=GetSumEdgeXYLen();
    if (TryN==0){
      BestSeed=CurSeed; BestLen=CurLen;
    } else
    if (CurLen<BestLen){
      BestSeed=CurSeed; BestLen=CurLen;
    }
  }
  Rnd.PutSeed(BestSeed);
  PlaceRndXY(Rnd);
}

double TGraph::GetSimAnnXYEnergy(
 const double& NodeDistrWgt, const double& BorderWgt,
 const double& EdgeLenWgt, const double& EdgeCrossWgt,
 const double& NodeEdgeDistWgt, const bool& UseNodeEdgeDist) const {
  // energy calculation
  double Energy=0;
  // node distributions
  if (NodeDistrWgt!=0){
    int Vrtx1P=FFirstVrtx(); PVrtx Vrtx1;
    while (FNextVrtx(Vrtx1P, Vrtx1)){
      int Vrtx2P=FFirstVrtx(); PVrtx Vrtx2;
      while (FNextVrtx(Vrtx2P, Vrtx2)){
        double EuclDist=
         sqrt(TMath::Sqr(Vrtx1->GetX()-Vrtx2->GetX())+
         TMath::Sqr(Vrtx1->GetY()-Vrtx2->GetY()));
        if (EuclDist>0){
          Energy+=NodeDistrWgt/TMath::Sqr(EuclDist);}
      }
    }
  }
  // borderlines
  if (BorderWgt!=0){
    double Vrtxs=GetVrtxs();
    int VrtxP=FFirstVrtx(); PVrtx Vrtx;
    while (FNextVrtx(VrtxP, Vrtx)){
      double Right=1-Vrtx->GetX();
      double Left=Vrtx->GetX();
      double Top=Vrtx->GetY();
      double Bottom=1-Vrtx->GetY();
      if (Right>0){Energy+=BorderWgt*TMath::Sqr(Right)*TMath::Sqr(Vrtxs);}
      if (Left>0){Energy+=BorderWgt*TMath::Sqr(Left)*TMath::Sqr(Vrtxs);}
      if (Top>0){Energy+=BorderWgt*TMath::Sqr(Top)*TMath::Sqr(Vrtxs);}
      if (Bottom>0){Energy+=BorderWgt*TMath::Sqr(Bottom)*TMath::Sqr(Vrtxs);}
//      if (Right>0){Energy+=BorderWgt/TMath::Sqr(Right);}
//      if (Left>0){Energy+=BorderWgt/TMath::Sqr(Left);}
//      if (Top>0){Energy+=BorderWgt/TMath::Sqr(Top);}
//      if (Bottom>0){Energy+=BorderWgt/TMath::Sqr(Bottom);}
    }
  }
  // edge lenghts
  if (EdgeLenWgt!=0){
    double Vrtxs=GetVrtxs();
    int EdgeP=FFirstEdge(); PEdge Edge;
    while (FNextEdge(EdgeP, Edge)){
      Energy+=EdgeLenWgt*TMath::Sqr(GetEdgeXYLen(Edge))*TMath::Sqr(Vrtxs);
    }
  }
  // edge crossing
  if (EdgeCrossWgt!=0){
    int Edge1P=FFirstEdge(); PEdge Edge1;
    while (FNextEdge(Edge1P, Edge1)){
      int Edge2P=Edge1P; PEdge Edge2;
      while (FNextEdge(Edge2P, Edge2)){
        if (&Edge1!=&Edge2){
          if
           ((Edge1->GetVId1()!=Edge2->GetVId1())&&
           (Edge1->GetVId1()!=Edge2->GetVId2())&&
           (Edge1->GetVId2()!=Edge2->GetVId1())&&
           (Edge1->GetVId2()!=Edge2->GetVId2())){
            if (IsEdgeXYInts(Edge1, Edge2)){
              Energy+=EdgeCrossWgt;
            }
          }
        }
      }
    }
  }
  // node edge distance weight
  if ((UseNodeEdgeDist)&&(NodeEdgeDistWgt!=0)){
    int EdgeP=FFirstEdge(); PEdge Edge;
    while (FNextEdge(EdgeP, Edge)){
      double EdgeLen=GetEdgeXYLen(Edge);
      PVrtx Vrtx1=GetVrtx(Edge->GetVId1());
      PVrtx Vrtx2=GetVrtx(Edge->GetVId2());
      int VrtxP=FFirstVrtx(); PVrtx Vrtx;
      while (FNextVrtx(VrtxP, Vrtx)){
        if ((&Vrtx!=&Vrtx1)&&(&Vrtx!=&Vrtx2)){
          double VrtxLen=GetVrtxXYLen(Vrtx, Vrtx1)+GetVrtxXYLen(Vrtx, Vrtx2);
          if (EdgeLen!=VrtxLen){
            Energy+=(NodeEdgeDistWgt/fabs(EdgeLen-VrtxLen));
          }
        }
      }
    }
  }
  return Energy;
}

double TGraph::GetSimAnnXYEnergy(
 const PXmlDoc& CfgXmlDoc, const bool& UseNodeEdgeDist) const {
  // energy calculation constants
  double NodeDistrWgt=10; // node distributions weight
  double BorderWgt=20; // borderlines weight
  double EdgeLenWgt=10; // edge length weight
  double EdgeCrossWgt=1000; // edge crossing weight
  double NodeEdgeDistWgt=10; // node edge distance weight
  if (!CfgXmlDoc.Empty()){
    NodeDistrWgt=CfgXmlDoc->GetTagTokFltArgVal("Config|NodeDistrWgt", "Val", NodeDistrWgt);
    BorderWgt=CfgXmlDoc->GetTagTokFltArgVal("Config|BorderWgt", "Val", BorderWgt);
    EdgeLenWgt=CfgXmlDoc->GetTagTokFltArgVal("Config|EdgeLenWgt", "Val", EdgeLenWgt);
    EdgeCrossWgt=CfgXmlDoc->GetTagTokFltArgVal("Config|EdgeCrossWgt", "Val", EdgeCrossWgt);
    NodeEdgeDistWgt=CfgXmlDoc->GetTagTokFltArgVal("Config|NodeEdgeDistWgt", "Val", NodeEdgeDistWgt);
  }

  return GetSimAnnXYEnergy(NodeDistrWgt, BorderWgt, EdgeLenWgt, EdgeCrossWgt,
   NodeEdgeDistWgt, UseNodeEdgeDist);
}

void TGraph::PlaceSimAnnXY(
 TRnd& Rnd, const PGks& Gks, const PXmlDoc& CfgXmlDoc, const PNotify& Notify,
 const int& _TempStages, const int& _TempStageTrys, const int& Secs) const {
  // general constants
  int Trys=1; // trys of simulated annealing
  double StartTemp=1000; // initial temperature
  double TempFact=0.95; // temp. geometric sequence factor (<1)
  int TempStages=_TempStages; // temp. geometric sequence length
  int TempStageTrys=_TempStageTrys; // trys per temp. stage
  int FineTuningTrys=3000; // fine tuning
  if (!CfgXmlDoc.Empty()){
    Trys=CfgXmlDoc->GetTagTokIntArgVal("Config|Trys", "Val", Trys);
    StartTemp=CfgXmlDoc->GetTagTokFltArgVal("Config|StartTemp", "Val", StartTemp);
    TempFact=CfgXmlDoc->GetTagTokFltArgVal("Config|TempFact", "Val", TempFact);
    TempStages=CfgXmlDoc->GetTagTokIntArgVal("Config|TempStages", "Val", TempStages);
    TempStageTrys=CfgXmlDoc->GetTagTokIntArgVal("Config|TempStageTrys", "Val", TempStageTrys);
    FineTuningTrys=CfgXmlDoc->GetTagTokIntArgVal("Config|FineTuningTrys", "Val", FineTuningTrys);
  }

  // energy calculation constants
  double NodeDistrWgt=10; // node distributions weight
  double BorderWgt=25; // borderlines weight
  double EdgeLenWgt=10; // edge length weight
  double EdgeCrossWgt=1000; // edge crossing weight
  double NodeEdgeDistWgt=10; // node edge distance weight
  if (!CfgXmlDoc.Empty()){
    NodeDistrWgt=CfgXmlDoc->GetTagTokFltArgVal("Config|NodeDistrWgt", "Val", NodeDistrWgt);
    BorderWgt=CfgXmlDoc->GetTagTokFltArgVal("Config|BorderWgt", "Val", BorderWgt);
    EdgeLenWgt=CfgXmlDoc->GetTagTokFltArgVal("Config|EdgeLenWgt", "Val", EdgeLenWgt);
    EdgeCrossWgt=CfgXmlDoc->GetTagTokFltArgVal("Config|EdgeCrossWgt", "Val", EdgeCrossWgt);
    NodeEdgeDistWgt=CfgXmlDoc->GetTagTokFltArgVal("Config|NodeEdgeDistWgt", "Val", NodeEdgeDistWgt);
  }

  // simulated annealing
  TSecTm StartTm=TSecTm::GetCurTm();
  double MnEnergy=TFlt::Mx; // minimal energy of the graph
  typedef TPair<PVrtx, TFltPr> TVrtxXYPrPr;
  TVec<TVrtxXYPrPr> VrtxXYPrPrV;
  for (int TryN=0; TryN<Trys; TryN++){
    PlaceRndXY(Rnd); // set initial random configuration
    double Energy=TFlt::Mx; // energy of the graph
    double Temp=StartTemp; // set initial temperature
    double PrevEnergy=TFlt::Mx; // energy of the graph from the previous step
    for (int TempStageN=0; TempStageN<TempStages; TempStageN++){
      TNotify::OnStatus(Notify,
       TInt::GetStr(1+TempStageN)+"/"+TInt::GetStr(TempStages)+"\r");
      for (int TempStageTryN=0; TempStageTryN<TempStageTrys; TempStageTryN++){
        // choose new configuration
        PVrtx Vrtx=GetRndVrtx(Rnd); // choose random vertex
        double PrevX=Vrtx->GetX();
        double PrevY=Vrtx->GetY();
        double NewX=Rnd.GetNrmDev(Vrtx->GetX(), Temp/StartTemp, 0, 1);
        double NewY=Rnd.GetNrmDev(Vrtx->GetY(), Temp/StartTemp, 0, 1);
  //      double NewX=Vrtx->GetX()+(Rnd.GetUniDev()-0.5)*Temp/StartTemp;
  //      double NewY=Vrtx->GetY()+(Rnd.GetUniDev()-0.5)*Temp/StartTemp;
        NewX=TFlt::GetInRng(NewX, 0, 1);
        NewY=TFlt::GetInRng(NewY, 0, 1);
        Vrtx->PutX(NewX);
        Vrtx->PutY(NewY);
        // calculate energy
        Energy=GetSimAnnXYEnergy(NodeDistrWgt, BorderWgt, EdgeLenWgt,
         EdgeCrossWgt, NodeEdgeDistWgt, false);
        //Energy=GetSimAnnXYEnergy(CfgXmlDoc, false);
        if (Energy<PrevEnergy){
          PrevEnergy=Energy;  // new configuration energy
        } else {
          // new configuration is worse then the previous
          double MetrPrb=exp((PrevEnergy-Energy)/Temp);
          double MetrRnd=Rnd.GetUniDev();
          if (MetrRnd>=MetrPrb){
            Vrtx->PutX(PrevX); Vrtx->PutY(PrevY); // return to previus state
          } else {
            PrevEnergy=Energy; // new configuration energy
          }
        }
      }
      // decrease the temperature
      Temp*=TempFact; // geometric sequence
      //Temp=Temp-StartTemp/(TempStages+1); // arithmetic sequence
      if (!Gks.Empty()){
        Gks->Clr();
        Draw(Gks, true);
      }
    }
    // save if solution better than the best so far
    if (Energy<MnEnergy){
      MnEnergy=Energy;
      VrtxXYPrPrV.Gen(GetVrtxs(), 0);
      int VrtxP=FFirstVrtx(); PVrtx Vrtx;
      while (FNextVrtx(VrtxP, Vrtx)){
        TFltPr XYPr(Vrtx->GetX(), Vrtx->GetY()); // save X and Y coordinates
        TVrtxXYPrPr VrtxXYPrPr(Vrtx, XYPr); // save vertex & XY pair
        VrtxXYPrPrV.Add(VrtxXYPrPr);
      }
    }
    // time
    TSecTm CurTm=TSecTm::GetCurTm();
    if (Secs!=-1){
      if (TSecTm::GetDSecs(StartTm, CurTm)>uint(Secs)){break;}
    }
  }

  // reconstruct best solution
  for (int VrtxXYPrPrN=0; VrtxXYPrPrN<VrtxXYPrPrV.Len(); VrtxXYPrPrN++){
    PVrtx Vrtx=VrtxXYPrPrV[VrtxXYPrPrN].Val1;
    TFltPr XYPr=VrtxXYPrPrV[VrtxXYPrPrN].Val2;
    double X=XYPr.Val1; double Y=XYPr.Val2;
    Vrtx->PutX(X); Vrtx->PutY(Y);
  }

  // fine tuning
  double PrevEnergy=TFlt::Mx; // energy of the graph from the previous step
  for (int FineTuningTryN=0; FineTuningTryN<FineTuningTrys; FineTuningTryN++){
    // choose new configuration
    PVrtx Vrtx=GetRndVrtx(Rnd); // choose random vertex
    double PrevX=Vrtx->GetX();
    double PrevY=Vrtx->GetY();
    double NewX=Rnd.GetNrmDev(Vrtx->GetX(), 0.001, 0, 1);
    double NewY=Rnd.GetNrmDev(Vrtx->GetY(), 0.001, 0, 1);
    Vrtx->PutX(NewX);
    Vrtx->PutY(NewY);
    // calculate energy
    double Energy=GetSimAnnXYEnergy(NodeDistrWgt, BorderWgt, EdgeLenWgt,
     EdgeCrossWgt, NodeEdgeDistWgt, true);
    //double Energy=GetSimAnnXYEnergy(CfgXmlDoc, true);
    if (Energy<PrevEnergy){
      PrevEnergy=Energy;  // new configuration energy
    } else {
      Vrtx->PutX(PrevX); Vrtx->PutY(PrevY); // return to previus state
    }
    if (FineTuningTryN%100==0){Draw(Gks, true);}
    // time
    TSecTm CurTm=TSecTm::GetCurTm();
    if (Secs!=-1){
      if (TSecTm::GetDSecs(StartTm, CurTm)>uint(Secs)){break;}
    }
  }
}

void TGraph::PlaceFineTuning(
 TRnd& Rnd, const PGks& Gks, const PNotify& Notify, const int& Secs) const {
  // general constants
  int FineTuningTrys=3000; // fine tuning

  // energy calculation constants
  double NodeDistrWgt=10; // node distributions weight
  double BorderWgt=25; // borderlines weight
  double EdgeLenWgt=10; // edge length weight
  double EdgeCrossWgt=1000; // edge crossing weight
  double NodeEdgeDistWgt=10; // node edge distance weight

  // fine tuning
  TSecTm StartTm=TSecTm::GetCurTm();
  double PrevEnergy=TFlt::Mx; // energy of the graph from the previous step
  for (int FineTuningTryN=0; FineTuningTryN<FineTuningTrys; FineTuningTryN++){
    // choose new configuration
    PVrtx Vrtx=GetRndVrtx(Rnd); // choose random vertex
    double PrevX=Vrtx->GetX();
    double PrevY=Vrtx->GetY();
    double NewX=Rnd.GetNrmDev(Vrtx->GetX(), 0.001, 0, 1);
    double NewY=Rnd.GetNrmDev(Vrtx->GetY(), 0.001, 0, 1);
    Vrtx->PutX(NewX);
    Vrtx->PutY(NewY);
    // calculate energy
    double Energy=GetSimAnnXYEnergy(NodeDistrWgt, BorderWgt, EdgeLenWgt,
     EdgeCrossWgt, NodeEdgeDistWgt, true);
    //double Energy=GetSimAnnXYEnergy(CfgXmlDoc, true);
    if (Energy<PrevEnergy){
      PrevEnergy=Energy;  // new configuration energy
    } else {
      Vrtx->PutX(PrevX); Vrtx->PutY(PrevY); // return to previus state
    }
    if (FineTuningTryN%100==0){Draw(Gks, true);}
    // time
    TSecTm CurTm=TSecTm::GetCurTm();
    if (Secs!=-1){
      if (TSecTm::GetDSecs(StartTm, CurTm)>uint(Secs)){break;}
    }
  }
}

void TGraph::PlaceCircle() const {
  double Vrtxs=GetVrtxs();
  double AngStep=0; if (Vrtxs>0){AngStep=2*TMath::Pi/Vrtxs;}
  double Ang=0;
  int VrtxP=FFirstVrtx(); PVrtx Vrtx;
  while (FNextVrtx(VrtxP, Vrtx)){
    TGksPPoint PPoint(1, Ang);
    TGksVPoint VPoint=PPoint.GetVPoint();
    Vrtx->PutXY(VPoint.X, VPoint.Y);
    Ang+=AngStep;
  }
}

void TGraph::PlaceTreeAsStar(
 const PVrtx& HomeVrtx, const int& Lev,
 const double& HomeX, const double& HomeY,
 const double& MnAng, const double& MxAng) const {
  HomeVrtx->PutXY(HomeX, HomeY);
  // set angle step & angle variable
  if (HomeVrtx->GetOutEIds()>0){
    double HalfAngStep=((MxAng-MnAng)/HomeVrtx->GetOutEIds())/2;
    double Ang=MnAng;
    for (int EIdN=0; EIdN<HomeVrtx->GetOutEIds(); EIdN++){
      // get source edge & vertex
      PEdge Edge=GetEdge(HomeVrtx->GetOutEId(EIdN));
      PVrtx NextVrtx=GetVrtx(Edge->GetOtherVId(HomeVrtx));
      // calculate coordinates of destination vertex
      Ang+=HalfAngStep;
      TGksPPoint PPoint(Lev+1, Ang);
      TGksVPoint VPoint=PPoint.GetVPoint();
      // recursively traverse & build graph
      PlaceTreeAsStar(NextVrtx, Lev+1,
       VPoint.X, VPoint.Y, Ang-HalfAngStep, Ang+HalfAngStep);
      Ang+=HalfAngStep;
    }
  }
}

void TGraph::PlaceTreeAsStar() const {
  PVrtx RootVrtx;
  IAssert(IsTree(RootVrtx));
  PlaceTreeAsStar(RootVrtx, 0, 0, 0, 0, 2*TMath::Pi);
}

void TGraph::PlaceGraphAsStar(const PVrtx& HomeVrtx, const TStr& DrawENm) const {
    // first we clear the marks on all the vertices
    ClrVrtxMarks();
    // vector of all the vertices in the current level
    // together with it's angle and MnAng and MxAng
    TVec<TPair<PVrtx,TFltPr> > LevVrtxV;
    LevVrtxV.Add(TPair<PVrtx,TFltPr>(HomeVrtx, TFltPr(0.0, 2*TMath::Pi)));
    HomeVrtx->SetMark();

    int Level = 0;
    while (!LevVrtxV.Empty()) {
        // position and mark the vertices in the current level
        for (int VrtxN = 0; VrtxN < LevVrtxV.Len(); VrtxN++) {
            const double MnAng = LevVrtxV[VrtxN].Val2.Val1;
            const double MxAng = LevVrtxV[VrtxN].Val2.Val2;
            const double Ang = MnAng + ((MxAng - MnAng)/2);
            PVrtx Vrtx = LevVrtxV[VrtxN].Val1;
            TGksPPoint PPoint(Level, Ang);
            TGksVPoint VPoint=PPoint.GetVPoint();
            Vrtx->PutXY(VPoint.X, VPoint.Y);
        }
        // make a list of vertices for the next level
        TVec<TPair<PVrtx,TFltPr> > NextLevVrtxV;
        for (int VrtxN = 0; VrtxN < LevVrtxV.Len(); VrtxN++) {
            const double MnAng = LevVrtxV[VrtxN].Val2.Val1;
            const double MxAng = LevVrtxV[VrtxN].Val2.Val2;
            PVrtx Vrtx = LevVrtxV[VrtxN].Val1;
            // prepare list of vertices to which we can go from here
            TVec<PVrtx> NextVrtxV;
            const int EIds = Vrtx->GetInEIds();
            for (int EIdN = 0; EIdN < EIds; EIdN++) {
                PEdge Edge = GetEdge(Vrtx->GetInEId(EIdN));
                PVrtx NextVrtx = GetVrtx(Edge->GetOtherVId(Vrtx));
                if ((!NextVrtx->IsMark()) && 
                    (DrawENm.Empty() || (Edge->GetENm() == DrawENm))) {
                        NextVrtxV.Add(NextVrtx);
                }
            }
            // add them to the list of next level vertices
            if (!NextVrtxV.Empty()) {
                // set angle step & angle variable
                double AngStep=(MxAng - MnAng)/NextVrtxV.Len();
                double Ang=MnAng;
                // position vertices
                for (int VIdN=0; VIdN < NextVrtxV.Len(); VIdN++){
                    PVrtx NextVrtx=NextVrtxV[VIdN];
                    const double NextMnAng = Ang;
                    const double NextMxAng = Ang + AngStep;
                    NextLevVrtxV.Add(TPair<PVrtx,TFltPr>(NextVrtx, TFltPr(NextMnAng, NextMxAng)));
                    NextVrtx->SetMark();
                    Ang += AngStep;
                }
            }
        }
        // go to the next level
        Level++; LevVrtxV = NextLevVrtxV;
    }
}

void TGraph::SetEdgeWidth(const int& Intvs) const {
  IAssert(Intvs>0);
  // get weight value extend
  double MnWgt=TFlt::Mx; double MxWgt=TFlt::Mn;
  {int EdgeP=FFirstEdge(); PEdge Edge;
  while (FNextEdge(EdgeP, Edge)){
    double Wgt=Edge->GetWgt();
    MnWgt=TFlt::GetMn(MnWgt, Wgt);
    MxWgt=TFlt::GetMx(MxWgt, Wgt);
  }}
  //   set weights
  {int EdgeP=FFirstEdge(); PEdge Edge;
  while (FNextEdge(EdgeP, Edge)){
    double Wgt=Edge->GetWgt();
    int Width=1;
    if (MxWgt>MnWgt){
      Width=int(1+Intvs*((Wgt-MnWgt)/(MxWgt-MnWgt)));}
    if (Width>Intvs){Width=Intvs;} // if Wgt==MxWgt
    Edge->PutWidth(Width);
  }}
}

void TGraph::Draw(const PGks& Gks, const bool& SimpleP,
 const bool& DrawEdgesP, const int& FontSize,
 const int& EdgeFontSize, const int& MxLnLen) const {
  if (Gks.Empty()){return;}
  // define colors
  TGksColor EdgeLineColor=TGksColor(0, 128, 0);
  TGksColor EdgeBoxColor=TGksColor(0, 128, 0);
  TGksColor NodeBoxColor=TGksColor(0, 0, 255);
  TGksColor SelNodeBoxColor=TGksColor(255, 0, 0);
  // clear screen
  Gks->Clr();
  // prepare pen
  Gks->SetPen(TGksPen::New(EdgeLineColor, gpmCopy, gpsSolid, 1));
  // prepare brush
  Gks->SetBrush(TGksBrush::New(TGksColor::GetWhite(), gbsSolid));
  // prepare font
  Gks->SetFont(TGksFont::New("Tahoma", EdgeFontSize, TGksColor::GetBlack()));
  // draw edges
  if (DrawEdgesP){
    // draw lines
    {int PrevWidth=1;
    int EdgeP=FFirstEdge(); PEdge Edge;
    while (FNextEdge(EdgeP, Edge)){
      PVrtx Vrtx1=GetVrtx(Edge->GetVId1());
      PVrtx Vrtx2=GetVrtx(Edge->GetVId2());
      bool DirP=Edge->IsDir();
      int X1=Gks->GetVToPX(Vrtx1->GetX());
      int Y1=Gks->GetVToPY(Vrtx1->GetY());
      int X2=Gks->GetVToPX(Vrtx2->GetX());
      int Y2=Gks->GetVToPY(Vrtx2->GetY());
      int Width=TInt::GetMx(1, Edge->GetWidth());
      if (Width!=PrevWidth){
        PrevWidth=Width;
        Gks->SetPen(TGksPen::New(TGksColor::GetGreen(), gpmCopy, gpsSolid, Width+1));
      }
      if (IsVrtxsEdge(Vrtx2->GetVId(), Vrtx1->GetVId(), true)){
        if (DirP){Gks->DirLine(X1, Y1, X2, Y2, true, false, 10, 0.15);}
        else {Gks->DirLine(X1, Y1, X2, Y2, false, false, 10, 0.15);}
      } else {
        if (DirP){Gks->DirLine(X1, Y1, X2, Y2, true, false, 10, 0.25);}
        else {Gks->DirLine(X1, Y1, X2, Y2, false, false, 10, 0.25);}
      }
    }}
    // draw line boxes
    if (EdgeFontSize>0){
      Gks->SetPen(TGksPen::New(EdgeBoxColor, gpmCopy, gpsSolid, 2));
      int EdgeP=FFirstEdge(); PEdge Edge;
      while (FNextEdge(EdgeP, Edge)){
        PVrtx Vrtx1=GetVrtx(Edge->GetVId1());
        PVrtx Vrtx2=GetVrtx(Edge->GetVId2());
        int X1=Gks->GetVToPX(Vrtx1->GetX());
        int Y1=Gks->GetVToPY(Vrtx1->GetY());
        int X2=Gks->GetVToPX(Vrtx2->GetX());
        int Y2=Gks->GetVToPY(Vrtx2->GetY());
        TStr Nm=(SimpleP) ? Edge->GetENm() : Edge->GetDNm();
        if ((!Nm.Empty())&&(Nm[0]!='_')){
          if (IsVrtxsEdge(Vrtx2->GetVId(), Vrtx1->GetVId(), true)){
            TGksRect Rect=Gks->TxtBox(gtbtRoundRect, Nm, int(X1+(X2-X1)*0.60), int(Y1+(Y2-Y1)*0.60));
          } else {
            TGksRect Rect=Gks->TxtBox(gtbtRoundRect, Nm, int(X1+(X2-X1)*0.5), int(Y1+(Y2-Y1)*0.5));
          }
        }
      }
    }
  }

  // draw vertices
  TStr BoldVNm;
  if (GetPp()->IsPp("BoldVNm")){
    BoldVNm=GetPp()->GetValStr("BoldVNm");}
  // prepare font
  Gks->SetFont(TGksFont::New("Tahoma", FontSize, TGksColor::GetBlack(), TFSet(gfsBold)));
  // prepare pen
  Gks->SetPen(TGksPen::New(TGksColor::GetBlue(), gpmCopy, gpsSolid, 1));
  if (SimpleP){
    int VrtxP=FFirstVrtx(); PVrtx Vrtx;
    while (FNextVrtx(VrtxP, Vrtx)){
      TStr VNm=Vrtx->GetVNm();
      if ((!VNm.Empty())&&(VNm[0]!='_')){
        int X=Gks->GetVToPX(Vrtx->GetX());
        int Y=Gks->GetVToPY(Vrtx->GetY());
        TGksTxtBoxType TxtBoxType=gtbtRoundRect;
        TGksColor Color=TGksColor::GetBlue();
        if (Vrtx->GetColor()!=0){
          Color.PutArgbVal(Vrtx->GetColor());}
        if (VNm==BoldVNm){
          Gks->SetPen(TGksPen::New(Color, gpmCopy, gpsSolid, 5));
        } else {
          Gks->SetPen(TGksPen::New(Color, gpmCopy, gpsSolid, 2));
        }
        if (MxLnLen!=-1){
          TChA ChA=VNm; int LnChN=0;
          for (int ChN=0; ChN<ChA.Len(); ChN++){
            if ((LnChN>MxLnLen)&&(ChA[ChN]==' ')){ChA[ChN]='\\'; LnChN=0;}
            LnChN++;
          }
          VNm=ChA;
        }
        TGksRect Rect=Gks->TxtBox(TxtBoxType, VNm, X, Y);
        Vrtx->PutRect(Rect);
      }
    }
  } else {
    int VrtxP=FFirstVrtx(); PVrtx Vrtx;
    while (FNextVrtx(VrtxP, Vrtx)){
      TStr DNm=Vrtx->GetDNm();
      int Brief=Vrtx->GetBrief();
      int X=Gks->GetVToPX(Vrtx->GetX());
      int Y=Gks->GetVToPY(Vrtx->GetY());
      TGksTxtBoxType TxtBoxType=gtbtRect;
      if (DNm==BoldVNm){
        Gks->SetPen(TGksPen::New(SelNodeBoxColor, gpmCopy, gpsSolid, 2));
      } else {
        Gks->SetPen(TGksPen::New(NodeBoxColor, gpmCopy, gpsSolid, 2));
      }
      if (Brief==0){
        TGksRect Rect=Gks->TxtBox(TxtBoxType, DNm, X, Y);
        Vrtx->PutRect(Rect);
      } else {
        if (DNm.IsChIn('#')){
          TStr TxtStr1; TStr TxtStr2;
          DNm.SplitOnCh(TxtStr1, '#', TxtStr2);
          TGksRect Rect=Gks->TxtBox(TxtBoxType, TxtStr1, X, Y);
          Vrtx->PutRect(Rect);
        } else {
          TGksRect Rect=Gks->TxtBox(TxtBoxType, DNm, X, Y);
          Vrtx->PutRect(Rect);
        }
      }
    }
  }
}

void TGraph::_DrawTxtTree(const PVrtx& Vrtx, TChAV& LnV) const {
//  TStr VNm;
//  if (Vrtx->GetVId()<20){
//    VNm=TStr("[")+TStr(char(Vrtx->GetVId()+65))+"]";
//  } else {
//    VNm=TFlt::GetStr(Vrtx->GetWgt(), "{%+4.2f}");
//  }
  //TStr VNm=TInt::GetStr(Vrtx->GetVId(), "[%2d]")+"-"+
  //TFlt::GetStr(Vrtx->GetWgt(), "{%+4.2f}");
  TStr VNm=
   TStr::GetStr(Vrtx->GetVNm(), "[%5s]-")+
   TFlt::GetStr(Vrtx->GetWgt(), "{%+4.2f}");
  if (Vrtx->NoOutEIds()){
    LnV.Add(VNm);
    LnV.Add();
  } else {
    int VrtxLnN=LnV.Len(); TIntV OutLnNV; TStrV OutENmV;
    for (int OutEIdN=0; OutEIdN<Vrtx->GetOutEIds(); OutEIdN++){
      PEdge Edge=GetEdge(Vrtx->GetOutEId(OutEIdN));
      PVrtx OutVrtx=GetVrtx(Edge->GetOutVId());
//      TStr ENm=TFlt::GetStr(Edge->GetWgt(), "{%+4.2f}"); OutENmV.Add(ENm);
      OutENmV.Add();
      OutLnNV.Add(LnV.Len());
      _DrawTxtTree(OutVrtx, LnV);
    }
    int VrtxLevChN=0;
    {for (int OutLnNN=0; OutLnNN<OutLnNV.Len(); OutLnNN++){
      int OutLnN=OutLnNV[OutLnNN];
      VrtxLevChN=TInt::GetMx(VrtxLevChN, LnV[OutLnN].Len());
    }}
    LnV[VrtxLnN].AddChTo('-', VrtxLevChN);
    LnV[VrtxLnN]+="-"; LnV[VrtxLnN]+=OutENmV[0];
    LnV[VrtxLnN]+="-+-"; LnV[VrtxLnN]+=VNm;
    {for (int OutLnNN=1; OutLnNN<OutLnNV.Len(); OutLnNN++){
      int OutLnN=OutLnNV[OutLnNN];
      LnV[OutLnN].AddChTo('-', VrtxLevChN);
      LnV[OutLnN]+="-"; LnV[OutLnN]+=OutENmV[OutLnNN];
      LnV[OutLnN]+="-+ ";
      for (int GapLnN=OutLnNV[OutLnNN-1]+1; GapLnN<OutLnN; GapLnN++){
        LnV[GapLnN].AddChTo(' ', VrtxLevChN+OutENmV[OutLnNN].Len()+1);
        LnV[GapLnN]+=" | ";
      }
    }}
  }
}

void TGraph::DrawTxtTree(const PSOut& SOut) const {
  PVrtx RootVrtx; IAssert(IsTree(RootVrtx));
  TChAV LnV;
  _DrawTxtTree(RootVrtx, LnV);
  for (int LnN=0; LnN<LnV.Len(); LnN++){
    SOut->PutStr(LnV[LnN]); SOut->PutLn();
  }
}

bool TGraph::IsVrtxAt(
 const PGks& /*Gks*/, const int& X, const int& Y, PVrtx& Vrtx) const {
  int VrtxP=FFirstVrtx();
  while (FNextVrtx(VrtxP, Vrtx)){
    if (Vrtx->IsInRect(X, Y)){return true;}}
  return false;
}

bool TGraph::IsEdgeAt(
 const PGks& /*Gks*/, const int& X, const int& Y, PEdge& Edge) const {
  int EdgeP=FFirstEdge();
  while (FNextEdge(EdgeP, Edge)){
    if (Edge->IsInRect(X, Y)){return true;}}
  return false;
}

bool TGraph::IsDir() const {
  int EdgeP=FFirstEdge(); PEdge Edge;
  while (FNextEdge(EdgeP, Edge)){
    if (!Edge->IsDir()){return false;}}
  return true;
}

bool TGraph::_IsDag(const PVrtx& Vrtx, const int& Mark) const {
  if (Vrtx->GetMark()==Mark){printf("[Bad:%d]", Vrtx->GetVId()); return false;}
  if (!Vrtx->IsMark()){
    Vrtx->PutMark(Mark);
    for (int EIdN=0; EIdN<Vrtx->GetOutEIds(); EIdN++){
      PEdge Edge=GetEdge(Vrtx->GetOutEId(EIdN));
      PVrtx Vrtx=GetVrtx(Edge->GetOutVId());
      if (!_IsDag(Vrtx, Mark)){return false;}
    }
  }
  return true;
}

bool TGraph::IsDag() const {
  if (!IsDir()){return false;}
  // depth first search with marking
  ClrVrtxMarks();
  int VrtxP=FFirstVrtx(); PVrtx Vrtx; int Mark=0;
  while (FNextVrtx(VrtxP, Vrtx)){
    if (!_IsDag(Vrtx, ++Mark)){return false;}
  }
  return true;
}

bool TGraph::IsTree(PVrtx& RootVrtx) const {
  if (!IsDir()){return false;}
  if (!IsDag()){return false;}
  // find only one root
  RootVrtx=NULL;
  int VrtxP=FFirstVrtx(); PVrtx Vrtx;
  while (FNextVrtx(VrtxP, Vrtx)){
    if (Vrtx->GetInEIds()==0){
      if (RootVrtx.Empty()){RootVrtx=Vrtx;} else {return false;}
    }
  }
  return !Vrtx.Empty();
}

void TGraph::GetLeafVIdV(const int& RootVId, TIntV& VIdV) const {
  IAssert(IsDag());
  VIdV.Clr();
  TIntQ VIdQ; VIdQ.Push(RootVId);
  do {
    int VId=VIdQ.Top(); VIdQ.Pop();
    PVrtx Vrtx=GetVrtx(VId);
    if (Vrtx->NoOutEIds()){
      VIdV.Add(VId);
    } else {
      for (int OutEIdN=0; OutEIdN<Vrtx->GetOutEIds(); OutEIdN++){
        PEdge Edge=GetEdge(Vrtx->GetOutEId(OutEIdN));
        PVrtx OutVrtx=GetVrtx(Edge->GetOutVId());
        VIdQ.Push(OutVrtx->GetVId());
      }
    }
  } while (!VIdQ.Empty());
}

void TGraph::GetHitsW() const {
  {int VrtxP=FFirstVrtx(); PVrtx Vrtx;
  while (FNextVrtx(VrtxP, Vrtx)){
    Vrtx->PutHitsHubW(1); Vrtx->PutHitsAuthW(1);}}
//  double EpsChange=1/TMath::Sqr(double(GetVrtxs()));
  double EpsChange=1/double(GetVrtxs());
  double MxChange; double SumChange=0;
  TFltV PrevHubWV(GetVrtxs()); TFltV PrevAuthWV(GetVrtxs());
  int IterN=0; printf("\n");
  do {
    MxChange=0; SumChange=0;
    // calculate authority weights
    {int VrtxP=FFirstVrtx(); PVrtx Vrtx;
    while (FNextVrtx(VrtxP, Vrtx)){
      double AuthW=0;
      for (int EIdN=0; EIdN<Vrtx->GetInEIds(); EIdN++){
        PEdge Edge=GetEdge(Vrtx->GetInEId(EIdN));
        PVrtx SrcVrtx=GetVrtx(Edge->GetInVId());
        AuthW+=SrcVrtx->GetHitsHubW();
      }
      Vrtx->PutHitsAuthW(AuthW);
    }}
    // calculate hub weights
    {int VrtxP=FFirstVrtx(); PVrtx Vrtx;
    while (FNextVrtx(VrtxP, Vrtx)){
      double HubW=0;
      for (int EIdN=0; EIdN<Vrtx->GetOutEIds(); EIdN++){
        PEdge Edge=GetEdge(Vrtx->GetOutEId(EIdN));
        PVrtx DstVrtx=GetVrtx(Edge->GetOutVId());
        HubW+=DstVrtx->GetHitsAuthW();
      }
      Vrtx->PutHitsHubW(HubW);
    }}

    // normalize authorities
    {int VrtxP=FFirstVrtx(); PVrtx Vrtx;
    while (FNextVrtx(VrtxP, Vrtx)){
      Vrtx->PutHitsAuthW(log(1+Vrtx->GetHitsAuthW()));}}

    double SumSqrAuthW=0;
    {int VrtxP=FFirstVrtx(); PVrtx Vrtx;
    while (FNextVrtx(VrtxP, Vrtx)){
      SumSqrAuthW+=TMath::Sqr(Vrtx->GetHitsAuthW());}}
    double SqrtSumSqrAuthW=sqrt(SumSqrAuthW);
    {int VrtxP=FFirstVrtx(); PVrtx Vrtx;
    while (FNextVrtx(VrtxP, Vrtx)){
      double AuthW=Vrtx->GetHitsAuthW()/SqrtSumSqrAuthW;
      Vrtx->PutHitsAuthW(AuthW);}}

    // normalize hubs
    {int VrtxP=FFirstVrtx(); PVrtx Vrtx;
    while (FNextVrtx(VrtxP, Vrtx)){
      Vrtx->PutHitsHubW(log(1+Vrtx->GetHitsHubW()));}}

    double SumSqrHubW=0;
    {int VrtxP=FFirstVrtx(); PVrtx Vrtx;
    while (FNextVrtx(VrtxP, Vrtx)){
      SumSqrHubW+=TMath::Sqr(Vrtx->GetHitsHubW());}}
    double SqrtSumSqrHubW=sqrt(SumSqrHubW);
    {int VrtxP=FFirstVrtx(); PVrtx Vrtx;
    while (FNextVrtx(VrtxP, Vrtx)){
      double HubW=Vrtx->GetHitsHubW()/SqrtSumSqrHubW;
      Vrtx->PutHitsHubW(HubW);}}

    // calculate change
    {int VrtxP=FFirstVrtx(); PVrtx Vrtx; int VrtxN=0;
    while (FNextVrtx(VrtxP, Vrtx)){
      double HubWChange=fabs(Vrtx->GetHitsHubW()-PrevHubWV[VrtxN]);
      double AuthWChange=fabs(Vrtx->GetHitsAuthW()-PrevAuthWV[VrtxN]);
      if (HubWChange>MxChange){MxChange=HubWChange;}
      if (AuthWChange>MxChange){MxChange=AuthWChange;}
      SumChange+=HubWChange; SumChange+=AuthWChange;
      PrevHubWV[VrtxN]=Vrtx->GetHitsHubW();
      PrevAuthWV[VrtxN]=Vrtx->GetHitsAuthW();
      VrtxN++;
    }}

/*    // generate hub & auth string
    TChA HubWVChA; TChA AuthWVChA;
    {int VrtxP=FFirstVrtx(); PVrtx Vrtx;
    while (FNextVrtx(VrtxP, Vrtx)){
      HubWVChA+=TFlt::GetStr(Vrtx->GetHitsHubW(), " %4.2f");
      AuthWVChA+=TFlt::GetStr(Vrtx->GetHitsAuthW(), " %4.2f");
    }}
    printf("%s\n%s\n", HubWVChA.CStr(), AuthWVChA.CStr());*/

    IterN++;
    printf("Iteration: %d   Max. Change: %g   Eps. Change: %g\n",
     IterN, MxChange, EpsChange);
  } while (IterN<20); // ((IterN<20)&&(MxChange>EpsChange));
}

void TGraph::GetTopHitsVrtxV(
 TVrtxV& TopHubVrtxV, TVrtxV& TopAuthVrtxV) const {
  typedef TKeyDat<TFlt, PVrtx> TWgtVrtxKd;
  TVec<TWgtVrtxKd> WgtVrtxKdV;
  TopHubVrtxV.Gen(GetVrtxs(), 0);
  TopAuthVrtxV.Gen(GetVrtxs(), 0);

  // load & sort by hub weight & save
  WgtVrtxKdV.Gen(GetVrtxs(), 0);
  {int VrtxP=FFirstVrtx(); PVrtx Vrtx;
  while (FNextVrtx(VrtxP, Vrtx)){
    WgtVrtxKdV.Add(TWgtVrtxKd(Vrtx->GetHitsHubW(), Vrtx));}}
  WgtVrtxKdV.Sort(false);
  for (int VrtxN=0; VrtxN<WgtVrtxKdV.Len(); VrtxN++){
    TopHubVrtxV.Add(WgtVrtxKdV[VrtxN].Dat);}

  // load & sort by hub weight & save
  WgtVrtxKdV.Gen(GetVrtxs(), 0);
  {int VrtxP=FFirstVrtx(); PVrtx Vrtx;
  while (FNextVrtx(VrtxP, Vrtx)){
    WgtVrtxKdV.Add(TWgtVrtxKd(Vrtx->GetHitsAuthW(), Vrtx));}}
  WgtVrtxKdV.Sort(false);
  {for (int VrtxN=0; VrtxN<WgtVrtxKdV.Len(); VrtxN++){
    TopAuthVrtxV.Add(WgtVrtxKdV[VrtxN].Dat);}}
}

void TGraph::GetConnVrtxV(PVrtx& RootVrtx, TVrtxV& ConnVrtxV) const {
  ConnVrtxV.Clr();
  ClrVrtxMarks();
  TQQueue<PVrtx> VrtxQ;
  VrtxQ.Push(RootVrtx);
  while (!VrtxQ.Empty()){
    PVrtx Vrtx=VrtxQ.Top(); VrtxQ.Pop();
    if (!Vrtx->IsMark()){
      ConnVrtxV.Add(Vrtx);
      Vrtx->PutMark(true);
      for (int EIdN=0; EIdN<Vrtx->GetOutEIds(); EIdN++){
        PEdge Edge=GetEdge(Vrtx->GetOutEId(EIdN));
        VrtxQ.Push(GetVrtx(Edge->GetVId1()));
        VrtxQ.Push(GetVrtx(Edge->GetVId2()));
      }
    }
  }
}

PGraph TGraph::GetRndGraph(
 TRnd& Rnd, const int& Vrtxs, const int& Edges,
 const bool& DirEdgeP, const int& EdgeToSameVrtxP){
  IAssert((Vrtxs>=0)&&(Edges>=0));
  PGraph Graph=New();
  for (int VrtxN=0; VrtxN<Vrtxs; VrtxN++){
    PVrtx Vrtx=PVrtx(new TGVrtx()); Graph->AddVrtx(Vrtx);}
  int EdgeN=0;
  while (Graph->GetEdges()<Edges){
    PVrtx Vrtx1=Graph->GetRndVrtx(Rnd);
    PVrtx Vrtx2=Graph->GetRndVrtx(Rnd);
    while ((!EdgeToSameVrtxP)&&(Vrtx1==Vrtx2)){
      IAssert(Vrtxs>1); Vrtx2=Graph->GetRndVrtx(Rnd);}
    PEdge Edge=PEdge(new TGEdge(Vrtx1, Vrtx2, TStr::GetNullStr(), DirEdgeP));
    Graph->AddEdge(Edge); EdgeN++;
  }
  return Graph;
}

PGraph TGraph::LoadXml(const PXmlDoc& XmlDoc, const TStr& TypeNm){
  // get xml shortcuts
  PXmlTok GraphTok=XmlDoc->GetTok();
  EAssertR(GraphTok->IsTag("Graph"), "Bad root XML tag (<Graph> expected).");
  int GraphSubToks=GraphTok->GetSubToks();

  // determine type-name
  TStr FinalTypeNm=TypeNm;
  if (FinalTypeNm.Empty()){
    FinalTypeNm=XmlDoc->GetTok()->GetArgVal("TypeNm", "");}

  // create graph
  PGraph Graph=New(FinalTypeNm);

  // add vertices
  for (int GraphTokN=0; GraphTokN<GraphSubToks; GraphTokN++){
    PXmlTok Tok=GraphTok->GetSubTok(GraphTokN);
    if (Tok->IsTag(TGraph::VrtxNm)){
      PVrtx Vrtx=TVrtx::LoadXml(Tok);
      Graph->AddVrtx(Vrtx);
    }
  }

  // add edges
  {for (int GraphTokN=0; GraphTokN<GraphSubToks; GraphTokN++){
    PXmlTok Tok=GraphTok->GetSubTok(GraphTokN);
    if (Tok->IsTag(TGraph::EdgeNm)){
      PEdge Edge=TEdge::LoadXml(Tok);
      Graph->AddEdge(Edge);
    }
  }}

  // return result
  return Graph;
}

PXmlDoc TGraph::GetXmlDoc() const {
  // create graph xml-token
  PXmlTok GraphTok=TXmlTok::New("Graph");
  GraphTok->AddArg("TypeNm", GetTypeNm());

  // add properties to graph xml-token
  for (int PpN=0; PpN<GetPps(); PpN++){
    GraphTok->AddArg(PpNm, GetPp(PpN)->GetStr());
  }
  // add vertices tokens
  int VrtxP=FFirstVrtx(); PVrtx Vrtx;
  while (FNextVrtx(VrtxP, Vrtx)){
    PXmlTok VrtxTok=Vrtx->GetXmlTok();
    GraphTok->AddSubTok(VrtxTok);
  }

  // add edges tokens
  int EdgeP=FFirstEdge(); PEdge Edge;
  while (FNextEdge(EdgeP, Edge)){
    PXmlTok EdgeTok=Edge->GetXmlTok();
    GraphTok->AddSubTok(EdgeTok);
  }

  // create & return graph xml-document
  PXmlDoc GraphDoc=TXmlDoc::New(GraphTok);
  return GraphDoc;
}

PGraph TGraph::LoadTxt(const PSIn& SIn, const TStr& TypeNm){
  TILx Lx(SIn, TFSet()|iloCmtAlw|iloSigNum);
  // determine type-name
  TStr FinalTypeNm=TypeNm;
  if (Lx.GetSym(syIdStr, syLBracket)==syIdStr){
    if (FinalTypeNm.Empty()){FinalTypeNm=Lx.Str;}
    Lx.GetSym(syColon); Lx.GetSym(syLBracket);
  }
  // construct graph
  PGraph Graph=New(FinalTypeNm);
  while (Lx.GetSym(syIdStr, syRBracket)==syIdStr){
    TStr Tag=Lx.Str;
    Lx.GetSym(syColon);
    if (Tag==PpNm){
      PPp Pp=TPp::LoadTxt(Lx);
      Graph->AddPp(Pp);
    } else
    if (Tag==VrtxNm){
      PVrtx Vrtx=TVrtx::LoadTxt(Lx);
      Graph->AddVrtx(Vrtx);
    } else
    if (Tag==EdgeNm){
      PEdge Edge=TEdge::LoadTxt(Lx);
      Graph->AddEdge(Edge);
    } else {
      Fail;
    }
  }
  return Graph;
}

void TGraph::SaveTxt(const PSOut& SOut) const {
  TOLx Lx(SOut, TFSet()|oloCmtAlw|oloFrcEoln|oloSigNum);
  Lx.PutVar(GetTypeNm(), true, true);
  // save properties
  for (int PpN=0; PpN<GetPps(); PpN++){
    Lx.PutIdStr(PpNm); Lx.PutSym(syColon);
    GetPp(PpN)->SaveTxt(Lx); Lx.PutLn();
  }
  // save vertices
  int VrtxP=FFirstVrtx(); PVrtx Vrtx;
  while (FNextVrtx(VrtxP, Vrtx)){
    Lx.PutIdStr(VrtxNm); Lx.PutSym(syColon);
    Vrtx->SaveTxt(Lx); Lx.PutLn();
  }
  // save edges
  int EdgeP=FFirstEdge(); PEdge Edge;
  while (FNextEdge(EdgeP, Edge)){
    Lx.PutIdStr(EdgeNm); Lx.PutSym(syColon);
    Edge->SaveTxt(Lx); Lx.PutLn();
  }
  Lx.PutVarEnd(true, true);
}

void TGraph::SaveTxtTab(const TStr& FNm) const {
  TFOut TabFOut(FNm); FILE* fTab=TabFOut.GetFileId();
  // save vertices
  fprintf(fTab, "VH\tVId\tName\tX\tY\n");
  int VrtxP=FFirstVrtx(); PVrtx Vrtx;
  while (FNextVrtx(VrtxP, Vrtx)){
    fprintf(fTab, "V\t%d\t%s\t%g\t%g\n",
     Vrtx->GetVId(), Vrtx->GetVNm().CStr(), Vrtx->GetX(), Vrtx->GetY());
  }
  // save edges
  fprintf(fTab, "EH\tEId\tVId1\tVId2\tDir\tName\tWidth\n");
  int EdgeP=FFirstEdge(); PEdge Edge;
  while (FNextEdge(EdgeP, Edge)){
    fprintf(fTab, "E\t%d\t%d\t%d\t%s\t%s\t%d\n",
     Edge->GetEId(), Edge->GetVId1(), Edge->GetVId2(),
     TBool::GetStr(Edge->IsDir()).CStr(), Edge->GetENm().CStr(),
     Edge->GetWidth());
  }
}

void TGraph::SaveTxtPajek(const TStr& FNm) const {
  PSOut SOut=TFOut::New(FNm);
  // save vertices
  SOut->PutStr("*vertices ");
  int Vrtxs=GetVrtxs();
  SOut->PutStr(TInt::GetStr(Vrtxs));
//  SOut->PutStr(TInt::GetStr(GetVrtxs()));
  SOut->PutDosLn();
  TIntIntH VIdToVrtxNH(Vrtxs);
  int VrtxP=FFirstVrtx(); PVrtx Vrtx; int VrtxN=0;
  while (FNextVrtx(VrtxP, Vrtx)){
    VrtxN++;
    VIdToVrtxNH.AddDat(Vrtx->GetVId(), VrtxN);
    SOut->PutStr(TInt::GetStr(VrtxN));
    SOut->PutStr(TStr(" \"")+Vrtx->GetVNm()+"\"");
    SOut->PutDosLn();
  }
  // save edges
  SOut->PutStr("*arcs"); SOut->PutDosLn();
  int EdgeP=FFirstEdge(); PEdge Edge;
  while (FNextEdge(EdgeP, Edge)){
    int VId1=Edge->GetVId1(); int VrtxN1=VIdToVrtxNH.GetDat(VId1);
    int VId2=Edge->GetVId2(); int VrtxN2=VIdToVrtxNH.GetDat(VId2);
    SOut->PutStr(TInt::GetStr(VrtxN1)+" "+TInt::GetStr(VrtxN2));
    SOut->PutDosLn();
  }
}

void TGraph::SaveTxtMatlab(const TStr& FNm) const {
  // save vertices
  PSOut VrtxSOut=TFOut::New(TStr::PutFExt(FNm, ".vrtx-names.dat"));
  int Vrtxs=GetVrtxs(); TIntIntH VIdToVrtxNH(Vrtxs);
  int VrtxP=FFirstVrtx(); PVrtx Vrtx; int VrtxN=0;
  while (FNextVrtx(VrtxP, Vrtx)){
    VrtxN++;
    VIdToVrtxNH.AddDat(Vrtx->GetVId(), VrtxN);
    VrtxSOut->PutStrLn(Vrtx->GetVNm());
  }
  // save edges
  PSOut EdgeSOut=TFOut::New(FNm);
  int EdgeP=FFirstEdge(); PEdge Edge;
  while (FNextEdge(EdgeP, Edge)){
    int VId1=Edge->GetVId1(); int VrtxN1=VIdToVrtxNH.GetDat(VId1);
    int VId2=Edge->GetVId2(); int VrtxN2=VIdToVrtxNH.GetDat(VId2);
    const double Wgt = Edge->GetWgt();
    EdgeSOut->PutStrLn(TStr::Fmt("%d %d %g", 
        TInt::GetMn(VrtxN1, VrtxN2), TInt::GetMx(VrtxN1, VrtxN2), Wgt));
  }
}

void TGraph::SaveTxtHits(const TStr& FNm, const int& TopN) const {
  // generate vectors for top hub & authority vertices
  TVrtxV HubVrtxV; TVrtxV AuthVrtxV;
  GetTopHitsVrtxV(HubVrtxV, AuthVrtxV);

  PSOut SOut=TFOut::New(FNm);
  // save hub vertices
  int HubVrtxs=(TopN==-1) ? HubVrtxV.Len() : TInt::GetMn(TopN, HubVrtxV.Len());
  SOut->PutStr("---Hubs---------------------------"); SOut->PutLn();
  for (int VrtxN=0; VrtxN<HubVrtxs; VrtxN++){
    PVrtx Vrtx=HubVrtxV[VrtxN];
    TStr RankStr=TInt::GetStr(1+VrtxN, "%d.");
    TStr WgtStr=TFlt::GetStr(Vrtx->GetHitsHubW(), "%0.4g");
    TStr InStr=TStr("in:")+TInt::GetStr(Vrtx->GetInEIds(), "%2d");
    TStr OutStr=TStr("out:")+TInt::GetStr(Vrtx->GetOutEIds(), "%2d");
    TStr Str=RankStr+" "+WgtStr+" -> "+InStr+" "+OutStr+" \""+Vrtx->GetVNm()+"\"";
    SOut->PutStr(Str); SOut->PutLn();
  }
  // save authority vertices
  int AuthVrtxs=(TopN==-1) ? AuthVrtxV.Len() : TInt::GetMn(TopN, AuthVrtxV.Len());
  SOut->PutStr("---Authorities--------------------"); SOut->PutLn();
  {for (int VrtxN=0; VrtxN<AuthVrtxs; VrtxN++){
    PVrtx Vrtx=AuthVrtxV[VrtxN];
    TStr RankStr=TInt::GetStr(1+VrtxN, "%d. ");
    TStr WgtStr=TFlt::GetStr(Vrtx->GetHitsAuthW(), "%0.4g");
    TStr InStr=TStr("in:")+TInt::GetStr(Vrtx->GetInEIds(), "%2d");
    TStr OutStr=TStr("out:")+TInt::GetStr(Vrtx->GetOutEIds(), "%2d");
    TStr Str=RankStr+" "+WgtStr+" -> "+InStr+" "+OutStr+" \""+Vrtx->GetVNm()+"\"";
    SOut->PutStr(Str); SOut->PutLn();
  }}
}

void TGraph::_SaveTxtSparseNbrs(const int& SrcVId,
 const int& Lev, const int& MxLev, TIntFltH& VIdToWgtH) const {
  if (Lev<=MxLev){
    PVrtx SrcVrtx=GetVrtx(SrcVId);
    for (int EIdN=0; EIdN<SrcVrtx->GetOutEIds(); EIdN++){
      PEdge Edge=GetEdge(SrcVrtx->GetOutEId(EIdN));
      int DstVId=Edge->GetOtherVId(SrcVrtx);
      VIdToWgtH.AddDat(DstVId)+=1.0/(Lev+1);
      _SaveTxtSparseNbrs(DstVId, Lev+1, MxLev, VIdToWgtH);
    }
  }
}

void TGraph::SaveTxtSparseNbrs(
 const TStr& VarIdFNm, const TStr& TupIdFNm, const TStr& TupVarValFNm,
 const int& MxLev) const {
  // variable/tuple id file
  {PSOut VarIdSOut=TFOut::New(VarIdFNm);
  PSOut TupIdSOut=TFOut::New(TupIdFNm);
  PSOut TupVarValSOut=TFOut::New(TupVarValFNm);
  int VrtxP=FFirstVrtx(); PVrtx Vrtx;
  while (FNextVrtx(VrtxP, Vrtx)){
    TStr NrVNm=Vrtx->GetVNm(); NrVNm.ChangeChAll('"', '\'');
    TChA ChA;
    ChA+=TInt::GetStr(Vrtx->GetVId()); ChA+=':';
    ChA+='"'; ChA+=NrVNm; ChA+='"'; ChA+='\n';
    // output line
    TupIdSOut->PutStr(ChA);
    VarIdSOut->PutStr(ChA);
  }}
  // tuple-variable file
  {PSOut TupVarValSOut=TFOut::New(TupVarValFNm);
  int VrtxP=FFirstVrtx(); PVrtx Vrtx;
  while (FNextVrtx(VrtxP, Vrtx)){
    // get vertex weights
    int VId=Vrtx->GetVId();
    TIntFltH VIdToWgtH;
    VIdToWgtH.AddDat(VId, 1);
    _SaveTxtSparseNbrs(VId, 1, MxLev, VIdToWgtH);
    // prepare sparse vector
    TIntFltPrV VIdWgtPrV; VIdToWgtH.GetKeyDatPrV(VIdWgtPrV);
    VIdWgtPrV.Sort();
    // compose output line
    TChA ChA;
    //TStr NrVNm=Vrtx->GetVNm(); NrVNm.ChangeChAll('"', '\'');
    //ChA+='"'; ChA+=NrVNm; ChA+='"';
    ChA+=TInt::GetStr(VId);
    for (int VIdN=0; VIdN<VIdWgtPrV.Len(); VIdN++){
      ChA+=' '; ChA+=TInt::GetStr(VIdWgtPrV[VIdN].Val1);
      ChA+=':'; ChA+=TFlt::GetStr(VIdWgtPrV[VIdN].Val2, "%.3f");
    }
    ChA+='\n';
    // output line
    TupVarValSOut->PutStr(ChA);
  }}
}


/////////////////////////////////////////////////
// General-Graph
bool TGGraph::IsReg=TGGraph::MkReg();

int TGGraph::AddVrtx(const PVrtx& Vrtx){
  // define vertex
  Vrtx->DefVrtx(this);
  // shortcut
  int VId=Vrtx->GetVId();
  TStr VNm=Vrtx->GetVNm();
  // check id
  IAssert(!VIdToVrtxH.IsKey(VId));
  // add vertex
  VIdToVrtxH.AddDat(VId, Vrtx);
  if (!VNm.Empty()){VNmToVIdH.AddDat(VNm, VId);}
  // return vertex-id
  return VId;
}

void TGGraph::DelVrtx(const PVrtx& Vrtx){
  while (Vrtx->GetInEIds()>0){
    DelEdge(Vrtx->GetInEId(0));}
  while (Vrtx->GetOutEIds()>0){
    DelEdge(Vrtx->GetOutEId(0));}
  VIdToVrtxH.DelKey(Vrtx->GetVId());
  if (!Vrtx->GetVNm().Empty()){VNmToVIdH.DelKey(Vrtx->GetVNm());}
}

bool TGGraph::IsVrtx(const TStr& VNm, PVrtx& Vrtx) const {
  int VNmToVIdP;
  if (VNmToVIdH.IsKey(VNm, VNmToVIdP)){
    Vrtx=GetVrtx(VNmToVIdH[VNmToVIdP]); return true;}
  else {return false;}
}

bool TGGraph::FNextVrtx(int& VrtxP, PVrtx& Vrtx) const {
  if (VIdToVrtxH.FNextKeyId(VrtxP)){
    Vrtx=VIdToVrtxH[VrtxP]; return true;}
  else {return false;}
}

void TGGraph::ClrVrtxMarks() const {
  int VrtxP=FFirstVrtx(); PVrtx Vrtx;
  while (FNextVrtx(VrtxP, Vrtx)){
    Vrtx->ClrMark();}
}

int TGGraph::AddEdge(const PEdge& Edge){
  // define edge
  Edge->DefEdge(this);
  // shortcuts
  int EId=Edge->GetEId();
  int VId1=Edge->GetVId1();
  int VId2=Edge->GetVId2();
  bool DirP=Edge->IsDir();
  TStr ENm=Edge->GetENm();
  // check ids
  IAssert(!EIdToEdgeH.IsKey(EId));
  IAssert(VIdToVrtxH.IsKey(VId1));
  IAssert(VIdToVrtxH.IsKey(VId2));
  // add edge
  DefEId(EId);
  EIdToEdgeH.AddDat(EId, Edge);
  if (!ENm.Empty()){ENmToEIdH.AddDat(ENm, EId);}
  // add edge to vertices
  VIdToVrtxH.GetDat(VId1)->AddOutEId(EId);
  VIdToVrtxH.GetDat(VId2)->AddInEId(EId);
  if (!DirP){
    VIdToVrtxH.GetDat(VId1)->AddInEId(EId);
    VIdToVrtxH.GetDat(VId2)->AddOutEId(EId);
  }
  // return edge-id
  return EId;
}

void TGGraph::DelEdge(const PEdge& Edge){
  // shortcuts
  int EId=Edge->GetEId();
  int VId1=Edge->GetVId1();
  int VId2=Edge->GetVId2();
  // remove references in vertices
  GetVrtx(VId1)->DelEId(EId);
  GetVrtx(VId2)->DelEId(EId);
  // delete edge
  EIdToEdgeH.DelKey(Edge->GetEId());
  if (!Edge->GetENm().Empty()){
    if (ENmToEIdH.IsKey(Edge->GetENm())){
      ENmToEIdH.DelKey(Edge->GetENm());}
  }
}

bool TGGraph::IsEdge(const TStr& ENm, PEdge& Edge) const {
  int ENmToEIdP;
  if (ENmToEIdH.IsKey(ENm, ENmToEIdP)){
    Edge=GetEdge(ENmToEIdH[ENmToEIdP]); return true;}
  else {return false;}
}

bool TGGraph::FNextEdge(int& EdgeP, PEdge& Edge) const {
  if (EIdToEdgeH.FNextKeyId(EdgeP)){
    Edge=EIdToEdgeH[EdgeP]; return true;}
  else {return false;}
}

bool TGGraph::IsVrtxsEdge(
 const int& VId1, const int& VId2, const bool& DirP, PEdge& Edge) const {
  PVrtx Vrtx1=GetVrtx(VId1);
  PVrtx Vrtx2=GetVrtx(VId2);
  for (int EIdN=0; EIdN<Vrtx1->GetOutEIds(); EIdN++){
    Edge=GetEdge(Vrtx1->GetOutEId(EIdN));
    if (VId2==Edge->GetOtherVId(Vrtx1)){return true;}
  }
  if (!DirP){
    for (int EIdN=0; EIdN<Vrtx2->GetOutEIds(); EIdN++){
      Edge=GetEdge(Vrtx2->GetOutEId(EIdN));
      if (VId1==Edge->GetOtherVId(Vrtx2)){return true;}
    }
  }
  return false;
}

