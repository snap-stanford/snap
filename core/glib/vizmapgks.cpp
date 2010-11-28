#include "vizmapgks.h"
#include <gkswf.h>

//PSysConsole SysConsole = TSysConsole::New();

TGksColor TVizMapContext::ColorLandscapeAbove = TGksColor(54, 92, 120);
TGksColor TVizMapContext::ColorAddFirstLevel = TGksColor(9, 7, 6);
TGksColor TVizMapContext::ColorAddSecondLevel = TGksColor(18, 14, 12);
TGksColor TVizMapContext::ColorLegendGridColor = TGksColor(192, 255, 255, 255);
TGksColor TVizMapContext::ColorPointCross = TGksColor(255, 220, 0);
TGksColor TVizMapContext::ColorSelPointCross = TGksColor(255, 145, 145);
TGksColor TVizMapContext::ColorPointFont = TGksColor(255, 220, 0);
TGksColor TVizMapContext::ColorSelPointFont = TGksColor(255, 255, 30);
TGksColor TVizMapContext::ColorLink = TGksColor(100, 100, 255, 100);
TGksColor TVizMapContext::ColorKeyWdFont = TGksColor(240,240,255);
TGksColor TVizMapContext::ColorCatNmFont = TGksColor(255,240,255);
TGksColor TVizMapContext::ColorZoomRect = TGksColor(100, 0, 0, 0); 
TGksColor TVizMapContext::ColorMgGlass = TGksColor(50, 0, 0, 0);
TGksColor TVizMapContext::ColorMgGlassWnd = TGksColor(192, 255, 255, 255);
TGksColor TVizMapContext::ColorMgGlassWndFrm = TGksColor(100, 0, 0, 0); 
TGksColor TVizMapContext::ColorMgGlassWndShadow = TGksColor(50, 0, 0, 0);

int TVizMapContext::SizePointCross = 2;
int TVizMapContext::PointNmWidth = 100;
int TVizMapContext::PointNmMxLines = 3;
double TVizMapContext::PointNmFontScale = 20.0;
int TVizMapContext::MgGlassWindowWidth = 150;
int TVizMapContext::RndKeyWdN = 3000;
double TVizMapContext::RelRadius = 0.2;
int TVizMapContext::KeyWdCandidates = 10;
double TVizMapContext::RelMnRptDist = 0.1;
double TVizMapContext::RelSparsity = 0.05;

void TVizMapContext::LoadColor() {
    LandscapeCatColorV.Clr();

    LandscapeCatColorV.Add(TGksColor(43, 76, 115));
    LandscapeCatColorV.Add(TGksColor(53, 46, 115));
    LandscapeCatColorV.Add(TGksColor(33, 106, 115));
    LandscapeCatColorV.Add(TGksColor(63, 80, 170));

    LandscapeCatColorV.Add(TGksColor(61, 106, 200));
    LandscapeCatColorV.Add(TGksColor(71, 106, 190));
    LandscapeCatColorV.Add(TGksColor(51, 106, 210));
    LandscapeCatColorV.Add(TGksColor(81, 106, 180));

    LandscapeCatColorV.Add(TGksColor(43, 91, 125));
    LandscapeCatColorV.Add(TGksColor(43, 91, 135));
    LandscapeCatColorV.Add(TGksColor(43, 91, 145));
    LandscapeCatColorV.Add(TGksColor(43, 91, 155));

    LandscapeCatColorV.Add(TGksColor(43, 106, 125));
    LandscapeCatColorV.Add(TGksColor(43, 106, 135));
    LandscapeCatColorV.Add(TGksColor(43, 106, 145));
    LandscapeCatColorV.Add(TGksColor(43, 106, 155));
}

