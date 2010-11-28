#ifndef gks_h
#define gks_h

/////////////////////////////////////////////////
// Includes
#include "base.h"

/////////////////////////////////////////////////
// Gks-Color
class TGksColor{
private:
  uint ArgbVal;
public:
  TGksColor(): ArgbVal(0){}
  TGksColor(const TGksColor& Color): ArgbVal(Color.ArgbVal){}
  TGksColor(const uint& _RgbVal): ArgbVal((0xFF)|(_RgbVal<<8)){}
  TGksColor(const uint& _ArgbVal, const bool& Alpha): ArgbVal(_ArgbVal){Assert(Alpha);}
  TGksColor(const uint& RVal, const uint& GVal, const uint& BVal): ArgbVal(0){
    ArgbVal=((0xFF)|(RVal&0xFF)<<8)|((GVal&0xFF)<<16)|((BVal&0xFF)<<24);}
  TGksColor(const uint& AVal, const uint& RVal, const uint& GVal, const uint& BVal): 
    ArgbVal(0){ ArgbVal=(AVal&0xFF)|((RVal&0xFF)<<8)|((GVal&0xFF)<<16)|((BVal&0xFF)<<24);}

  TGksColor& operator=(const TGksColor& Color){
    ArgbVal=Color.ArgbVal; return *this;}
  bool operator==(const TGksColor& GksColor) const {
    return ArgbVal==GksColor.ArgbVal;}

  void PutArgbVal(const uint& _ArgbVal){ArgbVal=_ArgbVal;}
  uint GetArgbVal() const {return ArgbVal;}
  uint GetRgbVal() const {return ArgbVal>>8&0xffffff;}
  int GetAVal() const {return ArgbVal&0xff;}
  int GetRVal() const {return (ArgbVal>>8)&0xff;}
  int GetGVal() const {return (ArgbVal>>16)&0xff;}
  int GetBVal() const {return (ArgbVal>>24)&0xff;}

  TGksColor Add(const uint& RVal, const uint& GVal, const uint& BVal) {
      return TGksColor(GetRVal() + RVal, GetGVal() + GVal, GetBVal() + BVal); }
  TGksColor Add(const TGksColor& AddColor) {
      return TGksColor(GetRVal() + AddColor.GetRVal(), 
          GetGVal() + AddColor.GetGVal(), GetBVal() + AddColor.GetBVal()); }

  static TGksColor GetRndColor(TRnd& Rnd){
    return TGksColor(Rnd.GetUniDevInt(0x1000000));}
  static TGksColor GetWhite(){return TGksColor(255, 255, 255);}
  static TGksColor GetBlack(){return TGksColor(0, 0, 0);}
  static TGksColor GetRed(){return TGksColor(255, 0, 0);}
  static TGksColor GetGreen(){return TGksColor(0, 255, 0);}
  static TGksColor GetBlue(){return TGksColor(0, 0, 255);}
  static TGksColor GetYellow(){return TGksColor(255, 255, 0);}
  static TGksColor GetMagenta(){return TGksColor(255, 0, 255);}
  static TGksColor GetCyan(){return TGksColor(0, 255, 255);}
  static TGksColor GetTransparent(){return TGksColor(0, 255, 255, 255);}

  static uint GetInvArgbVal(uint ArgbVal) {
      return ((ArgbVal>>24)&0xff)|(((ArgbVal>>16)&0xff)<<8)|
             (((ArgbVal>>8)&0xff)<<16)|((ArgbVal&0xff)<<24); }
};

/////////////////////////////////////////////////
// Gks-Screen-Point
class TGksSPoint{
public:
  int X;
  int Y;
public:
  TGksSPoint(): X(0), Y(0){}
  TGksSPoint(const TGksSPoint& Point): X(Point.X), Y(Point.Y){}
  TGksSPoint(const int& _X, const int& _Y): X(_X), Y(_Y){}

  TGksSPoint& operator=(const TGksSPoint& Point){
    X=Point.X; Y=Point.Y; return *this;}
};
typedef TVec<TGksSPoint> TGksSPointV;

