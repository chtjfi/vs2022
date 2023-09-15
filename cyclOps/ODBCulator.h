#pragma once

#include <string>
#include <map>

#include <Windows.h>

#include <sql.h>
#include <sqltypes.h>
#include <sqlext.h>

#include "ODBCResultSet.h"

namespace cyclOps {
	class ODBCulator
	{
	public:
		explicit ODBCulator(const std::string& strDSN, const std::string& strUser, const std::string& strPassword);
		explicit ODBCulator() { };
		~ODBCulator(void);
		void getResultSet(const std::string& strStatement, ODBCResultSet& resultSet);
		void setDSN(const std::string& dsn) { 
			_strDSN = dsn; 
			this->memcpy(&_pchDSN, this->_strDSN); 
		}
		void setUser(const std::string& user) { 
			_strUser = user;
			this->memcpy(&_pchUser, this->_strUser); 
		}
		void setPassword(const std::string& pw) {
			_strPassword = pw;
			this->memcpy(&_pchPassword, this->_strPassword); 
		}

	private :
		ODBCulator(const ODBCulator& o);
		ODBCulator& operator = (ODBCulator o);
		std::string _strDSN;
		SQLCHAR* _pchDSN;
		std::string _strUser;
		SQLCHAR* _pchUser;
		std::string _strPassword;
		SQLCHAR* _pchPassword;
		void checkSQLRETURN(SQLRETURN reeturn, const std::string& msg, SQLSMALLINT handleType, SQLHANDLE handle, const std::string& statement);
		void newDSNUserAndPassword();
		void deleteDSNUserAndPassword();
		void memcpy(SQLCHAR** ppchTarget, const std::string& strSource);
		void status( SQLSMALLINT handleType, SQLHANDLE theHandle);
		void fillResultSet(SQLHANDLE hStmt, ODBCResultSet& resultSet);
		void setColumns(ODBCResultSet& resultSet, SQLHANDLE hStmt);
		void addColumnToRow(const SQLHANDLE& hStmt, const ODBCColumn& i, std::map<std::string, std::string>& row);
	};
}