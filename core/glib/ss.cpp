/////////////////////////////////////////////////
// Spread-Sheet
TStr& TSs::At(const int& X, const int& Y){
//  Fail;
  if (Y>=CellStrVV.Len()){CellStrVV.Reserve(Y+1, Y+1);}
  if (X>=CellStrVV[Y]->Len()){CellStrVV[Y]->V.Reserve(X+1, X+1);}
  return CellStrVV[Y]->V[X];
}

void TSs::PutVal(const int& X, const int& Y, const TStr& Str){
  if (Y>=CellStrVV.Len()){CellStrVV.Reserve(Y+1, Y+1);}
  if (X>=CellStrVV[Y]->Len()){CellStrVV[Y]->V.Reserve(X+1, X+1);}
  CellStrVV[Y]->V[X]=Str;
}

TStr TSs::GetVal(const int& X, const int& Y) const {
  if ((0<=Y)&&(Y<CellStrVV.Len())){
    if ((0<=X)&&(X<CellStrVV[Y]->Len())){
      return CellStrVV[Y]->V[X];
    } else {
      return TStr::GetNullStr();
    }
  } else {
    return TStr::GetNullStr();
  }
}

int TSs::GetXLen() const {
  if (CellStrVV.Len()==0){
    return 0;
  } else {
    int MxXLen=CellStrVV[0]->Len();
    for (int Y=1; Y<CellStrVV.Len(); Y++){
      MxXLen=TInt::GetMx(MxXLen, CellStrVV[Y]->Len());}
    return MxXLen;
  }
}

int TSs::GetXLen(const int& Y) const {
  if ((0<=Y)&&(Y<CellStrVV.Len())){
    return CellStrVV[Y]->Len();
  } else {
    return 0;
  }
}

int TSs::GetYLen() const {
  return CellStrVV.Len();
}

int TSs::SearchX(const int& Y, const TStr& Str) const {
  return CellStrVV[Y]->V.SearchForw(Str);
}

int TSs::SearchY(const int& X, const TStr& Str) const {
  int YLen=GetYLen();
  for (int Y=0; Y<YLen; Y++){
     if (Str==GetVal(X, Y)){return Y;}}
  return -1;
}

void TSs::DelX(const int& X){
  int YLen=GetYLen();
  for (int Y=0; Y<YLen; Y++){
    CellStrVV[Y]->V.Del(X);
  }
}

void TSs::DelY(const int& Y){
  CellStrVV.Del(Y);
}

int TSs::GetFldX(const TStr& FldNm, const TStr& NewFldNm, const int& Y) const {
  if (GetYLen()>Y){
    int XLen=GetXLen(Y);
    for (int X=0; X<XLen; X++){
      if (GetVal(X, Y).GetTrunc()==FldNm){
        if (!NewFldNm.Empty()){GetVal(X, Y)=NewFldNm;}
        return X;
      }
    }
    return -1;
  } else {
    return -1;
  }
}

int TSs::GetFldY(const TStr& FldNm, const TStr& NewFldNm, const int& X) const {
  for (int Y=0; Y<GetYLen(); Y++){
    if (GetXLen(Y)>X){
      if (GetVal(X, Y).GetTrunc()==FldNm){
        if (!NewFldNm.Empty()){GetVal(X, Y)=NewFldNm;}
        return Y;
      }
    }
  }
  return -1;
}

