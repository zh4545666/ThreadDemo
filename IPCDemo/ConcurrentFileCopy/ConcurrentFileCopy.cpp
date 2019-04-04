#include "ConcurrentFileCopy.h"

TCHAR* szTitle = TEXT("Concurrent_file_copy");
TCHAR* szWindowClass = TEXT("__CFC_WND_CLASS__");

DWORD dwReadBytes = 0;
DWORD dwWriteBytes = 0;

DWORD dwBlockSize = 0;
DWORD dwFileSize = 0;
HLOCAL pMemory = NULL;

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPTSTR szCmdLine, int iCmdShow)
{
	UNREFERENCED_PARAMETER(hPrev);
	UNREFERENCED_PARAMETER(szCmdLine);

	RegisterWndClass(hInstance);

	HWND hWnd = NULL;
	HWND hWndPB = NULL;

	if (!(hWnd = InitializeInstance(hInstance, iCmdShow, hWndPB)))
	{
		return 1;
	}

	MSG msg = { 0 };
	TCHAR szReadFile[MAX_PATH];
	TCHAR szWriteFile[MAX_PATH];

	if (FileDialog(hWnd, szReadFile, FILE_OPEN) && FileDialog(hWnd, szWriteFile, FILE_SAVE))
	{
		COPYDETAILS copyDetails = { hInstance, hWndPB, szReadFile, szWriteFile };
		HANDLE hMutex = CreateMutex(NULL, FALSE, MUTEX_NAME);
		HANDLE hReadThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReadRoutine, &copyDetails, 0, NULL);

		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		CloseHandle(hMutex);
		CloseHandle(hReadThread);
	}
	else
	{
		MessageBox(hWnd,TEXT("Cannot open file!"),TEXT("Error"),MB_OK);
	}

	LocalFree(pMemory);
	UnregisterClass(szWindowClass, hInstance);
	return (int)msg.wParam;
}

ATOM RegisterWndClass(HINSTANCE hInstance)
{
	//实例化窗口结构
	WNDCLASSEX wndEx = { 0 };
	wndEx.cbClsExtra = 0;
	wndEx.cbSize = sizeof(wndEx);
	wndEx.cbWndExtra = 0;
	wndEx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndEx.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndEx.hInstance = hInstance;
	wndEx.lpfnWndProc = WndProc;
	wndEx.lpszClassName = szWindowClass;
	wndEx.lpszMenuName = NULL;
	wndEx.style = CS_HREDRAW | CS_VREDRAW;

	return RegisterClassEx(&wndEx);
}

HWND InitializeInstance(HINSTANCE hInstance, int iCmdShow, HWND& hWndPB)
{
	HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, 200, 200, 400, 290, NULL, NULL, hInstance, NULL);
	RECT rcClient = { 0 };
	int cyVScroll = 0;

	if (!hWnd)
	{
		return NULL;
	}

	HFONT hFont = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, BALTIC_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, TEXT("Arial"));
	HWND hButton = CreateWindow(TEXT("BUTTON"), TEXT("Close"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, 310, 200, 100, 25, hWnd, (HMENU)BUTTON_CLOSE, hInstance, NULL);
	SendMessage(hButton, WM_SETFONT, (WPARAM)hFont, TRUE);

	GetClientRect(hWnd, &rcClient);
	cyVScroll = GetSystemMetrics(SM_CYVSCROLL);

	hWndPB = CreateWindow(PROGRESS_CLASS, (LPTSTR)NULL, WS_CHILD | WS_VISIBLE, rcClient.left, rcClient.bottom - cyVScroll, rcClient.right, cyVScroll, hWnd, (HMENU)0, hInstance, NULL);
	SendMessage(hWndPB, PBM_SETSTEP, (WPARAM)1, 0);

	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);

	return hWnd;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case BUTTON_CLOSE:
				{
					DestroyWindow(hWnd);
				}break;
				default:
					break;
			}
		}break;
		case WM_DESTROY:
		{
			PostQuitMessage(0);
		}break;
		default:
		{
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}break;
	}
	return 0;
}

