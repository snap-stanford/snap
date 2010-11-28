#ifndef gksmfc_h
#define gksmfc_h

/////////////////////////////////////////////////
// Includes
#include "gks.h"
#include <afxwin.h>

/////////////////////////////////////////////////
// Vcl-Gks
class TMfcGks: public TGks {
private:
  CStatic* Static;
  mutable CRect StaticRect;
  mutable CDC BitmapDc;
  mutable CBitmap *Bitmap, *oldBitmap;
  mutable CPen *CurPen;
  mutable PGksPen gksPen;
  mutable CBrush *CurBrush;
  mutable PGksBrush gksBrush;
  mutable CFont *CurFont;
  mutable PGksFont gksFont;

public:
  TMfcGks(CStatic* _Static) : Static(_Static), Bitmap(NULL), CurPen(NULL), CurBrush(NULL), CurFont(NULL), oldBitmap(NULL) {
    Static->GetClientRect(&StaticRect);
    CClientDC Dc(Static);
    BitmapDc.CreateCompatibleDC(&Dc);
    BitmapDc.SetMapMode(MM_TEXT);
    Static->ModifyStyle(NULL, SS_BITMAP);
    Bitmap = new CBitmap();
    Bitmap->CreateCompatibleBitmap(&Dc, StaticRect.Width(), StaticRect.Height());
  }
  ~TMfcGks() { BitmapDc.DeleteDC();  if (Bitmap) delete Bitmap;  if (CurPen) delete CurPen;  
    if (CurBrush) delete CurBrush;  if (CurFont) delete CurFont; }
  static PGks New(CStatic* Static) { return new TMfcGks(Static); }
  
  // dimensions
  int GetWidth() const { return StaticRect.Width(); }
  int GetHeight() const { return StaticRect.Height(); }

  // clear
  void Clr() const { 
    if (oldBitmap==NULL) { BeginPaint();  BitmapDc.FillSolidRect(&StaticRect, BitmapDc.GetBkColor());  EndPaint(); }
    else { BitmapDc.FillSolidRect(&StaticRect, BitmapDc.GetBkColor()); }
  }
  void Inv() const { 
    if (oldBitmap==NULL) { Static->Invalidate(); }
    else { EndPaint();   Static->Invalidate();  BeginPaint(); } 
  }
  void BeginPaint() const {
    CRect newRect;  Static->GetClientRect(&newRect);
    if (newRect.Height() != StaticRect.Height() || newRect.Width() != StaticRect.Width()) {
      StaticRect = newRect;
      if (Bitmap != NULL) delete Bitmap;
      Bitmap = new CBitmap();
      Bitmap->CreateCompatibleBitmap(&CClientDC(Static), StaticRect.Width(), StaticRect.Height());
    }
    IAssertR(oldBitmap == NULL, "You haven't called EndPaint().");
    oldBitmap = BitmapDc.SelectObject(Bitmap);
  }
  void EndPaint() const { 
    BitmapDc.SelectObject(oldBitmap);  oldBitmap = 0;
    Static->SetBitmap(*Bitmap);
  }

  // pen
  void SetPen(const PGksPen& Pen) {
    gksPen = Pen;
    if (CurPen) delete CurPen;
    CurPen = new CPen(Pen->GetStyle(), Pen->GetWidth(), Pen->GetColor().GetRgbVal());
    BitmapDc.SelectObject(CurPen);
  }
  PGksPen GetPen() {
    return gksPen;
  }

  // brush
  void SetBrush(const PGksBrush& Brush) {
    gksBrush = Brush;
    if (CurBrush) delete CurBrush;
    switch (Brush->GetStyle()) {
      case gbsHorizontal : CurBrush = new CBrush(HS_HORIZONTAL, Brush->GetColor().GetRgbVal());  break;
      case gbsVertical : CurBrush = new CBrush(HS_VERTICAL, Brush->GetColor().GetRgbVal());  break;
      case gbsFDiagonal : CurBrush = new CBrush(HS_FDIAGONAL, Brush->GetColor().GetRgbVal());  break;
      case gbsBDiagonal : CurBrush = new CBrush(HS_BDIAGONAL, Brush->GetColor().GetRgbVal());  break;
      case gbsDiagCross : CurBrush = new CBrush(HS_DIAGCROSS, Brush->GetColor().GetRgbVal());  break;
      case gbsCross : CurBrush = new CBrush(HS_CROSS, Brush->GetColor().GetRgbVal());  break;
      default : CurBrush = new CBrush(Brush->GetColor().GetRgbVal());  break;
    }
    BitmapDc.SelectObject(CurBrush);
  }
  PGksBrush GetBrush() {
    return gksBrush;
  }

  // font
  void SetFont(const PGksFont& Font) {
    gksFont = Font;
    if (CurFont) delete CurFont;
    
    int Weight = FW_NORMAL;
    BOOL Italic = FALSE, Underline = FALSE;
    TFSet Style = Font->GetStyle();
    if (Style.In(gfsBold)) Weight = FW_BOLD;
    if (Style.In(gfsItalic)) Italic = TRUE;
    if (Style.In(gfsUnderline)) Underline = TRUE;
    CurFont = new CFont(); 
    CurFont->CreateFont(Font->GetSize(), 0, 0, 0, Weight, Italic, Underline, 0, ANSI_CHARSET, 
      OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, Font->GetNm().CStr());
    BitmapDc.SelectObject(CurFont);
    BitmapDc.SetTextColor(Font->GetColor().GetRgbVal());
  }
  PGksFont GetFont() {
    return gksFont;
  }

