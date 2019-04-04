#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#define BUFFER_SIZE 4096

int _tmain(void)
{
	TCHAR szBuffer[BUFFER_SIZE];
	DWORD dwRead = 0;
	DWORD dwWritten = 0;
	DWORD dwMode = PIPE_READMODE_MESSAGE;
	LPTSTR szPipename = TEXT("\\\\.\\pipe\\basicMPI");
	LPTSTR szMessage = TEXT("Message from client.");
	DWORD dwToWrite = _tcslen(szMessage) * sizeof(TCHAR);

	//or	CallNamedPipe(...)
	HANDLE hPipe = CreateFile(szPipename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	WaitNamedPipe(szPipename, INFINITE);
	SetNamedPipeHandleState(hPipe, &dwMode, NULL, NULL);
	if (!WriteFile(hPipe, szMessage, dwToWrite, &dwWritten, NULL))
	{
		CloseHandle(hPipe);
		return -1;
	}

	while (ReadFile(hPipe, szBuffer, BUFFER_SIZE * sizeof(TCHAR), &dwRead, NULL) && GetLastError() != ERROR_MORE_DATA)
	{
		_tprintf(TEXT("%s"), szBuffer);
	}

	_tprintf(TEXT("\nSuccess!"));
	CloseHandle(hPipe);
	system("Pause");	
	return 0;
}