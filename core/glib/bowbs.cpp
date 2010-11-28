/////////////////////////////////////////////////
// BagOfWords-Keywords-Set
void TBowKWordSet::GetWordStrV(TStrV& WordStrV) const {
  WordStrV.Clr(); int KWords=GetKWords();
  for (int KWordN=0; KWordN<KWords; KWordN++){
    WordStrV.Add(GetKWordStr(KWordN));
  }
}

TStr TBowKWordSet::GetKWordsStr() const {
  TChA ResChA; int KWords=GetKWords();
  for (int KWordN=0; KWordN<KWords; KWordN++){
    if (KWordN>0){ResChA+=", ";}
    ResChA+=GetKWordStr(KWordN);
  }
  return ResChA;
}

void TBowKWordSet::SortByStr(){
  TStrFltPrV WStrWWgtPrV;
  WStrToWWgtH.GetKeyDatPrV(WStrWWgtPrV);
  WStrWWgtPrV.Sort();
  WStrToWWgtH.Clr();
  for (int WordN=0; WordN<WStrWWgtPrV.Len(); WordN++){
    WStrToWWgtH.AddDat(WStrWWgtPrV[WordN].Val1, WStrWWgtPrV[WordN].Val2);
  }
}

void TBowKWordSet::SortByWgt(){
  TFltStrPrV WWgtWStrPrV;
  WStrToWWgtH.GetDatKeyPrV(WWgtWStrPrV);
  WWgtWStrPrV.Sort(false);
  WStrToWWgtH.Clr();
  for (int WordN=0; WordN<WWgtWStrPrV.Len(); WordN++){
    WStrToWWgtH.AddDat(WWgtWStrPrV[WordN].Val2, WWgtWStrPrV[WordN].Val1);
  }
}

PBowKWordSet TBowKWordSet::GetTopKWords(const int& MxKWords, const double& WgtSumPrc) const {
  TFltStrPrV WWgtWStrPrV;
  WStrToWWgtH.GetDatKeyPrV(WWgtWStrPrV);
  WWgtWStrPrV.Sort(false);
  double WgtSum=0;
  for (int WordN=0; WordN<WWgtWStrPrV.Len(); WordN++){
    WgtSum+=WWgtWStrPrV[WordN].Val1;
  }
  PBowKWordSet KWordSet=TBowKWordSet::New(GetNm());
  double WgtSumSF=0;
  for (int WordN=0; WordN<WWgtWStrPrV.Len(); WordN++){
    KWordSet->AddKWord(WWgtWStrPrV[WordN].Val2, WWgtWStrPrV[WordN].Val1);
    WgtSumSF+=WWgtWStrPrV[WordN].Val1;
    if ((MxKWords!=-1)&&(KWordSet->GetKWords()>=MxKWords)){break;}
    if ((WgtSum>0)&&(WgtSumSF/WgtSum>WgtSumPrc)){break;}
  }
  return KWordSet;
}

void TBowKWordSet::SaveTxt(const PSOut& SOut) const {
  SOut->PutStr(TStr::Fmt("'%s':", GetNm().CStr()));
  int KWords=GetKWords();
  for (int KWordN=0; KWordN<KWords; KWordN++){
    TStr KWordStr=GetKWordStr(KWordN);
    double KWordWgt=GetKWordWgt(KWordN);
    SOut->PutStr(TStr::Fmt(" [%s:%.5f]", KWordStr.CStr(), KWordWgt));
  }
}

void TBowKWordSet::SaveXml(const PSOut& SOut) const {
  int KWords=GetKWords();
  SOut->PutStr(TStr::Fmt("<KeywordSet Nm=\"%s\">", TXmlLx::GetXmlStrFromPlainStr(GetNm()).CStr()));
  for (int KWordN=0; KWordN<KWords; KWordN++){
    TStr KWordStr=GetKWordStr(KWordN);
    TStr KWordXmlStr=TXmlLx::GetXmlStrFromPlainStr(KWordStr);
    double KWordWgt=GetKWordWgt(KWordN);
    SOut->PutStr(TStr::Fmt("<Keyword Str=\"%s\" Wgt=\"%g\"/>", KWordXmlStr.CStr(), KWordWgt));
  }
  SOut->PutStr("</KeywordSet>");
}

/////////////////////////////////////////////////
// BagOfWords-Keywords-Base
void TBowKWordBs::SaveTxt(const PSOut& SOut) const {
  int KWordSets=GetKWordSets();
  for (int KWordSetN=0; KWordSetN<KWordSets; KWordSetN++){
    PBowKWordSet KWordSet=GetKWordSet(KWordSetN);
    KWordSet->SaveTxt(SOut);
    SOut->PutLn();
  }
}

void TBowKWordBs::SaveXml(const PSOut& SOut) const {
  SOut->PutStr("<KeywordBase>"); SOut->PutLn();
  int KWordSets=GetKWordSets();
  for (int KWordSetN=0; KWordSetN<KWordSets; KWordSetN++){
    PBowKWordSet KWordSet=GetKWordSet(KWordSetN);
    KWordSet->SaveXml(SOut);
    SOut->PutLn();
  }
  SOut->PutStr("</KeywordBase>");
}

/////////////////////////////////////////////////
// BagOfWords-Sparse-Vector
TBowSpV::TBowSpV(const int& _DId, const TFltV& FullVec,
        const double& Eps): DId(_DId) {

    double SqrSum = 0.0;
    for (int EltN = 0; EltN < FullVec.Len(); EltN++) {
        const double EltVal = FullVec[EltN];
        if (TFlt::Abs(EltVal) > Eps) {
            SqrSum += TMath::Sqr(EltVal);
            WIdWgtKdV.Add(TIntSFltKd(EltN, (sdouble)EltVal));
        }
    }
    Norm = sqrt(SqrSum);
}

TBowSpV::TBowSpV(const int& DId, const TIntFltKdV& SpV) {
    double SqrSum = 0.0;
    WIdWgtKdV.Gen(SpV.Len(), 0);
    for (int WIdN = 0; WIdN < SpV.Len(); WIdN++) {
        WIdWgtKdV.Add(TIntSFltKd(SpV[WIdN].Key, sdouble(SpV[WIdN].Dat.Val)));
    }
    Norm = sqrt(SqrSum);
}

void TBowSpV::PutUnitNorm(){
  int WIds=GetWIds();
  // get suqare-weight-sum
  double SqWgtSum=0;
  for (int WIdN=0; WIdN<WIds; WIdN++){
    SqWgtSum+=TMath::Sqr(WIdWgtKdV[WIdN].Dat);
  }
  if (SqWgtSum>0){
    // normalize weights
    for (int WIdN=0; WIdN<WIds; WIdN++){
      WIdWgtKdV[WIdN].Dat=(sdouble)sqrt(TMath::Sqr(WIdWgtKdV[WIdN].Dat)/SqWgtSum);
    }
    // assign unit-norm
    Norm=1.0;
  } else {
    // assign unit-norm
    Norm=0.0;
  }
}

double TBowSpV::GetNorm(){
  if (double(Norm)==-1){
    double SqWgtSum=0;
    int WIds=GetWIds();
    for (int WIdN=0; WIdN<WIds; WIdN++){
      SqWgtSum+=TMath::Sqr(WIdWgtKdV[WIdN].Dat);}
    Norm=sqrt(SqWgtSum);
  }
  return Norm;
}

void TBowSpV::GetWordStrWgtPrV(const PBowDocBs& BowDocBs,
 const int& TopWords, const double& TopWordsWgtPrc,
 TStrFltPrV& WordStrWgtPrV) const {
  // collect words&weights for sorting
  int WIds=GetWIds(); double WordWgtSum=0;
  TFltIntKdV WgtWIdKdV(WIds, 0);
  for (int WIdN=0; WIdN<WIds; WIdN++){
    int WId; double Wgt; GetWIdWgt(WIdN, WId, Wgt);
    WgtWIdKdV.Add(TFltIntKd(Wgt, WId));
    WordWgtSum+=Wgt;
  }
  WgtWIdKdV.Sort(false);
  // generate vector
  WordStrWgtPrV.Clr();
  double WordWgtSumSF=0;
  {for (int WIdN=0; WIdN<WIds; WIdN++){
    // check stopping criteria
    if ((TopWords!=-1)&&(WIdN>=TopWords)){break;}
    if ((WordWgtSum>0)&&(WordWgtSumSF/WordWgtSum>TopWordsWgtPrc)){break;}
    // get word-info
    int WId=WgtWIdKdV[WIdN].Dat;
    double WordWgt=WgtWIdKdV[WIdN].Key;
    // get word string
    TStr WordStr;
    if (BowDocBs.Empty()){WordStr=TInt::GetStr(WId);}
    else {WordStr=BowDocBs->GetWordStr(WId);}
    // update weights-sum
    WordWgtSumSF+=WordWgt;
    // add pair
    WordStrWgtPrV.Add(TStrFltPr(WordStr, WordWgt));
  }}
}

PBowKWordSet TBowSpV::GetKWordSet(const PBowDocBs& BowDocBs) const {
  TStrFltPrV WordStrWgtPrV; GetWordStrWgtPrV(BowDocBs, -1, 1.0, WordStrWgtPrV);
  PBowKWordSet KWordSet=TBowKWordSet::New();
  for (int WordN=0; WordN<WordStrWgtPrV.Len(); WordN++){
    TStr WStr=WordStrWgtPrV[WordN].Val1;
    double WWgt=WordStrWgtPrV[WordN].Val2;
    KWordSet->AddKWord(WStr, WWgt);
  }
  return KWordSet;
}

void TBowSpV::GetIntFltKdV(TIntFltKdV& SpV) const {
    const int Wds = WIdWgtKdV.Len(); SpV.Gen(Wds, 0);
    for (int WdN = 0; WdN < Wds; WdN++) {
        const TBowWIdWgtKd& WIdWgt = WIdWgtKdV[WdN];
        SpV.Add(TIntFltKd(WIdWgt.Key, WIdWgt.Dat.Val));
    }
}

void TBowSpV::CutLowWgtWords(const double& CutWordWgtSumPrc){
  if (CutWordWgtSumPrc<=0){return;}
  int WIds=GetWIds();
  // collect weights-sum and weights-vector
  double WgtSum=0; TFltV WgtV(WIds, 0);
  for (int WIdN=0; WIdN<WIds; WIdN++){
    double Wgt=WIdWgtKdV[WIdN].Dat;
    WgtSum+=Wgt; WgtV.Add(Wgt);
  }
  // determine minimal weight
  WgtV.Sort();
  double CutWgtSum=CutWordWgtSumPrc*WgtSum;
  double CutWgt=-1; int NonCutWgts=-1;
  for (int WgtN=0; WgtN<WIds; WgtN++){
    CutWgtSum-=WgtV[WgtN];
    if (CutWgtSum<=0){
      CutWgt=WgtV[WgtN]; NonCutWgts=WIds-WgtN; break;}
  }
  // create new word-id-weights vector
  if (NonCutWgts!=-1){
    TBowWIdWgtKdV NewWIdWgtKdV(NonCutWgts, 0);
    for (int WIdN=0; WIdN<WIds; WIdN++){
      double Wgt=WIdWgtKdV[WIdN].Dat;
      if (Wgt>=CutWgt){
        NewWIdWgtKdV.Add(WIdWgtKdV[WIdN]);
      }
    }
    // define new sparse vector
    Norm=-1;
    WIdWgtKdV.MoveFrom(NewWIdWgtKdV);
  }
}

