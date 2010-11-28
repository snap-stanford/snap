#ifndef vizmap_H
#define vizmap_H

//////////////////////////////////////////////////////////////////////////
// Forward-Declarations
class TVizMapFrame;
ClassHdTP(TVizMapXmlDocBs, PVizMapXmlDocBs);

//////////////////////////////////////////////////////////////////////////
// Visualization-Map-Links
ClassTPV(TVizMapLink, PVizMapLink, TVizMapLinkV) //{
private:
  // core link data
    TBool DirP;
    TInt SrcPointN;
    TInt DstPointN;
    TFlt Wgt; // link weight
  // auxiliary data
    TStr LinkNmStr; // link name
    TInt DocId; // DocId from BowDocBs for this link

public:
    TVizMapLink(const int& _SrcPointN, const int& _DstPointN,
        const bool& _DirP): DirP(_DirP), SrcPointN(_SrcPointN),
        DstPointN(_DstPointN), Wgt(1.0), LinkNmStr(""), DocId(-1) { }
    static PVizMapLink New(const int& SrcPointN, const int& DstPointN,
        const bool& DirP) { return new TVizMapLink(SrcPointN, DstPointN, DirP); }

    TVizMapLink(TSIn& SIn): DirP(SIn), SrcPointN(SIn), DstPointN(SIn),
        Wgt(SIn), LinkNmStr(SIn), DocId(SIn) { }
    static PVizMapLink Load(TSIn& SIn) { return new TVizMapLink(SIn); }
    void Save(TSOut& SOut) const {
        DirP.Save(SOut); SrcPointN.Save(SOut); DstPointN.Save(SOut);
        Wgt.Save(SOut); LinkNmStr.Save(SOut); DocId.Save(SOut); }

  // core data
    bool IsDir() const { return DirP; }
    // tells if point is source/destination
    bool IsSrcPointN(const int& PointN) const { return (PointN==SrcPointN); }
    bool IsDstPointN(const int& PointN) const { return (PointN==DstPointN); }
    // returns id of the opposite point
    int GetPointN(const int& PointN) const;
    // returns source/destination point
    int GetSrcPointN() const { return SrcPointN; }
    int GetDstPointN() const { return DstPointN; }
    // link weight
    double GetWgt() const { return Wgt; }
    void PutWgt(const double& _Wgt) { Wgt = _Wgt; }

  // properties
    // link name
    bool IsLinkNm() const { return !LinkNmStr.Empty(); }
    const TStr& GetLinkNm() const { return LinkNmStr; }
    void PutLinkNm(const TStr& _LinkNmStr) { LinkNmStr = _LinkNmStr; }
    // DocId from BowDocBs
    bool IsDocId() const { return (DocId != -1); }
    int GetDocId() const { return DocId; }
    void PutDocId(const int& _DocId) { EAssert(_DocId != -1); DocId = _DocId; }
};

//////////////////////////////////////////////////////////////////////////
// Visualization-Map-Point
ClassTPV(TVizMapPoint, PVizMapPoint, TVizMapPointV) //{
private:
  // core point data
    TFltV CoordV; // point location
    TFlt Wgt; // point weight
  // auxiliary data
    TStr PointNmStr; // point name
    TInt DocId; // DocId from BowDocBs for this point
    TIntV LinkNV; // links to other points
    TIntV CatIdV; // point categories

private:
    void AddCat(const int& CatId) {
        if (!CatIdV.IsInBin(CatId)) { CatIdV.AddSorted(CatId); } }

