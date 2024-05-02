#include "stdafx.h"
#include "GetOptionWindow.h"
#include "ResourceUtil.h"
#include "MyMessageBox.h"


CGetOptionWindow::CGetOptionWindow()
{
}


CGetOptionWindow::~CGetOptionWindow()
{
}


void CGetOptionWindow::InitWindow()
{
	__super::InitWindow();

	m_PaintManager.FindControl(L"title")->SetText(m_title.c_str());

	CComboUI* combo = (CComboUI*)m_PaintManager.FindControl(L"combo");
	for (auto& option : m_options)
	{
		CListLabelElementUI* pControl = new CListLabelElementUI();
		pControl->SetText(option.c_str());
		combo->Add(pControl);
	}

	SIZE dropBox;
	dropBox.cx = 0;
	dropBox.cy = m_options.size() * combo->GetFixedHeight() + 2;
	combo->SetDropBoxSize(dropBox);

	combo->SelectItem(0);
}

void CGetOptionWindow::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
}

CDuiString CGetOptionWindow::GetSkinFolder()
{
	return CResourceUtil::GetSkinFolder().c_str();
}

CDuiString CGetOptionWindow::GetSkinFile()
{
	return L"get_option_window.xml";
}

LPCTSTR CGetOptionWindow::GetWindowClassName(void) const
{
	return L"GetOptionWindow";
}

LRESULT CGetOptionWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void CGetOptionWindow::OnOk(TNotifyUI& msg)
{
	CComboUI* combo = (CComboUI*)m_PaintManager.FindControl(L"combo");
	m_selectedOption = combo->GetText().GetData();
	if (m_selectedOption.empty())
	{
		CMyMessageBox::Show(GetHWND(), L"请选择一个选项");
		return;
	}

	Close(1);
}

void CGetOptionWindow::OnCancel(TNotifyUI& msg)
{
	Close(0);
}