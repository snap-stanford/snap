/////////////////////////////////////////////////
// Probability-Estimator
PPrbEst TPrbEst::Load(TSIn& SIn){
  TStr TypeNm(SIn);
  if (TypeNm==TTypeNm<TPrbEstRelFq>()){return new TPrbEstRelFq(SIn);}
  else if (TypeNm==TTypeNm<TPrbEstLaplace>()){return new TPrbEstLaplace(SIn);}
  else if (TypeNm==TTypeNm<TPrbEstQuinLaplace>()){return new TPrbEstQuinLaplace(SIn);}
  else if (TypeNm==TTypeNm<TPrbEstMEst>()){return new TPrbEstMEst(SIn);}
  else {Fail; return NULL;}
}

const TStr TPrbEst::DNm("Probability Estimate");

PPp TPrbEst::GetPp(const TStr& Nm, const TStr& DNm){
  PPp Pp=new TPp(Nm, DNm, ptSel);
  Pp->AddPp(TPrbEstRelFq::GetPp(TTypeNm<TPrbEstRelFq>(), TPrbEstRelFq::DNm));
  Pp->AddPp(TPrbEstLaplace::GetPp(TTypeNm<TPrbEstLaplace>(), TPrbEstLaplace::DNm));
  Pp->AddPp(TPrbEstQuinLaplace::GetPp(TTypeNm<TPrbEstQuinLaplace>(), TPrbEstQuinLaplace::DNm));
  Pp->AddPp(TPrbEstMEst::GetPp(TTypeNm<TPrbEstMEst>(), TPrbEstMEst::DNm));
  Pp->PutDfVal(TTypeNm<TPrbEstRelFq>());
  return Pp;
}

PPrbEst TPrbEst::New(const PPp& Pp){
  if (Pp->GetVal()==TPpVal(TTypeNm<TPrbEstRelFq>())){
    return new TPrbEstRelFq(Pp->GetSelPp());}
  else if (Pp->GetVal()==TPpVal(TTypeNm<TPrbEstLaplace>())){
    return new TPrbEstLaplace(Pp->GetSelPp());}
  else if (Pp->GetVal()==TPpVal(TTypeNm<TPrbEstQuinLaplace>())){
    return new TPrbEstQuinLaplace(Pp->GetSelPp());}
  else if (Pp->GetVal()==TPpVal(TTypeNm<TPrbEstMEst>())){
    return new TPrbEstMEst(Pp->GetSelPp());}
  else {Fail; return NULL;}
}

PPrbEst TPrbEst::GetRelFq(){
  return PPrbEst(new TPrbEstRelFq());
}

/////////////////////////////////////////////////
// Probability-Estimator-Relative-Frequency
const TStr TPrbEstRelFq::DNm("Relative-Frequency");

/////////////////////////////////////////////////
// Probability-Estimator-Laplace
const TStr TPrbEstLaplace::DNm("Laplace");

/////////////////////////////////////////////////
// Probability-Estimator-Quinlan-Laplace
const TStr TPrbEstQuinLaplace::DNm("Quinlan-Laplace");

/////////////////////////////////////////////////
// Probability-Estimator-M-Estimate
const double TPrbEstMEst::DfMParam=2;

const TStr TPrbEstMEst::DNm("M-Estimate");

PPp TPrbEstMEst::GetPp(const TStr& Nm, const TStr& DNm){
  PPp Pp=new TPp(Nm, DNm, ptSet);
  Pp->AddPpFlt("MParam", "Parameter m", 0, TFlt::Mx, DfMParam);
  return Pp;
}

