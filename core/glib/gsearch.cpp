/////////////////////////////////////////////////
// WdGix-Constants
TStr TGixConst::WdGixFNm = "WdGix";
TStr TGixConst::WdGixDatFNm = "WdGix.Dat";
TStr TGixConst::WdGixBsFNm = "WdGixBs.MBlobBs";
TStr TGixConst::WdGixMDSFNm = "WdGixMDS.Dat";
TStr TGixConst::TrGixFNm = "TrGix";
TStr TGixConst::TrGixDatFNm = "TrGix.Dat";
TStr TGixConst::TrGixDocBsFNm = "TrGixDocBs.MBlobBs";
TStr TGixConst::TrGixSentBsFNm = "TrGixSentBs.MBlobBs";
TStr TGixConst::TrGixTrAttrBsFNm = "TrGixTrAttrBs.MBlobBs";
TStr TGixConst::MWdGixFNm = "MWdGix";
TStr TGixConst::MWdGixDatFNm = "MWdGix.Dat";
TStr TGixConst::MWdGixDocBsFNm = "MWdGixDocBs.MBlobBs";
TStr TGixConst::MWdGixBsFNm = "MWdGixBs.Dat";

/////////////////////////////////////////////////
// Word-Inverted-Index-DataField
int TWdGixItem::TitleBit = 0;
int TWdGixItem::NmObjBit = 1;
int TWdGixItem::AnchorBit = 2;
int TWdGixItem::EmphBit = 3;

TWdGixItem::TWdGixItem(const TBlobPt& BlobPt, const uchar& _Wgt, const uchar& _WdPos,
        const bool& TitleP, const bool& NmObjP, const bool& AnchorP, const bool& EmphP):
            Seg(BlobPt.GetSeg()), Addr(BlobPt.GetAddr()), WdPos(_WdPos) {

    FSet.SetBit(TitleBit, TitleP);
    FSet.SetBit(NmObjBit, NmObjP);
    FSet.SetBit(AnchorBit, AnchorP);
    FSet.SetBit(EmphBit, EmphP);
}

TWdGixItem::TWdGixItem(const uchar& _Seg, const uint& _Addr, const uchar& _Wgt,
        const uchar& _WdPos, const bool& TitleP, const bool& NmObjP, const bool& AnchorP,
        const bool& EmphP): Seg(_Seg), Addr(_Addr), Wgt(_Wgt), WdPos(_WdPos) {

    FSet.SetBit(TitleBit, TitleP);
    FSet.SetBit(NmObjBit, NmObjP);
    FSet.SetBit(AnchorBit, AnchorP);
    FSet.SetBit(EmphBit, EmphP);
}

TWdGixItem::TWdGixItem(TSIn& SIn) {
    SIn.Load(Seg); SIn.Load(Addr);
    SIn.Load(Wgt); SIn.Load(WdPos); FSet=TB8Set(SIn);
}

void TWdGixItem::Save(TSOut& SOut) const {
    SOut.Save(Seg); SOut.Save(Addr);
    SOut.Save(Wgt); SOut.Save(WdPos); FSet.Save(SOut);
}

inline bool TWdGixItem::operator==(const TWdGixItem& Item) const {
    return (Seg == Item.Seg) && (Addr == Item.Addr); // && (WdPos == Item.WdPos);
}

inline bool TWdGixItem::operator<(const TWdGixItem& Item) const {
    return (Seg < Item.Seg) ||
        ((Seg == Item.Seg) && (Addr < Item.Addr)); // ||
        //((Seg == Item.Seg) && (Addr == Item.Addr) && (WdPos < Item.WdPos));
}

/////////////////////////////////////////////////
// Word-Inverted-Index
void TWdGix::LoadTags() {
    TitleTagH.AddKey("<TITLE>");
    NmObjTagH.AddKey("<NMOBJ>");
    EmphTagH.AddKey("<EM>"); EmphTagH.AddKey("<A>"); EmphTagH.AddKey("<B>");
    EmphTagH.AddKey("<I>"); EmphTagH.AddKey("<H1>"); EmphTagH.AddKey("<H2>");
    EmphTagH.AddKey("<H3>"); EmphTagH.AddKey("<H4>"); EmphTagH.AddKey("<H5>");
}

TWdGix::TWdGix(const TStr& _FPath, const TFAccess& _FAccess, const int64& CacheSize) {
    // store policy
    FPath = _FPath; FAccess = _FAccess;
    // prepare gix
    WGix = TWGix::New(TGixConst::WdGixFNm, FPath, FAccess, CacheSize);
    // load word tables
    if (FAccess == faCreate) {
        Stemmer = TStemmer::New(stmtPorter, false);
        SwSet = TSwSet::New(swstEn523);
    } else {
        // prepare file name
        TStr WdGixDatFNm = TStr::GetNrFPath(FPath) + TGixConst::WdGixDatFNm;
        // load stuff
        TFIn FIn(WdGixDatFNm);
        WordH.Load(FIn);
        Stemmer = TStemmer::Load(FIn);
        SwSet = TSwSet::Load(FIn);
    }
    // load tags
    LoadTags();
}

TWdGix::~TWdGix() {
    if ((FAccess == faCreate) || (FAccess == faUpdate)) {
        // prepare file name
        TStr WdGixDatFNm = TStr::GetNrFPath(FPath) + TGixConst::WdGixDatFNm;
        // save word tables
        TFOut FOut(WdGixDatFNm);
        WordH.Save(FOut);
        Stemmer->Save(FOut);
        SwSet->Save(FOut);
    }
}

void TWdGix::AddHtml(const TStr& HtmlStr, const TBlobPt& BlobPt, const uchar& Wgt) {
    // create html lexical
    PSIn HtmlSIn = TStrIn::New(HtmlStr);
    THtmlLx HtmlLx(HtmlSIn); HtmlLx.GetSym();
    // prepare word vector, position counter and flags
    THash<TInt, TWdGixItemV> WIdToItemVH; uchar WdPos = 0;
    bool TitleP = false, NmObjP = false; int EmphLv = 0;
    // traverse html string symbols
    while (HtmlLx.Sym != hsyEof) {
        if (HtmlLx.Sym == hsyStr) {
            // get word token
            TStr WordStr = HtmlLx.UcChA;
            // should we keep this word?
            if (!SwSet->IsIn(WordStr)) {
                // stem the word
                WordStr=Stemmer->GetStem(WordStr);
                // get the word id
                const int WId = WordH.AddKey(WordStr);
                // increase the position count
                WdPos++;
                // add word to vector
                WIdToItemVH.AddDat(WId).Add(
                    TWdGixItem(BlobPt, Wgt, WdPos, TitleP, NmObjP, false, (EmphLv>0)));
            }
        } else if (HtmlLx.Sym == hsyBTag) {
            // we have a tag, we update flags accordingly
            TStr TagStr = HtmlLx.UcChA;
            if (TitleTagH.IsKey(TagStr)) { TitleP = true; }
            if (NmObjTagH.IsKey(TagStr)) { NmObjP = true; }
            if (EmphTagH.IsKey(TagStr)) { EmphLv++; }
        } else if (HtmlLx.Sym == hsyETag) {
            // we have a tag, we update flags accordingly
            TStr TagStr = HtmlLx.UcChA;
            if (TitleTagH.IsKey(TagStr)) { TitleP = false; }
            if (NmObjTagH.IsKey(TagStr)) { NmObjP = false; }
            if (EmphTagH.IsKey(TagStr)) { EmphLv--; EmphLv = TInt::GetMx(0, EmphLv); }
        }
        // get next symbol
        HtmlLx.GetSym();
    }
    // load add documents to words in inverted index
    int WdKeyId = WIdToItemVH.FFirstKeyId();
    while (WIdToItemVH.FNextKeyId(WdKeyId)) {
        const int WId = WIdToItemVH.GetKey(WdKeyId); WordH[WId]++;
        const TWdGixItemV& ItemV = WIdToItemVH[WdKeyId];
        // HACK combine all items into one
        const uchar Seg = ItemV[0].GetSeg();
        const uint Addr = ItemV[0].GetAddr();
        const uchar Count = uchar(TInt::GetMn(int(TUCh::Mx), ItemV.Len()));
        bool TitleP = false, NmObjP = false, EmphP = false, AnchorP = false;
        for (int ItemN = 0; ItemN < ItemV.Len(); ItemN++) {
            const TWdGixItem& Item = ItemV[ItemN];
            TitleP = TitleP || Item.IsTitle();
            NmObjP = NmObjP || Item.IsNmObj();
            EmphP = EmphP || Item.IsAnchor();
            AnchorP = AnchorP || Item.IsEmph();
        }
        TWdGixItem Item(Seg, Addr, Wgt, Count, TitleP, NmObjP, AnchorP, EmphP);
        // add ti index
        WGix->AddItem(WId, Item);
    }
}

bool TWdGix::Search(const TStr& QueryStr, TWdGixItemV& ResItemV) {
    //HACK simple parsing (no operators...)
    PWGixExpItem WGixExp = TWGixExpItem::NewEmpty();
    PSIn HtmlSIn = TStrIn::New(QueryStr);
    THtmlLx HtmlLx(HtmlSIn); HtmlLx.GetSym();
    while (HtmlLx.Sym != hsyEof) {
        if (HtmlLx.Sym == hsyStr) {
            // get word token
            TStr WordStr = HtmlLx.UcChA;
            // stem the word
            WordStr=Stemmer->GetStem(WordStr);
            // check if we have it
            const int WId = WordH.GetKeyId(WordStr);
            if (WId != -1) {
                PWGixExpItem WGixExpItem = TWGixExpItem::NewItem(WId);
                if (WGixExp->IsEmpty()) { WGixExp = WGixExpItem; }
                else { WGixExp = TWGixExpItem::NewAnd(WGixExp, WGixExpItem); }
            }
        }
        // get next symbol
        HtmlLx.GetSym();
    }
    // search
    return WGixExp->Eval(WGix, ResItemV);
}

/////////////////////////////////////////////////
// WdGix-Meta-Data-Base
void TWdGixMDS::AddDate(const TBlobPt& DocBlobPt, const TTm& DateTime) {
    TAddrPr AddrPr(DocBlobPt.GetSeg(), DocBlobPt.GetAddr());
    const uint64 DateMSecs = TTm::GetMSecsFromTm(DateTime);
    AddrPrToDateH.AddDat(AddrPr, DateMSecs);
}

inline uint64 TWdGixMDS::GetDateMSecs(const TBlobPt& DocBlobPt) const {
    return AddrPrToDateH.GetDat(TAddrPr(DocBlobPt.GetSeg(), DocBlobPt.GetAddr()));
}

inline TTm TWdGixMDS::GetDateTTm(const TBlobPt& DocBlobPt) const {
    return TTm::GetTmFromMSecs(GetDateMSecs(DocBlobPt));
}

/////////////////////////////////////////////////
// WdGix-Result-Set
void TWdGixRSet::AddDoc(const TStr& DocTitle, const TStr& DocStr,
        const TStrV& CatNmV, const TTm& DateTime) {

    DocTitleV.Add(DocTitle);
    DocTitleV.Last().DelChAll('\n');
    DocTitleV.Last().DelChAll('\r');
    DocStrV.Add(DocStr);
    CatNmVV.Add(CatNmV);
    DateTimeV.Add(DateTime);
}

