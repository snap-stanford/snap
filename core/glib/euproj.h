#ifndef euproj_h
#define euproj_h

/////////////////////////////////////////////////
// Includes
#include "mine.h"
#include "ciawfb.h"
#include "cordis.h"

/////////////////////////////////////////////////
// Forward
ClassHdTP(TEpBs, PEpBs);

/////////////////////////////////////////////////
// EuroProject-Organization
ClassTPV(TEpOrg, PEpOrg, TEpOrgV)//{
private:
  TStr OrgNm, BriefOrgNm;
  TStr OrgTypeNm;
  TStr CountryNm;
  TStrV ProjNmV;
  TIntIntH ClbOrgIdToFqH;
  TIntStrVH ClbOrgIdToProjNmVH;
  TInt SumClbOrgFq;
public:
  TEpOrg(){}
  TEpOrg(const TStr& _OrgNm, const TStr& _OrgTypeNm, const TStr& _CountryNm):
    OrgNm(_OrgNm), OrgTypeNm(_OrgTypeNm), CountryNm(_CountryNm){}
  static PEpOrg New(const TStr& OrgNm, const TStr& OrgTypeNm, const TStr& CountryNm){
    return new TEpOrg(OrgNm, OrgTypeNm, CountryNm);}
  ~TEpOrg(){}
  TEpOrg(TSIn& SIn):
    OrgNm(SIn), OrgTypeNm(SIn), CountryNm(SIn),
    ProjNmV(SIn), ClbOrgIdToFqH(SIn),
    ClbOrgIdToProjNmVH(SIn), SumClbOrgFq(SIn){}
  static PEpOrg Load(TSIn& SIn){return new TEpOrg(SIn);}
  void Save(TSOut& SOut){
    OrgNm.Save(SOut); OrgTypeNm.Save(SOut); CountryNm.Save(SOut);
    ProjNmV.Save(SOut); ClbOrgIdToFqH.Save(SOut);
    ClbOrgIdToProjNmVH.Save(SOut); SumClbOrgFq.Save(SOut);}

  TEpOrg& operator=(const TEpOrg& EpOrg){Fail; return *this;}
  bool operator==(const TEpOrg& EpOrg) const {
    return OrgNm==EpOrg.OrgNm;}
  bool operator<(const TEpOrg& EpOrg) const {
    return OrgNm<EpOrg.OrgNm;}

  // component retrieval & update
  TStr GetOrgNm() const {return OrgNm;}
  void PutBriefOrgNm(const TStr& _BriefOrgNm){BriefOrgNm=_BriefOrgNm;}
  TStr GetBriefOrgNm() const {
    if (BriefOrgNm.Empty()){return OrgNm;} else {return BriefOrgNm;}}
  static TStr GetMultiLnOrgNm(
   const TStr& OrgNm, const int& MnLnLen=10, const char& NewLnCh='\\');

  TStr GetOrgTypeNm() const {return OrgTypeNm;}
  TStr GetCountryNm() const {return CountryNm;}

  int GetProjs() const {return ProjNmV.Len();}
  TStr GetProjNm(const int& ProjN) const {return ProjNmV[ProjN];}
  void AddProj(const TStr& ProjNm){ProjNmV.Add(ProjNm);}
  void GetProjNmV(TStrV& _ProjNmV) const {
    _ProjNmV=ProjNmV; _ProjNmV.Sort();}

  int GetClbOrgs() const {return ClbOrgIdToFqH.Len();}
  int GetClbOrgId(const int& ClbOrgN) const {
    return ClbOrgIdToFqH.GetKey(ClbOrgN);}
  TStr GetClbOrgNm(const int& ClbOrgN, const TEpBs* EpBs) const;
  int GetClbOrgFq(const int& ClbOrgN) const {
    return ClbOrgIdToFqH[ClbOrgN];}
  int GetClbOrgFq(const TStr& ClbOrgNm, const TEpBs* EpBs) const;
  void GetClbOrgProjNmV(
   const TStr& ClbOrgNm, TStrV& ProjNmV, const TEpBs* EpBs) const;
  double GetClbOrgFqPrc(const TStr& ClbOrgNm, const TEpBs* EpBs) const {
    if (SumClbOrgFq==0){return 0;}
    else {return GetClbOrgFq(ClbOrgNm, EpBs)/double(SumClbOrgFq);}}
  void AddClbOrg(const TStr& ClbOrgNm, const TStr& ProjNm, const TEpBs* EpBs);
  int GetSumClbOrgsFq() const {return SumClbOrgFq;}
  void GetFqClbOrgIdPrV(TIntPrV& FqClbOrgIdPrV){
    ClbOrgIdToFqH.GetDatKeyPrV(FqClbOrgIdPrV);}
};

