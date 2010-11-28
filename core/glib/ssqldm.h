#ifndef sqldm_h
#define sqldm_h

/////////////////////////////////////////////////
// Includes
#include "base.h"

/////////////////////////////////////////////////
// SqlDm-Char-Definition
typedef enum {
 dmctUndef, dmctSpace, dmctLetter, dmctDigit, dmctEof} TSqlDmChTy;

class TSqlDmChDef{
private:
  TIntV ChTyV;
  TChV UcChV;
  void SetUcCh(const TStr& Str);
  void SetChTy(const TSqlDmChTy& ChTy, const TStr& Str);
public:
  TSqlDmChDef();
  TSqlDmChDef(TSIn&){Fail;}
  void Save(TSOut&){Fail;}

  TSqlDmChDef& operator=(const TSqlDmChDef& ChDef){Fail; return *this;}

  int GetChTy(const uchar& Ch) const {return ChTyV[Ch];}
  bool IsEof(const uchar& Ch) const {return ChTyV[Ch]==dmctEof;}
  bool IsSpace(const uchar& Ch) const {return ChTyV[Ch]==dmctSpace;}
  bool IsAlpha(const uchar& Ch) const {return ChTyV[Ch]==dmctLetter;}
  bool IsNum(const uchar& Ch) const {return ChTyV[Ch]==dmctDigit;}
  bool IsAlNum(const uchar& Ch) const {
    return (ChTyV[Ch]==dmctLetter)||(ChTyV[Ch]==dmctDigit);}
  char GetUc(const uchar& Ch) const {return UcChV[Ch];}
};

/////////////////////////////////////////////////
// SqlDm-Lexical-Symbols
typedef enum {
  dsyUndef, dsyEof,
  dsyName, dsyStr, dsyInt, dsyFlt,
  dsyComma, dsyPeriod, dsySemicolon,
  dsyLss, dsyLEq, dsyEq, dsyNEq, dsyGtr, dsyGEq,
  dsyPlus, dsyMinus, dsyAsterisk, dsySlash,
  dsyLParen, dsyRParen, dsyLBrace, dsyRBrace,
  dsyRw_Create, dsyRw_Insert, dsyRw_Select, dsyRw_Delete,
  dsyRw_Drop, dsyRw_Rename, dsyRw_Mining, dsyRw_Model,
  dsyRw_Using, dsyRw_Into, dsyRw_ColumnValues, dsyRw_Content,
  dsyRw_From, dsyRw_Predict, dsyRw_Prediction, dsyRw_Join,
  dsyRw_On, dsyRw_Natural, dsyRw_Where, dsyRw_Flattened,
  dsyRw_Distinct, dsyRw_Union, dsyRw_Pmml, dsyRw_Or, dsyRw_And,
  dsyRw_Not, dsyRw_Long, dsyRw_Boolean, dsyRw_Text, dsyRw_Double,
  dsyRw_Date, dsyRw_Table, dsyRw_Normal, dsyRw_Uniform, dsyRw_Binary,
  dsyRw_Null, dsyRw_Discrete, dsyRw_Continuous, dsyRw_Ordered,
  dsyRw_SeqTime, dsyRw_Cyclical, dsyRw_Discretized, dsyRw_Automatic,
  dsyRw_EqualAreas, dsyRw_Thresholds, dsyRw_Clusters, dsyRw_Key,
  dsyRw_Probability, dsyRw_Variance, dsyRw_StDev, dsyRw_ProbabilityVariance,
  dsyRw_ProbabilityStDev, dsyRw_Support, dsyRw_Skip, dsyRw_PredictStDev,
  dsyRw_PredictVariance, dsyRw_PredictSupport, dsyRw_PredictProbability,
  dsyRw_PredictAdjustedProbability, dsyRw_PredictProbabilityStDev,
  dsyRw_PredictProbabilityVariance, dsyRw_PredictHistogram,
  dsyRw_HSupport, dsyRw_HVariance, dsyRw_HStDev, dsyRw_HProbability,
  dsyRw_HAdjustedProbability, dsyRw_HProbabilityVariance,
  dsyRw_HProbabilityStDev, dsyRw_HDistance, dsyRw_PredictOnly,
  dsyRw_Related, dsyRw_To, dsyRw_Of,
  dsyRw_OpenQuery, dsyRw_Cluster, dsyRw_ClusterDistance,
  dsyRw_ClusterProbability, dsyRw_As, dsyRw_Shape, dsyRw_Append,
  dsyRw_Relate, dsyRw_ExcludeNull, dsyRw_IncludeNull, dsyRw_Exclusive,
  dsyRw_Inclusive, dsyRw_InputOnly, dsyRw_IncludeStatistics,
  dsyRw_TopCount, dsyRw_TopSum, dsyRw_TopPercent, dsyRw_BottomCount,
  dsyRw_BottomSum, dsyRw_BottomPercent, dsyRw_RangeMin,
  dsyRw_RangeMid, dsyRw_RangeMax, dsyMx} TSqlDmLxSym;

