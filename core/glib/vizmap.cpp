//////////////////////////////////////////////////////////////////////////
// Visualization-Map-Links
int TVizMapLink::GetPointN(const int& PointN) const {
    if (SrcPointN == PointN) { return DstPointN; }
    else if (DstPointN == PointN) { return SrcPointN; }
    else { return -1; }
}

//////////////////////////////////////////////////////////////////////////
// Visualization-Map-Landscape
TVizMapLandscape::TVizMapLandscape(const TVizMapLandscapeV& CatLandscapeV,
        const PVizMapLandscape& FullLandscape, const TIntH& CatIdH,
        const double& MxCover, const double& MnCover, const int& MnCats) {

    const int Cats = CatLandscapeV.Len();
    IAssert(Cats > 0); IAssert(Cats < TInt::Mx);
    const int XDim = CatLandscapeV[0]->GetXDim();
    const int YDim = CatLandscapeV[0]->GetYDim();
    IAssert(FullLandscape->GetXDim() == XDim);
    IAssert(FullLandscape->GetYDim() == YDim);
    // load height matrix where each element has the maximal value
    HeightMatrixVV.Gen(XDim, YDim); HeightMatrixVV.PutAll(0.0);
    for (int XPos = 0; XPos < XDim; XPos++) {
        for (int YPos = 0; YPos < YDim; YPos++) {
            HeightMatrixVV(XPos, YPos) = sdouble(FullLandscape->GetHeight(XPos, YPos));
        }
    }
    // eliminate too or non frequent categories
    TIntH CatIdToCountH; TIntH EdgeCatH;
    forever {
        // anything left to remove
        const int ActiveCats = CatIdH.Len() - EdgeCatH.Len();
        if (ActiveCats <= MnCats) { break; }
        // count categories
        CatIdToCountH.Clr();
        for (int XPos = 0; XPos < XDim; XPos++) {
            for (int YPos = 0; YPos < YDim; YPos++) {
                // get maximal category
                double MxHeight = 0.0; int MxCatId = -1;
                for (int CatId = 0; CatId < Cats; CatId++) {
                    if (EdgeCatH.IsKey(CatId)) { continue; }
                    const double Height = CatLandscapeV[CatId]->GetHeight(XPos, YPos);
                    if (Height > MxHeight) { MxHeight = Height; MxCatId = CatId; }
                }
                if (MxCatId != -1) { CatIdToCountH.AddDat(MxCatId)++; }
            }
        }
        // get most and least frequent
        int MxCount = 0, MxCatId = -1;
        int MnCount = XDim * YDim, MnCatId = -1;
        int CatKeyId = CatIdToCountH.FFirstKeyId();
        while (CatIdToCountH.FNextKeyId(CatKeyId)) {
            const int CatId = CatIdToCountH.GetKey(CatKeyId);
            const int CatCount = CatIdToCountH[CatKeyId];
            if (CatCount > MxCount) { MxCount = CatCount; MxCatId = CatId; }
            if (CatCount < MnCount) { MnCount = CatCount; MnCatId = CatId; }
        }
        // compute covers
        const double TopCover = double(MxCount) / double(XDim * YDim);
        const double BottomCover = double(MnCount) / double(XDim * YDim);
        if (TopCover > MxCover && MxCatId != -1) {
            // check if we should remove the most frequent category
            printf("RemovingT %d (cover: %.4f), Left: %d\n", MxCatId, TopCover, ActiveCats-1);
            EdgeCatH.AddDat(MxCatId);
        } else if (BottomCover < MnCover && MnCatId != -1) {
            // check if we should remove the least frequent category
            printf("RemovingB %d (cover: %.4f), Left: %d\n", MnCatId, BottomCover, ActiveCats-1);
            EdgeCatH.AddDat(MnCatId);
        } else {
            break; // nothing left to remove or non too frequent anymore
        }
    }
    // prepare a list of remaining categories
    TIntV SelCatIdV;
    for (int CatId = 0; CatId < Cats; CatId++) {
        if (EdgeCatH.IsKey(CatId)) { continue; }
        SelCatIdV.Add(CatId);
    }
    // load only the categories between the cover thersolds
    CatIdToCountH.Clr(); CatIdToCenterH.Clr();
    //const double FirstLevel = FullLandscape->GetLevel(0);
    CatMatrixVV.Gen(XDim, YDim); CatMatrixVV.PutAll(-1);
    for (int XPos = 0; XPos < XDim; XPos++) {
        for (int YPos = 0; YPos < YDim; YPos++) {
            // get maximal category
            double MxHeight = 0.0; int MxCatIdN = -1;
            for (int CatN = 0; CatN < SelCatIdV.Len(); CatN++) {
                const int CatIdN = SelCatIdV[CatN];
                const double Height = CatLandscapeV[CatIdN]->GetHeight(XPos, YPos);
                if (Height > MxHeight) { MxHeight = Height; MxCatIdN = CatIdN; }
            }
            if (MxCatIdN != -1) {
                // add it to the class matrix
                const int MxCatId = CatIdH.GetKey(MxCatIdN);
                CatMatrixVV(XPos, YPos) = MxCatId;
                // count category
                CatIdToCountH.AddDat(MxCatId)++;
                // calculate mass center
                CatIdToCenterH.AddDat(MxCatId).Val1 += double(XPos) / double(XDim - 1);
                CatIdToCenterH.AddDat(MxCatId).Val2 += double(YPos) / double(YDim - 1);
            }
        }
    }
    // finalize the category mass centers
    int CatKeyId = CatIdToCenterH.FFirstKeyId();
    while (CatIdToCenterH.FNextKeyId(CatKeyId)) {
        const int CatId = CatIdToCenterH.GetKey(CatKeyId);
        const double CatCount = double(CatIdToCountH.GetDat(CatId));
        IAssert(CatCount > 0);
        CatIdToCenterH[CatKeyId].Val1 = CatIdToCenterH[CatKeyId].Val1 / CatCount;
        CatIdToCenterH[CatKeyId].Val2 = CatIdToCenterH[CatKeyId].Val2 / CatCount;
    }
    // normalize the hight matrix
    LevelV = FullLandscape->GetLevelV();
}

void TVizMapLandscape::PutCatVV(const TIntVV& _CatMatrixVV) {
    IAssert(_CatMatrixVV.GetXDim() == GetXDim());
    IAssert(_CatMatrixVV.GetYDim() == GetYDim());
    CatMatrixVV = _CatMatrixVV;
}

int TVizMapLandscape::GetCatFromCoord(const double& X, const double& Y) {
    const int XPos = TFlt::Round(X * double(GetXDim() - 1));
    const int YPos = TFlt::Round(Y * double(GetYDim() - 1));
    return GetCat(XPos, YPos);
}

void TVizMapLandscape::CalcStart(const int& XDim, const int& YDim, const double& Sigma) {
    PKernel Kernel = TRadialKernel::New(1/Sigma);
    CalcMatrixVV.Gen(XDim, YDim);
    CalcMatrixVV.PutAll(0.0);

    // find the size of the stamp
    const double Threshold = 1e-10 * Kernel->Calc(0.0,0.0,0.0);
    const double w = XDim, h = YDim;
    int StampWidth = 0, StampHeight = 0;
    while (Kernel->Calc(0,0,TMath::Sqr(StampWidth/w)) > Threshold) { StampWidth++; }
    while (Kernel->Calc(0,0,TMath::Sqr(StampHeight/h)) > Threshold) { StampHeight++; }
    StampWidth = TInt::GetMn(StampWidth, XDim);
    StampHeight = TInt::GetMn(StampHeight, YDim);
    // prepare stamp
    CalcStampVV.Gen(StampWidth, StampHeight);
    for (int x = 0; x < StampWidth; x++) {
        const double X = x/w;
        for (int y = 0; y < StampHeight; y++) {
            const double Y = y/h;
            CalcStampVV(x,y) = Kernel->Calc(0.0,0.0,X*X+Y*Y);
        }
    }
}

void TVizMapLandscape::CalcPutStamp(const double& PointX,
        const double& PointY, const double& PointWgt) {

    const int Width = CalcMatrixVV.GetXDim();
    const int Height = CalcMatrixVV.GetYDim();
    const int StampWidth = CalcStampVV.GetXDim();
    const int StampHeight = CalcStampVV.GetYDim();
    // find discrete position of document
    const int DocX = TFlt::Round(PointX * Width);
    const int DocY = TFlt::Round(PointY * Height);
    // find borders around document for stamping
    const int LowX = TInt::GetMx(DocX-StampWidth, -1);
    const int LowY = TInt::GetMx(DocY-StampHeight, -1);
    const int HighX = TInt::GetMn(DocX+StampWidth, Width);
    const int HighY = TInt::GetMn(DocY+StampHeight, Height);
    // lower right part of stamp
    for (int x = DocX; x < HighX; x++) {
        for (int y = DocY; y < HighY; y++) {
            EAssertR(0 <= x && x < Width, TInt(x).GetStr());
            EAssertR(0 <= y && y < Height, TInt(y).GetStr());
            EAssertR(0 <= x-DocX && x-DocX < StampWidth, TInt(x).GetStr());
            EAssertR(0 <= y-DocY && y-DocY < StampHeight, TInt(y).GetStr());
            CalcMatrixVV(x,y) += PointWgt * CalcStampVV(x-DocX,y-DocY);
        }
    }
    // lower left part of stamp
    for (int x = DocX-1; x > LowX; x--) {
        for (int y = DocY; y < HighY; y++) {
            EAssertR(0 <= x && x < Width, TInt(x).GetStr());
            EAssertR(0 <= y && y < Height, TInt(y).GetStr());
            EAssertR(0 <= DocX-x && DocX-x < StampWidth, TInt(x).GetStr());
            EAssertR(0 <= y-DocY && y-DocY < StampHeight, TInt(y).GetStr());
            CalcMatrixVV(x,y) += PointWgt * CalcStampVV(DocX-x,y-DocY);
        }
    }
    // upper right part of stamp
    for (int x = DocX; x < HighX; x++) {
        for (int y = DocY-1; y > LowY; y--) {
            EAssertR(0 <= x && x < Width, TInt(x).GetStr());
            EAssertR(0 <= y && y < Height, TInt(y).GetStr());
            EAssertR(0 <= x-DocX && x-DocX < StampWidth, TInt(x).GetStr());
            EAssertR(0 <= DocY-y && DocY-y < StampHeight, TInt(y).GetStr());
            CalcMatrixVV(x,y) += PointWgt * CalcStampVV(x-DocX,DocY-y);
        }
    }
    // upper left part of stamp
    for (int x = DocX-1; x > LowX; x--) {
        for (int y = DocY-1; y > LowY; y--) {
            EAssertR(0 <= x && x < Width, TInt(x).GetStr());
            EAssertR(0 <= y && y < Height, TInt(y).GetStr());
            EAssertR(0 <= DocX-x && DocX-x < StampWidth, TInt(x).GetStr());
            EAssertR(0 <= DocY-y && DocY-y < StampHeight, TInt(y).GetStr());
            CalcMatrixVV(x,y) += PointWgt * CalcStampVV(DocX-x,DocY-y);
        }
    }
}

void TVizMapLandscape::CalcFinish(const double& MxVal, const int& Levels) {
    const int Width = CalcMatrixVV.GetXDim();
    const int Height = CalcMatrixVV.GetYDim();
    // find min and max of the height map
    double Mn = TFlt::Mx, Mx = 0.0;
    for (int x = 0; x < Width; x++) {
        for (int y = 0; y < Height; y++) {
            Mx = TFlt::GetMx(Mx, CalcMatrixVV(x,y));
            Mn = TFlt::GetMn(Mn, CalcMatrixVV(x,y));
        }
    }
    double Diff = Mx - Mn; EAssert(Diff > 0.0);
    // normalize the height map
    HeightMatrixVV.Gen(Width, Height);
    TSFltV HeightV(Width*Height, 0);
    for (int x = 0; x < Width; x++) {
        for (int y = 0; y < Height; y++) {
            // normalize to [0,MxVal]
            HeightMatrixVV(x,y) = sdouble(MxVal * (CalcMatrixVV(x,y)-Mn)/Diff);
            HeightV.Add(HeightMatrixVV(x,y));
        }
    }
    // clear temporary thingies
    CalcMatrixVV.Clr(); CalcStampVV.Clr();
    // calculate limits for isohipses
    HeightV.Sort(); LevelV.Gen(Levels-1, 0);
    for (int LevelN = 1; LevelN < Levels; LevelN++) {
        LevelV.Add(LevelN * HeightV[HeightV.Len()/Levels]);
    }
}

//////////////////////////////////////////////////////////////////////////
// Visualization-Map-Frame
TVizMapFrame::TVizMapFrame(): PointV(), LinkV(), FrameSortN(-1) {
}

