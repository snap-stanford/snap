/////////////////////////////////////////////////
// Json-Value
PJsonVal TJsonVal::NewArr(const TJsonValV& ValV) {
	PJsonVal Val = TJsonVal::NewArr();
	for (int ValN = 0; ValN < ValV.Len(); ValN++) {
		Val->AddToArr(ValV[ValN]);
	}
	return Val;
}

PJsonVal TJsonVal::NewArr(const TStrV& StrV) {
	PJsonVal Val = TJsonVal::NewArr();
	for (int StrN = 0; StrN < StrV.Len(); StrN++) {
		Val->AddToArr(TJsonVal::NewStr(StrV[StrN]));
	}
	return Val;
}

PJsonVal TJsonVal::GetObjKey(const TStr& Key) const {
  EAssert(IsObj());
  EAssert(IsObjKey(Key)); 
  return KeyValH.GetDat(Key);
}

PJsonVal TJsonVal::GetObjKey(const char *Key) const {
  EAssert(IsObj());
  EAssert(IsObjKey(Key));
  return KeyValH.GetDat(Key);
}

bool TJsonVal::GetObjBool(const TStr& Key, const bool& DefBool) const { 
  EAssert(IsObj());
  return (IsObjKey(Key)) ? KeyValH.GetDat(Key)->GetBool() : DefBool;
}

bool TJsonVal::GetObjBool(const char *Key, const bool& DefBool) const { 
  EAssert(IsObj());
  return (IsObjKey(Key)) ? KeyValH.GetDat(Key)->GetBool() : DefBool;
}

double TJsonVal::GetObjNum(const TStr& Key, const double& DefNum) const { 
  EAssert(IsObj());
  return (IsObjKey(Key)) ? KeyValH.GetDat(Key)->GetNum() : DefNum;
} 

double TJsonVal::GetObjNum(const char *Key, const double& DefNum) const { 
  EAssert(IsObj());
  return (IsObjKey(Key)) ? KeyValH.GetDat(Key)->GetNum() : DefNum;
}

TStr TJsonVal::GetObjStr(const TStr& Key, const TStr& DefStr) const { 
  EAssert(IsObj());
  return (IsObjKey(Key)) ? KeyValH.GetDat(Key)->GetStr() : DefStr;
}

TStr TJsonVal::GetObjStr(const char *Key, const TStr& DefStr) const { 
  EAssert(IsObj());
  return (IsObjKey(Key)) ? KeyValH.GetDat(Key)->GetStr() : DefStr;
}
  
PJsonVal TJsonVal::GetValFromLx(TILx& Lx){
  static TFSet ValExpect=TFSet()|syIdStr|syFlt|syQStr|syLBracket|syLBrace;
  PJsonVal Val=TJsonVal::New();
  if ((Lx.Sym==syIdStr)&&(Lx.Str=="null")){
    Val->PutNull(); Lx.GetSym();
  } else if ((Lx.Sym==syIdStr)&&(Lx.Str=="true")){
    Val->PutBool(true); Lx.GetSym();
  } else if ((Lx.Sym==syIdStr)&&(Lx.Str=="false")){
    Val->PutBool(false); Lx.GetSym();
  } else if (Lx.Sym==syFlt){
    Val->PutNum(Lx.Flt); Lx.GetSym();
  } else if (Lx.Sym==syQStr){
    Val->PutStr(Lx.Str); Lx.GetSym();
  } else if ((Lx.Sym==syLBracket)){
    Val->PutArr(); Lx.GetSym(ValExpect); // added ValExpect to correctyl parse arrays of floats
    if (Lx.Sym!=syRBracket){
      forever{
        PJsonVal SubVal=TJsonVal::GetValFromLx(Lx);
        Val->AddToArr(SubVal);
        if (Lx.Sym==syComma){Lx.GetSym(ValExpect);} 
        else if (Lx.Sym==syRBracket){break;} 
        else {TExcept::Throw("JSON Array not properly formed.");}
      }
    }
    Lx.GetSym();
  } else if ((Lx.Sym==syLBrace)){
    Val->PutObj(); Lx.GetSym(TFSet()|syRBrace|syQStr);
    if (Lx.Sym!=syRBrace){
      forever{
        TStr SubKey=Lx.Str; 
        Lx.GetSym(syColon); 
        Lx.GetSym(ValExpect);
        PJsonVal SubVal=TJsonVal::GetValFromLx(Lx);
        Val->AddToObj(SubKey, SubVal);
        if (Lx.Sym==syComma){Lx.GetSym(TFSet()|syQStr);} 
        else if (Lx.Sym==syRBrace){break;} 
        else {TExcept::Throw("JSON Object not properly formed.");}
      }
    }
    Lx.GetSym();
  } else {
    TExcept::Throw("Unexpected JSON symbol.");
  }
  return Val;
}

