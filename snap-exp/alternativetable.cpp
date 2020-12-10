/*
Alternative implementations (with very limited functionality) for TTable.
These are for benchmarking purposes only. Add this file name to snap-core/Snap.cpp if used.
*/

TInt const RowBasedTable::Last = -1;
TInt const RowBasedTable::Invalid = -2;

RowBasedRowIterator& RowBasedRowIterator::operator++(int) {
  return this->Next();
}

RowBasedRowIterator& RowBasedRowIterator::Next() {
  CurrRowIdx = Table->Next[CurrRowIdx];
  return *this;
}

bool RowBasedRowIterator::operator < (const RowBasedRowIterator& RowI) const{ 
  if (CurrRowIdx == RowBasedTable::Last) { return false; }
  if (RowI.CurrRowIdx == RowBasedTable::Last) { return true; }
  return CurrRowIdx < RowI.CurrRowIdx;
}

bool RowBasedRowIterator::operator == (const RowBasedRowIterator& RowI) const {
  return CurrRowIdx == RowI.CurrRowIdx;
}

TInt RowBasedRowIterator::GetRowIdx() const {
  return CurrRowIdx;
}

TInt RowBasedRowIterator::GetIntAttr(TInt ColIdx) const {
  return Table->Rows[CurrRowIdx].GetIntVal(ColIdx);
}

RowBasedTable::RowBasedTable(const Schema& TableSchema):
  NumRows(0), NumValidRows(0), FirstValidRow(0), LastValidRow(-1), Sch(TableSchema){}
  
// Load table. Sequential and only supporting int attributes
PRowBasedTable RowBasedTable::LoadSS(const Schema& S, const TStr& InFNm, const char& Separator, TBool HasTitleLine) {
  Schema SR = S;
  PRowBasedTable T = New(SR);
  TSsParser Ss(InFNm, Separator);
  // if title line (i.e. names of the columns) is included as first row in the
  // input file - use it to validate schema
  if (HasTitleLine) {
    Ss.Next();  
    if (S.Len() != Ss.GetFlds()) {
      printf("%s\n", Ss[0]); TExcept::Throw("Table Schema Mismatch!");
    }
    for (TInt i = 0; i < Ss.GetFlds(); i++) {
      // remove carriage return char
      TInt L = strlen(Ss[i]);
      if (Ss[i][L-1] < ' ') { Ss[i][L-1] = 0; }
    }
  }
  // populate table columns
  uint64_t Cnt = 0;
  while (Ss.Next()) {
    if (Ss.GetFlds() != SR.Len()) {
      printf("%s\n", Ss[SR.Len()]); TExcept::Throw("Error reading tsv file");
    }
    T->Rows.Add(RowBasedTableRow());
    for (TInt i = 0; i < SR.Len(); i++) {
    	T->Rows[Cnt].AddInt(Ss.GetInt(i));
    }
    Cnt += 1;
  }
  // set number of rows and "Next" vector
  T->NumRows = Cnt;
  T->NumValidRows = T->NumRows;
  T->Next.Clr();
  T->Next.Gen(Cnt);
  for (uint64_t i = 0; i < Cnt-1; i++) {
    T->Next[i] = i+1;
  }
  T->Next[Cnt-1] = Last;
  T->LastValidRow = T->NumRows - 1;
  return T;
}

void RowBasedTable::SaveSS(const TStr& OutFNm){
  if (NumValidRows == 0) {
    printf("Table is empty");
    return;
  }
  FILE* F = fopen(OutFNm.CStr(), "w");
  if (F == NULL) {
    printf("failed to open file %s\n", OutFNm.CStr());
    perror("fail ");
    return;
  }
  TInt L = Sch.Len();
  // print title (schema)
  fprintf(F, "# ");
  for (TInt i = 0; i < L-1; i++) {
    fprintf(F, "%s\t", Sch[i].Val1.CStr());
  }  
  fprintf(F, "%s\n", Sch[L-1].Val1.CStr());
  // print table contents
  for (RowBasedRowIterator RowI = BegRI(); RowI < EndRI(); RowI++) {
    for (TInt i = 0; i < L; i++) {
      char C = (i == L-1) ? '\n' : '\t';
      fprintf(F, "%d%c", RowI.GetIntAttr(i).Val, C);
    }
  }
  fclose(F);
}

/*******************************************************************************/

TInt const ExplicitStringTable::Last = -1;
TInt const ExplicitStringTable::Invalid = -2;

ExplicitStringRowIterator& ExplicitStringRowIterator::operator++(int) {
  return this->Next();
}

ExplicitStringRowIterator& ExplicitStringRowIterator::Next() {
  CurrRowIdx = Table->Next[CurrRowIdx];
  return *this;
}

bool ExplicitStringRowIterator::operator < (const ExplicitStringRowIterator& RowI) const{ 
  if (CurrRowIdx == ExplicitStringTable::Last) { return false; }
  if (RowI.CurrRowIdx == ExplicitStringTable::Last) { return true; }
  return CurrRowIdx < RowI.CurrRowIdx;
}

bool ExplicitStringRowIterator::operator == (const ExplicitStringRowIterator& RowI) const {
  return CurrRowIdx == RowI.CurrRowIdx;
}

TInt ExplicitStringRowIterator::GetRowIdx() const {
  return CurrRowIdx;
}

TInt ExplicitStringRowIterator::GetIntAttr(TInt ColIdx) const{
	return Table->IntCols[ColIdx][CurrRowIdx];
}

TFlt ExplicitStringRowIterator::GetFltAttr(TInt ColIdx) const{
	return Table->FltCols[ColIdx][CurrRowIdx];
}

TStr ExplicitStringRowIterator::GetStrAttr(TInt ColIdx) const{
	return Table->StrCols[ColIdx][CurrRowIdx];
}

TInt ExplicitStringRowIterator::GetIntAttr(const TStr& Col) const {
  TInt ColIdx = Table->GetColIdx(Col);
  return Table->IntCols[ColIdx][CurrRowIdx];
}

TFlt ExplicitStringRowIterator::GetFltAttr(const TStr& Col) const {
  TInt ColIdx = Table->GetColIdx(Col);
  return Table->FltCols[ColIdx][CurrRowIdx];
}

