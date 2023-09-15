#include "StdAfx.h"

#include "..\cyclOps\cyclOps.h"

#include "ODBCColumn.h"

using std::string;

namespace cyclOps {
	ODBCColumn::ODBCColumn
		(const int& iColumn, const std::string& strName, 
		const SQLSMALLINT& typeOfColumn, const SQLULEN& sizeOfColumn)
		: _iColumnNumber(iColumn), _strColumnName(strName), _typeOfColumn(typeOfColumn),
		_sizeOfColumn(sizeOfColumn)

	{
	}


	ODBCColumn::~ODBCColumn(void)
	{
	}

	struct ODBCColumnType {
		SQLSMALLINT i; 
		const char* sz;
		ODBCColumn::HANDLE_AS handleAs;
	};


	ODBCColumnType ODBC_COLUMN_TYPES[] = {
		{SQL_UNKNOWN_TYPE, "SQL_UNKNOWN_TYPE", ODBCColumn::UNKNOWN},
		{SQL_CHAR, "SQL_CHAR", ODBCColumn::UNKNOWN},
		{SQL_NUMERIC, "SQL_NUMERIC", ODBCColumn::UNKNOWN},
		{SQL_DECIMAL, "SQL_DECIMAL", ODBCColumn::UNKNOWN},
		{SQL_INTEGER, "SQL_INTEGER", ODBCColumn::UNKNOWN},
		{SQL_SMALLINT, "SQL_SMALLINT", ODBCColumn::UNKNOWN},
		{SQL_FLOAT, "SQL_FLOAT", ODBCColumn::UNKNOWN},
		{SQL_REAL, "SQL_REAL", ODBCColumn::UNKNOWN},
		{SQL_DOUBLE, "SQL_DOUBLE", ODBCColumn::UNKNOWN},
		{SQL_DATETIME, "SQL_DATETIME", ODBCColumn::UNKNOWN},
		{SQL_VARCHAR, "SQL_VARCHAR", ODBCColumn::UNKNOWN},
		{SQL_TYPE_DATE, "SQL_TYPE_DATE", ODBCColumn::UNKNOWN},
		{SQL_TYPE_TIME, "SQL_TYPE_TIME", ODBCColumn::UNKNOWN},
		{SQL_TYPE_TIMESTAMP, "SQL_TYPE_TIMESTAMP", ODBCColumn::UNKNOWN},
		{SQL_WCHAR, "SQL_WCHAR", ODBCColumn::UNKNOWN},
		{SQL_WVARCHAR, "SQL_WVARCHAR", ODBCColumn::UNKNOWN},
		{SQL_WLONGVARCHAR, "SQL_WLONGVARCHAR", ODBCColumn::UNKNOWN},
		{SQL_C_WCHAR, "SQL_C_WCHAR", ODBCColumn::UNKNOWN},
		{SQL_C_TCHAR, "SQL_C_TCHAR", ODBCColumn::UNKNOWN}
	};

	string ODBCColumn::getTypeString() const {
		size_t sizeOfArray = sizeof(ODBC_COLUMN_TYPES) / sizeof(ODBC_COLUMN_TYPES[0]); CYCLOPSDEBUG("sizeOfArray  = %d", sizeOfArray );
		for (int i = 0; i < sizeOfArray; ++i) {
			if (this->_typeOfColumn == ODBC_COLUMN_TYPES[i].i) {
				return ODBC_COLUMN_TYPES[i].sz;
			}
		}
		CYCLOPS_THROW_EXCEPTION_IV(cyclOps::Exception, "The type %d was not found.", _typeOfColumn);
	}
}