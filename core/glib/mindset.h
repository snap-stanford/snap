#include <google.h>

ClassTV(TMindSetCat, TMindSetCatV) //{
public:
    TStr NameStr;
    PBowSpV CatSpV;
public:
    TMindSetCat() {};
    TMindSetCat(const TStr& _NameStr, PBowSpV _CatSpV):
        NameStr(_NameStr), CatSpV(_CatSpV) {};

    PXmlTok GetXmlTok(const int& MyId) const {
        PXmlTok CatTok = TXmlTok::New("category");
        CatTok->AddArg("id", TStr::Fmt("cat%d", MyId));
        CatTok->AddArg("name", NameStr);
        return CatTok;
    }
};

class TMindSetItem {
public:
    TInt BowId;
    TStr TitleStr;
    TStr ContextStr;
    TStr UrlStr;
    TFltV CptWgtV;
public:
    TMindSetItem() {};
    TMindSetItem(const int& I, const TStr& T, const TStr& C, const TStr& U):
        BowId(I), TitleStr(T), ContextStr(C), UrlStr(U) {};

    PXmlTok GetXmlTok(const int& MyId) const {
        PXmlTok ItemTok = TXmlTok::New("item");
        ItemTok->AddArg("id", TStr::Fmt("item%d", MyId));
        ItemTok->AddSubTok(TXmlTok::New("title", TitleStr));
        ItemTok->AddSubTok(TXmlTok::New("snipet", ContextStr));
        ItemTok->AddSubTok(TXmlTok::New("url", UrlStr));
        PXmlTok CatsTok = TXmlTok::New("categories");
        for (int CatN = 0; CatN < CptWgtV.Len(); CatN++) {
            PXmlTok CatTok = TXmlTok::New("category");
            CatTok->AddArg("id", TStr::Fmt("cat%d", CatN));
            CatTok->AddArg("cal", CptWgtV[CatN]);
            CatsTok->AddSubTok(CatTok);
        }   
        ItemTok->AddSubTok(CatsTok);
        return ItemTok;    
    }
};
typedef TKeyDat<TFlt, TMindSetItem> TFltMindSetItemKd;
typedef TVec<TFltMindSetItemKd> TFltMindSetItemKdV;

ClassTP(TMindSet, PMindSet) //{
private:
    PGgRSet GgRSet;
    PBowDocBs BowDocBs;
public:
    TFltMindSetItemKdV ItemV;
    TMindSetCatV CptV;
