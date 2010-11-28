/////////////////////////////////////////////////
// Includes
#include "mte.h"

/////////////////////////////////////////////////
// MulTextEast-Lexical-Chars
void TMteLxChDef::SetUcCh(const TStr& Str){
  for (int CC=1; CC<Str.Len(); CC++){UcChV[Str[CC]]=Str[0];}}

void TMteLxChDef::SetChTy(const TMteLxChTy& ChTy, const TStr& Str){
  for (int CC=0; CC<Str.Len(); CC++){ChTyV[Str[CC]]=ChTy;}}

TMteLxChDef::TMteLxChDef():
  ChTyV(TCh::Vals), UcChV(TCh::Vals){
  // Character-Types
  {for (int Ch=0; Ch<TCh::Vals; Ch++){ChTyV[Ch]=mtelcSpace;}}
  SetChTy(mtelcStr, "ABC^]D\\EFGHIJKLMNOPQRS[TUVWXYZ@");
  SetChTy(mtelcStr, "abc~}d|efghijklmnopqrs{tuvwxyz`");
  SetChTy(mtelcStr, "0123456789");
  SetChTy(mtelcStr, "-_.");
  SetChTy(mtelcEq, "=");
  SetChTy(mtelcComma, ",");
  ChTyV[TCh::CrCh]=mtelcEoln;
  ChTyV[TCh::LfCh]=mtelcEoln;
  ChTyV[TCh::EofCh]=mtelcEof;

  // Upper-Case
  {for (int Ch=0; Ch<TCh::Vals; Ch++){UcChV[Ch]=(char)Ch;}}
  SetUcCh("Aa"); SetUcCh("Bb"); SetUcCh("Cc"); SetUcCh("^~"); SetUcCh("]}");
  SetUcCh("Dd"); SetUcCh("\\|"); SetUcCh("Ee"); SetUcCh("Ff"); SetUcCh("Gg");
  SetUcCh("Hh"); SetUcCh("Ii"); SetUcCh("Jj"); SetUcCh("Kk"); SetUcCh("Ll");
  SetUcCh("Mm"); SetUcCh("Nn"); SetUcCh("Oo"); SetUcCh("Pp"); SetUcCh("Qq");
  SetUcCh("Rr"); SetUcCh("Ss"); SetUcCh("[{"); SetUcCh("Tt"); SetUcCh("Uu");
  SetUcCh("Vv"); SetUcCh("Ww"); SetUcCh("Xx"); SetUcCh("Yy"); SetUcCh("Zz");
  SetUcCh("@`");
}

/////////////////////////////////////////////////
// MulTextEast-Lexical
TMteLxSym TMteLx::GetSym(){
  while (ChDef.IsSpace(Ch)){GetCh();}
  switch (ChDef.GetChTy(Ch)){
    case mtelcStr:
      ChA.Clr(); UcChA.Clr();
      do {
        ChA.AddCh(Ch); UcChA.AddCh(ChDef.GetUc(Ch));
      } while (ChDef.IsStr(GetCh()));
      return Sym=mtesyStr;
    case mtelcEq:
      GetCh(); return Sym=mtesyEq;
    case mtelcComma:
      GetCh(); return Sym=mtesyComma;
    case mtelcEoln:
      if (Ch==TCh::CrCh){GetCh(); if (Ch==TCh::LfCh){GetCh();}}
      else if (Ch==TCh::LfCh){GetCh(); if (Ch==TCh::CrCh){GetCh();}}
      return Sym=mtesyEoln;
    case mtelcEof:
      IAssert(Sym!=mtesyEof); return Sym=mtesyEof;
    default:
      Fail; return mtesyUndef;
  }
}

/////////////////////////////////////////////////
// MulTextEast-Lex
TMteLex::TMteLex(const TStr& FNm, const int& Recs):
  InfWordToNrWordH(), NrWordToInfWordNVH(){
  printf("Loading Morphological Dictionary... ");
  if (Recs!=0){
    // loading dictionary
    PSIn SIn=TFIn::New(FNm);
    TMteLx Lx(SIn);
    while (Lx.GetSym(mtesyStr, mtesyEof)==mtesyStr){
      TStr InfWordStr=Lx.UcChA;
      TStr NrWordStr;
      if (Lx.GetSym(mtesyStr, mtesyEq)==mtesyStr){NrWordStr=Lx.UcChA;}
      else {NrWordStr=InfWordStr;}
      InfWordToNrWordH.AddDat(InfWordStr, NrWordStr);
      Lx.GetSym(mtesyStr);
      Lx.GetSym(mtesyEoln);
      if (InfWordToNrWordH.Len()%10000==0){printf("*");}
      if ((Recs!=-1)&&(InfWordToNrWordH.Len()>=Recs)){break;}
    }
    // calculating inverse dictionary
    int InfWords=InfWordToNrWordH.Len();
    TStr InfWordStr; TStr NrWordStr;
    for (int InfWordN=0; InfWordN<InfWords; InfWordN++){
      GetInfWordNrWordStr(InfWordN, InfWordStr, NrWordStr);
      NrWordToInfWordNVH.AddDat(NrWordStr).Add(InfWordN);
    }
    int NrWords=NrWordToInfWordNVH.Len();
    for (int NrWordN=0; NrWordN<NrWords; NrWordN++){
      NrWordToInfWordNVH[NrWordN].Pack();
    }
  }
  printf(" Done.\n");
}

