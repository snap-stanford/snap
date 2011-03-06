/////////////////////////////////////////////////
// Spread-Sheet
typedef enum {ssfUndef,
 ssfTabSep, ssfCommaSep, ssfSemicolonSep, ssfSpaceSep, ssfWhiteSep, ssfMx} TSsFmt;

ClassTP(TSs, PSs)//{
private:
  TVec<PStrV> CellStrVV;
public:
  TSs(): CellStrVV(){}
  static PSs New(){return PSs(new TSs());}
  ~TSs(){}
  TSs(TSIn& SIn): CellStrVV(SIn){}
  static PSs Load(TSIn& SIn){return new TSs(SIn);}
  void Save(TSOut& SOut){CellStrVV.Save(SOut);}

  TSs& operator=(const TSs& Ss){
    if (this!=&Ss){CellStrVV=Ss.CellStrVV;} return *this;}

  // values
  TStr& At(const int& X, const int& Y);
  void PutVal(const int& X, const int& Y, const TStr& Str);
  TStr GetVal(const int& X, const int& Y) const;

  // row & column
  int GetXLen() const;
  int GetXLen(const int& Y) const;
  int GetYLen() const;
  void DelX(const int& X);
  void DelY(const int& Y);
  int SearchX(const int& Y, const TStr& Str) const;
  int SearchY(const int& X, const TStr& Str) const;

  // fields
  int GetFlds() const {return GetXLen(0);}
  int GetFldX(const TStr& FldNm, const TStr& NewFldNm="", const int& Y=0) const;
  int GetFldY(const TStr& FldNm, const TStr& NewFldNm="", const int& X=0) const;
  TStr GetFldNm(const int& FldX) const {return GetVal(FldX, 0);}

  // files
  static PSs LoadTxt(
   const TSsFmt& SsFmt, const TStr& FNm,
   const PNotify& Notify=NULL, const bool& IsExcelEoln=true,
   const int& MxY=-1, const TIntV& AllowedColNV=TIntV(), const bool& IsQStr=true);
  void SaveTxt(const TStr& FNm, const PNotify& Notify=NULL) const;
  static void LoadTxtFldV(
   const TSsFmt& SsFmt, const PSIn& SIn, char& Ch,
   TStrV& FldValV, const bool& IsExcelEoln=true, const bool& IsQStr=true);

  // format
  static TSsFmt GetSsFmtFromStr(const TStr& SsFmtNm);
  static TStr GetStrFromSsFmt(const TSsFmt& SsFmt);
  static TStr GetSsFmtNmVStr();
};

/////////////////////////////////////////////////
// Fast-Spread-Sheet-Parser
ClassTP(TSsParser, PSsParser)//{
private:
  TSsFmt SsFmt;
  bool SkipLeadBlanks, SkipCmt, SkipEmptyFld; // skip emptry fields
  uint64 LineCnt;
  char SplitCh;
  TChA LineStr;
  TVec<char*> FldV;
  PSIn FInPt;
  UndefDefaultCopyAssign(TSsParser);
public:
  TSsParser(const TStr& FNm, const TSsFmt _SsFmt, const bool& _SkipLeadBlanks=false, const bool& _SkipCmt=true, const bool& _SkipEmptyFld=false);
  TSsParser(const TStr& FNm, const char& Separator, const bool& _SkipLeadBlanks=false, const bool& _SkipCmt=true, const bool& _SkipEmptyFld=false);
  ~TSsParser();
  static PSsParser New(const TStr& FNm, const TSsFmt SsFmt) { return new TSsParser(FNm, SsFmt); }

  bool Next();
  int Len() const { return FldV.Len(); }
  int GetFlds() const { return Len(); }
  uint64 GetLineNo() const { return LineCnt; }
  bool IsCmt() const { return Len()>0 && GetFld(0)[0] == '#'; }
  bool Eof() const { return FInPt->Eof(); }
  const TChA& GetLnStr() const { return LineStr; }
  void ToLc();

  const char* GetFld(const int& FldN) const { return FldV[FldN]; }
  char* GetFld(const int& FldN) { return FldV[FldN]; }
  const char* operator [] (const int& FldN) const { return FldV[FldN]; }
  char* operator [] (const int& FldN) { return FldV[FldN]; }
  bool GetInt(const int& FldN, int& Val) const;
  int GetInt(const int& FldN) const {
    int Val=0; IAssertR(GetInt(FldN, Val), TStr::Fmt("Field %d not INT.\n%s", FldN, DumpStr()).CStr()); return Val; }
  bool IsInt(const int& FldN) const { int v; return GetInt(FldN, v); }
  bool GetFlt(const int& FldN, double& Val) const;
  bool IsFlt(const int& FldN) const { double v; return GetFlt(FldN, v); }
  double GetFlt(const int& FldN) const {
    double Val=0.0; IAssert(GetFlt(FldN, Val)); return Val; }

  const char* DumpStr() const;
};

