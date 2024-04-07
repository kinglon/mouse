#include "stdafx.h"
#include "MyMessageBox.h"
#include "ResourceUtil.h"


CMyMessageBox::CMyMessageBox()
{
}


CMyMessageBox::~CMyMessageBox()
{
}


void CMyMessageBox::InitWindow()
{
	__super::InitWindow();

	m_PaintManager.FindControl(L"textLabel")->SetText(m_text.c_str());
	m_PaintManager.FindControl(L"cancelArea")->SetVisible(m_showCancel);
}

void CMyMessageBox::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
}

CDuiString CMyMessageBox::GetSkinFolder()
{
	return CResourceUtil::GetSkinFolder().c_str();
}

CDuiString CMyMessageBox::GetSkinFile()
{
	return L"message_box.xml";
}

LPCTSTR CMyMessageBox::GetWindowClassName(void) const
{
	return L"MouseMessageBox";
}

LRESULT CMyMessageBox::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return __super::HandleMessage(uMsg, wParam, lParam);
}

bool CMyMessageBox::Show(HWND hParentWnd, std::wstring message, bool showCancel)
{
	CMyMessageBox messageBox;
	messageBox.SetText(message);
	messageBox.SetShowCancel(showCancel);
	messageBox.Create(hParentWnd, NULL, WS_VISIBLE | WS_POPUP, 0);
	messageBox.CenterWindow();
	return messageBox.ShowModal() == 1;
}

void CMyMessageBox::OnOk(TNotifyUI& msg)
{
	Close(1);
}

void CMyMessageBox::OnCancel(TNotifyUI& msg)
{
	Close(0);
}