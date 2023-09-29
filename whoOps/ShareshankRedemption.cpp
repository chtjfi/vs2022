#include "StdAfx.h"

#include <string.h>

#include <sstream>

/* I can't get the Winsock header file errors to go away!  See cyclOps.h. */
#include <WinSock2.h>



#include "whoOpsPrivate.h"

#include "ShareshankRedemption.h"

using std::wstring;
using std::wstringstream;
using std::stringstream;
using std::vector;

using cyclOps::StringEmUp;

namespace whoOps {

	const string ShareshankRedemption::UNSPECIFIED = "UNSPECIFIED";

	ShareshankRedemption::ShareshankRedemption(const string& strServer, const string& strShare)	{
		initialize(strServer, strShare);
	}

	ShareshankRedemption::ShareshankRedemption(const cyclOps::JNIRequest& request) {
		string strServer = request.getProperty("server");
		string strShare;
		try {
			strShare = request.getProperty("share");
		} catch (const cyclOps::ExceptionNoSuchNode& ignore) { ignore;
			strShare = ShareshankRedemption::UNSPECIFIED; 
		}
		initialize(strServer, strShare);
	}

	void ShareshankRedemption::initialize(const string& strServer, const string& strShare) {
		this->_strServer = strServer;
		this->_strShare = strShare;
		this->_boContinueOnLookupAccountSidFailure = false;
	}

	ShareshankRedemption::~ShareshankRedemption(void)
	{
	}

	string ShareshankRedemption::getPath(void) {
		SHARE_INFO_502* pShareInfo502;
		wchar_t wszServer[1000];
		CYCLOPS_SNWPRINTF_S(wszServer, L"%S", _strServer.c_str());
		wchar_t wszShare[500];
		CYCLOPS_SNWPRINTF_S(wszShare, L"%S", _strShare.c_str());
		NET_API_STATUS nas = NetShareGetInfo(wszServer, wszShare, 502, (LPBYTE*) &pShareInfo502);
		if (nas != ERROR_SUCCESS) {
			CYCLOPS_THROW_EXCEPTION_IV(whoOps::ExceptionShareshankRedemption, "Error %d returned from NetShareGetInfo() for \\\\%S\\%S", nas, wszServer, wszShare);
		} else {
			wstring wstrPath(pShareInfo502->shi502_path);
			string strPath(wstrPath.begin(), wstrPath.end());
			NetApiBufferFree(pShareInfo502);
			return strPath;
		}
	}

	void ShareshankRedemption::netShareEnum(whoOps::JNIResponseShareshankRedemption& response) {
		vector<Share> vectorOfShares = this->getShareVector();
		for (size_t i = 0; i < vectorOfShares.size(); ++i) {
			response.addShare(vectorOfShares[i]);
		}
	}

	vector<Share> ShareshankRedemption::getShareVector(void) {
		vector<Share> vectorOfShares;
		wstring wstrServer(this->_strServer.begin(), this->_strServer.end()); CYCLOPSDEBUG("wstrServer = %S", wstrServer.c_str());
		wchar_t wszServer[1000];
		CYCLOPS_SNWPRINTF_S(wszServer, L"%S", _strServer.c_str());
		SHARE_INFO_502 *pShareInfo = NULL;
		DWORD dwRead, dwTotal;
		NET_API_STATUS nas = NetShareEnum(wszServer, 502, (LPBYTE*) &pShareInfo, MAX_PREFERRED_LENGTH, &dwRead, &dwTotal, NULL); 
		if (nas != ERROR_SUCCESS && nas != ERROR_MORE_DATA ) {
			ErroroneousMonk error(nas);
			CYCLOPS_THROW_EXCEPTION_IV(whoOps::ExceptionShareshankRedemption, "NetShareEnum() failed for server %S.  %s", wszServer, error.formatMessage().c_str());
		}
		for (DWORD i = 0; i < dwRead; i++ ) { CYCLOPSDEBUG("i = %d", i);
			if (pShareInfo[i].shi502_type == STYPE_DISKTREE) {
				Share share = this->createShareBean(&(pShareInfo[i])); CYCLOPSDEBUG("share = %s", share.getName().c_str());
				vectorOfShares.push_back(share); CYCLOPSDEBUG("push_back() done.");
			} else { CYCLOPSDEBUG("Share %S not of type STYPE_DISKTREE", pShareInfo->shi502_netname); } CYCLOPSDEBUG("i %d done.", i);
		} CYCLOPSDEBUG("vectorOfShares.size() = %d", vectorOfShares.size());
		return vectorOfShares;
	}

