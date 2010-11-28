/////////////////////////////////////////////////
// BagOfWords-Document-Partition-Cluster
ClassHdTP(TBowDocPart, PBowDocPart)

ClassTPV(TBowDocPartClust, PBowDocPartClust, TBowDocPartClustV)//{
private:
  TStr Nm; // name
  TFlt Qual; // quality
  TIntV DIdV; // vector of document-ids
  TFltV DCSimV; // vector of document-to-centroid similarities
  TFltVV DDSimVV; // vector of document-to-document similarities
  PBowSpV ConceptSpV; // concept sparse vector
  PBowDocPart SubPart; // sub partitions
  TFlt Entropy; // entropy
  TIntFltKdV CIdPrbKdV; // cat-id/probability vector
  void CalcEntropy(const PBowDocBs& BowDocBs);
public:
  TBowDocPartClust():
    Nm(), Qual(), DIdV(), DCSimV(), DDSimVV(), ConceptSpV(), SubPart(), Entropy(){}
  TBowDocPartClust(
   const PBowDocBs& BowDocBs, const TStr& _Nm, const double& _Qual,
   const TIntV& _DIdV, const TFltV& _DCSimV, const TFltVV& _DDSimVV,
   const PBowSpV& _ConceptSpV, const PBowDocPart& _SubPart):
    Nm(_Nm), Qual(_Qual), DIdV(_DIdV), DCSimV(_DCSimV), DDSimVV(_DDSimVV),
    ConceptSpV(_ConceptSpV), SubPart(_SubPart), Entropy(), CIdPrbKdV(){
    CalcEntropy(BowDocBs);}
  static PBowDocPartClust New(
   const PBowDocBs& BowDocBs,
   const TStr& Nm, const double& Qual, const TIntV& DIdV,
   const PBowSpV& ConceptSpV, const PBowDocPart& SubPart){
    return new TBowDocPartClust(
     BowDocBs, Nm, Qual, DIdV, TFltV(), TFltVV(), ConceptSpV, SubPart);}
  TBowDocPartClust(TSIn& SIn):
    Nm(SIn), Qual(SIn), DIdV(SIn),
    DCSimV(SIn), DDSimVV(SIn),
    ConceptSpV(SIn), SubPart(SIn),
    Entropy(SIn), CIdPrbKdV(SIn){}
  static PBowDocPartClust Load(TSIn& SIn){return new TBowDocPartClust(SIn);}
  void Save(TSOut& SOut) const {
    Nm.Save(SOut); Qual.Save(SOut); DIdV.Save(SOut);
    DCSimV.Save(SOut); DDSimVV.Save(SOut);
    ConceptSpV.Save(SOut); SubPart.Save(SOut);
    Entropy.Save(SOut); CIdPrbKdV.Save(SOut);}

  TBowDocPartClust& operator=(const TBowDocPartClust& Clust){
    Nm=Clust.Nm; Qual=Clust.Qual; DIdV=Clust.DIdV;
    DCSimV=Clust.DCSimV; DDSimVV=Clust.DDSimVV;
    ConceptSpV=Clust.ConceptSpV; SubPart=Clust.SubPart;
    Entropy=Clust.Entropy; CIdPrbKdV=Clust.CIdPrbKdV;
    return *this;}

  // name
  void PutNm(const TStr& _Nm){Nm=_Nm;}
  TStr GetNm() const {return Nm;}

  // quality & similarity
  double GetQual() const {return Qual;}
  double GetMeanSim() const {
    if (GetDocs()==0){return 0;} else {return GetQual()/double(GetDocs());}}

  // documents
  int GetDocs() const {return DIdV.Len();}
  int GetDId(const int& DIdN) const {return DIdV[DIdN];}
  void GetDIdV(TIntV& _DIdV) const {_DIdV=DIdV;}
  TIntV& GetDIdVRef(){return DIdV;}

  // document-to-centroid similarity
  void AddDCSimV(const TFltV& _DCSimV){DCSimV=_DCSimV;}
  bool IsDCSimV() const {return DIdV.Len()==DCSimV.Len();}
  double GetDCSim(const int& DIdN) const {return DCSimV[DIdN];}

  // document-to-document similarity
  void AddDDSimVV(const TFltVV& _DDSimVV){DDSimVV=_DDSimVV;}
  bool IsDDSimVV() const {
    return (DIdV.Len()>0)&&(DIdV.Len()==DDSimVV.GetXDim())&&(DIdV.Len()==DDSimVV.GetYDim());}
  double GetDDSim(const int& DIdN1, const int& DIdN2) const {
    return DDSimVV.At(DIdN1, DIdN2);}

  // concept vector
  bool IsConceptSpV() const {return !ConceptSpV.Empty();}
  PBowSpV GetConceptSpV() const {return ConceptSpV;}
  int GetConceptWords() const {
    if (IsConceptSpV()){return GetConceptSpV()->GetWIds();} else {return 0;}}
  void GetTopWordStrWgtPrV(
   const PBowDocBs& BowDocBs,
   const int& TopWords, const double& TopWordsWgtPrc,
   TStrFltPrV& WordStrWgtPrV) const;

  // sub-partitions
  bool IsSubPart() const {return !SubPart.Empty();}
  void PutSubPart(const PBowDocPart& _SubPart){SubPart=_SubPart;}
  PBowDocPart GetSubPart() const {return SubPart;}

  // entropy
  bool IsEntropy() const {return GetCIdPrbs()>0;}
  double GetEntropy() const {return Entropy;}

  // category probabilty
  int GetCIdPrbs() const {return CIdPrbKdV.Len();}
  void GetCIdPrb(const int& CIdPrbN, int& CId, double& Prb) const {
    CId=CIdPrbKdV[CIdPrbN].Key; Prb=CIdPrbKdV[CIdPrbN].Dat;}
  double GetCatPrb(const int& CId) const;
  int GetMxPrbCId() const;
};
typedef TKeyDat<TFlt, PBowDocPartClust> TFltBowDocPartClustKd;
typedef TVec<TFltBowDocPartClustKd> TFltBowDocPartClustKdV;

