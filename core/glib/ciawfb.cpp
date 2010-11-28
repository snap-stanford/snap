#include "stdafx.h"
#include "ciawfb.h"

/////////////////////////////////////////////////
// CiaWorldFactBook-Country
void TCiaWFBCountry::AddFld(const TStr& _CountryNm,
 const TStr& SecNm, const TStr& FldNm, const TStr& FldVal){
  TStr _UcCountryNm=_CountryNm.GetUc();
  if (CountryNm.Empty()){CountryNm=_UcCountryNm;}
  else {IAssert(CountryNm==_UcCountryNm);}
  FldNmToValH.AddDat(FldNm, FldVal);
  FldNmToSecNmH.AddDat(FldNm, SecNm);
}

double TCiaWFBCountry::GetFldValNum(const TStr& FldNm) const {
  TChA ChA=GetFldVal(FldNm);
  int ChN=0;
  while ((ChN<ChA.Len())&&(!TCh::IsNum(ChA[ChN]))){ChN++;}
  if (ChN==ChA.Len()){return -1;}
  TChA NumChA;
  while ((ChN<ChA.Len())&&((TCh::IsNum(ChA[ChN]))||(ChA[ChN]==','))){
    if (TCh::IsNum(ChA[ChN])){
      NumChA+=ChA[ChN];}
    ChN++;
  }
  double Num=TStr(NumChA).GetFlt();
  return Num;
}

TStr TCiaWFBCountry::GetDescStr() const {
  TChA DescChA;
  DescChA+=GetCountryNm(); DescChA+=' ';
  DescChA+=GetFldVal("Background"); DescChA+=' ';
  DescChA+=GetFldVal("Location"); DescChA+=' ';
  DescChA+=GetFldVal("Climate"); DescChA+=' ';
  DescChA+=GetFldVal("Ethnic groups"); DescChA+=' ';
  DescChA+=GetFldVal("Religions"); DescChA+=' ';
  DescChA+=GetFldVal("Economy - overview"); DescChA+=' ';
  DescChA+=GetFldVal("Industries"); DescChA+=' ';
  DescChA+=GetFldVal("Disputes - international"); DescChA+=' ';
  DescChA+=GetFldVal("Illicit drugs"); DescChA+=' ';
  return DescChA;
}

void TCiaWFBCountry::Dump() const {
  printf("Country: %s\n", CountryNm.CStr());
  printf("Fields:\n");
  for (int i = 0; i < FldNmToValH.Len(); i++) {
    printf("  %s : %s\n", FldNmToValH.GetKey(i).CStr(), FldNmToValH[i].CStr()); }
  printf("\n");
}

