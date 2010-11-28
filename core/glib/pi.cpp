/////////////////////////////////////////////////
// Includes
#include "pi.h"

/////////////////////////////////////////////////
// Project-Intelligence-Organization
void TPiRec::AddFldNmVal(
 TPiDb* PiDb, const TStr& FldNm, const TStr& FldVal){
  int FldNmCId=PiDb->AddCStr(FldNm);
  int FldValCId=PiDb->AddCStr(FldVal);
  FldNmCIdToValCIdH.AddDat(FldNmCId, FldValCId);
}

bool TPiRec::IsGetFldNmVal(const int& FldNmCId, TInt& FldValCId) const {
  return FldNmCIdToValCIdH.IsKeyGetDat(FldNmCId, FldValCId);
}

void TPiRec::GetFldNmVal(
 const int& FldN, int& FldNmCId, int& FldValCId) const {
  FldNmCId=FldNmCIdToValCIdH.GetKey(FldN);
  FldValCId=FldNmCIdToValCIdH[FldN];
}

void TPiRec::GetFldNmVal(
 const TPiDb* PiDb, const int& FldN, TStr& FldNm, TStr& FldVal) const {
  int FldNmCId=FldNmCIdToValCIdH.GetKey(FldN);
  int FldValCId=FldNmCIdToValCIdH[FldN];
  FldNm=PiDb->GetCStr(FldNmCId);
  FldVal=PiDb->GetCStr(FldValCId);
}

TStr TPiRec::GetFldVal(
 const TPiDb* PiDb, const TStr& FldNm, const TStr& DfVal) const {
  int FldNmCId=PiDb->GetCId(FldNm);
  int FldId;
  if (FldNmCIdToValCIdH.IsKey(FldNmCId, FldId)){
    int FldValCId=FldNmCIdToValCIdH[FldId];
    return PiDb->GetCStr(FldValCId);
  } else {
    return DfVal;
  }
}

TStr TPiRec::GetNrMoneyStr(const TStr& MoneyStr){
  const TStr MEuroStr=" million euro";
  const TStr EuroStr=" euro";
  TStr NrMoneyStr;
  if (MoneyStr.IsSuffix(MEuroStr)){
    TStr NumStr=MoneyStr.GetSubStr(0, MoneyStr.Len()-MEuroStr.Len()-1);
    if (NumStr.IsFlt()){
      NrMoneyStr=TFlt::GetStr(NumStr.GetFlt(), "%8.2fMeuro");
    } else {
      NrMoneyStr="Unknown";
    }
  } else
  if (MoneyStr.IsSuffix(EuroStr)){
    TStr NumStr=MoneyStr.GetSubStr(0, MoneyStr.Len()-EuroStr.Len()-1);
    if (NumStr.IsFlt()){
      NrMoneyStr=TFlt::GetStr(NumStr.GetFlt()/1000000.0, "%8.2fMeuro");
    } else {
      NrMoneyStr="Unknown";
    }
  } else {
    NrMoneyStr="Unknown";
  }
  return NrMoneyStr;
}

TStr TPiRec::GetNrDurationStr(const TStr& DurationStr){
  const TStr MonthsStr=" months";
  TStr NrDurationStr;
  if (DurationStr.IsSuffix(MonthsStr)){
    TStr NumStr=DurationStr.GetSubStr(0, DurationStr.Len()-MonthsStr.Len()-1);
    if (NumStr.IsInt()){
      NrDurationStr=TInt::GetStr(NumStr.GetInt(), "%3d months");
    } else {
      NrDurationStr=DurationStr;
    }
  } else {
    NrDurationStr=DurationStr;
  }
  return NrDurationStr;
}

/////////////////////////////////////////////////
// Project-Intelligence-Graph
TPiGraph::TPiGraph(const PPiDb& PiDb){
  printf("Generating Graphs...\n");
  // collect values
  printf("  ...collecting values\n");
  for (int ProjN=0; ProjN<PiDb->GetProjs(); ProjN++){
    PPiRec Proj=PiDb->GetProj(ProjN);
    CallNmH.AddKey(Proj->GetCallNm(PiDb()));
    SubProgNmH.AddKey(Proj->GetProjSubProg(PiDb()));
    TStr ProjRef=Proj->GetProjRef(PiDb());
    int ProjOrgs=PiDb->GetProjOrgs(ProjRef);
    for (int OrgN=0; OrgN<ProjOrgs; OrgN++){
      PPiRec Org=PiDb->GetProjOrg(ProjRef, OrgN);
      CountryNmH.AddKey(Org->GetCountryNm(PiDb()));
      OrgNmH.AddKey(Org->GetOrgNm(PiDb()));
    }
  }
  TStrV CallNmV; CallNmH.GetKeyV(CallNmV); CallNmV.Sort();
  TStrV SubProgNmV; SubProgNmH.GetKeyV(SubProgNmV); SubProgNmV.Sort();
  TStrV CountryNmV; CountryNmH.GetKeyV(CountryNmV); CountryNmV.Sort();
  TStrV OrgNmV; OrgNmH.GetKeyV(OrgNmV); OrgNmV.Sort();

  // organization & country collaboration
  printf("  ...organization & countries\n");
  THash<TStr, TStrH> CallNmToOrgNmHH;
  THash<TStr, TStrH> SubProgNmToOrgNmHH;
  for (int ProjN=0; ProjN<PiDb->GetProjs(); ProjN++){
    PPiRec Proj=PiDb->GetProj(ProjN);
    TStr CallNm=Proj->GetCallNm(PiDb());
    TStr SubProgNm=Proj->GetProjSubProg(PiDb());
    TStr ProjRef=Proj->GetProjRef(PiDb());
    int ProjOrgs=PiDb->GetProjOrgs(ProjRef);
    for (int OrgN1=0; OrgN1<ProjOrgs; OrgN1++){
      PPiRec Org1=PiDb->GetProjOrg(ProjRef, OrgN1);
      TStr OrgNm1=Org1->GetOrgNm(PiDb());
      TStr CountryNm1=Org1->GetCountryNm(PiDb());
      CallNmToOrgNmHH.AddDat(CallNm).AddKey(OrgNm1);
      SubProgNmToOrgNmHH.AddDat(SubProgNm).AddKey(OrgNm1);
      for (int OrgN2=OrgN1+1; OrgN2<ProjOrgs; OrgN2++){
        PPiRec Org2=PiDb->GetProjOrg(ProjRef, OrgN2);
        TStr OrgNm2=Org2->GetOrgNm(PiDb());
        TStr CountryNm2=Org2->GetCountryNm(PiDb());
        // add organization collaboration
        if (OrgNm1<OrgNm2){
          OrgNmPrToFqH.AddDat(TStrPr(OrgNm1, OrgNm2))++;
          CallNmToOrgNmPrToFqH.AddDat(CallNm).AddDat(TStrPr(OrgNm1, OrgNm2))++;
          SubProgNmToOrgNmPrToFqH.AddDat(SubProgNm).AddDat(TStrPr(OrgNm1, OrgNm2))++;
          if (CountryNm1==CountryNm2){
            CountryNmToOrgNmPrToFqH.AddDat(CountryNm1).AddDat(TStrPr(OrgNm1, OrgNm2))++;}
        } else {
          OrgNmPrToFqH.AddDat(TStrPr(OrgNm2, OrgNm1))++;
          CallNmToOrgNmPrToFqH.AddDat(CallNm).AddDat(TStrPr(OrgNm2, OrgNm1))++;
          SubProgNmToOrgNmPrToFqH.AddDat(SubProgNm).AddDat(TStrPr(OrgNm2, OrgNm1))++;
          if (CountryNm1==CountryNm2){
            CountryNmToOrgNmPrToFqH.AddDat(CountryNm1).AddDat(TStrPr(OrgNm2, OrgNm1))++;}
        }
        // add country collaboration
        if (CountryNm1<CountryNm2){CountryNmPrToFqH.AddDat(TStrPr(CountryNm1, CountryNm2))++;}
        else {CountryNmPrToFqH.AddDat(TStrPr(CountryNm2, CountryNm1))++;}
      }
    }
  }

  // extraction of call graph
  printf("  ...calls\n");
  for (int CallNmN1=0; CallNmN1<CallNmV.Len(); CallNmN1++){
    for (int CallNmN2=CallNmN1+1; CallNmN2<CallNmV.Len(); CallNmN2++){
      TStrV OrgNmV1; CallNmToOrgNmHH.GetDat(CallNmV[CallNmN1]).GetKeyV(OrgNmV1);
      TStrV OrgNmV2; CallNmToOrgNmHH.GetDat(CallNmV[CallNmN2]).GetKeyV(OrgNmV2);
      OrgNmV1.Sort(); OrgNmV2.Sort(); OrgNmV1.Intrs(OrgNmV2);
      CallNmPrToFqH.AddDat(
       TStrPr(CallNmV[CallNmN1], CallNmV[CallNmN2]), OrgNmV1.Len());
    }
  }

  // extraction of subprogramme graph
  printf("  ...subprogrames\n");
  for (int SubProgNmN1=0; SubProgNmN1<SubProgNmV.Len(); SubProgNmN1++){
    if (!SubProgNmToOrgNmHH.IsKey(SubProgNmV[SubProgNmN1])){continue;}
    for (int SubProgNmN2=SubProgNmN1+1; SubProgNmN2<SubProgNmV.Len(); SubProgNmN2++){
      if (!SubProgNmToOrgNmHH.IsKey(SubProgNmV[SubProgNmN2])){continue;}
      TStrV OrgNmV1; SubProgNmToOrgNmHH.GetDat(SubProgNmV[SubProgNmN1]).GetKeyV(OrgNmV1);
      TStrV OrgNmV2; SubProgNmToOrgNmHH.GetDat(SubProgNmV[SubProgNmN2]).GetKeyV(OrgNmV2);
      OrgNmV1.Sort(); OrgNmV2.Sort(); OrgNmV1.Intrs(OrgNmV2);
      SubProgNmPrToFqH.AddDat(
       TStrPr(SubProgNmV[SubProgNmN1], SubProgNmV[SubProgNmN2]), OrgNmV1.Len());
    }
  }

  // fixing color scheme for organizations
  for (int OrgN=0; OrgN<PiDb->GetOrgs(); OrgN++){
    PPiRec Org=PiDb->GetOrg(OrgN);
    TStr OrgNm=Org->GetOrgNm(PiDb());
    TStr OrgType=Org->GetOrgType(PiDb());
    TStr OrgSME=Org->GetSME(PiDb());
    int OrgColor=TGksColor::GetCyan().GetArgbVal();
    if (OrgType=="REC"){OrgColor=TGksColor::GetBlue().GetArgbVal();}
    else if (OrgType=="GOV"){OrgColor=TGksColor::GetCyan().GetArgbVal();}
    else if (OrgType=="HES"){OrgColor=TGksColor::GetGreen().GetArgbVal();}
    else if (OrgType=="PRI"){OrgColor=TGksColor::GetRed().GetArgbVal();}
    else if (OrgType=="IND"){
      OrgColor=TGksColor::GetBlack().GetArgbVal();
      if (OrgSME=="Yes"){OrgColor=TGksColor::GetYellow().GetArgbVal();}
    } else {
      OrgColor=TGksColor::GetCyan().GetArgbVal();
    }
    OrgNmToColorH.AddDat(OrgNm, OrgColor);
  }

  printf("Done.\n");
}

