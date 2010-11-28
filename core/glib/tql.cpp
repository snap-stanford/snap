/////////////////////////////////////////////////
// Includes
#include "StdAfx.h"
#include "tql.h"

/////////////////////////////////////////////////
// Tql-Exception
class ETql{
private:
  TStr ErrMsg;
public:
  ETql(const TStr& _ErrMsg):
    ErrMsg(_ErrMsg){}

  ETql& operator=(const ETql& E){
    ErrMsg=E.ErrMsg; return *this;}

  TStr GetErrMsg() const {return ErrMsg;}
};

/////////////////////////////////////////////////
// Tql-Lexical-Chars
void TTqlChDef::SetChTy(const TTqlLxChTy& ChTy, const TStr& Str){
  for (int ChN=0; ChN<Str.Len(); ChN++){ChTyV[Str[ChN]-TCh::Mn]=TInt(ChTy);}
}

TTqlChDef::TTqlChDef():
  ChTyV(TCh::Vals){
  ChTyV.PutAll(TInt(tlcAlpha));
  for (char Ch=TCh::NullCh; Ch<=' '; Ch++){SetChTy(tlcSpace, TStr(Ch));}
  SetChTy(tlcNum, "0123456789");
  SetChTy(tlcSym, "&|!()[]\"+-#*:<=>");
  SetChTy(tlcEof, TCh::EofCh);
}

/////////////////////////////////////////////////
// Tql-Lexical-Reserved-Words
TTqlRwMap::TTqlRwMap():
  StrToRwH(20){
  StrToRwH.AddDat("domain", trwDomain);
  StrToRwH.AddDat("depth", trwDepth);
  StrToRwH.AddDat("feature", trwFeature);
  StrToRwH.AddDat("outgoingurl", trwOutgoingUrl);
  StrToRwH.AddDat("title", trwTitle);
  StrToRwH.AddDat("after", trwAfter);
  StrToRwH.AddDat("before", trwBefore);
  StrToRwH.AddDat("within", trwWithin);
}

bool TTqlRwMap::IsRw(const TStr& RwStr, TTqlRw& Rw) const {
  int KeyId=StrToRwH.GetKeyId(RwStr);
  if (KeyId==-1){return false;}
  else {Rw=TTqlRw(int(StrToRwH[KeyId])); return true;}
}

/////////////////////////////////////////////////
// Tql-Expression-Lexical
TTqlChDef TTqlLx::ChDef;

TTqlLx::TTqlLx(const TStr& ExpStr):
  SIn(TStrIn::New(ExpStr)), Ch(' '),  Sym(tsyUndef), Str(){
}

TTqlLxSym TTqlLx::GetSym(const TFSet& Expect){
  Str.Clr();
  while (ChDef.IsSpace(Ch)){GetCh();}
  if (Expect.In(tsyNum)){
    if (!ChDef.IsNum(Ch)){throw ETql("Number expected.");}
    Sym=tsyNum;
    do {Str.AddCh(Ch); GetCh();} while (ChDef.IsNum(Ch));
  } else {
    switch (ChDef.GetChTy(Ch)){
      case tlcAlpha:
      case tlcNum:
        do {Str.AddCh(Ch); GetCh();} while (ChDef.IsAlNum(Ch));
        if (Str=="OR"){Sym=tsyOr;}
        else if (Str=="AND"){Sym=tsyAnd;}
        else if (Str=="NOT"){Sym=tsyNot;}
        else {Sym=tsyWord;}
        break;
      case tlcSym:
        Str.AddCh(Ch);
        switch (Ch){
          case '\"': Sym=tsyDQuote; GetCh(); break;
          case ':': Sym=tsyColon; GetCh(); break;
          case '(': Sym=tsyLParen; GetCh(); break;
          case ')': Sym=tsyRParen; GetCh(); break;
          case '[': Sym=tsyLBracket; GetCh(); break;
          case ']': Sym=tsyRBracket; GetCh(); break;
          case '|': Sym=tsyOr; GetCh(); break;
          case '&': Sym=tsyAnd; GetCh(); break;
          case '!': Sym=tsyNot; GetCh(); break;
          case '+': Sym=tsyIncl; GetCh(); break;
          case '-': Sym=tsyExcl; GetCh(); break;
          case '#': Sym=tsyTitle; GetCh(); break;
          case '*': Sym=tsyWCard; GetCh(); break;
          case '<':
            GetCh();
            if (Ch=='='){Sym=tsyLEq; GetCh();}
            else if (Ch=='>'){Sym=tsyNEq; GetCh();}
            else {Sym=tsyLss;}
            break;
          case '=': Sym=tsyEq; GetCh(); break;
          case '>':
            GetCh();
            if (Ch=='='){Sym=tsyGEq; GetCh();}
            else {Sym=tsyGtr;}
            break;
          default: Sym=tsySSym; GetCh();
        }
        break;
      case tlcEof: Sym=tsyEof; break;
      default: Sym=tsyUndef; GetCh();
    }
  }

  if ((!Expect.In(Sym))&&(!Expect.Empty())){
    if (Sym==tsyEof){
      throw ETql("Unexpected end of expression.");
    } else {
      throw ETql("Unexpected symbol.");
    }
  }
  return Sym;
}

