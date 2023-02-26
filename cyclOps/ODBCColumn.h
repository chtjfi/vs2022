#pragma once

#include <string>

#include <Windows.h>

#include <sql.h>
#include <sqltypes.h>
#include <sqlext.h>

namespace cyclOps {

	class ODBCColumn
	{
	public:
		ODBCColumn
			(const int& iColumn, const std::string& strName, 
			const SQLSMALLINT& typeOfColumn, const SQLULEN& sizeOfColumn);
		~ODBCColumn(void);
		int getNumber() const { return _iColumnNumber; }
		std::string getName() const { return _strColumnName; }
		SQLSMALLINT getType() const { return _typeOfColumn; }
		std::string getTypeString() const;
		SQLULEN getSize() const { return _sizeOfColumn; }
		enum HANDLE_AS {
			WCHAR,
			INTEGER,
			DATETIME,
			UNKNOWN
		};
		
	private:
		int _iColumnNumber;
		std::string _strColumnName;
		SQLSMALLINT _typeOfColumn;
		SQLULEN _sizeOfColumn;
	};
}