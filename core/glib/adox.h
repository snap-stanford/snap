#ifndef adox_h
#define adox_h

/////////////////////////////////////////////////
// Includes
#include <ADODB.hpp>
#include "base.h"

/////////////////////////////////////////////////
// Ado-DataBase
ClassTP(TAdoDb, PAdoDb)//{
protected:
  static int LastDbId;
  TADOConnection* Connection;
  bool Ok;
  TStr ErrMsg;
  int GetNewDbId(){return ++LastDbId;}
public:
  TAdoDb();
  virtual ~TAdoDb();
  TAdoDb(TSIn&){Fail;}
  static PAdoDb Load(TSIn& SIn){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  void ConnDb(const TStr& ConnStr);

  bool IsOk() const {return Ok;}
  TStr GetErrMsg() const {return ErrMsg;}

  TADOConnection* GetConnection(){return Connection;}
  TStr GetNm() const {return Connection->Name.c_str();}
  void Open() const {Connection->Open();}
  void Close() const {Connection->Close();}
};

/////////////////////////////////////////////////
// SqlServer-Data-Base
class TSqlSrvAdoDb: public TAdoDb{
public:
  TSqlSrvAdoDb(): TAdoDb(){}
  static PAdoDb New(const TStr& AliasNm, const TStr& UsrNm, const TStr& PwdStr);
};

/////////////////////////////////////////////////
// Ado-Query
ClassTP(TAdoQuery, PAdoQuery)//{
private:
  TADOQuery* Query;
  bool Ok;
  TStr ErrMsg;
  int RecN;
public:
  TAdoQuery(const PAdoDb& AdoDb);
  static PAdoQuery New(const PAdoDb& AdoDb){
    return PAdoQuery(new TAdoQuery(AdoDb));}
  ~TAdoQuery();
  static PAdoQuery Load(TSIn& SIn){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TADOQuery* GetQuery(){return Query;}
  bool IsOk(){return Ok;}
  TStr GetErrMsg(){return ErrMsg;}

  void Execute(const TStr& QueryStr);
  void Select(const TStr& QueryStr);

  int GetRecs() const {return Query->RecordCount;}
  int GetRecN() const {return RecN;}
  void First(){Query->First(); RecN=0;}
  void Next(){Query->Next(); RecN++;}
  bool Eof(){return Query->Eof;}

  int GetFlds(){
    return Query->FieldCount;}
  TStr GetFldNm(const int& FldN){
    return Query->Fields->Fields[FldN]->FieldName.c_str();}

  TStr GetValStr(const int& FldN){
    return Query->Fields->Fields[FldN]->AsString.c_str();}
  TStr GetValStr(const TStr& FldNm){
    return Query->FieldByName(FldNm.CStr())->AsString.c_str();}
  bool GetValBool(const TStr& FldNm){
    return Query->FieldByName(FldNm.CStr())->AsInteger!=0;}
  int GetValInt(const int& FldN){
    return Query->Fields->Fields[FldN]->AsInteger;}
  int GetValInt(const TStr& FldNm){
    return Query->FieldByName(FldNm.CStr())->AsInteger;}
  double GetValFlt(const TStr& FldNm){
    return Query->FieldByName(FldNm.CStr())->AsFloat;}
};

/////////////////////////////////////////////////
// Ado-Table
ClassTP(TAdoTable, PAdoTable)//{
private:
  TADOTable* Table;
  int RecN;
public:
  TAdoTable(const PAdoDb& AdoDb, const TStr& TableNm);
  static PAdoTable New(const PAdoDb& AdoDb, const TStr& TableNm){
    return PAdoTable(new TAdoTable(AdoDb, TableNm));}
  ~TAdoTable();
  static PAdoTable Load(TSIn& SIn){Fail; return NULL;}
  void Save(TSOut&){Fail;}

  TADOTable* GetTable() const {return Table;}
  void Open() const {Table->Open();}
  void Close() const {Table->Close();}

  int GetRecs() const {return Table->RecordCount;}
  int GetRecN() const {return RecN;}
  void First(){Table->First(); RecN=0;}
  void Next(){Table->Next(); RecN++;}
  bool Eof() const {return Table->Eof;}
  void Insert() const {Table->Insert();}
  void Edit() const {Table->Edit();}
  void Post() const {Table->Post();}

  int GetFlds() const {
    return Table->FieldCount;}
  TStr GetFldNm(const int& FldN) const {
    return Table->Fields->Fields[FldN]->FieldName.c_str();}

  TStr GetValStr(const int& FldN) const {
    return Table->Fields->Fields[FldN]->AsString.c_str();}
  void PutValStr(const TStr& FldNm, const TStr& FldVal) const {
    Table->FieldByName(FldNm.CStr())->AsString=FldVal.CStr();}
  TStr GetValStr(const TStr& FldNm) const {
    return Table->FieldByName(FldNm.CStr())->AsString.c_str();}
  bool GetValBool(const TStr& FldNm) const {
    return Table->FieldByName(FldNm.CStr())->AsInteger!=0;}
  int GetValInt(const TStr& FldNm) const {
    return Table->FieldByName(FldNm.CStr())->AsInteger;}
  double GetValFlt(const TStr& FldNm) const {
    return Table->FieldByName(FldNm.CStr())->AsFloat;}
};

#endif
