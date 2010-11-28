/////////////////////////////////////////////////
// Includes
#include "sgraph.h"

/////////////////////////////////////////////////
// Simple-Graph
void TSGraph::AddEdgeOrIncWgt(const int& SrcVrtxN, const int& DstVrtxN,
 const bool& DirP, const double Wgt){
  bool FoundP=false; int Edges=GetEdges(SrcVrtxN);
  for (int EdgeN=0; EdgeN<Edges; EdgeN++){
    int SubVrtxN=GetEdgeDstVrtxN(SrcVrtxN, EdgeN);
    if (SubVrtxN==DstVrtxN){
      FoundP=true;
      IncEdgeWgt(SrcVrtxN, EdgeN, Wgt);
      if (DirP){
        AddEdgeOrIncWgt(DstVrtxN, SrcVrtxN, false, Wgt); break;}
    }
  }
  if (!FoundP){
    AddEdge(SrcVrtxN, DstVrtxN, DirP, Wgt);}
}

void TSGraph::_GetMnMxXY(
 double& MnX, double& MnY, double& MxX, double& MxY){
  // determine min. & max. coordinates
  MnX=0; MxX=0; MnY=0; MxY=0;
  bool First=true;
  for (int VrtxN=0; VrtxN<GetVrtxs(); VrtxN++){
    double X; double Y; GetVrtxXY(VrtxN, X, Y);
    if (First){
      MnX=X; MxX=X; MnY=Y; MxY=Y;
      First=false;
    } else {
      if (X<MnX){MnX=X;}
      if (X>MxX){MxX=X;}
      if (Y<MnY){MnY=Y;}
      if (Y>MxY){MxY=Y;}
    }
  }
}

void TSGraph::_AddBorders(const double& BorderFac,
 double& MnX, double& MnY, double& MxX, double& MxY){
  // add borders
  if (MxX-MnX==0){MnX-=BorderFac; MxX+=BorderFac;}
  else {MnX-=(MxX-MnX)*BorderFac; MxX+=(MxX-MnX)*BorderFac;}
  if (MxY-MnY==0){MnY-=BorderFac; MxY+=BorderFac;}
  else {MnY-=(MxY-MnY)*BorderFac; MxY+=(MxY-MnY)*BorderFac;}
}

void TSGraph::RescaleXY(const double& BorderFac){
  // determine min. & max. coordinates
  double MnX; double MnY; double MxX; double MxY;
  _GetMnMxXY(MnX, MnY, MxX, MxY);
  // add borders
  _AddBorders(BorderFac, MnX, MnY, MxX, MxY);
  // rescale coordinates
  for (int VrtxN=0; VrtxN<GetVrtxs(); VrtxN++){
    double X; double Y; GetVrtxXY(VrtxN, X, Y);
    PutVrtxXY(VrtxN, (X-MnX)/(MxX-MnX), (Y-MnY)/(MxY-MnY));
  }
}

void TSGraph::RescaleXY(const double& MnX, const double& MnY,
 const double& MxX, const double& MxY){
  if ((fabs(MxX-MnX)<0.01)||(fabs(MxY-MnY)<0.01)){return;}
  // rescale coordinates
  for (int VrtxN=0; VrtxN<GetVrtxs(); VrtxN++){
    double X; double Y; GetVrtxXY(VrtxN, X, Y);
    PutVrtxXY(VrtxN, (X-MnX)/(MxX-MnX), (Y-MnY)/(MxY-MnY));
  }
}

void TSGraph::SaveTxt(const TStr& FNm) const {
  printf("Saving Text to %s ...", FNm.CStr());
  TFOut FOut(FNm); FILE* fOut=FOut.GetFileId();
  for (int VrtxN=0; VrtxN<GetVrtxs(); VrtxN++){
    fprintf(fOut, "[%d] '%s'\n", VrtxN, GetVrtxNm(VrtxN).CStr());
    int Edges=GetEdges(VrtxN);
    for (int EdgeN=0; EdgeN<Edges; EdgeN++){
      int DstVrtxN=GetEdgeDstVrtxN(VrtxN, EdgeN);
      double EdgeWgt=GetEdgeWgt(VrtxN, EdgeN);
      fprintf(fOut, "     <%d>:%g '%s'\n",
       DstVrtxN, EdgeWgt, GetVrtxNm(DstVrtxN).CStr());
    }
  }
  printf(" Done.\n");
}

