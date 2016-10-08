#ifndef TABLE_H
#define TABLE_H

/// Boolean operators for selection predicates
typedef enum {NOT, AND, OR, NOP} TPredOp; 
/// Comparison operators for selection predicates
typedef enum {LT = 0, LTE, EQ, NEQ, GTE, GT, SUBSTR, SUPERSTR} TPredComp; 

class TAtomicPredicate;
class TPredicateNode;
class TPredicate;

//#//////////////////////////////////////////////
/// Atomic predicate - encapsulates comparison operations
class TAtomicPredicate {
  private:
    TAttrType Type; ///< Type of the predicate variables
    TBool IsConst; ///< Flag if this atomic node represents a constant value
    TPredComp Compare; ///< Comparison op represented by this node
    TStr Lvar; ///< Left variable of the comparison op
    TStr Rvar; ///< Right variable of the comparison op
    TInt IntConst; ///< Int const value if this object is an integer constant
    TFlt FltConst; ///< Flt const value if this object is a float constant
    TStr StrConst; ///< Str const value if this object is a string constant
  // OP RS: 2014/03/25, NonAtom does not work with Snap.py
	//protected:
		//static const TAtomicPredicate NonAtom;
  public:
    /// Default constructor
    TAtomicPredicate() : Type(atInt), IsConst(true), 
      Compare(EQ), Lvar(""), Rvar(""), 
      IntConst(0), FltConst(0), StrConst("") {}
    //TAtomicPredicate() : Type(NonAtom.Type), IsConst(NonAtom.IsConst), 
    //  Compare(NonAtom.Compare), Lvar(NonAtom.Lvar), Rvar(NonAtom.Rvar), 
    //  IntConst(NonAtom.IntConst), FltConst(NonAtom.FltConst), StrConst(NonAtom.StrConst) {}
    /// Construct predicate from given comparison op, variables and constants
    TAtomicPredicate(TAttrType Typ, TBool IsCnst, TPredComp Cmp, TStr L, TStr R, 
    TInt ICnst, TFlt FCnst, TStr SCnst) : Type(Typ), IsConst(IsCnst), 
      Compare(Cmp), Lvar(L), Rvar(R), IntConst(ICnst), FltConst(FCnst),
      StrConst(SCnst) {}
    /// Compact prototype for constructing non-const atomic predicate
    TAtomicPredicate(TAttrType Typ, TBool IsCnst, TPredComp Cmp, TStr L, TStr R) :
      Type(Typ), IsConst(IsCnst), Compare(Cmp), Lvar(L), Rvar(R), IntConst(0), 
      FltConst(0), StrConst("") {}
    friend class TPredicate;
		friend class TPredicateNode;
};

//#//////////////////////////////////////////////
/// Predicate node - represents a binary predicate operation on two predicate nodes
class TPredicateNode {
	public:
		TPredOp Op; ///< Logical op represented by this node
		TBool Result; ///< Result of evaulating the predicate rooted at this node
		TAtomicPredicate Atom; ///< Atomic predicate at this node
		TPredicateNode* Parent; ///< Parent node of this node
		TPredicateNode* Left; ///< Left child of this node
		TPredicateNode* Right; ///< Right child of this node
		/// Default constructor
		TPredicateNode(): Op(NOP), Result(false), Atom(), Parent(NULL), Left(NULL), 
			Right(NULL) {}
		/// Constructor for atomic predicate node (leaf)
		TPredicateNode(const TAtomicPredicate& A): Op(NOP), Result(false), Atom(A), 
			Parent(NULL), Left(NULL), Right(NULL) {}
		/// Constructor for logical operation predicate node (internal node)
		TPredicateNode(TPredOp Opr): Op(Opr), Result(false), Atom(), Parent(NULL), 
			Left(NULL), Right(NULL) {}
		/// Copy constructor
		TPredicateNode(const TPredicateNode& P): Op(P.Op), Result(P.Result), Atom(P.Atom), 
			Parent(P.Parent), Left(P.Left), Right(P.Right) {}
		/// Add left child to this node
		void AddLeftChild(TPredicateNode* Child) { Left = Child; Child->Parent = this; }
		/// Add right child to this node
		void AddRightChild(TPredicateNode* Child) { Right = Child; Child->Parent = this; }
		/// Get variables in the predicate tree rooted at this node
		void GetVariables(TStrV& Variables);
		friend class TPredicate;
};

//#//////////////////////////////////////////////
/// Predicate - encapsulates comparison operations
class TPredicate {
	protected:
		THash<TStr, TInt> IntVars; ///< Int variables in the current predicate tree
		THash<TStr, TFlt> FltVars; ///< Float variables in the current predicate tree
		THash<TStr, TStr> StrVars; ///< String variables in the current predicate tree
		TPredicateNode* Root; ///< Rood node of the current predicate tree
	public:
		/// Default constructor
		TPredicate() : IntVars(), FltVars(), StrVars() {}
		/// Construct predicate with given root node \c R
		TPredicate(TPredicateNode* R) : IntVars(), FltVars(), StrVars(), Root(R) {}
		/// Copy constructor
		TPredicate(const TPredicate& Pred) : IntVars(Pred.IntVars), FltVars(Pred.FltVars), StrVars(Pred.StrVars), Root(Pred.Root) {}
		/// Get variables in current predicate
		void GetVariables(TStrV& Variables);
		/// Set int variable value in the predicate or all the children that use it
		void SetIntVal(TStr VarName, TInt VarVal) { IntVars.AddDat(VarName, VarVal); }
		/// Set flt variable value in the predicate or all the children that use it
		void SetFltVal(TStr VarName, TFlt VarVal) { FltVars.AddDat(VarName, VarVal); }
		/// Set str variable value in the predicate or all the children that use it
		void SetStrVal(TStr VarName, TStr VarVal) { StrVars.AddDat(VarName, VarVal); }
		/// Return the result of evaluating current predicate
		TBool Eval();
		/// Evaluate the give atomic predicate
		TBool EvalAtomicPredicate(const TAtomicPredicate& Atom);

		/// Compare atomic values Val1 and Val2 using predicate Cmp
		template <class T>
		static TBool EvalAtom(T Val1, T Val2, TPredComp Cmp) {
			switch (Cmp) {
				case LT: return Val1 < Val2;
				case LTE: return Val1 <= Val2;
				case EQ: return Val1 == Val2;
				case NEQ: return Val1 != Val2;
				case GTE: return Val1 >= Val2;
				case GT: return Val1 > Val2;
				default: return false;
			}
		};

		/// Compare atomic string values Val1 and Val2 using predicate Cmp
		static TBool EvalStrAtom(const TStr& Val1, const TStr& Val2, TPredComp Cmp) {
			switch (Cmp) {
				case LT: return Val1 < Val2;
				case LTE: return Val1 <= Val2;
				case EQ: return Val1 == Val2;
				case NEQ: return Val1 != Val2;
				case GTE: return Val1 >= Val2;
				case GT: return Val1 > Val2;
				case SUBSTR: return Val2.IsStrIn(Val1);
				case SUPERSTR: return Val1.IsStrIn(Val2);
				default: return false;
			}
		}
};

//#//////////////////////////////////////////////
/// Table class
class TTable;
class TTableContext;
typedef TPt<TTable> PTable;

/// Represents grouping key with IntV for integer and string attributes and FltV for float attributes.
typedef TPair<TIntV, TFltV> TGroupKey;

/// Distance metrics for similarity joins
// Haversine distance is used to calculate distance between two points on a sphere based on latitude and longitude
typedef enum {L1Norm, L2Norm, Jaccard, Haversine} TSimType;

#if 0
// TMetric and TEuclideanMetric are currently not used, kept for future use
//#//////////////////////////////////////////////
/// Metric class: base class for distance metrics
class TMetric {
protected:
  TStr MetricName; ///< Name of the metric defined by this class
public:
  TMetric(TStr Name) : MetricName(Name) {}
  /// Get the name of this metric
  TStr GetName();
  /// Virtual base function for defining metric on floats
  virtual TFlt NumDist(TFlt,TFlt) { return -1; }
  /// Virtual base function for defining metric on strings
  virtual TFlt StrDist(TStr,TStr) { return -1; }
};

//#//////////////////////////////////////////////
/// Euclidean metric class: compute distance between two floats
class TEuclideanMetric: public TMetric {
public:
  TEuclideanMetric(TStr Name) : TMetric(Name) {}
  /// Calculate the euclidean distance of two floats
  TFlt NumDist(TFlt x1,TFlt x2) { return fabs(x1-x2); }
};
#endif

//TODO: move to separate file (map.h / file with PR and HITS)
namespace TSnap {

  /// Gets sequence of PageRank tables from given \c GraphSeq into \c TableSeq.
  template <class PGraph>
  void MapPageRank(const TVec<PGraph>& GraphSeq, TVec<PTable>& TableSeq,
      TTableContext* Context, const double& C, const double& Eps, const int& MaxIter);

  /// Gets sequence of Hits tables from given \c GraphSeq into \c TableSeq.
  template <class PGraph>
  void MapHits(const TVec<PGraph>& GraphSeq, TVec<PTable>& TableSeq,
    TTableContext* Context, const int& MaxIter);
}

//#//////////////////////////////////////////////
/// Execution context
class TTableContext {
protected:
  TStrHash<TInt, TBigStrPool> StringVals; ///< StringPool - stores string data values and maps them to integers.
  friend class TTable;
public:
  /// Default constructor.
  TTableContext() {}
  /// Loads TTableContext in binary from \c SIn.
  TTableContext(TSIn& SIn): StringVals(SIn) {}
  /// Loads TTableContext in binary from \c SIn.
  void Load(TSIn& SIn) { StringVals.Load(SIn); }
  /// Saves TTableContext in binary to \c SOut.
  void Save(TSOut& SOut) { StringVals.Save(SOut); }
  /// Adds string \c Key to the context, returns its KeyId.
  TInt AddStr(const TStr& Key) {
    TInt KeyId = TInt(StringVals.AddKey(Key));
    return(KeyId);
  }
  /// Returns a string with \c KeyId.
  TStr GetStr(const TInt& KeyId) const {
    return StringVals.GetKey(KeyId);
  }
};

//#//////////////////////////////////////////////
/// Primitive class: Wrapper around primitive data types
class TPrimitive {
private:
  TInt IntVal;
  TFlt FltVal;
  TStr StrVal;
  TAttrType AttrType;

public:
  TPrimitive() : IntVal(-1), FltVal(-1), StrVal(""), AttrType(atInt) {}
  TPrimitive(const TInt& Val) : IntVal(Val), FltVal(-1), StrVal(""), AttrType(atInt) {}
  TPrimitive(const TFlt& Val) : IntVal(-1), FltVal(Val), StrVal(""), AttrType(atFlt) {}
  TPrimitive(const TStr& Val) : IntVal(-1), FltVal(-1), StrVal(Val.CStr()), AttrType(atStr) {}
  TPrimitive(const TPrimitive& Prim) : IntVal(Prim.IntVal), FltVal(Prim.FltVal),
    StrVal(Prim.StrVal.CStr()), AttrType(Prim.AttrType) {}

  TInt GetInt() const { return IntVal; }
  TFlt GetFlt() const { return FltVal; }
  TStr GetStr() const { return StrVal; }
  TAttrType GetType() const { return AttrType; }
};

