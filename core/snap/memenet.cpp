#include "stdafx.h"
#include "memenet.h"

/////////////////////////////////////////////////
// Meme Url network
int TMemeNet::GetSameDomLinks(const PMemeNetBs& NetBs) const {
  int SameDomLink = 0;
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    const int Src = EI.GetSrcNId();
    const int Dst = EI.GetDstNId();
    if (NetBs->GetDomNm(Src) == NetBs->GetDomNm(Dst)) {
      SameDomLink++;
    }
  }
  return SameDomLink;
}

void TMemeNet::DelSameDomLinks(const PMemeNetBs& NetBs) {
  TIntPrV DelEdgeV;
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    const int Src = EI.GetSrcNId();
    const int Dst = EI.GetDstNId();
    if (NetBs->GetDomNm(Src) == NetBs->GetDomNm(Dst)) {
      DelEdgeV.Add(TIntPr(Src, Dst));
    }
  }
  for (int e = 0; e < DelEdgeV.Len(); e++) {
    DelEdge(DelEdgeV[e].Val1, DelEdgeV[e].Val2); }
}

void TMemeNet::DrawNet(const TStr& OutFNm) const {
  DrawNet(OutFNm, NULL,3);
}

void TMemeNet::DrawNet(const TStr& OutFNm, const PMemeNetBs& MemeNetBs, const int& MinCcSize) const {
  FILE *F = fopen(TStr(OutFNm+".dot").CStr(), "wt");
  fprintf(F, "digraph G {\n");
  fprintf(F, "  graph [splines=false overlap=false, rankdir=\"RL\"]\n");
  fprintf(F, "  node  [width=0.3, height=0.3,color=black, fontcolor=black, style=filled, fontsize=10]\n");  
  TIntSet NodeSet;
  THash<TInt, TIntV> RankV;
  TCnComV CnComV;
  TSnap::GetWccs(PMemeNet((TMemeNet*)this), CnComV);
  /*for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    if (GetNI(EI.GetSrcNId()).GetDeg()==1 && GetNI(EI.GetDstNId()).GetDeg()==1) { continue; }
    NodeSet.AddKey(EI.GetSrcNId()); NodeSet.AddKey(EI.GetDstNId());
    RankV.AddDat(EI.GetSrcNDat().Tm.GetInUnits(tmu10Min)).Add(EI.GetSrcNId());
    RankV.AddDat(EI.GetDstNDat().Tm.GetInUnits(tmu10Min)).Add(EI.GetDstNId());
  }*/
  for (int c=0; c < CnComV.Len(); c++) {
    if (CnComV[c].Len() < MinCcSize) { continue; }
    for (int n = 0; n < CnComV[c].Len(); n++) {
      NodeSet.AddKey(CnComV[c][n]);
      RankV.AddDat(GetNDat(CnComV[c][n]).Tm.GetInUnits(tmu10Min)).Add(CnComV[c][n]);
    }
  }
  RankV.SortByDat(true);
  for (int r = 0; r < RankV.Len(); r++) {
    if (RankV[r].Len() > 1) { 
      fprintf(F, "{ rank=same; ");
      for (int i = 0; i < RankV[r].Len(); i++) { 
        fprintf(F, " %d;", RankV[r][i]); }
      fprintf(F, " }\n");
    }
  }
  for (int n = 0; n < NodeSet.Len(); n++) {
    TChA Url = MemeNetBs.Empty()?"":MemeNetBs->GetUrl(NodeSet[n]);
    if (! Url.Empty()) { Url=TStrUtil::GetDomNm2(Url); }
    fprintf(F, "  %d [label=\"%s\", shape=%s, fillcolor=%s];\n", NodeSet[n], Url.CStr(),
      GetNDat(NodeSet[n]).GetUrlTy()==utMedia?"ellipse":"box", 
      GetNDat(NodeSet[n]).GetUrlTy()==utMedia?"springgreen":"firebrick1");
  }
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    const int Src = EI.GetSrcNId();
    const int Dst = EI.GetDstNId();
    if (! NodeSet.IsKey(Src) || ! NodeSet.IsKey(Dst)) { continue; }
    fprintf(F, "  %d -> %d;\n", Src, Dst); 
    
  }
  fprintf(F, "  label = \"%s\";\n", QtStr.CStr());
  fprintf(F, "}\n");
  fclose(F);
  TGraphViz::DoLayout(OutFNm+".dot", OutFNm+".gif",  gvlDot);
}