/////////////////////////////////////////////////
// BagOfWords-Document-Partition
ClassTPV(TBowDocPart, PBowDocPart, TBowDocPartV)//{
public:
  TStr Nm;
  TFlt Qual;
  TBowDocPartClustV ClustV;
  TFltVV ClustSimVV;
public:
  TBowDocPart():
    Nm(), Qual(), ClustV(), ClustSimVV(){}
  static PBowDocPart New(){return new TBowDocPart();}
  TBowDocPart(TSIn& SIn):
    Nm(SIn), Qual(SIn), ClustV(SIn), ClustSimVV(SIn){}
  static PBowDocPart Load(TSIn& SIn){return new TBowDocPart(SIn);}
  void Save(TSOut& SOut) const {
    Nm.Save(SOut); Qual.Save(SOut); ClustV.Save(SOut); ClustSimVV.Save(SOut);}

  TBowDocPart& operator=(const TBowDocPart& BowDocPart){
    Nm=BowDocPart.Nm; Qual=BowDocPart.Qual; ClustV=BowDocPart.ClustV;
    ClustSimVV=BowDocPart.ClustSimVV; return *this;}

  // name
  void PutNm(const TStr& _Nm){Nm=_Nm;}
  TStr GetNm() const {return Nm;}

  // clusters
  void AddClust(const PBowDocPartClust& Clust){ClustV.Add(Clust);}
  int GetClusts() const {return ClustV.Len();}
  PBowDocPartClust GetClust(const int& ClustN) const {return ClustV[ClustN];}

  // quality & similarity
  void PutQual(const double& _Qual){Qual=_Qual;}
  double GetQual() const {return Qual;}
  double GetMeanSim() const {
    if (GetDocs()==0){return 0;} else {return GetQual()/GetDocs();}}

  // inter-cluster similarity
  void PutClustSim(const TFltVV& _ClustSimVV){
    ClustSimVV=_ClustSimVV;}
  double GetClustSim(const int& ClustN1, const int& ClustN2) const {
    return ClustSimVV.At(ClustN1, ClustN2);}
  void GetTopClustSimV(
   const double& ClustSimSumPrc, TFltIntIntTrV& ClustSimN1N2TrV) const;

  // documents
  int GetDocs() const;
  void GetDIdV(TIntV& DIdV) const;

  // predictions
  int GetBestClust(const PBowSpV& SpV, const PBowSim& BowSim) const;
  int GetBestCId(const PBowSpV& SpV, const PBowSim& BowSim) const;
  void GetClustSimV(
   const PBowSpV& SpV, const PBowSim& BowSim, TIntFltKdV& ClustSimV) const;

  // cluster names & classifications
  void GetDefClustNmV(const TStr& ClustNm, TStrV& ClustNmV) const;
  void GetDefClustNmV(TStrV& ClustNmV) const;
  void GetCfClustNmV(
   const PBowSpV TestDocSpV, const PBowSim& BowSim, TStrV& ClustNmV) const;
  void GetCfClustNmVV(
   const PBowDocBs& BowDocBs, const TBowWordWgtType& WordWgtType,
   const PBowSim& BowSim, const TIntV& TrainDIdV, const TIntV& TestDIdV,
   TVec<TStrV>& ClustNmVV, TVec<TBowDocPartClustV>& ClustVV) const;
  void SaveXmlCfClustNmVV(
   const PBowDocBs& BowDocBs, const TBowWordWgtType& WordWgtType,
   const PBowSim& BowSim, const TIntV& TrainDIdV, const TIntV& TestDIdV,
   const TStr& FNm);

  // cluster area split
  typedef TPair<PBowDocPartClust, TFltRect> TClustRectPr;
  typedef TVec<TClustRectPr> TClustRectPrV;
  void GetAreaPart(const TFltRect& Rect, TClustRectPrV& ClustRectPrV,
   const bool& ShuffleP=false) const;
  static void GetClustAtXY(const double& X, const double Y,
   const TClustRectPrV& ClustRectPrV, const bool& LeafPartP,
   TBowDocPartClustV& ClustV);

  // text-files
  void SavePartEntropyTxt(const PSOut& SOut, const PBowDocBs& BowDocBs) const;
  void SaveTxt(const PSOut& SOut, const PBowDocBs& BowDocBs,
   const bool& SaveWordsP, const int& TopWords, const double& TopWordsWgtPrc,
   const bool& SaveDocsP, const TStr& PathStr="") const;
  void SaveTxt(const TStr& FNm, const PBowDocBs& BowDocBs,
   const bool& SaveWordsP=true, const int& TopWords=25,
   const double& TopWordsWgtPrc=1.0, const bool& SaveDocsP=false) const {
    PSOut SOut=TFOut::New(FNm);
    SaveTxt(SOut, BowDocBs, SaveWordsP, TopWords, TopWordsWgtPrc, SaveDocsP);}

  // xml-files
  void SaveXml(const PSOut& SOut, const PBowDocBs& BowDocBs,
   const TStr& PathStr="") const;
  void SaveXml(const TStr& FNm, const PBowDocBs& BowDocBs) const {
    PSOut SOut=TFOut::New(FNm); SaveXml(SOut, BowDocBs);}

  // rdf-files (jupi)
  void SaveRdf(const TStr& FNm, const PBowDocBs& BowDocBs) const;
  void SaveRdfClusts(const PSOut& SOut, const PBowDocBs& BowDocBs,
   TIntIntVH& DIdClustIdVH, const int& FatherId, int& MyId) const;

  // binary-files
  static const TStr BowDocPartFExt;
  static PBowDocPart LoadBin(const TStr& FNm){
    TFIn SIn(FNm); return Load(SIn);}
  void SaveBin(const TStr& FNm) const {
    TFOut SOut(FNm); Save(SOut);}
};

