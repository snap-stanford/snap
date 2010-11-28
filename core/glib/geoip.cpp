/////////////////////////////////////////////////
// Geographical-IP
void TGeoIpBs::GetCountryNm(const TStr& IpNumStr, TStr& CountrySNm, TStr& CountryLNm){
  // prepare country-names
  CountrySNm="--"; CountryLNm="Unknown";

  // split ip-num to sub-number-strings
  TStrV IpSubNumStrV; IpNumStr.SplitOnAllCh('.', IpSubNumStrV, false);
  // convert sub-number-strings to sub-numbers and ip-number
  int IpSubNum0, IpSubNum1, IpSubNum2, IpSubNum3;
  uint IpNum;
  if (
   IpSubNumStrV[0].IsInt(true, 0, 255, IpSubNum0)&&
   IpSubNumStrV[1].IsInt(true, 0, 255, IpSubNum1)&&
   IpSubNumStrV[2].IsInt(true, 0, 255, IpSubNum2)&&
   IpSubNumStrV[3].IsInt(true, 0, 255, IpSubNum3)){
    IpNum=16777216*IpSubNum0+65536*IpSubNum1+256*IpSubNum2+IpSubNum3;
  } else {
    return;
  }

  // get country-id from ip-number
  int CountryId=-1;
  int IpNumN; CountryMnIpNumV.SearchBin(IpNum+1, IpNumN);
  if (IpNumN>0){
    uint MnIpNum=CountryMnIpNumV[IpNumN-1];
    uint MxIpNum=MnIpNumToMxIpNumCountryIdPrH.GetDat(MnIpNum).Val1;
    if ((MnIpNum<=IpNum)&&(IpNum<=MxIpNum)){
      CountryId=MnIpNumToMxIpNumCountryIdPrH.GetDat(MnIpNum).Val2;
    }
  }
  // get country-names
  if (CountryId!=-1){
    CountrySNm=CountrySNmToLNmH.GetKey(CountryId);
    CountryLNm=CountrySNmToLNmH[CountryId];
  }
}

void TGeoIpBs::GetLocation(const TStr& IpNumStr, 
 TStr& CountrySNm, TStr& CityNm, double& Latitude, double& Longitude){
  // prepare unknown location info
  CountrySNm="--"; CityNm="Unknown"; Latitude=0; Longitude=0;

  // split ip-num to sub-number-strings
  TStrV IpSubNumStrV; IpNumStr.SplitOnAllCh('.', IpSubNumStrV, false);
  // convert sub-number-strings to sub-numbers and ip-number
  int IpSubNum0, IpSubNum1, IpSubNum2, IpSubNum3;
  uint IpNum;
  if (
   IpSubNumStrV[0].IsInt(true, 0, 255, IpSubNum0)&&
   IpSubNumStrV[1].IsInt(true, 0, 255, IpSubNum1)&&
   IpSubNumStrV[2].IsInt(true, 0, 255, IpSubNum2)&&
   IpSubNumStrV[3].IsInt(true, 0, 255, IpSubNum3)){
    IpNum=16777216*IpSubNum0+65536*IpSubNum1+256*IpSubNum2+IpSubNum3;
  } else {
    return;
  }

  // get location-id from ip-number
  int LocId=-1;
  int IpNumN; LocMnIpNumV.SearchBin(IpNum+1, IpNumN);
  if (IpNumN>0){
    uint MnIpNum=LocMnIpNumV[IpNumN-1];
    uint MxIpNum=MnIpNumToMxIpNumLocIdPrH.GetDat(MnIpNum).Val1;
    if ((MnIpNum<=IpNum)&&(IpNum<=MxIpNum)){
      LocId=MnIpNumToMxIpNumLocIdPrH.GetDat(MnIpNum).Val2;
    }
  }
  // get location info
  if (LocId!=-1){
    int CountrySNmId=LocIdToCountryIdCityIdLatitudeLongitudeQuH.GetDat(LocId).Val1;
    CountrySNm=GeoNmH.GetKey(CountrySNmId);
    int CityNmId=LocIdToCountryIdCityIdLatitudeLongitudeQuH.GetDat(LocId).Val2;
    CityNm=GeoNmH.GetKey(CityNmId);
    Latitude=LocIdToCountryIdCityIdLatitudeLongitudeQuH.GetDat(LocId).Val3;
    Longitude=LocIdToCountryIdCityIdLatitudeLongitudeQuH.GetDat(LocId).Val4;
  }
}

