#ifndef NMEN_H
#define NMEN_H

/////////////////////////////////////////////////
// Name-Entity-Context-Base
ClassTP(TNmEnCtxBs, PNmEnCtxBs)//{
private:
    TUIntV CtxPtV;
    PStrPool CtxPool;
    THash<TInt, TTm> CtxDateH;

public:
    TNmEnCtxBs() { CtxPool = TStrPool::New(); }
    static PNmEnCtxBs New() { return new TNmEnCtxBs(); }
    TNmEnCtxBs(TSIn& SIn);
    static PNmEnCtxBs Load(TSIn& SIn) { return new TNmEnCtxBs(SIn); }
    void Save(TSOut& SOut) const;

    int AddCtx(const TStr& CtxStr, const TTm& CtxDate);
    int GetCtxs() const { return CtxPtV.Len(); }
    TStr GetCtxStr(const int& CtxId) const { return CtxPool->GetStr(CtxPtV[CtxId]); }
    TTm GetCtxTm(const int& CtxId) const { return CtxDateH.GetDat(CtxId); }
};

/////////////////////////////////////////////////
// Name-Entity-Base
typedef enum { netUndef, netOrg, netLocation, netPerson } TNmEnType;

ClassTP(TNmEnBs, PNmEnBs)//{
private:
    // name entity type conversion
    TStrH TypeStrToIntH;
    TIntStrH TypeIntToStrH;
    // name-entity info
    TStrH NmEnStrH;
    TIntIntHH NmEnTypeHH;
    TIntIntHH NmEnRelNmEnHH;
    // contexts
    PNmEnCtxBs NmEnCtxBs;
    TIntIntVH NmEnCtxIdVH;
    TIntPrIntVH NmEnPrCtxIdVH;
    // profiles
    TIntH NmEnToDIdH;
    PBowDocBs NmEnCtxBow;
    PBowDocWgtBs NmEnCtxBoww;
    // co-profiles
    TIntIntVH NmEnToDIdVH;
    PBowDocBs NmEnCooCtxBow;
    PBowDocWgtBs NmEnCooCtxBoww;

    TIntPr GetNmEnPr(const int& NmEnId1, const int& NmEnId2) const {
        return TIntPr(TInt::GetMn(NmEnId1, NmEnId2), TInt::GetMx(NmEnId1, NmEnId2)); }
    TStr GetNmEnPrStr(const int& NmEnId1, const int& NmEnId2) const {
        TIntPr NmEnPr = GetNmEnPr(NmEnId1, NmEnId2);
        return NmEnPr.Val1.GetStr() + "-" + NmEnPr.Val2.GetStr(); }
    void GetMxNmEnV(const int& MxNmEns, TIntV& NmEnIdV, const TIntH& TypeH) const;
    void GetMxNmEnH(const int& MxNmEns, TIntH& NmEnIdH) const;
    TStr GetClustNm(PBowDocPartClust Clust, PBowDocBs BowDocBs,
        PBowDocWgtBs BowDocWgtBs, const int& TopNmEns) const;

    // context generation functions
    TStr GenNmEnCtx(const TStr& NmEnStr, const TStr& ParStr) const;
    TStr GenNmEnPrCtx(const TStr& NmEn1Str, const TStr& NmEn2Str,
        const TStr& ParStr) const;

