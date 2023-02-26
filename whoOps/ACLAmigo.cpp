#include "StdAfx.h"

#include <sstream>
#include <string>
#include <vector>
#include <iterator>

#include "..\cyclOps\cyclOps.h"
#include "..\cyclOps\StringEmUp.h"

#include "ErroroneousMonk.h"
#include "ExceptionACLAmigo.h"
#include "FileFeeFoFum.h"
#include "ExceptionFileFeeFoFum.h"
#include "ACLAmigo.h"

using std::vector;
using std::wstring;
using std::string;

using cyclOps::StringEmUp;

namespace whoOps {
	whoOps::ACLAmigo::ACLAmigo(void)
	{
	}


	whoOps::ACLAmigo::~ACLAmigo(void)
	{
	}

	void whoOps::ACLAmigo::AddACEToDACL(const string& strPath, const char* szSecurityPrincipal, const DWORD dwPermission) { CYCLOPSDEBUG("strPath = %s", strPath.c_str());
		ACL_SIZE_INFORMATION ACLInfo; 
		memset(&ACLInfo, 0, sizeof(ACL_SIZE_INFORMATION));
		UCHAR   BuffSid[256];
		PSID pSID = (PSID) BuffSid;
		int returnCode = ResolveSID(szSecurityPrincipal, pSID);
		SE_OBJECT_TYPE SEObjType = SE_FILE_OBJECT; 
		PACL pOldDACL = NULL;
		PSECURITY_DESCRIPTOR pSD = NULL;
		SECURITY_INFORMATION ACLSecInfo = DACL_SECURITY_INFORMATION;
		boolean boSuccesful = true;
		returnCode = GetNamedSecurityInfoA(strPath.c_str(), SEObjType, ACLSecInfo, NULL, NULL, &pOldDACL, NULL, &pSD);  
		/* char* szReturn = NULL; */
		string strReturn;
		if (returnCode != ERROR_SUCCESS) {
			boSuccesful = false;
			ErroroneousMonk error(returnCode);
			strReturn = StringEmUp::format("GetNamedSecurityInfoA returned an error: %s", error.formatMessage().c_str());
		} else {
			BOOL getACLResult = GetAclInformation(pOldDACL, &ACLInfo, sizeof(ACLInfo), AclSizeInformation); 
			if (!getACLResult) {
				boSuccesful = false;
				strReturn = "GetAclInformation() failed.";
			} else {
				DWORD cb = 0;
				DWORD cbExtra = sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD) + GetLengthSid(pSID); 
				cb = ACLInfo.AclBytesInUse + cbExtra; 
				PACL pNewDACL = static_cast<PACL>(HeapAlloc(GetProcessHeap(),0,cb)); 
				BOOL initACLResult = InitializeAcl(pNewDACL, cb, ACL_REVISION); 
				if (!initACLResult) {
					boSuccesful = false;
					strReturn = "InitializeAcl() failed.";
				} else {
					for (DWORD i = 0; i < ACLInfo.AceCount; ++i)  { 
						ACE_HEADER * pACE = 0; 
						GetAce(pOldDACL, i, reinterpret_cast<void**>(&pACE)); 
						pACE->AceFlags = CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE;
						pACE->AceType = ACCESS_ALLOWED_ACE_TYPE;  
						AddAce(pNewDACL, ACL_REVISION, MAXDWORD, pACE, pACE->AceSize); 
					} 
					BOOL addACEResult = AddAccessAllowedAceEx(pNewDACL, ACL_REVISION, 
						CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE, dwPermission, pSID);
					if (!addACEResult) {
						boSuccesful = false;
						strReturn = "AddAccessAllowedAceEx() failed.";
					} else {	CYCLOPSDEBUG("strPath.size = %d", strPath.size());
						size_t iSize = strPath.size() + 10;
						char* pchPath = new char[iSize];
						_snprintf_s(pchPath, iSize - 1, _TRUNCATE, "%s", strPath.c_str()); CYCLOPSDEBUG("pchPath = %s", pchPath);
						DWORD setSIResult = SetNamedSecurityInfoA(pchPath, SEObjType, ACLSecInfo, NULL, NULL,
							pNewDACL, NULL); 
						if (setSIResult != ERROR_SUCCESS) {
							boSuccesful = false;
							ErroroneousMonk error(setSIResult);
							strReturn = StringEmUp::format("SetNamedSecurityInfoA returned an error: %s", error.formatMessage().c_str());
						} 
						delete[] pchPath;
					}
				}
				if (pNewDACL) HeapFree(GetProcessHeap(),0, pNewDACL);
			}
			if (pSD) LocalFree(pSD);
		}
		if ( ! boSuccesful) { 
			CYCLOPS_THROW_EXCEPTION_IV(whoOps::ExceptionACLAmigo, "Unable to set permission on %s for %s. %s", 
				strPath.c_str(), szSecurityPrincipal, strReturn.c_str());
		}
	}

	int whoOps::ACLAmigo::ResolveSID(const char* szSecurityPrinciple, const PSID O_pSID) { CYCLOPSDEBUG("Hello.");
		int returnCode = ERROR_SUCCESS;
		SID_NAME_USE SIDType;
		DWORD SIDSize = 0;
		DWORD buffDomainNameSize = 0;
		const int MAX_BUFF = 256;
		char buffDomainName[MAX_BUFF];

		SIDSize = buffDomainNameSize = MAX_BUFF;

		if (!LookupAccountNameA(NULL, 
							   szSecurityPrinciple, 
							   O_pSID, 
							   &SIDSize, 
							   buffDomainName, 
							   &buffDomainNameSize, 
							   &SIDType))
		{
			returnCode = GetLastError();
		}
		return returnCode;
	}

	DWORD whoOps::ACLAmigo::getPermission(const char* szPermission) { CYCLOPSDEBUG("Hello.");
		if (_stricmp(szPermission, "modify") == 0) {
			return whoOps::ACLAmigo::_FILE_GENERIC_EXECUTE 
				| whoOps::ACLAmigo::_FILE_GENERIC_WRITE 
				| whoOps::ACLAmigo::_FILE_GENERIC_READ 
				| whoOps::ACLAmigo::_DELETE;
		}
		if (_stricmp(szPermission, "read_control") == 0) {
			return whoOps::ACLAmigo::_READ_CONTROL;
		}
		if (_stricmp(szPermission, "full") == 0) {
			return whoOps::ACLAmigo::_STANDARD_RIGHTS_ALL | whoOps::ACLAmigo::_SPECIFIC_RIGHTS_ALL;
		}
		if (_stricmp(szPermission, "standard_rights_all") == 0) {
			return whoOps::ACLAmigo::_STANDARD_RIGHTS_ALL;
		}
		string strError = cyclOps::StringEmUp::format("The permission '%s' is not known to ACLAmigo.", szPermission);
		CYCLOPS_THROW_EXCEPTION_III(whoOps::ExceptionACLAmigo, strError);
	}

	void whoOps::ACLAmigo::addPermissions(const string& strPath, const char* szSecurityPrincipal, const char* szPermission) { 
		CYCLOPSWARNING("You are calling the old and unreliable addPermissions() function.  You should consider updating to use addPermissionsToFileOrFolder()."); 
		DWORD dwPermission = this->getPermission(szPermission);
		this->addPermissions(strPath, szSecurityPrincipal, dwPermission);
	}

	void whoOps::ACLAmigo::addPermissionsToFileOrFolder(const wstring& wstrPath, const string& strSecurityPrincipal, const char* szPermission) { 
		DWORD dwPermission = this->getPermission(szPermission);
		this->addPermissionsToFileOrFolder(wstrPath, strSecurityPrincipal, dwPermission);
	}


	void whoOps::ACLAmigo::addPermissions(const string& strPath, const char* szSecurityPrincipal, const DWORD dwPermission) { CYCLOPSDEBUG("Hello.");
		CYCLOPSWARNING("You are calling the old and unreliable addPermissions() function.  You should consider updating to use addPermissionsToFileOrFolder()."); 
		this->AddACEToDACL(strPath, szSecurityPrincipal, dwPermission);
		if (this->isDirectory(strPath)) {
			this->addPermissionsToChildren(strPath, szSecurityPrincipal, dwPermission);
		}
	}

	void whoOps::ACLAmigo::addPermissionsToFileOrFolder(const wstring& wstrPath, const string& strTrustee, 
		const DWORD dwAccessRights) { CYCLOPSDEBUG("Hello.");
		wstring wstrTrustee(strTrustee.begin(), strTrustee.end());
		this->addAceToObjectsSecurityDescriptor(
			wstrPath.c_str(), 
			SE_FILE_OBJECT, 
			wstrTrustee.c_str(), 
			TRUSTEE_IS_NAME,
			dwAccessRights, 
			GRANT_ACCESS, 
			CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE);
		if (this->isDirectory(wstrPath)) {
			this->addPermissionsToChildrenII(wstrPath, strTrustee, dwAccessRights);
		}
	}


	bool whoOps::ACLAmigo::isDirectory(const std::string& strPath) { CYCLOPSDEBUG("Hello.");
		DWORD dwFileAttributes = ::GetFileAttributesA(strPath.c_str());
		return (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 1;
	}

	bool whoOps::ACLAmigo::isDirectory(const std::wstring& wstrPath) { CYCLOPSDEBUG("Hello.");
		DWORD dwFileAttributes = ::GetFileAttributesW(wstrPath.c_str());
		return (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 1;
	}

	void whoOps::ACLAmigo::addPermissionsToChildren(const string& strPath, const char* szSecurityPrincipal, const DWORD dwPermission) { CYCLOPSDEBUG("Hello.");
		FileFeeFoFum file(strPath);
		vector<FileFeeFoFum> files = file.listFiles("*.*");
		for (std::vector<FileFeeFoFum>::iterator i = files.begin(); i != files.end(); ++i) {
			this->addPermissions(i->getAbsolutePath(), szSecurityPrincipal, dwPermission);		
		}
	}

	void whoOps::ACLAmigo::addPermissionsToChildrenII(const wstring& wstrPath, const string& strTrustee, const DWORD dwPermission) { CYCLOPSDEBUG("Hello.");
		FileFeeFoFum file(wstrPath);
		vector<FileFeeFoFum> files = file.listFiles("*.*");
		for (std::vector<FileFeeFoFum>::iterator i = files.begin(); i != files.end(); ++i) {
			this->addPermissionsToFileOrFolder(i->getAbsolutePathW(), strTrustee, dwPermission);		
		}
	}


	void ACLAmigo::AddACEToDACLII(ARGSS& args) { 
		CYCLOPSDEBUG("args = %S, %d", args.wstrObject.c_str(), args.objectType);
		DWORD dwRes = ::GetNamedSecurityInfo(args.wstrObject.c_str(), args.objectType, DACL_SECURITY_INFORMATION, 
			NULL, NULL, &args.pOldDACL, NULL, &args.pSecurityDescriptor); CYCLOPSVAR(dwRes, "%d");
		if (ERROR_SUCCESS != dwRes) {
			ACLAmigo::cleanup(args.pSecurityDescriptor, args.pNewDACL); 
			CYCLOPS_THROW_EXCEPTION_IV(whoOps::ExceptionACLAmigo, "GetNamedSecurityInfo Error %u", dwRes);
		}  
		ACLAmigo::setEntriesInACL(args);
		wchar_t wszObject[5000]; wcscpy_s(wszObject, args.wstrObject.c_str()); CYCLOPSDEBUG("wszObject = %S", wszObject);
		dwRes = ::SetNamedSecurityInfo(wszObject, args.objectType, DACL_SECURITY_INFORMATION, NULL, NULL, args.pNewDACL, NULL);
		if (ERROR_SUCCESS != dwRes)  {
			ErroroneousMonk err(dwRes);
			ACLAmigo::cleanup(args.pSecurityDescriptor, args.pNewDACL);
			CYCLOPS_THROW_EXCEPTION_IV(whoOps::ExceptionACLAmigo, "SetNamedSecurityInfo Error: %s", err.formatMessage().c_str());
		}  
	}

	void ACLAmigo::cleanup(PSECURITY_DESCRIPTOR pSD, PACL pACL) {
        if(pSD != NULL) 
            LocalFree((HLOCAL) pSD); 
        if(pACL != NULL) 
            LocalFree((HLOCAL) pACL); 
	}

	/* Creates a new ACL that merges the new ACE into the existing DACL. */
	void ACLAmigo::setEntriesInACL(ARGSS& args) { CYCLOPSDEBUG("Hello.");
		EXPLICIT_ACCESS ea;
		ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
		ea.grfAccessPermissions = args.dwAccessRights;
		ea.grfAccessMode = args.accessMode;
		ea.grfInheritance= args.dwInheritance;
		ea.Trustee.TrusteeForm = TRUSTEE_IS_NAME; 
		wchar_t wszAccount[5000]; wcscpy_s(wszAccount, args.wstrAccount.c_str()); CYCLOPSDEBUG("wszAccount = %S", wszAccount);
		ea.Trustee.ptstrName = wszAccount;
		DWORD dwRes = SetEntriesInAcl(1, &ea, args.pOldDACL, &args.pNewDACL);
		if (ERROR_SUCCESS != dwRes)  {
			ErroroneousMonk err(dwRes);
			cleanup(args.pSecurityDescriptor, args.pNewDACL);
			CYCLOPS_THROW_EXCEPTION_IV(whoOps::ExceptionACLAmigo, "SetEntriesInAcl() error for account %S: %s", wszAccount, err.formatMessage().c_str());
		}  
	}

	void ACLAmigo::revokeAccountFromShare(const std::wstring& wstrShare, const std::wstring wstrAccount) {
		CYCLOPSDEBUG("wstrShare/wstrAccount = %S/%S", wstrShare.c_str(), wstrAccount.c_str());
		ARGSS args = {};
		args.wstrObject = wstrShare;
		args.objectType = SE_LMSHARE;
		args.wstrAccount = wstrAccount;
		args.dwAccessRights = GENERIC_READ; /* Doesn't seem to matter what permission you use. */
		args.accessMode = REVOKE_ACCESS;
		args.dwInheritance = CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE; /* Also doesn't matter. */
		ACLAmigo::AddACEToDACLII(args);
	}
	void ACLAmigo::addAccountToShare(const std::wstring& wstrShare, const std::wstring wstrAccount, DWORD dwAccessRights) { 
		CYCLOPSDEBUG("wstrShare/wstrAccount = %S/%S", wstrShare.c_str(), wstrAccount.c_str());
		ARGSS args = {};
		args.wstrObject = wstrShare;
		args.objectType = SE_LMSHARE;
		args.wstrAccount = wstrAccount;
		args.dwAccessRights = dwAccessRights;
		args.accessMode = GRANT_ACCESS;
		args.dwInheritance = CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE;
		AddACEToDACLII(args);
	}

	void ACLAmigo::revokeAccountFromShare(const std::string& strShare, const std::string strAccount) {
		wstring wstrShare(strShare.begin(), strShare.end());
		wstring wstrAccount(strAccount.begin(), strAccount.end());
		revokeAccountFromShare(wstrShare, wstrAccount);
	}

	void ACLAmigo::addAccountToShare(const std::string& strShare, const std::string strAccount, DWORD dwPermission) {
		wstring wstrShare(strShare.begin(), strShare.end());
		wstring wstrAccount(strAccount.begin(), strAccount.end());
		addAccountToShare(wstrShare, wstrAccount, dwPermission);
	}


	/* From https://msdn.microsoft.com/en-us/library/windows/desktop/aa379283(v=vs.85).aspx */
	void ACLAmigo::addAceToObjectsSecurityDescriptor (
		const wstring& wstrPath,          // name of object
		SE_OBJECT_TYPE ObjectType,  // type of object
		const wstring& wstrTrustee,          // trustee for new ACE
		TRUSTEE_FORM TrusteeForm,   // format of trustee structure
		DWORD dwAccessRights,       // access mask for new ACE
		ACCESS_MODE AccessMode,     // type of ACE
		DWORD dwInheritance         // inheritance flags for new ACE
	) {
		PACL pOldDACL = NULL, pNewDACL = NULL;
		PSECURITY_DESCRIPTOR pSD = NULL;
		wchar_t* pwchPath = cyclOps::StringEmUp::new_wcharArray(wstrPath);
		DWORD dwRes = GetNamedSecurityInfoW(pwchPath, ObjectType, DACL_SECURITY_INFORMATION, NULL, NULL, &pOldDACL, NULL, &pSD);
		if (ERROR_SUCCESS != dwRes) {
			this->cleanup(pSD, pNewDACL);
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionACLAmigo, dwRes, cyclOps::Exception::TYPE::TYPE_WIN32, "GetNamedSecurityInfoW() failed for %S.", pwchPath);
		} else { CYCLOPSDEBUG("GetNamedSecurityInfoW() successful.");
			EXPLICIT_ACCESS ea;
			this->initializeEXPLICIT_ACCESS(ea, wstrTrustee, TrusteeForm, dwAccessRights, AccessMode, dwInheritance);
			dwRes = SetEntriesInAcl(1, &ea, pOldDACL, &pNewDACL);
			if (ERROR_SUCCESS != dwRes)  {
				this->cleanup(pSD, pNewDACL);
				CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionACLAmigo, dwRes, cyclOps::Exception::TYPE::TYPE_WIN32, 
					"SetEntriesInAcl() failed.");
			} else { CYCLOPSDEBUG("SetEntriesInAcl() successful.");
				dwRes = SetNamedSecurityInfoW(pwchPath, ObjectType, DACL_SECURITY_INFORMATION, NULL, NULL, pNewDACL, NULL);
				if (ERROR_SUCCESS != dwRes)  {
					this->cleanup(pSD, pNewDACL);
					CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionACLAmigo, dwRes, cyclOps::Exception::TYPE::TYPE_WIN32, 
						"SetNamedSecurityInfoW() failed for %S.", pwchPath);
				} else { CYCLOPSDEBUG("SetNamedSecurityInfoW() successful.");}
			}
		}
		delete[] pwchPath;
		this->cleanup(pSD, pNewDACL); CYCLOPSDEBUG("Goodbye.");
	}

	void ACLAmigo::initializeEXPLICIT_ACCESS(
		EXPLICIT_ACCESS& ea, 
		const wstring& wstrTrustee,          // trustee for new ACE
		TRUSTEE_FORM TrusteeForm,   // format of trustee structure
		DWORD dwAccessRights,       // access mask for new ACE
		ACCESS_MODE AccessMode,     // type of ACE
		DWORD dwInheritance         // inheritance flags for new ACE
	) {
		// Initialize an EXPLICIT_ACCESS structure for the new ACE. 
		ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
		ea.grfAccessPermissions = dwAccessRights;
		ea.grfAccessMode = AccessMode;
		ea.grfInheritance= dwInheritance;
		ea.Trustee.TrusteeForm = TrusteeForm;
		wchar_t wszTrustee[500]; CYCLOPS_SNWPRINTF_S(wszTrustee, L"%s", wstrTrustee.c_str());
		ea.Trustee.ptstrName = wszTrustee;
	}

}