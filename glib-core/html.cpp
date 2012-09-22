/////////////////////////////////////////////////
// Html-Lexical-Chars
void THtmlLxChDef::SetUcCh(const char& UcCh, const char& LcCh){
  // update upper-case (more lower cases may have one upper case)
  IAssert(
   (UcChV[LcCh-TCh::Mn]==TCh(0))||
   (UcChV[LcCh-TCh::Mn]==TCh(LcCh)));
  UcChV[LcCh-TCh::Mn]=TCh(UcCh);
  // update lower-case (one upper case may have only one lower case)
  if ((LcChV[UcCh-TCh::Mn]==TCh(0))||(LcChV[UcCh-TCh::Mn]==TCh(UcCh))){
    LcChV[UcCh-TCh::Mn]=TCh(LcCh);
  }
}

void THtmlLxChDef::SetUcCh(const TStr& Str){
  // set type of characters as letters
  SetChTy(hlctAlpha, Str);
  // first char in string is upper-case, rest are lower-case
  for (int ChN=1; ChN<Str.Len(); ChN++){
    SetUcCh(Str[0], Str[ChN]);
  }
}

void THtmlLxChDef::SetChTy(const THtmlLxChTy& ChTy, const TStr& Str){
  for (int ChN=0; ChN<Str.Len(); ChN++){
    ChTyV[Str[ChN]-TCh::Mn]=TInt(ChTy);}
}

void THtmlLxChDef::SetEscStr(const TStr& SrcStr, const TStr& DstStr){
  EscStrH.AddDat(SrcStr, DstStr);
}

TStr THtmlLxChDef::GetEscStr(const TStr& Str) const {
  int EscStrId;
  if ((EscStrId=EscStrH.GetKeyId(Str))!=-1){
    return EscStrH[EscStrId];
  } else
  if ((Str.Len()>=2)&&(Str[0]=='&')&&(Str[1]=='#')){
    int ChCd=0;
    for (int ChN=2; ChN<Str.Len(); ChN++){
      if (ChCd<=0xFFFF){ChCd=ChCd*10+Str[ChN]-'0';}}
    return TStr((char)ChCd);
  } else {
    return TStr(' ');
  }
}

