#ifndef TABLE_H
#define TABLE_H
#include "../../snap-1.11/snap-core/Snap.h"
#include "Predicate.h"
#include "TMetric.h"

/* 
Questions:
1. for loading / saving tables, should we use I/O streams (i.e. TSIn, TSOut) or files ?
2. GroupCol/CountCol/OrderCol don't have to be stored in actual columns
*/
class TTable{
protected:
	TInt NumRows;
	typedef enum{Int, Flt, Str} TYPE;
	TVec<TIntV> IntCols;
	TVec<TFltV> FltCols;
	TVec<TStrV> StrCols;  // TODO: use TVec<TIntV> + string pool instead ?
	THash<TStr,TPair<TYPE,TInt>> ColTypeMap;
	// grouping statement name --> (group index --> rows that belong to that group)
	THash<TStr,THash<TInt,TIntV>> GroupMapping;
	TStr WorkingCol;

public:
	// Nikhil + Jason
	TTable();
	TTable(TSIn& SIn);
	TTable(const TTable& Table);
	static TTable* Load(TSIn& SIn);
	void Save(TSOut& SOut);
	//PNEAGraph ToGraph();

	TStr GetSrcCol();
	TStr GetDstCol();
	TStrV GetNodeIntAttrs();
	TStrV GetEdgeIntAttrs();
	TStrV GetNodeFltAttrs();
	TStrV GetEdgeFltAttrs();
	TStrV GetNodeStrAttrs();
	TStrV GetEdgeStrAttrs();
	TYPE GetColType(TStr ColName);

	// Yonathan
	// change working column ; not sure if this should be a part of the public interface
	void ChangeWorkingCol(TStr column);
	// rename / add a label to a column
	void AddLabel(TStr column, TStr newLabel);

	// Yonathan
	// Remove rows with duplicate values in column "col"
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
	// lexicographic order). record order in OrderCol; Implementation: logical indexing
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
};

#endif //TABLE_H