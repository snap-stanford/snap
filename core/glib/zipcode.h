#ifndef zipcode_h
#define zipcode_h

/////////////////////////////////////////////////
// Includes
#include "base.h"

/////////////////////////////////////////////////
// Zip-Code-Entry
ClassTPV(TZipCdEntry, PZipCdEntry, TZipCdEntryV)//{
public:
  TStr StateCdStr; // Field 1 - State Fips Code 
  TStr ZipCdStr; // Field 2 - 5-digit Zipcode 
  TStr StateNm; // Field 3 - State Abbreviation 
  TStr PlaceNm; // Field 4 - Zipcode Name
  TFlt Longitude; // Field 5 - Longitude in Decimal Degrees (West is assumed, no minus sign)
  TFlt Latitude; // Field 6 - Latitude in Decimal Degrees (North is assumed, no plus sign) 
  TInt Population; // Field 7 - 1990 Population (100%) 
  TFlt PortionOfState; // Field 8 - Allocation Factor (decimal portion of state within zipcode)
  UndefCopyAssign(TZipCdEntry);
public:
  TZipCdEntry(){}
  static PZipCdEntry TZipCdEntry::New(){return new TZipCdEntry();}
  ~TZipCdEntry(){}
  TZipCdEntry(TSIn& SIn):
    StateCdStr(SIn), ZipCdStr(SIn), 
    StateNm(SIn), PlaceNm(SIn), 
    Longitude(SIn), Latitude(SIn), 
    Population(SIn), PortionOfState(SIn){}
  static PZipCdEntry Load(TSIn& SIn){return new TZipCdEntry(SIn);}
  void Save(TSOut& SOut) const {
    StateCdStr.Save(SOut); ZipCdStr.Save(SOut);
    StateNm.Save(SOut); PlaceNm.Save(SOut);
    Longitude.Save(SOut); Latitude.Save(SOut);
    Population.Save(SOut); PortionOfState.Save(SOut);}
    
  // components
  TStr GetStateCdStr() const {return StateCdStr;}
  TStr GetZipCdStr() const {return ZipCdStr;}
  TStr GetStateNm() const {return StateNm;}
  TStr GetPlaceNm() const {return PlaceNm;}
  double GetLongitude() const {return Longitude;}
  double GetLatitude() const {return Latitude;}
  int GetPopulation() const {return Population;}
  double GetPortionOfState() const {return PortionOfState;}
};

/////////////////////////////////////////////////
// Zip-Code-Base
ClassTP(TZipCdBs, PZipCdBs)//{
private:
  THash<TStr, PZipCdEntry> CdToZipEntryH;
  UndefCopyAssign(TZipCdBs);
public:
  TZipCdBs():
    CdToZipEntryH(){}
  static PZipCdBs TZipCdBs::New(){return new TZipCdBs();}
  ~TZipCdBs(){}
  TZipCdBs(TSIn& SIn):
    CdToZipEntryH(SIn){}
  static PZipCdBs Load(TSIn& SIn){return new TZipCdBs(SIn);}
  void Save(TSOut& SOut) const {
    CdToZipEntryH.Save(SOut);}
    
  // query
  int GetZipCds() const {return CdToZipEntryH.Len();}
  TStr GetZipCdStr(const int& ZipCdN) const {return CdToZipEntryH[ZipCdN]->GetZipCdStr();}
  PZipCdEntry GetZipCdEntry(const int& ZipCdN) const {return CdToZipEntryH[ZipCdN];}
  bool IsZipCd(const TStr& ZipCdStr) const {return CdToZipEntryH.IsKey(ZipCdStr);}
  PZipCdEntry GetZipCdEntry(const TStr& ZipCdStr) const {return CdToZipEntryH.GetDat(ZipCdStr);}
  void SearchZipCd(const TStr& QueryStr, const bool& ExactMatchP, TZipCdEntryV& ZipCdEntryV);

  // files
  static PZipCdBs LoadCsv(const TStr& FNm);
  static PZipCdBs LoadBin(const TStr& FNm){
    TFIn SIn(FNm); return Load(SIn);}
  void SaveBin(const TStr& FNm){
    TFOut SOut(FNm); Save(SOut);}
};

#endif