void TMteLex::GetSampleNrWordNV(
 TRnd& Rnd, const int& SampleLen, TIntV& SampleNrWordNV) const {
  int NrWords=GetNrWords();
  SampleNrWordNV.Gen(NrWords);
  for (int NrWordN=0; NrWordN<NrWords; NrWordN++){
    SampleNrWordNV[NrWordN]=NrWordN;}
  SampleNrWordNV.Shuffle(Rnd);
  SampleNrWordNV.Trunc(SampleLen);
}

PMteLex TMteLex::LoadBin(const TStr& BinFNm){
  TFIn BinSIn(BinFNm);
  PMteLex MteLex=PMteLex(new TMteLex(BinSIn));
  return MteLex;
}

PMteLex TMteLex::LoadBinOrTxt(const TStr& BinFNm, const TStr& TxtFNm){
  printf("Loading files ...\n");
  PMteLex MteLex;
  if (TFile::Exists(BinFNm)){
    MteLex=LoadBin(BinFNm);
  } else {
    MteLex=TMteLex::New(TxtFNm);
    TFOut BinSOut(BinFNm); MteLex->Save(BinSOut);
  }
  printf("Done.\n");
  return MteLex;
}

void TMteLex::DumpWord(const int& NrWordN) const {
  TStr NrWordStr=GetNrWordStr(NrWordN);
  TIntV InfWordNV; GetInfWordNV(NrWordStr, InfWordNV);
  printf("'%s':", NrWordStr.CStr());
  for (int InfWordNN=0; InfWordNN<InfWordNV.Len(); InfWordNN++){
    int InfWordN=InfWordNV[InfWordNN];
    TStr InfWordStr=GetInfWordStr(InfWordN);
    printf(" '%s'", InfWordStr.CStr());
  }
  printf("\n");
}

/////////////////////////////////////////////////
// MulTextEast-Stop-Words
TMteSw::TMteSw(const TStr& FNm):
  WordStrH(2000000){
  printf("Searching for stop-words... ");
  PSIn SIn=TFIn::New(FNm);
  TMteLx Lx(SIn);
  TStr InfWordStr; TStr NrWordStr; TStr TypeStr; int LnN=0; char TypeCh;
  TVec<TCh> ChV;
  while (Lx.GetSym(mtesyStr, mtesyEof)==mtesyStr){
    InfWordStr=Lx.UcChA;
    if (Lx.GetSym(mtesyStr, mtesyEq)==mtesyStr){NrWordStr=Lx.UcChA;}
    else {NrWordStr=InfWordStr;}
    Lx.GetSym(mtesyStr); TypeStr=Lx.UcChA;
    Lx.GetSym(mtesyEoln); LnN++;
    TypeCh=TCh::NullCh; if (TypeStr.Len()>0){TypeCh=TypeStr[0];}
    if ((TypeCh=='Q')||(TypeCh=='C')||(TypeCh=='P')||(TypeCh=='S')||(TypeCh=='I')){
      WordStrH.AddDat(InfWordStr);
    } else
    if (TypeCh=='M'){
      WordStrH.AddDat(InfWordStr);
    } else
    if (!ChV.IsIn(TypeCh)){
      ChV.Add(TypeCh); printf("%c", TypeCh);
    }
    if (LnN%10000==0){printf("*");}
  }
  printf(" Done.\n");
}

void TMteSw::SaveTxt(const TStr& FNm, const bool& FmtCppP) const {
  PSOut SOut=TFOut::New(FNm);
  TStrV WordStrV;
  int WordStrP=WordStrH.FFirstKeyId();
  while (WordStrH.FNextKeyId(WordStrP)){
    TStr WordStr=WordStrH.GetKey(WordStrP);
    WordStrV.Add(WordStr);
  }
  WordStrV.Sort();
  if (FmtCppP){
    // c++ function calls
    TOLx Lx(SOut, TFSet(oloFrcEoln));
    for (int WordN=0; WordN<WordStrV.Len(); WordN++){
      if (WordN%5==0){
        Lx.PutUQStr(");"); Lx.PutLn(); Lx.PutUQStr("Add(");
      } else {
        Lx.PutSym(syComma);
      }
      Lx.PutQStr(WordStrV[WordN]);
    }
  } else {
    // word per line
    for (int WordN=0; WordN<WordStrV.Len(); WordN++){
      TStr WordStr=WordStrV[WordN];
      WordStr=THtmlLxChDef::GetIsoCeFromYuascii(WordStr);
      SOut->PutStr(WordStr); SOut->PutLn();
    }
  }
}

