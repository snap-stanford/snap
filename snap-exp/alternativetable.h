#ifndef ALTERNATIVE_TABLE_H
#define ALTERNATIVE_TABLE_H

/*
Alternative implementations (with very limited functionality) for TTable.
These are for benchmarking purposes only. Add this file name to snap-core/Snap.h if used.
*/

/************************** Row-Based data storage in Table ************************************/

class RowBasedTable;
typedef TPt<RowBasedTable> PRowBasedTable;

class RowBasedTableRow{
protected:
  TIntV IntVals;
public:
  RowBasedTableRow() {}
  void AddInt(const TInt& Val) { IntVals.Add(Val); }
  TInt GetIntVal(TInt Idx) const { return IntVals[Idx];}
  TIntV GetIntVals() const { return IntVals; }
};

class RowBasedRowIterator{ 
  TInt CurrRowIdx; ///< Physical row index of current row pointed by iterator.
  const RowBasedTable* Table; ///< Reference to table containing this row.
public:
  RowBasedRowIterator(): CurrRowIdx(0), Table(NULL) {}
  RowBasedRowIterator(TInt RowIdx, const RowBasedTable* TablePtr): CurrRowIdx(RowIdx), Table(TablePtr) {}
  RowBasedRowIterator(const RowBasedRowIterator& RowI): CurrRowIdx(RowI.CurrRowIdx), Table(RowI.Table) {}
  RowBasedRowIterator& operator++(int);
  RowBasedRowIterator& Next(); 
  bool operator < (const RowBasedRowIterator& RowI) const;
  bool operator == (const RowBasedRowIterator& RowI) const;
  TInt GetRowIdx() const;
  TInt GetIntAttr(TInt ColIdx) const;
};

class RowBasedTable{
protected:
	static const TInt Last; 
  	static const TInt Invalid; 
  	Schema Sch; 
    TCRef CRef;
    TInt NumRows; 
    TInt NumValidRows; 
    TInt FirstValidRow; 
    TInt LastValidRow; 
    TIntV Next;
  	TVec<RowBasedTableRow> Rows;
public:
	RowBasedTable(const Schema& S);
	static PRowBasedTable New(const Schema& S) { return new RowBasedTable(S);}
	static PRowBasedTable LoadSS(const Schema& S, const TStr& InFNm, const char& Separator = '\t', TBool HasTitleLine = false);
    void SaveSS(const TStr& OutFNm);
    RowBasedRowIterator BegRI() const { return RowBasedRowIterator(FirstValidRow, this);}
  	RowBasedRowIterator EndRI() const { return RowBasedRowIterator(RowBasedTable::Last, this);}
  	
  friend class TPt<RowBasedTable>;
  friend class RowBasedRowIterator;
}; 


/************************** Explicit String Storage in Table ************************************/
class ExplicitStringTable;
typedef TPt<ExplicitStringTable> PExplicitStringTable;

class ExplicitStringRowIterator{
  TInt CurrRowIdx; ///< Physical row index of current row pointed by iterator.
  const ExplicitStringTable* Table; ///< Reference to table containing this row.
public:
  ExplicitStringRowIterator(): CurrRowIdx(0), Table(NULL) {}
  ExplicitStringRowIterator(TInt RowIdx, const ExplicitStringTable* TablePtr): CurrRowIdx(RowIdx), Table(TablePtr) {}
  ExplicitStringRowIterator(const ExplicitStringRowIterator& RowI): CurrRowIdx(RowI.CurrRowIdx), Table(RowI.Table) {}
  ExplicitStringRowIterator& operator++(int);
  ExplicitStringRowIterator& Next(); 
  bool operator < (const ExplicitStringRowIterator& RowI) const;
  bool operator == (const ExplicitStringRowIterator& RowI) const;
  TInt GetRowIdx() const;
  TInt GetIntAttr(TInt ColIdx) const;
  TFlt GetFltAttr(TInt ColIdx) const;
  TStr GetStrAttr(TInt ColIdx) const;
  TInt GetIntAttr(const TStr& Col) const;
  TFlt GetFltAttr(const TStr& Col) const;
  TStr GetStrAttr(const TStr& Col) const;
  TBool CompareAtomicConst(TInt ColIdx, const TPrimitive& Val, TPredComp Cmp);
  TBool CompareAtomicConstTStr(TInt ColIdx, const  TStr& Val, TPredComp Cmp);
};

