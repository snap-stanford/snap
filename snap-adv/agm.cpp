#include "stdafx.h"
#include "Snap.h"
#include "agm.h"
#include "agmfit.h"

/////////////////////////////////////////////////
// AGM graph generation.

///Connect members of a given community by Erdos-Renyi
void TAGM::RndConnectInsideCommunity(PUNGraph& Graph, const TIntV& CmtyV, const double& Prob, TRnd& Rnd){
  int CNodes = CmtyV.Len(), CEdges;
  if (CNodes < 20) {
    CEdges = (int) Rnd.GetBinomialDev(Prob, CNodes * (CNodes-1) / 2);
  } else {
    CEdges = (int) (Prob * CNodes * (CNodes - 1) / 2);
  }
  THashSet<TIntPr> NewEdgeSet(CEdges);
  for (int edge = 0; edge < CEdges; ) {
    int SrcNId = CmtyV[Rnd.GetUniDevInt(CNodes)];
    int DstNId = CmtyV[Rnd.GetUniDevInt(CNodes)];
    if (SrcNId > DstNId) { Swap(SrcNId,DstNId); }
    if (SrcNId != DstNId && ! NewEdgeSet.IsKey(TIntPr(SrcNId, DstNId))) { // is new edge
      NewEdgeSet.AddKey(TIntPr(SrcNId, DstNId));
      Graph->AddEdge(SrcNId, DstNId);
      edge++; 
    } 
  }
}


PUNGraph TAGM::GenAGM(TVec<TIntV>& CmtyVV, const double& DensityCoef, const int TargetEdges, TRnd& Rnd){
  PUNGraph TryG = TAGM::GenAGM(CmtyVV, DensityCoef, 1.0, Rnd);
  const double ScaleCoef = (double) TargetEdges / (double) TryG->GetEdges();
  return TAGM::GenAGM(CmtyVV, DensityCoef, ScaleCoef, Rnd);
}

PUNGraph TAGM::GenAGM(TVec<TIntV>& CmtyVV, const double& DensityCoef, const double& ScaleCoef, TRnd& Rnd){
  TFltV CProbV;
  double Prob;
  for (int i = 0; i < CmtyVV.Len(); i++) {
    Prob = ScaleCoef*pow( double( CmtyVV[i].Len()), - DensityCoef);
    if (Prob > 1.0) { Prob = 1; }
    CProbV.Add(Prob);
  }
  return TAGM::GenAGM(CmtyVV, CProbV, Rnd);
}

///Generate graph using the AGM model. CProbV = vector of Pc
PUNGraph TAGM::GenAGM(TVec<TIntV>& CmtyVV, const TFltV& CProbV, TRnd& Rnd, const double PNoCom){
  PUNGraph G = TUNGraph::New(100 * CmtyVV.Len(), -1);
  printf("AGM begins\n");
  for (int i = 0; i < CmtyVV.Len(); i++) {
    TIntV& CmtyV = CmtyVV[i];
    for (int u = 0; u < CmtyV.Len(); u++) {
      if ( G->IsNode(CmtyV[u])) { continue; }
      G->AddNode(CmtyV[u]);
    }
    double Prob = CProbV[i];
    RndConnectInsideCommunity(G, CmtyV, Prob, Rnd);
  }
  if (PNoCom > 0.0) { //if we want to connect nodes that do not share any community
    TIntSet NIDS;
    for (int c = 0; c < CmtyVV.Len(); c++) {
      for (int u = 0; u < CmtyVV[c].Len(); u++) {
        NIDS.AddKey(CmtyVV[c][u]);
      }
    }
    TIntV NIDV;
    NIDS.GetKeyV(NIDV);
    RndConnectInsideCommunity(G,NIDV,PNoCom,Rnd);
  }
  printf("AGM completed (%d nodes %d edges)\n",G->GetNodes(),G->GetEdges());
  G->Defrag();
  return G;
}

////////////////////////////////////////////////////////////////////////////////////
/// AGMUtil:: Utilities for AGM

///Generate sequence from Power law
void TAGMUtil::GenPLSeq(TIntV& SzSeq, const int& SeqLen, const double& Alpha, TRnd& Rnd, const int& Min, const int& Max) {
  SzSeq.Gen(SeqLen, 0);
  while (SzSeq.Len() < SeqLen) {
    int Sz = (int) TMath::Round(Rnd.GetPowerDev(Alpha));
    if (Sz >= Min && Sz <= Max) {
      SzSeq.Add(Sz);
    }
  }
}

///Generate bipartite community affiliation from given power law coefficients for membership distribution and community size distribution.
void TAGMUtil::GenCmtyVVFromPL(TVec<TIntV>& CmtyVV, const int& Nodes, const int& Coms, const double& ComSzAlpha, const double& MemAlpha, const int& MinSz, const int& MaxSz, const int& MinK, const int& MaxK, TRnd& Rnd){
  TIntV NIDV(Nodes, 0);
  for (int i = 0; i < Nodes; i++) {
    NIDV.Add(i);
  }
  GenCmtyVVFromPL(CmtyVV, NIDV, Nodes, Coms, ComSzAlpha, MemAlpha, MinSz, MaxSz, MinK, MaxK, Rnd);
}

///Generate bipartite community affiliation from given power law coefficients for membership distribution and community size distribution.
void TAGMUtil::GenCmtyVVFromPL(TVec<TIntV>& CmtyVV, const PUNGraph& Graph, const int& Nodes, const int& Coms, const double& ComSzAlpha, const double& MemAlpha, const int& MinSz, const int& MaxSz, const int& MinK, const int& MaxK, TRnd& Rnd){
  if (Coms == 0 || Nodes == 0) {
    CmtyVV.Clr();
    return;
  }
  TIntV NIDV;
  Graph->GetNIdV(NIDV);
  GenCmtyVVFromPL(CmtyVV, NIDV, Nodes, Coms, ComSzAlpha, MemAlpha, MinSz, MaxSz, MinK, MaxK, Rnd);
}

