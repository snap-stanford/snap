/////////////////////////////////////////////////
// Approximate Neighborhood Function
namespace TSnap {
namespace TSnapDetail {

// interpolate effective diameter
double CalcEffDiam(const TIntFltKdV& DistNbrsCdfV, const double& Percentile) {
  const double EffPairs = Percentile * DistNbrsCdfV.Last().Dat;
  int ValN;
  for (ValN = 0; ValN < DistNbrsCdfV.Len(); ValN++) {
    if (DistNbrsCdfV[ValN].Dat() > EffPairs) {  break; }
  }
  if (ValN >= DistNbrsCdfV.Len()) return DistNbrsCdfV.Last().Key;
  if (ValN == 0) return 1;
  // interpolate
  const double DeltaNbrs = DistNbrsCdfV[ValN].Dat - DistNbrsCdfV[ValN-1].Dat;
  if (DeltaNbrs == 0) return DistNbrsCdfV[ValN].Key;
  return DistNbrsCdfV[ValN-1].Key + (EffPairs - DistNbrsCdfV[ValN-1].Dat)/DeltaNbrs;
}

double CalcEffDiam(const TFltPrV& DistNbrsCdfV, const double& Percentile) {
  TIntFltKdV KdV(DistNbrsCdfV.Len(), 0);
  for (int i = 0; i < DistNbrsCdfV.Len(); i++) {
    KdV.Add(TIntFltKd(int(DistNbrsCdfV[i].Val1()), DistNbrsCdfV[i].Val2));
  }
  return CalcEffDiam(KdV, Percentile);
}

double CalcEffDiamPdf(const TIntFltKdV& DistNbrsPdfV, const double& Percentile) {
  TIntFltKdV CdfV;
  TGUtil::GetCdf(DistNbrsPdfV, CdfV);
  return CalcEffDiam(CdfV, Percentile);
}

double CalcEffDiamPdf(const TFltPrV& DistNbrsPdfV, const double& Percentile) {
  TFltPrV CdfV;
  TGUtil::GetCdf(DistNbrsPdfV, CdfV);
  return CalcEffDiam(CdfV, Percentile);
}

double CalcAvgDiamPdf(const TIntFltKdV& DistNbrsPdfV) {
  double Paths=0, SumLen=0;
  for (int i = 0; i < DistNbrsPdfV.Len(); i++) {
    SumLen += DistNbrsPdfV[i].Key * DistNbrsPdfV[i].Dat;
    Paths += DistNbrsPdfV[i].Dat;
  }
  return SumLen/Paths;
}

double CalcAvgDiamPdf(const TFltPrV& DistNbrsPdfV) {
  double Paths=0, SumLen=0;
  for (int i = 0; i < DistNbrsPdfV.Len(); i++) {
    SumLen += DistNbrsPdfV[i].Val1 * DistNbrsPdfV[i].Val2;
    Paths += DistNbrsPdfV[i].Val2;
  }
  return SumLen/Paths;
}

} // namespace TSnapDetail
} // namespace TSnap