PCiaWFBCountry TCiaWFBCountry::LoadHtml(const TStr& FNm){
  // create country object
  PCiaWFBCountry CiaWFBCountry=TCiaWFBCountry::New();
  // prepare section name & value
  TChA SecNmChA;
  TChA SecValChA;
  // open html file
  PSIn HtmlSIn=TFIn::New(FNm);
  THtmlLx HtmlLx(HtmlSIn);
  // parse html file
  while (HtmlLx.Sym!=hsyEof){
    if ((HtmlLx.Sym==hsyBTag)&&(HtmlLx.ChA=="<TD>")&&
     (HtmlLx.GetArg("CLASS")=="SectionHeadingPrint")){
      // section name
      HtmlLx.GetSym(); SecNmChA.Clr();
      while (!((HtmlLx.Sym==hsyETag)&&(HtmlLx.ChA=="<TD>"))){
        if ((HtmlLx.Sym==hsyStr)||(HtmlLx.Sym==hsyNum)||(HtmlLx.Sym==hsySSym)){
          SecNmChA+=HtmlLx.PreSpaceChA; SecNmChA+=HtmlLx.ChA;}
        HtmlLx.GetSym();
      }
      HtmlLx.GetSym();
      // section value
      IAssert((HtmlLx.Sym==hsyBTag)&&(HtmlLx.ChA=="<TD>")&&
       (HtmlLx.GetArg("CLASS")=="SectionHeadingPrint"));
      HtmlLx.GetSym(); SecValChA.Clr();
      while (!((HtmlLx.Sym==hsyETag)&&(HtmlLx.ChA=="<TD>"))){
        if ((HtmlLx.Sym==hsyStr)||(HtmlLx.Sym==hsyNum)||(HtmlLx.Sym==hsySSym)){
          if (HtmlLx.PreSpaces>0){SecValChA+=' ';}
          SecValChA+=HtmlLx.ChA;
        } else {SecValChA+=' ';}
        HtmlLx.GetSym();
      }
      //printf("   Section: '%s': '%s'\n", SecNmChA.CStr(), SecValChA.CStr());
    } else
    if ((HtmlLx.Sym==hsyBTag)&&(HtmlLx.ChA=="<TD>")&&
     (HtmlLx.GetArg("CLASS")=="FieldLabel")){
      // field name
      TChA FldNmChA;
      HtmlLx.GetSym();
      while (!((HtmlLx.Sym==hsyETag)&&(HtmlLx.ChA=="<TD>"))){
        if ((HtmlLx.Sym==hsyStr)||(HtmlLx.Sym==hsyNum)||(HtmlLx.Sym==hsySSym)){
          FldNmChA+=HtmlLx.PreSpaceChA; FldNmChA+=HtmlLx.ChA;}
        HtmlLx.GetSym();
      }
      if ((FldNmChA.Len()>0)&&(FldNmChA.LastCh()==':')){FldNmChA.Pop();}
      HtmlLx.GetSym();
      // field value
      IAssert((HtmlLx.Sym==hsyBTag)&&(HtmlLx.ChA=="<TD>"));
      TChA FldValChA;
      HtmlLx.GetSym();
      while (!((HtmlLx.Sym==hsyETag)&&
       ((HtmlLx.ChA=="<TD>")||(HtmlLx.ChA=="<TABLE>")))){
        if ((HtmlLx.Sym==hsyStr)||(HtmlLx.Sym==hsyNum)||(HtmlLx.Sym==hsySSym)){
          if (HtmlLx.PreSpaces>0){FldValChA+=' ';}
          FldValChA+=HtmlLx.ChA;
        } else {FldValChA+=' ';}
        HtmlLx.GetSym();
      }
      //printf("      Field: '%s': '%s'\n", FldNmChA.CStr(), FldValChA.CStr());
      CiaWFBCountry->AddFld(SecValChA, SecNmChA, FldNmChA, FldValChA);
    } else {
      //J: format se je spremenil, malo sem pohekal, da dobim ven ime drzave
      //J: sections so ukinjene, ni nobenega lepega nacina (kot recimo "FieldLabel")
      //J: da bi dobil ven imena sekcij
      //J: Se enkrat preveril Aprila 2006 in je tole zgoraj res. Tabela FldNmToSecNmH ima samo kljuce, nima podatkov
      //J: Ce ne uporabim kode spodaj je ime drzave prazno.
      if (HtmlLx.GetArg("CLASS")=="CountryName"){
        TChA FldNmChA;
        HtmlLx.GetSym();
        while (!((HtmlLx.Sym==hsyETag)&&(HtmlLx.ChA=="<TD>")||(HtmlLx.ChA=="<BR>"))){
          if ((HtmlLx.Sym==hsyStr)||(HtmlLx.Sym==hsyNum)||(HtmlLx.Sym==hsySSym)){
            FldNmChA+=HtmlLx.PreSpaceChA; FldNmChA+=HtmlLx.ChA;}
          HtmlLx.GetSym();
        }
        SecValChA = TStr(FldNmChA).GetLc(); // ime drzave
        SecNmChA = FNm.GetFMid(); // kratko ime drzave -- kratica
      }
      HtmlLx.GetSym();
    }
  }
  return CiaWFBCountry;
}

