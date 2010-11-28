/////////////////////////////////////////////////
// Notifications
TStr TNotify::GetTypeStr(
 const TNotifyType& Type, const bool& Brief){
  static TStr InfoSStr="I"; static TStr InfoLStr="Information";
  static TStr WarnSStr="W"; static TStr WarnLStr="Warning";
  static TStr ErrSStr="E"; static TStr ErrLStr="Error";
  static TStr StatSStr=""; static TStr StatLStr="Status";
  switch (Type){
    case ntInfo: if (Brief){return InfoSStr;} else {return InfoLStr;}
    case ntWarn: if (Brief){return WarnSStr;} else {return WarnLStr;}
    case ntErr: if (Brief){return ErrSStr;} else {return ErrLStr;}
    case ntStat: if (Brief){return StatSStr;} else {return StatLStr;}
    default: Fail; return TStr();
  }
}

void TNotify::DfOnNotify(const TNotifyType& Type, const TStr& MsgStr){
  switch (Type){
    case ntInfo: InfoNotify(MsgStr); break;
    case ntWarn: WarnNotify(MsgStr); break;
    case ntErr: ErrNotify(MsgStr); break;
    case ntStat: StatNotify(MsgStr); break;
    default: Fail;
  }
}

const PNotify TNotify::NullNotify=TNullNotify::New();
const PNotify TNotify::StdNotify=TStdNotify::New();

/////////////////////////////////////////////////
// Standard-Notifier
void TStdNotify::OnNotify(const TNotifyType& Type, const TStr& MsgStr){
  if (Type==ntInfo){
    printf("%s\n", MsgStr.CStr());
  } else {
    TStr TypeStr=TNotify::GetTypeStr(Type, false);
    printf("%s: %s\n", TypeStr.CStr(), MsgStr.CStr());
  }
}

void TStdNotify::OnStatus(const TStr& MsgStr){
  printf("%s", MsgStr.CStr());
  // print '\n' if message not overlayed
  if ((!MsgStr.Empty())&&(MsgStr.LastCh()!='\r')){
    printf("\n");}
}

/////////////////////////////////////////////////
// Exception
TExcept::TOnExceptF TExcept::OnExceptF=NULL;