TStr ExplicitStringRowIterator::GetStrAttr(const TStr& Col) const {
  TInt ColIdx = Table->GetColIdx(Col);
  return Table->StrCols[ColIdx][CurrRowIdx];
}

TBool ExplicitStringRowIterator::CompareAtomicConst(TInt ColIdx, const TPrimitive& Val, TPredComp Cmp) {
  TBool Result;
  switch (Val.GetType()) {
    case atInt:
      Result = TPredicate::EvalAtom(GetIntAttr(ColIdx), Val.GetInt(), Cmp);
      break;
    case atFlt:
      Result = TPredicate::EvalAtom(GetFltAttr(ColIdx), Val.GetFlt(), Cmp);
      break;
    case atStr:
      Result = TPredicate::EvalStrAtom(GetStrAttr(ColIdx), Val.GetStr(), Cmp);
      break;
    default:
      Result = TBool(false);
  }
  return Result;
}

TBool ExplicitStringRowIterator::CompareAtomicConstTStr(TInt ColIdx, const TStr& Val, TPredComp Cmp) {
  TBool Result;
  Result = TPredicate::EvalStrAtom(GetStrAttr(ColIdx), Val, Cmp);
  return Result;
}

ExplicitStringRowIteratorWithRemove::ExplicitStringRowIteratorWithRemove(TInt RowIdx, ExplicitStringTable* TablePtr) :
  CurrRowIdx(RowIdx), Table(TablePtr), Start(RowIdx == TablePtr->FirstValidRow) {}
  
ExplicitStringRowIteratorWithRemove& ExplicitStringRowIteratorWithRemove::operator++(int) {
  return this->Next();
}

ExplicitStringRowIteratorWithRemove& ExplicitStringRowIteratorWithRemove::Next() {
  CurrRowIdx = GetNextRowIdx();
  Start = false;
  Assert(CurrRowIdx != ExplicitStringTable::Invalid);
  return *this;
}

bool ExplicitStringRowIteratorWithRemove::operator < (const ExplicitStringRowIteratorWithRemove& RowI) const { 
  if (CurrRowIdx == ExplicitStringTable::Last) { return false; }
  if (RowI.CurrRowIdx == ExplicitStringTable::Last) { return true; }
  return CurrRowIdx < RowI.CurrRowIdx;
}

bool ExplicitStringRowIteratorWithRemove::operator == (const ExplicitStringRowIteratorWithRemove& RowI) const {
  return CurrRowIdx == RowI.CurrRowIdx;
}

TInt ExplicitStringRowIteratorWithRemove::GetRowIdx() const {
  return CurrRowIdx;
}

TInt ExplicitStringRowIteratorWithRemove::GetNextRowIdx() const {
  return (Start ? Table->FirstValidRow : Table->Next[CurrRowIdx]);
}

TInt ExplicitStringRowIteratorWithRemove::GetNextIntAttr(TInt ColIdx) const {
  return Table->IntCols[ColIdx][GetNextRowIdx()];
}

TFlt ExplicitStringRowIteratorWithRemove::GetNextFltAttr(TInt ColIdx) const {
  return Table->FltCols[ColIdx][GetNextRowIdx()];
}

TStr ExplicitStringRowIteratorWithRemove::GetNextStrAttr(TInt ColIdx) const {
  return Table->StrCols[ColIdx][GetNextRowIdx()];
}

TBool ExplicitStringRowIteratorWithRemove::IsFirst() const {
  return CurrRowIdx == Table->FirstValidRow;
}

void ExplicitStringRowIteratorWithRemove::RemoveNext() {
  Table->RemoveRow(GetNextRowIdx(), CurrRowIdx);
}

TBool ExplicitStringRowIteratorWithRemove::CompareAtomicConst(TInt ColIdx, const TPrimitive& Val, TPredComp Cmp) {
  TBool Result;
  switch (Val.GetType()) {
    case atInt:
      Result = TPredicate::EvalAtom(GetNextIntAttr(ColIdx), Val.GetInt(), Cmp);
      break;
    case atFlt:
      Result = TPredicate::EvalAtom(GetNextFltAttr(ColIdx), Val.GetFlt(), Cmp);
      break;
    case atStr:
      Result = TPredicate::EvalStrAtom(GetNextStrAttr(ColIdx), Val.GetStr(), Cmp);
      break;
    default:
      Result = TBool(false);
  }
  return Result;
}

ExplicitStringTable::ExplicitStringTable():
  NumRows(0), NumValidRows(0), FirstValidRow(0), LastValidRow(-1) {}

ExplicitStringTable::ExplicitStringTable(const Schema& TableSchema):
  NumRows(0), NumValidRows(0), FirstValidRow(0), LastValidRow(-1) {
  TInt IntColCnt = 0;
  TInt FltColCnt = 0;
  TInt StrColCnt = 0;
  for (TInt i = 0; i < TableSchema.Len(); i++) {
    TStr ColName = TableSchema[i].Val1;
    TAttrType ColType = TableSchema[i].Val2;
    AddSchemaCol(ColName, ColType);
    switch (ColType) {
      case atInt:
        AddColType(ColName, atInt, IntColCnt);
        IntColCnt++;
        break;
      case atFlt:
        AddColType(ColName, atFlt, FltColCnt);
        FltColCnt++;
        break;
      case atStr:
        AddColType(ColName, atStr, StrColCnt);
        StrColCnt++;
        break;
    }
  }
  IntCols = TVec<TIntV>(IntColCnt);
  FltCols = TVec<TFltV>(FltColCnt);
  StrCols = TVec<TStrV>(StrColCnt);
}