PGraph TPiGraph::GetDistTree(const PGraph& Graph, const TStr& StartVNm){
  // create distance graph
  PGraph DstTree=TGraph::New();
  PVrtx StartNode=new TGVrtx(StartVNm);
  DstTree->AddVrtx(StartNode);
  // prepare traversal data
  IAssert(Graph->IsVrtx(StartVNm));
  int StartVId=Graph->GetVrtx(StartVNm)->GetVId();
  TIntV VDistV(Graph->GetVrtxs()); VDistV.PutAll(-1);
  TIntPrQ OpenVIdDistPrQ; OpenVIdDistPrQ.Push(TIntPr(StartVId, 0));
  VDistV[StartVId]=0;
  while (!OpenVIdDistPrQ.Empty()){
    // get vertex-id from queue
    int VId=OpenVIdDistPrQ.Top().Val1;
    int VDist=OpenVIdDistPrQ.Top().Val2;
    OpenVIdDistPrQ.Pop();
    IAssert(VDistV[VId]==VDist);
    PVrtx Vrtx=Graph->GetVrtx(VId);
    PVrtx SrcNode;
    if (DstTree->GetVrtxs()==1){SrcNode=DstTree->GetVrtx(Vrtx->GetVNm());}
    else {SrcNode=DstTree->GetVrtx(TStr("_")+Vrtx->GetVNm());}
    int OutEIds=Vrtx->GetOutEIds();
    for (int EIdN=0; EIdN<OutEIds; EIdN++){
      // get child vertex-id
      int EId=Vrtx->GetOutEId(EIdN);
      PEdge Edge=Graph->GetEdge(EId);
      int OtherVId=Edge->GetOtherVId(Vrtx);
      TStr OtherVNm=Graph->GetVrtx(OtherVId)->GetVNm();
      // push child vertex-id if necessary
      if (VDistV[OtherVId]==-1){
        OpenVIdDistPrQ.Push(TIntPr(OtherVId, VDist+1));
        VDistV[OtherVId]=VDist+1;
        // create vertex
        PVrtx DstNode=TGVrtx::New(TStr("_")+OtherVNm);
        DstTree->AddVrtx(DstNode);
        // create edge
        TStr ENm=TInt::GetStr(OtherVId);
        PEdge Branch=TGEdge::New(SrcNode, DstNode, TStr("_")+ENm);
        DstTree->AddEdge(Branch);
      }
    }
  }
  return DstTree;
}

PGraph TPiGraph::GetGraph(
 const TStrPrIntH& StrPrToFqH, const TStrIntH& StrToColorH,
 const double& MxWgtSumPrc, const int& MxVrtxs){
  // prepare filter value
  double WgtSum=0;
  for (int StrPrN=0; StrPrN<StrPrToFqH.Len(); StrPrN++){
    WgtSum+=StrPrToFqH[StrPrN];
  }
  double MxWgtSum=WgtSum*MxWgtSumPrc;
  // collect values
  TIntStrPrPrV FqStrPrPrV; StrPrToFqH.GetDatKeyPrV(FqStrPrPrV);
  FqStrPrPrV.Sort(false);
  double WgtSumSF=0; int MxStrPrN=-1; int MnFq=0; int MxFq=0;
  TStrH StrH;
  for (int StrPrN=0; StrPrN<FqStrPrPrV.Len(); StrPrN++){
    if (WgtSumSF>MxWgtSum){break;}
    if ((MxVrtxs!=-1)&&(StrH.Len()>MxVrtxs)){break;}
    MxStrPrN=StrPrN;
    StrH.AddKey(FqStrPrPrV[StrPrN].Val2.Val1);
    StrH.AddKey(FqStrPrPrV[StrPrN].Val2.Val2);
    int Fq=FqStrPrPrV[StrPrN].Val1;
    WgtSumSF+=Fq;
    if ((StrPrN==0)||(Fq<MnFq)){MnFq=Fq;}
    if ((StrPrN==0)||(Fq>MxFq)){MxFq=Fq;}
  }
  TStrV StrV; StrH.GetKeyV(StrV);
  // create graph
  PGraph Graph=TGGraph::New();
  // create vertices
  THash<TStr, PVrtx> StrToVrtxH;
  for (int StrN=0; StrN<StrV.Len(); StrN++){
    PVrtx Vrtx=new TGVrtx(StrV[StrN]);
    Graph->AddVrtx(Vrtx);
    StrToVrtxH.AddDat(StrV[StrN], Vrtx);
    if (StrToColorH.IsKey(StrV[StrN])){
      Vrtx->PutColor(StrToColorH.GetDat(StrV[StrN]));
    }
  }
  // create edges
  for (int StrPrN=0; StrPrN<=MxStrPrN; StrPrN++){
    TStr Str1=FqStrPrPrV[StrPrN].Val2.Val1;
    TStr Str2=FqStrPrPrV[StrPrN].Val2.Val2;
    if (Str1==Str2){continue;}
    PVrtx Vrtx1=StrToVrtxH.GetDat(Str1);
    PVrtx Vrtx2=StrToVrtxH.GetDat(Str2);
    int Fq=FqStrPrPrV[StrPrN].Val1;
    TStr EdgeNm=TInt::GetStr(Fq);
    PEdge Edge=new TGEdge(Vrtx1, Vrtx2, EdgeNm, false);
    Graph->AddEdge(Edge);
    int Width=int(1+10*((double(Fq)-MnFq)/(MxFq-MnFq+1)));
    Edge->PutWidth(Width);
    Edge->PutWgt(Fq);
  }
  // return graph
  return Graph;
}

PMom TPiGraph::GetDistMom(const PGraph& Graph, const int& StartVId){
  PMom DistMom=TMom::New();
  TIntV VDistV(Graph->GetVrtxs()); VDistV.PutAll(-1);
  TIntPrQ OpenVIdDistPrQ; OpenVIdDistPrQ.Push(TIntPr(StartVId, 0));
  VDistV[StartVId]=0;
  while (!OpenVIdDistPrQ.Empty()){
    // get org-id from queue
    int VId=OpenVIdDistPrQ.Top().Val1;
    int VDist=OpenVIdDistPrQ.Top().Val2;
    OpenVIdDistPrQ.Pop();
    IAssert(VDistV[VId]==VDist);
    PVrtx Vrtx=Graph->GetVrtx(VId);
    int OutEdges=Vrtx->GetOutEIds();
    for (int OutEIdN=0; OutEIdN<OutEdges; OutEIdN++){
      // get child vertex-id
      int EId=Vrtx->GetOutEId(OutEIdN);
      PEdge Edge=Graph->GetEdge(EId);
      int OutVId=Edge->GetOtherVId(Vrtx);
      // push child org-id if necessary
      if (VDistV[OutVId]==-1){
        OpenVIdDistPrQ.Push(TIntPr(OutVId, VDist+1));
        VDistV[OutVId]=VDist+1;
        DistMom->Add(VDist+1);
      }
    }
  }
  DistMom->Def();
  return DistMom;
}

void TPiGraph::GetConnectedness(
 const PGraph& Graph, const TStr& TxtFNm, const TStr& TabFNm){
  // get sorted vertices according to the number of projects
  int Vrtxs=Graph->GetVrtxs();
  typedef TTriple<PMom, TFlt, TInt> TMomDistVIdTr;
  TVec<TMomDistVIdTr> MomDistVIdV(Vrtxs, 0);
  PMom AllVrtxDistMom=TMom::New();
  for (int VId=0; VId<Vrtxs; VId++){
    printf("%d/%d\r", 1+VId, Vrtxs);
    PVrtx Vrtx=Graph->GetVrtx(VId);
    // get statistics
    PMom VrtxDistMom=GetDistMom(Graph, VId);
    double VrtxDistMean=0;
    if (VrtxDistMom->IsUsable()){
      VrtxDistMean=VrtxDistMom->GetMean();}
    MomDistVIdV.Add(TMomDistVIdTr(VrtxDistMom, -VrtxDistMean, VId));
    AllVrtxDistMom->Add(VrtxDistMean);
  }
  AllVrtxDistMom->Def();

  printf("\n");
  MomDistVIdV.Sort(false);
  // output
  TFOut TxtFOut(TxtFNm); FILE* fTxtOut=TxtFOut.GetFileId();
  TFOut TabFOut(TabFNm); FILE* fTabOut=TabFOut.GetFileId();
  // toplist
  if (AllVrtxDistMom->IsUsable()){
    fprintf(fTxtOut, "*** Average distance between vertices: %.3f\n",
     AllVrtxDistMom->GetMean());
  }
  fprintf(fTxtOut, "*** Vertices by average distance to the rest of the graph\n");
  fprintf(fTabOut, "Rank\tConnectedness\tName\n");
  double MxVrtxMeanDistSF=0;
  for (int VIdN=0; VIdN<MomDistVIdV.Len(); VIdN++){
    PMom VrtxDistMom=MomDistVIdV[VIdN].Val1;
    double VrtxMeanDist=-MomDistVIdV[VIdN].Val2;
    int VId=MomDistVIdV[VIdN].Val3;
    PVrtx Vrtx=Graph->GetVrtx(VId);
    fprintf(fTxtOut, "%4d. %4.2f avg.dist. '%s'\n",
     1+VIdN, VrtxMeanDist, Vrtx->GetVNm().CStr());
    if ((VIdN==0)||(VrtxMeanDist>=MxVrtxMeanDistSF)){
      fprintf(fTabOut, "%d\t%.2f\t%s\n",
       1+VIdN, VrtxMeanDist, Vrtx->GetVNm().CStr());
      MxVrtxMeanDistSF=VrtxMeanDist;
    }
  }
  fprintf(fTxtOut, "\n");
  // full
/*  fprintf(fTxtOut, "*** Full listing\n");
  {for (int VIdN=0; VIdN<MomDistVIdV.Len(); VIdN++){
    PMom VrtxDistMom=MomDistVIdV[VIdN].Val1;
    double VrtxMeanDist=-MomDistVIdV[VIdN].Val2;
    int VId=MomDistVIdV[VIdN].Val3;
    PVrtx Vrtx=Graph->GetVrtx(VId);
    fprintf(fTxtOut, "%4d. %4.2f avg.dist. '%s':\n\t%s\n",
     1+VIdN, VrtxMeanDist, Vrtx->GetVNm().CStr(),
     VrtxDistMom->GetStr(' ', ':', true, "%.2f").CStr());
  }}*/
  fprintf(fTxtOut, "\n");
  printf("Done.\n");
}

