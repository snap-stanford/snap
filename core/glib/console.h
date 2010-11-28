// #define GLib_Console if you want Con output

class TCon;

/////////////////////////////////////////////////
// Console-Manipulator
class TConManip {
protected:
  TConManip() { }
  TConManip(const TConManip&);
  TConManip& operator = (const TConManip&);
  virtual ~TConManip();
public:
  virtual TCon& operator () (TCon& Con) const = 0;
};

/////////////////////////////////////////////////
// Console
class TCon {
private:
#if defined (GLib_CreateConsole)
  bool Ok;
  HANDLE HStdOut;
#endif

private:
  TCon(const TCon&);
  TCon& operator = (const TCon&);
public:
  TCon();
  ~TCon();

  int PutCh(const int& Ch) { PutBf((void *) &Ch, 1);  return Ch; }
  void PutBf(const void *LBf, const int& LBfL);
  void Flush() { }

  TCon& operator << (const bool& Bool) { PutCh(Bool ? 'T' : 'F'); return *this; }
  TCon& operator << (const uchar& UCh) { PutBf(&UCh, sizeof(UCh)); return *this; }
  TCon& operator << (const char& Ch) { PutBf(&Ch, sizeof(Ch)); return *this; }
  TCon& operator << (const int& Int);
  TCon& operator << (const uint& Int);
  TCon& operator << (const short& Sh) { operator<<((int) Sh); return *this; }
  TCon& operator << (const ushort& USh) { operator<<((uint) USh); return *this; }
  TCon& operator << (const float& Flt);
  TCon& operator << (const double& Double);
  TCon& operator << (const long double& LDouble);
  TCon& operator << (const char *CStr) { PutBf(CStr, int(strlen(CStr))); return *this; }
  TCon& operator << (const TStr& Str) { PutBf(Str.CStr(), Str.Len()); return *this; }
  TCon& operator << (const TChA& ChA) { PutBf(ChA.CStr(), ChA.Len()); return *this; }
  TCon& operator << (const TConManip& Mnp) { return Mnp(*this); }
  TCon& operator << (TCon& (*FuncPt)(TCon&)) { return FuncPt(*this); }

  void operator () (const char * FmtStr, ...);
};

TCon& Eol(TCon& Con);
TCon& Tab(TCon& Con);
TCon& Spc(TCon& Con);

#if defined (GLib_Console)
  extern TCon Con;
#endif

