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
#include "MyMessageBox.h"
#include "DpiLockWindow.h"

// ���÷���
#define SETTING_CATEGORY_ALL		0		//����
#define SETTING_CATEGORY_KEY		1		//����
#define SETTING_CATEGORY_POWER		2		//��Դ
#define SETTING_CATEGORY_DPI		3		//DPI
#define SETTING_CATEGORY_LIGHT		4		//��Ч
#define SETTING_CATEGORY_HUIBAO		5		//�ر���
#define SETTING_CATEGORY_LOD		6		//LOD
#define SETTING_CATEGORY_QUDOU		7		//����ȥ��
#define SETTING_CATEGORY_SENSOR		8		//Sensor


#define WM_MOUSE_DATA_ARRIVE	WM_USER+100

// selectitem�¼��������ڲ���ShowModal()�������첽����һ������Ϣ
#define WM_SELECT_ITEM			WM_USER+101

#define CURRENT_DPI_TEXT_COLOR	0xffff0000

#define TIMERID_RUNTASKPOOL		100

// ����ID
#define TASKID_GET_MOUSE_SETTING		1
#define TASKID_GET_BATTERY				2


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
		// ��ֹ˫���Ŵ󴰿�
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

	if (uMsg == WM_SELECT_ITEM)
	{
		CControlUI* control = (CControlUI*)lParam;
		if (control->GetName() == L"timeCombo")
		{
			SetSleepTimeToMouse();
		}
		else if (control->GetName() == L"lightCombo")
		{
			SetLightSettingToMouse();
		}
		else if (control->GetName() == L"huiBaoCombo")
		{
			SetHuibaoSettingToMouse();
		}
		else if (control->GetName() == L"lodCombo")
		{
			SetLodSettingToMouse();
		}

		return 0L;
	}

	if (uMsg == WM_TIMER)
	{
		int timerId = (int)wParam;
		if (timerId == TIMERID_RUNTASKPOOL)
		{
			RunTaskPool();
			return 0L;
		}
	}
		
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void CMainWindow::RecvDataCallback(unsigned char* data, int dataLength)
{
	if (dataLength <= 0)
	{
		return;
	}

	unsigned char* lparam = new unsigned char[dataLength];
	memcpy(lparam, data, dataLength);
	PostMessage(WM_MOUSE_DATA_ARRIVE, (WPARAM)dataLength, (LPARAM)lparam);
}

void CMainWindow::OnItemSelectEvent(TNotifyUI& msg)
{
	if (!m_manualTrigger)
	{
		return;
	}

	PostMessage(WM_SELECT_ITEM, 0, (LPARAM)msg.pSender);
}

void CMainWindow::OnSelectChangeEvent(TNotifyUI& msg)
{
	if (!m_manualTrigger)
	{
		return;
	}

	std::wstring controlName = msg.pSender->GetName();
	if (controlName == L"lineRejustOption" || controlName == L"motionSyncOption" || controlName == L"rippleControlOption")
	{
		SetSensorSettingToMouse();
	}
}

void CMainWindow::OnWindowInit(TNotifyUI& msg)
{
	UpdateControls(SETTING_CATEGORY_ALL);

	m_taskPool.push_back(TASKID_GET_BATTERY);
	m_taskPool.push_back(TASKID_GET_MOUSE_SETTING);
	RunTaskPool();
	SetTimer(GetHWND(), TIMERID_RUNTASKPOOL, 3000, NULL);
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

	// �����������ɹ������ò����ɹ�
	if (package.m_commandId == 0xd1 || package.m_commandId == 0xd4)
	{
		if (m_waitingWindow)
		{
			m_waitingWindow->SetSuccess(true);
			m_waitingWindow->Close();
		}
	}
	else if (package.m_commandId == 0xd3)
	{
		RecvBatteryInfo(package);
	}
	else if (package.m_commandId == 0xd0)
	{
		RecvMouseSetting(package);
	}
}

