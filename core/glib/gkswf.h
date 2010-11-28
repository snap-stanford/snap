#ifndef gkswf_h
#define gkswf_h

/////////////////////////////////////////////////
// Includes
#include "gks.h"
#include <gdiplus.h>

/////////////////////////////////////////////////
// Windows-Forms-Gks
class TWfGks: public TGks {
private:
    // canvas
    mutable Gdiplus::Graphics* GdiGraphics;
    // gks elements and their GDI+ associets :)
    mutable PGksPen GksPen;
    mutable Gdiplus::Pen GdiPen;
    mutable PGksBrush GksBrush;
    mutable Gdiplus::Brush *GdiBrush;
    mutable PGksFont GksFont;
    mutable Gdiplus::Font *GdiFont;
    mutable Gdiplus::Brush *GdiFontBrush;
    // stora some info so it is always available
    mutable int Width;
    mutable int Height;
private:
    Gdiplus::Color Gks2GdiColor(const TGksColor& GksCl) const {
        return Gdiplus::Color(GksCl.GetAVal(), GksCl.GetRVal(), GksCl.GetGVal(), GksCl.GetBVal());
    }

    (Gdiplus::Point*) LoadTGksSPointV(const TGksSPointV& PointV) {
        Gdiplus::Point *Pts = new Gdiplus::Point[PointV.Len()];
        for (int PtC = 0; PtC < PointV.Len(); PtC++) {
            Pts[PtC] = Gdiplus::Point(PointV[PtC].X, PointV[PtC].Y);
        }
        return Pts;
    }

    (WCHAR*) TStr2WCHAR(const TStr& Str) {
        WCHAR *lpWideCharStr = new WCHAR[Str.Len()+1];
        const int Res = MultiByteToWideChar(CP_ACP, 0, 
            Str.CStr(), Str.Len()+1, lpWideCharStr, Str.Len()+1);
        return lpWideCharStr;
    }

    void InvArgbP(const int Len, uint* ScanP) {
        for (int Elt = 0; Elt < Len; Elt++) {
            ScanP[Elt] = TGksColor::GetInvArgbVal(ScanP[Elt]);
        }
    }

    void MeasureString(const TStr& Str, int& Width, int& Height) {
        // first we change TStr to wchar*
        WCHAR *lpWideCharStr = TStr2WCHAR(Str);
        // than we measure it
        Gdiplus::RectF layoutRect(0, 0, 2000, 2000), boundRect;
        GdiGraphics->MeasureString(lpWideCharStr, 
            Str.Len(), GdiFont, layoutRect, &boundRect);
        Width = (int)ceil(boundRect.Width);
        Height = (int)ceil(boundRect.Height);
        delete lpWideCharStr;
    }

public:
    TWfGks(): GdiGraphics(NULL), GdiPen(Gdiplus::Color()), 
        GdiBrush(new Gdiplus::SolidBrush(Gdiplus::Color(0,0,0))), 
        GdiFontBrush(new Gdiplus::SolidBrush(Gdiplus::Color(0,0,0))), 
        GdiFont(new Gdiplus::Font(L"ARIAL", 10, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel, NULL)),
        Width(0), Height(0) { };
    static PGks New() { return new TWfGks(); }
    ~TWfGks() { 
        IAssert(GdiBrush != NULL); delete GdiBrush; 
        IAssert(GdiFont != NULL); delete GdiFont; }

    // paint
    void BeginPaint(HDC Hdc = 0, HANDLE Handle = 0) const { 
        IAssert(GdiGraphics == NULL);
        if (Handle == 0) {
            GdiGraphics = new Gdiplus::Graphics(Hdc);
        } else {
            GdiGraphics = new Gdiplus::Graphics(Hdc, Handle);
        }
        //InfoNotify("Status: " + TInt((int)GdiGraphics->GetLastStatus()).GetStr());
        GdiGraphics->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
        Gdiplus::Rect boundRect;
        GdiGraphics->GetVisibleClipBounds(&boundRect);
        Width = boundRect.Width;
        Height = boundRect.Height;
    }
    void EndPaint() const {
        if (GdiGraphics != NULL)  delete GdiGraphics;
        GdiGraphics = NULL;
    }

    // dimensions
    int GetWidth() const { return Width; }
    int GetHeight() const { return Height; }

    // clear
    void Clr() const {
        Gdiplus::Rect boundRect;
        GdiGraphics->GetVisibleClipBounds(&boundRect);
        GdiGraphics->FillRectangle(
            &Gdiplus::SolidBrush(Gdiplus::Color(255, 255, 255, 255)), boundRect);
    }
    void Inv() const { /* what is this? */ } //TODO

    // pen
    void SetPen(const PGksPen& Pen){ 
        GksPen = Pen;
        GdiPen.SetColor(Gks2GdiColor(Pen->GetColor()));
        GdiPen.SetWidth((Gdiplus::REAL)Pen->GetWidth());
    }
    PGksPen GetPen(){ return GksPen; }