/////////////////////////////////////////////////
// Tql-Expression-Item
TTqlItem::TTqlItem(const TTqlItemType& _Type,
 const PTqlItem& Item1, const PTqlItem& Item2):
  Type(_Type), ItemV(), WordStrV(){
  IAssert(
   (Type==titOr)||(Type==titAnd)||(Type==titNot)||
   (Type==titIncl)||(Type==titExcl)||
   (Type==titInTag)||(Type==titTitle)||(Type==titWCard));
  if (Type==titWCard){
    IAssert(Item1->GetType()==titWord);
    IAssert(Item2.Empty());
  }
  ItemV.Add(Item1);
  if (!Item2.Empty()){ItemV.Add(Item2);}
}

TTqlItem::TTqlItem(const TStr& TagNm, const PTqlItem& Item):
  Type(titInTag), ItemV(), WordStrV(){
  WordStrV.Add(TagNm);
  ItemV.Add(Item);
}

TTqlItem::TTqlItem(const TStr& TagNm, const TStr& RelOpStr, const TStr& ArgStr):
  Type(titTagRel), ItemV(), WordStrV(){
  WordStrV.Add(TagNm);
  WordStrV.Add(RelOpStr);
  WordStrV.Add(ArgStr);
}

TTqlItem::TTqlItem(const TStr& WordStr):
  Type(titWord), ItemV(), WordStrV(){
  WordStrV.Add(WordStr);
}

TTqlItem::TTqlItem(const TStrV& _WordStrV):
  Type(titPhrase), ItemV(), WordStrV(_WordStrV){}

TTqlItem::TTqlItem(const TStr& MetaTag, const TStrV& ArgStrV):
  Type(titMeta), ItemV(), WordStrV(){
  WordStrV.Add(MetaTag);
  WordStrV.AddV(ArgStrV);
}

PTqlItem TTqlItem::GetItem(const int& ItemN) const {
  IAssert(
   (Type==titOr)||(Type==titAnd)||(Type==titNot)||
   (Type==titIncl)||(Type==titExcl)||
   (Type==titInTag)||(Type==titTitle)||(Type==titWCard));
  return ItemV[ItemN];
}

TStr TTqlItem::GetTagNm() const {
  IAssert((Type==titInTag)||(Type==titTagRel));
  return WordStrV[0];
}

TRelOp TTqlItem::GetRelOp() const {
  IAssert(Type==titTagRel);
  TStr RelOpStr=WordStrV[1];
  if (RelOpStr=="<"){return roLs;}
  else if (RelOpStr=="<="){return roLEq;}
  else if (RelOpStr=="="){return roEq;}
  else if (RelOpStr=="<>"){return roEq;}
  else if (RelOpStr==">="){return roGEq;}
  else if (RelOpStr==">"){return roGt;}
  else {return roUndef;}
}

TStr TTqlItem::GetRelArgStr() const {
  IAssert(Type==titTagRel);
  return WordStrV[2];
}

