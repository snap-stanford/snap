#include "Snap.h"

void print_array(int *x, int length) {
  for (int i = 0; i < length; i++)
    printf("x[%d] = %d\n", i, x[i]);
}

TFltV PyTFltV(double x[10]) {

  TFltV V;
  for (int i = 0; i < 10; i++) {
    V.Add(x[i]);
  }
  
  return V;
}

TIntV PyToTIntV (int *array, int length) {
  
  TIntV V;
  for (int i = 0; i < length; i++) {
    //printf("Adding array[%d] = %d\n", i, array[i]);
    V.Add(array[i]);
  }
  
  return V;
}

int count(char *str, int len, char c) {
  int sum = 0;
  for (int i = 0; i < len; i++) {
    if (str[i] == c)
      sum++;
  }
  return sum;
}

//void TIntVToPy (TIntV originalList, TIntV snapList, int& len) {
void TIntVToPy (TIntV originalList, TIntV *OutValue) {

  printf("BEFORE: Original TIntV list size to %d\n", originalList.Len());
  *OutValue = originalList;
  printf("BEFORE: New TIntV list size = %d\n", OutValue->Len());

}

