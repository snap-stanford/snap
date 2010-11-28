/////////////////////////////////////////////////
// Includes
#include "sqldm.h"

////////////////////////////////////////////////
// Lexical-Chars
void TSqlDmChDef::SetUcCh(const TStr& Str){
  for (int CC=1; CC<Str.Len(); CC++){
    uchar Ch=Str[CC];
    UcChV[Ch]=Str[0];
  }
}

void TSqlDmChDef::SetChTy(const TSqlDmChTy& ChTy, const TStr& Str){
  for (int CC=0; CC<Str.Len(); CC++){
    uchar Ch=Str[CC];
    ChTyV[Ch]=ChTy;
  }
}

TSqlDmChDef::TSqlDmChDef():
  ChTyV(TUCh::Vals), UcChV(TUCh::Vals){
  // Character-Types
  ChTyV.PutAll(TInt(dmctUndef));
  SetChTy(dmctLetter, "ABCDEFGHIJKLMNOPQRSTUVWXYZ_");
  SetChTy(dmctLetter, "abcdefghijklmnopqrstuvwxyz");
  SetChTy(dmctDigit, "0123456789");
  SetChTy(dmctSpace, " \t\r\n");
  SetChTy(dmctEof, TCh::EofCh);

  // Upper-Case
  for (int Ch=0; Ch<=TUCh::Mx; Ch++){UcChV[Ch]=uchar(Ch);}
  SetUcCh("Aa"); SetUcCh("Bb"); SetUcCh("Cc"); SetUcCh("Dd"); SetUcCh("Ee");
  SetUcCh("Ff"); SetUcCh("Gg"); SetUcCh("Hh"); SetUcCh("Ii"); SetUcCh("Jj");
  SetUcCh("Kk"); SetUcCh("Ll"); SetUcCh("Mm"); SetUcCh("Nn"); SetUcCh("Oo");
  SetUcCh("Pp"); SetUcCh("Qq"); SetUcCh("Rr"); SetUcCh("Ss"); SetUcCh("Tt");
  SetUcCh("Uu"); SetUcCh("Vv"); SetUcCh("Ww"); SetUcCh("Xx"); SetUcCh("Yy");
  SetUcCh("Zz");
}

/////////////////////////////////////////////////
// SqlDm-Lexical-Symbols
TStrIntH TSqlDmLxSymStr::RwStrToSymH;

