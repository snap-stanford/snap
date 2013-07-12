#ifndef TABLE_H
#define TABLE_H
#include "Snap.h"
#include "Predicate.h"
#include "TMetric.h"

/* 
Guidelines:
1. Bad code duplication everywhere (repetitions for int, flt and str).
   Maybe should probably use templates
2. Give a-priori memory allocation to vector/hash table constructors 
3. Smart pointer for Ttable: type PTable; Remove explicit pointrer usages
4. Create simple classes for complex hash table types / implement accessors
5. Use string pools instead of big string vectors
6. Use row ids / NumRows with uint64 (TODO)

TODO : - Add a function to support tables for undirected graphs
       - Still have a bug with copy-c'tor

Q: after self-join T.Join(C,T,C) => to which column does C address ? how do we access the other one ?
  should we explicitly add different prefixes to column names ? - i.e. test for the case where both tables have the same name 
  Is such a self-join allowed, or should we just do T' = T; T.Join(C,T',C);  / load both T and T'?
*/
class TTable;
typedef TPt<TTable> PTable;

class TTable{
public:
  typedef enum {INT, FLT, STR} TYPE;   // must be consistent with TYPE definition in TPredicate
  typedef TVec<TPair<TStr, TYPE> > Schema; 
protected:
  // special values for Next column
  static const TInt Last;
  static const TInt Invalid;
public:
  // An iterator class to iterate over all currently existing rows
  // Iteration over the rows should be done using only this iterator
  class TRowIterator{
    TInt CurrRowIdx;
    const TTable* Table; 
  public:
    TRowIterator(): CurrRowIdx(0), Table(NULL){}
    TRowIterator(TInt RowIdx, const TTable* TablePtr): CurrRowIdx(RowIdx), Table(TablePtr){}
    TRowIterator(const TRowIterator& RowI): CurrRowIdx(RowI.CurrRowIdx), Table(RowI.Table){}
    TRowIterator& operator++(int){
      CurrRowIdx = Table->Next[CurrRowIdx];
      Assert(CurrRowIdx != Invalid);
      return *this;
    }
    bool operator < (const TRowIterator& RowI) const{ 
      if(CurrRowIdx == TTable::Last){ return false;}
      if(RowI.CurrRowIdx == TTable::Last){ return true;}
      return CurrRowIdx < RowI.CurrRowIdx;
    }
    bool operator == (const TRowIterator& RowI) const{ return CurrRowIdx == RowI.CurrRowIdx;}
    TInt GetRowIdx() const { return CurrRowIdx;}
    // we do not check column type in the iterator
    TInt GetIntAttr(TStr Col) const{ TInt ColIdx = Table->ColTypeMap.GetDat(Col).Val2; return Table->IntCols[ColIdx][CurrRowIdx];}
    TFlt GetFltAttr(TStr Col) const{ TInt ColIdx = Table->ColTypeMap.GetDat(Col).Val2; return Table->FltCols[ColIdx][CurrRowIdx];}
    TStr GetStrAttr(TStr Col) const{ return Table->GetStrVal(Col, CurrRowIdx);}   
  };

