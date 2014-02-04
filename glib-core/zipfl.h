#ifndef zipfl_h
#define zipfl_h

//#//////////////////////////////////////////////
/// Compressed File Input Stream. The class reads from a compressed file without explicitly uncompressing it.
/// This is eachieved by running external 7ZIP program which uncompresses to standard output, which is then piped to TZipFl.
/// The class requires 7ZIP to be installed on the machine. Go to http://www.7-zip.org to install the software.
/// 7z (7z.exe) is an executable and can decompress the following formats: .gz, .7z, .rar, .zip, .cab, .arj. bzip2.
/// The class TZipIn expects that '7z' ('7z.exe') is in the working path. Make sure you can execute '7z e -y -bd -so <FILENAME>'
/// For 7z to work properly you need both the 7z executable and the directory 'Codecs'.
/// Use TZipIn::SevenZipPath to set the path to 7z executable.
///
/// NOTE: Current implementation of TZipIn supports only .zip format, other compression formats are not supported.
// Obsolete note (RS 2014/01/29): You can only load .gz files of uncompressed size <2GB. If you load some other format (like .bz2 or rar) there is no such limitation.
class TZipIn : public TSIn {
public:
  static TStr SevenZipPath;
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
  int FindEol(int& BfN);
  void CreateZipProcess(const TStr& Cmd, const TStr& ZipFNm);
  static void FillFExtToCmdH();
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
  bool GetNextLnBf(TChA& LnChA);

  uint64 GetFLen() const { return FLen; }
  uint64 GetCurFPos() const { return CurFPos; }

  /// Check whether the file extension of FNm is that of a compressed file (.gz, .7z, .rar, .zip, .cab, .arj. bzip2).
  static bool IsZipFNm(const TStr& FNm) { return IsZipExt(FNm.GetFExt()); }
  /// Check whether the file extension FNmExt is that of a compressed file (.gz, .7z, .rar, .zip, .cab, .arj. bzip2).
  static bool IsZipExt(const TStr& FNmExt);
  /// Return a command-line string that is executed in order to decompress a file to standard output. 
  static TStr GetCmd(const TStr& ZipFNm);
  /// Return the uncompressed size (in bytes) of the compressed file ZipFNm.
  static uint64 GetFLen(const TStr& ZipFNm);
  static PSIn NewIfZip(const TStr& FNm) { return IsZipFNm(FNm) ? New(FNm) : TFIn::New(FNm); }
};

//#//////////////////////////////////////////////
/// Compressed File Output Stream. The class directly writes to a compressed file.
/// This is eachieved by TZipFl outputing into a pipe from which 7ZIP then reads and compresses.
/// The class requires 7ZIP to be installed on the machine. Go to http://www.7-zip.org to install the software.
/// 7z (7z.exe) is an executable and can decompress the following formats: .gz, .7z, .rar, .zip, .cab, .arj. bzip2.
/// The class TZIpOut expects that '7z' ('7z.exe') is in the working path.
/// Note2: For 7z to work properly you need both the 7z executable and the directory 'Codecs'.
/// Note3: Use TZipIn::SevenZipPath to set the path to 7z executable.
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
  static void FillFExtToCmdH();
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

  /// Check whether the file extension of FNm is that of a compressed file (.gz, .7z, .rar, .zip, .cab, .arj. bzip2).
  static bool IsZipFNm(const TStr& FNm) { return IsZipExt(FNm.GetFExt()); }
  /// Check whether the file extension FNmExt is that of a compressed file (.gz, .7z, .rar, .zip, .cab, .arj. bzip2).
  static bool IsZipExt(const TStr& FNmExt);
  /// Return a command-line string that is executed in order to decompress a file to standard output. 
  static TStr GetCmd(const TStr& ZipFNm);
  static PSOut NewIfZip(const TStr& FNm) { return IsZipFNm(FNm) ? New(FNm) : TFOut::New(FNm); }
};

#endif
