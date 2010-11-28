/////////////////////////////////////////////////
// Wix-Expression-Lexical-Chars
class EWixExp{
private:
  TStr ErrMsg;
public:
  EWixExp(const TWixExp);
  EWixExp(const TStr& _ErrMsg):
    ErrMsg(_ErrMsg){}

  EWixExp& operator=(const EWixExp& E){
    ErrMsg=E.ErrMsg; return *this;}

  TStr GetErrMsg() const {return ErrMsg;}
};

/////////////////////////////////////////////////
// Wix-Expression-Lexical-Chars
void TWixExpLxChDef::SetChTy(const TWixExpLxChTy& ChTy, const TStr& Str){
  for (int ChN=0; ChN<Str.Len(); ChN++){ChTyV[Str[ChN]-TCh::Mn]=TInt(ChTy);}}

TWixExpLxChDef::TWixExpLxChDef():
  ChTyV(TCh::Vals){
  ChTyV.PutAll(TInt(welctAlpha));
  for (char Ch=TCh::NullCh; Ch<=' '; Ch++){SetChTy(welctSpace, TStr(Ch));}
  SetChTy(welctNum, "0123456789");
  SetChTy(welctSym, "&|!()\"+-*:");
  SetChTy(welctEof, TStr(TCh::EofCh));
}

/////////////////////////////////////////////////
// Wix-Expression-Lexical-Keywords
TWixExpKwMap::TWixExpKwMap():
  StrToKwH(20){
  StrToKwH.AddDat("domain", wekDomain);
  StrToKwH.AddDat("depth", wekDepth);
  StrToKwH.AddDat("feature", wekFeature);
  StrToKwH.AddDat("outgoingurl", wekOutgoingUrl);
  StrToKwH.AddDat("title", wekTitle);
  StrToKwH.AddDat("after", wekAfter);
  StrToKwH.AddDat("before", wekBefore);
  StrToKwH.AddDat("within", wekWithin);
}

bool TWixExpKwMap::IsExpKw(const TStr& ExpKwStr, TWixExpKw& ExpKw){
  int KeyId=StrToKwH.GetKeyId(ExpKwStr);
  if (KeyId==-1){return false;}
  else {ExpKw=TWixExpKw(int(StrToKwH[KeyId])); return true;}
}

/////////////////////////////////////////////////
// Wix-Expression-Lexical
TWixExpLxChDef TWixExpLx::ChDef;

TWixExpLx::TWixExpLx(const TStr& ExpStr):
  SIn(TStrIn::New(ExpStr)), Ch(' '),  Sym(wesyUndef), Str(){
}

TWixExpLxSym TWixExpLx::GetSym(const TFSet& Expect){
  Str.Clr();
  while (ChDef.IsSpace(Ch)){GetCh();}
  if (Expect.In(wesyNum)){
    if (!ChDef.IsNum(Ch)){throw EWixExp("Number expected.");}
    Sym=wesyNum;
    do {Str.AddCh(Ch); GetCh();} while (ChDef.IsNum(Ch));
  } else {
    switch (ChDef.GetChTy(Ch)){
      case welctAlpha:
      case welctNum:
        do {Str.AddCh(Ch); GetCh();} while (ChDef.IsAlNum(Ch));
        if (Str=="OR"){Sym=wesyOr;}
        else if (Str=="AND"){Sym=wesyAnd;}
        else if (Str=="NOT"){Sym=wesyNot;}
        else {Sym=wesyWord;}
        break;
      case welctSym:
        Str.AddCh(Ch);
        switch (Ch){
          case ':': Sym=wesyColon; break;
          case '(': Sym=wesyLParen; break;
          case ')': Sym=wesyRParen; break;
          case '|': Sym=wesyOr; break;
          case '&': Sym=wesyAnd; break;
          case '!': Sym=wesyNot; break;
          case '+': Sym=wesyIncl; break;
          case '-': Sym=wesyExcl; break;
          case '*': Sym=wesyWCard; break;
          default: Sym=wesySSym;
        }
        GetCh();
        break;
      case welctEof: Sym=wesyEof; break;
      default: Sym=wesyUndef; GetCh();
    }
  }

  if ((!Expect.In(Sym))&&(!Expect.Empty())){
    if (Sym==wesyEof){
      throw EWixExp("Unexpected end of expression.");
    } else {
      throw EWixExp("Unexpected symbol.");
    }
  }
  return Sym;
}