void TSqlDmLxSymStr::InitRwStrToSymH(){
  AddRw("create", dsyRw_Create);
  AddRw("insert", dsyRw_Insert);
  AddRw("select", dsyRw_Select);
  AddRw("delete", dsyRw_Delete);
  AddRw("drop", dsyRw_Drop);
  AddRw("rename", dsyRw_Rename);

  AddRw("mining", dsyRw_Mining);
  AddRw("model", dsyRw_Model);
  AddRw("using", dsyRw_Using);

  AddRw("into", dsyRw_Into);
  AddRw("column_values", dsyRw_ColumnValues);
  AddRw("content", dsyRw_Content);
  AddRw("from", dsyRw_From);
  AddRw("predict", dsyRw_Predict);
  AddRw("prediction", dsyRw_Prediction);
  AddRw("join", dsyRw_Join);
  AddRw("on", dsyRw_On);
  AddRw("natural", dsyRw_Natural);
  AddRw("where", dsyRw_Where);
  AddRw("flattened", dsyRw_Flattened);
  AddRw("distinct", dsyRw_Distinct);
  AddRw("union", dsyRw_Union);
  AddRw("pmml", dsyRw_Pmml);

  AddRw("or", dsyRw_Or);
  AddRw("and", dsyRw_And);
  AddRw("not", dsyRw_Not);

  AddRw("long", dsyRw_Long);
  AddRw("boolean", dsyRw_Boolean);
  AddRw("text", dsyRw_Text);
  AddRw("nvarchar", dsyRw_Text);
  AddRw("ntext", dsyRw_Text);
  AddRw("double", dsyRw_Double);
  AddRw("date", dsyRw_Date);
  AddRw("table", dsyRw_Table);

  AddRw("normal", dsyRw_Normal);
  AddRw("uniform", dsyRw_Uniform);

  AddRw("model_existence_only", dsyRw_Binary);
  AddRw("null", dsyRw_Null);

  AddRw("discrete", dsyRw_Discrete);
  AddRw("continuous", dsyRw_Continuous);
  AddRw("ordered", dsyRw_Ordered);
  AddRw("sequence_time", dsyRw_SeqTime);
  AddRw("cyclical", dsyRw_Cyclical);

  AddRw("discretized", dsyRw_Discretized);
  AddRw("automatic", dsyRw_Automatic);
  AddRw("eqal_areas", dsyRw_EqualAreas);
  AddRw("tresholds", dsyRw_Thresholds);
  AddRw("clusters", dsyRw_Clusters);

  AddRw("key", dsyRw_Key);
  AddRw("probability", dsyRw_Probability);
  AddRw("variance", dsyRw_Variance);
  AddRw("stdev", dsyRw_StDev);
  AddRw("stddev", dsyRw_StDev);
  AddRw("probability_variance", dsyRw_ProbabilityVariance);
  AddRw("probability_stdev", dsyRw_ProbabilityStDev);
  AddRw("probability_stddev", dsyRw_ProbabilityStDev);
  AddRw("support", dsyRw_Support);

  AddRw("skip", dsyRw_Skip);

  AddRw("predictstdev", dsyRw_PredictStDev);
  AddRw("predictstddev", dsyRw_PredictStDev);
  AddRw("predictvariance", dsyRw_PredictVariance);
  AddRw("predictsupport", dsyRw_PredictSupport);
  AddRw("predictprobability", dsyRw_PredictProbability);
  AddRw("predictadjustedprobability", dsyRw_PredictAdjustedProbability);
  AddRw("predictprobabilitystdev", dsyRw_PredictProbabilityStDev);
  AddRw("predictprobabilitystddev", dsyRw_PredictProbabilityStDev);
  AddRw("predictprobabilityvariance", dsyRw_PredictProbabilityVariance);
  AddRw("predicthistogram", dsyRw_PredictHistogram);

  AddRw("$support", dsyRw_HSupport);
  AddRw("$variance", dsyRw_HVariance);
  AddRw("$stdev", dsyRw_HStDev);
  AddRw("$probability", dsyRw_HProbability);
  AddRw("$adjustedprobability", dsyRw_HAdjustedProbability);
  AddRw("$probabilityvariance", dsyRw_HProbabilityVariance);
  AddRw("$probabilitystdev", dsyRw_HProbabilityStDev);
  AddRw("$distance", dsyRw_HDistance);

  AddRw("predict_only", dsyRw_PredictOnly);

  AddRw("related", dsyRw_Related);
  AddRw("to", dsyRw_To);
  AddRw("of", dsyRw_Of);

  AddRw("openrowset", dsyRw_OpenQuery);

  AddRw("cluster", dsyRw_Cluster);
  AddRw("clusterdistance", dsyRw_ClusterDistance);
  AddRw("clusterprobability", dsyRw_ClusterProbability);

  AddRw("as", dsyRw_As);

  AddRw("shape", dsyRw_Shape);
  AddRw("append", dsyRw_Append);
  AddRw("relate", dsyRw_Relate);

  AddRw("exclude_null", dsyRw_ExcludeNull);
  AddRw("include_null", dsyRw_IncludeNull);
  AddRw("exclusive", dsyRw_Exclusive);
  AddRw("inclusive", dsyRw_Inclusive);
  AddRw("input_only", dsyRw_InputOnly);
  AddRw("include_statistics", dsyRw_IncludeStatistics);

  AddRw("topcount", dsyRw_TopCount);
  AddRw("topsum", dsyRw_TopSum);
  AddRw("toppercent", dsyRw_TopPercent);
  AddRw("bottomcount", dsyRw_BottomCount);
  AddRw("bottomsum", dsyRw_BottomSum);
  AddRw("bottompercent", dsyRw_BottomPercent);

  AddRw("rangemin", dsyRw_RangeMin);
  AddRw("rangemid", dsyRw_RangeMid);
  AddRw("rangemax", dsyRw_RangeMax);
}

TSqlDmLxSym TSqlDmLxSymStr::GetRwSym(const TStr& RwStr){
  if (RwStrToSymH.Empty()){InitRwStrToSymH();}
  TInt SymInt;
  if (RwStrToSymH.IsKeyGetDat(RwStr, SymInt)){
    TSqlDmLxSym Sym=TSqlDmLxSym(SymInt.Val);
    return Sym;
  } else {
    return dsyUndef;
  }
}

TStr TSqlDmLxSymStr::GetRwStr(const TSqlDmLxSym& Sym){
  for (int RwN=0; RwN<RwStrToSymH.Len(); RwN++){
    TSqlDmLxSym RwSym=TSqlDmLxSym(int(RwStrToSymH[RwN]));
    if (Sym==RwSym){
      TStr RwStr=RwStrToSymH.GetKey(RwN);
      return RwStr;
    }
  }
  Fail; return TStr::NullStr;
}

