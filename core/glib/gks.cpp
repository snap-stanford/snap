/////////////////////////////////////////////////
// Includes
#include "stdafx.h"
#include "gks.h"

/////////////////////////////////////////////////
// Gks-Bitmap
TGksBitmap::TGksBitmap(const int& _Width, const int& _Height, const TGksColor& Color):
 Width(_Width), Height(_Height){
  PixelV.Gen(Width*Height);
  const int Pixels=PixelV.Len();
  for (int PixelN=0; PixelN<Pixels; PixelN++){
   PixelV[PixelN]=Color.GetArgbVal();
  }
}


/////////////////////////////////////////////////
// Gks-Canvas
void TGks::GetDbTxtData(
 const TStr& TxtStr1, const TStr& TxtStr2, const int& /*X*/, const int& Y,
 int& TxtWidth1, int& TxtHeight1, int& TxtWidth2, int& TxtHeight2,
 int& TxtWidth, int& TxtHeight, int& Y1, int& Y2){
  TxtWidth1=GetTxtWidth(TxtStr1);
  TxtHeight1=GetTxtHeight(TxtStr1);
  TxtWidth2=GetTxtWidth(TxtStr2);
  TxtHeight2=GetTxtHeight(TxtStr2);
  TxtWidth=TInt::GetMx(TxtWidth1, TxtWidth2);
  TxtHeight=TxtHeight1+TxtHeight2;
  Y1=Y-TxtHeight1/2;
  Y2=Y+TxtHeight2/2;
}

void TGks::GetStrVData(
 const TStrV& StrV, const int& /*X*/, const int& Y,
 TIntV& StrWidthV, TIntV& StrHeightV,
 int& StrVWidth, int& StrVHeight, TIntV& YV){
  // prepare single-string & string-vector widths & heights
  StrWidthV.Gen(StrV.Len()); StrHeightV.Gen(StrV.Len());
  StrVWidth=0; StrVHeight=0;
  for (int StrN=0; StrN<StrV.Len(); StrN++){
    // set single-string width & height
    StrWidthV[StrN]=GetTxtWidth(StrV[StrN]);
    StrHeightV[StrN]=GetTxtHeight(StrV[StrN]);
    // set string-vector width & height
    StrVWidth=TInt::GetMx(StrVWidth, StrWidthV[StrN]);
    StrVHeight+=StrHeightV[StrN];
  }
  // set Y coordinates for strings
  YV.Gen(StrV.Len());
  if (StrV.Len()>0){
    YV[0]=Y-StrVHeight/2+StrHeightV[0]/2;
    for (int StrN=1; StrN<StrV.Len(); StrN++){
      YV[StrN]=YV[StrN-1]+StrHeightV[StrN];
    }
  }
}

