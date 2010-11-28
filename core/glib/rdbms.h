#ifndef rdbms_h
#define rdbms_h

/////////////////////////////////////////////////
// Includes
#include "base.h"
#include "assert.h"

typedef char* Rdbms_TCStr;
typedef Rdbms_TCStr* Rdbms_TCStrPt;
typedef unsigned int RdbmsRef;
typedef RdbmsRef* RdbmsRefPt;

// Free a string allocated via StrNew or StrAlloc by DLL's memory manager.
// procedure ForgetString(var pc:PChar); cdecl; external DLL_NameC;
typedef void (*TRdbms_ForgetString)(Rdbms_TCStr CStr);

// Given a RDBMS reference to InField, the function allocates a PChar string/BLOB
// and reads into it the database BLOB field; returns the PChar string/BLOB
// (this must be deallocated by the calling program using ForgetString);
// after the call Size contains the PChar string/BLOB size in bytes
// (not including the Null terminator).
// function ReadBlobFromDatabaseField(const InField: RDBMSRef;
//  var Size: LongInt): PChar; cdecl; external DLL_NameC;
typedef Rdbms_TCStr (*TRdmbs_ReadBlobFromDatabaseField)(RdbmsRef InField, int Size);

// Given a RDBMS reference to OutField, a PChar string/BLOB and its Size,
// the function writes the PChar string/BLOB to the database BLOB field
// (not including the Null terminator).
// procedure WriteBlobToDatabaseField(const OutField: RDBMSRef;
//  const Value: PChar; const Size: LongInt); cdecl; external DLL_NameC;
typedef void (*TRdmbs_WriteBlobToDatabaseField)(RdbmsRef OutField, Rdbms_TCStr CStr, int Size);

// Moves the cursor of the corresponding table to the first record;
// IsEof is set to False (unless the table is empty).
// procedure GotoFirstRecord(const Field: RDBMSRef); cdecl; external DLL_NameC;
typedef void (*TRdmbs_GotoFirstRecord)(RdbmsRef Field);

// Moves the cursor of the corresponding table to the next record;
// if the cursor is already at the last position it remains unchanged and
// IsEof is set to True.
// procedure GotoNextRecord(const Field: RDBMSRef); cdecl; external DLL_NameC;
typedef void (*TRdmbs_GotoNextRecord)(RdbmsRef Field);

// Returns True if the cursor of the corresponding table was moved past
// the last record.
// function IsEof(const Field: RDBMSRef): Boolean; cdecl; external DLL_NameC;
typedef bool (*TRdmbs_IsEof)(RdbmsRef Field);

// Auxiliary DLL routines - only used for testing of TM with RDBMS

// Creates and opens a test database with the given name in given folder;
//     generates two sample tables and returns fife field references to be used for testing:
//     DocField, NameField and ClassField are part of the first table,
//     while TempField1, TempField2 and TempField3 are in the second table.
// Test documents (*.html) that are used to populate the first table are found in the folder called "TestDocs";
//     "TestDocs" folder MUST be within <DatabaseFolderPath>, i.e. <DatabaseFolderPath>\TestFolder;
//     "TestDocs" folder can have one level of subfolders corresponding to classes and their names.
// procedure CreateTestDatabase(
//  const DatabaseName, DatabaseFolderPath: PChar;
//  var DocField, NameField, ClassField,
//  TempField1, TempField2, TempField3: RDBMSRef); cdecl; external DLL_NameC;
typedef void (*TRdbms_CreateTestDatabase)(
 Rdbms_TCStr DatabaseName, Rdbms_TCStr DatabaseFolderPath,
 RdbmsRefPt DocField, RdbmsRefPt NameField, RdbmsRefPt ClassField,
 RdbmsRefPt TempField1, RdbmsRefPt TempField2, RdbmsRefPt TempField3);

// Closes the test database; deletes the database file if DeleteFile is True.
// procedure DestroyTestDatabase(const DeleteFile: Boolean); cdecl; external DLL_NameC;
typedef void (*TRdbms_DestroyTestDatabase)(bool DeleteFile);

