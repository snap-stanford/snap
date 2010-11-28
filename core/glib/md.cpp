/////////////////////////////////////////////////
// Includes
#include "md.h"
//#include "mdtr.h"

/////////////////////////////////////////////////
// Model
PMd TMd::Load(TSIn& SIn){
  TStr TypeNm(SIn);
  if (TypeNm==TTypeNm<TMdPrior>()){return new TMdPrior(SIn);}
  else if (TypeNm==TTypeNm<TMdBayes>()){return new TMdBayes(SIn);}
//  else if (TypeNm==TTypeNm<TMdTree>()){return new TMdTree(SIn);}
  else {Fail; return NULL;}
}

/////////////////////////////////////////////////
// Model-Bayes
PTbValDs TMdBayes::GetPostrValDs(const PValRet& ValRet, const int& ClassN) const {
  PDmDs DmDs=DmDsV[GetClassNN(ClassN)];
  int CDscs=DmDs->GetCDs()->GetDscs();
  PTbValDs ValDs=new TTbValDs(CDscs);
  double LnSumW=log(DmDs->GetSumW());
  for (int CDsc=0; CDsc<CDscs; CDsc++){
    double PriorCPrb=DmDs->GetCPrb_RelFq(CDsc);
    if (PriorCPrb==0){continue;}
    double LnPriorCPrb=log(PriorCPrb);
    double LnPostrCPrb=LnPriorCPrb;
    for (int AttrN=0; AttrN<GetDmHd()->GetAttrs(); AttrN++){
      TTbVal AVal=ValRet->GetVal(AttrN);
      double AValPrb=DmDs->GetCAVPrb_RelFq(CDsc, AttrN, AVal)/PriorCPrb;
      if (AValPrb==0){
        LnPostrCPrb+=LnPriorCPrb-LnSumW;} // Clark&Nibblet modification
      else {LnPostrCPrb+=log(AValPrb);}
    }
    ValDs->AddVal(TTbVal(CDsc), LnPostrCPrb);
  }
  ValDs->ExpW(); ValDs->Def();
  return ValDs;
}

/////////////////////////////////////////////////
// Model-Nearest-Neighbour
TMdNNbr::TMdNNbr(
 const PDm& _Dm, const PExSet& _ExSet, const PMtr& _Mtr, const int& _Nbrs):
  TMd(_Dm->GetDmHd()), Dm(_Dm), ExSet(_ExSet), Mtr(_Mtr), Nbrs(_Nbrs),
  PriorValDsV(Dm->GetClasses(), 0){
  for (int ClassN=0; ClassN<Dm->GetClasses(); ClassN++){
    PTbValDs PriorValDs=TTbValDs::GetCValDs(ClassN, Dm, ExSet);
    IAssert(AddClassN(ClassN)==PriorValDsV.Add(PriorValDs));
  }
  Def();
}

PTbValDs TMdNNbr::GetPostrValDs(const PValRet& LValRet, const int& ClassN) const {
  TVec<TFltIntKd> DistExKdV;
  TDmValRet& DmValRet=*new TDmValRet(GetDmHd(), Dm); PValRet RValRet(&DmValRet);
  int ExP=ExSet->FFirstExP();
  while (ExSet->FNextExP(ExP)){
    int ExN=ExSet->GetExId(ExP);
    DmValRet.SetExN(ExN);
    double Dist=Mtr->GetDist(LValRet, RValRet);
    DistExKdV.AddSorted(TFltIntKd(Dist, ExN), true, Nbrs);
  }

  int Dscs=Dm->GetClass(ClassN)->GetVarType()->GetDscs();
  PTbValDs ValDs=new TTbValDs(Dscs);
  for (int DistExKdN=0; DistExKdN<DistExKdV.Len(); DistExKdN++){
    TTbVal Val=Dm->GetClassVal(DistExKdV[DistExKdN].Dat, ClassN);
    ValDs->AddVal(Val, 1);
  }
  ValDs->Def();
  return ValDs;
}

