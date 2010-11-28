/////////////////////////////////////////////////
// Forward
class TLwOnto;

/////////////////////////////////////////////////
// Light-Weight-Language-Base
ClassTP(TLwLangBs, PLwLangBs)//{
private:
  TStrStrH LangNmToDescH;
  UndefCopyAssign(TLwLangBs);
public:
  TLwLangBs(): LangNmToDescH(){}
  static PLwLangBs New(){return new TLwLangBs();}
  TLwLangBs(TSIn& SIn): LangNmToDescH(SIn){}
  static PLwLangBs Load(TSIn& SIn){return new TLwLangBs(SIn);}
  void Save(TSOut& SOut) const {LangNmToDescH.Save(SOut);}

  int GetLangs() const {return LangNmToDescH.Len();}
  int AddLang(const TStr& LangNm, const TStr& LangDesc){
    LangNmToDescH.AddDat(LangNm, LangDesc); return LangNmToDescH.GetKeyId(LangNm);}
  bool IsLangId(const int& LangId) const {return LangNmToDescH.IsKeyId(LangId);}
  bool IsLangNm(const TStr& LangNm) const {return LangNmToDescH.IsKey(LangNm);}
  int GetLangId(const int& LangN) const {return LangN;}
  int GetLangId(const TStr& LangNm) const {return LangNmToDescH.GetKeyId(LangNm);}
  TStr GetLangNm(const int& LangId) const {return LangNmToDescH.GetKey(LangId);}
  TStr GetLangDesc(const int& LangId) const {return LangNmToDescH[LangId];}

  void SaveTxt(FILE* fOut);
};

/////////////////////////////////////////////////
// Light-Weight-Term-Type
ClassTPV(TLwTermType, PLwTermType, TLwTermTypeV)//{
private:
  TInt TermTypeId;
  TStr TermTypeNm;
  TInt LangId;
  TStr DescStr;
  UndefCopyAssign(TLwTermType);
public:
  TLwTermType(){}
  static PLwTermType New(){return new TLwTermType();}
  static PLwTermType New(
   const int& TermTypeId, const TStr& TermTypeNm, const int& LangId,
   const TStr& DescStr=""){
    PLwTermType LwTermType=TLwTermType::New();
    LwTermType->PutTermTypeId(TermTypeId);
    LwTermType->PutTermTypeNm(TermTypeNm);
    LwTermType->PutLangId(LangId);
    LwTermType->PutDescStr(DescStr);
    return LwTermType;
  }
  TLwTermType(TSIn& SIn):
    TermTypeId(SIn), TermTypeNm(SIn), LangId(SIn), DescStr(SIn){}
  static PLwTermType Load(TSIn& SIn){return new TLwTermType(SIn);}
  void Save(TSOut& SOut) const {
    TermTypeId.Save(SOut); TermTypeNm.Save(SOut); LangId.Save(SOut);
    DescStr.Save(SOut);}

  // components
  void PutTermTypeId(const int& _TermTypeId){TermTypeId=_TermTypeId;}
  int GetTermTypeId() const {return TermTypeId;}

  void PutTermTypeNm(const TStr& _TermTypeNm){TermTypeNm=_TermTypeNm;}
  TStr GetTermTypeNm() const {return TermTypeNm;}

  void PutLangId(const int& _LangId){LangId=_LangId;}
  int GetLangId() const {return LangId;}

  void PutDescStr(const TStr& _DescStr){DescStr=_DescStr;}
  TStr GetDescStr() const {return DescStr;}

  // files
  void SaveTxt(FILE* fOut);
};

/////////////////////////////////////////////////
// Light-Weight-Term-Type-Base
ClassTP(TLwTermTypeBs, PLwTermTypeBs)//{
private:
  THash<TInt, TLwTermTypeV> IdToTermTypeVH;
  TStrIntPrIntH TermTypeNmLangIdToIdH;
  UndefCopyAssign(TLwTermTypeBs);
