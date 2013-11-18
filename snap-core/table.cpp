//#include "table.h"

TInt const TTable::Last =-1;
TInt const TTable::Invalid =-2;

TRowIterator& TRowIterator::operator++(int){
  return this->Next();
}

TRowIterator& TRowIterator::Next(){
  CurrRowIdx = Table->Next[CurrRowIdx];
  Assert(CurrRowIdx != TTable::Invalid);
  return *this;
}

bool TRowIterator::operator < (const TRowIterator& RowI) const{ 
  if(CurrRowIdx == TTable::Last){ return false;}
  if(RowI.CurrRowIdx == TTable::Last){ return true;}
  return CurrRowIdx < RowI.CurrRowIdx;
}

bool TRowIterator::operator == (const TRowIterator& RowI) const {
  return CurrRowIdx == RowI.CurrRowIdx;
}

TInt TRowIterator::GetRowIdx() const {
  return CurrRowIdx;
}
// we do not check column type in the iterator
TInt TRowIterator::GetIntAttr(TInt ColIdx) const {
  return Table->IntCols[ColIdx][CurrRowIdx];
}

TFlt TRowIterator::GetFltAttr(TInt ColIdx) const {
  return Table->FltCols[ColIdx][CurrRowIdx];
}

TStr TRowIterator::GetStrAttr(TInt ColIdx) const {
  return Table->GetStrVal(ColIdx, CurrRowIdx);
}

TInt TRowIterator::GetIntAttr(const TStr& Col) const {
  TInt ColIdx = Table->ColTypeMap.GetDat(Col).Val2;
  return Table->IntCols[ColIdx][CurrRowIdx];
}

TFlt TRowIterator::GetFltAttr(const TStr& Col) const {
  TInt ColIdx = Table->ColTypeMap.GetDat(Col).Val2;
  return Table->FltCols[ColIdx][CurrRowIdx];
}

TStr TRowIterator::GetStrAttr(const TStr& Col) const {
  return Table->GetStrVal(Col, CurrRowIdx);
}

TInt TRowIterator::GetStrMap(const TStr& Col) const {
  TInt ColIdx = Table->ColTypeMap.GetDat(Col).Val2;
  return Table->StrColMaps[ColIdx][CurrRowIdx];
}

TInt TRowIterator::GetStrMap(TInt ColIdx) const {
  return Table->StrColMaps[ColIdx][CurrRowIdx];
}

TRowIteratorWithRemove::TRowIteratorWithRemove(TInt RowIdx, TTable* TablePtr):
  CurrRowIdx(RowIdx),
  Table(TablePtr),
  Start(RowIdx == TablePtr->FirstValidRow){}

TRowIteratorWithRemove& TRowIteratorWithRemove::operator++(int){
  return this->Next();
}

TRowIteratorWithRemove& TRowIteratorWithRemove::Next(){
  CurrRowIdx = GetNextRowIdx();
  Start = false;
  Assert(CurrRowIdx != TTable::Invalid);
  return *this;
}

bool TRowIteratorWithRemove::operator < (const TRowIteratorWithRemove& RowI) const { 
  if(CurrRowIdx == TTable::Last){ return false;}
  if(RowI.CurrRowIdx == TTable::Last){ return true;}
  return CurrRowIdx < RowI.CurrRowIdx;
}

bool TRowIteratorWithRemove::operator == (const TRowIteratorWithRemove& RowI) const {
  return CurrRowIdx == RowI.CurrRowIdx;
}

TInt TRowIteratorWithRemove::GetRowIdx() const {
  return CurrRowIdx;
}

TInt TRowIteratorWithRemove::GetNextRowIdx() const {
  return (Start ? Table->FirstValidRow : Table->Next[CurrRowIdx]);
}

// we do not check column type in the iterator
TInt TRowIteratorWithRemove::GetNextIntAttr(TInt ColIdx) const {
  return Table->IntCols[ColIdx][GetNextRowIdx()];
}

TFlt TRowIteratorWithRemove::GetNextFltAttr(TInt ColIdx) const {
  return Table->FltCols[ColIdx][GetNextRowIdx()];
}

TStr TRowIteratorWithRemove::GetNextStrAttr(TInt ColIdx) const {
  return Table->GetStrVal(ColIdx, GetNextRowIdx());
}

TInt TRowIteratorWithRemove::GetNextIntAttr(const TStr& Col) const {
  TInt ColIdx = Table->ColTypeMap.GetDat(Col).Val2;
  return Table->IntCols[ColIdx][GetNextRowIdx()];
}

TFlt TRowIteratorWithRemove::GetNextFltAttr(const TStr& Col) const {
  TInt ColIdx = Table->ColTypeMap.GetDat(Col).Val2;
  return Table->FltCols[ColIdx][GetNextRowIdx()];
}

TStr TRowIteratorWithRemove::GetNextStrAttr(const TStr& Col) const {
  return Table->GetStrVal(Col, GetNextRowIdx());
}

TBool TRowIteratorWithRemove::IsFirst() const {
  return CurrRowIdx == Table->FirstValidRow;
}

void TRowIteratorWithRemove::RemoveNext(){
  TInt OldNextRowIdx = GetNextRowIdx();
  if(OldNextRowIdx == Table->FirstValidRow){
    Table->RemoveFirstRow();
    return;
  }
  Assert(OldNextRowIdx != TTable::Invalid);
  if(OldNextRowIdx == TTable::Last){ return;}
  Table->Next[CurrRowIdx] = Table->Next[OldNextRowIdx];
  if (Table->LastValidRow == OldNextRowIdx){
    Table->LastValidRow = CurrRowIdx;
  }
  Table->Next[OldNextRowIdx] = TTable::Invalid;
  Table->NumValidRows--;
}

// better not use default constructor as it leads to a memory leak - OR - implement a destructor
TTable::TTable(): Context(*(new TTableContext)), NumRows(0), NumValidRows(0), FirstValidRow(0), LastValidRow(-1){}

TTable::TTable(TTableContext& Context): Context(Context), NumRows(0), NumValidRows(0), FirstValidRow(0), LastValidRow(-1){} 

TTable::TTable(const TStr& TableName, const Schema& TableSchema, TTableContext& Context): Name(TableName), Context(Context),
  S(TableSchema), NumRows(0), NumValidRows(0), FirstValidRow(0), LastValidRow(-1){
  TInt IntColCnt = 0;
  TInt FltColCnt = 0;
  TInt StrColCnt = 0;
  for(TInt i = 0; i < S.Len(); i++){
    TStr ColName = GetSchemaColName(i);
    TAttrType ColType = GetSchemaColType(i);
    switch(ColType){
      case atInt:
        ColTypeMap.AddDat(ColName, TPair<TAttrType,TInt>(atInt, IntColCnt));
        IntColCnt++;
        break;
      case atFlt:
        ColTypeMap.AddDat(ColName, TPair<TAttrType,TInt>(atFlt, FltColCnt));
        FltColCnt++;
        break;
      case atStr:
        ColTypeMap.AddDat(ColName, TPair<TAttrType,TInt>(atStr, StrColCnt));
        StrColCnt++;
        break;
    }
  }
  IntCols = TVec<TIntV>(IntColCnt);
  FltCols = TVec<TFltV>(FltColCnt);
  StrColMaps = TVec<TIntV>(StrColCnt);
}

TTable::TTable(TSIn& SIn, TTableContext& Context): Name(SIn), Context(Context), NumRows(SIn), NumValidRows(SIn), FirstValidRow(SIn), 
  Next(SIn), IntCols(SIn), FltCols(SIn), StrColMaps(SIn){
  THash<TStr,TPair<TInt,TInt> > ColTypeIntMap(SIn);

  ColTypeMap.Clr();
  S.Clr();
  for (THash<TStr,TPair<TInt,TInt> >::TIter it = ColTypeIntMap.BegI(); it < ColTypeIntMap.EndI(); it++){
    TPair<TInt,TInt> dat = it.GetDat();
    switch(dat.GetVal1()){
      case 0:
        ColTypeMap.AddDat(it.GetKey(), TPair<TAttrType,TInt>(atInt, dat.GetVal2()));
        S.Add(TPair<TStr,TAttrType>(it.GetKey(), atInt));
        break;
      case 1:
        ColTypeMap.AddDat(it.GetKey(), TPair<TAttrType,TInt>(atFlt, dat.GetVal2()));
        S.Add(TPair<TStr,TAttrType>(it.GetKey(), atFlt));
        break;
      case 2:
        ColTypeMap.AddDat(it.GetKey(), TPair<TAttrType,TInt>(atStr, dat.GetVal2()));
        S.Add(TPair<TStr,TAttrType>(it.GetKey(), atStr));
        break;
    }
  }
}

TTable::TTable(const TStr& TableName, const THash<TInt,TInt>& H, const TStr& Col1, const TStr& Col2, TTableContext& Context, const TBool IsStrKeys): 
  Name(TableName), Context(Context), NumRows(H.Len()), NumValidRows(H.Len()), FirstValidRow(0), LastValidRow(-1){
    TAttrType KeyType = IsStrKeys ? atStr : atInt;
    S.Add(TPair<TStr,TAttrType>(Col1, KeyType));
    S.Add(TPair<TStr,TAttrType>(Col2, atInt));
    ColTypeMap.AddDat(Col1, TPair<TAttrType,TInt>(KeyType,0));
    ColTypeMap.AddDat(Col2, TPair<TAttrType,TInt>(atInt,1));
    if(IsStrKeys){
      StrColMaps = TVec<TIntV>(1);
      IntCols = TVec<TIntV>(1);
      H.GetKeyV(StrColMaps[0]);
      H.GetDatV(IntCols[0]);
    } else{
      IntCols = TVec<TIntV>(2);
      H.GetKeyV(IntCols[0]);
      H.GetDatV(IntCols[1]);
    }
    Next = TIntV(NumRows);
    for(TInt i = 0; i < NumRows; i++){
      Next[i] = i+1;
    }
    Next[NumRows-1] = Last;
}

TTable::TTable(const TStr& TableName, const THash<TInt,TFlt>& H, const TStr& Col1, const TStr& Col2, TTableContext& Context, const TBool IsStrKeys):
  Name(TableName), Context(Context), NumRows(H.Len()), NumValidRows(H.Len()), FirstValidRow(0), LastValidRow(-1){
    TAttrType KeyType = IsStrKeys ? atStr : atInt;
    S.Add(TPair<TStr,TAttrType>(Col1, KeyType));
    S.Add(TPair<TStr,TAttrType>(Col2, atFlt));
    ColTypeMap.AddDat(Col1, TPair<TAttrType,TInt>(KeyType,0));
    ColTypeMap.AddDat(Col2, TPair<TAttrType,TInt>(atFlt,0));
    if(IsStrKeys){
      StrColMaps = TVec<TIntV>(1);
      H.GetKeyV(StrColMaps[0]);
    } else{
      IntCols = TVec<TIntV>(1);
      H.GetKeyV(IntCols[0]);
    }
    FltCols = TVec<TFltV>(1);   
    H.GetDatV(FltCols[0]);
    Next = TIntV(NumRows);
    for(TInt i = 0; i < NumRows; i++){
      Next[i] = i+1;
    }
    Next[NumRows-1] = Last;
}

PTable TTable::LoadSS(const TStr& TableName, const Schema& S, const TStr& InFNm, TTableContext& Context, const TIntV& RelevantCols, const char& Separator, TBool HasTitleLine){
  TSsParser Ss(InFNm, Separator);
  Schema SR;
  if(RelevantCols.Len() == 0){
    SR = S;
  } else{
    for(TInt i = 0; i < RelevantCols.Len(); i++){
      SR.Add(S[RelevantCols[i]]);
    }
  }
  PTable T = New(TableName, SR, Context);

  // if title line (i.e. names of the columns) is included as first row in the
  // input file - use it to validate schema
  if(HasTitleLine){
    Ss.Next();  
    if(S.Len() != Ss.GetFlds()){printf("%s\n", Ss[0]); TExcept::Throw("Table Schema Mismatch!");}
    for(TInt i = 0; i < Ss.GetFlds(); i++){
      // remove carriage return char
      TInt L = strlen(Ss[i]);
      if(Ss[i][L-1] < ' '){ Ss[i][L-1] = 0;}
      if(S[i].Val1 != Ss[i]){ TExcept::Throw("Table Schema Mismatch!");}
    }
  }

  TInt RowLen = SR.Len();
  TVec<TAttrType> ColTypes = TVec<TAttrType>(RowLen);
  for(TInt i = 0; i < RowLen; i++){
    ColTypes[i] = T->GetSchemaColType(i);
  }
  // TInt Cnt = 1;
  // populate table columns
  while(Ss.Next()){
    TInt IntColIdx = 0;
    TInt FltColIdx = 0;
    TInt StrColIdx = 0;
    Assert(Ss.GetFlds() == S.Len()); // compiled only in debug
    if(Ss.GetFlds() != S.Len()){printf("%s\n", Ss[S.Len()]); TExcept::Throw("Error reading tsv file");}
    for(TInt i = 0; i < RowLen; i++){
      switch(ColTypes[i]){
        case atInt:
          if(RelevantCols.Len() == 0){
            T->IntCols[IntColIdx].Add(Ss.GetInt(i));
          } else {
            T->IntCols[IntColIdx].Add(Ss.GetInt(RelevantCols[i]));
          }
          IntColIdx++;
          break;
        case atFlt:
          if(RelevantCols.Len() == 0){
            T->FltCols[FltColIdx].Add(Ss.GetFlt(i));
          } else {
            T->FltCols[FltColIdx].Add(Ss.GetFlt(RelevantCols[i]));
          }
          FltColIdx++;
          break;
        case atStr:
          TInt ColIdx;
          if(RelevantCols.Len() == 0){
            ColIdx = i;
          } else {
            ColIdx = RelevantCols[i];
          }
          TStr Sval = TStr(Ss[ColIdx]);
          T->AddStrVal(StrColIdx, Sval);
          StrColIdx++;
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
  T->LastValidRow = T->NumRows - 1;
  T->InitIds();
  return T;
}

PTable TTable::LoadSS(const TStr& TableName, const Schema& S, const TStr& InFNm, TTableContext& Context, const char& Separator, TBool HasTitleLine){
  return LoadSS(TableName, S, InFNm, Context, TIntV(), Separator, HasTitleLine);
}


void TTable::SaveSS(const TStr& OutFNm){
  FILE* F = fopen(OutFNm.CStr(), "w");
  // debug
  if(F == NULL){
    printf("failed to open file %s\n", OutFNm.CStr());
    perror("fail ");
    return;
  }

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
	      case atInt:{
		    fprintf(F, "%d%c", RowI.GetIntAttr(GetSchemaColName(i)).Val, C);
		    break;
		  }
	     case atFlt:{
		    fprintf(F, "%f%c", RowI.GetFltAttr(GetSchemaColName(i)).Val, C);
		    break;
		  }
	    case atStr:{
		    fprintf(F, "%s%c", RowI.GetStrAttr(GetSchemaColName(i)).CStr(), C);
		    break;
		  }
	   }
	  }
 }
 fclose(F);
}

