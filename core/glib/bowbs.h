/////////////////////////////////////////////////
// Forward
ClassHdTP(TBowDocBs, PBowDocBs);

/////////////////////////////////////////////////
// BagOfWords-Keywords-Set
ClassTP(TBowKWordSet, PBowKWordSet)//{
private:
  TStr Nm;
  THash<TStr, TFlt> WStrToWWgtH;
public:
  TBowKWordSet(const TStr& _Nm=""): Nm(_Nm), WStrToWWgtH(){}
  static PBowKWordSet New(const TStr& Nm=""){return new TBowKWordSet(Nm);}
  TBowKWordSet(const TBowKWordSet& BowKWordSet):
    Nm(BowKWordSet.Nm), WStrToWWgtH(BowKWordSet.WStrToWWgtH){}
  TBowKWordSet(TSIn& SIn): Nm(SIn), WStrToWWgtH(SIn){}
  static PBowKWordSet Load(TSIn& SIn){return new TBowKWordSet(SIn);}
  void Save(TSOut& SOut) const {Nm.Save(SOut); WStrToWWgtH.Save(SOut);}

  TBowKWordSet& operator=(const TBowKWordSet& BowKWordSet){
    Nm=BowKWordSet.Nm; WStrToWWgtH=BowKWordSet.WStrToWWgtH; return *this;}

  void PutNm(const TStr& _Nm){Nm=_Nm;}
  TStr GetNm() const {return Nm;}

  void AddKWord(const TStr& KWordStr, const double& KWordWgt){
    WStrToWWgtH.AddDat(KWordStr, KWordWgt);}
  int GetKWords() const {return WStrToWWgtH.Len();}
  bool IsKWordStr(const TStr& KWordStr) const {return WStrToWWgtH.IsKey(KWordStr);}
  int GetKWordN(const TStr& KWordStr) const {return WStrToWWgtH.GetKeyId(KWordStr);}
  TStr GetKWordStr(const int& KWordN) const {return WStrToWWgtH.GetKey(KWordN);}
  double GetKWordWgt(const int& KWordN) const {return WStrToWWgtH[KWordN];}
  void GetWordStrV(TStrV& WordStrV) const;
  TStr GetKWordsStr() const;

  // postprocessing
  void SortByStr();
  void SortByWgt();
  PBowKWordSet GetTopKWords(const int& MxKWords, const double& WgtSumPrc) const;

  // files
  void SaveTxt(const PSOut& SOut) const;
  void SaveTxt(const TStr& FNm) const {
    PSOut SOut=TFOut::New(FNm); SaveTxt(SOut);}
  void SaveXml(const PSOut& SOut) const;
  void SaveXml(const TStr& FNm) const {
    PSOut SOut=TFOut::New(FNm); SaveXml(SOut);}
  void SaveBin(const PSOut& SOut) const {
    Save(*SOut);}
  void SaveBin(const TStr& FNm) const {
    PSOut SOut=TFOut::New(FNm); SaveBin(SOut);}
};

/////////////////////////////////////////////////
// BagOfWords-Keywords-Base
ClassTP(TBowKWordBs, PBowKWordBs)//{
private:
  THash<TStr, PBowKWordSet> SetNmToSetH;
public:
  TBowKWordBs(): SetNmToSetH(){}
  static PBowKWordBs New(){return new TBowKWordBs();}
  TBowKWordBs(const TBowKWordBs& BowKWordBs):
    SetNmToSetH(BowKWordBs.SetNmToSetH){}
  TBowKWordBs(TSIn& SIn): SetNmToSetH(SIn){}
  static PBowKWordBs Load(TSIn& SIn){return new TBowKWordBs(SIn);}
  void Save(TSOut& SOut) const {SetNmToSetH.Save(SOut);}

  TBowKWordBs& operator=(const TBowKWordBs& BowKWordBs){
    SetNmToSetH=BowKWordBs.SetNmToSetH; return *this;}

  // keyword-sets
  void AddKWordSet(const TStr& KWordSetNm, const PBowKWordSet& KWordSet){
    SetNmToSetH.AddDat(KWordSetNm, KWordSet);}
  void AddKWordSet(const PBowKWordSet& KWordSet){
    SetNmToSetH.AddDat(KWordSet->GetNm(), KWordSet);}
  int GetKWordSets() const {return SetNmToSetH.Len();}
  bool IsKWordSset(const TStr& KWordSetNm) const {return SetNmToSetH.IsKey(KWordSetNm);}
  PBowKWordSet GetKWordSet(const int& KWordSetN) const {return SetNmToSetH[KWordSetN];}
  PBowKWordSet GetKWordSet(const TStr& KWordSetNm) const {return SetNmToSetH.GetDat(KWordSetNm);}

  // files
  void SaveTxt(const PSOut& SOut) const;
  void SaveTxt(const TStr& FNm) const {
    PSOut SOut=TFOut::New(FNm); SaveTxt(SOut);}
  void SaveXml(const PSOut& SOut) const;
  void SaveXml(const TStr& FNm) const {
    PSOut SOut=TFOut::New(FNm); SaveXml(SOut);}
  static PBowKWordBs LoadBin(const TStr& FNm){
    TFIn SIn(FNm); return Load(SIn);}
  void SaveBin(const PSOut& SOut) const {
    Save(*SOut);}
  void SaveBin(const TStr& FNm) const {
    PSOut SOut=TFOut::New(FNm); SaveBin(SOut);}
};