TStr TBowSpV::GetStr(const PBowDocBs& BowDocBs,
 const int& TopWords, const double& TopWordsWgtPrc, const TStr& SepStr,
 const bool& ShowWeightsP, const bool& KeepUndelineP) const {
  // collect ordered limited word/weights-vector
  TStrFltPrV WordStrWgtPrV;
  GetWordStrWgtPrV(BowDocBs, TopWords, TopWordsWgtPrc, WordStrWgtPrV);
  // create string
  TChA ChA;
  for (int WordN=0; WordN<WordStrWgtPrV.Len(); WordN++){
    TStr WordStr=WordStrWgtPrV[WordN].Val1;
    if (!KeepUndelineP){
      WordStr.ChangeChAll('_', ' ');}
    double WordWgt=WordStrWgtPrV[WordN].Val2;
    if (!ShowWeightsP) WordStr.ToLc();
    if (!ChA.Empty()){ChA+=SepStr;}
    if (ShowWeightsP){ChA+='[';}
    ChA+=WordStr;
    if (ShowWeightsP) ChA+=TFlt::GetStr(WordWgt, ":%g]");
  }
  return ChA;
}

void TBowSpV::SaveTxt(const PSOut& SOut, const PBowDocBs& BowDocBs,
 const int& TopWords, const double& TopWordsWgtPrc, const char& SepCh) const {
  TChA SepStr; SepStr += SepCh; //B: HACK
  TStr Str=GetStr(BowDocBs, TopWords, TopWordsWgtPrc, SepStr);
  SOut->PutStr(Str);
  SOut->PutLn();
}

void TBowSpV::SaveXml(const PSOut& SOut, const PBowDocBs& BowDocBs) const {
  // collect & sort words
  int WIds=GetWIds();
  TFltIntKdV WgtWIdKdV(WIds, 0);
  for (int WIdN=0; WIdN<WIds; WIdN++){
    int WId; double Wgt; GetWIdWgt(WIdN, WId, Wgt);
    WgtWIdKdV.Add(TFltIntKd(Wgt, WId));
  }
  WgtWIdKdV.Sort(false);
  // save xml
  SOut->PutStr("<SparseVec>");
  {for (int WIdN=0; WIdN<WIds; WIdN++){
    int WId=WgtWIdKdV[WIdN].Dat;
    double Wgt=WgtWIdKdV[WIdN].Key;
    TChA WordChA;
    WordChA+="<Word";
    WordChA+=" Id=\""; WordChA+=TInt::GetStr(WId); WordChA+='\"';
    if (!BowDocBs.Empty()){
      WordChA+=" Str=\""; WordChA+=BowDocBs->GetWordStr(WId); WordChA+='\"';
    }
    WordChA+=" Wgt=\""; WordChA+=TFlt::GetStr(Wgt); WordChA+='\"';
    WordChA+="/>";
    SOut->PutStr(WordChA);
  }}
  SOut->PutStr("</SparseVec>");
}

/////////////////////////////////////////////////
// BagOfWords-Similarity-Matrix
double TBowSimMtx::GetSim(const int& DId1, const int& DId2) const {
  if ((DId1==-1)||(DId2==-1)){return 0;}
  int MtxDId1=MtxDIdV[DId1]; int MtxDId2=MtxDIdV[DId2];
  if (MtxDId1>MtxDId2){TInt::Swap(MtxDId1, MtxDId2);}
  TFlt Sim=0;
  DIdPrToSimH.IsKeyGetDat(TIntPr(MtxDId1, MtxDId2), Sim);
  return Sim;
}

PBowSimMtx TBowSimMtx::LoadTxt(const TStr& FNm){
  // create distance matrix object
  PBowSimMtx BowSimMtx=TBowSimMtx::New();
  // open file & prepare lexer
  PSIn SIn=TFIn::New(FNm);
  TIntH MtxDIdH;
  TILx Lx(SIn, TFSet()|iloExcept);
  while (Lx.GetSym(syLBracket, syEof)!=syEof){
    // read input data of form [eid1:eid2=dist]
    int MtxDId1=Lx.GetInt(); Lx.GetSym(syColon);
    int MtxDId2=Lx.GetInt(); Lx.GetSym(syEq);
    double Sim=Lx.GetFlt(); Lx.GetSym(syRBracket);
    // add data to matrix
    if (MtxDId1>MtxDId2){TInt::Swap(MtxDId1, MtxDId2);}
    BowSimMtx->DIdPrToSimH.AddDat(TIntPr(MtxDId1, MtxDId2), Sim);
    MtxDIdH.AddKey(MtxDId1); MtxDIdH.AddKey(MtxDId2);
  }
  // assign doc-ids
  int MtxDIds=MtxDIdH.Len();
  BowSimMtx->MtxDIdV.Gen(MtxDIdH.Len(), 0);
  for (int MtxDIdN=0; MtxDIdN<MtxDIds; MtxDIdN++){
    BowSimMtx->MtxDIdV.Add(MtxDIdH.GetKey(MtxDIdN));}
  BowSimMtx->MtxDIdV.Sort();
  // return distance matrix object
  return BowSimMtx;
}

/////////////////////////////////////////////////
// BagOfWords-Similarity
double TBowSim::GetSim(const int& DId1, const int& DId2) const {
  IAssert(SimType==bstMtx);
  return SimMtx->GetSim(DId1, DId2);
}

double TBowSim::GetSim(const PBowSpV& SpV1, const PBowSpV& SpV2) const {
  switch (SimType){
    case bstBlock: return GetBlockSim(SpV1, SpV2);
    case bstEucl: return GetEuclSim(SpV1, SpV2);
    case bstCos: return GetCosSim(SpV1, SpV2);
    case bstMtx: return SimMtx->GetSim(SpV1->GetDId(), SpV2->GetDId());
    default: Fail; return 0;
  }
}

double TBowSim::GetSim(const TBowSpVV& SpVV1, const TBowSpVV& SpVV2) const {
  double Sim=0; int Sims=0;
  PMom SimMom=TMom::New();
  for (int SpVN1=0; SpVN1<SpVV1.Len(); SpVN1++){
    for (int SpVN2=0; SpVN2<SpVV2.Len(); SpVN2++){
      Sim+=GetSim(SpVV1[SpVN1], SpVV2[SpVN2]); Sims++;
      SimMom->Add(GetSim(SpVV1[SpVN1], SpVV2[SpVN2]));
    }
  }
  if (Sims>0){Sim/=Sims;}
  SimMom->Def();
  Sim=SimMom->GetMn();
  return Sim;
}

double TBowSim::GetBlockSim(const PBowSpV& SpV1, const PBowSpV& SpV2) {
  // prepare similarity
  double Sim=0;
  // prepare shortcuts
  int WIds1=SpV1->GetWIds();
  int WIds2=SpV2->GetWIds();
  // search for equal words in both documents
  int WIdN1=0; int WIdN2=0;
  while ((WIdN1<WIds1)&&(WIdN2<WIds2)){
    int WId1=SpV1->GetWId(WIdN1);
    int WId2=-1;
    forever {
      if (WIdN2>=WIds2){break;}
      WId2=SpV2->GetWId(WIdN2);
      if (WId2>=WId1){break;}
      WIdN2++;
    }
    if ((WIdN2<WIds2)&&(WId1==WId2)){
      double WordWgt1=SpV1->GetWgt(WIdN1); WIdN1++;
      double WordWgt2=SpV2->GetWgt(WIdN2); WIdN2++;
      if (WordWgt1==WordWgt2){Sim++;}
    } else {
      WIdN1++;
    }
  }
  // return similarity
  return Sim;
}

double TBowSim::GetEuclSim(const PBowSpV& SpV1, const PBowSpV& SpV2) {
  // prepare shortcuts
  int WIds1=SpV1->GetWIds();
  int WIds2=SpV2->GetWIds();
  // search for equal words in both documents
  double SqDifSum=0;
  int WIdN1=0; int WIdN2=0;
  while ((WIdN1<WIds1)&&(WIdN2<WIds2)){
    int WId1=SpV1->GetWId(WIdN1);
    int WId2=SpV2->GetWId(WIdN2);
    if (WId1==WId2){
      double WordWgt1=SpV1->GetWgt(WIdN1); WIdN1++;
      double WordWgt2=SpV2->GetWgt(WIdN2); WIdN2++;
      SqDifSum+=TMath::Sqr(WordWgt1-WordWgt2);
    } else
    if (WId1<WId2){
      double WordWgt1=SpV1->GetWgt(WIdN1); WIdN1++;
      double WordWgt2=0;
      SqDifSum+=TMath::Sqr(WordWgt1-WordWgt2);
    } else {
      // WId1>WId2
      double WordWgt1=0;
      double WordWgt2=SpV2->GetWgt(WIdN2); WIdN2++;
      SqDifSum+=TMath::Sqr(WordWgt1-WordWgt2);
    }
  }
  for (int RestWIdN1=WIdN1; RestWIdN1<WIds1; RestWIdN1++){
    double WordWgt1=SpV1->GetWgt(RestWIdN1);
    SqDifSum+=TMath::Sqr(WordWgt1);
  }
  for (int RestWIdN2=WIdN2; RestWIdN2<WIds2; RestWIdN2++){
    double WordWgt2=SpV2->GetWgt(RestWIdN2);
    SqDifSum+=TMath::Sqr(WordWgt2);
  }
  // return results
  double Sim=-sqrt(SqDifSum);
  return Sim;
}

double TBowSim::GetCosSim(const PBowSpV& SpV1, const PBowSpV& SpV2) {
  // prepare shortcuts
  int WIds1=SpV1->GetWIds();
  int WIds2=SpV2->GetWIds();
  // search for equal words in both documents
  double WordWgtProdSum=0; int IntsWords=0;
  int WIdN1=0; int WIdN2=0;
  while ((WIdN1<WIds1)&&(WIdN2<WIds2)){
    int WId1=SpV1->GetWId(WIdN1);
    int WId2=-1;
    forever {
      if (WIdN2>=WIds2){break;}
      WId2=SpV2->GetWId(WIdN2);
      if (WId2>=WId1){break;}
      WIdN2++;
    }
    if ((WIdN2<WIds2)&&(WId1==WId2)){
      double WordWgt1=SpV1->GetWgt(WIdN1); WIdN1++;
      double WordWgt2=SpV2->GetWgt(WIdN2); WIdN2++;
      double WordWgtProd=WordWgt1*WordWgt2;
      WordWgtProdSum+=WordWgtProd; IntsWords++;
    } else {
      WIdN1++;
    }
  }
  // return results
  double Norm1=SpV1->GetNorm();
  double Norm2=SpV2->GetNorm();
  double Sim;
  if (Norm1*Norm2==0){
    Sim=0;
  } else {
    Sim=WordWgtProdSum/(Norm1*Norm2);
  }
  return Sim;
}

