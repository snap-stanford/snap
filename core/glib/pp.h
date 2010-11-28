/////////////////////////////////////////////////
// Property-Tag
typedef enum {
  ptUndef, ptBool, ptInt, ptFlt, ptStr, ptValV, ptSel, ptSet} TPpTagVal;

class TPpTag{
public:
  static TStr GetStr(const int& Tag);
  static TPpTagVal GetTag(const TStr& Str);
  static TPpTagVal LoadTxt(TILx& Lx){
    return GetTag(Lx.GetIdStr());}
  static void SaveTxt(TOLx& Lx, const TInt& Tag){SaveTxt(Lx, int(Tag));}
  static void SaveTxt(TOLx& Lx, const int& Tag){
    Lx.PutIdStr(GetStr(Tag));}
};

/////////////////////////////////////////////////
// Property-Value
class TPpVal{
private:
  TInt Tag;
  TBool Bool; TInt Int; TFlt Flt; TStr Str; TVec<TPpVal> ValV;
public:
  TPpVal(): Tag(ptUndef){}
  TPpVal(const TPpVal& PpVal): Tag(PpVal.Tag), Bool(PpVal.Bool),
    Int(PpVal.Int), Flt(PpVal.Flt), Str(PpVal.Str), ValV(PpVal.ValV){}
  TPpVal(const bool& _Bool): Tag(ptBool), Bool(_Bool){}
  TPpVal(const TBool& _Bool): Tag(ptBool), Bool(_Bool){}
  TPpVal(const int& _Int): Tag(ptInt), Int(_Int){}
  TPpVal(const TInt& _Int): Tag(ptInt), Int(_Int){}
  TPpVal(const double& _Flt): Tag(ptFlt), Flt(_Flt){}
  TPpVal(const TFlt& _Flt): Tag(ptFlt), Flt(_Flt){}
  TPpVal(const TStr& _Str): Tag(ptStr), Str(_Str){}
  TPpVal(const char* CStr): Tag(ptStr), Str(TStr(CStr)){}
  TPpVal(const TVec<TPpVal>& _ValV): Tag(ptValV), ValV(_ValV){}
  TPpVal(TSIn& SIn):
    Tag(SIn), Bool(SIn), Int(SIn), Flt(SIn), Str(SIn), ValV(SIn){}
  void Save(TSOut& SOut){
    Tag.Save(SOut); Bool.Save(SOut); Int.Save(SOut);
    Flt.Save(SOut); Str.Save(SOut); ValV.Save(SOut);}

  TPpVal& operator=(const TPpVal& PpVal){
    Tag=PpVal.Tag; Bool=PpVal.Bool; Int=PpVal.Int;
    Flt=PpVal.Flt; Str=PpVal.Str; ValV=PpVal.ValV; return *this;}
  bool operator==(const TPpVal& PpVal) const;
  bool operator<(const TPpVal& PpVal) const {
    if ((Tag==PpVal.Tag)&&(int(Tag)==ptInt)){return Int<PpVal.Int;}
    else if ((Tag==PpVal.Tag)&&(int(Tag)==ptFlt)){return Flt<PpVal.Flt;}
    else {Fail; return false;}}

  int GetTag() const {return (int)Tag;}
  TBool GetBool() const {IAssert(int(Tag)==ptBool); return Bool;}
  TInt GetInt() const {IAssert(int(Tag)==ptInt); return Int;}
  TFlt GetFlt() const {IAssert(int(Tag)==ptFlt); return Flt;}
  TStr GetStr() const {IAssert(int(Tag)==ptStr); return Str;}
  TVec<TPpVal> GetValV() const {IAssert(int(Tag)==ptValV); return ValV;}

  TStr GetValStr(const bool& DoAddTag=false) const;
  static TPpVal LoadTxt(TILx& Lx);
  void SaveTxt(TOLx& Lx) const;
};
typedef TVec<TPpVal> TPpValV;

/////////////////////////////////////////////////
// Property
class TPp;
typedef TPt<TPp> PPp;
typedef TVec<PPp> TPpV;

