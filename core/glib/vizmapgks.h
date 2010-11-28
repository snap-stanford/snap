#ifndef vizmapgks_H
#define vizmapgks_H

#include "mine.h"
#include "gks.h"

ClassTP(TVizMapContext, PVizMapContext)//{
private:
    static TGksColor ColorLandscapeAbove;
    static TGksColor ColorAddFirstLevel;
    static TGksColor ColorAddSecondLevel;
    static TGksColor ColorLegendGridColor;
    static TGksColor ColorLink;
    static TGksColor ColorPointCross;
    static TGksColor ColorSelPointCross;
    static TGksColor ColorPointFont;
    static TGksColor ColorSelPointFont;
    static TGksColor ColorKeyWdFont;
    static TGksColor ColorCatNmFont;
    static TGksColor ColorZoomRect;
    static TGksColor ColorMgGlass;
    static TGksColor ColorMgGlassWnd;
    static TGksColor ColorMgGlassWndFrm;
    static TGksColor ColorMgGlassWndShadow;

    static int SizePointCross;
    static int PointNmWidth;
    static int PointNmMxLines;
    static double PointNmFontScale;
    static int MgGlassWindowWidth;
    static int RndKeyWdN;
    static double RelRadius;
    static int KeyWdCandidates;
    static double RelMnRptDist;
    static double RelSparsity;

    typedef enum {vmsmZoom, vmsmSelect} TVizMapSelMode;

private:
    PVizMap VizMap;
    TInt VizMapFrameN;
    PVizMapFrame VizMapFrame;

    TSStack<TFltRect> ZoomS; // zoom stack
    TVizMapSelMode SelMode;
    TIntV SelPointV; // selected points
    PVizMapLandscape SelLandscape; // currently viewd landscape
    PGksBitmap LandscapeBmp; // current landscape bitmap
    TInt MgGlassPoints;
    TStrV MgGlassKeyWdV; // magnifying glass keywords

    TBool MouseInP;
    TInt ScreenX, ScreenY; // current mouse position (screen coords)
    TFlt MouseX, MouseY; // current mouse position (map coords)
    TBool LeftButtonDownP;
    TInt DownX, DownY; // where was button pressed (screen coords)
    TFlt MgGlassSize;
    TInt NearPointN;

    TVec<TGksColor> LandscapeCatColorV;
    void LoadColor();

    TStrStrH CatFullNmH;
    void LoadCatFullNms();

    TFltRect GetZoomRect();
    void RenderLandscapeBmp();
    void ResetMgGlassSize();
    void ResetMgGlassKeyWd(const int& MgGlassKeyWds);
    void ResetKeyWd();        
    void ResetNearPointN();

    double GetMapCoord(const int& ScreenCoord, const int& ScreenLen, 
        const double& MapMn, const double& MapLen);
    int GetScreenCoord(const double& MapCoord, const double& MapMn, 
        const double& MapLen, const int& ScreenLen);

    int GetEncoderClsid(const TStr& EncoderType, CLSID* pClsid);

    void ResetFrame();
        