///Generate bipartite community affiliation from given power law coefficients for membership distribution and community size distribution.
void TAGMUtil::GenCmtyVVFromPL(TVec<TIntV>& CmtyVV, const TIntV& NIDV, const int& Nodes, const int& Coms, const double& ComSzAlpha, const double& MemAlpha, const int& MinSz, const int& MaxSz, const int& MinK, const int& MaxK, TRnd& Rnd){
  if (Coms == 0 || Nodes == 0) {
    CmtyVV.Clr();
    return;
  }
  TIntV ComSzSeq, MemSeq;
  TAGMUtil::GenPLSeq(ComSzSeq,Coms,ComSzAlpha,Rnd,MinSz,MaxSz);
  TAGMUtil::GenPLSeq(MemSeq,Nodes,MemAlpha,Rnd,MinK,MaxK);
  TIntPrV CIDSzPrV, NIDMemPrV;
  for (int i = 0; i < ComSzSeq.Len(); i++) {
    CIDSzPrV.Add(TIntPr(i, ComSzSeq[i]));
  }
  for (int i = 0; i < MemSeq.Len(); i++) {
    NIDMemPrV.Add(TIntPr(NIDV[i], MemSeq[i]));
  }
  TAGMUtil::ConnectCmtyVV(CmtyVV, CIDSzPrV, NIDMemPrV, Rnd);
}

///Generate bipartite community affiliation from given power law coefficients for membership distribution and community size distribution.
void TAGMUtil::ConnectCmtyVV(TVec<TIntV>& CmtyVV, const TIntPrV& CIDSzPrV, const TIntPrV& NIDMemPrV, TRnd& Rnd) {
  const int Nodes = NIDMemPrV.Len(), Coms = CIDSzPrV.Len();
  TIntV NDegV,CDegV;
  TIntPrSet CNIDSet;
  TIntSet HitNodes(Nodes);
  THash<TInt,TIntV> CmtyVH;
  for (int i = 0;i < CIDSzPrV.Len(); i++) {
    for (int j = 0; j < CIDSzPrV[i].Val2; j++) {
      CDegV.Add(CIDSzPrV[i].Val1);
    }
  }
  for (int i = 0; i < NIDMemPrV.Len(); i++) {
    for (int j = 0; j < NIDMemPrV[i].Val2; j++) {
      NDegV.Add(NIDMemPrV[i].Val1);
    }
  }
  while (CDegV.Len() < (int) (1.2 * Nodes)) {
    CDegV.Add(CIDSzPrV[Rnd.GetUniDevInt(Coms)].Val1);
  }
  while (NDegV.Len() < CDegV.Len()) {
    NDegV.Add(NIDMemPrV[Rnd.GetUniDevInt(Nodes)].Val1);
  }
  printf("Total Mem: %d, Total Sz: %d\n",NDegV.Len(), CDegV.Len());
  int c=0;
  while (c++ < 15 && CDegV.Len() > 1) {
    for (int i = 0; i < CDegV.Len(); i++) {
      int u = Rnd.GetUniDevInt(CDegV.Len());
      int v = Rnd.GetUniDevInt(NDegV.Len());
      if (CNIDSet.IsKey(TIntPr(CDegV[u], NDegV[v]))) { continue; }
      CNIDSet.AddKey(TIntPr(CDegV[u], NDegV[v]));
      HitNodes.AddKey(NDegV[v]);
      if (u == CDegV.Len() - 1) { CDegV.DelLast(); }
      else { 
        CDegV[u] = CDegV.Last(); 
        CDegV.DelLast();
      }
      if (v == NDegV.Len() - 1) { NDegV.DelLast(); }
      else { 
        NDegV[v] = NDegV.Last();
        NDegV.DelLast();
      }
    }
  }
  //make sure that every node belongs to at least one community
  for (int i = 0; i < Nodes; i++) {
    int NID = NIDMemPrV[i].Val1;
    if (! HitNodes.IsKey(NID)) {
      CNIDSet.AddKey(TIntPr(CIDSzPrV[Rnd.GetUniDevInt(Coms)].Val1, NID));
      HitNodes.AddKey(NID);
    }
  }
  IAssert(HitNodes.Len() == Nodes);
  for (int i = 0; i < CNIDSet.Len(); i++) {
    TIntPr CNIDPr = CNIDSet[i];
    CmtyVH.AddDat(CNIDPr.Val1);
    CmtyVH.GetDat(CNIDPr.Val1).Add(CNIDPr.Val2);
  }
  CmtyVH.GetDatV(CmtyVV);
}

/// rewire bipartite community affiliation graphs
void TAGMUtil::RewireCmtyVV(const TVec<TIntV>& CmtyVVIn, TVec<TIntV>& CmtyVVOut, TRnd& Rnd){
  THash<TInt,TIntV> CmtyVH;
  for (int i = 0; i < CmtyVVIn.Len(); i++) {
    CmtyVH.AddDat(i, CmtyVVIn[i]);
  }
  TAGMUtil::RewireCmtyNID(CmtyVH, Rnd);
  CmtyVH.GetDatV(CmtyVVOut);
}

/// rewire bipartite community affiliation graphs
void TAGMUtil::RewireCmtyNID(THash<TInt,TIntV >& CmtyVH, TRnd& Rnd) {
  THash<TInt,TIntV > NewCmtyVH(CmtyVH.Len());
  TIntV NDegV;
  TIntV CDegV;
  for (int i = 0; i < CmtyVH.Len(); i++) {
    int CID = CmtyVH.GetKey(i);
    for (int j = 0; j < CmtyVH[i].Len(); j++) {
      int NID = CmtyVH[i][j];
      NDegV.Add(NID);
      CDegV.Add(CID);
    }
  }
  TIntPrSet CNIDSet(CDegV.Len());
  int c=0;
  while (c++ < 15 && CDegV.Len() > 1){
    for (int i = 0; i < CDegV.Len(); i++) {
      int u = Rnd.GetUniDevInt(CDegV.Len());
      int v = Rnd.GetUniDevInt(NDegV.Len());
      if (CNIDSet.IsKey(TIntPr(CDegV[u], NDegV[v]))) { continue; }
      CNIDSet.AddKey(TIntPr(CDegV[u], NDegV[v]));
      if (u == CDegV.Len() - 1) { 
        CDegV.DelLast(); 
      }  else {
        CDegV[u] = CDegV.Last();
        CDegV.DelLast();
      }
      if ( v == NDegV.Len() - 1) {
        NDegV.DelLast();
      }  else{
        NDegV[v] = NDegV.Last();
        NDegV.DelLast();
      }
    }
  }
  for (int i = 0; i < CNIDSet.Len(); i++) {
    TIntPr CNIDPr = CNIDSet[i];
    IAssert(CmtyVH.IsKey(CNIDPr.Val1));
    NewCmtyVH.AddDat(CNIDPr.Val1);
    NewCmtyVH.GetDat(CNIDPr.Val1).Add(CNIDPr.Val2);
  }
  CmtyVH = NewCmtyVH;
}

