#ifndef glib_cia_wfb
#define glib_cia_wfb

/////////////////////////////////////////////////
// CiaWorldFactBook-Country
ClassTPV(TCiaWFBCountry, PCiaWFBCountry, TCiaWFBCountryV)//{
private:
  TStr CountryNm;
  TStrStrH FldNmToValH;
  TStrStrH FldNmToSecNmH;
public:
  TCiaWFBCountry(){}
  static PCiaWFBCountry New(){return new TCiaWFBCountry();}
  TCiaWFBCountry(TSIn& SIn):
    CountryNm(SIn), FldNmToValH(SIn), FldNmToSecNmH(SIn){}
  static PCiaWFBCountry Load(TSIn& SIn){return new TCiaWFBCountry(SIn);}
  void Save(TSOut& SOut){
    CountryNm.Save(SOut); FldNmToValH.Save(SOut); FldNmToSecNmH.Save(SOut);}

  TCiaWFBCountry& operator=(const TCiaWFBCountry&){Fail; return *this;}

  // fields
  void AddFld(const TStr& _CountryNm,
   const TStr& SecNm, const TStr& FldNm, const TStr& FldVal);
  TStr GetCountryNm() const {return CountryNm;}
  int GetFlds() const {return FldNmToValH.Len();}
  void GetFldNmVal(const int& FldN, TStr& FldNm, TStr& FldVal) const {
    FldNm=FldNmToValH.GetKey(FldN); FldVal=FldNmToValH[FldN];}
  TStr GetSecNm(const TStr& FldNm) const {
    return FldNmToSecNmH.GetDat(FldNm);}
  bool IsFld(const TStr& FldNm, TStr& FldVal) const {
    return FldNmToValH.IsKeyGetDat(FldNm, FldVal);}
  TStr GetFldVal(const TStr& FldNm) const {
    TStr FldVal; if (IsFld(FldNm, FldVal)){return FldVal;} else {return "";}}
  double GetFldValNum(const TStr& FldNm) const;
  TStr GetDescStr() const;
  void Dump() const;

  // files
  static PCiaWFBCountry LoadBin(const TStr& FNm){
    TFIn FIn(FNm); return new TCiaWFBCountry(FIn);}
  void SaveBin(const TStr& FNm){
    TFOut FOut(FNm); Save(FOut);}

  // loading from original html file
  static PCiaWFBCountry LoadHtml(const TStr& FNm);
};

/////////////////////////////////////////////////
// CiaWorldFactBook-Country-Properties
typedef enum {
  wfbGeoXPos, wfbGeoYPos, wfbTotalArea, wfbLandArea, wfbWaterArea, 
  wfbPopulation, wfbAgeLs14, wfbAgeLs64, wfbAgeGt65, wfbAgeMedian,
  wfbPopGrowth, wfbBirthRt, wfbDeathRt, wfbInfMort, wfbLifeExp, wfbFertility, wfbAidsPct, 
  wfbLiteracy, wfbGdpPerCapita, wfbGdpGrowth, wfbLaborForce, wfbUnemploy, wfbPoverty,
  wfbInflation, wfbInvestment, wfbPubDebt, 
  wfbTVs, wfbTelephones, wfbMobiphones, wfbInetHosts, wfbInetUsrs,
  wfbRailways, wfbRoadways, wfbMiltaryGdp,
  wfpPropMx
} TCiaWfbProp;

class TCiaWFBCntryProp {
private:
  TStr CountryNm;
  THash<TInt, TFlt> PropH;
public:
  TCiaWFBCntryProp() { }
  TCiaWFBCntryProp(const PCiaWFBCountry& CiaWFBCountry);
  TCiaWFBCntryProp(const TCiaWFBCntryProp& Prop) : CountryNm(Prop.CountryNm), PropH(Prop.PropH) { }
  TCiaWFBCntryProp(TSIn& SIn) : CountryNm(SIn), PropH(SIn) { }
  void Save(TSOut& SOut) const { CountryNm.Save(SOut); PropH.Save(SOut); }

  bool HasAll() const;
  double GetProp(const TCiaWfbProp& PropId) const;
  void Dump();
  static double GetNum(const TStr& Str, const TStr& PrefStr);
  static TStr GetPropStr(const TCiaWfbProp& PropId);
};

/////////////////////////////////////////////////
// CiaWorldFactBook-Base
ClassTPV(TCiaWFBBs, PCiaWFBBs, TCiaWFBBsV)//{
private:
  THash<TStr, PCiaWFBCountry> NmToCountryH;
public:
  TCiaWFBBs(){}
  static PCiaWFBBs New(){return new TCiaWFBBs();}
  TCiaWFBBs(TSIn& SIn): NmToCountryH(SIn){}
  static PCiaWFBBs Load(TSIn& SIn){return new TCiaWFBBs(SIn);}
  void Save(TSOut& SOut){NmToCountryH.Save(SOut);}

  TCiaWFBBs& operator=(const TCiaWFBBs&){Fail; return *this;}

  // countries
  void AddCountry(const PCiaWFBCountry& CiaWFBCountry){
    NmToCountryH.AddDat(CiaWFBCountry->GetCountryNm(), CiaWFBCountry);}
  int GetCountries() const {return NmToCountryH.Len();}
  PCiaWFBCountry GetCountry(const int& CountryN){
    return NmToCountryH[CountryN];}
  bool IsCountry(const TStr& CountryNm) const {
    return NmToCountryH.IsKey(CountryNm.GetUc());}
  bool IsCountry(const TStr& CountryNm, PCiaWFBCountry& CiaWFBCountry) const {
    return NmToCountryH.IsKeyGetDat(CountryNm.GetUc(), CiaWFBCountry);}
  PCiaWFBCountry GetCountry(const TStr& CountryNm){
    return NmToCountryH.GetDat(CountryNm.GetUc());}

  // files
  static PCiaWFBBs LoadBin(const TStr& FNm){
    TFIn FIn(FNm); return new TCiaWFBBs(FIn);}
  void SaveBin(const TStr& FNm){
    TFOut FOut(FNm); Save(FOut);}

  // loading from original html files
  static PCiaWFBBs LoadHtml(const TStr& FPath);
  static PCiaWFBBs LoadBinOrHtml(const TStr& BinFNm, const TStr& HtmlFPath);
};

#endif
