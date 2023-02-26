#pragma once

#include <string>
#include <vector>
#include <map>

#include <Windows.h>

#include <sql.h>
#include <sqltypes.h>
#include <sqlext.h>

#include "ODBCColumn.h"

namespace cyclOps {

	class ODBCResultSet
	{
	public:
		typedef std::vector<cyclOps::ODBCColumn> COLUMN_VECTOR;
		typedef std::vector<cyclOps::ODBCColumn>::const_iterator COLUMN_ITERATOR;
		typedef std::vector<std::map<std::string, std::string>> ROW_VECTOR;
		typedef std::vector<std::map<std::string, std::string>>::const_iterator ROW_ITERATOR;

		ODBCResultSet(void);
		~ODBCResultSet(void);
		void addColumn(const int& iColumn, const SQLCHAR* puchName, const SQLSMALLINT& typeOfColumn, const SQLULEN& sizeOfColumn);
		void addRow(const std::map<std::string, std::string>& row);
		const COLUMN_VECTOR& getColumns() const { return _vectorOfColumns; }
		const ROW_VECTOR& getRows() const { return _vectorOfRows; }
		ODBCColumn getColumn(int i) const { return _vectorOfColumns[i - 1]; }
		size_t getRowCount() const { return _vectorOfRows.size(); }
	private:
		COLUMN_VECTOR _vectorOfColumns;
		ROW_VECTOR _vectorOfRows;
	};
}