public:
  TLwTermTypeBs(): IdToTermTypeVH(), TermTypeNmLangIdToIdH(){}
  static PLwTermTypeBs New(){return new TLwTermTypeBs();}
  TLwTermTypeBs(TSIn& SIn): IdToTermTypeVH(SIn), TermTypeNmLangIdToIdH(SIn){}
  static PLwTermTypeBs Load(TSIn& SIn){return new TLwTermTypeBs(SIn);}
  void Save(TSOut& SOut) const {
    IdToTermTypeVH.Save(SOut); TermTypeNmLangIdToIdH.Save(SOut);}

  int GetTermTypes() const {return IdToTermTypeVH.Len();}
  int AddTermType(const PLwTermType& LwTermType);
  bool IsTermTypeId(const int& TermTypeId) const {
    return IdToTermTypeVH.IsKey(TermTypeId);}
  int GetTermTypeId(const int& TermTypeN) const {
    return IdToTermTypeVH.GetKey(TermTypeN);}
  int GetTermTypeId(const TStr& TermTypeNm, const int& LangId) const {
    return TermTypeNmLangIdToIdH.GetDat(TStrIntPr(TermTypeNm, LangId));}
  PLwTermType GetTermType(const int& TermTypeId) const {
    return IdToTermTypeVH.GetDat(TermTypeId)[0];}

  // files
  void SaveTxt(FILE* fOut);
};

/////////////////////////////////////////////////
// Light-Weight-Term
ClassTPV(TLwTerm, PLwTerm, TLwTermV)//{
private:
  TInt TermId;
  TStr TermNm;
  TInt LangId;
  TInt TermTypeId;
  TStr DescStr;
  UndefCopyAssign(TLwTerm);
public:
  TLwTerm(): TermId(-1){}
  static PLwTerm New(){return new TLwTerm();}
  static PLwTerm New(
   const int& TermId, const TStr& TermNm, const int& LangId,
   const int& TermTypeId=-1, const TStr& DescStr=""){
    PLwTerm LwTerm=TLwTerm::New();
    LwTerm->PutTermId(TermId);
    LwTerm->PutTermNm(TermNm);
    LwTerm->PutLangId(LangId);
    LwTerm->PutTermTypeId(TermTypeId);
    LwTerm->PutDescStr(DescStr);
    return LwTerm;
  }
  TLwTerm(TSIn& SIn):
    TermId(SIn), TermNm(SIn), LangId(SIn), TermTypeId(SIn), DescStr(SIn){}
  static PLwTerm Load(TSIn& SIn){return new TLwTerm(SIn);}
  void Save(TSOut& SOut) const {
    TermId.Save(SOut); TermNm.Save(SOut); LangId.Save(SOut);
    TermTypeId.Save(SOut); DescStr.Save(SOut);}

  // components
  void PutTermId(const int& _TermId){TermId=_TermId;}
  int GetTermId() const {return TermId;}

  void PutTermNm(const TStr& _TermNm){TermNm=_TermNm;}
  TStr GetTermNm() const {return TermNm;}

  void PutLangId(const int& _LangId){LangId=_LangId;}
  int GetLangId() const {return LangId;}

  void PutTermTypeId(const int& _TermTypeId){TermTypeId=_TermTypeId;}
  int GetTermTypeId() const {return TermTypeId;}

  void PutDescStr(const TStr& _DescStr){DescStr=_DescStr;}
  TStr GetDescStr() const {return DescStr;}

  void SaveTxt(FILE* fOut);
};

/////////////////////////////////////////////////
// Light-Weight-Term-Base
ClassTP(TLwTermBs, PLwTermBs)//{
private:
  TLwOnto* Onto;
  TInt MxTermId;
  THash<TInt, TLwTermV> IdToTermVH;
  TStrIntPrIntH TermNmLangIdToToTermIdH;
  UndefCopyAssign(TLwTermBs);
public:
  TLwTermBs(TLwOnto* _Onto):
    Onto(_Onto), MxTermId(0),
    IdToTermVH(), TermNmLangIdToToTermIdH(){}
  static PLwTermBs New(TLwOnto* Onto){return new TLwTermBs(Onto);}
  TLwTermBs(TSIn& SIn, TLwOnto* _Onto):
    Onto(_Onto), MxTermId(SIn),
    IdToTermVH(SIn), TermNmLangIdToToTermIdH(SIn){}
  static PLwTermBs Load(TSIn& SIn, void* Onto){
    return new TLwTermBs(SIn, (TLwOnto*)Onto);}
  void Save(TSOut& SOut) const {
    MxTermId.Save(SOut);
    IdToTermVH.Save(SOut); TermNmLangIdToToTermIdH.Save(SOut);}

  int GetTerms() const {return IdToTermVH.Len();}
  void AddTerm(const PLwTerm& Term);
  int AddTermGetTermId(const PLwTerm& Term);
  bool IsTermId(const int& TermId){
    return IdToTermVH.IsKey(TermId);}
  int GetTermId(const int& TermN) const {
    return IdToTermVH.GetKey(TermN);}
  bool IsTermId(const TStr& TermNm, const int& LangId) const {
    return TermNmLangIdToToTermIdH.IsKey(TStrIntPr(TermNm, LangId));}
  int GetTermId(const TStr& TermNm, const int& LangId) const {
    return TermNmLangIdToToTermIdH.GetDat(TStrIntPr(TermNm, LangId));}
  void GetTermV(const int& TermId, TLwTermV& TermV){
    TermV=IdToTermVH.GetDat(TermId);}
  PLwTerm GetTerm(const int& TermId);

  void SaveTxt(FILE* fOut);
};