/////////////////////////////////////////////////
// EuroProject-Country
ClassTPV(TEpCountry, PEpCountry, TEpCountryV)//{
private:
  TStr CountryNm;
  TIntIntH OrgIdToFqH;
  TStrIntH OrgTypeNmToFqH;
  TStrIntH ProjNmToFqH;
  TStrIntH ClbCountryNmToFqH;
  TInt SumClbCountriesFq;
public:
  TEpCountry(){}
  TEpCountry(const TStr& _CountryNm):
    CountryNm(_CountryNm){}
  static PEpCountry New(const TStr& CountryNm){
    return new TEpCountry(CountryNm);}
  ~TEpCountry(){}
  TEpCountry(TSIn& SIn):
    CountryNm(SIn), OrgIdToFqH(SIn),
    OrgTypeNmToFqH(SIn), ProjNmToFqH(SIn),
    ClbCountryNmToFqH(SIn), SumClbCountriesFq(SIn){}
  static PEpCountry Load(TSIn& SIn){return new TEpCountry(SIn);}
  void Save(TSOut& SOut){
    CountryNm.Save(SOut); OrgIdToFqH.Save(SOut);
    OrgTypeNmToFqH.Save(SOut); ProjNmToFqH.Save(SOut);
    ClbCountryNmToFqH.Save(SOut); SumClbCountriesFq.Save(SOut);}

  TEpCountry& operator=(const TEpCountry& EpCountry){Fail; return *this;}
  bool operator==(const TEpCountry& EpCountry) const {
    return CountryNm==EpCountry.CountryNm;}
  bool operator<(const TEpCountry& EpCountry) const {
    return CountryNm<EpCountry.CountryNm;}

  // component retrieval & update
  TStr GetCountryNm() const {return CountryNm;}
  void AddOrg(const TStr& OrgNm, const TEpBs* EpBs);
  void AddOrgType(const TStr& OrgTypeNm){OrgTypeNmToFqH.AddDat(OrgTypeNm)++;}
  int GetOrgs() const {return OrgIdToFqH.Len();}
  int GetOrgId(const int& OrgN) const {return OrgIdToFqH.GetKey(OrgN);}
  TStr GetOrgNm(const int& OrgN, const TEpBs* EpBs) const;
  int GetOrgFq(const int& OrgN) const {return OrgIdToFqH[OrgN];}
  int GetOrgFqSum() const {
    int Orgs=GetOrgs(); int OrgFqSum=0;
    for (int OrgN=0; OrgN<Orgs; OrgN++){OrgFqSum+=GetOrgFq(OrgN);}
    return OrgFqSum;}

  void AddProj(const TStr& ProjNm){ProjNmToFqH.AddDat(ProjNm)++;}
  int GetProjs() const {return ProjNmToFqH.Len();}
  TStr GetProjNm(const int& ProjN) const {return ProjNmToFqH.GetKey(ProjN);}
  void GetProjNmV(TStrV& ProjNmV) const {
    ProjNmToFqH.GetKeyV(ProjNmV); ProjNmV.Sort();}

  int GetClbCountries() const {return ClbCountryNmToFqH.Len();}
  TStr GetClbCountryNm(const int& ClbCountryN) const {return ClbCountryNmToFqH.GetKey(ClbCountryN);}
  int GetClbCountryFq(const int& ClbCountryN) const {return ClbCountryNmToFqH[ClbCountryN];}
  double GetClbCountryFqPrc(const int& ClbCountryN) const {
    if (SumClbCountriesFq==0){return 0;}
    else {return GetClbCountryFq(ClbCountryN)/double(SumClbCountriesFq);}}
  void AddClbCountry(const TStr& ClbCountryNm){
    ClbCountryNmToFqH.AddDat(ClbCountryNm)++; SumClbCountriesFq++;}
  int GetSumClbCountriesFq() const {return SumClbCountriesFq;}
  void GetFqClbCountryNmPrV(TIntStrPrV& FqClbCountryNmPrV){
    ClbCountryNmToFqH.GetDatKeyPrV(FqClbCountryNmPrV);}
};

