/////////////////////////////////////////////////
// Includes
#include "ultra.h"

/////////////////////////////////////////////////
// Gas-Definitions
TGasDef::TGasDef(const bool& DbP):
  FldNmTyUseTrV(){
  if (DbP){
    LocIdFldN=FldNmTyUseTrV.Add(TStrTr("LocationID", "int", "meta"));
    ProdIdFldN=FldNmTyUseTrV.Add(TStrTr("ProductID", "int", "meta"));
    DateFldN=FldNmTyUseTrV.Add(TStrTr("Date", "date", "active"));
    DayNFldN=FldNmTyUseTrV.Add(TStrTr("Day", "int", "active"));
    MonthNFldN=FldNmTyUseTrV.Add(TStrTr("Month", "int", "active"));
    YearNFldN=FldNmTyUseTrV.Add(TStrTr("Year", "int", "active"));
    HourNFldN=FldNmTyUseTrV.Add(TStrTr("HourStart", "int", "active"));
    FldNmTyUseTrV.Add(TStrTr("HourEnd", "int", "active"));
    DowNFldN=FldNmTyUseTrV.Add(TStrTr("DayOfWeek", "dow", "active"));
    IsHolidayFldN=FldNmTyUseTrV.Add(TStrTr("ISHoliday", "int", "active"));
    IsHolidayStartFldN=FldNmTyUseTrV.Add(TStrTr("IsHolidayStart", "int", "active"));
    IsHolidayEndFldN=FldNmTyUseTrV.Add(TStrTr("IsHolidayEnd", "int", "active"));
    IsDayBeforeHolidayFldN=FldNmTyUseTrV.Add(TStrTr("IsDayBeforeHoliday", "int", "active"));
    IsDayAfterHolidayFldN=FldNmTyUseTrV.Add(TStrTr("IsDayAfterHoliday", "int", "active"));
    VolFldN=FldNmTyUseTrV.Add(TStrTr("VolumeStart", "int", "active"));
    CsptFldN=FldNmTyUseTrV.Add(TStrTr("Cnsmptn", "int", "active"));
    FldNmTyUseTrV.Add(TStrTr("Interpolation", "int", "active"));
    CsptLast1WFldN=FldNmTyUseTrV.Add(TStrTr("CnsmptnLast1W", "int", "active"));
    CsptLast2WFldN=FldNmTyUseTrV.Add(TStrTr("CnsmptnLast2W", "int", "active"));
    CsptLast3WFldN=FldNmTyUseTrV.Add(TStrTr("CnsmptnLast3W", "int", "active"));
    SumCsptLast1WFldN=FldNmTyUseTrV.Add(TStrTr("SumCnsmptnLast1W", "int", "active"));
    SumCsptLast2WFldN=FldNmTyUseTrV.Add(TStrTr("SumCnsmptnLast2W", "int", "active"));
    SumCsptLast3WFldN=FldNmTyUseTrV.Add(TStrTr("SumCnsmptnLast3W", "int", "active"));
    CsptLast1DFldN=FldNmTyUseTrV.Add(TStrTr("CnsmptnLast1D", "int", "active"));
    CsptLast2DFldN=FldNmTyUseTrV.Add(TStrTr("CnsmptnLast2D", "int", "active"));
    CsptLast3DFldN=FldNmTyUseTrV.Add(TStrTr("CnsmptnLast3D", "int", "active"));
    CsptLast4DFldN=FldNmTyUseTrV.Add(TStrTr("CnsmptnLast4D", "int", "active"));
    CsptLast5DFldN=FldNmTyUseTrV.Add(TStrTr("CnsmptnLast5D", "int", "active"));
    CsptLast6DFldN=FldNmTyUseTrV.Add(TStrTr("CnsmptnLast6D", "int", "active"));
    SumCsptLast1DFldN=FldNmTyUseTrV.Add(TStrTr("SumCnsmptnLast1D", "int", "active"));
    SumCsptLast2DFldN=FldNmTyUseTrV.Add(TStrTr("SumCnsmptnLast2D", "int", "active"));
    SumCsptLast3DFldN=FldNmTyUseTrV.Add(TStrTr("SumCnsmptnLast3D", "int", "active"));
    SumCsptLast4DFldN=FldNmTyUseTrV.Add(TStrTr("SumCnsmptnLast4D", "int", "active"));
    SumCsptLast5DFldN=FldNmTyUseTrV.Add(TStrTr("SumCnsmptnLast5D", "int", "active"));
    SumCsptLast6DFldN=FldNmTyUseTrV.Add(TStrTr("SumCnsmptnLast6D", "int", "active"));
    PriceChangeFldN=FldNmTyUseTrV.Add(TStrTr("PriceChange", "int", "active"));
    DaysBeforePriceChangeFldN=FldNmTyUseTrV.Add(TStrTr("DaysBeforePriceChange", "int", "active"));
  } else {
    LocIdFldN=FldNmTyUseTrV.Add(TStrTr("LocationID", "int", "meta"));
    TankIdFldN=FldNmTyUseTrV.Add(TStrTr("TankID", "int", "meta"));
    ProdIdFldN=FldNmTyUseTrV.Add(TStrTr("ProductID", "int", "meta"));
    DateFldN=FldNmTyUseTrV.Add(TStrTr("Date", "date", "active"));
    HourNFldN=FldNmTyUseTrV.Add(TStrTr("HourStart", "int", "active"));
    FldNmTyUseTrV.Add(TStrTr("HourEnd", "int", "active"));
    CsptFldN=FldNmTyUseTrV.Add(TStrTr("Cnsmptn", "int", "active"));
    VolFldN=FldNmTyUseTrV.Add(TStrTr("VolumeStart", "int", "active"));
    FldNmTyUseTrV.Add(TStrTr("Interpolation", "int", "active"));
    CsptLast1WFldN=FldNmTyUseTrV.Add(TStrTr("CnsmptnLast1W", "int", "active"));
    CsptLast2WFldN=FldNmTyUseTrV.Add(TStrTr("CnsmptnLast2W", "int", "active"));
    CsptLast3WFldN=FldNmTyUseTrV.Add(TStrTr("CnsmptnLast3W", "int", "active"));
    SumCsptLast1WFldN=FldNmTyUseTrV.Add(TStrTr("SumCnsmptnLast1W", "int", "active"));
    SumCsptLast2WFldN=FldNmTyUseTrV.Add(TStrTr("SumCnsmptnLast2W", "int", "active"));
    SumCsptLast3WFldN=FldNmTyUseTrV.Add(TStrTr("SumCnsmptnLast3W", "int", "active"));
    CsptLast1DFldN=FldNmTyUseTrV.Add(TStrTr("CnsmptnLast1D", "int", "active"));
    CsptLast2DFldN=FldNmTyUseTrV.Add(TStrTr("CnsmptnLast2D", "int", "active"));
    CsptLast3DFldN=FldNmTyUseTrV.Add(TStrTr("CnsmptnLast3D", "int", "active"));
    CsptLast4DFldN=FldNmTyUseTrV.Add(TStrTr("CnsmptnLast4D", "int", "active"));
    CsptLast5DFldN=FldNmTyUseTrV.Add(TStrTr("CnsmptnLast5D", "int", "active"));
    CsptLast6DFldN=FldNmTyUseTrV.Add(TStrTr("CnsmptnLast6D", "int", "active"));
    SumCsptLast1DFldN=FldNmTyUseTrV.Add(TStrTr("SumCnsmptnLast1D", "int", "active"));
    SumCsptLast2DFldN=FldNmTyUseTrV.Add(TStrTr("SumCnsmptnLast2D", "int", "active"));
    SumCsptLast3DFldN=FldNmTyUseTrV.Add(TStrTr("SumCnsmptnLast3D", "int", "active"));
    SumCsptLast4DFldN=FldNmTyUseTrV.Add(TStrTr("SumCnsmptnLast4D", "int", "active"));
    SumCsptLast5DFldN=FldNmTyUseTrV.Add(TStrTr("SumCnsmptnLast5D", "int", "active"));
    SumCsptLast6DFldN=FldNmTyUseTrV.Add(TStrTr("SumCnsmptnLast6D", "int", "active"));
    DayNFldN=FldNmTyUseTrV.Add(TStrTr("Day", "int", "active"));
    MonthNFldN=FldNmTyUseTrV.Add(TStrTr("Month", "int", "active"));
    YearNFldN=FldNmTyUseTrV.Add(TStrTr("Year", "int", "active"));
    DowNFldN=FldNmTyUseTrV.Add(TStrTr("DayOfWeek", "dow", "active"));
    IsHolidayFldN=FldNmTyUseTrV.Add(TStrTr("IsHoliday", "int", "active"));
    IsHolidayStartFldN=FldNmTyUseTrV.Add(TStrTr("IsHolidayStart", "int", "active"));
    IsHolidayEndFldN=FldNmTyUseTrV.Add(TStrTr("IsHolidayEnd", "int", "active"));
    IsDayBeforeHolidayFldN=FldNmTyUseTrV.Add(TStrTr("IsDayBeforeHoliday", "int", "active"));
    IsDayAfterHolidayFldN=FldNmTyUseTrV.Add(TStrTr("IsDayAfterHoliday", "int", "active"));
    PriceChangeFldN=FldNmTyUseTrV.Add(TStrTr("PriceChange", "int", "active"));
    DaysBeforePriceChangeFldN=FldNmTyUseTrV.Add(TStrTr("DaysBeforePriceChange", "int", "active"));
    ProdNmFldN=FldNmTyUseTrV.Add(TStrTr("ProductName", "int", "ignore"));
    LocNmFldN=FldNmTyUseTrV.Add(TStrTr("LocationName", "int", "ignore"));
  }
}

bool TGasDef::IsFldNmVOk(const TStrV& FldNmV) const {
  if (GetFlds()!=FldNmV.Len()){return false;}
  for (int FldN=0; FldN<FldNmV.Len(); FldN++){
    if (FldNmTyUseTrV[FldN].Val1!=FldNmV[FldN]){
      printf("'%s'!='%s'", FldNmTyUseTrV[FldN].Val1.CStr(), FldNmV[FldN].CStr());
      return false;
    }
  }
  return true;
}

void TGasDef::GetFldIntValV(const TStrV& FldStrValV, TIntV& FldIntValV) const {
  int Flds=GetFlds();
  EAssertR(Flds==FldStrValV.Len(), "Number of data fields doesn't fit.");
  FldIntValV.Gen(Flds);
  for (int FldN=0; FldN<Flds; FldN++){
    TStrTr& FldNmTyUseTr=FldNmTyUseTrV[FldN];
    if (FldNmTyUseTr.Val3!="ignore"){
      if (FldNmTyUseTr.Val2=="int"){
        if (FldStrValV[FldN]=="True"){FldIntValV[FldN]=1;}
        else if (FldStrValV[FldN]=="False"){FldIntValV[FldN]=0;}
        else {FldIntValV[FldN]=FldStrValV[FldN].GetInt();}
      } else
      if (FldNmTyUseTr.Val2=="date"){
        TSecTm Date=TSecTm::GetDtTmFromDmyStr(FldStrValV[FldN]);
        FldIntValV[FldN]=Date.GetAbsSecs();
      } else
      if (FldNmTyUseTr.Val2=="dow"){
        TStr DowNm=FldStrValV[FldN];
        if (!DowNm.Empty()){DowNm.PutCh(0, TCh::GetUc(DowNm[0]));}
        int DowN=TTmInfo::GetDayOfWeekN(DowNm, lSi);
        FldIntValV[FldN]=DowN;
      }
    }
  }
}

TSecTm TGasDef::GetTm(const TIntV& FldIntValV) const {
  int HourN=FldIntValV[HourNFldN];
  int DayN=FldIntValV[DayNFldN];
  int MonthN=FldIntValV[MonthNFldN];
  int YearN=FldIntValV[YearNFldN];
  TSecTm Tm=TSecTm::GetDtTm(YearN, MonthN, DayN);
  Tm.AddHours(HourN);
  return Tm;
}

void TGasDef::GetHdm(
 const TIntV& FldIntValV, int& HourN, int& DowN, int& MonthN) const {
  HourN=FldIntValV[HourNFldN];
  DowN=FldIntValV[DowNFldN]-1;
  MonthN=FldIntValV[MonthNFldN]-1;
}

/////////////////////////////////////////////////
// Gas-Location/Tank/Product-Base
TSecTm TGasLtpBs::GetMnDate() const {
  TSecTm MnDate;
  int Recs=GetRecs();
  for (int RecN=0; RecN<Recs; RecN++){
    TSecTm Date=GetDate(RecN);
    if ((RecN==0)||(Date<MnDate)){MnDate=Date;}
  }
  //MnDate=TSecTm::GetDtTm(MnDate);
  return MnDate;
}

TSecTm TGasLtpBs::GetMxDate() const {
  TSecTm MxDate;
  int Recs=GetRecs();
  for (int RecN=0; RecN<Recs; RecN++){
    TSecTm Date=GetDate(RecN);
    if ((RecN==0)||(Date>MxDate)){MxDate=Date;}
  }
  //MxDate=TSecTm::GetDtTm(MxDate);
  return MxDate;
}

void TGasLtpBs::GenStat(){
  TMom::NewV(HourMomV, 24);
  TMom::NewV(DowMomV, 7);
  TMom::NewV(MonthMomV, 12);
  TMom::NewVV(DowHourMomVV, 7, 24);
  int Recs=GetRecs();
  for (int RecN=0; RecN<Recs; RecN++){
    int HourN; int DowN; int MonthN;
    GasDef->GetHdm(GetRec(RecN), HourN, DowN, MonthN);
    int Cspt=GasDef->GetCspt(GetRec(RecN));
    HourMomV[HourN]->Add(Cspt);
    DowMomV[DowN]->Add(Cspt);
    MonthMomV[MonthN]->Add(Cspt);
    DowHourMomVV.At(DowN, HourN)->Add(Cspt);
  }
  TMom::DefV(HourMomV);
  TMom::DefV(DowMomV);
  TMom::DefV(MonthMomV);
  TMom::DefVV(DowHourMomVV);
}

bool TGasLtpBs::IsDowHourMomUsable() const {
  return
   TMom::IsUsableV(HourMomV)&&
   TMom::IsUsableV(DowMomV)&&
   TMom::IsUsableVV(DowHourMomVV);
}

void TGasLtpBs::RepairUnknowns(){
  if (!IsDowHourMomUsable()){
    printf("*** Error: day-of-week & hour statistics cannot be generated (not enough data)!\n");
    printf("*** Can not repair data!\n");
    return;
  }
  // traverse, check & repair records
  int Recs=GetRecs();
  for (int RecN=0; RecN<Recs; RecN++){
    int RecHourN; int RecDowN; int RecMonthN;
    GetHdm(RecN, RecHourN, RecDowN, RecMonthN);
    TSecTm RecTm=GetTm(RecN);
    TSecTm RecDate=GetDate(RecN);
    // check if current record is one hour older
    TStr Str=RecTm.GetStr();
    if (RecN>0){
      int DSecs=TSecTm::GetDSecs(GetTm(RecN-1), RecTm);
      if (DSecs!=3600){
        printf("*** Error - records not at hour interval (%s - %s)!\n",
         GetTm(RecN-1).GetStr().CStr(), RecTm.GetStr().CStr());
        //printf("*** Can not repair data!\n");
        //return;
      }
    }
    // check consumption
    if (GetCspt(RecN)<0){
      printf("*** Error: consumption is negative (%d)!\n", GetCspt(RecN));
      printf("*** Can not repair data!\n");
      return;
    }

    // repair last days
    if (GetCsptLast1D(RecN)==-1){
      GetCsptLast1D(RecN)=DowHourMomVV.At((RecDowN+7-1)%7, RecHourN)->GetMedian();}
    if (GetCsptLast2D(RecN)==-1){
      GetCsptLast2D(RecN)=DowHourMomVV.At((RecDowN+7-2)%7, RecHourN)->GetMedian();}
    if (GetCsptLast3D(RecN)==-1){
      GetCsptLast3D(RecN)=DowHourMomVV.At((RecDowN+7-3)%7, RecHourN)->GetMedian();}
    if (GetCsptLast4D(RecN)==-1){
      GetCsptLast4D(RecN)=DowHourMomVV.At((RecDowN+7-4)%7, RecHourN)->GetMedian();}
    if (GetCsptLast5D(RecN)==-1){
      GetCsptLast5D(RecN)=DowHourMomVV.At((RecDowN+7-5)%7, RecHourN)->GetMedian();}
    if (GetCsptLast6D(RecN)==-1){
      GetCsptLast6D(RecN)=DowHourMomVV.At((RecDowN+7-6)%7, RecHourN)->GetMedian();}
    // repair sum last days
    if (GetSumCsptLast1D(RecN)==-1){
      GetSumCsptLast1D(RecN)=DowMomV[(RecDowN+7-1)%7]->GetMedian();}
    if (GetSumCsptLast2D(RecN)==-1){
      GetSumCsptLast2D(RecN)=DowMomV[(RecDowN+7-2)%7]->GetMedian();}
    if (GetSumCsptLast3D(RecN)==-1){
      GetSumCsptLast3D(RecN)=DowMomV[(RecDowN+7-1)%7]->GetMedian();}
    if (GetSumCsptLast4D(RecN)==-1){
      GetSumCsptLast4D(RecN)=DowMomV[(RecDowN+7-4)%7]->GetMedian();}
    if (GetSumCsptLast5D(RecN)==-1){
      GetSumCsptLast5D(RecN)=DowMomV[(RecDowN+7-5)%7]->GetMedian();}
    if (GetSumCsptLast6D(RecN)==-1){
      GetSumCsptLast6D(RecN)=DowMomV[(RecDowN+7-6)%7]->GetMedian();}
    // repair last weeks
    if (GetCsptLast1W(RecN)==-1){
      GetCsptLast1W(RecN)=DowHourMomVV.At(RecDowN, RecHourN)->GetMedian();}
    if (GetCsptLast2W(RecN)==-1){
      GetCsptLast2W(RecN)=DowHourMomVV.At(RecDowN, RecHourN)->GetMedian();}
    if (GetCsptLast3W(RecN)==-1){
      GetCsptLast3W(RecN)=DowHourMomVV.At(RecDowN, RecHourN)->GetMedian();}
    // repair sum last weeks
    if (GetSumCsptLast1W(RecN)==-1){
      GetSumCsptLast1W(RecN)=DowMomV[RecDowN]->GetMedian();}
    if (GetSumCsptLast2W(RecN)==-1){
      GetSumCsptLast2W(RecN)=DowMomV[RecDowN]->GetMedian();}
    if (GetSumCsptLast3W(RecN)==-1){
      GetSumCsptLast3W(RecN)=DowMomV[RecDowN]->GetMedian();}
  }
}

void TGasLtpBs::SaveStatTxt(const PSOut& SOut) const {
  SOut->PutStr("==========================================================");
  SOut->PutLn();
  SOut->PutStr(LtpNm); SOut->PutStr("  /  ");
  SOut->PutStr(LocNm); SOut->PutStr("  /  ");
  SOut->PutStr(ProdNm); SOut->PutLn();
  SOut->PutStr("Consumption per Hour");
  SOut->PutLn();
  for (int HourN=0; HourN<24; HourN++){
    SOut->PutInt(HourN); SOut->PutStr(": ");
    SOut->PutStr(HourMomV[HourN]->GetStr(' ', ':', true));
    SOut->PutLn();
  }
  SOut->PutStr("Consumption per Day-Of-Week");
  SOut->PutLn();
  for (int DowN=0; DowN<7; DowN++){
    SOut->PutStr(TTmInfo::GetDayOfWeekNm(DowN+1)); SOut->PutStr(": ");
    SOut->PutStr(DowMomV[DowN]->GetStr(' ', ':', true));
    SOut->PutLn();
  }
  SOut->PutStr("Consumption per Month");
  SOut->PutLn();
  for (int MonthN=0; MonthN<12; MonthN++){
    SOut->PutStr(TTmInfo::GetMonthNm(MonthN+1)); SOut->PutStr(": ");
    SOut->PutStr(MonthMomV[MonthN]->GetStr(' ', ':', true));
    SOut->PutLn();
  }
  SOut->PutStr("Consumption per Day-Of-Week&Hour");
  SOut->PutLn();
  {for (int DowN=0; DowN<7; DowN++){
    for (int HourN=0; HourN<24; HourN++){
      SOut->PutStr(TTmInfo::GetDayOfWeekNm(DowN+1)); SOut->PutStr("/");
      SOut->PutInt(HourN); SOut->PutStr(": ");
      SOut->PutStr(DowHourMomVV.At(DowN, HourN)->GetStr(' ', ':', true));
      SOut->PutLn();
    }
  }}
  SOut->Flush();
}

void TGasLtpBs::SaveStatBin(const PSOut& SOut) const {
  PGasLtpBs StatLtpBs=TGasLtpBs::New(LtpNm, LocNm, ProdNm);
  StatLtpBs->HourMomV=HourMomV;
  StatLtpBs->DowMomV=DowMomV;
  StatLtpBs->MonthMomV=MonthMomV;
  StatLtpBs->DowHourMomVV=DowHourMomVV;
  StatLtpBs->Save(*SOut);
  SOut->Flush();
}

void TGasLtpBs::LoadStatV(const TStr& FNm, TGasLtpBsV& GasLtpBsV){
  GasLtpBsV.Clr();
  if (TFile::Exists(FNm)){
    PSIn SIn=TFIn::New(FNm);
    while (!SIn->Eof()){
      PGasLtpBs GasLtpBs=TGasLtpBs::Load(*SIn);
      GasLtpBsV.Add(GasLtpBs);
    }
  }
}

