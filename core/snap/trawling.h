#ifndef snap_trawiling_h
#define snap_trawiling_h

#include "Snap.h"

class TIntVHashFunc {
public:
 static inline int GetPrimHashCd(const TIntV& Key) { 
  //return Key.GetPrimHashCd(); 
  const int Len = Key.Len();
  int HashVal = 0x345678;
  for (int i = 0; i < Key.Len(); i++) {
    HashVal = 1000003*HashVal ^ Key[i];
  }
  HashVal = HashVal ^ Len;
  return HashVal;
 }
 static inline int GetSecHashCd(const TIntV& Key) { 
   return Key.GetSecHashCd(); 
 }
};


/////////////////////////////////////////////////
// Trawling the web for emerging communities 
// (WWW paper by Tomkins, Kumart, Raghavan, etc.)
class TTrawling {
private:
  int MinSup;
  THash<TInt, TIntV> NIdSetH; // node on the left and the neighbors on the right (contains only nodes with in-deg > MinSup)
  THash<TInt, TIntV> SetNIdH; // set to node ids
  THash<TIntV, TInt, TIntVHashFunc> CandItemH, CurItemH;
  TVec<TIntV> MxFqItemSetV;
public:
  TTrawling(const PNGraph& Graph, const int& MinSupport);
  TTrawling(const PUNGraph& UnGraph, const TIntV& LeftNIdV, const int& MinSupport);

  void GenCandidates();
  int GetSupport(const TIntV& ItemSet);
  void CountSupport();
  void ThresholdSupp();
  void GenCandAndCntSupp(const int& FqItemsetLen);
  
  void InitItemSets();
  int GetNextFqItemSets(const int& FqItemsetLen=-1);

  int GetFqItems() const { return MxFqItemSetV.Len(); }
  const TIntV& GetItem(const int& ItemN) const { return MxFqItemSetV[ItemN]; }
  const TVec<TIntV>& GetItemV() const { return MxFqItemSetV; }
  TVec<TIntV>& GetItemV() { return MxFqItemSetV; }

  TIntPrV PlotMinFqVsMaxSet(const TStr& OutFNm);
  static void JoinItems(const TIntV& Item1, const TIntV& Item2, TIntV& JoinItem);
  static bool IsIn(const TIntV& BigV, const TIntV& SmallV);
};

#endif
