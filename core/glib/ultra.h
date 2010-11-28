#ifndef ultra_h
#define ultra_h

/////////////////////////////////////////////////
// Includes
#include "base.h"

/////////////////////////////////////////////////
// Gas-Definitions
ClassTP(TGasDef, PGasDef)//{
public:
  TStrTrV FldNmTyUseTrV;
  TInt LocIdFldN, TankIdFldN, ProdIdFldN;
  TInt DateFldN, HourNFldN, DayNFldN, MonthNFldN, YearNFldN, DowNFldN;
  TInt CsptFldN, VolFldN;
  TInt CsptLast1WFldN, CsptLast2WFldN, CsptLast3WFldN;
  TInt SumCsptLast1WFldN, SumCsptLast2WFldN, SumCsptLast3WFldN;
  TInt CsptLast1DFldN, CsptLast2DFldN, CsptLast3DFldN;
  TInt CsptLast4DFldN, CsptLast5DFldN, CsptLast6DFldN;
  TInt SumCsptLast1DFldN, SumCsptLast2DFldN, SumCsptLast3DFldN;
  TInt SumCsptLast4DFldN, SumCsptLast5DFldN, SumCsptLast6DFldN;
  TInt IsHolidayFldN, IsHolidayStartFldN, IsHolidayEndFldN;
  TInt IsDayBeforeHolidayFldN, IsDayAfterHolidayFldN;
  TInt PriceChangeFldN, DaysBeforePriceChangeFldN;
  TInt LocNmFldN, ProdNmFldN;
public:
  TGasDef(const bool& DbP);
  static PGasDef New(const bool& DbP=false){return new TGasDef(DbP);}
  TGasDef(TSIn& SIn):
    FldNmTyUseTrV(SIn),
    LocIdFldN(SIn), TankIdFldN(SIn), ProdIdFldN(SIn),
    DateFldN(SIn), HourNFldN(SIn), DayNFldN(SIn), MonthNFldN(SIn),
    YearNFldN(SIn), DowNFldN(SIn),
    CsptFldN(SIn), VolFldN(SIn),
    CsptLast1WFldN(SIn), CsptLast2WFldN(SIn), CsptLast3WFldN(SIn),
    SumCsptLast1WFldN(SIn), SumCsptLast2WFldN(SIn), SumCsptLast3WFldN(SIn),
    CsptLast1DFldN(SIn), CsptLast2DFldN(SIn), CsptLast3DFldN(SIn),
    CsptLast4DFldN(SIn), CsptLast5DFldN(SIn), CsptLast6DFldN(SIn),
    SumCsptLast1DFldN(SIn), SumCsptLast2DFldN(SIn), SumCsptLast3DFldN(SIn),
    SumCsptLast4DFldN(SIn), SumCsptLast5DFldN(SIn), SumCsptLast6DFldN(SIn),
    IsHolidayFldN(SIn), IsHolidayStartFldN(SIn), IsHolidayEndFldN(SIn),
    IsDayBeforeHolidayFldN(SIn), IsDayAfterHolidayFldN(SIn),
    PriceChangeFldN(SIn), DaysBeforePriceChangeFldN(SIn),
    LocNmFldN(SIn), ProdNmFldN(SIn){}
  static PGasDef Load(TSIn& SIn){return new TGasDef(SIn);}
  void Save(TSOut& SOut){
    FldNmTyUseTrV.Save(SOut);
    LocIdFldN.Save(SOut); TankIdFldN.Save(SOut); ProdIdFldN.Save(SOut);
    DateFldN.Save(SOut); HourNFldN.Save(SOut); DayNFldN.Save(SOut);
    MonthNFldN.Save(SOut); YearNFldN.Save(SOut); DowNFldN.Save(SOut);
    CsptFldN.Save(SOut); VolFldN.Save(SOut);
    CsptLast1WFldN.Save(SOut); CsptLast2WFldN.Save(SOut); CsptLast3WFldN.Save(SOut);
    SumCsptLast1WFldN.Save(SOut); SumCsptLast2WFldN.Save(SOut); SumCsptLast3WFldN.Save(SOut);
    CsptLast1DFldN.Save(SOut); CsptLast2DFldN.Save(SOut); CsptLast3DFldN.Save(SOut);
    CsptLast4DFldN.Save(SOut); CsptLast5DFldN.Save(SOut); CsptLast6DFldN.Save(SOut);
    SumCsptLast1DFldN.Save(SOut); SumCsptLast2DFldN.Save(SOut); SumCsptLast3DFldN.Save(SOut);
    SumCsptLast4DFldN.Save(SOut); SumCsptLast5DFldN.Save(SOut); SumCsptLast6DFldN.Save(SOut);
    IsHolidayFldN.Save(SOut); IsHolidayStartFldN.Save(SOut); IsHolidayEndFldN.Save(SOut);
    IsDayBeforeHolidayFldN.Save(SOut); IsDayAfterHolidayFldN.Save(SOut);
    PriceChangeFldN.Save(SOut); DaysBeforePriceChangeFldN.Save(SOut);
    LocNmFldN.Save(SOut); ProdNmFldN.Save(SOut);}

  TGasDef& operator=(const TGasDef& GasDef){
    Fail; if (this!=&GasDef){} return *this;}

  int GetFlds() const {return FldNmTyUseTrV.Len();}
  bool IsFldNmVOk(const TStrV& FldNmV) const;
  TStr GetLtpNm(const TStrV& FldValV) const {
    return FldValV[LocIdFldN]+"-"+FldValV[TankIdFldN]+"-"+FldValV[ProdIdFldN];}
  TStr GetLocNm(const TStrV& FldValV) const {return FldValV[LocNmFldN];}
  TStr GetProdNm(const TStrV& FldValV) const {return FldValV[ProdNmFldN];}
  TStr GetLtpFNm(const TStr& LtpNm) const {return LtpNm+".Ltp";}
  void GetFldIntValV(const TStrV& FldStrValV, TIntV& FldIntValV) const;
  TSecTm GetTm(const TIntV& FldIntValV) const;
  void GetHdm(
   const TIntV& FldIntValV, int& HourN, int& DowN, int& MonthN) const;
  TSecTm GetDate(const TIntV& FldIntValV) const {return TSecTm(FldIntValV[DateFldN]);}
  int GetHourN(const TIntV& FldIntValV) const {return FldIntValV[HourNFldN];}
  int GetDowN(const TIntV& FldIntValV) const {return FldIntValV[DowNFldN];}
  int GetMonthN(const TIntV& FldIntValV) const {return FldIntValV[MonthNFldN];}
  int GetYearN(const TIntV& FldIntValV) const {return FldIntValV[YearNFldN];}
  int GetCspt(const TIntV& FldIntValV) const {return FldIntValV[CsptFldN];}
  int GetVol(const TIntV& FldIntValV) const {return FldIntValV[VolFldN];}
  TInt& GetCsptLast1W(TIntV& FldIntValV) const {return FldIntValV[CsptLast1WFldN];}
  TInt& GetCsptLast2W(TIntV& FldIntValV) const {return FldIntValV[CsptLast2WFldN];}
  TInt& GetCsptLast3W(TIntV& FldIntValV) const {return FldIntValV[CsptLast3WFldN];}
  TInt& GetSumCsptLast1W(TIntV& FldIntValV) const {return FldIntValV[SumCsptLast1WFldN];}
  TInt& GetSumCsptLast2W(TIntV& FldIntValV) const {return FldIntValV[SumCsptLast2WFldN];}
  TInt& GetSumCsptLast3W(TIntV& FldIntValV) const {return FldIntValV[SumCsptLast3WFldN];}
  TInt& GetCsptLast1D(TIntV& FldIntValV) const {return FldIntValV[CsptLast1DFldN];}
  TInt& GetCsptLast2D(TIntV& FldIntValV) const {return FldIntValV[CsptLast2DFldN];}
  TInt& GetCsptLast3D(TIntV& FldIntValV) const {return FldIntValV[CsptLast3DFldN];}
  TInt& GetCsptLast4D(TIntV& FldIntValV) const {return FldIntValV[CsptLast4DFldN];}
  TInt& GetCsptLast5D(TIntV& FldIntValV) const {return FldIntValV[CsptLast5DFldN];}
  TInt& GetCsptLast6D(TIntV& FldIntValV) const {return FldIntValV[CsptLast6DFldN];}
  TInt& GetSumCsptLast1D(TIntV& FldIntValV) const {return FldIntValV[SumCsptLast1DFldN];}
  TInt& GetSumCsptLast2D(TIntV& FldIntValV) const {return FldIntValV[SumCsptLast2DFldN];}
  TInt& GetSumCsptLast3D(TIntV& FldIntValV) const {return FldIntValV[SumCsptLast3DFldN];}
  TInt& GetSumCsptLast4D(TIntV& FldIntValV) const {return FldIntValV[SumCsptLast4DFldN];}
  TInt& GetSumCsptLast5D(TIntV& FldIntValV) const {return FldIntValV[SumCsptLast5DFldN];}
  TInt& GetSumCsptLast6D(TIntV& FldIntValV) const {return FldIntValV[SumCsptLast6DFldN];}
  int GetIsHoliday(const TIntV& FldIntValV) const {return FldIntValV[IsHolidayFldN];}
  int GetIsHolidayStart(const TIntV& FldIntValV) const {return FldIntValV[IsHolidayStartFldN];}
  int GetIsHolidayEnd(const TIntV& FldIntValV) const {return FldIntValV[IsHolidayEndFldN];}
  int GetIsDayBeforeHoliday(const TIntV& FldIntValV) const {return FldIntValV[IsDayBeforeHolidayFldN];}
  int GetIsDayAfterHoliday(const TIntV& FldIntValV) const {return FldIntValV[IsDayAfterHolidayFldN];}
  int GetPriceChange(const TIntV& FldIntValV) const {return FldIntValV[PriceChangeFldN];}
  int GetDaysBeforePriceChange(const TIntV& FldIntValV) const {return FldIntValV[DaysBeforePriceChangeFldN];}
};

