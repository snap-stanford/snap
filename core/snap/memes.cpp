#include "stdafx.h"
#include "memes.h"
//#include "spinn3r.h"

/////////////////////////////////////////////////
// Quote Statistics
int TQuote::GetFq() const {
  int fq=0;
  for(int i=0; i<TmUrlCntV.Len(); i++) {
    fq+=TmUrlCntV[i].Cnt(); }
  return fq;
  //return TmUrlCntV.Len(); //!!!!
}

int TQuote::GetFq(const TSecTm& BegTm, const TSecTm& EndTm) const {
  int fq=0;
  for(int i=0; i<TmUrlCntV.Len(); i++) {
    if (TmUrlCntV[i].Tm() >= BegTm && TmUrlCntV[i].Tm() < EndTm) {
      fq += TmUrlCntV[i].Cnt(); } //!!!
      //fq+=1; }
  }
  return fq;
}

int TQuote::GetDoms(const TQuoteBs& QtBs) const {
  THashSet<TChA> DomSet;
  for (int u = 0; u < TmUrlCntV.Len(); u++) {
    DomSet.AddKey(TStrUtil::GetDomNm(QtBs.GetStr(TmUrlCntV[u].UrlId())));
  }
  return DomSet.Len();
}

int TQuote::GetFq(const TUrlTy& UrlTy, const TQuoteBs& QtBs) const {
  int fq=0;
  for(int i=0; i<TmUrlCntV.Len(); i++) {
    if (UrlTy==utUndef || QtBs.GetUrlTy(TmUrlCntV[i].UrlId())==UrlTy) {
      fq += TmUrlCntV[i].Cnt(); } //!!!
      //fq+=1; }
  }
  return fq;
}

int TQuote::GetFq(const TSecTm& BegTm, const TSecTm& EndTm, const TUrlTy& UrlTy, const TQuoteBs& QtBs) const {
  int fq=0;
  for(int i=0; i<TmUrlCntV.Len(); i++) {
    if (TmUrlCntV[i].Tm() >= BegTm && TmUrlCntV[i].Tm() <= EndTm &&
     (UrlTy==utUndef || QtBs.GetUrlTy(TmUrlCntV[i].UrlId())==UrlTy)) {
      //fq += TmUrlCntV[i].Cnt(); } //!!!
      fq+=1; }
  }
  return fq;
}

int TQuote::GetUrls(const TSecTm& BegTm, const TSecTm& EndTm, const TUrlTy& UrlTy, const TQuoteBs& QtBs) const {
  int urls=0;
  for(int i=0; i<TmUrlCntV.Len(); i++) {
    if (TmUrlCntV[i].Tm() >= BegTm && TmUrlCntV[i].Tm() < EndTm &&
     (UrlTy==utUndef || QtBs.GetUrlTy(TmUrlCntV[i].UrlId())==UrlTy)) {
      urls+=1; }
  }
  return urls;
}

TSecTm TQuote::GetPeakTm(const TTmUnit& TmUnit, const TSecTm& AfterTm) const {
  TInt FreqAtPeak;
  return GetPeakTm(TmUnit, AfterTm, FreqAtPeak);
}

TSecTm TQuote::GetPeakTm(const TTmUnit& TmUnit, const TSecTm& AfterTm, TInt& FreqAtPeak) const {
  const TSecTm After = AfterTm.Round(TmUnit);
  THash<TSecTm, TInt> TmFqH;
  for(int i=0; i<TmUrlCntV.Len(); i++) {
    if (TmUrlCntV[i].Tm().Round(TmUnit) >= After) {
      TmFqH.AddDat(TmUrlCntV[i].Tm().Round(TmUnit)) += TmUrlCntV[i].Cnt(); }//!!!
      //TmFqH.AddDat(TmUrlCntV[i].Tm().Round(TmUnit)) += 1; }
  }
  if (TmFqH.Empty()) {
    FreqAtPeak = 0;
    return GetPeakTm(TmUnit, TSecTm(1));
  }
  TmFqH.SortByDat(false);
  FreqAtPeak = TmFqH[0];
  return TmFqH.GetKey(0);
}

TSecTm TQuote::GetPeakTm(const TTmUnit& TmUnit, const TSecTm& AfterTm, const TUrlTy& UrlTy, const TQuoteBs& QtBs) const {
  const TSecTm After = AfterTm.Round(TmUnit);
  THash<TSecTm, TInt> TmFqH;
  for(int i=0; i<TmUrlCntV.Len(); i++) {
    if (TmUrlCntV[i].Tm().Round(TmUnit) >= After && (UrlTy==utUndef || QtBs.GetUrlTy(TmUrlCntV[i].UrlId())==UrlTy)) {
      TmFqH.AddDat(TmUrlCntV[i].Tm().Round(TmUnit)) += TmUrlCntV[i].Cnt(); } //!!!
      //TmFqH.AddDat(TmUrlCntV[i].Tm().Round(TmUnit)) += 1; }
  }
  if (TmFqH.Empty()) {
    return GetPeakTm(TmUnit, TSecTm(1)); }
  TmFqH.SortByDat(false);
  //for (int i = 0; i < TmFqH.Len(); i++) {
  //  printf("  %d\t%s\n", TmFqH[i], TmFqH.GetKey(i).GetYmdTmStr().CStr()); }
  return TmFqH.GetKey(0);
}

TSecTm TQuote::GetMeanTm(const TTmUnit& TmUnit, const TUrlTy& UrlTy, const TQuoteBs& QtBs, const TSecTm& AfterTm) const {
  const TSecTm After = AfterTm.Round(TmUnit);
  double MeanTm = 0;
  int Cnt = 0;
  for(int i=0; i<TmUrlCntV.Len(); i++) {
    if (TmUrlCntV[i].Tm().Round(TmUnit) >= After && (UrlTy==utUndef || QtBs.GetUrlTy(TmUrlCntV[i].UrlId())==UrlTy)) {
      MeanTm += TmUrlCntV[i].Tm().Round(TmUnit) * TmUrlCntV[i].Cnt();
      Cnt += TmUrlCntV[i].Cnt();
    }
  }
  return TSecTm(uint(MeanTm/double(Cnt))).Round(TmUnit);
}

TSecTm TQuote::GetMedianTm(const TTmUnit& TmUnit, const TUrlTy& UrlTy, const TQuoteBs& QtBs, const TSecTm& AfterTm) const {
  const TSecTm After = AfterTm.Round(TmUnit);
  TMom Mom;
  for(int i=0; i<TmUrlCntV.Len(); i++) {
    if (TmUrlCntV[i].Tm().Round(TmUnit) >= After && (UrlTy==utUndef || QtBs.GetUrlTy(TmUrlCntV[i].UrlId())==UrlTy)) {
      Mom.Add(TmUrlCntV[i].Tm().Round(TmUnit).GetAbsSecs(), TmUrlCntV[i].Cnt());
    }
  }
  Mom.Def();
  return TSecTm(uint(Mom.GetMedian())).Round(TmUnit);
}

bool TQuote::IsSinglePeak(const TTmUnit& TmUnit, const TSecTm& AfterTm, const TUrlTy& UrlTy, const TQuoteBs& QtBs) const {
  const double PeakThresh = 0.4;
  TTmFltPrV FqOtV, SmoothV;
  GetFqOt(FqOtV, TmUnit, UrlTy, QtBs);
  TQuote::GetSmoothFqOt(SmoothV, FqOtV, TmUnit, 48, 1.2, AfterTm); // smoot the Fq vector
  SmoothV.Swap(FqOtV);
  double MaxVal=0;
  int maxI=0, maxL=0, maxR=0;
  for (int i = 0; i < FqOtV.Len(); i++) {
    if (MaxVal < FqOtV[i].Val2) {
      MaxVal = FqOtV[i].Val2;  maxI=i; }
  }
  MaxVal *= PeakThresh;
  for (maxL = maxI; maxL>0 && FqOtV[maxL].Val2 > MaxVal; maxL--) { }//printf ("  %d\t%f\n", maxL, FqOtV[maxL].Val2);}
  for (maxR = maxI; maxR<FqOtV.Len() && FqOtV[maxR].Val2 > MaxVal; maxR++) { }//printf ("    %d\t%f\n", maxR, FqOtV[maxR].Val2);}
  if (maxR-maxL > 100) { return false; }
  for (int i = maxR; i < FqOtV.Len(); i++) {
    // has to monotonically decrease or be below 50% of max
    if (FqOtV[i].Val2 > MaxVal) { return false; }
  }
  // has to monotonically increase or be below 50% of max
  for (int i = maxL; i >= 0; i--) {
    if (FqOtV[i].Val2 > MaxVal) { return false; }
  }
  return true;
  /*
  TTmFltPrV FqOtV, SmoothV;
  GetFqOt(FqOtV, TmUnit, UrlTy, QtBs);
  TQuote::GetSmoothFqOt(SmoothV, FqOtV, TmUnit, 48, 1.2, AfterTm); // smoot the Fq vector
  SmoothV.Swap(FqOtV);
  double MaxVal=0;
  int maxI=0;
  for (int i = 0; i < FqOtV.Len(); i++) {
    if (MaxVal < FqOtV[i].Val2) { MaxVal = FqOtV[i].Val2; maxI=i; }
  }
  bool RetVal=true;
  MaxVal *= PeakThresh;
  for (int i = maxI+1; i < FqOtV.Len(); i++) {
    if (FqOtV[i-1].Val2*1.10 < FqOtV[i].Val2 && FqOtV[i].Val2 > MaxVal) { // has to monotonically decrease or be below 50% of max)
      //printf("\n%d:\n  %d]+  prev val: %f\n  next val: %f\n  macVal: %f\n", clus+1, i, FqOtV[i-1].Val2, FqOtV[i].Val2, MaxVal);
      RetVal=false;  break;
    }
  }
  for (int i = maxI-1; i >= 0; i--) {
    if (FqOtV[i].Val2 > FqOtV[i+1].Val2*1.10 && FqOtV[i].Val2 > MaxVal) { // has to monotonically decrease or be below 50% of max)
      //printf("\n%d:\n  %d]-  prev val: %f\n  next val: %f\n  macVal: %f\n", clus+1, i, FqOtV[i+1].Val2, FqOtV[i].Val2, MaxVal);
      RetVal=false;  break;
    }
  }
  //TGnuPlot::PlotValV(FqOtV, TStr::Fmt("clu-%02d", ++clus), RetVal?"SINGLE PEAK":"MULTIPLE PEAKS");
  return RetVal;*/
}

void TQuote::GetFqOt(TTmFltPrV& RawFqOtV, const TTmUnit& TmUnit) const {
  THash<TSecTm, TFlt> TmCntH;
  for (int i = 0; i < TmUrlCntV.Len(); i++) {
    TmCntH.AddDat(TmUrlCntV[i].Tm().Round(TmUnit)) += TmUrlCntV[i].Cnt();
    //TmCntH.AddDat(TmUrlCntV[i].Tm().Round(TmUnit)) += 1;  //!!!
  }
  TmCntH.SortByKey();
  TmCntH.GetKeyDatPrV(RawFqOtV);
}

void TQuote::GetFqOt(TTmFltPrV& RawFqOtV, const TTmUnit& TmUnit, const TUrlTy& UrlTy, const TQuoteBs& QtBs) const {
  THash<TSecTm, TFlt> TmCntH;
  for (int i = 0; i < TmUrlCntV.Len(); i++) {
    if (UrlTy==utUndef || QtBs.GetUrlTy(TmUrlCntV[i].UrlId())==UrlTy) {
      TmCntH.AddDat(TmUrlCntV[i].Tm().Round(TmUnit)) += TmUrlCntV[i].Cnt();
      //TmCntH.AddDat(TmUrlCntV[i].Tm().Round(TmUnit)) += 1;  //!!!
    }
  }
  TmCntH.SortByKey();
  TmCntH.GetKeyDatPrV(RawFqOtV);
}

void TQuote::GetFqOt(TTmFltPrV& RawFqOtV, const TTmUnit& TmUnit, const TSecTm& BegTm, const TSecTm& EndTm) const {
  const TSecTm RBegTm = BegTm.Round(TmUnit);
  const TSecTm REndTm = EndTm.Round(TmUnit);
  THash<TSecTm, TFlt> TmCntH;
  for (int i = 0; i < TmUrlCntV.Len(); i++) {
    const TSecTm Tm = TmUrlCntV[i].Tm().Round(TmUnit);
    if (Tm >= RBegTm && Tm <= REndTm) {
      TmCntH.AddDat(Tm) += TmUrlCntV[i].Cnt();
      //TmCntH.AddDat(Tm) += 1; //!!!
    }
  }
  TmCntH.SortByKey();
  TmCntH.GetKeyDatPrV(RawFqOtV);
}

void TQuote::GetFqOt(TTmFltPrV& RawFqOtV, const TTmUnit& TmUnit, const TSecTm& BegTm, const TSecTm& EndTm, const TUrlTy& UrlTy, const TQuoteBs& QtBs) const {
  const TSecTm RBegTm = BegTm.Round(TmUnit);
  const TSecTm REndTm = EndTm.Round(TmUnit);
  THash<TSecTm, TFlt> TmCntH;
  for (int i = 0; i < TmUrlCntV.Len(); i++) {
    const TSecTm Tm = TmUrlCntV[i].Tm().Round(TmUnit);
    if (Tm >= RBegTm && Tm <= REndTm && (UrlTy==utUndef || QtBs.GetUrlTy(TmUrlCntV[i].UrlId())==UrlTy)) {
      TmCntH.AddDat(Tm) += TmUrlCntV[i].Cnt();
      //TmCntH.AddDat(Tm) += 1; //!!!
    }
  }
  TmCntH.SortByKey();
  TmCntH.GetKeyDatPrV(RawFqOtV);
}

void TQuote::GetSmoothFqOt(TTmFltPrV& FqOtV, const TTmUnit& TmUnit, const int& WndSz, const double& Smooth, const TSecTm& BegTm, const TSecTm& EndTm) const {
  TTmFltPrV RawFqOtV;
  GetFqOt(RawFqOtV, TmUnit, BegTm, EndTm);
  GetSmoothFqOt(FqOtV, RawFqOtV, TmUnit, WndSz, Smooth, BegTm, EndTm);
}

void TQuote::GetSmoothFqOt(TTmFltPrV& SmoothFqOtV, const TTmFltPrV& RawFqOtV, const TTmUnit& TmUnit, const int& WndSz, const double& Smooth, const TSecTm& BegTm, const TSecTm& EndTm) {
  const int TmUnitSecs = TTmInfo::GetTmUnitSecs(TmUnit);
  const int HalfWndSecs = (WndSz/2)*TmUnitSecs;
  double FullNormConst = 1;
  for (int j = 1; j <= WndSz/2; j++) {
    FullNormConst += 2*pow(Smooth, -j); }
  THash<TSecTm, TFlt> TmCntH;
  for (int i = 0; i < RawFqOtV.Len(); i++) {
    const TSecTm Tm = RawFqOtV[i].Val1;
    double NormConst = FullNormConst;
    // calculate new normalizing constant if the window does not fit
    if (Tm < BegTm+HalfWndSecs || Tm+HalfWndSecs > EndTm) {
      NormConst = 1;
      for (int j = 1; j <= WndSz/2; j++) {
        if (Tm >= BegTm+j*TmUnitSecs) { NormConst += pow(Smooth, -j);  }
        if (Tm+j*TmUnitSecs <= EndTm) { NormConst += pow(Smooth, -j);  }
    } }
    const double NormFq = RawFqOtV[i].Val2 / NormConst;
    for (int j = 1; j <= WndSz/2; j++) {
      const int Off = j*TmUnitSecs;
      if (Tm+Off <= EndTm) {
        TmCntH.AddDat(TSecTm(Tm+Off).Round(TmUnit)) += NormFq * pow(Smooth, -j); }
      if (Tm >= BegTm + Off) {
        TmCntH.AddDat(TSecTm(Tm-Off).Round(TmUnit)) += NormFq * pow(Smooth, -j); }
    }
    TmCntH.AddDat(Tm.Round(TmUnit)) += NormFq;
  }
  TmCntH.SortByKey();
  TmCntH.GetKeyDatPrV(SmoothFqOtV);
}

TStr TQuote::GetDesc() const {
  return TStr::Fmt("#:%d U:%d  %s", GetFq(), GetUrls(), QtStr.CStr());
}

void TQuote::PlotOverTm(const TStr& OutFNm) {
  TFltFltH HrCntH;
  TmUrlCntV.Sort();
  if (TmUrlCntV.Empty()) { return; }
  for (int i = 0; i < TmUrlCntV.Len(); i++) {
    double Hr = TSecTm(TmUrlCntV[i].Tm()-TmUrlCntV[0].Tm()).Round(tmu6Hour)/(24*3600.0);
    /*for (int j = 1; j < 6; j++) {
      HrCntH.AddDat(Hr+j) += TmUrlCntV[i].Cnt() * pow(1.2,-j);
      if (Hr-j>0) HrCntH.AddDat(Hr-j) += TmUrlCntV[i].Cnt() * pow(1.2,-j);
    }*/
    HrCntH.AddDat(Hr) += TmUrlCntV[i].Cnt();
  }
  HrCntH.SortByKey();
  TGnuPlot::PlotValCntH(HrCntH, OutFNm, TStr::Fmt("%d occurences, %d urls: %s", GetFq(), GetUrls(), QtStr.CStr()),
    TStr::Fmt("Time [days] from %s", TmUrlCntV[0].Tm().GetYmdTmStr().CStr()), "Frequency");
}

void TQuote::LoadQtV(const TStr& InFNm, TVec<TQuote>& QtV) {
  PSIn SIn = TZipIn::IsZipFNm(InFNm) ? TZipIn::New(InFNm) : TFIn::New(InFNm);
  QtV.Clr(false);
  while (! SIn->Eof()) {
    QtV.Add();
    QtV.Last().Load(*SIn);
  }
}

/////////////////////////////////////////////////
// Quote Mutations
TQuoteBs::TQuoteBs(TSIn& SIn) : StrQtIdH(SIn), QuoteH(SIn), UrlInDegH(SIn), UrlTyH(SIn) {
  if (! SIn.Eof()) { ClustQtIdVH.Load(SIn); }
  TIntSet CIdSet;
  for (int q = 0; q < ClustQtIdVH.Len(); q++) {
    CIdSet.AddKey(ClustQtIdVH.GetKey(q));
    for (int c = 0; c < ClustQtIdVH[q].Len(); c++) {
      CIdSet.AddKey(ClustQtIdVH[q][c]); } // make sure there are no duplicates
    CIdSet.GetKeyV(ClustQtIdVH[q]);
    ClustQtIdVH[q].Sort();
    CIdSet.Clr(false);
  }
  Dump();
}

void TQuoteBs::Save(TSOut& SOut) const {
  StrQtIdH.Save(SOut);
  QuoteH.Save(SOut);
  UrlInDegH.Save(SOut);
  UrlTyH.Save(SOut);
  ClustQtIdVH.Save(SOut);
}

PQuoteBs TQuoteBs::New() {
  return PQuoteBs(new TQuoteBs());
}

PQuoteBs TQuoteBs::Load(TSIn& SIn) {
  printf("loading %s...", SIn.GetSNm().CStr());
  return PQuoteBs(new TQuoteBs(SIn));
}

void TQuoteBs::GetQtIdV(TIntV& QtIdV) const {
  QtIdV.Clr();
  for (int q = 0; q < QuoteH.Len(); q++) {
    QtIdV.Add(QuoteH.GetKey(q)); }
}

TUrlTy TQuoteBs::GetUrlTy(const int& UrlId) const {
  if (UrlTyH.IsKey(UrlId)) {
    return (TUrlTy) UrlTyH.GetDat(UrlId).Val; } // return type
  return utBlog; // blog by default
}

void TQuoteBs::SetUrlTy(const TStr& InFNm, const TUrlTy& SetTy) {
  printf("Set url type\n");
  //UrlTyH.Clr(); //!!!
  TStrHash<TIntV> DomUrlV; // urls from each domain
  for (int q = 0; q < Len(); q++) {
    const TQuote::TTmUrlCntV& V = GetQtN(q).TmUrlCntV;
    for (int u = 0; u < V.Len(); u++) {
      const TChA Url = GetStr(V[u].UrlId());
      DomUrlV.AddDat(TStrUtil::GetDomNm(Url).CStr()).Add(V[u].UrlId());
    }
  }
  printf("  %d domains\n", DomUrlV.Len());
  TStrV TyUrlV; TStr Ln;
  if (! TFile::Exists(InFNm)) {
    printf("!!! %s does not exist\n", InFNm.CStr());
    return;
  }
  for (TFIn FIn(InFNm); FIn.GetNextLn(Ln); ) { TyUrlV.Add(Ln.GetTrunc()); }
  printf("  %d domains with type label loaded\n", TyUrlV.Len());
  int NDomSet=0, NUrlSet=0;
  TExeTm ExeTm;
  for (int d = 0; d < DomUrlV.Len(); d++) {
    for (int u = 0; u < TyUrlV.Len(); u++) {
      if (strstr(DomUrlV.GetKey(d), TyUrlV[u].CStr()) != NULL) {
        const TIntV& urlV = DomUrlV[d];
        for (int i = 0; i < urlV.Len(); i++) {
          UrlTyH.AddDat(urlV[i], SetTy); }
        NDomSet++;  NUrlSet+=urlV.Len();
        break;
      }

    }
    if (d % 1000 == 0) { printf("  %d/%d: labeled %d doms, %d urls [%s]\n", d, DomUrlV.Len(), NDomSet, NUrlSet, ExeTm.GetStr()); }
  }
  printf("  labeled %d doms, %d urls [%s]\n", NDomSet, NUrlSet, ExeTm.GetStr());
  printf("  %d total labeled urls\n", UrlTyH.Len());
}

bool AppearsAt(const TIntSet& UrlSet, const TQuote& Q) {
  for (int u = 0; u < Q.GetUrls(); u++) {
    if (UrlSet.IsKey(Q.GetUrlId(u))) { return true; }
  }
  return false;
}

// also merges quotes that are in clusters into a single quote
void TQuoteBs::GetQtIdVByFq(TIntV& QtIdV, const int& MinWrdLen, const int& MinQtFq, const bool& OnlyClustRoots, const TStr& HasWord, const TStr& AppearsAtUrl, const TUrlTy& OnlyCountTy, const TSecTm& BegTm, const TSecTm& EndTm) const {
  printf("Get top quotes from %d quotes\n", QuoteH.Len());
  printf("  %s -- %s\n", BegTm.GetYmdTmStr().CStr(), EndTm.GetYmdTmStr().CStr());
  TIntPrV FqQtIdV;
  int words=0, minFq=0, hasWord=0;
  TIntSet CSet, QSet;
  //TStrHash<TInt> BlackListH; TFIn FIn("quote_blacklist.txt");
  TIntSet UrlSet;
  if (! AppearsAtUrl.Empty()) {
    for (int q = 0; q < QuoteH.Len(); q++) {
      const TQuote& Q = GetQtN(q);
      TQuote::TTmUrlCntV TmUrlCntV(Q.GetUrls(), 0);
      for (int u = 0; u < Q.GetUrls(); u++) {
        if (UrlSet.IsKey(Q.GetUrlId(u))) {
          TmUrlCntV.Add(Q.TmUrlCntV[u]);
          continue; }
        if (strstr(GetStr(Q.GetUrlId(u)), AppearsAtUrl.CStr())!=NULL) {
          TmUrlCntV.Add(Q.TmUrlCntV[u]);
          UrlSet.AddKey(Q.GetUrlId(u)); } // add all urls
      }
      TQuote* QPt = (TQuote*) &(QuoteH[q]);
      QPt->TmUrlCntV = TmUrlCntV; // keep only scientific site appearances
    }
  }
  printf("done.");
  //for (TStr Line; FIn.GetNextLn(Line); ) { Line.ToTrunc(); BlackListH.AddKey(Line.GetLc()); }
  for (int q = 0; q < QuoteH.Len(); q++) {
    const TQuote& Qt = GetQtN(q);
    //if (BlackListH.IsKey(Qt.QtStr)) { continue; } // skip black list
    if (TStrUtil::CountWords(Qt.QtStr.CStr()) < MinWrdLen) { words++;  continue; }
    if ((! HasWord.Empty()) && Qt.QtStr.SearchStr(HasWord)==-1) { hasWord++;  continue; }
    int Fq = 0;
    if (OnlyClustRoots && Qt.GetTy() == qtRoot) {
      IAssert(Qt.GetCId() == GetQtId(q));
      IAssert(! CSet.IsKey(Qt.GetCId()));  CSet.AddKey(Qt.GetCId());
      if ((! HasWord.Empty()) && GetQt(GetCentrQtId(Qt.GetCId())).GetStr().SearchStr(HasWord)==-1) { hasWord++; continue; } // the quote we display has to have a word
      //Fq = GetClustFq(Qt.GetCId(), OnlyCountTy); // get frequency of all quotes in the cluster
      //const int CentrQtId = GetCentrQtId(Qt.GetCId());
      TQuote CentrQt;  GetMergedClustQt(Qt.GetCId(), CentrQt, true); // get frequency only  after the centroid appears
      if (! UrlSet.Empty() && ! AppearsAt(UrlSet, CentrQt)) { continue; }
      Fq = CentrQt.GetFq(BegTm, EndTm, OnlyCountTy, *this);
      if (Fq < MinQtFq) { minFq++; continue; }
      const int Doms = CentrQt.GetDoms(*this);
      if (Doms < 3 || 5*Doms < CentrQt.GetUrls()) { continue; }
    }
    else {
      if (! UrlSet.Empty() && ! AppearsAt(UrlSet, Qt)) { continue; }
      Fq = Qt.GetFq(BegTm, EndTm, OnlyCountTy, *this);
      if (Fq < MinQtFq) { minFq++; continue; }
      const int Doms = Qt.GetDoms(*this);
      if (Doms < 3 || 4*Doms < Qt.GetUrls()) { continue; }
    }
    IAssert(! QSet.IsKey(GetQtId(q))); QSet.AddKey(GetQtId(q));
    FqQtIdV.Add(TIntPr(Fq, GetQtId(q)));
  }
  printf("  skip %d : word len < %d\n", words, MinWrdLen);
  printf("  skip %d : qt fq < %d\n", minFq, MinQtFq);
  if (! HasWord.Empty()) { printf("  skip %d : not containing '%s'\n", hasWord, HasWord.CStr()); }
  printf("  remaining %d quotes\n", FqQtIdV.Len());
  FqQtIdV.Sort(false);
  QtIdV.Clr(false);
  TIntSet SeenSet;
  for (int i = 0; i < FqQtIdV.Len(); i++) {
    if (i < 100) { printf(" fq:%d", FqQtIdV[i].Val1()); }
    const int qid = FqQtIdV[i].Val2;
    if (! SeenSet.IsKey(qid)) {
      QtIdV.Add(qid);
      SeenSet.AddKey(qid);
      // if we get a root, don't also get the children
      if (GetQt(qid).GetTy()==qtRoot || GetQt(qid).GetTy()==qtCentr) {
        const TIntV& ClustV = GetClust(GetQt(qid).GetCId());
        for (int c = 0; c < ClustV.Len(); c++) { SeenSet.AddKey(ClustV[c]); }
      }
    }
  }
  printf("  return %d quotes\n", QtIdV.Len());
}

