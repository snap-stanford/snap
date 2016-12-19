#ifndef CONV_H
#define CONV_H

namespace TSnap {

/// Sequentially converts the table into a graph with links from nodes in \c SrcCol to those in \c DstCol.
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
      //Using AddNodeUnchecked ensures that no error is thrown when the same node is seen twice
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
//      if (strlen(Table->GetContextKey(SVal)) == 0) { continue; }  //illegal value
      TInt DVal = (Table->StrColMaps)[DstColIdx][CurrRowIdx];
//      if (strlen(Table->GetContextKey(DVal)) == 0) { continue; }  //illegal value
      //Using AddNodeUnchecked ensures that no error is thrown when the same node is seen twice
      Graph->AddNodeUnchecked(SVal);
      Graph->AddNodeUnchecked(DVal);
      Graph->AddEdgeUnchecked(SVal, DVal);
    }
  }

  Graph->SortNodeAdjV();
  return Graph;
}

///  Converts the Table into a graph with edges from \c SrcCol to \c DstCol, and attribute vector defined by the arguments.
template<class PGraph>
PGraph ToNetwork(PTable Table,
  const TStr& SrcCol, const TStr& DstCol,
  TStrV& SrcAttrV, TStrV& DstAttrV, TStrV& EdgeAttrV,
  TAttrAggr AggrPolicy)
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
    if ((Table->Next)[CurrRowIdx] == Table->Invalid) {
      continue;
    }

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
		for (TInt i = 0; i < EdgeAttrV.Len(); i++) {
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

/// Calls ToNetwork with an empty attribute vector. Convenience wrapper.
template<class PGraph>
PGraph ToNetwork(PTable Table,
  const TStr& SrcCol, const TStr& DstCol, TAttrAggr AggrPolicy)
{
  TStrV V;
  return ToNetwork<PGraph>(Table, SrcCol, DstCol, V, AggrPolicy);
}

#ifdef GCC_ATOMIC
/// Performs table to graph conversion in parallel using the sort-first algorithm. This is the recommended method to use.
template<class PGraphMP>
PGraphMP ToGraphMP(PTable Table, const TStr& SrcCol, const TStr& DstCol) {
  // double start = omp_get_wtime();
  const TInt SrcColIdx = Table->GetColIdx(SrcCol);
  const TInt DstColIdx = Table->GetColIdx(DstCol);
  const TAttrType NodeType = Table->GetColType(SrcCol);
  Assert(NodeType == Table->GetColType(DstCol));

  const TInt NumRows = Table->NumValidRows;

  TIntV SrcCol1, DstCol1, SrcCol2, DstCol2;

  #pragma omp parallel sections num_threads(4)
  {
    #pragma omp section
    { SrcCol1.Reserve(NumRows, NumRows); }
    #pragma omp section
    { SrcCol2.Reserve(NumRows, NumRows); }
    #pragma omp section
    { DstCol1.Reserve(NumRows, NumRows); }
    #pragma omp section
    { DstCol2.Reserve(NumRows, NumRows); }
  }

  // double endResize = omp_get_wtime();
  // printf("Resize time = %f\n", endResize-start);

  TIntPrV Partitions;
  Table->GetPartitionRanges(Partitions, omp_get_max_threads());
  TInt PartitionSize = Partitions[0].GetVal2()-Partitions[0].GetVal1()+1;

  // double endPartition = omp_get_wtime();
  // printf("Partition time = %f\n", endPartition-endResize);

  omp_set_num_threads(omp_get_max_threads());
  if (NodeType == atInt) {
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < Partitions.Len(); i++) {
      TRowIterator RowI(Partitions[i].GetVal1(), Table());
      TRowIterator EndI(Partitions[i].GetVal2(), Table());
      while (RowI < EndI) {
        TInt RowId = RowI.GetRowIdx();
        SrcCol1[RowId] = RowI.GetIntAttr(SrcColIdx);
        SrcCol2[RowId] = RowI.GetIntAttr(SrcColIdx);
        DstCol1[RowId] = RowI.GetIntAttr(DstColIdx);
        DstCol2[RowId] = RowI.GetIntAttr(DstColIdx);
        RowI++;
      }
    }
  }
  else if (NodeType == atStr) {
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < Partitions.Len(); i++) {
      TRowIterator RowI(Partitions[i].GetVal1(), Table());
      TRowIterator EndI(Partitions[i].GetVal2(), Table());
      while (RowI < EndI) {
        TInt RowId = RowI.GetRowIdx();
        SrcCol1[RowId] = RowI.GetStrMapById(SrcColIdx);
        SrcCol2[RowId] = RowI.GetStrMapById(SrcColIdx);
        DstCol1[RowId] = RowI.GetStrMapById(DstColIdx);
        DstCol2[RowId] = RowI.GetStrMapById(DstColIdx);
        RowI++;
      }
    }
  }

  omp_set_num_threads(omp_get_max_threads());
  #pragma omp parallel
  {
    #pragma omp single nowait
    {
      #pragma omp task untied shared(SrcCol1, DstCol1)
      { TTable::QSortKeyVal(SrcCol1, DstCol1, 0, NumRows-1); }
    }
    #pragma omp single nowait
    {
      #pragma omp task untied shared(SrcCol2, DstCol2)
     { TTable::QSortKeyVal(DstCol2, SrcCol2, 0, NumRows-1); }
    }
    #pragma omp taskwait
  }

  // TTable::PSRSKeyVal(SrcCol1, DstCol1, 0, NumRows-1);
  // TTable::PSRSKeyVal(DstCol2, SrcCol2, 0, NumRows-1);

  // TInt IsS = TTable::CheckSortedKeyVal(SrcCol1, DstCol1, 0, NumRows-1);
  // TInt IsD = TTable::CheckSortedKeyVal(DstCol2, SrcCol2, 0, NumRows-1);
  // printf("IsSorted = %d %d\n", IsS.Val, IsD.Val);

  // double endSort = omp_get_wtime();
  // printf("Sort time = %f\n", endSort-endCopy);
  //return TNGraphMP::New(10, 100);

  TInt NumThreads = omp_get_max_threads();
  TInt PartSize = (NumRows/NumThreads);

  TIntV SrcOffsets, DstOffsets;
  SrcOffsets.Add(0);
  for (TInt i = 1; i < NumThreads; i++) {
    TInt CurrOffset = i * PartSize;
    while (CurrOffset < (i+1) * PartSize &&
          SrcCol1[CurrOffset-1] == SrcCol1[CurrOffset]) {
      CurrOffset++;
    }
    if (CurrOffset < (i+1) * PartSize) { SrcOffsets.Add(CurrOffset); }
  }
  SrcOffsets.Add(NumRows);

  DstOffsets.Add(0);
  for (TInt i = 1; i < NumThreads; i++) {
    TInt CurrOffset = i * PartSize;
    while (CurrOffset < (i+1) * PartSize &&
          DstCol2[CurrOffset-1] == DstCol2[CurrOffset]) {
      CurrOffset++;
    }
    if (CurrOffset < (i+1) * PartSize) { DstOffsets.Add(CurrOffset); }
  }
  DstOffsets.Add(NumRows);

  TInt SrcPartCnt = SrcOffsets.Len()-1;
  TInt DstPartCnt = DstOffsets.Len()-1;

  // for (TInt i = 0; i < SrcOffsets.Len(); i++) {
  //   printf("%d ", SrcOffsets[i].Val);
  // }
  // printf("\n");
  // for (TInt i = 0; i < DstOffsets.Len(); i++) {
  //   printf("%d ", DstOffsets[i].Val);
  // }
  // printf("\n");

  TIntV SrcNodeCounts, DstNodeCounts;
  SrcNodeCounts.Reserve(SrcPartCnt, SrcPartCnt);
  DstNodeCounts.Reserve(DstPartCnt, DstPartCnt);

  #pragma omp parallel for schedule(dynamic)
  for (int t = 0; t < SrcPartCnt+DstPartCnt; t++) {
    if (t < SrcPartCnt) {
      TInt i = t;
      if (SrcOffsets[i] != SrcOffsets[i+1]) {
        SrcNodeCounts[i] = 1;
        TInt CurrNode = SrcCol1[SrcOffsets[i]];
        for (TInt j = SrcOffsets[i]+1; j < SrcOffsets[i+1]; j++) {
          while (j < SrcOffsets[i+1] && SrcCol1[j] == CurrNode) { j++; }
          if (j < SrcOffsets[i+1]) {
            SrcNodeCounts[i]++;
            CurrNode = SrcCol1[j];
          }
        }
      }
    } else {
      TInt i = t - SrcPartCnt;
      if (DstOffsets[i] != DstOffsets[i+1]) {
        DstNodeCounts[i] = 1;
        TInt CurrNode = DstCol2[DstOffsets[i]];
        for (TInt j = DstOffsets[i]+1; j < DstOffsets[i+1]; j++) {
          while (j < DstOffsets[i+1] && DstCol2[j] == CurrNode) { j++; }
          if (j < DstOffsets[i+1]) {
            DstNodeCounts[i]++;
            CurrNode = DstCol2[j];
          }
        }
      }
    }
  }

  // for (TInt i = 0; i < SrcNodeCounts.Len(); i++) {
  //   printf("%d ", SrcNodeCounts[i].Val);
  // }
  // printf("\n");
  // for (TInt i = 0; i < DstNodeCounts.Len(); i++) {
  //   printf("%d ", DstNodeCounts[i].Val);
  // }
  // printf("\n");

  TInt TotalSrcNodes = 0;
  TIntV SrcIdOffsets;
  for (int i = 0; i < SrcPartCnt; i++) {
    SrcIdOffsets.Add(TotalSrcNodes);
    TotalSrcNodes += SrcNodeCounts[i];
  }

  TInt TotalDstNodes = 0;
  TIntV DstIdOffsets;
  for (int i = 0; i < DstPartCnt; i++) {
    DstIdOffsets.Add(TotalDstNodes);
    TotalDstNodes += DstNodeCounts[i];
  }

  // printf("Total Src = %d, Total Dst = %d\n", TotalSrcNodes.Val, TotalDstNodes.Val);

  TIntPrV SrcNodeIds, DstNodeIds;
  #pragma omp parallel sections
  {
    #pragma omp section
    { SrcNodeIds.Reserve(TotalSrcNodes, TotalSrcNodes); }
    #pragma omp section
    { DstNodeIds.Reserve(TotalDstNodes, TotalDstNodes); }
  }

  #pragma omp parallel for schedule(dynamic)
  for (int t = 0; t < SrcPartCnt+DstPartCnt; t++) {
    if (t < SrcPartCnt) {
      TInt i = t;
      if (SrcOffsets[i] != SrcOffsets[i+1]) {
        TInt CurrNode = SrcCol1[SrcOffsets[i]];
        TInt ThreadOffset = SrcIdOffsets[i];
        SrcNodeIds[ThreadOffset] = TIntPr(CurrNode, SrcOffsets[i]);
        TInt CurrCount = 1;
        for (TInt j = SrcOffsets[i]+1; j < SrcOffsets[i+1]; j++) {
          while (j < SrcOffsets[i+1] && SrcCol1[j] == CurrNode) { j++; }
          if (j < SrcOffsets[i+1]) {
            CurrNode = SrcCol1[j];
            SrcNodeIds[ThreadOffset+CurrCount] = TIntPr(CurrNode, j);
            CurrCount++;
          }
        }
      }
    } else {
      TInt i = t - SrcPartCnt;
      if (DstOffsets[i] != DstOffsets[i+1]) {
        TInt CurrNode = DstCol2[DstOffsets[i]];
        TInt ThreadOffset = DstIdOffsets[i];
        DstNodeIds[ThreadOffset] = TIntPr(CurrNode, DstOffsets[i]);
        TInt CurrCount = 1;
        for (TInt j = DstOffsets[i]+1; j < DstOffsets[i+1]; j++) {
          while (j < DstOffsets[i+1] && DstCol2[j] == CurrNode) { j++; }
          if (j < DstOffsets[i+1]) {
            CurrNode = DstCol2[j];
            DstNodeIds[ThreadOffset+CurrCount] = TIntPr(CurrNode, j);
            CurrCount++;
          }
        }
      }
    }
  }

  // double endNode = omp_get_wtime();
  // printf("Node time = %f\n", endNode-endSort);

  TIntTrV Nodes;
  Nodes.Reserve(TotalSrcNodes+TotalDstNodes);

  // double endNodeResize = omp_get_wtime();
  // printf("(NodeResize time = %f)\n", endNodeResize-endNode);

  TInt i = 0, j = 0;
  while (i < TotalSrcNodes && j < TotalDstNodes) {
    if (SrcNodeIds[i].Val1 == DstNodeIds[j].Val1) {
      Nodes.Add(TIntTr(SrcNodeIds[i].Val1, i, j));
      i++;
      j++;
    } else if (SrcNodeIds[i].Val1 < DstNodeIds[j].Val1) {
      Nodes.Add(TIntTr(SrcNodeIds[i].Val1, i, -1));
      i++;
    } else {
      Nodes.Add(TIntTr(DstNodeIds[j].Val1, -1, j));
      j++;
    }
  }
  for (; i < TotalSrcNodes; i++) { Nodes.Add(TIntTr(SrcNodeIds[i].Val1, i, -1)); }
  for (; j < TotalDstNodes; j++) { Nodes.Add(TIntTr(DstNodeIds[j].Val1, -1, j)); }

  // double endMerge = omp_get_wtime();
  // printf("Merge time = %f\n", endMerge-endNode);

  TInt NumNodes = Nodes.Len();
  // printf("NumNodes = %d\n", NumNodes.Val);

  PGraphMP Graph = TNGraphMP::New(NumNodes, NumRows);
  NumThreads = 1;
  int Delta = (NumNodes+NumThreads-1)/NumThreads;

  TVec<TIntV> InVV(NumNodes);
  TVec<TIntV> OutVV(NumNodes);

  omp_set_num_threads(NumThreads);
  #pragma omp parallel for schedule(static,Delta)
  for (int m = 0; m < NumNodes; m++) {
    //double startTr = omp_get_wtime();
    //TIntV OutV, InV;
    TInt n, i, j;
    Nodes[m].GetVal(n, i, j);
    if (i >= 0) {
      TInt Offset = SrcNodeIds[i].GetVal2();
      TInt Sz = DstCol1.Len()-Offset;
      if (i < SrcNodeIds.Len()-1) { Sz = SrcNodeIds[i+1].GetVal2()-Offset; }
      //printf("OutV: %d %d %d\n", n.Val, Offset.Val, Sz.Val);
      OutVV[m].Reserve(Sz);
    }
    if (j >= 0) {
      TInt Offset = DstNodeIds[j].GetVal2();
      TInt Sz = SrcCol2.Len()-Offset;
      if (j < DstNodeIds.Len()-1) { Sz = DstNodeIds[j+1].GetVal2()-Offset; }
      //printf("OutV: %d %d %d\n", n.Val, Offset.Val, Sz.Val);
      InVV[m].Reserve(Sz);
    }
    //double endTr = omp_get_wtime();
    //printf("Thread=%d, i=%d, t=%f\n", omp_get_thread_num(), m, endTr-startTr);
  }

  // double endAlloc = omp_get_wtime();
  // printf("Alloc time = %f\n", endAlloc-endMerge);

  NumThreads = omp_get_max_threads();
  Delta = (NumNodes+NumThreads-1)/(10*NumThreads);
  omp_set_num_threads(NumThreads);
  #pragma omp parallel for schedule(dynamic)
  for (int m = 0; m < NumNodes; m++) {
    //double startTr = omp_get_wtime();
    //TIntV OutV, InV;
    TInt n, i, j;
    Nodes[m].GetVal(n, i, j);
    if (i >= 0) {
      TInt Offset = SrcNodeIds[i].GetVal2();
      TInt Sz = DstCol1.Len()-Offset;
      if (i < SrcNodeIds.Len()-1) { Sz = SrcNodeIds[i+1].GetVal2()-Offset; }
      //printf("OutV: %d %d %d\n", n.Val, Offset.Val, Sz.Val);
      OutVV[m].CopyUniqueFrom(DstCol1, Offset, Sz);
    }
    if (j >= 0) {
      TInt Offset = DstNodeIds[j].GetVal2();
      TInt Sz = SrcCol2.Len()-Offset;
      if (j < DstNodeIds.Len()-1) { Sz = DstNodeIds[j+1].GetVal2()-Offset; }
      //printf("OutV: %d %d %d\n", n.Val, Offset.Val, Sz.Val);
      InVV[m].CopyUniqueFrom(SrcCol2, Offset, Sz);
    }
    Graph->AddNodeWithEdges(n, InVV[m], OutVV[m]);
    //double endTr = omp_get_wtime();
    //printf("Thread=%d, i=%d, t=%f\n", omp_get_thread_num(), m, endTr-startTr);
  }
  Graph->SetNodes(NumNodes);

  // double endAdd = omp_get_wtime();
  // printf("Add time = %f\n", endAdd-endAlloc);

  return Graph;
}