public:
    TVizMapPoint(const TFltV& _CoordV): CoordV(_CoordV),
        Wgt(1.0), PointNmStr(""), DocId(-1), LinkNV(), CatIdV() { };
    static PVizMapPoint New(const TFltV& CoordV) {
        return new TVizMapPoint(CoordV); }

    TVizMapPoint(TSIn& SIn) {
        CoordV.Load(SIn); Wgt.Load(SIn); PointNmStr.Load(SIn);
        DocId.Load(SIn); LinkNV.Load(SIn); CatIdV.Load(SIn); }
    static PVizMapPoint Load(TSIn& SIn) { return new TVizMapPoint(SIn); }
    void Save(TSOut& SOut) const {
        CoordV.Save(SOut); Wgt.Save(SOut); PointNmStr.Save(SOut);
        DocId.Save(SOut); LinkNV.Save(SOut); CatIdV.Save(SOut); }

  // core data
    const TFltV& GetCoordV() const { return CoordV; }
    int GetPointDim() const { return CoordV.Len(); }
    double GetPointX() const { Assert(CoordV.Len() >= 1); return CoordV[0]; }
    double GetPointY() const { Assert(CoordV.Len() >= 2); return CoordV[1]; }
    // point weight
    double GetWgt() const { return Wgt; }
    void PutWgt(const double& _Wgt) { Wgt = _Wgt; }

  // properties
    // point name
    bool IsPointNm() const { return !PointNmStr.Empty(); }
    const TStr& GetPointNm() const { return PointNmStr; }
    void PutPointNm(const TStr& _PointNmStr) { PointNmStr = _PointNmStr; }
    // DocId from BowDocBs
    bool IsDocId() const { return (DocId != -1); }
    int GetDocId() const { return DocId; }
    void PutDocId(const int& _DocId) { EAssert(_DocId != -1); DocId = _DocId; }
    // links
    bool IsLinks() const { return !LinkNV.Empty(); }
    int GetLinks() const { return LinkNV.Len(); }
    int GetLinkN(const int& LinkNN) const { return LinkNV[LinkNN]; };
    void PutLinkN(const int& LinkN) { LinkNV.Add(LinkN); }
    // categories
    bool IsCats() const { return !CatIdV.Empty(); }
    int GetCats() const { return CatIdV.Len(); }
    int GetCatId(const int& CatIdN) const { return CatIdV[CatIdN]; }
    bool IsCatId(const int& CatId) const { return CatIdV.IsIn(CatId); }

    friend class TVizMapFrame;
};

//////////////////////////////////////////////////////////////////////////
// Visualization-Map-Landscape
ClassTPV(TVizMapLandscape, PVizMapLandscape, TVizMapLandscapeV) //{
private:
  // core data
    TSFltVV HeightMatrixVV; // height matrix
    TFltV LevelV;
  // auxiliary data
    TStr LandscapeNmStr; // landscape name
    TIntVV CatMatrixVV; // category matrix
    TIntFltPrH CatIdToCenterH; // centers of cats
  // computation time
    TFltVV CalcMatrixVV; // height matrix
    TFltVV CalcStampVV; // stamp matrix

public:
    TVizMapLandscape() { };
    static PVizMapLandscape New() { return new TVizMapLandscape; }
    TVizMapLandscape(const TVizMapLandscapeV& CatLandscapeV,
        const PVizMapLandscape& FullLandscape, const TIntH& CatIdH,
        const double& MxCover, const double& MnCover, const int& MnCats);
    static PVizMapLandscape New(const TVizMapLandscapeV& CatLandscapeV,
        const PVizMapLandscape& FullLandscape, const TIntH& CatIdH, const double& MxCover,
        const double& MnCover, const int& MnCats) { return new TVizMapLandscape(
            CatLandscapeV, FullLandscape, CatIdH, MxCover, MnCover, MnCats); }

    TVizMapLandscape(TSIn& SIn) { HeightMatrixVV.Load(SIn);  LevelV.Load(SIn);
        LandscapeNmStr.Load(SIn); CatMatrixVV.Load(SIn); CatIdToCenterH.Load(SIn); }
    static PVizMapLandscape Load(TSIn& SIn) { return new TVizMapLandscape(SIn); }
    void Save(TSOut& SOut) const { HeightMatrixVV.Save(SOut); LevelV.Save(SOut);
        LandscapeNmStr.Save(SOut); CatMatrixVV.Save(SOut); CatIdToCenterH.Save(SOut); }

  // core data
    int GetXDim() const { return HeightMatrixVV.GetXDim(); }
    int GetYDim() const { return HeightMatrixVV.GetYDim(); }
    double GetHeight(const int& XPos, const int& YPos) const {
        return HeightMatrixVV(XPos, YPos); }
    const TSFltVV& HeightVV() const { return HeightMatrixVV; }
    int GetLevels() const { return LevelV.Len(); }
    double GetLevel(const int& LevelN) const { return LevelV[LevelN]; }
    const TFltV& GetLevelV() const { return LevelV; }

  // properties
    // landscape name
    bool IsLandscapeNm() const { return !LandscapeNmStr.Empty(); }
    const TStr& GetLandscapeNm() const { return LandscapeNmStr; }
    void PutLandscapeNm(const TStr& _LandscapeNmStr) { LandscapeNmStr = _LandscapeNmStr; }
    // category coordinates
    bool IsCatVV() const { return !CatMatrixVV.Empty(); }
    int GetCat(const int& XPos, const int& YPos) const { return CatMatrixVV(XPos, YPos); }
    const TIntVV& CatVV() const { return CatMatrixVV; }
    void PutCatVV(const TIntVV& _CatMatrixVV);
    int GetCatFromCoord(const double& X, const double& Y);
    // categories
    bool IsCat(const int& CatId) const { return CatIdToCenterH.IsKey(CatId); }
    int GetCats() const { return CatIdToCenterH.Len(); }
    int GetCatFFirstKeyId() const { return CatIdToCenterH.FFirstKeyId(); }
    bool GetCatFNextKeyId(int& KeyId) const { return CatIdToCenterH.FNextKeyId(KeyId); }
    int GetCatId(const int& KeyId) const { return CatIdToCenterH.GetKey(KeyId); }
    double GetCatX(const int& CatId) const { return CatIdToCenterH.GetDat(CatId).Val1; }
    double GetCatY(const int& CatId) const { return CatIdToCenterH.GetDat(CatId).Val2; }

  // computation time
    void CalcStart(const int& XDim, const int& YDim, const double& Sigma);
    void CalcPutStamp(const double& PointX, const double& PointY, const double& PointWgt);
    void CalcFinish(const double& MxVal, const int& Levels);

    friend class TVizMapFrame;
};

