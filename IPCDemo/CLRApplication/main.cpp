#include <Windows.h>
#include <tchar.h>
#include "MyForm.h"

using namespace System::Windows::Forms;
using namespace System::Threading;
using namespace CLRApplication;

[STAThreadAttribute]
int APIENTRY _tWinMain(HINSTANCE hThis, HINSTANCE hPrev, LPTSTR szCommandLine, int iCmdShow)
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	Application::Run(gcnew MyForm());

	return 0;
}

//^ 托管的指针
//* 非托管的指针
//% 托管的引用
//& 非托管的引用
//initonly关键字，安全的并发读操作

//触发对象
// AutoResetEvent,ManualResetEvent,ManualResetEventSlim,CountDownEvent,Barrier,Wait,Paulse等;