#pragma once

namespace crOps {
	class ExceptionFatal
	{
		char* _szMessage;
	public:
		ExceptionFatal(char* szMessage);
		ExceptionFatal();
		~ExceptionFatal(void);
		char* getMessage(void);
	};
}