/////////////////////////////////////////////////
// Gas-Location/Tank/Product-Base
ClassTPV(TGasLtpBs, PGasLtpBs, TGasLtpBsV)//{
public:
  TStr LtpNm, LocNm, ProdNm;
  PGasDef GasDef;
  TVec<TIntV> FldValVV;
  TMomV HourMomV, DowMomV, MonthMomV;
  TMomVV DowHourMomVV;
public:
  TGasLtpBs(const TStr& _LtpNm, const TStr& _LocNm, const TStr& _ProdNm,
   const PGasDef& _GasDef):
    LtpNm(_LtpNm), LocNm(_LocNm), ProdNm(_ProdNm),
    GasDef(_GasDef), FldValVV(),
    HourMomV(), DowMomV(), MonthMomV(), DowHourMomVV(){}
  static PGasLtpBs New(const TStr& LtpNm, const TStr& LocNm, const TStr& ProdNm,
   const PGasDef& GasDef=NULL){
    return new TGasLtpBs(LtpNm, LocNm, ProdNm, GasDef);}
  TGasLtpBs(TSIn& SIn):
    LtpNm(SIn), LocNm(SIn), ProdNm(SIn),
    GasDef(SIn),
    FldValVV(SIn),
    HourMomV(SIn), DowMomV(SIn), MonthMomV(SIn),
    DowHourMomVV(SIn){}
  static PGasLtpBs Load(TSIn& SIn){return new TGasLtpBs(SIn);}
  static PGasLtpBs Load(const TStr& FNm){
    TFIn SIn(FNm);
    return new TGasLtpBs(SIn);}
  void Save(TSOut& SOut){
    LtpNm.Save(SOut); LocNm.Save(SOut); ProdNm.Save(SOut);
    GasDef.Save(SOut); FldValVV.Save(SOut);
    HourMomV.Save(SOut); DowMomV.Save(SOut); MonthMomV.Save(SOut);
    DowHourMomVV.Save(SOut);}

  TGasLtpBs& operator=(const TGasLtpBs& GasLtpBs){
    Fail; if (this!=&GasLtpBs){} return *this;}

  // description
  TStr GetLtpNm() const {return LtpNm;}
  TStr GetLocNm() const {return LocNm;}
  TStr GetProdNm() const {return ProdNm;}
  TStr GetTxtDescStr() const {
    return LocNm.GetTrunc()+"/"+ProdNm.GetTrunc();}
  TStr GetDescStr() const {
    return LtpNm+"/"+LocNm.GetTrunc()+"/"+ProdNm.GetTrunc();}

  // records
  int GetRecs() const {return FldValVV.Len();}
  TIntV& AddRec(){return FldValVV[FldValVV.Add()];}
  TIntV& GetRec(const int& RecN){return FldValVV[RecN];}

  // field values
  int GetHourN(const int& RecN) const {return GasDef->GetHourN(FldValVV[RecN]);}
  int GetDowN(const int& RecN) const {return GasDef->GetDowN(FldValVV[RecN]);}
  int GetMonthN(const int& RecN) const {return GasDef->GetMonthN(FldValVV[RecN]);}
  int GetYearN(const int& RecN) const {return GasDef->GetYearN(FldValVV[RecN]);}
  TSecTm GetTm(const int& RecN) const {return GasDef->GetTm(FldValVV[RecN]);}
  TSecTm GetDate(const int& RecN) const {return GasDef->GetDate(FldValVV[RecN]);}
  void GetHdm(const int& RecN, int& HourN, int& DowN, int& MonthN){
    GasDef->GetHdm(GetRec(RecN), HourN, DowN, MonthN);}
  int GetDateInt(const int& RecN) const {return GetDate(RecN).GetAbsSecs();}
  int GetCspt(const int& RecN) const {return GasDef->GetCspt(FldValVV[RecN]);}
  int GetVol(const int& RecN) const {return GasDef->GetVol(FldValVV[RecN]);}
  TInt& GetCsptLast1W(const int& RecN) const {return GasDef->GetCsptLast1W(FldValVV[RecN]);}
  TInt& GetCsptLast2W(const int& RecN) const {return GasDef->GetCsptLast2W(FldValVV[RecN]);}
  TInt& GetCsptLast3W(const int& RecN) const {return GasDef->GetCsptLast3W(FldValVV[RecN]);}
  TInt& GetSumCsptLast1W(const int& RecN) const {return GasDef->GetSumCsptLast1W(FldValVV[RecN]);}
  TInt& GetSumCsptLast2W(const int& RecN) const {return GasDef->GetSumCsptLast2W(FldValVV[RecN]);}
  TInt& GetSumCsptLast3W(const int& RecN) const {return GasDef->GetSumCsptLast3W(FldValVV[RecN]);}
  TInt& GetCsptLast1D(const int& RecN) const {return GasDef->GetCsptLast1D(FldValVV[RecN]);}
  TInt& GetCsptLast2D(const int& RecN) const {return GasDef->GetCsptLast2D(FldValVV[RecN]);}
  TInt& GetCsptLast3D(const int& RecN) const {return GasDef->GetCsptLast3D(FldValVV[RecN]);}
  TInt& GetCsptLast4D(const int& RecN) const {return GasDef->GetCsptLast4D(FldValVV[RecN]);}
  TInt& GetCsptLast5D(const int& RecN) const {return GasDef->GetCsptLast5D(FldValVV[RecN]);}
  TInt& GetCsptLast6D(const int& RecN) const {return GasDef->GetCsptLast6D(FldValVV[RecN]);}
  TInt& GetSumCsptLast1D(const int& RecN) const {return GasDef->GetSumCsptLast1D(FldValVV[RecN]);}
  TInt& GetSumCsptLast2D(const int& RecN) const {return GasDef->GetSumCsptLast2D(FldValVV[RecN]);}
  TInt& GetSumCsptLast3D(const int& RecN) const {return GasDef->GetSumCsptLast3D(FldValVV[RecN]);}
  TInt& GetSumCsptLast4D(const int& RecN) const {return GasDef->GetSumCsptLast4D(FldValVV[RecN]);}
  TInt& GetSumCsptLast5D(const int& RecN) const {return GasDef->GetSumCsptLast5D(FldValVV[RecN]);}
  TInt& GetSumCsptLast6D(const int& RecN) const {return GasDef->GetSumCsptLast6D(FldValVV[RecN]);}
  int GetIsHoliday(const int& RecN) const {return GasDef->GetIsHoliday(FldValVV[RecN]);}
  int GetIsHolidayStart(const int& RecN) const {return GasDef->GetIsHolidayStart(FldValVV[RecN]);}
  int GetIsHolidayEnd(const int& RecN) const {return GasDef->GetIsHolidayEnd(FldValVV[RecN]);}
  int GetIsDayBeforeHoliday(const int& RecN) const {return GasDef->GetIsDayBeforeHoliday(FldValVV[RecN]);}
  int GetIsDayAfterHoliday(const int& RecN) const {return GasDef->GetIsDayAfterHoliday(FldValVV[RecN]);}
  int GetPriceChange(const int& RecN) const {return GasDef->GetPriceChange(FldValVV[RecN]);}
  int GetDaysBeforePriceChange(const int& RecN) const {return GasDef->GetDaysBeforePriceChange(FldValVV[RecN]);}

  // min. and max. dates
  TSecTm GetMnDate() const;
  TSecTm GetMxDate() const;

  // statistics
  void GenStat();
  bool IsDowHourMomUsable() const;
  void RepairUnknowns();
  void SaveStatTxt(const PSOut& SOut) const;
  void SaveStatBin(const PSOut& SOut) const;
  static void LoadStatV(const TStr& FNm, TGasLtpBsV& GasLtpBsV);
  PMom GetHourMom(const int& HourN) const {return HourMomV[HourN];}
  PMom GetDowMom(const int& DowN) const {return DowMomV[DowN];}
  PMom GetMonthMom(const int& MonthN) const {return MonthMomV[MonthN];}
  PMom GetDowHourMom(const int& DowN, const int& HourN) const {
    return DowHourMomVV.At(DowN, HourN);}
  static const TStr LtpStatTxtFNm;
  static const TStr LtpStatBinFNm;

  // converting directory of Ultra-files to Ltp-Files
  static void ConvTabToLtp(const TStr& InTabFNmWc, const TStr& OutLtpFPath,
   const bool& RepairUnknownsP=false);
};