/////////////////////////////////////////////////
// Gks-Virtual-Point
class TGksVPoint{
public:
  double X;
  double Y;
public:
  TGksVPoint(): X(0), Y(0){}
  TGksVPoint(const TGksVPoint& Point): X(Point.X), Y(Point.Y){}
  TGksVPoint(const double& _X, const double& _Y): X(_X), Y(_Y){}

  TGksVPoint& operator=(const TGksVPoint& Point){
    X=Point.X; Y=Point.Y; return *this;}
};
typedef TVec<TGksVPoint> TGksVPointV;

/////////////////////////////////////////////////
// Gks-Polar-Point
class TGksPPoint{
public:
  double Rad;
  double Ang;
public:
  TGksPPoint(): Rad(0), Ang(0){}
  TGksPPoint(const TGksPPoint& Point): Rad(Point.Rad), Ang(Point.Ang){}
  TGksPPoint(const double& _Rad, const double& _Ang): Rad(_Rad), Ang(_Ang){}
  TGksPPoint(const TGksSPoint& Point):
    Rad(sqrt(TMath::Sqr(Point.X)+TMath::Sqr(Point.Y))),
    Ang((Point.X==0)&&(Point.Y==0)?0:atan2(double(Point.Y), double(Point.X))){}
  TGksPPoint(const TGksVPoint& Point):
    Rad(sqrt(TMath::Sqr(Point.X)+TMath::Sqr(Point.Y))),
    Ang((Point.X==0)&&(Point.Y==0)?0:atan2(Point.Y, Point.X)){}

  TGksPPoint& operator=(const TGksPPoint& Point){
    Rad=Point.Rad; Ang=Point.Ang; return *this;}

  TGksSPoint GetSPoint() const {
    return TGksSPoint(int(Rad*cos(Ang)), int(Rad*sin(Ang)));}
  TGksVPoint GetVPoint() const {
    return TGksVPoint(Rad*cos(Ang), Rad*sin(Ang));}
};
typedef TVec<TGksPPoint> TGksPPointV;

/////////////////////////////////////////////////
// Gks-Rectangle
class TGksRect{
public:
  double X1, Y1, X2, Y2;
public:
  TGksRect(): X1(0), Y1(0), X2(0), Y2(0){}
  TGksRect(const TGksRect& Rect):
    X1(Rect.X1), Y1(Rect.Y1), X2(Rect.X2), Y2(Rect.Y2){}
  TGksRect(const TGksSPoint& Point1, const TGksSPoint& Point2):
    X1(Point1.X), Y1(Point1.Y),
    X2(Point2.X), Y2(Point2.Y){}
  TGksRect(const int& _X1, const int& _Y1, const int& _X2, const int& _Y2):
    X1(_X1), Y1(_Y1), X2(_X2), Y2(_Y2){}
  TGksRect(const double& _X1, const double& _Y1, const double& _X2, const double& _Y2):
    X1(_X1), Y1(_Y1), X2(_X2), Y2(_Y2){}

  TGksRect& operator=(const TGksRect& Rect){
    if (this!=&Rect){X1=Rect.X1; Y1=Rect.Y1; X2=Rect.X2; Y2=Rect.Y2;}
    return *this;}

  int GetX1() const {return int(X1);}
  int GetY1() const {return int(Y1);}
  int GetX2() const {return int(X2);}
  int GetY2() const {return int(Y2);}

  // get lengths
  double GetXLen() const {return X2-X1;}
  double GetYLen() const {return Y2-Y1;}

  // get centers
  double GetXCenter() const {return X1+(X2-X1)/2;}
  double GetYCenter() const {return Y1+(Y2-Y1)/2;}

  // sorting coordinates
  void Sort(){
    if (X1>X2){Swap<double>(X1,X2);}
    if (Y1>Y2){Swap<double>(Y1,Y2);}}

  // test inclusion
  bool IsIn(const double& X, const double& Y) const {
    bool XOk; bool YOk;
    if (X1<X2){XOk=(X1<=X)&&(X<=X2);} else {XOk=(X2<=X)&&(X<=X1);}
    if (Y1<Y2){YOk=(Y1<=Y)&&(Y<=Y2);} else {YOk=(Y2<=Y)&&(Y<=Y1);}
    return XOk&&YOk;
  }