/////////////////////////////////////////////////
// EuroProject-Officer
ClassTPV(TEpOfficer, PEpOfficer, TEpOfficerV)//{
private:
  TStr OfficerNm;
  TIntIntH OrgIdToFqH;
  TStrH ProjNmH;
  TStrIntH CountryNmToFqH;
public:
  TEpOfficer(){}
  TEpOfficer(const TStr& _OfficerNm):
    OfficerNm(_OfficerNm){}
  static PEpOfficer New(const TStr& OfficerNm){
    return new TEpOfficer(OfficerNm);}
  ~TEpOfficer(){}
  TEpOfficer(TSIn& SIn):
    OfficerNm(SIn), OrgIdToFqH(SIn), ProjNmH(SIn),
    CountryNmToFqH(SIn){}
  static PEpOfficer Load(TSIn& SIn){return new TEpOfficer(SIn);}
  void Save(TSOut& SOut){
    OfficerNm.Save(SOut); OrgIdToFqH.Save(SOut); ProjNmH.Save(SOut);
    CountryNmToFqH.Save(SOut);}

  TEpOfficer& operator=(const TEpOfficer& EpOfficer){Fail; return *this;}

  // component retrieval & update
  TStr GetOfficerNm() const {return OfficerNm;}
  void AddOrg(const TStr& OrgNm, const TEpBs* EpBs);

  void AddProj(const TStr& ProjNm){ProjNmH.AddDat(ProjNm)++;}
  int GetProjs() const {return ProjNmH.Len();}
  TStr GetProjNm(const int& ProjN) const {return ProjNmH.GetKey(ProjN);}
  void GetProjNmV(TStrV& ProjNmV) const {
    ProjNmH.GetKeyV(ProjNmV); ProjNmV.Sort();}

  void AddCountry(const TStr& CountryNm){CountryNmToFqH.AddDat(CountryNm)++;}
  void GetFqCountryPrV(TIntStrPrV& FqCountryPrV) const {
    CountryNmToFqH.GetDatKeyPrV(FqCountryPrV);}
};

/////////////////////////////////////////////////
// EuroProject-Project
ClassTPV(TEpProj, PEpProj, TEpProjV)//{
private:
  TStr ProjNm, ProjRef;
  TStr DomainNm, UnitNm;
  TStr OfficerNm;
  TIntStrKdV OrgIdRoleKdV;
public:
  TEpProj(){}
  TEpProj(const TStr& _ProjNm, const TStr& _ProjRef, const TStr& _DomainNm,
   const TStr& _UnitNm, const TStr& _OfficerNm):
    ProjNm(_ProjNm), ProjRef(_ProjRef),
    DomainNm(_DomainNm), UnitNm(_UnitNm),
    OfficerNm(_OfficerNm){}
  static PEpProj New(const TStr& ProjNm, const TStr& ProjRef,
   const TStr& DomainNm, const TStr& UnitNm, const TStr& OfficerNm){
    return new TEpProj(ProjNm, ProjRef, DomainNm, UnitNm, OfficerNm);}
  ~TEpProj(){}
  TEpProj(TSIn& SIn):
    ProjNm(SIn), ProjRef(SIn),
    DomainNm(SIn), UnitNm(SIn),
    OfficerNm(SIn), OrgIdRoleKdV(SIn){}
  static PEpProj Load(TSIn& SIn){return new TEpProj(SIn);}
  void Save(TSOut& SOut){
    ProjNm.Save(SOut); ProjRef.Save(SOut);
    DomainNm.Save(SOut); UnitNm.Save(SOut);
    OfficerNm.Save(SOut); OrgIdRoleKdV.Save(SOut);}

  TEpProj& operator=(const TEpProj& EpProj){Fail; return *this;}

  // component retrieval & update
  TStr GetProjNm() const {return ProjNm;}
  TStr GetProjRef() const {return ProjRef;}
  TStr GetDomainNm() const {return DomainNm;}
  TStr GetUnitNm() const {return UnitNm;}
  TStr GetOfficerNm() const {return OfficerNm;}

  void AddOrgNmRole(const TStr& OrgNm, const TStr& OrgRoleNm, const TEpBs* EpBs);
  int GetRoles() const {return OrgIdRoleKdV.Len();}
  void GetRole(const int& RoleN, int& OrgId, TStr& RoleNm) const {
    OrgId=OrgIdRoleKdV[RoleN].Key; RoleNm=OrgIdRoleKdV[RoleN].Dat;}
  TStr GetRole(const TStr& OrgNm, const TEpBs* EpBs) const;

  int GetOrgs() const {return OrgIdRoleKdV.Len();}
  int GetOrgId(const int& OrgN) const {return OrgIdRoleKdV[OrgN].Key;}
  TStr GetOrgNm(const int& OrgN, const TEpBs* EpBs) const;
  TStr GetOrgRoleNm(const int& OrgN) const {return OrgIdRoleKdV[OrgN].Dat;}
};

