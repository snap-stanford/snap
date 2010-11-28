/////////////////////////////////////////////////
// Name-Entity-Context-Base
TNmEnCtxBs::TNmEnCtxBs(TSIn& SIn) {
    CtxPtV.Load(SIn);
    CtxPool = TStrPool::Load(SIn);
    CtxDateH.Load(SIn);
}

void TNmEnCtxBs::Save(TSOut& SOut) const {
    CtxPtV.Save(SOut);
    CtxPool->Save(SOut);
    CtxDateH.Save(SOut);
}

int TNmEnCtxBs::AddCtx(const TStr& CtxStr, const TTm& CtxDate) {
    const uint CtxPt = CtxPool->AddStr(CtxStr);
    const int CtxId = CtxPtV.Add(CtxPt);
    CtxDateH.AddDat(CtxId) = CtxDate;
    return CtxId;
}

/////////////////////////////////////////////////
// Name-Entity Base
void TNmEnBs::GetMxNmEnV(const int& MxNmEns,
        TIntV& NmEnIdV, const TIntH& TypeH) const {

    // sort entitites by count
    TIntPrV CountNmV;
    int NmEnKeyId = NmEnCtxIdVH.FFirstKeyId();
    while (NmEnCtxIdVH.FNextKeyId(NmEnKeyId)) {
        const int NmEnId = NmEnCtxIdVH.GetKey(NmEnKeyId);
        const int TypeInt = int(GetNmEnType(NmEnId));
        if (!TypeH.Empty() && !TypeH.IsKey(TypeInt)) { continue; }
        const int Count = GetNmEnCount(NmEnId);
        CountNmV.Add(TIntPr(Count, NmEnId));
    }
    CountNmV.Sort(false); NmEnIdV.Gen(MxNmEns, 0);
    for (int CountNmN = 0; CountNmN < CountNmV.Len(); CountNmN++) {
        if (CountNmN >= MxNmEns) { break; }
        NmEnIdV.Add(CountNmV[CountNmN].Val2);
    }
}

void TNmEnBs::GetMxNmEnH(const int& MxNmEns, TIntH& NmEnIdH) const {
    TIntV NmEnIdV; GetMxNmEnV(MxNmEns, NmEnIdV, TIntH()); NmEnIdH.Clr();
    for (int NmEnIdN = 0; NmEnIdN < NmEnIdV.Len(); NmEnIdN++) {
        printf("%d/%d\r", NmEnIdN, NmEnIdV.Len());
        const int NmEnId = NmEnIdV[NmEnIdN];
        NmEnIdH.AddKey(NmEnId);
    }
}

TStr TNmEnBs::GetClustNm(PBowDocPartClust Clust, PBowDocBs BowDocBs,
        PBowDocWgtBs BowDocWgtBs, const int& TopNmEns) const {
    Fail; return "";
  /*
    // get top most similar documents to the centroid
    PBowSim BowSim = TBowSim::New(bstCos);
    PBowSpV ClustSpV = Clust->GetConceptSpV();
    TFltIntKdHp TopNmEnWgtIdHp(htMin, TopNmEns);
    const int ClustDIds = Clust->GetDocs();
    for (int ClustDIdN = 0; ClustDIdN < ClustDIds; ClustDIdN++) {
        const int DId = Clust->GetDId(ClustDIdN);
        PBowSpV DocSpV = BowDocWgtBs->GetSpV(DId);
        const double DocSim = BowSim->GetSim(ClustSpV, DocSpV);
        TopNmEnWgtIdHp.Add(TFltIntKd(DocSim, DId));
    }
    // generate the cluster name from the top documetn names
    TFltIntKdV TopDIdV; TopNmEnWgtIdHp.GetV(TopDIdV); TopDIdV.Sort(false);
    TChA ClustNm;
    for (int TopDIdN = 0; TopDIdN < TopDIdV.Len(); TopDIdN++) {
        if (TopDIdN > 0) { ClustNm += ", "; }
        TStr DocNm = BowDocBs->GetDocNm(TopDIdV[TopDIdN].Dat);
        ClustNm += DocNm;
    }
    return ClustNm;*/
}

