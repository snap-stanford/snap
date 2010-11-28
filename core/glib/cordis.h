#ifndef cordisold_h
#define cordisold_h

/////////////////////////////////////////////////
// Includes
#include "net.h"
#include "mine.h"

/////////////////////////////////////////////////
// EuPartner
ClassTPV(TCordisEuPart, PCordisEuPart, TCordisEuPartV)//{
private:
  TStr DeptNm, OrgNm, OrgTypeNm, CountryNm;
  TBool CoordP;
public:
  TCordisEuPart(){}
  static PCordisEuPart New(){return new TCordisEuPart();}
  TCordisEuPart(const TCordisEuPart& LfTree){Fail;}
  TCordisEuPart(TSIn& SIn):
    DeptNm(SIn), OrgNm(SIn), OrgTypeNm(SIn),
    CountryNm(SIn), CoordP(SIn){}
  static PCordisEuPart Load(TSIn& SIn){return new TCordisEuPart(SIn);}
  void Save(TSOut& SOut){
    DeptNm.Save(SOut); OrgNm.Save(SOut); OrgTypeNm.Save(SOut);
    CountryNm.Save(SOut); CoordP.Save(SOut);}

  TCordisEuPart& operator=(const TCordisEuPart&){Fail; return *this;}

  // retrieve components
  TStr GetDeptNm() const {return DeptNm;}
  TStr GetOrgNm() const {return OrgNm;}
  TStr GetOrgTypeNm() const {return OrgTypeNm;}
  TStr GetCountryNm() const {return CountryNm;}
  bool IsCoord() const {return CoordP;}

  // extraction from html
  static TStr ExtrCountry(const TStr& AddrStr);
  static TStr ExtrDeptNm(const TStr& EuPartStr);
  static void ExtrEuPartV(const TStr& AllEuPartStr, TCordisEuPartV& EuPartV);

  // normalized string
  static TStr GetNrOrgNm(const TStr& Str);
};

/////////////////////////////////////////////////
// EuProject
ClassTPV(TCordisEuProj, PCordisEuProj, TCordisEuProjV)//{
private:
  TStr TitleStr, EuProgAcrStr, EuProjRefStr;
  TStr StartDateStr, EndDateStr, DurationStr;
  TStr EuProjStatusStr, EuProjAcrStr;
  TStr EuProjDescHtmlStr;
  TCordisEuPartV EuPartV;
  UndefCopyAssigm(TCordisEuProj);
public:
  TCordisEuProj(const TStr& _EuProjAcrStr=""):
    EuProjAcrStr(_EuProjAcrStr){}
  static PCordisEuProj New(const TStr& EuProjAcrStr=""){
    return new TCordisEuProj(EuProjAcrStr);}
  TCordisEuProj(TSIn& SIn):
    TitleStr(SIn), EuProgAcrStr(SIn), EuProjRefStr(SIn),
    StartDateStr(SIn), EndDateStr(SIn), DurationStr(SIn),
    EuProjStatusStr(SIn), EuProjAcrStr(SIn),
    EuProjDescHtmlStr(SIn), EuPartV(SIn){}
  static PCordisEuProj Load(TSIn& SIn){return new TCordisEuProj(SIn);}
  void Save(TSOut& SOut){
    TitleStr.Save(SOut); EuProgAcrStr.Save(SOut); EuProjRefStr.Save(SOut);
    StartDateStr.Save(SOut); EndDateStr.Save(SOut); DurationStr.Save(SOut);
    EuProjStatusStr.Save(SOut); EuProjAcrStr.Save(SOut);
    EuProjDescHtmlStr.Save(SOut); EuPartV.Save(SOut);}

  bool operator==(const TCordisEuProj& EuProj) const {
    return EuProjAcrStr==EuProj.EuProjAcrStr;}

  // retrieve simple components
  TStr GetTitleStr() const {return TitleStr;}
  TStr GetEuProgAcrStr() const {return EuProgAcrStr;}
  TStr GetEuProjRefStr() const {return EuProjRefStr;}
  TStr GetStartDateStr() const {return StartDateStr;}
  TStr GetEndDateStr() const {return EndDateStr;}
  TStr GetDurationStr() const {return DurationStr;}
  TStr GetEuProjStatusStr() const {return EuProjStatusStr;}
  TStr GetEuProjAcrStr() const {return EuProjAcrStr;}
  TStr GetEuProjDescHtmlStr() const {return EuProjDescHtmlStr;}

  // retrieve partners
  int GetEuParts() const {return EuPartV.Len();}
  PCordisEuPart GetEuPart(const int& EuPartN) const {return EuPartV[EuPartN];}

  // extraction from html
  static void ExtrEuProjV(const TStr& EuProjHtmlBsFNm, TCordisEuProjV& EuProjV);
};

