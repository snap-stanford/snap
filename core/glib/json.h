/////////////////////////////////////////////////
// Simple JSON parser (does not support nested objects and arrays)
class TJsonItem {
private:
  THash<TChA, TChA> KeyValH;
public:
  TJsonItem() { }
  TJsonItem(const TStr& JsonStr) { Parse(JsonStr.CStr()); }
  TJsonItem(const TChA& JsonStr) { Parse(JsonStr.CStr()); }
  TJsonItem(const char* JsonStr) { Parse(JsonStr); }
  const TChA& operator[](const int& KeyId) const { return KeyValH[KeyId]; }
  const TChA& operator[](const TChA& Key) const { return KeyValH.GetDat(Key); }
  const TChA& GetDat(const TChA& Key) const { return KeyValH.GetDat(Key); }
  bool IsKey(const TChA& Key) const { return KeyValH.IsKey(Key); }
  int Len() const { return KeyValH.Len(); }

  const char* GetStr(const char* Beg, TChA& Str) const {
    Str.Clr();
    const char* c = Beg+1;  IAssert(*Beg=='"');
    while (*c && *c != '"') {
      if (*c!='\\') { Str.Push(*c); }
      else {  c++;
        switch (*c) {
          case '"' : Str.Push('"'); break;
          case '\\' : Str.Push('\\'); break;
          case '/' : Str.Push('/'); break;
          case 'b' : Str.Push('\b'); break;
          case 'f' : Str.Push('\f'); break;
          case 'n' : Str.Push('\n'); break;
          case 't' : Str.Push('\t'); break;
          case 'r' : Str.Push('\r'); break;
          case 'u' : break; //printf("Unicode not supported: '%s'", Beg); break;
          default : FailR(TStr::Fmt("Unknown escape sequence: '%s'", Beg).CStr());
        };
      }
      c++;
    }
    if (*c && *c=='"') { c++; }
    return c;
  }
  void Parse(const char* JsonStr) {
    TChA KeyStr, ValStr;
    KeyValH.Clr(false, 1000, false);
    const char *c = JsonStr;
    while (*c && *c!='{' && *c!='[') { c++; } // find first '{' or '['
    while (*c && *c!='}' && *c!=']') {
      while (*c && *c!='"') { c++; }
      c = GetStr(c, KeyStr); // key -- string
      while (*c && TCh::IsWs(*c)) { c++; }
      IAssert(*c==':'); c++;
      while (*c && TCh::IsWs(*c)) { c++; }
      // value
      ValStr.Clr();
      if (*c == '"') { c = GetStr(c, ValStr); } // string
      else if (TCh::IsNum(*c) || (*c=='-' &&  TCh::IsNum(*(c+1)))) {  // number
        while (*c && *c!=',' && *c!='}' && *c!=']' && ! TCh::IsWs(*c)) { ValStr.Push(*c); c++; } }
      else if (*c=='t' || *c=='f' || *c=='n') { // true, false, null
        while (*c && *c!=',' && *c!='}' && *c!=']') { ValStr.Push(*c); c++; } }
      else if (*c=='{' || *c=='[') { FailR("Nested objects and arrays not supported"); }
      else { FailR(TStr::Fmt("Unknown character '%c' at position %d in %s", *c, int(c-JsonStr), JsonStr).CStr()); }
      KeyValH.AddDat(KeyStr, ValStr);
    }
  }
};

/////////////////////////////////////////////////
// Simple JSON parser
class TJsonLoader {
private:
  PSIn SIn;
  TFFile FFile;
  TChA Line;
  TJsonItem Item;
  int ItemCnt;
  TExeTm ExeTm;
public:
  TJsonLoader(const TStr& FNmWc) : FFile(FNmWc), ItemCnt(0) { }
  const TChA& operator[](const int& KeyId) const { return Item[KeyId]; }
  const TChA& operator[](const TChA& Key) const { return Item[Key]; }
  const TChA& GetDat(const TChA& Key) const { return Item.GetDat(Key); }
  bool IsKey(const TChA& Key) const { return Item.IsKey(Key); }
  int Len() const { return Item.Len(); }
  bool Next() {
    if (SIn.Empty() || SIn->Eof()) {
      TStr FNm;
      // if (! SIn.Empty()) { printf("  %d items\t[%s]\n", ItemCnt, ExeTm.GetTmStr()); }
      if (! FFile.Next(FNm)) { return false; }
      //printf("Parsing: %s\n", FNm.CStr()); ItemCnt=0;
      if (TZipIn::IsZipExt(FNm.GetFExt())) { SIn=TZipIn::New(FNm); }
      else { SIn=TFIn::New(FNm); }
      ExeTm.Tick();
    }
    SIn->GetNextLn(Line);
    Item.Parse(Line.CStr());  ItemCnt++;
    return true;
  }
};
