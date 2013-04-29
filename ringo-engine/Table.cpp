#include "Table.h"

TInt const TTable::Last =-1;
TInt const TTable::Invalid =-2;

TStrV TTable::GetNodeIntAttrV() const {
  TStrV IntNA = TStrV(IntCols.Len(),0);
  for (int i = 0; i < NodeAttrV.Len(); i++) {
    TStr Attr = NodeAttrV[i];
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

TStrV TTable::GetNodeFltAttrV() const {
  TStrV FltNA = TStrV(FltCols.Len(),0);
  for (int i = 0; i < NodeAttrV.Len(); i++) {
    TStr Attr = NodeAttrV[i];
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

TStrV TTable::GetNodeStrAttrV() const {
  TStrV StrNA = TStrV(StrCols.Len(),0);
  for (int i = 0; i < NodeAttrV.Len(); i++) {
    TStr Attr = NodeAttrV[i];
    if (ColTypeMap.GetDat(Attr).Val1 == STR) {
      StrNA.Add(Attr);
    }
  }
  return StrNA;
}

TStrV TTable::GetEdgeStrAttrV() const {
  TStrV StrEA = TStrV(StrCols.Len(),0);
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
void TTable::GroupByIntCol(TStr GroupBy, THash<TInt,TIntV>& grouping, const TIntV& IndexSet){
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
        TIntV& Col = IntCols[ColDat.Val2];       
        UpdateGrouping<TInt>(grouping, Col[RowIdx], RowIdx);
      }
    }
  }
}

void TTable::GroupByFltCol(TStr GroupBy, THash<TFlt,TIntV>& grouping, const TIntV& IndexSet){
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
        TFltV& Col = FltCols[ColDat.Val2];       
        UpdateGrouping<TFlt>(grouping, Col[RowIdx], RowIdx);
      }
    }
  }
}

void TTable::GroupByStrCol(TStr GroupBy, THash<TStr,TIntV>& grouping, const TIntV& IndexSet){
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
        TStrV& Col = StrCols[ColDat.Val2];       
        UpdateGrouping<TStr>(grouping, Col[RowIdx], RowIdx);
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
      TStrV& Column = StrCols[ColDat.Val2];
      GroupByStrCol(Col, T, TIntV(0));
      for(TRowIterator it = BegRI(); it < EndRI(); it++){
        CntCol.Add(T.GetDat(Column[it.GetRowIdx()]).Len());
      }
      break;
    }
  }
  IntCols.Add(CntCol);
  ColTypeMap.AddDat(Col, TPair<TYPE,TInt>(INT, IntCols.Len()));
}

