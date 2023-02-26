#include "stdafx.h"
#include "Automail.h"

#include <string>
#include <vector>
#include <fstream>

#include "..\cyclOps\cyclOps.h"

using std::string;
using std::vector;
using std::stringstream;
using std::endl;

namespace cyclOps {
	Automail::Automail() : _executable("Executable not specified.")
	{
	}


	Automail::~Automail()
	{
	}

	void Automail::send() {
		string guid = cyclOps::StringEmUp::getGUID();
		stringstream ssPath; ssPath << _automailShare << "\\" << guid << ".txt";
		std::ofstream file;
		file.open(ssPath.str(), std::ios::app);
		if (file.fail()) {
			stringstream ssError; ssError << "Could not open " << ssPath.str();
			throw std::runtime_error(ssError.str());
		}
		/*	writer.println(to);
			writer.println("");
			writer.println(this.getFrom()); 
			writer.println(this.getSubject());
			writer.println(this.getMessage());
			this.addAttachments(writer); */
		file << this->_executable << endl;
		file << this->getRecipients() << endl;
		file << endl;
		file << this->_from << endl;
		file << this->_subject << endl;
		file << this->_message << endl;
		file.close();
	}

	string Automail::getRecipients() {
		bool boFirstTime = true;
		stringstream ssRecipients;
		for (int i = 0; i < _recipients.size(); ++i) {
			if (boFirstTime) {
				boFirstTime = false;
			} else {
				ssRecipients << ",";
			}
			ssRecipients << _recipients[i];
		}
		return ssRecipients.str();
	}

	void Automail::addRecipients(const vector<string>& recipients) {
		for (vector<string>::const_iterator i = recipients.begin(); i < recipients.end(); ++i) {
			_recipients.push_back(*i);
		}
	}
}