TStr TNmEnBs::GenNmEnCtx(const TStr& NmEnStr, const TStr& ParStr) const {
    // split the ParStr to text before and text after the nm-entity
    int NmEnPos = ParStr.SearchStr(NmEnStr);
    TStr LeftStr = ParStr.Left(NmEnPos);
    TStr RightStr = ParStr.Right(ParStr.Len() - NmEnPos - NmEnStr.Len());
    const int WndWords = 3;
    // get the last three words in the left window
    TStrV LeftWndV(WndWords); LeftWndV.PutAll("");
    TMem LeftMem(LeftStr); THtmlLx LeftLx(LeftMem.GetSIn(), false);
    LeftLx.GetSym(); int WordCount = 0;
    while (LeftLx.Sym != hsyEof) {
        if (LeftLx.Sym == hsyStr) {
            LeftWndV[WordCount % WndWords] = LeftLx.ChA();
            WordCount++;
        }
        LeftLx.GetSym();
    }
    // get the first three words in the left window
    TStrV RightWndV(WndWords, 0); TMem RightMem(RightStr);
    THtmlLx RightLx(RightMem.GetSIn(), false); RightLx.GetSym();
    while ((RightLx.Sym != hsyEof) && (RightWndV.Len() < WndWords)) {
        if (RightLx.Sym == hsyStr) { RightWndV.Add(RightLx.ChA()); }
        RightLx.GetSym();
    }
    // generate the context
    TChA CtxChA;
    for (int LeftWndN = 0; LeftWndN < LeftWndV.Len(); LeftWndN++) {
        CtxChA += LeftWndV[LeftWndN]; CtxChA += " "; }
    CtxChA += "NM_EN"; CtxChA += " ";
    for (int RightWndN = 0; RightWndN < RightWndV.Len(); RightWndN++) {
        CtxChA += RightWndV[RightWndN]; CtxChA += " "; }
    return CtxChA;
}

TStr TNmEnBs::GenNmEnPrCtx(const TStr& NmEn1Str,
        const TStr& NmEn2Str, const TStr& ParStr) const {

    // extract the text between name entitites
    int NmEn1Pos = ParStr.SearchStr(NmEn1Str);
    int NmEn2Pos = ParStr.SearchStr(NmEn2Str);
    TStr MiddleStr;
    if (NmEn1Pos < NmEn2Pos) {
        MiddleStr = ParStr.GetSubStr(NmEn1Pos + NmEn1Str.Len(), NmEn2Pos-1);
    } else {
        MiddleStr = ParStr.GetSubStr(NmEn2Pos + NmEn2Str.Len(), NmEn1Pos-1);
    }
    return "NM_EN " + MiddleStr + " NM_EN";
}

TNmEnBs::TNmEnBs() {
    NmEnCtxBs = TNmEnCtxBs::New();
    NmEnCtxBow = TBowDocBs::New();
    NmEnCtxBoww = TBowDocWgtBs::New(NmEnCtxBow->GetSig());
    NmEnCooCtxBow = TBowDocBs::New();
    NmEnCooCtxBoww = TBowDocWgtBs::New(NmEnCooCtxBow->GetSig());
}

TNmEnBs::TNmEnBs(TSIn& SIn, const bool& LoadCtxP) {
    TypeStrToIntH.Load(SIn); NmEnStrH.Load(SIn);
    NmEnTypeHH.Load(SIn); NmEnRelNmEnHH.Load(SIn);
    if (LoadCtxP) {
        NmEnCtxBs = TNmEnCtxBs::Load(SIn);
        NmEnCtxIdVH.Load(SIn); NmEnPrCtxIdVH.Load(SIn);
        NmEnToDIdH.Load(SIn); NmEnCtxBow = TBowDocBs::Load(SIn);
        NmEnCtxBoww = TBowDocWgtBs::Load(SIn);
        NmEnToDIdVH.Load(SIn); NmEnCooCtxBow = TBowDocBs::Load(SIn);
        NmEnCooCtxBoww = TBowDocWgtBs::Load(SIn);
    }
}

TNmEnBs::TNmEnBs(TSIn& SIn, PBowDocBs _NmEnCtxBow) {
    TypeStrToIntH.Load(SIn); NmEnStrH.Load(SIn);
    NmEnTypeHH.Load(SIn); NmEnRelNmEnHH.Load(SIn);
    NmEnCtxBs = TNmEnCtxBs::Load(SIn);
    NmEnCtxIdVH.Load(SIn); NmEnPrCtxIdVH.Load(SIn);
    NmEnToDIdH.Load(SIn);
    {PBowDocBs LoadNmEnCtxBow = TBowDocBs::Load(SIn);
    EAssert(_NmEnCtxBow->GetSig() == LoadNmEnCtxBow->GetSig());
    NmEnCtxBow = _NmEnCtxBow;}
    NmEnCtxBoww = TBowDocWgtBs::Load(SIn);
    NmEnToDIdVH.Load(SIn); NmEnCooCtxBow = TBowDocBs::Load(SIn);
    NmEnCooCtxBoww = TBowDocWgtBs::Load(SIn);
}

void TNmEnBs::Save(TSOut& SOut) const {
    TypeStrToIntH.Save(SOut); NmEnStrH.Save(SOut);
    NmEnTypeHH.Save(SOut); NmEnRelNmEnHH.Save(SOut);
    NmEnCtxBs->Save(SOut); NmEnCtxIdVH.Save(SOut); NmEnPrCtxIdVH.Save(SOut);
    NmEnToDIdH.Save(SOut); NmEnCtxBow->Save(SOut); NmEnCtxBoww->Save(SOut);
    NmEnToDIdVH.Save(SOut); NmEnCooCtxBow->Save(SOut); NmEnCooCtxBoww->Save(SOut);
}

