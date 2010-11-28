/////////////////////////////////////////////////
// Includes
#include "prologparser.h"

TPlChDef::TPlChDef(){
  // prepare character types (ChTypeV)
  ChTypeV.Gen(TPlChDef::MxChar-TPlChDef::MnChar+1);
  IAssert(TPlChDef::MnChar==0); IAssert(TPlChDef::MxChar==255);
  for (int Ch=MnChar; Ch<=MxChar; Ch++){ChTypeV[Ch]=plctUnDef;}
  for (int Ch=MnChar; Ch<=' '; Ch++){ChTypeV[Ch]=plctBl;}
  ChTypeV[EofCh]=plctEof;
  ChTypeV['!']=plctSolo;
  ChTypeV['"']=plctDQuo;
  ChTypeV['#']=plctSym;
  ChTypeV['$']=plctSym;
  ChTypeV['%']=plctComm;
  ChTypeV['&']=plctSym;
  ChTypeV['\'']=plctSQuo;
  ChTypeV['(']=plctMeta;
  ChTypeV[')']=plctMeta;
  ChTypeV['*']=plctSym;
  ChTypeV['+']=plctSym;
  ChTypeV[',']=plctSolo;
  ChTypeV['-']=plctSym;
  ChTypeV['.']=plctSym;
  ChTypeV['/']=plctSym;
  for (int Ch='0'; Ch<='9'; Ch++){ChTypeV[Ch]=plctNum;}
  ChTypeV[':']=plctSym;
  ChTypeV[';']=plctSolo;
  ChTypeV['<']=plctSym;
  ChTypeV['=']=plctSym;
  ChTypeV['>']=plctSym;
  ChTypeV['?']=plctSym;
  ChTypeV['@']=plctSym;
  for (int Ch='A'; Ch<='Z'; Ch++){ChTypeV[Ch]=plctUpCs;}
  ChTypeV['[']=plctMeta;
  ChTypeV['\\']=plctSym;
  ChTypeV[']']=plctMeta;
  ChTypeV['^']=plctSym;
  ChTypeV['_']=plctUnderLn;
  ChTypeV['`']=plctSym;
  for (int Ch='a'; Ch<='z'; Ch++){ChTypeV[Ch]=plctLwCs;}
  ChTypeV['{']=plctMeta;
  ChTypeV['|']=plctMeta;
  ChTypeV['}']=plctMeta;
  ChTypeV['~']=plctSym;
  ChTypeV[DelCh]=plctBl;

  // prepare identifier characters (IdChTab)
  IdChPV.Gen(MxChar-MnChar+1);
  for (int Ch=MnChar; Ch<=MxChar; Ch++){IdChPV[Ch]=false;}
  for (int Ch='0'; Ch<='9'; Ch++){IdChPV[Ch]=true;}
  for (int Ch='A'; Ch<='Z'; Ch++){IdChPV[Ch]=true;}
  IdChPV['_']=true;
  for (int Ch='a'; Ch<='z'; Ch++){IdChPV[Ch]=true;}

  // prepare character-escaping characters (CEscChTab)
  ChEscChPV.Gen(MxChar-MnChar+1);
  for (int Ch=MnChar; Ch<=MxChar; Ch++){ChEscChPV[Ch]=false;}
  ChEscChPV[HTabCh]=true;
  ChEscChPV[' ']=true;
  for (int Ch=MnPrintableCh; Ch<=MxPrintableCh; Ch++){ChEscChPV[Ch]=true;}
}

/////////////////////////////////////////////////
// Prolog-Atom-Definition
void TPlAtomDef::SetOp(const TPlOpType& OpType, const uint& OpPrec){
  switch (OpType){
    case plot_fx: (AtomFSet&=AFlgOpPrefixReset)|=AFlgOp_fx; PrefixOpPrec=OpPrec; break;
    case plot_fy: (AtomFSet&=AFlgOpPrefixReset)|=AFlgOp_fy; PrefixOpPrec=OpPrec; break;
    case plot_yfx: (AtomFSet&=AFlgOpInfixReset)|=AFlgOp_yfx; InfixOpPrec=OpPrec; break;
    case plot_xfx: (AtomFSet&=AFlgOpInfixReset)|=AFlgOp_xfx; InfixOpPrec=OpPrec; break;
    case plot_xfy: (AtomFSet&=AFlgOpInfixReset)|=AFlgOp_xfy; InfixOpPrec=OpPrec; break;
    case plot_xf: (AtomFSet&=AFlgOpPostfixReset)|=AFlgOp_xf; PostfixOpPrec=OpPrec; break;
    case plot_yf: (AtomFSet&=AFlgOpPostfixReset)|=AFlgOp_yf; PostfixOpPrec=OpPrec; break;
    default: Fail;
  }
}

