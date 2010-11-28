#ifndef corrgr_h
#define corrgr_h

/////////////////////////////////////////////////
// Includes
#include "graph.h"

/////////////////////////////////////////////////
// Correlation-Groups
ClassTP(TCorrGroups, PCorrGroups)//{
public:
  PNotify Notify;
  TStrV VarNmV;
  TStrV RecNmV;
  TVec<TFltV> VarValVV;
  TFlt CorrCfTsh;
  TVec<TIntV> VarNVV;
public:
  TCorrGroups(const TStrV& _VarNmV, const TStrV& _RecNmV,
   const TVec<TFltV>& _VarValVV, const PNotify& _Notify=NULL):
    VarNmV(_VarNmV), RecNmV(_RecNmV), VarValVV(_VarValVV), Notify(_Notify){
    IAssert(VarNmV.Empty()||(VarNmV.Len()==VarValVV.Len()));}
  static PCorrGroups New(const TStrV& VarNmV, const TStrV& RecNmV,
   const TVec<TFltV>& VarValVV, const PNotify& Notify=NULL){
    return PCorrGroups(new TCorrGroups(VarNmV, RecNmV, VarValVV, Notify));}
  TCorrGroups(TSIn&){Fail;}
  static PCorrGroups Load(TSIn& SIn){return new TCorrGroups(SIn);}
  void Save(TSOut&){Fail;}

  TCorrGroups& operator=(const TCorrGroups&){Fail; return *this;}

  // variables
  int GetVars() const {return VarValVV.Len();}
  TStr GetVarNm(const int& VarN) const {
    if (VarNmV.Empty()){return TInt::GetStr(VarN);} else {return VarNmV[VarN];}}

  // records
  int GetRecs() const {return VarValVV[0].Len();}
  TStr GetRecNm(const int& RecN) const {
    if (RecNmV.Empty()){return TInt::GetStr(RecN);} else {return RecNmV[RecN];}}

  // similarities
  void GetCorrSimVV(TFltVV& SimVV) const;
  void GetEuclSimVV(TFltVV& SimVV) const;
  void GetSimVVFromData(TFltVV& SimVV) const;

  // groups
  void SetGroups(const double& _CorrCfTsh);
  double GetCorrCfTsh() const {return CorrCfTsh;}
  int GetGroups() const {return VarNVV.Len();}
  void GetGroup(const int& GroupN, TIntV& VarNV) const {VarNV=VarNVV[GroupN];}
  void GetGroupCentroid(const TIntV& VarNV, TMomV& VarValMomV) const;

  // hierarchical clustering
  PGraph GetHClustGraph(const TFltVV& SimVV) const;
  PGraph GetHClustGraph() const;
  void SetHClustGroups(const TFltVV& SimVV, const int& Groups);

  // k-means clustering
  void SetKMeansGroups(const int& Groups, const int& Trials);

  // loading file
  static PCorrGroups LoadTxtVarCVal(const TStr& FNm);

  // saving to file
  void SaveTxt(const PSOut& SOut,
   const bool& CentrP=false, const bool& SqrtValP=false) const;
  void SaveTxt(const TStr& FNm,
   const bool& CentrP=false, const bool& SqrtValP=false) const {
    PSOut SOut=TFOut::New(FNm); SaveTxt(SOut, CentrP, SqrtValP);}
};

#endif
