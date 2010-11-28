/////////////////////////////////////////////////
// Soap-Table
ClassTP(TSoapTb, PSoapTb)//{
protected:
  TStrH ColNmH;
  TVec<TStrKdV> ColNmValKdVV;
public:
  TSoapTb(const int& Cols=0, const int& Rows=0):
    ColNmH(Cols), ColNmValKdVV(Rows, 0){}
  TSoapTb(const TSoapTb& SoapTb):
    ColNmH(SoapTb.ColNmH), ColNmValKdVV(SoapTb.ColNmValKdVV){}
  static PSoapTb New(const int& Cols=0, const int& Rows=0){
    return new TSoapTb(Cols, Rows);}
  virtual ~TSoapTb(){}
  TSoapTb(TSIn& SIn):
    ColNmH(SIn), ColNmValKdVV(SIn){SIn.LoadCs();}
  static PSoapTb Load(TSIn& SIn){return new TSoapTb(SIn);}
  void Save(TSOut& SOut) const {
    ColNmH.Save(SOut); ColNmValKdVV.Save(SOut); SOut.SaveCs();}

  TSoapTb& operator=(const TSoapTb& SoapTb){
    ColNmH=SoapTb.ColNmH; ColNmValKdVV=SoapTb.ColNmValKdVV;
    return *this;}

  // table operations
  void Append(const PSoapTb& SoapTb);

  // columns
  int AddColNm(const TStr& ColNm){return ColNmH.AddKey(ColNm);}
  int GetCols() const {return ColNmH.Len();}
  bool IsColNm(const TStr& ColNm) const {return ColNmH.IsKey(ColNm);}
  TStr GetColNm(const int& ColN) const {return ColNmH.GetKey(ColN);}
  int GetColN(const TStr& ColNm) const {return ColNmH.GetKeyId(ColNm);}

  // rows & values
  virtual void OnRowChange(const int& RowN){}
  int AddRow(){
    int RowN=ColNmValKdVV.Add(); OnRowChange(RowN); return RowN;}
  void DelRow(const int& RowN){ColNmValKdVV.Del(RowN);}
  void ClrRow(const int& RowN){ColNmValKdVV[RowN].Clr();}
  int AddVal(const TStr& ColNm, const TStr& ValStr){
    ColNmH.AddKey(ColNm); OnRowChange(ColNmValKdVV.LastValN());
    return ColNmValKdVV.Last().AddUnique(TStrKd(ColNm, ValStr));}
  bool Empty() const {return GetRows()==0;}
  int GetRows() const {return ColNmValKdVV.Len();}
  int GetRowVals(const int& RowN) const {
    return ColNmValKdVV[RowN].Len();}
  void GetColNmVal(
   const int& RowN, const int& ValN, TStr& ColNm, TStr& ValStr) const {
    const TStrKd& ColNmValKd=ColNmValKdVV[RowN][ValN];
    ColNm=ColNmValKd.Key; ValStr=ColNmValKd.Dat;}
  bool IsGetColNmVal(
   const int& RowN, const int& ValN, TStr& ColNm, TStr& ValStr) const {
    if (ValN<ColNmValKdVV[RowN].Len()){
      GetColNmVal(RowN, ValN, ColNm, ValStr); return true;}
    else {ColNm.Clr(); ValStr.Clr(); return false;}}
  void PutVal(const int& RowN, const TStr& ColNm, const TStr& ValStr){
    IAssert((0<=RowN)&&(RowN<GetRows()));
    ColNmH.AddKey(ColNm); OnRowChange(RowN);
    ColNmValKdVV[RowN].AddUnique(TStrKd(ColNm, ValStr));}
  void PutVal(const TStr& ColNm, const TStr& ValStr){
    int LastRowN=GetRows()-1; IAssert(LastRowN>=0);
    PutVal(LastRowN, ColNm, ValStr);}
  TStr GetVal(const int& RowN, const TStr& ColNm) const {
    if ((0<=RowN)&&(RowN<GetRows())){
      int ValN=ColNmValKdVV[RowN].SearchForw(TStrKd(ColNm));
      return (ValN==-1) ? TStr("") : ColNmValKdVV[RowN][ValN].Dat;
    } else {
      return "";
    }}
  int SearchForw(const TStr& Val, const TStr& ColNm, const int& BegRowN=0){
    if (!IsColNm(ColNm)){return -1;}
    for (int RowN=BegRowN; RowN<GetRows(); RowN++){
      if (GetVal(RowN, ColNm)==Val){return RowN;}}
    return -1;
  }
  TStr GetVal(const int& RowN, const int& ColN) const {
    return GetVal(RowN, GetColNm(ColN));}
  void GetValV(const int& ColN, TStrV& ValStrV) const;
  void GetValPrV(const int& ColN1, const int& ColN2, TStrPrV& ValStrPrV) const;
  void GetValV(const TStr& ColNm, TStrV& ValStrV) const {
    GetValV(GetColN(ColNm), ValStrV);}
  void GetValPrV(const TStr& ColNm1, const TStr& ColNm2, TStrPrV& ValStrPrV) const {
    GetValPrV(GetColN(ColNm1), GetColN(ColNm2), ValStrPrV);}
  TTm GetValTm(const int& RowN, const TStr& ColNm) const {
    TStr TmStr=GetVal(RowN, GetColN(ColNm));
    return TTm::GetTmFromWebLogDateTimeStr(TmStr);}
  int GetRowN(const TStr& WhereColNm, const TStr& WhereValStr) const;

  // select emulation
  void SelectValStrV(const TStr& SelColNm, TStrV& SelValStrV,
   const TStr& WhereColNm, const TStr& WhereValStr) const;
  void SelectValStrV(const TStr& SelColNm, TStrV& SelValStrV,
   const TStr& WhereColNm1, const TStr& WhereValStr1,
   const TStr& WhereColNm2, const TStr& WhereValStr2) const;
  void SelectValStr(const TStr& SelColNm, TStr& SelValStr,
   const TStr& WhereColNm, const TStr& WhereValStr) const;
  void SelectValStr(const TStr& SelColNm, TStr& SelValStr,
   const TStr& WhereColNm1, const TStr& WhereValStr1,
   const TStr& WhereColNm2, const TStr& WhereValStr2) const;

  // files
  static PSoapTb LoadBin(const TStr& FNm){
    TFIn SIn(FNm); return Load(SIn);}
  void SaveBin(const TStr& FNm) const {
    TFOut SOut(FNm); Save(SOut);}
  void Dump(const TStr& FNm="Dump.Txt") const;
};
typedef THash<TStr, PSoapTb> TStrSoapTbH;

