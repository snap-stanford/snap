/////////////////////////////////////////////////
// Xml-Object-Saving
TStrStrH TXmlObjSer::TypeNmToTagNmH;

TStr TXmlObjSer::GetTagNm(const TStr& TypeNm){
  TStr& XmlTagNm=TypeNmToTagNmH.AddDat(TypeNm);
  if (XmlTagNm.Empty()){
    TChA XmlTagChA=TypeNm;
    for (int ChN=0; ChN<XmlTagChA.Len(); ChN++){
      char Ch=XmlTagChA[ChN];
      if (!(('A'<=Ch)&&(Ch<='Z')||('a'<=Ch)&&(Ch<='z')||('0'<=Ch)&&(Ch<='9'))){
        XmlTagChA.PutCh(ChN, '_');
      }
    }
    while ((XmlTagChA.Len()>0)&&(XmlTagChA.LastCh()=='_')){
      XmlTagChA.Pop();}
    XmlTagNm=XmlTagChA;
  }
  return XmlTagNm;
}

void TXmlObjSer::AssertXmlHd(
 const PXmlTok& XmlTok, const TStr& Nm, const TStr& TypeNm){
  // check if the token is full
  EAssertR(!XmlTok.Empty(), "Xml-Token Empty");
  // if name is empty then tag=type else tag=name
  if (!Nm.Empty()){
    // check if the token is tag
    if (!XmlTok->IsTag()){
      TStr ArgStr1="Expected: Tag";
      TStr ArgStr2=TStr("Found: ")+XmlTok->GetSymStr();
      TExcept::Throw("Invalid Xml-Token", ArgStr1, ArgStr2);
    }
    if (Nm!="-"){
      // check if the tag is correct
      if (!XmlTok->IsTag(Nm)){
        TStr ArgStr1=TStr("Expected: ")+Nm;
        TStr ArgStr2=TStr("Found: ")+XmlTok->GetStr();
        TExcept::Throw("Invalid Xml-Tag", ArgStr1, ArgStr2);
      }
      // check if the type is correct
      TStr TypeArgVal=XmlTok->GetStrArgVal("Type");
      if (TypeArgVal!=TypeNm){
        TStr ArgStr1=TStr("Expected: ")+TypeNm;
        TStr ArgStr2=TStr("Found: ")+TypeArgVal;
        TExcept::Throw("Invalid Xml-Type", ArgStr1, ArgStr2);
      }
    }
  } else {
    // check if the tag is correct
    if (!XmlTok->IsTag(TypeNm)){
      TStr ArgStr1=TStr("Expected: ")+TypeNm;
      TStr ArgStr2=TStr("Found: ")+XmlTok->GetSymStr();
      TExcept::Throw("Invalid Xml-Type-Tag", ArgStr1, ArgStr2);
    }
  }
}

bool TXmlObjSer::GetBoolArg(const PXmlTok& XmlTok, const TStr& Nm){
  TStr ValStr;
  if (XmlTok->IsArg(Nm, ValStr)){
    bool Val;
    if (ValStr.IsBool(Val)){
      return Val;
    } else {
      TExcept::Throw("Invalid Xml-Argument Boolean-Value", Nm, ValStr);
    }
  } else {
    TExcept::Throw("Xml-Argument Missing", Nm);
  }
  Fail; return 0;
}

int TXmlObjSer::GetIntArg(const PXmlTok& XmlTok, const TStr& Nm){
  TStr ValStr;
  if (XmlTok->IsArg(Nm, ValStr)){
    int Val;
    if (ValStr.IsInt(Val)){
      return Val;
    } else {
      TExcept::Throw("Invalid Xml-Argument Integer-Value", Nm, ValStr);
    }
  } else {
    TExcept::Throw("Xml-Argument Missing", Nm);
  }
  Fail; return 0;
}

int64 TXmlObjSer::GetInt64Arg(const PXmlTok& XmlTok, const TStr& Nm){
  TStr ValStr;
  if (XmlTok->IsArg(Nm, ValStr)){
    int64 Val;
    if (ValStr.IsInt64(Val)){
      return Val;
    } else {
      TExcept::Throw("Invalid Xml-Argument Integer64-Value", Nm, ValStr);
    }
  } else {
    TExcept::Throw("Xml-Argument Missing", Nm);
  }
  Fail; return 0;
}

double TXmlObjSer::GetFltArg(const PXmlTok& XmlTok, const TStr& Nm){
  TStr ValStr;
  if (XmlTok->IsArg(Nm, ValStr)){
    double Val;
    if (ValStr.IsFlt(Val)){
      return Val;
    } else {
      TExcept::Throw("Invalid Xml-Argument Double-Value", Nm, ValStr);
    }
  } else {
    TExcept::Throw("Xml-Argument Missing", Nm);
  }
  Fail; return 0;
}

/////////////////////////////////////////////////
// Xml-Object-Serialization-Tag-Name
TXmlObjSerTagNm::TXmlObjSerTagNm(
 TSOut& _SOut, const bool& ETagP,
 const TStr& Nm, const TStr& TypeNm,
 const TStr& ArgNm, const TStr& ArgVal):
  TagNm(), SOut(&_SOut){
  if (Nm!="-"){
    SOut->PutCh('<');
    if (Nm.Empty()){
      SOut->PutStr(TagNm=TypeNm);
    } else {
      SOut->PutStr(TagNm=Nm);
      SOut->PutStr(" Type=\""); SOut->PutStr(TypeNm); SOut->PutCh('"');
    }
    if (!ArgNm.Empty()){
      SOut->PutCh(' '); SOut->PutStr(ArgNm); SOut->PutCh('=');
      SOut->PutCh('"'); SOut->PutStr(ArgVal); SOut->PutCh('"');
    }
    if (ETagP){
      SOut->PutCh('/'); TagNm="";}
    SOut->PutCh('>');
  }
}

TXmlObjSerTagNm::TXmlObjSerTagNm(
 TSOut& _SOut, const bool& ETagP,
 const TStr& Nm, const TStr& TypeNm,
 const TStr& ArgNm1, const TStr& ArgVal1,
 const TStr& ArgNm2, const TStr& ArgVal2,
 const TStr& ArgNm3, const TStr& ArgVal3,
 const TStr& ArgNm4, const TStr& ArgVal4):
  TagNm(), SOut(&_SOut){
  if (Nm!="-"){
    SOut->PutCh('<');
    if (Nm.Empty()){
      SOut->PutStr(TagNm=TypeNm);
    } else {
      SOut->PutStr(TagNm=Nm);
      SOut->PutStr(" Type=\""); SOut->PutStr(TypeNm); SOut->PutCh('"');
    }
    if (!ArgNm1.Empty()){
      SOut->PutCh(' '); SOut->PutStr(ArgNm1); SOut->PutCh('=');
      SOut->PutCh('"'); SOut->PutStr(ArgVal1); SOut->PutCh('"');
    }
    if (!ArgNm2.Empty()){
      SOut->PutCh(' '); SOut->PutStr(ArgNm2); SOut->PutCh('=');
      SOut->PutCh('"'); SOut->PutStr(ArgVal2); SOut->PutCh('"');
    }
    if (!ArgNm3.Empty()){
      SOut->PutCh(' '); SOut->PutStr(ArgNm3); SOut->PutCh('=');
      SOut->PutCh('"'); SOut->PutStr(ArgVal3); SOut->PutCh('"');
    }
    if (!ArgNm4.Empty()){
      SOut->PutCh(' '); SOut->PutStr(ArgNm4); SOut->PutCh('=');
      SOut->PutCh('"'); SOut->PutStr(ArgVal4); SOut->PutCh('"');
    }
    if (ETagP){
      SOut->PutCh('/'); TagNm="";}
    SOut->PutCh('>');
  }
}

TXmlObjSerTagNm::~TXmlObjSerTagNm(){
  if (!TagNm.Empty()){
    SOut->PutCh('<'); SOut->PutCh('/'); SOut->PutStr(TagNm); SOut->PutCh('>');
  }
}

/////////////////////////////////////////////////
// Xml-Chars
void TXmlChDef::SetChTy(TBSet& ChSet, const int& MnCh, const int& MxCh){
  IAssert((0<=MnCh)&&((MxCh==-1)||((MnCh<=MxCh)&&(MxCh<Chs))));
  ChSet.Incl(MnCh);
  for (int Ch=MnCh+1; Ch<=MxCh; Ch++){
    ChSet.Incl(Ch);}
}