/////////////////////////////////////////////////
// Rdbms
ClassTP(TRdbms, PRdbms)//{
private:
  HINSTANCE hRdbmsLib;
  TRdbms_ForgetString ForgetStringF;
  TRdmbs_ReadBlobFromDatabaseField ReadBlobFromDatabaseFieldF;
  TRdmbs_WriteBlobToDatabaseField WriteBlobToDatabaseFieldF;
  TRdmbs_GotoFirstRecord GotoFirstRecordF;
  TRdmbs_GotoNextRecord GotoNextRecordF;
  TRdmbs_IsEof IsEofF;
  TRdbms_CreateTestDatabase CreateTestDatabaseF;
  TRdbms_DestroyTestDatabase DestroyTestDatabaseF;
public:
  TRdbms(){assert(false);}
  TRdbms(const TRdbms&){assert(false);}
  TRdbms(char* FNm);
  static PRdbms New(char* FNm){
    return PRdbms(new TRdbms(FNm));}
  ~TRdbms();

  TRdbms& operator=(const TRdbms&){assert(false); return *this;}

  void ForgetString(Rdbms_TCStr CStr){
    (*ForgetStringF)(CStr);}
  Rdbms_TCStr ReadBlobFromDatabaseField(RdbmsRef InField, int Size){
    return (*ReadBlobFromDatabaseFieldF)(InField, Size);}
  void WriteBlobToDatabaseField(RdbmsRef OutField, Rdbms_TCStr CStr, int Size){
    (*WriteBlobToDatabaseFieldF)(OutField, CStr, Size);}
  void GotoFirstRecord(RdbmsRef Field){
    (*GotoFirstRecordF)(Field);}
  void GotoNextRecord(RdbmsRef Field){
    (*GotoNextRecordF)(Field);}
  bool IsEof(RdbmsRef Field){
    return (*IsEofF)(Field);}

  void CreateTestDatabase(
   Rdbms_TCStr DatabaseName, Rdbms_TCStr DatabaseFolderPath,
   RdbmsRefPt DocField, RdbmsRefPt NameField, RdbmsRefPt ClassField,
   RdbmsRefPt TempField1, RdbmsRefPt TempField2, RdbmsRefPt TempField3){
    (*CreateTestDatabaseF)(
     DatabaseName, DatabaseFolderPath,
     DocField, NameField, ClassField,
     TempField1, TempField2, TempField3);}
  void DestroyTestDatabase(bool DeleteFile){
    (*DestroyTestDatabaseF)(DeleteFile);}
};

/////////////////////////////////////////////////
// Input-Rdbms
class TRdbmsIn: public TSIn{
private:
  uint RdbmsRef;
  PSIn SIn;
  UndefDefaultCopyAssign(TRdbmsIn);
public:
  TRdbmsIn(const uint& _RdbmsRef):
    RdbmsRef(_RdbmsRef), SIn(){}
  static PSIn New(const uint& RdbmsRef){
    return PSIn(new TRdbmsIn(RdbmsRef));}
  ~TRdbmsIn(){}

  bool Eof() const {return SIn->Eof();}
  int Len() const {return SIn->Len();}
  char GetCh(){return SIn->GetCh();}
  char PeekCh(){return SIn->PeekCh();}
  int GetBf(const void* LBf, const int& LBfL){
    return SIn->GetBf(LBf, LBfL);}
};

/////////////////////////////////////////////////
// Output-Rdbms
class TRdbmsOut: public TSOut{
private:
  uint RdbmsRef;
  PSOut SOut;
  UndefDefaultCopyAssign(TRdbmsOut);
public:
  TRdbmsOut(const uint& _RdbmsRef):
    RdbmsRef(_RdbmsRef), SOut(){}
  static PSOut New(const int& RdbmsRef){
    return new TRdbmsOut(RdbmsRef);}
  ~TRdbmsOut(){}

  int PutCh(const char& Ch){
    return SOut->PutCh(Ch);}
  int PutBf(const void* LBf, const int& LBfL){
    return SOut->PutBf(LBf, LBfL);}
  void Flush(){
    SOut->Flush();}
};

#endif