/////////////////////////////////////////////////
// Meme Url Network Base
TIntV TMemeNetBs::GetNetByNzNodes() const {
  TIntPrV SzIdV(GetNets(), 0);
  for (int n = 0; n < GetNets(); n++) {
    SzIdV.Add(TIntPr(GetNet(n)->GetNodes()-TSnap::CntDegNodes(GetNet(n), 0), n));
  }
  SzIdV.Sort(false);
  TIntV IdV(GetNets(), 0);
  for (int i = 0; i < SzIdV.Len(); i++) {
    IdV.Add(SzIdV[i].Val2);
  }
  return IdV;
}

void TMemeNetBs::QuoteToDomainSvd() const {
  TSparseRowMatrix Mtx;
  const int TopNQuotes = Mega(1);
  const int TopNDomains = Mega(1);
  const TStr OutFNm = "Memes";
  // get fq domains
  TStrIntH DomCntH;
  TIntV NIdV = GetNetByNzNodes();
  for (int n = 0; n < TMath::Mn(TopNQuotes, NIdV.Len()); n++) {
    for (TMemeNet::TNodeI NI = GetNet(NIdV[n])->BegNI(); NI < GetNet(NIdV[n])->EndNI(); NI++) {
      if (NI().UrlTy != utMedia) { continue; }
      DomCntH.AddDat(GetDomNm(NI.GetId())) += 1; }
  }
  DomCntH.SortByDat(false);
  // build quote to domain matrix
  THashSet<TStr> DomainSet;
  TStrV QtV;
  for (int i = 0; i < TMath::Mn(TopNDomains, DomCntH.Len()); i++) {
    DomainSet.AddKey(DomCntH.GetKey(i)); 
  }
  for (int n = 0; n < TMath::Mn(TopNQuotes, NIdV.Len()); n++) {
    TIntFltH DomCntH;
    PMemeNet Net = GetNet(NIdV[n]);
    QtV.Add(Net->GetStr());
    for (TMemeNet::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
      const int DomId = DomainSet.GetKeyId(GetDomNm(NI.GetId()));
      if (DomId == -1) { continue; }
      DomCntH.AddDat(DomId) += 1;
    }
    Mtx.DocSpVV.Add();
    TIntFltKdV& V = Mtx.DocSpVV.Last();
    for (int d = 0; d < DomCntH.Len(); d++) {
      V.Add(TIntFltKd(DomCntH.GetKey(d), DomCntH[d]));
    }
    V.Sort();
  }
  Mtx.RowN = QtV.Len();
  Mtx.ColN = DomainSet.Len();
  if (Mtx.RowN < Mtx.ColN) { Mtx.Transpose(); }
  printf("%d quotes (rows)\n", QtV.Len());
  printf("%d domains (cols)\n", DomainSet.Len());
  //Mtx.Transpose();
  TFltV SngValV;
  TFltVV LSingV, RSingV; // L: X:226357 Y:10, R: X:16768 Y:10
  TSparseSVD::LanczosSVD(Mtx, 10, 30, ssotFull, SngValV, LSingV, RSingV);
  if (RSingV.GetXDim() != QtV.Len()) { RSingV.Swap(LSingV); }
  printf("L(D): %d %d   R(Q): %d %d\n", LSingV.GetXDim(), LSingV.GetYDim(), RSingV.GetXDim(), RSingV.GetYDim());
  { TFOut FOut(OutFNm+".QtToDomSvd"); QtV.Save(FOut);  DomainSet.Save(FOut);
  SngValV.Save(FOut); LSingV.Save(FOut);  RSingV.Save(FOut); }
  TGnuPlot::PlotValV(SngValV, OutFNm+"-sngVals", TStr::Fmt("%d quotes, %d domains", QtV.Len(), DomainSet.Len()), "rank", "value");
  { FILE *F = fopen(TStr::Fmt("%s-quotes.tab", OutFNm.CStr()).CStr(), "wt");
  fprintf(F, "Qt\tVectors components\n");
  for (int x = 0; x < RSingV.GetXDim(); x++) {
    fprintf(F, "%s", QtV[x].CStr());
    for (int y = 0; y < RSingV.GetYDim(); y++) {
      fprintf(F, "\t%f", RSingV.At(x,y)); }
    fprintf(F, "\n");
  }
  fclose(F); }
  { FILE *F = fopen(TStr::Fmt("%s-domains.tab", OutFNm.CStr()).CStr(), "wt");
  fprintf(F, "Domain\tVectors components\n");
  for (int x = 0; x < LSingV.GetXDim(); x++) {
    fprintf(F, "%s", DomainSet[x].CStr());
    for (int y = 0; y < LSingV.GetYDim(); y++) {
      fprintf(F, "\t%f", LSingV.At(x,y)); }
    fprintf(F, "\n");
  }
  fclose(F); }
}

