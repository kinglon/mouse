#include "stdafx.h"
#include "GetVkCodeWindow.h"
#include "ResourceUtil.h"
#include "MyMessageBox.h"
#include "MacroEventMapping.h"


CGetVkCodeWindow::CGetVkCodeWindow()
{
}


CGetVkCodeWindow::~CGetVkCodeWindow()
{
}


void CGetVkCodeWindow::InitWindow()
{
	__super::InitWindow();
}

void CGetVkCodeWindow::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
}

CDuiString CGetVkCodeWindow::GetSkinFolder()
{
	return CResourceUtil::GetSkinFolder().c_str();
}

CDuiString CGetVkCodeWindow::GetSkinFile()
{
	return L"get_vkcode_window.xml";
}

LPCTSTR CGetVkCodeWindow::GetWindowClassName(void) const
{
	return L"GetVkCodeWindow";
}

LRESULT CGetVkCodeWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_KEYDOWN)
	{
		OnKey(true, wParam, lParam);
		return 0L;
	}

	return __super::HandleMessage(uMsg, wParam, lParam);
}

void CGetVkCodeWindow::OnKey(bool down, WPARAM wParam, LPARAM lParam)
{
	// 长按只需处理一个
	DWORD previousState = ((lParam >> 30) & 0x01);
	if (down && previousState == 1)
	{
		return;
	}

	int virtualKey = (int)wParam;
	std::wstring keyName = CMacroEventMapping::GetKeyName(virtualKey);
	if (keyName.empty())
	{
		return;
	}

	m_PaintManager.FindControl(L"keyName")->SetText(keyName.c_str());
	m_vkCode = virtualKey;
}

void CGetVkCodeWindow::OnOk(TNotifyUI& msg)
{	
	if (m_vkCode == 0)
	{
		CMyMessageBox::Show(GetHWND(), L"必须输入一个按键");
		return;
	}

	Close(1);
}

void CGetVkCodeWindow::OnCancel(TNotifyUI& msg)
{
	Close(0);
}