void TSGraph::SavePajek(const TStr& FNm) const {
    // get most connected vrtx
    int MxVrtxN = 0; int MxDegree = 0;
    for (int VrtxN = 0; VrtxN < GetVrtxs(); VrtxN++) {
        if (GetEdges(VrtxN) > MxDegree) {
            MxVrtxN = VrtxN;
            MxDegree = GetEdges(VrtxN);
        }
    }
    printf("Edges: %d\n", MxDegree);
    // find first level neighbors
    const int MxVrtxEdges = GetEdges(MxVrtxN); TIntV CloseVrtxV;
    for (int VrtxEdgeN = 0; VrtxEdgeN < MxVrtxEdges; VrtxEdgeN++) {
        CloseVrtxV.Add(GetEdgeDstVrtxN(MxVrtxN, VrtxEdgeN));
    }
    CloseVrtxV.Sort();
    // find second level neighbors
    for (int VrtxEdgeN = 0; VrtxEdgeN < MxVrtxEdges; VrtxEdgeN++) {
        const int VrxtN = CloseVrtxV[VrtxEdgeN];
        for (int SecVrtxEdgeN = 0; SecVrtxEdgeN < GetEdges(VrxtN); SecVrtxEdgeN++) {
            const int SecDstVrtxN = GetEdgeDstVrtxN(VrxtN, SecVrtxEdgeN);
            if (!CloseVrtxV.IsInBin(SecDstVrtxN)) { CloseVrtxV.AddSorted(SecDstVrtxN); }
        }
    }
    printf("full size: %d\n", CloseVrtxV.Len());
    // save vertices
    PSOut SOut=TFOut::New(FNm);
    SOut->PutStr("*vertices ");
    SOut->PutStr(TInt::GetStr(CloseVrtxV.Len())); SOut->PutLn();
    for (int VrtxN = 0; VrtxN < CloseVrtxV.Len(); VrtxN++) {
        SOut->PutStr(TInt::GetStr(VrtxN+1));
        SOut->PutStr(TStr(" \"") + TSGraph::GetVrtxNm(VrtxN) + "\"");
        SOut->PutLn();
    }
    // save edges
    SOut->PutStr("*arcs"); SOut->PutLn();
    for (int VrtxN = 0; VrtxN < CloseVrtxV.Len(); VrtxN++) {
        const int VrtxId = CloseVrtxV[VrtxN];
        const int VrtxEdges = GetEdges(VrtxId);
        for (int VrtxEdgeN = 0; VrtxEdgeN < VrtxEdges; VrtxEdgeN++) {
            const int DstVrtxId = GetEdgeDstVrtxN(VrtxId, VrtxEdgeN);
            const int DstVrtxN = CloseVrtxV.SearchBin(DstVrtxId);
            if (DstVrtxN != -1) {
                SOut->PutStr(TInt::GetStr(VrtxN+1)+" "+TInt::GetStr(DstVrtxN+1));
                SOut->PutLn();
            }
        }
    }
}

