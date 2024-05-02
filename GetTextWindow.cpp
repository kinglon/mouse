#include "stdafx.h"
#include "GetTextWindow.h"
#include "ResourceUtil.h"
#include "MyMessageBox.h"


CGetTextWindow::CGetTextWindow()
{
}


CGetTextWindow::~CGetTextWindow()
{
}


void CGetTextWindow::InitWindow()
{
	__super::InitWindow();

	m_PaintManager.FindControl(L"title")->SetText(m_title.c_str());
}

void CGetTextWindow::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
}

CDuiString CGetTextWindow::GetSkinFolder()
{
	return CResourceUtil::GetSkinFolder().c_str();
}

CDuiString CGetTextWindow::GetSkinFile()
{
	return L"get_text_window.xml";
}

LPCTSTR CGetTextWindow::GetWindowClassName(void) const
{
	return L"GetTextWindow";
}

LRESULT CGetTextWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void CGetTextWindow::OnOk(TNotifyUI& msg)
{
	m_inputContent = ((CEditUI*)m_PaintManager.FindControl(L"inputText"))->GetText().GetData();
	if (m_inputContent.empty())
	{
		CMyMessageBox::Show(GetHWND(), L"输入不能为空");
		return;
	}

	Close(1);
}

void CGetTextWindow::OnCancel(TNotifyUI& msg)
{
	Close(0);
}