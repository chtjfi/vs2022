#include "StdAfx.h"

#include "..\cyclOps\cyclOps.h"

#include "WMIClass.h"

using std::string;
using std::wstring;
using std::vector;
using std::map;

namespace whoOps {
	namespace wmi {

		string WMIClass::toString(void) {
			string strFullClass = typeid(*this).name();  CYCLOPSDEBUG("strFullClass = %s", strFullClass.c_str());
			return cyclOps::StringEmUp::substringAfterLastOccurrenceOfCharacter(strFullClass, ':');
		}


		vector<WMIClass::PROPERTY> WMIClass::getPropertyNamesBetween(WMIClass::PROPERTY begin, WMIClass::PROPERTY end) { CYCLOPSDEBUG("begin = %d", begin);
			vector<WMIClass::PROPERTY> vectorOfProps;
			for (int i = begin + 1; i < end; ++i) { CYCLOPSDEBUG("i = %d", i);
				vectorOfProps.push_back(static_cast<WMIClass::PROPERTY>(i));
			}
			return vectorOfProps;
		}
		vector<wstring> WMIClass::getPropertyNamesAsVectorOfString(void) {
			std::vector<std::wstring> vectorOfStrings;
			std::vector<WMIClass::PROPERTY> vectorOfProps = this->getPropertiesAsVectorOfEnum();
			for (int i = 0; i < vectorOfProps.size(); i++) {
				std::string s = this->enumToString(vectorOfProps[i]);
				std::wstring wstrProperty(s.begin(), s.end());
				vectorOfStrings.push_back(wstrProperty);
			}
			return vectorOfStrings;
		}
		string WMIClass::getStringProperty(WMIClass::PROPERTY property) {
			WMI_PROPERTY_MAP props = this->getData()[0];
			std::string strProperty = this->enumToString(property);
			std::wstring wstrProperty(strProperty.begin(), strProperty.end());
			Variant variant = props.map[property];
			std::wstring wstrValue = variant.getStringValueW();
			return string(wstrValue.begin(), wstrValue.end());
		}
		whoOps::wmi::Variant WMIClass::getPropertyAsVariant(WMIClass::PROPERTY property) {
			WMI_PROPERTY_MAP mapOfPropsToVars = this->getData()[0];
			return mapOfPropsToVars.map[property];
		}
	}
}