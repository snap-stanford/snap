/////////////////////////////////////////////////
// Approximate Neighborhood Function
namespace TAnf {

// interpolate effective diameter
double CalcEffDiam(const TIntFltKdV& DistNbhsCdfV, const double& Percentile) {
  const double EffPairs = Percentile * DistNbhsCdfV.Last().Dat;
  int ValN;
  for (ValN = 0; ValN < DistNbhsCdfV.Len(); ValN++) {
    if (DistNbhsCdfV[ValN].Dat() > EffPairs) {  break; }
  }
  if (ValN >= DistNbhsCdfV.Len()) return DistNbhsCdfV.Last().Key;
  if (ValN == 0) return 1;
  // interpolate
  const double DeltaNbhs = DistNbhsCdfV[ValN].Dat - DistNbhsCdfV[ValN-1].Dat;
  if (DeltaNbhs == 0) return DistNbhsCdfV[ValN].Key;
  return DistNbhsCdfV[ValN-1].Key + (EffPairs - DistNbhsCdfV[ValN-1].Dat)/DeltaNbhs;
}

double CalcEffDiam(const TFltPrV& DistNbhsCdfV, const double& Percentile) {
  TIntFltKdV KdV(DistNbhsCdfV.Len(), 0);
  for (int i = 0; i < DistNbhsCdfV.Len(); i++) {
    KdV.Add(TIntFltKd(int(DistNbhsCdfV[i].Val1()), DistNbhsCdfV[i].Val2));
  }
  return CalcEffDiam(KdV, Percentile);
}

double CalcEffDiamPdf(const TIntFltKdV& DistNbhsPdfV, const double& Percentile) {
  TIntFltKdV CdfV;
  TGUtil::GetCdf(DistNbhsPdfV, CdfV);
  return CalcEffDiam(CdfV, Percentile);
}

double CalcEffDiamPdf(const TFltPrV& DistNbhsPdfV, const double& Percentile) {
  TFltPrV CdfV;
  TGUtil::GetCdf(DistNbhsPdfV, CdfV);
  return CalcEffDiam(CdfV, Percentile);
}

double CalcAvgDiamPdf(const TIntFltKdV& DistNbhsPdfV) {
  double Paths=0, SumLen=0;
  for (int i = 0; i < DistNbhsPdfV.Len(); i++) {
    SumLen += DistNbhsPdfV[i].Key * DistNbhsPdfV[i].Dat;
    Paths += DistNbhsPdfV[i].Dat;
  }
  return SumLen/Paths;
}

double CalcAvgDiamPdf(const TFltPrV& DistNbhsPdfV) {
  double Paths=0, SumLen=0;
  for (int i = 0; i < DistNbhsPdfV.Len(); i++) {
    SumLen += DistNbhsPdfV[i].Val1 * DistNbhsPdfV[i].Val2;
    Paths += DistNbhsPdfV[i].Val2;
  }
  return SumLen/Paths;
}

} // namespace TAnf