int AppearsAtDom(const TQuote& CentrQt, const TIntSet& GoodDom, const TIntH& UrlDomH) {
  int DomCnt = 0;
  for (int u = 0; u < CentrQt.GetUrls(); u++) {
    const int U = CentrQt.GetUrlId(u);
    IAssert(UrlDomH.IsKey(U));
    if (GoodDom.IsKey(UrlDomH.GetDat(U))) { // appears at good domain
      DomCnt++; }
  }
  return DomCnt;
}

void TQuoteBs::GetQtIdVByFq(TIntV& QtIdV, const int& MinWrdLen, const int& MinQtFq, const TStrV& FromDomains,
                            const bool& OnlyClustRoots, const TStr& HasWord, int MinDoms) const {
  printf("Get top quotes from %d quotes appearing at %d domains\n", QuoteH.Len(), FromDomains.Len());
  TIntPrV FqQtIdV;
  TIntH UrlDomH;
  TStrSet DomainSet;
  TIntSet QSet;
  for (int q = 0; q < QuoteH.Len(); q++) {
    const TQuote& Q = GetQtN(q);
    for (int u = 0; u < Q.GetUrls(); u++) {
      const int U = Q.GetUrlId(u);
      const int D = DomainSet.AddKey(TStrUtil::GetDomNm2(GetStr(U)));
      UrlDomH.AddDat(U, D);
    }
  }
  TIntSet GoodDom;
  for (int d = 0; d < DomainSet.Len(); d++) {
    for (int f = 0; f < FromDomains.Len(); f++) {
      if (DomainSet[d].SearchStr(FromDomains[f]) != -1) {
        GoodDom.AddKey(d); break; }
    }
  }
  printf("%d total domains\n", DomainSet.Len());
  printf("%d from domains\n", FromDomains.Len());
  printf("%d good domains\n", GoodDom.Len());
  printf("done.\n\n");
  const TSecTm BegTm(1), EndTm(TSecTm::GetCurTm());
  for (int q = 0; q < QuoteH.Len(); q++) {
    const TQuote& Qt = GetQtN(q);
    if (TStrUtil::CountWords(Qt.QtStr.CStr()) < MinWrdLen) { continue; }
    if ((! HasWord.Empty()) && Qt.QtStr.SearchStr(HasWord)==-1) { continue; }
    int Fq = 0;
    if (Qt.GetTy() == qtRoot) {
      TQuote CentrQt;
      GetMergedClustQt(Qt.GetCId(), CentrQt, true); // get frequency only after the centroid appears
      if (AppearsAtDom(CentrQt, GoodDom, UrlDomH) < MinDoms) { continue; }
      Fq = CentrQt.GetFq(BegTm, EndTm, utUndef, *this);
      if (Fq < MinQtFq) { continue; }
      const int Doms = CentrQt.GetDoms(*this);
      if (Doms < 3 || 5*Doms < CentrQt.GetUrls()) { continue; }
    }
    else if (! OnlyClustRoots) {
      if (AppearsAtDom(Qt, GoodDom, UrlDomH) < MinDoms) { continue; }
      Fq = Qt.GetFq(BegTm, EndTm, utUndef, *this);
      if (Fq < MinQtFq) { continue; }
      const int Doms = Qt.GetDoms(*this);
      if (Doms < 3 || 4*Doms < Qt.GetUrls()) { continue; }
    }
    IAssert(! QSet.IsKey(GetQtId(q))); QSet.AddKey(GetQtId(q));
    FqQtIdV.Add(TIntPr(Fq, GetQtId(q)));
  }
  printf("  remaining %d quotes\n", FqQtIdV.Len());
  FqQtIdV.Sort(false);
  QtIdV.Clr(false);
  TIntSet SeenSet;
  for (int i = 0; i < FqQtIdV.Len(); i++) {
    const int qid = FqQtIdV[i].Val2;
    if (! SeenSet.IsKey(qid)) {
      QtIdV.Add(qid);
      SeenSet.AddKey(qid);
      // if we get a root, don't also get the children
      if (GetQt(qid).GetTy()==qtRoot || GetQt(qid).GetTy()==qtCentr) {
        const TIntV& ClustV = GetClust(GetQt(qid).GetCId());
        for (int c = 0; c < ClustV.Len(); c++) { SeenSet.AddKey(ClustV[c]); }
      }
    }
  }
  printf("  return %d quotes\n", QtIdV.Len());
}

void TQuoteBs::GetQtIdVByTm(const int& WndSzHr, const int& StepHr, const int& MinWrdLen, const int& MinQtFq, const int& TakePerStep) const {
  const TTmUnit TmUnit = tmu4Hour;
  TSecTm MinTm, MaxTm;  GetMinMaxTm(MinTm, MaxTm);
  MinTm=MinTm.Round(TmUnit); MaxTm=MaxTm.Round(TmUnit);
  printf("Dataset span %s -- %s\n", MinTm.GetStr().CStr(), MaxTm.GetStr().CStr());
  printf("  time window %dh, step size %dh, min wrd len %d, take top %d per step\n", WndSzHr, StepHr, MinWrdLen, TakePerStep);
  TIntSet TopQtIdSet;
  TIntV TopQtIdV;
  int cnt=0;
  FILE *F = fopen("top_qts_per_time_unit.txt", "wt");
  for (TSecTm Tm=MinTm; Tm <= MaxTm; Tm += StepHr*3600, cnt++) {
    GetQtIdVByFq(TopQtIdV, MinWrdLen, MinQtFq, true, "", "", utUndef, TSecTm(Tm), TSecTm(Tm+StepHr*3600));
    fprintf(F,"week of: %s\n", Tm.GetYmdTmStr().CStr());
    for (int q = 0, j=0; q < TopQtIdV.Len() && j<3; q++) {
      if (! TopQtIdSet.IsKey(TopQtIdV[q])) {
        fprintf(F, "%s\t%d\n", GetQt(GetCentrQtId(TopQtIdV[q])).GetStr().CStr(), GetClustFq(TopQtIdV[q]));
        j++; }
      TopQtIdSet.AddKey(TopQtIdV[q]);
    }
    fprintf(F, "\n");
  }
  fclose(F);
  printf("  done %d quotes\n", TopQtIdSet.Len());
  //TIntV QtIdV;
  //TopQtIdSet.GetKeyV(QtIdV);
  //return QtIdV;
  //printf("  return %d quotes\n", QtIdV.Len());
}

void TQuoteBs::GetCIdVByFq(TIntV& CIdV, const int& MinClFq, const TStr& RootHasWord, const TUrlTy& OnlyCountTy, const bool& OnlyAfterRoot, const TSecTm& BegTm, const TSecTm& EndTm) const {
  printf("Get top clusters from %d clusters\n", GetClusts());
  TIntPrV FqCIdV;
  for (int c = 0; c < GetClusts(); c++) {
    const int CId = GetCId(c);
    if ( ! IsQtId(CId)) { printf("!!! %d:%d\n", c, CId); continue; }
    IAssert(GetQt(CId).GetTy()==qtRoot);
    const int CentrQId = GetCentrQtId(CId);
    if (CentrQId == -1) { continue; }
    if ((! RootHasWord.Empty()) && GetQt(CentrQId).GetStr().SearchStr(RootHasWord)==-1) { continue; } // the quote we display has to have a word
    TQuote CentrQt;  GetMergedClustQt(CId, CentrQt, OnlyAfterRoot); // get frequency only after the centroid appears
    const int Fq = CentrQt.GetFq(BegTm, EndTm, OnlyCountTy, *this);
    if (Fq < MinClFq) { continue; }
    FqCIdV.Add(TIntPr(Fq, CId));
  }
  printf("  remaining %d clusters\n", FqCIdV.Len());
  FqCIdV.Sort(false);
  CIdV.Clr(false);
  for (int i = 0; i < FqCIdV.Len(); i++) {
    CIdV.Add(FqCIdV[i].Val2); }
}

void TQuoteBs::GetMinMaxTm(TSecTm& MinTm, TSecTm& MaxTm) const {
  MinTm = MaxTm = TSecTm();
  for (int q = 0; q < QuoteH.Len(); q++) {
    const TQuote::TTmUrlCntV& V = QuoteH[q].TmUrlCntV;
    if (V.Empty()) { continue; }
    if (! MinTm.IsDef() || MinTm > V[0].Tm()) {
      MinTm = V[0].Tm(); }
    if (! MaxTm.IsDef() || MaxTm < V[0].Tm()) {
      MaxTm = V[0].Tm(); }
  }
}

void TQuoteBs::GetMinMaxTm(const TIntV& QtIdV, TSecTm& MinTm, TSecTm&MaxTm) const {
  MinTm = MaxTm = TSecTm();
  for (int q = 0; q < QtIdV.Len(); q++) {
    const TQuote::TTmUrlCntV& V = GetQt(QtIdV[q]).TmUrlCntV;
    if (V.Empty()) { continue; }
    if (! MinTm.IsDef() || MinTm > V[0].Tm()) {
      MinTm = V[0].Tm(); }
    if (! MaxTm.IsDef() || MaxTm < V[0].Tm()) {
      MaxTm = V[0].Tm(); }
  }
}

// for each quote in QtIdV get the most representative url containing the quote
void TQuoteBs::GetQtPageUrl(const TIntV& QtIdV, TIntH& QtUrlIdH) const {
  THash<TChA, TInt> DomQtCntH;
  for (int q = 0; q < Len(); q++) {
    const TQuote& Q = GetQtN(q);
    for (int u = 0; u < Q.GetUrls(); u++) {
      DomQtCntH.AddDat(TStrUtil::GetDomNm(GetStr(Q.TmUrlCntV[u].UrlId()))) += Q.TmUrlCntV[u].Cnt(); }
  }
  // stop-list
  const TStr StopList = "blog.myspace.com|www.newsmeat.com|us.rd.yahoo.com|www.startribune.com|"
    "news.originalsignal.com|uk.news.yahoo.com|ap.google.com|www.cnn.com|www.opednews.com";
  TStrV StopListV; StopList.SplitOnAllCh('|', StopListV);
  for (int s = 0; s < StopListV.Len(); s++) {
    DomQtCntH.AddDat(StopListV[s]) = 1; }
  DomQtCntH.SortByDat(false);
  QtUrlIdH.Clr(false);
  // find url from best domain for each quote
  for (int q = 0; q < QtIdV.Len(); q++) {
    const TQuote& Q = GetQt(QtIdV[q]);
    int DomFq=0, BestUrlId=0;
    for (int u = 0; u < Q.TmUrlCntV.Len(); u++) {
      const TChA Dom = TStrUtil::GetDomNm(GetStr(Q.TmUrlCntV[u].UrlId()));
      if (DomFq < DomQtCntH.GetDat(Dom)) {
        DomFq = DomQtCntH.GetDat(Dom);
        BestUrlId = Q.TmUrlCntV[u].UrlId();
      }
    }
    QtUrlIdH.AddDat(QtIdV[q], BestUrlId);
  }
  //FILE *F = fopen("quotes_per_domain.tab", "wt");
  //for (int d = 0; d < DomQtCntH.Len(); d++) {
  //fprintf(F, "%s\t%d\n", DomQtCntH.GetKey(d).CStr(), DomQtCntH[d]); }
  //fclose(F);
}

// quote Id is preserved
void TQuoteBs::AddQuote(const TQuote& Quote, const TQuoteBs& CurQtBs) {
  const int QtId = CurQtBs.GetQtId(Quote.GetStr().CStr());
  TQuote& Qt = QuoteH.AddDat(QtId);
  StrQtIdH.AddDat(Quote.GetStr().CStr(), QtId);
  Qt.QtCIdTy = TQuote::TQtIdTy(Quote.GetCId(), Quote.GetTy());
  Qt.QtStr = Quote.QtStr;
  Qt.TmUrlCntV = Quote.TmUrlCntV;
  for (int u = 0; u < Qt.TmUrlCntV.Len(); u++) {
    Qt.TmUrlCntV[u].SetUrlId(AddStr(CurQtBs.GetStr(Quote.TmUrlCntV[u].UrlId())));
  }
}

void TQuoteBs::AddQuote(const TVec<TChA>& QuoteV, const TVec<TChA>& LinkV, const TChA& PostUrlStr, const TSecTm& PubTm, const int& MinQtWrdLen) {
  TIntH QtCntH; // qid --> cnt
  for (int q = 0; q < QuoteV.Len(); q++) {
    if (TStrUtil::CountWords(QuoteV[q].CStr()) < MinQtWrdLen) { continue; } // skip if too short
    int QtId = GetQtId(QuoteV[q].CStr());
    if (QtId == -1) { // new quote
      IAssert(! IsStr(QuoteV[q].CStr()));
      QtId = QuoteH.Len();
      TQuote& Qt = QuoteH.AddDat(QtId);
      Qt.QtCIdTy = TQuote::TQtIdTy(QtId, qtQuote);
      Qt.QtStr = QuoteV[q];
      StrQtIdH.AddDat(QuoteV[q].CStr(), QtId);
    } else { IAssert(IsStr(QuoteV[q].CStr())); }
    IAssert(IsQtId(QtId));
    QtCntH.AddDat(QtId) += 1; // count appearances of each quote
  }
  if (QtCntH.Len() > 0) {
    const int PostUrlId = AddStr(PostUrlStr);
    for (int i = 0; i < QtCntH.Len(); i++) {
      QuoteH.GetDat(QtCntH.GetKey(i)).TmUrlCntV.Add(TQuote::TTmUrlCnt(PubTm, PostUrlId, QtCntH[i]));
    }
  }
  // build in-link counts
  const TChA PostDomain = TStrUtil::GetDomNm(PostUrlStr);
  for (int l = 0; l < LinkV.Len(); l++) {
    const TChA& Url = LinkV[l];
    if (TStrUtil::GetDomNm(Url) == PostDomain) { continue; } // link inside same domain
    if (IsStr(Url.CStr())) { UrlInDegH.AddDat(GetStrId(Url.CStr())) += 1; }
  }
}

PQuoteBs TQuoteBs::GetQuoteBs(const TIntV& QtIdV) const {
  PQuoteBs _NewQtBs = TQuoteBs::New();
  TQuoteBs& NewQtBs = *_NewQtBs;
  // add quotes
  for (int q = 0; q < QtIdV.Len(); q++) {
    const TQuote& Qt = GetQt(QtIdV[q]);
    NewQtBs.AddQuote(Qt, *this);
  }
  // add urls
  for (int u = 0; u < UrlInDegH.Len(); u++) {
    const char* UrlStr = GetStr(UrlInDegH.GetKey(u));
    if (NewQtBs.IsStr(UrlStr)) {
      NewQtBs.UrlInDegH.AddDat(NewQtBs.GetStrId(UrlStr), UrlInDegH[u]); }
  }
  // add url types
  for (int t = 0; t < UrlTyH.Len(); t++) {
    const char* UrlStr = GetStr(UrlTyH.GetKey(t));
    if (NewQtBs.IsStr(UrlStr)) {
      NewQtBs.UrlTyH.AddDat(NewQtBs.GetStrId(UrlStr), UrlTyH[t]); }
  }
  // add clusters
  for (int c = 0; c < GetClusts(); c++) {
    const int CId = GetCId(c);
    if ( ! IsQtId(CId)) { printf("!!! %d:%d\n", c, CId); continue; }
    const TIntV& CQtIdV = GetClust(CId);
    if (NewQtBs.IsQtId(CId)) {
      IAssert(! NewQtBs.ClustQtIdVH.IsKey(CId));
      TIntV& CIdV = NewQtBs.ClustQtIdVH.AddDat(CId);
      for (int i = 0; i < CQtIdV.Len(); i++) {
        if (NewQtBs.IsQtId(CQtIdV[i])) { CIdV.Add(CQtIdV[i]); }
      }
      IAssert(CIdV.Len() > 0);
    }
  }
  return _NewQtBs;
}

int TQuoteBs::GetQtsInClust() const {
  int qts = 0;
  for (int q = 0; q < ClustQtIdVH.Len(); q++) {
    qts += ClustQtIdVH[q].Len();
  }
  return qts;
}

int TQuoteBs::GetClustFq(const int& CId) const {
  int fq = 0;
  TIntSet S;
  const TIntV& ClustV = ClustQtIdVH.GetDat(CId);
  for (int c = 0; c < ClustV.Len(); c++) {
    fq += GetQt(ClustV[c]).GetFq();
    IAssert(! S.IsKey(ClustV[c]));
    S.AddKey((ClustV[c]));
  }
  return fq;
}

int TQuoteBs::GetClustFq(const int& CId, const TUrlTy& UrlTy) const {
  int fq = 0;
  const TIntV& ClustV = ClustQtIdVH.GetDat(CId);
  for (int c = 0; c < ClustV.Len(); c++) {
    fq += GetQt(ClustV[c]).GetFq(UrlTy, *this);
  }
  return fq;
}

int TQuoteBs::GetCentrQtId(const int& CId) const {
  return GetCentrQtId(GetClust(CId));
}

// find the representative quote of the cluster (most frequent quote in the clister)
int TQuoteBs::GetCentrQtId(const TIntV& ClustV) const {
  TIntPrV QtFqIdV;
  // no quotes of 6 or more words
  for (int c = 0; c < ClustV.Len(); c++) {
    if (! IsQtId(ClustV[c])) { continue; }
    const TQuote& Q = GetQt(ClustV[c]);
    QtFqIdV.Add(TIntPr(Q.GetUrls(), ClustV[c]));
  }
  if (QtFqIdV.Empty()) { return -1; }
  QtFqIdV.Sort(false);
  const TStr FqStr = GetQt(QtFqIdV[0].Val2).GetStr(); // most freq str or its superset
  for (int c = 0; c < QtFqIdV.Len(); c++) {
    IAssert(IsQtId(QtFqIdV[c].Val2));
    const TQuote& Q = GetQt(QtFqIdV[c].Val2);
    const int Words = TStrUtil::CountWords(Q.GetStr().CStr());
    if (Words >= 6 && Words < 50 && strstr(Q.GetStr().CStr(), FqStr.CStr())!=NULL) {
      return QtFqIdV[c].Val2; }
  }
  // just return most frequent quote
  return QtFqIdV[0].Val2;
}

// find cluster centroid (create artificial cluster)
void TQuoteBs::GetMergedClustQt(const int& CId, TQuote& NewQt, const bool& OnlyAfterBegTm) const {
  const TIntV& ClustV = GetClust(CId);
  GetMergedClustQt(ClustV, NewQt, OnlyAfterBegTm);
}

void TQuoteBs::GetMergedClustQt(const TIntV& ClustV, TQuote& NewQt, const bool& OnlyAfterBegTm) const {
  const int CentrQtId = GetCentrQtId(ClustV);
  const TSecTm BegTm = OnlyAfterBegTm ? GetClustBegTm(ClustV, CentrQtId) : TSecTm(1);
  THash<TPair<TSecTm, TInt>, TInt> TmUrlCntH;
  for (int c = 0; c < ClustV.Len(); c++) {
    const TQuote& Q = GetQt(ClustV[c]);
    //IAssert(Q.GetTy() == qtInClust);
    for (int u = 0; u < Q.TmUrlCntV.Len(); u++) {
      if (Q.TmUrlCntV[u].Tm() >= BegTm) {
        TmUrlCntH.AddDat(TPair<TSecTm, TInt>(Q.TmUrlCntV[u].Tm(), Q.TmUrlCntV[u].UrlId())) += Q.TmUrlCntV[u].Cnt(); }
    }
  }
  NewQt.QtCIdTy = TQuote::TQtIdTy(-1, qtCentr);
  NewQt.QtStr = GetQt(CentrQtId).GetStr(); // get most frequent string
  //NewQt.QtStr = GetQt(CId).GetStr();     // get longest string
  if (! TmUrlCntH.Empty()) {
    NewQt.TmUrlCntV.Gen(TmUrlCntH.Len(), 0);
    for (int u = 0; u < TmUrlCntH.Len(); u++) {
      NewQt.TmUrlCntV.Add(TQuote::TTmUrlCnt(TmUrlCntH.GetKey(u).Val1, TmUrlCntH.GetKey(u).Val2(), TmUrlCntH[u]()));
    }
    NewQt.TmUrlCntV.Sort();
  }
}

// given representative quote fird first time when CentrQtId or its super string first appeared
TSecTm TQuoteBs::GetClustBegTm(const int& CId, const int& CentrQtId) const {
  const TIntV& ClustV = GetClust(CId);
  return GetClustBegTm(ClustV, CentrQtId);
}

TSecTm TQuoteBs::GetClustBegTm(const TIntV& ClustV, const int& CentrQtId) const {
  TStrHash<TInt> StrH;
  TIntV CntrWIdV, WIdV;
  int WIdV1Start, WIdV2Start, SkipId;
  TStrUtil::GetAddWIdV(StrH, GetQt(CentrQtId).GetStr().CStr(), CntrWIdV);
  TSecTm BegTm = TSecTm::GetCurTm();
  for (int c = 0; c < ClustV.Len(); c++) {
    TStrUtil::GetAddWIdV(StrH, GetQt(ClustV[c]).GetStr().CStr(), WIdV);
    if (WIdV.Len() < CntrWIdV.Len()) { continue; }
    const int Overlap = LongestCmnSubSq(CntrWIdV, WIdV, WIdV1Start, WIdV2Start, SkipId);
    if (Overlap >= CntrWIdV.Len() && SkipId==0) { // centroid is contained in the bigger quote
      if (GetQt(ClustV[c]).TmUrlCntV.Empty()) { continue; }
      BegTm = TMath::Mn(BegTm, GetQt(ClustV[c]).TmUrlCntV[0].Tm()); // take the earlier
    }
  }
  return BegTm;
}

int TQuoteBs::LongestCmnSubSq(const TIntV& WIdV1, const TIntV& WIdV2, int& WIdV1Start, int& WIdV2Start, int& SkipId) {
  const TIntV& V1 = WIdV1.Len()>WIdV2.Len() ? WIdV1:WIdV2; // long
  const TIntV& V2 = WIdV1.Len()>WIdV2.Len() ? WIdV2:WIdV1; // short
  const int V1Len = V1.Len();
  const int V2Len = V2.Len();
  static THash<TInt, TIntV> WIdPosH;
  static THashSet<TInt> V2WIdSet;
  // clear hash-tables
  V2WIdSet.Clr(false);
  for (int i=0; i < WIdPosH.Len(); i++) { WIdPosH[i].Clr(false); }
  WIdPosH.Clr(false, -1, false);
  WIdV1Start = WIdV2Start = SkipId = 0;
  for (int i = 0; i < V2Len; i++) { // word position index
    V2WIdSet.AddKey(V2[i]); }
  for (int i = 0; i < V1Len; i++) { // word position index
    if (V2WIdSet.IsKey(V1[i])) { WIdPosH.AddDat(V1[i]).Add(i); } }
  // early termination
  {const int cmnWords = WIdPosH.Len();
  if (cmnWords < V2WIdSet.Len()) {
    if (cmnWords < 4) { return 0; }
    else if (V2Len >= 5 && cmnWords < 5) { return 0; }
    else if (V2Len > 6 && (2*cmnWords < V2Len && cmnWords < 10)) { return 0; } }
  }
  // count the sequence length
  int MaxLen = 0;
  for (int w = 0; w < V2Len; w++) { // short
    const int wid = V2[w];
    if (! WIdPosH.IsKey(wid)) { continue; }
    const TIntV& OccV = WIdPosH.GetDat(wid);
    for (int o = 0; o < OccV.Len(); o++) {
      const int beg = OccV[o];
      int cnt = 0, tmp = 0;
      while (w+cnt < V2Len && beg+cnt < V1Len && V2[w+cnt]==V1[beg+cnt]) { cnt++; tmp=0; }           // no skip
      while (beg+1+cnt < V1Len && w+cnt < V2Len && V2[w+cnt]==V1[beg+cnt+1]) { cnt++; tmp=-1; }      // skip word in long
      while (beg+cnt+1 < V1Len && w+cnt+1 < V2Len && V2[w+cnt+1]==V1[beg+cnt+1]) {  cnt++; tmp=-2;}  // skip word in both
      while (beg+cnt < V1Len && w+cnt+1 < V2Len && V2[w+cnt+1]==V1[beg+cnt]) { cnt++; tmp=-3;}       // skip word in short
      if (MaxLen < cnt) { MaxLen = cnt; SkipId=tmp; WIdV1Start = beg;  WIdV2Start = w; }
      IAssert(cnt >= 1);
    }
  }
  if (! (WIdV1.Len()>WIdV2.Len())) {
    int tmp=WIdV1Start; WIdV1Start=WIdV2Start; WIdV2Start=tmp;
  }
  return MaxLen;
}