/// Performs table to graph conversion in parallel. Uses the hash-first method, which is less optimal, use ToGraphMP instead.
template<class PGraphMP>
PGraphMP ToGraphMP3(PTable Table, const TStr& SrcCol, const TStr& DstCol) {
  PNGraphMP Graph;
  int MaxThreads = omp_get_max_threads();
  int Length, Threads, Delta, Nodes, Last;
  uint64_t NumNodesEst;
  TInt SrcColIdx, DstColIdx;
  TIntV InVec, OutVec;

  SrcColIdx = Table->GetColIdx(SrcCol);
  DstColIdx = Table->GetColIdx(DstCol);
  const TAttrType NodeType = Table->GetColType(SrcCol);
  Assert(NodeType == Table->GetColType(DstCol));


  /* Estimate number of nodes in the graph */
  int NumRows = Table->Next.Len();
  double Load = 10;
  int sz = NumRows / Load;
  int *buckets = (int *)malloc(sz * sizeof(int));

  #pragma omp parallel for
    for (int i = 0; i < sz; i++)
      buckets[i] = 0;

  if (NodeType == atInt) {
    #pragma omp parallel for
      for (int i = 0; i < NumRows; i++) {
        int vert = Table->IntCols[DstColIdx][i];
        buckets[vert % sz] = 1;
      }
  }
  else if (NodeType == atStr ) {
    #pragma omp parallel for
      for (int i = 0; i < NumRows; i++) {
        int vert = (Table->StrColMaps)[DstColIdx][i];
        buckets[vert % sz] = 1;
      }
  }
  int cnt = 0;
  #pragma omp parallel for reduction(+:cnt)
    for (int i = 0; i < sz; i++) {
      if (buckets[i] == 0)
        cnt += 1;
  }

  NumNodesEst = sz * log ((double)sz / cnt);
  free (buckets);

  /* Until we correctly estimate the number of nodes */
  while (1)
  {
    Graph = TNGraphMP::New(NumNodesEst, 100);

    Length = Graph->Reserved();
    Threads = MaxThreads/2;
    Delta = (Length + Threads - 1) / Threads;

    OutVec.Gen(Length);
    InVec.Gen(Length);

    /* build the node hash table, count the size of edge lists */
    Last = NumRows;
    Nodes = 0;
    omp_set_num_threads(Threads);
    #pragma omp parallel for schedule(static, Delta)
      for (int CurrRowIdx = 0; CurrRowIdx < Last; CurrRowIdx++) {
        if ((uint64_t) Nodes + 1000 >= NumNodesEst) {
          /* need bigger hash table */
          continue;
        }

        TInt SVal, DVal;
        if (NodeType == atInt) {
          SVal = Table->IntCols[SrcColIdx][CurrRowIdx];
          DVal = Table->IntCols[DstColIdx][CurrRowIdx];
        }
        else if (NodeType == atStr ) {
          SVal = (Table->StrColMaps)[SrcColIdx][CurrRowIdx];
          DVal = (Table->StrColMaps)[DstColIdx][CurrRowIdx];
        }
        int SrcIdx = abs((SVal.GetPrimHashCd()) % Length);
        if (!Graph->AddOutEdge1(SrcIdx, SVal, DVal)) {
          #pragma omp critical
          {
                  Nodes++;
          }
        }
        __sync_fetch_and_add(&OutVec[SrcIdx].Val, 1);

        int DstIdx = abs((DVal.GetPrimHashCd()) % Length);
        if (!Graph->AddInEdge1(DstIdx, SVal, DVal)) {
          #pragma omp critical
          {
            Nodes++;
          }
        }
        __sync_fetch_and_add(&InVec[DstIdx].Val, 1);

      }
    if ((uint64_t) Nodes + 1000 >= NumNodesEst) {
      /* We need to double our num nodes estimate */
      Graph.Clr();
      InVec.Clr();
      OutVec.Clr();
      NumNodesEst *= 2;
    }
    else {
      break;
    }
  }

  Graph->SetNodes(Nodes);

  uint Edges = 0;
  for (int i = 0; i < Length; i++) {
    Edges += OutVec[i] + InVec[i];
  }

  for (int Idx = 0; Idx < Length; Idx++) {
    if (OutVec[Idx] > 0 || InVec[Idx] > 0) {
      Graph->ReserveNodeDegs(Idx, InVec[Idx], OutVec[Idx]);
    }
  }

  /* assign edges */
  Length = Graph->Reserved();
  Threads = MaxThreads;
  Delta = (Length + Threads - 1) / Threads;

  omp_set_num_threads(Threads);
  #pragma omp parallel for schedule(static,Delta)
  for (int CurrRowIdx = 0; CurrRowIdx < Last; CurrRowIdx++) {
	TInt SVal, DVal;
	if (NodeType == atInt) {
      SVal = Table->IntCols[SrcColIdx][CurrRowIdx];
      DVal = Table->IntCols[DstColIdx][CurrRowIdx];
	}
	else if (NodeType == atStr) {
      SVal = (Table->StrColMaps)[SrcColIdx][CurrRowIdx];
      DVal = (Table->StrColMaps)[DstColIdx][CurrRowIdx];
	}

    Graph->AddOutEdge2(SVal, DVal);
    Graph->AddInEdge2(SVal, DVal);
  }

  /* sort edges */
  Length = Graph->Reserved();
  Threads = MaxThreads*2;
  Delta = (Length + Threads - 1) / Threads;

  omp_set_num_threads(Threads);
  #pragma omp parallel for schedule(dynamic)
  for (int Idx = 0; Idx < Length; Idx++) {
    if (OutVec[Idx] > 0 || InVec[Idx] > 0) {
      Graph->SortEdges(Idx, InVec[Idx], OutVec[Idx]);
    }
  }

  return Graph;
}