/////////////////////////////////////////////////
// Soap-Environment
ClassTP(TSoapEnv, PSoapEnv)//{
private:
  TStrStrH FldNmToValH;
  TStrStrVH FldNmToValVH;
  TStrSoapTbH FldNmToSoapTbH;
  void CopySoapTbH(const TSoapEnv& SoapEnv);
public:
  TSoapEnv(const TStrPrV& FldNmValPrV=TStrPrV());
  static PSoapEnv New(const TStrPrV& FldNmValPrV=TStrPrV()){
    return new TSoapEnv(FldNmValPrV);}
  TSoapEnv(const TSoapEnv& SoapEnv):
    FldNmToValH(SoapEnv.FldNmToValH),
    FldNmToValVH(SoapEnv.FldNmToValVH),
    FldNmToSoapTbH(){CopySoapTbH(SoapEnv);}
  TSoapEnv(TSIn& SIn):
    FldNmToValH(SIn), FldNmToValVH(SIn), FldNmToSoapTbH(SIn){SIn.LoadCs();}
  static PSoapEnv Load(TSIn& SIn){return new TSoapEnv(SIn);}
  void Save(TSOut& SOut) const {
    FldNmToValH.Save(SOut); FldNmToValVH.Save(SOut);
    FldNmToSoapTbH.Save(SOut); SOut.SaveCs();}

  TSoapEnv& operator=(const TSoapEnv& SoapEnv){
    FldNmToValH=SoapEnv.FldNmToValH;
    FldNmToValVH=SoapEnv.FldNmToValVH;
    CopySoapTbH(SoapEnv); return *this;}

  // management
  void Clr(){
    FldNmToValH.Clr(); FldNmToValVH.Clr(); FldNmToSoapTbH.Clr();}
  void MergeSoapEnv(const PSoapEnv& SoapEnv, const bool& AppendSoapTbP=false);

  // simple fields
  void AddFldNmVal(const TStr& FldNm, const TStr& FldVal){
    FldNmToValH.AddDat(FldNm, FldVal);}
  int GetFlds() const {return FldNmToValH.Len();}
  void GetFldNmVal(const int& FldN, TStr& FldNm, TStr& FldVal) const {
    FldNm=FldNmToValH.GetKey(FldN); FldVal=FldNmToValH[FldN];}
  bool IsFldNm(const TStr& FldNm) const {
    return FldNmToValH.IsKey(FldNm);}
  TStr GetFldVal(const TStr& FldNm, const TStr& DfVal="") const {
    if (IsFldNm(FldNm)){return FldNmToValH.GetDat(FldNm);} else {return DfVal;}}
  void DelFld(const TStr& Nm){FldNmToValH.DelIfKey(Nm);}

  // table data
  int GetTbs() const {return FldNmToSoapTbH.Len();}
  bool IsTb(const TStr& TbNm) const {
    return FldNmToSoapTbH.IsKey(TbNm);}
  PSoapTb GetTb(const TStr& TbNm) const {
    return FldNmToSoapTbH.IsKey(TbNm)?FldNmToSoapTbH.GetDat(TbNm):PSoapTb();}
  void AddTb(const TStr& TbNm){
    FldNmToSoapTbH.AddDat(TbNm, TSoapTb::New());}
  void AddTb(const TStr& TbNm, const PSoapTb& SoapTb){
    FldNmToSoapTbH.AddDat(TbNm, SoapTb);}
  void AddOneRowTb(const TStr& TbNm){
    FldNmToSoapTbH.AddDat(TbNm, TSoapTb::New())->AddRow();}
  void GetTbNmVal(const int& TbN, TStr& TbNm, PSoapTb& SoapTb) const {
    TbNm=FldNmToSoapTbH.GetKey(TbN); SoapTb=FldNmToSoapTbH[TbN];}
  void DelTb(const TStr& Nm){FldNmToSoapTbH.DelIfKey(Nm);}
  void PutTbVal(
   const TStr& TbNm, const int& RowN, const TStr& ColNm, const TStr& ValStr){
    IAssert(FldNmToSoapTbH.IsKey(TbNm));
    PSoapTb SoapTb=FldNmToSoapTbH.GetDat(TbNm);
    SoapTb->PutVal(RowN, ColNm, ValStr);
  }
  void PutTbVal(const TStr& TbNm, const TStr& ColNm, const TStr& ValStr){
    PutTbVal(TbNm, 0, ColNm, ValStr);}
  TStr GetTbVal(const TStr& TbNm, const int& RowN, const TStr& ColNm){
    if (FldNmToSoapTbH.IsKey(TbNm)){
      PSoapTb SoapTb=FldNmToSoapTbH.GetDat(TbNm);
      return SoapTb->GetVal(RowN, ColNm);
    } else {return "";}
  }
  TStr GetTbVal(const TStr& TbNm, const TStr& ColNm){
    return GetTbVal(TbNm, 0, ColNm);}

  // serializing soap-environment
  void AddFlattened(TStrPrV& FldNmValPrV) const;

  // files
  static PSoapEnv LoadBin(const TStr& FNm){
    TFIn SIn(FNm); return Load(SIn);}
  static PSoapEnv LoadBinIfValid(const TStr& FNm){
    try {TFIn SIn(FNm); return Load(SIn);} catch (...){} return NULL;}
  void SaveBin(const TStr& FNm) const {
    TFOut SOut(FNm); Save(SOut);}
};

