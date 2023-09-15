#pragma once

#include <vector>
#include <map>
#include <string>

#define _WIN32_DCOM
#include <wbemidl.h>

#include "..\cyclOps\cyclOps.h"

#include "Variant.h"


namespace whoOps {
	namespace wmi {
		class WMIClass {
		public:
			WMIClass() { CYCLOPSDEBUG("Constructing a fresh WMIClass.");  }
			~WMIClass() { }
			CYCLOPS_ENUM(PROPERTY,
					Name, 
					Model, 
					Manufacturer, 
					TotalPhysicalMemory, 
					NumberOfProcessors, 
					NumberOfLogicalProcessors, 
					Version, 
					ServicePackMajorVersion,
					MaxClockSpeed,
					SerialNumber,
					ReleaseDate, 
					FreeSpace,
					Size,
					DeviceID,
					DriveType,
					InstallDate,
					InstallDate2,
					InstallLocation,
					Description,
					IdentifyingNumber,
					InstallSource,
					ProductID,
					Vendor,
					ProcessID,
					CommandLine,
					CreationDate,
					ExecutablePath,
					Owner /* A method-based property of Win32_Process. */
				)
			/* https://msdn.microsoft.com/en-us/library/074af4b6.aspx */
			/* http://stackoverflow.com/a/22052790/502556 */
			struct WMI_PROPERTY_MAP { 
				std::map<whoOps::wmi::WMIClass::PROPERTY, whoOps::wmi::Variant> map; 
			};
			typedef std::vector<WMI_PROPERTY_MAP> WMI_DATA;
			virtual void dummyToMakeClassVirtual() { };
			virtual void getMethodBasedProperties(IWbemClassObject *pclsObj, IWbemServices *pSvc,
				whoOps::wmi::WMIClass::WMI_PROPERTY_MAP& mapOfPropNamesToVariants) { /* Override this.  See Win32_Process. */ };
			std::vector<PROPERTY> getPropertyNamesBetween(PROPERTY begin, PROPERTY end);
			std::vector<WMIClass::PROPERTY> getPropertiesAsVectorOfEnum(void) { return _properties; }
			void addObjectWithProperties(WMI_PROPERTY_MAP obj) { _data.push_back(obj); }
			std::vector<std::wstring> getPropertyNamesAsVectorOfString(void);
			std::string getStringProperty(WMIClass::PROPERTY property);
			whoOps::wmi::Variant getPropertyAsVariant(WMIClass::PROPERTY property);
			void addToVectorOfProperties(WMIClass::PROPERTY prop) { _properties.push_back(prop); }
			std::string toString(void);
			size_t getDataSize() const { return _data.size(); }
			WMI_PROPERTY_MAP getDataItem(int i) const { return _data[i]; }
			void setHostname(const std::wstring& h) { _wstrHostname.assign(h); }
			std::wstring getHostnameW(void) const { return _wstrHostname; }
			std::string getHostnameA(void) { std::string hostname(_wstrHostname.begin(), _wstrHostname.end()); return hostname; }
		protected:
			const WMI_DATA& getData(void) { return _data;  }
		private:
			WMI_DATA _data;
			std::vector<WMIClass::PROPERTY> _properties;
			std::wstring _wstrHostname;
		};

	}
}