class ExplicitStringRowIteratorWithRemove {
  TInt CurrRowIdx; ///< Physical row index of current row pointer by iterator.
  ExplicitStringTable* Table; ///< Reference to table containing this row.
  TBool Start;	///< A flag indicating whether the current row in the first valid row of the table.
public:
  /// Default constructor.
  ExplicitStringRowIteratorWithRemove(): CurrRowIdx(0), Table(NULL), Start(true) {}
  /// Constructs iterator pointing to given row.
  ExplicitStringRowIteratorWithRemove(TInt RowIdx, ExplicitStringTable* TablePtr);
  /// Constructs iterator pointing to given row.
  ExplicitStringRowIteratorWithRemove(TInt RowIdx, ExplicitStringTable* TablePtr, TBool IsStart) : CurrRowIdx(RowIdx), 
    Table(TablePtr), Start(IsStart) {}
  /// Increments the iterator.
  ExplicitStringRowIteratorWithRemove& operator++(int);
  /// Increments the iterator (For Python compatibility).
  ExplicitStringRowIteratorWithRemove& Next(); 
  /// Checks if this iterator points to a row that is before the one pointed by \c RowI.
  bool operator < (const ExplicitStringRowIteratorWithRemove& RowI) const;
  /// Checks if this iterator points to the same row pointed by \c RowI.
  bool operator == (const ExplicitStringRowIteratorWithRemove& RowI) const;
  /// Gets physical index of current row.
  TInt GetRowIdx() const;
  /// Gets physical index of next row.
  TInt GetNextRowIdx() const;
  /// Returns value of integer attribute specified by integer column index for next row.
  TInt GetNextIntAttr(TInt ColIdx) const;
  /// Returns value of float attribute specified by float column index for next row.
  TFlt GetNextFltAttr(TInt ColIdx) const;
  /// Returns value of string attribute specified by string column index for next row.
  TStr GetNextStrAttr(TInt ColIdx) const;     
  /// Checks whether iterator points to first valid row of the table.
  TBool IsFirst() const;
  /// Removes next row.
  void RemoveNext();
  /// Compares value in column \c ColIdx with given primitive \c Val.
  TBool CompareAtomicConst(TInt ColIdx, const TPrimitive& Val, TPredComp Cmp);
};

/* Table that explicitly stores string attributes, without using a string pool */
class ExplicitStringTable {
protected:
  static const TInt Last; ///< Special value for Next vector entry - last row in table.
  static const TInt Invalid; ///< Special value for Next vector entry - logically removed row.

  Schema Sch; ///< Table Schema.
  TCRef CRef;
  TInt NumRows; ///< Number of rows in the table (valid and invalid).
  TInt NumValidRows; ///< Number of valid rows in the table (i.e. rows that were not logically removed).
  TInt FirstValidRow; ///< Physical index of first valid row.
  TInt LastValidRow; ///< Physical index of last valid row.
  TIntV Next; ///< A vector describing the logical order of the rows. 
  TVec<TIntV> IntCols; ///< Data columns of integer attributes.
  TVec<TFltV> FltCols; ///< Data columns of floating point attributes.
  TVec<TStrV> StrCols;
  TStr IdColName; ///< Name of column associated with (optional) permanent row identifiers.
  TIntIntH RowIdMap; ///< Mapping of permanent row ids to physical id.
  THash<TStr,TPair<TAttrType,TInt> > ColTypeMap;
  
