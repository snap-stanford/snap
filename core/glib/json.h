class TJsonLoader;
class TJsonObj;

/////////////////////////////////////////////////
// JSON object
class TJsonObj {
private:
  THash<TChA, TChA> KeyValH;              // key-value pairs
  THash<TChA, TJsonObj> KeyObjH;          // key-nested object
  THash<TChA, TVec<TJsonObj> > KeyArrayH; // key-array of objects
private:
  int GetStr(const TStr& JsonStr, int Beg, TChA& Str, bool &IsValid) const;
  int ParseArrayVal(const TStr& JsonStr, int Beg, bool &valid);
  int Parse(const TStr& JsonStr, int Beg, bool &valid);  
  
  const char* GetStr(const char* Beg, TChA& Str) const;
  const char* ParseArrayVal(const char* JsonStr);
  const char* Parse(const char* JsonStr);
public:
  TJsonObj() { }
  TJsonObj(const TStr& JsonStr) { Parse(JsonStr.CStr()); }
  TJsonObj(const TChA& JsonStr) { Parse(JsonStr.CStr()); }
  TJsonObj(const char* JsonStr) { Parse(JsonStr); }
  TJsonObj(const TJsonObj& Item) : KeyValH(Item.KeyValH), KeyObjH(Item.KeyObjH), KeyArrayH(Item.KeyArrayH) { }

  // access to key:value elements (all values are strings)
  const TChA& operator[](const int& KeyId) const { return KeyValH[KeyId]; }
  const TChA& operator[](const TChA& Key) const { return KeyValH.GetDat(Key); }
  const TChA& GetDat(const TChA& Key) const { return KeyValH.GetDat(Key); }
  bool IsKey(const TChA& Key) const { return KeyValH.IsKey(Key); }
  int Len() const { return KeyValH.Len(); }
  
  // access to key:object and key:array  elements 
  // (array elements are objects with a single key:value element)
  const TJsonObj& GetObj(const int& KeyId) const { return KeyObjH[KeyId]; }
  const TJsonObj& GetObj(const TChA& Key) const { return KeyObjH.GetDat(Key); }
  bool IsObj(const TChA& Key) const { return KeyObjH.IsKey(Key); }
  int LenObj() const { return KeyObjH.Len(); }
  
  // array
  const TChA& GetArrayKey(const int& KeyId) const { return KeyArrayH.GetKey(KeyId); }
  int LenArray() const { return KeyArrayH.Len(); }
  bool IsArray(const TChA& Key) const { return KeyArrayH.IsKey(Key); }
  const TVec<TJsonObj>& GetArray(const int& KeyId) const { return KeyArrayH[KeyId]; }
  const TVec<TJsonObj>& GetArray(const TChA& Key) const { return KeyArrayH.GetDat(Key); }
    
  void Clr();
  void Dump(const int& Indent=0) const;
  friend class TJsonLoader;
};

/////////////////////////////////////////////////
// JSON parser
class TJsonLoader {
private:
  PSIn SIn;
  TFFile FFile;
  TChA Line;
  TJsonObj Item;
  int LineNo, FileCnt, ItemCnt;
  TExeTm ExeTm;
public:
  TJsonLoader(const TStr& FNmWc) : FFile(FNmWc), LineNo(0), FileCnt(0), ItemCnt(0) { }
  
  bool Next();
  TStr GetCurFNm() const { return SIn->GetSNm(); }
  int GetLineNo() const { return LineNo; }
  TChA GetLineStr() const { return Line; }

  // access to key:value elements (all values are strings)
  const TChA& operator[](const int& KeyId) const { return Item[KeyId]; }
  const TChA& operator[](const TChA& Key) const { return Item[Key]; }
  const TChA& GetDat(const TChA& Key) const { return Item.GetDat(Key); }
  bool IsKey(const TChA& Key) const { return Item.IsKey(Key); }
  int Len() const { return Item.Len(); }
  
  // access to key:object and key:array  elements 
  // (array elements are objects with a single key:value element)
  const TJsonObj& GetObj(const int& KeyId) const { return Item.GetObj(KeyId); }
  const TJsonObj& GetObj(const TChA& Key) const { return Item.GetObj(Key); }
  bool IsObj(const TChA& Key) const { return Item.IsObj(Key); }
  int LenObj() const { return Item.LenObj(); }
  const TVec<TJsonObj>& GetArray(const int& KeyId) const { return Item.GetArray(KeyId); }
  const TVec<TJsonObj>& GetArray(const TChA& Key) const { return Item.GetArray(Key); }
  bool IsArray(const TChA& Key) const { return Item.IsArray(Key); }
  int LenArray() const { return Item.LenArray(); }

  void Dump() const { Item.Dump(0); }
};
