/////////////////////////////////////////////////
// BagOfWords-Model
THash<TStr, TBowMd::TBowMdLoadF> TBowMd::TypeToLoadFH;

PBowMd TBowMd::Load(TSIn& SIn){
  TStr TypeNm(SIn);
  int TypeKeyId=-1;
  if (TypeToLoadFH.IsKey(TypeNm, TypeKeyId)){
    TBowMdLoadF LoadF=TypeToLoadFH[TypeKeyId];
    return (*LoadF())(SIn);
  } else {
    return NULL;
  }
}

bool TBowMd::Reg(const TStr& TypeNm, const TBowMdLoadF& LoadF){
  IAssert(!TypeToLoadFH.IsKey(TypeNm));
  TypeToLoadFH.AddDat(TypeNm, LoadF);
  return true;
}

void TBowMd::SaveXmlCfy(const PSOut& SOut, const TFltStrPrV& WgtCatNmPrV){
  SOut->PutStr("<Classification>"); SOut->PutLn();
  for (int WCPrN=0; WCPrN<WgtCatNmPrV.Len(); WCPrN++){
    double Wgt=WgtCatNmPrV[WCPrN].Val1;
    TStr CatNm=WgtCatNmPrV[WCPrN].Val2;
    //printf("%d. %.3f '%s'\n", 1+WCPrN, Wgt, CatNm.CStr());
    SOut->PutStr("<Category ");
    TChA Ln;
    Ln+=TInt::GetStr(1+WCPrN, "Rank=\"%d\"");
    Ln+=TFlt::GetStr(Wgt, " Weight=\"%.3f\"");
    Ln+=TStr::GetStr(CatNm, " Category=\"%s\"");
    SOut->PutStr(Ln);
    SOut->PutStr("/>"); SOut->PutLn();
  }
  SOut->PutStr("</Classification>"); SOut->PutLn();
}

void TBowMd::SaveTxtCfy(const PSOut& SOut, const TFltStrPrV& WgtCatNmPrV){
  for (int WCPrN=0; WCPrN<WgtCatNmPrV.Len(); WCPrN++){
    double Wgt=WgtCatNmPrV[WCPrN].Val1;
    TStr CatNm=WgtCatNmPrV[WCPrN].Val2;
    //printf("%d. %.3f '%s'\n", 1+WCPrN, Wgt, CatNm.CStr());
    TChA Ln;
    Ln+=TInt::GetStr(1+WCPrN, "%d.");
    Ln+=TFlt::GetStr(Wgt, " %.3f");
    Ln+=TStr::GetStr(CatNm, " '%s'");
    SOut->PutStr(Ln); SOut->PutLn();
  }
}

const TStr TBowMd::BowMdFExt=".BowMd";

PBowMd TBowMd::LoadBin(const TStr& FNm, const PBowDocBs& BowDocBs){
  PBowMd BowMd; {TFIn SIn(FNm); BowMd=Load(SIn);}
  if ((!BowDocBs.Empty())&&(BowMd->GetBowDocBsSig()!=BowDocBs->GetSig())){
    TExcept::Throw("Bow-Model and Bow-Data signatures don't match!");}
  return BowMd;
}

/////////////////////////////////////////////////
// BagOfWords-Multi-Model
bool TBowMultiMd::IsReg=TBowMultiMd::MkReg();

void TBowMultiMd::GetCfy(const PBowSpV& QueryBowSpV, TFltStrPrV& WgtCatNmPrV){
  // create category weights
  TStrFltH CatNmToWgtH;
  // traverse models
  int BowMds=GetBowMds();
  for (int BowMdN=0; BowMdN<BowMds; BowMdN++){
    // get model
    PBowMd BowMd=GetBowMd(BowMdN);
    // get single model classification
    TFltStrPrV LocWgtCatNmPrV;
    BowMd->GetCfy(QueryBowSpV, LocWgtCatNmPrV);
    // upadate category weights
    for (int WCPrN=0; WCPrN<LocWgtCatNmPrV.Len(); WCPrN++){
      double Wgt=LocWgtCatNmPrV[WCPrN].Val1;
      TStr CatNm=LocWgtCatNmPrV[WCPrN].Val2;
      CatNmToWgtH.AddDat(CatNm)+=Wgt;
    }
  }
  // create sorted weigt-category pairs
  CatNmToWgtH.GetDatKeyPrV(WgtCatNmPrV);
  WgtCatNmPrV.Sort(false);
}

/////////////////////////////////////////////////
// BagOfWords-Winnow-Model
bool TBowWinnowMd::IsReg=TBowWinnowMd::MkReg();

