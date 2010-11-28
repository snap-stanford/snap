/////////////////////////////////////////////////
// Includes
#include "sql.h"

/////////////////////////////////////////////////
// Sql-Type-Definition
TFSet TSqlTypeDef::Expect=TFSet()|syIdStr;

PSqlTypeDef TSqlTypeDef::LoadTxt(TILx& Lx, const TFSet& Expect){
  TLxSym syNot=Lx.GetRw("NOT");
  TLxSym syNull=Lx.GetRw("NULL");
  PSqlTypeDef SqlTypeDef=PSqlTypeDef(new TSqlTypeDef());
  IAssert(Lx.Sym==syIdStr);
  if (Lx.Str=="DATE"){
    SqlTypeDef->Type=sttDate;
  } else
  if (Lx.Str=="NUMBER"){
    SqlTypeDef->Type=sttNumber;
    Lx.GetSym(syLParen);
    SqlTypeDef->Width=Lx.GetInt();
    if (Lx.GetSym(syComma, syRParen)==syComma){
      SqlTypeDef->Prec=Lx.GetInt();
      Lx.GetSym(syRParen);
    }
  } else
  if (Lx.Str=="VARCHAR2"){
    SqlTypeDef->Type=sttVarChar2;
    Lx.GetSym(syLParen);
    SqlTypeDef->Width=Lx.GetInt();
    Lx.GetSym(syRParen);
  } else
  if (Lx.Str=="COMPUTED"){
    SqlTypeDef->Type=sttComputed;
    while (Lx.GetSym()!=syRParen){}
  } else {
    Fail;
  }
  if (Lx.GetSym(TFSet(Expect)|syNot)==syNot){
    Lx.GetSym(syNull);
    SqlTypeDef->NotNull=true;
    Lx.GetSym(Expect);
  }

  return SqlTypeDef;
}

/////////////////////////////////////////////////
// Sql-Field-Definition
PSqlFldDef TSqlFldDef::LoadTxt(TILx& Lx, const TFSet& Expect){
  IAssert(Lx.Sym==syIdStr);
  PSqlFldDef FldDef=PSqlFldDef(new TSqlFldDef());
  FldDef->Nm=Lx.Str;
  Lx.GetSym(TSqlTypeDef::Expect);
  FldDef->Type=TSqlTypeDef::LoadTxt(Lx, Expect);
  return FldDef;
}

/////////////////////////////////////////////////
// Sql-Table-Definition
PSqlTbDef TSqlTbDef::LoadTxt(TILx& Lx, const TFSet& Expect){
  TLxSym syTable=Lx.GetRw("TABLE");
  IAssert(Lx.Sym==syTable);
  PSqlTbDef TbDef=PSqlTbDef(new TSqlTbDef());
  TbDef->Nm=Lx.GetIdStr();
  Lx.GetSym(syLParen);
  do {
    Lx.GetIdStr();
    PSqlFldDef FldDef=TSqlFldDef::LoadTxt(Lx, TFSet()|syComma|syRParen);
    TbDef->SqlFldNmToTypeH.AddDat(FldDef->GetNm(), FldDef);
  } while (Lx.Sym==syComma);
  Lx.GetSym(Expect);
  return TbDef;
}

void TSqlTbDef::LoadAlterTxt(TILx& Lx, const TFSet& Expect){
  TLxSym syAdd=Lx.GetRw("ADD");
  TLxSym syConstraint=Lx.GetRw("CONSTRAINT");
  TLxSym syForeigen=Lx.GetRw("FOREIGN");
  TLxSym syKey=Lx.GetRw("KEY");
  TLxSym syPrimary=Lx.GetRw("PRIMARY");
  TLxSym syReferences=Lx.GetRw("REFERENCES");
  IAssert((Lx.Sym==syIdStr)&&(Lx.Str==Nm));

  Lx.GetSym(TFSet(Expect)|syAdd);
  while (Lx.Sym==syAdd){
    Lx.GetSym(syConstraint);
    Lx.GetIdStr();
    Lx.GetSym();
    if (Lx.Sym==syPrimary){
      Lx.GetSym(syKey);
      Lx.GetSym(syLParen);
      do {
        Lx.GetIdStr();
      } while (Lx.GetSym(syComma, syRParen)==syComma);
      Lx.GetSym(TFSet(Expect)|syAdd);
    } else
    if (Lx.Sym==syForeigen){
      Lx.GetSym(syKey);
      Lx.GetSym(syLParen); TStr FldNm=Lx.GetIdStr(); Lx.GetSym(syRParen);
      Lx.GetSym(syReferences); TStr RefTbNm=Lx.GetIdStr();
      Lx.GetSym(syLParen); TStr RefFldNm=Lx.GetIdStr(); Lx.GetSym(syRParen);
      Lx.GetSym(TFSet(Expect)|syAdd);
      PSqlFldDef FldDef=GetFldDef(FldNm);
      FldDef->PutRefFld(RefTbNm, RefFldNm);
    } else {
      Fail;
    }
  }
}

/////////////////////////////////////////////////
// Sql-Definition
PSqlDef TSqlDef::LoadTxt(const TStr& FNm){
  PSqlDef SqlDef=PSqlDef(new TSqlDef());
  TILx Lx(PSIn(new TFIn(FNm)), TFSet()|iloCmtAlw);
  Lx.AddRw("ADD"); Lx.AddRw("ALTER"); Lx.AddRw("CONSTRAINT");
  Lx.AddRw("CREATE"); Lx.AddRw("FOREIGN"); Lx.AddRw("INDEX");
  Lx.AddRw("INSERT"); Lx.AddRw("KEY"); Lx.AddRw("NOT");
  Lx.AddRw("NULL"); Lx.AddRw("ON"); Lx.AddRw("PRIMARY");
  Lx.AddRw("REFERENCES"); Lx.AddRw("TABLE");

  TLxSym syAlter=Lx.GetRw("ALTER");
  TLxSym syCreate=Lx.GetRw("CREATE");
  TLxSym syIndex=Lx.GetRw("INDEX");
  TLxSym syInsert=Lx.GetRw("INSERT");
  TLxSym syTable=Lx.GetRw("TABLE");

  while (Lx.GetSym()!=syEof){
    if (Lx.Sym==syMinus){
      Lx.GetSym(syMinus); Lx.GetSym(syLn);
    } else
    if (Lx.Sym==syAlter){
      Lx.GetSym();
      if (Lx.Sym==syTable){
        TStr TbNm=Lx.GetIdStr();
        PSqlTbDef TbDef=SqlDef->GetTbDef(TbNm);
        TbDef->LoadAlterTxt(Lx, TFSet()|sySemicolon);
      } else {
        Fail;
      }
    } else
    if (Lx.Sym==syCreate){
      Lx.GetSym();
      if (Lx.Sym==syTable){
        PSqlTbDef TbDef=TSqlTbDef::LoadTxt(Lx, TFSet()|sySemicolon);
        SqlDef->AddTbDef(TbDef);
      } else
      if (Lx.Sym==syIndex){
        Lx.SkipToSym(sySemicolon);
      } else {
        Fail;
      }
    } else
    if (Lx.Sym==syInsert){
      Lx.SkipToSym(sySemicolon);
    } else {
      Fail;
    }
  }

  return SqlDef;
}