void TVizMapContext::LoadCatFullNms() {
    CatFullNmH.AddDat("1POL", "CURRENT NEWS - POLITICS");
    CatFullNmH.AddDat("2ECO", "CURRENT NEWS - ECONOMICS");
    CatFullNmH.AddDat("3SPO", "CURRENT NEWS - SPORT");
    CatFullNmH.AddDat("4GEN", "CURRENT NEWS - GENERAL");
    CatFullNmH.AddDat("6INS", "CURRENT NEWS - INSURANCE");
    CatFullNmH.AddDat("7RSK", "CURRENT NEWS - RISK NEWS");
    CatFullNmH.AddDat("8YDB", "TEMPORARY");
    CatFullNmH.AddDat("9BNX", "TEMPORARY");
    CatFullNmH.AddDat("ADS10", "CURRENT NEWS - ADVERTISING");
    CatFullNmH.AddDat("BNW14", "CURRENT NEWS - BUSINESS NEWS");
    CatFullNmH.AddDat("BRP11", "CURRENT NEWS - BRANDS");
    CatFullNmH.AddDat("C11", "STRATEGY/PLANS");
    CatFullNmH.AddDat("C12", "LEGAL/JUDICIAL");
    CatFullNmH.AddDat("C13", "REGULATION/POLICY");
    CatFullNmH.AddDat("C14", "SHARE LISTINGS");
    CatFullNmH.AddDat("C15", "PERFORMANCE");
    CatFullNmH.AddDat("C151", "ACCOUNTS/EARNINGS");
    CatFullNmH.AddDat("C1511", "ANNUAL RESULTS");
    CatFullNmH.AddDat("C152", "COMMENT/FORECASTS");
    CatFullNmH.AddDat("C16", "INSOLVENCY/LIQUIDITY");
    CatFullNmH.AddDat("C17", "FUNDING/CAPITAL");
    CatFullNmH.AddDat("C171", "SHARE CAPITAL");
    CatFullNmH.AddDat("C172", "BONDS/DEBT ISSUES");
    CatFullNmH.AddDat("C173", "LOANS/CREDITS");
    CatFullNmH.AddDat("C174", "CREDIT RATINGS");
    CatFullNmH.AddDat("C18", "OWNERSHIP CHANGES");
    CatFullNmH.AddDat("C181", "MERGERS/ACQUISITIONS");
    CatFullNmH.AddDat("C182", "ASSET TRANSFERS");
    CatFullNmH.AddDat("C183", "PRIVATISATIONS");
    CatFullNmH.AddDat("C21", "PRODUCTION/SERVICES");
    CatFullNmH.AddDat("C22", "NEW PRODUCTS/SERVICES");
    CatFullNmH.AddDat("C23", "RESEARCH/DEVELOPMENT");
    CatFullNmH.AddDat("C24", "CAPACITY/FACILITIES");
    CatFullNmH.AddDat("C31", "MARKETS/MARKETING");
    CatFullNmH.AddDat("C311", "DOMESTIC MARKETS");
    CatFullNmH.AddDat("C312", "EXTERNAL MARKETS");
    CatFullNmH.AddDat("C313", "MARKET SHARE");
    CatFullNmH.AddDat("C32", "ADVERTISING/PROMOTION");
    CatFullNmH.AddDat("C33", "CONTRACTS/ORDERS");
    CatFullNmH.AddDat("C331", "DEFENCE CONTRACTS");
    CatFullNmH.AddDat("C34", "MONOPOLIES/COMPETITION");
    CatFullNmH.AddDat("C41", "MANAGEMENT");
    CatFullNmH.AddDat("C411", "MANAGEMENT MOVES");
    CatFullNmH.AddDat("C42", "LABOUR");
    CatFullNmH.AddDat("CCAT", "CORPORATE/INDUSTRIAL");
    CatFullNmH.AddDat("E11", "ECONOMIC PERFORMANCE");
    CatFullNmH.AddDat("E12", "MONETARY/ECONOMIC");
    CatFullNmH.AddDat("E121", "MONEY SUPPLY");
    CatFullNmH.AddDat("E13", "INFLATION/PRICES");
    CatFullNmH.AddDat("E131", "CONSUMER PRICES");
    CatFullNmH.AddDat("E132", "WHOLESALE PRICES");
    CatFullNmH.AddDat("E14", "CONSUMER FINANCE");
    CatFullNmH.AddDat("E141", "PERSONAL INCOME");
    CatFullNmH.AddDat("E142", "CONSUMER CREDIT");
    CatFullNmH.AddDat("E143", "RETAIL SALES");
    CatFullNmH.AddDat("E21", "GOVERNMENT FINANCE");
    CatFullNmH.AddDat("E211", "EXPENDITURE/REVENUE");
    CatFullNmH.AddDat("E212", "GOVERNMENT BORROWING");
    CatFullNmH.AddDat("E31", "OUTPUT/CAPACITY");
    CatFullNmH.AddDat("E311", "INDUSTRIAL PRODUCTION");
    CatFullNmH.AddDat("E312", "CAPACITY UTILIZATION");
    CatFullNmH.AddDat("E313", "INVENTORIES");
    CatFullNmH.AddDat("E41", "EMPLOYMENT/LABOUR");
    CatFullNmH.AddDat("E411", "UNEMPLOYMENT");
    CatFullNmH.AddDat("E51", "TRADE/RESERVES");
    CatFullNmH.AddDat("E511", "BALANCE OF PAYMENTS");
    CatFullNmH.AddDat("E512", "MERCHANDISE TRADE");
    CatFullNmH.AddDat("E513", "RESERVES");
    CatFullNmH.AddDat("E61", "HOUSING STARTS");
    CatFullNmH.AddDat("E71", "LEADING INDICATORS");
    CatFullNmH.AddDat("ECAT", "ECONOMICS");
    CatFullNmH.AddDat("ENT12", "CURRENT NEWS - ENTERTAINMENT");
    CatFullNmH.AddDat("G11", "SOCIAL AFFAIRS");
    CatFullNmH.AddDat("G111", "HEALTH/SAFETY");
    CatFullNmH.AddDat("G112", "SOCIAL SECURITY");
    CatFullNmH.AddDat("G113", "EDUCATION/RESEARCH");
    CatFullNmH.AddDat("G12", "INTERNAL POLITICS");
    CatFullNmH.AddDat("G13", "INTERNATIONAL RELATIONS");
    CatFullNmH.AddDat("G131", "DEFENCE");
    CatFullNmH.AddDat("G14", "ENVIRONMENT");
    CatFullNmH.AddDat("G15", "EUROPEAN COMMUNITY");
    CatFullNmH.AddDat("G151", "EC INTERNAL MARKET");
    CatFullNmH.AddDat("G152", "EC CORPORATE POLICY");
    CatFullNmH.AddDat("G153", "EC AGRICULTURE POLICY");
    CatFullNmH.AddDat("G154", "EC MONETARY/ECONOMIC");
    CatFullNmH.AddDat("G155", "EC INSTITUTIONS");
    CatFullNmH.AddDat("G156", "EC ENVIRONMENT ISSUES");
    CatFullNmH.AddDat("G157", "EC COMPETITION/SUBSIDY");
    CatFullNmH.AddDat("G158", "EC EXTERNAL RELATIONS");
    CatFullNmH.AddDat("G159", "EC GENERAL");
    CatFullNmH.AddDat("GCAT", "GOVERNMENT/SOCIAL");
    CatFullNmH.AddDat("GCRIM", "CRIME, LAW ENFORCEMENT");
    CatFullNmH.AddDat("GDEF", "DEFENCE");
    CatFullNmH.AddDat("GDIP", "INTERNATIONAL RELATIONS");
    CatFullNmH.AddDat("GDIS", "DISASTERS AND ACCIDENTS");
    CatFullNmH.AddDat("GEDU", "EDUCATION");
    CatFullNmH.AddDat("GENT", "ARTS, CULTURE, ENTERTAINMENT");
    CatFullNmH.AddDat("GENV", "ENVIRONMENT AND NATURAL WORLD");
    CatFullNmH.AddDat("GFAS", "FASHION");
    CatFullNmH.AddDat("GHEA", "HEALTH");
    CatFullNmH.AddDat("GJOB", "LABOUR ISSUES");
    CatFullNmH.AddDat("GMIL", "MILLENNIUM ISSUES");
    CatFullNmH.AddDat("GOBIT", "OBITUARIES");
    CatFullNmH.AddDat("GODD", "HUMAN INTEREST");
    CatFullNmH.AddDat("GPOL", "DOMESTIC POLITICS");
    CatFullNmH.AddDat("GPRO", "BIOGRAPHIES, PERSONALITIES, PEOPLE");
    CatFullNmH.AddDat("GREL", "RELIGION");
    CatFullNmH.AddDat("GSCI", "SCIENCE AND TECHNOLOGY");
    CatFullNmH.AddDat("GSPO", "SPORTS");
    CatFullNmH.AddDat("GTOUR", "TRAVEL AND TOURISM");
    CatFullNmH.AddDat("GVIO", "WAR, CIVIL WAR");
    CatFullNmH.AddDat("GVOTE", "ELECTIONS");
    CatFullNmH.AddDat("GWEA", "WEATHER");
    CatFullNmH.AddDat("GWELF", "WELFARE, SOCIAL SERVICES");
    CatFullNmH.AddDat("M11", "EQUITY MARKETS");
    CatFullNmH.AddDat("M12", "BOND MARKETS");
    CatFullNmH.AddDat("M13", "MONEY MARKETS");
    CatFullNmH.AddDat("M131", "INTERBANK MARKETS");
    CatFullNmH.AddDat("M132", "FOREX MARKETS");
    CatFullNmH.AddDat("M14", "COMMODITY MARKETS");
    CatFullNmH.AddDat("M141", "SOFT COMMODITIES");
    CatFullNmH.AddDat("M142", "METALS TRADING");
    CatFullNmH.AddDat("M143", "ENERGY MARKETS");
    CatFullNmH.AddDat("MCAT", "MARKETS");
    CatFullNmH.AddDat("MEUR", "EURO CURRENCY");
    CatFullNmH.AddDat("PRB13", "CURRENT NEWS - PRESS RELEASE WIRES");
}

