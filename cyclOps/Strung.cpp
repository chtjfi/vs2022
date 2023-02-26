#include "stdafx.h"

#include <sstream>
#include <boost/algorithm/string/replace.hpp>

#include "cyclOps.h"
#include "Strung.h"

using std::stringstream;
using std::string;
using std::map;

namespace cyclOps {

	string Strung::getMapAsString(void) const {
		stringstream output;
		for (map<string, string>::const_iterator i = this->_mapOfVariables.begin();
			i != this->_mapOfVariables.end(); ++i) 
		{
			output << i->first << "\t" << i->second << std::endl;
		}
		return output.str();
	}

	Strung& Strung::set1000(const string& strVariable, const char* szFormat, ...) { CYCLOPSDEBUG("strVariable = %s", strVariable.c_str());
		va_list vaArguments; 
		va_start(vaArguments, szFormat); 
		char szOutput[1000]; 
		_vsnprintf_s(szOutput, sizeof(szOutput) / sizeof(szOutput[0]), _TRUNCATE, szFormat, vaArguments); 
		_mapOfVariables[strVariable] = szOutput;
		return *this;
	}
	Strung& Strung::set(const string& strVariable, const DWORD dwValue) { 
		std::string strValue = cyclOps::StringEmUp::to_string(dwValue);
		_mapOfVariables[strVariable] = strValue;
		return *this;
	}

	Strung& Strung::set(const string& strVariable, const string& strValue) { CYCLOPSDEBUG("variable/value = %s/%s", strVariable.c_str(), strValue.c_str());
		_mapOfVariables[strVariable] = strValue;
		return *this;
	}
	string Strung::format(const string& strFormat) const {
		/* This is the low performance version for a nice Javaesque grammar. */
		string strTarget = strFormat;
		this->format(strTarget, strFormat);
		return strTarget;
	}

	void Strung::format(string& strTarget, const string& strFormat) const { CYCLOPSDEBUG("strTarget/strFormat = '%s'/'%s'", strTarget.c_str(), strFormat.c_str());
		strTarget = strFormat;
		/* This is the high performance version!!! */
		for (map<string, string>::const_iterator iterator = _mapOfVariables.begin(); 
			iterator != _mapOfVariables.end(); ++iterator) 
		{
			string strVariable = iterator->first; CYCLOPSDEBUG("strVariable = %s", strVariable.c_str());
			string strValue = iterator->second; CYCLOPSDEBUG("strValue = '%s'", strValue.c_str());
			stringstream ssVariableWithBrackets;
			ssVariableWithBrackets << "${" << strVariable << "}"; CYCLOPSDEBUG("ssVariableWithBrackets = '%s'", ssVariableWithBrackets.str().c_str()); CYCLOPSDEBUG("strTarget before replacment = '%s'", strTarget.c_str());
			boost::replace_all(strTarget, ssVariableWithBrackets.str(), strValue); CYCLOPSDEBUG("strTarget after replacment = '%s'", strTarget.c_str());
		}
	}

	Strung& Strung::addVariables(const map<string, string>& variables) { 
		for (map<string, string>::const_iterator i = variables.begin(); i != variables.end(); ++i ) {
			string key = (*i).first;
			string value = (*i).second;
			_mapOfVariables[key] = value;
		}
		return *this;
	}

}
		