void TWdGixRSet::SortByDate(const bool& Asc) {
    // sort hits by date
    typedef TPair<TUInt64, TInt> TUInt64IntPr;
    TVec<TUInt64IntPr> TmMSecsDocNV;
    const int Docs = GetDocs();
    for (int DocN = 0; DocN < Docs; DocN++) {
        uint64 TmMSecs = TTm::GetMSecsFromTm(DateTimeV[DocN]);
        TmMSecsDocNV.Add(TUInt64IntPr(TmMSecs, DocN));
    }
    TmMSecsDocNV.Sort(Asc);
    // resort the original vectors
    TStrV NewDocTitleV(Docs, 0), NewDocStrV(Docs, 0);
    TVec<TStrV> NewCatNmVV(Docs, 0); TTmV NewDateTimeV(Docs, 0);
    for (int NewDocN = 0; NewDocN < Docs; NewDocN++) {
        const int OldDocN = TmMSecsDocNV[NewDocN].Val2;
        NewDocTitleV.Add(DocTitleV[OldDocN]);
        NewDocStrV.Add(DocStrV[OldDocN]);
        NewCatNmVV.Add(CatNmVV[OldDocN]);
        NewDateTimeV.Add(DateTimeV[OldDocN]);
    }
    DocTitleV = NewDocTitleV; DocStrV = NewDocStrV;
    CatNmVV = NewCatNmVV; DateTimeV = NewDateTimeV;
}

void TWdGixRSet::PrintRes(PNotify Notify) {
    const int Docs = GetDocs();
    Notify->OnStatus(TStr::Fmt(
        "All results: %d, Showing results from %d to %d",
        AllDocs.Val, Docs, Docs + Offset.Val));
    for (int DocN = 0; DocN < Docs; DocN++) {
        TTm DateTime = DateTimeV[DocN];
        if (DateTime.IsDef()) {
            Notify->OnStatus(TStr::Fmt("[%d: %s] %s ...", DocN+1,
                DateTime.GetWebLogDateStr().CStr(),
                DocTitleV[DocN].Left(50).CStr()));
        } else {
            Notify->OnStatus(TStr::Fmt("[%d] %s ...", DocN+1,
                DocTitleV[DocN].Left(60).CStr()));
        }
    }
    Notify->OnStatus(TStr::Fmt("All results: %d, Showing results from %d to %d",
        AllDocs.Val, Docs, Docs + Offset.Val));
}

PBowDocBs TWdGixRSet::GenBowDocBs() const {
    PSwSet SwSet = TSwSet::New(swstEn523);
    PStemmer Stemmer = TStemmer::New(stmtPorter, true);
    PBowDocBs BowDocBs = TBowDocBs::New(SwSet, Stemmer, NULL);
    const int Docs = GetDocs();
    for (int DocN = 0; DocN < Docs; DocN++) {
        const TStr& DocNm = DocTitleV[DocN];
        const TStr& DocStr = DocStrV[DocN];
        BowDocBs->AddHtmlDoc(DocNm, TStrV(), DocStr, true);
    }
    return BowDocBs;
}

/////////////////////////////////////////////////
// WdGix-Base
void TWdGixBs::Filter(const TWgtWdGixItemKdV& InItemV,
        const TWdGixBsGrouping& Grouping, TWgtWdGixItemKdV& OutItemV) {

    OutItemV.Clr();
    if (Grouping == wgbgName) {
        // group by name and remember best ranked item for it
        TStrFltH NameToRankH; TStrH NameToItemNH;
        const int Items = InItemV.Len();
        for (int ItemN = 0; ItemN < Items; ItemN++) {
            TBlobPt BlobPt = InItemV[ItemN].Dat.GetBlobPt();
            TStr Name = GetDocTitle(BlobPt);
            const double Rank = InItemV[ItemN].Key;
            if (NameToRankH.IsKey(Name)) {
                //TODO why? const int KeyId = NameToRankH.GetKeyId(Name);
                const double OldRank = NameToRankH.GetDat(Name);
                if (Rank > OldRank) {
                    NameToRankH.GetDat(Name) = Rank;
                    NameToItemNH.GetDat(Name) = ItemN;
                }
            } else {
                NameToRankH.AddDat(Name) = Rank;
                NameToItemNH.AddDat(Name) = ItemN;
            }
        }
        // load the best items int othe OutItemV
        int KeyId = NameToItemNH.FFirstKeyId();
        while (NameToItemNH.FNextKeyId(KeyId)) {
            const int ItemN = NameToItemNH[KeyId];
            OutItemV.Add(InItemV[ItemN]);
        }
    } else if (Grouping == wgbgDate) {
        Fail;
    } else if (Grouping == wgbgDateTime) {
        Fail;
    }
}

TWdGixBs::TWdGixBs(const TStr& _FPath, const TFAccess& _FAccess, const int64& CacheSize) {
    // store policy
    FPath = _FPath; FAccess = _FAccess;
    // create blob base
    TStr WdGixBsFNm = TStr::GetNrFPath(FPath) + TGixConst::WdGixBsFNm;
    DocBBs = TMBlobBs::New(WdGixBsFNm, FAccess);
    // load metadata store
    if (FAccess == faCreate) { WdGixMDS = TWdGixMDS::New(); }
    else {
        TStr WdGixMDSFNm = TStr::GetNrFPath(FPath) + TGixConst::WdGixMDSFNm;
        WdGixMDS = TWdGixMDS::LoadBin(WdGixMDSFNm);
    }
    // load inverted index
    WdGix = TWdGix::New(FPath, FAccess, CacheSize);
}

TWdGixBs::~TWdGixBs() {
    if ((FAccess == faCreate) || (FAccess == faUpdate)) {
        TStr WdGixMDSFNm = TStr::GetNrFPath(FPath) + TGixConst::WdGixMDSFNm;
        WdGixMDS->SaveBin(WdGixMDSFNm);
    }
}

void TWdGixBs::AddDoc(const TStr& DocTitle, const TStr& DocStr,
        const TStrV& CatNmV, const TTm& DateTime, const uchar& Wgt) {
    // save to blob base
    TMOut DocMOut;
    DocTitle.Save(DocMOut); DocStr.Save(DocMOut); CatNmV.Save(DocMOut);
    TBlobPt DocBlobPt = DocBBs->PutBlob(DocMOut.GetSIn());
    // save to metadata store
    if (DateTime.IsDef()) { WdGixMDS->AddDate(DocBlobPt, DateTime); }
    // add to index
    WdGix->AddHtml(DocStr, DocBlobPt, Wgt);
}

void TWdGixBs::AddDoc(const TStr& DocTitle, const TStr& DocStoreStr,
        const TStr& DocIndexStr, const TStrV& CatNmV,
        const TTm& DateTime, const uchar& Wgt) {

    // save to blob base
    TMOut DocMOut;
    DocTitle.Save(DocMOut); DocStoreStr.Save(DocMOut); CatNmV.Save(DocMOut);
    TBlobPt DocBlobPt = DocBBs->PutBlob(DocMOut.GetSIn());
    // save to metadata store
    if (DateTime.IsDef()) { WdGixMDS->AddDate(DocBlobPt, DateTime); }
    // add to index
    WdGix->AddHtml(DocIndexStr, DocBlobPt, Wgt);
}

void TWdGixBs::GetDoc(const TBlobPt& BlobPt,
        TStr& DocTitle, TStr& DocStr, TStrV& CatNmV) const {
    PSIn SIn = DocBBs->GetBlob(BlobPt);
    DocTitle.Load(*SIn); DocStr.Load(*SIn); CatNmV.Load(*SIn);
}

TStr TWdGixBs::GetDocTitle(const TBlobPt& BlobPt) const {
    PSIn SIn = DocBBs->GetBlob(BlobPt);
    TStr DocTitle; DocTitle.Load(*SIn);
    return DocTitle;
}

TStr TWdGixBs::GetDocStr(const TBlobPt& BlobPt) const {
    PSIn SIn = DocBBs->GetBlob(BlobPt);
    {TStr DocTitle; DocTitle.Load(*SIn);}
    TStr DocStr; DocStr.Load(*SIn);
    return DocStr;
}

TStrV TWdGixBs::GetDocCatNmV(const TBlobPt& BlobPt) const {
    PSIn SIn = DocBBs->GetBlob(BlobPt);
    {TStr DocTitle; DocTitle.Load(*SIn);}
    {TStr DocStr; DocStr.Load(*SIn);}
    TStrV CatNmV; CatNmV.Load(*SIn);
    return CatNmV;
}

PWdGixRSet TWdGixBs::SearchDoc(const TStr& QueryStr,
        const TWdGixBsGrouping& Grouping, TWdGixRankFun& RankFun,
        const int& Docs, const int& Offset, const TTm& MnDate,
        const TTm& MxDate) {

    // retrieve list of matching documents from inverted index
    printf("  Loading from Gix ...\n");
    TWdGixItemV ResItemV; WdGix->Search(QueryStr, ResItemV);
    // sort and filter by date the documents
    printf("  Weighting %d hits ...\n", ResItemV.Len());
    TWgtWdGixItemKdV FullWgtItemV(ResItemV.Len(), 0);
    const bool CheckMnDateP = MnDate.IsDef();
    const bool CheckMxDateP = MxDate.IsDef();
    for (int ItemN = 0; ItemN < ResItemV.Len(); ItemN++) {
        const TWdGixItem& Item = ResItemV[ItemN];
        TTm DateTime = WdGixMDS->GetDateTTm(Item.GetBlobPt());
        // check if document in time range
        if (CheckMnDateP && DateTime < MnDate) { continue; }
        if (CheckMxDateP && DateTime > MxDate) { continue; }
        // calculate the ranking weight of the document
        const double Wgt = RankFun(DateTime, Item.GetWgt(), Item.GetWdPos(),
            Item.IsTitle(), Item.IsNmObj(), Item.IsAnchor(), Item.IsEmph());
        // add to the full result list
        FullWgtItemV.Add(TWgtWdGixItemKd(Wgt, Item));
    }
    // filter the result list
    printf("  Filtering ...\n");
    if (Grouping != wgbgNone) {
        TWgtWdGixItemKdV TmpWgtItemV;
        Filter(FullWgtItemV, Grouping, TmpWgtItemV);
        FullWgtItemV = TmpWgtItemV;
    }
    // select the correct portion of documents
    printf("  Sorting %d hits ...\n", FullWgtItemV.Len());
    TWgtWdGixItemKdV WgtItemV;
    if (Docs == -1) {
        // return all resuts
        WgtItemV = FullWgtItemV;
    } else if (ResItemV.Len() >= (Docs + Offset)) {
        // get a subset of results from perticular sub-page
        WgtItemV = FullWgtItemV;
        WgtItemV.Sort(false); WgtItemV.Trunc(Docs + Offset);
        WgtItemV.Sort(true); WgtItemV.Trunc(Docs);
    } else if (ResItemV.Len() > Offset) {
        // last page ...
        WgtItemV = FullWgtItemV; WgtItemV.Sort(true);
        WgtItemV.Trunc(FullWgtItemV.Len() - Offset);
    } else {
        // out of range, leave empty
    }
    // sort according to the rank
    WgtItemV.Sort(false);
    // feed them to the result set
    printf("  Loading content for %d hits ...\n", WgtItemV.Len());
    PWdGixRSet RSet = TWdGixRSet::New(
        QueryStr, FullWgtItemV.Len(), Offset);
    for (int ItemN = 0; ItemN < WgtItemV.Len(); ItemN++) {
        const TWdGixItem& Item = WgtItemV[ItemN].Dat;
        TBlobPt DocBlobPt = Item.GetBlobPt();
        TStr DocTitle, DocStr; TStrV CatNmV;
        GetDoc(DocBlobPt, DocTitle, DocStr, CatNmV);
        TTm DateTime = WdGixMDS->GetDateTTm(DocBlobPt);
        RSet->AddDoc(DocTitle, DocStr, CatNmV, DateTime);
    }
    printf("  Done\n");
    return RSet;
}