/////////////////////////////////////////////////
// BagOfWords-WordId-Weight-Types
typedef TIntSFltKd TBowWIdWgtKd;
typedef TBowWIdWgtKd* PBowWIdWgtKd;
typedef TVec<TBowWIdWgtKd> TBowWIdWgtKdV;
typedef TVec<TBowWIdWgtKdV> TBowWIdWgtKdVV;

/////////////////////////////////////////////////
// BagOfWords-Sparse-Vector
ClassTPV(TBowSpV, PBowSpV, TBowSpVV)//{
private:
  TInt DId;
  TFlt Norm;
  TBowWIdWgtKdV WIdWgtKdV;
  UndefCopyAssign(TBowSpV);
public:
  TBowSpV(const int& _DId=-1, const int& ExpWIds=0):
    DId(_DId), Norm(-1), WIdWgtKdV(ExpWIds, 0){}
  static PBowSpV New(const int& DId=-1, const int& ExpWIds=0){
    return PBowSpV(new TBowSpV(DId, ExpWIds));}
  TBowSpV(const int& DId, const TFltV& FullVec, const double& Eps=1e-14);
  static PBowSpV New(const int& DId, const TFltV& FullVec,
    const double& Eps=1e-14) { return PBowSpV(new TBowSpV(DId, FullVec, Eps));}
  TBowSpV(const int& DId, const TIntFltKdV& SpV);
  static PBowSpV New(const int& DId, const TIntFltKdV& SpV) {
    return PBowSpV(new TBowSpV(DId, SpV));}
  TBowSpV(TSIn& SIn):
    DId(SIn), Norm(SIn), WIdWgtKdV(SIn){}
  static PBowSpV Load(TSIn& SIn){return new TBowSpV(SIn);}
  void Save(TSOut& SOut) const {
    DId.Save(SOut); Norm.Save(SOut); WIdWgtKdV.Save(SOut);}

  // general
  void Clr(){DId=-1; WIdWgtKdV.Clr();}
  void GenMx(const int& MxWIds){WIdWgtKdV.Gen(MxWIds, 0);}
  void Sort(){WIdWgtKdV.Sort();}
  void Trunc(){WIdWgtKdV.Trunc();}
  int Len() const {return WIdWgtKdV.Len();}
  int Reserved() const {return WIdWgtKdV.Reserved();}

  // doc-id
  void PutDId(const int& _DId){DId=_DId;}
  int GetDId() const {return DId;}
  bool IsDId() const {return DId!=-1;}

  // iterators
  PBowWIdWgtKd BegI() const {return WIdWgtKdV.BegI();}
  PBowWIdWgtKd EndI() const {return WIdWgtKdV.EndI();}
  PBowWIdWgtKd GetI(const int& ValN) const {return WIdWgtKdV.GetI(ValN);}

  // normal
  void AssignNorm(const double& _Norm){Norm=_Norm;}
  void PutUnitNorm();
  double GetNorm();

  // word-id/weights
  void AddWIdWgt(const int& WId, const double& Wgt){
    WIdWgtKdV.Add(TBowWIdWgtKd(WId, (sdouble)Wgt));}
  int GetWIds() const {return WIdWgtKdV.Len();}
  int GetWIdN(const int& WId) const {return WIdWgtKdV.SearchForw(TBowWIdWgtKd(WId));}
  bool IsWId(const int& WId) const {return WIdWgtKdV.SearchForw(TBowWIdWgtKd(WId))!=-1;}
  int GetWId(const int& WIdN) const {return WIdWgtKdV[WIdN].Key;}
  TSFlt& GetWgt(const int& WIdN){return WIdWgtKdV[WIdN].Dat;}
  void GetWIdWgt(const int& WIdN, int& WId, double& Wgt) const {
    const TBowWIdWgtKd& WIdWgtKd=WIdWgtKdV[WIdN];
    WId=WIdWgtKd.Key; Wgt=WIdWgtKd.Dat;}
  void GetWordStrWgtPrV(const PBowDocBs& BowDocBs,
   const int& TopWords, const double& TopWordsWgtPrc,
   TStrFltPrV& WordStrWgtPrV) const;
  PBowKWordSet GetKWordSet(const PBowDocBs& BowDocBs) const;
  int GetLastWId() const { return WIdWgtKdV.Last().Key; }
  void GetIntFltKdV(TIntFltKdV& SpV) const;

  // cut low frequency words
  void CutLowWgtWords(const double& CutWordWgtSumPrc);

  // strings
  TStr GetStr(const PBowDocBs& BowDocBs,
   const int& TopWords=-1, const double& TopWordsWgtPrc=1, const TStr& SepStr=" ",
   const bool& ShowWeightsP=true, const bool& KeepUndelineP=true) const;

  // files
  void SaveTxt(const PSOut& SOut, const PBowDocBs& BowDocBs,
   const int& TopWords=-1, const double& TopWordsWgtPrc=1,
   const char& SepCh=' ') const;
  void SaveXml(const PSOut& SOut, const PBowDocBs& BowDocBs) const;
};