  /* Utility functions to handle schema */
  void AddSchemaCol(const TStr& ColName, TAttrType ColType) { 
    Sch.Add(TPair<TStr,TAttrType>(ColName, ColType)); 
  }
  void AddColType(const TStr& ColName, TPair<TAttrType,TInt> ColType) { 
    ColTypeMap.AddDat(ColName, ColType);
  }
  void AddColType(const TStr& ColName, TAttrType ColType, TInt Index) { 
    AddColType(ColName, TPair<TAttrType,TInt>(ColType, Index));
  }
  TBool IsColName(const TStr& ColName) const {
    return ColTypeMap.IsKey(ColName);
  } 
  TPair<TAttrType, TInt> GetColTypeMap(const TStr& ColName) const { 
    return ColTypeMap.GetDat(ColName); 
  }
  TInt GetColIdx(const TStr& ColName) const { 
    return ColTypeMap.IsKey(ColName) ? ColTypeMap.GetDat(ColName).Val2 : TInt(-1); 
  }
  TAttrType GetColType(const TStr& ColName) const { 
    return ColTypeMap.GetDat(ColName).Val1; 
  }
  TStr GetSchemaColName(TInt Idx) const { return Sch[Idx].Val1; }
  TAttrType GetSchemaColType(TInt Idx) const { return Sch[Idx].Val2; }
  TStr RenumberColName(const TStr& ColName) const;
  void AddIdColumn(const TStr& IdColName);
  TStr GetIdColName() const { return IdColName; }
  /* Utility functions for inserting new records */
  void AddRow(const ExplicitStringRowIterator& RI);
  void UpdateTableForNewRow();
  bool IsRowValid(TInt RowIdx) const{ return Next[RowIdx] != Invalid;}
  void RemoveFirstRow();
  void RemoveRow(TInt RowIdx, TInt PrevRowIdx);
  /* Utility functions for Join */
  /// Initializes an empty table for the join of this table with the given table.
  PExplicitStringTable InitializeJointTable(const ExplicitStringTable& Table);
  /// Adds joint row T1[RowIdx1]<=>T2[RowIdx2].
  void AddJointRow(const ExplicitStringTable& T1, const ExplicitStringTable& T2, TInt RowIdx1, TInt RowIdx2);
  template <class T> void GroupByIntCol(const TStr& GroupBy, T& Grouping, const TIntV& IndexSet, TBool All) const;
  template <class T> void GroupByFltCol(const TStr& GroupBy, T& Grouping, const TIntV& IndexSet, TBool All) const;
  template <class T> void GroupByStrCol(const TStr& GroupBy, T& Grouping, const TIntV& IndexSet, TBool All) const;
  template <class T> void UpdateGrouping(THash<T,TIntV>& Grouping, T Key, TInt Val) const;
  void GroupingSanityCheck(const TStr& GroupBy, const TAttrType& AttrType) const;
  
public:
  ExplicitStringTable();
  ExplicitStringTable(const Schema& S);
  static PExplicitStringTable New() { return new ExplicitStringTable();}
  static PExplicitStringTable New(const Schema& S) { return new ExplicitStringTable(S);}
  static PExplicitStringTable LoadSS(const Schema& S, const TStr& InFNm, const char& Separator = '\t', TBool HasTitleLine = false);
  void SaveSS(const TStr& OutFNm);
  void InitIds();
  ExplicitStringRowIterator BegRI() const { return ExplicitStringRowIterator(FirstValidRow, this);}
  ExplicitStringRowIterator EndRI() const { return ExplicitStringRowIterator(ExplicitStringTable::Last, this);}
  ExplicitStringRowIteratorWithRemove BegRIWR(){ return ExplicitStringRowIteratorWithRemove(FirstValidRow, this);}
  ExplicitStringRowIteratorWithRemove EndRIWR(){ return ExplicitStringRowIteratorWithRemove(ExplicitStringTable::Last, this);}
  void SelectAtomicConst(const TStr& Col, const TPrimitive& Val, TPredComp Cmp, 
  	TIntV& SelectedRows, PExplicitStringTable& SelectedTable, TBool Remove, TBool Table);
  PExplicitStringTable Join(const TStr& Col1, const ExplicitStringTable& Table, const TStr& Col2);
  PExplicitStringTable Join(const TStr& Col1, const PExplicitStringTable& Table, const TStr& Col2) { 
    return Join(Col1, *Table, Col2); 
  }
  PExplicitStringTable SelfJoin(const TStr& Col) { return Join(Col, *this, Col);}
  void PrintSize();
  /// Returns approximate memory used by table in [KB]
  TSize GetMemUsedKB();
  
