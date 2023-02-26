#include "stdafx.h"

#include <string>
#include <vector>
#include <map>
#include <exception>
#include <typeinfo>
#include <iostream>

#include <boost/regex.hpp>

#include "..\cyclOps\cyclOps.h"
#include "..\whoOps\whoOpsBonanza.h"

using std::string;
using std::wstring;
using std::stringstream;
using std::vector;
using std::map;

using cyclOps::CommandLinebacker;
using cyclOps::StringEmUp;
using cyclOps::XMLigator;

namespace spacename {

	cyclOps::XMLigator _xmlConfig; 
	cyclOps::Logger _logger; 
	char* OPTION_FORMAT = "format";
	char* OPTION_FIELD = "field";
	char* OPTION_PATTERN = "pattern";
	char* OPTION_LIST_FIELDS = "listFields";
	char* OPTION_HIDE_UNUSED_FIELDS = "hideUnusedFields";
	char* OPTION_TRIM = "trim";

	void setPlusFields(cyclOps::Strung& strung, const vector<string>& row, int field) {
		string plusFieldName = cyclOps::StringEmUp::format("%d+", field + 1); CYCLOPSDEBUG("plusFieldName = %s", plusFieldName.c_str());
		stringstream value;
		for (int i = field; i < row.size(); ++i) {
			value << row[i] << " ";
		}
		string strValue = value.str();
		StringEmUp::trim(strValue);
		strung.set(plusFieldName, strValue);
	}

	void output(const vector<string>& row, const string& format, const string& line, int lineNumber,
		boolean hideUnusedFields, boolean trim) 
	{ CYCLOPSDEBUG("format = %s", format.c_str());
		cyclOps::Strung strung;
		int rowSize = row.size(); CYCLOPSDEBUG("rowSize = %d", rowSize);
		for (int j = 0; j < rowSize; ++j) { CYCLOPSDEBUG("j = %d", j);
			string field = cyclOps::StringEmUp::format("%d", j + 1); CYCLOPSDEBUG("field = %s", field.c_str());
			string value = row[j]; CYCLOPSDEBUG("value = %s", value.c_str());
			if (trim) {
				StringEmUp::trim(value);
			}
			strung.set(field, value);
			spacename::setPlusFields(strung, row, j);
		}  CYCLOPSDEBUG("%s", strung.getMapAsString().c_str()); 
		if (rowSize > 0) {
			strung.set("last", row[rowSize - 1]); CYCLOPSDEBUG("Hello");
		}
		strung.set("line", line); CYCLOPSDEBUG("Hello");
		string bgc = lineNumber % 2 == 0 ? "lightgrey" : "white"; CYCLOPSDEBUG("bgc = %s", bgc.c_str());
		strung.set("bgc", bgc);
		stringstream output_with_possible_unreplace_vars; output_with_possible_unreplace_vars << strung.format(format).c_str();
		string output;
		if (hideUnusedFields) { CYCLOPSDEBUG("Hiding unused fields.");
			cyclOps::RegExtravaganza::replace(output_with_possible_unreplace_vars.str(),
				"\\$\\{[0-9]+\\}", "", output, cyclOps::RegExtravaganza::IgnoreCase::True);
		} else { CYCLOPSDEBUG("Not hiding unused fields.");
			output.assign(output_with_possible_unreplace_vars.str());
		}
		printf("%s\n", output.c_str());
	}

	void outputField(const vector<vector<string>>& table, int field) {
		for (int i = 0; i < table.size(); ++i) { CYCLOPSDEBUG("row: %d", i);
			vector<string> row = table[i]; CYCLOPSDEBUG("size: %d", row.size());
			if (row.size() >= field) {
				printf("%s\n", row[field - 1].c_str());
			} else {
				printf("\n");
			}
		}
	}

	bool shouldWeListFields(const CommandLinebacker& cmd) {
		if (cmd.isOptionPresent("listFields")) {
			return true;
		} else {
			if ( ! (cmd.isOptionPresent(OPTION_FORMAT) || cmd.isOptionPresent(OPTION_FIELD))) {
				return true;
			}
		}
		return false;
	}

	void main(const CommandLinebacker& cmd) {
		/*********************************************/
		/* !!! USE _logger TO LOG EVERYTHING NOW !!! */
		/*********************************************/
		vector<vector<string>> table;
		/* Default tokenates on all whitespace. */
		string pattern = cmd.getOption(OPTION_PATTERN, "[\\s]+");
		std::string line;
        bool isFormatSpecified = cmd.isOptionPresent(OPTION_FORMAT);
		int lineNumber = 1;
		while (std::getline(std::cin, line)) {
		  cyclOps::StringEmUp::trim(line);
		  vector<string> tokens;
		  cyclOps::StringEmUp::tokenize(line, pattern, tokens);
		  boolean shouldWeListFields = spacename::shouldWeListFields(cmd);
		  if (shouldWeListFields) {
			  printf("\nLINE %d: %s\n", lineNumber, line.c_str());
			  for (int k = 0; k < tokens.size(); ++k) {
				string content = tokens[k];
				printf("%d: %s\n", k + 1, content.c_str());
			  }
		  }
		  table.push_back(tokens);
		  if (isFormatSpecified) {
			  string format = cmd.getOption(OPTION_FORMAT); 
			  boolean hideUnusedFields = cmd.isOptionPresent(OPTION_HIDE_UNUSED_FIELDS); /* CYCLOPSDEBUG("%d", hideUnusedFields);*/
			  boolean trim = cmd.isOptionPresent(OPTION_TRIM);
			  spacename::output(tokens, format, line, lineNumber, hideUnusedFields, trim);
		  } 
		  ++lineNumber;
		}	
		if (cmd.isOptionPresent("field")) {
			int field = cmd.getInt("field");
			spacename::outputField(table, field);
		}
	}

}

int main(int argc, char* argv[])
{
	try {

		/*********************************************/
		/* !!! USE _logger TO LOG EVERYTHING NOW !!! */
		/*********************************************/
		
		CommandLinebacker cmd(argc, argv);
		g_boCyclOpsDebug = cmd.isOptionPresent("debug"); 
		// cmd.initializeConfigFile(spacename::_xmlConfig, true); /* Don't use exe name as default. */
		// cmd.initializeLogFile(spacename::_logger);
		spacename::main(cmd); CYCLOPSDEBUG("Command completed successfully.");
		return 0;
	} catch (const cyclOps::Exception& e) {
		CYCLOPSERROR_II(spacename::_logger, "%s - %s - %s\n", typeid(e).name(), e.what(), e.formatMessage().c_str());
		CYCLOPSERROR_II(spacename::_logger, "%s\n", e.what());
		return 1;
	} catch (const std::exception& e) {
		CYCLOPSERROR_II(spacename::_logger, "%s - %s\n", typeid(e).name(), e.what());
		CYCLOPSERROR_II(spacename::_logger, "%s\n", e.what());
		return 1;
	} catch (...) {
		CYCLOPSERROR_II(spacename::_logger, "UNKNOWN EXCEPTION\n");
		return 1;
	}
}