TFltRect TVizMapContext::GetZoomRect() {
    if (!ZoomS.Empty()) {
        return ZoomS.Top();
    } else {
        return TFltRect(0.0, 0.0, 1.0, 1.0);
    }
}

void TVizMapContext::RenderLandscapeBmp() {
    if (SelLandscape.Empty()) return;

    TFltRect ZoomRect = GetZoomRect();
    const int MaxWidth = SelLandscape->GetXDim();
    const int MaxHeight = SelLandscape->GetYDim();
    const int Width = TFlt::Round(ZoomRect.GetXLen()*MaxWidth);
    const int Height = TFlt::Round(ZoomRect.GetYLen()*MaxHeight);
    const int LeftX = TFlt::Round(ZoomRect.GetMnX()*MaxWidth);
    const int TopY = TFlt::Round(ZoomRect.GetMnY()*MaxHeight);

    if (Width > 0 && Height > 0) {        
        const TSFltVV& HeightVV = SelLandscape->HeightVV();
        // do we have background gradient or color coding?
        //if (!SelLandscape->IsCatVV()) {
            // get limits for isohipses
            Assert(SelLandscape->GetLevels() == 2);
            const double First = SelLandscape->GetLevel(0);
            const double Second = SelLandscape->GetLevel(1);
            // generate bitmap with releief
            LandscapeBmp = TGksBitmap::New(Width, Height, ColorLandscapeAbove);
            for (int X = 0; X < Width; X++) {
                for (int Y = 0; Y < Height; Y++) {
                    double Val = HeightVV(LeftX+X, TopY+Y);
                    if (Val > Second) { Val = Second + 3*(Val - Second); }
                    TGksColor PixelColor = ColorLandscapeAbove.Add(
                        TFlt::Round(Val*34), TFlt::Round(Val*28), TFlt::Round(Val*24));
                    if (Val > Second) { PixelColor = PixelColor.Add(ColorAddSecondLevel); } 
                    if (Val > First) { PixelColor = PixelColor.Add(ColorAddFirstLevel); }
                    LandscapeBmp->SetPixel(X, Y, PixelColor);
                }
            }
        //} else {
        //    const TIntVV& CatVV = SelLandscape->CatVV();
        //    LandscapeBmp = TGksBitmap::New(Width, Height, TGksColor::GetWhite());
        //    const int Colors = LandscapeCatColorV.Len() - 1;
        //    for (int X = 0; X < Width; X++) {
        //        for (int Y = 0; Y < Height; Y++) {
        //            const double Val = HeightVV(LeftX+X, TopY+Y);
        //            const int CatId = CatVV(LeftX+X, TopY+Y) % Colors + 1;
        //            TGksColor PixelColor = LandscapeCatColorV[0];
        //            if (CatId != -1) { PixelColor = LandscapeCatColorV[CatId]; }
        //            LandscapeBmp->SetPixel(X, Y, PixelColor);
        //        }            
        //    }
        //}
    } else {
        LandscapeBmp = TGksBitmap::New(1, 1, ColorLandscapeAbove);
    }
}

void TVizMapContext::ResetMgGlassSize() { 
    TFltRect ZoomRect = GetZoomRect();
    MgGlassSize = 0.2 * TFlt::GetMn(ZoomRect.GetXLen(), ZoomRect.GetYLen()); 
}

void TVizMapContext::ResetMgGlassKeyWd(const int& MgGlassKeyWds) {
    // get top TopKeyWdN keywords
    MgGlassPoints = VizMapFrame->GenKeyWd(TFltV::GetV(MouseX, MouseY), 
        MgGlassSize, MgGlassKeyWds, MgGlassKeyWdV, false, false);
}

void TVizMapContext::ResetKeyWd() {
    // get parameters
    TFltRect ZoomRect = GetZoomRect();
    const double MnZoomRectSize = TFlt::GetMn(ZoomRect.GetXLen(), ZoomRect.GetYLen());
    const double Radius = RelRadius * MnZoomRectSize; 
    // get random keywords
    VizMapFrame->GenKeyWds(RndKeyWdN, Radius, KeyWdCandidates, 0, ZoomRect); 
}

void TVizMapContext::ResetNearPointN() {
    double MnDist = TFlt::Mx;
    NearPointN = -1;
    int Points = VizMapFrame->GetPoints();
    for (int PointN = 0; PointN < Points; PointN++) {
        PVizMapPoint Point = VizMapFrame->GetPoint(PointN);
        const double PointX = Point->GetPointX(), PointY = Point->GetPointY();
        const double Dist = TMath::Sqr(PointX - MouseX) + 
            TMath::Sqr(PointY - MouseY);
        if (Dist < MnDist) { MnDist = Dist; NearPointN = PointN; }
    }
    if (TMath::Sqrt(MnDist) > 0.1) { NearPointN = -1; }
}

inline double TVizMapContext::GetMapCoord(const int& ScreenCoord, 
        const int& ScreenLen, const double& MapMn, const double& MapLen) {
    return MapMn + MapLen*(double(ScreenCoord)/double(ScreenLen));
}

inline int TVizMapContext::GetScreenCoord(const double& MapCoord, const double& MapMn, 
        const double& MapLen, const int& ScreenLen) {
    return TFlt::Round(ScreenLen * ((MapCoord - MapMn)/MapLen));
}

int TVizMapContext::GetEncoderClsid(const TStr& EncoderType, CLSID* pClsid) {
    // convert EncoderType to WCHAR*
    const int StrLen = EncoderType.Len() + 1;
    WCHAR* format = new WCHAR[StrLen];
    const int Res = MultiByteToWideChar(CP_ACP, 0, 
        EncoderType.CStr(), StrLen, format, StrLen);

    UINT  num = 0;          // number of image encoders
    UINT  size = 0;         // size of the image encoder array in bytes

    Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0) { return -1; } // Failure

    pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if(pImageCodecInfo == NULL) { return -1; } // Failure

    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

    for(UINT j = 0; j < num; ++j) {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;  // Success
        }    
    }

    free(pImageCodecInfo);
    return -1;  // Failure
}

void TVizMapContext::ResetFrame() {
    if (VizMapFrame->GetLandscapes() > 0) {
        SelLandscape = VizMapFrame->GetLandscape(0); 
        RenderLandscapeBmp();
    }
    ResetKeyWd(); SelPointV.Clr();
}

TVizMapContext::TVizMapContext(PVizMap _VizMap): 
        VizMap(_VizMap), VizMapFrameN(0),
        VizMapFrame(_VizMap->GetFirst()), SelMode(vmsmZoom), 
        MgGlassPoints(0), MouseInP(false), LeftButtonDownP(false), 
        MgGlassSize(0.2), NearPointN(-1) { 

    LoadColor();
    LoadCatFullNms();
    ResetFrame();
}