  class TRowIteratorWithRemove{
    TInt CurrRowIdx;
    TTable* Table;
    TBool Start;
  public:
    public:
    TRowIteratorWithRemove(): CurrRowIdx(0), Table(NULL), Start(true){}
    TRowIteratorWithRemove(TInt RowIdx, TTable* TablePtr): CurrRowIdx(RowIdx), Table(TablePtr), Start(RowIdx == TablePtr->FirstValidRow){}
    TRowIteratorWithRemove(TInt RowIdx, TTable* TablePtr, TBool IsStart): CurrRowIdx(RowIdx), Table(TablePtr), Start(IsStart){}
    TRowIteratorWithRemove(const TRowIteratorWithRemove& RowI): CurrRowIdx(RowI.CurrRowIdx), Table(RowI.Table), Start(RowI.Start){}
    TRowIteratorWithRemove& operator++(int){
      CurrRowIdx = GetNextRowIdx();
      Start = false;
      Assert(CurrRowIdx != Invalid);
      return *this;
    }
    bool operator < (const TRowIteratorWithRemove& RowI) const{ 
      if(CurrRowIdx == TTable::Last){ return false;}
      if(RowI.CurrRowIdx == TTable::Last){ return true;}
      return CurrRowIdx < RowI.CurrRowIdx;
    }
    bool operator == (const TRowIteratorWithRemove& RowI) const{ return CurrRowIdx == RowI.CurrRowIdx;}
    TInt GetRowIdx() const{ return CurrRowIdx;}
    TInt GetNextRowIdx() const { return (Start ? Table->FirstValidRow : Table->Next[CurrRowIdx]);}
    // we do not check column type in the iterator
    TInt GetNextIntAttr(TInt ColIdx) const{ return Table->IntCols[ColIdx][GetNextRowIdx()];}
    TFlt GetNextFltAttr(TInt ColIdx) const{ return Table->FltCols[ColIdx][GetNextRowIdx()];}
    TStr GetNextStrAttr(TInt ColIdx) const{ return Table->GetStrVal(ColIdx, GetNextRowIdx());}   
    TInt GetNextIntAttr(TStr Col) const{ TInt ColIdx = Table->ColTypeMap.GetDat(Col).Val2; return Table->IntCols[ColIdx][GetNextRowIdx()];}
    TFlt GetNextFltAttr(TStr Col) const{ TInt ColIdx = Table->ColTypeMap.GetDat(Col).Val2; return Table->FltCols[ColIdx][GetNextRowIdx()];}
    TStr GetNextStrAttr(TStr Col) const{ return Table->GetStrVal(Col, GetNextRowIdx());}   
    TBool IsFirst() const { return CurrRowIdx == Table->FirstValidRow;}
    void RemoveNext();
  };

public:
  TStr Name;
protected:
  Schema S;
  // Reference Counter for Garbage Collection
  TCRef CRef;
  // number of rows in the table
  TInt NumRows;
  TInt NumValidRows;
  // Meta-data for keeping track of valid (existing) rows
  TInt FirstValidRow;
  TIntV Next; 
  // The actual columns - divided by types
	TVec<TIntV> IntCols;
	TVec<TFltV> FltCols;
  // string columns are implemented using a string pool to fight memory fragmentation
  // The value of string column c in row r is StrColVals.GetStr(StrColMaps[c][r])
  // Q: why not use TStrHash class ?
	TVec<TIntV> StrColMaps; 
  TBigStrPool StrColVals;
  TInt NumOfDistinctStrVals;
  // column name --> <column type, column index among columns of the same type>
	THash<TStr,TPair<TYPE,TInt> > ColTypeMap;
	// grouping statement name --> (group index --> rows that belong to that group)
  // Note that these mappings are invalid after we remove rows
	THash<TStr,THash<TInt,TIntV> > GroupMapping;

	//TStr WorkingCol;  // do we even need this here ?
  // keeping track of "working column" is done by the engine - i.e. the program
  // that calls the methods of TTable objects
  // column to serve as src nodes when constructing the graph
  TStr SrcCol;
  // column to serve as dst nodes when constructing the graph
  TStr DstCol;
  // list of columns to serve as edge attributes
  TStrV EdgeAttrV;
  // list of columns to serve as source node attributes
  TStrV SrcNodeAttrV;
  // list of columns to serve as source node attributes
  TStrV DstNodeAttrV;
  // Node values - i.e. the unique values of src/dst col
  // (assuming non-bipartite graphs)
  TIntV IntNodeVals;
  THash<TFlt, TInt> FltNodeVals;
  TIntV StrNodeVals; // StrColMaps mappings