TNmEnType TNmEnBs::GetType(const TStr& TypeStr) {
    // load if not loaded already
    if (TypeStrToIntH.Empty()) {
        TypeStrToIntH.AddDat("organization", int(netOrg));
        TypeStrToIntH.AddDat("location", int(netLocation));
        TypeStrToIntH.AddDat("person", int(netPerson));
    }
    // convert string to type using the hash table
    if (TypeStrToIntH.IsKey(TypeStr)) {
        return TNmEnType(TypeStrToIntH.GetDat(TypeStr).Val);
    } else {
        return netUndef;
    }
}

TStr TNmEnBs::GetTypeStr(const TNmEnType& TypeInt) {
    // load if not loaded already
    if (TypeIntToStrH.Empty()) {
        TypeIntToStrH.AddDat(int(netOrg), "organization");
        TypeIntToStrH.AddDat(int(netLocation), "location");
        TypeIntToStrH.AddDat(int(netPerson), "person");
    }
    // convert string to type using the hash table
    if (TypeIntToStrH.IsKey(TypeInt)) {
        return TypeIntToStrH.GetDat(TypeInt);
    } else {
        return "";
    }
}

TNmEnType TNmEnBs::GetNmEnType(const int& NmEnId) const {
    const TIntH& TypeH = NmEnTypeHH.GetDat(NmEnId);
    int MxType = -1, MxVal = -1, KeyId = TypeH.FFirstKeyId();
    while (TypeH.FNextKeyId(KeyId)) {
        if (TypeH[KeyId] > MxVal) {
            MxType = TypeH.GetKey(KeyId);
            MxVal = TypeH[KeyId];
        }
    }
    return (MxType != -1) ? TNmEnType(MxType) : netUndef;
}

int TNmEnBs::AddNmEn(const TStr& NmEnStr, const TNmEnType& NmEnType) {
    const int NmEnId = NmEnStrH.AddKey(NmEnStr);
    NmEnTypeHH.AddDat(NmEnId).AddDat(NmEnType, 1);
    return NmEnId;
}

void TNmEnBs::GetRelated(const int& NmEnId, TIntPrV& RelNmEnIdWgtV,
        const int& MxNmEns, const bool& SortP) {

    // load and count related entities
    const TIntH& RelNmEnIdWgtH = NmEnRelNmEnHH.GetDat(NmEnId);
    RelNmEnIdWgtH.GetKeyDatPrV(RelNmEnIdWgtV);
    // sort by count
    if (SortP) {
        TIntPrV RelNmEnWgtIdV;
        GetSwitchedPrV<TInt, TInt>(RelNmEnIdWgtV, RelNmEnWgtIdV);
        RelNmEnWgtIdV.Sort(false);
        GetSwitchedPrV<TInt, TInt>(RelNmEnWgtIdV, RelNmEnIdWgtV);
    }
    // keep only the top
    if (MxNmEns != -1 && RelNmEnIdWgtV.Len() > MxNmEns) {
        RelNmEnIdWgtV.Trunc(MxNmEns);
    }
}

void TNmEnBs::PrintRelated(const TStr& NmEnStr, const int& MxNmEns) {
    // get relations
    const int NmEnId = GetNmEnId(NmEnStr);
    TIntPrV RelNmEnIdWgtV;
    GetRelated(NmEnId, RelNmEnIdWgtV, MxNmEns, true);
    // print
    printf("[%s]\n", NmEnStr.CStr());
    for (int RelNmEnN = 0; RelNmEnN < RelNmEnIdWgtV.Len(); RelNmEnN++) {
        const int RelNmEnId = RelNmEnIdWgtV[RelNmEnN].Val1;
        TStr RelNmEnStr = GetNmEnStr(RelNmEnId);
        const int RelNmEnWgt = RelNmEnIdWgtV[RelNmEnN].Val2;
        TStr DocNm = GetNmEnPrStr(NmEnId, RelNmEnId);
        if (NmEnCooCtxBow->IsDocNm(DocNm)) {
            const int DId = NmEnCooCtxBow->GetDId(DocNm);
            PBowSpV DocSpV = NmEnCooCtxBoww->GetSpV(DId);
            PBowKWordSet KWordSet = DocSpV->GetKWordSet(NmEnCooCtxBow)->GetTopKWords(5, 1.0);
            printf(" => %s [%d] (%s)\n", RelNmEnStr.CStr(),
                RelNmEnWgt, KWordSet->GetKWordsStr().ToLc().CStr());
        } else {
            printf(" => %s [%d] (missing ...)\n", RelNmEnStr.CStr(), RelNmEnWgt);
        }
    }
}