void TGks::DirLine(
 const int& X1, const int& Y1, const int& X2, const int& Y2,
 const bool& LeftDir, const bool& RightDir,
 const int& ArrowLen, const double& ArrowOffset){
  Line(X1, Y1, X2, Y2);
//  double K=99999; if (X2-X1!=0){K=double(Y2-Y1)/double(X2-X1);}
  double AK=99999; if (Y2-Y1!=0){AK=double(X2-X1)/double(Y1-Y2);}
//  double N=double(Y1)-K*double(X1);
  double LineLen=GetLineLen(X1, Y1, X2, Y2);
  double ArrowLenRatio=0;
  if (LineLen!=0){ArrowLenRatio=ArrowLen/LineLen;}
  if (LeftDir){
    // XY for top of arrow
    int TopX=int(X1+(1-ArrowOffset)*(X2-X1));
    int TopY=int(Y1+(1-ArrowOffset)*(Y2-Y1));
    // XY for back of arrow on line
    int BackX=int(X1+((1-ArrowOffset)-ArrowLenRatio)*(X2-X1));
    int BackY=int(Y1+((1-ArrowOffset)-ArrowLenRatio)*(Y2-Y1));
    // tail length ratio
    double AN=BackY-AK*BackX;
    double AX=BackX+100;
    double AY=AK*AX+AN;
    double TailLenRatio=(0.5*ArrowLen)/GetLineLen(BackX, BackY, int(AX), int(AY));
    // XY for upper tail
    int UpTailX=int(BackX+(TailLenRatio)*(AX-BackX));
    int UpTailY=int(BackY+(TailLenRatio)*(AY-BackY));
    // XY for lower tail
    int LowTailX=int(BackX-(TailLenRatio)*(AX-BackX));
    int LowTailY=int(BackY-(TailLenRatio)*(AY-BackY));
    // draw
    Line(TopX, TopY, UpTailX, UpTailY);
    Line(TopX, TopY, LowTailX, LowTailY);
    Line(LowTailX, LowTailY, TopX, TopY);
  }
  if (RightDir){
    // XY for top of arrow
    int TopX=int(X1+ArrowOffset*(X2-X1));
    int TopY=int(Y1+ArrowOffset*(Y2-Y1));
    // XY for back of arrow on line
    int BackX=int(X1+(ArrowOffset+ArrowLenRatio)*(X2-X1));
    int BackY=int(Y1+(ArrowOffset+ArrowLenRatio)*(Y2-Y1));
    // tail length ratio
    double AN=BackY-AK*BackX;
    double AX=BackX+1;
    double AY=AK*AX+AN;
    double TailLenRatio=(0.5*ArrowLen)/GetLineLen(BackX, BackY, int(AX), int(AY));
    // XY for upper tail
    int UpTailX=int(BackX+(TailLenRatio)*(AX-BackX));
    int UpTailY=int(BackY+(TailLenRatio)*(AY-BackY));
    // XY for lower tail
    int LowTailX=int(BackX-(TailLenRatio)*(AX-BackX));
    int LowTailY=int(BackY-(TailLenRatio)*(AY-BackY));
    // draw
    Line(TopX, TopY, UpTailX, UpTailY);
    Line(TopX, TopY, LowTailX, LowTailY);
  }
}

TStr TGks::BreakTxt(
  const TStr& TxtStr, const TStr& SplitStr, const TStr& EndLnStr,
  const int& TxtWidth, const int& MxLines) {

  TStrV PartV; TxtStr.SplitOnStr(SplitStr, PartV);
  return BreakTxt(PartV, SplitStr, EndLnStr, TxtWidth, MxLines);
}

TStr TGks::BreakTxt(
  const TStrV& PartV, const TStr& SplitStr, const TStr& EndLnStr,
  const int& TxtWidth, const int& MxLines) {

  if (PartV.Empty()) { return TStr(); }
  TChA NewTxtStr = PartV[0]; int LineN = 0;
  int CurrWidth = GetTxtWidth(PartV[0]); 
  for (int PartN = 1; PartN < PartV.Len(); PartN++) {
    const TStr& PartStr = PartV[PartN];
    const int PartWidth = GetTxtWidth(PartStr);
    if ((CurrWidth+PartWidth) > TxtWidth) {
      CurrWidth = PartWidth; LineN++;
      if (MxLines==LineN) { NewTxtStr += SplitStr + "..."; break; }
        NewTxtStr += EndLnStr; NewTxtStr += '\n'; NewTxtStr += PartStr;
      } else {
        CurrWidth += PartWidth;
        NewTxtStr += SplitStr; NewTxtStr += PartStr;
      }
  }
  return TStr(NewTxtStr);
}

TGksTxtBoxType TGks::GetTxtBoxType(const TStr& TxtBoxTypeNm){
  if (TxtBoxTypeNm=="Rect"){return gtbtRect;}
  else if (TxtBoxTypeNm=="RoundRect"){return gtbtRoundRect;}
  else if (TxtBoxTypeNm=="Ellipse"){return gtbtEllipse;}
  else if (TxtBoxTypeNm=="Romb"){return gtbtRomb;}
  else {return gtbtRect;}
}

TStr TGks::GetTxtBoxTypeNm(const TGksTxtBoxType& Type){
  switch (Type){
    case gtbtRect: return "Rect";
    case gtbtRoundRect: return "RoundRect";
    case gtbtEllipse: return "Ellipse";
    case gtbtRomb: return "Romb";
    default: Fail; return TStr();
  }
}

