#ifndef _CTHREAD_
#define _CTHREAD_

#define STATE_RUNNING    0x0001  //线程正在运行
#define STATE_READY      0x0002  //线程正在加入调用方
#define STATE_BLOCKED    0x0004  //线程被挂起
#define STATE_ALIVE      0x0008  //线程被创建
#define STATE_ASYNC      0x0010  //线程将并发执行,即在线程返回之前，调用方不会阻塞线程
#define STATE_SYNC       0x0020  //线程将不会并发执行，即调用方在线程返回之前将一直阻塞
#define STATE_CONTINUOUS 0x0040  //线程将连续工作永不停止，或者至少在进程退出前一直运行

#define THREADTEST_MUTEX_NAME TEXT("_tmp_mutex_lock_")
#define THREADTEST_EVENT_NAME TEXT("_tmp_event_")

#ifdef THREAD_EXPORTS
#define THREAD_API __declspec(dllexport)
#else
#define THREAD_API __declspec(dllimport)
#endif

class THREAD_API CLock
{
public:
	CLock();
	~CLock();

private:
	HANDLE hMutex;
};

class THREAD_API CThread
{
public:
	CThread() : hThread(0), dwThreadId(0), dwState(0), lpUserData(0), lpParameter(0){}
	~CThread();

	HANDLE Create(LPVOID lpParameter, DWORD dwInitialState = STATE_ASYNC, DWORD dwCreationFlag = 0);
	void Join(DWORD dwWaitInterval = INFINITE);
	DWORD Suspend();
	DWORD Resume();
	void SetUserData(void* lpUserData);
	void* GetUserData() const;
	DWORD GetId() const;
	HANDLE GetHandle() const;
	DWORD GetAsyncState() const;
	DWORD GetState() const;
	void SetState(DWORD dwNewState);
	BOOL Alert();

protected:
	virtual void Run(LPVOID lpParameter = 0) = 0;
	LPVOID lpParameter;

private:
	static DWORD WINAPI StartAddress(LPVOID lpParameter);
	HANDLE hThread;
	DWORD dwThreadId;
	DWORD dwState;
	void* lpUserData;
	HANDLE hEvent;
};

#endif

