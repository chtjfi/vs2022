#pragma once

#include <map>
#include <vector>
#include <string>

namespace cyclOps {
class STLadocious
{
public:
	explicit STLadocious();
	~STLadocious();
	static bool doesMapOfStringsContain(const std::map<std::string, std::string>& mapp, const std::string& stringg);
	static void iterateVectorOfStrings(std::vector<std::string>& input);
	static bool doesVectorOfStringsContainIgnoreCase(const std::vector<std::string>& vectorr, const std::string& stringg);
private:
	STLadocious & operator=(const STLadocious&);
	STLadocious(const STLadocious&);
};

}