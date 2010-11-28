/////////////////////////////////////////////////
// Includes
#include "euproj.h"
#include "graph.h"

/////////////////////////////////////////////////
// EuroProject-Organization
TStr TEpOrg::GetMultiLnOrgNm(
 const TStr& OrgNm, const int& MnLnLen, const char& NewLnCh){
  TChA OrgNmChA=OrgNm;
  TChA MultiLnOrgNmChA; int LnLen=0;
  for (int ChN=0; ChN<OrgNmChA.Len(); ChN++){
    if ((LnLen>MnLnLen)&&(OrgNmChA[ChN]==' ')){
      MultiLnOrgNmChA+=NewLnCh; LnLen=0;
    } else {
      MultiLnOrgNmChA+=OrgNmChA[ChN]; LnLen++;
    }
  }
  return MultiLnOrgNmChA;
}

TStr TEpOrg::GetClbOrgNm(const int& ClbOrgN, const TEpBs* EpBs) const {
  int ClbOrgId=ClbOrgIdToFqH.GetKey(ClbOrgN);
  return EpBs->GetOrg(ClbOrgId)->GetOrgNm();
}

int TEpOrg::GetClbOrgFq(const TStr& ClbOrgNm, const TEpBs* EpBs) const {
  int ClbOrgId=EpBs->GetOrgId(ClbOrgNm);
  if (ClbOrgIdToFqH.IsKey(ClbOrgId)){
    return ClbOrgIdToFqH.GetDat(ClbOrgId);}
  else {return 0;}
}

void TEpOrg::GetClbOrgProjNmV(
 const TStr& ClbOrgNm, TStrV& ProjNmV, const TEpBs* EpBs) const {
  int ClbOrgId=EpBs->GetOrgId(ClbOrgNm);
  ProjNmV=ClbOrgIdToProjNmVH.GetDat(ClbOrgId);
}

void TEpOrg::AddClbOrg(
 const TStr& ClbOrgNm, const TStr& ProjNm, const TEpBs* EpBs){
  int ClbOrgId=EpBs->GetOrgId(ClbOrgNm);
  ClbOrgIdToFqH.AddDat(ClbOrgId)++;
  ClbOrgIdToProjNmVH.AddDat(ClbOrgId).AddSorted(ProjNm);
  SumClbOrgFq++;
}

/////////////////////////////////////////////////
// EuroProject-Country
void TEpCountry::AddOrg(const TStr& OrgNm, const TEpBs* EpBs){
  int OrgId=EpBs->GetOrgId(OrgNm);
  OrgIdToFqH.AddDat(OrgId)++;
}

TStr TEpCountry::GetOrgNm(const int& OrgN, const TEpBs* EpBs) const {
  int OrgId=OrgIdToFqH.GetKey(OrgN);
  return EpBs->GetOrg(OrgId)->GetOrgNm();
}

/////////////////////////////////////////////////
// EuroProject-Officer
void TEpOfficer::AddOrg(const TStr& OrgNm, const TEpBs* EpBs){
  int OrgId=EpBs->GetOrgId(OrgNm);
  OrgIdToFqH.AddDat(OrgId)++;
}

/////////////////////////////////////////////////
// EuroProject-Project
void TEpProj::AddOrgNmRole(
 const TStr& OrgNm, const TStr& OrgRoleNm, const TEpBs* EpBs){
  int OrgId=EpBs->GetOrgId(OrgNm);
  OrgIdRoleKdV.AddMerged(TIntStrKd(OrgId, OrgRoleNm));
}

TStr TEpProj::GetRole(const TStr& OrgNm, const TEpBs* EpBs) const {
  int OrgId=EpBs->GetOrgId(OrgNm);
  return OrgIdRoleKdV.GetDat(TIntStrKd(OrgId)).Dat;
}

TStr TEpProj::GetOrgNm(const int& OrgN, const TEpBs* EpBs) const {
  int OrgId=OrgIdRoleKdV[OrgN].Key;
  return EpBs->GetOrg(OrgId)->GetOrgNm();
}

/////////////////////////////////////////////////
// EuroProject-Domain
void TEpDomain::AddOrg(const TStr& OrgNm, const TEpBs* EpBs){
  int OrgId=EpBs->GetOrgId(OrgNm);
  OrgIdToFqH.AddDat(OrgId)++;
}

TStr TEpDomain::GetOrgNm(const int& OrgN, const TEpBs* EpBs) const {
  int OrgId=OrgIdToFqH.GetKey(OrgN);
  return EpBs->GetOrg(OrgId)->GetOrgNm();
}

bool TEpDomain::IsOrg(const TStr& OrgNm, TInt& OrgFq, const TEpBs* EpBs) const {
  int OrgId=EpBs->GetOrgId(OrgNm);
  return OrgIdToFqH.IsKeyGetDat(OrgId, OrgFq);
}

/////////////////////////////////////////////////
// EuroProject-Unit
void TEpUnit::AddOrg(const TStr& OrgNm, const TEpBs* EpBs){
  int OrgId=EpBs->GetOrgId(OrgNm);
  OrgIdToFqH.AddDat(OrgId)++;
}

TStr TEpUnit::GetOrgNm(const int& OrgN, const TEpBs* EpBs) const {
  int OrgId=OrgIdToFqH.GetKey(OrgN);
  return EpBs->GetOrg(OrgId)->GetOrgNm();
}

bool TEpUnit::IsOrg(const TStr& OrgNm, TInt& OrgFq, const TEpBs* EpBs) const {
  int OrgId=EpBs->GetOrgId(OrgNm);
  return OrgIdToFqH.IsKeyGetDat(OrgId, OrgFq);
}

/////////////////////////////////////////////////
// EuroProject-Base
TStr TEpBs::GetUnitDNm(const TStr& UnitNm){
  if (UnitNm=="B1"){return "Health-Applications";}
  if (UnitNm=="B2"){return "Handicapped&Eldely";}
  if (UnitNm=="B3"){return "eGovernment";}
  if (UnitNm=="B4"){return "Environment";}
  if (UnitNm=="B5"){return "Transport&Tourism";}
  if (UnitNm=="C1"){return "eWork";}
  if (UnitNm=="C2"){return "eBusiness";}
  if (UnitNm=="C3"){return "eCommerce";}
  if (UnitNm=="C4"){return "Trust&Security";}
  if (UnitNm=="D1"){return "ePublishing";}
  if (UnitNm=="D2"){return "Cultural-Heritage";}
  if (UnitNm=="D3"){return "eLearning";}
  if (UnitNm=="D4"){return "Linguistics";}
  if (UnitNm=="E1"){return "Networks";}
  if (UnitNm=="E2"){return "Software";}
  if (UnitNm=="E3"){return "Multimedia&Displays";}
  if (UnitNm=="E4"){return "Mobile-Communications";}
  if (UnitNm=="E5"){return "Peripherals&Microsystems";}
  if (UnitNm=="E6"){return "Microelectronics";}
  if (UnitNm=="F1"){return "Future&Emerging-Technologies";}
  if (UnitNm=="F2"){return "Research-Networks";}
  return UnitNm;
}

void TEpBs::GetOrgIdSetV(const int& OrgId,
 const int& Depth, TBoolV& OrgIdSetV, int& OrgIdSetVTrues) const {
  int Orgs=GetOrgs();
  OrgIdSetV.Gen(Orgs);
  if (Depth==-1){return;}
  if (!OrgIdSetV[OrgId]){
    OrgIdSetV[OrgId]=true; OrgIdSetVTrues++;}
  if (Depth>0){
    PEpOrg Org=GetOrg(OrgId);
    int ClbOrgs=Org->GetClbOrgs();
    for (int ClbOrgN=0; ClbOrgN<ClbOrgs; ClbOrgN++){
      int ClbOrgId=Org->GetClbOrgId(ClbOrgN);
      TBoolV SubOrgIdSetV;
      GetOrgIdSetV(ClbOrgId, Depth-1, SubOrgIdSetV, OrgIdSetVTrues);
      for (int SubOrgId=0; SubOrgId<Orgs; SubOrgId++){
        OrgIdSetV[SubOrgId]=OrgIdSetV[SubOrgId]||SubOrgIdSetV[SubOrgId];
      }
    }
  }
}

PMom TEpBs::GetDistMom(const int& StartOrgId) const {
  PMom DistMom=TMom::New();
  TIntV OrgDistV(GetOrgs()); OrgDistV.PutAll(-1);
  TIntPrQ OpenOrgIdQ; OpenOrgIdQ.Push(TIntPr(StartOrgId, 0));
  OrgDistV[StartOrgId]=0;
  while (!OpenOrgIdQ.Empty()){
    // get org-id from queue
    int OrgId=OpenOrgIdQ.Top().Val1;
    int OrgDist=OpenOrgIdQ.Top().Val2;
    OpenOrgIdQ.Pop();
    IAssert(OrgDistV[OrgId]==OrgDist);
    PEpOrg Org=GetOrg(OrgId);
    int ClbOrgs=Org->GetClbOrgs();
    for (int ClbOrgN=0; ClbOrgN<ClbOrgs; ClbOrgN++){
      // get child org-id
      int ClbOrgId=Org->GetClbOrgId(ClbOrgN);
      // push child org-id if necessary
      if (OrgDistV[ClbOrgId]==-1){
        OpenOrgIdQ.Push(TIntPr(ClbOrgId, OrgDist+1));
        OrgDistV[ClbOrgId]=OrgDist+1;
        DistMom->Add(OrgDist+1);
      }
    }
  }
  DistMom->Def();
  return DistMom;
}

PEpBs TEpBs::GetSelEpBs(const PSs& Ss,
 const TStrV& SelOrgTypeNmV, const TStrV& SelCountryNmV,
 const TStrV& SelDomainNmV, const TStrV& SelUnitNmV,
 const TStrV& SelOfficerNmV, const TStrV& SelProjNmV){
  int ProjRefFldN=Ss->GetFldX("Project Ref");
  int ProjNmFldN=Ss->GetFldX("Acronym");
  int DomainFldN=Ss->GetFldX("Domain / Key Action");
  int UnitFldN=Ss->GetFldX("Unit");
  int OfficerFldN=Ss->GetFldX("PO");
  int OrgNmFldN=Ss->GetFldX("Legal Name");
  int OrgCountryFldN=Ss->GetFldX("Legal Country");
  int OrgTypeFldN=Ss->GetFldX("Type of organisation");
  int OrgRoleFldN=Ss->GetFldX("Participant role");
  
  // create euproj-base
  PEpBs EpBs=TEpBs::New(Ss);
  // traverse project-participation records
  int PPRecs=Ss->GetYLen();
  for (int PPRecN=1; PPRecN<PPRecs; PPRecN++){
    if (PPRecN%100==0){printf("%d/%d\r", PPRecN, PPRecs);}
    // get fields values
    TStr ProjRef=Ss->GetVal(ProjRefFldN, PPRecN);
    TStr ProjNm=Ss->GetVal(ProjNmFldN, PPRecN).GetTrunc();
    TStr DomainNm=Ss->GetVal(DomainFldN, PPRecN);
    TStr UnitNm=Ss->GetVal(UnitFldN, PPRecN);
    TStr OfficerNm=Ss->GetVal(OfficerFldN, PPRecN);
    TStr OrgNm=Ss->GetVal(OrgNmFldN, PPRecN);
    TStr CountryNm=Ss->GetVal(OrgCountryFldN, PPRecN);
    TStr OrgTypeNm=Ss->GetVal(OrgTypeFldN, PPRecN);
    TStr OrgRoleNm=Ss->GetVal(OrgRoleFldN, PPRecN);

    // selection
    if ((!SelOrgTypeNmV.Empty())&&(!SelOrgTypeNmV.IsIn(OrgTypeNm))){continue;}
    if ((!SelCountryNmV.Empty())&&(!SelCountryNmV.IsIn(CountryNm))){continue;}
    if ((!SelDomainNmV.Empty())&&(!SelDomainNmV.IsIn(DomainNm))){continue;}
    if ((!SelUnitNmV.Empty())&&(!SelUnitNmV.IsIn(UnitNm))){continue;}
    if ((!SelOfficerNmV.Empty())&&(!SelOfficerNmV.IsIn(OfficerNm))){continue;}
    if ((!SelProjNmV.Empty())&&(!SelProjNmV.IsIn(ProjNm))){continue;}

    // update organization
    PEpOrg Org;
    if (!EpBs->IsOrg(OrgNm, Org)){
      Org=TEpOrg::New(OrgNm, OrgTypeNm, CountryNm); EpBs->AddOrg(Org);}
    Org->AddProj(ProjNm);

    // update country
    PEpCountry Country;
    if (!EpBs->IsCountry(CountryNm, Country)){
      Country=TEpCountry::New(CountryNm); EpBs->AddCountry(Country);}
    Country->AddOrg(OrgNm, EpBs());
    Country->AddOrgType(OrgTypeNm);
    Country->AddProj(ProjNm);

    // update officer
    PEpOfficer Officer;
    if (!EpBs->IsOfficer(OfficerNm, Officer)){
      Officer=TEpOfficer::New(OfficerNm); EpBs->AddOfficer(Officer);}
    Officer->AddOrg(OrgNm, EpBs());
    Officer->AddProj(ProjNm);
    Officer->AddCountry(CountryNm);

    // update project
    PEpProj Proj;
    if (!EpBs->IsProj(ProjNm, Proj)){
      Proj=TEpProj::New(ProjNm, ProjRef, DomainNm, UnitNm, OfficerNm);
      EpBs->AddProj(Proj);}
    Proj->AddOrgNmRole(OrgNm, OrgRoleNm, EpBs());

    // update domain
    PEpDomain Domain;
    if (!EpBs->IsDomain(DomainNm, Domain)){
      Domain=TEpDomain::New(DomainNm); EpBs->AddDomain(Domain);}
    Domain->AddOrg(OrgNm, EpBs());
    Domain->AddProj(ProjNm);
    Domain->AddCountry(CountryNm);

    // update unit
    PEpUnit Unit;
    if (!EpBs->IsUnit(UnitNm, Unit)){
      Unit=TEpUnit::New(UnitNm); EpBs->AddUnit(Unit);}
    Unit->AddOrg(OrgNm, EpBs());
    Unit->AddProj(ProjNm);
    Unit->AddCountry(CountryNm);
  }

  // update project collaboration
  printf("\n");
  int Projs=EpBs->GetProjs();
  for (int ProjN=0; ProjN<Projs; ProjN++){
    if (ProjN%1==0){printf("%d/%d\r", ProjN, Projs);}
    PEpProj Proj=EpBs->GetProj(ProjN);
    // traverse project organizations
    for (int MainOrgN=0; MainOrgN<Proj->GetOrgs(); MainOrgN++){
      // get organization & country data
      TStr MainOrgNm=Proj->GetOrgNm(MainOrgN, EpBs());
      PEpOrg MainOrg=EpBs->GetOrg(MainOrgNm);
      TStr MainCountryNm=MainOrg->GetCountryNm();
      PEpCountry MainCountry=EpBs->GetCountry(MainCountryNm);
      // traverse collaboration organizations
      for (int ClbOrgN=0; ClbOrgN<Proj->GetOrgs(); ClbOrgN++){
        // skip if same organization
        if (MainOrgN==ClbOrgN){continue;}
        // get organization & country data
        TStr ClbOrgNm=Proj->GetOrgNm(ClbOrgN, EpBs());
        PEpOrg ClbOrg=EpBs->GetOrg(ClbOrgNm);
        TStr ClbCountryNm=ClbOrg->GetCountryNm();
        // add collaboration organization & country
        MainOrg->AddClbOrg(ClbOrgNm, Proj->GetProjNm(), EpBs());
        MainCountry->AddClbCountry(ClbCountryNm);
      }
    }
  }
  printf("\n");

  // return euproj-base
  return EpBs;
}