/// load bipartite community affiliation graph from text file (each row contains the member node IDs for each community)
void TAGMUtil::LoadCmtyVV(const TStr& InFNm, TVec<TIntV>& CmtyVV) {
  CmtyVV.Gen(Kilo(100), 0);
  TSsParser Ss(InFNm, ssfWhiteSep);
  while (Ss.Next()) {
    if(Ss.GetFlds() > 0) {
      TIntV CmtyV;
      for (int i = 0; i < Ss.GetFlds(); i++) {
        if (Ss.IsInt(i)) {
          CmtyV.Add(Ss.GetInt(i));
        }
      }
      CmtyVV.Add(CmtyV);
    }
  }
  CmtyVV.Pack();
  printf("community loading completed (%d communities)\n",CmtyVV.Len());

}

/// load bipartite community affiliation graph from text file (each row contains the member node IDs for each community)
void TAGMUtil::LoadCmtyVV(const TStr& InFNm, TVec<TIntV>& CmtyVV, TStrHash<TInt>& StrToNIdH, const int BeginCol, const int MinSz, const TSsFmt Sep) {
  CmtyVV.Gen(Kilo(100), 0);
  TSsParser Ss(InFNm, Sep);
  while (Ss.Next()) {
    if(Ss.GetFlds() > BeginCol) {
      TIntV CmtyV;
      for (int i = BeginCol; i < Ss.GetFlds(); i++) {
        if (StrToNIdH.IsKey(Ss.GetFld(i))) {
          CmtyV.Add(StrToNIdH.GetKeyId(Ss.GetFld(i)));
        }
      }
      if (CmtyV.Len() < MinSz) { continue; }
      CmtyVV.Add(CmtyV);
    }
  }
  CmtyVV.Pack();
  printf("community loading completed (%d communities)\n",CmtyVV.Len());
}

/// dump bipartite community affiliation into a text file
void TAGMUtil::DumpCmtyVV(const TStr& OutFNm, const TVec<TIntV>& CmtyVV) {
  FILE* F = fopen(OutFNm.CStr(),"wt");
  for (int i = 0; i < CmtyVV.Len(); i++) {
    for (int j = 0; j < CmtyVV[i].Len(); j++) {
      fprintf(F,"%d\t", (int) CmtyVV[i][j]);
    }
    fprintf(F,"\n");
  }
  fclose(F);
}

/// dump bipartite community affiliation into a text file with node names
void TAGMUtil::DumpCmtyVV(const TStr OutFNm, TVec<TIntV>& CmtyVV, TIntStrH& NIDNmH) {
  FILE* F = fopen(OutFNm.CStr(), "wt");
  for (int c = 0; c < CmtyVV.Len(); c++) {
    for (int u = 0; u < CmtyVV[c].Len(); u++) {
      if (NIDNmH.IsKey(CmtyVV[c][u])){
        fprintf(F, "%s\t", NIDNmH.GetDat(CmtyVV[c][u]).CStr());
      }
      else {
        fprintf(F, "%d\t", (int) CmtyVV[c][u]);
      }
    }
    fprintf(F, "\n");
  }
  fclose(F);
}

/// total number of memberships (== sum of the sizes of communities)
int TAGMUtil::TotalMemberships(const TVec<TIntV>& CmtyVV){
  int M = 0;
  for (int i = 0; i < CmtyVV.Len(); i++) {
    M += CmtyVV[i].Len();
  }
  return M;
}

/// get hash table of <Node ID, membership size>
void TAGMUtil::GetNodeMembership(TIntH& NIDComVH, const THash<TInt,TIntV >& CmtyVH) {
  NIDComVH.Clr();
  for (THash<TInt,TIntV>::TIter HI = CmtyVH.BegI(); HI < CmtyVH.EndI(); HI++){
    for (int j = 0;j < HI.GetDat().Len(); j++) {
      int NID = HI.GetDat()[j];
      NIDComVH.AddDat(NID)++;
    }
  }
}

/// get hash table of <Node ID, community IDs which node belongs to>
void TAGMUtil::GetNodeMembership(THash<TInt,TIntSet >& NIDComVH, const TVec<TIntV>& CmtyVV) {
  NIDComVH.Clr();
  for (int i = 0; i < CmtyVV.Len(); i++){
    int CID = i;
    for (int j = 0; j < CmtyVV[i].Len(); j++) {
      int NID = CmtyVV[i][j];
      NIDComVH.AddDat(NID).AddKey(CID);
    }
  }
}

/// get hash table of <Node ID, community IDs which node belongs to>. Some nodes in NIDV might belong to no community
void TAGMUtil::GetNodeMembership(THash<TInt,TIntSet >& NIDComVH, const TVec<TIntV>& CmtyVV, const TIntV& NIDV) {
  NIDComVH.Clr();
  for (int u = 0; u < NIDV.Len(); u++) {
    NIDComVH.AddDat(NIDV[u]);
  }
  for (int i = 0; i < CmtyVV.Len(); i++){
    int CID = i;
    for (int j = 0; j < CmtyVV[i].Len(); j++) {
      int NID = CmtyVV[i][j];
      NIDComVH.AddDat(NID).AddKey(CID);
    }
  }
}