void TPiGraph::GetConnComps(const PGraph& Graph, const TStr& FNm){
  int Vrtxs=Graph->GetVrtxs();
  // count connected components
  TIntV VrtxMarkV(Vrtxs); VrtxMarkV.PutAll(-1);
  int ConnComps=0; int NonSingleConnComps=0; TIntV ConnCompLenV;
  forever{
    // get first unmarked org-id
    int StartVId=VrtxMarkV.SearchForw(-1);
    if (StartVId==-1){break;}
    int ConnCompN=ConnComps; ConnComps++; ConnCompLenV.Add(0);
    // traverse & mark connected component
    TIntQ OpenVIdQ; OpenVIdQ.Push(StartVId);
    while (!OpenVIdQ.Empty()){
      // get org-id from queue
      int VId=OpenVIdQ.Top(); OpenVIdQ.Pop();
      PVrtx Vrtx=Graph->GetVrtx(VId);
      // mark current org-id & push children org-ids
      IAssert((VrtxMarkV[VId]==-1)||(VrtxMarkV[VId]==ConnCompN));
      if (VrtxMarkV[VId]==-1){
        // mark current vertex-id
        VrtxMarkV[VId]=ConnCompN;
        ConnCompLenV[ConnCompN]++;
        // push children vertex-ids
        int OutEdges=Vrtx->GetOutEIds();
        for (int OutEIdN=0; OutEIdN<OutEdges; OutEIdN++){
          // get child vertex-id
          int EId=Vrtx->GetOutEId(OutEIdN);
          PEdge Edge=Graph->GetEdge(EId);
          int OutVId=Edge->GetOtherVId(Vrtx);
          PVrtx OutVrtx=Graph->GetVrtx(OutVId);
          TStr OutVrtxNm=OutVrtx->GetVNm();
          // push child vertex-id if necessary
          IAssert((VrtxMarkV[OutVId]==-1)||(VrtxMarkV[OutVId]==ConnCompN));
          if (VrtxMarkV[OutVId]==-1){
            OpenVIdQ.Push(OutVId);
          }
        }
      }
    }
    if (ConnCompLenV[ConnCompN]>1){
      NonSingleConnComps++;}
  }

  // collect connected components
  TIntStrVH ConnCompNToVrtxNmVH;
  for (int VId=0; VId<Vrtxs; VId++){
    int ConnCompN=VrtxMarkV[VId];
    int ConnCompLen=ConnCompLenV[ConnCompN];
    if (ConnCompLen>1){
      PVrtx Vrtx=Graph->GetVrtx(VId);
      ConnCompNToVrtxNmVH.AddDat(ConnCompN).Add(Vrtx->GetVNm());}
  }
  // output connected components
  TFOut FOut(FNm); FILE* fOut=FOut.GetFileId();
  fprintf(fOut, "*** All Connected-Components: %d\n", ConnComps);
  fprintf(fOut, "*** Non-Single Connected-Components: %d\n", NonSingleConnComps);
  fprintf(fOut, "\n");
  for (int ConnCompP=0; ConnCompP<ConnCompNToVrtxNmVH.Len(); ConnCompP++){
    int ConnCompN=ConnCompNToVrtxNmVH.GetKey(ConnCompP);
    int ConnCompLen=ConnCompLenV[ConnCompN];
    TStrV& VrtxNmV=ConnCompNToVrtxNmVH[ConnCompP];
    VrtxNmV.Sort();
    fprintf(fOut, "*** Connected component %d (%d organizations)\n",
     1+ConnCompP, ConnCompLen);
    for (int VrtxNmN=0; VrtxNmN<VrtxNmV.Len(); VrtxNmN++){
      PVrtx Vrtx=Graph->GetVrtx(VrtxNmV[VrtxNmN]);
      fprintf(fOut, "'%s'\n",
       Vrtx->GetVNm().CStr());
    }
    fprintf(fOut, "\n");
  }
}

/////////////////////////////////////////////////
// Project-Intelligence-Database
void TPiDb::GetCStrV(const TIntH& StrCIdH, TStrV& StrV) const {
  // transform cache-ids to strings
  TIntV StrCIdV; StrCIdH.GetKeyV(StrCIdV);
  StrV.Clr();
  for (int StrN=0; StrN<StrCIdV.Len(); StrN++){
    int StrCId=StrCIdV[StrN];
    TStr Str=GetCStr(StrCId);
    StrV.Add(Str);
  }
  StrV.Sort();
}

double TPiDb::GetOrgFunding(const TStr& OrgNm) const {
  const TIntV& ProjRefCIdV=OrgCNmToProjCRefVH.GetDat(GetCId(OrgNm));
  double Funding=0;
  for (int ProjN=0; ProjN<ProjRefCIdV.Len(); ProjN++){
    TStr ProjRef=GetCStr(ProjRefCIdV[ProjN]);
    PPiRec Proj=GetProj(ProjRef);
    TStr ProjFundingStr=Proj->GetProjFund(this);
    TStr MMoneyStr; TStr EuroStr;
    ProjFundingStr.SplitOnCh(MMoneyStr, 'M', EuroStr);
    if (MMoneyStr.IsFlt()&&(EuroStr=="euro")){
      Funding+=MMoneyStr.GetFlt()/GetProjOrgs(ProjRef);
    }
  }
  return Funding;
}

int TPiDb::GetPrimeContrs(const TStr& OrgNm) const {
  int PrimeContrs=0;
  for (int ProjN=0; ProjN<GetOrgProjs(OrgNm); ProjN++){
    PPiRec Proj=GetOrgProj(OrgNm, ProjN);
    if (Proj->GetProjPrimeContr(this)==OrgNm){PrimeContrs++;}
  }
  return PrimeContrs;
}

void TPiDb::GetFqClbOrgNmV(
 const TStr& OrgNm, int& SumClbs, TIntStrPrV& FqClbOrgNmPrV) const {
  SumClbs=0;
  TStrIntH ClbOrgNmFqH;
  for (int ProjN=0; ProjN<GetOrgProjs(OrgNm); ProjN++){
    PPiRec Proj=GetOrgProj(OrgNm, ProjN);
    TStr ProjRef=Proj->GetProjRef(this);
    for (int OrgN=0; OrgN<GetProjOrgs(ProjRef); OrgN++){
      PPiRec Org=GetProjOrg(ProjRef, OrgN);
      TStr ClbOrgNm=Org->GetOrgNm(this);
      if (OrgNm!=ClbOrgNm){
        ClbOrgNmFqH.AddDat(ClbOrgNm)++; SumClbs++;}
    }
  }
  ClbOrgNmFqH.GetDatKeyPrV(FqClbOrgNmPrV);
  FqClbOrgNmPrV.Sort(false);
}

void TPiDb::GetClbProjRefV(
 const TStr& OrgNm1, const TStr& OrgNm2, TStrV& ClbProjRefV) const {
  ClbProjRefV.Clr();
  for (int ProjN=0; ProjN<GetOrgProjs(OrgNm1); ProjN++){
    PPiRec Proj=GetOrgProj(OrgNm1, ProjN);
    TStr ProjRef=Proj->GetProjRef(this);
    for (int OrgN=0; OrgN<GetProjOrgs(ProjRef); OrgN++){
      PPiRec Org=GetProjOrg(ProjRef, OrgN);
      TStr ClbOrgNm=Org->GetOrgNm(this);
      if (ClbOrgNm==OrgNm2){
        ClbProjRefV.Add(ProjRef);}
    }
  }
  ClbProjRefV.Sort();
}

void TPiDb::GetOrgProjSummary(const TStr& OrgNm, TStrPrV& FundProjRefPrV) const {
  FundProjRefPrV.Clr();
  for (int ProjN=0; ProjN<GetOrgProjs(OrgNm); ProjN++){
    PPiRec Proj=GetOrgProj(OrgNm, ProjN);
    TStr ProjNm=Proj->GetProjAcronym(this).GetUc();
    TStr ProjRef=Proj->GetProjRef(this);
    TStr ProjFund=Proj->GetProjFund(this);
    FundProjRefPrV.Add(TStrPr(ProjNm, ProjRef));
  }
  FundProjRefPrV.Sort(true);
}

void TPiDb::GetCountryProjSummary(const TStr& CountryNm,
 int& PrimeContracts, TIntStrPrV& FqPrimeContrOrgNmPrV,
 int& SumProjRefFq, TStrStrIntTrV& FundProjRefFqTrV,
 int& SumCountryClbFq, TIntStrPrV& FqCountryNmPrV) const {
  PrimeContracts=0; TStrIntH PrimeContrOrgNmToFqH;
  SumProjRefFq=0; FundProjRefFqTrV.Clr();
  SumCountryClbFq=0; FqCountryNmPrV.Clr(); TStrIntH CountryNmToFqH;
  for (int ProjN=0; ProjN<GetProjs(); ProjN++){
    PPiRec Proj=GetProj(ProjN);
    TStr ProjRef=Proj->GetProjRef(this);
    for (int OrgN=0; OrgN<GetProjOrgs(ProjRef); OrgN++){
      PPiRec Org=GetProjOrg(ProjRef, OrgN);
      if (Org->GetOrgCountry(this)==CountryNm){
        if (Proj->GetProjPrimeContr(this)==Org->GetOrgNm(this)){
          PrimeContrOrgNmToFqH.AddDat(Org->GetOrgNm(this))++;
          PrimeContracts++;
        }
        SumProjRefFq++;
        if ((FundProjRefFqTrV.Len()>0)&&(FundProjRefFqTrV.Last().Val2==ProjRef)){
          FundProjRefFqTrV.Last().Val3++;
        } else {
          FundProjRefFqTrV.Add(TStrStrIntTr(Proj->GetProjFund(this), ProjRef, 1));
        }
      }
    }
    if ((FundProjRefFqTrV.Len()>0)&&(FundProjRefFqTrV.Last().Val2==ProjRef)){
      for (int OrgN=0; OrgN<GetProjOrgs(ProjRef); OrgN++){
        PPiRec Org=GetProjOrg(ProjRef, OrgN);
        if (Org->GetOrgCountry(this)!=CountryNm){
          SumCountryClbFq++;
          CountryNmToFqH.AddDat(Org->GetOrgCountry(this))++;
        }
      }
    }
  }
  // prime contracts
  PrimeContrOrgNmToFqH.GetDatKeyPrV(FqPrimeContrOrgNmPrV);
  FqPrimeContrOrgNmPrV.Sort(false);
  // funding
  FundProjRefFqTrV.Sort(false);
  CountryNmToFqH.GetDatKeyPrV(FqCountryNmPrV); FqCountryNmPrV.Sort(false);
}