  friend class TPt<ExplicitStringTable>;
  friend class ExplicitStringRowIterator;
  friend class ExplicitStringRowIteratorWithRemove;
};

template <class T>
  void ExplicitStringTable::GroupByIntCol(const TStr& GroupBy, T& Grouping, 
   const TIntV& IndexSet, TBool All) const {
    GroupingSanityCheck(GroupBy, atInt);
    if (All) {
     // Optimize for the common and most expensive case - iterate over only valid rows.
      for (ExplicitStringRowIterator it = BegRI(); it < EndRI(); it++) {
        UpdateGrouping<TInt>(Grouping, it.GetIntAttr(GroupBy), it.GetRowIdx());
      }
    } else {
      // Consider only rows in IndexSet.
      for (TInt i = 0; i < IndexSet.Len(); i++) {
        if (IsRowValid(IndexSet[i])) {
          TInt RowIdx = IndexSet[i];
          const TIntV& Col = IntCols[GetColIdx(GroupBy)];       
          UpdateGrouping<TInt>(Grouping, Col[RowIdx], RowIdx);
		}
      }
    }
  }
  
  template <class T>
  void ExplicitStringTable::GroupByFltCol(const TStr& GroupBy, T& Grouping, 
   const TIntV& IndexSet, TBool All) const {
    GroupingSanityCheck(GroupBy, atFlt);
    if (All) {
       // Optimize for the common and most expensive case - iterate over only valid rows.
      for (ExplicitStringRowIterator it = BegRI(); it < EndRI(); it++) {
        UpdateGrouping<TFlt>(Grouping, it.GetFltAttr(GroupBy), it.GetRowIdx());
      }
    } else {
      // Consider only rows in IndexSet.
      for (TInt i = 0; i < IndexSet.Len(); i++) {
        if (IsRowValid(IndexSet[i])) {
          TInt RowIdx = IndexSet[i];
          const TFltV& Col = FltCols[GetColIdx(GroupBy)];       
          UpdateGrouping<TFlt>(Grouping, Col[RowIdx], RowIdx);
        }
      }
    }
}

  template <class T>
  void ExplicitStringTable::GroupByStrCol(const TStr& GroupBy, T& Grouping, 
   const TIntV& IndexSet, TBool All) const {
    GroupingSanityCheck(GroupBy, atStr);
    if (All) {
      // Optimize for the common and most expensive case - iterate over all valid rows.
      for (ExplicitStringRowIterator it = BegRI(); it < EndRI(); it++) {
        UpdateGrouping<TStr>(Grouping, it.GetStrAttr(GroupBy), it.GetRowIdx());
      }
    } else {
      // Consider only rows in IndexSet.
      for (TInt i = 0; i < IndexSet.Len(); i++) {
        if (IsRowValid(IndexSet[i])) {
          TInt RowIdx = IndexSet[i];     
          const TStrV& Col = StrCols[GetColIdx(GroupBy)];       
          UpdateGrouping<TStr>(Grouping, Col[RowIdx], RowIdx);
        }
      }
    }
  }
  
  template <class T>
  void ExplicitStringTable::UpdateGrouping(THash<T,TIntV>& Grouping, T Key, TInt Val) const{
    if (Grouping.IsKey(Key)) {
      Grouping.GetDat(Key).Add(Val);
    } else {
      TIntV NewGroup;
      NewGroup.Add(Val);
      Grouping.AddDat(Key, NewGroup);
    }
  }
  
