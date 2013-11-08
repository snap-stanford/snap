#ifndef TABLE_H
#define TABLE_H
#include "predicate.h"
#include "tmetric.h"
//#include "snap.h"

class TTable;
typedef TPt<TTable> PTable;

/*
This class serves as a wrapper for all data that needs to be shared by
several tables in an execution context
*/
class TTableContext{
protected:
  TStrHash<TInt, TBigStrPool> StringVals;
  friend class TTable;
public:
  TTableContext() {}
  TTableContext(TSIn& SIn): StringVals(SIn) {}
  void Save(TSOut& SOut) { StringVals.Save(SOut);}
};

/* possible policies for aggregating node attributes */
typedef enum {aaMin, aaMax, aaFirst, aaLast, aaAvg, aaMean} TAttrAggr;
/* possible operations on columns */
typedef enum {OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD} OPS;

/* a table schema is a vector of pairs <attribute name, attribute type> */
typedef TVec<TPair<TStr, TAttrType> > Schema; 

/************* Iterator classes ************/
// An iterator class to iterate over all currently existing rows
// Iteration over the rows should be done using only this iterator
class TRowIterator{
  TInt CurrRowIdx;
  const TTable* Table; 
public:
  TRowIterator(): CurrRowIdx(0), Table(NULL){}
  TRowIterator(TInt RowIdx, const TTable* TablePtr): CurrRowIdx(RowIdx), Table(TablePtr){}
  TRowIterator(const TRowIterator& RowI): CurrRowIdx(RowI.CurrRowIdx), Table(RowI.Table){}
  TRowIterator& operator++(int);
  TRowIterator& Next(); // For Python compatibility
  bool operator < (const TRowIterator& RowI) const;
  bool operator == (const TRowIterator& RowI) const;
  TInt GetRowIdx() const;
  // we do not check column type in the iterator
  TInt GetIntAttr(TInt ColIdx) const;
  TFlt GetFltAttr(TInt ColIdx) const;
  TStr GetStrAttr(TInt ColIdx) const;
  TInt GetIntAttr(const TStr& Col) const;
  TFlt GetFltAttr(const TStr& Col) const;
  TStr GetStrAttr(const TStr& Col) const;  
  TInt GetStrMap(const TStr& Col) const;
};

/* an iterator that also allows logical row removal while iterating */
class TRowIteratorWithRemove{
  TInt CurrRowIdx;
  TTable* Table;
  TBool Start;
public:
  TRowIteratorWithRemove(): CurrRowIdx(0), Table(NULL), Start(true){}
  TRowIteratorWithRemove(TInt RowIdx, TTable* TablePtr);
  TRowIteratorWithRemove(TInt RowIdx, TTable* TablePtr, TBool IsStart): CurrRowIdx(RowIdx), Table(TablePtr), Start(IsStart){}
  TRowIteratorWithRemove(const TRowIteratorWithRemove& RowI): CurrRowIdx(RowI.CurrRowIdx), Table(RowI.Table), Start(RowI.Start){}
  TRowIteratorWithRemove& operator++(int);
  TRowIteratorWithRemove& Next(); // For Python compatibility
  bool operator < (const TRowIteratorWithRemove& RowI) const;
  bool operator == (const TRowIteratorWithRemove& RowI) const;
  TInt GetRowIdx() const;
  TInt GetNextRowIdx() const;
  // we do not check column type in the iterator
  TInt GetNextIntAttr(TInt ColIdx) const;
  TFlt GetNextFltAttr(TInt ColIdx) const;
  TStr GetNextStrAttr(TInt ColIdx) const;   
  TInt GetNextIntAttr(const TStr& Col) const;
  TFlt GetNextFltAttr(const TStr& Col) const;
  TStr GetNextStrAttr(const TStr& Col) const;   
  TBool IsFirst() const;
  void RemoveNext();
};

