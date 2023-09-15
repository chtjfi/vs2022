#include "stdafx.h"
#include "jni.h"
#include "JNIJunkie.h"
#include "cyclOps.h"
#include <boost/algorithm/string.hpp>

cyclOps::JNIJunkie::JNIJunkie(JNIEnv *env, jobject obj, jstring jstringInput) {
	CYCLOPSDEBUG("hello");
	this->_pJNIEnv = env;
	this->_pJobject = &obj;
	this->_pJstringInput = &jstringInput;
}

cyclOps::JNIJunkie::~JNIJunkie() { }

const char* cyclOps::JNIJunkie::GetStringUTFChars(void) {
	const char *str = _pJNIEnv->GetStringUTFChars(*_pJstringInput, NULL);
	if (str == NULL) {
		const char* szError = "ERROR: GetStringUTFChars returned NULL";
		CYCLOPSERROR("%s", szError);
		return "poo"; //_pJNIEnv->NewStringUTF(szError);
	}
	return "poo";
}


void cyclOps::JNIJunkie::getArgumentArray(wchar_t wszArgumentArray[][5000], const int iArgumentNumber) 
{
	// LOOKLOOKLOOKLOOKLOOKLOOKLOOKLOOKLOOKLOOKLOOKLOOKLOOKLOOKLOOKLOOKLOOKLOOKLOOKLOOKLOOKLOOKLOOKLOOKLOOKLOOK
	if (iArgumentNumber < 2) {
		CYCLOPS_THROW_EXCEPTION_IV(cyclOps::Exception, "Incorrect number of arguments.");
	}
	//wchar_t wargv[2][500];
	const char* szInput = _pJNIEnv->GetStringUTFChars(*_pJstringInput, NULL);
	char szTemp[1000];
	_snprintf_s(szTemp, sizeof(szTemp) / sizeof(szTemp[0]), _TRUNCATE, "%s", szInput);
	char * superfluous;
	char* pch = strtok_s(szTemp, ";", &superfluous);
	for (int iCurrentArgument = 0; iCurrentArgument < iArgumentNumber && pch != NULL; ++iCurrentArgument) {
		_snwprintf_s(
			wszArgumentArray[iCurrentArgument], 
			sizeof(wszArgumentArray[iCurrentArgument]) / sizeof(wszArgumentArray[iCurrentArgument][0]), 
			_TRUNCATE, L"%S", pch);
		pch = strtok_s(NULL, ";", &superfluous);
	}
}


std::vector<std::string> cyclOps::JNIJunkie::getArgumentVector(void)
{
	const char* szInput = _pJNIEnv->GetStringUTFChars(*_pJstringInput, NULL);
	std::vector<std::string> argumentVector;
	boost::split(argumentVector, szInput, boost::is_any_of(";"));	
	return argumentVector;
}