TVizMapFrame::TVizMapFrame(TSIn& SIn) {
    PointV.Load(SIn); LinkV.Load(SIn);
    FrameNmStr.Load(SIn); FrameSortN.Load(SIn);
    LandscapeV.Load(SIn);
    QueryLandscape = PVizMapLandscape(SIn);
    PointBowDocBs = PBowDocBs(SIn);
    KeyWdBowDocBs = PBowDocBs(SIn);
    KeyWdSpVV.Load(SIn);
    LinkBowDocBs = PBowDocBs(SIn);
    LinkSpVV.Load(SIn);
    KeyWdPointV.Load(SIn); KeyWdV.Load(SIn);
    CatNmH.Load(SIn); DId2PointNMap.Load(SIn);
}

TVizMapFrame::TVizMapFrame(TSIn& SIn, PBowDocBs _PointBowDocBs,
        PBowDocBs _KeyWdBowDocBs, PBowDocBs _LinkBowDocBs) {

    PointV.Load(SIn); LinkV.Load(SIn);
    FrameNmStr.Load(SIn); FrameSortN.Load(SIn);
    LandscapeV.Load(SIn);
    QueryLandscape = PVizMapLandscape(SIn);
    PointBowDocBs = _PointBowDocBs;
    KeyWdBowDocBs = _KeyWdBowDocBs;
    KeyWdSpVV.Load(SIn);
    LinkBowDocBs = _LinkBowDocBs;
    LinkSpVV.Load(SIn);
    KeyWdPointV.Load(SIn); KeyWdV.Load(SIn);
    CatNmH.Load(SIn); DId2PointNMap.Load(SIn);
}

void TVizMapFrame::Save(TSOut& SOut, const bool& SaveBows) const {
    PointV.Save(SOut); LinkV.Save(SOut);
    FrameNmStr.Save(SOut); FrameSortN.Save(SOut);
    LandscapeV.Save(SOut); QueryLandscape.Save(SOut);
    if (SaveBows) { PointBowDocBs.Save(SOut); }
    if (SaveBows) { KeyWdBowDocBs.Save(SOut); } KeyWdSpVV.Save(SOut);
    if (SaveBows) { LinkBowDocBs.Save(SOut); } LinkSpVV.Save(SOut);
    KeyWdPointV.Save(SOut); KeyWdV.Save(SOut);
    CatNmH.Save(SOut); DId2PointNMap.Save(SOut);
}

int TVizMapFrame::GetPointN(const TFltV& CoordV, const double& Radius) const {
    const int Points = PointV.Len();
    // search for the closest point
    int MnPointN = -1; double MnDist = TFlt::Mx;
    for (int PointN = 0; PointN < Points; PointN++) {
        const double Dist = TLinAlg::EuclDist2(CoordV, GetPoint(PointN)->GetCoordV());
        if (MnDist > Dist) {
            MnPointN = PointN; MnDist = Dist;
        }
    }
    if (sqrt(MnDist) > Radius) { MnPointN = -1; }
    return MnPointN;
}

int TVizMapFrame::GetRectPointV(const TFltRect& Rect, TIntV& RectPointV) const {
    const int Points = PointV.Len(); RectPointV.Clr();
    for (int PointN = 0; PointN < Points; PointN++) {
        PVizMapPoint Point = GetPoint(PointN);
        if (Rect.IsXYIn(Point->GetPointX(), Point->GetPointY())) {
            RectPointV.Add(PointN);
        }
    }
    return RectPointV.Len();
}

int TVizMapFrame::AddLink(PVizMapLink VizMapLink) {
    EAssert(IsPointN(VizMapLink->GetSrcPointN()));
    EAssert(IsPointN(VizMapLink->GetDstPointN()));

    int LinkN = LinkV.Add(VizMapLink);
    PointV[VizMapLink->GetSrcPointN()]->PutLinkN(LinkN);
    PointV[VizMapLink->GetDstPointN()]->PutLinkN(LinkN);
    return LinkN;
}

void TVizMapFrame::AddLandscape(const int& XDim, const int& YDim,
        const double& Sigma, const bool& PointWgtP) {

    PVizMapLandscape NewLandscape = TVizMapLandscape::New();
    NewLandscape->CalcStart(XDim, YDim, Sigma);

    const int Points = GetPoints();
    for (int PointN = 0; PointN < Points; PointN++) {
        const PVizMapPoint VizMapPoint = GetPoint(PointN);
        const double PointWgt = PointWgtP ? VizMapPoint->GetWgt() : 1.0;
        NewLandscape->CalcPutStamp(VizMapPoint->GetPointX(),
            VizMapPoint->GetPointY(), PointWgt);
    }

    NewLandscape->CalcFinish(1.0, 3);
    LandscapeV.Add(NewLandscape);
}

void TVizMapFrame::AddLandscapeFromLndMrk(const int& XDim,
        const int& YDim, const double& Sigma,
        const TVec<TFltV>& LndMrkPointV) {

    PVizMapLandscape NewLandscape = TVizMapLandscape::New();
    NewLandscape->CalcStart(XDim, YDim, Sigma);

    const int LndMrkPoints = LndMrkPointV.Len();
    for (int LndMrkPointN = 0; LndMrkPointN < LndMrkPoints; LndMrkPointN++) {
        const TFltV LndMrkPoint = LndMrkPointV[LndMrkPointN];
        IAssert(LndMrkPoint.Len() == 2);
        NewLandscape->CalcPutStamp(LndMrkPoint[0], LndMrkPoint[1], 1.0);
    }

    NewLandscape->CalcFinish(1.0, 3);
    LandscapeV.Add(NewLandscape);
}

void TVizMapFrame::AddLandscapeWithCats(const int& XDim, const int& YDim,
        const double& Sigma, const bool& PointWgtP) {

    TIntKdV CountCatIdV;
    {int CatId = CatNmH.FFirstKeyId();
    while (CatNmH.FNextKeyId(CatId)) {
        const int Count = CatNmH[CatId];
        CountCatIdV.Add(TIntKd(Count, CatId));
    }}
    CountCatIdV.Sort(false); const int MxCats = 20;
    if (CountCatIdV.Len() > MxCats) { CountCatIdV.Trunc(MxCats); }
    // load a list of big categories
    TIntH BigCatIdH;
    for (int CatN = 0; CatN < CountCatIdV.Len(); CatN++) {
        BigCatIdH.AddKey(CountCatIdV[CatN].Dat); }
    // prepare an empty landscape for each category
    const int Cats = BigCatIdH.Len(); IAssert(Cats > 0);
    TVizMapLandscapeV CatLandscapeV(Cats, 0);
    for (int CatIdN = 0; CatIdN < Cats; CatIdN++) {
        PVizMapLandscape CatLandscape = TVizMapLandscape::New();
        CatLandscape->CalcStart(XDim, YDim, Sigma);
        CatLandscapeV.Add(CatLandscape);
    }
    PVizMapLandscape FullLandscape = TVizMapLandscape::New();
    FullLandscape->CalcStart(XDim, YDim, Sigma);
    // add points to appropriate categories
    const int Points = GetPoints();
    for (int PointN = 0; PointN < Points; PointN++) {
        const PVizMapPoint VizMapPoint = GetPoint(PointN);
        const double PointWgt = PointWgtP ? VizMapPoint->GetWgt() : 1.0;
        for (int CatIdN = 0; CatIdN < VizMapPoint->GetCats(); CatIdN++) {
            const int CatId = VizMapPoint->GetCatId(CatIdN);
            if (BigCatIdH.IsKey(CatId)) {
                CatLandscapeV[BigCatIdH.GetKeyId(CatId)]->CalcPutStamp(
                    VizMapPoint->GetPointX(), VizMapPoint->GetPointY(), PointWgt);
            }
        }
        FullLandscape->CalcPutStamp(VizMapPoint->GetPointX(),
            VizMapPoint->GetPointY(), PointWgt);
    }
    // finish the category landscapes
    for (int CatIdN = 0; CatIdN < Cats; CatIdN++) {
        const int CatId = BigCatIdH.GetKey(CatIdN);
        const double MxVal = double(CatNmH[CatId]) / double(Points);
        CatLandscapeV[CatIdN]->CalcFinish(MxVal, 3);
    }
    FullLandscape->CalcFinish(1.0, 3);
    // and finaly make one overal landscape
    LandscapeV.Add(TVizMapLandscape::New(CatLandscapeV,
        FullLandscape, BigCatIdH, 0.6, 0.1, 2));
}

int TVizMapFrame::GetDocId(const int& PointN) const {
    if (!IsPointN(PointN)) { return -1; }
    PVizMapPoint Point = GetPoint(PointN);
    if (PointBowDocBs.Empty() || !Point->IsDocId()) { return -1; }
    return Point->GetDocId();
}

TStr TVizMapFrame::GetDocNm(const int& PointN) const {
    if (!IsPointN(PointN)) { return ""; }
    PVizMapPoint Point = GetPoint(PointN);
    if (PointBowDocBs.Empty() || !Point->IsDocId()) { return ""; }
    return PointBowDocBs->GetDocNm(Point->GetDocId());
}

TStr TVizMapFrame::GetDocBody(const int& PointN) const {
    if (!IsPointN(PointN)) { return ""; }
    PVizMapPoint Point = GetPoint(PointN);
    if (PointBowDocBs.Empty() || !Point->IsDocId()) { return ""; }
    return PointBowDocBs->GetDocStr(Point->GetDocId());
}

TStr TVizMapFrame::GetDocUrl(const int& PointN) const {
    if (!IsPointN(PointN)) { return ""; }
    PVizMapPoint Point = GetPoint(PointN);
    if (PointBowDocBs.Empty() || !Point->IsDocId()) { return ""; }
    TStr UrlStr = PointBowDocBs->GetDocDescStr(Point->GetDocId());
    if (UrlStr.SearchStr("://") != -1) { return UrlStr; }
    else { return ""; }
}

int TVizMapFrame::GenKeyWd(const TFltV& CoordV, const double& Radius,
        const int& MxKeyWd, TStrV& KeyWdStrV, const bool& DistWgtP,
        const bool& PointWgtP, const bool& BackKeyWdP) const {

    // do we have keyword information?
    if (KeyWdBowDocBs.Empty() || KeyWdSpVV.Empty()) {
        KeyWdStrV.Clr(); return 0; }
    // average documents inside circle
    TFltV FullKeyWdStrV(KeyWdBowDocBs->GetWords());
    FullKeyWdStrV.PutAll(0.0);
    const TVizMapPointV& _PointV = KeyWdPointV.Empty() ? PointV : KeyWdPointV;
    const int Points = _PointV.Len(); int UsedPoints = 0, CenterPoints = 0;
    bool PPQuad = false, PNQuad = false, NPQuad = false, NNQuad = false;
    const double CoordX = CoordV[0], CoordY = CoordV[1];
    for (int PointN = 0; PointN < Points; PointN++) {
        PVizMapPoint Point = _PointV[PointN];
        const double Dist = TLinAlg::EuclDist(CoordV, Point->GetCoordV());
        if (Dist < Radius) {
            PBowSpV DocSpV = KeyWdSpVV[PointN];
            const double DistWgt = DistWgtP ? (Radius - Dist)/Radius : 1.0;
            const double PointWgt = PointWgtP ? Point->GetWgt() : 1.0;
            TBowLinAlg::AddVec(DistWgt*PointWgt, DocSpV, FullKeyWdStrV);
            UsedPoints++; if (Dist < (Radius/3.0)) { CenterPoints++; }
            const double DiffX = CoordX - Point->GetPointX();
            const double DiffY = CoordY - Point->GetPointY();
            PPQuad = PPQuad || (DiffX > 0 && DiffY > 0);
            PNQuad = PNQuad || (DiffX > 0 && DiffY < 0);
            NPQuad = NPQuad || (DiffX < 0 && DiffY > 0);
            NNQuad = NNQuad || (DiffX < 0 && DiffY < 0);
        }
    }
    if (UsedPoints == 0) { KeyWdStrV.Clr(); return 0; }
    if (BackKeyWdP) {
        if (CenterPoints == 5) { KeyWdStrV.Clr(); return 0; }
        if (!(PPQuad && PNQuad && NPQuad && NNQuad)) { KeyWdStrV.Clr(); return 0; }
    }
    TLinAlg::NormalizeL1(FullKeyWdStrV);
    // find top keywords
    TFltIntKdV WgtWIdKdV(FullKeyWdStrV.Len(), 0);
    for (int WId = 0; WId < FullKeyWdStrV.Len(); WId++) {
        WgtWIdKdV.Add(TFltIntKd(FullKeyWdStrV[WId], WId));
    }
    WgtWIdKdV.Sort(false);
    KeyWdStrV.Gen(MxKeyWd, 0); int WIdN = 0;
    while (WIdN < MxKeyWd) {
        if (WIdN >= WgtWIdKdV.Len()) { return UsedPoints; }
        if (WgtWIdKdV[WIdN].Key.Val < 0.001) { return UsedPoints; }

        const int WId = WgtWIdKdV[WIdN].Dat;
        KeyWdStrV.Add(KeyWdBowDocBs->GetWordStr(WId));
        WIdN++;
    }
    return UsedPoints;
}