/* 
TTable is a class representing an in-memory relational table with columnar data storage
*/
class TTable{
/******** Various typedefs / constants ***********/
protected:
  // special values for Next column
  static const TInt Last;
  static const TInt Invalid;

/*************** TTable object fields ********************/
public:
  // Table name
  TStr Name;
protected:
  // Execution Context
  TTableContext& Context;
  // string pool for string values (part of execution context)
  // TStrHash<TInt, TBigStrPool>& StrColVals; 
  // use Context.StringVals to access the global string pool

  // Table Schema
  Schema S;
  // Reference Counter for Garbage Collection
  TCRef CRef;

  // number of rows in the table (valid and invalid)
  TInt NumRows;
  // number of valid rows in the table (i.e. rows that were not logically removed)
  TInt NumValidRows;
  // physical index of first valid row
  TInt FirstValidRow;
  // physical index of last valid row
  TInt LastValidRow;
  // A vactor describing the logical order of the rows: Next[i] is the successor of row i
  // Table iterators follow the order dictated by Next
  TIntV Next; 

  // The actual columns - divided by types
	TVec<TIntV> IntCols;
	TVec<TFltV> FltCols;

  // string columns are implemented using a string pool to fight memory fragmentation
  // The value of string column c in row r is Context.StringVals.GetKey(StrColMaps[c][r])
	TVec<TIntV> StrColMaps; 
 
  // column name --> <column type, column index among columns of the same type>
	THash<TStr,TPair<TAttrType,TInt> > ColTypeMap;

  // A map to store Group statement results:
	// grouping statement name --> (group index --> rows that belong to that group)
  // Note that these mappings are invalid after we remove rows
  // Unless we use explicit Ids
	THash<TStr,THash<TInt,TIntV> > GroupMapping;

  // Fields to be used when constructing a graph
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
  // list of attributes pairs with values common to source and destination
  // and their common name. for instance: <T_1.age,T_2.age, age>
  //- T_1.age is a src node attribute, T_2.age is a dst node attribute.
  // However, since all nodes refer to the same universe of entities (users)
  // we just do one assignment of age per node
  // This list should be very small...
  TVec<TStrTr> CommonNodeAttrs;

  TVec<TIntV> RowIdBuckets;

public:
  /***** value getters - getValue(column name, physical row Idx) *****/
  // no type checking. assuming ColName actually refers to the right type.
  TInt GetIntVal(const TStr& ColName, const TInt& RowIdx){ return IntCols[ColTypeMap.GetDat(ColName).Val2][RowIdx];}
  TFlt GetFltVal(const TStr& ColName, const TInt& RowIdx){ return FltCols[ColTypeMap.GetDat(ColName).Val2][RowIdx];}
  TStr GetStrVal(const TStr& ColName, const TInt& RowIdx) const{ return GetStrVal(ColTypeMap.GetDat(ColName).Val2, RowIdx);}

