#pragma once

#include <vector>
#include <string>

namespace cyclOps {
	class JNIJunkie {
		private:
			JNIEnv* _pJNIEnv;
			jobject* _pJobject;
			jstring* _pJstringInput;
		public:
			JNIJunkie(JNIEnv *env, jobject obj, jstring input);
			~JNIJunkie(void);
			const char* GetStringUTFChars(void);
			void getArgumentArray(wchar_t wszArgumentArray[][5000], const int iArguments);
			std::vector<std::string> getArgumentVector(void);
	};
}