  // conversion
  TFltRect GetFltRect() const {
    return TFltRect(X1, Y1, X2, Y2);}
};
typedef TVec<TGksRect> TGksRectV;

/////////////////////////////////////////////////
// Gks-Pen
typedef enum {gpmBlack, gpmWhite, gpmNop, gpmNot, gpmCopy, gpmNotCopy,
 gpmMergePenNot, gpmMaskPenNot, gpmMergeNotPen, gpmMaskNotPen, gpmMerge,
 gpmNotMerge, gpmMask, gpmNotMask, gpmXor, gpmNotXor} TGksPenMode;

typedef enum  {gpsSolid, gpsDash, gpsDot, gpsDashDot, gpsDashDotDot,
 gpsClear, gpsInsideFrame} TGksPenStyle;

ClassTP(TGksPen, PGksPen)//{
private:
  TGksColor Color;
  TGksPenMode Mode;
  TGksPenStyle Style;
  int Width;
public:
  TGksPen():
    Color(), Mode(gpmCopy), Style(gpsSolid), Width(1){}
  TGksPen(const TGksPen& Pen):
    Color(Pen.Color), Mode(Pen.Mode), Style(Pen.Style), Width(Pen.Width){}
  TGksPen(const TGksColor& _Color, const TGksPenMode& _Mode=gpmCopy,
   const TGksPenStyle& _Style=gpsSolid, const int& _Width=1):
    Color(_Color), Mode(_Mode), Style(_Style), Width(_Width){}
  static PGksPen New(const TGksColor& Color, const TGksPenMode& Mode=gpmCopy,
   const TGksPenStyle& Style=gpsSolid, const int& Width=1){
    return new TGksPen(Color, Mode, Style, Width);}
  TGksPen(TSIn&){Fail;}
  static PGksPen Load(TSIn& SIn){return new TGksPen(SIn);}
  void Save(TSOut&){Fail;}

  TGksPen& operator=(const TGksPen& Pen){
    if (this!=&Pen){
      Color=Pen.Color; Mode=Pen.Mode; Style=Pen.Style; Width=Pen.Width;}
    return *this;}

  TGksColor GetColor() const {return Color;}
  TGksPenMode GetMode() const {return Mode;}
  TGksPenStyle GetStyle() const {return Style;}
  int GetWidth() const {return Width;}
};

/////////////////////////////////////////////////
// Gks-Brush
typedef enum  {gbsSolid, gbsClear, gbsHorizontal, gbsVertical,
 gbsFDiagonal, gbsBDiagonal, gbsCross, gbsDiagCross} TGksBrushStyle;

ClassTP(TGksBrush, PGksBrush)//{
private:
  TGksColor Color;
  TGksBrushStyle Style;
public:
  TGksBrush():
    Color(), Style(gbsSolid){}
  TGksBrush(const TGksBrush& Brush):
    Color(Brush.Color), Style(Brush.Style){}
  TGksBrush(const TGksColor& _Color, const TGksBrushStyle& _Style=gbsSolid):
    Color(_Color), Style(_Style){}
  static PGksBrush New(
   const TGksColor& Color, const TGksBrushStyle& Style=gbsSolid){
    return new TGksBrush(Color, Style);}
  TGksBrush(TSIn&){Fail;}
  static PGksBrush Load(TSIn& SIn){return new TGksBrush(SIn);}
  void Save(TSOut&){Fail;}

  TGksBrush& operator=(const TGksBrush& Brush){
    if (this!=&Brush){Color=Brush.Color; Style=Brush.Style;} return *this;}

  TGksColor GetColor() const {return Color;}
  TGksBrushStyle GetStyle() const {return Style;}
};

/////////////////////////////////////////////////
// Gks-Font
typedef enum  {gfsBold, gfsItalic, gfsUnderline, gfsStrikeOut} TGksFontStyle;

ClassTP(TGksFont, PGksFont)//{
private:
  TStr Nm;
  int Size;
  TGksColor Color;
  TFSet Style;
