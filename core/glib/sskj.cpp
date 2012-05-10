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
  SetUcCh("Qq"); SetUcCh("Rr"); SetUcCh("Ss"); SetUcCh("Tt");
  SetUcCh("Uu"); SetUcCh("Ùù"); SetUcCh("Úú"); SetUcCh("Ûû");
  SetUcCh("Üü"); SetUcCh("Vv"); SetUcCh("Ww"); SetUcCh("Xx");
  SetUcCh("Yyÿ"); SetUcCh("Ýý"); SetUcCh("Zz");

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

