/////////////////////////////////////////////////
// Url
typedef enum {usUndef, usHttp, usOther} TUrlScheme;

ClassTPV(TUrl, PUrl, TUrlV)//{
private:
  static const TStr UrlHttpPrefixStr;
  static const TStr UrlHttpAbsPrefixStr;
  TUrlScheme Scheme;
  TStr UrlStr, RelUrlStr, BaseUrlStr;
  TStr SchemeNm, HostNm;
  TStr PortStr, PathStr, SearchStr, FragIdStr;
  int PortN;
  TStrV PathSegV;
  TStr IpNum;
  TStr FinalUrlStr, FinalHostNm;
  TStr HttpRqStr;
  void GetAbs(const TStr& AbsUrlStr);
  void GetAbsFromBase(const TStr& RelUrlStr, const TStr& BaseUrlStr);
  UndefDefaultCopyAssign(TUrl);
public:
  TUrl(const TStr& _RelUrlStr, const TStr& _BaseUrlStr=TStr());
  static PUrl New(const TStr& RelUrlStr, const TStr& BaseUrlStr=TStr()){
    return PUrl(new TUrl(RelUrlStr, BaseUrlStr));}
  ~TUrl(){}
  TUrl(TSIn&){Fail;}
  static PUrl Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  bool IsOk(const TUrlScheme _Scheme=usUndef) const {
    if (_Scheme==usUndef){return Scheme!=usUndef;}
    else {return Scheme==_Scheme;}}
  TUrlScheme GetScheme(){return Scheme;}
  TStr GetUrlStr() const {return UrlStr;}
  TStr GetRelUrlStr() const {return RelUrlStr;}
  bool IsBaseUrl(){return !BaseUrlStr.Empty();}
  TStr GetBaseUrlStr() const {return BaseUrlStr;}
  TStr GetSchemeNm() const {IAssert(IsOk()); return SchemeNm;}
  TStr GetHostNm() const {IAssert(IsOk()); return HostNm;}
  TStr GetDmNm(const int& MxDmSegs=-1) const;
  TStr GetPortStr() const {IAssert(IsOk()); return PortStr;}
  int GetPortN() const {IAssert(IsOk()&&(PortN!=-1)); return PortN;}
  TStr GetPathStr() const {IAssert(IsOk()); return PathStr;}
  int GetPathSegs() const {return PathSegV.Len();}
  TStr GetPathSeg(const int& PathSegN) const {return PathSegV[PathSegN];}
  TStr GetSearchStr() const {IAssert(IsOk()); return SearchStr;}
  TStr GetFragIdStr() const {IAssert(IsOk()); return FragIdStr;}

  bool IsIpNum() const {return !IpNum.Empty();}
  void PutIpNum(const TStr& _IpNum){IpNum=_IpNum;}
  TStr GetIpNum() const {IAssert(IsIpNum()); return IpNum;}
  TStr GetIpNumOrHostNm() const {return IsIpNum() ? GetIpNum() : GetHostNm();}

  bool IsDefFinalUrl() const {
    IAssert(IsOk(usHttp)); return !FinalUrlStr.Empty();}
  TStr GetFinalUrlStr() const {
    IAssert(IsDefFinalUrl()); return FinalUrlStr;}
  TStr GetAsFinalUrlStr() const {
    if (IsDefFinalUrl()){return FinalUrlStr;} else {return UrlStr;}}
  TStr GetFinalHostNm() const {
    IAssert(IsDefFinalUrl()); return FinalHostNm;}
  TStr GetAsFinalHostNm() const {
    if (IsDefFinalUrl()){return FinalHostNm;} else {return HostNm;}}
  void DefUrlAsFinal(){
    IAssert(IsOk(usHttp)); IAssert(!IsDefFinalUrl());
    FinalUrlStr=UrlStr; FinalHostNm=HostNm;}
  void DefFinalUrl(const TStr& _FinalHostNm);

  void PutHttpRqStr(const TStr& _HttpRqStr){HttpRqStr=_HttpRqStr;}
  TStr GetHttpRqStr() const {return HttpRqStr;}
  bool IsHttpRqStr() const {return !HttpRqStr.Empty();}
  void ChangeHttpRqStr(const TStr& SrcStr, const TStr& DstStr){
    HttpRqStr.ChangeStr(SrcStr, DstStr);}

  bool IsInHost(const TStr& _HostNm) const {
    IAssert(IsOk()); return HostNm.GetUc().IsSuffix(_HostNm.GetUc());}
  bool IsInPath(const TStr& _PathStr) const {
    IAssert(IsOk()); return PathStr.GetUc().IsPrefix(_PathStr.GetUc());}
  void ToLcPath();

  static bool IsAbs(const TStr& UrlStr);
  static bool IsScript(const TStr& UrlStr);
  static bool IsSite(const TStr& UrlStr);

  static PUrl GetUrlFromShortcut(const TStr& ShortcutUrlStr,
   const TStr& DfHostNmPrefix, const TStr& DfHostNmSufix);
  static TStr GetUrlSearchStr(const TStr& Str);
  static TStr GetDocStrFromUrlStr(const TStr& UrlStr, const int& Copies=1);
  static TStr GetTopDownDocNm(
   const TStr& UrlStr, const int& MxLen=-1, const bool& HostOnlyP=false);
};
typedef TPair<TInt, PUrl> TIdUrlPr;
typedef TQQueue<TIdUrlPr> TIdUrlPrQ;
typedef THash<TInt, PUrl> TIdToUrlH;

