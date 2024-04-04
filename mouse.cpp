#include "stdafx.h"
#include <shlwapi.h>
#include "MainWindow.h"
#include "LogUtil.h"
#include "DumpUtil.h"
#include "ImPath.h"
#include "SettingManager.h"

using namespace std;

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

	g_dllLog = CLogUtil::GetLog(L"mouse");

	//��ʼ������ת������
	CDumpUtil::SetDumpFilePath(CImPath::GetDumpPath().c_str());
	CDumpUtil::Enable(true);

	int nLogLevel = CSettingManager::GetInstance()->m_logLevel;
	g_dllLog->SetLogLevel((ELogLevel)nLogLevel);

	_SetProcessDPIAware();

	//��ʼ��COM
	HRESULT Hr = ::CoInitialize(NULL);
	if (FAILED(Hr))
	{
		return FALSE;
	}

	//��ʼ��Duilib��
	CPaintManagerUI::SetInstance(hInstance);

	CMainWindow mainWindow;
	mainWindow.Create(NULL, NULL, WS_VISIBLE | WS_OVERLAPPEDWINDOW, WS_EX_APPWINDOW);
	mainWindow.CenterWindow();
	mainWindow.ShowModal();

	::CoUninitialize();

	return 0;
}
