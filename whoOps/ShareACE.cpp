#include "StdAfx.h"

#include <string>
#include <sstream>

#include "..\cyclOps\cyclOps.h"

#include "ACLAmigo.h"

#include "ShareACE.h"

using std::string;
using std::stringstream;

namespace whoOps {
	ShareACE::ShareACE(void)
	{
	}


	ShareACE::~ShareACE(void)
	{
	}

	string ShareACE::getMaskString(void) const { CYCLOPSDEBUG("Hello."); 
		switch (_mask) {
		case ACLAmigo::ACCESS_MASK_SHARE_FULL:
			return "FULL";
		case ACLAmigo::ACCESS_MASK_SHARE_READ:
			return "READ";
		case ACLAmigo::ACCESS_MASK_SHARE_CHANGE:
			return "CHANGE";
		default:
			char szReturn[1000];
			_snprintf_s(szReturn, sizeof(szReturn) / sizeof(szReturn[0]), _TRUNCATE, "UNKNOWN - 0x%08x", _mask);
			return "UNKNOWN";
		}
	}
}