void TGasLtpBs::ConvTabToLtp(const TStr& InTabFNmWc, const TStr& OutLtpFPath,
 const bool& RepairUnknownsP){
  // create gas definition
  PGasDef GasDef=TGasDef::New();
  // create Ltp base
  TStr OutLtpNrFPath=TStr::GetNrFPath(OutLtpFPath);
  PGasLtpBs GasLtpBs; TStr LtpNm; TSecTm PrevLtpTm;
  PSOut LtpStatTxtSOut=TFOut::New(OutLtpNrFPath+LtpStatTxtFNm);
  PSOut LtpStatBinSOut=TFOut::New(OutLtpNrFPath+LtpStatBinFNm);
  // traverse input data files
  TFFile FFile(InTabFNmWc, false); TStr FNm; int Recs=0;
  while (FFile.Next(FNm)){
    printf("Processing file '%s'\n", FNm.CStr());
    // open data file
    PSIn SIn=TFIn::New(FNm);
    // read names line
    char PrevCh=' '; TStrV FldNmV;
    TSs::LoadTxtFldV(ssfTabSep, SIn, PrevCh, FldNmV);
    EAssertR(GasDef->IsFldNmVOk(FldNmV), TStr("Field names don't fit in ")+FNm);
    // read data lines
    int LnN=0;
    while (!SIn->Eof()){
      char PrevCh=' '; TStrV FldStrValV;
      TSs::LoadTxtFldV(ssfTabSep, SIn, PrevCh, FldStrValV); LnN++;
      if (LtpNm!=GasDef->GetLtpNm(FldStrValV)){
        if (!GasLtpBs.Empty()){
          // save existing Ltp
          GasLtpBs->GenStat();
          if (RepairUnknownsP){GasLtpBs->RepairUnknowns();}
          GasLtpBs->SaveStatTxt(LtpStatTxtSOut);
          GasLtpBs->SaveStatBin(LtpStatBinSOut);
          TStr LtpFNm=OutLtpNrFPath+GasDef->GetLtpFNm(LtpNm);
          {TFOut LtpSOut(LtpFNm); GasLtpBs->Save(LtpSOut);}
          PGasLtpBs NewGasLtpBs=TGasLtpBs::Load(LtpFNm);
        }
        // prepare new Ltp
        LtpNm=GasDef->GetLtpNm(FldStrValV);
        TStr LocNm=GasDef->GetLocNm(FldStrValV);
        TStr ProdNm=GasDef->GetProdNm(FldStrValV);
        GasLtpBs=TGasLtpBs::New(LtpNm, LocNm, ProdNm, GasDef);
        PrevLtpTm.Undef();
        printf("Processing Ltp: '%s' '%s' '%s'\n",
         LtpNm.CStr(), LocNm.CStr(), ProdNm.CStr());
      }
      // add new field-value-vector
      TIntV& FldIntValV=GasLtpBs->AddRec();
      GasDef->GetFldIntValV(FldStrValV, FldIntValV);
      // check increased time
      TSecTm LtpTm=GasDef->GetTm(FldIntValV);
      if (PrevLtpTm.IsDef()){
        if (PrevLtpTm>=LtpTm){
          printf("Time is not increasing [Line:%d PrevTime:%s CurTime:%s]\n",
           LnN, PrevLtpTm.GetStr().CStr(), LtpTm.GetStr().CStr());
        }
        if (TSecTm::GetDSecs(PrevLtpTm, LtpTm)!=3600){
          printf("Not one hour difference between subsequent records "
           "[Line:%d PrevTime:%s CurTime:%s]\n",
           LnN, PrevLtpTm.GetStr().CStr(), LtpTm.GetStr().CStr());
        }
        EAssertR(PrevLtpTm<LtpTm, "Records not in increased time sequence.");
        //EAssertR(TSecTm::GetDSecs(PrevLtpTm, LtpTm)==3600, "Not one hour difference between subsequent records.");
      }
      PrevLtpTm=LtpTm;
      // update record number
      Recs++; if (Recs%1000==0){printf("%d\r", Recs);}
    }
  }
  if (!GasLtpBs.Empty()){
    // save existing Ltp
    GasLtpBs->GenStat();
    if (RepairUnknownsP){GasLtpBs->RepairUnknowns();}
    GasLtpBs->SaveStatTxt(LtpStatTxtSOut);
    GasLtpBs->SaveStatBin(LtpStatBinSOut);
    TStr LtpFNm=OutLtpNrFPath+GasDef->GetLtpFNm(LtpNm);
    {TFOut LtpSOut(LtpFNm); GasLtpBs->Save(LtpSOut);}
    PGasLtpBs NewGasLtpBs=TGasLtpBs::Load(LtpFNm);
  }
}

const TStr TGasLtpBs::LtpStatTxtFNm="LtpStat.Txt";
const TStr TGasLtpBs::LtpStatBinFNm="LtpStat.Dat";

/////////////////////////////////////////////////
// Gas-Domain
int TGasDm::AddVar(const TStr& VarNm, const TStr& VarTy){
  IAssert((!VarNmTyKdV.IsIn(TStrKd(VarNm)))&&(VarValVV.Empty()));
  int VarN=VarNmTyKdV.Add(TStrKd(VarNm, VarTy));
  if (VarTy=="class"){IAssert(ClassVarN==-1); ClassVarN=VarN;}
  if (VarTy=="date"){IAssert(DateVarN==-1); DateVarN=VarN;}
  if (VarTy=="volume"){IAssert(VolVarN==-1); VolVarN=VarN;}
  if (VarTy=="attr"){Attrs++;}
  return VarN;
}

void TGasDm::GetAttrNmV(TStrV& AttrNmV) const {
  AttrNmV.Gen(Attrs, 0);
  for (int VarN=0; VarN<GetVars(); VarN++){
    if (GetVarTy(VarN)=="attr"){
      AttrNmV.Add(GetVarNm(VarN));
    }
  }
}

int TGasDm::GetAttrN(const TStr& AttrNm) const {
  TStrV AttrNmV; GetAttrNmV(AttrNmV);
  return AttrNmV.SearchForw(AttrNm);
}

TStr TGasDm::GetClassNm() const {
  for (int VarN=0; VarN<GetVars(); VarN++){
    if (GetVarTy(VarN)=="class"){
      return GetVarNm(VarN);
    }
  }
  Fail;
  return TStr();
}

void TGasDm::GetRecNV(
 const TSecTm& StartDate, const TSecTm& EndDate,
 const int& StartRecN, const int& EndRecN, const double& MnCspt, TIntV& RecNV){
  if ((StartDate.IsDef()&&EndDate.IsDef())){
    IAssert((StartRecN==-1)&&(EndRecN==-1));
    RecNV.Clr(); int Recs=GetRecs();
    for (int RecN=0; RecN<Recs; RecN++){
      TSecTm RecDate=GetDateVal(RecN);
      //printf("[%s]\n", RecDate.GetDtStr().CStr());
      if ((StartDate.IsDef()&&(RecDate<StartDate))||
       (EndDate.IsDef()&&(EndDate<RecDate))){continue;}
      if ((MnCspt!=-1)&&(GetClassVal(RecN)<MnCspt)){continue;}
      RecNV.Add(RecN);
    }
  } else
  if ((StartRecN!=-1)&&(EndRecN!=-1)){
    IAssert((!StartDate.IsDef())&&(!EndDate.IsDef()));
    RecNV.Clr();
    for (int RecN=StartRecN; RecN<=EndRecN; RecN++){
      RecNV.Add(RecN);
    }
  } else {
    Fail;
  }
}

void TGasDm::GetAttrValV(const int& RecN, TFltV& AttrValV) const {
  AttrValV.Gen(Attrs, 0);
  for (int VarN=0; VarN<GetVars(); VarN++){
    if (GetVarTy(VarN)=="attr"){
      AttrValV.Add(VarValVV[RecN][VarN]);
    }
  }
}

double TGasDm::GetAttrVal(
 const int& RecN, const int& AttrN, const double& DfVal) const {
  TFltV AttrValV; GetAttrValV(RecN, AttrValV);
  if (AttrN==-1){return DfVal;}
  else {return AttrValV[AttrN];}
}

double TGasDm::GetAttrVal(
 const int& RecN, const TStr& AttrNm, const double& DfVal) const {
  TFltV AttrValV; GetAttrValV(RecN, AttrValV);
  int AttrN=GetAttrN(AttrNm);
  if (AttrN==-1){return DfVal;}
  else {return AttrValV[AttrN];}
}

TStr TGasDm::GetBitsStr(const int& RecN) const {
  double IsHolidayVal=GetAttrVal(RecN, "IsHoliday", 0);
  double IsHolidayStartVal=GetAttrVal(RecN, "IsHolidayStart", 0);
  double IsHolidayEndVal=GetAttrVal(RecN, "IsHolidayEnd", 0);
  double IsDayBeforeHolidayVal=GetAttrVal(RecN, "IsDayBeforeHoliday", 0);
  double IsDayAfterHolidayVal=GetAttrVal(RecN, "IsDayAfterHoliday", 0);
  double PriceChangeVal=GetAttrVal(RecN, "PriceChange", 0);
  double DaysBeforePriceChangeVal=GetAttrVal(RecN, "DaysBeforePriceChange", 0);
  TChA ChA;
  if (IsHolidayVal!=0){ChA+=" IsHoliday";}
  if (IsHolidayStartVal!=0){ChA+=" IsHolidayStart";}
  if (IsHolidayEndVal!=0){ChA+=" IsHolidayEnd";}
  if (IsDayBeforeHolidayVal!=0){ChA+=" IsDayBeforeHoliday";}
  if (IsDayAfterHolidayVal!=0){ChA+=" IsDayAfterHoliday";}
  if (PriceChangeVal!=0){ChA+=" PriceChange:"; ChA+=TFlt::GetStr(PriceChangeVal);}
  if (DaysBeforePriceChangeVal!=0){
    ChA+=" DaysBeforePriceChange:"; ChA+=TFlt::GetStr(DaysBeforePriceChangeVal);
    if (RecN+1<GetRecs()){
      ChA+=TFlt::GetStr(GetAttrVal(RecN+1, "PriceChange", 0), "(%g%%)");}
  }
  return ChA;
}

PGasDm TGasDm::GetWeekGasDm(const PGasLtpBs& GasLtpBs){
  // collect day consumptions
  TIntIntH DayNToCsptH;
  for (int RecN=0; RecN<GasLtpBs->GetRecs(); RecN++){
    int DayN=GasLtpBs->GetDateInt(RecN);
    DayNToCsptH.AddDat(DayN)+=GasLtpBs->GetCspt(RecN);
  }
  // collect week consumptions
  TSecTm MnLtpDate=GasLtpBs->GetMnDate();
  TSecTm MxLtpDate=GasLtpBs->GetMxDate();
  typedef TKeyDat<TSecTm, TInt> TDateCsptKd;
  TVec<TDateCsptKd> DateCsptKdV;
  for (TSecTm LtpDate=MnLtpDate; LtpDate<MxLtpDate; LtpDate.AddDays(1)){
    int DayN=LtpDate.GetAbsSecs();
    if (!DayNToCsptH.IsKey(DayN)){continue;}
    int Cspt=DayNToCsptH.GetDat(DayN);
    if (LtpDate.GetDayOfWeekN()==TTmInfo::SunN){
      DateCsptKdV.Add(TDateCsptKd(LtpDate, 0));}
    if (DateCsptKdV.Len()>0){
      DateCsptKdV.Last().Dat+=Cspt;}
  }

  // create domain
  PGasDm GasDm=TGasDm::New("Week");
  GasDm->PutLtpNm(GasLtpBs->GetLtpNm());
  GasDm->PutLocNm(GasLtpBs->GetLocNm());
  GasDm->PutProdNm(GasLtpBs->GetProdNm());
  GasDm->AddVar("Date", "date");
  GasDm->AddVar("Cspt", "class");
  GasDm->AddVar("Const", "attr");
  GasDm->AddVar("Month", "attr");
  GasDm->AddVar("SumCsptLast1W", "attr");
  GasDm->AddVar("SumCsptLast2W", "attr");
  GasDm->AddVar("SumCsptLast3W", "attr");
  GasDm->AddVar("SumCsptLast1D", "attr");
  GasDm->AddVar("SumCsptLast2D", "attr");
  GasDm->AddVar("SumCsptLast3D", "attr");
  GasDm->AddVar("SumCsptLast4D", "attr");
  GasDm->AddVar("SumCsptLast5D", "attr");
  GasDm->AddVar("SumCsptLast6D", "attr");
  GasDm->AddVar("Volume", "volume");
  {for (int RecN=0; RecN<GasLtpBs->GetRecs(); RecN++){
    // get date & time of the record
    TSecTm Date=TSecTm(GasLtpBs->GetDateInt(RecN));
    // search & reset vector component for the data
    int DateCsptKdN=DateCsptKdV.SearchForw(TDateCsptKd(Date));
    if (DateCsptKdN==-1){continue;}
    // get & reset consumption for the date
    double Cspt=DateCsptKdV[DateCsptKdN].Dat;
    if (Cspt==-1){continue;}
    else {DateCsptKdV[DateCsptKdN].Dat=-1;}
    // collect values
    TFltV VarValV(GasDm->GetVars(), 0);
    VarValV.Add(GasLtpBs->GetDateInt(RecN));
    VarValV.Add(Cspt);
    VarValV.Add(1);
    VarValV.Add(GasLtpBs->GetMonthN(RecN));
    VarValV.Add(TFlt(GasLtpBs->GetSumCsptLast1W(RecN)));
    VarValV.Add(TFlt(GasLtpBs->GetSumCsptLast2W(RecN)));
    VarValV.Add(TFlt(GasLtpBs->GetSumCsptLast3W(RecN)));
    VarValV.Add(TFlt(GasLtpBs->GetSumCsptLast1D(RecN)));
    VarValV.Add(TFlt(GasLtpBs->GetSumCsptLast2D(RecN)));
    VarValV.Add(TFlt(GasLtpBs->GetSumCsptLast3D(RecN)));
    VarValV.Add(TFlt(GasLtpBs->GetSumCsptLast4D(RecN)));
    VarValV.Add(TFlt(GasLtpBs->GetSumCsptLast5D(RecN)));
    VarValV.Add(TFlt(GasLtpBs->GetSumCsptLast6D(RecN)));
    // check if all values are defined
    if (VarValV.IsIn(-1)){
      printf("Ignoring %s\n", GasLtpBs->GetTm(RecN).GetStr().CStr()); continue;}
    VarValV.Add(GasLtpBs->GetVol(RecN));
    // add value record
    GasDm->AddRec(VarValV);
  }}
  return GasDm;
}

PGasDm TGasDm::GetDayGasDm(const PGasLtpBs& GasLtpBs){
  // collect day consumptions
  TIntIntH DayNToCsptH;
  for (int RecN=0; RecN<GasLtpBs->GetRecs(); RecN++){
    int DayN=GasLtpBs->GetDateInt(RecN);
    DayNToCsptH.AddDat(DayN)+=GasLtpBs->GetCspt(RecN);
  }
  // create & define domain
  PGasDm GasDm=TGasDm::New("Day");
  GasDm->PutLtpNm(GasLtpBs->GetLtpNm());
  GasDm->PutLocNm(GasLtpBs->GetLocNm());
  GasDm->PutProdNm(GasLtpBs->GetProdNm());
  GasDm->AddVar("Date", "date");
  GasDm->AddVar("Cspt", "class");
  GasDm->AddVar("Const", "attr");
  GasDm->AddVar("Dow", "attr");
  GasDm->AddVar("Month", "attr");
  GasDm->AddVar("SumCsptLast1W", "attr");
  GasDm->AddVar("SumCsptLast2W", "attr");
  GasDm->AddVar("SumCsptLast3W", "attr");
  GasDm->AddVar("SumCsptLast1D", "attr");
  GasDm->AddVar("SumCsptLast2D", "attr");
  GasDm->AddVar("SumCsptLast3D", "attr");
  GasDm->AddVar("SumCsptLast4D", "attr");
  GasDm->AddVar("SumCsptLast5D", "attr");
  GasDm->AddVar("SumCsptLast6D", "attr");
  GasDm->AddVar("IsHoliday", "attr");
  GasDm->AddVar("IsHolidayStart", "attr");
  GasDm->AddVar("IsHolidayEnd", "attr");
  GasDm->AddVar("IsDayBeforeHoliday", "attr");
  GasDm->AddVar("IsDayAfterHoliday", "attr");
  GasDm->AddVar("PriceChange", "attr");
  GasDm->AddVar("DaysBeforePriceChange", "attr");
  GasDm->AddVar("Volume", "volume");
  // add data
  {for (int RecN=0; RecN<GasLtpBs->GetRecs(); RecN++){
    // get & reset day consumption
    //TStr DateStr=TSecTm(GasLtpBs->GetDateInt(RecN)).GetStr();
    double Cspt=DayNToCsptH.GetDat(GasLtpBs->GetDateInt(RecN));
    if (Cspt==-1){continue;}
    else {DayNToCsptH.GetDat(GasLtpBs->GetDateInt(RecN))=-1;}
    // collect data
    TFltV VarValV(GasDm->GetVars(), 0);
    VarValV.Add(GasLtpBs->GetDateInt(RecN));
    VarValV.Add(Cspt);
    VarValV.Add(1);
    VarValV.Add(GasLtpBs->GetDowN(RecN));
    VarValV.Add(GasLtpBs->GetMonthN(RecN));
    VarValV.Add(TFlt(GasLtpBs->GetSumCsptLast1W(RecN)));
    VarValV.Add(TFlt(GasLtpBs->GetSumCsptLast2W(RecN)));
    VarValV.Add(TFlt(GasLtpBs->GetSumCsptLast3W(RecN)));
    VarValV.Add(TFlt(GasLtpBs->GetSumCsptLast1D(RecN)));
    VarValV.Add(TFlt(GasLtpBs->GetSumCsptLast2D(RecN)));
    VarValV.Add(TFlt(GasLtpBs->GetSumCsptLast3D(RecN)));
    VarValV.Add(TFlt(GasLtpBs->GetSumCsptLast4D(RecN)));
    VarValV.Add(TFlt(GasLtpBs->GetSumCsptLast5D(RecN)));
    VarValV.Add(TFlt(GasLtpBs->GetSumCsptLast6D(RecN)));
    // check if all values are defined
    if (VarValV.IsIn(-1)){
      printf("Ignoring %s\n", GasLtpBs->GetTm(RecN).GetStr().CStr()); continue;}
    VarValV.Add(GasLtpBs->GetIsHoliday(RecN));
    VarValV.Add(GasLtpBs->GetIsHolidayStart(RecN));
    VarValV.Add(GasLtpBs->GetIsHolidayEnd(RecN));
    VarValV.Add(GasLtpBs->GetIsDayBeforeHoliday(RecN));
    VarValV.Add(GasLtpBs->GetIsDayAfterHoliday(RecN));
    VarValV.Add(GasLtpBs->GetPriceChange(RecN));
    VarValV.Add(GasLtpBs->GetDaysBeforePriceChange(RecN));
    VarValV.Add(GasLtpBs->GetVol(RecN));
    // add value record
    GasDm->AddRec(VarValV);
  }}
  return GasDm;
}

void TGasDm::AddCsptBlock(
 TIntIntH& HourNToCsptH, const int& BlockN, const int& Cspt){
  if ((!HourNToCsptH.IsKey(BlockN))||(HourNToCsptH.GetDat(BlockN)!=-1)){
    if (Cspt==-1){
      HourNToCsptH.AddDat(BlockN)=-1;
    } else {
      HourNToCsptH.AddDat(BlockN)=Cspt;
    }
  }
}

