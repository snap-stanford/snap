/////////////////////////////////////////////////
// Forward
ClassHdTP(TTxtBs, PTxtBs);

/////////////////////////////////////////////////
// Text-Base-Results
class TTxtBsResBPt: public TBlobPt{
public:
  TTxtBsResBPt& operator=(const TBlobPt& Pt){
    TBlobPt::operator=(Pt); return *this;}
  bool operator==(const TBlobPt& Pt) const {
    return (FSet1==Pt.FSet1)&&(FSet2==Pt.FSet2)&&(FSet3==Pt.FSet3);}
  bool operator<(const TTxtBsResBPt& Pt) const {
    if ((FSet1==Pt.FSet1)&&(FSet2==Pt.FSet2)&&(FSet3==Pt.FSet3)){
      return (Seg<Pt.Seg)||((Seg==Pt.Seg)&&(Addr<Pt.Addr));
    } else {
      return
       (FSet1<Pt.FSet1)||
       ((FSet1==Pt.FSet1)&&(FSet2<Pt.FSet2))||
       ((FSet1==Pt.FSet1)&&(FSet2==Pt.FSet2)&&(FSet3<Pt.FSet3));
    }
  }
};
typedef TVec<TTxtBsResBPt> TTxtBsResBPtV;

/////////////////////////////////////////////////
// Text-Base-Results
ClassTP(TTxtBsRes, PTxtBsRes)//{
private:
  bool Ok;
  TStr ErrMsg;
  PTxtBs TxtBs;
  TStr WixExpStr;
  TTxtBsResBPtV DocIdV;
public:
  TTxtBsRes(
   const PTxtBs& _TxtBs, const TStr& _WixExpStr, const TBlobPtV& _DocIdV);
  TTxtBsRes(const TStr& _ErrMsg):
    Ok(false), ErrMsg(_ErrMsg), TxtBs(), WixExpStr(), DocIdV(){}
  ~TTxtBsRes(){}
  TTxtBsRes(TSIn&){Fail;}
  static PTxtBsRes Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TTxtBsRes& operator=(const TTxtBsRes&);

  bool IsOk() const {return Ok;}
  TStr GetErrMsg() const {return ErrMsg;}
  PTxtBs GetTxtBs() const {return TxtBs;}
  TStr GetWixExpStr() const {return WixExpStr;}
  int GetDocs() const {return DocIdV.Len();}
  TBlobPt GetDocId(const int& DocIdN) const {return DocIdV[DocIdN];}
  void GetDocInfo(
   const int& DocIdN, const int& MxDocTitleLen, const int& MxDocCtxLen,
   TStr& DocNm, TStr& DocTitleStr, TStr& DocStr, TStr& DocCtxStr) const;

  void GetHitSetMnMxDocN(
   const int& HitSetN, const int& HitSetDocs, int& MnDocN, int& MxDocN) const;
  void GetHitSet(
   const int& HitSetN, const int& HitSetDocs, const int& TocHitSets,
   const TStr& HitSetUrlFldNm, const PUrlEnv& UrlEnv,
   TStr& PrevTocUrlStr, TStrPrV& NmTocUrlStrPrV, TStr& NextTocUrlStr) const;

  friend class TTxtBs;
};

/////////////////////////////////////////////////
// Text-Base-Document-Fixed-Data
class TTxtBsDocFDat{
private:
  TBlobPt MainNmId;
  TSecTm Tm;
  TInt Len;
  TBlobPt DocId;
public:
  TTxtBsDocFDat(){}
  TTxtBsDocFDat(
   const TBlobPt& _MainNmId, const TSecTm& _Tm,
   const TInt& _Len, const TBlobPt& _DocId):
    MainNmId(_MainNmId), Tm(_Tm), Len(_Len), DocId(_DocId){}
  TTxtBsDocFDat(TSIn& SIn):
    MainNmId(SIn), Tm(SIn), Len(SIn), DocId(SIn){}
  void Save(TSOut& SOut) const {
    MainNmId.Save(SOut); Tm.Save(SOut); Len.Save(SOut); DocId.Save(SOut);}

  //TTxtBsDocFDat operator=(const TTxtBsDocFDat& FDat) const {
  //  if (this!=&FDat){
  //    MainNmId=FDat.MainNmId; Tm=FDat.Tm; Len=FDat.Len; DocId=FDat.DocId;}
  //  return *this;}
  int GetMemUsed() const {
    return MainNmId.GetMemUsed()+Tm.GetMemUsed()+
     Len.GetMemUsed()+DocId.GetMemUsed();}

