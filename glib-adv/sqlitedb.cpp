#include "sqlitedb.h"
#include "sqlite3.h"

TSQLConnection::TSQLConnection(const TStr &ConnStr):
ConnStr(ConnStr)
{
	sqlite3 * pTmpDb = NULL;
	if(sqlite3_open(ConnStr.CStr(), &pTmpDb) != SQLITE_OK)
	{
		TExcept::Throw("Error opening database connection");
	}	
	pDb = pTmpDb;
}

TSQLConnection::~TSQLConnection()
{	
	sqlite3_close((sqlite3 *)pDb);
}


TSQLCommand::TSQLCommand(const PSQLConnection Conn, const TStr &SqlStr, const TVec<PSQLParameter> &ParamV):
Conn(Conn),
stmt(NULL),
SqlStr(SqlStr)
{
	sqlite3_stmt * pTmpStmt = NULL;
	if(sqlite3_prepare_v2((sqlite3 *)Conn->pDb, SqlStr.CStr(), -1, &pTmpStmt, NULL) != SQLITE_OK)
	{
		TExcept::Throw("Error preparing SQL statement");
	}
	stmt = pTmpStmt;
	Bind(ParamV);
}

TSQLCommand::~TSQLCommand()
{
	if(sqlite3_finalize((sqlite3_stmt *)stmt) != SQLITE_OK)
	{
		TExcept::Throw("Error finalizing SQLITE statement");
	}
}

void TSQLCommand::Bind(const PSQLParameter &Param, int index) const
{
	// the sqlite engine takes query parameters with index base 1
	Param->Bind(stmt, index + 1);
}

void TSQLCommand::Bind(const TVec<PSQLParameter> &ParamV) const
{
	for(int i = 0; i < ParamV.Len(); i++)
	{
		Bind(ParamV[i], i);
	}
}

void TSQLCommand::ResetCommand() const
{
	if(sqlite3_reset((sqlite3_stmt *)stmt) != SQLITE_OK)
	{
		TExcept::Throw("Error reseting SQL command");
	}
}

void TSQLCommand::ClearParameters() const
{
	if(sqlite3_clear_bindings((sqlite3_stmt *)stmt) != SQLITE_OK)
	{
		TExcept::Throw("Error clearing SQL parameters");
	}
}

void TSQLCommand::ExecuteNonQuery() const
{
	int ret = sqlite3_step((sqlite3_stmt *)stmt);
	if(ret != SQLITE_DONE)
	{
		/* see http://www.sqlite.org/c3ref/c_abort.html */
		TExcept::Throw(TStr::Fmt("Error executing SQL nonquery: %d", ret));
	}
}

void TSQLCommand::ExecuteQuery() const
{
	// empty for now, some bookkeeping code might be here in the future
}

bool TSQLCommand::ReadNext() const
{
	int ret = sqlite3_step((sqlite3_stmt *)stmt);
	return (ret == SQLITE_ROW);
}

int TSQLCommand::GetInt(int colIndex) const
{
	return sqlite3_column_int((sqlite3_stmt *)stmt, colIndex);
}


uint64 TSQLCommand::GetUInt64(int colIndex) const
{
	return sqlite3_column_int64((sqlite3_stmt *)stmt, colIndex);
}


TFlt TSQLCommand::GetFloat(int colIndex) const
{
	return sqlite3_column_double((sqlite3_stmt *)stmt, colIndex);
}

TStr TSQLCommand::GetText(int colIndex) const
{
	return TStr((const char *)sqlite3_column_text((sqlite3_stmt *)stmt, colIndex));
}

PMem TSQLCommand::GetBlob(int colIndex) const
{
	int Len = sqlite3_column_bytes((sqlite3_stmt *)stmt, colIndex);
	const void *Data = sqlite3_column_blob((sqlite3_stmt *)stmt, colIndex);
	return TMem::New(Data, Len);
}

uint64 TSQLCommand::GetMemUsed() const {
	return uint64(sizeof(void *)) + uint64(SqlStr.GetMemUsed());
}

TSQLIntParameter::TSQLIntParameter(int val):
TSQLParameter(),
val(val)
{
	
}

TSQLIntParameter::~TSQLIntParameter()
{
	//printf("TSQLIntParameter destructor\n");
}

void TSQLIntParameter::Bind(void * stmt, int index) const
{	
	if(sqlite3_bind_int((sqlite3_stmt *)stmt, index, val) != SQLITE_OK)
	{
		TExcept::Throw("Error binding SQL Parameter");
	}
}

TSQLUInt64Parameter::TSQLUInt64Parameter(uint64 v):
TSQLParameter(),
val(v)
{
	
}

TSQLUInt64Parameter::~TSQLUInt64Parameter()
{
	//printf("TSQLIntParameter destructor\n");
}

void TSQLUInt64Parameter::Bind(void * stmt, int index) const
{	
	if(sqlite3_bind_int64((sqlite3_stmt *)stmt, index, val) != SQLITE_OK)
	{
		TExcept::Throw("Error binding SQL Parameter");
	}
}


TSQLFloatParameter::TSQLFloatParameter(TFlt val):
TSQLParameter(),
val(val)
{
}

TSQLFloatParameter::~TSQLFloatParameter()
{
	//printf("TSQLFloatParameter destructor\n");
}

void TSQLFloatParameter::Bind(void *stmt, int index) const
{	
	if(sqlite3_bind_double((sqlite3_stmt *)stmt, index, val.Val) != SQLITE_OK)
	{
		TExcept::Throw("Error binding SQL Parameter");
	}
}

TSQLStrParameter::TSQLStrParameter(TStr val):
TSQLParameter(),
val(val)
{
}

TSQLStrParameter::~TSQLStrParameter()
{
	//printf("TSQLStrParameter destructor\n");
}

void TSQLStrParameter::Bind(void *stmt, int index) const
{	
	if(sqlite3_bind_text((sqlite3_stmt *)stmt, index, val.CStr(), -1, SQLITE_TRANSIENT) != SQLITE_OK)
	{
		TExcept::Throw("Error binding SQL Parameter");
	}
}

TSQLBlobParameter::TSQLBlobParameter(const PMem& val):
TSQLParameter(),
val(val)
{
}

TSQLBlobParameter::~TSQLBlobParameter()
{
	//printf("TSQLStrParameter destructor\n");
}

void TSQLBlobParameter::Bind(void *stmt, int index) const
{	
	if(sqlite3_bind_blob((sqlite3_stmt *)stmt, index, val->GetBf(), val->Len(), SQLITE_TRANSIENT) != SQLITE_OK)
	{
		TExcept::Throw("Error binding SQL Parameter");
	}
}