/////////////////////////////////////////////////
// EuroProject-Domain
ClassTPV(TEpDomain, PEpDomain, TEpDomainV)//{
private:
  TStr DomainNm;
  TIntIntH OrgIdToFqH;
  TStrH ProjNmH;
  TStrIntH CountryNmToFqH;
public:
  TEpDomain(){}
  TEpDomain(const TStr& _DomainNm):
    DomainNm(_DomainNm){}
  static PEpDomain New(const TStr& DomainNm){
    return new TEpDomain(DomainNm);}
  ~TEpDomain(){}
  TEpDomain(TSIn& SIn):
    DomainNm(SIn), OrgIdToFqH(SIn), ProjNmH(SIn),
    CountryNmToFqH(SIn){}
  static PEpDomain Load(TSIn& SIn){return new TEpDomain(SIn);}
  void Save(TSOut& SOut){
    DomainNm.Save(SOut); OrgIdToFqH.Save(SOut); ProjNmH.Save(SOut);
    CountryNmToFqH.Save(SOut);}

  TEpDomain& operator=(const TEpDomain& EpDomain){Fail; return *this;}

  // component retrieval & update
  TStr GetDomainNm() const {return DomainNm;}

  void AddOrg(const TStr& OrgNm, const TEpBs* EpBs);
  int GetOrgs() const {return OrgIdToFqH.Len();}
  int GetOrgId(const int& OrgN) const {return OrgIdToFqH.GetKey(OrgN);}
  TStr GetOrgNm(const int& OrgN, const TEpBs* EpBs) const;
  int GetOrgFq(const int& OrgN) const {return OrgIdToFqH[OrgN];}
  bool IsOrg(const TStr& OrgNm, TInt& OrgFq, const TEpBs* EpBs) const;

  void AddProj(const TStr& ProjNm){ProjNmH.AddDat(ProjNm)++;}
  int GetProjs() const {return ProjNmH.Len();}
  TStr GetProjNm(const int& ProjN) const {return ProjNmH.GetKey(ProjN);}
  void GetProjNmV(TStrV& ProjNmV) const {
    ProjNmH.GetKeyV(ProjNmV); ProjNmV.Sort();}

  void AddCountry(const TStr& CountryNm){CountryNmToFqH.AddDat(CountryNm)++;}
};

/////////////////////////////////////////////////
// EuroProject-Unit
ClassTPV(TEpUnit, PEpUnit, TEpUnitV)//{
private:
  TStr UnitNm;
  TIntIntH OrgIdToFqH;
  TStrH ProjNmH;
  TStrIntH CountryNmToFqH;