//#//////////////////////////////////////////////
/// Table Row (Record)
class TTableRow {
protected:
  TIntV IntVals; ///< Values of the int columns for this row.
  TFltV FltVals; ///< Values of the flt columns for this row.
  TStrV StrVals; ///< Values of the str columns for this row.
public:
  /// Default constructor.
  TTableRow() {}
  /// Adds int attribute to this row.
  void AddInt(const TInt& Val) { IntVals.Add(Val); }
  /// Adds float attribute to this row.
  void AddFlt(const TFlt& Val) { FltVals.Add(Val); }
  /// Adds string attribute to this row.
  void AddStr(const TStr& Val) { StrVals.Add(Val); }
  /// Gets int attributes of this row.
  TIntV GetIntVals() const { return IntVals; }
  /// Gets float attributes of this row.
  TFltV GetFltVals() const { return FltVals; }
  /// Gets string attributes of this row.
  TStrV GetStrVals() const { return StrVals; }
};

/// Possible policies for aggregating node attributes.
typedef enum {aaMin, aaMax, aaFirst, aaLast, aaMean, aaMedian, aaSum, aaCount} TAttrAggr;
/// Possible column-wise arithmetic operations.
typedef enum {aoAdd, aoSub, aoMul, aoDiv, aoMod, aoMin, aoMax} TArithOp;

/// A table schema is a vector of pairs <attribute name, attribute type>.
typedef TVec<TPair<TStr, TAttrType> > Schema;

//#//////////////////////////////////////////////
/// A class representing a cached grouping statement identifier
class GroupStmt{
protected:
	TStrV GroupByAttrs;
	TBool Ordered;
	TBool UsePhysicalRowIds;
	TBool Valid;
public:
	GroupStmt(): GroupByAttrs(TStrV()), Ordered(true), UsePhysicalRowIds(true), Valid(true){}
	GroupStmt(const TStrV& Attrs): GroupByAttrs(Attrs), Ordered(true), UsePhysicalRowIds(true), Valid(true){}
	GroupStmt(const TStrV& Attrs, TBool ordered, TBool physical): GroupByAttrs(Attrs), Ordered(ordered), UsePhysicalRowIds(physical), Valid(true){}
	GroupStmt(const GroupStmt& stmt): GroupByAttrs(stmt.GroupByAttrs), Ordered(stmt.Ordered), UsePhysicalRowIds(stmt.UsePhysicalRowIds), Valid(stmt.Valid){}
	TBool UsePhysicalIds(){return UsePhysicalRowIds;}
	TBool operator ==(const GroupStmt& stmt) const{
		if(stmt.Ordered != Ordered || stmt.UsePhysicalRowIds != UsePhysicalRowIds){ return false;}
		if(stmt.GroupByAttrs.Len() != GroupByAttrs.Len()){ return false;}
		for(int i = 0; i < GroupByAttrs.Len(); i++){
			if(stmt.GroupByAttrs[i] != GroupByAttrs[i]){ return false;}
		}
		return true;
	}
	TBool IsValid(){ return Valid;}
	void Invalidate(){ Valid = false;}
	TBool IncludesAttr(const TStr& Attr){
		for(int i = 0; i < GroupByAttrs.Len(); i++){
			if(GroupByAttrs[i] == Attr){ return true;}
		}
		return false;
	}
	TSize GetMemUsed() const{
		TSize sz = 3 * sizeof(TBool);
		sz += GroupByAttrs.GetMemUsed();
		for(int i = 0; i < GroupByAttrs.Len(); i++){
			sz += GroupByAttrs[i].GetMemUsed();
		}
		return sz;
	}
	
	int GetPrimHashCd() const{
		int hc1 = GroupByAttrs.GetPrimHashCd();
		TTriple<TBool,TBool,TBool> flags;
		int hc2 = flags.GetPrimHashCd();
		return TPairHashImpl::GetHashCd(hc1, hc2);
	}
	
	int GetSecHashCd() const{
		int hc1 = GroupByAttrs.GetSecHashCd();
		TTriple<TBool,TBool,TBool> flags;
		int hc2 = flags.GetSecHashCd();
		return TPairHashImpl::GetHashCd(hc1, hc2);
	}
	
	void Print(){
		for(int i = 0; i < GroupByAttrs.Len(); i++){
			printf("%s ", GroupByAttrs[i].CStr());
		}
		printf("Ordered: %d, UsePhysicalRows: %d, Valid: %d\n", Ordered.Val, UsePhysicalRowIds.Val, Valid.Val);
	}
};

//#//////////////////////////////////////////////
/// Iterator class for TTable rows. ##Iterator
class TRowIterator{
  TInt CurrRowIdx; ///< Physical row index of current row pointed by iterator.
  const TTable* Table; ///< Reference to table containing this row.
public:
  /// Default constructor.
  TRowIterator(): CurrRowIdx(0), Table(NULL) {}
  /// Constructs iterator to row \c RowIds of \c TablePtr.
  TRowIterator(TInt RowIdx, const TTable* TablePtr): CurrRowIdx(RowIdx), Table(TablePtr) {}
  /// Copy constructor.
  TRowIterator(const TRowIterator& RowI): CurrRowIdx(RowI.CurrRowIdx), Table(RowI.Table) {}
  /// Increments the iterator.
  TRowIterator& operator++(int);
  /// Increments the iterator (For Python compatibility).
  TRowIterator& Next();
  /// Checks if this iterator points to a row that is before the one pointed by \c RowI.
  bool operator < (const TRowIterator& RowI) const;
  /// Checks if this iterator points to the same row pointed by \c RowI.
  bool operator == (const TRowIterator& RowI) const;
  /// Gets the id of the row pointed by this iterator.
  TInt GetRowIdx() const;
  /// Returns value of integer attribute specified by integer column index for current row.
  TInt GetIntAttr(TInt ColIdx) const;
  /// Returns value of floating point attribute specified by float column index for current row.
  TFlt GetFltAttr(TInt ColIdx) const;
  /// Returns value of string attribute specified by string column index for current row.
  TStr GetStrAttr(TInt ColIdx) const;
  /// Returns integer mapping of a string attribute value specified by string column index for current row.
  TInt GetStrMapById(TInt ColIdx) const;
  /// Returns value of integer attribute specified by attribute name for current row.
  TInt GetIntAttr(const TStr& Col) const;
  /// Returns value of float attribute specified by attribute name for current row.
  TFlt GetFltAttr(const TStr& Col) const;
  /// Returns value of string attribute specified by attribute name for current row.
  TStr GetStrAttr(const TStr& Col) const;
  /// Returns integer mapping of string attribute specified by attribute name for current row.
  TInt GetStrMapByName(const TStr& Col) const;
  /// Compares value in column \c ColIdx with given primitive \c Val.
  TBool CompareAtomicConst(TInt ColIdx, const TPrimitive& Val, TPredComp Cmp);
  /// Compares value in column \c ColIdx with given TStr \c Val.
  TBool CompareAtomicConstTStr(TInt ColIdx, const  TStr& Val, TPredComp Cmp);
};

//#//////////////////////////////////////////////
/// Iterator class for TTable rows, that allows logical row removal while iterating.
class TRowIteratorWithRemove {
  TInt CurrRowIdx; ///< Physical row index of current row pointer by iterator.
  TTable* Table; ///< Reference to table containing this row.
  TBool Start;	///< A flag indicating whether the current row in the first valid row of the table.
public:
  /// Default constructor.
  TRowIteratorWithRemove(): CurrRowIdx(0), Table(NULL), Start(true) {}
  /// Constructs iterator pointing to given row.
  TRowIteratorWithRemove(TInt RowIdx, TTable* TablePtr);
  /// Constructs iterator pointing to given row.
  TRowIteratorWithRemove(TInt RowIdx, TTable* TablePtr, TBool IsStart) : CurrRowIdx(RowIdx),
    Table(TablePtr), Start(IsStart) {}
  /// Copy constructor.
  TRowIteratorWithRemove(const TRowIteratorWithRemove& RowI) : CurrRowIdx(RowI.CurrRowIdx),
    Table(RowI.Table), Start(RowI.Start) {}
  /// Increments the iterator.
  TRowIteratorWithRemove& operator++(int);
  /// Increments the iterator (For Python compatibility).
  TRowIteratorWithRemove& Next();
  /// Checks if this iterator points to a row that is before the one pointed by \c RowI.
  bool operator < (const TRowIteratorWithRemove& RowI) const;
  /// Checks if this iterator points to the same row pointed by \c RowI.
  bool operator == (const TRowIteratorWithRemove& RowI) const;
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
  /// Returns value of integer attribute specified by attribute name for next row.
  TInt GetNextIntAttr(const TStr& Col) const;
  /// Returns value of float attribute specified by attribute name for next row.
  TFlt GetNextFltAttr(const TStr& Col) const;
  /// Returns value of string attribute specified by attribute name for next row.
  TStr GetNextStrAttr(const TStr& Col) const;
  /// Checks whether iterator points to first valid row of the table.
  TBool IsFirst() const;
  /// Removes next row.
  void RemoveNext();
  /// Compares value in column \c ColIdx with given primitive \c Val.
  TBool CompareAtomicConst(TInt ColIdx, const TPrimitive& Val, TPredComp Cmp);
};

//#//////////////////////////////////////////////
/// Iterator over a vector of tables.
class TTableIterator {
  TVec<PTable> PTableV; ///< Vector of TTables which are to be iterated over.
  TInt CurrTableIdx; ///< Index of the current table pointed to by this iterator.
public:
  /// Default constructor.
  TTableIterator(TVec<PTable>& PTableV): PTableV(PTableV), CurrTableIdx(0) {}
  /// Returns next table in the sequence and update iterator.
  PTable Next() { return PTableV[CurrTableIdx++]; }
  /// Checks if iterator has reached end of the sequence.
  bool HasNext() { return CurrTableIdx < PTableV.Len(); }
};

