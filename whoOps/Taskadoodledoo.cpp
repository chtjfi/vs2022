#include "stdafx.h"

/*

WinSock2.h must come before Windows.h

From http://stackoverflow.com/questions/11726958/cant-include-winsock2-h-in-msvc-2010

To elaborate on this, winsock2.h is meant to be a replacement for winsock.h, they are not meant to be used together. If winsock.h is included first, winsock2.h fails to compile because it redeclares everything that winsock.h already declared. If winsock2.h is included first, it disables winsock.h for you. windows.h includes winsock2.h if _WIN32_WINNT >= 0x0400, otherwise it includes winsock.h instead.

*/

#include <WinSock2.h>


#include <comdef.h>

#include "..\cyclOps\cyclOps.h"
#include "..\whoOps\whoOpsBonanza.h"

#include "Taskadoodledoo.h"

using std::string;
using std::wstring;

using cyclOps::StringEmUp;
using cyclOps::JNIRequest;
using cyclOps::JNIResponse;

/*
	ITaskService -> GetFolder() -> GetTask() -> Run()
*/

namespace whoOps {
	Taskadoodledoo::Taskadoodledoo(const string& server, const string& user, const string& domain, const string& password)
		: _strServer(server), _strUser(user), _strDomain(domain), _strPassword(password)
	{
	}


	Taskadoodledoo::~Taskadoodledoo()
	{
	}

	struct State {
		TASK_STATE state;
		char* name;
	};

	State STATES[] = {
        {TASK_STATE_UNKNOWN, "TASK_STATE_UNKNOWN"},
        {TASK_STATE_DISABLED, "TASK_STATE_DISABLED"},
        {TASK_STATE_QUEUED, "TASK_STATE_QUEUED"},
        {TASK_STATE_READY, "TASK_STATE_READY"},
        {TASK_STATE_RUNNING, "TASK_STATE_RUNNING"}
	};

	void Taskadoodledoo::runOrStop(const string& strTask, const whoOpsWhatToDo& whatToDo) const { CYCLOPSDEBUG("Hello.");
		IRegisteredTask* pTask = this->getRegisteredTask(strTask);
		try {
			switch (whatToDo) {
			case RUN_TASK:
				this->run(pTask);
				break;
			case STOP_TASK:
				this->stop(pTask);
				break;
			}
			this->cleanup(NULL, NULL, pTask);
		} catch (const whoOps::ExceptionTaskadoodledoo& e) {
			e;
			this->cleanup(NULL, NULL, pTask);
			throw;
		}
	}

