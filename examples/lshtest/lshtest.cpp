#include "Snap.h"
#include "lsh.h"

#define DIM 3

void outputPoint(TFltV Point) {
  printf("(");
  for (int i=0; i<Point.Len(); i++) {
    if (i>0)
      printf(", ");
    printf("%.2f", (double)Point[i]);
  }
  printf(")");
}

int main() {
  TLSHash LSH(7, 7, DIM, TLSHash::EUCLIDEAN);
  LSH.Init();

  TRnd Gen;
  Gen.Randomize();

  TVec<TFltV> DataV;
  for (int i=0; i<1000000; i++) {
    TFltV Datum;
    for (int j=0; j<3; j++) {
      Datum.Add(Gen.GetUniDev()*2100);
    }
    DataV.Add(Datum);
  }
  LSH.AddV(DataV);
  
  TVec<TPair<TFltV, TFltV> > NeighborsV = LSH.GetAllCandidatePairs();
  printf("Number of Candidates: %d\n", NeighborsV.Len());

  NeighborsV = LSH.GetAllNearPairs();
  printf("Number of Close Pairs: %d\n", NeighborsV.Len());
  for (int i=0; i<NeighborsV.Len(); i++) {
    outputPoint(NeighborsV[i].GetVal1());
    printf(" ");
    outputPoint(NeighborsV[i].GetVal2());
    printf("\n");
  }
  return 0;
}
