#pragma once

namespace crOps {
	class ExceptionNONFatal
	{
		char* _szMessage;
	public:
		ExceptionNONFatal(char* szMessage);
		~ExceptionNONFatal(void);
		char* getMessage(void) const;
		void setMessage(char* szMessage);
	};
}
