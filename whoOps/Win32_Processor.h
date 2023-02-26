#pragma once

#include <string>
#include <map>
#include <vector>

#include "WMIClass.h"

#include "..\cyclOps\cyclOps.h"


namespace whoOps {
	namespace wmi {
		class Win32_Processor :
			public whoOps::wmi::WMIClass
		{
		public:

			Win32_Processor()
			{
/*"MaxClockSpeed", "Name", "Manufacturer"*/
				this->addToVectorOfProperties(Name);
				this->addToVectorOfProperties(MaxClockSpeed);
				this->addToVectorOfProperties(Manufacturer);
			}

			~Win32_Processor()
			{
			}
			std::string getManufacturer(void) {
				return this->getStringProperty(Manufacturer);
			}
			std::string getModel(void) { CYCLOPSDEBUG("size = %d", this->getData().size());
				return this->getStringProperty(Model);
			}
		private:
			Win32_Processor(const Win32_Processor &);
			Win32_Processor operator=(const Win32_Processor&);

		};
	}
}