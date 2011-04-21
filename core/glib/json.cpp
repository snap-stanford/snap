/////////////////////////////////////////////////
// JSON object
const char* TJsonObj::GetStr(const char* Beg, TChA& Str) const {
  Str.Clr();
  const char* c = Beg+1;  
  EAssertR(*Beg=='"', "JSON error: Strings should start with '\"'");
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
        default : EFailR(TStr::Fmt("JSON Error: Unknown escape sequence: '%s'", Beg).CStr());
      };
    }
    c++;
  }
  if (*c && *c=='"') { c++; }
  return c;
}
const char* TJsonObj::ParseArrayVal(const char* JsonStr) {
  const char *c = JsonStr;
  bool Nested = false;
  TChA ValStr;
  Clr();
  while (*c && TCh::IsWs(*c)) { c++; }
  if (*c == '"') { c = GetStr(c, ValStr); } // string
  else if (TCh::IsNum(*c) || (*c=='-' &&  TCh::IsNum(*(c+1)))) {  // number
    while (*c && *c!=',' && *c!='}' && *c!=']' && ! TCh::IsWs(*c)) { ValStr.Push(*c); c++; } }
  else if (*c=='t' || *c=='f' || *c=='n') { // true, false, null
    while (*c && *c!=',' && *c!='}' && *c!=']') { ValStr.Push(*c); c++; } }
  else if (*c=='{') { // nested object
    EAssertR(! KeyObjH.IsKey("key"), "JSON error: object already contains 'key' element");
    TJsonObj& Obj = KeyObjH.AddDat("key");
    c = Obj.Parse(c) + 1;  Nested = true;
  }
  else if (*c=='[') { // array
    EAssertR(! KeyArrayH.IsKey("key"), "JSON error: object already contains 'key' element");
    TVec<TJsonObj>& Array = KeyArrayH.AddDat("key");
      c++;
      while (*c && *c!=']') {
        while (*c && TCh::IsWs(*c)) { c++; }
        Array.Add();
        if (*c=='{') { c = Array.Last().Parse(c) + 1; } // nested object
        else { c = Array.Last().ParseArrayVal(c); }
        if (*c && *c==',') { c++; }
      }
      c++; Nested = true;
  }
  if (! Nested) {
    EAssertR(! KeyValH.IsKey("key"), "JSON error: object already contains 'key' element");
    KeyValH.AddDat("key", ValStr); 
  }
  while (*c && TCh::IsWs(*c)) { c++; }
  return c;
}

const char* TJsonObj::Parse(const char* JsonStr) {
  TChA KeyStr, ValStr;
  Clr();
  const char *c = JsonStr;
  bool Nested = false;
  while (*c && *c!='{') { c++; } // find first '{'
  while (*c && *c!='}') {
    while (*c && *c!='"') { c++; }
    c = GetStr(c, KeyStr); // key -- string
    while (*c && TCh::IsWs(*c)) { c++; }
    EAssertR(*c==':', "JSON error: Expect ':'");
    c++;
    while (*c && TCh::IsWs(*c)) { c++; }
    // value
    ValStr.Clr();
    if (*c == '"') { c = GetStr(c, ValStr); } // string
    else if (TCh::IsNum(*c) || (*c=='-' &&  TCh::IsNum(*(c+1)))) {  // number
      while (*c && *c!=',' && *c!='}' && *c!=']' && ! TCh::IsWs(*c)) { ValStr.Push(*c); c++; } }
    else if (*c=='t' || *c=='f' || *c=='n') { // true, false, null
      while (*c && *c!=',' && *c!='}' && *c!=']') { ValStr.Push(*c); c++; } }
    else if (*c=='{') { // nested object
      TJsonObj& Obj = KeyObjH.AddDat(KeyStr);
      c = Obj.Parse(c) + 1;  Nested = true;
    }
    else if (*c=='[') { // array
      TVec<TJsonObj>& Array = KeyArrayH.AddDat(KeyStr);
      c++;
      while (*c && *c!=']') {
        while (*c && TCh::IsWs(*c)) { c++; }
        Array.Add();
        if (*c=='{') { c = Array.Last().Parse(c) + 1; } // nested object
        else { c = Array.Last().ParseArrayVal(c); }
        if (*c && *c==',') { c++; }
      }
      c++; Nested = true;
    }
    else { EFailR(TStr::Fmt("JSON error: Unknown character '%c' at position %d in %s", *c, int(c-JsonStr), JsonStr).CStr()); }
    if (! Nested) {
      KeyValH.AddDat(KeyStr, ValStr); }
    while (*c && TCh::IsWs(*c)) { c++; }
    if (*c && *c==',') { c++; }
    while (*c && TCh::IsWs(*c)) { c++; }
  }
  return c;
}

void TJsonObj::Clr() { 
  KeyValH.Clr(false, 1000, false);
  KeyObjH.Clr(false, 1000, false);
  KeyArrayH.Clr(false, 1000, false);
}

void TJsonObj::Dump(const int& Indent) const {
  // short outputs
  if (KeyValH.Len()==0 && KeyArrayH.Len()==0 && KeyObjH.Len()==0) {
    for (int x=0;x<Indent;x++){printf(" ");} 
    printf("{ }\n", KeyValH.GetKey(0).CStr(), KeyValH[0].CStr());
    return;
  }
  if (KeyValH.Len()==1 && KeyArrayH.Len()==0 && KeyObjH.Len()==0) {
    for (int x=0;x<Indent;x++){printf(" ");} 
    printf("{ %s : %s }\n", KeyValH.GetKey(0).CStr(), KeyValH[0].CStr());
    return;
  }
  // long output
  for (int x=0;x<Indent;x++){printf(" ");} printf("{\n");
  for (int i = 0; i < KeyValH.Len(); i++) {
    for (int x=0;x<Indent;x++){printf(" ");}
    printf("%s : %s\n", KeyValH.GetKey(i).CStr(), KeyValH[i].CStr());
  }
  if (KeyArrayH.Len() > 0) {
    for (int i = 0; i < KeyArrayH.Len(); i++) {
      const TVec<TJsonObj>& Array = KeyArrayH[i];
      for (int x=0;x<Indent;x++){printf(" ");} printf("%s : [\n", KeyArrayH.GetKey(i).CStr());
      for (int a = 0; a < Array.Len(); a++) { Array[a].Dump(Indent+2); }
      for (int x=0;x<Indent;x++){printf(" ");} printf("]\n");
    }
  }
  if (KeyObjH.Len() > 0) {
    for (int i = 0; i < KeyObjH.Len(); i++) {
      for (int x=0;x<Indent;x++){printf(" ");} 
      printf("%s : \n", KeyObjH.GetKey(i).CStr());
      KeyObjH[i].Dump(Indent+2);
    }
  }
  for (int x=0;x<Indent;x++){printf(" ");} printf("}\n");
}

/////////////////////////////////////////////////
// Simple JSON parser
bool TJsonLoader::Next() {
  if (SIn.Empty() || SIn->Eof()) {
    TStr FNm;
    if (! SIn.Empty()) { printf("  %d items in file [%s]\n", LineNo, ExeTm.GetTmStr()); }
    if (! FFile.Next(FNm)) { return false; }
    printf("Parsing: %s\n", FNm.CStr());  LineNo=0;
    if (TZipIn::IsZipExt(FNm.GetFExt())) { SIn=TZipIn::New(FNm); }
    else { SIn=TFIn::New(FNm); }
    ExeTm.Tick();
  }
  SIn->GetNextLn(Line);
  Item.Parse(Line.CStr());
  LineNo++;
  return true;
}