void TMemeNetBs::LinkDomainSvd() const {
  TIntV NIdV = GetNetByNzNodes();
  TStrSet GoodDomainSet;
  const int TopNQuotes =  Mega(1);
  const int TopNDomains = Mega(1);
  const TStr OutFNm = "Memes-1k";
  // get freq domains
  TStrIntH DomCntH;
  { int InDomL=0, BetDomL=0;
  for (int n = 0; n < TMath::Mn(TopNQuotes, NIdV.Len()); n++) {
    PMemeNet Net = GetNet(NIdV[n]);
    for (TMemeNet::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++) {
      const TChA S=GetDomNm(EI.GetSrcNId());
      const TChA D=GetDomNm(EI.GetDstNId());
      if (S != D) { // skip links inside same domain
        DomCntH.AddDat(S) += 1;  DomCntH.AddDat(D) += 1; BetDomL++;
      } else { InDomL++; }
    }
  }
  printf("%d %d\n", InDomL, BetDomL);
  DomCntH.SortByDat(false);
  for (int i = 0; i < TMath::Mn(DomCntH.Len(), TopNDomains); i++) {
    GoodDomainSet.AddKey(DomCntH.GetKey(i)); 
  } }
  // create matrix
  TStrSet DomainSet;
  THash<TInt, TIntH> DomOutDH;
  int AllE=0, GoodE=0;
  for (int n = 0; n < TMath::Mn(TopNQuotes, NIdV.Len()); n++) {
    PMemeNet Net = GetNet(NIdV[n]);
    for (TMemeNet::TEdgeI EI = Net->BegEI(); EI < Net->EndEI(); EI++, AllE++) {
      int SrcDom = GoodDomainSet.GetKeyId(GetDomNm(EI.GetSrcNId()));
      int DstDom = GoodDomainSet.GetKeyId(GetDomNm(EI.GetDstNId()));
      if (SrcDom==-1 || DstDom==-1 || SrcDom==DstDom) { continue; } GoodE++;
      SrcDom = DomainSet.AddKey(GoodDomainSet[SrcDom]);
      DstDom = DomainSet.AddKey(GoodDomainSet[DstDom]);
      DomOutDH.AddDat(SrcDom).AddDat(DstDom) += 1;
    }
  }
  printf("%d edges, %d inside top %d domains\n", AllE, GoodE, DomainSet.Len());
  printf("%d %d %d %d\n", DomCntH.Len(), GoodDomainSet.Len(), DomainSet.Len(), DomOutDH.Len());
  TSparseRowMatrix Mtx;
  Mtx.ColN = DomainSet.Len();
  Mtx.RowN = DomainSet.Len();
  Mtx.DocSpVV.Gen(DomainSet.Len());
  DomOutDH.SortByKey();
  for (int d = 0; d < DomOutDH.Len(); d++) {
    TIntFltKdV& V = Mtx.DocSpVV[DomOutDH.GetKey(d)];
    V.Gen(DomOutDH[d].Len(), 0);
    for (int i = 0; i < DomOutDH[d].Len(); i++) {
      V.Add(TIntFltKd(DomOutDH[d].GetKey(i), DomOutDH[d][i].Val)); }
    V.Sort();
  }
  // SVD
  TFltV SngValV;
  TFltVV LSingV, RSingV;
  TSparseSVD::LanczosSVD(Mtx, 10, 50, ssotFull, SngValV, LSingV, RSingV);
  printf("%d %d\n", RSingV.GetXDim(), RSingV.GetYDim());
  printf("%d %d\n", LSingV.GetXDim(), LSingV.GetYDim());
  
  TGnuPlot::PlotValV(SngValV, OutFNm+"-DomSngVals", TStr::Fmt("%d domains. Domain link network.", DomainSet.Len()), "rank", "value");
  { FILE *F = fopen(TStr::Fmt("%s-DomRight.tab", OutFNm.CStr()).CStr(), "wt");
  fprintf(F, "Domain\tWgtDeg\tOutDeg\tRight Vectors\n");
  IAssert(RSingV.GetXDim() == DomainSet.Len());
  for (int x = 0; x < RSingV.GetXDim(); x++) {
    fprintf(F, "%s\t%d\t%d", DomainSet[x].CStr(), DomCntH.GetDat(DomainSet[x]), Mtx.DocSpVV[x].Len());
    for (int y = 0; y < RSingV.GetYDim(); y++) {
      fprintf(F, "\t%f", RSingV.At(x,y)); }
    fprintf(F, "\n");
  }
  fclose(F); }
  { FILE *F = fopen(TStr::Fmt("%s-DomLeft.tab", OutFNm.CStr()).CStr(), "wt");
  fprintf(F, "Domain\tWgtDeg\tOutDeg\tLeft Vectors\n");
  IAssert(LSingV.GetXDim() == DomainSet.Len());
  for (int x = 0; x < LSingV.GetXDim(); x++) {
    fprintf(F, "%s\t%d\t%d", DomainSet[x].CStr(), DomCntH.GetDat(DomainSet[x]), Mtx.DocSpVV[x].Len());
    for (int y = 0; y < LSingV.GetYDim(); y++) {
      fprintf(F, "\t%f", LSingV.At(x,y)); }
    fprintf(F, "\n");
  }
  fclose(F); }
}

