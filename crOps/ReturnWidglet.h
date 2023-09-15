#pragma once
namespace crOps {
	class ReturnWidglet
	{
	private:
		unsigned int _iStatus;
	public:
		ReturnWidglet(void);
		~ReturnWidglet(void);
		void setWarning();
		void setError();
		void setFatal();
		unsigned int getStatus();

	};
}