TStr TTqlItem::GetWordStr() const {
  IAssert(Type==titWord);
  return WordStrV[0];
}

int TTqlItem::GetPhraseLen() const {
  IAssert(Type==titPhrase);
  return WordStrV.Len();
}

TStr TTqlItem::GetPhraseWordStr(const int& WordStrN) const {
  IAssert(Type==titPhrase);
  return WordStrV[WordStrN];
}

PTqlCtxSet TTqlItem::Eval(const TTqlIx& TqlIx) const {
  PTqlCtxSet CtxSet;
  switch (Type){
    case titOr:{
      CtxSet=GetItem(0)->Eval(TqlIx);
      PTqlCtxSet RDocIdSet=GetItem(1)->Eval(TqlIx);
      CtxSet=CtxSet->GetOrCtxSet(RDocIdSet);
      break;}
    case titAnd:{
      CtxSet=GetItem(0)->Eval(TqlIx);
      PTqlCtxSet RDocIdSet=GetItem(1)->Eval(TqlIx);
      CtxSet=CtxSet->GetAndCtxSet(RDocIdSet);
      break;}
    case titNot:{
      CtxSet=GetItem()->Eval(TqlIx);
      // CtxSet->PutNegated(!CtxSet->IsNegated());
      break;} //**
    case titIncl: CtxSet=GetItem()->Eval(TqlIx); break; //**
    case titExcl: CtxSet=GetItem()->Eval(TqlIx); break; //**
    case titInTag:
      CtxSet=GetItem()->Eval(TqlIx);
      CtxSet=CtxSet->GetTagCtxSet(GetTagNm());
      break;
    case titTagRel:
      CtxSet=TqlIx.GetCtxSet(GetTagNm(), GetRelOp(), GetRelArgStr(), 1000);
      break;
    case titTitle:
      CtxSet=GetItem()->Eval(TqlIx);
      CtxSet=CtxSet->GetTitleCtxSet();
      break;
    case titWCard:{
      PTqlItem SubItem=GetItem();
      CtxSet=TqlIx.GetCtxSet(SubItem->GetWordStr(), true, 100);
      break;}
    case titWord:{
      CtxSet=TqlIx.GetCtxSet(GetWordStr(), false, -1);
      break;}
    case titPhrase:{
      //** performs "and" between phrase words only
      if (GetPhraseLen()>0){
        CtxSet=TqlIx.GetCtxSet(GetPhraseWordStr(0), false, -1);
        for (int WordN=1; WordN<GetPhraseLen(); WordN++){
          PTqlCtxSet RDocIdSet=
           TqlIx.GetCtxSet(GetPhraseWordStr(WordN), false, -1);
          CtxSet=CtxSet->GetAndCtxSet(RDocIdSet);
        }
      } else {
        CtxSet=TqlIx.GetEmptyCtxSet();
      }
      break;}
    case titMeta: CtxSet=GetItem()->Eval(TqlIx); break; //**
    default: Fail;
  }
  return CtxSet;
}

