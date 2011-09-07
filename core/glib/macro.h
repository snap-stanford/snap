/////////////////////////////////////////////////
// Macro-Processor
ClassTP(TMacro, PMacro)//{
public:
  bool Ok;
  TStr MsgStr;
  char MacroCh;
  char VarCh;
  TStr TxtStr;
  TStrStrH SubstToValStrH;
  TStrStrH VarNmToValStrH;
public:
  TMacro(const TStr& TxtStr, const char& _MacroCh='$', const char& _VarCh='#');
  static PMacro New(
   const TStr& TxtStr, const char& MacroCh='$', const char& VarCh='#'){
    return PMacro(new TMacro(TxtStr, MacroCh, VarCh));}
  ~TMacro(){}
  TMacro(TSIn&){Fail;}
  static PMacro Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&) const {Fail;}

  TMacro& operator=(const TMacro&){Fail; return *this;}

  bool IsOk() const {return Ok;}
  TStr GetMsgStr() const {return MsgStr;}

  TStr GetSrcTxtStr() const {return TxtStr;}
  TStr GetDstTxtStr() const;

  int GetSubstStrs() const {return SubstToValStrH.Len();}
  TStr GetSrcSubstStr(const int& SubstStrN) const {
    return SubstToValStrH.GetKey(SubstStrN);}
  bool IsSrcSubstStr(const TStr& SrcSubstStr, int& SubstStrN) const {
    return SubstToValStrH.IsKey(SrcSubstStr, SubstStrN);}
  int GetSrcSubstStrN(const TStr& SrcSubstStr) const {
    return SubstToValStrH.GetKeyId(SrcSubstStr);}
  void GetSrcSubstStrV(TStrV& SubstStrV) const;
  TStr GetDstSubstStr(const int& SubstStrN=0) const;
  void PutSubstValStr(const int& SubstStrN, const TStr& ValStr){
    SubstToValStrH[SubstStrN]=ValStr;}
  TStr GetSubstValStr(const int& SubstStrN) const {
    return SubstToValStrH[SubstStrN];}
  TStr GetAllSubstValStr() const;

  int GetVars() const {return VarNmToValStrH.Len();}
  TStr GetVarNm(const int& VarN) const {return VarNmToValStrH.GetKey(VarN);}
  void GetVarNmV(TStrV& VarNmV) const;
  void PutVarVal(const TStr& VarNm, const TStr& ValStr){
    VarNmToValStrH.AddDat(VarNm, ValStr);}
  TStr GetVarVal(const TStr& VarNm) const {
    return VarNmToValStrH.GetDat(VarNm);}

  static void SplitVarNm(
   const TStr& VarNm, TStr& CapStr,
   bool& IsComboBox, TStr& TbNm, TStr& ListFldNm, TStr& DataFldNm);
};

