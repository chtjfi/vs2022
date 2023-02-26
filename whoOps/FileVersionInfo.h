#pragma once

namespace whoOps {
	/* This comment is to cause a recompile. */
	class FileVersionInfo
	{
	public:
		FileVersionInfo(void);
		~FileVersionInfo(void);
		static void getFileVersionInfo(const char* szFile, char* szVersion, size_t iSize, 
			bool boDebug = true);
	};
}