/////////////////////////////////////////////////
// Tql-Expression
PTqlItem TTqlExp::ParseFact(TTqlLx& Lx, const TFSet& Expect){
  if (Lx.Sym==tsyNot){
    Lx.GetSym(FactExpect);
    PTqlItem Item=ParseFact(Lx, Expect);
    return PTqlItem(new TTqlItem(titNot, Item));
  } else
  if (Lx.Sym==tsyIncl){
    Lx.GetSym(FactExpect);
    PTqlItem Item=ParseFact(Lx, Expect);
    return PTqlItem(new TTqlItem(titIncl, Item));
  } else
  if (Lx.Sym==tsyExcl){
    Lx.GetSym(FactExpect);
    PTqlItem Item=ParseFact(Lx, Expect);
    return PTqlItem(new TTqlItem(titExcl, Item));
  } else
  if (Lx.Sym==tsyTitle){
    Lx.GetSym(FactExpect);
    PTqlItem Item=ParseFact(Lx, Expect);
    return PTqlItem(new TTqlItem(titTitle, Item));
  } else
  if (Lx.Sym==tsyLParen){
    Lx.GetSym(ExpExpect);
    PTqlItem Item=ParseExp(Lx, TFSet()|tsyRParen);
    Lx.GetSym(Expect);
    return Item;
  } else
  if (Lx.Sym==tsyLBracket){
    Lx.GetSym(tsyWord); TStr TagNm=Lx.Str;
    Lx.GetSym(tsyRBracket);
    Lx.GetSym(TFSet(FactExpect)|RelOpSet);
    if (RelOpSet.In(Lx.Sym)){
      TStr RelOpStr=Lx.Str;
      Lx.GetSym(tsyWord, tsyExcl);
      TStr ArgStr;
      if (Lx.Sym==tsyExcl){
        Lx.GetSym(tsyWord); ArgStr=TStr("-")+Lx.Str;
      } else {
        ArgStr=Lx.Str;
      }
      Lx.GetSym(Expect);
      return PTqlItem(new TTqlItem(TagNm, RelOpStr, ArgStr));
    } else {
      PTqlItem Item=ParseFact(Lx, Expect);
      return PTqlItem(new TTqlItem(TagNm, Item));
    }
  } else
  if (Lx.Sym==tsyDQuote){
    TStrV WordStrV;
    do {
      Lx.GetSym();
      if (Lx.Sym==tsyWord){
        WordStrV.Add(Lx.Str);}
    } while (Lx.Sym!=tsyDQuote);
    Lx.GetSym(Expect);
    return PTqlItem(new TTqlItem(WordStrV));
  } else
  if (Lx.Sym==tsyWord){
    PTqlItem Item=PTqlItem(new TTqlItem(Lx.Str));
    Lx.GetSym(TFSet(Expect)|tsyWCard);
    if (Lx.Sym==tsyWCard){
      PTqlItem WCardItem=
       PTqlItem(new TTqlItem(titWCard, Item));
      Item=WCardItem;
      Lx.GetSym(Expect);
    }
    return Item;
  } else {
    Fail; return NULL;
  }
}

PTqlItem TTqlExp::ParseTerm(TTqlLx& Lx, const TFSet& Expect){
  TFSet LoopExpect=TFSet(Expect)|FactExpect|tsyAnd;
  PTqlItem Item=ParseFact(Lx, LoopExpect);
  while ((Lx.Sym==tsyAnd)||(FactExpect.In(Lx.Sym))){
    if (Lx.Sym==tsyAnd){
      Lx.GetSym(FactExpect);}
    PTqlItem RItem=ParseFact(Lx, LoopExpect);
    Item=PTqlItem(new TTqlItem(titAnd, Item, RItem));
  }
  return Item;
}

PTqlItem TTqlExp::ParseExp(TTqlLx& Lx, const TFSet& Expect){
  PTqlItem Item=ParseTerm(Lx, TFSet(Expect)|tsyOr);
  while (Lx.Sym==tsyOr){
    Lx.GetSym(TermExpect);
    PTqlItem RItem=ParseTerm(Lx, TFSet(Expect)|tsyOr);
    Item=PTqlItem(new TTqlItem(titOr, Item, RItem));
  }
  return Item;
}

TTqlExp::TTqlExp(const TStr& _ExpStr):
  ExpStr(_ExpStr), Ok(true), ErrMsg(), Item(),
  RelOpSet(), FactExpect(), TermExpect(), ExpExpect(){
  // symbol sets
  RelOpSet=TFSet(tsyLss, tsyLEq, tsyEq, tsyNEq, tsyGEq, tsyGtr);
  FactExpect=TFSet(tsyNot, tsyIncl, tsyExcl, tsyTitle, tsyLParen, tsyLBracket,
   tsyDQuote, tsyWord);
  TermExpect=FactExpect;
  ExpExpect=TermExpect;

  TTqlLx Lx(ExpStr);
  try {
    Lx.GetSym(ExpExpect);
    Item=ParseExp(Lx, TFSet()|tsyEof);
  }
  catch (ETql E){
    Ok=false; ErrMsg=E.GetErrMsg();
  }
}