/////////////////////////////////////////////////
// BagOfWords-Similarity-Matrix
ClassTP(TBowSimMtx, PBowSimMtx)//{
private:
  TIntV MtxDIdV;
  TIntPrFltH DIdPrToSimH;
public:
  TBowSimMtx(): MtxDIdV(), DIdPrToSimH(){}
  static PBowSimMtx New(){return new TBowSimMtx();}
  TBowSimMtx(TSIn& SIn): MtxDIdV(SIn), DIdPrToSimH(SIn){}
  static PBowSimMtx Load(TSIn& SIn){return new TBowSimMtx(SIn);}
  void Save(TSOut& SOut) const {MtxDIdV.Save(SOut); DIdPrToSimH.Save(SOut);}

  TBowSimMtx& operator=(const TBowSimMtx&){Fail; return *this;}

  // doc-ids
  int GetDocs() const {return MtxDIdV.Len();}
  int GetMtxDId(const int& MtxDIdN) const {return MtxDIdV[MtxDIdN];}
  void GetDIdV(TIntV& _MtxDIdV) const {_MtxDIdV=MtxDIdV;}

  // similarity
  double GetSim(const int& DId1, const int& DId2) const;

  // files
  static PBowSimMtx LoadTxt(const TStr& FNm);
};

/////////////////////////////////////////////////
// BagOfWords-Similarity
typedef enum {bstUndef, bstBlock, bstEucl, bstCos, bstMtx} TBowSimType;

ClassTP(TBowSim, PBowSim)//{
private:
  TInt SimType; // TBowSimType
  PBowSimMtx SimMtx;
public:
  TBowSim(const TBowSimType& _Type=bstUndef, const PBowSimMtx& _SimMtx=NULL):
    SimType(_Type), SimMtx(_SimMtx){}
  static PBowSim New(const TBowSimType& SimType, const PBowSimMtx& SimMtx=NULL){
    return new TBowSim(SimType, SimMtx);}
  TBowSim(TSIn& SIn): SimType(SIn), SimMtx(SIn){}
  static PBowSim Load(TSIn& SIn){return new TBowSim(SIn);}
  void Save(TSOut& SOut) const {SimType.Save(SOut); SimMtx.Save(SOut);}

  TBowSim& operator=(const TBowSim&){Fail; return *this;}

  // typed similarity
  TBowSimType GetSimType() const {return TBowSimType(int(SimType));}
  double GetSim(const int& DId1, const int& DId2) const;
  double GetSim(const PBowSpV& SpV1, const PBowSpV& SpV2) const;
  double GetSim(const TBowSpVV& SpVV1, const TBowSpVV& SpVV2) const;

  // similarity functions
  static double GetBlockSim(const PBowSpV& SpV1, const PBowSpV& SpV2);
  static double GetEuclSim(const PBowSpV& SpV1, const PBowSpV& SpV2);
  static double GetCosSim(const PBowSpV& SpV1, const PBowSpV& SpV2);
  static double GetCosSim(const PBowSpV& SpV1, const PBowSpV& SpV2,
   TFltIntPrV& WgtWIdPrV);

  // similarity functions
  static TBowSimType GetSimType(const TStr& Nm);
};