/////////////////////////////////////////////////
// CiaWorldFactBook-Country-Properties
TCiaWFBCntryProp::TCiaWFBCntryProp(const PCiaWFBCountry& CiaWFBCountry) : CountryNm(CiaWFBCountry->GetCountryNm()) {
  TStr Fld, Left, Right;
  int i1, i2, i3, i4;
  char ch1, ch2;
  // geo coordinates: wfbGeoXPos, wfbGeoYPos
  Fld = CiaWFBCountry->GetFldVal("Geographic coordinates").ToTrunc(); // 41 54 N, 12 27 E
  sscanf(Fld.CStr(), "%d %d %c, %d %d %c", &i1, &i2, &ch1, &i3, &i4, &ch2);
  PropH.AddDat(wfbGeoXPos, (ch1=='N'?1:-1) * i1+i2/60.0);
  PropH.AddDat(wfbGeoYPos, (ch2=='E'?1:-1) * i3+i4/60.0);
  // area
  Fld = CiaWFBCountry->GetFldVal("Area").ToTrunc();
  PropH.AddDat(wfbTotalArea, GetNum(Fld, "total:"));
  PropH.AddDat(wfbLandArea, GetNum(Fld, "land:"));
  PropH.AddDat(wfbWaterArea, GetNum(Fld, "water:"));
  // population: wfbPopulation, wfbAgeLs14, wfbAgeLs64, wfbAgeGt65, wfbAgeMedian
  PropH.AddDat(wfbPopulation, GetNum(CiaWFBCountry->GetFldVal("Population").ToTrunc(), ""));
  Fld = CiaWFBCountry->GetFldVal("Age structure").ToTrunc();
  PropH.AddDat(wfbAgeLs14, GetNum(Fld, "0-14 years:"));
  PropH.AddDat(wfbAgeLs64, GetNum(Fld, "15-64 years:"));
  PropH.AddDat(wfbAgeGt65, GetNum(Fld, "65 years and over:"));
  PropH.AddDat(wfbAgeMedian, GetNum(CiaWFBCountry->GetFldVal("Median age").ToTrunc(), "total:"));
  // wfbPopGrowth, wfbBirthRt, wfbDeathRt, wfbInfMort, wfbLifeExp, wfbFertility, wfbAidsPct, wfbLiteracy,
  PropH.AddDat(wfbPopGrowth, GetNum(CiaWFBCountry->GetFldVal("Population growth rate").ToTrunc(), ""));
  PropH.AddDat(wfbBirthRt, GetNum(CiaWFBCountry->GetFldVal("Birth rate").ToTrunc(), ""));
  PropH.AddDat(wfbDeathRt, GetNum(CiaWFBCountry->GetFldVal("Death rate").ToTrunc(), ""));
  PropH.AddDat(wfbInfMort, GetNum(CiaWFBCountry->GetFldVal("Infant mortality rate").ToTrunc(), "total:"));
  PropH.AddDat(wfbLifeExp, GetNum(CiaWFBCountry->GetFldVal("Life expectancy at birth").ToTrunc(), "total population:"));
  PropH.AddDat(wfbFertility, GetNum(CiaWFBCountry->GetFldVal("Total fertility rate").ToTrunc(), ""));
  PropH.AddDat(wfbAidsPct, GetNum(CiaWFBCountry->GetFldVal("HIV/AIDS - adult prevalence rate").ToTrunc(), ""));
  PropH.AddDat(wfbLiteracy, GetNum(CiaWFBCountry->GetFldVal("Literacy").ToTrunc(), "total population:"));
  PropH.AddDat(wfbGdpPerCapita, GetNum(CiaWFBCountry->GetFldVal("GDP - per capita (PPP)").ToTrunc(), ""));
  PropH.AddDat(wfbGdpGrowth, GetNum(CiaWFBCountry->GetFldVal("GDP - real growth rate").ToTrunc(), ""));
  PropH.AddDat(wfbLaborForce, GetNum(CiaWFBCountry->GetFldVal("Labor force").ToTrunc(), ""));
  PropH.AddDat(wfbUnemploy, GetNum(CiaWFBCountry->GetFldVal("Unemployment rate").ToTrunc(), ""));
  PropH.AddDat(wfbPoverty, GetNum(CiaWFBCountry->GetFldVal("Population below poverty line").ToTrunc(), ""));
  //wfbInflation, wfbInvestment, wfbPubDebt,
  PropH.AddDat(wfbInflation, GetNum(CiaWFBCountry->GetFldVal("Inflation rate (consumer prices)").ToTrunc(), ""));
  PropH.AddDat(wfbInvestment, GetNum(CiaWFBCountry->GetFldVal("Investment (gross fixed)").ToTrunc(), ""));
  PropH.AddDat(wfbPubDebt, GetNum(CiaWFBCountry->GetFldVal("Public debt").ToTrunc(), ""));
  //wfbTVs, wfbTelephones, wfbMobiphones, wfbRailways, wfbRoadways, wfbMiltaryGdp
  PropH.AddDat(wfbTVs, GetNum(CiaWFBCountry->GetFldVal("").ToTrunc(), ""));
  PropH.AddDat(wfbTelephones, GetNum(CiaWFBCountry->GetFldVal("Telephones - main lines in use").ToTrunc(), ""));
  PropH.AddDat(wfbMobiphones, GetNum(CiaWFBCountry->GetFldVal("Telephones - mobile cellular").ToTrunc(), ""));
  PropH.AddDat(wfbInetHosts, GetNum(CiaWFBCountry->GetFldVal("Internet hosts").ToTrunc(), ""));
  PropH.AddDat(wfbInetUsrs, GetNum(CiaWFBCountry->GetFldVal("Internet users").ToTrunc(), ""));
  PropH.AddDat(wfbRailways, GetNum(CiaWFBCountry->GetFldVal("Railways").ToTrunc(), "total:"));
  PropH.AddDat(wfbRoadways, GetNum(CiaWFBCountry->GetFldVal("Roadways").ToTrunc(), "totals:"));
  PropH.AddDat(wfbMiltaryGdp, GetNum(CiaWFBCountry->GetFldVal("Military expenditures - percent of GDP").ToTrunc(), ""));
  // if (! HasAll()) { Dump(); }
}

