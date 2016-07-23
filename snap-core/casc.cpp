namespace TSnap {

PNGraph CascGraphSource(PTable P,const TStr C1,const TStr C2,const TStr C3,const TStr C4,const TInt W) {
  // Attribute to Int mapping
  TInt SIdx = P->GetColIdx(C1); //Source
  TInt DIdx = P->GetColIdx(C2); //Dest
  TInt StIdx = P->GetColIdx(C3); //Start
  TInt DuIdx = P->GetColIdx(C4); //Duration
  TIntV MapV;
  TStrV SortBy;
  SortBy.Add(C1);
  P->Order(SortBy);
  TIntV Source; 
  P->ReadIntCol(C1,Source);
  PNGraph Graph = TNGraph::New();
  //Add Nodes
  for (TRowIterator RI = P->BegRI(); RI < P-> EndRI(); RI++) {
    Graph->AddNode(RI.GetRowIdx().Val);
    MapV.Add(RI.GetRowIdx());
  }
  //Add Edges
  for (TRowIterator OI = P->BegRI(); OI < P->EndRI(); OI++) {
    int OIdx = OI.GetRowIdx().Val;
    int ODest = P->GetIntValAtRowIdx(DIdx,OIdx).Val;
    int OStart = P->GetIntValAtRowIdx(StIdx,OIdx).Val;
    int ODur = P->GetIntValAtRowIdx(DuIdx,OIdx).Val;
    // Inline binary Search
    int val = ODest;
    int lo = 0;
    int hi = Source.Len() - 1;
    int index = -1;
    while (hi >= lo) {
      int mid = lo + (hi - lo)/2;
      if (Source.GetVal(mid) > val) { hi = mid - 1;}
      else if (Source.GetVal(mid) < val) { lo = mid + 1;}
      else { index = mid; hi = mid - 1;}
    }
    // End of binary Search
    int BIdx = index;
    for(int i = BIdx; i < Source.Len(); i++) {
      int InIdx = MapV.GetVal(i).Val;
      if (InIdx == OIdx) {continue;}
      int InSource = P->GetIntValAtRowIdx(SIdx,InIdx).Val;
      int InStart = P->GetIntValAtRowIdx(StIdx,InIdx).Val;
      if (InSource != ODest) { break;}
      if (InStart >= (ODur + OStart) && InStart - (ODur + OStart) <= W.Val) {
        if (!Graph->IsEdge(OIdx,InIdx)) {
          Graph->AddEdge(OIdx,InIdx);
        }
      }      
    }
  }
  return Graph;
}

PNGraph CascGraphTime(PTable P,const TStr C1,const TStr C2,const TStr C3,const TStr C4,const TInt W) {
  // Attribute to Int mapping
  TInt SIdx = P->GetColIdx(C1); //Source
  TInt DIdx = P->GetColIdx(C2); //Dest
  TInt StIdx = P->GetColIdx(C3); //Start
  TInt DuIdx = P->GetColIdx(C4); //Duration
  TIntV MapV;
  TStrV SortBy;
  SortBy.Add(C3);
  P->Order(SortBy);
  TIntV Start; 
  P->ReadIntCol(C3,Start);
  PNGraph Graph = TNGraph::New();
  //Add Nodes
  for (TRowIterator RI = P->BegRI(); RI < P-> EndRI(); RI++) {
    Graph->AddNode(RI.GetRowIdx().Val);
    MapV.Add(RI.GetRowIdx());
  }
  //Add Edges
  for (TRowIterator OI = P->BegRI(); OI < P->EndRI(); OI++) {
    int OIdx = OI.GetRowIdx().Val;
    int ODest = P->GetIntValAtRowIdx(DIdx,OIdx).Val;
    int OStart = P->GetIntValAtRowIdx(StIdx,OIdx).Val;
    int ODur = P->GetIntValAtRowIdx(DuIdx,OIdx).Val;
    // Inline binary Search
    int val = OStart + ODur;
    int lo = 0;
    int hi = Start.Len() - 1;
    int index = -1;
    if (val >= Start.GetVal(hi)) { val = Start.GetVal(hi);}
    while (hi >= lo) {
      int mid = lo + (hi - lo)/2;
      if (Start.GetVal(mid) > val) {
        if ((mid-1) >= lo  &&  Start.GetVal(mid - 1) < val) {
          index = mid - 1;break;
        }
        hi = mid - 1;
      }
      else if (Start.GetVal(mid) < val) {
        if (mid + 1 <= hi  &&  Start.GetVal(mid + 1) > val) {
          index = mid;break;
        }
        lo = mid + 1;
      }
      else { index = mid; hi = mid - 1;}
    }
    // End of binary Search
    int BIdx = index;
    for(int i = BIdx; i < Start.Len(); i++) {
      int InIdx = MapV.GetVal(i).Val;
      if (InIdx == OIdx) {continue;}
      int InSource = P->GetIntValAtRowIdx(SIdx,InIdx).Val;
      int InStart = P->GetIntValAtRowIdx(StIdx,InIdx).Val;
      if (InStart - (ODur + OStart) > W.Val) { break;}
      if (InSource == ODest && InStart >= (ODur + OStart)) {
        if (!Graph->IsEdge(OIdx,InIdx)) {
          Graph->AddEdge(OIdx,InIdx);
        }
      }      
    }
  }
  return Graph;
}

PNGraph CascGraph(PTable P,const TStr C1,const TStr C2,const TStr C3,const TStr C4,const TInt W,bool SortParam) {
  if (SortParam) {
    return CascGraphSource(P, C1, C2, C3, C4, W);
  }
  else {
    return CascGraphTime(P, C1, C2, C3, C4, W);
  }
}

void CascFind(PNGraph Graph,PTable P,const TStr C1,const TStr C2,const TStr C3,const TStr C4,TVec<TIntV> &TopCascVV,bool Print) {
  // Attribute to Int mapping
  TInt SIdx = P->GetColIdx(C1);
  TInt DIdx = P->GetColIdx(C2);
  TInt StIdx = P->GetColIdx(C3);
  TInt DuIdx = P->GetColIdx(C4);
  TIntV MapV, PhyV;
  TStrV SortBy;
  SortBy.Add(C3);
  P->Order(SortBy);
  int count = 0;
  for (TRowIterator RI = P->BegRI(); RI < P-> EndRI(); RI++) {
    MapV.Add(RI.GetRowIdx());
    PhyV.Add(count++);
  }
  // After sort attach with each row a rank helpful for sorting
  P->StoreIntCol("Physical",PhyV);
  TInt PIdx = P->GetColIdx("Physical");
  for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    // Check for top cascades
    if (NI.GetInDeg() != 0) { continue;}
    TIntV CurCasc;
    TSnapQueue<TInt> EventQ;
    THashSet<TInt> VisitedH;
    TInt NId = NI.GetId();
    EventQ.Push(NId);
    VisitedH.AddKey(NId);
    CurCasc.Add(P->GetIntValAtRowIdx(PIdx,NId));
    while (! EventQ.Empty()) {
      TNGraph::TNodeI CNI = Graph->GetNI(EventQ.Top().Val); //Get Current Node
      EventQ.Pop();
      // Go over the outdegree nodes of the currernt node
      for (int e = 0; e < CNI.GetOutDeg(); e++) {
        TInt CId = CNI.GetOutNId(e);
        if ( !VisitedH.IsKey(CId)) {
          EventQ.Push(CId);
          VisitedH.AddKey(CId);
          CurCasc.Add(P->GetIntValAtRowIdx(PIdx,CId));
        }
      }
    }
    CurCasc.Sort();
    TIntV ToAddV;
    if (Print && VisitedH.Len() > 1) {
      printf("__casacade__\t%d\n",VisitedH.Len());
    }
    for (TIntV::TIter VI = CurCasc.BegI(); VI < CurCasc.EndI(); VI++) {
      ToAddV.Add(MapV.GetVal(VI->Val));
      if (Print && VisitedH.Len() > 1) {
        int PIdx = MapV.GetVal(VI->Val).Val;
        int PSource = P->GetIntValAtRowIdx(SIdx,PIdx).Val;
        int PDest = P->GetIntValAtRowIdx(DIdx,PIdx).Val;
        int PStart = P->GetIntValAtRowIdx(StIdx,PIdx).Val;    
        int PDur = P->GetIntValAtRowIdx(DuIdx,PIdx).Val;
        printf("%d\t%d\t%d\t%d\t%d\n",PIdx,PSource,PDest,PStart,PDur);
      }   
    }
    if (ToAddV.Len() > 1) {
      TopCascVV.Add(ToAddV);
    }
  }
  return;
}

