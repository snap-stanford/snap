/////////////////////////////////////////////////
// Console Output
TCon::TCon() {
#if defined (GLib_CreateConsole)
  Ok = (AllocConsole() != 0);
  HStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
  IAssert(HStdOut != INVALID_HANDLE_VALUE);
#endif
}

TCon::~TCon() {
#if defined (GLib_CreateConsole)
  if (Ok) IAssert(FreeConsole());
#endif
}

void TCon::PutBf(const void *LBf, const int& LBfL) {
#if defined (GLib_Console)
  #if defined (GLib_CreateConsole)
    DWORD ChsWritten;
    WriteConsole(HStdOut, LBf, LBfL, &ChsWritten, 0);
    IAssert(ChsWritten == static_cast<DWORD>(LBfL));
  #else
    fwrite(LBf, sizeof(char), LBfL, stdout);
  #endif
#endif
}

TCon& TCon::operator << (const int& Int) {
  char Bf[255];
  sprintf(Bf, "%d", Int);
  PutBf((void *) Bf, int(strlen(Bf)));
  return *this;
}

TCon& TCon::operator << (const uint& UInt) {
  char Bf[255];
  sprintf(Bf, "%u", UInt);
  PutBf((void *) Bf, int(strlen(Bf)));
  return *this;
}

TCon& TCon::operator << (const float& Flt) {
  char Bf[255];
  sprintf(Bf, "%g", Flt);
  PutBf((void *) Bf, int(strlen(Bf)));
  return *this;

}

TCon& TCon::operator << (const double& Double) {
  char Bf[255];
  sprintf(Bf, "%g", Double);
  PutBf((void *) Bf, int(strlen(Bf)));
  return *this;
}

TCon& TCon::operator << (const long double& LDouble) {
  char Bf[255];
  sprintf(Bf, "%Lg", LDouble);
  PutBf((void *) Bf, int(strlen(Bf)));
  return *this;
}

void TCon::operator () (const char * FmtStr, ...) {
  static char Bf [2048];
  va_list valist;  va_start(valist, FmtStr);
  int BfL=vsnprintf(Bf, 2048, FmtStr, valist); va_end(valist);
  if (BfL!=-1){PutBf((void *) Bf, BfL);}
  else {PutBf((void *) Bf, 2048);}
}

TCon& Eol(TCon& Con) {
  Con.PutCh('\n'); return Con;
}

TCon& Tab(TCon& Con) {
  Con.PutCh('\t');  return Con;
}

TCon& Spc(TCon& Con) {
  Con.PutCh(' ');  return Con;
}

#if defined (GLib_Console)
  TCon Con;
#endif
