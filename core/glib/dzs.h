#ifndef dzs_h
#define dzs_h

/////////////////////////////////////////////////
// Includes
#include "book.h"

/////////////////////////////////////////////////
// Dzs-Base-Document
class TDzsBsDoc;
typedef TPt<TDzsBsDoc> PDzsBsDoc;
typedef TVec<PDzsBsDoc> TDzsBsDocV;

class TDzsBsDoc{
private:
  TCRef CRef;
private:
  TStr IdStr;
  TStr TitleStr;
  TStr DataStr;
  TInt YearN;
public:
  TDzsBsDoc(): IdStr(), TitleStr(), DataStr(), YearN(){}
  TDzsBsDoc(const TStr _IdStr, const TStr& _TitleStr,
   const TStr& _DataStr, const int& _YearN):
    IdStr(_IdStr), TitleStr(_TitleStr), DataStr(_DataStr), YearN(_YearN){}
  static PDzsBsDoc New(const TStr& IdStr, const TStr& TitleStr,
   const TStr& DataStr, const int& YearN){
    return PDzsBsDoc(new TDzsBsDoc(IdStr, TitleStr, DataStr, YearN));}
  static PDzsBsDoc Load(TSIn& SIn){return new TDzsBsDoc(SIn);}
  TDzsBsDoc(TSIn& SIn):
    IdStr(SIn), TitleStr(SIn), DataStr(SIn), YearN(SIn){}
  void Save(TSOut& SOut){
    IdStr.Save(SOut); TitleStr.Save(SOut);
    DataStr.Save(SOut); YearN.Save(SOut);}

  TDzsBsDoc& operator=(const TDzsBsDoc& Doc){
    IdStr=Doc.IdStr; TitleStr=Doc.TitleStr;
    DataStr=Doc.DataStr; YearN=Doc.YearN; return *this;}

  // component retrieval
  TStr GetIdStr() const {return IdStr;}
  TStr GetTitleStr() const {return TitleStr;}
  TStr GetDataStr() const {return DataStr;}
  int GetYearN() const {return YearN;}

  // extracting data from xml document
  static void AddDataTokToChA(const PXmlTok& Tok, TChA& ChA);
  static TStr GetDataTokStr(const PXmlTok& Tok);
  static TStr GetDataTokVStr(const TXmlTokV& TokV, const TStr& SepStr);
  static void GetDocParts(
   const TStr& FNm, const PXmlDoc& XmlDoc,
   const TStr& FPath, const TStr& WebAlias,
   bool& Ok, TStr& IdStr, TStr& TitleStr, TStr& DataStr, int& YearN);
  static PDzsBsDoc GetDzsBsDoc(
   const TStr& FNm, const PXmlDoc& XmlDoc,
   const TStr& FPath, const TStr& WebAlias);

  // extracting data from html document
  static void GetDocParts(
   const TStr& FNm, const PHtmlDoc& HtmlDoc,
   const TStr& FPath, const TStr& WebAlias,
   bool& Ok, TStr& IdStr, TStr& TitleStr, TStr& DataStr, int& YearN);
  static PDzsBsDoc GetDzsBsDoc(
   const TStr& FNm, const PHtmlDoc& HtmlDoc,
   const TStr& FPath, const TStr& WebAlias);

  friend PDzsBsDoc;
};

/////////////////////////////////////////////////
// Dzs-Base
class TDzsBs: public TBook{
private:
  static bool IsReg;
  static bool MkReg(){
    return TBook::Reg(TTypeNm<TDzsBs>(), TBookLoadF(&Load));}
private:
  TDzsBsDocV DzsBsDocV;
  PBix Bix;
  void AddDoc(const PDzsBsDoc& DzsBsDoc);
public:
  TDzsBs(const TStr& FPath, const TStr& WebAlias);
  static PBook New(const TStr& FPath, const TStr& WebAlias){
    return PBook(new TDzsBs(FPath, WebAlias));}
  TDzsBs(TSIn& SIn):
    TBook(SIn), DzsBsDocV(SIn), Bix(SIn){}
  static PBook Load(TSIn& SIn){return PBook(new TDzsBs(SIn));}
  void Save(TSOut& SOut){
    TBook::Save(SOut); DzsBsDocV.Save(SOut); Bix.Save(SOut);}

  TDzsBs& operator=(const TDzsBs&){Fail; return *this;}

  PBookToc GetBookToc() const {Fail; return NULL;}
  int GetSecs() const {return DzsBsDocV.Len();}
  void GetSecInfo(
   const int& SecId, TStr& SecIdStr, TStr& TitleStr, TStr& SecStr) const;
  PBixRes Search(const TStr& QueryStr) const {
    return Bix->Search(QueryStr);}
};

/////////////////////////////////////////////////
// Dzs-Base ActiveX-Interface
class TDzsBsX;
typedef TPt<TDzsBsX> PDzsBsX;

class TDzsBsX{
private:
  TCRef CRef;
private:
  PBook Book;
  PBixRes BixRes;
public:
  TDzsBsX(): Book(), BixRes(){}
  static PDzsBsX New(){return PDzsBsX(new TDzsBsX());}
  ~TDzsBsX(){}
  TDzsBsX(TSIn&){Fail;}
  static PDzsBsX Load(TSIn& SIn){return PDzsBsX(new TDzsBsX(SIn));}
  void Save(TSOut&){Fail;}

  TDzsBsX& operator=(const TDzsBsX&){Fail; return *this;}