public:
  TEpUnit(){}
  TEpUnit(const TStr& _UnitNm):
    UnitNm(_UnitNm){}
  static PEpUnit New(const TStr& UnitNm){
    return new TEpUnit(UnitNm);}
  ~TEpUnit(){}
  TEpUnit(TSIn& SIn):
    UnitNm(SIn), OrgIdToFqH(SIn), ProjNmH(SIn),
    CountryNmToFqH(SIn){}
  static PEpUnit Load(TSIn& SIn){return new TEpUnit(SIn);}
  void Save(TSOut& SOut){
    UnitNm.Save(SOut); OrgIdToFqH.Save(SOut); ProjNmH.Save(SOut);
    CountryNmToFqH.Save(SOut);}

  TEpUnit& operator=(const TEpUnit& EpUnit){Fail; return *this;}

  // component retrieval & update
  TStr GetUnitNm() const {return UnitNm;}

  void AddOrg(const TStr& OrgNm, const TEpBs* EpBs);
  int GetOrgs() const {return OrgIdToFqH.Len();}
  int GetOrgId(const int& OrgN) const {return OrgIdToFqH.GetKey(OrgN);}
  TStr GetOrgNm(const int& OrgN, const TEpBs* EpBs) const;
  int GetOrgFq(const int& OrgN) const {return OrgIdToFqH[OrgN];}
  bool IsOrg(const TStr& OrgNm, TInt& OrgFq, const TEpBs* EpBs) const;

  void AddProj(const TStr& ProjNm){ProjNmH.AddDat(ProjNm)++;}
  int GetProjs() const {return ProjNmH.Len();}
  TStr GetProjNm(const int& ProjN) const {return ProjNmH.GetKey(ProjN);}
  void GetProjNmV(TStrV& ProjNmV) const {
    ProjNmH.GetKeyV(ProjNmV); ProjNmV.Sort();}

  void AddCountry(const TStr& CountryNm){CountryNmToFqH.AddDat(CountryNm)++;}
};

/////////////////////////////////////////////////
// EuroProject-Base
ClassTP(TEpBs, PEpBs)//{
private:
  PSs Ss;
  THash<TStr, PEpOrg> NmToOrgH;
  THash<TStr, PEpCountry> NmToCountryH;
  THash<TStr, PEpOfficer> NmToOfficerH;
  THash<TStr, PEpProj> NmToProjH;
  THash<TStr, PEpDomain> NmToDomainH;
  THash<TStr, PEpUnit> NmToUnitH;