void TWdGixBs::AddReuters(const TStr& XmlFNm) {
    // parse reuters articles
    PXmlDoc Doc=TXmlDoc::LoadTxt(XmlFNm);
    // parse date
    TStr DateStr = Doc->GetTagTok("newsitem")->GetArgVal("date");
    TTm DateTm = TTm::GetTmFromWebLogDateTimeStr(DateStr, '-');
    // parse content
    TChA DocChA; DocChA += "<doc>";
    TStr DocTitle = Doc->GetTagTok("newsitem|title")->GetTokStr(false);
    DocChA += "<title>"; DocChA += TXmlDoc::GetXmlStr(DocTitle); DocChA += "</title>";
    DocChA += "<body>";
    // get headline as emphesised text
    TStr DocHeadline = Doc->GetTagTok("newsitem|headline")->GetTokStr(false);
    DocChA += "<p><em>"; DocChA += TXmlDoc::GetXmlStr(DocHeadline); DocChA += "</em></p>\n";
    // document content
    TXmlTokV ParTokV; Doc->GetTagTokV("newsitem|text|p", ParTokV);
    for (int ParTokN = 0; ParTokN < ParTokV.Len(); ParTokN++){
        TStr ParStr = TXmlDoc::GetXmlStr(ParTokV[ParTokN]->GetTokStr(false));
        TXmlTokV NmObjTokV; ParTokV[ParTokN]->GetTagTokV("enamex", NmObjTokV);
        // mark name entities
        for (int NmObjTokN = 0; NmObjTokN < NmObjTokV.Len(); NmObjTokN++) {
            TStr NmObjStr = TXmlDoc::GetXmlStr(NmObjTokV[NmObjTokN]->GetTokStr(false));
            ParStr.ChangeStrAll(NmObjStr, "<nmobj>" + NmObjStr + "</nmobj>");
        }
        DocChA += "<p>"; DocChA += ParStr; DocChA += "</p>";
    }
    DocChA += "</body></doc>";
    // categories
    TStrV CatNmV;
    TXmlTokV CdsTokV; Doc->GetTagTokV("newsitem|metadata|codes", CdsTokV);
    for (int CdsTokN = 0; CdsTokN < CdsTokV.Len(); CdsTokN++){
        PXmlTok CdsTok = CdsTokV[CdsTokN];
        TXmlTokV CdTokV; CdsTok->GetTagTokV("code", CdTokV);
        if (CdsTok->GetArgVal("class") == "bip:topics:1.0"){
            for (int CdTokN = 0; CdTokN < CdTokV.Len(); CdTokN++){
                TStr CdNm = CdTokV[CdTokN]->GetArgVal("code");
                CatNmV.AddMerged(CdNm);
            }
        } else if (CdsTok->GetArgVal("class")=="bip:countries:1.0"){
            for (int CdTokN = 0; CdTokN < CdTokV.Len(); CdTokN++){
                TStr CdNm=CdTokV[CdTokN]->GetArgVal("code");
                CatNmV.AddMerged(CdNm);
            }
        } else if (CdsTok->GetArgVal("class")=="bip:industries:1.0"){
            for (int CdTokN = 0; CdTokN < CdTokV.Len(); CdTokN++){
                TStr CdNm=CdTokV[CdTokN]->GetArgVal("code");
                CatNmV.AddMerged(CdNm);
            }
        } else { Fail; }
    }
    // store the news article to the search base
    AddDoc(DocTitle, DocChA, CatNmV, DateTm);
}

void TWdGixBs::IndexReuters(const TStr& FPath) {
    PNotify Notify = TStdNotify::New();
    Notify->OnStatus("Loading Reuters documents from " + FPath + " ...\n");
    TFFile FFile(FPath, ".xml", true);
    TStr XmlFNm; int Files = 0;
    while (FFile.Next(XmlFNm)) {
        //if (Files > 10000) { break; }
        // load document
        if (TFile::Exists(XmlFNm)) { AddReuters(XmlFNm); Files++; }
        // print statistics
        if (Files % 1000 == 0) { Notify->OnStatus(TStr::Fmt("F:%d\r", Files)); }
    }
    Notify->OnStatus(TStr::Fmt("F:%d\n", Files));
}

void TWdGixBs::IndexNmEnBs(const TStr& FNm) {
    PNotify Notify = TStdNotify::New();
    Notify->OnStatus("Loading name-entitites from " + FNm + " ...\n");
    // load name-entity base together with contexts
    PNmEnBs NmEnBs = TNmEnBs::LoadBin(FNm, true);
    // add name-entities to
    int NmEnKeyId = NmEnBs->GetFFirstNmEn();
    int NmEnN = 0; const int NmEns = NmEnBs->GetNmEns();
    while (NmEnBs->GetFNextNmEn(NmEnKeyId)) {
        if (NmEnN > 100000) { break; }
        // print statistics
        if (NmEnN % 1000 == 0) { Notify->OnStatus(TStr::Fmt("N:%d/%d\r", NmEnN, NmEns)); }
        // get name-entity name
        TStr NmEnStr = NmEnBs->GetNmEnStr(NmEnKeyId);
        IAssertR(NmEnBs->IsNmEn(NmEnStr), NmEnStr);
        // iterate over all the mentions of the name-entity
        // and concatenate them accross days
        THash<TUInt, TChA> DateIntToCtxH; THash<TUInt, TInt> DateIntToCountH;
        const TIntV& NmEnCtxIdV = NmEnBs->GetCtxIdV(NmEnKeyId);
        for (int CtxIdN = 0; CtxIdN < NmEnCtxIdV.Len(); CtxIdN++) {
            const int CtxId = NmEnCtxIdV[CtxIdN];
            TStr NmEnCtxStr = NmEnBs->GetCtxStr(CtxId);
            TTm NmEnCtxTm = NmEnBs->GetCtxTm(CtxId);
            const uint DateInt = TTm::GetDateIntFromTm(NmEnCtxTm);
            DateIntToCtxH.AddDat(DateInt) += NmEnCtxStr;
            DateIntToCountH.AddDat(DateInt)++;
        }
        // store the name-entity to the search base per day
        int CtxKeyId = DateIntToCtxH.FFirstKeyId();
        while (DateIntToCtxH.FNextKeyId(CtxKeyId)) {
            const int DateInt = DateIntToCtxH.GetKey(CtxKeyId);
            TTm CtxDate = TTm::GetTmFromDateTimeInt(DateInt);
            TStr CtxStr = DateIntToCtxH[CtxKeyId];
            const uchar Wgt = uchar(DateIntToCountH.GetDat(DateInt).Val);
            AddDoc(NmEnStr, CtxStr, TStrV(), CtxDate, Wgt);
        }
        // next
        NmEnN++;
    }
    Notify->OnStatus(TStr::Fmt("N:%d/%d", NmEnN, NmEns));
}

void TWdGixBs::IndexNyt(const TStr& XmlFNm) {
    PNotify Notify = TStdNotify::New();
    Notify->OnStatus("Loading NYT documents from " + XmlFNm + " ...\n");

    PSIn SIn = TFIn::New(XmlFNm); int Docs = 0;
    TStr LastTitle = "";
    forever {
        if (Docs % 1000 == 0) { Notify->OnStatus(TStr::Fmt("Docs: %d\r", Docs)); }
        PXmlDoc Doc = TXmlDoc::LoadTxt(SIn); Docs++;
        if (!Doc->IsOk()) { printf("%s - %s\n", LastTitle.CStr(), Doc->GetMsgStr().CStr()); break; }
        // parse date
        TStr DateStr = Doc->GetTagTok("newsitem")->GetArgVal("date");
        TTm DateTm = TTm::GetTmFromWebLogDateTimeStr(DateStr, '-');
        // parse content
        TChA DocChA; DocChA += "<doc>";
        TStr DocTitle = Doc->GetTagTok("newsitem|title")->GetTokStr(false);
        DocChA += "<title>"; DocChA += TXmlDoc::GetXmlStr(DocTitle); DocChA += "</title>";
        DocChA += "<body>";
        // document content
        TXmlTokV ParTokV; Doc->GetTagTokV("newsitem|text|p", ParTokV);
        for (int ParTokN = 0; ParTokN < ParTokV.Len(); ParTokN++){
            TStr ParStr = TXmlDoc::GetXmlStr(ParTokV[ParTokN]->GetTokStr(false));
            TXmlTokV NmObjTokV; ParTokV[ParTokN]->GetTagTokV("ent", NmObjTokV);
            // mark name entities
            for (int NmObjTokN = 0; NmObjTokN < NmObjTokV.Len(); NmObjTokN++) {
                TStr NmObjStr = TXmlDoc::GetXmlStr(NmObjTokV[NmObjTokN]->GetTokStr(false));
                ParStr.ChangeStrAll(NmObjStr, "<nmobj>" + NmObjStr + "</nmobj>");
            }
            DocChA += "<p>"; DocChA += ParStr; DocChA += "</p>";
        }
        DocChA += "</body></doc>";
        // store the news article to the search base
        AddDoc(DocTitle, DocChA, TStrV(), DateTm);
        LastTitle = DocTitle;
    }
    Notify->OnStatus(TStr::Fmt("Docs: %d", Docs));
}

/////////////////////////////////////////////////
// Search-Topics
TSearchTopics::TSearchTopics(PWdGixRSet RSet, const int& Topics) {
    PBowDocBs BowDocBs = RSet->GenBowDocBs();
    TRnd Rnd(1);
    PBowDocPart BowDocPart = TBowClust::GetKMeansPart(
        TNullNotify::New(), BowDocBs, TBowSim::New(bstCos), Rnd,
        Topics, 1, 10, 1, bwwtLogDFNrmTFIDF, 0.0, 0);

    TopicV.Gen(Topics, 0);
    TIntH FrameH; THash<TInt, TIntH> FrameTopicHH;
    for (int ClustN = 0; ClustN < BowDocPart->GetClusts(); ClustN++) {
        PBowDocPartClust Clust = BowDocPart->GetClust(ClustN);
        TStr TopicNm = Clust->GetConceptSpV()->GetStr(BowDocBs, 3, 1, ", ", false, false);
        TopicV.Add(TopicNm);
        for (int DocN = 0; DocN < Clust->GetDocs(); DocN++) {
            const int DocId = Clust->GetDId(DocN);
            TTm DocDate = RSet->GetDocDateTime(DocId);
            //const uint FrameId = TTm::GetMonthIntFromTm(DocDate);
            const uint FrameId = TTm::GetYearIntFromTm(DocDate);
            FrameH.AddDat(FrameId)++;
            FrameTopicHH.AddDat(FrameId).AddDat(ClustN)++;
        }
    }

    const int Frames = FrameH.Len();
    FrameV.Gen(Frames, 0);
    TopicFrameFqVV.Gen(Topics, Frames); TopicFrameFqVV.PutAll(0.0);
    FrameH.SortByKey(); int FrameKeyId = FrameH.FFirstKeyId();
    while (FrameH.FNextKeyId(FrameKeyId)) {
        int FrameId = FrameH.GetKey(FrameKeyId);
        // load name
        TTm FrameDate = TTm::GetTmFromDateTimeInt(FrameId);
        //TStr FrameNm = TStr::Fmt("%4d-%2d", FrameDate.GetYear(), FrameDate.GetMonth());
        TStr FrameNm = TStr::Fmt("%4d", FrameDate.GetYear());
        const int FrameN = FrameV.Add(FrameNm);
        // load counts
        //const int FrameCounts = FrameH.GetDat(FrameId);
        const TIntH& TopicH = FrameTopicHH.GetDat(FrameId);
        int TopicKeyId = TopicH.FFirstKeyId(); int CountSum = 0;
        while (TopicH.FNextKeyId(TopicKeyId)) {
            const int TopicN = TopicH.GetKey(TopicKeyId);
            int TopicCount = TInt::Abs(TopicH.GetDat(TopicKeyId)) > 1000 ?
                0 : TopicH.GetDat(TopicKeyId)();
            CountSum += TopicCount;

            const double Fq = double(CountSum); // / double(FrameCounts);
            TopicFrameFqVV(TopicN, FrameN) = Fq;
        }
    }
}

