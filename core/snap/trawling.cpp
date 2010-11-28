#include "stdafx.h"
#include "trawling.h"

/////////////////////////////////////////////////
// Trawling the web for emerging communities 
// graph, left points to right
TTrawling::TTrawling(const PNGraph& Graph, const int& MinSupport) : MinSup(MinSupport) {
  TIntH ItemCntH;
  for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    IAssert(NI.GetOutDeg()==0 || NI.GetInDeg()==0); // edges only point from left to right
    if (NI.GetOutDeg()==0) { continue; }
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      ItemCntH.AddDat(NI.GetOutNId(e)) += 1;
    }
  }

  TIntV RightV;
  for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    IAssert(NI.GetOutDeg()==0 || NI.GetInDeg()==0); // edges only point from left to right
    if (NI.GetOutDeg()==0) { continue; }
    RightV.Clr(false);
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      const int itm = NI.GetOutNId(e);
      // only include items that already are above minimum support
      if (ItemCntH.GetDat(itm) >= MinSup) {
        RightV.Add(itm); }
    }
    if (! RightV.Empty()) {
      NIdSetH.AddDat(NI.GetId(), RightV);
    }
  }
  // 
  for (int n = 0; n < NIdSetH.Len(); n++) {
    const TIntV& Set = NIdSetH[n];
    for (int s = 0; s < Set.Len(); s++) {
      SetNIdH.AddDat(Set[s]).Add(n);
    }
  }
}

TTrawling::TTrawling(const PUNGraph& Graph, const TIntV& LeftNIdV, const int& MinSupport) : MinSup(MinSupport) {
  /*TIntSet LeftSet(LeftNIdV);
  TIntV RightV;
  for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    if (! LeftSet.IsKey(NI.GetId())) { continue; }
    RightV.Clr(false);
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      if (! LeftSet.IsKey(NI.GetId())) { 
        RightV.Add(NI.GetOutNId(e)); 
      }
    }
    NIdSetH.AddDat(NI.GetId(), RightV);
  }*/
}

void Dump(const TIntV& V, TStr Desc="") { 
  printf("%s", Desc.CStr());
  for (int i = 0; i < V.Len(); i++) {
    printf("  %d", V[i]);
  }
  printf("\n");
}

// initialize with itemsets of size 1
void TTrawling::InitItemSets() {
  CandItemH.Clr(false);
  CurItemH.Clr(false);
  MxFqItemSetV.Clr(false);
  for (int s = 0; s < NIdSetH.Len(); s++) {
    const TIntV& I = NIdSetH[s];
    for (int i = 0; i < I.Len(); i++) {
      CandItemH.AddDat(TIntV::GetV(I[i])) += 1; }
  }
  ThresholdSupp();
}

void TTrawling::JoinItems(const TIntV& Item1, const TIntV& Item2, TIntV& JoinItem) {
  int i = 0, j = 0;
  JoinItem.Clr(false);
  const int MaxL = Item1.Len()+1;
  while (i < Item1.Len()) {
    while (j < Item2.Len() && Item2[j] < Item1[i]) { 
      JoinItem.Add(Item2[j]); j++; }
    JoinItem.Add(Item1[i]);
    if (j < Item2.Len() && Item1[i] == Item2[j]) { j++; }
    i++;
    if (JoinItem.Len() > MaxL) { JoinItem.Clr(false); return; }
  }
  while (j < Item2.Len()) { 
    JoinItem.Add(Item2[j]); j++; 
  }
  /*if (JoinItem.Len() > 3) {
    Dump(Item1, "\n1:");
    Dump(Item2, "2:");
    Dump(JoinItem, "J:");
  }*/
  IAssert(JoinItem.IsSorted());
}

void TTrawling::GenCandidates() {
  CandItemH.Clr(false);
  TIntV JoinItem;
  if (CurItemH.GetKey(0).Len() == 1) {
    // join 1-items into 2-items
    for (int i = 0; i < CurItemH.Len(); i++) {
      for (int j = i+1; j < CurItemH.Len(); j++) {
        JoinItems(CurItemH.GetKey(i), CurItemH.GetKey(j), JoinItem);
        if (JoinItem.Len() == CurItemH.GetKey(i).Len()+1) {
          CandItemH.AddDat(JoinItem, 0);
        }
      }
    }
  } else {
    // join longer item sets
    CurItemH.SortByKey();
    for (int i = 0; i < CurItemH.Len(); i++) {
      const TIntV& Set = CurItemH.GetKey(i);
      const int Val = Set[Set.Len()-2];
      for (int j=i+1; j < CurItemH.Len() && CurItemH.GetKey(j)[CurItemH.GetKey(j).Len()-2] == Val; j++) {
        JoinItems(CurItemH.GetKey(i), CurItemH.GetKey(j), JoinItem);
        if (JoinItem.Len() == CurItemH.GetKey(i).Len()+1) {
          CandItemH.AddDat(JoinItem, 0);
        }
      }
    }
  }//*/
}