/////////////////////////////////////////////////
// Url-Environment
ClassTP(TUrlEnv, PUrlEnv)//{
private:
  TStr BaseUrlStr;
  TStrV KeyNmV;
  TStrStrVH KeyNmToValH;
public:
  TUrlEnv():
    KeyNmV(), KeyNmToValH(10){}
  TUrlEnv(const TUrlEnv& UrlEnv):
    KeyNmV(UrlEnv.KeyNmV), KeyNmToValH(UrlEnv.KeyNmToValH){}
  static PUrlEnv New(){return new TUrlEnv();}
  static PUrlEnv New(const TStr& BaseUrlStr,
   const TStr& KeyNm1=TStr(), const TStr& ValStr1=TStr(),
   const TStr& KeyNm2=TStr(), const TStr& ValStr2=TStr(),
   const TStr& KeyNm3=TStr(), const TStr& ValStr3=TStr(),
   const TStr& KeyNm4=TStr(), const TStr& ValStr4=TStr()){
    PUrlEnv UrlEnv=New();
    UrlEnv->PutBaseUrlStr(BaseUrlStr);
    if (!KeyNm1.Empty()){UrlEnv->AddKeyVal(KeyNm1, ValStr1);}
    if (!KeyNm2.Empty()){UrlEnv->AddKeyVal(KeyNm2, ValStr2);}
    if (!KeyNm3.Empty()){UrlEnv->AddKeyVal(KeyNm3, ValStr3);}
    if (!KeyNm4.Empty()){UrlEnv->AddKeyVal(KeyNm4, ValStr4);}
    return UrlEnv;}
  ~TUrlEnv(){}
  TUrlEnv(TSIn& SIn): KeyNmV(SIn), KeyNmToValH(SIn){}
  static PUrlEnv Load(TSIn& SIn){return new TUrlEnv(SIn);}
  void Save(TSOut& SOut){KeyNmV.Save(SOut); KeyNmToValH.Save(SOut);}

  TUrlEnv& operator=(const TUrlEnv& Env){
    if (this!=&Env){KeyNmV=Env.KeyNmV; KeyNmToValH=Env.KeyNmToValH;}
    return *this;}

  // base url
  void PutBaseUrlStr(const TStr& _BaseUrlStr){BaseUrlStr=_BaseUrlStr;}
  TStr GetBaseUrlStr() const {return BaseUrlStr;}

  // adding key-value
  void AddKeyVal(const TStr& KeyNm, const TStr& ValStr){
    if (!IsKey(KeyNm)){KeyNmV.Add(KeyNm); KeyNmToValH.AddKey(KeyNm);}
    KeyNmToValH.GetDat(KeyNm).Clr();
    KeyNmToValH.GetDat(KeyNm).Add(ValStr);}
  void AddToKeyVal(const TStr& KeyNm, const TStr& ValStr){
    if (!IsKey(KeyNm)){KeyNmV.Add(KeyNm); KeyNmToValH.AddKey(KeyNm);}
    KeyNmToValH.GetDat(KeyNm).Add(ValStr);}

  // key retrieval
  bool Empty() const {return KeyNmV.Empty();}
  int GetKeys() const {return KeyNmV.Len();}
  bool IsKey(const TStr& KeyNm) const {return KeyNmV.SearchForw(KeyNm)!=-1;}
  int GetKeyN(const TStr& KeyNm) const {return KeyNmV.SearchForw(KeyNm);}
  TStr GetKeyNm(const int& KeyN) const {return KeyNmV[KeyN];}

  // value retrieval
  int GetVals(const int& KeyN) const {
    return KeyNmToValH.GetDat(KeyNmV[KeyN]).Len();}
  int GetVals(const TStr& KeyNm) const {
    return KeyNmToValH.GetDat(KeyNm).Len();}
  TStr GetVal(const int& KeyN, const int& ValN=0) const {
    return KeyNmToValH.GetDat(KeyNmV[KeyN])[ValN];}
  TStr GetVal(const TStr& KeyNm, const int& ValN=0, const TStr& DfVal="") const {
    if (KeyNmToValH.IsKey(KeyNm)){
      return KeyNmToValH.GetDat(KeyNm)[ValN];}
    else {return DfVal;}}

  // full-url-string
  TStr GetFullUrlStr() const;

  static PUrlEnv MkClone(const PUrlEnv& UrlEnv);
};