PGasDm TGasDm::GetHourBlockGasDm(const PGasLtpBs& GasLtpBs, const int& HourBlockLen){
  EAssertRA(24%HourBlockLen==0, "Hour resulution not divisible by 24", TInt::GetStr(HourBlockLen));
  // collect day consumptions
  TIntIntH BlockNToCsptH;
  TIntIntH BlockNToCsptLast1WH;
  TIntIntH BlockNToCsptLast2WH;
  TIntIntH BlockNToCsptLast3WH;
  //TIntIntH BlockNToSumCsptLast1WH;
  //TIntIntH BlockNToSumCsptLast2WH;
  //TIntIntH BlockNToSumCsptLast3WH;
  TIntIntH BlockNToCsptLast1DH;
  TIntIntH BlockNToCsptLast2DH;
  TIntIntH BlockNToCsptLast3DH;
  TIntIntH BlockNToCsptLast4DH;
  TIntIntH BlockNToCsptLast5DH;
  TIntIntH BlockNToCsptLast6DH;
  for (int RecN=0; RecN<GasLtpBs->GetRecs(); RecN++){
    int DayN=GasLtpBs->GetDateInt(RecN); IAssert(DayN%100==0);
    int HourN=GasLtpBs->GetHourN(RecN);
    IAssert((0<=HourN)&&(HourN<24));
    int BlockN=DayN+(HourN/HourBlockLen)*HourBlockLen;
    BlockNToCsptH.AddDat(BlockN)+=GasLtpBs->GetCspt(RecN);
    AddCsptBlock(BlockNToCsptLast1WH, BlockN, GasLtpBs->GetCsptLast1W(RecN));
    AddCsptBlock(BlockNToCsptLast2WH, BlockN, GasLtpBs->GetCsptLast2W(RecN));
    AddCsptBlock(BlockNToCsptLast3WH, BlockN, GasLtpBs->GetCsptLast3W(RecN));
    //AddCsptBlock(BlockNToSumCsptLast1WH, BlockN, GasLtpBs->GetSumCsptLast1W(RecN));
    //AddCsptBlock(BlockNToSumCsptLast2WH, BlockN, GasLtpBs->GetSumCsptLast2W(RecN));
    //AddCsptBlock(BlockNToSumCsptLast3WH, BlockN, GasLtpBs->GetSumCsptLast3W(RecN));
    AddCsptBlock(BlockNToCsptLast1DH, BlockN, GasLtpBs->GetCsptLast1D(RecN));
    AddCsptBlock(BlockNToCsptLast2DH, BlockN, GasLtpBs->GetCsptLast2D(RecN));
    AddCsptBlock(BlockNToCsptLast3DH, BlockN, GasLtpBs->GetCsptLast3D(RecN));
    AddCsptBlock(BlockNToCsptLast4DH, BlockN, GasLtpBs->GetCsptLast4D(RecN));
    AddCsptBlock(BlockNToCsptLast5DH, BlockN, GasLtpBs->GetCsptLast5D(RecN));
    AddCsptBlock(BlockNToCsptLast6DH, BlockN, GasLtpBs->GetCsptLast6D(RecN));
  }
  // create & define domain
  PGasDm GasDm=TGasDm::New(TStr("Hour")+TInt::GetStr(HourBlockLen));
  GasDm->PutLtpNm(GasLtpBs->GetLtpNm());
  GasDm->PutLocNm(GasLtpBs->GetLocNm());
  GasDm->PutProdNm(GasLtpBs->GetProdNm());
  GasDm->AddVar("Date", "date"); // define date variable (not used for learning)
  GasDm->AddVar("Cspt", "class"); // define class variable
  GasDm->AddVar("Const", "attr"); // define constant attribute (for regression)
  GasDm->AddVar("Hour", "attr"); // define hour attribute
  GasDm->AddVar("Dow", "attr"); // define day-of-week attribute
  GasDm->AddVar("Month", "attr"); // define month attribute
  GasDm->AddVar("CsptLast1W", "attr");
  GasDm->AddVar("CsptLast2W", "attr");
  GasDm->AddVar("CsptLast3W", "attr");
  //GasDm->AddVar("SumCsptLast1W", "attr");
  //GasDm->AddVar("SumCsptLast2W", "attr");
  //GasDm->AddVar("SumCsptLast3W", "attr");
  GasDm->AddVar("CsptLast1D", "attr");
  GasDm->AddVar("CsptLast2D", "attr");
  GasDm->AddVar("CsptLast3D", "attr");
  GasDm->AddVar("CsptLast4D", "attr");
  GasDm->AddVar("CsptLast5D", "attr");
  GasDm->AddVar("CsptLast6D", "attr");
  //GasDm->AddVar("SumCsptLast1D", "attr");
  //GasDm->AddVar("SumCsptLast2D", "attr");
  //GasDm->AddVar("SumCsptLast3D", "attr");
  //GasDm->AddVar("SumCsptLast4D", "attr");
  //GasDm->AddVar("SumCsptLast5D", "attr");
  //GasDm->AddVar("SumCsptLast6D", "attr");
  GasDm->AddVar("IsHoliday", "attr");
  GasDm->AddVar("Volume", "volume");
  // add data
  {for (int RecN=0; RecN<GasLtpBs->GetRecs(); RecN++){
    // get hour-block
    int DayN=GasLtpBs->GetDateInt(RecN); IAssert(DayN%100==0);
    int HourN=GasLtpBs->GetHourN(RecN);
    IAssert((0<=HourN)&&(HourN<24));
    int BlockN=DayN+(HourN/HourBlockLen)*HourBlockLen;
    int BlockHourN=BlockN%100;
    // get & reset hour-block consumption
    double Cspt=BlockNToCsptH.GetDat(BlockN);
    if (Cspt==-1){continue;}
    else {BlockNToCsptH.GetDat(BlockN)=-1;}
    //printf("[Hour:%d]", BlockHourN);
    // create vector of variable values
    TFltV VarValV(GasDm->GetVars(), 0);
    // add values
    VarValV.Add(GasLtpBs->GetDateInt(RecN)); // add date value
    VarValV.Add(Cspt); // add consumption
    VarValV.Add(1); // add constant
    VarValV.Add(BlockHourN); // add hour
    VarValV.Add(GasLtpBs->GetDowN(RecN)); // add day-of-week
    VarValV.Add(GasLtpBs->GetMonthN(RecN)); // add month
    VarValV.Add(int(BlockNToCsptLast1WH.GetDat(BlockN))); //printf(" [CsptLast1W:%d]", BlockNToCsptLast1WH.GetDat(BlockN));
    VarValV.Add(int(BlockNToCsptLast2WH.GetDat(BlockN))); //printf(" [CsptLast2W:%d]", BlockNToCsptLast2WH.GetDat(BlockN));
    VarValV.Add(int(BlockNToCsptLast3WH.GetDat(BlockN))); //printf(" [CsptLast3W:%d]", BlockNToCsptLast3WH.GetDat(BlockN));
    //VarValV.Add(int(BlockNToSumCsptLast1WH.GetDat(BlockN))); //printf(" [SumCsptLast1W:%d]", BlockNToSumCsptLast1WH.GetDat(BlockN));
    //VarValV.Add(int(BlockNToSumCsptLast2WH.GetDat(BlockN))); //printf(" [SumCsptLast2W:%d]", BlockNToSumCsptLast2WH.GetDat(BlockN));
    //VarValV.Add(int(BlockNToSumCsptLast3WH.GetDat(BlockN))); //printf(" [SumCsptLast3W:%d]", BlockNToSumCsptLast3WH.GetDat(BlockN));
    VarValV.Add(int(BlockNToCsptLast1DH.GetDat(BlockN))); //printf(" [CsptLast1D:%d]", BlockNToCsptLast1DH.GetDat(BlockN));
    VarValV.Add(int(BlockNToCsptLast2DH.GetDat(BlockN))); //printf(" [CsptLast2D:%d]", BlockNToCsptLast2DH.GetDat(BlockN));
    VarValV.Add(int(BlockNToCsptLast3DH.GetDat(BlockN))); //printf(" [CsptLast3D:%d]", BlockNToCsptLast3DH.GetDat(BlockN));
    VarValV.Add(int(BlockNToCsptLast4DH.GetDat(BlockN))); //printf(" [CsptLast4D:%d]", BlockNToCsptLast4DH.GetDat(BlockN));
    VarValV.Add(int(BlockNToCsptLast5DH.GetDat(BlockN))); //printf(" [CsptLast5D:%d]", BlockNToCsptLast5DH.GetDat(BlockN));
    VarValV.Add(int(BlockNToCsptLast6DH.GetDat(BlockN))); //printf(" [CsptLast6D:%d]", BlockNToCsptLast6DH.GetDat(BlockN));
    //VarValV.Add(GasLtpBs->GetSumCsptLast1D(RecN));
    //VarValV.Add(GasLtpBs->GetSumCsptLast2D(RecN));
    //VarValV.Add(GasLtpBs->GetSumCsptLast3D(RecN));
    //VarValV.Add(GasLtpBs->GetSumCsptLast4D(RecN));
    //VarValV.Add(GasLtpBs->GetSumCsptLast5D(RecN));
    //VarValV.Add(GasLtpBs->GetSumCsptLast6D(RecN));
    VarValV.Add(GasLtpBs->GetIsHoliday(RecN));
    // check if all values are defined
    if (VarValV.IsIn(-1)){
      printf("Ignoring %s\n", GasLtpBs->GetTm(RecN).GetStr().CStr()); continue;}
    VarValV.Add(GasLtpBs->GetVol(RecN));
    // add value record
    GasDm->AddRec(VarValV);
  }}
  return GasDm;
}

PGasDm TGasDm::GetHourGasDm(const PGasLtpBs& GasLtpBs){
  // create domain
  PGasDm GasDm=TGasDm::New();
  // define domain
  GasDm->PutLtpNm(GasLtpBs->GetLtpNm());
  GasDm->PutLocNm(GasLtpBs->GetLocNm());
  GasDm->PutProdNm(GasLtpBs->GetProdNm());
  GasDm->AddVar("Date", "date"); // define date variable (not used for learning)
  GasDm->AddVar("Cspt", "class"); // define class variable
  GasDm->AddVar("Const", "attr"); // define constant attribute (for regression)
  GasDm->AddVar("Hour", "attr"); // define hour attribute
  GasDm->AddVar("Dow", "attr"); // define day-of-week attribute
  GasDm->AddVar("Month", "attr"); // define month attribute
  GasDm->AddVar("CsptLast1W", "attr");
  GasDm->AddVar("CsptLast2W", "attr");
  GasDm->AddVar("CsptLast3W", "attr");
  //GasDm->AddVar("SumCsptLast1W", "attr");
  //GasDm->AddVar("SumCsptLast2W", "attr");
  //GasDm->AddVar("SumCsptLast3W", "attr");
  GasDm->AddVar("CsptLast1D", "attr");
  GasDm->AddVar("CsptLast2D", "attr");
  GasDm->AddVar("CsptLast3D", "attr");
  //GasDm->AddVar("CsptLast4D", "attr");
  //GasDm->AddVar("CsptLast5D", "attr");
  //GasDm->AddVar("CsptLast6D", "attr");
  //GasDm->AddVar("SumCsptLast1D", "attr");
  //GasDm->AddVar("SumCsptLast2D", "attr");
  //GasDm->AddVar("SumCsptLast3D", "attr");
  //GasDm->AddVar("SumCsptLast4D", "attr");
  //GasDm->AddVar("SumCsptLast5D", "attr");
  //GasDm->AddVar("SumCsptLast6D", "attr");
  GasDm->AddVar("IsHoliday", "attr");
  GasDm->AddVar("Volume", "volume");
  // add data
  for (int RecN=0; RecN<GasLtpBs->GetRecs(); RecN++){
    // create vector of variable values
    TFltV VarValV(GasDm->GetVars(), 0);
    // add values
    VarValV.Add(GasLtpBs->GetDateInt(RecN)); // add date value
    VarValV.Add(GasLtpBs->GetCspt(RecN)); // add consumption
    VarValV.Add(1); // add constant
    VarValV.Add(GasLtpBs->GetHourN(RecN)); // add hour
    VarValV.Add(GasLtpBs->GetDowN(RecN)); // add day-of-week
    VarValV.Add(GasLtpBs->GetMonthN(RecN)); // add month
    VarValV.Add(TFlt(GasLtpBs->GetCsptLast1W(RecN)));
    VarValV.Add(TFlt(GasLtpBs->GetCsptLast2W(RecN)));
    VarValV.Add(TFlt(GasLtpBs->GetCsptLast3W(RecN)));
    //VarValV.Add(GasLtpBs->GetSumCsptLast1W(RecN));
    //VarValV.Add(GasLtpBs->GetSumCsptLast2W(RecN));
    //VarValV.Add(GasLtpBs->GetSumCsptLast3W(RecN));
    VarValV.Add(TFlt(GasLtpBs->GetCsptLast1D(RecN)));
    VarValV.Add(TFlt(GasLtpBs->GetCsptLast2D(RecN)));
    VarValV.Add(TFlt(GasLtpBs->GetCsptLast3D(RecN)));
    //VarValV.Add(TFlt(GasLtpBs->GetCsptLast4D(RecN)));
    //VarValV.Add(TFlt(GasLtpBs->GetCsptLast5D(RecN)));
    //VarValV.Add(TFlt(GasLtpBs->GetCsptLast6D(RecN)));
    //VarValV.Add(GasLtpBs->GetSumCsptLast1D(RecN));
    //VarValV.Add(GasLtpBs->GetSumCsptLast2D(RecN));
    //VarValV.Add(GasLtpBs->GetSumCsptLast3D(RecN));
    //VarValV.Add(GasLtpBs->GetSumCsptLast4D(RecN));
    //VarValV.Add(GasLtpBs->GetSumCsptLast5D(RecN));
    //VarValV.Add(GasLtpBs->GetSumCsptLast6D(RecN));
    VarValV.Add(GasLtpBs->GetIsHoliday(RecN));
    // check if all values are defined
    if (VarValV.IsIn(-1)){
      printf("Ignoring %s\n", GasLtpBs->GetTm(RecN).GetStr().CStr()); continue;}
    VarValV.Add(GasLtpBs->GetVol(RecN));
    // add value record
    GasDm->AddRec(VarValV);
  }
  return GasDm;
}

int TGasDm::AddWeekRec(const double& Cspt){
  IAssert(TypeNm=="Week");
  // get dates
  TSecTm PrevDt=GetDateVal(GetRecs()-1);
  TSecTm TodayDt=TSecTm(PrevDt).AddDays(+7);
  // create new record
  int TodayRecN=AddRec();
  if (TodayRecN<=21){
    TExcept::Throw("Not enough past data - cannot calculate future records.");}
  // add new record values
  PutVarVal(TodayRecN, "Date", TodayDt.GetAbsSecs());
  PutVarVal(TodayRecN, "Cspt", Cspt);
  PutVarVal(TodayRecN, "Const", 1);
  PutVarVal(TodayRecN, "Month", TodayDt.GetMonthN());
  PutVarVal(TodayRecN, "SumCsptLast1W", GetVarVal(TodayRecN-7, "Cspt"));
  PutVarVal(TodayRecN, "SumCsptLast2W", GetVarVal(TodayRecN-14, "Cspt"));
  PutVarVal(TodayRecN, "SumCsptLast3W", GetVarVal(TodayRecN-21, "Cspt"));
  PutVarVal(TodayRecN, "SumCsptLast1D", GetVarVal(TodayRecN-1, "Cspt"));
  PutVarVal(TodayRecN, "SumCsptLast2D", GetVarVal(TodayRecN-2, "Cspt"));
  PutVarVal(TodayRecN, "SumCsptLast3D", GetVarVal(TodayRecN-3, "Cspt"));
  PutVarVal(TodayRecN, "SumCsptLast4D", GetVarVal(TodayRecN-4, "Cspt"));
  PutVarVal(TodayRecN, "SumCsptLast5D", GetVarVal(TodayRecN-5, "Cspt"));
  PutVarVal(TodayRecN, "SumCsptLast6D", GetVarVal(TodayRecN-6, "Cspt"));
  PutVarVal(TodayRecN, "Volume", -1);
  // return record number
  return TodayRecN;
}

int TGasDm::AddDayRec(const double& Cspt){
  IAssert(TypeNm=="Day");
  // get dates
  TSecTm PrevDt=GetDateVal(GetRecs()-1);
  TSecTm TodayDt=TSecTm(PrevDt).AddDays(+1);
  // create new record
  int TodayRecN=AddRec();
  if (TodayRecN<=21){
    TExcept::Throw("Not enough past data - cannot calculate future records.");}
  // add new record values
  PutVarVal(TodayRecN, "Date", TodayDt.GetAbsSecs());
  PutVarVal(TodayRecN, "Cspt", Cspt);
  PutVarVal(TodayRecN, "Const", 1);
  PutVarVal(TodayRecN, "Dow", TodayDt.GetDayOfWeekN());
  PutVarVal(TodayRecN, "Month", TodayDt.GetMonthN());
  PutVarVal(TodayRecN, "SumCsptLast1W", GetVarVal(TodayRecN-7, "Cspt"));
  PutVarVal(TodayRecN, "SumCsptLast2W", GetVarVal(TodayRecN-14, "Cspt"));
  PutVarVal(TodayRecN, "SumCsptLast3W", GetVarVal(TodayRecN-21, "Cspt"));
  PutVarVal(TodayRecN, "SumCsptLast1D", GetVarVal(TodayRecN-1, "Cspt"));
  PutVarVal(TodayRecN, "SumCsptLast2D", GetVarVal(TodayRecN-2, "Cspt"));
  PutVarVal(TodayRecN, "SumCsptLast3D", GetVarVal(TodayRecN-3, "Cspt"));
  PutVarVal(TodayRecN, "SumCsptLast4D", GetVarVal(TodayRecN-4, "Cspt"));
  PutVarVal(TodayRecN, "SumCsptLast5D", GetVarVal(TodayRecN-5, "Cspt"));
  PutVarVal(TodayRecN, "SumCsptLast6D", GetVarVal(TodayRecN-6, "Cspt"));
  PutVarVal(TodayRecN, "IsHoliday", 0);
  PutVarVal(TodayRecN, "IsHolidayStart", 0);
  PutVarVal(TodayRecN, "IsHolidayEnd", 0);
  PutVarVal(TodayRecN, "IsDayBeforeHoliday", 0);
  PutVarVal(TodayRecN, "IsDayAfterHoliday", 0);
  PutVarVal(TodayRecN, "PriceChange", 0);
  PutVarVal(TodayRecN, "DaysBeforePriceChange", 0);
  PutVarVal(TodayRecN, "Volume", -1);
  // return record number
  return TodayRecN;
}

int TGasDm::AddHourBlockRec(const double& Cspt){
  IAssert(TypeNm.IsPrefix("Hour"));
  TStr HoursResStr=TypeNm.GetSubStr(TStr("Hour").Len(), 999);
  int HoursRes=HoursResStr.GetInt(1);
  int RecsPerDay=24/HoursRes;
  int RecsPerWeek=(7*24)/HoursRes;
  // get dates
  TSecTm PrevTm=GetDateVal(GetRecs()-1);
  TSecTm NowTm=TSecTm(PrevTm).AddHours(HoursRes);
  // create new record
  int NowRecN=AddRec();
  if (NowRecN<=3*RecsPerWeek){
    TExcept::Throw("Not enough past data - cannot calculate future records.");}
  // add new record values
  PutVarVal(NowRecN, "Date", NowTm.GetAbsSecs());
  PutVarVal(NowRecN, "Cspt", Cspt);
  PutVarVal(NowRecN, "Const", 1);
  PutVarVal(NowRecN, "Hour", NowTm.GetHourN());
  PutVarVal(NowRecN, "Dow", NowTm.GetDayOfWeekN());
  PutVarVal(NowRecN, "Month", NowTm.GetMonthN());
  PutVarVal(NowRecN, "CsptLast1W", GetVarVal(NowRecN-1*RecsPerWeek, "Cspt"));
  PutVarVal(NowRecN, "CsptLast2W", GetVarVal(NowRecN-2*RecsPerWeek, "Cspt"));
  PutVarVal(NowRecN, "CsptLast3W", GetVarVal(NowRecN-3*RecsPerWeek, "Cspt"));
  PutVarVal(NowRecN, "CsptLast1D", GetVarVal(NowRecN-1*RecsPerDay, "Cspt"));
  PutVarVal(NowRecN, "CsptLast2D", GetVarVal(NowRecN-2*RecsPerDay, "Cspt"));
  PutVarVal(NowRecN, "CsptLast3D", GetVarVal(NowRecN-3*RecsPerDay, "Cspt"));
  PutVarVal(NowRecN, "CsptLast4D", GetVarVal(NowRecN-4*RecsPerDay, "Cspt"));
  PutVarVal(NowRecN, "CsptLast5D", GetVarVal(NowRecN-5*RecsPerDay, "Cspt"));
  PutVarVal(NowRecN, "CsptLast6D", GetVarVal(NowRecN-6*RecsPerDay, "Cspt"));
  PutVarVal(NowRecN, "IsHoliday", 0);
  PutVarVal(NowRecN, "Volume", -1);
  // return record number
  return NowRecN;
}

int TGasDm::AddTmResRec(const double& Cspt){
  if (TypeNm=="Week"){return AddWeekRec(Cspt);}
  if (TypeNm=="Day"){return AddDayRec(Cspt);}
  if (TypeNm.IsPrefix("Hour")){return AddHourBlockRec(Cspt);}
  else {Fail; return -1;}
}

void TGasDm::DumpRec(const int& RecN) const {
  printf("Record[%d]:", RecN);
  for (int VarN=0; VarN<GetVars(); VarN++){
    if (GetVarNm(VarN)=="Date"){
      printf(" [%s:%s]", GetVarNm(VarN).CStr(),
       TSecTm(int(GetVarVal(RecN, VarN))).GetStr().CStr());
    } else {
      printf(" [%s:%g]", GetVarNm(VarN).CStr(), GetVarVal(RecN, VarN));
    }
  }
  printf("\n\n");
}

/////////////////////////////////////////////////
// Gas-Domain-Header
void TGasDmHd::GetAttrNmV(TStrV& AttrNmV) const {
  AttrNmV.Clr();
  for (int VarN=0; VarN<GetVars(); VarN++){
    if (GetVarTy(VarN)=="attr"){
      AttrNmV.Add(GetVarNm(VarN));
    }
  }
}

TStr TGasDmHd::GetClassNm() const {
  for (int VarN=0; VarN<GetVars(); VarN++){
    if (GetVarTy(VarN)=="class"){
      return GetVarNm(VarN);
    }
  }
  Fail;
  return TStr();
}

/////////////////////////////////////////////////
// Gas-Model-Multi-Type
TStr TGasMdMType::GetDescStr() const {
  if (GetMdTypes()==1){
    TGasMdType GasMdType=GetMdType(0);
    return TGasMd::GetGasMdTypeBriefNm(GasMdType);
  } else {
    TChA ChA;
    ChA+="Ensemble(";
    for (int MdTypeN=0; MdTypeN<GetMdTypes(); MdTypeN++){
      if (MdTypeN>0){ChA+=' ';}
      TGasMdType GasMdType=GetMdType(MdTypeN);
      double Wgt=GetMdWgt(MdTypeN);
      ChA+=TGasMd::GetGasMdTypeBriefNm(GasMdType);
      ChA+=":";
      ChA+=TFlt::GetStr(Wgt, "%.2f");
    }
    ChA+=")";
    return ChA;
  }
}

void TGasMdMType::AddMdTypeWgt(const TGasMdType& MdType, const double& MdWgt){
  if (MdWgt<=0){return;}
  // add model-type/weight pair
  MdTypeWgtPrV.Add(TIntFltPr(int(MdType), MdWgt));
}

void TGasMdMType::NrmMdWgt(){
  // calculate sum of weights
  double SumMdWgt=0;
  for (int MdTypeN=0; MdTypeN<GetMdTypes(); MdTypeN++){
    SumMdWgt+=GetMdWgt(MdTypeN);}
  // renormalize
  {for (int MdTypeN=0; MdTypeN<GetMdTypes(); MdTypeN++){
    MdTypeWgtPrV[MdTypeN].Val2=MdTypeWgtPrV[MdTypeN].Val2/SumMdWgt;}}
}

/////////////////////////////////////////////////
// Gas-Model
double TGasMd::GetCorPredClassVal(
 const PGasDm& GasDm, const int& RecN, const double& CorFact) const {
  // get base predicted class value
  double ClassVal=GetPredClassVal(GasDm, RecN);
  // correct predicted class value
  if (GasDm->GetAttrVal(RecN, "DaysBeforePriceChange", 0)==1){
    ClassVal*=CorFact;}
  // don't return negative values
  if (ClassVal<0){
    ClassVal=0;}
  // return corrected class value
  return ClassVal;
}

PGasMd TGasMd::New(
 const TGasMdType& GasMdType,
 const PGasMdMType& GasMdMType, const TStr& GasMdParamStr,
 const PGasDm& GasDm,
 const TSecTm& StartDate, const TSecTm& EndDate,
 const int& StartRecN, const int& EndRecN, const double& MnCspt){
  switch (GasMdType){
    case gmtCTb: return TGasMdCTb::New(GasMdParamStr, GasDm, StartDate, EndDate, StartRecN, EndRecN, MnCspt);
    case gmtLr: return TGasMdSvd::New(GasMdParamStr, GasDm, StartDate, EndDate, StartRecN, EndRecN, MnCspt);
    case gmtNNbr: return TGasMdNNbr::New(GasMdParamStr, GasDm, StartDate, EndDate, StartRecN, EndRecN, MnCspt);
    case gmtSvm: return TGasMdSvm::New(GasMdParamStr, GasDm, StartDate, EndDate, StartRecN, EndRecN, MnCspt);
    case gmtCubist: return TGasMdCubist::New(GasMdParamStr, GasDm, StartDate, EndDate, StartRecN, EndRecN, MnCspt);
    case gmtRegTree: return TGasMdRegTree::New(GasMdParamStr, GasDm, StartDate, EndDate, StartRecN, EndRecN, MnCspt);
    case gmtEnsemble: return TGasMdEnsemble::New(GasMdMType, GasMdParamStr, GasDm, StartDate, EndDate, StartRecN, EndRecN, MnCspt);
    default: Fail; return NULL;
  }
}