PSs TSs::LoadTxt(
 const TSsFmt& SsFmt, const TStr& FNm,
 const PNotify& Notify, const bool& IsExcelEoln,
 const int& MxY, const TIntV& AllowedColNV, const bool& IsQStr){
  TNotify::OnNotify(Notify, ntInfo, TStr("Loading File ")+FNm+" ...");
  PSIn SIn=TFIn::New(FNm);
  PSs Ss=TSs::New();
  if (!SIn->Eof()){
    int X=0; int Y=0; int PrevX=-1; int PrevY=-1;
    char Ch=SIn->GetCh(); TChA ChA;
    while (!SIn->Eof()){
      // compose value
      ChA.Clr();
      if (IsQStr&&(Ch=='"')){
        // quoted string ('""' sequence means '"')
        Ch=SIn->GetCh();
        forever {
          while ((!SIn->Eof())&&(Ch!='"')){
            ChA+=Ch; Ch=SIn->GetCh();}
          if (Ch=='"'){
            Ch=SIn->GetCh();
            if (Ch=='"'){ChA+=Ch; Ch=SIn->GetCh();}
            else {break;}
          }
        }
      } else {
        if (SsFmt==ssfTabSep){
          while ((!SIn->Eof())&&(Ch!='\t')&&(Ch!='\r')&&((Ch!='\n')||IsExcelEoln)){
            ChA+=Ch; Ch=SIn->GetCh();
          }
        } else
        if (SsFmt==ssfCommaSep){
          while ((!SIn->Eof())&&(Ch!=',')&&(Ch!='\r')&&((Ch!='\n')||IsExcelEoln)){
            ChA+=Ch; Ch=SIn->GetCh();
          }
        } else
        if (SsFmt==ssfSemicolonSep){
          while ((!SIn->Eof())&&(Ch!=';')&&(Ch!='\r')&&((Ch!='\n')||IsExcelEoln)){
            ChA+=Ch; Ch=SIn->GetCh();
          }
        } else
        if (SsFmt==ssfSpaceSep){
          while ((!SIn->Eof())&&(Ch!=' ')&&(Ch!='\r')&&((Ch!='\n')||IsExcelEoln)){
            ChA+=Ch; Ch=SIn->GetCh();
          }
        } else {
          Fail;
        }
      }
      // add new line if neccessary
      if (PrevY!=Y){
        if ((MxY!=-1)&&(Ss->CellStrVV.Len()==MxY)){break;}
        Ss->CellStrVV.Add(TStrVP::New()); PrevY=Y;
        int Recs=Ss->CellStrVV.Len();
        if (Recs%1000==0){
          TNotify::OnStatus(Notify, TStr::Fmt("  %d\r", Recs));}
      }
      // add value to spreadsheet
      if (AllowedColNV.Empty()||AllowedColNV.IsIn(X)){
        Ss->CellStrVV[Y]->V.Add(ChA); 
      }
      // process delimiters
      if (SIn->Eof()){
        break;
      } else
      if ((SsFmt==ssfTabSep)&&(Ch=='\t')){
        X++; Ch=SIn->GetCh();
      } else
      if ((SsFmt==ssfCommaSep)&&(Ch==',')){
        X++; Ch=SIn->GetCh();
      } else
      if ((SsFmt==ssfSemicolonSep)&&(Ch==';')){
        X++; Ch=SIn->GetCh();
      } else
      if ((SsFmt==ssfSpaceSep)&&(Ch==' ')){
        X++; Ch=SIn->GetCh();
      } else
      if (Ch=='\r'){
        if ((PrevX!=-1)&&(X!=PrevX)){
          TNotify::OnNotify(Notify, ntWarn, "Number of fields is not the same!");}
        PrevX=X; X=0; Y++; Ch=SIn->GetCh();
        if ((Ch=='\n')&&(!SIn->Eof())){Ch=SIn->GetCh();}
        //if (Ss->CellStrVV.Len()%1000==0){Y--; break;}
      } else
      if (Ch=='\n'){
        if ((PrevX!=-1)&&(X!=PrevX)){
          TNotify::OnNotify(Notify, ntWarn, "Number of fields is not the same!");}
        PrevX=X; X=0; Y++; Ch=SIn->GetCh();
        if ((Ch=='\r')&&(!SIn->Eof())){Ch=SIn->GetCh();}
        //if (Ss->CellStrVV.Len()%1000==0){Y--; break;}
      } else {
        Fail;
      }
    }
  }
  int Recs=Ss->CellStrVV.Len();
  TNotify::OnNotify(Notify, ntInfo, TStr::Fmt("  %d records read.", Recs));
  TNotify::OnNotify(Notify, ntInfo, "... Done.");
  return Ss;
}