  // no type checking. assuming ColName actually refers to the right type.
  TInt GetIntVal(TStr ColName, TInt RowIdx){ return IntCols[ColTypeMap.GetDat(ColName).Val2][RowIdx];}
  TFlt GetFltVal(TStr ColName, TInt RowIdx){ return FltCols[ColTypeMap.GetDat(ColName).Val2][RowIdx];}
  TStr GetStrVal(TInt ColIdx, TInt RowIdx) const{ return StrColVals.GetCStr(StrColMaps[ColIdx][RowIdx]);}
  TStr GetStrVal(TStr Col, TInt RowIdx) const{ return GetStrVal(ColTypeMap.GetDat(Col).Val2, RowIdx);}
  void AddStrVal(TInt ColIdx, TStr Val);
  void AddStrVal(TStr Col, TStr Val);
  // Various Getters
  TStr GetSchemaColName(TInt Idx) const{ return S[Idx].Val1;}
  TYPE GetSchemaColType(TInt Idx) const{ return S[Idx].Val2;}
  void AddSchemaCol(TStr ColName, TYPE ColType) { S.Add(TPair<TStr,TYPE>(ColName, ColType));}
  TInt GetColIdx(TStr ColName) const{ return ColTypeMap.GetDat(ColName).Val2;}  // column index among columns of the same type
  void AddGraphAttribute(TStr Attr, TBool IsEdge, TBool IsSrc, TBool IsDst);
  void AddGraphAttributeV(TStrV& Attrs, TBool IsEdge, TBool IsSrc, TBool IsDst);

  // Iterators 
  TRowIterator BegRI() const { return TRowIterator(FirstValidRow, this);}
  TRowIterator EndRI() const { return TRowIterator(TTable::Last, this);}
  TRowIteratorWithRemove BegRIWR(){ return TRowIteratorWithRemove(FirstValidRow, this);}
  TRowIteratorWithRemove EndRIWR(){ return TRowIteratorWithRemove(TTable::Last, this);}
  bool IsRowValid(TInt RowIdx) const{ return Next[RowIdx] != Invalid;}

  /***** OLD grouping unility functions *************/
	// Group/hash by a single column with integer values. Returns hash table with grouping.
  // IndexSet tells what rows to consider. It is the callers responsibility to check that 
  // these rows are valid. An empty IndexSet means taking all rows into consideration.
	void GroupByIntCol(TStr GroupBy, THash<TInt,TIntV>& grouping, const TIntV& IndexSet, TBool All) const; 
	// Group/hash by a single column with float values. Returns hash table with grouping.
	void GroupByFltCol(TStr GroupBy, THash<TFlt,TIntV>& grouping, const TIntV& IndexSet, TBool All) const;
	// Group/hash by a single column with string values. Returns hash table with grouping.
	void GroupByStrCol(TStr GroupBy, THash<TStr,TIntV>& grouping, const TIntV& IndexSet, TBool All) const;
	// Performs grouping according to the values of columns GroupBy[i] where 
	// i >= GroupByStartIdx; Considers only tuples whose indices are in IndexSet
	// Adds the groups to hash table "grouping". Does not write to "GroupMapping"
	//void GroupAux(const TStrV& GroupBy, TInt GroupByStartIdx, THash<TInt,TIntV>& grouping, const TIntV& IndexSet, TBool InSet);

  void GroupAux(const TStrV& GroupBy, THash<TInt,TIntV>& Grouping, TIntV& UniqueVec, TBool Ordered = true, TBool KeepUnique = false, TBool KeepGrouping = true);
  // Store a group indices column in GroupMapping
	void StoreGroupCol(TStr GroupColName, const THash<TInt,TIntV>& grouping);
  /* template for utility function to update a grouping hash map */
 template <class T>
  void UpdateGrouping(THash<T,TIntV>& Grouping, T Key, TInt Val) const{
    if(Grouping.IsKey(Key)){
      Grouping.GetDat(Key).Add(Val);
    } else{
      TIntV NewGroup;
      NewGroup.Add(Val);
      Grouping.AddDat(Key, NewGroup);
    }
  }

  // returns positive value if R1 is bigger, negative value if R2 is bigger, and 0 if they are equal (strcmp semantics)
  TInt CompareRows(TInt R1, TInt R2, const TStr CompareBy);
  TInt CompareRows(TInt R1, TInt R2, const TStrV& CompareBy); 
  TInt GetPivot(TIntV& V, TInt StartIdx, TInt EndIdx, const TStrV& SortBy);
  TInt Partition(TIntV& V, TInt StartIdx, TInt EndIdx, const TStrV& SortBy);
  void ISort(TIntV& V, TInt StartIdx, TInt EndIdx, const TStrV& SortBy);
  void QSort(TIntV& V, TInt StartIdx, TInt EndIdx, const TStrV& SortBy);