PGeoIpBs TGeoIpBs::LoadCsv(const TStr& GeoIpFPath, const bool& CountriesOnlyP){
  PGeoIpBs GeoIpBs=TGeoIpBs::New();
  // filenames
  TStr GeoIpNrFPath=TStr::GetNrFPath(GeoIpFPath);
  TStr CountryFNm=GeoIpNrFPath+"GeoIPCountryWhois.csv";
  TStr CityLocationFNm=GeoIpNrFPath+"GeoLiteCity-Location.csv";
  TStr CityBlocksFNm=GeoIpNrFPath+"GeoLiteCity-Blocks.csv";
  // country-level data
  {PSs Ss=TSs::LoadTxt(ssfCommaSep, CountryFNm, TNotify::StdNotify);
  for (int Y=0; Y<Ss->GetYLen(); Y++){
    uint MnIpNum=Ss->At(2, Y).GetUInt();
    uint MxIpNum=Ss->At(3, Y).GetUInt();;
    TStr CountrySNm=Ss->At(4, Y);
    TStr CountryLNm=Ss->At(5, Y);
    GeoIpBs->CountrySNmToLNmH.AddDat(CountrySNm)=CountryLNm;
    int CountryId=GeoIpBs->CountrySNmToLNmH.GetKeyId(CountrySNm);
    GeoIpBs->MnIpNumToMxIpNumCountryIdPrH.AddDat(MnIpNum, TUIntIntPr(MxIpNum, CountryId));
    GeoIpBs->CountryMnIpNumV.Add(MnIpNum);
  }
  printf("Sorting ... ");
  GeoIpBs->CountryMnIpNumV.Sort();}
  printf("Done.\n");
  if (!CountriesOnlyP){
    // city-locations data
    {PSs Ss=TSs::LoadTxt(ssfCommaSep, CityLocationFNm, TNotify::StdNotify, false);
    for (int Y=2; Y<Ss->GetYLen(); Y++){
      int LocId=Ss->At(0, Y).GetInt();
      TStr CountrySNm=Ss->At(1, Y);
      int CountryId=GeoIpBs->GeoNmH.AddKey(CountrySNm);
      //TStr RegionNm=Ss->At(2, Y);
      //int RegionId=GeoIpBs->GeoNmH.AddKey(RegionNm);
      TStr CityNm=Ss->At(3, Y);
      int CityId=GeoIpBs->GeoNmH.AddKey(CityNm);
      double Latitude=Ss->At(5, Y).GetFlt();
      double Longitude=Ss->At(6, Y).GetFlt();
      // add data
      GeoIpBs->LocIdToCountryIdCityIdLatitudeLongitudeQuH.AddDat(
       LocId, TIntIntFltFltQu(CountryId, CityId, Latitude, Longitude));
    }}
    // city-blocks data
    {PSs Ss=TSs::LoadTxt(ssfCommaSep, CityBlocksFNm, TNotify::StdNotify, false);
    for (int Y=2; Y<Ss->GetYLen(); Y++){
      uint MnIpNum=Ss->At(0, Y).GetUInt();
      uint MxIpNum=Ss->At(1, Y).GetUInt();
      int LocId=Ss->At(2, Y).GetInt();
      GeoIpBs->MnIpNumToMxIpNumLocIdPrH.AddDat(MnIpNum, TUIntIntPr(MxIpNum, LocId));
      GeoIpBs->LocMnIpNumV.Add(MnIpNum);
    }
    printf("Sorting ... ");
    GeoIpBs->LocMnIpNumV.Sort();}
    printf("Done.\n");
  }
  // return geoip base
  return GeoIpBs;
}