/// Does Table to Network conversion in parallel using the sort-first algorithm. This is the recommended method to use.
template<class PGraphMP>
inline PGraphMP ToNetworkMP(PTable Table,
  const TStr& SrcCol, const TStr& DstCol,
  TStrV& SrcAttrV, TStrV& DstAttrV, TStrV& EdgeAttrV,
  TAttrAggr AggrPolicy) {
  TStopwatch* Sw = TStopwatch::GetInstance();

  Sw->Start(TStopwatch::AllocateColumnCopies);
  const TInt SrcColIdx = Table->GetColIdx(SrcCol);
  const TInt DstColIdx = Table->GetColIdx(DstCol);
  const TInt NumRows = Table->GetNumValidRows();

  const TAttrType NodeType = Table->GetColType(SrcCol);
  Assert(NodeType == Table->GetColType(DstCol));


  TIntV SrcCol1, EdgeCol1, EdgeCol2, DstCol2;

  THash<TInt, TStrIntVH> NodeIntAttrs;
  THash<TInt, TStrFltVH> NodeFltAttrs;
  THash<TInt, TStrStrVH> NodeStrAttrs;

  #pragma omp parallel sections num_threads(4)
  {
    #pragma omp section
    { SrcCol1.Reserve(NumRows, NumRows); }
    #pragma omp section
    { EdgeCol1.Reserve(NumRows, NumRows); }
    #pragma omp section
    { DstCol2.Reserve(NumRows, NumRows); }
    #pragma omp section
    { EdgeCol2.Reserve(NumRows, NumRows); }
  }
  Sw->Stop(TStopwatch::AllocateColumnCopies);

  Sw->Start(TStopwatch::CopyColumns);
  TIntPrV Partitions;
  Table->GetPartitionRanges(Partitions, omp_get_max_threads());
  TInt PartitionSize = Partitions[0].GetVal2()-Partitions[0].GetVal1()+1;

  // double endPartition = omp_get_wtime();
  // printf("Partition time = %f\n", endPartition-endResize);

  omp_set_num_threads(omp_get_max_threads());
  if (NodeType == atInt) {
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < Partitions.Len(); i++) {
      TRowIterator RowI(Partitions[i].GetVal1(), Table());
      TRowIterator EndI(Partitions[i].GetVal2(), Table());
      while (RowI < EndI) {
        TInt RowId = RowI.GetRowIdx();
        SrcCol1[RowId] = RowI.GetIntAttr(SrcColIdx);
        EdgeCol1[RowId] = RowId;
        DstCol2[RowId] = RowI.GetIntAttr(DstColIdx);
        EdgeCol2[RowId] = RowId;
        RowI++;
      }
    }
  }
  else if (NodeType == atStr) {
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < Partitions.Len(); i++) {
      TRowIterator RowI(Partitions[i].GetVal1(), Table());
      TRowIterator EndI(Partitions[i].GetVal2(), Table());
      while (RowI < EndI) {
        TInt RowId = RowI.GetRowIdx();
        SrcCol1[RowId] = RowI.GetStrMapById(SrcColIdx);
        EdgeCol1[RowId] = RowId;
        DstCol2[RowId] = RowI.GetStrMapById(DstColIdx);
        EdgeCol2[RowId] = RowId;
        RowI++;
      }
    }
  }
  Sw->Stop(TStopwatch::CopyColumns);

  Sw->Start(TStopwatch::Sort);
  omp_set_num_threads(omp_get_max_threads());
  #pragma omp parallel
  {
    #pragma omp single nowait
    {
      #ifndef GLib_WIN32
      #pragma omp task untied shared(SrcCol1, EdgeCol1)
      #endif
      { TTable::QSortKeyVal(SrcCol1, EdgeCol1, 0, NumRows-1); }
    }
    #pragma omp single nowait
    {
      #ifndef GLib_WIN32
      #pragma omp task untied shared(EdgeCol2, DstCol2)
      #endif
     { TTable::QSortKeyVal(DstCol2, EdgeCol2, 0, NumRows-1); }
    }
    #ifndef GLib_WIN32
    #pragma omp taskwait
    #endif
  }
  Sw->Stop(TStopwatch::Sort);

  Sw->Start(TStopwatch::Group);
  TInt NumThreads = omp_get_max_threads();
  TInt PartSize = (NumRows/NumThreads);

  // Find the offset of all partitions, each of which contains a list of rows.
  // Nodes from same sources or destinations are ensured to be kept within same partition.
  TIntV SrcOffsets, DstOffsets;
  SrcOffsets.Add(0);
  for (TInt i = 1; i < NumThreads; i++) {
    TInt CurrOffset = i * PartSize;
    while (CurrOffset < (i+1) * PartSize &&
          SrcCol1[CurrOffset-1] == SrcCol1[CurrOffset]) {
      // ensure that rows from the same sources are grouped together
      CurrOffset++;
    }
    if (CurrOffset < (i+1) * PartSize) { SrcOffsets.Add(CurrOffset); }
  }
  SrcOffsets.Add(NumRows);

  DstOffsets.Add(0);
  for (TInt i = 1; i < NumThreads; i++) {
    TInt CurrOffset = i * PartSize;
    while (CurrOffset < (i+1) * PartSize &&
          DstCol2[CurrOffset-1] == DstCol2[CurrOffset]) {
      // ensure that rows to the same destinations are grouped together
      CurrOffset++;
    }
    if (CurrOffset < (i+1) * PartSize) { DstOffsets.Add(CurrOffset); }
  }
  DstOffsets.Add(NumRows);

  TInt SrcPartCnt = SrcOffsets.Len()-1; // number of partitions
  TInt DstPartCnt = DstOffsets.Len()-1; // number of partitions

  // count the number of source nodes and destination nodes in each partition
  TIntV SrcNodeCounts, DstNodeCounts;
  SrcNodeCounts.Reserve(SrcPartCnt, SrcPartCnt);
  DstNodeCounts.Reserve(DstPartCnt, DstPartCnt);

  #pragma omp parallel for schedule(dynamic)
  for (int t = 0; t < SrcPartCnt+DstPartCnt; t++) {
    if (t < SrcPartCnt) {
      TInt i = t;
      if (SrcOffsets[i] != SrcOffsets[i+1]) {
        SrcNodeCounts[i] = 1;
        TInt CurrNode = SrcCol1[SrcOffsets[i]];
        for (TInt j = SrcOffsets[i]+1; j < SrcOffsets[i+1]; j++) {
          while (j < SrcOffsets[i+1] && SrcCol1[j] == CurrNode) { j++; }
          if (j < SrcOffsets[i+1]) {
            SrcNodeCounts[i]++;
            CurrNode = SrcCol1[j];
          }
        }
      }
    } else {
      TInt i = t - SrcPartCnt;
      if (DstOffsets[i] != DstOffsets[i+1]) {
        DstNodeCounts[i] = 1;
        TInt CurrNode = DstCol2[DstOffsets[i]];
        for (TInt j = DstOffsets[i]+1; j < DstOffsets[i+1]; j++) {
          while (j < DstOffsets[i+1] && DstCol2[j] == CurrNode) { j++; }
          if (j < DstOffsets[i+1]) {
            DstNodeCounts[i]++;
            CurrNode = DstCol2[j];
          }
        }
      }
    }
  }

  TInt TotalSrcNodes = 0;
  TIntV SrcIdOffsets;
  for (int i = 0; i < SrcPartCnt; i++) {
    SrcIdOffsets.Add(TotalSrcNodes);
    TotalSrcNodes += SrcNodeCounts[i];
  }

  TInt TotalDstNodes = 0;
  TIntV DstIdOffsets;
  for (int i = 0; i < DstPartCnt; i++) {
    DstIdOffsets.Add(TotalDstNodes);
    TotalDstNodes += DstNodeCounts[i];
  }

  // printf("Total Src = %d, Total Dst = %d\n", TotalSrcNodes.Val, TotalDstNodes.Val);

  // find vector of (node_id, start_offset) where start_offset is the index of the first row with node_id
  TIntPrV SrcNodeIds, DstNodeIds;
  #pragma omp parallel sections
  {
    #pragma omp section
    { SrcNodeIds.Reserve(TotalSrcNodes, TotalSrcNodes); }
    #pragma omp section
    { DstNodeIds.Reserve(TotalDstNodes, TotalDstNodes); }
  }

  // Find the starting offset of each node (in both src and dst)
  #pragma omp parallel for schedule(dynamic)
  for (int t = 0; t < SrcPartCnt+DstPartCnt; t++) {
    if (t < SrcPartCnt) {
      TInt i = t;
      if (SrcOffsets[i] != SrcOffsets[i+1]) {
        TInt CurrNode = SrcCol1[SrcOffsets[i]];
        TInt ThreadOffset = SrcIdOffsets[i];
        SrcNodeIds[ThreadOffset] = TIntPr(CurrNode, SrcOffsets[i]);
        TInt CurrCount = 1;
        for (TInt j = SrcOffsets[i]+1; j < SrcOffsets[i+1]; j++) {
          while (j < SrcOffsets[i+1] && SrcCol1[j] == CurrNode) { j++; }
          if (j < SrcOffsets[i+1]) {
            CurrNode = SrcCol1[j];
            SrcNodeIds[ThreadOffset+CurrCount] = TIntPr(CurrNode, j);
            CurrCount++;
          }
        }
      }
    } else {
      TInt i = t - SrcPartCnt;
      if (DstOffsets[i] != DstOffsets[i+1]) {
        TInt CurrNode = DstCol2[DstOffsets[i]];
        TInt ThreadOffset = DstIdOffsets[i];
        DstNodeIds[ThreadOffset] = TIntPr(CurrNode, DstOffsets[i]);
        TInt CurrCount = 1;
        for (TInt j = DstOffsets[i]+1; j < DstOffsets[i+1]; j++) {
          while (j < DstOffsets[i+1] && DstCol2[j] == CurrNode) { j++; }
          if (j < DstOffsets[i+1]) {
            CurrNode = DstCol2[j];
            DstNodeIds[ThreadOffset+CurrCount] = TIntPr(CurrNode, j);
            CurrCount++;
          }
        }
      }
    }
  }
  Sw->Stop(TStopwatch::Group);

  Sw->Start(TStopwatch::MergeNeighborhoods);
  // Find the combined neighborhood (both out-neighbors and in-neighbors) of each node
  TIntTrV Nodes;
  Nodes.Reserve(TotalSrcNodes+TotalDstNodes);

  TInt i = 0, j = 0;
  while (i < TotalSrcNodes && j < TotalDstNodes) {
    if (SrcNodeIds[i].Val1 == DstNodeIds[j].Val1) {
      Nodes.Add(TIntTr(SrcNodeIds[i].Val1, i, j));
      i++;
      j++;
    } else if (SrcNodeIds[i].Val1 < DstNodeIds[j].Val1) {
      Nodes.Add(TIntTr(SrcNodeIds[i].Val1, i, -1));
      i++;
    } else {
      Nodes.Add(TIntTr(DstNodeIds[j].Val1, -1, j));
      j++;
    }
  }
  for (; i < TotalSrcNodes; i++) { Nodes.Add(TIntTr(SrcNodeIds[i].Val1, i, -1)); }
  for (; j < TotalDstNodes; j++) { Nodes.Add(TIntTr(DstNodeIds[j].Val1, -1, j)); }
  Sw->Stop(TStopwatch::MergeNeighborhoods);

  Sw->Start(TStopwatch::AddNeighborhoods);
  TInt NumNodes = Nodes.Len();
  PGraphMP Graph = PGraphMP::TObj::New(NumNodes, NumRows);
//  NumThreads = omp_get_max_threads();
//  int Delta = (NumNodes+NumThreads-1)/NumThreads;

  TVec<TIntV> InVV(NumNodes);
  TVec<TIntV> OutVV(NumNodes);

