#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#define BUFFER_SIZE 4096

DWORD WINAPI StartAddress(LPVOID lpParameter);

int _tmain(void)
{
	LPTSTR szPipename = TEXT("\\\\.\\pipe\\basicMPI");

	while (true)
	{
		_tprintf(TEXT("basicMPI::waiting client connection\n"));

		HANDLE hPipe = CreateNamedPipe(szPipename, PIPE_ACCESS_DUPLEX, PIPE_WAIT | PIPE_READMODE_MESSAGE | PIPE_TYPE_MESSAGE,
			PIPE_UNLIMITED_INSTANCES, BUFFER_SIZE, BUFFER_SIZE, 0, NULL);

		if (hPipe == INVALID_HANDLE_VALUE)
		{
			_tprintf(TEXT("CreateNamePipe faild !Error code:[%u]\n"), GetLastError());
			return 2;
		}

		if (ConnectNamedPipe(hPipe, NULL))
		{
			_tprintf(TEXT("Connection succeeded.\n"));
			HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartAddress, hPipe, 0, NULL);
			CloseHandle(hThread);
		}
	}

	return 0;
}

DWORD WINAPI StartAddress(LPVOID lpParameter)
{
	HANDLE hPipe = (HANDLE)lpParameter;
	PTCHAR szRequest = (PTCHAR)HeapAlloc(GetProcessHeap(), 0, BUFFER_SIZE * sizeof(TCHAR));
	PTCHAR szReply = (PTCHAR)HeapAlloc(GetProcessHeap(), 0, BUFFER_SIZE * sizeof(TCHAR));
	DWORD dwBytesRead = 0;
	DWORD dwReplyBytes = 0;
	DWORD dwBytesWritten = 0;

	_tprintf(TEXT("Waiting messages.\n"));

	if (!ReadFile(hPipe, szRequest, BUFFER_SIZE * sizeof(TCHAR), &dwBytesRead, NULL))
	{
		_tprintf(TEXT("ReadFile faild! Error code:[%u]\n"), GetLastError());
		DisconnectNamedPipe(hPipe);
		CloseHandle(hPipe);
		HeapFree(GetProcessHeap(), 0, szRequest);
		HeapFree(GetProcessHeap(), 0, szReply);
		return 2;
	}

	_tprintf(TEXT("Client request:\"%s\"\n"), szRequest);
	_tcscpy_s(szReply, BUFFER_SIZE, TEXT("default answer from server"));
	dwReplyBytes = (_tcslen(szReply) + 1) * sizeof(TCHAR);

	if (!WriteFile(hPipe, szReply, dwReplyBytes, &dwBytesWritten, NULL))
	{
		_tprintf(TEXT("WriteFile faild! Error code:[%u]"), GetLastError());
		DisconnectNamedPipe(hPipe);
		CloseHandle(hPipe);
		HeapFree(GetProcessHeap(), 0, szRequest);
		HeapFree(GetProcessHeap(), 0, szReply);
		return 2;
	}
	FlushFileBuffers(hPipe);
	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);

	HeapFree(GetProcessHeap(), 0, szRequest);
	HeapFree(GetProcessHeap(), 0, szReply);
	
	_tprintf(TEXT("Success.\n"));
	return 0;

}