/////////////////////////////////////////////////
// Prolog-Atom-Base
void TPlAtomBs::PrepImpAtoms(){
  // set important atom-ids
  ImpAtomIdV.Gen(plaMx);
  ImpAtomIdV[pla_Impl]=AddAtomStr(":-");
  ImpAtomIdV[pla_ImplDCG]=AddAtomStr("-->");
  ImpAtomIdV[pla_Query]=AddAtomStr("?-");
  ImpAtomIdV[pla_Mode]=AddAtomStr("mode");
  ImpAtomIdV[pla_Public]=AddAtomStr("public");
  ImpAtomIdV[pla_Dynamic]=AddAtomStr("dynamic");
  ImpAtomIdV[pla_MultiFile]=AddAtomStr("multifile");
  ImpAtomIdV[pla_MetaPredicate]=AddAtomStr("meta_predicate");
  ImpAtomIdV[pla_Semicolon]=AddAtomStr(";");
  ImpAtomIdV[pla_VerticalBar]=AddAtomStr("|");
  ImpAtomIdV[pla_If]=AddAtomStr("->");
  ImpAtomIdV[pla_Comma]=AddAtomStr(",");
  ImpAtomIdV[pla_Not]=AddAtomStr("\\+");
  ImpAtomIdV[pla_Spy]=AddAtomStr("spy");
  ImpAtomIdV[pla_NoSpy]=AddAtomStr("nospy");
  ImpAtomIdV[pla_Unify]=AddAtomStr("=");
  ImpAtomIdV[pla_Is]=AddAtomStr("is");
  ImpAtomIdV[pla_Uni]=AddAtomStr("=..");
  ImpAtomIdV[pla_Identical]=AddAtomStr("==");
  ImpAtomIdV[pla_NotIdentical]=AddAtomStr("\\==");
  ImpAtomIdV[pla_SOLess]=AddAtomStr("@<");
  ImpAtomIdV[pla_SOGreater]=AddAtomStr("@>");
  ImpAtomIdV[pla_SOLessOrEqual]=AddAtomStr("@=<");
  ImpAtomIdV[pla_SOGreaterOrEqual]=AddAtomStr("@>=");
  ImpAtomIdV[pla_AEqual]=AddAtomStr("=:=");
  ImpAtomIdV[pla_ANotEqual]=AddAtomStr("=\\=");
  ImpAtomIdV[pla_ALess]=AddAtomStr("<");
  ImpAtomIdV[pla_AGreater]=AddAtomStr(">");
  ImpAtomIdV[pla_ALessOrEqual]=AddAtomStr("=<");
  ImpAtomIdV[pla_AGreaterOrEqual]=AddAtomStr(">=");
  ImpAtomIdV[pla_Colon]=AddAtomStr(":");
  ImpAtomIdV[pla_Plus]=AddAtomStr("+");
  ImpAtomIdV[pla_Minus]=AddAtomStr("-");
  ImpAtomIdV[pla_BitOr]=AddAtomStr("\\/");
  ImpAtomIdV[pla_BitAnd]=AddAtomStr("/\\");
  ImpAtomIdV[pla_Divide]=AddAtomStr("/");
  ImpAtomIdV[pla_IntDivide]=AddAtomStr("//");
  ImpAtomIdV[pla_Times]=AddAtomStr("*");
  ImpAtomIdV[pla_ShiftLeft]=AddAtomStr("<<");
  ImpAtomIdV[pla_ShiftRight]=AddAtomStr(">>");
  ImpAtomIdV[pla_Mod]=AddAtomStr("mod");
  ImpAtomIdV[pla_ExistQuantifier]=AddAtomStr("^");
  ImpAtomIdV[pla_ExclPoint]=AddAtomStr("!");
  ImpAtomIdV[pla_EmptyList]=AddAtomStr("[]");
  ImpAtomIdV[pla_EmptyBrace]=AddAtomStr("{}");
  ImpAtomIdV[pla_EmptyParen]=AddAtomStr("()");
  ImpAtomIdV[pla_Period]=AddAtomStr(".");
  ImpAtomIdV[pla_VAR_N]=AddAtomStr("$VAR");
  ImpAtomIdV[pla_EndOfFile]=AddAtomStr("end_of_file");

  // set operators
  GetAtomDef(ImpAtomIdV[pla_Impl]).SetOp(plot_xfx, 1200);
  GetAtomDef(ImpAtomIdV[pla_ImplDCG]).SetOp(plot_xfx, 1200);
  GetAtomDef(ImpAtomIdV[pla_Impl]).SetOp(plot_fx, 1200);
  GetAtomDef(ImpAtomIdV[pla_Query]).SetOp(plot_fx, 1200);
  GetAtomDef(ImpAtomIdV[pla_Mode]).SetOp(plot_fx, 1150);
  GetAtomDef(ImpAtomIdV[pla_Public]).SetOp(plot_fx, 1150);
  GetAtomDef(ImpAtomIdV[pla_Dynamic]).SetOp(plot_fx, 1150);
  GetAtomDef(ImpAtomIdV[pla_MultiFile]).SetOp(plot_fx, 1150);
  GetAtomDef(ImpAtomIdV[pla_MetaPredicate]).SetOp(plot_fx, 1150);
  GetAtomDef(ImpAtomIdV[pla_Semicolon]).SetOp(plot_xfy, 1100);
  GetAtomDef(ImpAtomIdV[pla_VerticalBar]).SetOp(plot_xfy, 1100);
  GetAtomDef(ImpAtomIdV[pla_If]).SetOp(plot_xfy, 1050);
  GetAtomDef(ImpAtomIdV[pla_Comma]).SetOp(plot_xfy, 1000);
  GetAtomDef(ImpAtomIdV[pla_Not]).SetOp(plot_fy, 900);
  GetAtomDef(ImpAtomIdV[pla_Spy]).SetOp(plot_fy, 900);
  GetAtomDef(ImpAtomIdV[pla_NoSpy]).SetOp(plot_fy, 900);
  GetAtomDef(ImpAtomIdV[pla_Unify]).SetOp(plot_xfx, 700);
  GetAtomDef(ImpAtomIdV[pla_Is]).SetOp(plot_xfx, 700);
  GetAtomDef(ImpAtomIdV[pla_Uni]).SetOp(plot_xfx, 700);
  GetAtomDef(ImpAtomIdV[pla_Identical]).SetOp(plot_xfx, 700);
  GetAtomDef(ImpAtomIdV[pla_NotIdentical]).SetOp(plot_xfx, 700);
  GetAtomDef(ImpAtomIdV[pla_SOLess]).SetOp(plot_xfx, 700);
  GetAtomDef(ImpAtomIdV[pla_SOGreater]).SetOp(plot_xfx, 700);
  GetAtomDef(ImpAtomIdV[pla_SOLessOrEqual]).SetOp(plot_xfx, 700);
  GetAtomDef(ImpAtomIdV[pla_SOGreaterOrEqual]).SetOp(plot_xfx, 700);
  GetAtomDef(ImpAtomIdV[pla_AEqual]).SetOp(plot_xfx, 700);
  GetAtomDef(ImpAtomIdV[pla_ANotEqual]).SetOp(plot_xfx, 700);
  GetAtomDef(ImpAtomIdV[pla_ALess]).SetOp(plot_xfx, 700);
  GetAtomDef(ImpAtomIdV[pla_AGreater]).SetOp(plot_xfx, 700);
  GetAtomDef(ImpAtomIdV[pla_ALessOrEqual]).SetOp(plot_xfx, 700);
  GetAtomDef(ImpAtomIdV[pla_AGreaterOrEqual]).SetOp(plot_xfx, 700);
  GetAtomDef(ImpAtomIdV[pla_Colon]).SetOp(plot_xfy, 600);
  GetAtomDef(ImpAtomIdV[pla_Plus]).SetOp(plot_yfx, 500);
  GetAtomDef(ImpAtomIdV[pla_Minus]).SetOp(plot_yfx, 500);
  GetAtomDef(ImpAtomIdV[pla_BitOr]).SetOp(plot_yfx, 500);
  GetAtomDef(ImpAtomIdV[pla_BitAnd]).SetOp(plot_yfx, 500);
  GetAtomDef(ImpAtomIdV[pla_Plus]).SetOp(plot_fx, 500);
  GetAtomDef(ImpAtomIdV[pla_Minus]).SetOp(plot_fx, 500);
  GetAtomDef(ImpAtomIdV[pla_Divide]).SetOp(plot_yfx, 400);
  GetAtomDef(ImpAtomIdV[pla_IntDivide]).SetOp(plot_yfx, 400);
  GetAtomDef(ImpAtomIdV[pla_Times]).SetOp(plot_yfx, 400);
  GetAtomDef(ImpAtomIdV[pla_ShiftLeft]).SetOp(plot_yfx, 400);
  GetAtomDef(ImpAtomIdV[pla_ShiftRight]).SetOp(plot_yfx, 400);
  GetAtomDef(ImpAtomIdV[pla_Mod]).SetOp(plot_xfx, 300);
  GetAtomDef(ImpAtomIdV[pla_ExistQuantifier]).SetOp(plot_xfy, 200);
}