  /***** schema getter *****/
  Schema GetSchema() { return S; }

/***** Utility functions *****/
protected:
  /* Utility functions for columns */
  void AddIntCol(const TStr& ColName);
  void AddFltCol(const TStr& ColName);

/***** Utility functions for handling string values *****/
  TStr GetStrVal(TInt ColIdx, TInt RowIdx) const{ return TStr(Context.StringVals.GetKey(StrColMaps[ColIdx][RowIdx]));}
  void AddStrVal(const TInt ColIdx, const TStr& Val);
  void AddStrVal(const TStr& Col, const TStr& Val);

/***** Utility functions for handling Schema *****/
  TStr GetSchemaColName(TInt Idx) const{ return S[Idx].Val1;}
  TAttrType GetSchemaColType(TInt Idx) const{ return S[Idx].Val2;}
  void AddSchemaCol(const TStr& ColName, TAttrType ColType) { S.Add(TPair<TStr,TAttrType>(ColName, ColType));}
  TInt GetColIdx(const TStr& ColName) const{ return ColTypeMap.IsKey(ColName) ? ColTypeMap.GetDat(ColName).Val2 : TInt(-1);}  // column index among columns of the same type
  TBool IsAttr(const TStr& Attr);

/***** Utility functions for building graph from TTable *****/
  // add names of columns to be used as graph attributes
  void AddGraphAttribute(const TStr& Attr, TBool IsEdge, TBool IsSrc, TBool IsDst);
  void AddGraphAttributeV(TStrV& Attrs, TBool IsEdge, TBool IsSrc, TBool IsDst);
  // Checks if given node id is seen earlier; if not, adds it to graph and hashmap
  void CheckAndAddIntNode(PNEANet Graph, THashSet<TInt>& NodeVals, TInt NodeId);
  TInt CheckAndAddFltNode(PNEANet Graph, THash<TFlt, TInt>& NodeVals, TFlt FNodeVal);
  // Adds attributes of edge corresponding to RowId to the Graph
  void AddEdgeAttributes(PNEANet& Graph, int RowId);
  // Takes as parameters, and updates, maps NodeXAttrs: Node Id --> (attribute name --> Vector of attribute values)
  void AddNodeAttributes(TInt NId, TStrV NodeAttrV, TInt RowId, THash<TInt, TStrIntVH>& NodeIntAttrs, 
    THash<TInt, TStrFltVH>& NodeFltAttrs, THash<TInt, TStrStrVH>& NodeStrAttrs);
  // Makes a single pass over the rows in the given row id set, and creates nodes, edges, assigns node and edge attributes
  PNEANet BuildGraph(const TIntV& RowIds, TAttrAggr AggrPolicy);
  // Returns sets of row ids, partitioned on the value of the column SplitColId, 
  // according to the range specified by JumpSize and WindowSize.
  // Called by ToGraphSequence.
  void GetRowIdBuckets(int SplitColId, TInt JumpSize, TInt WindowSize, TInt StartVal, TInt EndVal);

  // aggregate vector into a single scalar value according to a policy;
  // used for choosing an attribute value for a node when this node appears in
  // several records and has conflicting attribute values
  template <class T> 
  T AggregateVector(TVec<T>& V, TAttrAggr Policy){
    switch(Policy){
      case aaMin:{
        T Res = V[0];
        for(TInt i = 1; i < V.Len(); i++){
          if(V[i] < Res){ Res = V[i];}
        }
        return Res;
      }
      case aaMax:{
        T Res = V[0];
        for(TInt i = 1; i < V.Len(); i++){
          if(V[i] > Res){ Res = V[i];}
        }
        return Res;
      }
      case aaFirst:{
        return V[0];
      }
      case aaLast:{
        return V[V.Len()-1];
      }
      case aaAvg:{
        T Res = V[0];
        for(TInt i = 1; i < V.Len(); i++){
          Res = Res + V[i];
        }
        //Res = Res / V.Len(); // TODO: Handle Str case separately?
        return Res;
      }
      case aaMean:{
        V.Sort();
        return V[V.Len()/2];
      }
    }
    // added to remove a compiler warning
    T ShouldNotComeHere;
    return ShouldNotComeHere;
  }

  /***** Grouping Utility functions *************/
	// Group/hash by a single column with integer values. Returns hash table with grouping.
  // IndexSet tells what rows to consider (vector of physical row ids). It is used only if All == true.
  // Note that the IndexSet option is currently not used anywhere...
	void GroupByIntCol(const TStr& GroupBy, THash<TInt,TIntV>& grouping, const TIntV& IndexSet, TBool All) const; 
	// Group/hash by a single column with float values. Returns hash table with grouping.
	void GroupByFltCol(const TStr& GroupBy, THash<TFlt,TIntV>& grouping, const TIntV& IndexSet, TBool All) const;
	// Group/hash by a single column with string values. Returns hash table with grouping.
	void GroupByStrCol(const TStr& GroupBy, THash<TStr,TIntV>& grouping, const TIntV& IndexSet, TBool All) const;