class TSqlDmLxSymStr{
  static TStrIntH RwStrToSymH;
  static void AddRw(const TStr& RwStr, const TSqlDmLxSym& Sym){
    RwStrToSymH.AddDat(RwStr.GetUc(), Sym);}
  static void InitRwStrToSymH();
public:
  static TSqlDmLxSym GetRwSym(const TStr& RwStr);
  static TStr GetRwStr(const TSqlDmLxSym& Sym);
  static TStr GetSymStr(const TSqlDmLxSym& Sym);
};

/////////////////////////////////////////////////
// SqlDm-Lexical-Input-Symbol-State
class TSqlDmLx;

class TSqlDmLxSymSt{
private:
  TSqlDmLxSym Sym;
  TStr Str, UcStr;
  int Int; double Flt;
  int SymLnN, SymLnChN, SymChN;
public:
  TSqlDmLxSymSt();
  TSqlDmLxSymSt(const TSqlDmLxSymSt& SymSt);
  TSqlDmLxSymSt(TSqlDmLx& Lx);
  TSqlDmLxSymSt(TSIn&){Fail;}
  void Save(TSOut&){Fail;}

  void Restore(TSqlDmLx& Lx);
};

/////////////////////////////////////////////////
// SqlDm-Lexical
class TSqlDmLx{
private: // character level functions
  static TSqlDmChDef ChDef;
  PSIn SIn;
  TChA ChStack;
  uchar PrevCh, Ch;
  int LnN, LnChN, ChN;
  TSStack<TSqlDmLxSymSt> PrevSymStStack;
  uchar GetCh();
  void PutCh(const uchar& _Ch){ChStack.Push(Ch); Ch=_Ch;}
  void PutStr(const TStr& Str){
    for (int ChN=Str.Len()-1; ChN>=0; ChN--){PutCh(Str[ChN]);}}
public: // symbol state
  TSqlDmLxSym Sym;
  TChA Str, UcStr;
  int Int; double Flt;
  int SymLnN, SymLnChN, SymChN;
public:
  TSqlDmLx(const PSIn& _SIn);

  TSqlDmLx& operator=(const TSqlDmLx&){Fail; return *this;}

  TStr GetFPosStr() const;
  TSqlDmLxSym GetSym(const TFSet& Expect);
  TSqlDmLxSym GetSym(){return GetSym(TFSet::EmptyFSet);}
  TStr GetName(){GetSym(TFSet(dsyName)); return Str;}
  double GetFlt(){GetSym(TFSet(dsyFlt)); return Flt;}
};

/////////////////////////////////////////////////
// SqlDm-Delete-Statement
ClassTPV(TSqlDmStat_Delete, PSqlDmStat_Delete, TSqlDmStatV_Delete)//{
private:
  TStr MdNm;
  TBool ContentP;
public:
  TSqlDmStat_Delete(): MdNm(), ContentP(){}
  static PSqlDmStat_Delete New(){
    return PSqlDmStat_Delete(new TSqlDmStat_Delete());}
  TSqlDmStat_Delete(SIn): MdNm(SIn), ContentP(SIn){}
  void Save(TSOut& SOut){MdNm.Save(SOut); ContentP.Save(SOut);}

  TSqlDmStat_Delete& operator=(const TSqlDmStat_Delete&){Fail; return *this;}

  static PSqlDmStat_Delete LoadTxt(TSqlDmLx& Lx, const TFSet& Expect);
};

/////////////////////////////////////////////////
// SqlDm-Drop-Statement
ClassTPV(TSqlDmStat_Drop, PSqlDmStat_Drop, TSqlDmStatV_Drop)//{
private:
  TStr MdNm;
