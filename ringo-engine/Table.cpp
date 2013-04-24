#include "Table.h"

void TTable::ChangeWorkingCol(TStr column){
  if(!ColTypeMap.IsKey(column)){TExcept::Throw("no such column " + column);}
  WorkingCol = column;
}

void TTable::AddLabel(TStr column, TStr newLabel){
  if(!ColTypeMap.IsKey(column)){TExcept::Throw("no such column " + column);}
  TPair<TYPE,TInt> ColVal = ColTypeMap.GetDat(column);
  ColTypeMap.AddDat(newLabel,ColVal);
}

void TTable::RemoveSortedRows(const TIntV& remove){
  RemoveSortedRowsIntCols(remove);
  RemoveSortedRowsFltCols(remove);
  RemoveSortedRowsStrCols(remove);
}

void TTable::RemoveSortedRowsIntCols(const TIntV& remove){
  // In place deletion of rows is expensive - If we need to remove many rows
  // - create a new copy of the reduced column
  if(remove.Len() > CopyOnRemoveThreshold*NumRows){
    for(TInt i = 0; i < IntCols.Len(); i++){
      TIntV NewCol;
      TInt k = 0;
      for(TInt j = 0; j < NumRows; j++){
        if(j != remove[k]){
          NewCol.Add(IntCols[i][j]);
        } else{
          k++;
        }
      }
      IntCols[i] = NewCol;
    }
  // if we only need to delete a few rows - delete in-place to save memory
  } else{
      for(TInt i = 0; i < IntCols.Len(); i++){
        for(TInt k = remove.Len()-1; k >= 0; k--){
          IntCols[i].Del(remove[k]);
        }
      }
  }
}

void TTable::RemoveSortedRowsFltCols(const TIntV& remove){
  if(remove.Len() > CopyOnRemoveThreshold*NumRows){
    for(TInt i = 0; i < FltCols.Len(); i++){
      TFltV NewCol;
      TInt k = 0;
      for(TInt j = 0; j < NumRows; j++){
        if(j != remove[k]){
          NewCol.Add(FltCols[i][j]);
        } else{
          k++;
        }
      }
      FltCols[i] = NewCol;
    }
  } else{
      for(TInt i = 0; i < FltCols.Len(); i++){
        for(TInt k = remove.Len()-1; k >= 0; k--){
          FltCols[i].Del(remove[k]);
        }
      }
  }
}

void TTable::RemoveSortedRowsStrCols(const TIntV& remove){
  if(remove.Len() > CopyOnRemoveThreshold*NumRows){
    for(TInt i = 0; i < StrCols.Len(); i++){
      TStrV NewCol;
      TInt k = 0;
      for(TInt j = 0; j < NumRows; j++){
        if(j != remove[k]){
          NewCol.Add(StrCols[i][j]);
        } else{
          k++;
        }
      }
      StrCols[i] = NewCol;
    }
  } else{
      for(TInt i = 0; i < StrCols.Len(); i++){
        for(TInt k = remove.Len()-1; k >= 0; k--){
          StrCols[i].Del(remove[k]);
        }
      }
  }
}

void TTable::KeepSortedRows(const TIntV& keep){
  KeepSortedRowsIntCols(keep);
  KeepSortedRowsFltCols(keep);
  KeepSortedRowsStrCols(keep);
}

void TTable::KeepSortedRowsIntCols(const TIntV& keep){
  // If we need to keep only a few rows - create a new copy of the reduced column
  if(keep.Len() <= (1-CopyOnRemoveThreshold)*NumRows){
    for(TInt i = 0; i < IntCols.Len(); i++){
      TIntV NewCol;
      TInt k = 0;
      for(TInt k = 0; k < NumRows; k++){
        NewCol.Add(IntCols[i][k]);
      }
      IntCols[i] = NewCol;
    }
  // if we keep many rows - delete in-place the ones we don't keep
  } else{
      for(TInt i = 0; i < IntCols.Len(); i++){
        TInt k = 0;
        for(TInt j = 0; j < NumRows; j++){
          if(j != keep[k]){
            IntCols[i].Del(j);
          } else{
            k++;
          }
        }
      }
  }
}

void TTable::KeepSortedRowsFltCols(const TIntV& keep){
  if(keep.Len() <= (1-CopyOnRemoveThreshold)*NumRows){
    for(TInt i = 0; i < FltCols.Len(); i++){
      TFltV NewCol;
      TInt k = 0;
      for(TInt k = 0; k < NumRows; k++){
        NewCol.Add(FltCols[i][k]);
      }
      FltCols[i] = NewCol;
    }
  } else{
      for(TInt i = 0; i < FltCols.Len(); i++){
        TInt k = 0;
        for(TInt j = 0; j < NumRows; j++){
          if(j != keep[k]){
            FltCols[i].Del(j);
          } else{
            k++;
          }
        }
      }
  }
}

