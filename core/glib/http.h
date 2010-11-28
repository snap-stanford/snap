/////////////////////////////////////////////////
// Http-General
class THttp{
public:
  // general strings
  static const TStr HttpStr;
  static const TStr SlashStr;
  static const TStr ColonStr;
  // field names
  static const TStr ContTypeFldNm;
  static const TStr ContLenFldNm;
  static const TStr HostFldNm;
  static const TStr AcceptRangesFldNm;
  static const TStr CacheCtrlFldNm;
  static const TStr AcceptFldNm;
  static const TStr SrvFldNm;
  static const TStr ConnFldNm;
  static const TStr FetchIdFldNm;
  static const TStr LocFldNm;
  static const TStr SetCookieFldNm;
  static const TStr CookieFldNm;
  // content-type field-values
  static const TStr TextFldVal;
  static const TStr TextPlainFldVal;
  static const TStr TextHtmlFldVal;
  static const TStr TextXmlFldVal;
  static const TStr TextWmlFldVal;
  static const TStr ImageGifFldVal;
  static const TStr AppOctetFldVal;
  static const TStr AppSoapXmlFldVal;
  static const TStr AppW3FormFldVal;
  static const TStr ConnKeepAliveFldVal;
  // file extensions
  static bool IsHtmlFExt(const TStr& FExt);
  static bool IsGifFExt(const TStr& FExt);

  // port number
  static const int DfPortN;
  // status codes
  static const int OkStatusCd;
  static const int RedirStatusCd;
  static const int BadRqStatusCd;
  static const int ErrStatusCd;
  static const int ErrNotFoundStatusCd;
  static TStr GetReasonPhrase(const int& StatusCd);
  // method names
  static const TStr GetMethodNm;
  static const TStr HeadMethodNm;
  static const TStr PostMethodNm;
};

/////////////////////////////////////////////////
// Http-Request
typedef enum {
  hrmUndef, hrmGet, hrmHead, hrmPost} THttpRqMethod;

ClassTP(THttpRq, PHttpRq)//{
private:
  bool Ok;
  bool CompleteP;
  int MajorVerN, MinorVerN;
  THttpRqMethod Method;
  PUrl Url;
  TStrStrH FldNmToValH;
  PUrlEnv UrlEnv;
  TStr HdStr;
  TMem BodyMem;
  void ParseSearch(const TStr& SearchStr);
  void ParseHttpRq(const PSIn& SIn);
public:
  THttpRq(const PSIn& SIn);
  static PHttpRq New(const PSIn& SIn){
    return PHttpRq(new THttpRq(SIn));}
  THttpRq(
   const THttpRqMethod& _Method, const PUrl& _Url,
   const TStr& ContTypeFldVal, const TMem& _BodyMem, const int& FetchId=-1);
  static PHttpRq New(
   const THttpRqMethod& Method, const PUrl& Url,
   const TStr& ContTypeFldVal, const TMem& BodyMem){
    return PHttpRq(new THttpRq(Method, Url, ContTypeFldVal, BodyMem));}
  static PHttpRq New(const PUrl& Url, const int& FetchId=-1){
    return PHttpRq(new THttpRq(hrmGet, Url, "", TMem(), FetchId));}
  ~THttpRq(){}
  THttpRq(TSIn&){Fail;}
  static PHttpRq Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  THttpRq& operator=(const THttpRq&){Fail; return *this;}

  // component-retrieval
  bool IsOk() const {return Ok;}
  bool IsComplete() const {return CompleteP;}
  THttpRqMethod GetMethod() const {return Method;}
  TStr GetMethodNm() const;
  PUrl GetUrl() const {return Url;}
  PUrlEnv GetUrlEnv() const {return UrlEnv;}
  bool IsFldNm(const TStr& FldNm) const;
  TStr GetFldVal(const TStr& FldNm) const;
  bool IsFldVal(const TStr& FldNm, const TStr& FldVal) const;

  // header & body
  TStr GetHdStr() const {return HdStr;}
  TStr GetBodyAsStr() const {
    return BodyMem.GetAsStr(' ');}
  void GetAsMem(TMem& Mem) const {
    Mem.Clr(); Mem+=HdStr; Mem+=BodyMem;}

  // content-type
  bool IsContType(const TStr& ContTypeStr) const {
    return GetFldVal(THttp::ContTypeFldNm).IsStrIn(ContTypeStr);}
  bool IsContLen(int& ContLen) const {
    return GetFldVal(THttp::ContLenFldNm).IsInt(ContLen);}

  // string representation
  TStr GetStr() const;
};