  void RemoveFirstRow();
  void RemoveRow(TInt RowIdx);
  void RemoveRows(const TIntV& RemoveV);
  // remove all rows that are not mentioned in the SORTED vector KeepV
  void KeepSortedRows(const TIntV& KeepV);
  // Initialize an empty table for the join of this table with the given table
  PTable InitializeJointTable(const TTable& Table);
  // add joint row T1[RowIdx1]<=>T2[RowIdx2]
  void AddJointRow(const TTable& T1, const TTable& T2, TInt RowIdx1, TInt RowIdx2);
  // preparation for graph generation of final table
  void GraphPrep();
public:
  // do we need an assignment operator?
	TTable(): NumRows(0), NumValidRows(0), FirstValidRow(0), NumOfDistinctStrVals(0){}  
  TTable(const TStr& TableName, const Schema& S);
  TTable(TSIn& SIn){}  // TODO
  // NOTE: Copy-constructor currently doesn't work because a bug in the copy-c'tor of TBigStrPool
  TTable(const TTable& Table): Name(Table.Name), NumRows(Table.NumRows),
    NumValidRows(Table.NumValidRows), FirstValidRow(Table.FirstValidRow),
    Next(Table.Next), IntCols(Table.IntCols), FltCols(Table.FltCols),
    StrColMaps(Table.StrColMaps), StrColVals(Table.StrColVals), 
    NumOfDistinctStrVals(Table.NumOfDistinctStrVals), ColTypeMap(Table.ColTypeMap),
    GroupMapping(Table.GroupMapping), SrcCol(Table.SrcCol), DstCol(Table.DstCol),
    EdgeAttrV(Table.EdgeAttrV), SrcNodeAttrV(Table.SrcNodeAttrV), DstNodeAttrV(Table.DstNodeAttrV){} 

  static PTable New(){ return new TTable();}
  static PTable New(const TStr& TableName, const Schema& S){ return new TTable(TableName, S);}
  static PTable Load(TSIn& SIn){ return new TTable(SIn);} 
  // Load table from spread sheet (TSV, CSV, etc)
  static PTable LoadSS(const TStr& TableName, const Schema& S, const TStr& InFNm, const char& Separator = '\t', TBool HasTitleLine = true);
  static PTable LoadSS(const TStr& TableName, const Schema& S, const TStr& InFNm, const TIntV& RelevantCols, const char& Separator = '\t', TBool HasTitleLine = true);
  void SaveSS(const TStr& OutFNm);
	void Save(TSOut& SOut);
	PNEANet ToGraph();
  /* Getters and Setters of data required for building a graph out of the table */
	TStr GetSrcCol() const { return SrcCol; }
  void SetSrcCol(TStr Src) {
    // debug
    //for(THash<TStr,TPair<TYPE,TInt>>::TIter it = ColTypeMap.BegI(); it < ColTypeMap.EndI(); it++){printf("%s\n", it.GetKey().CStr());}
    if(!ColTypeMap.IsKey(Src)){TExcept::Throw(Src + ": no such column");}
    SrcCol = Src;
  }
	TStr GetDstCol() const { return DstCol; }
  void SetDstCol(TStr Dst) {
    if(!ColTypeMap.IsKey(Dst)){TExcept::Throw(Dst + ": no such column");}
    DstCol = Dst;
  }
  void AddEdgeAttr(TStr Attr){AddGraphAttribute(Attr, true, false, false);}
  void AddEdgeAttr(TStrV& Attrs){AddGraphAttributeV(Attrs, true, false, false);}
  void AddSrcNodeAttr(TStr Attr){AddGraphAttribute(Attr, false, true, false);}
  void AddSrcNodeAttr(TStrV& Attrs){AddGraphAttributeV(Attrs, false, true, false);}
  void AddDstNodeAttr(TStr Attr){AddGraphAttribute(Attr, false, false, true);}
  void AddDstNodeAttr(TStrV& Attrs){AddGraphAttributeV(Attrs, false, false, true);}
  // TODO: this is very wasteful in the non-disjoint ("non-bipartite") case
  // as it will lead to an iteration over the src nodes as well as the dst nodes, even thougth 
  // the intersection betweeen these two node sets can be very significant
  // Need to have a separate NodeAttributes vector for the non-disjoint case..
  void AddNodeAttr(TStr Attr){AddSrcNodeAttr(Attr); AddDstNodeAttr(Attr);}
  void AddNodeAttr(TStrV& Attrs){AddSrcNodeAttr(Attrs); AddDstNodeAttr(Attrs);}