PEpBs TEpBs::GetSelEpBs(
 const TStr& SelOrgTypeNm, const TStr& SelCountryNm,
 const TStr& SelDomainNm, const TStr& SelUnitNm,
 const TStr& SelOfficerNm, const TStr& SelProjNm) const {
  TStrV SelOrgTypeNmV; TStrV SelCountryNmV; TStrV SelDomainNmV;
  TStrV SelUnitNmV; TStrV SelOfficerNmV; TStrV SelProjNmV;
  if (!SelOrgTypeNm.Empty()){SelOrgTypeNmV.Add(SelOrgTypeNm);}
  if (!SelCountryNm.Empty()){SelCountryNmV.Add(SelCountryNm);}
  if (!SelDomainNm.Empty()){SelDomainNmV.Add(SelDomainNm);}
  if (!SelUnitNm.Empty()){SelUnitNmV.Add(SelUnitNm);}
  if (!SelOfficerNm.Empty()){SelOfficerNmV.Add(SelOfficerNm);}
  if (!SelProjNm.Empty()){SelProjNmV.Add(SelProjNm);}
  return TEpBs::GetSelEpBs(Ss,
   SelOrgTypeNmV, SelCountryNmV, SelDomainNmV,
   SelUnitNmV, SelOfficerNmV, SelProjNmV);
}

PEpBs TEpBs::GetSelEpBs(
 const TStrV& SelOrgTypeNmV, const TStrV& SelCountryNmV,
 const TStrV& SelDomainNmV, const TStrV& SelUnitNmV,
 const TStrV& SelOfficerNmV, const TStrV& SelProjNmV) const {
  return TEpBs::GetSelEpBs(
   Ss, SelOrgTypeNmV, SelCountryNmV,
   SelDomainNmV, SelUnitNmV,
   SelOfficerNmV, SelProjNmV);
}

PEpBs TEpBs::LoadTxt(const TStr& FNm){
  // load file
  PSs Ss=TSs::LoadTxt(ssfTabSep, FNm);
  // create base
  PEpBs EpBs=EpBs->GetSelEpBs(Ss,
   TStrV(), TStrV(),
   TStrV(), TStrV(),
   TStrV(), TStrV());
  // return base
  return EpBs;
}

PEpBs TEpBs::LoadBinOrTxt(const TStr& BinFNm, const TStr& TxtFNm){
  printf("Loading files ...\n");
  PEpBs EpBs;
  if (TFile::Exists(BinFNm)){
    EpBs=TEpBs::LoadBin(BinFNm);
  } else {
    EpBs=LoadTxt(TxtFNm);
    EpBs->SaveBin(BinFNm);
  }
  printf("Done.\n");
  return EpBs;
}

void TEpBs::LoadBriefOrgNm(const TStr& FNm) const {
  if (!TFile::Exists(FNm)){
    printf("Brief-Organization-Names file doesn't exist (%s)\n", FNm.CStr());
    return;
  }
  PSIn SIn=TFIn::New(FNm);
  TILx Lx(SIn, TFSet()|iloRetEoln);
  Lx.GetSym(syQStr, syEoln, syEof);
  while (Lx.Sym!=syEof){
    if (Lx.Sym!=syEoln){
      // read brief & full org-name
      TStr BriefOrgNm=Lx.Str;
      TStr OrgNm=Lx.GetQStr();
      Lx.GetSym(syEoln);
      // put brief-org-name
      PEpOrg Org;
      if (IsOrg(OrgNm, Org)){
        Org->PutBriefOrgNm(BriefOrgNm);
      }
    }
    Lx.GetSym(syQStr, syEoln, syEof);
  }
}

void TEpBs::SaveTxtXmlGraph(const TStr& TxtFNm, const TStr& XmlFNm) const {
  printf("Save Txt/Xml Graph to %s/%s ...\n", TxtFNm.CStr(), XmlFNm.CStr());
  // create files
  TFOut TxtFOut(TxtFNm); FILE* fTxt=TxtFOut.GetFileId();
  TFOut XmlFOut(XmlFNm); FILE* fXml=XmlFOut.GetFileId();
  fprintf(fXml, "<CollaborationGraph>\n");
  // output organizations
  fprintf(fTxt, "*organizations\t%d\n", GetOrgs());
  for (int OrgId=0; OrgId<GetOrgs(); OrgId++){
    PEpOrg Org=GetOrg(OrgId);
    // text
    fprintf(fTxt, "%d", OrgId);
    fprintf(fTxt, "\t%s", Org->GetOrgNm().CStr());
    fprintf(fTxt, "\t%s", Org->GetCountryNm().CStr());
    fprintf(fTxt, "\t%s\n", Org->GetOrgTypeNm().CStr());
    fprintf(fXml, "<Org>");
    // xml
    fprintf(fXml, " <Id>%d</Id>", OrgId);
    fprintf(fXml, " <Name>%s</Name>", TXmlLx::GetXmlStrFromPlainStr(Org->GetOrgNm()).CStr());
    fprintf(fXml, " <Country>%s</Country>", TXmlLx::GetXmlStrFromPlainStr(Org->GetCountryNm()).CStr());
    fprintf(fXml, " <Type>%s</Type>", TXmlLx::GetXmlStrFromPlainStr(Org->GetOrgTypeNm()).CStr());
    fprintf(fXml, " </Org>\n");
  }
  // count collaborations
  int Clbs=0;
  for (int SrcOrgId=0; SrcOrgId<GetOrgs(); SrcOrgId++){
    PEpOrg SrcOrg=GetOrg(SrcOrgId);
    for (int ProjN=0; ProjN<SrcOrg->GetProjs(); ProjN++){
      TStr ProjNm=SrcOrg->GetProjNm(ProjN);
      PEpProj Proj=GetProj(ProjNm);
      for (int DstOrgN=0; DstOrgN<Proj->GetOrgs(); DstOrgN++){
        int DstOrgId=Proj->GetOrgId(DstOrgN);
        if (SrcOrgId>=DstOrgId){continue;}
        Clbs++;
      }
    }
  }
  // output collaborations
  fprintf(fTxt, "*collaborations\t%d\n", Clbs);
  {for (int SrcOrgId=0; SrcOrgId<GetOrgs(); SrcOrgId++){
    PEpOrg SrcOrg=GetOrg(SrcOrgId);
    for (int ProjN=0; ProjN<SrcOrg->GetProjs(); ProjN++){
      TStr ProjNm=SrcOrg->GetProjNm(ProjN);
      PEpProj Proj=GetProj(ProjNm);
      TStr DomainNm=GetProj(ProjNm)->GetDomainNm();
      TStr UnitNm=GetProj(ProjNm)->GetUnitNm();
      for (int DstOrgN=0; DstOrgN<Proj->GetOrgs(); DstOrgN++){
        int DstOrgId=Proj->GetOrgId(DstOrgN);
        if (SrcOrgId>=DstOrgId){continue;}
        // text
        fprintf(fTxt, "%d", SrcOrgId);
        fprintf(fTxt, "\t%d", DstOrgId);
        fprintf(fTxt, "\t%s", ProjNm.CStr());
        fprintf(fTxt, "\t%s", DomainNm.CStr());
        fprintf(fTxt, "\t%s\n", UnitNm.CStr());
        // xml
        fprintf(fXml, "<Clb>");
        fprintf(fXml, " <SrcId>%d</SrcId>", SrcOrgId);
        fprintf(fXml, " <DstId>%d</DstId>", DstOrgId);
        fprintf(fXml, " <Proj>%s</Proj>", TXmlLx::GetXmlStrFromPlainStr(ProjNm).CStr());
        fprintf(fXml, " <Domain>%s</Domain>", TXmlLx::GetXmlStrFromPlainStr(DomainNm).CStr());
        fprintf(fXml, " <Unit>%s</Unit>", TXmlLx::GetXmlStrFromPlainStr(UnitNm).CStr());
        fprintf(fXml, " </Clb>\n");
      }
    }
  }}
  fprintf(fXml, "</CollaborationGraph>\n");
  printf("Done.\n");
}

void TEpBs::SaveTxtCountryClb(const TStr& TxtFNm, const TStr& GraphFNm) const {
  printf("Save Country Collaboration to %s/%s ...\n", TxtFNm.CStr(), GraphFNm.CStr());
  // prepare frequency/country sorted pairs
  typedef TPair<TInt, PEpCountry> TFqCountryPr;
  TVec<TFqCountryPr> FqCountryPrV;
  TStrPrIntH CountryPrFqH;
  for (int CountryN=0; CountryN<GetCountries(); CountryN++){
    PEpCountry Country=GetCountry(CountryN);
    TStr CountryNm=Country->GetCountryNm();
    FqCountryPrV.Add(TFqCountryPr(Country->GetSumClbCountriesFq(), Country));
    // collect frequency/country-name pairs
    for (int ClbCountryN=0; ClbCountryN<Country->GetClbCountries(); ClbCountryN++){
      TStr ClbCountryNm=Country->GetClbCountryNm(ClbCountryN);
      int ClbCountryFq=Country->GetClbCountryFq(ClbCountryN);
      CountryPrFqH.AddDat(TStrPr(CountryNm, ClbCountryNm))+=ClbCountryFq;
    }
  }

  // create output file
  TFOut FOut(TxtFNm); FILE* fTxtOut=FOut.GetFileId();

  // output data for all countries
  TIntStrPrPrV FqCountryPrPrV; CountryPrFqH.GetDatKeyPrV(FqCountryPrPrV);
  FqCountryPrPrV.Sort(false);
  fprintf(fTxtOut, "*** All Countries Collaborations:\n");
  for (int CountryPrN=0; CountryPrN<FqCountryPrPrV.Len(); CountryPrN++){
    TStr CountryNm1=FqCountryPrPrV[CountryPrN].Val2.Val1;
    TStr CountryNm2=FqCountryPrPrV[CountryPrN].Val2.Val2;
    int Fq=FqCountryPrPrV[CountryPrN].Val1;
    if ((CountryNm1>CountryNm2)||(Fq<5)){continue;}
    fprintf(fTxtOut, "%6d: %s - %s\n", Fq, CountryNm1.CStr(), CountryNm2.CStr());
  }
  fprintf(fTxtOut, "\n");

  // output data per country
  FqCountryPrV.Sort(false);
  {for (int CountryN=0; CountryN<FqCountryPrV.Len(); CountryN++){
    PEpCountry Country=FqCountryPrV[CountryN].Val2;
    // collect frequency-percent/country-name pairs
    TIntStrPrV FqClbCountryNmPrV;
    Country->GetFqClbCountryNmPrV(FqClbCountryNmPrV);
    FqClbCountryNmPrV.Sort(false);
    // output
    fprintf(fTxtOut, "*** %s (%d collaborations) (rank %d/%d):\n",
     Country->GetCountryNm().CStr(), Country->GetSumClbCountriesFq(),
     1+CountryN, FqCountryPrV.Len());
    for (int SubCountryN=0; SubCountryN<FqClbCountryNmPrV.Len(); SubCountryN++){
      int Fq=FqClbCountryNmPrV[SubCountryN].Val1;
      double FqPrc=double(Fq)/Country->GetSumClbCountriesFq();
      TStr SubCountryNm=FqClbCountryNmPrV[SubCountryN].Val2;
      if (FqPrc<0.001){continue;}
      fprintf(fTxtOut, "   %2d. %s (%.2f%% / %d)\n",
       1+SubCountryN, SubCountryNm.CStr(), 100*FqPrc, Fq);
    }
    fprintf(fTxtOut, "\n");
  }}

  // output graph
  PGraph Graph=TGraph::New();
  // create vertices
  TStrH CountryNmH;
  {for (int CountryPrN=0; CountryPrN<FqCountryPrPrV.Len(); CountryPrN++){
    if (CountryNmH.Len()>11){break;}
    TStr CountryNm1=FqCountryPrPrV[CountryPrN].Val2.Val1;
    TStr CountryNm2=FqCountryPrPrV[CountryPrN].Val2.Val2;
    int Fq=FqCountryPrPrV[CountryPrN].Val1;
    if (CountryNm1<=CountryNm2){continue;}
    // create vertices
    if (!CountryNmH.IsKey(CountryNm1)){
      PEpCountry Country1=GetCountry(CountryNm1);
      int CountryN1=GetCountryN(CountryNm1);
      CountryNmH.AddKey(CountryNm1);
      TStr VNm=CountryNm1+"\n("+TInt::GetStr(Country1->GetProjs())+"proj.)";
      Graph->AddVrtx(TGVrtx::New(CountryN1, VNm));
    }
    if (!CountryNmH.IsKey(CountryNm2)){
      PEpCountry Country2=GetCountry(CountryNm2);
      int CountryN2=GetCountryN(CountryNm2);
      CountryNmH.AddKey(CountryNm2);
      TStr VNm=CountryNm2+"\n("+TInt::GetStr(Country2->GetProjs())+"proj.)";
      Graph->AddVrtx(TGVrtx::New(CountryN2, VNm));
    }
    // create edges
    PVrtx Vrtx1=Graph->GetVrtx(GetCountryN(CountryNm1));
    PVrtx Vrtx2=Graph->GetVrtx(GetCountryN(CountryNm2));
    PEdge Edge=TGEdge::New(Vrtx1, Vrtx2, TInt::GetStr(Fq), false);
    Edge->PutWgt(1+log(Fq));
    Graph->AddEdge(Edge);
  }}
  Graph->SetEdgeWidth(5);
  Graph->SaveTxt(GraphFNm);
  printf("Done.\n");
}

void TEpBs::SaveCountryClbTree(const TStr& CountryNm, const TStr& GraphFNm){
  // create tree
  // create graph
  PGraph Graph=TGraph::New();
  // create root note
  int RootCountryId=GetCountryN(CountryNm);
  Graph->AddVrtx(TGVrtx::New(RootCountryId, CountryNm));
  // create distance vector
  TIntV CountryDistV(GetCountries()); CountryDistV.PutAll(-1);
  CountryDistV[RootCountryId]=0;
  // create queue
  TIntPrQ OpenCountryIdDistPrQ; OpenCountryIdDistPrQ.Push(TIntPr(RootCountryId, 0));
  while (!OpenCountryIdDistPrQ.Empty()){
    // get country-id from queue
    int CountryId=OpenCountryIdDistPrQ.Top().Val1;
    int CountryDist=OpenCountryIdDistPrQ.Top().Val2;
    OpenCountryIdDistPrQ.Pop();
    IAssert(CountryDistV[CountryId]==CountryDist);
    // get country data
    PEpCountry Country=GetCountry(CountryId);
    TStr CountryNm=Country->GetCountryNm();
    printf("[%s:%d] ", CountryNm.CStr(), CountryDist);
    // check distance
    if (CountryDist>0){continue;}
    // get country vertex
    PVrtx SrcVrtx=Graph->GetVrtx(CountryId);
    // get country children
    TIntStrPrV FqClbCountryNmPrV;
    Country->GetFqClbCountryNmPrV(FqClbCountryNmPrV);
    FqClbCountryNmPrV.Sort(false);
    int SubCountries=FqClbCountryNmPrV.Len();
    // traverse country children
    int CreatedSubCountries=0;
    for (int SubCountryN=0; SubCountryN<SubCountries; SubCountryN++){
      // get child data
      int SubCountryFq=FqClbCountryNmPrV[SubCountryN].Val1;
      TStr SubCountryNm=FqClbCountryNmPrV[SubCountryN].Val2;
      int SubCountryId=GetCountryN(SubCountryNm);
      // push child country-id if necessary
      if ((CountryDistV[SubCountryId]==-1)&&(!SubCountryNm.IsStrIn("EMPTY"))){
        // check number of subnodes
        int MxCreatedSubCountries=0;
        switch (CountryDist){
          case 0: MxCreatedSubCountries=70; break;
          case 1: MxCreatedSubCountries=4; break;
          case 2: MxCreatedSubCountries=2; break;
          case 3: MxCreatedSubCountries=1; break;
          case 4: MxCreatedSubCountries=1; break;
          default: MxCreatedSubCountries=0; break;
        }
        // check if stop creating branches
        CreatedSubCountries++;
        if (CreatedSubCountries>MxCreatedSubCountries){break;}
        // push edge
        OpenCountryIdDistPrQ.Push(TIntPr(SubCountryId, CountryDist+1));
        CountryDistV[SubCountryId]=CountryDist+1;
        // create vertex
        TStr VNm=SubCountryNm;
        PVrtx DstVrtx=TGVrtx::New(SubCountryId, VNm);
        Graph->AddVrtx(DstVrtx);
        // create edge
        //TStr ENm=TStr("_")+TInt::GetStr(CountryId)+"-"+TInt::GetStr(SubCountryId);
        TStr ENm=TInt::GetStr(SubCountryFq);
        PEdge Edge=TGEdge::New(SrcVrtx, DstVrtx, ENm);
        Edge->PutWgt(1+log(SubCountryFq));
        Graph->AddEdge(Edge);
      }
    }
  }
  Graph->SetEdgeWidth(5);
  // save tree
  Graph->SaveTxt(GraphFNm);
}