void TQuoteBs::CreateClusters(const TVec<TIntV>& ClustV) {
  TIntV CIdV;
  ClustQtIdVH.Clr();
  // reset all quotes
  printf("len %d\n", Len());
  for (int q = 0; q < Len(); q++) {
    TQuote& Q = GetQt(q);
    Q.QtCIdTy = TQuote::TQtIdTy(q, qtQuote);
  }
  TIntH SeenNId;
  for (int c = 0; c < ClustV.Len(); c++) {
    if (ClustV[c].Len() < 3) { continue; } // skip super small clusters
    // set cluster root
    const int CentrQtId = GetCentrQtId(ClustV[c]);
    if (CentrQtId == -1) {
      printf("Cluster %d of size %d has no root!!!\n", c, ClustV[c].Len());
      for (int i = 0; i < ClustV[c].Len(); i++) {
        printf("  %d%c", ClustV[c][i].Val, IsQtId(ClustV[c][i])?'t':'f'); } printf("\n");
      continue;
    }
    IAssert(! SeenNId.IsKey(CentrQtId));
    SeenNId.AddKey(CentrQtId);
    TQuote& Q = GetQt(CentrQtId);
    if (Q.GetTy() != qtQuote) { printf("  %d", Q.GetTy()); }
    IAssert(! ClustQtIdVH.IsKey(CentrQtId));
    Q.QtCIdTy = TQuote::TQtIdTy(CentrQtId, qtRoot);
    ClustQtIdVH.AddDat(CentrQtId, ClustV[c]);
    // point all members to the root
    for (int q = 0; q < ClustV[c].Len(); q++) {
      if (ClustV[c][q] == CentrQtId) { continue; }
      IAssert(! SeenNId.IsKey(ClustV[c][q]));
      SeenNId.AddKey(ClustV[c][q]);
      TQuote& Q2 = GetQt(ClustV[c][q]);
      Q2.QtCIdTy = TQuote::TQtIdTy(CentrQtId, qtInClust);
    }
  }
}

void TQuoteBs::ClusterQts(const int& MinRootWrdLen, const int& MinQtFq, const TStr& OutFNmPref, const TStrV& BlackListV) {
  printf("Cluster quotes with min Fq >= %d: %d total quotes\n", MinQtFq, Len());
  TExeTm ExeTm;
  TStrHash<TInt> StrH;
  // quote --> word id vector
  TVec<TPair<TInt, TIntV> > QWIdVV;  // (QtId, WIdV)
  { TIntPrV LenQIdV;  TIntV QWIdV;
  printf("sort qid by len\n");
  TStrHash<TInt> BlackListH;
  for (int i = 0; i < BlackListV.Len(); i++) { BlackListH.AddDatId(BlackListV[i].GetTrunc()); }
  printf("blacklist len: %d\n", BlackListH.Len());
  for (int q1 = 0; q1 < Len(); q1++) {
    const TQuote& Q = GetQtN(q1);
    const int Doms = Q.GetDoms(*this);
    if ((Q.GetTy()==qtQuote || Q.GetTy()==qtRoot) && Doms>1 && Doms*4>Q.GetUrls()
      && Q.GetFq() >= MinQtFq && (! BlackListH.IsKey(Q.GetStr().CStr()))) { // quote has not yet been merged (or is a root)
        LenQIdV.Add(TIntPr(TStrUtil::CountWords(Q.QtStr.CStr()), GetQtId(q1))); }
  }
  printf("sort %d candidates\n", LenQIdV.Len());
  LenQIdV.Sort(false);
  printf("get word id vectors\n");
  for (int q1 = 0; q1 < LenQIdV.Len(); q1++) {
    TStrUtil::GetAddWIdV(StrH, GetQt(LenQIdV[q1].Val2).QtStr.CStr(), QWIdV);
    QWIdVV.Add(TPair<TInt, TIntV>(LenQIdV[q1].Val2, QWIdV));
  } }
  printf("  %d root quotes\n", ClustQtIdVH.Len());
  printf("  %d quotes to merge\n", QWIdVV.Len());
  // cluster
  int NMergers=0;
  FILE *F = fopen(TStr(OutFNmPref+"-merged.txt").CStr(), "wt");
  const int ClusterQ=QWIdVV.Len();
  for (int q1 = 0; q1 < ClusterQ; q1++) {
    if (QWIdVV[q1].Val2.Len() < MinRootWrdLen) { continue; } // can't be a cluster root if your length < 6
    const int Qt1Id = QWIdVV[q1].Val1;
    for (int q2 = q1+1; q2 < ClusterQ; q2++) {
      int idx1=0, idx2=0, SkipTy=0;
      bool DoMerge = false;
      const int ShortLen = TMath::Mn(QWIdVV[q1].Val2.Len(), QWIdVV[q2].Val2.Len());
      if (ShortLen == 0) { continue; }
      const int Overlap = LongestCmnSubSq(QWIdVV[q1].Val2, QWIdVV[q2].Val2, idx1, idx2, SkipTy);
      if (ShortLen == 4 && Overlap == 4 /*&& SkipTy==0*/) { DoMerge=true; } // full overlap, no skip
      else if (ShortLen == 5 && Overlap == 5 /*&& SkipTy==0*/) { DoMerge=true; } // full overlap, no skip
      else if ((ShortLen == 6 && Overlap >= 5 /*&& SkipTy==0) || (ShortLen == 6 && Overlap == 5*/)) { DoMerge=true; }
      else if (Overlap/double(ShortLen+3) > 0.5 || Overlap > 10) { DoMerge=true; }
      if (DoMerge == true) {
        NMergers++;
        const int Qt2Id = QWIdVV[q2].Val1;
        TQuote& Q1 = GetQt(Qt1Id);
        TQuote& Q2 = GetQt(Qt2Id);
        if (Q1.GetTy() != qtRoot) { // create new cluster (set QtTy to qtRoot)
          IAssert(Q1.GetTy() == qtQuote);
          IAssert(! ClustQtIdVH.IsKey(Qt1Id));
          ClustQtIdVH.AddDat(Qt1Id).Add(Qt1Id);
          Q1.QtCIdTy = TQuote::TQtIdTy(Qt1Id, qtRoot);
        } else { IAssert(Q1.GetTy() == qtRoot); }
        // merge Q2 into Q1
        if (Q2.GetTy() == qtRoot) { // merge two clusters
          for (int c = 0; c < ClustQtIdVH.GetDat(Qt2Id).Len(); c++) {
            ClustQtIdVH.AddDat(Qt1Id).Add(ClustQtIdVH.GetDat(Qt2Id)[c]); }
          Q2.QtCIdTy = TQuote::TQtIdTy(Qt1Id, qtInClust);
          ClustQtIdVH.DelKey(Qt2Id);
          IAssert(ClustQtIdVH.AddDat(Qt1Id).IsIn(Qt2Id));
        } else {
          IAssert(Q2.GetTy() == qtQuote);
          Q2.QtCIdTy = TQuote::TQtIdTy(Qt1Id, qtInClust);
          ClustQtIdVH.AddDat(Qt1Id).Add(Qt2Id);
        }
        QWIdVV[q2].Val2.Clr(true); // matched
        // save
        { TStr Str= "    :"; if(SkipTy==-1){Str="long=";} else if(SkipTy==-2){Str="both=";} else if(SkipTy==-3){Str="shrt=";}
        if (Str.Len()>0) { printf("%c", Str[0]); }
        if (ClustQtIdVH.GetDat(Qt1Id).Len() == 2) { fprintf(F, "\n[%d] %s \t%d\n", TStrUtil::CountWords(Q1.QtStr.CStr()), Q1.QtStr.CStr(), Q1.GetFq()); }
        fprintf(F, "%s%d [%d] %s \t%d\n", Str.CStr(), Overlap, TStrUtil::CountWords(Q2.QtStr.CStr()), Q2.QtStr.CStr(), Q2.GetFq()); }
      }
    }
    if (ClustQtIdVH.IsKey(Qt1Id)) {
      ClustQtIdVH.GetDat(Qt1Id).Pack();
      ClustQtIdVH.GetDat(Qt1Id).Sort();
      QWIdVV[q1].Val2.Clr(true);
    }
    if (q1>0 && q1 % 100 == 0) {
      printf("\r  %d/%d: %d merged [%s]  ", q1, ClusterQ, NMergers, ExeTm.GetStr());
      if (q1 % 1000 == 0) { printf("save.\n");  fflush(F);
        TFOut FOut(OutFNmPref+"-QtBs.ClustQtIdVH"); ClustQtIdVH.Save(FOut); }
    }
  }
  fclose(F);
  printf("\n%d quotes, %d clusters [%s]\n\n.", NMergers, ClustQtIdVH.Len(), ExeTm.GetStr());
}

void TQuoteBs::ResetClusters() {
  ClustQtIdVH.Clr();
  for (int q = 0; q < Len(); q++) {
    GetQtN(q).QtCIdTy = TQuote::TQtIdTy(GetQtId(q), qtQuote);
  }
}

void TQuoteBs::ReassignToClust(const int& QtId, const int& NewCId) {
  const int QtCl = GetQt(QtId).GetCId();
  printf("assign %d from clust %d to %d\n", QtId, QtCl, NewCId);
  if (QtCl != QtId) {
    ClustQtIdVH.GetDat(QtCl).DelIfIn(QtId); }
  GetQt(QtId).QtCIdTy = TQuote::TQtIdTy(NewCId, qtInClust);
  ClustQtIdVH.AddDat(NewCId).Add(QtId);
}

void TQuoteBs::Mergec2Clusters(const int& ParentCId, const int& ChildCId) {
  IAssert(IsClust(ParentCId) && IsClust(ChildCId));
  printf("merging %d (fq: %d) to %d (%d fq)\n", ChildCId, GetClustFq(ChildCId), ParentCId, GetClustFq(ParentCId));
  TIntSet ClustSet;
  { const TIntV& ClustV = GetClust(ParentCId);
  for (int c = 0; c < ClustV.Len(); c++) {
    ClustSet.AddKey(ClustV[c]); } }
  { const TIntV& ClustV = GetClust(ChildCId);
  for (int c = 0; c < ClustV.Len(); c++) {
    ClustSet.AddKey(ClustV[c]); } }
  ClustSet.GetKeyV(ClustQtIdVH.GetDat(ParentCId));
  GetQt(ChildCId).QtCIdTy = TQuote::TQtIdTy(ChildCId, qtInClust);
  ClustQtIdVH.DelKey(ChildCId);
  ClustQtIdVH.Defrag();
}

PClustNet TQuoteBs::GetClustNet(const int& MinQtFq, const TStr& OutFNmPref) const {
  printf("Cluster quotes with Fq >= %d: %d total quotes\n", MinQtFq, Len());
  TExeTm ExeTm;
  TStrHash<TInt> StrH;
  FILE *F = fopen(TStr(OutFNmPref+"-candidates.txt").CStr(), "wt");
  // quote --> word id vector
  THash<TInt, TIntV> QWIdVH;  // QtId --> WIdV
  { TIntPrV LenQIdV;  TIntV QWIdV;
  printf("sort qid by len\n");
  for (int q1 = 0; q1 < Len(); q1++) {
    const TQuote& Q = GetQtN(q1);
    const int Doms = Q.GetDoms(*this);
    if ((Q.GetTy()!=qtCentr) && Doms>1 && Doms*4 > Q.GetUrls() && Q.GetFq() >= MinQtFq) { // quote has not yet been merged (or is a root)
      LenQIdV.Add(TIntPr(TStrUtil::CountWords(Q.QtStr.CStr()), GetQtId(q1)));
    }
  }
  printf("sort %d candidates by word length\n", LenQIdV.Len());
  LenQIdV.Sort(false);
  for (int q1 = 0; q1 < LenQIdV.Len(); q1++) {
    TStrUtil::GetAddWIdV(StrH, GetQt(LenQIdV[q1].Val2).QtStr.CStr(), QWIdV);
    QWIdVH.AddDat(LenQIdV[q1].Val2, QWIdV);
    fprintf(F, "%s\n", GetQt(LenQIdV[q1].Val2).QtStr.CStr());
  } }
  printf("  %d root quotes\n", ClustQtIdVH.Len());
  printf("  %d quotes to merge\n", QWIdVH.Len());
  // cluster
  const int ClusterQ=QWIdVH.Len();
  PClustNet ClustNet = TClustNet::New();
  TIntH NIdDepthH;
  TIntSet SeenQtIdSet;
  int NMerges=0, idx1=0, idx2=0, SkipTy=0; // TreeSz=0, TreeDepth=0,
  for (int q1 = 0; q1 < ClusterQ; q1++) { // for each quote
    SeenQtIdSet.Clr(false);
    const int Qt1Id = QWIdVH.GetKey(q1);
    for (int q2 = q1-1; q2 >= 0; q2--) { //  go over all shorter quotes
      const int Qt2Id = QWIdVH.GetKey(q2);
      if (SeenQtIdSet.IsKey(Qt2Id)) { continue; }
      bool DoMerge = false;
      const int Overlap = LongestCmnSubSq(QWIdVH[q1], QWIdVH[q2], idx1, idx2, SkipTy);
      const int ShortLen = QWIdVH[q1].Len(); IAssert(QWIdVH[q1].Len()<= QWIdVH[q2].Len());
      if (ShortLen == 4 && Overlap == 4) { DoMerge=true; } // full overlap, no skip
      else if (ShortLen == 5 && Overlap == 5) { DoMerge=true; } // full overlap, no skip
      else if (ShortLen == 6 && Overlap >= 5) { DoMerge=true; }
      else if (Overlap/double(ShortLen+3) > 0.5 || Overlap > 10) { DoMerge=true; }
      if (DoMerge==true) { NMerges++;
        if (! ClustNet->IsNode(Qt1Id)) { ClustNet->AddNode(Qt1Id, GetQt(Qt1Id)); }
        if (! ClustNet->IsNode(Qt2Id)) { ClustNet->AddNode(Qt2Id, GetQt(Qt2Id)); }
        //ClustNet->AddEdge(Qt1Id, Qt2Id, -1, Overlap);
        ClustNet->AddEdge(Qt1Id, Qt2Id);
        SeenQtIdSet.AddKey(Qt1Id);  SeenQtIdSet.AddKey(Qt2Id);
        /*TGAlg::GetSubTreeSz(ClustNet, Qt2Id, true, NIdDepthH, TreeSz, TreeDepth);
        for (int i = 0; i < NIdDepthH.Len(); i++) {
          SeenQtIdSet.AddKey(NIdDepthH.GetKey(i)); }*/
        fprintf(F, "%d Merge\t%d\t%d\ttree\t%d\n\t%s\n\t%s\n", q1, QWIdVH[q1].Len(), QWIdVH[q2].Len(), NIdDepthH.Len(), GetQt(Qt2Id).GetStr().CStr(), GetQt(Qt1Id).GetStr().CStr());
        fflush(F);
      }
    }
    if (q1>0 && q1 % 100 == 0) {
      printf("\r  %d/%d: %d merged [%s]  ", q1, ClusterQ, NMerges, ExeTm.GetStr());
      if (q1 % 1000 == 0) {
        printf("save: %d merges, %d nodes, %d edges in ClustNet [%s]\n.", NMerges, ClustNet->GetNodes(), ClustNet->GetEdges(), ExeTm.GetStr());
        TFOut FOut(OutFNmPref+".ClustNet"); ClustNet->Save(FOut); }
    }
  }
  fclose(F);
  printf("\n%d merges, %d nodes, %d edges in ClustNet [%s]\n\n.", NMerges, ClustNet->GetNodes(), ClustNet->GetEdges(), ExeTm.GetStr());
  return ClustNet;
}

void TQuoteBs::AddMergedQtsToQtBs() {
  printf("merge quote clusters and add them to the quote base\n");
  TQuote MergedQt;
  // delete ClustQtIdVH.Len() quotes with Fq == 1
  int toDel = ClustQtIdVH.Len();
  for (int q = 0; q < QuoteH.Len(); q++) {
    if (QuoteH[q].GetFq() == 1) { QuoteH.DelKeyId(q); toDel--; }
    if (toDel == 0) { break; }
  }
  // add merged quotes
  for (int q = 0; q < ClustQtIdVH.Len(); q++) {
    GetMergedClustQt(ClustQtIdVH.GetKey(q), MergedQt);
    IAssert(MergedQt.TmUrlCntV.Len() > 0); // quote string still points to
    QuoteH.AddDat(QuoteH.Len(), MergedQt); // add new quote
  }
  printf("IsKeyIdEqKeyN: %s\n", QuoteH.IsKeyIdEqKeyN()?"T":"F");
}

PQuoteBs TQuoteBs::GetMergeClusters(const bool& OnlyClusters) const {
  printf("Merging clusters into single quotes:\n");
  PQuoteBs NewQtBs = TQuoteBs::New();
  TQuote MergedQt;
  printf("  %d quotes total\n", Len());
  printf("  %d clusters\n", ClustQtIdVH.Len());
  TIntSet SeenQtIdSet;
  for (int q = 0; q < ClustQtIdVH.Len(); q++) {
    GetMergedClustQt(ClustQtIdVH.GetKey(q), MergedQt);
    IAssert(MergedQt.TmUrlCntV.Len() >0);
    NewQtBs->AddQuote(MergedQt, *this);
    const TIntV& ClustQtIdV = ClustQtIdVH[q];
    for (int c = 0; c < ClustQtIdV.Len(); c++) {
      const int qid = ClustQtIdV[c];
      IAssert(! SeenQtIdSet.IsKey(qid)); // each Qt is member of only one cluster
      SeenQtIdSet.AddKey(qid);
    }
  }
  if (! OnlyClusters) {
    for (int q = 0; q < Len(); q++) {
      const TQuote& Q = GetQtN(q);
      if (! SeenQtIdSet.IsKey(GetQtId(q))) {
        NewQtBs->AddQuote(Q, *this); }
    }
  }
  printf("  %d quotes in new Quotes base\n", NewQtBs->Len());
  return NewQtBs;
}

// get domains by the number of top quotes they mention
void TQuoteBs::GetTopQtDoms(TStrIntPrV& DomCntV, const int& TakeNClust, const int& MinClFq, const TStr& RootHasWord, const TUrlTy& OnlyCountTy) const {
  TIntV CIdV;
  GetCIdVByFq(CIdV, MinClFq, RootHasWord, OnlyCountTy);
  THash<TStr, TInt> DomCntH;
  for (int c = 0; c < TMath::Mn(CIdV.Len(), TakeNClust); c++) {
    const TIntV& ClustV = GetClust(CIdV[c]);
    for (int q = 0; q < ClustV.Len(); q++) {
      const TQuote& Q = GetQt(ClustV[q]);
      for (int u = 0; u < Q.TmUrlCntV.Len(); u++) {
        DomCntH.AddDat(TStrUtil::GetDomNm(GetStr(Q.TmUrlCntV[u].UrlId()))) += 1;
      }
    }
  }
  DomCntH.SortByDat(false);
  DomCntH.GetKeyDatPrV(DomCntV);
}

void TQuoteBs::TopDomsByLag(const TTmUnit& TmUnit, const int& TakeNDoms, const int& TakeNClusts, const int& Thresh) const {
  THash<TStr, TMom> DomLagH;
  TIntV CIdV;  GetCIdVByFq(CIdV, 10, "", utUndef);
  //TIntV QIdV;  GetQIdVByFq(QIdV, 10, "", utUndef);
  TStrIntPrV DomCntV;  GetTopQtDoms(DomCntV, TakeNClusts, 10, "", utUndef);
  printf("Take %d clusters, %d domains", TakeNClusts, TakeNDoms);
  for (int d = 0; d < TMath::Mn(TakeNDoms, DomCntV.Len()); d++) {
    DomLagH.AddDat(DomCntV[d].Val1);
    printf("%d\t%s\n", DomCntV[d].Val2(), DomCntV[d].Val1.CStr());
  }
  int NQuotes=0;
  for (int c = 0; c < TMath::Mn(CIdV.Len(), TakeNClusts); c++) {
    TQuote Q;  GetMergedClustQt(CIdV[c], Q, false);
    const TSecTm MedTm = Q.GetMedianTm(TmUnit, utUndef, *this).Round(TmUnit);
    TStrSet DomSet;
    for (int u = 0; u < Q.TmUrlCntV.Len(); u++) {
      IAssert(IsStrId(Q.TmUrlCntV[u].UrlId()));
      const TStr Dom = TStrUtil::GetDomNm(GetStr(Q.TmUrlCntV[u].UrlId()));
      if (DomSet.IsKey(Dom)) { continue; } // only take first query occurence on the domain
      if (! DomLagH.IsKey(Dom)) { continue; }
      DomLagH.AddDat(Dom).Add((int(Q.TmUrlCntV[u].Tm().Round(TmUnit))-int(MedTm))/3600.0);
      DomSet.AddKey(Dom);
    }
    NQuotes += GetClust(CIdV[c]).Len();
  }
  FILE *F = fopen(TStr::Fmt("domsByLag-clust%d.tab", TakeNClusts).CStr(), "wt");
  fprintf(F, "Top clusters: %d, total quotes in top clusters: %d\n", TakeNClusts, NQuotes);
  TFltFltStrTrV MedLagDomV;
  for (int i = 0; i < DomLagH.Len(); i++) {
    DomLagH[i].Def();
    MedLagDomV.Add(TFltFltStrTr(DomLagH[i].GetMedian(), DomLagH[i].GetVals(), DomLagH.GetKey(i)));
  }
  MedLagDomV.Sort();
  fprintf(F, "AGGREGATE PEAK\nTop domains with least lag (out of top %d considered) that mention at least 10 pct of top %d quotes (clusters) :\n", TakeNDoms, TakeNClusts);
  for (int i = 0; i < MedLagDomV.Len(); i++) {
    if (MedLagDomV[i].Val2() < Thresh*TakeNClusts/100.0) { continue; }
    fprintf(F, "%g\t%g\t%s\n", MedLagDomV[i].Val1(), MedLagDomV[i].Val2(), MedLagDomV[i].Val3.CStr());
  }
  /*printf("Bottom 100 domains with least lag:\n");
  for (int i = 0; i < 100; i++) {
    printf("  %.2f\t%s\n", MedLagDomV[i].Val1(), MedLagDomV[i].Val2.CStr());
  }*/
  fclose(F);
}

void TQuoteBs::PlotQtFqCnt(const TStr& OutFNmPref) const {
  TIntH UrlCntH, DomCntH, CntFqH;
  THashSet<TChA> DomSet;
  for (int q = 0; q < QuoteH.Len(); q++) {
    const TQuote& Q = GetQtN(q);
    UrlCntH.AddDat(Q.GetUrls()) += 1;
    CntFqH.AddDat(Q.GetFq()) += 1;
    DomSet.Clr(false);
    for (int u = 0; u < Q.GetUrls(); u++) {
      DomSet.AddKey(TStrUtil::GetDomNm(GetStr(Q.TmUrlCntV[u].UrlId()))); }
    DomCntH.AddDat(DomSet.Len()) += 1;
  }
  TGnuPlot::PlotValCntH(UrlCntH, OutFNmPref+"-url", "", "number of urls quote appears at", "count", gpsLog10XY);
  TGnuPlot::PlotValCntH(DomCntH, OutFNmPref+"-dom", "", "number of domains quote appears at", "count", gpsLog10XY);
  TGnuPlot::PlotValCntH(CntFqH, OutFNmPref+"-fq", "", "number of times quote appears", "count", gpsLog10XY);
}

void TQuoteBs::PlotQtMediaVsBlogFq(const int& QtId, const TStr& OutFNmPref) const {
  TQuote::TTmFltPrV BlogFqV, MediaFqV, SmoothV;
  TFltPrV FqV;
  const TQuote& ClQt = GetQt(QtId);
  ClQt.GetFqOt(BlogFqV, tmu4Hour, utBlog, *this);
  ClQt.GetFqOt(MediaFqV, tmu4Hour, utMedia, *this);
  const TSecTm MinTm = ClQt.TmUrlCntV[0].Tm().Round(tmuDay);
  TGnuPlot GP("mediaVsBlogs-"+OutFNmPref, TStr::Fmt("%s. b:%d m:%d u:%d d:%d", ClQt.GetStr().CStr(),
    ClQt.GetFq(utBlog, *this), ClQt.GetFq(utMedia, *this), ClQt.GetUrls(), ClQt.GetDoms(*this)));
  TQuote::GetSmoothFqOt(SmoothV, BlogFqV, tmu4Hour, 24, 1.2, MinTm);
  for (int i = 0; i < SmoothV.Len(); i++) {
    FqV.Add(TFltPr(double(SmoothV[i].Val1-MinTm)/(24.0*3600.0), SmoothV[i].Val2())); }
  GP.AddPlot(FqV, gpwLines, "Blog frequency");
  TQuote::GetSmoothFqOt(SmoothV, MediaFqV, tmu4Hour, 24, 1.2, MinTm);
  FqV.Clr();
  for (int i = 0; i < SmoothV.Len(); i++) {
    IAssert(SmoothV[i].Val1>=MinTm);
    FqV.Add(TFltPr(double(SmoothV[i].Val1-MinTm)/(24.0*3600.0)*(54.0/46.0), SmoothV[i].Val2()));
  }
  GP.AddPlot(FqV, gpwLines, "Media frequency");
  GP.SetXYLabel(TStr::Fmt("time [days] since %s", MinTm.GetYmdTmStr().CStr()), "Quote frequency (normalized for the baseline 46 vs 54)");
  GP.SavePng();
}

int NormMaxTo1(const TQuote::TTmFltPrV& TmFqV, TFltPrV& OutV, const TSecTm& BegTm) {
  int Idx=0;
  double MaxVal=0;
  for (int i = 0; i < TmFqV.Len(); i++) {
    if (MaxVal < TmFqV[i].Val2) {
      MaxVal=TmFqV[i].Val2; Idx = i; }
  }
  OutV.Gen(TmFqV.Len(), 0);
  for (int i = 0; i < TmFqV.Len(); i++) {
    OutV.Add(TFltPr(double(TmFqV[i].Val1-BegTm)/(24.0*3600.0), TmFqV[i].Val2())); //MaxVal));
  }
  return Idx;
}

