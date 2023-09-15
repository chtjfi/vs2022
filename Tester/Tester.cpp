#include "stdafx.h"

#include <string>
#include <vector>
#include <map>
#include <exception>
#include <typeinfo>
#include <ctime>
#include <iostream>
#include <time.h>
#include <regex>

#include <sstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/regex.hpp>

#include "..\cyclOps\cyclOps.h"
#include "..\whoOps\whoOpsBonanza.h"

using std::string;
using std::wstring;
using std::stringstream;
using std::vector;
using std::map;
using std::cout;
using std::endl;

using cyclOps::CommandLinebacker;
using cyclOps::StringEmUp;

namespace spacename {

	class Beanie {
	public:
		Beanie() {
			printf("%p\n", this);
		}
	};

	void mainTokenize(const CommandLinebacker& c) {
		string s = "a\nb;c\nd;e";
		string pattern = "[;\n]";
		vector<string> tokens;
		StringEmUp::tokenize(s, pattern, tokens);
		printf("s: %s \n pattern: %s\n====\n", s.c_str(), pattern.c_str());
		for (int i = 0; i < tokens.size(); ++i) {
			printf("%d: %s\n----\n", i, tokens[i].c_str());
		}
		
	}

	void main(const CommandLinebacker& c) {
		wstring s; s = L"a";
		vector<wstring> v; v.push_back(L"b");
		if (cyclOps::StringEmUp::inW(s, v)) {
			printf("true\n");
		}
		else {
			printf("false\n");
		}
		v.push_back(L"A");
		if (cyclOps::StringEmUp::inW(s, v)) {
			printf("true %S\n", s.c_str());
		}
		else {
			printf("false %S\n", s.c_str());
		}
		
	}

	void mainReplaceWithRegex(const CommandLinebacker& c) {
		string text = c.getOption("text");
		string pattern = c.getOption("pattern");
		string replaceWith = c.getOption("replaceWith");
		std::regex reg(pattern);
		string newString = std::regex_replace(text, reg, replaceWith);
		cout << newString << endl;

		
	}

	void tokenizeStringWithRegex(const CommandLinebacker& c) {
		string pattern = c.getOption("pattern");
		boost::regex re(pattern);
		std::string s;
		while (std::getline(std::cin, s)) {
		  boost::sregex_token_iterator i(s.begin(), s.end(), re, -1);
		  boost::sregex_token_iterator j;
		  while (i != j) {
			 std::cout << *i++ << "\n";
		  }
		  std::cout << std::endl;
		}	
	}

	void isNowLaterThan(const CommandLinebacker& c) {
		cyclOps::TimeTiger tiger;
		string time = c.getOption("time"); //"2018-10-05T09:00:00";
		int hours = c.getInt("hours");
		printf("%d\n", tiger.isNowLaterThanTimePlusXHours(time, hours));
	}

	void mainAddServerToPrOps(const CommandLinebacker& c) {
		cyclOps::HTTPopotamus http;
		http.setHostname("chvdevops50");
		string query = "/prOps/actions/ServerDetails.do?"
			"method=addOrUpdateServerAndRole&"
			"hostname=newtstnew&"
			"owner=chtjfi&"
			"application=Citrix&"
			"roleName=Citrix Server";
		http.setQuery(query);
		http.GET();
	}

	void playWithTime(void) {
		/* Getting time difference in milliseconds. */
		clock_t t;
		t = clock();

		size_t size = 100000;
		int *pInt = new int[size];
		for(size_t i = 0; i < size; i++)
			pInt[i] = rand();

		t = clock() - t;
		cout << "time: " << t << " miliseconds" << endl;
		cout << CLOCKS_PER_SEC << " clocks per second" << endl;
		cout << "time: " << t*1.0/CLOCKS_PER_SEC << " seconds" << endl;

		system("pause");
		delete [] pInt;
	}

	void mainJSON(const CommandLinebacker& cmd) {
		string foo = "{\"foo\":\"bar\"}";
		// Read json.
		boost::property_tree::ptree pt2;
		std::istringstream is (foo);
		boost::property_tree::read_json (is, pt2);
		std::string bar = pt2.get<std::string> ("foo");
		printf("%s\n", bar.c_str());

	}
}

int main(int argc, char* argv[])
{
	try {
		CommandLinebacker cmd(argc, argv);
		g_boCyclOpsDebug = cmd.isOptionPresent("debug"); 
		spacename::main(cmd);
		printf("The command completed successfully.\n");
		fprintf(stderr, "stderr\n");
		return 0;
	} catch (const std::exception& e) {
		printf("%s - %s\n", typeid(e).name(), e.what());
		return 1;
	} catch (...) {
		printf("UNKNOWN EXCEPTION\n");
		return 1;
	}
}
