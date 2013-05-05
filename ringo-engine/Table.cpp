#include "Table.h"

TInt const TTable::Last =-1;
TInt const TTable::Invalid =-2;

// expensive - consider using a StrHash / hashSet for str vals
// to verify uniquness of str vals
void TTable::AddStrVal(TInt ColIdx, TStr Val){
  for(TInt i = 0; i < NumOfDistinctStrVals; i++){
    if(Val == StrColVals.GetCStr(i)){
      StrColMaps[ColIdx].Add(i);
      return;
    }
  }
  TInt K = StrColVals.AddStr(Val);
  StrColMaps[ColIdx].Add(K);
  NumOfDistinctStrVals++;
}

void TTable::AddStrVal(TStr Col, TStr Val){
  TPair<TYPE,TInt> ColDat = ColTypeMap.GetDat(Col);
  if(ColDat.Val1 != STR){TExcept::Throw(Col + " is not a string valued column");}
  AddStrVal(ColDat.Val2, Val);
}

TStrV TTable::GetSrcNodeIntAttrV() const {
  TStrV IntNA = TStrV(IntCols.Len(),0);
  for (int i = 0; i < SrcNodeAttrV.Len(); i++) {
    TStr Attr = SrcNodeAttrV[i];
    if (ColTypeMap.GetDat(Attr).Val1 == INT) {
      IntNA.Add(Attr);
    }
  }
  return IntNA;
}

TStrV TTable::GetDstNodeIntAttrV() const {
  TStrV IntNA = TStrV(IntCols.Len(),0);
  for (int i = 0; i < DstNodeAttrV.Len(); i++) {
    TStr Attr = DstNodeAttrV[i];
    if (ColTypeMap.GetDat(Attr).Val1 == INT) {
      IntNA.Add(Attr);
    }
  }
  return IntNA;
}

TStrV TTable::GetEdgeIntAttrV() const {
  TStrV IntEA = TStrV(IntCols.Len(),0);
  for (int i = 0; i < EdgeAttrV.Len(); i++) {
    TStr Attr = EdgeAttrV[i];
    if (ColTypeMap.GetDat(Attr).Val1 == INT) {
      IntEA.Add(Attr);
    }
  }
  return IntEA;
}

TStrV TTable::GetSrcNodeFltAttrV() const {
  TStrV FltNA = TStrV(FltCols.Len(),0);
  for (int i = 0; i < SrcNodeAttrV.Len(); i++) {
    TStr Attr = SrcNodeAttrV[i];
    if (ColTypeMap.GetDat(Attr).Val1 == FLT) {
      FltNA.Add(Attr);
    }
  }
  return FltNA;
}

TStrV TTable::GetDstNodeFltAttrV() const {
  TStrV FltNA = TStrV(FltCols.Len(),0);
  for (int i = 0; i < DstNodeAttrV.Len(); i++) {
    TStr Attr = DstNodeAttrV[i];
    if (ColTypeMap.GetDat(Attr).Val1 == FLT) {
      FltNA.Add(Attr);
    }
  }
  return FltNA;
}

TStrV TTable::GetEdgeFltAttrV() const {
  TStrV FltEA = TStrV(FltCols.Len(),0);;
  for (int i = 0; i < EdgeAttrV.Len(); i++) {
    TStr Attr = EdgeAttrV[i];
    if (ColTypeMap.GetDat(Attr).Val1 == FLT) {
      FltEA.Add(Attr);
    }
  }
  return FltEA;
}

TStrV TTable::GetSrcNodeStrAttrV() const {
  TStrV StrNA = TStrV(StrColMaps.Len(),0);
  for (int i = 0; i < SrcNodeAttrV.Len(); i++) {
    TStr Attr = SrcNodeAttrV[i];
    if (ColTypeMap.GetDat(Attr).Val1 == STR) {
      StrNA.Add(Attr);
    }
  }
  return StrNA;
}