/////////////////////////////////////////////////
// Gas-Domain
ClassTP(TGasDm, PGasDm)//{
public:
  TStr TypeNm;
  TStr LtpNm;
  TStr LocNm;
  TStr ProdNm;
  TStrKdV VarNmTyKdV;
  TVec<TFltV> VarValVV;
  TInt ClassVarN, DateVarN, VolVarN, Attrs;
public:
  TGasDm(const TStr& _TypeNm="Undef"):
    TypeNm(_TypeNm), LtpNm(), LocNm(), ProdNm(), VarNmTyKdV(), VarValVV(),
    ClassVarN(-1), DateVarN(-1), VolVarN(-1), Attrs(0){}
  static PGasDm New(const TStr& TypeNm="Undef"){
    return new TGasDm(TypeNm);}
  TGasDm(TSIn& SIn):
    TypeNm(SIn), LtpNm(SIn), LocNm(SIn), ProdNm(SIn),
    VarNmTyKdV(SIn), VarValVV(SIn),
    ClassVarN(SIn), DateVarN(SIn), VolVarN(SIn), Attrs(SIn){}
  static PGasDm Load(TSIn& SIn){return new TGasDm(SIn);}
  void Save(TSOut& SOut){
    TypeNm.Save(SOut); LtpNm.Save(SOut); LocNm.Save(SOut); ProdNm.Save(SOut);
    VarNmTyKdV.Save(SOut); VarValVV.Save(SOut);
    ClassVarN.Save(SOut); DateVarN.Save(SOut); VolVarN.Save(SOut);
    Attrs.Save(SOut);}

  TGasDm& operator=(const TGasDm& GasDm){
    Fail; if (this!=&GasDm){} return *this;}

  // type/description/product
  void PutTypeNm(const TStr& _TypeNm){TypeNm=_TypeNm;}
  TStr GetTypeNm() const {return TypeNm;}
  void PutLtpNm(const TStr& _LtpNm){LtpNm=_LtpNm;}
  TStr GetLtpNm() const {return LtpNm;}
  void PutLocNm(const TStr& _LocNm){LocNm=_LocNm;}
  TStr GetLocNm() const {return LocNm;}
  void PutProdNm(const TStr& _ProdNm){ProdNm=_ProdNm;}
  TStr GetProdNm() const {return ProdNm;}
  TStr GetDescStr() const {return GetLtpNm()+"/"+GetLocNm()+"/"+GetProdNm();}

  // variables
  int GetVars() const {return VarNmTyKdV.Len();}
  TStr GetVarNm(const int& VarN) const {return VarNmTyKdV[VarN].Key;}
  TStr GetVarTy(const int& VarN) const {return VarNmTyKdV[VarN].Dat;}
  int GetVarN(const TStr& VarNm) const {
    return VarNmTyKdV.SearchForw(TStrKd(VarNm));}
  int AddVar(const TStr& VarNm, const TStr& VarTy);
  int GetAttrs() const {return Attrs;}
  void GetAttrNmV(TStrV& AttrNmV) const;
  TStr GetAttrNm(const int& AttrN) const {
    TStrV AttrNmV; GetAttrNmV(AttrNmV); return AttrNmV[AttrN];}
  int GetAttrN(const TStr& AttrNm) const;
  TStr GetClassNm() const;

  // records
  int GetRecs() const {return VarValVV.Len();}
  TFltV& GetRec(const int& RecN){return VarValVV[RecN];}
  int AddRec(const TFltV& VarValV){
    IAssert(VarValV.Len()==GetVars()); return VarValVV.Add(VarValV);}
  int AddRec(){
    VarValVV.Add(); TFltV& VarValV=VarValVV.Last();
    VarValV.Gen(GetVars()); VarValV.PutAll(-1); return VarValVV.Len()-1;}
  void GetRecNV(
   const TSecTm& StartDate, const TSecTm& EndDate,
   const int& StartRecN, const int& EndRecN, const double& MnCspt,
   TIntV& RecNV);

  // variable value retrieval
  void PutVarVal(const int& RecN, const TStr& VarNm, const double& Val){
    int VarN=GetVarN(VarNm); VarValVV[RecN][VarN]=Val;}
  double GetVarVal(const int& RecN, const int& VarN) const {
    return VarValVV[RecN][VarN];}
  double GetVarVal(const int& RecN, const TStr& VarNm) const {
    return GetVarVal(RecN, GetVarN(VarNm));}

  // class/attribute value retrieval
  double GetClassVal(const int& RecN) const {
    return VarValVV[RecN][ClassVarN];}
  TSecTm GetDateVal(const int& RecN) const {
    return TSecTm(int(VarValVV[RecN][DateVarN]));}
  int GetHourN(const int& RecN) const {
    return GetAttrVal(RecN, "Hour", -1);}
  double GetVolVal(const int& RecN) const {
    return VarValVV[RecN][VolVarN];}
  void GetAttrValV(const int& RecN, TFltV& AttrValV) const;
  double GetAttrVal(const int& RecN, const int& AttrN, const double& DfVal) const;
  double GetAttrVal(const int& RecN, const TStr& AttrNm, const double& DfVal) const;
  TStr GetBitsStr(const int& RecN) const;

  // default domains
  static PGasDm GetWeekGasDm(const PGasLtpBs& GasLtpBs);
  static PGasDm GetDayGasDm(const PGasLtpBs& GasLtpBs);
  static PGasDm GetHourBlockGasDm(const PGasLtpBs& GasLtpBs, const int& HourBlockLen);
  static PGasDm GetHourGasDm(const PGasLtpBs& GasLtpBs);
  static void AddCsptBlock(TIntIntH& HourNToCsptH, const int& HourBlockN, const int& Cspt);

  // add & update new domain records
  int AddWeekRec(const double& Cspt=-1);
  int AddDayRec(const double& Cspt=-1);
  int AddHourBlockRec(const double& Cspt=-1);
  int AddTmResRec(const double& Cspt=-1);
  void AssureDayRec(const int& RecN, const double& Cspt=-1){
    if (RecN>=GetRecs()){AddDayRec(Cspt);}}
  void PutCsptIfEmpty(const int& RecN, const double& Cspt){
    if (GetVarVal(RecN, "Cspt")==-1){PutVarVal(RecN, "Cspt", Cspt);}}
  void DumpRec(const int& RecN) const;
  void DumpRecs(const int& StartRecN, const int& EndRecN) const {
    for (int RecN=StartRecN; RecN<=EndRecN; RecN++){DumpRec(RecN);}}
};