  /* 
  The main logic behind general grouping. Takes the following parameters:
  GroupBy - a vector of column names by which to group
  Grouping - a grouping map GroupId --> list of rows (physical ids) that belong to that group
  UniqueVec - a vector of the first valid row that belongs to each group
  Ordered - a flag to indicate whether to treat grouping keys as ordered or unordered.
  KeepUnique - a flag to indicate whether to maintain UniqueVec
  KeepGrouping - a flag to indicate whether to maintain Grouping
  */
  void GroupAux(const TStrV& GroupBy, THash<TInt,TIntV>& Grouping, TIntV& UniqueVec, TBool Ordered = true, TBool KeepUnique = false, TBool KeepGrouping = true);
  // Store grouping in GroupMapping and add a column with group indices
	void StoreGroupCol(const TStr& GroupColName, const THash<TInt,TIntV>& grouping);
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

  /***** Utility functions for sorting by columns *****/
  // returns positive value if R1 is bigger, negative value if R2 is bigger, and 0 if they are equal (strcmp semantics)
  TInt CompareRows(TInt R1, TInt R2, const TStr& CompareBy, TBool Asc = true);
  TInt CompareRows(TInt R1, TInt R2, const TStrV& CompareBy, TBool Asc = true); 
  TInt GetPivot(TIntV& V, TInt StartIdx, TInt EndIdx, const TStrV& SortBy, TBool Asc);
  TInt Partition(TIntV& V, TInt StartIdx, TInt EndIdx, const TStrV& SortBy, TBool Asc);
  void ISort(TIntV& V, TInt StartIdx, TInt EndIdx, const TStrV& SortBy, TBool Asc = true);
  void QSort(TIntV& V, TInt StartIdx, TInt EndIdx, const TStrV& SortBy, TBool Asc = true);

  bool IsRowValid(TInt RowIdx) const{ return Next[RowIdx] != Invalid;}
  TInt GetLastValidRowIdx();

/***** Utility functions for removing rows (not through iterator) *****/
  void RemoveFirstRow();
  void RemoveRow(TInt RowIdx);
  void RemoveRows(const TIntV& RemoveV);
  // remove all rows that are not mentioned in the SORTED vector KeepV
  void KeepSortedRows(const TIntV& KeepV);

/***** Utility functions for Join *****/
  // Initialize an empty table for the join of this table with the given table
  PTable InitializeJointTable(const TTable& Table);
  // add joint row T1[RowIdx1]<=>T2[RowIdx2]
  void AddJointRow(const TTable& T1, const TTable& T2, TInt RowIdx1, TInt RowIdx2);

public:
/***** Constructors *****/
	TTable(); 
  TTable(TTableContext& Context);
  TTable(const TStr& TableName, const Schema& S, TTableContext& Context);
  TTable(TSIn& SIn, TTableContext& Context);

  // constructors to build table out of a hash table
  TTable(const TStr& TableName, const THash<TInt,TInt>& H, const TStr& Col1, const TStr& Col2, TTableContext& Context);
  TTable(const TStr& TableName, const THash<TInt,TFlt>& H, const TStr& Col1, const TStr& Col2, TTableContext& Context);
  //TTable(const TStr& TableName, const THash<TInt,TStr>& H, const TStr& Col1, const TStr& Col2, TTableContext& Context);
  
  // copy constructor
  TTable(const TTable& Table): Name(Table.Name), Context(Table.Context), S(Table.S),
    NumRows(Table.NumRows), NumValidRows(Table.NumValidRows), FirstValidRow(Table.FirstValidRow),
    Next(Table.Next), IntCols(Table.IntCols), FltCols(Table.FltCols),
    StrColMaps(Table.StrColMaps), ColTypeMap(Table.ColTypeMap), 
    GroupMapping(Table.GroupMapping),
    SrcCol(Table.SrcCol), DstCol(Table.DstCol),
    EdgeAttrV(Table.EdgeAttrV), SrcNodeAttrV(Table.SrcNodeAttrV),
    DstNodeAttrV(Table.DstNodeAttrV), CommonNodeAttrs(Table.CommonNodeAttrs){} 

