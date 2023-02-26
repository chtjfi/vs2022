#include "stdafx.h"
#include "StringEmUp.h"
#include "Encryptigator.h"

using std::string;

namespace cyclOps {
	Encryptigator::Encryptigator()
	{
	}


	Encryptigator::~Encryptigator()
	{
	}

	std::string Encryptigator::decryptWonky(const string& encrypted) {
		string decrypted;
		int position = 0;
		for (string::const_reverse_iterator rit = encrypted.rbegin(); rit != encrypted.rend(); ++rit) {
			if (position % 2 == 0) {
				decrypted += *rit;
			}
			++position;
		}
		return decrypted;
	}

	std::string Encryptigator::encryptWonky(const string& decrypted) {
		string encrypted;
		for (string::const_reverse_iterator rit = decrypted.rbegin(); rit != decrypted.rend(); ++rit) {
			/* First character is throwaway. */
			char c = cyclOps::StringEmUp::getRandomCharacter();
			encrypted += c;
			encrypted += *rit;
		}
		return encrypted;
	}

}