//  omp_set_num_threads(NumThreads);
  #pragma omp parallel for schedule(static,100)
  for (int m = 0; m < NumNodes; m++) {
    //double startTr = omp_get_wtime();
    //TIntV OutV, InV;
    TInt n, i, j;
    Nodes[m].GetVal(n, i, j);
    if (i >= 0) {
      TInt Offset = SrcNodeIds[i].GetVal2();
      TInt Sz = EdgeCol1.Len()-Offset;
      if (i < SrcNodeIds.Len()-1) { Sz = SrcNodeIds[i+1].GetVal2()-Offset; }
      OutVV[m].Reserve(Sz);
      OutVV[m].CopyUniqueFrom(EdgeCol1, Offset, Sz);
    }
    if (j >= 0) {
      TInt Offset = DstNodeIds[j].GetVal2();
      TInt Sz = EdgeCol2.Len()-Offset;
      if (j < DstNodeIds.Len()-1) { Sz = DstNodeIds[j+1].GetVal2()-Offset; }
      InVV[m].Reserve(Sz);
      InVV[m].CopyUniqueFrom(EdgeCol2, Offset, Sz);
    }
    Graph->AddNodeWithEdges(n, InVV[m], OutVV[m]);
  }
  Graph->SetNodes(NumNodes);
  Sw->Stop(TStopwatch::AddNeighborhoods);

  Sw->Start(TStopwatch::AddEdges);
  omp_set_num_threads(omp_get_max_threads());
  if (NodeType == atInt) {
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < Partitions.Len(); i++) {
      TRowIterator RowI(Partitions[i].GetVal1(), Table());
      TRowIterator EndI(Partitions[i].GetVal2(), Table());
      while (RowI < EndI) {
        TInt RowId = RowI.GetRowIdx(); // EdgeId
        TInt SrcId = RowI.GetIntAttr(SrcColIdx);
        TInt DstId = RowI.GetIntAttr(DstColIdx);
        Graph->AddEdgeUnchecked(RowId, SrcId, DstId);
        RowI++;
		for (TInt ea_i = 0; ea_i < EdgeAttrV.Len(); ea_i++) {
          TStr ColName = EdgeAttrV[ea_i];
          TAttrType T = Table->GetColType(ColName);
          TInt Index = Table->GetColIdx(ColName);
          switch (T) {
            case atInt:
              Graph->AddIntAttrDatE(RowId, Table->IntCols[Index][RowId], ColName);
              break;
            case atFlt:
              Graph->AddFltAttrDatE(RowId, Table->FltCols[Index][RowId], ColName);
              break;
            case atStr:
              Graph->AddStrAttrDatE(RowId, Table->GetStrVal(Index, RowId), ColName);
              break;
          }
        }
        if ((Table->SrcNodeAttrV).Len() > 0) {
          Table->AddNodeAttributes(SrcId, Table->SrcNodeAttrV, RowId, NodeIntAttrs, NodeFltAttrs, NodeStrAttrs);
       	}

        if ((Table->DstNodeAttrV).Len() > 0) {
         Table->AddNodeAttributes(SrcId, Table->DstNodeAttrV, RowId, NodeIntAttrs, NodeFltAttrs, NodeStrAttrs);
        }
      }
    }
  }
  else if (NodeType == atStr) {
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < Partitions.Len(); i++) {
      TRowIterator RowI(Partitions[i].GetVal1(), Table());
      TRowIterator EndI(Partitions[i].GetVal2(), Table());
      while (RowI < EndI) {
        TInt RowId = RowI.GetRowIdx(); // EdgeId
        TInt SrcId = RowI.GetStrMapById(SrcColIdx);
        TInt DstId = RowI.GetStrMapById(DstColIdx);
        Graph->AddEdgeUnchecked(RowId, SrcId, DstId);
        RowI++;
		for (TInt ea_i = 0; ea_i < EdgeAttrV.Len(); ea_i++) {
          TStr ColName = EdgeAttrV[ea_i];
          TAttrType T = Table->GetColType(ColName);
          TInt Index = Table->GetColIdx(ColName);
          switch (T) {
            case atInt:
              Graph->AddIntAttrDatE(RowId, Table->IntCols[Index][RowId], ColName);
              break;
            case atFlt:
              Graph->AddFltAttrDatE(RowId, Table->FltCols[Index][RowId], ColName);
              break;
            case atStr:
              Graph->AddStrAttrDatE(RowId, Table->GetStrVal(Index, RowId), ColName);
              break;
          }
        }
        if ((Table->SrcNodeAttrV).Len() > 0) {
          Table->AddNodeAttributes(SrcId, Table->SrcNodeAttrV, RowId, NodeIntAttrs, NodeFltAttrs, NodeStrAttrs);
       	}

        if ((Table->DstNodeAttrV).Len() > 0) {
         Table->AddNodeAttributes(SrcId, Table->DstNodeAttrV, RowId, NodeIntAttrs, NodeFltAttrs, NodeStrAttrs);
        }

      }
    }

  }

  // aggregate node attributes and add to graph
  if ((Table->SrcNodeAttrV).Len() > 0 || (Table->DstNodeAttrV).Len() > 0) {
    for (typename PGraphMP::TObj::TNodeI NodeI = Graph->BegNI(); NodeI < Graph->EndNI(); NodeI++) {
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


  Graph->SetEdges(NumRows);
  Sw->Stop(TStopwatch::AddEdges);

  // double endAdd = omp_get_wtime();
  // printf("Add time = %f\n", endAdd-endAlloc);

  return Graph;
}

/// Calls ToNetworkMP with empty attribute vector. Convenience wrapper.
template<class PGraphMP>
PGraphMP ToNetworkMP(PTable Table,
  const TStr& SrcCol, const TStr& DstCol, TAttrAggr AggrPolicy)
{
  TStrV V;
  return ToNetworkMP<PGraphMP>(Table, SrcCol, DstCol, V,AggrPolicy);
}



///Implements table to network conversion in parallel. Not the recommended algorithm, using ToNetworkMP instead.
template<class PGraphMP>
inline PGraphMP ToNetworkMP2(PTable Table,
  const TStr& SrcCol, const TStr& DstCol,
  TStrV& SrcAttrV, TStrV& DstAttrV, TStrV& EdgeAttrV,
  TAttrAggr AggrPolicy) {
  TStopwatch* Sw = TStopwatch::GetInstance();

  Sw->Start(TStopwatch::AllocateColumnCopies);
  const TInt SrcColIdx = Table->GetColIdx(SrcCol);
  const TInt DstColIdx = Table->GetColIdx(DstCol);
  const TInt NumRows = Table->NumValidRows;

  const TAttrType NodeType = Table->GetColType(SrcCol);
  Assert(NodeType == Table->GetColType(DstCol));



  TIntV SrcCol1, EdgeCol1, EdgeCol2, DstCol2;

  #pragma omp parallel sections num_threads(4)
  {
    #pragma omp section
    { SrcCol1.Reserve(NumRows, NumRows); }
    #pragma omp section
    { EdgeCol1.Reserve(NumRows, NumRows); }
    #pragma omp section
    { DstCol2.Reserve(NumRows, NumRows); }
    #pragma omp section
    { EdgeCol2.Reserve(NumRows, NumRows); }
  }
  Sw->Stop(TStopwatch::AllocateColumnCopies);
  Sw->Start(TStopwatch::CopyColumns);
  TIntPrV Partitions;
//  int NThreads = omp_get_max_threads();
  const int NThreads = 40;
  Table->GetPartitionRanges(Partitions, NThreads);
  TInt PartitionSize = Partitions[0].GetVal2()-Partitions[0].GetVal1()+1;

  // double endPartition = omp_get_wtime();
  // printf("Partition time = %f\n", endPartition-endResize);

  if (NodeType == atInt) {
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < Partitions.Len(); i++) {
      TRowIterator RowI(Partitions[i].GetVal1(), Table());
      TRowIterator EndI(Partitions[i].GetVal2(), Table());
      while (RowI < EndI) {
        TInt RowId = RowI.GetRowIdx();
        SrcCol1[RowId] = RowI.GetIntAttr(SrcColIdx);
        EdgeCol1[RowId] = RowId;
        DstCol2[RowId] = RowI.GetIntAttr(DstColIdx);
        EdgeCol2[RowId] = RowId;
        RowI++;
      }
    }
  }
  else if (NodeType == atStr) {
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < Partitions.Len(); i++) {
      TRowIterator RowI(Partitions[i].GetVal1(), Table());
      TRowIterator EndI(Partitions[i].GetVal2(), Table());
      while (RowI < EndI) {
        TInt RowId = RowI.GetRowIdx();
        SrcCol1[RowId] = RowI.GetStrMapById(SrcColIdx);
        EdgeCol1[RowId] = RowId;
        DstCol2[RowId] = RowI.GetStrMapById(DstColIdx);
        EdgeCol2[RowId] = RowId;
        RowI++;
      }
    }

  }

//  printf("NumRows = %d\n", NumRows.Val);
//  printf("NThreads = %d\n", NThreads);
//  for (int i = 0; i < Partitions.Len(); i++) {
//    printf("Partition %d %d->%d\n", i, Partitions[i].GetVal1().Val, Partitions[i].GetVal2().Val);
//  }
  int Parts[NThreads+1];
  for (int i = 0; i < NThreads; i++) {
    Parts[i] = NumRows.Val / NThreads * i;
  }
  Parts[NThreads] = NumRows;
//  for (int i = 0; i < NThreads+1; i++) {
//    printf("Parts[%d] = %d\n", i, Parts[i]);
//  }
  Sw->Stop(TStopwatch::CopyColumns);

  Sw->Start(TStopwatch::Sort);
  TInt ExtremePoints[4][NThreads];
  omp_set_num_threads(omp_get_max_threads());
  #pragma omp parallel
  {
    #pragma omp for schedule(static) nowait
    for (int i = 0; i < NThreads; i++) {
      TInt StartPos = Parts[i];
      TInt EndPos = Parts[i+1]-1;
      // TODO: Handle empty partition
      TTable::QSortKeyVal(SrcCol1, EdgeCol1, StartPos, EndPos);
      ExtremePoints[0][i] = SrcCol1[StartPos];
      ExtremePoints[2][i] = SrcCol1[EndPos];
    }
    #pragma omp for schedule(static) nowait
    for (int i = 0; i < NThreads; i++) {
      TInt StartPos = Parts[i];
      TInt EndPos = Parts[i+1]-1;
      // TODO: Handle empty partition
      TTable::QSortKeyVal(DstCol2, EdgeCol2, StartPos, EndPos);
      ExtremePoints[1][i] = DstCol2[StartPos];
      ExtremePoints[3][i] = DstCol2[EndPos];
    }
  }
//  for (int i = 0; i < NThreads; i++) {
//    printf("ExtremePoints[%d] = %d-%d -> %d-%d\n", i, ExtremePoints[0][i].Val, ExtremePoints[1][i].Val, ExtremePoints[2][i].Val, ExtremePoints[3][i].Val);
//  }

  // find min points
  TInt MinId(INT_MAX);
  for (int j = 0; j < 2; j++) {
    for (int i = 0; i < NThreads; i++) {
      if (MinId > ExtremePoints[j][i]) { MinId = ExtremePoints[j][i]; }
    }
  }
  TInt MaxId(-1);
  for (int j = 2; j < 4; j++) {
    for (int i = 0; i < NThreads; i++) {
      if (MaxId < ExtremePoints[j][i]) { MaxId = ExtremePoints[j][i]; }
    }
  }
//  printf("MinId = %d\n", MinId.Val);
//  printf("MaxId = %d\n", MaxId.Val);
  Sw->Stop(TStopwatch::Sort);

  Sw->Start(TStopwatch::Group);
//  const int NumCollectors = omp_get_max_threads();
  const int NumCollectors = 20;
  int Range = MaxId.Val - MinId.Val;
  TIntV IdRanges(NumCollectors+1);
  for (int j = 0; j < NumCollectors; j++) {
    IdRanges[j] = MinId + Range/NumCollectors*j;
  }
  IdRanges[NumCollectors] = MaxId+1;
//  for (int i = 0; i < NumCollectors+1; i++) {
//    printf("IdRanges[%d] = %d\n", i, IdRanges[i].Val);
//  }

  int SrcOffsets[NThreads][NumCollectors+1];
  #pragma omp parallel for schedule(static)
  for (int i = 0; i < NThreads; i++) {
    int CollectorId = 0;
    for (int j = Parts[i]; j < Parts[i+1]; j++) {
      while (SrcCol1[j] >= IdRanges[CollectorId]) {
        SrcOffsets[i][CollectorId++] = j;
      }
    }
    while (CollectorId <= NumCollectors) {
      SrcOffsets[i][CollectorId++] = Parts[i+1];
    }
  }
  int DstOffsets[NThreads][NumCollectors+1];
  #pragma omp parallel for schedule(static)
  for (int i = 0; i < NThreads; i++) {
    int CollectorId = 0;
    for (int j = Parts[i]; j < Parts[i+1]; j++) {
      while (DstCol2[j] >= IdRanges[CollectorId]) {
        DstOffsets[i][CollectorId++] = j;
      }
    }
    while (CollectorId <= NumCollectors) {
      DstOffsets[i][CollectorId++] = Parts[i+1];
    }
  }
//  for (int i = 0; i < NThreads; i++) {
//    for (int j = 0; j < NumCollectors+1; j++) {
//      printf("SrcOffsets[%d][%d] = %d\n", i, j, SrcOffsets[i][j]);
//    }
//  }
//  for (int i = 0; i < NThreads; i++) {
//    for (int j = 0; j < NumCollectors+1; j++) {
//      printf("DstOffsets[%d][%d] = %d\n", i, j, DstOffsets[i][j]);
//    }
//  }

  TIntV SrcCollectorOffsets(NumCollectors+1);
  SrcCollectorOffsets[0] = 0;
  for (int k = 0; k < NumCollectors; k++) {
    int SumOffset = 0;
    for (int i = 0; i < NThreads; i++) {
      SumOffset += SrcOffsets[i][k+1] - SrcOffsets[i][k];
    }
    SrcCollectorOffsets[k+1] = SrcCollectorOffsets[k] + SumOffset;
  }
  TIntV DstCollectorOffsets(NumCollectors+1);
  DstCollectorOffsets[0] = 0;
  for (int k = 0; k < NumCollectors; k++) {
    int SumOffset = 0;
    for (int i = 0; i < NThreads; i++) {
      SumOffset += DstOffsets[i][k+1] - DstOffsets[i][k];
    }
    DstCollectorOffsets[k+1] = DstCollectorOffsets[k] + SumOffset;
  }