/////////////////////////////////////////////////
// BagOfWords-Clustering
class TBowClust{
public:
  // clustering on BowDocWgtBs
  static PBowSpV GetConceptSpV(
   const PBowDocWgtBs& BowDocWgtBs, const PBowSim& BowSim, const TIntV& DIdV,
   const double& CutWordWgtSumPrc=1.0, const TIntFltPrV& DocIdWgtPrV=TIntFltPrV());
  static void GetConceptSpVV(const PBowSim& BowSim,
   const TBowSpVV& SpVV1, const TBowSpVV& SpVV2, TBowSpVV& ConceptSpVV);
  static double GetClustQual(
   const PBowDocWgtBs& BowDocWgtBs,
   const TIntV& DIdV, const PBowSpV& ConceptSpV, const PBowSim& BowSim);
  static void GetPartQual(
   const PBowDocWgtBs& BowDocWgtBs,
   const TVec<TIntV>& DIdVV, const TBowSpVV& ConceptSpVV,
   const PBowSim& BowSim, double& Qual, TFltV& ClustQualV);
  static PBowDocPart GetKMeansPartForDocWgtBs(
   const PNotify& Notify,
   const PBowDocWgtBs& BowDocWgtBs,
   const PBowDocBs& BowDocBs, const PBowSim& BowSim, TRnd& Rnd,
   const int& Clusts, const int& ClustTrials,
   const double& ConvergEps, const int& MnDocsPerClust,
   const TIntFltPrV& DocIdWgtPrV=TIntFltPrV());
  static PBowDocPart GetHPartForDocWgtBs(
   const PNotify& Notify,
   const PBowDocWgtBs& BowDocWgtBs,
   const PBowDocBs& BowDocBs, const PBowSim& BowSim, TRnd& Rnd);