public:
    TVizMapContext(PVizMap _VizMap);
    static PVizMapContext New(PVizMap VizMap) { 
        return new TVizMapContext(VizMap); }

    PVizMap GetVizMap() { return VizMap; }
    PVizMapFrame GetVizMapFrame() { return VizMapFrame; }
    int GetVizMapFrameN() { return VizMapFrameN; }
    void SetVizMapFrame(const int& NewVizMapFrameN);

    void SaveVrml(const TStr& VrmlFNm, bool ShowPointNmP, bool ShowKeyWdP);

    // paint stuff
    static void PaintEmpty(PGks Gks);    
    void Paint(PGks Gks, const bool& ShowPointNmP, const int& PointFontSize,
        const int& PointNmFontScale, const double& PointWgtThreshold, const int& CatId,
        const bool& ShowCatNmP, const bool& ShowKeyWdP, const int& KeyWdFontSize, 
        const bool& ShowMgGlassP, const int& LegendGridWidth, const int& LegendGridHeight);
    void PaintLandscape(PGks Gks);
    void PaintLegendGrid(PGks Gks, const int& LegendGridWidth, const int& LegendGridHeight);
    void PaintLinks(PGks Gks);
    void TVizMapContext::PaintArrow(PGks Gks, const int& SrcX, const int& SrcY, 
        const int& DstX, const int& DstY, const int& Width);
    void PaintPoints(PGks Gks, const int& PointFontSize, const int& PointNmFontScale,
        const double& PointWgtThreshold, const int& CatId, const bool& ShowMgGlassP,
        TVec<TFltRect>& PointNmRectV);
    void PaintPointCross(PGks Gks, const int& X, const int& Y, 
        const bool& SelPointP, const bool& IsCatP);
    TFltRect PaintPointNm(PGks Gks, PVizMapPoint Point, const int& X, 
        const int& Y, const int& PointFontSize, const int& PointNmFontScale, 
        const bool& SelPointP, const bool& IsCatP);
    void PaintCatNms(PGks Gks, const int& KeyWdFontSize, TVec<TFltRect>& PointNmRectV);
    void PaintKeyWds(PGks Gks, const int& KeyWdFontSize, TVec<TFltRect>& PointNmRectV);
    void PaintZoomRect(PGks Gks);
    void PaintMgGlass(PGks Gks, const int& KeyWdFontSize);

    // saving to files
    void Export(const TStr& FNm, const TStr& EncoderType, const int& Width, 
        const int& Height, const bool& ShowPointNmP, const int& PointFontSize, 
        const int& PointNmFontScale, const double& PointWgtThreshold, 
        const bool& ShowCatNmP, const bool& ShowKeyWdP, const int& KeyWdFontSize, 
        const bool& ShowMgGlassP, const int& LegendGridWidth, 
        const int& LegendGridHeight);

    // mouse moves
    void MouseEnter() { MouseInP = true; }
    void MouseLeave() { MouseInP = false; }
    bool MouseMove(const int& X, const int& Y, const int& Width, 
        const int& Height, const int& MgGlassKeyWds);
    void MouseLeftDown(const int& X, const int& Y, const int& Width, const int& Height);
    void MouseLeftUp(const int& X, const int& Y, const int& Width, const int& Height);
    void MouseWheel(const int& Degree, const int& MgGlassKeyWds);

    // select mode
    void SetZoomMode() { SelMode = vmsmZoom; SelPointV.Clr(); }
    bool IsZoomMode() { return (SelMode == vmsmZoom); }
    void SetSelectMode() { SelMode = vmsmSelect; SelPointV.Clr(); }
    bool IsSelectMode() { return (SelMode == vmsmSelect); }

    // zoom controls
    void ZoomIn(const double& X1, const double& Y1, const double& X2, const double& Y2);
    void ZoomIn(const TFltRect& ZoomInRect);
    bool ZoomOut();
    bool ZoomAll();

    // select controls
    void Select(const double& X1, const double& Y1, const double& X2, const double& Y2);
    void Select(const TFltRect& SelRect);
    void Select(const int& DocId);
    void GetSelectDIdV(TIntV& DIdV);
    void UnselectAll();

    // document interaction
    int GetDocs() const { return VizMapFrame->GetPoints(); }
    TStr GetDocNm(const int DId) { return VizMapFrame->GetPointBow()->GetDocNm(DId); }
    bool IsSelDoc() { return !SelPointV.Empty(); }
    void SelectDoc(const double& DocX, const double& DocY);
    int SelDocId() { return VizMapFrame->GetDocId(SelPointV[0]); }
    TStr SelDocNm() { return VizMapFrame->GetDocNm(SelPointV[0]); }
    TStr SelDocBody() { return VizMapFrame->GetDocBody(SelPointV[0]); }
    TStr SelDocUrl() { return VizMapFrame->GetDocUrl(SelPointV[0]); }
};

#endif