/////////////////////////////////////////////////
// Soap-General
class TSoap{
public:
  // general
  static const TStr ResponseStr;

  // Fault Codes
  static const TStr VersionMismatchCodeNm;
  static const TStr MustUnderstandCodeNm;
  static const TStr DataEncodingUnknownCodeNm;
  static const TStr SenderCodeNm;
  static const TStr ReceiverCodeNm;

  // Soap-XML-strings
  static void GetFromXmlStr(
   const TStr& XmlStr, const bool& RespP,
   TBool& Ok, bool& FaultP,
   TStr& FuncNm, TStrPrV& FldNmValPrV,
   TStr& FaultCodeNm, TStr& FaultReasonStr);
  static TStr GetUrlPathStr(
   const TStr& FuncNm, const bool& RespP,
   const TStrPrV& FldNmValPrV);
  static TStr GetUrlPathStr(
   const TStr& FuncNm, const bool& RespP,
   const TStr& FldNm1="", const TStr& FldVal1="",
   const TStr& FldNm2="", const TStr& FldVal2="");
  static TStr GetXmlStr(
   const TStr& FuncNm, const bool& RespP,
   const TStrPrV& FldNmValPrV);
  static TStr GetXmlStr(
   const TStr& FuncNm, const bool& RespP);
  static TStr GetXmlStr(
   const TStr& FuncNm, const bool& RespP,
   const TStr& FldNm, const TStr& FldVal);
  static TStr GetXmlStr(
   const TStr& FuncNm, const bool& RespP,
   const TStr& FldNm1, const TStr& FldVal1,
   const TStr& FldNm2, const TStr& FldVal2);
  static TStr GetFaultXmlStr(const TStr& CodeNm, const TStr& ReasonStr);

