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
ClassTP(TSQLParameter, PSQLParameter)
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
	TFlt GetFloat(int colIndex) const;
	TStr GetText(int colIndex) const;
	const void * GetBlob(int colIndex) const;
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


#endif