/// The name of the friend is not found by simple name lookup until a matching declaration is provided in that namespace scope (either before or after the class declaration granting friendship).
namespace TSnap {
	/// Converts table to a directed/undirected graph. Suitable for PUNGraph and PNGraph, but not for PNEANet where attributes are expected.
	template<class PGraph> PGraph ToGraph(PTable Table,
    const TStr& SrcCol, const TStr& DstCol, TAttrAggr AggrPolicy);
	/// Converts table to a network. Suitable for PNEANet - Requires node and edge attribute column names as vectors.
  template<class PGraph> PGraph ToNetwork(PTable Table,
    const TStr& SrcCol, const TStr& DstCol,
    TStrV& SrcAttrs, TStrV& DstAttrs, TStrV& EdgeAttrs,
    TAttrAggr AggrPolicy);
	/// Converts table to a network. Suitable for PNEANet - Assumes no node and edge attributes.
  template<class PGraph> PGraph ToNetwork(PTable Table,
    const TStr& SrcCol, const TStr& DstCol, TAttrAggr AggrPolicy);
  template<class PGraph> PGraph ToNetwork(PTable Table,
    const TStr& SrcCol, const TStr& DstCol,
    TStrV& EdgeAttrV,
    TAttrAggr AggrPolicy);
  template<class PGraph> PGraph ToNetwork(PTable Table,
    const TStr& SrcCol, const TStr& DstCol,
    TStrV& EdgeAttrV, PTable NodeTable, const TStr& NodeCol, TStrV& NodeAttrV,
    TAttrAggr AggrPolicy);
  int LoadCrossNet(TCrossNet& Graph, PTable Table,
    const TStr& SrcCol, const TStr& DstCol,
    TStrV& EdgeAttrV);
  int LoadMode(TModeNet& Graph, PTable Table, const TStr& NCol,
    TStrV& NodeAttrV);

#ifdef GCC_ATOMIC
  template<class PGraphMP> PGraphMP ToGraphMP(PTable Table,
    const TStr& SrcCol, const TStr& DstCol);
  template<class PGraphMP> PGraphMP ToGraphMP3(PTable Table,
    const TStr& SrcCol, const TStr& DstCol);
  template<class PGraphMP> PGraphMP ToNetworkMP(PTable Table, const TStr& SrcCol, const TStr& DstCol,
		  TStrV& SrcAttrs, TStrV& DstAttrs, TStrV& EdgeAttrs, TAttrAggr AggrPolicy);
  template<class PGraphMP> PGraphMP ToNetworkMP2(PTable Table, const TStr& SrcCol, const TStr& DstCol,
		  TStrV& SrcAttrs, TStrV& DstAttrs, TStrV& EdgeAttrs, TAttrAggr AggrPolicy);
  template<class PGraphMP> PGraphMP ToNetworkMP(PTable Table, const TStr& SrcCol, const TStr& DstCol,
		  TStrV& EdgeAttrV, TAttrAggr AggrPolicy);
  template<class PGraphMP> PGraphMP ToNetworkMP(PTable Table, const TStr& SrcCol, const TStr& DstCol,
		  TAttrAggr AggrPolicy);
  template<class PGraphMP> PGraphMP ToNetworkMP(PTable Table, const TStr& SrcCol, const TStr& DstCol,
		  TStrV& EdgeAttrV, PTable NodeTable, const TStr& NodeCol, TStrV& NodeAttrV, TAttrAggr AggrPolicy);
 

#endif // GCC_ATOMIC
}

//#//////////////////////////////////////////////
/// Table class: Relational table with columnar data storage
class TTable {
protected:
  static const TInt Last; ///< Special value for Next vector entry - last row in table.
  static const TInt Invalid; ///< Special value for Next vector entry - logically removed row.

  static TInt UseMP; ///< Global switch for choosing multi-threaded versions of TTable functions.
public:
  template<class PGraph> friend PGraph TSnap::ToGraph(PTable Table,
    const TStr& SrcCol, const TStr& DstCol, TAttrAggr AggrPolicy);
  template<class PGraph> friend PGraph TSnap::ToNetwork(PTable Table,
    const TStr& SrcCol, const TStr& DstCol,
    TStrV& SrcAttrs, TStrV& DstAttrs, TStrV& EdgeAttrs,
    TAttrAggr AggrPolicy);
  template<class PGraph> friend PGraph TSnap::ToNetwork(PTable Table,
    const TStr& SrcCol, const TStr& DstCol,
    TAttrAggr AggrPolicy);
  template<class PGraph> friend PGraph TSnap::ToNetwork(PTable Table,
    const TStr& SrcCol, const TStr& DstCol,
    TStrV& EdgeAttrV,
    TAttrAggr AggrPolicy);
  template<class PGraph> friend PGraph TSnap::ToNetwork(PTable Table,
    const TStr& SrcCol, const TStr& DstCol,
    TStrV& EdgeAttrV, PTable NodeTable, const TStr& NodeCol, TStrV& NodeAttrV,
    TAttrAggr AggrPolicy);
  friend int TSnap::LoadCrossNet(TCrossNet& Graph, PTable Table,
      const TStr& SrcCol, const TStr& DstCol,
      TStrV& EdgeAttrV);
  friend int TSnap::LoadMode(TModeNet& Graph, PTable Table,
      const TStr& NCol, TStrV& NodeAttrV); 

#ifdef GCC_ATOMIC
  template<class PGraphMP> friend PGraphMP TSnap::ToGraphMP(PTable Table, const TStr& SrcCol, const TStr& DstCol);
  template<class PGraphMP> friend PGraphMP TSnap::ToGraphMP3(PTable Table, const TStr& SrcCol, const TStr& DstCol);
  template<class PGraphMP> friend PGraphMP TSnap::ToNetworkMP(PTable Table, const TStr& SrcCol, const TStr& DstCol, TStrV& SrcAttrs, TStrV& DstAttrs, TStrV& EdgeAttrs, TAttrAggr AggrPolicy);
  template<class PGraphMP> friend PGraphMP TSnap::ToNetworkMP2(PTable Table, const TStr& SrcCol, const TStr& DstCol,  TStrV& SrcAttrs, TStrV& DstAttrs, TStrV& EdgeAttrs, TAttrAggr AggrPolicy);
  template<class PGraphMP> friend PGraphMP TSnap::ToNetworkMP(PTable Table, const TStr& SrcCol, const TStr& DstCol, TStrV& EdgeAttrV, TAttrAggr AggrPolicy);
  template<class PGraphMP> friend PGraphMP TSnap::ToNetworkMP(PTable Table, const TStr& SrcCol, const TStr& DstCol, TAttrAggr AggrPolicy);
  template<class PGraphMP> friend PGraphMP TSnap::ToNetworkMP(PTable Table, const TStr& SrcCol, const TStr& DstCol,
		  TStrV& EdgeAttrV, PTable NodeTable, const TStr& NodeCol, TStrV& NodeAttrV, TAttrAggr AggrPolicy);
 
#endif // GCC_ATOMIC

  static void SetMP(TInt Value) { UseMP = Value; }
  static TInt GetMP() { return UseMP; }

  /// Adds suffix to column name if it doesn't exist
  static TStr NormalizeColName(const TStr& ColName) {
    TStr Result = ColName;
    int RLen = Result.Len();
    if (RLen == 0) { return Result; }
    if (Result.GetCh(0) == '_') { return Result; }
    if (RLen >= 2  &&  Result.GetCh(RLen-2) == '-') { return Result; }
    return Result + "-1";
  }
  /// Adds suffix to column name if it doesn't exist
  static TStrV NormalizeColNameV(const TStrV& Cols) {
    TStrV NCols;
    for (TInt i = 0; i < Cols.Len(); i++) { NCols.Add(NormalizeColName(Cols[i])); }
    return NCols;
  }
protected:
  TTableContext* Context;  ///< Execution Context. ##TTable::Context
  Schema Sch; ///< Table Schema.
  TCRef CRef;
  TInt NumRows; ///< Number of rows in the table (valid and invalid).
  TInt NumValidRows; ///< Number of valid rows in the table (i.e. rows that were not logically removed).
  TInt FirstValidRow; ///< Physical index of first valid row.
  TInt LastValidRow; ///< Physical index of last valid row.
  TIntV Next; ///< A vector describing the logical order of the rows. ##TTable::Next
  TVec<TIntV> IntCols; ///< Data columns of integer attributes.
  TVec<TFltV> FltCols; ///< Data columns of floating point attributes.
  TVec<TIntV> StrColMaps; ///< Data columns of integer mappings of string attributes. ##TTable::StrColMaps
  THash<TStr,TPair<TAttrType,TInt> > ColTypeMap; /// A mapping from column name to column type and column index among columns of the same type.
  TStr IdColName; ///< Name of column associated with (optional) permanent row identifiers.
  TIntIntH RowIdMap; ///< Mapping of permanent row ids to physical id.

  THash<TStr, THash<TInt, TIntV> > IntColIndexes; ///< Indexes for Int Columns.
  THash<TStr, THash<TInt, TIntV> > StrMapColIndexes; ///< Indexes for String Columns.
  THash<TStr, THash<TFlt, TIntV> > FltColIndexes; ///< Indexes for Float  Columns.

  // Group mapping data structures.
  THash<TStr, GroupStmt > GroupStmtNames; ///< Maps user-given grouping statement names to their group-by attributes. ##TTable::GroupStmtNames
  THash<GroupStmt, THash<TInt, TGroupKey> >GroupIDMapping; ///< Maps grouping statements to their (group id --> group-by key) mapping. ##TTable::GroupIDMapping
  THash<GroupStmt, THash<TGroupKey, TIntV> >GroupMapping; ///< Maps grouping statements to their (group-by key --> group id) mapping. ##TTable::GroupMapping
  void InvalidatePhysicalGroupings(); // to be called when rows are added / physically removed
  void InvalidateAffectedGroupings(const TStr& Attr); // to be called when attributes are removed (projected) or values updated in-place

  // Fields to be used when constructing a graph.
  TStr SrcCol; ///< Column (attribute) to serve as src nodes when constructing the graph.
  TStr DstCol; ///< Column (attribute) to serve as dst nodes when constructing the graph.
  TStrV EdgeAttrV; ///< List of columns (attributes) to serve as edge attributes.
  TStrV SrcNodeAttrV; ///< List of columns (attributes) to serve as source node attributes.
  TStrV DstNodeAttrV; ///< List of columns (attributes) to serve as destination node attributes.
  TStrTrV CommonNodeAttrs; ///< List of attribute pairs with values common to source and destination and their common given name. ##TTable::CommonNodeAttrs
  TVec<TIntV> RowIdBuckets; ///< Partitioning of row ids into buckets corresponding to different graph objects when generating a sequence of graphs.
  TInt CurrBucket; ///< Current row id bucket - used when generating a sequence of graphs using an iterator.
  TAttrAggr AggrPolicy; ///< Aggregation policy used for solving conflicts between different values of an attribute of the same node.

