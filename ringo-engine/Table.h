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

  TStr GetStrVal(TInt ColIdx, TInt RowIdx) const{ return StrColVals.GetCStr(StrColMaps[ColIdx][RowIdx]);}
  TStr GetStrVal(TStr Col, TInt RowIdx) const{ return GetStrVal(ColTypeMap.GetDat(Col).Val2, RowIdx);}
  void AddStrVal(TInt ColIdx, TStr Val);
  void AddStrVal(TStr Col, TStr Val);
  // Various Getters
  TStr GetSchemaColName(TInt Idx) const{ return S[Idx].Val1;}
  TYPE GetSchemaColType(TInt Idx) const{ return S[Idx].Val2;}
  void AddSchemaCol(TStr ColName, TYPE ColType) { S.Add(TPair<TStr,TYPE>(ColName, ColType));}
  TInt GetColIdx(TStr ColName) const{ return ColTypeMap.GetDat(ColName).Val2;}  // column index among columns of the same type

  // Iterators 
  TRowIterator BegRI() const { return TRowIterator(FirstValidRow, this);}
  TRowIterator EndRI() const { return TRowIterator(TTable::Last, this);}
  TRowIteratorWithRemove BegRIWR(){ return TRowIteratorWithRemove(FirstValidRow, this);}
  TRowIteratorWithRemove EndRIWR(){ return TRowIteratorWithRemove(TTable::Last, this);}
  bool IsRowValid(TInt RowIdx) const{ return Next[RowIdx] != Invalid;}

	// Store a group indices column in GroupMapping
	void StoreGroupCol(TStr GroupColName, const THash<TInt,TIntV>& grouping);
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
	void GroupAux(const TStrV& GroupBy, TInt GroupByStartIdx, THash<TInt,TIntV>& grouping, const TIntV& IndexSet, TBool InSet);
  /* template for utility functions to be used by GroupByXCol */
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

  void RemoveFirstRow();
  void RemoveRow(TInt RowIdx);
  void RemoveRows(const TIntV& RemoveV);
  // remove all rows that are not mentioned in the SORTED vector KeepV
  void KeepSortedRows(const TIntV& KeepV);
  // Initialize an empty table for the join of this table with the given table
  PTable InitializeJointTable(const TTable& Table);
  // add joint row T1[RowIdx1]<=>T2[RowIdx2]
  void AddJointRow(const TTable& T1, const TTable& T2, TInt RowIdx1, TInt RowIdx2);

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
	PNEAGraph ToGraph();
  /* Getters of data required for building a graph out of the table */
	TStr GetSrcCol() const { return SrcCol; }
	TStr GetDstCol() const { return DstCol; }
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
	void Unique(TStr col);
	// select - remove rows for which the given predicate doesn't hold
	void Select(TPredicate& Predicate);
	
	// group by the values of the columns specified in "GroupBy" vector 
	// group indices are stored in GroupCol; Implementation: use GroupMapping hash
	void Group(TStr GroupColName, const TStrV& GroupBy);
	// count - count the number of appearences of the different elements of col
	// record results in column CountCol; Implementation: add a new column to table
	void Count(TStr CountColName, TStr Col);
	// order the rows according to the values in columns of OrderBy (in descending
	// lexicographic order). record order in OrderCol; Implementation: logical indexing / actually remove rows
	void Order(TStr OrderColName, const TStrV& OrderBy);

	// perform equi-join with given columns - i.e. keep tuple pairs where 
	// this->Col1 == Table->Col2; Implementation: Hash-Join - build a hash out of the smaller table
	// hash the larger table and check for collisions
	PTable Join(TStr Col1, const TTable& Table, TStr Col2);
	// compute distances between elements in this->Col1 and Table->Col2 according
	// to given metric. Store the distances in DistCol, but keep only rows where
	// distance <= threshold
	void Dist(TStr Col1, const TTable& Table, TStr Col2, TStr DistColName, const TMetric& Metric, TFlt threshold);

  // Release memory of deleted rows, and defrag
  // also updates meta-data as row indices have changed
  // need some liveness analysis of columns
  void Defrag();

   // helper functions
 private:
 TInt GetNId(TStr Col, TInt RowIdx, THash<TFlt, TInt>& FSrNodeMap, THash<TFlt, TInt>& FDsNodeMap);
 void BuildGraphTopology(PNEAGraph& Graph, THash<TFlt, TInt>& FSrNodeMap, THash<TFlt, TInt>& FDsNodeMap);
 void AddNodeAttributes(PNEAGraph& Graph, THash<TFlt, TInt>& FSrNodeMap, THash<TFlt, TInt>& FDsNodeMap);
 void AddEdgeAttributes(PNEAGraph& Graph);

  friend class TPt<TTable>;
};
#endif //TABLE_H