/////////////////////////////////////////////////
// Triplet-Inverted-Index-Item
TTrGixItem::TTrGixItem(const TBlobPt& BlobPt, const int& _SubjectId,
    const int& _PredicatId, const int& _ObjectId, const int& _WdId,
    const uchar& Type, const uchar& Pos, const bool& Full, const bool& Stem,
    const uchar& Hyper): Seg(BlobPt.GetSeg()), Addr(BlobPt.GetAddr()),
        SubjectId(_SubjectId), PredicatId(_PredicatId), ObjectId(_ObjectId), WdId(_WdId) {

    SetWordInfo(Type, Pos, Full, Stem, Hyper);
    ClrMergeInfo();
}

TTrGixItem::TTrGixItem(TSIn& SIn) {
    SIn.Load(Seg); SIn.Load(Addr);
    SIn.Load(SubjectId);
    SIn.Load(PredicatId);
    SIn.Load(ObjectId);
    SIn.Load(WdId);
    SIn.Load(WdInfo);
    ClrMergeInfo();
}

void TTrGixItem::Save(TSOut& SOut) const {
    SOut.Save(Seg); SOut.Save(Addr);
    SOut.Save(SubjectId);
    SOut.Save(PredicatId);
    SOut.Save(ObjectId);
    SOut.Save(WdId);
    SOut.Save(WdInfo);
}

bool TTrGixItem::operator==(const TTrGixItem& Item) const {
    return ((Seg==Item.Seg)&&(Addr==Item.Addr)&&
        (SubjectId==Item.SubjectId)&&
        (PredicatId==Item.PredicatId)&&
        (ObjectId==Item.ObjectId));
}

bool TTrGixItem::operator<(const TTrGixItem& Item) const {
    return (Seg<Item.Seg) ||
        ((Seg==Item.Seg)&&(Addr<Item.Addr)) ||
        ((Seg==Item.Seg)&&(Addr==Item.Addr)&&(SubjectId<Item.SubjectId)) ||
        ((Seg==Item.Seg)&&(Addr==Item.Addr)&&(SubjectId==Item.SubjectId)&&(PredicatId<Item.PredicatId)) ||
        ((Seg==Item.Seg)&&(Addr==Item.Addr)&&(SubjectId==Item.SubjectId)&&(PredicatId==Item.PredicatId)&&(ObjectId<Item.ObjectId));
}

void TTrGixItem::SetWordInfo(const uchar& Type, const uchar& Pos,
        const bool& Full, const bool& Stem, const uchar& Hyper) {

    TTrGixItemWdInfo Info;
    Info.Short = 0;
    Info.Bits.Type = Type;
    Info.Bits.Pos = Type;
    Info.Bits.Full = Full ? 1 : 0;
    Info.Bits.Stem = Stem ? 1 : 0;
    Info.Bits.Hyper = Hyper;
    WdInfo = Info.Short;
}

/////////////////////////////////////////////////
// Triplet-Inverted-Index
char TTrGix::SubjectType = 0;
char TTrGix::SubjectWdType = 1;
char TTrGix::SubjectAttrWdType = 2;
char TTrGix::SubjectStemType = 3;
char TTrGix::SubjectAttrStemType = 4;
char TTrGix::PredicatType = 5;
char TTrGix::PredicatWdType = 6;
char TTrGix::PredicatAttrWdType = 7;
char TTrGix::PredicatStemType = 8;
char TTrGix::PredicatAttrStemType = 9;
char TTrGix::ObjectType = 10;
char TTrGix::ObjectWdType = 11;
char TTrGix::ObjectAttrWdType = 12;
char TTrGix::ObjectStemType = 13;
char TTrGix::ObjectAttrStemType = 14;

void TTrGix::AddTrPart(const int& FullId, const char& Type, const int& SubjectId,
        const int& PredicatId, const int& ObjectId, const TBlobPt& BlobPt) {

    // add to the index
    Gix->AddItem(TTrGixKey(FullId, Type),
        TTrGixItem(BlobPt, SubjectId, PredicatId,
            ObjectId, FullId, Type, 0, true, false, 0));
}

void TTrGix::AddTrPart(const TIntPrV& IdPrV, const char& WdType, const char& StemType,
        const int& SubjectId, const int& PredicatId, const int& ObjectId,
        const TBlobPt& BlobPt) {

    for (int IdPrN = 0; IdPrN < IdPrV.Len(); IdPrN++) {
        // add word to the index
        const int WdId = IdPrV[IdPrN].Val1;
        Gix->AddItem(TTrGixKey(WdId, WdType),
            TTrGixItem(BlobPt, SubjectId, PredicatId,
                ObjectId, WdId, WdType, IdPrN, false, false, 0));
        // add stem to the index
        const int StemId = IdPrV[IdPrN].Val2;
        Gix->AddItem(TTrGixKey(StemId, StemType),
            TTrGixItem(BlobPt, SubjectId, PredicatId,
                ObjectId, StemId, StemType, IdPrN, false, true, 0));
    }
}

TTrGix::PTGixExpItem TTrGix::GetExactExp(const TStr& Str, const char& Type) {
    TTrGixKey FullTrKey = TTrGixKey(GetWordId(Str, false), Type);
    return TTGixExpItem::NewItem(FullTrKey);
}

TTrGix::PTGixExpItem TTrGix::GetPartExp(const TStr& Str, const char& WdType, const char& StemType) {
    // get word/stem ids from the string
    TIntPrV WordStemIdV; GetWordIdV(Str, WordStemIdV, false);
    // prepare the expresion for these words/stems
    PTGixExpItem Exp = TTGixExpItem::NewEmpty();
    for (int WordStemIdN = 0; WordStemIdN < WordStemIdV.Len(); WordStemIdN++) {
        // prepare the search keys for both word and for the stem
        TTrGixKey WdKey(WordStemIdV[WordStemIdN].Val1, WdType);
        TTrGixKey StemKey(WordStemIdV[WordStemIdN].Val2, StemType);
        // either of the two should appear
        PTGixExpItem ExpItem = TTGixExpItem::NewOr(
            TTGixExpItem::NewItem(WdKey), TTGixExpItem::NewItem(StemKey));
        // update the expresion with the new word/stem
        if (Exp->IsEmpty()) { Exp = ExpItem; }
        else { Exp = TTGixExpItem::NewAnd(Exp, ExpItem); }
    }
    return Exp;
}


TTrGix::TTrGix(const TStr& _FPath, const TFAccess& _FAccess, const int64& CacheSize) {
    // store policy
    FPath = _FPath; FAccess = _FAccess;
    // prepare gix
    Gix = TTGix::New(TGixConst::TrGixFNm, FPath, FAccess, CacheSize);
    // load word tables
    if (FAccess == faCreate) {
        Stemmer = TStemmer::New(stmtPorter, true);
    } else {
        // prepare file name
        TStr TrGixDatFNm = TStr::GetNrFPath(FPath) + TGixConst::TrGixDatFNm;
        // load stuff
        TFIn FIn(TrGixDatFNm);
        WordH.Load(FIn);
        Stemmer = TStemmer::Load(FIn);
    }
}

TTrGix::~TTrGix() {
    if ((FAccess == faCreate) || (FAccess == faUpdate)) {
        // prepare file name
        TStr TrGixDatFNm = TStr::GetNrFPath(FPath) + TGixConst::TrGixDatFNm;
        // save word tables
        TFOut FOut(TrGixDatFNm);
        WordH.Save(FOut);
        Stemmer->Save(FOut);
    }
}

int TTrGix::GetWordId(const TStr& WordStr, const bool& AddIfNotExistP) {
    if (WordStr.Empty()) { return -1; }
    // should we just do a look-up or add the word if not exist
    if (AddIfNotExistP) { return WordH.AddKey(WordStr.GetUc()); }
    else { return WordH.GetKeyId(WordStr.GetUc()); }
}

inline TStr TTrGix::GetWordStr(const int& WId) const {
    return WId != -1 ? WordH.GetKey(WId) : "";
}

void TTrGix::GetWordIdV(const TStr& Str,
        TIntPrV& WordStemIdV, const bool& AddIfNotExistP) {

    // tokenize the phrase
    PSIn HtmlSIn = TStrIn::New(Str);
    THtmlLx HtmlLx(HtmlSIn); HtmlLx.GetSym();
    while (HtmlLx.Sym != hsyEof) {
        if (HtmlLx.Sym == hsyStr) {
            const TStr WordStr = HtmlLx.UcChA;
            // get the word id
            const int WordId = GetWordId(WordStr, AddIfNotExistP);
            // get the stem id
            const int StemId = GetWordId(Stemmer->GetStem(WordStr), AddIfNotExistP);
            // store it
            WordStemIdV.Add(TIntPr(WordId, StemId));
        }
        HtmlLx.GetSym();
    }
}

void TTrGix::GetWordIdV(const TStrV& WordStrV,
        TIntPrV& WordStemIdV, const bool& AddIfNotExistP) {

    // just iterate over all the elements and get their word-stem pairs
    for (int WordStrN = 0; WordStrN < WordStrV.Len(); WordStrN++) {
        GetWordIdV(WordStrV[WordStrN], WordStemIdV, AddIfNotExistP);
    }
}