bool TCiaWFBCntryProp::HasAll() const {
  for (int p = 0; p < int(wfpPropMx); p++) {
    if (! PropH.IsKey(p)) { return false; }
    if (PropH.GetDat(p) == -1.0) { return false; }
  }
  return true;
}

double TCiaWFBCntryProp::GetProp(const TCiaWfbProp& PropId) const {
  if (! PropH.IsKey(PropId)) { return -1; }
  else { return PropH.GetDat(PropId); }
}

void TCiaWFBCntryProp::Dump() {
  printf("%s\n", CountryNm.CStr());
  for (int p = 0; p < int(wfpPropMx); p++) {
    printf("  %10s : %g\n", GetPropStr(TCiaWfbProp(p)).CStr(), GetProp(TCiaWfbProp(p)));
  }
  printf("\n");
}

double TCiaWFBCntryProp::GetNum(const TStr& Str, const TStr& PrefStr) {
  int b = Str.SearchStr(PrefStr);
  if (b == -1) { return -1; }
  b = b+PrefStr.Len();
  while (b < Str.Len() && ! TCh::IsNum(Str[b])) { b++; }
  if (b == Str.Len()) { return -1; }
  TChA Num;
  while (b < Str.Len() && TCh::IsNum(Str[b])) {
    if (TCh::IsNum(Str[b])) { Num += Str[b]; }  b++;
    if (Str[b] == '.') { Num += Str[b]; b++; }
    else if (Str[b] == ',') { b++; }
  }
  double Val = TStr(Num).GetFlt();
  if (Val != -1) {
    double Const = 1;
    int Bl = Str.SearchStr("billion");
    if (Bl != -1 && Bl-b < 5 && Bl-b > 0) { Const = 1e9; }
    int Ml = Str.SearchStr("million");
    if (Ml != -1 && Ml-b < 5 && Ml-b > 0) { Const = 1e6; }
    Val = Val * Const;
  }
  return Val;
}

