/////////////////////////////////////////////////
// Unicode-Definition
class TUnicodeDef{
private:
  static TUnicodeDef UnicodeDef;
private:
  TUnicode* Unicode;
  UndefCopyAssign(TUnicodeDef);
public:
  TUnicodeDef(): Unicode(NULL){}
  ~TUnicodeDef(){if (Unicode!=NULL){delete Unicode;} Unicode=NULL;}

  // files
  static TStr GetDfFNm();
  static void Load(){
    Load(GetDfFNm());}
  static void Load(const TStr& FNm){
    UnicodeDef.Unicode=new TUnicode(FNm);}

  // status
  static bool IsDef(){
    return UnicodeDef.Unicode!=NULL;}
  static TUnicode* GetDef(){
    Assert(UnicodeDef.Unicode!=NULL); return UnicodeDef.Unicode;}
};

/////////////////////////////////////////////////
// Unicode-String
class TUStr;
typedef TVec<TUStr> TUStrV;

class TUStr{
private:
  TIntV UniChV;
  static void AssertUnicodeDefOk(){
    EAssertR(TUnicodeDef::IsDef(), "Unicode-Definition-File not loaded!");}
public:
  TUStr(): UniChV(){AssertUnicodeDefOk();}
  TUStr(const TUStr& UStr): UniChV(UStr.UniChV){AssertUnicodeDefOk();}
  TUStr(const TIntV& _UniChV): UniChV(_UniChV){AssertUnicodeDefOk();}
  TUStr(const TStr& Str);
  ~TUStr(){}
  TUStr(TSIn& SIn): UniChV(SIn){AssertUnicodeDefOk();}
  void Save(TSOut& SOut) const {UniChV.Save(SOut);}
  void LoadXml(const PXmlTok& XmlTok, const TStr& Nm);
  void SaveXml(TSOut& SOut, const TStr& Nm) const;

  TUStr& operator=(const TUStr& UStr){
    if (this!=&UStr){UniChV=UStr.UniChV;} return *this;}
  bool operator==(const TUStr& UStr) const {return UniChV==UStr.UniChV;}

  TUStr& operator+=(const TUStr& UStr){UniChV.AddV(UStr.UniChV); return *this;}
  int operator[](const int& UniChN) const {return UniChV[UniChN];}

  // basic operations
  void Clr(){UniChV.Clr();}
  int Len() const {return UniChV.Len();}
  bool Empty() const {return UniChV.Empty();}

  // transformations
  void ToLowerCase();
  void ToUpperCase();
  void ToStarterCase();

  // word boundaries
  void GetWordBoundPV(TBoolV& WordBoundPV);
  void GetWordUStrV(TUStrV& UStrV);

  // conversions to string
  TStr GetStr() const;
  TStr GetStarterStr() const;
  TStr GetStarterLowerCaseStr() const;

  // scripts
  static int GetScriptId(const TStr& ScriptNm);
  static TStr GetScriptNm(const int& ScriptId);
  static int GetChScriptId(const int& UniCh);
  static TStr GetChScriptNm(const int& UniCh);

  // characters
  static TStr GetChNm(const int& UniCh);
  static TStr GetChTypeStr(const int& UniCh);
  static bool IsCase(const int& UniCh);
  static bool IsUpperCase(const int& UniCh);
  static bool IsLowerCase(const int& UniCh);
  static bool IsAlphabetic(const int& UniCh);
  static bool IsMath(const int& UniCh);

  // converstions to/from UTF8
  static TStr EncodeUtf8(const int& UniCh);
};
