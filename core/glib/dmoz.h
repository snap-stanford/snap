/////////////////////////////////////////////////
// DMoz-Info
class TDMozInfo{
public:
  // RDF files
  static const TStr RdfStructFBase;
  static const TStr RdfContentFBase;
  static const TStr RdfStructSampleFBase;
  static const TStr RdfContentSampleFBase;

  // binary files
  static const TStr BinStructFBase;
  static const TStr BinFullFBase;
  static const TStr BinStructSampleFBase;
  static const TStr BinFullSampleFBase;
};

/////////////////////////////////////////////////
// DMoz-Topic
class TDMozTopic{
public:
  TInt CatSId;
  TInt SubCatTypeIdVN;
  TInt ExtUrlIdVN;
public:
  TDMozTopic():
    CatSId(-1), SubCatTypeIdVN(-1), ExtUrlIdVN(-1){}
  TDMozTopic(const TDMozTopic&){Fail;}
  TDMozTopic(TSIn& SIn):
    CatSId(SIn), SubCatTypeIdVN(SIn), ExtUrlIdVN(SIn){}
  void Save(TSOut& SOut) const {
    CatSId.Save(SOut); SubCatTypeIdVN.Save(SOut); ExtUrlIdVN.Save(SOut);}

  TDMozTopic& operator=(const TDMozTopic& Topic){
    CatSId=Topic.CatSId; SubCatTypeIdVN=Topic.SubCatTypeIdVN;
    ExtUrlIdVN=Topic.ExtUrlIdVN; return *this;}
};

/////////////////////////////////////////////////
// DMoz-Alias
class TDMozAlias{
public:
  TInt SymSId;
  TInt SubstSId;
public:
  TDMozAlias(): SymSId(-1), SubstSId(-1){}
  TDMozAlias(const TDMozAlias&){Fail;}

  TDMozAlias& operator=(const TDMozAlias& Alias){
    SymSId=Alias.SymSId; SubstSId=Alias.SubstSId; return *this;}
};

/////////////////////////////////////////////////
// DMoz-Category-Type
typedef enum {dmsctUndef,
 dmsctNarrow, dmsctLetterbar, dmsctRelated, dmsctSymbolic} TDMozSubCatType;

/////////////////////////////////////////////////
// DMoz-Base
ClassTP(TDMozBs, PDMozBs)//{
private:
  TSStrPool CatNmPool; // category-names pool
  THash<TMd5Sig, TDMozTopic> CatMd5ToTopicH; // category name to topic desc.
  TUIntV SubCatTypeIdPrV; // sub-category (type,id) pair vectors pool
  TIntV ExtUrlIdV; // external url-id vectors pool
  TSStrPool ExtPgStrPool; // external-page-string pool
  THash<TMd5Sig, TIntTr> ExtUrlMd5ToUrlTitleDescSIdTrH;
   // external url to title/description string-id pair
  UndefDefaultCopyAssign(TDMozBs);
