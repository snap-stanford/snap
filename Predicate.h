#ifndef PREDICATE_H
#define PREDICATE_H
#include "Snap.h"

/*
// Nikhil & Jason Initial implementation 
class TPredicate {
 public:
  typedef enum{INT, FLT, STR} TYPE;
  typedef struct FieldValue {
    TInt IntValue;
    TFlt FltValue;
    TStr StrValue;
  } FieldValue;

  // A Leaf node that is a triple that contains a binary comparison operator, and two field references.
  class Predicate {
   private:
    TStr Col1;
    TStr Col2;
    TStr Comp;
    
    //TODO(nkhadke): Can we get rid of this code duplication.
    bool EvalStrPred(TStr a, TStr b, TStr Comp) {
      if (Comp == "<") {
	return a < b;
      } else if (Comp == ">") {
	return a > b;
      } else if (Comp == ">=") {
	return a >= b;
      } else if (Comp == "<=") {
	return a <= b;
      } else if (Comp == "=") {
	return a == b;
      } else if (Comp == "!=") {
	return !(a == b);
      }
    }

    bool EvalFltPred(TFlt a, TFlt b, TStr Comp) {
      if (Comp == "<") {
	return a < b;
      } else if (Comp == ">") {
	return a > b;
      } else if (Comp == ">=") {
	return a >= b;
      } else if (Comp == "<=") {
	return a <= b;
      } else if (Comp == "=") {
	return a == b;
      } else if (Comp == "!=") {
	return !(a == b);
      }
    }
    bool EvalIntPred(TInt a, TInt b, TStr Comp) {
      if (Comp == "<") {
	return a < b;
      } else if (Comp == ">") {
	return a > b;
      } else if (Comp == ">=") {
	return a >= b;
      } else if (Comp == "<=") {
	return a <= b;
      } else if (Comp == "=") {
	return a == b;
      } else if (Comp == "!=") {
	return !(a == b);
      }
    }
    
   public:
    Predicate() {
      Col1 = TStr::GetNullStr();
      Col2 = TStr::GetNullStr();
      Comp = TStr::GetNullStr();
    }
    
    Predicate(TStr C1, TStr C2, TStr Cmp) {
      Col1 = C1;
      Col2 = C2;
      Comp = Cmp;
    }
    bool IsNull() {
      return (Col1 == TStr::GetNullStr() || Col2 == TStr::GetNullStr() || Comp == TStr::GetNullStr());
    }
    
    bool EvalPredicate(THash<TStr, TPair<TYPE, FieldValue> >& ValueDict) {
      TPair<TYPE, FieldValue> PairL = ValueDict.GetDat(Col1);
      TPair<TYPE, FieldValue> PairR = ValueDict.GetDat(Col2);
      if (PairL.Val1 != PairR.Val1) {
	// TODO(nkhadke): Throw error
	return false;
      }
      TYPE Type = PairL.Val1;
      if (Type == INT) {
	return EvalIntPred(PairL.Val2.IntValue, PairR.Val2.IntValue, Comp);
      } else if (Type == FLT) {
	return EvalFltPred(PairL.Val2.FltValue, PairR.Val2.FltValue, Comp);
      } else {
	return EvalStrPred(PairL.Val2.StrValue, PairR.Val2.StrValue, Comp);
      }
    }
  };
  
  // A TPredicate Tree contains pointers to 2 children that under circumstance
  // can be NULL (in the case of NOT). A TPredicate node can either be
  // a internal node, in which case it must have Op set to some boolean string
  // or a leaf node in which case it must have a non-null Predicate.
  TPredicate *Left;
  TPredicate *Right;
  TStr Op;
  Predicate Pred;

  TPredicate(Predicate Pred) {
    Left = NULL;
    Right = NULL;
    this->Op = TStr::GetNullStr();
    this->Pred = Pred;
  }
  
  TPredicate(TStr Op) {
    Left = NULL;
    Right = NULL;
    this->Op = Op;
    this->Pred = Predicate();
  }

  bool EvalTPredicate(TPredicate *Root, THash<TStr, TPair<TYPE,FieldValue> >& ValueDict) {
    if (Pred.IsNull() && Op == TStr::GetNullStr()) {
      return false;
    }
    if (!Pred.IsNull() && Op == TStr::GetNullStr()) {
      return Pred.EvalPredicate(ValueDict);
    }
    if (Op == "NOT") {
      if (!Left && !Right) {
	//TODO(nkhadke): Throw error.
	return false;
      } else if (Left) {
	return !(EvalTPredicate(Root->Left, ValueDict));
      } else {
	return !(EvalTPredicate(Root->Right, ValueDict));
      }
    } else if (Op == "AND") {
      return EvalTPredicate(Root->Left, ValueDict) && EvalTPredicate(Root->Right, ValueDict);
    } else if (Op == "OR") {
      return EvalTPredicate(Root->Left, ValueDict) || EvalTPredicate(Root->Right, ValueDict);
    } else {
      // TODO(nkhadke): Throw error.
      return false;
    }
  }

  bool EvalPredicate(THash<TStr, TPair<TYPE,FieldValue> >& ValueDict) {
    return EvalTPredicate(this, ValueDict);
  }
};
*/