void TSs::SaveTxt(const TStr& FNm, const PNotify&) const {
  PSOut SOut=TFOut::New(FNm);
  for (int Y=0; Y<CellStrVV.Len(); Y++){
    for (int X=0; X<CellStrVV[Y]->Len(); X++){
      if (X>0){SOut->PutCh('\t');}
      TStr Str=CellStrVV[Y]->V[X];
      TChA ChA(Str);
      for (int ChN=0; ChN<ChA.Len(); ChN++){
        char Ch=ChA[ChN];
        if ((Ch=='\t')||(Ch=='\r')||(Ch=='\n')){
          ChA.PutCh(ChN, ' ');
        }
      }
      SOut->PutStr(ChA);
    }
    SOut->PutCh('\r'); SOut->PutCh('\n');
  }
}

void TSs::LoadTxtFldV(
 const TSsFmt& SsFmt, const PSIn& SIn, char& Ch,
 TStrV& FldValV, const bool& IsExcelEoln, const bool& IsQStr){
  if (!SIn->Eof()){
    FldValV.Clr(false); int X=0;
    if (Ch==TCh::NullCh){Ch=SIn->GetCh();}
    TChA ChA;
    while (!SIn->Eof()){
      // compose value
      ChA.Clr();
      if (IsQStr&&(Ch=='"')){
        // quoted string ('""' sequence means '"')
        Ch=SIn->GetCh();
        forever {
          while ((!SIn->Eof())&&(Ch!='"')){
            ChA+=Ch; Ch=SIn->GetCh();}
          if (Ch=='"'){
            Ch=SIn->GetCh();
            if (Ch=='"'){ChA+=Ch; Ch=SIn->GetCh();}
            else {break;}
          }
        }
      } else {
        if (SsFmt==ssfTabSep){
          while ((!SIn->Eof())&&(Ch!='\t')&&(Ch!='\r')&&
           ((Ch!='\n')||IsExcelEoln)){
            ChA+=Ch; Ch=SIn->GetCh();
          }
          if ((!ChA.Empty())&&(ChA.LastCh()=='\"')){
            ChA.Trunc(ChA.Len()-1);}
        } else
        if (SsFmt==ssfCommaSep){
          while ((!SIn->Eof())&&(Ch!=',')&&(Ch!='\r')&&
           ((Ch!='\n')||IsExcelEoln)){
            ChA+=Ch; Ch=SIn->GetCh();
          }
        } else
        if (SsFmt==ssfSemicolonSep){
          while ((!SIn->Eof())&&(Ch!=';')&&(Ch!='\r')&&
           ((Ch!='\n')||IsExcelEoln)){
            ChA+=Ch; Ch=SIn->GetCh();
          }
        } else {
          Fail;
        }
      }
      // add value to spreadsheet
      ChA.Trunc();
      FldValV.Add(ChA);
      // process delimiters
      if (SIn->Eof()){
        break;
      } else
      if ((SsFmt==ssfTabSep)&&(Ch=='\t')){
        X++; Ch=SIn->GetCh();
      } else
      if ((SsFmt==ssfCommaSep)&&(Ch==',')){
        X++; Ch=SIn->GetCh();
      } else
      if ((SsFmt==ssfSemicolonSep)&&(Ch==';')){
        X++; Ch=SIn->GetCh();
      } else
      if (Ch=='\r'){
        Ch=SIn->GetCh();
        if ((Ch=='\n')&&(!SIn->Eof())){Ch=SIn->GetCh();}
        break;
      } else
      if (Ch=='\n'){
        X=0; Ch=SIn->GetCh();
        if ((Ch=='\r')&&(!SIn->Eof())){Ch=SIn->GetCh();}
        break;
      } else {
        Fail;
      }
    }
  }
}

