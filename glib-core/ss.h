#include "bd.h"

//#//////////////////////////////////////////////
/// Spread-Sheet Separator Format.
typedef enum {ssfUndef,
  ssfTabSep,       ///< Tab separated
  ssfCommaSep,     ///< Comma separated
  ssfSemicolonSep, ///< Semicolon separated
  ssfVBar,         ///< Vertical bar separated
  ssfSpaceSep,     ///< Space separated
  ssfWhiteSep,     ///< Whitespace (space or tab) separated
  ssfMx} TSsFmt;

//#//////////////////////////////////////////////
/// Small Spread-Sheet Parser. ##TSs
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

//#//////////////////////////////////////////////
/// Fast Spread Sheet Parser. ##TSsParser
ClassTP(TSsParser, PSsParser)//{
private:
  TSsFmt SsFmt;  ///< Separator type.
  bool SkipLeadBlanks;  ///< Ignore leading whitespace characters in a line.
  bool SkipCmt;         ///< Skip comments (lines starting with #).
  bool SkipEmptyFld;    ///< Skip empty fields (i.e., multiple consecutive separators are considered as one).
  uint64 LineCnt;       ///< Number of processed lines so far.
  char SplitCh;         ///< Separator character (if one of the non-started separators is used)
  TChA LineStr;         ///< Current line.
  TVec<char*> FldV;     ///< Pointers to fields of the current line.
  PSIn FInPt;           ///< Pointer to the input file stream.
  UndefDefaultCopyAssign(TSsParser);
public:
  /// Constructor. ##TSsParser::TSsParser1
  TSsParser(const TStr& FNm, const TSsFmt _SsFmt=ssfTabSep, const bool& _SkipLeadBlanks=false, const bool& _SkipCmt=true, const bool& _SkipEmptyFld=false);
  /// Constructor. ##TSsParser::TSsParser2
  TSsParser(const TStr& FNm, const char& Separator, const bool& _SkipLeadBlanks=false, const bool& _SkipCmt=true, const bool& _SkipEmptyFld=false);
  ~TSsParser();
  static PSsParser New(const TStr& FNm, const TSsFmt SsFmt) { return new TSsParser(FNm, SsFmt); }

  /// Loads next line from the input file. ##TSsParser::Next
  bool Next();
  /// Loads next line from the input file (older, slow implementation - deprecated). ##TSsParser::NextSlow
  bool NextSlow();
  /// Returns the number of fields in the current line.
  int Len() const { return FldV.Len(); }
  /// Returns the number of fields in the current line.
  int GetFlds() const { return Len(); }
  /// Returns the line number of the current line.
  uint64 GetLineNo() const { return LineCnt; }
  /// Checks whether the current line is a comment (starts with '#').
  bool IsCmt() const { return Len()>0 && GetFld(0)[0] == '#'; }
  /// Checks for end of file.
  bool Eof() const { return FInPt->Eof(); }
  /// Returns the current line
  TChA GetLnStr() const { TChA LnOut;  for (int i = 0; i < Len(); i++) { LnOut+=GetFld(i); LnOut+=' '; }  if (LnOut.Len() > 0) LnOut.DelLastCh();  return LnOut; }
  /// Transforms the current line to lower case.
  void ToLc();

  /// Returns the contents of the field at index \c FldN.
  const char* GetFld(const int& FldN) const { return FldV[FldN]; }
  /// Returns the contents of the field at index \c FldN.
  char* GetFld(const int& FldN) { return FldV[FldN]; }
  /// Returns the contents of the field at index \c FldN.
  const char* operator [] (const int& FldN) const { return FldV[FldN]; }
  /// Returns the contents of the field at index \c FldN.
  char* operator [] (const int& FldN) { return FldV[FldN]; }

  /// If the field \c FldN is an integer its value is returned in \c Val and the function returns \c true.
  bool GetInt(const int& FldN, int& Val) const;
  /// Assumes \c FldN is an integer its value is returned. If \c FldN is not an integer an exception is thrown.
  int GetInt(const int& FldN) const {
    int Val=0; IAssertR(GetInt(FldN, Val), TStr::Fmt("Field %d not INT.\n%s", FldN, DumpStr()).CStr()); return Val; }
  /// Checks whether fields \c FldN is an integer.  
  bool IsInt(const int& FldN) const { int v; return GetInt(FldN, v); }

  /// If the field \c FldN is a float its value is returned in \c Val and the function returns \c true.
  bool GetFlt(const int& FldN, double& Val) const;
  /// Checks whether fields \c FldN is a float. 
  bool IsFlt(const int& FldN) const { double v; return GetFlt(FldN, v); }
  /// Assumes \c FldN is a floating point number its value is returned. If \c FldN is not an integer an exception is thrown.
  double GetFlt(const int& FldN) const { double Val=0.0; IAssert(GetFlt(FldN, Val)); return Val; }

  /// If the field \c FldN is a 64-bit unsigned integer its value is returned in \c Val and the function returns \c true.
  bool GetUInt64(const int& FldN, uint64& Val) const;
  /// Checks whether fields \c FldN is unsigned 64-bit integer number.
  bool IsUInt64(const int& FldN) const { uint64 v; return GetUInt64(FldN, v); }
  /// Assumes \c FldN is a 64-bit unsigned integer point number its value is returned. If \c FldN is not a 64-bit unsigned integer an exception is thrown.
  uint64 GetUInt64(const int& FldN) const { uint64 Val=0; IAssert(GetUInt64(FldN, Val)); return Val; }

  const char* DumpStr() const;
};