TStrV TTable::GetDstNodeStrAttrV() const {
  TStrV StrNA = TStrV(StrColMaps.Len(),0);
  for (int i = 0; i < DstNodeAttrV.Len(); i++) {
    TStr Attr = DstNodeAttrV[i];
    if (ColTypeMap.GetDat(Attr).Val1 == STR) {
      StrNA.Add(Attr);
    }
  }
  return StrNA;
}


TStrV TTable::GetEdgeStrAttrV() const {
  TStrV StrEA = TStrV(StrColMaps.Len(),0);
  for (int i = 0; i < EdgeAttrV.Len(); i++) {
    TStr Attr = EdgeAttrV[i];
    if (ColTypeMap.GetDat(Attr).Val1 == STR) {
      StrEA.Add(Attr);
    }
  }
  return StrEA;
}


void TTable::ChangeWorkingCol(TStr column){
  if(!ColTypeMap.IsKey(column)){TExcept::Throw("no such column " + column);}
  WorkingCol = column;
}

void TTable::AddLabel(TStr column, TStr newLabel){
  if(!ColTypeMap.IsKey(column)){TExcept::Throw("no such column " + column);}
  TPair<TYPE,TInt> ColVal = ColTypeMap.GetDat(column);
  ColTypeMap.AddDat(newLabel,ColVal);
}

void TTable::RemoveRow(TInt RowIdx){
  if(Next[RowIdx] == Invalid){ return;} // row was already removed
  if(RowIdx == FirstValidRow){
    FirstValidRow = Next[RowIdx];
  } else{
    TInt i = RowIdx-1;
    while(Next[i] != RowIdx){i--;}
    Next[i] = Next[RowIdx];
  }
  Next[RowIdx] = Invalid;
  NumRows--;
}

void TTable::RemoveRows(const TIntV& RemoveV){
  for(TInt i = 0; i < RemoveV.Len(); i++){RemoveRow(RemoveV[i]);}
}

void TTable::KeepSortedRows(const TIntV& KeepV){
  TInt KeepIdx = 0;
  for(TRowIterator RowI = BegRI(); RowI < EndRI(); RowI++){
    if((KeepIdx < KeepV.Len()) && (KeepV[KeepIdx] == RowI.GetRowIdx())){
      KeepIdx++;
    } else{
      RemoveRow(RowI.GetRowIdx());
    }   
  }
}

void TTable::Unique(TStr col){
  if(!ColTypeMap.IsKey(col)){TExcept::Throw("no such column " + col);}
  TIntV RemainingRows = TIntV(NumRows,0);
  TPair<TYPE,TInt> ColDat = ColTypeMap.GetDat(col);
  // group by given column (keys) and keep only first row for each key
  switch(ColDat.Val1){
    case INT:{
      THash<TInt,TIntV> T;  // can't really estimate the size of T for constructor hinting
      GroupByIntCol(col, T, TIntV(0));
      for(THash<TInt,TIntV>::TIter it = T.BegI(); it < T.EndI(); it++){
        RemainingRows.Add(it->Dat[0]);
      }
      break;
    }
    case FLT:{
      THash<TFlt,TIntV> T;
      GroupByFltCol(col, T, TIntV(0));
      for(THash<TFlt,TIntV>::TIter it = T.BegI(); it < T.EndI(); it++){
        RemainingRows.Add(it->Dat[0]);
      }
      break;
    }
    case STR:{
      THash<TStr,TIntV> T;
      GroupByStrCol(col, T, TIntV(0));
      for(THash<TStr,TIntV>::TIter it = T.BegI(); it < T.EndI(); it++){
        RemainingRows.Add(it->Dat[0]);
      }
      break;
    }
  }
  // with the current implementation of GroupByX, RemainingRows is sorted:
  // GroupByX returns a hash Table T:X-->TIntV. In the current implementation,
  // if key X1 appears before key X2 in T Then T(X1)[0] <= T(X2)[0]
  // Not sure if we could always make this assumption. Might want to remove this sorting..
  RemainingRows.Sort();
  KeepSortedRows(RemainingRows);
}