/////////////////////////////////////////////////
// Word-Inverted-Index-Document-Id-Set
PWixExpDocIdSet TWixExpDocIdSet::AndDocSet(const PWixExpDocIdSet& DocSet){
  PWixExpDocIdSet DstDocIdSet=PWixExpDocIdSet(new TWixExpDocIdSet());
  TBlobPtV& V1=DocIdV;
  TBlobPtV& V2=DocSet->DocIdV;
  TBlobPtV& DstV=DstDocIdSet->DocIdV;

  int N1=0; int N2=0;
  while ((N1<V1.Len())&&(N2<V2.Len())){
    TBlobPt& Val1=V1[N1];
    while ((N2<V2.Len())&&(Val1>V2[N2])){
      N2++;}
    if ((N2<V2.Len())&&(Val1==V2[N2])){
      TBlobPt Val(Val1); Val.MergeFlags(V2[N2]);
      DstV.Add(Val); N2++;
    }
    N1++;
  }
  return DstDocIdSet;
}

PWixExpDocIdSet TWixExpDocIdSet::OrDocSet(const PWixExpDocIdSet& DocSet){
  PWixExpDocIdSet DstDocIdSet=PWixExpDocIdSet(new TWixExpDocIdSet());
  TBlobPtV& V1=DocIdV;
  TBlobPtV& V2=DocSet->DocIdV;
  TBlobPtV& DstV=DstDocIdSet->DocIdV;

  int N1=0; int N2=0;
  while ((N1<V1.Len())&&(N2<V2.Len())){
    TBlobPt& Val1=V1[N1];
    TBlobPt& Val2=V2[N2];
    if (Val1<Val2){DstV.Add(Val1); N1++;}
    else if (Val1>Val2){DstV.Add(Val2); N2++;}
    else {TBlobPt Val(Val1); Val.MergeFlags(Val2); DstV.Add(Val); N1++; N2++;}
  }
  for (int RestN1=N1; RestN1<V1.Len(); RestN1++){DstV.Add(V1[RestN1]);}
  for (int RestN2=N2; RestN2<V2.Len(); RestN2++){DstV.Add(V2[RestN2]);}

  return DstDocIdSet;
}

/////////////////////////////////////////////////
// Wix-Expression-Item
TWixExpItem::TWixExpItem(const TWixExpItemType& _Type,
 const PWixExpItem& ExpItem1, const PWixExpItem& ExpItem2):
  Type(_Type), ExpItemV(), WordStrV(){
  IAssert(
   (Type==weitOr)||(Type==weitAnd)||(Type==weitNot)||
   (Type==weitIncl)||(Type==weitExcl)||(Type==weitWCard));
  if (Type==weitWCard){
    IAssert(ExpItem1->GetType()==weitWord);
    IAssert(ExpItem2.Empty());
  }
  ExpItemV.Add(ExpItem1);
  if (!ExpItem2.Empty()){ExpItemV.Add(ExpItem2);}
}

TWixExpItem::TWixExpItem(const TStr& WordStr):
  Type(weitWord), ExpItemV(), WordStrV(){
  WordStrV.Add(WordStr);
}

TWixExpItem::TWixExpItem(const TStrV& _WordStrV):
  Type(weitPhrase), ExpItemV(), WordStrV(_WordStrV){}

TWixExpItem::TWixExpItem(const TStr& MetaTag, const TStrV& ArgStrV):
  Type(weitMeta), ExpItemV(), WordStrV(){
  WordStrV.Add(MetaTag);
  WordStrV.AddV(ArgStrV);
}

PWixExpItem TWixExpItem::GetExpItem(const int& ExpItemN){
  IAssert(
   (Type==weitOr)||(Type==weitAnd)||(Type==weitNot)||
   (Type==weitIncl)||(Type==weitExcl)||(Type==weitWCard));
  return ExpItemV[ExpItemN];
}

TStr TWixExpItem::GetWordStr(){
  IAssert(Type==weitWord);
  return WordStrV[0];
}

int TWixExpItem::GetPhraseLen(){
  IAssert(Type==weitPhrase);
  return WordStrV.Len();
}

TStr TWixExpItem::GetPhraseWordStr(const int& WordStrN){
  IAssert(Type==weitPhrase);
  return WordStrV[WordStrN];
}

PWixExpDocIdSet TWixExpItem::Eval(const PWix& Wix){
  PWixExpDocIdSet DocIdSet;
  switch (Type){
    case weitOr:{
      DocIdSet=GetExpItem(0)->Eval(Wix);
      PWixExpDocIdSet RDocIdSet=GetExpItem(1)->Eval(Wix);
      DocIdSet=DocIdSet->OrDocSet(RDocIdSet);
      break;}
    case weitAnd:{
      DocIdSet=GetExpItem(0)->Eval(Wix);
      PWixExpDocIdSet RDocIdSet=GetExpItem(1)->Eval(Wix);
      DocIdSet=DocIdSet->AndDocSet(RDocIdSet);
      break;}
    case weitNot:{
      DocIdSet=GetExpItem()->Eval(Wix);
      DocIdSet->PutNegated(!DocIdSet->IsNegated());
      break;}
    case weitIncl: break; //**
    case weitExcl: break; //**
    case weitWCard:{
      PWixExpItem SubExpItem=GetExpItem();
      TBlobPtV DocIdV;
      Wix->GetDocIdV(SubExpItem->GetWordStr(), false, 100, DocIdV);
      DocIdSet=PWixExpDocIdSet(new TWixExpDocIdSet(DocIdV));
      break;}
    case weitWord:{
      TBlobPtV DocIdV;
      Wix->GetDocIdV(GetWordStr(), true, -1, DocIdV);
      DocIdSet=PWixExpDocIdSet(new TWixExpDocIdSet(DocIdV));
      break;}
    case weitPhrase: break; //**
    case weitMeta: break; //**
    default: Fail;
  }
  return DocIdSet;
}