  static PTable New(){ return new TTable();}
  static PTable New(TTableContext& Context){ return new TTable(Context);}
  static PTable New(const TStr& TableName, const Schema& S, TTableContext& Context){ return new TTable(TableName, S, Context);}
  static PTable New(const TStr& TableName, const THash<TInt,TInt>& H, const TStr& Col1, const TStr& Col2, TTableContext& Context, TBool IsStrKeys = false){
    return new TTable(TableName, H, Col1, Col2, Context, IsStrKeys);
  }
  static PTable New(const TStr& TableName, const THash<TInt,TFlt>& H, const TStr& Col1, const TStr& Col2, TTableContext& Context, TBool IsStrKeys = false){
    return new TTable(TableName, H, Col1, Col2, Context, IsStrKeys);
  }
  static PTable New(const PTable Table){ return new TTable(*Table);}
  static PTable New(const PTable Table, const TStr& TableName){ PTable T = New(Table); T->Name = TableName; return T;}

/***** Save / Load functions *****/
  // Load table from spread sheet (TSV, CSV, etc)
  static PTable LoadSS(const TStr& TableName, const Schema& S, const TStr& InFNm, TTableContext& Context, const char& Separator = '\t', TBool HasTitleLine = false);
  // Load table from spread sheet - but only load the columns specified by RelevantCols
  static PTable LoadSS(const TStr& TableName, const Schema& S, const TStr& InFNm, TTableContext& Context, const TIntV& RelevantCols, const char& Separator = '\t', TBool HasTitleLine = false);
  // Load table from spread sheet - compact prototype (tab-separated input file, has title line, anonymous table 
  static PTable LoadSS(const Schema& S, const TStr& InFnm, TTableContext& Context){
    return LoadSS(TStr(), S, InFnm, Context, '\t', true);
  }
  // Save table schema + content into a TSV file
  void SaveSS(const TStr& OutFNm);
  // Load table from binary. The TTableContext must be provided separately as it shared among multiple TTables and should be saved in a separate binary.
  static PTable Load(TSIn& SIn, TTableContext& Context){ return new TTable(SIn, Context);} 
  // Save table schema + content into binary. Note that TTableContext must be saved in a separate binary (as it is shared among multiple TTables).
	void Save(TSOut& SOut);

/***** Graph handling *****/
  // Create a graph out of the FINAL table
  PNEANet ToGraph(TAttrAggr AggrPolicy);
  // Create a sequence of graphs based on values of column SplitAttr
  TVec<PNEANet> ToGraphSequence(TStr SplitAttr, TAttrAggr AggrPolicy, 
    TInt WindowSize, TInt JumpSize, TInt StartVal, TInt EndVal);