PExplicitStringTable ExplicitStringTable::LoadSS(const Schema& S, const TStr& InFNm, const char& Separator, TBool HasTitleLine) {
  Schema SR = S;
  PExplicitStringTable T = New(SR);
  // find col types and check for string cols
  TInt RowLen = SR.Len();
  TVec<TAttrType> ColTypes = TVec<TAttrType>(RowLen);
  for (TInt i = 0; i < RowLen; i++) {
    ColTypes[i] = T->GetSchemaColType(i);
  }
  TSsParser Ss(InFNm, Separator);
  // if title line (i.e. names of the columns) is included as first row in the
  // input file - use it to validate schema
  if (HasTitleLine) {
    Ss.Next();  
    if (S.Len() != Ss.GetFlds()) {
      printf("%s\n", Ss[0]); TExcept::Throw("Table Schema Mismatch!");
    }
    for (TInt i = 0; i < Ss.GetFlds(); i++) {
      // remove carriage return char
      TInt L = strlen(Ss[i]);
      if (Ss[i][L-1] < ' ') { Ss[i][L-1] = 0; }
    }
  }
  // populate table columns
  uint64_t Cnt = 0;
  while (Ss.Next()) {
    TInt IntColIdx = 0;
    TInt FltColIdx = 0;
    TInt StrColIdx = 0;
    if (Ss.GetFlds() != S.Len()) {
      printf("%s\n", Ss[S.Len()]); TExcept::Throw("Error reading tsv file");
    }
    for (TInt i = 0; i < RowLen; i++) {
      switch (ColTypes[i]) {
        case atInt:
          T->IntCols[IntColIdx].Add(Ss.GetInt(i));
          IntColIdx++;
          break;
        case atFlt:
          T->FltCols[FltColIdx].Add(Ss.GetFlt(i));
          FltColIdx++;
          break;
        case atStr:
          T->StrCols[StrColIdx].Add(Ss[i]);
          StrColIdx++;
          break;
      }
    }
    Cnt += 1;
  }
  // set number of rows and "Next" vector
  T->NumRows = Cnt;
  T->NumValidRows = T->NumRows;
  T->Next.Clr();
  T->Next.Gen(Cnt);
  for (uint64_t i = 0; i < Cnt-1; i++) {
    T->Next[i] = i+1;
  }
  T->Next[Cnt-1] = Last;
  T->LastValidRow = T->NumRows - 1;
  T->InitIds();
  return T;
}

void ExplicitStringTable::SaveSS(const TStr& OutFNm) {
  if (NumValidRows == 0) {
    printf("Table is empty");
    return;
  }
  FILE* F = fopen(OutFNm.CStr(), "w");
  if (F == NULL) {
    printf("failed to open file %s\n", OutFNm.CStr());
    perror("fail ");
    return;
  }
  TInt L = Sch.Len();
  // print title (schema)
  fprintf(F, "# ");
  for (TInt i = 0; i < L-1; i++) {
    fprintf(F, "%s\t", Sch[i].Val1.CStr());
  }  
  fprintf(F, "%s\n", Sch[L-1].Val1.CStr());
  // print table contents
  for (ExplicitStringRowIterator RowI = BegRI(); RowI < EndRI(); RowI++) {
    for (TInt i = 0; i < L; i++) {
      char C = (i == L-1) ? '\n' : '\t';
      switch (GetSchemaColType(i)) {
        case atInt: {
          fprintf(F, "%d%c", RowI.GetIntAttr(GetSchemaColName(i)).Val, C);
          break;
        }
        case atFlt: {
          fprintf(F, "%f%c", RowI.GetFltAttr(GetSchemaColName(i)).Val, C);
          break;
        }
        case atStr: {
          fprintf(F, "%s%c", RowI.GetStrAttr(GetSchemaColName(i)).CStr(), C);
          break;
        }
      }
    }
  }
  fclose(F);
}

void ExplicitStringTable::SelectAtomicConst(const TStr& Col, const TPrimitive& Val, TPredComp Cmp, 
  	TIntV& SelectedRows, PExplicitStringTable& SelectedTable, TBool Remove, TBool Table) {
  TAttrType Type = GetColType(Col);
  TInt ColIdx = GetColIdx(Col);
  if (Type != Val.GetType()) { 
    TExcept::Throw("SelectAtomicConst: coltype does not match const type"); 
  }
  if(Remove){
      ExplicitStringRowIteratorWithRemove RowI = BegRIWR();
      while(RowI.GetNextRowIdx() != Last){
        if (!RowI.CompareAtomicConst(ColIdx, Val, Cmp)) {
          RowI.RemoveNext();
        } else {
          RowI++;
        }
      }
  } else if (Table) {
      for(ExplicitStringRowIterator RowI = BegRI(); RowI < EndRI(); RowI++){
        if (RowI.CompareAtomicConst(ColIdx, Val, Cmp)) { 
          SelectedTable->AddRow(RowI);
        }
      }
  } else {
    for(ExplicitStringRowIterator RowI = BegRI(); RowI < EndRI(); RowI++){
      if (RowI.CompareAtomicConst(ColIdx, Val, Cmp)) { 
        SelectedRows.Add(RowI.GetRowIdx());
      }
    }
  }
}

void ExplicitStringTable::AddRow(const ExplicitStringRowIterator& RI) {
  for (TInt c = 0; c < Sch.Len(); c++) {
    TStr ColName = GetSchemaColName(c);
    if (ColName == IdColName) { continue; }
    TInt ColIdx = GetColIdx(ColName);
    switch (GetColType(ColName)) {
    case atInt:
       IntCols[ColIdx].Add(RI.GetIntAttr(ColName));
       break;
    case atFlt:
       FltCols[ColIdx].Add(RI.GetFltAttr(ColName));
       break;
    case atStr:
       StrCols[ColIdx].Add(RI.GetStrAttr(ColName));
       break;
    }
  }
  UpdateTableForNewRow();
}

void ExplicitStringTable::UpdateTableForNewRow() {
  if (LastValidRow >= 0) {
    Next[LastValidRow] = NumRows;
  }
  Next.Add(Last);
  LastValidRow = NumRows;
  NumRows++;
  NumValidRows++;
}