void TNmEnBs::GetRelated(const TIntV& ClustDIdV1, const TIntV& ClustDIdV2,
        TIntV& CooCtxDIdV, TIntPrV& NmEnWgtV) {

    // load documents from each cluster into a hash map
    TIntH Clust1DIdH, Clust2DIdH;
    for (int ClustDIdN = 0; ClustDIdN < ClustDIdV1.Len(); ClustDIdN++) {
        Clust1DIdH.AddKey(ClustDIdV1[ClustDIdN]); }
    for (int ClustDIdN = 0; ClustDIdN < ClustDIdV2.Len(); ClustDIdN++) {
        Clust2DIdH.AddKey(ClustDIdV2[ClustDIdN]); }
    // iterate over all the pairs and keep the ones that relate the clsuteres
    CooCtxDIdV.Clr(); TIntH NmEnWgtH;
    int NmEnPrKeyId = NmEnPrCtxIdVH.FFirstKeyId();
    while (NmEnPrCtxIdVH.FNextKeyId(NmEnPrKeyId)) {
        // get ids of entitiy pair
        const int NmEnId1 = NmEnPrCtxIdVH.GetKey(NmEnPrKeyId).Val1;
        const int NmEnId2 = NmEnPrCtxIdVH.GetKey(NmEnPrKeyId).Val2;
        if (!IsNmEnDId(NmEnId1) || !IsNmEnDId(NmEnId2)) { continue; }
        const int NmEnDId1 = GetNmEnDId(NmEnId1);
        const int NmEnDId2 = GetNmEnDId(NmEnId2);
        // check if the documents are relating clusters
        if ((Clust1DIdH.IsKey(NmEnDId1) && Clust2DIdH.IsKey(NmEnDId2)) ||
            (Clust2DIdH.IsKey(NmEnDId2) && Clust1DIdH.IsKey(NmEnDId1))) {

            TStr DocNm = GetNmEnPrStr(NmEnId1, NmEnId2);
            if (NmEnCooCtxBow->IsDocNm(DocNm)) {
                const int CooCtxDId = NmEnCooCtxBow->GetDId(DocNm);
                CooCtxDIdV.AddMerged(CooCtxDId);
                NmEnWgtH.AddDat(NmEnId1)++;
                NmEnWgtH.AddDat(NmEnId2)++;
            }
        }
    }
    // sort the name entities in the intersection
    TIntPrV WgtNmEnV; NmEnWgtH.GetDatKeyPrV(WgtNmEnV);
    WgtNmEnV.Sort(false); GetSwitchedPrV<TInt, TInt>(WgtNmEnV, NmEnWgtV);
}

void TNmEnBs::GetRelated(const TIntV& ClustDIdV1, const TIntV& ClustDIdV2,
        TStr& RelNm, int& RelPrs, PBowSpV& RelSpV) {

    // get relation data
    TIntV CooCtxDIdV; TIntPrV NmEnWgtV;
    GetRelated(ClustDIdV1, ClustDIdV2, CooCtxDIdV, NmEnWgtV);
    // process the data to pass it back
    RelPrs = CooCtxDIdV.Len();
    PBowSim BowSim = TBowSim::New(bstCos);
    RelSpV = TBowClust::GetConceptSpV(
        NmEnCooCtxBoww, BowSim, CooCtxDIdV, 1.0);
    // generate relation name from most active name entities
    const int MxNmEns = TInt::GetMn(10, NmEnWgtV.Len());
    for (int NmEnN = 0; NmEnN < MxNmEns; NmEnN++) {
        if (NmEnN > 0) { RelNm += ", "; }
        RelNm += GetNmEnStr(NmEnWgtV[NmEnN].Val1);
    }
}

void TNmEnBs::GetRelated(const TIntV& ClustDIdV1, const TIntV& ClustDIdV2,
        const int& MxKeyWds, TStr& RelNm, int& RelPrs, TStr& RelKeyWdStr) {

    PBowSpV RelSpV; GetRelated(ClustDIdV1, ClustDIdV2, RelNm, RelPrs, RelSpV);
    PBowKWordSet KWordSet = RelSpV->GetKWordSet(NmEnCooCtxBow)->GetTopKWords(MxKeyWds, 1.0);
    RelKeyWdStr = KWordSet->GetKWordsStr().ToLc();
}

void TNmEnBs::AddNmEnCtx(const int& NmEnId, const int& CtxId) {
    NmEnCtxIdVH.AddDat(NmEnId).AddMerged(CtxId);
}

void TNmEnBs::AddNmEnPrCtx(const int& NmEnId1, const int& NmEnId2, const int& CtxId) {
    NmEnPrCtxIdVH.AddDat(GetNmEnPr(NmEnId1, NmEnId2)).AddMerged(CtxId);
}

