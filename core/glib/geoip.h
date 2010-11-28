/////////////////////////////////////////////////
// Geographical-IP
ClassTP(TGeoIpBs, PGeoIpBs)//{
private:
  TStrStrH CountrySNmToLNmH;
  TUIntV CountryMnIpNumV;
  THash<TUInt, TUIntIntPr> MnIpNumToMxIpNumCountryIdPrH;
  THash<TInt, TIntIntFltFltQu> LocIdToCountryIdCityIdLatitudeLongitudeQuH;
  TUIntV LocMnIpNumV;
  THash<TUInt, TUIntIntPr> MnIpNumToMxIpNumLocIdPrH;
  TStrH GeoNmH;
  UndefCopyAssign(TGeoIpBs);
public:
  TGeoIpBs():
    CountrySNmToLNmH(), 
    CountryMnIpNumV(), MnIpNumToMxIpNumCountryIdPrH(), 
    LocIdToCountryIdCityIdLatitudeLongitudeQuH(),
    LocMnIpNumV(), MnIpNumToMxIpNumLocIdPrH(), 
    GeoNmH(){}
  static PGeoIpBs New(){return new TGeoIpBs();}
  ~TGeoIpBs(){}
  TGeoIpBs(TSIn& SIn):
    CountrySNmToLNmH(SIn), 
    CountryMnIpNumV(SIn), MnIpNumToMxIpNumCountryIdPrH(SIn), 
    LocIdToCountryIdCityIdLatitudeLongitudeQuH(SIn),
    LocMnIpNumV(SIn), MnIpNumToMxIpNumLocIdPrH(SIn), 
    GeoNmH(SIn){}
  static PGeoIpBs Load(TSIn& SIn){return new TGeoIpBs(SIn);}
  void Save(TSOut& SOut){
    CountrySNmToLNmH.Save(SOut);
    CountryMnIpNumV.Save(SOut); MnIpNumToMxIpNumCountryIdPrH.Save(SOut); 
    LocIdToCountryIdCityIdLatitudeLongitudeQuH.Save(SOut);
    LocMnIpNumV.Save(SOut); MnIpNumToMxIpNumLocIdPrH.Save(SOut); 
    GeoNmH.Save(SOut);}

  // search
  void GetCountryNm(const TStr& IpNumStr, TStr& CountrySNm, TStr& CountryLNm);
  void GetLocation(const TStr& IpNumStr, 
   TStr& CountrySNm, TStr& CityNm, double& Latitude, double& Longitude);

  // files
  static PGeoIpBs LoadCsv(const TStr& GeoIpFPath, const bool& CountriesOnlyP=false);
  static PGeoIpBs LoadBin(const TStr& FNm){
    TFIn SIn(FNm); return Load(SIn);}
  void SaveBin(const TStr& FNm){
    TFOut SOut(FNm); Save(SOut);}
};