class TPp{
private:
  TCRef CRef;
private:
  static TStr PpNm;
private:
  TStr IdNm, UcIdNm, DescNm;
  TInt Tag, ValVTag;
  TPpVal Val, DfVal;
  TPpVal MnVal, MxVal;
  TPpValV CcValV;
  TPpV PpV;
  void IAssertSubPp() const {IAssert((int(Tag)==ptSel)||(int(Tag)==ptSet));}
  int GetPpN(const TStr& IdNm) const;
  void GetChA(const int& Lev, const bool& Brief, TChA& ChA) const;
public:
  TPp():
    IdNm(), UcIdNm(), DescNm(),
    Tag(ptUndef), ValVTag(),
    Val(), DfVal(), MnVal(), MxVal(),
    CcValV(), PpV(){}
  TPp(const TPp& Pp):
    IdNm(Pp.IdNm), UcIdNm(Pp.IdNm.GetUc()), DescNm(Pp.DescNm),
    Tag(Pp.Tag), ValVTag(Pp.ValVTag),
    Val(Pp.Val), DfVal(Pp.DfVal), MnVal(Pp.MnVal), MxVal(Pp.MxVal),
    CcValV(Pp.CcValV), PpV(Pp.PpV){}
  TPp(const TStr& _IdNm, const TStr& _DescNm,
   const TPpTagVal& _Tag=ptUndef, const TPpTagVal& _ValVTag=ptUndef):
    IdNm(_IdNm), UcIdNm(_IdNm.GetUc()), DescNm(_DescNm),
    Tag(_Tag), ValVTag(_ValVTag),
    Val(), DfVal(), MnVal(), MxVal(),
    CcValV(), PpV(){IAssert(int(ValVTag)!=ptValV);}
  TPp(TSIn& SIn):
    IdNm(SIn), UcIdNm(SIn), DescNm(SIn),
    Tag(SIn), ValVTag(SIn),
    Val(SIn), DfVal(SIn), MnVal(SIn), MxVal(SIn),
    CcValV(SIn), PpV(SIn){}
  static PPp Load(TSIn& SIn){return new TPp(SIn);}
  void Save(TSOut& SOut){
    IdNm.Save(SOut); UcIdNm.Save(SOut); DescNm.Save(SOut);
    Tag.Save(SOut); ValVTag.Save(SOut);
    Val.Save(SOut); DfVal.Save(SOut); MnVal.Save(SOut); MxVal.Save(SOut);
    CcValV.Save(SOut); PpV.Save(SOut);}

  TPp& operator=(const TPp& Pp){
    if (this!=&Pp){
      IdNm=Pp.IdNm; UcIdNm=Pp.UcIdNm; DescNm=Pp.DescNm;
      Tag=Pp.Tag; ValVTag=Pp.ValVTag;
      Val=Pp.Val; DfVal=Pp.DfVal; MnVal=Pp.MnVal; MxVal=Pp.MxVal;
      CcValV=Pp.CcValV; PpV=Pp.PpV;}
    return *this;}

  // basic operations
  TStr GetIdNm() const {return IdNm;}
  TStr GetUcIdNm() const {return UcIdNm;}
  TStr GetDescNm() const {return DescNm;}
  int GetTag() const {return Tag;}
  int GetValVTag() const {return ValVTag;}
  void PutVal(const TPpVal& _Val);
  TPpVal GetVal() const {if (Val==TPpVal()){return DfVal;} else {return Val;}}

  // property definition
  void PutDfVal(const TPpVal& _DfVal){PutVal(_DfVal); DfVal=Val;}
  TPpVal GetDfVal() const {return DfVal;}
  void PutMnMxVal(const TPpVal& _MnVal, const TPpVal& _MxVal){
    IAssert((int(Tag)==ptInt)||(int(Tag)==ptFlt));
    IAssert((Tag==_MnVal.GetTag())&&(Tag==_MxVal.GetTag()));
    MnVal=_MnVal; MxVal=_MxVal;}
  TPpVal GetMnVal() const {IAssert((int(Tag)==ptInt)||(int(Tag)==ptFlt)); return MnVal;}
  TPpVal GetMxVal() const {IAssert((int(Tag)==ptInt)||(int(Tag)==ptFlt)); return MxVal;}
  void AddCcVal(const TPpVal& _Val){
    IAssert((int(Tag)==ptStr)||(int(Tag)==_Val.GetTag()));
    IAssert(CcValV.SearchForw(_Val)==-1); CcValV.Add(_Val);}
  void PutCcValV(const TPpValV& CcValV){
    for (int CcValN=0; CcValN<CcValV.Len(); CcValN++){AddCcVal(CcValV[CcValN]);}}
  int GetCcVals() const {IAssert(int(Tag)==ptStr); return CcValV.Len();}
  TPpVal GetCcVal(const int& CcValN) const {
    IAssert(int(Tag)==ptStr); return CcValV[CcValN];}

  // property put/get
  bool IsPp(const TStr& IdNm) const {IAssertSubPp(); return GetPpN(IdNm)!=-1;}
  bool IsPp(const TStr& IdNm, PPp& Pp) const {
    IAssertSubPp(); int PpN=GetPpN(IdNm);
    if (PpN==-1){return false;} else {Pp=GetPp(PpN); return true;}}
  int GetPps() const {IAssertSubPp(); return PpV.Len();}
  int AddPp(const PPp& Pp){IAssertSubPp(); return PpV.Add(Pp);}
  void AddPpV(const TPpV& PpV){
    for (int PpN=0; PpN<PpV.Len(); PpN++){AddPp(PpV[PpN]);}}
  PPp GetPp(const int& PpN) const {IAssertSubPp(); return PpV[PpN];}
  PPp GetPp(const TStr& IdPath) const;
  PPp GetSelPp() const {
    IAssert(int(Tag)==ptSel); return GetPp(GetVal().GetStr());}