PBowMd TBowWinnowMd::New(
 const PBowDocBs& BowDocBs, const TStr& CatNm, const double& Beta){
  // create model
  TBowWinnowMd* WinnowMd=new TBowWinnowMd(BowDocBs); PBowMd BowMd(WinnowMd);
  WinnowMd->CatNm=CatNm;
  WinnowMd->Beta=Beta;
  WinnowMd->VoteTsh=0.5;
  // prepare Winnow parameters
  const double MnExpertWgtSum=1e-15;
  // get cat-id
  int CId=BowDocBs->GetCId(CatNm);
  if (CId==-1){
    TExcept::Throw(TStr::GetStr(CatNm, "Invalid Category Name ('%s')!"));}
  // get training documents
  TIntV TrainDIdV; BowDocBs->GetAllDIdV(TrainDIdV);
  int TrainDocs=TrainDIdV.Len();
  // prepare mini-experts
  int Words=BowDocBs->GetWords();
  WinnowMd->PosExpertWgtV.Gen(Words); WinnowMd->PosExpertWgtV.PutAll(1);
  WinnowMd->NegExpertWgtV.Gen(Words); WinnowMd->NegExpertWgtV.PutAll(1);
  // winnow loop
  double PrevAcc=0; double PrevPrec=0; double PrevRec=0; double PrevF1=0;
  const double MxDiff=-0.005; const int MxWorseIters=3; int WorseIters=0;
  const int MxIters=50; int IterN=0;
  while ((IterN<MxIters)&&(WorseIters<MxWorseIters)){
    IterN++;
    int FalsePos=0; int FalseNeg=0; int TruePos=0; int TrueNeg=0;
    for (int DIdN=0; DIdN<TrainDocs; DIdN++){
      int DId=TrainDIdV[DIdN];
      bool ClassVal=BowDocBs->IsCatInDoc(DId, CId);
      double PosWgt=0; double NegWgt=0;
      double OldSum=0; double NewSum=0;
      int WIds=BowDocBs->GetDocWIds(DId);
      // change only experts of words that occur in the document
      for (int WIdN=0; WIdN<WIds; WIdN++){
        int WId=BowDocBs->GetDocWId(DId, WIdN);
        OldSum+=WinnowMd->PosExpertWgtV[WId]+WinnowMd->NegExpertWgtV[WId];
        // penalize expert giving wrong class prediction
        if (ClassVal){
          WinnowMd->NegExpertWgtV[WId]*=Beta;
        } else {
          WinnowMd->PosExpertWgtV[WId]*=Beta;
        }
        NewSum+=WinnowMd->PosExpertWgtV[WId]+WinnowMd->NegExpertWgtV[WId];
        PosWgt+=WinnowMd->PosExpertWgtV[WId];
        NegWgt+=WinnowMd->NegExpertWgtV[WId];
      }
      // normalize all experts
      if (NewSum>MnExpertWgtSum){
        for (int WIdN=0; WIdN<WIds; WIdN++){
          int WId=BowDocBs->GetDocWId(DId, WIdN);
          WinnowMd->PosExpertWgtV[WId]*=OldSum/NewSum;
          WinnowMd->NegExpertWgtV[WId]*=OldSum/NewSum;
        }
      }
      bool PredClassVal;
      if (PosWgt+NegWgt==0){PredClassVal=TBool::GetRnd();}
      else {PredClassVal=(PosWgt/(PosWgt+NegWgt))>WinnowMd->VoteTsh;}
      if (PredClassVal==ClassVal){
        if (PredClassVal){TruePos++;} else {TrueNeg++;}
      } else {
        if (PredClassVal){FalsePos++;} else {FalseNeg++;}
      }
    }
    // calculate temporary results
    if (TrainDocs==0){break;}
    double Acc=0; double Prec=0; double Rec=0; double F1=0;
    if (TrainDocs>0){
      Acc=100*(TruePos+TrueNeg)/double(TrainDocs);
      if (TruePos+FalsePos>0){
        Prec=(TruePos/double(TruePos+FalsePos));
        Rec=(TruePos/double(TruePos+FalseNeg));
        if (Prec+Rec>0){
          F1=(2*Prec*Rec/(Prec+Rec));
        }
      }
    }
    // check if the current iteration gave worse results then the previous
    if (((Acc-PrevAcc)<MxDiff)||((F1-PrevF1)<MxDiff)||(((Prec-PrevPrec)<MxDiff)&&
     ((Rec-PrevRec)<MxDiff))){WorseIters++;}
    else {WorseIters=0;}
    PrevAcc=Acc; PrevPrec=Prec; PrevRec=Rec; PrevF1=F1;
    printf("%d. Precision:%0.3f   Recall:%0.3f   F1:%0.3f   Accuracy:%0.3f%%\n",
     IterN, Prec, Rec, F1, Acc);
  }
  // return model
  return BowMd;
}

PBowMd TBowWinnowMd::NewMulti(
 const PBowDocBs& BowDocBs, const int& TopCats, const double& Beta){
  // create model
  TBowMultiMd* MultiMd=new TBowMultiMd(BowDocBs); PBowMd BowMd(MultiMd);
  // traverse categories
  TIntStrPrV FqCatNmPrV; BowDocBs->GetTopCatV(TopCats, FqCatNmPrV);
  for (int CatN=0; CatN<FqCatNmPrV.Len(); CatN++){
    // get category data
    TStr CatNm=FqCatNmPrV[CatN].Val2;
    int CId=BowDocBs->GetCId(CatNm);
    // output header
    printf("*** Generating model for category: '%s' %d Docs (%d/%d Cats)\n",
     CatNm.CStr(), BowDocBs->GetCatFq(CId), 1+CId, BowDocBs->GetCats());
    // create model
    PBowMd BowMd=New(BowDocBs, CatNm, Beta);
    // add model to model-set
    MultiMd->AddBowMd(BowMd);
  }
  // return model
  return BowMd;
}

void TBowWinnowMd::GetCfy(const PBowSpV& QueryBowSpV, TFltStrPrV& WgtCatNmPrV){
  // coolect weights-sum
  double PosWgtSum=0; double NegWgtSum=0;
  int WIds=QueryBowSpV->GetWIds();
  for (int WIdN=0; WIdN<WIds; WIdN++){
    int WId; double Wgt; QueryBowSpV->GetWIdWgt(WIdN, WId, Wgt);
    PosWgtSum+=PosExpertWgtV[WId];
    NegWgtSum+=NegExpertWgtV[WId];
  }
  // classify
  WgtCatNmPrV.Clr();
  if (PosWgtSum+NegWgtSum>0){
    double PredWgt=double(PosWgtSum/(PosWgtSum+NegWgtSum))-VoteTsh;
    WgtCatNmPrV.Add(TFltStrPr(PredWgt, CatNm));
    if ((PosWgtSum/(PosWgtSum+NegWgtSum))>VoteTsh){
    } else {
    }
  }
}

