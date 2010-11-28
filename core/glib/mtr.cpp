/////////////////////////////////////////////////
// Includes
#include "mtr.h"

/////////////////////////////////////////////////
// Metric
const TStr TMtr::DNm("Metric");

PPp TMtr::GetPp(const TStr& Nm, const TStr& DNm){
  PPp Pp=new TPp(Nm, DNm, ptSel);
  Pp->AddPp(TMtrManhattan::GetPp(TTypeNm<TMtrManhattan>(), TMtrManhattan::DNm));
  Pp->AddPp(TMtrEucl::GetPp(TTypeNm<TMtrEucl>(), TMtrEucl::DNm));
  Pp->AddPp(TMtrCos::GetPp(TTypeNm<TMtrCos>(), TMtrCos::DNm));
  Pp->AddPp(TMtrVdm::GetPp(TTypeNm<TMtrVdm>(), TMtrVdm::DNm));
  Pp->PutDfVal(TTypeNm<TMtrManhattan>());
  return Pp;
}

PMtr TMtr::New(const PPp& Pp){
  if (Pp->GetVal()==TPpVal(TTypeNm<TMtrManhattan>())){
    return new TMtrManhattan(Pp->GetSelPp());}
  else if (Pp->GetVal()==TPpVal(TTypeNm<TMtrEucl>())){
    return new TMtrEucl(Pp->GetSelPp());}
  else if (Pp->GetVal()==TPpVal(TTypeNm<TMtrCos>())){
    return new TMtrCos(Pp->GetSelPp());}
  else if (Pp->GetVal()==TPpVal(TTypeNm<TMtrVdm>())){
    return new TMtrVdm(Pp->GetSelPp());}
  else {Fail; return NULL;}
}

/////////////////////////////////////////////////
// Manhattan-Metric
double TMtrManhattan::GetDist(const PValRet& LValRet, const PValRet& RValRet){
  IAssert(LValRet->GetAttrs()==RValRet->GetAttrs());
  double Dist=0;
  for (int AttrN=0; AttrN<LValRet->GetAttrs(); AttrN++){
    if (LValRet->GetVal(AttrN)!=RValRet->GetVal(AttrN)){Dist++;}
  }
  return Dist;
}

const TStr TMtrManhattan::DNm("Manhattan");

/////////////////////////////////////////////////
// Euclidean-Metric
double TMtrEucl::GetDist(const PValRet& LValRet, const PValRet& RValRet){
  IAssert(LValRet->GetAttrs()==RValRet->GetAttrs());
  double Dist=0;
  for (int AttrN=0; AttrN<LValRet->GetAttrs(); AttrN++){
    double LNrmFlt=LValRet->GetNrmFlt(AttrN);
    double RNrmFlt=RValRet->GetNrmFlt(AttrN);
    Dist+=sqr(LNrmFlt-RNrmFlt);
  }
  return sqrt(Dist);
}

const TStr TMtrEucl::DNm("Euclidian");

/////////////////////////////////////////////////
// Cosine-Metric
double TMtrCos::GetDist(const PValRet& LValRet, const PValRet& RValRet){
  IAssert(LValRet->GetAttrs()==RValRet->GetAttrs());
  double Dist=0; double LNorm=0; double RNorm=0;
  for (int AttrN=0; AttrN<LValRet->GetAttrs(); AttrN++){
    double LNrmFlt=LValRet->GetNrmFlt(AttrN);
    double RNrmFlt=RValRet->GetNrmFlt(AttrN);
    Dist+=LNrmFlt*RNrmFlt; LNorm+=sqr(LNrmFlt); RNorm+=sqr(RNrmFlt);
  }
  if (LNorm*RNorm==0){Dist=1;}
  else {Dist=1-Dist/(sqrt(LNorm)*sqrt(RNorm));}
  return Dist;
}

const TStr TMtrCos::DNm("Cosine");

/////////////////////////////////////////////////
// VDM-Metric
double TMtrVdm::GetDist(const PValRet&, const PValRet&){
  Fail; return 0;
}

const TStr TMtrVdm::DNm("VDM");

PPp TMtrVdm::GetPp(const TStr& Nm, const TStr& DNm){
  PPp Pp=new TPp(Nm, DNm, ptSet);
  Pp->AddPp(TPrbEst::GetPp(TTypeNm<TPrbEst>(), TPrbEst::DNm));
  return Pp;
}