void TVizMapFrame::GenKeyWds(const int& KeyWds, const double& Radius,
        const int& Candidantes, const int& RndSeed, const TFltRect& Rect) {

    KeyWdV.Gen(KeyWds, 0); TRnd Rnd(RndSeed); TStrV KeyWdStrV;
    const double MnX = Rect.GetMnX(), MnY = Rect.GetMnY();
    const double LenX = Rect.GetXLen(), LenY = Rect.GetYLen();
    for (int KeyWdN = 0; KeyWdN < KeyWds; KeyWdN++) {
        const double X = MnX + Rnd.GetUniDev() * LenX;
        const double Y = MnY + Rnd.GetUniDev() * LenY;
        GenKeyWd(TFltV::GetV(X, Y), Radius, Candidantes, KeyWdStrV, false, false, true);
        if (!KeyWdStrV.Empty()) {
            // select random keyword
            const int KeyWdStrN = Rnd.GetUniDevInt(KeyWdStrV.Len());
            TStr KeyWdStr = KeyWdStrV[KeyWdStrN];
            // add the word to the list
            KeyWdV.Add(TVizMapKeyWd::New(TFltV::GetV(X, Y), KeyWdStr));
        }
    }
}

void TVizMapFrame::AddCat(const int& PointN, const int& CatId) {
    IAssert(IsPointN(PointN)); IAssert(CatNmH.IsKeyId(CatId));
    CatNmH[CatId]++; GetPoint(PointN)->AddCat(CatId);
}

void TVizMapFrame::CalcDId2PointNMap() {
    DId2PointNMap.Clr(); const int Points = GetPoints();
    for (int PointN = 0; PointN < Points; PointN++) {
        const PVizMapPoint VizMapPoint = GetPoint(PointN);
        EAssert(VizMapPoint->IsDocId());
        DId2PointNMap.AddDat(VizMapPoint->GetDocId(), PointN);
    }
}

void TVizMapFrame::SaveVrml(const TStr& VrmlFNm, PVizMapLandscape Landscape,
        const bool& ShowDocNmP, const bool& ShowDocPtP,
        const bool& ShowKeyWdP, const double& FontSize,
        const TStr& SkyColor, const TStr& TerrainColor,
        const TStr& KeywordColor, const TStr& DocNmColor) {

    PSOut SOut = TFOut::New(VrmlFNm);
    TVrml::InsertHeader(SOut, SkyColor);

    const TSFltVV& Rlf = Landscape->HeightVV(); TFltVV NewRlf;
    const double Scale = TVrml::InsertEvelationGrid(SOut,
        Rlf, NewRlf, TerrainColor, 5, 15.0, 0.7);

    TStr DocSize = TStr::Fmt("%.3f %.3f %.3f",
      FontSize, FontSize, FontSize);
    for (int PointN = 0; PointN < GetPoints(); PointN++) {
        TStr OrgDocNm = TStr::GetFNmStr(GetPoint(PointN)->GetPointNm());
        //TStrV DocNmPartV; OrgDocNm.SplitOnAllCh('_', DocNmPartV);
        //if (DocNmPartV.Len() > 1) {
        //    OrgDocNm = DocNmPartV[0];
        //    for (int PartN = 1; PartN < (DocNmPartV.Len()-1); PartN++) {
        //        OrgDocNm += " " + DocNmPartV[PartN];
        //    }
        //} else {
        //    OrgDocNm = DocNmPartV[0];
        //}
        const double PosX = GetPoint(PointN)->GetPointX();
        const double PosY = GetPoint(PointN)->GetPointY();
        TVrml::InsertBillboard(SOut, NewRlf, OrgDocNm, KeywordColor,
          DocSize, PosX, PosY, 1.1, Scale, ShowDocNmP, ShowDocPtP);
    }

    if (ShowKeyWdP) {
        TStr KeyWdSize = TStr::Fmt("%.3f %.3f %.3f",
          0.5*FontSize, 0.5*FontSize, 0.5*FontSize);
        TRnd Rnd(0);
        for (int WdC = 0; WdC < 100; WdC++) {
            const double PosX = Rnd.GetUniDev();
            const double PosY = Rnd.GetUniDev();
            TStrV KeyWdV; GenKeyWd(TFltV::GetV(PosX, PosY),
                0.3, 4, KeyWdV, false, false, true);
            if (KeyWdV.Len() > 0) {
                TVrml::InsertBillboard(SOut, NewRlf,
                  KeyWdV[Rnd.GetUniDevInt(KeyWdV.Len())], DocNmColor,
                  KeyWdSize, PosX, PosY, 2.0, Scale, true, false);
            }
        }
        for (int WdC = 0; WdC < 50; WdC++) {
            const double PosX = Rnd.GetUniDev();
            const double PosY = Rnd.GetUniDev();
            TStrV KeyWdV; GenKeyWd(TFltV::GetV(PosX, PosY),
                0.2, 4, KeyWdV, false, false, true);
            if (KeyWdV.Len() > 0) {
                TVrml::InsertBillboard(SOut, NewRlf,
                  KeyWdV[Rnd.GetUniDevInt(KeyWdV.Len())], DocNmColor,
                  KeyWdSize, PosX, PosY, 2.0, Scale, true, false);
            }
        }
    }
}

void TVizMapFrame::SaveLegend(const TStr& TxtFNm, const int& LegendGridWidth,
        const int& LegendGridHeight) {

    TFOut FOut(TxtFNm);
    for (int PointN = 0; PointN < GetPoints(); PointN++) {
        PVizMapPoint Point = GetPoint(PointN);
        const int XMark = (int)floor(Point->GetPointX() * LegendGridWidth);
        const int YMark = (int)floor(Point->GetPointY() * LegendGridHeight) + 1;
        const char XMarkCh = 'A' + XMark;
        FOut.PutStrLn(Point->GetPointNm() + "\t" + TStr(XMarkCh) + TInt::GetStr(YMark));
    }
    FOut.Flush();
}

//////////////////////////////////////////////////////////////////////////
// Visualization-Map
TVizMap::TVizMap(TSIn& SIn) {
    TBool AllSameP(SIn);
    if (AllSameP) {
        PBowDocBs PointBowDocBs(SIn);
        PBowDocBs KeyWdBowDocBs(SIn);
        PBowDocBs LinkBowDocBs(SIn);
        TInt Frames(SIn);
        VizMapFrameV.Gen(Frames, 0);
        for (int FrameN = 0; FrameN < Frames; FrameN++) {
            PVizMapFrame Frame = TVizMapFrame::Load(SIn,
                PointBowDocBs, KeyWdBowDocBs, LinkBowDocBs);
            VizMapFrameV.Add(Frame);
        }
    } else {
        VizMapFrameV.Load(SIn);
    }
}

void TVizMap::Save(TSOut& SOut) const {
    // check if all the Bows in the frames are the same
    TBool AllSameP = true;
    for (int FrameN = 1; FrameN < VizMapFrameV.Len(); FrameN++) {
        PVizMapFrame PrevFrame = VizMapFrameV[FrameN - 1];
        PVizMapFrame Frame = VizMapFrameV[FrameN];
        // compare bow signatures between previous and current frame
        const bool PointBowP =
            (Frame->GetPointBow().Empty() && PrevFrame->GetPointBow().Empty()) ||
            (Frame->GetPointBow()->GetSig() == PrevFrame->GetPointBow()->GetSig());
        const bool KeyWdBowP =
            (Frame->GetKeyWdBow().Empty() && PrevFrame->GetKeyWdBow().Empty()) ||
            (Frame->GetKeyWdBow()->GetSig() == PrevFrame->GetKeyWdBow()->GetSig());
        const bool LinkBowP =
            (Frame->GetLinkBow().Empty() && PrevFrame->GetLinkBow().Empty()) ||
            (Frame->GetLinkBow()->GetSig() == PrevFrame->GetLinkBow()->GetSig());
        AllSameP = PointBowP && KeyWdBowP && LinkBowP;
        // if not same, quit
        if (!AllSameP) { break; }
    }
    // remmaber the way we store bows
    AllSameP.Save(SOut);
    // store frames
    if (AllSameP) {
        // first save bows
        PVizMapFrame Frame = VizMapFrameV[0];
        Frame->GetPointBow().Save(SOut);
        Frame->GetKeyWdBow().Save(SOut);
        Frame->GetLinkBow().Save(SOut);
        // save frames without bows
        TInt Frames = VizMapFrameV.Len(); Frames.Save(SOut);
        for (int FrameN = 0; FrameN < Frames; FrameN++) {
            VizMapFrameV[FrameN]->Save(SOut, false);
        }
    } else {
        // can't do much...
        VizMapFrameV.Save(SOut);
    }
}

void TVizMap::AddVizMapFrame(const PVizMapFrame& VizMapFrame, const bool& SortedP) {
    if (SortedP) { VizMapFrameV.Add(VizMapFrame); }
    else { VizMapFrameV.AddSorted(VizMapFrame); }
}

void TVizMap::SaveXmlDoc(TSOut& SOut, const TStr& Nm, const TStr& Body,
        const TStr& DisplayBody, const TStrV& CatNmV, const TStrV& AuthorV,
        const TStr& FrameNm, const int& FrameSortN) {

    SOut.PutStrLn(" <document>");
    SOut.PutStrLn("  <name>" + TXmlDoc::GetXmlStr(Nm) + "</name>");
    SOut.PutStrLn("  <body>" + TXmlDoc::GetXmlStr(Body) + "</body>");
    if (!DisplayBody.Empty()) {
        SOut.PutStrLn("  <display_body>" + TXmlDoc::GetXmlStr(DisplayBody) + "</display_body>");
    }
    if (!CatNmV.Empty()) {
        SOut.PutStrLn("  <categories>");
        for (int CatNmN = 0; CatNmN < CatNmV.Len(); CatNmN++) {
            SOut.PutStrLn("   <category>" +
                TXmlDoc::GetXmlStr(CatNmV[CatNmN]) + "</category>");
        }
        SOut.PutStrLn("  </categories>");
    }
    if (!AuthorV.Empty()) {
        SOut.PutStrLn("  <authors>");
        for (int AuthorN = 0; AuthorN < AuthorV.Len(); AuthorN++) {
            SOut.PutStrLn("   <author>" +
                TXmlDoc::GetXmlStr(AuthorV[AuthorN]) + "</author>");
        }
        SOut.PutStrLn("  </authors>");
    }
    if (!FrameNm.Empty() && (FrameSortN != -1)) {
        SOut.PutStrLn(TStr::Fmt("  <frame id=\"%d\">%s</frame>",
            FrameSortN, TXmlDoc::GetXmlStr(FrameNm).CStr()));
    }
    SOut.PutStrLn(" </document>");
}

//////////////////////////////////////////////////////////////////////////
// Visualization-Map Xml-Document
int TVizMapXmlDoc::AddToBowDocBs(PVizMapXmlDocBs VizMapXmlDocBs, PBowDocBs BowDocBs) const {
    // get unique document name
    TStr DocNm = Nm;
    if (BowDocBs->IsDocNm(DocNm)) {
        int Count = 2; TStr NewDocNm;
        do { NewDocNm = TStr::Fmt("%s (%3d)", DocNm.CStr(), Count++);
        } while (BowDocBs->IsDocNm(NewDocNm));
        DocNm = NewDocNm;
    }
    // get the content
    TStr DocBody = Body;
    // get the categories
    TStrV CatNmV;
    for (int CatIdN = 0; CatIdN < CatIdV.Len(); CatIdN++) {
        const int CatId = CatIdV[CatIdN];
        CatNmV.Add(VizMapXmlDocBs->CatNmH.GetKey(CatId));
    }
    // add it to bow
    const int DId = BowDocBs->AddHtmlDoc(DocNm, CatNmV, DocBody, true);
    // check if we have a more user friendly version of the document
    TStr AuthorStr;
    for (int AuthorIdN = 0; AuthorIdN < AuthorIdV.Len(); AuthorIdN++) {
        const int AuthorId = AuthorIdV[AuthorIdN];
        if (AuthorIdN > 0) { AuthorStr + ", "; }
        AuthorStr += VizMapXmlDocBs->AuthorNmH.GetKey(AuthorId);
    }
    if (!DisplayBody.Empty()) {
        TStr DisplayBody = AuthorStr.Empty() ? DisplayBody :
            AuthorStr + "\r\n" + DisplayBody;
        BowDocBs->PutDocStr(DId, DisplayBody);
    } else if (!AuthorStr.Empty()) {
        BowDocBs->PutDocStr(DId, AuthorStr + "\r\n" + DocBody);
    }
    //retrun the document id
    return DId;
}

