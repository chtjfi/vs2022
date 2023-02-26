#ifndef _STRUNG_H_
#define _STRUNG_H_

#include <string>
#include <map>

namespace cyclOps {
	class Strung {
	public:
		Strung(void) { }
		~Strung(void) { }
		Strung& set(const std::string& strVariable, const std::string& strValue);
		Strung& Strung::set(const std::string& strVariable, const DWORD dwValue);
		Strung& set1000(const std::string& strVariable, const char* szFormat, ...);
		/* Memory wasting Javaesque version. */
		std::string format(const std::string& strFormat) const; 
		/* Speicherfreundlich C++ style. */
		void format(std::string& strTarget, const std::string& strFormat) const; 
		Strung& addVariables(const std::map<std::string, std::string>& variables);
		std::string getMapAsString(void) const;
	private:
		std::map<std::string, std::string> _mapOfVariables;
	};
}

#endif