void TTable::Save(TSOut& SOut){ 
  Name.Save(SOut);
  NumRows.Save(SOut); NumValidRows.Save(SOut); 
  FirstValidRow.Save(SOut); Next.Save(SOut); 
  IntCols.Save(SOut); FltCols.Save(SOut); StrColMaps.Save(SOut); 

  THash<TStr,TPair<TInt,TInt> > ColTypeIntMap;
  TInt atIntVal = TInt(0);
  TInt atFltVal = TInt(1);
  TInt atStrVal = TInt(2);
  for (THash<TStr,TPair<TAttrType,TInt> >::TIter it = ColTypeMap.BegI(); it < ColTypeMap.EndI(); it++){
    TPair<TAttrType,TInt> dat = it.GetDat();
    switch(dat.GetVal1()){
      case atInt:
        ColTypeIntMap.AddDat(it.GetKey(), TPair<TInt,TInt>(atIntVal, dat.GetVal2()));
        break;
      case atFlt:
        ColTypeIntMap.AddDat(it.GetKey(), TPair<TInt,TInt>(atFltVal, dat.GetVal2()));
        break;
      case atStr:
        ColTypeIntMap.AddDat(it.GetKey(), TPair<TInt,TInt>(atStrVal, dat.GetVal2()));
        break;
    }
  }
  ColTypeIntMap.Save(SOut);
}

void TTable::AddStrVal(const TInt ColIdx, const TStr& Val){
  TInt Key = Context.StringVals.GetKeyId(Val);
  if(Key != -1){
    StrColMaps[ColIdx].Add(Key);
  } else{
    Key = Context.StringVals.AddKey(Val);
    StrColMaps[ColIdx].Add(Key);
  }
}

void TTable::AddStrVal(const TStr& Col, const TStr& Val){
  if(GetColType(Col) != atStr){TExcept::Throw(Col + " is not a string valued column");}
  AddStrVal(GetColIdx(Col), Val);
}

void TTable::AddGraphAttribute(const TStr& Attr, TBool IsEdge, TBool IsSrc, TBool IsDst){
  if(!ColTypeMap.IsKey(Attr)){TExcept::Throw(Attr + ": No such column");}
  if(IsEdge){EdgeAttrV.Add(Attr);}
  if(IsSrc){SrcNodeAttrV.Add(Attr);}
  if(IsDst){DstNodeAttrV.Add(Attr);}
}

void TTable::AddGraphAttributeV(TStrV& Attrs, TBool IsEdge, TBool IsSrc, TBool IsDst){
  for(TInt i = 0; i < Attrs.Len(); i++){
      if(!ColTypeMap.IsKey(Attrs[i])){TExcept::Throw(Attrs[i] + ": no such column");}
    }    
  if(IsEdge){EdgeAttrV.AddV(Attrs);}
  if(IsSrc){SrcNodeAttrV.AddV(Attrs);}
  if(IsDst){DstNodeAttrV.AddV(Attrs);}
}

TStrV TTable::GetSrcNodeIntAttrV() const {
  TStrV IntNA = TStrV(IntCols.Len(),0);
  for (int i = 0; i < SrcNodeAttrV.Len(); i++) {
    TStr Attr = SrcNodeAttrV[i];
    if (GetColType(Attr) == atInt) {
      IntNA.Add(Attr);
    }
  }
  return IntNA;
}

TStrV TTable::GetDstNodeIntAttrV() const {
  TStrV IntNA = TStrV(IntCols.Len(),0);
  for (int i = 0; i < DstNodeAttrV.Len(); i++) {
    TStr Attr = DstNodeAttrV[i];
    if (GetColType(Attr) == atInt) {
      IntNA.Add(Attr);
    }
  }
  return IntNA;
}

TStrV TTable::GetEdgeIntAttrV() const {
  TStrV IntEA = TStrV(IntCols.Len(),0);
  for (int i = 0; i < EdgeAttrV.Len(); i++) {
    TStr Attr = EdgeAttrV[i];
    if (GetColType(Attr) == atInt) {
      IntEA.Add(Attr);
    }
  }
  return IntEA;
}

TStrV TTable::GetSrcNodeFltAttrV() const {
  TStrV FltNA = TStrV(FltCols.Len(),0);
  for (int i = 0; i < SrcNodeAttrV.Len(); i++) {
    TStr Attr = SrcNodeAttrV[i];
    if (GetColType(Attr) == atFlt) {
      FltNA.Add(Attr);
    }
  }
  return FltNA;
}

TStrV TTable::GetDstNodeFltAttrV() const {
  TStrV FltNA = TStrV(FltCols.Len(),0);
  for (int i = 0; i < DstNodeAttrV.Len(); i++) {
    TStr Attr = DstNodeAttrV[i];
    if (GetColType(Attr) == atFlt) {
      FltNA.Add(Attr);
    }
  }
  return FltNA;
}

TStrV TTable::GetEdgeFltAttrV() const {
  TStrV FltEA = TStrV(FltCols.Len(),0);;
  for (int i = 0; i < EdgeAttrV.Len(); i++) {
    TStr Attr = EdgeAttrV[i];
    if (GetColType(Attr) == atFlt) {
      FltEA.Add(Attr);
    }
  }
  return FltEA;
}

TStrV TTable::GetSrcNodeStrAttrV() const {
  TStrV StrNA = TStrV(StrColMaps.Len(),0);
  for (int i = 0; i < SrcNodeAttrV.Len(); i++) {
    TStr Attr = SrcNodeAttrV[i];
    if (GetColType(Attr) == atStr) {
      StrNA.Add(Attr);
    }
  }
  return StrNA;
}

TStrV TTable::GetDstNodeStrAttrV() const {
  TStrV StrNA = TStrV(StrColMaps.Len(),0);
  for (int i = 0; i < DstNodeAttrV.Len(); i++) {
    TStr Attr = DstNodeAttrV[i];
    if (GetColType(Attr) == atStr) {
      StrNA.Add(Attr);
    }
  }
  return StrNA;
}


TStrV TTable::GetEdgeStrAttrV() const {
  TStrV StrEA = TStrV(StrColMaps.Len(),0);
  for (int i = 0; i < EdgeAttrV.Len(); i++) {
    TStr Attr = EdgeAttrV[i];
    if (GetColType(Attr) == atStr) {
      StrEA.Add(Attr);
    }
  }
  return StrEA;
}

void TTable::AddLabel(const TStr& column, const TStr& NewLabel){
  if(!ColTypeMap.IsKey(column)){TExcept::Throw("no such column " + column);}
  TPair<TAttrType,TInt> ColVal = ColTypeMap.GetDat(column);
  ColTypeMap.AddDat(NewLabel,ColVal);
}

void TTable::Rename(const TStr& column, const TStr& NewLabel){
  // This function is necessary, for example to take the union of two tables where the attribute names don't match
  if(!ColTypeMap.IsKey(column)){TExcept::Throw("no such column " + column);}
  TPair<TAttrType,TInt> ColVal = ColTypeMap.GetDat(column);
  ColTypeMap.DelKey(column);
  ColTypeMap.AddDat(NewLabel,ColVal);
  for(TInt c = 0; c < S.Len(); c++){
    if (S[c].Val1 == column) {
      S.SetVal(c, TPair<TStr, TAttrType>(NewLabel, S[c].Val2));
      break;
    }
  }
}

void TTable::RemoveFirstRow(){
  if (FirstValidRow == LastValidRow){
    LastValidRow = -1;
  }
  TInt Old = FirstValidRow;
  FirstValidRow = Next[FirstValidRow];
  Next[Old] = TTable::Invalid;
  NumValidRows--;
  RowIdMap.AddDat(Old, Invalid);
}

void TTable::RemoveRow(TInt RowIdx){
  if(Next[RowIdx] == TTable::Invalid){ return;} // row was already removed
  if(RowIdx == FirstValidRow){
    RemoveFirstRow();
  } else{
    TInt i = RowIdx-1;
    while(Next[i] != RowIdx){i--;}
    if (RowIdx == LastValidRow) {
      LastValidRow = i;
    }
    Next[i] = Next[RowIdx];
  }
  Next[RowIdx] = TTable::Invalid;
  NumValidRows--;
  RowIdMap.AddDat(RowIdx, Invalid);
}

void TTable::RemoveRows(const TIntV& RemoveV){
  for(TInt i = 0; i < RemoveV.Len(); i++){RemoveRow(RemoveV[i]);}
}

// TODO: simplify using TRowIteratorWithRemove
void TTable::KeepSortedRows(const TIntV& KeepV){
  // need to remove first rows
  while(FirstValidRow != KeepV[0]){ RemoveRow(FirstValidRow);}
  // at this point we know the first row will stay - i.e. FirstValidRow == KeepV[0]
  TInt KeepIdx = 1;
  TRowIterator RowI = BegRI();
  while(RowI < EndRI()){
    if(KeepV.Len() > KeepIdx){
      if(KeepV[KeepIdx] == Next[RowI.GetRowIdx()]){
        KeepIdx++;
        RowI++;
      } else{
          RemoveRow(Next[RowI.GetRowIdx()]);
      }
    // covered all of KeepV. remove the rest of the rows
    // current RowI.CurrRowIdx is the last element of KeepV
    } else{
      while(Next[RowI.GetRowIdx()] != Last){
        RemoveRow(Next[RowI.GetRowIdx()]);
      }
      // removed the rest of the rows. increment RowI to EndRI
      RowI++;
    }
  }
}

