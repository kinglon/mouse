#include "stdafx.h"
#include "MainWindow.h"
#include "ImPath.h"
#include "ImCharset.h"
#include "Resource.h"
#include <Commdlg.h>
#include <stdlib.h>
#include <time.h>
#include <shellapi.h>
#include <sstream>
#include <shlwapi.h>
#include <set>
#include <fstream>


CMainWindow::CMainWindow()
{
	m_keyMenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_KEY_MENU));
}

CMainWindow::~CMainWindow()
{
	if (m_keyMenu)
	{
		DestroyMenu(m_keyMenu);
	}
}

void CMainWindow::InitWindow()
{
	__super::InitWindow();

	SetIcon(IDI_MOUSE);
	InitControls();
}

void CMainWindow::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
}

CDuiString CMainWindow::GetSkinFolder()
{
	static std::wstring strSkinRootPath = L"";
	if (!strSkinRootPath.empty())
	{
		return strSkinRootPath.c_str();
	}

	if (PathFileExists((CImPath::GetSoftInstallPath() + L"resource\\").c_str()))
	{
		strSkinRootPath = L"resource\\";
		return strSkinRootPath.c_str();
	}
	else if (PathFileExists((CImPath::GetSoftInstallPath() + L"..\\resource\\").c_str()))
	{
		strSkinRootPath = L"..\\resource\\";
		return strSkinRootPath.c_str();
	}
	else
	{
		return L"";
	}
}

CDuiString CMainWindow::GetSkinFile()
{
	return L"main.xml";
}

LPCTSTR CMainWindow::GetWindowClassName() const
{
	return L"MouseMain";
}

LRESULT CMainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_NCLBUTTONDBLCLK)
	{
		// 禁止双击放大窗口
		return 0L;
	}
		
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void CMainWindow::OnClickEvent(TNotifyUI& msg)
{
	auto& controlName = msg.pSender->GetName();

	// 右侧设置面板是分组面板，只显示其中一个，所以显示前先关闭所有
	static std::wstring rightSettingButtons[] = \
		{L"dpiHeadBtn", L"lightHeadBtn", L"huiBaoHeadBtn", L"lodHeadBtn", L"qudouHeadBtn", L"systemSettingHeadBtn", L"sensorHeadBtn",\
		 L"matchHeadBtn"};
	for (auto& btn : rightSettingButtons)
	{
		if (controlName == btn.c_str())
		{
			CloseRightSettingPannels();
			break;
		}
	}

	if (controlName == L"minBtn")
	{
		::ShowWindow(GetHWND(), SW_MINIMIZE);
	}
	else if (controlName == L"closeBtn")
	{
		Close();
	}
	else if (controlName == L"keyHeadBtn")
	{
		m_PaintManager.FindControl(L"keyPanel")->SetVisible(true);
	}
	else if (controlName == L"keyHideBtn")
	{
		m_PaintManager.FindControl(L"keyPanel")->SetVisible(false);
	}
	else if (controlName == L"batteryHeadBtn")
	{
		m_PaintManager.FindControl(L"batteryPanel")->SetVisible(true);
	}
	else if (controlName == L"batteryHideBtn")
	{
		m_PaintManager.FindControl(L"batteryPanel")->SetVisible(false);
	}
	else if (controlName == L"powerHeadBtn")
	{
		m_PaintManager.FindControl(L"powerPanel")->SetVisible(true);
	}
	else if (controlName == L"powerHideBtn")
	{
		m_PaintManager.FindControl(L"powerPanel")->SetVisible(false);
	}
	else if (controlName == L"dpiHeadBtn")
	{
		m_PaintManager.FindControl(L"dpiPanel")->SetVisible(true);
	}
	else if (controlName == L"dpiHideBtn")
	{
		m_PaintManager.FindControl(L"dpiPanel")->SetVisible(false);
	}
	else if (controlName == L"lightHeadBtn")
	{
		m_PaintManager.FindControl(L"lightPanel")->SetVisible(true);
	}
	else if (controlName == L"lightHideBtn")
	{
		m_PaintManager.FindControl(L"lightPanel")->SetVisible(false);
	}
	else if (controlName == L"huiBaoHeadBtn")
	{
		m_PaintManager.FindControl(L"huiBaoPanel")->SetVisible(true);
	}
	else if (controlName == L"huiBaoHideBtn")
	{
		m_PaintManager.FindControl(L"huiBaoPanel")->SetVisible(false);
	}
	else if (controlName == L"lodHeadBtn")
	{
		m_PaintManager.FindControl(L"lodPanel")->SetVisible(true);
	}
	else if (controlName == L"lodHideBtn")
	{
		m_PaintManager.FindControl(L"lodPanel")->SetVisible(false);
	}
	else if (controlName == L"qudouHeadBtn")
	{
		m_PaintManager.FindControl(L"qudouPanel")->SetVisible(true);
	}
	else if (controlName == L"qudouHideBtn")
	{
		m_PaintManager.FindControl(L"qudouPanel")->SetVisible(false);
	}
	else if (controlName == L"systemSettingHeadBtn")
	{
		m_PaintManager.FindControl(L"systemSettingPanel")->SetVisible(true);
	}
	else if (controlName == L"systemSettingHideBtn")
	{
		m_PaintManager.FindControl(L"systemSettingPanel")->SetVisible(false);
	}
	else if (controlName == L"sensorHeadBtn")
	{
		m_PaintManager.FindControl(L"sensorPanel")->SetVisible(true);
	}
	else if (controlName == L"sensorHideBtn")
	{
		m_PaintManager.FindControl(L"sensorPanel")->SetVisible(false);
	}
	else if (controlName == L"matchHeadBtn")
	{
		m_PaintManager.FindControl(L"matchPanel")->SetVisible(true);
	}
	else if (controlName == L"matchHideBtn")
	{
		m_PaintManager.FindControl(L"matchPanel")->SetVisible(false);
	}
	else if (controlName == L"firstKeyBtn" || controlName == L"secondKeyBtn" || controlName == L"thirdKeyBtn" || \
		controlName == L"fouthKeyBtn" || controlName == L"fifthKeyBtn" || controlName == L"sixthKeyBtn")
	{
		m_clickedKeyBtn = msg.pSender;
		PopupKeyMenu(msg.pSender);
	}
}

