#include "stdafx.h"
#include "DpiLockWindow.h"
#include "ResourceUtil.h"
#include "MyMessageBox.h"


CDpiLockWindow::CDpiLockWindow()
{
}


CDpiLockWindow::~CDpiLockWindow()
{
}


void CDpiLockWindow::InitWindow()
{
	__super::InitWindow();
}

void CDpiLockWindow::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
}

CDuiString CDpiLockWindow::GetSkinFolder()
{
	return CResourceUtil::GetSkinFolder().c_str();
}

CDuiString CDpiLockWindow::GetSkinFile()
{
	return L"dpilock_window.xml";
}

LPCTSTR CDpiLockWindow::GetWindowClassName(void) const
{
	return L"MouseDpiLock";
}

LRESULT CDpiLockWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void CDpiLockWindow::OnOk(TNotifyUI& msg)
{
	m_dpiValue = _wtoi(m_PaintManager.FindControl(L"dpiValueEdit")->GetText().GetData());
	if (m_dpiValue < 200 || m_dpiValue > 26000 || m_dpiValue % 100 != 0)
	{
		CMyMessageBox::Show(GetHWND(), L"DPI必须是200-26000而且100整数倍");
		return;
	}
	Close(1);
}

void CDpiLockWindow::OnCancel(TNotifyUI& msg)
{
	Close(0);
}