void TEpBs::SaveTxtCountryClbPerCap(
 const TStr& TxtFNm, const TStr& GraphFNm, const PCiaWFBBs& CiaWFBBs) const {
  printf("Save Country Collaboration Per Capita to %s/%s ...\n",
   TxtFNm.CStr(), GraphFNm.CStr());
  // prepare frequency/country sorted pairs
  typedef TPair<TFlt, PEpCountry> TWgtCountryPr;
  TVec<TWgtCountryPr> WgtCountryPrV;
  TStrPrFltH CountryPrWgtH;
  TStrPrIntH CountryPrFqH;
  for (int CountryN=0; CountryN<GetCountries(); CountryN++){
    PEpCountry Country=GetCountry(CountryN);
    TStr CountryNm=Country->GetCountryNm();
    PCiaWFBCountry CiaWFBCountry;
    if (CiaWFBBs->IsCountry(CountryNm, CiaWFBCountry)){
      double Population=CiaWFBCountry->GetFldValNum("Population");
      // collect frequency/country-name pairs
      for (int ClbCountryN=0; ClbCountryN<Country->GetClbCountries(); ClbCountryN++){
        TStr ClbCountryNm=Country->GetClbCountryNm(ClbCountryN);
        PCiaWFBCountry ClbCiaWFBCountry;
        if (CiaWFBBs->IsCountry(ClbCountryNm, ClbCiaWFBCountry)){
          double ClbPopulation=ClbCiaWFBCountry->GetFldValNum("Population");
          int ClbCountryFq=Country->GetClbCountryFq(ClbCountryN);
          double ClbCountryWgt;
          if (CountryNm==ClbCountryNm){ClbCountryWgt=ClbCountryFq/ClbPopulation;}
          else {ClbCountryWgt=ClbCountryFq/(ClbPopulation+Population);}
          CountryPrWgtH.AddDat(TStrPr(CountryNm, ClbCountryNm))+=ClbCountryWgt;
          CountryPrFqH.AddDat(TStrPr(CountryNm, ClbCountryNm))+=ClbCountryFq;
        }
      }
    }
  }

  // create output file
  TFOut FOut(TxtFNm); FILE* fTxtOut=FOut.GetFileId();

  // output data for all countries
  TFltStrPrPrV WgtCountryPrPrV; CountryPrWgtH.GetDatKeyPrV(WgtCountryPrPrV);
  WgtCountryPrPrV.Sort(false);
  fprintf(fTxtOut, "*** All Countries Collaborations Per Capita:\n");
  fprintf(fTxtOut, "[Rank] [Population-Per-Collaboration] [Collaborations] [Country-Country] [Population]\n");
  {int RankN=0;
  for (int CountryPrN=0; CountryPrN<WgtCountryPrPrV.Len(); CountryPrN++){
    TStr CountryNm1=WgtCountryPrPrV[CountryPrN].Val2.Val1;
    TStr CountryNm2=WgtCountryPrPrV[CountryPrN].Val2.Val2;
    double Wgt=WgtCountryPrPrV[CountryPrN].Val1;
    int Fq=CountryPrFqH.GetDat(TStrPr(CountryNm1, CountryNm2));
    double Population1=CiaWFBBs->GetCountry(CountryNm1)->GetFldValNum("Population");
    double Population2=CiaWFBBs->GetCountry(CountryNm2)->GetFldValNum("Population");
    if ((CountryNm1>CountryNm2)||(Wgt<0)){continue;}
    if (CountryNm1==CountryNm2){
      Wgt=Population1/Fq;
      fprintf(fTxtOut, "%3d. %6.0f ppc (%5d colab.): %s - %s   (pop.: %.0f)\n",
       ++RankN, Wgt, Fq, CountryNm1.CStr(), CountryNm2.CStr(), Population1);
    } else {
      Wgt=(Population1+Population2)/Fq;
      fprintf(fTxtOut, "%3d. %6.0f ppc (%5d colab.): %s - %s   (pop.: %.0f=%.0f+%.0f)\n",
       ++RankN, Wgt, Fq, CountryNm1.CStr(), CountryNm2.CStr(),
       Population1+Population2, Population1, Population2);
    }
  }}
  fprintf(fTxtOut, "\n");

  // output graph
  PGraph Graph=TGraph::New();
  // create vertices
  TStrH CountryNmH;
  for (int CountryPrN=0; CountryPrN<WgtCountryPrPrV.Len(); CountryPrN++){
    if (CountryNmH.Len()>12){break;}
    TStr CountryNm1=WgtCountryPrPrV[CountryPrN].Val2.Val1;
    TStr CountryNm2=WgtCountryPrPrV[CountryPrN].Val2.Val2;
    double Wgt=WgtCountryPrPrV[CountryPrN].Val1;
    if (CountryNm1<=CountryNm2){continue;}
    // create vertices
    if (!CountryNmH.IsKey(CountryNm1)){
      PEpCountry Country1=GetCountry(CountryNm1);
      int CountryN1=GetCountryN(CountryNm1);
      CountryNmH.AddKey(CountryNm1);
      TStr VNm=CountryNm1+"\n("+TInt::GetStr(Country1->GetProjs())+"proj.)";
      Graph->AddVrtx(TGVrtx::New(CountryN1, VNm));
    }
    if (!CountryNmH.IsKey(CountryNm2)){
      PEpCountry Country2=GetCountry(CountryNm2);
      int CountryN2=GetCountryN(CountryNm2);
      CountryNmH.AddKey(CountryNm2);
      TStr VNm=CountryNm2+"\n("+TInt::GetStr(Country2->GetProjs())+"proj.)";
      Graph->AddVrtx(TGVrtx::New(CountryN2, VNm));
    }
    // calculate weight
    int Fq=CountryPrFqH.GetDat(TStrPr(CountryNm1, CountryNm2));
    double Population1=CiaWFBBs->GetCountry(CountryNm1)->GetFldValNum("Population");
    double Population2=CiaWFBBs->GetCountry(CountryNm2)->GetFldValNum("Population");
    double PPCWgt=(Population1+Population2)/Fq;
    // create edges
    PVrtx Vrtx1=Graph->GetVrtx(GetCountryN(CountryNm1));
    PVrtx Vrtx2=Graph->GetVrtx(GetCountryN(CountryNm2));
    PEdge Edge=TGEdge::New(Vrtx1, Vrtx2, TFlt::GetStr(PPCWgt, "%0.fppc"), false);
    Edge->PutWgt(1+log(Wgt));
    Graph->AddEdge(Edge);
  }
  Graph->SetEdgeWidth(5);
  Graph->SaveTxt(GraphFNm);

  printf("Done.\n");
}

void TEpBs::SaveTxtOrgClb(const TStr& FNm) const {
  printf("Save Organization Collaboration to %s ...\n", FNm.CStr());
  // prepare frequency/country sorted pairs
  typedef TPair<TInt, PEpOrg> TFqOrgPr;
  TVec<TFqOrgPr> FqOrgPrV;
  TStrPrIntH OrgPrFqH;
  for (int OrgN=0; OrgN<GetOrgs(); OrgN++){
    PEpOrg Org=GetOrg(OrgN);
    TStr OrgNm=Org->GetOrgNm();
    FqOrgPrV.Add(TFqOrgPr(Org->GetSumClbOrgsFq(), Org));
    // collect frequency/organization-name pairs
    for (int ClbOrgN=0; ClbOrgN<Org->GetClbOrgs(); ClbOrgN++){
      TStr ClbOrgNm=Org->GetClbOrgNm(ClbOrgN, this);
      int ClbOrgFq=Org->GetClbOrgFq(ClbOrgN);
      OrgPrFqH.AddDat(TStrPr(OrgNm, ClbOrgNm))+=ClbOrgFq;
    }
  }

  // create output file
  TFOut FOut(FNm); FILE* fOut=FOut.GetFileId();

  // output data for all organizations
  TIntStrPrPrV FqOrgPrPrV; OrgPrFqH.GetDatKeyPrV(FqOrgPrPrV);
  FqOrgPrPrV.Sort(false);
  fprintf(fOut, "*** All Organizations Collaborations:\n");
  for (int OrgPrN=0; OrgPrN<FqOrgPrPrV.Len(); OrgPrN++){
    TStr OrgNm1=FqOrgPrPrV[OrgPrN].Val2.Val1;
    TStr OrgNm2=FqOrgPrPrV[OrgPrN].Val2.Val2;
    int Fq=FqOrgPrPrV[OrgPrN].Val1;
    if ((OrgNm1>OrgNm2)||(Fq<3)){continue;}
    fprintf(fOut, "%6d: '%s' - '%s'\n", Fq, OrgNm1.CStr(), OrgNm2.CStr());
  }
  fprintf(fOut, "\n");

  // output data per organization
  FqOrgPrV.Sort(false);
  {for (int OrgN=0; OrgN<FqOrgPrV.Len(); OrgN++){
    PEpOrg Org=FqOrgPrV[OrgN].Val2;
    // collect frequency-percent/country-name pairs
    TIntPrV FqClbOrgIdPrV;
    Org->GetFqClbOrgIdPrV(FqClbOrgIdPrV);
    FqClbOrgIdPrV.Sort(false);
    // output
    fprintf(fOut, "*** %s/%s (%d collaborations) (rank: %d/%d):\n",
     Org->GetOrgNm().CStr(), Org->GetCountryNm().CStr(), Org->GetSumClbOrgsFq(),
     1+OrgN, FqOrgPrV.Len());
    if (FqClbOrgIdPrV.Len()==0){
      fprintf(fOut, "   Projects: [");
      for (int ProjN=0; ProjN<Org->GetProjs(); ProjN++){
        TStr DomainNm=GetProj(Org->GetProjNm(ProjN))->GetDomainNm();
        TStr UnitNm=GetProj(Org->GetProjNm(ProjN))->GetUnitNm();
        fprintf(fOut, "'%s'/%s-%s ", Org->GetProjNm(ProjN).CStr(), DomainNm.CStr(), UnitNm.CStr());
      }
      fprintf(fOut, "]\n");
    } else {
      for (int ClbOrgN=0; ClbOrgN<FqClbOrgIdPrV.Len(); ClbOrgN++){
        int ClbOrgId=FqClbOrgIdPrV[ClbOrgN].Val2;
        TStr ClbOrgNm=GetOrg(ClbOrgId)->GetOrgNm();
        TStr ClbCountryNm=GetOrg(ClbOrgNm)->GetCountryNm();
        int Fq=Org->GetClbOrgFq(ClbOrgNm, this);
        double FqPrc=double(Fq)/Org->GetSumClbOrgsFq();
        TStrV ProjNmV; Org->GetClbOrgProjNmV(ClbOrgNm, ProjNmV, this);
        fprintf(fOut, "   %2d. %s/%s (%.2f%% / %d)     [",
         1+ClbOrgN, ClbOrgNm.CStr(), ClbCountryNm.CStr(), 100*FqPrc, Fq);
        for (int ProjN=0; ProjN<ProjNmV.Len(); ProjN++){
          TStr DomainNm=GetProj(ProjNmV[ProjN])->GetDomainNm();
          TStr UnitNm=GetProj(ProjNmV[ProjN])->GetUnitNm();
          fprintf(fOut, "'%s'/%s-%s ", ProjNmV[ProjN].CStr(), DomainNm.CStr(), UnitNm.CStr());
        }
        fprintf(fOut, "]\n");
      }
    }
    fprintf(fOut, "\n");
  }}
  printf("Done.\n");
}

