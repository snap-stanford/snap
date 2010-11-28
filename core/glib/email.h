#ifndef email_h
#define email_h

/////////////////////////////////////////////////
// Includes
#include "base.h"

/////////////////////////////////////////////////
// EMail-Alias-Base
ClassTP(TEMailAliasBs, PEMailAliasBs)//{
private:
  TStrStrPrH EAddrToPersonNmDepNmPrH;
  UndefCopyAssign(TEMailAliasBs);
public:
  TEMailAliasBs():
    EAddrToPersonNmDepNmPrH(){}
  static PEMailAliasBs New(){
    return new TEMailAliasBs();}
  TEMailAliasBs(TSIn& SIn):
    EAddrToPersonNmDepNmPrH(SIn){}
  static PEMailAliasBs Load(TSIn& SIn){return new TEMailAliasBs(SIn);}
  void Save(TSOut& SOut) const {
    EAddrToPersonNmDepNmPrH.Save(SOut);}

  // retrieval
  bool IsEAddr(const TStr& EAddr) const {
    return EAddrToPersonNmDepNmPrH.IsKey(EAddr);}
  TStr GetPersonNm(const TStr& EAddr) const {
    if (IsEAddr(EAddr)){
      return EAddrToPersonNmDepNmPrH.GetDat(EAddr).Val1;
    } else {return "";}}
  TStr GetDepNm(const TStr& EAddr) const {
    if (IsEAddr(EAddr)){
      return EAddrToPersonNmDepNmPrH.GetDat(EAddr).Val2;
    } else {return "";}}

  // files
  static PEMailAliasBs LoadSsTxt(const TStr& FNm);
};

/////////////////////////////////////////////////
// EMail-Transaction
ClassTPV(TEMailTsact, PEMailTsact, TEMailTsactV)//{
public:
  TInt SrcEAddrId;
  TIntV DstEAddrIdV;
  TTm Tm;
public:
  TEMailTsact():
    SrcEAddrId(), DstEAddrIdV(), Tm(){}
  TEMailTsact(const TEMailTsact& EMailTsact):
    SrcEAddrId(EMailTsact.SrcEAddrId), DstEAddrIdV(EMailTsact.DstEAddrIdV),
    Tm(EMailTsact.Tm){}
  TEMailTsact(
   const TInt& _SrcEAddrId, const TIntV& _DstEAddrVId, const TTm& _Tm):
    SrcEAddrId(_SrcEAddrId), DstEAddrIdV(_DstEAddrVId), Tm(_Tm){}
  static PEMailTsact New(
   const TInt& SrcEAddrId, const TIntV& DstEAddrIdV, const TTm& Tm){
    return new TEMailTsact(SrcEAddrId, DstEAddrIdV, Tm);}
  TEMailTsact(TSIn& SIn):
    SrcEAddrId(SIn), DstEAddrIdV(SIn), Tm(SIn){}
  void Save(TSOut& SOut) const {
    SrcEAddrId.Save(SOut); DstEAddrIdV.Save(SOut); Tm.Save(SOut);}

  TEMailTsact& operator=(const TEMailTsact& EMailTsact){
    SrcEAddrId=EMailTsact.SrcEAddrId; DstEAddrIdV=EMailTsact.DstEAddrIdV;
    Tm=EMailTsact.Tm; return *this;}
  bool operator==(const TEMailTsact& EMailTsact) const {
    return Tm==EMailTsact.Tm;}
  bool operator<(const TEMailTsact& EMailTsact) const {
    return Tm<EMailTsact.Tm;}
};

/////////////////////////////////////////////////
// EMail-Transaction-Base
ClassTP(TEMailTsactBs, PEMailTsactBs)//{
private:
  TStrHash<TIntPr> EAddrStrToSrcFqDstFqPrH;
  TVec<TEMailTsact> EMailTsactV;
  UndefCopyAssign(TEMailTsactBs);
