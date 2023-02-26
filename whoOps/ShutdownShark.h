#pragma once

#include <string>

namespace whoOps {
	class ShutdownShark
	{
	public:
		ShutdownShark(void);
		~ShutdownShark(void);
		void reboot(std::string strHostname);
	};
}