double TBowSim::GetCosSim(
 const PBowSpV& SpV1, const PBowSpV& SpV2, TFltIntPrV& WgtWIdPrV) {
  // prepare shortcuts
  int WIds1=SpV1->GetWIds();
  int WIds2=SpV2->GetWIds();
  // search for equal words in both documents
  double WordWgtProdSum=0; int IntsWords=0;
  int WIdN1=0; int WIdN2=0; WgtWIdPrV.Clr();
  while ((WIdN1<WIds1)&&(WIdN2<WIds2)){
    int WId1=SpV1->GetWId(WIdN1);
    int WId2=-1;
    forever {
      if (WIdN2>=WIds2){break;}
      WId2=SpV2->GetWId(WIdN2);
      if (WId2>=WId1){break;}
      WIdN2++;
    }
    if ((WIdN2<WIds2)&&(WId1==WId2)){
      double WordWgt1=SpV1->GetWgt(WIdN1); WIdN1++;
      double WordWgt2=SpV2->GetWgt(WIdN2); WIdN2++;
      double WordWgtProd=WordWgt1*WordWgt2;
      WordWgtProdSum+=WordWgtProd; IntsWords++;
      WgtWIdPrV.Add(TFltIntPr(WordWgtProd, WId1));
    } else {
      WIdN1++;
    }
  }
  // return results
  double Norm1=SpV1->GetNorm();
  double Norm2=SpV2->GetNorm();
  double Sim;
  if (Norm1*Norm2==0){
    Sim=0;
  } else {
    Sim=WordWgtProdSum/(Norm1*Norm2);
  }
  return Sim;
}

TBowSimType TBowSim::GetSimType(const TStr& Nm){
  TStr UcNm=Nm.GetUc();
  if (UcNm=="UNDEF"){return bstUndef;}
  else if (UcNm=="BLOCK"){return bstBlock;}
  else if (UcNm=="EUCL"){return bstEucl;}
  else if (UcNm=="COS"){return bstCos;}
  else if (UcNm=="MTX"){return bstMtx;}
  else {return bstUndef;}
}

/////////////////////////////////////////////////
// BagOfWords-Document-Weights-Base
PBowDocWgtBs TBowDocWgtBs::New(
 const PBowDocBs& BowDocBs, const TBowWordWgtType& _WordWgtType,
 const double& _CutWordWgtSumPrc, const int& _MnWordFq,
 const TIntV& _DIdV, const TIntV& _BaseDIdV, const PNotify& Notify){
  // create document-weights-base
  PBowDocWgtBs DocWgtBs=TBowDocWgtBs::New(BowDocBs->GetSig());
  // build-parameters
  DocWgtBs->WordWgtType=_WordWgtType;
  DocWgtBs->CutWordWgtSumPrc=_CutWordWgtSumPrc;
  DocWgtBs->MnWordFq=_MnWordFq;
  // create document-id vector (DIdV)
  if (_DIdV.Empty()){BowDocBs->GetAllDIdV(DocWgtBs->DIdV);}
  else {DocWgtBs->DIdV=_DIdV;}
  // shortcuts
  int Docs=DocWgtBs->GetDocs();
  int AllDocs=BowDocBs->GetDocs();
  int AllWords=BowDocBs->GetWords();
  // calculate different weighting schemas
  if ((DocWgtBs->WordWgtType==bwwtEq)||(DocWgtBs->WordWgtType==bwwtNrmEq)){
    // copy sparse vectors from document base
    DocWgtBs->WordFqV.Gen(AllWords);
    DocWgtBs->DocSpVV.Gen(AllDocs);
    for (int DIdN=0; DIdN<Docs; DIdN++){
      TNotify::OnNotify(Notify, ntInfo, TStr("Computing weights (")+TInt::GetStr(DIdN+1)+"/"+TInt::GetStr(Docs)+")...");
      // get document info
      int DId=DocWgtBs->GetDId(DIdN);
      int DocWIds=BowDocBs->GetDocWIds(DId);
      // copy document sparse vector
      DocWgtBs->DocSpVV[DId]=BowDocBs->GetDocSpV(DId);
      // calculate & add words weights
      for (int DocWIdN=0; DocWIdN<DocWIds; DocWIdN++){
        int DocWId; double DocWordFq;
        BowDocBs->GetDocWIdFq(DId, DocWIdN, DocWId, DocWordFq);
        DocWgtBs->WordFqV[DocWId]+=DocWordFq;
      }
      // put unit-norm
      if (DocWgtBs->WordWgtType==bwwtNrmEq) {
        DocWgtBs->DocSpVV[DId]->PutUnitNorm();
      }
    }
  } else if ((DocWgtBs->WordWgtType==bwwtBin)||(DocWgtBs->WordWgtType==bwwtNrmBin)) {
    // calculate word-weights for documents (DocSpVV)
    DocWgtBs->WordFqV.Gen(AllWords);
    DocWgtBs->DocSpVV.Gen(AllDocs);
    for (int DIdN=0; DIdN<Docs; DIdN++){
      TNotify::OnNotify(Notify, ntInfo, TStr("Computing weights (")+TInt::GetStr(DIdN+1)+"/"+TInt::GetStr(Docs)+")...");
      // get document info
      int DId=DocWgtBs->GetDId(DIdN);
      int DocWIds=BowDocBs->GetDocWIds(DId);
      // create document sparse vector
      DocWgtBs->DocSpVV[DId]=TBowSpV::New(DId, DocWIds);
      // calculate & add words weights
      for (int DocWIdN=0; DocWIdN<DocWIds; DocWIdN++){
        int DocWId; double DocWordFq;
        BowDocBs->GetDocWIdFq(DId, DocWIdN, DocWId, DocWordFq);
        DocWgtBs->WordFqV[DocWId]+=DocWordFq;
        DocWgtBs->DocSpVV[DId]->AddWIdWgt(DocWId, 1.0);
      }
      // put unit-norm
      if (DocWgtBs->WordWgtType==bwwtNrmBin) {
        DocWgtBs->DocSpVV[DId]->PutUnitNorm();
      }
    }
  } else if (DocWgtBs->WordWgtType==bwwtNrm01){
    // calculate word-weights for documents (DocSpVV)
    DocWgtBs->WordFqV.Gen(AllWords);
    DocWgtBs->DocSpVV.Gen(AllDocs);
    for (int DIdN=0; DIdN<Docs; DIdN++){
      TNotify::OnNotify(Notify, ntInfo, TStr("Computing weights (")+TInt::GetStr(DIdN+1)+"/"+TInt::GetStr(Docs)+")...");
      // get document info
      int DId=DocWgtBs->GetDId(DIdN);
      int DocWIds=BowDocBs->GetDocWIds(DId);
      // create document sparse vector
      DocWgtBs->DocSpVV[DId]=TBowSpV::New(DId, DocWIds);
      // calculate & add words weights
      for (int DocWIdN=0; DocWIdN<DocWIds; DocWIdN++){
        int DocWId; double DocWordFq;
        BowDocBs->GetDocWIdFq(DId, DocWIdN, DocWId, DocWordFq);
        DocWgtBs->WordFqV[DocWId]+=DocWordFq;
        double MnWIdFq=BowDocBs->GetWordMnVal(DocWId);
        double MxWIdFq=BowDocBs->GetWordMxVal(DocWId);
        if (MnWIdFq!=MxWIdFq){
          double DocWordWgt=(DocWordFq-MnWIdFq)/(MxWIdFq-MnWIdFq);
          DocWgtBs->DocSpVV[DId]->AddWIdWgt(DocWId, DocWordWgt);
        }
      }
    }
  } else if (
   (DocWgtBs->WordWgtType==bwwtNrmTFIDF)||
   (DocWgtBs->WordWgtType==bwwtLogDFNrmTFIDF)){
    // prepare base documents
    TIntV BaseDIdV;
    if (_BaseDIdV.Empty()){BaseDIdV=DocWgtBs->DIdV;}
    else {BaseDIdV=_BaseDIdV;}
    int BaseDocs=BaseDIdV.Len();
    // create document-frequency for words
    int Words=BowDocBs->GetWords();
    DocWgtBs->WordFqV.Gen(Words);
    for (int DIdN=0; DIdN<BaseDocs; DIdN++){
      TNotify::OnNotify(Notify, ntInfo, TStr("Computing weights (")+TInt::GetStr(DIdN+1)+"/"+TInt::GetStr(BaseDocs)+")...");
      int DId=BaseDIdV[DIdN];
      int DocWIds=BowDocBs->GetDocWIds(DId);
      for (int DocWIdN=0; DocWIdN<DocWIds; DocWIdN++){
        int DocWId; double DocWordFq;
        BowDocBs->GetDocWIdFq(DId, DocWIdN, DocWId, DocWordFq);
        DocWgtBs->WordFqV[DocWId]++;
      }
    }
    // create inverse-document-frequency for words
    TFltV WordIDFV(Words);
    for (int WId=0; WId<Words; WId++){
      if (WId%100==0){
        TNotify::OnNotify(Notify, ntInfo, TStr("Computing IDF values (")+TInt::GetStr(WId+1)+"/"+TInt::GetStr(Words)+")...");}
      double WordDf=DocWgtBs->WordFqV[WId];
      if (WordDf>0){
        WordIDFV[WId]=log(double(Docs)/WordDf);}
    }
    // calculate word-weights for documents (DocSpVV)
    DocWgtBs->DocSpVV.Gen(AllDocs);
    {for (int DIdN=0; DIdN<Docs; DIdN++){
      TNotify::OnNotify(Notify, ntInfo, TStr("Computing weights (")+TInt::GetStr(DIdN+1)+"/"+TInt::GetStr(BaseDocs)+")...");
      // get document info
      int DId=DocWgtBs->GetDId(DIdN);
      int DocWIds=BowDocBs->GetDocWIds(DId);
      // create document sparse vector
      PBowSpV DocSpV=TBowSpV::New(DId, DocWIds);
      DocWgtBs->DocSpVV[DId]=DocSpV;
      // calculate & add words weights
      for (int DocWIdN=0; DocWIdN<DocWIds; DocWIdN++){
        int DocWId; double DocWordFq;
        BowDocBs->GetDocWIdFq(DId, DocWIdN, DocWId, DocWordFq);
        TStr WordStr=BowDocBs->GetWordStr(DocWId); // for debugging
        if (BowDocBs->GetWordFq(DocWId)>=_MnWordFq){
          // calculate TFIDF
          double DocWordWgt=DocWordFq*WordIDFV[DocWId];
          // multiply by Log DF
          if (DocWgtBs->WordWgtType==bwwtLogDFNrmTFIDF){
            double WordDf=DocWgtBs->WordFqV[DocWId];
            DocWordWgt=log(1+WordDf)*DocWordWgt;
          }
          // save weight
          DocSpV->AddWIdWgt(DocWId, DocWordWgt);
        }
      }
      // cut low weight words
      DocSpV->CutLowWgtWords(_CutWordWgtSumPrc);
      // put unit-norm
      DocSpV->PutUnitNorm();
    }}
  } else if (DocWgtBs->WordWgtType==bwwtNrmTFICF){
    // prepare base documents
    TIntV BaseDIdV;
    if (_BaseDIdV.Empty()){BaseDIdV=DocWgtBs->DIdV;}
    else {BaseDIdV=_BaseDIdV;}
    const int BaseDocs=BaseDIdV.Len();
    // create category-frequency for words
    const int Words=BowDocBs->GetWords();
    TVec<TIntH> WordCIdHV(Words, 0);
    for (int WId = 0; WId < Words; WId++) { WordCIdHV.Add(TIntH()); }
    // we check for each word in what categories it appears
    for (int DIdN=0; DIdN<BaseDocs; DIdN++){
      const int DId=BaseDIdV[DIdN];
      const int DocWIds=BowDocBs->GetDocWIds(DId);
      const int DocCIds = BowDocBs->GetDocCIds(DId);
      for (int DocWIdN=0; DocWIdN<DocWIds; DocWIdN++){
        int DocWId; double DocWordFq;
        BowDocBs->GetDocWIdFq(DId, DocWIdN, DocWId, DocWordFq);
        for (int DocCIdN = 0; DocCIdN < DocCIds; DocCIdN++) {
            const int DocCId = BowDocBs->GetDocCId(DId, DocCIdN);
            if (!WordCIdHV[DocWId].IsKey(DocCId)) {
                WordCIdHV[DocWId].AddKey(DocCId);
            }
        }
      }
    }
    // create inverse-category-frequency for words
    const int Cats=BowDocBs->GetCats();
    TFltV WordICFV(Words);
    for (int WId=0; WId<Words; WId++){
      double WordCf=WordCIdHV[WId].Len();
      if (WordCf>0){
        WordICFV[WId]=log(double(Cats)/WordCf);
      } else {
        WordICFV[WId]=0.0;
      }
    }
    // calculate word-weights for documents (DocSpVV)
    DocWgtBs->WordFqV.Gen(Words);
    DocWgtBs->DocSpVV.Gen(AllDocs);
    {for (int DIdN=0; DIdN<Docs; DIdN++){
      // get document info
      int DId=DocWgtBs->GetDId(DIdN);
      int DocWIds=BowDocBs->GetDocWIds(DId);
      // create document sparse vector
      PBowSpV DocSpV=TBowSpV::New(DId, DocWIds);
      DocWgtBs->DocSpVV[DId]=DocSpV;
      // calculate & add words weights
      for (int DocWIdN=0; DocWIdN<DocWIds; DocWIdN++){
        int DocWId; double DocWordFq;
        BowDocBs->GetDocWIdFq(DId, DocWIdN, DocWId, DocWordFq);
        DocWgtBs->WordFqV[DocWId]+=DocWordFq;
        TStr WordStr=BowDocBs->GetWordStr(DocWId); // for debugging
        if (BowDocBs->GetWordFq(DocWId)>=_MnWordFq){
          // calculate TFICF
          double DocWordWgt=DocWordFq*WordICFV[DocWId];
          // save weight
          DocSpV->AddWIdWgt(DocWId, DocWordWgt);
        }
      }
      // cut low weight words
      DocSpV->CutLowWgtWords(_CutWordWgtSumPrc);
      // put unit-norm
      DocSpV->PutUnitNorm();
    }}
  } else {
    Fail;
  }
  // return result
  return DocWgtBs;
}