PJsonVal TJsonVal::GetValFromStr(const TStr& JsonStr){
  PSIn SIn=TStrIn::New(JsonStr);
  TILx Lx(SIn, TFSet()|iloCmtAlw|iloCsSens|iloExcept|iloSigNum);
  PJsonVal Val; bool Ok=true; TStr MsgStr="Ok";
  try {
    Lx.GetSym(TFSet()|syLBracket|syLBrace);
    Val=GetValFromLx(Lx);
  }
  catch (PExcept Except){
    Ok=false; MsgStr=Except->GetMsgStr();
    Val=TJsonVal::New();
  }
  return Val;
}

void TJsonVal::AddEscapeChAFromStr(const TStr& Str, TChA& ChA){
	if (TUnicodeDef::IsDef()) {
		// parse the UTF8 string (old: TUStr InUStr(InStr);)
		TIntV UStr; TUnicodeDef::GetDef()->DecodeUtf8(Str, UStr);
		// escape the string
		for (int ChN = 0; ChN < UStr.Len(); ChN++) {
			const int UCh = UStr[ChN];
			if (UCh < 0x80) {
				// 7-bit ascii
				const char Ch = (char)UCh;
				switch (Ch) {
					case '"' : ChA.AddCh('\\'); ChA.AddCh('"'); break;
					case '\\' : ChA.AddCh('\\'); ChA.AddCh('\\'); break;
					case '/' : ChA.AddCh('\\'); ChA.AddCh('/'); break;
					case '\b' : ChA.AddCh('\\'); ChA.AddCh('b'); break;
					case '\f' : ChA.AddCh('\\'); ChA.AddCh('f'); break;
					case '\n' : ChA.AddCh('\\'); ChA.AddCh('n'); break;
					case '\r' : ChA.AddCh('\\'); ChA.AddCh('r'); break;
					case '\t' : ChA.AddCh('\\'); ChA.AddCh('t'); break;
					default :
						ChA.AddCh(Ch);
				}
			} else {
				// escape
				ChA += "\\u";
				ChA += TStr::Fmt("%04x", UCh);
			}
		}
	} else {
		// escape the string
		for (int ChN = 0; ChN < Str.Len(); ChN++) {
			const char Ch = Str[ChN];
			if ((Ch & 0x80) == 0) {
				// 7-bit ascii
				switch (Ch) {
					case '"' : ChA.AddCh('\\'); ChA.AddCh('"'); break;
					case '\\' : ChA.AddCh('\\'); ChA.AddCh('\\'); break;
					case '/' : ChA.AddCh('\\'); ChA.AddCh('/'); break;
					case '\b' : ChA.AddCh('\\'); ChA.AddCh('b'); break;
					case '\f' : ChA.AddCh('\\'); ChA.AddCh('f'); break;
					case '\n' : ChA.AddCh('\\'); ChA.AddCh('n'); break;
					case '\r' : ChA.AddCh('\\'); ChA.AddCh('r'); break;
					case '\t' : ChA.AddCh('\\'); ChA.AddCh('t'); break;
					default : ChA.AddCh(Ch);
				}
			} else {
				// escape
				ChA += "\\u";
				ChA += TStr::Fmt("%02x", (int)Ch);
			}
		}
	}
}

void TJsonVal::AddQChAFromStr(const TStr& Str, TChA& ChA){
  ChA+="\"";
  AddEscapeChAFromStr(Str, ChA);
  ChA+="\"";
}

void TJsonVal::GetChAFromVal(const PJsonVal& Val, TChA& ChA){
  switch (Val->GetJsonValType()){
    case jvtNull: 
      ChA+="null"; break;
    case jvtBool:
      if (Val->GetBool()){ChA+="true";} else {ChA+="false";} break;
    case jvtNum: 
      ChA+=TStr::Fmt("%g", Val->GetNum()); break;
    case jvtStr:
      AddQChAFromStr(Val->GetStr(), ChA); break;
    case jvtArr:
      ChA+="[";
      for (int ArrValN=0; ArrValN<Val->GetArrVals(); ArrValN++){
        if (ArrValN>0){ChA+=", ";}
        GetChAFromVal(Val->GetArrVal(ArrValN), ChA);
      }
      ChA+="]"; 
      break;
    case jvtObj:
      ChA+="{";
      for (int ObjKeyN=0; ObjKeyN<Val->GetObjKeys(); ObjKeyN++){
        if (ObjKeyN>0){ChA+=", ";}
        TStr ObjKey; PJsonVal ObjVal; Val->GetObjKeyVal(ObjKeyN, ObjKey, ObjVal);
        AddQChAFromStr(ObjKey, ChA);
        ChA+=":";
        GetChAFromVal(ObjVal, ChA);
      }
      ChA+="}"; 
      break;
	default: TExcept::Throw("Error parsing json");
  }
}

TStr TJsonVal::GetStrFromVal(const PJsonVal& Val){
  TChA ChA;
  GetChAFromVal(Val, ChA);
  return ChA;
}