void TTrGix::AddTr(const TStr& SubjectStr, const TStrV& SubjectAttrV,
        const TStr& PredicatStr, const TStrV& PredicatAttrV, const TStr& ObjectStr,
        const TStrV& ObjectAttrV, const TBlobPt& BlobPt) {

    // store full strings
    const int SubjectId = GetWordId(SubjectStr, true);
    const int PredicatId = GetWordId(PredicatStr, true);
    const int ObjectId = GetWordId(ObjectStr, true);
    // add them to the index
    AddTrPart(SubjectId, SubjectType, SubjectId, PredicatId, ObjectId, BlobPt);
    AddTrPart(PredicatId, PredicatType, SubjectId, PredicatId, ObjectId, BlobPt);
    AddTrPart(ObjectId, ObjectType, SubjectId, PredicatId, ObjectId, BlobPt);
    // get ids of separate words and their stems, including from attributes
    TIntPrV SubjectWIdSIdV; GetWordIdV(SubjectStr, SubjectWIdSIdV, true);
    TIntPrV SubjectAttrWIdSIdV; GetWordIdV(SubjectAttrV, SubjectAttrWIdSIdV, true);
    TIntPrV PredicatWIdSIdV; GetWordIdV(PredicatStr, PredicatWIdSIdV, true);
    TIntPrV PredicatAttrWIdSIdV; GetWordIdV(PredicatAttrV, PredicatAttrWIdSIdV, true);
    TIntPrV ObjectWIdSIdV; GetWordIdV(ObjectStr, ObjectWIdSIdV, true);
    TIntPrV ObjectAttrWIdSIdV; GetWordIdV(ObjectAttrV, ObjectAttrWIdSIdV, true);
    // add them to the index
    AddTrPart(SubjectWIdSIdV, SubjectWdType, SubjectStemType,
        SubjectId, PredicatId, ObjectId, BlobPt);
    AddTrPart(SubjectAttrWIdSIdV, SubjectAttrWdType, SubjectAttrStemType,
        SubjectId, PredicatId, ObjectId, BlobPt);
    AddTrPart(PredicatWIdSIdV, PredicatWdType, PredicatStemType,
        SubjectId, PredicatId, ObjectId, BlobPt);
    AddTrPart(PredicatAttrWIdSIdV, PredicatAttrWdType, PredicatAttrStemType,
        SubjectId, PredicatId, ObjectId, BlobPt);
    AddTrPart(ObjectWIdSIdV, ObjectWdType, ObjectStemType,
        SubjectId, PredicatId, ObjectId, BlobPt);
    AddTrPart(ObjectAttrWIdSIdV, ObjectAttrWdType, ObjectAttrStemType,
        SubjectId, PredicatId, ObjectId, BlobPt);
}

bool TTrGix::SearchExact(const TStr& SubjectStr, const TStr& PredicatStr,
        const TStr& ObjectStr, TTrGixItemV& ResItemV) {

    // generate exprasion
    PTGixExpItem Exp = TTGixExpItem::NewEmpty();
    if (!SubjectStr.Empty()) {
        PTGixExpItem ExpItem = GetExactExp(SubjectStr, SubjectType);
        if (Exp->IsEmpty()) { Exp = ExpItem; }
        else { Exp = TTGixExpItem::NewAnd(Exp, ExpItem); }
    }
    if (!PredicatStr.Empty()) {
        PTGixExpItem ExpItem = GetExactExp(PredicatStr, PredicatType);
        if (Exp->IsEmpty()) { Exp = ExpItem; }
        else { Exp = TTGixExpItem::NewAnd(Exp, ExpItem); }
    }
    if (!ObjectStr.Empty()) {
        PTGixExpItem ExpItem = GetExactExp(ObjectStr, ObjectType);
        if (Exp->IsEmpty()) { Exp = ExpItem; }
        else { Exp = TTGixExpItem::NewAnd(Exp, ExpItem); }
    }
    // evaluate the expresion
    return Exp->Eval(Gix, ResItemV);
}

bool TTrGix::SearchPart(const TStr& SubjectStr, const TStr& PredicatStr,
        const TStr& ObjectStr, TTrGixItemV& ResItemV, const bool& IncExactP) {

    // generate exprasion
    PTGixExpItem Exp = TTGixExpItem::NewEmpty();
    if (!SubjectStr.Empty()) {
        PTGixExpItem ExpItem = GetPartExp(SubjectStr, SubjectWdType, SubjectStemType);
        if (IncExactP) { TTGixExpItem::NewOr(GetExactExp(SubjectStr, SubjectType), ExpItem); }
        if (Exp->IsEmpty()) { Exp = ExpItem; }
        else { Exp = TTGixExpItem::NewAnd(Exp, ExpItem); }
    }
    if (!PredicatStr.Empty()) {
        PTGixExpItem ExpItem = GetPartExp(PredicatStr, PredicatWdType, PredicatStemType);
        if (IncExactP) { TTGixExpItem::NewOr(GetExactExp(PredicatStr, PredicatType), ExpItem); }
        if (Exp->IsEmpty()) { Exp = ExpItem; }
        else { Exp = TTGixExpItem::NewAnd(Exp, ExpItem); }
    }
    if (!ObjectStr.Empty()) {
        PTGixExpItem ExpItem = GetPartExp(ObjectStr, ObjectWdType, ObjectStemType);
        if (IncExactP) { TTGixExpItem::NewOr(GetExactExp(ObjectStr, ObjectType), ExpItem); }
        if (Exp->IsEmpty()) { Exp = ExpItem; }
        else { Exp = TTGixExpItem::NewAnd(Exp, ExpItem); }
    }
    // evaluate the expresion
    return Exp->Eval(Gix, ResItemV);
}

/////////////////////////////////////////////////
// TrGix-Result-Set
void TTrGixRSet::AddTr(const TStrTr& TrStr, const TBlobPtV& TrAttrBlobPtV) {
    TrStrV.Add(TrStr);
    TrAttrBlobPtVV.Add(TrAttrBlobPtV);
}

void TTrGixRSet::GetSubjectV(TStrIntKdV& SubjectStrWgtV) {
    TIntStrKdV SubjectWgtStrV;
    for (int TrN = 0; TrN < GetTrs(); TrN++) {
        const TStr& SubjectStr = TrStrV[TrN].Val1;
        const int Wgt = GetTrCount(TrN);
        SubjectWgtStrV.Add(TIntStrKd(Wgt, SubjectStr));
    }
    SubjectWgtStrV.Sort(false);
    GetSwitchedKdV<TInt, TStr>(SubjectWgtStrV, SubjectStrWgtV);
}

void TTrGixRSet::GetPredicatV(TStrIntKdV& PredicatStrWgtV) {
    TIntStrKdV PredicatWgtStrV;
    for (int TrN = 0; TrN < GetTrs(); TrN++) {
        const TStr& PredicatStr = TrStrV[TrN].Val2;
        const int Wgt = GetTrCount(TrN);
        PredicatWgtStrV.Add(TIntStrKd(Wgt, PredicatStr));
    }
    PredicatWgtStrV.Sort(false);
    GetSwitchedKdV<TInt, TStr>(PredicatWgtStrV, PredicatStrWgtV);
}

void TTrGixRSet::GetObjectV(TStrIntKdV& ObjectStrWgtV) {
    TIntStrKdV ObjectWgtStrV;
    for (int TrN = 0; TrN < GetTrs(); TrN++) {
        const TStr& ObjectStr = TrStrV[TrN].Val3;
        const int Wgt = GetTrCount(TrN);
        ObjectWgtStrV.Add(TIntStrKd(Wgt, ObjectStr));
    }
    ObjectWgtStrV.Sort(false);
    GetSwitchedKdV<TInt, TStr>(ObjectWgtStrV, ObjectStrWgtV);
}

void TTrGixRSet::PrintRes(const bool& PrintSentsP, PNotify Notify) const {
    // result set stats
    printf("Query:\n");
    printf("  Subject: '%s'\n", GetSubjectStr().CStr());
    printf("  Predicat: '%s'\n", GetPredicatStr().CStr());
    printf("  Object: '%s'\n", GetObjectStr().CStr());
    printf("Displaying: %d - %d (All hits: %d)\n",
        GetOffset()+1, Offset.Val+GetTrs()+1, GetAllTrs());
    // hits
    for (int TrN = 0; TrN < GetTrs(); TrN++) {
        printf("%d. [%s <- %s -> %s], (Support:%d)\n", TrN+GetOffset()+1,
            GetTrSubjectStr(TrN).CStr(), GetTrPredicatStr(TrN).CStr(),
            GetTrObjectStr(TrN).CStr(), GetTrCount(TrN));
        //if (PrintSentsP) {
        //    for (int SentN = 0; SentN < GetTrSents(TrN); SentN++) {
        //        const TStr& SentStr = GetTrSentStr(TrN, SentN);
        //        if (SentStr.Len() < 110) { printf("     %s\n", SentStr.CStr()); }
        //        else { printf("     %s...\n", SentStr.Left(110).CStr()); }
        //    }
        //}
    }
}

/////////////////////////////////////////////////
// TrGix-Merger


/////////////////////////////////////////////////
// TrGix-Base
void TTrGixBs::GetAttrV(PXmlTok XmlTok, TStrV& AttrV) {
    TXmlTokV AttrTokV; XmlTok->GetTagTokV("attrib", AttrTokV);
    for (int AttrTokN = 0; AttrTokN < AttrTokV.Len(); AttrTokN++) {
        PXmlTok AttrTok = AttrTokV[AttrTokN];
        AttrV.Add(AttrTok->GetStrArgVal("word"));
        GetAttrV(AttrTok, AttrV);
    }
}

TTrGixBs::TTrGixBs(const TStr& _FPath, const TFAccess& _FAccess, const int64& CacheSize) {
    // store policy
    FPath = _FPath; FAccess = _FAccess;
    // create document blob base
    TStr TrGixDocBsFNm = TStr::GetNrFPath(FPath) + TGixConst::TrGixDocBsFNm;
    DocBBs = TMBlobBs::New(TrGixDocBsFNm, FAccess);
    // create sentence blob base
    TStr TrGixSentBsFNm = TStr::GetNrFPath(FPath) + TGixConst::TrGixSentBsFNm;
    SentBBs = TMBlobBs::New(TrGixSentBsFNm, FAccess);
    // create triplet attribute blob base
    TStr TrGixTrAttrBsFNm = TStr::GetNrFPath(FPath) + TGixConst::TrGixTrAttrBsFNm;
    TrAttrBBs = TMBlobBs::New(TrGixTrAttrBsFNm, FAccess);
    // load inverted index
    TrGix = TTrGix::New(FPath, FAccess, CacheSize);
}

TTrGixBs::~TTrGixBs() {
    if ((FAccess == faCreate) || (FAccess == faUpdate)) {
        // save whatever doesnt save itself automaticaly in destructor
    }
}

TBlobPt TTrGixBs::AddDoc(const TStr& DocTitle, const TStr& DocStr, const TStrV& CatNmV) {
    // save to blob base
    TMOut DocMOut;
    DocTitle.Save(DocMOut); DocStr.Save(DocMOut); CatNmV.Save(DocMOut);
    TBlobPt DocBlobPt = DocBBs->PutBlob(DocMOut.GetSIn());
    return DocBlobPt;
}

TBlobPt TTrGixBs::AddSent(const TStr& SentStr) {
    TMOut SentMOut; SentStr.Save(SentMOut);
    TBlobPt SentBlobPt = SentBBs->PutBlob(SentMOut.GetSIn());
    return SentBlobPt;
}

TBlobPt TTrGixBs::AddTrAttr(const TStr& SubjectStr, const TStrV& SubjectAttrV,
        const TStr& PredicatStr, const TStrV& PredicatAttrV, const TStr& ObjectStr,
        const TStrV& ObjectAttrV, const TBlobPt& SentBlobPt, const TBlobPt& DocBlobPt) {

    TMOut TrAttrMOut;
    SubjectStr.Save(TrAttrMOut); SubjectAttrV.Save(TrAttrMOut);
    PredicatStr.Save(TrAttrMOut); PredicatAttrV.Save(TrAttrMOut);
    ObjectStr.Save(TrAttrMOut); ObjectAttrV.Save(TrAttrMOut);
    SentBlobPt.Save(TrAttrMOut); DocBlobPt.Save(TrAttrMOut);
    TBlobPt TrAttrBlobPt = TrAttrBBs->PutBlob(TrAttrMOut.GetSIn());
    return TrAttrBlobPt;
}