  // vector fields
  static bool IsVecFld(const int& FldN, const TStrPrV& FldNmValPrV);
  static bool IsVecFld(const TStr& FldNm, const TStrPrV& FldNmValPrV);
  static void AddVecFld(const TStr& FldNm, const TStrV& FldValV, TStrPrV& FldNmValPrV);
  static void GetVecFld(const TStr& FldNm, TStrV& FldValV, const TStrPrV& FldNmValPrV);
  static int GetVecFld(const int& FldN, TStr& FldNm, TStrV& FldValV, const TStrPrV& FldNmValPrV);

  // table fields
  static bool IsTbFld(const int& FldN, const TStrPrV& FldNmValPrV);
  static bool IsTbFld(const TStr& FldNm, const TStrPrV& FldNmValPrV);
  static void AddTbFld(const TStr& FldNm, const PSoapTb& SoapTb, TStrPrV& FldNmValPrV);
  static void GetTbFld(const TStr& FldNm, PSoapTb& SoapTb, const TStrPrV& FldNmValPrV);
  static int GetTbFld(const int& FldN, TStr& FldNm, PSoapTb& SoapTb, const TStrPrV& FldNmValPrV);
};

/////////////////////////////////////////////////
// Soap-Request
ClassTP(TSoapRq, PSoapRq)//{
private:
  TBool Ok;
  TStr FuncNm;
  TStrPrV FldNmValPrV;
  UndefDefaultCopyAssign(TSoapRq);
