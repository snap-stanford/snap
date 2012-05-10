/////////////////////////////////////////////////
// Mathmatical-Errors
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__==0x0530)
int std::_matherr(struct math_exception* e){
  e->retval=0;
  return 1;
}
#elif defined(GLib_GLIBC) || defined(GLib_BSD)
int _matherr(struct __exception* e){
  e->retval=0;
  return 1;
}
#elif defined(GLib_SOLARIS)
int _matherr(struct __math_exception* e){
  e->retval=0;
  return 1;
}
#elif defined(GLib_CYGWIN)
int matherr(struct __exception *e){
  e->retval=0;
  return 1;
}
#elif defined(GLib_MACOSX)
//int matherr(struct exception *e) {
//  e->retval=0;
//  return 1;
//}
#else
int _matherr(struct _exception* e){
  e->retval=0;
  return 1;
}
#endif

/////////////////////////////////////////////////
// Messages
void WrNotify(const char* CaptionCStr, const char* NotifyCStr){
#if defined(__CONSOLE__) || defined(_CONSOLE)
  printf("*** %s: %s\n", CaptionCStr, NotifyCStr);
#else
  MessageBox(NULL, NotifyCStr, CaptionCStr, MB_OK);
#endif
}

void SaveToErrLog(const char* MsgCStr){
  int MxFNmLen=1000;
  char* FNm=new char[MxFNmLen]; if (FNm==NULL){return;}
  int FNmLen=GetModuleFileName(NULL, FNm, MxFNmLen); if (FNmLen==0){return;}
  FNm[FNmLen++]='.'; FNm[FNmLen++]='E'; FNm[FNmLen++]='r'; FNm[FNmLen++]='r';
  FNm[FNmLen++]=char(0);
  time_t Time=time(NULL);
  FILE* fOut=fopen(FNm, "a+b"); if (fOut==NULL){return;}
  fprintf(fOut, "--------\r\n%s\r\n%s%s\r\n--------\r\n",
   FNm, ctime(&Time), MsgCStr);
  fclose(fOut);
  delete[] FNm;
}

/////////////////////////////////////////////////
// Assertions
TOnExeStop::TOnExeStopF TOnExeStop::OnExeStopF=NULL;

void ExeStop(
 const char* MsgCStr, const char* ReasonCStr,
 const char* CondCStr, const char* FNm, const int& LnN){
  char ReasonMsgCStr[1000];
  // construct reason message
  if (ReasonCStr==NULL){ReasonMsgCStr[0]=0;}
  else {sprintf(ReasonMsgCStr, " [Reason:'%s']", ReasonCStr);}
  // construct full message
  char FullMsgCStr[1000];
  if (MsgCStr==NULL){
    if (CondCStr==NULL){
      sprintf(FullMsgCStr, "Execution stopped%s!", ReasonMsgCStr);
    } else {
      sprintf(FullMsgCStr, "Execution stopped: %s%s, file %s, line %d",
       CondCStr, ReasonMsgCStr, FNm, LnN);
    }
  } else {
    if (CondCStr==NULL){
      sprintf(FullMsgCStr, "%s\nExecution stopped!", MsgCStr);
    } else {
      sprintf(FullMsgCStr, "Message: %s%s\nExecution stopped: %s, file %s, line %d",
       MsgCStr, ReasonMsgCStr, CondCStr, FNm, LnN);
    }
  }
  // report full message to log file
  SaveToErrLog(FullMsgCStr);
  // report to screen & stop execution
  bool Continue=false;
  // call handler
  if (TOnExeStop::IsOnExeStopF()){
    Continue=!((*TOnExeStop::GetOnExeStopF())(FullMsgCStr));}
  if (!Continue){
    ErrNotify(FullMsgCStr);
#ifdef GLib_WIN32
    abort();
    //ExitProcess(1);
#else
    exit(1);
#endif
  }
}
