#pragma once

#include <string>

#include <taskschd.h>

#include "whoOpsPrivate.h"

#include "Task.h"

/* http://msdn.microsoft.com/en-us/magazine/cc163350.aspx */

namespace whoOps { 
	class Taskadoodledoo
	{
	public:
		explicit Taskadoodledoo(const std::string& server = "", const std::string& user = "", 
			const std::string& domain = "", const std::string& password = "");
		~Taskadoodledoo();
		void runOrStop(const std::string& task, const whoOpsWhatToDo& what) const;
		std::string getLastRunTime(const std::string& strTask) const;
		std::string getUser(const std::string& strTask) const;
		void getAllInformation(const std::string& taskname, whoOps::Task& task) const;
		void schedule(const whoOps::Task& task);
	private:
		Taskadoodledoo(const Taskadoodledoo& r);
		Taskadoodledoo operator=(const Taskadoodledoo& r);
		const std::string _strServer;
		const std::string _strUser;
		const std::string _strDomain;
		const std::string _strPassword;
		std::string _task;
		ITaskService* getTaskService(void) const;
		ITaskFolder* getTaskFolder(ITaskService* pTaskService) const;
		void coInitializeSecurity(void) const;
		void cleanup(ITaskService* pService = 0, ITaskFolder* pFolder = 0, 
			IRegisteredTask* pTask = 0, ITaskDefinition* pDefinition = 0,
			IPrincipal* pPrincipal = 0, IExecAction* pExecAction = 0,
			IActionCollection* pActions = 0, IAction* pAction = 0) const;
		IRegisteredTask* getRegisteredTask(ITaskFolder* pFolder, const std::string& strTask) const;
		void run(IRegisteredTask* pTask) const;
		ITaskService* coCreateInstance(void) const;
		void setIfNecessary(variant_t& varbie, const std::string& value) const;
		IRegisteredTask* getRegisteredTask(const std::string& strTask) const;
		ITaskDefinition* getTaskDefinition(IRegisteredTask* pTask) const;
		ITaskDefinition* getTaskDefinition(const whoOps::Task& task, ITaskService* pTaskService) const;
		void registerTaskDefinition(ITaskFolder* pFolder, ITaskDefinition* pTaskDefinition, const whoOps::Task& task) const;
		std::string getLastRunTime(IRegisteredTask* pTask) const;
		std::string getUser(ITaskDefinition* pDefinition) const;
		std::string getNextRunTime(IRegisteredTask* pTask) const;
		LONG getLastResult(IRegisteredTask* pTask) const;
		std::string getLastResultMessage(const LONG& lastResult) const;
		std::string getState(IRegisteredTask* pTask) const;
		std::string convertStateToString(TASK_STATE state) const;
		IExecAction* getExecAction(ITaskDefinition* pTaskDefinition) const;
		IExecAction* getFirstExecAction(IActionCollection* pActions) const;
		void getActionInfo(ITaskDefinition* pDefinition, Task& task) const;
		void stop(IRegisteredTask* pTask) const;
		void deleteTask(ITaskFolder* pFolder, const whoOps::Task& task) const;
		void addTriggersThatAreUseless(ITaskDefinition* pTask, const std::string& serverAndTask) const;
	};

}