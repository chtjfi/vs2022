#pragma once

#include <string>
#include <map>
#include <vector>

#include "WMIClass.h"

#include "..\cyclOps\cyclOps.h"


namespace whoOps {
	namespace wmi {
		class Win32_ComputerSystem :
			public whoOps::wmi::WMIClass
		{
		public:

			Win32_ComputerSystem()
			{
				this->addToVectorOfProperties(Name);
				this->addToVectorOfProperties(Model);
				this->addToVectorOfProperties(Manufacturer);
				this->addToVectorOfProperties(TotalPhysicalMemory);
				this->addToVectorOfProperties(NumberOfProcessors);
				this->addToVectorOfProperties(NumberOfLogicalProcessors);
			}

			~Win32_ComputerSystem()
			{
			}
			std::string getManufacturer(void) {
				return this->getStringProperty(Manufacturer);
			}
			std::string getModel(void) { CYCLOPSDEBUG("size = %d", this->getData().size());
				return this->getStringProperty(Model);
			}
			int getCores(void) {
				throw std::runtime_error("Not implemented.");
			}
		private:
			Win32_ComputerSystem(const Win32_ComputerSystem &);
			Win32_ComputerSystem operator=(const Win32_ComputerSystem&);

		};
	}
}