void TPiDb::GetCountryOrgSummary(const TStr& CountryNm,
 int& SumOrgNmFq, TIntStrPrV& FqOrgNmPrV,
 int& SumOrgTypeFq, TIntStrPrV& FqOrgTypePrV) const {
  SumOrgNmFq=0; TStrIntH OrgNmToFqH;
  SumOrgTypeFq=0; TStrIntH OrgTypeToFqH;
  for (int OrgN=0; OrgN<GetOrgs(); OrgN++){
    PPiRec Org=GetOrg(OrgN);
    TStr OrgNm=Org->GetOrgNm(this);
    if (Org->GetOrgCountry(this)==CountryNm){
      int OrgProjs=GetOrgProjs(OrgNm);
      SumOrgNmFq+=OrgProjs; OrgNmToFqH.AddDat(OrgNm)+=OrgProjs;
      SumOrgTypeFq++; OrgTypeToFqH.AddDat(Org->GetOrgType(this))++;
    }
  }
  OrgNmToFqH.GetDatKeyPrV(FqOrgNmPrV); FqOrgNmPrV.Sort(false);
  OrgTypeToFqH.GetDatKeyPrV(FqOrgTypePrV); FqOrgTypePrV.Sort(false);
}

void TPiDb::GetSortedProjRefV(const TStr& KeyNm, TStrV& ProjRefV) const {
  TStrPrV KeyValProjRefPrV(GetProjs(), 0);
  bool AscOrderP=true;
  for (int ProjN=0; ProjN<GetProjs(); ProjN++){
    PPiRec Proj=GetProj(ProjN);
    TStr ProjRef=Proj->GetProjRef(this);
    // get key value
    TStr KeyVal;
    if (KeyNm=="Acronym"){KeyVal=Proj->GetProjAcronym(this);}
    else if (KeyNm=="Instrument"){KeyVal=Proj->GetProjInstr(this)+Proj->GetProjValue(this);}
    else if (KeyNm=="Value"){KeyVal=Proj->GetProjValue(this); AscOrderP=false;}
    else if (KeyNm=="Funding"){KeyVal=Proj->GetProjFund(this); AscOrderP=false;}
    else if (KeyNm=="Duration"){KeyVal=Proj->GetProjDuration(this)+Proj->GetProjValue(this); AscOrderP=false;}
    else if (KeyNm=="Consortium"){KeyVal=Proj->GetProjPartners(this)+Proj->GetProjValue(this); AscOrderP=false;}
    else if (KeyNm=="Strategic Objective"){KeyVal=Proj->GetProjSubProg(this)+Proj->GetProjInstr(this); AscOrderP=false;}
    else if (KeyNm=="Programme"){KeyVal=Proj->GetProjProg(this)+Proj->GetProjSubProg(this)+Proj->GetProjInstr(this);}
    else if (KeyNm=="SubProgramme"){KeyVal=Proj->GetProjSubProg(this)+Proj->GetProjInstr(this)+Proj->GetProjValue(this);}
    else {KeyVal=Proj->GetProjAcronym(this);}
    if (KeyVal=="Unknown"){KeyVal="\tUnknown";}
    KeyVal=KeyVal+Proj->GetProjAcronym(this);
    // save key-value/project-ref pair
    TStrPr KeyValProjRefPr(KeyVal, Proj->GetProjRef(this));
    KeyValProjRefPrV.Add(KeyValProjRefPr);
  }
  KeyValProjRefPrV.Sort(AscOrderP);
  // save sorted project-refs
  ProjRefV.Gen(KeyValProjRefPrV.Len(), 0);
  for (int ProjN=0; ProjN<KeyValProjRefPrV.Len(); ProjN++){
    ProjRefV.Add(KeyValProjRefPrV[ProjN].Val2);
  }
}

void TPiDb::GetSortedOrgNmV(const TStr& KeyNm, TStrV& OrgNmV) const {
  TStrPrV KeyValOrgNmPrV(GetOrgs(), 0); bool AscOrderP=true;
  for (int OrgN=0; OrgN<GetOrgs(); OrgN++){
    PPiRec Org=GetOrg(OrgN);
    TStr OrgNm=Org->GetOrgNm(this);
    // get key value
    TStr KeyVal;
    if (KeyNm=="Organisation"){KeyVal=OrgNm;}
    else if (KeyNm=="Projects"){KeyVal=Org->GetOrgProjs(this); AscOrderP=false;}
    else if (KeyNm=="Funding"){KeyVal=Org->GetOrgFunding(this); AscOrderP=false;}
    else if (KeyNm=="Type"){KeyVal=Org->GetOrgType(this);}
    else if (KeyNm=="Country"){KeyVal=Org->GetOrgCountry(this)+Org->GetOrgProjs(this);}
    else if (KeyNm=="City"){KeyVal=Org->GetOrgCity(this)+Org->GetOrgProjs(this);}
    else {KeyVal=OrgNm;}
    if (KeyVal=="Unknown"){KeyVal="\tUnknown";}
    KeyVal=KeyVal+OrgNm;
    // save key-value/project-ref pair
    TStrPr KeyValOrgNmPr(KeyVal, OrgNm);
    KeyValOrgNmPrV.Add(KeyValOrgNmPr);
  }
  KeyValOrgNmPrV.Sort(AscOrderP);
  // save sorted project-refs
  OrgNmV.Gen(KeyValOrgNmPrV.Len(), 0);
  for (int OrgN=0; OrgN<KeyValOrgNmPrV.Len(); OrgN++){
    OrgNmV.Add(KeyValOrgNmPrV[OrgN].Val2);
  }
}

void TPiDb::GetSortedCountryNmV(const TStr& KeyNm, TStrV& CountryNmV) const {
  TStrPrV KeyValCountryNmPrV(GetCountries(), 0); bool AscOrderP=true;
  for (int CountryN=0; CountryN<GetCountries(); CountryN++){
    PPiRec Country=GetCountry(CountryN);
    TStr CountryNm=Country->GetCountryNm(this);
    // get key value
    TStr KeyVal;
    if (KeyNm=="Country"){KeyVal=CountryNm;}
    else if (KeyNm=="Organisations"){KeyVal=Country->GetCountryOrgs(this); AscOrderP=false;}
    else if (KeyNm=="Projects"){KeyVal=Country->GetCountryProjs(this); AscOrderP=false;}
    else if (KeyNm=="Funding"){KeyVal=Country->GetCountryFunding(this); AscOrderP=false;}
    else if (KeyNm=="Population"){KeyVal=Country->GetCountryPopulation(this); AscOrderP=false;}
    else if (KeyNm=="GDP Per Capita"){KeyVal=Country->GetCountryGdpPerCapita(this); AscOrderP=false;}
    else if (KeyNm=="Funding Per Capita"){KeyVal=Country->GetCountryFundingPerCapita(this); AscOrderP=false;}
    else if (KeyNm=="Funding Per GDP"){KeyVal=Country->GetCountryFundingPerGdp(this); AscOrderP=false;}
    else {KeyVal=CountryNm;}
    if (KeyVal=="Unknown"){KeyVal="\tUnknown";}
    KeyVal=KeyVal+CountryNm;
    // save key-value/project-ref pair
    TStrPr KeyValCountryNmPr(KeyVal, CountryNm);
    KeyValCountryNmPrV.Add(KeyValCountryNmPr);
  }
  KeyValCountryNmPrV.Sort(AscOrderP);
  // save sorted project-refs
  CountryNmV.Gen(KeyValCountryNmPrV.Len(), 0);
  for (int CountryN=0; CountryN<KeyValCountryNmPrV.Len(); CountryN++){
    CountryNmV.Add(KeyValCountryNmPrV[CountryN].Val2);
  }
}

PBowDocBs TPiDb::GenBowDocBs() const {
  printf("Generating Bag-Of-Words...\n");
  // create document vector
  TStrV ProjNmV;
  TStrV ProjDescStrV;
  for (int ProjN=0; ProjN<GetProjs(); ProjN++){
    PPiRec Proj=GetProj(ProjN);
    // get project name & description
    TStr ProjNm=Proj->GetProjRef(this);
    //TStr ProjNm=/*Proj->GetProjProg(this)+"-"+*/Proj->GetProjAcronym(this);
    TStr ProjDescStr=
     Proj->GetProjTitle(this)+"; "+
     Proj->GetProjSubj(this)+"; "+
     Proj->GetProjDesc(this);
    // save project name & description
    ProjNmV.Add(ProjNm);
    ProjDescStrV.Add(ProjDescStr);
  }
  // create ngrams
  PSwSet SwSet=TSwSet::GetSwSet(swstEn523);
  PStemmer Stemmer=TStemmer::New(stmtPorter, true);
  PNGramBs NGramBs=
   TNGramBs::GetNGramBsFromHtmlStrV(ProjDescStrV, 3, 3, SwSet, Stemmer);
  NGramBs->SaveTxt("NGram.Txt");
  // create bag-of-words
  printf("\n");
  PBowDocBs BowDocBs=TBowDocBs::New();
  BowDocBs->PutNGramBs(NGramBs);
  {for (int ProjN=0; ProjN<GetProjs(); ProjN++){
    if (ProjN%10==0){printf("%d/%d\r", ProjN, GetProjs());}
    //PPiRec Proj=GetProj(ProjN);
    BowDocBs->AddHtmlDoc(ProjNmV[ProjN], TStrV(), ProjDescStrV[ProjN]);
  }
  printf("%d/%d\n", GetProjs(), GetProjs());}
  BowDocBs->AssertOk();
  // return bag-of-words
  printf("\nDone.\n");
  return BowDocBs;
}

