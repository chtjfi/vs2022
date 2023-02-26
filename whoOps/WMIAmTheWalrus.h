#pragma once

#include <string>

#define _WIN32_DCOM
#include <wbemidl.h>

#include "..\cyclOps\cyclOps.h"

#include "whoOpsBonanza.h"
#include "Server.h"
#include "WMIClass.h"

namespace whoOps {
	namespace wmi {
	class WMIAmTheWalrus
	{
	public:
		explicit WMIAmTheWalrus(void);
		~WMIAmTheWalrus(void);
		/* Call this only once at exe entry point. */
		static void initialize(void);
		/* void connect(const std::string& host); */
		std::string getPropertyForFirstObjectOnly(const std::string& property);
		void cleanUpAndShutDown(void);
		void execQuery(const std::string& host, whoOps::wmi::WMIClass& wmiClass) const;
		void populateAllServerProperties(whoOps::Server& server) const;

		typedef std::map<whoOps::wmi::WMIClass::PROPERTY, whoOps::wmi::Variant> WMIPropertyMap_t;
	private:
		WMIAmTheWalrus(const WMIAmTheWalrus &);
		WMIAmTheWalrus operator=(const WMIAmTheWalrus&);
		// IWbemLocator *_pLoc;
		// IWbemServices *_pSvc;
		/* IEnumWbemClassObject* _pEnumerator; */
		static void CoInitializeEx(void);
		static void CoInitializeSecurity(void);
		IWbemServices* createConnectionToWMINamespace(const std::wstring& computer) const;
		void setSecurityLevelsOnWMIConnection(IWbemServices* pSvc) const ;
		std::string toString(const whoOps::wmi::WMIClass& wmiClass);
		void populateWMIClassWithProperties(whoOps::wmi::WMIClass& wmiClass, IEnumWbemClassObject* pEnumerator,
			IWbemServices *pSvc) const;
		void addPropertyToMap(
			::IWbemClassObject* pclsObj, 
			const whoOps::wmi::WMIClass::PROPERTY& prop, 
			WMIPropertyMap_t& mapOfPropsToVariants,
			whoOps::wmi::WMIClass& wmiClass) const ;
		void populateAllSingleLevelProperties(whoOps::Server& server) const;
		void populateAllDoubleLevelProperties(whoOps::Server& server) const;
	};
	}
}