/* 
Q: This approach of passing a hash table by reference and adding entries to it is common here.
But it also implies certain assumptions about the input - i.e. empty input table. 
Why PHash is never used? - why not let the function allocate the new table and return a smart ptr PHash..
*/
void TTable::GroupByIntCol(TStr GroupBy, THash<TInt,TIntV>& grouping, const TIntV& IndexSet) const{
  if(!ColTypeMap.IsKey(GroupBy)){TExcept::Throw("no such column " + GroupBy);}
  TPair<TYPE,TInt> ColDat = ColTypeMap.GetDat(GroupBy);
  if(ColDat.Val1 != INT){TExcept::Throw(GroupBy + " values are not of expected type integer");}
  if(IndexSet.Len() == 0){
     // optimize for the common and most expensive case - itearte over only valid rows
    for(TRowIterator it = BegRI(); it < EndRI(); it++){
      UpdateGrouping<TInt>(grouping, it.GetIntAttr(GroupBy), it.GetRowIdx());
    }
  } else{
    // consider only rows in IndexSet
    for(TInt i = 0; i < IndexSet.Len(); i++){
      if(IsRowValid(IndexSet[i])){
        TInt RowIdx = IndexSet[i];
        const TIntV& Col = IntCols[ColDat.Val2];       
        UpdateGrouping<TInt>(grouping, Col[RowIdx], RowIdx);
      }
    }
  }
}

void TTable::GroupByFltCol(TStr GroupBy, THash<TFlt,TIntV>& grouping, const TIntV& IndexSet) const{
  if(!ColTypeMap.IsKey(GroupBy)){TExcept::Throw("no such column " + GroupBy);}
  TPair<TYPE,TInt> ColDat = ColTypeMap.GetDat(GroupBy);
  if(ColDat.Val1 != FLT){TExcept::Throw(GroupBy + " values are not of expected type float");}
   if(IndexSet.Len() == 0){
     // optimize for the common and most expensive case - itearte over only valid rows
    for(TRowIterator it = BegRI(); it < EndRI(); it++){
      UpdateGrouping<TFlt>(grouping, it.GetFltAttr(GroupBy), it.GetRowIdx());
    }
  } else{
    // consider only rows in IndexSet
    for(TInt i = 0; i < IndexSet.Len(); i++){
      if(IsRowValid(IndexSet[i])){
        TInt RowIdx = IndexSet[i];
        const TFltV& Col = FltCols[ColDat.Val2];       
        UpdateGrouping<TFlt>(grouping, Col[RowIdx], RowIdx);
      }
    }
  }
}

void TTable::GroupByStrCol(TStr GroupBy, THash<TStr,TIntV>& grouping, const TIntV& IndexSet) const{
  if(!ColTypeMap.IsKey(GroupBy)){TExcept::Throw("no such column " + GroupBy);}
  TPair<TYPE,TInt> ColDat = ColTypeMap.GetDat(GroupBy);
  if(ColDat.Val1 != STR){TExcept::Throw(GroupBy + " values are not of expected type string");}
   if(IndexSet.Len() == 0){
     // optimize for the common and most expensive case - itearte over only valid rows
    for(TRowIterator it = BegRI(); it < EndRI(); it++){
      UpdateGrouping<TStr>(grouping, it.GetStrAttr(GroupBy), it.GetRowIdx());
    }
  } else{
    // consider only rows in IndexSet
    for(TInt i = 0; i < IndexSet.Len(); i++){
      if(IsRowValid(IndexSet[i])){
        TInt RowIdx = IndexSet[i];     
        UpdateGrouping<TStr>(grouping, GetStrVal(GroupBy, RowIdx), RowIdx);
      }
    }
  }
}