TStr TGasMd::GetGasMdTypeNm(const TGasMdType& GasMdType){
  switch (GasMdType){
    case gmtCTb: return "Contingency-Table";
    case gmtLr: return "Singular-Value-Decomposition";
    case gmtNNbr: return "Nearest-Neigbour";
    case gmtSvm: return "Support-Vector-Machine";
    case gmtCubist: return "Cubist";
    case gmtRegTree: return "Regression-Tree";
    case gmtEnsemble: return "Ensemble";
    default: Fail; return TStr();
  }
}

TStr TGasMd::GetGasMdTypeBriefNm(const TGasMdType& GasMdType){
  switch (GasMdType){
    case gmtCTb: return "ContTb";
    case gmtLr: return "SVD";
    case gmtNNbr: return "NNbr";
    case gmtSvm: return "SVM";
    case gmtCubist: return "Cubist";
    case gmtRegTree: return "RTree";
    case gmtEnsemble: return "Ensemble";
    default: Fail; return TStr();
  }
}

TStr TGasMd::GetMdParamStr(){
  return
   "<Model>"
   "  <ContTable><Moment>Median</Moment></ContTable>"
   "  <NNbr><K>5</K></NNbr>"
   "  <Cubist><FileId>CubistData</FileId></Cubist>"
   "  <Svm><FileId>SvmData</FileId></Svm>"
   "  <RegTree><MnLeafExs>5</MnLeafExs></RegTree>"
   "</Model>";
}

/////////////////////////////////////////////////
// Gas-Model-Ensemble
PGasMd TGasMdEnsemble::New(
 const PGasMdMType& GasMdMType, const TStr& ParamStr, const PGasDm& GasDm,
 const TSecTm& StartDate, const TSecTm& EndDate,
 const int& StartRecN, const int& EndRecN, const double& MnCspt){
  // create domain header
  PGasDmHd GasDmHd=TGasDmHd::New(GasDm);
  // create model
  TGasMdEnsemble* GasMdEnsemble=new TGasMdEnsemble(GasDmHd, GasMdMType);
  PGasMd GasMd(GasMdEnsemble);
  // create models
  for (int MdTypeN=0; MdTypeN<GasMdMType->GetMdTypes(); MdTypeN++){
    TGasMdType GasMdType=GasMdMType->GetMdType(MdTypeN);
    PGasMd SubGasMd=TGasMd::New(GasMdType, NULL, ParamStr, GasDm, StartDate, EndDate, StartRecN, EndRecN, MnCspt);
    GasMdEnsemble->GasMdV.Add(SubGasMd);
  }
  // return model
  return GasMd;
}

double TGasMdEnsemble::GetPredClassVal(const PGasDm& GasDm, const int& RecN) const {
  // check model & domain compatibility
  IAssert(GetGasDmHd()->IsCompatible(GasDm));
  // get class value
  double PredClassVal=0;
  for (int MdTypeN=0; MdTypeN<GasMdMType->GetMdTypes(); MdTypeN++){
    double GasMdWgt=GasMdMType->GetMdWgt(MdTypeN);
    PGasMd SubGasMd=GasMdV[MdTypeN];
    PredClassVal+=GasMdWgt*SubGasMd->GetPredClassVal(GasDm, RecN);
  }
  // return predicted class value
  return PredClassVal;
}

PMom TGasMdEnsemble::GetClassValMom() const {
  return GasMdV[0]->GetClassValMom();
}

void TGasMdEnsemble::Print() const {
  if (GasMdMType->GetMdTypes()==1){
    GasMdV[0]->Print();
  } else {
    printf("==========================================================\n");
    printf("ClassVal: %s\n", GetClassValMom()->GetStr(' ', ':', true).CStr());
    printf("ClassVal =\n");
    for (int MdTypeN=0; MdTypeN<GasMdMType->GetMdTypes(); MdTypeN++){
      TGasMdType GasMdType=GasMdMType->GetMdType(MdTypeN);
      double GasMdWgt=GasMdMType->GetMdWgt(MdTypeN);
      printf("    + %g * %s\n", GasMdWgt, TGasMd::GetGasMdTypeNm(GasMdType).CStr());
    }
    printf("\n");
  }
}

/////////////////////////////////////////////////
// Gas-Model-Contingency-Tables
PGasMd TGasMdCTb::New(const TStr& ParamStr, const PGasDm& GasDm,
 const TSecTm& StartDate, const TSecTm& EndDate,
 const int& StartRecN, const int& EndRecN, const double& MnCspt){
  // create domain header
  PGasDmHd GasDmHd=TGasDmHd::New(GasDm);
  // create model
  TGasMdCTb* GasMdCTb=new TGasMdCTb(GasDmHd); PGasMd GasMd(GasMdCTb);
  // get parameters
  PSIn ParamXmlSIn=TStrIn::New(ParamStr);
  PXmlDoc ParamXmlDoc=TXmlDoc::LoadTxt(ParamXmlSIn);
  GasMdCTb->MomNm=ParamXmlDoc->GetTagTokStr("Model|ContTable|Moment");
  // select records for model building
  TIntV RecNV; GasDm->GetRecNV(StartDate, EndDate, StartRecN, EndRecN, MnCspt, RecNV);
  int Recs=RecNV.Len();
  // get attribute numbers
  GasMdCTb->HourAttrN=GasDm->GetAttrN("Hour");
  GasMdCTb->DowAttrN=GasDm->GetAttrN("Dow");
  GasMdCTb->MonthAttrN=GasDm->GetAttrN("Month");
  // create contingency-tables
  GasMdCTb->ClassValMom=TMom::New();
  TMom::NewV(GasMdCTb->HourMomV, 24);
  TMom::NewV(GasMdCTb->DowMomV, 7);
  TMom::NewV(GasMdCTb->MonthMomV, 12);
  TMom::NewVV(GasMdCTb->DowHourMomVV, 7, 24);
  // fill contingency-tables
  for (int RecNN=0; RecNN<Recs; RecNN++){
    int RecN=RecNV[RecNN];
    // get class value
    double ClassVal=GasDm->GetClassVal(RecN);
    // get hour & day-of-week & month values
    int HourN, DowN, MonthN;
    if (GasMdCTb->HourAttrN!=-1){HourN=GasDm->GetAttrVal(RecN, GasMdCTb->HourAttrN, 0);}
    if (GasMdCTb->DowAttrN!=-1){DowN=GasDm->GetAttrVal(RecN, GasMdCTb->DowAttrN, 0)-1;}
    if (GasMdCTb->MonthAttrN!=-1){MonthN=GasDm->GetAttrVal(RecN, GasMdCTb->MonthAttrN, 0)-1;}
    // save class to hour/dow/month statistics
    GasMdCTb->ClassValMom->Add(ClassVal);
    if (GasMdCTb->HourAttrN!=-1){
      GasMdCTb->HourMomV[HourN]->Add(ClassVal);}
    if (GasMdCTb->DowAttrN!=-1){
      GasMdCTb->DowMomV[DowN]->Add(ClassVal);}
    if (GasMdCTb->MonthAttrN!=-1){
      GasMdCTb->MonthMomV[MonthN]->Add(ClassVal);}
    if ((GasMdCTb->DowAttrN!=-1)&&(GasMdCTb->HourAttrN!=-1)){
      GasMdCTb->DowHourMomVV.At(DowN, HourN)->Add(ClassVal);}
  }
  // finish contingency-tables
  GasMdCTb->ClassValMom->Def();
  TMom::DefV(GasMdCTb->HourMomV);
  TMom::DefV(GasMdCTb->DowMomV);
  TMom::DefV(GasMdCTb->MonthMomV);
  TMom::DefVV(GasMdCTb->DowHourMomVV);
  // return model
  return GasMd;
}

double TGasMdCTb::GetPredClassVal(const PGasDm& GasDm, const int& RecN) const {
  // check model & domain compatibility
  IAssert(GetGasDmHd()->IsCompatible(GasDm));
  // get hour & day-of-week & month values
  int HourN, DowN, MonthN;
  if (HourAttrN!=-1){HourN=GasDm->GetAttrVal(RecN, HourAttrN, 0);}
  if (DowAttrN!=-1){DowN=GasDm->GetAttrVal(RecN, DowAttrN, 0)-1;}
  if (MonthAttrN!=-1){MonthN=GasDm->GetAttrVal(RecN, MonthAttrN, 0)-1;}
  // select statistical moment
  PMom Mom1; PMom Mom2;
  if ((HourAttrN!=-1)&&(DowAttrN!=-1)){
    Mom1=DowHourMomVV.At(DowN, HourN);
  } else
  if ((DowAttrN!=-1)&&(MonthAttrN!=-1)){
    Mom1=DowMomV[DowN];
    //Mom2=MonthMomV[MonthN];
  } else
  if (HourAttrN!=-1){
    Mom1=HourMomV[HourN];
  } else
  if (DowAttrN!=-1){
    Mom1=DowMomV[DowN];
  } else
  if (MonthAttrN!=-1){
    Mom1=MonthMomV[MonthN];
  } else {
    Mom1=ClassValMom;
  }
  // get class value from stat. moment
  double PredClassVal=0;
  if (Mom1->IsUsable()){
    PredClassVal=Mom1->GetByNm(MomNm);
  }
  if ((!Mom2.Empty())&&(Mom2->IsUsable())){
    PredClassVal=0.5*PredClassVal+0.5*Mom2->GetByNm(MomNm);
  }
  // return predicted class value
  return PredClassVal;
}

void TGasMdCTb::Print() const {
  printf("==========================================================\n");
  printf("ClassVal: %s\n", ClassValMom->GetStr(' ', ':', true).CStr());
  printf("per Hour:");
  for (int HourN=0; HourN<24; HourN++){
    printf(" [%d:%s]", HourN, HourMomV[HourN]->GetStrByNm(MomNm).CStr());}
  printf("\n");
  printf("per Day-Of-Week:");
  for (int DowN=0; DowN<7; DowN++){
    printf(" [%s:%s]", TTmInfo::GetDayOfWeekNm(DowN+1).CStr(),
     DowMomV[DowN]->GetStrByNm(MomNm).CStr());
  }
  printf("\n");
  printf("per Month:");
  for (int MonthN=0; MonthN<12; MonthN++){
    printf(" [%s:%s]", TTmInfo::GetMonthNm(MonthN+1).CStr(),
     MonthMomV[MonthN]->GetStrByNm(MomNm).CStr());
  }
  printf("\n");
  /*printf("per Day-Of-Week&Hour:");
  {for (int DowN=0; DowN<7; DowN++){
    for (int HourN=0; HourN<24; HourN++){
      printf(" [%s/%d:%s]", TTmInfo::GetDayOfWeekNm(DowN+1).CStr(), DowN,
       DowHourMomVV.At(DowN, HourN)->GetStrByNm(MomNm).CStr());
    }
  }}
  printf("\n");*/
}

/////////////////////////////////////////////////
// Gas-Model-Singular-Value-Decomposition
PGasMd TGasMdSvd::New(const TStr& ParamStr, const PGasDm& GasDm,
 const TSecTm& StartDate, const TSecTm& EndDate,
 const int& StartRecN, const int& EndRecN, const double& MnCspt){
  // create domain header
  PGasDmHd GasDmHd=TGasDmHd::New(GasDm);
  // create model
  TGasMdSvd* GasMdSvd=new TGasMdSvd(GasDmHd); PGasMd GasMd(GasMdSvd);
  // select records for model building
  TIntV RecNV; GasDm->GetRecNV(StartDate, EndDate, StartRecN, EndRecN, MnCspt, RecNV);
  int Attrs=GasDm->GetAttrs(); int Recs=RecNV.Len();
  // pripare data for Svd
  TFltVV XVV(Recs, Attrs); TFltV YV(Recs); GasMdSvd->ClassValMom=TMom::New();
  for (int RecNN=0; RecNN<Recs; RecNN++){
    int RecN=RecNV[RecNN];
    double ClassVal=GasDm->GetClassVal(RecN);
    TFltV AttrValV; GasDm->GetAttrValV(RecN, AttrValV);
    for (int AttrN=0; AttrN<AttrValV.Len(); AttrN++){
      XVV.At(RecNN, AttrN)=AttrValV[AttrN];
    }
    YV[RecNN]=ClassVal;
    GasMdSvd->ClassValMom->Add(ClassVal);
  }
  GasMdSvd->ClassValMom->Def();
  // create svd model
  PSvd Svd=TSvd::New(XVV, YV);
  // take results from svd model
  Svd->GetCfV(GasMdSvd->CfV);
  Svd->GetCfUncerV(GasMdSvd->CfUncerV);
  GasMdSvd->ChiSq=Svd->GetChiSq();
  // return model
  return GasMd;
}

double TGasMdSvd::GetPredClassVal(const PGasDm& GasDm, const int& RecN) const {
  IAssert(GetGasDmHd()->IsCompatible(GasDm));
  double PredClassVal=0;
  TFltV AttrValV; GasDm->GetAttrValV(RecN, AttrValV);
  IAssert(AttrValV.Len()==CfV.Len());
  for (int AttrN=0; AttrN<AttrValV.Len(); AttrN++){
    PredClassVal+=CfV[AttrN]*AttrValV[AttrN];
  }
  return PredClassVal;
}

void TGasMdSvd::Print() const {
  printf("==========================================================\n");
  PGasDmHd GasDmHd=GetGasDmHd();
  TStrV AttrNmV; GasDmHd->GetAttrNmV(AttrNmV);
  TStr ClassNm=GasDmHd->GetClassNm();
  printf("%s = ", ClassNm.CStr());
  IAssert((AttrNmV.Len()==CfV.Len())&&(AttrNmV.Len()==CfUncerV.Len()));
  for (int AttrN=0; AttrN<AttrNmV.Len(); AttrN++){
    printf("\t%+g * %s\n", CfV[AttrN](), AttrNmV[AttrN].CStr());
  }
  printf("\n");
  //printf("ChiSq: %g\n", ChiSq);
  printf("ClassVal: %s\n", ClassValMom->GetStr(' ', ':', true).CStr());
}

/////////////////////////////////////////////////
// Gas-Model-Nearest-Neighbour
PGasMd TGasMdNNbr::New(const TStr& ParamStr, const PGasDm& GasDm,
 const TSecTm& StartDate, const TSecTm& EndDate,
 const int& StartRecN, const int& EndRecN, const double& MnCspt){
  // create domain header
  PGasDmHd GasDmHd=TGasDmHd::New(GasDm);
  // create model
  TGasMdNNbr* GasMdNNbr=new TGasMdNNbr(GasDmHd); PGasMd GasMd(GasMdNNbr);
  // get parameters
  PSIn ParamXmlSIn=TStrIn::New(ParamStr);
  PXmlDoc ParamXmlDoc=TXmlDoc::LoadTxt(ParamXmlSIn);
  GasMdNNbr->NNbrs=ParamXmlDoc->GetTagTokStr("Model|NNbr|K").GetInt();
  IAssert(GasMdNNbr->NNbrs>0);

  // select records for model building
  TIntV RecNV; GasDm->GetRecNV(StartDate, EndDate, StartRecN, EndRecN, MnCspt, RecNV);
  int Attrs=GasDm->GetAttrs(); int Recs=RecNV.Len();
  // prepare attribute & class data for nnbr model
  GasMdNNbr->AttrValVV.Gen(Recs); TMom::NewV(GasMdNNbr->AttrValMomV, Attrs);
  GasMdNNbr->ClassValV.Gen(Recs); GasMdNNbr->ClassValMom=TMom::New();
  for (int RecNN=0; RecNN<Recs; RecNN++){
    int RecN=RecNV[RecNN];
    // save attribute values
    TFltV& AttrValV=GasMdNNbr->AttrValVV[RecNN];
    GasDm->GetAttrValV(RecN, AttrValV);
    for (int AttrN=0; AttrN<Attrs; AttrN++){
      GasMdNNbr->AttrValMomV[AttrN]->Add(AttrValV[AttrN]);}
    // save class value
    double ClassVal=GasDm->GetClassVal(RecN);
    GasMdNNbr->ClassValV[RecNN]=ClassVal;
    GasMdNNbr->ClassValMom->Add(ClassVal);
  }
  TMom::DefV(GasMdNNbr->AttrValMomV);
  GasMdNNbr->ClassValMom->Def();
  // return model
  return GasMd;
}

double TGasMdNNbr::GetPredClassVal(const PGasDm& GasDm, const int& RecN) const {
  // check model & domain compatibility
  IAssert(GetGasDmHd()->IsCompatible(GasDm));
  // create distance/record-number class-value vector
  TFltIntKdV DistRecNKdV;
  // create predicting attribute values
  TFltV PredAttrValV; GasDm->GetAttrValV(RecN, PredAttrValV);
  for (int AttrValVN=0; AttrValVN<AttrValVV.Len(); AttrValVN++){
    // get record attribute values
    TFltV& DmAttrValV=AttrValVV[AttrValVN];
    // calculate distance
    double Dist=0;
    for (int AttrN=0; AttrN<PredAttrValV.Len(); AttrN++){
      double AttrValDist=PredAttrValV[AttrN]-DmAttrValV[AttrN];
      double AttrNrmVal=AttrValMomV[AttrN]->GetMedian();
      if (AttrNrmVal!=0){AttrValDist=AttrValDist/AttrNrmVal;}
      Dist+=TMath::Sqr(AttrValDist);
    }
    Dist=sqrt(Dist);
    // add distance
    TFltIntKd DistRecNKd(Dist, AttrValVN);
    DistRecNKdV.AddSorted(DistRecNKd, true, NNbrs);
  }
  // calculate predicted class value
  double PredClassVal=0;
  for (int DistRecNKdN=0; DistRecNKdN<DistRecNKdV.Len(); DistRecNKdN++){
    int RecN=DistRecNKdV[DistRecNKdN].Dat;
    double ClassVal=ClassValV[RecN];
    PredClassVal+=ClassVal;
  }
  if (DistRecNKdV.Len()>0){
    PredClassVal=PredClassVal/DistRecNKdV.Len();}
  // output target & nearest neighbour records
  /*TStrV AttrNmV; GasDmHd->GetAttrNmV(AttrNmV);
  printf("Target: Class:%g", GasDm->GetClassVal(RecN));
  for (int AttrValN=0; AttrValN<PredAttrValV.Len(); AttrValN++){
    printf(" %s:%g", AttrNmV[AttrValN].CStr(), PredAttrValV[AttrValN]());}
  printf("\n");
  for (int DistRecNKdN=0; DistRecNKdN<DistRecNKdV.Len(); DistRecNKdN++){
    double Dist=DistRecNKdV[DistRecNKdN].Key;
    int RecN=DistRecNKdV[DistRecNKdN].Dat;
    double ClassVal=ClassValV[RecN];
    PredClassVal+=ClassVal;
    // output nnbrs
    TFltV& AttrValV=AttrValVV[RecN];
    printf("%d. Dist:%g Class:%g", DistRecNKdN+1, Dist, ClassVal);
    for (int AttrValN=0; AttrValN<AttrValV.Len(); AttrValN++){
      printf(" %s:%g", AttrNmV[AttrValN].CStr(), AttrValV[AttrValN]());}
    printf("\n");
  }*/

  // return predicted class value
  return PredClassVal;
}

TStr TGasMdNNbr::GetPredExpl(const PGasDm& GasDm, const int& RecN) const {
  // check model & domain compatibility
  IAssert(GetGasDmHd()->IsCompatible(GasDm));
  // create distance/record-number class-value vector
  TFltIntKdV DistRecNKdV;
  // create predicting attribute values
  TFltV PredAttrValV; GasDm->GetAttrValV(RecN, PredAttrValV);
  for (int AttrValVN=0; AttrValVN<AttrValVV.Len(); AttrValVN++){
    // get record attribute values
    TFltV& DmAttrValV=AttrValVV[AttrValVN];
    // calculate distance
    double Dist=0;
    for (int AttrN=0; AttrN<PredAttrValV.Len(); AttrN++){
      double AttrValDist=PredAttrValV[AttrN]-DmAttrValV[AttrN];
      double AttrNrmVal=AttrValMomV[AttrN]->GetMedian();
      if (AttrNrmVal!=0){AttrValDist=AttrValDist/AttrNrmVal;}
      Dist+=TMath::Sqr(AttrValDist);
    }
    Dist=sqrt(Dist);
    // add distance
    TFltIntKd DistRecNKd(Dist, AttrValVN);
    DistRecNKdV.AddSorted(DistRecNKd, true, NNbrs);
  }
  // output target & nearest neighbour records
  TChA PredExplChA;
  /*
  char Bf[1000];
  TStrV AttrNmV; GasDmHd->GetAttrNmV(AttrNmV);
  sprintf(Bf, "    Target:\t\tClass:%5g", GasDm->GetClassVal(RecN)); PredExplChA+=Bf;
  for (int AttrValN=0; AttrValN<PredAttrValV.Len(); AttrValN++){
    sprintf(Bf, "\t%s:%5g", AttrNmV[AttrValN].CStr(), PredAttrValV[AttrValN]());
    PredExplChA+=Bf;
  }
  sprintf(Bf, "\n"); PredExplChA+=Bf;
  for (int DistRecNKdN=0; DistRecNKdN<DistRecNKdV.Len(); DistRecNKdN++){
    double Dist=DistRecNKdV[DistRecNKdN].Key;
    int RecN=DistRecNKdV[DistRecNKdN].Dat;
    double ClassVal=ClassValV[RecN];
    // output nnbrs
    TFltV& AttrValV=AttrValVV[RecN];
    sprintf(Bf, "    %d. Dist:%5.3g\tClass:%5g", DistRecNKdN+1, Dist, ClassVal);
    PredExplChA+=Bf;
    for (int AttrValN=0; AttrValN<AttrValV.Len(); AttrValN++){
      sprintf(Bf, "\t%s:%5g", AttrNmV[AttrValN].CStr(), AttrValV[AttrValN]());
      PredExplChA+=Bf;
    }
    sprintf(Bf, "\n"); PredExplChA+=Bf;
  }*/

  // return predicted class value
  return PredExplChA;
}