	ITaskDefinition* Taskadoodledoo::getTaskDefinition(IRegisteredTask* pTask) const { CYCLOPSDEBUG("Hello.");
		ITaskDefinition* pDefinition = 0;
		HRESULT hr = pTask->get_Definition(&pDefinition); CYCLOPSDEBUG("hr = %d", hr);
		if (FAILED(hr)) {
			/* Never cleanup something you didn't allocate yourself. */
			this->cleanup(NULL, NULL, NULL, pDefinition);
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionTaskadoodledoo, hr, 
				cyclOps::Exception::TYPE_HRESULT, "Error calling IRegisteredTask::get_Definition()");
		}
		return pDefinition;
	}

	IExecAction* Taskadoodledoo::getExecAction(ITaskDefinition* pTaskDefinition) const {
		IActionCollection* pActions = 0; 
		HRESULT hr = pTaskDefinition->get_Actions(&pActions); CYCLOPSDEBUG("pActions = %p", pActions);
		if (FAILED(hr)) {
			this->cleanup(NULL, NULL, NULL, NULL, NULL, NULL, pActions);
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionTaskadoodledoo, hr, 
				cyclOps::Exception::TYPE_HRESULT, "Error calling ITaskDefinition::get_Actions()");
		}
		IExecAction* pExecAction = this->getFirstExecAction(pActions); CYCLOPSDEBUG("pExecAction = %p" , pExecAction);
		return pExecAction;
	}

	IExecAction* Taskadoodledoo::getFirstExecAction(IActionCollection* pActions) const {
		long count;
		pActions->get_Count(&count); CYCLOPSDEBUG("count = %d", count);
		if (count > 0) {
			IAction* pAction = 0;
			HRESULT hr = pActions->get_Item(1, &pAction); CYCLOPSDEBUG("pAction = %p", pAction);
			if (FAILED(hr)) {
				this->cleanup(0, 0, 0, 0, 0, 0, 0, pAction);
				CYCLOPS_THROW_EXCEPTION_V(ExceptionTaskadoodledoo, hr, cyclOps::Exception::TYPE_HRESULT,
					"Error calling IActionCollection::get_Item()");
			}
			IExecAction *pExecAction = 0;
			hr = pAction->QueryInterface(IID_IExecAction, (void**) &pExecAction); CYCLOPSDEBUG("pExecAction = %p", pExecAction);
			if (FAILED(hr)) {
				this->cleanup(0, 0, 0, 0, 0, pExecAction, 0, pAction);
				CYCLOPS_THROW_EXCEPTION_V(ExceptionTaskadoodledoo, hr, cyclOps::Exception::TYPE_HRESULT,
					"Error calling IAction::QueryInterface()");
			}
			this->cleanup(0, 0, 0, 0, 0, 0, 0, pAction);
			return pExecAction;
		} else {
			CYCLOPS_THROW_EXCEPTION_IV(ExceptionTaskHasNoActions, "The task has no actions.");
		}
	}

	void Taskadoodledoo::getAllInformation(const string& taskname, whoOps::Task& task) const {
		task.setTaskName(taskname);
		IRegisteredTask* pTask = this->getRegisteredTask(taskname); 
		string lastRun = this->getLastRunTime(pTask);
		task.setLastRunTime(lastRun);
		string nextRun = this->getNextRunTime(pTask);
		task.setNextRunTime(nextRun);
		ITaskDefinition* pDefinition = this->getTaskDefinition(pTask); CYCLOPSDEBUG("pDefinition = %p", pDefinition);
		string user = this->getUser(pDefinition); CYCLOPSDEBUG("user = %s", user.c_str());
		task.setUserID(user);
		task.setServer(this->_strServer);
		LONG lastResult = this->getLastResult(pTask); CYCLOPSDEBUG("lastResult = %d", lastResult);
		task.setLastResult(lastResult);
		string lastResultMessage = this->getLastResultMessage(lastResult);
		task.setLastResultMessage(lastResultMessage);
		string state = this->getState(pTask); CYCLOPSDEBUG("state = %s", state.c_str());
		task.setState(state);
		this->getActionInfo(pDefinition, task);
		this->cleanup(NULL, NULL, pTask, pDefinition);
	}

	void Taskadoodledoo::getActionInfo(ITaskDefinition* pDefinition, Task& task) const {
		IExecAction* pExecAction = 0;
		try {
			pExecAction = this->getExecAction(pDefinition); CYCLOPSDEBUG("pExecAction = %p", pExecAction);
			BSTR bstrPath = 0;
			HRESULT hr = pExecAction->get_Path(&bstrPath); CYCLOPSDEBUG("bstrPath = %S", bstrPath);
			if (FAILED(hr)) { 
				if (pExecAction != 0) { pExecAction->Release(); }
				CYCLOPS_THROW_EXCEPTION_V(ExceptionTaskadoodledoo, hr, 
					cyclOps::Exception::TYPE_HRESULT, "IExecAction::get_Path() failed.");
			}
			task.setPath(bstrPath);
			BSTR bstrArgs = 0;
			hr = pExecAction->get_Arguments(&bstrArgs); CYCLOPSDEBUG("bstrArgs = %S", bstrArgs);
			if (FAILED(hr)) { 
				if (pExecAction != 0) { pExecAction->Release(); }
				CYCLOPS_THROW_EXCEPTION_V(ExceptionTaskadoodledoo, hr, 
					cyclOps::Exception::TYPE_HRESULT, "IExecAction::get_Argumetns() failed.");
			}
			task.setArguments(bstrArgs);
		} catch (const ExceptionTaskHasNoActions& ignore) { ignore;
			this->cleanup(NULL, NULL, NULL, NULL, NULL, pExecAction);
		}
	}

	string Taskadoodledoo::getState(IRegisteredTask* pTask) const { CYCLOPSDEBUG("pTask = %p", pTask);
		TASK_STATE state = TASK_STATE_UNKNOWN;
		pTask->get_State(&state);
		return this->convertStateToString(state);
	}

	string Taskadoodledoo::convertStateToString(TASK_STATE state) const {
		size_t size = sizeof(STATES) / sizeof(STATES[0]);
		for (size_t i = 0; i < size; ++i) {
			if (STATES[i].state == state) {
				return STATES[i].name;
			}
		}
		return "UNKNOWN";
	
	}

	string Taskadoodledoo::getUser(const string& strTask) const {
		IRegisteredTask* pTask = this->getRegisteredTask(strTask);
		ITaskDefinition* pDefinition = this->getTaskDefinition(pTask);
		string user = this->getUser(pDefinition);
		this->cleanup(NULL, NULL, pTask, pDefinition);
		return user;
	}

	string Taskadoodledoo::getUser(ITaskDefinition* pDefinition) const { 
		IPrincipal* pPrincipal = 0; 
		HRESULT hr = pDefinition->get_Principal(&pPrincipal);
		if (FAILED(hr)) {
			this->cleanup(NULL, NULL, NULL, NULL, pPrincipal);
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionTaskadoodledoo, hr, 
				cyclOps::Exception::TYPE_HRESULT, "Error calling IPrincipal::get_Principal");
		}
		BSTR bstrDisplayName = 0;
		pPrincipal->get_UserId(&bstrDisplayName); CYCLOPSDEBUG("bstrDisplayName = %S\n", bstrDisplayName);
		wstring w(bstrDisplayName);
		string user(w.begin(), w.end());
		this->cleanup(NULL, NULL, NULL, NULL, pPrincipal);
		return user;
	}

	string Taskadoodledoo::getLastRunTime(const string& strTask) const { CYCLOPSDEBUG("Hello.");
		IRegisteredTask* pTask = this->getRegisteredTask(strTask);
		return this->getLastRunTime(pTask);
	}

	string Taskadoodledoo::getLastRunTime(IRegisteredTask* pTask) const {
		DATE date;
		pTask->get_LastRunTime(&date); 
		cyclOps::TimeTiger tiger;
		string iso8601 = tiger.variantTimeToISO8601(date);
		return iso8601;
	}

	string Taskadoodledoo::getNextRunTime(IRegisteredTask* pTask) const {
		DATE date;
		pTask->get_NextRunTime(&date); 
		cyclOps::TimeTiger tiger;
		string iso8601 = tiger.TimeTiger::variantTimeToISO8601(date);
		return iso8601;
	}

	LONG Taskadoodledoo::getLastResult(IRegisteredTask* pTask) const { CYCLOPSDEBUG("pTask = %p", pTask);
		LONG lastResult;
		pTask->get_LastTaskResult(&lastResult); CYCLOPSDEBUG("lastResult = %d", lastResult);
		return lastResult;
	}

	string Taskadoodledoo::getLastResultMessage(const LONG& lastResult) const {
		return ErroroneousMonk::hresultToString(lastResult);
	}

	IRegisteredTask* Taskadoodledoo::getRegisteredTask(const string& strTask) const { CYCLOPSDEBUG("Hello.");
		ITaskFolder* pFolder = 0;
		IRegisteredTask* pTask = 0;
		ITaskService* pTaskService = 0;
		try {
			pTaskService = this->getTaskService();
			pFolder = this->getTaskFolder(pTaskService);
			pTask = this->getRegisteredTask(pFolder, strTask); 
		} catch (const whoOps::ExceptionTaskadoodledoo& e) {
			e;
			this->cleanup(pTaskService, pFolder, pTask);
			throw;
		}
		this->cleanup(pTaskService, pFolder);
		return pTask;
	}

	void Taskadoodledoo::stop(IRegisteredTask* pTask) const {
		HRESULT hr = pTask->Stop(0);
		if (FAILED(hr)) {
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionTaskadoodledoo, hr, cyclOps::Exception::TYPE_HRESULT,
				"IRegisteredTask::Stop() failed.");
		}
	}

	void Taskadoodledoo::run(IRegisteredTask* pTask) const { CYCLOPSDEBUG("pTask = %p", pTask); 
		IRunningTask* pRunningTask = 0;
		variant_t params; 
		params.vt = VT_NULL;
		HRESULT hResult = pTask->Run(params, &pRunningTask); CYCLOPSDEBUG("Task has been run.");
		if (FAILED(hResult)) {
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionTaskadoodledoo, hResult, cyclOps::Exception::TYPE_HRESULT, "IRegisteredTask::Run() failed.");
		}
	}

	void Taskadoodledoo::cleanup(ITaskService* pService, ITaskFolder* pFolder, 
		IRegisteredTask* pTask, ITaskDefinition* pDefinition, IPrincipal* pPrincipal, 
		IExecAction* pExecAction, IActionCollection* pActions, IAction* pAction) const {
		if (pService) {
			pService->Release();
		}
		if (pFolder) {
			pFolder->Release();
		}
		if (pTask) {
			pTask->Release();
		}
		if (pDefinition) {
			pDefinition->Release();
		}
		if (pPrincipal) {
			pPrincipal->Release();
		}
		if (pExecAction) {
			pExecAction->Release();
		}
		if (pActions) { 
			pActions->Release();
		}
		if (pAction) {
			pAction->Release();
		}
	}

	IRegisteredTask* Taskadoodledoo::getRegisteredTask(ITaskFolder* pFolder, const string& strTask) const { CYCLOPSDEBUG("Hello.");
		IRegisteredTask* pTask = 0;
		//wstring wstrTask(strTask.begin(), strTask.end());
		wchar_t* pwchTask = cyclOps::StringEmUp::new_wcharArray(strTask);
		HRESULT hResult = pFolder->GetTask(pwchTask, &pTask);
		delete[] pwchTask;
		if (FAILED(hResult)) {
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionTaskadoodledoo, hResult, cyclOps::Exception::TYPE_HRESULT, 
				"ITaskFolder::GetTask() failed for task %s on server %s.", strTask.c_str(), this->_strServer.c_str());
		}
		return pTask;
	}

	ITaskFolder* Taskadoodledoo::getTaskFolder(ITaskService* pService) const { CYCLOPSVAR(pService, "%p");
		if (pService == 0) { 
			CYCLOPS_THROW_EXCEPTION_IV(whoOps::ExceptionTaskadoodledoo, "NULL passed in pServce.  Someone forgot to initialize the service!");
		}
		ITaskFolder* pFolder = 0;
		HRESULT hResult = pService->GetFolder(L"\\", &pFolder); CYCLOPSVAR(hResult, "%x");
		if (FAILED(hResult)) {
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionTaskadoodledoo, hResult, cyclOps::Exception::TYPE_HRESULT, "ITaskService::GetFolder() failed.");
		}
		return pFolder;
	}

	void Taskadoodledoo::coInitializeSecurity(void) const {
		HRESULT hr = CoInitializeSecurity(
			NULL,
			-1,
			NULL,
			NULL,
			RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
			RPC_C_IMP_LEVEL_IMPERSONATE,
			NULL,
			0,
			NULL);

		if( FAILED(hr) && ! (hr == RPC_E_TOO_LATE)) {
			CoUninitialize();
			CYCLOPS_THROW_EXCEPTION_IV(whoOps::ExceptionTaskadoodledoo, "CoInitializeSecurity() failed.");
		}
	}

	ITaskService* Taskadoodledoo::getTaskService(void) const { CYCLOPSDEBUG("Hello.");
		/* http://msdn.microsoft.com/en-us/library/windows/desktop/aa446864(v=vs.85).aspx */
		HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
		if( FAILED(hr) ) {
			CYCLOPS_THROW_EXCEPTION_IV(whoOps::ExceptionTaskadoodledoo, "CoInitializeEx() failed.");
		}
		this->coInitializeSecurity();
		ITaskService *pService = this->coCreateInstance();
		variant_t server; this->setIfNecessary(server, _strServer);
		variant_t user; this->setIfNecessary(user, _strUser);
		variant_t domain; this->setIfNecessary(domain, _strDomain);
		variant_t password; this->setIfNecessary(password, _strPassword); CYCLOPSDEBUG("%S/%S/%S/%S", server.bstrVal, user.bstrVal, domain.bstrVal, password.bstrVal);
		hr = pService->Connect(server, user, domain, password);
		if (FAILED(hr)) {
			pService->Release();
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionTaskadoodledoo, hr, cyclOps::Exception::TYPE_HRESULT, "ITaskService::Connect() failed.");
		}
		return pService;
	}

	void Taskadoodledoo::setIfNecessary(variant_t& varbie, const string& value) const {
		if (!cyclOps::StringEmUp::isWhitespaceOnly(value)) {
			varbie.SetString(value.c_str());
		}
	}
	ITaskService* Taskadoodledoo::coCreateInstance(void) const {
		ITaskService *pService = NULL;
		HRESULT hr = CoCreateInstance(CLSID_TaskScheduler,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_ITaskService,
			(void**) &pService);
		if (FAILED(hr)) {
			CoUninitialize();
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionTaskadoodledoo, hr, cyclOps::Exception::TYPE_HRESULT,
				"CoCreateInstance() failed.");
		}
		return pService;
	}

	void Taskadoodledoo::schedule(const whoOps::Task& task) {
		/* https://msdn.microsoft.com/en-us/library/windows/desktop/aa382113(v=vs.85).aspx */
		ITaskFolder* pFolder = 0;
		ITaskDefinition* pTaskDefinition = 0;
		ITaskService* pTaskService = 0;
		try {
			pTaskService = this->getTaskService();
			pFolder = this->getTaskFolder(pTaskService); 
			this->deleteTask(pFolder, task);
			pTaskDefinition = this->getTaskDefinition(task, pTaskService);
			this->registerTaskDefinition(pFolder, pTaskDefinition, task);
		} catch (const whoOps::ExceptionTaskadoodledoo& e) {
			e;
			this->cleanup(pTaskService, pFolder, NULL, pTaskDefinition);
			throw;
		}
		this->cleanup(pTaskService, pFolder);
	}

	void Taskadoodledoo::registerTaskDefinition(ITaskFolder* pFolder, ITaskDefinition* pTaskDefinition, const whoOps::Task& task) const {
		IRegisteredTask *pRegisteredTask = NULL; 
		wchar_t* pwchTaskName = StringEmUp::new_wcharArray(task.getTaskName());
		HRESULT hr = pFolder->RegisterTaskDefinition(
				pwchTaskName,
				pTaskDefinition,
				TASK_CREATE_OR_UPDATE, 
				_variant_t(), /* User */
				_variant_t(), /* Password */
				TASK_LOGON_INTERACTIVE_TOKEN,
				_variant_t(L""),
				&pRegisteredTask);
		delete[] pwchTaskName;
		if (FAILED(hr)) {
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionTaskadoodledoo, hr, cyclOps::Exception::TYPE_HRESULT, 
				"ITaskFolder::RegisterTaskDefinition() failed for task '%s' on server '%s'", task.getTaskName().c_str(), task.getServer().c_str());
		}
	}

	ITaskDefinition* Taskadoodledoo::getTaskDefinition(const whoOps::Task& task, ITaskService* pService) const {
		string serverAndTask = StringEmUp::format("task '%s' on server '%s'", task.getTaskName().c_str(), task.getServer());
		ITaskDefinition* pTask = NULL; 
		HRESULT hr = pService->NewTask( 0, &pTask );
	    if (FAILED(hr)) {
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionTaskadoodledoo, hr, cyclOps::Exception::TYPE_HRESULT, "ITaskService::NewTask() failed for %s", serverAndTask.c_str());
		}
		IRegistrationInfo *pRegInfo = NULL;
		hr = pTask->get_RegistrationInfo( &pRegInfo );
		if (FAILED(hr)) {
			pTask->Release();
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionTaskadoodledoo, hr, cyclOps::Exception::TYPE_HRESULT, "ITaskDefinition::get_RegistrationInfo() failed for %s", serverAndTask.c_str());
		}
		wchar_t* pwchUserID = task.getUserIDAsNewWcharArray();
		hr = pRegInfo->put_Author(pwchUserID);
		pRegInfo->Release(); 
		if (FAILED(hr)) {
			pTask->Release();
			delete[] pwchUserID;
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionTaskadoodledoo, hr, cyclOps::Exception::TYPE_HRESULT, "IRegistrationInfo::put_Author() failed for %s", serverAndTask.c_str());
		}
		IPrincipal *pPrincipal = NULL;
	    hr = pTask->get_Principal( &pPrincipal );
		if (FAILED(hr)) {
	        pTask->Release();
			delete[] pwchUserID;
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionTaskadoodledoo, hr, cyclOps::Exception::TYPE_HRESULT, "ITaskDefinition::get_Principal() failed for %s", serverAndTask.c_str());
	    }
		hr = pPrincipal->put_Id(pwchUserID); 
		delete[] pwchUserID;
	    hr = pPrincipal->put_LogonType( TASK_LOGON_SERVICE_ACCOUNT );
	    hr = pPrincipal->put_RunLevel( TASK_RUNLEVEL_LUA ); 
		pPrincipal->Release();  
		if (FAILED(hr)) {
	        pTask->Release();
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionTaskadoodledoo, hr, cyclOps::Exception::TYPE_HRESULT, "IPrincipal::put_RunLevel() failed for %s", serverAndTask.c_str());
	    }
		ITaskSettings *pSettings = NULL;
		hr = pTask->get_Settings( &pSettings );
		if (FAILED(hr)) {
	        pTask->Release();
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionTaskadoodledoo, hr, cyclOps::Exception::TYPE_HRESULT, "ITaskDefinition::get_Settings() failed for %s", serverAndTask.c_str());
		}
	    hr = pSettings->put_StartWhenAvailable(VARIANT_TRUE);    
		pSettings->Release(); 
		if (FAILED(hr)) {
	        pTask->Release();
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionTaskadoodledoo, hr, cyclOps::Exception::TYPE_HRESULT, "ITaskSettings::put_StartWhenAvailable() failed for %s", serverAndTask.c_str());
		}

