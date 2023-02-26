#ifndef SHARESHANKREDEMPTION_H
#define SHARESHANKREDEMPTION_H

#include <string>
#include <vector>

/* See cyclOps.h for the reason this is commented. */
/* #include <Windows.h> */
#include <LM.h>

#include "..\cyclOps\cyclOps.h"
#include "whoOpsBonanza.h"

#include "Share.h"
#include "ShareACE.h"
#include "JNIResponseShareshankRedemption.h"

namespace whoOps {
	class ShareshankRedemption
	{
	public:
		ShareshankRedemption(const std::string& strServer, const std::string& strShare = ShareshankRedemption::UNSPECIFIED);
		ShareshankRedemption(const cyclOps::JNIRequest& request);
		~ShareshankRedemption(void);
		std::string getPath(void);
		void netShareEnum(whoOps::JNIResponseShareshankRedemption& response);
		void getShare(whoOps::JNIResponseShareshankRedemption& response);
		std::vector<Share> getShareVector(void);
		static const std::string UNSPECIFIED;
		void setContinueOnLookupAccountSidFailure(bool b) { _boContinueOnLookupAccountSidFailure = b; }
		void revokeAndAdd(const std::string& strAccount, const ACCESS_MASK& accessMask);
		static void revokeAndAdd(const Share& share, const std::string& strAccount, const ACCESS_MASK& accessMask);
		void setRemark(const std::string& strRemark);
		void setRemark(const std::wstring& wstrRemark);
		void netShareAdd(const std::string& strPath, const std::string& strRemark);
		void netShareAdd(const std::wstring& wstrPath, const std::wstring& wstrRemark);
		std::wstring getUNCW(void) const;
		std::string  getUNCA(void) const;
		whoOps::Share getShare(void) const;
	private:
		void initialize(const std::string& strServer, const std::string& strShare);
		const char* getShareInfoParameterName(DWORD dwParam);
		bool _boContinueOnLookupAccountSidFailure;
		Share createShareBean(SHARE_INFO_502* pShareInfo) const; 
		void addACLToShareBean(PSECURITY_DESCRIPTOR pSecurityDescriptor, Share& share) const;
		void addACEToShareBean(PACL pACL, DWORD i, Share& share) const;
		void addSpaceInfoToShareBean(whoOps::Share& share) const;
		std::string _strServer;
		std::string _strShare;
	};
}

#endif