//////////////////////////////////////////////////////////////////////////
// Visualization-Map Xml-Document-Base
TVizMapXmlDocBs::TVizMapXmlDocBs(const TStr& XmlFNm, const PNotify& Notify) {
    PSIn XmlSIn = TFIn::New(XmlFNm); TXmlDoc::SkipTopTag(XmlSIn);
    PXmlDoc XmlDoc; int XmlDocs=0;
    forever {
        // load next message
        XmlDoc = TXmlDoc::LoadTxt(XmlSIn); XmlDocs++;
        // stop if at the last tag
        if (!XmlDoc->IsOk()) { /*InfoNotify(XmlDoc->GetMsgStr());*/ break; }
        // display count
        if (XmlDocs % 10 == 0) { Notify->OnStatus(TStr::Fmt("%d\r", XmlDocs)); }
        // parse the message metadata
        PXmlTok XmlTok = XmlDoc->GetTok();
        TVizMapXmlDoc VizMapXmlDoc;
        VizMapXmlDoc.Nm = XmlTok->GetTagVal("name", false);
        //Notify->OnStatus(VizMapXmlDoc.Nm);
        VizMapXmlDoc.Body = XmlTok->GetTagVal("body", false);
        if (XmlTok->IsTagTok("display_body")) {
            VizMapXmlDoc.DisplayBody = XmlTok->GetTagVal("display_body", false); }
        // read categories
        TXmlTokV CatTokV; XmlTok->GetTagTokV("categories|category", CatTokV);
        for (int CatTokN = 0; CatTokN < CatTokV.Len(); CatTokN++) {
            TStr CatNm = CatTokV[CatTokN]->GetTokStr(false);
            int CatId = CatNmH.AddKey(CatNm.GetTrunc());
            VizMapXmlDoc.CatIdV.Add(CatId);
        }
        // read authors
        TXmlTokV AuthorTokV; XmlTok->GetTagTokV("authors|author", AuthorTokV);
        for (int AuthorTokN = 0; AuthorTokN < AuthorTokV.Len(); AuthorTokN++) {
            TStr AuthorNm = AuthorTokV[AuthorTokN]->GetTokStr(false);
            int AuthorId = AuthorNmH.AddKey(AuthorNm.GetTrunc());
            VizMapXmlDoc.AuthorIdV.Add(AuthorId);
        }
        // read frame
        if (XmlTok->IsSubTag("frame")) {
            TStr FrameNm = XmlTok->GetTagVal("frame", false);
            const int FrameSortN = XmlTok->GetTagTok("frame")->GetIntArgVal("id", -1);
            if (FrameNmToSortN.IsKey(FrameNm)) {
                IAssert(FrameSortN == FrameNmToSortN(FrameNm));
            } else {
                FrameNmToSortN.AddDat(FrameNm, FrameSortN);
            }
            VizMapXmlDoc.FrameNm = FrameNm.GetTrunc();
            VizMapXmlDoc.FrameSortN = FrameSortN;
        } else {
            IAssert(FrameNmToSortN.Empty());
        }
        // add doc to base
        const int DocId = XmlDocV.Add(VizMapXmlDoc);
        if (!FrameNmToSortN.Empty()) {
            FrameSortNToDocId.AddDat(VizMapXmlDoc.FrameSortN).Add(DocId);
        }
    }
    Notify->OnStatus(TStr::Fmt("%d", XmlDocs-1));
}

TVizMapXmlDocBs::TVizMapXmlDocBs(const PWdGixRSet& RSet,
        const TVizMapXmlDocGrouping& Grouping, const PNotify& Notify) {

    const int Docs = RSet->GetDocs();
    for (int DocN = 0; DocN < Docs; DocN++) {
        TVizMapXmlDoc VizMapXmlDoc;
        // get frame
        TTm DateTm = RSet->GetDocDateTime(DocN);
        int FrameSortN = 0; TStr FrameNm = "";
        if (Grouping == vmxdgDay) {
            FrameNm = DateTm.GetWebLogDateStr();
            FrameSortN = TTm::GetDateIntFromTm(DateTm);
        } else if (Grouping == vmxdgMonth) {
            FrameNm = TStr::Fmt("%04d-%02d", DateTm.GetYear(), DateTm.GetMonth());
            FrameSortN = TTm::GetMonthIntFromTm(DateTm);
        } else if (Grouping == vmxdgYear) {
            FrameNm = TStr::Fmt("%04d", DateTm.GetYear());
            FrameSortN = DateTm.GetYear();
        }
        // keep track of all the frames
        if (Grouping == vmxdgNone) {
            IAssert(FrameNmToSortN.Empty());
        } else {
            if (FrameNmToSortN.IsKey(FrameNm)) {
                IAssert(FrameSortN == FrameNmToSortN(FrameNm));
            } else {
                FrameNmToSortN.AddDat(FrameNm, FrameSortN);
            }
        }
        VizMapXmlDoc.FrameNm = FrameNm;
        VizMapXmlDoc.FrameSortN = FrameSortN;

        // parse document content
        PXmlDoc Doc=TXmlDoc::LoadStr(RSet->GetDocStr(DocN));
        IAssertR(Doc->IsOk(), Doc->GetMsgStr());
        // parse title
        VizMapXmlDoc.Nm =Doc->GetTagTok("doc|title")->GetTokStr(false);
        // parse content and name entities
        TChA BodyChA; VizMapXmlDoc.AuthorIdV.Clr();
        TXmlTokV ParTokV; Doc->GetTagTokV("doc|body|p", ParTokV);
        for (int ParTokN = 0; ParTokN < ParTokV.Len(); ParTokN++){
            TStr ParStr = ParTokV[ParTokN]->GetTokStr(false);
            if (!BodyChA.Empty()) { BodyChA += "\r\n"; }
            BodyChA += ParStr;
            // check for new name entities
            TXmlTokV NmEnTokV; ParTokV[ParTokN]->GetTagTokV("nmobj", NmEnTokV);
            for (int NmEnTokN = 0; NmEnTokN < NmEnTokV.Len(); NmEnTokN++) {
                TStr NmEnStr = NmEnTokV[NmEnTokN]->GetTokStr(false);
                const int NmEnId = AuthorNmH.AddKey(NmEnStr);
                VizMapXmlDoc.AuthorIdV.AddMerged(NmEnId);
            }
        }
        VizMapXmlDoc.Body = BodyChA;
        // codes
        VizMapXmlDoc.CatIdV.Clr();
        const TStrV& CatNmV = RSet->GetDocCatNmV(DocN);
        for (int CatNmN = 0; CatNmN < CatNmV.Len(); CatNmN++) {
            const TStr& CatNm = CatNmV[CatNmN];
            const int CatId = CatNmH.AddKey(CatNm); CatNmH[CatId]++;
            VizMapXmlDoc.CatIdV.AddMerged(CatId);
        }
        // add doc to base
        const int DocId = XmlDocV.Add(VizMapXmlDoc);
        if (!FrameNmToSortN.Empty()) {
            FrameSortNToDocId.AddDat(VizMapXmlDoc.FrameSortN).Add(DocId);
        }
    }
}

PBowDocBs TVizMapXmlDocBs::LoadBowDocBs(const TStr& XmlFNm, PSwSet SwSet,
        PStemmer Stemmer, const int& MxNGramLen, const int& MnNGramFq) {

    // parse xml
    PVizMapXmlDocBs XmlDocBs = TVizMapXmlDocBs::New(XmlFNm);
    const TVizMapXmlDocV& XmlDocV = XmlDocBs->XmlDocV;
    // load documents to for NGrams
    PNGramBs NGramBs = NULL;
    if (MxNGramLen > 1) {
        TStrV DocBodyV(XmlDocV.Len(), 0);
        for (int XmlDocN = 0; XmlDocN < XmlDocV.Len(); XmlDocN++) {
            const TVizMapXmlDoc& XmlDoc = XmlDocV[XmlDocN];
            DocBodyV.Add(XmlDoc.Nm + "\n" + XmlDoc.Body);
        }
        NGramBs = TNGramBs::GetNGramBsFromHtmlStrV(DocBodyV,
            MxNGramLen, MnNGramFq, SwSet, Stemmer);
    }
    // load documents to Bow
    PBowDocBs BowDocBs = TBowDocBs::New(SwSet, Stemmer, NGramBs);
    for (int XmlDocN = 0; XmlDocN < XmlDocV.Len(); XmlDocN++) {
        // read stuff from document
        const TVizMapXmlDoc& XmlDoc = XmlDocV[XmlDocN];
        TStr DocNm = XmlDoc.Nm, DocBody = XmlDoc.Body;
        TStrV CatNmV;
        for (int CatIdN = 0; CatIdN < XmlDoc.CatIdV.Len(); CatIdN++) {
            const int CatId = XmlDoc.CatIdV[CatIdN];
            CatNmV.Add(XmlDocBs->CatNmH.GetKey(CatId));
        }
        TStr AuthorStr;
        for (int AuthorIdN = 0; AuthorIdN < XmlDoc.AuthorIdV.Len(); AuthorIdN++) {
            const int AuthorId = XmlDoc.AuthorIdV[AuthorIdN];
            if (AuthorIdN > 0) { AuthorStr + ", "; }
            AuthorStr += XmlDocBs->AuthorNmH.GetKey(AuthorId);
        }
        if (AuthorStr.Empty()) { DocBody = DocBody; }
        else { DocBody = AuthorStr + "\r\n" + DocBody;}
        // add it to bow
        const int DId = BowDocBs->AddHtmlDoc(DocNm, CatNmV, DocBody, true);
        // check if we have a more user friendly version of the document
        if (!XmlDoc.DisplayBody.Empty()) {
            TStr DisplayBody = AuthorStr.Empty() ? XmlDoc.DisplayBody :
                AuthorStr + "\r\n" + XmlDoc.DisplayBody;
            BowDocBs->PutDocStr(DId, DisplayBody);
        }
    }
    return BowDocBs;
}

//////////////////////////////////////////////////////////////////////////
// Visualization-Map Factory
void TVizMapFactory::CG(const TMatrix& Matrix, const TFltV& b,
        TFltV& x, PNotify Notify, const int& MaxStep, const double& EpsTer) {
    int M = x.Len(), R = b.Len(), i;
    TFltV r(M), p(M), q(M), tmp(R);

    x.PutAll(0.0);
    // calculate right side of system
    Matrix.MultiplyT(b, r);

    // conjugate gradient method - CG
    // from "Templates for the soltuion of linear systems" (M == eye)
    double nro, ro, alpha, beta;
    nro = TLinAlg::Norm2(r);
    int k=1;
    //for (int k = 1; k <= MaxStep && nro > EpsTer && k <= M; k++) {
    while (k <= MaxStep && nro > EpsTer && k <= M){
        if (k == 1) {
            p = r;
        } else {
            beta = nro / ro;
            for (i = 0; i < M; i++)
                p[i] = r[i] + beta*p[i];
        }

        // q = A*p = (X'*X)*p
        Matrix.Multiply(p, tmp);
        Matrix.MultiplyT(tmp, q);

        // calcualte new x and residual
        alpha = nro / TLinAlg::DotProduct(p, q);
        for (i = 0; i < M; i++) {
            x[i] = x[i] + alpha * p[i];
            r[i] = r[i] - alpha * q[i];
        }

        ro = nro;
        nro = TLinAlg::Norm2(r);
        k++;
    }
}

void TVizMapFactory::MakeFlat(PSVMTrainSet Set,
        const TVizDistType& DistType, TVec<TFltV>& DocPointV,
        const int& MxStep, const int& MxSecs, const double& MnDiff,
        const bool& RndStartPos, PNotify Notify) {

    const int Len = Set->Len();
    //const int Dim = Set->Dim();
    const int EqN = (Len*(Len - 1))/2;
    Notify->OnStatus(TStr::Fmt("matrix size: %d x %d", EqN, 2*Len));

    TFltV x(Len), y(Len), dxdy(Len + Len);
    if (RndStartPos) {
        TRnd Rnd1(1), Rnd2(2);
        TLAMisc::FillRnd(x, Rnd1); TLAMisc::FillRnd(y, Rnd2);
    } else {
        EAssertR(DocPointV.Len() == Len, TStr::Fmt("%d == %d", DocPointV.Len(), Len));
        for (int PntN = 0; PntN < Len; PntN++) {
            x[PntN] = DocPointV[PntN][0];
            y[PntN] = DocPointV[PntN][1];
        }
    }
    TFltV b(EqN), z(2*Len);

    // prepare matrix B
    TSparseRowMatrix B; B.RowN = EqN; B.ColN = 2*Len; B.DocSpVV.Gen(EqN);
    for (int i = 0; i < B.RowN; i++) B.DocSpVV[i].Gen(4);

    // main loop
    int Step = 0; TTm StartTm = TTm::GetCurUniTm();
    int PrevTimeSec = 0;
    forever {
        // preparing matrix
        b.PutAll(0.0); z.PutAll(0.0);
        int k = 0;
        TVec<TIntFltKdV>& RowVV = B.DocSpVV;
        for (int i = 0; i < Len; i++) {
            for (int j = i+1; j < Len; j++) {
                TIntFltKdV& RowV = RowVV[k];
                RowV[0] = TIntFltKd(i, x[i] - x[j]);
                RowV[1] = TIntFltKd(j, x[j] - x[i]);
                RowV[2] = TIntFltKd(Len+i, y[i] - y[j]);
                RowV[3] = TIntFltKd(Len+j, y[j] - y[i]);
                double Dij = 0.0;
                switch (DistType) {
                    case vdtEucl:    Dij = Set->EuclDist(i, j); break;
                    case vdtCos: Dij = 1.0 - Set->DotProduct(i, j); break;
                    case vdtSqrtCos: Dij = 1.0 - sqrt(Set->DotProduct(i, j)); break;
                    default: TExcept::Throw("Wrong DistType!");
                }
                b[k] = (TMath::Sqr(Dij) - TMath::Sqr(x[j]-x[i]) - TMath::Sqr(y[j]-y[i])) / 2;
                k++;
            }
        }

        // solving linear system
        dxdy.PutAll(0.0);
        CG(B, b, dxdy, Notify);

        // prepare terminating conditions
        double Diff = TMath::Mx(TFlt::Abs(dxdy[0]), TFlt::Abs(dxdy[Len]));
        for (int i = 0; i < Len; i++) {
            x[i] += dxdy[i]; y[i] += dxdy[Len+i];
            Diff = TMath::Mx(Diff, TFlt::Abs(dxdy[i]), TFlt::Abs(dxdy[Len+i]));
        }
        Step = Step + 1;
        const int RunTimeSec =
          int(TTm::GetDiffMSecs(StartTm, TTm::GetCurUniTm()) / 1000);

        // check conditions
        if (PrevTimeSec < RunTimeSec) {
            PrevTimeSec = RunTimeSec;
            Notify->OnStatus(TStr::Fmt("[Step:%5d, Time:%4d, Diff %8.5f]\r", Step, RunTimeSec, Diff));
        }
        if ((MxStep > 0) && (Step > MxStep))  { break; }
        if ((MxSecs > 0) && (RunTimeSec > MxSecs))  { break; }
        if ((Diff >= 0.0) && (Diff < MnDiff))  { break; }
    }
    Notify->OnStatus("\n");

    DocPointV.Gen(Len, 0);
    for (int i = 0; i < Len; i++) {
        DocPointV.Add(TFltV::GetV(x[i], y[i]));
    }
}