void CMainWindow::OnMenuCommand(int commandId)
{
	if (m_clickedKeyBtn == nullptr)
	{
		return;
	}

	// DPI��������������DPIֵ
	int dpiValue = 200;
	if (commandId == ID_KEY_DPILOCK)
	{
		CDpiLockWindow dpiLockWindow;
		dpiLockWindow.Create(GetHWND(), NULL, WS_VISIBLE | WS_POPUP, 0);
		dpiLockWindow.CenterWindow();
		if (dpiLockWindow.ShowModal() == 0) // ȡ��
		{
			return;
		}
		dpiValue = dpiLockWindow.GetDpiValue();
	}

	// ��ȡKeyNum
	std::wstring keyName = CKeyMapping::GetKeyNameByCommandId(commandId);
	if (keyName.empty())
	{
		return;
	}
	static std::wstring keyBtnNames[] = { L"firstKeyBtn", L"secondKeyBtn", L"thirdKeyBtn", L"fouthKeyBtn", L"fifthKeyBtn", L"sixthKeyBtn" };
	int keyNum = -1;
	for (int i = 0; i < ARRAYSIZE(keyBtnNames); i++)
	{
		if (m_clickedKeyBtn->GetName() == keyBtnNames[i].c_str())
		{
			keyNum = i + 1;
		}
	}
	if (keyNum < 1)
	{
		return;
	}

	// ���İ�������	
	m_clickedKeyBtn->SetText(keyName.c_str());

	// ���Ͱ������ø����
	if (commandId == ID_KEY_DPILOCK)
	{
		SetDpiLockToMouse(keyNum, dpiValue);
	}
	else{
		SetKeyToMouse(keyNum, CKeyMapping::GetKeyIndexByName(keyName));
	}
}