  TInt IsNextDirty; ///< Flag to signify whether the rows are stored in logical sequence or reordered. Used for optimizing GetPartitionRanges.

/***** Utility functions *****/
public:
  /// Adds an integer column with name \c ColName.
  void AddIntCol(const TStr& ColName);
  /// Adds a float column with name \c ColName.
  void AddFltCol(const TStr& ColName);
  /// Adds a string column with name \c ColName.
  void AddStrCol(const TStr& ColName);
protected:
  /// Increments the next vector and set last, NumRows and NumValidRows.
  void IncrementNext();
 /// Adds a label attribute with positive labels on selected rows and negative labels on the rest.
  void ClassifyAux(const TIntV& SelectedRows, const TStr& LabelName,
   const TInt& PositiveLabel = 1, const TInt& NegativeLabel=  0);

/***** Utility functions for handling string values *****/
  /// Gets the Key of the Context StringVals pool. Used by ToGraph method in conv.cpp.
  const char* GetContextKey(TInt Val) const {
	  return Context->StringVals.GetKey(Val);
  }
  /// Gets the value in column with id \c ColIdx at row \c RowIdx.
  TStr GetStrVal(TInt ColIdx, TInt RowIdx) const {
    return TStr(Context->StringVals.GetKey(StrColMaps[ColIdx][RowIdx]));
  }
  /// Adds \c Val in column with id \c ColIdx.
  void AddStrVal(const TInt& ColIdx, const TStr& Val);
  /// Adds \c Val in column with name \c Col.
  void AddStrVal(const TStr& Col, const TStr& Val);

/***** Utility functions for handling Schema *****/
  /// Gets name of the id column of this table.
  TStr GetIdColName() const { return IdColName; }
  /// Gets name of the column with index \c Idx in the schema.
  TStr GetSchemaColName(TInt Idx) const { return Sch[Idx].Val1; }
  /// Gets type of the column with index \c Idx in the schema.
  TAttrType GetSchemaColType(TInt Idx) const { return Sch[Idx].Val2; }
  /// Adds column with name \c ColName and type \c ColType to the schema.
  void AddSchemaCol(const TStr& ColName, TAttrType ColType) {
    TStr NColName = NormalizeColName(ColName);
    Sch.Add(TPair<TStr,TAttrType>(NColName, ColType));
  }
  TBool IsColName(const TStr& ColName) const {
    TStr NColName = NormalizeColName(ColName);
    return ColTypeMap.IsKey(NColName);
  }
  /// Adds column with name \c ColName and type \c ColType to the ColTypeMap.
  void AddColType(const TStr& ColName, TPair<TAttrType,TInt> ColType) {
    TStr NColName = NormalizeColName(ColName);
    ColTypeMap.AddDat(NColName, ColType);
  }
  /// Adds column with name \c ColName and type \c ColType to the ColTypeMap.
  void AddColType(const TStr& ColName, TAttrType ColType, TInt Index) {
    TStr NColName = NormalizeColName(ColName);
    AddColType(NColName, TPair<TAttrType,TInt>(ColType, Index));
  }
  /// Adds column with name \c ColName and type \c ColType to the ColTypeMap.
  void DelColType(const TStr& ColName) {
    TStr NColName = NormalizeColName(ColName);
    ColTypeMap.DelKey(NColName);
  }
  /// Gets column type and index of \c ColName.
  TPair<TAttrType, TInt> GetColTypeMap(const TStr& ColName) const {
    TStr NColName = NormalizeColName(ColName);
    return ColTypeMap.GetDat(NColName);
  }
  /// Returns a re-numbered column name based on number of existing columns with conflicting names.
  TStr RenumberColName(const TStr& ColName) const;
  /// Removes suffix to column name if exists
  TStr DenormalizeColName(const TStr& ColName) const;
  /// Removes suffix to column names in the Schema
  Schema DenormalizeSchema() const;
  /// Checks if \c Attr is an attribute of this table schema.
  TBool IsAttr(const TStr& Attr);

/***** Utility functions for adding rows and tables to TTable *****/
  /// Adds all the rows of the input table. Allows duplicate rows (not a union).
  void AddTable(const TTable& T);
  /// Appends all rows of \c T to this table, and recalculate indices.
  void ConcatTable(const PTable& T) {AddTable(*T); Reindex(); }

  /// Adds row corresponding to \c RI.
  void AddRow(const TRowIterator& RI);
  /// Adds row with values corresponding to the given vectors by type.
  void AddRow(const TIntV& IntVals, const TFltV& FltVals, const TStrV& StrVals);

/***** Utility functions for building graph from TTable *****/
  /// Adds names of columns to be used as graph attributes.
  void AddGraphAttribute(const TStr& Attr, TBool IsEdge, TBool IsSrc, TBool IsDst);
  /// Adds vector of names of columns to be used as graph attributes.
  void AddGraphAttributeV(TStrV& Attrs, TBool IsEdge, TBool IsSrc, TBool IsDst);
  /// Checks if given \c NodeId is seen earlier; if not, add it to \c Graph and hashmap \c NodeVals.
  void CheckAndAddIntNode(PNEANet Graph, THashSet<TInt>& NodeVals, TInt NodeId);
  /// Checks if given \c NodeVal is seen earlier; if not, add it to \c Graph and hashmap \c NodeVals.
  template<class T> TInt CheckAndAddFltNode(T Graph, THash<TFlt, TInt>& NodeVals, TFlt FNodeVal);
  /// Adds attributes of edge corresponding to \c RowId to the \c Graph.
  void AddEdgeAttributes(PNEANet& Graph, int RowId);
  /// Takes as parameters, and updates, maps NodeXAttrs: Node Id --> (attribute name --> Vector of attribute values).
  void AddNodeAttributes(TInt NId, TStrV NodeAttrV, TInt RowId,
   THash<TInt, TStrIntVH>& NodeIntAttrs, THash<TInt, TStrFltVH>& NodeFltAttrs,
   THash<TInt, TStrStrVH>& NodeStrAttrs);
  /// Makes a single pass over the rows in the given row id set, and creates nodes, edges, assigns node and edge attributes.
  PNEANet BuildGraph(const TIntV& RowIds, TAttrAggr AggrPolicy);
  /// Initializes the RowIdBuckets vector which will be used for the graph sequence creation.
  void InitRowIdBuckets(int NumBuckets);
  /// Fills RowIdBuckets with sets of row ids. ##TTable::FillBucketsByWindow
  void FillBucketsByWindow(TStr SplitAttr, TInt JumpSize, TInt WindowSize,
   TInt StartVal, TInt EndVal);
  /// Fills RowIdBuckets with sets of row ids. ##TTable::FillBucketsByInterval
  void FillBucketsByInterval(TStr SplitAttr, TIntPrV SplitIntervals);
  /// Returns a sequence of graphs. ##TTable::GetGraphsFromSequence
  TVec<PNEANet> GetGraphsFromSequence(TAttrAggr AggrPolicy);
  /// Returns the first graph of the sequence. ##TTable::GetFirstGraphFromSequence
  PNEANet GetFirstGraphFromSequence(TAttrAggr AggrPolicy);
  /// Returns the next graph in sequence corresponding to RowIdBuckets. ##TTable::GetNextGraphFromSequence
  PNEANet GetNextGraphFromSequence();

  /// Aggregates vector into a single scalar value according to a policy. ##TTable::AggregateVector
  template <class T> T AggregateVector(TVec<T>& V, TAttrAggr Policy);

  /***** Grouping Utility functions *************/
  /// Checks if grouping key exists and matches given attr type.
  void GroupingSanityCheck(const TStr& GroupBy, const TAttrType& AttrType) const;
  /// Groups/hashes by a single column with integer values. ##TTable::GroupByIntCol
  template <class T> void GroupByIntCol(const TStr& GroupBy, T& Grouping, 
    const TIntV& IndexSet, TBool All, TBool UsePhysicalIds = true) const;
#ifdef GCC_ATOMIC
  public:	//Should be protected - this is for debug only
  /// Groups/hashes by a single column with integer values, using OpenMP multi-threading.
  void GroupByIntColMP(const TStr& GroupBy, THashMP<TInt, TIntV>& Grouping, TBool UsePhysicalIds = true) const;
#endif // GCC_ATOMIC
  protected:
  /// Groups/hashes by a single column with float values. Returns hash table with grouping.
  template <class T> void GroupByFltCol(const TStr& GroupBy, T& Grouping, 
    const TIntV& IndexSet, TBool All, TBool UsePhysicalIds = true) const;
  /// Groups/hashes by a single column with string values. Returns hash table with grouping.
  template <class T> void GroupByStrCol(const TStr& GroupBy, T& Grouping, 
    const TIntV& IndexSet, TBool All, TBool UsePhysicalIds = true) const;
  /// Template for utility function to update a grouping hash map.
  template <class T> void UpdateGrouping(THash<T,TIntV>& Grouping, T Key, TInt Val) const;
#ifdef GCC_ATOMIC
  /// Template for utility function to update a parallel grouping hash map.
  template <class T> void UpdateGrouping(THashMP<T,TIntV>& Grouping, T Key, TInt Val) const;
#endif // GCC_ATOMIC
  void PrintGrouping(const THash<TGroupKey, TIntV>& Grouping) const;

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

/***** Utility functions for removing rows (not through iterator) *****/
  /// Checks if \c RowIdx corresponds to a valid (i.e. not deleted) row.
  bool IsRowValid(TInt RowIdx) const{ return Next[RowIdx] != Invalid;}
  /// Gets the id of the last valid row of the table.
  TInt GetLastValidRowIdx();
  /// Removes first valid row of the table.
  void RemoveFirstRow();
  /// Removes row with id \c RowIdx.
  void RemoveRow(TInt RowIdx, TInt PrevRowIdx);
  /// Removes all rows that are not mentioned in the SORTED vector \c KeepV.
  void KeepSortedRows(const TIntV& KeepV);
  /// Sets the first valid row of the TTable.
  void SetFirstValidRow() {
    for (int i = 0; i < Next.Len(); i++) {
      if(Next[i] != TTable::Invalid) { FirstValidRow = i; return;}
    }
    TExcept::Throw("SetFirstValidRow: Table is empty");
  }

/***** Utility functions for Join *****/
  /// Initializes an empty table for the join of this table with the given table.
  PTable InitializeJointTable(const TTable& Table);
  /// Adds joint row T1[RowIdx1]<=>T2[RowIdx2].
  void AddJointRow(const TTable& T1, const TTable& T2, TInt RowIdx1, TInt RowIdx2);
/***** Utility functions for Threshold Join *****/
  void ThresholdJoinInputCorrectness(const TStr& KeyCol1, const TStr& JoinCol1, const TTable& Table, 
    const TStr& KeyCol2, const TStr& JoinCol2);
  void ThresholdJoinCountCollisions(const TTable& TB, const TTable& TS, 
    const TIntIntVH& T, TInt JoinColIdxB, TInt KeyColIdxB, TInt KeyColIdxS, 
    THash<TIntPr,TIntTr>& Counters, TBool ThisIsSmaller, TAttrType JoinColType, TAttrType KeyType);
  PTable ThresholdJoinOutputTable(const THash<TIntPr,TIntTr>& Counters, TInt Threshold, const TTable& Table);
  void ThresholdJoinCountPerJoinKeyCollisions(const TTable& TB, const TTable& TS, 
    const TIntIntVH& T, TInt JoinColIdxB, TInt KeyColIdxB, TInt KeyColIdxS, 
    THash<TIntTr,TIntTr>& Counters, TBool ThisIsSmaller, TAttrType JoinColType, TAttrType KeyType);
  PTable ThresholdJoinPerJoinKeyOutputTable(const THash<TIntTr,TIntTr>& Counters, TInt Threshold, const TTable& Table);

  /// Resizes the table to hold \c RowCount rows.
  void ResizeTable(int RowCount);
  /// Gets the start index to a chunk of empty rows of size \c NewRows.
  int GetEmptyRowsStart(int NewRows);
  /// Adds rows from \c Table that correspond to ids in \c RowIDs.
  void AddSelectedRows(const TTable& Table, const TIntV& RowIDs);
  /// Adds \c NewRows rows from the given vectors for each column type.
  void AddNRows(int NewRows, const TVec<TIntV>& IntColsP, const TVec<TFltV>& FltColsP,
   const TVec<TIntV>& StrColMapsP);
#ifdef USE_OPENMP
  /// Adds rows from T1 and T2 to this table in a parallel manner. Used by Join.
  void AddNJointRowsMP(const TTable& T1, const TTable& T2, const TVec<TIntPrV>& JointRowIDSet);
#endif // USE_OPENMP
  /// Updates table state after adding one or more rows.
  void UpdateTableForNewRow();

#ifdef GCC_ATOMIC
  /// Parallelly loads data from input file at InFNm into NewTable. Only work when NewTable has no string columns.
  static void LoadSSPar(PTable& NewTable, const Schema& S, const TStr& InFNm, const TIntV& RelevantCols, const char& Separator, TBool HasTitleLine);
#endif // GCC_ATOMIC
  /// Sequentially loads data from input file at InFNm into NewTable
  static void LoadSSSeq(PTable& NewTable, const Schema& S, const TStr& InFNm, const TIntV& RelevantCols, const char& Separator, TBool HasTitleLine);

/***** Utility functions for Group *****/
  /// Helper function for grouping. ##TTable::GroupAux
  void GroupAux(const TStrV& GroupBy, THash<TGroupKey, TPair<TInt, TIntV> >& Grouping, 
   TBool Ordered, const TStr& GroupColName, TBool KeepUnique, TIntV& UniqueVec, TBool UsePhysicalIds = true);
#ifdef USE_OPENMP
  /// Parallel helper function for grouping. - we currently don't support such parallel grouping by complex keys
  //void GroupAuxMP(const TStrV& GroupBy, THashGenericMP<TGroupKey, TPair<TInt, TIntV> >& Grouping, 
  // TBool Ordered, const TStr& GroupColName, TBool KeepUnique, TIntV& UniqueVec, TBool UsePhysicalIds = false);
#endif // USE_OPENMP
  /// Stores column for a group. Physical row ids have to be passed.
  void StoreGroupCol(const TStr& GroupColName, const TVec<TPair<TInt, TInt> >& GroupAndRowIds);
  /// Register (cache) result of a grouping statement by a single group-by attribute
  /// T is a hash table mapping a key x to rows keyed by x  => DISABLED FOR NOW
  //template<class T> void RegisterGrouping(const T& Grouping, const TStr& GroupByCol, TBool UsePhysicalRows);