/////////////////////////////////////////////////
// Gas-Domain-Header
ClassTP(TGasDmHd, PGasDmHd)//{
public:
  TStrKdV VarNmTyKdV;
public:
  TGasDmHd(const PGasDm& GasDm):
    VarNmTyKdV(GasDm->VarNmTyKdV){}
  static PGasDmHd New(const PGasDm& GasDm){
    return new TGasDmHd(GasDm);}
  TGasDmHd(TSIn& SIn): VarNmTyKdV(SIn){}
  static PGasDmHd Load(TSIn& SIn){return new TGasDmHd(SIn);}
  void Save(TSOut& SOut){VarNmTyKdV.Save(SOut);}

  TGasDmHd& operator=(const TGasDmHd& GasDmHd){
    if (this!=&GasDmHd){VarNmTyKdV=GasDmHd.VarNmTyKdV;} return *this;}
  bool operator==(const TGasDmHd& GasDmHd) const {
    return VarNmTyKdV==GasDmHd.VarNmTyKdV;}

  // compatibility with domain
  bool IsCompatible(const PGasDm& GasDm) const {
    return VarNmTyKdV==GasDm->VarNmTyKdV;}

  // variables
  int GetVars() const {return VarNmTyKdV.Len();}
  TStr GetVarNm(const int& VarN) const {return VarNmTyKdV[VarN].Key;}
  TStr GetVarTy(const int& VarN) const {return VarNmTyKdV[VarN].Dat;}

  // names
  void GetAttrNmV(TStrV& AttrNmV) const;
  TStr GetAttrNm(const int& AttrN) const {
    TStrV AttrNmV; GetAttrNmV(AttrNmV); return AttrNmV[AttrN];}
  TStr GetClassNm() const;
};

/////////////////////////////////////////////////
// Gas-Model-Type
typedef enum {gmtUndef,
 gmtCTb, gmtLr, gmtNNbr, gmtSvm, gmtCubist, gmtRegTree, gmtEnsemble,
 gmtMx} TGasMdType;

/////////////////////////////////////////////////
// Gas-Model-Multi-Type
ClassTP(TGasMdMType, PGasMdMType)//{
private:
  TIntFltPrV MdTypeWgtPrV;
public:
  TGasMdMType(): MdTypeWgtPrV(){}
  static PGasMdMType New(){return PGasMdMType(new TGasMdMType());}
  static PGasMdMType New(const TGasMdType& MdType){
    PGasMdMType MdMType=New(); MdMType->AddMdTypeWgt(MdType, 1); return MdMType;}
  TGasMdMType(TSIn& SIn): MdTypeWgtPrV(SIn){}
  static PGasMdMType Load(TSIn& SIn){return new TGasMdMType(SIn);}
  void Save(TSOut& SOut){MdTypeWgtPrV.Save(SOut);}

  TGasMdMType& operator=(const TGasMdMType& GasMdMType){
    if (this!=&GasMdMType){MdTypeWgtPrV=GasMdMType.MdTypeWgtPrV;} return *this;}

  TStr GetDescStr() const;

  int GetMdTypes() const {return MdTypeWgtPrV.Len();}
  TGasMdType GetMdType(const int& MdTypeN) const {
    return TGasMdType(int(MdTypeWgtPrV[MdTypeN].Val1));}
  double GetMdWgt(const int& MdTypeN) const {
    return MdTypeWgtPrV[MdTypeN].Val2;}
  void AddMdTypeWgt(const TGasMdType& MdType, const double& MdWgt=1);
  void NrmMdWgt();
};

/////////////////////////////////////////////////
// Gas-Model
ClassTPV(TGasMd, PGasMd, TGasMdV)//{
public:
  PGasDmHd GasDmHd;
