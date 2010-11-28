/////////////////////////////////////////////////
// Stop-Word-Set

// reference
// The stoplist is the SMART system's list of 524 common words, like "the" and "of".)

typedef enum {swstNone,
 swstEn8, swstEn425, swstEn523, swstEnMsdn,
 swstGe, swstEs,
 swstSiYuAscii, swstSiIsoCe,
 swstINetEng, swstINetSlo, swstWebIndex} TSwSetType;

ClassTP(TSwSet, PSwSet)//{
private:
  TInt SwSetType;
  TStrH SwStrH;
  TInt MnWordLen;
  void Add(const TStr& WordStr);
  void MultiAdd(const TStr& Str);
  void AddEn425();
  void AddEn523();
  void AddEnMsdn();
  void AddEs();
  void AddGe();
  void AddSiQCPSIYuAscii();
  void AddSiRYuAscii();
  void AddSiMYuAscii();
  void AddSiYuAscii();
  void AddSiIsoCe();
  void AddINetEng();
  void AddINetSlo();
  UndefCopyAssign(TSwSet);
public:
  TSwSet(const TSwSetType& _SwSetType=swstNone, const int& _MnWordLen=0);
  static PSwSet New(const TSwSetType& SwSetType=swstNone){
    return PSwSet(new TSwSet(SwSetType));}
  TSwSet(TSIn& SIn): SwSetType(SIn), SwStrH(SIn){}
  static PSwSet Load(TSIn& SIn){return new TSwSet(SIn);}
  void Save(TSOut& SOut) const {SwSetType.Save(SOut); SwStrH.Save(SOut);}

  // component retrieval
  TSwSetType GetSwSetType() const {
    return TSwSetType(int(SwSetType));}
  bool IsIn(const TStr& WordStr, const bool& UcWordStrP=true) const;
  void AddWord(const TStr& WordStr);
  void LoadFromFile(const TStr& FNm);

  // traverse words
  int FFirstSwId() const {return SwStrH.FFirstKeyId();}
  bool FNextSwId(int& SwId, TStr& SwStr) const {
    if (SwStrH.FNextKeyId(SwId)){
      SwStr=SwStrH.GetKey(SwId); return true;
    } else {return false;}}

  // stop-word-set creators
  static void GetSwSetTypeNmV(TStrV& SwSetTypeNmV, TStrV& SwSetTypeDNmV);
  static TStr GetSwSetTypeNmVStr();
  static TSwSetType GetSwSetType(const TStr& SwSetTypeNm);
  static PSwSet GetSwSet(const TSwSetType& SwSetType);
  static PSwSet GetSwSet(const bool& StopWordP, const TSwSetType& SwSetType){
    if (StopWordP){return GetSwSet(SwSetType);} else {return NULL;}}
  static PSwSet GetSwSet(const TStr& SwSetTypeNm){
    return GetSwSet(true, GetSwSetType(SwSetTypeNm));}
};

