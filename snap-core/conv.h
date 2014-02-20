#ifndef CONV_H
#define CONV_H

namespace TSnap {

template<class PGraph>
PGraph ToGraph(PTable Table, const TStr& SrcCol, const TStr& DstCol, TAttrAggr AggrPolicy)
{
	PGraph Graph = PGraph::TObj::New();

  const TAttrType NodeType = Table->GetColType(SrcCol);
  Assert(NodeType == Table->GetColType(DstCol));
  const TInt SrcColIdx = Table->GetColIdx(SrcCol);
  const TInt DstColIdx = Table->GetColIdx(DstCol);
  
  // make single pass over all rows in the table
  if (NodeType == atInt) {
    for (int CurrRowIdx = 0; CurrRowIdx < (Table->Next).Len(); CurrRowIdx++) {
      if ((Table->Next)[CurrRowIdx] == Table->Invalid) { continue; }
      // add src and dst nodes to graph if they are not seen earlier
      TInt SVal = (Table->IntCols)[SrcColIdx][CurrRowIdx];
      TInt DVal = (Table->IntCols)[DstColIdx][CurrRowIdx];
      Graph->AddNodeUnchecked(SVal);
      Graph->AddNodeUnchecked(DVal);
      Graph->AddEdgeUnchecked(SVal, DVal);
    }
  } else if (NodeType == atFlt) {
    // node values - i.e. the unique values of src/dst col
    //THashSet<TInt> IntNodeVals; // for both int and string node attr types.
    THash<TFlt, TInt> FltNodeVals;
    for (int CurrRowIdx = 0; CurrRowIdx < (Table->Next).Len(); CurrRowIdx++) {
      if ((Table->Next)[CurrRowIdx] == Table->Invalid) { continue; }
      // add src and dst nodes to graph if they are not seen earlier
      TInt SVal, DVal;
      TFlt FSVal = (Table->FltCols)[SrcColIdx][CurrRowIdx];
      SVal = Table->CheckAndAddFltNode(Graph, FltNodeVals, FSVal);
      TFlt FDVal = (Table->FltCols)[SrcColIdx][CurrRowIdx];
      DVal = Table->CheckAndAddFltNode(Graph, FltNodeVals, FDVal);
      Graph->AddEdge(SVal, DVal);
    }  
  } else {
    for (int CurrRowIdx = 0; CurrRowIdx < (Table->Next).Len(); CurrRowIdx++) {
      if ((Table->Next)[CurrRowIdx] == Table->Invalid) { continue; }
      // add src and dst nodes to graph if they are not seen earlier
      TInt SVal = (Table->StrColMaps)[SrcColIdx][CurrRowIdx];
      if (strlen(Table->GetContextKey(SVal)) == 0) { continue; }  //illegal value
      TInt DVal = (Table->StrColMaps)[DstColIdx][CurrRowIdx];
      if (strlen(Table->GetContextKey(DVal)) == 0) { continue; }  //illegal value
      Graph->AddNode(SVal);
      Graph->AddNode(DVal);
      Graph->AddEdge(SVal, DVal);
    }  
  }
  
  Graph->SortNodeAdjV();
  return Graph;
}

template<class PGraph> 
PGraph ToNetwork(PTable Table, const TStr& SrcCol, const TStr& DstCol, 
	TStrV& SrcAttrV, TStrV& DstAttrV, TStrV& EdgeAttrV, TAttrAggr AggrPolicy)
{
	PGraph Graph = PGraph::TObj::New();

  const TAttrType NodeType = Table->GetColType(SrcCol);
  Assert(NodeType == Table->GetColType(DstCol));
  const TInt SrcColIdx = Table->GetColIdx(SrcCol);
  const TInt DstColIdx = Table->GetColIdx(DstCol);

	//Table->AddGraphAttributeV(SrcAttrV, false, true, false);
	//Table->AddGraphAttributeV(DstAttrV, false, false, true);
	//Table->AddGraphAttributeV(EdgeAttrV, true, false, true);

  // node values - i.e. the unique values of src/dst col
  //THashSet<TInt> IntNodeVals; // for both int and string node attr types.
  THash<TFlt, TInt> FltNodeVals;

  // node attributes
  THash<TInt, TStrIntVH> NodeIntAttrs;
  THash<TInt, TStrFltVH> NodeFltAttrs;
  THash<TInt, TStrStrVH> NodeStrAttrs;

  // make single pass over all rows in the table
  for (int CurrRowIdx = 0; CurrRowIdx < (Table->Next).Len(); CurrRowIdx++) {
    if ((Table->Next)[CurrRowIdx] == Table->Invalid) { continue; }

    // add src and dst nodes to graph if they are not seen earlier
   TInt SVal, DVal;
    if (NodeType == atFlt) {
      TFlt FSVal = (Table->FltCols)[SrcColIdx][CurrRowIdx];
      SVal = Table->CheckAndAddFltNode(Graph, FltNodeVals, FSVal);
      TFlt FDVal = (Table->FltCols)[SrcColIdx][CurrRowIdx];
      DVal = Table->CheckAndAddFltNode(Graph, FltNodeVals, FDVal);
    } else if (NodeType == atInt || NodeType == atStr) {
      if (NodeType == atInt) {
        SVal = (Table->IntCols)[SrcColIdx][CurrRowIdx];
        DVal = (Table->IntCols)[DstColIdx][CurrRowIdx];
      } else {
        SVal = (Table->StrColMaps)[SrcColIdx][CurrRowIdx];
        if (strlen(Table->GetContextKey(SVal)) == 0) { continue; }  //illegal value
        DVal = (Table->StrColMaps)[DstColIdx][CurrRowIdx];
        if (strlen(Table->GetContextKey(DVal)) == 0) { continue; }  //illegal value
      }
      if (!Graph->IsNode(SVal)) {Graph->AddNode(SVal); }
      if (!Graph->IsNode(DVal)) {Graph->AddNode(DVal); }
      //CheckAndAddIntNode(Graph, IntNodeVals, SVal);
      //CheckAndAddIntNode(Graph, IntNodeVals, DVal);
    } 

    // add edge and edge attributes 
    Graph->AddEdge(SVal, DVal, CurrRowIdx);

		// Aggregate edge attributes and add to graph
		for (TInt i = 0; i < Table->EdgeAttrV.Len(); i++) {
			TStr ColName = EdgeAttrV[i];
			TAttrType T = Table->GetColType(ColName);
			TInt Index = Table->GetColIdx(ColName);
			switch (T) {
				case atInt:
					Graph->AddIntAttrDatE(CurrRowIdx, Table->IntCols[Index][CurrRowIdx], ColName);
					break;
				case atFlt:
					Graph->AddFltAttrDatE(CurrRowIdx, Table->FltCols[Index][CurrRowIdx], ColName);
					break;
				case atStr:
					Graph->AddStrAttrDatE(CurrRowIdx, Table->GetStrVal(Index, CurrRowIdx), ColName);
					break;
			}
		}

    // get src and dst node attributes into hashmaps
    if ((Table->SrcNodeAttrV).Len() > 0) { 
      Table->AddNodeAttributes(SVal, Table->SrcNodeAttrV, CurrRowIdx, NodeIntAttrs, NodeFltAttrs, NodeStrAttrs);
  	}

    if ((Table->DstNodeAttrV).Len() > 0) {
      Table->AddNodeAttributes(DVal, Table->DstNodeAttrV, CurrRowIdx, NodeIntAttrs, NodeFltAttrs, NodeStrAttrs);
    }
  }

  // aggregate node attributes and add to graph
  if ((Table->SrcNodeAttrV).Len() > 0 || (Table->DstNodeAttrV).Len() > 0) {
    for (TNEANet::TNodeI NodeI = Graph->BegNI(); NodeI < Graph->EndNI(); NodeI++) {
      TInt NId = NodeI.GetId();
      if (NodeIntAttrs.IsKey(NId)) {
        TStrIntVH IntAttrVals = NodeIntAttrs.GetDat(NId);
        for (TStrIntVH::TIter it = IntAttrVals.BegI(); it < IntAttrVals.EndI(); it++) {
          TInt AttrVal = Table->AggregateVector<TInt>(it.GetDat(), AggrPolicy);
          Graph->AddIntAttrDatN(NId, AttrVal, it.GetKey());
        }
      }
      if (NodeFltAttrs.IsKey(NId)) {
        TStrFltVH FltAttrVals = NodeFltAttrs.GetDat(NId);
        for (TStrFltVH::TIter it = FltAttrVals.BegI(); it < FltAttrVals.EndI(); it++) {
          TFlt AttrVal = Table->AggregateVector<TFlt>(it.GetDat(), AggrPolicy);
          Graph->AddFltAttrDatN(NId, AttrVal, it.GetKey());
        }
      }
      if (NodeStrAttrs.IsKey(NId)) {
        TStrStrVH StrAttrVals = NodeStrAttrs.GetDat(NId);
        for (TStrStrVH::TIter it = StrAttrVals.BegI(); it < StrAttrVals.EndI(); it++) {
          TStr AttrVal = Table->AggregateVector<TStr>(it.GetDat(), AggrPolicy);
          Graph->AddStrAttrDatN(NId, AttrVal, it.GetKey());
        }
      }
    }
  }

  return Graph;
}

template<class PGraph>
PGraph ToNetwork(PTable Table, const TStr& SrcCol, const TStr& DstCol, TAttrAggr AggrPolicy)
{
	TStrV V;
	return ToNetwork<PNEANet>(Table, SrcCol, DstCol, V, V, V, AggrPolicy);
}

template<class PGraphMP>
PGraphMP ToGraphMP(PTable Table, const TStr& SrcCol, const TStr& DstCol) {
  //PGraphMP Graph = TNGraphMP::New(4850000, 69000000);
  PGraphMP Graph = TNGraphMP::New(41700000, 1470000000);

  //printf("TNGraphMP::New nodes %d, edges %d\n", Graph->GetNodes(), Graph->GetEdges());
  printf("TNGraphMP::New reserved %d\n", Graph->Reserved());
  
  // make single pass over all rows in the table
  
  int Length = Graph->Reserved();
  int Threads = 40;
  int Delta = (Length + Threads - 1) / Threads;

  printf("Build Threads %d\n", Threads);

  TIntV OutVec(Length);
  TIntV InVec(Length);

  const TInt SrcColIdx = Table->GetColIdx(SrcCol);
  const TInt DstColIdx = Table->GetColIdx(DstCol);

  printf("%d %d\n", SrcColIdx.Val, DstColIdx.Val);
  const int L1 = Length;

  //int *ps = &IntCols[SrcColIdx][0].Val;
  //int *pd = &IntCols[DstColIdx][0].Val;

  //
  // build the node hash table, count the size of edge lists
  //
  const int Last = Table->Next.Len();
  int Nodes = 0;
#pragma omp parallel for schedule(dynamic, 10000) reduction(+:Nodes)
  for (int CurrRowIdx = 0; CurrRowIdx < Last; CurrRowIdx++) {
    //if (Next[CurrRowIdx] == Invalid) {continue;}

    TInt SVal = Table->IntCols[SrcColIdx][CurrRowIdx];
    TInt DVal = Table->IntCols[DstColIdx][CurrRowIdx];

    //SVal = *ps++;
    //DVal = *pd++;

    int SrcIdx = abs((SVal.GetPrimHashCd()) % L1);
    //int SrcIdx = abs((17*SVal.Val) % L1);
    if (!Graph->AddOutEdge1(SrcIdx, SVal, DVal)) {
      Nodes++;
    }
    __sync_fetch_and_add(&OutVec[SrcIdx].Val, 1);
    //OutVec[SrcIdx]++;

    int DstIdx = abs((DVal.GetPrimHashCd()) % L1);
    //int DstIdx = abs((17*DVal.Val) % L1);
    if (!Graph->AddInEdge1(DstIdx, SVal, DVal)) {
      Nodes++;
    }
    __sync_fetch_and_add(&InVec[DstIdx].Val, 1);
    //InVec[DstIdx]++;
  }
  Graph->SetNodes(Nodes);
  printf("nodes %d\n", Nodes);

  uint Edges = 0;
  for (int i = 0; i < Length; i++) {
    Edges += OutVec[i] + InVec[i];
  }
  printf("edges %d\n", (int) (Edges/2));

  //
  // allocate edge lists
  //
  
  Length = Graph->Reserved();
  Threads = 1;
  Delta = (Length + Threads - 1) / Threads;

  printf("Allocate Threads %d\n", Threads);

  // requires memory allocation, no threads seems the best
  // #pragma omp parallel for schedule(static,Delta)
  // omp_set_num_threads(Threads);
  // #pragma omp parallel for schedule(static)
  for (int Idx = 0; Idx < Length; Idx++) {
    if (OutVec[Idx] > 0 || InVec[Idx] > 0) {
      Graph->ReserveNodeDegs(Idx, InVec[Idx], OutVec[Idx]);
    }
  }

  printf("allocated edge lists\n");

  //
  // assign edges
  //

  Length = Graph->Reserved();
  Threads = 80;
  Delta = (Length + Threads - 1) / Threads;

  printf("Assign Threads %d\n", Threads);

#pragma omp parallel for schedule(static,Delta)
  for (int CurrRowIdx = 0; CurrRowIdx < Last; CurrRowIdx++) {
    //if (Next[CurrRowIdx] == Invalid) {continue;}

    TInt SVal = Table->IntCols[SrcColIdx][CurrRowIdx];
    TInt DVal = Table->IntCols[DstColIdx][CurrRowIdx];

    Graph->AddOutEdge2(SVal, DVal);
    Graph->AddInEdge2(SVal, DVal);
  }

  printf("allocated edge lists\n");

  //
  // sort edges
  //

  Length = Graph->Reserved();
  //Threads = 160;
  //Delta = (Length + Threads - 1) / Threads;
  //printf("Sort Threads %d\n", Threads);

  // #pragma omp parallel for schedule(static,Delta)
#pragma omp parallel for schedule(dynamic)
  for (int Idx = 0; Idx < Length; Idx++) {
    if (OutVec[Idx] > 0 || InVec[Idx] > 0) {
      Graph->SortEdges(Idx, InVec[Idx], OutVec[Idx]);
    }
  }

  printf("sorted edge lists\n");

  return Graph;
}

};

#endif //CONV_H