TSsFmt TSs::GetSsFmtFromStr(const TStr& SsFmtNm){
  TStr LcSsFmtNm=SsFmtNm.GetLc();
  if (LcSsFmtNm=="tab"){return ssfTabSep;}
  else if (LcSsFmtNm=="comma"){return ssfCommaSep;}
  else if (LcSsFmtNm=="semicolon"){return ssfSemicolonSep;}
  else if (LcSsFmtNm=="space"){return ssfSpaceSep;}
  else if (LcSsFmtNm=="white"){return ssfWhiteSep;}
  else {return ssfUndef;}
}

TStr TSs::GetStrFromSsFmt(const TSsFmt& SsFmt){
  switch (SsFmt){
    case ssfTabSep: return "tab";
    case ssfCommaSep: return "comma";
    case ssfSemicolonSep: return "semicolon";
    case ssfSpaceSep: return "space";
    case ssfWhiteSep: return "white";
    default: return "undef";
  }
}

TStr TSs::GetSsFmtNmVStr(){
  TChA ChA;
  ChA+='(';
  ChA+="tab"; ChA+=", ";
  ChA+="comma"; ChA+=", ";
  ChA+="semicolon"; ChA+=", ";
  ChA+="space"; ChA+=", ";
  ChA+="white"; ChA+=")";
  return ChA;
}

/////////////////////////////////////////////////
// Fast-Spread-Sheet-Parser
TSsParser::TSsParser(const TStr& FNm, const TSsFmt _SsFmt, const bool& _SkipLeadBlanks, const bool& _SkipCmt, const bool& _SkipEmptyFld) : SsFmt(_SsFmt), 
 SkipLeadBlanks(_SkipLeadBlanks), SkipCmt(_SkipCmt), SkipEmptyFld(_SkipEmptyFld), LineCnt(0), /*Bf(NULL),*/ SplitCh('\t'), FldV(), FInPt(NULL) {
  if (TZipIn::IsZipExt(FNm.GetFExt())) { FInPt = TZipIn::New(FNm); }
  else { FInPt = TFIn::New(FNm); }
  //Bf = new char [BfLen];
  switch(SsFmt) {
    case ssfTabSep : SplitCh = '\t'; break;
    case ssfCommaSep : SplitCh = ','; break;
    case ssfSemicolonSep : SplitCh = ';'; break;
    case ssfSpaceSep : SplitCh = ' '; break;
    case ssfWhiteSep: SplitCh = ' '; break;
    default: FailR("Unknown separator character.");
  }
}

TSsParser::TSsParser(const TStr& FNm, const char& Separator, const bool& _SkipLeadBlanks, const bool& _SkipCmt, const bool& _SkipEmptyFld) : SsFmt(ssfSpaceSep), 
 SkipLeadBlanks(_SkipLeadBlanks), SkipCmt(_SkipCmt), SkipEmptyFld(_SkipEmptyFld), LineCnt(0), /*Bf(NULL),*/ SplitCh('\t'), FldV(), FInPt(NULL) {
  if (TZipIn::IsZipExt(FNm.GetFExt())) { FInPt = TZipIn::New(FNm); }
  else { FInPt = TFIn::New(FNm); }
  SplitCh = Separator;
}

TSsParser::~TSsParser() {
  //if (Bf != NULL) { delete [] Bf; }
}

/*bool TSsParser::Next() { // split on SplitCh
  const char* EndBf = Bf+BfLen-1;
  memset(Bf, 0, BfLen);
  char *cur = Bf, *last = Bf;
  FldV.Clr(false);
  TSIn& FIn = *FInPt;
  if (SkipLeadBlanks) { // skip leadning blanks
    while (! FIn.Eof() && cur < EndBf && (FIn.PeekCh()=='\t' || FIn.PeekCh()==' ')) { FIn.GetCh(); } 
  }
  while (! FIn.Eof() && cur < EndBf) {
    if (SsFmt == ssfWhiteSep) {
      while (! FIn.Eof() && cur < EndBf && ! TCh::IsWs(*cur=FIn.GetCh())) { cur++; }
    } else {
      while (! FIn.Eof() && cur < EndBf && (*cur=FIn.GetCh())!=SplitCh && *cur!='\r' && *cur!='\n') { cur++; }
    }
    if (*cur=='\r' || *cur=='\n') {
      *cur = 0; cur++;
      if (*last) { FldV.Add(last); }
      last = cur;
      break;
    }
    *cur = 0;  cur++;
    FldV.Add(last);  last = cur;
    if (SkipEmpty && strlen(FldV.Last())==0) { FldV.DelLast(); }
  }
  if (SkipEmpty && FldV.Len()>0 && strlen(FldV.Last())==0) { 
    FldV.DelLast(); 
  }
  LineCnt++;
  if (! FldV.Empty() && cur < EndBf) { 
    if (SkipCmt && IsCmt()) { return Next(); }
    else { return true; } }
  else if (! FIn.Eof() && ! SkipEmpty) { return true; }
  else { return false; }
}*/