  bool LoadBook(const TStr& FNm);
  void UnloadBook();
  int GetBookSecs();
  bool GetBookSecInfo(
   const int& SecId, TStr& SecIdStr, TStr& TitleStr, TStr& SecStr);
  bool SearchBook(const TStr& QueryStr);

  bool IsResultOk();
  bool GetResultMsgStr(TStr& MsgStr);
  bool GetResultQueryStr(TStr& QueryStr);
  int GetResultHits();
  bool GetResultHitInfo(
   const int& HitN, const int& MxTitleLen, const int& MxCtxLen,
   TStr& SecIdStr, TStr& TitleStr, TStr& SecStr, TStr& CtxStr);

  friend PDzsBsX;
};

/////////////////////////////////////////////////
// Dzs-Keyword-Base
class TDzsKwBs;
typedef TPt<TDzsKwBs> PDzsKwBs;

class TDzsKwBs{
private:
  TCRef CRef;
private:
  TStrStrVH KwToSecIdVH;
public:
  TDzsKwBs(): KwToSecIdVH(){}
  TDzsKwBs(const TStr& FNm);
  static PDzsKwBs New(const TStr& FNm){return PDzsKwBs(new TDzsKwBs(FNm));}
  ~TDzsKwBs(){}
  TDzsKwBs(TSIn&){Fail;}
  static PDzsKwBs Load(TSIn& SIn){return PDzsKwBs(new TDzsKwBs(SIn));}
  void Save(TSOut&){Fail;}

  TDzsKwBs& operator=(const TDzsKwBs&){Fail; return *this;}

  int GetKws() const {return KwToSecIdVH.Len();}
  TStr GetKwStr(const int& KwN) const {return KwToSecIdVH.GetKey(KwN);}
  void AddHits(const PBook& Book, const TStr& FNm);


  friend PDzsKwBs;
};

/////////////////////////////////////////////////
// Dzs-Hierarchical-Keyword
class TDzsHKw;
typedef TPt<TDzsHKw> PDzsHKw;
typedef TVec<PDzsHKw> TDzsHKwV;

class TDzsHKw{
private:
  TCRef CRef;
private:
  TStr KwNm;
  int Lev;
  TStrV QueryStrV;
  TStrV SecIdStrV;
public:
  TDzsHKw(): KwNm(), Lev(), QueryStrV(), SecIdStrV(){}
  TDzsHKw(const TStr& KwNm, const int& Lev, const TStrV& QueryStrV);
  static PDzsHKw New(
   const TStr& KwNm, const int& Lev, const TStrV& QueryStrV=TStrV()){
    return PDzsHKw(new TDzsHKw(KwNm, Lev, QueryStrV));}

  ~TDzsHKw(){}
  TDzsHKw(TSIn&){Fail;}
  static PDzsHKw Load(TSIn& SIn){return PDzsHKw(new TDzsHKw(SIn));}
  void Save(TSOut&){Fail;}

  TDzsHKw& operator=(const TDzsHKw&){Fail; return *this;}

  TStr GetKwNm() const {return KwNm;}
  int GetLev() const {return Lev;}
  int GetQueryStrs() const {return QueryStrV.Len();}
  TStr GetQueryStr(const int& QueryStrN) const {return QueryStrV[QueryStrN];}

  void AddSecIdStr(const TStr& SecIdStr){
    SecIdStrV.AddUnique(SecIdStr);}

  friend PDzsHKw;
};

/////////////////////////////////////////////////
// Dzs-Hierarchical-Keyword-Base
class TDzsHKwBs;
typedef TPt<TDzsHKwBs> PDzsHKwBs;

class TDzsHKwBs{
private:
  TCRef CRef;
private:
  PXmlDoc XmlDoc;
public:
  TDzsHKwBs(const PXmlDoc& _XmlDoc):
    XmlDoc(_XmlDoc){EAssert(IsOk(), "Invalid Xml File.");}
  static PDzsHKwBs New(const PXmlDoc& XmlDoc){
    return PDzsHKwBs(new TDzsHKwBs(XmlDoc));}
  ~TDzsHKwBs(){}
  TDzsHKwBs(TSIn&){Fail;}
  static PDzsHKwBs Load(TSIn& SIn){return PDzsHKwBs(new TDzsHKwBs(SIn));}
  void Save(TSOut&){Fail;}

  TDzsHKwBs& operator=(const TDzsHKwBs&){Fail; return *this;}

  // checking
  static bool IsTopicOk(const PXmlTok& TopicTok);
  bool IsOk() const {
    return IsTopicOk(XmlDoc->GetTok());}

  // adding hits
  static void AddHits(const PXmlTok& TopicTok, const PBook& Book);
  void AddHits(const PBook& Book){
    AddHits(XmlDoc->GetTok(), Book);}

  // string conversion
  static TStr GetQueryStr(const TStr& TxtStr);
  static TStr GetFPathSegStr(const TStr& TxtStr);

  // loading & saving
  static PDzsHKwBs LoadTxt(const TStr& FNm);
  static PDzsHKwBs LoadXml(const TStr& FNm){
    return New(TXmlDoc::LoadTxt(FNm));}
  void SaveXml(const TStr& FNm) const {
    XmlDoc->SaveTxt(FNm);}

  static void SaveHtml(const PXmlTok& TopicTok, const TStr& FPath);
  void SaveHtml(const TStr& FPath){
    SaveHtml(XmlDoc->GetTok(), FPath);}

  friend PDzsHKwBs;
};

#endif

