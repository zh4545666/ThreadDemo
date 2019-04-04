#include <windows.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

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
	wndEx.lpszClassName = TEXT("GUIProject");
	wndEx.lpszMenuName = NULL;
	wndEx.style = CS_HREDRAW | CS_VREDRAW;
	if (!RegisterClassEx(&wndEx))
		return -1;

	HWND hWnd = CreateWindow(wndEx.lpszClassName, TEXT("GUIProject"), WS_OVERLAPPEDWINDOW, 200, 200, 400, 300, HWND_DESKTOP, NULL, hInstance, 0);
	if (!hWnd)
		return -1;

	UpdateWindow(hWnd);
	ShowWindow(hWnd, nShowCmd);

	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	DestroyWindow(hWnd);
	UnregisterClass(wndEx.lpszClassName, hInstance);

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			break;
		}
		default:
		{
			return DefWindowProc(hWnd, nMsg, wParam, lParam);
		}
	}

	return 0;
}