/////////////////////////////////////////////////
// EuProject-Base
ClassTPV(TCordisEuProjBs, PCordisEuProjBs, TCordisEuProjBsV)//{
private:
  TCordisEuProjV EuProjV;
public:
  TCordisEuProjBs(){}
  static PCordisEuProjBs New(){return new TCordisEuProjBs();}
  TCordisEuProjBs(const TCordisEuProjBs& LfTree){Fail;}
  TCordisEuProjBs(TSIn& SIn): EuProjV(SIn){}
  static PCordisEuProjBs Load(TSIn& SIn){return new TCordisEuProjBs(SIn);}
  void Save(TSOut& SOut){EuProjV.Save(SOut);}

  TCordisEuProjBs& operator=(const TCordisEuProjBs&){Fail; return *this;}

  // retrieve components
  int GetEuProjs() const {return EuProjV.Len();}
  PCordisEuProj GetEuProj(const int& EuProjN) const {return EuProjV[EuProjN];}
  PCordisEuProj GetEuProj(const TStr& EuProjAcrStr) const {
    return EuProjV[EuProjV.SearchForw(TCordisEuProj::New(EuProjAcrStr))];}
  bool IsEuProj(const TStr& EuProjAcrStr, PCordisEuProj& EuProj) const {
    int EuProjN=EuProjV.SearchForw(TCordisEuProj::New(EuProjAcrStr));
    if (EuProjN==-1){return false;}
    else {EuProj=EuProjV[EuProjN]; return true;}}

  // extraction from html
  static PCordisEuProjBs ExtrEuProjBs(const TStr& EuProjHtmlBsFNm);

  // bag-of-words
  PBowDocBs GetBowDocBsFromEuProjDesc() const;

  // files
  static PCordisEuProjBs TCordisEuProjBs::LoadBin(const TStr& FNm){
    TFIn SIn(FNm); return Load(SIn);}
  void SaveBin(const TStr& FNm){
    TFOut SOut(FNm); Save(SOut);}
};

/////////////////////////////////////////////////
// EuProjects-Web-Fetch
typedef enum {eupUndef, eupAll,
 eupIST, eupQualityOfLife, eupGrowth, eupEEST, eupINCO2,
 eupInnovationSMEs, eupImprovingHumanPotential, eupEurAtom, eupMx} TEuProg;

class TCordisEuProjWebFetch: public TWebPgFetch{
private:
  PSOut EuProjSOut;
  TStr FetchSesIdStr;
  int FetchDocN;
  TStr FetchUrlStr;
  int FetchRetries;
public:
  TCordisEuProjWebFetch(const TStr& EuProjHtmlBsFNm):
    TWebPgFetch(),
    EuProjSOut(TFOut::New(EuProjHtmlBsFNm)),
    FetchSesIdStr(), FetchDocN(-1),
    FetchUrlStr(), FetchRetries(0){}

  // event handlers
  void OnFetch(const int& FId, const PWebPg& WebPg);
  void OnError(const int& FId, const TStr& MsgStr);

  // eu-programs
  static TStr GetEuProgUrlStr(const TEuProg& EuProg);
  static void FetchEuProg(const TEuProg& EuProg, const TStr& EuProjHtmlBsFNm);
};

#endif
