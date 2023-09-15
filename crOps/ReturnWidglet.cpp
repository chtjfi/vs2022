#include "StdAfx.h"
#include "ReturnWidglet.h"


crOps::ReturnWidglet::ReturnWidglet(void)
{
	_iStatus = 0;
}


crOps::ReturnWidglet::~ReturnWidglet(void)
{
}

void crOps::ReturnWidglet::setWarning() {
	if (_iStatus < 1) {
		_iStatus = 1;
	}
}

void crOps::ReturnWidglet::setError() {
	if (_iStatus < 2) {
		_iStatus = 2;
	}
}

void crOps::ReturnWidglet::setFatal() {
	if (_iStatus < 3) {
		_iStatus = 3;
	}
}

unsigned int crOps::ReturnWidglet::getStatus() {
	return _iStatus;
}