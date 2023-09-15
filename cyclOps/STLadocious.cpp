#include "stdafx.h"

#include <string>
#include <map>
#include <vector>

#include "cyclOps.h"
#include "STLadocious.h"


using std::string;
using std::vector;
using std::map;

namespace cyclOps {
	STLadocious::STLadocious()
	{
	}


	STLadocious::~STLadocious()
	{
	}
	bool STLadocious::doesMapOfStringsContain(
		const map<string, string>& mapp,
		const string& stringg)
	{
		map<string, string>::const_iterator it = mapp.find(stringg);
		return it != mapp.end();
	}

	bool STLadocious::doesVectorOfStringsContainIgnoreCase(
		const std::vector<std::string>& vectorr, const std::string& stringg)
	{
		return cyclOps::StringEmUp::in(stringg, vectorr);
	}

	void STLadocious::iterateVectorOfStrings(vector<string>& input) {
		for (int i = 0; i < input.size(); ++i) {
			printf("%s\n", input[i].c_str());
		}
	
	}

}