public:
  TGasMd(const PGasDmHd& _GasDmHd): GasDmHd(_GasDmHd){}
  virtual ~TGasMd(){}
  TGasMd(TSIn& SIn): GasDmHd(SIn){}
  static PGasMd Load(TSIn& SIn){return new TGasMd(SIn);}
  void Save(TSOut& SOut){GasDmHd.Save(SOut);}

  TGasMd& operator=(const TGasMd& GasMd){
    if (this!=&GasMd){GasDmHd=GasMd.GasDmHd;} return *this;}

  PGasDmHd GetGasDmHd() const {return GasDmHd;}

  virtual PMom GetClassValMom() const {Fail; return NULL;}
  virtual double GetPredClassVal(const PGasDm& GasDm, const int& RecN) const {
    Fail; return 0;}
  double GetCorPredClassVal(
   const PGasDm& GasDm, const int& RecN, const double& CorFact=1.0) const;
  virtual TStr GetPredExpl(const PGasDm& GasDm, const int& RecN) const {
    Fail; return "";}

  virtual void Print() const {Fail;}

  static PGasMd New(
   const TGasMdType& GasMdType,
   const PGasMdMType& GasMdMType, const TStr& GasMdParamStr,
   const PGasDm& GasDm,
   const TSecTm& StartDate, const TSecTm& EndDate,
   const int& StartRecN, const int& EndRecN, const double& MnCspt);

  static TStr GetGasMdTypeNm(const TGasMdType& GasMdType);
  static TStr GetGasMdTypeBriefNm(const TGasMdType& GasMdType);
  static TStr GetMdParamStr();
};

/////////////////////////////////////////////////
// Gas-Model-Ensemble
ClassTE(TGasMdEnsemble, TGasMd)//{
public:
  PGasMdMType GasMdMType;
  TGasMdV GasMdV;
public:
  TGasMdEnsemble(const PGasDmHd& GasDmHd, const PGasMdMType& _GasMdMType):
    TGasMd(GasDmHd), GasMdMType(_GasMdMType), GasMdV(){}
  static PGasMd New(
   const PGasMdMType& GasMdMType, const TStr& ParamStr, const PGasDm& GasDm,
   const TSecTm& StartDate, const TSecTm& EndDate,
   const int& StartRecN, const int& EndRecN, const double& MnCspt);
  TGasMdEnsemble(TSIn& SIn):
    TGasMd(SIn), GasMdMType(SIn), GasMdV(SIn){}
  static PGasMd Load(TSIn& SIn){return new TGasMdEnsemble(SIn);}
  void Save(TSOut& SOut){
    TGasMd::Save(SOut); GasMdMType.Save(SOut); GasMdV.Save(SOut);}

  TGasMdEnsemble& operator=(const TGasMdEnsemble& GasMdEnsemble){
    Fail; if (this!=&GasMdEnsemble){} return *this;}

  double GetPredClassVal(const PGasDm& GasDm, const int& RecN) const;
  PMom GetClassValMom() const;

  void Print() const;
};

/////////////////////////////////////////////////
// Gas-Model-Contingency-Tables
ClassTE(TGasMdCTb, TGasMd)//{
public:
  TStr MomNm;
  TInt HourAttrN, DowAttrN, MonthAttrN;
  PMom ClassValMom;
  TMomV HourMomV, DowMomV, MonthMomV;
  TMomVV DowHourMomVV;
public:
  TGasMdCTb(const PGasDmHd& GasDmHd):
    TGasMd(GasDmHd), MomNm(),
    HourAttrN(-1), DowAttrN(-1), MonthAttrN(-1),
    ClassValMom(), HourMomV(), DowMomV(), MonthMomV(), DowHourMomVV(){}
  static PGasMd New(const TStr& ParamStr, const PGasDm& GasDm,
   const TSecTm& StartDate, const TSecTm& EndDate,
   const int& StartRecN, const int& EndRecN, const double& MnCspt);
  TGasMdCTb(TSIn& SIn):
    TGasMd(SIn), MomNm(SIn),
    HourAttrN(SIn), DowAttrN(SIn), MonthAttrN(SIn),
    ClassValMom(SIn), HourMomV(SIn), DowMomV(SIn),
    MonthMomV(SIn), DowHourMomVV(SIn){}
  static PGasMd Load(TSIn& SIn){return new TGasMdCTb(SIn);}
  void Save(TSOut& SOut){
    TGasMd::Save(SOut); MomNm.Save(SOut);
    HourAttrN.Save(SOut); DowAttrN.Save(SOut); MonthAttrN.Save(SOut);
    ClassValMom.Save(SOut); HourMomV.Save(SOut); DowMomV.Save(SOut);
    MonthMomV.Save(SOut); DowHourMomVV.Save(SOut);}

  TGasMdCTb& operator=(const TGasMdCTb& GasMdCTb){
    Fail; if (this!=&GasMdCTb){} return *this;}

  double GetPredClassVal(const PGasDm& GasDm, const int& RecN) const;
  PMom GetClassValMom() const {return ClassValMom;}

  void Print() const;
};

/////////////////////////////////////////////////
// Gas-Model-Singular-Value-Decomposition
ClassTE(TGasMdSvd, TGasMd)//{
public:
  TFltV CfV;
  TFltV CfUncerV;
  TFlt ChiSq;
  PMom ClassValMom;
public:
  TGasMdSvd(const PGasDmHd& GasDmHd):
    TGasMd(GasDmHd), CfV(), CfUncerV(), ChiSq(), ClassValMom(){}
  static PGasMd New(const TStr& ParamStr, const PGasDm& GasDm,
   const TSecTm& StartDate, const TSecTm& EndDate,
   const int& StartRecN, const int& EndRecN, const double& MnCspt);
  TGasMdSvd(TSIn& SIn):
    TGasMd(SIn), CfV(SIn), CfUncerV(SIn), ChiSq(SIn), ClassValMom(SIn){}
  static PGasMd Load(TSIn& SIn){return new TGasMdSvd(SIn);}
  void Save(TSOut& SOut){
    TGasMd::Save(SOut); CfV.Save(SOut); CfUncerV.Save(SOut); ChiSq.Save(SOut);
    ClassValMom.Save(SOut);}

  TGasMdSvd& operator=(const TGasMdSvd& GasMdSvd){
    Fail; if (this!=&GasMdSvd){} return *this;}

  double GetPredClassVal(const PGasDm& GasDm, const int& RecN) const;
  PMom GetClassValMom() const {return ClassValMom;}

  void Print() const;
};

/////////////////////////////////////////////////
// Gas-Model-Nearest-Neighbour
ClassTE(TGasMdNNbr, TGasMd)//{
public:
  TInt NNbrs;
  TVec<TFltV> AttrValVV;
  TMomV AttrValMomV;
  TFltV ClassValV;
  PMom ClassValMom;
