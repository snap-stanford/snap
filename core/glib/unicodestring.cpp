/////////////////////////////////////////////////
// Unicode-Definition

TUnicodeDef TUnicodeDef::UnicodeDef;

TStr TUnicodeDef::GetDfFNm(){
  return TSysProc::GetExeFNm().GetFPath()+"UnicodeDef.Bin";
}

/////////////////////////////////////////////////
// Unicode-String
TUStr::TUStr(const TStr& Str){
  AssertUnicodeDefOk();
  TUnicodeDef::GetDef()->DecodeUtf8(Str, UniChV);
  TIntV NfcUniChV; TUnicodeDef::GetDef()->Decompose(UniChV, NfcUniChV, true);
  UniChV=NfcUniChV;
}

void TUStr::ToLowerCase(){
  TUnicodeDef::GetDef()->ToSimpleLowerCase(UniChV);
}

void TUStr::ToUpperCase(){
  TUnicodeDef::GetDef()->ToSimpleUpperCase(UniChV);
}

void TUStr::ToStarterCase(){
  TIntV StarterUniChV;
  TUnicodeDef::GetDef()->ExtractStarters(UniChV, StarterUniChV);
  TUnicodeDef::GetDef()->Decompose(StarterUniChV, UniChV, true);
}

void TUStr::GetWordBoundPV(TBoolV& WordBoundPV){
  TUnicodeDef::GetDef()->FindWordBoundaries(UniChV, WordBoundPV);
}

void TUStr::GetWordUStrV(TUStrV& WordUStrV){
  // clear word vector
  WordUStrV.Clr();
  // create boundaries
  TBoolV WordBoundPV; GetWordBoundPV(WordBoundPV);
  IAssert(Len()==WordBoundPV.Len()-1);
  IAssert((WordBoundPV.Len()>0)&&(WordBoundPV.Last()));
  // traverse characters and bounds
  int UniChs=Len(); TIntV WordUniChV;
  for (int UniChN=0; UniChN<=UniChs; UniChN++){
    if ((UniChN==UniChs)||(WordBoundPV[UniChN+1])){ // finish or word-boundary
      if (UniChN<UniChs){ // if not finish
        // if last-word-char or single-alphabetic-char
        if ((!WordUniChV.Empty())||(IsAlphabetic(UniChV[UniChN]))){
          WordUniChV.Add(UniChV[UniChN]); // add char
        }
      }
      if (!WordUniChV.Empty()){ // add current word to vector
        TUStr WordUStr(WordUniChV); // construct word from char-vector
        WordUStrV.Add(WordUStr); // add word to word-vector
        WordUniChV.Clr(false); // clear char-vector
      }
    } else {
      // add character to char-vector
      WordUniChV.Add(UniChV[UniChN]);
    }
  }
}

TStr TUStr::GetStr() const {
  TStr Str=TUnicodeDef::GetDef()->EncodeUtf8Str(UniChV);
  return Str;
}

TStr TUStr::GetStarterLowerCaseStr() const {
  TIntV UniChV1; TIntV UniChV2; TIntV UniChV3;
  TUnicodeDef::GetDef()->GetSimpleLowerCase(UniChV, UniChV1);
  TUnicodeDef::GetDef()->ExtractStarters(UniChV1, UniChV2);
  TUnicodeDef::GetDef()->Decompose(UniChV2, UniChV3, true);
  TStr Str=TUnicodeDef::GetDef()->EncodeUtf8Str(UniChV3);
  return Str;
}

int TUStr::GetScriptId(const TStr& ScriptNm){
  return TUnicodeDef::GetDef()->ucd.GetScriptByName(ScriptNm);
}

TStr TUStr::GetScriptNm(const int& ScriptId){
  return TUnicodeDef::GetDef()->ucd.GetScriptName(ScriptId);
}

int TUStr::GetChScriptId(const int& UniCh){
  return TUnicodeDef::GetDef()->ucd.GetScript(UniCh);
}

TStr TUStr::GetChScriptNm(const int& UniCh){
  return GetScriptNm(GetChScriptId(UniCh));
}

TStr TUStr::GetChNm(const int& UniCh){
  TStr UniChNm(TUnicodeDef::GetDef()->ucd.GetCharNameS(UniCh));
  return UniChNm;
}

TStr TUStr::GetChTypeStr(const int& UniCh){
  TChA ChTypeChA;
  ChTypeChA+='[';
  if (IsCase(UniCh)){ChTypeChA+="Case,";}
  if (IsUpperCase(UniCh)){ChTypeChA+="UpperCase,";}
  if (IsLowerCase(UniCh)){ChTypeChA+="LowerCase,";}
  if (IsAlphabetic(UniCh)){ChTypeChA+="Alphabetic,";}
  if (IsMath(UniCh)){ChTypeChA+="Math,";}
  if (ChTypeChA.LastCh()=='['){ChTypeChA+=']';}
  else {ChTypeChA[ChTypeChA.Len()-1]=']';}
  return ChTypeChA;
}

bool TUStr::IsCase(const int& UniCh){
  TUniChInfo ChInfo;
  if (TUnicodeDef::GetDef()->ucd.IsGetChInfo(UniCh, ChInfo)){
    return ChInfo.IsCased();}
  else {return false;}
}

bool TUStr::IsUpperCase(const int& UniCh){
  TUniChInfo ChInfo;
  if (TUnicodeDef::GetDef()->ucd.IsGetChInfo(UniCh, ChInfo)){
    return ChInfo.IsUppercase();}
  else {return false;}
}

bool TUStr::IsLowerCase(const int& UniCh){
  TUniChInfo ChInfo;
  if (TUnicodeDef::GetDef()->ucd.IsGetChInfo(UniCh, ChInfo)){
    return ChInfo.IsLowercase();}
  else {return false;}
}

bool TUStr::IsAlphabetic(const int& UniCh){
  TUniChInfo ChInfo;
  if (TUnicodeDef::GetDef()->ucd.IsGetChInfo(UniCh, ChInfo)){
    return ChInfo.IsAlphabetic();}
  else {return false;}
}

bool TUStr::IsMath(const int& UniCh){
  TUniChInfo ChInfo;
  if (TUnicodeDef::GetDef()->ucd.IsGetChInfo(UniCh, ChInfo)){
    return ChInfo.IsMath();}
  else {return false;}
}