/************************** Physical Ordering Table ************************************/  
/// Table class
class PhysicalOrderTable;
class PhysicalOrderTableContext;
typedef TPt<PhysicalOrderTable> PPhysicalOrderTable;

//#//////////////////////////////////////////////
/// Execution context
class PhysicalOrderTableContext {
protected:
  TStrHash<TInt, TBigStrPool> StringVals; ///< StringPool - stores string data values and maps them to integers.
  friend class PhysicalOrderTable;
public:
  /// Default constructor.
  PhysicalOrderTableContext() {}
  /// Loads PhysicalOrderTableContext in binary from \c SIn.
  PhysicalOrderTableContext(TSIn& SIn): StringVals(SIn) {}
  /// Saves PhysicalOrderTableContext in binary to \c SOut.
  void Save(TSOut& SOut) { StringVals.Save(SOut); }
};

//#//////////////////////////////////////////////
/// Table class: Relational table with columnar data storage
class PhysicalOrderTable {
protected:
  static TInt UseMP; ///< Global switch for choosing multi-threaded versions of PhysicalOrderTable functions.
public:
  static void SetMP(TInt Value) { UseMP = Value; }
  static TInt GetMP() { return UseMP; }
  
protected:
  PhysicalOrderTableContext& Context;  ///< Execution Context. ##PhysicalOrderTable::Context
  Schema Sch; ///< Table Schema.
  TCRef CRef;
  TInt NumRows; ///< Number of rows in the table (valid and invalid).
  TVec<TIntV> IntCols; ///< Data columns of integer attributes.
  TVec<TFltV> FltCols; ///< Data columns of floating point attributes.
  TVec<TIntV> StrColMaps; ///< Data columns of integer mappings of string attributes. ##PhysicalOrderTable::StrColMaps
  THash<TStr,TPair<TAttrType,TInt> > ColTypeMap; /// A mapping from column name to column type and column index among columns of the same type.
  TStr IdColName; ///< Name of column associated with (optional) permanent row identifiers.
  TIntIntH RowIdMap; ///< Mapping of permanent row ids to physical id.
  
/***** Utility functions for handling string values *****/
  /// Gets the value in column with id \c ColIdx at row \c RowIdx.
  TStr GetStrVal(TInt ColIdx, TInt RowIdx) const { 
    return TStr(Context.StringVals.GetKey(StrColMaps[ColIdx][RowIdx])); 
  }
  /// Adds \c Val in column with id \c ColIdx.
  void AddStrVal(const TInt& ColIdx, const TStr& Val);
  /// Adds \c Val in column with name \c Col.
  void AddStrVal(const TStr& Col, const TStr& Val);

/***** Utility functions for handling Schema *****/
  /// Gets name of the column with index \c Idx in the schema.
  TStr GetSchemaColName(TInt Idx) const { return Sch[Idx].Val1; }
  /// Gets type of the column with index \c Idx in the schema.
  TAttrType GetSchemaColType(TInt Idx) const { return Sch[Idx].Val2; }
  /// Adds column with name \c ColName and type \c ColType to the schema.
  void AddSchemaCol(const TStr& ColName, TAttrType ColType) { 
    Sch.Add(TPair<TStr,TAttrType>(ColName, ColType)); 
  }
  /// Adds column with name \c ColName and type \c ColType to the ColTypeMap.
  void AddColType(const TStr& ColName, TPair<TAttrType,TInt> ColType) { 
    ColTypeMap.AddDat(ColName, ColType);
  }
  /// Adds column with name \c ColName and type \c ColType to the ColTypeMap.
  void AddColType(const TStr& ColName, TAttrType ColType, TInt Index) { 
    AddColType(ColName, TPair<TAttrType,TInt>(ColType, Index));
  }
  /// Gets index of column \c ColName among columns of the same type in the schema.
  TInt GetColIdx(const TStr& ColName) const { 
    return ColTypeMap.IsKey(ColName) ? ColTypeMap.GetDat(ColName).Val2 : TInt(-1); 
  }
  TAttrType GetColType(const TStr& ColName) const { 
    return ColTypeMap.GetDat(ColName).Val1; 
  }

