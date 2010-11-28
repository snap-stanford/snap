#ifndef YahooEx_h
#define YahooEx_h

/////////////////////////////////////////////////
// Includes
#include "base.h"
#include "yahoobs.h"
#include "yahoodm.h"

/////////////////////////////////////////////////
// Yahoo-Experiment
class TYExpr{
public:
  // file prefixes&&extensions
  static const TStr LogFPfx;
  static const TStr DatFExt;
  static const TStr TxtFExt;
  static const TStr TmpFExt;
  // Yahoo-specific filename prefixes
  static const TStr YWebBsFPfx;
  static const TStr YTsWebBsFPfx;
  static const TStr YBsFPfx;
  static const TStr YWordFPfx;
  static const TStr YCtgFPfx;
  static const TStr YMdFPfx;
  static const TStr YDsBsFPfx;
  static const TStr YFSelFPfx;
  static const TStr YInvIxFPfx;
public:
  static void GenYBs(
   const TStr& WebBsFPath, const TStr& YDmNm, const PNotify& Notify);
  static void GenYDsBs(
   const TStr& WebBsFPath, const TStr& YDmNm, const PNotify& Notify);


  static void PutMomHd(TOLx& Lx, const TStr& VarNm);
  static void PutMomVal(TOLx& Lx, const PMom& Mom);

  static void Go(
   const PSOut& SOut, const bool& PutHd, const bool& SaveTmp,
   const TStr& YDmNm, const TStr& YRootFPath, const TStr& InfoMsg,
   const PYBs& _YBs, const PYDsBs& _YDsBs,
   const PYFSelBs& _YFSelBs, const PYInvIx& _YInvIx,
   const int& SampleDocs,
   const int& TsRuns, const int& TsDocs,
   const int& MnTsWords, const double& MnTsWordPrb, const bool& ExclTs,
   const int& MxNGram, const int& MnWordFq, const TSwSetTy& SwSetTy,
   const TYDsBsNrType& YDsBsNrType, const double& YDsBsWordFqExp,
   const double& YDsBsMnDocWordPrb, const double& YDsBsSumAllWordPrb,
   const TYFSelType& YFSelType, const TFltV& YFSelsV,
   const bool& YFSelPosWords, const PAttrEstV& YFSelAttrEstV,
   const TYNegDsType& YNegDsType, const TYPriorType& YPriorType,
   const double& YInvIxEstExp, const TFltV& YInvIxSumEstPrbV,
   const TIntV& YInvIxMnDocFqV,
   const TFltV& PrRePrbTshV,
   const int& ShowTopHits, const PNotify& Notify);

  static void ExtrResMlj(const PNotify& Notify);
  static void ExtrResKdd(const PNotify& Notify);
  static void Extr(const TStr& FNm, const PSOut& SOut);

  static PMd GenMd(
   const TStr& YDmNm, const TStr& YRootFPath, const bool& DoSave,
   const int& MxNGram, const int& MnWordFq, const TSwSetTy& SwSetTy,
   const TYDsBsNrType& YDsBsNrType, const double& YDsBsWordFqExp,
   const double& YDsBsMnDocWordPrb, const double& YDsBsSumAllWordPrb,
   const TYFSelType& YFSelType, const double& YFSels,
   const bool& YFSelPosWords, const PAttrEst& YFSelAttrEst,
   const TYNegDsType& YNegDsType, const TYPriorType& YPriorType,
   const double& YInvIxEstExp, const double& YInvIxSumEstPrb,
   const PNotify& Notify);

  class TDocCf;
  typedef TVec<TDocCf> TDocCfV;
  static void UseMd(
   const TStr& YDmNm, const TStr& YRootFPath,
   const PSIn& SIn, const PSOut& SOut,
   const double& MnDocPrbTsh, const int& MxTopHits,
   const double& MnTsWordPrb, const int& MnDocFq,
   const PNotify& Notify);
};

class TYExpr::TDocCf{
public:
  TStr UrlStr;
  double DocPrb;
  TFltIntKdV WordPrbIdV;
public:
  TDocCf(): UrlStr(), DocPrb(), WordPrbIdV(){}
  TDocCf(const TStr& _UrlStr, const double& _DocPrb, const TFltIntKdV& _WordPrbIdV):
    UrlStr(_UrlStr), DocPrb(_DocPrb), WordPrbIdV(_WordPrbIdV){}
  TDocCf(const TDocCf& DocCf):
    UrlStr(DocCf.UrlStr), DocPrb(DocCf.DocPrb), WordPrbIdV(DocCf.WordPrbIdV){}
  TDocCf(TSIn&){Fail;}
  void Save(TSOut&){Fail;}

  TDocCf& operator=(const TDocCf& DocCf){
    if (this!=&DocCf){
      UrlStr=DocCf.UrlStr; DocPrb=DocCf.DocPrb; WordPrbIdV=DocCf.WordPrbIdV;}
    return *this;}
  bool operator==(const TDocCf& DocCf) const {return DocPrb==DocCf.DocPrb;}
  bool operator<(const TDocCf& DocCf) const {return DocPrb<DocCf.DocPrb;}
};

#endif