/////////////////////////////////////////////////
// Wix-Expression
PWixExpItem TWixExp::ParseFact(TWixExpLx& Lx, const TFSet& Expect){
  if (Lx.Sym==wesyNot){
    Lx.GetSym(FactExpect);
    PWixExpItem ExpItem=ParseFact(Lx, Expect);
    return PWixExpItem(new TWixExpItem(weitNot, ExpItem));
  } else
  if (Lx.Sym==wesyIncl){
    Lx.GetSym(FactExpect);
    PWixExpItem ExpItem=ParseFact(Lx, Expect);
    return PWixExpItem(new TWixExpItem(weitIncl, ExpItem));
  } else
  if (Lx.Sym==wesyExcl){
    Lx.GetSym(FactExpect);
    PWixExpItem ExpItem=ParseFact(Lx, Expect);
    return PWixExpItem(new TWixExpItem(weitExcl, ExpItem));
  } else
  if (Lx.Sym==wesyLParen){
    Lx.GetSym(ExpExpect);
    PWixExpItem ExpItem=ParseExp(Lx, TFSet()|wesyRParen);
    Lx.GetSym(Expect);
    return ExpItem;
  } else
  if (Lx.Sym==wesyDQuote){
    TStrV WordStrV;
    Lx.GetSym(TFSet()|wesyWord|wesyDQuote);
    while (Lx.Sym==wesyWord){
      WordStrV.Add(Lx.Str);
      Lx.GetSym(TFSet()|wesyWord|wesyDQuote);
    }
    Lx.GetSym(Expect);
    return PWixExpItem(new TWixExpItem(WordStrV));
  } else
  if (Lx.Sym==wesyWord){
    PWixExpItem ExpItem=PWixExpItem(new TWixExpItem(Lx.Str));
    Lx.GetSym(TFSet(Expect)|wesyWCard);
    if (Lx.Sym==wesyWCard){
      PWixExpItem WCardExpItem=
       PWixExpItem(new TWixExpItem(weitWCard, ExpItem));
      ExpItem=WCardExpItem;
      Lx.GetSym(Expect);
    }
    return ExpItem;
  } else {
    Fail; return NULL;
  }
}

PWixExpItem TWixExp::ParseTerm(TWixExpLx& Lx, const TFSet& Expect){
  TFSet LoopExpect=TFSet(Expect)|FactExpect|wesyAnd;
  PWixExpItem ExpItem=ParseFact(Lx, LoopExpect);
  while ((Lx.Sym==wesyAnd)||(FactExpect.In(Lx.Sym))){
    if (Lx.Sym==wesyAnd){
      Lx.GetSym(FactExpect);}
    PWixExpItem RExpItem=ParseFact(Lx, LoopExpect);
    ExpItem=PWixExpItem(new TWixExpItem(weitAnd, ExpItem, RExpItem));
  }
  return ExpItem;
}

PWixExpItem TWixExp::ParseExp(TWixExpLx& Lx, const TFSet& Expect){
  PWixExpItem ExpItem=ParseTerm(Lx, TFSet(Expect)|wesyOr);
  while (Lx.Sym==wesyOr){
    Lx.GetSym(TermExpect);
    PWixExpItem RExpItem=ParseTerm(Lx, TFSet(Expect)|wesyOr);
    ExpItem=PWixExpItem(new TWixExpItem(weitOr, ExpItem, RExpItem));
  }
  return ExpItem;
}

TWixExp::TWixExp(const TStr& _ExpStr):
  ExpStr(_ExpStr), Ok(true), ErrMsg(), ExpItem(),
  FactExpect(), TermExpect(), ExpExpect(){
  // prepare expect sets
  FactExpect=TFSet()|wesyNot|wesyIncl|wesyExcl|wesyLParen|wesyDQuote|wesyWord;
  TermExpect=FactExpect;
  ExpExpect=TermExpect;
  // parse the expression string
  TWixExpLx Lx(ExpStr);
  try {
    Lx.GetSym(ExpExpect);
    ExpItem=ParseExp(Lx, TFSet()|wesyEof);
  }
  catch (EWixExp E){
    Ok=false; ErrMsg=E.GetErrMsg();
  }
}

PWixExpDocIdSet TWixExp::Eval(const PWix& Wix){
  PWixExpDocIdSet DocIdSet=ExpItem->Eval(Wix);
  return DocIdSet;
}