TPlAtomBs::TPlAtomBs():
  StrToAtomDefH(), ImpAtomIdV(){
  PrepImpAtoms();
}

/////////////////////////////////////////////////
// Prolog-Lexical
TPlLx::TPlLx(const PSIn& _SIn, const PPlAtomBs& _AtomBs, const PPlVarBs& _VarBs):
  ChDef(), SIn(_SIn), RSIn(*SIn),
  CCh(' '), NCh(' '), ChX(0), ChEscBf(), ChType(plctBl), Sym(plstUndef),
  AtomBs(_AtomBs), VarBs(_VarBs){}

void TPlLx::EThrow(const TStr& MsgStr) const {
  TChA FPosChA;
  FPosChA+=" [File:"; FPosChA+=SIn->GetSNm();
  //FPosChA+=" Line:"; FPosChA+=TInt::GetStr(LnN);
  FPosChA+=" Char:"; FPosChA+=TInt::GetStr(ChX);
  FPosChA+="]";
  TStr FullMsgStr=MsgStr+FPosChA;
  TExcept::Throw(FullMsgStr);
}

void TPlLx::SkipComment(){
  while (!((CCh=='*')&&(NCh=='/'))){
    if ((CCh=='/')&&(NCh=='*')){
      GetCh(); GetCh(); SkipComment();
    } else {
      GetCh();
      if (CCh==TPlChDef::EofCh){EThrow("Comment not concluded with '*/'");}
    }
  }
  GetCh(); GetCh();
}