  // add&create property shortcuts
  int AddPpInt(const TStr& IdNm, const TStr& DescNm=TStr()){
    PPp Pp=PPp(new TPp(IdNm, DescNm, ptInt)); return AddPp(Pp);}
  int AddPpInt(const TStr& IdNm, const TStr& DescNm,
   const int& MnVal, const int& MxVal, const int& DfVal){
    PPp Pp=PPp(new TPp(IdNm, DescNm, ptInt));
    Pp->PutMnMxVal(MnVal, MxVal); Pp->PutDfVal(DfVal); return AddPp(Pp);}
  int AddPpFlt(const TStr& IdNm, const TStr& DescNm=TStr()){
    PPp Pp=PPp(new TPp(IdNm, DescNm, ptFlt)); return AddPp(Pp);}
  int AddPpFlt(const TStr& IdNm, const TStr& DescNm,
   const double& MnVal, const double& MxVal, const double& DfVal){
    PPp Pp=PPp(new TPp(IdNm, DescNm, ptFlt));
    Pp->PutMnMxVal(MnVal, MxVal); Pp->PutDfVal(DfVal); return AddPp(Pp);}
  int AddPpStr(const TStr& IdNm, const TStr& DescNm=TStr(), const TStr& DfVal=TStr()){
    PPp Pp=PPp(new TPp(IdNm, DescNm, ptStr));
    Pp->PutDfVal(DfVal); return AddPp(Pp);}
  static PPp GetSetPp(const TStr& IdNm, const TStr& DescNm=TStr()){
    return PPp(new TPp(IdNm, DescNm, ptSet));}

  // boolean property put/get shortcuts
  void PutValBool(const bool& Val){PutVal(TPpVal(Val));}
  TBool GetValBool() const {return GetVal().GetBool();}
  void PutValBool(const TStr& IdPath, const bool& Val){
    GetPp(IdPath)->PutVal(TPpVal(Val));}
  TBool GetValBool(const TStr& IdPath) const {
    return GetPp(IdPath)->GetVal().GetBool();}

  // integer property put/get shortcuts
  void PutValInt(const int& Val){PutVal(TPpVal(Val));}
  TInt GetValInt() const {return GetVal().GetInt();}
  void PutValInt(const TStr& IdPath, const int& Val){
    GetPp(IdPath)->PutVal(TPpVal(Val));}
  TInt GetValInt(const TStr& IdPath) const {
    return GetPp(IdPath)->GetVal().GetInt();}

  // float property put/get shortcuts
  void PutValFlt(const double& Val){PutVal(TPpVal(Val));}
  TFlt GetValFlt() const {return GetVal().GetFlt();}
  void PutValFlt(const TStr& IdPath, const double& Val){
    GetPp(IdPath)->PutVal(TPpVal(Val));}
  TFlt GetValFlt(const TStr& IdPath) const {
    return GetPp(IdPath)->GetVal().GetFlt();}

  // string property put/get shortcuts
  void PutValStr(const TStr& Val){PutVal(TPpVal(Val));}
  TStr GetValStr() const {return GetVal().GetStr();}
  void PutValStr(const TStr& IdPath, const TStr& Val){
    GetPp(IdPath)->PutVal(TPpVal(Val));}
  TStr GetValStr(const TStr& IdPath) const {
    return GetPp(IdPath)->GetVal().GetStr();}

  // value property vector put/get shortcuts
  void PutValValV(const TPpValV& Val){PutVal(TPpVal(Val));}
  TPpValV GetValValV() const {return GetVal().GetValV();}
  void PutValValV(const TStr& IdPath, const TPpValV& Val){
    GetPp(IdPath)->PutVal(TPpVal(Val));}
  TPpValV GetValValV(const TStr& IdPath) const {
    return GetPp(IdPath)->GetVal().GetValV();}

  // string
  TStr GetStr(const bool& Brief=true) const {
    TChA ChA; GetChA(0, Brief, ChA); return ChA;}

  // file
  static PPp LoadTxt(const PSIn& SIn){
    TILx Lx(SIn, TFSet()|iloCmtAlw|iloSigNum|oloCsSens); return LoadTxt(Lx);}
  void SaveTxt(const PSOut& SOut) const {
    TOLx Lx(SOut, TFSet()|oloCmtAlw|oloFrcEoln|oloSigNum|oloCsSens); SaveTxt(Lx);}
  static PPp LoadTxt(TILx& Lx);
  void SaveTxt(TOLx& Lx, const int& Lev=0) const;

  friend class TPt<TPp>;
};

inline PPp MkClone(const PPp& Pp){return new TPp(*Pp);}

