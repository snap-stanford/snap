#ifndef wbmp_h
#define wbmp_h

/////////////////////////////////////////////////
// Includes
#include "base.h"
#include "gks.h"

/////////////////////////////////////////////////
// Includes
class TWbmp;
typedef TPt<TWbmp> PWbmp;

/////////////////////////////////////////////////
// Wireless-BMP
class TWbmp{
private:
  TCRef CRef;
private:
  TBoolVV PxValVV;
  static void PutMultiByteInt(const PSOut& SOut, const uint& Val);
  static uint GetMultiByteInt(const PSIn& SIn);
public:
  TWbmp(): PxValVV(){}
  TWbmp(const int& Width, const int& Height): PxValVV(){
    Gen(Width, Height);}
  static PWbmp New(){
    return PWbmp(new TWbmp());}
  static PWbmp New(const int& Width, const int& Height){
    return PWbmp(new TWbmp(Width, Height));}
  ~TWbmp(){}
  TWbmp(TSIn& SIn){Fail;}
  static PWbmp Load(TSIn& SIn);
  void Save(TSOut& SOut){Fail;}

  TWbmp& operator=(const TWbmp&){Fail; return *this;}

  void Clr(){PxValVV.Gen(GetWidth(), GetHeight());}
  void Gen(const int& Width, const int& Height){PxValVV.Gen(Width, Height);}
  int GetWidth() const {return PxValVV.GetXDim();}
  int GetHeight() const {return PxValVV.GetYDim();}
  bool PutPxVal(const int& X, const int& Y, const bool& PxVal) const {
    return PxValVV.At(X, Y)=PxVal;}
  bool GetPxVal(const int& X, const int& Y) const {
    return PxValVV.At(X, Y);}

  void Draw(const PGks& Gks);
  
  static PWbmp LoadWbmp(const TStr& FNm){
    return LoadWbmp(TFIn::New(FNm));}
  void SaveWbmp(const TStr& FNm) const {
    SaveWbmp(TFOut::New(FNm));}
  static PWbmp LoadWbmp(const PSIn& SIn);
  void SaveWbmp(const PSOut& SOut) const;


  friend PWbmp;
};

#endif