void TQuoteBs::PlotClustMediaVsBlogFq(const int& CId, const TStr& OutFNmPref) const {
  const TTmUnit TmUnit = tmu4Hour;
  TQuote::TTmFltPrV BlogFqV, MediaFqV, AllFqV, SmoothV;
  TFltPrV FqV, RawFqV;
  TQuote ClQt;
  GetMergedClustQt(CId, ClQt, false);
  ClQt.GetFqOt(BlogFqV, TmUnit, utBlog, *this);
  ClQt.GetFqOt(MediaFqV, TmUnit, utMedia, *this);
  const TSecTm MinTm = ClQt.TmUrlCntV[0].Tm().Round(tmuDay);
  const TSecTm  MediaPeak = ClQt.GetPeakTm(TmUnit, TSecTm(1), utMedia, *this);
  const TSecTm BlogPeak = ClQt.GetPeakTm(TmUnit, TSecTm(1), utBlog, *this);
  const int Lag = (int(BlogPeak.Round(TmUnit))-int(MediaPeak.Round(TmUnit)))/3600;
  const bool Is1Peak = ClQt.IsSinglePeak(TmUnit, TSecTm(1), utUndef, *this);
  TGnuPlot GP("mediaVsBlogs-"+OutFNmPref, TStr::Fmt("%s. b:%d m:%d u:%d d:%d", Is1Peak?"SINGLE PEAK": "MORE PEAKS", //ClQt.GetStr().CStr(),
    Lag, ClQt.GetFq(utBlog, *this), ClQt.GetFq(utMedia, *this), ClQt.GetUrls(), ClQt.GetDoms(*this)));
  // blogs
  int mx = NormMaxTo1(BlogFqV, RawFqV, MinTm);
  //GP.AddPlot(RawFqV, gpwLines, TStr::Fmt("RAW Blog : max %s, %s", BlogFqV[mx].Val1.GetYmdTmStr().CStr(), BlogPeak.GetYmdTmStr().CStr()), "pt 7 ps 1");
  TQuote::GetSmoothFqOt(SmoothV, BlogFqV, TmUnit, 2*24, 1.2, MinTm);
  mx = NormMaxTo1(SmoothV, FqV, MinTm);
  GP.AddPlot(FqV, gpwLines, TStr::Fmt("SMOOTH Blog : max %s", SmoothV[mx].Val1.GetYmdTmStr().CStr()), "pt 7 ps 1");
  const TSecTm SmBlog = SmoothV[mx].Val1;
  // media
  mx = NormMaxTo1(MediaFqV, RawFqV, MinTm);
  //GP.AddPlot(RawFqV, gpwLines, TStr::Fmt("RAW Media : max %s, %s", MediaFqV[mx].Val1.GetYmdTmStr().CStr(), MediaPeak.GetYmdTmStr().CStr()), "pt 5 ps 1");
  TQuote::GetSmoothFqOt(SmoothV, MediaFqV, TmUnit, 2*24, 1.2, MinTm);
  mx = NormMaxTo1(SmoothV, FqV, MinTm);
  const TSecTm SmMed = SmoothV[mx].Val1;
  const int SmLag = (int(SmBlog.Round(TmUnit))-int(SmMed.Round(TmUnit)))/3600;
  GP.AddPlot(FqV, gpwLines, TStr::Fmt("SMOOTH Media is behind: %dh, smooth: %dh.", -Lag, -SmLag), "pt 5 ps 1");
  //ClQt.GetFqOt(AllFqV, TmUnit);
  //NormMaxTo1(AllFqV, FqV, MinTm);
  //GP.AddPlot(FqV, gpwLines, "ALL", "pt 5 ps 1");
  GP.SetXYLabel(TStr::Fmt("time [days] since %s", MinTm.GetYmdTmStr().CStr()), "Cluster frequency (normalized for the baseline 46 vs 54)");
  GP.AddCmd("set xtics 10");
  GP.AddCmd("set mxtics 10");
  GP.SavePng();
}

void TQuoteBs::PlotMediaVsBlogLag(const TTmUnit& TmUnit, const bool& TakeClusters, const int& TakeN, const TStr& OutFNmPref) const {
  TQuote Qt;  TIntV IdV;
  TMom LagMom1d, LagMom2d, LagMom3d, LagMom4d, LagMom7d;
  TFltFltH LagCntH, LagBCntH, LagMCntH;//, LagFqH, LagMedFq, LagBlogFq;
  int Cnt=0;
  if (TakeClusters) { GetCIdVByFq(IdV, 10, "", utUndef, false); }
  else { GetQtIdVByFq(IdV, 8, 10, false, "", "", utUndef); }
  for (int c = 0; c < TakeN; c++) {
    if (TakeClusters) { GetMergedClustQt(IdV[c], Qt, false); }
    else { Qt = GetQt(IdV[c]); }
    //if (! Qt.IsSinglePeak(tmu4Hour, TSecTm(1), utMedia, *this)) { continue; }
    //if (! Qt.IsSinglePeak(tmu4Hour, TSecTm(1), utBlog, *this)) { continue; }
    //const TSecTm  MediaPeak = Qt.GetPeakTm(TmUnit, TSecTm(1), utMedia, *this);
    //const TSecTm BlogPeak = Qt.GetPeakTm(TmUnit, TSecTm(1), utBlog, *this);
    //const TSecTm  MediaPeak = Qt.GetMeanTm(TmUnit, utMedia, *this);
    //const TSecTm BlogPeak = Qt.GetMeanTm(TmUnit, utBlog, *this);
    const TSecTm  AllPeak = Qt.GetMedianTm(TmUnit, utUndef, *this);
    const TSecTm BlogPeak = Qt.GetMedianTm(TmUnit, utBlog, *this);
    const TSecTm  MediaPeak = Qt.GetMedianTm(TmUnit, utMedia, *this);
    const double Lag = (int(BlogPeak.Round(TmUnit))-int(MediaPeak.Round(TmUnit)))/3600.0;
    const double Lag1 = (int(BlogPeak.Round(TmUnit))-int(AllPeak.Round(TmUnit)))/3600.0;
    const double Lag2 = (int(MediaPeak.Round(TmUnit))-int(AllPeak.Round(TmUnit)))/3600.0;
    if (fabs(Lag) < 1*24) { LagMom1d.Add(Lag); }
    if (fabs(Lag) < 2*24) { LagMom2d.Add(Lag); }
    if (fabs(Lag) < 3*24) { LagMom3d.Add(Lag); }
    if (fabs(Lag) < 4*24) { LagMom4d.Add(Lag); }
    if (fabs(Lag) < 7*24) { LagMom7d.Add(Lag); }
    LagCntH.AddDat(Lag) += 1;
    LagBCntH.AddDat(Lag1) += 1;
    LagMCntH.AddDat(Lag2) += 1;
    /*LagFqH.AddDat(Lag, Qt.GetFq());
    LagMedFq.AddDat(Lag, Qt.GetFq(utMedia, *this));
    LagBlogFq.AddDat(Lag, Qt.GetFq(utBlog, *this));*/
    //if (TakeClusters) { PlotClustMediaVsBlogFq(IdV[c], TStr::Fmt("%s-%02d", OutFNmPref.CStr(), c+1)); }
    //else { PlotQtMediaVsBlogFq(IdV[c], TStr::Fmt("%s-%02d", OutFNmPref.CStr(), c+1)); }
    Cnt++;
  }
  TFltPrV PrV;
  LagMom1d.Def();  LagMom2d.Def();  LagMom3d.Def();  LagMom4d.Def();  LagMom7d.Def();
  { TGnuPlot GP("lagBlogsVsMedia-"+OutFNmPref, TStr::Fmt("Lag: 1D: %.2f %g;   2D: %.2f  %g;   3D: %.2f  %g;   4D: %.2f  %g;   7D: %.2f  %g",
    LagMom1d.GetMean(), LagMom1d.GetMedian(), LagMom2d.GetMean(), LagMom2d.GetMedian(), LagMom3d.GetMean(), LagMom3d.GetMedian(),
    LagMom4d.GetMean(), LagMom4d.GetMedian(), LagMom7d.GetMean(), LagMom7d.GetMedian()), true);
  LagBCntH.GetKeyDatPrV(PrV); PrV.Sort(); GP.AddPlot(PrV, gpwLinesPoints, TStr::Fmt("%s. b:%d m:%d u:%d d:%d BLOG", Qt.GetStr().CStr(),
    Qt.GetFq(utBlog, *this), Qt.GetFq(utMedia, *this), Qt.GetUrls(), Qt.GetDoms(*this)));
  LagMCntH.GetKeyDatPrV(PrV); PrV.Sort(); GP.AddPlot(PrV, gpwLinesPoints, TStr::Fmt("%s. b:%d m:%d u:%d d:%d MEDIA", Qt.GetStr().CStr(),
    Qt.GetFq(utBlog, *this), Qt.GetFq(utMedia, *this), Qt.GetUrls(), Qt.GetDoms(*this)));
  GP.AddCmd("set xrange[-24:24]\nset mxtics 5\nset xtics 5");
  GP.SetXYLabel("media lag in hours (+: news lag, -: news lead)", "count");
  GP.SavePng(); }
  /*{ TGnuPlot GP("lagScatterBlogs-"+OutFNmPref, "");
  LagBlogFq.GetKeyDatPrV(PrV);  GP.AddPlot(PrV, gpwPoints);
  GP.SetXYLabel("lag [hours]", "frequency (blogs)"); GP.SavePng(); }
  { TGnuPlot GP("lagScatterMedia-"+OutFNmPref, "");
  LagMedFq.GetKeyDatPrV(PrV);  GP.AddPlot(PrV, gpwPoints);
  GP.SetXYLabel("lag [hours]", "frequency (media)"); GP.SavePng(); }
  { TGnuPlot GP("lagScatter-"+OutFNmPref, "");
  LagFqH.GetKeyDatPrV(PrV);  GP.AddPlot(PrV, gpwPoints);
  GP.SetXYLabel("lag [hours]", "frequency (all)"); GP.SavePng(); }*/
  printf("\nTake top %d clusters. TmUnit: %s\n", TakeN, TTmInfo::GetTmUnitStr(TmUnit).CStr());
  printf("  1d:\tavg:\t%f\tmed:\t%f\n", LagMom1d.GetMean(), LagMom1d.GetMedian());
  printf("  2d:\tavg:\t%f\tmed:\t%f\n", LagMom2d.GetMean(), LagMom2d.GetMedian());
  printf("  3d:\tavg:\t%f\tmed:\t%f\n", LagMom3d.GetMean(), LagMom3d.GetMedian());
  printf("  4d:\tavg:\t%f\tmed:\t%f\n", LagMom4d.GetMean(), LagMom4d.GetMedian());
  printf("  7d:\tavg:\t%f\tmed:\t%f\n", LagMom7d.GetMean(), LagMom7d.GetMedian());

}

void TQuoteBs::PlotFqDecay(const TTmUnit& TmUnit, const bool& TakeClusters, const TUrlTy& CntUrlTy, const int& PlotN, const int& MinValsPerTm, const TStr& OutFNmPref) const {
  THash<TFlt, TMom> MomH;
  TQuote Qt;
  TIntV IdV;
  TGnuPlot GP("decay-"+OutFNmPref);
  if (TakeClusters) { GetCIdVByFq(IdV, 10, "", CntUrlTy, false); }
  else { GetQtIdVByFq(IdV, 8, 10, false, "", "", CntUrlTy); }
  for (int c = 0; c < PlotN; c++) {
    if (TakeClusters) { GetMergedClustQt(IdV[c], Qt, false); }
    else { Qt = GetQt(IdV[c]); }
    //if (! Qt.IsSinglePeak(TmUnit, TSecTm(0), CntUrlTy, *this)) { printf("."); continue; }
    TQuote::TTmFltPrV QtV;  TFltPrV TmV;
    Qt.GetFqOt(QtV, TmUnit);
    //Qt.GetSmoothFqOt(QtV, TmUnit, 48, 1.4);
    int pos=0;  double val=0.0;
    for (int i = 0; i < QtV.Len(); i++) {
      if (val < QtV[i].Val2) { pos = i;  val = QtV[i].Val2; }
    }
    const double PeakVal = val;
    const TSecTm PeakTm = QtV[pos].Val1;
    for (int i = 0; i < QtV.Len(); i++) {
      TmV.Add(TFltPr((double(QtV[i].Val1)-double(PeakTm))/(24.0*3600), QtV[i].Val2/PeakVal));
      MomH.AddDat(TmV.Last().Val1).Add(TmV.Last().Val2);
    }
    const TStr Label = "";//TStr::Fmt("%s %d %d %d", Qt.GetStr().CStr(), Qt.GetFq(), Qt.GetUrls(), Qt.GetDoms(*this));
    //GP.AddPlot(TmV, gpwLines, Label, "lt 0");
  }
  TFltPrV AvgV, MedV;
  MomH.SortByKey();
  for (int i = 0; i < MomH.Len(); i++) {
    MomH[i].Def();
    if (MomH[i].GetVals()< MinValsPerTm) { printf("."); continue; }
    AvgV.Add(TFltPr(MomH.GetKey(i), MomH[i].GetMean()));
    MedV.Add(TFltPr(MomH.GetKey(i), MomH[i].GetMedian()));
  }
  GP.AddPlot(AvgV, gpwLines, "Average", "lt 2 lw 2");
  GP.AddPlot(MedV, gpwLines, TStr::Fmt("Median (last qt: %d %d %d)", Qt.GetFq(), Qt.GetUrls(), Qt.GetDoms(*this)), "lt 1 lw 2");
  GP.SetXYLabel("time [days]", "count");
  //GP.AddCmd("set nokey");
  GP.AddCmd("set mxtics 6");
  GP.AddCmd("set xtics 1");
  GP.AddCmd("set yrange [0:1]");
  GP.AddCmd("set xrange [-5:5]");
  GP.AddCmd("set yzeroaxis lt -1");
  GP.SavePng();
}

void NormPeakAt1(THash<TInt, TFlt>& H) {
  double PeakFq=1;
  for (int i = 0; i < H.Len(); i++) {
    PeakFq=TMath::Mx((double)H[i], PeakFq); }
  for (int i = 0; i < H.Len(); i++) {
    H[i]/=PeakFq; }
}

void TQuoteBs::PlotBlogVsMediaFqOt(const TTmUnit& TmUnit, const bool& TakeClusters, int PlotN, const TStr& OutFNmPref) const {
  THash<TInt, TFlt> Peak1AllH, Peak2AllH, Peak3AllH;
  THash<TInt, TFlt> Peak1MedH, Peak2MedH, Peak3MedH;
  THash<TInt, TFlt> Peak1BlogH, Peak2BlogH, Peak3BlogH;
  TQuote Qt;
  TIntV IdV;
  TInt FqAtPeak;
  if (TakeClusters) { GetCIdVByFq(IdV, 100, "", utUndef, false); }
  else { GetQtIdVByFq(IdV, 8, 10, false, "", "", utUndef); }
  for (int c = 0; c < TMath::Mn(PlotN, IdV.Len()); c++) {
    if (TakeClusters) { GetMergedClustQt(IdV[c], Qt, false); }
    else { Qt = GetQt(IdV[c]); }
    if (! Qt.IsSinglePeak(TmUnit, TSecTm(0), utUndef, *this)) { PlotN++; printf("."); continue; }
    TQuote::TTmFltPrV AllFqV, BlogFqV, MedFqV;  TFltPrV TmV;
    const double Peak1 = Qt.GetPeakTm(TmUnit, TSecTm(1), FqAtPeak).GetAbsSecs(); // peak
    const double Peak2 = Qt.GetMeanTm(TmUnit, utUndef, *this).GetAbsSecs();    // mean
    const double Peak3 = Qt.GetMedianTm(TmUnit, utUndef, *this).GetAbsSecs();  // median
    double PeakFq = FqAtPeak;
    Qt.GetFqOt(AllFqV, TmUnit, utUndef, *this);
    Qt.GetFqOt(MedFqV, TmUnit, utMedia, *this);
    Qt.GetFqOt(BlogFqV, TmUnit, utBlog, *this);
    /*for (int i = 0; i < AllFqV.Len(); i++) {
      const int T1 = (int) ((double(AllFqV[i].Val1.GetAbsSecs())-Peak1)/(3600.0));
      const int T2 = (int) ((double(AllFqV[i].Val1.GetAbsSecs())-Peak2)/(3600.0));
      const int T3 = (int) ((double(AllFqV[i].Val1.GetAbsSecs())-Peak3)/(3600.0));
      Peak1AllH.AddDat(T1) += AllFqV[i].Val2;
      Peak2AllH.AddDat(T2) += AllFqV[i].Val2;
      Peak3AllH.AddDat(T3) += AllFqV[i].Val2;
    }*/
    PeakFq=1;
    for (int i = 0; i < MedFqV.Len(); i++) { PeakFq=TMath::Mx((double)MedFqV[i].Val2(), PeakFq); }
    for (int i = 0; i < MedFqV.Len(); i++) {
      const int T1 = (int) TMath::Round((double(MedFqV[i].Val1.GetAbsSecs())-Peak1)/(3600.0));
      const int T2 = (int) TMath::Round((double(MedFqV[i].Val1.GetAbsSecs())-Peak2)/(3600.0));
      const int T3 = (int) TMath::Round((double(MedFqV[i].Val1.GetAbsSecs())-Peak3)/(3600.0));
      Peak1MedH.AddDat(T1) += MedFqV[i].Val2/PeakFq;
      Peak2MedH.AddDat(T2) += MedFqV[i].Val2/PeakFq;
      Peak3MedH.AddDat(T3) += MedFqV[i].Val2/PeakFq;
    }
    PeakFq=1;
    for (int i = 0; i < BlogFqV.Len(); i++) { PeakFq=TMath::Mx((double)BlogFqV[i].Val2(), PeakFq); }
    for (int i = 0; i < BlogFqV.Len(); i++) {
      const int T1 = (int) TMath::Round((double(BlogFqV[i].Val1.GetAbsSecs())-Peak1)/(3600.0));
      const int T2 = (int) TMath::Round((double(BlogFqV[i].Val1.GetAbsSecs())-Peak2)/(3600.0));
      const int T3 = (int) TMath::Round((double(BlogFqV[i].Val1.GetAbsSecs())-Peak3)/(3600.0));
      Peak1BlogH.AddDat(T1) += BlogFqV[i].Val2/PeakFq;
      Peak2BlogH.AddDat(T2) += BlogFqV[i].Val2/PeakFq;
      Peak3BlogH.AddDat(T3) += BlogFqV[i].Val2/PeakFq;
    }
  }
  NormPeakAt1(Peak1AllH); NormPeakAt1(Peak2AllH); NormPeakAt1(Peak3AllH);
  NormPeakAt1(Peak1MedH); NormPeakAt1(Peak2MedH); NormPeakAt1(Peak3MedH);
  NormPeakAt1(Peak1BlogH); NormPeakAt1(Peak2BlogH); NormPeakAt1(Peak3BlogH);
  TGnuPlot::PlotValCntH(Peak1AllH, "ALL", Peak1MedH, "MEDIA", Peak1BlogH, "BLOG", "peakMax-"+OutFNmPref, "Max is the peak", "Time [h]", "Count (sum of frequencies of all quotes)");
  //TGnuPlot::PlotValCntH(Peak2AllH, "ALL", Peak2MedH, "MEDIA", Peak2BlogH, "BLOG", "peakAvg-"+OutFNmPref, "Average is the peak", "Time [h]", "Count (sum of frequencies of all quotes)");
  TGnuPlot::PlotValCntH(Peak3AllH, "ALL", Peak3MedH, "MEDIA", Peak3BlogH, "BLOG", "peakMed-"+OutFNmPref, "Median is the peak", "Time [h]", "Count (sum of frequencies of all quotes)");
  //TGnuPlot::PlotValCntH(Peak1AllH, "MAX PEAK", Peak2AllH, "AVG PEAK", Peak3BlogH, "MEDIAN PEAK", "peakAll-"+OutFNmPref, "ALL urls", "Time [h]", "Count (sum of frequencies of all quotes)");
  //TGnuPlot::PlotValCntH(Peak1MedH, "MAX PEAK", Peak2MedH, "AVG PEAK", Peak3BlogH, "MEDIAN PEAK", "peakMedia-"+OutFNmPref, "MEDIA urls", "Time [h]", "Count (sum of frequencies of Med quotes)");
  //TGnuPlot::PlotValCntH(Peak1BlogH, "MAX PEAK", Peak2BlogH, "AVG PEAK", Peak3BlogH, "MEDIAN PEAK", "peakBlog-"+OutFNmPref, "BLOG urls", "Time [h]", "Count (sum of frequencies of Blog quotes)");
}

void TQuoteBs::PlotBlogFracOt(const TTmUnit& TmUnit, const bool& TakeClusters, const int& PlotN, const TStr& OutFNmPref) const {
  TQuote Qt;
  TIntV IdV;
  THash<TFlt, TMom> TmMomH;
  THash<TFlt, TFltPr> TmBmH;
  if (TakeClusters) { GetCIdVByFq(IdV, 10, "", utUndef, false); }
  else { GetQtIdVByFq(IdV, 8, 10, false, "", "", utUndef); }
  for (int c = 0; c < PlotN; c++) {
    if (TakeClusters) { GetMergedClustQt(IdV[c], Qt, false); }
    else { Qt = GetQt(IdV[c]); }
    TQuote::TTmFltPrV MedQtV, BlgQtV;
    Qt.GetFqOt(MedQtV, TmUnit, utMedia, *this);
    Qt.GetFqOt(BlgQtV, TmUnit, utBlog, *this);
    TFltPrV FracV;
    const int PeakTm = Qt.GetPeakTm(TmUnit, TSecTm(1), utUndef, *this);
    //const int PeakTm = Qt.GetMeanTm(TmUnit, utUndef, *this);
    //const int PeakTm = Qt.GetMedianTm(TmUnit, utUndef, *this);
    for (int m=0, b=0; m < MedQtV.Len(); m++) {
      while (b<BlgQtV.Len() && BlgQtV[b].Val1 < MedQtV[m].Val1) { b++; }
      if (b<BlgQtV.Len() && BlgQtV[b].Val1 == MedQtV[m].Val1) {
        const double Tm = (int(MedQtV[m].Val1.Round(TmUnit))-PeakTm)/(24*3600.0);
        //if (BlgQtV[b].Val2>1 && MedQtV[m].Val2>1) {
          const double Frac = BlgQtV[b].Val2/double(BlgQtV[b].Val2+MedQtV[m].Val2);
          FracV.Add(TFltPr(Tm, Frac));
          TmMomH.AddDat(Tm).Add(Frac);
        //}[
        TFltPr& BM = TmBmH.AddDat(Tm);
        BM.Val1+= BlgQtV[b].Val2;
        BM.Val2+= MedQtV[m].Val2;
      }
    }
    //TGnuPlot::PlotValV(FracV, TStr::Fmt("fqFrac-%s-%02d", OutFNmPref.CStr(), c+1), "", "time [days]", "fraction of blog mentions");
  }
  { TFltPrV PrV;
  for (int b = 0; b < TmMomH.Len(); b++) {
    TmMomH[b].Def();
    PrV.Add(TFltPr(TmMomH.GetKey(b), TmMomH[b].GetMean())); }
  TGnuPlot GP(TStr::Fmt("fqFracA-%s", OutFNmPref.CStr()));
  PrV.Sort();  GP.AddPlot(PrV, gpwLinesPoints);
  GP.AddCmd("set xrange [-7:7]\nset yzeroaxis lt -1"); GP.SavePng(); }
  { TFltPrV PrV;
  for (int b = 0; b < TmMomH.Len(); b++) {
    PrV.Add(TFltPr(TmMomH.GetKey(b), TmMomH[b].GetMedian())); }
  TGnuPlot GP(TStr::Fmt("fqFracM-%s", OutFNmPref.CStr()));
  PrV.Sort();  GP.AddPlot(PrV, gpwLinesPoints);
  GP.AddCmd("set xrange [-7:7]\nset yzeroaxis lt -1"); GP.SavePng(); }
  { TFltPrV PrV;
  for (int b = 0; b < TmBmH.Len(); b++) {
    PrV.Add(TFltPr(TmBmH.GetKey(b), TmBmH[b].Val1/(TmBmH[b].Val1+TmBmH[b].Val2))); }
  TGnuPlot GP(TStr::Fmt("fqFracS-%s", OutFNmPref.CStr()));
  PrV.Sort(); GP.AddPlot(PrV, gpwLinesPoints);
  GP.AddCmd("set xrange [-7:7]\nset yzeroaxis lt -1"); GP.SavePng(); }
}

void TQuoteBs::PlotPopularityCnt(const bool& TakeClusters, const TUrlTy& UrlTy, const int& PlotN, const TStr& OutFNmPref) const {
  TIntV IdV;
  TIntH FqCntH;
  TQuote Qt;
  //if (TakeClusters) { GetCIdVByFq(IdV, 0, "", UrlTy, false); }
  //else { GetQtIdVByFq(IdV, 0, 0, false, "", UrlTy); }
  /*for (int c = 0; c < TMath::Mn(PlotN, IdV.Len()); c++) {
    if (TakeClusters) { GetMergedClustQt(IdV[c], Qt, false); }
    else { Qt = GetQt(IdV[c]); }
    FqCntH.AddDat(Qt.GetFq(UrlTy, *this)) += 1;
  }*/
  for (int c = 0; c < Len(); c++) {
    const TQuote& Qt = GetQtN(c);
    if (Qt.GetStr().IsStrIn("lipstick") && Qt.GetStr().IsStrIn("pig")) {
      FqCntH.AddDat(Qt.GetFq(UrlTy, *this)) += 1; }
  }
  TIntPrV FqCntV, CdfV;
  FqCntH.GetKeyDatPrV(FqCntV);  FqCntV.Sort();
  TGUtil::GetCCdf(FqCntV, CdfV);
  TGnuPlot::PlotValV(FqCntV, "pop-"+OutFNmPref, "", "total frequency", "count", gpsLog10XY);
  TGnuPlot::PlotValV(CdfV, "popCDF-"+OutFNmPref+"", "", "total frequency", "NCDF", gpsLog10XY);
}

