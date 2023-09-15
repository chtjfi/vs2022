#pragma once

#include <string>
#include <map>
#include <vector>

#include "..\cyclOps\cyclOps.h"

#include "WMIClass.h"
#include "LogicalDisk.h"

namespace whoOps {
	namespace wmi {
		class Win32_LogicalDisk :
			public whoOps::wmi::WMIClass
		{
		public:

			Win32_LogicalDisk()
			{
				/* "FreeSpace", "Size", "DeviceID", "DriveType" */
				this->addToVectorOfProperties(DeviceID);
				this->addToVectorOfProperties(DriveType);
				this->addToVectorOfProperties(Size);
				this->addToVectorOfProperties(FreeSpace);
			}

			~Win32_LogicalDisk()
			{
			}
			std::string getDeviceID(void) {
				return this->getStringProperty(DeviceID);
			}
			unsigned long getFreeSpaceOnDriveInGB(const std::string& drive) { 
				for (int i = 0; i < this->getDataSize(); ++i) {
					whoOps::wmi::WMIClass::WMI_PROPERTY_MAP mapOfPropertiesToVariants = this->getDataItem(i);
					whoOps::wmi::Variant& variantDeviceID = mapOfPropertiesToVariants.map[whoOps::wmi::WMIClass::DeviceID];
					std::string strDriveLetter = variantDeviceID.getStringValueA(); CYCLOPSDEBUG("drive/strDriveLetter = %s/%s", drive.c_str(), strDriveLetter.c_str());
					if (cyclOps::StringEmUp::compareIgnoreCase(drive, strDriveLetter) == 0) {
						std::wstring wstrFreeSpace = mapOfPropertiesToVariants.map[whoOps::wmi::WMIClass::FreeSpace].getStringValueW(); CYCLOPSDEBUG("wstrFreeSpace = '%S'", wstrFreeSpace.c_str());
						unsigned long long ulSize = std::stoll(wstrFreeSpace); CYCLOPSVAR(ulSize, "%lu");
						long lSize = (long) ulSize / (1024 * 1024 * 1024);
						return lSize;
					}
				}
				throw std::exception("No such drive.");
			}

			void getLogicalDisks(std::vector<whoOps::LogicalDisk>& logicalDisks) const {
				for (int i = 0; i < this->getDataSize(); ++i) {
					whoOps::wmi::WMIClass::WMI_PROPERTY_MAP mapRepresentingDisk = this->getDataItem(i);
					whoOps::wmi::Variant variantForSize = mapRepresentingDisk.map[whoOps::wmi::WMIClass::Size];
					/* If the value of the Size property was VT_NULL this is a removable drive. */
					if ( ! variantForSize.isNullType()) {
						whoOps::LogicalDisk disk;
						disk.setHostname(this->getHostnameW());
						const std::wstring& wstrSizeInBytes = variantForSize.getStringValueW();
						disk.setSizeInBytes(wstrSizeInBytes);
						const std::wstring& wstrDeviceID = mapRepresentingDisk.map[whoOps::wmi::WMIClass::DeviceID].getStringValueW();
						disk.setDeviceID(wstrDeviceID);
						const std::wstring& wstrFreeSpaceInBytes = mapRepresentingDisk.map[whoOps::wmi::WMIClass::FreeSpace].getStringValueW(); 
						disk.setFreeSpaceInBytes(wstrFreeSpaceInBytes);
						unsigned int iDriveType = mapRepresentingDisk.map[whoOps::wmi::WMIClass::DriveType].getUintValue();
						disk.setDriveType(iDriveType);
						logicalDisks.push_back(disk);
					}
				}
			}

		};
	}
};