void TXmlChDef::SetChTy(TBSet& ChSet, const TStr& Str){
  for (int ChN=0; ChN<Str.Len(); ChN++){
    uchar Ch=Str[ChN];
    ChSet.Incl(Ch);
  }
}

void TXmlChDef::SetEntityVal(const TStr& Nm, const TStr& Val){
  EntityNmToValH.AddDat(Nm, Val);
}

TXmlChDef::TXmlChDef():
  Chs(TUCh::Vals),
  CharChSet(), CombChSet(), ExtChSet(),
  LetterChSet(), DigitChSet(), NameChSet(), PubidChSet(),
  EntityNmToValH(100){

  // Character-Sets
  // Char ::= #x9 | #xA | #xD | [#x20-#xD7FF] | ...
  CharChSet.Gen(Chs);
  // ... because of DMoz (temporary patch)
  SetChTy(CharChSet, 0x1); SetChTy(CharChSet, 0x3); SetChTy(CharChSet, 0x6);
  SetChTy(CharChSet, 11); SetChTy(CharChSet, 24); SetChTy(CharChSet, 27);
  // regular characters
  SetChTy(CharChSet, 0x9); SetChTy(CharChSet, 0xA); SetChTy(CharChSet, 0xD);
  SetChTy(CharChSet, 0x20, TUCh::Mx);
  // BaseChar ::=  [#x0041-#x005A] | [#x0061-#x007A] | [#x00C0-#x00D6] |
  //  [#x00D8-#x00F6] | [#x00F8-#x00FF] | ...
  TBSet BaseChSet(Chs);
  SetChTy(BaseChSet, 0x41, 0x5A); SetChTy(BaseChSet, 0x61, 0x7A);
  SetChTy(BaseChSet, 0xC0, 0xD6); SetChTy(BaseChSet, 0xD8, 0xF6);
  SetChTy(BaseChSet, 0xF8, 0xFF);
  // Ideographic ::= ...
  TBSet IdeoChSet(Chs);
  // CombiningChar ::= ...
  CombChSet.Gen(Chs);
  // Extender ::=  #x00B7 | ...
  ExtChSet.Gen(Chs);
  SetChTy(ExtChSet, 0xB7);
  // Letter ::=  BaseChar | Ideographic
  LetterChSet=BaseChSet|IdeoChSet;
  // Digit ::=  [#x0030-#x0039] | ...
  DigitChSet.Gen(Chs);
  SetChTy(DigitChSet, 0x30, 0x39);
  // NameChar ::=  Letter | Digit | '.' | '-' | '_' | ':' | CombiningChar
  NameChSet=LetterChSet|DigitChSet|
   uchar('.')|uchar('-')|uchar('_')|uchar(':')|CombChSet;
  // PubidChar ::=  #x20 | #xD | #xA | [a-zA-Z0-9] | [-'()+,./:=?;!*#@$_%]
  PubidChSet.Gen(Chs);
  SetChTy(PubidChSet, 0x20); SetChTy(PubidChSet, 0xD); SetChTy(PubidChSet, 0xA);
  SetChTy(PubidChSet, 'a', 'z'); SetChTy(PubidChSet, 'A', 'Z');
  SetChTy(PubidChSet, '0', '9'); SetChTy(PubidChSet, "-'()+,./:=?;!*#@$_%");

  // Standard-Entity-Sequences
  SetEntityVal("amp", "&");
  SetEntityVal("lt", "<"); SetEntityVal("gt", ">");
  SetEntityVal("apos", "'"); SetEntityVal("quot", "\"");
}

/////////////////////////////////////////////////
// Xml-Lexical
TXmlChDef TXmlLx::ChDef;

uchar TXmlLx::GetCh(){
  EAssert(Ch!=TCh::EofCh);
  PrevCh=Ch;
  if (ChStack.Empty()){Ch=(RSIn.Eof()) ? TCh::EofCh : RSIn.GetCh();}
  else {Ch=ChStack.Pop();}
  ChN++; if (Ch==TCh::LfCh){LnN++; LnChN=0;} else {LnChN++;}
  //putchar(Ch);
  return Ch;
}

void TXmlLx::ToNrSpacing(){
  if (Spacing==xspIntact){
  } else
  if (Spacing==xspPreserve){
    int SrcChN=0; int DstChN=0;
    while (SrcChN<TxtChA.Len()){
      if (TxtChA[SrcChN]==TCh::CrCh){
        TxtChA.PutCh(DstChN, TCh::LfCh); SrcChN++; DstChN++;
        if ((SrcChN<TxtChA.Len())&&(TxtChA[SrcChN]==TCh::LfCh)){SrcChN++;}
      } else {
        if (SrcChN!=DstChN){
          TxtChA.PutCh(DstChN, TxtChA[SrcChN]);}
        SrcChN++; DstChN++;
      }
    }
    TxtChA.Trunc(DstChN);
  } else
  if (Spacing==xspSeparate){
    // squeeze series of white-spaces to single space
    int SrcChN=0; int DstChN=0;
    while (SrcChN<TxtChA.Len()){
      if (ChDef.IsWs(TxtChA[SrcChN])){
        if ((DstChN>0)&&(TxtChA[DstChN-1]==' ')){
          SrcChN++;
        } else {
          TxtChA.PutCh(DstChN, ' ');
          SrcChN++; DstChN++;
        }
      } else {
        TxtChA.PutCh(DstChN, TxtChA[SrcChN]);
        SrcChN++; DstChN++;
      }
    }
    TxtChA.Trunc(DstChN);
  } else
  if (Spacing==xspTruncate){
    // cut leading and trailing white-spaces and
    // squeeze series of white-spaces to single space
    int SrcChN=0; int DstChN=0;
    while (SrcChN<TxtChA.Len()){
      if (ChDef.IsWs(TxtChA[SrcChN])){
        if ((DstChN>0)&&(TxtChA[DstChN-1]==' ')){
          SrcChN++;
        } else {
          TxtChA.PutCh(DstChN, ' ');
          SrcChN++; DstChN++;
        }
      } else {
        TxtChA.PutCh(DstChN, TxtChA[SrcChN]);
        SrcChN++; DstChN++;
      }
    }
    TxtChA.Trunc(DstChN);
    // delete trailing white-spaces
    while ((TxtChA.Len()>0)&&(ChDef.IsWs(TxtChA.LastCh()))){
      TxtChA.Trunc(TxtChA.Len()-1);}
  } else {
    Fail;
  }
}

void TXmlLx::GetWs(const bool& IsRq){
  // [3] S ::=  (#x20 | #x9 | #xD | #xA)+
  int WSpaces=0; TxtChA.Clr();
  while (ChDef.IsWs(Ch)){
    WSpaces++; TxtChA+=Ch; GetCh();}
  if (IsRq&&(WSpaces==0)){
    EThrow("White-space required.");}
}

TStr TXmlLx::GetReference(){
  // [67] Reference ::=  EntityRef | CharRef
  if (Ch=='#'){
    // [66]  CharRef ::=  '&#' [0-9]+ ';' | '&#x' [0-9a-fA-F]+ ';'
    TChA RefChA; int RefCd=0;
    if (GetCh()=='x'){
      // hex-decimal character code
      forever {
        GetCh();
        if (TCh::IsHex(Ch)){
          RefChA+=Ch;
          RefCd=RefCd*16+TCh::GetHex(Ch);
        } else {
          break;
        }
      }
    } else {
      // decimal character code
      forever {
        if (TCh::IsNum(Ch)){
          RefChA+=Ch;
          RefCd=RefCd*10+TCh::GetNum(Ch);
        } else {
          break;
        }
        GetCh();
      }
    }
    if ((!RefChA.Empty())&&(Ch==';')){
      GetCh();
      uchar RefCh=uchar(RefCd);
      return TStr(RefCh);
    } else {
      EThrow("Invalid Char-Reference."); Fail; return TStr();
    }
  } else {
    // [68]  EntityRef ::=  '&' Name ';'
    TStr EntityNm=GetName();
    if ((!EntityNm.Empty())&&(Ch==';')){
      GetCh();
      TStr EntityVal;
      if (IsEntityNm(EntityNm, EntityVal)){/*intentionaly empty*/}
      else if (ChDef.IsEntityNm(EntityNm, EntityVal)){/*intentionaly empty*/}
      else {EThrow(TStr("Entity-Reference (")+EntityNm+") does not exist.");}
      return EntityVal;
    } else {
      EThrow("Invalid Entity-Reference."); Fail; return TStr();
    }
  }
}

