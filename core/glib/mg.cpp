/////////////////////////////////////////////////
// Includes
#include "mg.h"

/////////////////////////////////////////////////
// Model-Generator
TMg::TMg(const PPp& Pp, const PDm& _Dm, const int& _ClassN):
  Dm(TDm::New(Pp->GetPp(TTypeNm<TDm>()), _Dm)),
  ClassN(TClassId::New(Pp->GetPp(TTypeNm<TClassId>()), Dm, _ClassN)){}

const TStr TMg::DNm("Model-Generator");

PPp TMg::GetPp(const TStr& Nm, const TStr& DNm){
  PPp Pp=new TPp(Nm, DNm, ptSel);
  Pp->AddPp(TMgPrior::GetPp(TTypeNm<TMgPrior>(), TMgPrior::DNm));
  Pp->AddPp(TMgBayes::GetPp(TTypeNm<TMgBayes>(), TMgBayes::DNm));
  Pp->AddPp(TMgNNbr::GetPp(TTypeNm<TMgNNbr>(), TMgNNbr::DNm));
  Pp->AddPp(TMgId3::GetPp(TTypeNm<TMgId3>(), TMgId3::DNm));
  return Pp;
}

void TMg::GetSubPp(const PPp& Pp){
  Pp->AddPp(TDm::GetPp(TTypeNm<TDm>(), TDm::DNm));
  Pp->AddPp(TClassId::GetPp(TTypeNm<TClassId>(), TClassId::DNm));
}

PMg TMg::New(const PPp& Pp, const PDm& _Dm, const int& _ClassN){
  if (Pp->GetVal()==TPpVal(TTypeNm<TMgPrior>())){
    return new TMgPrior(Pp->GetSelPp(), _Dm, _ClassN);}
  else if (Pp->GetVal()==TPpVal(TTypeNm<TMgBayes>())){
    return new TMgBayes(Pp->GetSelPp(), _Dm, _ClassN);}
  else if (Pp->GetVal()==TPpVal(TTypeNm<TMgNNbr>())){
    return new TMgNNbr(Pp->GetSelPp(), _Dm, _ClassN);}
  else if (Pp->GetVal()==TPpVal(TTypeNm<TMgId3>())){
    return new TMgId3(Pp->GetSelPp(), _Dm, _ClassN);}
  else {Fail; return NULL;}
}

/////////////////////////////////////////////////
// Model-Generator-Prior
PMd TMgPrior::GetMd(const PExSet& ExSet) const {
  IAssert(TDmCheck(GetDm()).IsClass(GetClassN()));
  PTbValDs ValDs=TTbValDs::GetCValDs(GetClassN(), GetDm(), ExSet);
  return new TMdPrior(GetDm()->GetDmHd(), GetClassN(), ValDs);
}

const TStr TMgPrior::DNm("Prior");

PPp TMgPrior::GetPp(const TStr& Nm, const TStr& DNm){
  PPp Pp=new TPp(Nm, DNm, ptSet);
  TMg::GetSubPp(Pp);
  return Pp;
}

/////////////////////////////////////////////////
// Model-Generator-Bayes
PMd TMgBayes::GetMd(const PExSet& ExSet) const {
  IAssert(TDmCheck(GetDm()).IsDmDsc(GetClassN()));
  PDmDs DmDs=new TGDmDs(GetDm(), GetClassN(), ExSet);
  return new TMdBayes(GetDm()->GetDmHd(), GetClassN(), DmDs);
}

const TStr TMgBayes::DNm("Bayes");

PPp TMgBayes::GetPp(const TStr& Nm, const TStr& DNm){
  PPp Pp=new TPp(Nm, DNm, ptSet);
  TMg::GetSubPp(Pp);
  return Pp;
}

/////////////////////////////////////////////////
// Model-Generator-Nearest-Neighbour
TMgNNbr::TMgNNbr(const PPp& Pp, const PDm& Dm, const int& ClassN):
  TMg(Pp, Dm, ClassN),
  Mtr(TMtr::New(Pp->GetPp(TTypeNm<TMtr>()))),
  Nbrs(Pp->GetValInt("Nbrs")){}

PMd TMgNNbr::GetMd(const PExSet& ExSet) const {
  return new TMdNNbr(GetDm(), ExSet, Mtr, Nbrs);}

const TStr TMgNNbr::DNm("Nearest-Neighbour");

PPp TMgNNbr::GetPp(const TStr& Nm, const TStr& DNm){
  PPp Pp=new TPp(Nm, DNm, ptSet);
  TMg::GetSubPp(Pp);
  Pp->AddPp(TMtr::GetPp(TTypeNm<TMtr>(), TMtr::DNm));
  Pp->AddPpInt("Nbrs", "Neighbours", 1, TInt::Mx, 10);
  return Pp;
}

/////////////////////////////////////////////////
// Model-Generator-Id3
class TMgId3::TSplit{
private:
  int AttrN;
  PTbValSet OrTbValSet;
  double Err;
public:
  TSplit(): AttrN(-1), OrTbValSet(), Err(){}
  TSplit(const int& _AttrN, const PTbValSet& _OrTbValSet, const double& _Err):
    AttrN(_AttrN), OrTbValSet(_OrTbValSet), Err(_Err){}

  TSplit& operator=(const TSplit& Split){
    AttrN=Split.AttrN; OrTbValSet=Split.OrTbValSet; Err=Split.Err;
    return *this;}

  double GetErr(){return Err;}
};

TMgId3::TSplit TMgId3::GetBestSplit(const PExSet&){
  TSplit BestSplit, Split;
  for (int AttrN=0; AttrN<GetDm()->GetAttrs(); AttrN++){
    TTbValSetExp& TbValSetExp=*new TTbValSetExp(loOr);
    PTbValSet TbValSet(&TbValSetExp);
    for (int Dsc=0; Dsc<GetDm()->GetAttr(AttrN)->GetVarType()->GetDscs(); Dsc++){
      TbValSetExp.Add(PTbValSet(new TTbValSetSel(TTbValSetSel(TTbVal(Dsc)))));}
    TSplit Split(AttrN, TbValSet, 0);
    if ((AttrN==0)||(Split.GetErr()<BestSplit.GetErr())){BestSplit=Split;}
  }
  return BestSplit;
}

PDNd TMgId3::GetDNd(const PExSet& ExSet, const PExSet& AllExSet) const {
  if (ExSet->GetExsWgt()==0){
    TMgPrior Mg(GetDm(), GetClassN());
    return new TDNdLeaf(Mg.GetMd(AllExSet));
  } else {
    PTbValDs CValDs=TTbValDs::GetCValDs(GetClassN(), GetDm(), ExSet);
    if (CValDs->IsOneVal()){
      TMgPrior Mg(GetDm(), GetClassN());
      return NULL; //new TDNdLeaf(Mg.GetMd(ExSet), ClassN);
    } else {
//      TSplit Split=GetBestSplit(ExSet);
      return NULL;
    }
  }
}

PMd TMgId3::GetMd(const PExSet&) const {
  IAssert(TDmCheck(GetDm()).IsDmDsc(GetClassN()));
  return NULL;
//**  return new TMdTree(Dm->GetDmHd(), ClassN, GetDNd(ExSet, ExSet));
}

const TStr TMgId3::DNm("Id3");

PPp TMgId3::GetPp(const TStr& Nm, const TStr& DNm){
  PPp Pp=new TPp(Nm, DNm, ptSet);
  TMg::GetSubPp(Pp);
  return Pp;
}

