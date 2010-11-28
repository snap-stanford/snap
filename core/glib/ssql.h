#ifndef sql_h
#define sql_h

/////////////////////////////////////////////////
// Includes
#include "base.h"

/////////////////////////////////////////////////
// Forward
class TSqlTypeDef;
typedef TPt<TSqlTypeDef> PSqlTypeDef;

class TSqlFldDef;
typedef TPt<TSqlFldDef> PSqlFldDef;

class TSqlTbDef;
typedef TPt<TSqlTbDef> PSqlTbDef;

class TSqlDef;
typedef TPt<TSqlDef> PSqlDef;

/////////////////////////////////////////////////
// Sql-Type-Definition
typedef enum {
  sttUndef, sttDate, sttNumber, sttVarChar2, sttComputed} TSqlTypeType;

class TSqlTypeDef{
private:
  TCRef CRef;
private:
  TInt Type;
  TInt Width;
  TInt Prec;
  TBool NotNull;
public:
  TSqlTypeDef(): Type(sttUndef), Width(-1), Prec(-1), NotNull(false){}
  ~TSqlTypeDef(){}
  TSqlTypeDef(TSIn&){Fail;}
  static PSqlTypeDef Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TSqlTypeDef& operator=(const TSqlTypeDef&){Fail; return *this;}

  TSqlTypeType GetType(){return TSqlTypeType(int(Type));}
  int GetWidth(){return Width;}
  int GetPrec(){return Prec;}

  static TFSet Expect;
  static PSqlTypeDef LoadTxt(TILx& Lx, const TFSet& Expect);

  friend PSqlTypeDef;
};

/////////////////////////////////////////////////
// Sql-Field-Definition
class TSqlFldDef{
private:
  TCRef CRef;
private:
  TStr Nm;
  PSqlTypeDef Type;
  TStr RefTbNm;
  TStr RefFldNm;
public:
  TSqlFldDef(): Nm(), Type(){}
  ~TSqlFldDef(){}
  TSqlFldDef(TSIn&){Fail;}
  static PSqlFldDef Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TSqlFldDef& operator=(const TSqlFldDef&){Fail; return *this;}

  TStr GetNm(){return Nm;}

  bool IsForeignKey(){return !RefTbNm.Empty();}
  bool IsComputed(){return Type->GetType()==sttComputed;}
  void PutRefFld(const TStr& _RefTbNm, const TStr& _RefFldNm){
    IAssert(!IsForeignKey());
    IAssert(!_RefTbNm.Empty()); IAssert(!_RefFldNm.Empty());
    RefTbNm=_RefTbNm; RefFldNm=_RefFldNm;}
  TStr GetRefTbNm(){IAssert(IsForeignKey()); return RefTbNm;}
  TStr GetRefFldNm(){IAssert(IsForeignKey()); return RefFldNm;}

  int GetWidth(){return Type->GetWidth();}

  static PSqlFldDef LoadTxt(TILx& Lx, const TFSet& Expect);

  friend PSqlFldDef;
};

/////////////////////////////////////////////////
// Sql-Table-Definition
class TSqlTbDef{
private:
  TCRef CRef;
private:
  TStr Nm;
  THash<TStr, PSqlFldDef> SqlFldNmToTypeH;
public:
  TSqlTbDef(): Nm(), SqlFldNmToTypeH(10){}
  ~TSqlTbDef(){}
  TSqlTbDef(TSIn&){Fail;}
  static PSqlTbDef Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TSqlTbDef& operator=(const TSqlTbDef&){Fail; return *this;}

  TStr GetNm(){return Nm;}
  int GetFlds(){return SqlFldNmToTypeH.Len();}
  PSqlFldDef GetFldDef(const TStr& FldNm){
    return SqlFldNmToTypeH.GetDat(FldNm);}
  PSqlFldDef GetFldDef(const int& FldN){
    return SqlFldNmToTypeH[FldN];}

