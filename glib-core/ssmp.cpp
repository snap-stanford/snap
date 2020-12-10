//#//////////////////////////////////////////////

TSsParserMP::TSsParserMP(const TStr& FNm, const char& Separator, const bool& _SkipLeadBlanks, const bool& _SkipCmt, const bool& _SkipEmptyFld) : SsFmt(ssfSpaceSep), 
 SkipLeadBlanks(_SkipLeadBlanks), SkipCmt(_SkipCmt), SkipEmptyFld(_SkipEmptyFld), LineCnt(0), /*Bf(NULL),*/ SplitCh('\t'), LineStr(), FldV(), FInPt(NULL) {
  FInPt = TMIn::New(FNm, true);
  SplitCh = Separator;
}

TSsParserMP::~TSsParserMP() {
}

void TSsParserMP::SkipCommentLines() {
  FInPt->SkipCommentLines();
}

// Gets and parses the next line, quick version, works with buffers, not chars.
bool TSsParserMP::Next() { // split on SplitCh
  FldV.Clr(false);
  LineStr.Clr();
  FldV.Clr();
  LineCnt++;
  if (! FInPt->GetNextLnBf(LineStr)) { return false; }
  if (SkipCmt && !LineStr.Empty() && LineStr[0]=='#') { return Next(); }

  char* cur = LineStr.CStr();
  if (SkipLeadBlanks) { // skip leading blanks
    while (*cur && TCh::IsWs(*cur)) { cur++; }
  }
  char *last = cur;
  while (*cur) {
    if (SsFmt == ssfWhiteSep) { while (*cur && ! TCh::IsWs(*cur)) { cur++; } } 
    else { while (*cur && *cur!=SplitCh) { cur++; } }
    if (*cur == 0) { break; }
    *cur = 0;  cur++;
    FldV.Add(last);  last = cur;
    if (SkipEmptyFld && strlen(FldV.Last())==0) { FldV.DelLast(); } // skip empty fields
  }
  FldV.Add(last);  // add last field
  if (SkipEmptyFld && FldV.Empty()) { return Next(); } // skip empty lines
  return true; 
}

void TSsParserMP::ToLc() {
  for (int f = 0; f < FldV.Len(); f++) {
    for (char *c = FldV[f]; *c; c++) {
      *c = tolower(*c); }
  }
}

bool TSsParserMP::GetInt(const int& FldN, int& Val) const {
  // parsing format {ws} [+/-] +{ddd}
  int _Val = -1;
  bool Minus=false;
  const char *c = GetFld(FldN);
  while (TCh::IsWs(*c)) { c++; }
  if (*c=='-') { Minus=true; c++; }
  if (! TCh::IsNum(*c)) { return false; }
  _Val = TCh::GetNum(*c);  c++;
  while (TCh::IsNum(*c)){ 
    _Val = 10 * _Val + TCh::GetNum(*c); 
    c++; 
  }
  if (Minus) { _Val = -_Val; }
  if (*c != 0) { return false; }
  Val = _Val;
  return true;
}

bool TSsParserMP::GetFlt(const int& FldN, double& Val) const {
  // parsing format {ws} [+/-] +{d} ([.]{d}) ([E|e] [+/-] +{d})
  const char *c = GetFld(FldN);
  while (TCh::IsWs(*c)) { c++; }
  if (*c=='+' || *c=='-') { c++; }
  if (! TCh::IsNum(*c) && *c!='.') { return false; }
  while (TCh::IsNum(*c)) { c++; }
  if (*c == '.') {
    c++;
    while (TCh::IsNum(*c)) { c++; }
  }
  if (*c=='e' || *c == 'E') {
    c++;
    if (*c == '+' || *c == '-' ) { c++; }
    if (! TCh::IsNum(*c)) { return false; }
    while (TCh::IsNum(*c)) { c++; }
  }
  if (*c != 0) { return false; }
  Val = atof(GetFld(FldN));
  return true;
}

const char* TSsParserMP::DumpStr() const {
  static TChA ChA(10*1024);
  ChA.Clr();
  for (int i = 0; i < FldV.Len(); i++) {
    ChA += TStr::Fmt("  %d: '%s'\n", i, FldV[i]);
  }
  return ChA.CStr();
}