void TEpBs::SaveTxtDomainClb(const TStr& TxtFNm, const TStr& GraphFNm) const {
  printf("Save Domain Collaboration to %s/%s ...\n", TxtFNm.CStr(), GraphFNm.CStr());
  TStrPrFltH DomainPrToWgtH;
  TStrPrFltH DomainPrToOrgsH;
  // traverse main domains
  for (int DomainN=0; DomainN<GetDomains(); DomainN++){
    PEpDomain Domain=GetDomain(DomainN);
    TStr DomainNm=Domain->GetDomainNm();
    if (DomainNm=="#EMPTY"){continue;}
    // traverse domain organizations
    for (int OrgN=0; OrgN<Domain->GetOrgs(); OrgN++){
      TStr OrgNm=Domain->GetOrgNm(OrgN, this);
      int OrgFq=Domain->GetOrgFq(OrgN);
      if (OrgNm=="#EMPTY"){continue;}
      // traverse sub domains
      for (int SubDomainN=0; SubDomainN<GetDomains(); SubDomainN++){
        PEpDomain SubDomain=GetDomain(SubDomainN);
        TStr SubDomainNm=SubDomain->GetDomainNm();
        if (SubDomainNm=="#EMPTY"){continue;}
        TInt SubOrgFq;
        if (SubDomain->IsOrg(OrgNm, SubOrgFq, this)){
          if (DomainNm==SubDomainNm){SubOrgFq--;}
          double ClbWgt=(OrgFq*SubOrgFq);
          if (ClbWgt>0){
            DomainPrToWgtH.AddDat(TStrPr(DomainNm, SubDomainNm))+=ClbWgt;}
          DomainPrToOrgsH.AddDat(TStrPr(DomainNm, SubDomainNm))++;
        }
      }
    }
  }
  // create output file
  TFOut TxtFOut(TxtFNm); FILE* fTxtOut=TxtFOut.GetFileId();
  // organizations
  // transform & sort
  TFltStrPrPrV OrgsDomainPrPrV;
  DomainPrToOrgsH.GetDatKeyPrV(OrgsDomainPrPrV);
  OrgsDomainPrPrV.Sort(false);
  // output
  fprintf(fTxtOut, "*** Number Of Organizations In Two Domains\n");
  int OrgsDomainRank=0;
  {for (int DomainPrN=0; DomainPrN<OrgsDomainPrPrV.Len(); DomainPrN++){
    TStr DomainNm1=OrgsDomainPrPrV[DomainPrN].Val2.Val1;
    TStr DomainNm2=OrgsDomainPrPrV[DomainPrN].Val2.Val2;
    double ClbOrgs=OrgsDomainPrPrV[DomainPrN].Val1;
    if (DomainNm1<DomainNm2){continue;}
    OrgsDomainRank++;
    fprintf(fTxtOut, "%3d. %6.0f: %s - %s\n",
     OrgsDomainRank, ClbOrgs, DomainNm1.CStr(), DomainNm2.CStr());
  }}
  fprintf(fTxtOut, "\n");
  // weights
  // transform & sort
  TFltStrPrPrV WgtDomainPrPrV;
  DomainPrToWgtH.GetDatKeyPrV(WgtDomainPrPrV);
  WgtDomainPrPrV.Sort(false);
  // output
  fprintf(fTxtOut, "*** Weighted Number Of Organizations In Two Domains\n");
  int WgtDomainRank=0;
  {for (int DomainPrN=0; DomainPrN<WgtDomainPrPrV.Len(); DomainPrN++){
    TStr DomainNm1=WgtDomainPrPrV[DomainPrN].Val2.Val1;
    TStr DomainNm2=WgtDomainPrPrV[DomainPrN].Val2.Val2;
    double ClbWgt=WgtDomainPrPrV[DomainPrN].Val1;
    if (DomainNm1<DomainNm2){continue;}
    WgtDomainRank++;
    fprintf(fTxtOut, "%3d. %6.0f: %s - %s\n",
     WgtDomainRank, ClbWgt, DomainNm1.CStr(), DomainNm2.CStr());
  }}
  fprintf(fTxtOut, "\n");

  // create & save collaboration graph
  PGraph Graph=TGraph::New();
  // create vertices
  {for (int DomainN=0; DomainN<GetDomains(); DomainN++){
    TStr DomainNm=GetDomain(DomainN)->GetDomainNm();
    if (DomainNm=="#EMPTY"){continue;}
    Graph->AddVrtx(TGVrtx::New(DomainN, DomainNm));
  }}
  // calculate properties
  double SumGraphClbOrgs=0;
  {for (int DomainPrN=0; DomainPrN<OrgsDomainPrPrV.Len(); DomainPrN++){
    TStr DomainNm1=OrgsDomainPrPrV[DomainPrN].Val2.Val1;
    TStr DomainNm2=OrgsDomainPrPrV[DomainPrN].Val2.Val2;
    double ClbOrgs=OrgsDomainPrPrV[DomainPrN].Val1;
    if (DomainNm1<=DomainNm2){continue;}
    SumGraphClbOrgs+=ClbOrgs;
  }}
  double MnGraphClbOrgs=TFlt::Mx; double MxGraphClbOrgs=TFlt::Mn;
  double SumGraphClbOrgsSF=0; int MxDomainPrN=0;
  {for (int DomainPrN=0; DomainPrN<OrgsDomainPrPrV.Len(); DomainPrN++){
    TStr DomainNm1=OrgsDomainPrPrV[DomainPrN].Val2.Val1;
    TStr DomainNm2=OrgsDomainPrPrV[DomainPrN].Val2.Val2;
    double ClbOrgs=OrgsDomainPrPrV[DomainPrN].Val1;
    if (DomainNm1<=DomainNm2){continue;}
    SumGraphClbOrgsSF+=ClbOrgs;
    if (SumGraphClbOrgsSF>SumGraphClbOrgs*0.75){break;}
    MnGraphClbOrgs=TFlt::GetMn(MnGraphClbOrgs, ClbOrgs);
    MxGraphClbOrgs=TFlt::GetMx(MxGraphClbOrgs, ClbOrgs);
    MxDomainPrN=DomainPrN;
  }}
  // create edges
  {for (int DomainPrN=0; DomainPrN<=MxDomainPrN; DomainPrN++){
    TStr DomainNm1=OrgsDomainPrPrV[DomainPrN].Val2.Val1;
    TStr DomainNm2=OrgsDomainPrPrV[DomainPrN].Val2.Val2;
    double ClbOrgs=OrgsDomainPrPrV[DomainPrN].Val1;
    if (DomainNm1<=DomainNm2){continue;}
    PVrtx Vrtx1=Graph->GetVrtx(DomainNm1);
    PVrtx Vrtx2=Graph->GetVrtx(DomainNm2);
    PEdge Edge=TGEdge::New(Vrtx1, Vrtx2, TFlt::GetStr(ClbOrgs, "%.0f"), false);
    Edge->PutWgt(ClbOrgs);
    int Width=1+10*((ClbOrgs-MnGraphClbOrgs)/(MxGraphClbOrgs-MnGraphClbOrgs+1));
    Edge->PutWidth(Width);
    Graph->AddEdge(Edge);
  }}
  Graph->SaveTxt(GraphFNm);
  printf("Done.\n");
}

void TEpBs::SaveTxtUnitClb(const TStr& TxtFNm, const TStr& GraphFNm) const {
  printf("Save Unit Collaboration to %s/%s ...\n", TxtFNm.CStr(), GraphFNm.CStr());
  int Units=GetUnits();
  TStrPrFltH UnitPrToWgtH;
  TStrPrFltH UnitPrToOrgsH;
  // traverse main domains
  for (int UnitN=0; UnitN<Units; UnitN++){
    PEpUnit Unit=GetUnit(UnitN);
    TStr UnitNm=Unit->GetUnitNm();
    // traverse domain organizations
    for (int OrgN=0; OrgN<Unit->GetOrgs(); OrgN++){
      TStr OrgNm=Unit->GetOrgNm(OrgN, this);
      int OrgFq=Unit->GetOrgFq(OrgN);
      // traverse sub domains
      for (int SubUnitN=0; SubUnitN<Units; SubUnitN++){
        PEpUnit SubUnit=GetUnit(SubUnitN);
        TStr SubUnitNm=SubUnit->GetUnitNm();
        TInt SubOrgFq;
        if (SubUnit->IsOrg(OrgNm, SubOrgFq, this)){
          if (UnitNm==SubUnitNm){SubOrgFq--;}
          double ClbWgt=(OrgFq*SubOrgFq);
          if (ClbWgt>0){
            UnitPrToWgtH.AddDat(TStrPr(UnitNm, SubUnitNm))+=ClbWgt;}
          UnitPrToOrgsH.AddDat(TStrPr(UnitNm, SubUnitNm))++;
        }
      }
    }
  }
  // create output file
  TFOut TxtFOut(TxtFNm); FILE* fTxtOut=TxtFOut.GetFileId();
  // organizations
  // transform & sort
  TFltStrPrPrV OrgsUnitPrPrV;
  UnitPrToOrgsH.GetDatKeyPrV(OrgsUnitPrPrV);
  OrgsUnitPrPrV.Sort(false);
  // output
  fprintf(fTxtOut, "*** Number Of Organizations In Two Units\n");
  {for (int UnitPrN=0; UnitPrN<OrgsUnitPrPrV.Len(); UnitPrN++){
    TStr UnitNm1=OrgsUnitPrPrV[UnitPrN].Val2.Val1;
    TStr UnitNm2=OrgsUnitPrPrV[UnitPrN].Val2.Val2;
    double ClbOrgs=OrgsUnitPrPrV[UnitPrN].Val1;
    if (UnitNm1<=UnitNm2){continue;}
    fprintf(fTxtOut, "%6.0f: %s - %s (%s - %s)\n", ClbOrgs,
     UnitNm1.CStr(), UnitNm2.CStr(),
     GetUnitDNm(UnitNm1).CStr(), GetUnitDNm(UnitNm2).CStr());
  }}
  fprintf(fTxtOut, "\n");
  // weights
  // transform & sort
  TFltStrPrPrV WgtUnitPrPrV;
  UnitPrToWgtH.GetDatKeyPrV(WgtUnitPrPrV);
  WgtUnitPrPrV.Sort(false);
  // output
  fprintf(fTxtOut, "*** Weighted-Number Of Organizations In Two Units\n");
  {for (int UnitPrN=0; UnitPrN<WgtUnitPrPrV.Len(); UnitPrN++){
    TStr UnitNm1=WgtUnitPrPrV[UnitPrN].Val2.Val1;
    TStr UnitNm2=WgtUnitPrPrV[UnitPrN].Val2.Val2;
    double ClbWgt=WgtUnitPrPrV[UnitPrN].Val1;
    if (UnitNm1<=UnitNm2){continue;}
    fprintf(fTxtOut, "%6.0f: %s - %s (%s - %s)\n", ClbWgt,
     UnitNm1.CStr(), UnitNm2.CStr(),
     GetUnitDNm(UnitNm1).CStr(), GetUnitDNm(UnitNm2).CStr());
  }}
  fprintf(fTxtOut, "\n");

  // create & save collaboration graph
  PGraph Graph=TGraph::New();
  // create vertices
  {for (int UnitN=0; UnitN<GetUnits(); UnitN++){
    TStr UnitNm=GetUnit(UnitN)->GetUnitNm();
    Graph->AddVrtx(TGVrtx::New(UnitN, UnitNm+"/"+GetUnitDNm(UnitNm)));
  }}
  // calculate properties
  double SumGraphClbOrgs=0;
  {for (int UnitPrN=0; UnitPrN<OrgsUnitPrPrV.Len(); UnitPrN++){
    TStr UnitNm1=OrgsUnitPrPrV[UnitPrN].Val2.Val1;
    TStr UnitNm2=OrgsUnitPrPrV[UnitPrN].Val2.Val2;
    double ClbOrgs=OrgsUnitPrPrV[UnitPrN].Val1;
    if (UnitNm1<=UnitNm2){continue;}
    SumGraphClbOrgs+=ClbOrgs;
  }}
  double MnGraphClbOrgs=TFlt::Mx; double MxGraphClbOrgs=TFlt::Mn;
  double SumGraphClbOrgsSF=0; int MxUnitPrN=0;
  {for (int UnitPrN=0; UnitPrN<OrgsUnitPrPrV.Len(); UnitPrN++){
    TStr UnitNm1=OrgsUnitPrPrV[UnitPrN].Val2.Val1;
    TStr UnitNm2=OrgsUnitPrPrV[UnitPrN].Val2.Val2;
    double ClbOrgs=OrgsUnitPrPrV[UnitPrN].Val1;
    if (UnitNm1<=UnitNm2){continue;}
    SumGraphClbOrgsSF+=ClbOrgs;
    if (SumGraphClbOrgsSF>SumGraphClbOrgs*0.2){break;}
    MnGraphClbOrgs=TFlt::GetMn(MnGraphClbOrgs, ClbOrgs);
    MxGraphClbOrgs=TFlt::GetMx(MxGraphClbOrgs, ClbOrgs);
    MxUnitPrN=UnitPrN;
  }}
  // create edges
  for (int UnitPrN=0; UnitPrN<=MxUnitPrN; UnitPrN++){
    TStr UnitNm1=OrgsUnitPrPrV[UnitPrN].Val2.Val1;
    TStr UnitNm2=OrgsUnitPrPrV[UnitPrN].Val2.Val2;
    double ClbOrgs=OrgsUnitPrPrV[UnitPrN].Val1;
    if (UnitNm1<=UnitNm2){continue;}
    PVrtx Vrtx1=Graph->GetVrtx(UnitNm1+"/"+GetUnitDNm(UnitNm1));
    PVrtx Vrtx2=Graph->GetVrtx(UnitNm2+"/"+GetUnitDNm(UnitNm2));
    PEdge Edge=TGEdge::New(Vrtx1, Vrtx2, TFlt::GetStr(ClbOrgs, "%.0f"), false);
    Edge->PutWgt(ClbOrgs);
    int Width=1+10*((ClbOrgs-MnGraphClbOrgs)/(MxGraphClbOrgs-MnGraphClbOrgs+1));
    Edge->PutWidth(Width);
    Graph->AddEdge(Edge);
  }
  Graph->SaveTxt(GraphFNm);
  printf("Done.\n");
}