void TQuoteBs::PlotEmptyY(const TTmUnit& TmUnit, const bool& TakeClusters, const int& PlotN, const TStr& OutFNmPref) const {
  TQuote Qt;
  TIntV IdV;
  THash<TInt, TInt> TmCumCntH;
  int MaxY=0;
  if (TakeClusters) { GetCIdVByFq(IdV, 10, "", utUndef, false); }
  else { GetQtIdVByFq(IdV, 8, 10, false, "", "", utUndef); }
  for (int c = 0; c < PlotN; c++) {
    if (TakeClusters) { GetMergedClustQt(IdV[c], Qt, false); }
    else { Qt = GetQt(IdV[c]); }
    TQuote::TTmFltPrV QtV;
    Qt.GetFqOt(QtV, TmUnit);
    for (int i = 0; i < QtV.Len(); i++) {
      const int Tm = QtV[i].Val1.Round(tmu1Hour)/3600;
      TInt& CumCnt = TmCumCntH.AddDat(Tm);
      CumCnt += (int) QtV[i].Val2;
      MaxY = TMath::Mx(CumCnt(), MaxY);
    }
  }
  TmCumCntH.SortByKey(true);
  const int BegHr = TmCumCntH.GetKey(0);
  THash<TFlt, TFlt> EmptyH;
  TFltPrV TmV;
  for (int t = 0; t < TmCumCntH.Len(); t++) {
    printf("%d  ", TmCumCntH[t]());
    for (int y = TmCumCntH[t]; y < MaxY; y+=1) {
      EmptyH.AddDat(y) += 1; }
    TmV.Add(TFltPr((TmCumCntH.GetKey(t)-BegHr)/24.0, TmCumCntH[t].Val));
  }
  EmptyH.SortByKey(false);
  TFltPrV EmptyV;
  EmptyV.Add(TFltPr(EmptyH.GetKey(0), EmptyH[0]));
  for (int i = 1; i < EmptyH.Len(); i++) {
    EmptyV.Add(TFltPr(EmptyH.GetKey(i), EmptyH[i]+EmptyV.Last().Val2));
  }
  TGnuPlot::PlotValCntH(EmptyH, "emptyF-"+OutFNmPref+"-d", "Amount of empty area above the cummulative query-frequency curve", "y value", "area at y");
  TGnuPlot::PlotValV(EmptyV, "emptyF-"+OutFNmPref+"-c", "Amount of empty area above the cummulative query-frequency curve", "y value", "area above y");
  TGnuPlot::PlotValV(TmV, "emptyF1"+OutFNmPref, "Cummulative query frequency over time", "time (days)", "frequency");
}

// load dataset and return a hiperlink graph between documents that contain quotes
// node ids are string url ids from QtBs
PNGraph TQuoteBs::GetQuotePostNet(const TStr& DatasetFNm) const {
  PNGraph Graph = TNGraph::New();
  for (TMemesDataLoader Memes(DatasetFNm); Memes.LoadNext(); ) {
    if (Memes.MemeV.Empty()) { continue; }
    if (! IsStr(Memes.PostUrlStr.CStr())) { continue; }
    const int SrcNId = GetStrId(Memes.PostUrlStr.CStr());
    for (int u = 0; u < Memes.LinkV.Len(); u++) {
      if (! IsStr(Memes.LinkV[u].CStr())) { continue; }
      const int DstNId = GetStrId(Memes.LinkV[u].CStr());
      if (! Graph->IsNode(SrcNId)) { Graph->AddNode(SrcNId); }
      if (! Graph->IsNode(DstNId)) { Graph->AddNode(DstNId); }
      Graph->AddEdge(SrcNId, DstNId);
    }
  }
  TSnap::PrintInfo(Graph);
  return Graph;
}

PQtDomNet TQuoteBs::GetQuoteDomNet(const PNGraph& PostGraph, const int& CId) const {
  TQuote Qt;  GetMergedClustQt(CId, Qt, false);
  PQtDomNet DomG = TQtDomNet::New();
  TIntSet UrlSet;
  for (int i = 0; i < Qt.GetTimes(); i++) { UrlSet.AddKey(Qt.GetUrlId(i)); }
  TStrSet DomH;
  for (int i = 0; i < Qt.GetTimes(); i++) {
    const int url = Qt.GetUrlId(i);
    if (! PostGraph->IsNode(url)) { continue; }
    const int dom = DomH.AddKey(TStrUtil::GetDomNm2(GetStr(url)));
    TNGraph::TNodeI NI = PostGraph->GetNI(url);
    for (int o = 0; o < NI.GetOutDeg(); o++) {
      if (! UrlSet.IsKey(NI.GetOutNId(o))) { continue; }
      const int url2 = NI.GetOutNId(o);
      const int dom2 = DomH.AddKey(TStrUtil::GetDomNm2(GetStr(url2)));
      if (dom==dom2) { continue; }
      if (! DomG->IsNode(dom)) {
        DomG->AddNode(dom, TPair<TStr, TInt>(DomH[dom], GetUrlTy(url))); }
      if (! DomG->IsNode(dom2)) {
        DomG->AddNode(dom2, TPair<TStr, TInt>(DomH[dom2], GetUrlTy(url2))); }
      if (DomG->IsEdge(dom, dom2)) {
        DomG->GetEDat(dom, dom2) += 1; }
      else {
        DomG->AddEdge(dom, dom2, 1); }
    }
  }
  return DomG;
}

void TQuoteBs::SaveQuotes(const int& MinQtFq, const TStr& OutFNm) const {
  TIntV QtIdV;
  GetQtIdVByFq(QtIdV, 0, MinQtFq);
  SaveQuotes(QtIdV, OutFNm);
}

void TQuoteBs::SaveQuotes(const TIntV& QtIdV, const TStr& OutFNm) const {
  TIntSet QtIdSet;
  FILE *F = fopen(OutFNm.CStr(), "wt");
  printf("saving %d quotes\n", QtIdV.Len());
  fprintf(F, "#Freq\tUrls\tDomains\tQuote\n");
  for (int q = 0; q < QtIdV.Len(); q++) {
    const int QtId = QtIdV[q];
    if (QtIdSet.IsKey(QtId)) { continue; } // save each quote only once
    QtIdSet.AddKey(QtId);
    const TQuote& Q = GetQt(QtId);
    //fprintf(F, "\n[%d] %d=%d %s \t%d\t%d\t%d\n", TStrUtil::CountWords(Q.QtStr.CStr()), QtId, Q.GetCId(), Q.QtStr.CStr(), Q.GetFq(), Q.GetUrls(), Q.GetDoms(*this));
  fprintf(F, "%d\t%d\t%d\t%s\n", Q.GetFq(), Q.GetUrls(), Q.GetDoms(*this), Q.QtStr.CStr());
    if (Q.GetTy() == qtRoot || Q.GetTy() == qtCentr) { // cluster root or centroid
      IAssert(ClustQtIdVH.IsKey(Q.GetCId()));
      const TIntV& ClustV = ClustQtIdVH.GetDat(Q.GetCId());
      for (int i = 0; i < ClustV.Len(); i++) {
        const TQuote& Q1 = GetQt(ClustV[i]);
        fprintf(F, "\t[%d] %d=%d %s\t%d\t%d\t%d\n", TStrUtil::CountWords(Q1.QtStr.CStr()),
          //ClustV[i], GetQtId(Q1.GetStr().CStr()),
          ClustV[i](), Q1.GetCId(), Q1.QtStr.CStr(), Q1.GetFq(), Q1.GetUrls(), Q1.GetDoms(*this));
        QtIdSet.AddKey(ClustV[i]);
      }
    }
  }
  fclose(F);
}
// save: root qt, all members, all urls tms for each memeber
void TQuoteBs::SaveClusters(const TStr& OutFNm, const bool& SkipUrls) const {
  TIntPrV FqCIdV;
  TIntH ClSzCntH, ClFqCntH;
  for (int c = 0; c < GetClusts(); c++) {
    const int CId = GetCId(c);
    if ( ! IsQtId(CId)) { printf("!!! %d:%d\n", c, CId); continue; }
    FqCIdV.Add(TIntPr(GetClustFq(CId), CId));
    ClSzCntH.AddDat(GetClust(CId).Len()) += 1;
    ClFqCntH.AddDat(GetClustFq(CId)) += 1;
  }
  TGnuPlot::PlotValCntH(ClSzCntH, OutFNm+"-clSz", TStr::Fmt("%d clusters", GetClusts()), "Number of quotes in the cluster", "Number of clusters", gpsLog);
  TGnuPlot::PlotValCntH(ClFqCntH, OutFNm+"-clFq", TStr::Fmt("%d clusters", GetClusts()), "Volume of quotes in the cluster", "Number of clusters", gpsLog, false, gpwLinesPoints, false, false);
  TGnuPlot::PlotValCntH(ClFqCntH, OutFNm+"-clFqB", TStr::Fmt("%d clusters", GetClusts()), "Volume of quotes in the cluster", "Number of clusters", gpsLog, false, gpwLinesPoints, false, true);
  FqCIdV.Sort(false);
  TIntPrV QtFqV;
  FILE *F = fopen(TStr::Fmt("%s-clust.txt", OutFNm.CStr()).CStr(), "wt");
  fprintf(F, "format:\n<ClSz>\t<TotFq>\t<Root>\t<ClusterId>\n");
  fprintf(F, "\t<QtFq>\t<Urls>\t<QtStr>\t<QuteId>\n");
  fprintf(F, "\t\t<Tm>\t<Fq>\t<UrlTy>\t<Url>\n\n");
  for (int c = 0; c < FqCIdV.Len(); c++) {
    const int CId = FqCIdV[c].Val2;
    const TIntV& ClustV = GetClust(CId);
    fprintf(F, "\n%d\t%d\t%s\t%d\n", ClustV.Len(), FqCIdV[c].Val1(), GetQt(CId).GetStr().CStr(), CId);
    QtFqV.Clr(false);
    for (int q = 0; q < ClustV.Len(); q++) {
      IAssert(IsQtId(ClustV[q]));
      QtFqV.Add(TIntPr(GetQt(ClustV[q]).GetUrls(), ClustV[q])); }
    QtFqV.Sort(false);
    for (int q = 0; q < QtFqV.Len(); q++) {
      IAssert(IsQtId(QtFqV[q].Val2));
      const TQuote& Qt = GetQt(QtFqV[q].Val2);
      if ((! SkipUrls) && q > 0) { fprintf(F, "\n"); }
      fprintf(F, "\t%d\t%d\t%s\t%d\n", Qt.GetFq(), Qt.GetUrls(), Qt.GetStr().CStr(), QtFqV[q].Val2());
      if (! SkipUrls) {
        for (int u = 0; u < Qt.GetUrls(); u++) {
          fprintf(F, "\t\t%s\t%d\t%s\t%s\n", Qt.TmUrlCntV[u].Tm().GetYmdTmStr().CStr(), Qt.TmUrlCntV[u].Cnt(),
            GetUrlTy(Qt.TmUrlCntV[u].UrlId())==utMedia?"M":"B", GetStr(Qt.TmUrlCntV[u].UrlId())); }
      }
    }
  }
  fclose(F);
}

// save: root qt, all members, all urls tms for each memeber
void TQuoteBs::SaveClusters(const TIntV& QtIdV, const TStr& OutFNm, const bool& SkipUrls) const {
  FILE *F = fopen(TStr::Fmt("%s-clust.txt", OutFNm.CStr()).CStr(), "wt");
  fprintf(F, "format:\n<ClSz>\t<TotFq>\t<Root>\t<ClusterId>\n");
  fprintf(F, "\t<QtFq>\t<Urls>\t<QtStr>\t<QuteId>\n");
  fprintf(F, "\t\t<Tm>\t<Fq>\t<UrlTy>\t<Url>\n\n");
  for (int c = 0; c < QtIdV.Len(); c++) {
    int CId = QtIdV[c];
    if (! IsClust(CId)) { CId = GetQt(CId).GetCId(); }
    const TIntV& ClustV = GetClust(CId);
    fprintf(F, "\n%d\t%d\t%s\t%d\n", ClustV.Len(), GetClustFq(CId), GetQt(CId).GetStr().CStr(), CId);
    for (int q = 0; q < ClustV.Len(); q++) {
      const TQuote& Qt = GetQt(ClustV[q]);
      if ((! SkipUrls) && q > 0) { fprintf(F, "\n"); }
      fprintf(F, "\t%d\t%d\t%s\t%d\n", Qt.GetFq(), Qt.GetUrls(), Qt.GetStr().CStr(), ClustV[q]());
      if (! SkipUrls) {
        for (int u = 0; u < Qt.GetUrls(); u++) {
          fprintf(F, "\t\t%s\t%d\t%s\t%s\n", Qt.TmUrlCntV[u].Tm().GetYmdTmStr().CStr(), Qt.TmUrlCntV[u].Cnt(),
            GetUrlTy(Qt.TmUrlCntV[u].UrlId())==utMedia?"M":"B", GetStr(Qt.TmUrlCntV[u].UrlId())); }
      }
    }
  }
  fclose(F);
}

void TQuoteBs::SaveBigBlogMassQt(const TStr& OutFNm) const {
  const TTmUnit TmUnit = tmuDay;
  const bool TakeClusters = true;
  TIntV IdV;
  TQuote Qt;
  int Candidates=0;
  if (TakeClusters) { GetCIdVByFq(IdV, 10, "", utUndef, false); }
  else { GetQtIdVByFq(IdV, 8, 10, false, "", "", utUndef); }
  TVec<TPair<TFlt, TStr> > ScoreV;
  for (int c = 0; c < IdV.Len(); c++) {
    if (TakeClusters) { GetMergedClustQt(IdV[c], Qt, false); }
    else { Qt = GetQt(IdV[c]); }
    //if (TStrUtil::CountWords(Qt.GetStr().CStr()) < 10) { continue; }
    const int MediaPeak = Qt.GetPeakTm(TmUnit, TSecTm(1), utMedia, *this); // media peak
    //const double BlogFq = Qt.GetFq(TSecTm(1), TSecTm(MediaPeak-7*24*3600), utBlog, *this);  // Fq 7 week before the media peak
    //const double TotFq = Qt.GetFq();
    const double BlogFq = Qt.GetUrls(TSecTm(MediaPeak-21*24*3600), TSecTm(MediaPeak-7*24*3600), utBlog, *this);  // Fq 7 week before the media peak
    const double TotFq = Qt.GetUrls();
    //const double Score = BlogFq/TotFq;
    if (BlogFq < 10) { continue; }
    Candidates++;
    if (BlogFq < TotFq*0.15 || BlogFq > TotFq*0.75) { continue; }
    //if (Score > 0.5) { continue; }
    ScoreV.Add(TFltStrPr(TotFq, TStr::Fmt("%g\t%g\t%s", BlogFq, TotFq, Qt.GetStr().CStr())));
    /*if (TotFq > 500) {
      TFltFltH MediaCntH, BlogCntH;
      for (int i = 0; i < Qt.TmUrlCntV.Len(); i++) {
        const double Hr = (double(Qt.TmUrlCntV[i].Tm().Round(tmuDay))-double(MediaPeak))/(24*3600.0);
        if (GetUrlTy(Qt.TmUrlCntV[i].UrlId()) == utMedia) { MediaCntH.AddDat(Hr) += Qt.TmUrlCntV[i].Cnt(); }
        else { BlogCntH.AddDat(Hr) += Qt.TmUrlCntV[i].Cnt(); }
      }
      TGnuPlot GP(TStr::Fmt("qt-%05d", int(TotFq)), Qt.GetStr());
      TFltPrV MV, BV; MediaCntH.GetKeyDatPrV(MV); BlogCntH.GetKeyDatPrV(BV); MV.Sort(); BV.Sort();
      GP.AddPlot(MV, gpwLinesPoints, "MEDIA");
      GP.AddPlot(BV, gpwLinesPoints, "BLOGS");
      GP.SavePng();
    }*/
  }
  printf("Considered: %d quotes\n", IdV.Len());
  printf("            %d candidates\n", Candidates);
  printf("            %d selected\n", ScoreV.Len());
  ScoreV.Sort(false);
  FILE *F = fopen(TStr::Fmt("%s.txt", OutFNm.CStr()).CStr(), "wt");
  fprintf(F, "total %d items\n", IdV.Len());
  fprintf(F, "items above blog threshdolg %d\n", ScoreV.Len());
  fprintf(F, "counts are numbers of urls (not actually number of occurences)\n");
  fprintf(F, "\nblog\ttotal\tQuote\n");
  for (int i = 0; i < ScoreV.Len(); i++) {
    fprintf(F, "%s\n", ScoreV[i].Val2.CStr());
  }
  fclose(F);
}

// save for flash visualization:
//  File: <date_hour> <quote_id> <raw_freq> <smooth_freq>
//  File  <qoote_id> <quote> <quote_url>
void TQuoteBs::SaveForFlash(const TIntV& QtIdV, const TStr& OutFNm, const TTmUnit& TmUnit, int SaveN, const TSecTm& BegTm, const TSecTm& EndTm, const TUrlTy& OnlyCountTy) const {
  const int TmUnitSecs = TTmInfo::GetTmUnitSecs(TmUnit);
  //TVec<TQuad<TInt, TInt, TInt, TInt> > QtInfoH; // QtId --> (peak time, QtStr, take url from quote QtId, FqOverTmV)
  TVec<TQuad<TFlt, TInt, TInt, TQuote::TTmFltPrV> > QtInfoV, QtInfoV2; // (PeakTm, CentrQtId, TotFq, RawFqOt)
  TQuote::TTmFltPrV FqV, SmoothFqV;
  TIntV VizQtIdV;
  if (SaveN == -1) { SaveN = TInt::Mx; }
  TSecTm MinTm=TSecTm(TInt::Mx-1), MaxTm=TSecTm(1);
  //TIntSet QSet, CSet;
  printf("Saving top %d out of %d quotes\n", SaveN, QtIdV.Len());
  THash<TInt, TIntPr> WeekPeakQtIdH; // top quote of each week
  for (int q = 0; q < QtIdV.Len() && QtInfoV2.Len() < (SaveN+100); q++) { // take top SaveN
    const TQuote& Qt = GetQt(QtIdV[q]);
    if (Qt.GetTy() == qtQuote) {
      Qt.GetFqOt(FqV, TmUnit, BegTm, EndTm, OnlyCountTy, *this);
      if (FqV.Empty()) { continue; }
      QtInfoV2.Add(TQuad<TFlt, TInt, TInt, TQuote::TTmFltPrV>((double)Qt.GetPeakTm(TmUnit, BegTm)+1e-6*Qt.GetUrls(), QtIdV[q], Qt.GetFq(), FqV));
      MinTm = TMath::Mn(MinTm, FqV[0].Val1);
      MaxTm = TMath::Mx(MaxTm, FqV.Last().Val1);
      VizQtIdV.Add(QtIdV[q]);
      TInt PeakFq;
      const int PeakTm = Qt.GetPeakTm(tmuWeek, BegTm, PeakFq).GetAbsSecs();
      if (! WeekPeakQtIdH.IsKey(PeakTm)) {
        WeekPeakQtIdH.AddDat(PeakTm, TIntPr(PeakFq, QtInfoV2.Len()-1)); }
      else if (WeekPeakQtIdH.GetDat(PeakTm).Val1<PeakFq) {
        WeekPeakQtIdH.AddDat(PeakTm, TIntPr(PeakFq, QtInfoV2.Len()-1)); }
    } else { // quote cluster
      const int CId = Qt.GetCId();
      if (! ClustQtIdVH.IsKey(CId)) { continue; }
      const int CentrQtId = GetCentrQtId(CId);
      TQuote CentrQt;  GetMergedClustQt(CId, CentrQt, true);
      //printf("%d\t%s\n", CId, CentrQt.GetStr().CStr());
      CentrQt.GetFqOt(FqV, TmUnit, BegTm, EndTm, OnlyCountTy, *this);
      if (FqV.Empty()) { continue; }
      QtInfoV2.Add(TQuad<TFlt, TInt, TInt, TQuote::TTmFltPrV>((double)CentrQt.GetPeakTm(TmUnit, BegTm)+1e-6*CentrQt.GetUrls(), CentrQtId, GetClustFq(CId), FqV));
      TInt PeakFq;
      const int PeakTm = CentrQt.GetPeakTm(tmuWeek, BegTm, PeakFq).GetAbsSecs();
      if (! WeekPeakQtIdH.IsKey(PeakTm)) {
        WeekPeakQtIdH.AddDat(PeakTm, TIntPr(PeakFq, QtInfoV2.Len()-1)); }
      else if (WeekPeakQtIdH.GetDat(PeakTm).Val1<PeakFq) {
        WeekPeakQtIdH.AddDat(PeakTm, TIntPr(PeakFq, QtInfoV2.Len()-1)); }
      MinTm = TMath::Mn(MinTm, FqV[0].Val1);
      MaxTm = TMath::Mx(MaxTm, FqV.Last().Val1);
      VizQtIdV.Add(CentrQtId);
    } //*/
  }
  // make sure that for each week there is a quote that peaks
  { TIntSet QtIdSet;
  // first take top quite of that week
  for (int i = 0; i < WeekPeakQtIdH.Len(); i++) {
    //QtInfoV.Add(QtInfoV2[WeekPeakQtIdH[i].Val2]);
    //QtIdSet.AddKey(WeekPeakQtIdH[i].Val2);
  }
  QtInfoV.Clr(true);
  // fill with other top quotes
  for (int i = 0; i < TMath::Mn(SaveN, QtInfoV2.Len()); i++) {
    if (QtIdSet.IsKey(i)) { continue; }
    QtInfoV.Add(QtInfoV2[i]);
  printf("%d  ", (int) QtInfoV2[i].Val3()); //(PeakTm, CentrQtId, TotFq, RawFqOt)
    if (QtInfoV.Len() > SaveN) { break; }
  } } // */
  // make sure there are only K quotes active at a time
  THash<TSecTm, TIntFltH> TmCntH; // for each time, get all quote Ids and frequencies
  for (int i = 0; i < QtInfoV.Len(); i++) {
    const TQuote::TTmFltPrV& FqV = QtInfoV[i].Val4;
    for (int t = 0; t < FqV.Len(); t++) {
      TmCntH.AddDat(FqV[t].Val1).AddDat(i, FqV[t].Val2); }
  }
  for (int i = 0; i < TmCntH.Len(); i++) {
    TmCntH[i].SortByDat(false); }
  const int K = 10;
  const int Slack = 3*24*3600; // give it additional 3 days
  THash<TInt, TSecTm> QtLastTopH; // for each quote last time they were in top K
  for (int i = 0; i < TmCntH.Len(); i++) {
    TIntFltH& TmH = TmCntH[i];
    for (int j = 0; j < TMath::Mn(K, TmH.Len()); j++) {
      QtLastTopH.AddDat(TmH.GetKey(j), TmCntH.GetKey(i));
    }
  }
  // delete all appearances after the quote moves out of top K
  for (int i = 0; i < QtInfoV.Len(); i++) {
    TQuote::TTmFltPrV& FqV = QtInfoV[i].Val4;
    const TSecTm LastTime = QtLastTopH.GetDat(i);
    int x = 0; for (x = 0; x < FqV.Len() && FqV[x].Val1 <= LastTime+Slack; x++) { }
    if (x < FqV.Len()) { FqV.Del(x, FqV.Len()-1); }
  } // */
  printf("save from %d quotes\n", QtInfoV.Len());
  QtInfoV.Sort();
  TIntH QtIdUrlH;  GetQtPageUrl(VizQtIdV, QtIdUrlH);
  // save quote info
  FILE *F = fopen(TStr::Fmt("qtOt-%sQ.tab", OutFNm.CStr()).CStr(), "wt");
  fprintf(F, "QuoteId\tQuote\tUrl\tTotalVol\n");
  for (int i = 0; i < TMath::Mn(SaveN, QtInfoV.Len()); i++) {
    fprintf(F, "%d\t%s\t%s\t%d\n", SaveN-i, GetQt(QtInfoV[i].Val2).QtStr.CStr(), GetStr(QtIdUrlH.GetDat(QtInfoV[i].Val2)), (int) QtInfoV[i].Val3);
  }
  fclose(F);
  printf("saveT\n");
  // save quote time frequency
  F = fopen(TStr::Fmt("qtOt-%sT.tab", OutFNm.CStr()).CStr(), "wt");
  fprintf(F, "Time\tQuoteId\tSmoothFreq\tRawFreq\n");
  for (int i = 0; i < TMath::Mn(SaveN, QtInfoV.Len()); i++) {
    const TQuote::TTmFltPrV RawFqV = QtInfoV[i].Val4;
    //TQuote::GetSmoothFqOt(SmoothFqV, RawFqV, TmUnit, 6*24*3600/TmUnitSecs, 1.2, MinTm, MaxTm); // less smoothing
    TQuote::GetSmoothFqOt(SmoothFqV, RawFqV, TmUnit, 6*24*3600/TmUnitSecs, 1.05, MinTm, MaxTm);  // more smootning
    //SmoothFqV = RawFqV;
    IAssert(SmoothFqV.Len() >= RawFqV.Len());
    for (int d=0, ds = 0; ds < SmoothFqV.Len(); ds++) {
      if (TmUnit == tmuDay) { fprintf(F, "%s", SmoothFqV[ds].Val1.GetDtYmdStr().CStr()); }
      else { fprintf(F, "%s_%02d", SmoothFqV[ds].Val1.GetDtYmdStr().CStr(), SmoothFqV[ds].Val1.GetHourN()); }
      fprintf(F, "\t%d\t%.2f", SaveN-i, SmoothFqV[ds].Val2());
      if (d < RawFqV.Len() && SmoothFqV[ds].Val1 == RawFqV[d].Val1) { fprintf(F, "\t%.0f", RawFqV[d].Val2()); d++; } else { fprintf(F, "\t0"); }
      fprintf(F, "\n");
    }
    //if (QtIdV[N] == MaxStepsQId) { // add zero counts to fill the interval [MinTm, MaxTm]
    //  for (TSecTm Tm = SmoothFqV.Last().Val1; Tm.GetInUnits(TmUnit) < MaxTm.GetInUnits(TmUnit); Tm += TmUnitSecs) {
    //    if (TmUnit == tmuDay) { fprintf(F, "%s", Tm.GetDtYmdStr().CStr()); }
    //    else { fprintf(F, "%s_%02d", Tm.GetDtYmdStr().CStr(), Tm.GetHourN()); }
    //    fprintf(F, "\t%d\t0\t0\n", SaveN-i); } }
  }
  fclose(F);
  printf("done.\n");
}