  /// Reinitializes row ids.
  void Reindex();
  /// Adds a column of explicit integer identifiers to the rows.
  void AddIdColumn(const TStr& IdColName);

  static TInt CompareKeyVal(const TInt& K1, const TInt& V1, const TInt& K2, const TInt& V2);
  static TInt CheckSortedKeyVal(TIntV& Key, TIntV& Val, TInt Start, TInt End);
  static void ISortKeyVal(TIntV& Key, TIntV& Val, TInt Start, TInt End);
  static TInt GetPivotKeyVal(TIntV& Key, TIntV& Val, TInt Start, TInt End);
  static TInt PartitionKeyVal(TIntV& Key, TIntV& Val, TInt Start, TInt End);
  static void QSortKeyVal(TIntV& Key, TIntV& Val, TInt Start, TInt End);

  /// Gets set of row ids of rows common with table \c T.
  void GetCollidingRows(const TTable& T, THashSet<TInt>& Collisions);

public:
/***** Constructors *****/
  TTable();
  TTable(TTableContext* Context);
  TTable(const Schema& S, TTableContext* Context);
  TTable(TSIn& SIn, TTableContext* Context);

  /// Constructor to build table out of a hash table of int->int.
  TTable(const THash<TInt,TInt>& H, const TStr& Col1, const TStr& Col2,
   TTableContext* Context, const TBool IsStrKeys = false);
  /// Constructor to build table out of a hash table of int->float.
  TTable(const THash<TInt,TFlt>& H, const TStr& Col1, const TStr& Col2,
   TTableContext* Context, const TBool IsStrKeys = false);
  // TTable(const TStr& TableName, const THash<TInt,TStr>& H, const TStr& Col1,
  //  const TStr& Col2, TTableContext* Context);

  /// Copy constructor.
  TTable(const TTable& Table): Context(Table.Context), Sch(Table.Sch),
    NumRows(Table.NumRows), NumValidRows(Table.NumValidRows), FirstValidRow(Table.FirstValidRow),
    LastValidRow(Table.LastValidRow), Next(Table.Next), IntCols(Table.IntCols),
    FltCols(Table.FltCols), StrColMaps(Table.StrColMaps), ColTypeMap(Table.ColTypeMap),
    IdColName(Table.IdColName), RowIdMap(Table.RowIdMap), GroupStmtNames(Table.GroupStmtNames),
    GroupIDMapping(Table.GroupIDMapping), GroupMapping(Table.GroupMapping),
    SrcCol(Table.SrcCol), DstCol(Table.DstCol),
    EdgeAttrV(Table.EdgeAttrV), SrcNodeAttrV(Table.SrcNodeAttrV),
    DstNodeAttrV(Table.DstNodeAttrV), CommonNodeAttrs(Table.CommonNodeAttrs),
    IsNextDirty(Table.IsNextDirty) {}

  TTable(const TTable& Table, const TIntV& RowIds);

  static PTable New() { return new TTable(); }
  static PTable New(TTableContext* Context) { return new TTable(Context); }
  static PTable New(const Schema& S, TTableContext* Context) {
    return new TTable(S, Context);
  }
  /// Returns pointer to a table constructed from given int->int hash.
  static PTable New(const THash<TInt,TInt>& H, const TStr& Col1,
   const TStr& Col2, TTableContext* Context, const TBool IsStrKeys = false) {
    return new TTable(H, Col1, Col2, Context, IsStrKeys);
  }
  /// Returns pointer to a table constructed from given int->float hash.
  static PTable New(const THash<TInt,TFlt>& H, const TStr& Col1,
   const TStr& Col2, TTableContext* Context, const TBool IsStrKeys = false) {
    return new TTable(H, Col1, Col2, Context, IsStrKeys);
  }
  /// Returns pointer to a new table created from given \c Table.
  static PTable New(const PTable Table) { return new TTable(*Table); }
  /// Returns pointer to a new table created from given \c Table, with name set to \c TableName.
  // static PTable New(const PTable Table, const TStr& TableName) {
  //   PTable T = New(Table); T->Name = TableName;
  //   return T;
  // }
  /// Automatically detects the Schema of a input file (data is assumed to be in tsv format)
  static void GetSchema(const TStr& InFNm, Schema& S, const char& Separator = '\t');
/***** Save / Load functions *****/
  /// Loads table from spread sheet (TSV, CSV, etc). Note: HasTitleLine = true is not supported. Please comment title lines instead
  static PTable LoadSS(const Schema& S, const TStr& InFNm, TTableContext* Context,
   const char& Separator = '\t', TBool HasTitleLine = false);
  /// Loads table from spread sheet - but only load the columns specified by RelevantCols. Note: HasTitleLine = true is not supported. Please comment title lines instead
  static PTable LoadSS(const Schema& S, const TStr& InFNm, TTableContext* Context,
   const TIntV& RelevantCols, const char& Separator = '\t', TBool HasTitleLine = false);
  /// Saves table schema and content to a TSV file.
  void SaveSS(const TStr& OutFNm);
  /// Saves table schema and content to a binary file.
  void SaveBin(const TStr& OutFNm);
  /// Loads table from a binary format. ##TTable::Load
  static PTable Load(TSIn& SIn, TTableContext* Context){ return new TTable(SIn, Context);}
  /// Saves table schema and content to a binary format. ##TTable::Save
  void Save(TSOut& SOut);
  /// Prints table contents to a text file.
  void Dump(FILE *OutF=stdout) const;

  /// Builds table from hash table of int->int.
  static PTable TableFromHashMap(const THash<TInt,TInt>& H, const TStr& Col1, const TStr& Col2,
   TTableContext* Context, const TBool IsStrKeys = false) {
    PTable T = New(H, Col1, Col2, Context, IsStrKeys);
    T->InitIds();
    return T;
  }
  /// Builds table from hash table of int->float.
  static PTable TableFromHashMap(const THash<TInt,TFlt>& H, const TStr& Col1, const TStr& Col2,
   TTableContext* Context, const TBool IsStrKeys = false) {
    PTable T = New(H, Col1, Col2, Context, IsStrKeys);
    T->InitIds();
    return T;
  }
  /// Adds row with values taken from given TTableRow.
  void AddRow(const TTableRow& Row) { AddRow(Row.GetIntVals(), Row.GetFltVals(), Row.GetStrVals()); };

  /// Returns the context.
  TTableContext* GetContext() {
    return Context;
  }
  /// Changes the current context. Moves all object items to the new context.
  TTableContext* ChangeContext(TTableContext* Context);

/***** Value Getters - getValue(column name, physical row Idx) *****/
  /// Gets index of column \c ColName among columns of the same type in the schema.
  TInt GetColIdx(const TStr& ColName) const {
    TStr NColName = NormalizeColName(ColName);
    return ColTypeMap.IsKey(NColName) ? ColTypeMap.GetDat(NColName).Val2 : TInt(-1);
  }

  // No type checking. Assuming ColName actually refers to the right type.
  /// Gets the value of integer attribute \c ColName at row \c RowIdx.
  TInt GetIntVal(const TStr& ColName, const TInt& RowIdx) {
    return IntCols[GetColIdx(ColName)][RowIdx];
  }
  /// Gets the value of float attribute \c ColName at row \c RowIdx.
  TFlt GetFltVal(const TStr& ColName, const TInt& RowIdx) {
    return FltCols[GetColIdx(ColName)][RowIdx];
  }
  /// Gets the value of string attribute \c ColName at row \c RowIdx.
  TStr GetStrVal(const TStr& ColName, const TInt& RowIdx) const {
    return GetStrVal(GetColIdx(ColName), RowIdx);
  }

  /// Gets the integer mapping of the string at column \c ColIdx at row \c RowIdx.
  TInt GetStrMapById(TInt ColIdx, TInt RowIdx) const {
    return StrColMaps[ColIdx][RowIdx];
  }

  /// Gets the integer mapping of the string at column \c ColName at row \c RowIdx.
  TInt GetStrMapByName(const TStr& ColName, TInt RowIdx) const {
    return StrColMaps[GetColIdx(ColName)][RowIdx];
  }

  /// Gets the value of the string attribute at column \c ColIdx at row \c RowIdx.
  TStr GetStrValById(TInt ColIdx, TInt RowIdx) const {
    return GetStrVal(ColIdx, RowIdx);
  }

  /// Gets the value of the string attribute at column \c ColName at row \c RowIdx.
  TStr GetStrValByName(const TStr& ColName, const TInt& RowIdx) const {
    return GetStrVal(ColName, RowIdx);
  }

  /// Gets the rows containing Val in int column \c ColName. ##TTable::GetIntRowIdxByVal
  TIntV GetIntRowIdxByVal(const TStr& ColName, const TInt& Val) const;
  /// Gets the rows containing int mapping Map in str column \c ColName. ##TTable::GetStrRowIdxByMap
  TIntV GetStrRowIdxByMap(const TStr& ColName, const TInt& Map) const;
  /// Gets the rows containing Val in flt column \c ColName. ##TTable::GetFltRowIdxByVal
  TIntV GetFltRowIdxByVal(const TStr& ColName, const TFlt& Val) const;

  /// Creates Index for Int Column \c ColName ##TTable::RequestIndexInt
  TInt RequestIndexInt(const TStr& ColName);
  /// Creates Index for Flt Column \c ColName. ##TTable::RequestIndexFlt
  TInt RequestIndexFlt(const TStr& ColName);
  /// Creates Index for Str Column \c ColName. ##TTable::RequestIndexStrMap
  TInt RequestIndexStrMap(const TStr& ColName);

  /// Gets the string with \c KeyId.
  TStr GetStr(const TInt& KeyId) const {
    return Context->StringVals.GetKey(KeyId);
  }

/***** Value Getters - getValue(col idx, row Idx) *****/
  // No type and bound checking
  /// Get the integer value at column \c ColIdx and row \c RowIdx
  TInt GetIntValAtRowIdx(const TInt& ColIdx, const TInt& RowIdx) {
    return IntCols[ColIdx][RowIdx];
  }
  /// Get the float value at column \c ColIdx and row \c RowIdx
  TFlt GetFltValAtRowIdx(const TInt& ColIdx, const TInt& RowIdx) {
    return FltCols[ColIdx][RowIdx];
  }