class TPredicate{
public:
  typedef enum {INT, FLT, STR} TYPE;   // must be consistent with TYPE definition in TTable
  typedef enum {NOT, AND, OR, NOP} OP;
  typedef enum {LT = 0, LTE, EQ, NEQ, GTE, GT} COMP;
  class TAtomicPredicate;
protected:
  const static TAtomicPredicate NonAtom;
public:
  class TAtomicPredicate{
    TYPE Type;
    TBool IsConst;
    COMP Compare;
    TStr Lvar;
    TStr Rvar;
    TInt IntConst;
    TFlt FltConst;
    TStr StrConst;
  public:
    TAtomicPredicate(TYPE Typ, TBool IsCnst, COMP Cmp, TStr L, TStr R, TInt ICnst, TFlt FCnst, TStr SCnst):
      Type(Typ), IsConst(IsCnst), Compare(Cmp), Lvar(L), Rvar(R), IntConst(ICnst), FltConst(FCnst), StrConst(SCnst){}
    TAtomicPredicate(TYPE Typ, TBool IsCnst, COMP Cmp, TStr L, TStr R):
      Type(Typ), IsConst(IsCnst), Compare(Cmp), Lvar(L), Rvar(R), IntConst(0), FltConst(0), StrConst(""){}
    friend class TPredicate;
  };

  class TPredicateNode{
  //protected:
  public:
    OP Op;
    TBool Result;
    const TAtomicPredicate& Atom;
    TPredicateNode* Parent;
    TPredicateNode* Left;
    TPredicateNode* Right;
  public:
    TPredicateNode(): Op(NOP), Result(false), Atom(NonAtom), Parent(NULL), Left(NULL), Right(NULL){}
    // constructor for atomic predicate node (leaf)
    TPredicateNode(const TAtomicPredicate& A): Op(NOP), Result(false), Atom(A), Parent(NULL), Left(NULL), Right(NULL){}
    // constructor for logical operation predicate node (internal node)
    TPredicateNode(OP Opr): Op(Opr), Result(false), Atom(NonAtom), Parent(NULL), Left(NULL), Right(NULL){}
    TPredicateNode(const TPredicateNode& P): Op(P.Op), Result(P.Result), Atom(P.Atom), Parent(P.Parent), Left(P.Left), Right(P.Right){}
    void AddLeftChild(TPredicateNode* Child){ Left = Child; Child->Parent = this;}
    void AddRightChild(TPredicateNode* Child){ Right = Child; Child->Parent = this;}
    void GetVariables(TStrV& Variables);
    friend class TPredicate;
  };

protected:
  THash<TStr, TInt> IntVars;
  THash<TStr, TFlt> FltVars;
  THash<TStr, TStr> StrVars;
  TPredicateNode* Root;
public:
  TPredicate(): IntVars(), FltVars(), StrVars(){}
  TPredicate(TPredicateNode* R):IntVars(), FltVars(), StrVars(), Root(R){}
  TPredicate(const TPredicate& Pred): IntVars(Pred.IntVars), FltVars(Pred.FltVars), StrVars(Pred.StrVars), Root(Pred.Root){}
  void GetVariables(TStrV& Variables);
  // set variable value in the predicate or all the children that use it
  void SetIntVal(TStr VarName, TInt VarVal){ IntVars.AddDat(VarName, VarVal);}
  void SetFltVal(TStr VarName, TFlt VarVal){ FltVars.AddDat(VarName, VarVal);}
  void SetStrVal(TStr VarName, TStr VarVal){ StrVars.AddDat(VarName, VarVal);}
  TBool Eval();
  TBool EvalAtomicPredicate(const TAtomicPredicate& Atom);
  template <class T>
  TBool EvalAtom(T Val1, T Val2, COMP Cmp){
    switch(Cmp){
      case LT: return Val1 < Val2;
      case LTE: return Val1 <= Val2;
      case EQ: return Val1 == Val2;
      case NEQ: return Val1 != Val2;
      case GTE: return Val1 >= Val2;
      case GT: return Val1 > Val2;
    }
    return false;
  }
};
#endif // PREDICATE_H