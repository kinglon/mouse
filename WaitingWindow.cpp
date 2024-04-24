#include "stdafx.h"
#include "WaitingWindow.h"
#include "ResourceUtil.h"


#define TIMERID 1000

CWaitingWindow::CWaitingWindow()
{
}


CWaitingWindow::~CWaitingWindow()
{
}


void CWaitingWindow::InitWindow()
{
	__super::InitWindow();

	m_PaintManager.FindControl(L"caption")->SetText(m_caption.c_str());
	CProgressUI* progress = (CProgressUI*)m_PaintManager.FindControl(L"progress");
	progress->SetMaxValue(m_waitSeconds);
	progress->SetValue(0);
	SetTimer(GetHWND(), TIMERID, 1000, NULL);
}

void CWaitingWindow::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
}

CDuiString CWaitingWindow::GetSkinFolder()
{
	return CResourceUtil::GetSkinFolder().c_str();
}

CDuiString CWaitingWindow::GetSkinFile()
{
	return L"waiting_window.xml";
}

LPCTSTR CWaitingWindow::GetWindowClassName(void) const
{
	return L"MouseWaitingWindow";
}

LRESULT CWaitingWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_TIMER)
	{
		CProgressUI* progress = (CProgressUI*)m_PaintManager.FindControl(L"progress");
		int value = progress->GetValue() + 1;
		progress->SetValue(value);
		if (m_success || value >= progress->GetMaxValue())
		{
			KillTimer(GetHWND(), TIMERID);
			Close();
		}

		return 0L;
	}

	return __super::HandleMessage(uMsg, wParam, lParam);
}