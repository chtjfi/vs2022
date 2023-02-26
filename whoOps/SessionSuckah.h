#pragma once

#include <string>

#include "whoOps.h"

using std::string;

namespace whoOps {
	class SessionSuckah
	{
	public:
		SessionSuckah(const string& strHost);
		~SessionSuckah(void);
		void NetSessionDel(const string& strTarget, WHOOPS_SESSION_TYPE sessionType);
		string getReturnString(const NET_API_STATUS& status); 
	private:
		const string _strHost;
	};
}