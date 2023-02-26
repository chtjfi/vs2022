#pragma once

#include <string>

#include <wbemidl.h>

namespace whoOps {
	namespace wmi {
	class Variant
	{
	public:
		Variant() : _uintValue(0), _isTypeSet(false), _type(VT_EMPTY)  { }
		~Variant() { }
		/*  Look at VARENUM::XXX for the types via Intellisense. */
		void setStringValueW(const std::wstring& s) { _wstrValue = s; }
		std::wstring getStringValueW(void) { 
			/* VARENUM::VT_BSTR */ 
			return _wstrValue;  
		}
		std::string getStringValueA(void) {
			return std::string(_wstrValue.begin(), _wstrValue.end());
		}
		void setUintValue(::UINT i) { CYCLOPSDEBUG("Setting _uintValue to %d", i);
			_uintValue = i;  
		}
		::UINT getUintValue(void) { 
			/* VARENUM::VT_I4 */
			return _uintValue; 
		}
		void setType(::VARTYPE type) { 
			_type = type;   
			_isTypeSet = true;
		}
		::VARTYPE getType(void) { 
			/* Returns VT_NULL for Size/FreeSpace properties of uninserted floppy/DVD. */ 
			return _type; 
		}
		boolean isNullType(void) { return _type == ::VT_NULL; }
		boolean isTypeSet(void) { return _isTypeSet; }
		std::string getValueAsString(void) { CYCLOPSDEBUG("_type/VT_BSTR/VT_I4 = %u/%u/%u", _type, VT_BSTR, VT_I4);
			std::stringstream ss;
			switch (_type) {
			case VT_BSTR:
				return std::string(_wstrValue.begin(), _wstrValue.end());
				break;
			case VT_I4:
				ss << _uintValue;
				return ss.str();
			default:
				ss << "Not a recognized VARENUM type. " << __FILE__ << "(" << __LINE__ << ")";
				throw std::exception(ss.str().c_str());
			}
		}
	private:
		std::wstring _wstrValue;
		::UINT _uintValue;
		::VARTYPE _type;
		boolean _isTypeSet;
	};

}
}