// Finds number of new line chars in interval [lb, ub)
// Assumes that lines end in '\n'
uint64 TSsParserMP::CountNewLinesInRange(uint64 Lb, uint64 Ub) const {
  return FInPt->CountNewLinesInRange(Lb, Ub);
}

TVec<uint64> TSsParserMP::GetStartPosV(uint64 Lb, uint64 Ub) const {
  TVec<uint64> Ret;
  if (Lb >= GetStreamLen()) {
    return Ret;
  }
  while (Lb < Ub) {
    // Find line corresponding to Lb
    uint64 StartPos = FInPt->GetLineStartPos(Lb);
    uint64 EndPos = FInPt->GetLineEndPos(Lb);

    // If line ends in given range, add to count
    if (Lb <= EndPos && EndPos < Ub) {
      Ret.Add(StartPos);
    }
    // Start at next line
    Lb = EndPos + 1;
  }
  return Ret;
}

// Essesntially the same as TssParser::Next
// For parallel load, FldV cannot be shared across many threads
void TSsParserMP::NextFromIndex(uint64 Index, TVec<char*>& FieldsV) 
{ 
  // split on SplitCh
  FieldsV.Clr();

  char* cur = FInPt->GetLine(Index);

  if (SkipLeadBlanks) { // skip leading blanks
    while (*cur && TCh::IsWs(*cur)) { cur++; }
  }
  char *last = cur;
  while (*cur != 0 && *cur != '\n') {
    if (SsFmt == ssfWhiteSep) { while (*cur && (*cur != '\n') && ! TCh::IsWs(*cur)) { cur++; } } 
    else { while (*cur && *cur!=SplitCh && (*cur != '\n')) { cur++; } }
    if (*cur == 0) { break; }
    if (*cur == '\n') { break; }
    //*cur = 0;  
    cur++;
    FieldsV.Add(last);  last = cur;
    if (SkipEmptyFld && strlen(FieldsV.Last())==0) { FieldsV.DelLast(); } // skip empty fields
  }
  FieldsV.Add(last);  // add last field
}

int TSsParserMP::GetIntFromFldV(TVec<char*>& FieldsV, const int& FldN) {
  // parsing format {ws} [+/-] +{ddd}
  int _Val = -1;
  bool Minus=false;
  const char *c = FieldsV[FldN];
  while (TCh::IsWs(*c)) { c++; }
  if (*c=='-') { Minus=true; c++; }
  if (! TCh::IsNum(*c)) { return -1; }
  _Val = TCh::GetNum(*c);  c++;
  while (TCh::IsNum(*c)){ 
    _Val = 10 * _Val + TCh::GetNum(*c); 
    c++; 
  }
  if (Minus) { _Val = -_Val; }
  if (*c != 0  &&  !TCh::IsWs(*c)) { return -1; }
  return _Val;
}

double TSsParserMP::GetFltFromFldV(TVec<char*>& FieldsV, const int& FldN) {
  // parsing format {ws} [+/-] +{d} ([.]{d}) ([E|e] [+/-] +{d})
  const char *c = FieldsV[FldN];
  /* skip whitespace at the beginning */
  while (TCh::IsWs(*c)) { c++; }
  /* skip the sign */
  if (*c=='+' || *c=='-') { c++; }
  /* error, if not a digit or '.' */
  if (! TCh::IsNum(*c) && *c!='.') { return -1; }
  /* skip digits */
  while (TCh::IsNum(*c)) { c++; }
  if (*c == '.') {
    c++;
    while (TCh::IsNum(*c)) { c++; }
  }
  /* skip exponent */
  if (*c=='e' || *c == 'E') {
    c++;
    if (*c == '+' || *c == '-' ) { c++; }
    if (! TCh::IsNum(*c)) { return -1; }
    while (TCh::IsNum(*c)) { c++; }
  }
  if (*c != 0  &&  !TCh::IsWs(*c)) { return -1; }
  return atof(FieldsV[FldN]);
}