void TAGMUtil::GetNodeMembership(THash<TInt,TIntSet >& NIDComVH, const TVec<TIntSet>& CmtyVV) {
  for (int i = 0; i < CmtyVV.Len(); i++){
    int CID = i;
    for (TIntSet::TIter SI = CmtyVV[i].BegI(); SI < CmtyVV[i].EndI(); SI++) {
      int NID = SI.GetKey();
      NIDComVH.AddDat(NID).AddKey(CID);
    }
  }
}
void TAGMUtil::GetNodeMembership(THash<TInt,TIntSet >& NIDComVH, const THash<TInt,TIntV>& CmtyVH) {
  for (THash<TInt,TIntV>::TIter HI = CmtyVH.BegI(); HI < CmtyVH.EndI(); HI++){
    int CID = HI.GetKey();
    for (int j = 0; j < HI.GetDat().Len(); j++) {
      int NID = HI.GetDat()[j];
      NIDComVH.AddDat(NID).AddKey(CID);
    }
  }
}

void TAGMUtil::GetNodeMembership(THash<TInt,TIntV >& NIDComVH, const THash<TInt,TIntV>& CmtyVH) {
  for (int i = 0; i < CmtyVH.Len(); i++){
    int CID = CmtyVH.GetKey(i);
    for (int j = 0; j < CmtyVH[i].Len(); j++) {
      int NID = CmtyVH[i][j];
      NIDComVH.AddDat(NID).Add(CID);
    }
  }
}

void TAGMUtil::GetNodeMembership(THash<TInt,TIntV >& NIDComVH, const TVec<TIntV>& CmtyVV) {
  THash<TInt,TIntV> CmtyVH;
  for (int i = 0; i < CmtyVV.Len(); i++) {
    CmtyVH.AddDat(i, CmtyVV[i]);
  }
  GetNodeMembership(NIDComVH, CmtyVH);
}

int TAGMUtil::Intersection(const TIntV& C1, const TIntV& C2) {
  TIntSet S1(C1), S2(C2);
  return TAGMUtil::Intersection(S1, S2);
}

void TAGMUtil::GetIntersection(const THashSet<TInt>& A, const THashSet<TInt>& B, THashSet<TInt>& C) {
  C.Gen(A.Len());
  if (A.Len() < B.Len()) {
    for (THashSetKeyI<TInt> it = A.BegI(); it < A.EndI(); it++) 
      if (B.IsKey(it.GetKey())) C.AddKey(it.GetKey());
  } else {
    for (THashSetKeyI<TInt> it = B.BegI(); it < B.EndI(); it++) 
      if (A.IsKey(it.GetKey())) C.AddKey(it.GetKey());
  }
}

int TAGMUtil::Intersection(const THashSet<TInt>& A, const THashSet<TInt>& B) {
  int n = 0;
  if (A.Len() < B.Len()) {
    for (THashSetKeyI<TInt> it = A.BegI(); it < A.EndI(); it++) 
      if (B.IsKey(it.GetKey())) n++;
  } else {
    for (THashSetKeyI<TInt> it = B.BegI(); it < B.EndI(); it++) 
      if (A.IsKey(it.GetKey())) n++;
  }
  return n;
}

/// save graph into a gexf file which Gephi can read
void TAGMUtil::SaveGephi(const TStr& OutFNm, const PUNGraph& G, const TVec<TIntV>& CmtyVVAtr, const double MaxSz, const double MinSz, const TIntStrH& NIDNameH, const THash<TInt, TIntTr>& NIDColorH ) {
  THash<TInt,TIntV> NIDComVHAtr;
  TAGMUtil::GetNodeMembership(NIDComVHAtr, CmtyVVAtr);

  FILE* F = fopen(OutFNm.CStr(), "wt");
  fprintf(F, "<?xml version='1.0' encoding='UTF-8'?>\n");
  fprintf(F, "<gexf xmlns='http://www.gexf.net/1.2draft' xmlns:viz='http://www.gexf.net/1.1draft/viz' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:schemaLocation='http://www.gexf.net/1.2draft http://www.gexf.net/1.2draft/gexf.xsd' version='1.2'>\n");
  fprintf(F, "\t<graph mode='static' defaultedgetype='undirected'>\n");
  if (CmtyVVAtr.Len() > 0) {
    fprintf(F, "\t<attributes class='node'>\n");
    for (int c = 0; c < CmtyVVAtr.Len(); c++) {
      fprintf(F, "\t\t<attribute id='%d' title='c%d' type='boolean'>", c, c);
      fprintf(F, "\t\t<default>false</default>\n");
      fprintf(F, "\t\t</attribute>\n");
    }
    fprintf(F, "\t</attributes>\n");
  }
  fprintf(F, "\t\t<nodes>\n");
  for (TUNGraph::TNodeI NI = G->BegNI(); NI < G->EndNI(); NI++) {
    int NID = NI.GetId();
    TStr Label = NIDNameH.IsKey(NID)? NIDNameH.GetDat(NID): "";
    TIntTr Color = NIDColorH.IsKey(NID)? NIDColorH.GetDat(NID) : TIntTr(120, 120, 120);

    double Size = MinSz;
    double SizeStep = (MaxSz - MinSz) / (double) CmtyVVAtr.Len();
    if (NIDComVHAtr.IsKey(NID)) {
      Size = MinSz +  SizeStep *  (double) NIDComVHAtr.GetDat(NID).Len();
    }
    double Alpha = 1.0;
    fprintf(F, "\t\t\t<node id='%d' label='%s'>\n", NID, Label.CStr());
    fprintf(F, "\t\t\t\t<viz:color r='%d' g='%d' b='%d' a='%.1f'/>\n", Color.Val1.Val, Color.Val2.Val, Color.Val3.Val, Alpha);
    fprintf(F, "\t\t\t\t<viz:size value='%.3f'/>\n", Size);
    //specify attributes
    if (NIDComVHAtr.IsKey(NID)) {
      fprintf(F, "\t\t\t\t<attvalues>\n");
      for (int c = 0; c < NIDComVHAtr.GetDat(NID).Len(); c++) {
        int CID = NIDComVHAtr.GetDat(NID)[c];
        fprintf(F, "\t\t\t\t\t<attvalue for='%d' value='true'/>\n", CID);
      }
      fprintf(F, "\t\t\t\t</attvalues>\n");
    }

    fprintf(F, "\t\t\t</node>\n");
  }
  fprintf(F, "\t\t</nodes>\n");
  //plot edges
  int EID = 0;
  fprintf(F, "\t\t<edges>\n");
  for (TUNGraph::TEdgeI EI = G->BegEI(); EI < G->EndEI(); EI++) {
    fprintf(F, "\t\t\t<edge id='%d' source='%d' target='%d'/>\n", EID++, EI.GetSrcNId(), EI.GetDstNId());
  }
  fprintf(F, "\t\t</edges>\n");
  fprintf(F, "\t</graph>\n");
  fprintf(F, "</gexf>\n");
  fclose(F);
}