void TVizMapContext::SetVizMapFrame(const int& NewVizMapFrameN) {
    VizMapFrameN = NewVizMapFrameN;
    VizMapFrame = VizMap->GetVizMapFrame(VizMapFrameN);
    ResetFrame();
}

void TVizMapContext::SaveVrml(const TStr& VrmlFNm, bool ShowPointNmP, bool ShowKeyWdP) {    
    VizMapFrame->SaveVrml(VrmlFNm, SelLandscape, ShowPointNmP, !ShowPointNmP,  ShowKeyWdP);
}

void TVizMapContext::PaintEmpty(PGks Gks) {
    Gks->SetBrush(TGksBrush::New(ColorLandscapeAbove));
    Gks->FillRect(-1, -1, Gks->GetWidth()+1, Gks->GetHeight()+1);
}

void TVizMapContext::Paint(PGks Gks, const bool& ShowPointNmP, const int& PointFontSize,
        const int& PointNmFontScale, const double& PointWgtThreshold, const int& CatId,
        const bool& ShowCatNmP, const bool& ShowKeyWdP, const int& KeyWdFontSize, 
        const bool& ShowMgGlassP, const int& LegendGridWidth, const int& LegendGridHeight) {

    // background
    if (!SelLandscape.Empty()) { PaintLandscape(Gks); } 
    else { PaintEmpty(Gks); }
    // legend grid
    PaintLegendGrid(Gks, LegendGridWidth, LegendGridHeight);
    // links
    PaintLinks(Gks);
    //documents and keywords
    TVec<TFltRect> PointNmRectV;
    if (ShowPointNmP) { 
        PaintPoints(Gks, PointFontSize, PointNmFontScale, 
            PointWgtThreshold, CatId, ShowMgGlassP, PointNmRectV); 
    } else { 
        PaintPoints(Gks, PointFontSize, PointNmFontScale, 
            TFlt::Mx, CatId, ShowMgGlassP, PointNmRectV); 
    }
    if (ShowCatNmP && !SelLandscape.Empty()) { 
        PaintCatNms(Gks, KeyWdFontSize, PointNmRectV); };
    if (ShowKeyWdP) { PaintKeyWds(Gks, KeyWdFontSize, PointNmRectV); }
    // mouse interactions
    if (LeftButtonDownP) { PaintZoomRect(Gks); }
    else if (MouseInP && ShowMgGlassP) { PaintMgGlass(Gks, KeyWdFontSize); }
}

void TVizMapContext::PaintLandscape(PGks Gks){
    Gks->Bitmap(LandscapeBmp, -1, -1, Gks->GetWidth()+1, Gks->GetHeight()+1);
}

void TVizMapContext::PaintLegendGrid(PGks Gks, 
        const int& LegendGridWidth, const int& LegendGridHeight) {

    Gks->SetPen(TGksPen::New(ColorLegendGridColor));
    for (int LineN = 1; LineN < LegendGridWidth; LineN++) {
        const double RelativeWidth = double(LineN) / double(LegendGridWidth);
        const int X = TFlt::Round(RelativeWidth * Gks->GetWidth());
        Gks->Line(X, -1, X, Gks->GetHeight());
    }
    for (int LineN = 1; LineN < LegendGridHeight; LineN++) {
        const double RelativeHeight = double(LineN) / double(LegendGridHeight);
        const int Y = TFlt::Round(RelativeHeight * Gks->GetHeight());
        Gks->Line(-1, Y, Gks->GetWidth(), Y);
    }

}

void TVizMapContext::PaintLinks(PGks Gks) {
    int Links = VizMapFrame->GetLinks();
    //InfoNotify(TStr::Fmt("Links %d", Links));
    TFltRect ZoomRect = GetZoomRect();
    for (int LinkN = 0; LinkN < Links; LinkN++) {
        PVizMapLink Link = VizMapFrame->GetLink(LinkN);
        //if (!SelPointV.IsInBin(Link->GetSrcPointN()) &&
        //    !SelPointV.IsInBin(Link->GetDstPointN())) { continue; }
        // get link coordinates
        PVizMapPoint SrcPoint = VizMapFrame->GetPoint(Link->GetSrcPointN());
        PVizMapPoint DstPoint = VizMapFrame->GetPoint(Link->GetDstPointN());
        const double SrcPointX = SrcPoint->GetPointX(), SrcPointY = SrcPoint->GetPointY();
        const double DstPointX = DstPoint->GetPointX(), DstPointY = DstPoint->GetPointY();
        //TODO: check if link crosses ZoomRect
        // get coordinates in pixels
        const int SrcX = GetScreenCoord(SrcPointX, ZoomRect.GetMnX(), 
            ZoomRect.GetXLen(), Gks->GetWidth());
        const int SrcY = GetScreenCoord(SrcPointY, ZoomRect.GetMnY(), 
            ZoomRect.GetYLen(), Gks->GetHeight());
        const int DstX = GetScreenCoord(DstPointX, ZoomRect.GetMnX(), 
            ZoomRect.GetXLen(), Gks->GetWidth());
        const int DstY = GetScreenCoord(DstPointY, ZoomRect.GetMnY(), 
            ZoomRect.GetYLen(), Gks->GetHeight());
        // draw arrow
        PaintArrow(Gks, SrcX, SrcY, DstX, DstY, 3);
    }
}

void TVizMapContext::PaintArrow(PGks Gks, const int& SrcX, const int& SrcY, 
        const int& DstX, const int& DstY, const int& Width) {

    Gks->SetPen(TGksPen::New(ColorLink, gpmCopy, gpsSolid, Width));
    Gks->Line(SrcX, SrcY, DstX, DstY);

    // calculate edges of the head
    TFltV DirectionV = TFltV::GetV(DstX - SrcX, DstY - SrcY); 
    TLinAlg::Normalize(DirectionV);
    const int HeadSize = TFlt::Round(2.5*Width); 
    const double AngleRad = 0.75*TMath::Pi;
    const int dX1 = TFlt::Round(HeadSize*DirectionV[0]);
    const int dY1 = TFlt::Round(HeadSize*DirectionV[1]);
    double dxx, dyy;
    TLinAlg::Rotate(DirectionV[0], DirectionV[1], AngleRad, dxx, dyy);
    const int dX2 = TFlt::Round(HeadSize*dxx);
    const int dY2 = TFlt::Round(HeadSize*dyy);
    TLinAlg::Rotate(DirectionV[0], DirectionV[1], -AngleRad, dxx, dyy);
    const int dX3 = TFlt::Round(HeadSize*dxx);
    const int dY3 = TFlt::Round(HeadSize*dyy);
    // Create points that define polygon.
    TGksSPointV HeadPointV(3, 0);
    HeadPointV.Add(TGksSPoint(DstX, DstY));
    HeadPointV.Add(TGksSPoint(DstX-dX1+dX2, DstY-dY1+dY2));
    HeadPointV.Add(TGksSPoint(DstX-dX1+dX3, DstY-dY1+dY3)); 
    // draw polygon
    Gks->SetBrush(TGksBrush::New(ColorLink));
    Gks->Polygon(HeadPointV);
}

