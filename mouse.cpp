#include "stdafx.h"
#include <shlwapi.h>
#include "MainWindow.h"
#include "LogUtil.h"
#include "DumpUtil.h"
#include "ImPath.h"
#include "SettingManager.h"
#include <gdiplus.h>

using namespace Gdiplus;
using namespace std;

#pragma comment(lib, "gdiplus.lib")

CLogUtil* g_dllLog = nullptr;

void _SetProcessDPIAware()
{
	HINSTANCE hUser32 = LoadLibrary(_T("user32.dll"));
	if (hUser32)
	{
		typedef BOOL(WINAPI* pFnSetProcessDPIAware)(void);
		pFnSetProcessDPIAware pSetProcessDPIAware = (pFnSetProcessDPIAware)GetProcAddress(hUser32, "SetProcessDPIAware");
		if (pSetProcessDPIAware)
		{
			pSetProcessDPIAware();
		}

		FreeLibrary(hUser32);
	}
}

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// 单实例
	const wchar_t* mutexName = L"{4ED33E4A-D69A-4D0A-8523-158D74420098}";
	HANDLE mutexHandle = CreateMutexW(nullptr, TRUE, mutexName);
	if (mutexHandle == nullptr || GetLastError() == ERROR_ALREADY_EXISTS)
	{
		MessageBox(NULL, L"程序已经在运行", L"提示", MB_OK);
		return 0;
	}

	g_dllLog = CLogUtil::GetLog(L"mouse");

	//初始化崩溃转储机制
	CDumpUtil::SetDumpFilePath(CImPath::GetDumpPath().c_str());
	CDumpUtil::Enable(true);

	int nLogLevel = CSettingManager::GetInstance()->m_logLevel;
	g_dllLog->SetLogLevel((ELogLevel)nLogLevel);

	_SetProcessDPIAware();

	//初始化COM
	HRESULT Hr = ::CoInitialize(NULL);
	if (FAILED(Hr))
	{
		return FALSE;
	}

	// Initialize GDI+
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

	//初始化Duilib库
	CPaintManagerUI::SetInstance(hInstance);

	CMainWindow mainWindow;
	mainWindow.Create(NULL, NULL, WS_VISIBLE | WS_OVERLAPPEDWINDOW, WS_EX_APPWINDOW);
	mainWindow.CenterWindow();
	mainWindow.ShowModal();

	::CoUninitialize();

	return 0;
}