PBowDocWgtBs TBowDocWgtBs::New(const TVec<PBowSpV>& BowSpVV) {
    // create document-weights-base
    PBowDocWgtBs DocWgtBs=TBowDocWgtBs::New(0);
    // build-parameters
    DocWgtBs->WordWgtType=bwwtPreCalc;
    DocWgtBs->CutWordWgtSumPrc=0.0;
    DocWgtBs->MnWordFq=0;
    // go over all documents and read the ids and number of words
    int AllWords=0; DocWgtBs->DIdV.Gen(BowSpVV.Len(), 0);
    for (int DocN = 0; DocN < BowSpVV.Len(); DocN++) {
        DocWgtBs->DIdV.Add(BowSpVV[DocN]->GetDId());
        AllWords = TInt::GetMx(AllWords, BowSpVV[DocN]->GetLastWId());
    }
    // load documents
    DocWgtBs->WordFqV.Gen(AllWords); 
    DocWgtBs->WordFqV.PutAll(0.0);
    DocWgtBs->DocSpVV = BowSpVV;
    // return result
    return DocWgtBs;
}

PBowDocWgtBs TBowDocWgtBs::NewPreCalcWgt(const PBowDocBs& BowDocBs,
 const TFltV& WordWgtV, const bool& PutUniteNorm,
 const double& _CutWordWgtSumPrc, const int& _MnWordFq,
 const TIntV& _DIdV) {
    // create document-weights-base
    PBowDocWgtBs DocWgtBs=TBowDocWgtBs::New(BowDocBs->GetSig());
    // build-parameters
    DocWgtBs->WordWgtType=bwwtPreCalc;
    DocWgtBs->CutWordWgtSumPrc=_CutWordWgtSumPrc;
    DocWgtBs->MnWordFq=_MnWordFq;
    // create document-id vector (DIdV)
    if (_DIdV.Empty()){BowDocBs->GetAllDIdV(DocWgtBs->DIdV);}
    else {DocWgtBs->DIdV=_DIdV;}
    // shortcuts
    int Docs=DocWgtBs->GetDocs();
    int AllDocs=BowDocBs->GetDocs();
    int AllWords=BowDocBs->GetWords();
    EAssert(AllWords == WordWgtV.Len());
    // load documents
    DocWgtBs->WordFqV.Gen(AllWords);
    DocWgtBs->DocSpVV.Gen(AllDocs);
    for (int DIdN=0; DIdN<Docs; DIdN++) {
        // get document info
        int DId=DocWgtBs->GetDId(DIdN);
        int DocWIds=BowDocBs->GetDocWIds(DId);
        // create document sparse vector
        PBowSpV DocSpV=TBowSpV::New(DId, DocWIds);
        DocWgtBs->DocSpVV[DId]=DocSpV;
        // calculate & add words weights
        for (int DocWIdN=0; DocWIdN<DocWIds; DocWIdN++){
            int DocWId; double DocWordFq;
            BowDocBs->GetDocWIdFq(DId, DocWIdN, DocWId, DocWordFq);
            TStr WordStr=BowDocBs->GetWordStr(DocWId); // for debugging
            // calculate and save word weights
            if (BowDocBs->GetWordFq(DocWId)>=_MnWordFq){
                DocSpV->AddWIdWgt(DocWId, DocWordFq*WordWgtV[DocWId]);
            }
            DocWgtBs->WordFqV[DocWId] += DocWordFq;
        }
        // cut low weight words
        DocSpV->CutLowWgtWords(_CutWordWgtSumPrc);
        // put unit-norm
        if (PutUniteNorm) { DocSpV->PutUnitNorm(); }
    }
    // return result
    return DocWgtBs;
}

PBowDocWgtBs TBowDocWgtBs::NewSvmWgt(
   const PBowDocBs& BowDocBs,
   const PBowDocWgtBs& BowDocWgtBs,
   const TIntV& _TrainDIdV,
   const double& SvmCostParam,
   const int& MxTimePerCat,
   const bool& NegFeaturesP,
   const TIntV& _DIdV,
   const bool& PutUniteNormP,
   const double& _CutWordWgtSumPrc,
   const int& _MnWordFq) {

    // create document-weights-base
    PBowDocWgtBs DocWgtBs=TBowDocWgtBs::New(BowDocBs->GetSig());
    // build-parameters
    DocWgtBs->WordWgtType=bwwtSvm;
    DocWgtBs->CutWordWgtSumPrc=_CutWordWgtSumPrc;
    DocWgtBs->MnWordFq=_MnWordFq;
    // create document-id vector (DIdV)
    if (_DIdV.Empty()){BowDocBs->GetAllDIdV(DocWgtBs->DIdV);}
    else {DocWgtBs->DIdV=_DIdV;}
    // shortcuts
    const int Docs=DocWgtBs->DIdV.Len();
    const int AllDocs=BowDocBs->GetDocs();
    const int AllWords=BowDocBs->GetWords();
    EAssert(BowDocBs->IsCats());
    const int Cats = BowDocBs->GetCats();

    // prepair train documen ids
    TIntV TrainDIdV;
    if (_TrainDIdV.Empty()){
        TrainDIdV.Gen(BowDocWgtBs->GetDocs(), 0);
        for (int DIdN = 0; DIdN < BowDocWgtBs->GetDocs(); DIdN++) {
            TrainDIdV.Add(BowDocWgtBs->GetDId(DIdN));
        }
    } else { TrainDIdV = TrainDIdV; }
    // learn svm models and calculate word weights for each category
    TVec<TFltV> CatWgtVV(Cats);
    for (int CId = 0; CId < Cats; CId++) {
        printf("Cat %d/%d\r", CId+1, Cats);
        // prepare vectors
        PSVMTrainSet CatTrainSet = TBowDocBs2TrainSet::NewBowAllCat(
            BowDocBs, BowDocWgtBs, CId, TrainDIdV);
        TIntFltKdV PPCatWIdWgtV, NNCatWIdWgtV;
        if (CatTrainSet->HasPosNegVecs(5)) {
            // learn SVM model for current category
            const double SvmUnbalanceParam = 5.0;
            PSVMModel CatSvmModel = TSVMModel::NewClsLinear(
                CatTrainSet, SvmCostParam, SvmUnbalanceParam,
                TIntV(), TSVMLearnParam::Lin(MxTimePerCat));
            // get SVM model's hyperplane normal vector
            TFltV NormalV; CatSvmModel->GetWgtV(NormalV);
            // calculate weights for words from normal vector
            CatTrainSet->GetKeywords(NormalV, PPCatWIdWgtV, TIntV(), -1, 1.0, 1.0, true);
            CatTrainSet->GetKeywords(NormalV, NNCatWIdWgtV, TIntV(), -1, -1.0, -1.0, true);
            TLinAlg::NormalizeLinf(PPCatWIdWgtV); TLinAlg::NormalizeLinf(NNCatWIdWgtV);
        }
        // save calculated word weights
        TFltV& CatWgtV = CatWgtVV[CId];
        CatWgtV.Gen(AllWords); CatWgtV.PutAll(0.0);

        for (int WdN = 0; WdN < PPCatWIdWgtV.Len(); WdN++) {
            const int CatWId = PPCatWIdWgtV[WdN].Key;
            const double CatWordWgt = pow(2*PPCatWIdWgtV[WdN].Dat+1.0, 1.0/4.0);
            CatWgtV[CatWId] += CatWordWgt;
        }
        if (NegFeaturesP) {
            for (int WdN = 0; WdN < NNCatWIdWgtV.Len(); WdN++) {
                const int CatWId = NNCatWIdWgtV[WdN].Key;
                const double CatWordWgt = pow(2*NNCatWIdWgtV[WdN].Dat+1.0, 1.0/4.0);
                CatWgtV[CatWId] += CatWordWgt;
            }
        }
        TLinAlg::Normalize(CatWgtV);
    }
    printf("\n");

    // load documents
    DocWgtBs->WordFqV.Gen(AllWords);
    DocWgtBs->DocSpVV.Gen(AllDocs);
    for (int DIdN=0; DIdN<Docs; DIdN++) {
        printf("Doc %d/%d\r", DIdN+1, Docs);
        // get document info
        const int DId=DocWgtBs->GetDId(DIdN);
        const int DocWIds=BowDocBs->GetDocWIds(DId);
        // create document sparse vector
        PBowSpV DocSpV=TBowSpV::New(DId, DocWIds);
        DocWgtBs->DocSpVV[DId]=DocSpV;
        // calculate & add word weights
        const int DocCIds = BowDocBs->GetDocCIds(DId);
        for (int DocWIdN=0; DocWIdN<DocWIds; DocWIdN++){
            int DocWId; double DocWordFq;
            BowDocBs->GetDocWIdFq(DId, DocWIdN, DocWId, DocWordFq);
            TStr WordStr=BowDocBs->GetWordStr(DocWId); // for debugging
            // calculate and save word weights
            if (BowDocBs->GetWordFq(DocWId)>=_MnWordFq){
                // sum weights from all document categories
                double WordWgt = 0.0;
                for (int DocCatN = 0; DocCatN < DocCIds; DocCatN++) {
                    const int DocCatId = BowDocBs->GetDocCId(DId, DocCatN);
                    WordWgt += CatWgtVV[DocCatId][DocWId];
                }
                // add weighted element
                DocSpV->AddWIdWgt(DocWId, DocWordFq*WordWgt);
            }
            DocWgtBs->WordFqV[DocWId] += DocWordFq;
        }
        // cut low weight words
        DocSpV->CutLowWgtWords(_CutWordWgtSumPrc);
        // put unit-norm
        if (PutUniteNormP) { DocSpV->PutUnitNorm(); }
    }
    printf("\n");
    // return result
    return DocWgtBs;
}