	Share ShareshankRedemption::createShareBean(SHARE_INFO_502* pShareInfo) const { CYCLOPSDEBUG("pShareInfo->netName = %S", pShareInfo->shi502_netname);
		Share share; 
		share.setName(pShareInfo->shi502_netname);
		share.setServer(this->_strServer); CYCLOPSDEBUG("setServer() done.");
		if (pShareInfo->shi502_path == 0) {
			CYCLOPS_THROW_EXCEPTION_IV(whoOps::ExceptionShareshankRedemption, "Null path for \\\\%s\\%s.", _strServer.c_str(), pShareInfo->shi502_netname);
		}
		share.setPath(pShareInfo->shi502_path);
		if (pShareInfo->shi502_remark != 0) {
			share.setRemark(pShareInfo->shi502_remark);
		}
		/* From http://technet.microsoft.com/en-us/library/cc781716(v=ws.10).aspx 
		   Note that it is possible for a security descriptor to have no DACL (also known as a NULL DACL); this gives 
		   unconditional access to everyone. A security descriptor with an empty DACL gives no access to anyone. */
		if (pShareInfo->shi502_security_descriptor != 0) {
			share.setSecurityDescriptorNull(false);
			this->addACLToShareBean(pShareInfo->shi502_security_descriptor, share);
		}
		this->addSpaceInfoToShareBean(share);
		return share;
	}

	void ShareshankRedemption::addSpaceInfoToShareBean(whoOps::Share& share) const {
		ULARGE_INTEGER freeBytesForCaller, totalBytes, totalFreeBytes;
		::GetDiskFreeSpaceExA(share.getUNCA().c_str(), &freeBytesForCaller, &totalBytes, &totalFreeBytes);
		__int64 ulTotalGB = totalBytes.QuadPart / (1024 * 1024 * 1024);
		__int64 ulFreeGB = totalFreeBytes.QuadPart / (1024 * 1024 * 1024);
		share.setTotalGB(ulTotalGB);
		share.setFreeGB(ulFreeGB);
	}

	void ShareshankRedemption::addACLToShareBean(PSECURITY_DESCRIPTOR pSecurityDescriptor, Share& share) const { CYCLOPSDEBUG("pSecurityDescriptor = %p.", pSecurityDescriptor);
		BOOL bDaclPresent;
		PACL pACL;
		BOOL bDaclDefaulted;
		if (!GetSecurityDescriptorDacl(pSecurityDescriptor, &bDaclPresent, &pACL, &bDaclDefaulted)) { CYCLOPSDEBUG("GetSecurityDescriptorDacl() failed.");
			CYCLOPS_THROW_EXCEPTION_IV(whoOps::ExceptionShareshankRedemption, "GetSecurityDescriptorDacl() failed.");
		} CYCLOPSDEBUG("GetSecurityDescriptorDacl() done.");
		if (bDaclPresent) { CYCLOPSDEBUG("DACL present.");
			ACL_SIZE_INFORMATION aclSizeInfo;
			if ( ! GetAclInformation(pACL, &aclSizeInfo, sizeof(ACL_SIZE_INFORMATION), AclSizeInformation)) { CYCLOPSDEBUG("GetAclInformation() failed.");
				CYCLOPS_THROW_EXCEPTION_IV(whoOps::ExceptionShareshankRedemption, "GetAclInformation() failed.");
			}
			for (DWORD i = 0; i < aclSizeInfo.AceCount; ++i) { CYCLOPSDEBUG("i = %d", i);
				this->addACEToShareBean(pACL, i, share);
			}
		} else { CYCLOPSDEBUG("DACL not present."); }
	}

