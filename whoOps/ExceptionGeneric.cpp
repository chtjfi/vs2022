#include "StdAfx.h"
#include <stdio.h>
#include "ExceptionGeneric.h"
#include "..\cyclOps\cyclOps.h"

whoOps::ExceptionGeneric::ExceptionGeneric(void)
{
}

whoOps::ExceptionGeneric::ExceptionGeneric(char* szMessage)
{
	//::g_boCyclOpsDebug = true;
	CYCLOPSVAR(szMessage, "%s");
	_snprintf_s(_szMessage, CYCLOPSSIZEOF(_szMessage), _TRUNCATE, "%s", szMessage);
}


whoOps::ExceptionGeneric::~ExceptionGeneric(void)
{
}

char* whoOps::ExceptionGeneric::getMessage() {
	return _szMessage;
}

void whoOps::ExceptionGeneric::setMessage(char* szFormat, ...) {
	va_list vaArguments;
	va_start(vaArguments, szFormat);
	_vsnprintf_s(_szMessage, sizeof(_szMessage) / sizeof(_szMessage[0]), 
		_TRUNCATE, szFormat, vaArguments);
}