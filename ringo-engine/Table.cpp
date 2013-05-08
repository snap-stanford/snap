#include "Table.h"

TInt const TTable::Last =-1;
TInt const TTable::Invalid =-2;

TTable::TTable(const TStr& TableName, const Schema& TableSchema): Name(TableName),
  S(TableSchema), NumRows(0), NumValidRows(0), FirstValidRow(0), NumOfDistinctStrVals(0){
  TInt IntColCnt = 0;
  TInt FltColCnt = 0;
  TInt StrColCnt = 0;
  for(TInt i = 0; i < S.Len(); i++){
    TStr ColName = GetSchemaColName(i);
    TYPE ColType = GetSchemaColType(i);
    switch(ColType){
      case INT:
        ColTypeMap.AddDat(ColName, TPair<TYPE,TInt>(INT, IntColCnt));
        IntColCnt++;
        break;
      case FLT:
        ColTypeMap.AddDat(ColName, TPair<TYPE,TInt>(FLT, FltColCnt));
        FltColCnt++;
        break;
      case STR:
        ColTypeMap.AddDat(ColName, TPair<TYPE,TInt>(STR, StrColCnt));
        StrColCnt++;
        break;
    }
  }
  IntCols = TVec<TIntV>(IntColCnt);
  FltCols = TVec<TFltV>(FltColCnt);
  StrColMaps = TVec<TIntV>(StrColCnt);
}

PTable TTable::LoadSS(const TStr& TableName, const Schema& S, const TStr& InFNm, const char& Separator, TBool HasTitleLine){
  TSsParser Ss(InFNm, Separator);
  PTable T = New(TableName, S);
  // if title line (i.e. names of the columns) is included as first roe in the
  // input file - vuse it to validate schema
  if(HasTitleLine){
    Ss.Next();
    if(S.Len() != Ss.Len()){TExcept::Throw("Table Schema Mismatch!");}
    for(TInt i = 0; i < S.Len(); i++){
      // remove carriage return char
      TInt L = strlen(Ss[i]);
      if(Ss[i][L-1] < ' '){ Ss[i][L-1] = 0;}
      if(T->GetSchemaColName(i) != Ss[i]){
        TExcept::Throw("Table Schema Mismatch!");
      }
    }
  }
  // populate table columns
  while(Ss.Next()){
    Assert(Ss.Len() == S.Len());
    for(TInt i = 0; i < S.Len(); i++){
      TInt ColIdx = T->GetColIdx(T->GetSchemaColName(i));
      switch(T->GetSchemaColType(i)){
        case INT:
          T->IntCols[ColIdx].Add(Ss.GetInt(i));
          break;
        case FLT:
          T->FltCols[ColIdx].Add(Ss.GetFlt(i));
          break;
        case STR:
          T->AddStrVal(ColIdx, Ss[i]);
          break;
      }
    }
  }

  // set number of rows and "Next" vector
  T->NumRows = Ss.GetLineNo()-1;
  if(HasTitleLine){T->NumRows--;}
  T->NumValidRows = T->NumRows;
  T->Next = TIntV(T->NumRows,0);
  for(TInt i = 0; i < T->NumRows-1; i++){
    T->Next.Add(i+1);
  }
  T->Next.Add(Last);
  return T;
}

void TTable::SaveSS(const TStr& OutFNm){
  FILE* F = fopen(OutFNm.CStr(), "wt");
  TInt L = S.Len();
  // print title (schema)
  for(TInt i = 0; i < L-1; i++){
    fprintf(F, "%s\t", GetSchemaColName(i).CStr());
  }  
   fprintf(F, "%s\n", GetSchemaColName(L-1).CStr());
  // print table contents
  for(TRowIterator RowI = BegRI(); RowI < EndRI(); RowI++){
    for(TInt i = 0; i < L; i++){
      char C = (i == L-1) ? '\n' : '\t';
	    switch(GetSchemaColType(i)){
	      case INT:{
		    fprintf(F, "%d%c", RowI.GetIntAttr(GetSchemaColName(i)).Val, C);
		    break;
		  }
	     case FLT:{
		    fprintf(F, "%f%c", RowI.GetFltAttr(GetSchemaColName(i)).Val, C);
		    break;
		  }
	    case STR:{
		    fprintf(F, "%s%c", RowI.GetStrAttr(GetSchemaColName(i)).CStr(), C);
		    break;
		  }
	   }
	}
 }
 fclose(F);
}

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
  if(GetColType(Col) != STR){TExcept::Throw(Col + " is not a string valued column");}
  AddStrVal(GetColIdx(Col), Val);
}

