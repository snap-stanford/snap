#ifndef GksVml_h
#define GksVml_h

/////////////////////////////////////////////////
// Includes
#include "gks.h"

// http://www.carto.net/papers/svg/

/////////////////////////////////////////////////
// Svg-Gks
class TVmlGks: public TGks {
private:
  TStr OutFNm, Title;
  mutable FILE *OutF;
  TInt Width, Height;
  PGksPen CurPen;
  PGksBrush CurBrush;
  PGksFont CurFont;

private:
  void Flush() const { fflush(OutF);}
  void PutHtmlHeader() const;
  void PutHtmlFooter() const;
public:
  TVmlGks() : OutF(NULL), OutFNm("gkssvg.html"), Width(1000), Height(1000) { PutHtmlHeader();}
  TVmlGks(const TStr& OutFName, const TStr& _Title, const TInt& _Width, const TInt& _Height);
  //: Static(_Static), Bitmap(NULL), CurPen(NULL), CurBrush(NULL), CurFont(NULL), oldBitmap(NULL) {

  ~TVmlGks() {
    if (OutF) { PutHtmlFooter(); fclose(OutF);}  OutF=NULL;}
  static PGks New(const TStr& OutFName=TStr(), const TStr& Title=TStr(), const TInt& Width=0, const TInt& Height=0) {
    return new TVmlGks(OutFName, Title, Width, Height);}

  static TStr GetHtmlColor(const TGksColor& GksColor){
    return TStr::Fmt("#%02x%02x%02x",
     GksColor.GetRVal(), GksColor.GetGVal(), GksColor.GetBVal());}

  // dimensions
  int GetWidth() const { return Width;}
  int GetHeight() const { return Height;}

  // clear
  void Clr() const { fclose(OutF);  OutF=NULL;  PutHtmlHeader();}
  void Inv() const { Flush();}
  void BeginPaint() const { }
  void EndPaint() const { Flush();}

  // pen
  void SetPen(const PGksPen& Pen) { CurPen = Pen;}
  PGksPen GetPen() { return CurPen;}

  // brush
  void SetBrush(const PGksBrush& Brush) { CurBrush = Brush;}
  PGksBrush GetBrush() { return CurBrush;}

  // font
  void SetFont(const PGksFont& Font) { CurFont = Font;}
  PGksFont GetFont() { return CurFont;}

  // pixels
  void PutPixel(const int& X, const int& Y, const TGksColor& GksColor) {
    fprintf(OutF, "<v:rect style=\"left:%d; top:%d; width:1; height:1;\" fillcolor=\"%s\" strokeweight=\"1px\" strokecolor=\"%s\" />\n",
     X, Y, GetHtmlColor(GksColor).CStr(), GetHtmlColor(GksColor).CStr());
  }
  TGksColor GetPixel(const int& X, const int& Y){Fail; return TGksColor();}

