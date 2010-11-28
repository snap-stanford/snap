#ifndef pi_h
#define pi_h

/////////////////////////////////////////////////
// Includes
#include "mine.h"
#include "graph.h"
#include "gks.h"

/////////////////////////////////////////////////
// Forward
ClassHdTP(TPiDb, PPiDb);

/////////////////////////////////////////////////
// Project-Intelligence-Record
ClassTPV(TPiRec, PPiRec, TPiRecV)//{
private:
  TIntIntH FldNmCIdToValCIdH;
  UndefCopyAssign(TPiRec);
public:
  TPiRec(): FldNmCIdToValCIdH(){}
  static PPiRec TPiRec::New(){return new TPiRec();}
  TPiRec(TSIn& SIn): FldNmCIdToValCIdH(SIn){}
  static PPiRec Load(TSIn& SIn){return PPiRec(new TPiRec(SIn));}
  void Save(TSOut& SOut) const {FldNmCIdToValCIdH.Save(SOut);}

  // field-values
  void AddFldNmVal(
   TPiDb* PiDb, const TStr& FldNm, const TStr& FldVal);
  int GetFlds() const {return FldNmCIdToValCIdH.Len();}
  bool IsGetFldNmVal(const int& FldNmCId, TInt& FldValCId) const;
  void GetFldNmVal(const int& FldN, int& FldNmCId, int& FldValCId) const;
  void GetFldNmVal(
   const TPiDb* PiDb, const int& FldN, TStr& FldNm, TStr& FldVal) const;
  TStr GetFldVal(
   const TPiDb* PiDb, const TStr& FldNm, const TStr& DfVal="") const;

  // project - specific values
  TStr GetProjAcronym(const TPiDb* PiDb) const {
    return GetFldVal(PiDb, "Project Acronym");}
  TStr GetProjTitle(const TPiDb* PiDb) const {
    return GetFldVal(PiDb, "Title");}
  TStr GetProjSubj(const TPiDb* PiDb) const {
    return GetFldVal(PiDb, "Subject Index ");}
  TStr GetProjStartDate(const TPiDb* PiDb) const {
    return GetFldVal(PiDb, "Start Date");}
  TStr GetProjEndDate(const TPiDb* PiDb) const {
    return GetFldVal(PiDb, "End Date");}
  TStr GetProjDuration(const TPiDb* PiDb) const {
    return GetFldVal(PiDb, "Duration");}
  TStr GetProjDesc(const TPiDb* PiDb) const {
    return GetFldVal(PiDb, "Objective");}
  TStr GetProjRef(const TPiDb* PiDb) const {
    return GetFldVal(PiDb, "Project Reference");}
  TStr GetCallNm(const TPiDb* PiDb) const {
    return GetFldVal(PiDb, "Call");}
  TStr GetProjProg(const TPiDb* PiDb) const {
    return GetFldVal(PiDb, "Programme Acronym ");}
  TStr GetProjSubProg(const TPiDb* PiDb) const {
    return GetFldVal(PiDb, "Subprogramme Area");}
  TStr GetProjInstr(const TPiDb* PiDb) const {
    return GetFldVal(PiDb, "Contract Type");}
  TStr GetProjValue(const TPiDb* PiDb) const {
    return GetFldVal(PiDb, "Project Cost");}
  TStr GetProjFund(const TPiDb* PiDb) const {
    return GetFldVal(PiDb, "Project Funding");}
  TStr GetProjPrimeContr(const TPiDb* PiDb) const {
    return GetFldVal(PiDb, "Prime Contractor");}
  TStr GetProjPartners(const TPiDb* PiDb) const {
    return GetFldVal(PiDb, "Partners");}

  // organization - specific values
  TStr GetOrgNm(const TPiDb* PiDb) const {
    return GetFldVal(PiDb, "Organisation");}
  TStr GetOrgType(const TPiDb* PiDb) const {
    return GetFldVal(PiDb, "Organisation Type");}
  TStr GetSME(const TPiDb* PiDb) const {
    return GetFldVal(PiDb, "SME");}
  TStr GetOrgCountry(const TPiDb* PiDb) const {
    return GetFldVal(PiDb, "Country");}
  TStr GetOrgRegion(const TPiDb* PiDb) const {
    return GetFldVal(PiDb, "Region");}
  TStr GetOrgCity(const TPiDb* PiDb) const {
    return GetFldVal(PiDb, "City");}
  TStr GetOrgProjs(const TPiDb* PiDb) const {
    return GetFldVal(PiDb, "Projects");}
  TStr GetOrgFunding(const TPiDb* PiDb) const {
    return GetFldVal(PiDb, "Funding");}

  // country - specific values
  TStr GetCountryNm(const TPiDb* PiDb) const {
    return GetFldVal(PiDb, "Country");}
  TStr GetCountryOrgs(const TPiDb* PiDb) const {
    return GetFldVal(PiDb, "Country Organisations");}
  TStr GetCountryProjs(const TPiDb* PiDb) const {
    return GetFldVal(PiDb, "Country Projects");}
  TStr GetCountryFunding(const TPiDb* PiDb) const {
    return GetFldVal(PiDb, "Country Funding");}
  TStr GetCountryPopulation(const TPiDb* PiDb) const {
    return GetFldVal(PiDb, "Country Population");}
  TStr GetCountryGdpPerCapita(const TPiDb* PiDb) const {
    return GetFldVal(PiDb, "Country GDP Per Capita");}
  TStr GetCountryFundingPerCapita(const TPiDb* PiDb) const {
    return GetFldVal(PiDb, "Country Funding Per Capita");}
  TStr GetCountryFundingPerGdp(const TPiDb* PiDb) const {
    return GetFldVal(PiDb, "Country Funding Per GDP");}

  // special strings
  static TStr GetNrMoneyStr(const TStr& MoneyStr);
  static TStr GetNrDurationStr(const TStr& DurationStr);
};