TStr TCiaWFBCntryProp::GetPropStr(const TCiaWfbProp& PropId) {
  switch (PropId) {
    case wfbGeoXPos : return "GeoXPos";
    case wfbGeoYPos : return "GeoYPos";
    case wfbTotalArea : return "TotalArea";
    case wfbLandArea : return "LandArea";
    case wfbWaterArea : return "WaterArea";
    case wfbPopulation : return "Population";
    case wfbAgeLs14 : return "AgeLs14";
    case wfbAgeLs64 : return "AgeLs64";
    case wfbAgeGt65 : return "AgeGt65";
    case wfbAgeMedian : return "AgeMedian";
    case wfbPopGrowth : return "PopGrowth";
    case wfbBirthRt : return "BirthRt";
    case wfbDeathRt : return "DeathRt";
    case wfbInfMort : return "InfMort";
    case wfbLifeExp : return "LifeExp";
    case wfbFertility : return "Fertility";
    case wfbAidsPct : return "AidsPct";
    case wfbLiteracy : return "Literacy";
    case wfbGdpPerCapita : return "GdpPerCapita";
    case wfbGdpGrowth : return "GdpGrowth";
    case wfbLaborForce : return "LaborForce";
    case wfbUnemploy : return "Unemploy";
    case wfbPoverty : return "Poverty";
    case wfbInflation : return "Inflation";
    case wfbInvestment : return "Investment";
    case wfbPubDebt : return "PubDebt";
    case wfbTVs : return "TVs";
    case wfbTelephones : return "Telephones";
    case wfbMobiphones : return "Mobiphones";
    case wfbInetHosts : return "InetHosts";
    case wfbInetUsrs : return "InetUsrs";
    case wfbRailways : return "Railways";
    case wfbRoadways : return "Roadways";
    case wfbMiltaryGdp : return "MiltaryGdp";
    default: Fail; // not all cases in switch are handled
  };
  Fail;
  return TStr::GetNullStr();
}

/////////////////////////////////////////////////
// CiaWorldFactBook-Base
PCiaWFBBs TCiaWFBBs::LoadHtml(const TStr& FPath){
  // create base
  PCiaWFBBs CiaWFBBs=TCiaWFBBs::New();
  // traverse files
  TFFile FFile(FPath, TStr(".html")); TStr FNm;
  while (FFile.Next(FNm)){
    if (FNm.GetFMid().Len()!=2){continue;}
    printf("%s\n", FNm.CStr());
    PCiaWFBCountry CiaWFBCountry=TCiaWFBCountry::LoadHtml(FNm);
    CiaWFBBs->AddCountry(CiaWFBCountry);
  }
  // return
  return CiaWFBBs;
}

PCiaWFBBs TCiaWFBBs::LoadBinOrHtml(const TStr& BinFNm, const TStr& HtmlFPath){
  printf("Loading files ...\n");
  PCiaWFBBs CiaWFBBs;
  if (TFile::Exists(BinFNm)){
    CiaWFBBs=TCiaWFBBs::LoadBin(BinFNm);
  } else {
    CiaWFBBs=LoadHtml(HtmlFPath);
    CiaWFBBs->SaveBin(BinFNm);
  }
  printf("Done.\n");
  return CiaWFBBs;
}