    // brush
    void SetBrush(const PGksBrush& Brush){
        GksBrush = Brush;      
        ((Gdiplus::SolidBrush*)GdiBrush)->SetColor(
            Gks2GdiColor(Brush->GetColor()));
    }
    PGksBrush GetBrush(){ return GksBrush; }

    // font
    void SetFont(const PGksFont& Font){ 
        GksFont = Font;
        ((Gdiplus::SolidBrush*)GdiFontBrush)->SetColor(
            Gks2GdiColor(Font->GetColor()));
        IAssert(GdiFont != NULL); delete GdiFont;
        GdiFont = new Gdiplus::Font(L"ARIAL", (float)Font->GetSize(), 
            Font->GetStyle().In(gfsBold) ? Gdiplus::FontStyleBold : Gdiplus::FontStyleRegular, 
            Gdiplus::UnitPixel, NULL);
    }
    PGksFont GetFont(){ return GksFont; }

    // TODO pixels
    void PutPixel(const int& X, const int& Y, const TGksColor& GksColor) { Fail; }
    TGksColor GetPixel(const int& X, const int& Y) { Fail; return NULL; }

    // lines
    void Line(const int& X1, const int& Y1, const int& X2, const int& Y2) { 
        GdiGraphics->DrawLine(&GdiPen, X1, Y1, X2, Y2);
    }
    void PolyLine(const TGksSPointV& PointV) { 
        Gdiplus::Point* Pts = LoadTGksSPointV(PointV);
        GdiGraphics->DrawLines(&GdiPen, Pts, PointV.Len());
        delete[] Pts;
    }
    void Polygon(const TGksSPointV& PointV) {
        Gdiplus::Point* Pts = LoadTGksSPointV(PointV);
        GdiGraphics->DrawPolygon(&GdiPen, Pts, PointV.Len());
        delete[] Pts;
    }
    void PolyBezier(const TGksSPointV& PointV) { 
        // should we draw normal bezier here or a spline?!
        Gdiplus::Point* Pts = LoadTGksSPointV(PointV);
        GdiGraphics->DrawCurve(&GdiPen, Pts, PointV.Len());
        delete[] Pts;
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
        const int x = TInt::GetMn(X1,X2), width = TInt::Abs(X1-X2);
        const int y = TInt::GetMn(Y1,Y2), height = TInt::Abs(Y1-Y2);
        GdiGraphics->FillRectangle(GdiBrush, x, y, width, height);
    }
    void Rectangle(const int& X1, const int& Y1, const int& X2, const int& Y2) {
        const int x = TInt::GetMn(X1,X2), width = TInt::Abs(X1-X2);
        const int y = TInt::GetMn(Y1,Y2), height = TInt::Abs(Y1-Y2);
        //Gdiplus::SolidBrush WhiteBrush(Gdiplus::Color(255,255,255));
        //GdiGraphics->FillRectangle(&WhiteBrush, x, y, width, height);
        GdiGraphics->FillRectangle(GdiBrush, x, y, width, height);
        GdiGraphics->DrawRectangle(&GdiPen, x, y, width, height);
    }
    void RoundRect(const int& X1, const int& Y1, const int& X2, 
            const int& Y2, const int& EWidthXX, const int& EHeightXX) {
        const int EWidth = 5, EHeight = 5;
        const int x = TInt::GetMn(X1,X2), width = TInt::Abs(X1-X2);
        const int y = TInt::GetMn(Y1,Y2), height = TInt::Abs(Y1-Y2);
        //Gdiplus::SolidBrush WhiteBrush(Gdiplus::Color(255,255,255));
        // first clear the area
        GdiGraphics->FillRectangle(GdiBrush, x+EWidth, y, width-2*EWidth, height); 
        GdiGraphics->FillRectangle(GdiBrush, x, y+EHeight, width, height-2*EHeight); 
        GdiGraphics->FillPie(GdiBrush, x, y, 2*EWidth, 2*EHeight, 180.0, 90.0);
        GdiGraphics->FillPie(GdiBrush, x+width-2*EWidth, y, 2*EWidth, 2*EHeight, 270.0, 90.0);
        GdiGraphics->FillPie(GdiBrush, x, y+height-2*EHeight, 2*EWidth, 2*EHeight, 90.0, 90.0);
        GdiGraphics->FillPie(GdiBrush, x+width-2*EWidth, y+height-2*EHeight, 2*EWidth, 2*EHeight, 0.0, 90.0);
        // draw strait lines
        Line(x+EWidth, y, x + width - EWidth, y);
        Line(x+EWidth, y+height, x + width - EWidth, y+height);
        Line(x, y+EHeight, x, y + height - EHeight+1);
        Line(x+width, y+EHeight, x+width, y + height - EHeight+1);
        // draw edges
        GdiGraphics->DrawArc(&GdiPen, x, y, 2*EWidth, 2*EHeight, 180.0, 90.0);
        GdiGraphics->DrawArc(&GdiPen, x+width-2*EWidth, y, 2*EWidth, 2*EHeight, 270.0, 90.0);
        GdiGraphics->DrawArc(&GdiPen, x, y+height-2*EHeight, 2*EWidth, 2*EHeight, 90.0, 90.0);
        GdiGraphics->DrawArc(&GdiPen, x+width-2*EWidth, y+height-2*EHeight, 2*EWidth, 2*EHeight, 0.0, 90.0);
    }