TStr TSqlDmLxSymStr::GetSymStr(const TSqlDmLxSym& Sym){
  switch (Sym){
    case dsyUndef: return "<Undef>";
    case dsyEof: return "<Eof>";
    case dsyName: return "<Name>";
    case dsyStr: return "<String>";
    case dsyInt: return "<Int>";
    case dsyFlt: return "<Float>";
    case dsyComma: return ",";
    case dsyPeriod: return ".";
    case dsySemicolon: return ";";
    case dsyLss: return "<";
    case dsyLEq: return "<=";
    case dsyEq: return "=";
    case dsyNEq: return "<>";
    case dsyGtr: return ">";
    case dsyGEq: return ">=";
    case dsyPlus: return "+";
    case dsyMinus: return "-";
    case dsyAsterisk: return "*";
    case dsySlash: return "/";
    case dsyLParen: return "(";
    case dsyRParen: return ")";
    case dsyLBrace: return "{";
    case dsyRBrace: return "}";
    default: return GetRwStr(Sym);
  }
}

/////////////////////////////////////////////////
// SqlDm-Lexical-Symbol-State
TSqlDmLxSymSt::TSqlDmLxSymSt():
  Sym(dsyUndef), Str(), UcStr(), Int(0), Flt(0),
  SymLnN(-1), SymLnChN(-1), SymChN(-1){}

TSqlDmLxSymSt::TSqlDmLxSymSt(const TSqlDmLxSymSt& SymSt):
  Sym(SymSt.Sym),
  Str(SymSt.Str), UcStr(SymSt.UcStr), Int(SymSt.Int), Flt(SymSt.Flt),
  SymLnN(SymSt.SymLnN), SymLnChN(SymSt.SymLnChN), SymChN(SymSt.SymChN){Fail;}

TSqlDmLxSymSt::TSqlDmLxSymSt(TSqlDmLx& Lx):
  Sym(Lx.Sym), Str(Lx.Str), UcStr(Lx.UcStr), Int(Lx.Int), Flt(Lx.Flt),
  SymLnN(Lx.SymLnN), SymLnChN(Lx.SymLnChN), SymChN(Lx.SymChN){}

void TSqlDmLxSymSt::Restore(TSqlDmLx& Lx){
  Lx.Sym=Sym; Lx.Str=Str; Lx.UcStr=UcStr; Lx.Int=Int; Lx.Flt=Flt;
  Lx.SymLnN=SymLnN; Lx.SymLnChN=SymLnChN; Lx.SymChN=Lx.SymChN;}

/////////////////////////////////////////////////
// SqlDm-Lexical
TSqlDmChDef TSqlDmLx::ChDef;

uchar TSqlDmLx::GetCh(){
  IAssert(Ch!=TCh::EofCh);
  PrevCh=Ch;
  if (ChStack.Empty()){Ch=(SIn->Eof()) ? TCh::EofCh : SIn->GetCh();}
  else {Ch=ChStack.Pop();}
  ChN++; if (Ch==TCh::LfCh){LnN++; LnChN=0;} else {LnChN++;}
  printf("%c", Ch);
  return Ch;
}

TSqlDmLx::TSqlDmLx(const PSIn& _SIn):
  SIn(_SIn),
  ChStack(), PrevCh(' '), Ch(' '),
  LnN(0), LnChN(0-1), ChN(0-1),
  PrevSymStStack(),
  Sym(dsyUndef), Str(), UcStr(), Int(0), Flt(0),
  SymLnN(-1), SymLnChN(-1), SymChN(-1){
}

TStr TSqlDmLx::GetFPosStr() const {
  TChA ChA;
  ChA+="File:"; ChA+=SIn->GetSNm();
  ChA+=" Line:"; ChA+=TInt::GetStr(LnN+1);
  ChA+=" Char:"; ChA+=TInt::GetStr(LnChN);
  return ChA;
}

