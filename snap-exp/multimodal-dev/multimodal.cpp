#include "stdafx.h"
#include <Snap.h>
#include <stdio.h>
#include <omp.h>

void MergeNodeTables(const TVec<PTable>& NodeTables, const Schema& NodeSchema, THash<TStr,TInt>& Hash, TStrV& OriNIdV) {
  TInt Index = 0;

  for (TVec<PTable>::TIter it = NodeTables.BegI(); it < NodeTables.EndI(); it++) {
    PTable Table = *it;
    TStr IdColName = NodeSchema.GetVal(0).GetVal1();
    for (int CurrRowIdx = 0; CurrRowIdx < Table->GetNumRows(); CurrRowIdx++) {
      TStr Val = Table->GetStrVal(IdColName, CurrRowIdx);
      Hash.AddDat(Val, Index);
      OriNIdV[CurrRowIdx] = Val;
      Index++;
    }
  }
}

PTable MergeEdgeTables(const TVec<TPair<PTable,int> >& EdgeTables, const Schema& EdgeSchema,
                      THash<TStr,TInt>& Hash, TTableContext& Context) {
  TStr SrcIdColName = EdgeSchema.GetVal(0).GetVal1();
  TStr DstIdColName = EdgeSchema.GetVal(1).GetVal1();
  TStr WeightColName = EdgeSchema.GetVal(2).GetVal1();

  Schema MergedEdgeSchema = Schema();
  MergedEdgeSchema.Add(TPair<TStr,TAttrType>("SrcId", atInt));
  MergedEdgeSchema.Add(TPair<TStr,TAttrType>("DstId", atInt));
  MergedEdgeSchema.Add(TPair<TStr,TAttrType>("Weight", atFlt));
  PTable EdgeTbl = TTable::New(MergedEdgeSchema, Context);

  for (int i = 0; i < EdgeTables.Len(); i++) {
    PTable Table = EdgeTables[i].GetVal1();
    int direction = EdgeTables[i].GetVal2();
    for (int CurrRowIdx = 0; CurrRowIdx < Table->GetNumRows(); CurrRowIdx++) {
      TStr OriginalSrcId = Table->GetStrVal(SrcIdColName, CurrRowIdx);
      IAssertR(Hash.IsKey(OriginalSrcId), "SrcId of edges must be a node Id");
      TInt UniversalSrcId = Hash.GetDat(OriginalSrcId);
      TStr OriginalDstId = Table->GetStrVal(DstIdColName, CurrRowIdx);
      IAssertR(Hash.IsKey(OriginalDstId), "DstId of edges must be a node Id");
      TInt UniversalDstId = Hash.GetDat(OriginalDstId);
      TFlt Weight = Table->GetFltVal(WeightColName, CurrRowIdx);
      if ((direction & 2) != 0) {
        TTableRow NewRow;
        NewRow.AddInt(UniversalSrcId);
        NewRow.AddInt(UniversalDstId);
        NewRow.AddFlt(Weight);
        EdgeTbl->AddRow(NewRow);
      }
      if ((direction & 1) != 0) {
        TTableRow NewRow;
        NewRow.AddInt(UniversalDstId);
        NewRow.AddInt(UniversalSrcId);
        NewRow.AddFlt(Weight);
        EdgeTbl->AddRow(NewRow);
      }
    }
  }
  return EdgeTbl;
}
