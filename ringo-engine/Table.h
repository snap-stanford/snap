#ifndef TABLE_H
#define TABLE_H
#include "Snap.h"
#include "Predicate.h"
#include "TMetric.h"

/* 
TODO:
1. Bad code duplication everywhere (repetitions for int, flt and str).
   Maybe should probably use templates
2. Give a-priori memory allocation to vector/hash table constructors 
3. Smart pointer for Ttable: type PTable; Remove explicit pointrer usages
4. Create simple classes for complex hash table types <--
5. Use string pools instead of big string vectors <--
6. Remove recursion from GroupAux <-- 
7. Use row ids with uint64
*/
class TTable;
typedef TPt<TTable> PTable;

class TTable{
protected:
	typedef enum{INT, FLT, STR} TYPE;
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
    TRowIterator& operator++(int){CurrRowIdx = Table->Next[CurrRowIdx]; return *this;}
    bool operator < (const TRowIterator& RowI) const{ return CurrRowIdx < RowI.CurrRowIdx;}
    bool operator == (const TRowIterator& RowI) const{ return CurrRowIdx == RowI.CurrRowIdx;}
    TInt GetRowIdx(){ return CurrRowIdx;}
    // we do not check column type in the iterator
    TInt GetIntAttr(TStr Col) const{ TInt ColIdx = Table->ColTypeMap.GetDat(Col).Val2; return Table->IntCols[ColIdx][CurrRowIdx];}
    TFlt GetFltAttr(TStr Col) const{ TInt ColIdx = Table->ColTypeMap.GetDat(Col).Val2; return Table->FltCols[ColIdx][CurrRowIdx];}
    TStr GetStrAttr(TStr Col) const{ return Table->GetStrVal(Col, CurrRowIdx);}   
  };

protected:
  // Reference Counter for Garbage Collection
  TCRef CRef;
  // number of rows in the table
  TInt NumRows;
  // Meta-data for keeping track of valid (existing) rows
  TInt FirstValidRow;
  TIntV Next; 
  // The actual columns - divided by types
	TVec<TIntV> IntCols;
	TVec<TFltV> FltCols;
  // string columns are implemented using a string pool to fight memory fragmentation
  // The value of string column c in row r is StrColVals.GetStr(StrColMaps[c][r])
	TVec<TIntV> StrColMaps; 
  TBigStrPool StrColVals;
	THash<TStr,TPair<TYPE,TInt> > ColTypeMap;
	// grouping statement name --> (group index --> rows that belong to that group)
  // Note that these mappings are invalid after we remove rows
	THash<TStr,THash<TInt,TIntV> > GroupMapping; // use separate class

	TStr WorkingCol;  // do we even need this here ?
  // column to serve as src nodes when constructing the graph
  TStr SrcCol;
  // column to serve as dst nodes when constructing the graph
  TStr DstCol;
  // list of columns to serve as edge attributes
  TStrV EdgeAttrV;
  // list of columns to serve as node attributes
  TStrV NodeAttrV;

  TStr GetStrVal(TStr Col, TInt RowIdx) const{ return StrColVals.GetCStr(StrColMaps[ColTypeMap.GetDat(Col).Val2][RowIdx]);}
  
  // Iterators 
  TRowIterator BegRI() const{ return TRowIterator(FirstValidRow, this);}
  TRowIterator EndRI() const{ return TRowIterator(NumRows-1, this);}
  bool IsRowValid(TInt RowIdx){ return Next[RowIdx] != Invalid;}

	// Store a group indices column in GroupMapping
	void StoreGroupCol(TStr GroupColName, const THash<TInt,TIntV>& grouping);
	// Group/hash by a single column with integer values. Returns hash table with grouping.
  // IndexSet tells what rows to consider. It is the callers responsibility to check that 
  // these rows are valid. An empty IndexSet means taking all rows into consideration.
	void GroupByIntCol(TStr GroupBy, THash<TInt,TIntV>& grouping, const TIntV& IndexSet); 
	// Group/hash by a single column with float values. Returns hash table with grouping.
	void GroupByFltCol(TStr GroupBy, THash<TFlt,TIntV>& grouping, const TIntV& IndexSet);
	// Group/hash by a single column with string values. Returns hash table with grouping.
	void GroupByStrCol(TStr GroupBy, THash<TStr,TIntV>& grouping, const TIntV& IndexSet);
	// Performs grouping according to the values of columns GroupBy[i] where 
	// i >= GroupByStartIdx; Considers only tuples whose indices are in IndexSet
	// Adds the groups to hash table "grouping". Does not write to "GroupMapping"
	void GroupAux(const TStrV& GroupBy, TInt GroupByStartIdx, THash<TInt,TIntV>& grouping, const TIntV& IndexSet);
  /* template for utility functions to be used by GroupByXCol */
 template <class T>
  void UpdateGrouping(THash<T,TIntV>& Grouping, T Key, TInt Val){
    if(Grouping.IsKey(Key)){
      Grouping.GetDat(Key).Add(Val);
    } else{
      TIntV NewGroup;
      NewGroup.Add(Val);
      Grouping.AddDat(Key, NewGroup);
    }
  }

  void RemoveRow(TInt RowIdx);
  void RemoveRows(const TIntV& RemoveV);
  // remove all rows that are not mentioned in the SORTED vector KeepV
  void KeepSortedRows(const TIntV& KeepV);


