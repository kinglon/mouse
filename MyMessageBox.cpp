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

void CMyMessageBox::OnOk(TNotifyUI& msg)
{
	Close();
}