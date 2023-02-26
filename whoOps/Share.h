#ifndef WHOOPS_SHARE_H
#define WHOOPS_SHARE_H

#include <vector>
#include <string>

#include "ShareACE.h"

namespace whoOps {
	class Share
	{
	public:
		Share(void);
		~Share(void);
		void		
			setName(const std::wstring& wstrName);
		void		
			setRemark(const std::wstring& wstr);
		void		
			setPath(const std::wstring& wstr);
		std::string 
			getPath() const { return _strPath; }
		void		
			addACE(const ShareACE& ace);
		const std::string& 
			getName(void) const { return _strName; }
		const std::vector<ShareACE>& 
			getACEVector(void) const;
		void		
			setServer(const std::string& strServer) { _strServer = strServer; }
		std::string 
			getServer() const { return _strServer; }
		std::wstring 
			getServerW() const;
		void 
			setSecurityDescriptorNull(bool boIsNull) { _boIsNull = boIsNull; }
		bool 
			isSecurityDescriptorNull() const { return _boIsNull; }
		std::wstring 
			getUNCW(void) const;
		std::string
			getUNCA(void) const;
		bool isAccountPermitted(const std::wstring& wstrAccount) const;
		bool isAccountPermitted(const std::string& strAccount) const;
		std::string getRemark(void) const { return _strRemark; }
		void setTotalGB(const __int64 gb) { _ulTotalGB = gb; }
		void setFreeGB(const __int64 gb) { _ulFreeGB = gb; }
		unsigned __int64 getTotalGB(void) const { return _ulTotalGB; }
		unsigned __int64 getFreeGB(void) const { return _ulFreeGB; }
	private:
		std::string _strName;
		std::string _strServer;
		std::string _strRemark;
		std::string _strPath;
		bool _boIsNull;
		std::vector<ShareACE> _aceVector;
		__int64 _ulTotalGB;
		__int64 _ulFreeGB;
	};
}

#endif