void CMainWindow::OnClickEvent(TNotifyUI& msg)
{
	auto& controlName = msg.pSender->GetName();

	// �Ҳ���������Ƿ�����壬ֻ��ʾ����һ����������ʾǰ�ȹر�����
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
	else if (controlName.Find(L"dpiColorBtn") == 0)
	{
		ClickDpiColorBtn((CButtonUI*)msg.pSender);
	}
	else if (controlName.Find(L"dpiValueBtn") == 0 || controlName.Find(L"dpiValueForgroundBtn") == 0)
	{
		ClickDpiValueBtn((CButtonUI*)msg.pSender);
	}
	else if (controlName.Find(L"dpiValueLabel") == 0)
	{
		SwitchCurrentDpi((CButtonUI*)msg.pSender);
	}
	else if (controlName == L"qudouBtn")
	{
		OnQudouBtnClicked();
	}
	else if (controlName == L"matchBtn")
	{
		OnMatchBtnClicked();
	}
	else if (controlName == L"resetBtn")
	{
		OnResetBtnClicked();
	}
	else if (controlName == L"systemSettingBtn")
	{
		OnSystemMouseBtnClicked();
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
	// ˯��ʱ�����������ѡ��
	CComboUI* timeCombo = (CComboUI*)m_PaintManager.FindControl(L"timeCombo");
	for (int i = 1; i <= 8; i++)
	{
		CListLabelElementUI* pControl = new CListLabelElementUI();		
		pControl->SetText(std::to_wstring(i).c_str());
		timeCombo->Add(pControl);
	}

	// ��Ч�������������ѡ��
	CComboUI* lightCombo = (CComboUI*)m_PaintManager.FindControl(L"lightCombo");
	static std::wstring lightOptionNames[] = { L"�ر�", L"����", L"��ɫ����"};
	for (int i = 0; i < ARRAYSIZE(lightOptionNames); i++)
	{
		CListLabelElementUI* pControl = new CListLabelElementUI();
		pControl->SetText(lightOptionNames[i].c_str());
		lightCombo->Add(pControl);
	}

	// �ر����������������ѡ��
	CComboUI* huibaoCombo = (CComboUI*)m_PaintManager.FindControl(L"huiBaoCombo");
	static std::wstring huibaoOptionNames[] = { L"125Hz", L"250Hz", L"500Hz", L"1000Hz", L"2000Hz", L"4000Hz", L"8000Hz" };
	for (int i = 0; i < ARRAYSIZE(huibaoOptionNames); i++)
	{
		CListLabelElementUI* pControl = new CListLabelElementUI();
		pControl->SetText(huibaoOptionNames[i].c_str());
		huibaoCombo->Add(pControl);
	}

	// LOD�������������ѡ��
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

	m_manualTrigger = false;
	int selIndex = max(mouseConfig.m_sleepTime - 1, 0);
	((CComboUI*)m_PaintManager.FindControl(L"timeCombo"))->SelectItem(selIndex);
	m_manualTrigger = true;
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
		CButtonUI* dpiValueLabel = (CButtonUI*)m_PaintManager.FindControl(controlName.c_str());
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

	m_manualTrigger = false;
	int selIndex = max(mouseConfig.m_lightIndex, 0);
	((CComboUI*)m_PaintManager.FindControl(L"lightCombo"))->SelectItem(selIndex);
	m_manualTrigger = true;
}

void CMainWindow::UpdateHuibaoPanel()
{
	if (!m_PaintManager.FindControl(L"huiBaoPanel")->IsVisible())
	{
		return;
	}

	CMouseConfig& mouseConfig = CSettingManager::GetInstance()->m_mouseConfig;

	m_manualTrigger = false;
	int selIndex = max(mouseConfig.m_huibaorate, 0);
	((CComboUI*)m_PaintManager.FindControl(L"huiBaoCombo"))->SelectItem(selIndex);
	m_manualTrigger = true;
}

void CMainWindow::UpdateLodPanel()
{
	if (!m_PaintManager.FindControl(L"lodPanel")->IsVisible())
	{
		return;
	}

	CMouseConfig& mouseConfig = CSettingManager::GetInstance()->m_mouseConfig;

	m_manualTrigger = false;
	int selIndex = max(mouseConfig.m_lodIndex - 1, 0);
	((CComboUI*)m_PaintManager.FindControl(L"lodCombo"))->SelectItem(selIndex);
	m_manualTrigger = true;
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

int CMainWindow::GetDpiIndexByControlName(std::wstring controlName)
{
	if (controlName.empty())
	{
		return -1;
	}

	int firstNumberIndex = -1;
	for (unsigned int i = controlName.size() - 1; i >= 0; i--)
	{
		if (controlName[i] < L'0' || controlName[i] > L'9')
		{
			break;
		}
		firstNumberIndex = i;
	}
	if (firstNumberIndex == -1)
	{
		return -1;
	}

	std::wstring number = controlName.substr(firstNumberIndex);
	return _wtoi(number.c_str());
}

void CMainWindow::ClickDpiColorBtn(CButtonUI* button)
{
	// ѡ����ɫ
	static COLORREF acrCustClr[16];
	CHOOSECOLOR cc;
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = GetHWND();
	cc.lpCustColors = (LPDWORD)acrCustClr;
	cc.lpfnHook = (LPCCHOOKPROC)&CMainWindow::ColorDialogProc;
	cc.Flags = CC_FULLOPEN | CC_ENABLEHOOK;
	if (!ChooseColor(&cc))
	{
		return;
	}
	DWORD color = 0XFF000000 | (GetRValue(cc.rgbResult)<<16) | (GetGValue(cc.rgbResult)<<8) | (GetBValue(cc.rgbResult));
	button->SetBkColor(color);

	// �������ø����
	SetDpiSettingToMouse();
}

void CMainWindow::ClickDpiValueBtn(CButtonUI* button)
{
	// ��ȡDPIֵ
	int dpiValue = 200;
	CDpiLockWindow dpiLockWindow;
	dpiLockWindow.Create(GetHWND(), NULL, WS_VISIBLE | WS_POPUP, 0);
	dpiLockWindow.CenterWindow();
	if (dpiLockWindow.ShowModal() == 0) // ȡ��
	{
		return;
	}
	dpiValue = dpiLockWindow.GetDpiValue();

	// ��ȡDPI����
	int dpiIndex = GetDpiIndexByControlName(button->GetName().GetData());
	if (dpiIndex < 1 || dpiIndex > 6)
	{
		return;
	}

	// ���½���
	CControlUI* dpiValueLabel = m_PaintManager.FindControl((std::wstring(L"dpiValueLabel") + std::to_wstring(dpiIndex)).c_str());
	dpiValueLabel->SetText(std::to_wstring(dpiValue).c_str());

	CControlUI* dpiValueBtn = m_PaintManager.FindControl((std::wstring(L"dpiValueBtn") + std::to_wstring(dpiIndex)).c_str());
	CControlUI* dpiValueForgroundBtn = m_PaintManager.FindControl((std::wstring(L"dpiValueForgroundBtn") + std::to_wstring(dpiIndex)).c_str());
	int height = (int)((dpiValue / 26000.0f)*dpiValueBtn->GetFixedHeight());
	dpiValueForgroundBtn->SetFixedHeight(height);

	// �������ø����
	SetDpiSettingToMouse();
}

void CMainWindow::SwitchCurrentDpi(CButtonUI* button)
{
	// ���ĵ�ǰDPI��������ɫ
	for (int i = 1; i <= 6; i++)
	{
		CButtonUI* btn = (CButtonUI*)m_PaintManager.FindControl((std::wstring(L"dpiValueLabel")+std::to_wstring(i)).c_str());
		if (btn == button)
		{
			btn->SetTextColor(CURRENT_DPI_TEXT_COLOR);
		}
		else
		{
			btn->SetTextColor(0xfff0f0f0);
		}
	}

	// �������ø����
	SetDpiSettingToMouse();
}

void CMainWindow::SetDpiLockToMouse(int keyNum, int dpiValue)
{
	CProtocalTLV tlv;
	tlv.m_type = 0x4a;
	tlv.m_length = 0x05;
	tlv.m_value[0] = (unsigned char)keyNum;
	tlv.m_value[1] = dpiValue & 0xff;
	tlv.m_value[2] = (dpiValue >> 8) & 0xff;

	CProtocalPackage package;
	package.m_commandId = 0xd1;
	package.m_tlvs.push_back(tlv);

	SendSetting(SETTING_CATEGORY_KEY, package);
}

void CMainWindow::SetSleepTimeToMouse()
{
	int sleepTime = _wtoi(m_PaintManager.FindControl(L"timeCombo")->GetText().GetData());

	CProtocalTLV tlv;
	tlv.m_type = 0x46;
	tlv.m_length = 0x03;
	tlv.m_value[0] = (unsigned char)sleepTime;

	CProtocalPackage package;
	package.m_commandId = 0xd1;
	package.m_tlvs.push_back(tlv);
	
	SendSetting(SETTING_CATEGORY_POWER, package);
}

void CMainWindow::SetLightSettingToMouse()
{
	int curSel = ((CComboUI*)m_PaintManager.FindControl(L"lightCombo"))->GetCurSel();

	CProtocalTLV tlv;
	tlv.m_type = 0x48;
	tlv.m_length = 0x05;
	tlv.m_value[0] = (unsigned char)curSel;

	CProtocalPackage package;
	package.m_commandId = 0xd1;
	package.m_tlvs.push_back(tlv);

	SendSetting(SETTING_CATEGORY_LIGHT, package);
}

void CMainWindow::SetLodSettingToMouse()
{
	int curSel = ((CComboUI*)m_PaintManager.FindControl(L"lodCombo"))->GetCurSel();

	CProtocalTLV tlv;
	tlv.m_type = 0x43;
	tlv.m_length = 0x03;
	tlv.m_value[0] = (unsigned char)(curSel+1);

	CProtocalPackage package;
	package.m_commandId = 0xd1;
	package.m_tlvs.push_back(tlv);

	SendSetting(SETTING_CATEGORY_LOD, package);
}

void CMainWindow::SetSensorSettingToMouse()
{
	bool lineRejust = ((COptionUI*)m_PaintManager.FindControl(L"lineRejustOption"))->IsSelected();
	bool motionSync = ((COptionUI*)m_PaintManager.FindControl(L"motionSyncOption"))->IsSelected();
	bool rippleControl = ((COptionUI*)m_PaintManager.FindControl(L"rippleControlOption"))->IsSelected();

	CProtocalTLV tlv;
	tlv.m_type = 0x47;
	tlv.m_length = 0x05;
	tlv.m_value[0] = lineRejust ? 0x01 : 0x00;
	tlv.m_value[1] = motionSync ? 0x01 : 0x00;
	tlv.m_value[2] = rippleControl ? 0x01 : 0x00;

	CProtocalPackage package;
	package.m_commandId = 0xd1;
	package.m_tlvs.push_back(tlv);

	SendSetting(SETTING_CATEGORY_SENSOR, package);
}

void CMainWindow::SetHuibaoSettingToMouse()
{
	int curSel = ((CComboUI*)m_PaintManager.FindControl(L"huiBaoCombo"))->GetCurSel();

	CProtocalTLV tlv;
	tlv.m_type = 0x45;
	tlv.m_length = 0x03;
	tlv.m_value[0] = (unsigned char)(curSel);

	CProtocalPackage package;
	package.m_commandId = 0xd1;
	package.m_tlvs.push_back(tlv);

	SendSetting(SETTING_CATEGORY_HUIBAO, package);
}

void CMainWindow::OnResetBtnClicked()
{
	if (!CMyMessageBox::Show(GetHWND(), L"��ǰ�����Ƿ���Ҫ�ָ���Ĭ�����ã�", true))
	{
		return;
	}

	if (!CMouseCommManager::GetInstance()->IsMouseConnected())
	{
		CMyMessageBox::Show(GetHWND(), L"���δ����");
		return;
	}	

	CProtocalPackage package;
	package.m_commandId = 0xd4;
	if (!CProtocalUtil::SendPackage(package))
	{
		CMyMessageBox::Show(GetHWND(), L"����ʧ��");
		return;
	}

	// ��ʾ�ȴ����ڣ�Ҫô�ȴ���ʱ������Ҫô�ⲿ���ý���ر�
	m_waitingWindow = new CWaitingWindow();
	m_waitingWindow->SetCaption(L"������...");
	m_waitingWindow->SetWaitSec(3);
	m_waitingWindow->Create(GetHWND(), NULL, WS_VISIBLE | WS_OVERLAPPEDWINDOW, 0);
	m_waitingWindow->CenterWindow();
	m_waitingWindow->ShowModal();
	bool success = m_waitingWindow->IsSuccess();
	delete m_waitingWindow;
	m_waitingWindow = nullptr;
	
	if (!success)
	{
		CMyMessageBox::Show(GetHWND(), L"����ʧ��");
	}
	else
	{
		CSettingManager::GetInstance()->m_mouseConfig = CMouseConfig();
		CSettingManager::GetInstance()->SaveMouseConfig();
		UpdateControls(SETTING_CATEGORY_ALL);
	}
}

void CMainWindow::OnQudouBtnClicked()
{
	std::wstring text = m_PaintManager.FindControl(L"qudouTimeEdit")->GetText().GetData();
	int value = _wtoi(text.c_str());
	if (!IsNumber(text) || value < 1 || value > 30)
	{
		CMyMessageBox::Show(GetHWND(), L"ֻ��������1-30");
		return;
	}

	CProtocalTLV tlv;
	tlv.m_type = 0x44;
	tlv.m_length = 0x03;
	tlv.m_value[0] = (unsigned char)value;

	CProtocalPackage package;
	package.m_commandId = 0xd1;
	package.m_tlvs.push_back(tlv);

	SendSetting(SETTING_CATEGORY_QUDOU, package);
}

void CMainWindow::OnSystemMouseBtnClicked()
{
	HWND hwnd = (HWND)ShellExecute(NULL, NULL, L"control", L"mouse", NULL, SW_SHOW);
	SetForegroundWindow(hwnd);
}

void CMainWindow::OnMatchBtnClicked()
{
	// todo by yejinlong����Թ���
}

bool CMainWindow::IsNumber(std::wstring text)
{
	if (text.empty())
	{
		return false;
	}

	for (auto& ch : text)
	{
		if (ch < L'0' || ch > '9')
		{
			return false;
		}
	}

	return true;
}

void CMainWindow::SetKeyToMouse(int keyNum, int keyIndex)
{
	const auto& keyStructs = CKeyMapping::GetKeyStructs();
	auto it = keyStructs.find(keyIndex);
	if (it == keyStructs.end())
	{
		LOG_ERROR(L"failed to find the key struct of %d", keyIndex);
		return;
	}


	// ����Value
	unsigned char tlvData[6];
	memset(tlvData, 0, sizeof(tlvData));
	tlvData[0] = (unsigned char)keyNum;  // key num
	tlvData[1] = it->second.m_reportId;  // report id
	tlvData[2] = it->second.m_keyCode1;  // key code1
	tlvData[3] = it->second.m_keyCode2;  // key code2
	tlvData[4] = it->second.m_disable;  // disable
	tlvData[5] = it->second.m_loopCount;  // loop count

	// ����TLV
	CProtocalTLV tlv;
	tlv.m_type = 0x40;
	tlv.m_length = 0x08;
	memcpy(tlv.m_value, tlvData, sizeof(tlvData));

	// ����Э���
	CProtocalPackage package;
	package.m_reportId = 0xa5;
	package.m_commandId = 0xd1;
	package.m_tlvs.push_back(tlv);

	// ����Э���
	SendSetting(SETTING_CATEGORY_KEY, package);
}

void CMainWindow::SaveSetting(int settingCategory)
{
	CMouseConfig& mouseConfig = CSettingManager::GetInstance()->m_mouseConfig;

	// ���水������
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

	// ��Դ����
	if (settingCategory == SETTING_CATEGORY_POWER)
	{
		std::wstring text = m_PaintManager.FindControl(L"timeCombo")->GetText();
		mouseConfig.m_sleepTime = _wtoi(text.c_str());
	}

	// DPI	
	if (settingCategory == SETTING_CATEGORY_DPI)
	{
		GetDpiSetting(mouseConfig.m_currentDpi, mouseConfig.m_dpiSetting);		
	}

	// ��Ч
	if (settingCategory == SETTING_CATEGORY_LIGHT)
	{
		CComboUI* lightCombo = (CComboUI*)m_PaintManager.FindControl(L"lightCombo");
		int selIndex = lightCombo->GetCurSel();
		mouseConfig.m_lightIndex = selIndex;
	}

	// �ر���
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

	// ����ȥ��
	if (settingCategory == SETTING_CATEGORY_QUDOU)
	{
		CEditUI* edit = (CEditUI*)m_PaintManager.FindControl(L"qudouTimeEdit");
		std::wstring text = edit->GetText().GetData();
		mouseConfig.m_qudouTime = _wtoi(text.c_str());
	}

	// Sensor�߽��趨
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

UINT_PTR CMainWindow::ColorDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_INITDIALOG)
	{
		// ������ʾ����
		RECT rc;
		GetWindowRect(hwnd, &rc);
		int width = rc.right - rc.left;
		int height = rc.bottom - rc.top;
		int x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		int y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
		SetWindowPos(hwnd, NULL, x, y, 0, 0, SWP_NOSIZE);
	}

	return 0;
}

void CMainWindow::RunTaskPool()
{
	// ���������ڲ����ܻ��������أ�����һ�ݲ����Ƚϱ���
	auto taskPool = m_taskPool;
	for (int taskid : taskPool)
	{
		if (taskid == TASKID_GET_BATTERY)
		{
			GetBatteryFromMouse();
		}		
		else if (taskid == TASKID_GET_MOUSE_SETTING)
		{
			GetMouseSettingFromMouse();
		}
	}
}

void CMainWindow::GetBatteryFromMouse()
{
	if (!CMouseCommManager::GetInstance()->IsMouseConnected())
	{		
		return;
	}

	// ��ȡʱ��δ��
	if (GetTickCount64() < m_nextGetBatteryTime)
	{
		return;
	}

	CProtocalPackage package;
	package.m_commandId = 0xd3;
	CProtocalUtil::SendPackage(package);
}

void CMainWindow::RecvBatteryInfo(const CProtocalPackage& package)
{
	// 3���Ӻ��ٸ���
	m_nextGetBatteryTime = GetTickCount64() + 180000;

	if (package.m_tlvs.size() == 0 || package.m_tlvs[0].m_type != 0x4b)
	{
		LOG_ERROR(L"recv an invalid battery tlv");
		return;
	}

	int battery = min(package.m_tlvs[0].m_value[0], 100);
	m_PaintManager.FindControl(L"batteryValue")->SetText((std::to_wstring(battery)+L"%").c_str());
	
	int height = (int)((100 - battery) / 100.0f * 50);
	m_PaintManager.FindControl(L"batteryArea")->SetFixedHeight(height);
}


void CMainWindow::GetMouseSettingFromMouse()
{
	if (!CMouseCommManager::GetInstance()->IsMouseConnected())
	{
		return;
	}

	CProtocalPackage package;
	package.m_commandId = 0xd0;
	CProtocalUtil::SendPackage(package);
}

void CMainWindow::RecvMouseSetting(const CProtocalPackage& package)
{
	// ������ɣ�ȥ����
	for (auto it = m_taskPool.begin(); it != m_taskPool.end(); it++)
	{
		if (*it == TASKID_GET_MOUSE_SETTING)
		{
			m_taskPool.erase(it);
			break;
		}
	}

	CMouseConfig& mouseConfig = CSettingManager::GetInstance()->m_mouseConfig;
	for (const auto& tlv : package.m_tlvs)
	{
		if (tlv.m_type == 0x40)  // ��������
		{
			int keyNum = (int)tlv.m_value[0];

			CKeyStruct keyStruct;
			keyStruct.m_reportId = tlv.m_value[1];
			keyStruct.m_keyCode1 = tlv.m_value[2];
			keyStruct.m_keyCode2 = tlv.m_value[3];
			keyStruct.m_disable = tlv.m_value[4];
			keyStruct.m_loopCount = tlv.m_value[5];

			int keyIndex = -1;
			if (keyStruct.m_disable)
			{
				keyIndex = KEY_INDEX_DISABLE;
			}
			else
			{
				const auto& keyStructs = CKeyMapping::GetKeyStructs();
				for (const auto& item : keyStructs)
				{
					const auto& value = item.second;
					if (value.m_reportId == keyStruct.m_reportId && value.m_keyCode1 == keyStruct.m_keyCode1 \
						&& value.m_keyCode2 == keyStruct.m_keyCode2 && value.m_loopCount == keyStruct.m_loopCount)
					{
						keyIndex = item.first;
						break;
					}
				}
			}

			if (keyIndex == -1)
			{
				LOG_ERROR(L"not found the key: 0x%02x 0x%02x 0x%02x", keyStruct.m_reportId, keyStruct.m_keyCode1, keyStruct.m_keyCode2);
				continue;
			}

			mouseConfig.SetKey(keyNum, keyIndex);
			UpdateKeyPanel();
		}
		else if (tlv.m_type == 0x41)  // ��ǰDPI�ȼ�
		{
			int dpiIndex = tlv.m_value[0];
			if (dpiIndex >= 1 && dpiIndex <= 6)
			{
				mouseConfig.m_currentDpi = dpiIndex - 1;
				UpdateDpiPanel();
			}
		}
		else if (tlv.m_type == 0x42)  // DPI�ȼ�����
		{
			int dpiIndex = tlv.m_value[0];
			int dpiValue = tlv.m_value[1] + (tlv.m_value[2] << 8);
			COLORREF dpiColor = 0xff000000 + RGB(tlv.m_value[3], tlv.m_value[4], tlv.m_value[5]);
			if (dpiIndex < 1 || dpiIndex > 6 || dpiValue < 100 || dpiValue > 26000)
			{
				LOG_ERROR(L"recv an invalid dpi param, index=%d, value=%d", dpiIndex, dpiValue);
				continue;
			}
			mouseConfig.m_dpiSetting[dpiIndex - 1].m_dpiLevel = dpiValue;
			mouseConfig.m_dpiSetting[dpiIndex - 1].m_dpiColor = dpiColor;
			UpdateDpiPanel();
		}
		else if (tlv.m_type == 0x43)  // LOD
		{
			int lodIndex = tlv.m_value[0];
			if (lodIndex < 1 || lodIndex > 2)
			{
				LOG_ERROR(L"recv an invalid lod param, index=%d", lodIndex);
				continue;
			}
			mouseConfig.m_lodIndex = lodIndex;
			UpdateLodPanel();
		}
		else if (tlv.m_type == 0x44)  // ����ȥ��
		{
			int qudouTime = tlv.m_value[0];
			if (qudouTime < 0 || qudouTime > 30)
			{
				LOG_ERROR(L"recv an invalid qudou param, time=%d", qudouTime);
				continue;
			}
			mouseConfig.m_qudouTime = qudouTime;
			UpdateQudouPanel();
		}
		else if (tlv.m_type == 0x45)  // �ر��ʲ���
		{
			int index = tlv.m_value[0];
			if (index <0 || index > 6)
			{
				LOG_ERROR(L"recv an invalid huibao param, index=%d", index);
				continue;
			}
			mouseConfig.m_huibaorate = index;
			UpdateHuibaoPanel();
		}
		else if (tlv.m_type == 0x46)  // ����ʱ��
		{
			int time = tlv.m_value[0];
			if (time < 1 || time > 8)
			{
				LOG_ERROR(L"recv an invalid sleep time param, time=%d", time);
				continue;
			}
			mouseConfig.m_sleepTime = time;
			UpdatePowerPannel();
		}
		else if (tlv.m_type == 0x47)  // Sensor�߽�����
		{
			mouseConfig.m_lineRejust = tlv.m_value[0] == 0x01 ? true : false;
			mouseConfig.m_motionSync = tlv.m_value[1] == 0x01 ? true : false;
			mouseConfig.m_rippleControl = tlv.m_value[2] == 0x01 ? true : false;
			UpdateSensorPanel();
		}
		else if (tlv.m_type == 0x48)  // ��Ч����
		{
			int ledMode = tlv.m_value[0];
			if (ledMode < 0 || ledMode > 2)
			{
				LOG_ERROR(L"recv an invalid light param, ledmode=%d", ledMode);
				continue;
			}
			mouseConfig.m_lightIndex = ledMode;
			UpdateLightPanel();
		}
		else if (tlv.m_type == 0x49) // �汾��
		{
			std::wstring version = std::to_wstring(tlv.m_value[1]) + L'.';
			version += std::to_wstring(tlv.m_value[2]) + L'.';
			version += std::to_wstring(tlv.m_value[3]) + L'.';
			version += std::to_wstring(tlv.m_value[4]);
			m_PaintManager.FindControl(L"driverVerson")->SetText(version.c_str());
		}
	}

	CSettingManager::GetInstance()->SaveMouseConfig();
}

void CMainWindow::UpdateControls(int settingCategory)
{
	// ����
	if (settingCategory == SETTING_CATEGORY_ALL || settingCategory == SETTING_CATEGORY_KEY)
	{
		UpdateKeyPanel();
	}

	// ��Դ
	if (settingCategory == SETTING_CATEGORY_ALL || settingCategory == SETTING_CATEGORY_POWER)
	{
		UpdatePowerPannel();
	}	

	// DPI����
	if (settingCategory == SETTING_CATEGORY_ALL || settingCategory == SETTING_CATEGORY_DPI)
	{
		UpdateDpiPanel();
	}

	// �ƹ�Ч��
	if (settingCategory == SETTING_CATEGORY_ALL || settingCategory == SETTING_CATEGORY_LIGHT)
	{
		UpdateLightPanel();
	}

	// �ر�������
	if (settingCategory == SETTING_CATEGORY_ALL || settingCategory == SETTING_CATEGORY_HUIBAO)
	{
		UpdateHuibaoPanel();
	}

	// LOD�߶�
	if (settingCategory == SETTING_CATEGORY_ALL || settingCategory == SETTING_CATEGORY_LOD)
	{
		UpdateLodPanel();
	}

	// ����ȥ��ʱ��
	if (settingCategory == SETTING_CATEGORY_ALL || settingCategory == SETTING_CATEGORY_QUDOU)
	{
		UpdateQudouPanel();
	}

	// Sensor�߽��趨
	if (settingCategory == SETTING_CATEGORY_ALL || settingCategory == SETTING_CATEGORY_SENSOR)
	{
		UpdateSensorPanel();
	}
}

void CMainWindow::SetOption(COptionUI* control, bool check)
{
	m_manualTrigger = false;
	control->Selected(check);
	m_manualTrigger = true;
	control->SetBkImage(check ? L"option_check.png" : L"option_uncheck.png");
}

void CMainWindow::SendSetting(int settingCategory, const CProtocalPackage& package)
{
	if (!CMouseCommManager::GetInstance()->IsMouseConnected())
	{
		CMyMessageBox::Show(GetHWND(), L"���δ����");
		UpdateControls(settingCategory);
		return;
	}

	if (!CProtocalUtil::SendPackage(package))
	{
		CMyMessageBox::Show(GetHWND(), L"�������ø����ʧ��");
		UpdateControls(settingCategory);
		return;
	}

	// ��ʾ�ȴ����ڣ�Ҫô�ȴ���ʱ������Ҫô�ⲿ���ý���ر�
	m_waitingWindow = new CWaitingWindow();
	m_waitingWindow->SetCaption(L"���������...");
	m_waitingWindow->SetWaitSec(3);
	m_waitingWindow->Create(GetHWND(), NULL, WS_VISIBLE | WS_OVERLAPPEDWINDOW, 0);
	m_waitingWindow->CenterWindow();
	m_waitingWindow->ShowModal();
	bool success = m_waitingWindow->IsSuccess();
	delete m_waitingWindow;
	m_waitingWindow = nullptr;

	// ���óɹ��͸������ã�����ʧ�ָܻ�����
	if (!success)
	{
		CMyMessageBox::Show(GetHWND(), L"����ʧ��");
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

void CMainWindow::GetDpiSetting(int& currentDpiIndex, CMouseDpiSetting dpiSetting[6])
{
	for (int i = 0; i < 6; i++)
	{
		std::wstring controlName = std::wstring(L"dpiValueLabel") + std::to_wstring(i + 1);
		CButtonUI* dpiValueLabel = (CButtonUI*)m_PaintManager.FindControl(controlName.c_str());
		std::wstring text = dpiValueLabel->GetText().GetData();
		dpiSetting[i].m_dpiLevel = _wtoi(text.c_str());

		controlName = std::wstring(L"dpiColorBtn") + std::to_wstring(i + 1);
		CControlUI* dpiColorBtn = m_PaintManager.FindControl(controlName.c_str());
		dpiSetting[i].m_dpiColor = dpiColorBtn->GetBkColor();
		if (dpiValueLabel->GetTextColor() == CURRENT_DPI_TEXT_COLOR)
		{
			currentDpiIndex = i;
		}
	}
}

void CMainWindow::SetDpiSettingToMouse()
{
	// �ӽ����ȡDPI����
	int currentDpi = 0;
	CMouseDpiSetting dpiSetting[6];
	GetDpiSetting(currentDpi, dpiSetting);

	// ����Э���
	CProtocalPackage package;
	package.m_commandId = 0xd1;

	// ��ӵ�ǰ�ȼ�
	CProtocalTLV currentDpiTlv;
	currentDpiTlv.m_type = 0x41;
	currentDpiTlv.m_length = 0x03;
	currentDpiTlv.m_value[0] = (unsigned char)(currentDpi + 1);
	package.m_tlvs.push_back(currentDpiTlv);

	// ���ÿһ���ȼ�����
	for (int i = 0; i < 6; i++)
	{
		CProtocalTLV tlv;
		tlv.m_type = 0x42;
		tlv.m_length = 0x08;
		tlv.m_value[0] = (unsigned char)(i + 1);
		tlv.m_value[1] = dpiSetting[i].m_dpiLevel & 0xff;
		tlv.m_value[2] = (dpiSetting[i].m_dpiLevel >> 8) & 0xff;
		tlv.m_value[3] = GetRValue(dpiSetting[i].m_dpiColor);
		tlv.m_value[4] = GetGValue(dpiSetting[i].m_dpiColor);
		tlv.m_value[5] = GetBValue(dpiSetting[i].m_dpiColor);
		package.m_tlvs.push_back(tlv);
	}	

	SendSetting(SETTING_CATEGORY_DPI, package);
}