public:
  TDMozBs(const int& ExtPgStrPoolLen):
    CatNmPool(), CatMd5ToTopicH(), SubCatTypeIdPrV(),
    ExtUrlIdV(), ExtPgStrPool(ExtPgStrPoolLen), ExtUrlMd5ToUrlTitleDescSIdTrH(){}
  static PDMozBs New(const int& ExtPgStrPoolLen){
    return new TDMozBs(ExtPgStrPoolLen);}
  TDMozBs(TSIn& SIn):
    CatNmPool(SIn), CatMd5ToTopicH(SIn), SubCatTypeIdPrV(SIn),
    ExtUrlIdV(SIn), ExtPgStrPool(SIn), ExtUrlMd5ToUrlTitleDescSIdTrH(SIn){}
  static PDMozBs Load(TSIn& SIn){return new TDMozBs(SIn);}
  void Save(TSOut& SOut) const {
    CatNmPool.Save(SOut); CatMd5ToTopicH.Save(SOut); SubCatTypeIdPrV.Save(SOut);
    ExtUrlIdV.Save(SOut);
    ExtPgStrPool.Save(SOut);
    ExtUrlMd5ToUrlTitleDescSIdTrH.Save(SOut);}

  // categories
  int GetCats() const {return CatMd5ToTopicH.Len();}
  TStr GetCatNm(const int& CatId) const {
    return CatNmPool.GetStr(CatMd5ToTopicH[CatId].CatSId);}
  int GetCatId(const TStr& CatNm) const {
    TMd5Sig CatMd5(CatNm); return CatMd5ToTopicH.GetKeyId(CatMd5);}
  void GetCatIdV(const TStrV& CatNmV, TIntV& CatIdV) const;
  void GetSubCatTypeIdPrV(const int& CatId, TUIntV& _SubCatTypeIdPrV) const;
  void GetSubCatIdV(const int& CatId, TIntV& SubCatIdV, const bool& SubTreeOnlyP=true) const;
  void GetExtUrlIdV(const int& CatId, TIntV& _ExtUrlIdV, const bool& AppendP=false) const;
  void GetExtUrlIdV(const TIntV& CatIdV, TIntV& _ExtUrlIdV, const bool& AppendP=false) const;

  // external urls
  int GetExtUrls() const {
    return ExtUrlMd5ToUrlTitleDescSIdTrH.Len();}
  int GetRndExtUrlId(const int& RootCatId, TRnd& Rnd);
  TStr GetExtUrlMd5Str(const int& ExtUrlId) const {
    return ExtUrlMd5ToUrlTitleDescSIdTrH.GetKey(ExtUrlId).GetStr();}
  TStr GetExtUrlStr(const int& ExtUrlId) const {
    return ExtPgStrPool.GetStr(ExtUrlMd5ToUrlTitleDescSIdTrH[ExtUrlId].Val1);}
  TStr GetExtUrlTitleStr(const int& ExtUrlId) const {
    return ExtPgStrPool.GetStr(ExtUrlMd5ToUrlTitleDescSIdTrH[ExtUrlId].Val2);}
  TStr GetExtUrlDescStr(const int& ExtUrlId) const {
    return ExtPgStrPool.GetStr(ExtUrlMd5ToUrlTitleDescSIdTrH[ExtUrlId].Val3);}

  // sub-trees
  void GetSubTreeCatIdV(
   const int& RootCatId, const TIntV& PosCatIdV, const TIntV& NegCatIdV, 
   TIntV& CatIdV, const bool& SubTreeOnlyP=true) const;
  void GetSubTreeCatIdV(
   const TStr& RootCatNm, const TStrV& PosCatNmV, const TStrV& NegCatNmV, 
   TIntV& CatIdV, const bool& SubTreeOnlyP=true) const;
  void GetSubTreeDocV(
   const TStr& RootCatNm, const TStrV& PosCatNmV, const TStrV& NegCatNmV, 
   TStrV& DocNmV, TStrV& DocStrV,
   const bool& SubTreeOnlyP=true, const int& UrlWordsCopies=1) const;

  // bag-of-words
  PBowDocPartClust GetBowDocPartClust(
   const int& RootCatId, const TIntV& PosCatIdV, const TIntV& NegCatIdV,
   const PBowDocBs& BowDocBs, const PBowDocWgtBs& BowDocWgtBs, const PBowSim& BowSim,
   const int& MnCatDocs, const int& Cats, int& CatN);
  PBowDocPart _GetBowDocPart(
   const int& RootCatId, const TIntV& PosCatIdV, const TIntV& NegCatIdV, 
   const PBowDocBs& BowDocBs, const PBowDocWgtBs& BowDocWgtBs, const PBowSim& BowSim,
   const int& MnCatDocs, const int& Cats, int& CatN);
  PBowDocPart GetBowDocPart(
   const TStr& RootCatNm, const TStrV& PosCatNmV, const TStrV& NegCatNmV,
   const PBowDocBs& BowDocBs, const PBowDocWgtBs& BowDocWgtBs, const PBowSim& BowSim,
   const int& MnCatDocs);
  static void _GetBestClustV(
   const PBowDocPart& BowDocPart, const PBowSim& BowSim,
   const PBowSpV DocBowSpV, TFltBowDocPartClustKdV& WgtClustKdV);
  static void _GetBestWordVV(
   TFltBowDocPartClustKdV& WgtClustKdV, const PBowSim& BowSim,
   const PBowSpV DocBowSpV, TVec<TFltIntPrV>& WgtWIdPrVV);
  static void GetBestClustV(
   const PBowDocPart& BowDocPart, const PBowSim& BowSim,
   const PBowSpV DocBowSpV, const int& MxCats,
   TFltBowDocPartClustKdV& WgtClustKdV, TVec<TFltIntPrV>& WgtWIdPrVV);
  static void GetBestKWordV(
   const TFltBowDocPartClustKdV& WgtClustKdV, const double& TopWgtSumPrc, 
   const bool& AddLevP, const int& MnLev, const int& MxLev, 
   TStrFltPrV& KWordStrWgtPrV);

  // files
  static PDMozBs LoadTxt(const TStr& FPath,
   const bool& StructOnlyP=false, const bool& SampleDataP=false,
   const int& ExtPgStrPoolLen=800000000);
  static PDMozBs LoadBin(const TStr& FBase, const TStr& FPath=TStr());
  void SaveBin(const TStr& FBase, const TStr& FPath=TStr());
  static void SaveTxt(const PDMozBs& DMozBs, const TStr& RootCatNm,
   const TStr& FBase, const TStr& FPath);
  static void SaveLnDocTxt(const PDMozBs& DMozBs, const TStr& RootCatNm,
   const TStr& FBase, const TStr& FPath, const bool& SaveMd5P);
  static void SaveSampleTxt(const PDMozBs& DMozBs, const TStr& RootCatNm,
   const TStr& FBase, const TStr& FPath,
   const int& SampleRndSeed, const int& SampleSize,
   const bool& SampleSiteUrlP, const bool& SampleStructP);

  // category type
  static TDMozSubCatType GetSubCatType(const TStr& CatTypeNm);
  static TStr GetSubCatTypeNm(const TDMozSubCatType& SubCatType);
  static bool IsSubTreeSubCatType(const TDMozSubCatType& SubCatType){
    return (SubCatType==dmsctNarrow)/*||(SubCatType==dmsctLetterbar)*/;}

  // category type & category id pair
  static uint GetSubCatTypeIdPr(
   const TDMozSubCatType& SubCatType, const int& CatId=0);
  static void GetSubCatTypeCatId(
   const uint& SubCatTypeIdPr, TDMozSubCatType& SubCatType, int& CatId);
};

