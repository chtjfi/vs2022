#pragma once

#include <string>


namespace cyclOps {
	class Encryptigator
	{
	public:
		Encryptigator();
		~Encryptigator();
		static std::string decryptWonky(const std::string& encrypted);
		static std::string encryptWonky(const std::string& decrypted);
	};
}