//////////////////////////////////////////////////////////////////////////
// Visualization-Map-Keyword
ClassTPV(TVizMapKeyWd, PVizMapKeyWd, TVizMapKeyWdV) //{
private:
  // core keyword data
    TFltV CoordV; // keyword location
    TStr KeyWdStr; // keyword string

public:
    TVizMapKeyWd() { };
    TVizMapKeyWd(const TFltV& _CoordV, const TStr& _KeyWdStr):
        CoordV(_CoordV), KeyWdStr(_KeyWdStr) {}

    TVizMapKeyWd(TSIn& SIn) { CoordV.Load(SIn); KeyWdStr.Load(SIn); }
    static PVizMapKeyWd Load(TSIn& SIn) { return new TVizMapKeyWd(SIn); }
    void Save(TSOut& SOut) const { CoordV.Save(SOut); KeyWdStr.Save(SOut); }

    static PVizMapKeyWd New(const TFltV& CoordV, const TStr& KeyWdStr) {
        return new TVizMapKeyWd(CoordV, KeyWdStr); }

  // core data
    const TFltV& GetCoordV() const { return CoordV; }
    double GetKeyWdX() const { Assert(CoordV.Len() >= 1); return CoordV[0]; }
    double GetKeyWdY() const { Assert(CoordV.Len() >= 2); return CoordV[1]; }
    // keyword string
    TStr GetKeyWdStr() const { return KeyWdStr; }
};

//////////////////////////////////////////////////////////////////////////
// Visualization-Map
ClassTPV(TVizMapFrame, PVizMapFrame, TVizMapFrameV) //{
private:
  // core map data
    TVizMapPointV PointV; // list of all points in the map
    TVizMapLinkV LinkV; // list of all links between the points on the map

  // properties
    TStr FrameNmStr; // frame name
    TInt FrameSortN; // number after wich we sort the frames

  // landscapes
    TVizMapLandscapeV LandscapeV; // precalulated landscapes
    PVizMapLandscape QueryLandscape; // query generated landscape

  // bag-of-words
    // documents linked to points for content and queries
    PBowDocBs PointBowDocBs;
    // documents linked to points for calculating background keywords
    PBowDocBs KeyWdBowDocBs;
    TBowSpVV KeyWdSpVV;
    // documents linked to links for content and link keywords
    PBowDocBs LinkBowDocBs;
    TBowSpVV LinkSpVV;

  // keywords
    TVizMapPointV KeyWdPointV;
    TVizMapKeyWdV KeyWdV; // list of precalculated keywords

  // categoreis
    TStrH CatNmH;

  // auxilary data
    TIntH DId2PointNMap; // DId -> PointN map

private:
    bool IsPointN(const int& PointN) const { return (0 <= PointN) && (PointN < PointV.Len());}