public:
	// Nikhil + Jason
	TTable();
	TTable(TSIn& SIn);
	TTable(const TTable& Table);
  static PTable New(){ return new TTable();}
	static PTable Load(TSIn& SIn);
	void Save(TSOut& SOut);
	PNEAGraph ToGraph();

  /* Getters of data required for building a graph out of the table */
	TStr GetSrcCol() const { return SrcCol; }
	TStr GetDstCol() const { return DstCol; }
	TStrV GetNodeIntAttrV() const;
	TStrV GetEdgeIntAttrV() const;
	TStrV GetNodeFltAttrV() const;
	TStrV GetEdgeFltAttrV() const;
	TStrV GetNodeStrAttrV() const;
	TStrV GetEdgeStrAttrV() const;
	TYPE GetColType(TStr ColName) { return ColTypeMap.GetDat(ColName).Val1; };

	// Yonathan
	// change working column ; not sure if this should be a part of the public interface
	void ChangeWorkingCol(TStr column); 
	// rename / add a label to a column
	void AddLabel(TStr column, TStr newLabel);

	// Yonathan
	// Remove rows with duplicate values in given columns
	void Unique(TStr col);
	// select - remove rows for which the given predicate doesn't hold
	// Nikhil + Jason
	void Select(const TPredicate& Predicate);
	
	// Yonathan
	// group by the values of the columns specified in "GroupBy" vector 
	// group indices are stored in GroupCol; Implementation: use GroupMapping hash
	void Group(TStr GroupColName, const TStrV& GroupBy);
	// count - count the number of appearences of the different elements of col
	// record results in column CountCol; Implementation: add a new column to table
	void Count(TStr CountColName, TStr Col);
	// order the rows according to the values in columns of OrderBy (in descending
	// lexicographic order). record order in OrderCol; Implementation: logical indexing / actually remove rows
	void Order(TStr OrderColName, const TStrV& OrderBy);

	// Nikhil + Jason
	// perform equi-join with given columns - i.e. keep tuple pairs where 
	// this->Col1 == Table->Col2; Implementation: Hash-Join - build a hash out of the smaller table
	// hash the larger table and check for collisions
	void Join(TStr Col1, const TTable& Table, TStr Col2);
	// compute distances between elements in this->Col1 and Table->Col2 according
	// to given metric. Store the distances in DistCol, but keep only rows where
	// distance <= threshold
	void Dist(TStr Col1, const TTable& Table, TStr Col2, TStr DistColName, const TMetric& Metric, TFlt threshold);

  // Yonathan
  // Release memory of deleted rows, and defrag
  // also updates meta-data as row indices have changed
  // need some liveness analysis of columns
  void Defrag();

  friend class TPt<TTable>;
};
#endif //TABLE_H