TSqlDmLxSym TSqlDmLx::GetSym(const TFSet& Expect){
  if (!PrevSymStStack.Empty()){
    // symbols already on the stack
    PrevSymStStack.Top().Restore(*this); PrevSymStStack.Pop();
  } else {
    // usual symbol
    while (ChDef.IsSpace(Ch)){GetCh();}
    SymLnN=LnN; SymLnChN=LnChN; SymChN=ChN;

    if (ChDef.IsAlpha(Ch)){ // name
      Str.Clr(); UcStr.Clr();
      do {Str.AddCh(Ch); UcStr.AddCh(ChDef.GetUc(Ch));}
      while (ChDef.IsAlNum(GetCh()));
      Sym=TSqlDmLxSymStr::GetRwSym(UcStr);
      if (Sym==dsyUndef){Sym=dsyName;}
    } else
    if (Ch=='['){ // bracketed name
      Str.Clr(); UcStr.Clr(); GetCh();
      while ((Ch!=']')&&(Ch!=TCh::EofCh)){
        Str.AddCh(Ch); UcStr.AddCh(ChDef.GetUc(Ch)); GetCh();}
      if (Ch==TCh::EofCh){Sym=dsyUndef;}
      else {Sym=dsyName; GetCh();}
    } else
    if (Ch=='\''){ // string in single quotes
      Str.Clr(); UcStr.Clr(); GetCh();
      while ((Ch!='\'')&&(Ch!=TCh::EofCh)){
        Str.AddCh(Ch); UcStr.AddCh(ChDef.GetUc(Ch)); GetCh();}
      if (Ch==TCh::EofCh){Sym=dsyUndef;}
      else {Sym=dsyStr; GetCh();}
    } else
    if (ChDef.IsNum(Ch)){ // integer or float
      Str.Clr();
      do {Str.AddCh(Ch);} while (ChDef.IsNum(GetCh()));
      if (Ch=='.'){
        Str.AddCh(Ch);
        while (ChDef.IsNum(GetCh())){Str.AddCh(Ch);}
      }
      if ((Ch=='e')||(Ch=='E')){
        Str.AddCh(Ch); GetCh();
        if ((Ch=='+')||(Ch=='-')){Str.AddCh(Ch); GetCh();}
        while (ChDef.IsNum(Ch)){Str.AddCh(Ch); GetCh();}
      }
      UcStr=Str;
      if (Expect.In(syFlt)){Sym=dsyFlt; Flt=atof(Str.CStr());}
      else {Sym=dsyInt; Int=atoi(Str.CStr());}
    } else
    if (Ch=='/'){ // slash or // commenatry or /* */ commentary
      GetCh();
      if (Ch=='/'){
        do {GetCh();} while (!ChDef.IsEof(Ch));
        if (Ch==TCh::CrCh){
          if (GetCh()==TCh::LfCh){GetCh();}
        } else
        if (Ch==TCh::LfCh){
          if (GetCh()==TCh::CrCh){GetCh();}
        }
        GetSym(Expect);
      } else
      if (Ch=='*'){
        do {while (GetCh()!='*'){}} while (GetCh()!='/');
        GetCh(); GetSym(Expect);
      } else {
        Sym=dsySlash;
      }
    } else
    if (Ch=='-'){ // minus or -- commenatry
      GetCh();
      if (Ch=='-'){
        do {GetCh();} while (!ChDef.IsEof(Ch));
        if (Ch==TCh::CrCh){
          if (GetCh()==TCh::LfCh){GetCh();}
        } else
        if (Ch==TCh::LfCh){
          if (GetCh()==TCh::CrCh){GetCh();}
        }
        GetSym(Expect);
      } else {
        Sym=dsyMinus;
      }
    } else
    if (Ch==TCh::EofCh){ // end-of-file
      Sym=dsyEof;
    } else { // other symbols
      switch (Ch){
        case ',': Sym=dsyComma; GetCh(); break;
        case '.':
          if (ChDef.IsNum(GetCh())){
            PutCh('.'); PutCh('0'); return GetSym(Expect);}
          else {Sym=dsyPeriod;}
          break;
        case ';': Sym=dsySemicolon; GetCh(); break;
        case '<':
          GetCh();
          if (Ch=='='){Sym=dsyLEq; GetCh();}
          else if (Ch=='>'){Sym=dsyNEq; GetCh();}
          else {Sym=dsyLss;} break;
        case '=': Sym=dsyEq; GetCh(); break;
        case '>':
          if (GetCh()=='='){Sym=dsyGEq; GetCh();}
          else {Sym=dsyGtr;} break;
        case '+': Sym=dsyPlus; GetCh(); break;
        case '-': Sym=dsyMinus; GetCh(); break;
        case '*': Sym=dsyAsterisk; GetCh(); break;
        case '/': Sym=dsySlash; GetCh(); break;
        case '(': Sym=dsyLParen; GetCh(); break;
        case ')': Sym=dsyRParen; GetCh(); break;
        case '{': Sym=dsyLBrace; GetCh(); break;
        case '}': Sym=dsyRBrace; GetCh(); break;
        default: Sym=dsyUndef; GetCh(); break;
      }
    }
  }

  if ((!Expect.In(Sym))&&(!Expect.Empty())){
   TStr MsgStr=
    TStr("Unexpected symbol (")+Str+") ["+GetFPosStr()+"]";
   throw TExcept::Throw(MsgStr);
  }
  return Sym;
}