public:
    TVizMapFrame();
    static PVizMapFrame New() { return new TVizMapFrame(); }

    TVizMapFrame(TSIn& SIn);
    static PVizMapFrame Load(TSIn& SIn) { return new TVizMapFrame(SIn); }
    TVizMapFrame(TSIn& SIn, PBowDocBs _PointBowDocBs,
        PBowDocBs _KeyWdBowDocBs, PBowDocBs _LinkBowDocBs);
    static PVizMapFrame Load(TSIn& SIn, PBowDocBs _PointBowDocBs,
        PBowDocBs _KeyWdBowDocBs, PBowDocBs _LinkBowDocBs) {
            return new TVizMapFrame(SIn, _PointBowDocBs,
                _KeyWdBowDocBs, _LinkBowDocBs); }
    void Save(TSOut& SOut, const bool& SaveBows = true) const;

    bool operator<(const TVizMapFrame& VizMapFrame) const {
        return FrameSortN < VizMapFrame.FrameSortN; }

  // core map data
    // points
    int GetPoints() const { return PointV.Len(); }
    PVizMapPoint GetPoint(const int& PointN) const { return PointV[PointN]; }
    int AddPoint(PVizMapPoint VizMapPoint) { return PointV.Add(VizMapPoint); }
    int GetPointN(const TFltV& CoordV, const double& Radius) const;
    int GetRectPointV(const TFltRect& Rect, TIntV& RectPointV) const;
    //links
    int GetLinks() const { return LinkV.Len(); }
    PVizMapLink GetLink(const int LinkN) const { return LinkV[LinkN]; }
    int AddLink(PVizMapLink VizMapLink);

  // properties
    // frame name
    bool IsFrameNm() const { return !FrameNmStr.Empty(); }
    const TStr& GetFrameNm() const { return FrameNmStr; }
    void PutFrameNm(const TStr& _FrameNmStr) { FrameNmStr = _FrameNmStr; }
    // frame sort number
    bool IsFrameSortN() const { return FrameSortN != -1; }
    int GetFrameSortN() const { return FrameSortN; }
    void PutFrameSortN(const int& _FrameSortN) {
        IAssert(FrameSortN != -1); FrameSortN = _FrameSortN; }

  // landscapes
    int GetLandscapes() const { return LandscapeV.Len(); }
    PVizMapLandscape GetLandscape(const int& LandscapeN) const {
        return LandscapeV[LandscapeN]; }
    void AddLandscape(const int& XDim, const int& YDim,
        const double& Sigma, const bool& PointWgtP);
    void AddLandscapeFromLndMrk(const int& XDim, const int& YDim,
        const double& Sigma, const TVec<TFltV>& LndMrkPointV);
    void AddLandscapeWithCats(const int& XDim, const int& YDim,
        const double& Sigma, const bool& PointWgtP);

  // bag-of-words retlated stuff
    void PutPointBow(PBowDocBs _PointBowDocBs) {
        PointBowDocBs = _PointBowDocBs;}
    PBowDocBs GetPointBow() { return PointBowDocBs; }

    void PutKeyWdBow(PBowDocBs _KeyWdBowDocBs, const TBowSpVV& _KeyWdSpVV) {
        KeyWdBowDocBs = _KeyWdBowDocBs; KeyWdSpVV = _KeyWdSpVV; }
    PBowDocBs GetKeyWdBow() { return KeyWdBowDocBs; }
    const TBowSpVV& GetKeyWdSpVV() const { return KeyWdSpVV; }

    void PutLinkBow(PBowDocBs _LinkBowDocBs, const TBowSpVV& _LinkSpVV) {
        LinkBowDocBs = _LinkBowDocBs; LinkSpVV = _LinkSpVV; }
    PBowDocBs GetLinkBow() { return LinkBowDocBs; }
    const TBowSpVV& GetLinkSpVV() const { return LinkSpVV; }

    int GetDocId(const int& PointN) const;
    TStr GetDocNm(const int& PointN) const;
    TStr GetDocBody(const int& PointN) const;
    TStr GetDocUrl(const int& PointN) const;

  // keywords
    // adds a background point for serving the keywords
    int AddKeyWdPoint(PVizMapPoint VizMapPoint) { return KeyWdPointV.Add(VizMapPoint); }
    // calculates top MxKeyWd keywords for the given part of the map
    int GenKeyWd(const TFltV& CoordV, const double& Radius,
        const int& MxKeyWd, TStrV& KeyWdStrV, const bool& DistWgtP = false,
        const bool& PointWgtP = false, const bool& BackKeyWdP = false) const;
    // calculates and stores KeyWds keywords for the map
    void GenKeyWds(const int& KeyWds, const double& Radius,
        const int& Candidantes, const int& RndSeed, const TFltRect& Rect);
    int GetKeyWds() const { return KeyWdV.Len(); }
    PVizMapKeyWd GetKeyWd(const int& KeyWdN) const { return KeyWdV[KeyWdN]; }

  // categories
    int AddCatNm(const TStr& CatNm) { return CatNmH.AddKey(CatNm); }
    bool IsCatId(const TStr& CatNm) const { return CatNmH.IsKey(CatNm); }
    int GetCatId(const TStr& CatNm) const { return CatNmH.GetKeyId(CatNm); }
    int GetCats() const { return CatNmH.Len(); }
    TStr GetCatNm(const int& CatId) const { return CatNmH.GetKey(CatId); }
    void AddCat(const int& PointN, const int& CatId);

  // auxilary data
    void CalcDId2PointNMap();
    bool IsDId2PointNMap() const { return !DId2PointNMap.Empty(); }
    bool IsInDId(const int& DId) const { return DId2PointNMap.IsKey(DId); }
    int GetPointN(const int& DId) const { return DId2PointNMap.GetDat(DId); }

    // files
    void SaveVrml(const TStr& VrmlFNm, PVizMapLandscape Landscape,
      const bool& ShowDocNmP = true, const bool& ShowDocPtP = true,
      const bool& ShowKeyWdP = true, const double& FontSize = 2.0,
      const TStr& SkyColor = "0.4 0.4 1.0", const TStr& TerrainColor = "0.1 0.5 0.1",
      const TStr& KeywordColor = "0.9 0.9 0.2", const TStr& DocNmColor = "1.0 0.4 0");
    void SaveLegend(const TStr& TxtFNm, const int& LegendGridWidth,
        const int& LegendGridHeight);
};