PExplicitStringTable ExplicitStringTable::Join(const TStr& Col1, const ExplicitStringTable& Table, const TStr& Col2) {
  if (!IsColName(Col1)) {
    TExcept::Throw("no such column " + Col1);
  }
  if (!Table.IsColName(Col2)) {
    TExcept::Throw("no such column " + Col2);
  }
  if (GetColType(Col1) != Table.GetColType(Col2)) {
    TExcept::Throw("Trying to Join on columns of different type");
  }
  // initialize result table
  PExplicitStringTable JointTable = InitializeJointTable(Table);
  // hash smaller table (group by column)
  TAttrType ColType = GetColType(Col1);
  TBool ThisIsSmaller = (NumValidRows <= Table.NumValidRows);
  const ExplicitStringTable& TS = ThisIsSmaller ? *this : Table;
  const ExplicitStringTable& TB = ThisIsSmaller ?  Table : *this;
  TStr ColS = ThisIsSmaller ? Col1 : Col2;
  TStr ColB = ThisIsSmaller ? Col2 : Col1;
  TInt ColBId = ThisIsSmaller ? Table.GetColIdx(ColB) : GetColIdx(ColB);
    switch (ColType) {
      case atInt:{
        TIntIntVH T;
        TS.GroupByIntCol(ColS, T, TIntV(), true);
        for (ExplicitStringRowIterator RowI = TB.BegRI(); RowI < TB.EndRI(); RowI++) {
          TInt K = RowI.GetIntAttr(ColBId);
          if (T.IsKey(K)) {
            TIntV& Group = T.GetDat(K);
            for (TInt i = 0; i < Group.Len(); i++) {
              if (ThisIsSmaller) {
                JointTable->AddJointRow(*this, Table, Group[i], RowI.GetRowIdx());
              } else {
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
        for (ExplicitStringRowIterator RowI = TB.BegRI(); RowI < TB.EndRI(); RowI++) {
          TFlt K = RowI.GetFltAttr(ColBId);
          if (T.IsKey(K)) {
            TIntV& Group = T.GetDat(K);
            for (TInt i = 0; i < Group.Len(); i++) {
              if (ThisIsSmaller) {
                JointTable->AddJointRow(*this, Table, Group[i], RowI.GetRowIdx());
              } else {
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
        for (ExplicitStringRowIterator RowI = TB.BegRI(); RowI < TB.EndRI(); RowI++) {
          TStr K = RowI.GetStrAttr(ColBId);
          if (T.IsKey(K)) {
            TIntV& Group = T.GetDat(K);
            for (TInt i = 0; i < Group.Len(); i++) {
              if (ThisIsSmaller) {
                JointTable->AddJointRow(*this, Table, Group[i], RowI.GetRowIdx());
              } else {
                JointTable->AddJointRow(*this, Table, RowI.GetRowIdx(), Group[i]);
              }
            }
          }
        }
      }
      break;
    }
  return JointTable; 
}

PExplicitStringTable ExplicitStringTable::InitializeJointTable(const ExplicitStringTable& Table) {
  PExplicitStringTable JointTable = New();
  JointTable->IntCols = TVec<TIntV>(IntCols.Len() + Table.IntCols.Len() + 1);
  JointTable->FltCols = TVec<TFltV>(FltCols.Len() + Table.FltCols.Len());
  JointTable->StrCols = TVec<TStrV>(StrCols.Len() + Table.StrCols.Len());
  for (TInt i = 0; i < Sch.Len(); i++) {
    TStr ColName = GetSchemaColName(i);
    TAttrType ColType = GetSchemaColType(i);
    TStr CName = JointTable->RenumberColName(ColName);
    TPair<TAttrType, TInt> TypeMap = GetColTypeMap(ColName);
    JointTable->AddColType(CName, TypeMap);
    JointTable->AddSchemaCol(CName, ColType);
  }
  for (TInt i = 0; i < Table.Sch.Len(); i++) {
    TStr ColName = Table.GetSchemaColName(i);
    TAttrType ColType = Table.GetSchemaColType(i);
    TStr CName = JointTable->RenumberColName(ColName);
    TPair<TAttrType, TInt> NewDat = Table.GetColTypeMap(ColName);
    // add offsets
    switch (NewDat.Val1) {
      case atInt:
        NewDat.Val2 += IntCols.Len();
        break;
      case atFlt:
        NewDat.Val2 += FltCols.Len();
        break;
      case atStr:
        NewDat.Val2 += StrCols.Len();
        break;
    }
    JointTable->AddColType(CName, NewDat);
    JointTable->AddSchemaCol(CName, ColType);
  }
  TStr IdColName = "_id";
  JointTable->AddColType(IdColName, atInt, IntCols.Len() + Table.IntCols.Len());
  JointTable->AddSchemaCol(IdColName, atInt);
  return JointTable;
}

TStr ExplicitStringTable::RenumberColName(const TStr& ColName) const {
  TStr NColName = ColName;
  if (NColName.GetCh(NColName.Len()-2) == '-') { 
    NColName = NColName.GetSubStr(0,NColName.Len()-3); 
  }
  TInt Conflicts = 0;
  for (TInt i = 0; i < Sch.Len(); i++) {
    if (NColName == Sch[i].Val1.GetSubStr(0, Sch[i].Val1.Len()-3)) {
      Conflicts++;
    }
  }
  Conflicts++;
  NColName = NColName + "-" + Conflicts.GetStr();
  return NColName;
}

void ExplicitStringTable::GroupingSanityCheck(const TStr& GroupBy, const TAttrType& AttrType) const {
  if (!IsColName(GroupBy)) {
    TExcept::Throw("no such column " + GroupBy);
  }
  if (GetColType(GroupBy) != AttrType) {
    TExcept::Throw(GroupBy + " values are not of expected type");
  }
}

void ExplicitStringTable::AddJointRow(const ExplicitStringTable& T1, const ExplicitStringTable& T2, TInt RowIdx1, TInt RowIdx2) {
  for (TInt i = 0; i < T1.IntCols.Len(); i++) {
    IntCols[i].Add(T1.IntCols[i][RowIdx1]);
  }
  for (TInt i = 0; i < T1.FltCols.Len(); i++) {
    FltCols[i].Add(T1.FltCols[i][RowIdx1]);
  }
  for (TInt i = 0; i < T1.StrCols.Len(); i++) {
    StrCols[i].Add(T1.StrCols[i][RowIdx1]);
  }
  TInt IntOffset = T1.IntCols.Len();
  TInt FltOffset = T1.FltCols.Len();
  TInt StrOffset = T1.StrCols.Len();
  for (TInt i = 0; i < T2.IntCols.Len(); i++) {
    IntCols[i+IntOffset].Add(T2.IntCols[i][RowIdx2]);
  }
  for (TInt i = 0; i < T2.FltCols.Len(); i++) {
    FltCols[i+FltOffset].Add(T2.FltCols[i][RowIdx2]);
  }
  for (TInt i = 0; i < T2.StrCols.Len(); i++) {
    StrCols[i+StrOffset].Add(T2.StrCols[i][RowIdx2]);
  }
  TInt IdOffset = IntOffset + T2.IntCols.Len();
  NumRows++;
  NumValidRows++;
  if (!Next.Empty()) {
    Next[Next.Len()-1] = NumValidRows-1;
    LastValidRow = NumValidRows-1;
  }
  Next.Add(Last);
  RowIdMap.AddDat(NumRows-1,NumRows-1);
  IntCols[IdOffset].Add(NumRows-1);
}

void ExplicitStringTable::RemoveFirstRow() {
  if (FirstValidRow == LastValidRow) {
    LastValidRow = -1;
  }
  TInt Old = FirstValidRow;
  FirstValidRow = Next[FirstValidRow];
  Next[Old] = ExplicitStringTable::Invalid;
  NumValidRows--;
  TInt IdColIdx = GetColIdx(GetIdColName());
  RowIdMap.AddDat(IntCols[IdColIdx][Old], Invalid);
}

void ExplicitStringTable::RemoveRow(TInt RowIdx, TInt PrevRowIdx) {
  if (RowIdx == FirstValidRow) {
    RemoveFirstRow();
    return;
  }
  Assert(RowIdx != ExplicitStringTable::Invalid);
  if (RowIdx == ExplicitStringTable::Last) { return; }
  Next[PrevRowIdx] = Next[RowIdx];
  if (LastValidRow == RowIdx) {
    LastValidRow = RowIdx;
  }
  Next[RowIdx] = ExplicitStringTable::Invalid;
  NumValidRows--;
  TInt IdColIdx = GetColIdx(GetIdColName());
  RowIdMap.AddDat(IntCols[IdColIdx][RowIdx], Invalid);
}

void ExplicitStringTable::InitIds() {
  IdColName = "_id";
  AddIdColumn(IdColName);
}

void ExplicitStringTable::AddIdColumn(const TStr& ColName) {
  TInt IdCol = IntCols.Add();
  IntCols[IdCol].Reserve(NumRows, NumRows);
  TInt IdCnt = 0;
  RowIdMap.Clr();
  for (ExplicitStringRowIterator RI = BegRI(); RI < EndRI(); RI++) {
    IntCols[IdCol][RI.GetRowIdx()] = IdCnt;
    RowIdMap.AddDat(IdCnt, RI.GetRowIdx());
    IdCnt++;
  }
  AddSchemaCol(ColName, atInt);
  AddColType(ColName, atInt, IntCols.Len()-1);
}

void ExplicitStringTable::PrintSize(){
	printf("Total number of rows: %d\n", NumRows.Val);
	printf("Number of valid rows: %d\n", NumValidRows.Val);
	printf("Number of Int columns: %d\n", IntCols.Len());
	printf("Number of Flt columns: %d\n", FltCols.Len());
	printf("Number of Str columns: %d\n", StrCols.Len());
	TSize MemUsed = GetMemUsedKB();
	printf("Approximated size is %lu KB\n", MemUsed);
}

TSize ExplicitStringTable::GetMemUsedKB() {
  TSize ApproxSize = 0;
  ApproxSize += Next.GetMemUsed()/1000;  // Next vector
  for(int i = 0; i < IntCols.Len(); i++){
  	ApproxSize += IntCols[i].GetMemUsed()/1000;
  }
  for(int i = 0; i < FltCols.Len(); i++){
  	ApproxSize += FltCols[i].GetMemUsed()/1000;
  }
  TSize StringLens = 0;
  for(int i = 0; i < StrCols.Len(); i++){
  	ApproxSize += StrCols[i].GetMemUsed()/1000;
  	for(int j = 0; j < NumRows; j++){
  		StringLens += StrCols[i][j].Len();  
  	}
  }
  ApproxSize += StringLens / 1000;
  ApproxSize += RowIdMap.GetMemUsed()/1000;
  return ApproxSize;
}

/************************** Physical Order Table ************************************/

TInt PhysicalOrderTable::UseMP = 1;

PhysicalOrderTable::PhysicalOrderTable(): Context(*(new PhysicalOrderTableContext)), NumRows(0){}

PhysicalOrderTable::PhysicalOrderTable(const Schema& TableSchema, PhysicalOrderTableContext& Context): Context(Context), NumRows(0) {
  TInt IntColCnt = 0;
  TInt FltColCnt = 0;
  TInt StrColCnt = 0;
  for (TInt i = 0; i < TableSchema.Len(); i++) {
    TStr ColName = TableSchema[i].Val1;
    TAttrType ColType = TableSchema[i].Val2;
    AddSchemaCol(ColName, ColType);
    switch (ColType) {
      case atInt:
        AddColType(ColName, atInt, IntColCnt);
        IntColCnt++;
        break;
      case atFlt:
        AddColType(ColName, atFlt, FltColCnt);
        FltColCnt++;
        break;
      case atStr:
        AddColType(ColName, atStr, StrColCnt);
        StrColCnt++;
        break;
    }
  }
  IntCols = TVec<TIntV>(IntColCnt);
  FltCols = TVec<TFltV>(FltColCnt);
  StrColMaps = TVec<TIntV>(StrColCnt);
}

void PhysicalOrderTable::AddIdColumn(const TStr& ColName) {
  TInt IdCol = IntCols.Add();
  IntCols[IdCol].Reserve(NumRows, NumRows);
  TInt IdCnt = 0;
  RowIdMap.Clr();
  for (int i = 0; i < NumRows; i++) {
    IntCols[IdCol][i] = IdCnt;
    RowIdMap.AddDat(IdCnt, i);
    IdCnt++;
  }
  AddSchemaCol(ColName, atInt);
  AddColType(ColName, atInt, IntCols.Len()-1);
}


void PhysicalOrderTable::AddStrVal(const TInt& ColIdx, const TStr& Val) {
  TInt Key = TInt(Context.StringVals.AddKey(Val));
  StrColMaps[ColIdx].Add(Key);
}

void PhysicalOrderTable::AddStrVal(const TStr& Col, const TStr& Val) {
  if (GetColType(Col) != atStr) {
    TExcept::Throw(Col + " is not a string valued column");
  }
  AddStrVal(GetColIdx(Col), Val);
}

inline TInt PhysicalOrderTable::CompareRows(TInt R1, TInt R2, const TAttrType& CompareByType, const TInt& CompareByIndex, TBool Asc) {
  switch (CompareByType) {
    case atInt:{
      if (IntCols[CompareByIndex][R1] > IntCols[CompareByIndex][R2]) { return (Asc ? 1 : -1); }
      if (IntCols[CompareByIndex][R1] < IntCols[CompareByIndex][R2]) { return (Asc ? -1 : 1); }
      return 0;
    }
    case atFlt:{
      if (FltCols[CompareByIndex][R1] > FltCols[CompareByIndex][R2]) { return (Asc ? 1 : -1); }
      if (FltCols[CompareByIndex][R1] < FltCols[CompareByIndex][R2]) { return (Asc ? -1 : 1); }
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

inline TInt PhysicalOrderTable::CompareRows(TInt R1, TInt R2, const TVec<TAttrType>& CompareByTypes, const TIntV& CompareByIndices, TBool Asc) {
  for (TInt i = 0; i < CompareByTypes.Len(); i++) {
    TInt res = CompareRows(R1, R2, CompareByTypes[i], CompareByIndices[i], Asc);
    if (res != 0) { return res; }
  }
  return 0;
}

TInt PhysicalOrderTable::GetPivot(TIntV& V, TInt StartIdx, TInt EndIdx, const TVec<TAttrType>& SortByTypes, const TIntV& SortByIndices, TBool Asc) {
  TInt L = EndIdx - StartIdx + 1;
  const TInt Idx1 = StartIdx + TInt::GetRnd(L);
  const TInt Idx2 = StartIdx + TInt::GetRnd(L);
  const TInt Idx3 = StartIdx + TInt::GetRnd(L);
  if (CompareRows(V[Idx1], V[Idx2], SortByTypes, SortByIndices, Asc) < 0) {
    if (CompareRows(V[Idx2], V[Idx3], SortByTypes, SortByIndices, Asc) < 0) { return Idx2; }
    if (CompareRows(V[Idx1], V[Idx3], SortByTypes, SortByIndices, Asc) < 0) { return Idx3; }
    return Idx1;
  } else {
    if (CompareRows(V[Idx3], V[Idx2], SortByTypes, SortByIndices, Asc) < 0) { return Idx2; }
    if (CompareRows(V[Idx3], V[Idx1], SortByTypes, SortByIndices, Asc) < 0) { return Idx3; }
    return Idx1;
  }
}

TInt PhysicalOrderTable::Partition(TIntV& V, TInt StartIdx, TInt EndIdx, const TVec<TAttrType>& SortByTypes, const TIntV& SortByIndices, TBool Asc) {
  // test if the elements are already sorted
  TInt j;
  for (j = StartIdx; j < EndIdx; j++) {
    if (CompareRows(V[j], V[j+1], SortByTypes, SortByIndices, Asc) > 0) {
      break;
    }
  }
  if (j >= EndIdx) {
    return EndIdx+1;
  }

  TInt PivotIdx = GetPivot(V, StartIdx, EndIdx, SortByTypes, SortByIndices, Asc);
  TInt Pivot = V[PivotIdx];
  V.Swap(PivotIdx, EndIdx);
  TInt StoreIdx = StartIdx;
  for (TInt i = StartIdx; i < EndIdx; i++) {
    if (CompareRows(V[i], Pivot, SortByTypes, SortByIndices, Asc) <= 0) {
      V.Swap(i, StoreIdx);
      StoreIdx++;
    }
  }
  // move pivot value to its place
  V.Swap(StoreIdx, EndIdx);
  return StoreIdx;
}

void PhysicalOrderTable::ISort(TIntV& V, TInt StartIdx, TInt EndIdx, const TVec<TAttrType>& SortByTypes, const TIntV& SortByIndices, TBool Asc) {
  if (StartIdx < EndIdx) {
    for (TInt i = StartIdx+1; i <= EndIdx; i++) {
      TInt Val = V[i];
      TInt j = i;
      while ((StartIdx < j) && (CompareRows(V[j-1], Val, SortByTypes, SortByIndices, Asc) > 0)) {
        V[j] = V[j-1];
        j--;
      }
      V[j] = Val;
    }
  }
}

void PhysicalOrderTable::QSort(TIntV& V, TInt StartIdx, TInt EndIdx, const TVec<TAttrType>& SortByTypes, const TIntV& SortByIndices, TBool Asc) {
  if (StartIdx < EndIdx) {
    if (EndIdx - StartIdx < 20) {
      ISort(V, StartIdx, EndIdx, SortByTypes, SortByIndices, Asc);
    } else {
      TInt Pivot = Partition(V, StartIdx, EndIdx, SortByTypes, SortByIndices, Asc);
      if (Pivot > EndIdx) {
        return;
      }
      int Ub = Pivot - 1;
      while(Ub >= StartIdx && CompareRows(V[Ub], V[Pivot], SortByTypes, SortByIndices, Asc) == 0){
      	Ub -= 1;
      }
      QSort(V, StartIdx, Ub, SortByTypes, SortByIndices, Asc);
      QSort(V, Pivot+1, EndIdx, SortByTypes, SortByIndices, Asc);
    }
  }
}

void PhysicalOrderTable::Merge(TIntV& V, TInt Idx1, TInt Idx2, TInt Idx3, const TVec<TAttrType>& SortByTypes, const TIntV& SortByIndices, TBool Asc) {
  TInt i = Idx1, j = Idx2;
  TIntV SortedV;
  while  (i < Idx2 && j < Idx3) {
    if (CompareRows(V[i], V[j], SortByTypes, SortByIndices, Asc) <= 0) {
      SortedV.Add(V[i]);
      i++;
    }
    else {
      SortedV.Add(V[j]);
      j++;
    }
  }
  while (i < Idx2) {
    SortedV.Add(V[i]);
    i++;
  }
  while (j < Idx3) {
    SortedV.Add(V[j]);
    j++;
  }

  for (TInt sz = 0; sz < Idx3 - Idx1; sz++) {
    V[Idx1 + sz] = SortedV[sz];
  }
}

#ifdef USE_OPENMP
void PhysicalOrderTable::QSortPar(TIntV& V, const TVec<TAttrType>& SortByTypes, const TIntV& SortByIndices, TBool Asc) {
  TInt NumThreads = 8;
  TInt Sz = V.Len();
  TIntV IndV, NextV;
  for (TInt i = 0; i < NumThreads; i++) {
    IndV.Add(i * (Sz / NumThreads));
  }
  IndV.Add(Sz);

  omp_set_num_threads(NumThreads);
  #pragma omp parallel for
  for (TInt i = 0; i < NumThreads; i++) {
    QSort(V, IndV[i], IndV[i+1] - 1, SortByTypes, SortByIndices, Asc);
  }

  while (NumThreads > 1) {
    omp_set_num_threads(NumThreads / 2);
    #pragma omp parallel for
    for (TInt i = 0; i < NumThreads; i += 2) {
      Merge(V, IndV[i], IndV[i+1], IndV[i+2], SortByTypes, SortByIndices, Asc);
    }

    NextV.Clr();
    for (TInt i = 0; i < NumThreads; i+=2) {
      NextV.Add(IndV[i]);
    }
    NextV.Add(Sz);
    IndV = NextV;

    NumThreads = NumThreads / 2;
  }
}
#endif // USE_OPENMP

void PhysicalOrderTable::SwapRows(TInt Idx1, TInt Idx2){
	for(int c = 0; c < IntCols.Len(); c++){
		IntCols[c].Swap(Idx1,Idx2);
	}
	for(int c = 0; c < FltCols.Len(); c++){
		FltCols[c].Swap(Idx1,Idx2);
	}
	for(int c = 0; c < StrColMaps.Len(); c++){
		StrColMaps[c].Swap(Idx1,Idx2);
	}
}


void PhysicalOrderTable::Order(const TStrV& OrderBy, TStr OrderColName, TBool Asc) {
  // get a vector of all valid row indices
  TIntV ValidRows = TIntV(NumRows);
  for (TInt i = 0; i < NumRows; i++) {
		ValidRows[i] = i;
  }
  TVec<TAttrType> OrderByTypes(OrderBy.Len());
  TIntV OrderByIndices(OrderBy.Len());
  for (TInt i = 0; i < OrderBy.Len(); i++) {
    OrderByTypes[i] = GetColType(OrderBy[i]);
    OrderByIndices[i] = GetColIdx(OrderBy[i]);
  }

  // sort that vector according to the attributes given in "OrderBy" in lexicographic order
#ifdef USE_OPENMP
  if (GetMP()) {
    QSortPar(ValidRows, OrderByTypes, OrderByIndices, Asc);
  } else {
#endif
    QSort(ValidRows, 0, NumRows-1, OrderByTypes, OrderByIndices, Asc);
#ifdef USE_OPENMP
  }
#endif
  // debug
  //for(int i = 0; i < ValidRows.Len(); i++){
  //	printf("%d ", ValidRows[i].Val);
  //}
  //printf("\n");
 
  TBoolV InPlace(NumRows);
  for(int i = 0; i < NumRows; i++){
  	InPlace[i] = false;
  }
  TInt InPlaceCnt = 0;
  TInt cycle_start = 0;
  while(true){
  	TInt cycle_idx = cycle_start;
  	while(!InPlace[ValidRows[cycle_idx]]){
  		SwapRows(cycle_idx, ValidRows[cycle_idx]);
  		InPlace[cycle_idx] = true;
  		cycle_idx = ValidRows[cycle_idx];
  	}
  	InPlace[cycle_idx] = true;
  	while(InPlace[cycle_start]){
  		cycle_start++;
  	}
  	if(cycle_start >= NumRows){ break;}
  }

  // add rank column
  if (!OrderColName.Empty()) {
    TIntV RankCol = TIntV(NumRows);
    for (TInt i = 0; i < NumRows; i++) {
      RankCol[ValidRows[i]] = i;
    }
    IntCols.Add(RankCol);
    AddSchemaCol(OrderColName, atInt);
    AddColType(OrderColName, atInt, IntCols.Len()-1);
  }
}

PPhysicalOrderTable PhysicalOrderTable::LoadSS(const Schema& S, const TStr& InFNm, PhysicalOrderTableContext& Context, 
 const char& Separator, TBool HasTitleLine) {
  TVec<uint64_t> IntGroupByCols;
  bool NoStringCols = true;

  // find schema
  Schema SR = S;

  PPhysicalOrderTable T = New(SR, Context);

  // find col types and check for string cols
  TInt RowLen = SR.Len();
  TVec<TAttrType> ColTypes = TVec<TAttrType>(RowLen);
  for (TInt i = 0; i < RowLen; i++) {
    ColTypes[i] = T->GetSchemaColType(i);
    if (ColTypes[i] == atStr) {
      NoStringCols = false;
    }
  }

#ifdef USE_OPENMP
  // Right now, can load in parallel only in Linux (for mmap) and if
  // there are no string columns
  if (GetMP() && NoStringCols) {
    TSsParserMP Ss(InFNm, Separator);
    Ss.SkipCommentLines();

    // if title line (i.e. names of the columns) is included as first row in the
    // input file - use it to validate schema
    if (HasTitleLine) {
      Ss.Next();  
      if (S.Len() != Ss.GetFlds()) {
        printf("%s\n", Ss[0]); TExcept::Throw("Table Schema Mismatch!");
      }
      for (TInt i = 0; i < Ss.GetFlds(); i++) {
        // remove carriage return char
        TInt L = strlen(Ss[i]);
        if (Ss[i][L-1] < ' ') { Ss[i][L-1] = 0; }
      }
    }

    // Divide remaining part of stream into equal sized chunks
    // Find starting position in stream for each thread
    uint64_t Cnt = 0;
    uint64_t Pos = Ss.GetStreamPos();
    uint64_t Len = Ss.GetStreamLen();
    uint64_t Rem = Len - Pos;
    int NumThreads = omp_get_max_threads();

    uint64_t Delta = Rem / NumThreads;
    if (Delta < 1) Delta = 1;

    TVec<uint64_t> StartIntV(NumThreads);
    TVec<uint64_t> LineCountV(NumThreads);
    TVec<uint64_t> PrefixSumV(NumThreads);

    StartIntV[0] = Pos;
    for (int i = 1; i < NumThreads; i++) {
      StartIntV[i] = StartIntV[i-1] + Delta;
    }
    StartIntV.Add(Len);

    // Find number of lines handled by each thread
    omp_set_num_threads(NumThreads);
#pragma omp parallel for schedule(dynamic) reduction(+:Cnt)
    for (int i = 0; i < NumThreads; i++) {
      LineCountV[i] = Ss.CountNewLinesInRange(StartIntV[i], StartIntV[i+1]);
      Cnt += LineCountV[i];
    }

    // Calculate row index offsets for each thread
    PrefixSumV[0] = 0;
    for (int i = 1; i < NumThreads; i++) {
      PrefixSumV[i] = PrefixSumV[i-1] + LineCountV[i-1];
    }
    Ss.SetStreamPos(Pos);

    // allocate memory for columns
    TInt IntColIdx = 0;
    TInt FltColIdx = 0;
    for (TInt i = 0; i < RowLen; i++) {
      switch (ColTypes[i]) {
        case atInt:
          T->IntCols[IntColIdx].Gen(Cnt);
          IntColIdx++;
          break;
        case atFlt:
          T->FltCols[FltColIdx].Gen(Cnt);
          FltColIdx++;
          break;
        case atStr:
          break;
      }
    }

    Cnt = 0;
    omp_set_num_threads(NumThreads);
#pragma omp parallel for schedule(dynamic) reduction(+:Cnt)
    for (int i = 0; i < NumThreads; i++) {
      // calculate beginning of each line handled by thread
      TVec<uint64_t> LineStartPosV = Ss.GetStartPosV(StartIntV[i], StartIntV[i+1]);

      // parse line and fill rows
      for (uint64_t k = 0; k < (uint64_t) LineStartPosV.Len(); k++) {
        TVec<char*> FieldsV;
        Ss.NextFromIndex(LineStartPosV[k], FieldsV);
        if (FieldsV.Len() != S.Len()) {
          TExcept::Throw("Error reading tsv file");
        }
        TInt IntColIdx = 0;
        TInt FltColIdx = 0;
        TInt RowIdx = PrefixSumV[i] + k;

        for (TInt j = 0; j < RowLen; j++) {
          switch (ColTypes[j]) {
            case atInt:
              T->IntCols[IntColIdx][RowIdx] = \
                  (Ss.GetIntFromFldV(FieldsV, j));
              IntColIdx++;
              break;
            case atFlt:
              T->FltCols[FltColIdx][RowIdx] = \
                  (Ss.GetFltFromFldV(FieldsV, j));
              FltColIdx++;
              break;
            case atStr:
              TExcept::Throw("TTable::LoadSS:: Str Col found\n");
              break;
          }
        }
        Cnt++;
      }
    }

    T->NumRows = Cnt;
    T->IdColName = "_id";
    TInt IdCol = T->IntCols.Add();
    T->IntCols[IdCol].Gen(Cnt);
    // initialize ID column
    omp_set_num_threads(NumThreads);
    #pragma omp parallel for schedule(dynamic, 10000)
    for (uint64_t i = 0; i < Cnt; i++) {
      T->IntCols[IdCol][i] = i;
    }

    T->AddSchemaCol(T->IdColName, atInt);
    T->AddColType(T->IdColName, atInt, T->IntCols.Len()-1);
  
  } else {
  #endif
  #endif
  // Sequential load
  TSsParser Ss(InFNm, Separator);

  // if title line (i.e. names of the columns) is included as first row in the
  // input file - use it to validate schema
  if (HasTitleLine) {
    Ss.Next();  
    if (S.Len() != Ss.GetFlds()) {
      printf("%s\n", Ss[0]); TExcept::Throw("Table Schema Mismatch!");
    }
    for (TInt i = 0; i < Ss.GetFlds(); i++) {
      // remove carriage return char
      TInt L = strlen(Ss[i]);
      if (Ss[i][L-1] < ' ') { Ss[i][L-1] = 0; }
    }
  }

  // populate table columns
  uint64_t Cnt = 0;
  while (Ss.Next()) {
    TInt IntColIdx = 0;
    TInt FltColIdx = 0;
    TInt StrColIdx = 0;
    Assert(Ss.GetFlds() == S.Len()); // compiled only in debug
    if (Ss.GetFlds() != S.Len()) {
      printf("%s\n", Ss[S.Len()]); TExcept::Throw("Error reading tsv file");
    }
    for (TInt i = 0; i < RowLen; i++) {
      int x = -1;
      switch (ColTypes[i]) {
        case atInt:
          x = Ss.GetInt(i);
          T->IntCols[IntColIdx].Add(x);
          //printf("%d ", x);
          IntColIdx++;
          break;
        case atFlt:
          T->FltCols[FltColIdx].Add(Ss.GetFlt(i));
          FltColIdx++;
          break;
        case atStr:
          TInt ColIdx;
          ColIdx = i;
          TStr Sval = TStr(Ss[ColIdx]);
          T->AddStrVal(StrColIdx, Sval);
          StrColIdx++;
          break;
      }
    }
    Cnt += 1;
  }

  T->NumRows = Cnt;
  T->InitIds();
#ifdef USE_OPENMP
  }
#endif
  return T;
}

void PhysicalOrderTable::SaveSS(const TStr& OutFNm) {
  if (NumRows == 0) {
    printf("Table is empty");
    return;
  }
  FILE* F = fopen(OutFNm.CStr(), "w");
  // debug
  if (F == NULL) {
    printf("failed to open file %s\n", OutFNm.CStr());
    perror("fail ");
    return;
  }

  TInt L = Sch.Len();
  // print title (schema)
  fprintf(F, "# ");
  for (TInt i = 0; i < L-1; i++) {
    fprintf(F, "%s\t", Sch[i].Val1.CStr());
  }  
  fprintf(F, "%s\n", Sch[L-1].Val1.CStr());
  // print table contents
  for (int r = 0; r < NumRows; r++) {
  	TInt IntColCnt = 0;
  	TInt FltColCnt = 0;
  	TInt StrColCnt = 0;
    for (TInt i = 0; i < L; i++) {
      char C = (i == L-1) ? '\n' : '\t';
      switch (GetSchemaColType(i)) {
        case atInt: {
          fprintf(F, "%d%c", IntCols[IntColCnt][r].Val, C);
          IntColCnt++;
          break;
        }
        case atFlt: {
          fprintf(F, "%f%c", FltCols[FltColCnt][r].Val, C);
          FltColCnt++;
          break;
        }
        case atStr: {
          fprintf(F, "%s%c", GetStrVal(StrColCnt,r).CStr(), C);
          StrColCnt++;
          break;
        }
      }
    }
  }
  fclose(F);
}

void PhysicalOrderTable::InitIds() {
  IdColName = "_id";
  AddIdColumn(IdColName);
}