  /// Gets the schema of this table.
  Schema GetSchema() { return DenormalizeSchema(); }

/***** Graph handling *****/
  /// Creates a sequence of graphs based on values of column SplitAttr and windows specified by JumpSize and WindowSize.
  TVec<PNEANet> ToGraphSequence(TStr SplitAttr, TAttrAggr AggrPolicy,
    TInt WindowSize, TInt JumpSize, TInt StartVal = TInt::Mn, TInt EndVal = TInt::Mx);
  /// Creates a sequence of graphs based on values of column SplitAttr and intervals specified by SplitIntervals.
  TVec<PNEANet> ToVarGraphSequence(TStr SplitAttr, TAttrAggr AggrPolicy, TIntPrV SplitIntervals);
  /// Creates a sequence of graphs based on grouping specified by GroupAttr.
  TVec<PNEANet> ToGraphPerGroup(TStr GroupAttr, TAttrAggr AggrPolicy);

  /// Creates the graph sequence one at a time. ##TTable::ToGraphSequenceIterator
  PNEANet ToGraphSequenceIterator(TStr SplitAttr, TAttrAggr AggrPolicy,
    TInt WindowSize, TInt JumpSize, TInt StartVal = TInt::Mn, TInt EndVal = TInt::Mx);
  /// Creates the graph sequence one at a time. ##TTable::ToVarGraphSequenceIterator
  PNEANet ToVarGraphSequenceIterator(TStr SplitAttr, TAttrAggr AggrPolicy, TIntPrV SplitIntervals);
  /// Creates the graph sequence one at a time. ##TTable::ToGraphPerGroupIterator
  PNEANet ToGraphPerGroupIterator(TStr GroupAttr, TAttrAggr AggrPolicy);
  /// Calls to this must be preceded by a call to one of the above ToGraph*Iterator functions.
  PNEANet NextGraphIterator();
  /// Checks if the end of the graph sequence is reached.
  TBool IsLastGraphOfSequence();

  /// Gets the name of the column to be used as src nodes in the graph.
	TStr GetSrcCol() const { return SrcCol; }
  /// Sets the name of the column to be used as src nodes in the graph.
  void SetSrcCol(const TStr& Src) {
    if (!IsColName(Src)) { TExcept::Throw(Src + ": no such column"); }
    SrcCol = NormalizeColName(Src);
  }
  /// Gets the name of the column to be used as dst nodes in the graph.
	TStr GetDstCol() const { return DstCol; }
  /// Sets the name of the column to be used as dst nodes in the graph.
  void SetDstCol(const TStr& Dst) {
    if (!IsColName(Dst)) { TExcept::Throw(Dst + ": no such column"); }
    DstCol = NormalizeColName(Dst);
  }
  /// Adds column to be used as graph edge attribute.
  void AddEdgeAttr(const TStr& Attr) { AddGraphAttribute(Attr, true, false, false); }
  /// Adds columns to be used as graph edge attributes.
  void AddEdgeAttr(TStrV& Attrs) { AddGraphAttributeV(Attrs, true, false, false); }
  /// Adds column to be used as src node atribute of the graph.
  void AddSrcNodeAttr(const TStr& Attr) { AddGraphAttribute(Attr, false, true, false); }
  /// Adds columns to be used as src node attributes of the graph.
  void AddSrcNodeAttr(TStrV& Attrs) { AddGraphAttributeV(Attrs, false, true, false); }
  /// Adds column to be used as dst node atribute of the graph.
  void AddDstNodeAttr(const TStr& Attr) { AddGraphAttribute(Attr, false, false, true); }
  /// Adds columns to be used as dst node attributes of the graph.
  void AddDstNodeAttr(TStrV& Attrs) { AddGraphAttributeV(Attrs, false, false, true); }
  /// Handles the common case where src and dst both belong to the same "universe" of entities.
  void AddNodeAttr(const TStr& Attr) { AddSrcNodeAttr(Attr); AddDstNodeAttr(Attr); }
  /// Handles the common case where src and dst both belong to the same "universe" of entities.
  void AddNodeAttr(TStrV& Attrs) { AddSrcNodeAttr(Attrs); AddDstNodeAttr(Attrs); }
  /// Sets the columns to be used as both src and dst node attributes.
  void SetCommonNodeAttrs(const TStr& SrcAttr, const TStr& DstAttr, const TStr& CommonAttrName){
    CommonNodeAttrs.Add(TStrTr(NormalizeColName(SrcAttr), NormalizeColName(DstAttr), NormalizeColName(CommonAttrName)));
  }
  /// Gets src node int attribute name vector.
  TStrV GetSrcNodeIntAttrV() const;
  /// Gets dst node int attribute name vector.
  TStrV GetDstNodeIntAttrV() const;
  /// Gets edge int attribute name vector.
  TStrV GetEdgeIntAttrV() const;
  /// Gets src node float attribute name vector.
	TStrV GetSrcNodeFltAttrV() const;
  /// Gets dst node float attribute name vector.
  TStrV GetDstNodeFltAttrV() const;
  /// Gets edge float attribute name vector.
	TStrV GetEdgeFltAttrV() const;
  /// Gets src node str attribute name vector.
	TStrV GetSrcNodeStrAttrV() const;
  /// Gets dst node str attribute name vector.
  TStrV GetDstNodeStrAttrV() const;
  /// Gets edge str attribute name vector.
	TStrV GetEdgeStrAttrV() const;

  /// Extracts node TTable from PNEANet.
  static PTable GetNodeTable(const PNEANet& Network, TTableContext* Context);
  /// Extracts edge TTable from PNEANet.
  static PTable GetEdgeTable(const PNEANet& Network, TTableContext* Context);

#ifdef USE_OPENMP
  /// Extracts edge TTable from parallel graph PNGraphMP.
  static PTable GetEdgeTablePN(const PNGraphMP& Network, TTableContext* Context);
#endif // USE_OPENMP

  /// Extracts node and edge property TTables from THash.
  static PTable GetFltNodePropertyTable(const PNEANet& Network, const TIntFltH& Property,
   const TStr& NodeAttrName, const TAttrType& NodeAttrType, const TStr& PropertyAttrName,
   TTableContext* Context);

/***** Basic Getters *****/
  /// Gets type of column \c ColName.
	TAttrType GetColType(const TStr& ColName) const {
    TStr NColName = NormalizeColName(ColName);
    return ColTypeMap.GetDat(NColName).Val1;
  }
  /// Gets total number of rows in this table.
  TInt GetNumRows() const { return NumRows;}
  /// Gets number of valid, i.e. not deleted, rows in this table.
  TInt GetNumValidRows() const { return NumValidRows;}

  /// Gets a map of logical to physical row ids.
  THash<TInt, TInt> GetRowIdMap() const { return RowIdMap;}

/***** Iterators *****/
  /// Gets iterator to the first valid row of the table.
  TRowIterator BegRI() const { return TRowIterator(FirstValidRow, this);}
  /// Gets iterator to the last valid row of the table.
  TRowIterator EndRI() const { return TRowIterator(TTable::Last, this);}
  /// Gets iterator with reomve to the first valid row.
  TRowIteratorWithRemove BegRIWR(){ return TRowIteratorWithRemove(FirstValidRow, this);}
  /// Gets iterator with reomve to the last valid row.
  TRowIteratorWithRemove EndRIWR(){ return TRowIteratorWithRemove(TTable::Last, this);}
  /// Partitions the table into \c NumPartitions and populate \c Partitions with the ranges.
  void GetPartitionRanges(TIntPrV& Partitions, TInt NumPartitions) const;

/***** Table Operations *****/
  /// Renames a column.
  void Rename(const TStr& Column, const TStr& NewLabel);

	/// Removes rows with duplicate values in given column.
  void Unique(const TStr& Col);
  /// Removes rows with duplicate values in given columns.
	void Unique(const TStrV& Cols, TBool Ordered = true);

	/// Selects rows that satisfy given \c Predicate. ##TTable::Select
	void Select(TPredicate& Predicate, TIntV& SelectedRows, TBool Remove = true);
  void Select(TPredicate& Predicate) {
    TIntV SelectedRows;
    Select(Predicate, SelectedRows, true);
  }
  void Classify(TPredicate& Predicate, const TStr& LabelName, const TInt& PositiveLabel = 1,
   const TInt& NegativeLabel = 0);

  /// Selects rows using atomic compare operation. ##TTable::SelectAtomic
  void SelectAtomic(const TStr& Col1, const TStr& Col2, TPredComp Cmp,
   TIntV& SelectedRows, TBool Remove = true);
  void SelectAtomic(const TStr& Col1, const TStr& Col2, TPredComp Cmp) {
    TIntV SelectedRows;
    SelectAtomic(Col1, Col2, Cmp, SelectedRows, true);
  }
  void ClassifyAtomic(const TStr& Col1, const TStr& Col2, TPredComp Cmp,
   const TStr& LabelName, const TInt& PositiveLabel = 1, const TInt& NegativeLabel = 0);

  /// Selects rows where the value of \c Col matches given primitive \c Val.
  void SelectAtomicConst(const TStr& Col, const TPrimitive& Val, TPredComp Cmp,
   TIntV& SelectedRows, PTable& SelectedTable, TBool Remove = true, TBool Table = true);

  template <class T>
  void SelectAtomicConst(const TStr& Col, const T& Val, TPredComp Cmp) {
    TIntV SelectedRows;
    PTable SelectedTable;
    SelectAtomicConst(Col, TPrimitive(Val), Cmp, SelectedRows, SelectedTable, true, false);
  }
  template <class T>
  void SelectAtomicConst(const TStr& Col, const T& Val, TPredComp Cmp, PTable& SelectedTable) {
    TIntV SelectedRows;
    SelectAtomicConst(Col, TPrimitive(Val), Cmp, SelectedRows, SelectedTable, false, true);
  }
  template <class T>
  void ClassifyAtomicConst(const TStr& Col, const T& Val, TPredComp Cmp,
   const TStr& LabelName, const TInt& PositiveLabel = 1, const TInt& NegativeLabel = 0) {
    TIntV SelectedRows;
    PTable SelectedTable;
    SelectAtomicConst(Col, TPrimitive(Val), Cmp, SelectedRows, SelectedTable, false, false);
    ClassifyAux(SelectedRows, LabelName, PositiveLabel, NegativeLabel);
  }

  void SelectAtomicIntConst(const TStr& Col, const TInt& Val, TPredComp Cmp) {
    SelectAtomicConst(Col, Val, Cmp);
  }
  void SelectAtomicIntConst(const TStr& Col, const TInt& Val, TPredComp Cmp, PTable& SelectedTable) {
    SelectAtomicConst(Col, Val, Cmp, SelectedTable);
  }

  void SelectAtomicStrConst(const TStr& Col, const TStr& Val, TPredComp Cmp) {
    SelectAtomicConst(Col, Val, Cmp);
  }
  void SelectAtomicStrConst(const TStr& Col, const TStr& Val, TPredComp Cmp, PTable& SelectedTable) {
    SelectAtomicConst(Col, Val, Cmp, SelectedTable);
  }