public:
    TMindSet(const TStr& QueryStr, const int& MxCands, const int& CptN, const int& WdN) {
        printf("entering mindset ...\n");
        // get search results
        PGgRSet RSet = TGg::WebSearch(QueryStr, MxCands, TNotify::StdNotify);
        printf("Number of retrieved documents: %d/%d", RSet->GetHits(), MxCands);
        BowDocBs = RSet->GetBowDocBs();
        TIntV DIdV; BowDocBs->GetAllDIdV(DIdV);
        PBowDocWgtBs BowDocWgtBs = TBowDocWgtBs::New(BowDocBs, bwwtLogDFNrmTFIDF);
        // fill the list with returend results
        ItemV.Gen(RSet->GetHits(), 0);
        for (int HitN = 0; HitN < RSet->GetHits(); HitN++) {
            TStr Title = RSet->GetHitTitleStr(HitN);
            TStr Snipet = RSet->GetHitCtxStr(HitN);
            TStr Url = RSet->GetHitUrlStr(HitN);
            ItemV.Add(TFltMindSetItemKd(0.0, TMindSetItem(HitN, Title, Snipet, Url)));
        }
        // generate ;mindset' categories (KMeans)
        PBowSim BowSim=TBowSim::New(bstCos);
        TBowWordWgtType WordWgtType=bwwtLogDFNrmTFIDF;
        PBowDocPart BowDocPart=TBowClust::GetKMeansPart(TNotify::StdNotify, BowDocBs, 
            BowSim, TRnd(1), CptN, 1, 10, 1, WordWgtType, 0, 0, DIdV);
        IAssert(BowDocPart->GetClusts() == CptN);
        // load categories
        CptV.Clr();
        // google rangking
        CptV.Add(TMindSetCat("GoogleRank",NULL));
        for (int DocC = 0; DocC < ItemV.Len(); DocC++) {
            ItemV[DocC].Dat.CptWgtV.Add(1.0 - (double)DocC / (double)ItemV.Len());
        }
        // mindset ranking
        TIntV AllDIdV, ClustDIdV; BowDocBs->GetAllDIdV(AllDIdV);
        for (int CptC = 0; CptC < CptN; CptC++) {
            PBowSpV CptSpV = BowDocPart->GetClust(CptC)->GetConceptSpV();
            for (int DocC = 0; DocC < ItemV.Len(); DocC++) {
                double SimWgt = BowSim->GetSim(BowDocWgtBs->GetSpV(DocC), CptSpV);
                ItemV[DocC].Dat.CptWgtV.Add(SimWgt);
            }
            BowDocPart->GetClust(CptC)->GetDIdV(ClustDIdV);
            TStr NameStr;
            if (0 < ClustDIdV.Len() && ClustDIdV.Len() < AllDIdV.Len()) {
                PBowSpV SvmSpV = TBowSVMMd::GetKeywords(BowDocBs, 
                    AllDIdV, ClustDIdV, -1, 1.0, 2.0, 2);
                NameStr = SvmSpV->GetStr(BowDocBs, WdN, 1.0, ", ", false);
            } else {
                NameStr = CptSpV->GetStr(BowDocBs, WdN, 1.0, ", ", false);
            }
            CptV.Add(TMindSetCat(NameStr, CptSpV));
        }
        // sort items
        ItemV.Sort(false);
        printf("mindset finished ...\n");
    }

    static PMindSet New(const TStr& QueryStr, const int& MxCands = 100, 
        const int& CptN = 5, const int& WdN = 10) { 
            return new TMindSet(QueryStr, MxCands, CptN, WdN); }

    void Resort(const TFltV& CptWgtV) {
        // calculate new weights from prefered categories
        for (int DocC = 0; DocC < ItemV.Len(); DocC++) {
            double Wgt = 0;
            IAssert(CptWgtV.Len() <= ItemV[DocC].Dat.CptWgtV.Len());
            for (int CptC = 0; CptC < CptWgtV.Len(); CptC++) {
                Wgt += CptWgtV[CptC] * ItemV[DocC].Dat.CptWgtV[CptC];
            }
            ItemV[DocC].Key = Wgt;
        }
        // resort items according to the new weights
        ItemV.Sort(false);
    }

    void SaveXml(TStr& XmlStr) {
        PXmlTok ResultSetBsTok=TXmlTok::New("resultset");
        // write down categories
        PXmlTok CatsTok=TXmlTok::New("categories");
        for (int CptN = 0; CptN < CptV.Len(); CptN++) {
            CatsTok->AddSubTok(CptV[CptN].GetXmlTok(CptN));
        }
        ResultSetBsTok->AddSubTok(CatsTok);
        // write down items
        PXmlTok ItemsTok=TXmlTok::New("items");
        for (int ItemN = 0; ItemN < ItemV.Len(); ItemN++) {
            ItemsTok->AddSubTok(ItemV[ItemN].Dat.GetXmlTok(ItemN));
        }
        ResultSetBsTok->AddSubTok(ItemsTok);
        // finish XmlDoc and save to stream
        PXmlDoc XmlDoc = TXmlDoc::New(ResultSetBsTok);        
        XmlDoc->SaveStr(XmlStr);
    }

    static void SaveEmptyXml(TStr& XmlStr) {
        PXmlTok ResultSetBsTok=TXmlTok::New("resultset");
        PXmlDoc XmlDoc = TXmlDoc::New(ResultSetBsTok);
        XmlDoc->SaveStr(XmlStr);
    }
};