void TMemeNetBs::SaveNetStat(const TStr& OutFNm) const {
  FILE *F = fopen(TStr::Fmt("netStat-%s.tab", OutFNm.CStr()).CStr(), "wt");
  fprintf(F, "Rank\tRootQuote\tNodes\tEdges\tNonZeroDegNodes");
  fprintf(F, "\tBetDomEdges\tWccNodes\tWccEdges\tDiam");
  fprintf(F, "\tMxDeg\tMxInDeg\tMxOutDeg\tNDeg0\tNDeg1\tClosedTr\tOpenTr\n");
  for (int q = 0; q < QtIdNetH.Len(); q++) {
    PMemeNet Net = GetNet(q);
    PMemeNet Wcc = TSnap::GetMxWcc(Net);
    fprintf(F, "%d\t%s\t%d\t%d\t%d", q+1, Net->GetStr().CStr(), Net->GetNodes(), Net->GetEdges(), Net->GetNodes()-TSnap::CntDegNodes(Net, 0));
    fprintf(F, "\t%d\t%d\t%d", Net->GetSameDomLinks(PMemeNetBs((TMemeNetBs*)this)), Wcc->GetNodes(), Wcc->GetEdges());
    Net->DelSameDomLinks(PMemeNetBs((TMemeNetBs*)this));
    //TSnap::DelZeroDegNodes(Net);
    fprintf(F, "\t%d", TSnap::GetBfsFullDiam(Net, 100));
    // degrees
    int MxDeg=0, MxInDeg=0, MxOutDeg=0, NDeg0=0, NDeg1=0, Triads=0;
    for (TMemeNet::TNodeI NI = Net->BegNI(); NI < Net->EndNI(); NI++) {
      const int Deg = NI.GetDeg();
      if (Deg > MxDeg) { MxDeg = Deg; }
      if (NI.GetInDeg() > MxInDeg) { MxInDeg = NI.GetInDeg(); }
      if (NI.GetOutDeg() > MxOutDeg) { MxOutDeg = NI.GetOutDeg(); }
      if (Deg == 0) { NDeg0++; }
      if (Deg == 1) { NDeg1++; }
    }
    int ClosedTriads, OpenTriads;
    TSnap::GetTriads(Net, ClosedTriads, OpenTriads);
    fprintf(F, "\t%d\t%d\t%d\t%d\t%d\t%d\t%d", MxDeg, MxInDeg, MxOutDeg, NDeg0, NDeg1, ClosedTriads, OpenTriads);
    fprintf(F, "\n");
  }
  fclose(F);
}