void TEpBs::SaveTxtOrgHub(const TStr& CountryHubFNm, const TStr& DomainHubFNm,
 const TStr& UnitHubFNm) const {
  printf("Save Organization Hubs to %s/%s/%s ...\n",
   CountryHubFNm.CStr(), DomainHubFNm.CStr(), UnitHubFNm.CStr());
  // prepare frequency/country sorted pairs
  TStrStrIntKdVH OrgNmToCountryFqKdVH;
  TStrStrIntKdVH OrgNmToDomainFqKdVH;
  TStrStrIntKdVH OrgNmToUnitFqKdVH;
  for (int OrgN=0; OrgN<GetOrgs(); OrgN++){
    PEpOrg Org=GetOrg(OrgN);
    TStr OrgNm=Org->GetOrgNm();
    // collect country frequency
    for (int ClbOrgN=0; ClbOrgN<Org->GetClbOrgs(); ClbOrgN++){
      TStr ClbOrgNm=Org->GetClbOrgNm(ClbOrgN, this);
      int ClbOrgFq=Org->GetClbOrgFq(ClbOrgN);
      TStr ClbCountryNm=GetOrg(ClbOrgNm)->GetCountryNm();
      TStrIntKdV& CountryFqKdV=OrgNmToCountryFqKdVH.AddDat(OrgNm);
      CountryFqKdV.GetAddDat(TStrIntKd(ClbCountryNm)).Dat+=ClbOrgFq;
    }
    // collect domain & unit frequency
    for (int ProjN=0; ProjN<Org->GetProjs(); ProjN++){
      TStr ProjNm=Org->GetProjNm(ProjN);
      PEpProj Proj=GetProj(ProjNm);
      // domain
      TStr DomainNm=Proj->GetDomainNm();
      TStrIntKdV& DomainFqKdV=OrgNmToDomainFqKdVH.AddDat(OrgNm);
      DomainFqKdV.GetAddDat(TStrIntKd(DomainNm)).Dat++;
      // unit
      TStr UnitNm=Proj->GetUnitNm();
      TStrIntKdV& UnitFqKdV=OrgNmToUnitFqKdVH.AddDat(OrgNm);
      UnitFqKdV.GetAddDat(TStrIntKd(UnitNm)).Dat++;
    }
  }

  // collect weights
  TFltStrPrV CountryWgtOrgPrV;
  TFltStrPrV DomainWgtOrgPrV;
  TFltStrPrV UnitWgtOrgPrV;
  {for (int OrgN=0; OrgN<GetOrgs(); OrgN++){
    PEpOrg Org=GetOrg(OrgN);
    TStr OrgNm=Org->GetOrgNm();
    // country weights
    if (OrgNmToCountryFqKdVH.IsKey(OrgNm)){
      TStrIntKdV& CountryFqKdV=OrgNmToCountryFqKdVH.GetDat(OrgNm);
      double Wgt=0;
      for (int CountryN=0; CountryN<CountryFqKdV.Len(); CountryN++){
        int Fq=CountryFqKdV[CountryN].Dat;
        if (Fq>0){Wgt+=1+log(Fq);}
      }
      CountryWgtOrgPrV.Add(TFltStrPr(Wgt, OrgNm));
    }
    // domain weights
    if (OrgNmToDomainFqKdVH.IsKey(OrgNm)){
      TStrIntKdV& DomainFqKdV=OrgNmToDomainFqKdVH.GetDat(OrgNm);
      double Wgt=0;
      for (int DomainN=0; DomainN<DomainFqKdV.Len(); DomainN++){
        int Fq=DomainFqKdV[DomainN].Dat;
        if (Fq>0){Wgt+=1+log(Fq);}
      }
      DomainWgtOrgPrV.Add(TFltStrPr(Wgt, OrgNm));
    }
    // unit weights
    if (OrgNmToUnitFqKdVH.IsKey(OrgNm)){
      TStrIntKdV& UnitFqKdV=OrgNmToUnitFqKdVH.GetDat(OrgNm);
      double Wgt=0;
      for (int UnitN=0; UnitN<UnitFqKdV.Len(); UnitN++){
        int Fq=UnitFqKdV[UnitN].Dat;
        if (Fq>0){Wgt+=1+log(Fq);}
      }
      UnitWgtOrgPrV.Add(TFltStrPr(Wgt, OrgNm));
    }
  }}
  CountryWgtOrgPrV.Sort(false);
  DomainWgtOrgPrV.Sort(false);
  UnitWgtOrgPrV.Sort(false);

  // output country hubs
  {TFOut FOut(CountryHubFNm); FILE* fOut=FOut.GetFileId();
  fprintf(fOut, "*** Country Hubs\n");
  fprintf(fOut, "[Rank] [Score] [#Countries] [Organization] [Distribution]\n");
  for (int OrgN=0; OrgN<CountryWgtOrgPrV.Len(); OrgN++){
    double Wgt=CountryWgtOrgPrV[OrgN].Val1;
    TStr OrgNm=CountryWgtOrgPrV[OrgN].Val2;
    PEpOrg Org=GetOrg(OrgNm);
    // invert country/frequency vector
    TStrIntKdV& CountryFqKdV=OrgNmToCountryFqKdVH.GetDat(OrgNm);
    fprintf(fOut, "%5d. %6.2f (%2d): '%s'/%s   [",
     1+OrgN, Wgt, CountryFqKdV.Len(), OrgNm.CStr(), Org->GetCountryNm().CStr());
    TIntStrPrV FqCountryPrV;
    for (int CountryN=0; CountryN<CountryFqKdV.Len(); CountryN++){
      FqCountryPrV.Add(TIntStrPr(CountryFqKdV[CountryN].Dat, CountryFqKdV[CountryN].Key));}
    FqCountryPrV.Sort(false);
    // ouput country list
    {for (int CountryN=0; CountryN<CountryFqKdV.Len(); CountryN++){
      TStr CountryNm=FqCountryPrV[CountryN].Val2;
      int Fq=FqCountryPrV[CountryN].Val1;
      fprintf(fOut, "'%s':%d ", CountryNm.CStr(), Fq);
    }}
    fprintf(fOut, "]\n");
  }}

  // output domain hubs
  {TFOut FOut(DomainHubFNm); FILE* fOut=FOut.GetFileId();
  fprintf(fOut, "*** Domain Hubs\n");
  fprintf(fOut, "[Rank] [Score] [#Domains] [Organization] [Distribution]\n");
  for (int OrgN=0; OrgN<DomainWgtOrgPrV.Len(); OrgN++){
    double Wgt=DomainWgtOrgPrV[OrgN].Val1;
    TStr OrgNm=DomainWgtOrgPrV[OrgN].Val2;
    PEpOrg Org=GetOrg(OrgNm);
    // invert country/frequency vector
    TStrIntKdV& DomainFqKdV=OrgNmToDomainFqKdVH.GetDat(OrgNm);
    fprintf(fOut, "%5d. %6.2f (%2d): '%s'/%s   [",
     1+OrgN, Wgt, DomainFqKdV.Len(), OrgNm.CStr(), Org->GetCountryNm().CStr());
    TIntStrPrV FqDomainPrV;
    for (int DomainN=0; DomainN<DomainFqKdV.Len(); DomainN++){
      FqDomainPrV.Add(TIntStrPr(DomainFqKdV[DomainN].Dat, DomainFqKdV[DomainN].Key));}
    FqDomainPrV.Sort(false);
    // ouput country list
    {for (int DomainN=0; DomainN<DomainFqKdV.Len(); DomainN++){
      TStr DomainNm=FqDomainPrV[DomainN].Val2;
      int Fq=FqDomainPrV[DomainN].Val1;
      fprintf(fOut, "'%s':%d ", DomainNm.CStr(), Fq);
    }}
    fprintf(fOut, "]\n");
  }}

  // output unit hubs
  {TFOut FOut(UnitHubFNm); FILE* fOut=FOut.GetFileId();
  fprintf(fOut, "*** Unit Hubs\n");
  fprintf(fOut, "[Rank] [Score] [#Units] [Organization] [Distribution]\n");
  for (int OrgN=0; OrgN<UnitWgtOrgPrV.Len(); OrgN++){
    double Wgt=UnitWgtOrgPrV[OrgN].Val1;
    TStr OrgNm=UnitWgtOrgPrV[OrgN].Val2;
    PEpOrg Org=GetOrg(OrgNm);
    // invert country/frequency vector
    TStrIntKdV& UnitFqKdV=OrgNmToUnitFqKdVH.GetDat(OrgNm);
    fprintf(fOut, "%5d. %6.2f (%2d): '%s'/%s   [",
     1+OrgN, Wgt, UnitFqKdV.Len(), OrgNm.CStr(), Org->GetCountryNm().CStr());
    TIntStrPrV FqUnitPrV;
    for (int UnitN=0; UnitN<UnitFqKdV.Len(); UnitN++){
      FqUnitPrV.Add(TIntStrPr(UnitFqKdV[UnitN].Dat, UnitFqKdV[UnitN].Key));}
    FqUnitPrV.Sort(false);
    // ouput country list
    {for (int UnitN=0; UnitN<UnitFqKdV.Len(); UnitN++){
      TStr UnitNm=FqUnitPrV[UnitN].Val2;
      int Fq=FqUnitPrV[UnitN].Val1;
      fprintf(fOut, "'%s'(%s):%d ",
       UnitNm.CStr(), TEpBs::GetUnitDNm(UnitNm).CStr(), Fq);
    }}
    fprintf(fOut, "]\n");
  }}
  printf("Done.\n");
}

void TEpBs::SaveOrgConnectivity(const TStr& TxtFNm, const TStr& GraphFNm,
 const int& MnClbOrgFq) const {
  int Orgs=GetOrgs();
  printf("Save Organization Connectivity %s/%s ...\n", TxtFNm.CStr(), GraphFNm.CStr());
  // count connected components
  TIntV OrgMarkV(Orgs); OrgMarkV.PutAll(-1);
  int ConnComps=0; int NonSingleConnComps=0; TIntV ConnCompLenV;
  forever{
    // get first unmarked org-id
    int StartOrgId=OrgMarkV.SearchForw(-1);
    if (StartOrgId==-1){break;}
    int ConnCompN=ConnComps; ConnComps++; ConnCompLenV.Add(0);
    // traverse & mark connected component
    TIntQ OpenOrgIdQ; OpenOrgIdQ.Push(StartOrgId);
    while (!OpenOrgIdQ.Empty()){
      // get org-id from queue
      int OrgId=OpenOrgIdQ.Top(); OpenOrgIdQ.Pop();
      PEpOrg Org=GetOrg(OrgId);
      // mark current org-id & push children org-ids
      IAssert((OrgMarkV[OrgId]==-1)||(OrgMarkV[OrgId]==ConnCompN));
      if (OrgMarkV[OrgId]==-1){
        // mark current org-id
        OrgMarkV[OrgId]=ConnCompN;
        ConnCompLenV[ConnCompN]++;
        // push children org-ids
        int ClbOrgs=Org->GetClbOrgs();
        for (int ClbOrgN=0; ClbOrgN<ClbOrgs; ClbOrgN++){
          // get child org-id
          TStr ClbOrgNm=Org->GetClbOrgNm(ClbOrgN, this);
          int ClbOrgFq=Org->GetClbOrgFq(ClbOrgN);
          int ClbOrgId=GetOrgId(ClbOrgNm);
          // push child org-id if necessary
          if (ClbOrgFq>=MnClbOrgFq){
            IAssert((OrgMarkV[ClbOrgId]==-1)||(OrgMarkV[ClbOrgId]==ConnCompN));
            if (OrgMarkV[ClbOrgId]==-1){
              OpenOrgIdQ.Push(ClbOrgId);
            }
          }
        }
      }
    }
    if (ConnCompLenV[ConnCompN]>1){
      NonSingleConnComps++;}
  }

  // collect connected components
  TIntStrVH ConnCompNToOrgNmVH;
  for (int OrgId=0; OrgId<Orgs; OrgId++){
    int ConnCompN=OrgMarkV[OrgId];
    int ConnCompLen=ConnCompLenV[ConnCompN];
    if (ConnCompLen>1){
      PEpOrg Org=GetOrg(OrgId);
      ConnCompNToOrgNmVH.AddDat(ConnCompN).Add(Org->GetOrgNm());}
  }
  // output connected components
  TFOut FOut(TxtFNm); FILE* fOut=FOut.GetFileId();
  fprintf(fOut, "*** All Connected-Components: %d\n", ConnComps);
  fprintf(fOut, "*** Non-Single Connected-Components: %d\n", NonSingleConnComps);
  fprintf(fOut, "*** Minimal Collaboration-Frequency: %d\n", MnClbOrgFq);
  fprintf(fOut, "\n");
  for (int ConnCompP=0; ConnCompP<ConnCompNToOrgNmVH.Len(); ConnCompP++){
    int ConnCompN=ConnCompNToOrgNmVH.GetKey(ConnCompP);
    int ConnCompLen=ConnCompLenV[ConnCompN];
    TStrV& OrgNmV=ConnCompNToOrgNmVH[ConnCompP];
    OrgNmV.Sort();
    fprintf(fOut, "*** Connected component %d (%d organizations)\n",
     1+ConnCompP, ConnCompLen);
    for (int OrgNmN=0; OrgNmN<OrgNmV.Len(); OrgNmN++){
      PEpOrg Org=GetOrg(OrgNmV[OrgNmN]);
      fprintf(fOut, "'%s'/%s\n",
       Org->GetOrgNm().CStr(), Org->GetCountryNm().CStr());
    }
    fprintf(fOut, "\n");
  }

  // create & save graph
  PGraph Graph=TGraph::New();
  TIntV VrtxOrgIdV;
  int Vrtxs=0;
  {for (int OrgId=0; OrgId<Orgs; OrgId++){
    int ConnCompN=OrgMarkV[OrgId];
    int ConnCompLen=ConnCompLenV[ConnCompN];
    if (ConnCompLen>1){
      PEpOrg Org=GetOrg(OrgId);
      TStr MLOrgNm=TEpOrg::GetMultiLnOrgNm(Org->GetOrgNm());
      VrtxOrgIdV.Add(OrgId);
      Graph->AddVrtx(TGVrtx::New(OrgId, MLOrgNm));
      Vrtxs++;
    }
  }}
  int Edges=0;
  for (int OrgIdN1=0; OrgIdN1<VrtxOrgIdV.Len(); OrgIdN1++){
    int OrgId1=VrtxOrgIdV[OrgIdN1];
    PEpOrg Org1=GetOrg(OrgId1);
    for (int OrgIdN2=OrgIdN1+1; OrgIdN2<VrtxOrgIdV.Len(); OrgIdN2++){
      int OrgId2=VrtxOrgIdV[OrgIdN2];
      PEpOrg Org2=GetOrg(OrgId2);
      int ClbFq=Org1->GetClbOrgFq(Org2->GetOrgNm(), this);
      if (ClbFq>=MnClbOrgFq){
        PVrtx Vrtx1=Graph->GetVrtx(OrgId1);
        PVrtx Vrtx2=Graph->GetVrtx(OrgId2);
        PEdge Edge=TGEdge::New(Vrtx1, Vrtx2, TInt::GetStr(ClbFq), false);
        Edge->PutWidth(ClbFq-MnClbOrgFq+1);
        //Edge->PutDNm(TInt::GetStr(ClbFq));
        Graph->AddEdge(Edge);
        Edges++;
      }
    }
  }
  if (Edges>0){
    Graph->SaveTxt(GraphFNm);}
  printf("Done.\n");
}

void TEpBs::SaveOrgConnectivity(const TStr& TxtFNm, const TStr& GraphFNm,
 const int& MnClbOrgFq, const int& MxClbOrgFq) const {
  for (int ClbOrgFq=MnClbOrgFq; ClbOrgFq<=MxClbOrgFq; ClbOrgFq++){
    TStr SubTxtFNm=TxtFNm.GetFPath()+
     TxtFNm.GetFMid()+TInt::GetStr(ClbOrgFq)+TxtFNm.GetFExt();
    TStr SubGraphFNm=GraphFNm.GetFPath()+
     GraphFNm.GetFMid()+TInt::GetStr(ClbOrgFq)+GraphFNm.GetFExt();
    SaveOrgConnectivity(SubTxtFNm, SubGraphFNm, ClbOrgFq);
  }
}

void TEpBs::SaveOrgSmallWorld(const TStr& TxtFNm, const TStr& TreeFNm) const {
  printf("Save Organization Small World to %s/%s ...\n", TxtFNm.CStr(), TreeFNm.CStr());
  // get sorted orgs according to the number of projects
  int Orgs=GetOrgs();
  typedef TTriple<PMom, TFlt, TInt> TMomDistOrgIdTr;
  TVec<TMomDistOrgIdTr> MomDistOrgIdV(Orgs, 0);
  for (int OrgId=0; OrgId<Orgs; OrgId++){
    printf("%d/%d\r", 1+OrgId, Orgs);
    PEpOrg Org=GetOrg(OrgId);
    // get statistics
    PMom OrgDistMom=GetDistMom(OrgId);
    double OrgDistMean=OrgDistMom->GetMean();
    MomDistOrgIdV.Add(TMomDistOrgIdTr(OrgDistMom, -OrgDistMean, OrgId));
  }
  printf("\n");
  MomDistOrgIdV.Sort(false);
  // output
  TFOut TxtFOut(TxtFNm); FILE* fTxtOut=TxtFOut.GetFileId();
  // toplist
  fprintf(fTxtOut, "*** Organizations by average distance to the rest of the graph\n");
  for (int OrgIdN=0; OrgIdN<MomDistOrgIdV.Len(); OrgIdN++){
    PMom OrgDistMom=MomDistOrgIdV[OrgIdN].Val1;
    double OrgMeanDist=-MomDistOrgIdV[OrgIdN].Val2;
    int OrgId=MomDistOrgIdV[OrgIdN].Val3;
    PEpOrg Org=GetOrg(OrgId);
    fprintf(fTxtOut, "%4d. %4.2f avg.dist. '%s'/%s (%d projects)\n",
     1+OrgIdN, OrgMeanDist, Org->GetOrgNm().CStr(), Org->GetCountryNm().CStr(),
     Org->GetProjs());
  }
  fprintf(fTxtOut, "\n");
  // full
  fprintf(fTxtOut, "*** Full listing\n");
  {for (int OrgIdN=0; OrgIdN<MomDistOrgIdV.Len(); OrgIdN++){
    PMom OrgDistMom=MomDistOrgIdV[OrgIdN].Val1;
    double OrgMeanDist=-MomDistOrgIdV[OrgIdN].Val2;
    int OrgId=MomDistOrgIdV[OrgIdN].Val3;
    PEpOrg Org=GetOrg(OrgId);
    fprintf(fTxtOut, "%4d. %4.2f avg.dist. '%s'/%s (%d projects):\n\t%s\n",
     1+OrgIdN, OrgMeanDist, Org->GetOrgNm().CStr(), Org->GetCountryNm().CStr(),
     Org->GetProjs(), OrgDistMom->GetStr(' ', ':', true, "%.2f").CStr());
  }}
  fprintf(fTxtOut, "\n");
  // save smallest tree
  if (MomDistOrgIdV.Len()>0){
    int BestOrgId=MomDistOrgIdV[0].Val3;
    PEpOrg BestOrg=GetOrg(BestOrgId);
    SaveOrgDistTree(TreeFNm, BestOrg->GetOrgNm());
  }

  printf("Done.\n");
}