void TTrGixBs::AddTr(const TStr& SubjectStr, const TStrV& SubjectAttrV,
        const TStr& PredicatStr, const TStrV& PredicatAttrV, const TStr& ObjectStr,
        const TStrV& ObjectAttrV, const TBlobPt& TrAttrBlobPt) {

    TrGix->AddTr(SubjectStr, SubjectAttrV, PredicatStr,
        PredicatAttrV, ObjectStr, ObjectAttrV, TrAttrBlobPt);
}

void TTrGixBs::GetDoc(const TBlobPt& DocBlobPt, TStr& DocTitle, TStr& DocStr, TStrV& CatNmV) const {
    PSIn SIn = DocBBs->GetBlob(DocBlobPt);
    DocTitle.Load(*SIn); DocStr.Load(*SIn); CatNmV.Load(*SIn);
}

TStr TTrGixBs::GetDocTitle(const TBlobPt& BlobPt) const {
    PSIn SIn = DocBBs->GetBlob(BlobPt);
    TStr DocTitle; DocTitle.Load(*SIn);
    return DocTitle;
}

TStr TTrGixBs::GetDocStr(const TBlobPt& BlobPt) const {
    PSIn SIn = DocBBs->GetBlob(BlobPt);
    {TStr DocTitle; DocTitle.Load(*SIn);}
    TStr DocStr; DocStr.Load(*SIn);
    return DocStr;
}

TStrV TTrGixBs::GetDocCatNmV(const TBlobPt& BlobPt) const {
    PSIn SIn = DocBBs->GetBlob(BlobPt);
    {TStr DocTitle; DocTitle.Load(*SIn);}
    {TStr DocStr; DocStr.Load(*SIn);}
    TStrV CatNmV; CatNmV.Load(*SIn);
    return CatNmV;
}

TStr TTrGixBs::GetSentStr(const TBlobPt& SentBlobPt) {
    PSIn SIn = SentBBs->GetBlob(SentBlobPt);
    return TStr(*SIn);
}

void TTrGixBs::GetTrAttr(const TBlobPt& TrAttrBlobPt, TStr& SubjectStr,
        TStrV& SubjectAttrV, TStr& PredicatStr, TStrV& PredicatAttrV,
        TStr& ObjectStr, TStrV& ObjectAttrV, TBlobPt& SentBlobPt,
        TBlobPt& DocBlobPt) {

    PSIn SIn = TrAttrBBs->GetBlob(TrAttrBlobPt);
    SubjectStr.Load(*SIn); SubjectAttrV.Load(*SIn);
    PredicatStr.Load(*SIn); PredicatAttrV.Load(*SIn);
    ObjectStr.Load(*SIn); ObjectAttrV.Load(*SIn);
    SentBlobPt = TBlobPt(*SIn); DocBlobPt = TBlobPt(*SIn);
}

PTrGixRSet TTrGixBs::SearchTr(const TStr& SubjectStr, const TStr& PredicatStr,
        const TStr& ObjectStr, TTrGixRankFun& RankFun, const int& Docs,
        const int& Offset, const bool& ExactP) {

    // get all hits from index
    //printf("  Loading from Gix ...\n");
    TTrGixItemV ResItemV;
    if (ExactP) { TrGix->SearchExact(SubjectStr, PredicatStr, ObjectStr, ResItemV); }
    else { TrGix->SearchPart(SubjectStr, PredicatStr, ObjectStr, ResItemV, true); }
    // grouping the hits by triplets
    //printf("  Grouping %d hits ...\n", ResItemV.Len());
    THash<TIntTr, TIntV> TrToItemVH;
    for (int ItemN = 0; ItemN < ResItemV.Len(); ItemN++) {
        const TTrGixItem& Item = ResItemV[ItemN];
        TrToItemVH.AddDat(Item.GetIdTr()).Add(ItemN);
    }
    // rank the triplets
    //printf("  Ranking %d triplets ...\n", TrToItemVH.Len());
    TFltIntKdV FullWgtTrKeyIdV;
    int TrKeyId = TrToItemVH.FFirstKeyId();
    while (TrToItemVH.FNextKeyId(TrKeyId)) {
        const double Wgt = RankFun(TrToItemVH[TrKeyId].Len());
        FullWgtTrKeyIdV.Add(TFltIntKd(Wgt, TrKeyId));
    }
    // select the correct portion of documents
    //printf("  Sorting %d triplets ...\n", TrToItemVH.Len());
    TFltIntKdV WgtTrKeyIdV;
    if (Docs == -1) {
        // return all resuts
        WgtTrKeyIdV = FullWgtTrKeyIdV;
    } else if (ResItemV.Len() >= (Docs + Offset)) {
        // get a subset of results from perticular sub-page
        WgtTrKeyIdV = FullWgtTrKeyIdV;
        WgtTrKeyIdV.Sort(false); WgtTrKeyIdV.Trunc(Docs + Offset);
        WgtTrKeyIdV.Sort(true); WgtTrKeyIdV.Trunc(Docs);
    } else if (ResItemV.Len() > Offset) {
        // last page ...
        WgtTrKeyIdV = FullWgtTrKeyIdV; WgtTrKeyIdV.Sort(true);
        WgtTrKeyIdV.Trunc(FullWgtTrKeyIdV.Len() - Offset);
    } else {
        // out of range, leave empty
    }
    // sort according to the rank
    WgtTrKeyIdV.Sort(false);
    // feed them to the result set
    //printf("  Loading content for %d triplets ...\n", WgtTrKeyIdV.Len());
    PTrGixRSet RSet = TTrGixRSet::New(SubjectStr, PredicatStr,
        ObjectStr, FullWgtTrKeyIdV.Len(), Offset);
    for (int TrN = 0; TrN < WgtTrKeyIdV.Len(); TrN++) {
        const int TrKeyId = WgtTrKeyIdV[TrN].Dat;
        const TIntTr& WIdTr = TrToItemVH.GetKey(TrKeyId);
        const TIntV& ItemV = TrToItemVH[TrKeyId];
        TStr SubjectStr = TrGix->GetWordStr(WIdTr.Val1);
        TStr PredicatStr = TrGix->GetWordStr(WIdTr.Val2);
        TStr ObjectStr = TrGix->GetWordStr(WIdTr.Val3);
        TStrTr TrStr(SubjectStr, PredicatStr, ObjectStr);
        // load triplet attribute blob pointers
        TBlobPtV TrAttrBlobPtV;
        for (int ItemN = 0; ItemN < ItemV.Len(); ItemN++) {
            const TTrGixItem& Item = ResItemV[ItemV[ItemN]];
            TBlobPt TrAttrBlobPt = Item.GetBlobPt();
            TrAttrBlobPtV.Add(TrAttrBlobPt);
        }
        // add the triplet to the RSet
        RSet->AddTr(TrStr, TrAttrBlobPtV);
    }
    //printf("  Done\n");
    return RSet;
}

void TTrGixBs::AddReuters(const TStr& XmlFNm, int& Trs, const PSOut& CsvOut) {
    PNotify Notify = TStdNotify::New();
    // create empty document
    TBlobPt EmptyDocBlobPt = AddDoc("No full document text!");
    // we skip the top tak so we only parse one sentence at a time
    PSIn XmlSIn = TFIn::New(XmlFNm);
    TXmlDoc::SkipTopTag(XmlSIn);
    PXmlDoc XmlDoc; int XmlDocs = 0;
    forever{
        // print count
        if (Trs % 100 == 0) { Notify->OnStatus(TStr::Fmt("%d\r", Trs)); }
        // load xml tree
        XmlDocs++; XmlDoc = TXmlDoc::LoadTxt(XmlSIn);
        // stop if at the last tag
        if (!XmlDoc->IsOk()) { break; }
        // extract documents from xml-trees
        PXmlTok TopTok = XmlDoc->GetTok();
        if (TopTok->IsTag("sentence")){
            // read and store the document from which the sentence comes
            TStr DocStr = ""; TBlobPt DocBlobPt = EmptyDocBlobPt;
            // read and store the sentence from where the triplet comes
            TStr SentStr = TopTok->GetTagTok("originalSentence")->GetTokStr(false);
            TBlobPt SentBlobPt = AddSent(SentStr);
            // iterate over all the triplets and add them to the index
            TXmlTokV TrTokV; TopTok->GetTagTokV("triplet", TrTokV);
            for (int TrTokN = 0; TrTokN < TrTokV.Len(); TrTokN++) {
                PXmlTok TrTok = TrTokV[TrTokN];
                TStr SubjectStr = TrTok->GetTagTok("subject")->GetStrArgVal("word");
                TStr PredicatStr = TrTok->GetTagTok("verb")->GetStrArgVal("word");
                TStr ObjectStr = TrTok->GetTagTok("object")->GetStrArgVal("word");
                TStrV SubjectAttrV; GetAttrV(TrTok->GetTagTok("subject"), SubjectAttrV);
                TStrV PredicatAttrV; GetAttrV(TrTok->GetTagTok("verb"), PredicatAttrV);
                TStrV ObjectAttrV; GetAttrV(TrTok->GetTagTok("object"), ObjectAttrV);
                TBlobPt TrAttrBlobPt = AddTrAttr(SubjectStr, SubjectAttrV,
                    PredicatStr, PredicatAttrV, ObjectStr, ObjectAttrV,
                    SentBlobPt, DocBlobPt);
                AddTr(SubjectStr, SubjectAttrV, PredicatStr, PredicatAttrV,
                    ObjectStr, ObjectAttrV, TrAttrBlobPt);
                Trs++;
                // we should also output CSV file
                if (!CsvOut.Empty()) {
                    // make sure no ',' in the strings and dump the triplet
                    SubjectStr.DelChAll(','); CsvOut->PutStr(SubjectStr + ",");
                    PredicatStr.DelChAll(','); CsvOut->PutStr(PredicatStr + ",");
                    ObjectStr.DelChAll(','); CsvOut->PutStr(ObjectStr + ",");
                    // dump the document blob pointer
                    CsvOut->PutStr(TStr::Fmt("%u,", uint(SentBlobPt.GetSeg())));
                    CsvOut->PutStr(TStr::Fmt("%u,", SentBlobPt.GetAddr()));
                    // dump the adjectives
                    CsvOut->PutStr(TStr::Fmt("%d,", SubjectAttrV.Len()));
                    for (int AttrN = 0; AttrN < SubjectAttrV.Len(); AttrN++) {
                        SubjectAttrV[AttrN].DelChAll(',');
                        CsvOut->PutStr(SubjectAttrV[AttrN]);
                        CsvOut->PutStr(",");
                    }
                    CsvOut->PutStr(TStr::Fmt("%d,", PredicatAttrV.Len()));
                    for (int AttrN = 0; AttrN < PredicatAttrV.Len(); AttrN++) {
                        PredicatAttrV[AttrN].DelChAll(',');
                        CsvOut->PutStr(PredicatAttrV[AttrN]);
                        CsvOut->PutStr(",");
                    }
                    CsvOut->PutStr(TStr::Fmt("%d,", ObjectAttrV.Len()));
                    for (int AttrN = 0; AttrN < ObjectAttrV.Len(); AttrN++) {
                        ObjectAttrV[AttrN].DelChAll(',');
                        CsvOut->PutStr(ObjectAttrV[AttrN]);
                        CsvOut->PutStr(",");
                    }
                    CsvOut->PutStr("-1"); CsvOut->PutLn();
                }
            }
        }
    }
    CsvOut->Flush();
}

