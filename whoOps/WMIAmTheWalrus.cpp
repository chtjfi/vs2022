#include "StdAfx.h"

/* I can't get the Winsock header file errors to go away!  See cyclOps.h. */
#include <WinSock2.h>

#include <exception>
#include <Windows.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

#include "..\whoOps\whoOpsBonanza.h"

# pragma comment(lib, "wbemuuid.lib")

using std::wstring;
using std::string;
using std::wstringstream;
using std::vector;
using std::map;

namespace whoOps {
	namespace wmi {
		WMIAmTheWalrus::WMIAmTheWalrus(void) /*:  _pLoc(NULL), _pSvc(NULL)*/{
		}


		WMIAmTheWalrus::~WMIAmTheWalrus(void) {
			this->cleanUpAndShutDown();
		}


		void WMIAmTheWalrus::CoInitializeEx(void) {
			/* http://msdn.microsoft.com/en-us/library/aa390885(v=vs.85).aspx */
			HRESULT hr;
			hr = ::CoInitializeEx(0, COINIT_MULTITHREADED);
			if (FAILED(hr)) {
				throw std::exception("CoInitializeEx() failed.");
			}
		}





		void WMIAmTheWalrus::CoInitializeSecurity(void)
		{
			/* http://msdn.microsoft.com/en-us/library/aa390885(v=vs.85).aspx */
			HRESULT hr = ::CoInitializeSecurity(
				NULL,                      // Security descriptor    
				-1,                        // COM negotiates authentication service
				NULL,                      // Authentication services
				NULL,                      // Reserved
				RPC_C_AUTHN_LEVEL_DEFAULT, // Default authentication level for proxies
				RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation level for proxies
				NULL,                        // Authentication info
				EOAC_NONE,                   // Additional capabilities of the client or server
				NULL);                       // Reserved

			if (FAILED(hr)) {
				std::stringstream ss;
				ss << "Failed to initialize security. Error code = 0x" << std::hex << hr << std::endl;
				if (hr == 0x80041002) {
					CYCLOPSWARNING("%s - I think this is ok.", ss.str().c_str());
				} else {
					CYCLOPS_THROW_EXCEPTION_V(cyclOps::Exception, hr, cyclOps::Exception::TYPE_HRESULT, "%s", ss.str().c_str());
				}
			}
		}


		IWbemServices*  WMIAmTheWalrus::createConnectionToWMINamespace(const wstring& wstrComputerName) const {
			/* http://msdn.microsoft.com/en-us/library/aa389749(v=vs.85).aspx */
			IWbemLocator *pLoc;
			HRESULT hr = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *) &pLoc);
			if (FAILED(hr)) {
				string error = whoOps::ErroroneousMonk::hresultToString(hr);
				CYCLOPS_THROW_EXCEPTION_IV(cyclOps::Exception, "Failed to create IWbemLocator object.  %s", error.c_str());
			}
			wchar_t wszURL[10000];
			_snwprintf_s(wszURL, sizeof(wszURL) / sizeof(wszURL[0]), L"\\\\%s\\root\\cimv2", wstrComputerName.c_str());
			IWbemServices* pSvc;
			hr = pLoc->ConnectServer(BSTR(wszURL), NULL, NULL, 0, NULL, 0, 0, &pSvc);
			if (pLoc) pLoc->Release();
			if (FAILED(hr)) 			{
				CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionUnableToConnect, hr, 
					cyclOps::Exception::TYPE_HRESULT, "ConnectServer() failed for %S", wstrComputerName.c_str());
			}
			return pSvc;
		}


		void WMIAmTheWalrus::setSecurityLevelsOnWMIConnection(IWbemServices* pSvc) const {

			/* http://msdn.microsoft.com/en-us/library/aa393619(v=vs.85).aspx */

			HRESULT hres;
			/* IWbemServices *pSvc = 0; */
			/* IWbemLocator *pLoc = 0; */

			// Set the proxy so that impersonation of the client occurs.
			hres = CoSetProxyBlanket(
				pSvc,
				RPC_C_AUTHN_WINNT,
				RPC_C_AUTHZ_NONE,
				NULL,
				RPC_C_AUTHN_LEVEL_CALL,
				RPC_C_IMP_LEVEL_IMPERSONATE,
				NULL,
				EOAC_NONE
				);

			if (FAILED(hres))
			{
				std::stringstream ss;
				ss << "Could not set proxy blanket. Error code = 0x" << std::hex << hres << std::endl;
				throw std::exception(ss.str().c_str());
			}


		}


		void WMIAmTheWalrus::cleanUpAndShutDown(void)
		{
			/* http://msdn.microsoft.com/en-us/library/aa389227(v=vs.85).aspx */
			// if (_pSvc) _pSvc->Release();
			// if (_pLoc) _pLoc->Release();
			/* if (_pEnumerator) _pEnumerator->Release();  */
			// CoUninitialize();
		}

		void WMIAmTheWalrus::initialize(void) {
			WMIAmTheWalrus::CoInitializeEx();
			WMIAmTheWalrus::CoInitializeSecurity(); 
		}

		void WMIAmTheWalrus::execQuery(const string& strHostname, whoOps::wmi::WMIClass& wmiClass) const {
			wstring wstrHost(strHostname.begin(), strHostname.end());
			wmiClass.setHostname(wstrHost);
			IWbemServices *pSvc = this->createConnectionToWMINamespace(wstrHost);
			this->setSecurityLevelsOnWMIConnection(pSvc);
			string strClass = wmiClass.toString(); CYCLOPSDEBUG("strClass= %s", strClass.c_str());
			wstring wstrClass(strClass.begin(), strClass.end());
			wstringstream wssQuery; wssQuery << L"SELECT * FROM " << wstrClass;
			IEnumWbemClassObject* pEnumeratorOfObjects;
			HRESULT hr = pSvc->ExecQuery(BSTR(L"WQL"), BSTR(wssQuery.str().c_str()), 
				WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
				NULL, &pEnumeratorOfObjects);
				IWbemClassObject* pmethodGetOwner = NULL;
			if (FAILED(hr)) {
				CYCLOPS_THROW_EXCEPTION_V(cyclOps::Exception, hr, cyclOps::Exception::TYPE_HRESULT, "ExecQuery() failed.");
			} else {
				this->populateWMIClassWithProperties(wmiClass, pEnumeratorOfObjects, pSvc);
			}
		}


