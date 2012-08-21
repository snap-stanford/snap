/////////////////////////////////////////////////
// Json-Value

typedef enum {
  jvtUndef, jvtNull, jvtBool, jvtNum, jvtStr, jvtArr, jvtObj} TJsonValType;

ClassTPV(TJsonVal, PJsonVal, TJsonValV)//{
private:
  TJsonValType JsonValType;
  TBool Bool; 
  TFlt Num; 
  TStr Str; 
  TJsonValV ValV;
  THash<TStr, PJsonVal> KeyValH;
  UndefCopyAssign(TJsonVal);
public:
  TJsonVal(): JsonValType(jvtUndef){}
  static PJsonVal New(){
    return new TJsonVal();}
  TJsonVal(TSIn& SIn){}
  static PJsonVal Load(TSIn& SIn){return new TJsonVal(SIn);}
  void Save(TSOut& SOut) const {}

  // putting value
  void PutNull(){JsonValType=jvtNull;}
  void PutBool(const bool& _Bool){JsonValType=jvtBool; Bool=_Bool;}
  void PutNum(const double& _Num){JsonValType=jvtNum; Num=_Num;}
  void PutStr(const TStr& _Str){JsonValType=jvtStr; Str=_Str;}
  void PutArr(){JsonValType=jvtArr;}
  void AddToArr(const PJsonVal& Val){
    EAssert(JsonValType==jvtArr); ValV.Add(Val);}
  void PutObj(){JsonValType=jvtObj;}
  void AddToObj(const TStr& KeyNm, const PJsonVal& Val){
    EAssert(JsonValType==jvtObj); KeyValH.AddDat(KeyNm, Val);}
  void AddToObj(const TStr& KeyNm, const int& Val){ AddToObj(KeyNm, NewNum((double)Val)); }
  void AddToObj(const TStr& KeyNm, const double& Val){ AddToObj(KeyNm, NewNum(Val)); }
  void AddToObj(const TStr& KeyNm, const TStr& Val){ AddToObj(KeyNm, NewStr(Val)); }
  void AddToObj(const TStr& KeyNm, const char* Val){ AddToObj(KeyNm, NewStr(Val)); }
  void AddToObj(const TStr& KeyNm, const bool& Val){ AddToObj(KeyNm, NewBool(Val)); }
  void AddToObj(const TStr& KeyNm, const TJsonValV& ValV){ AddToObj(KeyNm, NewArr(ValV)); }

  // simplified coreation of basic elements
  static PJsonVal NewNull() { PJsonVal Val = TJsonVal::New(); Val->PutNull(); return Val; }
  static PJsonVal NewBool(const bool& Bool) { PJsonVal Val = TJsonVal::New(); Val->PutBool(Bool); return Val; }
  static PJsonVal NewNum(const double& Num) { PJsonVal Val = TJsonVal::New(); Val->PutNum(Num); return Val; }
  static PJsonVal NewStr(const TStr& Str) { PJsonVal Val = TJsonVal::New(); Val->PutStr(Str); return Val; }
  static PJsonVal NewArr() { PJsonVal Val = TJsonVal::New(); Val->PutArr(); return Val; }
  static PJsonVal NewArr(const TJsonValV& ValV);
  static PJsonVal NewArr(const TStrV& StrV);
  static PJsonVal NewObj() { PJsonVal Val = TJsonVal::New(); Val->PutObj(); return Val; }
  static PJsonVal NewObj(const TStr& KeyNm, const PJsonVal& ObjVal) {
	  PJsonVal Val = TJsonVal::New(); Val->PutObj(); Val->AddToObj(KeyNm, ObjVal); return Val; }
  static PJsonVal NewObj(const TStr& KeyNm, const int& ObjVal) {
	  PJsonVal Val = TJsonVal::New(); Val->PutObj(); Val->AddToObj(KeyNm, ObjVal); return Val; }
  static PJsonVal NewObj(const TStr& KeyNm, const double& ObjVal) {
	  PJsonVal Val = TJsonVal::New(); Val->PutObj(); Val->AddToObj(KeyNm, ObjVal); return Val; }
  static PJsonVal NewObj(const TStr& KeyNm, const TStr& ObjVal) {
	  PJsonVal Val = TJsonVal::New(); Val->PutObj(); Val->AddToObj(KeyNm, ObjVal); return Val; }
  static PJsonVal NewObj(const TStr& KeyNm, const bool& ObjVal) {
	  PJsonVal Val = TJsonVal::New(); Val->PutObj(); Val->AddToObj(KeyNm, ObjVal); return Val; }

  // testing value-type
  TJsonValType GetJsonValType() const {return JsonValType;}
  bool IsNull() const {return JsonValType==jvtNull;}
  bool IsBool() const {return JsonValType==jvtBool;}
  bool IsNum() const {return JsonValType==jvtNum;}
  bool IsStr() const {return JsonValType==jvtStr;}
  bool IsArr() const {return JsonValType==jvtArr;}
  bool IsObj() const {return JsonValType==jvtObj;}

  // getting value
  bool GetBool() const {EAssert(IsBool()); return Bool;}
  double GetNum() const {EAssert(IsNum()); return Num;}
  TStr GetStr() const {EAssert(IsStr()); return Str;}
  int GetArrVals() const {EAssert(IsArr()); return ValV.Len();}
  PJsonVal GetArrVal(const int& ValN) const {return ValV[ValN];}
  int GetObjKeys() const {EAssert(IsObj()); return KeyValH.Len();}
  void GetObjKeyVal(const int& KeyValN, TStr& Key, PJsonVal& Val) const {
    EAssert(IsObj()); Key=KeyValH.GetKey(KeyValN); Val=KeyValH[KeyValN];}
  bool IsObjKey(const TStr& Key) const {EAssert(IsObj()); return KeyValH.IsKey(Key);}
  bool IsObjKey(const char *Key) const {EAssert(IsObj()); return KeyValH.IsKey(Key);}
  PJsonVal GetObjKey(const TStr& Key) const;
  PJsonVal GetObjKey(const char *Key) const;
  bool GetObjBool(const TStr& Key) const { return GetObjKey(Key)->GetBool(); }
  bool GetObjBool(const char *Key) const { return GetObjKey(Key)->GetBool(); }
  double GetObjNum(const TStr& Key) const { return GetObjKey(Key)->GetNum(); }
  double GetObjNum(const char *Key) const { return GetObjKey(Key)->GetNum(); }
  TStr GetObjStr(const TStr& Key) const { return GetObjKey(Key)->GetStr(); }
  TStr GetObjStr(const char *Key) const { return GetObjKey(Key)->GetStr(); }
  bool GetObjBool(const TStr& Key, const bool& DefBool) const;
  bool GetObjBool(const char *Key, const bool& DefBool) const;
  double GetObjNum(const TStr& Key, const double& DefNum) const;
  double GetObjNum(const char *Key, const double& DefNum) const;
  TStr GetObjStr(const TStr& Key, const TStr& DefStr) const;
  TStr GetObjStr(const char *Key, const TStr& DefStr) const;

  // (de)serialization
  static PJsonVal GetValFromLx(TILx& Lx);
  static PJsonVal GetValFromStr(const TStr& JsonStr);
  static void AddEscapeChAFromStr(const TStr& Str, TChA& ChA);
  static TStr AddEscapeStrFromStr(const TStr& Str) { 
	  TChA ChA; AddEscapeChAFromStr(Str, ChA); return ChA; }
  static void AddQChAFromStr(const TStr& Str, TChA& ChA);
  static void GetChAFromVal(const PJsonVal& Val, TChA& ChA);
  static TStr GetStrFromVal(const PJsonVal& Val);
};