void TNmEnBs::GenNmEnCtxBow(const int& MxNmEns, const TIntH& TypeH) {
    PSwSet SwSet = TSwSet::New(swstEn523); SwSet->AddWord("NM_EN");
    PStemmer Stemmer = TStemmer::New(stmtPorter, true);
    NmEnCtxBow = TBowDocBs::New(SwSet, Stemmer, NULL);
    // iterate over all the name entities and get it's contexts
    printf("Loading profiles ...\n");
    TIntV NmEnIdV; GetMxNmEnV(MxNmEns, NmEnIdV, TypeH); NmEnToDIdH.Clr();
    for (int NmEnIdN = 0; NmEnIdN < NmEnIdV.Len(); NmEnIdN++) {
        if (NmEnIdN % 100 == 0) { printf("%d/%d\r", NmEnIdN, NmEnIdV.Len()); }
        const int NmEnId = NmEnIdV[NmEnIdN];
        TStr NmEnStr = NmEnStrH.GetKey(NmEnId);
        const TIntV& CtxIdV = GetCtxIdV(NmEnId);
        TChA NmEnFullCtx;
        for (int CtxIdN = 0; CtxIdN < CtxIdV.Len(); CtxIdN++) {
            TStr CtxStr = GetCtxStr(CtxIdV[CtxIdN]);
            NmEnFullCtx += CtxStr; NmEnFullCtx += '\n';
        }
        TStr CatNm = GetTypeStr(GetNmEnType(NmEnId));
        const int DId = NmEnCtxBow->AddHtmlDoc(NmEnStr,
            TStrV::GetV(CatNm), TStr(NmEnFullCtx), true);
        NmEnToDIdH.AddDat(NmEnId, DId);
    } printf("\n");
    NmEnCtxBoww = TBowDocWgtBs::New(NmEnCtxBow, bwwtLogDFNrmTFIDF);
}

void TNmEnBs::GenNmEnCooCtxBow(const int& MxNmEns) {
    PSwSet SwSet = TSwSet::New(swstEn523);
    PStemmer Stemmer = TStemmer::New(stmtPorter, true);
    NmEnCooCtxBow = TBowDocBs::New(SwSet, Stemmer, NULL);
    // iterate over all the name entities and get it's contexts
    int NmEnPrKeyId = NmEnPrCtxIdVH.FFirstKeyId();
    printf("Loading co-profiles ...\n");
    TIntH NmEnIdH; GetMxNmEnH(MxNmEns, NmEnIdH); NmEnToDIdVH.Clr();
    while (NmEnPrCtxIdVH.FNextKeyId(NmEnPrKeyId)) {
        if (NmEnPrKeyId % 1000 == 0) {
            printf("%d/%d\r ", NmEnPrKeyId, NmEnPrCtxIdVH.Len());
        }
        // get ids of entitiy pair
        const int NmEnId1 = NmEnPrCtxIdVH.GetKey(NmEnPrKeyId).Val1;
        if (!NmEnIdH.IsKey(NmEnId1)) { continue; }
        const int NmEnId2 = NmEnPrCtxIdVH.GetKey(NmEnPrKeyId).Val2;
        if (!NmEnIdH.IsKey(NmEnId2)) { continue; }
        // get the contexts of this entitites
        const TIntV& CtxIdV = GetPrCtxIdV(NmEnId1, NmEnId2);
        TChA NmEnFullCtx;
        for (int CtxIdN = 0; CtxIdN < CtxIdV.Len(); CtxIdN++) {
            TStr CtxStr = GetCtxStr(CtxIdV[CtxIdN]);
            NmEnFullCtx += CtxStr; NmEnFullCtx += '\n';
        }
        //if (GetNmEnStr(NmEnId1) == "Agassi" || GetNmEnStr(NmEnId2) == "Agassi") {
        //    printf("%s - %s \n%s\n", GetNmEnStr(NmEnId1).CStr(),
        //        GetNmEnStr(NmEnId2).CStr(), NmEnFullCtx.CStr());
        //}
        // add the document to the bow
        const int DId = NmEnCooCtxBow->AddHtmlDoc(
            GetNmEnPrStr(NmEnId1, NmEnId2),
            TStrV(), TStr(NmEnFullCtx), false);
        // make a map from the entities to this document
        NmEnToDIdVH.AddDat(NmEnId1).Add(DId);
        NmEnToDIdVH.AddDat(NmEnId2).Add(DId);
    } printf("\n");
    NmEnCooCtxBoww = TBowDocWgtBs::New(NmEnCooCtxBow, bwwtLogDFNrmTFIDF);
}

PBowDocPart TNmEnBs::GenBowDocPart(const int& Clusts) {
    PBowSim BowSim = TBowSim::New(bstCos);
    TRnd Rnd(1);
    PBowDocPart BowDocPart = TBowClust::GetKMeansPartForDocWgtBs(
        TNotify::StdNotify, NmEnCtxBoww, NmEnCtxBow,
        BowSim, Rnd, Clusts, 1, 10, 1);
    return BowDocPart;
}