void TPlLx::SkipWs(){
  forever {
    while (ChType==plctBl){GetCh();}
    if ((CCh=='/')&&(NCh=='*')){
      GetCh(); GetCh(); SkipComment();
    } else
    if (CCh=='%'){
      forever{
        GetCh();
        if ((CCh==TPlChDef::CrCh)||(CCh==TPlChDef::LfCh)||(CCh==TPlChDef::EofCh)){break;}
      }
      if (CCh==TPlChDef::EofCh){break;} else {GetCh();}
    } else {
      break;
    }
  }
}

void TPlLx::GetChEscBf(){
  ChEscBf.Clr(); GetCh();
  switch (toupper(CCh)){
    case 'B': ChEscBf+=TPlChDef::BsCh; GetCh(); break;
    case 'T': ChEscBf+=TPlChDef::HTabCh; GetCh(); break;
    case 'N': ChEscBf+=TPlChDef::LfCh; GetCh(); break;
    case 'V': ChEscBf+=TPlChDef::VTabCh; GetCh(); break;
    case 'F': ChEscBf+=TPlChDef::FfCh; GetCh(); break;
    case 'R': ChEscBf+=TPlChDef::CrCh; GetCh(); break;
    case 'E': ChEscBf+=TPlChDef::EscCh; GetCh(); break;
    case 'D': ChEscBf+=TPlChDef::DelCh; GetCh(); break;
    case '0': case '1': case '2': case '3':
    case '4': case '5': case '6': case '7': // octal number
      {int Cd=0; int Chs=0;
      forever{
        Chs++;
        Cd=Cd*8+(CCh-48);
        if (Cd>TPlChDef::MxChar){EThrow("Octal number overflow");}
        GetCh();
        if ((Chs==3)||(CCh<'0')||(CCh>'7')){break;}
      }
      ChEscBf+=char(Cd); GetCh(); break;}
    case '^': // control character
      GetCh();
      if ((CCh<'@')||(CCh>'_')){
        EThrow("Invalid escaped control character");}
      ChEscBf+=CCh-'@'; GetCh(); break;
    case 'C' : // ignore till next printable character
      forever{
        GetCh();
        if (CCh==TPlChDef::EofCh){EThrow("Unexpected end-of-file");}
        if ((CCh>=TPlChDef::MnPrintableCh)&&(CCh<=TPlChDef::MxPrintableCh)){
          break;}
      }
    default:
      ChEscBf+=CCh; GetCh(); break;
  }
}