void TVizMapContext::PaintPoints(PGks Gks, const int& PointFontSize, 
        const int& PointNmFontScale, const double& PointWgtThreshold, 
        const int& CatId, const bool& ShowMgGlassP,
        TVec<TFltRect>& PointNmRectV) {

    int Points = VizMapFrame->GetPoints();
    TFltRect ZoomRect = GetZoomRect();
    for (int PointN = 0; PointN < Points; PointN++) {
        PVizMapPoint Point = VizMapFrame->GetPoint(PointN);
        // we ignore selected and nearest point in the first run
        bool IsSelPointP = SelPointV.IsInBin(PointN);
        if (IsSelPointP || ((NearPointN == PointN) && !ShowMgGlassP)) { continue; }
        const double PointX = Point->GetPointX(), PointY = Point->GetPointY();
        if (ZoomRect.IsXYIn(PointX, PointY)) {
            // get coordinates in pixels
            const int X = GetScreenCoord(PointX, ZoomRect.GetMnX(), 
                ZoomRect.GetXLen(), Gks->GetWidth());
            const int Y = GetScreenCoord(PointY, ZoomRect.GetMnY(), 
                ZoomRect.GetYLen(), Gks->GetHeight());
            // check if point has given category
            bool IsCatP = Point->IsCatId(CatId);
            // check if the point is under threshold
            if (Point->IsPointNm() && 
                ((Point->GetWgt() > PointWgtThreshold) || IsSelPointP)) {

                // write full point name
                PointNmRectV.Add(PaintPointNm(Gks, Point, X, Y, 
                    PointFontSize, PointNmFontScale, IsSelPointP, IsCatP));
            } else {
                // draw a cross
                PaintPointCross(Gks, X, Y, IsSelPointP, IsCatP);
            }
        }
    }
    // paint selected points
    for (int SelPointN = 0; SelPointN < SelPointV.Len(); SelPointN++) {
        const int PointN = SelPointV[SelPointN];
        if ((NearPointN == PointN) && !ShowMgGlassP) { continue; }
        PVizMapPoint Point = VizMapFrame->GetPoint(PointN);
        const double PointX = Point->GetPointX(), PointY = Point->GetPointY();
        if (ZoomRect.IsXYIn(PointX, PointY)) {
            // get coordinates in pixels
            const int X = GetScreenCoord(PointX, ZoomRect.GetMnX(), 
                ZoomRect.GetXLen(), Gks->GetWidth());
            const int Y = GetScreenCoord(PointY, ZoomRect.GetMnY(), 
                ZoomRect.GetYLen(), Gks->GetHeight());
            // check if point has given category
            bool IsCatP = Point->IsCatId(CatId);
            // check if the point is under threshold
            if (Point->IsPointNm()) {
                // write full point name
                PointNmRectV.Add(PaintPointNm(Gks, Point, X, Y, 
                    PointFontSize, PointNmFontScale, true, IsCatP));
            } else {
                // draw a cross
                PaintPointCross(Gks, X, Y, true, IsCatP);
            }
        }
    }
    // paint nearest point
    if (!ShowMgGlassP && (NearPointN != -1)) {
        PVizMapPoint Point = VizMapFrame->GetPoint(NearPointN);
        const double PointX = Point->GetPointX(), PointY = Point->GetPointY();
        if (ZoomRect.IsXYIn(PointX, PointY)) {
            // get coordinates in pixels
            const int X = GetScreenCoord(PointX, ZoomRect.GetMnX(), 
                ZoomRect.GetXLen(), Gks->GetWidth());
            const int Y = GetScreenCoord(PointY, ZoomRect.GetMnY(), 
                ZoomRect.GetYLen(), Gks->GetHeight());
            // check if point has given category
            bool IsCatP = Point->IsCatId(CatId);
            // check if point is selected
            bool IsSelPointP = SelPointV.IsInBin(NearPointN);
            // check if the point is under threshold
            if (Point->IsPointNm()) {
                // write full point name
                PointNmRectV.Add(PaintPointNm(Gks, Point, X, Y, 
                    PointFontSize, PointNmFontScale, IsSelPointP, IsCatP));
            } else {
                // draw a cross
                PaintPointCross(Gks, X, Y, IsSelPointP, IsCatP);
            }
        }
    }
}

void TVizMapContext::PaintPointCross(PGks Gks, const int& X, 
        const int& Y, const bool& SelPointP, const bool& IsCatP) {

    // is point selected or not
    Gks->SetPen(TGksPen::New(SelPointP ? ColorSelPointCross : ColorPointCross));
    // draw a cross
    Gks->Line(X - SizePointCross, Y - SizePointCross, 
              X + SizePointCross, Y + SizePointCross);
    Gks->Line(X - SizePointCross, Y + SizePointCross, 
              X + SizePointCross, Y - SizePointCross);
    // make bold if cat
    if (IsCatP) {
        Gks->Line(X - SizePointCross + 1, Y - SizePointCross, 
                  X + SizePointCross + 1, Y + SizePointCross);
        Gks->Line(X - SizePointCross + 1, Y + SizePointCross, 
                  X + SizePointCross + 1, Y - SizePointCross);
    }
}

TFltRect TVizMapContext::PaintPointNm(PGks Gks, PVizMapPoint Point, const int& X, 
        const int& Y, const int& PointFontSize, const int& PointNmFontScale, 
        const bool& SelPointP, const bool& IsCatP) {

    // get and clean point name
    TStr PointNm = Point->GetPointNm(); 
    PointNm.ChangeChAll('_', ' ');
    if (PointNm.IsStrIn("[[")) {
        const int StartPos = PointNm.SearchStr("[[");
        PointNm = PointNm.Left(StartPos - 1);
    }
    // set font
    TGksColor FontColor = SelPointP ? ColorSelPointFont : ColorPointFont;
    const int FontSize = PointFontSize + TFlt::Round(Point->GetWgt()*PointNmFontScale);
    //TFSet FontStyle = IsCatP ? (TFSet() | gfsBold) : TFSet();
    //Gks->SetFont(TGksFont::New("ARIAL", FontSize, FontColor, FontStyle));
    Gks->SetFont(TGksFont::New("ARIAL", FontSize, FontColor));
    // refit it for the screen
    TStr ScreenPointNm = Gks->BreakTxt(PointNm, " ", "", PointNmWidth, PointNmMxLines);
    // calculate string position on the screen
    const int HalfTxtWidth = Gks->GetTxtWidth(ScreenPointNm) / 2;
    const int HalfTxtHeight = Gks->GetTxtHeight(ScreenPointNm) / 2;
     // draw it!
    const int MnX = X - HalfTxtWidth;
    int CurrY = Y - HalfTxtHeight;
    TStrV LineV; ScreenPointNm.SplitOnAllCh('\n', LineV);
    for (int LineN = 0; LineN < LineV.Len(); LineN++) {
        const int HalfLineWidth = Gks->GetTxtWidth(LineV[LineN]) / 2;
        const int LineHeight = Gks->GetTxtHeight(LineV[LineN]);
        Gks->PutTxt(LineV[LineN], MnX + (HalfTxtWidth - HalfLineWidth), CurrY);
        CurrY += LineHeight-3;
    }
    // finish
    return TFltRect(X - HalfTxtWidth, Y - HalfTxtHeight,
        X + HalfTxtWidth, Y + HalfTxtHeight - LineV.Len()*3);
}