public:
  TGasMdNNbr(const PGasDmHd& GasDmHd):
    TGasMd(GasDmHd), NNbrs(-1), AttrValVV(), AttrValMomV(),
    ClassValV(), ClassValMom(){}
  static PGasMd New(const TStr& ParamStr, const PGasDm& GasDm,
   const TSecTm& StartDate, const TSecTm& EndDate,
   const int& StartRecN, const int& EndRecN, const double& MnCspt);
  TGasMdNNbr(TSIn& SIn):
    TGasMd(SIn), NNbrs(SIn), AttrValVV(SIn), AttrValMomV(SIn),
    ClassValV(SIn), ClassValMom(SIn){}
  static PGasMd Load(TSIn& SIn){return new TGasMdNNbr(SIn);}
  void Save(TSOut& SOut){
    TGasMd::Save(SOut); NNbrs.Save(SOut);
    AttrValVV.Save(SOut); AttrValMomV.Save(SOut);
    ClassValV.Save(SOut); ClassValMom.Save(SOut);}

  TGasMdNNbr& operator=(const TGasMdNNbr& GasMdNNbr){
    Fail; if (this!=&GasMdNNbr){} return *this;}

  PMom GetClassValMom() const {return ClassValMom;}
  double GetPredClassVal(const PGasDm& GasDm, const int& RecN) const;
  TStr GetPredExpl(const PGasDm& GasDm, const int& RecN) const;

  void Print() const;
};

/////////////////////////////////////////////////
// Gas-Model-Cubist
ClassTE(TGasMdCubist, TGasMd)//{
public:
  TStr IdFNm;
  TStr NamesFileStr, DataFileStr, ModelFileStr;
  PMom ClassValMom;
public:
  TGasMdCubist(const PGasDmHd& GasDmHd):
    TGasMd(GasDmHd), IdFNm(),
    NamesFileStr(), DataFileStr(), ModelFileStr(),
    ClassValMom(){}
  static PGasMd New(const TStr& ParamStr, const PGasDm& GasDm,
   const TSecTm& StartDate, const TSecTm& EndDate,
   const int& StartRecN, const int& EndRecN, const double& MnCspt);
  TGasMdCubist(TSIn& SIn):
    TGasMd(SIn), IdFNm(SIn),
    NamesFileStr(SIn), DataFileStr(SIn), ModelFileStr(SIn),
    ClassValMom(SIn){}
  static PGasMd Load(TSIn& SIn){return new TGasMdCubist(SIn);}
  void Save(TSOut& SOut){
    TGasMd::Save(SOut); IdFNm.Save(SOut);
    NamesFileStr.Save(SOut); DataFileStr.Save(SOut); ModelFileStr.Save(SOut);
    ClassValMom.Save(SOut);}

  TGasMdCubist& operator=(const TGasMdCubist& GasMdCubist){
    Fail; if (this!=&GasMdCubist){} return *this;}

  double GetPredClassVal(const PGasDm& GasDm, const int& RecN) const;
  PMom GetClassValMom() const {return ClassValMom;}

  void Print() const;
};

/////////////////////////////////////////////////
// Gas-Model-Svm
ClassTE(TGasMdSvm, TGasMd)//{
public:
  TStr IdFNm;
  TStr ModelFileStr;
  PMom ClassValMom;
public:
  TGasMdSvm(const PGasDmHd& GasDmHd):
    TGasMd(GasDmHd), IdFNm(),
    ModelFileStr(), ClassValMom(){}
  static PGasMd New(const TStr& ParamStr, const PGasDm& GasDm,
   const TSecTm& StartDate, const TSecTm& EndDate,
   const int& StartRecN, const int& EndRecN, const double& MnCspt);
  TGasMdSvm(TSIn& SIn):
    TGasMd(SIn), IdFNm(SIn),
    ModelFileStr(SIn), ClassValMom(SIn){}
  static PGasMd Load(TSIn& SIn){return new TGasMdSvm(SIn);}
  void Save(TSOut& SOut){
    TGasMd::Save(SOut); IdFNm.Save(SOut);
    ModelFileStr.Save(SOut); ClassValMom.Save(SOut);}

  TGasMdSvm& operator=(const TGasMdSvm& GasMdSvm){
    Fail; if (this!=&GasMdSvm){} return *this;}

  double GetPredClassVal(const PGasDm& GasDm, const int& RecN) const;
  PMom GetClassValMom() const {return ClassValMom;}

  void Print() const;
};

/////////////////////////////////////////////////
// Gas-Regression-Node
ClassTP(TGasRegNd, PGasRegNd)//{
public:
  PMom ClassValMom;
  TFlt SplitQual;
  TInt SplitAttrN;
  TFlt SplitAttrVal;
  PGasRegNd LRegNd, RRegNd;
public:
  TGasRegNd():
    ClassValMom(), SplitQual(-1),
    SplitAttrN(-1), SplitAttrVal(0),
    LRegNd(), RRegNd(){}
  static PGasRegNd New(){return new TGasRegNd();}
  static PGasRegNd TGasRegNd::New(const int& MnLeafExs, 
   const TVec<TFltV>& AttrValVV, const TFltV& ClassValV, const TIntV& ExNV);
  TGasRegNd(TSIn& SIn):
    ClassValMom(SIn), SplitQual(SIn),
    SplitAttrN(SIn), SplitAttrVal(SIn),
    LRegNd(SIn), RRegNd(SIn){}
  static PGasRegNd Load(TSIn& SIn){return new TGasRegNd(SIn);}
  void Save(TSOut& SOut){
    ClassValMom.Save(SOut); SplitQual.Save(SOut);
    SplitAttrN.Save(SOut); SplitAttrVal.Save(SOut);
    LRegNd.Save(SOut); RRegNd.Save(SOut);}

  TGasRegNd& operator=(const TGasRegNd& GasRegNd){
    Fail; if (this!=&GasRegNd){} return *this;}

  bool IsLeaf() const {return double(SplitQual)==-1;}
  double GetPredClassVal(const TFltV& AttrValV) const;

  void Print(const PGasDmHd& GasDmHd, const int& Levs=0) const;
};

/////////////////////////////////////////////////
// Gas-Model-RegTree
ClassTE(TGasMdRegTree, TGasMd)//{
public:
  PGasRegNd RegNd;
  PMom ClassValMom;
public:
  TGasMdRegTree(const PGasDmHd& GasDmHd):
    TGasMd(GasDmHd), RegNd(), ClassValMom(){}
  static PGasMd New(const TStr& ParamStr, const PGasDm& GasDm,
   const TSecTm& StartDate, const TSecTm& EndDate,
   const int& StartRecN, const int& EndRecN, const double& MnCspt);
  TGasMdRegTree(TSIn& SIn):
    TGasMd(SIn), RegNd(SIn), ClassValMom(SIn){}
  static PGasMd Load(TSIn& SIn){return new TGasMdRegTree(SIn);}
  void Save(TSOut& SOut){
    TGasMd::Save(SOut); RegNd.Save(SOut); ClassValMom.Save(SOut);}

  TGasMdRegTree& operator=(const TGasMdRegTree& GasMdRegTree){
    Fail; if (this!=&GasMdRegTree){} return *this;}

  double GetPredClassVal(const PGasDm& GasDm, const int& RecN) const;
  PMom GetClassValMom() const {return ClassValMom;}

  void Print() const;
};

/////////////////////////////////////////////////
// Gas-Model-Correction
ClassTP(TGasMdCor, PGasMdCor)//{
private:
  typedef TPair<TSecTm, TStr> TDateProdNmPr;
  THash<TDateProdNmPr, PMom> DateProdNmToCorFactMomH;
