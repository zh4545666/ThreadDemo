#include "stdafx.h"
#include "CThread.h"

CLock::CLock()
{
	hMutex = CreateMutex(NULL, FALSE, THREADTEST_MUTEX_NAME);
	WaitForSingleObject(hMutex, INFINITE);
}

CLock::~CLock()
{
	ReleaseMutex(hMutex);
	CloseHandle(hMutex);
}

CThread::~CThread()
{

}

HANDLE CThread::Create(LPVOID lpParameter, DWORD dwInitialState, DWORD dwCreationFlag)
{
	dwState |= dwInitialState;
	this->lpParameter = lpParameter;
	if (dwState & STATE_ALIVE)
	{
		return hThread;
	}
	hThread = CreateThread(NULL, 0, StartAddress, this, dwCreationFlag, &dwThreadId);
	dwState |= STATE_ALIVE;
	if (dwState & STATE_CONTINUOUS)
	{
		hEvent = CreateEvent(NULL, TRUE, FALSE, THREADTEST_EVENT_NAME);
	}
	return hThread;
}

void CThread::Join(DWORD dwWaitInterval)
{
	if (dwState & STATE_BLOCKED)
	{
		return;
	}
	if (dwState & STATE_READY)
	{
		return;
	}
	dwState |= STATE_READY;
	WaitForSingleObject(hThread, dwWaitInterval);
	dwState ^= STATE_READY;
}

DWORD CThread::Suspend()
{
	if (dwState & STATE_BLOCKED)
	{
		return -1;
	}
	if (dwState & STATE_READY)
	{
		return -1;
	}
	DWORD dwSuspendCount = SuspendThread(hThread);
	dwState |= STATE_BLOCKED;

	return dwSuspendCount;
}

DWORD CThread::Resume()
{
	if (dwState & STATE_RUNNING)
	{
		return -1;
	}
	DWORD dwSuspendCount = ResumeThread(hThread);
	dwState ^= STATE_BLOCKED;
	return dwSuspendCount;
}

void CThread::SetUserData(void* lpUserData)
{
	this->lpUserData = lpUserData;
}

void* CThread::GetUserData() const
{
	return lpUserData;
}

DWORD CThread::GetAsyncState() const
{
	if (dwState & STATE_SYNC)
	{
		return STATE_SYNC;
	}
	return STATE_ASYNC;
}

DWORD CThread::GetState() const
{
	return dwState;
}

void CThread::SetState(DWORD dwNewState)
{
	dwState = 0;
	dwState |= dwNewState;
}

BOOL CThread::Alert()
{
	return SetEvent(hEvent);
}

DWORD WINAPI CThread::StartAddress(LPVOID lpParameter)
{
	CThread* pThread = (CThread*)lpParameter;
	if (pThread->GetAsyncState() == STATE_ASYNC)
	{
		if (pThread->dwState & STATE_CONTINUOUS)
		{
			DWORD dwWaitStatus = 0;
			while (true)
			{
				pThread->Run(pThread->lpParameter);
				dwWaitStatus = WaitForSingleObject(pThread->hEvent, 10);
				if (dwWaitStatus == WAIT_OBJECT_0)
				{
					break;
				}
			}
			return 0;
		}

		pThread->Run(pThread->lpParameter);
		return 0;
	}

	if (pThread->dwState & STATE_CONTINUOUS)
	{
		DWORD dwWaitStatus = 0;
		while (true)
		{
			CLock lock;
			pThread->Run(pThread->lpParameter);
			dwWaitStatus = WaitForSingleObject(pThread->hEvent, 10);
			if (dwWaitStatus == WAIT_OBJECT_0)
			{
				break;
			}
		}
		return 0;
	}

	CLock lock;
	pThread->Run(pThread->lpParameter);
	return 0;
}

inline DWORD CThread::GetId() const
{
	return dwThreadId;
}

inline HANDLE CThread::GetHandle() const
{
	return hThread;
}