TStrV TTable::GetSrcNodeIntAttrV() const {
  TStrV IntNA = TStrV(IntCols.Len(),0);
  for (int i = 0; i < SrcNodeAttrV.Len(); i++) {
    TStr Attr = SrcNodeAttrV[i];
    if (GetColType(Attr) == INT) {
      IntNA.Add(Attr);
    }
  }
  return IntNA;
}

TStrV TTable::GetDstNodeIntAttrV() const {
  TStrV IntNA = TStrV(IntCols.Len(),0);
  for (int i = 0; i < DstNodeAttrV.Len(); i++) {
    TStr Attr = DstNodeAttrV[i];
    if (GetColType(Attr) == INT) {
      IntNA.Add(Attr);
    }
  }
  return IntNA;
}

TStrV TTable::GetEdgeIntAttrV() const {
  TStrV IntEA = TStrV(IntCols.Len(),0);
  for (int i = 0; i < EdgeAttrV.Len(); i++) {
    TStr Attr = EdgeAttrV[i];
    if (GetColType(Attr) == INT) {
      IntEA.Add(Attr);
    }
  }
  return IntEA;
}

TStrV TTable::GetSrcNodeFltAttrV() const {
  TStrV FltNA = TStrV(FltCols.Len(),0);
  for (int i = 0; i < SrcNodeAttrV.Len(); i++) {
    TStr Attr = SrcNodeAttrV[i];
    if (GetColType(Attr) == FLT) {
      FltNA.Add(Attr);
    }
  }
  return FltNA;
}

TStrV TTable::GetDstNodeFltAttrV() const {
  TStrV FltNA = TStrV(FltCols.Len(),0);
  for (int i = 0; i < DstNodeAttrV.Len(); i++) {
    TStr Attr = DstNodeAttrV[i];
    if (GetColType(Attr) == FLT) {
      FltNA.Add(Attr);
    }
  }
  return FltNA;
}

TStrV TTable::GetEdgeFltAttrV() const {
  TStrV FltEA = TStrV(FltCols.Len(),0);;
  for (int i = 0; i < EdgeAttrV.Len(); i++) {
    TStr Attr = EdgeAttrV[i];
    if (GetColType(Attr) == FLT) {
      FltEA.Add(Attr);
    }
  }
  return FltEA;
}

TStrV TTable::GetSrcNodeStrAttrV() const {
  TStrV StrNA = TStrV(StrColMaps.Len(),0);
  for (int i = 0; i < SrcNodeAttrV.Len(); i++) {
    TStr Attr = SrcNodeAttrV[i];
    if (GetColType(Attr) == STR) {
      StrNA.Add(Attr);
    }
  }
  return StrNA;
}

TStrV TTable::GetDstNodeStrAttrV() const {
  TStrV StrNA = TStrV(StrColMaps.Len(),0);
  for (int i = 0; i < DstNodeAttrV.Len(); i++) {
    TStr Attr = DstNodeAttrV[i];
    if (GetColType(Attr) == STR) {
      StrNA.Add(Attr);
    }
  }
  return StrNA;
}


