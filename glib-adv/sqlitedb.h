#ifndef __SQLITEDB_H__
#define __SQLITEDB_H__

#include "base.h"

class TSQLParameter;
class TSQLCommand;

// sqlite connection object; can be reused between several TSQLiteCommand objects
ClassTP(TSQLConnection, PSQLConnection)
//{
private:
	void * pDb;
	TStr ConnStr;
public:
	TSQLConnection(const TStr & ConnStr);
	static PSQLConnection New(const TStr & ConnStr) { return new TSQLConnection(ConnStr); }
	~TSQLConnection();

	friend class TSQLCommand;
};

// abstract SQL parameter class
// parameters are used when creating SQL commands
ClassTPV(TSQLParameter, PSQLParameter, TSQLParameterV)
//{
public:
	TSQLParameter() {}	
	virtual ~TSQLParameter() { /*printf("TSQLParameter destructor\n");*/ }

	virtual void Bind(void * stmt, int index) const = 0;
};

// This class acts like a cursor
// it creates and executes an sql statement to which parameters can be bound
// if the execution returns data then this data can be iterated
ClassTP(TSQLCommand, PSQLCommand)
//{
private:
	const PSQLConnection Conn;
	void * stmt;
	const TStr & SqlStr;
	
public:
	TSQLCommand(const PSQLConnection, 
		const TStr & SqlStr, 
		const TVec<PSQLParameter> & ParamV = TVec<PSQLParameter>());
	static PSQLCommand New(const PSQLConnection Conn, const TStr & SqlStr, 
		const TVec<PSQLParameter> & ParamV = TVec<PSQLParameter>()) { 
			return new TSQLCommand(Conn, SqlStr, ParamV); }
	~TSQLCommand();
	
	// resets the SQL command to be executed again,
	// but does not clear the parameters
	void ResetCommand() const;
	void ClearParameters() const;
	void Bind(const TVec<PSQLParameter> & ParamV) const;	
	void Bind(const PSQLParameter & Param, int index) const;
	void ExecuteQuery() const;
	void ExecuteNonQuery() const;
	// functions to iterate and retrieve data
	bool ReadNext() const;
	int GetInt(int colIndex) const;
	uint64 GetUInt64(int colIndex) const;
	TFlt GetFloat(int colIndex) const;
	TStr GetText(int colIndex) const;
	PMem GetBlob(int colIndex) const;
	uint64 GetMemUsed() const;
	void OnDelFromCache(const TStr& Key, void *Container) {}
};

ClassTPE(TSQLIntParameter, PSQLIntParameter, TSQLParameter)
//{
private:
	int val;
public:
	TSQLIntParameter(int val);

	~TSQLIntParameter();

	void Bind(void * stmt, int index) const;
};

ClassTPE(TSQLUInt64Parameter, PSQLUInt64Parameter, TSQLParameter)
//{
private:
	uint64 val;
public:
	TSQLUInt64Parameter(uint64 val);

	~TSQLUInt64Parameter();

	void Bind(void * stmt, int index) const;
};

ClassTPE(TSQLFloatParameter, PSQLFloatParameter, TSQLParameter)
//{
private:
	TFlt val;
public:
	TSQLFloatParameter(TFlt val);
	~TSQLFloatParameter();

	void Bind(void * stmt, int index) const;
};

ClassTPE(TSQLStrParameter, PSQLStrParameter, TSQLParameter)
//{
private:
	TStr val;
public:
	TSQLStrParameter(TStr val);
	~TSQLStrParameter();

	void Bind(void * stmt, int index) const;
};

ClassTPE(TSQLBlobParameter, PSQLBlobParameter, TSQLParameter)
//{
private:
	PMem val;
public:
	TSQLBlobParameter(const PMem& val);
	~TSQLBlobParameter();

	void Bind(void * stmt, int index) const;
};



typedef TCache<TStr,PSQLCommand> TStrSQLCommandCache; 
/**
 * A query cache instance that re-uses SQLCommand objects for a given connection. 
 * Not thread safe.
 */
ClassTPE(TSQLQueryCache, PSQLQueryCache, TStrSQLCommandCache)// 
protected:
	PSQLConnection Conn;
public:
	TSQLQueryCache() {}
	TSQLQueryCache(PSQLConnection C) : TCache(1024 * 1024, 32, NULL), Conn(C) {
		PutRefToBs(this);
	}
	PSQLCommand GetQ(const TStr& Query, const TVec<PSQLParameter>& Params = TVec<PSQLParameter>()) {
		PSQLCommand Cmd;
		if (Get(Query, Cmd)) {
			// clean each command before use
			Cmd->ResetCommand();
		} else {
			Cmd = new TSQLCommand(Conn, Query);
			Put(Query, Cmd);
		}
		Cmd->Bind(Params);
		return Cmd;
	}
};



#endif