void TVizMapContext::PaintCatNms(PGks Gks, const int& KeyWdFontSize, 
        TVec<TFltRect>& PointNmRectV) {
        
    // calculate frequency of categories
    TIntH CatH; TIntFltPrH CatPosH;
    PBowDocBs BowDocBs = VizMapFrame->GetKeyWdBow();
    const int Points = VizMapFrame->GetPoints();
    for (int PointN = 0; PointN < Points; PointN++) {
        PVizMapPoint Point = VizMapFrame->GetPoint(PointN);
        const int DId = Point->GetDocId();
        const int CIds = BowDocBs->GetDocCIds(DId);
        for (int CIdN = 0; CIdN < CIds; CIdN++) {
            const int CId = BowDocBs->GetDocCId(DId, CIdN);
            CatH.AddDat(CId)++;
            CatPosH.AddDat(CId).Val1 += Point->GetPointX();
            CatPosH.AddDat(CId).Val2 += Point->GetPointY();
        }
        
    }
    CatH.SortByDat(false); 

    // draw the top cats
    const int TopCats = Points > 100 ? 6 : 4; 
    TFltRect ZoomRect = GetZoomRect();    
    Gks->SetFont(TGksFont::New("ARIAL", KeyWdFontSize + 3, ColorCatNmFont));
    TVec<TFltRect> CatNmRectV; TVec<TFltV> CatNmPosV;
    const int MnSize = TInt::GetMn(Gks->GetWidth(), Gks->GetHeight());
    const int MnDist = TFlt::Round(0.3 * double(MnSize));
    int Cats = 0, CatKeyId = CatH.FFirstKeyId();
    while (CatH.FNextKeyId(CatKeyId)) {
        if (Cats == TopCats) { break; } 
        if (double(CatH[CatKeyId]) / double(Points) < 0.05) { break; } 
        const int CId = CatH.GetKey(CatKeyId);
        // get name
        TStr CatNm = BowDocBs->GetCatNm(CId);
        if (CatFullNmH.IsKey(CatNm)) {
            CatNm = CatFullNmH.GetDat(CatNm);
        } else { continue; }
        // get position
        TFltPr CatPos = CatPosH.GetDat(CId);
        const int CatCount = CatH.GetDat(CId); IAssert(CatCount > 0);
        const double CatX = CatPos.Val1 / double(CatCount);
        const double CatY = CatPos.Val2 / double(CatCount);
        // is it within the zoom?
        if (!ZoomRect.IsXYIn(CatX, CatY)) { continue; }
        // calculate string size on the screen
        const int HalfTxtWidth = Gks->GetTxtWidth(CatNm) / 2;
        const int HalfTxtHeight = Gks->GetTxtHeight(CatNm) / 2;
        // get coordinates in pixels
        const int X = GetScreenCoord(CatX , ZoomRect.GetMnX(), 
            ZoomRect.GetXLen(), Gks->GetWidth());
        const int Y = GetScreenCoord(CatY, ZoomRect.GetMnY(), 
            ZoomRect.GetYLen(), Gks->GetHeight());
        // is it to close to any of the most prominent categories
        int CatNmDist = MnSize; TFltV CatNmPos = TFltV::GetV(double(X), double(Y));
        for (int CatNmPosN = 0; CatNmPosN < CatNmPosV.Len(); CatNmPosN++) {
            const double Dist = TLinAlg::EuclDist(CatNmPosV[CatNmPosN], CatNmPos);
            CatNmDist = TInt::GetMn(TFlt::Round(Dist), CatNmDist);
        }
        if (CatNmDist < MnDist) { continue; }
        // does it overlap with any of hte most prominent categories
        TFltRect CatNmRect(X - HalfTxtWidth, Y - HalfTxtHeight,
            X + HalfTxtWidth, Y + HalfTxtHeight);
        bool DoDraw = true; const int Rects = CatNmRectV.Len();
        for (int RectN = 0; (RectN < Rects) && DoDraw; RectN++) {
            DoDraw = !TFltRect::Intersection(CatNmRect, CatNmRectV[RectN]); }
        if (!DoDraw) { continue; }
        // draw it!
        Gks->PutTxt(CatNm, X - HalfTxtWidth, Y - HalfTxtHeight); 
        // remember string area
        CatNmRectV.Add(CatNmRect); Cats++;
        // remember string position
        CatNmPosV.Add(CatNmPos);
    }
    PointNmRectV.AddV(CatNmRectV);
}