PBowDocBs TSGraph::GetNhoodV(const int& MxVrtxLev, TIntH& DIdVrtxNH,
 const TStrStrH& BowDocNmToCatNmH, const int& MnEAddrDegree) const {
  //printf("Create neighbourhood vector...\n");
  int Vrtxs=GetVrtxs();
  // create bow-doc-bs and its vocabulary
  //printf("...create vocabulary\n");
  PBowDocBs BowDocBs=TBowDocBs::New();
  for (int VrtxN=0; VrtxN<Vrtxs; VrtxN++){
    //printf("%d/%d (%.1f%%)\r", 1+VrtxN, Vrtxs, 100.0*(1+VrtxN)/Vrtxs);
    TStr VrtxNm=TInt::GetStr(VrtxN)+": "+GetVrtxNm(VrtxN);
    int WId=BowDocBs->AddWordStr(VrtxNm);
    IAssert(VrtxN==WId);
  }
  //printf("\n");
  // for each vertex create sparse-vectors
  //printf("...create sparse-vectors\n");
  int Nhoods=0; int AvgNHoodLen=0;
  for (int RootVrtxN=0; RootVrtxN<Vrtxs; RootVrtxN++){
    //printf("%d/%d (%.1f%%)\r", 1+RootVrtxN, Vrtxs, 100.0*(1+RootVrtxN)/Vrtxs);
    // check if allowed
    TStr VrtxNm=GetVrtxNm(RootVrtxN);
    if ((!BowDocNmToCatNmH.Empty())&&(!BowDocNmToCatNmH.IsKey(VrtxNm))){continue;}
    // hash table for collecting weights
    TIntFltH VrtxNToWgtH;
    // create traversal queue
    TIntPrQ OpenVrtxNLevPrQ; OpenVrtxNLevPrQ.Push(TIntPr(RootVrtxN, 0));
    while (!OpenVrtxNLevPrQ.Empty()){
      // get vertex and level from queue
      int VrtxN=OpenVrtxNLevPrQ.Top().Val1;
      int VrtxLev=OpenVrtxNLevPrQ.Top().Val2;
      OpenVrtxNLevPrQ.Pop();
      if (VrtxNToWgtH.IsKey(VrtxN)){continue;}
      // get weight
      double VrtxWgt=0;
      switch (VrtxLev){
        case 0: VrtxWgt=1; break;
        case 1: VrtxWgt=0.5; break;
        case 2: VrtxWgt=0.25; break;
        case 3: VrtxWgt=0.125; break;
        case 4: VrtxWgt=0.0625; break;
        default: VrtxWgt=0.03125; break;
      }
      // assign weight to vertex
      VrtxNToWgtH.AddDat(VrtxN, VrtxWgt);
      // create new entries
      if (VrtxLev<MxVrtxLev){
        int Edges=GetEdges(VrtxN);
        for (int EdgeN=0; EdgeN<Edges; EdgeN++){
          int DstVrtxN=GetEdgeDstVrtxN(VrtxN, EdgeN);
          if (!VrtxNToWgtH.IsKey(DstVrtxN)){
            OpenVrtxNLevPrQ.Push(TIntPr(DstVrtxN, VrtxLev+1));
          }
        }
      }
    }
    // get weighted sparse vector
    TIntFltPrV VrtxNWgtPrV; VrtxNToWgtH.GetKeyDatPrV(VrtxNWgtPrV);
    if (VrtxNWgtPrV.Len()>=MnEAddrDegree){
      // add sparse vector as document to bow-document-base
      //TStr DocNm=TInt::GetStr(RootVrtxN);
      TStr DocNm=/*TInt::GetStr(RootVrtxN)+": "+*/GetVrtxNm(RootVrtxN);
      TStrV CatNmV;
      if (BowDocNmToCatNmH.IsKey(DocNm)) {
        TStr CatNm=BowDocNmToCatNmH.GetDat(DocNm);
        if (!CatNm.Empty()){CatNmV.Add(CatNm);}
      }
      int DId=BowDocBs->AddDoc(DocNm, CatNmV, VrtxNWgtPrV);
      DIdVrtxNH.AddDat(DId, RootVrtxN);
      //IAssert(RootVrtxN==DId);
      // neighbourhood statistics
      Nhoods++; AvgNHoodLen+=VrtxNWgtPrV.Len();
      if ((Nhoods>0)&&(Nhoods%100==0)){
        //printf("%6d (%.1f)\r", Nhoods, AvgNHoodLen/double(Nhoods));
      }
    }
  }
  //printf("\n");
  BowDocBs->AssertOk();
  //printf("Done.\n");
  // return
  return BowDocBs;
}