PBowDocWgtBs TBowDocWgtBs::NewBinSvmWgt(
   const PBowDocBs& BowDocBs,
   const PBowDocWgtBs& BowDocWgtBs,
   const TStr& CatNm,
   const TIntV& TrainDIdV,
   const double& SvmCostParam,
   const double& SvmUnbalanceParam,
   const double& MnWgt,
   const bool& NegFeaturesP,
   const bool& PutUniteNormP,
   const bool& AvgNormalP,
   const TIntV& _DIdV,
   const double& _CutWordWgtSumPrc,
   const int& _MnWordFq) {

    // create document-weights-base
    PBowDocWgtBs DocWgtBs=TBowDocWgtBs::New(BowDocBs->GetSig());
    // build-parameters
    DocWgtBs->WordWgtType=bwwtSvm;
    DocWgtBs->CutWordWgtSumPrc=_CutWordWgtSumPrc;
    DocWgtBs->MnWordFq=_MnWordFq;
    // create document-id vector (DIdV)
    if (_DIdV.Empty()){BowDocBs->GetAllDIdV(DocWgtBs->DIdV);}
    else {DocWgtBs->DIdV=_DIdV;}
    // shortcuts
    const int Docs=DocWgtBs->DIdV.Len();
    const int AllDocs=BowDocBs->GetDocs();
    const int AllWords=BowDocBs->GetWords();
    EAssert(BowDocBs->IsCatNm(CatNm));
    const int CId = BowDocBs->GetCId(CatNm);

    // prepare vectors
    PSVMTrainSet CatTrainSet = TBowDocBs2TrainSet::NewBowAllCat(
        BowDocBs, BowDocWgtBs, CId, TrainDIdV);
    TIntFltKdV PPCatWIdWgtV, NNCatWIdWgtV;
    EAssert(CatTrainSet->HasPosNegVecs(5));
    // learn SVM model for current category
    PSVMModel CatSvmModel = TSVMModel::NewClsLinear(
        CatTrainSet, SvmCostParam, SvmUnbalanceParam);
    // get SVM model's hyperplane normal vector
    TFltV NormalV; CatSvmModel->GetWgtV(NormalV);
    // calculate weights for words from normal vector
    CatTrainSet->GetKeywords(NormalV, PPCatWIdWgtV, TIntV(), -1, 1.0, 1.0, AvgNormalP);
    CatTrainSet->GetKeywords(NormalV, NNCatWIdWgtV, TIntV(), -1, -1.0, -1.0, AvgNormalP);
    TLinAlg::NormalizeLinf(PPCatWIdWgtV); TLinAlg::NormalizeLinf(NNCatWIdWgtV);
    // save calculated word weights
    TFltV CatWgtV; CatWgtV.Gen(AllWords); CatWgtV.PutAll(MnWgt);
    for (int WdN = 0; WdN < PPCatWIdWgtV.Len(); WdN++) {
        const int CatWId = PPCatWIdWgtV[WdN].Key;
        const double CatWordWgt = pow(2*PPCatWIdWgtV[WdN].Dat+1.0, 1.0/4.0);
        CatWgtV[CatWId] += CatWordWgt;
    }
    if (NegFeaturesP) {
        for (int WdN = 0; WdN < NNCatWIdWgtV.Len(); WdN++) {
            const int CatWId = NNCatWIdWgtV[WdN].Key;
            const double CatWordWgt = pow(2*NNCatWIdWgtV[WdN].Dat+1.0, 1.0/4.0);
            CatWgtV[CatWId] += CatWordWgt;
        }
    }

    // load documents
    DocWgtBs->WordFqV.Gen(AllWords);
    DocWgtBs->DocSpVV.Gen(AllDocs);
    for (int DIdN=0; DIdN<Docs; DIdN++) {
        // get document info
        const int DId=DocWgtBs->GetDId(DIdN);
        const int DocWIds=BowDocBs->GetDocWIds(DId);
        // create document sparse vector
        PBowSpV DocSpV=TBowSpV::New(DId, DocWIds);
        DocWgtBs->DocSpVV[DId]=DocSpV;
        // calculate & add word weights
        for (int DocWIdN=0; DocWIdN<DocWIds; DocWIdN++){
            int DocWId; double DocWordFq;
            BowDocBs->GetDocWIdFq(DId, DocWIdN, DocWId, DocWordFq);
            TStr WordStr=BowDocBs->GetWordStr(DocWId); // for debugging
            const double WordWgt = CatWgtV[DocWId];
            DocSpV->AddWIdWgt(DocWId, DocWordFq*WordWgt);
            DocWgtBs->WordFqV[DocWId] += DocWordFq;
        }
        // cut low weight words
        DocSpV->CutLowWgtWords(_CutWordWgtSumPrc);
        // put unit-norm
        if (PutUniteNormP) { DocSpV->PutUnitNorm(); }
    }
    // return result
    return DocWgtBs;
}

TBowWordWgtType TBowDocWgtBs::GetWordWgtTypeFromStr(const TStr& Nm){
  TStr UcNm=Nm.GetUc();
  if (UcNm=="UNDEF"){return bwwtUndef;}
  else if (UcNm=="EQ"){return bwwtEq;}
  else if (UcNm=="NRMEQ"){return bwwtNrmEq;}
  else if (UcNm=="BIN"){return bwwtNrmBin;}
  else if (UcNm=="NRM01"){return bwwtNrm01;}
  else if (UcNm=="NRMTFIDF"){return bwwtNrmTFIDF;}
  else if (UcNm=="LOGDFNRMTFIDF"){return bwwtLogDFNrmTFIDF;}
  else if (UcNm=="NRMTFICF"){return bwwtNrmTFICF;}
  else if (UcNm=="PRECALC"){return bwwtPreCalc;}
  else if (UcNm=="SVM"){return bwwtSvm;}
  else {return bwwtUndef;}
}

void TBowDocWgtBs::GetSimDIdV(
 const PBowSpV& RefBowSpV, const PBowSim& BowSim,
 TFltIntKdV& SimDIdKdV, const bool& RefBowSpVInclude) const {
  int Docs=GetDocs();
  SimDIdKdV.Gen(Docs, 0);
  for (int DIdN=0; DIdN<Docs; DIdN++){
    int DId=GetDId(DIdN);
    PBowSpV DocBowSpV=GetSpV(DId);
    //B: dodan (RefBowSpVInclude)|| na zacetek if-a
    if ((RefBowSpVInclude)||(!RefBowSpV->IsDId())||(RefBowSpV->GetDId()!=DocBowSpV->GetDId())){
      double Sim=BowSim->GetSim(RefBowSpV, DocBowSpV);
      SimDIdKdV.Add(TFltIntKd(Sim, DId));
    }
  }
  SimDIdKdV.Sort(false);
}

void TBowDocWgtBs::SaveTxtSimDIdV(
 const PSOut& SOut, const PBowDocBs& BowDocBs,
 const PBowSpV& RefBowSpV, const TFltIntKdV& SimDIdKdV,
 const int& TopHits, const double& MnSim, const int& TopDocWords,
 const char& SepCh) const {
  // output reference document
  SOut->PutStr("Query Document:\n");
  if (RefBowSpV->IsDId()){
    int RefDId=RefBowSpV->GetDId();
    TStr RefDocNm=BowDocBs->GetDocNm(RefDId);
    SOut->PutStr(RefDocNm, "   Document '%s'");
    SOut->PutLn();
  }
  RefBowSpV->SaveTxt(SOut, BowDocBs, TopDocWords, SepCh);
  SOut->PutLn();
  // output result-set
  int OutDocs=TopHits;
  if ((OutDocs==-1)||(OutDocs>=SimDIdKdV.Len())){OutDocs=SimDIdKdV.Len();}
  for (int OutDocN=0; OutDocN<OutDocs; OutDocN++){
    // get document data
    double Sim=SimDIdKdV[OutDocN].Key;
    int DId=SimDIdKdV[OutDocN].Dat;
    TStr DocNm=BowDocBs->GetDocNm(DId);
    PBowSpV DocSpV=GetSpV(DId);
    if (Sim<MnSim){break;}
    // output
    SOut->PutInt(OutDocN+1, "%d.");
    SOut->PutStr(DocNm, "   Document '%s'");
    SOut->PutFlt(Sim, "   Similarity: %g");
    SOut->PutLn();
    DocSpV->SaveTxt(SOut, BowDocBs, TopDocWords, SepCh);
    SOut->PutLn();
  }
}

