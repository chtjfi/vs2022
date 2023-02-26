#pragma once

#include <string>
#include <vector>

namespace cyclOps {
	class NetworkNanny
	{
	public:
		NetworkNanny(void);
		~NetworkNanny(void);
		bool ping(const std::string& strHost);
		static std::string getIPAddress(const std::string& strHost);
		static std::vector<std::string> getIPAddresses(const std::string& strHost);
		static std::string getFQDN(void);
		static std::string getIPAddress(void);
		static std::string getUserName(void);
	};
}