/////////////////////////////////////////////////
// Includes
#include "zipcode.h"

/////////////////////////////////////////////////
// Zip-Code-Base
void TZipCdBs::SearchZipCd(
 const TStr& QueryStr, const bool& ExactMatchP, TZipCdEntryV& ZipCdEntryV){
  ZipCdEntryV.Clr();
  int ZipCds=GetZipCds(); TStr NrQueryStr=QueryStr.GetUc();
  for (int ZipCdN=0; ZipCdN<GetZipCds(); ZipCdN++){
    PZipCdEntry ZipCdEntry=GetZipCdEntry(ZipCdN);
    if (ExactMatchP){
      if ((ZipCdEntry->GetPlaceNm()==NrQueryStr)||
       (ZipCdEntry->GetStateNm()==NrQueryStr)){
        ZipCdEntryV.Add(ZipCdEntry);
      }
    } else {
      if ((ZipCdEntry->GetPlaceNm().IsStrIn(NrQueryStr))||
       (ZipCdEntry->GetStateNm().IsStrIn(NrQueryStr))){
        ZipCdEntryV.Add(ZipCdEntry);
      }
    }
  }
}

PZipCdBs TZipCdBs::LoadCsv(const TStr& FNm){
  // create zip-code base
  PZipCdBs ZipCdBs=TZipCdBs::New();
  // load zip-code csv file
  PSs Ss=TSs::LoadTxt(ssfCommaSep, FNm, TNotify::StdNotify, false);
  // traverse zip-code records
  int ZipCds=Ss->GetYLen();
  for (int ZipCdN=0; ZipCdN<ZipCds; ZipCdN++){
    PZipCdEntry ZipCdEntry=TZipCdEntry::New();
    // get fields values
    ZipCdEntry->StateCdStr=Ss->GetVal(0, ZipCdN);
    ZipCdEntry->ZipCdStr=Ss->GetVal(1, ZipCdN).GetTrunc();
    ZipCdEntry->StateNm=Ss->GetVal(2, ZipCdN);
    ZipCdEntry->PlaceNm=Ss->GetVal(3, ZipCdN);
    ZipCdEntry->Longitude=Ss->GetVal(4, ZipCdN).GetFlt();
    ZipCdEntry->Latitude=Ss->GetVal(5, ZipCdN).GetFlt();
    ZipCdEntry->Population=Ss->GetVal(6, ZipCdN).GetInt();
    ZipCdEntry->PortionOfState=Ss->GetVal(7, ZipCdN).GetTrunc().GetFlt();
    // add entry to base
    ZipCdBs->CdToZipEntryH.AddDat(ZipCdEntry->GetZipCdStr(), ZipCdEntry);
  }
  // return base
  return ZipCdBs;
}