public:
  TEMailTsactBs():
    EAddrStrToSrcFqDstFqPrH(), EMailTsactV(){}
  static PEMailTsactBs New(){
    return new TEMailTsactBs();}
  TEMailTsactBs(TSIn& SIn):
    EAddrStrToSrcFqDstFqPrH(SIn), EMailTsactV(SIn){}
  static PEMailTsactBs Load(TSIn& SIn){return new TEMailTsactBs(SIn);}
  void Save(TSOut& SOut) const {
    EAddrStrToSrcFqDstFqPrH.Save(SOut); EMailTsactV.Save(SOut);}

  // email-addresses
  int GetEAddrs() const {return EAddrStrToSrcFqDstFqPrH.Len();}
  TStr GetEAddr(const int& EAddrId) const {
    return EAddrStrToSrcFqDstFqPrH.GetKey(EAddrId);}
  static bool IsEAddrOk(const TStr& EAddr);

  // email-transactions
  int GetTsacts() const {return EMailTsactV.Len();}
  int AddTsact(const TStr& SrcEAddr, const TStrV& DstEAddrV, const TTm& Tm);
  TEMailTsact& GetTsact(const int& TsactN){
    return EMailTsactV[TsactN];}

  // files
  static PEMailTsactBs LoadBin(const TStr& FNm){
    TFIn SIn(FNm); return Load(SIn);}
  static void SaveBin(const TStr& FNm, const PEMailTsactBs& EMailTsactBs);
  void SaveBin(const TStr& FNm){
    TFOut SOut(FNm); Save(SOut);}
};

/////////////////////////////////////////////////
// EMail-Message
ClassTPV(TEml, PEml, TEmlV)//{
public:
  TStr OwnerNm;
  TStrV FromAddrStrV;
  TStrV ToAddrStrV;
  TTm Tm;
  TStr SubjStr;
  TStr ContStr;
public:
  TEml():
    OwnerNm(), FromAddrStrV(), ToAddrStrV(), Tm(), SubjStr(), ContStr(){}
  static PEml New(){return new TEml();}
  TEml(const TEml& Eml):
    OwnerNm(Eml.OwnerNm), 
    FromAddrStrV(Eml.FromAddrStrV), ToAddrStrV(Eml.ToAddrStrV), 
    Tm(Eml.Tm), SubjStr(Eml.SubjStr), ContStr(Eml.ContStr){}
  TEml(TSIn& SIn):
    OwnerNm(SIn), FromAddrStrV(SIn), ToAddrStrV(SIn), Tm(SIn), SubjStr(SIn), ContStr(SIn){}
  static PEml Load(TSIn& SIn){return new TEml(SIn);}
  void Save(TSOut& SOut) const {
    OwnerNm.Save(SOut);
    FromAddrStrV.Save(SOut); ToAddrStrV.Save(SOut); Tm.Save(SOut); 
    SubjStr.Save(SOut); ContStr.Save(SOut);}

  TEml& operator=(const TEml& Eml){
    OwnerNm=Eml.OwnerNm;
    FromAddrStrV=Eml.FromAddrStrV; ToAddrStrV=Eml.ToAddrStrV; Tm=Eml.Tm; 
    SubjStr=Eml.SubjStr; ContStr=Eml.ContStr; return *this;}
  bool operator==(const TEml& Eml) const {
    return Tm==Eml.Tm;}
  bool operator<(const TEml& Eml) const {
    return Tm<Eml.Tm;}

  // retrieve components
  TStr GetOwnerNm() const {return OwnerNm;}
  int GetFromEAddrs() const {return FromAddrStrV.Len();}
  TStr GetFromEAddr(const int& EAddrN) const {return FromAddrStrV[EAddrN];}
  int GetToEAddrs() const {return ToAddrStrV.Len();}
  TStr GetToEAddr(const int& EAddrN) const {return ToAddrStrV[EAddrN];}
  TTm GetTm() const {return Tm;}
  TStr GetSubjStr() const {return SubjStr;}
  TStr GetContStr() const {return ContStr;}

  // parsing eml file
  TTm GetTmFromRfc2822DateTimeStr(const TStr& DateTimeStr);
  void ExtrMergeToAddrStrV(const TStr& FldVal, TStrV& AddrStrV);
  bool IsFieldNmCh(const uchar& Ch) const;
  void ToNrEncoding(const TStr& ContTransferEncVal, TChA& ChA);
  void ExtrAddBoundaryStr(const TStr& ContTypeVal, TStrV& BoundaryStrV) const;
  bool Eof(const PSIn& SIn){return SIn->Eof();}
  char GetCh(const PSIn& SIn){return SIn->Eof() ? 0 : SIn->GetCh();}
  bool LoadField(const PSIn& SIn, TChA& FldNm, TChA& FldVal, TChA& LcFldVal);
  void LoadMessage(const PSIn& SIn);
  static PEml LoadEml(const TStr& FNm, const TStr& OwnerNm);
};