  void SelectAtomicFltConst(const TStr& Col, const TFlt& Val, TPredComp Cmp) {
    SelectAtomicConst(Col, Val, Cmp);
  }
  void SelectAtomicFltConst(const TStr& Col, const TFlt& Val, TPredComp Cmp, PTable& SelectedTable) {
    SelectAtomicConst(Col, Val, Cmp, SelectedTable);
  }

  /// Groups rows depending on values of \c GroupBy columns. ##TTable::Group
  void Group(const TStrV& GroupBy, const TStr& GroupColName, TBool Ordered = true, TBool UsePhysicalIds = true);
  
  /// Counts number of unique elements. ##TTable::Count
  void Count(const TStr& CountColName, const TStr& Col);

  /// Orders the rows according to the values in columns of OrderBy (in descending lexicographic order).
  void Order(const TStrV& OrderBy, TStr OrderColName = "", TBool ResetRankByMSC = false, TBool Asc = true);

  /// Aggregates values of ValAttr after grouping with respect to GroupByAttrs. Result are stored as new attribute ResAttr.
  void Aggregate(const TStrV& GroupByAttrs, TAttrAggr AggOp, const TStr& ValAttr,
   const TStr& ResAttr, TBool Ordered = true);

  /// Aggregates attributes in AggrAttrs across columns.
  void AggregateCols(const TStrV& AggrAttrs, TAttrAggr AggOp, const TStr& ResAttr);

  /// Splices table into subtables according to a grouping statement.
  TVec<PTable> SpliceByGroup(const TStrV& GroupByAttrs, TBool Ordered = true);

  /// Performs equijoin. ##TTable::Join
  PTable Join(const TStr& Col1, const TTable& Table, const TStr& Col2);
  PTable Join(const TStr& Col1, const PTable& Table, const TStr& Col2) {
    return Join(Col1, *Table, Col2);
  }
  PTable ThresholdJoin(const TStr& KeyCol1, const TStr& JoinCol1, const TTable& Table, const TStr& KeyCol2, const TStr& JoinCol2, TInt Threshold, TBool PerJoinKey = false);
  
  /// Joins table with itself, on values of \c Col.
  PTable SelfJoin(const TStr& Col) { return Join(Col, *this, Col); }
  PTable SelfSimJoin(const TStrV& Cols, const TStr& DistanceColName, const TSimType& SimType, const TFlt& Threshold) { return SimJoin(Cols, *this, Cols, DistanceColName, SimType, Threshold); }
	/// Performs join if the distance between two rows is less than the specified threshold. ##TTable::SimJoinPerGroup
	PTable SelfSimJoinPerGroup(const TStr& GroupAttr, const TStr& SimCol, const TStr& DistanceColName, const TSimType& SimType, const TFlt& Threshold);

	/// Performs join if the distance between two rows is less than the specified threshold.
	PTable SelfSimJoinPerGroup(const TStrV& GroupBy, const TStr& SimCol, const TStr& DistanceColName, const TSimType& SimType, const TFlt& Threshold);

	/// Performs join if the distance between two rows is less than the specified threshold.
	PTable SimJoin(const TStrV& Cols1, const TTable& Table, const TStrV& Cols2, const TStr& DistanceColName, const TSimType& SimType, const TFlt& Threshold);
  /// Selects first N rows from the table.
  void SelectFirstNRows(const TInt& N);

	// Computes distances between elements in this->Col1 and Table->Col2 according
	// to given metric. Store the distances in DistCol, but keep only rows where
	// distance <= threshold
	// void Dist(const TStr& Col1, const TTable& Table, const TStr Col2, const TStr& DistColName,
  //  const TMetric& Metric, TFlt threshold);

  /// Releases memory of deleted rows, and defrags. ##TTable::Defrag
  void Defrag();

  /// Adds entire int column to table.
  void StoreIntCol(const TStr& ColName, const TIntV& ColVals);
  /// Adds entire flt column to table.
  void StoreFltCol(const TStr& ColName, const TFltV& ColVals);
  /// Adds entire str column to table.
  void StoreStrCol(const TStr& ColName, const TStrV& ColVals);
  
  // Assumption: KeyAttr is a primary key in this table, and FKeyAttr is a primary key in 
  // the argument table. Equivalent to SQL's: UPDATE this SET UpdateAttr = ReadAttr WHERE KeyAttr = FKeyAttr
  void UpdateFltFromTable(const TStr& KeyAttr, const TStr& UpdateAttr, const TTable& Table, 
  	const TStr& FKeyAttr, const TStr& ReadAttr, TFlt DefaultFltVal = 0.0);
#ifdef GCC_ATOMIC
  void UpdateFltFromTableMP(const TStr& KeyAttr, const TStr& UpdateAttr, const TTable& Table, 
  	const TStr& FKeyAttr, const TStr& ReadAttr, TFlt DefaultFltVal = 0.0);
  // TODO: this should be a generic vector operation (parallel equivalent to TVec::PutAll)
  void SetFltColToConstMP(TInt UpdateColIdx, TFlt DefaultFltVal);
#endif // GCC_ATOMIC

  /// Returns union of this table with given \c Table.
  PTable Union(const TTable& Table);
  PTable Union(const PTable& Table) { return Union(*Table); };
  /// Returns union of this table with given \c Table, preserving duplicates.
  PTable UnionAll(const TTable& Table);
  PTable UnionAll(const PTable& Table) { return UnionAll(*Table); };
  /// Same as TTable::ConcatTable
  void UnionAllInPlace(const TTable& Table);
  void UnionAllInPlace(const PTable& Table) { return UnionAllInPlace(*Table); };
  /// Returns intersection of this table with given \c Table.
  PTable Intersection(const TTable& Table);
  PTable Intersection(const PTable& Table) { return Intersection(*Table); };
  /// Returns table with rows that are present in this table but not in given \c Table.
  PTable Minus(TTable& Table);
  PTable Minus(const PTable& Table) { return Minus(*Table); };
  /// Returns table with only the columns in \c ProjectCols.
  PTable Project(const TStrV& ProjectCols);
  /// Keeps only the columns specified in \c ProjectCols.
  void ProjectInPlace(const TStrV& ProjectCols);

  /* Column-wise arithmetic operations */

  /// Performs columnwise arithmetic operation ##TTable::ColGenericOp
  void ColGenericOp(const TStr& Attr1, const TStr& Attr2, const TStr& ResAttr, TArithOp op);
#ifdef USE_OPENMP
  void ColGenericOpMP(TInt ArgColIdx1, TInt ArgColIdx2, TAttrType ArgType1, TAttrType ArgType2, TInt ResColIdx, TArithOp op);
#endif // USE_OPENMP
  /// Performs columnwise addition. See TTable::ColGenericOp
  void ColAdd(const TStr& Attr1, const TStr& Attr2, const TStr& ResultAttrName="");
  /// Performs columnwise subtraction. See TTable::ColGenericOp
  void ColSub(const TStr& Attr1, const TStr& Attr2, const TStr& ResultAttrName="");
  /// Performs columnwise multiplication. See TTable::ColGenericOp
  void ColMul(const TStr& Attr1, const TStr& Attr2, const TStr& ResultAttrName="");
  /// Performs columnwise division. See TTable::ColGenericOp
  void ColDiv(const TStr& Attr1, const TStr& Attr2, const TStr& ResultAttrName="");
  /// Performs columnwise modulus. See TTable::ColGenericOp
  void ColMod(const TStr& Attr1, const TStr& Attr2, const TStr& ResultAttrName="");
  /// Performs min of two columns. See TTable::ColGenericOp
  void ColMin(const TStr& Attr1, const TStr& Attr2, const TStr& ResultAttrName="");
  /// Performs max of two columns. See TTable::ColGenericOp
  void ColMax(const TStr& Attr1, const TStr& Attr2, const TStr& ResultAttrName="");

  /// Performs columnwise arithmetic operation with column of given table.
  void ColGenericOp(const TStr& Attr1, TTable& Table, const TStr& Attr2, const TStr& ResAttr,
    TArithOp op, TBool AddToFirstTable);
  // void ColGenericOpMP(TTable& Table, TBool AddToFirstTable, TInt ArgColIdx1, TInt ArgColIdx2,
  //  TAttrType ArgType1, TAttrType ArgType2, TInt ResColIdx, TArithOp op);
  /// Performs columnwise addition with column of given table.
  void ColAdd(const TStr& Attr1, TTable& Table, const TStr& Attr2, const TStr& ResAttr="",
    TBool AddToFirstTable=true);
  /// Performs columnwise subtraction with column of given table.
  void ColSub(const TStr& Attr1, TTable& Table, const TStr& Attr2, const TStr& ResAttr="",
    TBool AddToFirstTable=true);
  /// Performs columnwise multiplication with column of given table.
  void ColMul(const TStr& Attr1, TTable& Table, const TStr& Attr2, const TStr& ResAttr="",
    TBool AddToFirstTable=true);
  /// Performs columnwise division with column of given table.
  void ColDiv(const TStr& Attr1, TTable& Table, const TStr& Attr2, const TStr& ResAttr="",
    TBool AddToFirstTable=true);
  /// Performs columnwise modulus with column of given table.
  void ColMod(const TStr& Attr1, TTable& Table, const TStr& Attr2, const TStr& ResAttr="",
    TBool AddToFirstTable=true);

  /// Performs arithmetic op of column values and given \c Num
  void ColGenericOp(const TStr& Attr1, const TFlt& Num, const TStr& ResAttr, TArithOp op, const TBool floatCast);
#ifdef USE_OPENMP
  void ColGenericOpMP(const TInt& ColIdx1, const TInt& ColIdx2, TAttrType ArgType, const TFlt& Num, TArithOp op, TBool ShouldCast);
#endif // USE_OPENMP
  /// Performs addition of column values and given \c Num
  void ColAdd(const TStr& Attr1, const TFlt& Num, const TStr& ResultAttrName="", const TBool floatCast=false);
  /// Performs subtraction of column values and given \c Num
  void ColSub(const TStr& Attr1, const TFlt& Num, const TStr& ResultAttrName="", const TBool floatCast=false);
  /// Performs multiplication of column values and given \c Num
  void ColMul(const TStr& Attr1, const TFlt& Num, const TStr& ResultAttrName="", const TBool floatCast=false);
  /// Performs division of column values and given \c Num
  void ColDiv(const TStr& Attr1, const TFlt& Num, const TStr& ResultAttrName="", const TBool floatCast=false);
  /// Performs modulus of column values and given \c Num
  void ColMod(const TStr& Attr1, const TFlt& Num, const TStr& ResultAttrName="", const TBool floatCast=false);

  /* Column-wise string operations */

  /// Concatenates two string columns.
  void ColConcat(const TStr& Attr1, const TStr& Attr2, const TStr& Sep = "", const TStr& ResAttr="");
  /// Concatenates string column with column of given table.
  void ColConcat(const TStr& Attr1, TTable& Table, const TStr& Attr2, const TStr& Sep = "", const TStr& ResAttr="",
    TBool AddToFirstTable=true);
  /// Concatenates column values with given string value.
  void ColConcatConst(const TStr& Attr1, const TStr& Val, const TStr& Sep = "", const TStr& ResAttr="");