/// save bipartite community affiliation into gexf file
void TAGMUtil::SaveBipartiteGephi(const TStr& OutFNm, const TIntV& NIDV, const TVec<TIntV>& CmtyVV, const double MaxSz, const double MinSz, const TIntStrH& NIDNameH, const THash<TInt, TIntTr>& NIDColorH, const THash<TInt, TIntTr>& CIDColorH ) {
  /// Plot bipartite graph
  if (CmtyVV.Len() == 0) { return; }
  double NXMin = 0.1, YMin = 0.1, NXMax = 250.00, YMax = 30.0;
  double CXMin = 0.3 * NXMax, CXMax = 0.7 * NXMax;
  double CStep = (CXMax - CXMin) / (double) CmtyVV.Len(), NStep = (NXMax - NXMin) / (double) NIDV.Len();
  THash<TInt,TIntV> NIDComVH;
  TAGMUtil::GetNodeMembership(NIDComVH, CmtyVV);

  FILE* F = fopen(OutFNm.CStr(), "wt");
  fprintf(F, "<?xml version='1.0' encoding='UTF-8'?>\n");
  fprintf(F, "<gexf xmlns='http://www.gexf.net/1.2draft' xmlns:viz='http://www.gexf.net/1.1draft/viz' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:schemaLocation='http://www.gexf.net/1.2draft http://www.gexf.net/1.2draft/gexf.xsd' version='1.2'>\n");
  fprintf(F, "\t<graph mode='static' defaultedgetype='directed'>\n");
  fprintf(F, "\t\t<nodes>\n");
  for (int c = 0; c < CmtyVV.Len(); c++) {
    int CID = c;
    double XPos = c * CStep + CXMin;
    TIntTr Color = CIDColorH.IsKey(CID)? CIDColorH.GetDat(CID) : TIntTr(120, 120, 120);
    fprintf(F, "\t\t\t<node id='C%d' label='C%d'>\n", CID, CID);
    fprintf(F, "\t\t\t\t<viz:color r='%d' g='%d' b='%d'/>\n", Color.Val1.Val, Color.Val2.Val, Color.Val3.Val);
    fprintf(F, "\t\t\t\t<viz:size value='%.3f'/>\n", MaxSz);
    fprintf(F, "\t\t\t\t<viz:shape value='square'/>\n");
    fprintf(F, "\t\t\t\t<viz:position x='%f' y='%f' z='0.0'/>\n", XPos, YMax); 
    fprintf(F, "\t\t\t</node>\n");
  }

  for (int u = 0;u < NIDV.Len(); u++) {
    int NID = NIDV[u];
    TStr Label = NIDNameH.IsKey(NID)? NIDNameH.GetDat(NID): "";
    double Size = MinSz;
    double XPos = NXMin + u * NStep;
    TIntTr Color = NIDColorH.IsKey(NID)? NIDColorH.GetDat(NID) : TIntTr(120, 120, 120);
    double Alpha = 1.0;
    fprintf(F, "\t\t\t<node id='%d' label='%s'>\n", NID, Label.CStr());
    fprintf(F, "\t\t\t\t<viz:color r='%d' g='%d' b='%d' a='%.1f'/>\n", Color.Val1.Val, Color.Val2.Val, Color.Val3.Val, Alpha);
    fprintf(F, "\t\t\t\t<viz:size value='%.3f'/>\n", Size);
    fprintf(F, "\t\t\t\t<viz:shape value='square'/>\n");
    fprintf(F, "\t\t\t\t<viz:position x='%f' y='%f' z='0.0'/>\n", XPos, YMin); 
    fprintf(F, "\t\t\t</node>\n");
  }
  fprintf(F, "\t\t</nodes>\n");
  fprintf(F, "\t\t<edges>\n");
  int EID = 0;
  for (int u = 0;u < NIDV.Len(); u++) {
    int NID = NIDV[u];
    if (NIDComVH.IsKey(NID)) {
      for (int c = 0; c < NIDComVH.GetDat(NID).Len(); c++) {
        int CID = NIDComVH.GetDat(NID)[c];
        fprintf(F, "\t\t\t<edge id='%d' source='C%d' target='%d'/>\n", EID++, CID, NID);
      }
    }
  }
  fprintf(F, "\t\t</edges>\n");
  fprintf(F, "\t</graph>\n");
  fprintf(F, "</gexf>\n");
}