PSGraph TSGraph::GetSGraphFromAmazon(const TStr& FNm, const int& Recs){
  // create graph
  PSGraph SGraph=TSGraph::New();
  // create hash table for connecting ASIN to internal id
  TStrIntH AsinStrToVrtxNH;
  // first pass - collect vertices
  {printf("First Pass - %s ...\n", FNm.CStr());
  PSIn XmlSIn=TFIn::New(FNm); // open file
  PXmlDoc XmlDoc;
  do {
    if (SGraph->GetVrtxs()%100==0){printf("%6d\r", SGraph->GetVrtxs());}
    if ((Recs!=-1)&&(SGraph->GetVrtxs()>=Recs)){break;}
    XmlDoc=TXmlDoc::LoadTxt(XmlSIn);
    if (XmlDoc->IsOk()){
      TStr AsinStr=XmlDoc->GetTagTokStr("AmazonItem|ItemId");
      TChA VrtxNmChA=XmlDoc->GetTagTokStr("AmazonItem|Title");
      TXmlTokV AuthXmlTokV;
      XmlDoc->GetTagTokV("AmazonItem|Authors|Name", AuthXmlTokV);
      for (int XmlTokN=0; XmlTokN<AuthXmlTokV.Len(); XmlTokN++){
        TStr AuthNm=AuthXmlTokV[XmlTokN]->GetTagVal("Name", false);
        VrtxNmChA+="; "; VrtxNmChA+=AuthNm;
      }
      int VrtxN=SGraph->AddVrtx(VrtxNmChA);
      AsinStrToVrtxNH.AddDat(AsinStr, VrtxN);
    }
  } while (XmlDoc->IsOk());
  printf("\nDone.\n");}
  // second pass - collect edges
  {printf("Second Pass - %s ...\n", FNm.CStr());
  PSIn XmlSIn=TFIn::New(FNm); // open file
  PXmlDoc XmlDoc; int XmlDocs=0;
  do {
    XmlDocs++; if (XmlDocs%100==0){printf("%6d\r", XmlDocs);}
    if ((Recs!=-1)&&(XmlDocs>=Recs)){break;}
    XmlDoc=TXmlDoc::LoadTxt(XmlSIn);
    if (XmlDoc->IsOk()){
      TStr SrcAsinStr=XmlDoc->GetTagTokStr("AmazonItem|ItemId");
      TXmlTokV ItemIdXmlTokV;
      XmlDoc->GetTagTokV("AmazonItem|XSell|ItemId", ItemIdXmlTokV);
      int SrcVrtxN=AsinStrToVrtxNH.GetDat(SrcAsinStr);
      SGraph->ReserveVrtxEdges(SrcVrtxN, ItemIdXmlTokV.Len());
      for (int XmlTokN=0; XmlTokN<ItemIdXmlTokV.Len(); XmlTokN++){
        TStr DstAsinStr=ItemIdXmlTokV[XmlTokN]->GetTagVal("ItemId", false);
        if (AsinStrToVrtxNH.IsKey(DstAsinStr)){
          int DstVrtxN=AsinStrToVrtxNH.GetDat(DstAsinStr);
          SGraph->AddEdge(SrcVrtxN, DstVrtxN, false);
        }
      }
    }
  } while (XmlDoc->IsOk());
  printf("\nDone.\n");}
  // return graph
  return SGraph;
}