void TVizMapFactory::NormalizePoints(TVec<TFltV>& PointV) {
    // normalize coordiantes between 0.02 and 0.98
    const int Dim = PointV[0].Len();
    for (int d = 0; d < Dim; d++) {
        double Min = PointV[0][d];
        double Max = PointV[0][d];
        for (int i = 1; i < PointV.Len(); i++) {
            const double PointVal = PointV[i][d];
            Min = TFlt::GetMn(Min, PointVal);
            Max = TFlt::GetMx(Max, PointVal);
        }
        double Diff = Max - Min; IAssert(Diff > 0);
        for (int i = 0; i < PointV.Len(); i++) {
            const double OldVal = PointV[i][d];
            const double NewVal = 0.96*(OldVal - Min)/Diff + 0.02;
            EAssertR(0.0 <= NewVal && NewVal <= 1.0,
              TStr::Fmt("%d:%d %g %g %g %g %g", i,
              PointV.Len(), Min, Max, Diff, OldVal, NewVal));
            PointV[i][d] = NewVal;
        }
    }
}

void TVizMapFactory::LsiMds(TVec<PBowSpV> DocSpV, PSemSpace SemSpace,
        TVec<TFltV>& DocPointV, const double& SemSpaceThresh,
        const int& MxStep, const int& MxSecs, const double& MnDiff,
        PNotify Notify) {

    double ApproxVal;
    Notify->OnStatus(TStr::Fmt("Approximating %d docs ...\n", DocSpV.Len()));
    int SemSpcDim = SemSpace->GetDimNeededForApprox(
        DocSpV, SemSpaceThresh, ApproxVal, 3);
    EAssert((SemSpcDim == -1) || (SemSpcDim > 0));
    if (SemSpcDim == -1) { SemSpcDim = SemSpace->GetVecs(); }
    Notify->OnStatus(TStr::Fmt("Norm(dim:%d) = [avg: %.3f, mn:%.3f]",
        SemSpcDim, ApproxVal, SemSpaceThresh));

    Notify->OnStatus("Multidimensional Scaling:");
    const int Docs = DocSpV.Len();
    TVec<PBowSpV> ProjDocSpV(Docs, 0);
    for (int DocN = 0; DocN < Docs; DocN++) {
        ProjDocSpV.Add(SemSpace->ProjectSpV(DocSpV[DocN], SemSpcDim, false)); }
    PSVMTrainSet DocSet = TBowDocBs2TrainSet::NewBowNoCat(ProjDocSpV);
    MakeFlat(DocSet, vdtEucl, DocPointV, MxStep,
        MxSecs, MnDiff, DocPointV.Empty(), Notify);
    NormalizePoints(DocPointV);
}

PVizMapFrame TVizMapFactory::DocLsiMds(PBowDocWgtBs BowDocWgtBs,
        PSemSpace SemSpace, const TVec<TFltV>& _DocPointV,
        const double& SemSpaceThresh, const int& MxStep,
        const int& MxSecs, const double& MnDiff, PNotify Notify) {

    // calculate positions for documents
    const int Docs = BowDocWgtBs->GetDocs();
    TVec<PBowSpV> DocSpV(Docs, 0);
    for (int DIdN = 0; DIdN < Docs; DIdN++) {
        const int DId = BowDocWgtBs->GetDId(DIdN);
        DocSpV.Add(BowDocWgtBs->GetSpV(DId));
    }
    TVec<TFltV> DocPointV = _DocPointV;
    LsiMds(DocSpV, SemSpace, DocPointV, SemSpaceThresh, MxStep, MxSecs, MnDiff, Notify);

    // generate a VizMapFrame from positions
    PVizMapFrame VizMapFrame = TVizMapFrame::New();
    // add document points
    for (int DIdN = 0; DIdN < Docs; DIdN++) {
        const int DId = BowDocWgtBs->GetDId(DIdN);
        PVizMapPoint DocPoint = TVizMapPoint::New(DocPointV[DIdN]);
        DocPoint->PutDocId(DId); VizMapFrame->AddPoint(DocPoint);
    }
    VizMapFrame->CalcDId2PointNMap();
    // finish...
    return VizMapFrame;
}

void TVizMapFactory::AddDocMetadata(PVizMapFrame VizMapFrame,
        PBowDocBs PointBowDocBs, PBowDocBs KeyWdBowDocBs,
        PBowDocWgtBs KeyWdBowDocWgtBs, PVizMapLndMrk VizMapLndMrk) {

    // categories
    for (int CId = 0; CId < PointBowDocBs->GetCats(); CId++) {
        TStr CatNm = PointBowDocBs->GetCatNm(CId);
        const int NewCId = VizMapFrame->AddCatNm(CatNm);
        IAssert(NewCId == CId);
    }
    // document names and content
    VizMapFrame->PutPointBow(PointBowDocBs);
    const int PointDocs = PointBowDocBs->GetDocs();
    for (int PointDId = 0; PointDId < PointDocs; PointDId++) {
        if (VizMapFrame->IsInDId(PointDId)) {
            const int PointN = VizMapFrame->GetPointN(PointDId);
            VizMapFrame->GetPoint(PointN)->PutPointNm(PointBowDocBs->GetDocNm(PointDId));
            const int PointCIds = PointBowDocBs->GetDocCIds(PointDId);
            for (int PointCIdN = 0; PointCIdN < PointCIds; PointCIdN++) {
                const int PointCId = PointBowDocBs->GetDocCId(PointDId, PointCIdN);
                VizMapFrame->AddCat(PointN, PointCId);
            }
        }
    }
    // keywords data
    if (VizMapLndMrk.Empty()) {
        TBowSpVV KeyWdSpVV;
        const int Points = VizMapFrame->GetPoints();
        for (int PointN = 0; PointN <  Points; PointN++) {
            const int PointDId = VizMapFrame->GetPoint(PointN)->GetDocId();
            PBowSpV KeyWdSpV = KeyWdBowDocWgtBs->GetSpV(PointDId);
            KeyWdSpVV.Add(KeyWdSpV);
        }
        VizMapFrame->PutKeyWdBow(KeyWdBowDocBs, KeyWdSpVV);
    } else {
        TBowSpVV KeyWdSpVV;
        const int LndMrks = VizMapLndMrk->GetLndMrks();
        for (int LndMrkN = 0; LndMrkN < LndMrks; LndMrkN++) {
            PVizMapPoint KeyWdPoint =
                TVizMapPoint::New(VizMapLndMrk->GetLndMrkPointV(LndMrkN));
            VizMapFrame->AddKeyWdPoint(KeyWdPoint);
            PBowSpV KeyWdSpV = VizMapLndMrk->GetLndMrkSpV(LndMrkN);
            KeyWdSpVV.Add(KeyWdSpV);
        }
        VizMapFrame->PutKeyWdBow(KeyWdBowDocBs, KeyWdSpVV);
    }
}

void TVizMapFactory::LndMrk(PBowDocWgtBs BowDocWgtBs, PBowSim BowSim,
        const TVec<PBowSpV>& ClustSpV, const TVec<TFltV>& ClustPointV,
        TVec<TFltV>& DocPointV, const int& LinCombNum, PNotify Notify) {

    const int Clusts = ClustSpV.Len();
    const int TopClustN = TInt::GetMn(LinCombNum, Clusts);
    const int Docs = BowDocWgtBs->GetDocs();
    Notify->OnStatus(TStr::Fmt("Positioning %d documents using %d landmarks:", Docs, Clusts));

    DocPointV.Gen(Docs, 0);
    for (int DocN = 0; DocN < Docs; DocN++) {
        Notify->OnStatus(TStr::Fmt("%d\r", DocN));
        const int DId = BowDocWgtBs->GetDId(DocN);
        PBowSpV DocSpV = BowDocWgtBs->GetSpV(DId);
        // get the top closest clusters for the current document
        TFltIntKdV SimClustV(Clusts, 0);
        for (int ClustId = 0; ClustId < Clusts; ClustId++) {
            const double ClustSim = BowSim->GetSim(DocSpV, ClustSpV[ClustId]);
            SimClustV.Add(TFltIntKd(ClustSim, ClustId));
        }
        SimClustV.Sort(false);
        // normalize the similarity to TopClustN
        double SimSum = 0.0;
        for (int ClustN = 0; ClustN < TopClustN; ClustN++) {
            SimSum += SimClustV[ClustN].Key; }
        // calculate document position based on top TopClustN clusters
        DocPointV.Add(TFltV::GetV(0.0, 0.0));
        for (int ClustN = 0; ClustN < TopClustN; ClustN++) {
            const double SumWgt = (SimSum > 1e-7) ?
                SimClustV[ClustN].Key / SimSum : 0.0;
            const int ClustId = SimClustV[ClustN].Dat;
            TLinAlg::AddVec(SumWgt, ClustPointV[ClustId],
                DocPointV.Last(), DocPointV.Last());
        }
    }
    Notify->OnStatus("");
}

PVizMapFrame TVizMapFactory::DocLndMrk(PBowDocWgtBs BowDocWgtBs,
        const TVec<PBowSpV>& ClustSpV, const TVec<TFltV>& ClustPointV,
        const int& LinCombNum, PNotify Notify) {

    // calculate positons for
    TVec<TFltV> DocPointV;
    PBowSim BowSim = TBowSim::New(bstCos);
    LndMrk(BowDocWgtBs, BowSim, ClustSpV, ClustPointV, DocPointV, LinCombNum, Notify);

    // generate a VizMapFrame from positions
    PVizMapFrame VizMapFrame = TVizMapFrame::New();
    // add document points
    const int Docs = BowDocWgtBs->GetDocs();
    for (int DIdN = 0; DIdN < Docs; DIdN++) {
        const int DId = BowDocWgtBs->GetDId(DIdN);
        PVizMapPoint DocPoint = TVizMapPoint::New(DocPointV[DIdN]);
        DocPoint->PutDocId(DId); VizMapFrame->AddPoint(DocPoint);
    }
    VizMapFrame->CalcDId2PointNMap();
    // finish...
    return VizMapFrame;
}


PVizMapFrame TVizMapFactory::ClustLsiMdsDocLndMrk(PBowDocWgtBs BowDocWgtBs,
        PBowDocPart BowDocPart, PSemSpace SemSpace, const int& LinCombNum,
        const double& SemSpaceThresh, const int& MxStep, const int& MxSecs,
        const double& MnDiff, PNotify Notify) {

    // calculate positions for clusters' centroids
    const int Clusts = BowDocPart->GetClusts();
    TVec<PBowSpV> ClustSpV(Clusts, 0);
    for (int ClustN = 0; ClustN < Clusts; ClustN++) {
        ClustSpV.Add(BowDocPart->GetClust(ClustN)->GetConceptSpV()); }
    TVec<TFltV> ClustPointV; EAssert(ClustPointV.Empty());
    LsiMds(ClustSpV, SemSpace, ClustPointV,
        SemSpaceThresh, MxStep, MxSecs, MnDiff, Notify);

    // finish...
    return DocLndMrk(BowDocWgtBs, ClustSpV, ClustPointV, LinCombNum, Notify);
}

