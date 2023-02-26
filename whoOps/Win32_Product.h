#pragma once


#include <string>
#include <map>
#include <vector>

#include "WMIClass.h"

#include "..\cyclOps\cyclOps.h"


namespace whoOps {
	namespace wmi {
		class Win32_Product :
			public whoOps::wmi::WMIClass
		{
		public:

			Win32_Product()
			{
				this->addToVectorOfProperties(Name);
				this->addToVectorOfProperties(InstallDate);
				this->addToVectorOfProperties(InstallDate2);
				this->addToVectorOfProperties(InstallLocation);
				this->addToVectorOfProperties(Description);
				this->addToVectorOfProperties(IdentifyingNumber);
				this->addToVectorOfProperties(InstallSource);
				this->addToVectorOfProperties(ProductID);
				this->addToVectorOfProperties(Vendor);
				this->addToVectorOfProperties(Version);
			}

			~Win32_Product()
			{
			}
		};
	}
};