TGksRect TGks::TxtBox(const TGksTxtBoxType& Type, const TStr& TxtStr,
 const int& X, const int& Y, const int& Margin){
  if (TxtStr.IsChIn('#')){
    TStr TxtStr1; TStr TxtStr2;
    TxtStr.SplitOnCh(TxtStr1, '#', TxtStr2);
    return TxtBox(Type, TxtStr1, TxtStr2, X, Y, Margin);
  } else {
    switch (Type){
      case gtbtRect: return RectTxtBox(TxtStr, X, Y, Margin);
      case gtbtRoundRect: return RoundRectTxtBox(TxtStr, X, Y, Margin);
      case gtbtEllipse: return EllipseTxtBox(TxtStr, X, Y, Margin);
      case gtbtRomb: return RombTxtBox(TxtStr, X, Y, Margin);
      default: Fail; return TGksRect();
    }
  }
}

TGksRect TGks::TxtBox(const TGksTxtBoxType& Type,
 const TStr& TxtStr1, const TStr& TxtStr2,
 const int& X, const int& Y, const int& Margin){
  switch (Type){
    case gtbtRect: return RectTxtBox(TxtStr1, TxtStr2, X, Y, Margin);
    case gtbtRoundRect: return RoundRectTxtBox(TxtStr1, TxtStr2, X, Y, Margin);
    case gtbtEllipse: return EllipseTxtBox(TxtStr1, TxtStr2, X, Y, Margin);
    case gtbtRomb: return RombTxtBox(TxtStr1, TxtStr2, X, Y, Margin);
    default: Fail; return TGksRect();
  }
}

TGksRect TGks::LineTxt(const TStr& TxtStr,
 const int& X1, const int& Y1, const int& X2, const int& Y2){
  int TxtWidth=GetTxtWidth(TxtStr);
  int TxtHeight=GetTxtHeight(TxtStr);
  int TxtX=(X1+(X2-X1)/2);
  int TxtY=(Y1+(Y2-Y1)/2);
  // PutTxt(TxtStr, TxtX-TxtWidth/2, TxtY-TxtHeight/2);
  TxtBox(gtbtRect, TxtStr, TxtX, TxtY, 2);
  TGksRect Rect(
   TxtX-TxtWidth/2, TxtY-TxtHeight/2,
   TxtX+TxtWidth/2, TxtY+TxtHeight/2);
  return Rect;
}

TGksRect TGks::RectTxtBox(const TStr& TxtStr,
 const int& X, const int& Y, const int& Margin){
  if ((!TxtStr.IsChIn('\n'))&&(!TxtStr.IsChIn('\\'))){
    // single line text
    int TxtWidth=GetTxtWidth(TxtStr);
    int TxtHeight=GetTxtHeight(TxtStr);
    TGksRect Rect(
     X-TxtWidth/2-Margin, Y-TxtHeight/2-Margin,
     X+TxtWidth/2+Margin, Y+TxtHeight/2+Margin);
    RoundRect(Rect, (TxtWidth+Margin)/4, (TxtHeight+Margin)/2);
    PutTxt(TxtStr, X-TxtWidth/2, Y-TxtHeight/2);
    return Rect;
  } else {
    // multi line text
    // split text to lines
    TStrV TxtStrV;
    if (TxtStr.IsChIn('\n')){TxtStr.SplitOnAllCh('\n', TxtStrV);}
    else if (TxtStr.IsChIn('\\')){TxtStr.SplitOnAllCh('\\', TxtStrV);}
    // get lines position data
    TIntV TxtWidthV; TIntV TxtHeightV;
    int TxtVWidth; int TxtVHeight; TIntV YV;
    GetStrVData(TxtStrV, X, Y,
     TxtWidthV, TxtHeightV, TxtVWidth, TxtVHeight, YV);
    // create frame
    TGksRect Rect(
     X-TxtVWidth/2-Margin, Y-TxtVHeight/2-Margin,
     X+TxtVWidth/2+Margin, Y+TxtVHeight/2+Margin);
    // draw frame
    Rectangle(Rect);
    // draw text
    for (int TxtStrN=0; TxtStrN<TxtStrV.Len(); TxtStrN++){
      PutTxt(TxtStrV[TxtStrN],
       X-TxtWidthV[TxtStrN]/2, YV[TxtStrN]-TxtHeightV[TxtStrN]/2);
    }
    // return frame
    return Rect;
  }
  /*int TxtWidth=GetTxtWidth(TxtStr);
  int TxtHeight=GetTxtHeight(TxtStr);
  TGksRect Rect(
   X-TxtWidth/2-Margin, Y-TxtHeight/2-Margin,
   X+TxtWidth/2+Margin, Y+TxtHeight/2+Margin);
  Rectangle(Rect);
  PutTxt(TxtStr, X-TxtWidth/2, Y-TxtHeight/2);
  return Rect;*/
}

