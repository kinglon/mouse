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
	
	m_PaintManager.AddMessageFilter(this);

	// 设置标题
	m_PaintManager.FindControl(L"title")->SetText(m_title.c_str());	

	// 设置按键
	if (m_vkCode != 0)
	{
		std::wstring keyName = CMacroEventMapping::GetKeyName(m_vkCode);
		if (!keyName.empty())
		{
			m_PaintManager.FindControl(L"keyName")->SetText(keyName.c_str());
		}
	}

	// 设置特殊按键状态
	if (m_vkCodeState & 0x01)
	{
		COptionUI* option = (COptionUI*)m_PaintManager.FindControl(L"ctrlOption");
		option->Selected(true);
	}

	if (m_vkCodeState & 0x02)
	{
		COptionUI* option = (COptionUI*)m_PaintManager.FindControl(L"shiftOption");
		option->Selected(true);
	}

	if (m_vkCodeState & 0x04)
	{
		COptionUI* option = (COptionUI*)m_PaintManager.FindControl(L"altOption");
		option->Selected(true);
	}

	if (m_vkCodeState & 0x08)
	{
		COptionUI* option = (COptionUI*)m_PaintManager.FindControl(L"winOption");
		option->Selected(true);
	}
}

void CGetVkCodeWindow::OnFinalMessage(HWND hWnd)
{
	m_PaintManager.RemoveMessageFilter(this);

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
	if (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN)
	{
		OnKey(true, wParam, lParam);
		return 0L;
	}
	
	return __super::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CGetVkCodeWindow::ResponseDefaultKeyEvent(WPARAM wParam)
{
	if (wParam == VK_ESCAPE)
	{
		return FALSE;
	}

	return __super::ResponseDefaultKeyEvent(wParam);
}

LRESULT CGetVkCodeWindow::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	bHandled = false;
	if (uMsg == WM_KEYDOWN && wParam == VK_TAB)
	{
		OnKey(true, wParam, lParam);
		bHandled = true;
		return 0L;
	}

	return __super::MessageHandler(uMsg, wParam, lParam, bHandled);
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

	// 获取特殊按键配置
	m_vkCodeState = 0;
	COptionUI* option = (COptionUI*)m_PaintManager.FindControl(L"ctrlOption");
	if (option->IsSelected())
	{
		m_vkCodeState += 0x01;
	}

	option = (COptionUI*)m_PaintManager.FindControl(L"shiftOption");
	if (option->IsSelected())
	{
		m_vkCodeState += 0x02;
	}

	option = (COptionUI*)m_PaintManager.FindControl(L"altOption");
	if (option->IsSelected())
	{
		m_vkCodeState += 0x04;
	}

	option = (COptionUI*)m_PaintManager.FindControl(L"winOption");
	if (option->IsSelected())
	{
		m_vkCodeState += 0x08;
	}

	Close(1);
}

void CGetVkCodeWindow::OnCancel(TNotifyUI& msg)
{
	Close(0);
}

unsigned char CGetVkCodeWindow::GetSpecialKeyState()
{
	// 获取Ctrl、Shift、Alt按键是否按下的状态
	// 第0-7bit各代表left control, left shift, left alt, left win, right control, right shift, right alt, right win, 1 down, 0 up
	unsigned char keyState = 0;
	SHORT state = GetKeyState(VK_LCONTROL);  // 最高位1表示donw, 0表示up
	keyState |= ((state >> 15) & 0X01);
	state = GetKeyState(VK_LSHIFT);
	keyState |= ((state >> 14) & 0X02);
	state = GetKeyState(VK_LMENU);
	keyState |= ((state >> 13) & 0X04);
	state = GetKeyState(VK_LWIN);
	keyState |= ((state >> 12) & 0X08);
	state = GetKeyState(VK_RCONTROL);
	keyState |= ((state >> 11) & 0X10);
	state = GetKeyState(VK_RSHIFT);
	keyState |= ((state >> 10) & 0X20);
	state = GetKeyState(VK_RMENU);
	keyState |= ((state >> 9) & 0X40);
	state = GetKeyState(VK_RWIN);
	keyState |= ((state >> 8) & 0X80);
	return keyState;
}