#pragma once

#include <string>
#include <map>
#include <vector>

#include "whoOpsBonanza.h"
#include "..\cyclOps\cyclOps.h"

#include "WMIClass.h"

namespace whoOps {
	namespace wmi {
		class Win32_Process :
			public whoOps::wmi::WMIClass
		{
		public:

			Win32_Process()
			{
				this->addToVectorOfProperties(Name);
				this->addToVectorOfProperties(ProcessID);
				this->addToVectorOfProperties(CommandLine);
				this->addToVectorOfProperties(CreationDate);
				this->addToVectorOfProperties(ExecutablePath);
			}

			~Win32_Process()
			{
			}
			void getMethodBasedProperties(IWbemClassObject *classObjectForThisIteration, IWbemServices *pSvc,
				whoOps::wmi::WMIClass::WMI_PROPERTY_MAP& mapOfPropNamesToVariants) 
			{ 
				BSTR MethodName = SysAllocString(L"GetOwner");
				BSTR ClassName = SysAllocString(L"Win32_Process");
				IWbemClassObject* pClassForObtainingMethod = NULL;
				HRESULT hres = pSvc->GetObject(ClassName, 0, NULL, &pClassForObtainingMethod, NULL);
				if (FAILED(hres)) {
					CYCLOPS_THROW_EXCEPTION_V(cyclOps::Exception, hres, cyclOps::Exception::TYPE_HRESULT, "GetObject() failed.");
				}
				IWbemClassObject* pmethodGetOwner = NULL;
				hres = pClassForObtainingMethod->GetMethod(MethodName, 0, NULL, &pmethodGetOwner);
				if (FAILED(hres)) {
					CYCLOPS_THROW_EXCEPTION_V(cyclOps::Exception, hres, cyclOps::Exception::TYPE_HRESULT, "GetMethod() failed.");
				}
				IWbemClassObject* pInInst = NULL;
				hres = pmethodGetOwner->SpawnInstance(0, &pInInst);
				if (FAILED(hres)) {
					CYCLOPS_THROW_EXCEPTION_V(cyclOps::Exception, hres, cyclOps::Exception::TYPE_HRESULT, "SpawnInstance() failed.");
				}
				VARIANT vtProp2; 
				// Get the PATH to the object in question the result in vtProp is similar to '\\name_of_computer\ROOT\CIMV2:Win32_Process.Handle="pid_of_process"'
				hres = classObjectForThisIteration->Get(L"__PATH", 0, &vtProp2, 0, 0); CYCLOPSDEBUG("%S", V_BSTR(&vtProp2));
				if (FAILED(hres)) {
					CYCLOPS_THROW_EXCEPTION_V(cyclOps::Exception, hres, cyclOps::Exception::TYPE_HRESULT, "Get() failed.");
				}
				hres = pSvc->ExecMethod(vtProp2.bstrVal, L"GetOwner", 0, NULL, NULL, &pmethodGetOwner, NULL);
				if (FAILED(hres)) {
					CYCLOPS_THROW_EXCEPTION_V(cyclOps::ExceptionNoSuchEntry, hres, cyclOps::Exception::TYPE_HRESULT, "ExecMethod() failed getting GetOwner().");
				}
				VARIANT vtDomain; 
				VARIANT vtUsername;					
				hres = pmethodGetOwner->Get(L"User", 0, &vtUsername, NULL, 0);
				if (FAILED(hres)) {
					CYCLOPS_THROW_EXCEPTION_V(cyclOps::Exception, hres, cyclOps::Exception::TYPE_HRESULT, "Get() failed for User.");
				}
				pmethodGetOwner->Get(L"Domain", 0, &vtDomain, NULL, 0);
				if (FAILED(hres)) {
					CYCLOPS_THROW_EXCEPTION_V(cyclOps::Exception, hres, cyclOps::Exception::TYPE_HRESULT, "Get() failed for Domain.");
				}
				whoOps::wmi::Variant variant;
			variant.setType(vtUsername.vt); 
			switch (vtUsername.vt) { 
			/* http://stackoverflow.com/questions/6492865/iwbemclassobjectnext-yields-vt-null-types-when-they-are-not-what-does-this-me */
			case VT_NULL:
				/* This happens when, for example, you try to get the size of uninserted media (floppy/DVD). */
				break;
			case VT_BSTR:
				variant.setStringValueW(vtUsername.bstrVal); 
				break;
			case VT_I4:
				variant.setUintValue(vtUsername.uintVal);
				break;
			default:
				CYCLOPS_THROW_EXCEPTION_IV(cyclOps::Exception, "Unrecognized variant type: %d", vtUsername.vt);
			}
			mapOfPropNamesToVariants.map[whoOps::wmi::WMIClass::Owner] = variant;

			};
		private:
			Win32_Process(const Win32_Process&);
			Win32_Process operator=(const Win32_Process&);
		};
	}
}