TGksRect TGks::RectTxtBox(const TStr& TxtStr1, const TStr& TxtStr2,
 const int& X, const int& Y, const int& Margin){
  int TxtWidth1; int TxtHeight1; int TxtWidth2; int TxtHeight2;
  int TxtWidth; int TxtHeight; int Y1; int Y2;
  GetDbTxtData(TxtStr1, TxtStr2, X, Y,
   TxtWidth1, TxtHeight1, TxtWidth2, TxtHeight2,
   TxtWidth, TxtHeight, Y1, Y2);
  TGksRect Rect(
   X-TxtWidth/2-Margin, Y-TxtHeight/2-Margin,
   X+TxtWidth/2+Margin, Y+TxtHeight/2+Margin);
  Rectangle(Rect);
  PutTxt(TxtStr1, X-TxtWidth1/2, Y1-TxtHeight1/2);
  PutTxt(TxtStr2, X-TxtWidth2/2, Y2-TxtHeight2/2);
  return Rect;
}

TGksRect TGks::RoundRectTxtBox(const TStr& TxtStr,
 const int& X, const int& Y, const int& Margin){
  if ((!TxtStr.IsChIn('\n'))&&(!TxtStr.IsChIn('\\'))){
    // single line text
    int TxtWidth=GetTxtWidth(TxtStr);
    int TxtHeight=GetTxtHeight(TxtStr);
    TGksRect Rect(
     X-TxtWidth/2-Margin, Y-TxtHeight/2-Margin,
     X+TxtWidth/2+Margin, Y+TxtHeight/2+Margin);
    RoundRect(Rect, (TxtWidth+Margin)/4, (TxtHeight+Margin)/2);
    PutTxt(TxtStr, X-TxtWidth/2, Y-TxtHeight/2);
    return Rect;
  } else {
    // multi line text
    // split text to lines
    TStrV TxtStrV;
    if (TxtStr.IsChIn('\n')){TxtStr.SplitOnAllCh('\n', TxtStrV);}
    else if (TxtStr.IsChIn('\\')){TxtStr.SplitOnAllCh('\\', TxtStrV);}
    // get lines position data
    TIntV TxtWidthV; TIntV TxtHeightV;
    int TxtVWidth; int TxtVHeight; TIntV YV;
    GetStrVData(TxtStrV, X, Y,
     TxtWidthV, TxtHeightV, TxtVWidth, TxtVHeight, YV);
    // create frame
    TGksRect Rect(
     X-TxtVWidth/2-Margin, Y-TxtVHeight/2-Margin,
     X+TxtVWidth/2+Margin, Y+TxtVHeight/2+Margin);
    // draw frame
    RoundRect(Rect, (TxtVWidth+Margin)/4, (TxtVHeight+Margin)/2);
    // draw text
    for (int TxtStrN=0; TxtStrN<TxtStrV.Len(); TxtStrN++){
      PutTxt(TxtStrV[TxtStrN],
       X-TxtWidthV[TxtStrN]/2, YV[TxtStrN]-TxtHeightV[TxtStrN]/2);
    }
    // return frame
    return Rect;
  }
}

TGksRect TGks::RoundRectTxtBox(const TStr& TxtStr1, const TStr& TxtStr2,
 const int& X, const int& Y, const int& Margin){
  int TxtWidth1; int TxtHeight1; int TxtWidth2; int TxtHeight2;
  int TxtWidth; int TxtHeight; int Y1; int Y2;
  GetDbTxtData(TxtStr1, TxtStr2, X, Y,
   TxtWidth1, TxtHeight1, TxtWidth2, TxtHeight2,
   TxtWidth, TxtHeight, Y1, Y2);
  TGksRect Rect(
   X-TxtWidth/2-Margin, Y-TxtHeight/2-Margin,
   X+TxtWidth/2+Margin, Y+TxtHeight/2+Margin);
  RoundRect(Rect, (TxtWidth+Margin)/4, (TxtHeight+Margin)/2);
  PutTxt(TxtStr1, X-TxtWidth1/2, Y1-TxtHeight1/2);
  PutTxt(TxtStr2, X-TxtWidth2/2, Y2-TxtHeight2/2);
  return Rect;
}