PVizMapFrame TVizMapFactory::NewVizMapFrame(PBowDocBs BowDocBs, PBowDocWgtBs BowDocWgtBs,
        PBowDocBs KeyWdBowDocBs, PBowDocWgtBs KeyWdBowDocWgtBs, const int& ThDocs,
        const int& Clusts, const double& SvdThreshold, const PNotify& Notify,
        const bool& LndPointWgt, const bool& CalcLandscapeP) {

    TIntV AllDIdV; BowDocBs->GetAllDIdV(AllDIdV); PVizMapFrame VizMapFrame;
    Notify->OnStatus(TStr::Fmt("Size of dataset: %d docs", BowDocBs->GetDocs()));
    if (BowDocBs->GetDocs() < ThDocs) {
        Notify->OnStatus("Calculating semantic space ... ");
        PSemSpace SemSpace = TSemSpaceAlg::CalcFullLSI(
            BowDocWgtBs, AllDIdV, BowDocBs->GetDocs());
        Notify->OnStatus("Calculating visualization map ... ");
        VizMapFrame = TVizMapFactory::DocLsiMds(BowDocWgtBs, SemSpace,
            TVec<TFltV>(), 0.93 * SvdThreshold, 5000, 500, 0.0001, Notify);
        Notify->OnStatus("Adding metadata ... ");
        TVizMapFactory::AddDocMetadata(VizMapFrame,
            KeyWdBowDocBs, KeyWdBowDocBs, KeyWdBowDocWgtBs);
        Notify->OnStatus("Calculating background landscapes ... ");
        if (CalcLandscapeP && KeyWdBowDocBs->IsCats()) {
            VizMapFrame->AddLandscape(400, 400, 0.02, LndPointWgt);
        } else if (CalcLandscapeP) {
            VizMapFrame->AddLandscape(400, 400, 0.02, LndPointWgt);
        }
    } else {
        Notify->OnStatus("Using large-scale pipeline ...");
        PBowSim BowSim = TBowSim::New(bstCos);
        TRnd Rnd(1);
        PBowDocPart BowDocPart = TBowClust::GetKMeansPartForDocWgtBs(
            Notify, BowDocWgtBs, BowDocBs, BowSim, Rnd, Clusts, 1, 10, 1);
        Notify->OnStatus("Calculating LSI ...");
        const int LndMrks = BowDocPart->GetClusts();
        TVec<PBowSpV> LndMrkSpV(LndMrks, 0);
        for (int LndMrkN = 0; LndMrkN < LndMrks; LndMrkN++) {
            LndMrkSpV.Add(BowDocPart->GetClust(LndMrkN)->GetConceptSpV()); }
        PSemSpace SemSpace = TSemSpaceAlg::CalcFullLSI(LndMrkSpV, LndMrks-1);
        Notify->OnStatus("Calculating visualization map ... ");
        VizMapFrame = TVizMapFactory::ClustLsiMdsDocLndMrk(BowDocWgtBs, BowDocPart,
            SemSpace, 3, SvdThreshold, 5000, 500, 0.0001, Notify);
        Notify->OnStatus("Adding metadata ... ");
        TVizMapFactory::AddDocMetadata(VizMapFrame,
            KeyWdBowDocBs, KeyWdBowDocBs, KeyWdBowDocWgtBs);
        Notify->OnStatus("Calculating background landscapes ... ");
        double BellSize = 0.02;
        if (BowDocBs->GetDocs() > 1500) BellSize *= 0.7;
        if (BowDocBs->GetDocs() > 2000) BellSize *= 0.7;
        if (BowDocBs->GetDocs() > 3000) BellSize *= 0.5;
        if (BowDocBs->GetDocs() > 4000) BellSize *= 0.5;
        if (CalcLandscapeP && KeyWdBowDocBs->IsCats()) {
            VizMapFrame->AddLandscape(400, 400, BellSize, LndPointWgt);
        } else if (CalcLandscapeP) {
            VizMapFrame->AddLandscape(400, 400, BellSize, LndPointWgt);
        }
    }

    Notify->OnStatus("Done ... ");
    return VizMapFrame;
}

PVizMapFrame TVizMapFactory::NewVizMapFrame(PBowDocBs BowDocBs, const int& ThDocs,
        const int& Clusts, const double& SvdThreshold, const PNotify& Notify,
        const bool& LndPointWgt, const bool& CalcLandscapeP) {

    PBowDocWgtBs BowDocWgtBs = TBowDocWgtBs::New(BowDocBs, bwwtLogDFNrmTFIDF);
    return NewVizMapFrame(BowDocBs, BowDocWgtBs, BowDocBs, BowDocWgtBs,
        ThDocs, Clusts, SvdThreshold, Notify, LndPointWgt, CalcLandscapeP);
}

PVizMapFrame TVizMapFactory::NewVizMapFrameFromLndMrk(PBowDocBs BowDocBs,
        PBowDocWgtBs BowDocWgtBs, PVizMapLndMrk VizMapLndMrk, const bool& LndMrkLndP,
        const bool& LndMrkKeyWdP, const PNotify& Notify) { // TODO use LndMrkKeyWdP

    // position documents using landmarks from the LndMrk
    Notify->OnStatus(TStr::Fmt("Size of dataset: %d docs", BowDocWgtBs->GetDocs()));
    Notify->OnStatus("Calculating visualization map ... ");
    const TBowSpVV& LndMrkSpVV = VizMapLndMrk->GetLndMrkSpVV();
    const TVec<TFltV>& LndMrkPointVV = VizMapLndMrk->GetLndMrkPointVV();
    PVizMapFrame VizMapFrame = DocLndMrk(BowDocWgtBs,
        LndMrkSpVV, LndMrkPointVV, 20, Notify);
    // add metadata to the points
    Notify->OnStatus("Adding metadata ... ");
    if (LndMrkKeyWdP) {
        AddDocMetadata(VizMapFrame, BowDocBs, VizMapLndMrk->GetBowDocBs(),
            VizMapLndMrk->GetBowDocWgtBs(), VizMapLndMrk);
    } else {
        AddDocMetadata(VizMapFrame, BowDocBs, BowDocBs, BowDocWgtBs);
    }
    // calculate landscape
    Notify->OnStatus("Calculating background landscapes ... ");
    double BellSize = 0.02;
    if (BowDocBs->GetDocs() > 1500) BellSize *= 0.7;
    if (BowDocBs->GetDocs() > 2000) BellSize *= 0.7;
    if (BowDocBs->GetDocs() > 3000) BellSize *= 0.5;
    if (BowDocBs->GetDocs() > 4000) BellSize *= 0.5;
    if (!LndMrkLndP && BowDocBs->IsCats()) {
        VizMapFrame->AddLandscape(400, 400, BellSize, false);
    } else if (!LndMrkLndP) {
        VizMapFrame->AddLandscape(400, 400, BellSize, false);
    } else {
        VizMapFrame->AddLandscapeFromLndMrk(400, 400, BellSize, LndMrkPointVV);
    }
    Notify->OnStatus("Done ... ");
    return VizMapFrame;
}

//////////////////////////////////////////////////////////////////////////
// Visualization-for-Dummys
PVizMap TVizMapFactory::NewVizMap(PBowDocBs BowDocBs, PBowDocWgtBs BowDocWgtBs,
        PBowDocBs KeyWdBowDocBs, PBowDocWgtBs KeyWdBowDocWgtBs, const int& ThDocs,
        const int& Clusts, const double& SvdThreshold, const PNotify& Notify,
        const bool& LndPointWgtP, const bool& CalcLandscapeP) {

    return TVizMap::New(NewVizMapFrame(BowDocBs, BowDocWgtBs, KeyWdBowDocBs,
        KeyWdBowDocWgtBs, ThDocs, Clusts, SvdThreshold, Notify, LndPointWgtP,
        CalcLandscapeP));
}

PVizMap TVizMapFactory::NewVizMap(PBowDocBs BowDocBs, const int& ThDocs,
        const int& Clusts, const double& SvdThreshold, const PNotify& Notify,
        const bool& LndPointWgtP, const bool& CalcLandscapeP) {

    return TVizMap::New(NewVizMapFrame(BowDocBs, ThDocs, Clusts,
        SvdThreshold, Notify, LndPointWgtP, CalcLandscapeP));
}

//////////////////////////////////////////////////////////////////////////
// Visualization-of-DAX-strucutres (Document-Atlas-Xml format)
PVizMap TVizMapFactory::NewVizMapStaticDoc(const PVizMapXmlDocBs& XmlDocBs,
        PSwSet SwSet, PStemmer Stemmer, const int& ThDocs, const int& Clusts,
        const double& SvdThreshold, const PNotify& Notify, const bool& LndPointWgt,
        const bool& CalcLandscapeP) {

    // load documents to bow
    PBowDocBs BowDocBs = TBowDocBs::New(SwSet, Stemmer, NULL);
    const TVizMapXmlDocV& XmlDocV = XmlDocBs->XmlDocV;
    for (int XmlDocN = 0; XmlDocN < XmlDocV.Len(); XmlDocN++) {
        // read stuff from document
        const TVizMapXmlDoc& XmlDoc = XmlDocV[XmlDocN];
        // add it to the bow
        const int DId = XmlDoc.AddToBowDocBs(XmlDocBs, BowDocBs);
        IAssertR(DId == XmlDocN, TStr::Fmt("%d-%d", DId, XmlDocN));
    }
    // finish ...
    return NewVizMap(BowDocBs, ThDocs, Clusts, SvdThreshold,
        Notify, LndPointWgt, CalcLandscapeP);
}

PVizMap TVizMapFactory::NewVizMapDynamicDoc(const PVizMapXmlDocBs& XmlDocBs,
        PSwSet SwSet, PStemmer Stemmer, const int& ThDocs, const int& Clusts,
        const double& SvdThreshold, const PNotify& Notify, const bool& LndPointWgt,
        const bool& CalcLandscapeP) {

    // load documents to bow
    TIntStrH FrameSortNToNmH; TIntIntVH FrameSortNToDIdVH;
    PBowDocBs BowDocBs = TBowDocBs::New(SwSet, Stemmer, NULL);
    const TVizMapXmlDocV& XmlDocV = XmlDocBs->XmlDocV;
    for (int XmlDocN = 0; XmlDocN < XmlDocV.Len(); XmlDocN++) {
        // read stuff from document
        const TVizMapXmlDoc& XmlDoc = XmlDocV[XmlDocN];
        // frame information
        const int FrameSortN = XmlDoc.FrameSortN;
        const TStr& FrameNm = XmlDoc.FrameNm;
        if (FrameSortNToNmH.IsKey(FrameSortN)) {
            IAssertR(FrameSortNToNmH.GetDat(FrameSortN) == FrameNm,
                FrameSortNToNmH.GetDat(FrameSortN) + " != " + FrameNm);
        } else {
            FrameSortNToNmH.AddDat(FrameSortN, FrameNm);
        }
        // add it to the bow
        const int DId = XmlDoc.AddToBowDocBs(XmlDocBs, BowDocBs);
        IAssertR(DId == XmlDocN, TStr::Fmt("%d-%d", DId, XmlDocN));
        FrameSortNToDIdVH.AddDat(FrameSortN).Add(DId);
    }
    // sort frames by sort number
    FrameSortNToNmH.SortByKey();
    // prepare map with landmarks
    PVizMapLndMrk VizMapLndMrk = TVizMapFactory::NewVizMapLndMrk(
        BowDocBs, TIntV(), ThDocs, Clusts, SvdThreshold, Notify);
    // make VizMaps for each frame
    PVizMap VizMap = TVizMap::New();
    Notify->OnStatus(TStr::Fmt("Processing %d frames ... ", FrameSortNToNmH.Len()));
    int FrameKeyId = FrameSortNToNmH.FFirstKeyId();
    while (FrameSortNToNmH.FNextKeyId(FrameKeyId)) {
        const int FrameSortN = FrameSortNToNmH.GetKey(FrameKeyId);
        // place authors on the map
        const TIntV& FrameDIdV = FrameSortNToDIdVH.GetDat(FrameSortN);
        Notify->OnStatus(TStr::Fmt("Frame %d/%d (%d)... ",
            FrameKeyId+1, FrameSortNToNmH.Len(), FrameDIdV.Len()));
        PBowDocWgtBs BowDocWgtBs = TBowDocWgtBs::New(
            BowDocBs, bwwtLogDFNrmTFIDF, 0, 0, FrameDIdV);
        PVizMapFrame VizMapFrame = NewVizMapFrameFromLndMrk(BowDocBs,
            BowDocWgtBs, VizMapLndMrk, false, false, Notify);
        // add metadata to the frame
        Notify->OnStatus("Adding metadata ... ");
        VizMapFrame->PutFrameNm(FrameSortNToNmH.GetDat(FrameSortN));
        // add frame to vizmap
        VizMap->AddVizMapFrame(VizMapFrame, true);
    }
    Notify->OnStatus("Done");
    return VizMap;
}

