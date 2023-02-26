#ifndef WHOOPS_SHAREACE_H
#define WHOOPS_SHAREACE_H

#include <string>

#include "Constants.h"
#include "ShareACE.h"

namespace whoOps {
	class ShareACE
	{
	public:
		ShareACE(void);
		~ShareACE(void);
		void setAccount(const std::string& str) { _strAccount = str; }
		void setDomain(const std::string& str) { _strDomain = str; }
		void setMask(ACCESS_MASK mask) { _mask = mask; }
		const std::string& getAccount(void) const { return _strAccount; }
		const std::string& getDomain(void) const { return _strDomain; }
		const ACCESS_MASK& getMask(void) const { return _mask; }
		std::string getMaskString(void) const;
	private:
		std::string _strAccount;
		std::string _strDomain;
		ACCESS_MASK _mask;
	};
}

#endif