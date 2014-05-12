#include "Snap.h"

int main(int argc, const char * argv[]) {
  TSsParser Ss("TestData3.zip", ssfCommaSep);

  while (Ss.Next()) {
    printf("%s\n", Ss.GetLnStr().CStr());
  }

  return 0;
}