PVizMap TVizMapFactory::NewVizMapStaticAuthor(const PVizMapXmlDocBs& XmlDocBs,
        PSwSet SwSet, PStemmer Stemmer, const int& MxAuthors, const int& ThDocs,
        const int& Clusts, const double& SvdThreshold, const PNotify& Notify,
        const bool& LndPointWgt, const bool& CalcLandscapeP) {

    const TVizMapXmlDocV& XmlDocV = XmlDocBs->XmlDocV;
    // count authors
    TIntH AuthorIdToFqH;
    for (int XmlDocN = 0; XmlDocN < XmlDocV.Len(); XmlDocN++) {
        const TVizMapXmlDoc& XmlDoc = XmlDocV[XmlDocN];
        for (int AuthorIdN = 0; AuthorIdN < XmlDoc.AuthorIdV.Len(); AuthorIdN++) {
            const int AuthorId = XmlDoc.AuthorIdV[AuthorIdN];
            AuthorIdToFqH.AddDat(AuthorId)++;
        }
    }
    IAssert(!AuthorIdToFqH.Empty()); AuthorIdToFqH.SortByDat(false);
    // select and index the most frequent ones
    TIntH FqAuthorIdH;
    int AuthorKeyId = AuthorIdToFqH.FFirstKeyId();
    while (AuthorIdToFqH.FNextKeyId(AuthorKeyId)) {
        if (FqAuthorIdH.Len() == MxAuthors) { break; }
        const int AuthorId = AuthorIdToFqH.GetKey(AuthorKeyId);
        FqAuthorIdH.AddKey(AuthorId);
    }
    // prepare documets for authors
    THash<TInt, TChA> AuthorIdToBodyH, AuthorIdToDisplayBodyH;
    TIntStrVH AuthorIdToCatNmVH;
    for (int XmlDocN = 0; XmlDocN < XmlDocV.Len(); XmlDocN++) {
        const TVizMapXmlDoc& XmlDoc = XmlDocV[XmlDocN];
        // update author profiles with document
        for (int AuthorIdN = 0; AuthorIdN < XmlDoc.AuthorIdV.Len(); AuthorIdN++) {
            const int AuthorId = XmlDoc.AuthorIdV[AuthorIdN];
            // skip non-frequnet authors
            if (!FqAuthorIdH.IsKey(AuthorId)) { continue; }
            // add content
            AuthorIdToBodyH.AddDat(AuthorId) += XmlDoc.Nm + " -- " + XmlDoc.Body + "\r\n";
            TStr DisplayBody = XmlDoc.DisplayBody.Empty() ? XmlDoc.Body : XmlDoc.DisplayBody;
            AuthorIdToDisplayBodyH.AddDat(AuthorId) += XmlDoc.Nm + " -- " + DisplayBody + "\r\n";
            // add categories
            for (int CatIdN = 0; CatIdN < XmlDoc.CatIdV.Len(); CatIdN++) {
                const int CatId = XmlDoc.CatIdV[CatIdN];
                AuthorIdToCatNmVH.AddDat(AuthorId).Add(
                    XmlDocBs->CatNmH.GetKey(CatId));
            }
        }
    }
    // load documents to bow
    PBowDocBs BowDocBs = TBowDocBs::New(SwSet, Stemmer, NULL);
    TIntH AuthorIdToDIdH; TIntV AuthorDIdV;
    AuthorKeyId = AuthorIdToBodyH.FFirstKeyId();
    while (AuthorIdToBodyH.FNextKeyId(AuthorKeyId)) {
        const int AuthorId = AuthorIdToBodyH.GetKey(AuthorKeyId);
        TStr DocNm = XmlDocBs->AuthorNmH.GetKey(AuthorId);
        TStr DocBody = AuthorIdToBodyH.GetDat(AuthorId);
        const TStrV& CatNmV = AuthorIdToCatNmVH.AddDat(AuthorId);
        const int DId = BowDocBs->AddHtmlDoc(DocNm, CatNmV, DocBody, false);
        AuthorDIdV.Add(DId);
        BowDocBs->PutDocStr(DId, AuthorIdToDisplayBodyH.GetDat(AuthorId));
        // remember map from author to it's document
        AuthorIdToDIdH.AddDat(AuthorId, DId);
    }
    // make viz map frame
    PVizMapFrame VizMapFrame = NewVizMapFrame(BowDocBs, 300, 200, 0.8, Notify, false, true);
    // add weights
    const int MxAuthorFq = AuthorIdToFqH[0];
    AuthorKeyId = AuthorIdToDIdH.FFirstKeyId();
    while (AuthorIdToDIdH.FNextKeyId(AuthorKeyId)) {
        const int AuthorId = AuthorIdToDIdH.GetKey(AuthorKeyId);
        const int AuthorDId = AuthorIdToDIdH[AuthorKeyId];
        const int AuthorFq = AuthorIdToFqH.GetDat(AuthorId);
        const double AuthorWgt = double(AuthorFq) / double(MxAuthorFq);
        const int PointN = VizMapFrame->GetPointN(AuthorDId);
        VizMapFrame->GetPoint(PointN)->PutWgt(AuthorWgt);
    }
    // make viz map
    return TVizMap::New(VizMapFrame);
}

PVizMap TVizMapFactory::NewVizMapDynamicAuthor(const PVizMapXmlDocBs& XmlDocBs,
        PSwSet SwSet, PStemmer Stemmer, const int& MxFrames,
        const int& MxAuthors, const int& ThDocs,
        const int& Clusts, const double& SvdThreshold, const PNotify& Notify,
        const bool& LndPointWgt, const bool& CalcLandscapeP) {

    const TVizMapXmlDocV& XmlDocV = XmlDocBs->XmlDocV;
    // count authors
    TIntH AuthorIdToFqH;
    for (int XmlDocN = 0; XmlDocN < XmlDocV.Len(); XmlDocN++) {
        const TVizMapXmlDoc& XmlDoc = XmlDocV[XmlDocN];
        for (int AuthorIdN = 0; AuthorIdN < XmlDoc.AuthorIdV.Len(); AuthorIdN++) {
            const int AuthorId = XmlDoc.AuthorIdV[AuthorIdN];
            AuthorIdToFqH.AddDat(AuthorId)++;
        }
    }
    IAssert(!AuthorIdToFqH.Empty()); AuthorIdToFqH.SortByDat(false);
    // select and index the most frequent ones
    TIntH FqAuthorIdH;
    int AuthorKeyId = AuthorIdToFqH.FFirstKeyId();
    while (AuthorIdToFqH.FNextKeyId(AuthorKeyId)) {
        if (FqAuthorIdH.Len() == MxAuthors) { break; }
        const int AuthorId = AuthorIdToFqH.GetKey(AuthorKeyId);
        FqAuthorIdH.AddKey(AuthorId);
    }
    // prepare documets for most frequent authors and sort them by frames
    // and start filling bow with landmarks
    PBowDocBs BowDocBs = TBowDocBs::New(SwSet, Stemmer, NULL);
    TIntStrH FrameSortNToNmH; TIntIntHH FrameSortNToAuthorIdHH;
    THash<TIntPr, TChA> FrameAuthorIdToBodyH, FrameAuthorIdToDisplayBodyH;
    TIntPrStrVH FrameAuthorIdToCatNmVH; TIntV DocDIdV;
    for (int XmlDocN = 0; XmlDocN < XmlDocV.Len(); XmlDocN++) {
        // read stuff from document
        const TVizMapXmlDoc& XmlDoc = XmlDocV[XmlDocN];
        // frame information
        const int FrameSortN = XmlDoc.FrameSortN;
        const TStr& FrameNm = XmlDoc.FrameNm;
        if (FrameSortNToNmH.IsKey(FrameSortN)) {
            IAssertR(FrameSortNToNmH.GetDat(FrameSortN) == FrameNm,
                FrameSortNToNmH.GetDat(FrameSortN) + " != " + FrameNm);
        } else {
            FrameSortNToNmH.AddDat(FrameSortN, FrameNm);
        }
        // author information
        for (int AuthorIdN = 0; AuthorIdN < XmlDoc.AuthorIdV.Len(); AuthorIdN++) {
            const int AuthorId = XmlDoc.AuthorIdV[AuthorIdN];
            // skip non-frequnet authors
            if (!FqAuthorIdH.IsKey(AuthorId)) { continue; }
            // add author to the frame
            TIntPr FrameAuthorId(FrameSortN, AuthorId);
            FrameSortNToAuthorIdHH.AddDat(FrameSortN).AddDat(AuthorId)++;
            // add content
            FrameAuthorIdToBodyH.AddDat(FrameAuthorId) +=
                XmlDoc.Nm + " -- " + XmlDoc.Body + "\r\n";
            TStr DisplayBody = XmlDoc.DisplayBody.Empty() ?
                XmlDoc.Body : XmlDoc.DisplayBody;
            FrameAuthorIdToDisplayBodyH.AddDat(FrameAuthorId) +=
                XmlDoc.Nm + " -- " + DisplayBody + "\r\n";
            // add categories
            for (int CatIdN = 0; CatIdN < XmlDoc.CatIdV.Len(); CatIdN++) {
                const int CatId = XmlDoc.CatIdV[CatIdN];
                FrameAuthorIdToCatNmVH.AddDat(FrameAuthorId).Add(
                    XmlDocBs->CatNmH.GetKey(CatId));
            }
        }
        // add it to the bow for generation of semantic landscape
        const int DId = XmlDoc.AddToBowDocBs(XmlDocBs, BowDocBs); DocDIdV.Add(DId);
        IAssertR(DId == XmlDocN, TStr::Fmt("%d-%d", DId, XmlDocN));
    }
    // sort frames by sort number
    FrameSortNToNmH.SortByKey();
    // load documents to bow
    TIntPrIntH FrameAuthorIdToDIdH; TIntIntVH FrameToDIdVH;
    int FrameAuthorKeyId = FrameAuthorIdToBodyH.FFirstKeyId();
    while (FrameAuthorIdToBodyH.FNextKeyId(FrameAuthorKeyId)) {
        const TIntPr& FrameAuthorId = FrameAuthorIdToBodyH.GetKey(FrameAuthorKeyId);
        const int FrameSortN = FrameAuthorId.Val1;
        const int AuthorId = FrameAuthorId.Val2;
        // read document and add it to bow
        TStr DocNm = TStr::Fmt("%s [[%d]]",
            XmlDocBs->AuthorNmH.GetKey(AuthorId).CStr(), FrameSortN);
        TStr DocBody = FrameAuthorIdToBodyH.GetDat(FrameAuthorId);
        const TStrV& CatNmV = FrameAuthorIdToCatNmVH.GetDat(FrameAuthorId);
        const int DId = BowDocBs->AddHtmlDoc(DocNm, CatNmV, DocBody, false);
        BowDocBs->PutDocStr(DId, FrameAuthorIdToDisplayBodyH.GetDat(FrameAuthorId));
        // remamber frame autor id to document id map
        FrameAuthorIdToDIdH.AddDat(FrameAuthorId, DId);
        FrameToDIdVH.AddDat(FrameSortN).Add(DId);
    }
    // prepare landmarks
    PVizMapLndMrk VizMapLndMrk = TVizMapFactory::NewVizMapLndMrk(
        BowDocBs, DocDIdV, ThDocs, Clusts, SvdThreshold, Notify);
    // make VizMaps for each frame
    PVizMap VizMap = TVizMap::New();
    Notify->OnStatus(TStr::Fmt("Processing %d frames ... ", FrameSortNToNmH.Len()));
    int FrameKeyId = FrameSortNToNmH.FFirstKeyId();
    while (FrameSortNToNmH.FNextKeyId(FrameKeyId)) {
        const int FrameSortN = FrameSortNToNmH.GetKey(FrameKeyId);
        // place authors on the map
        Notify->OnStatus(TStr::Fmt("Frame %d/%d... ",
            FrameKeyId+1, FrameSortNToNmH.Len()));
        const TIntV& FrameDIdV = FrameToDIdVH.GetDat(FrameSortN);
        PBowDocWgtBs BowDocWgtBs = TBowDocWgtBs::New(
            BowDocBs, bwwtLogDFNrmTFIDF, 0, 0, FrameDIdV);
        PVizMapFrame VizMapFrame = NewVizMapFrameFromLndMrk(BowDocBs,
            BowDocWgtBs, VizMapLndMrk, false, false, Notify);
        // add metadata to the frame
        Notify->OnStatus("Adding metadata ... ");
        VizMapFrame->PutFrameNm(FrameSortNToNmH.GetDat(FrameSortN));
        // add weights do points
        const TIntH& AuthorIdH = FrameSortNToAuthorIdHH.GetDat(FrameSortN);
        int MxAuthorFq = 1, AuthorKeyId = AuthorIdH.FFirstKeyId();
        while (AuthorIdH.FNextKeyId(AuthorKeyId)) {
            MxAuthorFq = TInt::GetMx(MxAuthorFq, AuthorIdH[AuthorKeyId]); }
        AuthorKeyId = AuthorIdH.FFirstKeyId();
        while (AuthorIdH.FNextKeyId(AuthorKeyId)) {
            const int AuthorId = AuthorIdH.GetKey(AuthorKeyId);
            const int AuthorDId = FrameAuthorIdToDIdH.GetDat(TIntPr(FrameSortN, AuthorId));
            const int PointN = VizMapFrame->GetPointN(AuthorDId);
            const int AuthorFq = AuthorIdH[AuthorKeyId];
            const double AuthorWgt = double(AuthorFq) / double(MxAuthorFq);
            VizMapFrame->GetPoint(PointN)->PutWgt(AuthorWgt);
        }
        // add frame to vizmap
        VizMap->AddVizMapFrame(VizMapFrame, true);
    }
    Notify->OnStatus("Done");
    return VizMap;
}

