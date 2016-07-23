#include "Snap.h"

//
// short demo code for the TSsParser (spreadsheet parser) class
//

void ReadByField() {
  int LnCnt;

  TSsParser Ss("parser/TestData.zip", ssfCommaSep);

  LnCnt = 0;
  while (Ss.Next()) {
    LnCnt++;
    for (int i = 0; i < Ss.Len(); i++) {
      printf("line %2d, field %2d: %s\n", LnCnt, i+1, Ss[i]);
    }
  }
}

void ReadByLine() {
  TSsParser Ss("parser/TestData.zip", ssfCommaSep);

  while (Ss.Next()) {
    printf("%s\n", Ss.GetLnStr().CStr());
  }
}

int main(int argc, const char * argv[]) {
  ReadByField();
  ReadByLine();
  return 0;
}