/*		this->addTriggers(pTask, serverAndTask); */

	    IActionCollection *pActionCollection = NULL;
		hr = pTask->get_Actions( &pActionCollection );
		if (FAILED(hr)) {
	        pTask->Release();
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionTaskadoodledoo, hr, cyclOps::Exception::TYPE_HRESULT, "ITaskDefinition::get_Actions() failed for %s", serverAndTask.c_str());
		}
		IAction *pAction = NULL;
		hr = pActionCollection->Create( TASK_ACTION_EXEC, &pAction );
		pActionCollection->Release();
		if (FAILED(hr)) {
	        pTask->Release();
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionTaskadoodledoo, hr, cyclOps::Exception::TYPE_HRESULT, "IActionCollection::Create() failed for %s", serverAndTask.c_str());
	    }
		IExecAction *pExecAction = NULL;
	    hr = pAction->QueryInterface(IID_IExecAction, (void**) &pExecAction );
		pAction->Release();
		if (FAILED(hr)) {
	        pTask->Release();
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionTaskadoodledoo, hr, cyclOps::Exception::TYPE_HRESULT, "IAction::QueryInterface() failed for %s", serverAndTask.c_str());
		}
		wchar_t* pwchPath = StringEmUp::new_wcharArray(task.getPath());
		hr = pExecAction->put_Path(pwchPath); 
		delete[] pwchPath;
		pExecAction->Release(); 
		if (FAILED(hr)) {
	        pTask->Release();
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionTaskadoodledoo, hr, cyclOps::Exception::TYPE_HRESULT, "IExecAction::put_Path() failed for %s", serverAndTask.c_str());
	    }
		return pTask;
	}

	void Taskadoodledoo::deleteTask(ITaskFolder* pFolder, const whoOps::Task& task) const {
		wchar_t* pwchTask = task.getTaskNameAsNewWcharArray();
		HRESULT hr = pFolder->DeleteTask(pwchTask, 0);
		delete[] pwchTask;
		if (FAILED(hr) && ! ErroroneousMonk::isFileNotFound(hr)) { //CYCLOPSVAR(hr, "%l");
			string msg = cyclOps::StringEmUp::format("Error calling ITaskFolder::DeleteTask()");
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionTaskadoodledoo, hr, cyclOps::Exception::TYPE_HRESULT, "WTF?"); //msg.c_str());
			throw std::runtime_error("WTF?");
		}
	}

	void Taskadoodledoo::addTriggersThatAreUseless(ITaskDefinition* pTask, const string& serverAndTask) const {
		ITriggerCollection *pTriggerCollection = NULL;
		HRESULT hr = pTask->get_Triggers( &pTriggerCollection );
		if (FAILED(hr)) {
	        pTask->Release();
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionTaskadoodledoo, hr, cyclOps::Exception::TYPE_HRESULT, "ITaskDefinition::get_Triggers() failed for %s", serverAndTask.c_str());
	    }
		ITrigger *pTrigger = NULL;    
		hr = pTriggerCollection->Create( TASK_TRIGGER_REGISTRATION, &pTrigger );
		pTriggerCollection->Release();
		if (FAILED(hr)) {
	        pTask->Release();
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionTaskadoodledoo, hr, cyclOps::Exception::TYPE_HRESULT, "ITriggerCollection::Create() failed for %s", serverAndTask.c_str());
		}     
		IRegistrationTrigger *pRegistrationTrigger = NULL;
		hr = pTrigger->QueryInterface(IID_IRegistrationTrigger, (void**) &pRegistrationTrigger );
		pTrigger->Release();
		if (FAILED(hr)) {
	        pTask->Release();
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionTaskadoodledoo, hr, cyclOps::Exception::TYPE_HRESULT, "ITrigger::QueryInterface() failed for %s", serverAndTask.c_str());
	   }
		hr = pRegistrationTrigger->put_Id( _bstr_t( L"Trigger1" ) );
		hr = pRegistrationTrigger->put_Delay( L"PT30S" );
		pRegistrationTrigger->Release();
		if (FAILED(hr)) {
	        pTask->Release();
			CYCLOPS_THROW_EXCEPTION_V(whoOps::ExceptionTaskadoodledoo, hr, cyclOps::Exception::TYPE_HRESULT, "IRegistrationTrigger::put_Delay() failed for %s", serverAndTask.c_str());
		}   
	}
}