/////////////////////////////////////////////////
// Project-Intelligence-Graph
ClassTP(TPiGraph, PPiGraph)//{
private:
  TStrH CallNmH;
  TStrH SubProgNmH;
  TStrH CountryNmH;
  TStrH OrgNmH;
  TStrPrIntH CallNmPrToFqH;
  TStrPrIntH SubProgNmPrToFqH;
  TStrPrIntH CountryNmPrToFqH;
  TStrPrIntH OrgNmPrToFqH;
  TStrIntH OrgNmToColorH;
  THash<TStr, TStrPrIntH> CallNmToOrgNmPrToFqH;
  THash<TStr, TStrPrIntH> SubProgNmToOrgNmPrToFqH;
  THash<TStr, TStrPrIntH> CountryNmToOrgNmPrToFqH;
  UndefDefaultCopyAssign(TPiGraph);
public:
  TPiGraph(const PPiDb& PiDb);
  static PPiGraph New(const PPiDb& PiDb){
    return new TPiGraph(PiDb);}
  TPiGraph(TSIn& SIn):
    CallNmH(SIn), SubProgNmH(SIn),
    CountryNmH(SIn), OrgNmH(SIn),
    CallNmPrToFqH(SIn), SubProgNmPrToFqH(SIn),
    CountryNmPrToFqH(SIn), OrgNmPrToFqH(SIn),
    OrgNmToColorH(SIn),
    CallNmToOrgNmPrToFqH(SIn), SubProgNmToOrgNmPrToFqH(SIn),
    CountryNmToOrgNmPrToFqH(SIn){}
  static PPiGraph Load(TSIn& SIn){return PPiGraph(new TPiGraph(SIn));}
  void Save(TSOut& SOut) const {
    CallNmH.Save(SOut); SubProgNmH.Save(SOut);
    CountryNmH.Save(SOut); OrgNmH.Save(SOut);
    CallNmPrToFqH.Save(SOut); SubProgNmPrToFqH.Save(SOut);
    CountryNmPrToFqH.Save(SOut); OrgNmPrToFqH.Save(SOut);
    OrgNmToColorH.Save(SOut);
    CallNmToOrgNmPrToFqH.Save(SOut); SubProgNmToOrgNmPrToFqH.Save(SOut);
    CountryNmToOrgNmPrToFqH.Save(SOut);}

  // graph construction
  PGraph GetDistTree(const PGraph& Graph, const TStr& StartVNm);
  PGraph GetGraph(const TStrPrIntH& StrPrToFqH, const TStrIntH& StrToColorH,
   const double& MxWgtSumPrc=1.0, const int& MxVrtxs=-1);

  // graph analysis
  static PMom GetDistMom(const PGraph& Graph, const int& StartVId);
  static void GetConnectedness(
   const PGraph& Graph, const TStr& TxtFNm, const TStr& TabFNm);
  static void GetConnComps(const PGraph& Graph, const TStr& FNm);

  // calls
  int GetCalls() const {return CallNmH.Len();}
  TStr GetCallNm(const int& CallNmN) const {return CallNmH.GetKey(CallNmN);}
  PGraph GetCallGraph(const double& MxWgtSumPrc=1.0, const int& MxVrtxs=-1){
    return GetGraph(CallNmPrToFqH, OrgNmToColorH, MxWgtSumPrc, MxVrtxs);}
  PGraph GetCallGraph(const TStr& CallNm,
   const double& MxWgtSumPrc=1.0, const int& MxVrtxs=-1){
    if (!CallNmToOrgNmPrToFqH.IsKey(CallNm)){return PGraph::New();}
    TStrPrIntH& OrgNmPrToFqH=CallNmToOrgNmPrToFqH.GetDat(CallNm);
    return GetGraph(OrgNmPrToFqH, OrgNmToColorH, MxWgtSumPrc, MxVrtxs);}

  // subprogrammes
  int GetSubProgs() const {return SubProgNmH.Len();}
  TStr GetSubProgNm(const int& SubProgNmN) const {return SubProgNmH.GetKey(SubProgNmN);}
  PGraph GetSubProgGraph(const double& MxWgtSumPrc=1.0, const int& MxVrtxs=-1){
    return GetGraph(SubProgNmPrToFqH, OrgNmToColorH, MxWgtSumPrc, MxVrtxs);}
  PGraph GetSubProgGraph(const TStr& SubProgNm,
   const double& MxWgtSumPrc=1.0, const int& MxVrtxs=-1){
    if (!SubProgNmToOrgNmPrToFqH.IsKey(SubProgNm)){return PGraph::New();}
    TStrPrIntH& OrgNmPrToFqH=SubProgNmToOrgNmPrToFqH.GetDat(SubProgNm);
    return GetGraph(OrgNmPrToFqH, OrgNmToColorH, MxWgtSumPrc, MxVrtxs);}

  // countries
  int GetCountries() const {return CountryNmH.Len();}
  TStr GetCountryNm(const int& CountryNmN) const {return CountryNmH.GetKey(CountryNmN);}
  PGraph GetCountryGraph(const double& MxWgtSumPrc=1.0, const int& MxVrtxs=-1){
    return GetGraph(CountryNmPrToFqH, OrgNmToColorH, MxWgtSumPrc, MxVrtxs);}
  PGraph GetCountryGraph(const TStr& CountryNm,
   const double& MxWgtSumPrc=1.0, const int& MxVrtxs=-1){
    if (!CountryNmToOrgNmPrToFqH.IsKey(CountryNm)){return PGraph::New();}
    TStrPrIntH& OrgNmPrToFqH=CountryNmToOrgNmPrToFqH.GetDat(CountryNm);
    return GetGraph(OrgNmPrToFqH, OrgNmToColorH, MxWgtSumPrc, MxVrtxs);}

  // orgs
  int GetOrgs() const {return OrgNmH.Len();}
  TStr GetOrgNm(const int& OrgNmN) const {return OrgNmH.GetKey(OrgNmN);}
  PGraph GetOrgGraph(const double& MxWgtSumPrc=1.0, const int& MxVrtxs=-1){
    return GetGraph(OrgNmPrToFqH, OrgNmToColorH, MxWgtSumPrc, MxVrtxs);}
};