void TEpBs::SaveOfficerStat(const TStr& FNm, const PBowDocBs& EpBowDocBs) const {
  printf("Save Officer Statistics to %s ...\n", FNm.CStr());
  // get sorted officers according to the number of projects
  int Offs=GetOfficers();
  TIntKdV FqOffIdKdV(Offs, 0);
  for (int OffId=0; OffId<Offs; OffId++){
    PEpOfficer Officer=GetOfficer(OffId);
    FqOffIdKdV.Add(TIntKd(Officer->GetProjs(), OffId));
  }
  FqOffIdKdV.Sort(false);
  // output
  PBowDocWgtBs EpBowDocWgtBs=TBowDocWgtBs::New(EpBowDocBs, bwwtNrmTFIDF, 0, 3);
  TFOut FOut(FNm); FILE* fOut=FOut.GetFileId();
  for (int OffIdN=0; OffIdN<FqOffIdKdV.Len(); OffIdN++){
    int OffId=FqOffIdKdV[OffIdN].Dat;
    PEpOfficer Officer=GetOfficer(OffId);
    // collect organization frequency
    TStrIntH OrgNmToFqH;
    for (int ProjN=0; ProjN<Officer->GetProjs(); ProjN++){
      TStr ProjNm=Officer->GetProjNm(ProjN);
      PEpProj Proj=GetProj(ProjNm);
      // traverse project organizations
      for (int OrgN=0; OrgN<Proj->GetOrgs(); OrgN++){
        TStr OrgNm=Proj->GetOrgNm(OrgN, this);
        OrgNmToFqH.AddDat(OrgNm)++;
      }
    }

    // output officer header
    fprintf(fOut, "*** %4d. '%s' (%d projects):\n",
     1+OffIdN, Officer->GetOfficerNm().CStr(), Officer->GetProjs());

    // output projects
    fprintf(fOut, "Projects(%d):", Officer->GetProjs());
    {for (int ProjN=0; ProjN<Officer->GetProjs(); ProjN++){
      if ((ProjN)%4==0){fprintf(fOut, "\n  ");}
      TStr ProjNm=Officer->GetProjNm(ProjN);
      PEpProj Proj=GetProj(ProjNm);
      fprintf(fOut, " '%s'", Proj->GetProjNm().CStr());
      //fprintf(fOut, " '%s'/%s/%s", Proj->GetProjNm().CStr(),
      // Proj->GetDomainNm().CStr(), Proj->GetUnitNm().CStr());
    }}
    fprintf(fOut, "\n");

    // output countries
    TIntStrPrV FqCountryPrV; Officer->GetFqCountryPrV(FqCountryPrV);
    FqCountryPrV.Sort(false);
    fprintf(fOut, "Countries(all:%d)\n", FqCountryPrV.Len());
    int SumOrgFq=0;
    for (int CountryN=0; CountryN<FqCountryPrV.Len(); CountryN++){
      SumOrgFq+=FqCountryPrV[CountryN].Val1;}
    fprintf(fOut, "Organizations-Per-Country(all:%d):", SumOrgFq);
    {for (int CountryN=0; CountryN<FqCountryPrV.Len(); CountryN++){
      int Fq=FqCountryPrV[CountryN].Val1;
      TStr CountryNm=FqCountryPrV[CountryN].Val2;
      if ((CountryN)%4==0){fprintf(fOut, "\n  ");}
      fprintf(fOut, " %s(%.1f%%/%d)",
       CountryNm.CStr(), 100*Fq/double(SumOrgFq), Fq);
    }}
    fprintf(fOut, "\n");

    // output organizations
    TIntStrPrV FqOrgNmPrV; OrgNmToFqH.GetDatKeyPrV(FqOrgNmPrV);
    FqOrgNmPrV.Sort(false);
    fprintf(fOut, "Organizations(all:%d):\n", FqOrgNmPrV.Len());
    for (int OrgN=0; OrgN<FqOrgNmPrV.Len(); OrgN++){
      int Fq=FqOrgNmPrV[OrgN].Val1;
      TStr OrgNm=FqOrgNmPrV[OrgN].Val2;
      if (Fq>2){fprintf(fOut, "   '%s':%d\n", OrgNm.CStr(), Fq);}
      else {break;}
    }

    // output keywords
    // collect documents
    TIntV DIdV; int DId;
    {for (int ProjN=0; ProjN<Officer->GetProjs(); ProjN++){
      TStr ProjNm=Officer->GetProjNm(ProjN);
      if (EpBowDocBs->IsDocNm(ProjNm, DId)){DIdV.Add(DId);}
    }}
    // create concept document
    PBowSpV ConceptSpV;
    TBowClust::GetConceptSpV(EpBowDocWgtBs, NULL, DIdV, ConceptSpV);
    // collect ordered limited word/weights-vector
    TStrFltPrV WordStrWgtPrV;
    ConceptSpV->GetWordStrWgtPrV(EpBowDocBs, 25, 0.66, WordStrWgtPrV);
    fprintf(fOut, "Keywords:");
    for (int WordN=0; WordN<WordStrWgtPrV.Len(); WordN++){
      TStr WordStr=WordStrWgtPrV[WordN].Val1;
      double WordWgt=WordStrWgtPrV[WordN].Val2;
      if ((WordN)%4==0){fprintf(fOut, "\n  ");}
      fprintf(fOut, " [%s:%.2f]", WordStr.CStr(), WordWgt);
    }
    fprintf(fOut, "\n");

    fprintf(fOut, "\n");
  }
  printf("Done.\n");
}

void TEpBs::SaveOrgStat(const TStr& FNm, const PBowDocBs& EpBowDocBs) const {
  printf("Save Organization Statistics to %s ...\n", FNm.CStr());
  // get sorted organization according to the number of projects
  int Orgs=GetOrgs();
  TIntKdV FqOrgIdKdV(Orgs, 0);
  for (int OrgId=0; OrgId<Orgs; OrgId++){
    PEpOrg Org=GetOrg(OrgId);
    FqOrgIdKdV.Add(TIntKd(Org->GetProjs(), OrgId));
  }
  FqOrgIdKdV.Sort(false);
  // output
  PBowDocWgtBs EpBowDocWgtBs=TBowDocWgtBs::New(EpBowDocBs, bwwtNrmTFIDF, 0, 3);
  TFOut FOut(FNm); FILE* fOut=FOut.GetFileId();
  for (int OrgIdN=0; OrgIdN<FqOrgIdKdV.Len(); OrgIdN++){
    printf("%d/%d\r", OrgIdN, FqOrgIdKdV.Len());
    int OrgId=FqOrgIdKdV[OrgIdN].Dat;
    PEpOrg Org=GetOrg(OrgId);
    fprintf(fOut, "*** %4d. '%s'/%s (%d projects):\n",
     1+OrgIdN, Org->GetOrgNm().CStr(), Org->GetCountryNm().CStr(),
     Org->GetProjs());

    // output projects
    fprintf(fOut, "Projects(%d):", Org->GetProjs());
    for (int ProjN=0; ProjN<Org->GetProjs(); ProjN++){
      if ((ProjN)%5==0){fprintf(fOut, "\n  ");}
      fprintf(fOut, " '%s'", Org->GetProjNm(ProjN).CStr());
    }
    fprintf(fOut, "\n");

    // output roles
    TStrIntH RoleNmToFqH;
    {for (int ProjN=0; ProjN<Org->GetProjs(); ProjN++){
      PEpProj Proj=GetProj(Org->GetProjNm(ProjN));
      TStr RoleNm=Proj->GetRole(Org->GetOrgNm(), this);
      RoleNmToFqH.AddDat(RoleNm)++;
    }}
    TIntStrPrV FqRoleNmPrV; RoleNmToFqH.GetDatKeyPrV(FqRoleNmPrV);
    FqRoleNmPrV.Sort(false);
    fprintf(fOut, "Project-Roles:\n");
    for (int RoleN=0; RoleN<FqRoleNmPrV.Len(); RoleN++){
      int RoleFq=FqRoleNmPrV[RoleN].Val1;
      TStr RoleNm=FqRoleNmPrV[RoleN].Val2;
      fprintf(fOut, " '%s':%.2f%% (%d/%d)", RoleNm.CStr(),
       100*RoleFq/double(Org->GetProjs()), RoleFq, Org->GetProjs());
    }
    fprintf(fOut, "\n");

    // output keywords
    // collect documents
    TIntV DIdV; int DId;
    {for (int ProjN=0; ProjN<Org->GetProjs(); ProjN++){
      TStr ProjNm=Org->GetProjNm(ProjN);
      if (EpBowDocBs->IsDocNm(ProjNm, DId)){DIdV.Add(DId);}
    }}
    // create concept document
    PBowSpV ConceptSpV;
    TBowClust::GetConceptSpV(EpBowDocWgtBs, NULL, DIdV, ConceptSpV);
    // collect ordered limited word/weights-vector
    TStrFltPrV WordStrWgtPrV;
    ConceptSpV->GetWordStrWgtPrV(EpBowDocBs, 25, 0.66, WordStrWgtPrV);
    fprintf(fOut, "Keywords:");
    for (int WordN=0; WordN<WordStrWgtPrV.Len(); WordN++){
      TStr WordStr=WordStrWgtPrV[WordN].Val1;
      double WordWgt=WordStrWgtPrV[WordN].Val2;
      if ((WordN)%4==0){fprintf(fOut, "\n  ");}
      fprintf(fOut, " [%s:%.2f]", WordStr.CStr(), WordWgt);
    }
    fprintf(fOut, "\n");

    fprintf(fOut, "\n");
  }
  printf("\nDone.\n");
}

void TEpBs::SaveDomainStat(const TStr& FNm, const PBowDocBs& EpBowDocBs) const {
  printf("Save Domain Statistics to %s ...\n", FNm.CStr());
  // get sorted organization according to the number of projects
  TIntKdV FqDomainIdKdV(GetDomains(), 0);
  for (int DomainId=0; DomainId<GetDomains(); DomainId++){
    PEpDomain Domain=GetDomain(DomainId);
    FqDomainIdKdV.Add(TIntKd(Domain->GetProjs(), DomainId));
  }
  FqDomainIdKdV.Sort(false);
  // output
  PBowDocWgtBs EpBowDocWgtBs=TBowDocWgtBs::New(EpBowDocBs, bwwtNrmTFIDF, 0, 3);
  TFOut FOut(FNm); FILE* fOut=FOut.GetFileId();
  for (int DomainIdN=0; DomainIdN<FqDomainIdKdV.Len(); DomainIdN++){
    int DomainId=FqDomainIdKdV[DomainIdN].Dat;
    PEpDomain Domain=GetDomain(DomainId);
    fprintf(fOut, "*** %4d. '%s' (%d projects):\n",
     1+DomainIdN, Domain->GetDomainNm().CStr(), Domain->GetProjs());

    // output organization-types & countries
    TStrIntH OrgTypeToFqH; TStrIntH CountryToFqH; int OrgFqSum=0;
    for (int OrgN=0; OrgN<Domain->GetOrgs(); OrgN++){
      int OrgId=Domain->GetOrgId(OrgN);
      int OrgFq=Domain->GetOrgFq(OrgN);
      PEpOrg Org=GetOrg(OrgId);
      OrgTypeToFqH.AddDat(Org->GetOrgTypeNm())+=OrgFq;
      CountryToFqH.AddDat(Org->GetCountryNm())+=OrgFq;
      OrgFqSum+=OrgFq;
    }
    TIntStrPrV FqOrgTypePrV; OrgTypeToFqH.GetDatKeyPrV(FqOrgTypePrV);
    TIntStrPrV FqCountryPrV; CountryToFqH.GetDatKeyPrV(FqCountryPrV);
    FqOrgTypePrV.Sort(false); FqCountryPrV.Sort(false);
    fprintf(fOut, "Organization-Types:\n");
    for (int OrgTypeN=0; OrgTypeN<FqOrgTypePrV.Len(); OrgTypeN++){
      int OrgTypeFq=FqOrgTypePrV[OrgTypeN].Val1;
      TStr OrgTypeNm=FqOrgTypePrV[OrgTypeN].Val2;
      fprintf(fOut, "   '%s': %.0f%% (%d/%d)\n", OrgTypeNm.CStr(),
       100*OrgTypeFq/double(OrgFqSum), OrgTypeFq, OrgFqSum);
    }
    fprintf(fOut, "Countries:");
    for (int CountryN=0; CountryN<FqCountryPrV.Len(); CountryN++){
      int CountryFq=FqCountryPrV[CountryN].Val1;
      double CountryPrc=CountryFq/double(OrgFqSum);
      if (CountryPrc<0.05){break;}
      TStr CountryNm=FqCountryPrV[CountryN].Val2;
      if ((CountryN)%2==0){fprintf(fOut, "\n  ");}
      fprintf(fOut, " ['%s' %.0f%% (%d/%d)]", CountryNm.CStr(),
       100*CountryPrc, CountryFq, OrgFqSum);
    }
    fprintf(fOut, "\n");

    // output keywords
    // collect documents
    TIntV DIdV; int DId;
    {for (int ProjN=0; ProjN<Domain->GetProjs(); ProjN++){
      TStr ProjNm=Domain->GetProjNm(ProjN);
      if (EpBowDocBs->IsDocNm(ProjNm, DId)){DIdV.Add(DId);}
    }}
    // create concept document
    PBowSpV ConceptSpV;
    TBowClust::GetConceptSpV(EpBowDocWgtBs, NULL, DIdV, ConceptSpV);
    // collect ordered limited word/weights-vector
    TStrFltPrV WordStrWgtPrV;
    ConceptSpV->GetWordStrWgtPrV(EpBowDocBs, 50, 0.5, WordStrWgtPrV);
    fprintf(fOut, "Keywords:");
    for (int WordN=0; WordN<WordStrWgtPrV.Len(); WordN++){
      TStr WordStr=WordStrWgtPrV[WordN].Val1;
      double WordWgt=WordStrWgtPrV[WordN].Val2;
      if ((WordN)%4==0){fprintf(fOut, "\n  ");}
      fprintf(fOut, " [%s:%.2f]", WordStr.CStr(), WordWgt);
    }
    fprintf(fOut, "\n");

    // output projects
    fprintf(fOut, "Projects(%d):", Domain->GetProjs());
    for (int ProjN=0; ProjN<Domain->GetProjs(); ProjN++){
      if ((ProjN)%5==0){fprintf(fOut, "\n  ");}
      fprintf(fOut, " '%s'", Domain->GetProjNm(ProjN).CStr());
    }
    fprintf(fOut, "\n");

    fprintf(fOut, "\n");
  }
  printf("Done.\n");
}