void TNmEnBs::PrintClustSim(const TStr& FNm, const int& Clusts) {
    TFOut FOut(FNm);
    // make clustering
    printf("clustering...\n");
    PBowDocPart BowDocPart = GenBowDocPart(Clusts);
    // output clusters
    FOut.PutStrLn("Clusters:"); printf("outputing clusters...\n");
    for (int ClustN = 0; ClustN < BowDocPart->GetClusts(); ClustN++) {
        printf("%d\r", ClustN+1);
        TStr ClustNm = GetClustNm(BowDocPart->GetClust(ClustN), NmEnCtxBow, NmEnCtxBoww, 10);
        PBowSpV ClustSpV = BowDocPart->GetClust(ClustN)->GetConceptSpV();
        TStr ClustStr = ClustSpV->GetKWordSet(NmEnCtxBow)->GetTopKWords(10, 1.0)->GetKWordsStr();
        FOut.PutStrLn(TStr::Fmt(" [%2d] ClustNm: %s", ClustN+1, ClustNm.CStr()));
        FOut.PutStrLn("      Keywords: " + ClustStr.ToLc());
    } printf("\n");
    FOut.PutLn(); FOut.PutStrLn("Relations:"); printf("outputing relations...\n");
    // sort relations
    TIntIntPrPrV RelWgtPrV; THash<TIntPr, PBowSpV> RelToBowSpVH; TIntPrStrH RelToNmH;
    for (int ClustN = 0; ClustN < BowDocPart->GetClusts(); ClustN++) {
        for (int RelClustN = ClustN+1; RelClustN < BowDocPart->GetClusts(); RelClustN++) {
            TStr RelNm; int RelPrs = 0; PBowSpV RelSpV;
            GetRelated(BowDocPart->GetClust(ClustN)->GetDIdVRef(),
                BowDocPart->GetClust(RelClustN)->GetDIdVRef(),
                RelNm, RelPrs, RelSpV);
            RelWgtPrV.Add(TIntIntPrPr(RelPrs, TIntPr(ClustN, RelClustN)));
            RelToBowSpVH.AddDat(TIntPr(ClustN, RelClustN), RelSpV);
            RelToNmH.AddDat(TIntPr(ClustN, RelClustN), RelNm);
        }
    }
    RelWgtPrV.Sort(false);
    // output relations
    for (int RelWgtPrN = 0; RelWgtPrN < RelWgtPrV.Len(); RelWgtPrN++) {
        // get data
        const int RelPrs = RelWgtPrV[RelWgtPrN].Val1;
        const TIntPr ClustPr = RelWgtPrV[RelWgtPrN].Val2;
        PBowSpV RelSpV = RelToBowSpVH.GetDat(ClustPr);
        TStr RelNm = RelToNmH.GetDat(ClustPr);
        const int ClustN = ClustPr.Val1;
        const int RelClustN = ClustPr.Val2;
        // print data
        printf("%2d-%2d\r", ClustN+1, RelClustN+1);
        TStr RelStr = RelSpV->GetKWordSet(
            NmEnCooCtxBow)->GetTopKWords(10, 1.0)->GetKWordsStr();
        FOut.PutStrLn(TStr::Fmt(" [%4d: %2d-%2d] %s", RelPrs,
            ClustN+1, RelClustN+1, RelNm.CStr()));
        FOut.PutStrLn("       Keywords: " + RelStr.ToLc());
    } printf("\n");
    printf("done\n");
}

void TNmEnBs::AddReutersXml(const TStr& XmlFNm, const bool& GenCooCtxP) {
    PXmlDoc Doc=TXmlDoc::LoadTxt(XmlFNm);
    // get text strings
    // general document data
    TStr DateStr=Doc->GetTagTok("newsitem")->GetArgVal("date");
    TStr TitleStr=Doc->GetTagTok("newsitem|title")->GetTokStr(false);
    TStr HeadlineStr=Doc->GetTagTok("newsitem|headline")->GetTokStr(false);
    // parse date
    TTm DateTm = TTm::GetTmFromWebLogDateTimeStr(DateStr, '-');
    // text paragraphs
    TXmlTokV ParTokV; Doc->GetTagTokV("newsitem|text|p", ParTokV);
    for (int ParTokN = 0; ParTokN < ParTokV.Len(); ParTokN++){
        TStr ParStr = ParTokV[ParTokN]->GetTokStr(false);
        TXmlTokV NmEnTokV; ParTokV[ParTokN]->GetTagTokV("enamex", NmEnTokV);
        // check for new name entities
        TIntV NmEnIdV;
        for (int NmEnTokN = 0; NmEnTokN < NmEnTokV.Len(); NmEnTokN++) {
            TStr NmEnTypeStr = NmEnTokV[NmEnTokN]->GetArgVal("type", "");
            TStr NmEnStr = NmEnTokV[NmEnTokN]->GetTokStr(false);
            TNmEnType NmEnType = GetType(NmEnTypeStr);
            if (NmEnType == netUndef) { printf("Unknown: %s", NmEnTypeStr.CStr()); continue; }
            if (IsNmEn(NmEnStr)) { // old name entity
                const int NmEnId = GetNmEnId(NmEnStr);
                IAssert(GetNmEnStr(NmEnId) == NmEnStr);
                NmEnTypeHH.AddDat(NmEnId).AddDat(NmEnType)++;
                NmEnIdV.AddMerged(NmEnId);
            } else {
                const int NmEnId = AddNmEn(NmEnStr, NmEnType);
                NmEnIdV.AddMerged(NmEnId);
            }
        }
        if (NmEnIdV.Empty()) { continue; }
        // load contexts
        for (int NmEn1IdN = 0; NmEn1IdN < NmEnIdV.Len(); NmEn1IdN++) {
            // add single context
            const int NmEn1Id = NmEnIdV[NmEn1IdN];
            TStr NmEn1Str = GetNmEnStr(NmEn1Id);
            TStr NmEn1CtxStr = GenNmEnCtx(NmEn1Str, ParStr);
            const int NmEn1CtxId = NmEnCtxBs->AddCtx(NmEn1CtxStr, DateTm);
            AddNmEnCtx(NmEn1Id, NmEn1CtxId);
            // add co-occurences contexts
            for (int NmEn2IdN = 0; NmEn2IdN < NmEnIdV.Len(); NmEn2IdN++) {
                const int NmEn2Id = NmEnIdV[NmEn2IdN];
                if (NmEn1Id == NmEn2Id) { continue; }
                // count co-occurrence
                AddRelNmEn(NmEn1Id, NmEn2Id);
                if (GenCooCtxP) {
                    // generate context of co-occurrence and store it
                    TStr NmEn2Str = GetNmEnStr(NmEn2Id);
                    TStr NmEnPrCtxStr = GenNmEnPrCtx(NmEn1Str, NmEn2Str, ParStr);
                    const int NmEnPrCtxId = NmEnCtxBs->AddCtx(NmEnPrCtxStr, DateTm);
                    AddNmEnPrCtx(NmEn1Id, NmEn2Id, NmEnPrCtxId);
                }
            }
        }
    }
}

