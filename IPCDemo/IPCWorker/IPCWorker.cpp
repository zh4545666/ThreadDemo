// IPCWorker.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <string>

#define COMMUNICATION_OBJECT_NAME TEXT("__FILE_MAPPING__")
#define SYNCHRONIZING_MUTEX_NAME TEXT("__TEST_MUTEX__")

typedef struct _tagCOMMUNICATIONOBJECT
{
	HWND hWndClient;
	BOOL bExitLoop;
	LONG lSleepTimeout;
} COMMUNICATIONOBJECT,*PCOMMUNICATIONOBJECT;


std::string TCHAR2STRING(TCHAR *STR)
{
	int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);
	char* chRtn = new char[iLen*sizeof(char)];
	WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);
	std::string str(chRtn);
	delete chRtn;
	return str;
}

int _tmain(int argc, _TCHAR* argv[])
{
	std::cout << argc << std::endl;
	for (int i = 0; i < argc; i++)
	{
		std::string s = TCHAR2STRING(argv[i]);
		std::cout << s << std::endl;
	}

	HBRUSH hBrush = NULL;
	if (_tcscmp(TEXT("Blue"), argv[0]) == 0)
		hBrush = CreateSolidBrush(RGB(0, 0, 255));
	else
		hBrush = CreateSolidBrush(RGB(255, 0, 0));

	HWND hWnd = NULL;
	HDC hDC = NULL;
	RECT rectClient = { 0 };
	LONG lWaitTimeout = 0;
	HANDLE hMapping = NULL;
	PCOMMUNICATIONOBJECT pCommObject = NULL;
	BOOL bContinueLoop = TRUE;

	HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS,FALSE,SYNCHRONIZING_MUTEX_NAME);
	hMapping = OpenFileMapping(FILE_MAP_READ, FALSE, COMMUNICATION_OBJECT_NAME);
	if (hMapping)
	{
		while (bContinueLoop)
		{
			WaitForSingleObject(hMutex,INFINITE);
			pCommObject = (PCOMMUNICATIONOBJECT)MapViewOfFile(hMapping,FILE_MAP_READ,0,0,sizeof(COMMUNICATIONOBJECT));

			if (pCommObject)
			{
				bContinueLoop = !pCommObject->bExitLoop;
				hWnd = pCommObject->hWndClient;
				lWaitTimeout = pCommObject->lSleepTimeout;
				UnmapViewOfFile(pCommObject);
				hDC = GetDC(hWnd);
				if (GetClientRect(hWnd, &rectClient))
				{
					FillRect(hDC, &rectClient, hBrush);
				}
				ReleaseDC(hWnd, hDC);
				Sleep(lWaitTimeout);
			}
			ReleaseMutex(hMutex);
		}
	}

	CloseHandle(hMapping);
	CloseHandle(hMutex);
	DeleteObject(hBrush);

	return 0;
}