//////////////////////////////////////////////////////////////////////////
// Visualization-Time-Map
ClassTP(TVizMap, PVizMap)//{
private:
    TVizMapFrameV VizMapFrameV;

public:
    TVizMap() { };
    static PVizMap New() { return new TVizMap; }
    TVizMap(const PVizMapFrame& VizMapFrame) { VizMapFrameV.Add(VizMapFrame); };
    static PVizMap New(const PVizMapFrame& VizMapFrame) {
        return new TVizMap(VizMapFrame); }

    TVizMap(TSIn& SIn);
    static PVizMap Load(TSIn& SIn) { return new TVizMap(SIn); }
    void Save(TSOut& SOut) const;

    // frames
    void AddVizMapFrame(const PVizMapFrame& VizMapFrame, const bool& SortedP);
    int GetVizMapFrames() const { return VizMapFrameV.Len(); }
    PVizMapFrame GetVizMapFrame(const int& VizMapFrameN) const {
        return VizMapFrameV[VizMapFrameN]; }
    PVizMapFrame GetFirst() const { return VizMapFrameV[0]; }
    bool Empty() const { return VizMapFrameV.Empty(); }

    // visualization parameters


    // files
    static PVizMap LoadBin(const TStr& FNm) {
        TFIn FIn(FNm); return TVizMap::Load(FIn); }
    void SaveBin(const TStr& FNm) { TFOut FOut(FNm); Save(FOut); }

    // saving doc atlas XML format
    static void SaveXmlStart(TSOut& SOut) { SOut.PutStrLn("<docatlas>"); }
    static void SaveXmlEnd(TSOut& SOut) { SOut.PutStrLn("</docatlas>"); }
    static void SaveXmlDoc(TSOut& SOut, const TStr& Nm, const TStr& Body,
        const TStr& DisplayBody = TStr(), const TStrV& CatNmV = TStrV(),
        const TStrV& AuthorV = TStrV(), const TStr& FrameNm = TStr(),
        const int& FrameSortN = -1);
};

//////////////////////////////////////////////////////////////////////////
// Visualization-Landmark-Map
ClassTP(TVizMapLndMrk, PVizMapLndMrk)//{
private:
    PBowDocBs BowDocBs;
    PBowDocWgtBs BowDocWgtBs;
    TBowSpVV LndMrkSpVV;
    TVec<TFltV> LndMrkPointVV;

public:
    TVizMapLndMrk(PBowDocBs _BowDocBs, PBowDocWgtBs _BowDocWgtBs,
        const TBowSpVV& _LndMrkSpVV, const TVec<TFltV>& _LndMrkPointVV):
            BowDocBs(_BowDocBs), BowDocWgtBs(_BowDocWgtBs),
            LndMrkSpVV(_LndMrkSpVV), LndMrkPointVV(_LndMrkPointVV) { }
    static PVizMapLndMrk New(PBowDocBs BowDocBs, PBowDocWgtBs BowDocWgtBs,
        const TBowSpVV& LndMrkSpVV, const TVec<TFltV>& LndMrkPointVV) {
            return new TVizMapLndMrk(BowDocBs, BowDocWgtBs, LndMrkSpVV, LndMrkPointVV); }

    TVizMapLndMrk(TSIn& SIn) {
        BowDocBs = TBowDocBs::Load(SIn); BowDocWgtBs = TBowDocWgtBs::Load(SIn);
        LndMrkSpVV.Load(SIn); LndMrkPointVV.Load(SIn); }
    static PVizMapLndMrk Load(TSIn& SIn) { return new TVizMapLndMrk(SIn); }
    void Save(TSOut& SOut) const {
        BowDocBs->Save(SOut); BowDocWgtBs->Save(SOut);
        LndMrkSpVV.Save(SOut); LndMrkPointVV.Save(SOut); }

    PBowDocBs GetBowDocBs() const { return BowDocBs; }
    PBowDocWgtBs GetBowDocWgtBs() const { return BowDocWgtBs; }
    const TBowSpVV& GetLndMrkSpVV() const { return LndMrkSpVV; }
    const TVec<TFltV>& GetLndMrkPointVV() const { return LndMrkPointVV; }
    // access to documents
    int GetLndMrks() const { return LndMrkSpVV.Len(); }
    int GetLndMrkDId(const int& LndMrkN) const { return BowDocWgtBs->GetDId(LndMrkN); }
    const TFltV& GetLndMrkPointV(const int& LndMrkN) const { return LndMrkPointVV[LndMrkN]; }
    PBowSpV GetLndMrkSpV(const int& LndMrkN) const { return LndMrkSpVV[LndMrkN]; }

    // files
    static PVizMapLndMrk LoadBin(const TStr& FNm) {
        PSIn SIn = TFIn::New(FNm); return TVizMapLndMrk::Load(*SIn); }
    void SaveBin(const TStr& FNm) { PSOut SOut = TFOut::New(FNm); Save(*SOut); }
};

