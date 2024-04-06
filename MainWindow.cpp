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
	if (!m_manualTrigger)
	{
		return;
	}

	PostMessage(WM_SELECT_ITEM, 0, (LPARAM)msg.pSender);
}

void CMainWindow::OnSelectChangeEvent(TNotifyUI& msg)
{
	std::wstring controlName = msg.pSender->GetName();
	if (controlName == L"lineRejustOption" || controlName == L"motionSyncOption" || controlName == L"rippleControlOption")
	{
		SetSensorSettingToMouse();
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

	if (package.m_commandId == 0xd1) // ������������Ӧ
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
	// ����Value
	unsigned char tlvData[6];
	memset(tlvData, 0, sizeof(tlvData));
	tlvData[0] = (unsigned char)keyNum;  // key num
	tlvData[1] = 0x02;  // report id, Ĭ�������
	tlvData[4] = 0x00;  // disable��Ĭ����enable
	tlvData[5] = 0x01;  // loop count��Ĭ����1

	if (keyIndex == KEY_INDEX_LEFT)
	{
		tlvData[2] = 0x01;
		tlvData[3] = 0x00;
	}
	else if (keyIndex == KEY_INDEX_RIGHT)
	{
		tlvData[2] = 0x02;
		tlvData[3] = 0x00;
	}
	else if (keyIndex == KEY_INDEX_MIDDLE)
	{
		tlvData[2] = 0x04;
		tlvData[3] = 0x00;
	}
	else if (keyIndex == KEY_INDEX_BACKWARD)
	{
		tlvData[2] = 0x08;
		tlvData[3] = 0x00;
	}
	else if (keyIndex == KEY_INDEX_FORWARD)
	{
		tlvData[2] = 0x10;
		tlvData[3] = 0x00;
	}
	else if (keyIndex == KEY_INDEX_DPIPLUS)
	{
		tlvData[2] = 0xFF;
		tlvData[3] = 0xFF;
	}
	else if (keyIndex == KEY_INDEX_DPISUB)
	{
		tlvData[2] = 0xFF;
		tlvData[3] = 0x7F;
	}
	else if (keyIndex == KEY_INDEX_DPISWITCH)
	{
		tlvData[2] = 0xFF;
		tlvData[3] = 0x00;
	}
	else if (keyIndex == KEY_INDEX_HUOLI)
	{
		tlvData[2] = 0x01;
		tlvData[3] = 0x00;
		tlvData[5] = 0X02;
	}
	else if (keyIndex == KEY_INDEX_SANLIANFA)
	{
		tlvData[2] = 0x01;
		tlvData[3] = 0x00;
		tlvData[5] = 0X03;
	}
	else if (keyIndex == KEY_INDEX_LIGHT)
	{
		tlvData[2] = 0x0F;
		tlvData[3] = 0xFF;
	}
	else if (keyIndex == KEY_INDEX_COMBIN)
	{
		// todo by yejinlong, ��ϼ���ʵ��
	}
	else if (keyIndex == KEY_INDEX_MM_PLAYER)
	{
		tlvData[1] = 0x03;
		tlvData[2] = 0x83;
		tlvData[3] = 0x01;
	}
	else if (keyIndex == KEY_INDEX_MM_LAST)
	{
		tlvData[1] = 0x03;
		tlvData[2] = 0xb6;
		tlvData[3] = 0x00;
	}
	else if (keyIndex == KEY_INDEX_MM_NEXT)
	{
		tlvData[1] = 0x03;
		tlvData[2] = 0xb5;
		tlvData[3] = 0x00;
	}
	else if (keyIndex == KEY_INDEX_MM_STOP)
	{
		tlvData[1] = 0x03;
		tlvData[2] = 0xcc;
		tlvData[3] = 0x00;
	}
	else if (keyIndex == KEY_INDEX_MM_PLAY)
	{
		tlvData[1] = 0x03;
		tlvData[2] = 0xcd;
		tlvData[3] = 0x00;
	}
	else if (keyIndex == KEY_INDEX_MM_MUSE)
	{
		tlvData[1] = 0x03;
		tlvData[2] = 0xe2;
		tlvData[3] = 0x00;
	}
	else if (keyIndex == KEY_INDEX_MM_VADD)
	{
		tlvData[1] = 0x03;
		tlvData[2] = 0xe9;
		tlvData[3] = 0x00;
	}
	else if (keyIndex == KEY_INDEX_MM_VSUB)
	{
		tlvData[1] = 0x03;
		tlvData[2] = 0xea;
		tlvData[3] = 0x00;
	}
	else if (keyIndex == KEY_INDEX_MM_MAIL)
	{
		tlvData[1] = 0x03;
		tlvData[2] = 0x8a;
		tlvData[3] = 0x01;
	}
	else if (keyIndex == KEY_INDEX_MM_CALC)
	{
		tlvData[1] = 0x03;
		tlvData[2] = 0x92;
		tlvData[3] = 0x01;
	}
	else if (keyIndex == KEY_INDEX_COPY)
	{
		tlvData[1] = 0x01;
		tlvData[2] = 0x01;
		tlvData[3] = 0x06;
	}
	else if (keyIndex == KEY_INDEX_PASTE)
	{
		tlvData[1] = 0x01;
		tlvData[2] = 0x01;
		tlvData[3] = 0x19;
	}
	else if (keyIndex == KEY_INDEX_CUT)
	{
		tlvData[1] = 0x01;
		tlvData[2] = 0x01;
		tlvData[3] = 0x1b;
	}
	else if (keyIndex == KEY_INDEX_ALL)
	{
		tlvData[1] = 0x01;
		tlvData[2] = 0x01;
		tlvData[3] = 0x04;
	}
	else if (keyIndex == KEY_INDEX_NEW)
	{
		tlvData[1] = 0x01;
		tlvData[2] = 0x01;
		tlvData[3] = 0x11;
	}
	else if (keyIndex == KEY_INDEX_SAVE)
	{
		tlvData[1] = 0x01;
		tlvData[2] = 0x01;
		tlvData[3] = 0x16;
	}
	else if (keyIndex == KEY_INDEX_PRINT)
	{
		tlvData[1] = 0x01;
		tlvData[2] = 0x01;
		tlvData[3] = 0x13;
	}
	else if (keyIndex == KEY_INDEX_OPEN)
	{
		tlvData[1] = 0x01;
		tlvData[2] = 0x08;
		tlvData[3] = 0x08;
	}
	else if (keyIndex == KEY_INDEX_UNDO)
	{
		tlvData[1] = 0x01;
		tlvData[2] = 0x01;
		tlvData[3] = 0x1d;
	}
	else if (keyIndex == KEY_INDEX_COMPUTER)
	{
		tlvData[1] = 0x01;
		tlvData[2] = 0x08;
		tlvData[3] = 0x08;
	}
	else if (keyIndex == KEY_INDEX_CLOSEWIN)
	{
		tlvData[1] = 0x01;
		tlvData[2] = 0x04;
		tlvData[3] = 0x3d;
	}
	else if (keyIndex == KEY_INDEX_DESKTOP)
	{
		tlvData[1] = 0x01;
		tlvData[2] = 0x08;
		tlvData[3] = 0x07;
	}
	else if (keyIndex == KEY_INDEX_RUN)
	{
		tlvData[1] = 0x01;
		tlvData[2] = 0x08;
		tlvData[3] = 0x15;
	}
	else if (keyIndex == KEY_INDEX_MINIMIZE)
	{
		tlvData[1] = 0x01;
		tlvData[2] = 0x08;
		tlvData[3] = 0x51;
	}
	else if (keyIndex == KEY_INDEX_MAXMIZE)
	{
		tlvData[1] = 0x01;
		tlvData[2] = 0x08;
		tlvData[3] = 0x52;
	}
	else if (keyIndex == KEY_INDEX_OPENAPP)
	{
		tlvData[1] = 0x04;
	}
	else if (keyIndex == KEY_INDEX_OPENWEB)
	{
		tlvData[1] = 0x05;
	}
	else if (keyIndex == KEY_INDEX_ENTERTEXT)
	{
		tlvData[1] = 0x06;
	}
	else if (keyIndex == KEY_INDEX_DISABLE)
	{
		tlvData[4] = 0x01;
	}

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
	control->Selected(check);
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