void TGasMdNNbr::Print() const {
  printf("==========================================================\n");
  printf("Nearest-Neigbours Window-Size: %d\n", NNbrs);
  printf("Records: %d\n", AttrValVV.Len());
  printf("ClassVal: %s\n", ClassValMom->GetStr(' ', ':', true).CStr());
}

/////////////////////////////////////////////////
// Gas-Model-Cubist
PGasMd TGasMdCubist::New(
 const TStr& ParamStr, const PGasDm& GasDm,
 const TSecTm& StartDate, const TSecTm& EndDate,
 const int& StartRecN, const int& EndRecN, const double& MnCspt){
  // create domain header
  PGasDmHd GasDmHd=TGasDmHd::New(GasDm);
  // create model
  TGasMdCubist* GasMdCubist=new TGasMdCubist(GasDmHd); PGasMd GasMd(GasMdCubist);
  // get parameters
  PSIn ParamXmlSIn=TStrIn::New(ParamStr);
  PXmlDoc ParamXmlDoc=TXmlDoc::LoadTxt(ParamXmlSIn);
  GasMdCubist->IdFNm=ParamXmlDoc->GetTagTokStr("Model|Cubist|FileId");
  TStr IdFNm=GasMdCubist->IdFNm;

  // generate ".names" file
  PSOut NamesSOut=TFOut::New(IdFNm+".names");
  // output class
  NamesSOut->PutStr(GasDm->GetClassNm()); NamesSOut->PutStr(".\n\n");
  NamesSOut->PutStr(GasDm->GetClassNm()); NamesSOut->PutStr(": ");
  NamesSOut->PutStr("continuous");
  NamesSOut->PutStr(".\n");
  // output attributes
  TStrV AttrNmV; GasDm->GetAttrNmV(AttrNmV);
  for (int AttrN=0; AttrN<AttrNmV.Len(); AttrN++){
    NamesSOut->PutStr(AttrNmV[AttrN]); NamesSOut->PutStr(": ");
    if (AttrNmV[AttrN]=="Dow"){
      for (int DowN=0; DowN<7; DowN++){
        if (DowN>0){NamesSOut->PutStr(",");}
        NamesSOut->PutStr(TTmInfo::GetDayOfWeekNm(DowN+1));
      }
    } else
    if (AttrNmV[AttrN]=="Month"){
      for (int MonthN=0; MonthN<12; MonthN++){
        if (MonthN>0){NamesSOut->PutStr(",");}
        NamesSOut->PutStr(TTmInfo::GetMonthNm(MonthN+1));
      }
    } else {
      NamesSOut->PutStr("continuous");
    }
    NamesSOut->PutStr(".\n");
  }
  NamesSOut=NULL;

  // select records for model building
  TIntV RecNV; GasDm->GetRecNV(StartDate, EndDate, StartRecN, EndRecN, MnCspt, RecNV);
  int Recs=RecNV.Len();
  // generate ".data" file
  PSOut DataSOut=TFOut::New(IdFNm+".data");
  GasMdCubist->ClassValMom=TMom::New();
  for (int RecNN=0; RecNN<Recs; RecNN++){
    int RecN=RecNV[RecNN];
    // get class & attribute values
    double ClassVal=GasDm->GetClassVal(RecN);
    TFltV AttrValV; GasDm->GetAttrValV(RecN, AttrValV);
    GasMdCubist->ClassValMom->Add(ClassVal);
    // output class & attribute values
    DataSOut->PutFlt(ClassVal);
    for (int AttrN=0; AttrN<AttrValV.Len(); AttrN++){
      DataSOut->PutStr(",");
      if (AttrNmV[AttrN]=="Dow"){
        DataSOut->PutStr(TTmInfo::GetDayOfWeekNm(AttrValV[AttrN]));
      } else
      if (AttrNmV[AttrN]=="Month"){
        DataSOut->PutStr(TTmInfo::GetMonthNm(AttrValV[AttrN]));
      } else {
        DataSOut->PutFlt(AttrValV[AttrN]);
      }
    }
    DataSOut->PutStr("\n");
  }
  GasMdCubist->ClassValMom->Def();
  DataSOut=NULL;

  // run Cubist
  TFile::Del(IdFNm+".model", false);
  TStr CubistCmLn=TStr("CubistX.exe -f ")+IdFNm/*+" "+OptStr*/;
  system(CubistCmLn.CStr());

  // load .names & .data & .model files
  {PSIn NamesSIn=TFIn::New(IdFNm+".names");
  GasMdCubist->NamesFileStr=TStr(NamesSIn);}
  {PSIn DataSIn=TFIn::New(IdFNm+".data");
  GasMdCubist->DataFileStr=TStr(DataSIn);}
  {PSIn ModelSIn=TFIn::New(IdFNm+".model");
  GasMdCubist->ModelFileStr=TStr(ModelSIn);}

  // delete generated files
  TFile::Del(IdFNm+".names", false);
  TFile::Del(IdFNm+".data", false);
  TFile::Del(IdFNm+".model", false);

  // return model
  return GasMd;
}

double TGasMdCubist::GetPredClassVal(const PGasDm& GasDm, const int& RecN) const {
  // check model & domain compatibility
  IAssert(GetGasDmHd()->IsCompatible(GasDm));

  // create .names & .data & .model files
  {PSOut NamesSOut=TFOut::New(IdFNm+".names");
  NamesSOut->PutStr(NamesFileStr);}
  {PSOut DataSOut=TFOut::New(IdFNm+".data");
  DataSOut->PutStr(DataFileStr);}
  {PSOut ModelSOut=TFOut::New(IdFNm+".model");
  ModelSOut->PutStr(ModelFileStr);}

  // create .cases file
  PSOut CasesSOut=TFOut::New(IdFNm+".cases");
  TStrV AttrNmV; GasDm->GetAttrNmV(AttrNmV);
  double ClassVal=GasDm->GetClassVal(RecN);
  TFltV AttrValV; GasDm->GetAttrValV(RecN, AttrValV);
  // output class & attribute values
  CasesSOut->PutFlt(ClassVal);
  for (int AttrN=0; AttrN<AttrValV.Len(); AttrN++){
    CasesSOut->PutStr(",");
    if (AttrNmV[AttrN]=="Dow"){
      CasesSOut->PutStr(TTmInfo::GetDayOfWeekNm(AttrValV[AttrN]));
    } else
    if (AttrNmV[AttrN]=="Month"){
      CasesSOut->PutStr(TTmInfo::GetMonthNm(AttrValV[AttrN]));
    } else {
      CasesSOut->PutFlt(AttrValV[AttrN]);
    }
  }
  CasesSOut->PutStr("\n");
  CasesSOut=NULL;

  // run Cubist classifier
  TFile::Del(IdFNm+".xml", false);
  TStr CubistCmLn=TStr("CubistC.exe -f ")+IdFNm;
  system(CubistCmLn.CStr());

  // load classification from xml file
  PXmlDoc XmlDoc=TXmlDoc::LoadTxt(IdFNm+".xml");
  TStr PredClassValStr=XmlDoc->GetTagTokStr("Prediction|Predicted");
  double PredClassVal=PredClassValStr.GetFlt();

  // delete generated files
  TFile::Del(IdFNm+".names", false);
  TFile::Del(IdFNm+".data", false);
  TFile::Del(IdFNm+".model", false);
  TFile::Del(IdFNm+".cases", false);
  TFile::Del(IdFNm+".xml", false);

  // return predicted class value
  return PredClassVal;
}

void TGasMdCubist::Print() const {
  printf("==========================================================\n");
  printf("ClassVal: %s\n", ClassValMom->GetStr(' ', ':', true).CStr());
}

/////////////////////////////////////////////////
// Gas-Model-Svm
PGasMd TGasMdSvm::New(const TStr& ParamStr, const PGasDm& GasDm,
 const TSecTm& StartDate, const TSecTm& EndDate,
 const int& StartRecN, const int& EndRecN, const double& MnCspt){
  // create domain header
  PGasDmHd GasDmHd=TGasDmHd::New(GasDm);
  // create model
  TGasMdSvm* GasMdSvm=new TGasMdSvm(GasDmHd); PGasMd GasMd(GasMdSvm);
  // get parameters
  PSIn ParamXmlSIn=TStrIn::New(ParamStr);
  PXmlDoc ParamXmlDoc=TXmlDoc::LoadTxt(ParamXmlSIn);
  GasMdSvm->IdFNm=ParamXmlDoc->GetTagTokStr("Model|Svm|FileId");
  TStr IdFNm=GasMdSvm->IdFNm;

  // select records for model building
  TIntV RecNV; GasDm->GetRecNV(StartDate, EndDate, StartRecN, EndRecN, MnCspt, RecNV);
  int Recs=RecNV.Len();

  // create train examples file
  PSOut TrainSOut=TFOut::New(IdFNm+".train");
  // output class & attributes names as comment
  TrainSOut->PutStr("# ");
  TStrV AttrNmV; GasDm->GetAttrNmV(AttrNmV);
  for (int AttrN=0; AttrN<AttrNmV.Len(); AttrN++){
    TrainSOut->PutStr(AttrNmV[AttrN]); TrainSOut->PutStr(" ");}
  TrainSOut->PutStr(GasDm->GetClassNm());
  TrainSOut->PutStr("\n");
  // output examples header
  TrainSOut->PutStr("@examples\n");
  TrainSOut->PutStr("dimension "); TrainSOut->PutInt(AttrNmV.Len()); TrainSOut->PutStr("\n");
  TrainSOut->PutStr("format xy\n");
  // output class & attributes values
  GasMdSvm->ClassValMom=TMom::New();
  for (int RecNN=0; RecNN<Recs; RecNN++){
    int RecN=RecNV[RecNN];
    // get class & attribute values
    double ClassVal=GasDm->GetClassVal(RecN);
    TFltV AttrValV; GasDm->GetAttrValV(RecN, AttrValV);
    GasMdSvm->ClassValMom->Add(ClassVal);
    // output class & attribute values
    for (int AttrN=0; AttrN<AttrValV.Len(); AttrN++){
      TrainSOut->PutFlt(AttrValV[AttrN]);
      TrainSOut->PutStr(" ");
    }
    TrainSOut->PutFlt(ClassVal);
    TrainSOut->PutStr("\n");
  }
  GasMdSvm->ClassValMom->Def();
  TrainSOut=NULL;

  // create parameter file
  PSOut ParamSOut=TFOut::New(IdFNm+".param");
  ParamSOut->PutStr("@kernel\n");
  ParamSOut->PutStr("type dot\n");
  ParamSOut->PutStr("@parameters\n");
  ParamSOut->PutStr("C 1000\n");
  ParamSOut->PutStr("epsilon 0.1\n");
  ParamSOut=NULL;

  // run Svm
  TFile::Del(IdFNm+".train.svm", false);
  TStr SvmCmLn=TStr("MySvmLearn.exe ")+IdFNm+".train "+IdFNm+".param";
  system(SvmCmLn.CStr());

  // load model file
  {PSIn ModelSIn=TFIn::New(IdFNm+".train.svm");
  GasMdSvm->ModelFileStr=TStr(ModelSIn);}

  // delete generated files
  TFile::Del(IdFNm+".train", false);
  TFile::Del(IdFNm+".param", false);
  TFile::Del(IdFNm+".train.svm", false);

  // return model
  return GasMd;
}

double TGasMdSvm::GetPredClassVal(const PGasDm& GasDm, const int& RecN) const {
  // check model & domain compatibility
  IAssert(GetGasDmHd()->IsCompatible(GasDm));

  // create model file
  {PSOut ModelSOut=TFOut::New(IdFNm+".train.svm");
  ModelSOut->PutStr(ModelFileStr);}

  // create test examples file
  PSOut TestSOut=TFOut::New(IdFNm+".test");
  // output class & attributes names as comment
  TestSOut->PutStr("# ");
  TStrV AttrNmV; GasDm->GetAttrNmV(AttrNmV);
  for (int AttrN=0; AttrN<AttrNmV.Len(); AttrN++){
    TestSOut->PutStr(AttrNmV[AttrN]); TestSOut->PutStr(" ");}
  TestSOut->PutStr(GasDm->GetClassNm());
  TestSOut->PutStr("\n");
  // output examples header
  TestSOut->PutStr("@examples\n");
  TestSOut->PutStr("dimension "); TestSOut->PutInt(AttrNmV.Len()); TestSOut->PutStr("\n");
  TestSOut->PutStr("format xy\n");
  // get class & attribute values
  double ClassVal=GasDm->GetClassVal(RecN);
  TFltV AttrValV; GasDm->GetAttrValV(RecN, AttrValV);
  // output class & attribute values
  {for (int AttrN=0; AttrN<AttrValV.Len(); AttrN++){
    TestSOut->PutFlt(AttrValV[AttrN]);
    TestSOut->PutStr(" ");
  }}
  TestSOut->PutFlt(ClassVal);
  TestSOut->PutStr("\n");
  TestSOut=NULL;

  // create parameter file
  PSOut ParamSOut=TFOut::New(IdFNm+".param");
  ParamSOut->PutStr("@kernel\n");
  ParamSOut->PutStr("type dot\n");
  ParamSOut->PutStr("@parameters\n");
  ParamSOut->PutStr("C 1000\n");
  ParamSOut->PutStr("epsilon 0.1\n");
  ParamSOut=NULL;

  // run Svm classifier
  TFile::Del(IdFNm+"test.pred", false);
  TStr SvmCmLn=TStr("MySvmPredict.exe ")+
   IdFNm+".train.svm "+IdFNm+".test "+IdFNm+".param";
  system(SvmCmLn.CStr());

  // load classification from ".pred" file
  PSIn PredSIn=TFIn::New(IdFNm+".test.pred");
  TILx PredLx(PredSIn, TFSet()|iloRetEoln|iloSigNum);
  PredLx.GetSym(syLn);
  double PredClassVal=PredLx.GetFlt();

  // delete generated files
  TFile::Del(IdFNm+".train.svm", false);
  TFile::Del(IdFNm+".test", false);
  TFile::Del(IdFNm+".param", false);
  TFile::Del(IdFNm+".test.pred", false);

  // return predicted class value
  return PredClassVal;
}

void TGasMdSvm::Print() const {
  printf("==========================================================\n");
  printf("ClassVal: %s\n", ClassValMom->GetStr(' ', ':', true).CStr());
}

/////////////////////////////////////////////////
// Gas-Regression-Node
PGasRegNd TGasRegNd::New(const int& MnLeafExs,
 const TVec<TFltV>& AttrValVV, const TFltV& ClassValV, const TIntV& ExNV){
  IAssert(ExNV.Len()>0);
  // create node
  PGasRegNd RegNd=PGasRegNd(new TGasRegNd());
  // calculate class values
  RegNd->ClassValMom=TMom::New();
  for (int ExNN=0; ExNN<ExNV.Len(); ExNN++){
    RegNd->ClassValMom->Add(ClassValV[ExNV[ExNN]]);}
  RegNd->ClassValMom->Def();
  // make leaf if less than minimimal number of examples in node
  if (ExNV.Len()<MnLeafExs){
    return RegNd;}
  int MnSubRegNdExs=TInt::GetMx(ExNV.Len()/10, MnLeafExs);
  // create best split state variables
  double BestSplitQual=-1;
  int BestSplitAttrN=-1; double BestSplitAttrVal=0;
  TIntV BestLExNV; TIntV BestRExNV;
  // find best split
  int Attrs=AttrValVV[0].Len();
  for (int SplitAttrN=0; SplitAttrN<Attrs; SplitAttrN++){
    // collect split values
    TFltV SplitValV; PMom SplitValMom=TMom::New();
    for (int ExNN=0; ExNN<ExNV.Len(); ExNN++){
      double AttrVal=AttrValVV[ExNV[ExNN]][SplitAttrN];
      SplitValV.Add(AttrVal); SplitValMom->Add(AttrVal);
    }
    SplitValV.Sort(); SplitValMom->Def();
    double MnSplitValResol=
     (SplitValMom->GetQuart3()-SplitValMom->GetQuart1())/100;
    // try all splits
    double PrevSplitVal;
    for (int SplitValN=1; SplitValN<SplitValV.Len()-1; SplitValN++){
      // get new split value
      double SplitVal=SplitValV[SplitValN];
      // test if new split value differs enough from previous one
      if ((SplitValN>1)&&(SplitVal-PrevSplitVal<=MnSplitValResol)){continue;}
      PrevSplitVal=SplitVal;
      // create left & right split moments & example sets
      PMom LClassValMom=TMom::New(); TIntV LExNV;
      PMom RClassValMom=TMom::New(); TIntV RExNV;
      // distribute examples to left or right subset
      for (int ExNN=0; ExNN<ExNV.Len(); ExNN++){
        int ExN=ExNV[ExNN];
        double AttrVal=AttrValVV[ExN][SplitAttrN];
        if (AttrVal<SplitVal){
          LClassValMom->Add(ClassValV[ExN]); LExNV.Add(ExN);
        } else {
          RClassValMom->Add(ClassValV[ExN]); RExNV.Add(ExN);
        }
      }
      LClassValMom->Def(); RClassValMom->Def();
      // test if the split is the best
      if ((LClassValMom->IsUsable()&&RClassValMom->IsUsable())&&
       (LExNV.Len()>MnSubRegNdExs)&&(RExNV.Len()>MnSubRegNdExs)){
        //double SplitQual=
        // sqr(LClassValMom->GetVari())+sqr(RClassValMom->GetVari());
        double SplitQual=
         TMath::Sqr(LClassValMom->GetSDev()*LExNV.Len())+
         TMath::Sqr(RClassValMom->GetSDev()*RExNV.Len());
        if ((BestSplitQual==-1)||(SplitQual<BestSplitQual)){
          BestSplitQual=SplitQual;
          BestSplitAttrN=SplitAttrN; BestSplitAttrVal=SplitVal;
          BestLExNV=LExNV; BestRExNV=RExNV;
        }
      }
    }
  }
  // create split & sub-trees
  if (BestSplitQual!=-1){
    RegNd->SplitQual=BestSplitQual;
    RegNd->SplitAttrN=BestSplitAttrN;
    RegNd->SplitAttrVal=BestSplitAttrVal;
    if ((BestLExNV.Len()>0)&&(BestRExNV.Len()>0)){
      RegNd->LRegNd=TGasRegNd::New(MnLeafExs, AttrValVV, ClassValV, BestLExNV);
      RegNd->RRegNd=TGasRegNd::New(MnLeafExs, AttrValVV, ClassValV, BestRExNV);
    }
  }
  // return current node
  printf(".");
  return RegNd;
}

double TGasRegNd::GetPredClassVal(const TFltV& AttrValV) const {
  if (IsLeaf()){
    return ClassValMom->GetMedian();
  } else {
    if (AttrValV[SplitAttrN]<SplitAttrVal){
      return LRegNd->GetPredClassVal(AttrValV);
    } else {
      return RRegNd->GetPredClassVal(AttrValV);
    }
  }
}

void TGasRegNd::Print(const PGasDmHd& GasDmHd, const int& Levs) const {
  if (IsLeaf()){
    for (int LevN=0; LevN<Levs; LevN++){printf("   ");}
    printf("%s=%g(%g) [%d Exs.]\n",
     GasDmHd->GetClassNm().CStr(),
     ClassValMom->GetMean(), ClassValMom->GetSDev(), ClassValMom->GetVals());
  } else {
    for (int LevN=0; LevN<Levs; LevN++){printf("   ");}
    printf("%s <  %g [%s=%g(%g)] [%d Exs.]\n",
     GasDmHd->GetAttrNm(SplitAttrN).CStr(), SplitAttrVal,
     GasDmHd->GetClassNm().CStr(),
     ClassValMom->GetMean(), ClassValMom->GetSDev(), ClassValMom->GetVals());
    LRegNd->Print(GasDmHd, Levs+1);
    {for (int LevN=0; LevN<Levs; LevN++){printf("   ");}}
    printf("%s >= %g [%s=%g(%g)] [%d Exs.]\n",
     GasDmHd->GetAttrNm(SplitAttrN).CStr(), SplitAttrVal,
     GasDmHd->GetClassNm().CStr(),
     ClassValMom->GetMean(), ClassValMom->GetSDev(), ClassValMom->GetVals());
    RRegNd->Print(GasDmHd, Levs+1);
  }
}

/////////////////////////////////////////////////
// Gas-Model-RegTree
PGasMd TGasMdRegTree::New(const TStr& ParamStr, const PGasDm& GasDm,
 const TSecTm& StartDate, const TSecTm& EndDate,
 const int& StartRecN, const int& EndRecN, const double& MnCspt){
  // create domain header
  PGasDmHd GasDmHd=TGasDmHd::New(GasDm);
  // create model
  TGasMdRegTree* GasMdRegTree=new TGasMdRegTree(GasDmHd); PGasMd GasMd(GasMdRegTree);
  // get parameters
  PSIn ParamXmlSIn=TStrIn::New(ParamStr);
  PXmlDoc ParamXmlDoc=TXmlDoc::LoadTxt(ParamXmlSIn);
  int MnLeafExs=ParamXmlDoc->GetTagTokStr("Model|RegTree|MnLeafExs").GetInt();

  // select records for model building
  TIntV RecNV; GasDm->GetRecNV(StartDate, EndDate, StartRecN, EndRecN, MnCspt, RecNV);
  /*int Attrs=GasDm->GetAttrs();*/ int Recs=RecNV.Len();
  // prepare data for learning
  TVec<TFltV> AttrValVV(Recs);
  TFltV ClassValV(Recs); GasMdRegTree->ClassValMom=TMom::New();
  for (int RecNN=0; RecNN<Recs; RecNN++){
    int RecN=RecNV[RecNN];
    // save attribute values
    TFltV& AttrValV=AttrValVV[RecNN];
    GasDm->GetAttrValV(RecN, AttrValV);
    // save class value
    double ClassVal=GasDm->GetClassVal(RecN);
    ClassValV[RecNN]=ClassVal;
    GasMdRegTree->ClassValMom->Add(ClassVal);
  }
  GasMdRegTree->ClassValMom->Def();
  // create regression-tree model
  int AllExs=AttrValVV.Len();
  TIntV AllExNV(AllExs); for (int ExN=0; ExN<AllExs; ExN++){AllExNV[ExN]=ExN;}
  GasMdRegTree->RegNd=TGasRegNd::New(MnLeafExs, AttrValVV, ClassValV, AllExNV);
  // return model
  return GasMd;
}