PNmEnBs TNmEnBs::LoadReuters(const TStr& FPath, const TTm& StartDate,
        const TTm& EndDate, const bool& GenCooCtxP) {

    PNotify Notify = TStdNotify::New();
    Notify->OnStatus("Loading Reuters documents from file-path " + FPath + " ...");
    PNmEnBs NmEnBs = TNmEnBs::New();
    TFFile FFile(FPath, ".xml", true);
    TStr FNm; int Docs = 0, Files = 0;
    while (FFile.Next(FNm)) {
        if (Files > 100000) { break; }
        // load document
        if (TFile::Exists(FNm)) {
            TStr DateStr, RestStr;
            FNm.GetFBase().SplitOnCh(DateStr, '_', RestStr);
            TTm DateTm(DateStr.GetSubStr(0, 3).GetInt(),
                DateStr.GetSubStr(4, 5).GetInt(),
                DateStr.GetSubStr(6, 7).GetInt());
            if (StartDate <= DateTm && DateTm <= EndDate) {
                NmEnBs->AddReutersXml(FNm, GenCooCtxP); Docs++;
            }
            Files++;
        }
        // print statistics
        if (Files % 1000 == 0) {
            const int NmEns = NmEnBs->GetNmEns();
            const int Ctxs = NmEnBs->GetCtxs();
            Notify->OnStatus(TStr::Fmt("F:%d / D:%d / NE:%d/ C:%d\r", Files, Docs, NmEns, Ctxs));
        }
    }
    const int NmEns = NmEnBs->GetNmEns(), Ctxs = NmEnBs->GetCtxs();
    Notify->OnStatus(TStr::Fmt("F:%d / D:%d / NE:%d/ C:%d", Files, Docs, NmEns, Ctxs));
    return NmEnBs;
}

PNmEnBs TNmEnBs::LoadMsrSent(const TStr& InFNm) {
    PNmEnBs NmEnBs = TNmEnBs::New();
    TLnRet LnRet(TFIn::New(InFNm));
    TStr LnStr; TStrV PartV;
    while (LnRet.NextLn(LnStr)) {
        // parse the line
        LnStr.ToLc(); LnStr.SplitOnAllCh(' ', PartV);
        // load name entitites
        TIntV NmEnIdV;
        for (int PartN = 0; PartN < PartV.Len(); PartN++) {
            if (PartV[PartN].IsChIn('_')) {
                TStr NmEnStr = PartV[PartN];
                if (NmEnBs->IsNmEn(NmEnStr)) { // old name entity
                    const int NmEnId = NmEnBs->GetNmEnId(NmEnStr);
                    NmEnIdV.AddMerged(NmEnId);
                } else {
                    const int NmEnId = NmEnBs->AddNmEn(NmEnStr, netUndef);
                    NmEnIdV.AddMerged(NmEnId);
                }
                while (LnStr.DelStr(NmEnStr)) { }
            }
        }
        if (NmEnIdV.Empty()) { continue; }
        // load contexts
        const int CtxId = NmEnBs->NmEnCtxBs->AddCtx(LnStr, TTm());
        for (int NmEnIdN1 = 0; NmEnIdN1 < NmEnIdV.Len(); NmEnIdN1++) {
            // add single context
            const int NmEnId1 = NmEnIdV[NmEnIdN1];
            NmEnBs->AddNmEnCtx(NmEnId1, CtxId);
            // add co-occurences contexts
            for (int NmEnIdN2 = 0; NmEnIdN2 < NmEnIdV.Len(); NmEnIdN2++) {
                const int NmEnId2 = NmEnIdV[NmEnIdN2];
                if (NmEnId1 == NmEnId2) { continue; }
                NmEnBs->AddRelNmEn(NmEnId1, NmEnId2);
                NmEnBs->AddNmEnPrCtx(NmEnId1, NmEnId2, CtxId);
            }
        }
    }
    return NmEnBs;
}