public:
  TEpBs(const PSs& _Ss): Ss(_Ss){}
  static PEpBs New(const PSs& Ss){return new TEpBs(Ss);}
  ~TEpBs(){}
  TEpBs(TSIn& SIn):
    Ss(SIn),
    NmToOrgH(SIn), NmToCountryH(SIn), NmToOfficerH(SIn),
    NmToProjH(SIn), NmToDomainH(SIn), NmToUnitH(SIn){}
  static PEpBs Load(TSIn& SIn){return new TEpBs(SIn);}
  void Save(TSOut& SOut){
    Ss.Save(SOut);
    NmToOrgH.Save(SOut); NmToCountryH.Save(SOut); NmToOfficerH.Save(SOut);
    NmToProjH.Save(SOut); NmToDomainH.Save(SOut); NmToUnitH.Save(SOut);}

  TEpBs& operator=(const TEpBs& EpBs){Fail; return *this;}

  // organization
  bool IsOrg(const TStr& OrgNm, PEpOrg& Org) const {
    return NmToOrgH.IsKeyGetDat(OrgNm, Org);}
  bool IsOrg(const TStr& OrgNm) const {
    PEpOrg Org; return IsOrg(OrgNm, Org);}
  void AddOrg(const PEpOrg& Org){
    NmToOrgH.AddDat(Org->GetOrgNm(), Org);}
  int GetOrgs() const {return NmToOrgH.Len();}
  PEpOrg GetOrg(const int& OrgId) const {return NmToOrgH[OrgId];}
  PEpOrg GetOrg(const TStr& OrgNm) const {return NmToOrgH.GetDat(OrgNm);}
  int GetOrgId(const TStr& OrgNm) const {return NmToOrgH.GetKeyId(OrgNm);}
  void GetOrgNmV(TStrV& OrgNmV) const {
    NmToOrgH.GetKeyV(OrgNmV); OrgNmV.Sort();}

  // country
  bool IsCountry(const TStr& CountryNm, PEpCountry& Country) const {
    return NmToCountryH.IsKeyGetDat(CountryNm, Country);}
  void AddCountry(const PEpCountry& Country){
    NmToCountryH.AddDat(Country->GetCountryNm(), Country);}
  int GetCountries() const {return NmToCountryH.Len();}
  int GetCountryN(const TStr& CountryNm) const {return NmToCountryH.GetKeyId(CountryNm);}
  PEpCountry GetCountry(const int& CountryN) const {return NmToCountryH[CountryN];}
  PEpCountry GetCountry(const TStr& CountryNm) const {return NmToCountryH.GetDat(CountryNm);}
  void GetCountryNmV(TStrV& CountryNmV) const {
    NmToCountryH.GetKeyV(CountryNmV); CountryNmV.Sort();}

  // officer
  bool IsOfficer(const TStr& OfficerNm, PEpOfficer& Officer) const {
    return NmToOfficerH.IsKeyGetDat(OfficerNm, Officer);}
  void AddOfficer(const PEpOfficer& Officer){
    NmToOfficerH.AddDat(Officer->GetOfficerNm(), Officer);}
  int GetOfficers() const {return NmToOfficerH.Len();}
  PEpOfficer GetOfficer(const int& OfficerN) const {return NmToOfficerH[OfficerN];}
  PEpOfficer GetOfficer(const TStr& OfficerNm) const {return NmToOfficerH.GetDat(OfficerNm);}
  void GetOfficerNmV(TStrV& OfficerNmV) const {
    NmToOfficerH.GetKeyV(OfficerNmV); OfficerNmV.Sort();}

  // project
  bool IsProj(const TStr& ProjNm, PEpProj& Proj) const {
    return NmToProjH.IsKeyGetDat(ProjNm, Proj);}
  bool IsProj(const TStr& ProjNm) const {
    PEpProj Proj; return IsProj(ProjNm, Proj);}
  void AddProj(const PEpProj& Proj){
    NmToProjH.AddDat(Proj->GetProjNm(), Proj);}
  int GetProjs() const {return NmToProjH.Len();}
  PEpProj GetProj(const int& ProjN) const {return NmToProjH[ProjN];}
  PEpProj GetProj(const TStr& ProjNm) const {return NmToProjH.GetDat(ProjNm);}
  void GetProjNmV(TStrV& ProjNmV) const {
    NmToProjH.GetKeyV(ProjNmV); ProjNmV.Sort();}

  // domain
  bool IsDomain(const TStr& DomainNm, PEpDomain& Domain) const {
    return NmToDomainH.IsKeyGetDat(DomainNm, Domain);}
  void AddDomain(const PEpDomain& Domain){
    NmToDomainH.AddDat(Domain->GetDomainNm(), Domain);}
  int GetDomains() const {return NmToDomainH.Len();}
  PEpDomain GetDomain(const int& DomainN) const {return NmToDomainH[DomainN];}
  PEpDomain GetDomain(const TStr& DomainNm) const {return NmToDomainH.GetDat(DomainNm);}
  void GetDomainNmV(TStrV& DomainNmV) const {
    NmToDomainH.GetKeyV(DomainNmV); DomainNmV.Sort();}

  // unit
  bool IsUnit(const TStr& UnitNm, PEpUnit& Unit) const {
    return NmToUnitH.IsKeyGetDat(UnitNm, Unit);}
  void AddUnit(const PEpUnit& Unit){
    NmToUnitH.AddDat(Unit->GetUnitNm(), Unit);}
  int GetUnits() const {return NmToUnitH.Len();}
  PEpUnit GetUnit(const int& UnitN) const {return NmToUnitH[UnitN];}
  PEpUnit GetUnit(const TStr& UnitNm) const {return NmToUnitH.GetDat(UnitNm);}
  static TStr GetUnitDNm(const TStr& UnitNm);
  void GetUnitNmV(TStrV& UnitNmV) const {
    NmToUnitH.GetKeyV(UnitNmV); UnitNmV.Sort();}

  // organization-analysis
  void GetOrgIdSetV(const int& OrgId,
   const int& Depth, TBoolV& OrgIdSetV, int& OrgIdSetVTrues) const;
  PMom GetDistMom(const int& OrgId) const;

  // selection
  static PEpBs GetSelEpBs(const PSs& Ss,
   const TStrV& SelOrgTypeNmV, const TStrV& SelCountryNmV,
   const TStrV& SelDomainNmV, const TStrV& SelUnitNmV,
   const TStrV& SelOfficerNmV, const TStrV& SelProjNmV);
  PEpBs GetSelEpBs(
   const TStrV& SelOrgTypeNmV, const TStrV& SelCountryNmV,
   const TStrV& SelDomainNmV, const TStrV& SelUnitNmV,
   const TStrV& SelOfficerNmV, const TStrV& SelProjNmV) const;
  PEpBs GetSelEpBs(
   const TStr& SelOrgTypeNm, const TStr& SelCountryNm,
   const TStr& SelDomainNm, const TStr& SelUnitNm,
   const TStr& SelOfficerNm, const TStr& SelProjNm) const;

  // files
  static PEpBs LoadTxt(const TStr& FNm);
  static PEpBs LoadBin(const TStr& FNm){
    TFIn FIn(FNm); return new TEpBs(FIn);}
  void SaveBin(const TStr& FNm){
    TFOut FOut(FNm); Save(FOut);}
  static PEpBs LoadBinOrTxt(const TStr& BinFNm, const TStr& TxtFNm);
  void LoadBriefOrgNm(const TStr& FNm) const;

  // analysis
  void SaveTxtCountryClb(const TStr& TxtFNm, const TStr& GraphFNm) const;
  void SaveCountryClbTree(const TStr& CountryNm, const TStr& TreeFNm);
  void SaveTxtCountryClbPerCap(
   const TStr& TxtFNm, const TStr& GraphFNm, const PCiaWFBBs& CiaWFBBs) const;
  void SaveTxtOrgClb(const TStr& FNm) const;
  void SaveTxtDomainClb(const TStr& TxtFNm, const TStr& GraphFNm) const;
  void SaveTxtUnitClb(const TStr& TxtFNm, const TStr& GraphFNm) const;
  void SaveTxtOrgHub(const TStr& CountryHubFNm, const TStr& DomainHubFNm,
   const TStr& UnitHubFNm) const;
  void SaveOrgConnectivity(const TStr& TxtFNm, const TStr& GraphFNm,
   const int& MnClbOrgFq) const;
  void SaveOrgConnectivity(const TStr& TxtFNm, const TStr& GraphFNm,
   const int& MnClbOrgFq, const int& MxClbOrgFq) const;
  void SaveOrgSmallWorld(const TStr& TxtFNm, const TStr& TreeFNm) const;
  void SaveOfficerStat(const TStr& FNm, const PBowDocBs& EpBowDocBs) const;
  void SaveOrgStat(const TStr& FNm, const PBowDocBs& EpBowDocBs) const;
  void SaveDomainStat(const TStr& FNm, const PBowDocBs& EpBowDocBs) const;
  void SaveUnitStat(const TStr& FNm, const PBowDocBs& EpBowDocBs) const;
  void SaveCountryStat(const TStr& FNm, const PBowDocBs& EpBowDocBs) const;
  void SaveTxtXmlGraph(const TStr& TxtFNm, const TStr& XmlFNm) const;
  void SaveOrgDistTree(const TStr& FNm, const TStr& OrgNmWc) const;
  void SaveDomainSim(const TStr& FNm, const PBowDocBs& EpBowDocBs,
   const double& SimSumPrc, const int& MxKws) const;
  void SaveUnitSim(const TStr& FNm, const PBowDocBs& EpBowDocBs,
   const double& SimSumPrc, const int& MxKws) const;
  void SaveOfficerSim(const TStr& GraphFNm, const PBowDocBs& EpBowDocBs,
   const double& SimSumPrc, const int& MxKws) const;
  void SaveOrgPowerDist(const TStr& TxtFNm) const;
  void SaveCountryProjects(const TStr& TxtFNm,
   const PCordisEuProjBs& EuProjBs, const TStr& CountryNm) const;
  void GetClbWgtProjNmPrV(
   TFltStrPrV& ClbWgtProjNmPrV, const int& DumpTopProjs=0) const;
  void GetBestClbWgtProjNmV(const int& BestProjs, TStrV& ProjNmV);
};

#endif
