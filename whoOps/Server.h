#pragma once
#include  <string>
#include <map>

#include "..\cyclOps\cyclOps.h"

#include "WMIClass.h"
#include "Win32_LogicalDisk.h"

#include "Variant.h"

namespace whoOps {
	class Server
	{
	public:
		Server(std::string hostname) : _hostname(hostname) { }
		~Server() {	}
		/* makeSettersAndGetters.sh */
		whoOps::wmi::Variant getPropertyAsVariant(const std::string& prop) const {
			return _mapOfProperties.at(prop);
		}
		std::string getPropertyAsString(const std::string& prop) const {
			whoOps::wmi::Variant variant = this->getPropertyAsVariant(prop);
			return variant.getValueAsString();
		}
		void setProcessorManufacturer(const std::string& s) { _processorManufacturer = s; }
		std::string getProcessorManufacturer(void) { return _processorManufacturer; }
		void setSockets(int s) { _sockets = s; }
		int getSockets(void) { return _sockets; }
		void setServicePackMajorVersion(const std::string& s) { _servicePackMajorVersion = s; }
		std::string getServicePackMajorVersion(void) { return _servicePackMajorVersion; }
		void setOperatingSystemVersion(const std::string& s) { _operatingSystemVersion = s; }
		std::string getOperatingSystemVersion(void) { return _operatingSystemVersion; }
		void setBiosReleaseDate(const std::string& s) { _biosReleaseDate = s; }
		std::string getBiosReleaseDate(void) { return _biosReleaseDate; }
		void setSerialNumber(const std::string& s) { _serialNumber = s; }
		std::string getSerialNumber(void) { return _serialNumber; }
		void setMaxClockSpeed(int s) { _maxClockSpeed = s; }
		int getMaxClockSpeed(void) { return _maxClockSpeed; }
		void setMemoryInBytes(unsigned long s) { _memoryInBytes = s; }
		unsigned long getMemoryInBytes(void) { return _memoryInBytes; }
		int getMemoryInGigabytes(void) const { 
			whoOps::wmi::Variant variant = _mapOfProperties.at("Win32_ComputerSystem.TotalPhysicalMemory"); 
			std::wstring strMem = variant.getStringValueW();
			long long ulBytes = cyclOps::StringEmUp::toLongLong(strMem);
			float flGigs = (float) ulBytes / (float) (1024 * 1024 * 1024);
			int iMem = (int) flGigs + 0.5;
			return iMem;
		}
		void setHostname(const std::string& s) { _hostname = s; }
		std::string getHostname(void) const { return _hostname; }
		void setModel(const std::string& s) { _model = s; }
		std::string getModel(void) const { CYCLOPSDEBUG("Hello.");
			whoOps::wmi::Variant variant = _mapOfProperties.at("Win32_ComputerSystem.Model"); 
			std::string s = variant.getStringValueA();		
			return s;
		}
		void setManufacturer(const std::string& s) { _manufacturer = s; }
		std::string getManufacturer(void) { return _manufacturer; }
		void setOperatingSystem(const std::string& s) { _operatingSystem = s; }
		std::string getOperatingSystem(void) const { 
			whoOps::wmi::Variant variant = _mapOfProperties.at("Win32_OperatingSystem.Name"); 
			std::string s = variant.getStringValueA();
			std::vector<std::string> v = cyclOps::StringEmUp::split(s, '|');
			std::string os = v[0];
			cyclOps::StringEmUp::trim(os);
			return os;
		}
		void setCores(int s) { _cores = s; }
		unsigned int getCores(void) const { 
			whoOps::wmi::Variant variant = _mapOfProperties.at("Win32_ComputerSystem.NumberOfLogicalProcessors"); 
			return variant.getUintValue();
		}
		void setProcessorName(const std::string& s) { _processorName = s; }
		std::string getProcessorName(void) const { 
			whoOps::wmi::Variant variant = _mapOfProperties.at("Win32_Processor.Name");
			return variant.getStringValueA();
		}
		void putProperty(const std::string& propertyName, whoOps::wmi::Variant& value) { CYCLOPSDEBUG("propertyName/value = %s/%u\n", propertyName.c_str(), value.getUintValue());
			_mapOfProperties[propertyName] = value; 
		}
		void setLogicalDisk(const whoOps::wmi::Win32_LogicalDisk& disk) {
			_disk = disk;
		}
		void getLogicalDisks(std::vector<whoOps::LogicalDisk>& disks) const {
			_disk.getLogicalDisks(disks);
		}
		unsigned long getFreeSpaceOnDriveInGB(const std::string& drive) { 
			return this->_disk.getFreeSpaceOnDriveInGB(drive);
		}
	private:
		/* This holds single level properties like hostname, model. */
		std::map<std::string, whoOps::wmi::Variant> _mapOfProperties;
		/* This holds collections of objects like disks. It's redundant because it holds all the info in Win32_LogicalDisk. */
		whoOps::wmi::Win32_LogicalDisk _disk;
		std::string _processorManufacturer;
		int			_sockets;
		std::string _servicePackMajorVersion;
		std::string _operatingSystemVersion;
		std::string _biosReleaseDate;
		std::string _serialNumber;
		int			_maxClockSpeed;
		unsigned long _memoryInBytes;
		std::string _hostname;
		std::string _model;
		std::string _manufacturer;
		std::string _operatingSystem;
		int			_cores;
		std::string _processorName;

	};
}