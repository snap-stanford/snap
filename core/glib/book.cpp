/////////////////////////////////////////////////
// Includes
#include "StdAfx.h"
#include "book.h"

/////////////////////////////////////////////////
// Book
THash<TStr, TBook::TBookLoadF> TBook::TypeToLoadFH(100);

PBook TBook::Load(TSIn& SIn){
  TStr TypeNm(SIn);
  TBookLoadF LoadF=TypeToLoadFH.GetDat(TypeNm);
  return (*LoadF())(SIn);
}

bool TBook::Reg(const TStr& TypeNm, const TBookLoadF& LoadF){
  IAssert(!TypeToLoadFH.IsKey(TypeNm));
  TypeToLoadFH.AddDat(TypeNm, LoadF);
  return true;
}

/////////////////////////////////////////////////
// Bible
bool TBible::IsReg=TBible::MkReg();

void TBible::AddPsalm(
 const TStr& ChpNm, const int& SecN, const int& SSecN, const TStr& SSecStr){
  TBiblePsalm Psalm(ChpNm, SecN, SSecN, SSecStr);
  int SecId=PsalmV.Add(Psalm);
  TStr TitleStr=ChpNm+"|"+TInt::GetStr(SecN)+":"+TInt::GetStr(SSecN);
  Bix->AddSec(SecId, TitleStr+" "+SSecStr);
}

TBible::TBible(const TStr& FNm):
  TBook("Bible", "King James Version", "Religious"),
  ChpNmSecIdKdV(), PsalmV(), Bix(TBix::New()){
  // open file
  PSIn SIn=TFIn::New(FNm);
  TILx Lx(SIn, TFSet());
  // define state variables
  bool InPsalm=false; TStr ChpNm; TChA SecNChA; int SecN;
  TChA SSecNChA; int SSecN; TStr SSecStr;
  // parsing
  while (Lx.GetSym(syLn, syEof)!=syEof){
    TChA& Ln=Lx.Str;
    int VBarChN=Ln.SearchCh('|');
    if ((VBarChN!=-1)&&(0<VBarChN)&&(VBarChN<Lx.Str.Len())&&
     TCh::IsAlNum(Ln[VBarChN-1])&&TCh::IsNum(Ln[VBarChN+1])){
      if (InPsalm){AddPsalm(ChpNm, SecN, SSecN, SSecStr);}
//      if (PsalmV.Len()>100){return;}
      InPsalm=true;
      // psalm name
      ChpNm=Ln.GetSubStr(0, VBarChN-1).GetTrunc();
      if (ChpNmSecIdKdV.Empty()||ChpNmSecIdKdV.Last().Key!=ChpNm){
        ChpNmSecIdKdV.Add(TStrIntKd(ChpNm, PsalmV.Len()));}
      // section number
      SecNChA.Clr(); int ChN=VBarChN+1;
      while (TCh::IsNum(Ln[ChN])){SecNChA+=Ln[ChN]; ChN++;}
      SecN=TStr(SecNChA).GetInt();
      IAssert(Ln[ChN]==':');
      // subsection number
      SSecNChA.Clr(); ChN++;
      while (TCh::IsNum(Ln[ChN])){SSecNChA+=Ln[ChN]; ChN++;}
      SSecN=TStr(SSecNChA).GetInt();
      // text
      SSecStr=Ln.GetSubStr(ChN, Ln.Len()-1).GetTrunc();
    } else
    if (InPsalm){
      SSecStr=(SSecStr+' '+Ln).GetTrunc();
    }
  }
  // save last psalm
  if (InPsalm){AddPsalm(ChpNm, SecN, SSecN, SSecStr);}
}

PBookToc TBible::GetBookToc() const {
  PBookToc BookToc=TBookToc::New();
  BookToc->AddLevNm("Chapter");
  for (int ChpN=0; ChpN<ChpNmSecIdKdV.Len(); ChpN++){
    TStr TitleNm=ChpNmSecIdKdV[ChpN].Key;
    int SecId=ChpNmSecIdKdV[ChpN].Dat;
    BookToc->AddItem(0, TitleNm, SecId);
  }
  return BookToc;
}

void TBible::GetSecInfo(
 const int& SecId, TStr& SecIdStr, TStr& TitleStr, TStr& SecStr) const {
  TBiblePsalm& Psalm=PsalmV[SecId];
  SecIdStr=TInt::GetStr(SecId);
  TitleStr=Psalm.GetChpNm()+"|"+
   TInt::GetStr(Psalm.GetSecN())+":"+TInt::GetStr(Psalm.GetSSecN());
  SecStr=Psalm.GetSSecStr();
}

