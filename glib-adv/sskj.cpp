/////////////////////////////////////////////////
// Includes
#include "sskj.h"

/////////////////////////////////////////////////
// Sskj-Lexical-Chars
void TSskjLxChDef::SetUcCh(const TStr& Str){
  for (int ChN=1; ChN<Str.Len(); ChN++){UcChV[Str[ChN]-TCh::Mn]=TCh(Str[0]);}}

void TSskjLxChDef::SetChTy(const TSskjLxChTy& ChTy, const TStr& Str){
  for (int ChN=0; ChN<Str.Len(); ChN++){ChTyV[Str[ChN]-TCh::Mn]=TInt(ChTy);}}

void TSskjLxChDef::SetEscStr(const TStr& SrcStr, const TStr& DstStr){
  EscStrH.AddDat(SrcStr, DstStr);}

TStr TSskjLxChDef::GetEscStr(const TStr& Str){
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

TSskjLxChDef::TSskjLxChDef():
  ChTyV(TCh::Vals), UcChV(TCh::Vals), EscStrH(100){

  // Character-Types
  ChTyV.PutToAll(TInt(hlctSpace));
  SetChTy(hlctAlpha, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  SetChTy(hlctAlpha, "abcdefghijklmnopqrstuvwxyz");
  SetChTy(hlctAlpha, "~^{[@`}]\\|");
  SetChTy(hlctNum, "0123456789");
  SetChTy(hlctSym, "!#$%&*()-=+;:'\",./?");
  SetChTy(hlctLTag, "<"); SetChTy(hlctRTag, ">");
  SetChTy(hlctEof, EofCh);
  for (int Ch=TCh::Mn; Ch<=TCh::Mx; Ch++){
    if ((Ch<0)||(Ch>127)){SetChTy(hlctAlpha, TStr(TCh(char(Ch))));}}

  // Upper-Case
  {for (int Ch=TCh::Mn; Ch<=TCh::Mx; Ch++){UcChV[Ch-TCh::Mn]=TCh(char(Ch));}}
  SetUcCh("Aa"); SetUcCh("\xc0\xe0"); SetUcCh("\xc1\xe1"); SetUcCh("\xc2\xe2");
  SetUcCh("\xc3\xe3"); SetUcCh("\xc4\xe4"); SetUcCh("\xc5\xe5"); SetUcCh("\xc6\xe6");
  SetUcCh("Bb"); SetUcCh("Cc"); SetUcCh("\xc7\xe7"); SetUcCh("Dd");
  SetUcCh("\xd0\xf0"); SetUcCh("Ee"); SetUcCh("\xc8\xe8"); SetUcCh("\xc9\xe9");
  SetUcCh("\xca\xea"); SetUcCh("\xcb\xeb"); SetUcCh("Ff"); SetUcCh("Gg");
  SetUcCh("Hh"); SetUcCh("Ii"); SetUcCh("\xcc\xec"); SetUcCh("\xcd\xed");
  SetUcCh("\xce\xee"); SetUcCh("\xcf\xef"); SetUcCh("Jj"); SetUcCh("Kk");
  SetUcCh("Ll"); SetUcCh("Mm"); SetUcCh("Nn"); SetUcCh("\xd1\xf1");
  SetUcCh("Oo"); SetUcCh("\xd2\xf2"); SetUcCh("\xd3\xf3"); SetUcCh("\xd4\xf4");
  SetUcCh("\xd5\xf5"); SetUcCh("\xd6\xf6"); SetUcCh("\xd8\xf8"); SetUcCh("Pp");
  SetUcCh("Qq"); SetUcCh("Rr"); SetUcCh("Ss"); SetUcCh("Tt");
  SetUcCh("Uu"); SetUcCh("\xd9\xf9"); SetUcCh("\xda\xfa"); SetUcCh("\xdb\xfb");
  SetUcCh("\xdc\xfc"); SetUcCh("Vv"); SetUcCh("Ww"); SetUcCh("Xx");
  SetUcCh("Yy\xff"); SetUcCh("\xdd\xfd"); SetUcCh("Zz");

  // Escape-Sequences
  SetEscStr("&quot", "\""); SetEscStr("&amp", "&");
  SetEscStr("&lt", "<"); SetEscStr("&gt", ">");
  SetEscStr("&nbsp", " ");

  SetEscStr("&auml", "\xe4"); SetEscStr("&Auml", "\xc4");
  SetEscStr("&ouml", "\xf6"); SetEscStr("&Ouml", "\xd6");
  SetEscStr("&uuml", "\xfc"); SetEscStr("&Uuml", "\xdc");
  SetEscStr("&aring", "\xe5"); SetEscStr("&Aring", "\xc5");
  SetEscStr("&oslash", "\xf8"); SetEscStr("&Oslash", "\xd8");
  SetEscStr("&Aelig", "\xc6"); SetEscStr("&aelig", "\xe6");

  SetEscStr("&eacute", "e"); SetEscStr("&Eacute", "E");
  SetEscStr("&egrave", "e"); SetEscStr("&Egrave", "E");
  SetEscStr("&agrave", "a"); SetEscStr("&Agrave", "A");
}

/////////////////////////////////////////////////
// Sskj-Lexical
TSskjLxChDef TSskjLx::ChDef;

void TSskjLx::GetCh(){
  if (ChStack.Empty()){
    Ch=(SIn->Eof()) ? EofCh : SIn->GetCh();
  } else {
    Ch=ChStack.Pop();
  }
  if ((Ch==CrCh)||(Ch==LfCh)){GetCh();}
}