public:
  TSqlDmStat_Drop(): MdNm(){}
  static PSqlDmStat_Drop New(){
    return PSqlDmStat_Drop(new TSqlDmStat_Drop());}
  TSqlDmStat_Drop(SIn): MdNm(SIn){}
  void Save(TSOut& SOut){MdNm.Save(SOut);}

  TSqlDmStat_Drop& operator=(const TSqlDmStat_Drop&){Fail; return *this;}

  static PSqlDmStat_Drop LoadTxt(TSqlDmLx& Lx, const TFSet& Expect);
};

/////////////////////////////////////////////////
// SqlDm-Rename-Statement
ClassTPV(TSqlDmStat_Rename, PSqlDmStat_Rename, TSqlDmStatV_Rename)//{
private:
  TStr SrcMdNm;
  TStr DstMdNm;
public:
  TSqlDmStat_Rename(): SrcMdNm(), DstMdNm(){}
  static PSqlDmStat_Rename New(){
    return PSqlDmStat_Rename(new TSqlDmStat_Rename());}
  TSqlDmStat_Rename(SIn): SrcMdNm(SIn), DstMdNm(SIn){}
  void Save(TSOut& SOut){SrcMdNm.Save(SOut); DstMdNm.Save(SOut);}

  TSqlDmStat_Rename& operator=(const TSqlDmStat_Rename&){Fail; return *this;}

  static PSqlDmStat_Rename LoadTxt(TSqlDmLx& Lx, const TFSet& Expect);
};

/////////////////////////////////////////////////
// SqlDm-Statement
typedef enum {
 dstUndef, dstCreateDm, dstCreatePmml, dstSelectInto,
 dstDelete, dstDrop, dstRename} TSqlDmStatType;

ClassTPV(TSqlDmStat, PSqlDmStat, TSqlDmStatV)//{
private:
  TInt Type; // TSqlDmStatType
  //PSqlDmStat_CreateDm CreateDm;
  //PSqlDmStat_CreatePmml CreatePmml;
  //PSqlDmStat_SelectInto SelectInto;
  PSqlDmStat_Delete Delete;
  PSqlDmStat_Drop Drop;
  PSqlDmStat_Rename Rename;
public:
  TSqlDmStat(): Type(){}
  static PSqlDmStat New(){return PSqlDmStat(new TSqlDmStat());}
  TSqlDmStat(SIn): Type(SIn){}
  void Save(TSOut& SOut){Type.Save(SOut);}

  TSqlDmStat& operator=(const TSqlDmStat&){Fail; return *this;}

  TSqlDmStatType GetType() const {return TSqlDmStatType(int(Type));}
  //PSqlDmStat_CreateDm GetCreateDm() const {IAssert(Type==dstCreateDm); return;}
  //PSqlDmStat_CreatePmml GetCreatePmml() const {IAssert(Type==dstCreatePmml); return CreatePmml;}
  //PSqlDmStat_SelectInto GetSelectInto() const {IAssert(Type==dstSelectInto); return SelectInto;}
  PSqlDmStat_Delete GetDelete() const {IAssert(Type==dstDelete); return Delete;}
  PSqlDmStat_Drop GetDrop() const {IAssert(Type==dstDrop); return Drop;}
  PSqlDmStat_Rename GetRename() const {IAssert(Type==dstRename); return Rename;}

  static const TFSet StatExpect;
  static PSqlDmStat LoadTxt(TSqlDmLx& Lx, const TFSet& Expect);
};

/////////////////////////////////////////////////
// SqlDm-Program
ClassTP(TSqlDmProg, PSqlDmProg)//{
private:
  TSqlDmStatV StatV;
public:
  TSqlDmProg(): StatV(){}
  static PSqlDmProg New(){return PSqlDmProg(new TSqlDmProg());}
  TSqlDmProg(SIn): StatV(SIn){}
  void Save(TSOut& SOut){StatV.Save(SOut);}

  TSqlDmProg& operator=(const TSqlDmProg&){Fail; return *this;}

  int GetStats() const {return StatV.Len();}
  PSqlDmStat GetStats(const int& StatN) const {return StatV[StatN];}

  static PSqlDmProg LoadTxt(const TStr& FNm){
    PSIn SIn=TFIn::New(FNm); return LoadTxt(SIn);}
  static PSqlDmProg LoadTxt(PSIn& SIn);
};

#endif