  // lines
  void Line(const int& X1, const int& Y1, const int& X2, const int& Y2) {
    TGksPenStyle Style = CurPen->GetStyle();
    fprintf(OutF, "<v:line from=\"%d,%d\" to=\"%d,%d\" strokeweight=\"%d\" strokecolor=\"%s\"> ",
     X1, Y1, X2, Y2, CurPen->GetWidth(), GetHtmlColor(CurPen->GetColor()).CStr());
    switch(CurPen->GetStyle()) {
      case gpsDash : fprintf(OutF, "<v:stroke dashstyle=\"dash\" /> </v:line>\n");  break;
      case gpsDot : fprintf(OutF, "<v:stroke dashstyle=\"dot\" /> </v:line>\n");  break;
      case gpsDashDot : fprintf(OutF, "<v:stroke dashstyle=\"dashdot\" /> </v:line>\n");  break;
      case gpsDashDotDot : fprintf(OutF, "<v:stroke dashstyle=\"longdashdotdot\" /> </v:line>\n");  break;
      default : fprintf(OutF, "<v:stroke dashstyle=\"solid\" /> </v:line>\n");  break;
    };
  }
  void PolyLine(const TGksSPointV& PointV){
    TGksPenStyle Style = CurPen->GetStyle();
    fprintf(OutF, "<v:polyline points=\"%d,%d", PointV[0].X, PointV[0].Y);
    for (int i = 1; i < PointV.Len(); i++) fprintf(OutF, ", %d,%d", PointV[i].X, PointV[i].Y);
    fprintf(OutF, "\" strokecolor=\"%s\" strokeweight=\"%d\">",
     GetHtmlColor(CurPen->GetColor()).CStr(), CurPen->GetWidth());
    switch(CurPen->GetStyle()) {
      case gpsDash : fprintf(OutF, "<v:stroke dashstyle=\"dash\" /> </v:line>\n");  break;
      case gpsDot : fprintf(OutF, "<v:stroke dashstyle=\"dot\" /> </v:line>\n");  break;
      case gpsDashDot : fprintf(OutF, "<v:stroke dashstyle=\"dashdot\" /> </v:line>\n");  break;
      case gpsDashDotDot : fprintf(OutF, "<v:stroke dashstyle=\"longdashdotdot\" /> </v:line>\n");  break;
      default : fprintf(OutF, "<v:stroke dashstyle=\"solid\" /> </v:polyline>\n");  break;
    };
  }
  void Polygon(const TGksSPointV& PointV) { Fail;}
  void PolyBezier(const TGksSPointV& PointV) { Fail;}

  // rectangles
  void FrameRect(const int& X1, const int& Y1, const int& X2, const int& Y2) {
    TGksSPointV PointV(5, 0);
    PointV.Add(TGksSPoint(X1, Y1)); PointV.Add(TGksSPoint(X1, Y2));
    PointV.Add(TGksSPoint(X2, Y2)); PointV.Add(TGksSPoint(X2, Y1));
    PointV.Add(TGksSPoint(X1, Y1));
    PolyLine(PointV);
  }
  void FillRect(const int& X1, const int& Y1, const int& X2, const int& Y2) {
    fprintf(OutF, "<v:rect style=\"left:%d; top:%d; width:%d; height:%d;\" fillcolor=\"%s\" strokeweight=\"1px\" strokecolor=\"%s\" />\n", X1, Y1, X2-X1, Y2-Y1, GetHtmlColor(CurBrush->GetColor()).CStr(), GetHtmlColor(CurBrush->GetColor()).CStr());
  }
  void Rectangle(const int& X1, const int& Y1, const int& X2, const int& Y2) {
    fprintf(OutF, "<v:rect style=\"left:%d; top:%d; width:%d; height:%d;\" fillcolor=\"%s\" strokeweight=\"%d\" strokecolor=\"%s\" />\n", X1, Y1, X2-X1, Y2-Y1, GetHtmlColor(CurBrush->GetColor()).CStr(), CurPen->GetWidth(), GetHtmlColor(CurPen->GetColor()).CStr());
  }
  void RoundRect(const int& X1, const int& Y1, const int& X2, const int& Y2, const int& EWidth, const int& EHeight) {
    fprintf(OutF, "<v:roundrect style=\"left:%d; top:%d; width:%d; height:%d;\" arcsize=\"%.4f\"", X1, Y1, X2-X1, Y2-Y1, double(EWidth)/double(TMath::Mx(X2-X1, Y2-Y1)));
    fprintf(OutF, " fillcolor=\"%s\" strokecolor=\"%s\" strokeweight=\"%d\" />\n", GetHtmlColor(CurBrush->GetColor()).CStr(), GetHtmlColor(CurPen->GetColor()).CStr(), CurPen->GetWidth());
  }
  // ellipses & circles & arcs & pies
  void Ellipse(const int& X1, const int& Y1, const int& X2, const int& Y2) {
    fprintf(OutF, "<v:oval style=\"left:%d; top:%d; width:%d; height:%d;\"", X1, Y1, X2-X1, Y2-Y1);
    fprintf(OutF, " fillcolor=\"%s\" strokecolor=\"%s\" strokeweight=\"%d\" />\n",
     GetHtmlColor(CurBrush->GetColor()).CStr(), GetHtmlColor(CurPen->GetColor()).CStr(), CurPen->GetWidth());
  }
  void FillEllipse(const int& X1, const int& Y1, const int& X2, const int& Y2) { }
  void Arc(
   const int& X1, const int& Y1, const int& X2, const int& Y2,
   const int& X3, const int& Y3, const int& X4, const int& Y4){
    Fail;
  }