void TVizMapContext::PaintKeyWds(PGks Gks, const int& KeyWdFontSize, 
        TVec<TFltRect>& PointNmRectV) {

    // set font
    Gks->SetFont(TGksFont::New("ARIAL", KeyWdFontSize, ColorKeyWdFont));
    // prepare parameters
    TFltRect ZoomRect = GetZoomRect();    
    const double MnZoomRectSize = TFlt::GetMn(ZoomRect.GetXLen(), ZoomRect.GetYLen());
    const double MnRptDist = RelMnRptDist * MnZoomRectSize;
    const double Sparsity = RelSparsity * MnZoomRectSize;
    TVizMapKeyWdV OkKeyWdV;
    // start drawing the keywords
    int KeyWds = VizMapFrame->GetKeyWds();
    //SysConsole->PutLn(TStr::Fmt("Starting to draw %d keywords", KeyWds));
    for (int KeyWdN = 0; KeyWdN < KeyWds; KeyWdN++) {
        PVizMapKeyWd KeyWd = VizMapFrame->GetKeyWd(KeyWdN);
        TStr KeyWdStr = KeyWd->GetKeyWdStr().GetLc();
        const double KeyWdX = KeyWd->GetKeyWdX();
        const double KeyWdY = KeyWd->GetKeyWdY();
        if (ZoomRect.IsXYIn(KeyWdX, KeyWdY)) {
            // get coordinates in pixels
            const int X = GetScreenCoord(KeyWdX, ZoomRect.GetMnX(), 
                ZoomRect.GetXLen(), Gks->GetWidth());
            const int Y = GetScreenCoord(KeyWdY, ZoomRect.GetMnY(), 
                ZoomRect.GetYLen(), Gks->GetHeight());
            // calculate string position on the screen
            const int HalfTxtWidth = Gks->GetTxtWidth(KeyWdStr) / 2;
            const int HalfTxtHeight = Gks->GetTxtHeight(KeyWdStr) / 2;
            TFltRect KeyWdRect(X - HalfTxtWidth, Y - HalfTxtHeight,
                X + HalfTxtWidth, Y + HalfTxtHeight);
            // should we draw it?
            bool DoDrawP = true; const int Rects = PointNmRectV.Len();
            for (int RectN = 0; RectN < Rects; RectN++) {
                if (TFltRect::Intersection(KeyWdRect, PointNmRectV[RectN])) { 
                    DoDrawP = false; break; }
            }
            if (!DoDrawP) { continue; } // word overlaps, we skip it
            // check if it fits the sparsity and repetition constraint
            TFltV KeyWdCoodV = TFltV::GetV(KeyWdX, KeyWdY);
            //SysConsole->PutLn(TStr::Fmt("Keyword: %s (%.2f, %.2f)", KeyWdStr.CStr(), KeyWdX, KeyWdY));            
            const int OkKeyWds = OkKeyWdV.Len(); bool KeyWdOkP = true;
            for (int OkKeyWdN = 0; OkKeyWdN < OkKeyWds; OkKeyWdN++) {
                PVizMapKeyWd OkKeyWd = OkKeyWdV[OkKeyWdN];
                TStr OkKeyWdStr = OkKeyWd->GetKeyWdStr().GetLc();
                const double Dist = TLinAlg::EuclDist(KeyWdCoodV, OkKeyWd->GetCoordV());
                if (Dist < Sparsity) { KeyWdOkP = false; break; }
                if (OkKeyWdStr == KeyWdStr) {
                    //SysConsole->PutLn(TStr::Fmt("  Comparing to %s(%.2f, %.2f): %.2f / %.2f / %.2f",
                    //    OkKeyWdStr.CStr(), OkKeyWd->GetKeyWdX(), OkKeyWd->GetKeyWdY(), Dist, Sparsity, MnRptDist));
                    if (Dist < MnRptDist) { KeyWdOkP = false; break; }
                }
            }
            if (!KeyWdOkP) { continue; } // word repeats or is to dense, we skip it
            //SysConsole->PutLn("  All ok");
            // draw it!
            Gks->PutTxt(KeyWdStr, X - HalfTxtWidth, Y - HalfTxtHeight); 
            PointNmRectV.Add(KeyWdRect); OkKeyWdV.Add(KeyWd);
        }
    }
}

void TVizMapContext::PaintZoomRect(PGks Gks) {
    Gks->SetPen(TGksPen::New(ColorZoomRect, gpmCopy, gpsSolid, 2));
    Gks->FrameRect(DownX, DownY, ScreenX, ScreenY);
}

void TVizMapContext::PaintMgGlass(PGks Gks, const int& KeyWdFontSize) {
    // drawing the dark circle
    TFltRect ZoomRect = GetZoomRect();
    int SizeX = TFlt::Round((MgGlassSize/ZoomRect.GetXLen()) * Gks->GetWidth());
    int SizeY = TFlt::Round((MgGlassSize/ZoomRect.GetYLen()) * Gks->GetHeight());
    Gks->SetBrush(TGksBrush::New(ColorMgGlass));
    Gks->FillEllipse(ScreenX-SizeX, ScreenY-SizeY, ScreenX+SizeX, ScreenY+SizeY);
    // drawing the keywords
    if (MgGlassKeyWdV.Len() > 0) {
        // prepare the string
        Gks->SetFont(TGksFont::New("ARIAL", KeyWdFontSize, TGksColor::GetBlack(), TFSet()|gfsBold));
        TStr KeyWdStr = Gks->BreakTxt(MgGlassKeyWdV, ", ", ",", MgGlassWindowWidth);       
        TStr NearPointStr;
        if (NearPointN != -1) {
            PVizMapPoint NearPoint = VizMapFrame->GetPoint(NearPointN);
            if (NearPoint->IsPointNm()) {
                TStr NearPointNm = NearPoint->GetPointNm();
                if (NearPointNm.IsStrIn("[[")) {
                    const int StartPos = NearPointNm.SearchStr("[[");
                    NearPointNm = NearPointNm.Left(StartPos - 1);
                }
                NearPointStr = Gks->BreakTxt(NearPointNm, 
                    " ", "", MgGlassWindowWidth, 1);
                NearPointStr.DelChAll('\n');
                NearPointStr += "\n";
            }
        }
        TStr DocCountStr = "#documents = " + MgGlassPoints.GetStr() + "\n";
        // compose the final message
        KeyWdStr = NearPointStr + DocCountStr + KeyWdStr;
        // find position of the window
        int WndWidth = Gks->GetTxtWidth(KeyWdStr) + 6;
        int WndHeight = Gks->GetTxtHeight(KeyWdStr) + 6;
        int PosX = ScreenX + 20, PosY = ScreenY + 20;
        if (PosX + WndWidth > Gks->GetWidth()) {
            PosX = ScreenX - 20 - WndWidth; }
        if (PosY + WndHeight > Gks->GetHeight()) {
            PosY = ScreenY - 20 - WndHeight; }
        // draw the keyword string
        Gks->SetBrush(TGksBrush::New(ColorMgGlassWndShadow));
        Gks->FillRect(PosX + 5, PosY + 5, 
            PosX + WndWidth + 5, PosY + WndHeight + 5);
        Gks->SetBrush(TGksBrush::New(ColorMgGlassWnd));
        Gks->SetPen(TGksPen::New(ColorMgGlassWndFrm));
        Gks->Rectangle(PosX, PosY, 
            PosX + WndWidth, PosY + WndHeight);
        Gks->PutTxt(KeyWdStr, PosX+3, PosY+3);
    }
}

void TVizMapContext::Export(const TStr& FNm, const TStr& EncoderType, const int& Width, 
        const int& Height, const bool& ShowPointNmP, const int& PointFontSize, 
        const int& PointNmFontScale, const double& PointWgtThreshold, const bool& ShowCatNmP,
        const bool& ShowKeyWdP, const int& KeyWdFontSize, const bool& ShowMgGlassP, 
        const int& LegendGridWidth, const int& LegendGridHeight) {

    // create graphics
    Gdiplus::Bitmap* Bmp = new Gdiplus::Bitmap(Width, Height);
    Gdiplus::Graphics* g = Gdiplus::Graphics::FromImage(Bmp);
    PGks BmpGks = TWfGks::New();
    // paint graphics
    HDC HdcHandle = g->GetHDC(); BmpGks->BeginPaint(HdcHandle);
    Paint(BmpGks, ShowPointNmP, PointFontSize, PointNmFontScale,
        PointWgtThreshold, -1, ShowCatNmP, ShowKeyWdP, KeyWdFontSize, ShowMgGlassP, 
        LegendGridWidth, LegendGridHeight);
    BmpGks->EndPaint(); g->ReleaseHDC(HdcHandle);
    // save to disk
    WCHAR* FNmWChar = new WCHAR[FNm.Len() + 1];
    const int Res = MultiByteToWideChar(CP_ACP, 0, 
        FNm.CStr(), FNm.Len() + 1, FNmWChar, FNm.Len() + 1);
    CLSID pngClsid; GetEncoderClsid(EncoderType, &pngClsid);
    Bmp->Save(FNmWChar, &pngClsid, NULL);
    // clean after
    delete FNmWChar; delete Bmp; delete g;
}