  /* Getters and Setters of data required for building a graph out of the table */
	TStr GetSrcCol() const { return SrcCol; }
  void SetSrcCol(const TStr& Src) {
    if(!ColTypeMap.IsKey(Src)){TExcept::Throw(Src + ": no such column");}
    SrcCol = Src;
  }
	TStr GetDstCol() const { return DstCol; }
  void SetDstCol(const TStr& Dst) {
    if(!ColTypeMap.IsKey(Dst)){TExcept::Throw(Dst + ": no such column");}
    DstCol = Dst;
  }
  /* Specify table attributes (columns) as graph attributes */
  void AddEdgeAttr(const TStr& Attr){AddGraphAttribute(Attr, true, false, false);}
  void AddEdgeAttr(TStrV& Attrs){AddGraphAttributeV(Attrs, true, false, false);}
  void AddSrcNodeAttr(const TStr& Attr){AddGraphAttribute(Attr, false, true, false);}
  void AddSrcNodeAttr(TStrV& Attrs){AddGraphAttributeV(Attrs, false, true, false);}
  void AddDstNodeAttr(const TStr& Attr){AddGraphAttribute(Attr, false, false, true);}
  void AddDstNodeAttr(TStrV& Attrs){AddGraphAttributeV(Attrs, false, false, true);}
  // handling the common case where source and destination both belong to the same "universe" of entities
  void AddNodeAttr(const TStr& Attr){AddSrcNodeAttr(Attr); AddDstNodeAttr(Attr);}
  void AddNodeAttr(TStrV& Attrs){AddSrcNodeAttr(Attrs); AddDstNodeAttr(Attrs);}
  void SetCommonNodeAttrs(const TStr& SrcAttr, const TStr& DstAttr, const TStr& CommonAttrName){ 
    CommonNodeAttrs.Add(TStrTr(SrcAttr, DstAttr, CommonAttrName));
  }
  /* getters for attribute name vectors */
	TStrV GetSrcNodeIntAttrV() const;
  TStrV GetDstNodeIntAttrV() const;
	TStrV GetEdgeIntAttrV() const;
	TStrV GetSrcNodeFltAttrV() const;
  TStrV GetDstNodeFltAttrV() const;
	TStrV GetEdgeFltAttrV() const;
	TStrV GetSrcNodeStrAttrV() const;
  TStrV GetDstNodeStrAttrV() const;
	TStrV GetEdgeStrAttrV() const;

  /* Extract node and edge TTables from PNEANet */
  static PTable GetNodeTable(const PNEANet& Network, const TStr& TableName, TTableContext& Context);
  static PTable GetEdgeTable(const PNEANet& Network, const TStr& TableName, TTableContext& Context);

  /* Extract node and edge property TTables from THash */
  static PTable GetFltNodePropertyTable(const PNEANet& Network, const TStr& TableName, const TIntFltH& Property, const TStr& NodeAttrName, const TAttrType& NodeAttrType, const TStr& PropertyAttrName, TTableContext& Context);

/***** Basic Getters *****/
	TAttrType GetColType(const TStr& ColName) const{ return ColTypeMap.GetDat(ColName).Val1; };
  TInt GetNumRows() const { return NumRows;}
  TInt GetNumValidRows() const { return NumValidRows;}

	
/***** Iterators *****/
  TRowIterator BegRI() const { return TRowIterator(FirstValidRow, this);}
  TRowIterator EndRI() const { return TRowIterator(TTable::Last, this);}
  TRowIteratorWithRemove BegRIWR(){ return TRowIteratorWithRemove(FirstValidRow, this);}
  TRowIteratorWithRemove EndRIWR(){ return TRowIteratorWithRemove(TTable::Last, this);}

/***** Table Operations *****/
	// rename / add a label to a column
	void AddLabel(const TStr& Column, const TStr& NewLabel);
  void Rename(const TStr& Column, const TStr& NewLabel);

	// Remove rows with duplicate values in given columns
  void Unique(const TStr& Col);
	void Unique(const TStrV& Cols, TBool Ordered = true);
  void UniqueExistingGroup(const TStr& GroupStmt);

