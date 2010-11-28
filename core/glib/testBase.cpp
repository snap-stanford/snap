#include "base.h"

int main(int argc, char* argv[]) {
  BaseTralala();

  TStr A = "tralala";
  THash<TInt, TFlt> H;
  for (int i = 0; i < 10; i++) { 
    H.AddDat(i) = TInt::Rnd.GetUniDev(); 
  }
  { TFOut FOut("hash.bin");
  H.Save(FOut); }
  { TFIn FIn("hash.bin"); 
  THash<TInt, TFlt> H2(FIn);
  printf("hash len: %d\n", H2.Len()); }
  TFile::Del("hash.bin");
  
  TUInt64 i(0x1234567898765432ll);
  printf("uint64: 0x%llx = [0x%x, 0x%x]\n", i.Val, i.GetPrimHashCd(), i.GetSecHashCd());
  
  return 0;
}