    // ellipses & circles & arcs & pies
    void Ellipse(const int& X1, const int& Y1, const int& X2, const int& Y2) {
        const int x = TInt::GetMn(X1,X2), width = TInt::Abs(X1-X2);
        const int y = TInt::GetMn(Y1,Y2), height = TInt::Abs(Y1-Y2);
        GdiGraphics->DrawEllipse(&GdiPen, x, y, width, height);
    }
    void FillEllipse(const int& X1, const int& Y1, const int& X2, const int& Y2) {
        const int x = TInt::GetMn(X1,X2), width = TInt::Abs(X1-X2);
        const int y = TInt::GetMn(Y1,Y2), height = TInt::Abs(Y1-Y2);
        GdiGraphics->FillEllipse(GdiBrush, x, y, width, height);
    }
    void Arc(const int& X1, const int& Y1, const int& X2, const int& Y2,
        const int& X3, const int& Y3, const int& X4, const int& Y4) {
        //TODO convert upper parameteres to parameters for GDI+ DrawArc
        Fail;
    }

    void Bitmap(const PGksBitmap& Bmp, const int& X, const int& Y) {
        const int Width = Bmp->GetWidth(), Height = Bmp->GetHeight();
        uint* ScanP = Bmp->GetPixelV().BegI(); InvArgbP(Width*Height, ScanP);
        Gdiplus::Bitmap* GdiBmp = new Gdiplus::Bitmap(Width, Height, 
            sizeof(uint)*Width, PixelFormat32bppRGB, (BYTE*)ScanP);
        GdiGraphics->DrawImage(GdiBmp, X, Y); 
        delete GdiBmp; InvArgbP(Width*Height, ScanP); 
    }

    void Bitmap(const PGksBitmap& Bmp, const int& X1, 
            const int& Y1, const int& X2, const int& Y2) {

        const int Width = Bmp->GetWidth(), Height = Bmp->GetHeight();
        uint* ScanP = Bmp->GetPixelV().BegI(); InvArgbP(Width*Height, ScanP);
        Gdiplus::Bitmap* GdiBmp = new Gdiplus::Bitmap(Width, Height, 
            sizeof(uint)*Width, PixelFormat32bppRGB, (BYTE*)ScanP);
        Gdiplus::RectF GdiRectF(
            (Gdiplus::REAL)TInt::GetMn(X1, X2), (Gdiplus::REAL)TInt::GetMn(Y1, Y2), 
            (Gdiplus::REAL)TInt::Abs(X2 - X1),  (Gdiplus::REAL)TInt::Abs(Y2 - Y1));
        GdiGraphics->DrawImage(GdiBmp, GdiRectF);
        delete GdiBmp; InvArgbP(Width*Height, ScanP); 
    }

    // text
    int GetTxtWidth(const TStr& TxtStr) { 
        int Width, Height; MeasureString(TxtStr, Width, Height); return Width; }
    int GetTxtHeight(const TStr& TxtStr) {
        int Width, Height; MeasureString(TxtStr, Width, Height); return Height; }
    void PutTxt(const TStr& TxtStr, const int& X, const int& Y) { 
        WCHAR *lpWideCharStr = TStr2WCHAR(TxtStr);
        GdiGraphics->DrawString(lpWideCharStr, TxtStr.Len(), GdiFont, 
            Gdiplus::PointF((float)X, (float)Y), GdiFontBrush);
        delete lpWideCharStr;
    }
    void PutTxt(const TStr& TxtStr, const int& X, const int& Y, const TGksRect& ClipRect) { 
        // prepare parameters
        Gdiplus::RectF Rct((float)ClipRect.GetX1(), (float)ClipRect.GetX2(),
            (float)ClipRect.GetXLen(), (float)ClipRect.GetYLen());
        Gdiplus::StringFormat StrF;
        // draw string
        WCHAR *lpWideCharStr = TStr2WCHAR(TxtStr);
        GdiGraphics->DrawString(lpWideCharStr, TxtStr.Len(), 
            GdiFont, Rct, &StrF, GdiFontBrush);
        delete lpWideCharStr;
    }
};

#endif