/*****  Grouping Utility functions ****/
void TTable::GroupByIntCol(const TStr& GroupBy, THash<TInt,TIntV>& grouping, const TIntV& IndexSet, TBool All) const{
  if(!ColTypeMap.IsKey(GroupBy)){TExcept::Throw("no such column " + GroupBy);}
  if(GetColType(GroupBy) != atInt){TExcept::Throw(GroupBy + " values are not of expected type integer");}
  if(All){
     // optimize for the common and most expensive case - iterate over only valid rows
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

void TTable::GroupByFltCol(const TStr& GroupBy, THash<TFlt,TIntV>& grouping, const TIntV& IndexSet, TBool All) const{
  if(!ColTypeMap.IsKey(GroupBy)){TExcept::Throw("no such column " + GroupBy);}
  if(GetColType(GroupBy) != atFlt){TExcept::Throw(GroupBy + " values are not of expected type float");}
   if(All){
     // optimize for the common and most expensive case - iterate over only valid rows
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

void TTable::GroupByStrCol(const TStr& GroupBy, THash<TStr,TIntV>& Grouping, const TIntV& IndexSet, TBool All) const{
  if(!ColTypeMap.IsKey(GroupBy)){TExcept::Throw("no such column " + GroupBy);}
  if(GetColType(GroupBy) != atStr){TExcept::Throw(GroupBy + " values are not of expected type string");}
   if(All){
     // optimize for the common and most expensive case - iterate over all valid rows
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

void TTable::Unique(const TStr& Col){
    TIntV RemainingRows;
    switch(GetColType(Col)){
      case atInt:{
        THash<TInt,TIntV> Grouping;
        GroupByIntCol(Col, Grouping, TIntV(), true);
        for(THash<TInt,TIntV>::TIter it = Grouping.BegI(); it < Grouping.EndI(); it++){
          RemainingRows.Add(it->Dat[0]);
        }
        break;
      }
      case atFlt:{
        THash<TFlt,TIntV> Grouping;
        GroupByFltCol(Col, Grouping, TIntV(), true);
        for(THash<TFlt,TIntV>::TIter it = Grouping.BegI(); it < Grouping.EndI(); it++){
          RemainingRows.Add(it->Dat[0]);
        }
        break;
      }
      case atStr:{
        THash<TStr,TIntV> Grouping;
        GroupByStrCol(Col, Grouping, TIntV(), true);
        for(THash<TStr,TIntV>::TIter it = Grouping.BegI(); it < Grouping.EndI(); it++){
          RemainingRows.Add(it->Dat[0]);
        }
        break;
      }
   }
   KeepSortedRows(RemainingRows);
}

void TTable::Unique(const TStrV& Cols, TBool Ordered){
  THash<TGroupKey, TPair<TInt, TIntV> > Grouping;
  TIntV UniqueVec;
  GroupAux(Cols, Grouping, Ordered, "", true, UniqueVec);
  KeepSortedRows(UniqueVec);
}

void TTable::StoreGroupCol(const TStr& GroupColName, const TVec<TPair<TInt, TInt> >& GroupAndRowIds) {
  // add a column where the value of the i'th row is the group id of row i
  IntCols.Add(TIntV(NumRows));
  TInt L = IntCols.Len();
  ColTypeMap.AddDat(GroupColName, TPair<TAttrType,TInt>(atInt, L-1));
  // Store group id for each row
  for(TInt i = 0; i < GroupAndRowIds.Len(); i++) {
    IntCols[L-1][GroupAndRowIds[i].Val2] = GroupAndRowIds[i].Val1;
  }
}

// core crouping logic
void TTable::GroupAux(const TStrV& GroupBy, THash<TGroupKey, TPair<TInt, TIntV> >& Grouping, TBool Ordered, const TStr& GroupColName, TBool KeepUnique,
    TIntV& UniqueVec) {
  TIntV IntGroupByCols;
  TIntV FltGroupByCols;
  TIntV StrGroupByCols;
  // get indices for each column type
  for(TInt c = 0; c < GroupBy.Len(); c++){
    if(!ColTypeMap.IsKey(GroupBy[c])){TExcept::Throw("no such column " + GroupBy[c]);}

    TPair<TAttrType, TInt> ColType = ColTypeMap.GetDat(GroupBy[c]);
    switch(ColType.Val1){
      case atInt:
        IntGroupByCols.Add(ColType.Val2);
        break;
      case atFlt:
        FltGroupByCols.Add(ColType.Val2);
        break;
      case atStr:
        StrGroupByCols.Add(ColType.Val2);
        break;
    }
  }

  TInt IKLen = IntGroupByCols.Len();
  TInt FKLen = FltGroupByCols.Len();
  TInt SKLen = StrGroupByCols.Len();

  TInt GroupNum = 0;
  TInt IdColIdx = ColTypeMap.GetDat(IdColName).Val2;

  TVec<TPair<TInt, TInt> > GroupAndRowIds;

  // iterate over rows
  for(TRowIterator it = BegRI(); it < EndRI(); it++){
    TIntV IKey(IKLen + SKLen, 0);
    TFltV FKey(FKLen, 0);
    TIntV SKey(SKLen, 0);

    // find group key
    for(TInt c = 0; c < IKLen; c++){
      IKey.Add(it.GetIntAttr(IntGroupByCols[c])); 
    }
    for(TInt c = 0; c < FKLen; c++){
      FKey.Add(it.GetFltAttr(FltGroupByCols[c])); 
    }
    for(TInt c = 0; c < SKLen; c++){
      SKey.Add(it.GetStrMap(StrGroupByCols[c])); 
    }
    if(!Ordered){
      if(IKLen > 0){IKey.ISort(0, IKey.Len()-1, true);}
      if(FKLen > 0){FKey.ISort(0, FKey.Len()-1, true);}
      if(SKLen > 0){SKey.ISort(0, SKey.Len()-1, true);}
    }
    for(TInt c = 0; c < SKLen; c++){
      IKey.Add(SKey[c]);
    }

    // look for group matching the key
    TGroupKey GroupKey = TGroupKey(IKey, FKey);

    TInt RowIdx = it.GetRowIdx();
    if (!Grouping.IsKey(GroupKey)) {
      // Grouping key hasn't been seen before, create a new group
      TPair<TInt, TIntV> NewGroup;
      NewGroup.Val1 = GroupNum;
      NewGroup.Val2.Add(IntCols[IdColIdx][RowIdx]);
      Grouping.AddDat(GroupKey, NewGroup);
      if (GroupColName != "") {
        GroupAndRowIds.Add(TPair<TInt, TInt>(GroupNum, RowIdx));
      }
      if (KeepUnique) { 
        UniqueVec.Add(RowIdx);
      }
      GroupNum++;
    } else {
      // Grouping key has been seen before, update corresponding group
      if (!KeepUnique) {
        TPair<TInt, TIntV>& NewGroup = Grouping.GetDat(GroupKey);
        NewGroup.Val2.Add(IntCols[IdColIdx][RowIdx]);
        if (GroupColName != "") {
          GroupAndRowIds.Add(TPair<TInt, TInt>(NewGroup.Val1, RowIdx));
        }
      }
    }
  }

  // update group mapping
  if (!KeepUnique) {
    TPair<TStrV, TBool> GroupStmt(GroupBy, Ordered);
    GroupStmtNames.AddDat(GroupColName, GroupStmt);
    GroupIDMapping.AddDat(GroupStmt);
    GroupMapping.AddDat(GroupStmt);
    for (THash<TGroupKey, TPair<TInt, TIntV> >::TIter it = Grouping.BegI(); it < Grouping.EndI(); it++) {
      TGroupKey key = it.GetKey();
      TPair<TInt, TIntV> group = it.GetDat();
      GroupIDMapping.GetDat(GroupStmt).AddDat(group.Val1, key);
      GroupMapping.GetDat(GroupStmt).AddDat(key, group.Val2);
    }
  }

  // add a column to the table
  if (GroupColName != "") {
    StoreGroupCol(GroupColName, GroupAndRowIds);
    AddSchemaCol(GroupColName, atInt);  // update schema
  }
}

// grouping begins here
void TTable::Group(const TStrV& GroupBy, const TStr& GroupColName, TBool Ordered) {
  TIntV UniqueVec;
  THash<TGroupKey, TPair<TInt, TIntV> >Grouping;
  // by default, we assume we don't want unique rows
  GroupAux(GroupBy, Grouping, Ordered, GroupColName, false, UniqueVec);
}

void TTable::Aggregate(const TStrV& GroupByAttrs, TAttrAggr AggOp,
  const TStr& ValAttr, const TStr& ResAttr, TBool Ordered) {
  // check if grouping already exists
  TPair<TStrV, TBool> GroupStmtName(GroupByAttrs, Ordered);

  if (!GroupMapping.IsKey(GroupStmtName)) {
    // group mapping does not exist, perform grouping first
    Group(GroupByAttrs, "", Ordered);
  }

  // group mapping exists, retrieve it and aggregate
  THash<TGroupKey, TIntV> Mapping = GroupMapping.GetDat(GroupStmtName);

  // add column corresponding to result attribute type
  if (AggOp == aaCount) {
    AddIntCol(ResAttr);
  }
  else {
    TAttrType T = ColTypeMap.GetDat(ValAttr).Val1;
    if (T == atInt) {
      AddIntCol(ResAttr);
    }
    else if (T == atFlt) {
      AddFltCol(ResAttr);
    }
    else {
      // Count is the only aggregation operation handled for Str
      TExcept::Throw("Invalid aggregation for Str type!");
    }
  }
  TInt ColIdx = ColTypeMap.GetDat(ResAttr).Val2;

  for (THash<TGroupKey, TIntV>::TIter it = Mapping.BegI(); it != Mapping.EndI(); it++) {
    TIntV& GroupRows = it.GetDat();
    
    // find valid rows of group
    TIntV ValidRows;
    for (TInt i = 0; i < GroupRows.Len(); i++) {
      TInt RowId = RowIdMap.GetDat(GroupRows[i]);
      // GroupRows has physical row indices
      if (RowId != Invalid) {
        ValidRows.Add(RowId);
      }
    }

    // Count is handled separately (other operations have aggregation policies defined in a template)
    if (AggOp == aaCount) {
      TInt sz = ValidRows.Len();
      for (TInt i = 0; i < sz; i++) {
        IntCols[ColIdx][ValidRows[i]] = sz;
      }
    }
    else {
      // aggregate based on column type
      TAttrType T = GetColType(ValAttr);
      if (T == atStr) {
        TExcept::Throw("Invalid aggregation for Str type!");
      }
      else if (T == atInt) {
        TIntV V;
        TInt sz = ValidRows.Len();
        if (sz <= 0) continue;

        TInt AggrColIdx = ColTypeMap.GetDat(ValAttr).Val2;
        for (TInt i = 0; i < sz; i++) {
          V.Add(IntCols[AggrColIdx][ValidRows[i]]);
        }
        TInt Res = AggregateVector<TInt>(V, AggOp);
        if (AggOp == aaMean) {
          Res = Res / sz;
        }
        for (TInt i = 0; i < sz; i++) {
          IntCols[ColIdx][ValidRows[i]] = Res;
        }
      }
      else {
        TFltV V;
        TInt sz = ValidRows.Len();
        if (sz <= 0) continue;

        TInt AggrColIdx = ColTypeMap.GetDat(ValAttr).Val2;
        for (TInt i = 0; i < sz; i++) {
          V.Add(FltCols[AggrColIdx][ValidRows[i]]);
        }
        TFlt Res = AggregateVector<TFlt>(V, AggOp);
        if (AggOp == aaMean) {
          Res /= sz;
        }

        for (TInt i = 0; i < sz; i++) {
          FltCols[ColIdx][ValidRows[i]] = Res;
        }
      }
    }
  }
}

void TTable::Count(const TStr& CountColName, const TStr& Col){
  TStrV GroupByAttrs;
  GroupByAttrs.Add(CountColName);
  Aggregate(GroupByAttrs, aaCount, "", Col);
}

void TTable::InitIds() {
  IdColName = Name + "_id";
  Assert(NumRows == NumValidRows);
  RowIdMap.Clr();
  AddIdColumn(IdColName);
  for (TInt i = 0; i < NumRows; i++) {
    RowIdMap.AddDat(i, i);
  }
}

void TTable::AddIdColumn(const TStr& ColName){
  TIntV IdCol(NumRows);
  TInt IdCnt = 0;
  for(TRowIterator RI = BegRI(); RI < EndRI(); RI++){
    IdCol[RI.GetRowIdx()] = IdCnt;
    IdCnt++;
  }
  IntCols.Add(IdCol);
  AddSchemaCol(ColName, atInt);
  ColTypeMap.AddDat(ColName, TPair<TAttrType,TInt>(atInt, IntCols.Len()-1));
}

 PTable TTable::InitializeJointTable(const TTable& Table){
  PTable JointTable = New(Context);
  JointTable->Name = Name + "_" + Table.Name;
  JointTable->IntCols = TVec<TIntV>(IntCols.Len() + Table.IntCols.Len());
  JointTable->FltCols = TVec<TFltV>(FltCols.Len() + Table.FltCols.Len());
  JointTable->StrColMaps = TVec<TIntV>(StrColMaps.Len() + Table.StrColMaps.Len());
  TStr Name1 = Name;
  TStr Name2 = Table.Name;
  if(Name1 == Name2){
    Name1 = Name + "_1";
    Name2 = Name + "_2";
  }
  for(TInt i = 0; i < S.Len(); i++){
    TStr ColName = GetSchemaColName(i);
    TAttrType ColType = GetSchemaColType(i);
    TStr CName = Name1 + "." + ColName;
    JointTable->ColTypeMap.AddDat(CName, ColTypeMap.GetDat(ColName));
    JointTable->AddLabel(CName, ColName);
    JointTable->AddSchemaCol(CName, ColType);
  }
  for(TInt i = 0; i < Table.S.Len(); i++){
    TStr ColName = Table.GetSchemaColName(i);
    TAttrType ColType = Table.GetSchemaColType(i);
    TStr CName = Name2 + "." + ColName;
    TPair<TAttrType, TInt> NewDat = Table.ColTypeMap.GetDat(ColName);
    Assert(ColType == NewDat.Val1);
    // add offsets
    switch(NewDat.Val1){
      case atInt:
        NewDat.Val2 += IntCols.Len();
        break;
      case atFlt:
        NewDat.Val2 += FltCols.Len();
        break;
      case atStr:
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
    StrColMaps[i].Add(T1.StrColMaps[i][RowIdx1]);
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
    StrColMaps[i+StrOffset].Add(T2.StrColMaps[i][RowIdx2]);
  }
  NumRows++;
  NumValidRows++;
  if(!Next.Empty()){
    Next[Next.Len()-1] = NumValidRows-1;
    LastValidRow = NumValidRows-1;
  }
  Next.Add(Last);
}

// Q: Do we want to have any gurantees in terms of order of the 0t rows - i.e. 
// ordered by "this" table row idx as primary key and "Table" row idx as secondary key
 // This means only keeping joint row indices (pairs of original row indices), sorting them
 // and adding all rows in the end. Sorting can be expensive, but we would be able to pre-allocate 
 // memory for the joint table..
PTable TTable::Join(const TStr& Col1, const TTable& Table, const TStr& Col2) {
  if(!ColTypeMap.IsKey(Col1)){
    TExcept::Throw("no such column " + Col1);
  }
  if(!Table.ColTypeMap.IsKey(Col2)){
    TExcept::Throw("no such column " + Col2);
  }
  if (GetColType(Col1) != Table.GetColType(Col2)) {
    TExcept::Throw("Trying to Join on columns of different type");
  }
  // initialize result table
  PTable JointTable = InitializeJointTable(Table);
  // hash smaller table (group by column)
  TAttrType ColType = GetColType(Col1);
  TBool ThisIsSmaller = (NumValidRows <= Table.NumValidRows);
  const TTable& TS = ThisIsSmaller ? *this : Table;
  const TTable& TB = ThisIsSmaller ?  Table : *this;
  TStr ColS = ThisIsSmaller ? Col1 : Col2;
  TStr ColB = ThisIsSmaller ? Col2 : Col1;
  // iterate over the rows of the bigger table and check for "collisions" 
  // with the group keys for the small table.
  switch(ColType){
    case atInt:{
      THash<TInt, TIntV> T;
      TS.GroupByIntCol(ColS, T, TIntV(), true);
      for(TRowIterator RowI = TB.BegRI(); RowI < TB.EndRI(); RowI++){
        TInt K = RowI.GetIntAttr(ColB);
        if(T.IsKey(K)){
          TIntV& Group = T.GetDat(K);
          for(TInt i = 0; i < Group.Len(); i++){
            if(ThisIsSmaller){
              JointTable->AddJointRow(*this, Table, Group[i], RowI.GetRowIdx());
            } else{
              JointTable->AddJointRow(*this, Table, RowI.GetRowIdx(), Group[i]);
            }
          }
        }
      }
      break;
    }
    case atFlt:{
      THash<TFlt, TIntV> T;
      TS.GroupByFltCol(ColS, T, TIntV(), true);
      for(TRowIterator RowI = TB.BegRI(); RowI < TB.EndRI(); RowI++){
        TFlt K = RowI.GetFltAttr(ColB);
        if(T.IsKey(K)){
          TIntV& Group = T.GetDat(K);
          for(TInt i = 0; i < Group.Len(); i++){
            if(ThisIsSmaller){
              JointTable->AddJointRow(*this, Table, Group[i], RowI.GetRowIdx());
            } else{
              JointTable->AddJointRow(*this, Table, RowI.GetRowIdx(), Group[i]);
            }
          }
        }
      }
      break;
    }
    case atStr:{
      THash<TStr, TIntV> T;
      TS.GroupByStrCol(ColS, T, TIntV(), true);
      for(TRowIterator RowI = TB.BegRI(); RowI < TB.EndRI(); RowI++){
        TStr K = RowI.GetStrAttr(ColB);
        if(T.IsKey(K)){
          TIntV& Group = T.GetDat(K);
          for(TInt i = 0; i < Group.Len(); i++){
            if(ThisIsSmaller){
              JointTable->AddJointRow(*this, Table, Group[i], RowI.GetRowIdx());
            } else{
              JointTable->AddJointRow(*this, Table, RowI.GetRowIdx(), Group[i]);
            }
          }
        }
      }
    }
    break;
  }
 JointTable->InitIds();
 return JointTable; 
}

/*
void TTable::Dist(TStr Col1, const TTable& Table, TStr Col2, TStr DistColName, const TMetric& Metric, TFlt threshold) {
  if(!ColTypeMap.IsKey(Col1)){
    TExcept::Throw("no such column " + Col1);
  }
  if(!ColTypeMap.IsKey(Col2)){
    TExcept::Throw("no such column " + Col2);
  }
  //TAttrType T1 = GetColType(Col1);
  //CHECK do we need to make this a const?
  TAttrType T2 = Table.GetColType(Col2);
  if ((T1  == atStr && T2 != atStr) || (T1  != atStr && T2 == atStr) ) {
    TExcept::Throw("Trying to c./ompare strings with numbers.");
  }
}
*/

void TTable::Select(TPredicate& Predicate, TIntV& SelectedRows, TBool Remove){
  TIntV Selected;
  TStrV RelevantCols;
  Predicate.GetVariables(RelevantCols);
  TInt NumRelevantCols = RelevantCols.Len();
  TVec<TAttrType> ColTypes = TVec<TAttrType>(NumRelevantCols);
  TIntV ColIndices = TIntV(NumRelevantCols);
  for(TInt i = 0; i < NumRelevantCols; i++){
    ColTypes[i] = GetColType(RelevantCols[i]);
    ColIndices[i] = GetColIdx(RelevantCols[i]);
  } 
  
  if(Remove){
    TRowIteratorWithRemove RowI = BegRIWR();
    while(RowI.GetNextRowIdx() != Last){
      // prepare arguments for predicate evaluation
      for(TInt i = 0; i < NumRelevantCols; i++){
        switch(ColTypes[i]){
        case atInt:
          Predicate.SetIntVal(RelevantCols[i], RowI.GetNextIntAttr(ColIndices[i]));
          break;
        case atFlt:
          Predicate.SetFltVal(RelevantCols[i], RowI.GetNextFltAttr(ColIndices[i]));
          break;
        case atStr:
          Predicate.SetStrVal(RelevantCols[i], RowI.GetNextStrAttr(ColIndices[i]));
          break;
        }
      }
      if(!Predicate.Eval()){ 
        RowI.RemoveNext();
      } else{
        RowI++;
      }
    }
  } else{
    for(TRowIterator RowI = BegRI(); RowI < EndRI(); RowI++){
      for(TInt i = 0; i < NumRelevantCols; i++){
        switch(ColTypes[i]){
        case atInt:
          Predicate.SetIntVal(RelevantCols[i], RowI.GetIntAttr(RelevantCols[i]));
          break;
        case atFlt:
          Predicate.SetFltVal(RelevantCols[i], RowI.GetFltAttr(RelevantCols[i]));
          break;
        case atStr:
          Predicate.SetStrVal(RelevantCols[i], RowI.GetStrAttr(RelevantCols[i]));
          break;
        }
      }
      if(Predicate.Eval()){ SelectedRows.Add(RowI.GetRowIdx());}
    }
  }
}

void TTable::Classify(TPredicate& Predicate, const TStr& LabelName, const TInt& PositiveLabel, const TInt& NegativeLabel){
  TIntV SelectedRows;
  Select(Predicate, SelectedRows, false);
  ClassifyAux(SelectedRows, LabelName, PositiveLabel, NegativeLabel);
}


// Further optimization: both comparison operation and type of columns don't change between rows..
void TTable::SelectAtomic(const TStr& Col1, const TStr& Col2, TPredComp Cmp, TIntV& SelectedRows, TBool Remove){
  const TAttrType Ty1 = GetColType(Col1);
  const TAttrType Ty2 = GetColType(Col2);
  const TInt ColIdx1 = GetColIdx(Col1);
  const TInt ColIdx2 = GetColIdx(Col2);
  if(Ty1 != Ty2){
    TExcept::Throw("SelectAtomic: diff types");
  }
  if(Cmp == SUBSTR || Cmp == SUPERSTR){Assert(Ty1 == atStr);}

  if(Remove){
    TRowIteratorWithRemove RowI = BegRIWR();
    while(RowI.GetNextRowIdx() != Last){
      TBool Result;
      switch(Ty1){
        case atInt:
          Result = TPredicate::EvalAtom(RowI.GetNextIntAttr(ColIdx1), RowI.GetNextIntAttr(ColIdx2), Cmp);
          break;
        case atFlt:
          Result = TPredicate::EvalAtom(RowI.GetNextFltAttr(ColIdx1), RowI.GetNextFltAttr(ColIdx2), Cmp);
          break;
        case atStr:
          Result = TPredicate::EvalStrAtom(RowI.GetNextStrAttr(ColIdx1), RowI.GetNextStrAttr(ColIdx2), Cmp);
          break;
      }
      if(!Result){ 
        RowI.RemoveNext();
      } else{
        RowI++;
      }
    }
  } else{
    for(TRowIterator RowI = BegRI(); RowI < EndRI(); RowI++){
      TBool Result;
      switch(Ty1){
        case atInt:
          Result = TPredicate::EvalAtom(RowI.GetIntAttr(Col1), RowI.GetIntAttr(Col2), Cmp);
          break;
        case atFlt:
          Result = TPredicate::EvalAtom(RowI.GetFltAttr(Col1), RowI.GetFltAttr(Col2), Cmp);
          break;
        case atStr:
          Result = TPredicate::EvalStrAtom(RowI.GetStrAttr(Col1), RowI.GetStrAttr(Col2), Cmp);
          break;
      }
      if(Result){ SelectedRows.Add(RowI.GetRowIdx());}
    }
  }
}

void TTable::ClassifyAtomic(const TStr& Col1, const TStr& Col2, TPredComp Cmp, const TStr& LabelName, const TInt& PositiveLabel, const TInt& NegativeLabel){
  TIntV SelectedRows;
  SelectAtomic(Col1, Col2, Cmp, SelectedRows, false);
  ClassifyAux(SelectedRows, LabelName, PositiveLabel, NegativeLabel);
}

void TTable::SelectAtomicIntConst(const TStr& Col1, const TInt& Val2, TPredComp Cmp, TIntV& SelectedRows, TBool Remove){
  Assert(Cmp < SUBSTR);
  TAttrType Ty1;
  TInt ColIdx1;

  Ty1 = GetColType(Col1);
  ColIdx1 = GetColIdx(Col1);
  if(Ty1 != atInt){TExcept::Throw("SelectAtomic: not type TInt");}

  if(Remove){
    TRowIteratorWithRemove RowI = BegRIWR();
    while(RowI.GetNextRowIdx() != Last){
      if(!TPredicate::EvalAtom(RowI.GetNextIntAttr(ColIdx1), Val2, Cmp)){
        RowI.RemoveNext();
      } else{
        RowI++;
      }
    }
  } else{
    for(TRowIterator RowI = BegRI(); RowI < EndRI(); RowI++){
      if(TPredicate::EvalAtom(RowI.GetIntAttr(Col1), Val2, Cmp)){ 
        SelectedRows.Add(RowI.GetRowIdx());
      }
    }
  }
}

void TTable::ClassifyAtomicIntConst(const TStr& Col1, const TInt& Val2, TPredComp Cmp, const TStr& LabelName, const TInt& PositiveLabel, const TInt& NegativeLabel){
  TIntV SelectedRows;
  SelectAtomicIntConst(Col1, Val2, Cmp, SelectedRows, false);
  ClassifyAux(SelectedRows, LabelName, PositiveLabel, NegativeLabel);
}

void TTable::SelectAtomicStrConst(const TStr& Col1, const TStr& Val2, TPredComp Cmp, TIntV& SelectedRows, TBool Remove){
  TAttrType Ty1;
  TInt ColIdx1;

  Ty1 = GetColType(Col1);
  ColIdx1 = GetColIdx(Col1);
  if(Ty1 != atStr){TExcept::Throw("SelectAtomic: not type TStr");}

  if(Remove){
    TRowIteratorWithRemove RowI = BegRIWR();
    while(RowI.GetNextRowIdx() != Last){
      if(!TPredicate::EvalStrAtom(RowI.GetNextStrAttr(ColIdx1), Val2, Cmp)){
        RowI.RemoveNext();
      } else{
        RowI++;
      }
    }
  } else{
    for(TRowIterator RowI = BegRI(); RowI < EndRI(); RowI++){
      if(TPredicate::EvalStrAtom(RowI.GetStrAttr(Col1), Val2, Cmp)){ 
        SelectedRows.Add(RowI.GetRowIdx());
      }
    }
  }
}

void TTable::ClassifyAtomicStrConst(const TStr& Col1, const TStr& Val2, TPredComp Cmp, const TStr& LabelName, const TInt& PositiveLabel, const TInt& NegativeLabel){
  TIntV SelectedRows;
  SelectAtomicStrConst(Col1, Val2, Cmp, SelectedRows, false);
  ClassifyAux(SelectedRows, LabelName, PositiveLabel, NegativeLabel);
}

void TTable::SelectAtomicFltConst(const TStr& Col1, const TFlt& Val2, TPredComp Cmp, TIntV& SelectedRows, TBool Remove){
  Assert(Cmp < SUBSTR);
  TAttrType Ty1;
  TInt ColIdx1;

  Ty1 = GetColType(Col1);
  ColIdx1 = GetColIdx(Col1);
  if(Ty1 != atFlt){TExcept::Throw("SelectAtomic: not type TFlt");}

  if(Remove){
    TRowIteratorWithRemove RowI = BegRIWR();
    while(RowI.GetNextRowIdx() != Last){
      if(!TPredicate::EvalAtom(RowI.GetNextFltAttr(ColIdx1), Val2, Cmp)){
        RowI.RemoveNext();
      } else{
        RowI++;
      }
    }
  } else{
    for(TRowIterator RowI = BegRI(); RowI < EndRI(); RowI++){
      if(TPredicate::EvalAtom(RowI.GetFltAttr(Col1), Val2, Cmp)){ 
        SelectedRows.Add(RowI.GetRowIdx());
      }
    }
  }
}

void TTable::ClassifyAtomicFltConst(const TStr& Col1, const TFlt& Val2, TPredComp Cmp, const TStr& LabelName, const TInt& PositiveLabel, const TInt& NegativeLabel){
  TIntV SelectedRows;
  SelectAtomicFltConst(Col1, Val2, Cmp, SelectedRows, false);
  ClassifyAux(SelectedRows, LabelName, PositiveLabel, NegativeLabel);
}

inline TInt TTable::CompareRows(TInt R1, TInt R2, const TAttrType& CompareByType, const TInt& CompareByIndex, TBool Asc){
  //printf("comparing rows %d %d by %s\n", R1.Val, R2.Val, CompareBy.CStr());
  switch(CompareByType){
    case atInt:{
      if(IntCols[CompareByIndex][R1] > IntCols[CompareByIndex][R2]) return (Asc ? 1 : -1);
      if(IntCols[CompareByIndex][R1] < IntCols[CompareByIndex][R2]) return (Asc ? -1 : 1);
      return 0;
    }
    case atFlt:{
      if(FltCols[CompareByIndex][R1] > FltCols[CompareByIndex][R2]) return (Asc ? 1 : -1);
      if(FltCols[CompareByIndex][R1] < FltCols[CompareByIndex][R2]) return (Asc ? -1 : 1);
      return 0;
    }
    case atStr:{
      TStr S1 = GetStrVal(CompareByIndex, R1);
      TStr S2 = GetStrVal(CompareByIndex, R2);
      int CmpRes = strcmp(S1.CStr(), S2.CStr());
      return (Asc ? CmpRes : -CmpRes);
    }
  }
  // code should not come here, added to remove a compiler warning
  return 0;
}

inline TInt TTable::CompareRows(TInt R1, TInt R2, const TVec<TAttrType>& CompareByTypes, const TIntV& CompareByIndices, TBool Asc){
  for(TInt i = 0; i < CompareByTypes.Len(); i++){
    TInt res = CompareRows(R1, R2, CompareByTypes[i], CompareByIndices[i], Asc);
    if(res != 0){ return res;}
  }
  return 0;
}

void TTable::ISort(TIntV& V, TInt StartIdx, TInt EndIdx, const TVec<TAttrType>& SortByTypes, const TIntV& SortByIndices, TBool Asc){
  if(StartIdx < EndIdx){
    for(TInt i = StartIdx+1; i <= EndIdx; i++){
      TInt Val = V[i];
      TInt j = i;
      while((StartIdx < j) && (CompareRows(V[j-1], Val, SortByTypes, SortByIndices, Asc) > 0)){
        V[j] = V[j-1];
        j--;
      }
      V[j] = Val;
    }
  }
}

TInt TTable::GetPivot(TIntV& V, TInt StartIdx, TInt EndIdx, const TVec<TAttrType>& SortByTypes, const TIntV& SortByIndices, TBool Asc){
  TInt L = EndIdx - StartIdx + 1;
  const TInt Idx1 = StartIdx + TInt::GetRnd(L);
  const TInt Idx2 = StartIdx + TInt::GetRnd(L);
  const TInt Idx3 = StartIdx + TInt::GetRnd(L);
  if(CompareRows(V[Idx1], V[Idx2], SortByTypes, SortByIndices, Asc) < 0){
    if(CompareRows(V[Idx2], V[Idx3], SortByTypes, SortByIndices, Asc) < 0){ return Idx2;}
    if(CompareRows(V[Idx1], V[Idx3], SortByTypes, SortByIndices, Asc) < 0){ return Idx3;}
    return Idx1;
  } else{
    if(CompareRows(V[Idx3], V[Idx2], SortByTypes, SortByIndices, Asc) < 0){ return Idx2;}
    if(CompareRows(V[Idx3], V[Idx1], SortByTypes, SortByIndices, Asc) < 0){ return Idx3;}
    return Idx1;
  }
}

TInt TTable::Partition(TIntV& V, TInt StartIdx, TInt EndIdx, const TVec<TAttrType>& SortByTypes, const TIntV& SortByIndices, TBool Asc){
  TInt PivotIdx = GetPivot(V, StartIdx, EndIdx, SortByTypes, SortByIndices, Asc);
  TInt Pivot = V[PivotIdx];
  V.Swap(PivotIdx, EndIdx);
  TInt StoreIdx = StartIdx;
  for(TInt i = StartIdx; i < EndIdx; i++){
    if(CompareRows(V[i], Pivot, SortByTypes, SortByIndices, Asc) <= 0){
      V.Swap(i, StoreIdx);
      StoreIdx++;
    }
  }
  // move pivot value to its place
  V.Swap(StoreIdx, EndIdx);
  return StoreIdx;
}

void TTable::QSort(TIntV& V, TInt StartIdx, TInt EndIdx, const TVec<TAttrType>& SortByTypes, const TIntV& SortByIndices, TBool Asc){
  if(StartIdx < EndIdx){
    if(EndIdx - StartIdx < 20){
      ISort(V, StartIdx, EndIdx, SortByTypes, SortByIndices, Asc);
    } else{
      TInt Pivot = Partition(V, StartIdx, EndIdx, SortByTypes, SortByIndices, Asc);
      QSort(V, StartIdx, Pivot-1, SortByTypes, SortByIndices, Asc);
      QSort(V, Pivot+1, EndIdx, SortByTypes, SortByIndices, Asc);
    }
  }
}

void TTable::Order(const TStrV& OrderBy, const TStr& OrderColName, TBool ResetRankByMSC, TBool Asc){
  // get a vector of all valid row indices
  TIntV ValidRows = TIntV(NumValidRows);
  if(NumRows == NumValidRows){
    for(TInt i = 0; i < NumValidRows; i++){
      ValidRows[i] = i;
    }
  } else{
    TInt i = 0;
    for(TRowIterator RI = BegRI(); RI < EndRI(); RI++){
      ValidRows[i] = RI.GetRowIdx();
      i++;
    }
  }
  TVec<TAttrType> OrderByTypes(OrderBy.Len());
  TIntV OrderByIndices(OrderBy.Len());
  for(TInt i = 0; i < OrderBy.Len(); i++){
    OrderByTypes[i] = GetColType(OrderBy[i]);
    OrderByIndices[i] = GetColIdx(OrderBy[i]);
  }
  // sort that vector according to the attributes given in "OrderBy" in lexicographic order
  QSort(ValidRows, 0, NumValidRows-1, OrderByTypes, OrderByIndices, Asc);
  // rewire Next vector
  if (NumValidRows > 0){
    FirstValidRow = ValidRows[0];
  } else{
    FirstValidRow = Last;
  }
  for(TInt i = 0; i < NumValidRows-1; i++){
    Next[ValidRows[i]] = ValidRows[i+1];
  }
  if (NumValidRows > 0){
    Next[ValidRows[NumValidRows-1]] = Last;
    LastValidRow = NumValidRows-1;
  } else {
    LastValidRow = Last;
  }

  // add rank column
  if(!OrderColName.Empty()){
    TIntV RankCol = TIntV(NumRows);
    for(TInt i = 0; i < NumValidRows; i++){
      RankCol[ValidRows[i]] = i;
    }
    if(ResetRankByMSC){
      for(TInt i = 1; i < NumValidRows; i++){
        TStr GroupName = OrderBy[0];
        if(GetStrVal(GroupName, ValidRows[i]) != GetStrVal(GroupName, ValidRows[i-1])){ 
          RankCol[ValidRows[i]] = 0;
        } else{
          RankCol[ValidRows[i]] = RankCol[ValidRows[i-1]] + 1;
        }
      }
    }
    IntCols.Add(RankCol);
    AddSchemaCol(OrderColName, atInt);
    ColTypeMap.AddDat(OrderColName, TPair<TAttrType,TInt>(atInt, IntCols.Len()-1));
  }
}

void TTable::Defrag() {
  TInt FreeIndex = 0;
  TIntV Mapping;  // Mapping[old_index] = new_index/invalid

  TInt IdColIdx = ColTypeMap.GetDat(IdColName).Val2;

  for (TInt i = 0; i < Next.Len(); i++) {
    if (Next[i] != TTable::Invalid) {  
      // "first row" properly set beforehand
      if (FreeIndex == 0) {
        Assert (i == FirstValidRow);
        FirstValidRow = 0;
      }
 
      if (Next[i] != Last) { 
        Next[FreeIndex] = FreeIndex + 1;
        Mapping.Add(FreeIndex);
      } else {
        Next[FreeIndex] = Last;
        LastValidRow = FreeIndex;
        Mapping.Add(Last);
      }

      RowIdMap.AddDat(IntCols[IdColIdx][i], FreeIndex);

      for (TInt j = 0; j < IntCols.Len(); j++) {
        IntCols[j][FreeIndex] = IntCols[j][i];
      }
      for (TInt j = 0; j < FltCols.Len(); j++) {
        FltCols[j][FreeIndex] = FltCols[j][i];
      }
      for (TInt j = 0; j < StrColMaps.Len(); j++) {
        StrColMaps[j][FreeIndex] = StrColMaps[j][i];
      }

      FreeIndex++;
    } else {
      NumRows--;
      Mapping.Add(TTable::Invalid);
    }
  }

  // should match, or bug somewhere
  Assert (NumValidRows == NumRows);
}

// // Currently support only the non-bipartite case
// // wrong reading of string attributes
// void TTable::BuildGraphTopology(PNEANet& Graph) {
//   const TAttrType SrCT = GetColType(SrcCol);
//   const TInt SrIdx = GetColIdx(SrcCol);
//   const TAttrType DsCT = GetColType(DstCol);
//   const TInt DsIdx = GetColIdx(DstCol);
//   // debug
//   /*
//   for(TInt i = 0; i < IntNodeVals.Len(); i++){
//     for(TInt j = i+1; j < IntNodeVals.Len(); j++){
//       if(IntNodeVals[i] == IntNodeVals[j]){printf("bug: %d %d %d\n", i, j, IntNodeVals[i]);}
//     }
//   }
//   */
//   // Add Nodes - The non-bipartite case
//   Assert(SrCT == DsCT); 
//   switch(SrCT){
//     case atInt:{
//       for(TInt i = 0; i < IntNodeVals.Len(); i++){
//         if(IntNodeVals[i] == -1){ continue;}  // illegal value
//         Graph->AddNode(IntNodeVals[i]);
//       }
//       break;
//     }
//     case atFlt:{
//       for(THash<TFlt,TInt>::TIter it = FltNodeVals.BegI(); it < FltNodeVals.EndI(); it++){
//         Graph->AddNode(it.GetDat());
//       }
//       break;
//     }
//     case atStr:{
//       for(TInt i = 0; i < StrNodeVals.Len(); i++){
//         if(strlen(Context.StringVals.GetKey(StrNodeVals[i])) == 0){ continue;}  //illegal value
//         Graph->AddNode(StrNodeVals[i]);
//       }
//       break;
//     }
//   }
  
//   for(TRowIterator RowI = BegRI(); RowI < EndRI(); RowI++) {
//     if (SrCT == atInt && DsCT == atInt) {
//       TInt Sval = IntCols[SrIdx][RowI.GetRowIdx()];
//       TInt Dval = IntCols[DsIdx][RowI.GetRowIdx()];
//       if(Sval == -1 || Dval == -1){ continue;}  // illegal value
//       Graph->AddEdge(Sval, Dval, RowI.GetRowIdx());
//     } else if (SrCT == atInt && DsCT == atFlt) {
//       TFlt val = FltCols[DsIdx][RowI.GetRowIdx()];
//       TInt Sval = IntCols[SrIdx][RowI.GetRowIdx()];
//       TInt Dval = FltNodeVals.GetDat(val);
//       if(Sval == -1 || Dval == -1 || val.Val == -1){ continue;}  // illegal value
//       Graph->AddEdge(Sval, Dval, RowI.GetRowIdx());
//     } else if (SrCT == atInt && DsCT == atStr) {
//       TInt Sval = IntCols[SrIdx][RowI.GetRowIdx()];
//       TInt Dval = StrColMaps[DsIdx][RowI.GetRowIdx()];
//       if(Sval == -1 || Dval == -1 || strlen(Context.StringVals.GetKey(Dval)) == 0){ continue;}  // illegal value
//       Graph->AddEdge(Sval, Dval, RowI.GetRowIdx());
//     } else if (SrCT == atFlt && DsCT == atInt) {
//       TFlt val = FltCols[SrIdx][RowI.GetRowIdx()];
//       TInt Sval = FltNodeVals.GetDat(val);
//       TInt Dval = IntCols[SrIdx][RowI.GetRowIdx()];
//       if(Sval == -1 || Dval == -1 || val.Val == -1){ continue;}  // illegal value
//       Graph->AddEdge(Sval, Dval, RowI.GetRowIdx());
//     } else if (SrCT == atFlt && DsCT == atStr) {
//       TFlt val = FltCols[SrIdx][RowI.GetRowIdx()];
//       TInt Sval = FltNodeVals.GetDat(val);
//       TInt Dval = StrColMaps[SrIdx][RowI.GetRowIdx()];
//       if(Sval == -1 || Dval == -1 || val.Val == -1 || strlen(Context.StringVals.GetKey(Dval)) == 0){ continue;}  // illegal value
//       Graph->AddEdge(Sval, Dval, RowI.GetRowIdx());
//     } else if (SrCT == atFlt && DsCT == atFlt) {
//       TFlt val1 = FltCols[SrIdx][RowI.GetRowIdx()];
//       TFlt val2 = FltCols[DsIdx][RowI.GetRowIdx()];
//       TInt Sval = FltNodeVals.GetDat(val1);
//       TInt Dval = FltNodeVals.GetDat(val2);
//       if(Sval == -1 || Dval == -1 || val1.Val == -1 || val2.Val == -1){ continue;} // illegal value
//       Graph->AddEdge(Sval, Dval, RowI.GetRowIdx());
//     }
//     else if(SrCT == atStr && DsCT == atInt){
//       TInt Sval = StrColMaps[SrIdx][RowI.GetRowIdx()];
//       TInt Dval = IntCols[DsIdx][RowI.GetRowIdx()];
//       if(Sval == -1 || Dval == -1 || strlen(Context.StringVals.GetKey(Sval)) == 0){ continue;} // illegal value
//       Graph->AddEdge(Sval, Dval, RowI.GetRowIdx());
//     } else if(SrCT == atStr && DsCT == atFlt){
//       TFlt val = FltCols[DsIdx][RowI.GetRowIdx()];
//       TInt Sval = StrColMaps[SrIdx][RowI.GetRowIdx()];
//       TInt Dval = FltNodeVals.GetDat(val);
//       if(Sval == -1 || Dval == -1 || strlen(Context.StringVals.GetKey(Sval)) == 0 || val.Val == -1){ continue;} // illegal value
//       Graph->AddEdge(Sval, Dval, RowI.GetRowIdx());
//     } else if(SrCT == atStr && DsCT == atStr){
//       TInt Sval = StrColMaps[SrIdx][RowI.GetRowIdx()];
//       TInt Dval = StrColMaps[DsIdx][RowI.GetRowIdx()];
//       if(Sval == -1 || Dval == -1 || strlen(Context.StringVals.GetKey(Sval)) == 0 || strlen(Context.StringVals.GetKey(Dval)) == 0){ continue;} // illegal value
//       Graph->AddEdge(Sval, Dval, RowI.GetRowIdx());
//     }
//   }
// }

// TInt TTable::GetNId(const TStr& Col, TInt RowIdx) {
//   Assert(Col == SrcCol || Col == DstCol);
//   TAttrType CT = GetColType(Col);
//   TInt Idx = GetColIdx(Col);
//   if (CT == atInt) {
//     return IntCols[Idx][RowIdx];
//   } else if (CT == atFlt) {
//     return FltNodeVals.GetDat(FltCols[Idx][RowIdx]);
//   } else {
//     return StrColMaps[Idx][RowIdx]();
//   }
//   return 0;
// }

// // assuming SrcCol and DstCol values belong to the same "universe" of possible node values
// void TTable::GraphPrep(){
//  THash<TInt,TInt> IntVals;
//  THash<TInt,TInt> StrVals;  // keys are StrColMaps mappings
//  const TAttrType NodeType = GetColType(SrcCol);
//  const TInt SrcColIdx = GetColIdx(SrcCol);
//  const TInt DstColIdx = GetColIdx(DstCol);
//  Assert(NodeType == GetColType(DstCol));
//  for(TRowIterator it = BegRI(); it < EndRI(); it++){
//    switch(NodeType){
//      case atInt:{
//        TInt Sval = it.GetIntAttr(SrcCol);
//        if(!IntVals.IsKey(Sval)){
//          IntVals.AddKey(Sval);
//          IntNodeVals.Add(Sval);
//        }
//        TInt Dval = it.GetIntAttr(DstCol);
//        if(!IntVals.IsKey(Dval)){
//          IntVals.AddKey(Dval);
//          IntNodeVals.Add(Dval);
//        }
//        break;
//      }
//      case atFlt:{
//        TFlt Sval = it.GetFltAttr(SrcCol);
//        if(!FltNodeVals.IsKey(Sval)){
//          TInt Val = FltNodeVals.Len();
//          FltNodeVals.AddKey(Sval);
//          FltNodeVals.AddDat(Sval, Val);
//        }
//        TFlt Dval = it.GetFltAttr(DstCol);
//        if(!FltNodeVals.IsKey(Dval)){
//          TInt Val = FltNodeVals.Len();
//          FltNodeVals.AddKey(Dval);
//          FltNodeVals.AddDat(Dval, Val);
//        }
//        break;
//      }
//      case atStr:{
//        TInt Sval = StrColMaps[SrcColIdx][it.GetRowIdx()];
//        if(!StrVals.IsKey(Sval)){
//          StrVals.AddKey(Sval);
//          StrNodeVals.Add(Sval);
//        }
//        TInt Dval = StrColMaps[DstColIdx][it.GetRowIdx()];
//        if(!StrVals.IsKey(Dval)){
//          StrVals.AddKey(Dval);
//          StrNodeVals.Add(Dval);
//        }
//        break;
//      }
//    }
//  }
// }

// PNEANet TTable::ToGraph(ATTR_AGGR AttrAggrPolicy) {
//   //printf("starting graph prep\n");
//   GraphPrep();
//   PNEANet Graph = PNEANet::New();
//   THash<TFlt, TInt> FSrNodeMap = THash<TFlt, TInt>();
//   THash<TFlt, TInt> FDsNodeMap = THash<TFlt, TInt>();
//   //printf("starting to build graph topology\n");
//   BuildGraphTopology(Graph);
//   //printf("starting to add edge attributes\n");
//   AddEdgeAttributes(Graph);
//   //printf("starting to add node attributes\n");
//   AddNodeAttributes(Graph, AttrAggrPolicy);
//   return Graph;
// }

void TTable::CheckAndAddIntNode(PNEANet Graph, THashSet<TInt>& NodeVals, TInt NodeId) {
  if(!NodeVals.IsKey(NodeId)){
    Graph->AddNode(NodeId);
    NodeVals.AddKey(NodeId);
  }
}

TInt TTable::CheckAndAddFltNode(PNEANet Graph, THash<TFlt, TInt>& NodeVals, TFlt FNodeVal) {
  if(!NodeVals.IsKey(FNodeVal)){
    TInt NodeVal = NodeVals.Len();
    Graph->AddNode(NodeVal);
    NodeVals.AddKey(FNodeVal);
    NodeVals.AddDat(FNodeVal, NodeVal);
    return NodeVal;
  } else { return NodeVals.GetDat(FNodeVal);}
}

void TTable::AddEdgeAttributes(PNEANet& Graph, int RowId) {
  for (int i = 0; i < EdgeAttrV.Len(); i++) {
    TStr ColName = EdgeAttrV[i];
    TAttrType T = GetColType(ColName);
    TInt Index = GetColIdx(ColName);
    switch (T) {
      case atInt:
        Graph->AddIntAttrDatE(RowId, IntCols[Index][RowId], ColName);
        break;
      case atFlt:
        Graph->AddFltAttrDatE(RowId, FltCols[Index][RowId], ColName);
        break;
      case atStr:
        Graph->AddStrAttrDatE(RowId, GetStrVal(Index, RowId), ColName);
        break;
    }
  }
}

void TTable::AddNodeAttributes(TInt NId, TStrV NodeAttrV, TInt RowId, THash<TInt, TStrIntVH>& NodeIntAttrs, 
    THash<TInt, TStrFltVH>& NodeFltAttrs, THash<TInt, TStrStrVH>& NodeStrAttrs){
  for (int i = 0; i < NodeAttrV.Len(); i++) {
    TStr ColAttr = NodeAttrV[i];
    TAttrType CT = GetColType(ColAttr);
    int ColId = GetColIdx(ColAttr);
    // check if this is a common src-dst attribute
    for(TInt i = 0; i < CommonNodeAttrs.Len(); i++){
      if(CommonNodeAttrs[i].Val1 == ColAttr || CommonNodeAttrs[i].Val2 == ColAttr){
        ColAttr = CommonNodeAttrs[i].Val3;
        break;
      }
    }
    if (CT == atInt) {
      if(!NodeIntAttrs.IsKey(NId)){ NodeIntAttrs.AddKey(NId);}
      if(!NodeIntAttrs.GetDat(NId).IsKey(ColAttr)){ NodeIntAttrs.GetDat(NId).AddKey(ColAttr);}
      NodeIntAttrs.GetDat(NId).GetDat(ColAttr).Add(IntCols[ColId][RowId]);
    } else if (CT == atFlt) {
      if(!NodeFltAttrs.IsKey(NId)){ NodeFltAttrs.AddKey(NId);}
      if(!NodeFltAttrs.GetDat(NId).IsKey(ColAttr)){ NodeFltAttrs.GetDat(NId).AddKey(ColAttr);}
      NodeFltAttrs.GetDat(NId).GetDat(ColAttr).Add(FltCols[ColId][RowId]);
    } else {
      if(!NodeStrAttrs.IsKey(NId)){ NodeStrAttrs.AddKey(NId);}
      if(!NodeStrAttrs.GetDat(NId).IsKey(ColAttr)){ NodeStrAttrs.GetDat(NId).AddKey(ColAttr);}
      NodeStrAttrs.GetDat(NId).GetDat(ColAttr).Add(GetStrVal(ColId, RowId));
    }
  }
}

// // TODO: This is incredibly wasteful, as we iterate over the edges to update node attribute values
// // should instead keep a map NId->attr_vals for the attribute we want to keep track of
// // assuming that the number of nodes before any table operations is significantly
// // smaller than the number of edges in the final graph
// void TTable::AddNodeAttributes(PNEANet& Graph, ATTR_AGGR Policy) {
//     THash<TStr, TIntIntVH> NodeIntAttrs;
//     THash<TStr, TIntFltVH> NodeFltAttrs;
//     THash<TStr, TIntStrVH> NodeStrAttrs;
//     AddNodeAttributesAux(NodeIntAttrs, NodeFltAttrs, NodeStrAttrs, true);
//     AddNodeAttributesAux(NodeIntAttrs, NodeFltAttrs, NodeStrAttrs, false);
//     // TODO: aggregation policy per attribute
//     for(THash<TStr, TIntIntVH>::TIter it = NodeIntAttrs.BegI(); it < NodeIntAttrs.EndI(); it++){
//       for(TIntIntVH::TIter i = it.GetDat().BegI(); i < it.GetDat().EndI(); i++){
//         TInt AttrVal = AggregateVector<TInt>(i.GetDat(), Policy);
//         Graph->AddIntAttrDatN(i.GetKey(), AttrVal, it.GetKey());
//       }
//     }
//     for(THash<TStr, TIntFltVH>::TIter it = NodeFltAttrs.BegI(); it < NodeFltAttrs.EndI(); it++){
//       for(TIntFltVH::TIter i = it.GetDat().BegI(); i < it.GetDat().EndI(); i++){
//         TFlt AttrVal = AggregateVector<TFlt>(i.GetDat(), Policy);
//         Graph->AddFltAttrDatN(i.GetKey(), AttrVal, it.GetKey());
//       }
//     }
//     for(THash<TStr, TIntStrVH>::TIter it = NodeStrAttrs.BegI(); it < NodeStrAttrs.EndI(); it++){
//       for(TIntStrVH::TIter i = it.GetDat().BegI(); i < it.GetDat().EndI(); i++){
//         TStr AttrVal = (Policy == FIRST) ? i.GetDat()[0] : i.GetDat()[i.GetDat().Len()-1];
//         Graph->AddStrAttrDatN(i.GetKey(), AttrVal, it.GetKey());
//       }
//     }
// }

// Makes one pass over all the rows in the vector RowIds, and builds
// a PNEANet, with each row as an edge between SrcCol and DstCol.
PNEANet TTable::BuildGraph(const TIntV& RowIds, TAttrAggr AggrPolicy) {
  PNEANet Graph = PNEANet::New();
  
  const TAttrType NodeType = GetColType(SrcCol);
  Assert(NodeType == GetColType(DstCol));
  const TInt SrcColIdx = GetColIdx(SrcCol);
  const TInt DstColIdx = GetColIdx(DstCol);
  
  // node values - i.e. the unique values of src/dst col
  THashSet<TInt> IntNodeVals; // for both int and string node attr types.
  THash<TFlt, TInt> FltNodeVals;

  // node attributes
  THash<TInt, TStrIntVH> NodeIntAttrs;
  THash<TInt, TStrFltVH> NodeFltAttrs;
  THash<TInt, TStrStrVH> NodeStrAttrs;

  // make single pass over all rows in given row id set
  for(int i = 0; i < RowIds.Len(); i++){
    TRowIterator it = TRowIterator(RowIds[i], this);

    // add src and dst nodes to graph if they are not seen earlier
    TInt SVal, DVal;
    if (NodeType == atFlt) {
      TFlt FSVal = it.GetFltAttr(SrcCol);
      SVal = CheckAndAddFltNode(Graph, FltNodeVals, FSVal);
      TFlt FDVal = it.GetFltAttr(DstCol);
      DVal = CheckAndAddFltNode(Graph, FltNodeVals, FDVal);
    } else if (NodeType == atInt || NodeType == atStr) {
      if (NodeType == atInt) {
        SVal = it.GetIntAttr(SrcCol);
        DVal = it.GetIntAttr(DstCol);
      } else if (NodeType == atStr) {
        SVal = StrColMaps[SrcColIdx][RowIds[i]];
        if(strlen(Context.StringVals.GetKey(SVal)) == 0){ continue;}  //illegal value
        DVal = StrColMaps[DstColIdx][RowIds[i]];
        if(strlen(Context.StringVals.GetKey(DVal)) == 0){ continue;}  //illegal value
      }
      CheckAndAddIntNode(Graph, IntNodeVals, SVal);
      CheckAndAddIntNode(Graph, IntNodeVals, DVal);
    } 

    // add edge and edge attributes 
    Graph->AddEdge(SVal, DVal, RowIds[i]);
    AddEdgeAttributes(Graph, RowIds[i]);

    // get src and dst node attributes into hashmaps
    AddNodeAttributes(SVal, SrcNodeAttrV, RowIds[i], NodeIntAttrs, NodeFltAttrs, NodeStrAttrs);
    AddNodeAttributes(DVal, DstNodeAttrV, RowIds[i], NodeIntAttrs, NodeFltAttrs, NodeStrAttrs);
  }

  // aggregate node attributes and add to graph
  for (TNEANet::TNodeI NodeI = Graph->BegNI(); NodeI < Graph->EndNI(); NodeI++) {
    TInt NId = NodeI.GetId();
    if (NodeIntAttrs.IsKey(NId)) {
      TStrIntVH IntAttrVals = NodeIntAttrs.GetDat(NId);
      for(TStrIntVH::TIter it = IntAttrVals.BegI(); it < IntAttrVals.EndI(); it++){
        TInt AttrVal = AggregateVector<TInt>(it.GetDat(), AggrPolicy);
        Graph->AddIntAttrDatN(NId, AttrVal, it.GetKey());
      }
    }
    if (NodeFltAttrs.IsKey(NId)) {
      TStrFltVH FltAttrVals = NodeFltAttrs.GetDat(NId);
      for(TStrFltVH::TIter it = FltAttrVals.BegI(); it < FltAttrVals.EndI(); it++){
        TFlt AttrVal = AggregateVector<TFlt>(it.GetDat(), AggrPolicy);
        Graph->AddFltAttrDatN(NId, AttrVal, it.GetKey());
      }
    }
    if (NodeStrAttrs.IsKey(NId)) {
      TStrStrVH StrAttrVals = NodeStrAttrs.GetDat(NId);
      for(TStrStrVH::TIter it = StrAttrVals.BegI(); it < StrAttrVals.EndI(); it++){
        TStr AttrVal = AggregateVector<TStr>(it.GetDat(), AggrPolicy);
        Graph->AddStrAttrDatN(NId, AttrVal, it.GetKey());
      }
    }
  }

  return Graph;
}

PNEANet TTable::ToGraph(TAttrAggr AggrPolicy) {
  TIntV RowIds;
  for (int i = 0; i < Next.Len(); i++) {
    if (Next[i] != Invalid){ RowIds.Add(i);}
  }

  return BuildGraph(RowIds, AggrPolicy);
}

void TTable::InitRowIdBuckets(int NumBuckets) {
  for (int i = 0; i < RowIdBuckets.Len(); i++) {
    RowIdBuckets[i].Clr();
  }
  RowIdBuckets.Clr();

  RowIdBuckets.Gen(NumBuckets);
  for (int i = 0; i < NumBuckets; i++) {
    RowIdBuckets[i].Gen(10, 0);
  }
}

void TTable::FillBucketsByWindow(TStr SplitAttr, TInt JumpSize, TInt WindowSize, TInt StartVal, TInt EndVal) {
  Assert (JumpSize <= WindowSize);
  int NumBuckets, MinBucket, MaxBucket;
  TInt SplitColId = GetColIdx(SplitAttr);

  if (StartVal == TInt::Mn || EndVal == TInt::Mx){
    // calculate min and max value of the column 'SplitAttr'
    TInt MinValue = TInt::Mx;
    TInt MaxValue = TInt::Mn;
    for (int i = 0; i < Next.Len(); i++) {
      if (Next[i] != Invalid) { 
        if (MinValue > IntCols[SplitColId][i]) {
          MinValue = IntCols[SplitColId][i];
        }
        if (MaxValue < IntCols[SplitColId][i]) {
          MaxValue = IntCols[SplitColId][i];
        }
      }
    }

    if (StartVal == TInt::Mn) StartVal = MinValue;
    if (EndVal == TInt::Mx) EndVal = MaxValue;
  }

  // initialize buckets
  if (JumpSize == 0) { NumBuckets = (EndVal - StartVal)/JumpSize + 1;}
  else { NumBuckets = (EndVal - StartVal)/JumpSize + 1;}

  InitRowIdBuckets(NumBuckets);

  // populate RowIdSets by computing the range of buckets for each row
  for (int i = 0; i < Next.Len(); i++) {
    if (Next[i] == Invalid) {continue;}
    int SplitVal = IntCols[SplitColId][i];
    if (SplitVal < StartVal || SplitVal > EndVal){ continue;}
    if (JumpSize == 0) { // expanding windows
      MinBucket = (SplitVal-StartVal)/WindowSize;
      MaxBucket = NumBuckets-1;
    } else { // sliding/disjoint windows
      MinBucket = max(0,(SplitVal-WindowSize-StartVal)/JumpSize + 1);
      MaxBucket = (SplitVal - StartVal)/JumpSize;  
    }
    for (int j = MinBucket; j <= MaxBucket; j++){ RowIdBuckets[j].Add(i);}
  }
}

void TTable::FillBucketsByInterval(TStr SplitAttr, TIntPrV SplitIntervals) {
  TInt SplitColId = GetColIdx(SplitAttr);
  int NumBuckets = SplitIntervals.Len();
  InitRowIdBuckets(NumBuckets);

  // populate RowIdSets by computing the range of buckets for each row
  for (int i = 0; i < Next.Len(); i++) {
    if (Next[i] == Invalid) {continue;}
    int SplitVal = IntCols[SplitColId][i];
    for (int j = 0; j < SplitIntervals.Len(); j++){ 
      if (SplitVal >= SplitIntervals[j].Val1 && SplitVal < SplitIntervals[j].Val2){
        RowIdBuckets[j].Add(i);
      }
    }
  }
}

TVec<PNEANet> TTable::GetGraphsFromSequence(TAttrAggr AggrPolicy) {
  //call BuildGraph on each row id set - parallelizable!
  TVec<PNEANet> GraphSequence;
  for (int i = 0; i < RowIdBuckets.Len(); i++) {
    if (RowIdBuckets[i].Len() == 0){ continue;}
    PNEANet PNet = BuildGraph(RowIdBuckets[i], AggrPolicy);
    GraphSequence.Add(PNet);
  }

  return GraphSequence;
}

PNEANet TTable::GetFirstGraphFromSequence(TAttrAggr AggrPolicy) {
  CurrBucket = -1;
  this->AggrPolicy = AggrPolicy;
  return GetNextGraphFromSequence();
}

PNEANet TTable::GetNextGraphFromSequence() {
  CurrBucket++;
  while (CurrBucket < RowIdBuckets.Len() && RowIdBuckets[CurrBucket].Len() == 0) {
    CurrBucket++;
  }
  if (CurrBucket >= RowIdBuckets.Len()){ return NULL;}
  return BuildGraph(RowIdBuckets[CurrBucket], AggrPolicy);
}

// Only integer SplitAttr supported
// Setting JumpSize = WindowSize will give disjoint windows
// Setting JumpSize < WindowSize will give sliding windows
// Setting JumpSize > WindowSize will drop certain rows (currently not supported)
// Setting JumpSize = 0 will give expanding windows (i.e. starting at 0 and ending at i*WindowSize)
// To set the range of values of SplitAttr to be considered, use StartVal and EndVal (inclusive)
// If StartVal == TInt.Mn, then the buckets will start from the min value of SplitAttr in the table. 
// If EndVal == TInt.Mx, then the buckets will end at the max value of SplitAttr in the table. 
TVec<PNEANet> TTable::ToGraphSequence(TStr SplitAttr, TAttrAggr AggrPolicy, TInt WindowSize, TInt JumpSize, TInt StartVal, TInt EndVal) {
  FillBucketsByWindow(SplitAttr, JumpSize, WindowSize, StartVal, EndVal);
  return GetGraphsFromSequence(AggrPolicy);  
}

TVec<PNEANet> TTable::ToVarGraphSequence(TStr SplitAttr, TAttrAggr AggrPolicy, TIntPrV SplitIntervals){
  FillBucketsByInterval(SplitAttr, SplitIntervals);
  return GetGraphsFromSequence(AggrPolicy);
}

TVec<PNEANet> TTable::ToGraphPerGroup(TStr GroupAttr, TAttrAggr AggrPolicy){
  return ToGraphSequence(GroupAttr, AggrPolicy, TInt(1), TInt(1), TInt::Mn, TInt::Mx);
}

PNEANet TTable::ToGraphSequenceIterator(TStr SplitAttr, TAttrAggr AggrPolicy, TInt WindowSize, TInt JumpSize, TInt StartVal, TInt EndVal) {
  FillBucketsByWindow(SplitAttr, JumpSize, WindowSize, StartVal, EndVal);
  return GetFirstGraphFromSequence(AggrPolicy);  
}

PNEANet TTable::ToVarGraphSequenceIterator(TStr SplitAttr, TAttrAggr AggrPolicy, TIntPrV SplitIntervals){
  FillBucketsByInterval(SplitAttr, SplitIntervals);
  return GetFirstGraphFromSequence(AggrPolicy);
}

PNEANet TTable::ToGraphPerGroupIterator(TStr GroupAttr, TAttrAggr AggrPolicy){
  return ToGraphSequenceIterator(GroupAttr, AggrPolicy, TInt(1), TInt(1), TInt::Mn, TInt::Mx);
}

// calls to this must be preceded by a call to one of the above ToGraph*Iterator functions
PNEANet TTable::NextGraphIterator(){
  return GetNextGraphFromSequence();
}

PTable TTable::GetNodeTable(const PNEANet& Network, const TStr& TableName, TTableContext& Context){
  Schema SR;
  SR.Add(TPair<TStr,TAttrType>("node_id",atInt));

  TStrV IntAttrNames;
  TStrV FltAttrNames;
  TStrV StrAttrNames;

  TNEANet::TNodeI NodeI = Network->BegNI();
  NodeI.GetIntAttrNames(IntAttrNames);
  NodeI.GetFltAttrNames(FltAttrNames);
  NodeI.GetStrAttrNames(StrAttrNames);
  for (int i = 0; i < IntAttrNames.Len(); i++){
    SR.Add(TPair<TStr,TAttrType>(IntAttrNames[i],atInt));
  }
  for (int i = 0; i < FltAttrNames.Len(); i++){
    SR.Add(TPair<TStr,TAttrType>(FltAttrNames[i],atFlt));
  }
  for (int i = 0; i < StrAttrNames.Len(); i++){
    SR.Add(TPair<TStr,TAttrType>(StrAttrNames[i],atStr));
  }

  PTable T = New(TableName, SR, Context);

  TInt Cnt = 0;
  // populate table columns
  while(NodeI < Network->EndNI()){
    T->IntCols[0].Add(NodeI.GetId());
    for (int i = 0; i < IntAttrNames.Len(); i++){
      T->IntCols[i+1].Add(Network->GetIntAttrDatN(NodeI,IntAttrNames[i]));
    }
    for (int i = 0; i < FltAttrNames.Len(); i++){
      T->FltCols[i].Add(Network->GetFltAttrDatN(NodeI,FltAttrNames[i]));
    }
    for (int i = 0; i < StrAttrNames.Len(); i++){
      T->AddStrVal(i, Network->GetStrAttrDatN(NodeI,StrAttrNames[i]));
    }
    Cnt++;
    NodeI++;
  }
  // set number of rows and "Next" vector
  T->NumRows = Cnt;
  T->NumValidRows = T->NumRows;
  T->Next = TIntV(T->NumRows,0);
  for(TInt i = 0; i < T->NumRows-1; i++){
    T->Next.Add(i+1);
  }
  T->LastValidRow = T->NumRows-1;
  T->Next.Add(Last);
  return T;
}

PTable TTable::GetEdgeTable(const PNEANet& Network, const TStr& TableName, TTableContext& Context){
  Schema SR;
  SR.Add(TPair<TStr,TAttrType>("edg_id",atInt));
  SR.Add(TPair<TStr,TAttrType>("src_id",atInt));
  SR.Add(TPair<TStr,TAttrType>("dst_id",atInt));

  TStrV IntAttrNames;
  TStrV FltAttrNames;
  TStrV StrAttrNames;

  TNEANet::TEdgeI EdgeI = Network->BegEI();
  EdgeI.GetIntAttrNames(IntAttrNames);
  EdgeI.GetFltAttrNames(FltAttrNames);
  EdgeI.GetStrAttrNames(StrAttrNames);
  for (int i = 0; i < IntAttrNames.Len(); i++){
    SR.Add(TPair<TStr,TAttrType>(IntAttrNames[i],atInt));
  }
  for (int i = 0; i < FltAttrNames.Len(); i++){
    SR.Add(TPair<TStr,TAttrType>(FltAttrNames[i],atFlt));
  }
  for (int i = 0; i < StrAttrNames.Len(); i++){
    printf("%s\n",StrAttrNames[i].CStr());
    SR.Add(TPair<TStr,TAttrType>(StrAttrNames[i],atStr));
  }

  PTable T = New(TableName, SR, Context);

  TInt Cnt = 0;
  // populate table columns
  while(EdgeI < Network->EndEI()){
    T->IntCols[0].Add(EdgeI.GetId());
    T->IntCols[1].Add(EdgeI.GetSrcNId());
    T->IntCols[2].Add(EdgeI.GetDstNId());
    for (int i = 0; i < IntAttrNames.Len(); i++){
      T->IntCols[i+3].Add(Network->GetIntAttrDatE(EdgeI,IntAttrNames[i]));
    }
    for (int i = 0; i < FltAttrNames.Len(); i++){
      T->FltCols[i].Add(Network->GetFltAttrDatE(EdgeI,FltAttrNames[i]));
    }
    for (int i = 0; i < StrAttrNames.Len(); i++){
      T->AddStrVal(i, Network->GetStrAttrDatE(EdgeI,StrAttrNames[i]));
    }
    Cnt++;
    EdgeI++;
  }
  // set number of rows and "Next" vector
  T->NumRows = Cnt;
  T->NumValidRows = T->NumRows;
  T->Next = TIntV(T->NumRows,0);
  for(TInt i = 0; i < T->NumRows-1; i++){
    T->Next.Add(i+1);
  }
  T->LastValidRow = T->NumRows-1;
  T->Next.Add(Last);
  return T;
}

PTable TTable::GetFltNodePropertyTable(const PNEANet& Network, const TStr& TableName, const TIntFltH& Property, const TStr& NodeAttrName, const TAttrType& NodeAttrType, const TStr& PropertyAttrName, TTableContext& Context){
  Schema SR;
  // Determine type of node id
  SR.Add(TPair<TStr,TAttrType>(NodeAttrName,NodeAttrType));
  SR.Add(TPair<TStr,TAttrType>(PropertyAttrName,atFlt));
  PTable T = New(TableName, SR, Context);
  TInt NodeColIdx = T->GetColIdx(NodeAttrName);
  TInt Cnt = 0;
  // populate table columns
  for (TNEANet::TNodeI NodeI = Network->BegNI(); NodeI < Network->EndNI(); NodeI++){
    switch(NodeAttrType){
      case atInt:
        T->IntCols[NodeColIdx].Add(Network->GetIntAttrDatN(NodeI,NodeAttrName));
        break;
      case atFlt:
        T->FltCols[NodeColIdx].Add(Network->GetFltAttrDatN(NodeI,NodeAttrName));
        break;
      case atStr:
        T->AddStrVal(TInt(0), Network->GetStrAttrDatN(NodeI,NodeAttrName));
        break;
    }
    T->FltCols[0].Add(Property.GetDat(NodeI.GetId()));
    Cnt++;
  }
  // set number of rows and "Next" vector
  T->NumRows = Cnt;
  T->NumValidRows = T->NumRows;
  T->Next = TIntV(T->NumRows,0);
  for(TInt i = 0; i < T->NumRows-1; i++){
    T->Next.Add(i+1);
  }
  T->LastValidRow = T->NumRows-1;
  T->Next.Add(Last);
  return T;
}

/*** Special Filters ***/
PTable TTable::IsNextK(const TStr& OrderCol, TInt K, const TStr& GroupBy, const TStr& RankColName){
  TStrV OrderBy;
  if(GroupBy.Empty()){
    OrderBy.Add(OrderCol);
  } else{
    OrderBy.Add(GroupBy);
    OrderBy.Add(OrderCol);
  }
  if(RankColName.Empty()){
    Order(OrderBy);
  } else{
    Order(OrderBy, RankColName, true);
  }
  TAttrType GroupByAttrType = GetColType(GroupBy);
  PTable T = InitializeJointTable(*this);
  for(TRowIterator RI = BegRI(); RI < EndRI(); RI++){
    TInt Succ = RI.GetRowIdx();
    TBool OutOfGroup = false;
    for(TInt i = 0; i < K; i++){
      Succ = Next[Succ];
      if(Succ == Last){break;}
      switch(GroupByAttrType){
        case atInt:
          if(GetIntVal(GroupBy, Succ) != RI.GetIntAttr(GroupBy)){ OutOfGroup = true;}
          break;
        case atFlt:
          if(GetFltVal(GroupBy, Succ) != RI.GetFltAttr(GroupBy)){ OutOfGroup = true;}
          break;
        case atStr:
          if(GetStrVal(GroupBy, Succ) != RI.GetStrAttr(GroupBy)){ OutOfGroup = true;}
          break;
      }
      if(OutOfGroup){break;}  // break out of inner for loop
      T->AddJointRow(*this, *this, RI.GetRowIdx(), Succ);
    }
  }
  return T;
}

void TTable::PrintSize(){
  double ApproxSize = 0;
  printf("Table Name: %s\n", Name.CStr());
  printf("Total number of rows: %d\n", NumRows.Val);
  printf("Number of valid rows: %d\n", NumValidRows.Val);
  ApproxSize += NumRows*sizeof(TInt)/1000.0;  // Next vector
  printf("Number of Int columns: %d\n", IntCols.Len());
  ApproxSize += IntCols.Len()*NumRows*sizeof(TInt)/1000.0;
  printf("Number of Flt columns: %d\n", FltCols.Len());
  ApproxSize += FltCols.Len()*NumRows*sizeof(TFlt)/1000.0;
  printf("Number of Str columns: %d\n", StrColMaps.Len());
  ApproxSize += StrColMaps.Len()*NumRows*sizeof(TInt)/1000.0;
  // printf("Number of Int node values : %d\n", IntNodeVals.Len());
  // ApproxSize += IntNodeVals.Len()*sizeof(TInt)/1000.0;
  // printf("Number of Flt node values : %d\n", FltNodeVals.Len());
  // ApproxSize += FltNodeVals.Len()*(sizeof(TInt) + sizeof(TFlt))/1000.0;
  // printf("Number of Str node values : %d\n", StrNodeVals.Len());
  // ApproxSize += StrNodeVals.Len()*sizeof(TInt)/1000.0;
  printf("Approximated size is %f KB\n", ApproxSize);
}


void TTable::AddTable(const TTable& T){
  //for(TInt c = 0; c < S.Len(); c++){
  //  if(S[c] != T.S[c]){ printf("(%s,%d) != (%s,%d)\n", S[c].Val1.CStr(), S[c].Val2, T.S[c].Val1.CStr(), T.S[c].Val2); TExcept::Throw("when adding tables, their schemas must match!");}
  //}
  for(TInt c = 0; c < S.Len(); c++){
    TStr ColName = GetSchemaColName(c);
    TInt ColIdx = GetColIdx(ColName);
    TInt TColIdx = T.GetColIdx(ColName);
    if (TColIdx < 0){ TExcept::Throw("when adding a table, it must contain all columns of source table!");}
    switch(GetColType(ColName)){ 
    case atInt:
       IntCols[ColIdx].AddV(T.IntCols[TColIdx]);
       break;
    case atFlt:
       FltCols[ColIdx].AddV(T.FltCols[TColIdx]);
       break;
    case atStr:
       StrColMaps[ColIdx].AddV(T.StrColMaps[TColIdx]);
       break;
    }
  }

  TIntV TNext(T.Next);
  for(TInt i = 0; i < TNext.Len(); i++){
    if(TNext[i] != Last && TNext[i] != TTable::Invalid){ TNext[i] += NumRows;}
  }

  // checks if table is empty 
  if (LastValidRow >= 0) {
    Next[LastValidRow] = T.FirstValidRow + NumRows;
  }
  Next.AddV(TNext);
  LastValidRow = NumRows + T.LastValidRow;
  NumRows += T.NumRows;
  NumValidRows += T.NumValidRows;
}

// returns physical indices of rows of given table present in our table
// we assume that schema matches exactly (including index of id cols)
void TTable::GetCollidingRows(const TTable& Table, THashSet<TInt>& Collisions) {
  TIntV UniqueVec;
  THash<TGroupKey, TPair<TInt, TIntV> >Grouping;
  TStrV GroupBy;

  // indices of columns of each type
  TIntV IntGroupByCols;
  TIntV FltGroupByCols;
  TIntV StrGroupByCols;

  TInt IKLen, FKLen, SKLen;

  // check that schemas match
  for(TInt c = 0; c < S.Len(); c++){
    if (S[c].Val1 == IdColName) {
      if (Table.S[c].Val1 != Table.GetIdColName()) {
        TExcept::Throw("GetCollidingRows: schemas do not match!");
      }
      continue;
    }
    if(S[c] != Table.S[c]){
      printf("(%s,%d) != (%s,%d)\n", S[c].Val1.CStr(), S[c].Val2, Table.S[c].Val1.CStr(), Table.S[c].Val2); 
      TExcept::Throw("GetCollidingRows: schemas do not match!");
    }
    GroupBy.Add(S[c].Val1);
    TPair<TAttrType, TInt> ColType = Table.ColTypeMap.GetDat(S[c].Val1);
    switch(ColType.Val1){
      case atInt:
        IntGroupByCols.Add(ColType.Val2);
        break;
      case atFlt:
        FltGroupByCols.Add(ColType.Val2);
        break;
      case atStr:
        StrGroupByCols.Add(ColType.Val2);
        break;
    }
  }

  IKLen = IntGroupByCols.Len();
  FKLen = FltGroupByCols.Len();
  SKLen = StrGroupByCols.Len();

  // group rows of first table
  GroupAux(GroupBy, Grouping, true, "", false, UniqueVec);

  // find colliding rows of second table
  for(TRowIterator it = Table.BegRI(); it < Table.EndRI(); it++){
    // read keys from row
    TIntV IKey(IKLen + SKLen, 0);
    TFltV FKey(FKLen, 0);

    // find group key
    for(TInt c = 0; c < IKLen; c++){
      IKey.Add(it.GetIntAttr(IntGroupByCols[c])); 
    }
    for(TInt c = 0; c < FKLen; c++){
      FKey.Add(it.GetFltAttr(FltGroupByCols[c])); 
    }
    for(TInt c = 0; c < SKLen; c++){
      IKey.Add(it.GetStrMap(StrGroupByCols[c])); 
    }
    // look for group matching the key
    TGroupKey GroupKey = TGroupKey(IKey, FKey);

    TInt RowIdx = it.GetRowIdx();
    if (Grouping.IsKey(GroupKey)) {
      // row exists in first table
      Collisions.AddKey(RowIdx);
    }
  }
}

// can ONLY be called when a table is being initialised (before IDs are allocated)
void TTable::AddRow(const TRowIterator& RI) {
  for(TInt c = 0; c < S.Len(); c++){
    TStr ColName = GetSchemaColName(c);
    if (ColName == IdColName) continue;

    TInt ColIdx = GetColIdx(ColName);

    switch(GetColType(ColName)){
    case atInt:
       IntCols[ColIdx].Add(RI.GetIntAttr(ColName));
       break;
    case atFlt:
       FltCols[ColIdx].Add(RI.GetFltAttr(ColName));
       break;
    case atStr:
       StrColMaps[ColIdx].Add(RI.GetStrMap(ColName));
       break;
    }
  }

  if (LastValidRow >= 0) {
    Next[LastValidRow] = NumRows;
  }
  Next.Add(Last);
  LastValidRow = NumRows;

  NumRows++;
  NumValidRows++;
}

PTable TTable::Union(const TTable& Table, const TStr& TableName){
  Schema NewSchema;
  THashSet<TInt> Collisions;

  for(TInt c = 0; c < S.Len(); c++){
    if (S[c].Val1 != GetIdColName()) {
      NewSchema.Add(TPair<TStr,TAttrType>(S[c].Val1, S[c].Val2));
    }
  }
  PTable result = TTable::New(TableName, NewSchema, Context);

  GetCollidingRows(Table, Collisions);

  result->AddTable(*this);

  // this part should be made faster by adding all the rows in one go
  for(TRowIterator it = Table.BegRI(); it < Table.EndRI(); it++){
    if (!Collisions.IsKey(it.GetRowIdx())){
      result->AddRow(it);
    }
  }
  result->InitIds();
  return result;
}

PTable TTable::Intersection(const TTable& Table, const TStr& TableName){
  Schema NewSchema;
  THashSet<TInt> Collisions;

  for(TInt c = 0; c < S.Len(); c++){
    if (S[c].Val1 != GetIdColName()) {
      NewSchema.Add(TPair<TStr,TAttrType>(S[c].Val1, S[c].Val2));
    }
  }
  PTable result = TTable::New(TableName, NewSchema, Context);

  GetCollidingRows(Table, Collisions);

  // this part should be made faster by adding all the rows in one go
  for(TRowIterator it = Table.BegRI(); it < Table.EndRI(); it++){
    if (Collisions.IsKey(it.GetRowIdx())){
      result->AddRow(it);
    }
  }
  result->InitIds();
  return result;
}

// TTable cannot be const because we will eventually call Table->GroupAux
// as of now, GroupAux cannot be const because it modifies the table in some cases
PTable TTable::Minus(TTable& Table, const TStr& TableName){
  Schema NewSchema;
  THashSet<TInt> Collisions;

  for(TInt c = 0; c < S.Len(); c++){
    if (S[c].Val1 != GetIdColName()) {
      NewSchema.Add(TPair<TStr,TAttrType>(S[c].Val1, S[c].Val2));
    }
  }
  PTable result = TTable::New(TableName, NewSchema, Context);

  Table.GetCollidingRows(*this, Collisions);

  // this part should be made faster by adding all the rows in one go
  for(TRowIterator it = BegRI(); it < EndRI(); it++){
    if (!Collisions.IsKey(it.GetRowIdx())){
      result->AddRow(it);
    }
  }
  result->InitIds();
  return result;
}

PTable TTable::Project(const TStrV& ProjectCols, const TStr& TableName){
  Schema NewSchema;
  for(TInt c = 0; c < ProjectCols.Len(); c++){
    if(!ColTypeMap.IsKey(ProjectCols[c])){TExcept::Throw("no such column " + ProjectCols[c]);}
    NewSchema.Add(TPair<TStr,TAttrType>(ProjectCols[c], GetColType(ProjectCols[c])));
  }

  PTable result = TTable::New(TableName, NewSchema, Context);
  result->AddTable(*this);
  result->InitIds();
  return result;
}

TBool TTable::IsAttr(const TStr& Attr) {
  if(!ColTypeMap.IsKey(Attr)){
    return false;
  }
  return true;
}

void TTable::AddIntCol(const TStr& ColName) {
  AddSchemaCol(ColName, atInt);
  IntCols.Add(TIntV(NumRows));
  TInt L = IntCols.Len();
  ColTypeMap.AddDat(ColName, TPair<TAttrType,TInt>(atInt, L-1));
}

void TTable::AddFltCol(const TStr& ColName) {
  AddSchemaCol(ColName, atFlt);
  FltCols.Add(TFltV(NumRows));
  TInt L = FltCols.Len();
  ColTypeMap.AddDat(ColName, TPair<TAttrType,TInt>(atFlt, L-1));
}

void TTable::ClassifyAux(const TIntV& SelectedRows, const TStr& LabelName, const TInt& PositiveLabel, const TInt& NegativeLabel){
  S.Add(TPair<TStr,TAttrType>(LabelName, atInt));
  TInt LabelColIdx = IntCols.Len();
  ColTypeMap.AddDat(LabelName, TPair<TAttrType, TInt>(atInt, LabelColIdx));
  IntCols.Add(TIntV(NumRows));
  for(TInt i = 0; i < NumRows; i++){
    IntCols[LabelColIdx][i] = NegativeLabel;
  }
  for(TInt i = 0; i < SelectedRows.Len(); i++){
    IntCols[LabelColIdx][SelectedRows[i]] = PositiveLabel;
  }
}

/* Performs a generic operations on two numeric attributes
 * Operation can be +, -, *, / or %
 * Alternative is to write separate functions for each operation
 * Branch prediction may result in as fast performance anyway ?
 *
 */
void TTable::ColGenericOp(const TStr& Attr1, const TStr& Attr2, const TStr& ResAttr, OPS op) {
  // check if attributes are valid
  if (!IsAttr(Attr1)) TExcept::Throw("No attribute present: " + Attr1);
  if (!IsAttr(Attr2)) TExcept::Throw("No attribute present: " + Attr2);

  TPair<TAttrType, TInt> Info1 = ColTypeMap.GetDat(Attr1);
  TPair<TAttrType, TInt> Info2 = ColTypeMap.GetDat(Attr2);

  if (Info1.Val1 == atStr || Info2.Val1 == atStr) {
    TExcept::Throw("Only numeric operations supported on columns");
  }

  // source column indices
  TInt ColIdx1 = Info1.Val2;
  TInt ColIdx2 = Info2.Val2;

  // destination column index
  TInt ColIdx3 = ColIdx1;

  // Create empty result column with type that of first attribute
  if (ResAttr != "") {
      if (Info1.Val1 == atInt) {
          AddIntCol(ResAttr);
      }
      else {
          AddFltCol(ResAttr);
      }
      ColIdx3 = GetColIdx(ResAttr);
  }

  for(TRowIterator RowI = BegRI(); RowI < EndRI(); RowI++){
    if (Info1.Val1 == atInt) {
      TInt Val, CurVal;
      CurVal = RowI.GetIntAttr(ColIdx1);
      if (Info2.Val1 == atInt) {
        Val = RowI.GetIntAttr(ColIdx2);
      }
      else {
        Val = RowI.GetFltAttr(ColIdx2);
      }
      if (op == OP_ADD) IntCols[ColIdx3][RowI.GetRowIdx()] = CurVal + Val;
      if (op == OP_SUB) IntCols[ColIdx3][RowI.GetRowIdx()] = CurVal - Val;
      if (op == OP_MUL) IntCols[ColIdx3][RowI.GetRowIdx()] = CurVal * Val;
      if (op == OP_DIV) IntCols[ColIdx3][RowI.GetRowIdx()] = CurVal / Val;
      if (op == OP_MOD) IntCols[ColIdx3][RowI.GetRowIdx()] = CurVal % Val;
    }
    else {
      TFlt Val, CurVal;
      CurVal = RowI.GetFltAttr(ColIdx1);
      if (Info2.Val1 == atInt) {
        Val = RowI.GetIntAttr(ColIdx2);
      }
      else {
        Val = RowI.GetFltAttr(ColIdx2);
      }
      if (op == OP_ADD) FltCols[ColIdx3][RowI.GetRowIdx()] = CurVal + Val;
      if (op == OP_SUB) FltCols[ColIdx3][RowI.GetRowIdx()] = CurVal - Val;
      if (op == OP_MUL) FltCols[ColIdx3][RowI.GetRowIdx()] = CurVal * Val;
      if (op == OP_DIV) FltCols[ColIdx3][RowI.GetRowIdx()] = CurVal / Val;
      if (op == OP_MOD) TExcept::Throw("Cannot find modulo for float columns");
    }
  }
}

void TTable::ColAdd(const TStr& Attr1, const TStr& Attr2, const TStr& ResultAttrName) {
  ColGenericOp(Attr1, Attr2, ResultAttrName, OP_ADD);
}

void TTable::ColSub(const TStr& Attr1, const TStr& Attr2, const TStr& ResultAttrName) {
  ColGenericOp(Attr1, Attr2, ResultAttrName, OP_SUB);
}

void TTable::ColMul(const TStr& Attr1, const TStr& Attr2, const TStr& ResultAttrName) {
  ColGenericOp(Attr1, Attr2, ResultAttrName, OP_MUL);
}

void TTable::ColDiv(const TStr& Attr1, const TStr& Attr2, const TStr& ResultAttrName) {
  ColGenericOp(Attr1, Attr2, ResultAttrName, OP_DIV);
}

void TTable::ColMod(const TStr& Attr1, const TStr& Attr2, const TStr& ResultAttrName) {
  ColGenericOp(Attr1, Attr2, ResultAttrName, OP_MOD);
}

void TTable::ColGenericOp(const TStr& Attr1, TTable& Table, const TStr& Attr2, const TStr& ResAttr,
  OPS op, TBool AddToFirstTable) {
  // check if attributes are valid
  if (!IsAttr(Attr1)) TExcept::Throw("No attribute present: " + Attr1);
  if (!Table.IsAttr(Attr2)) TExcept::Throw("No attribute present: " + Attr2);

  if (NumValidRows != Table.NumValidRows) TExcept::Throw("Tables do not have equal number of rows");

  TPair<TAttrType, TInt> Info1 = ColTypeMap.GetDat(Attr1);
  TPair<TAttrType, TInt> Info2 = Table.ColTypeMap.GetDat(Attr2);

  if (Info1.Val1 == atStr || Info2.Val1 == atStr) {
    TExcept::Throw("Only numeric operations supported on columns");
  }

  // source column indices
  TInt ColIdx1 = Info1.Val2;
  TInt ColIdx2 = Info2.Val2;

  // destination column index
  TInt ColIdx3 = ColIdx1;

  if (!AddToFirstTable)
    ColIdx3 = ColIdx2;

  // Create empty result column in appropriate table with type that of first attribute
  if (ResAttr != "") {
    if (AddToFirstTable) {
      if (Info1.Val1 == atInt) {
          AddIntCol(ResAttr);
      }
      else {
          AddFltCol(ResAttr);
      }
      ColIdx3 = GetColIdx(ResAttr);
    }
    else {
      if (Info1.Val1 == atInt) {
          Table.AddIntCol(ResAttr);
      }
      else {
          Table.AddFltCol(ResAttr);
      }
      ColIdx3 = Table.GetColIdx(ResAttr);
    }
  }

  TRowIterator RI1, RI2;

  RI1 = BegRI();
  RI2 = Table.BegRI();
  
  while (RI1 < EndRI() && RI2 < Table.EndRI()) {
    if (Info1.Val1 == atInt) {
      TInt Val, CurVal;
      CurVal = RI1.GetIntAttr(ColIdx1);
      if (Info2.Val1 == atInt) {
        Val = RI2.GetIntAttr(ColIdx2);
      }
      else {
        Val = RI2.GetFltAttr(ColIdx2);
      }
      if (AddToFirstTable) {
        if (op == OP_ADD) IntCols[ColIdx3][RI1.GetRowIdx()] = CurVal + Val;
        if (op == OP_SUB) IntCols[ColIdx3][RI1.GetRowIdx()] = CurVal - Val;
        if (op == OP_MUL) IntCols[ColIdx3][RI1.GetRowIdx()] = CurVal * Val;
        if (op == OP_DIV) IntCols[ColIdx3][RI1.GetRowIdx()] = CurVal / Val;
        if (op == OP_MOD) IntCols[ColIdx3][RI1.GetRowIdx()] = CurVal % Val;
      }
      else {
        if (op == OP_ADD) Table.IntCols[ColIdx3][RI2.GetRowIdx()] = CurVal + Val;
        if (op == OP_SUB) Table.IntCols[ColIdx3][RI2.GetRowIdx()] = CurVal - Val;
        if (op == OP_MUL) Table.IntCols[ColIdx3][RI2.GetRowIdx()] = CurVal * Val;
        if (op == OP_DIV) Table.IntCols[ColIdx3][RI2.GetRowIdx()] = CurVal / Val;
        if (op == OP_MOD) Table.IntCols[ColIdx3][RI2.GetRowIdx()] = CurVal % Val;
      }
    }
    else {
      TFlt Val, CurVal;
      CurVal = RI1.GetFltAttr(ColIdx1);
      if (Info2.Val1 == atInt) {
        Val = RI2.GetIntAttr(ColIdx2);
      }
      else {
        Val = RI2.GetFltAttr(ColIdx2);
      }
      if (AddToFirstTable) {
        if (op == OP_ADD) FltCols[ColIdx3][RI1.GetRowIdx()] = CurVal + Val;
        if (op == OP_SUB) FltCols[ColIdx3][RI1.GetRowIdx()] = CurVal - Val;
        if (op == OP_MUL) FltCols[ColIdx3][RI1.GetRowIdx()] = CurVal * Val;
        if (op == OP_DIV) FltCols[ColIdx3][RI1.GetRowIdx()] = CurVal / Val;
        if (op == OP_MOD) TExcept::Throw("Cannot find modulo for float columns");
      }
      else {
        if (op == OP_ADD) Table.FltCols[ColIdx3][RI2.GetRowIdx()] = CurVal + Val;
        if (op == OP_SUB) Table.FltCols[ColIdx3][RI2.GetRowIdx()] = CurVal - Val;
        if (op == OP_MUL) Table.FltCols[ColIdx3][RI2.GetRowIdx()] = CurVal * Val;
        if (op == OP_DIV) Table.FltCols[ColIdx3][RI2.GetRowIdx()] = CurVal / Val;
        if (op == OP_MOD) TExcept::Throw("Cannot find modulo for float columns");
      }
    }
    RI1++;
    RI2++;
  }

  if (RI1 != EndRI() || RI2 != Table.EndRI()) TExcept::Throw("ColGenericOp: Iteration error");
}

void TTable::ColAdd(const TStr& Attr1, TTable& Table, const TStr& Attr2, 
  const TStr& ResultAttrName, TBool AddToFirstTable) {
  ColGenericOp(Attr1, Table, Attr2, ResultAttrName, OP_ADD, AddToFirstTable);
}

void TTable::ColSub(const TStr& Attr1, TTable& Table, const TStr& Attr2, 
  const TStr& ResultAttrName, TBool AddToFirstTable) {
  ColGenericOp(Attr1, Table, Attr2, ResultAttrName, OP_SUB, AddToFirstTable);
}

void TTable::ColMul(const TStr& Attr1, TTable& Table, const TStr& Attr2, 
  const TStr& ResultAttrName, TBool AddToFirstTable) {
  ColGenericOp(Attr1, Table, Attr2, ResultAttrName, OP_MUL, AddToFirstTable);
}

void TTable::ColDiv(const TStr& Attr1, TTable& Table, const TStr& Attr2, 
  const TStr& ResultAttrName, TBool AddToFirstTable) {
  ColGenericOp(Attr1, Table, Attr2, ResultAttrName, OP_DIV, AddToFirstTable);
}

void TTable::ColMod(const TStr& Attr1, TTable& Table, const TStr& Attr2, 
  const TStr& ResultAttrName, TBool AddToFirstTable) {
  ColGenericOp(Attr1, Table, Attr2, ResultAttrName, OP_MOD, AddToFirstTable);
}


void TTable::ColGenericOp(const TStr& Attr1, const TFlt& Num, const TStr& ResAttr, OPS op, const TBool floatCast) {
  // check if attribute is valid
  if (!IsAttr(Attr1)) TExcept::Throw("No attribute present: " + Attr1);

  TPair<TAttrType, TInt> Info1 = ColTypeMap.GetDat(Attr1);

  if (Info1.Val1 == atStr) {
    TExcept::Throw("Only numeric operations supported on columns");
  }

  // source column index
  TInt ColIdx1 = Info1.Val2;

  // destination column index
  TInt ColIdx2 = ColIdx1;

  // Create empty result column with type that of first attribute
  TBool shouldCast = floatCast;
  if (ResAttr != "") {
      if ((Info1.Val1 == atInt) & !shouldCast) {
          AddIntCol(ResAttr);
      }
      else {
          AddFltCol(ResAttr);
      }
      ColIdx2 = GetColIdx(ResAttr);
  } else {
    // Cannot change type of existing attribute
    shouldCast = false;
  }

  for(TRowIterator RowI = BegRI(); RowI < EndRI(); RowI++){
    if ((Info1.Val1 == atInt) && !shouldCast) {
      TInt CurVal = RowI.GetIntAttr(ColIdx1);
      TInt Val = (TInt) Num;
      if (op == OP_ADD) IntCols[ColIdx2][RowI.GetRowIdx()] = CurVal + Val;
      if (op == OP_SUB) IntCols[ColIdx2][RowI.GetRowIdx()] = CurVal - Val;
      if (op == OP_MUL) IntCols[ColIdx2][RowI.GetRowIdx()] = CurVal * Val;
      if (op == OP_DIV) IntCols[ColIdx2][RowI.GetRowIdx()] = CurVal / Val;
      if (op == OP_MOD) IntCols[ColIdx2][RowI.GetRowIdx()] = CurVal % Val;
    }
    else {
      TFlt CurVal = Info1.Val1 == atFlt ? RowI.GetFltAttr(ColIdx1) : (TFlt) RowI.GetIntAttr(ColIdx1);
      if (op == OP_ADD) FltCols[ColIdx2][RowI.GetRowIdx()] = CurVal + Num;
      if (op == OP_SUB) FltCols[ColIdx2][RowI.GetRowIdx()] = CurVal - Num;
      if (op == OP_MUL) FltCols[ColIdx2][RowI.GetRowIdx()] = CurVal * Num;
      if (op == OP_DIV) FltCols[ColIdx2][RowI.GetRowIdx()] = CurVal / Num;
      if (op == OP_MOD) TExcept::Throw("Cannot find modulo for float columns");
    }
  }
}

void TTable::ColAdd(const TStr& Attr1, const TFlt& Num, const TStr& ResultAttrName, const TBool floatCast) {
  ColGenericOp(Attr1, Num, ResultAttrName, OP_ADD, floatCast);
}

void TTable::ColSub(const TStr& Attr1, const TFlt& Num, const TStr& ResultAttrName, const TBool floatCast) {
  ColGenericOp(Attr1, Num, ResultAttrName, OP_SUB, floatCast);
}

void TTable::ColMul(const TStr& Attr1, const TFlt& Num, const TStr& ResultAttrName, const TBool floatCast) {
  ColGenericOp(Attr1, Num, ResultAttrName, OP_MUL, floatCast);
}

void TTable::ColDiv(const TStr& Attr1, const TFlt& Num, const TStr& ResultAttrName, const TBool floatCast) {
  ColGenericOp(Attr1, Num, ResultAttrName, OP_DIV, floatCast);
}

void TTable::ColMod(const TStr& Attr1, const TFlt& Num, const TStr& ResultAttrName, const TBool floatCast) {
  ColGenericOp(Attr1, Num, ResultAttrName, OP_MOD, floatCast);
}

void TTable::ProjectInPlace(const TStrV& ProjectCols){
  for(TInt c = 0; c < ProjectCols.Len(); c++){
    if(!ColTypeMap.IsKey(ProjectCols[c])){TExcept::Throw("no such column " + ProjectCols[c]);}
  }
  THashSet<TStr> ProjectColsSet = THashSet<TStr>(ProjectCols);
  // Delete the column vectors
  for(TInt i = S.Len() - 1; i >= 0; i--){
    TStr ColName = GetSchemaColName(i);
    if (ProjectColsSet.IsKey(ColName)){ continue;}
    TAttrType ColType = GetSchemaColType(i);
    TInt ColId = GetColIdx(ColName);
    switch(ColType){
      case atInt:
        IntCols.Del(ColId);
        break;
      case atFlt:
        FltCols.Del(ColId);
        break;
      case atStr:
        StrColMaps.Del(ColId);
        break;
    }
  }

  // Rebuild the ColTypeMap with new indexes of the column vectors
  TInt IntColCnt = 0;
  TInt FltColCnt = 0;
  TInt StrColCnt = 0;
  ColTypeMap.Clr();
  for(TInt i = 0; i < S.Len(); i++){
    TStr ColName = GetSchemaColName(i);
    if (!ProjectColsSet.IsKey(ColName)){ continue;}
    TAttrType ColType = GetSchemaColType(i);
    switch(ColType){
      case atInt:
        ColTypeMap.AddDat(ColName, TPair<TAttrType,TInt>(atInt, IntColCnt));
        IntColCnt++;
        break;
      case atFlt:
        ColTypeMap.AddDat(ColName, TPair<TAttrType,TInt>(atFlt, FltColCnt));
        FltColCnt++;
        break;
      case atStr:
        ColTypeMap.AddDat(ColName, TPair<TAttrType,TInt>(atStr, StrColCnt));
        StrColCnt++;
        break;
    }
  }

  // Update schema
  for (TInt i = S.Len() - 1; i >= 0; i--){
    TStr ColName = GetSchemaColName(i);
    if (ProjectColsSet.IsKey(ColName)){ continue;}
    S.Del(i);
  }
  InitIds();
}