  static PSqlTbDef LoadTxt(TILx& Lx, const TFSet& Expect);
  void LoadAlterTxt(TILx& Lx, const TFSet& Expect);

  friend PSqlTbDef;
};

/////////////////////////////////////////////////
// Sql-Definition
class TSqlDef{
private:
  TCRef CRef;
private:
  THash<TStr, PSqlTbDef> TbNmToDefH;
  TStrStrH TbNmToDNmH;
  TStrStrH DNmToTbNmH;
  TStrStrH TbFldNmToDNmH;
  TStrStrH TbKeyNmToDFldNmH;
public:
  TSqlDef():
    TbNmToDefH(100),
    TbNmToDNmH(100), DNmToTbNmH(100),
    TbFldNmToDNmH(100), TbKeyNmToDFldNmH(100){}
  ~TSqlDef(){}
  TSqlDef(TSIn&){Fail;}
  static PSqlDef Load(TSIn&){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TSqlDef& operator=(const TSqlDef&){Fail; return *this;}

  void AddTbDef(const PSqlTbDef& TbDef){
    TbNmToDefH.AddDat(TbDef->GetNm(), TbDef);}
  int GetTbs(){return TbNmToDefH.Len();}
  int GetTbDefN(const TStr& TbNm){
    return TbNmToDefH.GetKeyId(TbNm);}
  PSqlTbDef GetTbDef(const TStr& TbNm){
    return TbNmToDefH.GetDat(TbNm);}
  PSqlTbDef GetTbDef(const int& TbN){
    return TbNmToDefH[TbN];}

  void AddTbToDNm(const TStr& TbNm, const TStr& DNm){
    IAssert(!TbNmToDNmH.IsKey(TbNm)); TbNmToDNmH.AddDat(TbNm, DNm);
    IAssert(!DNmToTbNmH.IsKey(DNm)); DNmToTbNmH.AddDat(DNm, TbNm);}
  TStr GetDNmFromTbNm(const TStr& TbNm){
    if (TbNmToDNmH.IsKey(TbNm)){return TbNmToDNmH.GetDat(TbNm);}
    else {return TbNm;}}
  TStr GetTbNmFromDNm(const TStr& DNm){
    if (DNmToTbNmH.IsKey(DNm)){
      return DNmToTbNmH.GetDat(DNm);}
    else {return DNm;}}

  void AddTbFldNmToDNm(const TStr& TbNm, const TStr& FldNm, const TStr& DNm){
    TStr TbFldNm=TbNm+"."+FldNm;
    IAssert(!TbFldNmToDNmH.IsKey(TbFldNm));
    TbFldNmToDNmH.AddDat(TbFldNm, DNm);}
  TStr GetDNmFromTbFldNm(const TStr& TbNm, const TStr& FldNm){
    TStr TbFldNm=TbNm+"."+FldNm;
    if (TbFldNmToDNmH.IsKey(TbFldNm)){
      return TbFldNmToDNmH.GetDat(TbFldNm);}
    else {return FldNm;}}

  void AddTbKeyNmToDFldNm(
   const TStr& TbNm, const TStr& FldNm, const TStr& DFldNm){
    TStr TbKeyNm=TbNm+"."+FldNm;
    IAssert(!TbKeyNmToDFldNmH.IsKey(TbKeyNm));
    TbKeyNmToDFldNmH.AddDat(TbKeyNm, DFldNm);}
  TStr GetDFldNmFromTbKeyNm(const TStr& TbNm, const TStr& FldNm){
    TStr TbKeyNm=TbNm+"."+FldNm;
    if (TbKeyNmToDFldNmH.IsKey(TbKeyNm)){
      return TbKeyNmToDFldNmH.GetDat(TbKeyNm);}
    else {return FldNm;}}

  static PSqlDef LoadTxt(const TStr& FNm);

  friend PSqlDef;
};

#endif