  // clustering on BowDocBs
  static PBowDocPart GetKMeansPart(
   const PNotify& Notify,
   const PBowDocBs& BowDocBs, const PBowSim& BowSim, TRnd& Rnd,
   const int& Clusts, const int& ClustTrials,
   const double& ConvergEps, const int& MnDocsPerClust,
   const TBowWordWgtType& WordWgtType, const double& CutWordWgtSumPrc,
   const int& MnWordFq, const TIntV& DIdV=TIntV());
  static PBowDocPart GetHKMeansPart(
   const PNotify& Notify,
   const PBowDocBs& BowDocBs, const PBowSim& BowSim, TRnd& Rnd,
   const int& MxDocsPerLeafClust, const int& ClustTrials,
   const double& ConvergEps, const int& MnDocsPerClust,
   const TBowWordWgtType& WordWgtType, const double& CutWordWgtSumPrc,
   const int& MnWordFq, const TIntV& DIdV=TIntV(),
   const bool& PropBowDocWgtBsP=false, const PBowDocWgtBs& BowDocWgtBs=NULL);
  static PBowDocPart GetHPart(
   const PNotify& Notify,
   const PBowDocBs& BowDocBs, const PBowSim& BowSim, TRnd& Rnd,
   const TBowWordWgtType& WordWgtType, const double& CutWordWgtSumPrc,
   const int& MnWordFq, const TIntV& DIdV=TIntV());

  // clustering based classification
  static double GetNNbrCfAcc(
   const PNotify& Notify,
   const PBowDocBs& BowDocBs, const PBowSim& BowSim,
   const TBowWordWgtType& WordWgtType, const int& KNbrs,
   const TIntV& TrainDIdV, const TIntV& TestDIdV);
  static double GetClustCfAcc(
   const PNotify& Notify,
   const PBowDocBs& BowDocBs, const PBowDocPart& Part, const PBowSim& BowSim,
   const TBowWordWgtType& WordWgtType,
   const TIntV& TrainDIdV, const TIntV& TestDIdV);
};