/////////////////////////////////////////////////
// Project-Intelligence-Database
ClassTP(TPiDb, PPiDb)//{
private:
  TStrIntH StrCacheToFqH;
  THash<TInt, PPiRec> ProjCRefToProjH;
  THash<TInt, PPiRec> OrgCNmToOrgH;
  THash<TInt, PPiRec> CountryCNmToCountryH;
  THash<TInt, TIntV> OrgCNmToProjCRefVH;
  THash<TInt, TIntV> ProjCRefToOrgCNmVH;
  PPiGraph PiGraph;
  PBowDocBs BowDocBs;
  UndefCopyAssign(TPiDb);
public:
  TPiDb():
    StrCacheToFqH(),
    ProjCRefToProjH(), OrgCNmToOrgH(), CountryCNmToCountryH(),
    OrgCNmToProjCRefVH(), ProjCRefToOrgCNmVH(), PiGraph(), BowDocBs(){}
  static PPiDb TPiDb::New(){return new TPiDb();}
  TPiDb(TSIn& SIn):
    StrCacheToFqH(SIn),
    ProjCRefToProjH(SIn), OrgCNmToOrgH(SIn), CountryCNmToCountryH(SIn),
    OrgCNmToProjCRefVH(SIn), ProjCRefToOrgCNmVH(SIn), PiGraph(SIn), BowDocBs(SIn){}
  static PPiDb Load(TSIn& SIn){return PPiDb(new TPiDb(SIn));}
  void Save(TSOut& SOut) const {
    StrCacheToFqH.Save(SOut);
    ProjCRefToProjH.Save(SOut); OrgCNmToOrgH.Save(SOut);
    CountryCNmToCountryH.Save(SOut);
    OrgCNmToProjCRefVH.Save(SOut); ProjCRefToOrgCNmVH.Save(SOut);
    PiGraph.Save(SOut); BowDocBs.Save(SOut);}

  // string-cache
  int AddCStr(const TStr& Str){
    int StrCId=StrCacheToFqH.AddKey(Str); StrCacheToFqH[StrCId]++; return StrCId;}
  int GetCId(const TStr& Str) const {int StrCId;
    if (StrCacheToFqH.IsKey(Str, StrCId)){return StrCId;} else {return -1;}}
  TStr GetCStr(const int& StrCId) const {
    if (StrCacheToFqH.IsKeyId(StrCId)){return StrCacheToFqH.GetKey(StrCId);}
    else {return "";}}
  void GetCStrV(const TIntH& StrCIdH, TStrV& StrV) const;
  int GetCStrs() const {return StrCacheToFqH.Len();}
  void GetCStrFq(const int& StrCId, TStr& Str, int& StrFq) const {
    Str=StrCacheToFqH.GetKey(StrCId); StrFq=StrCacheToFqH[StrCId];}

  // projects
  void AddProj(const PPiRec& Proj){
    ProjCRefToProjH.AddDat(AddCStr(Proj->GetProjRef(this)), Proj);}
  bool IsProj(const TStr& ProjRef) const {
    return ProjCRefToProjH.IsKey(GetCId(ProjRef));}
  int GetProjs() const {return ProjCRefToProjH.Len();}
  PPiRec GetProj(const int& ProjN) const {return ProjCRefToProjH[ProjN];}
  PPiRec GetProj(const TStr& ProjRef) const {
    if (ProjCRefToProjH.IsKey(GetCId(ProjRef))){
      return ProjCRefToProjH.GetDat(GetCId(ProjRef));}
    else {return NULL;}}
  bool IsProjOrg(const TStr& ProjRef, const TStr& OrgNm) const {
    int ProjOrgs=GetProjOrgs(ProjRef);
    for (int OrgN=0; OrgN<ProjOrgs; OrgN++){
      PPiRec Org=GetProjOrg(ProjRef, OrgN);
      if (Org->GetOrgNm(this)==OrgNm){return true;}}
    return false;}
  int GetProjOrgs(const TStr& ProjRef) const {
    int ProjRefCId=GetCId(ProjRef);
    if (ProjCRefToOrgCNmVH.IsKey(ProjRefCId)){
      return ProjCRefToOrgCNmVH.GetDat(ProjRefCId).Len();}
    else {return 0;}}
  TStr GetProjOrgsStr(const TStr& ProjRef) const {
    int ProjOrgs=GetProjOrgs(ProjRef);
    if (ProjOrgs==1){return TInt::GetStr(ProjOrgs, "%3d partner");}
    else {return TInt::GetStr(ProjOrgs, "%3d partners");}}
  PPiRec GetProjOrg(const TStr& ProjRef, const int& OrgN) const {
    int ProjRefCId=GetCId(ProjRef);
    if (ProjCRefToOrgCNmVH.IsKey(ProjRefCId)){
      int OrgNmCId=ProjCRefToOrgCNmVH.GetDat(ProjRefCId)[OrgN];
      TStr OrgNm=GetCStr(OrgNmCId);
      return GetOrg(OrgNm);
    } else {return TPiRec::New();}}

  // organizations
  void AddOrgOnProj(const PPiRec& Org, const TStr& ProjRef){
    OrgCNmToOrgH.AddDat(AddCStr(Org->GetOrgNm(this)), Org);
    OrgCNmToProjCRefVH.AddDat(AddCStr(Org->GetOrgNm(this))).Add(AddCStr(ProjRef));
    ProjCRefToOrgCNmVH.AddDat(AddCStr(ProjRef)).Add(AddCStr(Org->GetOrgNm(this)));}
  bool IsOrg(const TStr& OrgNm) const {
    return OrgCNmToOrgH.IsKey(GetCId(OrgNm));}
  int GetOrgs() const {return OrgCNmToOrgH.Len();}
  PPiRec GetOrg(const int& OrgN) const {return OrgCNmToOrgH[OrgN];}
  PPiRec GetOrg(const TStr& OrgNm) const {
    if (OrgCNmToOrgH.IsKey(GetCId(OrgNm))){
      return OrgCNmToOrgH.GetDat(GetCId(OrgNm));}
    else {return TPiRec::New();}}
  int GetOrgProjs(const TStr& OrgNm) const {
    if (OrgCNmToProjCRefVH.IsKey(GetCId(OrgNm))){
      return OrgCNmToProjCRefVH.GetDat(GetCId(OrgNm)).Len();}
    else {return 0;}}
  TStr GetOrgProjsStr(const TStr& OrgNm) const {
    if (GetOrgProjs(OrgNm)==1){return TInt::GetStr(GetOrgProjs(OrgNm), "%3d project");}
    else {return TInt::GetStr(GetOrgProjs(OrgNm), "%3d projects");}}
  PPiRec GetOrgProj(const TStr& OrgNm, const int& ProjN) const {
    int OrgNmCId=GetCId(OrgNm);
    if (OrgCNmToProjCRefVH.IsKey(OrgNmCId)){
      int ProjRefCId=OrgCNmToProjCRefVH.GetDat(GetCId(OrgNm))[ProjN];
      TStr ProjRef=GetCStr(ProjRefCId);
      return GetProj(ProjRef);
    } else {return TPiRec::New();}}
  double GetOrgFunding(const TStr& OrgNm) const;
  TStr GetOrgFundingStr(const TStr& OrgNm) const {
    return TFlt::GetStr(GetOrgFunding(OrgNm), "%8.2fMeuro");}
  int GetPrimeContrs(const TStr& OrgNm) const;
  void GetFqClbOrgNmV(
   const TStr& OrgNm, int& SumClbs, TIntStrPrV& FqClbOrgNmPrV) const;
  void GetClbProjRefV(
   const TStr& OrgNm1, const TStr& OrgNm2, TStrV& ClbProjRefV) const;
  void GetOrgProjSummary(const TStr& OrgNm, TStrPrV& FundProjRefPrV) const;

  // countries
  void AddCountry(const PPiRec& Country){
    CountryCNmToCountryH.AddDat(AddCStr(Country->GetCountryNm(this)), Country);}
  bool IsCountry(const TStr& CountryNm) const {
    return CountryCNmToCountryH.IsKey(GetCId(CountryNm));}
  int GetCountries() const {return CountryCNmToCountryH.Len();}
  PPiRec GetCountry(const int& CountryN) const {return CountryCNmToCountryH[CountryN];}
  PPiRec GetCountry(const TStr& CountryNm) const {
    if (CountryCNmToCountryH.IsKey(GetCId(CountryNm))){
      return CountryCNmToCountryH.GetDat(GetCId(CountryNm));}
    else {return TPiRec::New();}}
  void GetCountryProjSummary(const TStr& CountryNm,
   int& PrimeContracts, TIntStrPrV& FqPrimeContrOrgNmPrV,
   int& SumProjRefFq, TStrStrIntTrV& FundProjRefFqTrV,
   int& SumCountryClbFq, TIntStrPrV& FqCountryNmPrV) const;
  void GetCountryOrgSummary(const TStr& CountryNm,
   int& SumOrgNmFq, TIntStrPrV& FqOrgNmPrV,
   int& SumOrgTypeFq, TIntStrPrV& FqOrgTypePrV) const;

  // fields & values
  void GetSortedProjRefV(const TStr& KeyNm, TStrV& ProjRefV) const;
  void GetSortedOrgNmV(const TStr& KeyNm, TStrV& OrgNmV) const;
  void GetSortedCountryNmV(const TStr& KeyNm, TStrV& CountryOrgNmV) const;

  // graphs
  PPiGraph GetPiGraph() const {return PiGraph;}

  // bow
  PBowDocBs GenBowDocBs() const;
  PBowDocBs GetBowDocBs() const {return BowDocBs;}

  // files
  static PPiRec LoadProjHtml(const TStr& FNm, const PPiDb& PiDb);
  static PPiDb LoadHtml(const TStr& FPath, const TStr& FBasePrefixStr="");
  static PPiDb LoadBin(const TStr& FNm){
    TFIn SIn(FNm); return Load(SIn);}
  void SaveBin(const TStr& FNm){
    TFOut SOut(FNm); Save(SOut);}
  void SaveTxtLnDoc(const TStr& FNm);

  // custom file formats
  static PPiDb LoadFP5IstTab(const TStr& ProjOrgTabFNm);
  static PPiDb LoadFP6IstTab(const TStr& ProjOrgTabFNm, const TStr& ProjDocXmlFNm);
  static PPiDb LoadFP7IstTab(const TStr& ProjOrgTabFNm);
};

#endif
