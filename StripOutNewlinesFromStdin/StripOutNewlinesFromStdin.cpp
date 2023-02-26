#include "stdafx.h"

#include <string>
#include <vector>
#include <map>
#include <exception>
#include <typeinfo>
#include <iostream>

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

	boolean isBlankLine(const string& line) {
		if (cyclOps::RegExtravaganza::matches(line, "\\\\s+")) {
			return true;
		}
		if (StringEmUp::isEmpty(line)) {
			return true;
		}
		return false;
	}

	void main(const CommandLinebacker& cmd) {
		/*********************************************/
		/* !!! USE _logger TO LOG EVERYTHING NOW !!! */
		/*********************************************/
		std::string line;
		string output;
		int numberOfBlankLines = 0;
		while (std::getline(std::cin, line) && ! StringEmUp::equalsIgnoreCase(line, "exit")) {
			string append;
			if (spacename::isBlankLine(line)) { CYCLOPSDEBUG("Whitespace only.");
				++numberOfBlankLines;
				if (numberOfBlankLines >= 1) {
					append = "\n\n";
					numberOfBlankLines = 0;
				}
			} else {
				append.assign(line + " ");
			}
			output += append;
		}
		printf("%s", output.c_str());
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
		/* cmd.initializeConfigFile(spacename::_xmlConfig, false); /* Don't use exe name as default. 
		cmd.initializeLogFile(spacename::_logger, cyclOps::CommandLinebacker::MakeLognameUnique::False);*/
		spacename::main(cmd);
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