void TTable::GroupAux(const TStrV& GroupBy, TInt GroupByStartIdx, THash<TInt,TIntV>& grouping, const TIntV& IndexSet){
  /* recursion base - add IndexSet as group */
  if(GroupByStartIdx == GroupBy.Len()){
    if(IndexSet.Len() == 0){return;}
	  TInt key = grouping.Len();
	  grouping.AddDat(key, IndexSet);
	  return;
  }
  if(!ColTypeMap.IsKey(GroupBy[GroupByStartIdx])){TExcept::Throw("no such column " + GroupBy[GroupByStartIdx]);}
  TPair<TYPE,TInt> GroupByColDat = ColTypeMap.GetDat(GroupBy[GroupByStartIdx]);
  switch(GroupByColDat.Val1){
    case INT:{
      // group by current column
      // not sure of to estimate the size of T for constructor hinting purpose.
      // It is bounded by the length of the IndexSet or the length of the grouping column if the IndexSet vector is empty
      // but this bound may be way too big
	    THash<TInt,TIntV> T;  
	    GroupByIntCol(GroupBy[GroupByStartIdx], T, IndexSet);
	    for(THash<TInt,TIntV>::TIter it = T.BegI(); it < T.EndI(); it++){
	      TIntV& CurrGroup = it->Dat;
        // each group according to current column will be used as an IndexSet
        // for grouping according to next column
		    GroupAux(GroupBy, GroupByStartIdx+1, grouping, CurrGroup);
	   }
	    break;
	  }
	  case FLT:{
	    THash<TFlt,TIntV> T;
	    GroupByFltCol(GroupBy[GroupByStartIdx], T, IndexSet);
	    for(THash<TFlt,TIntV>::TIter it = T.BegI(); it < T.EndI(); it++){
	      TIntV& CurrGroup = it->Dat;
		    GroupAux(GroupBy, GroupByStartIdx+1, grouping, CurrGroup);
	    }
	    break;
	  }
	  case STR:{
	    THash<TStr,TIntV> T;
	    GroupByStrCol(GroupBy[GroupByStartIdx], T, IndexSet);
	    for(THash<TStr,TIntV>::TIter it = T.BegI(); it < T.EndI(); it++){
	      TIntV& CurrGroup = it->Dat;
	      GroupAux(GroupBy, GroupByStartIdx+1, grouping, CurrGroup);
	    }
	    break;
	  }
  }
}

void TTable::StoreGroupCol(TStr GroupColName, const THash<TInt,TIntV>& grouping){
  GroupMapping.AddDat(GroupColName, grouping);
}

void TTable::Group(TStr GroupColName, const TStrV& GroupBy){
  THash<TInt,TIntV> grouping;
  GroupAux(GroupBy, 0, grouping, TIntV(0));
  StoreGroupCol(GroupColName, grouping);
}