  /// Reads values of entire int column into \c Result.
  void ReadIntCol(const TStr& ColName, TIntV& Result) const;
  /// Reads values of entire float column into \c Result.
  void ReadFltCol(const TStr& ColName, TFltV& Result) const;
  /// Reads values of entire string column into \c Result.
  void ReadStrCol(const TStr& ColName, TStrV& Result) const;

  /// Adds explicit row ids, initialize hash set mapping ids to physical rows.
  void InitIds();

  /// Distance based filter. ##TTable::IsNextK
  PTable IsNextK(const TStr& OrderCol, TInt K, const TStr& GroupBy, const TStr& RankColName = "");

  /// Gets sequence of PageRank tables from given \c GraphSeq.
  static TTableIterator GetMapPageRank(const TVec<PNEANet>& GraphSeq, TTableContext* Context,
   const double& C = 0.85, const double& Eps = 1e-4, const int& MaxIter = 100) {
    TVec<PTable> TableSeq(GraphSeq.Len());
    TSnap::MapPageRank(GraphSeq, TableSeq, Context, C, Eps, MaxIter);
    return TTableIterator(TableSeq);
  }

  /// Gets sequence of Hits tables from given \c GraphSeq.
  static TTableIterator GetMapHitsIterator(const TVec<PNEANet>& GraphSeq,
   TTableContext* Context, const int& MaxIter = 20) {
    TVec<PTable> TableSeq(GraphSeq.Len());
    TSnap::MapHits(GraphSeq, TableSeq, Context, MaxIter);
    return TTableIterator(TableSeq);
  }
  
  void PrintSize();
  void PrintContextSize();
  /// Returns approximate memory used by table in [KB]
  TSize GetMemUsedKB();
  /// Returns approximate memory used by table context in [KB]
  TSize GetContextMemUsedKB();

  friend class TPt<TTable>;
  friend class TRowIterator;
  friend class TRowIteratorWithRemove;
};

typedef TPair<TStr,TAttrType> TStrTypPr;

template<class T>
TInt TTable::CheckAndAddFltNode(T Graph, THash<TFlt, TInt>& NodeVals, TFlt FNodeVal) {
  if (!NodeVals.IsKey(FNodeVal)) {
    TInt NodeVal = NodeVals.Len();
    Graph->AddNode(NodeVal);
    NodeVals.AddKey(FNodeVal);
    NodeVals.AddDat(FNodeVal, NodeVal);
    return NodeVal;
  } else { return NodeVals.GetDat(FNodeVal); }
}

template <class T>
T TTable::AggregateVector(TVec<T>& V, TAttrAggr Policy) {
  switch (Policy) {
    case aaMin: {
      T Res = V[0];
      for (TInt i = 1; i < V.Len(); i++) {
        if (V[i] < Res) { Res = V[i]; }
      }
      return Res;
    }
    case aaMax: {
      T Res = V[0];
      for (TInt i = 1; i < V.Len(); i++) {
        if (V[i] > Res) { Res = V[i]; }
      }
      return Res;
    }
    case aaFirst: {
      return V[0];
    }
    case aaLast:{
      return V[V.Len()-1];
    }
    case aaSum: {
      T Res = V[0];
      for (TInt i = 1; i < V.Len(); i++) {
        Res = Res + V[i];
      }
      return Res;
    }
    case aaMean: {
      T Res = V[0];
      for (TInt i = 1; i < V.Len(); i++) {
        Res = Res + V[i];
      }
      //Res = Res / V.Len(); // TODO: Handle Str case separately?
      return Res;
    }
    case aaMedian: {
      V.Sort();
      return V[V.Len()/2];
    }
    case aaCount: {
      // NOTE: Code should never reach here
      // I had to put this here to avoid a compiler warning.
      // Is there a better way to do this?
      return V[0];
    }
  }
  // Added to remove a compiler warning.
  T ShouldNotComeHere;
  return ShouldNotComeHere;
}

template <class T>
void TTable::GroupByIntCol(const TStr& GroupBy, T& Grouping, 
 const TIntV& IndexSet, TBool All, TBool UsePhysicalIds) const {
  TInt IdColIdx = GetColIdx(IdColName);
  if(!UsePhysicalIds && IdColIdx < 0){
  	TExcept::Throw("Grouping: Either use physical row ids, or have an id column");
  }
 // TO do: add a check if grouping already exists and is valid
  GroupingSanityCheck(GroupBy, atInt);
  if (All) {
     // Optimize for the common and most expensive case - iterate over only valid rows.
    for (TRowIterator it = BegRI(); it < EndRI(); it++) {
      TInt idx = UsePhysicalIds ? it.GetRowIdx() : it.GetIntAttr(IdColIdx);
      UpdateGrouping<TInt>(Grouping, it.GetIntAttr(GroupBy), idx);
    }
  } else {
    // Consider only rows in IndexSet.
    for (TInt i = 0; i < IndexSet.Len(); i++) {
      if (IsRowValid(IndexSet[i])) {
        TInt RowIdx = IndexSet[i];
        const TIntV& Col = IntCols[GetColIdx(GroupBy)];
        TInt idx = UsePhysicalIds ? RowIdx : IntCols[IdColIdx][RowIdx];       
        UpdateGrouping<TInt>(Grouping, Col[RowIdx], idx);
      }
    }
  }
}

template <class T>
void TTable::GroupByFltCol(const TStr& GroupBy, T& Grouping, 
 const TIntV& IndexSet, TBool All, TBool UsePhysicalIds) const {
  TInt IdColIdx = GetColIdx(IdColName);
  if(!UsePhysicalIds && IdColIdx < 0){
  	TExcept::Throw("Grouping: Either use physical row ids, or have an id column");
  }
  GroupingSanityCheck(GroupBy, atFlt);
  if (All) {
     // Optimize for the common and most expensive case - iterate over only valid rows.
    for (TRowIterator it = BegRI(); it < EndRI(); it++) {
      TInt idx = UsePhysicalIds ? it.GetRowIdx() : it.GetIntAttr(IdColIdx);
      UpdateGrouping<TFlt>(Grouping, it.GetFltAttr(GroupBy), idx);
    }
  } else {
    // Consider only rows in IndexSet.
    for (TInt i = 0; i < IndexSet.Len(); i++) {
      if (IsRowValid(IndexSet[i])) {
        TInt RowIdx = IndexSet[i];
        const TFltV& Col = FltCols[GetColIdx(GroupBy)];   
        TInt idx = UsePhysicalIds ? RowIdx : IntCols[IdColIdx][RowIdx];     
        UpdateGrouping<TFlt>(Grouping, Col[RowIdx], idx);
      }
    }
  }
}

template <class T>
void TTable::GroupByStrCol(const TStr& GroupBy, T& Grouping, 
 const TIntV& IndexSet, TBool All, TBool UsePhysicalIds) const {
  TInt IdColIdx = GetColIdx(IdColName);
  if(!UsePhysicalIds && IdColIdx < 0){
  	TExcept::Throw("Grouping: Either use physical row ids, or have an id column");
  }
  GroupingSanityCheck(GroupBy, atStr);
  if (All) {
    // Optimize for the common and most expensive case - iterate over all valid rows.
    for (TRowIterator it = BegRI(); it < EndRI(); it++) {
      TInt idx = UsePhysicalIds ? it.GetRowIdx() : it.GetIntAttr(IdColIdx);
      UpdateGrouping<TInt>(Grouping, it.GetStrMapByName(GroupBy), idx);
    }
  } else {
    // Consider only rows in IndexSet.
    for (TInt i = 0; i < IndexSet.Len(); i++) {
      if (IsRowValid(IndexSet[i])) {
        TInt RowIdx = IndexSet[i];
        TInt ColIdx = GetColIdx(GroupBy);
        TInt idx = UsePhysicalIds ? RowIdx : IntCols[IdColIdx][RowIdx];  
        UpdateGrouping<TInt>(Grouping, StrColMaps[ColIdx][RowIdx], idx);
      }
    }
  }
}

template <class T>
void TTable::UpdateGrouping(THash<T,TIntV>& Grouping, T Key, TInt Val) const{
  if (Grouping.IsKey(Key)) {
    Grouping.GetDat(Key).Add(Val);
  } else {
    TIntV NewGroup;
    NewGroup.Add(Val);
    Grouping.AddDat(Key, NewGroup);
  }
}

#ifdef GCC_ATOMIC
template <class T>
void TTable::UpdateGrouping(THashMP<T,TIntV>& Grouping, T Key, TInt Val) const{
  if (Grouping.IsKey(Key)) {
  	//printf("y\n");
    Grouping.GetDat(Key).Add(Val);
  } else {
  	//printf("n\n");
    TIntV NewGroup;
    NewGroup.Add(Val);
    Grouping.AddDat(Key, NewGroup);
  }
}
#endif // GCC_ATOMIC

/*
template<class T> 
void TTable::RegisterGrouping(const T& Grouping, const TStr& GroupByCol, TBool UsePhysicalIds){
	TStrV GroupByVec;
	GroupByVec.Add(GroupByCol);
	GroupStmt Stmt(NormalizeColNameV(GroupByVec), true, UsePhysicalIds);
	GroupMapping.AddKey(Stmt);
	for(T::TIter it = Grouping.BegI(); it < Grouping.EndI(); it++){
		GroupMapping.GetDat(Stmt).AddDat(it.GetKey(), TIntV(it.GetDat()));
	}
}
*/

namespace TSnap {

  /// Gets sequence of PageRank tables from given \c GraphSeq into \c TableSeq.
  template <class PGraph>
  void MapPageRank(const TVec<PGraph>& GraphSeq, TVec<PTable>& TableSeq,
   TTableContext* Context, const double& C, const double& Eps, const int& MaxIter) {
    int NumGraphs = GraphSeq.Len();
    TableSeq.Reserve(NumGraphs, NumGraphs);
    // This loop is parallelizable.
    for (TInt i = 0; i < NumGraphs; i++){
      TIntFltH PRankH;
      GetPageRank(GraphSeq[i], PRankH, C, Eps, MaxIter);
      TableSeq[i] = TTable::TableFromHashMap(PRankH, "NodeId", "PageRank", Context, false);
    }
  }

  /// Gets sequence of Hits tables from given \c GraphSeq into \c TableSeq.
  template <class PGraph>
  void MapHits(const TVec<PGraph>& GraphSeq, TVec<PTable>& TableSeq,
    TTableContext* Context, const int& MaxIter) {
    int NumGraphs = GraphSeq.Len();
    TableSeq.Reserve(NumGraphs, NumGraphs);
    // This loop is parallelizable.
    for (TInt i = 0; i < NumGraphs; i++){
      TIntFltH HubH;
      TIntFltH AuthH;
      GetHits(GraphSeq[i], HubH, AuthH, MaxIter);
      PTable HubT =  TTable::TableFromHashMap(HubH, "NodeId", "Hub", Context, false);
      PTable AuthT =  TTable::TableFromHashMap(AuthH, "NodeId", "Authority", Context, false);
      PTable HitsT = HubT->Join("NodeId", AuthT, "NodeId");
      HitsT->Rename("1.NodeId", "NodeId");
      HitsT->Rename("1.Hub", "Hub");
      HitsT->Rename("2.Authority", "Authority");
      TStrV V = TStrV(3, 0);
      V.Add("NodeId");
      V.Add("Hub");
      V.Add("Authority");
      HitsT->ProjectInPlace(V);
      TableSeq[i] = HitsT;
    }
  }
}

#endif //TABLE_H