TStr TXmlLx::GetPEReference(){
  // [69]  PEReference ::=  '%' Name ';'
  TStr EntityNm=GetName();
  if ((EntityNm.Empty())||(Ch!=';')){EThrow("Invalid PEntity-Reference.");}
  GetCh();
  TStr EntityVal;
  if (IsPEntityNm(EntityNm, EntityVal)){/*intentionaly empty*/}
  else {EThrow(TStr("PEntity-Reference (")+EntityNm+") does not exist.");}
  return EntityVal;
}

void TXmlLx::GetEq(){
  // [25] Eq ::=  S? '=' S?
  GetWs(false);
  if (Ch=='='){GetCh();}
  else {EThrow("Equality ('=') character expected.");}
  GetWs(false);
}

TStr TXmlLx::GetName(){
  // [5] Name ::=  (Letter | '_' | ':') (NameChar)*
  TChA NmChA;
  if (ChDef.IsFirstNameCh(Ch)){
    do {NmChA+=Ch;} while (ChDef.IsName(GetCh()));
  } else {
    EThrow("Invalid first name character.");
    // EThrow(TStr::Fmt("Invalid first name character [%u:'%c%c%c%c%c'].", 
    //  uint(Ch), Ch, RSIn.GetCh(), RSIn.GetCh(), RSIn.GetCh(), RSIn.GetCh()));
  }
  return NmChA;
}

TStr TXmlLx::GetName(const TStr& RqNm){
  TStr Nm=GetName();
  // test if the name is equal to the required name
  if (Nm==RqNm){return RqNm;}
  else {EThrow(TStr("Name '")+RqNm+"' expected."); Fail; return TStr();}
}

void TXmlLx::GetComment(){
  // [15] Comment ::=  {{'<!-}}-' ((Char - '-') | ('-' (Char - '-')))* '-->'
  if (GetCh()!='-'){EThrow("Invalid comment start.");}
  TxtChA.Clr();
  forever {
    GetCh();
    if (!ChDef.IsChar(Ch)){EThrow("Invalid comment character.");}
    if (Ch=='-'){
      if (GetCh()=='-'){
        if (GetCh()=='>'){GetCh(); break;} // final bracket
        else {EThrow("Invalid comment end.");}
      } else {
        if (!ChDef.IsChar(Ch)){EThrow("Invalid comment character.");}
        TxtChA+='-'; TxtChA+=Ch; // special case if single '-'
      }
    } else {
      TxtChA+=Ch; // usual char
    }
  }
}

TStr TXmlLx::GetAttValue(){
  // [10]  AttValue ::=  '"' ([^<&"] | Reference)* '"'
  //  |  "'" ([^<&'] | Reference)* "'"
  uchar QCh=Ch;
  if ((QCh!='"')&&(QCh!='\'')){EThrow("Invalid attribute-value start.");}
  TChA ValChA; GetCh();
  forever {
    if ((Ch=='<')||(!ChDef.IsChar(Ch))){
      EThrow("Invalid attribute-value character.");}
    if (Ch==QCh){GetCh(); break;} // final quote
    else if (Ch=='&'){GetCh(); ValChA+=GetReference();} // reference
    else {ValChA+=Ch; GetCh();} // usual char
  }
  return ValChA;
}

TStr TXmlLx::GetVersionNum(){
  // [24] VersionInfo ::=  {{S 'version' Eq}} (' VersionNum ' | " VersionNum ")
  // [26] VersionNum ::=  ([a-zA-Z0-9_.:] | '-')+
  char QCh=Ch;
  if ((Ch!='\'')&&(Ch!='"')){EThrow("Quote character (' or \") expected.");}
  TChA VerNumChA;
  GetCh();
  do {
    if ((('a'<=Ch)&&(Ch<='z'))||(('A'<=Ch)&&(Ch<='Z'))||
     (('0'<=Ch)&&(Ch<='9'))||(Ch=='_')||(Ch=='.')||(Ch==':')||(Ch=='-')){
      VerNumChA+=Ch;
    } else {
      EThrow("Invalid version-number character.");
    }
    GetCh();
  } while (Ch!=QCh);
  GetCh();
  return VerNumChA;
}

TStr TXmlLx::GetEncName(){
  // [80] EncodingDecl ::=  {{S 'encoding' Eq}} ('"' EncName '"' |  "'" EncName "'" )
  // [81] EncName ::=  [A-Za-z] ([A-Za-z0-9._] | '-')*
  char QCh=Ch;
  if ((Ch!='\'')&&(Ch!='"')){EThrow("Quote character (' or \") expected.");}
  TChA EncNmChA;
  GetCh();
  if ((('a'<=Ch)&&(Ch<='z'))||(('A'<=Ch)&&(Ch<='Z'))){EncNmChA+=Ch;}
  else {EThrow("Invalid encoding-name character.");}
  GetCh();
  while (Ch!=QCh){
    if ((('a'<=Ch)&&(Ch<='z'))||(('A'<=Ch)&&(Ch<='Z'))||
     (('0'<=Ch)&&(Ch<='9'))||(Ch=='.')||(Ch=='_')||(Ch=='-')){EncNmChA+=Ch;}
    else {EThrow("Invalid version-number character.");}
    GetCh();
  }
  GetCh();
  return EncNmChA;
}

TStr TXmlLx::GetStalVal(){
  // [32] SDDecl ::=  {{S 'standalone' Eq}}
  //  (("'" ('yes' | 'no') "'") | ('"' ('yes' | 'no') '"'))
  char QCh=Ch;
  if ((Ch!='\'')&&(Ch!='"')){EThrow("Quote character (' or \") expected.");}
  TChA StalChA;
  GetCh();
  while (Ch!=QCh){
    if (('a'<=Ch)&&(Ch<='z')){StalChA+=Ch;}
    else {EThrow("Invalid standalone-value character.");}
    GetCh();
  }
  GetCh();
  TStr StalVal=StalChA;
  if ((StalVal=="yes")||(StalVal=="no")){return StalVal;}
  else {EThrow("Invalid standalone-value."); Fail; return TStr();}
}

void TXmlLx::GetXmlDecl(){
  // [23] XMLDecl ::=  {{'<?xml'}}... VersionInfo EncodingDecl? SDDecl? S? '?>'
  // [24] VersionInfo ::=  S 'version' Eq (' VersionNum ' | " VersionNum ")
  GetWs(true);
  TStr VerNm=GetName("version"); GetEq(); TStr VerVal=GetVersionNum();
  if (VerVal!="1.0"){EThrow("Invalid XML version.");}
  AddArg(VerNm, VerVal);
  GetWs(false);
  if (Ch!='?'){
    // EncodingDecl ::=  {{S}} 'encoding' Eq
    //  ('"' EncName '"' |  "'" EncName "'" )
    TStr EncNm=GetName("encoding"); GetEq(); TStr EncVal=GetEncName();
    AddArg(EncNm, EncVal);
  }
  GetWs(false);
  if (Ch!='?'){
    // SDDecl ::=  {{S}} 'standalone' Eq
    //  (("'" ('yes' | 'no') "'") | ('"' ('yes' | 'no') '"'))
    TStr StalNm=GetName("standalone"); GetEq(); TStr StalVal=GetStalVal();
    AddArg(StalNm, StalVal);
  }
  GetWs(false);
  if (Ch=='?'){
    GetCh();
    if (Ch=='>'){GetCh();}
    else {EThrow("Invalid end-of-tag in XML-declaration.");}
  } else {
    EThrow("Invalid end-of-tag in XML-declaration.");
  }
}

void TXmlLx::GetPI(){
  // [16]  PI ::=  {{'<?' PITarget}} (S (Char* - (Char* '?>' Char*)))? '?>'
  // [17]  PITarget ::=  Name - (('X' | 'x') ('M' | 'm') ('L' | 'l'))
  GetWs(false);
  TxtChA.Clr();
  forever {
    if (!ChDef.IsChar(Ch)){EThrow("Invalid PI character.");}
    if (Ch=='?'){
      if (GetCh()=='>'){
        GetCh(); break;
      } else {
        if (!ChDef.IsChar(Ch)){EThrow("Invalid PI character.");}
        TxtChA+='?'; TxtChA+=Ch; // special case if single '?'
      }
    } else {
      TxtChA+=Ch; // usual char
    }
    GetCh();
  }
}