DWORD WINAPI ReadRoutine(LPVOID lpParameter)
{
	PCOPYDETAILS pCopyDetails = (PCOPYDETAILS)lpParameter;
	HANDLE hFile = CreateFile(pCopyDetails->szReadFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == (HANDLE)INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	dwFileSize = GetFileSize(hFile, NULL);
	dwBlockSize = GetBlockSize(dwFileSize);
	HANDLE hWriteThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)WriteRoutine,pCopyDetails,0,NULL);
	size_t uBufferLength = (size_t)ceil((double)dwFileSize / dwBlockSize);
	SendMessage(pCopyDetails->hWndPB, PBM_SETRANGE, 0, MAKEWPARAM(0, uBufferLength));
	pMemory = LocalAlloc(LPTR, dwFileSize);
	void* pBuffer = LocalAlloc(LPTR, dwBlockSize);

	int iOffset = 0;
	DWORD dwBytesRed = 0;

	do
	{
		ReadFile(hFile, pBuffer, dwBlockSize, &dwBytesRed, NULL);
		if (!dwBytesRed)
			break;
		HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, MUTEX_NAME);
		WaitForSingleObject(hMutex, INFINITE);
		memcpy((char*)pMemory + iOffset, pBuffer, dwBytesRed);
		ReleaseMutex(hMutex);
		dwReadBytes += dwBytesRed;
		iOffset += (int)dwBlockSize;
	} while (true);

	LocalFree(pBuffer);
	CloseHandle(hFile);
	CloseHandle(hWriteThread);
	return 0;
}

DWORD WINAPI WriteRoutine(LPVOID lpParameter)
{
	PCOPYDETAILS pCopyDetals = (PCOPYDETAILS)lpParameter;
	HANDLE hFile = CreateFile(pCopyDetals->szWriteFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == (HANDLE)INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	DWORD dwBytesWritten = 0;
	int iOffset = 0;

	do
	{
		int iRemainingBytes = (int)dwFileSize - iOffset;
		if (iRemainingBytes <= 0)
		{
			break;
		}
		Sleep(10);
		if (dwWriteBytes < dwReadBytes)
		{
			DWORD dwBytesToWrite = dwBlockSize;
			if (!(dwFileSize / dwBlockSize))
			{
				dwBytesToWrite = (DWORD)iRemainingBytes;
			}
			HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, MUTEX_NAME);
			WaitForSingleObject(hMutex, INFINITE);
			WriteFile(hFile, (char*)pMemory + iOffset, dwBytesToWrite, &dwBytesWritten, NULL);
			ReleaseMutex(hMutex);
			dwWriteBytes += dwBytesWritten;
			SendMessage(pCopyDetals->hWndPB, PBM_STEPIT, 0, 0);
			iOffset += (int)dwBlockSize;
		}
	} while (true);

	CloseHandle(hFile);
	return 0;
}

BOOL FileDialog(HWND hWnd, LPTSTR szFileName, DWORD dwFileOperation)
{
	OPENFILENAME ofn;
	TCHAR szFile[MAX_PATH];

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = TEXT("ALL\0*.*\0Text\0*.TXT\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = dwFileOperation == FILE_OPEN ? OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST : OFN_SHOWHELP | OFN_OVERWRITEPROMPT;

	if (dwFileOperation == FILE_OPEN)
	{
		if (GetOpenFileName(&ofn) == TRUE)
		{
			_tcscpy_s(szFileName, MAX_PATH - 1, szFile);
			return TRUE;
		}
	}
	else
	{
		if (GetSaveFileName(&ofn) == TRUE)
		{
			_tcscpy_s(szFileName, MAX_PATH - 1, szFile);
			return TRUE;
		}
	}

	return FALSE;
}

DWORD GetBlockSize(DWORD dwFileSize)
{
	return dwFileSize > 4096 ? 4096 : 512;
}