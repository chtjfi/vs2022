#include "StdAfx.h"

#include <sstream>

#include "ODBCResultSet.h"

using std::stringstream;
using std::map;

namespace cyclOps {
	ODBCResultSet::ODBCResultSet(void)
	{
	}


	ODBCResultSet::~ODBCResultSet(void)
	{
	}

	void ODBCResultSet::addColumn(const int& iColumn, const SQLCHAR* puchName, 
		const SQLSMALLINT& typeOfColumn, const SQLULEN& sizeOfColumn) 
	{
		stringstream ss;
		ss << puchName;
		cyclOps::ODBCColumn column(iColumn, ss.str(), typeOfColumn, sizeOfColumn);
		_vectorOfColumns.push_back(column);
	}

	void ODBCResultSet::addRow(const map<std::string, std::string>& row) {
		_vectorOfRows.push_back(row);
	}
}