#pragma once

#include <string>
#include <map>
#include <vector>

#include "WMIClass.h"

#include "..\cyclOps\cyclOps.h"


namespace whoOps {
	namespace wmi {
		class Win32_OperatingSystem :
			public whoOps::wmi::WMIClass
		{
		public:

			Win32_OperatingSystem()
			{
				this->addToVectorOfProperties(Name);
				this->addToVectorOfProperties(Version);
				this->addToVectorOfProperties(ServicePackMajorVersion);
			}

			~Win32_OperatingSystem()
			{
			}
			std::string getManufacturer(void) {
				return this->getStringProperty(Manufacturer);
			}
			std::string getModel(void) { CYCLOPSDEBUG("size = %d", this->getData().size());
				return this->getStringProperty(Model);
			}
		private:
			Win32_OperatingSystem(const Win32_OperatingSystem &);
			Win32_OperatingSystem operator=(const Win32_OperatingSystem&);

		};
	}
}