//////////////////////////////////////////////////////////////////////////
// Visualization-Map Xml-Document
ClassTV(TVizMapXmlDoc, TVizMapXmlDocV)//{
public:
    TStr Nm;
    TStr Body;
    TStr DisplayBody;
    TIntV CatIdV;
    TIntV AuthorIdV;
    TStr FrameNm;
    TInt FrameSortN;
public:
    TVizMapXmlDoc(): FrameNm(), FrameSortN(-1) { }
    TVizMapXmlDoc(const TStr& _Nm, const TStr& _Body, const TStr& _DisplayBody,
        const TIntV& _CatIdV, const TIntV& _AuthorIdV, const TStr& _FrameNm,
        const int& _FrameSortN): Nm(_Nm), Body(_Body), DisplayBody(_DisplayBody),
            CatIdV(_CatIdV), AuthorIdV(_AuthorIdV), FrameNm(_FrameNm),
            FrameSortN(_FrameSortN) { }

    int AddToBowDocBs(PVizMapXmlDocBs VizMapXmlDocBs, PBowDocBs BowDocBs) const;
};

//////////////////////////////////////////////////////////////////////////
// Visualization-Map Xml-Document-Base
typedef enum { vmxdgNone, vmxdgDay, vmxdgMonth, vmxdgYear } TVizMapXmlDocGrouping;

ClassTP(TVizMapXmlDocBs, PVizMapXmlDocBs)//{
public:
    TVizMapXmlDocV XmlDocV;
    TStrH CatNmH;
    TStrH AuthorNmH;
    TStrH FrameNmToSortN;
    TIntIntVH FrameSortNToDocId;

public:
    // load from xml file
    TVizMapXmlDocBs(const TStr& XmlFNm,
        const PNotify& Notify = TStdNotify::New());
    static PVizMapXmlDocBs New(const TStr& XmlFNm,
        const PNotify& Notify = TStdNotify::New()) {
            return new TVizMapXmlDocBs(XmlFNm, Notify); }

    // create from SearchBs RSet
    TVizMapXmlDocBs(const PWdGixRSet& RSet,
        const TVizMapXmlDocGrouping& Grouping,
        const PNotify& Notify = TStdNotify::New());
    static PVizMapXmlDocBs New(const PWdGixRSet& RSet,
        const TVizMapXmlDocGrouping& Grouping,
        const PNotify& Notify = TStdNotify::New()) {
            return new TVizMapXmlDocBs(RSet, Grouping, Notify); }

    // generates bow with documents as documents
    static PBowDocBs LoadBowDocBs(const TStr& XmlFNm, PSwSet SwSet,
        PStemmer Stemmer, const int& MxNGramLen = 3, const int& MnNGramFq = 5);
};

//////////////////////////////////////////////////////////////////////////
// Visualization-Map Factory
// -- learning math
typedef enum {vxmtStaticDoc, vxmtDynamicDoc,
    vxmtStaticAuthor, vxmtDynamicAuthor } TVizXmlMapType;
typedef enum {vdtEucl, vdtCos, vdtSqrtCos} TVizDistType;

class TVizMapFactory {
private:
    static void CG(const TMatrix& Matrix, const TFltV& b, TFltV& x,
        PNotify Notify, const int& MaxStep = 200, const double& EpsTer = 1e-7);

private:
    // the ultimate MDS...
    static void MakeFlat(PSVMTrainSet Set, const TVizDistType& DistType,
        TVec<TFltV>& DocPointV, const int& MxStep, const int& MxSecs,
        const double& MnDiff, const bool& RndStartPos, PNotify Notify);
    static void NormalizePoints(TVec<TFltV>& PointV);

