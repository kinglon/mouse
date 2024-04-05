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
#include "SettingManager.h"
#include "ResourceUtil.h"

// 设置分类
#define SETTING_CATEGORY_ALL		0		//所有
#define SETTING_CATEGORY_KEY		1		//按键
#define SETTING_CATEGORY_POWER		2		//电源
#define SETTING_CATEGORY_DPI		3		//DPI
#define SETTING_CATEGORY_LIGHT		4		//灯效
#define SETTING_CATEGORY_HUIBAO		5		//回报率
#define SETTING_CATEGORY_LOD		6		//LOD
#define SETTING_CATEGORY_QUDOU		7		//按键去抖
#define SETTING_CATEGORY_SENSOR		8		//Sensor


#define WM_MOUSE_DATA_ARRIVE	WM_USER+100

#define CURRENT_DPI_TEXT_COLOR	0xffff0000


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
	CMouseCommManager::GetInstance()->Init(this);
}

void CMainWindow::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
}

CDuiString CMainWindow::GetSkinFolder()
{
	return CResourceUtil::GetSkinFolder().c_str();
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

	if (uMsg == WM_COMMAND && HIWORD(wParam) == 0)
	{
		OnMenuCommand(LOWORD(wParam));
		return 0L;
	}

	if (uMsg == WM_MOUSE_DATA_ARRIVE)
	{
		int dataLength = int(wParam);
		unsigned char* data = (unsigned char*)lParam;
		OnMouseDataArrive(data, dataLength);
		delete[] data;
		return 0L;
	}
		
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void CMainWindow::RecvDataCallback(unsigned char* data, int dataLength)
{
	if (dataLength <= 0)
	{
		return;
	}

	LPARAM lparam = (LPARAM)new unsigned char[dataLength];
	PostMessage(WM_MOUSE_DATA_ARRIVE, (WPARAM)dataLength, lparam);
}

void CMainWindow::OnItemSelectEvent(TNotifyUI& msg)
{
	std::wstring controlName = msg.pSender->GetName();
	if (controlName == L"timeCombo")
	{
		int sleepTime = _wtoi(msg.pSender->GetText().GetData());
		SetSleepTime(sleepTime);
	}
}

void CMainWindow::OnWindowInit(TNotifyUI& msg)
{
	UpdateControls(SETTING_CATEGORY_ALL);
}

void CMainWindow::OnMouseDataArrive(const unsigned char* data, int dataLength)
{
	CProtocalPackage package;
	if (!CProtocalUtil::ParsePackage(data, dataLength, package))
	{
		return;
	}

	if (package.m_reportId != 0x5a)
	{
		return;
	}

	if (package.m_commandId == 0xd1) // 设置鼠标参数响应
	{
		if (m_waitingWindow)
		{
			m_waitingWindow->SetSuccess(true);
			m_waitingWindow->Close();
		}
	}
}

void CMainWindow::OnMenuCommand(int commandId)
{
	std::wstring keyName = CKeyMapping::GetKeyNameByCommandId(commandId);
	if (keyName.empty())
	{
		return;
	}

	if (m_clickedKeyBtn)
	{
		m_clickedKeyBtn->SetText(keyName.c_str());
	}
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
		UpdateKeyPanel();
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
		UpdatePowerPannel();
	}
	else if (controlName == L"powerHideBtn")
	{
		m_PaintManager.FindControl(L"powerPanel")->SetVisible(false);
	}
	else if (controlName == L"dpiHeadBtn")
	{
		m_PaintManager.FindControl(L"dpiPanel")->SetVisible(true);
		UpdateDpiPanel();
	}
	else if (controlName == L"dpiHideBtn")
	{
		m_PaintManager.FindControl(L"dpiPanel")->SetVisible(false);
	}
	else if (controlName == L"lightHeadBtn")
	{
		m_PaintManager.FindControl(L"lightPanel")->SetVisible(true);
		UpdateLightPanel();
	}
	else if (controlName == L"lightHideBtn")
	{
		m_PaintManager.FindControl(L"lightPanel")->SetVisible(false);
	}
	else if (controlName == L"huiBaoHeadBtn")
	{
		m_PaintManager.FindControl(L"huiBaoPanel")->SetVisible(true);
		UpdateHuibaoPanel();
	}
	else if (controlName == L"huiBaoHideBtn")
	{
		m_PaintManager.FindControl(L"huiBaoPanel")->SetVisible(false);
	}
	else if (controlName == L"lodHeadBtn")
	{
		m_PaintManager.FindControl(L"lodPanel")->SetVisible(true);
		UpdateLodPanel();
	}
	else if (controlName == L"lodHideBtn")
	{
		m_PaintManager.FindControl(L"lodPanel")->SetVisible(false);
	}
	else if (controlName == L"qudouHeadBtn")
	{
		m_PaintManager.FindControl(L"qudouPanel")->SetVisible(true);
		UpdateQudouPanel();
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
		UpdateSensorPanel();
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
	else if (controlName == L"lineRejustOption" || controlName == L"motionSyncOption" || controlName == L"rippleControlOption")
	{
		SetOption((COptionUI*)msg.pSender, ((COptionUI*)msg.pSender)->IsSelected());
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

void CMainWindow::UpdateKeyPanel()
{
	if (!m_PaintManager.FindControl(L"keyPanel")->IsVisible())
	{
		return;
	}

	CMouseConfig& mouseConfig = CSettingManager::GetInstance()->m_mouseConfig;
	
	std::wstring keyName = CKeyMapping::GetKeyNameByKeyIndex(mouseConfig.m_firstKey);
	m_PaintManager.FindControl(L"firstKeyBtn")->SetText(keyName.c_str());

	keyName = CKeyMapping::GetKeyNameByKeyIndex(mouseConfig.m_secondKey);
	m_PaintManager.FindControl(L"secondKeyBtn")->SetText(keyName.c_str());

	keyName = CKeyMapping::GetKeyNameByKeyIndex(mouseConfig.m_thirdtKey);
	m_PaintManager.FindControl(L"thirdKeyBtn")->SetText(keyName.c_str());

	keyName = CKeyMapping::GetKeyNameByKeyIndex(mouseConfig.m_fourthKey);
	m_PaintManager.FindControl(L"fouthKeyBtn")->SetText(keyName.c_str());

	keyName = CKeyMapping::GetKeyNameByKeyIndex(mouseConfig.m_fifthKey);
	m_PaintManager.FindControl(L"fifthKeyBtn")->SetText(keyName.c_str());

	keyName = CKeyMapping::GetKeyNameByKeyIndex(mouseConfig.m_sixthKey);
	m_PaintManager.FindControl(L"sixthKeyBtn")->SetText(keyName.c_str());
}

void CMainWindow::UpdatePowerPannel()
{
	if (!m_PaintManager.FindControl(L"powerPanel")->IsVisible())
	{
		return;
	}

	CMouseConfig& mouseConfig = CSettingManager::GetInstance()->m_mouseConfig;

	int selIndex = max(mouseConfig.m_sleepTime - 1, 0);
	((CComboUI*)m_PaintManager.FindControl(L"timeCombo"))->SelectItem(selIndex);
}

void CMainWindow::UpdateDpiPanel()
{
	if (!m_PaintManager.FindControl(L"dpiPanel")->IsVisible())
	{
		return;
	}

	CMouseConfig& mouseConfig = CSettingManager::GetInstance()->m_mouseConfig;

	for (int i = 0; i < ARRAYSIZE(mouseConfig.m_dpiSetting); i++)
	{
		std::wstring controlName = std::wstring(L"dpiValueForgroundBtn") + std::to_wstring(i + 1);
		CControlUI* dpiValueForgroundBtn = m_PaintManager.FindControl(controlName.c_str());
		controlName = std::wstring(L"dpiValueBtn") + std::to_wstring(i + 1);
		CControlUI* dpiValueBtn = m_PaintManager.FindControl(controlName.c_str());
		controlName = std::wstring(L"dpiValueLabel") + std::to_wstring(i + 1);
		CLabelUI* dpiValueLabel = (CLabelUI*)m_PaintManager.FindControl(controlName.c_str());
		controlName = std::wstring(L"dpiColorBtn") + std::to_wstring(i + 1);
		CControlUI* dpiColorBtn = m_PaintManager.FindControl(controlName.c_str());

		int height = (int)((mouseConfig.m_dpiSetting[i].m_dpiLevel / 26000.0f)*dpiValueBtn->GetFixedHeight());
		dpiValueForgroundBtn->SetFixedHeight(height);
		dpiValueLabel->SetText(std::to_wstring(mouseConfig.m_dpiSetting[i].m_dpiLevel).c_str());
		dpiColorBtn->SetBkColor(mouseConfig.m_dpiSetting[i].m_dpiColor);
		if (mouseConfig.m_currentDpi == i)
		{
			dpiValueLabel->SetTextColor(CURRENT_DPI_TEXT_COLOR);
		}
		else
		{
			dpiValueLabel->SetTextColor(0xfff0f0f0);
		}
	}
}

void CMainWindow::UpdateLightPanel()
{
	if (!m_PaintManager.FindControl(L"lightPanel")->IsVisible())
	{
		return;
	}

	CMouseConfig& mouseConfig = CSettingManager::GetInstance()->m_mouseConfig;

	int selIndex = max(mouseConfig.m_lightIndex, 0);
	((CComboUI*)m_PaintManager.FindControl(L"lightCombo"))->SelectItem(selIndex);
}

void CMainWindow::UpdateHuibaoPanel()
{
	if (!m_PaintManager.FindControl(L"huiBaoPanel")->IsVisible())
	{
		return;
	}

	CMouseConfig& mouseConfig = CSettingManager::GetInstance()->m_mouseConfig;

	int selIndex = max(mouseConfig.m_huibaorate, 0);
	((CComboUI*)m_PaintManager.FindControl(L"huiBaoCombo"))->SelectItem(selIndex);
}

void CMainWindow::UpdateLodPanel()
{
	if (!m_PaintManager.FindControl(L"lodPanel")->IsVisible())
	{
		return;
	}

	CMouseConfig& mouseConfig = CSettingManager::GetInstance()->m_mouseConfig;

	int selIndex = max(mouseConfig.m_lodIndex - 1, 0);
	((CComboUI*)m_PaintManager.FindControl(L"lodCombo"))->SelectItem(selIndex);
}

void CMainWindow::UpdateQudouPanel()
{
	if (!m_PaintManager.FindControl(L"qudouPanel")->IsVisible())
	{
		return;
	}

	CMouseConfig& mouseConfig = CSettingManager::GetInstance()->m_mouseConfig;

	int qudouTime = max(mouseConfig.m_qudouTime, 1);
	qudouTime = min(qudouTime, 30);
	((CEditUI*)m_PaintManager.FindControl(L"qudouTimeEdit"))->SetText(std::to_wstring(qudouTime).c_str());
}

void CMainWindow::UpdateSensorPanel()
{
	if (!m_PaintManager.FindControl(L"sensorPanel")->IsVisible())
	{
		return;
	}

	CMouseConfig& mouseConfig = CSettingManager::GetInstance()->m_mouseConfig;

	SetOption((COptionUI*)m_PaintManager.FindControl(L"lineRejustOption"), mouseConfig.m_lineRejust);
	SetOption((COptionUI*)m_PaintManager.FindControl(L"motionSyncOption"), mouseConfig.m_motionSync);
	SetOption((COptionUI*)m_PaintManager.FindControl(L"rippleControlOption"), mouseConfig.m_rippleControl);
}

void CMainWindow::SetSleepTime(int sleepTime)
{
	CProtocalTLV tlv;
	tlv.m_type = 0x46;
	tlv.m_length = 0x03;
	tlv.m_value[0] = (unsigned char)sleepTime;

	CProtocalPackage package;
	package.m_commandId = 0xd1;
	package.m_tlvs.push_back(tlv);
	
	SendSetting(SETTING_CATEGORY_POWER, package);
}

void CMainWindow::SaveSetting(int settingCategory)
{
	CMouseConfig& mouseConfig = CSettingManager::GetInstance()->m_mouseConfig;

	// 保存按键设置
	if (settingCategory == SETTING_CATEGORY_KEY)
	{
		std::wstring text = m_PaintManager.FindControl(L"firstKeyBtn")->GetText();
		int index = CKeyMapping::GetKeyIndexByName(text);
		if (index != -1)
		{
			mouseConfig.m_firstKey = index;
		}

		text = m_PaintManager.FindControl(L"secondKeyBtn")->GetText();
		index = CKeyMapping::GetKeyIndexByName(text);
		if (index != -1)
		{
			mouseConfig.m_secondKey = index;
		}

		text = m_PaintManager.FindControl(L"thirdKeyBtn")->GetText();
		index = CKeyMapping::GetKeyIndexByName(text);
		if (index != -1)
		{
			mouseConfig.m_thirdtKey = index;
		}

		text = m_PaintManager.FindControl(L"fouthKeyBtn")->GetText();
		index = CKeyMapping::GetKeyIndexByName(text);
		if (index != -1)
		{
			mouseConfig.m_fourthKey = index;
		}

		text = m_PaintManager.FindControl(L"fifthKeyBtn")->GetText();
		index = CKeyMapping::GetKeyIndexByName(text);
		if (index != -1)
		{
			mouseConfig.m_fifthKey = index;
		}

		text = m_PaintManager.FindControl(L"sixthKeyBtn")->GetText();
		index = CKeyMapping::GetKeyIndexByName(text);
		if (index != -1)
		{
			mouseConfig.m_sixthKey = index;
		}
	}

	// 电源管理
	if (settingCategory == SETTING_CATEGORY_POWER)
	{
		std::wstring text = m_PaintManager.FindControl(L"timeCombo")->GetText();
		mouseConfig.m_sleepTime = _wtoi(text.c_str());
	}

	// DPI
	if (settingCategory == SETTING_CATEGORY_DPI)
	{
		for (int i = 0; i < ARRAYSIZE(mouseConfig.m_dpiSetting); i++)
		{			
			std::wstring controlName = std::wstring(L"dpiValueLabel") + std::to_wstring(i + 1);
			CLabelUI* dpiValueLabel = (CLabelUI*)m_PaintManager.FindControl(controlName.c_str());
			std::wstring text = dpiValueLabel->GetText().GetData();
			mouseConfig.m_dpiSetting[i].m_dpiLevel = _wtoi(text.c_str());

			controlName = std::wstring(L"dpiColorBtn") + std::to_wstring(i + 1);
			CControlUI* dpiColorBtn = m_PaintManager.FindControl(controlName.c_str());
			mouseConfig.m_dpiSetting[i].m_dpiColor = dpiColorBtn->GetBkColor();
			if (mouseConfig.m_dpiSetting[i].m_dpiColor == CURRENT_DPI_TEXT_COLOR)
			{
				mouseConfig.m_currentDpi = i;
			}
		}
	}

	// 灯效
	if (settingCategory == SETTING_CATEGORY_LIGHT)
	{
		CComboUI* lightCombo = (CComboUI*)m_PaintManager.FindControl(L"lightCombo");
		int selIndex = lightCombo->GetCurSel();
		mouseConfig.m_lightIndex = selIndex;
	}

	// 回报率
	if (settingCategory == SETTING_CATEGORY_HUIBAO)
	{
		CComboUI* combo = (CComboUI*)m_PaintManager.FindControl(L"huiBaoCombo");
		int selIndex = combo->GetCurSel();
		mouseConfig.m_huibaorate = selIndex;
	}

	// LOD
	if (settingCategory == SETTING_CATEGORY_LOD)
	{
		CComboUI* combo = (CComboUI*)m_PaintManager.FindControl(L"lodCombo");
		int selIndex = combo->GetCurSel();
		mouseConfig.m_lodIndex = selIndex;
	}

	// 按键去抖
	if (settingCategory == SETTING_CATEGORY_QUDOU)
	{
		CEditUI* edit = (CEditUI*)m_PaintManager.FindControl(L"qudouTimeEdit");
		std::wstring text = edit->GetText().GetData();
		mouseConfig.m_qudouTime = _wtoi(text.c_str());
	}

	// Sensor高阶设定
	if (settingCategory == SETTING_CATEGORY_SENSOR)
	{
		COptionUI* option = (COptionUI*)m_PaintManager.FindControl(L"lineRejustOption");		
		mouseConfig.m_lineRejust = option->IsSelected();

		option = (COptionUI*)m_PaintManager.FindControl(L"motionSyncOption");
		mouseConfig.m_motionSync = option->IsSelected();

		option = (COptionUI*)m_PaintManager.FindControl(L"rippleControlOption");
		mouseConfig.m_rippleControl = option->IsSelected();
	}

	CSettingManager::GetInstance()->SaveMouseConfig();
}

void CMainWindow::UpdateControls(int settingCategory)
{
	// 按键
	if (settingCategory == SETTING_CATEGORY_ALL || settingCategory == SETTING_CATEGORY_KEY)
	{
		UpdateKeyPanel();
	}

	// 电源
	if (settingCategory == SETTING_CATEGORY_ALL || settingCategory == SETTING_CATEGORY_POWER)
	{
		UpdatePowerPannel();
	}	

	// DPI设置
	if (settingCategory == SETTING_CATEGORY_ALL || settingCategory == SETTING_CATEGORY_DPI)
	{
		UpdateDpiPanel();
	}

	// 灯光效果
	if (settingCategory == SETTING_CATEGORY_ALL || settingCategory == SETTING_CATEGORY_LIGHT)
	{
		UpdateLightPanel();
	}

	// 回报率设置
	if (settingCategory == SETTING_CATEGORY_ALL || settingCategory == SETTING_CATEGORY_HUIBAO)
	{
		UpdateHuibaoPanel();
	}

	// LOD高度
	if (settingCategory == SETTING_CATEGORY_ALL || settingCategory == SETTING_CATEGORY_LOD)
	{
		UpdateLodPanel();
	}

	// 按键去抖时间
	if (settingCategory == SETTING_CATEGORY_ALL || settingCategory == SETTING_CATEGORY_QUDOU)
	{
		UpdateQudouPanel();
	}

	// Sensor高阶设定
	if (settingCategory == SETTING_CATEGORY_ALL || settingCategory == SETTING_CATEGORY_SENSOR)
	{
		UpdateSensorPanel();
	}
}

void CMainWindow::SetOption(COptionUI* control, bool check)
{
	control->Selected(check);
	control->SetBkImage(check ? L"option_check.png" : L"option_uncheck.png");
}

void CMainWindow::SendSetting(int settingCategory, const CProtocalPackage& package)
{
	if (!CMouseCommManager::GetInstance()->IsMouseConnected())
	{
		MessageBox(GetHWND(), L"鼠标未连接", L"提示", MB_OK);
		UpdateControls(settingCategory);
		return;
	}

	if (!CProtocalUtil::SendPackage(package))
	{
		MessageBox(GetHWND(), L"发送设置给鼠标失败", L"提示", MB_OK);
		UpdateControls(settingCategory);
		return;
	}

	// 显示等待窗口，要么等待超时结束，要么外部设置结果关闭
	m_waitingWindow = new CWaitingWindow();
	m_waitingWindow->SetWaitSec(3);
	m_waitingWindow->Create(GetHWND(), NULL, WS_VISIBLE | WS_OVERLAPPEDWINDOW, 0);
	m_waitingWindow->CenterWindow();
	m_waitingWindow->ShowModal();
	bool success = m_waitingWindow->IsSuccess();
	delete m_waitingWindow;
	m_waitingWindow = nullptr;

	// 设置成功就更新配置，设置失败恢复界面
	if (!success)
	{
		MessageBox(GetHWND(), L"设置失败", L"提示", MB_OK);
		UpdateControls(settingCategory);
	}
	else
	{
		SaveSetting(settingCategory);
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