void TPlLx::GetVar(){
  Sym=plsyVar; ChA.Clr();
  do {
    ChA+=CCh; GetCh();
  } while (ChDef.IsIdCh(CCh));
  VarId=VarBs->AddVarStr(ChA);
}

void TPlLx::GetAtomNm(){
  Sym=plsyAtom; ChA.Clr();
  do {
    ChA+=CCh; GetCh();
  } while (ChDef.IsIdCh(CCh));
  if (CCh=='('){GetCh(); Sym=plsyFAtom;}
  AtomId=AtomBs->AddAtomStr(ChA);
}

void TPlLx::GetAtomSym(){
  Sym=plsyAtom; ChA.Clr();
  do {
    ChA+=CCh; GetCh();
  } while (ChType==plctSym);
  if (CCh=='('){GetCh(); Sym=plsyFAtom;}
  AtomId=AtomBs->AddAtomStr(ChA);
}

void TPlLx::GetAtomQ(){
  Sym=plsyAtom; ChA.Clr();
  GetCh();
  forever{
    if ((CCh=='\'')&&(NCh!='\'')){GetCh(); break;}
    if ((CCh=='\'')&&(NCh=='\'')){GetCh();}
    if (ChEscapingP&&(CCh==ChDef.StartChEscCh)){
      GetChEscBf(); ChA+=ChEscBf;
    } else {
      ChA+=CCh; GetCh();
    }
  }
  if (CCh=='('){GetCh(); Sym=plsyFAtom;}
  AtomId=AtomBs->AddAtomStr(ChA);
  AtomBs->GetAtomDef(AtomId).AtomFSet|=TPlAtomDef::AFlgWriteQ;
}

void TPlLx::GetListStr(){
  Sym=plsyList; ChA.Clr();
  GetCh();
  forever{
    if ((CCh=='"')&&(NCh!='"')){GetCh(); break;}
    if ((CCh=='"')&&(NCh=='"')){GetCh();}
    if (ChEscapingP&&(CCh==ChDef.StartChEscCh)){
      GetChEscBf(); ChA+=ChEscBf;
    } else {
      ChA+=CCh; GetCh();
    }
  }
}

