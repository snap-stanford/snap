/////////////////////////////////////////////////
// Wide-Char
const TWCh TWCh::Mn(0, 0);
const TWCh TWCh::Mx(0xFF, 0xFF);
const int TWCh::Vals=0x10000;

const TWCh TWCh::StartWCh(0xFE, 0xFF);
const TWCh TWCh::TabWCh(TCh::TabCh);
const TWCh TWCh::LfWCh(TCh::LfCh);
const TWCh TWCh::CrWCh(TCh::CrCh);
const TWCh TWCh::SpaceWCh(' ');

/////////////////////////////////////////////////
// Wide-Char-Array
void TWChA::AddCStr(const char* CStr){
  int CStrLen=int(strlen(CStr));
  for (int ChN=0; ChN<CStrLen; ChN++){
    WChV.Add(TWCh(CStr[ChN]));}
}

void TWChA::PutCStr(const char* CStr){
  int CStrLen=int(strlen(CStr));
  WChV.Gen(CStrLen, 0);
  for (int ChN=0; ChN<CStrLen; ChN++){
    WChV.Add(TWCh(CStr[ChN]));}
}

TStr TWChA::GetStr() const {
  TChA ChA;
  for (int WChN=0; WChN<WChV.Len(); WChN++){
    ChA+=WChV[WChN].GetCh();}
  return ChA;
}

void TWChA::LoadTxt(const PSIn& SIn, TWChA& WChA){
  WChA.Clr();
  TWCh WCh=TWCh::LoadTxt(SIn);
  IAssert(WCh==TWCh::StartWCh);
  while (!SIn->Eof()){
    WChA+=TWCh::LoadTxt(SIn);}
}

void TWChA::SaveTxt(const PSOut& SOut) const {
  TWCh::StartWCh.SaveTxt(SOut);
  for (int WChN=0; WChN<WChV.Len(); WChN++){
    WChV[WChN].SaveTxt(SOut);}
}

TWChA TWChA::EmptyWChA;

