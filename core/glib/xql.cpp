/////////////////////////////////////////////////
// Includes
#include "xql.h"

/////////////////////////////////////////////////
// Xql-Expression-Value
bool TXqlExpVal::operator==(const TXqlExpVal& ExpVal) const {
  if (ValType!=ExpVal.ValType){return false;}
  switch (ValType){
    case xevtUndef: return true;
    case xevtFlt: return Flt==ExpVal.Flt;
    case xevtStr: return Str==ExpVal.Str;
    case xevtVec:{
      if (ValV.Len()!=ExpVal.ValV.Len()){return false;}
      for (int VecValN=0; VecValN<ExpVal.ValV.Len(); VecValN++){
        if (*ValV[VecValN]!=*ExpVal.ValV[VecValN]){return false;}}
      return true;}
    default: Fail; return false;
  }
}

bool TXqlExpVal::operator<(const TXqlExpVal& ExpVal) const {
  if (ValType!=ExpVal.ValType){
    return ValType<ExpVal.ValType;}
  switch (ValType){
    case xevtUndef: return false;
    case xevtFlt: return Flt<ExpVal.Flt;
    case xevtStr: return Str<ExpVal.Str;
    case xevtVec:{
      int VecValN=0;
      while ((VecValN<ValV.Len())&&(VecValN<ExpVal.ValV.Len())){
        if (*ValV[VecValN]<*ExpVal.ValV[VecValN]){return true;}
        else if (*ValV[VecValN]==*ExpVal.ValV[VecValN]){VecValN++;}
        else {return false;}
      }
      return ValV.Len()<ExpVal.ValV.Len();}
    default: Fail; return false;
  }
}

/////////////////////////////////////////////////
// Expression-Built-Ins
TXqlBi::TXqlBi():
  XqlBiNmToIdH(100), XqlBiIdToArgTypeH(100){

  AddBi("SELECT", xbi_SELECT);
  AddBi("ALL", xbi_ALL);
  AddBi("DISTINCT", xbi_DISTINCT);
  AddBi("TOP", xbi_TOP);
  AddBi("PERCENT", xbi_PERCENT);
  AddBi("FROM", xbi_FROM);
  AddBi("AS", xbi_AS);
  AddBi("WHERE", xbi_WHERE);
  AddBi("AND", xbi_AND);
  AddBi("OR", xbi_OR);
  AddBi("ORDER", xbi_ORDER);
  AddBi("BY", xbi_BY);
  AddBi("ASC", xbi_ASC);
  AddBi("DESC", xbi_DESC);
  AddBi("INTO", xbi_INTO);
}

void TXqlBi::AddBi(const TStr& XqlBiNm, const TXqlBiId& XqlBiId,
 const TXqlBiArgType& ExpBiArgType){
  XqlBiNmToIdH.AddDat(XqlBiNm.GetUc(), TInt(int(XqlBiId)));
  XqlBiIdToArgTypeH.AddDat(TInt(int(XqlBiId)), TInt(int(ExpBiArgType)));
}

bool TXqlBi::IsXqlBiId(const TStr& XqlBiNm, TXqlBiId& XqlBiId){
  int XqlBiIdP;
  if (XqlBiNmToIdH.IsKey(XqlBiNm.GetUc(), XqlBiIdP)){
    XqlBiId=TXqlBiId(int(XqlBiNmToIdH[XqlBiIdP])); return true;
  } else {
    XqlBiId=xbi_Undef; return false;
  }
}

TXqlBiArgType TXqlBi::GetXqlBiArgType(const TXqlBiId& XqlBiId){
  TInt XqlBiArgType=XqlBiIdToArgTypeH.GetDat(TInt(int(XqlBiId)));
  return TXqlBiArgType(int(XqlBiArgType));
}

void TXqlBi::AssertArgs(const int& RqArgs, const int& ActArgs){
  if (RqArgs!=ActArgs){
    TExcept::Throw("Invalid number of arguments.");
  }
}