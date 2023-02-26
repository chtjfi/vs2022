#pragma once

#include <string>

namespace whoOps {
	class Task
	{
	public:

		explicit Task()
		{
		}

		~Task()
		{
		}

		void setTaskName(const std::string& s) { taskName.assign(s); }
		void setLastRunTime(const char* s) { if (s != 0) lastRunTime.assign(s); }
		void setLastRunTime(const std::string& s) { lastRunTime.assign(s); }
		void setNextRunTime(const char* s) { if (s != 0) nextRunTime.assign(s); }
		void setNextRunTime(const std::string& s) { nextRunTime.assign(s); }
		void setUserID(const char* s) { if (s != 0) userID.assign(s); }
		void setUserID(const std::string& s) { userID.assign(s); }
		void setServer(const char* s) { if (s != 0) server.assign(s); }
		void setServer(const std::string& s) { server.assign(s); }
		void setLastResult(const LONG& l) { lastResult = l; }
		void setLastResultMessage(const char* s) { if (s != 0) lastResultMessage.assign(s); }
		void setLastResultMessage(const std::string& s) { lastResultMessage.assign(s); }
		void setState(const char* s) { if (s != 0) this->setState(std::string(s)); }
		void setState(const std::string& s) { state.assign(s); }
		void setPath(const wchar_t* w) { if (w != 0) this->setPath(std::wstring(w)); }
		void setPath(const std::wstring& w) { path.assign(w.begin(), w.end()); }
		void setPath(const char* w) { if (w != 0) this->setPath(std::string(w)); }
		void setPath(const std::string& w) { path.assign(w.begin(), w.end()); }
		void setArguments(const wchar_t* w) { if (w != 0) this->setArguments(std::wstring(w)); }
		void setArguments(const std::wstring& w) { arguments.assign(w.begin(), w.end()); }
		void setArguments(const char* w) { if (w != 0) this->setArguments(std::string(w)); }
		void setArguments(const std::string& w) { arguments.assign(w.begin(), w.end()); }
		void setPassword(const char* w) { if (w != 0) this->setPassword(std::string(w)); }
		void setPassword(const std::string& w) { password.assign(w.begin(), w.end()); }
		std::string getTaskName(void) const { return taskName; }
		wchar_t* getPropertyAsNewWcharArray(const std::string& s) const {
			size_t size = s.size() + 1;
			wchar_t* pwchNew = new wchar_t[size];
			::swprintf_s(pwchNew, size, L"%S", s.c_str());
			return pwchNew;
		}
		wchar_t* getTaskNameAsNewWcharArray(void) const {
			return this->getPropertyAsNewWcharArray(this->taskName);
		}
		wchar_t* getUserIDAsNewWcharArray(void) const {
			return this->getPropertyAsNewWcharArray(this->userID);
		}
		std::string getLastRunTime(void) const { return lastRunTime; }
		std::string getNextRunTime(void) const { return nextRunTime; }
		std::string getUserID(void) const { return userID; }
		std::string getServer(void) const { return server; }
		std::string getLastResultMessage(void) const { return lastResultMessage; }
		LONG getLastResult(void) const { return lastResult; }
		std::string getState(void) const { return state; }
		std::string getPath(void) const { return path; }
		std::string getArguments(void) const { return arguments; }
	private:
		Task(const Task& t);
		Task& operator=(const Task& t);
		std::string password;
		std::string taskName;
		std::string lastRunTime;
		std::string userID;
		std::string server;
		std::string nextRunTime;
		std::string lastResultMessage;
		LONG lastResult;
		std::string state;
		std::string path;
		std::string arguments;
	};

}