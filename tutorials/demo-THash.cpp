#include "Snap.h"

// Table manipulations, keys are integers
void ManipulateTableInt(TIntIntH& TableInt) {
  const int64 NElems = 1000000;
  int DDist = 10;
  const char *FName = "demo.hashint.dat";
  TIntIntH TableInt1;
  TIntIntH TableInt2;
  int i;
  int n;
  int Id;
  int Key;
  int Value;
  bool t;

  // add table elements: key i, value i+1, i = 0..NElems
  for (i = 0; i < NElems; i++) {
    TableInt.AddDat(i,i+1);
  }
  t = TableInt.Empty();
  n = TableInt.Len();
  printf("TableInt : %d elements\n",TableInt.Len());

  // get elements by successive key values
  for (i = 0; i < NElems; i++) {
    // get the element id number
    Id = TableInt.GetKeyId(i);
    // get the key
    Key = TableInt.GetKey(Id);
    // get the value
    Value = TableInt.GetDat(Key);
  }

  // get elements by an iterator
  for (TIntIntH::TIter It = TableInt.BegI(); It < TableInt.EndI(); It++) {
    // get the key
    Key = It.GetKey();
    // get the value
    Value = It.GetDat();
  }

  // get elements by an index
  Id = TableInt.FFirstKeyId();
  while (TableInt.FNextKeyId(Id)) {
    // get the key
    Key = TableInt.GetKey(Id);
    // get the value
    Value = TableInt.GetDat(Key);
  }

  // update values from (i,i+1) to (i,i+2)
  for (i = 0; i < NElems; i++) {
    TableInt.AddDat(i,i*10);
  }

  // delete elements by a key
  for (i = 0; i < NElems; i += DDist) {
    TableInt.DelKey(i);
  }
  printf("TableInt : %d elements\n",TableInt.Len());

  // delete elements by an index
  for (i = 1; i < NElems; i += DDist) {
    Id = TableInt.GetKeyId(i);
    TableInt.DelKeyId(Id);
  }
  printf("TableInt : %d elements\n",TableInt.Len());

  // assignment
  TableInt1 = TableInt;
  printf("TableInt1: %d elements\n",TableInt1.Len());

  // save the table
  {
    TFOut FOut(FName);
    TableInt.Save(FOut);
    FOut.Flush();
  }

  // load a table
  {
    TFIn FIn(FName);
    TableInt2.Load(FIn);
  }
  printf("TableInt2: %d elements\n",TableInt2.Len());

  // remove all elements
  n = TableInt.Len();
  for (i = 0; i < n; i++) {
    Id = TableInt.GetRndKeyId(TInt::Rnd, 0.5);
    TableInt.DelKeyId(Id);
  }
  printf("TableInt : %d elements\n",TableInt.Len());

  // clear the table
  TableInt1.Clr();
  printf("TableInt1: %d elements\n",TableInt1.Len());
}

// Table manipulations, keys are TStr
void ManipulateTableStr(TStrIntH& TableStr) {
  const int64 NElems = 1000000;
  int DDist = 10;
  const char *FName = "demo.hashstr.dat";
  TStrIntH TableStr1;
  TStrIntH TableStr2;
  int i;
  int n;
  int Id;
  TStr KeyStr;
  int Value;
  bool t;
  char s[32];

  // add table elements: key str(i), value i+1, i = 0..NElems
  for (i = 0; i < NElems; i++) {
    sprintf(s,"%d",i);
    TStr Str = TStr(s);
    TableStr.AddDat(Str,i+1);
  }
  t = TableStr.Empty();
  n = TableStr.Len();
  printf("TableStr : %d elements\n",TableStr.Len());

  // get elements by successive key values
  for (i = 0; i < NElems; i++) {
    // get the element id number
    sprintf(s,"%d",i);
    TStr Str = TStr(s);
    Id = TableStr.GetKeyId(s);
    // get the key
    KeyStr = TableStr.GetKey(Id);
    // get the value
    Value = TableStr.GetDat(KeyStr);
  }

  // get elements by an iterator
  for (TStrIntH::TIter It = TableStr.BegI(); It < TableStr.EndI(); It++) {
    // get the key
    KeyStr = It.GetKey();
    // get the value
    Value = It.GetDat();
  }

  // verify elements by an index
  Id = TableStr.FFirstKeyId();
  while (TableStr.FNextKeyId(Id)) {
    // get the key
    KeyStr = TableStr.GetKey(Id);
    // get the value
    Value = TableStr.GetDat(KeyStr);
  }

  // delete elements by a key
  for (i = 0; i < NElems; i += DDist) {
    sprintf(s,"%d",i);
    TStr Str = TStr(s);
    TableStr.DelKey(Str);
  }
  printf("TableStr : %d elements\n",TableStr.Len());

  // delete elements by an index
  for (i = 1; i < NElems; i += DDist) {
    sprintf(s,"%d",i);
    TStr Str = TStr(s);
    Id = TableStr.GetKeyId(Str);
    TableStr.DelKeyId(Id);
  }
  printf("TableStr : %d elements\n",TableStr.Len());

  // assignment
  TableStr1 = TableStr;
  printf("TableStr1: %d elements\n",TableStr1.Len());

  // save the table
  {
    TFOut FOut(FName);
    TableStr.Save(FOut);
    FOut.Flush();
  }

  // load a table
  {
    TFIn FIn(FName);
    TableStr2.Load(FIn);
  }
  printf("TableStr2: %d elements\n",TableStr2.Len());

  // remove all elements
  n = TableStr.Len();
  for (i = 0; i < n; i++) {
    Id = TableStr.GetRndKeyId(TInt::Rnd, 0.5);
    TableStr.DelKeyId(Id);
  }
  printf("TableStr : %d elements\n",TableStr.Len());

  // clear the table
  TableStr1.Clr();
  printf("TableStr1: %d elements\n",TableStr1.Len());
}

int main(int argc, char* argv[]) {
  TIntIntH TableInt;
  TStrIntH TableStr;

  ManipulateTableInt(TableInt);
  ManipulateTableStr(TableStr);
}