double TGasMdRegTree::GetPredClassVal(const PGasDm& GasDm, const int& RecN) const {
  IAssert(GetGasDmHd()->IsCompatible(GasDm));
  TFltV AttrValV; GasDm->GetAttrValV(RecN, AttrValV);
  double PredClassVal=RegNd->GetPredClassVal(AttrValV);
  return PredClassVal;
}

void TGasMdRegTree::Print() const {
  printf("==========================================================\n");
  printf("ClassVal: %s\n", ClassValMom->GetStr(' ', ':', true).CStr());
  RegNd->Print(GasDmHd);
}

/////////////////////////////////////////////////
// Gas-Model-Corection
TGasMdCor::TGasMdCor(const TStr& LtpFPath):
  DateProdNmToCorFactMomH(){
  TFFile FFile(LtpFPath, ".Ltp", false); TStr FNm;
  while (FFile.Next(FNm)){
    //if (FFile.GetFNmN()>100){break;}
    printf(".");
    PGasLtpBs GasLtpBs=TGasLtpBs::Load(FNm);
    TStr ProdNm=GasLtpBs->ProdNm;
    // create domain
    PGasDm GasDm=TGasDm::GetDayGasDm(GasLtpBs);
    // determine min. & max. Ltp dates
    TSecTm MnDate=GasLtpBs->GetMnDate();
    TSecTm MxDate=GasLtpBs->GetMxDate();
    // create model
    PGasMd GasMd=TGasMd::New(
     gmtCTb, NULL, TGasMd::GetMdParamStr(), GasDm, MnDate, MxDate, -1, -1, 100);
    // traverse records
    for (int RecN=0; RecN<GasDm->GetRecs(); RecN++){
      double Cspt=GasDm->GetClassVal(RecN);
      double PredCspt=GasMd->GetPredClassVal(GasDm, RecN);
      if (PredCspt==0){continue;}
      double CorFact=Cspt/PredCspt;
      TSecTm Date=GasDm->GetDateVal(RecN);
      TDateProdNmPr DateProdNmPr(Date, ProdNm);
      if (!DateProdNmToCorFactMomH.IsKey(DateProdNmPr)){
        DateProdNmToCorFactMomH.AddDat(DateProdNmPr, TMom::New());}
      DateProdNmToCorFactMomH.GetDat(DateProdNmPr)->Add(CorFact);
    }
  }
  // define moments
  for (int DateProdNmP=0; DateProdNmP<DateProdNmToCorFactMomH.Len(); DateProdNmP++){
    DateProdNmToCorFactMomH[DateProdNmP]->Def();}
}

double TGasMdCor::GetCorFact(const TSecTm& Date, const TStr& ProdNm) const {
  TDateProdNmPr DateProdNmPr(Date, ProdNm);
  int DateProdNmP;
  if (DateProdNmToCorFactMomH.IsKey(DateProdNmPr, DateProdNmP)){
    PMom CorFactMom=DateProdNmToCorFactMomH[DateProdNmP];
    if (CorFactMom->IsUsable()){
      return CorFactMom->GetMean();
    } else {
      return 1;
    }
  } else {
    return 1;
  }
}

void TGasMdCor::SaveTxt(const TStr& FNm) const {
  // sort according to dates and products
  typedef TTriple<TSecTm, TStr, TInt> TDateProdNmIdTr;
  TVec<TDateProdNmIdTr> DateProdNmIdTrV;
  for (int DateProdNmP=0; DateProdNmP<DateProdNmToCorFactMomH.Len(); DateProdNmP++){
    TSecTm Date=DateProdNmToCorFactMomH.GetKey(DateProdNmP).Val1;
    TStr ProdNm=DateProdNmToCorFactMomH.GetKey(DateProdNmP).Val2;
    TDateProdNmIdTr DateProdNmIdTr(Date, ProdNm, DateProdNmP);
    DateProdNmIdTrV.Add(DateProdNmIdTr);
  }
  DateProdNmIdTrV.Sort();
  // save to file
  TFOut LogFOut(FNm); FILE* fLog=LogFOut.GetFileId();
  for (int DpiN=0; DpiN<DateProdNmIdTrV.Len(); DpiN++){
    TSecTm Date=DateProdNmIdTrV[DpiN].Val1;
    TStr ProdNm=DateProdNmIdTrV[DpiN].Val2;
    //if (ProdNm!="EUROSUPER 95 OKT"){continue;}
    int DateProdNmP=DateProdNmIdTrV[DpiN].Val3;
    PMom CorFactMom=DateProdNmToCorFactMomH[DateProdNmP];
    fprintf(fLog, "%s - %-25s - %s\n",
     Date.GetDtStr().CStr(), ProdNm.CStr(),
     CorFactMom->GetStr(' ', ':', true).CStr());
  }
}

/////////////////////////////////////////////////
// Gas-Order
double TGasOrder::GetQuantity(const TSecTm& Dt, TGasOrderV& GasOrderV){
  double Quantity=0;
  for (int GasOrderN=0; GasOrderN<GasOrderV.Len(); GasOrderN++){
    if (GasOrderV[GasOrderN]->ReceiveDt==Dt){
      Quantity+=GasOrderV[GasOrderN]->Quantity;}
  }
  return Quantity;
}

double TGasOrder::GetQuantity(
 const TSecTm& MnTm, const TSecTm& MxTm, TGasOrderV& GasOrderV){
  double Quantity=0;
  for (int GasOrderN=0; GasOrderN<GasOrderV.Len(); GasOrderN++){
    TSecTm ReceiveDt=GasOrderV[GasOrderN]->ReceiveDt;
    if ((MnTm<=ReceiveDt)&&(ReceiveDt<MxTm)){
      Quantity+=GasOrderV[GasOrderN]->Quantity;}
  }
  return Quantity;
}

void TGasOrder::DelOrders(const TSecTm& Dt, TGasOrderV& GasOrderV){
  PGasOrder GasOrder=TGasOrder::New(Dt, Dt, 0, false);
  GasOrderV.DelAll(GasOrder);
}

void TGasOrder::DelOrders(
 const TSecTm& MnTm, const TSecTm& MxTm, TGasOrderV& GasOrderV){
  for (int GasOrderN=GasOrderV.Len()-1; GasOrderN>=0; GasOrderN--){
    TSecTm ReceiveDt=GasOrderV[GasOrderN]->ReceiveDt;
    if ((MnTm<=ReceiveDt)&&(ReceiveDt<MxTm)){
      GasOrderV.Del(GasOrderN);
    }
  }
}

void TGasOrder::DelUnapprovedOrders(
 const TSecTm& MnTm, TGasOrderV& GasOrderV, TStrV& MsgStrV){
  MsgStrV.Clr();
  for (int GasOrderN=GasOrderV.Len()-1; GasOrderN>=0; GasOrderN--){
    if (!GasOrderV[GasOrderN]->ApprovedP){
      TSecTm ReceiveDt=GasOrderV[GasOrderN]->ReceiveDt;
      if (MnTm<=ReceiveDt){
        MsgStrV.Add(GasOrderV[GasOrderN]->GetStr());
        GasOrderV.Del(GasOrderN);
      }
    }
  }
}

PGasOrder TGasOrder::LoadXml(const PXmlTok& XmlTok){
  // issue-date
  TStr IssueDtStr=XmlTok->GetTagTokStr("IssueDate");
  TSecTm IssueDt=TSecTm::GetDtTmFromDmyStr(IssueDtStr);
  // receive-date
  TStr ReceiveDtStr=XmlTok->GetTagTokStr("ReceiveDate");
  TSecTm ReceiveDt=TSecTm::GetDtTmFromDmyStr(ReceiveDtStr);
  // quantity
  TStr QuantityStr=XmlTok->GetTagTokStr("Quantity");
  double Quantity=QuantityStr.GetFlt(-1);
  // create gas-order
  PGasOrder GasOrder=TGasOrder::New(IssueDt, ReceiveDt, Quantity, true);
  // return
  return GasOrder;
}

void TGasOrder::SaveXml(const PSOut& SOut, const TStr& IndentStr) const {
  SOut->PutStr(IndentStr); SOut->PutStr("<GasOrder>\n");
  SOut->PutStr(IndentStr);
  SOut->PutStr(TStr::GetStr(IssueDt.GetDtMdyStr(), "  <IssueDate>%s</IssueDate>\n"));
  SOut->PutStr(IndentStr);
  SOut->PutStr(TStr::GetStr(ReceiveDt.GetDtMdyStr(), "  <ReceiveDate>%s</ReceiveDate>\n"));
  SOut->PutStr(IndentStr);
  SOut->PutStr(TFlt::GetStr(Quantity, "  <Quantity>%g</Quantity>\n"));
  SOut->PutStr(IndentStr); SOut->PutStr("</GasOrder>\n");
}

/////////////////////////////////////////////////
// Gas-Station
PGasStation TGasStation::LoadXml(const PXmlTok& XmlTok){
  // create gas-station
  PGasStation GasStation=TGasStation::New();
  // load definition-data
  GasStation->DescStr=XmlTok->GetTagTokStr("Description");
  GasStation->ProdNm=XmlTok->GetTagTokStr("Product");
  GasStation->FullVolumeVal=XmlTok->GetTagTokStr("FullVolume").GetFlt(-1);
  GasStation->SafetyVolumeVal=XmlTok->GetTagTokStr("SafetyVolume").GetFlt(-1);
  GasStation->DeliveryDays=XmlTok->GetTagTokStr("DeliveryDays").GetFlt(-1);
  GasStation->StockDurationDays=XmlTok->GetTagTokStr("StockDurationDays").GetFlt(-1);
  GasStation->OrderQuantityCorFact=XmlTok->GetTagTokStr("OrderQuantityCorrection").GetFlt(-1);
  GasStation->ExceptPredCorFact=XmlTok->GetTagTokStr("ExceptionPredictionCorrection").GetFlt(-1);
  GasStation->HistWnDays=XmlTok->GetTagTokStr("HistoryWindowDays").GetFlt(-1);
  GasStation->TmGridHours=XmlTok->GetTagTokStr("TimeGridHours").GetFlt(-1);
  // load state-data
  GasStation->VolumeVal=XmlTok->GetTagTokStr("Volume").GetFlt(-1);
  // gas-order-list
  TXmlTokV GasOrderXmlTokV;
  XmlTok->GetTagTokV("GasOrderList|GasOrder", GasOrderXmlTokV);
  GasStation->GasOrderV.Clr();
  for (int GasOrderN=0; GasOrderN<GasOrderXmlTokV.Len(); GasOrderN++){
    PGasOrder GasOrder=TGasOrder::LoadXml(GasOrderXmlTokV[GasOrderN]);
    GasStation->GasOrderV.Add(GasOrder);
  }
  // return
  return GasStation;
}

PGasStation TGasStation::LoadXml(const TStr& FNm){
  // open file
  PSIn SIn=TFIn::New(FNm);
  // load xml file
  PXmlDoc XmlDoc=TXmlDoc::LoadTxt(SIn);
  if (!XmlDoc->IsOk()){
    TExcept::Throw("Invalid Xml File", SIn->GetSNm());}
  // return result
  return LoadXml(XmlDoc->GetTok());
}

void TGasStation::SaveXml(const PSOut& SOut) const {
  SOut->PutStr("<GasStation>\n");
  SOut->PutStr(TStr::GetStr(DescStr, "  <Description>%s</Description>\n"));
  SOut->PutStr(TStr::GetStr(ProdNm, "  <Product>%s</Product>\n"));
  SOut->PutStr(TFlt::GetStr(FullVolumeVal, "  <FullVolume>%g</FullVolume>\n"));
  SOut->PutStr(TFlt::GetStr(SafetyVolumeVal, "  <SafetyVolume>%g</SafetyVolume>\n"));
  SOut->PutStr(TInt::GetStr(DeliveryDays, "  <DeliveryDays>%d</DeliveryDays>\n"));
  SOut->PutStr(TInt::GetStr(StockDurationDays, "  <StockDurationDays>%d</StockDurationDays>\n"));
  SOut->PutStr(TFlt::GetStr(OrderQuantityCorFact, "  <OrderQuantityCorrection>%g</OrderQuantityCorrection>\n"));
  SOut->PutStr(TFlt::GetStr(ExceptPredCorFact, "  <ExceptionPredictionCorrection>%g</ExceptionPredictionCorrection>\n"));
  SOut->PutStr(TInt::GetStr(HistWnDays, "  <HistoryWindowDays>%d</HistoryWindowDays>\n"));
  SOut->PutStr(TInt::GetStr(TmGridHours, "  <TimeGridHours>%d</TimeGridHours>\n"));

  SOut->PutStr(TFlt::GetStr(VolumeVal, "  <Volume>%g</Volume>\n"));
  SOut->PutStr("  <NewGasOrderList>\n");
  for (int GasOrderN=0; GasOrderN<NewGasOrderV.Len(); GasOrderN++){
    NewGasOrderV[GasOrderN]->SaveXml(SOut, "    ");
  }
  SOut->PutStr("  </NewGasOrderList>\n");
  SOut->PutStr("</GasStation>\n");
}

void TGasStation::DumpDef(FILE* fOut) const {
  fprintf(fOut, "===GasStation-Start========\n");
  fprintf(fOut, "DescStr: %s\n", DescStr.CStr());
  fprintf(fOut, "ProdStr: %s\n", ProdNm.CStr());
  fprintf(fOut, "FullVolumeVal: %g\n", FullVolumeVal);
  fprintf(fOut, "SafetyVolumeVal: %g\n", SafetyVolumeVal);
  fprintf(fOut, "DeliveryDays: %d\n", DeliveryDays);
  fprintf(fOut, "StockDurationDays: %d\n", StockDurationDays);
  fprintf(fOut, "OrderQuantityCorFact: %g\n", OrderQuantityCorFact);
  fprintf(fOut, "ExceptPredCorFact: %g\n", ExceptPredCorFact);
  fprintf(fOut, "HistWnDays: %d\n", HistWnDays);
  fprintf(fOut, "TmGridHours: %d\n", TmGridHours);
  fprintf(fOut, "VolumeVal: %d\n", VolumeVal);
  fprintf(fOut, "GasOrderV: (%d)\n", GasOrderV.Len());
  for (int GasOrderN=0; GasOrderN<GasOrderV.Len(); GasOrderN++){
    fprintf(fOut, "  %s\n", GasOrderV[GasOrderN]->GetStr().CStr());}
  fprintf(fOut, "NewGasOrderV: (%d)\n", NewGasOrderV.Len());
  {for (int GasOrderN=0; GasOrderN<NewGasOrderV.Len(); GasOrderN++){
    fprintf(fOut, "  %s\n", NewGasOrderV[GasOrderN]->GetStr().CStr());}}
  fprintf(fOut, "===GasStation-End==========\n");
}

/////////////////////////////////////////////////
// Gas-Evaluation
void TGasExe::GenPredEval(
 const PGasMdMType& GasMdMType, const TStr& GasMdParamStr,
 const PGasMdCor& GasMdCor, const bool& UseMdCorP,
 const PGasDm& GasDm,
 const TSecTm& MnDate, const TSecTm& MxDate,
 const int& TrainWnLen, const int& TestWnLen, const int& WnStepLen,
 const double& MnCspt, const double& MnOutlierRelErr,
 const TStr& LogFNm, const int& LogVerbLev, const bool& AppendToLog,
 const TStr& TabLogFNm, const bool& AppendToTabLog,
 FILE* fOlap, const TStr& OlapPfxLn,
 TGasPredV& GasPredV,
 PMom& AllPredAbsErrMom, PMom& AllPredRelErrMom,
 const bool& PrintToScrP){
  // create/append log file
  TFOut LogFOut(LogFNm, AppendToLog); FILE* fLog=LogFOut.GetFileId();
  TFOut TabLogFOut(TabLogFNm, AppendToTabLog); FILE* fTabLog=TabLogFOut.GetFileId();
  // create evaluation time stamp
  TSecTm EvalStampTm=TSecTm::GetCurTm();
  // save evaluation header to log
  fprintf(fLog, "===Start-Evaluation======================\n");
  fprintf(fLog, "Evaluation (%s)\n", EvalStampTm.GetStr().CStr());
  if (GasMdMType->GetMdTypes()==1){
    TGasMdType GasMdType=GasMdMType->GetMdType(0);
    fprintf(fLog, "Training algorithm '%s'\n", TGasMd::GetGasMdTypeNm(GasMdType).CStr());
  } else {
    for (int MdTypeN=0; MdTypeN<GasMdMType->GetMdTypes(); MdTypeN++){
      TGasMdType GasMdType=GasMdMType->GetMdType(MdTypeN);
      double GasMdWgt=GasMdMType->GetMdWgt(MdTypeN);
      fprintf(fLog, "Training algorithm '%s'(%g)\n", TGasMd::GetGasMdTypeNm(GasMdType).CStr(), GasMdWgt);
    }
  }
  fprintf(fLog, "Location/Tank/Product: %s\n", GasDm->GetDescStr().CStr());
  fprintf(fLog, "Min. Date: %s   Max. Date: %s\n", MnDate.GetDtStr().CStr(), MxDate.GetDtStr().CStr());
  fprintf(fLog, "Train Window: %d   Test Window: %d   Window Step: %d\n", TrainWnLen, TestWnLen, WnStepLen);
  fprintf(fLog, "Min. Consumption: %g\n", MnCspt);
  if (LogVerbLev>0){
    fprintf(fLog, "=========================================\n");}
  // prepare predictions and global absolute & relatve error statistics
  GasPredV.Clr();
  AllPredAbsErrMom=TMom::New();
  AllPredRelErrMom=TMom::New();
  // outlier descriptions vector
  TStrV OutlierStrV;
  // slide time window
  for (TSecTm Date=MnDate; Date<MxDate; Date.AddDays(WnStepLen)){
    // determine start & end & predict dates
    TSecTm StartDate=Date;
    TSecTm EndDate=TSecTm(Date).AddDays(TrainWnLen);
    TSecTm StartPredDate=TSecTm(Date).AddDays(TrainWnLen+1);
    TSecTm EndPredDate=TSecTm(Date).AddDays(TrainWnLen+1+TestWnLen-1);
    if (EndPredDate>=MxDate){continue;}
    TIntV TrainRecNV; GasDm->GetRecNV(StartDate, EndDate, -1, -1, MnCspt, TrainRecNV);
    int TrainRecs=TrainRecNV.Len();
    // save window info to log
    if (LogVerbLev>0){
      fprintf(fLog, "---Start-Window--------------------------\n");
      fprintf(fLog, "Training From %s   To %s (%d records)\n",
       StartDate.GetDtStr().CStr(), EndDate.GetDtStr().CStr(), TrainRecs);
      fprintf(fLog, "Testing  From %s   To %s\n",
       StartPredDate.GetDtStr().CStr(), EndPredDate.GetDtStr().CStr());
    }
    if (double(TrainRecs)<double(TrainWnLen)*0.5){continue;}
    // create model
    PGasMd GasMd=TGasMd::New(gmtEnsemble, GasMdMType, GasMdParamStr, GasDm, StartDate, EndDate, -1, -1, MnCspt);
    if (PrintToScrP){GasMd->Print();}
    if (LogVerbLev>1){
      fprintf(fLog, ".........................................\n");}
    // create absolute & relatve error statistics
    PMom PredAbsErrMom=TMom::New();
    PMom PredRelErrMom=TMom::New();
    // create test-class-value statistics
    PMom TestClassValMom=TMom::New();
    // extract records for prediction
    TIntV PredRecNV;
    GasDm->GetRecNV(StartPredDate, EndPredDate, -1, -1, MnCspt, PredRecNV);
    // predict class values for extracted records
    for (int PredRecNN=0; PredRecNN<PredRecNV.Len(); PredRecNN++){
      int RecN=PredRecNV[PredRecNN];
      // get date & hour
      TSecTm Date=GasDm->GetDateVal(RecN);
      TStr DateStr=Date.GetDtStr();
      int HourN=GasDm->GetAttrVal(RecN, "Hour", -1);
      // get true & predicted class value
      double ClassVal=GasDm->GetClassVal(RecN);
      //double PredClassVal=GasMd->GetPredClassVal(GasDm, RecN);
      double PredClassVal=GasMd->GetCorPredClassVal(GasDm, RecN, 1.5);
      // correction
      if (UseMdCorP){
        double CorFact=GasMdCor->GetCorFact(Date, GasDm->GetProdNm());
        PredClassVal=CorFact*PredClassVal;
      }
      // calculate absolute & relative error
      double RawPredAbsErr=ClassVal-PredClassVal;
      double PredAbsErr=fabs(ClassVal-PredClassVal);
      double PredRelErr=0; double RawPredRelErr=0;
      if (ClassVal!=0){
        RawPredRelErr=(ClassVal-PredClassVal)/ClassVal;
        PredRelErr=fabs(RawPredRelErr);
      }
      // add errors to statistics
      PredAbsErrMom->Add(PredAbsErr); AllPredAbsErrMom->Add(PredAbsErr);
      PredRelErrMom->Add(PredRelErr); AllPredRelErrMom->Add(PredRelErr);
      // add test-class-value to statistics
      TestClassValMom->Add(ClassVal);
      // create & save prediction object
      PGasPred GasPred=TGasPred::New();
      GasPred->CorrVal=ClassVal;
      GasPred->PredVal=PredClassVal;
      GasPred->AbsErr=RawPredAbsErr;
      GasPred->RelErr=RawPredRelErr;
      GasPred->Tm=GasDm->GetDateVal(RecN);
      GasPred->OutlierP=(PredRelErr>MnOutlierRelErr);
      GasPred->DescStr=GasDm->GetBitsStr(RecN);
      GasPredV.Add(GasPred);
      // save prediction to log
      if (LogVerbLev>1){
        fprintf(fLog,
         "Correct: %g   Predicted: %g   Abs-Error: %g   Rel-Error: %g   Date: %s   Hour: %d",
         ClassVal, PredClassVal, RawPredAbsErr, PredRelErr, DateStr.CStr(), HourN);
        fprintf(fLog, "   Desc: {%s}", GasDm->GetBitsStr(RecN).CStr());
        // outlier
        if (PredRelErr>MnOutlierRelErr){fprintf(fLog, "   Outlier");}
        fprintf(fLog, "\n");
      }
      // save prediction to olap
      if (fOlap!=NULL){
        fprintf(fOlap, "%s%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
         OlapPfxLn.CStr(),
         Date.GetDtMdyStr().CStr(),
         TTmInfo::GetMonthNm(Date.GetMonthN()).CStr(),
         TTmInfo::GetDayOfWeekNm(Date.GetDayOfWeekN()).CStr(),
         TFlt::GetStr(ClassVal).CStr(),
         TFlt::GetStr(PredClassVal).CStr(),
         TFlt::GetStr(RawPredAbsErr).CStr(),
         TFlt::GetStr(PredRelErr).CStr());
      }
    }
    // finish error statistics
    PredAbsErrMom->Def();
    PredRelErrMom->Def();
    // finish test class statistics
    TestClassValMom->Def();
    // save test window statistics to log
    if (LogVerbLev>1){
      fprintf(fLog, ".........................................\n");}
    // save class distribution to log
    if (LogVerbLev>0){
      fprintf(fLog, "Train Class Stat.: %s\n", GasMd->GetClassValMom()->GetStr(' ', ':', true).CStr());
      fprintf(fLog, "Test Class Stat.: %s\n", TestClassValMom->GetStr(' ', ':', true).CStr());
      fprintf(fLog, ".........................................\n");
      fprintf(fLog, "Abs-Error: %s\n", PredAbsErrMom->GetStr(' ', ':', true).CStr());
      fprintf(fLog, "Rel-Error: %s\n", PredRelErrMom->GetStr(' ', ':', true).CStr());
      fprintf(fLog, "---End-Window----------------------------\n");
    }
    // output results for the model
    if (PrintToScrP){
      printf("PredAbsErr: %s\n", PredAbsErrMom->GetStr(' ', ':', true).CStr());
      printf("PredRelErr: %s\n", PredRelErrMom->GetStr(' ', ':', true).CStr());
      printf("Model for: %s - %s ; Predict for: %s - %s\n",
       StartDate.GetDtStr().CStr(), EndDate.GetDtStr().CStr(),
       StartPredDate.GetDtStr().CStr(), EndPredDate.GetDtStr().CStr());
    }
  }
  // finish global error statistics
  AllPredAbsErrMom->Def();
  AllPredRelErrMom->Def();
  // save evaluation footer to log
  if (LogVerbLev>0){
    fprintf(fLog, "=========================================\n");}
  fprintf(fLog, "Abs-Error: %s\n", AllPredAbsErrMom->GetStr(' ', ':', true).CStr());
  fprintf(fLog, "Rel-Error: %s\n", AllPredRelErrMom->GetStr(' ', ':', true).CStr());
  fprintf(fLog, "Evaluation (%s)\n", EvalStampTm.GetStr().CStr());
  fprintf(fLog, "===End-Evaluation========================\n");
  // tab-separated log-file
  fprintf(fTabLog, "%s\t", GasDm->GetLtpNm().CStr());
  fprintf(fTabLog, "%s\t", GasDm->GetLocNm().CStr());
  fprintf(fTabLog, "%s\t", GasDm->GetProdNm().CStr());
  fprintf(fTabLog, "%s\t", GasDm->GetTypeNm().CStr());
  fprintf(fTabLog, "%d\t", TrainWnLen);
  fprintf(fTabLog, "%d\t", TestWnLen);
  fprintf(fTabLog, "%s\t", GasMdMType->GetDescStr().CStr());
  fprintf(fTabLog, "%s\t", AllPredAbsErrMom->TMom::GetValVStr().CStr());
  fprintf(fTabLog, "%s\n", AllPredRelErrMom->TMom::GetValVStr().CStr());
}