	TStrV GetSrcNodeIntAttrV() const;
  TStrV GetDstNodeIntAttrV() const;
	TStrV GetEdgeIntAttrV() const;
	TStrV GetSrcNodeFltAttrV() const;
  TStrV GetDstNodeFltAttrV() const;
	TStrV GetEdgeFltAttrV() const;
	TStrV GetSrcNodeStrAttrV() const;
  TStrV GetDstNodeStrAttrV() const;
	TStrV GetEdgeStrAttrV() const;


	TYPE GetColType(TStr ColName) const{ return ColTypeMap.GetDat(ColName).Val1; };
  TInt GetNumRows() const { return NumRows;}
  TInt GetNumValidRows() const { return NumValidRows;}

	// change working column ; not really needed in TTable
	// void ChangeWorkingCol(TStr column); 

	// rename / add a label to a column
	void AddLabel(TStr column, TStr newLabel);

	// Remove rows with duplicate values in given columns
	void Unique(TStrV Cols, TBool Ordered = true);
  void UniqueExistingGroup(TStr GroupStmt);
	// select - remove rows for which the given predicate doesn't hold
	void Select(TPredicate& Predicate);

  // select atomic - remove rows for which the comparison doesn't hold
  void SelectAtomic(TStr Col1, TStr Col2, TPredicate::COMP Cmp);
  void SelectAtomicIntConst(TStr Col1, TInt Val2, TPredicate::COMP Cmp);
  void SelectAtomicStrConst(TStr Col1, TStr Val2, TPredicate::COMP Cmp);
  void SelectAtomicFltConst(TStr Col1, TFlt Val2, TPredicate::COMP Cmp);
	
	// group by the values of the columns specified in "GroupBy" vector 
	// group indices are stored in GroupCol; Implementation: use GroupMapping hash
	void Group(TStr GroupColName, const TStrV& GroupBy, TBool Ordered = true);
	// count - count the number of appearences of the different elements of col
	// record results in column CountCol; Implementation: add a new column to table
	void Count(TStr CountColName, TStr Col);
	// order the rows according to the values in columns of OrderBy (in descending
	// lexicographic order). 
	void Order(const TStrV& OrderBy, TStr OrderColName = "");

	// perform equi-join with given columns - i.e. keep tuple pairs where 
	// this->Col1 == Table->Col2; Implementation: Hash-Join - build a hash out of the smaller table
	// hash the larger table and check for collisions
	PTable Join(TStr Col1, const TTable& Table, TStr Col2);
  PTable SelfJoin(TStr Col){return Join(Col, *this, Col);}
	// compute distances between elements in this->Col1 and Table->Col2 according
	// to given metric. Store the distances in DistCol, but keep only rows where
	// distance <= threshold
	void Dist(TStr Col1, const TTable& Table, TStr Col2, TStr DistColName, const TMetric& Metric, TFlt threshold);

  // Release memory of deleted rows, and defrag
  // also updates meta-data as row indices have changed
  // need some liveness analysis of columns
  void Defrag();

  /* Special Filters to be applied at the end */
  PTable IsNextK(TStr OrderCol, TInt K, TStr GroupBy);

   // helper functions
 private:
 TInt GetNId(TStr Col, TInt RowIdx);
 void BuildGraphTopology(PNEANet& Graph);
 void AddNodeAttributes(PNEANet& Graph);
 void AddEdgeAttributes(PNEANet& Graph);

 friend class TPt<TTable>;
};
#endif //TABLE_H