void TNmEnBs::SaveNmEnStat(const TStr& FNm) const {
    TFOut FOut(FNm);
    // sort entitites by count
    TIntStrPrV CountNmV;
    int NmEnKeyId = NmEnCtxIdVH.FFirstKeyId();
    while (NmEnCtxIdVH.FNextKeyId(NmEnKeyId)) {
        const int NmEnId = NmEnCtxIdVH.GetKey(NmEnKeyId);
        TStr NmEnStr = GetNmEnStr(NmEnId);
        const int Count = GetNmEnCount(NmEnId);
        CountNmV.Add(TIntStrPr(Count, NmEnStr));
    }
    CountNmV.Sort(false);
    FOut.PutStrLn("Name entities:");
    for (int CountNmN = 0; CountNmN < CountNmV.Len(); CountNmN++) {
        FOut.PutStrLn(TStr::Fmt("%d : %s",
            CountNmV[CountNmN].Val1.Val,
            CountNmV[CountNmN].Val2.CStr()));
    }
    FOut.Flush();
}

void TNmEnBs::SaveNmEnCtxStat(const TStr& FNm) const {
    TIntV NmEnIdV; GetMxNmEnV(50000, NmEnIdV, TIntH());
    TFOut FOut(FNm); TRnd Rnd(1); //NmEnIdV.Shuffle(Rnd);
    if (NmEnIdV.Len() > 100) { NmEnIdV.Trunc(100); }
    for (int NmEnIdN = 0; NmEnIdN < NmEnIdV.Len(); NmEnIdN++) {
        const int NmEnId = NmEnIdV[NmEnIdN];
        TStr NmEnStr = GetNmEnStr(NmEnId);
        FOut.PutStrLn(NmEnStr);
        TIntV CtxIdV = GetCtxIdV(NmEnId); CtxIdV.Shuffle(Rnd);
        if (CtxIdV.Len() > 10) { CtxIdV.Trunc(10); }
        for (int CtxIdN = 0; CtxIdN < CtxIdV.Len(); CtxIdN++) {
            const int CtxId = CtxIdV[CtxIdN];
            TStr CtxStr = GetCtxStr(CtxId);
            FOut.PutStrLn("  " + CtxStr);
        }
        FOut.Flush();
    }
}

void TNmEnBs::SaveNmEnPrStat(const TStr& FNm) const {
    TFOut FOut(FNm);
    // sort cooccurences by count
    printf("NmEnPrCtxIdVH.Len() == %d\n", NmEnPrCtxIdVH.Len());
    TIntStrPrV CountNmV;
    int NmEnPrKeyId = NmEnPrCtxIdVH.FFirstKeyId();
    while (NmEnPrCtxIdVH.FNextKeyId(NmEnPrKeyId)) {
        const int NmEnId1 = NmEnPrCtxIdVH.GetKey(NmEnPrKeyId).Val1;
        const int NmEnId2 = NmEnPrCtxIdVH.GetKey(NmEnPrKeyId).Val2;
        const int Count = GetPrCtxIdV(NmEnId1, NmEnId2).Len();
        CountNmV.Add(TIntStrPr(Count, GetNmEnPrStr(NmEnId1, NmEnId2)));
    }
    printf("CountNmV.Len() == %d\n", CountNmV.Len());
    CountNmV.Sort(false);
    FOut.PutLn();
    FOut.PutStrLn("Co-occurance:");
    for (int CountNmN = 0; CountNmN < CountNmV.Len(); CountNmN++) {
        TStr NmEnIdStr1, NmEnIdStr2;
        TStr DocNm = CountNmV[CountNmN].Val2;
        DocNm.SplitOnCh(NmEnIdStr1, '-', NmEnIdStr2);
        const int NmEnId1 = NmEnIdStr1.GetInt();
        const int NmEnId2 = NmEnIdStr2.GetInt();
        TStr NmEnStr1 = NmEnStrH.GetKey(NmEnId1);
        TStr NmEnStr2 = NmEnStrH.GetKey(NmEnId2);

        if (NmEnCooCtxBow->IsDocNm(DocNm)) {
            const int DId = NmEnCooCtxBow->GetDId(DocNm);
            PBowSpV DocSpV = NmEnCooCtxBoww->GetSpV(DId);
            PBowKWordSet KWordSet = DocSpV->GetKWordSet(NmEnCooCtxBow)->GetTopKWords(5, 1.0);
            FOut.PutStrLn(TStr::Fmt("%d : %s - %s [%s]",  CountNmV[CountNmN].Val1.Val,
                NmEnStr1.CStr(), NmEnStr2.CStr(), KWordSet->GetKWordsStr().ToLc().CStr()));
        } else {
            FOut.PutStrLn(TStr::Fmt("%d : %s - %s",
                CountNmV[CountNmN].Val1.Val, NmEnStr1.CStr(), NmEnStr2.CStr()));
        }
    }
    FOut.Flush();
}