void TTrGixBs::IndexReuters(const TStr& XmlFPath, const TStr& CsvFNm, const int& MxTrs) {
    PNotify Notify = TStdNotify::New();
    Notify->OnStatus("Loading Reuters documents from " + XmlFPath + " ...\n");
    TFFile FFile(XmlFPath, ".xml", true); TStr XmlFNm; int Files = 0, Trs = 0;
    PSOut CsvOut; if (!CsvFNm.Empty()) { CsvOut = TFOut::New(CsvFNm); }
    while (FFile.Next(XmlFNm) && ((MxTrs == -11)||(MxTrs > Trs))) {
        // load document
        Notify->OnStatus(TStr::Fmt("Loading %3d : %s ...", Files+1, XmlFNm.CStr()));
        if (TFile::Exists(XmlFNm)) { AddReuters(XmlFNm, Trs, CsvOut); Files++; }
    }
    Notify->OnStatus(TStr::Fmt("Triplets loaded: %d", Trs));
}

/////////////////////////////////////////////////
// Multilingual-Word-Inverted-Index-Item
TMWdGixItem::TMWdGixItem(TSIn& SIn) {
    SIn.Load(Seg); SIn.Load(Addr);
    SIn.Load(WdFq); SIn.Load(DocWds);
}

void TMWdGixItem::Save(TSOut& SOut) const {
    SOut.Save(Seg); SOut.Save(Addr);
    SOut.Save(WdFq); SOut.Save(DocWds);
}

inline bool TMWdGixItem::operator==(const TMWdGixItem& Item) const {
    return (Seg == Item.Seg) && (Addr == Item.Addr);
}

inline bool TMWdGixItem::operator<(const TMWdGixItem& Item) const {
    return (Seg < Item.Seg) || ((Seg == Item.Seg) && (Addr < Item.Addr));
}

/////////////////////////////////////////////////
// General-Inverted-Index-Default-Merger
void TMWdGixDefMerger::Union(TMWdGixItemV& DstV, const TMWdGixItemV& SrcV) const {
    TMWdGixItemV DstValV(TInt::GetMx(DstV.Len(), SrcV.Len()), 0);
    int ValN1 = 0; int ValN2 = 0;
    while ((ValN1<DstV.Len()) && (ValN2<SrcV.Len())){
        const TMWdGixItem& Val1 = DstV.GetVal(ValN1);
        const TMWdGixItem& Val2 = SrcV.GetVal(ValN2);
        if (Val1 < Val2) { DstValV.Add(Val1); ValN1++; }
        else if (Val1>Val2) { DstValV.Add(Val2); ValN2++; }
        else { DstValV.Add(TMWdGixItem(Val1, Val2)); ValN1++; ValN2++; }
    }
    for (int RestValN1=ValN1; RestValN1<DstV.Len(); RestValN1++){
        DstValV.Add(DstV.GetVal(RestValN1));}
    for (int RestValN2=ValN2; RestValN2<SrcV.Len(); RestValN2++){
        DstValV.Add(SrcV.GetVal(RestValN2));}
    DstV = DstValV;
}

void TMWdGixDefMerger::Def(const TInt& Key, TMWdGixItemV& ItemV) const  {
    const int WdDocFq = MWdGix->GetWdFq(Key);
    const int Docs = MWdGix->GetAllDocs();
    const double AvgDocWds = MWdGix->GetAvgDocWds();
    const int Items = ItemV.Len();
    for (int ItemN = 0; ItemN < Items; ItemN++) {
        TMWdGixItem& Item = ItemV[ItemN];
        const int WdFq = Item.GetWdFq();
        const int DocWds = Item.GetDocWds();
        const double Wgt = RankFun->WdRank(WdFq, DocWds, WdDocFq, Docs, AvgDocWds);
        Item.PutWgt(Wgt);
    }
}

/////////////////////////////////////////////////
// Multilingual-Word-Inverted-Index
TMWdGix::TMWdGix(const TStr& _FPath, const TFAccess& _FAccess, const int64& CacheSize) {
    // store policy
    FPath = _FPath; FAccess = _FAccess;
    printf("Loading '%s' .. ", FPath.CStr());
    if (FAccess == faCreate) { printf("create .. "); }
    if (FAccess == faRdOnly) { printf("read-only .. "); }
    printf("Cache[%s]\n", TUInt64::GetMegaStr(CacheSize).CStr());
    // prepare gix
    MWGix = TMWGix::New(TGixConst::MWdGixFNm, FPath, FAccess, CacheSize);
    // load word tables
    if (FAccess != faCreate) {
        // prepare file name
        TStr MWdGixDatFNm = TStr::GetNrFPath(FPath) + TGixConst::MWdGixDatFNm;
        // load stuff
        TFIn FIn(MWdGixDatFNm);
        WordH.Load(FIn);
        AllDocs.Load(FIn);
        AllWords.Load(FIn);
    }
}

TMWdGix::~TMWdGix() {
    if ((FAccess == faCreate) || (FAccess == faUpdate)) {
        printf("Closing %s: docs=%d, words=%d\n",
            FPath.CStr(), AllDocs.Val, AllWords.Val);
        // prepare file name
        TStr MWdGixDatFNm = TStr::GetNrFPath(FPath) + TGixConst::MWdGixDatFNm;
        // save word tables
        TFOut FOut(MWdGixDatFNm);
        WordH.Save(FOut);
        AllDocs.Save(FOut);
        AllWords.Save(FOut);
    }
}

void TMWdGix::AddHtml(const TStr& DocStr, const TBlobPt& BlobPt) {
    // create html lexical
    PSIn HtmlSIn = TStrIn::New(DocStr);
    THtmlLx HtmlLx(HtmlSIn); HtmlLx.GetSym();
    // prepare word vector, position counter and flags
    TIntH DocWIdH; int DocWds = 0;
    // traverse html string symbols
    while (HtmlLx.Sym != hsyEof) {
        if (HtmlLx.Sym == hsyStr) {
            // get word token
            TStr WordStr = HtmlLx.UcChA;
            // get the word id
            const int WId = WordH.AddKey(WordStr);
            // increas the frequency count
            DocWIdH.AddDat(WId)++;
            // increase document word count
            DocWds++;
        }
        // get next symbol
        HtmlLx.GetSym();
    }
    // load add documents to words in inverted index
    int WdKeyId = DocWIdH.FFirstKeyId();
    while (DocWIdH.FNextKeyId(WdKeyId)) {
        const int WId = DocWIdH.GetKey(WdKeyId);
        const int WdFq = DocWIdH[WdKeyId];
        WordH[WId]++; // count for document frequency
        TMWdGixKey Key(WId);
        TMWdGixItem Item(BlobPt, WdFq, DocWds);
        // add to index
        MWGix->AddItem(Key, Item);
    }
    // increase counters
    AllDocs++; AllWords += DocWds;
}

bool TMWdGix::Search(const TStr& QueryStr,
        TMWdGixItemV& ResItemV, const TMWdGixDefMerger& Merger) {

    // first we prepare the query just for current language
    PMWGixExpItem MWGixExp = TMWGixExpItem::NewEmpty();
    PSIn HtmlSIn = TStrIn::New(QueryStr);
    THtmlLx HtmlLx(HtmlSIn); HtmlLx.GetSym();
    while (HtmlLx.Sym != hsyEof) {
        if (HtmlLx.Sym == hsyStr) {
            // get word token
            TStr WordStr = HtmlLx.UcChA;
            // check if we have it
            const int WId = WordH.GetKeyId(WordStr);
            if (WId != -1) {
                PMWGixExpItem MWGixExpItem = TMWGixExpItem::NewItem(TMWdGixKey(WId));
                if (MWGixExp->IsEmpty()) { MWGixExp = MWGixExpItem; }
                else { MWGixExp = TMWGixExpItem::NewOr(MWGixExp, MWGixExpItem); }
            }
        }
        // get next symbol
        HtmlLx.GetSym();
    }
    // search
    return MWGixExp->Eval(MWGix, ResItemV, Merger);
}

/////////////////////////////////////////////////
// MWdGix-Result-Set
TStr TMWdGixRSet::GetMainPara(const TStr& QueryStr, const TStr& FullStr) {
    PBowDocBs BowDocBs = TBowDocBs::New();
    BowDocBs->AddHtmlDoc("Query", TStrV(), QueryStr, false);
    TStrV ParaV; FullStr.SplitOnAllCh('\n', ParaV);
    if (ParaV.Empty()) { return ""; }
    for (int ParaN = 0; ParaN < ParaV.Len(); ParaN++) {
        BowDocBs->AddHtmlDoc("Doc" + TInt::GetStr(ParaN), TStrV(), ParaV[ParaN], false);
    }
    PBowDocWgtBs BowDocWgtBs = TBowDocWgtBs::New(BowDocBs, bwwtNrmTFIDF);
    PBowSpV QuerySpV = BowDocWgtBs->GetSpV(0); int MxParaN = 0;
    double MxParaSim = TBowSim::GetCosSim(QuerySpV, BowDocWgtBs->GetSpV(1));
    for (int ParaN = 1; ParaN < ParaV.Len(); ParaN++) {
        const double ParaSim = TBowSim::GetCosSim(QuerySpV, BowDocWgtBs->GetSpV(ParaN+1));
        if (ParaSim > MxParaSim) {
            MxParaSim = ParaSim;
            MxParaN = ParaN;
        }
    }
    return ParaV[MxParaN];
}

void TMWdGixRSet::AddDoc(const TStr& DocTitle, const TStr& DocStr,
        const TStr& DocLang, const TStrV& KeyWdV) {

    DocTitleV.Add(DocTitle);
    DocTitleV.Last().DelChAll('\n');
    DocTitleV.Last().DelChAll('\r');
    DocStrV.Add(GetMainPara(LangQueryH.GetDat(DocLang), DocStr));
    DocLangV.Add(DocLang);
    KeyWdVV.Add(KeyWdV);
}

void TMWdGixRSet::PrintRes(PNotify Notify) {
    const int Docs = GetDocs();
    Notify->OnStatus(TStr::Fmt("All results: %d, Showing results from %d to %d",
        AllDocs.Val, Docs, Docs + Offset.Val));
    for (int DocN = 0; DocN < Docs; DocN++) {
        TStr DocStr = DocTitleV[DocN] + " - " + DocStrV[DocN];
        DocStr.DelChAll('\n'); DocStr.DelChAll('\r');
        Notify->OnStatus(TStr::Fmt("[%d:%s] %s ...", DocN+1,
            DocLangV[DocN].CStr(), DocStr.Left(60).CStr()));
    }
    Notify->OnStatus(TStr::Fmt("All results: %d, Showing results from %d to %d",
        AllDocs.Val, Docs, Docs + Offset.Val));
}