    // BowSpV->LSI->MDS->DocPointV
    // applys LSI and MDS to documents in DocSpV
    static void LsiMds(TVec<PBowSpV> DocSpV, PSemSpace SemSpace, TVec<TFltV>& DocPointV,
        const double& SemSpaceThresh, const int& MxStep, const int& MxSecs,
        const double& MnDiff, PNotify Notify);

    // Bow->LSI->MDS->Viz
    // for smaller datasets
    static PVizMapFrame DocLsiMds(PBowDocWgtBs BowDocWgtBs, PSemSpace SemSpace,
        const TVec<TFltV>& _DocPointV, const double& SemSpaceThresh,
        const int& MxStep, const int& MxSecs, const double& MnDiff, PNotify Notify);

    // adds metadata from BowDocBs and info for computing keywords to VizMap
    static void AddDocMetadata(PVizMapFrame VizMapFrame, PBowDocBs PointBowDocBs,
        PBowDocBs KeyWdBowDocBs, PBowDocWgtBs KeyWdBowDocWgtBs,
        PVizMapLndMrk VizMapLndMrk = NULL);

    // Landmarks+Bow->DocPointV
    // interpolates positions of documents in BowDocWgtBs based
    // on positions ClustPointV of landmarks ClustSpV
    static void LndMrk(PBowDocWgtBs BowDocWgtBs, PBowSim BowSim,
        const TVec<PBowSpV>& ClustSpV, const TVec<TFltV>& ClustPointV,
        TVec<TFltV>& DocPointV, const int& LinCombNum, PNotify Notify);

    // Bow->Landmarks->Viz
    static PVizMapFrame DocLndMrk(PBowDocWgtBs BowDocWgtBs,
        const TVec<PBowSpV>& ClustSpV, const TVec<TFltV>& ClustPointV,
        const int& LinCombNum = 3, PNotify Notify = TNullNotify::New());
    // Bow->KMeans->LSI->MDS->Viz -- for larger datasets
    static PVizMapFrame ClustLsiMdsDocLndMrk(PBowDocWgtBs BowDocWgtBs,
        PBowDocPart BowDocPart, PSemSpace SemSpace, const int& LinCombNum,
        const double& SemSpaceThresh, const int& MxStep, const int& MxSecs,
        const double& MnDiff, PNotify Notify);

    // Bow->VizFrame
    static PVizMapFrame NewVizMapFrame(PBowDocBs BowDocBs, PBowDocWgtBs BowDocWgtBs,
        PBowDocBs KeyWdBowDocBs, PBowDocWgtBs KeyWdBowDocWgtBs, const int& ThDocs,
        const int& Clusts, const double& SvdThreshold, const PNotify& Notify,
        const bool& LndPointWgt, const bool& CalcLandscapeP);
    static PVizMapFrame NewVizMapFrame(PBowDocBs BowDocBs, const int& ThDocs,
        const int& Clusts, const double& SvdThreshold, const PNotify& Notify,
        const bool& LndPointWgt, const bool& CalcLandscapeP);

    // Visualization using Landmark-Maps
    static PVizMapFrame NewVizMapFrameFromLndMrk(PBowDocBs BowDocBs,
        PBowDocWgtBs BowDocWgtBs, PVizMapLndMrk VizMapLndMrk, const bool& LndMrkLndP,
        const bool& LndMrkKeyWdP, const PNotify& Notify);

public:
    // Visualization-for-Dummys
    static PVizMap NewVizMap(PBowDocBs BowDocBs, PBowDocWgtBs BowDocWgtBs,
        PBowDocBs KeyWdBowDocBs, PBowDocWgtBs KeyWdBowDocWgtBs, const int& ThDocs = 300,
        const int& Clusts = 200, const double& SvdThreshold = 0.8,
        const PNotify& Notify = TStdNotify::New(), const bool& LndPointWgtP = false,
        const bool& CalcLandscapeP = true);
    static PVizMap NewVizMap(PBowDocBs BowDocBs, const int& ThDocs = 300,
        const int& Clusts = 200, const double& SvdThreshold = 0.8,
        const PNotify& Notify = TStdNotify::New(), const bool& LndPointWgtP = false,
        const bool& CalcLandscapeP = true);

    // Visualization-of-DAX-strucutres (Document-Atlas-Xml format)
    static PVizMap NewVizMapStaticDoc(
        const PVizMapXmlDocBs& XmlDocBs, PSwSet SwSet = TSwSet::New(swstEn523),
        PStemmer Stemmer = TStemmer::New(stmtPorter, true),
        const int& ThDocs = 300, const int& Clusts = 200, const double& SvdThreshold = 0.8,
        const PNotify& Notify = TStdNotify::New(), const bool& LndPointWgt = false,
        const bool& CalcLandscapeP = true);