void TQuoteBs::SaveDomainStat(const TStr& OutFNm, const int& MinCnt) const {
  THash<TInt, TInt> MedOt, BlogOt, AllOt;
  int MedCnt=0, BlogCnt=0, AllCnt=0;
  TSecTm BegTm(2008,7,31,0,0,0);
  THash<TStr, TIntPr> DomCntH;
  for (int q = 0; q < QuoteH.Len(); q++) {
    const TQuote& Q = QuoteH[q];
    for (int u = 0; u < Q.GetUrls(); u++) {
      const int Day = TSecTm(Q.GetTm(u)-BegTm).GetInUnits(tmuDay);
      AllOt.AddDat(Day) += 1;  AllCnt++;
      if (GetUrlTy(Q.GetUrlId(u)) ==  utMedia) { MedOt.AddDat(Day) += 1;  MedCnt++;}
      else { BlogOt.AddDat(Day) += 1;  BlogCnt++; }
      TStr Dom = TStrUtil::GetDomNm2(GetStr(Q.GetUrlId(u)));
      DomCntH.AddDat(Dom).Val1 += 1;
      DomCntH.AddDat(Dom).Val2 += Q.GetCnt(u);
    }
  }
  TGnuPlot::PlotValCntH(AllOt, "ALL", MedOt, "MEDIA", BlogOt, "BLOGS", "overTm-"+OutFNm, TStr::Fmt("Quote freq over time: A:%d M:%d B:%d", AllCnt, MedCnt, BlogCnt),
    "Time [days]", "Number of quote mentions");
  DomCntH.SortByDat(false);
  FILE *F = fopen(TStr::Fmt("domains-%s.tab", OutFNm.CStr()).CStr(), "wt");
  fprintf(F, "# %d domains, total mentions:\tAll:%d\tMedia:%d\tBlogs:%d", DomCntH.Len(), AllCnt, MedCnt, BlogCnt);
  fprintf(F, "#Domain\tNumber of quotes mentioned\tNumber of mentions\n");
  for (int d = 0; d < DomCntH.Len(); d++) {
    fprintf(F, "%s\t%d\t%d\n", DomCntH.GetKey(d).CStr(), DomCntH[d].Val1(), DomCntH[d].Val2());
  }
  fclose(F);
}

void TQuoteBs::Dump(const bool& Fast) const {
  printf("Quote base:\n");
  if (! Fast) {
    TSecTm MinTm, MaxTm; GetMinMaxTm(MinTm, MaxTm);
    printf("  %s -- %s\n", MinTm.GetYmdTmStr().CStr(), MaxTm.GetYmdTmStr().CStr());
    int totCnt=0, totVol=0;
    for (int q = 0; q < QuoteH.Len(); q++) {
      const TQuote::TTmUrlCntV& V = QuoteH[q].TmUrlCntV;
      totCnt += V.Len();
      for (int u = 0; u < V.Len(); u++) {
        totVol += V[u].Cnt();
      }
    }
    printf("  %d number of posts mentioning quotes\n", totCnt);
    printf("  %d number of mentions\n", totVol);
  }
  printf("  %d quotes\n", Len());
  printf("  %d strings (quotes+urls, no domains)\n", StrQtIdH.Len());
  printf("  %d clusters\n", ClustQtIdVH.Len());
  printf("  %d quotes in clusters\n", GetQtsInClust());
  printf("  %d urls with type\n\n", UrlTyH.Len());
}

// build QuoteBs of quotes in QuoteStrV but count their occurences in free text
PQuoteBs TQuoteBs::FindQtInTxtQBs(const TStr& InBinFNmWc, const TStrV& QuoteStrV) {
  THashSet<TMd5Sig> SeenUrlH; // skip duplicate urls
  PQuoteBs QtBsPt = TQuoteBs::New();
  TQuoteBs& QtBs = *QtBsPt;
  TStrHash<TInt> StrH;
  TVec<TIntV> QtWIdVV;
  TVec<TIntV> QtSkipLenVV; // if i-th word does not match, how much can we skip
  for (int q = 0; q < QuoteStrV.Len(); q++) {
    const int QId = QtBs.QuoteH.Len(); //QtBs.AddStr(QuoteStrV[q].CStr());
    TQuote& Qt = QtBs.QuoteH.AddDat(QId);
    Qt.QtCIdTy = TQuote::TQtIdTy(QId, qtQuote);
    Qt.QtStr = QuoteStrV[q];
    QtWIdVV.Add();  QtSkipLenVV.Add();
    TStrUtil::GetAddWIdV(StrH, QuoteStrV[q].CStr(), QtWIdVV.Last());
    TIntV& WIdV = QtWIdVV.Last();
    TIntV& SkipV = QtSkipLenVV.Last();
    SkipV.Add(1);
    for (int w = 1; w < WIdV.Len(); w++) {
      int skipLen = 1;
      while (w-skipLen >= 0 && WIdV[w-skipLen] != WIdV[w]) { skipLen++; }
      SkipV.Add(skipLen);
    }
  }
  TInt WId;
  TIntV TxtWIdV;
  TVec<char *> WrdV;
  TQuoteLoader QL(InBinFNmWc);
  //Fail; //!! chage so that we read the data directly from spinn3r parser
  while (QL.Next()) {
    if (SeenUrlH.IsKey(TMd5Sig(QL.PostUrlStr))) { continue; }
    SeenUrlH.AddKey(TMd5Sig(QL.PostUrlStr));
    // content word id vector (skip unknown words by insering -1)
    TxtWIdV.Clr();  WrdV.Clr(false);
    { TStrUtil::SplitWords(QL.ContentStr, WrdV);
    for (int w = 0; w < WrdV.Len(); w++) {
      if (StrH.IsKeyGetDat(WrdV[w], WId)) { TxtWIdV.Add(WId); }
      else if (TxtWIdV.Len()>0 && TxtWIdV.Last()!=-1) { TxtWIdV.Add(-1); }
    } }
    // count number of occurences
    for (int q = 0; q < QtWIdVV.Len(); q++) {
      const TIntV& QWIdV = QtWIdVV[q];
      const TIntV& SkipV = QtSkipLenVV[q];
      int cnt = 0;
      for (int w = 0; w < TxtWIdV.Len()-QWIdV.Len(); ) {
        int len = 0;
        while (len < QWIdV.Len() && TxtWIdV[w+len] == QWIdV[len]) { len++; }
        if (len == QWIdV.Len()) { cnt+=1;  w+=len; }
        else { w += SkipV[len]; }
      }
      if (cnt > 0) {
        QtBs.GetQtN(q).TmUrlCntV.Add(TQuote::TTmUrlCnt(QL.PubTm, QtBs.AddStr(QL.PostUrlStr), cnt));
        if (cnt > 255) { printf("*"); }
      }
    }
  }
  for (int i = 0; i < QtBs.Len(); i++) { QtBs.QuoteH[i].TmUrlCntV.Sort(); }
  return QtBsPt;
}



#ifdef false
// for a list of quotes calculate the counts of the all possible subsequences
// of a quote appearing on the full text. Store this into TQuote::SubSqCntH
void TQuoteBs::BuildQtSubSeqCounts(TIntV& QtIdV, const TStr& OutFNm) {
  /*const int NotWId = -1;
  TVec<char *> WrdV;
  TChA TxtStr;
  TIntV DocWIdV;  TInt WId;
  // build query word index for faster processing
  TStrHash<TInt> WrdIdH;
  THash<TInt, TIntV>  QtWIdH; // QtId --> WIdV
  THash<TInt, THash<TInt, TIntV> > QtIdWIdIdxH; // QtId --> (WId -> index in WIdV where it appears)
  for (int qt = 0; qt < QtIdV.Len(); qt++) {
    const TQuote& Q = QuoteH.GetDat(QtIdV[qt]);
    IAssert(Q.QtCId == QtIdV[qt]);
    TxtStr = Q.QtStr();  TStrUtil::SplitWords(TxtStr, WrdV);
    TIntV& QtWIdV = QtWIdH.AddDat(Q.QtCId);
    for (int w = 0; w < WrdV.Len(); w++) {
      QtWIdV.Add(WrdIdH.AddDatId(WrdV[w])); }
    THash<TInt, TIntV>& WIdIdxH = QtIdWIdIdxH.AddDat(Q.QtCId);
    for (int i = 0; i < QtWIdV.Len(); i++) {
      WIdIdxH.AddDat(QtWIdV[i]).Add(i); }
  }
  // search subs-equences
  int matchLen=0, bestMatch=0, bestMatchBeg=0;
  TQuoteLoader QL(InFNmWc);
  while (QL.Next()) {
    DocWIdV.Clr(false);
    TStrUtil::SplitWords(QL.ContentStr, WrdV);
    for (int w = 0; w < WrdV.Len(); w++) {
      if (WrdIdH.IsKeyGetDat(WrdV[w], WId)) { DocWIdV.Add(WId); }
      else if (! DocWIdV.Empty() && DocWIdV.Last()!=NotWId) { DocWIdV.Add(NotWId); }
    }
    for (int q = 0; q < QtWIdH.Len(); q++) {
      const int qtId = QtWIdH.GetKey(q);
      const TIntV& QtWIdV = QtWIdH.GetDat(qtId);
      const THash<TInt, TIntV>& QtWIdIdxH = QtIdWIdIdxH.GetDat(qtId);
      THash<TIntPr, TInt>& SubSqCntH = QuoteH.GetDat(qtId).SubSeqCntH;
      // find subsequences
      for (int w = 0; w < DocWIdV.Len(); ) {
        const int wid = DocWIdV[w];
        if (wid == NotWId || ! QtWIdIdxH.IsKey(wid)) { w++; continue; }
        const TIntV& WIdxV = QtWIdIdxH.GetDat(wid);
        bestMatch = 0;
        for (int wi = 0; wi < WIdxV.Len(); wi++) {
          const int widx = WIdxV[wi];
          for (matchLen=0; widx+matchLen<QtWIdV.Len() && w+matchLen<DocWIdV.Len() && QtWIdV[widx+matchLen]==DocWIdV[w+matchLen]; matchLen++) { }
          if (bestMatch < matchLen) {
            bestMatch = matchLen; bestMatchBeg = widx; }
        }
        SubSqCntH.AddDat(TIntPr(bestMatchBeg, bestMatchBeg+bestMatch)) += 1;
        w += bestMatch;
      }
    }
  }
  printf("  done\n");
  // save QuoteStat for selected quotes
  TFOut FOut(OutFNm);
  for (int q = 0; q < QtIdV.Len(); q++) {
    QuoteH.GetDat(QtIdV[q]).Save(FOut);
  }*/
}

void TQuoteBs::PlotQtFqOverTm(const TIntV& QtIdV, const TStr& OutFNm, int PlotN) const {
  TSecTm MinTm, MaxTm;  GetMinMaxTm(MinTm, MaxTm);
  TGnuPlot GP("fqOT."+OutFNm);
  TFltPrV HrFqV;
  if (PlotN == -1) { PlotN = TInt::Mx; }
  for (int i = 0; i < TMath::Mn(PlotN, QtIdV.Len()); i++) {
    const TQuote& Qt = GetQt(QtIdV[i]);
    Qt.GetSmoothFqOverTm(HrFqV, tmu1Hour, MinTm, 12, 1.2);
    for (int h = 0; h < HrFqV.Len(); h++) {
      HrFqV[h].Val1 = TMath::Round(HrFqV[h].Val1/24.0, 2);
    }
    GP.AddPlot(HrFqV, gpwLines, TStrUtil::GetShorStr(Qt.QtStr), "lt 1");
  }
  GP.SetXYLabel("Time [days]", "Frequency");
  GP.AddCmd("set nokey");
  GP.SavePng("fqOT."+OutFNm+".png", -1, -1, "", "set terminal png small size 3000,600");
  GP.AddCmd("set size 2,0.5");
  GP.SaveEps(10);
}

void TQuoteBs::PlotQtCumFqOverTm(const TIntV& QtIdV, const TStr& OutFNm, int PlotN) const {
  const int TmuSecs = TTmInfo::GetTmUnitSecs(tmu1Hour);
  TSecTm MinTm, MaxTm;  GetMinMaxTm(MinTm, MaxTm);
  const TSecTm StartTm = MinTm.Round(tmu1Hour);
  const TSecTm EndTm = MaxTm.Round(tmu1Hour);
  TQuote::TTmFltPrV HrFqV;
  TFltFltH SumCntH;
  for (TSecTm Tm = StartTm; Tm <= MaxTm.Round(tmu1Hour); Tm += TmuSecs) {
    SumCntH.AddDat(double(Tm-StartTm)/(24.0*3600.0)) = 0;
  }
  TGnuPlot GP("cfqOT."+OutFNm);
  if (PlotN == -1) { PlotN = TInt::Mx; }
  for (int i = 0; i < TMath::Mn(PlotN, QtIdV.Len()); i++) {
    const TQuote& Qt = GetQt(QtIdV[i]);
    Qt.GetSmoothFqOverTm(HrFqV, tmu1Hour, 48, 1.2);
    for (int h = 0; h < HrFqV.Len(); h++) {
      const TSecTm roundTm = HrFqV[h].Val1.Round(tmu1Hour);
      if (roundTm >= StartTm && roundTm<=EndTm) {
        SumCntH.AddDat(double(roundTm-StartTm)/(24.0*3600.0)) += HrFqV[h].Val2;
      }
    }
    GP.AddPlot(SumCntH, gpwLines, TStrUtil::GetShorStr(Qt.QtStr, 100).CStr());
  }
  GP.SetXYLabel(TStr::Fmt("Time [days] since %s", StartTm.GetYmdTmStr().CStr()), "Stacked frequency");
  GP.AddCmd("set notitle");
  GP.AddCmd("set nogrid");
  GP.AddCmd("set mxtics 4");
  //GP.SavePng();
  GP.SavePng("cfqOT."+OutFNm+".png", -1, -1, "", "set terminal png small size 1000,600");
}

void TQuoteBs::PlotQtCumFqOverTm1(const TIntV& QtIdV, const TStr& OutFNm, int PlotN) const {
  TSecTm MinTm, MaxTm;  GetMinMaxTm(MinTm, MaxTm);
  TGnuPlot GP("cfqOT."+OutFNm);
  TFltPrV HrFqV, HrCFqV;
  TFltFltH SumCntH;
  if (PlotN == -1) { PlotN = TInt::Mx; }
  for (int i = 0; i < TMath::Mn(PlotN, QtIdV.Len()); i++) {
    const TQuote& Qt = GetQt(QtIdV[i]);
    Qt.GetSmoothFqOverTm(HrFqV, tmu1Hour, MinTm, 12, 1.2);
    HrCFqV.Clr(false);
    for (int h = 0; h < HrFqV.Len(); h++) {
      const double hr = TMath::Round(HrFqV[h].Val1/24.0+0.001, 2);
      SumCntH.AddDat(hr) += HrFqV[h].Val2;
    }
    GP.AddPlot(SumCntH, gpwLines, TStrUtil::GetShorStr(Qt.QtStr).CStr(), "lt 1");
  }
  GP.SetXYLabel("Time [days]", "Stacked frequency");
  GP.AddCmd("set nokey");
  GP.AddCmd("set notitle");
  GP.AddCmd("set nogrid");
  GP.AddCmd("set mxtics 5");
  GP.AddCmd("set xtics 5");
  GP.SavePng("cfqOT."+OutFNm+".png", -1, -1, "", "set terminal png small size 3000,600");
  GP.AddCmd("set size 2,0.5");
  GP.SaveEps(8);
}


// save <time> <Qt1Fq> <Qt2Fq> ...
void TQuoteBs::SaveForMatlab(const TIntV& QtIdV, const TStr& OutFNm, int SaveN) const {
  const TTmUnit TmUnit = tmu1Hour;
  TSecTm MinTm, MaxTm;  GetMinMaxTm(MinTm, MaxTm);
  THash<TInt, TFlt> TmCntH;
  TVec<TFltV> TmQtFqV;
  const int MinTmUnit = MinTm.GetInUnits(TmUnit);
  for (uint i = MinTmUnit; i <= MaxTm.GetInUnits(TmUnit); i++) {
    TmCntH.AddDat(i, 0);  TmQtFqV.Add();
    TmQtFqV.Last().Add(i-MinTmUnit+1); // add time
  }
  TQuote::TTmFltPrV DayFqV;
  if (SaveN == -1) { SaveN = TInt::Mx; }
  for (int i = 0; i < TMath::Mn(SaveN, QtIdV.Len()); i++) {
    const TQuote& Qt = GetQt(QtIdV[i]);
    Qt.GetSmoothFqOverTm(DayFqV, TmUnit, 48, 1.2);
    for (int d = 0; d < TmCntH.Len(); d++) { TmCntH[d] = 0; }
    for (int d = 0; d < DayFqV.Len(); d++) {
      const int T = DayFqV[d].Val1.GetInUnits(TmUnit);
      if (TmCntH.IsKey(T)) {
        TmCntH.AddDat(T) += DayFqV[d].Val2; }
    }
    for (int d = 0; d < TmCntH.Len(); d++) {
      TmQtFqV[d].Add(TmCntH[d]); }
  }

  FILE *F = fopen(TStr::Fmt("qtFqOtMATLAB-%s.tab", OutFNm.CStr()).CStr(), "wt");
  for (int d = 0; d < TmQtFqV.Len(); d++) {
    fprintf(F, "%g", TmQtFqV[d][0]());
    for (int q = 1; q < TmQtFqV[d].Len(); q++) {
      fprintf(F, "\t%g", TmQtFqV[d][q]()); }
    fprintf(F, "\n");
  }
  fclose(F);
}

// save <quote> <time> <url>
void TQuoteBs::SaveQtTmUrlTxt(const TIntV& QtIdV, const TStr& OutFNm, int SaveN) const {
  if (SaveN == -1) { SaveN = TInt::Mx; }
  SaveN = TMath::Mn(SaveN, QtIdV.Len());
  FILE *F = fopen(OutFNm.CStr(), "wt");
  for (int q = 0; q < QtIdV.Len(); q++) {
    const TQuote& Qt = GetQt(QtIdV[q]);
    fprintf(F, "%s\t%d\n", Qt.QtStr.CStr(), Qt.GetFq());
    for (int u = 0; u < Qt.GetUrls(); u++) {
      fprintf(F, "\t%s\t%s\n", Qt.TmUrlCntV[u].Val1.GetYmdTmStr().CStr(), GetStr(Qt.TmUrlCntV[u].Val2));
    }
    fprintf(F, "\n");
  }
  fclose(F);
}

// plot Quote and all of its substrings that appear as quotes over time
void TQuoteBs::PlotSubQtOverTm(const int& QtId, const TStr& OutFNm) const {
  TIntV PlotQtV;
  const TQuote Qt = GetQt(QtId);
  PlotQtV.Add(QtId);
  TIntV WrdBegV; WrdBegV.Add(0);
  for (int i = 1; i < Qt.QtStr.Len()-1; i++) {
    if (TCh::IsAlNum(Qt.QtStr.GetCh(i)) && TCh::IsWs(Qt.QtStr.GetCh(i-1))) { WrdBegV.Add(i); }
  }
  TInt subQtId;
  WrdBegV.Add(Qt.QtStr.Len()+1); // one past last character
  const int QtWords = TStrUtil::CountWords(Qt.QtStr.CStr());
  for (int WndSz = 5; WndSz < QtWords; WndSz++) {
    for (int word = 0; word < QtWords-WndSz; word++) {
      const TStr SubQtStr = Qt.QtStr.GetSubStr(WrdBegV[word], WrdBegV[word+WndSz]-2);
      if (StrH.IsKeyGetDat(SubQtStr, subQtId) && QuoteH.IsKey(subQtId) && subQtId!=QtId) { PlotQtV.Add(subQtId); printf("."); }
    }
  }
  // plot
  TGnuPlot GP(OutFNm, TStr::Fmt("%d occurences, %d urls, %d domains: %s", Qt.GetFq(), Qt.GetUrls(), Qt.GetDoms(), Qt.QtStr.CStr()));
  TFltPrV HrFqV;
  for (int i = 0; i < PlotQtV.Len(); i++) {
    const TQuote& subQt = QuoteH.GetDat(PlotQtV[i]);
    subQt.GetSmoothFqOverTm(HrFqV, tmu1Hour);
    GP.AddPlot(HrFqV, gpwLines, subQt.QtStr.CStr());
  }
  GP.SetXYLabel("Time [hours]", "Frequency");
  GP.SavePng(OutFNm+".png", -1, -1, "", "set terminal png small size 1000,800");
}

void TQuoteBs::PlotTopQuotesOverTm(const int& StepSecs, const int& IntervalSecs, const int& TakeNPerStep, const TStr& OutFNm) const {
  TSecTm MinTm, MaxTm;  GetMinMaxTm(MinTm, MaxTm);
  printf("Dataset span %s -- %s\n", MinTm.GetStr().CStr(), MaxTm.GetStr().CStr());

  TIntSet TopQtIdSet;
  int cnt=0;
  TIntV TopQtIdV;
  for (TSecTm Tm(MinTm); Tm < MaxTm; Tm += StepSecs, cnt++) {
    //const TIntV TopQtIdV = GetTopLongQuotes(5, 0, TSecTm(Tm-IntervalSecs/2), TSecTm(Tm+IntervalSecs/2), 4, TakeNPerStep);
    FailR("TopQuotesOverTm: take top quotes from each day");
    for (int q = 0; q < TopQtIdV.Len(); q++) { TopQtIdSet.AddKey(TopQtIdV[q]); }
  }
  printf("\n%d time steps total, %d quotes per step: %d total quotes\n", cnt, TakeNPerStep, TopQtIdSet.Len());
  printf("%s   %d\n", MinTm.Round(tmuDay).GetStr().CStr(), MinTm.Round(tmuDay).GetInUnits(tmu1Hour));
  TFltPrV FqOverTm;
  TGnuPlot GP(TStr("topQtOverTm.")+OutFNm, "");
  TFOut FOut(OutFNm+TStr(".Quote"));
  for (int q = 0; q < TopQtIdSet.Len(); q++) {
    const TQuote& Q = QuoteH.GetDat(TopQtIdSet[q]);
    Q.Save(FOut);
    Q.GetSmoothFqOverTm(FqOverTm, tmu1Hour, MinTm.Round(tmuDay));
    GP.AddPlot(FqOverTm, gpwLines, Q.QtStr);
  }
  GP.AddCmd("set xtics 24"); GP.AddCmd("set mxtics 4");
  GP.SetXYLabel(TStr("time [hours] from "+MinTm.Round(tmuDay).GetStr()), "frequency");
  GP.SavePng(TStr("topQtOverTm.")+OutFNm+TStr(".png"), -1, -1, "", "set terminal png small size 2000,800");
}

PQuoteBs TQuoteBs::BuildQuoteBs(const TStr& InBinFNmWc, const int& MinQtWrdLen) {
  PQuoteBs QBsPt = TQuoteBs::New(InBinFNmWc);
  QBsPt->AddToQuoteBs(InBinFNmWc, MinQtWrdLen);
  return QBsPt;
}
#endif

/////////////////////////////////////////////////
// Make QuoteBase from Spinn3r data
/*
TMakeQtBsFromSpinn3r::TMakeQtBsFromSpinn3r(const PQuoteBs& QtBsPt, const int& MinWordLen) : TQuoteExtractor(false),
  QtBs(QtBsPt), MinWrdLen(MinWordLen) { // don't save post content in .ContentBin
}

void TMakeQtBsFromSpinn3r::Save(TSOut& SOut) const {
  Items.Save(SOut); DomSet.Save(SOut); UrlSet.Save(SOut); BlogSet.Save(SOut);
  WrdPerH.Save(SOut); ItmPerH.Save(SOut); PostPerH.Save(SOut); LinkPerH.Save(SOut); QtsPerH.Save(SOut); BytesPerH.Save(SOut);
}
void TMakeQtBsFromSpinn3r::SavePlots() {
  printf("** %d iterms\n%d domains\n%d unique post urls\n%d uniq blog urls\n", Items(), DomSet.Len(), UrlSet.Len(), BlogSet.Len());
  const TSecTm BegTm = TSecTm(2008, 1, 1); // beginning of time
  ItmPerH.SortByKey();  if (ItmPerH.Empty()) { return; }
  TStr TmStr = TStr::Fmt("%s -- %s", TSecTm(3600*ItmPerH.GetKey(0)+BegTm).GetYmdTmStr().CStr(),
    TSecTm(3600*ItmPerH.GetKey(ItmPerH.Len()-1)+BegTm).GetYmdTmStr().CStr());
  TGnuPlot::PlotValCntH(WrdPerH, "overTm-WordsPerH", TStr("Words per hour.")+TmStr, "Time [hours]", "Total words");
  TGnuPlot::PlotValCntH(ItmPerH, "overTm-ItemsPerH", TStr("Items (includes duplicates) per hour")+TmStr, "Time [hours]", "Total items (includes duplicates)");
  TGnuPlot::PlotValCntH(UniqItmH, "overTm-UniqItemsPerH", TStr("Unique Items (no duplicates) per hour")+TmStr, "Time [hours]", "Total items (includes duplicates)");
  TGnuPlot::PlotValCntH(LinkPerH, "overTm-LinksPerH", TStr("Links per hour")+TmStr, "Time [hours]", "Total links");
  TGnuPlot::PlotValCntH(QtsPerH, "overTm-QtsPerH", TStr("Quotes per hour")+TmStr, "Time [hours]", "Total quotes");
  TGnuPlot::PlotValCntH(BytesPerH, "overTm-BytesPerH", TStr("Bytes per hour")+TmStr, "Time [hours]", "Total bytes");
  TGnuPlot::PlotValCntH(PostPerH, "overTm-PostsPerH", TStr("Posts (good items) per hour")+TmStr, "Time [hours]", "Total Posts (good items)");
}

void TMakeQtBsFromSpinn3r::OnQuotesExtracted(const TQuoteExtractor& QuoteExtractor) {
  const TSecTm BegTm = TSecTm(2008, 1, 1); // beginning of time
  const int Tm = TSecTm(PubTm-BegTm).GetInUnits(tmu1Hour); // hours since Jan 1 2008
  const TMd5Sig UrlSig(PostUrlStr);
  ItmPerH.AddDat(Tm) += 1;  Items++;
  if (UrlSet.IsKey(UrlSig)) {
    return; // duplicate url
  }
  UrlSet.AddKey(UrlSig);
  DomSet.AddKey(TMd5Sig(TStrUtil::GetDomNm(PostUrlStr.CStr())));
  BlogSet.AddKey(TMd5Sig(BlogUrlStr));
  UniqItmH.AddDat(Tm) += 1;
  PostPerH.AddDat(Tm) += 1;
  WrdPerH.AddDat(Tm) += TStrUtil::CountWords(PostText);
  LinkPerH.AddDat(Tm) += LinkV.Len();
  QtsPerH.AddDat(Tm) += QuoteV.Len();
  BytesPerH.AddDat(Tm) += PostText.Len();
  // add quote to QuoteBase
  //for (int q = 0; q < QuoteV.Len(); q++) { // remove black listed quotes
  //  if (QtBlackList.IsKey(TMd5Sig(QuoteV[q]))) {
  //    QuoteV.Swap(q, QuoteV.Len()-1);  QuoteV.DelLast();  q--; }
  //}
  // add all quotes to the quote base
  const int OldL = QtBs->Len();
  QtBs->AddQuote(QuoteV, LinkV, PostUrlStr, PubTm, MinWrdLen); // add quote
  const int NewL = QtBs->Len();
  if (OldL > 0 && NewL % 100000 == 0 && NewL-OldL==1) {
    printf("** %d iterms\n%d domains\n%d unique post urls\n%d uniq blog urls\n", Items(), DomSet.Len(), UrlSet.Len(), BlogSet.Len());
    QtBs->Dump(true);
    if (NewL % 1000000 == 0) { SavePlots(); }
  }
}
//*/