public:
  TGasMdCor(const TStr& LtpFPath);
  static PGasMdCor New(const TStr& LtpFPath){return new TGasMdCor(LtpFPath);}
  TGasMdCor(TSIn& SIn): DateProdNmToCorFactMomH(SIn){}
  static PGasMdCor Load(TSIn& SIn){return new TGasMdCor(SIn);}
  void Save(TSOut& SOut){DateProdNmToCorFactMomH.Save(SOut);}

  TGasMdCor& operator=(const TGasMdCor& GasMdCor){Fail; return *this;}

  // corection factor retrieval
  double GetCorFact(const TSecTm& Date, const TStr& ProdNm) const;

  // files
  static PGasMdCor LoadBin(const TStr& FNm){
    TFIn FIn(FNm); return new TGasMdCor(FIn);}
  void SaveBin(const TStr& FNm){
    TFOut FOut(FNm); Save(FOut);}
  void SaveTxt(const TStr& FNm) const;
};

/////////////////////////////////////////////////
// Gas-Prediction
ClassTPV(TGasPred, PGasPred, TGasPredV)//{
public:
  TFlt CorrVal;
  TFlt PredVal;
  TFlt AbsErr;
  TFlt RelErr;
  TSecTm Tm;
  TBool OutlierP;
  TStr DescStr;
public:
  TGasPred(){}
  static PGasPred New(){return new TGasPred();}
  TGasPred(TSIn& SIn){Fail;}
  static PGasPred Load(TSIn& SIn){return new TGasPred(SIn);}
  void Save(TSOut& SOut){Fail;}

  TGasPred& operator=(const TGasPred& GasPred){
    Fail; if (this!=&GasPred){} return *this;}
};


/////////////////////////////////////////////////
// Gas-Order
ClassTPV(TGasOrder, PGasOrder, TGasOrderV)//{
public:
  TSecTm IssueDt;
  TSecTm ReceiveDt;
  TFlt Quantity;
  TBool ApprovedP;
public:
  TGasOrder(){}
  TGasOrder(
   const TSecTm& _IssueDt, const TSecTm& _ReceiveDt, const double& _Quantity, const bool& _ApprovedP):
    IssueDt(_IssueDt), ReceiveDt(_ReceiveDt), Quantity(_Quantity), ApprovedP(_ApprovedP){}
  static PGasOrder New(
   const TSecTm& IssueDt, const TSecTm& ReceiveDt, const double& Quantity, const bool& ApprovedP){
    return new TGasOrder(IssueDt, ReceiveDt, Quantity, ApprovedP);}
  TGasOrder(TSIn& SIn):
    IssueDt(SIn), ReceiveDt(SIn), Quantity(SIn), ApprovedP(SIn){}
  static PGasOrder Load(TSIn& SIn){return new TGasOrder(SIn);}
  void Save(TSOut& SOut){
    IssueDt.Save(SOut); ReceiveDt.Save(SOut); Quantity.Save(SOut); ApprovedP.Save(SOut);}

  TGasOrder& operator=(const TGasOrder& GasOrder){
    IssueDt=GasOrder.IssueDt; ReceiveDt=GasOrder.ReceiveDt;
    Quantity=GasOrder.Quantity; ApprovedP=GasOrder.ApprovedP;
    return *this;}
  bool operator==(const TGasOrder& GasOrder) const {
    return ReceiveDt==GasOrder.ReceiveDt;}
  bool operator<(const TGasOrder& GasOrder) const {
    return ReceiveDt<GasOrder.ReceiveDt;}

  static double GetQuantity(
   const TSecTm& Dt, TGasOrderV& GasOrderV);
  static double GetQuantity(
   const TSecTm& MnTm, const TSecTm& MxTm, TGasOrderV& GasOrderV);
  static void DelOrders(const TSecTm& Dt, TGasOrderV& GasOrderV);
  static void DelOrders(
   const TSecTm& MnTm, const TSecTm& MxTm, TGasOrderV& GasOrderV);
  static void DelUnapprovedOrders(
   const TSecTm& MnTm, TGasOrderV& GasOrderV, TStrV& MsgStrV);

  TStr GetStr() const {
    return TStr("Order[")+
     "Issue:"+IssueDt.GetStr()+
     " Receive:"+ReceiveDt.GetStr()+
     " Quantity:"+TFlt::GetStr(Quantity, "%.0f")+"]";
  }

  static PGasOrder LoadXml(const PXmlTok& XmlTok);
  void SaveXml(const PSOut& SOut, const TStr& IndentStr) const;
};

/////////////////////////////////////////////////
// Gas-Station
ClassTP(TGasStation, PGasStation)//{
public:
  // definition data
  TStr DescStr;
  TStr ProdNm;
  TFlt FullVolumeVal;
  TFlt SafetyVolumeVal;
  TInt DeliveryDays;
  TInt StockDurationDays;
  TFlt OrderQuantityCorFact;
  TFlt ExceptPredCorFact;
  TInt HistWnDays;
  TInt TmGridHours;
  // state data
  TFlt VolumeVal;
  TGasOrderV GasOrderV;
  TGasOrderV NewGasOrderV;
public:
  TGasStation(){}
  static PGasStation New(){return new TGasStation();}
  ~TGasStation(){}
  TGasStation(TSIn& SIn):
    DescStr(SIn), ProdNm(SIn),
    FullVolumeVal(SIn), SafetyVolumeVal(SIn),
    DeliveryDays(SIn), StockDurationDays(SIn),
    OrderQuantityCorFact(SIn), ExceptPredCorFact(SIn),
    HistWnDays(SIn), TmGridHours(SIn),
    VolumeVal(SIn), GasOrderV(SIn), NewGasOrderV(SIn){}
  static PGasStation Load(TSIn& SIn){return new TGasStation(SIn);}
  void Save(TSOut& SOut){
    DescStr.Save(SOut); ProdNm.Save(SOut);
    FullVolumeVal.Save(SOut); SafetyVolumeVal.Save(SOut);
    DeliveryDays.Save(SOut); StockDurationDays.Save(SOut);
    OrderQuantityCorFact.Save(SOut); ExceptPredCorFact.Save(SOut);
    HistWnDays.Save(SOut); TmGridHours.Save(SOut);
    VolumeVal.Save(SOut); GasOrderV.Save(SOut); NewGasOrderV.Save(SOut);}

  TGasStation& operator=(const TGasStation&){
    Fail; return *this;}

  static PGasStation LoadXml(const PXmlTok& XmlTok);
  static PGasStation LoadXml(const TStr& FNm);
  void SaveXml(const PSOut& SOut) const;
  void SaveXml(const TStr& FNm) const {
    PSOut SOut=TFOut::New(FNm); SaveXml(SOut);}

  void DumpDef(FILE* fOut=stdout) const;
};

/////////////////////////////////////////////////
// Gas-Simulation-Record
ClassTPV(TGasSimRec, PGasSimRec, TGasSimRecV)//{
public:
  PGasStation GasStation;
  TStr TmStr;
  TFlt Real_Vol, Real_Cspt, Real_Order;
  TStrV Sim_TmStrV;
  TFltV Sim_VolV, Sim_PrevVolV, Sim_CsptV, Sim_OrderV;
  TStrV Ord_TmStrV;
  TFltV Ord_QuantV, Ord_PrevQuantV, Ord_CsptV;
  TFlt Ord_CorrQuant;