    static PVizMap NewVizMapDynamicDoc(const PVizMapXmlDocBs& XmlDocBs, PSwSet SwSet = TSwSet::New(swstEn523),
        PStemmer Stemmer = TStemmer::New(stmtPorter, true),
        const int& ThDocs = 300, const int& Clusts = 200, const double& SvdThreshold = 0.8,
        const PNotify& Notify = TStdNotify::New(), const bool& LndPointWgt = false,
        const bool& CalcLandscapeP = true);

    static PVizMap NewVizMapStaticAuthor(const PVizMapXmlDocBs& XmlDocBs, PSwSet SwSet = TSwSet::New(swstEn523),
        PStemmer Stemmer = TStemmer::New(stmtPorter, true), const int& MxAuthors = 100,
        const int& ThDocs = 300, const int& Clusts = 200, const double& SvdThreshold = 0.8,
        const PNotify& Notify = TStdNotify::New(), const bool& LndPointWgt = false,
        const bool& CalcLandscapeP = true);

    static PVizMap NewVizMapDynamicAuthor(const PVizMapXmlDocBs& XmlDocBs, PSwSet SwSet = TSwSet::New(swstEn523),
        PStemmer Stemmer = TStemmer::New(stmtPorter, true), const int& MxFrames = 5,
        const int& MxAuthors = 10, const int& ThDocs = 300, const int& Clusts = 200,
        const double& SvdThreshold = 0.8, const PNotify& Notify = TStdNotify::New(),
        const bool& LndPointWgt = false, const bool& CalcLandscapeP = true);

    static PVizMap NewVizMap(const PVizMapXmlDocBs& XmlDocBs,
        const TVizXmlMapType& VizXmlMapType, PSwSet SwSet = TSwSet::New(swstEn523),
        PStemmer Stemmer = TStemmer::New(stmtPorter, true), const int& ThDocs = 300,
        const int& Clusts = 200, const double& SvdThreshold = 0.8,
        const PNotify& Notify = TStdNotify::New(), const bool& LndPointWgt = false,
        const bool& CalcLandscapeP = true);

    static PVizMap NewVizMap(const TStr& XmlFNm,
        const TVizXmlMapType& VizXmlMapType, PSwSet SwSet = TSwSet::New(swstEn523),
        PStemmer Stemmer = TStemmer::New(stmtPorter, true), const int& ThDocs = 300,
        const int& Clusts = 200, const double& SvdThreshold = 0.8,
        const PNotify& Notify = TStdNotify::New(), const bool& LndPointWgt = false,
        const bool& CalcLandscapeP = true);

    // Generation of LandMark-Maps
    static PVizMapLndMrk NewVizMapLndMrk(PBowDocBs BowDocBs,
        PBowDocWgtBs BowDocWgtBs, const int& ThDocs = 600, const int& Clusts = 400,
        const double& SvdThreshold = 0.8, const PNotify& Notify = TStdNotify::New());
    static PVizMapLndMrk NewVizMapLndMrk(PBowDocBs BowDocBs,
        const TIntV& DIdV = TIntV(), const int& ThDocs = 600, const int& Clusts = 400,
        const double& SvdThreshold = 0.8, const PNotify& Notify = TStdNotify::New());

    // Visualization using Landmark-Maps
    static PVizMap NewVizMapFromLndMrk(PBowDocBs BowDocBs, PBowDocWgtBs BowDocWgtBs,
        PVizMapLndMrk VizMapLndMrk, const bool& LndMrkLndP = false,
        const bool& LndMrkKeyWdP = false, const PNotify& Notify = TStdNotify::New());
    static PVizMap NewVizMapFromLndMrk(PBowDocBs BowDocBs, const TIntV& DIdV,
        PVizMapLndMrk VizMapLndMrk, const bool& LndMrkLndP = false,
        const bool& LndMrkKeyWdP = false,
        const TBowWordWgtType& WgtType = bwwtLogDFNrmTFIDF,
        const PNotify& Notify = TStdNotify::New());
};

//////////////////////////////////////////////////////////////////////////
// Vrml-Writer
class TVrml {
public:
    static void InsertHeader(PSOut SOut, const TStr& SkyColor);

    static double InsertEvelationGrid(PSOut SOut, const TSFltVV& Rlf,
        TFltVV& NewRlf, const TStr& TerrainColor, const int& N = 5,
        const double& Height = 20.0, const double& Spacing = 0.7);

    static void InsertBillboard(PSOut SOut, const TFltVV& Rlf,
        const TStr& Text, const TStr& Color, const TStr& Size,
        const double& x, const double& y, const double& Height,
        const double& Scale, bool DoText = true, bool DoPoint = false);
};

#endif