PVizMap TVizMapFactory::NewVizMap(const PVizMapXmlDocBs& XmlDocBs,
        const TVizXmlMapType& VizXmlMapType, PSwSet SwSet, PStemmer Stemmer,
        const int& ThDocs, const int& Clusts, const double& SvdThreshold,
        const PNotify& Notify, const bool& LndPointWgt, const bool& CalcLandscapeP) {

    // act according to MapType
    if (VizXmlMapType == vxmtStaticDoc) {
        // map with static documents
        return NewVizMapStaticDoc(XmlDocBs, SwSet, Stemmer, ThDocs,
            Clusts, SvdThreshold, Notify, LndPointWgt, CalcLandscapeP);
    } else if (VizXmlMapType == vxmtDynamicAuthor) {
        // map with dynamic documents
        return NewVizMapDynamicDoc(XmlDocBs, SwSet, Stemmer, ThDocs,
            Clusts, SvdThreshold, Notify, LndPointWgt, CalcLandscapeP);
    } else if (VizXmlMapType == vxmtStaticAuthor) {
        // map with static authors
        return NewVizMapStaticAuthor(XmlDocBs, SwSet, Stemmer, -1, ThDocs,
            Clusts, SvdThreshold, Notify, LndPointWgt, CalcLandscapeP);
    } else if (VizXmlMapType == vxmtDynamicAuthor) {
        // map with dynamic authors
        return NewVizMapDynamicAuthor(XmlDocBs, SwSet, Stemmer, -1, -1, ThDocs,
            Clusts, SvdThreshold, Notify, LndPointWgt, CalcLandscapeP);
    }
    Fail; return NULL;
}

PVizMap TVizMapFactory::NewVizMap(const TStr& XmlFNm, const TVizXmlMapType& VizXmlMapType,
        PSwSet SwSet, PStemmer Stemmer, const int& ThDocs, const int& Clusts,
        const double& SvdThreshold, const PNotify& Notify, const bool& LndPointWgt,
        const bool& CalcLandscapeP) {

    // parse xml
    PVizMapXmlDocBs XmlDocBs = TVizMapXmlDocBs::New(XmlFNm, Notify);
    // make and return
    return NewVizMap(XmlDocBs, VizXmlMapType, SwSet, Stemmer,
        ThDocs, Clusts, SvdThreshold, Notify, LndPointWgt, CalcLandscapeP);
}

//////////////////////////////////////////////////////////////////////////
// Generation of LandMark-Maps
PVizMapLndMrk TVizMapFactory::NewVizMapLndMrk(PBowDocBs BowDocBs,
        PBowDocWgtBs BowDocWgtBs, const int& ThDocs, const int& Clusts,
        const double& SvdThreshold, const PNotify& Notify) {

    Notify->OnStatus(TStr::Fmt("Size of dataset: %d docs", BowDocWgtBs->GetDocs()));
    // load landmarks
    TVec<PBowSpV> LndMrkSpV;
    if (BowDocWgtBs->GetDocs() < ThDocs) {
        // we use the documents as landmarks
        const int Docs = BowDocWgtBs->GetDocs();
        LndMrkSpV.Gen(Docs, 0);
        for (int DIdN = 0; DIdN < Docs; DIdN++) {
            const int DId = BowDocWgtBs->GetDId(DIdN);
            PBowSpV DocSpV = BowDocWgtBs->GetSpV(DId);
            LndMrkSpV.Add(DocSpV);
        }
    } else {
        Notify->OnStatus("Using large-scale pipeline ...");
        // clusters documents to create landmarks
        PBowSim BowSim = TBowSim::New(bstCos);
        TRnd Rnd(1);
        PBowDocPart BowDocPart = TBowClust::GetKMeansPartForDocWgtBs(
            Notify, BowDocWgtBs, BowDocBs, BowSim, Rnd, Clusts, 1, 10, 1);
        // load landmarks
        const int Clusts = BowDocPart->GetClusts();
        LndMrkSpV.Gen(Clusts, 0);
        for (int ClustN = 0; ClustN < Clusts; ClustN++) {
            LndMrkSpV.Add(BowDocPart->GetClust(ClustN)->GetConceptSpV()); }
    }
    // calculate LSI on landmarks
    const int LndMrks = LndMrkSpV.Len();
    Notify->OnStatus(TStr::Fmt("Calculating LSI on %d landmarks ...", LndMrks));
    PSemSpace SemSpace = TSemSpaceAlg::CalcFullLSI(LndMrkSpV, LndMrks-1);
    // positiong landmarks on the map
    Notify->OnStatus("Calculating LndMrk map ... ");
    TVec<TFltV> LndMrkPointV;
    LsiMds(LndMrkSpV, SemSpace, LndMrkPointV, SvdThreshold, 5000, 500, 0.0001, Notify);
    Notify->OnStatus("Done ... ");
    return TVizMapLndMrk::New(BowDocBs, BowDocWgtBs, LndMrkSpV, LndMrkPointV);
}

PVizMapLndMrk TVizMapFactory::NewVizMapLndMrk(PBowDocBs BowDocBs,
        const TIntV& DIdV, const int& ThDocs, const int& Clusts,
        const double& SvdThreshold, const PNotify& Notify) {

    TIntV NewDIdV;
    if (DIdV.Empty()) { BowDocBs->GetAllDIdV(NewDIdV); } else { NewDIdV = DIdV; }
    PBowDocWgtBs BowDocWgtBs = TBowDocWgtBs::New(BowDocBs, bwwtLogDFNrmTFIDF, 0, 0, NewDIdV);
    return NewVizMapLndMrk(BowDocBs, BowDocWgtBs, ThDocs, Clusts, SvdThreshold, Notify);
}

//////////////////////////////////////////////////////////////////////////
// Visualization using Landmark-Maps
PVizMap TVizMapFactory::NewVizMapFromLndMrk(PBowDocBs BowDocBs, PBowDocWgtBs BowDocWgtBs,
        PVizMapLndMrk VizMapLndMrk, const bool& LndMrkLndP, const bool& LndMrkKeyWdP,
        const PNotify& Notify) {

    return TVizMap::New(NewVizMapFrameFromLndMrk(BowDocBs, BowDocWgtBs,
        VizMapLndMrk, LndMrkLndP, LndMrkKeyWdP, Notify));
}

PVizMap TVizMapFactory::NewVizMapFromLndMrk(PBowDocBs BowDocBs, const TIntV& DIdV,
        PVizMapLndMrk VizMapLndMrk, const bool& LndMrkLndP, const bool& LndMrkKeyWdP,
        const TBowWordWgtType& WgtType, const PNotify& Notify) {

    PBowDocWgtBs BowDocWgtBs = TBowDocWgtBs::New(BowDocBs, WgtType, 0, 0, DIdV);
    return TVizMap::New(NewVizMapFrameFromLndMrk(BowDocBs, BowDocWgtBs,
        VizMapLndMrk, LndMrkLndP, LndMrkKeyWdP, Notify));
}

//////////////////////////////////////////////////////////////////////////
// Vrml-Writer
void TVrml::InsertHeader(PSOut SOut, const TStr& SkyColor) {
    SOut->PutStrLn("#VRML V2.0 utf8");
    SOut->PutStrLn("WorldInfo {");
    SOut->PutStrLn("  title \"Semantic Wonderland\"");
    SOut->PutStrLn("  info [\"Spluf\"]");
    SOut->PutStrLn("}");
    SOut->PutStrLn("Background {");
    SOut->PutStrLn("  skyColor [0 0 0 , " + SkyColor + "]");
    SOut->PutStrLn("  skyAngle [3.14]");
    SOut->PutStrLn("}");
    SOut->PutStrLn("PointLight {");
    SOut->PutStrLn("  location 0 3.5 1");
    SOut->PutStrLn("}");
}

double TVrml::InsertEvelationGrid(PSOut SOut, const TSFltVV& Rlf,
        TFltVV& NewRlf, const TStr& TerrainColor, const int& N,
        const double& Height, const double& Spacing) {

    // transform grid of TSFlt to grid of TFlt of hight
    // Height and N times less points than in Rlf
    //InfoNotify(TStr::Fmt("%d : %d", Rlf.GetXDim(), Rlf.GetYDim()));
    NewRlf.Gen(Rlf.GetXDim()/N, Rlf.GetYDim()/N);
    NewRlf.PutAll(0.0);

    for (int x = 0; x < Rlf.GetXDim(); x++) {
        for (int y = 0; y < Rlf.GetYDim(); y++) {
            NewRlf(x/N,y/N) += double(Rlf(x,y));
        }
    }

    // write it to stream
    SOut->PutStrLn("Transform {");
    SOut->PutStrLn("   translation 0 0 0");
    SOut->PutStrLn("   children [");
    SOut->PutStrLn("      Shape {");
    SOut->PutStrLn("         appearance Appearance {");
    SOut->PutStrLn("            material Material {");
    SOut->PutStrLn("               diffuseColor " + TerrainColor);
    SOut->PutStrLn("            }");
    SOut->PutStrLn("         }");
    SOut->PutStrLn("         geometry ElevationGrid {");
    SOut->PutStrLn("            xDimension " + TInt::GetStr(NewRlf.GetXDim()));
    SOut->PutStrLn("            zDimension " + TInt::GetStr(NewRlf.GetYDim()));
    SOut->PutStrLn(TStr::Fmt("            xSpacing %.2f", Spacing));
    SOut->PutStrLn(TStr::Fmt("            zSpacing %.2f", Spacing));
    SOut->PutStrLn("            height [");

    for (int x = 0; x < NewRlf.GetXDim(); x++) {
        for (int y = 0; y < NewRlf.GetYDim(); y++) {
            IAssertR(NewRlf(x,y) <= N*N, TStr::Fmt("%d:%d:%g", x, y, NewRlf(x,y)()));
            NewRlf(x,y) = Height * NewRlf(x,y)/(N*N);
            SOut->PutStr(TStr::Fmt("%.4f ", NewRlf(x,y)()));
        }
        SOut->PutLn();
    }

    SOut->PutStrLn("            ]");
    SOut->PutStrLn("         }");
    SOut->PutStrLn("      }");
    SOut->PutStrLn("   ]");
    SOut->PutStrLn("}");

    return NewRlf.GetXDim() * Spacing;
}

void TVrml::InsertBillboard(PSOut SOut, const TFltVV& Rlf,
        const TStr& Text, const TStr& Color, const TStr& Size,
        const double& x, const double& y, const double& Height,
        const double& Scale, bool DoText, bool DoPoint) {

    const double Spacing = Scale / Rlf.GetXDim();
    int PosX = TFlt::Round(Scale*x/Spacing);
    int PosY = TFlt::Round(Scale*y/Spacing);
    if (PosX < 0) PosX = 0; if (PosY < 0) PosY = 0;
    if (PosX >= Rlf.GetXDim()) PosX = Rlf.GetXDim() - 1;
    if (PosY >= Rlf.GetYDim()) PosY = Rlf.GetYDim() - 1;
    const double z = Rlf(PosX, PosY) + Height;

    if (DoText) {
        SOut->PutStrLn("Transform {");
        SOut->PutStrLn(TStr::Fmt("  translation %.3f %.3f %.3f", Scale*y, z, Scale*x));
        SOut->PutStrLn("  scale " + Size);
        SOut->PutStrLn("  children Billboard {");
      SOut->PutStrLn("  axisOfRotation 0 0 0");
      SOut->PutStrLn("  children [");
        SOut->PutStrLn("    Shape {");
        SOut->PutStrLn("      appearance Appearance { material Material {");
        SOut->PutStrLn("                 diffuseColor " + Color + " } }");
        SOut->PutStrLn("      geometry Text { fontStyle FontStyle {justify \"MIDDLE\"} string \"" + Text + "\" }");
        SOut->PutStrLn("     }]");
        SOut->PutStrLn("  }");
        SOut->PutStrLn("}");
    }

    if (DoPoint) {
        SOut->PutStrLn("Anchor {");
        SOut->PutStrLn("  children [");
        SOut->PutStrLn("    Transform {");
        SOut->PutStrLn(TStr::Fmt("      translation %.3f %.3f %.3f", Scale*y, z - 0.95*Height, Scale*x));
        SOut->PutStrLn("      rotation 0 0 1 0");
        SOut->PutStrLn("      children Shape {");
        SOut->PutStrLn("        appearance Appearance { material Material { diffuseColor " + Color + "  } }");
        SOut->PutStrLn("        geometry Sphere { radius 0.1 }");
        SOut->PutStrLn("      }                                     ");
        SOut->PutStrLn("    }");
        SOut->PutStrLn("  ]");
        SOut->PutStrLn("  description \"" + Text + "\"");
        SOut->PutStrLn("}");
    }
}