TStr TMWdGixRSet::GetWsXml(const TStrPrStrH& EurovocH) const {
    PXmlTok TopTok = TXmlTok::New("cca");
    TopTok->AddArg("allhits", GetAllDocs());
    for (int DocN = 0; DocN < GetDocs(); DocN++) {
        PXmlTok HitTok = TXmlTok::New("hit");
        HitTok->AddArg("rank", DocN+1);
        HitTok->AddArg("lang", DocLangV[DocN]);
        TStr Title = DocTitleV[DocN];
        if (Title.Len() > 100) { Title = Title.Left(100) + "..."; }
        TStr Snipet = DocStrV[DocN].Left(800);
        if (Snipet.Len() > 800) { Snipet = Snipet.Left(800) + "..."; }
        HitTok->AddSubTok(TXmlTok::New("title", Title));
        HitTok->AddSubTok(TXmlTok::New("snipet", Snipet));
        PXmlTok KeyWdTok = TXmlTok::New("keywords");
        const TStrV& KeyWdV = KeyWdVV[DocN]; int GoodKeyWds = 0;
        for (int KeyWdN = 0; KeyWdN < KeyWdV.Len(); KeyWdN++) {
            TStrPr KeyWd(QueryLang, KeyWdV[KeyWdN]);
            //printf("'%s' - '%s'\n", KeyWd.Val1.CStr(), KeyWd.Val2.CStr());
            if (EurovocH.IsKey(KeyWd)) {
                KeyWdTok->AddSubTok(TXmlTok::New("keyword", EurovocH.GetDat(KeyWd)));
                GoodKeyWds++;
            }
        }
        HitTok->AddSubTok(KeyWdTok);
        if (GoodKeyWds == 0) { continue; }
        TopTok->AddSubTok(HitTok);
    }
    return TopTok->GetTokStr();
}

/////////////////////////////////////////////////
// MWdGix-Base
TMWdGixBs::TMWdGixBs(const TStr& _FPath, const TFAccess& _FAccess, const int64& _CacheSize) {
    // store policy
    FPath = _FPath; FAccess = _FAccess; CacheSize = _CacheSize;
    // create blob base
    TStr MWdGixDocBsFNm = TStr::GetNrFPath(FPath) + TGixConst::MWdGixDocBsFNm;
    DocBBs = TMBlobBs::New(MWdGixDocBsFNm, FAccess);
    // load AlignPairBs
    if (FAccess != faCreate) {
        TStr MWdGixBsFNm = TStr::GetNrFPath(FPath) + TGixConst::MWdGixBsFNm;
        AlignPairBs = TAlignPairBs::LoadBin(MWdGixBsFNm);
        InitGixs(FAccess);
    }
}

TMWdGixBs::~TMWdGixBs() {
    if ((FAccess == faCreate) || (FAccess == faUpdate)) {
        TStr MWdGixBsFNm = TStr::GetNrFPath(FPath) + TGixConst::MWdGixBsFNm;
        AlignPairBs->SaveBin(MWdGixBsFNm);
    }
}

void TMWdGixBs::AddDoc(const TStr& DocTitle, const TStr& DocStr,
        const TStr& DocLang, const TStrV& KeyWdV) {
    // save to blob base
    TMOut DocMOut;
    DocTitle.Save(DocMOut); DocStr.Save(DocMOut);
    DocLang.Save(DocMOut); KeyWdV.Save(DocMOut);
    TBlobPt DocBlobPt = DocBBs->PutBlob(DocMOut.GetSIn());
    // add to index
    LangMWdGixH.GetDat(DocLang)->AddHtml(DocStr, DocBlobPt);
}

void TMWdGixBs::GetDoc(const TBlobPt& BlobPt, TStr& DocTitle,
        TStr& DocStr, TStr& DocLang, TStrV& KeyWdV) const {

    PSIn SIn = DocBBs->GetBlob(BlobPt);
    DocTitle.Load(*SIn); DocStr.Load(*SIn);
    DocLang.Load(*SIn); KeyWdV.Load(*SIn);
}

PMWdGixRSet TMWdGixBs::SearchDoc(const TStr& QueryStr, const TStr& QueryLang,
        const TStrV& TargetLangV, const int& Docs, const int& Offset,
        PMWdGixRankFun& RankFun) {

    // check if language is good
    if (!AlignPairBs->IsLang(QueryLang)) {
        return TMWdGixRSet::New(QueryStr, "", TStrStrH(), 0, 0); }
    // translate queries to target languages
    const int Queries = TargetLangV.Len();
    printf("  Translationg %d queries ...\n", Queries);
    const int QueryLangId = AlignPairBs->GetLangId(QueryLang);
    TStrStrH LangQueryH;
    TWgtMWdGixIntItemKdV FullWgtLangItemV;
    for (int TargetLangN = 0; TargetLangN < Queries; TargetLangN++) {
        if (!AlignPairBs->IsLang(TargetLangV[TargetLangN])) { continue; }
        // first we translate the query
        const TStr& TargetLang = TargetLangV[TargetLangN];
        const int TargetLangId = AlignPairBs->GetLangId(TargetLang);
        if (TargetLangId == QueryLangId) { continue; }
        TStr TargetQueryStr = AlignPairBs->MapQuery(
            AlignPairMap, QueryStr, QueryLangId, TargetLangId);
        LangQueryH.AddDat(TargetLang, TargetQueryStr);
        printf("  Query: '%s' -> '%s'\n", QueryStr.CStr(), TargetQueryStr.CStr());
        // then we fire it against the inverted index
        printf("  Loading from Gix ...\n");
        TMWdGixItemV LangResItemV;
        PMWdGix LangMWdGix = LangMWdGixH.GetDat(TargetLang);
        TMWdGixDefMerger LangMerger(LangMWdGix, RankFun);
        LangMWdGix->Search(TargetQueryStr, LangResItemV, LangMerger);
        // get max weight
        double MxWgt = 0.0;
        for (int ItemN = 0; ItemN < LangResItemV.Len(); ItemN++) {
            const TMWdGixItem& Item = LangResItemV[ItemN];
            MxWgt = TFlt::GetMx(Item.GetWgt(), MxWgt);
        }
        // reweight so max is 1.0
        printf("  MxWgt: %g\n", MxWgt);
        for (int ItemN = 0; ItemN < LangResItemV.Len(); ItemN++) {
            const TMWdGixItem& Item = LangResItemV[ItemN];
            const double Wgt = MxWgt > 0.0 ? Item.GetWgt() / MxWgt : 0.0;
            TMWdGixIntItemPr LangItemPr(TargetLangId, Item);
            FullWgtLangItemV.Add(TWgtMWdGixIntItemKd(Wgt, LangItemPr));
        }
    }
    // sort the results
    FullWgtLangItemV.Sort(false);
    // select the correct portion of documents
    printf("  Sorting %d hits ...\n", FullWgtLangItemV.Len());
    TWgtMWdGixIntItemKdV WgtLangItemV;
    if (Docs == -1) {
        // return all resuts
        WgtLangItemV = FullWgtLangItemV;
    } else if (FullWgtLangItemV.Len() >= (Docs + Offset)) {
        // get a subset of results from perticular sub-page
        WgtLangItemV = FullWgtLangItemV;
        WgtLangItemV.Sort(false); WgtLangItemV.Trunc(Docs + Offset);
        WgtLangItemV.Sort(true); WgtLangItemV.Trunc(Docs);
    } else if (FullWgtLangItemV.Len() > Offset) {
        // last page ...
        WgtLangItemV = FullWgtLangItemV; WgtLangItemV.Sort(true);
        WgtLangItemV.Trunc(FullWgtLangItemV.Len() - Offset);
    } else {
        // out of range, leave empty
    }
    // sort according to the rank
    WgtLangItemV.Sort(false);
    // feed them to the result set
    printf("  Loading content for %d hits ...\n", WgtLangItemV.Len());
    PMWdGixRSet RSet = TMWdGixRSet::New(QueryStr,
        QueryLang, LangQueryH, FullWgtLangItemV.Len(), Offset);
    for (int ItemN = 0; ItemN < WgtLangItemV.Len(); ItemN++) {
        const TMWdGixIntItemPr& LangItem = WgtLangItemV[ItemN].Dat;
        const TMWdGixItem& Item = LangItem.Val2;
        TBlobPt DocBlobPt = Item.GetBlobPt();
        TStr DocTitle, DocStr, DocLang; TStrV KeyWdV;
        GetDoc(DocBlobPt, DocTitle, DocStr, DocLang, KeyWdV);
        RSet->AddDoc(DocTitle, DocStr, DocLang, KeyWdV);
    }
    printf("  Done\n");
    return RSet;
}

void TMWdGixBs::AddAcquis(const TStr& XmlFNm, const TStr& Lang) {
    PXmlDoc XmlDoc = TXmlDoc::LoadTxt(XmlFNm);
    if (!XmlDoc->IsOk()) { return; }
    PXmlTok TopTok = XmlDoc->GetTok();
    // title & body
    PXmlTok TextTok = TopTok->GetTagTok("text|body");
    if (TextTok.Empty()) { printf("  Bad file '%s'\n", XmlFNm.CStr()); return; }
    TStr DocTitle; TChA DocChA;
    for (int SubTokN = 0; SubTokN < TextTok->GetSubToks(); SubTokN++) {
        PXmlTok SubTok = TextTok->GetSubTok(SubTokN);
        if (!SubTok->IsTag()) { continue; }
        if (SubTok->GetTagNm() == "head") {
            DocTitle = SubTok->GetTokStr(false);
        } else if (SubTok->GetTagNm() == "div") {
            // iterate over paragraphs
            for (int ParaN = 0; ParaN < SubTok->GetSubToks(); ParaN++) {
                if (SubTok->IsTag()) {
                    DocChA += SubTok->GetSubTok(ParaN)->GetTokStr(false);
                    DocChA += '\n';
                }
            }
            DocChA += '\n';
        }
    }
    // keywords
    TStrV KeyWdV;
    PXmlTok KeyWdTok = TopTok->GetTagTok("teiHeader|profileDesc|textClass");
    if (!KeyWdTok.Empty()) {
        for (int SubTokN = 0; SubTokN < KeyWdTok->GetSubToks(); SubTokN++) {
            PXmlTok SubTok = KeyWdTok->GetSubTok(SubTokN);
            if (!SubTok->IsTag()) { continue; }
            if (SubTok->IsArg("scheme")) {
                KeyWdV.Add(SubTok->GetStrArgVal("scheme") + "-" + SubTok->GetTokStr(false));
            }
        }
    }
    // index
    if (!KeyWdV.Empty()) { AddDoc(DocTitle, DocChA, Lang, KeyWdV); }
}

void TMWdGixBs::IndexAcquis(const TStr& InFPath, PAlignPairBs _AlignPairBs,
        const int& MxDocs, const int64& IndexCacheSize) {

    AlignPairBs = _AlignPairBs;
    // iterate over all the lanugages
    int LangId = AlignPairBs->FFirstLangId();
    while (AlignPairBs->FNextLangId(LangId)) {
        const TStr& Lang = AlignPairBs->GetLang(LangId);
        TStr LangFPath = InFPath + "/" + Lang;
        printf("Indexing %s ...\n", LangFPath.CStr());
        // prepare index for this language
        LangMWdGixH.AddDat(Lang) =
                TMWdGix::New(FPath + "/" + Lang, FAccess, IndexCacheSize);
        // iterate over all the files for this language and index them
        TFFile FFile(LangFPath, ".xml", true);
        TStr XmlFNm; int XmlFNms = 0;
        while (FFile.Next(XmlFNm)) {
            if (XmlFNms == MxDocs) { break; }
            if (XmlFNms % 100 == 0) { printf("  %d\r", XmlFNms); }
            try { AddAcquis(XmlFNm, Lang); } catch (...) { }
            XmlFNms++;
        } printf("\n");
        // clear the index to disk
        LangMWdGixH.Clr();
    }
    // fin :-)
    InitGixs(faRdOnly);
}