PPiRec TPiDb::LoadProjHtml(const TStr& FNm, const PPiDb& PiDb){
  // create project
  PPiRec Proj=TPiRec::New();
  // load & open html file
  TStr HtmlStr=TStr::LoadTxt(FNm);
  PSIn HtmlSIn=TStrIn::New(HtmlStr);
  // traverse symbols
  THtmlLx Lx(HtmlSIn);
  // collect project fields
  while (Lx.GetSym()!=hsyEof){
    if ((Lx.Sym==hsyBTag)&&(Lx.UcChA=="<TABLE>")&&(Lx.GetArg("BORDER", "")=="1")){
      break;
    } else
    if ((Lx.Sym==hsyBTag)&&(Lx.UcChA=="<P>")){
      Lx.GetSym();
      if ((Lx.Sym==hsyBTag)&&(Lx.UcChA=="<B>")){
        TChA FldNm; TChA FldVal;
        // get field name
        Lx.GetSym();
        while (!((Lx.Sym==hsyETag)&&(Lx.UcChA=="<B>"))){
          FldNm+=Lx.PreSpaceChA; FldNm+=Lx.ChA; Lx.GetSym();}
        FldNm.Trunc();
        if ((FldNm.Len()>0)&&(FldNm.LastCh()==':')){FldNm.Pop();}
        // get field value
        Lx.GetSym();
        while (!((Lx.Sym==hsyETag)&&(Lx.UcChA=="<P>"))){
          if ((Lx.Sym!=hsyBTag)&&(Lx.Sym!=hsyETag)){
            FldVal+=Lx.PreSpaceChA; FldVal+=Lx.ChA;}
          Lx.GetSym();
        }
        FldVal.Trunc();
        // adaptations
        if (FldVal=="IP (Integrated Project)"){FldVal="IP";}
        if (FldVal=="NoE (Network of Excellence)"){FldVal="NoE";}
        if (FldVal=="STREP (Specific Targeted Research Project)"){FldVal="STREP";}
        if (FldVal=="CON (Coordination of research actions)"){FldVal="CON";}
        if (FldVal=="CA (Coordination action)"){FldVal="CA";}
        if (FldVal=="SSA (Specific Support Action)"){FldVal="SSA";}
        if (FldNm=="Project Cost"){FldVal=TPiRec::GetNrMoneyStr(FldVal);}
        if (FldNm=="Project Funding"){FldVal=TPiRec::GetNrMoneyStr(FldVal);}
        if (FldNm=="Duration"){FldVal=TPiRec::GetNrDurationStr(FldVal);}
        // saving field value
        Proj->AddFldNmVal(PiDb(), FldNm, FldVal);
        //printf("%s = %s\n", FldNm.CStr(), FldVal.CStr());
      }
    }
  }
  // collect organization names
  if ((Lx.Sym==hsyBTag)&&(Lx.UcChA=="<TABLE>")&&(Lx.GetArg("BORDER", "")=="1")){
    bool PrimeContractorP=false;
    while (Lx.GetSym()!=hsyEof){
      if ((Lx.Sym==hsyBTag)&&(Lx.UcChA=="<TD>")&&(Lx.GetArg("ALIGN", "")=="center")){
        // get info on prime-contractor & other-contractors flags
        Lx.GetSym(); TChA FldVal;
        while (!((Lx.Sym==hsyETag)&&(Lx.UcChA=="<TD>"))){
          if ((Lx.Sym!=hsyBTag)&&(Lx.Sym!=hsyETag)){
            FldVal+=Lx.PreSpaceChA; FldVal+=Lx.ChA;}
          Lx.GetSym();
        }
        FldVal.Trunc();
        if (FldVal=="Prime Contractor"){PrimeContractorP=true;}
      } else
      if ((Lx.Sym==hsyBTag)&&(Lx.UcChA=="<TABLE>")&&(Lx.GetArg("BORDER", "")=="0")){
        PPiRec Org=TPiRec::New();
        // get organization fields
        while (Lx.GetSym()!=hsyEof){
          if ((Lx.Sym==hsyETag)&&(Lx.UcChA=="<TABLE>")){
            break;
          } else
          if ((Lx.Sym==hsyBTag)&&(Lx.UcChA=="<TD>")){
            // get field
            Lx.GetSym();
            if ((Lx.Sym==hsyBTag)&&(Lx.UcChA=="<B>")){
              TChA FldNm; TChA FldVal;
              // get field name
              Lx.GetSym();
              while (!((Lx.Sym==hsyETag)&&(Lx.UcChA=="<B>"))){
                FldNm+=Lx.PreSpaceChA; FldNm+=Lx.ChA; Lx.GetSym();}
              FldNm.Trunc();
              if ((FldNm.Len()>0)&&(FldNm.LastCh()==':')){FldNm.Pop();}
              // get field value
              Lx.GetSym();
              while (!((Lx.Sym==hsyETag)&&(Lx.UcChA=="<TD>"))){
                if ((Lx.Sym!=hsyBTag)&&(Lx.Sym!=hsyETag)){
                  FldVal+=Lx.PreSpaceChA; FldVal+=Lx.ChA;
                } else
                if ((Lx.Sym==hsyBTag)&&(Lx.UcChA=="<BR>")){
                  FldVal+=" / ";
                }
                Lx.GetSym();
              }
              FldVal.Trunc();
              // adaptations
              if ((FldNm=="Organisation Name")||(FldNm=="Organisation")){
                FldNm="Organisation";
                FldVal.ChangeCh('\"', ' ');
                FldVal.ChangeCh('.', ' ');
                FldVal.ChangeCh('-', ' ');
                FldVal.Trunc();
                FldVal.ToUc();
                if (FldVal=="JOSEF STEPHAN INSTITUTE"){FldVal="JOZEF STEFAN INSTITUTE";}
                if (FldVal=="FRAUNHOFER GESELLSCHAFT ZUR FORDERUNG DER ANGEWANDTEN FORSCHUNG E V"){
                  FldVal="FRAUNHOFER GESELLSCHAFT ZUR FOERDERUNG DER ANGEWANDTEN FORSCHUNG E V";}
                if (FldVal=="SIRMA AI EAD"){FldVal="SIRMA AI LTD";}
              }
              if (FldNm=="City"){FldVal.ToUc();}
              if (FldNm=="Org. Country"){FldNm="Country";}
              if (FldVal=="RUSSIAN FEDERATION"){FldVal="RUSSIA";}
              // saving field value
              Org->AddFldNmVal(PiDb(), FldNm, FldVal);
              //printf("'%s' = '%s'\n", FldNm.CStr(), FldVal.CStr());
            }
          }
        }
        // prime contractor
        if (PrimeContractorP){
          Proj->AddFldNmVal(PiDb(), "Prime Contractor", Org->GetOrgNm(PiDb()));
          PrimeContractorP=false;
        }
        // save organisation
        if (!PiDb->IsProjOrg(Proj->GetProjRef(PiDb()), Org->GetOrgNm(PiDb()))){
          PiDb->AddOrgOnProj(Org, Proj->GetProjRef(PiDb()));}
      }
    }
  }
  // return results
  return Proj;
}

PPiDb TPiDb::LoadHtml(const TStr& FPath, const TStr& FBasePrefixStr){
  // create project-base
  PPiDb PiDb=TPiDb::New();
  // traverse project files
  TFFile FFile(TStr::GetNrFPath(FPath)+FBasePrefixStr); TStr FNm;
  while (FFile.Next(FNm)){
    printf("%s\n", FNm.CStr());
    PPiRec Proj=LoadProjHtml(FNm, PiDb);
    PiDb->AddProj(Proj);
  }
  // add global project fields
  for (int ProjN=0; ProjN<PiDb->GetProjs(); ProjN++){
    PPiRec Proj=PiDb->GetProj(ProjN);
    TStr ProjRef=Proj->GetProjRef(PiDb());
    Proj->AddFldNmVal(PiDb(), "Partners", PiDb->GetProjOrgsStr(ProjRef));
    if (Proj->GetProjValue(PiDb())=="Unknown"){
      Proj->AddFldNmVal(PiDb(), "Project Cost", Proj->GetProjFund(PiDb()));}
  }
  // add global organisation fields
  for (int OrgN=0; OrgN<PiDb->GetOrgs(); OrgN++){
    PPiRec Org=PiDb->GetOrg(OrgN);
    TStr OrgNm=Org->GetOrgNm(PiDb());
    Org->AddFldNmVal(PiDb(), "Projects", PiDb->GetOrgProjsStr(OrgNm));
    Org->AddFldNmVal(PiDb(), "Funding", PiDb->GetOrgFundingStr(OrgNm));
  }
  // create country table
  TStrIntH CountryNmToOrgsH;
  TStrIntH CountryNmToProjsH;
  TStrFltH CountryNmToFundingH;
  for (int OrgN=0; OrgN<PiDb->GetOrgs(); OrgN++){
    PPiRec Org=PiDb->GetOrg(OrgN);
    TStr OrgNm=Org->GetOrgNm(PiDb());
    TStr CountryNm=Org->GetOrgCountry(PiDb());
    CountryNmToOrgsH.AddDat(CountryNm)++;
    CountryNmToProjsH.AddDat(CountryNm)+=PiDb->GetOrgProjs(OrgNm);
    CountryNmToFundingH.AddDat(CountryNm)+=PiDb->GetOrgFunding(OrgNm);
  }
  // per capita
  PCiaWFBBs CiaWFBBs=TCiaWFBBs::LoadBin("CiaWFB2002.Bin");
  for (int CountryN=0; CountryN<CountryNmToOrgsH.Len(); CountryN++){
    TStr CountryNm=CountryNmToOrgsH.GetKey(CountryN);
    int Orgs=CountryNmToOrgsH.GetDat(CountryNm);
    int Projs=CountryNmToProjsH.GetDat(CountryNm);
    double Funding=CountryNmToFundingH.GetDat(CountryNm);
    // population
    PCiaWFBCountry CiaWFBCountry;
    double Population=0;
    double GdpPerCapita=0;
    if (CiaWFBBs->IsCountry(CountryNm, CiaWFBCountry)){
      Population=CiaWFBCountry->GetFldValNum("Population");
      GdpPerCapita=CiaWFBCountry->GetFldValNum("GDP - per capita");
    }
    // funding per capita
    double FundingPerCapita=0;
    if (Population>0){
      FundingPerCapita=1000000*Funding/Population;}
    // create record
    PPiRec Country=TPiRec::New();
    Country->AddFldNmVal(PiDb(), "Country", CountryNm);
    Country->AddFldNmVal(PiDb(), "Country Organisations", TInt::GetStr(Orgs, "%4d org."));
    Country->AddFldNmVal(PiDb(), "Country Projects", TInt::GetStr(Projs, "%4d proj."));
    Country->AddFldNmVal(PiDb(), "Country Funding", TFlt::GetStr(Funding, "%8.2fMeuro"));
    if (Population>0){
      Country->AddFldNmVal(PiDb(), "Country Population", TFlt::GetStr(Population/1000000, "%8.2fM"));}
    if (GdpPerCapita>0){
      Country->AddFldNmVal(PiDb(), "Country GDP Per Capita", TFlt::GetStr(GdpPerCapita, "%8.0f$"));}
    if (FundingPerCapita>0){
      Country->AddFldNmVal(PiDb(), "Country Funding Per Capita", TFlt::GetStr(FundingPerCapita, "%8.2f$/cap"));}
    if (GdpPerCapita>0){
      Country->AddFldNmVal(PiDb(), "Country Funding Per GDP", TFlt::GetStr(FundingPerCapita/GdpPerCapita, "%12.8fGDP"));}
    // add country to database
    PiDb->AddCountry(Country);
  }

  // return results
  return PiDb;
}