/* typedef enum tag_CIMTYPE_ENUMERATION { 
  CIM_ILLEGAL     = 4095, // 0xFFF
  CIM_EMPTY       = 0,    // 0x0
  CIM_SINT8       = 16,   // 0x10
  CIM_UINT8       = 17,   // 0x11
  CIM_SINT16      = 2,    // 0x2
  CIM_UINT16      = 18,   // 0x12
  CIM_SINT32      = 3,    // 0x3
  CIM_UINT32      = 19,   // 0x13
  CIM_SINT64      = 20,   // 0x14
  CIM_UINT64      = 21,   // 0x15
  CIM_REAL32      = 4,    // 0x4
  CIM_REAL64      = 5,    // 0x5
  CIM_BOOLEAN     = 11,   // 0xB
  CIM_STRING      = 8,    // 0x8
  CIM_DATETIME    = 101,  // 0x65
  CIM_REFERENCE   = 102,  // 0x66
  CIM_CHAR16      = 103,  // 0x67
  CIM_OBJECT      = 13,   // 0xD
  CIM_FLAG_ARRAY  = 8192 // 0x2000
} CIMTYPE_ENUMERATION; */

		void throwExceptionFromGet(HRESULT hr, WMIClass& wmiClass, const wstring& wstrProp) {
			if (hr == 0x80041002) {
				CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionNoSuchProperty, hr, cyclOps::Exception::TYPE_HRESULT, 
					"The host '%S' has no property '%S'.  "
					"This is most commonly seen on Windows 2003 with the logical processors property.", 
					wmiClass.getHostnameW().c_str(), wstrProp.c_str());
			} else {
				CYCLOPS_THROW_EXCEPTION_V(cyclOps::Exception, hr, cyclOps::Exception::TYPE_HRESULT, 
					"Get() failed for property %S on %S.", wstrProp.c_str(), 
					wmiClass.getHostnameW().c_str());
			}
		
		}

		void WMIAmTheWalrus::addPropertyToMap(IWbemClassObject* pclsObj, const WMIClass::PROPERTY& prop, 
				WMIPropertyMap_t& mapOfPropsToVariants,
				WMIClass& wmiClass) const {
			VARIANT vtProp;
			string strProp = wmiClass.enumToString(prop); 
			wstring wstrProp(strProp.begin(), strProp.end()); CYCLOPSDEBUG("wmiClass/wstrProp = %s/%S", wmiClass.toString().c_str(), wstrProp.c_str());
			/* https://msdn.microsoft.com/en-us/library/aa386309(v=vs.85).aspx */
			CIMTYPE cimtype;
			HRESULT hr = pclsObj->Get(wstrProp.c_str(), 0, &vtProp, &cimtype, 0);  
			if (FAILED(hr)) {
				whoOps::wmi::throwExceptionFromGet(hr, wmiClass, wstrProp);
			} CYCLOPSDEBUG("type/cimtype is %d/%d", vtProp.vt, cimtype);
			whoOps::wmi::Variant variant;
			variant.setType(vtProp.vt); 
			switch (vtProp.vt) { 
			/* http://stackoverflow.com/questions/6492865/iwbemclassobjectnext-yields-vt-null-types-when-they-are-not-what-does-this-me */
			case VT_NULL:
				/* This happens when, for example, you try to get the size of uninserted media (floppy/DVD). */
				break;
			case VT_BSTR:
				variant.setStringValueW(vtProp.bstrVal); CYCLOPSDEBUG("bstrVal = %S", vtProp.bstrVal);
				break;
			case VT_I4:
				variant.setUintValue(vtProp.uintVal);
				break;
			default:
				CYCLOPS_THROW_EXCEPTION_IV(cyclOps::Exception, "Unrecognized variant type: %d", vtProp.vt);
			}
			mapOfPropsToVariants[prop] = variant;
		}

		void WMIAmTheWalrus::populateWMIClassWithProperties(whoOps::wmi::WMIClass& wmiClass, 
			IEnumWbemClassObject* pEnumeratorOfObjects, IWbemServices *pSvc) const { CYCLOPSDEBUG("Hello.");
			const vector<WMIClass::PROPERTY>& properties = wmiClass.getPropertiesAsVectorOfEnum();
			ULONG uReturn = 0;
			int i = 0;
			while (pEnumeratorOfObjects) {
				IWbemClassObject *pclsObj = NULL;
				HRESULT hr = pEnumeratorOfObjects->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
				if (FAILED(hr)) {
					CYCLOPS_THROW_EXCEPTION_V(cyclOps::Exception, hr, cyclOps::Exception::TYPE_HRESULT, 
						"Next() failed for WMIClass '%s' on host '%S' on iteration %d (0-based).", 
						wmiClass.toString().c_str(), wmiClass.getHostnameW().c_str(), i);
				}
				++i;
				if (0 == uReturn) {
					break;
				}
				whoOps::wmi::WMIClass::WMI_PROPERTY_MAP mapOfPropNamesToVariants;
				for (int i = 0; i < properties.size(); ++i) { 
					const WMIClass::PROPERTY& prop = properties[i]; CYCLOPSDEBUG("prop/enum = %d/%s", prop, 
						wmiClass.enumToString(prop).c_str());
					try {
						this->addPropertyToMap(pclsObj, prop, mapOfPropNamesToVariants.map, wmiClass);
					} catch (const whoOps::ExceptionNoSuchProperty& e) {
						CYCLOPSERROR("%s", e.getMessage().c_str());
					}
				}
				try {
					wmiClass.getMethodBasedProperties(pclsObj, pSvc, mapOfPropNamesToVariants);
				} catch (const cyclOps::ExceptionNoSuchEntry& e) {
					CYCLOPSERROR("%s", e.getMessage().c_str());
				}
				wmiClass.addObjectWithProperties(mapOfPropNamesToVariants);
				if (pclsObj) {
					pclsObj->Release();
				}
			}
			pEnumeratorOfObjects->Release();
		}

		void WMIAmTheWalrus::populateAllServerProperties(whoOps::Server& server) const {
			this->populateAllSingleLevelProperties(server);
			this->populateAllDoubleLevelProperties(server); 
		}

		void WMIAmTheWalrus::populateAllSingleLevelProperties(whoOps::Server& server) const {
			vector<WMIClass*> vectorOfWMIClass;
			whoOps::wmi::Win32_ComputerSystem temp;
			vectorOfWMIClass.push_back(&temp);
			whoOps::wmi::Win32_OperatingSystem temp2;
			vectorOfWMIClass.push_back(&temp2);
			whoOps::wmi::Win32_Processor temp3;
			vectorOfWMIClass.push_back(&temp3);
			whoOps::wmi::Win32_BIOS temp4;
			vectorOfWMIClass.push_back(&temp4);
			for (int i = 0; i < vectorOfWMIClass.size(); ++i) {
				whoOps::wmi::WMIClass* pWmiClass = vectorOfWMIClass[i];
				this->execQuery(server.getHostname(), *pWmiClass);
				vector<WMIClass::PROPERTY> props = pWmiClass->getPropertiesAsVectorOfEnum();
				string wmiClassName = pWmiClass->toString();
				for (int i = 0; i < props.size(); ++i) {
					whoOps::wmi::Variant variant = pWmiClass->getPropertyAsVariant(props[i]);
					string propertyName = pWmiClass->toString() + "." + pWmiClass->enumToString(props[i]); CYCLOPSDEBUG("propertyName = %s", propertyName.c_str());
					server.putProperty(propertyName, variant);
				}
			}
		}

		void WMIAmTheWalrus::populateAllDoubleLevelProperties(whoOps::Server& server) const {
			whoOps::wmi::Win32_LogicalDisk temp1;
			this->execQuery(server.getHostname(), temp1);
			server.setLogicalDisk(temp1); 
		}

	}
}