void TTable::Count(TStr CountColName, TStr Col){
  if(!ColTypeMap.IsKey(Col)){TExcept::Throw("no such column " + Col);}
  TPair<TYPE,TInt> ColDat = ColTypeMap.GetDat(Col);
  TIntV CntCol(NumRows);
  switch(ColDat.Val1){
    case INT:{
      THash<TInt,TIntV> T;  // can't really estimate the size of T for constructor hinting
      TIntV& Column = IntCols[ColDat.Val2];
      GroupByIntCol(Col, T, TIntV(0));
      for(TRowIterator it = BegRI(); it < EndRI(); it++){
        CntCol.Add(T.GetDat(Column[it.GetRowIdx()]).Len());
      }
      break;
    }
    case FLT:{
      THash<TFlt,TIntV> T;
      TFltV& Column = FltCols[ColDat.Val2];
      GroupByFltCol(Col, T, TIntV(0));
      for(TRowIterator it = BegRI(); it < EndRI(); it++){
        CntCol.Add(T.GetDat(Column[it.GetRowIdx()]).Len());
      }
      break;
    }
    case STR:{
      THash<TStr,TIntV> T;
      GroupByStrCol(Col, T, TIntV(0));
      for(TRowIterator it = BegRI(); it < EndRI(); it++){
        CntCol.Add(T.GetDat(GetStrVal(Col, it.GetRowIdx())).Len());
      }
      break;
    }
  }
  IntCols.Add(CntCol);
  ColTypeMap.AddDat(Col, TPair<TYPE,TInt>(INT, IntCols.Len()));
}

 PTable TTable::InitializeJointTable(const TTable& Table){
  PTable JointTable = New();
  JointTable->Name = Name + "_" + Table.Name;
  JointTable->IntCols = TVec<TIntV>(IntCols.Len() + Table.IntCols.Len());
  JointTable->FltCols = TVec<TFltV>(FltCols.Len() + Table.FltCols.Len());
  JointTable->StrColMaps = TVec<TIntV>(StrColMaps.Len() + Table.StrColMaps.Len());
  for(THash<TStr,TPair<TYPE,TInt> >::TIter it = ColTypeMap.BegI(); it < ColTypeMap.EndI(); it++){
    TStr CName = Name + "." + it->Key;
    JointTable->ColTypeMap.AddDat(CName, it->Dat);
    JointTable->AddLabel(CName, it->Key);
  }
  for(THash<TStr,TPair<TYPE,TInt> >::TIter it = Table.ColTypeMap.BegI(); it < Table.ColTypeMap.EndI(); it++){
    TStr CName = Name + "." + it->Key;
    TPair<TYPE, TInt> NewDat = it->Dat;
    switch(NewDat.Val1){
      case INT:
        NewDat.Val2 += IntCols.Len();
        break;
      case FLT:
        NewDat.Val2 += FltCols.Len();
        break;
      case STR:
        NewDat.Val2 += StrColMaps.Len();
        break;
    }
    JointTable->ColTypeMap.AddDat(CName, NewDat);
    JointTable->AddLabel(CName, it->Key);
    return JointTable;
  }
 }

 void TTable::AddJointRow(const TTable& T1, const TTable& T2, TInt RowIdx1, TInt RowIdx2){
   for(TInt i = 0; i < T1.IntCols.Len(); i++){
     IntCols[i].Add(T1.IntCols[i][RowIdx1]);
   }
   for(TInt i = 0; i < T1.FltCols.Len(); i++){
     FltCols[i].Add(T1.FltCols[i][RowIdx1]);
   }
   for(TInt i = 0; i < T1.StrColMaps.Len(); i++){
     AddStrVal(i, GetStrVal(i, RowIdx1));
   }
   TInt IntOffset = T1.IntCols.Len();
   TInt FltOffset = T1.FltCols.Len();
   TInt StrOffset = T1.StrColMaps.Len();
   for(TInt i = 0; i < T2.IntCols.Len(); i++){
     IntCols[i+IntOffset].Add(T2.IntCols[i][RowIdx2]);
   }
   for(TInt i = 0; i < T2.FltCols.Len(); i++){
     FltCols[i+FltOffset].Add(T2.FltCols[i][RowIdx2]);
   }
   for(TInt i = 0; i < T2.StrColMaps.Len(); i++){
     AddStrVal(i+StrOffset, GetStrVal(i, RowIdx2));
   }
 }

PTable TTable::Join(TStr Col1, const TTable& Table, TStr Col2) {
  if(!ColTypeMap.IsKey(Col1)){
    TExcept::Throw("no such column " + Col1);
  }
  if(!ColTypeMap.IsKey(Col2)){
    TExcept::Throw("no such column " + Col2);
  }
  TPair<TYPE,TInt> Col1Dat = ColTypeMap.GetDat(Col1);
  TPair<TYPE,TInt> Col2Dat = ColTypeMap.GetDat(Col2);
  if (Col1Dat.Val1 != Col2Dat.Val1) {
    TExcept::Throw("Trying to Join on columns of different type");
  }
  // initialize result table
  PTable JointTable = InitializeJointTable(Table);
  // hash smaller table (group by column)
  TYPE ColType = Col1Dat.Val1;
  TBool ThisIsSmaller = (NumRows <= Table.NumRows);
  const TTable& TS = ThisIsSmaller ? *this : Table;
  const TTable& TB = ThisIsSmaller ?  Table : *this;
  TStr ColS = ThisIsSmaller ? Col1 : Col2;
  TStr ColB = ThisIsSmaller ? Col2 : Col1;
  // iterate over the rows of the bigger table and check for "collisions" 
  // with the group keys for the small table.
  switch(ColType){
    case INT:{
      THash<TInt, TIntV> T;
      TS.GroupByIntCol(Col1, T, TIntV());
      for(TRowIterator RowI = TB.BegRI(); RowI < TB.EndRI(); RowI++){
        TInt K = RowI.GetIntAttr(ColB);
        if(T.IsKey(K)){
          TIntV& Group = T.GetDat(K);
          for(TInt i = 0; i < Group.Len(); i++){
            JointTable->AddJointRow(*this, Table, Group[i], RowI.GetRowIdx());
          }
        }
      }
    }
    case FLT:{
      THash<TFlt, TIntV> T;
      TS.GroupByFltCol(Col1, T, TIntV());
      for(TRowIterator RowI = TB.BegRI(); RowI < TB.EndRI(); RowI++){
        TFlt K = RowI.GetFltAttr(ColB);
        if(T.IsKey(K)){
          TIntV& Group = T.GetDat(K);
          for(TInt i = 0; i < Group.Len(); i++){
            JointTable->AddJointRow(*this, Table, Group[i], RowI.GetRowIdx());
          }
        }
      }
    }
    case STR:{
      THash<TStr, TIntV> T;
      TS.GroupByStrCol(Col1, T, TIntV());
      for(TRowIterator RowI = TB.BegRI(); RowI < TB.EndRI(); RowI++){
        TStr K = RowI.GetStrAttr(ColB);
        if(T.IsKey(K)){
          TIntV& Group = T.GetDat(K);
          for(TInt i = 0; i < Group.Len(); i++){
            JointTable->AddJointRow(*this, Table, Group[i], RowI.GetRowIdx());
          }
        }
      }
    }
  }
 return JointTable; 
}