/////////////////////////////////////////////////
// Light-Weight-Link-Type
ClassTPV(TLwLinkType, PLwLinkType, TLwLinkTypeV)//{
private:
  TInt LinkTypeId;
  TStr LinkTypeNm;
  TInt LangId;
  TStr SDescStr;
  TStr DescStr;
  TInt ReverseLinkTypeId;
  TInt ParentLinkTypeId;
  UndefCopyAssign(TLwLinkType);
public:
  TLwLinkType(){}
  static PLwLinkType New(){return new TLwLinkType();}
  static PLwLinkType New(
   const int& LinkTypeId, const TStr& LinkTypeNm, const int& LangId,
   const TStr& SDescStr, const TStr& DescStr="",
   const int& ReverseLinkTypeId=-1, const int& ParentLinkTypeId=-1){
    PLwLinkType LwLinkType=TLwLinkType::New();
    LwLinkType->PutLinkTypeId(LinkTypeId);
    LwLinkType->PutLinkTypeNm(LinkTypeNm);
    LwLinkType->PutLangId(LangId);
    LwLinkType->PutSDescStr(SDescStr);
    LwLinkType->PutDescStr(DescStr);
    LwLinkType->PutReverseLinkTypeId(ReverseLinkTypeId);
    LwLinkType->PutParentLinkTypeId(ParentLinkTypeId);
    return LwLinkType;
  }
  TLwLinkType(TSIn& SIn):
    LinkTypeId(SIn), LinkTypeNm(SIn), LangId(SIn),
    SDescStr(SIn), DescStr(SIn),
    ReverseLinkTypeId(SIn), ParentLinkTypeId(SIn){}
  static PLwLinkType Load(TSIn& SIn){return new TLwLinkType(SIn);}
  void Save(TSOut& SOut) const {
    LinkTypeId.Save(SOut); LinkTypeNm.Save(SOut); LangId.Save(SOut);
    SDescStr.Save(SOut); DescStr.Save(SOut);
    ReverseLinkTypeId.Save(SOut); ParentLinkTypeId.Save(SOut);}

  // components
  void PutLinkTypeId(const int& _LinkTypeId){LinkTypeId=_LinkTypeId;}
  int GetLinkTypeId() const {return LinkTypeId;}

  void PutLinkTypeNm(const TStr& _LinkTypeNm){LinkTypeNm=_LinkTypeNm;}
  TStr GetLinkTypeNm() const {return LinkTypeNm;}

  void PutLangId(const int& _LangId){LangId=_LangId;}
  int GetLangId() const {return LangId;}

  void PutSDescStr(const TStr& _SDescStr){SDescStr=_SDescStr;}
  TStr GetSDescStr() const {return SDescStr;}

  void PutDescStr(const TStr& _DescStr){DescStr=_DescStr;}
  TStr GetDescStr() const {return DescStr;}

  void PutReverseLinkTypeId(const int& _ReverseLinkTypeId){ReverseLinkTypeId=_ReverseLinkTypeId;}
  int GetReverseLinkTypeId() const {return ReverseLinkTypeId;}

  void PutParentLinkTypeId(const int& _ParentLinkTypeId){ParentLinkTypeId=_ParentLinkTypeId;}
  int GetParentLinkTypeId() const {return ParentLinkTypeId;}

  void SaveTxt(FILE* fOut);
};

/////////////////////////////////////////////////
// Light-Weight-Link-Type-Base
ClassTP(TLwLinkTypeBs, PLwLinkTypeBs)//{
private:
  THash<TInt, TLwLinkTypeV> IdToLinkTypeVH;
  TStrIntPrIntH LinkTypeNmLangIdToIdH;
  UndefCopyAssign(TLwLinkTypeBs);
