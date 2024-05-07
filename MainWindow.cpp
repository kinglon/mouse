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
#include "GetOptionWindow.h"
#include "MacroEventMapping.h"
#include "MacroWindow.h"

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

// selectitem事件处理函数内不能ShowModal()，于是异步发送一个新消息
#define WM_SELECT_ITEM			WM_USER+101

// selectchange事件处理函数内不能ShowModal()，于是异步发送一个新消息
#define WM_SELECT_CHANGE		WM_USER+102

#define CURRENT_DPI_TEXT_COLOR	0xffff0000

#define TIMERID_RUNTASKPOOL		100

// 任务ID
#define TASKID_GET_MOUSE_SETTING		1
#define TASKID_GET_BATTERY				2

// 按键控件名字
static std::wstring keyBtnNames[] = { L"firstKeyBtn", L"secondKeyBtn", L"thirdKeyBtn", L"fouthKeyBtn", L"fifthKeyBtn", L"sixthKeyBtn" };

// 宏定义按键名称
#define KEY_NAME_MACRO L"宏定义-"

CMainWindow::CMainWindow()
{
	InitKeyMenu();	
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

	if (uMsg == WM_MEASUREITEM)
	{		
		MEASUREITEMSTRUCT* lpMeasureItem = (MEASUREITEMSTRUCT*)lParam;
		if (lpMeasureItem->CtlType == ODT_MENU)
		{
			lpMeasureItem->itemWidth = 130;
			lpMeasureItem->itemHeight = 25;
			return 0L;
		}
	}

	if (uMsg == WM_DRAWITEM)
	{
		LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lParam;
		if (dis->CtlType == ODT_MENU)
		{
			OnDrawItem(dis);
			return 0L;
		}
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

	if (uMsg == WM_SELECT_CHANGE)
	{		
		CControlUI* control = (CControlUI*)lParam;
		std::wstring controlName = control->GetName();
		if (controlName == L"lineRejustOption" || controlName == L"motionSyncOption" || controlName == L"rippleControlOption")
		{
			SetSensorSettingToMouse();
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
	COptionUI* control = (COptionUI*)msg.pSender;
	control->SetBkImage(control->IsSelected() ? L"option_check.png" : L"option_uncheck.png");

	if (!m_manualTrigger)
	{
		return;
	}

	PostMessage(WM_SELECT_CHANGE, 0, (LPARAM)msg.pSender);
}

void CMainWindow::OnWindowInit(TNotifyUI& msg)
{
	UpdateControls(SETTING_CATEGORY_ALL);

	m_taskPool.push_back(TASKID_GET_BATTERY);
	m_taskPool.push_back(TASKID_GET_MOUSE_SETTING);
	RunTaskPool();
	SetTimer(GetHWND(), TIMERID_RUNTASKPOOL, 3000, NULL);
}

bool CMainWindow::OnEvent(void* event2)
{	
	TEventUI* event = (TEventUI*)event2;
	if (event->Type == UIEVENT_MOUSEENTER)
	{
		for (int i = 0; i < ARRAYSIZE(keyBtnNames); i++)
		{
			if (event->pSender->GetName() == keyBtnNames[i].c_str())
			{
				std::wstring number_control_name = std::wstring(L"number") + std::to_wstring(i + 1);
				m_PaintManager.FindControl(number_control_name.c_str())->SetVisible(true);
				break;
			}
		}
	}
	else if (event->Type == UIEVENT_MOUSELEAVE)
	{
		for (int i = 0; i < ARRAYSIZE(keyBtnNames); i++)
		{
			if (event->pSender->GetName() == keyBtnNames[i].c_str())
			{
				std::wstring number_control_name = std::wstring(L"number") + std::to_wstring(i + 1);
				m_PaintManager.FindControl(number_control_name.c_str())->SetVisible(false);
				break;
			}
		}
	}

	return true;
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

	// 设置鼠标参数成功、重置参数成功
	if (package.m_commandId == 0xd1 || package.m_commandId == 0xd2 || package.m_commandId == 0xd4)
	{
		if (m_waitingWindow)
		{
			m_waitingWindow->SetSuccess(true);
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

int CMainWindow::GetKeyIndexByCtrlName(std::wstring ctrlName)
{
	int keyNum = -1;
	for (int i = 0; i < ARRAYSIZE(keyBtnNames); i++)
	{
		if (ctrlName == keyBtnNames[i].c_str())
		{
			keyNum = i + 1;
			break;
		}
	}

	return keyNum;
}

void CMainWindow::OnMenuCommand(int commandId)
{
	if (m_clickedKeyBtn == nullptr)
	{
		return;
	}

	// 设置为宏命令，特殊处理
	if (commandId == ID_KEY_MACRO)
	{
		OnMacroMenuCommand();
		return;
	}

	// DPI锁定，弹窗设置DPI值
	int dpiValue = 200;
	if (commandId == ID_KEY_DPILOCK)
	{
		CDpiLockWindow dpiLockWindow;
		dpiLockWindow.Create(GetHWND(), NULL, WS_VISIBLE | WS_POPUP, 0);
		dpiLockWindow.CenterWindow();
		if (dpiLockWindow.ShowModal() == 0) // 取消
		{
			return;
		}
		dpiValue = dpiLockWindow.GetDpiValue();
	}

	// 获取KeyNum
	int keyNum = GetKeyIndexByCtrlName(m_clickedKeyBtn->GetName().GetData());
	if (keyNum < 1)
	{
		return;
	}

	// 获取按键名称
	std::wstring keyName = CKeyMapping::GetKeyNameByCommandId(commandId);
	if (keyName.empty())
	{
		return;
	}	

	// 更改按键名称	
	m_clickedKeyBtn->SetText(keyName.c_str());

	// 发送按键配置给鼠标
	if (commandId == ID_KEY_DPILOCK)
	{
		SetDpiLockToMouse(keyNum, dpiValue);
	}
	else{
		SetKeyToMouse(keyNum, CKeyMapping::GetKeyIndexByName(keyName));
	}
}

void CMainWindow::OnMacroMenuCommand()
{
	// 没有任何宏命令，提示它
	auto& macroCmdConfig = CSettingManager::GetInstance()->m_macroCmdConfig;
	if (macroCmdConfig.size() == 0)
	{
		CMyMessageBox::Show(GetHWND(), L"没有宏可以选择");
		return;
	}

	// 选择一个宏
	std::vector<std::wstring> options;
	for (auto& macroCmd : macroCmdConfig)
	{
		options.push_back(macroCmd.m_name);
	}

	CGetOptionWindow getOptionWindow;
	getOptionWindow.SetTitle(L"选择一个宏");
	getOptionWindow.SetOptions(options);
	getOptionWindow.Create(GetHWND(), NULL, WS_VISIBLE | WS_POPUP, 0);
	getOptionWindow.CenterWindow();
	if (getOptionWindow.ShowModal() == 0) // 取消
	{
		return;
	}
	std::wstring macroName = getOptionWindow.GetSelectedOption();

	// 获取KeyNum
	int keyNum = GetKeyIndexByCtrlName(m_clickedKeyBtn->GetName().GetData());
	if (keyNum < 1)
	{
		return;
	}

	// 更改按键名称为：宏定义-xxx
	std::wstring keyName = KEY_NAME_MACRO;
	keyName += macroName;
	m_clickedKeyBtn->SetText(keyName.c_str());

	// 发送宏配置给鼠标
	SetMacroToMouse(keyNum, macroName);
}

void CMainWindow::OnClickEvent(TNotifyUI& msg)
{
	auto& controlName = msg.pSender->GetName();
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
		CControlUI* keyPanel = m_PaintManager.FindControl(L"keyPanel");
		if (!keyPanel->IsVisible())
		{			
			keyPanel->SetVisible(true);
			UpdateKeyPanel();
		}
		else
		{
			keyPanel->SetVisible(false);
		}
		SetPanelHeadBkImage(msg.pSender, keyPanel->IsVisible());
	}	
	else if (controlName == L"batteryHeadBtn")
	{
		CControlUI* batteryPanel = m_PaintManager.FindControl(L"batteryPanel");
		batteryPanel->SetVisible(!batteryPanel->IsVisible());
		SetPanelHeadBkImage(msg.pSender, batteryPanel->IsVisible());
	}
	else if (controlName == L"powerHeadBtn")
	{
		CControlUI* powerPanel = m_PaintManager.FindControl(L"powerPanel");
		if (!powerPanel->IsVisible())
		{
			powerPanel->SetVisible(true);
			UpdatePowerPannel();
		}
		else
		{
			powerPanel->SetVisible(false);
		}
		SetPanelHeadBkImage(msg.pSender, powerPanel->IsVisible());
	}	
	else if (controlName == L"dpiHeadBtn")
	{
		CControlUI* dpiPanel = m_PaintManager.FindControl(L"dpiPanel");
		if (!dpiPanel->IsVisible())
		{
			CloseRightSettingPannels();
			dpiPanel->SetVisible(true);
			UpdateDpiPanel();
		}
		else
		{
			dpiPanel->SetVisible(false);
		}
		SetPanelHeadBkImage(msg.pSender, dpiPanel->IsVisible());
	}
	else if (controlName == L"lightHeadBtn")
	{
		CControlUI* lightPanel = m_PaintManager.FindControl(L"lightPanel");
		if (!lightPanel->IsVisible())
		{
			CloseRightSettingPannels();
			lightPanel->SetVisible(true);
			UpdateLightPanel();
		}
		else
		{
			lightPanel->SetVisible(false);
		}
		SetPanelHeadBkImage(msg.pSender, lightPanel->IsVisible());
	}
	else if (controlName == L"huiBaoHeadBtn")
	{
		CControlUI* huiBaoPanel = m_PaintManager.FindControl(L"huiBaoPanel");
		if (!huiBaoPanel->IsVisible())
		{
			CloseRightSettingPannels();
			huiBaoPanel->SetVisible(true);
			UpdateHuibaoPanel();
		}
		else
		{
			huiBaoPanel->SetVisible(false);
		}
		SetPanelHeadBkImage(msg.pSender, huiBaoPanel->IsVisible());
	}
	else if (controlName == L"lodHeadBtn")
	{
		CControlUI* lodPanel = m_PaintManager.FindControl(L"lodPanel");
		if (!lodPanel->IsVisible())
		{
			CloseRightSettingPannels();
			lodPanel->SetVisible(true);
			UpdateLodPanel();
		}
		else
		{
			lodPanel->SetVisible(false);
		}
		SetPanelHeadBkImage(msg.pSender, lodPanel->IsVisible());
	}
	else if (controlName == L"qudouHeadBtn")
	{
		CControlUI* qudouPanel = m_PaintManager.FindControl(L"qudouPanel");
		if (!qudouPanel->IsVisible())
		{
			CloseRightSettingPannels();
			qudouPanel->SetVisible(true);
			UpdateQudouPanel();
		}
		else
		{
			qudouPanel->SetVisible(false);
		}
		SetPanelHeadBkImage(msg.pSender, qudouPanel->IsVisible());
	}
	else if (controlName == L"systemSettingHeadBtn")
	{
		CControlUI* systemSettingPanel = m_PaintManager.FindControl(L"systemSettingPanel");
		if (!systemSettingPanel->IsVisible())
		{
			CloseRightSettingPannels();
			systemSettingPanel->SetVisible(true);
		}
		else
		{
			systemSettingPanel->SetVisible(false);
		}
		SetPanelHeadBkImage(msg.pSender, systemSettingPanel->IsVisible());
	}
	else if (controlName == L"sensorHeadBtn")
	{
		CControlUI* sensorPanel = m_PaintManager.FindControl(L"sensorPanel");
		if (!sensorPanel->IsVisible())
		{
			CloseRightSettingPannels();
			sensorPanel->SetVisible(true);
			UpdateSensorPanel();
		}
		else
		{
			sensorPanel->SetVisible(false);
		}
		SetPanelHeadBkImage(msg.pSender, sensorPanel->IsVisible());
	}
	else if (controlName == L"matchHeadBtn")
	{
		CControlUI* matchPanel = m_PaintManager.FindControl(L"matchPanel");
		if (!matchPanel->IsVisible())
		{
			CloseRightSettingPannels();
			matchPanel->SetVisible(true);
		}
		else
		{
			matchPanel->SetVisible(false);
		}
		SetPanelHeadBkImage(msg.pSender, matchPanel->IsVisible());
	}
	else if (controlName == L"firstKeyBtn" || controlName == L"secondKeyBtn" || controlName == L"thirdKeyBtn" || \
		controlName == L"fouthKeyBtn" || controlName == L"fifthKeyBtn" || controlName == L"sixthKeyBtn")
	{
		m_clickedKeyBtn = msg.pSender;
		PopupKeyMenu(msg.pSender);
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
	else if (controlName == L"macroHeadBtn")
	{
		OnMacroBtnClicked();
	}
}

void CMainWindow::CloseRightSettingPannels()
{
	m_PaintManager.FindControl(L"dpiPanel")->SetVisible(false);
	SetPanelHeadBkImage(m_PaintManager.FindControl(L"dpiHeadBtn"), false);

	m_PaintManager.FindControl(L"lightPanel")->SetVisible(false);
	SetPanelHeadBkImage(m_PaintManager.FindControl(L"lightHeadBtn"), false);

	m_PaintManager.FindControl(L"huiBaoPanel")->SetVisible(false);
	SetPanelHeadBkImage(m_PaintManager.FindControl(L"huiBaoHeadBtn"), false);

	m_PaintManager.FindControl(L"lodPanel")->SetVisible(false);
	SetPanelHeadBkImage(m_PaintManager.FindControl(L"lodHeadBtn"), false);

	m_PaintManager.FindControl(L"qudouPanel")->SetVisible(false);
	SetPanelHeadBkImage(m_PaintManager.FindControl(L"qudouHeadBtn"), false);

	m_PaintManager.FindControl(L"systemSettingPanel")->SetVisible(false);
	SetPanelHeadBkImage(m_PaintManager.FindControl(L"systemSettingHeadBtn"), false);

	m_PaintManager.FindControl(L"sensorPanel")->SetVisible(false);
	SetPanelHeadBkImage(m_PaintManager.FindControl(L"sensorHeadBtn"), false);

	m_PaintManager.FindControl(L"matchPanel")->SetVisible(false);
	SetPanelHeadBkImage(m_PaintManager.FindControl(L"matchHeadBtn"), false);
}

void CMainWindow::SetPanelHeadBkImage(CControlUI* control, bool open)
{
	if (open)
	{
		control->SetBkImage(L"file='header2_ov.png' corner='5,5,40,5'");
	}
	else
	{
		control->SetBkImage(L"file='header2_nr.png' corner='5,5,40,5'");
	}
}

void CMainWindow::InitKeyMenu()
{
	m_keyMenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_KEY_MENU));

	// 设置菜单背景色
	static HBRUSH brush = CreateSolidBrush(RGB(56, 56, 56));	
	MENUINFO mi;
	mi.cbSize = sizeof(MENUINFO);
	mi.fMask = MIM_BACKGROUND;
	mi.hbrBack = brush;

	HMENU firstLayerMenu = GetSubMenu(m_keyMenu, 0);
	SetMenuInfo(firstLayerMenu, &mi);

	// 设置所有子菜单为自绘
	MENUITEMINFO mii;
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_FTYPE;
	mii.fType = MFT_OWNERDRAW;
	
	int firstLayerMenuCount = GetMenuItemCount(firstLayerMenu);	
	for (int i = 0; i < firstLayerMenuCount; i++)
	{
		SetMenuItemInfo(firstLayerMenu, i, TRUE, &mii);
		HMENU currentMenu = GetSubMenu(firstLayerMenu, i);
		SetMenuInfo(currentMenu, &mi);
		int currentMenuCount = GetMenuItemCount(currentMenu);
		for (int j = 0; j < currentMenuCount; j++)
		{
			SetMenuItemInfo(currentMenu, j, TRUE, &mii);
		}
	}
}

void CMainWindow::OnDrawItem(LPDRAWITEMSTRUCT lpdis)
{
	Graphics graphics(lpdis->hDC);

	// 根据菜单状态获取背景图
	std::wstring bgFilePath = CImPath::GetSoftInstallPath() + CResourceUtil::GetSkinFolder();
	bool selected = (lpdis->itemState & ODS_SELECTED);
	if (selected)
	{
		bgFilePath += L"button2_ov.png";
	}
	else
	{		
		bgFilePath += L"button2_nr.png";
	}

	Gdiplus::Bitmap bgImage(bgFilePath.c_str());
	if (bgImage.GetLastStatus() != Gdiplus::Ok)
	{
		LOG_ERROR(L"failed to load menu bg image, path is %s", bgFilePath.c_str());
		return;
	}

	// 等比例画背景图
	Gdiplus::Rect destRect;
	destRect.X = lpdis->rcItem.left;
	destRect.Y = lpdis->rcItem.top;
	destRect.Width = lpdis->rcItem.right - lpdis->rcItem.left;
	destRect.Height = lpdis->rcItem.bottom - lpdis->rcItem.top;
	graphics.DrawImage(&bgImage, destRect);

	// 获取菜单文本
	wchar_t text[MAX_PATH] = {0};
	MENUITEMINFO menuItemInfo;
	menuItemInfo.cbSize = sizeof(MENUITEMINFO);
	menuItemInfo.fMask = MIIM_STRING;
	menuItemInfo.cch = MAX_PATH;
	menuItemInfo.dwTypeData = text;
	if (!GetMenuItemInfo((HMENU)lpdis->hwndItem, lpdis->itemID, FALSE, &menuItemInfo))
	{
		LOG_ERROR(L"failed to call GetMenuItemInfo, error is %d", GetLastError());
		return;
	}	

	if (wcslen(text) == 0)
	{
		return;
	}

	// 画文本
	Gdiplus::Color textColor;
	textColor.SetFromCOLORREF(RGB(0xf0, 0xf0, 0xf0));
	Gdiplus::SolidBrush textBrush(textColor);
	
	Gdiplus::Font textFont(L"微软雅黑", 12, FontStyleRegular, UnitPixel);
	Gdiplus::StringFormat stringFormat;
	stringFormat.SetAlignment(Gdiplus::StringAlignment::StringAlignmentNear);
	stringFormat.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);

	Gdiplus::RectF layoutRect((Gdiplus::REAL)lpdis->rcItem.left + 30, (Gdiplus::REAL)lpdis->rcItem.top,
		(Gdiplus::REAL)(lpdis->rcItem.right - lpdis->rcItem.left), (Gdiplus::REAL)(lpdis->rcItem.bottom - lpdis->rcItem.top));
	graphics.DrawString(text, -1, &textFont, layoutRect, &stringFormat, &textBrush);
}

void CMainWindow::InitControls()
{
	// 监听按键按钮事件
	for (int i = 0; i < ARRAYSIZE(keyBtnNames); i++)
	{
		CButtonUI* keyBtn = (CButtonUI*)m_PaintManager.FindControl(keyBtnNames[i].c_str());
		keyBtn->OnEvent += CDelegate<CMainWindow, CMainWindow>(this, &CMainWindow::OnEvent);
	}

	// 睡眠时间下拉框添加选项
	CComboUI* timeCombo = (CComboUI*)m_PaintManager.FindControl(L"timeCombo");
	timeCombo->SetMouseWheelEnable(false);
	for (int i = 1; i <= 8; i++)
	{
		CListLabelElementUI* pControl = new CListLabelElementUI();		
		pControl->SetText(std::to_wstring(i).c_str());
		timeCombo->Add(pControl);
	}

	// 灯效设置下拉框添加选项
	CComboUI* lightCombo = (CComboUI*)m_PaintManager.FindControl(L"lightCombo");
	lightCombo->SetMouseWheelEnable(false);
	static std::wstring lightOptionNames[] = { L"关闭", L"呼吸", L"单色常量"};
	for (int i = 0; i < ARRAYSIZE(lightOptionNames); i++)
	{
		CListLabelElementUI* pControl = new CListLabelElementUI();
		pControl->SetText(lightOptionNames[i].c_str());
		lightCombo->Add(pControl);
	}

	// 回报率设置下拉框添加选项
	CComboUI* huibaoCombo = (CComboUI*)m_PaintManager.FindControl(L"huiBaoCombo");
	huibaoCombo->SetMouseWheelEnable(false);
	static std::wstring huibaoOptionNames[] = { L"125Hz", L"250Hz", L"500Hz", L"1000Hz", L"2000Hz", L"4000Hz", L"8000Hz" };
	for (int i = 0; i < ARRAYSIZE(huibaoOptionNames); i++)
	{
		CListLabelElementUI* pControl = new CListLabelElementUI();
		pControl->SetText(huibaoOptionNames[i].c_str());
		huibaoCombo->Add(pControl);
	}

	// LOD设置下拉框添加选项
	CComboUI* lodCombo = (CComboUI*)m_PaintManager.FindControl(L"lodCombo");
	lodCombo->SetMouseWheelEnable(false);
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
	
	std::wstring keyName;
	if (mouseConfig.m_firstKey == KEY_INDEX_MACRO)
	{
		keyName = KEY_NAME_MACRO;
		keyName += mouseConfig.m_macroCmdNames[0];
	}
	else
	{
		keyName = CKeyMapping::GetKeyNameByKeyIndex(mouseConfig.m_firstKey);
	}
	m_PaintManager.FindControl(L"firstKeyBtn")->SetText(keyName.c_str());

	if (mouseConfig.m_secondKey == KEY_INDEX_MACRO)
	{
		keyName = KEY_NAME_MACRO;
		keyName += mouseConfig.m_macroCmdNames[1];
	}
	else
	{
		keyName = CKeyMapping::GetKeyNameByKeyIndex(mouseConfig.m_secondKey);
	}	
	m_PaintManager.FindControl(L"secondKeyBtn")->SetText(keyName.c_str());

	if (mouseConfig.m_thirdtKey == KEY_INDEX_MACRO)
	{
		keyName = KEY_NAME_MACRO;
		keyName += mouseConfig.m_macroCmdNames[2];
	}
	else
	{
		keyName = CKeyMapping::GetKeyNameByKeyIndex(mouseConfig.m_thirdtKey);
	}	
	m_PaintManager.FindControl(L"thirdKeyBtn")->SetText(keyName.c_str());

	if (mouseConfig.m_fourthKey == KEY_INDEX_MACRO)
	{
		keyName = KEY_NAME_MACRO;
		keyName += mouseConfig.m_macroCmdNames[3];
	}
	else
	{
		keyName = CKeyMapping::GetKeyNameByKeyIndex(mouseConfig.m_fourthKey);
	}
	m_PaintManager.FindControl(L"fouthKeyBtn")->SetText(keyName.c_str());

	if (mouseConfig.m_fifthKey == KEY_INDEX_MACRO)
	{
		keyName = KEY_NAME_MACRO;
		keyName += mouseConfig.m_macroCmdNames[4];
	}
	else
	{
		keyName = CKeyMapping::GetKeyNameByKeyIndex(mouseConfig.m_fifthKey);
	}
	m_PaintManager.FindControl(L"fifthKeyBtn")->SetText(keyName.c_str());

	if (mouseConfig.m_sixthKey == KEY_INDEX_MACRO)
	{
		keyName = KEY_NAME_MACRO;
		keyName += mouseConfig.m_macroCmdNames[5];
	}
	else
	{
		keyName = CKeyMapping::GetKeyNameByKeyIndex(mouseConfig.m_sixthKey);
	}
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
	// 选择颜色
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

	// 更新配置给鼠标
	SetDpiSettingToMouse();
}

void CMainWindow::ClickDpiValueBtn(CButtonUI* button)
{
	// 获取DPI值
	int dpiValue = 200;
	CDpiLockWindow dpiLockWindow;
	dpiLockWindow.Create(GetHWND(), NULL, WS_VISIBLE | WS_POPUP, 0);
	dpiLockWindow.CenterWindow();
	if (dpiLockWindow.ShowModal() == 0) // 取消
	{
		return;
	}
	dpiValue = dpiLockWindow.GetDpiValue();

	// 获取DPI索引
	int dpiIndex = GetDpiIndexByControlName(button->GetName().GetData());
	if (dpiIndex < 1 || dpiIndex > 6)
	{
		return;
	}

	// 更新界面
	CControlUI* dpiValueLabel = m_PaintManager.FindControl((std::wstring(L"dpiValueLabel") + std::to_wstring(dpiIndex)).c_str());
	dpiValueLabel->SetText(std::to_wstring(dpiValue).c_str());

	CControlUI* dpiValueBtn = m_PaintManager.FindControl((std::wstring(L"dpiValueBtn") + std::to_wstring(dpiIndex)).c_str());
	CControlUI* dpiValueForgroundBtn = m_PaintManager.FindControl((std::wstring(L"dpiValueForgroundBtn") + std::to_wstring(dpiIndex)).c_str());
	int height = (int)((dpiValue / 26000.0f)*dpiValueBtn->GetFixedHeight());
	dpiValueForgroundBtn->SetFixedHeight(height);

	// 更新配置给鼠标
	SetDpiSettingToMouse();
}

void CMainWindow::SwitchCurrentDpi(CButtonUI* button)
{
	// 更改当前DPI的字体颜色
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

	// 更新配置给鼠标
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
	if (!CMyMessageBox::Show(GetHWND(), L"当前配置是否需要恢复到默认配置？", true))
	{
		return;
	}

	if (!CMouseCommManager::GetInstance()->IsMouseConnected())
	{
		CMyMessageBox::Show(GetHWND(), L"鼠标未连接");
		return;
	}	

	CProtocalPackage package;
	package.m_commandId = 0xd4;
	if (!CProtocalUtil::SendPackage(package))
	{
		CMyMessageBox::Show(GetHWND(), L"重置失败");
		return;
	}

	// 显示等待窗口，要么等待超时结束，要么外部设置结果关闭
	m_waitingWindow = new CWaitingWindow();
	m_waitingWindow->SetCaption(L"重置中...");
	m_waitingWindow->SetWaitSec(3);
	m_waitingWindow->Create(GetHWND(), NULL, WS_VISIBLE | WS_OVERLAPPEDWINDOW, 0);
	m_waitingWindow->CenterWindow();
	m_waitingWindow->ShowModal();
	bool success = m_waitingWindow->IsSuccess();
	delete m_waitingWindow;
	m_waitingWindow = nullptr;
	
	if (!success)
	{
		CMyMessageBox::Show(GetHWND(), L"重置失败");
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
		CMyMessageBox::Show(GetHWND(), L"只允许输入1-30");
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
	ShellExecute(NULL, NULL, L"control", L"mouse", NULL, SW_SHOW);

	// 鼠标属性窗口总是在后面，特殊处理，把它带到前面来
	for (int i = 0; i < 20; i++)
	{
		Sleep(100);
		HWND hWnd = FindWindow(NULL, L"鼠标 属性");
		if (hWnd != NULL)
		{
			BringWindowToTop(hWnd);
			break;
		}
	}
}

void CMainWindow::OnMacroBtnClicked()
{
	CMacroWindow macroWindow;
	macroWindow.Create(GetHWND(), NULL, WS_VISIBLE | WS_POPUP, 0);
	macroWindow.CenterWindow();
	macroWindow.ShowModal();
}

void CMainWindow::OnMatchBtnClicked()
{
	// todo by yejinlong，配对功能
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
		LOG_INFO(L"failed to find the key struct of %d", keyIndex);
		CMyMessageBox::Show(GetHWND(), L"暂时不支持该按键设置");
		UpdateControls(SETTING_CATEGORY_KEY);
		return;
	}


	// 构造Value
	unsigned char tlvData[6];
	memset(tlvData, 0, sizeof(tlvData));
	tlvData[0] = (unsigned char)keyNum;  // key num
	tlvData[1] = it->second.m_reportId;  // report id
	tlvData[2] = it->second.m_keyCode1;  // key code1
	tlvData[3] = it->second.m_keyCode2;  // key code2
	tlvData[4] = it->second.m_disable;  // disable
	tlvData[5] = it->second.m_loopCount;  // loop count

	// 构造TLV
	CProtocalTLV tlv;
	tlv.m_type = 0x40;
	tlv.m_length = 0x08;
	memcpy(tlv.m_value, tlvData, sizeof(tlvData));

	// 构造协议包
	CProtocalPackage package;
	package.m_reportId = 0xa5;
	package.m_commandId = 0xd1;
	if (keyIndex == KEY_INDEX_OPENAPP || keyIndex == KEY_INDEX_OPENWEB || keyIndex == KEY_INDEX_ENTERTEXT)
	{
		package.m_commandId = 0xd2;
	}
	package.m_tlvs.push_back(tlv);

	// 发送协议包
	SendSetting(SETTING_CATEGORY_KEY, package);
}

void CMainWindow::SetMacroToMouse(int keyNum, std::wstring macroName)
{
	// 获取宏命令详细配置
	CMacroCmd* macroCmd = nullptr;
	for (auto& item : CSettingManager::GetInstance()->m_macroCmdConfig)
	{
		if (item.m_name == macroName)
		{
			macroCmd = &item;
			break;
		}
	}
	if (macroCmd == nullptr)
	{
		return;
	}

	// 合并延时事件
	std::vector<CMacroEvent> newEvents;
	int delay = 0;
	for (const auto& event : macroCmd->m_events)
	{
		if (event.m_type == 3)
		{
			delay += event.m_delayMillSec;
			continue;
		}
		else
		{
			CMacroEvent newEvent = event;
			newEvent.m_delayMillSec = delay;
			if (newEvent.m_delayMillSec > 65535)
			{
				newEvent.m_delayMillSec = 65535;
			}
			newEvents.push_back(newEvent);
			if (newEvents.size() == 48)
			{
				break;
			}
			delay = 0;
		}
	}

	// 构造Value
	unsigned char tlvData[256];
	memset(tlvData, 0, sizeof(tlvData));
	tlvData[0] = (unsigned char)keyNum;  // key num
	tlvData[1] = (unsigned char)macroCmd->m_loopCount;  // loop count
	tlvData[2] = (unsigned char)macroCmd->m_overFlag;  // over flag
	tlvData[3] = (unsigned char)newEvents.size();  // group num

	// 添加按键列表
	int offset = 4;
	for (const auto& event : newEvents)
	{
		if (event.m_type == 1)  // 键盘按键
		{			
			ST_KEY_CODE* keyCode = CMacroEventMapping::GetKeyCodeByVkCode(event.m_vkCode);
			if (keyCode == nullptr)
			{
				LOG_ERROR(L"the vkcode is wrong");
				return;
			}

			tlvData[offset] = keyCode->uc_reportID;
			if (keyCode->uc_reportID == 1)
			{
				if (event.m_vkCode == VK_BROWSER_HOME)  // 特殊处理
				{
					if (event.m_down)
					{
						tlvData[offset + 1] = keyCode->aucCode[0];
						tlvData[offset + 2] = keyCode->aucCode[1];
					}
					else
					{
						tlvData[offset + 1] = 0;
						tlvData[offset + 2] = 0;
					}
				}
				else
				{
					tlvData[offset + 1] = event.m_keyFlag;
					if (event.m_down)
					{
						tlvData[offset + 2] = keyCode->aucCode[1];
					}
					else
					{
						tlvData[offset + 2] = 0;
					}
				}
			}
			else
			{
				if (event.m_down)
				{
					tlvData[offset + 1] = keyCode->aucCode[0];
					tlvData[offset + 2] = keyCode->aucCode[1];
				}
				else
				{
					tlvData[offset + 1] = 0;
					tlvData[offset + 2] = 0;
				}
			}			
		}
		else if (event.m_type == 2)
		{
			// 鼠标按键，第一字节为02，第二字节和第三个字节是键值
			tlvData[offset] = 0x02;
			if (event.m_down)
			{
				if (event.m_mouseKey == 1) // 鼠标左键
				{
					tlvData[offset + 1] = 0x01;
					tlvData[offset + 2] = 0x00;
				}
				else if (event.m_mouseKey == 2) // 鼠标中键
				{
					tlvData[offset + 1] = 0x04;
					tlvData[offset + 2] = 0x00;
				}
				else if (event.m_mouseKey == 3) // 鼠标右键
				{
					tlvData[offset + 1] = 0x02;
					tlvData[offset + 2] = 0x00;
				}
				else
				{
					LOG_ERROR(L"the mouse key is wrong");
					return;
				}
			}
			else
			{
				// up 固定为 0x00 0x00
				tlvData[offset + 1] = 0x00;
				tlvData[offset + 2] = 0x00;
			}
		}
		else
		{
			LOG_ERROR(L"the macro event type is wrong");
			return;
		}

		// 第四和第五字节为延时，小端格式
		tlvData[offset + 3] = (unsigned char)(event.m_delayMillSec & 0xFF);
		tlvData[offset + 4] = (unsigned char)((event.m_delayMillSec >> 8) & 0xFF);

		offset += 5;
	}

	// 构造TLV
	CProtocalTLV tlv;
	tlv.m_type = 0x4C;
	tlv.m_length = (unsigned char)(offset+2);
	memcpy(tlv.m_value, tlvData, offset);

	// 构造协议包
	CProtocalPackage package;
	package.m_reportId = 0xa5;
	package.m_commandId = 0xd1;
	package.m_tlvs.push_back(tlv);

	// 发送协议包
	SendSetting(SETTING_CATEGORY_KEY, package);
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
		else
		{
			// 判断是否为宏命令
			if (text.find(KEY_NAME_MACRO) == 0)
			{
				mouseConfig.m_firstKey = KEY_INDEX_MACRO;
				mouseConfig.m_macroCmdNames[0] = text.substr(wcslen(KEY_NAME_MACRO));
			}
		}

		text = m_PaintManager.FindControl(L"secondKeyBtn")->GetText();
		index = CKeyMapping::GetKeyIndexByName(text);
		if (index != -1)
		{
			mouseConfig.m_secondKey = index;
		}
		else
		{
			// 判断是否为宏命令
			if (text.find(KEY_NAME_MACRO) == 0)
			{
				mouseConfig.m_secondKey = KEY_INDEX_MACRO;
				mouseConfig.m_macroCmdNames[1] = text.substr(wcslen(KEY_NAME_MACRO));
			}
		}

		text = m_PaintManager.FindControl(L"thirdKeyBtn")->GetText();
		index = CKeyMapping::GetKeyIndexByName(text);
		if (index != -1)
		{
			mouseConfig.m_thirdtKey = index;
		}
		else
		{
			// 判断是否为宏命令
			if (text.find(KEY_NAME_MACRO) == 0)
			{
				mouseConfig.m_thirdtKey = KEY_INDEX_MACRO;
				mouseConfig.m_macroCmdNames[2] = text.substr(wcslen(KEY_NAME_MACRO));
			}
		}

		text = m_PaintManager.FindControl(L"fouthKeyBtn")->GetText();
		index = CKeyMapping::GetKeyIndexByName(text);
		if (index != -1)
		{
			mouseConfig.m_fourthKey = index;
		}
		else
		{
			// 判断是否为宏命令
			if (text.find(KEY_NAME_MACRO) == 0)
			{
				mouseConfig.m_fourthKey = KEY_INDEX_MACRO;
				mouseConfig.m_macroCmdNames[3] = text.substr(wcslen(KEY_NAME_MACRO));
			}
		}

		text = m_PaintManager.FindControl(L"fifthKeyBtn")->GetText();
		index = CKeyMapping::GetKeyIndexByName(text);
		if (index != -1)
		{
			mouseConfig.m_fifthKey = index;
		}
		else
		{
			// 判断是否为宏命令
			if (text.find(KEY_NAME_MACRO) == 0)
			{
				mouseConfig.m_fifthKey = KEY_INDEX_MACRO;
				mouseConfig.m_macroCmdNames[4] = text.substr(wcslen(KEY_NAME_MACRO));
			}
		}

		text = m_PaintManager.FindControl(L"sixthKeyBtn")->GetText();
		index = CKeyMapping::GetKeyIndexByName(text);
		if (index != -1)
		{
			mouseConfig.m_sixthKey = index;
		}
		else
		{
			// 判断是否为宏命令
			if (text.find(KEY_NAME_MACRO) == 0)
			{
				mouseConfig.m_sixthKey = KEY_INDEX_MACRO;
				mouseConfig.m_macroCmdNames[5] = text.substr(wcslen(KEY_NAME_MACRO));
			}
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
		GetDpiSetting(mouseConfig.m_currentDpi, mouseConfig.m_dpiSetting);		
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

UINT_PTR CMainWindow::ColorDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_INITDIALOG)
	{
		// 居中显示窗口
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
	// 运行任务内部可能会操作任务池，备份一份操作比较保险
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

	// 获取时间未到
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
	// 3分钟后再更新
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
	// 任务完成，去除它
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
		if (tlv.m_type == 0x40)  // 按键配置
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
		else if (tlv.m_type == 0x41)  // 当前DPI等级
		{
			int dpiIndex = tlv.m_value[0];
			if (dpiIndex >= 1 && dpiIndex <= 6)
			{
				mouseConfig.m_currentDpi = dpiIndex - 1;
				UpdateDpiPanel();
			}
		}
		else if (tlv.m_type == 0x42)  // DPI等级参数
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
		else if (tlv.m_type == 0x44)  // 按键去抖
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
		else if (tlv.m_type == 0x45)  // 回报率参数
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
		else if (tlv.m_type == 0x46)  // 休眠时间
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
		else if (tlv.m_type == 0x47)  // Sensor高阶设置
		{
			mouseConfig.m_lineRejust = tlv.m_value[0] == 0x01 ? true : false;
			mouseConfig.m_motionSync = tlv.m_value[1] == 0x01 ? true : false;
			mouseConfig.m_rippleControl = tlv.m_value[2] == 0x01 ? true : false;
			UpdateSensorPanel();
		}
		else if (tlv.m_type == 0x48)  // 灯效设置
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
		else if (tlv.m_type == 0x49) // 版本号
		{
			std::wstring version = std::to_wstring(tlv.m_value[1]) + L'.';
			version += std::to_wstring(tlv.m_value[2]) + L'.';
			version += std::to_wstring(tlv.m_value[3]) + L'.';
			version += std::to_wstring(tlv.m_value[4]);
			m_PaintManager.FindControl(L"driverVerson")->SetText(version.c_str());
		}
		else if (tlv.m_type == 0x4c) // 宏定义
		{
			int keyNum = (int)tlv.m_value[0];
			mouseConfig.SetKey(keyNum, KEY_INDEX_MACRO);
			UpdateKeyPanel();
		}
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
	m_manualTrigger = false;
	control->Selected(check);
	m_manualTrigger = true;	
}

void CMainWindow::SendSetting(int settingCategory, const CProtocalPackage& package)
{
	if (!CMouseCommManager::GetInstance()->IsMouseConnected())
	{
		CMyMessageBox::Show(GetHWND(), L"鼠标未连接");
		UpdateControls(settingCategory);
		return;
	}

	if (!CProtocalUtil::SendPackage(package))
	{
		CMyMessageBox::Show(GetHWND(), L"发送设置给鼠标失败");
		UpdateControls(settingCategory);
		return;
	}

	// 显示等待窗口，要么等待超时结束，要么外部设置结果关闭
	m_waitingWindow = new CWaitingWindow();
	m_waitingWindow->SetCaption(L"鼠标配置中...");
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
		CMyMessageBox::Show(GetHWND(), L"设置失败");
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
	// 从界面获取DPI设置
	int currentDpi = 0;
	CMouseDpiSetting dpiSetting[6];
	GetDpiSetting(currentDpi, dpiSetting);

	// 构造协议包
	CProtocalPackage package;
	package.m_commandId = 0xd1;

	// 添加当前等级
	CProtocalTLV currentDpiTlv;
	currentDpiTlv.m_type = 0x41;
	currentDpiTlv.m_length = 0x03;
	currentDpiTlv.m_value[0] = (unsigned char)(currentDpi + 1);
	package.m_tlvs.push_back(currentDpiTlv);

	// 添加每一个等级参数
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