	/* 
  Select. Has two modes of operation:
  1. If Remove == true then (logically) remove the rows for which the predicate doesn't hold
  2. If Remove == false then add the physical indices of the rows for which the predicate holds to the vactor SelectedRows
  */
	void Select(TPredicate& Predicate, TIntV& SelectedRows, TBool Remove = true);
  void Select(TPredicate& Predicate){
    TIntV SelectedRows;
    Select(Predicate, SelectedRows, true);
  }
  // select atomic - optimized cases of select with predicate of an atomic form: compare attribute to attribute or compare attribute to a constant
  void SelectAtomic(const TStr& Col1, const TStr& Col2, TPredComp Cmp, TIntV& SelectedRows, TBool Remove = true);
  void SelectAtomic(const TStr& Col1, const TStr& Col2, TPredComp Cmp){
    TIntV SelectedRows;
    SelectAtomic(Col1, Col2, Cmp, SelectedRows, true);
  }
  void SelectAtomicIntConst(const TStr& Col1, const TInt& Val2, TPredComp Cmp, TIntV& SelectedRows, TBool Remove = true);
  void SelectAtomicIntConst(const TStr& Col1, const TInt& Val2, TPredComp Cmp){
    TIntV SelectedRows;
    SelectAtomicIntConst(Col1, Val2, Cmp, SelectedRows, true);
  }
  void SelectAtomicStrConst(const TStr& Col1, const TStr& Val2, TPredComp Cmp, TIntV& SelectedRows, TBool Remove = true);
  void SelectAtomicStrConst(const TStr& Col1, const TStr& Val2, TPredComp Cmp){
    TIntV SelectedRows;
    SelectAtomicStrConst(Col1, Val2, Cmp, SelectedRows, true);
  }
  void SelectAtomicFltConst(const TStr& Col1, const TFlt& Val2, TPredComp Cmp, TIntV& SelectedRows, TBool Remove = true);
  void SelectAtomicFltConst(const TStr& Col1, const TFlt& Val2, TPredComp Cmp){
    TIntV SelectedRows;
    SelectAtomicFltConst(Col1, Val2, Cmp, SelectedRows, true);
  }
	
	// group by the values of the columns specified in "GroupBy" vector 
	// group indices are stored in GroupCol; Implementation: use GroupMapping hash
	void Group(const TStr& GroupColName, const TStrV& GroupBy, TBool Ordered = true);
	// count - count the number of appearences of the different elements of col
	// record results in column CountCol
	void Count(const TStr& CountColName, const TStr& Col);
	// order the rows according to the values in columns of OrderBy (in descending
	// lexicographic order). 
	void Order(const TStrV& OrderBy, const TStr& OrderColName = "", TBool ResetRankByMSC = false, TBool Asc = true);

	// perform equi-join with given columns - i.e. keep tuple pairs where 
	// this->Col1 == Table->Col2; Implementation: Hash-Join - build a hash out of the smaller table
	// hash the larger table and check for collisions
	PTable Join(const TStr& Col1, const TTable& Table, const TStr& Col2);
  PTable Join(const TStr& Col1, const PTable& Table, const TStr& Col2){ return Join(Col1, *Table, Col2); };
  PTable SelfJoin(const TStr& Col){return Join(Col, *this, Col);}

	// compute distances between elements in this->Col1 and Table->Col2 according
	// to given metric. Store the distances in DistCol, but keep only rows where
	// distance <= threshold
	//void Dist(const TStr& Col1, const TTable& Table, const TStr Col2, const TStr& DistColName, const TMetric& Metric, TFlt threshold);

  // Release memory of deleted rows, and defrag
  // also updates meta-data as row indices have changed
  // need some liveness analysis of columns
  void Defrag();
  
  // Add all the rows of the input table (which ,ust have the same schema as current table) - allows duplicate rows (not a union)
  void AddTable(const TTable& T);
  
  void AddRow(const TRowIterator& RI);
  void GetCollidingRows(const TTable& T, THashSet<TInt>& Collisions) const;
  PTable Union(const TTable& Table, const TStr& TableName);
  PTable Intersection(const TTable& Table, const TStr& TableName);
  PTable Minus(const TTable& Table, const TStr& TableName);
  PTable Union(const PTable& Table, const TStr& TableName){ return Union(*Table, TableName); };
  PTable Intersection(const PTable& Table, const TStr& TableName){ return Intersection(*Table, TableName); };
  PTable Minus(const PTable& Table, const TStr& TableName){ return Minus(*Table, TableName); };
  PTable Project(const TStrV& ProjectCols, const TStr& TableName);
  void ProjectInPlace(const TStrV& ProjectCols);
  