/////////////////////////////////////////////////
// Http-Response
ClassTP(THttpResp, PHttpResp)//{
private:
  bool Ok;
  int MajorVerN, MinorVerN;
  int StatusCd;
  TStr ReasonPhrase;
  TStrStrVH FldNmToValVH;
  TStr HdStr;
  TMem BodyMem;
  void AddHdFld(const TStr& FldNm, const TStr& FldVal, TChA& HdChA);
  void ParseHttpResp(const PSIn& SIn);
public:
  THttpResp(const int& _StatusCd, const TStr& ContTypeVal,
   const bool& CacheCtrlP, const PSIn& BodySIn, const TStr LocStr);
  static PHttpResp New(const int& StatusCd, const TStr& ContTypeVal,
   const bool& CacheCtrlP, const PSIn& BodySIn, const TStr LocStr=TStr()){
    return PHttpResp(new
     THttpResp(StatusCd, ContTypeVal, CacheCtrlP, BodySIn, LocStr));}
  THttpResp(const PSIn& SIn);
  static PHttpResp New(const PSIn& SIn){
    return PHttpResp(new THttpResp(SIn));}
  ~THttpResp(){}
  THttpResp(TSIn&){Fail;}
  static PHttpResp Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  THttpResp& operator=(const THttpResp&){Fail; return *this;}

  bool IsOk() const {return Ok;}
  int Len() const {return HdStr.Len()+BodyMem.Len();}
  bool IsContLenOk() const {int ContLen;
    return IsOk()&&IsContLen(ContLen)&&(ContLen==BodyMem.Len());}
  void GetAsMem(TMem& Mem) const {
    Mem.Clr(); Mem+=HdStr; Mem+=BodyMem;}
  TStr GetHdStr() const {return HdStr;}
  TMem GetBodyAsMem() const {
    return BodyMem;}
  TStr GetBodyAsStr() const {
    return BodyMem.GetAsStr(' ');}
  int GetStatusCd() const {return StatusCd;}
  TStr GetReasonPhrase() const {return THttp::GetReasonPhrase(StatusCd);}
  int GetFlds() const {return FldNmToValVH.Len();}
  int GetFldVals(const int& FldN) const {
    return FldNmToValVH[FldN].Len();}
  void GetFldNmVal(const int& FldN, TStr& FldNm, TStr& FldVal){
    FldNm=FldNmToValVH.GetKey(FldN); FldVal=FldNmToValVH[FldN][0];}
  void GetFldNmVal(const int& FldN, const int& ValN, TStr& FldNm, TStr& FldVal){
    FldNm=FldNmToValVH.GetKey(FldN); FldVal=FldNmToValVH[FldN][ValN];}
  bool IsFldNm(const TStr& FldNm) const;
  TStr GetFldVal(const TStr& FldNm, const int& ValN=0) const;
  void GetFldValV(const TStr& FldNm, TStrV& FldValV) const;
  bool IsFldVal(const TStr& FldNm, const TStr& FldVal) const;
  void AddFldVal(const TStr& FldNm, const TStr& FldVal);

  bool IsStatusCd_Ok() const {
    return IsOk() && (GetStatusCd()/100==THttp::OkStatusCd/100);}
  bool IsStatusCd_Redir() const {
    return IsOk() && (GetStatusCd()/100==THttp::RedirStatusCd/100);}

  bool IsContType() const {
    return IsFldNm(THttp::ContTypeFldNm);}
  bool IsContType(const TStr& ContTypeStr) const {
    return GetFldVal(THttp::ContTypeFldNm).IsStrIn(ContTypeStr);}
  bool IsContLen(int& ContLen) const {
    return GetFldVal(THttp::ContLenFldNm).IsInt(ContLen);}
  TStr GetSrvNm() const {
    return GetFldVal(THttp::SrvFldNm);}
  void GetCookieKeyValDmPathQuV(TStrQuV& CookieKeyValDmPathQuV);

  int GetTxtLen() const {return HdStr.Len()+BodyMem.Len();}
  static PHttpResp LoadTxt(PSIn& SIn){
    return new THttpResp(SIn);}
  void SaveTxt(const PSOut& SOut) const {
    HdStr.SaveTxt(SOut); BodyMem.SaveMem(SOut);}
  void SaveBody(const PSOut& SOut) const {
    BodyMem.SaveMem(SOut);}

  PSIn GetSIn() const;
};