/////////////////////////////////////////////////
// BagOfWords-Document-Weights-Base
typedef enum {bwwtUndef,
  bwwtEq, // vectors directly copied from BowDocBs
  bwwtNrmEq, // + normalized to norm 1.0
  bwwtBin, // features changed to binary (1.0 if nonzero, else 0.0)
  bwwtNrmBin, // + normalized
  bwwtNrm01, // each feature is normalized between 0.0 and 1.0
  bwwtNrmTFIDF, // x[i] = TF[i] * log(Docs/DF[i]) and normalized
  bwwtLogDFNrmTFIDF, // x[i] = log(1+DF[i]) * TF[i] * log(Docs/DF[i]) and normalized
  bwwtNrmTFICF, //
  bwwtPreCalc, // weights are passed as a parameter (use function NewPreCalcWgt)
  bwwtSvm // wegiths are calculated using SVM (use function NewSvmWgt)
} TBowWordWgtType;

ClassTP(TBowDocWgtBs, PBowDocWgtBs)//{
private:
  TInt Sig; // signature
  TInt BowDocBsSig; // corresponding BowDocBs signature
  TInt WordWgtType;
  TFlt CutWordWgtSumPrc;
  TInt MnWordFq;
  TIntV DIdV;
  TFltV WordFqV;
  TBowSpVV DocSpVV;
public:
  TBowDocWgtBs(const int& _BowDocBsSig):
    Sig(TSecTm::GetCurTm().GetAbsSecs()), BowDocBsSig(_BowDocBsSig),
    WordWgtType(), CutWordWgtSumPrc(), MnWordFq(),
    DIdV(), WordFqV(), DocSpVV(){}
  static PBowDocWgtBs New(const int& BowDocBsSig){
    return PBowDocWgtBs(new TBowDocWgtBs(BowDocBsSig));}
  static PBowDocWgtBs New(
   const PBowDocBs& BowDocBs, const TBowWordWgtType& _WordWgtType,
   const double& _CutWordWgtSumPrc=0, const int& _MnWordFq=0,
   const TIntV& _DIdV=TIntV(), const TIntV& BaseDIdV=TIntV(),
   const PNotify& Notify=TNotify::NullNotify);
  // generates weighted bow from precalculated sparse vectors
  static PBowDocWgtBs New(const TVec<PBowSpV>& BowSpVV);
  // generates weighted bow from precalculated word weights
  static PBowDocWgtBs NewPreCalcWgt(const PBowDocBs& BowDocBs,
   const TFltV& WordWgtV, const bool& PutUniteNorm = false,
   const double& _CutWordWgtSumPrc=0, const int& _MnWordFq=0,
   const TIntV& _DIdV=TIntV());
  // generates weighted bow using SVM to generate word weights
  // (uses category information for all documents, therefor
  // not usefull for classification but only for clustering)
  static PBowDocWgtBs NewSvmWgt(
   const PBowDocBs& BowDocBs,
   const PBowDocWgtBs& BowDocWgtBs,
   const TIntV& _TrainDIdV = TIntV(),
   const double& SvmCostParam = 1.0,
   const int& MxTimePerCat = 60,
   const bool& NegFeaturesP = false,
   const TIntV& _DIdV = TIntV(),
   const bool& PutUniteNormP = false,
   const double& _CutWordWgtSumPrc = 0.0,
   const int& _MnWordFq = 0);
  // generates weighted bow using SVM to generate word weights
  // weights are optimized for the given category
  static PBowDocWgtBs NewBinSvmWgt(
   const PBowDocBs& BowDocBs,
   const PBowDocWgtBs& BowDocWgtBs,
   const TStr& CatNm,
   const TIntV& TrainDIdV = TIntV(),
   const double& SvmCostParam = 1.0,
   const double& SvmUnbalanceParam = 1.0,
   const double& MnWgt = 0.0,
   const bool& NegFeaturesP = false,
   const bool& PutUniteNormP = true,
   const bool& AvgNormalP = true,
   const TIntV& _DIdV = TIntV(),
   const double& _CutWordWgtSumPrc = 0.0,
   const int& _MnWordFq = 0);
  TBowDocWgtBs(TSIn& SIn):
    Sig(SIn), BowDocBsSig(SIn),
    WordWgtType(SIn), CutWordWgtSumPrc(SIn), MnWordFq(SIn),
    DIdV(SIn), WordFqV(SIn), DocSpVV(SIn){}
  static PBowDocWgtBs Load(TSIn& SIn){return new TBowDocWgtBs(SIn);}
  void Save(TSOut& SOut) const {
    Sig.Save(SOut); BowDocBsSig.Save(SOut);
    WordWgtType.Save(SOut); CutWordWgtSumPrc.Save(SOut); MnWordFq.Save(SOut);
    DIdV.Save(SOut); WordFqV.Save(SOut); DocSpVV.Save(SOut);}

  TBowDocWgtBs& operator=(const TBowDocWgtBs& BowDocWgtBs){
    Sig=BowDocWgtBs.Sig;
    BowDocBsSig=BowDocWgtBs.BowDocBsSig;
    WordWgtType=BowDocWgtBs.WordWgtType;
    CutWordWgtSumPrc=BowDocWgtBs.CutWordWgtSumPrc;
    MnWordFq=BowDocWgtBs.MnWordFq;
    DIdV=BowDocWgtBs.DIdV;
    WordFqV=BowDocWgtBs.WordFqV;
    DocSpVV=BowDocWgtBs.DocSpVV;
    return *this;}

  // added for the purpose of propagating weights in clustering algorithms
  PBowDocWgtBs GetSubSet(const TIntV& IdSubSet){
    PBowDocWgtBs BowDocWgtBs=TBowDocWgtBs::New(BowDocBsSig);
    *BowDocWgtBs=*this;
    BowDocWgtBs->DIdV=IdSubSet;
    return BowDocWgtBs;}

  // signatures
  int GetSig() const {return Sig;}
  int GetBowDocBsSig() const {return BowDocBsSig;}

  // word weighting functions
  TBowWordWgtType GetWordWgtType() const {
    return TBowWordWgtType(int(WordWgtType));}
  static TBowWordWgtType GetWordWgtTypeFromStr(const TStr& Nm);

  // create-parameters
  double GetCutWordWgtSumPrc() const {return CutWordWgtSumPrc;}
  int GetMnWordFq() const {return MnWordFq;}

  // documents
  int GetDocs() const {return DIdV.Len();}
  int GetDId(const int& DIdN) const {return DIdV[DIdN];}
  PBowSpV GetSpV(const int& DId) const {return DocSpVV[DId];}
  void SetSpV(const int& DId, PBowSpV DocSpV) { DocSpVV[DId] = DocSpV; } //B:

  void GetDIdV(TIntV& _DIdV) const {_DIdV=DIdV;}
  void SetDIdV(const TIntV& _DIdV){DIdV=_DIdV;}

  const TBowSpVV& GetDocSpVV() const { return DocSpVV; }

  // words
  int GetWords() const {return WordFqV.Len();}
  double GetWordFq(const int& WId) const {return WordFqV[WId];}

  // similarity search
  void GetSimDIdV(
   const PBowSpV& RefBowSpV, const PBowSim& BowSim,
   TFltIntKdV& SimDIdKdV, const bool& RefBowSpVInclude=false) const;
  void SaveTxtSimDIdV(
   const PSOut& SOut, const PBowDocBs& BowDocBs,
   const PBowSpV& RefBowSpV, const TFltIntKdV& SimDIdKdV,
   const int& TopHits=-1, const double& MnSim=0, const int& TopDocWords=-1,
   const char& SepCh=' ') const;
  void SaveXmlSimDIdV(
   const PSOut& SOut, const PBowDocBs& BowDocBs,
   const PBowSpV& RefBowSpV, const TFltIntKdV& SimDIdKdV,
   const int& TopHits=-1, const double& MnSim=0) const;

  // statistics files
  void SaveTxtStat(
   const TStr& StatFNm, const PBowDocBs& BowDocBs,
   const bool& SaveWordsP=true, const bool& SaveCatsP=true,
   const bool& SaveDocsP=true) const;

  // binary files
  static const TStr BowDocWgtBsFExt;
  static PBowDocWgtBs LoadBin(const TStr& FNm){
    TFIn SIn(FNm); return Load(SIn);}
  void SaveBin(const TStr& FNm) const {
    TFOut SOut(FNm); Save(SOut);}
};