  /* Column-wise arithmetic operations */

  /*
   * Performs Attr1 OP Attr2 and stores it in Attr1
   * If ResAttr != "", result is stored in a new column ResAttr
   */
  void ColGenericOp(const TStr& Attr1, const TStr& Attr2, const TStr& ResAttr, OPS op);
  void ColAdd(const TStr& Attr1, const TStr& Attr2, const TStr& ResultAttrName="");
  void ColSub(const TStr& Attr1, const TStr& Attr2, const TStr& ResultAttrName="");
  void ColMul(const TStr& Attr1, const TStr& Attr2, const TStr& ResultAttrName="");
  void ColDiv(const TStr& Attr1, const TStr& Attr2, const TStr& ResultAttrName="");
  void ColMod(const TStr& Attr1, const TStr& Attr2, const TStr& ResultAttrName="");

  /* Performs Attr1 OP Attr2 and stores it in Attr1 or Attr2
   * This is done depending on the flag AddToFirstTable
   * If ResAttr != "", result is stored in a new column ResAttr
   */
  void ColGenericOp(const TStr& Attr1, TTable& Table, const TStr& Attr2, const TStr& ResAttr, 
    OPS op, TBool AddToFirstTable);
  void ColAdd(const TStr& Attr1, TTable& Table, const TStr& Attr2, const TStr& ResAttr="",
    TBool AddToFirstTable=true);
  void ColSub(const TStr& Attr1, TTable& Table, const TStr& Attr2, const TStr& ResAttr="",
    TBool AddToFirstTable=true);
  void ColMul(const TStr& Attr1, TTable& Table, const TStr& Attr2, const TStr& ResAttr="",
    TBool AddToFirstTable=true);
  void ColDiv(const TStr& Attr1, TTable& Table, const TStr& Attr2, const TStr& ResAttr="",
    TBool AddToFirstTable=true);
  void ColMod(const TStr& Attr1, TTable& Table, const TStr& Attr2, const TStr& ResAttr="",
    TBool AddToFirstTable=true);

  /* Performs Attr1 OP Num and stores it in Attr1
   * If ResAttr != "", result is stored in a new column ResAttr
   */
  void ColGenericOp(const TStr& Attr1, const TFlt& Num, const TStr& ResAttr, OPS op, const TBool floatCast);
  void ColAdd(const TStr& Attr1, const TFlt& Num, const TStr& ResultAttrName="", const TBool floatCast=false);
  void ColSub(const TStr& Attr1, const TFlt& Num, const TStr& ResultAttrName="", const TBool floatCast=false);
  void ColMul(const TStr& Attr1, const TFlt& Num, const TStr& ResultAttrName="", const TBool floatCast=false);
  void ColDiv(const TStr& Attr1, const TFlt& Num, const TStr& ResultAttrName="", const TBool floatCast=false);
  void ColMod(const TStr& Attr1, const TFlt& Num, const TStr& ResultAttrName="", const TBool floatCast=false);

  // add a column of explicit integer identifiers to the rows
  void AddIdColumn(const TStr& IdColName);

  // creates a table T' where the rows are joint rows (T[r1],T[r2]) such that 
  // r2 is one of the successive rows to r1 when this table is ordered by OrderCol,
  // and both r1 and r2 have the same value of GroupBy column
  PTable IsNextK(const TStr& OrderCol, TInt K, const TStr& GroupBy, const TStr& RankColName = "");

  // debug print sizes of various fields of table
  void PrintSize();

  friend class TPt<TTable>;
  friend class TRowIterator;
  friend class TRowIteratorWithRemove;
};

typedef TPair<TStr,TAttrType> TStrTypPr;
#endif //TABLE_H

