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

//^ �йܵ�ָ��
//* ���йܵ�ָ��
//% �йܵ�����
//& ���йܵ�����
//initonly�ؼ��֣���ȫ�Ĳ���������

//��������
// AutoResetEvent,ManualResetEvent,ManualResetEventSlim,CountDownEvent,Barrier,Wait,Paulse��;