void TGasExe::GenDayOrder(
 const TStr& LogFNm,
 const PGasStation& GasStation, const TStr& OutGasStationFNm,
 const TSecTm& StartDt, const int& ProcessDays,
 const PGasDm& GasDm,
 const PGasMdMType& GasMdMType, const TStr& GasMdParamStr,
 TGasSimRecV& GasSimRecV){
  // statistics
  PMom TodayPredVolMom=TMom::New();
  PMom TodayRealVolMom=TMom::New();
  int UnderSafetyLevWarns=0;
  int ReschOrdDtWarns=0;

  // log-file
  TFOut LogSOut(LogFNm); FILE* fLog=LogSOut.GetFileId();
  GasStation->DumpDef(fLog);

  // set first-day record-number
  if (!StartDt.IsDef()){
    TExcept::Throw("Invalid start date.");}
  int FirstRecN=0;
  while (FirstRecN<GasDm->GetRecs()){
    TSecTm CurRecDt=GasDm->GetDateVal(FirstRecN);
    if (CurRecDt==StartDt){break;}
    FirstRecN++;
  }
  if (FirstRecN==GasDm->GetRecs()){
    TSecTm CurRecDt=GasDm->GetDateVal(GasDm->GetRecs()-1);
    CurRecDt.AddDays(1);
    if (CurRecDt!=StartDt){
      TExcept::Throw("Invalid start date.", StartDt.GetDtStr());}
  }

  // create simulation-records-vector
  GasSimRecV.Clr();

  // process days
  for (int TodayRecN=FirstRecN; TodayRecN<FirstRecN+ProcessDays-1; TodayRecN++){
    GasDm->AssureDayRec(TodayRecN);
    TSecTm TodayDt=GasDm->GetDateVal(TodayRecN);
    TStr TodayDtStr=TodayDt.GetDtStr();
    fprintf(fLog, "Start day at record [%d] for date [%s]\n", TodayRecN, TodayDtStr.CStr());
    fprintf(fLog, "\tDay-Info: [%s]\n", GasDm->GetBitsStr(TodayRecN).CStr());

    // check if records are continuos
    if (TodayRecN>FirstRecN){
      TSecTm PrevDt=GasDm->GetDateVal(TodayRecN-1);
      EAssertRA(TodayDt==TSecTm(PrevDt).AddDays(1),
       "Invalid time grid", TodayDtStr);
    }

    // create simulation record
    PGasSimRec GasSimRec=TGasSimRec::New(GasStation, TodayDtStr);

    // create model
    int MnMdRecN=TodayRecN-GasStation->HistWnDays;
    int MxMdRecN=TodayRecN-1;
    PGasMd GasMd=TGasMd::New(
     gmtEnsemble, GasMdMType, GasMdParamStr,
     GasDm,
     GasDm->GetDateVal(MnMdRecN), GasDm->GetDateVal(MxMdRecN), -1, -1,
     100);
    fprintf(fLog, "\tCreate Model for %d records (from %d to %d)\n",
     MxMdRecN-MnMdRecN+1, MnMdRecN, MxMdRecN);

    // execute orders
    double OrderQuantity=TGasOrder::GetQuantity(TodayDt, GasStation->GasOrderV);
    fprintf(fLog, "\tExecuted Orders for Quantity: %.0f\n", OrderQuantity);

    // predict consumption
    double TodayPredCspt=GasMd->GetPredClassVal(GasDm, TodayRecN);
    GasDm->PutCsptIfEmpty(TodayRecN, TodayPredCspt);
    fprintf(fLog, "\tPredicted consumption: %.0f\n", TodayPredCspt);

    // get real consumption
    double TodayRealCspt=GasDm->GetClassVal(TodayRecN);
    fprintf(fLog, "\tReal consumption: %.0f\n", TodayRealCspt);

    // predicted consumption statistics
    double TodayPredOpenVol=GasStation->VolumeVal+OrderQuantity;
    double TodayPredCloseVol=TodayPredOpenVol-TodayRealCspt;
    if ((TodayPredOpenVol!=-1)&&(TodayPredCloseVol!=-1)){
      TodayPredVolMom->Add((TodayPredOpenVol+TodayPredCloseVol)/2);}

    // real consumption statistics
    double TodayOpenRealVol=GasDm->GetVolVal(TodayRecN);
    double TodayCloseRealVol=-1;
    if (TodayRecN+1<GasDm->GetRecs()){
      TodayCloseRealVol=GasDm->GetVolVal(TodayRecN+1);}
    if ((TodayOpenRealVol!=-1)&&(TodayCloseRealVol!=-1)){
      TodayRealVolMom->Add((TodayOpenRealVol+TodayCloseRealVol)/2);}

    // create order if neeed in simulated future
    double SimVolumeVal=GasStation->VolumeVal;
    int SimTodayRecN=TodayRecN;
    int SimDays=GasStation->StockDurationDays+GasStation->DeliveryDays;
    fprintf(fLog, "\tSimulation to generate orders for %d (%d+%d) days.\n",
     SimDays, GasStation->StockDurationDays, GasStation->DeliveryDays);

    // update simulation record
    GasSimRec->PutReal(TodayPredOpenVol, TodayRealCspt, OrderQuantity);

    // simulation for SimDays days
    while (SimTodayRecN-TodayRecN<SimDays){
      // set time
      GasDm->AssureDayRec(SimTodayRecN);
      TSecTm SimTodayDt=GasDm->GetDateVal(SimTodayRecN);
      TStr SimTodayDtStr=SimTodayDt.GetDtStr();
      // execute orders
      double SimOrderQuantity=
       TGasOrder::GetQuantity(SimTodayDt, GasStation->GasOrderV);
      // predict simulated consumption
      double SimPredCspt=GasMd->GetCorPredClassVal(
       GasDm, SimTodayRecN, GasStation->ExceptPredCorFact);
      GasDm->PutCsptIfEmpty(SimTodayRecN, SimPredCspt);
      // update simulation record
      GasSimRec->AddSimStep(SimTodayDtStr,
       SimVolumeVal-SimPredCspt+SimOrderQuantity,
       SimVolumeVal, SimPredCspt, SimOrderQuantity);
      // output simulated-volume
      fprintf(fLog, "\t\tSimVolume: %.0f = (v)%.0f - (c)%.0f + (o)%.0f (Date:%s) (Day:%d)\n",
       SimVolumeVal-SimPredCspt+SimOrderQuantity,
       SimVolumeVal, SimPredCspt, SimOrderQuantity,
       SimTodayDtStr.CStr(), SimTodayRecN-TodayRecN);
      // if simulated-volume under safety-level
      if (SimVolumeVal-SimPredCspt+SimOrderQuantity>=GasStation->SafetyVolumeVal){
        // update simulated volume
        SimVolumeVal=SimVolumeVal+SimOrderQuantity-SimPredCspt;
        // increment simulated day
        SimTodayRecN++;
      } else {
        fprintf(fLog, "\t\tUnder safety level - calculate order quantity for %d days\n",
         GasStation->StockDurationDays);
        // get order-quantity lasting simulated StockDurationDays days
        double SubOrderQuantity=0;
        for (int DurDayN=0; DurDayN<GasStation->StockDurationDays; DurDayN++){
          GasDm->AssureDayRec(SimTodayRecN+DurDayN);
          double SubSimPredCspt=0;
          if (DurDayN==0){
            SubSimPredCspt=GasStation->SafetyVolumeVal-
             (SimVolumeVal-SimPredCspt+SimOrderQuantity);
          } else {
            SubSimPredCspt=GasMd->GetCorPredClassVal(
             GasDm, SimTodayRecN+DurDayN, GasStation->ExceptPredCorFact);
          }
          GasDm->PutCsptIfEmpty(SimTodayRecN+DurDayN, SubSimPredCspt);
          // update simulation record
          GasSimRec->AddOrdStep(
           GasDm->GetDateVal(SimTodayRecN+DurDayN).GetDtStr(),
           SubOrderQuantity+SubSimPredCspt, SubOrderQuantity, SubSimPredCspt);
          // output ordered-quantity
          fprintf(fLog, "\t\t\tOrderQuantity: %.0f = (o)%.0f + (c)%.0f (Date:%s) (Day:%d)\n",
           SubOrderQuantity+SubSimPredCspt, SubOrderQuantity, SubSimPredCspt,
           GasDm->GetDateVal(SimTodayRecN+DurDayN).GetDtStr().CStr(), DurDayN);
          SubOrderQuantity+=SubSimPredCspt;
        }
        // update simulation record
        GasSimRec->PutCorrQuant(
         SubOrderQuantity*GasStation->OrderQuantityCorFact);
          // output ordered-quantity
        fprintf(fLog, "\t\t\tOrderQuantity: %.0f = %.0f * %.2f (Correction)\n",
         SubOrderQuantity*GasStation->OrderQuantityCorFact,
         SubOrderQuantity, GasStation->OrderQuantityCorFact);
        SubOrderQuantity*=GasStation->OrderQuantityCorFact;
        // get order issue & receive date
        int OrderIssueDayRecN=SimTodayRecN-GasStation->DeliveryDays;
        if (OrderIssueDayRecN<0){OrderIssueDayRecN=0;}
        if (OrderIssueDayRecN<TodayRecN){
          fprintf(fLog, "\t\tReschedule Order: [%s] to [%s] !!!\n",
           GasDm->GetDateVal(OrderIssueDayRecN).GetDtStr().CStr(),
           GasDm->GetDateVal(TodayRecN).GetDtStr().CStr());
          OrderIssueDayRecN=TodayRecN;
          ReschOrdDtWarns++;
        }
        TSecTm OrderIssueDt=GasDm->GetDateVal(OrderIssueDayRecN);
        TSecTm OrderReceiveDt=
         GasDm->GetDateVal(OrderIssueDayRecN+GasStation->DeliveryDays);
        // delete unapproved orders where receive-date>current-receive-date
        TStrV MsgStrV;
        TGasOrder::DelUnapprovedOrders(OrderReceiveDt, GasStation->GasOrderV, MsgStrV);
        TGasOrder::DelUnapprovedOrders(OrderReceiveDt, GasStation->NewGasOrderV, MsgStrV);
        for (int MsgStrN=0; MsgStrN<MsgStrV.Len(); MsgStrN++){
          fprintf(fLog, "\t\tDelete-Order: %s\n", MsgStrV[MsgStrN].CStr());}
        // create order
        PGasOrder GasOrder=
         TGasOrder::New(OrderIssueDt, OrderReceiveDt, SubOrderQuantity, false);
        GasStation->GasOrderV.AddSorted(GasOrder);
        GasStation->NewGasOrderV.AddSorted(GasOrder);
        fprintf(fLog, "\t\tCreate-Order: %s\n", GasOrder->GetStr().CStr());
        break;
      }
    }

    // calculate new volume
    fprintf(fLog, "\tNewVolume: %.0f = (v)%.0f + (o)%.0f - (c)%.0f\n",
     GasStation->VolumeVal+OrderQuantity-TodayRealCspt,
     GasStation->VolumeVal, OrderQuantity, TodayRealCspt);
    GasStation->VolumeVal=GasStation->VolumeVal+OrderQuantity-TodayRealCspt;
    if (GasStation->VolumeVal<GasStation->SafetyVolumeVal){
      UnderSafetyLevWarns++;
      fprintf(fLog, "\tUnder safety level: %.0f<%.0f !!!\n",
       GasStation->VolumeVal, GasStation->SafetyVolumeVal);
    }

    // delete expired orders
    TGasOrder::DelOrders(TodayDt, GasStation->GasOrderV);

    // save simulation-records
    GasSimRecV.Add(GasSimRec);

    // close
    fprintf(fLog, "Close day at record [%d] for date [%s]\n", TodayRecN, TodayDtStr.CStr());
  }
  // output gas-station data
  GasStation->SaveXml(OutGasStationFNm);
  GasStation->DumpDef(fLog);

  // output statistics
  TodayPredVolMom->Def(); TodayRealVolMom->Def();
  fprintf(fLog, "===Statistics-Start======================\n");
  fprintf(fLog, "Predicted-Volume-Statistics: %s\n", TodayPredVolMom->GetStr().CStr());
  fprintf(fLog, "     Real-Volume-Statistics: %s\n", TodayRealVolMom->GetStr().CStr());
  fprintf(fLog, "Under-Safety-Level-Warnings: %d\n", UnderSafetyLevWarns);
  fprintf(fLog, "  Reschedule-Order-Warnings: %d\n", ReschOrdDtWarns);
  fprintf(fLog, "===Statistics-End========================\n");
}