void TTable::KeepSortedRowsStrCols(const TIntV& keep){
  if(keep.Len() <= (1-CopyOnRemoveThreshold)*NumRows){
    for(TInt i = 0; i < StrCols.Len(); i++){
      TStrV NewCol;
      TInt k = 0;
      for(TInt k = 0; k < NumRows; k++){
        NewCol.Add(StrCols[i][k]);
      }
      StrCols[i] = NewCol;
    }
  } else{
      for(TInt i = 0; i < StrCols.Len(); i++){
        TInt k = 0;
        for(TInt j = 0; j < NumRows; j++){
          if(j != keep[k]){
            StrCols[i].Del(j);
          } else{
            k++;
          }
        }
      }
  }
}

void TTable::Unique(TStr col){
  if(!ColTypeMap.IsKey(col)){TExcept::Throw("no such column " + col);}
  TIntV RemainingRows;
  TPair<TYPE,TInt> ColDat = ColTypeMap.GetDat(col);
  // group by given column (keys) and keep only first row for each key
  switch(ColDat.Val1){
    case INT:{
      THash<TInt,TIntV> T;
      GroupByIntCol(col, T, NULL);
      for(THash<TInt,TIntV>::TIter it = T.BegI(); it < T.EndI(); it++){
        RemainingRows.Add(it->Dat[0]);
      }
      break;
    }
    case FLT:{
      THash<TFlt,TIntV> T;
      GroupByFltCol(col, T, NULL);
      for(THash<TFlt,TIntV>::TIter it = T.BegI(); it < T.EndI(); it++){
        RemainingRows.Add(it->Dat[0]);
      }
      break;
    }
    case STR:{
      THash<TStr,TIntV> T;
      GroupByStrCol(col, T, NULL);
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
void TTable::GroupByIntCol(TStr GroupBy, THash<TInt,TIntV>& grouping, const TIntV* IndexSet){
  if(!ColTypeMap.IsKey(GroupBy)){TExcept::Throw("no such column " + GroupBy);}
  TPair<TYPE,TInt> ColDat = ColTypeMap.GetDat(GroupBy);
  if(ColDat.Val1 != INT){TExcept::Throw(GroupBy + " values are not of expected type integer");}
  TIntV& Col = IntCols[ColDat.Val2];
  // consider only rows in IndexSet
  TInt NumOfRelevantRows = (IndexSet == NULL) ? NumRows : IndexSet->Len();
  for(TInt i = 0; i < NumOfRelevantRows; i++){
    TInt idx = (IndexSet == NULL)  ? i : (*IndexSet)[i];
	  TInt key = Col[idx];
	  if(grouping.IsKey(key)){
	    grouping.GetDat(key).Add(idx);
	  } else{
	    TIntV newGroup;
	    newGroup.Add(idx);
	    grouping.AddDat(key,newGroup);
	  }
  }
}

void TTable::GroupByFltCol(TStr GroupBy, THash<TFlt,TIntV>& grouping, const TIntV* IndexSet){
  if(!ColTypeMap.IsKey(GroupBy)){TExcept::Throw("no such column " + GroupBy);}
  TPair<TYPE,TInt> ColDat = ColTypeMap.GetDat(GroupBy);
  if(ColDat.Val1 != FLT){TExcept::Throw(GroupBy + " values are not of expected type float");}
  TFltV& Col = FltCols[ColDat.Val2];
  TInt NumOfRelevantRows = (IndexSet == NULL) ? NumRows : IndexSet->Len();
  for(TInt i = 0; i < NumOfRelevantRows; i++){
    TInt idx = (IndexSet == NULL)  ? i : (*IndexSet)[i];
	  TFlt key = Col[idx];
	  if(grouping.IsKey(key)){
	    grouping.GetDat(key).Add(idx);
	  } else{
	    TIntV newGroup;
	    newGroup.Add(idx);
	    grouping.AddDat(key,newGroup);
	  }
  }
}

void TTable::GroupByStrCol(TStr GroupBy, THash<TStr,TIntV>& grouping, const TIntV* IndexSet){
  if(!ColTypeMap.IsKey(GroupBy)){TExcept::Throw("no such column " + GroupBy);}
  TPair<TYPE,TInt> ColDat = ColTypeMap.GetDat(GroupBy);
  if(ColDat.Val1 != STR){TExcept::Throw(GroupBy + " values are not of expected type string");}
  TStrV& Col = StrCols[ColDat.Val2];
  TInt NumOfRelevantRows = (IndexSet == NULL) ? NumRows : IndexSet->Len();
  for(TInt i = 0; i < NumOfRelevantRows; i++){
    TInt idx = (IndexSet == NULL)  ? i : (*IndexSet)[i];
	TStr key = Col[idx];
	if(grouping.IsKey(key)){
	  grouping.GetDat(key).Add(idx);
	} else{
	  TIntV newGroup;
	  newGroup.Add(idx);
	  grouping.AddDat(key,newGroup);
	}
  }
}


void TTable::GroupAux(const TStrV& GroupBy, TInt GroupByStartIdx, THash<TInt,TIntV>& grouping, const TIntV* IndexSet){
  /* recursion base - add IndexSet as group */
  if(GroupByStartIdx == GroupBy.Len()){
    if(IndexSet == NULL){return;}
	  TInt key = grouping.Len();
	  grouping.AddDat(key, *IndexSet);
	  return;
  }
  if(!ColTypeMap.IsKey(GroupBy[GroupByStartIdx])){TExcept::Throw("no such column " + GroupBy[GroupByStartIdx]);}
  TPair<TYPE,TInt> GroupByColDat = ColTypeMap.GetDat(GroupBy[GroupByStartIdx]);
  switch(GroupByColDat.Val1){
    case INT:{
      // group by current column
	    THash<TInt,TIntV> T;
	    GroupByIntCol(GroupBy[GroupByStartIdx], T, IndexSet);
	    for(THash<TInt,TIntV>::TIter it = T.BegI(); it < T.EndI(); it++){
	      TIntV& CurrGroup = it->Dat;
        // each group according to current column will be used as an IndexSet
        // for grouping according to next column
		    GroupAux(GroupBy, GroupByStartIdx+1, grouping, &CurrGroup);
	   }
	    break;
	  }
	  case FLT:{
	    THash<TFlt,TIntV> T;
	    GroupByFltCol(GroupBy[GroupByStartIdx], T, IndexSet);
	    for(THash<TFlt,TIntV>::TIter it = T.BegI(); it < T.EndI(); it++){
	      TIntV& CurrGroup = it->Dat;
		    GroupAux(GroupBy, GroupByStartIdx+1, grouping, &CurrGroup);
	    }
	    break;
	  }
	  case STR:{
	    THash<TStr,TIntV> T;
	    GroupByStrCol(GroupBy[GroupByStartIdx], T, IndexSet);
	    for(THash<TStr,TIntV>::TIter it = T.BegI(); it < T.EndI(); it++){
	      TIntV& CurrGroup = it->Dat;
	      GroupAux(GroupBy, GroupByStartIdx+1, grouping, &CurrGroup);
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
  GroupAux(GroupBy, 0, grouping, NULL);
  StoreGroupCol(GroupColName, grouping);
}

void TTable::Count(TStr CountColName, TStr Col){
  if(!ColTypeMap.IsKey(Col)){TExcept::Throw("no such column " + Col);}
  TPair<TYPE,TInt> ColDat = ColTypeMap.GetDat(Col);
  TIntV CntCol(NumRows);
  switch(ColDat.Val1){
    case INT:{
      THash<TInt,TIntV> T;
      TIntV& Column = IntCols[ColDat.Val2];
      GroupByIntCol(Col, T, NULL);
      for(TInt i = 0; i < NumRows; i++){
        CntCol.Add(T.GetDat(Column[i]).Len());
      }
      break;
    }
    case FLT:{
      THash<TFlt,TIntV> T;
      TFltV& Column = FltCols[ColDat.Val2];
      GroupByFltCol(Col, T, NULL);
      for(TInt i = 0; i < NumRows; i++){
        CntCol.Add(T.GetDat(Column[i]).Len());
      }
      break;
    }
    case STR:{
      THash<TStr,TIntV> T;
      TStrV& Column = StrCols[ColDat.Val2];
      GroupByStrCol(Col, T, NULL);
      for(TInt i = 0; i < NumRows; i++){
        CntCol.Add(T.GetDat(Column[i]).Len());
      }
      break;
    }
  }
  IntCols.Add(CntCol);
  ColTypeMap.AddDat(Col, TPair<TYPE,TInt>(INT, IntCols.Len()));
}