void TPiDb::SaveTxtLnDoc(const TStr& FNm){
  TFOut FOut(FNm); FILE* fOut=FOut.GetFileId();
  for (int ProjN=0; ProjN<GetProjs(); ProjN++){
    PPiRec Proj=GetProj(ProjN);
    // get project name & description
    //TStr ProjNm=Proj->GetProjRef(this);
    TStr ProjNm=Proj->GetProjProg(this)+"-"+Proj->GetProjAcronym(this);
    TStr CatNm=Proj->GetProjProg(this);
    TStr ProjDescStr=
     Proj->GetProjTitle(this)+"; "+
     Proj->GetProjSubj(this)+"; "+
     Proj->GetProjDesc(this);
    ProjDescStr.ChangeChAll('\r', ' ');
    ProjDescStr.ChangeChAll('\n', ' ');
    // save project name & category & description
    fprintf(fOut, "%s !%s %s\n", ProjNm.CStr(), CatNm.CStr(), ProjDescStr.CStr());
  }
}

PPiDb TPiDb::LoadFP6IstTab(const TStr& ProjOrgTabFNm, const TStr& ProjDocXmlFNm){
  // create project-base
  PPiDb PiDb=TPiDb::New();
  // Project-Organizations
  PSs ProjOrgSs=TSs::LoadTxt(ssfTabSep, ProjOrgTabFNm, TNotify::StdNotify);
  int ProjRef_FldN=ProjOrgSs->GetFldX("PROJECT_REF", "Project Reference"); EAssert(ProjRef_FldN!=-1);
  int StartDate_FldN=ProjOrgSs->GetFldX("START_DATE"); EAssert(StartDate_FldN!=-1);
  int EndDate_FldN=ProjOrgSs->GetFldX("END_DATE"); EAssert(EndDate_FldN!=-1);
  int Title_FldN=ProjOrgSs->GetFldX("TITLE", "Title"); EAssert(Title_FldN!=-1);
  int Acronym_FldN=ProjOrgSs->GetFldX("ACRONYM", "Project Acronym"); EAssert(Acronym_FldN!=-1);
  int CallNm_FldN=ProjOrgSs->GetFldX("CALL_IDENTIFIER", "Call"); EAssert(CallNm_FldN!=-1);
  int StrategicObj_FldN=ProjOrgSs->GetFldX("STRATEGIC_OBJECTIVE"); EAssert(StrategicObj_FldN!=-1);
  int ContractType_FldN=ProjOrgSs->GetFldX("CONTRACT_TYPE.DESCRIPTION", "Contract Type"); EAssert(ContractType_FldN!=-1);
//  int ParticipantNum_FldN=ProjOrgSs->GetFldX("PARTICIPANT_NO");
  int PartnerRole_FldN=ProjOrgSs->GetFldX("PARTNER_ROLE.DESCRIPTION"); EAssert(PartnerRole_FldN!=-1);
  int Org_FldN=ProjOrgSs->GetFldX("Organisation"); EAssert(Org_FldN!=-1);
//  int ShortNm_FldN=ProjOrgSs->GetFldX("SHORT_NAME in Database");
  int LegalNm_FldN=ProjOrgSs->GetFldX("Organisation"); EAssert(LegalNm_FldN!=-1);
//  int LegalNm_FldN=ProjOrgSs->GetFldX("LEGAL_NAME", "Organisation"); EAssert(LegalNm_FldN!=-1);
//  int Department_FldN=ProjOrgSs->GetFldX("DEPARTMENT");
  int OrgType_FldN=ProjOrgSs->GetFldX("ORGANISATION_TYPE", "Organisation Type"); EAssert(OrgType_FldN!=-1);
  int SMEFlag_FldN=ProjOrgSs->GetFldX("SME Flag"); EAssert(SMEFlag_FldN!=-1);
//  int Consultants_FldN=ProjOrgSs->GetFldX("Consultants");
  int WorkingCity_FldN=ProjOrgSs->GetFldX("WORKING_CITY", "City"); EAssert(WorkingCity_FldN!=-1);
//  int WorkingNutsCd_FldN=ProjOrgSs->GetFldX("WORKING_NUTS_CODE");
  int WorkingCountryCd_FldN=ProjOrgSs->GetFldX("WORKING_COUNTRY_CODE", "Country"); EAssert(WorkingCountryCd_FldN!=-1);
  for (int ProjN=1; ProjN<ProjOrgSs->GetYLen(); ProjN++){
    // extract project-fields
    TStr ProjRefVal=ProjOrgSs->GetVal(ProjRef_FldN, ProjN);
    TStr StartDateVal=ProjOrgSs->GetVal(StartDate_FldN, ProjN);
    TStr EndDateVal=ProjOrgSs->GetVal(EndDate_FldN, ProjN);
    TStr TitleVal=ProjOrgSs->GetVal(Title_FldN, ProjN);
    TStr AcronymVal=ProjOrgSs->GetVal(Acronym_FldN, ProjN);
    TStr CallNmVal=ProjOrgSs->GetVal(CallNm_FldN, ProjN);
    TStr ObjectiveVal=ProjOrgSs->GetVal(StrategicObj_FldN, ProjN);
    TStr ContractTypeVal=ProjOrgSs->GetVal(ContractType_FldN, ProjN);
    // duration
    TStr DurationValStr="Unknown";
    TSecTm StartDateSecTm=TSecTm::GetDtTmFromDmyStr(StartDateVal);
    TSecTm EndDateSecTm=TSecTm::GetDtTmFromDmyStr(EndDateVal);
    if (StartDateSecTm.IsDef()&&EndDateSecTm.IsDef()){
      uint DurationSecs=TSecTm::GetDSecs(StartDateSecTm, EndDateSecTm);
      DurationValStr=
       TInt::GetStr(DurationSecs/(30*3600*24))+" months";
    }
    // adaptations
    if (ContractTypeVal=="Integrated Project"){ContractTypeVal="IP";}
    if (ContractTypeVal=="Specific Targeted Research Project"){ContractTypeVal="STREP";}
    if (ContractTypeVal=="Network of Excellence"){ContractTypeVal="NoE";}
    if (ContractTypeVal=="Specific Support Action"){ContractTypeVal="SSA";}
    if (ContractTypeVal=="Coordination Action"){ContractTypeVal="CA";}
    // get project-record
    PPiRec Proj;
    if (PiDb->IsProj(ProjRefVal)){
      // retrieve project
      Proj=PiDb->GetProj(ProjRefVal);
    } else {
      // create project-record
      Proj=TPiRec::New();
      Proj->AddFldNmVal(PiDb(), "Project Reference", ProjRefVal);
      Proj->AddFldNmVal(PiDb(), "Title", TitleVal);
      Proj->AddFldNmVal(PiDb(), "Project Acronym", AcronymVal);
      Proj->AddFldNmVal(PiDb(), "Call", CallNmVal);
      Proj->AddFldNmVal(PiDb(), "Subprogramme Area", ObjectiveVal);
      Proj->AddFldNmVal(PiDb(), "Contract Type", ContractTypeVal);
      Proj->AddFldNmVal(PiDb(), "Start Date", StartDateVal);
      Proj->AddFldNmVal(PiDb(), "End Date", EndDateVal);
      Proj->AddFldNmVal(PiDb(), "Duration", DurationValStr);
      PiDb->AddProj(Proj);
    }
    // extract organization-fields
    TStr PartnerRoleVal=ProjOrgSs->GetVal(PartnerRole_FldN, ProjN);
    TStr OrganisationNmVal=ProjOrgSs->GetVal(Org_FldN, ProjN);
    TStr LegalNmVal=ProjOrgSs->GetVal(LegalNm_FldN, ProjN);
    TStr OrgTypeVal=ProjOrgSs->GetVal(OrgType_FldN, ProjN);
    TStr SMEFlagVal=ProjOrgSs->GetVal(SMEFlag_FldN, ProjN);
    TStr CountryCdVal=ProjOrgSs->GetVal(WorkingCountryCd_FldN, ProjN);
    TStr CityNmVal=ProjOrgSs->GetVal(WorkingCity_FldN, ProjN);
    if (LegalNmVal.Empty()){LegalNmVal=OrganisationNmVal;}
    // create organization-record
    PPiRec Org=TPiRec::New();
    Org->AddFldNmVal(PiDb(), "Organisation", LegalNmVal);
    Org->AddFldNmVal(PiDb(), "Organisation Type", OrgTypeVal);
    Org->AddFldNmVal(PiDb(), "SME", (SMEFlagVal=="X")?"Yes":"No");
    Org->AddFldNmVal(PiDb(), "City", CityNmVal);
    Org->AddFldNmVal(PiDb(), "Country", CountryCdVal);
    // add organization to project
    PiDb->AddOrgOnProj(Org, ProjRefVal);
    if (PartnerRoleVal=="Coordinator"){
      Proj->AddFldNmVal(PiDb(), "Prime Contractor", Org->GetOrgNm(PiDb()));}
  }

  // Project-Documents
  if (!ProjDocXmlFNm.Empty()){
    printf("Loading '%s' ...\n", ProjDocXmlFNm.CStr());
    PXmlDoc ProjDocXmlDoc=TXmlDoc::LoadTxt(ProjDocXmlFNm);
    TXmlTokV ProjDocXmlTokV;
    ProjDocXmlDoc->GetTagTokV("Projects|Project", ProjDocXmlTokV);
    for (int ProjN=0; ProjN<ProjDocXmlTokV.Len(); ProjN++){
      printf("  %d/%d\r", 1+ProjN, ProjDocXmlTokV.Len());
      TStr ProjRef=ProjDocXmlTokV[ProjN]->GetArgVal("id");
      TStr ProjNm=ProjDocXmlTokV[ProjN]->GetTagTokStr("Acronym");
      TStr ProjTitleStr=ProjDocXmlTokV[ProjN]->GetTagTokStr("projectTitle");
      TStr ProjAbstractStr=ProjDocXmlTokV[ProjN]->GetTagTokStrOrDf("projectAbstract");
      if (PiDb->IsProj(ProjRef)){
        PPiRec Proj=PiDb->GetProj(ProjRef);
        Proj->AddFldNmVal(PiDb(), "Objective", ProjAbstractStr);
      }
    }
    printf("\nDone.\n");
  }

  // add global project fields
  for (int ProjN=0; ProjN<PiDb->GetProjs(); ProjN++){
    PPiRec Proj=PiDb->GetProj(ProjN);
    TStr ProjRef=Proj->GetProjRef(PiDb());
    Proj->AddFldNmVal(PiDb(), "Partners", PiDb->GetProjOrgsStr(ProjRef));
  }
  // add global organisation fields
  for (int OrgN=0; OrgN<PiDb->GetOrgs(); OrgN++){
    PPiRec Org=PiDb->GetOrg(OrgN);
    TStr OrgNm=Org->GetOrgNm(PiDb());
    Org->AddFldNmVal(PiDb(), "Projects", PiDb->GetOrgProjsStr(OrgNm));
  }
  // create country table
  TStrIntH CountryNmToOrgsH;
  TStrIntH CountryNmToProjsH;
  for (int OrgN=0; OrgN<PiDb->GetOrgs(); OrgN++){
    PPiRec Org=PiDb->GetOrg(OrgN);
    TStr OrgNm=Org->GetOrgNm(PiDb());
    TStr CountryNm=Org->GetOrgCountry(PiDb());
    CountryNmToOrgsH.AddDat(CountryNm)++;
    CountryNmToProjsH.AddDat(CountryNm)+=PiDb->GetOrgProjs(OrgNm);
  }
  // countries
  for (int CountryN=0; CountryN<CountryNmToOrgsH.Len(); CountryN++){
    TStr CountryNm=CountryNmToOrgsH.GetKey(CountryN);
    int Orgs=CountryNmToOrgsH.GetDat(CountryNm);
    int Projs=CountryNmToProjsH.GetDat(CountryNm);
    // create record
    PPiRec Country=TPiRec::New();
    Country->AddFldNmVal(PiDb(), "Country", CountryNm);
    Country->AddFldNmVal(PiDb(), "Country Organisations", TInt::GetStr(Orgs, "%4d org."));
    Country->AddFldNmVal(PiDb(), "Country Projects", TInt::GetStr(Projs, "%4d proj."));
    // add country to database
    PiDb->AddCountry(Country);
  }
  // create pi-graph
  PiDb->PiGraph=TPiGraph::New(PiDb);
  // create bow
  PiDb->BowDocBs=PiDb->GenBowDocBs();

  // return
  return PiDb;
}