void TTable::Dist(TStr Col1, const TTable& Table, TStr Col2, TStr DistColName, const TMetric& Metric, TFlt threshold) {
  if(!ColTypeMap.IsKey(Col1)){
    TExcept::Throw("no such column " + Col1);
  }
  if(!ColTypeMap.IsKey(Col2)){
    TExcept::Throw("no such column " + Col2);
  }
  TYPE T1 = GetColType(Col1);
  //CHECK do we need to make this a const?
  /*
  TYPE T2 = Table.GetColType(Col2);
  if ((T1  == STR && T2 != STR) || (T1  != STR && T2 == STR) ) {
    TExcept::Throw("Trying to compare strings with numbers.");
  }
  */
}

void TTable::Select(TPredicate& Predicate){
  TIntV Selected;
  TStrV RelevantCols;
  Predicate.GetVariables(RelevantCols);
  for(TRowIterator RowI = BegRI(); RowI < EndRI(); RowI++){
    // prepare arguments for 
    for(TInt i = 0; i < RelevantCols.Len(); i++){
      switch(ColTypeMap.GetDat(RelevantCols[i]).Val1){
      case INT:
        Predicate.SetIntVal(RelevantCols[i], RowI.GetIntAttr(RelevantCols[i]));
        break;
      case FLT:
        Predicate.SetFltVal(RelevantCols[i], RowI.GetFltAttr(RelevantCols[i]));
        break;
      case STR:
        Predicate.SetStrVal(RelevantCols[i], RowI.GetStrAttr(RelevantCols[i]));
        break;
      }
    }
    if(Predicate.Eval()){ Selected.Add(RowI.GetRowIdx());}
  }
  KeepSortedRows(Selected);
}