TStr TXmlLx::GetSystemLiteral(){
  // [11]  SystemLiteral ::=  ('"' [^"]* '"') | ("'" [^']* "'")
  char QCh=Ch;
  if ((Ch!='\'')&&(Ch!='"')){EThrow("Quote character (' or \") expected.");}
  TChA LitChA; GetCh();
  while (Ch!=QCh){
    if (!ChDef.IsChar(Ch)){EThrow("Invalid System-Literal character.");}
    LitChA+=Ch; GetCh();
  }
  GetCh();
  return LitChA;
}

TStr TXmlLx::GetPubidLiteral(){
  // [12]  PubidLiteral ::=  '"' PubidChar* '"' | "'" (PubidChar - "'")* "'"
  char QCh=Ch;
  if ((Ch!='\'')&&(Ch!='"')){EThrow("Quote character (' or \") expected.");}
  TChA LitChA; GetCh();
  while (Ch!=QCh){
    if (!ChDef.IsPubid(Ch)){EThrow("Invalid Public-Id-Literal character.");}
    LitChA+=Ch; GetCh();
  }
  GetCh();
  return LitChA;
}

void TXmlLx::GetExternalId(){
  // ExternalID ::=  'SYSTEM' S SystemLiteral
  //  | 'PUBLIC' S PubidLiteral S SystemLiteral
  TStr ExtIdNm=GetName();
  if (ExtIdNm=="SYSTEM"){
    GetWs(true); GetSystemLiteral();
  } else if (ExtIdNm=="PUBLIC"){
    GetWs(true); GetPubidLiteral(); GetWs(true); GetSystemLiteral();
  } else {
    EThrow("Invalid external-id ('SYSTEM' or 'PUBLIC' expected).");
  }
}

void TXmlLx::GetNData(){
  // [76]  NDataDecl ::=  S 'NDATA' S Name
  GetName("NDATA"); GetWs(true); GetName();
}

void TXmlLx::GetDocTypeDecl(){
  // [28] doctypedecl ::=  {{'<!DOCTYPE'}} S Name (S ExternalID)? S?
  //  ('[' (markupdecl | PEReference | S)* ']' S?)? '>'
  GetWs(true);
  TStr DocTypeDeclNm=GetName();
  GetWs(false);
  if (Ch=='>'){GetCh(); return;}
  if (Ch!='['){GetExternalId();}
  GetWs(false);
  if (Ch=='['){
    GetCh();
    // [28] (markupdecl | PEReference | S)*
    GetWs(false);
    while (Ch!=']'){
      if (ChDef.IsWs(Ch)){GetWs(true);}
      else if (Ch=='%'){GetPEReference();}
      else {
        GetSym();
      }
    }
    GetCh();
  }
  GetWs(false);
  // '>'
  if (Ch=='>'){GetCh();}
  else {EThrow("Invalid end-of-tag in document-type-declaration.");}
  TagNm=DocTypeDeclNm;
}

void TXmlLx::GetElement(){
  TxtChA.Clr();
  while (Ch!='>'){
    if (!ChDef.IsChar(Ch)){EThrow("Invalid Element character.");}
    TxtChA+=Ch; GetCh();
  }
  GetCh();
}

void TXmlLx::GetAttList(){
  TxtChA.Clr();
  while (Ch!='>'){
    if (!ChDef.IsChar(Ch)){EThrow("Invalid Element character.");}
    TxtChA+=Ch; GetCh();
  }
  GetCh();
}

TStr TXmlLx::GetEntityValue(){
  // [9]  EntityValue ::=  '"' ([^%&"] | PEReference | Reference)* '"'
  //  | "'" ([^%&'] | PEReference | Reference)* "'"
  uchar QCh=Ch;
  if ((QCh!='"')&&(QCh!='\'')){EThrow("Invalid entity-value start.");}
  TChA ValChA; GetCh();
  forever {
    if (!ChDef.IsChar(Ch)){EThrow("Invalid entity-value character.");}
    if (Ch==QCh){GetCh(); break;} // final quote
    else if (Ch=='&'){GetCh(); ValChA+=GetReference();} // reference
    else if (Ch=='%'){GetCh(); ValChA+=GetPEReference();} // pereference
    else {ValChA+=Ch; GetCh();} // usual char
  }
  return ValChA;
}

void TXmlLx::GetEntity(){
  // [70] EntityDecl ::=  GEDecl | PEDecl
  // [71] GEDecl ::=  '<!ENTITY' S Name S EntityDef S? '>'
  // [72] PEDecl ::=  '<!ENTITY' S '%' S Name S PEDef S? '>'
  GetWs(true); TStr EntityNm;
  if (Ch=='%'){
    GetCh(); GetWs(true); EntityNm=GetName(); GetWs(true);
    // [74] PEDef ::=  EntityValue | ExternalID
    if ((Ch=='\"')||(Ch=='\'')){
      TStr EntityVal=GetEntityValue();
      PutPEntityVal(EntityNm, EntityVal);
    } else {
      GetExternalId();
      GetWs(false);
      if (Ch!='>'){GetNData();}
    }
  } else {
    EntityNm=GetName(); GetWs(true);
    // [73] EntityDef ::=  EntityValue | (ExternalID NDataDecl?)
    if ((Ch=='\"')||(Ch=='\'')){
      TStr EntityVal=GetEntityValue();
      PutEntityVal(EntityNm, EntityVal);
    } else {
      GetExternalId();
    }
  }
  GetWs(false);
  if (Ch=='>'){GetCh();}
  else {EThrow("Invalid end-of-tag in entity-declaration.");}
  TagNm=EntityNm;
}

void TXmlLx::GetNotation(){
  // [82] NotationDecl ::=  '<!NOTATION' S Name S (ExternalID |  PublicID) S? '>'
  // [83]  PublicID ::=  'PUBLIC' S PubidLiteral
  TxtChA.Clr();
  while (Ch!='>'){
    if (!ChDef.IsChar(Ch)){EThrow("Invalid Element character.");}
    TxtChA+=Ch; GetCh();
  }
  GetCh();
}

void TXmlLx::GetCDSect(){
  // [18]  CDSect ::=  CDStart CData CDEnd
  // [19]  CDStart ::=  '<![CDATA{{['}}
  // [20]  CData ::=  (Char* - (Char* ']]>' Char*))
  // [21]  CDEnd ::=  ']]>'
  if (Ch=='['){GetCh();}
  else {EThrow("Invalid start of CDATA section.");}
  TxtChA.Clr();
  forever {
    if (!ChDef.IsChar(Ch)){EThrow("Invalid CDATA character.");}
    if ((Ch=='>')&&(TxtChA.Len()>=2)&&
     (TxtChA[TxtChA.Len()-2]==']')&&(TxtChA[TxtChA.Len()-1]==']')){
      GetCh(); TxtChA.Trunc(TxtChA.Len()-2); break;
    } else {
      TxtChA+=Ch; GetCh();
    }
  }
}

void TXmlLx::SkipWs(){
  // [3] S ::=  (#x20 | #x9 | #xD | #xA)+
  while (ChDef.IsWs(Ch)){GetCh();}
}

