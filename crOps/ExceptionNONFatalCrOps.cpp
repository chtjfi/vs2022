#include "StdAfx.h"
#include "ExceptionNONFatalCrOps.h"


crOps::ExceptionNONFatal::ExceptionNONFatal(char* szMessage) : _szMessage(szMessage)
{
	_szMessage = szMessage;
}


crOps::ExceptionNONFatal::~ExceptionNONFatal(void)
{
}


char* crOps::ExceptionNONFatal::getMessage(void) const
{
	return _szMessage;
}


void crOps::ExceptionNONFatal::setMessage(char* szMessage)
{

}