public:
  TGksFont():
    Nm("System"), Size(10), Color(), Style(){}
  TGksFont(const TGksFont& Font):
    Nm(Font.Nm), Size(Font.Size), Color(Font.Color), Style(Font.Style){}
  TGksFont(const TStr& _Nm, const int& _Size=10,
   const TGksColor& _Color=TGksColor::GetBlack(), const TFSet& _Style=TFSet()):
    Nm(_Nm), Size(_Size), Color(_Color), Style(_Style){}
  static PGksFont New(const TStr& Nm, const int& Size=10,
   const TGksColor& Color=TGksColor::GetBlack(), const TFSet& Style=TFSet()){
    return new TGksFont(Nm, Size, Color, Style);}
  TGksFont(TSIn&){Fail;}
  static PGksFont Load(TSIn& SIn){return new TGksFont(SIn);}
  void Save(TSOut&){Fail;}

  TGksFont& operator=(const TGksFont& Font){
    if (this!=&Font){
      Nm=Font.Nm; Size=Font.Size; Color=Font.Color; Style=Font.Style;}
    return *this;}

  TStr GetNm() const {return Nm;}
  int GetSize() const {return Size;}
  TGksColor GetColor() const {return Color;}
  TFSet GetStyle() const {return Style;}
};

/////////////////////////////////////////////////
// Gks-Bitmap
ClassTP(TGksBitmap, PGksBitmap)//{
private:
  int Width;
  int Height;
  TVec<uint> PixelV;
public:
  TGksBitmap(): Width(0), Height(0), PixelV() {};

  TGksBitmap(const int& _Width, const int& _Height,
   const TGksColor& Color=TGksColor::GetWhite());
  static PGksBitmap New(const int& Width, const int& Height,
   const TGksColor& Color=TGksColor::GetWhite()){
    return new TGksBitmap(Width, Height, Color);}

  int GetWidth() const {return Width;}
  int GetHeight() const {return Height;}

  TGksColor GetPixel(const int& X, const int& Y) const {
    Assert((0<=X && X<Width)&&(0<=Y && Y<Height));
    return TGksColor(PixelV[Y*Width+X], true);}
  void SetPixel(const int& X, const int& Y, const TGksColor& Color) {
    Assert((0<=X && X<Width)&&(0<=Y && Y<Height));
    PixelV[Y*Width + X] = Color.GetArgbVal();}

    TVec<uint>& GetPixelV() { return PixelV; }
};

/////////////////////////////////////////////////
// Gks
typedef enum  {
 gtbtUndef, gtbtRect, gtbtRoundRect, gtbtEllipse, gtbtRomb} TGksTxtBoxType;

ClassTP(TGks, PGks)//{
public:
  void GetDbTxtData(
   const TStr& TxtStr1, const TStr& TxtStr2, const int& X, const int& Y,
   int& TxtWidth1, int& TxtHeight1, int& TxtWidth2, int& TxtHeight2,
   int& TxtWidth, int& TxtHeight, int& Y1, int& Y2);
  void GetStrVData(
   const TStrV& StrV, const int& /*X*/, const int& Y,
   TIntV& StrWidthV, TIntV& StrHeightV,
   int& StrVWidth, int& StrVHeight, TIntV& YV);