//  for (int i = 0; i < NumCollectors+1; i++) {
//    printf("SrcCollectorOffsets[%d] = %d\n", i, SrcCollectorOffsets[i].Val);
//  }
//  for (int i = 0; i < NumCollectors+1; i++) {
//    printf("DstCollectorOffsets[%d] = %d\n", i, DstCollectorOffsets[i].Val);
//  }

  TIntV SrcCol3, EdgeCol3, EdgeCol4, DstCol4;
  #pragma omp parallel sections num_threads(4)
  {
    #pragma omp section
    { SrcCol3.Reserve(NumRows, NumRows); }
    #pragma omp section
    { EdgeCol3.Reserve(NumRows, NumRows); }
    #pragma omp section
    { DstCol4.Reserve(NumRows, NumRows); }
    #pragma omp section
    { EdgeCol4.Reserve(NumRows, NumRows); }
  }

  TIntV SrcNodeCounts(NumCollectors), DstNodeCounts(NumCollectors);
  #pragma omp parallel for schedule(static)
  for (int k = 0; k < NumCollectors; k++) {
    int ind = SrcCollectorOffsets[k];
    for (int i = 0; i < NThreads; i++) {
      for (int j = SrcOffsets[i][k]; j < SrcOffsets[i][k+1]; j++) {
        SrcCol3[ind] = SrcCol1[j];
        EdgeCol3[ind] = EdgeCol1[j];
        ind++;
      }
    }
    TTable::QSortKeyVal(SrcCol3, EdgeCol3, SrcCollectorOffsets[k], SrcCollectorOffsets[k+1]-1);
    int SrcCount = 0;
    if (SrcCollectorOffsets[k+1] > SrcCollectorOffsets[k]) {
      SrcCount = 1;
      for (int j = SrcCollectorOffsets[k]+1; j < SrcCollectorOffsets[k+1]; j++) {
        if (SrcCol3[j] != SrcCol3[j-1]) { SrcCount++; }
      }
    }
    SrcNodeCounts[k] = SrcCount;

    ind = DstCollectorOffsets[k];
    for (int i = 0; i < NThreads; i++) {
      for (int j = DstOffsets[i][k]; j < DstOffsets[i][k+1]; j++) {
        DstCol4[ind] = DstCol2[j];
        EdgeCol4[ind] = EdgeCol2[j];
        ind++;
      }
    }
    TTable::QSortKeyVal(DstCol4, EdgeCol4, DstCollectorOffsets[k], DstCollectorOffsets[k+1]-1);
    int DstCount = 0;
    if (DstCollectorOffsets[k+1] > DstCollectorOffsets[k]) {
      DstCount = 1;
      for (int j = DstCollectorOffsets[k]+1; j < DstCollectorOffsets[k+1]; j++) {
        if (DstCol4[j] != DstCol4[j-1]) { DstCount++; }
      }
    }
    DstNodeCounts[k] = DstCount;
  }

  TInt TotalSrcNodes = 0;
  TIntV SrcIdOffsets;
  for (int i = 0; i < NumCollectors; i++) {
    SrcIdOffsets.Add(TotalSrcNodes);
    TotalSrcNodes += SrcNodeCounts[i];
  }

//  printf("Sorted = %d - %d\n", SrcCol3.IsSorted(), DstCol4.IsSorted());
//  for (int i = 0; i < NumRows-1; i++) {
//    if (SrcCol3[i] > SrcCol3[i+1]) { printf("i=%d: %d %d\n", i, SrcCol3[i].Val, SrcCol3[i+1].Val); }
//  }
//  for (int i = 0; i < NumRows-1; i++) {
//    if (DstCol4[i] > DstCol4[i+1]) { printf("i=%d: %d %d\n", i, DstCol4[i].Val, DstCol4[i+1].Val); }
//  }

  TInt TotalDstNodes = 0;
  TIntV DstIdOffsets;
  for (int i = 0; i < NumCollectors; i++) {
    DstIdOffsets.Add(TotalDstNodes);
    TotalDstNodes += DstNodeCounts[i];
  }

  // find vector of (node_id, start_offset) where start_offset is the index of the first row with node_id
  TIntPrV SrcNodeIds, DstNodeIds;
  #pragma omp parallel sections
  {
    #pragma omp section
    { SrcNodeIds.Reserve(TotalSrcNodes, TotalSrcNodes); }
    #pragma omp section
    { DstNodeIds.Reserve(TotalDstNodes, TotalDstNodes); }
  }

  // Find the starting offset of each node (in both src and dst)
  #pragma omp parallel for schedule(dynamic)
  for (int t = 0; t < 2*NumCollectors; t++) {
    if (t < NumCollectors) {
      TInt i = t;
      if (SrcCollectorOffsets[i] < SrcCollectorOffsets[i+1]) {
        TInt CurrNode = SrcCol3[SrcCollectorOffsets[i]];
        TInt ThreadOffset = SrcIdOffsets[i];
        SrcNodeIds[ThreadOffset] = TIntPr(CurrNode, SrcCollectorOffsets[i]);
        TInt CurrCount = 1;
        for (TInt j = SrcCollectorOffsets[i]+1; j < SrcCollectorOffsets[i+1]; j++) {
          while (j < SrcCollectorOffsets[i+1] && SrcCol3[j] == CurrNode) { j++; }
          if (j < SrcCollectorOffsets[i+1]) {
            CurrNode = SrcCol3[j];
            SrcNodeIds[ThreadOffset+CurrCount] = TIntPr(CurrNode, j);
            CurrCount++;
          }
        }
      }
    } else {
      TInt i = t - NumCollectors;
      if (DstCollectorOffsets[i] < DstCollectorOffsets[i+1]) {
        TInt CurrNode = DstCol4[DstCollectorOffsets[i]];
        TInt ThreadOffset = DstIdOffsets[i];
        DstNodeIds[ThreadOffset] = TIntPr(CurrNode, DstCollectorOffsets[i]);
        TInt CurrCount = 1;
        for (TInt j = DstCollectorOffsets[i]+1; j < DstCollectorOffsets[i+1]; j++) {
          while (j < DstCollectorOffsets[i+1] && DstCol4[j] == CurrNode) { j++; }
          if (j < DstCollectorOffsets[i+1]) {
            CurrNode = DstCol4[j];
            DstNodeIds[ThreadOffset+CurrCount] = TIntPr(CurrNode, j);
            CurrCount++;
          }
        }
      }
    }
  }
  Sw->Stop(TStopwatch::Group);

  Sw->Start(TStopwatch::MergeNeighborhoods);
  // Find the combined neighborhood (both out-neighbors and in-neighbors) of each node
  TIntTrV Nodes;
  Nodes.Reserve(TotalSrcNodes+TotalDstNodes);

  TInt i = 0, j = 0;
  while (i < TotalSrcNodes && j < TotalDstNodes) {
    if (SrcNodeIds[i].Val1 == DstNodeIds[j].Val1) {
      Nodes.Add(TIntTr(SrcNodeIds[i].Val1, i, j));
      i++;
      j++;
    } else if (SrcNodeIds[i].Val1 < DstNodeIds[j].Val1) {
      Nodes.Add(TIntTr(SrcNodeIds[i].Val1, i, -1));
      i++;
    } else {
      Nodes.Add(TIntTr(DstNodeIds[j].Val1, -1, j));
      j++;
    }
  }
  for (; i < TotalSrcNodes; i++) { Nodes.Add(TIntTr(SrcNodeIds[i].Val1, i, -1)); }
  for (; j < TotalDstNodes; j++) { Nodes.Add(TIntTr(DstNodeIds[j].Val1, -1, j)); }
  Sw->Stop(TStopwatch::MergeNeighborhoods);

  Sw->Start(TStopwatch::AddNeighborhoods);
  TInt NumNodes = Nodes.Len();
  PGraphMP Graph = PGraphMP::TObj::New(NumNodes, NumRows);
//  NumThreads = omp_get_max_threads();
//  int Delta = (NumNodes+NumThreads-1)/NumThreads;

  TVec<TIntV> InVV(NumNodes);
  TVec<TIntV> OutVV(NumNodes);

//  omp_set_num_threads(NumThreads);
  #pragma omp parallel for schedule(static,100)
  for (int m = 0; m < NumNodes; m++) {
    //double startTr = omp_get_wtime();
    //TIntV OutV, InV;
    TInt n, i, j;
    Nodes[m].GetVal(n, i, j);
    if (i >= 0) {
      TInt Offset = SrcNodeIds[i].GetVal2();
      TInt Sz = EdgeCol3.Len()-Offset;
      if (i < SrcNodeIds.Len()-1) { Sz = SrcNodeIds[i+1].GetVal2()-Offset; }
      OutVV[m].Reserve(Sz);
      OutVV[m].CopyUniqueFrom(EdgeCol3, Offset, Sz);
    }
    if (j >= 0) {
      TInt Offset = DstNodeIds[j].GetVal2();
      TInt Sz = EdgeCol4.Len()-Offset;
      if (j < DstNodeIds.Len()-1) { Sz = DstNodeIds[j+1].GetVal2()-Offset; }
      InVV[m].Reserve(Sz);
      InVV[m].CopyUniqueFrom(EdgeCol4, Offset, Sz);
    }
    Graph->AddNodeWithEdges(n, InVV[m], OutVV[m]);
  }
  Graph->SetNodes(NumNodes);
  Sw->Stop(TStopwatch::AddNeighborhoods);

  Sw->Start(TStopwatch::AddEdges);
  omp_set_num_threads(omp_get_max_threads());
  if (NodeType == atInt) {
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < Partitions.Len(); i++) {
      TRowIterator RowI(Partitions[i].GetVal1(), Table());
      TRowIterator EndI(Partitions[i].GetVal2(), Table());
      while (RowI < EndI) {
        TInt RowId = RowI.GetRowIdx(); // EdgeId
        TInt SrcId = RowI.GetIntAttr(SrcColIdx);
        TInt DstId = RowI.GetIntAttr(DstColIdx);
        Graph->AddEdgeUnchecked(RowId, SrcId, DstId);
        RowI++;
      }
    }
  }
  else if (NodeType == atStr) {
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < Partitions.Len(); i++) {
      TRowIterator RowI(Partitions[i].GetVal1(), Table());
      TRowIterator EndI(Partitions[i].GetVal2(), Table());
      while (RowI < EndI) {
        TInt RowId = RowI.GetRowIdx(); // EdgeId
        TInt SrcId = RowI.GetStrMapById(SrcColIdx);
        TInt DstId = RowI.GetStrMapById(DstColIdx);
        Graph->AddEdgeUnchecked(RowId, SrcId, DstId);
        RowI++;
      }
    }
  }
  Graph->SetEdges(NumRows);
  Sw->Stop(TStopwatch::AddEdges);

  // double endAdd = omp_get_wtime();
  // printf("Add time = %f\n", endAdd-endAlloc);

  return Graph;
}
/// Calls ToNetworkMP2 with an empty attribute vector. Convenience wrapper.
template<class PGraphMP>
PGraphMP ToNetworkMP2(PTable Table,
  const TStr& SrcCol, const TStr& DstCol, TAttrAggr AggrPolicy)
{
  TStrV V;
  return ToNetworkMP2<PGraphMP>(Table, SrcCol, DstCol, V, V, V, AggrPolicy);
}
#endif // GCC_ATOMIC


/// Loads a mode, with name Name, into the PMMNet from the TTable. NCol specifies the node id column and NodeAttrV the node attributes.
int LoadModeNetToNet(PMMNet Graph, const TStr& Name, PTable Table, const TStr& NCol,
  TStrV& NodeAttrV);
/// Loads the nodes specified in column NCol from the TTable with the attributes specified in NodeAttrV.
int LoadMode(TModeNet& Graph, PTable Table, const TStr& NCol,
  TStrV& NodeAttrV);
/// Loads a crossnet from Mode1 to Mode2, with name CrossName, from the provided TTable. EdgeAttrV specifies edge attributes.
int LoadCrossNetToNet(PMMNet Graph, const TStr& Mode1, const TStr& Mode2, const TStr& CrossName,
 PTable Table, const TStr& SrcCol, const TStr& DstCol, TStrV& EdgeAttrV);
/// Loads the edges from the TTable and EdgeAttrV specifies columns containing edge attributes.
int LoadCrossNet(TCrossNet& Graph, PTable Table, const TStr& SrcCol, const TStr& DstCol,
  TStrV& EdgeAttrV);