PPiDb TPiDb::LoadFP7IstTab(const TStr& ProjOrgTabFNm){
  // create project-base
  PPiDb PiDb=TPiDb::New();
  // Project-Organizations
  PSs ProjOrgSs=TSs::LoadTxt(ssfTabSep, ProjOrgTabFNm, TNotify::StdNotify);
  int ProjRef_FldN=ProjOrgSs->GetFldX("PROPOSAL", "Project Reference"); EAssert(ProjRef_FldN!=-1);
  int StartDate_FldN=ProjOrgSs->GetFldX("START DATE"); EAssert(StartDate_FldN!=-1);
  int EndDate_FldN=ProjOrgSs->GetFldX("END DATE"); EAssert(EndDate_FldN!=-1);
  int Title_FldN=ProjOrgSs->GetFldX("TITLE", "Title"); EAssert(Title_FldN!=-1);
  int Acronym_FldN=ProjOrgSs->GetFldX("ACRONYM", "Project Acronym"); EAssert(Acronym_FldN!=-1);
  int CallNm_FldN=ProjOrgSs->GetFldX("CALL IDENTIFIER", "Call"); EAssert(CallNm_FldN!=-1);
  int StrategicObj_FldN=ProjOrgSs->GetFldX("STRATEGIC OBJECTIVE"); EAssert(StrategicObj_FldN!=-1);
  int ContractType_FldN=ProjOrgSs->GetFldX("INSTRUMENT", "Contract Type"); EAssert(ContractType_FldN!=-1);
//  int ParticipantNum_FldN=ProjOrgSs->GetFldX("PARTICIPANT_NO"); EAssert(!=-1);
  int PartnerRole_FldN=ProjOrgSs->GetFldX("PARTNER ROLE DESCRIPTION"); EAssert(PartnerRole_FldN!=-1);
  int Org_FldN=ProjOrgSs->GetFldX("SHORT NAME"); EAssert(Org_FldN!=-1);
//  int ShortNm_FldN=ProjOrgSs->GetFldX("SHORT_NAME in Database"); EAssert(!=-1);
  int LegalNm_FldN=ProjOrgSs->GetFldX("LEAGL NAME", "Organisation"); EAssert(LegalNm_FldN!=-1);
//  int Department_FldN=ProjOrgSs->GetFldX("DEPARTMENT"); EAssert(!=-1);
//  int OrgType_FldN=ProjOrgSs->GetFldX("XXX", "Organisation Type"); EAssert(!=-1);
//  int SMEFlag_FldN=ProjOrgSs->GetFldX("XXX"); EAssert(!=-1);
//  int Consultants_FldN=ProjOrgSs->GetFldX("Consultants"); EAssert(!=-1);
  int WorkingCity_FldN=ProjOrgSs->GetFldX("LEGAL CITY", "City"); EAssert(WorkingCity_FldN!=-1);
//  int WorkingNutsCd_FldN=ProjOrgSs->GetFldX("WORKING_NUTS_CODE"); EAssert(!=-1);
  int WorkingCountryCd_FldN=ProjOrgSs->GetFldX("LEGAL COUNTRY CODE", "Country"); EAssert(WorkingCountryCd_FldN!=-1);
  for (int ProjN=1; ProjN<ProjOrgSs->GetYLen(); ProjN++){
    // extract project-fields
    TStr ProjRefVal=ProjOrgSs->GetVal(ProjRef_FldN, ProjN);
    TStr StartDateVal=ProjOrgSs->GetVal(StartDate_FldN, ProjN);
    TStr EndDateVal=ProjOrgSs->GetVal(EndDate_FldN, ProjN);
    TStr TitleVal=ProjOrgSs->GetVal(Title_FldN, ProjN);
    TStr AcronymVal=ProjOrgSs->GetVal(Acronym_FldN, ProjN);
    TStr CallNmVal=ProjOrgSs->GetVal(CallNm_FldN, ProjN);
    TStr ObjectiveVal=ProjOrgSs->GetVal(StrategicObj_FldN, ProjN);
    TStr ContractTypeVal=ProjOrgSs->GetVal(ContractType_FldN, ProjN);
    // duration
    TStr DurationValStr="Unknown";
    TSecTm StartDateSecTm=TSecTm::GetDtTmFromDmyStr(StartDateVal);
    TSecTm EndDateSecTm=TSecTm::GetDtTmFromDmyStr(EndDateVal);
    if (StartDateSecTm.IsDef()&&EndDateSecTm.IsDef()){
      uint DurationSecs=TSecTm::GetDSecs(StartDateSecTm, EndDateSecTm);
      DurationValStr=
       TInt::GetStr(DurationSecs/(30*3600*24))+" months";
    }
    // adaptations
    if (ContractTypeVal=="Large-scale integrating project"){ContractTypeVal="IP";}
    if (ContractTypeVal=="Small or medium-scale focused research project -STREP"){ContractTypeVal="STREP";}
    if (ContractTypeVal=="Network of Excellence"){ContractTypeVal="NoE";}
    if (ContractTypeVal=="Coordination and support action - Support"){ContractTypeVal="SSA";}
    if (ContractTypeVal=="Coordination and support action - Coordination"){ContractTypeVal="CA";}
    // get project-record
    PPiRec Proj;
    if (PiDb->IsProj(ProjRefVal)){
      // retrieve project
      Proj=PiDb->GetProj(ProjRefVal);
    } else {
      // create project-record
      Proj=TPiRec::New();
      Proj->AddFldNmVal(PiDb(), "Project Reference", ProjRefVal);
      Proj->AddFldNmVal(PiDb(), "Title", TitleVal);
      Proj->AddFldNmVal(PiDb(), "Project Acronym", AcronymVal);
      Proj->AddFldNmVal(PiDb(), "Call", CallNmVal);
      Proj->AddFldNmVal(PiDb(), "Subprogramme Area", ObjectiveVal);
      Proj->AddFldNmVal(PiDb(), "Contract Type", ContractTypeVal);
      Proj->AddFldNmVal(PiDb(), "Start Date", StartDateVal);
      Proj->AddFldNmVal(PiDb(), "End Date", EndDateVal);
      Proj->AddFldNmVal(PiDb(), "Duration", DurationValStr);
      PiDb->AddProj(Proj);
    }
    // extract organization-fields
    TStr PartnerRoleVal=ProjOrgSs->GetVal(PartnerRole_FldN, ProjN);
    TStr OrganisationNmVal=ProjOrgSs->GetVal(Org_FldN, ProjN);
    TStr LegalNmVal=ProjOrgSs->GetVal(LegalNm_FldN, ProjN);
    TStr OrgTypeVal="";
    TStr SMEFlagVal="";
    TStr CountryCdVal=ProjOrgSs->GetVal(WorkingCountryCd_FldN, ProjN);
    TStr CityNmVal=ProjOrgSs->GetVal(WorkingCity_FldN, ProjN);
    if (LegalNmVal.Empty()){LegalNmVal=OrganisationNmVal;}
    // create organization-record
    PPiRec Org=TPiRec::New();
    Org->AddFldNmVal(PiDb(), "Organisation", LegalNmVal);
    Org->AddFldNmVal(PiDb(), "Organisation Type", OrgTypeVal);
    Org->AddFldNmVal(PiDb(), "SME", (SMEFlagVal=="X")?"Yes":"No");
    Org->AddFldNmVal(PiDb(), "City", CityNmVal);
    Org->AddFldNmVal(PiDb(), "Country", CountryCdVal);
    // add organization to project
    PiDb->AddOrgOnProj(Org, ProjRefVal);
    if (PartnerRoleVal=="Coordinator"){
      Proj->AddFldNmVal(PiDb(), "Prime Contractor", Org->GetOrgNm(PiDb()));}
  }

  // add global project fields
  for (int ProjN=0; ProjN<PiDb->GetProjs(); ProjN++){
    PPiRec Proj=PiDb->GetProj(ProjN);
    TStr ProjRef=Proj->GetProjRef(PiDb());
    Proj->AddFldNmVal(PiDb(), "Partners", PiDb->GetProjOrgsStr(ProjRef));
  }
  // add global organisation fields
  for (int OrgN=0; OrgN<PiDb->GetOrgs(); OrgN++){
    PPiRec Org=PiDb->GetOrg(OrgN);
    TStr OrgNm=Org->GetOrgNm(PiDb());
    Org->AddFldNmVal(PiDb(), "Projects", PiDb->GetOrgProjsStr(OrgNm));
  }
  // create country table
  TStrIntH CountryNmToOrgsH;
  TStrIntH CountryNmToProjsH;
  for (int OrgN=0; OrgN<PiDb->GetOrgs(); OrgN++){
    PPiRec Org=PiDb->GetOrg(OrgN);
    TStr OrgNm=Org->GetOrgNm(PiDb());
    TStr CountryNm=Org->GetOrgCountry(PiDb());
    CountryNmToOrgsH.AddDat(CountryNm)++;
    CountryNmToProjsH.AddDat(CountryNm)+=PiDb->GetOrgProjs(OrgNm);
  }
  // countries
  for (int CountryN=0; CountryN<CountryNmToOrgsH.Len(); CountryN++){
    TStr CountryNm=CountryNmToOrgsH.GetKey(CountryN);
    int Orgs=CountryNmToOrgsH.GetDat(CountryNm);
    int Projs=CountryNmToProjsH.GetDat(CountryNm);
    // create record
    PPiRec Country=TPiRec::New();
    Country->AddFldNmVal(PiDb(), "Country", CountryNm);
    Country->AddFldNmVal(PiDb(), "Country Organisations", TInt::GetStr(Orgs, "%4d org."));
    Country->AddFldNmVal(PiDb(), "Country Projects", TInt::GetStr(Projs, "%4d proj."));
    // add country to database
    PiDb->AddCountry(Country);
  }
  // create pi-graph
  PiDb->PiGraph=TPiGraph::New(PiDb);
  // create bow
  PiDb->BowDocBs=PiDb->GenBowDocBs();

  // return
  return PiDb;
}