/// estimate number of communities using AGM
int TAGMUtil::FindComsByAGM(const PUNGraph& Graph, const int InitComs, const int MaxIter, const int RndSeed, const double RegGap, const double PNoCom, const TStr PltFPrx) {
  TRnd Rnd(RndSeed);
  int LambdaIter = 100;
  if (Graph->GetNodes() < 200) { LambdaIter = 1; } 
  if (Graph->GetNodes() < 200 && Graph->GetEdges() > 2000) { LambdaIter = 100; } 

  //Find coms with large C
  TAGMFit AGMFitM(Graph, InitComs, RndSeed);
  if (PNoCom > 0.0) { AGMFitM.SetPNoCom(PNoCom); }
  AGMFitM.RunMCMC(MaxIter, LambdaIter, "");

  int TE = Graph->GetEdges();
  TFltV RegV; 
  RegV.Add(0.3 * TE);
  for (int r = 0; r < 25; r++) {
    RegV.Add(RegV.Last() * RegGap);
  }
  TFltPrV RegComsV, RegLV, RegBICV;
  TFltV LV, BICV;
  //record likelihood and number of communities with nonzero P_c
  for (int r = 0; r < RegV.Len(); r++) {
    double RegCoef = RegV[r];
    AGMFitM.SetRegCoef(RegCoef);
    AGMFitM.MLEGradAscentGivenCAG(0.01, 1000);
    AGMFitM.SetRegCoef(0.0);
        
    TVec<TIntV> EstCmtyVV;
    AGMFitM.GetCmtyVV(EstCmtyVV, 0.99);
    int NumLowQ = EstCmtyVV.Len();
    RegComsV.Add(TFltPr(RegCoef, (double) NumLowQ));

    if (EstCmtyVV.Len() > 0) {
      TAGMFit AFTemp(Graph, EstCmtyVV, Rnd);
      AFTemp.MLEGradAscentGivenCAG(0.001, 1000);
      double CurL = AFTemp.Likelihood();
      LV.Add(CurL);
      BICV.Add(-2.0 * CurL + (double) EstCmtyVV.Len() * log((double) Graph->GetNodes() * (Graph->GetNodes() - 1) / 2.0));
    }
    else {
      break;
    }
  }
  // if likelihood does not exist or does not change at all, report the smallest number of communities or 2
  if (LV.Len() == 0) { return 2; }
  else if (LV[0] == LV.Last()) { return (int) TMath::Mx<TFlt>(2.0, RegComsV[LV.Len() - 1].Val2); }


  //normalize likelihood and BIC to 0~100
  int MaxL = 100;
  {
    TFltV& ValueV = LV;
    TFltPrV& RegValueV = RegLV;
    double MinValue = TFlt::Mx, MaxValue = TFlt::Mn;
    for (int l = 0; l < ValueV.Len(); l++) {
      if (ValueV[l] < MinValue) { MinValue = ValueV[l]; }
      if (ValueV[l] > MaxValue) { MaxValue = ValueV[l]; }
    }
    while (ValueV.Len() < RegV.Len()) { ValueV.Add(MinValue); }
    double RangeVal = MaxValue - MinValue;
    for (int l = 0; l < ValueV.Len(); l++) {
      RegValueV.Add(TFltPr(RegV[l], double(MaxL) * (ValueV[l] - MinValue) / RangeVal));
    }
    
  }
  {
    TFltV& ValueV = BICV;
    TFltPrV& RegValueV = RegBICV;
    double MinValue = TFlt::Mx, MaxValue = TFlt::Mn;
    for (int l = 0; l < ValueV.Len(); l++) {
      if (ValueV[l] < MinValue) { MinValue = ValueV[l]; }
      if (ValueV[l] > MaxValue) { MaxValue = ValueV[l]; }
    }
    while (ValueV.Len() < RegV.Len()) { ValueV.Add(MaxValue); }
    double RangeVal = MaxValue - MinValue;
    for (int l = 0; l < ValueV.Len(); l++) {
      RegValueV.Add(TFltPr(RegV[l], double(MaxL) * (ValueV[l] - MinValue) / RangeVal));
    }
  }

  //fit logistic regression to normalized likelihood.
  TVec<TFltV> XV(RegLV.Len());
  TFltV YV (RegLV.Len());
  for (int l = 0; l < RegLV.Len(); l++) {
    XV[l] = TFltV::GetV(log(RegLV[l].Val1));
    YV[l] = RegLV[l].Val2 / (double) MaxL;
  }
  TFltPrV LRVScaled, LRV;
  TLogRegFit LRFit;
  PLogRegPredict LRMd = LRFit.CalcLogRegNewton(XV, YV, PltFPrx);
  for (int l = 0; l < RegLV.Len(); l++) {
    LRV.Add(TFltPr(RegV[l], LRMd->GetCfy(XV[l])));
    LRVScaled.Add(TFltPr(RegV[l], double(MaxL) * LRV.Last().Val2));
  }

  //estimate # communities from fitted logistic regression
  int NumComs = 0, IdxRegDrop = 0;
  double LRThres = 1.1, RegDrop; // 1 / (1 + exp(1.1)) = 0.25
  double LeftReg = 0.0, RightReg = 0.0;
  TFltV Theta;
  LRMd->GetTheta(Theta);
  RegDrop = (- Theta[1] - LRThres) / Theta[0];
  if (RegDrop <= XV[0][0]) { NumComs = (int) RegComsV[0].Val2; }
  else if (RegDrop >= XV.Last()[0]) { NumComs = (int) RegComsV.Last().Val2; }
  else {  //interpolate for RegDrop
    for (int i = 0; i < XV.Len(); i++) {
      if (XV[i][0] > RegDrop) { IdxRegDrop = i; break; }
    }
    
    if (IdxRegDrop == 0) {
      printf("Error!! RegDrop:%f, Theta[0]:%f, Theta[1]:%f\n", RegDrop, Theta[0].Val, Theta[1].Val);
      for (int l = 0; l < RegLV.Len(); l++) {
        printf("X[%d]:%f, Y[%d]:%f\n", l, XV[l][0].Val, l, YV[l].Val);
      }
    }
    IAssert(IdxRegDrop > 0);
    LeftReg = RegDrop - XV[IdxRegDrop - 1][0];
    RightReg = XV[IdxRegDrop][0] - RegDrop;
    NumComs = (int) TMath::Round( (RightReg * RegComsV[IdxRegDrop - 1].Val2 + LeftReg * RegComsV[IdxRegDrop].Val2) / (LeftReg + RightReg));

  }
  //printf("Interpolation coeff: %f, %f, index at drop:%d (%f), Left-Right Vals: %f, %f\n", LeftReg, RightReg, IdxRegDrop, RegDrop, RegComsV[IdxRegDrop - 1].Val2, RegComsV[IdxRegDrop].Val2);
  printf("Num Coms:%d\n", NumComs);
  if (NumComs < 2) { NumComs = 2; }

  if (PltFPrx.Len() > 0) {
    TStr PlotTitle = TStr::Fmt("N:%d, E:%d ", Graph->GetNodes(), TE);
    TGnuPlot GPC(PltFPrx + ".l");
    GPC.AddPlot(RegComsV, gpwLinesPoints, "C");
    GPC.AddPlot(RegLV, gpwLinesPoints, "likelihood");
    GPC.AddPlot(RegBICV, gpwLinesPoints, "BIC");
    GPC.AddPlot(LRVScaled, gpwLinesPoints, "Sigmoid (scaled)");
    GPC.SetScale(gpsLog10X);
    GPC.SetTitle(PlotTitle);
    GPC.SavePng(PltFPrx + ".l.png");
  }
  
  return NumComs;
}