/// Converts table to a network sequentially. Use if network has only edge attributes.
template<class PGraph>
PGraph ToNetwork(PTable Table,
  const TStr& SrcCol, const TStr& DstCol,
  TStrV& EdgeAttrV,
  TAttrAggr AggrPolicy) {
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

  // make single pass over all rows in the table
  for (int CurrRowIdx = 0; CurrRowIdx < (Table->Next).Len(); CurrRowIdx++) {
    if ((Table->Next)[CurrRowIdx] == Table->Invalid) {
      continue;
    }

    // add src and dst nodes to graph if they are not seen earlier
    TInt SVal, DVal;
    if (NodeType == atFlt) {
      TFlt FSVal = (Table->FltCols)[SrcColIdx][CurrRowIdx];
      SVal = Table->CheckAndAddFltNode(Graph, FltNodeVals, FSVal);
      TFlt FDVal = (Table->FltCols)[SrcColIdx][CurrRowIdx];
      DVal = Table->CheckAndAddFltNode(Graph, FltNodeVals, FDVal);
    }
    else if (NodeType == atInt || NodeType == atStr) {
      if (NodeType == atInt) {
        SVal = (Table->IntCols)[SrcColIdx][CurrRowIdx];
        DVal = (Table->IntCols)[DstColIdx][CurrRowIdx];
      }
      else {
        SVal = (Table->StrColMaps)[SrcColIdx][CurrRowIdx];
  //        if (strlen(Table->GetContextKey(SVal)) == 0) { continue; }  //illegal value
        DVal = (Table->StrColMaps)[DstColIdx][CurrRowIdx];
  //        if (strlen(Table->GetContextKey(DVal)) == 0) { continue; }  //illegal value
      }
      if (!Graph->IsNode(SVal)) {Graph->AddNode(SVal); }
      if (!Graph->IsNode(DVal)) {Graph->AddNode(DVal); }
        //CheckAndAddIntNode(Graph, IntNodeVals, SVal);
        //CheckAndAddIntNode(Graph, IntNodeVals, DVal);
    }

      // add edge and edge attributes
    Graph->AddEdge(SVal, DVal, CurrRowIdx);

  		// Aggregate edge attributes and add to graph
    for (TInt i = 0; i < EdgeAttrV.Len(); i++) {
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
  }
  return Graph;

}

#ifdef GCC_ATOMIC

/// Converts table to network in parallel. Use if network has only edge attributes.
template<class PGraphMP>
inline PGraphMP ToNetworkMP(PTable Table,
  const TStr& SrcCol, const TStr& DstCol,
  TStrV& EdgeAttrV,
  TAttrAggr AggrPolicy) {
  TStopwatch* Sw = TStopwatch::GetInstance();

  Sw->Start(TStopwatch::AllocateColumnCopies);
  const TInt SrcColIdx = Table->GetColIdx(SrcCol);
  const TInt DstColIdx = Table->GetColIdx(DstCol);
  const TInt NumRows = Table->GetNumValidRows();

  const TAttrType NodeType = Table->GetColType(SrcCol);
  Assert(NodeType == Table->GetColType(DstCol));

  TIntV SrcCol1, EdgeCol1, EdgeCol2, DstCol2;

  THash<TInt, TStrIntVH> NodeIntAttrs;
  THash<TInt, TStrFltVH> NodeFltAttrs;
  THash<TInt, TStrStrVH> NodeStrAttrs;

  #pragma omp parallel sections num_threads(4)
  {
    #pragma omp section
    { SrcCol1.Reserve(NumRows, NumRows); }
    #pragma omp section
    { EdgeCol1.Reserve(NumRows, NumRows); }
    #pragma omp section
    { DstCol2.Reserve(NumRows, NumRows); }
    #pragma omp section
    { EdgeCol2.Reserve(NumRows, NumRows); }
  }
  Sw->Stop(TStopwatch::AllocateColumnCopies);

  Sw->Start(TStopwatch::CopyColumns);
  TIntPrV Partitions;
  Table->GetPartitionRanges(Partitions, omp_get_max_threads());
  TInt PartitionSize = Partitions[0].GetVal2()-Partitions[0].GetVal1()+1;


  // double endPartition = omp_get_wtime();
  // printf("Partition time = %f\n", endPartition-endResize);

  omp_set_num_threads(omp_get_max_threads());
  if (NodeType == atInt) {
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < Partitions.Len(); i++) {
      TRowIterator RowI(Partitions[i].GetVal1(), Table());
      TRowIterator EndI(Partitions[i].GetVal2(), Table());
      while (RowI < EndI) {
        TInt RowId = RowI.GetRowIdx();
        SrcCol1[RowId] = RowI.GetIntAttr(SrcColIdx);
        EdgeCol1[RowId] = RowId;
        DstCol2[RowId] = RowI.GetIntAttr(DstColIdx);
        EdgeCol2[RowId] = RowId;
        RowI++;
      }
    }
  }
  else if (NodeType == atStr) {
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < Partitions.Len(); i++) {
      TRowIterator RowI(Partitions[i].GetVal1(), Table());
      TRowIterator EndI(Partitions[i].GetVal2(), Table());
      while (RowI < EndI) {
        TInt RowId = RowI.GetRowIdx();
        SrcCol1[RowId] = RowI.GetStrMapById(SrcColIdx);
        EdgeCol1[RowId] = RowId;
        DstCol2[RowId] = RowI.GetStrMapById(DstColIdx);
        EdgeCol2[RowId] = RowId;
        RowI++;
     }
    }
  }
  Sw->Stop(TStopwatch::CopyColumns);

  Sw->Start(TStopwatch::Sort);
  omp_set_num_threads(omp_get_max_threads());
  #pragma omp parallel
  {
    #pragma omp single nowait
    {
      #ifndef GLib_WIN32
      #pragma omp task untied shared(SrcCol1, EdgeCol1)
      #endif
      { TTable::QSortKeyVal(SrcCol1, EdgeCol1, 0, NumRows-1); }
    }
    #pragma omp single nowait
    {
      #ifndef GLib_WIN32
      #pragma omp task untied shared(EdgeCol2, DstCol2)
      #endif
     { TTable::QSortKeyVal(DstCol2, EdgeCol2, 0, NumRows-1); }
    }
    #ifndef GLib_WIN32
    #pragma omp taskwait
    #endif
  }
  Sw->Stop(TStopwatch::Sort);

  Sw->Start(TStopwatch::Group);
  TInt NumThreads = omp_get_max_threads();
  TInt PartSize = (NumRows/NumThreads);

  // Find the offset of all partitions, each of which contains a list of rows.
  // Nodes from same sources or destinations are ensured to be kept within same partition.
  TIntV SrcOffsets, DstOffsets;
  SrcOffsets.Add(0);
  for (TInt i = 1; i < NumThreads; i++) {
    TInt CurrOffset = i * PartSize;
    while (CurrOffset < (i+1) * PartSize &&
          SrcCol1[CurrOffset-1] == SrcCol1[CurrOffset]) {
      // ensure that rows from the same sources are grouped together
      CurrOffset++;
    }
    if (CurrOffset < (i+1) * PartSize) { SrcOffsets.Add(CurrOffset); }
  }
  SrcOffsets.Add(NumRows);

  DstOffsets.Add(0);
  for (TInt i = 1; i < NumThreads; i++) {
    TInt CurrOffset = i * PartSize;
    while (CurrOffset < (i+1) * PartSize &&
          DstCol2[CurrOffset-1] == DstCol2[CurrOffset]) {
      // ensure that rows to the same destinations are grouped together
      CurrOffset++;
    }
    if (CurrOffset < (i+1) * PartSize) { DstOffsets.Add(CurrOffset); }
  }
  DstOffsets.Add(NumRows);

  TInt SrcPartCnt = SrcOffsets.Len()-1; // number of partitions
  TInt DstPartCnt = DstOffsets.Len()-1; // number of partitions

  // count the number of source nodes and destination nodes in each partition
  TIntV SrcNodeCounts, DstNodeCounts;
  SrcNodeCounts.Reserve(SrcPartCnt, SrcPartCnt);
  DstNodeCounts.Reserve(DstPartCnt, DstPartCnt);

  #pragma omp parallel for schedule(dynamic)
  for (int t = 0; t < SrcPartCnt+DstPartCnt; t++) {
    if (t < SrcPartCnt) {
      TInt i = t;
      if (SrcOffsets[i] != SrcOffsets[i+1]) {
        SrcNodeCounts[i] = 1;
        TInt CurrNode = SrcCol1[SrcOffsets[i]];
        for (TInt j = SrcOffsets[i]+1; j < SrcOffsets[i+1]; j++) {
          while (j < SrcOffsets[i+1] && SrcCol1[j] == CurrNode) { j++; }
          if (j < SrcOffsets[i+1]) {
            SrcNodeCounts[i]++;
            CurrNode = SrcCol1[j];
          }
        }
      }
    } else {
      TInt i = t - SrcPartCnt;
      if (DstOffsets[i] != DstOffsets[i+1]) {
        DstNodeCounts[i] = 1;
        TInt CurrNode = DstCol2[DstOffsets[i]];
        for (TInt j = DstOffsets[i]+1; j < DstOffsets[i+1]; j++) {
          while (j < DstOffsets[i+1] && DstCol2[j] == CurrNode) { j++; }
          if (j < DstOffsets[i+1]) {
            DstNodeCounts[i]++;
            CurrNode = DstCol2[j];
          }
        }
      }
    }
  }

  TInt TotalSrcNodes = 0;
  TIntV SrcIdOffsets;
  for (int i = 0; i < SrcPartCnt; i++) {
    SrcIdOffsets.Add(TotalSrcNodes);
    TotalSrcNodes += SrcNodeCounts[i];
  }

  TInt TotalDstNodes = 0;
  TIntV DstIdOffsets;
  for (int i = 0; i < DstPartCnt; i++) {
    DstIdOffsets.Add(TotalDstNodes);
    TotalDstNodes += DstNodeCounts[i];
  }

  // printf("Total Src = %d, Total Dst = %d\n", TotalSrcNodes.Val, TotalDstNodes.Val);

  // find vector of (node_id, start_offset) where start_offset is the index of the first row with node_id
  TIntPrV SrcNodeIds, DstNodeIds;
  #pragma omp parallel sections
  {
    #pragma omp section
    { SrcNodeIds.Reserve(TotalSrcNodes, TotalSrcNodes); }
    #pragma omp section
    { DstNodeIds.Reserve(TotalDstNodes, TotalDstNodes); }
  }

  // Find the starting offset of each node (in both src and dst)
  #pragma omp parallel for schedule(dynamic)
  for (int t = 0; t < SrcPartCnt+DstPartCnt; t++) {
    if (t < SrcPartCnt) {
      TInt i = t;
      if (SrcOffsets[i] != SrcOffsets[i+1]) {
        TInt CurrNode = SrcCol1[SrcOffsets[i]];
        TInt ThreadOffset = SrcIdOffsets[i];
        SrcNodeIds[ThreadOffset] = TIntPr(CurrNode, SrcOffsets[i]);
        TInt CurrCount = 1;
        for (TInt j = SrcOffsets[i]+1; j < SrcOffsets[i+1]; j++) {
          while (j < SrcOffsets[i+1] && SrcCol1[j] == CurrNode) { j++; }
          if (j < SrcOffsets[i+1]) {
            CurrNode = SrcCol1[j];
            SrcNodeIds[ThreadOffset+CurrCount] = TIntPr(CurrNode, j);
            CurrCount++;
          }
        }
      }
    } else {
      TInt i = t - SrcPartCnt;
      if (DstOffsets[i] != DstOffsets[i+1]) {
        TInt CurrNode = DstCol2[DstOffsets[i]];
        TInt ThreadOffset = DstIdOffsets[i];
        DstNodeIds[ThreadOffset] = TIntPr(CurrNode, DstOffsets[i]);
        TInt CurrCount = 1;
        for (TInt j = DstOffsets[i]+1; j < DstOffsets[i+1]; j++) {
          while (j < DstOffsets[i+1] && DstCol2[j] == CurrNode) { j++; }
          if (j < DstOffsets[i+1]) {
            CurrNode = DstCol2[j];
            DstNodeIds[ThreadOffset+CurrCount] = TIntPr(CurrNode, j);
            CurrCount++;
          }
        }
      }
    }
  }
  Sw->Stop(TStopwatch::Group);

  Sw->Start(TStopwatch::MergeNeighborhoods);
  // Find the combined neighborhood (both out-neighbors and in-neighbors) of each node
  TIntTrV Nodes;
  Nodes.Reserve(TotalSrcNodes+TotalDstNodes);

  TInt i = 0, j = 0;
  while (i < TotalSrcNodes && j < TotalDstNodes) {
    if (SrcNodeIds[i].Val1 == DstNodeIds[j].Val1) {
      Nodes.Add(TIntTr(SrcNodeIds[i].Val1, i, j));
      i++;
      j++;
    } else if (SrcNodeIds[i].Val1 < DstNodeIds[j].Val1) {
      Nodes.Add(TIntTr(SrcNodeIds[i].Val1, i, -1));
      i++;
    } else {
      Nodes.Add(TIntTr(DstNodeIds[j].Val1, -1, j));
      j++;
    }
  }
  for (; i < TotalSrcNodes; i++) { Nodes.Add(TIntTr(SrcNodeIds[i].Val1, i, -1)); }
  for (; j < TotalDstNodes; j++) { Nodes.Add(TIntTr(DstNodeIds[j].Val1, -1, j)); }
  Sw->Stop(TStopwatch::MergeNeighborhoods);

  Sw->Start(TStopwatch::AddNeighborhoods);
  TInt NumNodes = Nodes.Len();
  PGraphMP Graph = PGraphMP::TObj::New(NumNodes, NumRows);
