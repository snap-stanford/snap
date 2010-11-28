/////////////////////////////////////////////////
// Includes
#include "rdbms.h"

/////////////////////////////////////////////////
// Rdbms
TRdbms::TRdbms(char* FNm){
  printf("Load Library\n");
  hRdbmsLib=
   LoadLibrary(TEXT(FNm));
  assert(hRdbmsLib!=NULL);

  printf("Creating Links to DLL functions\n");
  ForgetStringF=
   (TRdbms_ForgetString)GetProcAddress(hRdbmsLib, TEXT("ForgetString"));
  assert(ForgetStringF!=NULL);

  ReadBlobFromDatabaseFieldF=
   (TRdmbs_ReadBlobFromDatabaseField)GetProcAddress(hRdbmsLib, TEXT("ReadBlobFromDatabaseField"));
  assert(ReadBlobFromDatabaseFieldF!=NULL);

  WriteBlobToDatabaseFieldF=
   (TRdmbs_WriteBlobToDatabaseField)GetProcAddress(hRdbmsLib, TEXT("WriteBlobToDatabaseField"));
  assert(WriteBlobToDatabaseFieldF!=NULL);

  GotoFirstRecordF=
   (TRdmbs_GotoFirstRecord)GetProcAddress(hRdbmsLib, TEXT("GotoFirstRecord"));
  assert(GotoFirstRecordF!=NULL);

  GotoNextRecordF=
   (TRdmbs_GotoNextRecord)GetProcAddress(hRdbmsLib, TEXT("GotoNextRecord"));
  assert(GotoNextRecordF!=NULL);

  IsEofF=
   (TRdmbs_IsEof)GetProcAddress(hRdbmsLib, TEXT("IsEof"));
  assert(IsEofF!=NULL);

  CreateTestDatabaseF=
   (TRdbms_CreateTestDatabase)GetProcAddress(hRdbmsLib, TEXT("CreateTestDatabase"));
  assert(CreateTestDatabaseF!=NULL);

  DestroyTestDatabaseF=
   (TRdbms_DestroyTestDatabase)GetProcAddress(hRdbmsLib, TEXT("DestroyTestDatabase"));
  assert(DestroyTestDatabaseF!=NULL);
}

TRdbms::~TRdbms(){
  printf("FreeLibrary\n");
  BOOL IsFreeLibOk=FreeLibrary(hRdbmsLib);
  assert(IsFreeLibOk);
}