void TBowDocWgtBs::SaveXmlSimDIdV(
 const PSOut& SOut, const PBowDocBs& BowDocBs,
 const PBowSpV& RefBowSpV, const TFltIntKdV& SimDIdKdV,
 const int& TopHits, const double& MnSim) const {
  SOut->PutStr("<SimilaritySearchResults>");
  // output reference document
  SOut->PutStr("<QueryDocument");
  if (RefBowSpV->IsDId()){
    int RefDId=RefBowSpV->GetDId();
    TStr RefDocNm=BowDocBs->GetDocNm(RefDId);
    SOut->PutStr(TInt::GetStr(RefDId, "  DocId=\"%d\""));
    SOut->PutStr(TStr::GetStr(RefDocNm, "  DocNm=\"%s\""));
  }
  SOut->PutStr(">"); SOut->PutLn();
  RefBowSpV->SaveXml(SOut, BowDocBs);
  SOut->PutStr("</QueryDocument>"); SOut->PutLn();
  // output result-set
  int OutDocs=TopHits;
  if ((OutDocs==-1)||(OutDocs>=SimDIdKdV.Len())){OutDocs=SimDIdKdV.Len();}
  for (int OutDocN=0; OutDocN<OutDocs; OutDocN++){
    // get document data
    double Sim=SimDIdKdV[OutDocN].Key;
    int DId=SimDIdKdV[OutDocN].Dat;
    TStr DocNm=BowDocBs->GetDocNm(DId);
    PBowSpV DocSpV=GetSpV(DId);
    if (Sim<MnSim){break;}
    // output
    SOut->PutStr("<Hit");
    SOut->PutStr(TInt::GetStr(OutDocN+1, " Num=\"%d\""));
    SOut->PutStr(TInt::GetStr(DId, "  DocId=\"%d\""));
    SOut->PutStr(TStr::GetStr(DocNm, "  DocNm=\"%s\""));
    SOut->PutStr(TFlt::GetStr(Sim, "  Sim=\"%g\""));
    SOut->PutStr(">"); SOut->PutLn();
    DocSpV->SaveXml(SOut, BowDocBs);
    SOut->PutStr("</Hit>"); SOut->PutLn();
  }
  SOut->PutStr("</SimilaritySearchResults>");
}

void TBowDocWgtBs::SaveTxtStat(
 const TStr& StatFNm, const PBowDocBs& BowDocBs,
 const bool& SaveWordsP, const bool& SaveCatsP, const bool& SaveDocsP) const {
  // create output file
  TFOut StatSOut(StatFNm); FILE* fStat=StatSOut.GetFileId();
  // save word-statistics
  if (SaveWordsP){
    int DIds=GetDocs();
    int WIds=GetWords();
    // collect word-weights
    TIntIntFltPrH WIdToWFqWWgtPrH(WIds);
    for (int DIdN=0; DIdN<DIds; DIdN++){
      int DId=GetDId(DIdN);
      PBowSpV DocSpV=GetSpV(DId);
      int DocWIds=DocSpV->GetWIds();
      for (int DocWIdN=0; DocWIdN<DocWIds; DocWIdN++){
        int DocWId; double DocWgt;
        DocSpV->GetWIdWgt(DocWIdN, DocWId, DocWgt);
        TIntFltPr& WFqWWgtPr=WIdToWFqWWgtPrH.AddDat(DocWId);
        WFqWWgtPr.Val1++; WFqWWgtPr.Val2+=DocWgt;
      }
    }
    // calculate & sort average word-weights
    TFltIntPrV AvgWWgtWIdPrV(WIdToWFqWWgtPrH.Len(), 0);
    for (int WIdN=0; WIdN<WIdToWFqWWgtPrH.Len(); WIdN++){
      int WId=WIdToWFqWWgtPrH.GetKey(WIdN);
      TIntFltPr& WFqWWgtPr=WIdToWFqWWgtPrH.AddDat(WId);
      double AvgWWgt=0;
      if (WFqWWgtPr.Val1>0){AvgWWgt=WFqWWgtPr.Val2/WFqWWgtPr.Val1;}
      AvgWWgtWIdPrV.Add(TFltIntPr(AvgWWgt, WId));
    }
    AvgWWgtWIdPrV.Sort(false);
    // output words & word-weights & document-frequencies
    fprintf(fStat, "\nRank Word-Weight Document-Frequency Word-String\n\n");
    {for (int WIdN=0; WIdN<AvgWWgtWIdPrV.Len(); WIdN++){
      double AvgWWgt=AvgWWgtWIdPrV[WIdN].Val1;
      int WId=AvgWWgtWIdPrV[WIdN].Val2;
      TStr WordStr=BowDocBs->GetWordStr(AvgWWgtWIdPrV[WIdN].Val2);
      double WordDFq=GetWordFq(WId);
      fprintf(fStat, "%d.\t%.3f\t%g\t'%s'\n",
       1+WIdN, AvgWWgt, WordDFq, WordStr.CStr());
    }}
  }
  // save category-statistics
  if (SaveCatsP){}
  // save document-statistics
  if (SaveDocsP){
    fprintf(fStat, "\n\nDocument-Statistics\n\n");
    int Docs=GetDocs();
    for (int DIdN=0; DIdN<Docs; DIdN++){
      int DId=GetDId(DIdN);
      TStr DocNm=BowDocBs->GetDocNm(DId);
      PBowSpV DocSpV=GetSpV(DId);
      int DocWIds=DocSpV->GetWIds();
      fprintf(fStat, "DId:%d   Name:'%s' (%d Words):", DId, DocNm.CStr(), DocWIds);
      for (int DocWIdN=0; DocWIdN<DocWIds; DocWIdN++){
        int DocWId; double DocWgt;
        DocSpV->GetWIdWgt(DocWIdN, DocWId, DocWgt);
        TStr WordStr=BowDocBs->GetWordStr(DocWId);
        fprintf(fStat, " '%s':%.3f", WordStr.CStr(), DocWgt);
      }
      fprintf(fStat, "\n");
    }
  }
}

const TStr TBowDocWgtBs::BowDocWgtBsFExt=".Boww";

/////////////////////////////////////////////////
// BagOfWords-Document-Base
PBowDocBs TBowDocBs::New(
 const PSwSet& SwSet, const PStemmer& Stemmer, const PNGramBs& NGramBs){
  PBowDocBs BowDocBs=New();
  if (NGramBs.Empty()){
    BowDocBs->PutSwSet(SwSet); BowDocBs->PutStemmer(Stemmer);
  } else {
    BowDocBs->PutNGramBs(NGramBs);
  }
  return BowDocBs;
}

void TBowDocBs::AddDocs(const PBowDocBs& BowDocBs)
{
  // add words
  for (int WordN = 0; WordN < BowDocBs->GetWords(); WordN++)
  {
    TStr WordStr = BowDocBs->GetWordStr(WordN);
    int WordId = -1;
    if (!this->IsWordStr(WordStr, WordId))
    {
      this->AddWordStr(WordStr);
    }
  }
  // add documents
  for (int DocId = 0; DocId < BowDocBs->GetDocs(); DocId++)
  {
    TIntFltPrV NewDoc;
    for (int WIdN = 0; WIdN < BowDocBs->GetDocWIds(DocId); WIdN++)
    {
      int WId = BowDocBs->GetDocWId(DocId, WIdN);
      double WFq = BowDocBs->GetDocWFq(DocId, WIdN);
      TStr WordStr = BowDocBs->GetWordStr(WId);
      int WIdCmn = this->GetWId(WordStr);
      NewDoc.Add(TIntFltPr(WIdCmn, WFq));
    }
    this->AddDoc("", TStrV(), NewDoc);
  }
  // TODO: categories et al.
}

void TBowDocBs::AssertOk() const {
  // check documents
  int Docs=GetDocs();
  for (int DId=0; DId<Docs; DId++){
    PBowSpV DocSpV=GetDocSpV(DId);
    IAssert(DId==DocSpV->GetDId());
    TStr DocNm=GetDocNm(DId);
    // check document words
    for (int WIdN=0; WIdN<DocSpV->GetWIds(); WIdN++){
      int WId; double Wgt; DocSpV->GetWIdWgt(WIdN, WId, Wgt);
      TStr WordStr=GetWordStr(WId);
    }
    // check document categories
    int CIds=GetDocCIds(DId);
    for (int CIdN=0; CIdN<CIds; CIdN++){
      int CId=GetDocCId(DId, CIdN);
      TStr CatNm=GetCatNm(CId);
    }
  }
  // check training documents
  int TrainDocs=GetTrainDocs();
  for (int TrainDIdN=0; TrainDIdN<TrainDocs; TrainDIdN++){
    int TrainDId=GetTrainDId(TrainDIdN);
    PBowSpV TrainDocSpV=GetDocSpV(TrainDId);
  }
  // check testing documents
  int TestDocs=GetTestDocs();
  for (int TestDIdN=0; TestDIdN<TestDocs; TestDIdN++){
    int TestDId=GetTestDId(TestDIdN);
    PBowSpV TestDocSpV=GetDocSpV(TestDId);
  }
}

void TBowDocBs::GetWordStrVFromHtml(const TStr& HtmlStr, TStrV& WordStrV) const {
  WordStrV.Clr();
  if (!NGramBs.Empty()){
    // create ngram-list
    NGramBs->GetNGramStrV(HtmlStr, WordStrV);
  } else {
    // create html lexical
    PSIn HtmlSIn=TStrIn::New(HtmlStr);
    THtmlLx HtmlLx(HtmlSIn);
    // traverse html string symbols
    while (HtmlLx.Sym!=hsyEof){
      if (HtmlLx.Sym==hsyStr){
        TStr WordStr=HtmlLx.UcChA;
        if ((SwSet.Empty())||(!SwSet->IsIn(WordStr))){
          if (!Stemmer.Empty()){
            WordStr=Stemmer->GetStem(WordStr);}
          WordStrV.Add(WordStr);
        }
      }
      // get next symbol
      HtmlLx.GetSym();
    }
  }
}

int TBowDocBs::AddDoc(const TStr& _DocNm,
 const TStrV& CatNmV, const TIntFltPrV& WIdWgtPrV){
  // create doc-id
  TStr DocNm=_DocNm;
  if (DocNm.Empty()){DocNm=TInt::GetStr(GetDocs());}
  int DId=-1;
  if (!DocNmToDescStrH.IsKey(DocNm, DId)){
    DId=DocNmToDescStrH.AddKey(DocNm);
    DocSpVV.Add(TBowSpV::New(DId)); IAssert(DId==DocSpVV.Len()-1);
    DocStrV.Add();
    DocCIdVV.Add(); IAssert(DId==DocCIdVV.Len()-1);
  }

  // convert category names to cat-ids
  TIntV& DocCIdV=DocCIdVV[DId];
  DocCIdV.Gen(CatNmV.Len(), 0);
  for (int CatNmN=0; CatNmN<CatNmV.Len(); CatNmN++){
    int CId=CatNmToFqH.AddKey(CatNmV[CatNmN]);
    CatNmToFqH[CId]++; DocCIdV.Add(CId);
  }
  DocCIdV.Sort();

  // convert word strings to word-ids
  // collect words
  TIntFltH DocWIdToWgtH;
  for (int WordN=0; WordN<WIdWgtPrV.Len(); WordN++){
    int WId=WIdWgtPrV[WordN].Val1;
    IAssert(IsWId(WId));
    DocWIdToWgtH.AddDat(WId, WIdWgtPrV[WordN].Val2);
  }
  // compose document bag-of-words vector
  PBowSpV DocSpV=DocSpVV[DId];
  DocSpV->GenMx(DocWIdToWgtH.Len());
  for (int DocWordN=0; DocWordN<DocWIdToWgtH.Len(); DocWordN++){
    int WId=DocWIdToWgtH.GetKey(DocWordN);
    double Wgt=DocWIdToWgtH[DocWordN];
    WordStrToDescH[WId].Fq++;
    DocSpV->AddWIdWgt(WId, Wgt);
  }
  DocSpV->Sort();

  // return doc-id
  return DId;
}