double TAGMUtil::GetConductance(const PUNGraph& Graph, const TIntSet& CmtyS, const int Edges) {
  const int Edges2 = Edges >= 0 ? 2*Edges : Graph->GetEdges();
  int Vol = 0,  Cut = 0; 
  double Phi = 0.0;
  for (int i = 0; i < CmtyS.Len(); i++) {
    if (! Graph->IsNode(CmtyS[i])) { continue; }
    TUNGraph::TNodeI NI = Graph->GetNI(CmtyS[i]);
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      if (! CmtyS.IsKey(NI.GetOutNId(e))) { Cut += 1; }
    }
    Vol += NI.GetOutDeg();
  }
  // get conductance
  if (Vol != Edges2) {
    if (2 * Vol > Edges2) { Phi = Cut / double (Edges2 - Vol); }
    else if (Vol == 0) { Phi = 0.0; }
    else { Phi = Cut / double(Vol); }
  } else {
    if (Vol == Edges2) { Phi = 1.0; }
  }
  return Phi;
}

void TAGMUtil::GetNbhCom(const PUNGraph& Graph, const int NID, TIntSet& NBCmtyS) {
  TUNGraph::TNodeI NI = Graph->GetNI(NID);
  NBCmtyS.Gen(NI.GetDeg());
  NBCmtyS.AddKey(NID);
  for (int e = 0; e < NI.GetDeg(); e++) {
    NBCmtyS.AddKey(NI.GetNbrNId(e));
  }
}

///////////////////////////////////////////////////////////////////////
// Logistic regression by gradient ascent

void TLogRegFit::GetNewtonStep(TFltVV& HVV, const TFltV& GradV, TFltV& DeltaLV){
  bool HSingular = false;
  for (int i = 0; i < HVV.GetXDim(); i++) {
    if (HVV(i,i) == 0.0) {
      HVV(i,i) = 0.001;
      HSingular = true;
    }
    DeltaLV[i] = GradV[i] / HVV(i, i);
  }
  if (! HSingular) {
    if (HVV(0, 0) < 0) { // if Hessian is negative definite, convert it to positive definite
      for (int r = 0; r < Theta.Len(); r++) {
        for (int c = 0; c < Theta.Len(); c++) {
          HVV(r, c) = - HVV(r, c);
        }
      }
      TNumericalStuff::SolveSymetricSystem(HVV, GradV, DeltaLV);
    }
    else {
      TNumericalStuff::SolveSymetricSystem(HVV, GradV, DeltaLV);
      for (int i = 0; i < DeltaLV.Len(); i++) {
        DeltaLV[i] = - DeltaLV[i];
      }
    }

  }
}

void TLogRegFit::Hessian(TFltVV& HVV) {
  HVV.Gen(Theta.Len(), Theta.Len());
  TFltV OutV;
  TLogRegPredict::GetCfy(X, OutV, Theta);
  for (int i = 0; i < X.Len(); i++) {
    for (int r = 0; r < Theta.Len(); r++) {
      HVV.At(r, r) += - (X[i][r] * OutV[i] * (1 - OutV[i]) * X[i][r]);
      for (int c = r + 1; c < Theta.Len(); c++) {
        HVV.At(r, c) += - (X[i][r] * OutV[i] * (1 - OutV[i]) * X[i][c]);
        HVV.At(c, r) += - (X[i][r] * OutV[i] * (1 - OutV[i]) * X[i][c]);
      }
    }
  }
  /*
  printf("\n");
  for (int r = 0; r < Theta.Len(); r++) {
    for (int c = 0; c < Theta.Len(); c++) {
      printf("%f\t", HVV.At(r, c).Val);
    }
    printf("\n");
  }
  */
}

int TLogRegFit::MLENewton(const double& ChangeEps, const int& MaxStep, const TStr PlotNm) {
  TExeTm ExeTm;
  TFltV GradV(Theta.Len()), DeltaLV(Theta.Len());
  TFltVV HVV(Theta.Len(), Theta.Len());
  int iter = 0;
  double MinVal = -1e10, MaxVal = 1e10;
  for(iter = 0; iter < MaxStep; iter++) {
    Gradient(GradV);
    Hessian(HVV);
    GetNewtonStep(HVV, GradV, DeltaLV);
    double Increment = TLinAlg::DotProduct(GradV, DeltaLV);
    if (Increment <= ChangeEps) {break;}
    double LearnRate = GetStepSizeByLineSearch(DeltaLV, GradV, 0.15, 0.5);//InitLearnRate/double(0.01*(double)iter + 1);
    for(int i = 0; i < Theta.Len(); i++) {
      double Change = LearnRate * DeltaLV[i];
      Theta[i] += Change;
      if(Theta[i] < MinVal) { Theta[i] = MinVal;}
      if(Theta[i] > MaxVal) { Theta[i] = MaxVal;}
    }
  }
  if (! PlotNm.Empty()) {
    printf("MLE with Newton method completed with %d iterations(%s)\n",iter,ExeTm.GetTmStr());
  }

  return iter;
}

