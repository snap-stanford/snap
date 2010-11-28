/////////////////////////////////////////////////
// Environment
class TEnv{
private:
  //int Args;
  //char** ArgV;
  TStrV ArgV;
  TStr HdStr;
  int MnArgs;
  bool SilentP;
  PNotify Notify;
public:
  TEnv(): /*Args(0), ArgV(NULL)*/ ArgV(), HdStr(), MnArgs(1), SilentP(false), Notify(){}
  TEnv(const TEnv&);
  TEnv(const int& _Args, char** _ArgV, const PNotify& _Notify=NULL);
    //Args(_Args), ArgV(_ArgV), HdStr(), MnArgs(1), SilentP(false), Notify(_Notify){}
  TEnv(const TStr& _ArgStr, const PNotify& _Notify=NULL);

  TEnv& operator=(const TEnv& Env){
    /*Args=Env.Args;*/ ArgV=Env.ArgV; HdStr=Env.HdStr;
    MnArgs=Env.MnArgs; SilentP=Env.SilentP; Notify=Env.Notify; return *this;}

  // basic arguments & command line
  //int GetArgs() const {return Args;}
  int GetArgs() const {return ArgV.Len();}
  //TStr GetArg(const int& ArgN) const {
  //  IAssert((0<=ArgN)&&(ArgN<Args)); return TStr(ArgV[ArgN]);}
  TStr GetArg(const int& ArgN) const {
    IAssert((0<=ArgN)&&(ArgN<GetArgs())); return ArgV[ArgN];}
  TStr GetExeFNm() const;
  TStr GetExeFPath() const {return GetExeFNm().GetFPath();}
  TStr GetCmLn(const int& FromArgN=1) const;
  bool IsSilent() const {return SilentP;}

  // arguments prefix & postfix
  int GetPrefixArgN(const TStr& PrefixStr) const;
  bool IsArgPrefix(const TStr& PrefixStr) const {
    return GetPrefixArgN(PrefixStr)!=-1;}
  TStr GetArgPostfix(const TStr& PrefixStr) const;

  // top level arguments handling
  void PrepArgs(
   const TStr& _HdStr=TStr(), const int& _MnArgs=1, const bool& SilentP=false);
  TStr GetHdStr() const {return HdStr;}
  int GetMnArgs() const {return MnArgs;}
  bool IsEndOfRun() const;

  // arguments value retrieval
  TStr GetIfArgPrefixStr(
   const TStr& PrefixStr, const TStr& DfVal="", const TStr& DNm="") const;
  TStrV GetIfArgPrefixStrV(
   const TStr& PrefixStr, TStrV& DfValV, const TStr& DNm="") const;
  TStrV GetIfArgPrefixStrV(const TStr& PrefixStr, const TStr& DNm="") const {
    TStrV DfValV; return GetIfArgPrefixStrV(PrefixStr, DfValV, DNm);}
  bool GetIfArgPrefixBool(
   const TStr& PrefixStr, const bool& DfVal=false, const TStr& DNm="") const;
  int GetIfArgPrefixInt(
   const TStr& PrefixStr, const int& DfVal=0, const TStr& DNm="") const;
  TIntV GetIfArgPrefixIntV(
   const TStr& PrefixStr, TIntV& DfValV, const TStr& DNm="") const;
  TIntV GetIfArgPrefixIntV(const TStr& PrefixStr, const TStr& DNm="") const {
    TIntV DfValV; return GetIfArgPrefixIntV(PrefixStr, DfValV, DNm);}
  double GetIfArgPrefixFlt(
   const TStr& PrefixStr, const double& DfVal=0, const TStr& DNm="") const;

  // environment variables
  static void PutVarVal(const TStr& VarNm, const TStr& VarVal);
  TStr GetVarVal(const TStr& VarNm) const;
  void GetVarNmV(TStrV& VarNmV);
  void GetVarNmValV(TStrV& NmValV);
};

extern TEnv Env;