public:
  TSoapRq(const TStr& _FuncNm):
    Ok(true), FuncNm(_FuncNm), FldNmValPrV(){}
  static PSoapRq New(const TStr& FuncNm){
    return PSoapRq(new TSoapRq(FuncNm));}
  static PSoapRq New(const TStr& FuncNm,
   const TStr& FldNm, const TStr& FldVal="");
  static PSoapRq New(const TStr& FuncNm,
   const TStr& FldNm1, const TStr& FldVal1,
   const TStr& FldNm2, const TStr& FldVal2);
  static PSoapRq New(const TStr& FuncNm,
   const TStr& FldNm1, const TStr& FldVal1,
   const TStr& FldNm2, const TStr& FldVal2,
   const TStr& FldNm3, const TStr& FldVal3);
  static PSoapRq New(const TStr& FuncNm,
   const TStr& FldNm1, const TStr& FldVal1,
   const TStr& FldNm2, const TStr& FldVal2,
   const TStr& FldNm3, const TStr& FldVal3,
   const TStr& FldNm4, const TStr& FldVal4);
  static PSoapRq New(const TStr& FuncNm,
   const TStr& FldNm1, const TStr& FldVal1,
   const TStr& FldNm2, const TStr& FldVal2,
   const TStr& FldNm3, const TStr& FldVal3,
   const TStr& FldNm4, const TStr& FldVal4,
   const TStr& FldNm5, const TStr& FldVal5);
  TSoapRq(const PHttpRq& HttpRq);
  static PSoapRq New(const PHttpRq& HttpRq){
    return PSoapRq(new TSoapRq(HttpRq));}
  ~TSoapRq(){}
  TSoapRq(TSIn& SIn):
    Ok(SIn), FuncNm(SIn), FldNmValPrV(SIn){}
  static PSoapRq Load(TSIn& SIn){return new TSoapRq(SIn);}
  void Save(TSOut& SOut){
    Ok.Save(SOut); FuncNm.Save(SOut); FldNmValPrV.Save(SOut);}

  // general
  bool IsOk() const {return Ok;}

  // simple-fields
  void PutFuncNm(const TStr& _FuncNm){FuncNm=_FuncNm;}
  TStr GetFuncNm() const {return FuncNm;}
  void AddFldNmVal(const TStr& FldNm, const TStr& FldVal);
  int GetFlds() const {return FldNmValPrV.Len();}
  void GetFldNmVal(const int& FldN, TStr& FldNm, TStr& FldVal) const {
    FldNm=FldNmValPrV[FldN].Val1; FldVal=FldNmValPrV[FldN].Val2;}
  bool IsFldNm(const TStr& FldNm) const;
  bool IsFldNmVal(const TStr& FldNm, const TStr& FldVal) const;
  TStr GetFldVal(const TStr& FldNm) const;

  // vector-fields
  bool IsVecFld(const TStr& FldNm) const {
    return TSoap::IsVecFld(FldNm, FldNmValPrV);}
  void AddVecFld(const TStr& FldNm, const TStrV& FldValV){
    TSoap::AddVecFld(FldNm, FldValV, FldNmValPrV);}
  void GetVecFld(const TStr& FldNm, TStrV& FldValV) const {
    TSoap::GetVecFld(FldNm, FldValV, FldNmValPrV);}

  // table-fields
  bool IsTbFld(const TStr& FldNm) const {
    return TSoap::IsTbFld(FldNm, FldNmValPrV);}
  void AddTbFld(const TStr& FldNm, const PSoapTb& SoapTb){
    TSoap::AddTbFld(FldNm, SoapTb, FldNmValPrV);}
  void GetTbFld(const TStr& FldNm, PSoapTb& SoapTb) const {
    TSoap::GetTbFld(FldNm, SoapTb, FldNmValPrV);}

  // soap-environment
  void AddSoapEnv(const PSoapEnv& SoapEnv){
    SoapEnv->AddFlattened(FldNmValPrV);}
  PSoapEnv GetSoapEnv() const {
    return TSoapEnv::New(FldNmValPrV);}

  // get soap-request from xml-string
  void GetFromXmlStr(
   const TStr& XmlStr, const bool& RespP,
   bool& Ok, bool& FaultP,
   TStr& FuncNm, TStrPrV& FldNmValPrV,
   TStr& FaultCodeNm, TStr& FaultReasonStr);

  // get from soap-request
  TStr GetAsStr() const {
    IAssert(IsOk()&&(!FuncNm.Empty()));
    return GetRqStr(FuncNm, FldNmValPrV);}
  TStr GetAsUrlPathStr() const {
    return TSoap::GetUrlPathStr(FuncNm, false, FldNmValPrV);}
  PUrl GetAsUrl(const TStr& HostNm, const int& PortN=80){
    TStr UrlStr=TStr("http://")+HostNm+":"+TInt::GetStr(PortN)+"/"+GetAsUrlPathStr();
    PUrl Url=TUrl::New(UrlStr); IAssert(Url->IsOk());
    return Url;}
  PHttpRq GetAsHttpRq(const PUrl& Url) const {
    return THttpRq::New(hrmPost, Url, THttp::AppSoapXmlFldVal, TMem(GetAsStr()));}

  // get xml-strings
  static TStr GetRqStr(const TStr& FuncNm, const TStrPrV& FldNmValPrV){
    return TSoap::GetXmlStr(FuncNm, false, FldNmValPrV);}
  static TStr GetRqStr(const TStr& FuncNm){
    return TSoap::GetXmlStr(FuncNm, false);}
  static TStr GetRqStr(const TStr& FuncNm,
   const TStr& FldNm, const TStr& FldVal){
    return TSoap::GetXmlStr(FuncNm, false, FldNm, FldVal);}
  static TStr GetRqStr(const TStr& FuncNm,
   const TStr& FldNm1, const TStr& FldVal1,
   const TStr& FldNm2, const TStr& FldVal2){
    return TSoap::GetXmlStr(FuncNm, false, FldNm1, FldVal1, FldNm2, FldVal2);}
};

