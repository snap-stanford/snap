#include "Predicate.h"

const TPredicate::TAtomicPredicate TPredicate::NonAtom = TAtomicPredicate(INT, true, EQ, "", "", 0, 0, "");

void TPredicate::TPredicateNode::GetVariables(TStrV& Variables){
  if(Left != NULL){ Left->GetVariables(Variables);}
  if(Right != NULL){ Right->GetVariables(Variables);}
  if(Op == NOP){
    if(Atom.Lvar != ""){ Variables.Add(Atom.Lvar);}
    if(Atom.Rvar != ""){ Variables.Add(Atom.Rvar);}
  }
}

void TPredicate::GetVariables(TStrV& Variables){
  Root->GetVariables(Variables);
}

TBool TPredicate::Eval(){
  TPredicateNode* Curr = Root;
  TPredicateNode* Prev = NULL;
  while(!(Curr == NULL && Prev == Root))
  {
    // going down the tree
     if(Prev == NULL || Prev == Curr->Parent){
       // left child exists and was not yet evaluated
       if(Curr->Left != NULL){
         Prev = Curr;
         Curr = Curr->Left;
       } else if(Curr->Right != NULL){
         Prev = Curr;
         Curr = Curr->Right;
       } else{
         Curr->Result = EvalAtomicPredicate(Curr->Atom);
         Prev = Curr;
         Curr = Curr->Parent;
       }
     } else if(Prev == Curr->Left){
     // going back up through left (first) child
         switch(Curr->Op){
           case NOT:{
             Assert(Curr->Right == NULL);
             Curr->Result = !(Prev->Result);
             Prev = Curr;
             Curr = Curr->Parent;
             break;
           }
           case AND:{
             Assert(Curr->Right != NULL);
             if(!Prev->Result){
               Curr->Result = false;
               Prev = Curr;
               Curr = Curr->Parent;
             } else{
               Prev = Curr;
               Curr = Curr->Right;
             }
             break;
           }
           case OR:{
             Assert(Curr->Right != NULL);
             if(Prev->Result){
               Curr->Result = true;
               Prev = Curr;
               Curr = Curr->Parent;
             } else{
               Prev = Curr;
               Curr = Curr->Right;
             }
             break;
           }
           case NOP:{
             break;
           }
         }
      // going back up the tree from right (second) child
     } else{
       Assert(Prev == Curr->Right);
       switch(Curr->Op){
         case NOT:{
           Assert(Curr->Left == NULL);
           Curr->Result = !(Prev->Result);
           break;
         }
         case AND:{
           Assert(Curr->Left != NULL);
           Assert(Curr->Left->Result);
           Curr->Result = Prev->Result;
           break;
         }
         case OR:{
           Assert(Curr->Left != NULL);
           Assert(!Curr->Left->Result);
           Curr->Result = Prev->Result;
           break;
         }
         case NOP:{
           break;
         }
       }
       Prev = Curr;
       Curr = Curr->Parent;
     }
  }
  return Root->Result;
}

TBool TPredicate::EvalAtomicPredicate(const TAtomicPredicate& Atom){
   switch(Atom.Type){
     case INT:{
       if(Atom.IsConst){ return EvalAtom<TInt>(IntVars.GetDat(Atom.Lvar), Atom.IntConst, Atom.Compare);}
       return EvalAtom<TInt>(IntVars.GetDat(Atom.Lvar), IntVars.GetDat(Atom.Rvar), Atom.Compare);
     }
     case FLT:{
       if(Atom.IsConst){ return EvalAtom<TFlt>(FltVars.GetDat(Atom.Lvar), Atom.FltConst, Atom.Compare);}
       return EvalAtom<TFlt>(FltVars.GetDat(Atom.Lvar), FltVars.GetDat(Atom.Rvar), Atom.Compare);
     }
     case STR:{
       if(Atom.IsConst){ return EvalAtom<TStr>(StrVars.GetDat(Atom.Lvar), Atom.StrConst, Atom.Compare);}
       return EvalAtom<TStr>(StrVars.GetDat(Atom.Lvar), StrVars.GetDat(Atom.Rvar), Atom.Compare);
     }
   }
   return false;
}

