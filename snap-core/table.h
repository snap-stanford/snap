#ifndef TABLE_H
#define TABLE_H
#include "predicate.h"
#include "tmetric.h"
//#include "snap.h"

class TTable;
typedef TPt<TTable> PTable;

typedef TPair<TIntV, TFltV> TGroupKey;

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
typedef enum {aaMin, aaMax, aaFirst, aaLast, aaMean, aaMedian, aaSum, aaCount} TAttrAggr;
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
  TInt GetStrMap(TInt ColIdx) const;
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

  // name of column associated with permanent row id
  TStr IdColName;

  // hash table mapping permanent row ids to physical ids
  THash<TInt, TInt> RowIdMap;

  // group mapping data structures
  THash<TStr, TPair<TStrV, TBool> > GroupStmtNames;
  THash<TPair<TStrV, TBool>, THash<TInt, TGroupKey> >GroupIDMapping;
  THash<TPair<TStrV, TBool>, THash<TGroupKey, TIntV> >GroupMapping;

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
  // The following members are used when a graph sequence is created using iterators
  TInt CurrBucket;
  TAttrAggr AggrPolicy;

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
  // adds a label attribute with positive labels on selected rows and negative labels on the rest
  void ClassifyAux(const TIntV& SelectedRows, const TStr& LabelName, const TInt& PositiveLabel = 1, const TInt& NegativeLabel=  0);

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
  // Initialize the RowIdBuckets vector which will be used for the graph sequence creation.
  void InitRowIdBuckets(int NumBuckets);
  // Fills RowIdBuckets with sets of row ids, partitioned on the value of the column SplitAttr, 
  // according to the windows specified by JumpSize and WindowSize.
  // Called by ToGraphSequence and ToGraphSequenceIterator.
  void FillBucketsByWindow(TStr SplitAttr, TInt JumpSize, TInt WindowSize, TInt StartVal, TInt EndVal);
  // Fills RowIdBuckets with sets of row ids, partitioned on the value of the column SplitAttr, 
  // according to the intervals specified by SplitIntervals.
  // Called by ToVarGraphSequence and ToVarGraphSequenceIterator.
  void FillBucketsByInterval(TStr SplitAttr, TIntPrV SplitIntervals);
  // Returns a sequence of graphs, each constructed from the set of row ids corresponding to
  // a particular bucket in RowIdBuckets.
  TVec<PNEANet> GetGraphsFromSequence(TAttrAggr AggrPolicy);
  // Returns the first graph of the sequence corresponding to the sets of row ids in RowIdBuckets.
  // This is used by the ToGraph*Iterator functions.
  PNEANet GetFirstGraphFromSequence(TAttrAggr AggrPolicy);
  // Returns the next graph in sequence corresponding to RowIdBuckets.
  // This is used to iterate over the graph sequence by constructing one graph at a time.
  // Called by NextGraphIterator().
  PNEANet GetNextGraphFromSequence();

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
      case aaSum:{
        T Res = V[0];
        for(TInt i = 1; i < V.Len(); i++){
          Res = Res + V[i];
        }
        return Res;
      }
      case aaMean:{
        T Res = V[0];
        for(TInt i = 1; i < V.Len(); i++){
          Res = Res + V[i];
        }
        //Res = Res / V.Len(); // TODO: Handle Str case separately?
        return Res;
      }
      case aaMedian:{
        V.Sort();
        return V[V.Len()/2];
      }
      case aaCount:{
        // NOTE: Code should never reach here
        // I had to put this here to avoid a compiler warning
        // Is there a better way to do this?
        return V[0];
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
  inline TInt CompareRows(TInt R1, TInt R2, const TAttrType& CompareByType, const TInt& CompareByIndex, TBool Asc = true);
  inline TInt CompareRows(TInt R1, TInt R2, const TVec<TAttrType>& CompareByTypes, const TIntV& CompareByIndices, TBool Asc = true); 
  TInt GetPivot(TIntV& V, TInt StartIdx, TInt EndIdx, const TVec<TAttrType>& SortByTypes, const TIntV& SortByIndices, TBool Asc);
  TInt Partition(TIntV& V, TInt StartIdx, TInt EndIdx, const TVec<TAttrType>& SortByTypes, const TIntV& SortByIndices, TBool Asc);
  void ISort(TIntV& V, TInt StartIdx, TInt EndIdx, const TVec<TAttrType>& SortByTypes, const TIntV& SortByIndices, TBool Asc = true);
  void QSort(TIntV& V, TInt StartIdx, TInt EndIdx, const TVec<TAttrType>& SortByTypes, const TIntV& SortByIndices, TBool Asc = true);

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
  TTable(const TStr& TableName, const THash<TInt,TInt>& H, const TStr& Col1, const TStr& Col2, TTableContext& Context, const TBool IsStrKeys = false);
  TTable(const TStr& TableName, const THash<TInt,TFlt>& H, const TStr& Col1, const TStr& Col2, TTableContext& Context, const TBool IsStrKeys = false);
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
  static PTable New(const TStr& TableName, const THash<TInt,TInt>& H, const TStr& Col1, const TStr& Col2, TTableContext& Context, const TBool IsStrKeys = false){
    return new TTable(TableName, H, Col1, Col2, Context, IsStrKeys);
  }
  static PTable New(const TStr& TableName, const THash<TInt,TFlt>& H, const TStr& Col1, const TStr& Col2, TTableContext& Context, const TBool IsStrKeys = false){
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
  
  // Create a sequence of graphs based on values of column SplitAttr and windows
  // specified by JumpSize and WindowSize.
  TVec<PNEANet> ToGraphSequence(TStr SplitAttr, TAttrAggr AggrPolicy, 
    TInt WindowSize, TInt JumpSize, TInt StartVal, TInt EndVal);
  // Create a sequence of graphs based on values of column SplitAttr and intervals
  // specified by SplitIntervals.
  TVec<PNEANet> ToVarGraphSequence(TStr SplitAttr, TAttrAggr AggrPolicy, TIntPrV SplitIntervals);
  // Create a sequence of graphs based on grouping specified by GroupAttr
  TVec<PNEANet> ToGraphPerGroup(TStr GroupAttr, TAttrAggr AggrPolicy);

  // Similar to above functions, these functions create the graphs one at a time, to allow
  // efficient use of memory. A call to one of these functions must be followed by subsequent calls
  // to NextGraphIterator().
  PNEANet ToGraphSequenceIterator(TStr SplitAttr, TAttrAggr AggrPolicy, 
    TInt WindowSize, TInt JumpSize, TInt StartVal, TInt EndVal);
  PNEANet ToVarGraphSequenceIterator(TStr SplitAttr, TAttrAggr AggrPolicy, TIntPrV SplitIntervals);
  PNEANet ToGraphPerGroupIterator(TStr GroupAttr, TAttrAggr AggrPolicy);
  // Calls to this must be preceded by a call to one of the above ToGraph*Iterator functions.
  PNEANet NextGraphIterator();

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

  THash<TInt, TInt> GetRowIdMap() const { return RowIdMap;}
	
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
  void Classify(TPredicate& Predicate, const TStr& LabelName, const TInt& PositiveLabel = 1, const TInt& NegativeLabel = 0);

  // select atomic - optimized cases of select with predicate of an atomic form: compare attribute to attribute or compare attribute to a constant
  void SelectAtomic(const TStr& Col1, const TStr& Col2, TPredComp Cmp, TIntV& SelectedRows, TBool Remove = true);
  void SelectAtomic(const TStr& Col1, const TStr& Col2, TPredComp Cmp){
    TIntV SelectedRows;
    SelectAtomic(Col1, Col2, Cmp, SelectedRows, true);
  }
  void ClassifyAtomic(const TStr& Col1, const TStr& Col2, TPredComp Cmp, const TStr& LabelName, const TInt& PositiveLabel = 1, const TInt& NegativeLabel = 0);

  void SelectAtomicIntConst(const TStr& Col1, const TInt& Val2, TPredComp Cmp, TIntV& SelectedRows, TBool Remove = true);
  void SelectAtomicIntConst(const TStr& Col1, const TInt& Val2, TPredComp Cmp){
    TIntV SelectedRows;
    SelectAtomicIntConst(Col1, Val2, Cmp, SelectedRows, true);
  }
  void ClassifyAtomicIntConst(const TStr& Col1, const TInt& Val2, TPredComp Cmp, const TStr& LabelName, const TInt& PositiveLabel = 1, const TInt& NegativeLabel = 0);

  void SelectAtomicStrConst(const TStr& Col1, const TStr& Val2, TPredComp Cmp, TIntV& SelectedRows, TBool Remove = true);
  void SelectAtomicStrConst(const TStr& Col1, const TStr& Val2, TPredComp Cmp){
    TIntV SelectedRows;
    SelectAtomicStrConst(Col1, Val2, Cmp, SelectedRows, true);
  }
  void ClassifyAtomicStrConst(const TStr& Col1, const TStr& Val2, TPredComp Cmp, const TStr& LabelName, const TInt& PositiveLabel = 1, const TInt& NegativeLabel = 0);

  void SelectAtomicFltConst(const TStr& Col1, const TFlt& Val2, TPredComp Cmp, TIntV& SelectedRows, TBool Remove = true);
  void SelectAtomicFltConst(const TStr& Col1, const TFlt& Val2, TPredComp Cmp){
    TIntV SelectedRows;
    SelectAtomicFltConst(Col1, Val2, Cmp, SelectedRows, true);
  }
  void ClassifyAtomicFltConst(const TStr& Col1, const TFlt& Val2, TPredComp Cmp, const TStr& LabelName, const TInt& PositiveLabel = 1, const TInt& NegativeLabel = 0);

  // store column for a group, physical row ids have to be passed
  void StoreGroupCol(const TStr& GroupColName, const TVec<TPair<TInt, TInt> >& GroupAndRowIds);

  // if KeepUnique is true, UniqueVec will be modified to contain a row from each group
  // if KeepUnique is false, then normal grouping is done and a new column is added depending on 
  // whether GroupColName is empty
  void GroupAux(const TStrV& GroupBy, THash<TGroupKey, TPair<TInt, TIntV> >& Grouping, TBool Ordered,
    const TStr& GroupColName, TBool KeepUnique, TIntV& UniqueVec);

  // group - specify columns to group by, name of column in new table, whether to treat columns as ordered
  // if name of column is an empty string, no column is created
  void Group(const TStrV& GroupBy, const TStr& GroupColName, TBool Ordered = true);
  
  // count - count the number of appearences of the different elements of col
  // record results in column CountCol
  void Count(const TStr& CountColName, const TStr& Col);
  // order the rows according to the values in columns of OrderBy (in descending
  // lexicographic order). 
  void Order(const TStrV& OrderBy, const TStr& OrderColName = "", TBool ResetRankByMSC = false, TBool Asc = true);
  
  // aggregate values of ValAttr after grouping with respect to GroupByAttrs
  // result stored as new attribute ResAttr
  void Aggregate(const TStrV& GroupByAttrs, TAttrAggr AggOp, const TStr& ValAttr,
    const TStr& ResAttr, TBool Ordered = true);

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

  // add explicit row ids, initialise hash set mapping ids to physical rows
  void InitIds();

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

