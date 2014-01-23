#ifndef PREDICATE_H
#define PREDICATE_H

/// Attribute type
typedef enum TAttrType_ {atInt, atFlt, atStr} TAttrType;   
/// Boolean operators for selection predicates
typedef enum {NOT, AND, OR, NOP} TPredOp; 
/// Comparison operators for selection predicates
typedef enum {LT = 0, LTE, EQ, NEQ, GTE, GT, SUBSTR, SUPERSTR} TPredComp; 

//#//////////////////////////////////////////////
/// Predicate class - for encapsulating comparison operations
class TPredicate {
public:
  class TAtomicPredicate;
protected:
  static const TAtomicPredicate NonAtom;
public:
  //#//////////////////////////////////////////////
  /// Atomic predicate node - represents a binary predicate operation on two variables
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
  public:
    /// Default constructor
    TAtomicPredicate() : Type(NonAtom.Type), IsConst(NonAtom.IsConst), 
      Compare(NonAtom.Compare), Lvar(NonAtom.Lvar), Rvar(NonAtom.Rvar), 
      IntConst(NonAtom.IntConst), FltConst(NonAtom.FltConst), StrConst(NonAtom.StrConst) {}
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
  };

  //#//////////////////////////////////////////////
  /// Predicate node - represents a binary predicate operation on two predicate nodes
  class TPredicateNode {
  //protected:
  public:
    TPredOp Op; ///< Logical op represented by this node
    TBool Result; ///< Result of evaulating the predicate rooted at this node
    TAtomicPredicate Atom; ///< Atomic predicate at this node
    TPredicateNode* Parent; ///< Parent node of this node
    TPredicateNode* Left; ///< Left child of this node
    TPredicateNode* Right; ///< Right child of this node
  public:
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
  static TBool EvalStrAtom(TStr Val1, TStr Val2, TPredComp Cmp) {
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
#endif // PREDICATE_H