void TPlLx::GetNum(){
  // integer or base
  Sym=plsyInt; ChA.Clr(); Int=0;
  do {
    Int=Int*10+CCh; ChA+=CCh; GetCh();
  } while (ChType==plctNum);
  if (CCh=='\''){ // integer
    int IntBase=Int;
    if (IntBase>MxIntBase){EThrow("Integer-Base too large.");}
    GetCh();
    if (IntBase==0){ // base==0 means character code
      if (ChEscapingP&&(CCh==ChDef.StartChEscCh)){
        GetChEscBf();
        if (ChEscBf.Len()>0){Int=ChEscBf[0];} else {Int=0;}
      } else {
        Int=uint(CCh); GetCh();
      }
    } else { // integer with custom base
      Int=0;
      do {
        int Digit=0;
        if (ChType==plctNum){ // number
          Digit=CCh-'0';
        } else
        if ((ChType==plctUpCs)||(ChType==plctLwCs)){ // character
          Digit=toupper(CCh)-'A'+10;
        }
        if (Digit>=IntBase){EThrow("To large digit for integer base.");}
        Int=Int*IntBase+Digit;
        GetCh();
      } while ((ChType==plctNum)||(ChType==plctUpCs)||(ChType==plctLwCs));
    }
  } else
  if ((CCh=='.')||(CCh=='e')||(CCh=='E')){
    if (CCh=='.'){
      do {
        ChA+=CCh; GetCh();
      } while (ChType==plctNum);
    }
    if ((CCh=='e')||(CCh=='E')){
      ChA+=CCh; GetCh();
      if ((CCh=='+')||(CCh=='-')){ChA+=CCh; GetCh();}
      while (ChType==plctNum){ChA+=CCh; GetCh();}
    }
    Sym=plsyFlt; Flt=atof(ChA.CStr());
  }
}

TPlLxSym TPlLx::GetSym(){
  SkipWs();
  switch (ChType){
    case plctUpCs: // variable
      GetVar(); break;
    case plctUnderLn: // void variable or variable
      if (ChDef.IsIdCh(CCh)){GetVar();}
      else {Sym=plsyVoid; GetCh();}
      break;
    case plctLwCs: // atom - name
      GetAtomNm(); break;
    case plctSym: // neg. number or full-stop or atom - symbol characters
      if ((CCh=='-')&&(ChDef.GetChType(NCh)==plctNum)){
        GetCh(); GetNum();
        if (Sym==plsyInt){Int=-Int;}
        else if (Sym==plsyFlt){Flt=-Flt;}
      } else
      if ((CCh=='.')&&(NCh<=' ')){
        Sym=plsyFullStop; GetCh();
      } else {
        GetAtomSym();
      }
      break;
    case plctSolo: // atom - solo character atom
      Sym=plsyAtom;
      if (CCh=='!'){AtomId=AtomBs->GetImpAtomId(pla_ExclPoint);}
      else if (CCh==';'){AtomId=AtomBs->GetImpAtomId(pla_Semicolon);}
      else if (CCh==','){AtomId=AtomBs->GetImpAtomId(pla_Comma);}
      else {Fail;}
      GetCh(); break;
    case plctSQuo: // atom - single quote
      GetAtomQ(); break;
    case plctDQuo: // list string - list of characters
      GetListStr(); break;
    case plctMeta: // meta characters
      switch (CCh){
        case '(':
          GetCh(); SkipWs();
          if (CCh==')'){
            Sym=plsyAtom; AtomId=AtomBs->GetImpAtomId(pla_EmptyParen); GetCh();
          } else {
            Sym=plsyLPar;
          }
          break;
        case ')':
          Sym=plsyRPar; GetCh(); break;
        case '[':
          GetCh(); SkipWs();
          if (CCh==']'){
            Sym==plsyAtom; AtomId=AtomBs->GetImpAtomId(pla_EmptyList); GetCh();
          } else {
            Sym=plsyLBracket;
          }
          break;
        case ']':
          Sym=plsyRBracket; GetCh(); break;
        case '{':
          GetCh(); SkipWs();
          if (CCh=='}'){
            Sym=plsyAtom; AtomId=AtomBs->GetImpAtomId(pla_EmptyBrace); GetCh();
          } else {
            Sym=plsyLBrace;
          }
          break;
        case '}':
          Sym=plsyRBrace; GetCh(); break;
        case '|':
          if (ParseInsideListExpP){
            Sym=plsyVBar; GetCh();
          } else {
            Sym=plsyAtom; AtomId=AtomBs->GetImpAtomId(pla_VerticalBar); GetCh();
          }
          break;
        default:
          EThrow("Unknown meta character.");
      }
    case plctNum: // integer or float symbol
      GetNum(); break;
    case plctEof: // end-of-file symbol
      Sym=plsyEof; break;
    default:
      EThrow("Invalid character type");
  }
  return Sym;
}