int TLogRegFit::MLEGradient(const double& ChangeEps, const int& MaxStep, const TStr PlotNm) {
  TExeTm ExeTm;
  TFltV GradV(Theta.Len());
  int iter = 0;
  TIntFltPrV IterLV, IterGradNormV;
  double MinVal = -1e10, MaxVal = 1e10;
  double GradCutOff = 100000;
  for(iter = 0; iter < MaxStep; iter++) {
    Gradient(GradV);    //if gradient is going out of the boundary, cut off
    for(int i = 0; i < Theta.Len(); i++) {
      if (GradV[i] < -GradCutOff) { GradV[i] = -GradCutOff; }
      if (GradV[i] > GradCutOff) { GradV[i] = GradCutOff; }
      if (Theta[i] <= MinVal && GradV[i] < 0) { GradV[i] = 0.0; }
      if (Theta[i] >= MaxVal && GradV[i] > 0) { GradV[i] = 0.0; }
    }
    double Alpha = 0.15, Beta = 0.9;
    //double LearnRate = 0.1 / (0.1 * iter + 1); //GetStepSizeByLineSearch(GradV, GradV, Alpha, Beta);
    double LearnRate = GetStepSizeByLineSearch(GradV, GradV, Alpha, Beta);
    if (TLinAlg::Norm(GradV) < ChangeEps) { break; }
    for(int i = 0; i < Theta.Len(); i++) {
      double Change = LearnRate * GradV[i];
      Theta[i] += Change;
      if(Theta[i] < MinVal) { Theta[i] = MinVal;}
      if(Theta[i] > MaxVal) { Theta[i] = MaxVal;}
    }
    if (! PlotNm.Empty()) {
      double L = Likelihood();
      IterLV.Add(TIntFltPr(iter, L));
      IterGradNormV.Add(TIntFltPr(iter, TLinAlg::Norm(GradV)));
    }
    
  }
  if (! PlotNm.Empty()) {
    TGnuPlot::PlotValV(IterLV, PlotNm + ".likelihood_Q");
    TGnuPlot::PlotValV(IterGradNormV, PlotNm + ".gradnorm_Q");
    printf("MLE for Lambda completed with %d iterations(%s)\n",iter,ExeTm.GetTmStr());
  }
  return iter;
}

double TLogRegFit::GetStepSizeByLineSearch(const TFltV& DeltaV, const TFltV& GradV, const double& Alpha, const double& Beta) {
  double StepSize = 1.0;
  double InitLikelihood = Likelihood();
  IAssert(Theta.Len() == DeltaV.Len());
  TFltV NewThetaV(Theta.Len());
  double MinVal = -1e10, MaxVal = 1e10;
  for(int iter = 0; ; iter++) {
    for (int i = 0; i < Theta.Len(); i++){
      NewThetaV[i] = Theta[i] + StepSize * DeltaV[i];
      if (NewThetaV[i] < MinVal) { NewThetaV[i] = MinVal;  }
      if (NewThetaV[i] > MaxVal) { NewThetaV[i] = MaxVal; }
    }
    if (Likelihood(NewThetaV) < InitLikelihood + Alpha * StepSize * TLinAlg::DotProduct(GradV, DeltaV)) {
      StepSize *= Beta;
    } else {
      break;
    }
  }
  return StepSize;
}

double TLogRegFit::Likelihood(const TFltV& NewTheta) {
  TFltV OutV;
  TLogRegPredict::GetCfy(X, OutV, NewTheta);
  double L = 0;
  for (int r = 0; r < OutV.Len(); r++) {
    L += Y[r] * log(OutV[r]);
    L += (1 - Y[r]) * log(1 - OutV[r]);
  }
  return L;
}

void TLogRegFit::Gradient(TFltV& GradV) {
  TFltV OutV;
  TLogRegPredict::GetCfy(X, OutV, Theta);
  GradV.Gen(M);
  for (int r = 0; r < X.Len(); r++) {
    //printf("Y[%d] = %f, Out[%d] = %f\n", r, Y[r].Val, r, OutV[r].Val);
    for (int m = 0; m < M; m++) {
      GradV[m] += (Y[r] - OutV[r]) * X[r][m];
    }
  }
  //for (int m = 0; m < M; m++) {  printf("Theta[%d] = %f, GradV[%d] = %f\n", m, Theta[m].Val, m, GradV[m].Val); }
}

PLogRegPredict TLogRegFit::CalcLogRegNewton(const TVec<TFltV>& XPt, const TFltV& yPt, const TStr& PlotNm, const double& ChangeEps, const int& MaxStep, const bool Intercept) {

  X = XPt;
  Y = yPt;
  IAssert(X.Len() == Y.Len());
  if (Intercept == false) { // if intercept is not included, add it
    for (int r = 0; r < X.Len(); r++) {  X[r].Add(1); }
  }
  M = X[0].Len();
  for (int r = 0; r < X.Len(); r++) {  IAssert(X[r].Len() == M); }
  for (int r = 0; r < Y.Len(); r++) {  
    if (Y[r] >= 0.99999) { Y[r] = 0.99999; }
    if (Y[r] <= 0.00001) { Y[r] = 0.00001; }
  }
  Theta.Gen(M);
  MLENewton(ChangeEps, MaxStep, PlotNm);
  return new TLogRegPredict(Theta); 
};

PLogRegPredict TLogRegFit::CalcLogRegGradient(const TVec<TFltV>& XPt, const TFltV& yPt, const TStr& PlotNm, const double& ChangeEps, const int& MaxStep, const bool Intercept) {
  X = XPt;
  Y = yPt;
  IAssert(X.Len() == Y.Len());
  if (Intercept == false) { // if intercept is not included, add it
    for (int r = 0; r < X.Len(); r++) {  X[r].Add(1); }
  }
  M = X[0].Len();
  for (int r = 0; r < X.Len(); r++) {  IAssert(X[r].Len() == M); }
  for (int r = 0; r < Y.Len(); r++) {  
    if (Y[r] >= 0.99999) { Y[r] = 0.99999; }
    if (Y[r] <= 0.00001) { Y[r] = 0.00001; }
  }
  Theta.Gen(M);
  MLEGradient(ChangeEps, MaxStep, PlotNm);
  return new TLogRegPredict(Theta); 
};

///////////////////////////////////////////////////////////////////////
// Logistic-Regression-Model

double TLogRegPredict::GetCfy(const TFltV& AttrV, const TFltV& NewTheta) {
    int len = AttrV.Len();
    double res = 0;
    if (len < NewTheta.Len()) { res = NewTheta.Last(); } //if feature vector is shorter, add an intercept
    for (int i = 0; i < len; i++) {
      if (i < NewTheta.Len()) { res += AttrV[i] * NewTheta[i]; }
    }
    double mu = 1 / (1 + exp(-res));
    return mu;
}

void TLogRegPredict::GetCfy(const TVec<TFltV>& X, TFltV& OutV, const TFltV& NewTheta) {
  OutV.Gen(X.Len());
  for (int r = 0; r < X.Len(); r++) {
    OutV[r] = GetCfy(X[r], NewTheta);
  }
}