public:
  TGasSimRec(const PGasStation& _GasStation, const TStr& _TmStr):
    GasStation(_GasStation), TmStr(_TmStr){}
  static PGasSimRec New(const PGasStation& GasStation, const TStr& TmStr){
    return new TGasSimRec(GasStation, TmStr);}
  ~TGasSimRec(){}
  TGasSimRec(TSIn& SIn):
    GasStation(SIn), TmStr(SIn),
    Real_Vol(SIn), Real_Cspt(SIn), Real_Order(SIn),
    Sim_TmStrV(SIn),
    Sim_VolV(SIn), Sim_PrevVolV(SIn), Sim_CsptV(SIn), Sim_OrderV(SIn),
    Ord_TmStrV(SIn),
    Ord_QuantV(SIn), Ord_PrevQuantV(SIn), Ord_CsptV(SIn),
    Ord_CorrQuant(SIn){}
  static PGasSimRec Load(TSIn& SIn){return new TGasSimRec(SIn);}
  void Save(TSOut& SOut){
    GasStation.Save(SOut); TmStr.Save(SOut);
    Real_Vol.Save(SOut); Real_Cspt.Save(SOut); Real_Order.Save(SOut);
    Sim_TmStrV.Save(SOut);
    Sim_VolV.Save(SOut); Sim_PrevVolV.Save(SOut);
    Sim_CsptV.Save(SOut); Sim_OrderV.Save(SOut);
    Ord_TmStrV.Save(SOut);
    Ord_QuantV.Save(SOut); Ord_PrevQuantV.Save(SOut); Ord_CsptV.Save(SOut);
    Ord_CorrQuant.Save(SOut);}

  TGasSimRec& operator=(const TGasSimRec&){
    Fail; return *this;}

  // global properties
  PGasStation GetGasStation() const {return GasStation;}
  TStr GetTmStr() const {return TmStr;}

  // real properties
  void PutReal(
   const double& _Real_Vol, const double& _Real_Cspt, const double& _Real_Order){
    Real_Vol=_Real_Vol; Real_Cspt=_Real_Cspt; Real_Order=_Real_Order;}
  double GetRealVol() const {return Real_Vol;}
  double GetRealCspt() const {return Real_Cspt;}
  double GetRealOrder() const {return Real_Order;}

  // simulation steps
  void AddSimStep(const TStr& TmStr, const double& Vol, const double& PrevVol,
   const double& Cspt, const double& Order){
    Sim_TmStrV.Add(TmStr);
    Sim_VolV.Add(Vol); Sim_PrevVolV.Add(PrevVol);
    Sim_CsptV.Add(Cspt); Sim_OrderV.Add(Order);}
  int GetSimSteps() const {return Sim_TmStrV.Len();}
  TStr GetSimStepTmStr(const int& SimStepN) const {
    return Sim_TmStrV[SimStepN];}
  void GetSimStep(const int& SimStepN, TStr& TmStr,
   double& Vol, double& PrevVol, double& Cspt, double& Order) const {
    TmStr=Sim_TmStrV[SimStepN];
    Vol=Sim_VolV[SimStepN]; PrevVol=Sim_PrevVolV[SimStepN];
    Cspt=Sim_CsptV[SimStepN]; Order=Sim_OrderV[SimStepN];}

  // order-quantity-steps
  void AddOrdStep(const TStr& TmStr,
   const double& Quant, const double& PrevQuant, const double& Cspt){
    Ord_TmStrV.Add(TmStr);
    Ord_QuantV.Add(Quant); Ord_PrevQuantV.Add(PrevQuant);
    Ord_CsptV.Add(Cspt);}
  int GetOrdSteps() const {return Ord_TmStrV.Len();}
  void GetOrdStep(const int& OrdStepN, TStr& TmStr,
   double& Quant, double& PrevQuant, double& Cspt) const {
    TmStr=Ord_TmStrV[OrdStepN];
    Quant=Ord_QuantV[OrdStepN]; PrevQuant=Ord_PrevQuantV[OrdStepN];
    Cspt=Ord_CsptV[OrdStepN];}
  void PutCorrQuant(const double& _CorrQuant){Ord_CorrQuant=_CorrQuant;}
  double GetCorrQuant() const {return Ord_CorrQuant;}

  // binary files
  static void LoadBin(const TStr& FNm, TGasSimRecV& GasSimRecV){
    PSIn SIn=TFIn::New(FNm); GasSimRecV.Load(*SIn);}
  static void SaveBin(const TStr& FNm, const TGasSimRecV& GasSimRecV){
    PSOut SOut=TFOut::New(FNm); GasSimRecV.Save(*SOut);}
};

/////////////////////////////////////////////////
// Gas-Execution
ClassTP(TGasExe, PGasExe)//{
public:
public:
  TGasExe(){}
  static PGasExe New(){return new TGasExe();}
  TGasExe(TSIn& SIn){}
  static PGasExe Load(TSIn& SIn){return new TGasExe(SIn);}
  void Save(TSOut& SOut){}

  TGasExe& operator=(const TGasExe& GasExe){
    Fail; if (this!=&GasExe){} return *this;}

  static void GenPredEval(
   const PGasMdMType& GasMdMType, const TStr& GasMdParamStr,
   const PGasMdCor& GasMdCor, const bool& UseMdCorP,
   const PGasDm& GasDm,
   const TSecTm& MnDate, const TSecTm& MxDate,
   const int& TrainWnLen, const int& TestWnLen, const int& WnStepLen,
   const double& MnCspt, const double& OutlierRelErr,
   const TStr& LogFNm, const int& LogVerbLev, const bool& AppendToLog,
   const TStr& TabLogFNm, const bool& AppendToTabLog,
   FILE* fOlap, const TStr& OlapPfxLn,
   TGasPredV& GasPredV,
   PMom& AllPredAbsErrMom, PMom& AllPredRelErrMom,
   const bool& PrintToScrP);

  static void GenDayOrder(
   const TStr& LogFNm,
   const PGasStation& GasStation, const TStr& OutGasStationFNm,
   const TSecTm& StartDt, const int& ProcessDays,
   const PGasDm& GasDm,
   const PGasMdMType& GasMdMType, const TStr& GasMdParamStr,
   TGasSimRecV& GasSimRecV);

  static void GenHourBlockOrder(
   const TStr& LogFNm,
   const PGasStation& GasStation, const TStr& OutGasStationFNm,
   const TSecTm& StartDt, const int& ProcessDays,
   const PGasDm& GasDm,
   const PGasMdMType& GasMdMType, const TStr& GasMdParamStr,
   TGasSimRecV& GasSimRecV);

  static int GenPrediction(
   const TStr& TabFNm,
   const TStr& _LtpFNm,
   const TStr& LtpFPath,
   const bool& RepairP,
   const TStr& OutLogFNm,
   const TStr& OutXmlFNm,
   const TStr& MdNm,
   const TStr& TmResNm,
   const TStr& PredDateStr,
   const int& PredDays,
   const int& TrainDays,
   const double& MnCspt,
   const double& ExceptPredCorFact);

  static int GenOrder(
   const TStr& TabFNm,
   const TStr& LtpFNm,
   const TStr& InGasStationFNm,
   const TStr& OutGasStationFNm,
   const TStr& OutLogFNm,
   const TStr& OutSimFNm,
   const TStr& StartDateStr,
   const int& ProcessDays);
};

#endif