public:
  TLwLinkTypeBs(): IdToLinkTypeVH(), LinkTypeNmLangIdToIdH(){}
  static PLwLinkTypeBs New(){return new TLwLinkTypeBs();}
  TLwLinkTypeBs(TSIn& SIn): IdToLinkTypeVH(SIn), LinkTypeNmLangIdToIdH(SIn){}
  static PLwLinkTypeBs Load(TSIn& SIn){return new TLwLinkTypeBs(SIn);}
  void Save(TSOut& SOut) const {
    IdToLinkTypeVH.Save(SOut); LinkTypeNmLangIdToIdH.Save(SOut);}

  int GetLinkTypes() const {return IdToLinkTypeVH.Len();}
  void AddLinkType(const PLwLinkType& LwLinkType);
  bool IsLinkTypeId(const int& LinkTypeId) const {
    return IdToLinkTypeVH.IsKey(LinkTypeId);}
  int GetLinkTypeId(const int& LinkTypeN) const {
    return IdToLinkTypeVH.GetKey(LinkTypeN);}
  bool IsLinkType(const TStr& LinkTypeNm, const int& LangId) const {
    return LinkTypeNmLangIdToIdH.IsKey(TStrIntPr(LinkTypeNm, LangId));}
  int GetLinkTypeId(const TStr& LinkTypeNm, const int& LangId) const {
    return LinkTypeNmLangIdToIdH.GetDat(TStrIntPr(LinkTypeNm, LangId));}
  PLwLinkType GetLinkType(const int& LinkTypeId) const {
    return IdToLinkTypeVH.GetDat(LinkTypeId)[0];}

  void SaveTxt(FILE* fOut);
};

/////////////////////////////////////////////////
// Light-Weight-Link-Base
ClassTP(TLwLinkBs, PLwLinkBs)//{
private:
  TLwOnto* Onto;
  TIntIntPrVH FromTermIdToLinkTypeIdTermIdPrVH;
  TIntIntPrVH IntoTermIdToLinkTypeIdTermIdPrVH;
  UndefCopyAssign(TLwLinkBs);
public:
  TLwLinkBs(TLwOnto* _Onto):
    Onto(_Onto),
    FromTermIdToLinkTypeIdTermIdPrVH(),
    IntoTermIdToLinkTypeIdTermIdPrVH(){}
  static PLwLinkBs New(TLwOnto* Onto){return new TLwLinkBs(Onto);}
  TLwLinkBs(TSIn& SIn, TLwOnto* _Onto):
    Onto(_Onto),
    FromTermIdToLinkTypeIdTermIdPrVH(SIn),
    IntoTermIdToLinkTypeIdTermIdPrVH(SIn){}
  static PLwLinkBs Load(TSIn& SIn, void* Onto){
    return new TLwLinkBs(SIn, (TLwOnto*)Onto);}
  void Save(TSOut& SOut) const {
    FromTermIdToLinkTypeIdTermIdPrVH.Save(SOut);
    IntoTermIdToLinkTypeIdTermIdPrVH.Save(SOut);}

  int GetLinks() const;
  void AddLink(const int TermId1, int LinkTypeId, int TermId2);
  int GetFromLinks(const int SrcTermId) const;
  void GetFromLink(
   const int SrcTermId, const int& LinkN, int& LinkTypeId, int& DstTermId);
  int GetIntoLinks(const int DstTermId) const;
  void GetIntoLink(
   const int DstTermId, const int& LinkN, int& LinkTypeId, int& SrcTermId);

  void SaveTxt(FILE* fOut, const int& TermId, const TStr& IndentStr);
};

/////////////////////////////////////////////////
// Light-Weight-Ontology
ClassTP(TLwOnto, PLwOnto)//{
private:
  PLwLangBs LangBs;
  PLwTermTypeBs TermTypeBs;
  PLwTermBs TermBs;
  PLwLinkTypeBs LinkTypeBs;
  PLwLinkBs LinkBs;
  UndefCopyAssign(TLwOnto);
  TLwOnto* GetThis(){return this;}