#ifdef USE_OPENMP
void CascFindMP(PNGraph Graph,PTable P,const TStr C1,const TStr C2,const TStr C3,const TStr C4,TVec<TIntV> &TopCascVV) {
  // Attribute to Int mapping
  TInt SIdx = P->GetColIdx(C1);
  TInt DIdx = P->GetColIdx(C2);
  TInt StIdx = P->GetColIdx(C3);
  TInt DuIdx = P->GetColIdx(C4);
  TIntV MapV, PhyV;
  TStrV SortBy;
  SortBy.Add(C3);
  P->Order(SortBy);
  int count = 0;
  for (TRowIterator RI = P->BegRI(); RI < P-> EndRI(); RI++) {
    MapV.Add(RI.GetRowIdx());
    PhyV.Add(count++);
  }
  P->StoreIntCol("Physical",PhyV);
  TInt PIdx = P->GetColIdx("Physical");
  TIntV GNodeV;
  for (TNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    if (NI.GetInDeg() == 0) { GNodeV.Add(NI.GetId()); }
  }
  TVec<TIntV> ThTopCascVV; // for each thread
  #pragma omp parallel private(ThTopCascVV) num_threads(10)
  {
    #pragma omp for schedule(dynamic,1000)
    for (int i = 0; i < GNodeV.Len(); i++) {
      TIntV CurCasc;
      TSnapQueue<TInt> EventQ;
      THashSet<TInt> VisitedH;
      TInt NId = GNodeV[i];
      EventQ.Push(NId);
      VisitedH.AddKey(NId);
      CurCasc.Add(P->GetIntValAtRowIdx(PIdx,NId));
      while (! EventQ.Empty()) {
        TNGraph::TNodeI CNI = Graph->GetNI(EventQ.Top().Val); //Get Current Node
        EventQ.Pop();
        // Go over the outdegree nodes of the currernt node
        for (int e = 0; e < CNI.GetOutDeg(); e++) {
          TInt CId = CNI.GetOutNId(e);
          if ( !VisitedH.IsKey(CId)) {
            EventQ.Push(CId);
            VisitedH.AddKey(CId);
            CurCasc.Add(P->GetIntValAtRowIdx(PIdx,CId));
          }
        }
      }
      CurCasc.Sort();
      TIntV ToAddV;
      for (TIntV::TIter VI = CurCasc.BegI(); VI < CurCasc.EndI(); VI++) {
        ToAddV.Add(MapV.GetVal(VI->Val));
      }
      if (ToAddV.Len() > 1) { ThTopCascVV.Add(ToAddV);}  
    }
    #pragma omp critical
    {
      for (int j = 0; j < ThTopCascVV.Len(); j++) {
        TopCascVV.Add(ThTopCascVV[j]);
      }
    }
  }
  return;
}
#endif // USE_OPENMP
} //Namespace TSnap
