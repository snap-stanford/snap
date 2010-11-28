/////////////////////////////////////////////////
// Includes
#include "wbmp.h"

/////////////////////////////////////////////////
// Wireless-BMP
void TWbmp::PutMultiByteInt(const PSOut& SOut, const uint& Val){
  if (Val<128){
    SOut->PutCh(uchar(Val));
  } else
  if (Val<128*128){
    uchar MsCh=uchar(Val/128); MsCh+=uchar(128);
    uchar LsCh=uchar(Val%128);
    SOut->PutCh(MsCh);
    SOut->PutCh(LsCh);
  } else {
    Fail;
  }
}

uint TWbmp::GetMultiByteInt(const PSIn& SIn){
  uint Val=0; TB8Set BSet;
  do {
    BSet=uchar(SIn->GetCh());
    Val=Val*128+BSet.GetInt(0, 6);
  } while (BSet.In(7));
  return Val;
}

void TWbmp::Draw(const PGks& Gks){
  TGksColor WhiteColor=TGksColor::GetWhite();
  TGksColor BlackColor=TGksColor::GetBlack();
  for (int Y=0; Y<GetHeight(); Y++){
    for (int X=0; X<GetWidth(); X++){
      if (GetPxVal(X, Y)){Gks->PutPixel(X, Y, WhiteColor);}
      else {Gks->PutPixel(X, Y, BlackColor);}
    }
  }
}

PWbmp TWbmp::LoadWbmp(const PSIn& SIn){
  // read header
  uint TypeField=GetMultiByteInt(SIn);
  if (TypeField!=0){TExcept::Throw("Invalid WBMP TypeField.");}
  TB8Set FixHeaderField=uchar(SIn->GetCh());
  if (FixHeaderField.In(7)){
    GetMultiByteInt(SIn);} // ExtFields
  int Width=GetMultiByteInt(SIn);
  int Height=GetMultiByteInt(SIn);
  // create wbmp
  PWbmp Wbmp=TWbmp::New(Width, Height);
  // read & fill bitmap
  for (int Y=0; Y<Height; Y++){
    int X=0; TB8Set BSet;
    while (X<Width){
      if (X%8==0){BSet=uchar(SIn->GetCh());}
      Wbmp->PutPxVal(X, Y, BSet.In(7-X%8));
      X++;
    }
  }
  return Wbmp;
}

void TWbmp::SaveWbmp(const PSOut& SOut) const {
  // write header
  SOut->PutCh(uchar(0)); // TypeField
  SOut->PutCh(uchar(0)); // FixHeaderField
  PutMultiByteInt(SOut, GetWidth()); // Width
  PutMultiByteInt(SOut, GetHeight()); // Height
  // write bitmap
  for (int Y=0; Y<GetHeight(); Y++){
    TB8Set BSet; int X=0;
    while (X<GetWidth()){
      if (GetPxVal(X,Y)){BSet.Incl(7-X%8);}
      X++;
      if (X%8==0){SOut->PutCh(BSet.GetUCh()); BSet=0;}
    }
    if (X%8!=0){SOut->PutCh(BSet.GetUCh());}
  }
}

