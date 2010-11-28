#ifndef gksvcl_h
#define gksvcl_h

/////////////////////////////////////////////////
// Includes
#include "gks.h"
#include <vcl.h>

/////////////////////////////////////////////////
// Vcl-Gks
class TVclGks: public TGks{
  TPaintBox* PaintBox;
  TCanvas* Canvas;
public:
  TVclGks(TPaintBox* _PaintBox):
    PaintBox(_PaintBox), Canvas(PaintBox->Canvas){}
  static PGks New(TPaintBox* PaintBox){
    return new TVclGks(PaintBox);}

  // dimensions
  int GetWidth() const {return PaintBox->Width;}
  int GetHeight() const {return PaintBox->Height;}

  // clear
  void Clr() const {
    Canvas->Brush->Color=clWhite;
    Canvas->Brush->Style=bsSolid;
    Canvas->FillRect(PaintBox->ClientRect);}
  void Inv() const {PaintBox->Invalidate();}

  // pen
  void SetPen(const PGksPen& Pen){
    Canvas->Pen->Color=TColor(Pen->GetColor().GetRgbVal());
    Canvas->Pen->Mode=TPenMode(Pen->GetMode());
    Canvas->Pen->Style=TPenStyle(Pen->GetStyle());
    Canvas->Pen->Width=TPenStyle(Pen->GetWidth());
  }
  PGksPen GetPen(){
    TGksColor Color(uint(Canvas->Pen->Color));
    TGksPenMode Mode=TGksPenMode(Canvas->Pen->Mode);
    TGksPenStyle Style=TGksPenStyle(Canvas->Pen->Style);
    int Width=Canvas->Pen->Width;
    return PGksPen(new TGksPen(Color, Mode, Style, Width));
  }

  // brush
  void SetBrush(const PGksBrush& Brush){
    Canvas->Brush->Color=TColor(Brush->GetColor().GetRgbVal());
    Canvas->Brush->Style=TBrushStyle(Brush->GetStyle());
  }
  PGksBrush GetBrush(){
    TGksColor Color(uint(Canvas->Brush->Color));
    TGksBrushStyle Style=TGksBrushStyle(Canvas->Brush->Style);
    return PGksBrush(new TGksBrush(Color, Style));
  }

  // font
  void SetFont(const PGksFont& Font){
    Canvas->Font->Name=Font->GetNm().CStr();
    Canvas->Font->Size=Font->GetSize();
    Canvas->Font->Color=TColor(Font->GetColor().GetRgbVal());
  }
  PGksFont GetFont(){
    TStr Nm(Canvas->Font->Name.c_str());
    int Size=Canvas->Font->Size;
    TGksColor Color(uint(Canvas->Font->Color));
    return PGksFont(new TGksFont(Nm, Size, Color));
  }

  // pixels
  void PutPixel(const int& X, const int& Y, const TGksColor& GksColor){
    Canvas->Pixels[X][Y]=TColor(GksColor.GetRgbVal());}
  TGksColor GetPixel(const int& X, const int& Y){
    return TGksColor(Canvas->Pixels[X][Y]);}

  // lines
  void Line(const int& X1, const int& Y1, const int& X2, const int& Y2){
    Canvas->MoveTo(X1, Y1); Canvas->LineTo(X2, Y2);}
  void PolyLine(const TGksSPointV& PointV){
    POINT* PointT=new POINT[PointV.Len()];
    for (int PointN=0; PointN<PointV.Len(); PointN++){
      PointT[PointN].x=PointV[PointN].X;
      PointT[PointN].y=PointV[PointN].Y;
    }
    //Canvas->Polyline(PointT, PointV.Len()-1);
    delete[] PointT;
  }
  void Polygon(const TGksSPointV& PointV){
    Windows::TPoint PointT[4];
    for (int PointN=0; PointN<PointV.Len(); PointN++){
      PointT[PointN]=Point(PointV[PointN].X, PointV[PointN].Y);
    }
    Canvas->Polygon(PointT, PointV.Len()-1);
    delete[] PointT;
  }
  void PolyBezier(const TGksSPointV& PointV){
    POINT* PointT=new POINT[PointV.Len()];
    for (int PointN=0; PointN<PointV.Len(); PointN++){
      PointT[PointN].x=PointV[PointN].X;
      PointT[PointN].y=PointV[PointN].Y;
    }
    //Canvas->PolyBezier(PointT, PointV.Len()-1);
    delete[] PointT;
  }

  // rectangles
  void FrameRect(const int& X1, const int& Y1, const int& X2, const int& Y2){
    TGksSPointV PointV(5, 0);
    PointV.Add(TGksSPoint(X1, Y1)); PointV.Add(TGksSPoint(X1, Y2));
    PointV.Add(TGksSPoint(X2, Y2)); PointV.Add(TGksSPoint(X2, Y1));
    PointV.Add(TGksSPoint(X1, Y1));
    PolyLine(PointV);
  }
  void FillRect(const int& X1, const int& Y1, const int& X2, const int& Y2){
    TRect Rect; Rect.Left=X1; Rect.Top=Y1; Rect.Right=X2; Rect.Bottom=Y2;
    Canvas->FillRect(Rect);
  }
  void Rectangle(const int& X1, const int& Y1, const int& X2, const int& Y2){
    Canvas->Rectangle(X1, Y1, X2, Y2);
  }
  void RoundRect(const int& X1, const int& Y1, const int& X2, const int& Y2,
   const int& EWidth, const int& EHeight){
    //Canvas->RoundRect(X1, Y1, X2, Y2, EWidth, EHeight);
    Canvas->RoundRect(X1, Y1, X2, Y2, 10, 10);
  }

  // ellipses & circles & arcs & pies
  void Ellipse(const int& X1, const int& Y1, const int& X2, const int& Y2){
    Canvas->Ellipse(X1, Y1, X2, Y2);}
  void FillEllipse(
   const int& X1, const int& Y1, const int& X2, const int& Y2){
    Fail;
  }
  void Arc(
   const int& X1, const int& Y1, const int& X2, const int& Y2,
   const int& X3, const int& Y3, const int& X4, const int& Y4){
    Canvas->Arc(X1, Y1, X2, Y2, X3, Y3, X4, Y4);}

  // bitmaps
  void Bitmap(const PGksBitmap& Bmp, const int& X, const int& Y){
    Fail;}
  void Bitmap(const PGksBitmap& Bmp,
   const int& X1, const int& Y1, const int& X2, const int& Y2){
    Fail;}

  // text
  int GetTxtWidth(const TStr& TxtStr){
    return Canvas->TextWidth(TxtStr.CStr());}
  int GetTxtHeight(const TStr& TxtStr){
    return Canvas->TextHeight(TxtStr.CStr());}
  void PutTxt(const TStr& TxtStr, const int& X, const int& Y){
    Canvas->TextOut(X, Y, TxtStr.CStr());
  }
  void PutTxt(const TStr& TxtStr, const int& X, const int& Y,
   const TGksRect& ClipRect){
    TRect Rect;
    Rect.Left=ClipRect.GetX1(); Rect.Top=ClipRect.GetY1();
    Rect.Left=ClipRect.GetX2(); Rect.Top=ClipRect.GetY2();
    Canvas->TextRect(Rect, X, Y, TxtStr.CStr());
  }
};

#endif