void TEpBs::SaveUnitStat(const TStr& FNm, const PBowDocBs& EpBowDocBs) const {
  printf("Save Unit Statistics to %s ...\n", FNm.CStr());
  // get sorted organization according to the number of projects
  int Units=GetUnits();
  TIntKdV FqUnitIdKdV(Units, 0);
  for (int UnitId=0; UnitId<Units; UnitId++){
    PEpUnit Unit=GetUnit(UnitId);
    FqUnitIdKdV.Add(TIntKd(Unit->GetProjs(), UnitId));
  }
  FqUnitIdKdV.Sort(false);
  // output
  PBowDocWgtBs EpBowDocWgtBs=TBowDocWgtBs::New(EpBowDocBs, bwwtNrmTFIDF, 0, 3);
  TFOut FOut(FNm); FILE* fOut=FOut.GetFileId();
  for (int UnitIdN=0; UnitIdN<FqUnitIdKdV.Len(); UnitIdN++){
    int UnitId=FqUnitIdKdV[UnitIdN].Dat;
    PEpUnit Unit=GetUnit(UnitId);
    fprintf(fOut, "*** %4d. '%s' [%s] (%d projects):\n",
     1+UnitIdN, Unit->GetUnitNm().CStr(), GetUnitDNm(Unit->GetUnitNm()).CStr(),
     Unit->GetProjs());

    // output organization-types & countries
    TStrIntH OrgTypeToFqH; TStrIntH CountryToFqH; int OrgFqSum=0;
    for (int OrgN=0; OrgN<Unit->GetOrgs(); OrgN++){
      int OrgId=Unit->GetOrgId(OrgN);
      int OrgFq=Unit->GetOrgFq(OrgN);
      PEpOrg Org=GetOrg(OrgId);
      OrgTypeToFqH.AddDat(Org->GetOrgTypeNm())+=OrgFq;
      CountryToFqH.AddDat(Org->GetCountryNm())+=OrgFq;
      OrgFqSum+=OrgFq;
    }
    TIntStrPrV FqOrgTypePrV; OrgTypeToFqH.GetDatKeyPrV(FqOrgTypePrV);
    TIntStrPrV FqCountryPrV; CountryToFqH.GetDatKeyPrV(FqCountryPrV);
    FqOrgTypePrV.Sort(false); FqCountryPrV.Sort(false);
    fprintf(fOut, "Organization-Types:\n");
    for (int OrgTypeN=0; OrgTypeN<FqOrgTypePrV.Len(); OrgTypeN++){
      int OrgTypeFq=FqOrgTypePrV[OrgTypeN].Val1;
      TStr OrgTypeNm=FqOrgTypePrV[OrgTypeN].Val2;
      fprintf(fOut, "   '%s': %.0f%% (%d/%d)\n", OrgTypeNm.CStr(),
       100*OrgTypeFq/double(OrgFqSum), OrgTypeFq, OrgFqSum);
    }
    fprintf(fOut, "Countries:");
    for (int CountryN=0; CountryN<FqCountryPrV.Len(); CountryN++){
      int CountryFq=FqCountryPrV[CountryN].Val1;
      double CountryPrc=CountryFq/double(OrgFqSum);
      if (CountryPrc<0.03){break;}
      TStr CountryNm=FqCountryPrV[CountryN].Val2;
      if ((CountryN)%2==0){fprintf(fOut, "\n  ");}
      fprintf(fOut, " ['%s' %.0f%% (%d/%d)]", CountryNm.CStr(),
       100*CountryPrc, CountryFq, OrgFqSum);
    }
    fprintf(fOut, "\n");

    // output keywords
    // collect documents
    TIntV DIdV; int DId;
    {for (int ProjN=0; ProjN<Unit->GetProjs(); ProjN++){
      TStr ProjNm=Unit->GetProjNm(ProjN);
      if (EpBowDocBs->IsDocNm(ProjNm, DId)){DIdV.Add(DId);}
    }}
    // create concept document
    PBowSpV ConceptSpV;
    TBowClust::GetConceptSpV(EpBowDocWgtBs, NULL, DIdV, ConceptSpV);
    // collect ordered limited word/weights-vector
    TStrFltPrV WordStrWgtPrV;
    ConceptSpV->GetWordStrWgtPrV(EpBowDocBs, 50, 0.5, WordStrWgtPrV);
    fprintf(fOut, "Keywords:");
    for (int WordN=0; WordN<WordStrWgtPrV.Len(); WordN++){
      TStr WordStr=WordStrWgtPrV[WordN].Val1;
      double WordWgt=WordStrWgtPrV[WordN].Val2;
      if ((WordN)%4==0){fprintf(fOut, "\n  ");}
      fprintf(fOut, " [%s:%.2f]", WordStr.CStr(), WordWgt);
    }
    fprintf(fOut, "\n");

    // output projects
    fprintf(fOut, "Projects(%d):", Unit->GetProjs());
    for (int ProjN=0; ProjN<Unit->GetProjs(); ProjN++){
      if ((ProjN)%5==0){fprintf(fOut, "\n  ");}
      fprintf(fOut, " '%s'", Unit->GetProjNm(ProjN).CStr());
    }
    fprintf(fOut, "\n");

    fprintf(fOut, "\n");
  }
  printf("Done.\n");
}

void TEpBs::SaveCountryStat(const TStr& FNm, const PBowDocBs& EpBowDocBs) const {
  printf("Save Country Statistics to %s ...\n", FNm.CStr());
  // get sorted organization according to the number of projects
  int Countries=GetCountries();
  TIntKdV FqCountryIdKdV(Countries, 0);
  for (int CountryId=0; CountryId<Countries; CountryId++){
    PEpCountry Country=GetCountry(CountryId);
    FqCountryIdKdV.Add(TIntKd(Country->GetProjs(), CountryId));
  }
  FqCountryIdKdV.Sort(false);
  // output
  PBowDocWgtBs EpBowDocWgtBs=TBowDocWgtBs::New(EpBowDocBs, bwwtNrmTFIDF, 0, 3);
  TFOut FOut(FNm); FILE* fOut=FOut.GetFileId();
  for (int CountryIdN=0; CountryIdN<FqCountryIdKdV.Len(); CountryIdN++){
    int CountryId=FqCountryIdKdV[CountryIdN].Dat;
    PEpCountry Country=GetCountry(CountryId);
    fprintf(fOut, "*** %4d. '%s':\n", 1+CountryIdN, Country->GetCountryNm().CStr());
    fprintf(fOut, "Projects: %d\n", Country->GetProjs());
    fprintf(fOut, "Organizations: %d\n", Country->GetOrgs());
    fprintf(fOut, "Project-Participations: %d\n", Country->GetOrgFqSum());

    // output organization-types & countries
    TStrIntH OrgTypeToFqH; TStrIntH OrgToFqH; int OrgFqSum=0;
    for (int OrgN=0; OrgN<Country->GetOrgs(); OrgN++){
      int OrgId=Country->GetOrgId(OrgN);
      int OrgFq=Country->GetOrgFq(OrgN);
      PEpOrg Org=GetOrg(OrgId);
      OrgTypeToFqH.AddDat(Org->GetOrgTypeNm())+=OrgFq;
      OrgToFqH.AddDat(Org->GetOrgNm())+=OrgFq;
      OrgFqSum+=OrgFq;
    }
    TIntStrPrV FqOrgTypePrV; OrgTypeToFqH.GetDatKeyPrV(FqOrgTypePrV);
    TIntStrPrV FqOrgPrV; OrgToFqH.GetDatKeyPrV(FqOrgPrV);
    FqOrgTypePrV.Sort(false); FqOrgPrV.Sort(false);
    fprintf(fOut, "Organization-Types:\n");
    for (int OrgTypeN=0; OrgTypeN<FqOrgTypePrV.Len(); OrgTypeN++){
      int OrgTypeFq=FqOrgTypePrV[OrgTypeN].Val1;
      TStr OrgTypeNm=FqOrgTypePrV[OrgTypeN].Val2;
      fprintf(fOut, "   '%s': %.0f%% (%d/%d)\n", OrgTypeNm.CStr(),
       100*OrgTypeFq/double(OrgFqSum), OrgTypeFq, OrgFqSum);
    }
    fprintf(fOut, "Projects Participations Per Organization:\n");
    for (int CountryN=0; CountryN<FqOrgPrV.Len(); CountryN++){
      int OrgFq=FqOrgPrV[CountryN].Val1;
      double OrgPrc=OrgFq/double(OrgFqSum);
      if (CountryN>=10){break;}
      TStr OrgNm=FqOrgPrV[CountryN].Val2;
      fprintf(fOut, "   %2d. %5.2f%% (%d/%d) '%s'\n",
       1+CountryN, 100*OrgPrc, OrgFq, OrgFqSum, OrgNm.CStr());
    }

    // output keywords
    // collect documents
    TIntV DIdV; int DId;
    {for (int ProjN=0; ProjN<Country->GetProjs(); ProjN++){
      TStr ProjNm=Country->GetProjNm(ProjN);
      if (EpBowDocBs->IsDocNm(ProjNm, DId)){DIdV.Add(DId);}
    }}
    // create concept document
    PBowSpV ConceptSpV;
    TBowClust::GetConceptSpV(EpBowDocWgtBs, NULL, DIdV, ConceptSpV);
    // collect ordered limited word/weights-vector
    TStrFltPrV WordStrWgtPrV;
    ConceptSpV->GetWordStrWgtPrV(EpBowDocBs, 50, 0.5, WordStrWgtPrV);
    fprintf(fOut, "Keywords:");
    for (int WordN=0; WordN<WordStrWgtPrV.Len(); WordN++){
      TStr WordStr=WordStrWgtPrV[WordN].Val1;
      double WordWgt=WordStrWgtPrV[WordN].Val2;
      if ((WordN)%4==0){fprintf(fOut, "\n  ");}
      fprintf(fOut, " [%s:%.2f]", WordStr.CStr(), WordWgt);
    }
    fprintf(fOut, "\n");

    // output projects
    fprintf(fOut, "Projects(%d):", Country->GetProjs());
    for (int ProjN=0; ProjN<Country->GetProjs(); ProjN++){
      if ((ProjN)%5==0){fprintf(fOut, "\n  ");}
      fprintf(fOut, " '%s'", Country->GetProjNm(ProjN).CStr());
    }
    fprintf(fOut, "\n");

    fprintf(fOut, "\n");
  }
  printf("Done.\n");
}

void TEpBs::SaveOrgDistTree(const TStr& FNm, const TStr& OrgNmWc) const {
  printf("Save Organization Distance Graph to %s ...\n", FNm.CStr());
  // get sorted orgs according to the number of projects
  int Orgs=GetOrgs();
  int StartOrgId=-1;
  TIntKdV FqOrgIdKdV(Orgs, 0);
  for (int OrgId=0; OrgId<Orgs; OrgId++){
    PEpOrg Org=GetOrg(OrgId);
    TStr OrgNm=Org->GetOrgNm();
    if ((!OrgNmWc.Empty())&&(OrgNm.IsWcMatch(OrgNmWc))){
      StartOrgId=OrgId;}
    FqOrgIdKdV.Add(TIntKd(Org->GetProjs(), OrgId));
  }
  FqOrgIdKdV.Sort(false);
  // get min. number of project to have full name in graph
  int MnFullOrgNmProjs;
  if (FqOrgIdKdV.Len()<15){MnFullOrgNmProjs=FqOrgIdKdV.Last().Key;}
  else {MnFullOrgNmProjs=FqOrgIdKdV[15].Key;}
  // prepare start org-id
  if (OrgNmWc.Empty()){
    StartOrgId=FqOrgIdKdV[0].Dat;
  } else
  if (StartOrgId==-1){
    printf("No match for wild-card '%s'.\n", OrgNmWc.CStr());
    return;
  }
  PEpOrg StartOrg=GetOrg(StartOrgId);
  TStr StartOrgNm=StartOrg->GetOrgNm();
  printf("Creating distance graph for organization '%s'.\n", StartOrgNm.CStr());

  // create distance graph
  PGraph Graph=TGraph::New();
  Graph->AddVrtx(TGVrtx::New(StartOrgId, TEpOrg::GetMultiLnOrgNm(
   StartOrgNm+"("+TInt::GetStr(StartOrg->GetProjs())+")")));
  TIntV OrgDistV(GetOrgs()); OrgDistV.PutAll(-1);
  TIntPrQ OpenOrgIdQ; OpenOrgIdQ.Push(TIntPr(StartOrgId, 0));
  OrgDistV[StartOrgId]=0;
  while (!OpenOrgIdQ.Empty()){
    // get org-id from queue
    int OrgId=OpenOrgIdQ.Top().Val1;
    int OrgDist=OpenOrgIdQ.Top().Val2;
    OpenOrgIdQ.Pop();
    IAssert(OrgDistV[OrgId]==OrgDist);
    PEpOrg Org=GetOrg(OrgId);
    TStr OrgNm=Org->GetOrgNm();
    PVrtx SrcVrtx=Graph->GetVrtx(OrgId);
    int ClbOrgs=Org->GetClbOrgs();
    for (int ClbOrgN=0; ClbOrgN<ClbOrgs; ClbOrgN++){
      // get child org-id
      TStr ClbOrgNm=Org->GetClbOrgNm(ClbOrgN, this);
      int ClbOrgId=GetOrgId(ClbOrgNm);
      PEpOrg ClbOrg=GetOrg(ClbOrgId);
      // push child org-id if necessary
      if (OrgDistV[ClbOrgId]==-1){
        OpenOrgIdQ.Push(TIntPr(ClbOrgId, OrgDist+1));
        OrgDistV[ClbOrgId]=OrgDist+1;
        // create vertex
        TStr VNm=ClbOrg->GetBriefOrgNm();
        if (ClbOrg->GetProjs()<=MnFullOrgNmProjs){VNm=TStr("_")+VNm;}
        VNm=VNm+"("+TInt::GetStr(ClbOrg->GetProjs())+")";
        VNm=TEpOrg::GetMultiLnOrgNm(VNm);
        PVrtx DstVrtx=TGVrtx::New(ClbOrgId, VNm);
        Graph->AddVrtx(DstVrtx);
        // create edge
        TStr ENm=TInt::GetStr(OrgId)+"-"+TInt::GetStr(ClbOrgId);
        PEdge Edge=TGEdge::New(SrcVrtx, DstVrtx, TStr("_")+ENm);
        Graph->AddEdge(Edge);
      }
    }
  }
  // save graph
  Graph->SaveTxt(FNm);
  printf("Done.\n");
}

void TEpBs::SaveDomainSim(
 const TStr& FNm, const PBowDocBs& EpBowDocBs,
 const double& SimSumPrc, const int& MxKws) const {
  printf("Save Domain Graph to %s ...\n", FNm.CStr());
  PBowDocWgtBs EpBowDocWgtBs=TBowDocWgtBs::New(EpBowDocBs, bwwtNrmTFIDF, 0, 3);
  TStrStrH DomainNmToDescH;
  THash<TStr, PBowSpV> DomainNmToConceptSpV;
  for (int DomainId=0; DomainId<GetDomains(); DomainId++){
    PEpDomain Domain=GetDomain(DomainId);
    TStr DomainNm=Domain->GetDomainNm();
    if (DomainNm=="#EMPTY"){continue;}
    // collect domain project descriptions
    TIntV DIdV; int DId;
    for (int ProjN=0; ProjN<Domain->GetProjs(); ProjN++){
      TStr ProjNm=Domain->GetProjNm(ProjN);
      if (EpBowDocBs->IsDocNm(ProjNm, DId)){DIdV.Add(DId);}
    }
    // create concept document
    PBowSpV ConceptSpV;
    TBowClust::GetConceptSpV(EpBowDocWgtBs, NULL, DIdV, ConceptSpV);
    // collect ordered limited word/weights-vector
    TStrFltPrV WordStrWgtPrV;
    ConceptSpV->GetWordStrWgtPrV(EpBowDocBs, MxKws, 0.5, WordStrWgtPrV);
    DomainNmToConceptSpV.AddDat(DomainNm, ConceptSpV);
    // compose domain description
    TChA DomainDescChA;
    DomainDescChA+=TStr("*** ")+DomainNm+" ***";
    DomainDescChA+=TStr("\\")+TInt::GetStr(Domain->GetProjs())+ " projects";
    //printf("%s\n", DomainNm.CStr());
    for (int WordN=0; WordN<WordStrWgtPrV.Len(); WordN++){
      TStr WordStr=WordStrWgtPrV[WordN].Val1;
      double WordWgt=WordStrWgtPrV[WordN].Val2;
      DomainDescChA+=TStr("\\")+WordStr+":"+TFlt::GetStr(WordWgt, "%.2f");
      //printf(" [%s:%.2f]\n", WordStr.CStr(), WordWgt);
    }
    DomainNmToDescH.AddDat(DomainNm, DomainDescChA);
  }
  // calculate distances
  PBowSim BowSim=TBowSim::New(bstCos); // similarity object
  TFltVV DomainSimVV;
  double SimSum=0; TFltIntIntTrV SimN1N2TrV;
  for (int DomainN1=0; DomainN1<DomainNmToConceptSpV.Len(); DomainN1++){
    PBowSpV ConceptSpV1=DomainNmToConceptSpV[DomainN1];
    for (int DomainN2=DomainN1+1; DomainN2<DomainNmToConceptSpV.Len(); DomainN2++){
      PBowSpV ConceptSpV2=DomainNmToConceptSpV[DomainN2];
      double Sim=BowSim->GetSim(ConceptSpV1, ConceptSpV2);
      SimSum+=Sim;
      SimN1N2TrV.Add(TFltIntIntTr(Sim, DomainN1, DomainN2));
    }
  }
  SimN1N2TrV.Sort(false);
  // create & save graph
  PGraph Graph=TGraph::New();
  for (int DomainN=0; DomainN<DomainNmToConceptSpV.Len(); DomainN++){
    TStr DomainNm=DomainNmToConceptSpV.GetKey(DomainN);
    TStr DomainDescStr=DomainNmToDescH.GetDat(DomainNm);
    Graph->AddVrtx(TGVrtx::New(DomainN, DomainDescStr));
  }
  double SimSumSF=0;
  for (int SimTrN=0; SimTrN<SimN1N2TrV.Len(); SimTrN++){
    double Sim=SimN1N2TrV[SimTrN].Val1;
    double DomainN1=SimN1N2TrV[SimTrN].Val2;
    double DomainN2=SimN1N2TrV[SimTrN].Val3;
    if (SimSumSF>SimSumPrc*SimSum){continue;}
    PVrtx Vrtx1=Graph->GetVrtx(DomainN1);
    PVrtx Vrtx2=Graph->GetVrtx(DomainN2);
    PEdge Edge=TGEdge::New(Vrtx1, Vrtx2, TFlt::GetStr(Sim, "%.2f"), false);
    Edge->PutWgt(Sim);
    Graph->AddEdge(Edge);
    SimSumSF+=Sim;
  }
  Graph->SetEdgeWidth(5);
  Graph->SaveTxt(FNm);
  printf("Done.\n");
}