PMemeNetBs TMemeNetBs::Build(const PQuoteBs& QtBs, const TStr& MemesFNmWc, const int& TakeTopN) {
  PMemeNetBs NetBs = TMemeNetBs::New();
  THash<TChA, TInt> QtCIdH;
  // fill networks
  TIntV CIdV;  QtBs->GetCIdVByFq(CIdV, 100, "", utUndef, false);
  //QtBs->GetCIdVByFq(QtIdV, 100, "", utUndef, false); // only clusters
  printf("%d clusters\n", CIdV.Len());
  int urlc=0, qtc=0;
  for (int c = 0; c < TMath::Mn(CIdV.Len(), TakeTopN); c++) {
    const TIntV& QtIdV = QtBs->GetClust(CIdV[c]);
    const TChA QtCentr = QtBs->GetQt(QtBs->GetCentrQtId(CIdV[c])).GetStr();
    if (NetBs->IsUrl(QtCentr.CStr()) && NetBs->GetUrlTy(QtCentr.CStr())!=utUndef) { 
      printf("'%s' is URL\n", QtCentr.CStr()); continue; }
    const int NewCentrId = NetBs->AddUrl(QtCentr.CStr(), utUndef); // quote string
    PMemeNet& Net = NetBs->QtIdNetH.AddDat(NewCentrId);
    IAssert(! QtCentr.Empty());
    Net = TMemeNet::New(QtCentr);
    for (int q = 0; q < QtIdV.Len(); q++, qtc++) {
      const TQuote& Qt = QtBs->GetQt(QtIdV[q]);
      if (NetBs->IsUrl(Qt.GetStr().CStr())  && NetBs->GetUrlTy(Qt.GetStr().CStr())!=utUndef) { 
        printf("'%s' is URL2\n", QtCentr.CStr()); continue; }
      const int NewQtId = NetBs->AddUrl(Qt.GetStr().CStr(), utUndef); // quote string
      QtCIdH.AddDat(Qt.GetStr(), NewCentrId);
      for (int u = 0; u < Qt.GetUrls(); u++, urlc++) {
        const int urlid = NetBs->AddUrl(QtBs->GetStr(Qt.GetUrlId(u)), QtBs->GetUrlTy(Qt.GetUrlId(u)));
        Net->AddNode(urlid);
        TMemeNetDat& Dat = Net->GetNDat(urlid);
        Dat.UrlId = urlid;
        Dat.UrlTy = QtBs->GetUrlTy(Qt.GetUrlId(u));
        Dat.Tm = Qt.GetTm(u);
        Dat.QtIdV.Add(NewQtId);
      }
    }
  }
  printf("  %d quotes total\n  %d urls total\n", qtc, urlc);
  printf("  %d netbs unique posts urls\n", NetBs->GetUrls());
  printf("  %d quotes to netbs networks\n", QtCIdH.Len());
  printf("  %d quote networks\n\n", NetBs->QtIdNetH.Len());
  // load links
  TMemesDataLoader MDL(MemesFNmWc);
  int postc=0, qtpostc=0, linkc=0, oklinkc=0;
  while (MDL.LoadNext()) { postc++;
    if (! NetBs->IsUrl(MDL.PostUrlStr.CStr())) { continue; }  qtpostc++;
    const  int SrcUrlId = NetBs->GetUrlId(MDL.PostUrlStr.CStr());
    for (int q = 0; q < MDL.MemeV.Len(); q++) {
      if (! QtCIdH.IsKey(MDL.MemeV[q])) { continue; }
      PMemeNet Net =  NetBs->QtIdNetH.GetDat(QtCIdH.GetDat(MDL.MemeV[q]));
      for (int l = 0; l < MDL.LinkV.Len(); l++, linkc++) {
        if (! NetBs->IsUrl(MDL.LinkV[l].CStr())) { continue; } 
        const int DstUrlId = NetBs->GetUrlId(MDL.LinkV[l].CStr());
        if (SrcUrlId!=DstUrlId && Net->IsNode(DstUrlId)) { 
          Net->AddEdge(SrcUrlId, DstUrlId); 
          oklinkc++;
        }
      }
    }
    if (postc % Mega(1) == 0) {
      printf("%d posts, %d posts in base, %d links, %d ok links\n", postc, qtpostc, linkc, oklinkc);
    }
  }
  printf("done.\n");
  printf("%d posts\n%d posts in netbs\n%d total out-links of netbs posts\n%d links between netbs posts\n", 
    postc, qtpostc, linkc, oklinkc);
  return NetBs;
}
