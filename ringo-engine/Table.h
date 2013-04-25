#include "Snap.h"
#include "Predicate.h"
#include "TMetric.h"

/* 
Questions:
1. for loading / saving tables, should we use I/O streams (i.e. TSIn, TSOut) or files ?
2. Use vectors or lists for columns?
TODO:
Bad code duplication everywhere (repetitions for int, flt and str).
Should probably:
1. make better usage of templates or
2. map column values of all types to ints, and keep the columns themselves as
columns of integers
*/
class TTable{
protected:
	typedef enum{INT, FLT, STR} TYPE;
  // number of rows in the table
  TInt NumRows;
  // A tuning parameter. Upon removal of rows (for instance, for select),
  // if the fraction of the rows removed is more than CopyOnRemoveThreshold,
  // than we replace the columns with new copies containing only the remaining rows.
  // This is because removing elements from the vector takes O(n) per element.
  static const TFlt CopyOnRemoveThreshold;
  // The actual columns - divided by types
	TVec<TIntV> IntCols;
	TVec<TFltV> FltCols;
	TVec<TStrV> StrCols;  // TODO: use TVec<TIntV> + string pool instead ?
	THash<TStr,TPair<TYPE,TInt> > ColTypeMap;
	// grouping statement name --> (group index --> rows that belong to that group)
	THash<TStr,THash<TInt,TIntV> > GroupMapping;
	TStr WorkingCol;
  // column to serve as src nodes when constructing the graph
  TStr SrcCol;
  // column to serve as dst nodes when constructing the graph
  TStr DstCol;
  // list of columns to serve as edge attributes
  TStrV EdgeAttrs;
  // list of columns to serve as node attributes
  TStrV NodeAttrs;


	// Store a group indices column in GroupMapping
	void StoreGroupCol(TStr GroupColName, const THash<TInt,TIntV>& grouping);
	// Group/hash by a single column with integer values. Returns hash table with grouping.
	void GroupByIntCol(TStr GroupBy, THash<TInt,TIntV>& grouping, const TIntV* IndexSet);
	// Group/hash by a single column with float values. Returns hash table with grouping.
	void GroupByFltCol(TStr GroupBy, THash<TFlt,TIntV>& grouping, const TIntV* IndexSet);
	// Group/hash by a single column with string values. Returns hash table with grouping.
	void GroupByStrCol(TStr GroupBy, THash<TStr,TIntV>& grouping, const TIntV* IndexSet);
	// Performs grouping according to the values of columns GroupBy[i] where 
	// i >= GroupByStartIdx; Considers only tuples whose indices are in IndexSet
	// Adds the groups to hash table "grouping". Does not write to "GroupMapping"
	void GroupAux(const TStrV& GroupBy, TInt GroupByStartIdx, THash<TInt,TIntV>& grouping, const TIntV* IndexSet);

 
  // remove all rows / tuples whose index appear in the SORTED vector "remove"
  void RemoveSortedRows(const TIntV& remove);
  // the following three functions are called only from RemoveSortedRows
  void RemoveSortedRowsIntCols(const TIntV& remove);
  void RemoveSortedRowsFltCols(const TIntV& remove);
  void RemoveSortedRowsStrCols(const TIntV& remove);
  // remove all rows / tuples whose index does not appear in the SORTED vector "keep"
  void KeepSortedRows(const TIntV& keep);
  // the following three functions are called only from KeepSortedRows
  void KeepSortedRowsIntCols(const TIntV& keep);
  void KeepSortedRowsFltCols(const TIntV& keep);
  void KeepSortedRowsStrCols(const TIntV& keep);

public:
	// Nikhil + Jason
	TTable();
	TTable(TSIn& SIn);
	TTable(const TTable& Table);
	static TTable* Load(TSIn& SIn);
	void Save(TSOut& SOut);
	//PNEAGraph ToGraph();

	TStr GetSrcCol() const { return SrcCol; }
	TStr GetDstCol() const { return DstCol; }
	TStrV GetNodeIntAttrs() const;
	TStrV GetEdgeIntAttrs() const;
	TStrV GetNodeFltAttrs() const;
	TStrV GetEdgeFltAttrs() const;
	TStrV GetNodeStrAttrs() const;
	TStrV GetEdgeStrAttrs() const;
	TYPE GetColType(TStr ColName) { return ColTypeMap.GetDat(ColName).Val1; };


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
	// void Dist(TStr Col1, const TTable& Table, TStr Col2, TStr DistColName, const TMetric& Metric, TFlt threshold);
};
