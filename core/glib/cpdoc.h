/////////////////////////////////////////////////
// Compact-Document
ClassTP(TCpDoc, PCpDoc)//{
public:
  TStr DocNm, DateStr, TitleStr;
  TStr HeadlineStr, BylineStr, DatelineStr;
  TStrV ParStrV;
  TStrV TopCdNmV, GeoCdNmV, IndCdNmV;
  UndefCopyAssign(TCpDoc);
public:
  TCpDoc(){}
  TCpDoc(const TStr& _DocNm, const TStr& _TitleStr, const TStr& ParStr):
    DocNm(_DocNm), TitleStr(_TitleStr), ParStrV(){ParStrV.Add(ParStr);}
  static PCpDoc New(){return PCpDoc(new TCpDoc());}
  static PCpDoc New(const TStr& DocNm, const TStr& TitleStr, const TStr& ParStr){
    return PCpDoc(new TCpDoc(DocNm, TitleStr, ParStr));}
  TCpDoc(TSIn& SIn):
    DocNm(SIn), DateStr(SIn), TitleStr(SIn),
    HeadlineStr(SIn), BylineStr(SIn), DatelineStr(SIn),
    ParStrV(SIn),
    TopCdNmV(SIn), GeoCdNmV(SIn), IndCdNmV(SIn){
    SIn.LoadCs();}
  static PCpDoc Load(TSIn& SIn){return new TCpDoc(SIn);}
  void Save(TSOut& SOut) const {
    DocNm.Save(SOut); DateStr.Save(SOut); TitleStr.Save(SOut);
    HeadlineStr.Save(SOut); BylineStr.Save(SOut); DatelineStr.Save(SOut);
    ParStrV.Save(SOut);
    TopCdNmV.Save(SOut); GeoCdNmV.Save(SOut); IndCdNmV.Save(SOut);
    SOut.SaveCs();}

  // document-id
  void PutDocNm(const TStr& _DocNm){DocNm=_DocNm;}
  TStr GetDocNm() const {return DocNm;}

  // date
  void PutDateStr(const TStr& _DateStr){DateStr=_DateStr;}
  TStr GetDateStr() const {return DateStr;}
  TSecTm GetDate() const {
    return TSecTm::GetDtTmFromYmdHmsStr(DateStr, '-', ':');}

  // categories
  void AddCatNm(const TStr& CatNm){TopCdNmV.Add(CatNm);}
  int GetCats() const {return TopCdNmV.Len();}
  TStr GetCatNm(const int& CatN) const {return GetTopCdNm(CatN);}

  // get full-document
  TStr GetTxtStr() const;
  TStr GetHtmlStr() const;

  // title/headline/byline/dateline
  void PutTitleStr(const TStr& _TitleStr){TitleStr=_TitleStr;}
  TStr GetTitleStr() const {return TitleStr;}
  void PutHeadlineStr(const TStr& _HeadlineStr){HeadlineStr=_HeadlineStr;}
  TStr GetHeadlineStr() const {return HeadlineStr;}
  void PutBylineStr(const TStr& _BylineStr){BylineStr=_BylineStr;}
  TStr GetBylineStr() const {return BylineStr;}
  void PutDatelineStr(const TStr& _DatelineStr){DatelineStr=_DatelineStr;}
  TStr GetDatelineStr() const {return DatelineStr;}

  // paragraphs/statements
  void AddParStr(const TStr& ParStr){ParStrV.Add(ParStr);}
  int GetPars() const {return ParStrV.Len();}
  TStr GetParStr(const int& ParN) const {return ParStrV[ParN];}
  TStr GetAllParStr() const;

  // topic codes
  void AddTopCdNm(const TStr& TopCdNm){TopCdNmV.Add(TopCdNm);}
  int GetTopCds() const {return TopCdNmV.Len();}
  TStr GetTopCdNm(const int& TopCdN) const {return TopCdNmV[TopCdN];}
  void GetTopCdNmV(TStrV& _TopCdNmV) const {_TopCdNmV=TopCdNmV;}

  // country codes
  void AddGeoCdNm(const TStr& GeoCdNm){GeoCdNmV.Add(GeoCdNm);}
  int GetGeoCds() const {return GeoCdNmV.Len();}
  TStr GetGeoCdNm(const int& GeoCdN) const {return GeoCdNmV[GeoCdN];}

  // industry codes
  void AddIndCdNm(const TStr& IndCdNm){IndCdNmV.Add(IndCdNm);}
  int GetIndCds() const {return IndCdNmV.Len();}
  TStr GetIndCdNm(const int& IndCdN) const {return IndCdNmV[IndCdN];}

  // binary-files traversal
  static PSIn FFirstCpd(const TStr& FNm);
  static PSIn FFirstCpd(const PSIn& SIn);
  static bool FNextCpd(const PSIn& SIn, PCpDoc& CpDoc);

  // file formats
  // ...html
  static PCpDoc LoadHtmlDoc(const TStr& FNm);
  static void SaveHtmlToCpd(
   const TStr& InHtmlFPath, const TStr& OutCpdFNm,
   const bool& RecurseDirP=false, const int& MxDocs=-1);

  // text-base
  static void SaveTBsToCpd(
   const TStr& InTBsFNm, const TStr& OutCpdFNm, const int& MxDocs=-1);

  // line-documents
  static void SaveLnDocToCpd(
   const TStr& InLnDocFNm, const TStr& OutCpdFNm, const bool& NamedP, const int& MxDocs=-1);

  // ...Reuters-21578
  static void LoadReuters21578DocFromXml(const TStr& FNm,
   TStr& DocNm, TStr& DateStr, TStr& TitleStr,
   TStr& HeadlineStr, TStr& BylineStr, TStr& DatelineStr,
   TStrV& ParStrV,
   TStrV& TopCdNmV, TStrV& GeoCdNmV, TStrV& IndCdNmV);
  static PCpDoc LoadReuters21578DocFromXml(const TStr& FNm);
  static const TStr Reuters21578CpdFBase;
  static void SaveReuters21578ToCpd(
   const TStr& InFPath, const TStr& OutFPath, const int& MxDocs=-1);

  // ...Reuters-2000
  static void LoadReuters2000DocFromXml(const TStr& FNm,
   TStr& DocNm, TStr& DateStr, TStr& TitleStr,
   TStr& HeadlineStr, TStr& BylineStr, TStr& DatelineStr,
   TStrV& ParStrV,
   TStrV& TopCdNmV, TStrV& GeoCdNmV, TStrV& IndCdNmV);
  static PCpDoc LoadReuters2000DocFromXml(const TStr& FNm);
  static const TStr Reuters2000CpdFBase;
  static void SaveReuters2000ToCpd(
   const TStr& InFPath, const TStr& OutFPath, const int& MxDocs=-1);

  // ...ACM-Tech-News
  static const TStr AcmTechNewsCpdFBase;
  static void SaveAcmTechNewsToCpd(
   const TStr& InFPath, const TStr& OutCpdFNm, const int& MxDocs=-1);

  // ...CIA-World-Factbook
  static const TStr CiaWFBCpdFBase;
  static void SaveCiaWFBToCpd(
   const TStr& InFPath, const TStr& OutCpdFNm, const int& MxDocs=-1);

  // ASFA
  static const TStr AsfaFBase;
  static void SaveAsfaToCpd(const TStr& InFPath, const TStr& OutCpdFNm);

  // ...NlpWin
  void SaveForNlpWin(const PSOut& SOut);
};