int TBowDocBs::AddDoc(const TStr& _DocNm,
 const TStrV& CatNmV, const TStrV& WordStrV, const TStr& DocStr){
  // create doc-id
  TStr DocNm=_DocNm;
  if (DocNm.Empty()){DocNm=TInt::GetStr(GetDocs());}
  int DId=-1;
  if (!DocNmToDescStrH.IsKey(DocNm, DId)){
    DId=DocNmToDescStrH.AddKey(DocNm);
    DocSpVV.Add(TBowSpV::New(DId)); IAssert(DId==DocSpVV.Len()-1);
    DocStrV.Add(DocStr);
    DocCIdVV.Add(); IAssert(DId==DocCIdVV.Len()-1);
  }

  // convert category names to cat-ids
  TIntV& DocCIdV=DocCIdVV[DId];
  DocCIdV.Gen(CatNmV.Len(), 0);
  for (int CatNmN=0; CatNmN<CatNmV.Len(); CatNmN++){
    int CId=CatNmToFqH.AddKey(CatNmV[CatNmN]);
    CatNmToFqH[CId]++; DocCIdV.Add(CId);
  }
  DocCIdV.Sort();

  // convert word strings to word-ids
  // collect words
  TStrIntH DocWordStrToFqH;
  for (int WordStrN=0; WordStrN<WordStrV.Len(); WordStrN++){
    DocWordStrToFqH.AddDat(WordStrV[WordStrN])++;}
  // compose document bag-of-words vector
  PBowSpV DocSpV=DocSpVV[DId];
  DocSpV->GenMx(DocWordStrToFqH.Len());
  for (int DocWordStrN=0; DocWordStrN<DocWordStrToFqH.Len(); DocWordStrN++){
    TStr WordStr=DocWordStrToFqH.GetKey(DocWordStrN);
    int Fq=DocWordStrToFqH[DocWordStrN];
    int WId=WordStrToDescH.AddKey(WordStr);
    WordStrToDescH[WId].Fq++;
    DocSpV->AddWIdWgt(WId, Fq);
  }
  DocSpV->Sort();

  // return doc-id
  return DId;
}

int TBowDocBs::AddHtmlDoc(const TStr& DocNm, const TStrV& CatNmV,
 const TStr& HtmlDocStr, const bool& SaveDocP){
  // get word-list
  TStrV WordStrV; GetWordStrVFromHtml(HtmlDocStr, WordStrV);
  // add document as word-list
  int DocId;
  if (SaveDocP){DocId=AddDoc(DocNm, CatNmV, WordStrV, HtmlDocStr);}
  else {DocId=AddDoc(DocNm, CatNmV, WordStrV, "");}
  // return doc-id
  return DocId;
}

void TBowDocBs::GetAllDIdV(TIntV& DIdV) const {
  int Docs=GetDocs();
  DIdV.Gen(Docs);
  for (int DId=0; DId<Docs; DId++){
    DIdV[DId]=DId;}
}

bool TBowDocBs::IsDocWordStr(const int& DId, const TStr& WordStr) const {
  int WId;
  if (IsWordStr(WordStr, WId)){
    return DocSpVV[DId]->IsWId(WId);
  } else {
    return false;
  }
}

void TBowDocBs::GetTopCatV(const int& TopCats, TIntStrPrV& FqCatNmPrV) const {
  CatNmToFqH.GetDatKeyPrV(FqCatNmPrV);
  FqCatNmPrV.Sort(false);
  FqCatNmPrV.Trunc(TopCats);
}

void TBowDocBs::SetHOTrainTestDIdV(
 const double& TestDocsPrc, TRnd& Rnd){
  TIntV AllDIdV; GetAllDIdV(AllDIdV);
  AllDIdV.Shuffle(Rnd);
  int TestDocs=int(AllDIdV.Len()*TestDocsPrc);
  int MxTrainDIdN=AllDIdV.Len()-1-TestDocs;
  // get train doc-ids
  AllDIdV.GetSubValV(0, MxTrainDIdN, TrainDIdV);
  // get test doc-ids
  AllDIdV.GetSubValV(MxTrainDIdN+1, AllDIdV.Len()-1, TestDIdV);
}

void TBowDocBs::SetCVTrainTestDIdV(
 const int& Folds, const int& FoldN, TRnd& Rnd){
  TIntV AllDIdV; GetAllDIdV(AllDIdV);
  if (Folds==1){
    TrainDIdV=AllDIdV;
    TestDIdV=AllDIdV;
  } else {
    AllDIdV.Shuffle(Rnd);
    int MnTestDIdN=(FoldN*AllDIdV.Len())/Folds;
    int MxTestDIdN=(((FoldN+1)*AllDIdV.Len())/Folds)-1;
    // get train doc-ids
    AllDIdV.GetSubValV(0, MnTestDIdN-1, TrainDIdV);
    TIntV UpTrainDIdV;
    AllDIdV.GetSubValV(MxTestDIdN+1, AllDIdV.Len()-1, UpTrainDIdV);
    TrainDIdV.AddV(UpTrainDIdV);
    // get test doc-ids
    AllDIdV.GetSubValV(MnTestDIdN, MxTestDIdN, TestDIdV);
  }
}

PBowDocBs TBowDocBs::GetLimWordRelFqDocBs(
 const double& MnWordFqPrc, const double& MxWordFqPrc) const {
  // create document set
  PBowDocBs BowDocBs=TBowDocBs::New();
  // copy unchanged components
  BowDocBs->DocNmToDescStrH=DocNmToDescStrH;
  BowDocBs->CatNmToFqH=CatNmToFqH;
  BowDocBs->DocCIdVV=DocCIdVV;
  BowDocBs->TrainDIdV=TrainDIdV;
  BowDocBs->TestDIdV=TestDIdV;
  // create shortcuts
  int Docs=GetDocs();
  int Words=GetWords();
  // cut low & high frequency words
  for (int WId=0; WId<Words; WId++){
    TStr WordStr=GetWordStr(WId);
    int WordFq=GetWordFq(WId);
    double WordFqPrc=double(WordFq)/double(Docs);
    if ((MnWordFqPrc<WordFqPrc)&&(WordFqPrc<MxWordFqPrc)){
      BowDocBs->WordStrToDescH.AddDat(WordStr).Fq=WordFq;
    }
  }
  // generate new documents
  for (int DId=0; DId<Docs; DId++){
    PBowSpV DocSpV=DocSpVV[DId]; int WIds=DocSpV->GetWIds();
    BowDocBs->DocSpVV.Add(TBowSpV::New(DId));
    for (int WIdN=0; WIdN<WIds; WIdN++){
      int WId; double WordFq; DocSpV->GetWIdWgt(WIdN, WId, WordFq);
      TStr WordStr=GetWordStr(WId);
      int NewWId;
      if (BowDocBs->IsWordStr(WordStr, NewWId)){
        BowDocBs->DocSpVV.Last()->AddWIdWgt(NewWId, WordFq);
      }
    }
    BowDocBs->DocSpVV.Last()->Trunc();
  }
  // return results
  return BowDocBs;
}

PBowDocBs TBowDocBs::GetLimWordAbsFqDocBs(const int& MnWordFq) const {
  // create document set
  PBowDocBs BowDocBs=TBowDocBs::New();
  // copy unchanged components
  BowDocBs->DocNmToDescStrH=DocNmToDescStrH;
  BowDocBs->CatNmToFqH=CatNmToFqH;
  BowDocBs->DocCIdVV=DocCIdVV;
  BowDocBs->TrainDIdV=TrainDIdV;
  BowDocBs->TestDIdV=TestDIdV;
  // create shortcuts
  int Docs=GetDocs();
  int Words=GetWords();
  // cut low & high frequency words
  for (int WId=0; WId<Words; WId++){
    TStr WordStr=GetWordStr(WId);
    int WordFq=GetWordFq(WId);
    if (MnWordFq<=WordFq){
      BowDocBs->WordStrToDescH.AddDat(WordStr).Fq=WordFq;
    }
  }
  // generate new documents
  for (int DId=0; DId<Docs; DId++){
    PBowSpV DocSpV=DocSpVV[DId]; int WIds=DocSpV->GetWIds();
    BowDocBs->DocSpVV.Add(TBowSpV::New(DId));
    for (int WIdN=0; WIdN<WIds; WIdN++){
      int WId; double WordFq; DocSpV->GetWIdWgt(WIdN, WId, WordFq);
      TStr WordStr=GetWordStr(WId);
      int NewWId;
      if (BowDocBs->IsWordStr(WordStr, NewWId)){
        BowDocBs->DocSpVV.Last()->AddWIdWgt(NewWId, WordFq);
      }
    }
    BowDocBs->DocSpVV.Last()->Trunc();
  }
  // return results
  return BowDocBs;
}

PBowDocBs TBowDocBs::GetSubDocSet(const TIntV& DIdV) const {
  // create document set
  PBowDocBs BowDocBs=TBowDocBs::New();
  BowDocBs->DocSpVV.Gen(DIdV.Len(), 0);
  BowDocBs->DocCIdVV.Gen(DIdV.Len(), 0);
  // create shortcuts
  int Docs=DIdV.Len();
  // generate new documents
  for (int DIdN=0; DIdN<Docs; DIdN++){
    int DId=DIdV[DIdN];
    TStr DocNm=DocNmToDescStrH.GetKey(DId);
    PBowSpV SpV=DocSpVV[DId]; int WIds=SpV->GetWIds();
    const TIntV& DocCIdV=DocCIdVV[DId];
    // words
    int NewDId=BowDocBs->DocNmToDescStrH.AddKey(DocNm);
    PBowSpV NewSpV=TBowSpV::New(NewDId, WIds);
    BowDocBs->DocSpVV.Add(NewSpV); IAssert(NewDId==BowDocBs->DocSpVV.Len()-1);
    for (int WIdN=0; WIdN<WIds; WIdN++){
      int WId; double WordFq; SpV->GetWIdWgt(WIdN, WId, WordFq);
      TStr WordStr=GetWordStr(WId);
      int NewWId=BowDocBs->WordStrToDescH.AddKey(WordStr);
      BowDocBs->WordStrToDescH[NewWId].Fq++;
      NewSpV->AddWIdWgt(NewWId, WordFq);
    }
    NewSpV->Sort();
    // categories
    BowDocBs->DocCIdVV.Add(); IAssert(NewDId+1==BowDocBs->DocCIdVV.Len());
    BowDocBs->DocCIdVV.Last().Gen(DocCIdV.Len(), 0);
    for (int DocCIdN=0; DocCIdN<DocCIdV.Len(); DocCIdN++){
      int CId=DocCIdV[DocCIdN];
      TStr CatNm=GetCatNm(CId);
      const int NewCId=BowDocBs->CatNmToFqH.AddKey(CatNm);
      BowDocBs->CatNmToFqH[NewCId]++;
      BowDocBs->DocCIdVV.Last().Add(NewCId);
    }
  }
  // return results
  return BowDocBs;
}