public:
    TNmEnBs();
    static PNmEnBs New() { return new TNmEnBs; }
    TNmEnBs(TSIn& SIn, const bool& LoadCtxP = true);
    static PNmEnBs Load(TSIn& SIn, const bool& LoadCtxP = true) {
        return new TNmEnBs(SIn, LoadCtxP); }
    TNmEnBs(TSIn& SIn, PBowDocBs _NmEnCtxBow);
    static PNmEnBs Load(TSIn& SIn, PBowDocBs _NmEnCtxBow) {
        return new TNmEnBs(SIn, _NmEnCtxBow); }
    void Save(TSOut& SOut) const;

    TNmEnType GetType(const TStr& TypeStr);
    TStr GetTypeStr(const TNmEnType& TypeInt);
    int GetNmEns() const { return NmEnStrH.Len(); }
    int GetFFirstNmEn() const { return NmEnStrH.FFirstKeyId(); }
    bool GetFNextNmEn(int& KeyId) { return NmEnStrH.FNextKeyId(KeyId); }
    bool IsNmEn(const TStr& NmEnStr) const { return NmEnStrH.IsKey(NmEnStr); }
    int AddNmEn(const TStr& NmEnStr, const TNmEnType& NmEnType);
    void AddRelNmEn(const int& NmEnId1, const int& NmEnId2) {
        NmEnRelNmEnHH.AddDat(NmEnId1).AddDat(NmEnId2)++;
        NmEnRelNmEnHH.AddDat(NmEnId2).AddDat(NmEnId1)++; }
    int GetNmEnId(const TStr& NmEnStr) const { return NmEnStrH.GetKeyId(NmEnStr); }
    TStr GetNmEnStr(const int& NmEnId) const { return NmEnStrH.GetKey(NmEnId); }
    TNmEnType GetNmEnType(const int& NmEnId) const;

    bool IsNmEnDId(const int& NmEnId) const { return NmEnToDIdH.IsKey(NmEnId); }
    int GetNmEnDId(const int& NmEnId) const { return NmEnToDIdH.GetDat(NmEnId); }
    const TIntV& GetCtxIdV(const int& NmEnId) const { return NmEnCtxIdVH.GetDat(NmEnId); }
    int GetNmEnCount(const TStr& NmEnStr) const { return GetCtxIdV(GetNmEnId(NmEnStr)).Len(); }
    int GetNmEnCount(const int& NmEnId) const { return GetCtxIdV(NmEnId).Len(); }

    int GetNmEnPrs() const { return NmEnPrCtxIdVH.Len(); }
    const TIntV& GetPrCtxIdV(const int& NmEnId1, const int& NmEnId2) const {
        return NmEnPrCtxIdVH.GetDat(GetNmEnPr(NmEnId1, NmEnId2)); }
    int GetNmEnPrCount(const int& NmEnId1, const int& NmEnId2) const {
        return GetPrCtxIdV(NmEnId1, NmEnId2).Len(); }

    void GetRelated(const int& NmEnId, TIntPrV& RelNmEnIdWgtV,
        const int& MxNmEns = -1, const bool& SortP = true);
    void PrintRelated(const TStr& NmEnStr, const int& MxNmEns);

    void GetRelated(const TIntV& ClustDIdV1, const TIntV& ClustDIdV2,
        TIntV& CooCtxDIdV, TIntPrV& NmEnWgtV);
    void GetRelated(const TIntV& ClustDIdV1, const TIntV& ClustDIdV2,
        TStr& RelNm, int& RelPrs, PBowSpV& RelSpV);
    void GetRelated(const TIntV& ClustDIdV1, const TIntV& ClustDIdV2,
        const int& MxKeyWds, TStr& RelNm, int& RelPrs, TStr& RelKeyWdStr);

    int GetCtxs() const { return NmEnCtxBs->GetCtxs(); }
    void AddNmEnCtx(const int& NmEnId, const int& CtxId);
    void AddNmEnPrCtx(const int& NmEnId1, const int& NmEnId2, const int& CtxId);
    TStr GetCtxStr(const int& CtxId) const { return NmEnCtxBs->GetCtxStr(CtxId); }
    TTm GetCtxTm(const int& CtxId) const { return NmEnCtxBs->GetCtxTm(CtxId); }

    void GenNmEnCtxBow(const int& MxNmEns, const TIntH& TypeH);
    PBowDocBs GetNmEnCtxBow() const { return NmEnCtxBow; }
    void GenNmEnCooCtxBow(const int& MxNmEns);
    PBowDocBs GetNmEnCooCtxBow() const { return NmEnCooCtxBow; }

    PBowDocPart GenBowDocPart(const int& Clusts);
    void PrintClustSim(const TStr& FNm, const int& Clusts);

    void AddReutersXml(const TStr& XmlFNm, const bool& GenCooCtxP = false);
    static PNmEnBs LoadReuters(const TStr& FPath, const TTm& StartDate,
        const TTm& EndDate, const bool& GenCooCtxP = false);

    static PNmEnBs LoadMsrSent(const TStr& InFNm);

    static PNmEnBs LoadBin(const TStr& FNm, const bool& LoadCtxP = true) {
        TFIn FIn(FNm); return Load(FIn, LoadCtxP); }
    static PNmEnBs LoadBin(const TStr& FNm, PBowDocBs _NmEnCtxBow) {
        TFIn FIn(FNm); return Load(FIn, _NmEnCtxBow); }
    void SaveBin(const TStr& FNm) const { TFOut FOut(FNm); Save(FOut); }
    void SaveNmEnStat(const TStr& FNm) const;
    void SaveNmEnCtxStat(const TStr& FNm) const;
    void SaveNmEnPrStat(const TStr& FNm) const;
};

#endif