public:
  TLwOnto():
    LangBs(TLwLangBs::New()),
    TermTypeBs(TLwTermTypeBs::New()), TermBs(TLwTermBs::New(GetThis())),
    LinkTypeBs(TLwLinkTypeBs::New()), LinkBs(TLwLinkBs::New(GetThis())){}
  static PLwOnto New(){
    return new TLwOnto();}
  TLwOnto(TSIn& SIn):
    LangBs(SIn), TermTypeBs(SIn), TermBs(SIn, GetThis()),
    LinkTypeBs(SIn), LinkBs(SIn, GetThis()){}
  static PLwOnto Load(TSIn& SIn){return new TLwOnto(SIn);}
  void Save(TSOut& SOut) const {
    LangBs.Save(SOut); TermTypeBs.Save(SOut); TermBs.Save(SOut);
    LinkTypeBs.Save(SOut); LinkBs.Save(SOut);}

  // componets
  PLwLangBs GetLangBs() const {return LangBs;}
  PLwTermTypeBs GetTermTypeBs() const {return TermTypeBs;}
  PLwTermBs GetTermBs() const {return TermBs;}
  PLwLinkTypeBs GetLinkTypeBs() const {return LinkTypeBs;}
  PLwLinkBs GetLinkBs() const {return LinkBs;}

  // files
  static PLwOnto LoadBin(const TStr& FNm){
    TFIn SIn(FNm); return Load(SIn);}
  void SaveBin(const TStr& FNm){
    TFOut SOut(FNm); Save(SOut);}
  void SaveTxt(const TStr& FNm);

  // special formats
  static PLwOnto LoadAgroVoc(const TStr& FPath);
  static PLwOnto LoadEuroVoc(const TStr& FPath);
  static PLwOnto LoadAsfaVoc(const TStr& FPath);
  static PLwOnto LoadDMozVoc(const TStr& DMozBinFBase, const TStr& FPath);
  static PLwOnto LoadVoc(const TStr& FNm);
};

/////////////////////////////////////////////////
// Light-Weight-Ontology-Grounding
typedef THash<TInt, PBowSpV> TTermIdToConceptSpVH;
typedef TFltIntPrV TSimTermIdPrV;

ClassTP(TLwOntoGround, PLwOntoGround)//{
private:
  PLwOnto LwOnto;
  PBowDocBs BowDocBs;
  PBowDocWgtBs BowDocWgtBs;
  THash<TInt, PBowSpV> TermIdToConceptSpVH;
  UndefCopyAssign(TLwOntoGround);
public:
  TLwOntoGround(){}
  static PLwOntoGround New(const PLwOnto& LwOnto, const PBowDocBs& BowDocBs,
   const PBowDocWgtBs& BowDocWgtBs, const TTermIdToConceptSpVH TermIdToConceptSpVH){
    PLwOntoGround OntoGround=new TLwOntoGround();
    OntoGround->LwOnto=LwOnto;
    OntoGround->BowDocBs=BowDocBs;
    OntoGround->BowDocWgtBs=BowDocWgtBs;
    OntoGround->TermIdToConceptSpVH=TermIdToConceptSpVH;
    return OntoGround;}
  TLwOntoGround(TSIn& SIn):
    LwOnto(SIn), BowDocBs(SIn), BowDocWgtBs(SIn), TermIdToConceptSpVH(SIn){}
  static PLwOntoGround Load(TSIn& SIn){return new TLwOntoGround(SIn);}
  void Save(TSOut& SOut) const {
    LwOnto.Save(SOut); BowDocBs.Save(SOut);
    BowDocWgtBs.Save(SOut); TermIdToConceptSpVH.Save(SOut);}

  // components
  PLwOnto GetLwOnto() const {return LwOnto;}
  PBowDocBs GetBowDocBs() const {return BowDocBs;}
  PBowDocWgtBs GetBowDocWgtBs() const {return BowDocWgtBs;}
  bool IsConceptSpV(const int& TermId) const {
    return TermIdToConceptSpVH.IsKey(TermId);}
  PBowSpV GetConceptSpV(const int& TermId){
    return TermIdToConceptSpVH.GetDat(TermId);}

  // generation
  static PLwOntoGround GetOntoGround(
   const PLwOnto& LwOnto, const PBowDocBs& BowDocBs,
   const TStr& LangNm, const bool& DocCatIsTermIdP,
   const double& CutWordWgtSumPrc);
  static PLwOntoGround GetOntoGroundNN(
   const PLwOnto& LwOnto, const PBowDocBs& BowDocBs,
   const TStr& LangNm);

  // classfication
  void ClassifySpV(const PBowSpV& QueryBowSpV, TSimTermIdPrV& SimTermIdPrV) const;
  void ClassifyStr(const TStr& QueryStr, TSimTermIdPrV& SimTermIdPrV) const;
  void ClassifyHtmlFNm(const TStr& QueryHtmlFNm, TSimTermIdPrV& SimTermIdPrV) const;
  void SaveCfySetTxt(FILE* fTxt, TSimTermIdPrV& SimTermIdPrV, const int& MxRank=25);
  void SaveCfySetXml(FILE* fXml, TSimTermIdPrV& SimTermIdPrV, const int& MxRank=25);

  // files
  static PLwOntoGround LoadBin(const TStr& FNm){
    TFIn SIn(FNm); return Load(SIn);}
  void SaveBin(const TStr& FNm){
    TFOut SOut(FNm); Save(SOut);}
  void SaveTxt(const TStr& FNm);
};