/////////////////////////////////////////////////
// Cluster Network
PClustNet TClustNet::GetSubGraph(const TIntV& NIdV) const {
  PClustNet NewNetPt = TClustNet::New();
  TClustNet& NewNet = *NewNetPt;
  NewNet.Reserve(NIdV.Len(), -1);
  int node, edge;
  TClustNet::TNodeI NI;
  for (node = 0; node < NIdV.Len(); node++) {
    NewNet.AddNode(NIdV[node], GetNDat(NIdV[node])); // also copy the node data
  }
  for (node = 0; node < NIdV.Len(); node++) {
    NI = GetNI(NIdV[node]);
    const int SrcNId = NI.GetId();
    for (edge = 0; edge < NI.GetOutDeg(); edge++) {
      const int OutNId = NI.GetOutNId(edge);
      if (NewNet.IsNode(OutNId)) {
        NewNet.AddEdge(SrcNId, OutNId); }
    }
  }
  NewNet.Defrag();
  return NewNetPt;
}

void TClustNet::AddLink(const TQuote& SrcQt, const TQuote& DstQt) {
  const int Qt1Id = SrcQt.GetCId();
  const int Qt2Id = DstQt.GetCId();
  if (! IsNode(Qt1Id)) {
    AddNode(Qt1Id, SrcQt); }
  if (! IsNode(Qt2Id)) {
    AddNode(Qt2Id, DstQt); }
  if (! IsEdge(Qt2Id, Qt1Id)) {
    AddEdge(Qt1Id, Qt2Id);
  }
}

PClustNet TClustNet::GetSubGraph(const int& MinQtWords, const int& MaxQtWords, const int& MinFq) const {
  TIntV NIdV;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    const int Wrds = TStrUtil::CountWords(NI().GetStr().CStr());
    if (NI().GetFq() >= MinFq &&  Wrds >= MinQtWords && Wrds <= MaxQtWords) {
      NIdV.Add(NI.GetId()); }
  }
  return GetSubGraph(NIdV);
}

void TClustNet::RecalcEdges(const double& MinOverlapFrac) {
  printf("Recalculating edges...\n");
  TIntPrV DelEdgeV;
  TStrHash<TInt> StrH(Mega(1), true);
  int WIdV1Start, WIdV2Start, SkipTy;
  TIntV WIdV1, WIdV2;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    const TStr Q1 = NI().GetStr();
    TStrUtil::GetAddWIdV(StrH, Q1.CStr(), WIdV1);
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      const TStr Q2 = NI.GetOutNDat(e).GetStr();
      TStrUtil::GetAddWIdV(StrH, Q2.CStr(), WIdV2);
      const int Overlap = TQuoteBs::LongestCmnSubSq(WIdV1, WIdV2, WIdV1Start, WIdV2Start, SkipTy);
      const int ShortLen = TMath::Mn(WIdV1.Len(), WIdV2.Len());
      const int LongLen = TMath::Mx(WIdV1.Len(), WIdV2.Len());
      IAssert(Overlap<=ShortLen);
      //if (ShortLen <= 5 && Overlap == ShortLen) { continue; }
      //else if (ShortLen < 8 && Overlap > 5 && 2*ShortLen>LongLen) { continue; }
      //else if (ShortLen < 9 && Overlap > 6 && 2*ShortLen>LongLen) { continue; }
      //else if (ShortLen > 8 && (Overlap/double(ShortLen+3) > MinOverlapFrac) && 2*ShortLen>LongLen) { continue; }
      if (2*ShortLen>LongLen && Overlap/double(ShortLen) > MinOverlapFrac) { continue; }
      //if (ShortLen>10 && 1.5*ShortLen>LongLen && Overlap/double(ShortLen-3) > MinOverlapFrac) { continue; }
      DelEdgeV.Add(TIntPr(NI.GetId(), NI.GetOutNId(e)));
    }
  }
  printf("Deleting %d/%d (%.4f) edges\n", DelEdgeV.Len(), GetEdges(), DelEdgeV.Len()/double(GetEdges()));
  for (int i = 0; i < DelEdgeV.Len(); i++) {
    DelEdge(DelEdgeV[i].Val1, DelEdgeV[i].Val2);
  }
}

// create clusters based on the edges to keep
void TClustNet::MakeClusters(const TIntPrV& KeepEdgeV) {
  // make clusters
  PUNGraph G = TUNGraph::New();
  for (int e = 0; e < KeepEdgeV.Len(); e++) {
    if (! G->IsNode(KeepEdgeV[e].Val1)) {
      G->AddNode(KeepEdgeV[e].Val1); }
    if (! G->IsNode(KeepEdgeV[e].Val2)) {
      G->AddNode(KeepEdgeV[e].Val2); }
    G->AddEdge(KeepEdgeV[e].Val1, KeepEdgeV[e].Val2);
  }
  TCnComV CnComV;
  TSnap::GetWccs(G, CnComV);
  TIntH NIdCcIdH(GetNodes());
  for (int c = 0; c < CnComV.Len(); c++) {
    const TIntV& NIdV = CnComV[c].NIdV;
    for (int n = 0; n < NIdV.Len(); n++) {
      NIdCcIdH.AddDat(NIdV[n], c);
    }
  }
  TIntPrV DelEdgeV;
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    const int ccid1 = NIdCcIdH.IsKey(EI.GetSrcNId()) ? NIdCcIdH.GetDat(EI.GetSrcNId()).Val : -1;
    const int ccid2 = NIdCcIdH.IsKey(EI.GetDstNId()) ? NIdCcIdH.GetDat(EI.GetDstNId()).Val : -1;
    if (ccid1 != ccid2 && ccid1!=-1 && ccid2!=-1) {
      DelEdgeV.Add(TIntPr(EI.GetSrcNId(), EI.GetDstNId())); }
  }
  const int Edges = GetEdges();
  printf("Deleting %d out of %d  (%f) edges\n", DelEdgeV.Len(), Edges, DelEdgeV.Len()/double(Edges));
  for (int d = 0; d < DelEdgeV.Len(); d++) {
    DelEdge(DelEdgeV[d].Val1, DelEdgeV[d].Val2);
  }
}

void TClustNet::KeepOnlyTree(const TIntPrV& KeepEdgeV) {
  TIntPrSet EdgeSet(KeepEdgeV.Len());
  for (int i = 0; i < KeepEdgeV.Len(); i++) {
    EdgeSet.AddKey(TIntPr(TMath::Mn(KeepEdgeV[i].Val1, KeepEdgeV[i].Val2),
      TMath::Mx(KeepEdgeV[i].Val1, KeepEdgeV[i].Val2)));
  }
  TIntPrSet DelEdgeV;
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) {
    const int N1 = TMath::Mn(EI.GetSrcNId(), EI.GetDstNId());
    const int N2 = TMath::Mx(EI.GetSrcNId(), EI.GetDstNId());
    if (! EdgeSet.IsKey(TIntPr(N1, N2))) {
      DelEdgeV.AddKey(TIntPr(N1, N2)); }
  }
  printf("deleting %d edges\n", DelEdgeV.Len());
  for (int d = 0; d < DelEdgeV.Len(); d++) {
    DelEdge(DelEdgeV[d].Val1, DelEdgeV[d].Val2);
  }
}

// get connected components (clusters)
void TClustNet::GetClusters(TVec<TIntV>& QtNIdV) const {
  TCnComV CnComV;
  TSnap::GetWccs(GetThis(), CnComV);
  CnComV.Sort(false);
  QtNIdV.Clr(false);
  TIntSet SeenSet;
  for (int i = 0; i < CnComV.Len(); i++) {
    for (int n = 0; n < CnComV[i].NIdV.Len(); n++) {
      IAssert(! SeenSet.IsKey(CnComV[i].NIdV[n]));
      SeenSet.AddKey(CnComV[i].NIdV[n]);
    }
    QtNIdV.Add(CnComV[i].NIdV);
  }
}

void TClustNet::GetMergedClustQt(const TIntV& QtIdV, TQuote& NewQt) const {
  int CentrQtId=-1, MxFq=0;
  THash<TPair<TSecTm, TInt>, TInt> TmUrlCntH;
  for (int c = 0; c < QtIdV.Len(); c++) {
    const TQuote& Q = GetNDat(QtIdV[c]);
    IAssert(Q.GetId() == QtIdV[c]);
    for (int t = 0; t < Q.GetTimes(); t++) {
      TmUrlCntH.AddDat(TPair<TSecTm, TInt>(Q.GetTm(t), Q.GetUrlId(t))) += Q.GetCnt(t);
    }
    if (MxFq < Q.GetFq()) {
      MxFq = Q.GetFq();
      CentrQtId = Q.GetId();
    }
  }
  NewQt.QtCIdTy = TQuote::TQtIdTy(-1, qtCentr);
  NewQt.QtStr = GetNDat(CentrQtId).GetStr(); // get more frequent string
  //NewQt.QtStr = GetQt(CId).GetStr();     // get longest string
  NewQt.TmUrlCntV.Gen(TmUrlCntH.Len(), 0);
  for (int u = 0; u < TmUrlCntH.Len(); u++) {
    NewQt.TmUrlCntV.Add(TQuote::TTmUrlCnt(TmUrlCntH.GetKey(u).Val1, TmUrlCntH.GetKey(u).Val2(), TmUrlCntH[u]()));
  }
  NewQt.TmUrlCntV.Sort();
}

// create subgraphs and see the number of induced edges
int TClustNet::EvalPhraseClusters(const TIntPrV& KeepEdgeV, const bool& dump) const {
  PNGraph G = TNGraph::New();
  for (int e = 0; e < KeepEdgeV.Len(); e++) {
    if (! G->IsNode(KeepEdgeV[e].Val1)) {
      G->AddNode(KeepEdgeV[e].Val1); }
    if (! G->IsNode(KeepEdgeV[e].Val2)) {
      G->AddNode(KeepEdgeV[e].Val2); }
    G->AddEdge(KeepEdgeV[e].Val1, KeepEdgeV[e].Val2);
  }
  TCnComV CnComV;
  TSnap::GetWccs(G, CnComV);
  G = TSnap::ConvertGraph<PNGraph>(TPt<TClustNet>((TClustNet*) this));
  int TotEdges=0, EdgesInBigClust=0;
  int TotNodes=0, NodesInBigClust=0;
  int BigClust=0;
  for (int cc = 0; cc < CnComV.Len(); cc++) {
    if (CnComV[cc].NIdV.Len() < 3) { continue; }
    PNGraph CC = TSnap::GetSubGraph(G, CnComV[cc].NIdV);
    TotEdges += CC->GetEdges();
    TotNodes += CC->GetNodes()-1;
    if (CC->GetNodes() > 10) {
      EdgesInBigClust += CC->GetEdges();
      NodesInBigClust += CC->GetNodes()-1;
      BigClust++;
    }
  }
  if (dump) {
    printf("                                all\tbig(>10)\n");
    printf("  Number of clusters:           %d\t%d\n", CnComV.Len(), BigClust);
    printf("  Total edges inside clusters:  %d\t%d\n", TotEdges, TotEdges-TotNodes);
    printf("  Total edges deleted:          %d\t%d\n", GetEdges()-TotEdges, GetEdges()-TotEdges-TotNodes);
    printf("  Total edges in big clusters:  %d\t%d\n", EdgesInBigClust, EdgesInBigClust-NodesInBigClust);
  }
  return TotEdges-TotNodes;
}

// 1: keep link to most freqeunt variant
// 2: keep link to longest variant
// 3: keep link to shortest variant
// 4: keep random link
void TClustNet::ClustKeepSingleEdge(const int& MethodId) const {
  TIntPrV KeepEdgeV;
  ClustKeepSingleEdge(MethodId, KeepEdgeV);
}
// 1: keep edge to most freq variant
// 2: keep edge to longest variant
// 3: keep edge to shortest variant
// 4: keep edge to a random variant
void TClustNet::ClustKeepSingleEdge(const int& MethodId, TIntPrV& KeepEdgeV) const {
  PNGraph G = TSnap::ConvertGraph<PNGraph>(TPt<TClustNet>((TClustNet*) this));
  KeepEdgeV.Clr(false);
  // keep only a single edge out of each node
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    IAssert(NI.GetId() == NI().GetId());
    int EdgeToKeep = -1, BestVal=0;
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      // keep link to most frequent variant
      if (MethodId==1 && NI.GetOutNDat(e).GetFq() > BestVal) { EdgeToKeep=e; BestVal=NI.GetOutNDat(e).GetFq(); }
      // keep link to longest variant
      if (MethodId==2 && NI.GetOutNDat(e).GetStr().Len() > BestVal) { EdgeToKeep=e; BestVal=NI.GetOutNDat(e).GetStr().Len(); }
      // keep link to shortest variant
      if (MethodId==3 && NI.GetOutNDat(e).GetStr().Len() < BestVal ||BestVal==0) { EdgeToKeep=e; BestVal=NI.GetOutNDat(e).GetStr().Len(); }
    }
    // random link
    if (MethodId==4 && NI.GetOutDeg()>0) {
      EdgeToKeep = TInt::Rnd.GetUniDevInt(NI.GetOutDeg()); }
    if (EdgeToKeep!=-1) {
      KeepEdgeV.Add(TIntPr(NI.GetId(), NI.GetOutNId(EdgeToKeep))); }
  }
  if (MethodId==1) { printf("Keep edge to most frequent quote:\n"); }
  if (MethodId==2) { printf("Keep edge to longest quote:\n"); }
  if (MethodId==3) { printf("Keep edge to shortest quote:\n"); }
  if (MethodId==4) { printf("Keep random edge:\n"); }
  EvalPhraseClusters(KeepEdgeV);
}

void TClustNet::ClustGreedyTopDown() const {
  TIntPrV KeepEdgeV;
  ClustGreedyTopDown(KeepEdgeV);
}

void TClustNet::ClustGreedyTopDown(TIntPrV& KeepEdgeV) const {
  TIntH NIdOutDegH;
  TIntH NIdClustH;
  // find root nodes
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    NIdOutDegH.AddDat(NI.GetId(), NI.GetOutDeg());
    if (NI.GetOutDeg() == 0) {
      NIdClustH.AddDat(NI.GetId(), NI.GetId()); }
  }
  printf("%d root nodes\n", NIdClustH.Len());
  printf("%d nodes\n", NIdOutDegH.Len());
  NIdOutDegH.SortByDat(true);
  THash<TInt, TIntPr> ClustCntH;
  KeepEdgeV.Clr(false);
  while (NIdOutDegH.Len() > 0 && NIdOutDegH[0] == 0) {
    for (int i = 0; i < NIdOutDegH.Len() && NIdOutDegH[i] == 0; i++) {
      IAssert(IsNode(NIdOutDegH.GetKey(i)));
      const TNodeI NI = GetNI(NIdOutDegH.GetKey(i));
      // tell childeren that node knows its cluster id
      for (int e = 0; e < NI.GetInDeg(); e++) {
        NIdOutDegH.GetDat(NI.GetInNId(e)) -= 1; }
      // set node cluster id
      ClustCntH.Clr(false);
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        IAssert(NIdClustH.IsKey(NI.GetOutNId(e)));
        ClustCntH.AddDat(NIdClustH.GetDat(NI.GetOutNId(e))).Val1 += 1; // NI.GetOutNDat(e).GetFq();
        ClustCntH.AddDat(NIdClustH.GetDat(NI.GetOutNId(e))).Val2 = NI.GetOutNId(e);
      }
      ClustCntH.SortByDat(false);
      if (ClustCntH.Len() > 0) {
        // point to cluster where we have most edges into
        const int NId = NI.GetId();
        const int NId2 = ClustCntH[0].Val2;
        const int CId = NIdClustH.GetDat(NId2);
        KeepEdgeV.Add(TIntPr(NId, NId2)); // edge
        NIdClustH.AddDat(NId, CId); // cluster id
      }
      NIdOutDegH[i] = TInt::Mx;
    }
    NIdOutDegH.SortByDat(true);
  }
  printf("Greedy top down approach:\n");
  EvalPhraseClusters(KeepEdgeV);
}

void TClustNet::ClustGreedyRandom() const {
  printf("Greedy random:\n");
  THash<TInt, TIntV> OutNIdV;
  THash<TInt, TInt> EdgeH;
  TIntPrV KeepEdgeV;
  // created node out edge vector
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    if (NI.GetOutDeg() > 0) {
      EdgeH.AddDat(NI.GetId()) = NI.GetOutNId(TInt::Rnd.GetUniDevInt(NI.GetOutDeg()));
      if (NI.GetOutDeg() > 1) {
        for (int e = 0; e < NI.GetOutDeg(); e++) {
          OutNIdV.AddDat(NI.GetId()).Add(NI.GetOutNId(e)); }
      }
    }
  }
  // geedy top down to get the initial solution
  { TIntH NIdOutDegH;
  TIntH NIdClustH;
  for (TNodeI NI = BegNI(); NI < EndNI(); NI++) {
    NIdOutDegH.AddDat(NI.GetId()) = NI.GetOutDeg();
    if (NI.GetOutDeg() == 0) {
      NIdClustH.AddDat(NI.GetId(), NI.GetId()); }
  }
  NIdOutDegH.SortByDat(true);
  THash<TInt, TIntPr> ClustCntH;
  while (NIdOutDegH.Len() > 0 && NIdOutDegH[0] == 0) {
    for (int i = 0; i < NIdOutDegH.Len() && NIdOutDegH[i] == 0; i++) {
      const TNodeI NI = GetNI(NIdOutDegH.GetKey(i));
      // tell childern that node knows its cluster id
      for (int e = 0; e < NI.GetInDeg(); e++) {
        NIdOutDegH.GetDat(NI.GetInNId(e)) -= 1; }
      // set node cluster id
      ClustCntH.Clr(false);
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        ClustCntH.AddDat(NIdClustH.GetDat(NI.GetOutNId(e))).Val1 += 1;
        ClustCntH.AddDat(NIdClustH.GetDat(NI.GetOutNId(e))).Val2 = NI.GetOutNId(e);
      }
      ClustCntH.SortByDat(false);
      if (ClustCntH.Len() > 0) {
        // point to cluster where we have most edges into
        KeepEdgeV.Add(TIntPr(NI.GetId(), ClustCntH[0].Val2));
      }
      NIdOutDegH[i] = TInt::Mx;
    }
    NIdOutDegH.SortByDat(true);
  } }
  printf("%d\n", EdgeH.Len());
  for (int e = 0; e < KeepEdgeV.Len(); e++) {
    EdgeH.AddDat(KeepEdgeV[e].Val1) = KeepEdgeV[e].Val2;
  }
  printf("%d\n", EdgeH.Len());
  EdgeH.GetKeyDatPrV(KeepEdgeV);
  int CurScore = EvalPhraseClusters(KeepEdgeV);
  while (true) {
    //for (int i = 0; i < 10; i++) {
      const int RndNId = OutNIdV.GetKey(TInt::Rnd.GetUniDevInt(OutNIdV.Len()));
      const int RndEdge = OutNIdV.GetDat(RndNId)[TInt::Rnd.GetUniDevInt(OutNIdV.GetDat(RndNId).Len())];
      const int id = EdgeH.GetKeyId(RndNId);  IAssert(KeepEdgeV[id].Val1 == RndNId);
      if (KeepEdgeV[id].Val2 == RndEdge) { continue; } // same edge
      const int OldE = KeepEdgeV[id].Val2;
      KeepEdgeV[id].Val2 = RndEdge;
    //}
    const int NewScore = EvalPhraseClusters(KeepEdgeV, false);
    if (NewScore > CurScore /*|| TInt::Rnd.GetUniDev() < 0.1*/) {
      printf("%6d --> %6d\n", CurScore, NewScore);
      CurScore = NewScore;
    }
    else {  KeepEdgeV[id].Val2 = OldE; } // don't make the change
  }
}

TChA InsertLineBreaks(const TChA& ChA, const int& BreakAtPost) {
  TChA Tmp = ChA, Out;
  int Lines = 1;
  TVec<char*> WrdV;
  TStrUtil::SplitWords(Tmp, WrdV);
  for (int w = 0; w < WrdV.Len(); w++) {
    if (Out.Len() + (int)strlen(WrdV[w]) > Lines*BreakAtPost) {
      Lines++; Out+="\\n"; }
    Out += WrdV[w];
    Out += " ";
  }
  return Out;
}

void TClustNet::DrawNet(const TStr& OutFNm, const int& SaveTopN) const {
  TCnComV CnComV;
  TSnap::GetWccs(GetThis(), CnComV);
  CnComV.Sort(false);
  for (int Comp = 0; Comp<TMath::Mn(SaveTopN, CnComV.Len()); Comp++) {
    if (CnComV[Comp].Len() < 5) { continue; }
    TPt<TNet> SubNet = TSnap::GetSubGraph(TPt<TNet>((TClustNet*) this), CnComV[Comp].NIdV);
    printf("draw: %d nodes, %d edges\n", SubNet->GetNodes(), SubNet->GetEdges());
    FILE *F = fopen(TStr::Fmt("%s-c%02d.dot", OutFNm.CStr(), Comp).CStr(), "wt");
    fprintf(F, "digraph G { /*%d nodes, %d edges*/\n", SubNet->GetNodes(), SubNet->GetEdges());
    fprintf(F, "  graph [splines=true overlap=false rankdir=LR]\n");
    fprintf(F, "  node  [shape=box, fontsize=14]\n");
    for (TNet::TNodeI NI = SubNet->BegNI(); NI < SubNet->EndNI(); NI++) {
      fprintf(F, "  %d [label=\"%s (%d, %d)\"];\n", NI.GetId(), InsertLineBreaks(NI().GetStr(), 80).CStr(), NI().GetFq(), NI().GetUrls());
    }
    for (TNet::TEdgeI EI = SubNet->BegEI(); EI < SubNet->EndEI(); EI++) {
      fprintf(F, "  %d -> %d;\n", EI.GetSrcNId(), EI.GetDstNId());
    }
    fprintf(F, "}\n");
    fclose(F);
    TGraphViz::DoLayout(TStr::Fmt("%s-c%02d.dot", OutFNm.CStr(), Comp), TStr::Fmt("%s-c%02d.ps", OutFNm.CStr(), Comp), gvlDot);
    TSnap::SavePajek(SubNet, TStr::Fmt("%s-c%02d.net", OutFNm.CStr(), Comp));
  }
}

// Dump: Quote, Cluster size, Frequency
void TClustNet::DumpNodes(const TStr& OutFNm, const int& SaveTopN) const {
  TIntV NIdV;  GetNIdV(NIdV);
  TIntH NIdCompSzH;
  { TCnComV CnComV;
  TSnap::GetWccs(GetThis(), CnComV);
  for (int c = 0; c < CnComV.Len(); c++) {
    for (int n = 0; n < CnComV[c].NIdV.Len(); n++) {
      NIdCompSzH.AddDat(CnComV[c].NIdV[n], CnComV[c].Len());
    }
  } }
  TIntPrV FqNIdV;
  for (int n = 0; n < NIdV.Len(); n++) {
    FqNIdV.Add(TIntPr(GetNDat(NIdV[n]).GetFq(), NIdV[n]));
  }
  FqNIdV.Sort(false);
  FILE *F = fopen(TStr::Fmt("nodes-%s.txt", OutFNm.CStr()).CStr(), "wt");
  fprintf(F, "#Freq\tClustSz\tQuote\n");
  for (int i = 0; i < FqNIdV.Len(); i++) {
    const TQuote& Q = GetNDat(FqNIdV[i].Val2);
    fprintf(F, "%d\t%d\t%s\n", Q.GetFq(), NIdCompSzH.GetDat(FqNIdV[i].Val2).Val, Q.GetStr().CStr());
  }
}

void TClustNet::DumpClusters(const TStr& OutFNm, int SaveTopN) const {
  TCnComV CnComV;
  TSnap::GetWccs(GetThis(), CnComV); // wccs
  CnComV.Sort(false);
  FILE *F = fopen(TStr::Fmt("clust-%s.txt", OutFNm.CStr()).CStr(), "wt");
  if (SaveTopN==-1) { SaveTopN=TInt::Mx; }
  for (int Comp = 0; Comp<TMath::Mn(SaveTopN, CnComV.Len()); Comp++) {
    const TIntV& NIdV = CnComV[Comp].NIdV;
    TIntPrV FqNIdV;
    int SumFq=0;
    for (int n = 0; n < NIdV.Len(); n++) {
      FqNIdV.Add(TIntPr(GetNDat(NIdV[n]).GetFq(), NIdV[n]));
      SumFq += GetNDat(NIdV[n]).GetFq();
    }
    FqNIdV.Sort(false);
    fprintf(F, "%d quotes, total freq %d\n", FqNIdV.Len(), SumFq);
    for (int i = 0; i < FqNIdV.Len(); i++) {
      const TQuote& Q = GetNDat(FqNIdV[i].Val2);
      fprintf(F, "%d\t%s\n", Q.GetFq(), Q.GetStr().CStr());
    }
    fprintf(F, "\n");
  }
}

