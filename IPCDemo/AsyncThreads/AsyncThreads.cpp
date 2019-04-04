#include "stdafx.h"

#define THREAD_NUMBER 4
#define WINDOWS_NUMBER 3
#define SLEEP_TIME 1000

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

class Thread : public CThread
{
protected:
	virtual void Run(LPVOID lpParameter = 0);

public:
	int id;
};

void Thread::Run(LPVOID lpParameter)
{
	srand((unsigned)time(NULL) + id);
	HWND hWnd = (HWND)GetUserData();
	RECT rect;

	BOOL bError = GetClientRect(hWnd, &rect);
	if (!bError)
	{
		return;
	}

	int iClientX = rect.right - rect.left;
	int iClientY = rect.bottom - rect.top;
	if (iClientX <= 0 || iClientY <= 0)
	{
		return;
	}

	HDC hDC = GetDC(hWnd);
	if (hDC)
	{
		for (int iCount = 0; iCount < WINDOWS_NUMBER; iCount++)
		{
			int iStartX = (int)(rand() % iClientX);
			int iStartY = (int)(rand() % iClientY);
			int iStopX = (int)(rand() % iClientX);
			int iStopY = (int)(rand() % iClientY);

			int iRed = rand() & 255;
			int iGreen = rand() & 255;
			int iBlue = rand() & 255;

			HANDLE hBrush = CreateSolidBrush(GetNearestColor(hDC, RGB(iRed, iGreen, iBlue)));
			HANDLE hbrOld = SelectObject(hDC, hBrush);

			if (0 == rand() % 2)
				Rectangle(hDC, min(iStartX, iStopX), max(iStartX, iStopX), min(iStartY, iStopY), max(iStartY, iStopY));
			else
				Ellipse(hDC, min(iStartX, iStopX), max(iStartX, iStopX), min(iStartY, iStopY), max(iStartY, iStopY));
			DeleteObject(hbrOld);
		}

		ReleaseDC(hWnd, hDC);
	}
	Sleep(SLEEP_TIME);
	return;
}

int WINAPI _tWinMain(HINSTANCE hThis, HINSTANCE hPrev, LPTSTR szCmdLine, int iCmdShow)
{
	UNREFERENCED_PARAMETER(hPrev);
	UNREFERENCED_PARAMETER(szCmdLine);

	//实例化窗口结构
	WNDCLASSEX wndEx = { 0 };
	wndEx.cbClsExtra = 0;
	wndEx.cbSize = sizeof(wndEx);
	wndEx.cbWndExtra = 0;
	wndEx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndEx.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndEx.hInstance = hThis;
	wndEx.lpfnWndProc = WindowProcedure;
	wndEx.lpszClassName = TEXT("async_thread");
	wndEx.lpszMenuName = NULL;
	wndEx.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	if (!RegisterClassEx(&wndEx))
		return -1;

	HWND hWnd = CreateWindow(wndEx.lpszClassName, TEXT("Basic Thread Management"), WS_OVERLAPPEDWINDOW, 200, 200, 840, 440, HWND_DESKTOP, NULL, hThis, 0);
	if (!hWnd)
		return -1;

	HWND hRects[THREAD_NUMBER];
	hRects[0] = CreateWindow(TEXT("STATIC"), TEXT(""), WS_BORDER | WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 20, 20, 180, 350, hWnd, NULL, hThis, NULL);
	hRects[1] = CreateWindow(TEXT("STATIC"), TEXT(""), WS_BORDER | WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 220, 20, 180, 350, hWnd, NULL, hThis, NULL);
	hRects[2] = CreateWindow(TEXT("STATIC"), TEXT(""), WS_BORDER | WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 420, 20, 180, 350, hWnd, NULL, hThis, NULL);
	hRects[3] = CreateWindow(TEXT("STATIC"), TEXT(""), WS_BORDER | WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 620, 20, 180, 350, hWnd, NULL, hThis, NULL);

	UpdateWindow(hWnd);
	ShowWindow(hWnd, SW_SHOW);

	Thread threads[THREAD_NUMBER];
	for (int iIndex = 0; iIndex < THREAD_NUMBER; iIndex++)
	{
		threads[iIndex].id = iIndex;
		threads[iIndex].Create(NULL, STATE_ASYNC | STATE_CONTINUOUS);
		threads[iIndex].SetUserData(hRects[iIndex]);
		Sleep(20);
	}
	SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)threads);

	MSG msg = { 0 };
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnregisterClass(wndEx.lpszClassName, hThis);
	return 0;
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
		case WM_CLOSE:
		{
			Thread* pThread = (Thread*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			pThread->Alert();
			for (int iIndex = 0; iIndex < THREAD_NUMBER; iIndex++)
			{
				pThread[iIndex].Join();
			}
			DestroyWindow(hWnd);
			break;
		}
		default:
		{
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}
	return 0;
}