TXmlLxSym TXmlLx::GetSym(){
  if (Ch=='<'){
    GetCh(); ClrArgV();
    if (Ch=='?'){
      GetCh(); TagNm=GetName();
      if (TagNm.GetLc()=="xml"){Sym=xsyXmlDecl; GetXmlDecl();}
      else {Sym=xsyPI; GetPI();}
    } else
    if (Ch=='!'){
      GetCh();
      if (Ch=='['){
        GetCh(); TagNm=GetName();
        if (TagNm=="CDATA"){Sym=xsyQStr; GetCDSect();}
        else {EThrow(TStr("Invalid tag after '<![' (")+TagNm+").");}
      } else
      if (Ch=='-'){
        Sym=xsyComment; GetComment();
      } else {
        TagNm=GetName();
        if (TagNm=="DOCTYPE"){GetDocTypeDecl(); Sym=xsyDocTypeDecl;}
        else if (TagNm=="ELEMENT"){GetElement(); Sym=xsyElement;}
        else if (TagNm=="ATTLIST"){GetAttList(); Sym=xsyAttList;}
        else if (TagNm=="ENTITY"){GetEntity(); Sym=xsyEntity;}
        else if (TagNm=="NOTATION"){GetNotation(); Sym=xsyNotation;}
        else {EThrow(TStr("Invalid tag (")+TagNm+").");}
      }
    } else
    if (Ch=='/'){
      // xsyETag
      GetCh(); Sym=xsyETag; TagNm=GetName(); GetWs(false);
      if (Ch=='>'){GetCh();}
      else {EThrow("Invalid End-Tag.");}
    } else {
      // xsySTag or xsySETag
      TagNm=GetName(); GetWs(false);
      while ((Ch!='>')&&(Ch!='/')){
        TStr AttrNm=GetName();
        GetEq();
        TStr AttrVal=GetAttValue();
        GetWs(false);
        AddArg(AttrNm, AttrVal);
      }
      if (Ch=='/'){
        if (GetCh()=='>'){Sym=xsySETag; GetCh();}
        else {EThrow("Invalid Empty-Element-Tag.");}
      } else {
        Sym=xsySTag; GetCh();
      }
    }
    if (Spacing==xspTruncate){SkipWs();}
  } else
  if (ChDef.IsWs(Ch)){
    Sym=xsyWs; GetWs(true); ToNrSpacing();
    if (Spacing==xspTruncate){GetSym();}
  } else
  if (Ch==TCh::EofCh){
    Sym=xsyEof;
  } else {
    Sym=xsyStr; TxtChA.Clr();
    // [14]  CharData ::=  [^<&]* - ([^<&]* ']]>' [^<&]*)
    forever {
      if (!ChDef.IsChar(Ch)){
        EThrow(TUInt::GetStr(Ch, "Invalid character (%d)."));}
		// GetCh();  continue; // skip invalid characters
      if (Ch=='<'){break;} // tag
      if (Ch=='&'){GetCh(); TxtChA+=GetReference();} // reference
      else {
        if ((Ch=='>')&&(TxtChA.Len()>=2)&&
         (TxtChA[TxtChA.Len()-2]==']')&&(TxtChA[TxtChA.Len()-1]==']')){
          EThrow("Forbidden substring ']]>' in character data.");}
        TxtChA+=Ch; GetCh(); // usual char
      }
    }
    ToNrSpacing();
  }
  return Sym;
}

TStr TXmlLx::GetSymStr() const {
  TChA SymChA;
  switch (Sym){
    case xsyUndef:
      SymChA="{Undef}"; break;
    case xsyWs:
      SymChA+="{Space:'"; SymChA+=TStr(TxtChA).GetHex(); SymChA+="'}"; break;
    case xsyComment:
      SymChA+="<!--"; SymChA+=TxtChA; SymChA+="-->"; break;
    case xsyXmlDecl:{
      SymChA+="<?"; SymChA+=TagNm;
      for (int ArgN=0; ArgN<GetArgs(); ArgN++){
        TStr ArgNm; TStr ArgVal; GetArg(ArgN, ArgNm, ArgVal);
        char ArgValQCh=GetArgValQCh(ArgVal);
        SymChA+=' '; SymChA+=ArgNm; SymChA+='=';
        SymChA+=ArgValQCh; SymChA+=ArgVal; SymChA+=ArgValQCh;
      }
      SymChA+="?>"; break;}
    case xsyPI:
      SymChA+="<?"; SymChA+=TagNm;
      if (!TxtChA.Empty()){SymChA+=' '; SymChA+=TxtChA;}
      SymChA+="?>"; break;
    case xsyDocTypeDecl:
      SymChA+="<!DOCTYPE "; SymChA+=TagNm; SymChA+=">"; break;
    case xsySTag:
    case xsySETag:{
      SymChA+="<"; SymChA+=TagNm;
      for (int ArgN=0; ArgN<GetArgs(); ArgN++){
        TStr ArgNm; TStr ArgVal; GetArg(ArgN, ArgNm, ArgVal);
        char ArgValQCh=GetArgValQCh(ArgVal);
        SymChA+=' '; SymChA+=ArgNm; SymChA+='=';
        SymChA+=ArgValQCh; SymChA+=ArgVal; SymChA+=ArgValQCh;
      }
      if (Sym==xsySTag){SymChA+=">";}
      else if (Sym==xsySETag){SymChA+="/>";}
      else {Fail;}
      break;}
    case xsyETag:
      SymChA+="</"; SymChA+=TagNm; SymChA+=">"; break;
    case xsyStr:
      SymChA="{String:'"; SymChA+=TxtChA; SymChA+="'}"; break;
    case xsyQStr:
      SymChA="{QString:'"; SymChA+=TxtChA; SymChA+="'}"; break;
    case xsyEof:
      SymChA="{Eof}"; break;
    default: Fail;
  }
  return SymChA;
}

void TXmlLx::EThrow(const TStr& MsgStr) const {
  TChA FPosChA;
  FPosChA+=" [File:"; FPosChA+=SIn->GetSNm();
  FPosChA+=" Line:"; FPosChA+=TInt::GetStr(LnN);
  FPosChA+=" Char:"; FPosChA+=TInt::GetStr(LnChN);
  FPosChA+="]";
  TStr FullMsgStr=MsgStr+FPosChA;
  TExcept::Throw(FullMsgStr);
}

TStr TXmlLx::GetFPosStr() const { 
  TChA FPosChA;
  FPosChA+=" [File:"; FPosChA+=SIn->GetSNm();
  FPosChA+=" Line:"; FPosChA+=TInt::GetStr(LnN);
  FPosChA+=" Char:"; FPosChA+=TInt::GetStr(LnChN);
  FPosChA+="]";
  return FPosChA;
}

TStr TXmlLx::GetXmlLxSymStr(const TXmlLxSym& XmlLxSym){
  switch (XmlLxSym){
    case xsyUndef: return "Undef";
    case xsyWs: return "White-Space";
    case xsyComment: return "Comment";
    case xsyXmlDecl: return "Declaration";
    case xsyPI: return "PI";
    case xsyDocTypeDecl: return "Document-Type";
    case xsyElement: return "Element";
    case xsyAttList: return "Attribute-List";
    case xsyEntity: return "Entity";
    case xsyNotation: return "Notation";
    case xsyTag: return "Tag";
    case xsySTag: return "Start-Tag";
    case xsyETag: return "End-Tag";
    case xsySETag: return "Start-End-Tag";
    case xsyStr: return "String";
    case xsyQStr: return "Quoted-String";
    case xsyEof: return "Eon-Of-File";
    default: return "Undef";
  }
}

