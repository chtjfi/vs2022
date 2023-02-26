#include "StdAfx.h"

#include "cyclOps.h"

#include "ODBCulator.h"

using std::string;
using std::map;

namespace cyclOps {

	void printColumns(const SQLHSTMT& hStmt) {
		SQLSMALLINT numCols ;
		SQLRETURN retCode = SQLNumResultCols( hStmt, &numCols ); 
		SQLCHAR colName[ 256 ] ;
		SQLSMALLINT colNameLen, dataType, numDecimalDigits, allowsNullValues ;
		SQLULEN columnSize ;
		for( int i = 1 ; i <= numCols ; i++ ) {
			retCode = SQLDescribeColA( hStmt, i, colName, 255, &colNameLen, &dataType, &columnSize, &numDecimalDigits, &allowsNullValues );
			printf( "Column #%d: '%s', datatype=%d size=%I64u decimaldigits=%d nullable=%d\n",
				i,colName,   dataType, columnSize,  numDecimalDigits, allowsNullValues ) ;
		}
	}


	ODBCulator::ODBCulator(const std::string& strDSN, const std::string& strUser, const std::string& strPassword)
			: _strDSN(strDSN), _strUser(strUser), _strPassword(strPassword), _pchDSN(0), _pchUser(0), _pchPassword(0)
		//, _hConn(0), _hStmt(0), _hEnv(0)
	{ 
		this->newDSNUserAndPassword();
	};

	ODBCulator::~ODBCulator(void) {
		this->deleteDSNUserAndPassword();
		//this->freeHandles(); 		CYCLOPSDEBUG("Goodbye.");
	}

		void freeSQLHandles(		SQLHANDLE hStmt, SQLHANDLE hConn,		SQLHANDLE hEnv ) { //CYCLOPSDEBUG("_hStmt, _hConn, _hEnv = %d, %d, %d", _hStmt, _hConn, _hEnv);
		if (hStmt) {
			SQLRETURN sqlreturn = SQLFreeHandle( SQL_HANDLE_STMT, hStmt ) ;  
			if (sqlreturn != SQL_SUCCESS) {
				CYCLOPSERROR("Error freeing statement handle.");
			}
		}
		if (hConn) {
			SQLDisconnect(hConn);
			SQLRETURN sqlreturn = SQLFreeHandle( SQL_HANDLE_DBC, hConn ) ;
			if (sqlreturn != SQL_SUCCESS) {
				CYCLOPSERROR("Error freeing connection handle.");
			}
		}
		if (hEnv) {
			SQLRETURN sqlreturn = SQLFreeHandle( SQL_HANDLE_ENV, hEnv ) ; CYCLOPSDEBUG("Goodbye.");
			if (sqlreturn != SQL_SUCCESS) {
				CYCLOPSERROR("Error freeing environment handle.");
			}
		}
	}



	/* This comes from http://bobobobo.wordpress.com/2009/07/11/working-with-odbc-from-c/ which you can also find in LearnyDooey. */
	void ODBCulator::getResultSet(const std::string& strStatement, ODBCResultSet& resultSet) {
		SQLHANDLE hStmt = NULL;
		SQLHANDLE hConn = NULL;
		SQLHANDLE hEnv = NULL;
		SQLRETURN retCode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
		this->checkSQLRETURN(retCode, "SQLAllocHandle() failed.", SQL_HANDLE_ENV, hEnv, strStatement);
		retCode = SQLSetEnvAttr( hEnv, SQL_ATTR_ODBC_VERSION, (void*) SQL_OV_ODBC3, 0 ) ; 
		if ( ! SQL_SUCCEEDED(retCode)) {
			throw std::runtime_error("Error calling SQLSetEnvAttr().");
		}
		retCode = SQLAllocHandle( SQL_HANDLE_DBC, hEnv, &hConn ); CYCLOPSDEBUG("_pchUser/_pchPassword = %s/%s", _pchUser, _pchPassword);
		retCode = SQLConnectA(hConn, _pchDSN, SQL_NTS, _pchUser, SQL_NTS, _pchPassword, SQL_NTS);
		this->checkSQLRETURN(retCode, "SQLConnectA() failed.", SQL_HANDLE_DBC, hConn, strStatement);
		retCode = SQLAllocHandle(SQL_HANDLE_STMT, hConn, &hStmt);
		this->checkSQLRETURN(retCode, "SQLAllocHandle() failed.", SQL_HANDLE_STMT, hStmt, strStatement); CYCLOPSDEBUG("Checkpoint.");
		SQLCHAR* pchStatement = 0; this->memcpy(&pchStatement, strStatement); CYCLOPSDEBUG("pchStatement = %s", pchStatement);
		retCode = SQLExecDirectA(hStmt, pchStatement, SQL_NTS ); 
		this->checkSQLRETURN(retCode, "SQLExecDirectA() failed.", SQL_HANDLE_STMT, hStmt, strStatement);
		delete[] pchStatement; CYCLOPSDEBUG("pchStatement deleted.");
		this->fillResultSet(hStmt, resultSet); CYCLOPSDEBUG("fillResultSet() completed.");
		cyclOps::freeSQLHandles(hStmt, hConn, hEnv); CYCLOPSDEBUG("freeHandles() completed.");
	}

