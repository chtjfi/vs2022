#pragma once
namespace whoOps {
	class ExceptionGeneric
	{
	private:
		char _szMessage[5000];
	public:
		ExceptionGeneric(void);
		ExceptionGeneric(char* szMessage);
		~ExceptionGeneric(void);
		char* getMessage();
		void setMessage(char* szFormat, ...);
	};
}