bool TVizMapContext::MouseMove(const int& X, const int& Y, 
        const int& Width, const int& Height, const int& MgGlassKeyWds) {

    const int MoveDiff = TInt::Abs(X - ScreenX) + TInt::Abs(Y - ScreenY);
    ScreenX = X; ScreenY = Y;
    TFltRect ZoomRect = GetZoomRect();
    MouseX = GetMapCoord(X, Width, ZoomRect.GetMnX(), ZoomRect.GetXLen());
    MouseY = GetMapCoord(Y, Height, ZoomRect.GetMnY(), ZoomRect.GetYLen());
    if (!LeftButtonDownP && (MoveDiff > 0)) { 
        ResetMgGlassKeyWd(MgGlassKeyWds);
        ResetNearPointN();
    }
    return (MoveDiff > 0);
}

void TVizMapContext::MouseLeftDown(const int& X, 
        const int& Y, const int& Width, const int& Height) {
    LeftButtonDownP = true;
    DownX = X; DownY = Y;
}

void TVizMapContext::MouseLeftUp(const int& X, 
        const int& Y, const int& Width, const int& Height) {

    LeftButtonDownP = false;
    // get new mouse position coordinates
    TFltRect ZoomRect = GetZoomRect();
    MouseX = GetMapCoord(X, Width, ZoomRect.GetMnX(), ZoomRect.GetXLen());
    MouseY = GetMapCoord(Y, Height, ZoomRect.GetMnY(), ZoomRect.GetYLen());
    // what does the user want?
    if (TInt::Abs(DownX - X) > 10 && TInt::Abs(DownY - Y) > 10) {
        const double StartX = GetMapCoord(DownX, Width, ZoomRect.GetMnX(), ZoomRect.GetXLen());
        const double StartY = GetMapCoord(DownY, Height, ZoomRect.GetMnY(), ZoomRect.GetYLen());
        if (IsZoomMode()) { 
            ZoomIn(StartX, StartY, MouseX, MouseY); 
        } else if (IsSelectMode()) {
            Select(StartX, StartY, MouseX, MouseY);
        }
    } else {
        // document select
        SelectDoc(MouseX, MouseY);
    }
}

void TVizMapContext::MouseWheel(const int& Degree, const int& MgGlassKeyWds) { 
    MgGlassSize *= pow(TMath::E / 2, Degree); 
    if (!LeftButtonDownP) { ResetMgGlassKeyWd(MgGlassKeyWds); }
}

void TVizMapContext::ZoomIn(const double& X1, const double& Y1, const double& X2, const double& Y2) { 
    ZoomIn(TFltRect(TFlt::GetMn(X1, X2), TFlt::GetMn(Y1, Y2),
                    TFlt::GetMx(X1, X2), TFlt::GetMx(Y1, Y2))); 
}

void TVizMapContext::ZoomIn(const TFltRect& ZoomInRect) { 
    ZoomS.Push(ZoomInRect); 
    RenderLandscapeBmp();
    ResetMgGlassSize();
    ResetKeyWd();
}

bool TVizMapContext::ZoomOut() { 
    if (!ZoomS.Empty()) { 
        ZoomS.Pop(); 
        RenderLandscapeBmp(); 
        ResetMgGlassSize();
        ResetKeyWd();
        return true;
    } 
    return false;
}
bool TVizMapContext::ZoomAll() { 
    if (!ZoomS.Empty()) { 
        ZoomS.Clr(); 
        RenderLandscapeBmp(); 
        ResetMgGlassSize();
        ResetKeyWd();
        return true;
    }
    return false;
}

void TVizMapContext::Select(const double& X1, const double& Y1, const double& X2, const double& Y2) {
    Select(TFltRect(TFlt::GetMn(X1, X2), TFlt::GetMn(Y1, Y2),
                    TFlt::GetMx(X1, X2), TFlt::GetMx(Y1, Y2)));
}

void TVizMapContext::Select(const TFltRect& SelRect) {
    TIntV NewSelPointV; 
    if (VizMapFrame->GetRectPointV(SelRect, NewSelPointV) > 0) {
        SelPointV.Union(NewSelPointV);
    }
}

void TVizMapContext::Select(const int& DocId) {
    SelPointV.Clr();
    int Points = VizMapFrame->GetPoints();
    for (int PointN = 0; PointN < Points; PointN++) {
        PVizMapPoint Point = VizMapFrame->GetPoint(PointN);
        if (Point->IsDocId() && Point->GetDocId() == DocId) {
            SelPointV.Add(PointN); break;
        }
    }
    if (SelPointV.Empty()) {
        const int Frames = VizMap->GetVizMapFrames();
        for (int FrameN = 0; FrameN < Frames; FrameN++) {
            if (FrameN == VizMapFrameN) { continue; }
            PVizMapFrame OtherFrame = VizMap->GetVizMapFrame(FrameN);
            int Points = OtherFrame->GetPoints();
            for (int PointN = 0; PointN < Points; PointN++) {
                PVizMapPoint Point = OtherFrame->GetPoint(PointN);
                if (Point->IsDocId() && Point->GetDocId() == DocId) {
                    SelPointV.Add(PointN); SetVizMapFrame(FrameN); break;
                }
            }
        }
    }
}

void TVizMapContext::GetSelectDIdV(TIntV& DIdV) {
    DIdV.Gen(SelPointV.Len(), 0);
    const int SelPoints = SelPointV.Len();
    for (int SelPointN = 0; SelPointN < SelPoints; SelPointN++) {
        DIdV.Add(VizMapFrame->GetPoint(SelPointV[SelPointN])->GetDocId());
    }
}

void TVizMapContext::UnselectAll() {
    SelPointV.Clr();
}

void TVizMapContext::SelectDoc(const double& DocX, const double& DocY) {
    // get the closest point from the map
    const int PointN = VizMapFrame->GetPointN(TFltV::GetV(DocX, DocY), 1.5);
    // mark the point selected (depanding on the mode)
    if (IsZoomMode()) { 
        if (PointN != -1) { 
            SelPointV = TIntV::GetV(PointN); 
        } else { 
            SelPointV.Clr(); 
        }
    } else if (IsSelectMode()) {
        if (PointN != -1) { 
            if (SelPointV.IsInBin(PointN)) {
                SelPointV.DelIfIn(PointN);
            } else {
                SelPointV.AddSorted(PointN); 
            }
        }
    }
}