	void ShareshankRedemption::addACEToShareBean(PACL pACL, DWORD i, Share& share) const {
		ACCESS_ALLOWED_ACE *pAce;
		if( ! GetAce(pACL, i, (LPVOID *) &pAce)) {
			CYCLOPS_THROW_EXCEPTION_IV(whoOps::ExceptionShareshankRedemption, "GetAce() failed.");
		} 
		char szAccount[1000]; DWORD dwAccountSize = sizeof(szAccount) / sizeof(szAccount[0]);
		char szDomain[1000]; DWORD dwDomainSize = sizeof(szDomain) / sizeof(szDomain[0]);
		SID_NAME_USE sidType;
		if( ! LookupAccountSidA(NULL, &pAce->SidStart, szAccount, &dwAccountSize, szDomain, &dwDomainSize, &sidType)) {
			if (this->_boContinueOnLookupAccountSidFailure) {
				strcpy_s(szAccount, "UNKNOWN");
				strcpy_s(szDomain, "UNKNOWN");
			} else {
				DWORD dwLastError = ::GetLastError();
				CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionLookupAccountSID, dwLastError, cyclOps::Exception::TYPE_WIN32, "LookupAccountSid() failed.");
			}
		} CYCLOPSDEBUG("szAccount = %s", szAccount);
		ShareACE ace; 
		ace.setAccount(szAccount); CYCLOPSDEBUG("ace.getAccount() = %s", ace.getAccount().c_str());
		ace.setDomain(szDomain);
		ace.setMask(pAce->Mask);
		share.addACE(ace);
	}

	wstring ShareshankRedemption::getUNCW(void) const {
		string strUNC = this->getUNCA();
		return wstring(strUNC.begin(), strUNC.end());
	}

	string ShareshankRedemption::getUNCA(void) const { CYCLOPSDEBUG("_strShare/UNSPECIFIED = %s/%s", _strShare.c_str(), UNSPECIFIED.c_str());
		if (_strShare.compare(ShareshankRedemption::UNSPECIFIED) == 0) {
			CYCLOPS_THROW_EXCEPTION_IV(whoOps::ExceptionShareshankRedemption, "There is no share specified.");
		} else {
			string strUNC = "\\\\" + _strServer + "\\" + _strShare;
			return strUNC;
		}
	}


	void ShareshankRedemption::revokeAndAdd(const string& strAccount, const ACCESS_MASK& accessMask) { 
		wstring wstrAccount(strAccount.begin(), strAccount.end());
		wstring wstrUNC = this->getUNCW();
		ACLAmigo::revokeAccountFromShare(wstrUNC, wstrAccount.c_str());
		ACLAmigo::addAccountToShare(wstrUNC, wstrAccount, accessMask);
	}

	void ShareshankRedemption::revokeAndAdd(const Share& share, const string& strAccount, const ACCESS_MASK& accessMask) { CYCLOPSDEBUG("share/account/mask = %S/%s/%d", share.getUNCW().c_str(), strAccount.c_str(), accessMask);
		wstring wstrAccount(strAccount.begin(), strAccount.end());
		ACLAmigo::revokeAccountFromShare(share.getUNCW(), wstrAccount.c_str());
		ACLAmigo::addAccountToShare(share.getUNCW(), wstrAccount, accessMask);
	}
	
	void ShareshankRedemption::setRemark(const std::string& strRemark) {
		wstring wstrRemark(strRemark.begin(), strRemark.end());
		this->setRemark(wstrRemark);
	}

	void ShareshankRedemption::setRemark(const std::wstring& wstrRemark) {
		SHARE_INFO_1004 shareInfo1004;
		size_t sizeOfRemark = wstrRemark.length() + 10;
		wchar_t* pwchRemark = new wchar_t[sizeOfRemark];
		_snwprintf_s(pwchRemark, sizeOfRemark - 1, _TRUNCATE, L"%s", wstrRemark.c_str());
		shareInfo1004.shi1004_remark = pwchRemark;
		size_t sizeOfServer = this->_strServer.length() + 10;
		wchar_t* pwchServer = new wchar_t[sizeOfServer];
		_snwprintf_s(pwchServer, sizeOfServer - 1, _TRUNCATE, L"%S", this->_strServer.c_str());
		size_t sizeOfShare = this->_strShare.length() + 10;
		wchar_t* pwchShare = new wchar_t[sizeOfShare];
		_snwprintf_s(pwchShare , sizeOfShare - 1, _TRUNCATE, L"%S", this->_strShare.c_str());
		::NetShareSetInfo(pwchServer, pwchShare, 1004, (LPBYTE) &shareInfo1004, NULL);
		delete[] pwchRemark;
		delete[] pwchServer;
		delete[] pwchShare;
	}
	
	void ShareshankRedemption::netShareAdd(const std::string& strPath, const std::string& strRemark) { CYCLOPSDEBUG("Hello.");
		wstring wstrPath(strPath.begin(), strPath.end());
		wstring wstrRemark(strRemark.begin(), strRemark.end());
		this->netShareAdd(wstrPath, wstrRemark);
	}

	void ShareshankRedemption::netShareAdd(const std::wstring& wstrPath, const std::wstring& wstrRemark) { CYCLOPSDEBUG("Hello.");
		::SHARE_INFO_2 shareInfo2;
		wchar_t* pwchServer = StringEmUp::new_wcharArray(this->_strServer);
		shareInfo2.shi2_netname = StringEmUp::new_wcharArray(this->_strShare);
		shareInfo2.shi2_path = StringEmUp::new_wcharArray(wstrPath);
		shareInfo2.shi2_remark = StringEmUp::new_wcharArray(wstrRemark);
		shareInfo2.shi2_current_uses = 0;
		shareInfo2.shi2_max_uses = -1;
		shareInfo2.shi2_passwd = NULL;
		shareInfo2.shi2_permissions = 0;
		shareInfo2.shi2_type = STYPE_DISKTREE;
		DWORD dwOffendingParameter = 0; 
		NET_API_STATUS nas = ::NetShareAdd(pwchServer, 2, (LPBYTE) &shareInfo2, &dwOffendingParameter);
		if (nas != NERR_Success) {
			string strAdditionalInfo;
			if (nas == ERROR_INVALID_PARAMETER) {
				const char* szOffendingParameter = this->getShareInfoParameterName(dwOffendingParameter);
				stringstream ss;
				ss << "Invalid parameter is " << szOffendingParameter;
				strAdditionalInfo = ss.str();
			}
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionShareshankRedemption, nas, cyclOps::Exception::TYPE_WIN32,
				"Unable to create share %S on server %S with path %S.  %s",
				shareInfo2.shi2_netname, pwchServer, shareInfo2.shi2_path, strAdditionalInfo.c_str());
		}
		delete[] pwchServer;
		delete[] shareInfo2.shi2_netname;
		delete[] shareInfo2.shi2_path;
		delete[] shareInfo2.shi2_remark;
	}

	struct ShareInfoParameter {
		DWORD param;
		const char* szName;
	};

	ShareInfoParameter SHARE_INFO_PARAMETERS[] = {
		{SHARE_NETNAME_PARMNUM, "SHARE_NETNAME_PARMNUM"},
		{SHARE_TYPE_PARMNUM, "SHARE_TYPE_PARMNUM"},
		{SHARE_REMARK_PARMNUM, "SHARE_REMARK_PARMNUM"},
		{SHARE_PERMISSIONS_PARMNUM, "SHARE_PERMISSIONS_PARMNUM"},
		{SHARE_MAX_USES_PARMNUM, "SHARE_MAX_USES_PARMNUM"},
		{SHARE_CURRENT_USES_PARMNUM, "SHARE_CURRENT_USES_PARMNUM"},
		{SHARE_PATH_PARMNUM, "SHARE_PATH_PARMNUM"},
		{SHARE_PASSWD_PARMNUM, "SHARE_PASSWD_PARMNUM"},
		{SHARE_FILE_SD_PARMNUM, "SHARE_FILE_SD_PARMNUM"}
	};

	const char* ShareshankRedemption::getShareInfoParameterName(DWORD dwParam) {
		size_t size = sizeof(SHARE_INFO_PARAMETERS) / sizeof(SHARE_INFO_PARAMETERS[0]);
		for (size_t i = 0; i < size; ++i) {
			if (SHARE_INFO_PARAMETERS[i].param == dwParam) {
				return SHARE_INFO_PARAMETERS[i].szName;
			}
		}
		return "UNKNOWN";
	}

	void ShareshankRedemption::getShare(whoOps::JNIResponseShareshankRedemption& response) {
		Share share = this->getShare();
		response.addShare(share);
	}

	whoOps::Share ShareshankRedemption::getShare(void) const {
		SHARE_INFO_502* pShareInfo502;
		wchar_t wszServer[1000];
		CYCLOPS_SNWPRINTF_S(wszServer, L"%S", _strServer.c_str());
		wchar_t wszShare[500];
		CYCLOPS_SNWPRINTF_S(wszShare, L"%S", _strShare.c_str());
		NET_API_STATUS nas = NetShareGetInfo(wszServer, wszShare, 502, (LPBYTE*)&pShareInfo502);
		if (nas != ERROR_SUCCESS) {
			CYCLOPS_THROW_EXCEPTION_IV(whoOps::ExceptionShareshankRedemption, "Error %d returned from NetShareGetInfo() for \\\\%S\\%S", nas, wszServer, wszShare);
		}
		else {
			Share share = this->createShareBean(pShareInfo502);
			NetApiBufferFree(pShareInfo502);
			return share;
		}

	}
}