  // bitmaps
  void Bitmap(const PGksBitmap& Bmp, const int& X, const int& Y) { }
  void Bitmap(const PGksBitmap& Bmp,
      const int& X1, const int& Y1, const int& X2, const int& Y2) { }

  // text
  int GetTxtWidth(const TStr& TxtStr) {
    //const CSize Sz = BitmapDc.GetOutputTextExtent(TxtStr.CStr());
    //return Sz.cx;
    return 2*TxtStr.Len()*CurFont->GetSize()/3;
  }
  int GetTxtHeight(const TStr& TxtStr) {
    return CurFont->GetSize()*2;
  }
  void PutTxt(const TStr& TxtStr, const int& X, const int& Y) {
    fprintf(OutF, "<v:shape style='position: absolute; left: %d; top: %d; width: %d; height: %dpt'>", X-Width/100, Y-5, 100, CurFont->GetSize());
    TStr NewStr = TxtStr;  NewStr.ChangeStrAll("\n", "<br>");
    fprintf(OutF, "<v:textbox style=\"text-align:left; v-text-wrapping:true; ");
    const TFSet FontS = CurFont->GetStyle();
    if (FontS.In(gfsUnderline)) fprintf(OutF, "text-decoration: underline; "); 
    else if (FontS.In(gfsStrikeOut)) fprintf(OutF, "text-decoration: line-through; ");
    fprintf(OutF, "font: ");
    if (FontS.In(gfsBold)) fprintf(OutF, "bold ");
    if (FontS.In(gfsItalic)) fprintf(OutF, "italic ");
    fprintf(OutF, "%dpt %s; color:%s;\">", CurFont->GetSize(), CurFont->GetNm().CStr(), GetHtmlColor(CurFont->GetColor()).CStr());
    fprintf(OutF, " %s  </v:textbox> </v:shape>\n", NewStr.CStr());
  }
  void PutTxt(const TStr& TxtStr, const int& X, const int& Y, const TGksRect& ClipRect) {
    fprintf(OutF, "<v:shape style='position: absolute; left: %d; top: %d; width: %d; height: %dpt'>", X, Y, ClipRect.GetXLen(), ClipRect.GetYLen());
    TStr NewStr = TxtStr;  NewStr.ChangeStrAll("\n", "<br>");
    fprintf(OutF, "<v:textbox style=\"text-align:left; v-text-wrapping:true; ");
    const TFSet FontS = CurFont->GetStyle();
    if (FontS.In(gfsUnderline)) fprintf(OutF, "text-decoration: underline; ");
    else if (FontS.In(gfsStrikeOut)) fprintf(OutF, "text-decoration: line-through; ");
    fprintf(OutF, "font: ");
    if (FontS.In(gfsBold)) fprintf(OutF, "bold ");
    if (FontS.In(gfsItalic)) fprintf(OutF, "italic ");
    fprintf(OutF, "%dpt %s; color:%s;\">", CurFont->GetSize(), CurFont->GetNm().CStr(), GetHtmlColor(CurFont->GetColor()).CStr());
    fprintf(OutF, " %s  </v:textbox> </v:shape>\n", NewStr.CStr());
  }
};

#endif
