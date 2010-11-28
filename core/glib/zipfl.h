#ifndef zipfl_h
#define zipfl_h

/////////////////////////////////////////////////
// Includes
//#include "base.h"

// #define TFileIn(FNm) 
//   (TZipIn::IsZipFNm((FNm)) ? TZipIn(FNm) : TFIn(FNm));
// #define TFileOut(FNm) 
//   TZipOut::IsZipFNm(FNm) ? TZipOut(FNm) : TFOut(FNm);

/////////////////////////////////////////////////
// Compressed input and output streams.
// 7za.exe or 7z.exe must be in the path (http://www.7-zip.org)
// 7za.exe is a stand-alone program, which supports
//   -- extraction:  .gz, .7z, .rar, .zip, .cab, .arj. bzip2
//   -- compression: .7z
// 7z.exe uses DLLs in folders Codecs and Formats
//   -- extraction:  .gz, .7z, .rar, .zip, .cab, .arj. bzip2
//   -- compression: .gz, .7z, .rar, .zip, .cab, .arj. bzip2
// Add this to Windows registry to locate Codecs and Formats:
//   [HKEY_LOCAL_MACHINE\SOFTWARE\7-Zip]
//   "Path"="C:\\Programs\\Util"

/////////////////////////////////////////////////
// ZIP Input-File (requires 7za.exe or 7z.exe)
class TZipIn : public TSIn {
private:
  static TStrStrH FExtToCmdH;
  static const int MxBfL;
  #ifdef GLib_WIN
    HANDLE ZipStdoutRd, ZipStdoutWr;
  #else 
    FILE* ZipStdoutRd, *ZipStdoutWr;
  #endif
  uint64 FLen, CurFPos;
  char* Bf;
  int BfC, BfL;
private:
  void FillBf();
  void CreateZipProcess(const TStr& Cmd, const TStr& ZipFNm);
private:
  TZipIn();
  TZipIn(const TZipIn&);
  TZipIn& operator=(const TZipIn&);
public:
  TZipIn(const TStr& FNm);
  TZipIn(const TStr& FNm, bool& OpenedP);
  static PSIn New(const TStr& FNm);
  static PSIn New(const TStr& FNm, bool& OpenedP);
  ~TZipIn();

  bool Eof() { return CurFPos==FLen && BfC==BfL; }
  int Len() const { return int(FLen-CurFPos+BfL-BfC); }
  char GetCh() { if (BfC==BfL){FillBf();} return Bf[BfC++]; }
  char PeekCh() { if (BfC==BfL){FillBf();} return Bf[BfC]; }
  int GetBf(const void* LBf, const TSize& LBfL);

  uint64 GetFLen() const { return FLen; }
  uint64 GetCurFPos() const { return CurFPos; }

  static void AddZipExtCmd(const TStr& ZipFNmExt, const TStr& ZipCmd);
  static bool IsZipFNm(const TStr& FNm) { return IsZipExt(FNm.GetFExt()); }
  static bool IsZipExt(const TStr& FNmExt);
  static void FillFExtToCmdH();
  static TStr GetCmd(const TStr& ZipFNm);
  static uint64 GetFLen(const TStr& ZipFNm);
};

/////////////////////////////////////////////////
// ZIP Output-File (requires 7z.exe and 7za.exe)
class TZipOut : public TSOut{
private:
  static const TSize MxBfL;
  static TStrStrH FExtToCmdH;
  #ifdef GLib_WIN
    HANDLE ZipStdinRd, ZipStdinWr;
  #else 
    FILE *ZipStdinRd, *ZipStdinWr;
  #endif
  char* Bf;
  TSize BfL;
private:
  void FlushBf();
  void CreateZipProcess(const TStr& Cmd, const TStr& ZipFNm);
private:
  TZipOut();
  TZipOut(const TZipOut&);
  TZipOut& operator=(const TZipOut&);
public:
  TZipOut(const TStr& _FNm);
  static PSOut New(const TStr& FNm);
  ~TZipOut();

  int PutCh(const char& Ch);
  int PutBf(const void* LBf, const TSize& LBfL);
  void Flush();

  static void AddZipExtCmd(const TStr& ZipFNmExt, const TStr& ZipCmd);
  static bool IsZipFNm(const TStr& FNm) { return IsZipExt(FNm.GetFExt()); }
  static bool IsZipExt(const TStr& FNmExt);
  static void FillFExtToCmdH();
  static TStr GetCmd(const TStr& ZipFNm);
};

#endif