void TClustNet::DumpClustersByVol(const TStr& OutFNm, const int& MinClustSz, const int& MinVolume) const {
  TCnComV CnComV;
  TSnap::GetWccs(GetThis(), CnComV);
  CnComV.Sort(false);
  // sort clusters by FQ
  TIntPrV FqClustV;
  TIntH ClSzH, ClFqH;
  int FqMore1k=0;
  for (int Comp = 0; Comp < CnComV.Len(); Comp++) {
    const TIntV& NIdV = CnComV[Comp].NIdV;
    if (NIdV.Len() < MinClustSz) { continue; } // minimum number of variants
    int SumFq=0;
    for (int n = 0; n < NIdV.Len(); n++) {
      SumFq += GetNDat(NIdV[n]).GetFq(); }
    FqClustV.Add(TIntPr(SumFq, Comp));
    ClFqH.AddDat(SumFq) += 1;
    ClSzH.AddDat(NIdV.Len()) += 1;
    if (SumFq>500) { FqMore1k++; }
  }
  FqClustV.Sort(false);
  // save
  FILE *F = fopen(TStr::Fmt("clustFq-%s.txt", OutFNm.CStr()).CStr(), "wt");
  fprintf(F, "Cluster network:\n%d nodes\n%d edges\n%d clusters\n%d big clusters (>=%d)\n",
    GetNodes(), GetEdges(), CnComV.Len(), FqClustV.Len(), MinClustSz);
  for (int c = 0; c < FqClustV.Len(); c++) {
    const TIntV& NIdV = CnComV[FqClustV[c].Val2].NIdV;
    TIntPrV FqNIdV;
    int SumFq=0;
    for (int n = 0; n < NIdV.Len(); n++) {
      FqNIdV.Add(TIntPr(GetNDat(NIdV[n]).GetFq(), NIdV[n]));
      SumFq += GetNDat(NIdV[n]).GetFq();
    }
    if (SumFq < MinVolume) { continue; }
    FqNIdV.Sort(false);
    fprintf(F, "%d\t%d items\t%d totFq\n", c, FqNIdV.Len(), SumFq);
    for (int i = 0; i < FqNIdV.Len(); i++) {
      const TQuote& Q = GetNDat(FqNIdV[i].Val2);
      fprintf(F, "\t%d\t%s\n", Q.GetFq(), Q.GetStr().CStr());
    }
    fprintf(F, "\n");
  }
  TGnuPlot::PlotValCntH(ClFqH, "clVol."+OutFNm, TStr::Fmt("%s. %d nodes, %d edges, %d clusters, %d big clusters (>=%d), %d with vol>500",
    OutFNm.CStr(), GetNodes(), GetEdges(), CnComV.Len(), FqClustV.Len(), MinClustSz, FqMore1k), "Cluster volume", "Count", gpsLog);
  TGnuPlot::PlotValCntH(ClSzH, "clSz."+OutFNm, TStr::Fmt("%s. %d nodes, %d edges, %d clusters, %d big clusters (>=%d), %d with vol>500",
    OutFNm.CStr(), GetNodes(), GetEdges(), CnComV.Len(), FqClustV.Len(), MinClustSz, FqMore1k), "Cluster size", "Count", gpsLog);

}

// build phrase inverted index: word --> phrase id
void BuildPhraseInvertIdx(const PQuoteBs& QtBs, TStrHash<TInt>& WordIdH, THash<TInt, TIntV>& WIdQtIdVH) {
  printf("build quote word inverted index\n");
  TIntV WIdV;
  TIntSet WIdSet;
  for (int q = 0; q < QtBs->Len(); q++) {
    const TQuote& Q = QtBs->GetQtN(q);
    TStrUtil::GetAddWIdV(WordIdH, Q.GetStr().CStr(), WIdV);
    const int Doms = Q.GetDoms(*QtBs);
    if (! (Doms>1 && Doms*4 > Q.GetUrls() && Q.GetFq() >= 5)) { // skip quotes from too few domains
      continue;
    }
    WIdSet.Clr(false); // count each word only once
    for (int w = 0; w < WIdV.Len(); w++) {
      WIdSet.AddKey(WIdV[w]);
    }
    for (int w = 0; w < WIdSet.Len(); w++) {
      WIdQtIdVH.AddDat(WIdSet[w]).Add(q);
    }
  }
  for (int i = 0; i < WIdQtIdVH.Len(); i++) {
    WIdQtIdVH[i].Pack();
  }
  printf("done.\n");
}

bool IsLinkPhrases(const PQuoteBs& QtBs, const int& QtN1, const int& QtN2, TStrHash<TInt>& WordIdH, THash<TInt, TIntV>& QtToWordIdVH) {
  if (! QtToWordIdVH.IsKey(QtN1)) {
    TIntV WIdV;
    TStrUtil::GetWIdV(WordIdH, QtBs->GetQtN(QtN1).GetStr().CStr(), WIdV);
    WIdV.Pack();
    QtToWordIdVH.AddDat(QtN1, WIdV);
  }
  if (! QtToWordIdVH.IsKey(QtN2)) {
    TIntV WIdV;
    TStrUtil::GetWIdV(WordIdH, QtBs->GetQtN(QtN2).GetStr().CStr(), WIdV);
    WIdV.Pack();
    QtToWordIdVH.AddDat(QtN2, WIdV);
  }
  int idx1=0, idx2=0, SkipTy=0;
  const TIntV& WIdV1 = QtToWordIdVH.GetDat(QtN1);
  const TIntV& WIdV2 = QtToWordIdVH.GetDat(QtN2);
  const int ShortLen = TMath::Mn(WIdV1.Len(), WIdV2.Len());
  const int Overlap = TQuoteBs::LongestCmnSubSq(WIdV1, WIdV2, idx1, idx2, SkipTy);
  bool DoMerge = false;
  if (ShortLen <= 5 && Overlap == ShortLen /*&& SkipTy==0*/) { DoMerge=true; } // full overlap, no skip
  //else if (ShortLen == 5 && Overlap == 5 /*&& SkipTy==0*/) { DoMerge=true; } // full overlap, no skip
  else if ((ShortLen == 6 && Overlap >= 5 /*&& SkipTy==0) || (ShortLen == 6 && Overlap == 5*/)) { DoMerge=true; }
  else if (Overlap/double(ShortLen+3) > 0.5 || Overlap > 10) { DoMerge=true; }
  return DoMerge;
}

PClustNet TClustNet::GetFromQtBs(const PQuoteBs& QtBs, int MinQtFq, int MnWrdLen) {
  TStrHash<TInt> WordIdH;
  THash<TInt, TIntV> WIdQtIdVH; // inverted index
  BuildPhraseInvertIdx(QtBs, WordIdH, WIdQtIdVH);
  // create cluster network
  PClustNet Net = TClustNet::New();
  TVec<int> QtSharCnt(QtBs->Len()), QtWrdLen(QtBs->Len());
  THash<TInt, TIntV> QtToWordIdVH;   // hash qt string to word vector transformations
  TIntV WIdV;
  TIntH CandLenH, LinkLenH;
  THash<TInt, TIntH> QtLenCandH, QtLenLinkH;
  int AllCand=0, AllLinks=0;
  TExeTm ExeTm;
  for (int qt = 0; qt < QtBs->Len(); qt++) {
    QtWrdLen[qt] = TStrUtil::CountWords(QtBs->GetQtN(qt).GetStr().CStr());
  }
  const int QtBsLen = QtBs->Len();
  for (int qt = 0; qt < QtBs->Len(); qt++) {
    const TQuote& Q = QtBs->GetQtN(qt);
    const int Doms = Q.GetDoms(*QtBs);
  if (! (Doms>1 && Doms*4 > Q.GetUrls() && Q.GetFq() >= MinQtFq &&
    TStrUtil::CountWords(Q.GetStr())>=MnWrdLen)) {
      continue; }
    // for each quote count the number of words it shares with our quote
    memset(QtSharCnt.BegI(), 0, sizeof(int)*QtSharCnt.Len());
    TStrUtil::GetWIdV(WordIdH, Q.GetStr().CStr(), WIdV);
    if (! QtToWordIdVH.IsKey(qt)) {
      WIdV.Pack();
      QtToWordIdVH.AddDat(qt, WIdV);
    }
    for (int w = 0; w < WIdV.Len(); w++) {
      IAssert(WIdQtIdVH.IsKey(WIdV[w]));
      const TIntV& QtIdV = WIdQtIdVH.GetDat(WIdV[w]);
      for (int q = 0; q < QtIdV.Len(); q++) {
        QtSharCnt[QtIdV[q]] += 1;
      }
    }
    // candidate quotes
    const int W = WIdV.Len();
    int Candidates = 0, Links = 0, MinSharedWords = W-3;
    if (W <= 5) { MinSharedWords = W-1; }
    else if (W <= 10) { MinSharedWords = W-2; }
    for (int q = 0; q < QtSharCnt.Len(); q++) {
      // the matching quote is at most twice as long
      if (QtSharCnt[q] >= MinSharedWords && (QtWrdLen[q] >= W || QtWrdLen[q] > 20)) {
        const TQuote& Qt2 = QtBs->GetQtN(q);
        if (Doms>1 && Doms*4 > Q.GetUrls() && Q.GetFq() >= MinQtFq) { // quote has enough different domains
          if (IsLinkPhrases(QtBs, qt, q, WordIdH, QtToWordIdVH)) {    // edit distance computation
            Net->AddLink(Q, Qt2);
            Links++;
          }
        }
        Candidates++;
      }
    }
    AllCand += Candidates;
    AllLinks += Links;
    CandLenH.AddDat(Candidates) += 1;
    LinkLenH.AddDat(Links) += 1;
    if (W < 21) {
      QtLenCandH.AddDat(W).AddDat(Candidates) += 1;
      QtLenLinkH.AddDat(W).AddDat(Links) += 1;
    }
    if (qt % 1000 == 0 || qt==QtBsLen-1) {
      printf("\r%d  %s    allCands: %d    allLinks: %d    nodes:%d", qt, ExeTm.GetStr(), AllCand, AllLinks, Net->GetNodes());
      if (qt % 100000 == 0 || qt==QtBsLen-1) {
        TGnuPlot::PlotValCntH(CandLenH, "qtNet-NCand", "Number of candidate links for each quote (candidate has at most 2*words)",
          "ClustNet: number of candidate quotes to create link to", "Count", gpsLog);
        TGnuPlot::PlotValCntH(LinkLenH, "qtNet-NLink", "Number of links for each quote (candidate has at most 2*words)",
          "ClustNet: number of out-links of a quote", "Count", gpsLog);
        /*for (int i = 0; i < QtLenCandH.Len(); i++) {
          TGnuPlot::PlotValCntH(QtLenCandH[i], TStr::Fmt("qt08080902w3mfq5-NCand%02d", QtLenCandH.GetKey(i).Val),
            TStr::Fmt("Number of candidate links for each quote of word length %d (candidate has at most 2*words)", QtLenCandH.GetKey(i).Val),
            "ClustNet: number of candidate quotes to create link to", "Count", gpsLog);
        }
        for (int i = 0; i < QtLenLinkH.Len(); i++) {
          TGnuPlot::PlotValCntH(QtLenLinkH[i], TStr::Fmt("qt08080902w3mfq5-NLink%02d", QtLenLinkH.GetKey(i).Val),
            TStr::Fmt("Number of out-links for each quote of word length %d (candidate has at most 2*words)", QtLenLinkH.GetKey(i).Val),
            "ClustNet: number of out-links of a quote", "Count", gpsLog);
        }*/
      }
    }
    /*if (Candidates > 1) {
      printf("\n%s : %d\n", Q.GetStr().CStr(), Candidates);
      for (int q = 0; q < QtWrdCntH.Len(); q++) {
        if (q == qt) { continue; }
        if (QtWrdCntH[q] >= MinSharedWords && QtWrdLenV[q] >= W && QtWrdLenV[q] < 2*W) {
          printf("  %s\n", QtBs->GetQtN(q).GetStr().CStr()); }
      }
    }*/
  }
  printf("done.\n");
  TGnuPlot::PlotValCntH(CandLenH, "qtNet-NCand", "Number of candidate links for each quote (candidate has at most 2*words)",
    "ClustNet: number of candidate quotes to create link to", "Count", gpsLog);
  TGnuPlot::PlotValCntH(LinkLenH, "qtNet-NLink", "Number of links for each quote (candidate has at most 2*words)",
    "ClustNet: number of out-links of a quote", "Count", gpsLog);
  /*for (int i = 0; i < QtLenCandH.Len(); i++) {
    TGnuPlot::PlotValCntH(QtLenCandH[i], TStr::Fmt("qt08080902w3mfq5-NCand%02d", QtLenCandH.GetKey(i).Val),
      TStr::Fmt("Number of candidate links for each quote of word length %d (candidate has at most 2*words)", QtLenCandH.GetKey(i).Val),
      "ClustNet: number of candidate quotes to create link to", "Count", gpsLog);
  }
  for (int i = 0; i < QtLenLinkH.Len(); i++) {
    TGnuPlot::PlotValCntH(QtLenLinkH[i], TStr::Fmt("qt08080902w3mfq5-NLink%02d", QtLenLinkH.GetKey(i).Val),
      TStr::Fmt("Number of out-links for each quote of word length %d (candidate has at most 2*words)", QtLenLinkH.GetKey(i).Val),
      "ClustNet: number of out-links of a quote", "Count", gpsLog);
  } // */
  return Net;
}

/////////////////////////////////////////////////
// Quote Loader
void TQuoteLoader::Clr() {
  PostTitleStr.Clr();
  PostUrlStr.Clr();
  PubTm = TSecTm();
  BlogUrlStr.Clr();
  BlogTitleStr.Clr();
  ContentStr.Clr();
  QuoteV.Clr(false);
  LinkV.Clr(false);
}

bool TQuoteLoader::LoadItem(TXmlLx& XmlLx) {
  static const TSecTm BegOfTm(2008,8,30, 0, 0, 0);
  Clr();
  try {
    EAssert(XmlLx.TagNm == "post");
    const TChA T = TStrUtil::GetXmlTagVal(XmlLx, "pubDate");
    PubTm = TSecTm(atoi(T.GetSubStr(0,3).CStr()), atoi(T.GetSubStr(5,6).CStr()), atoi(T.GetSubStr(8,9).CStr()),
      atoi(T.GetSubStr(11,12).CStr()), atoi(T.GetSubStr(14,15).CStr()), atoi(T.GetSubStr(17,18).CStr()));
    EAssert(PubTm > BegOfTm);
    PostUrlStr = TStrUtil::GetXmlTagVal(XmlLx, "postUrl");
    PostTitleStr = TStrUtil::GetXmlTagVal(XmlLx, "postTitle");
    BlogUrlStr = TStrUtil::GetXmlTagVal(XmlLx, "blogUrl");
    BlogTitleStr = TStrUtil::GetXmlTagVal(XmlLx, "blogTitle");
    ContentStr = TStrUtil::GetXmlTagVal(XmlLx, "content");
    // load quotes
    while (XmlLx.GetSym()==xsySTag && XmlLx.TagNm=="q") {
      EAssert(XmlLx.GetSym() == xsyStr);
      QuoteV.Add(XmlLx.TxtChA);
      EAssert(XmlLx.GetSym() == xsyETag && XmlLx.TagNm=="q");
    }
  }
  catch (PExcept Except){
    ErrNotify(Except->GetStr());
    Fail;  return false;
  }
  return true;
}

void TQuoteLoader::Save(TSOut& SOut) const {
  PubTm.Save(SOut);
  PostUrlStr.Save(SOut);
  PostTitleStr.Save(SOut);
  BlogUrlStr.Save(SOut);
  BlogTitleStr.Save(SOut);
  ContentStr.Save(SOut);
  QuoteV.Save(SOut);
  LinkV.Save(SOut);
}

void TQuoteLoader::Load(TSIn& SIn) {
  PubTm.Load(SIn);
  PostUrlStr.Load(SIn);
  PostTitleStr.Load(SIn);
  BlogUrlStr.Load(SIn);
  BlogTitleStr.Load(SIn);
  ContentStr.Load(SIn);
  QuoteV.Load(SIn);
  LinkV.Load(SIn);
}

bool TQuoteLoader::Next() {
  if (SIn.Empty() || SIn->Eof()) {
    printf("  new file");
    if (! FFile.Next(CurFNm)) { return false; }
    printf(" %s\n", CurFNm.GetFMid().CStr());
    SIn = TZipIn::IsZipExt(CurFNm.GetFExt()) ? PSIn(new TZipIn(CurFNm)) : PSIn(new TFIn(CurFNm));
    StartProcFile(CurFNm);
  }
  Load(*SIn);
  if (++PostCnt % Kilo(10) == 0) { printf("\r  %dk [%s]  ", PostCnt/Kilo(1), ExeTm.GetStr()); }
  return true;
}

void TQuoteLoader::ProcessPosts(const bool& IsXml, int LoadN) {
  TExeTm ExeTm, TotalTm;
  StartProcess();
  if (LoadN < 0) { LoadN = TInt::Mx; }
  int FilePostCnt=0;
  for (int f = 1; FFile.Next(CurFNm); f++) {
    printf("*** FILE:  %s\n", CurFNm.GetFMid().CStr());
    SIn = TZipIn::IsZipExt(CurFNm.GetFExt()) ? PSIn(new TZipIn(CurFNm)) : PSIn(new TFIn(CurFNm));
    if (IsXml) {
      TXmlLx XmlLx(SIn, xspTruncate);
      StartProcFile(CurFNm);
      for (FilePostCnt=0; XmlLx.GetSym() != xsyEof; FilePostCnt++, PostCnt++) {
        if (! (XmlLx.Sym==xsySTag && XmlLx.TagNm=="post")) {
          while (XmlLx.GetSym()!=xsyEof && ! (XmlLx.Sym==xsySTag && XmlLx.TagNm=="post")) { }
          if (XmlLx.Sym == xsyEof) { break; }
        }
        const bool IsGoodPost = LoadItem(XmlLx);
        ProcessPost(IsGoodPost);
        if (PostCnt % Kilo(1) == 0) {
          printf("\r%dk [%s] ", PostCnt/Kilo(1), ExeTm.GetStr()); }
        if (PostCnt >= LoadN) { break; }
      }
    } else {
      FilePostCnt = 0;
      while (! SIn->Eof()) {
        Load(*SIn);  FilePostCnt++;  PostCnt++;
        ProcessPost(true);
        if (PostCnt % Kilo(10) == 0) {
          printf("\r%dk [%s] ", PostCnt/Kilo(1), ExeTm.GetStr()); }
        if (PostCnt >= LoadN) { break; }
      }
    }
    printf("\n================================================================\n");
    printf("  file:  %s\n", CurFNm.GetFMid().CStr());
    printf("  time:  %s   ", ExeTm.GetStr());
    printf("total: %s [%s]\n", TotalTm.GetStr(), TExeTm::GetCurTm());
    printf("  posts: %d   total: %d\n", FilePostCnt, PostCnt);
    EndProcFile(CurFNm);
    fflush(stdout);  ExeTm.Tick();
    if (PostCnt >= LoadN) { break; }
  }
  EndProcess(PostCnt);
}

/////////////////////////////////////////////////
// Memes Dataset Loader
bool TMemesDataLoader::GetNextFile() {
  TStr FNm;
  if (! FFile.Empty()) {
    if (! FFile->Next(FNm)) { return false; }
    printf("NEXT-FL:  %s :\t%s\n", FNm.GetFBase().CStr(), TExeTm::GetCurTm());
  } else {
    IAssert(! InFNmF.Empty());
    if (InFNmF->Eof()) { return false; }
  while (! InFNmF->Eof() && InFNmF->GetNextLn(FNm) && FNm.Empty()) { }
    printf("NEXT-LN:  %s :\t%s\n", FNm.GetFBase().CStr(), TExeTm::GetCurTm());
  }
  if (FNm.Empty()) { return false; }
  if (TZipIn::IsZipExt(FNm.GetFExt())) {
    SInPt = TZipIn::New(FNm); }
  else {
    SInPt = TFIn::New(FNm); }  LineCnt = 0;
  return true;
}

void TMemesDataLoader::Clr() {
  PostUrlStr.Clr();
  ContentStr.Clr();
  PubTm = TSecTm();
  MemeV.Clr(false);
  MemePosV.Clr(false);
  LinkV.Clr(false);
  LinkPosV.Clr(false);
}

// FORMAT:
//U \t Post URL
//D \t Post time
//T \t Post title (optional!)
//C \t Post content
//L \t Index \t URL      (URL starts at Content[Index])
//Q \t Index \t Length \t Quote (Quote starts at Content[Index])
bool TMemesDataLoader::LoadNext() {
  Clr();
  if (SInPt.Empty() || SInPt->Eof()) {
    if (! GetNextFile()) { return false; }
  }
  TSIn& SIn = *SInPt;
  CurLn.Clr();
  // keep reading until line starts with P\t
  while (SIn.GetNextLn(CurLn) && (CurLn.Empty() || (CurLn[0]!='U' || CurLn[1]!='\t'))) { 
    printf("L: %s\n", CurLn.CStr()); LineCnt++; }
  LineCnt++;
  if (CurLn.Empty()) { return LoadNext(); }
  IAssertR((! CurLn.Empty()) && CurLn[0]=='U' && CurLn[1]=='\t', 
    TStr::Fmt("ERROR1: %s [line %llu]: '%s'\n", SIn.GetSNm().CStr(), LineCnt, CurLn.CStr()).CStr());
  PostUrlStr = CurLn.CStr()+2;
  while (SIn.GetNextLn(CurLn) && (CurLn.Empty() || (CurLn[0]!='D' || CurLn[1]!='\t'))) { LineCnt++; }
  IAssertR((! CurLn.Empty()) && CurLn[0]=='D', 
    TStr::Fmt("ERROR2: %s [line %llu]: '%s'\n", SIn.GetSNm().CStr(), LineCnt, CurLn.CStr()).CStr());  LineCnt++;
  try {
    PubTm = TSecTm::GetDtTmFromStr(CurLn);
  } catch (PExcept Except){ PubTm = 1; ErrNotify(Except->GetStr());
    printf("ERROR3: %s [line %llu]: '%s'\n", SIn.GetSNm().CStr(), LineCnt, CurLn.CStr()); 
  }
  IAssertR(SIn.GetNextLn(CurLn) && (! CurLn.Empty()) && (CurLn[0]=='C' || CurLn[0]=='T'), 
    TStr::Fmt("ERROR4: %s [line %llu]: '%s'\n", SIn.GetSNm().CStr(), LineCnt, CurLn.CStr()).CStr());  LineCnt++;
  if (CurLn[0] == 'T') { // skip title
    IAssertR(SIn.GetNextLn(CurLn) && (! CurLn.Empty()) && CurLn[0]=='C', 
      TStr::Fmt("ERROR5: %s [line %llu]: '%s'\n", SIn.GetSNm().CStr(), LineCnt, CurLn.CStr()).CStr());  LineCnt++; }
  ContentStr = CurLn.CStr()+2;
  // links
  while (SIn.GetNextLn(CurLn)) {  LineCnt++;
    if (CurLn.Empty() || CurLn[0]!='L') { break; }
    int linkb=2;
    while (CurLn[linkb]!='\t') { linkb++; }
    CurLn[linkb]=0;
    LinkV.Add(CurLn.CStr()+linkb+1);
    LinkPosV.Add(atoi(CurLn.CStr()+2));
  }
  // quotes
  do {
    if (CurLn.Empty() || CurLn[0]!='Q') { break; }
    int qb1=2;      while (CurLn[qb1]!='\t') { qb1++; }
    int qb2=qb1+1;  while (CurLn[qb2]!='\t') { qb2++; }
    CurLn[qb1]=0;  CurLn[qb2]=0;
    MemeV.Add(CurLn.CStr()+qb2+1);
    MemePosV.Add(TIntPr(atoi(CurLn.CStr()+2), atoi(CurLn.CStr()+qb1+1)));
    LineCnt++;
  } while (SIn.GetNextLn(CurLn));
  return true;

  /*CurLn.Clr();
  while (SIn.GetNextLn(CurLn) && (CurLn.Empty() || CurLn[0]!='P')) { LineCnt++; }
  LineCnt++;
  if (Line.Empty()) {
    if (GetNextFile()) { return LoadNext(); }
    else { return false; }
  }
  IAssert((! Line.Empty()) && Line[0]=='P');
  PostUrlStr = Line.CStr()+2;
  IAssert(SIn.GetNextLn(Line) && (! Line.Empty()) && Line[0]=='T');  LineCnt++;
  PubTm = TSecTm::GetDtTmFromStr(Line);
  while (SIn.GetNextLn(Line)) {
    if (Line.Empty() || Line[0]!='Q') { break; }
    LineCnt++;
    MemeV.Add(Line.CStr()+2);
  }
  // links
  do {
    LineCnt++;
    if (Line.Empty() || Line[0]!='L') { break; }
    LinkV.Add(Line.CStr()+2);
  } while (SIn.GetNextLn(Line));
  return true; */
}

void TMemesDataLoader::SaveTxt(TSOut& SOut) const {
  /*SOut.PutStr("P\t");
  SOut.PutStrLn(PostUrlStr, true);
  SOut.PutStr("T\t");
  SOut.PutStrLn(PubTm.GetYmdTmStr(), true);
  for (int q = 0; q < MemeV.Len(); q++) {
    SOut.PutStr("Q\t");
    SOut.PutStrLn(MemeV[q], true);
  }
  for (int l = 0; l < LinkV.Len(); l++) {
    SOut.PutStr("L\t");
    SOut.PutStrLn(LinkV[l], true);
  }
  SOut.PutLn();
*/
}

void TMemesDataLoader::Dump(const bool& DumpAll) const {
  /*printf("%s\n  %s\tQ:%d\tL:%d\n", PostUrlStr.CStr(), PubTm.GetYmdTmStr().CStr(), MemeV.Len(), LinkV.Len());
  if (DumpAll) {
    for (int i = 0; i < MemeV.Len(); i++) { printf("%s\n", MemeV[i].CStr()); }
    for (int i = 0; i < LinkV.Len(); i++) { printf("%s\n", LinkV[i].CStr()); }
  }*/
}