PSGraph TSGraph::GetSGraphFromFP5IST(const TStr& FNm){
  // load spread-sheet
  printf("Load %s ...", FNm.CStr());
  PSs Ss=TSs::LoadTxt(ssfTabSep, FNm);
  printf("Done.\n");
  // initialize column numbers
  //int ProjRefFldN=Ss->GetFldX("Project Ref");
  int ProjNmFldN=Ss->GetFldX("Acronym");
  //int DomainFldN=Ss->GetFldX("Domain / Key Action");
  int UnitFldN=Ss->GetFldX("Unit");
  //int OfficerFldN=Ss->GetFldX("PO");
  int OrgNmFldN=Ss->GetFldX("Legal Name");
  int OrgCountryFldN=Ss->GetFldX("Legal Country");
  //int OrgTypeFldN=Ss->GetFldX("Type of organisation");
  //int OrgRoleFldN=Ss->GetFldX("Participant role");

  // create graph
  PSGraph SGraph=TSGraph::New();
  // create project organizations
  TStrIntH OrgNmToVrtxNH;
  TStrStrIntKdVH ProjNmToOrgNmFqKdVH;
  // traverse project-participation records
  int PPRecs=Ss->GetYLen();
  for (int PPRecN=1; PPRecN<PPRecs; PPRecN++){
    if (PPRecN%100==0){printf("%d/%d\r", PPRecN, PPRecs);}
    // get fields values
    TStr ProjNm=Ss->GetVal(ProjNmFldN, PPRecN).GetTrunc();
    TStr OrgNm=Ss->GetVal(OrgNmFldN, PPRecN);
    TStr CountryNm=Ss->GetVal(OrgCountryFldN, PPRecN);
    TStr UnitNm=Ss->GetVal(UnitFldN, PPRecN);
    //if (UnitNm!="C2"){continue;}
    OrgNm=OrgNm+", "+CountryNm;
    // create vertex
    int VrtxN=-1;
    if (OrgNmToVrtxNH.IsKey(OrgNm)){
      VrtxN=OrgNmToVrtxNH.GetDat(OrgNm);
    } else {
      VrtxN=SGraph->AddVrtx(OrgNm);
      OrgNmToVrtxNH.AddDat(OrgNm, VrtxN);
    }
    // create edges
    int SubPPRecN=PPRecN-1;
    while (SubPPRecN>=1){
      TStr SubProjNm=Ss->GetVal(ProjNmFldN, SubPPRecN).GetTrunc();
      TStr SubOrgNm=Ss->GetVal(OrgNmFldN, SubPPRecN);
      TStr SubCountryNm=Ss->GetVal(OrgCountryFldN, SubPPRecN);
      TStr SubUnitNm=Ss->GetVal(UnitFldN, SubPPRecN);
      SubOrgNm=SubOrgNm+", "+SubCountryNm;
      if (ProjNm!=SubProjNm){break;}
      if (UnitNm!=SubUnitNm){break;}
      int SubVrtxN=OrgNmToVrtxNH.GetDat(SubOrgNm);
      SGraph->AddEdgeOrIncWgt(VrtxN, SubVrtxN, true);
      SubPPRecN--;
    }
  }
  printf("\nDone. (%d Vertices)\n", SGraph->GetVrtxs());
  // return graph
  return SGraph;
}

PSGraph TSGraph::GetSGraphFromTest(){
  // create graph
  PSGraph SGraph=TSGraph::New();
  // create vertices
  int V0=SGraph->AddVrtx("0"); int V4=SGraph->AddVrtx("4"); int V8=SGraph->AddVrtx("8");
  int V1=SGraph->AddVrtx("1"); int V5=SGraph->AddVrtx("5"); int V9=SGraph->AddVrtx("9");
  int V2=SGraph->AddVrtx("2"); int V6=SGraph->AddVrtx("6"); int V10=SGraph->AddVrtx("10");
  int V3=SGraph->AddVrtx("3"); int V7=SGraph->AddVrtx("7"); int V11=SGraph->AddVrtx("11");
  // create edges
  SGraph->AddEdge(V0, V1, false);
  SGraph->AddEdge(V0, V6, false);
  SGraph->AddEdge(V0, V8, false);
  SGraph->AddEdge(V1, V7, false);
  SGraph->AddEdge(V1, V9, false);
  SGraph->AddEdge(V2, V4, false);
  SGraph->AddEdge(V3, V4, false);
  SGraph->AddEdge(V4, V8, false);
  SGraph->AddEdge(V5, V8, false);
  SGraph->AddEdge(V8, V11, false);
  SGraph->AddEdge(V9, V10, false);
  // return graph
  return SGraph;
}


