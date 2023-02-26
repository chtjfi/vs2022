#pragma once

#include <string>
#include <map>
#include <vector>

#include "WMIClass.h"

#include "..\cyclOps\cyclOps.h"


namespace whoOps {
	namespace wmi {
		class Win32_BIOS :
			public whoOps::wmi::WMIClass
		{
		public:

			Win32_BIOS()
			{
				this->addToVectorOfProperties(SerialNumber);
				this->addToVectorOfProperties(ReleaseDate);
			}

			~Win32_BIOS()
			{
			}
			std::string getManufacturer(void) {
				return this->getStringProperty(Manufacturer);
			}
			std::string getModel(void) { CYCLOPSDEBUG("size = %d", this->getData().size());
				return this->getStringProperty(Model);
			}
		private:
			Win32_BIOS(const Win32_BIOS &);
			Win32_BIOS operator=(const Win32_BIOS&);
		};
	}
}