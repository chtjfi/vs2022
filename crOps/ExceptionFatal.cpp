#include "StdAfx.h"
#include "ExceptionFatal.h"
#include <stdio.h>
#include "crOpsPrivate.h"


crOps::ExceptionFatal::ExceptionFatal(char* szMessage) : _szMessage(szMessage)
{
	CROPSDEBUG("ExceptionFatal constructor II.");
}

crOps::ExceptionFatal::ExceptionFatal() 
{
	CROPSDEBUG("ExceptionFatal constructor I.");
}

crOps::ExceptionFatal::~ExceptionFatal(void)
{
}


char* crOps::ExceptionFatal::getMessage(void)
{
	return _szMessage;
}