  // pixels
  void PutPixel(const int& X, const int& Y, const TGksColor& GksColor) { BitmapDc.SetPixel(X, Y, GksColor.GetRgbVal()); }
  TGksColor GetPixel(const int& X, const int& Y) { return TGksColor(BitmapDc.GetPixel(X, Y)); }

  // lines
  void Line(const int& X1, const int& Y1, const int& X2, const int& Y2) {
    BitmapDc.MoveTo(X1, Y1); BitmapDc.LineTo(X2, Y2);
  }
  void PolyLine(const TGksSPointV& PointV) {
    POINT* PointT = new POINT[PointV.Len()];
    for (int PointN=0; PointN < PointV.Len(); PointN++) { PointT[PointN].x=PointV[PointN].X;  PointT[PointN].y=PointV[PointN].Y; }
    BitmapDc.Polyline(PointT, PointV.Len()-1);
    delete[] PointT;
  }
  void Polygon(const TGksSPointV& PointV) {
    POINT* PointT = new POINT[PointV.Len()];
    for (int PointN = 0; PointN < PointV.Len(); PointN++) { PointT[PointN].x=PointV[PointN].X;  PointT[PointN].y=PointV[PointN].Y; }
    BitmapDc.Polygon(PointT, PointV.Len()-1);
    delete[] PointT;
  }
  void PolyBezier(const TGksSPointV& PointV) {
    POINT* PointT = new POINT[PointV.Len()];
    for (int PointN = 0; PointN < PointV.Len(); PointN++) { PointT[PointN].x=PointV[PointN].X;  PointT[PointN].y=PointV[PointN].Y; }
    BitmapDc.PolyBezier(PointT, PointV.Len()-1);
    delete[] PointT;
  }

  // rectangles
  void FrameRect(const int& X1, const int& Y1, const int& X2, const int& Y2) {
    TGksSPointV PointV(5, 0);
    PointV.Add(TGksSPoint(X1, Y1)); PointV.Add(TGksSPoint(X1, Y2));
    PointV.Add(TGksSPoint(X2, Y2)); PointV.Add(TGksSPoint(X2, Y1));
    PointV.Add(TGksSPoint(X1, Y1));
    PolyLine(PointV);
  }
  void FillRect(const int& X1, const int& Y1, const int& X2, const int& Y2) {
    BitmapDc.FillRect(CRect(X1, Y1, X2, Y2), BitmapDc.GetCurrentBrush()); }
  void Rectangle(const int& X1, const int& Y1, const int& X2, const int& Y2) {
    BitmapDc.Rectangle(X1, Y1, X2, Y2); }
  void RoundRect(const int& X1, const int& Y1, const int& X2, const int& Y2, const int& EWidth, const int& EHeight) {
    BitmapDc.RoundRect(X1, Y1, X2, Y2, EWidth, EHeight); }

  // ellipses & circles & arcs & pies
  void Ellipse(const int& X1, const int& Y1, const int& X2, const int& Y2) {
    BitmapDc.Ellipse(X1, Y1, X2, Y2); }
  void Arc(
   const int& X1, const int& Y1, const int& X2, const int& Y2,
   const int& X3, const int& Y3, const int& X4, const int& Y4) {
    BitmapDc.Arc(X1, Y1, X2, Y2, X3, Y3, X4, Y4); }

  // text
  int GetTxtWidth(const TStr& TxtStr) {
    const CSize Sz = BitmapDc.GetOutputTextExtent(TxtStr.CStr());
    return Sz.cx;
  }
  int GetTxtHeight(const TStr& TxtStr) {
    const CSize Sz = BitmapDc.GetOutputTextExtent(TxtStr.CStr());
    return Sz.cy;
  }
  void PutTxt(const TStr& TxtStr, const int& X, const int& Y) {
    LOGBRUSH logbrush;  CurBrush->GetLogBrush(&logbrush);
    const COLORREF OldColor = BitmapDc.SetBkColor(logbrush.lbColor);
    BitmapDc.TextOut(X, Y, TxtStr.CStr()); 
    BitmapDc.SetBkColor(OldColor);
  }
  void PutTxt(const TStr& TxtStr, const int& X, const int& Y, const TGksRect& ClipRect) {
    IAssert(ClipRect.GetX1() == X && ClipRect.GetY1() == Y);
    CRect Rect(ClipRect.GetX1(), ClipRect.GetY1(), ClipRect.GetX2(), ClipRect.GetY2());
    LOGBRUSH logbrush;  CurBrush->GetLogBrush(&logbrush);
    const COLORREF OldColor = BitmapDc.SetBkColor(logbrush.lbColor);
    BitmapDc.DrawText(TxtStr.CStr(), Rect, DT_CENTER);
    BitmapDc.SetBkColor(OldColor);
  }
};

#endif
