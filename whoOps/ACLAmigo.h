#pragma once

#include <string>

#include <Windows.h>
#include <AccCtrl.h>
#include <Aclapi.h>

namespace whoOps {
	class ACLAmigo
	{
	public:
		ACLAmigo(void);
		~ACLAmigo(void);
		/* This is deprecated sorta.  Use addPermissionsToFileOrFolder(). */
		void addPermissions(const std::string& strPath, const char* szSecurityPrincipal, const char* szPermission);
		void addPermissionsToFileOrFolder(const std::wstring& wstrPath, const std::string& strTrustee, const char* szPermission);
		static void revokeAccountFromShare(const std::wstring& wstrShare, const std::wstring wstrAccount);
		static void addAccountToShare(const std::wstring& wstrShare, const std::wstring wstrAccount, DWORD dwPermission);
		static void revokeAccountFromShare(const std::string& strShare, const std::string strAccount);
		static void addAccountToShare(const std::string& strShare, const std::string strAccount, DWORD dwPermission);
		static const ACCESS_MASK _STANDARD_RIGHTS_ALL = STANDARD_RIGHTS_ALL;
		static const ACCESS_MASK _FILE_GENERIC_EXECUTE = FILE_GENERIC_EXECUTE;
		static const ACCESS_MASK _FILE_GENERIC_READ = FILE_GENERIC_READ;
		static const ACCESS_MASK _FILE_GENERIC_WRITE = FILE_GENERIC_WRITE;
		static const ACCESS_MASK _DELETE = DELETE;
		static const ACCESS_MASK _SPECIFIC_RIGHTS_ALL = SPECIFIC_RIGHTS_ALL;
		static const ACCESS_MASK _READ_CONTROL = READ_CONTROL;
		static const ACCESS_MASK ACCESS_MASK_SHARE_READ = 0x001200a9;
		static const ACCESS_MASK ACCESS_MASK_SHARE_FULL = 0x001f01ff;
		static const ACCESS_MASK ACCESS_MASK_SHARE_CHANGE = 0x001301bf;
	private:
		typedef struct args { 
			std::wstring wstrObject;
			SE_OBJECT_TYPE objectType;
			std::wstring wstrAccount;
			DWORD dwAccessRights;
			ACCESS_MODE accessMode;
			DWORD dwInheritance;
			PACL pOldDACL;
			PACL pNewDACL;
			PSECURITY_DESCRIPTOR pSecurityDescriptor;
		} ARGSS;
		static void AddACEToDACLII (ARGSS& args);
		/* Don't make this next guy public because it does not handle traversal. */
		void AddACEToDACL(const std::string& strPath, const char* szSecurityPrincipal, const DWORD dwPermission);
		int ResolveSID(const char* szSecurityPrinciple, const PSID O_pSID);
		bool isDirectory(const std::string& strPath);
		bool isDirectory(const std::wstring& wstrPath);
		void addPermissionsToChildren(const std::string& strPath, const char* szSecurityPrincipal, const DWORD dwPermission);
		void addPermissionsToChildrenII(const std::wstring& strPath, const std::string& szSecurityPrincipal, const DWORD dwPermission);
		void addPermissions(const std::string& strPath, const char* szSecurityPrincipal, const DWORD dwPermission);
		void addPermissionsToFileOrFolder(const std::wstring& strPath, const std::string& strTrustee, const DWORD dwPermission);
		static void cleanup(PSECURITY_DESCRIPTOR pSD, PACL pACL);
		static void setEntriesInACL(ARGSS& args); 
		DWORD getPermission(const char* szPermission);
		void addAceToObjectsSecurityDescriptor (
			const std::wstring& wstrPath,          // name of object
			SE_OBJECT_TYPE ObjectType,  // type of object
			const std::wstring& wstrTrustee,          // trustee for new ACE
			TRUSTEE_FORM TrusteeForm,   // format of trustee structure
			DWORD dwAccessRights,       // access mask for new ACE
			ACCESS_MODE AccessMode,     // type of ACE
			DWORD dwInheritance         // inheritance flags for new ACE
		);
		void initializeEXPLICIT_ACCESS(
			EXPLICIT_ACCESS& ea, 
			const std::wstring& wstrTrustee,          // trustee for new ACE
			TRUSTEE_FORM TrusteeForm,   // format of trustee structure
			DWORD dwAccessRights,       // access mask for new ACE
			ACCESS_MODE AccessMode,     // type of ACE
			DWORD dwInheritance         // inheritance flags for new ACE
		);
	};
}