/////////////////////////////////////////////////
// EMail-Message-Base
ClassTP(TEmlBs, PEmlBs)//{
private:
  THash<TStr, PEml> NmToEmlH;
  TStrIntVH EAddrStrToEmlIdVH;
  UndefAssign(TEmlBs);
public:
  TEmlBs(): NmToEmlH(), EAddrStrToEmlIdVH(){}
  static PEmlBs New(){return new TEmlBs();}
  TEmlBs(const TEmlBs& EmlBs):
    NmToEmlH(EmlBs.NmToEmlH),
    EAddrStrToEmlIdVH(EmlBs.EAddrStrToEmlIdVH){}
  ~TEmlBs(){}
  TEmlBs(TSIn& SIn):
    NmToEmlH(SIn), EAddrStrToEmlIdVH(SIn){}
  static PEmlBs Load(TSIn& SIn){return new TEmlBs(SIn);}
  void Save(TSOut& SOut){
    NmToEmlH.Save(SOut); EAddrStrToEmlIdVH.Save(SOut);}

  // email-management
  int GetEmls() const {return NmToEmlH.Len();}
  void AddEml(const TStr& EmlNm, const PEml& Eml);
  void DelEml(const TStr& EmlNm);
  PEml GetEml(const TStr& EmlNm) const {return NmToEmlH.GetDat(EmlNm);}
  void GetEmlNmV(TStrV& EmlNmV) const {NmToEmlH.GetKeyV(EmlNmV);}
  void GetTmEmlNmPrV(TTmStrPrV& TmEmlNmPrV) const;

  // addresses
  int GetEAddrs() const {return EAddrStrToEmlIdVH.Len();}
  TStr GetEAddrStr(const int& EAddrN) const {return EAddrStrToEmlIdVH.GetKey(EAddrN);}
  int GetEAddrFq(const int& EAddrN) const {return EAddrStrToEmlIdVH[EAddrN].Len();}
  void AddEAddr(const TStr& EAddrStr, const int& EmlP){
    EAddrStrToEmlIdVH.AddDat(EAddrStr).AddBackSorted(EmlP, true);}
  void DelEAddr(const TStr& EAddrStr, const int& EmlP){
    EAddrStrToEmlIdVH.GetDat(EAddrStr).DelIfIn(EmlP);}
  void GetEAddrStrV(TStrV& EAddrStrV) const {
    EAddrStrToEmlIdVH.GetKeyV(EAddrStrV);}
  void GetEAddrEmlPV(const TStr& EAddrStr, TIntV& EmlPV){
    EmlPV=EAddrStrToEmlIdVH.GetDat(EAddrStr);}

  // email-account
  void LoadWinMailAccount(const TStr& FPath, const TStr& OwnerNm, const int& MxEmls=-1);
  void LoadEnronEMail(const TStr& FPath, const int& MxEmls=-1);

  // binary files
  static PEmlBs LoadBin(const TStr& FNm){
    TFIn SIn(FNm); return Load(SIn);}
  void SaveBin(const TStr& FNm){
    TFOut SOut(FNm); Save(SOut);}

  // text file
  void SaveTxt(const TStr& FNm);
  void SaveTxtEAddr(const TStr& FNm);
};

#endif
