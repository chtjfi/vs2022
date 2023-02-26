#include "StdAfx.h"

#include <Windows.h>

#include "ShutdownShark.h"


whoOps::ShutdownShark::ShutdownShark(void)
{
}


whoOps::ShutdownShark::~ShutdownShark(void)
{
}


void whoOps::ShutdownShark::reboot(std::string strHostname)
{
	char szHostname[5000];
	_snprintf_s(szHostname, sizeof(szHostname) / sizeof(szHostname[0]), "%s", strHostname.c_str());
	::InitiateSystemShutdownA(
		szHostname, 
		"Shutdown Shark",	/* Message written to eventlog. */
		0,					/* Timeout. */
		true,				/* Force apps closed. */
		true);				/* Reboot.*/
}