bool TSsParser::Next() { // split on SplitCh
  FldV.Clr(false);
  LineStr.Clr();
  FldV.Clr();
  LineCnt++;
  if (! FInPt->GetNextLn(LineStr)) { return false; }
  if (SkipCmt && LineStr.Len()>0 && LineStr[0]=='#') { return Next(); }

  char* cur = LineStr.CStr();
  if (SkipLeadBlanks) { // skip leadning blanks
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

  //const char* EndBf = Bf+BfLen-1;
  //memset(Bf, 0, BfLen);
  //char *cur = Bf, *last = Bf;
  /*if (SkipLeadBlanks) { // skip leadning blanks
    while (! FIn.Eof() && cur < EndBf && (FIn.PeekCh()=='\t' || FIn.PeekCh()==' ')) { FIn.GetCh(); } 
  }
  while (! FIn.Eof() && cur < EndBf) {
    if (SsFmt == ssfWhiteSep) {
      while (! FIn.Eof() && cur < EndBf && ! TCh::IsWs(*cur=FIn.GetCh())) { cur++; }
    } else {
      while (! FIn.Eof() && cur < EndBf && (*cur=FIn.GetCh())!=SplitCh && *cur!='\r' && *cur!='\n') { cur++; }
    }
    if (*cur=='\r' || *cur=='\n') {
      if (! FIn.Eof()) { // read the remaining of the line
        if (*cur == '\r' && FIn.PeekCh()=='\n') { FIn.GetCh(); }
        else if (*cur == '\n' && FIn.PeekCh()=='\r') { FIn.GetCh(); }
      }
      *cur = 0; cur++;
      FldV.Add(last);
      last = cur;
      break;
    }
    *cur = 0;  cur++;
    FldV.Add(last);  last = cur;
    if (SkipEmptyFld && strlen(FldV.Last())==0) { FldV.DelLast(); } // skip empty fields
  }
  LineCnt++;
  if (SkipCmt && IsCmt() && ! FIn.Eof()) { return Next(); }
  if (FldV.Len() == 1 && strlen(FldV[0])==0) { FldV.Clr(); return true; }
  if (SkipEmptyFld && FldV.Len()>0 && strlen(FldV.Last())==0) { FldV.DelLast(); }
  return ! FIn.Eof() || ! FldV.Empty();
  //if (SkipEmptyFld && FldV.Empty() && ! FIn.Eof()) { return Next(); } // skip empty line
  */
}

void TSsParser::ToLc() {
  for (int f = 0; f < FldV.Len(); f++) {
    for (char *c = FldV[f]; *c; c++) {
      *c = tolower(*c); }
  }
}

bool TSsParser::GetInt(const int& FldN, int& Val) const {
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
  if (Minus) { _Val =- _Val; }
  if (*c != 0) { return false; }
  Val = _Val;
  return true;
}

bool TSsParser::GetFlt(const int& FldN, double& Val) const {
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

const char* TSsParser::DumpStr() const {
  static TChA ChA(10*1024);
  ChA.Clr();
  for (int i = 0; i < FldV.Len(); i++) {
    ChA += TStr::Fmt("  %d: '%s'\n", i, FldV[i]);
  }
  return ChA.CStr();
}