//  NumThreads = omp_get_max_threads();
//  int Delta = (NumNodes+NumThreads-1)/NumThreads;

  TVec<TIntV> InVV(NumNodes);
  TVec<TIntV> OutVV(NumNodes);

//  omp_set_num_threads(NumThreads);
  #pragma omp parallel for schedule(static,100)
  for (int m = 0; m < NumNodes; m++) {
    //double startTr = omp_get_wtime();
    //TIntV OutV, InV;
    TInt n, i, j;
    Nodes[m].GetVal(n, i, j);
    if (i >= 0) {
      TInt Offset = SrcNodeIds[i].GetVal2();
      TInt Sz = EdgeCol1.Len()-Offset;
      if (i < SrcNodeIds.Len()-1) { Sz = SrcNodeIds[i+1].GetVal2()-Offset; }
      OutVV[m].Reserve(Sz);
      OutVV[m].CopyUniqueFrom(EdgeCol1, Offset, Sz);
    }
    if (j >= 0) {
      TInt Offset = DstNodeIds[j].GetVal2();
      TInt Sz = EdgeCol2.Len()-Offset;
      if (j < DstNodeIds.Len()-1) { Sz = DstNodeIds[j+1].GetVal2()-Offset; }
      InVV[m].Reserve(Sz);
      InVV[m].CopyUniqueFrom(EdgeCol2, Offset, Sz);
    }
    Graph->AddNodeWithEdges(n, InVV[m], OutVV[m]);
  }
  Graph->SetNodes(NumNodes);
  Sw->Stop(TStopwatch::AddNeighborhoods);

  Sw->Start(TStopwatch::AddEdges);
  omp_set_num_threads(omp_get_max_threads());
  if (NodeType == atInt) {
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < Partitions.Len(); i++) {
      TRowIterator RowI(Partitions[i].GetVal1(), Table());
      TRowIterator EndI(Partitions[i].GetVal2(), Table());
      while (RowI < EndI) {
        TInt RowId = RowI.GetRowIdx(); // EdgeId
        TInt SrcId = RowI.GetIntAttr(SrcColIdx);
        TInt DstId = RowI.GetIntAttr(DstColIdx);
        Graph->AddEdgeUnchecked(RowId, SrcId, DstId);
        RowI++;
      }
    }
  }
  else if (NodeType == atStr) {
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < Partitions.Len(); i++) {
      TRowIterator RowI(Partitions[i].GetVal1(), Table());
      TRowIterator EndI(Partitions[i].GetVal2(), Table());
      while (RowI < EndI) {
        TInt RowId = RowI.GetRowIdx(); // EdgeId
        TInt SrcId = RowI.GetStrMapById(SrcColIdx);
        TInt DstId = RowI.GetStrMapById(DstColIdx);
        Graph->AddEdgeUnchecked(RowId, SrcId, DstId);
        RowI++;
      }
    }

  }

  Graph->SetEdges(NumRows);
  Graph->SetMxEId(NumRows);
  Sw->Stop(TStopwatch::AddEdges);

  // make single pass over all rows in the table to add attributes
  for (int CurrRowIdx = 0; CurrRowIdx < (Table->Next).Len(); CurrRowIdx++) {
    if ((Table->Next)[CurrRowIdx] == Table->Invalid) {
      continue;
    }
    for (TInt ea_i = 0; ea_i < EdgeAttrV.Len(); ea_i++) {
      TStr ColName = EdgeAttrV[ea_i];
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
  }
  // double endAdd = omp_get_wtime();
  // printf("Add time = %f\n", endAdd-endAlloc);

  return Graph;
}
#endif // GCC_ATOMIC

/// Converts table to network sequentially. Takes edges from \c Table and nodes explicitly from \c NodeCol in \c NodeTable, with attribute vectors passed as columns in corresponding tables.
template<class PGraph>
PGraph ToNetwork(PTable Table,
  const TStr& SrcCol, const TStr& DstCol,
  TStrV& EdgeAttrV, PTable NodeTable, const TStr& NodeCol, TStrV& NodeAttrV,
  TAttrAggr AggrPolicy) {
  PGraph Graph = PGraph::TObj::New();

  const TAttrType NodeType = Table->GetColType(SrcCol);
  Assert(NodeType == Table->GetColType(DstCol));
  const TInt SrcColIdx = Table->GetColIdx(SrcCol);
  const TInt DstColIdx = Table->GetColIdx(DstCol);


  const TAttrType NodeTypeN = NodeTable->GetColType(NodeCol);
  const TInt NodeColIdx = NodeTable->GetColIdx(NodeCol);
  THash<TInt, TStrIntVH> NodeIntAttrs;
  THash<TInt, TStrFltVH> NodeFltAttrs;
  THash<TInt, TStrStrVH> NodeStrAttrs;


	//Table->AddGraphAttributeV(SrcAttrV, false, true, false);
	//Table->AddGraphAttributeV(DstAttrV, false, false, true);
	//Table->AddGraphAttributeV(EdgeAttrV, true, false, true);

  // node values - i.e. the unique values of src/dst col
  //THashSet<TInt> IntNodeVals; // for both int and string node attr types.
  THash<TFlt, TInt> FltNodeVals;

  // make single pass over all rows in the table
  for (int CurrRowIdx = 0; CurrRowIdx < (Table->Next).Len(); CurrRowIdx++) {
    if ((Table->Next)[CurrRowIdx] == Table->Invalid) {
      continue;
    }

    // add src and dst nodes to graph if they are not seen earlier
    TInt SVal, DVal;
    if (NodeType == atFlt) {
      TFlt FSVal = (Table->FltCols)[SrcColIdx][CurrRowIdx];
      SVal = Table->CheckAndAddFltNode(Graph, FltNodeVals, FSVal);
      TFlt FDVal = (Table->FltCols)[SrcColIdx][CurrRowIdx];
      DVal = Table->CheckAndAddFltNode(Graph, FltNodeVals, FDVal);
    }
    else if (NodeType == atInt || NodeType == atStr) {
      if (NodeType == atInt) {
        SVal = (Table->IntCols)[SrcColIdx][CurrRowIdx];
        DVal = (Table->IntCols)[DstColIdx][CurrRowIdx];
      }
      else {
        SVal = (Table->StrColMaps)[SrcColIdx][CurrRowIdx];
  //        if (strlen(Table->GetContextKey(SVal)) == 0) { continue; }  //illegal value
        DVal = (Table->StrColMaps)[DstColIdx][CurrRowIdx];
  //        if (strlen(Table->GetContextKey(DVal)) == 0) { continue; }  //illegal value
      }
      if (!Graph->IsNode(SVal)) {Graph->AddNode(SVal); }
      if (!Graph->IsNode(DVal)) {Graph->AddNode(DVal); }
        //CheckAndAddIntNode(Graph, IntNodeVals, SVal);
        //CheckAndAddIntNode(Graph, IntNodeVals, DVal);
    }

      // add edge and edge attributes
    Graph->AddEdge(SVal, DVal, CurrRowIdx);

  		// Aggregate edge attributes and add to graph
    for (TInt i = 0; i < EdgeAttrV.Len(); i++) {
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
  }


  //Add node attribtes
  if (NodeAttrV.Len() > 0) {
    for (int CurrRowIdx = 0; CurrRowIdx < (NodeTable->Next).Len(); CurrRowIdx++) {
      if ((NodeTable->Next)[CurrRowIdx] == NodeTable->Invalid) {
      	continue;
      }
      TInt NId;
      if (NodeTypeN == atInt) {
        NId = (NodeTable->IntCols)[NodeColIdx][CurrRowIdx];
      }
      else if (NodeTypeN == atStr){
        NId = (NodeTable->StrColMaps)[NodeColIdx][CurrRowIdx];
      }
      for (TInt i = 0; i < NodeAttrV.Len(); i++) {
        TStr ColName = NodeAttrV[i];
        TAttrType T = NodeTable->GetColType(ColName);
        TInt Index = NodeTable->GetColIdx(ColName);
        switch (T) {
          case atInt:
            Graph->AddIntAttrDatN(NId, NodeTable->IntCols[Index][CurrRowIdx], ColName);
            break;
          case atFlt:
            Graph->AddFltAttrDatN(NId, NodeTable->FltCols[Index][CurrRowIdx], ColName);
            break;
          case atStr:
            Graph->AddStrAttrDatN(NId, NodeTable->GetStrVal(Index, CurrRowIdx), ColName);
            break;
        }
      }
    }
  }

  return Graph;

}

#ifdef GCC_ATOMIC

/// Converts table to network in parallel. Takes edges from \c Table and nodes explicitly from \c NodeCol in \c NodeTable, with attribute vectors passed as columns in corresponding tables.
template<class PGraphMP>
inline PGraphMP ToNetworkMP(PTable Table,
  const TStr& SrcCol, const TStr& DstCol,
  TStrV& EdgeAttrV, PTable NodeTable, const TStr& NodeCol, TStrV& NodeAttrV,
  TAttrAggr AggrPolicy) {
  TStopwatch* Sw = TStopwatch::GetInstance();

  Sw->Start(TStopwatch::AllocateColumnCopies);
  const TInt SrcColIdx = Table->GetColIdx(SrcCol);
  const TInt DstColIdx = Table->GetColIdx(DstCol);
  const TInt NumRows = Table->GetNumValidRows();

  const TAttrType NodeType = Table->GetColType(SrcCol);
  Assert(NodeType == Table->GetColType(DstCol));


  TIntV SrcCol1, EdgeCol1, EdgeCol2, DstCol2;

  const TAttrType NodeTypeN = NodeTable->GetColType(NodeCol);
  const TInt NodeColIdx = NodeTable->GetColIdx(NodeCol);
  THash<TInt, TStrIntVH> NodeIntAttrs;
  THash<TInt, TStrFltVH> NodeFltAttrs;
  THash<TInt, TStrStrVH> NodeStrAttrs;

  #pragma omp parallel sections num_threads(4)
  {
    #pragma omp section
    { SrcCol1.Reserve(NumRows, NumRows); }
    #pragma omp section
    { EdgeCol1.Reserve(NumRows, NumRows); }
    #pragma omp section
    { DstCol2.Reserve(NumRows, NumRows); }
    #pragma omp section
    { EdgeCol2.Reserve(NumRows, NumRows); }
  }
  Sw->Stop(TStopwatch::AllocateColumnCopies);

  Sw->Start(TStopwatch::CopyColumns);
  TIntPrV Partitions;
  Table->GetPartitionRanges(Partitions, omp_get_max_threads());
  TInt PartitionSize = Partitions[0].GetVal2()-Partitions[0].GetVal1()+1;

  // double endPartition = omp_get_wtime();
  // printf("Partition time = %f\n", endPartition-endResize);

  omp_set_num_threads(omp_get_max_threads());
  if (NodeType == atInt) {
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < Partitions.Len(); i++) {
      TRowIterator RowI(Partitions[i].GetVal1(), Table());
      TRowIterator EndI(Partitions[i].GetVal2(), Table());
      while (RowI < EndI) {
        TInt RowId = RowI.GetRowIdx();
        SrcCol1[RowId] = RowI.GetIntAttr(SrcColIdx);
        EdgeCol1[RowId] = RowId;
        DstCol2[RowId] = RowI.GetIntAttr(DstColIdx);
        EdgeCol2[RowId] = RowId;
        RowI++;
      }
    }
  }
  else if (NodeType == atStr) {
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < Partitions.Len(); i++) {
      TRowIterator RowI(Partitions[i].GetVal1(), Table());
      TRowIterator EndI(Partitions[i].GetVal2(), Table());
      while (RowI < EndI) {
        TInt RowId = RowI.GetRowIdx();
        SrcCol1[RowId] = RowI.GetStrMapById(SrcColIdx);
        EdgeCol1[RowId] = RowId;
        DstCol2[RowId] = RowI.GetStrMapById(DstColIdx);
        EdgeCol2[RowId] = RowId;
        RowI++;
      }
    }
  }
  Sw->Stop(TStopwatch::CopyColumns);

  Sw->Start(TStopwatch::Sort);
  omp_set_num_threads(omp_get_max_threads());
  #pragma omp parallel
  {
    #pragma omp single nowait
    {
      #ifndef GLib_WIN32
      #pragma omp task untied shared(SrcCol1, EdgeCol1)
      #endif
      { TTable::QSortKeyVal(SrcCol1, EdgeCol1, 0, NumRows-1); }
    }
    #pragma omp single nowait
    {
      #ifndef GLib_WIN32
      #pragma omp task untied shared(EdgeCol2, DstCol2)
      #endif
     { TTable::QSortKeyVal(DstCol2, EdgeCol2, 0, NumRows-1); }
    }
    #ifndef GLib_WIN32
    #pragma omp taskwait
    #endif
  }
  Sw->Stop(TStopwatch::Sort);

  Sw->Start(TStopwatch::Group);
  TInt NumThreads = omp_get_max_threads();
  TInt PartSize = (NumRows/NumThreads);

  // Find the offset of all partitions, each of which contains a list of rows.
  // Nodes from same sources or destinations are ensured to be kept within same partition.
  TIntV SrcOffsets, DstOffsets;
  SrcOffsets.Add(0);
  for (TInt i = 1; i < NumThreads; i++) {
    TInt CurrOffset = i * PartSize;
    while (CurrOffset < (i+1) * PartSize &&
          SrcCol1[CurrOffset-1] == SrcCol1[CurrOffset]) {
      // ensure that rows from the same sources are grouped together
      CurrOffset++;
    }
    if (CurrOffset < (i+1) * PartSize) { SrcOffsets.Add(CurrOffset); }
  }
  SrcOffsets.Add(NumRows);

  DstOffsets.Add(0);
  for (TInt i = 1; i < NumThreads; i++) {
    TInt CurrOffset = i * PartSize;
    while (CurrOffset < (i+1) * PartSize &&
          DstCol2[CurrOffset-1] == DstCol2[CurrOffset]) {
      // ensure that rows to the same destinations are grouped together
      CurrOffset++;
    }
    if (CurrOffset < (i+1) * PartSize) { DstOffsets.Add(CurrOffset); }
  }
  DstOffsets.Add(NumRows);

  TInt SrcPartCnt = SrcOffsets.Len()-1; // number of partitions
  TInt DstPartCnt = DstOffsets.Len()-1; // number of partitions

  // count the number of source nodes and destination nodes in each partition
  TIntV SrcNodeCounts, DstNodeCounts;
  SrcNodeCounts.Reserve(SrcPartCnt, SrcPartCnt);
  DstNodeCounts.Reserve(DstPartCnt, DstPartCnt);

  #pragma omp parallel for schedule(dynamic)
  for (int t = 0; t < SrcPartCnt+DstPartCnt; t++) {
    if (t < SrcPartCnt) {
      TInt i = t;
      if (SrcOffsets[i] != SrcOffsets[i+1]) {
        SrcNodeCounts[i] = 1;
        TInt CurrNode = SrcCol1[SrcOffsets[i]];
        for (TInt j = SrcOffsets[i]+1; j < SrcOffsets[i+1]; j++) {
          while (j < SrcOffsets[i+1] && SrcCol1[j] == CurrNode) { j++; }
          if (j < SrcOffsets[i+1]) {
            SrcNodeCounts[i]++;
            CurrNode = SrcCol1[j];
          }
        }
      }
    } else {
      TInt i = t - SrcPartCnt;
      if (DstOffsets[i] != DstOffsets[i+1]) {
        DstNodeCounts[i] = 1;
        TInt CurrNode = DstCol2[DstOffsets[i]];
        for (TInt j = DstOffsets[i]+1; j < DstOffsets[i+1]; j++) {
          while (j < DstOffsets[i+1] && DstCol2[j] == CurrNode) { j++; }
          if (j < DstOffsets[i+1]) {
            DstNodeCounts[i]++;
            CurrNode = DstCol2[j];
          }
        }
      }
    }
  }

  TInt TotalSrcNodes = 0;
  TIntV SrcIdOffsets;
  for (int i = 0; i < SrcPartCnt; i++) {
    SrcIdOffsets.Add(TotalSrcNodes);
    TotalSrcNodes += SrcNodeCounts[i];
  }

  TInt TotalDstNodes = 0;
  TIntV DstIdOffsets;
  for (int i = 0; i < DstPartCnt; i++) {
    DstIdOffsets.Add(TotalDstNodes);
    TotalDstNodes += DstNodeCounts[i];
  }

  // printf("Total Src = %d, Total Dst = %d\n", TotalSrcNodes.Val, TotalDstNodes.Val);

  // find vector of (node_id, start_offset) where start_offset is the index of the first row with node_id
  TIntPrV SrcNodeIds, DstNodeIds;
  #pragma omp parallel sections
  {
    #pragma omp section
    { SrcNodeIds.Reserve(TotalSrcNodes, TotalSrcNodes); }
    #pragma omp section
    { DstNodeIds.Reserve(TotalDstNodes, TotalDstNodes); }
  }

  // Find the starting offset of each node (in both src and dst)
  #pragma omp parallel for schedule(dynamic)
  for (int t = 0; t < SrcPartCnt+DstPartCnt; t++) {
    if (t < SrcPartCnt) {
      TInt i = t;
      if (SrcOffsets[i] != SrcOffsets[i+1]) {
        TInt CurrNode = SrcCol1[SrcOffsets[i]];
        TInt ThreadOffset = SrcIdOffsets[i];
        SrcNodeIds[ThreadOffset] = TIntPr(CurrNode, SrcOffsets[i]);
        TInt CurrCount = 1;
        for (TInt j = SrcOffsets[i]+1; j < SrcOffsets[i+1]; j++) {
          while (j < SrcOffsets[i+1] && SrcCol1[j] == CurrNode) { j++; }
          if (j < SrcOffsets[i+1]) {
            CurrNode = SrcCol1[j];
            SrcNodeIds[ThreadOffset+CurrCount] = TIntPr(CurrNode, j);
            CurrCount++;
          }
        }
      }
    } else {
      TInt i = t - SrcPartCnt;
      if (DstOffsets[i] != DstOffsets[i+1]) {
        TInt CurrNode = DstCol2[DstOffsets[i]];
        TInt ThreadOffset = DstIdOffsets[i];
        DstNodeIds[ThreadOffset] = TIntPr(CurrNode, DstOffsets[i]);
        TInt CurrCount = 1;
        for (TInt j = DstOffsets[i]+1; j < DstOffsets[i+1]; j++) {
          while (j < DstOffsets[i+1] && DstCol2[j] == CurrNode) { j++; }
          if (j < DstOffsets[i+1]) {
            CurrNode = DstCol2[j];
            DstNodeIds[ThreadOffset+CurrCount] = TIntPr(CurrNode, j);
            CurrCount++;
          }
        }
      }
    }
  }
  Sw->Stop(TStopwatch::Group);

  Sw->Start(TStopwatch::MergeNeighborhoods);
  // Find the combined neighborhood (both out-neighbors and in-neighbors) of each node
  TIntTrV Nodes;
  Nodes.Reserve(TotalSrcNodes+TotalDstNodes);

  TInt i = 0, j = 0;
  while (i < TotalSrcNodes && j < TotalDstNodes) {
    if (SrcNodeIds[i].Val1 == DstNodeIds[j].Val1) {
      Nodes.Add(TIntTr(SrcNodeIds[i].Val1, i, j));
      i++;
      j++;
    } else if (SrcNodeIds[i].Val1 < DstNodeIds[j].Val1) {
      Nodes.Add(TIntTr(SrcNodeIds[i].Val1, i, -1));
      i++;
    } else {
      Nodes.Add(TIntTr(DstNodeIds[j].Val1, -1, j));
      j++;
    }
  }
  for (; i < TotalSrcNodes; i++) { Nodes.Add(TIntTr(SrcNodeIds[i].Val1, i, -1)); }
  for (; j < TotalDstNodes; j++) { Nodes.Add(TIntTr(DstNodeIds[j].Val1, -1, j)); }
  Sw->Stop(TStopwatch::MergeNeighborhoods);

  Sw->Start(TStopwatch::AddNeighborhoods);
  TInt NumNodes = Nodes.Len();
  PGraphMP Graph = PGraphMP::TObj::New(NumNodes, NumRows);