/////////////////////////////////////////////////
// SqlDm-Delete-Statement
PSqlDmStat_Delete TSqlDmStat_Delete::LoadTxt(TSqlDmLx& Lx, const TFSet& Expect){
  // create object
  PSqlDmStat_Delete Stat=TSqlDmStat_Delete::New();
  // parse object
  IAssert(Lx.Sym==dsyRw_Delete);
  Lx.GetSym(dsyAsterisk);
  Lx.GetSym(dsyRw_From);
  Stat->MdNm=Lx.GetName();
  Lx.GetSym(TFSet(Expect)|dsyPeriod);
  if (Lx.Sym==dsyPeriod){
    Lx.GetSym(dsyRw_Content); Stat->ContentP=false;
    Lx.GetSym(Expect);
  } else {
    Stat->ContentP=false;
  }
  // return object
  return Stat;
}

/////////////////////////////////////////////////
// SqlDm-Drop-Statement
PSqlDmStat_Drop TSqlDmStat_Drop::LoadTxt(TSqlDmLx& Lx, const TFSet& Expect){
  // create object
  PSqlDmStat_Drop Stat=TSqlDmStat_Drop::New();
  // parse object
  IAssert(Lx.Sym==dsyRw_Drop);
  Lx.GetSym(dsyRw_Mining);
  Lx.GetSym(dsyRw_Model);
  Stat->MdNm=Lx.GetName();
  Lx.GetSym(Expect);
  // return object
  return Stat;
}

/////////////////////////////////////////////////
// SqlDm-Rename-Statement
PSqlDmStat_Rename TSqlDmStat_Rename::LoadTxt(TSqlDmLx& Lx, const TFSet& Expect){
  // create object
  PSqlDmStat_Rename Stat=TSqlDmStat_Rename::New();
  // parse object
  IAssert(Lx.Sym==dsyRw_Rename);
  Lx.GetSym(dsyRw_Mining);
  Lx.GetSym(dsyRw_Model);
  Stat->SrcMdNm=Lx.GetName();
  Lx.GetSym(dsyRw_To);
  Stat->DstMdNm=Lx.GetName();
  Lx.GetSym(Expect);
  // return object
  return Stat;
}

/////////////////////////////////////////////////
// SqlDm-Statement
const TFSet TSqlDmStat::StatExpect(
 dsyRw_Create, dsyRw_Insert, dsyRw_Select,
 dsyRw_Delete, dsyRw_Drop, dsyRw_Rename);

PSqlDmStat TSqlDmStat::LoadTxt(TSqlDmLx& Lx, const TFSet& Expect){
  // create object
  PSqlDmStat Stat=TSqlDmStat::New();
  // parse object
  switch (Lx.Sym){
    case dsyRw_Create: break;
    case dsyRw_Insert: break;
    case dsyRw_Select: break;
    case dsyRw_Delete:
      Stat->Type=dstDelete;
      Stat->Delete=TSqlDmStat_Delete::LoadTxt(Lx, Expect);
      break;
    case dsyRw_Drop:
      Stat->Type=dstDrop;
      Stat->Drop=TSqlDmStat_Drop::LoadTxt(Lx, Expect);
      break;
    case dsyRw_Rename:
      Stat->Type=dstRename;
      Stat->Rename=TSqlDmStat_Rename::LoadTxt(Lx, Expect);
      break;
    default: Fail;
  }
  // return object
  return Stat;
}

/////////////////////////////////////////////////
// SqlDm-Program
PSqlDmProg TSqlDmProg::LoadTxt(PSIn& SIn){
  // create object
  PSqlDmProg Prog=TSqlDmProg::New();
  // parse object
  TSqlDmLx Lx(SIn);
  Lx.GetSym(TFSet(TSqlDmStat::StatExpect)|dsyEof);
  while (Lx.Sym!=dsyEof){
    PSqlDmStat Stat=TSqlDmStat::LoadTxt(Lx, TFSet(dsySemicolon, dsyEof));
    Prog->StatV.Add(Stat);
    Lx.GetSym(TFSet(TSqlDmStat::StatExpect)|dsyEof);
  }
  // return object
  return Prog;
}