void TTable::BuildGraphTopology(PNEAGraph& Graph, THash<TFlt, TInt>& FSrNodeMap, THash<TFlt, TInt>& FDsNodeMap) {
  TYPE SrCT = ColTypeMap.GetDat(SrcCol).Val1;
  TInt SrIdx = ColTypeMap.GetDat(SrcCol).Val2;
  TYPE DsCT = ColTypeMap.GetDat(DstCol).Val1;
  TInt DsIdx = ColTypeMap.GetDat(DstCol).Val2;
  TInt SrcCnt = 0;
  TInt DstCnt = 0;
  
  for(TRowIterator RowI = BegRI(); RowI < EndRI(); RowI++) {
    if (SrCT == INT && DsCT == INT) {
      Graph->AddNode(IntCols[SrIdx][RowI.GetRowIdx()]);
      Graph->AddNode(IntCols[DsIdx][RowI.GetRowIdx()]);
      Graph->AddEdge(IntCols[SrIdx][RowI.GetRowIdx()], IntCols[DsIdx][RowI.GetRowIdx()], RowI.GetRowIdx());
    } else if (SrCT == INT && DsCT == FLT) {
      Graph->AddNode(IntCols[SrIdx][RowI.GetRowIdx()]);
      TFlt val = FltCols[DsIdx][RowI.GetRowIdx()];
      if (!FDsNodeMap.IsKey(val)) {
	FDsNodeMap.AddDat(val, DstCnt++);
      }
      Graph->AddNode(FDsNodeMap.GetDat(val));
      Graph->AddEdge(IntCols[SrIdx][RowI.GetRowIdx()], FDsNodeMap.GetDat(val));
    } else if (SrCT == INT && DsCT == STR) {
      Graph->AddNode(IntCols[SrIdx][RowI.GetRowIdx()]);
      Graph->AddNode(StrColMaps[DsIdx][RowI.GetRowIdx()]);
      Graph->AddEdge(IntCols[SrIdx][RowI.GetRowIdx()], StrColMaps[DsIdx][RowI.GetRowIdx()], RowI.GetRowIdx());
    } else if (SrCT == FLT && DsCT == INT) {
      Graph->AddNode(IntCols[DsIdx][RowI.GetRowIdx()]);
      TFlt val = FltCols[SrIdx][RowI.GetRowIdx()];
      if (!FSrNodeMap.IsKey(val)) {
	FSrNodeMap.AddDat(val, SrcCnt++);
      }
      Graph->AddNode(FSrNodeMap.GetDat(val));
      Graph->AddEdge(FSrNodeMap.GetDat(val), IntCols[SrIdx][RowI.GetRowIdx()], RowI.GetRowIdx());
    } else if (SrCT == FLT && DsCT == STR) {
      Graph->AddNode(StrColMaps[DsIdx][RowI.GetRowIdx()]);
      TFlt val = FltCols[SrIdx][RowI.GetRowIdx()];
      if (!FSrNodeMap.IsKey(val)) {
	FSrNodeMap.AddDat(val, SrcCnt++);
      }
      Graph->AddNode(FSrNodeMap.GetDat(val));
      Graph->AddEdge(FSrNodeMap.GetDat(val), IntCols[SrIdx][RowI.GetRowIdx()], RowI.GetRowIdx());
    } else if (SrCT == FLT && DsCT == FLT) {
      TFlt val = FltCols[SrIdx][RowI.GetRowIdx()];
      if (!FSrNodeMap.IsKey(val)) {
	FSrNodeMap.AddDat(val, SrcCnt++);
      }
      Graph->AddNode(FSrNodeMap.GetDat(val));
      val = FltCols[DsIdx][RowI.GetRowIdx()];
      if (!FDsNodeMap.IsKey(val)) {
	FDsNodeMap.AddDat(val, DstCnt++);
      }
      Graph->AddNode(FDsNodeMap.GetDat(val));
      Graph->AddEdge(FSrNodeMap.GetDat(val), FDsNodeMap.GetDat(val), RowI.GetRowIdx());
    }
  }
}

TInt TTable::GetNId(TStr Col, TInt RowIdx, THash<TFlt, TInt>& FSrNodeMap, THash<TFlt, TInt>& FDsNodeMap) {
  TYPE CT = ColTypeMap.GetDat(Col).Val1;
  TInt Idx = ColTypeMap.GetDat(Col).Val2;
  if (CT == INT) {
    return IntCols[RowIdx][Idx];
  } else if (CT == FLT) {
    if (Col == SrcCol) {
      return FSrNodeMap.GetDat(FltCols[Idx][RowIdx]);
    } else if (Col == DstCol) {
      return FDsNodeMap.GetDat(FltCols[Idx][RowIdx]);
    } else {
      TExcept::Throw("Column " + Col + " is not source node or destination column");
    }
  } else {
    return StrColMaps[RowIdx][Idx]();
  }
}
    