THtmlLxChDef::THtmlLxChDef():
  ChTyV(TCh::Vals), UcChV(TCh::Vals), LcChV(TCh::Vals), EscStrH(100){

  // Character-Types
  ChTyV.PutAll(TInt(hlctSpace));
  SetChTy(hlctAlpha, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  SetChTy(hlctAlpha, "abcdefghijklmnopqrstuvwxyz");
  SetChTy(hlctAlpha, "@_");
  SetChTy(hlctNum, "0123456789");
  SetChTy(hlctSym, "`~!#$%^&*()-=+[{]}\\|;:'\",<.>/?");
  SetChTy(hlctLTag, "<"); SetChTy(hlctRTag, ">");
  SetChTy(hlctEof, TStr(TCh::EofCh));
  for (int Ch=TCh::Mn; Ch<=TCh::Mx; Ch++){
    if ((Ch<0)||(127<Ch)){SetChTy(hlctAlpha, TStr(TCh(char(Ch))));}}
  //SetChTy(hlctSpace, TStr(TCh(char(160))));

  // Upper-Case
  {for (int Ch=TCh::Mn; Ch<=TCh::Mx; Ch++){
    SetUcCh(char(Ch), char(Ch));}}
  SetUcCh("Aa"); SetUcCh("Àà"); SetUcCh("Áá"); SetUcCh("Ââ");
  SetUcCh("Ãã"); SetUcCh("Ää"); SetUcCh("Åå"); SetUcCh("Ææ");
  SetUcCh("Bb"); SetUcCh("Cc"); SetUcCh("Çç"); SetUcCh("Dd");
  SetUcCh("Ðð"); SetUcCh("Ee"); SetUcCh("Èè"); SetUcCh("Éé");
  SetUcCh("Êê"); SetUcCh("Ëë"); SetUcCh("Ff"); SetUcCh("Gg");
  SetUcCh("Hh"); SetUcCh("Ii"); SetUcCh("Ìì"); SetUcCh("Íí");
  SetUcCh("Îî"); SetUcCh("Ïï"); SetUcCh("Jj"); SetUcCh("Kk");
  SetUcCh("Ll"); SetUcCh("Mm"); SetUcCh("Nn"); SetUcCh("Ññ");
  SetUcCh("Oo"); SetUcCh("Òò"); SetUcCh("Óó"); SetUcCh("Ôô");
  SetUcCh("Õõ"); SetUcCh("Öö"); SetUcCh("Øø"); SetUcCh("Pp");
  SetUcCh("Qq"); SetUcCh("Rr"); SetUcCh("Ss"); SetUcCh("Šš");
  SetUcCh("Tt"); SetUcCh("Uu"); SetUcCh("Ùù"); SetUcCh("Úú");
  SetUcCh("Ûû"); SetUcCh("Üü"); SetUcCh("Vv"); SetUcCh("Ww");
  SetUcCh("Xx"); SetUcCh("Yyÿ"); SetUcCh("Ýý"); SetUcCh("Zz");
  SetUcCh("Žž");
  // ISO-CE
  //SetUcCh(uchar(169), uchar(185)); /*Sh - ©¹*/
  //SetUcCh(uchar(174), uchar(190)); /*Zh - ®¾*/
  //SetUcCh(uchar(200), uchar(232)); /*Ch - Èè*/
  //SetUcCh(uchar(198), uchar(230)); /*Cs - Ææ*/
  //SetUcCh(uchar(208), uchar(240)); /*Dz - Ðð*/

  // Annoying Unicode-characters
  //SetChTy(hlctSpace, "Âï");

  // Escape-Sequences
  SetEscStr("&quot", "\""); SetEscStr("&amp", "&");
  SetEscStr("&lt", "<"); SetEscStr("&gt", ">");
  SetEscStr("&nbsp", " ");

  SetEscStr("&auml", "ä"); SetEscStr("&Auml", "Ä");
  SetEscStr("&ouml", "ö"); SetEscStr("&Ouml", "Ö");
  SetEscStr("&uuml", "ü"); SetEscStr("&Uuml", "Ü");
  SetEscStr("&aring", "å"); SetEscStr("&Aring", "Å");
  SetEscStr("&oslash", "ø"); SetEscStr("&Oslash", "Ø");
  SetEscStr("&Aelig", "Æ"); SetEscStr("&aelig", "æ");

  SetEscStr("&eacute", "e"); SetEscStr("&Eacute", "E");
  SetEscStr("&egrave", "e"); SetEscStr("&Egrave", "E");
  SetEscStr("&agrave", "a"); SetEscStr("&Agrave", "A");
}

PHtmlLxChDef THtmlLxChDef::ChDef=PHtmlLxChDef(new THtmlLxChDef());

TStr THtmlLxChDef::GetCSZFromYuascii(const TChA& ChA){
  TChA DstChA;
  for (int ChN=0; ChN<ChA.Len(); ChN++){
    char Ch=ChA[ChN];
    switch (Ch){
      case '~': DstChA+='c'; break;
      case '^': DstChA+='C'; break;
      case '}': DstChA+='c'; break;
      case ']': DstChA+='C'; break;
      case '|': DstChA+='d'; break;
      case '\\': DstChA+='D'; break;
      case '{': DstChA+='s'; break;
      case '[': DstChA+='S'; break;
      case '`': DstChA+='z'; break;
      case '@': DstChA+='Z'; break;
      default: DstChA+=Ch;
    }
  }
  return DstChA;
}

TStr THtmlLxChDef::GetCSZFromWin1250(const TChA& ChA){
  TChA DstChA;
  for (int ChN=0; ChN<ChA.Len(); ChN++){
    const uchar Ch=ChA[ChN];
    switch (Ch){
      case 232: DstChA+='c'; break;
      case 200: DstChA+='C'; break;
      case 154: DstChA+='s'; break;
      case 138: DstChA+='S'; break;
      case 158: DstChA+='z'; break;
      case 142: DstChA+='Z'; break;
      default: DstChA+=Ch;
    }
  }
  return DstChA;
}

TStr THtmlLxChDef::GetWin1250FromYuascii(const TChA& ChA){
  TChA DstChA;
  for (int ChN=0; ChN<ChA.Len(); ChN++){
    char Ch=ChA[ChN];
    switch (Ch){
      case '~': DstChA+=uchar(232); break;
      case '^': DstChA+=uchar(200); break;
      case '}': DstChA+='c'; break;
      case ']': DstChA+='C'; break;
      case '|': DstChA+='d'; break;
      case '\\': DstChA+='D'; break;
      case '{': DstChA+=uchar(154); break;
      case '[': DstChA+=uchar(138); break;
      case '`': DstChA+=uchar(158); break;
      case '@': DstChA+=uchar(142); break;
      default: DstChA+=Ch;
    }
  }
  return DstChA;
}

TStr THtmlLxChDef::GetIsoCeFromYuascii(const TChA& ChA){
  TChA DstChA;
  for (int ChN=0; ChN<ChA.Len(); ChN++){
    char Ch=ChA[ChN];
    switch (Ch){
      case '~': DstChA+=uchar(232); break;
      case '^': DstChA+=uchar(200); break;
      case '}': DstChA+=uchar(230); break;
      case ']': DstChA+=uchar(198); break;
      case '|': DstChA+=uchar(240); break;
      case '\\': DstChA+=uchar(208); break;
      case '{': DstChA+=uchar(185); break;
      case '[': DstChA+=uchar(169); break;
      case '`': DstChA+=uchar(190); break;
      case '@': DstChA+=uchar(174); break;
      default: DstChA+=Ch;
    }
  }
  return DstChA;
}

/////////////////////////////////////////////////
// Html-Lexical
THtmlLxChDef THtmlLx::ChDef;

void THtmlLx::GetEscCh(){
  GetCh();
  EscCh=(Ch=='&');
  if (EscCh){
    EscChA.Clr(); EscChA.AddCh(Ch); GetCh();
    if (Ch=='#'){
      EscChA.AddCh(Ch); GetCh();
      if (('0'<=Ch)&&(Ch<='9')){
        do {EscChA.AddCh(Ch); GetCh();} while (('0'<=Ch)&&(Ch<='9'));
        if (Ch==';'){GetCh();}
        PutStr(ChDef.GetEscStr(EscChA));
      } else {
        PutCh('#'); PutCh('&');
      }
    } else
    if ((('a'<=Ch)&&(Ch<='z'))||(('A'<=Ch)&&(Ch<='Z'))){
      do {
        EscChA.AddCh(Ch); GetCh();
      } while ((('A'<=Ch)&&(Ch<='Z'))||(('a'<=Ch)&&(Ch<='z'))||(('0'<=Ch)&&(Ch<='9')));
      if (Ch==';'){
        GetCh(); PutStr(ChDef.GetEscStr(EscChA));
      } else {
        PutStr(EscChA);
      }      
    } else {
      PutCh('&');
    }
  }
}

void THtmlLx::GetMetaTag(){
  Sym=hsyMTag;
  if (Ch=='-'){
    char PCh=' ';
    while ((Ch!=TCh::EofCh) && ((PCh!='-')||(Ch!='>'))){PCh=Ch; GetCh();}
  } else {
    while ((Ch!=TCh::EofCh) && (Ch!='>')){GetCh();}
  }
  if (Ch!=TCh::EofCh){GetEscCh();}
}

void THtmlLx::GetTag(){
  if (Ch=='/'){Sym=hsyETag; GetCh();} else {Sym=hsyBTag;}
  UcChA.AddCh('<');
  while (ChDef.IsAlNum(Ch)||(Ch==':')){
    UcChA.AddCh(ChDef.GetUc(Ch)); GetCh();}
  UcChA.AddCh('>');
  ChA=UcChA;

  if (DoParseArg){
    while ((Ch!='>')&&(Ch!=TCh::EofCh)){
      while ((!ChDef.IsAlpha(Ch))&&(Ch!='>')&&(Ch!=TCh::EofCh)){GetCh();}
      if (ChDef.IsAlpha(Ch)){
        ArgNm.Clr(); ArgVal.Clr();
        while (ChDef.IsAlNum(Ch)||(Ch=='-')){ArgNm.AddCh(ChDef.GetUc(Ch)); GetCh();}
        while (ChDef.IsWs(Ch)){GetCh();}
        if (Ch=='='){
          GetCh(); while (ChDef.IsWs(Ch)){GetCh();}
          if (Ch=='"'){
            GetCh();
            while ((Ch!=TCh::EofCh)&&(Ch!='"')&&(Ch!='>')){
              if (!ChDef.IsEoln(Ch)){ArgVal.AddCh(Ch);} GetCh();}
            if (Ch=='"'){GetCh();}
          } else if (Ch=='\''){
            GetCh();
            while ((Ch!=TCh::EofCh)&&(Ch!='\'')&&(Ch!='>')){
              if (!ChDef.IsEoln(Ch)){ArgVal.AddCh(Ch);} GetCh();}
            if (Ch=='\''){GetCh();}
          } else {
            while ((!ChDef.IsWs(Ch))&&(Ch!='>')&&(Ch!=TCh::EofCh)){
              ArgVal.AddCh(Ch); GetCh();}
          }
          ArgNmValV.Add(TStrKd(ArgNm, ArgVal));
        }
      }
    }
  } else {
    while ((Ch!='>')&&(Ch!=TCh::EofCh)){GetCh();}
  }
  if (Ch!=TCh::EofCh){GetEscCh();}
}

THtmlLxSym THtmlLx::GetSym(){
  // prepare symbol descriptions
  ChA.Clr(); UcChA.Clr();
  PreSpaces=0; PreSpaceChA.Clr();
  ArgNmValV.Clr();
  // skip white-space
  while (ChDef.IsSpace(Ch)){
    if (ChX>0){PreSpaceChA+=Ch; PreSpaces++;} GetEscCh();}
  // parse symbol
  SymChA.Clr(); SymChA+=Ch; SymBChX=ChX;
  switch (ChDef.GetChTy(Ch)){
    case hlctAlpha:
      Sym=hsyStr;
      forever{
        do {
          ChA.AddCh(Ch); UcChA.AddCh(ChDef.GetUc(Ch)); GetEscCh();
        } while (ChDef.IsAlNum(Ch));
        if (Ch=='.'){
          GetCh();
          if (ChDef.IsAlNum(Ch)){ChA.AddCh('.'); UcChA.AddCh('.');}
          else {PutCh(Ch); Ch='.'; break;}
        } else {break;}
      }
      break;
    case hlctNum:
      Sym=hsyNum;
      forever{
        do {
          ChA.AddCh(Ch); UcChA.AddCh(Ch); GetEscCh();
        } while (ChDef.IsNum(Ch));
        if (Ch=='.'){
          GetCh();
          if (ChDef.IsAlNum(Ch)){ChA.AddCh('.'); UcChA.AddCh('.');}
          else {PutCh(Ch); Ch='.'; break;}
        } else if (ChDef.IsAlpha(Ch)){
          Sym=hsyStr;
        } else {
          break;
        }
      }
      break;
    case hlctSym:
      Sym=hsySSym; ChA.AddCh(Ch); UcChA.AddCh(Ch); GetEscCh();
      if ((ChA.LastCh()=='.')&&(ChDef.IsAlNum(Ch))){
        Sym=hsyStr;
        do {
          ChA.AddCh(Ch); UcChA.AddCh(ChDef.GetUc(Ch)); GetEscCh();
        } while (ChDef.IsAlNum(Ch));
      }
      break;
    case hlctLTag:
      if (EscCh){
        Sym=hsySSym; ChA.AddCh(Ch); UcChA.AddCh(Ch); GetEscCh();
      } else {
        GetCh();
        if (Ch=='!'){GetCh(); GetMetaTag();} else {GetTag();}
      }
      break;
    case hlctRTag:
      if (EscCh){
        Sym=hsySSym; ChA.AddCh(Ch); UcChA.AddCh(Ch); GetEscCh();
      } else {
        Sym=hsySSym; ChA.AddCh(Ch); UcChA.AddCh(Ch);  GetEscCh();
      }
      break;
    case hlctEof: Sym=hsyEof; break;
    default: Sym=hsyUndef; GetEscCh();
  }
  // set symbol last-character-position
  SymEChX=ChX-1;
  // delete last character
  if (!SymChA.Empty()){SymChA.Pop();}
  // return symbol
  return Sym;
}

PHtmlTok THtmlLx::GetTok(const bool& DoUc){
  if (DoUc){return PHtmlTok(new THtmlTok(Sym, UcChA, ArgNmValV));}
  else {return PHtmlTok(new THtmlTok(Sym, ChA, ArgNmValV));}
}

TStr THtmlLx::GetFullBTagStr() const {
  IAssert(Sym==hsyBTag);
  TChA BTagChA;
  BTagChA+=ChA; BTagChA.Pop();
  for (int ArgN=0; ArgN<GetArgs(); ArgN++){
    BTagChA+=' '; BTagChA+=GetArgNm(ArgN);
    BTagChA+='='; BTagChA+='"'; BTagChA+=GetArgVal(ArgN); BTagChA+='"';
  }
  BTagChA+='>';
  return BTagChA;
}

void THtmlLx::MoveToStrOrEof(const TStr& Str){
  do {
    GetSym();
  } while ((Sym!=hsyEof)&&((Sym!=hsyStr)||(ChA!=Str)));
}

void THtmlLx::MoveToBTagOrEof(const TStr& TagNm){
  do {
    GetSym();
  } while ((Sym!=hsyEof)&&((Sym!=hsyBTag)||(UcChA!=TagNm)));
}

void THtmlLx::MoveToBTag2OrEof(const TStr& TagNm1, const TStr& TagNm2){
  do {
    GetSym();
  } while ((Sym!=hsyEof)&&((Sym!=hsyBTag)||((UcChA!=TagNm1)&&(UcChA!=TagNm2))));
}

void THtmlLx::MoveToBTag3OrEof(const TStr& TagNm1, const TStr& TagNm2, const TStr& TagNm3){
  do {
    GetSym();
  } while ((Sym!=hsyEof)&&((Sym!=hsyBTag)||((UcChA!=TagNm1)&&(UcChA!=TagNm2)&&(UcChA!=TagNm3))));
}

void THtmlLx::MoveToBTagOrETagOrEof(const TStr& BTagNm, const TStr& ETagNm){
  do {
    GetSym();
  } while ((Sym!=hsyEof) && ((Sym!=hsyBTag)||(UcChA!=BTagNm)) && ((Sym!=hsyETag) || (UcChA!=ETagNm)));
}

void THtmlLx::MoveToBTagArgOrEof(
 const TStr& TagNm, const TStr& ArgNm, const TStr& ArgVal){
  forever {
    GetSym();
    if (Sym==hsyEof){break;}
    if ((Sym==hsyBTag)&&(UcChA==TagNm)&&
     (IsArg(ArgNm))&&(GetArg(ArgNm)==ArgVal)){break;}
  }
}

void THtmlLx::MoveToBTagArg2OrEof(const TStr& TagNm,
 const TStr& ArgNm1, const TStr& ArgVal1,
 const TStr& ArgNm2, const TStr& ArgVal2, const bool& AndOpP){
  forever {
    GetSym();
    if (Sym==hsyEof){break;}
    if (AndOpP){
      if ((Sym==hsyBTag)&&(UcChA==TagNm)&&
       (IsArg(ArgNm1))&&(GetArg(ArgNm1)==ArgVal1)&&
       (IsArg(ArgNm2))&&(GetArg(ArgNm2)==ArgVal2)){break;}
    } else {
      if ((Sym==hsyBTag)&&(UcChA==TagNm)&&
       (((IsArg(ArgNm1))&&(GetArg(ArgNm1)==ArgVal1))||
        ((IsArg(ArgNm2))&&(GetArg(ArgNm2)==ArgVal2)))){break;}
    }
  }
}

void THtmlLx::MoveToBTagOrEof(
 const TStr& TagNm1, const TStr& ArgNm1, const TStr& ArgVal1,
 const TStr& TagNm2, const TStr& ArgNm2, const TStr& ArgVal2){
  forever {
    GetSym();
    if (Sym==hsyEof){break;}
    if ((Sym==hsyBTag)&&(UcChA==TagNm1)&&
     (IsArg(ArgNm1))&&(GetArg(ArgNm1)==ArgVal1)){break;}
    if ((Sym==hsyBTag)&&(UcChA==TagNm2)&&
     (IsArg(ArgNm2))&&(GetArg(ArgNm2)==ArgVal2)){break;}
  }
}

void THtmlLx::MoveToETagOrEof(const TStr& TagNm){
  do {
    GetSym();
  } while ((Sym!=hsyEof)&&((Sym!=hsyETag)||(UcChA!=TagNm)));
}

TStr THtmlLx::GetTextOnlyStrToEof(){
  TChA OutChA;
  forever {
    GetSym();
    if (Sym==hsyEof){
      break;
    } else {
      if (PreSpaces>0){OutChA+=' ';}
      if ((Sym!=hsyBTag)&&(Sym!=hsyETag)){
        OutChA+=ChA;}
    }
  }
  return OutChA;
}

TStr THtmlLx::GetStrToBTag(const TStr& TagNm, const bool& TxtOnlyP){
  TChA OutChA;
  forever {
    GetSym();
    if ((Sym==hsyEof)||((Sym==hsyBTag)&&(UcChA==TagNm))){
      break;
    } else {
      if (PreSpaces>0){OutChA+=' ';}
      if ((TxtOnlyP&&(Sym!=hsyBTag)&&(Sym!=hsyETag))||(!TxtOnlyP)){
        OutChA+=ChA;}
    }
  }
  return OutChA;
}

TStr THtmlLx::GetStrToBTag(const TStr& TagNm, const TStr& ArgNm,
 const TStr& ArgVal, const bool& TxtOnlyP){
  TChA OutChA;
  forever {
    GetSym();
    if ((Sym==hsyEof)||((Sym==hsyBTag)&&(UcChA==TagNm)&&
     (IsArg(ArgNm))&&(GetArg(ArgNm)==ArgVal))){
      break;
    } else {
      if (PreSpaces>0){OutChA+=' ';}
      if ((TxtOnlyP&&(Sym!=hsyBTag)&&(Sym!=hsyETag))||(!TxtOnlyP)){
        OutChA+=ChA;}
    }
  }
  return OutChA;
}

TStr THtmlLx::GetStrToETag(const TStr& TagNm, const bool& TxtOnlyP){
  TChA OutChA;
  forever {
    GetSym();
    if ((Sym==hsyEof)||((Sym==hsyETag)&&(UcChA==TagNm))){
      break;
    } else {
      if (PreSpaces>0){OutChA+=' ';}
      if ((TxtOnlyP&&(Sym!=hsyBTag)&&(Sym!=hsyETag))||(!TxtOnlyP)){
        OutChA+=ChA;}
    }
  }
  return OutChA;
}

TStr THtmlLx::GetStrToETag2(const TStr& TagNm1, 
 const TStr& TagNm2, const bool& TxtOnlyP){
  TChA OutChA;
  forever {
    GetSym();
    if ((Sym==hsyEof)||((Sym==hsyETag)&&(UcChA==TagNm1))||((Sym==hsyETag)&&(UcChA==TagNm2))){
      break;
    } else {
      if (PreSpaces>0){OutChA+=' ';}
      if ((TxtOnlyP&&(Sym!=hsyBTag)&&(Sym!=hsyETag))||(!TxtOnlyP)){
        OutChA+=ChA;}
    }
  }
  return OutChA;
}

TStr THtmlLx::GetStrInTag(const TStr& TagNm, const bool& TxtOnlyP){
  MoveToBTagOrEof(TagNm);
  return GetStrToETag(TagNm, TxtOnlyP);
}

TStr THtmlLx::GetHRefBeforeStr(const TStr& Str){
  TStr HRefStr;
  forever {
    GetSym();
    if (Sym==hsyEof){HRefStr=""; break;}
    if ((Sym==hsyBTag)&&(UcChA=="<A>")){HRefStr=GetArg("HREF");}
    if ((Sym==hsyStr)&&(ChA==Str)){break;}
  }
  return HRefStr;
}

bool THtmlLx::IsGetBTag(const TStr& TagNm){
  if (GetSym()==hsyBTag){
    return ChA==TagNm;
  } else {return false;}
}

bool THtmlLx::IsGetETag(const TStr& TagNm){
  if (GetSym()==hsyETag){
    return ChA==TagNm;
  } else {return false;}
}

TStr THtmlLx::GetSymStr(const THtmlLxSym& Sym){
  switch (Sym){
    case hsyUndef: return "Undef";
    case hsyStr: return "Str";
    case hsyNum: return "Num";
    case hsySSym: return "SSym";
    case hsyUrl: return "Url";
    case hsyBTag: return "BTag";
    case hsyETag: return "ETag";
    case hsyMTag: return "MTag";
    case hsyEof: return "Eof";
    default: Fail; return TStr();
  }
}

TStr THtmlLx::GetEscapedStr(const TChA& ChA){
  TChA EscapedChA;
  for (int ChN=0; ChN<ChA.Len(); ChN++){
    char Ch=ChA[ChN];
    switch (Ch){
      case '"': EscapedChA+="&quot;"; break;
      case '&': EscapedChA+="&amp;"; break;
      case '\'': EscapedChA+="&apos;"; break;
      case '<': EscapedChA+="&lt;"; break;
      case '>': EscapedChA+="&gt;"; break;
      default: EscapedChA+=Ch;
    }
  }
  return EscapedChA;
}

TStr THtmlLx::GetAsciiStr(const TChA& ChA, const char& GenericCh){
  TChA AsciiChA;
  for (int ChN=0; ChN<ChA.Len(); ChN++){
    char Ch=ChA[ChN];
    if ((Ch<' ')||('~'<Ch)){
      Ch=GenericCh;}
    AsciiChA+=Ch;
  }
  return AsciiChA;
}

void THtmlLx::GetTokStrV(const TStr& Str, TStrV& TokStrV){
  PSIn SIn=TStrIn::New(Str);
  THtmlLx Lx(SIn);
  Lx.GetSym();
  TokStrV.Clr();
  while (Lx.Sym!=hsyEof){
    TokStrV.Add(Lx.ChA);
    Lx.GetSym();
  }
}

TStr THtmlLx::GetNoTag(const TStr& Str) {
  PSIn SIn=TStrIn::New(Str);
  THtmlLx Lx(SIn);
  Lx.GetSym();
  TChA ChA;
  while (Lx.Sym!=hsyEof){
    switch (Lx.Sym){
	  case hsyUndef: 
	  case hsyStr: 
	  case hsyNum: 
	  case hsySSym:
		if (Lx.PreSpaces > 0) { ChA += ' '; }
		ChA += Lx.ChA;
	  default: break;
	}
	Lx.GetSym();
  }
  return ChA;
}

/////////////////////////////////////////////////
// Html-Token
TStr THtmlTok::GetFullStr() const {
  if ((Sym==hsyBTag)&&(ArgNmValV.Len()>0)){
    TChA FullChA;
    FullChA+=Str.GetSubStr(0, Str.Len()-2);
    for (int ArgNmValN=0; ArgNmValN<ArgNmValV.Len(); ArgNmValN++){
      FullChA+=' '; FullChA+=ArgNmValV[ArgNmValN].Key; FullChA+='=';
      FullChA+='"'; FullChA+=ArgNmValV[ArgNmValN].Dat; FullChA+='"';
    }
    FullChA+='>';
    return FullChA;
  } else
  if (Sym==hsyETag){
    TChA FullChA;
    FullChA+='<'; FullChA+='/'; FullChA+=Str.GetSubStr(1, Str.Len()-1);
    return FullChA;
  } else {
    return GetStr();
  }
}

bool THtmlTok::IsUrlTok(TStr& RelUrlStr) const {
  if (GetSym()==hsyBTag){
    TStr TagNm=GetStr();
    if ((TagNm==ATagNm)&&(IsArg(HRefArgNm))){
      RelUrlStr=GetArg(HRefArgNm); return true;}
    else if ((TagNm==AreaTagNm)&&(IsArg(HRefArgNm))){
      RelUrlStr=GetArg(HRefArgNm); return true;}
    else if ((TagNm==FrameTagNm)&&(IsArg(SrcArgNm))){
      RelUrlStr=GetArg(SrcArgNm); return true;}
    else if ((TagNm==ImgTagNm)&&(IsArg(SrcArgNm))){
      RelUrlStr=GetArg(SrcArgNm); return true;}
    else if ((TagNm==MetaTagNm)&&(IsArg(HttpEquivArgNm))){
      TStr HttpEquivArgVal=GetArg(HttpEquivArgNm).GetUc();
      if ((HttpEquivArgVal=="REFRESH")&&IsArg("CONTENT")){
        TStr ContentStr=GetArg("CONTENT");
        TStr LeftStr; TStr RightStr; TStr UrlEqStr="URL=";
        ContentStr.GetUc().SplitOnStr(LeftStr, UrlEqStr, RightStr);
        RelUrlStr=ContentStr.GetSubStr(
         LeftStr.Len()+UrlEqStr.Len(), ContentStr.Len());
        return !RelUrlStr.Empty();
      } else {
        return false;
      }
    }
  }
  return false;
}

bool THtmlTok::IsRedirUrlTok() const {
  if (GetSym()==hsyBTag){
    TStr TagNm=GetStr();
    if ((TagNm==MetaTagNm)&&(IsArg(HttpEquivArgNm))){
      TStr HttpEquivArgVal=GetArg(HttpEquivArgNm).GetUc();
      if ((HttpEquivArgVal=="REFRESH")&&IsArg("CONTENT")){
        return true;
      } else {
        return false;
      }
    }
  }
  return false;
}

void THtmlTok::SaveTxt(const PSOut& SOut, const bool& TxtMode){
  if (TxtMode){
    SOut->PutStr(GetFullStr()); SOut->PutStr(" ");
  } else {
    SOut->PutStr(THtmlLx::GetSymStr(Sym)); SOut->PutStr(" ");
    SOut->PutStr(GetFullStr()); SOut->PutStr(" ");
  }
}

const TStr THtmlTok::ATagNm="<A>";
const TStr THtmlTok::AreaTagNm="<AREA>";
const TStr THtmlTok::BrTagNm="<BR>";
const TStr THtmlTok::CardTagNm="<CARD>";
const TStr THtmlTok::CenterTagNm="<CENTER>";
const TStr THtmlTok::FrameTagNm="<FRAME>";
const TStr THtmlTok::H1TagNm="<H1>";
const TStr THtmlTok::H2TagNm="<H2>";
const TStr THtmlTok::H3TagNm="<H3>";
const TStr THtmlTok::H4TagNm="<H4>";
const TStr THtmlTok::H5TagNm="<H5>";
const TStr THtmlTok::H6TagNm="<H6>";
const TStr THtmlTok::ImgTagNm="<IMG>";
const TStr THtmlTok::LiTagNm="<LI>";
const TStr THtmlTok::MetaTagNm="<META>";
const TStr THtmlTok::PTagNm="<P>";
const TStr THtmlTok::UlTagNm="<UL>";
const TStr THtmlTok::TitleTagNm="<TITLE>";
const TStr THtmlTok::TitleETagNm="</TITLE>";

const TStr THtmlTok::AltArgNm="ALT";
const TStr THtmlTok::HRefArgNm="HREF";
const TStr THtmlTok::SrcArgNm="SRC";
const TStr THtmlTok::TitleArgNm="TITLE";
const TStr THtmlTok::HttpEquivArgNm="HTTP-EQUIV";

bool THtmlTok::IsBreakTag(const TStr& TagNm){
  static TStrH BreakTagNmH(50);
  if (BreakTagNmH.Len()==0){
    BreakTagNmH.AddKey(TStr("<H1>")); BreakTagNmH.AddKey(TStr("<H2>"));
    BreakTagNmH.AddKey(TStr("<H3>")); BreakTagNmH.AddKey(TStr("<H4>"));
    BreakTagNmH.AddKey(TStr("<H5>")); BreakTagNmH.AddKey(TStr("<H6>"));
    BreakTagNmH.AddKey(TStr("<BR>")); BreakTagNmH.AddKey(TStr("<HR>"));
    BreakTagNmH.AddKey(TStr("<P>")); BreakTagNmH.AddKey(TStr("<DL>"));
    BreakTagNmH.AddKey(TStr("<UL>")); BreakTagNmH.AddKey(TStr("<OL>"));
    BreakTagNmH.AddKey(TStr("<LI>")); BreakTagNmH.AddKey(TStr("<DT>"));
    BreakTagNmH.AddKey(TStr("<DD>")); BreakTagNmH.AddKey(TStr("<HEAD>"));
    BreakTagNmH.AddKey(TStr("<TITLE>")); BreakTagNmH.AddKey(TStr("<META>"));
    BreakTagNmH.AddKey(TStr("<SCRIPT>"));
    BreakTagNmH.AddKey(TStr("<HEAD>")); BreakTagNmH.AddKey(TStr("<BODY>"));
  }
  return BreakTagNmH.IsKey(TagNm);
}

bool THtmlTok::IsBreakTok(const PHtmlTok& Tok){
  if ((Tok->GetSym()==hsyBTag)||(Tok->GetSym()==hsyETag)){
    return IsBreakTag(Tok->GetStr());
  } else {
    return false;
  }
}

bool THtmlTok::IsHTag(const TStr& TagNm, int& HTagN){
  if ((TagNm.Len()==4)&&(TagNm[0]=='<')&&(TagNm[1]=='H')&&(TagNm[3]=='>')){
    char Ch=TagNm[2];
    if (('1'<=Ch)&&(Ch<='6')){HTagN=Ch-'0'; return true;}
    else {HTagN=-1; return false;}
  } else {
    HTagN=-1; return false;
  }
}

PHtmlTok THtmlTok::GetHTok(const bool& IsBTag, const int& HTagN){
  THtmlLxSym HTagSym=IsBTag?hsyBTag:hsyETag;
  TStr HTagNm;
  switch (HTagN){
    case 1: HTagNm=H1TagNm; break;
    case 2: HTagNm=H2TagNm; break;
    case 3: HTagNm=H3TagNm; break;
    case 4: HTagNm=H4TagNm; break;
    case 5: HTagNm=H5TagNm; break;
    case 6: HTagNm=H6TagNm; break;
    default: Fail;
  }
  return PHtmlTok(new THtmlTok(HTagSym, HTagNm));
}

/////////////////////////////////////////////////
// Html-Document
THtmlDoc::THtmlDoc(const PSIn& SIn, const THtmlDocType& Type, const bool& DoUc):
  TokV(1000, 0){
  THtmlLx Lx(SIn);
  bool MkTok=false; bool InUL=false;
  while (Lx.GetSym()!=hsyEof){
    switch (Type){
      case hdtAll: MkTok=true; break;
      case hdtStr: MkTok=(Lx.Sym==hsyStr); break;
      case hdtStrNum: MkTok=(Lx.Sym==hsyStr)||(Lx.Sym==hsyNum); break;
      case hdtTag: MkTok=(Lx.Sym==hsyBTag)||(Lx.Sym==hsyETag); break;
      case hdtA: MkTok=(Lx.Sym==hsyBTag)&&(Lx.UcChA==THtmlTok::ATagNm); break;
      case hdtHRef:
        MkTok=(Lx.Sym==hsyBTag)&&
         ((Lx.UcChA==THtmlTok::ATagNm)||(Lx.UcChA==THtmlTok::AreaTagNm)||
         (Lx.UcChA==THtmlTok::FrameTagNm)||(Lx.UcChA==THtmlTok::ImgTagNm)||
         (Lx.UcChA==THtmlTok::MetaTagNm));
        break;
      case hdtUL:
        if ((Lx.Sym==hsyBTag)&&(Lx.UcChA==THtmlTok::UlTagNm)){InUL=true;}
        MkTok=InUL;
        if ((Lx.Sym==hsyETag)&&(Lx.UcChA==THtmlTok::UlTagNm)){InUL=false;}
        break;
      default: Fail;
    }
    if (MkTok){TokV.Add(Lx.GetTok(DoUc));}
  }
  TokV.Add(PHtmlTok(new THtmlTok(hsyEof)));
}

TStr THtmlDoc::GetTxtLnDoc(const TStr& HtmlStr){
  TChA LnDocChA;
  // prepare html parsing
  PSIn HtmlSIn=TStrIn::New(HtmlStr);
  THtmlLx HtmlLx(HtmlSIn);
  bool InScript=false;
  // save text
  while (HtmlLx.GetSym()!=hsyEof){
    TStr Str=HtmlLx.ChA;
    switch (HtmlLx.Sym){
      case hsyStr:
      case hsyNum:
      case hsySSym:
        if (InScript){break;}
        if (HtmlLx.PreSpaces>0){LnDocChA+=' ';}
        LnDocChA+=Str.CStr();
        break;
      case hsyBTag:
        if ((!LnDocChA.Empty())&&(LnDocChA.LastCh()!=' ')){LnDocChA+=' ';}
        if ((!InScript)&&(Str=="<SCRIPT>")){InScript=true;}
        break;
      case hsyETag:
        if ((!LnDocChA.Empty())&&(LnDocChA.LastCh()!=' ')){LnDocChA+=' ';}
        if ((InScript)&&(Str=="<SCRIPT>")){InScript=false;}
        break;
      default: break;
    }
  }
  // return result
  return LnDocChA;
}

TStr THtmlDoc::GetTxtLnDoc(const TStr& HtmlStr, 
 const TStr& BaseUrlStr, const bool& OutUrlP, const bool& OutTagsP){
  // prepare output-string
  TChA OutChA; OutChA+=' ';
  // prepare html parsing
  PSIn HtmlSIn=TStrIn::New(HtmlStr);
  THtmlLx HtmlLx(HtmlSIn);
  bool InScript=false;
  // save text
  while (HtmlLx.GetSym()!=hsyEof){
    TStr Str=HtmlLx.ChA;
    switch (HtmlLx.Sym){
      case hsyUndef:
      case hsyUrl:
      case hsyMTag:
        break;
      case hsyStr:
      case hsyNum:
      case hsySSym:
        if (InScript){break;}
        if (HtmlLx.PreSpaces>0){if (OutChA.LastCh()!=' '){OutChA+=' ';}}
        OutChA+=Str;
        break;
      case hsyBTag:
        // extract tag name
        Str=Str.GetSubStr(1, Str.Len()-2);
        // process tag
        if (!InScript){
          // check script tag
          if (Str=="SCRIPT"){
            InScript=true; break;}
          // output tag
          if (OutTagsP){
            OutChA+='<'; OutChA+=Str; OutChA+='>';
          } else {
            if (OutChA.LastCh()!=' '){OutChA+=' ';}
          }
          // check if URL present
          PHtmlTok Tok=HtmlLx.GetTok();
          TStr RelUrlStr;
          if (Tok->IsUrlTok(RelUrlStr)){
            PUrl Url=TUrl::New(RelUrlStr, BaseUrlStr);
            if (Url->IsOk()){
              if (OutUrlP){
                TStr XmlUrlStr=TXmlLx::GetXmlStrFromPlainStr(Url->GetUrlStr());
                OutChA+="<Url>"; OutChA+=XmlUrlStr; OutChA+="</Url>";
              }
            }
          }
        }
        break;
      case hsyETag:
        // extract tag name
        Str=Str.GetSubStr(1, Str.Len()-2);
        // process tag
        if (InScript){
          if (Str=="SCRIPT"){
            InScript=false; break;}
        } else {
          if (OutTagsP){
            OutChA+="</"; OutChA+=Str; OutChA+='>';
          } else {
            if (OutChA.LastCh()!=' '){OutChA+=' ';}
          }
        }
        break;
      case hsyEof: break;
      default: Fail;
    }
  }
  // return string
  return OutChA;
}


void THtmlDoc::SaveTxt(const PSOut& SOut, const bool& TxtMode) const {
  if (TxtMode){
    for (int TokN=0; TokN<TokV.Len(); TokN++){TokV[TokN]->SaveTxt(SOut);}
    SOut->PutLn();
  } else {
    for (int TokN=0; TokN<TokV.Len(); TokN++){
      SOut->PutStr(TInt::GetStr(TokN)); SOut->PutStr(": ");
      TokV[TokN]->SaveTxt(SOut);
      SOut->PutLn();
    }
  }
}

void THtmlDoc::SaveHtmlToTxt(
 const TStr& HtmlStr, const PSOut& TxtSOut, const TStr& BaseUrlStr,
 const bool& OutUrlP, const bool& OutTagsP){
  // get text-string from html-string
  TStr TxtStr=GetTxtLnDoc(HtmlStr, BaseUrlStr, OutUrlP, OutTagsP);
  // save text-string
  TxtStr.SaveTxt(TxtSOut);
}

void THtmlDoc::SaveHtmlToTxt(
 const TStr& HtmlStr, const TStr& TxtFNm, const TStr& BaseUrlStr,
 const bool& OutUrlP, const bool& OutTagsP){
  // create output file
  PSOut TxtSOut=TFOut::New(TxtFNm);
  // save to output file
  SaveHtmlToTxt(HtmlStr, TxtSOut, BaseUrlStr, OutUrlP, OutTagsP);
}

void THtmlDoc::SaveHtmlToXml(
 const TStr& HtmlStr, const PSOut& XmlSOut, const TStr& BaseUrlStr,
 const bool& OutTextP, const bool& OutUrlP, const bool& OutToksP,
 const bool& OutTagsP, const bool& OutArgsP){
  // prepare output-file-id
  TFileId fXml=XmlSOut->GetFileId();
  // create outgoing url
  TStrV OutUrlStrV;
  // open top tag
  fprintf(fXml, "<HtmlDoc>\n");
  // save url
  if (!BaseUrlStr.Empty()){
    TStr XmlBaseUrlStr=TXmlLx::GetXmlStrFromPlainStr(BaseUrlStr);
    fprintf(fXml, "<BaseUrl>%s</BaseUrl>\n", XmlBaseUrlStr.CStr());
  }
  // prepare html parsing
  PSIn HtmlSIn=TStrIn::New(HtmlStr);
  THtmlLx HtmlLx(HtmlSIn);
  TChA ContTextChA; bool InScript=false;
  // save text
  fprintf(fXml, "<Body>\n");
  while (HtmlLx.GetSym()!=hsyEof){
    TStr Str=HtmlLx.ChA;
    switch (HtmlLx.Sym){
      case hsyUndef:
      case hsyUrl:
      case hsyMTag:
        break;
      case hsyStr:
        if (InScript){break;}
        Str=TXmlLx::GetXmlStrFromPlainStr(Str);
        if (OutToksP){
          fprintf(fXml, "  <Str>%s</Str>\n", Str.CStr());}
        if (!ContTextChA.Empty()){ContTextChA+=' ';} ContTextChA+=Str;
        break;
      case hsyNum:
        if (InScript){break;}
        Str=TXmlLx::GetXmlStrFromPlainStr(Str);
        if (OutToksP){
          fprintf(fXml, "  <Num>%s</Num>\n", Str.CStr());}
        if (!ContTextChA.Empty()){ContTextChA+=' ';} ContTextChA+=Str;
        break;
      case hsySSym:
        if (InScript){break;}
        Str=TXmlLx::GetXmlStrFromPlainStr(Str);
        if (OutToksP){
          fprintf(fXml, "  <Sym>%s</Sym>\n", Str.CStr());}
        if (!ContTextChA.Empty()){ContTextChA+=' ';} ContTextChA+=Str;
        break;
      case hsyBTag:{
        // save continuos text
        if (!ContTextChA.Empty()){
          if (OutTextP){
            fprintf(fXml, "  <Text>%s</Text>\n", ContTextChA.CStr());}
          ContTextChA.Clr();
        }
        // extract tag name
        Str=Str.GetSubStr(1, Str.Len()-2);
        Str=TXmlLx::GetXmlStrFromPlainStr(Str);
        // process tag
        if (!InScript){
          // check script tag
          if (Str=="SCRIPT"){
            InScript=true; break;}
          // output tag
          if (OutTagsP){
            if (OutArgsP){
              fprintf(fXml, "  <BTag Nm=\"%s\">\n", Str.CStr());
              for (int ArgN=0; ArgN<HtmlLx.GetArgs(); ArgN++){
                TStr ArgNm=TXmlLx::GetXmlStrFromPlainStr(HtmlLx.GetArgNm(ArgN));
                TStr ArgVal=TXmlLx::GetXmlStrFromPlainStr(HtmlLx.GetArgVal(ArgN));
                fprintf(fXml, "    <Arg Nm=\"%s\" Val=\"%s\"/>", ArgNm.CStr(), ArgVal.CStr());
              }
              fprintf(fXml, "  </BTag>\n");
            } else {
              fprintf(fXml, "  <BTag Nm=\"%s\"/>\n", Str.CStr());
            }
          }
          // check if URL present
          PHtmlTok Tok=HtmlLx.GetTok();
          TStr RelUrlStr;
          if (Tok->IsUrlTok(RelUrlStr)){
            PUrl Url=TUrl::New(RelUrlStr, BaseUrlStr);
            if (Url->IsOk()){
              OutUrlStrV.Add(Url->GetUrlStr());
              if (OutUrlP){
                TStr XmlUrlStr=TXmlLx::GetXmlStrFromPlainStr(Url->GetUrlStr());
                fprintf(fXml, "  <Url>%s</Url>\n", XmlUrlStr.CStr());
              }
            }
          }
        }
        break;}
      case hsyETag:{
        // save continuos text
        if (!ContTextChA.Empty()){
          if (OutTextP){
            fprintf(fXml, "  <Text>%s</Text>\n", ContTextChA.CStr());}
          ContTextChA.Clr();
        }
        // extract tag name
        Str=Str.GetSubStr(1, Str.Len()-2);
        Str=TXmlLx::GetXmlStrFromPlainStr(Str);
        // process tag
        if (InScript){
          if (Str=="SCRIPT"){
            InScript=false; break;}
        } else {
          if (OutTagsP){
            fprintf(fXml, "  <ETag Nm=\"%s\"/>\n", Str.CStr());}
        }
        break;}
      case hsyEof: break;
      default: Fail;
    }
  }
  // save continuos text
  if (!ContTextChA.Empty()){
    if (OutTextP){
      fprintf(fXml, "  <Text>%s</Text>\n", ContTextChA.CStr());}
    ContTextChA.Clr();
  }
  fprintf(fXml, "</Body>\n");
  // save outgoing urls
  fprintf(fXml, "<OutUrls>\n");
  for (int UrlN=0; UrlN<OutUrlStrV.Len(); UrlN++){
    TStr XmlUrlStr=TXmlLx::GetXmlStrFromPlainStr(OutUrlStrV[UrlN]);
    fprintf(fXml, "  <Url N=\"%d\">%s</Url>\n", 1+UrlN, XmlUrlStr.CStr());
  }
  fprintf(fXml, "</OutUrls>\n");

  // close top tag
  fprintf(fXml, "</HtmlDoc>\n");
}

void THtmlDoc::SaveHtmlToXml(
 const TStr& HtmlStr, const TStr& XmlFNm, const TStr& BaseUrlStr,
 const bool& OutTextP, const bool& OutUrlP, const bool& OutToksP,
 const bool& OutTagsP, const bool& OutArgsP){
  // create output file
  PSOut XmlSOut=TFOut::New(XmlFNm);
  // save to output file
  SaveHtmlToXml(HtmlStr, XmlSOut, BaseUrlStr, OutTextP, OutUrlP,
   OutToksP, OutTagsP, OutArgsP);
}

TLxSym THtmlDoc::GetLxSym(const THtmlLxSym& HtmlLxSym, const TChA& ChA){
  switch (HtmlLxSym){
    case hsyUndef: return syUndef;
    case hsyStr: return syStr;
    case hsyNum: return syFlt;
    case hsySSym: return TLxSymStr::GetSSym(ChA);
    case hsyUrl: return syStr;
    case hsyBTag: return syStr;
    case hsyETag: return syStr;
    case hsyEof: return syEof;
    default: Fail; return syUndef;
  }
}

bool THtmlDoc::_IsTagRedir(
 const TStr& TagStr, const TStr& ArgNm, THtmlLx& Lx,
 const TStr& BaseUrlStr, const TStr& RedirUrlStr){
  IAssert(Lx.Sym==hsyBTag);
  if ((Lx.ChA==TagStr)&&(Lx.IsArg(ArgNm))){
    TStr RelUrlStr=Lx.GetArg(ArgNm);
    PUrl Url=TUrl::New(RelUrlStr, BaseUrlStr);
    if (Url->IsOk(usHttp)){
      TStr UrlStr=Url->GetUrlStr();
      PUrlEnv RedirUrlEnv=TUrlEnv::New(RedirUrlStr, "url", UrlStr);
      Lx.PutArg(ArgNm, RedirUrlEnv->GetFullUrlStr());
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

TStr THtmlDoc::GetRedirHtmlDocStr(const TStr& HtmlStr,
 const TStr& BaseUrlStr, const TStr& RedirUrlStr){
  PSIn SIn=TStrIn::New(HtmlStr);
  TMOut SOut;
  THtmlLx Lx(SIn);
  while (Lx.GetSym()!=hsyEof){
    SOut.PutStr(Lx.PreSpaceChA);
    if ((Lx.Sym==hsyBTag)&&(
     (_IsTagRedir(THtmlTok::ATagNm, THtmlTok::HRefArgNm, Lx, BaseUrlStr, RedirUrlStr))||
     (_IsTagRedir(THtmlTok::AreaTagNm, THtmlTok::HRefArgNm, Lx, BaseUrlStr, RedirUrlStr))||
     (_IsTagRedir(THtmlTok::FrameTagNm, THtmlTok::SrcArgNm, Lx, BaseUrlStr, RedirUrlStr))||
     (_IsTagRedir(THtmlTok::ImgTagNm, THtmlTok::SrcArgNm, Lx, BaseUrlStr, RedirUrlStr)))){
      SOut.PutStr(Lx.GetFullBTagStr());
    } else {
      SOut.PutStr(Lx.SymChA());
    }
  }
  return SOut.GetAsStr();
}

/////////////////////////////////////////////////
// Html-Hyper-Link-Document-Vector
THtmlHldV::THtmlHldV(const PHtmlDoc& _RefHtmlDoc, const int& HldWnLen):
  RefHtmlDoc(_RefHtmlDoc), HldV(){
  bool IsTitleAct=false; THtmlTokV TitleTokV;
  bool IsHAct=false; int ActHTagN=-1;
  TVec<THtmlTokV> HTokV(6);
  PHtmlTok Tok; THtmlLxSym TokSym; TStr TokStr;
  for (int TokN=0; TokN<RefHtmlDoc->GetToks(); TokN++){
    Tok=RefHtmlDoc->GetTok(TokN, TokSym, TokStr);
    if ((TokSym==hsyBTag)&&(TokStr==THtmlTok::ATagNm)){
      // collect tokens before, inside and after <a> ... </a> tags
      int ATokN; PHtmlTok ATok; THtmlLxSym ATokSym; TStr ATokStr;
      // inside <A> tags
      THtmlTokV ATokV; ATokN=TokN;
      forever{
        ATok=RefHtmlDoc->GetTok(ATokN, ATokSym, ATokStr);
        if (ATokSym!=hsySSym){ATokV.Add(ATok);}
        if ((ATokSym==hsyETag)&&(ATokStr==THtmlTok::ATagNm)){break;}
        ATokN++;
        if (ATokN>=RefHtmlDoc->GetToks()){break;}
      }
      int ETagATokN=ATokN+1;
      // before <A> tags
      THtmlTokV PrevATokV; ATokN=TokN;
      forever{
        ATokN--;
        if (ATokN<0){break;}
        ATok=RefHtmlDoc->GetTok(ATokN, ATokSym, ATokStr);
        if (THtmlTok::IsBreakTok(ATok)){break;}
        if ((ATokSym==hsyStr)||(ATokSym==hsyNum)){PrevATokV.Add(ATok);}
        if (ATokV.Len()>=HldWnLen){break;}
      }
      // after <A> tags
      THtmlTokV NextATokV; ATokN=ETagATokN;
      forever{
        ATokN++;
        if (ATokN>=RefHtmlDoc->GetToks()){break;}
        ATok=RefHtmlDoc->GetTok(ATokN, ATokSym, ATokStr);
        if (THtmlTok::IsBreakTok(ATok)){break;}
        if ((ATokSym==hsyStr)||(ATokSym==hsyNum)){NextATokV.Add(ATok);}
        if (ATokV.Len()>=HldWnLen){break;}
      }
      // construct html-document with hyper-link context
      PHtmlDoc HtmlDoc=PHtmlDoc(new THtmlDoc());
      HtmlDoc->AddTokV(TitleTokV);
      for (int HTagN=1; HTagN<=6; HTagN++){HtmlDoc->AddTokV(HTokV[HTagN-1]);}
      HtmlDoc->AddTokV(PrevATokV);
      HtmlDoc->AddTokV(ATokV);
      HtmlDoc->AddTokV(NextATokV);
      HldV.Add(HtmlDoc);
      HtmlDoc->SaveTxt(TSOut::StdOut);
    } else
    if (TokSym==hsyBTag){
      int HTagN;
      if (TokStr==THtmlTok::TitleTagNm){
        IsTitleAct=true; TitleTokV.Clr(); TitleTokV.Add(Tok);
      } else
      if (THtmlTok::IsHTag(TokStr, HTagN)){
        if (IsHAct){// conclude previous <H?> tag if left open
          HTokV[ActHTagN-1].Add(THtmlTok::GetHTok(false, ActHTagN));}
        IsHAct=true; ActHTagN=HTagN;
        {for (int HTagN=ActHTagN; HTagN<=6; HTagN++){HTokV[HTagN-1].Clr();}}
        HTokV[ActHTagN-1].Add(Tok);
      }
    } else
    if (TokSym==hsyETag){
      int HTagN;
      if (TokStr==THtmlTok::TitleTagNm){
        if (IsTitleAct){TitleTokV.Add(Tok); IsTitleAct=false;}
      } else
      if (THtmlTok::IsHTag(TokStr, HTagN)){
        if (IsHAct){HTokV[ActHTagN-1].Add(Tok); IsHAct=false;}
      }
    } else
    if (TokSym!=hsySSym){
      if (IsTitleAct){TitleTokV.Add(Tok);}
      if (IsHAct){HTokV[ActHTagN-1].Add(Tok);}
    }
  }
}

/////////////////////////////////////////////////
// Web-Page
void TWebPg::GetOutUrlV(TUrlV& OutUrlV, TUrlV& OutRedirUrlV) const {
  // create outgoing url vector
  OutUrlV.Clr(); OutRedirUrlV.Clr();
  // take interesting web-page components
  TStr UrlStr=GetUrlStr();
  TStr HtmlStr=GetHttpBodyAsStr();
  // prepare html parsing
  PSIn HtmlSIn=TStrIn::New(HtmlStr);
  PHtmlDoc HtmlDoc=THtmlDoc::New(HtmlSIn);
  PHtmlTok Tok;
  // traverse html
  for (int TokN=0; TokN<HtmlDoc->GetToks(); TokN++){
    PHtmlTok Tok=HtmlDoc->GetTok(TokN);
    if (Tok->GetSym()==hsyBTag){
      TStr RelUrlStr;
      if (Tok->IsUrlTok(RelUrlStr)){
        PUrl Url=TUrl::New(RelUrlStr, UrlStr);
        if (Url->IsOk(usHttp)){
          OutUrlV.Add(Url);
          if (Tok->IsRedirUrlTok()){
            OutRedirUrlV.Add(Url);
          }
        }
      }
    }
  }
}

void TWebPg::GetOutDescUrlStrKdV(TStrKdV& OutDescUrlStrKdV) const {
  // create outgoing url vector
  OutDescUrlStrKdV.Clr();
  // take interesting web-page components
  TStr UrlStr=GetUrlStr();
  TStr HtmlStr=GetHttpBodyAsStr();
  // prepare html parsing
  PSIn HtmlSIn=TStrIn::New(HtmlStr);
  PHtmlDoc HtmlDoc=THtmlDoc::New(HtmlSIn);
  // traverse html documents
  PHtmlTok Tok; THtmlLxSym TokSym; TStr TokStr;
  int TokN=0; int Toks=HtmlDoc->GetToks();
  while (TokN<Toks){
    Tok=HtmlDoc->GetTok(TokN, TokSym, TokStr); TokN++;
    if ((TokSym==hsyBTag)&&(TokStr==THtmlTok::ATagNm)){
      TStr RelUrlStr;
      if (Tok->IsUrlTok(RelUrlStr)){
        PUrl Url=TUrl::New(RelUrlStr, UrlStr);
        if (Url->IsOk()){
          TChA DescChA;
          while (TokN<Toks){
            Tok=HtmlDoc->GetTok(TokN, TokSym, TokStr); TokN++;
            if ((TokSym==hsyETag)&&(TokStr==THtmlTok::ATagNm)){
              break;
            } else {
              if ((TokSym==hsyStr)||(TokSym==hsyNum)||(TokSym==hsySSym)){
                if (!DescChA.Empty()){DescChA+=' ';}
                DescChA+=TokStr;
              }
            }
          }
          OutDescUrlStrKdV.Add(TStrKd(DescChA, Url->GetUrlStr()));
        }
      }
    }
  }
}

void TWebPg::SaveAsHttpBody(const TStr& FNm) const {
  // create output file
  PSOut SOut=TFOut::New(FNm);
  // save http-body
  HttpResp->SaveBody(SOut);
}

void TWebPg::SaveAsHttp(const TStr& FNm) const {
  // create output file
  PSOut SOut=TFOut::New(FNm);
  // save http
  HttpResp->SaveTxt(SOut);
}

bool TWebPg::IsTxt() const {
  if ((!HttpResp->IsContType())||HttpResp->IsContType(THttp::TextFldVal)){
    TStr Str=HttpResp->GetBodyAsStr();
    int StrLen=Str.Len(); int ChN=0; int PrintChs=0;
    while ((ChN<100)&&(ChN<StrLen)){
      char Ch=Str[ChN++];
      if (((' '<=Ch)&&(Ch<='~'))||(Ch==TCh::TabCh)||(Ch==TCh::LfCh)||(Ch==TCh::CrCh)){
        PrintChs++;}
    }
    double PrintPrb=double(PrintChs)/double(ChN+1);
    return PrintPrb>0.9;
  } else {
    return false;
  }
}

