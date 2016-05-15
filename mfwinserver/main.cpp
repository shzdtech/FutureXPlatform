#include <Windows.h> 
#include <glog/logging.h>
#include "MainForm.h"

using namespace System::Windows::Forms;

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
	)
{
	FLAGS_log_dir = std::string(".");

#ifndef _DEBUG
	TCHAR buffer[MAX_PATH];
	::GetModuleFileName(NULL, buffer, MAX_PATH);

	char exePath[MAX_PATH];
	std::wcstombs(exePath, buffer, MAX_PATH);
	google::InitGoogleLogging(exePath);
#endif

	Application::Run(gcnew Micro::Future::MainForm());

	System::Diagnostics::Process::GetCurrentProcess()->Kill();

	return 0;
}