public:
  TGks(){}
  TGks(TSIn&){Fail;}
  virtual ~TGks(){}
  static PGks Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TGks& operator=(const TGks&){Fail; return *this;}

  // dimensions
  virtual int GetWidth() const=0;
  virtual int GetHeight() const=0;

  // clear
  virtual void Clr() const=0;
  virtual void Inv() const=0;

  // paint
  virtual void BeginPaint(HDC Hdc = 0, HANDLE Handle = 0) const {}
  virtual void EndPaint() const {}

  // pen
  virtual void SetPen(const PGksPen& Pen)=0;
  virtual PGksPen GetPen()=0;

  // brush
  virtual void SetBrush(const PGksBrush& Brush)=0;
  virtual PGksBrush GetBrush()=0;

  // font
  virtual void SetFont(const PGksFont& Font)=0;
  virtual PGksFont GetFont()=0;

  // virtual coordinates
  int GetVToPX(const double& VX) const {return int(VX*GetWidth());}
  int GetVToPY(const double& VY) const {return int(VY*GetHeight());}
  double GetPToVX(const int& PX) const {return double(PX)/double(GetWidth());}
  double GetPToVY(const int& PY) const {return double(PY)/double(GetHeight());}
  TGksRect GetPToVRect(const TGksRect& Rect){
    return TGksRect(
     GetPToVX(Rect.GetX1()), GetPToVY(Rect.GetY1()),
     GetPToVX(Rect.GetX2()), GetPToVY(Rect.GetY2()));}
  TGksRect GetVToPRect(const TGksRect& Rect){
    return TGksRect(
     GetVToPX(Rect.GetX1()), GetVToPY(Rect.GetY1()),
     GetVToPX(Rect.GetX2()), GetVToPY(Rect.GetY2()));}
  TGksRect GetVToPRect(const TFltRect& Rect){
    return TGksRect(
     GetVToPX(Rect.GetMnX()), GetVToPY(Rect.GetMnY()),
     GetVToPX(Rect.GetMxX()), GetVToPY(Rect.GetMxY()));}

  // pixels
  virtual void PutPixel(const int& X, const int& Y, const TGksColor& Color)=0;
  void PutPixel(const TGksSPoint& Point, const TGksColor& Color){
    PutPixel(Point.X, Point.Y, Color);}
  void PutPixelV(const TGksSPointV& PointV, const TGksColor& Color){
    for (int PointN=0; PointN<PointV.Len(); PointN++){
      PutPixel(PointV[PointN], Color);}}
  virtual TGksColor GetPixel(const int& X, const int& Y)=0;
  TGksColor GetPixel(const TGksSPoint& Point){
    return GetPixel(Point.X, Point.X);}
  TGksSPoint GetRndPoint(TRnd& Rnd){
    return TGksSPoint(Rnd.GetUniDevInt(GetWidth()), Rnd.GetUniDevInt(GetHeight()));}

  // lines & polygons & bezier
  virtual void Line(
   const int& X1, const int& Y1, const int& X2, const int& Y2)=0;
  void Line(const TGksSPoint& Point1, const TGksSPoint& Point2){
    Line(Point1.X, Point1.Y, Point2.X, Point2.Y);}
  virtual void PolyLine(const TGksSPointV& PointV)=0;
  virtual void Polygon(const TGksSPointV& PointV)=0;
  virtual void PolyBezier(const TGksSPointV& PointV)=0;
  void DirLine(const int& X1, const int& Y1, const int& X2, const int& Y2,
   const bool& LeftDir, const bool& RightDir,
   const int& ArrowLen=10, const double& ArrowOffset=0.25);
  void DirLine(const TGksSPoint& Point1, const TGksSPoint& Point2,
   const bool& LeftDir, const bool& RightDir,
   const int& ArrowLen=10, const double& ArrowOffset=0.25){
    DirLine(Point1.X, Point1.Y, Point2.X, Point2.Y,
     LeftDir, RightDir, ArrowLen, ArrowOffset);}

  // rectangles
  TGksRect GetRndRect(TRnd& Rnd){
    return TGksRect(
     Rnd.GetUniDevInt(GetWidth()), Rnd.GetUniDevInt(GetHeight()),
     Rnd.GetUniDevInt(GetWidth()), Rnd.GetUniDevInt(GetHeight()));}
  // frame
  virtual void FrameRect(
   const int& X1, const int& Y1, const int& X2, const int& Y2)=0;
  void FrameRect(const TGksRect& Rect){
    FrameRect(Rect.GetX1(), Rect.GetY1(), Rect.GetX2(), Rect.GetY2());}
  // fill
  virtual void FillRect(
   const int& X1, const int& Y1, const int& X2, const int& Y2)=0;
  void FillRect(const TGksRect& Rect){
    FillRect(Rect.GetX1(), Rect.GetY1(), Rect.GetX2(), Rect.GetY2());}
  // frame & fill
  virtual void Rectangle(
   const int& X1, const int& Y1, const int& X2, const int& Y2)=0;
  void Rectangle(const TGksRect& Rect){
    Rectangle(Rect.GetX1(), Rect.GetY1(), Rect.GetX2(), Rect.GetY2());}
  // round frame & fill
  virtual void RoundRect(const int& X1, const int& Y1,
   const int& X2, const int& Y2, const int& EWidth, const int& EHeight)=0;
  void RoundRect(const TGksRect& Rect, const int& EWidth, const int& EHeight){
    RoundRect(Rect.GetX1(), Rect.GetY1(), Rect.GetX2(), Rect.GetY2(),
     EWidth, EHeight);}

  // ellipses & circles & arcs & pies
  virtual void Ellipse(
   const int& X1, const int& Y1, const int& X2, const int& Y2)=0;
  void Ellipse(const TGksRect& Rect){
    Ellipse(Rect.GetX1(), Rect.GetY1(), Rect.GetX2(), Rect.GetY2());}
  virtual void FillEllipse(
   const int& X1, const int& Y1, const int& X2, const int& Y2)=0;
  void FillEllipse(const TGksRect& Rect){
    FillEllipse(Rect.GetX1(), Rect.GetY1(), Rect.GetX2(), Rect.GetY2());}
  void Circle(const int& X, const int& Y, const int& Rad){
    Ellipse(X-Rad/2, Y-Rad/2, X+Rad/2, Y+Rad/2);}
  void Circle(const TGksSPoint& Point, const int& Rad){
    Circle(Point.X, Point.Y, Rad);}
  virtual void Arc(
   const int& X1, const int& Y1, const int& X2, const int& Y2,
   const int& X3, const int& Y3, const int& X4, const int& Y4)=0;

  // bitmaps
  virtual void Bitmap(const PGksBitmap& Bmp, const int& X, const int& Y)=0;
  virtual void Bitmap(const PGksBitmap& Bmp,
   const int& X1, const int& Y1, const int& X2, const int& Y2)=0;
  void Bitmap(const PGksBitmap& Bmp, const TGksRect& Rect){
   Bitmap(Bmp, Rect.GetX1(), Rect.GetY1(), Rect.GetX2(), Rect.GetY2());}

  // text
  virtual int GetTxtWidth(const TStr& TxtStr)=0;
  virtual int GetTxtHeight(const TStr& TxtStr)=0;
  virtual void PutTxt(const TStr& TxtStr, const int& X, const int& Y)=0;
  void PutTxt(const TStr& TxtStr, const TGksSPoint& Point){
    PutTxt(TxtStr, Point.X, Point.Y);}
  virtual void PutTxt(const TStr& TxtStr, const int& X, const int& Y,
   const TGksRect& ClipRect)=0;
  void PutTxt(const TStr& TxtStr, const TGksSPoint& Point,
   const TGksRect& ClipRect){
    PutTxt(TxtStr, Point.X, Point.Y, ClipRect);}
  TStr BreakTxt(const TStr& TxtStr, const TStr& SplitStr, const TStr& EndLnStr,
      const int& TxtWidth, const int& MxLines = -1);
  TStr BreakTxt(const TStrV& PartV, const TStr& SplitStr, const TStr& EndLnStr, 
      const int& TxtWidth, const int& MxLines = -1);

  // text-boxes
  // text-box type
  static TGksTxtBoxType GetTxtBoxType(const TStr& TxtBoxTypeNm);
  static TStr GetTxtBoxTypeNm(const TGksTxtBoxType& Type);
  // generic
  TGksRect TxtBox(const TGksTxtBoxType& Type, const TStr& TxtStr,
   const int& X, const int& Y, const int& Margin=5);
  TGksRect TxtBox(const TGksTxtBoxType& Type, const TStr& TxtStr,
   const TGksSPoint& Point, const int& Margin=5){
    return TxtBox(Type, TxtStr, Point.X, Point.Y, Margin);}
  TGksRect TxtBox(const TGksTxtBoxType& Type,
   const TStr& TxtStr1, const TStr& TxtStr2,
   const int& X, const int& Y, const int& Margin=5);
  TGksRect TxtBox(const TGksTxtBoxType& Type,
   const TStr& TxtStr1, const TStr& TxtStr2,
   const TGksSPoint& Point, const int& Margin=5){
    return TxtBox(Type, TxtStr1, TxtStr2, Point.X, Point.Y, Margin);}
  // line
  TGksRect LineTxt(const TStr& TxtStr,
   const int& X1, const int& Y1, const int& X2, const int& Y2);
  TGksRect LineTxt(const TStr& TxtStr,
   const TGksSPoint& Point1, const TGksSPoint& Point2){
    return LineTxt(TxtStr, Point1.X, Point1.Y, Point2.X, Point2.Y);}
  // rectangle
  TGksRect RectTxtBox(const TStr& TxtStr,
   const int& X, const int& Y, const int& Margin=5);
  TGksRect RectTxtBox(const TStr& TxtStr,
   const TGksSPoint& Point, const int& Margin=5){
    return RectTxtBox(TxtStr, Point.X, Point.Y, Margin);}
  TGksRect RectTxtBox(const TStr& TxtStr1, const TStr& TxtStr2,
   const int& X, const int& Y, const int& Margin=5);
  TGksRect RectTxtBox(const TStr& TxtStr1, const TStr& TxtStr2,
   const TGksSPoint& Point, const int& Margin=5){
    return RectTxtBox(TxtStr1, TxtStr2, Point.X, Point.Y, Margin);}
  // round rectangle
  TGksRect RoundRectTxtBox(const TStr& TxtStr,
   const int& X, const int& Y, const int& Margin=5);
  TGksRect RoundRectTxtBox(const TStr& TxtStr,
   const TGksSPoint& Point, const int& Margin=5){
    return RoundRectTxtBox(TxtStr, Point.X, Point.Y, Margin);}
  TGksRect RoundRectTxtBox(const TStr& TxtStr1, const TStr& TxtStr2,
   const int& X, const int& Y, const int& Margin=5);
  TGksRect RoundRectTxtBox(const TStr& TxtStr1, const TStr& TxtStr2,
   const TGksSPoint& Point, const int& Margin=5){
    return RoundRectTxtBox(TxtStr1, TxtStr2, Point.X, Point.Y, Margin);}
  // ellipse
  TGksRect EllipseTxtBox(const TStr& TxtStr,
   const int& X, const int& Y, const int& Margin=5);
  TGksRect EllipseTxtBox(const TStr& TxtStr,
   const TGksSPoint& Point, const int& Margin=5){
    return EllipseTxtBox(TxtStr, Point.X, Point.Y, Margin);}
  TGksRect EllipseTxtBox(const TStr& TxtStr1, const TStr& TxtStr2,
   const int& X, const int& Y, const int& Margin=5);
  TGksRect EllipseTxtBox(const TStr& TxtStr1, const TStr& TxtStr2,
   const TGksSPoint& Point, const int& Margin=5){
    return EllipseTxtBox(TxtStr1, TxtStr2, Point.X, Point.Y, Margin);}
  // romb
  TGksRect RombTxtBox(const TStr& TxtStr,
   const int& X, const int& Y, const int& Margin=5);
  TGksRect RombTxtBox(const TStr& TxtStr,
   const TGksSPoint& Point, const int& Margin=5){
    return RombTxtBox(TxtStr, Point.X, Point.Y, Margin);}
  TGksRect RombTxtBox(const TStr& TxtStr1, const TStr& TxtStr2,
   const int& X, const int& Y, const int& Margin=5);
  TGksRect RombTxtBox(const TStr& TxtStr1, const TStr& TxtStr2,
   const TGksSPoint& Point, const int& Margin=5){
    return RombTxtBox(TxtStr1, TxtStr2, Point.X, Point.Y, Margin);}

  // generic
  double GetLineLen(const int& X1, const int& Y1, const int& X2, const int& Y2){
    return sqrt(TMath::Sqr(Y2-Y1)+TMath::Sqr(X2-X1));}
};

#endif