	void ODBCulator::setColumns(ODBCResultSet& resultSet, SQLHANDLE hStmt) {
		SQLSMALLINT numCols ;
		SQLRETURN retCode = SQLNumResultCols( hStmt, &numCols ); 
		SQLCHAR colName[1000] ;
		SQLSMALLINT colNameLen, dataType, numDecimalDigits, allowsNullValues ;
		SQLULEN columnSize ; SQL_WCHAR                   ;
		for( int i = 1 ; i <= numCols ; i++ ) { SQL_BIT;
			retCode = SQLDescribeColA( hStmt, i, colName, 
				(sizeof(colName) / sizeof(colName[0])) - 1,
				&colNameLen, &dataType, &columnSize, 
				&numDecimalDigits, &allowsNullValues );
			resultSet.addColumn( i, colName, dataType, columnSize );
		}
	}

	void ODBCulator::fillResultSet(SQLHANDLE hStmt, ODBCResultSet& resultSet) {
		// int count = 0;
		cyclOps::printColumns( hStmt );
		this->setColumns ( resultSet, hStmt );
		while (SQL_SUCCEEDED(SQLFetch(hStmt))) {	
			// ++count;
			map<string, string> row;
			for (int i = 1; i <= resultSet.getColumns().size(); ++i) { CYCLOPSDEBUG("i = %d", i);
				this->addColumnToRow(hStmt, resultSet.getColumn(i), row); 
			}
			resultSet.addRow(row);
		}
	}

	void ODBCulator::addColumnToRow(const SQLHANDLE& hStmt, const ODBCColumn& col, map<string, string>& row) {
		SQLULEN sizeOfColumn = col.getSize(); CYCLOPSDEBUG("%d", sizeOfColumn);
		char* pchBuffer = new char[sizeOfColumn + 1];
		SQLLEN numBytes ;
		SQLRETURN retCode = SQLGetData(
			hStmt,
			col.getNumber(),           // COLUMN NUMBER of the data to get
			SQL_C_CHAR,  // the data type that you expect to receive
			pchBuffer,         // the place to put the data that you expect to receive
			sizeOfColumn,         // the size in bytes of buf (-1 for null terminator)
			&numBytes    // size in bytes of data returned
		);
		if (numBytes == SQL_NULL_DATA) {
			row[col.getName()] = "NULL";
		} else {
			row[col.getName()] = pchBuffer; 
		}
		delete[] pchBuffer;  CYCLOPSDEBUG("map[x] = %s", row[col.getName()].c_str());
	}


	void ODBCulator::checkSQLRETURN(SQLRETURN reeturn, const string& msg, SQLSMALLINT handleType, SQLHANDLE handle, const string& statement) {
		if ( ! SQL_SUCCEEDED(reeturn) && reeturn != SQL_NO_DATA) {
			this->status(handleType, handle);
			std::stringstream ss; ss << msg << " [" << statement << "]";
			throw std::runtime_error(ss.str().c_str());
		}
	}

	void ODBCulator::status( SQLSMALLINT handleType, SQLHANDLE theHandle) { CYCLOPSDEBUG("Hello.");
		SQLCHAR sqlState[6];
		SQLINTEGER nativeError;
		SQLCHAR msgStr[5000];
		SQLSMALLINT overBy ; 
		SQLRETURN retCode ;
		for( int i = 1 ; i < 20 ; i++ ) { CYCLOPSDEBUG("i = %d", i);
			retCode = SQLGetDiagRecA(
				handleType, 
				theHandle, 
				i, 
				sqlState, 
				&nativeError, 
				msgStr,      
				(sizeof(msgStr) / sizeof(msgStr[0])) - 1, 
				&overBy) ;
			if( SQL_SUCCEEDED( retCode ) ) {
				printf( "%d: %s - %d %s\n", i, sqlState, nativeError, msgStr ) ;
			} else { CYCLOPSDEBUG("SQLGetDiagRecA() failed.");
		      break ;
			}
		}
	}

	void ODBCulator::memcpy(SQLCHAR** ppchTarget, const string& strSource) {
		size_t sizeOfThing = strSource.length();
		*ppchTarget = new SQLCHAR[sizeOfThing + 1];
		::memcpy(*ppchTarget, strSource.c_str(), sizeOfThing + 1); CYCLOPSDEBUG("ppchTarget = %s", *ppchTarget);
	}

	void ODBCulator::newDSNUserAndPassword() {
		this->memcpy(&_pchDSN, this->_strDSN); CYCLOPSDEBUG("_pchDSN = %s", _pchDSN);
		this->memcpy(&_pchUser, this->_strUser); CYCLOPSDEBUG("_pchUser = %s", _pchUser);
		this->memcpy(&_pchPassword, this->_strPassword); CYCLOPSDEBUG("_pchPassword = %s", _pchPassword);
	}

	void ODBCulator::deleteDSNUserAndPassword() { CYCLOPSDEBUG("Hello.");
		if (_pchDSN) delete[] _pchDSN;
		if (_pchUser) delete[] _pchUser;
		if (_pchPassword) delete[] _pchPassword;
	}

}