/////////////////////////////////////////////////
// BagOfWords-Word-Description
class TBowWordDesc{
public:
  TInt Fq;
  TFlt MnVal;
  TFlt MxVal;
public:
  TBowWordDesc():
    Fq(0), MnVal(0), MxVal(0){}
  TBowWordDesc(const int& _Fq, const double& _MnVal, const double& _MxVal):
    Fq(_Fq), MnVal(_MnVal), MxVal(_MxVal){}

  TBowWordDesc(TSIn& SIn):
    Fq(SIn), MnVal(SIn), MxVal(SIn){}
  void Save(TSOut& SOut) const {
    Fq.Save(SOut); MnVal.Save(SOut); MxVal.Save(SOut);}

  TBowWordDesc& operator=(const TBowWordDesc& BowWordDesc){
    Fq=BowWordDesc.Fq; MnVal=BowWordDesc.MnVal; MxVal=BowWordDesc.MxVal;
    return *this;}
  bool operator==(const TBowWordDesc& BowWordDesc) const {
    return Fq==BowWordDesc.Fq;}
  bool operator<(const TBowWordDesc& BowWordDesc) const {
    return Fq<BowWordDesc.Fq;}
};
typedef TPair<TBowWordDesc, TStr> TBowWordDescStrPr;
typedef TVec<TBowWordDescStrPr> TBowWordDescStrPrV;