void TEpBs::SaveUnitSim(
 const TStr& FNm, const PBowDocBs& EpBowDocBs,
 const double& SimSumPrc, const int& MxKws) const {
  printf("Save Unit Graph to %s ...\n", FNm.CStr());
  PBowDocWgtBs EpBowDocWgtBs=TBowDocWgtBs::New(EpBowDocBs, bwwtNrmTFIDF, 0, 3);
  TStrStrH UnitNmToDescH;
  THash<TStr, PBowSpV> UnitNmToConceptSpV;
  for (int UnitId=0; UnitId<GetUnits(); UnitId++){
    PEpUnit Unit=GetUnit(UnitId);
    TStr UnitNm=Unit->GetUnitNm();
    if (UnitNm=="#EMPTY"){continue;}
    // collect domain project descriptions
    TIntV DIdV; int DId;
    for (int ProjN=0; ProjN<Unit->GetProjs(); ProjN++){
      TStr ProjNm=Unit->GetProjNm(ProjN);
      if (EpBowDocBs->IsDocNm(ProjNm, DId)){DIdV.Add(DId);}
    }
    // create concept document
    PBowSpV ConceptSpV;
    TBowClust::GetConceptSpV(EpBowDocWgtBs, NULL, DIdV, ConceptSpV);
    // collect ordered limited word/weights-vector
    TStrFltPrV WordStrWgtPrV;
    ConceptSpV->GetWordStrWgtPrV(EpBowDocBs, MxKws, 0.5, WordStrWgtPrV);
    UnitNmToConceptSpV.AddDat(UnitNm, ConceptSpV);
    // compose domain description
    TChA UnitDescChA;
    if (UnitNm==TEpBs::GetUnitDNm(UnitNm)){
      UnitDescChA+=UnitNm+"("+TInt::GetStr(Unit->GetProjs())+")";
    } else {
      UnitDescChA+=UnitNm+"/"+TEpBs::GetUnitDNm(UnitNm)+"("+TInt::GetStr(Unit->GetProjs())+")";
    }
    //printf("%s\n", UnitNm.CStr());
    for (int WordN=0; WordN<WordStrWgtPrV.Len(); WordN++){
      TStr WordStr=WordStrWgtPrV[WordN].Val1;
      double WordWgt=WordStrWgtPrV[WordN].Val2;
      UnitDescChA+=TStr("\\")+WordStr+":"+TFlt::GetStr(WordWgt, "%.2f");
      //printf(" [%s:%.2f]\n", WordStr.CStr(), WordWgt);
    }
    UnitNmToDescH.AddDat(UnitNm, UnitDescChA);
  }
  // calculate distances
  PBowSim BowSim=TBowSim::New(bstCos); // similarity object
  TFltVV UnitSimVV;
  double SimSum=0; TFltIntIntTrV SimN1N2TrV;
  for (int UnitN1=0; UnitN1<UnitNmToConceptSpV.Len(); UnitN1++){
    PBowSpV ConceptSpV1=UnitNmToConceptSpV[UnitN1];
    for (int UnitN2=UnitN1+1; UnitN2<UnitNmToConceptSpV.Len(); UnitN2++){
      PBowSpV ConceptSpV2=UnitNmToConceptSpV[UnitN2];
      double Sim=BowSim->GetSim(ConceptSpV1, ConceptSpV2);
      SimSum+=Sim;
      SimN1N2TrV.Add(TFltIntIntTr(Sim, UnitN1, UnitN2));
    }
  }
  SimN1N2TrV.Sort(false);
  // create & save graph
  PGraph Graph=TGraph::New();
  for (int UnitN=0; UnitN<UnitNmToConceptSpV.Len(); UnitN++){
    TStr UnitNm=UnitNmToConceptSpV.GetKey(UnitN);
    TStr UnitDescStr=UnitNmToDescH.GetDat(UnitNm);
    Graph->AddVrtx(TGVrtx::New(UnitN, UnitDescStr));
  }
  double SimSumSF=0;
  for (int SimTrN=0; SimTrN<SimN1N2TrV.Len(); SimTrN++){
    double Sim=SimN1N2TrV[SimTrN].Val1;
    double UnitN1=SimN1N2TrV[SimTrN].Val2;
    double UnitN2=SimN1N2TrV[SimTrN].Val3;
    if (SimSumSF>SimSumPrc*SimSum){continue;}
    PVrtx Vrtx1=Graph->GetVrtx(UnitN1);
    PVrtx Vrtx2=Graph->GetVrtx(UnitN2);
    PEdge Edge=TGEdge::New(Vrtx1, Vrtx2, TFlt::GetStr(Sim, "%.2f"), false);
    Edge->PutWgt(Sim);
    Graph->AddEdge(Edge);
    SimSumSF+=Sim;
  }
  Graph->SetEdgeWidth(5);
  Graph->SaveTxt(FNm);
  printf("Done.\n");
}

void TEpBs::SaveOfficerSim(
 const TStr& GraphFNm, const PBowDocBs& EpBowDocBs,
 const double& SimSumPrc, const int& MxKws) const {
  printf("Save Officer Graph to %s ...\n", GraphFNm.CStr());
  PBowDocWgtBs EpBowDocWgtBs=TBowDocWgtBs::New(EpBowDocBs, bwwtNrmTFIDF, 0, 3);
  TStrStrH OfficerNmToDescH;
  THash<TStr, PBowSpV> OfficerNmToConceptSpV;
  for (int OfficerId=0; OfficerId<GetOfficers(); OfficerId++){
    PEpOfficer Officer=GetOfficer(OfficerId);
    TStr OfficerNm=Officer->GetOfficerNm();
    if (OfficerNm=="#EMPTY"){continue;}
    if (OfficerNm.GetTrunc().Empty()){continue;}
    // collect domain project descriptions
    TIntV DIdV; int DId;
    for (int ProjN=0; ProjN<Officer->GetProjs(); ProjN++){
      TStr ProjNm=Officer->GetProjNm(ProjN);
      if (EpBowDocBs->IsDocNm(ProjNm, DId)){DIdV.Add(DId);}
    }
    // create concept document
    PBowSpV ConceptSpV;
    TBowClust::GetConceptSpV(EpBowDocWgtBs, NULL, DIdV, ConceptSpV);
    // collect ordered limited word/weights-vector
    TStrFltPrV WordStrWgtPrV;
    ConceptSpV->GetWordStrWgtPrV(EpBowDocBs, MxKws, 0.5, WordStrWgtPrV);
    OfficerNmToConceptSpV.AddDat(OfficerNm, ConceptSpV);
    // compose domain description
    TChA OfficerDescChA;
    OfficerDescChA+=OfficerNm+"("+TInt::GetStr(Officer->GetProjs())+")";
    for (int WordN=0; WordN<WordStrWgtPrV.Len(); WordN++){
      TStr WordStr=WordStrWgtPrV[WordN].Val1;
      double WordWgt=WordStrWgtPrV[WordN].Val2;
      OfficerDescChA+=TStr("\\")+WordStr+":"+TFlt::GetStr(WordWgt, "%.2f");
    }
    OfficerNmToDescH.AddDat(OfficerNm, OfficerDescChA);
  }
  // calculate distances
  PBowSim BowSim=TBowSim::New(bstCos); // similarity object
  TFltVV OfficerSimVV;
  double SimSum=0; TFltIntIntTrV SimN1N2TrV;
  for (int OfficerN1=0; OfficerN1<OfficerNmToConceptSpV.Len(); OfficerN1++){
    PBowSpV ConceptSpV1=OfficerNmToConceptSpV[OfficerN1];
    for (int OfficerN2=OfficerN1+1; OfficerN2<OfficerNmToConceptSpV.Len(); OfficerN2++){
      PBowSpV ConceptSpV2=OfficerNmToConceptSpV[OfficerN2];
      double Sim=BowSim->GetSim(ConceptSpV1, ConceptSpV2);
      SimSum+=Sim;
      SimN1N2TrV.Add(TFltIntIntTr(Sim, OfficerN1, OfficerN2));
    }
  }
  SimN1N2TrV.Sort(false);
  // create & save graph
  PGraph Graph=TGraph::New();
  for (int OfficerN=0; OfficerN<OfficerNmToConceptSpV.Len(); OfficerN++){
    TStr OfficerNm=OfficerNmToConceptSpV.GetKey(OfficerN);
    TStr OfficerDescStr=OfficerNmToDescH.GetDat(OfficerNm);
    Graph->AddVrtx(TGVrtx::New(OfficerN, OfficerDescStr));
  }
  double SimSumSF=0;
  for (int SimTrN=0; SimTrN<SimN1N2TrV.Len(); SimTrN++){
    double Sim=SimN1N2TrV[SimTrN].Val1;
    double OfficerN1=SimN1N2TrV[SimTrN].Val2;
    double OfficerN2=SimN1N2TrV[SimTrN].Val3;
    if (SimSumSF>SimSumPrc*SimSum){continue;}
    PVrtx Vrtx1=Graph->GetVrtx(OfficerN1);
    PVrtx Vrtx2=Graph->GetVrtx(OfficerN2);
    PEdge Edge=TGEdge::New(Vrtx1, Vrtx2, TFlt::GetStr(Sim, "%.2f"), false);
    Edge->PutWgt(Sim);
    Graph->AddEdge(Edge);
    SimSumSF+=Sim;
  }
  Graph->SetEdgeWidth(5);
  Graph->SaveTxt(GraphFNm);
  printf("Done.\n");
}

void TEpBs::SaveOrgPowerDist(const TStr& FNm) const {
  int Orgs=GetOrgs();
  TIntPrV ClbOrgsOrgIdPrV; TIntIntH ClbOrgsToFqH;
  for (int OrgId=0; OrgId<Orgs; OrgId++){
    PEpOrg Org=GetOrg(OrgId);
    ClbOrgsOrgIdPrV.Add(TIntPr(Org->GetClbOrgs(), OrgId));
    ClbOrgsToFqH.AddDat(Org->GetClbOrgs())++;
  }
  ClbOrgsOrgIdPrV.Sort(false);
  TIntPrV FqClbOrgsPrV; ClbOrgsToFqH.GetDatKeyPrV(FqClbOrgsPrV);
  FqClbOrgsPrV.Sort(false);

  // create output file
  TFOut FOut(FNm);
  ClbOrgsOrgIdPrV.Save(FOut);
  FqClbOrgsPrV.Save(FOut);
  //TFltPrV XYPrV;
  //for (int OrgN=0; OrgN<ClbOrgsOrgIdPrV.Len(); OrgN++){
  //  XYPrV.Add(TFltPr(log(1+OrgN), log(ClbOrgsOrgIdPrV[OrgN].Val1)));
  //}
}

void TEpBs::SaveCountryProjects(const TStr& FNm,
 const PCordisEuProjBs& EuProjBs, const TStr& CountryNm) const {
  // create output file
  TFOut FOut(FNm); FILE* fOut=FOut.GetFileId();
  // get country data
  PEpCountry Country=GetCountry(CountryNm);
  // traverse country projects
  for (int ProjN=0; ProjN<Country->GetProjs(); ProjN++){
    TStr ProjNm=Country->GetProjNm(ProjN);
    PCordisEuProj EuProj; if (EuProjBs->IsEuProj(ProjNm, EuProj)){}
    PEpProj Proj=GetProj(ProjNm);
    fprintf(fOut, "*** %s/%s-%s\n", Proj->GetProjNm().CStr(),
     Proj->GetDomainNm().CStr(), Proj->GetUnitNm().CStr());
    if (!EuProj.Empty()){
      fprintf(fOut, "Title: %s\n", EuProj->GetTitleStr().CStr());}
    fprintf(fOut, "Officer: %s\n", Proj->GetOfficerNm().CStr());
    fprintf(fOut, "Organizations (%d):\n", Proj->GetOrgs());
    for (int OrgN=0; OrgN<Proj->GetOrgs(); OrgN++){
      fprintf(fOut, "   %s/%s\n", Proj->GetOrgNm(OrgN, this).CStr(),
       GetOrg(Proj->GetOrgNm(OrgN, this))->GetCountryNm().CStr());
    }
    //if (!EuProj.Empty()){
    //  fprintf(fOut, "Description: %s\n", EuProj->GetEuProjDescHtmlStr().CStr());}
    fprintf(fOut, "\n");
  }
}

void TEpBs::GetClbWgtProjNmPrV(
 TFltStrPrV& ClbWgtProjNmPrV, const int& DumpTopProjs) const {
  ClbWgtProjNmPrV.Clr();
  for (int ProjN=0; ProjN<GetProjs(); ProjN++){
    PEpProj Proj=GetProj(ProjN);
    double ClbWgt=0;
    for (int OrgN=0; OrgN<Proj->GetOrgs(); OrgN++){
      TStr OrgNm=Proj->GetOrgNm(OrgN, this);
      ClbWgt+=log(1+GetOrg(OrgNm)->GetClbOrgs());
    }
    ClbWgtProjNmPrV.Add(TFltStrPr(ClbWgt, Proj->GetProjNm()));
  }
  ClbWgtProjNmPrV.Sort(false);
  // dump top projects
  int DumpProjs=(DumpTopProjs==-1) ? GetProjs() : DumpTopProjs;
  {for (int ProjN=0; ProjN<DumpProjs; ProjN++){
    double ClbWgt=ClbWgtProjNmPrV[ProjN].Val1;
    TStr ProjNm=ClbWgtProjNmPrV[ProjN].Val2;
    PEpProj Proj=GetProj(ProjNm);
    printf("%d. [%.1f] %s:\n", 1+ProjN, ClbWgt, ProjNm.CStr());
    //if (ProjNm=="SOL-EU-NET"){
    //  printf("");}
    /*printf("   ");
    for (int OrgN=0; OrgN<Proj->GetOrgs(); OrgN++){
      printf(" '%s'", Proj->GetOrgNm(OrgN, this).CStr());
    }
    printf("\n");*/
  }}
}

void TEpBs::GetBestClbWgtProjNmV(const int& BestProjs, TStrV& BestProjNmV){
  TFltStrPrV ClbWgtProjNmPrV; GetClbWgtProjNmPrV(ClbWgtProjNmPrV);
  int Projs=GetProjs(); BestProjNmV.Clr();
  for (int ProjN=0; ProjN<BestProjs; ProjN++){
    if (ProjN>=Projs){break;}
    BestProjNmV.Add(ClbWgtProjNmPrV[ProjN].Val2);
  }
}