  /***** Utility functions for sorting by columns *****/
  /// Returns positive value if R1 is bigger, negative value if R2 is bigger, and 0 if they are equal (strcmp semantics).
  inline TInt CompareRows(TInt R1, TInt R2, const TAttrType& CompareByType, 
   const TInt& CompareByIndex, TBool Asc = true);
  /// Returns positive value if R1 is bigger, negative value if R2 is bigger, and 0 if they are equal (strcmp semantics).
  inline TInt CompareRows(TInt R1, TInt R2, const TVec<TAttrType>& CompareByTypes, 
   const TIntV& CompareByIndices, TBool Asc = true); 
  /// Gets pivot element for QSort.
  TInt GetPivot(TIntV& V, TInt StartIdx, TInt EndIdx, const TVec<TAttrType>& SortByTypes, 
   const TIntV& SortByIndices, TBool Asc);
  /// Partitions vector for QSort.
  TInt Partition(TIntV& V, TInt StartIdx, TInt EndIdx, const TVec<TAttrType>& SortByTypes,
   const TIntV& SortByIndices, TBool Asc);
  /// Performs insertion sort on given vector \c V.
  void ISort(TIntV& V, TInt StartIdx, TInt EndIdx, const TVec<TAttrType>& SortByTypes, 
   const TIntV& SortByIndices, TBool Asc = true);
  /// Performs QSort on given vector \c V.
  void QSort(TIntV& V, TInt StartIdx, TInt EndIdx, const TVec<TAttrType>& SortByTypes, 
   const TIntV& SortByIndices, TBool Asc = true);
  /// Helper function for parallel QSort.
  void Merge(TIntV& V, TInt Idx1, TInt Idx2, TInt Idx3, const TVec<TAttrType>& SortByTypes, 
    const TIntV& SortByIndices, TBool Asc = true);
#ifdef USE_OPENMP
  /// Performs QSort in parallel on given vector \c V.
  void QSortPar(TIntV& V, const TVec<TAttrType>& SortByTypes, const TIntV& SortByIndices, 
    TBool Asc = true);
#endif // USE_OPENMP

  /// Adds a column of explicit integer identifiers to the rows.
  void AddIdColumn(const TStr& IdColName);
  
  void SwapRows(TInt Idx1, TInt Idx2);

public:
/***** Constructors *****/
  PhysicalOrderTable(); 
  PhysicalOrderTable(const Schema& S, PhysicalOrderTableContext& Context);

  static PPhysicalOrderTable New() { return new PhysicalOrderTable(); }
  static PPhysicalOrderTable New(const Schema& S, PhysicalOrderTableContext& Context) { 
    return new PhysicalOrderTable(S, Context); 
  }

/***** Save / Load functions *****/
  /// Loads table from spread sheet (TSV, CSV, etc).
  static PPhysicalOrderTable LoadSS(const Schema& S, const TStr& InFNm, PhysicalOrderTableContext& Context, 
   const char& Separator = '\t', TBool HasTitleLine = false);
  /// Saves table schema + content into a TSV file.
  void SaveSS(const TStr& OutFNm);

/***** Table Operations *****/
  /// Orders the rows according to the values in columns of OrderBy (in descending lexicographic order). 
  void Order(const TStrV& OrderBy, TStr OrderColName = "", TBool Asc = true);
  /// Adds explicit row ids, initialize hash set mapping ids to physical rows.
  void InitIds();

  friend class TPt<PhysicalOrderTable>;
};

#endif	// ALTERNATIVE_TABLE_H