TGksRect TGks::EllipseTxtBox(const TStr& TxtStr,
 const int& X, const int& Y, const int& Margin){
  int TxtWidth=GetTxtWidth(TxtStr);
  int TxtHeight=GetTxtHeight(TxtStr);
  TGksRect Rect(
   X-TxtWidth/2-Margin, Y-TxtHeight/2-Margin,
   X+TxtWidth/2+Margin, Y+TxtHeight/2+Margin);
  Ellipse(Rect);
  PutTxt(TxtStr, X-TxtWidth/2, Y-TxtHeight/2);
  return Rect;
}

TGksRect TGks::EllipseTxtBox(const TStr& TxtStr1, const TStr& TxtStr2,
 const int& X, const int& Y, const int& Margin){
  int TxtWidth1; int TxtHeight1; int TxtWidth2; int TxtHeight2;
  int TxtWidth; int TxtHeight; int Y1; int Y2;
  GetDbTxtData(TxtStr1, TxtStr2, X, Y,
   TxtWidth1, TxtHeight1, TxtWidth2, TxtHeight2,
   TxtWidth, TxtHeight, Y1, Y2);
  TGksRect Rect(
   X-TxtWidth/2-Margin, Y-TxtHeight/2-Margin,
   X+TxtWidth/2+Margin, Y+TxtHeight/2+Margin);
  Ellipse(Rect);
  PutTxt(TxtStr1, X-TxtWidth1/2, Y1-TxtHeight1/2);
  PutTxt(TxtStr2, X-TxtWidth2/2, Y2-TxtHeight2/2);
  return Rect;
}

TGksRect TGks::RombTxtBox(const TStr& TxtStr,
 const int& X, const int& Y, const int& Margin){
  int TxtWidth=GetTxtWidth(TxtStr);
  int TxtHeight=GetTxtHeight(TxtStr);
  TGksRect Rect(
   (X-TxtWidth/2-Margin), (Y-TxtHeight-Margin),
   (X+TxtWidth/2+Margin), (Y+TxtHeight+Margin));
  TGksSPoint Point1((Rect.GetX1()+Rect.GetX2())/2, Rect.GetY1());
  TGksSPoint Point2(Rect.GetX1(), (Rect.GetY1()+Rect.GetY2())/2);
  TGksSPoint Point3((Rect.GetX1()+Rect.GetX2())/2, Rect.GetY2());
  TGksSPoint Point4(Rect.GetX2(), (Rect.GetY1()+Rect.GetY2())/2);
//  TGksSPoint Point1(Rect.GetX1(), Rect.GetY1());
//  TGksSPoint Point2(Rect.GetX1(), Rect.GetY2());
//  TGksSPoint Point3(Rect.GetX2(), Rect.GetY1());
//  TGksSPoint Point4(Rect.GetX2(), Rect.GetY2());
  TGksSPointV PointV(4, 0);
  PointV.Add(Point1); PointV.Add(Point2);
  PointV.Add(Point3); PointV.Add(Point4);
  Polygon(PointV);
//  Rectangle(Rect);
  PutTxt(TxtStr, X-TxtWidth/2, Y-TxtHeight/2);
  return Rect;
}

TGksRect TGks::RombTxtBox(const TStr& TxtStr1, const TStr& TxtStr2,
 const int& X, const int& Y, const int& Margin){
  int TxtWidth1; int TxtHeight1; int TxtWidth2; int TxtHeight2;
  int TxtWidth; int TxtHeight; int Y1; int Y2;
  GetDbTxtData(TxtStr1, TxtStr2, X, Y,
   TxtWidth1, TxtHeight1, TxtWidth2, TxtHeight2,
   TxtWidth, TxtHeight, Y1, Y2);
  TGksRect Rect(
   X-TxtWidth/2-Margin, Y-TxtHeight/2-Margin,
   X+TxtWidth/2+Margin, Y+TxtHeight/2+Margin);
  Ellipse(Rect);
  PutTxt(TxtStr1, X-TxtWidth1/2, Y1-TxtHeight1/2);
  PutTxt(TxtStr2, X-TxtWidth2/2, Y2-TxtHeight2/2);
  return Rect;
}