PPiDb TPiDb::LoadFP5IstTab(const TStr& ProjOrgTabFNm){
  // create project-base
  PPiDb PiDb=TPiDb::New();
  // Project-Organizations
  PSs ProjOrgSs=TSs::LoadTxt(ssfTabSep, ProjOrgTabFNm, TNotify::StdNotify);
  int ProjRef_FldN=ProjOrgSs->GetFldX("PROJECT REF", "Project Reference", 1); EAssert(ProjRef_FldN!=-1);
  int StartDate_FldN=ProjOrgSs->GetFldX("START DATE", "", 1); EAssert(StartDate_FldN!=-1);
  int EndDate_FldN=ProjOrgSs->GetFldX("END DATE", "", 1); EAssert(EndDate_FldN!=-1);
  int Title_FldN=ProjOrgSs->GetFldX("TITLE", "Title", 1); EAssert(Title_FldN!=-1);
  int Acronym_FldN=ProjOrgSs->GetFldX("ACRONYM", "Project Acronym", 1); EAssert(Acronym_FldN!=-1);
  int CallNm_FldN=ProjOrgSs->GetFldX("CALL IDENTIFIER", "Call", 1); EAssert(CallNm_FldN!=-1);
  int StrategicObj_FldN=ProjOrgSs->GetFldX("STRATEGIC OBJECTIVE", "", 1); EAssert(StrategicObj_FldN!=-1);
  int ContractType_FldN=ProjOrgSs->GetFldX("INSTRUMENT", "Contract Type", 1); EAssert(ContractType_FldN!=-1);
//  int ParticipantNum_FldN=ProjOrgSs->GetFldX("PARTICIPANT_NO"); EAssert(!=-1);
  int PartnerRole_FldN=ProjOrgSs->GetFldX("PARTNER ROLE DESCRIPTION", "", 1); EAssert(PartnerRole_FldN!=-1);
  int Org_FldN=ProjOrgSs->GetFldX("SHORT NAME", "", 1); EAssert(Org_FldN!=-1);
//  int ShortNm_FldN=ProjOrgSs->GetFldX("SHORT_NAME in Database"); EAssert(!=-1);
  int LegalNm_FldN=ProjOrgSs->GetFldX("LEGAL NAME", "Organisation", 1); EAssert(LegalNm_FldN!=-1);
//  int Department_FldN=ProjOrgSs->GetFldX("DEPARTMENT"); EAssert(!=-1);
  int OrgType_FldN=ProjOrgSs->GetFldX("ORGANISATION TYPE", "Organisation Type", 1); EAssert(OrgType_FldN!=-1);
//  int SMEFlag_FldN=ProjOrgSs->GetFldX("XXX"); EAssert(SMEFlag_FldN!=-1);
//  int Consultants_FldN=ProjOrgSs->GetFldX("Consultants"); EAssert(!=-1);
  int WorkingCity_FldN=ProjOrgSs->GetFldX("LEGAL CITY", "City", 1); EAssert(WorkingCity_FldN!=-1);
//  int WorkingNutsCd_FldN=ProjOrgSs->GetFldX("WORKING_NUTS_CODE"); EAssert(!=-1);
  int WorkingCountryCd_FldN=ProjOrgSs->GetFldX("LEGAL COUNTRY CODE", "Country", 1); EAssert(WorkingCountryCd_FldN!=-1);
  for (int ProjN=1; ProjN<ProjOrgSs->GetYLen(); ProjN++){
    // extract project-fields
    TStr ProjRefVal=ProjOrgSs->GetVal(ProjRef_FldN, ProjN);
    TStr StartDateVal=ProjOrgSs->GetVal(StartDate_FldN, ProjN);
    TStr EndDateVal=ProjOrgSs->GetVal(EndDate_FldN, ProjN);
    TStr TitleVal=ProjOrgSs->GetVal(Title_FldN, ProjN);
    TStr AcronymVal=ProjOrgSs->GetVal(Acronym_FldN, ProjN);
    TStr CallNmVal=ProjOrgSs->GetVal(CallNm_FldN, ProjN);
    TStr ObjectiveVal=ProjOrgSs->GetVal(StrategicObj_FldN, ProjN);
    TStr ContractTypeVal=ProjOrgSs->GetVal(ContractType_FldN, ProjN);
    // duration
    TStr DurationValStr="Unknown";
    TSecTm StartDateSecTm=TSecTm::GetDtTmFromDmyStr(StartDateVal);
    TSecTm EndDateSecTm=TSecTm::GetDtTmFromDmyStr(EndDateVal);
    if (StartDateSecTm.IsDef()&&EndDateSecTm.IsDef()){
      uint DurationSecs=TSecTm::GetDSecs(StartDateSecTm, EndDateSecTm);
      DurationValStr=
       TInt::GetStr(DurationSecs/(30*3600*24))+" months";
    }
    // adaptations
    if (ContractTypeVal=="Large-scale integrating project"){ContractTypeVal="IP";}
    if (ContractTypeVal=="Small or medium-scale focused research project -STREP"){ContractTypeVal="STREP";}
    if (ContractTypeVal=="Network of Excellence"){ContractTypeVal="NoE";}
    if (ContractTypeVal=="Coordination and support action - Support"){ContractTypeVal="SSA";}
    if (ContractTypeVal=="Coordination and support action - Coordination"){ContractTypeVal="CA";}
    // get project-record
    PPiRec Proj;
    if (PiDb->IsProj(ProjRefVal)){
      // retrieve project
      Proj=PiDb->GetProj(ProjRefVal);
    } else {
      // create project-record
      Proj=TPiRec::New();
      Proj->AddFldNmVal(PiDb(), "Project Reference", ProjRefVal);
      Proj->AddFldNmVal(PiDb(), "Title", TitleVal);
      Proj->AddFldNmVal(PiDb(), "Project Acronym", AcronymVal);
      Proj->AddFldNmVal(PiDb(), "Call", CallNmVal);
      Proj->AddFldNmVal(PiDb(), "Subprogramme Area", ObjectiveVal);
      Proj->AddFldNmVal(PiDb(), "Contract Type", ContractTypeVal);
      Proj->AddFldNmVal(PiDb(), "Start Date", StartDateVal);
      Proj->AddFldNmVal(PiDb(), "End Date", EndDateVal);
      Proj->AddFldNmVal(PiDb(), "Duration", DurationValStr);
      PiDb->AddProj(Proj);
    }
    // extract organization-fields
    TStr PartnerRoleVal=ProjOrgSs->GetVal(PartnerRole_FldN, ProjN);
    TStr OrganisationNmVal=ProjOrgSs->GetVal(Org_FldN, ProjN);
    TStr LegalNmVal=ProjOrgSs->GetVal(LegalNm_FldN, ProjN);
    TStr OrgTypeVal=ProjOrgSs->GetVal(OrgType_FldN, ProjN);
    TStr SMEFlagVal="";
    TStr CountryCdVal=ProjOrgSs->GetVal(WorkingCountryCd_FldN, ProjN);
    TStr CityNmVal=ProjOrgSs->GetVal(WorkingCity_FldN, ProjN);
    if (LegalNmVal.Empty()){LegalNmVal=OrganisationNmVal;}
    // create organization-record
    PPiRec Org=TPiRec::New();
    Org->AddFldNmVal(PiDb(), "Organisation", LegalNmVal);
    Org->AddFldNmVal(PiDb(), "Organisation Type", OrgTypeVal);
    Org->AddFldNmVal(PiDb(), "SME", (SMEFlagVal=="X")?"Yes":"No");
    Org->AddFldNmVal(PiDb(), "City", CityNmVal);
    Org->AddFldNmVal(PiDb(), "Country", CountryCdVal);
    // add organization to project
    PiDb->AddOrgOnProj(Org, ProjRefVal);
    if (PartnerRoleVal=="Coordinator"){
      Proj->AddFldNmVal(PiDb(), "Prime Contractor", Org->GetOrgNm(PiDb()));}
  }

  // add global project fields
  for (int ProjN=0; ProjN<PiDb->GetProjs(); ProjN++){
    PPiRec Proj=PiDb->GetProj(ProjN);
    TStr ProjRef=Proj->GetProjRef(PiDb());
    Proj->AddFldNmVal(PiDb(), "Partners", PiDb->GetProjOrgsStr(ProjRef));
  }
  // add global organisation fields
  for (int OrgN=0; OrgN<PiDb->GetOrgs(); OrgN++){
    PPiRec Org=PiDb->GetOrg(OrgN);
    TStr OrgNm=Org->GetOrgNm(PiDb());
    Org->AddFldNmVal(PiDb(), "Projects", PiDb->GetOrgProjsStr(OrgNm));
  }
  // create country table
  TStrIntH CountryNmToOrgsH;
  TStrIntH CountryNmToProjsH;
  for (int OrgN=0; OrgN<PiDb->GetOrgs(); OrgN++){
    PPiRec Org=PiDb->GetOrg(OrgN);
    TStr OrgNm=Org->GetOrgNm(PiDb());
    TStr CountryNm=Org->GetOrgCountry(PiDb());
    CountryNmToOrgsH.AddDat(CountryNm)++;
    CountryNmToProjsH.AddDat(CountryNm)+=PiDb->GetOrgProjs(OrgNm);
  }
  // countries
  for (int CountryN=0; CountryN<CountryNmToOrgsH.Len(); CountryN++){
    TStr CountryNm=CountryNmToOrgsH.GetKey(CountryN);
    int Orgs=CountryNmToOrgsH.GetDat(CountryNm);
    int Projs=CountryNmToProjsH.GetDat(CountryNm);
    // create record
    PPiRec Country=TPiRec::New();
    Country->AddFldNmVal(PiDb(), "Country", CountryNm);
    Country->AddFldNmVal(PiDb(), "Country Organisations", TInt::GetStr(Orgs, "%4d org."));
    Country->AddFldNmVal(PiDb(), "Country Projects", TInt::GetStr(Projs, "%4d proj."));
    // add country to database
    PiDb->AddCountry(Country);
  }
  // create pi-graph
  PiDb->PiGraph=TPiGraph::New(PiDb);
  // create bow
  PiDb->BowDocBs=PiDb->GenBowDocBs();

  // return
  return PiDb;
}