  friend class TTxtBs;
};

/////////////////////////////////////////////////
// Text-Base-Document-Variable-Data
class TTxtBsDocVDat{
public:
  TTxtBsDocVDat(){}
  ~TTxtBsDocVDat(){}

  TTxtBsDocVDat operator=(const TTxtBsDocVDat& VDat){
    if (this!=&VDat){}
    return *this;}

  friend class TTxtBs;
};

/////////////////////////////////////////////////
// Text-Base
ClassTP(TTxtBs, PTxtBs)//{
public:
  static const TStr TxtBsFExt;
  static const TStr TxtBsDocNmFExt;
  static const TStr TxtBsDocDatFExt;
private:
  typedef TFHash<TStr, TTxtBsDocFDat, TTxtBsDocVDat> TDocNmH;
  typedef TPt<TDocNmH> PDocNmH;
  TStr NrNm;
  TStr NrFPath;
  TStr TxtBsFNm;
  TFAccess Access;
  TFAccess WixAccess;
  PWix Wix;
  PDocNmH DocNmH;
  PBlobBs DocBBs;
  static void GenTxtBsMainFile(const TStr& Nm, const TStr& FPath);
  static void AddToTxtBsMainFile(const TStr& TxtBsFNm, const TStr& MsgStr);
public:
  TTxtBs(const TStr& Nm, const TStr& FPath=TStr(),
   const TFAccess& _Access=faRdOnly, const TFAccess& _WixAccess=faUndef,
   const int& CacheSize=100000000);
  static PTxtBs New(const TStr& Nm, const TStr& FPath=TStr(),
   const TFAccess& Access=faRdOnly, const TFAccess& WixAccess=faUndef,
   const int& CacheSize=100000000){
    return new TTxtBs(Nm, FPath, Access, WixAccess, CacheSize);}
  ~TTxtBs();
  TTxtBs(TSIn&){Fail;}
  static PTxtBs Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TTxtBs& operator=(const TTxtBs&){Fail; return *this;}
  int64 GetMemUsed(){
    return Wix->GetMemUsed()+int64(DocNmH->GetMemUsed());}
  TStr GetMemUsedStr();
  void CacheFlushAndClr(){
    Wix->CacheFlushAndClr(); DocNmH->CacheFlushAndClr();}

  TStr GetNrNm() const {return NrNm;}
  TStr GetNrFPath() const {return NrFPath;}

  TBlobPt AddDocMem(const TStr& UrlStr, const TMem& DocMem, const bool& IndexP=true);
  TBlobPt AddHtmlDoc(const TStr& UrlStr, const PSIn& SIn, const bool& IndexP=true);
  void GetDocNmStr(const TBlobPt& BlobPt, TStr& DocNm, TStr& DocStr);
  void GetDocNmMem(const TBlobPt& BlobPt, TStr& DocNm, TMem& DocMem);
  TStr GetDocNm(const TBlobPt& BlobPt);
  TStr GetDocStr(const TStr& DocNm);
  void GetDocMem(const TStr& DocNm, TMem& DocMem);

  bool IsDoc(const TStr& Nm) const {return DocNmH->IsKey(Nm);}
  int GetDocs() const {return DocNmH->Len();}
  TBlobPt FFirstDocId();
  bool FNextDocId(TBlobPt& TrvBlobPt, TBlobPt& DocId);

  PTxtBsRes Search(const TStr& WixExpStr);

  static void GenIndex(const TStr& Nm, const TStr& FPath, const bool& MemStatP);

  void SaveTxt(const TStr& FNm){
    SaveTxt(TFOut::New(FNm));}
  void SaveTxt(const PSOut& SOut);
  void SaveTxtWordFq(const TStr& FNm){
    Wix->SaveTxtWordFq(TFOut::New(FNm));}

  static TStr GetFExt(){return TxtBsFExt;}
  static void GetFNms(
   const TStr& Nm, const TStr& FPath,
   TStr& TxtBsFNm, TStr& TxtBsDocNmFNm, TStr& TxtBsDocDatFNm);
  static bool Exists(const TStr& Nm, const TStr& FPath=TStr());
  static void Del(const TStr& Nm, const TStr& FPath=TStr());
};