PBowDocBs TBowDocBs::GetInvDocBs() const {
  // shortcuts
  int Docs=GetDocs();
  int Words=GetWords();
  // create document set
  PBowDocBs InvBowDocBs=TBowDocBs::New();
  // generate words
  InvBowDocBs->WordStrToDescH.Gen(Docs);
  TIntV WordFqV(Words);
  for (int DId=0; DId<Docs; DId++){
    TStr DocNm=GetDocNm(DId);
    PBowSpV DocSpV=GetDocSpV(DId);
    int DocWIds=DocSpV->GetWIds();
    InvBowDocBs->WordStrToDescH.AddDat(DocNm)=TBowWordDesc(DocWIds, 1, 1);
    for (int WIdN=0; WIdN<DocWIds; WIdN++){
      int WId; double Wgt; DocSpV->GetWIdWgt(WIdN, WId, Wgt);
      WordFqV[WId]++;
    }
  }
  // generate documents
  InvBowDocBs->DocNmToDescStrH.Gen(Words);
  InvBowDocBs->DocSpVV.Gen(Words, 0);
  InvBowDocBs->DocCIdVV.Gen(Words, 0);
  for (int WId=0; WId<Words; WId++){
    TStr WordStr=GetWordStr(WId);
    int InvDId=InvBowDocBs->DocNmToDescStrH.AddKey(WordStr); IAssert(WId==InvDId);
    int InvDocWIds=WordFqV[WId];
    PBowSpV BowSpV=TBowSpV::New(InvDId, InvDocWIds);
    InvBowDocBs->DocSpVV.Add(BowSpV);
  }
  // add words to documents
  {for (int DId=0; DId<Docs; DId++){
    PBowSpV DocSpV=GetDocSpV(DId);
    int DocWIds=DocSpV->GetWIds();
    for (int WIdN=0; WIdN<DocWIds; WIdN++){
      int WId; double Wgt; DocSpV->GetWIdWgt(WIdN, WId, Wgt);
      InvBowDocBs->DocSpVV[WId]->AddWIdWgt(DId, 1);
    }
  }}
  // sort word-vectors
  {for (int WId=0; WId<Words; WId++){
    InvBowDocBs->DocSpVV[WId]->Sort();
    IAssert(InvBowDocBs->DocSpVV[WId]->Len()==InvBowDocBs->DocSpVV[WId]->Reserved());
  }}
  // return results
  InvBowDocBs->AssertOk();
  return InvBowDocBs;
}

void TBowDocBs::SaveTxtStat(const TStr& StatFNm,
 const bool& SaveWordsP, const bool& SaveCatsP, const bool& SaveDocsP) const {
  // create output file
  TFOut StatSOut(StatFNm); FILE* fStat=StatSOut.GetFileId();
  // save word-statistics
  if (SaveWordsP){
    fprintf(fStat, "\nDocument-Frequency Word-Statistics\n\n");
    TBowWordDescStrPrV WordDescStrPrV;
    WordStrToDescH.GetDatKeyPrV(WordDescStrPrV);
    WordDescStrPrV.Sort(false);
    for (int WordN=0; WordN<WordDescStrPrV.Len(); WordN++){
      fprintf(fStat, "%d.\t%d\t'%s'\n",
       1+WordN, (int)WordDescStrPrV[WordN].Val1.Fq, WordDescStrPrV[WordN].Val2.CStr());
    }
  }
  // save category-statistics
  if (SaveCatsP){
    fprintf(fStat, "\nCategory-Statistics\n\n");
    TIntStrPrV FqCatNmPrV; CatNmToFqH.GetDatKeyPrV(FqCatNmPrV);
    FqCatNmPrV.Sort(false);
    for (int FqCatNmPrN=0; FqCatNmPrN<FqCatNmPrV.Len(); FqCatNmPrN++){
      fprintf(fStat, "%d\t'%s'\n",
       (int)FqCatNmPrV[FqCatNmPrN].Val1, FqCatNmPrV[FqCatNmPrN].Val2.CStr());
    }
  }
  // save document-statistics
  if (SaveDocsP){
    fprintf(fStat, "Document-Statistics\n\n");
    int Docs=GetDocs();
    for (int DId=0; DId<Docs; DId++){
      TStr DocNm=GetDocNm(DId);
      TStr DescStr=GetDocDescStr(DId);
      int DocWIds=GetDocWIds(DId);
      fprintf(fStat, "DId:%d   Name:'%s' Desc: '%s' (%d Words):",
       DId, DocNm.CStr(), DescStr.CStr(), DocWIds);
      int DocCIds=GetDocCIds(DId);
      if (DocCIds>0){
        fprintf(fStat, "Cats:[");
        for (int DocCIdN=0; DocCIdN<DocCIds; DocCIdN++){
          int CId=GetDocCId(DId, DocCIdN);
          TStr CatNm=GetCatNm(CId);
          if (DocCIdN>0){fprintf(fStat, ", ");}
          fprintf(fStat, "'%s'", CatNm.CStr());
        }
        fprintf(fStat, "] ");
      }
      for (int DocWIdN=0; DocWIdN<DocWIds; DocWIdN++){
        int WId; double WordFq;
        GetDocWIdFq(DId, DocWIdN, WId, WordFq);
        TStr WordStr=GetWordStr(WId);
        //fprintf(fStat, " [Str:'%s' Id:%d Fq:%g]", WordStr.CStr(), WId, WordFq);
        fprintf(fStat, " '%s':%g", WordStr.CStr(), WordFq);
      }
      fprintf(fStat, "\n");
    }
  }
}

PBowSpV TBowDocBs::GetSpVFromHtmlStr(
 const TStr& HtmlStr, const PBowDocWgtBs& BowDocWgtBs) const {
  // get word-list
  TStrV WordStrV; GetWordStrVFromHtml(HtmlStr, WordStrV);
  // get word-frequencies
  TIntH DocWIdToFqH(100);
  for (int WordStrN=0; WordStrN<WordStrV.Len(); WordStrN++){
    int WId;
    if (IsWordStr(WordStrV[WordStrN], WId)){
      DocWIdToFqH.AddDat(WId)++;
    }
  }
  // create document sparse vector
  PBowSpV DocSpV;
  if (BowDocWgtBs.Empty()){
    // create sparse vectors with word frequencies
    int DocWIds=DocWIdToFqH.Len();
    DocSpV=TBowSpV::New(-1, DocWIds);
    // traverse words
    TInt DocWId; TInt DocWordFq;
    for (int DocWIdN=0; DocWIdN<DocWIds; DocWIdN++){
      // get word id & frequency
      DocWIdToFqH.GetKeyDat(DocWIdN, DocWId, DocWordFq);
      DocSpV->AddWIdWgt(DocWId, DocWordFq);
    }
    DocSpV->Sort();
  } else {
    // create sparse vectors with TDIDF word weights
    int MnWordFq=BowDocWgtBs->GetMnWordFq();
    int Docs=BowDocWgtBs->GetDocs();
    int DocWIds=DocWIdToFqH.Len();
    DocSpV=TBowSpV::New(-1, DocWIds);
    // calculate & add words weights
    TInt DocWId; TInt DocWordFq;
    for (int DocWIdN=0; DocWIdN<DocWIds; DocWIdN++){
      // get word id & freq.
      DocWIdToFqH.GetKeyDat(DocWIdN, DocWId, DocWordFq);
      //TStr WordStr=GetWordStr(DocWId); // for debugging
      if (GetWordFq(DocWId)>=MnWordFq){
        // calculate & add TFIDF weight
        double WordDf=BowDocWgtBs->GetWordFq(DocWId);
        double WordIDf=0;
        if (WordDf>0){WordIDf=log(double(Docs)/WordDf);}
        double DocWordWgt=DocWordFq*WordIDf;
        DocSpV->AddWIdWgt(DocWId, DocWordWgt);
      }
    }
    DocSpV->Sort();
    // cut low weight words
    DocSpV->CutLowWgtWords(BowDocWgtBs->GetCutWordWgtSumPrc());
    // put unit-norm
    DocSpV->PutUnitNorm();
  }
  // return sparse vector
  return DocSpV;
}

PBowSpV TBowDocBs::GetSpVFromHtmlFile(
 const TStr& HtmlFNm, const PBowDocWgtBs& BowDocWgtBs) const {
  PSIn HtmlSIn=TFIn::New(HtmlFNm);
  TStr HtmlStr=TStr::LoadTxt(HtmlSIn);
  return GetSpVFromHtmlStr(HtmlStr, BowDocWgtBs);
}

PBowSpV TBowDocBs::GetSpVFromWIdWgtPrV(
 const TIntFltPrV& WIdWgtPrV, const PBowDocWgtBs& BowDocWgtBs) const {
  // create document sparse vector
  PBowSpV DocSpV;
  if (BowDocWgtBs.Empty()){
    // create sparse vectors with word frequencies
    int DocWIds=WIdWgtPrV.Len();
    DocSpV=TBowSpV::New(-1, DocWIds);
    // traverse words
    TInt DocWId; TInt DocWordFq;
    for (int DocWIdN=0; DocWIdN<DocWIds; DocWIdN++){
      // get word id & frequency
      DocSpV->AddWIdWgt(WIdWgtPrV[DocWIdN].Val1, WIdWgtPrV[DocWIdN].Val2);
    }
    DocSpV->Sort();
  } else {
    // create sparse vectors with TDIDF word weights
    int MnWordFq=BowDocWgtBs->GetMnWordFq();
    int Docs=BowDocWgtBs->GetDocs();
    int DocWIds=WIdWgtPrV.Len();
    DocSpV=TBowSpV::New(-1, DocWIds);
    // calculate & add words weights
    TInt DocWId; double DocWordFq;
    for (int DocWIdN=0; DocWIdN<DocWIds; DocWIdN++){
      // get word id & freq.
      DocWId=WIdWgtPrV[DocWIdN].Val1;
      DocWordFq=WIdWgtPrV[DocWIdN].Val2;
      //TStr WordStr=GetWordStr(DocWId); // for debugging
      if (GetWordFq(DocWId)>=MnWordFq){
        // calculate & add TFIDF weight
        double WordDf=BowDocWgtBs->GetWordFq(DocWId);
        double WordIDf=0;
        if (WordDf>0){WordIDf=log(double(Docs)/WordDf);}
        double DocWordWgt=DocWordFq*WordIDf;
        DocSpV->AddWIdWgt(DocWId, DocWordWgt);
      }
    }
    DocSpV->Sort();
    // cut low weight words
    DocSpV->CutLowWgtWords(BowDocWgtBs->GetCutWordWgtSumPrc());
    // put unit-norm
    DocSpV->PutUnitNorm();
  }
  // return sparse vector
  return DocSpV;
}

const TStr TBowDocBs::BowDocBsFExt=".Bow";