TStrV TTable::GetEdgeStrAttrV() const {
  TStrV StrEA = TStrV(StrColMaps.Len(),0);
  for (int i = 0; i < EdgeAttrV.Len(); i++) {
    TStr Attr = EdgeAttrV[i];
    if (GetColType(Attr) == STR) {
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
  NumValidRows--;
}

void TTable::RemoveRows(const TIntV& RemoveV){
  for(TInt i = 0; i < RemoveV.Len(); i++){RemoveRow(RemoveV[i]);}
}

void TTable::KeepSortedRows(const TIntV& KeepV){
  // need to remove first rows
  while(FirstValidRow != KeepV[0]){ RemoveRow(FirstValidRow);}
  // at this point we know the first row will stay - i.e. FirstValidRow == KeepV[0]
  TInt KeepIdx = 1;
  TRowIterator RowI = BegRI();
  while(RowI < EndRI()){
    if(KeepV.Len() <= KeepIdx){ return;}
    if(KeepV[KeepIdx] == Next[RowI.GetRowIdx()]){
      KeepIdx++;
      RowI++;
    } else{
      RemoveRow(Next[RowI.GetRowIdx()]);
    }
  }
}

void TTable::Unique(TStr Col){
  if(!ColTypeMap.IsKey(Col)){TExcept::Throw("no such column " + Col);}
  TIntV RemainingRows = TIntV(NumValidRows,0);
  // group by given column (keys) and keep only first row for each key
  switch(GetColType(Col)){
    case INT:{
      THash<TInt,TIntV> T;  // can't really estimate the size of T for constructor hinting
      GroupByIntCol(Col, T, TIntV(0), true);
      for(THash<TInt,TIntV>::TIter it = T.BegI(); it < T.EndI(); it++){
        RemainingRows.Add(it->Dat[0]);
      }
      break;
    }
    case FLT:{
      THash<TFlt,TIntV> T;
      GroupByFltCol(Col, T, TIntV(0), true);
      for(THash<TFlt,TIntV>::TIter it = T.BegI(); it < T.EndI(); it++){
        RemainingRows.Add(it->Dat[0]);
      }
      break;
    }
    case STR:{
      THash<TStr,TIntV> T;
      GroupByStrCol(Col, T, TIntV(0), true);
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
void TTable::GroupByIntCol(TStr GroupBy, THash<TInt,TIntV>& grouping, const TIntV& IndexSet, TBool All) const{
  if(!ColTypeMap.IsKey(GroupBy)){TExcept::Throw("no such column " + GroupBy);}
  if(GetColType(GroupBy) != INT){TExcept::Throw(GroupBy + " values are not of expected type integer");}
  if(All){
     // optimize for the common and most expensive case - itearte over only valid rows
    for(TRowIterator it = BegRI(); it < EndRI(); it++){
      UpdateGrouping<TInt>(grouping, it.GetIntAttr(GroupBy), it.GetRowIdx());
    }
  } else{
    // consider only rows in IndexSet
    for(TInt i = 0; i < IndexSet.Len(); i++){
      if(IsRowValid(IndexSet[i])){
        TInt RowIdx = IndexSet[i];
        const TIntV& Col = IntCols[GetColIdx(GroupBy)];       
        UpdateGrouping<TInt>(grouping, Col[RowIdx], RowIdx);
      }
    }
  }
}

void TTable::GroupByFltCol(TStr GroupBy, THash<TFlt,TIntV>& grouping, const TIntV& IndexSet, TBool All) const{
  if(!ColTypeMap.IsKey(GroupBy)){TExcept::Throw("no such column " + GroupBy);}
  if(GetColType(GroupBy) != FLT){TExcept::Throw(GroupBy + " values are not of expected type float");}
   if(All){
     // optimize for the common and most expensive case - itearte over only valid rows
    for(TRowIterator it = BegRI(); it < EndRI(); it++){
      UpdateGrouping<TFlt>(grouping, it.GetFltAttr(GroupBy), it.GetRowIdx());
    }
  } else{
    // consider only rows in IndexSet
    for(TInt i = 0; i < IndexSet.Len(); i++){
      if(IsRowValid(IndexSet[i])){
        TInt RowIdx = IndexSet[i];
        const TFltV& Col = FltCols[GetColIdx(GroupBy)];       
        UpdateGrouping<TFlt>(grouping, Col[RowIdx], RowIdx);
      }
    }
  }
}

void TTable::GroupByStrCol(TStr GroupBy, THash<TStr,TIntV>& Grouping, const TIntV& IndexSet, TBool All) const{
  if(!ColTypeMap.IsKey(GroupBy)){TExcept::Throw("no such column " + GroupBy);}
  if(GetColType(GroupBy) != STR){TExcept::Throw(GroupBy + " values are not of expected type string");}
   if(All){
     // optimize for the common and most expensive case - itearte over all valid rows
    for(TRowIterator it = BegRI(); it < EndRI(); it++){
      UpdateGrouping<TStr>(Grouping, it.GetStrAttr(GroupBy), it.GetRowIdx());
    }
  } else{
    // consider only rows in IndexSet
    for(TInt i = 0; i < IndexSet.Len(); i++){
      if(IsRowValid(IndexSet[i])){
        TInt RowIdx = IndexSet[i];     
        UpdateGrouping<TStr>(Grouping, GetStrVal(GroupBy, RowIdx), RowIdx);
      }
    }
  }
}


void TTable::GroupAux(const TStrV& GroupBy, TInt GroupByStartIdx, THash<TInt,TIntV>& grouping, const TIntV& IndexSet, TBool All){
  /* recursion base - add IndexSet as group */
  if(GroupByStartIdx == GroupBy.Len()){
    if(IndexSet.Len() == 0){return;}
	  TInt key = grouping.Len();
	  grouping.AddDat(key, IndexSet);
	  return;
  }
  if(!ColTypeMap.IsKey(GroupBy[GroupByStartIdx])){TExcept::Throw("no such column " + GroupBy[GroupByStartIdx]);}
  switch(GetColType(GroupBy[GroupByStartIdx])){
    case INT:{
      // group by current column
      // not sure of to estimate the size of T for constructor hinting purpose.
      // It is bounded by the length of the IndexSet or the length of the grouping column if the IndexSet vector is empty
      // but this bound may be way too big
	    THash<TInt,TIntV> T;  
	    GroupByIntCol(GroupBy[GroupByStartIdx], T, IndexSet, All);
	    for(THash<TInt,TIntV>::TIter it = T.BegI(); it < T.EndI(); it++){
	      TIntV& CurrGroup = it->Dat;
        // each group according to current column will be used as an IndexSet
        // for grouping according to next column
		    GroupAux(GroupBy, GroupByStartIdx+1, grouping, CurrGroup, false);
	   }
	    break;
	  }
	  case FLT:{
	    THash<TFlt,TIntV> T;
	    GroupByFltCol(GroupBy[GroupByStartIdx], T, IndexSet, All);
	    for(THash<TFlt,TIntV>::TIter it = T.BegI(); it < T.EndI(); it++){
	      TIntV& CurrGroup = it->Dat;
		    GroupAux(GroupBy, GroupByStartIdx+1, grouping, CurrGroup, false);
	    }
	    break;
	  }
	  case STR:{
	    THash<TStr,TIntV> T;
	    GroupByStrCol(GroupBy[GroupByStartIdx], T, IndexSet, All);
	    for(THash<TStr,TIntV>::TIter it = T.BegI(); it < T.EndI(); it++){
	      TIntV& CurrGroup = it->Dat;
	      GroupAux(GroupBy, GroupByStartIdx+1, grouping, CurrGroup, false);
	    }
	    break;
	  }
  }
}

void TTable::StoreGroupCol(TStr GroupColName, const THash<TInt,TIntV>& Grouping){
  GroupMapping.AddDat(GroupColName, Grouping);
  // add a column where the value of the i'th row is the group id of row i
  IntCols.Add(TIntV(NumRows));
  TInt L = IntCols.Len();
  ColTypeMap.AddDat(GroupColName, TPair<TYPE,TInt>(INT, L-1));
  for(THash<TInt,TIntV>::TIter it = Grouping.BegI(); it < Grouping.EndI(); it++){
    TIntV& G = it->Dat;
    for(TInt i = 0; i < G.Len(); i++){
      IntCols[L-1][G[i]] = it->Key;
    }
  }
}

void TTable::Group(TStr GroupColName, const TStrV& GroupBy){
  THash<TInt,TIntV> grouping;
  GroupAux(GroupBy, 0, grouping, TIntV(0), true);
  StoreGroupCol(GroupColName, grouping);
  AddSchemaCol(GroupColName, INT); // update schema
}

void TTable::Count(TStr CountColName, TStr Col){
  if(!ColTypeMap.IsKey(Col)){TExcept::Throw("no such column " + Col);}
  TIntV CntCol(NumRows);
  switch(GetColType(Col)){
    case INT:{
      THash<TInt,TIntV> T;  // can't really estimate the size of T for constructor hinting
      TIntV& Column = IntCols[GetColIdx(Col)];
      GroupByIntCol(Col, T, TIntV(0), true);
      for(TRowIterator it = BegRI(); it < EndRI(); it++){
        CntCol[it.GetRowIdx()] = T.GetDat(Column[it.GetRowIdx()]).Len();
      }
      break;
    }
    case FLT:{
      THash<TFlt,TIntV> T;
      TFltV& Column = FltCols[GetColIdx(Col)];
      GroupByFltCol(Col, T, TIntV(0), true);
      for(TRowIterator it = BegRI(); it < EndRI(); it++){
         CntCol[it.GetRowIdx()] = T.GetDat(Column[it.GetRowIdx()]).Len();
      }
      break;
    }
    case STR:{
      THash<TStr,TIntV> T;
      GroupByStrCol(Col, T, TIntV(0), true);
      for(TRowIterator it = BegRI(); it < EndRI(); it++){
        CntCol[it.GetRowIdx()] = T.GetDat(GetStrVal(Col, it.GetRowIdx())).Len();
      }
    }
  }
  // add count column
  IntCols.Add(CntCol);
  AddSchemaCol(CountColName, INT);
  ColTypeMap.AddDat(CountColName, TPair<TYPE,TInt>(INT, IntCols.Len()-1));
}

 PTable TTable::InitializeJointTable(const TTable& Table){
  PTable JointTable = New();
  JointTable->Name = Name + "_" + Table.Name;
  JointTable->IntCols = TVec<TIntV>(IntCols.Len() + Table.IntCols.Len());
  JointTable->FltCols = TVec<TFltV>(FltCols.Len() + Table.FltCols.Len());
  JointTable->StrColMaps = TVec<TIntV>(StrColMaps.Len() + Table.StrColMaps.Len());
  for(TInt i = 0; i < S.Len(); i++){
    TStr ColName = GetSchemaColName(i);
    TYPE ColType = GetSchemaColType(i);
    TStr CName = Name + "." + ColName;
    JointTable->ColTypeMap.AddDat(CName, ColTypeMap.GetDat(ColName));
    JointTable->AddLabel(CName, ColName);
    JointTable->S.Add(TPair<TStr,TYPE>(CName, ColType));
  }
  for(TInt i = 0; i < Table.S.Len(); i++){
    TStr ColName = GetSchemaColName(i);
    TYPE ColType = GetSchemaColType(i);
    TStr CName = Name + "." + ColName;
    TPair<TYPE, TInt> NewDat = Table.ColTypeMap.GetDat(ColName);
    Assert(ColType == NewDat.Val1);
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
    JointTable->AddLabel(CName, ColName);
    JointTable->AddSchemaCol(CName, ColType);
  }
  return JointTable;
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
  if (GetColType(Col1) != GetColType(Col2)) {
    TExcept::Throw("Trying to Join on columns of different type");
  }
  // initialize result table
  PTable JointTable = InitializeJointTable(Table);
  // hash smaller table (group by column)
  TYPE ColType = GetColType(Col1);
  TBool ThisIsSmaller = (NumValidRows <= Table.NumValidRows);
  const TTable& TS = ThisIsSmaller ? *this : Table;
  const TTable& TB = ThisIsSmaller ?  Table : *this;
  TStr ColS = ThisIsSmaller ? Col1 : Col2;
  TStr ColB = ThisIsSmaller ? Col2 : Col1;
  // iterate over the rows of the bigger table and check for "collisions" 
  // with the group keys for the small table.
  switch(ColType){
    case INT:{
      THash<TInt, TIntV> T;
      TS.GroupByIntCol(Col1, T, TIntV(), true);
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
      TS.GroupByFltCol(Col1, T, TIntV(), true);
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
      TS.GroupByStrCol(Col1, T, TIntV(), true);
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
      switch(GetColType(RelevantCols[i])){
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

// wrong reading of string attributes
void TTable::BuildGraphTopology(PNEAGraph& Graph, THash<TFlt, TInt>& FSrNodeMap, THash<TFlt, TInt>& FDsNodeMap) {
  TYPE SrCT = GetColType(SrcCol);
  TInt SrIdx = GetColIdx(SrcCol);
  TYPE DsCT = GetColType(DstCol);
  TInt DsIdx = GetColIdx(DstCol);
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
  TYPE CT = GetColType(Col);
  TInt Idx = GetColIdx(Col);
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
      TYPE CT = GetColType(SrcColAttr);
      int Idx = GetColIdx(SrcColAttr);
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
      TYPE CT = GetColType(DstColAttr);
      int Idx = GetColIdx(DstColAttr);
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
      TYPE T = GetColType(ColName);
      TInt Index = GetColIdx(ColName);
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