//  NumThreads = omp_get_max_threads();
//  int Delta = (NumNodes+NumThreads-1)/NumThreads;

  TVec<TIntV> InVV(NumNodes);
  TVec<TIntV> OutVV(NumNodes);

//  omp_set_num_threads(NumThreads);
  #pragma omp parallel for schedule(static,100)
  for (int m = 0; m < NumNodes; m++) {
    //double startTr = omp_get_wtime();
    //TIntV OutV, InV;
    TInt n, i, j;
    Nodes[m].GetVal(n, i, j);
    if (i >= 0) {
      TInt Offset = SrcNodeIds[i].GetVal2();
      TInt Sz = EdgeCol1.Len()-Offset;
      if (i < SrcNodeIds.Len()-1) { Sz = SrcNodeIds[i+1].GetVal2()-Offset; }
      OutVV[m].Reserve(Sz);
      OutVV[m].CopyUniqueFrom(EdgeCol1, Offset, Sz);
    }
    if (j >= 0) {
      TInt Offset = DstNodeIds[j].GetVal2();
      TInt Sz = EdgeCol2.Len()-Offset;
      if (j < DstNodeIds.Len()-1) { Sz = DstNodeIds[j+1].GetVal2()-Offset; }
      InVV[m].Reserve(Sz);
      InVV[m].CopyUniqueFrom(EdgeCol2, Offset, Sz);
    }
    Graph->AddNodeWithEdges(n, InVV[m], OutVV[m]);
  }
  Graph->SetNodes(NumNodes);
  Sw->Stop(TStopwatch::AddNeighborhoods);

  Sw->Start(TStopwatch::AddEdges);
  omp_set_num_threads(omp_get_max_threads());
  if (NodeType == atInt) {
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < Partitions.Len(); i++) {
      TRowIterator RowI(Partitions[i].GetVal1(), Table());
      TRowIterator EndI(Partitions[i].GetVal2(), Table());
      while (RowI < EndI) {
        TInt RowId = RowI.GetRowIdx(); // EdgeId
        TInt SrcId = RowI.GetIntAttr(SrcColIdx);
        TInt DstId = RowI.GetIntAttr(DstColIdx);
        Graph->AddEdgeUnchecked(RowId, SrcId, DstId);
        RowI++;
      }
    }
  }
  else if (NodeType == atStr) {
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < Partitions.Len(); i++) {
      TRowIterator RowI(Partitions[i].GetVal1(), Table());
      TRowIterator EndI(Partitions[i].GetVal2(), Table());
      while (RowI < EndI) {
        TInt RowId = RowI.GetRowIdx(); // EdgeId
        TInt SrcId = RowI.GetStrMapById(SrcColIdx);
        TInt DstId = RowI.GetStrMapById(DstColIdx);
        Graph->AddEdgeUnchecked(RowId, SrcId, DstId);
        RowI++;
      }
    }

  }

  Graph->SetEdges(NumRows);
  Graph->SetMxEId(NumRows);
  Sw->Stop(TStopwatch::AddEdges);

  // make single pass over all rows in the table to add attributes
  for (int CurrRowIdx = 0; CurrRowIdx < (Table->Next).Len(); CurrRowIdx++) {
    if ((Table->Next)[CurrRowIdx] == Table->Invalid) {
      continue;
    }
    for (TInt ea_i = 0; ea_i < EdgeAttrV.Len(); ea_i++) {
      TStr ColName = EdgeAttrV[ea_i];
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
  }

  // Add node attribtes
  if (NodeAttrV.Len() > 0) {
    for (int CurrRowIdx = 0; CurrRowIdx < (NodeTable->Next).Len(); CurrRowIdx++) {
      if ((NodeTable->Next)[CurrRowIdx] == NodeTable->Invalid) {
      	continue;
      }
      TInt NId;
      if (NodeTypeN == atInt) {
        NId = (NodeTable->IntCols)[NodeColIdx][CurrRowIdx];
      }
      else if (NodeTypeN == atStr){
        NId = (NodeTable->StrColMaps)[NodeColIdx][CurrRowIdx];
      }
      for (TInt i = 0; i < NodeAttrV.Len(); i++) {
        TStr ColName = NodeAttrV[i];
        TAttrType T = NodeTable->GetColType(ColName);
        TInt Index = NodeTable->GetColIdx(ColName);
        switch (T) {
          case atInt:
            Graph->AddIntAttrDatN(NId, NodeTable->IntCols[Index][CurrRowIdx], ColName);
            break;
          case atFlt:
            Graph->AddFltAttrDatN(NId, NodeTable->FltCols[Index][CurrRowIdx], ColName);
            break;
          case atStr:
            Graph->AddStrAttrDatN(NId, NodeTable->GetStrVal(Index, CurrRowIdx), ColName);
            break;
        }
      }
    }
  }
  // double endAdd = omp_get_wtime();
  // printf("Add time = %f\n", endAdd-endAlloc);

  return Graph;
}
#endif // GCC_ATOMIC

}; // TSnap namespace

#endif // CONV_H