bool TTrawling::IsIn(const TIntV& BigV, const TIntV& SmallV) {
  if (BigV.Len() < SmallV.Len()) { return false; }
  int CmnEls = 0, b=0, s=0;
  while (b < BigV.Len()) {
    while (s < SmallV.Len() && SmallV[s] < BigV[b]) { 
      return false; 
    }
    if (SmallV[s] == BigV[b]) { 
      if (++CmnEls == SmallV.Len()) { return true; }
      s++;
    }
    b++;
  }
  return false;
}

int TTrawling::GetSupport(const TIntV& ItemSet) {
  int Cnt = 0;
  /*for (int s = 0; s < NIdSetH.Len(); s++) {
    if (IsIn(NIdSetH[s], ItemSet)) { Cnt++; }
  }*/
  TIntH SetCntH;
  for (int i = 0; i < ItemSet.Len(); i++) {
    const TIntV& NIdV = SetNIdH.GetDat(ItemSet[i]);
    for (int n = 0; n < NIdV.Len(); n++) {
      SetCntH.AddDat(NIdV[n]) += 1;
    }
  }
  for (int i = 0; i < SetCntH.Len(); i++) {
    if (SetCntH[i]==ItemSet.Len()) { Cnt++; }
  }
  return Cnt;
}

void TTrawling::CountSupport() {
  for (int c = 0; c < CandItemH.Len(); c++) {
    CandItemH[c] = GetSupport(CandItemH.GetKey(c));
    if (c % Kilo(100) == 0) { printf("."); }
  }
}

// move candidates in CandItemH that are above MinSupport to CurItemH
void TTrawling::ThresholdSupp() {
  CurItemH.Clr(false);
  for (int k=CandItemH.FFirstKeyId(); CandItemH.FNextKeyId(k); ) {
    if (CandItemH[k] >= MinSup) { 
      CurItemH.AddDat(CandItemH.GetKey(k), CandItemH[k]);
    }
  }
}

// take CurItemH, compose item sets of next size and fill the frequent ones in CandItemH
void TTrawling::GenCandAndCntSupp(const int& FqItemsetLen) {
  CandItemH.Clr(false);
  TIntV JoinItem;
  if (CurItemH.GetKey(0).Len() == 1) {
    // join 1-items into 2-items
    for (int i = 0; i < CurItemH.Len(); i++) {
      for (int j = i+1; j < CurItemH.Len(); j++) {
        JoinItems(CurItemH.GetKey(i), CurItemH.GetKey(j), JoinItem);
        if (JoinItem.Len() == CurItemH.GetKey(i).Len()+1) {
          const int Supp = GetSupport(JoinItem);
          if (Supp >= MinSup) {
            CandItemH.AddDat(JoinItem, Supp);
          }
        }
      }
    }
  } else {
    // join longer item sets
    CurItemH.SortByKey();
    TIntV JoinSet(CurItemH.Len());
    for (int i = 0; i < CurItemH.Len(); i++) {
      const TIntV& CurSet = CurItemH.GetKey(i);
      const int Val = CurSet[CurSet.Len()-2];
      // keep chacking next itemsets that differ only at last element
      for (int j=i+1; j < CurItemH.Len() && CurItemH.GetKey(j)[CurItemH.GetKey(j).Len()-2] == Val; j++) {
        JoinItems(CurSet, CurItemH.GetKey(j), JoinItem);
        if (JoinItem.Len() == CurSet.Len()+1) { // new set is of the right length
          const int Supp = GetSupport(JoinItem);
          if (Supp >= MinSup) {
            CandItemH.AddDat(JoinItem, Supp);
            JoinSet[i]=1;
            JoinSet[j]=1;
          }
        }
      }
      if (FqItemsetLen!=-1 && CurSet.Len() >= FqItemsetLen && JoinSet[i]==0) { // expansion was not frequent
        MxFqItemSetV.Add(CurSet);
      }
    }
  }
}

int TTrawling::GetNextFqItemSets(const int& FqItemsetLen) {
  TExeTm ExeTm;
  printf("  C[%d][%s]", CurItemH.Len(), ExeTm.GetStr());
  /* // slow
  GenCandidates(); // CurItemH --> CandItemH
  printf(" S[%d][%s]", CandItemH.Len(), ExeTm.GetStr());
  CountSupport();  // set counters in CandItemH
  printf("T[%s]", ExeTm.GetStr());
  ThresholdSupp(); // CandItemH --> CurItemH
  printf("  Items:  %d\n", CurItemH.Len());*/
  GenCandAndCntSupp(FqItemsetLen);
  CurItemH.Swap(CandItemH);
  return CurItemH.Len();
}

TIntPrV TTrawling::PlotMinFqVsMaxSet(const TStr& OutFNm) {
  InitItemSets();
  TIntPrV SzCntH;
  SzCntH.Add(TIntPr(1, CurItemH.Len()));
  for (int ItemSetSz = 2; ItemSetSz < 100; ItemSetSz++) {
    printf("Itemset size %d:\n", ItemSetSz);
    GetNextFqItemSets();
    if (CurItemH.Empty()) { break; }
    SzCntH.Add(TIntPr(ItemSetSz, CurItemH.Len()));
    TGnuPlot::PlotValV(SzCntH, "itemSet-"+OutFNm, TStr::Fmt("Minimum Suport = %d", MinSup), 
      "Itemset size", "Number of itemsets > Minimum Support");
  }
  return SzCntH;
}
