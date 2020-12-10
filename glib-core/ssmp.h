#include "bd.h"

//#//////////////////////////////////////////////
/// TSsParser for Parallel Load. ##TSsParserMP
ClassTP(TSsParserMP, PSsParserMP)//{
private:
  TSsFmt SsFmt;  ///< Separator type.
  bool SkipLeadBlanks;  ///< Ignore leading whitespace characters in a line.
  bool SkipCmt;         ///< Skip comments (lines starting with #).
  bool SkipEmptyFld;    ///< Skip empty fields (i.e., multiple consecutive separators are considered as one).
  uint64 LineCnt;       ///< Number of processed lines so far.
  char SplitCh;         ///< Separator character (if one of the non-started separators is used)
  TChA LineStr;         ///< Current line.
  TVec<char*> FldV;     ///< Pointers to fields of the current line.
  TPt<TMIn> FInPt;      ///< Pointer to the input file stream.
  UndefDefaultCopyAssign(TSsParserMP);
public:
  /// Constructor. ##TSsParserMP::TSsParserMP1
  TSsParserMP(const TStr& FNm, const TSsFmt _SsFmt=ssfTabSep, const bool& _SkipLeadBlanks=false, const bool& _SkipCmt=true, const bool& _SkipEmptyFld=false);
  /// Constructor. ##TSsParserMP::TSsParserMP2
  TSsParserMP(const TStr& FNm, const char& Separator, const bool& _SkipLeadBlanks=false, const bool& _SkipCmt=true, const bool& _SkipEmptyFld=false);
  ~TSsParserMP();
  static PSsParserMP New(const TStr& FNm, const TSsFmt SsFmt) { return new TSsParserMP(FNm, SsFmt); }

  /// Loads next line from the input file. ##TSsParserMP::Next
  bool Next();
  /// Loads next line from the input file (older, slow implementation - deprecated). ##TSsParserMP::NextSlow
  bool NextSlow();
  /// Returns the number of fields in the current line.
  int Len() const { return FldV.Len(); }
  /// Returns the number of fields in the current line.
  int GetFlds() const { return Len(); }
  /// Returns the line number of the current line.
  uint64 GetLineNo() const { return LineCnt; }
  /// Checks whether the current line is a comment (starts with '#').
  bool IsCmt() const { return Len()>0 && GetFld(0)[0] == '#'; }
  /// Skips lines that begin with a comment character
  void SkipCommentLines();
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
  /// Assumes \c FldN is a floating  point number its value is returned. If \c FldN is not an integer an exception is thrown.
  double GetFlt(const int& FldN) const {
    double Val=0.0; IAssert(GetFlt(FldN, Val)); return Val; }
  
  /// Returns position of stream pointer
  uint64 GetStreamPos() const {
    return FInPt->GetBfC(); }

  /// Returns length of stream
  uint64 GetStreamLen() const {
    return FInPt->GetBfL(); }

  /// Sets position of stream pointer
  void SetStreamPos(uint64 Pos) {
    return FInPt->SetBfC(Pos); }

  /// Counts number of occurences of '\n' in [Lb, Ub)
  uint64 CountNewLinesInRange(uint64 Lb, uint64 Ub) const;

  /// Finds start positions of all lines ending somewhere in [Lb, Ub)
  TVec<uint64> GetStartPosV(uint64 Lb, uint64 Ub) const;

  /// Loads next line starting from a given position
  void NextFromIndex(uint64 Index, TVec<char*>& FieldsV);

  /// Gets integer at field \c FldN
  int GetIntFromFldV(TVec<char*>& FieldsV, const int& FldN);

  /// Gets float at field \c FldN
  double GetFltFromFldV(TVec<char*>& FieldsV, const int& FldN);

  const char* DumpStr() const;
};