/////////////////////////////////////////////////
// Soap-Response
ClassTP(TSoapResp, PSoapResp)//{
private:
  TBool Ok;
  bool FaultP;
  TStr FuncNm;
  TStrPrV FldNmValPrV;
  TStr FaultCodeNm;
  TStr FaultReasonStr;
  UndefDefaultCopyAssign(TSoapResp);
public:
  TSoapResp(const TStr& _FuncNm):
    Ok(true), FaultP(false), FuncNm(_FuncNm), FldNmValPrV(),
    FaultCodeNm(), FaultReasonStr(){}
  static PSoapResp New(const TStr& FuncNm){
    return PSoapResp(new TSoapResp(FuncNm));}
  TSoapResp(const PHttpResp& HttpResp);
  static PSoapResp New(const PHttpResp& HttpResp){
    return PSoapResp(new TSoapResp(HttpResp));}
  ~TSoapResp(){}
  TSoapResp(TSIn&){Fail;}
  static PSoapResp Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  // general
  bool IsOk() const {return Ok;}

  // simple-fields
  bool IsResult() const {return Ok&&(!FaultP);}
  void PutFuncNm(const TStr& _FuncNm){FuncNm=_FuncNm;}
  TStr GetFuncNm() const {return FuncNm;}
  void AddFldNmVal(const TStr& FldNm, const TStr& FldVal="");
  int GetFlds() const {return FldNmValPrV.Len();}
  void GetFldNmVal(const int& FldN, TStr& FldNm, TStr& FldVal) const {
    FldNm=FldNmValPrV[FldN].Val1; FldVal=FldNmValPrV[FldN].Val2;}
  void GetFldNmValKdV(TStrKdV& FldNmValKdV) const;
  bool IsFldNm(const TStr& FldNm) const;
  TStr GetFldVal(const TStr& FldNm) const;

  // vector-fields
  bool IsVecFld(const TStr& FldNm) const {
    return TSoap::IsVecFld(FldNm, FldNmValPrV);}
  void AddVecFld(const TStr& FldNm, const TStrV& FldValV){
    TSoap::AddVecFld(FldNm, FldValV, FldNmValPrV);}
  void GetVecFld(const TStr& FldNm, TStrV& FldValV) const {
    TSoap::GetVecFld(FldNm, FldValV, FldNmValPrV);}

  // table-fields
  bool IsTbFld(const TStr& FldNm) const {
    return TSoap::IsTbFld(FldNm, FldNmValPrV);}
  void AddTbFld(const TStr& FldNm, const PSoapTb& SoapTb){
    TSoap::AddTbFld(FldNm, SoapTb, FldNmValPrV);}
  void GetTbFld(const TStr& FldNm, PSoapTb& SoapTb) const {
    TSoap::GetTbFld(FldNm, SoapTb, FldNmValPrV);}

  // soap-environment
  void AddSoapEnv(const PSoapEnv& SoapEnv){
    SoapEnv->AddFlattened(FldNmValPrV);}
  PSoapEnv GetSoapEnv() const {
    return TSoapEnv::New(FldNmValPrV);}

  // failure information
  bool IsFault() const {return Ok&&FaultP;}
  TStr GetFaultCodeNm() const {return FaultCodeNm;}
  TStr GetFaultReasonStr() const {return FaultReasonStr;}

  // get response xml strings
  static TStr GetRespStr(const TStr& FuncNm, const TStrPrV& FldNmValPrV){
    return TSoap::GetXmlStr(FuncNm, true, FldNmValPrV);}
  static TStr GetRespStr(const TStr& FuncNm){
    return TSoap::GetXmlStr(FuncNm, true);}
  static TStr GetRespStr(const TStr& FuncNm,
   const TStr& FldNm, const TStr& FldVal){
    return TSoap::GetXmlStr(FuncNm, true, FldNm, FldVal);}
  static TStr GetRespStr(const TStr& FuncNm,
   const TStr& FldNm1, const TStr& FldVal1,
   const TStr& FldNm2, const TStr& FldVal2){
    return TSoap::GetXmlStr(FuncNm, true, FldNm1, FldVal1, FldNm2, FldVal2);}
  static TStr GetFaultRespStr(const TStr& CodeNm, const TStr& ReasonStr){
    return TSoap::GetFaultXmlStr(CodeNm, ReasonStr);}
  TStr GetRespStr() const;
  TStr GetAsUrlPathStr() const;

  // http responses
  PHttpResp GetHttpResp() const {
    TStr SoapStr=GetRespStr(FuncNm, FldNmValPrV);
    return THttpResp::New(
     THttp::OkStatusCd, THttp::AppSoapXmlFldVal, false, TStrIn::New(SoapStr));}
  static PHttpResp GetHttpResp(const TStr& FuncNm){
    TStr SoapStr=GetRespStr(FuncNm);
    return THttpResp::New(
     THttp::OkStatusCd, THttp::AppSoapXmlFldVal, false, TStrIn::New(SoapStr));}
  static PHttpResp GetHttpResp(const TStr& FuncNm,
   const TStr& FldNm, const TStr& FldVal=""){
    TStr SoapStr=GetRespStr(FuncNm, FldNm, FldVal);
    return THttpResp::New(
     THttp::OkStatusCd, THttp::AppSoapXmlFldVal, false, TStrIn::New(SoapStr));}
  static PHttpResp GetHttpResp(const TStr& FuncNm,
   const TStr& FldNm1, const TStr& FldVal1,
   const TStr& FldNm2, const TStr& FldVal2){
    TStr SoapStr=GetRespStr(FuncNm, FldNm1, FldVal1, FldNm2, FldVal2);
    return THttpResp::New(
     THttp::OkStatusCd, THttp::AppSoapXmlFldVal, false, TStrIn::New(SoapStr));}
  static PHttpResp GetHttpResp(const TStr& FuncNm,
   const TStr& FldNm1, const TStr& FldVal1,
   const TStr& FldNm2, const TStr& FldVal2,
   const TStr& FldNm3, const TStr& FldVal3);
  static PHttpResp GetHttpResp(const TStr& FuncNm,
   const TStr& FldNm1, const TStr& FldVal1,
   const TStr& FldNm2, const TStr& FldVal2,
   const TStr& FldNm3, const TStr& FldVal3,
   const TStr& FldNm4, const TStr& FldVal4);
  static PHttpResp GetHttpResp(const TStr& FuncNm,
   const TStr& FldNm1, const TStr& FldVal1,
   const TStr& FldNm2, const TStr& FldVal2,
   const TStr& FldNm3, const TStr& FldVal3,
   const TStr& FldNm4, const TStr& FldVal4,
   const TStr& FldNm5, const TStr& FldVal5);
  static PHttpResp GetFaultHttpResp(const TStr& CodeNm, const TStr& ReasonStr){
    TStr SoapStr=GetFaultRespStr(CodeNm, ReasonStr);
    return THttpResp::New(
     THttp::OkStatusCd, THttp::AppSoapXmlFldVal, false, TStrIn::New(SoapStr));}
  static PHttpResp GetFaultHttpResp(
   const TStr& CodeNm, const TStr& ReasonMsgStr, const TStr& ReasonArgStr){
    TStr ReasonStr=ReasonMsgStr+" ("+ReasonArgStr+").";
    return GetFaultHttpResp(CodeNm, ReasonStr);}
};