void TGasExe::GenHourBlockOrder(
 const TStr& LogFNm,
 const PGasStation& GasStation, const TStr& OutGasStationFNm,
 const TSecTm& StartDt, const int& ProcessDays,
 const PGasDm& GasDm,
 const PGasMdMType& GasMdMType, const TStr& GasMdParamStr,
 TGasSimRecV& GasSimRecV){
  // shortcuts
  int TmGridHours=GasStation->TmGridHours;

  // statistics
  PMom NowPredVolMom=TMom::New();
  PMom NowRealVolMom=TMom::New();
  int UnderSafetyLevWarns=0;
  int ReschOrdDtWarns=0;

  // log-file
  TFOut LogSOut(LogFNm); FILE* fLog=LogSOut.GetFileId();
  GasStation->DumpDef(fLog);

  // set first-day record-number
  if (!StartDt.IsDef()){
    TExcept::Throw("Invalid start date.");}
  int FirstRecN=0;
  while (FirstRecN<GasDm->GetRecs()){
    TSecTm CurRecDt=GasDm->GetDateVal(FirstRecN);
    if (CurRecDt==StartDt){break;}
    FirstRecN++;
  }
  if (FirstRecN==GasDm->GetRecs()){
    TSecTm CurRecDt=GasDm->GetDateVal(GasDm->GetRecs()-1);
    CurRecDt.AddDays(1);
    if (CurRecDt!=StartDt){
      TExcept::Throw("Invalid start date.", StartDt.GetDtStr());}
  }

  // create simulation-records-vector
  GasSimRecV.Clr();

  // process days
  int DayRecs=24/TmGridHours;
  int ProcessRecs=ProcessDays*DayRecs;
  for (int NowRecN=FirstRecN; NowRecN<FirstRecN+ProcessRecs; NowRecN++){
    GasDm->AssureDayRec(NowRecN);
    TSecTm NowDt=GasDm->GetDateVal(NowRecN);
    int NowHourN=GasDm->GetHourN(NowRecN);
    TStr NowDtTmStr=NowDt.GetDtStr()+"/"+TInt::GetStr(NowHourN);
    TSecTm NowStartTm=TSecTm(NowDt).AddHours(NowHourN);
    TSecTm NowEndTm=TSecTm(NowStartTm).AddHours(TmGridHours);
    fprintf(fLog, "Start day at record [%d] for date/hour [%s]\n", NowRecN, NowDtTmStr.CStr());
    fprintf(fLog, "\tFrom [%s] To [%s]\n", NowStartTm.GetStr().CStr(), NowEndTm.GetStr().CStr());
    fprintf(fLog, "\tDay-Info: [%s]\n", GasDm->GetBitsStr(NowRecN).CStr());

    // check if records are continuos
    if (NowRecN>FirstRecN){
      TSecTm PrevDt=GasDm->GetDateVal(NowRecN-1);
      int PrevHourN=GasDm->GetHourN(NowRecN-1);
      if (NowDt==PrevDt){
        EAssertRA(NowHourN-PrevHourN==TmGridHours, "Invalid time grid", NowDtTmStr);
      } else
      if (NowDt==TSecTm(PrevDt).AddDays(1)){
        EAssertRA(NowHourN==0, "Invalid time grid", NowDtTmStr);
      }
    }

    // create simulation record
    PGasSimRec GasSimRec=TGasSimRec::New(GasStation, NowDtTmStr);

    // create model
    int MnMdRecN=NowRecN-GasStation->HistWnDays*DayRecs;
    int MxMdRecN=NowRecN-1;
    PGasMd GasMd=TGasMd::New(
     gmtEnsemble, GasMdMType, GasMdParamStr,
     GasDm,
     TSecTm(), TSecTm(), MnMdRecN, MxMdRecN,
     100);
    fprintf(fLog, "\tCreate Model for %d records (from %d to %d)\n",
     MxMdRecN-MnMdRecN+1, MnMdRecN, MxMdRecN);

    // execute orders
    double OrderQuantity=
     TGasOrder::GetQuantity(NowStartTm, NowEndTm, GasStation->GasOrderV);
    fprintf(fLog, "\tExecuted Orders for Quantity: %.0f\n", OrderQuantity);

    // predict consumption
    double NowPredCspt=GasMd->GetPredClassVal(GasDm, NowRecN);
    if (NowPredCspt<0){NowPredCspt=0;}
    GasDm->PutCsptIfEmpty(NowRecN, NowPredCspt);
    fprintf(fLog, "\tPredicted consumption: %.0f\n", NowPredCspt);

    // get real consumption
    double NowRealCspt=GasDm->GetClassVal(NowRecN);
    fprintf(fLog, "\tReal consumption: %.0f\n", NowRealCspt);

    // predicted consumption statistics
    double NowPredOpenVol=GasStation->VolumeVal+OrderQuantity;
    double NowPredCloseVol=NowPredOpenVol-NowRealCspt;
    if ((NowPredOpenVol!=-1)&&(NowPredCloseVol!=-1)){
      NowPredVolMom->Add((NowPredOpenVol+NowPredCloseVol)/2);}

    // real consumption statistics
    double NowOpenRealVol=GasDm->GetVolVal(NowRecN);
    double NowCloseRealVol=-1;
    if (NowRecN+1<GasDm->GetRecs()){
      NowCloseRealVol=GasDm->GetVolVal(NowRecN+1);}
    if ((NowOpenRealVol!=-1)&&(NowCloseRealVol!=-1)){
      NowRealVolMom->Add((NowOpenRealVol+NowCloseRealVol)/2);}

    // create order if neeed in simulated future
    double SimVolumeVal=GasStation->VolumeVal;
    int SimNowRecN=NowRecN;
    int SimDays=GasStation->StockDurationDays+GasStation->DeliveryDays;
    int SimRecs=SimDays*DayRecs;
    fprintf(fLog, "\tSimulation to generate orders for %d (%d+%d) days (%d records).\n",
     SimDays, GasStation->StockDurationDays, GasStation->DeliveryDays, SimRecs);

    // update simulation record
    GasSimRec->PutReal(NowPredOpenVol, NowRealCspt, OrderQuantity);

    // simulation for SimRecs records
    while (SimNowRecN-NowRecN<SimRecs){
      // set time
      GasDm->AssureDayRec(SimNowRecN);
      TSecTm SimNowDt=GasDm->GetDateVal(SimNowRecN);
      int SimNowHourN=GasDm->GetHourN(SimNowRecN);
      TStr SimNowDtTmStr=SimNowDt.GetDtStr()+"/"+TInt::GetStr(SimNowHourN);
      TSecTm SimNowStartTm=TSecTm(SimNowDt).AddHours(SimNowHourN);
      TSecTm SimNowEndTm=TSecTm(SimNowStartTm).AddHours(TmGridHours);
      // execute orders
      double SimOrderQuantity=
       TGasOrder::GetQuantity(SimNowStartTm, SimNowEndTm, GasStation->GasOrderV);
      // predict simulated consumption
      double SimPredCspt=GasMd->GetCorPredClassVal(
       GasDm, SimNowRecN, GasStation->ExceptPredCorFact);
      GasDm->PutCsptIfEmpty(SimNowRecN, SimPredCspt);
      // update simulation record
      GasSimRec->AddSimStep(SimNowDtTmStr,
       SimVolumeVal-SimPredCspt+SimOrderQuantity,
       SimVolumeVal, SimPredCspt, SimOrderQuantity);
      // output simulated-volume
      fprintf(fLog, "\t\tSimVolume: %.0f = (v)%.0f - (c)%.0f + (o)%.0f (Date/Hour:%s) (Step:%d)\n",
       SimVolumeVal-SimPredCspt+SimOrderQuantity,
       SimVolumeVal, SimPredCspt, SimOrderQuantity,
       SimNowDtTmStr.CStr(), SimNowRecN-NowRecN);
      // if simulated-volume under safety-level
      if (SimVolumeVal-SimPredCspt+SimOrderQuantity>=GasStation->SafetyVolumeVal){
        // update simulated volume
        SimVolumeVal=SimVolumeVal+SimOrderQuantity-SimPredCspt;
        // increment simulated day
        SimNowRecN++;
      } else {
        fprintf(fLog, "\t\tUnder safety level - calculate order quantity for %d days\n",
         GasStation->StockDurationDays);
        // get order-quantity lasting simulated StockDurationDays days
        double SubOrderQuantity=0;
        int DurRecs=GasStation->StockDurationDays*DayRecs;
        for (int DurRecN=0; DurRecN<DurRecs; DurRecN++){
          GasDm->AssureDayRec(SimNowRecN+DurRecN);
          TSecTm DurDt=GasDm->GetDateVal(SimNowRecN+DurRecN);
          int DurHourN=GasDm->GetHourN(SimNowRecN+DurRecN);
          TStr DurDtTmStr=DurDt.GetDtStr()+"/"+TInt::GetStr(DurHourN);
          double SubSimPredCspt=0;
          if (DurRecN==0){
            SubSimPredCspt=GasStation->SafetyVolumeVal-
             (SimVolumeVal-SimPredCspt+SimOrderQuantity);
          } else {
            SubSimPredCspt=GasMd->GetCorPredClassVal(
             GasDm, SimNowRecN+DurRecN, GasStation->ExceptPredCorFact);
          }
          GasDm->PutCsptIfEmpty(SimNowRecN+DurRecN, SubSimPredCspt);
          // update simulation record
          GasSimRec->AddOrdStep(
           DurDtTmStr,
           SubOrderQuantity+SubSimPredCspt, SubOrderQuantity, SubSimPredCspt);
          // output ordered-quantity
          fprintf(fLog, "\t\t\tOrderQuantity: %.0f = (o)%.0f + (c)%.0f (Date:%s) (Rec:%d)\n",
           SubOrderQuantity+SubSimPredCspt, SubOrderQuantity, SubSimPredCspt,
           DurDtTmStr.CStr(), DurRecN);
          SubOrderQuantity+=SubSimPredCspt;
        }
        // update simulation record
        GasSimRec->PutCorrQuant(
         SubOrderQuantity*GasStation->OrderQuantityCorFact);
          // output ordered-quantity
        fprintf(fLog, "\t\t\tOrderQuantity: %.0f = %.0f * %.2f (Correction)\n",
         SubOrderQuantity*GasStation->OrderQuantityCorFact,
         SubOrderQuantity, GasStation->OrderQuantityCorFact);
        SubOrderQuantity*=GasStation->OrderQuantityCorFact;
        // get order issue & receive date
        int OrderIssueRecN=SimNowRecN-GasStation->DeliveryDays*DayRecs;
        if (OrderIssueRecN<0){OrderIssueRecN=0;}
        if (OrderIssueRecN<NowRecN){
          fprintf(fLog, "\t\tReschedule Order: [%s] to [%s] !!!\n",
           GasDm->GetDateVal(OrderIssueRecN).GetDtStr().CStr(),
           GasDm->GetDateVal(NowRecN).GetDtStr().CStr());
          OrderIssueRecN=NowRecN;
          ReschOrdDtWarns++;
        }
        // create order time
        TSecTm OrderIssueTm=GasDm->GetDateVal(OrderIssueRecN);
        int OrderIssueHourN=GasDm->GetHourN(OrderIssueRecN);
        OrderIssueTm.AddHours(OrderIssueHourN);
        // create receive time
        int DeliveryRecs=GasStation->DeliveryDays*DayRecs;
        TSecTm OrderReceiveTm=GasDm->GetDateVal(OrderIssueRecN+DeliveryRecs);
        int OrderReceiveHourN=GasDm->GetHourN(OrderIssueRecN+DeliveryRecs);
        OrderReceiveTm.AddHours(OrderReceiveHourN);
        // delete unapproved orders where receive-date>current-receive-date
        TStrV MsgStrV;
        TGasOrder::DelUnapprovedOrders(OrderReceiveTm, GasStation->GasOrderV, MsgStrV);
        TGasOrder::DelUnapprovedOrders(OrderReceiveTm, GasStation->NewGasOrderV, MsgStrV);
        for (int MsgStrN=0; MsgStrN<MsgStrV.Len(); MsgStrN++){
          fprintf(fLog, "\t\tDelete-Order: %s\n", MsgStrV[MsgStrN].CStr());}
        // create order
        PGasOrder GasOrder=
         TGasOrder::New(OrderIssueTm, OrderReceiveTm, SubOrderQuantity, false);
        GasStation->GasOrderV.AddSorted(GasOrder);
        GasStation->NewGasOrderV.AddSorted(GasOrder);
        fprintf(fLog, "\t\tCreate-Order: %s\n", GasOrder->GetStr().CStr());
        break;
      }
    }

    // calculate new volume
    fprintf(fLog, "\tNewVolume: %.0f = (v)%.0f + (o)%.0f - (c)%.0f\n",
     GasStation->VolumeVal+OrderQuantity-NowRealCspt,
     GasStation->VolumeVal, OrderQuantity, NowRealCspt);
    GasStation->VolumeVal=GasStation->VolumeVal+OrderQuantity-NowRealCspt;
    if (GasStation->VolumeVal<GasStation->SafetyVolumeVal){
      UnderSafetyLevWarns++;
      fprintf(fLog, "\tUnder safety level: %.0f<%.0f !!!\n",
       GasStation->VolumeVal, GasStation->SafetyVolumeVal);
    }

    // delete expired orders
    TGasOrder::DelOrders(NowStartTm, NowEndTm, GasStation->GasOrderV);

    // save simulation-records
    GasSimRecV.Add(GasSimRec);

    // close
    fprintf(fLog, "Close day at record [%d] for date/hour [%s]\n", NowRecN, NowDtTmStr.CStr());
  }
  // output gas-station data
  GasStation->SaveXml(OutGasStationFNm);
  GasStation->DumpDef(fLog);

  // output statistics
  NowPredVolMom->Def(); NowRealVolMom->Def();
  fprintf(fLog, "===Statistics-Start======================\n");
  fprintf(fLog, "Predicted-Volume-Statistics: %s\n", NowPredVolMom->GetStr().CStr());
  fprintf(fLog, "     Real-Volume-Statistics: %s\n", NowRealVolMom->GetStr().CStr());
  fprintf(fLog, "Under-Safety-Level-Warnings: %d\n", UnderSafetyLevWarns);
  fprintf(fLog, "  Reschedule-Order-Warnings: %d\n", ReschOrdDtWarns);
  fprintf(fLog, "===Statistics-End========================\n");
}

int TGasExe::GenPrediction(
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
 const double& ExceptPredCorFact){
  Try;
  TStr LtpFNm=_LtpFNm;
  if ((!TabFNm.Empty())&&(LtpFNm.Empty())){
    LtpFNm=TStr::PutFExt(TabFNm, ".Ltp");}
  TSecTm PredDate=TSecTm::GetDtTmFromDmyStr(PredDateStr);

  // log file
  TFOut LogFOut(OutLogFNm); FILE* fLog=LogFOut.GetFileId();

  // convert tabular to ltp data
  if (!TabFNm.Empty()){
    TGasLtpBs::ConvTabToLtp(TabFNm, LtpFPath, RepairP);}

  // load ltp data
  PGasLtpBs GasLtpBs=TGasLtpBs::Load(LtpFNm);

  // create model type
  PGasMdMType GasMdMType;
  if (MdNm=="ctb"){GasMdMType=TGasMdMType::New(gmtCTb);}
  else if (MdNm=="svd"){GasMdMType=TGasMdMType::New(gmtLr);}
  else if (MdNm=="nnbr"){GasMdMType=TGasMdMType::New(gmtNNbr);}
  else if (MdNm=="svm"){GasMdMType=TGasMdMType::New(gmtSvm);}
  else if (MdNm=="cubist"){GasMdMType=TGasMdMType::New(gmtCubist);}
  else if (MdNm=="rtree"){GasMdMType=TGasMdMType::New(gmtRegTree);}
  else if (MdNm=="ens"){
    GasMdMType=TGasMdMType::New();
    GasMdMType->AddMdTypeWgt(gmtLr, 40);
    GasMdMType->AddMdTypeWgt(gmtNNbr, 30);
    GasMdMType->AddMdTypeWgt(gmtCTb, 30);
    GasMdMType->NrmMdWgt();
  } else {
    TExcept::Throw("Invalid model type", MdNm);
  }

  // create domain
  PGasDm GasDm;
  if (TmResNm=="w"){GasDm=TGasDm::GetWeekGasDm(GasLtpBs);}
  else if (TmResNm=="d"){GasDm=TGasDm::GetDayGasDm(GasLtpBs);}
  else if (TmResNm=="24h"){GasDm=TGasDm::GetHourBlockGasDm(GasLtpBs, 24);}
  else if (TmResNm=="12h"){GasDm=TGasDm::GetHourBlockGasDm(GasLtpBs, 12);}
  else if (TmResNm=="6h"){GasDm=TGasDm::GetHourBlockGasDm(GasLtpBs, 6);}
  else if (TmResNm=="4h"){GasDm=TGasDm::GetHourBlockGasDm(GasLtpBs, 4);}
  else if (TmResNm=="3h"){GasDm=TGasDm::GetHourBlockGasDm(GasLtpBs, 3);}
  else if (TmResNm=="2h"){GasDm=TGasDm::GetHourBlockGasDm(GasLtpBs, 2);}
  else if (TmResNm=="1h"){GasDm=TGasDm::GetHourBlockGasDm(GasLtpBs, 1);}
  else {TExcept::Throw("Invalid time resolution", TmResNm);}

  // determine min. & max. Ltp dates
  TSecTm MnLtpDate=GasLtpBs->GetMnDate();
  TSecTm MxLtpDate=GasLtpBs->GetMxDate();
  fprintf(fLog, "Minimal-Possible-Train-Date: %s\n", MnLtpDate.GetDtStr().CStr());
  fprintf(fLog, "Maximal-Possible-Train-Date: %s\n", MxLtpDate.GetDtStr().CStr());

  // determine start & end & train & predict dates
  if (PredDateStr.Empty()){
    PredDate=TSecTm(MxLtpDate).AddDays(1);}
  TSecTm StartPredDate=PredDate;
  TSecTm EndPredDate=TSecTm(StartPredDate).AddDays(PredDays-1);
  TSecTm StartTrainDate=TSecTm(StartPredDate).AddDays(-TrainDays);
  TSecTm EndTrainDate=TSecTm(StartPredDate).AddDays(-1);
  fprintf(fLog, "Start-Train-Date: %s\n", StartTrainDate.GetDtStr().CStr());
  fprintf(fLog, "End-Train-Date: %s\n", EndTrainDate.GetDtStr().CStr());
  fprintf(fLog, "Start-Prediction-Date: %s\n", StartPredDate.GetDtStr().CStr());
  fprintf(fLog, "End-Prediction-Date: %s\n", EndPredDate.GetDtStr().CStr());
  if ((StartTrainDate<MnLtpDate)||(MxLtpDate<StartTrainDate)){
    TExcept::Throw("Bad Start-Train-Date", StartTrainDate.GetStr());}
  if ((EndTrainDate<MnLtpDate)||(MxLtpDate<EndTrainDate)){
    TExcept::Throw("Bad End-Train-Date", EndTrainDate.GetStr());}
  //if ((StartPredDate<MnLtpDate)||(MxLtpDate<StartPredDate)){
  //  TExcept::Throw("Bad Start-Prediction-Date", StartPredDate.GetStr());}
  //if ((EndPredDate<MnLtpDate)||(MxLtpDate<EndPredDate)){
  //  TExcept::Throw("Bad End-Prediction-Date", EndPredDate.GetStr());}

  // determine train & prediction records
  TIntV TrainRecNV; TIntV SimPredRecNV;
  GasDm->GetRecNV(StartTrainDate, EndTrainDate, -1, -1, MnCspt, TrainRecNV);
  GasDm->GetRecNV(StartPredDate, EndPredDate, -1, -1, MnCspt, SimPredRecNV);
  int TrainRecs=TrainRecNV.Len(); int SimPredRecs=SimPredRecNV.Len();
  if (TrainRecs==0){
    TExcept::Throw("Empty Train Set");
  } else {
    fprintf(fLog, "Train Records: %d (from %d to %d)\n",
     TrainRecs, TrainRecNV[0], TrainRecNV.Last());
  }
  fprintf(fLog, "Simulated Prediction Records: %d\n", SimPredRecs);

  // create model
  PGasMd GasMd=TGasMd::New(
   gmtEnsemble, GasMdMType,
   TGasMd::GetMdParamStr(),
   GasDm,
   StartTrainDate, EndTrainDate, -1, -1,
   MnCspt);
  //GasMd->Print();

  // predict & save
  // save xml header
  {TFOut SOut(OutXmlFNm); FILE* fOut=SOut.GetFileId();
  fprintf(fOut, "<Predictions>\n");
  fprintf(fOut, "  <Location>%s</Location>\n",
   TXmlLx::GetXmlStrFromPlainStr(GasLtpBs->GetLocNm()).CStr());
  fprintf(fOut, "  <Product>%s</Product>\n",
   TXmlLx::GetXmlStrFromPlainStr(GasLtpBs->GetProdNm()).CStr());
  fprintf(fOut, "  <LtpCode>%s</LtpCode>\n",
   TXmlLx::GetXmlStrFromPlainStr(GasLtpBs->GetLtpNm()).CStr());

  // predict simulated consumption
  TSecTm CurPredDate=StartPredDate;
  for (int SimPredRecNN=0; SimPredRecNN<SimPredRecs; SimPredRecNN++){
    int PredRecN=SimPredRecNV[SimPredRecNN];
    // get date & hour
    CurPredDate=TSecTm::GetDtTm(GasDm->GetDateVal(PredRecN));
    TStr PredDateStr=GasDm->GetDateVal(PredRecN).GetDtStr();
    int HourN=GasDm->GetAttrVal(PredRecN, "Hour", -1);
    TStr PredTmStr;
    if (HourN==-1){
      fprintf(fLog, "Prediction date: %s (record %d)\n",
       PredDateStr.CStr(), PredRecN);
       PredTmStr=PredDateStr;
    } else {
      fprintf(fLog, "Prediction date: %s %d:00 (record %d)\n",
       PredDateStr.CStr(), HourN, PredRecN);
      PredTmStr=PredDateStr+" "+TInt::GetStr(HourN)+":00";
    }
    // get true & predicted class value
    double PredClassVal=GasMd->GetCorPredClassVal(GasDm, PredRecN, ExceptPredCorFact);
    double RealClassVal=GasDm->GetClassVal(PredRecN);
    fprintf(fLog, "Predicted Consumption: %.0f\n", PredClassVal);
    fprintf(fLog, "Real Consumption: %.0f\n", RealClassVal);
    // output prediction to xml
    fprintf(fOut, "  <Prediction Mode=\"Simulation\">\n");
    fprintf(fOut, "    <Time>%s</Time>\n", PredTmStr.CStr());
    fprintf(fOut, "    <Predicted>%.0f</Predicted>\n", PredClassVal);
    if (RealClassVal!=-1){
      fprintf(fOut, "    <Real>%.0f</Real>\n", RealClassVal);}
    fprintf(fOut, "  </Prediction>\n");
  }

  // predict future consumption
  while (CurPredDate<=EndPredDate){
    int PredRecN=GasDm->AddTmResRec();
    // get date & hour
    CurPredDate=TSecTm::GetDtTm(GasDm->GetDateVal(PredRecN));
    TStr PredDateStr=GasDm->GetDateVal(PredRecN).GetDtStr();
    int HourN=GasDm->GetAttrVal(PredRecN, "Hour", -1);
    TStr PredTmStr;
    if (HourN==-1){
      fprintf(fLog, "Prediction date: %s (record %d)\n",
       PredDateStr.CStr(), PredRecN);
       PredTmStr=PredDateStr;
    } else {
      fprintf(fLog, "Prediction date: %s %d:00 (record %d)\n",
       PredDateStr.CStr(), HourN, PredRecN);
      PredTmStr=PredDateStr+" "+TInt::GetStr(HourN)+":00";
    }
    // get true & predicted class value
    double PredClassVal=GasMd->GetCorPredClassVal(GasDm, PredRecN, ExceptPredCorFact);
    GasDm->PutCsptIfEmpty(PredRecN, PredClassVal);
    fprintf(fLog, "Predicted Consumption: %.0f\n", PredClassVal);
    // output prediction to xml
    fprintf(fOut, "  <Prediction Mode=\"Future\">\n");
    fprintf(fOut, "    <Time>%s</Time>\n", PredTmStr.CStr());
    fprintf(fOut, "    <Predicted>%.0f</Predicted>\n", PredClassVal);
    fprintf(fOut, "  </Prediction>\n");
  }

  // save xml footer
  fprintf(fOut, "</Predictions>\n");}

  return 0;
  Catch;
  return 1;
}

//PGasStation GetGasStation(const PGasDm& GasDm){
//  // gas station
//  PGasStation GasStation=TGasStation::New();
//  // definition data
//  GasStation->DescStr=GasDm->GetDescStr();
//  GasStation->ProdNm=GasDm->GetProdNm();
//  GasStation->FullVolumeVal=50000;
//  GasStation->SafetyVolumeVal=5000;
//  GasStation->DeliveryDays=1;
//  GasStation->StockDurationDays=2;
//  GasStation->OrderQuantityCorFact=1.2;
//  GasStation->ExceptPredCorFact=1.5;
//  GasStation->HistWnDays=100;
//  GasStation->TmGridHours=100;
//  // state data
//  GasStation->VolumeVal=20000;
//  GasStation->GasOrderV.Clr();
//  GasStation->NewGasOrderV.Clr();
//  // return results
//  return GasStation;
//}

int TGasExe::GenOrder(
 const TStr& TabFNm,
 const TStr& _LtpFNm,
 const TStr& InGasStationFNm,
 const TStr& OutGasStationFNm,
 const TStr& OutLogFNm,
 const TStr& OutSimFNm,
 const TStr& StartDateStr,
 const int& ProcessDays){
  Try;
  // shortcuts
  TStr LtpFNm=_LtpFNm;
  if ((!TabFNm.Empty())&&(LtpFNm.Empty())){
    LtpFNm=TStr::PutFExt(TabFNm, ".Ltp");}
  TStr LtpFPath=LtpFNm.GetFPath();
  TSecTm StartDt=TSecTm::GetDtTmFromDmyStr(StartDateStr);

  // create model type
  PGasMdMType GasMdMType=TGasMdMType::New();
  GasMdMType->AddMdTypeWgt(gmtLr, 40);
  GasMdMType->AddMdTypeWgt(gmtNNbr, 30);
  GasMdMType->AddMdTypeWgt(gmtCTb, 30);
  GasMdMType->NrmMdWgt();

  // convert tabular to ltp data
  if (!TabFNm.Empty()){
    TGasLtpBs::ConvTabToLtp(TabFNm, LtpFPath);}

  // load ltp data
  PGasLtpBs GasLtpBs=TGasLtpBs::Load(LtpFNm);

  // get gas station data
  //PGasStation GasStation=GetGasStation(GasDm);
  PGasStation GasStation=TGasStation::LoadXml(InGasStationFNm);

  if (GasStation->TmGridHours==24){
    // day orders
    // create domain
    PGasDm GasDm=TGasDm::GetDayGasDm(GasLtpBs);
    // generate orders
    TGasSimRecV GasSimRecV;
    TGasExe::GenDayOrder(
     OutLogFNm,
     GasStation, OutGasStationFNm,
     StartDt, ProcessDays,
     GasDm,
     GasMdMType, TGasMd::GetMdParamStr(),
     GasSimRecV);
    // save simulation
    if (!OutSimFNm.Empty()){
      TGasSimRec::SaveBin(OutSimFNm, GasSimRecV);}
  } else
  if ((0<GasStation->TmGridHours)&&(GasStation->TmGridHours<24)){
    // create domain
    PGasDm GasDm=TGasDm::GetHourBlockGasDm(GasLtpBs, GasStation->TmGridHours);
    // generate orders
    TGasSimRecV GasSimRecV;
    TGasExe::GenHourBlockOrder(
     OutLogFNm,
     GasStation, OutGasStationFNm,
     StartDt, ProcessDays,
     GasDm,
     GasMdMType, TGasMd::GetMdParamStr(),
     GasSimRecV);
    // save simulation
    if (!OutSimFNm.Empty()){
      TGasSimRec::SaveBin(OutSimFNm, GasSimRecV);}
  } else {
    TExcept::Throw(
     "Time-Grid should be 0<tg<=24", TInt::GetStr(GasStation->TmGridHours));
  }
  return 0;
  Catch;
  return 1;
}