/////////////////////////////////////////////////
// BagOfWords-Document-Base
ClassTP(TBowDocBs, PBowDocBs)//{
private:
  TInt Sig; // signature
  PNGramBs NGramBs; // ngram-base
  PSwSet SwSet; // stop-words
  PStemmer Stemmer; // stemmer
  TStrStrH DocNmToDescStrH; // document-names & descriptions (key-id==doc-id)
  THash<TStr, TBowWordDesc> WordStrToDescH; // word-strings (key-id==word-id)
  //THash<TStr, TBowWordDesc, TStrHashF_OldGLib> WordStrToDescH; // word-strings (key-id==word-id) ...for old GLib
  TStrIntH CatNmToFqH; // category-name to frequency (key-id==cat-id)
  TBowSpVV DocSpVV; // vector of document word-id vectors (DocSpVV[DId])
  TStrV DocStrV; // vector of original document strings
  TVec<TIntV> DocCIdVV; // vector of cat-id vectors (DocCIdVV[DId])
  TIntV TrainDIdV; // vector of training doc-ids
  TIntV TestDIdV; // vector of testing doc-ids
public:
  TBowDocBs():
    Sig(TSecTm::GetCurTm().GetAbsSecs()),
    NGramBs(), SwSet(), Stemmer(),
    DocNmToDescStrH(), WordStrToDescH(), CatNmToFqH(),
    DocSpVV(), DocStrV(), DocCIdVV(),
    TrainDIdV(), TestDIdV(){}
  static PBowDocBs New(){return new TBowDocBs();}
  static PBowDocBs New(
   const PSwSet& SwSet, const PStemmer& Stemmer, const PNGramBs& NGramBs);
  TBowDocBs(TSIn& SIn):
    Sig(SIn),
    NGramBs(SIn), SwSet(SIn), Stemmer(SIn),
    DocNmToDescStrH(SIn), WordStrToDescH(SIn), CatNmToFqH(SIn),
    DocSpVV(SIn), DocStrV(SIn), DocCIdVV(SIn),
    TrainDIdV(SIn), TestDIdV(SIn){}
  static PBowDocBs Load(TSIn& SIn){return new TBowDocBs(SIn);}
  void Save(TSOut& SOut) const {
    Sig.Save(SOut);
    NGramBs.Save(SOut); SwSet.Save(SOut); Stemmer.Save(SOut);
    DocNmToDescStrH.Save(SOut); WordStrToDescH.Save(SOut); CatNmToFqH.Save(SOut);
    DocSpVV.Save(SOut); DocStrV.Save(SOut); DocCIdVV.Save(SOut);
    TrainDIdV.Save(SOut); TestDIdV.Save(SOut);}

  TBowDocBs& operator=(const TBowDocBs& BowDocBs){
    Sig=BowDocBs.Sig;
    NGramBs=BowDocBs.NGramBs;
    SwSet=BowDocBs.SwSet;
    Stemmer=BowDocBs.Stemmer;
    DocNmToDescStrH=BowDocBs.DocNmToDescStrH;
    WordStrToDescH=BowDocBs.WordStrToDescH;
    CatNmToFqH=BowDocBs.CatNmToFqH;
    DocSpVV=BowDocBs.DocSpVV;
    DocStrV=BowDocBs.DocStrV;
    DocCIdVV=BowDocBs.DocCIdVV;
    TrainDIdV=BowDocBs.TrainDIdV;
    TestDIdV=BowDocBs.TestDIdV;
    return *this;}

  // merge two BowDocBs's
  void AddDocs(const PBowDocBs& BowDocBs);

  // testing correctness
  void AssertOk() const;

  // signature
  int GetSig() const {return Sig;}

  // vocabulary
  void PutNGramBs(const PNGramBs& _NGramBs){NGramBs=_NGramBs;}
  PNGramBs GetNGramBs() const {return NGramBs;}
  void PutSwSet(const PSwSet& _SwSet){SwSet=_SwSet;}
  PSwSet GetSwSet() const {return SwSet;}
  void PutStemmer(const PStemmer& _Stemmer){Stemmer=_Stemmer;}
  PStemmer GetStemmer() const {return Stemmer;}

  // words
  int GetWords() const {return WordStrToDescH.Len();}
  int AddWordStr(const TStr& WordStr){return WordStrToDescH.AddKey(WordStr);}
  bool IsWordStr(const TStr& WordStr) const {
    return WordStrToDescH.IsKey(WordStr);}
  bool IsWordStr(const TStr& WordStr, int& WId) const {
    return WordStrToDescH.IsKey(WordStr, WId);}
  bool IsWId(const int& WId) const {
    return WordStrToDescH.IsKeyId(WId);}
  int GetWId(const TStr& WordStr) const {
    return WordStrToDescH.GetKeyId(WordStr);}
  TStr GetWordStr(const int& WId) const {
    return WordStrToDescH.GetKey(WId);}
  double GetWordMnVal(const int& WId) const {
    return WordStrToDescH[WId].MnVal;}
  double GetWordMxVal(const int& WId) const {
    return WordStrToDescH[WId].MxVal;}
  void PutWordFq(const int& WId, const int& Fq){
    WordStrToDescH[WId].Fq=Fq;}
  int GetWordFq(const int& WId) const {
    return WordStrToDescH[WId].Fq;}
  void GetWordStrVFromHtml(const TStr& HtmlStr, TStrV& WordStrV) const;

  // documents
  int AddDoc(const TStr& DocNm,
   const TStrV& CatNmV, const TIntFltPrV& WIdWgtPrV);
  int AddDoc(const TStr& DocNm,
   const TStrV& CatNmV, const TStrV& WordStrV, const TStr& DocStr=TStr());
  int AddDoc(const TStr& DocNm,
   const TStr& CatNm, const TStrV& WordStrV, const TStr& DocStr=TStr()){
    TStrV CatNmV; CatNmV.Add(CatNm);
    return AddDoc(DocNm, CatNmV, WordStrV, DocStr);}
  int AddHtmlDoc(const TStr& DocNm, const TStrV& CatNmV,
   const TStr& HtmlDocStr, const bool& SaveDocP=false);
  int GetDocs() const {return DocSpVV.Len();}
  PBowSpV GetDocSpV(const int& DId) const {return DocSpVV[DId];}
  void PutDocStr(const int& DId, const TStr& DocStr){DocStrV[DId]=DocStr; }
  TStr GetDocStr(const int& DId) const {return DocStrV[DId];}
  bool IsDocNm(const TStr& DocNm) const {
    return DocNmToDescStrH.IsKey(DocNm);}
  bool IsDocNm(const TStr& DocNm, int& DId) const {
    DId=DocNmToDescStrH.GetKeyId(DocNm); return DId!=-1;}
  TStr GetDocNm(const int& DId) const {
    if (DocNmToDescStrH.Empty()){return TInt::GetStr(DId);}
    else {return DocNmToDescStrH.GetKey(DId);}}
  bool IsDId(const int& DId) const {
    return (0<=DId)&&(DId<DocSpVV.Len());}
  int GetDId(const TStr& DocNm) const {
    return DocNmToDescStrH.GetKeyId(DocNm);}
  void GetAllDIdV(TIntV& DIdV) const;
  int GetDocWIds(const int& DId) const {
    return DocSpVV[DId]->GetWIds();}
  void GetDocWIdFq(
   const int& DId, const int& DocWIdN, int& WId, double& WordFq) const {
    DocSpVV[DId]->GetWIdWgt(DocWIdN, WId, WordFq);}
  int GetDocWIdN(const int& DId, const int& DocWId) const {
    return DocSpVV[DId]->GetWId(DocWId);}
  int GetDocWId(const int& DId, const int& DocWIdN) const {
    return DocSpVV[DId]->GetWId(DocWIdN);}
  void PutDocWFq(const int& DId, const int& DocWIdN, double& WordFq){
    DocSpVV[DId]->GetWgt(DocWIdN)=sdouble(WordFq);}
  double GetDocWFq(const int& DId, const int& DocWIdN) const {
    return DocSpVV[DId]->GetWgt(DocWIdN);}
  bool IsDocWordStr(const int& DId, const TStr& WordStr) const;
  int GetDocCIds(const int& DId) const {
    return DocCIdVV.Empty() ? 0 : DocCIdVV[DId].Len();}
  int GetDocCId(const int& DId, const int& DocCIdN) const {
    return DocCIdVV[DId][DocCIdN];}
  bool IsCatInDoc(const int& DId, const int& CId) const {
    return DocCIdVV[DId].IsIn(CId);}
  void PutDocDescStr(const int& DId, const TStr& DocDescStr){
    DocNmToDescStrH[DId]=DocDescStr;}
  TStr GetDocDescStr(const int& DId) const {
    return DocNmToDescStrH[DId];}
  void PutDateStr(const int& DId, const TStr& DateStr){
    PutDocDescStr(DId, DateStr);}
  TStr GetDateStr(const int& DId) const {
    return GetDocDescStr(DId);}

  // categories
  bool IsCats() const {return GetCats()>0;}
  int GetCats() const {return CatNmToFqH.Len();}
  bool IsCatNm(const TStr& CatNm) const {
    return CatNmToFqH.IsKey(CatNm);}
  bool IsCatNm(const TStr& CatNm, int& CId) const {
    return CatNmToFqH.IsKey(CatNm, CId);}
  bool IsCId(const int& CId) const {
    return CatNmToFqH.IsKeyId(CId);}
  int GetCId(const TStr& CatNm) const {
    return CatNmToFqH.GetKeyId(CatNm);}
  TStr GetCatNm(const int& CId) const {
    return CatNmToFqH.GetKey(CId);}
  int GetCatFq(const int& CId) const {
    return CatNmToFqH[CId];}
  void GetTopCatV(const int& TopCats, TIntStrPrV& FqCatNmPrV) const;
  void AddDocCId(const int& DId, const int& CId){
    CatNmToFqH[CId]++; DocCIdVV[DId].AddUnique(CId);}
  int AddCatNm(const TStr& CatNm){
    return CatNmToFqH.AddKey(CatNm);}

    // train & test documents
  void PutTrainDIdV(const TIntV& DIdV){TrainDIdV=DIdV;}
  void PutTestDIdV(const TIntV& DIdV){TestDIdV=DIdV;}
  void PutTrainDocNmV(const TStrV& DocNmV){
    TrainDIdV.Gen(DocNmV.Len(), 0);
    for (int DocNmN=0; DocNmN<DocNmV.Len(); DocNmN++){
      TrainDIdV.Add(GetDId(DocNmV[DocNmN]));}}
  void PutTestDocNmV(const TStrV& DocNmV){
    TestDIdV.Gen(DocNmV.Len(), 0);
    for (int DocNmN=0; DocNmN<DocNmV.Len(); DocNmN++){
      TestDIdV.Add(GetDId(DocNmV[DocNmN]));}}
  void AddTrainDId(const int& DId){TrainDIdV.Add(DId);}
  int GetTrainDocs() const {return TrainDIdV.Len();}
  int GetTrainDId(const int& DIdN) const {return TrainDIdV[DIdN];}
  void GetTrainDIdV(TIntV& _TrainDIdV){_TrainDIdV=TrainDIdV;}
  void AddTestDId(const int& DId){TestDIdV.Add(DId);}
  int GetTestDocs() const {return TestDIdV.Len();}
  int GetTestDId(const int& DIdN) const {return TestDIdV[DIdN];}
  void GetTestDIdV(TIntV& _TestDIdV){_TestDIdV=TestDIdV;}

  // hold-out & cross-validation creation of training & testing documents
  void SetHOTrainTestDIdV(const double& TestDocsPrc, TRnd& Rnd);
  void SetCVTrainTestDIdV(const int& Folds, const int& FoldN, TRnd& Rnd);

  // sampling
  void GetAbsSampleDIdV(const int& Docs, TRnd& Rnd, TIntV& DIdV) const {
    GetAllDIdV(DIdV); DIdV.Shuffle(Rnd); DIdV.Trunc(Docs);}
  void GetRelSampleDIdV(const double& DocsPrc, TRnd& Rnd, TIntV& DIdV) const {
    IAssert((0<=DocsPrc)&&(DocsPrc<=1));
    int Docs=int(DocsPrc*GetDocs()); GetAbsSampleDIdV(Docs, Rnd, DIdV);}

  // transformation
  PBowDocBs GetLimWordRelFqDocBs(
   const double& MnWordFqPrc, const double& MxWordFqPrc) const;
  PBowDocBs GetLimWordAbsFqDocBs(const int& MnWordFq) const;
  PBowDocBs GetSubDocSet(const TIntV& DIdV) const;
  PBowDocBs GetInvDocBs() const;

  // sparse vectors
  PBowSpV GetSpVFromHtmlStr(
   const TStr& HtmlStr, const PBowDocWgtBs& BowDocWgtBs=NULL) const;
  PBowSpV GetSpVFromHtmlFile(
   const TStr& HtmlFNm, const PBowDocWgtBs& BowDocWgtBs=NULL) const;
  PBowSpV GetSpVFromWIdWgtPrV(
   const TIntFltPrV& WIdWgtPrV, const PBowDocWgtBs& BowDocWgtBs=NULL) const;

  // statistics files
  void SaveTxtStat(const TStr& StatFNm, const bool& SaveWordsP=true,
   const bool& SaveCatsP=true, const bool& SaveDocsP=true) const;

  // binary files
  static const TStr BowDocBsFExt;
  static PBowDocBs LoadBin(const TStr& FNm){
    TFIn SIn(FNm); return Load(SIn);}
  void SaveBin(const TStr& FNm) const {
    TFOut SOut(FNm); Save(SOut);}

  friend class TBowFl;
};