void TTable::AddNodeAttributes(PNEAGraph& Graph, THash<TFlt, TInt>& FSrNodeMap, THash<TFlt, TInt>& FDsNodeMap) {
  for (TRowIterator RowI = BegRI(); RowI < EndRI(); RowI++) {
    // Add Source and Destination node attributes.
    for (int i = 0; i < SrcNodeAttrV.Len(); i++) {
      TStr SrcColAttr = SrcNodeAttrV[i];
      TYPE CT = ColTypeMap.GetDat(SrcColAttr).Val1;
      int Idx = ColTypeMap.GetDat(SrcColAttr).Val2;
      TInt RowIdx  = RowI.GetRowIdx();
      if (CT == INT) {
	Graph->AddIntAttrDatN(GetNId(SrcCol, RowIdx, FSrNodeMap, FDsNodeMap), IntCols[Idx][RowIdx], SrcColAttr);
      } else if (CT == FLT) {
	Graph->AddFltAttrDatN(GetNId(SrcCol, RowIdx, FSrNodeMap, FDsNodeMap), FltCols[Idx][RowIdx], SrcColAttr);
      } else {
	Graph->AddStrAttrDatN(GetNId(SrcCol, RowIdx, FSrNodeMap, FDsNodeMap), StrColVals.GetStr(StrColMaps[Idx][RowIdx]), SrcColAttr);
      }
    }

    for (int i = 0; i < DstNodeAttrV.Len(); i++) {
      TStr DstColAttr = DstNodeAttrV[i];
      TYPE CT = ColTypeMap.GetDat(DstColAttr).Val1;
      int Idx = ColTypeMap.GetDat(DstColAttr).Val2;
      TInt RowIdx  = RowI.GetRowIdx();
      if (CT == INT) {
	Graph->AddIntAttrDatN(GetNId(SrcCol, RowIdx, FSrNodeMap, FDsNodeMap), IntCols[Idx][RowIdx], DstColAttr);
      } else if (CT == FLT) {
	Graph->AddFltAttrDatN(GetNId(SrcCol, RowIdx, FSrNodeMap, FDsNodeMap), FltCols[Idx][RowIdx], DstColAttr);
      } else {
	Graph->AddStrAttrDatN(GetNId(SrcCol, RowIdx, FSrNodeMap, FDsNodeMap), StrColVals.GetStr(StrColMaps[Idx][RowIdx]), DstColAttr);
      }
    }
  }
}

void TTable::AddEdgeAttributes(PNEAGraph& Graph) {
  for(TRowIterator RowI = BegRI(); RowI < EndRI(); RowI++) {
    typedef THash<TStr,TPair<TYPE,TInt> >::TIter TColIter;
    for (int i = 0; i < EdgeAttrV.Len(); i++) {
      TStr ColName = EdgeAttrV[i];
      TYPE T = ColTypeMap.GetDat(ColName).Val1;
      TInt Index = ColTypeMap.GetDat(ColName).Val2;
      TInt Ival;
      TFlt Fval;
      TStr Sval;
      switch (T) {
      case INT:
	Ival = IntCols[Index][RowI.GetRowIdx()];
	Graph->AddIntAttrDatE(RowI.GetRowIdx(), Ival, ColName);
	break;
      case FLT:
	Fval = FltCols[Index][RowI.GetRowIdx()];
	Graph->AddFltAttrDatE(RowI.GetRowIdx(), Fval, ColName);
	break;
      case STR:
	Sval = StrColVals.GetStr(StrColMaps[Index][RowI.GetRowIdx()]);
	Graph->AddStrAttrDatE(RowI.GetRowIdx(), Sval, ColName);
	break;
      }
    }
  }
}

PNEAGraph TTable::ToGraph() {
  PNEAGraph Graph = PNEAGraph::New();
  THash<TFlt, TInt> FSrNodeMap = THash<TFlt, TInt>();
  THash<TFlt, TInt> FDsNodeMap = THash<TFlt, TInt>();
  BuildGraphTopology(Graph, FSrNodeMap, FDsNodeMap);
  AddEdgeAttributes(Graph);
  AddNodeAttributes(Graph, FSrNodeMap, FDsNodeMap);
  return Graph;
}