void CMainWindow::CloseRightSettingPannels()
{
	m_PaintManager.FindControl(L"dpiPanel")->SetVisible(false);
	m_PaintManager.FindControl(L"lightPanel")->SetVisible(false);
	m_PaintManager.FindControl(L"huiBaoPanel")->SetVisible(false);
	m_PaintManager.FindControl(L"lodPanel")->SetVisible(false);
	m_PaintManager.FindControl(L"qudouPanel")->SetVisible(false);
	m_PaintManager.FindControl(L"systemSettingPanel")->SetVisible(false);
	m_PaintManager.FindControl(L"sensorPanel")->SetVisible(false);
	m_PaintManager.FindControl(L"matchPanel")->SetVisible(false);
}

void CMainWindow::InitControls()
{
	// 睡眠时间下拉框添加选项
	CComboUI* timeCombo = (CComboUI*)m_PaintManager.FindControl(L"timeCombo");
	for (int i = 1; i <= 8; i++)
	{
		CListLabelElementUI* pControl = new CListLabelElementUI();		
		pControl->SetText(std::to_wstring(i).c_str());
		timeCombo->Add(pControl);
	}

	// 灯效设置下拉框添加选项
	CComboUI* lightCombo = (CComboUI*)m_PaintManager.FindControl(L"lightCombo");
	static std::wstring lightOptionNames[] = { L"关闭", L"呼吸", L"单色常量"};
	for (int i = 0; i < ARRAYSIZE(lightOptionNames); i++)
	{
		CListLabelElementUI* pControl = new CListLabelElementUI();
		pControl->SetText(lightOptionNames[i].c_str());
		lightCombo->Add(pControl);
	}

	// 回报率设置下拉框添加选项
	CComboUI* huibaoCombo = (CComboUI*)m_PaintManager.FindControl(L"huiBaoCombo");
	static std::wstring huibaoOptionNames[] = { L"125Hz", L"250Hz", L"500Hz", L"1000Hz", L"2000Hz", L"4000Hz", L"8000Hz" };
	for (int i = 0; i < ARRAYSIZE(huibaoOptionNames); i++)
	{
		CListLabelElementUI* pControl = new CListLabelElementUI();
		pControl->SetText(huibaoOptionNames[i].c_str());
		huibaoCombo->Add(pControl);
	}

	// LOD设置下拉框添加选项
	CComboUI* lodCombo = (CComboUI*)m_PaintManager.FindControl(L"lodCombo");
	static std::wstring lodOptionNames[] = { L"1mm", L"2mm"};
	for (int i = 0; i < ARRAYSIZE(lodOptionNames); i++)
	{
		CListLabelElementUI* pControl = new CListLabelElementUI();
		pControl->SetText(lodOptionNames[i].c_str());
		lodCombo->Add(pControl);
	}
}

void CMainWindow::PopupKeyMenu(CControlUI* fromControl)
{
	if (m_keyMenu == NULL)
	{
		return;
	}

	RECT pos = fromControl->GetPos();
	POINT pt = { pos.right, pos.top };
	::ClientToScreen(GetHWND(), &pt);
	TrackPopupMenu(GetSubMenu(m_keyMenu, 0), TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, GetHWND(), NULL);
}