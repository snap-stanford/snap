/////////////////////////////////////////////////
// Wide-Char
class TWCh{
private:
  uchar MsVal;
  uchar LsVal;
public:
  static const TWCh Mn;
  static const TWCh Mx;
  static const int Vals;

  TWCh(): MsVal(0), LsVal(0){}
  TWCh(const uchar& _LsVal): MsVal(0), LsVal(_LsVal){}
  TWCh(const uchar& _MsVal, const uchar& _LsVal):
    MsVal(_MsVal), LsVal(_LsVal){}
  TWCh(const int& _MsVal, const int& _LsVal):
    MsVal(uchar(_MsVal)), LsVal(uchar(_LsVal)){}
  operator char() const {return LsVal;}
  TWCh(TSIn& SIn){SIn.Load(MsVal); SIn.Load(LsVal);}
  void Save(TSOut& SOut) const {SOut.Save(MsVal); SOut.Save(MsVal);}

  TWCh& operator=(const TWCh& WCh){
    MsVal=WCh.MsVal; LsVal=WCh.LsVal; return *this;}
  bool operator==(const TWCh& WCh) const {
    return (MsVal==WCh.MsVal)&&(LsVal==WCh.LsVal);}
  bool operator<(const TWCh& WCh) const {
    return (MsVal<WCh.MsVal)||((MsVal==WCh.MsVal)&&(LsVal<WCh.LsVal));}
  int GetMemUsed() const {return sizeof(MsVal)+sizeof(LsVal);}

  int GetPrimHashCd() const {return LsVal;}
  int GetSecHashCd() const {return MsVal;}

  char GetCh() const {
    if (MsVal==TCh::NullCh){return LsVal;} else {return '#';}}

  static TWCh LoadTxt(const PSIn& SIn){
    uchar LsVal=SIn->GetCh(); uchar MsVal=SIn->GetCh();
    return TWCh(MsVal, LsVal);}
  void SaveTxt(const PSOut& SOut) const {
    SOut->PutCh(MsVal); SOut->PutCh(LsVal);}

  static const TWCh StartWCh;
  static const TWCh TabWCh;
  static const TWCh LfWCh;
  static const TWCh CrWCh;
  static const TWCh SpaceWCh;
};
typedef TVec<TWCh> TWChV;

/////////////////////////////////////////////////
// Wide-Char-Array
class TWChA{
private:
  TWChV WChV;
  void AddCStr(const char* CStr);
  void PutCStr(const char* CStr);
public:
  TWChA(const int& MxWChs=0): WChV(MxWChs, 0){}
  TWChA(const TWChA& WChA): WChV(WChA.WChV){}
  TWChA(const TWChV& _WChV): WChV(_WChV){}
  TWChA(const char* CStr): WChV(){PutCStr(CStr);}
  TWChA(const TChA& ChA): WChV(){PutCStr(ChA.CStr());}
  TWChA(const TStr& Str): WChV(){PutCStr(Str.CStr());}
  ~TWChA(){}
  TWChA(TSIn& SIn): WChV(SIn){}
  void Save(TSOut& SOut){WChV.Save(SOut);}

  TWChA& operator=(const TWChA& WChA){
    if (this!=&WChA){WChV=WChA.WChV;} return *this;}
  TWChA& operator=(const char* CStr){PutCStr(CStr); return *this;}
  TWChA& operator=(const TChA& ChA){PutCStr(ChA.CStr()); return *this;}
  TWChA& operator=(const TStr& Str){PutCStr(Str.CStr()); return *this;}
  bool operator==(const TWChA& WChA) const {return WChV==WChA.WChV;}
  bool operator==(const char* CStr) const {return strcmp(GetStr().CStr(), CStr)!=0;}
  TWChA& operator+=(const char& Ch){WChV.Add(TWCh(Ch)); return *this;}
  TWChA& operator+=(const TWCh& WCh){WChV.Add(WCh); return *this;}
  TWChA& operator+=(const char* CStr){AddCStr(CStr); return *this;}
  TWChA& operator+=(const TChA& ChA){AddCStr(ChA.CStr()); return *this;}
  TWChA& operator+=(const TStr& Str){AddCStr(Str.CStr()); return *this;}
  TWChA& operator+=(const TWChA& WChA){WChV.AddV(WChA.WChV); return *this;}
  TWCh operator[](const int& ChN) const {return WChV[ChN];}
  int GetMemUsed(){return WChV.GetMemUsed();}

  void Clr(){WChV.Clr();}
  int Len() const {return WChV.Len();}
  bool Empty() const {return WChV.Empty();}
  TStr GetStr() const;

  void GetSubWChA(const int& BChN, const int& EChN, TWChA& WChA) const {
    WChV.GetSubValV(BChN, EChN, WChA.WChV);}
  void InsStr(const int& BChN, const TStr& Str);
  void DelSubStr(const int& BChN, const int& EChN);
  bool DelStr(const TStr& Str);
  void SplitOnCh(TStr& LStr, const char& SplitCh, TStr& RStr) const;

  int SearchCh(const TWCh& WCh, const int& BChN=0) const {
    return WChV.SearchForw(WCh, BChN);}
  int SearchStr(const TWChA& WChA, const int& BChN=0) const {
    return WChV.SearchVForw(WChA.WChV, BChN);}
  bool IsChIn(const char& Ch) const {return SearchCh(Ch)!=-1;}
  bool IsStrIn(const TWChA& WChA) const {return SearchStr(WChA)!=-1;}
  bool IsPrefix(const TWChA& WChA) const {
    TWChV SubWChV; WChV.GetSubValV(0, WChA.Len()-1, SubWChV);
    return SubWChV==WChA.WChV;}
  bool IsSufix(const TWChA& WChA) const {
    TWChV SubWChV; WChV.GetSubValV(Len()-WChA.Len(), Len()-1, SubWChV);
    return SubWChV==WChA.WChV;}
  int ChangeStr(const TStr& SrcStr, const TStr& DstStr, const int& BChN=0);
  int ChangeStrAll(const TStr& SrcStr, const TStr& DstStr);

  static void LoadTxt(const PSIn& SIn, TWChA& WChA);
  void SaveTxt(const PSOut& SOut) const;

  static TWChA EmptyWChA;
};