bool TXmlLx::IsTagNm(const TStr& Str){
  TChA ChA=Str;
  if (ChA.Len()>0){
    if (TXmlLx::ChDef.IsFirstNameCh(ChA[0])){
      for (int ChN=1; ChN<ChA.Len(); ChN++){
        if (!TXmlLx::ChDef.IsName(ChA[ChN])){
          return false;
        }
      }
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

TStr TXmlLx::GetXmlStrFromPlainMem(const TMem& PlainMem){
  TChA XmlChA;
  for (int ChN=0; ChN<PlainMem.Len(); ChN++){
    uchar Ch=PlainMem[ChN];
    if ((' '<=Ch)&&(Ch<='~')){
      switch (Ch){
        case '"': XmlChA+="&quot;"; break;
        case '&': XmlChA+="&amp;"; break;
        case '\'': XmlChA+="&apos;"; break;
        case '<': XmlChA+="&lt;"; break;
        case '>': XmlChA+="&gt;"; break;
        default: XmlChA+=Ch;
      }
    } else
    if ((Ch=='\r')||(Ch=='\n')){
      XmlChA+=Ch;
    } else {
      XmlChA+='&'; XmlChA+='#'; XmlChA+=TUInt::GetStr(Ch); XmlChA+=';';
    }
  }
  return XmlChA;
}

TStr TXmlLx::GetXmlStrFromPlainStr(const TChA& PlainChA){
  TChA XmlChA;
  for (int ChN=0; ChN<PlainChA.Len(); ChN++){
    uchar Ch=PlainChA[ChN];
    if ((' '<=Ch)&&(Ch<='~')){
      switch (Ch){
        case '"': XmlChA+="&quot;"; break;
        case '&': XmlChA+="&amp;"; break;
        case '\'': XmlChA+="&apos;"; break;
        case '<': XmlChA+="&lt;"; break;
        case '>': XmlChA+="&gt;"; break;
        default: XmlChA+=Ch;
      }
    } else
    if ((Ch=='\r')||(Ch=='\n')){
      XmlChA+=Ch;
    } else {
      XmlChA+='&'; XmlChA+='#'; XmlChA+=TUInt::GetStr(Ch); XmlChA+=';';
    }
  }
  return XmlChA;
}

TStr TXmlLx::GetPlainStrFromXmlStr(const TStr& XmlStr){
  TChA PlainChA;
  TChRet Ch(TStrIn::New(XmlStr));
  Ch.GetCh();
  while (!Ch.Eof()){
    if (Ch()!='&'){
      PlainChA+=Ch(); Ch.GetCh();
    } else {
      // [67] Reference ::=  EntityRef | CharRef
      if (Ch.GetCh()=='#'){
        // [66]  CharRef ::=  '&#' [0-9]+ ';' | '&#x' [0-9a-fA-F]+ ';'
        TChA RefChA; int RefCd=0;
        if (Ch.GetCh()=='x'){
          // hex-decimal character code
          forever {
            Ch.GetCh();
            if (TCh::IsHex(Ch())){
              RefChA+=Ch();
              RefCd=RefCd*16+TCh::GetHex(Ch());
            } else {
              break;
            }
          }
        } else {
          // decimal character code
          forever {
            if (TCh::IsNum(Ch())){
              RefChA+=Ch();
              RefCd=RefCd*10+TCh::GetNum(Ch());
            } else {
              break;
            }
            Ch.GetCh();
          }
        }
        if ((!RefChA.Empty())&&(Ch()==';')){
          Ch.GetCh();
          uchar RefCh=uchar(RefCd);
          PlainChA+=RefCh;
        }
      } else {
        // [68]  EntityRef ::=  '&' Name ';'
        TChA EntityNm;
        while ((!Ch.Eof())&&(Ch()!=';')){
          EntityNm+=Ch(); Ch.GetCh();}
        if ((!EntityNm.Empty())&&(Ch()==';')){
          Ch.GetCh();
          if (EntityNm=="quot"){PlainChA+='"';}
          else if (EntityNm=="amp"){PlainChA+='&';}
          else if (EntityNm=="apos"){PlainChA+='\'';}
          else if (EntityNm=="lt"){PlainChA+='<';}
          else if (EntityNm=="gt"){PlainChA+='>';}
        }
      }
    }
  }
  return PlainChA;
}

TStr TXmlLx::GetUsAsciiStrFromXmlStr(const TStr& XmlStr){
  TStr UsAsciiStr=XmlStr;
  UsAsciiStr.ChangeStrAll("&#232;", "c");
  UsAsciiStr.ChangeStrAll("&#200;", "C");
  UsAsciiStr.ChangeStrAll("&#154;", "s");
  UsAsciiStr.ChangeStrAll("&#138;", "S");
  UsAsciiStr.ChangeStrAll("&#158;", "z");
  UsAsciiStr.ChangeStrAll("&#142;", "Z");
  TChA UsAsciiChA=TXmlLx::GetPlainStrFromXmlStr(UsAsciiStr);
  for (int ChN=0; ChN<UsAsciiChA.Len(); ChN++){
    char Ch=UsAsciiChA[ChN];
    if ((Ch<' ')||('~'<Ch)){UsAsciiChA.PutCh(ChN, 'x');}
  }
  return UsAsciiChA;
}

TStr TXmlLx::GetChRefFromYuEntRef(const TStr& YuEntRefStr){
  TStr ChRefStr=YuEntRefStr;
  ChRefStr.ChangeStrAll("&ch;", "&#232;");
  ChRefStr.ChangeStrAll("&Ch;", "&#200;");
  ChRefStr.ChangeStrAll("&sh;", "&#154;");
  ChRefStr.ChangeStrAll("&Sh;", "&#138;");
  ChRefStr.ChangeStrAll("&zh;", "&#158;");
  ChRefStr.ChangeStrAll("&Zh;", "&#142;");
  ChRefStr.ChangeStrAll("&cs", "c");
  ChRefStr.ChangeStrAll("&Cs;", "C");
  ChRefStr.ChangeStrAll("&dz;", "dz");
  ChRefStr.ChangeStrAll("&Dz;", "Dz");
  return ChRefStr;
}

/////////////////////////////////////////////////
// Xml-Token
bool TXmlTok::GetBoolArgVal(const TStr& ArgNm, const bool& DfVal) const {
  int ArgN=ArgNmValV.SearchForw(TStrKd(ArgNm));
  return (ArgN==-1) ? DfVal : (ArgNmValV[ArgN].Dat==TBool::TrueStr);
}

bool TXmlTok::GetBoolArgVal(
 const TStr& ArgNm, const TStr& TrueVal, const bool& DfVal) const {
  int ArgN=ArgNmValV.SearchForw(TStrKd(ArgNm));
  return (ArgN==-1) ? DfVal : (ArgNmValV[ArgN].Dat==TrueVal);
}

bool TXmlTok::GetBoolArgVal(const TStr& ArgNm,
 const TStr& TrueVal, const TStr& FalseVal, const bool& DfVal) const {
  int ArgN=ArgNmValV.SearchForw(TStrKd(ArgNm));
  if (ArgN==-1){return DfVal;}
  TStr ArgVal=ArgNmValV[ArgN].Dat;
  if (ArgVal==TrueVal){return true;}
  IAssert(ArgVal == FalseVal); return false;
}

int TXmlTok::GetIntArgVal(const TStr& ArgNm, const int& DfVal) const {
  int ArgN=ArgNmValV.SearchForw(TStrKd(ArgNm));
  if (ArgN==-1){
    return DfVal;
  } else {
    int Val;
    if (ArgNmValV[ArgN].Dat.IsInt(Val)){return Val;} else {return DfVal;}
  }
}

double TXmlTok::GetFltArgVal(const TStr& ArgNm, const double& DfVal) const {
  int ArgN=ArgNmValV.SearchForw(TStrKd(ArgNm));
  if (ArgN==-1){
    return DfVal;
  } else {
    double Val;
    if (ArgNmValV[ArgN].Dat.IsFlt(Val)){return Val;} else {return DfVal;}
  }
}

TStr TXmlTok::GetStrArgVal(const TStr& ArgNm, const TStr& DfVal) const {
  int ArgN=ArgNmValV.SearchForw(TStrKd(ArgNm));
  return (ArgN==-1) ? DfVal : ArgNmValV[ArgN].Dat;
}

void TXmlTok::PutSubTok(const PXmlTok& Tok, const int& SubTokN){
  if (SubTokN==-1){
    ClrSubTok(); AddSubTok(Tok);
  } else {
    SubTokV[SubTokN]=Tok;
  }
}

PXmlTok TXmlTok::GetTagTok(const TStr& TagPath) const {
  if (TagPath.Empty()){
    return (TXmlTok*)this;
  } else {
    TStr TagNm; TStr RestTagPath; TagPath.SplitOnCh(TagNm, '|', RestTagPath);
    PXmlTok SubTok;
    for (int SubTokN=0; SubTokN<SubTokV.Len(); SubTokN++){
      SubTok=SubTokV[SubTokN];
      if ((SubTok->GetSym()==xsyTag)&&(SubTok->GetStr()==TagNm)){break;}
      else {SubTok=NULL;}
    }
    if ((SubTok.Empty())||(RestTagPath.Empty())){return SubTok;}
    else {return SubTok->GetTagTok(RestTagPath);}
  }
}

void TXmlTok::GetTagTokV(const TStr& TagPath, TXmlTokV& XmlTokV) const {
  XmlTokV.Clr();
  TStr PreTagPath; TStr TagNm; TagPath.SplitOnLastCh(PreTagPath, '|', TagNm);
  PXmlTok Tok=GetTagTok(PreTagPath);
  if (!Tok.Empty()){
    for (int SubTokN=0; SubTokN<Tok->GetSubToks(); SubTokN++){
      PXmlTok SubTok=Tok->GetSubTok(SubTokN);
      if ((SubTok->GetSym()==xsyTag)&&(SubTok->GetStr()==TagNm)){
        XmlTokV.Add(SubTok);}
    }
  }
}

void TXmlTok::GetTagValV(const TStr& TagNm, const bool& XmlP, TStrV& ValV) const {
  if ((Sym==xsyTag)&&(Str==TagNm)){
    ValV.Add(GetTokStr(XmlP));
  } else {
    for (int SubTokN=0; SubTokN<GetSubToks(); SubTokN++){
      GetSubTok(SubTokN)->GetTagValV(TagNm, XmlP, ValV);}
  }
}

TStr TXmlTok::GetTagVal(const TStr& TagNm, const bool& XmlP) const {
  TStrV ValV; GetTagValV(TagNm, XmlP, ValV);
  if (ValV.Len()>0){return ValV[0];} else {return "";}
}

void TXmlTok::AddTokToChA(const bool& XmlP, TChA& ChA) const {
  switch (Sym){
    case xsyWs:
      ChA+=Str; break;
    case xsyStr:
      if (XmlP){ChA+=TXmlLx::GetXmlStrFromPlainStr(Str);} else {ChA+=Str;} break;
    case xsyQStr:
      if (XmlP){ChA+="<![CDATA[";}
      ChA+=Str;
      if (XmlP){ChA+="]]>";} break;
    case xsyTag:
      if (XmlP){
        ChA+='<'; ChA+=Str;
        for (int ArgN=0; ArgN<GetArgs(); ArgN++){
          TStr ArgNm; TStr ArgVal; GetArg(ArgN, ArgNm, ArgVal);
          if (XmlP){ArgVal=TXmlLx::GetXmlStrFromPlainStr(ArgVal);}
          char ArgValQCh=TXmlLx::GetArgValQCh(ArgVal);
          ChA+=' '; ChA+=ArgNm; ChA+='=';
          ChA+=ArgValQCh; ChA+=ArgVal; ChA+=ArgValQCh;
        }
      }
      if (GetSubToks()==0){
        if (XmlP){ChA+="/>";}
      } else {
        if (XmlP){ChA+=">";}
        for (int SubTokN=0; SubTokN<GetSubToks(); SubTokN++){
          GetSubTok(SubTokN)->AddTokToChA(XmlP, ChA);}
        if (XmlP){ChA+="</"; ChA+=Str; ChA+='>';}
      }
      break;
    default: Fail;
  }
}

TStr TXmlTok::GetTokVStr(const TXmlTokV& TokV, const bool& XmlP){
  TChA TokVChA;
  for (int TokN=0; TokN<TokV.Len(); TokN++){
    if (TokN>0){TokVChA+=' ';}
    TokVChA+=TokV[TokN]->GetTokStr(XmlP);
  }
  return TokVChA;
}

PXmlTok TXmlTok::GetTok(TXmlLx& Lx){
  switch (Lx.Sym){
    case xsyWs:
    case xsyStr:
    case xsyQStr:
      return TXmlTok::New(Lx.Sym, Lx.TxtChA);
    case xsySTag:
    case xsySETag:
      return TXmlTok::New(xsyTag, Lx.TagNm, Lx.ArgNmValKdV);
    default: Fail; return NULL;
  }
}

/////////////////////////////////////////////////
// Xml-Document
void TXmlDoc::LoadTxtMiscStar(TXmlLx& Lx){
  // [27] Misc ::=  Comment | PI |  S
  while ((Lx.Sym==xsyComment)||(Lx.Sym==xsyPI)||(Lx.Sym==xsyWs)){
    Lx.GetSym();}
}

PXmlTok TXmlDoc::LoadTxtElement(TXmlLx& Lx){
  // [39]  element ::=  EmptyElemTag | STag content ETag
  PXmlTok Tok;
  if (Lx.Sym==xsySETag){
    Tok=TXmlTok::GetTok(Lx);
  } else
  if (Lx.Sym==xsySTag){
    Tok=TXmlTok::GetTok(Lx);
    forever {
      Lx.GetSym();
      if (Lx.Sym==xsyETag){
        if (Tok->GetStr()==Lx.TagNm){
          break;
        } else {
          TStr MsgStr=TStr("Invalid End-Tag '")+Lx.TagNm+
           "' ('"+Tok->GetStr()+"' expected).";
          Lx.EThrow(MsgStr);
        }
      } else {
        PXmlTok SubTok;
        switch (Lx.Sym){
          case xsySTag:
            SubTok=LoadTxtElement(Lx); break;
          case xsySETag:
          case xsyStr:
          case xsyQStr:
          case xsyWs:
            SubTok=TXmlTok::GetTok(Lx); break;
          case xsyPI:
          case xsyComment:
            break;
          default: Lx.EThrow("Content or End-Tag expected.");
        }
        if (!SubTok.Empty()){
          Tok->AddSubTok(SubTok);}
      }
    }
  } else
  if (Lx.Sym==xsyETag){
    TStr MsgStr=
     TStr("Xml-Element (Start-Tag or Empty-Element-Tag) required.")+
     TStr::GetStr(Lx.TagNm, " End-Tag </%s> encountered.");
    Lx.EThrow(MsgStr);
  } else {
    Lx.EThrow("Xml-Element (Start-Tag or Empty-Element-Tag) required.");
  }
  return Tok;
}

PXmlTok TXmlDoc::GetTagTok(const TStr& TagPath) const {
  if (TagPath.Empty()){
    return Tok;
  } else {
    TStr TagNm; TStr RestTagPath; TagPath.SplitOnCh(TagNm, '|', RestTagPath);
    if ((Tok->GetSym()==xsyTag)&&(Tok->GetStr()==TagNm)){
      if (RestTagPath.Empty()){return Tok;}
      else {return Tok->GetTagTok(RestTagPath);}
    } else {
      return NULL;
    }
  }
}

void TXmlDoc::PutTagTokStr(const TStr& TagPath, const TStr& TokStr) const {
  PXmlTok Tok=GetTagTok(TagPath);
  Tok->ClrSubTok();
  PXmlTok StrTok=TXmlTok::New(xsyStr, TokStr);
  Tok->AddSubTok(StrTok);
}

void TXmlDoc::GetTagTokV(const TStr& TagPath, TXmlTokV& XmlTokV) const {
  XmlTokV.Clr();
  TStr PreTagPath; TStr TagNm; TagPath.SplitOnLastCh(PreTagPath, '|', TagNm);
  PXmlTok Tok=GetTagTok(PreTagPath);
  if (!Tok.Empty()){
    for (int SubTokN=0; SubTokN<Tok->GetSubToks(); SubTokN++){
      PXmlTok SubTok=Tok->GetSubTok(SubTokN);
      if ((SubTok->GetSym()==xsyTag)&&(SubTok->GetStr()==TagNm)){
        XmlTokV.Add(SubTok);}
    }
  }
}

bool TXmlDoc::GetTagTokBoolArgVal(
 const TStr& TagPath, const TStr& ArgNm, const bool& DfVal) const {
  PXmlTok TagTok;
  if (IsTagTok(TagPath, TagTok)){
    return TagTok->GetBoolArgVal(ArgNm, DfVal);}
  else {return DfVal;}
}

int TXmlDoc::GetTagTokIntArgVal(
 const TStr& TagPath, const TStr& ArgNm, const int& DfVal) const {
  PXmlTok TagTok;
  if (IsTagTok(TagPath, TagTok)){
    return TagTok->GetIntArgVal(ArgNm, DfVal);}
  else {return DfVal;}
}

double TXmlDoc::GetTagTokFltArgVal(
 const TStr& TagPath, const TStr& ArgNm, const double& DfVal) const {
  PXmlTok TagTok;
  if (IsTagTok(TagPath, TagTok)){
    return TagTok->GetFltArgVal(ArgNm, DfVal);}
  else {return DfVal;}
}

TStr TXmlDoc::GetTagTokStrArgVal(
 const TStr& TagPath, const TStr& ArgNm, const TStr& DfVal) const {
  PXmlTok TagTok;
  if (IsTagTok(TagPath, TagTok)){
    return TagTok->GetStrArgVal(ArgNm, DfVal);}
  else {return DfVal;}
}

TStr TXmlDoc::GetXmlStr(const TStr& Str){
  TChA ChA=Str;
  TChA XmlChA;
  for (int ChN=0; ChN<ChA.Len(); ChN++){
    uchar Ch=ChA[ChN];
    if ((' '<=Ch)&&(Ch<='~')){
      if (Ch=='&'){XmlChA+="&amp;";}
      else if (Ch=='>'){XmlChA+="&lt;";}
      else if (Ch=='<'){XmlChA+="&gt;";}
      else if (Ch=='\''){XmlChA+="&apos;";}
      else if (Ch=='\"'){XmlChA+="&quot;";}
      else {XmlChA+=Ch;}
    } else {
      XmlChA+="&#"; XmlChA+=TUInt::GetStr(Ch); XmlChA+=";";
    }
  }
  return XmlChA;
}

bool TXmlDoc::SkipTopTag(const PSIn& SIn){
  bool Ok=true;
  TXmlLx Lx(SIn, xspIntact);
  try {
    Lx.GetSym();
    // [22] prolog ::=  XMLDecl? Misc* (doctypedecl Misc*)?
    if (Lx.Sym==xsyXmlDecl){Lx.GetSym();}
    LoadTxtMiscStar(Lx);
    if (Lx.Sym==xsyDocTypeDecl){Lx.GetSym();}
    LoadTxtMiscStar(Lx);
    Ok=true;
  }
  catch (PExcept Except){
    Ok=false;
  }
  return Ok;
}

PXmlDoc TXmlDoc::LoadTxt(TXmlLx& Lx){
  PXmlDoc Doc=TXmlDoc::New();
  // [1]  document ::=  prolog element Misc*
  try {
    Lx.GetSym();
    // [22] prolog ::=  XMLDecl? Misc* (doctypedecl Misc*)?
    if (Lx.Sym==xsyXmlDecl){Lx.GetSym();}
    LoadTxtMiscStar(Lx);
    if (Lx.Sym==xsyDocTypeDecl){Lx.GetSym();}
    LoadTxtMiscStar(Lx);
    Doc->Tok=LoadTxtElement(Lx);
    LoadTxtMiscStar(Lx);
    Doc->Ok=true; Doc->MsgStr="Ok";
  }
  catch (PExcept& Except){
    Doc->Ok=false; Doc->MsgStr=Except->GetMsgStr();
  }
  return Doc;
}

PXmlDoc TXmlDoc::LoadTxt(const PSIn& SIn, const TXmlSpacing& Spacing){
  TXmlLx Lx(SIn, Spacing); return LoadTxt(Lx);
}

PXmlDoc TXmlDoc::LoadTxt(const TStr& FNm, const TXmlSpacing& Spacing){
  PSIn SIn=TFIn::New(FNm); return LoadTxt(SIn, Spacing);
}

void TXmlDoc::LoadTxt(
 const TStr& FNm, TXmlDocV& XmlDocV, const TXmlSpacing& Spacing){
  XmlDocV.Clr();
  PSIn SIn=TFIn::New(FNm);
  TXmlLx Lx(SIn, Spacing);
  PXmlDoc XmlDoc;
  forever {
    Lx.SkipWs();
    XmlDoc=LoadTxt(Lx);
    if (XmlDoc->IsOk()){XmlDocV.Add(XmlDoc);}
    else {break;}
  }
}

PXmlDoc TXmlDoc::LoadStr(const TStr& Str){
  PSIn SIn=TStrIn::New(Str);
  return LoadTxt(SIn);
}

void TXmlDoc::SaveStr(TStr& Str){
  PSOut SOut=TMOut::New(); TMOut& MOut=*(TMOut*)SOut();
  SaveTxt(SOut);
  Str=MOut.GetAsStr();
}

/////////////////////////////////////////////////
// Fast and dirty XML parser 
// very basic it does only <item>string</item>, no comments, no arguments
TXmlLxSym TXmlParser::GetSym() {
  if (NextSym != xsyUndef) {
    Sym = NextSym;  NextSym=xsyUndef;
    SymStr=NextSymStr;  NextSymStr.Clr();
    return Sym;
  }
  SymStr.Clr();
  char Ch;
  while (TCh::IsWs(Ch=GetCh())) { }
  if (Ch == TCh::EofCh) { Sym = xsyEof; return xsyEof; }
  if (Ch == '<') { // load tag
    Ch = GetCh();
    if (Ch == '/') { Sym = xsyETag; }
    else { Sym = xsySTag;  SymStr.Push(Ch); }
    while((Ch=GetCh())!='>' && Ch!=TCh::EofCh) { SymStr.Push(Ch); }
    const int StrLen = SymStr.Len();
    if (StrLen > 1 && SymStr[StrLen-1] == '/') { 
      Sym = xsyETag; SymStr[StrLen-1] = 0;
      for (char *c = SymStr.CStr()+StrLen-2; TCh::IsWs(*c); c--) { *c=0; }
    }
  } else { // load string
    _SymStr.Clr();  _SymStr.Push(Ch);
    while (! RSIn.Eof() && RSIn.PeekCh() != '<') { _SymStr.Push(GetCh()); }
    GetPlainStrFromXmlStr(_SymStr, SymStr);
    Sym = xsyStr;
  }
  if (Ch == TCh::EofCh) { SymStr.Clr(); Sym = xsyEof; return xsyEof; }
  return Sym;
}

TXmlLxSym TXmlParser::GetSym(TChA& _SymStr) {
  GetSym();
  _SymStr = SymStr;
  return Sym;
}

TXmlLxSym TXmlParser::PeekSym() { 
  if (NextSym == xsyUndef) {
    const TXmlLxSym TmpSim=Sym;
    const TChA TmpSymStr=SymStr;
    NextSym=GetSym(NextSymStr);
    Sym=TmpSim;
    SymStr=TmpSymStr;
  }
  return NextSym;
}

TXmlLxSym TXmlParser::PeekSym(TChA& _SymStr) { 
  PeekSym();
  _SymStr = NextSymStr;
  return NextSym; 
}

void TXmlParser::SkipTillTag(const TChA& _SymStr) { 
  while(PeekSym() != xsyEof) {
    if (NextSymStr == _SymStr) { return; }
    GetSym();
  }
}

// get <tag>value</tag>
void TXmlParser::GetTagVal(const TChA& TagStr, TChA& TagVal) { 
  EAssertR(GetTag(TagStr) == xsySTag, TStr::Fmt("Expected '<%s>'. Found '%s'", TagStr.CStr(), SymStr.CStr()).CStr());
  EAssertR(GetSym(TagVal) == xsyStr, "Expected string tag.");
  EAssertR(GetTag(TagStr) == xsyETag, TStr::Fmt("Expected '</%s>'. Found '%s'", TagStr.CStr(), SymStr.CStr()).CStr());
}

TXmlLxSym TXmlParser::GetTag(const TChA& TagStr) { 
  GetSym();
  EAssertR(TagStr==SymStr, TStr::Fmt("Expected xml symbol '%s'. Found '%s'", 
    TagStr.CStr(), SymStr.CStr()).CStr());
  return Sym;
}

void TXmlParser::GetPlainStrFromXmlStr(const TChA& XmlStr, TChA& PlainChA) {
  static TChA EntityNm;
  PlainChA.Clr();
  const char *Ch = XmlStr.CStr();
  while (*Ch){
    if (*Ch!='&'){ PlainChA+=*Ch; Ch++; } 
    else {
      if (*++Ch=='#'){
        TChA RefChA; int RefCd=0;
        if (*++Ch=='x'){
          forever {  Ch++;
            if (TCh::IsHex(*Ch)){ RefChA+=*Ch;  RefCd=RefCd*16+TCh::GetHex(*Ch); } 
            else { break; } }
        } else { // decimal character code
          forever {
            if (TCh::IsNum(*Ch)){ RefChA+=*Ch; RefCd=RefCd*10+TCh::GetNum(*Ch); } 
            else { break; } Ch++; }
        }
        if ((!RefChA.Empty())&&(*Ch==';')){
          Ch++;  const uchar RefCh=uchar(RefCd);  PlainChA+=RefCh; }
      } else {
        EntityNm.Clr();
        while ((*Ch)&&(*Ch!=';')){EntityNm+=*Ch; Ch++;}
        if ((!EntityNm.Empty())&&(*Ch==';')){  Ch++;
          if (EntityNm=="quot"){PlainChA+='"';}
          else if (EntityNm=="amp"){PlainChA+='&';}
          else if (EntityNm=="apos"){PlainChA+='\'';}
          else if (EntityNm=="lt"){PlainChA+='<';}
          else if (EntityNm=="gt"){PlainChA+='>';}
        }
      }
    }
  }
}
