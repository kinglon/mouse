#include "stdafx.h"
#include "MacroWindow.h"
#include "ResourceUtil.h"
#include "SettingManager.h"
#include "MacroEventMapping.h"
#include "GetTextWindow.h"
#include "MyMessageBox.h"
#include "GetVkCodeWindow.h"

// ����¼���С
#define MAX_EVENT_LIST_SIZE  48

// selectitem�¼��������ڲ���ShowModal()�������첽����һ������Ϣ
#define WM_SELECT_ITEM			WM_USER+101

// �����¼�����
#define INSERT_EVENT_INDEX_LEFT_MOUSE	0
#define INSERT_EVENT_INDEX_RIGHT_MOUSE	1
#define INSERT_EVENT_INDEX_MIDDLE_MOUSE	2
#define INSERT_EVENT_INDEX_DELAY		3
#define INSERT_EVENT_INDEX_KEYPAD		4


CMacroWindow::CMacroWindow()
{
}


CMacroWindow::~CMacroWindow()
{
}


void CMacroWindow::InitWindow()
{
	__super::InitWindow();	

	InitControls();
}

void CMacroWindow::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
}

CDuiString CMacroWindow::GetSkinFolder()
{
	return CResourceUtil::GetSkinFolder().c_str();
}

CDuiString CMacroWindow::GetSkinFile()
{
	return L"macro_window.xml";
}

LPCTSTR CMacroWindow::GetWindowClassName(void) const
{
	return L"MouseMacroWindow";
}

LRESULT CMacroWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_KEYDOWN)
	{
		OnKey(true, wParam, lParam);
		return 0L;
	}

	if (uMsg == WM_KEYUP)
	{
		OnKey(false, wParam, lParam);
		return 0L;
	}

	if (uMsg == WM_SELECT_ITEM)
	{
		CControlUI* control = (CControlUI*)lParam;
		if (control->GetName() == L"insertEventCombo")
		{
			OnInsertEvent();
		}
		else if (control->GetName() == L"macroList")
		{
			OnMacroListSelectedChange();
		}

		return 0L;
	}

	return __super::HandleMessage(uMsg, wParam, lParam);
}

void CMacroWindow::OnClickEvent(TNotifyUI& msg)
{
	auto& controlName = msg.pSender->GetName();
	if (controlName == L"closeBtn")
	{
		CSettingManager::GetInstance()->SaveMacroCmdConfig();
		Close();
	}
	else if (controlName == L"newMacroBtn")
	{
		OnNewMacro();
	}
	else if (controlName == L"deleteMacroBtn")
	{
		OnDeleteMacro();
	}
	else if (controlName == L"modifyEventBtn")
	{
		OnModifyMacroEvent();
	}
	else if (controlName == L"deleteEventBtn")
	{
		OnDeleteMacroEvent();
	}
	else if (controlName == L"recordBtn")
	{
		OnRecordMacro();
	}
}

void CMacroWindow::OnItemSelectEvent(TNotifyUI& msg)
{
	PostMessage(WM_SELECT_ITEM, 0, (LPARAM)msg.pSender);
}

void CMacroWindow::OnSelectChangedEvent(TNotifyUI& msg)
{
	COptionUI* control = (COptionUI*)msg.pSender;
	auto& controlName = msg.pSender->GetName();
	if (controlName == L"releaseKeyRadio")
	{
		if (control->IsSelected())
		{
			SetLoopOverFlag(1);
		}
	}
	else if (controlName == L"pressKeyRadio")
	{
		if (control->IsSelected())
		{
			SetLoopOverFlag(2);
		}
	}
	else if (controlName == L"loopCountRadio")
	{
		if (control->IsSelected())
		{
			SetLoopOverFlag(0);
		}
	}
}

void CMacroWindow::OnKey(bool down, WPARAM wParam, LPARAM lParam)
{
	// δ¼�ƣ�������
	if (!m_recordSetting.m_record)
	{
		return;
	}

	// ����ֻ�账��һ��
	DWORD previousState = ((lParam >> 30) & 0x01);
	if (down && previousState == 1)
	{
		return;
	}	

	// �����ʱ�¼�
	int delay = 0;
	if (m_recordSetting.m_insertDelay && m_recordSetting.m_lastKeyTime > 0)
	{
		delay = int(GetTickCount64() - m_recordSetting.m_lastKeyTime);
	}
	m_recordSetting.m_lastKeyTime = GetTickCount64();
	if (delay > 0)
	{
		CMacroEvent delayEvent;
		delayEvent.m_type = 3;
		delayEvent.m_delayMillSec = delay;
		InsertMacroEvent(delayEvent);
	}

	// ��Ӱ����¼�
	CMacroEvent keyEvent;
	keyEvent.m_type = 1;
	keyEvent.m_down = down;	
	keyEvent.m_vkCode = (int)wParam;

	// ��ȡCtrl��Shift��Alt�����Ƿ��µ�״̬
	// ��0-7bit������left control, left shift, left alt, left win, right control, right shift, right alt, right win, 1 down, 0 up
	unsigned char keyState = 0;
	SHORT state = GetKeyState(VK_LCONTROL);  // ���λ1��ʾdonw, 0��ʾup
	keyState |= ((state >> 15) & 0X01);
	state = GetKeyState(VK_LSHIFT);
	keyState |= ((state >> 14) & 0X02);
	state = GetKeyState(VK_LMENU);
	keyState |= ((state >> 13) & 0X04);
	state = GetKeyState(VK_LWIN);
	keyState |= ((state >> 12) & 0X08);
	state = GetKeyState(VK_RCONTROL);
	keyState |= ((state >> 11) & 0X10);
	state = GetKeyState(VK_RSHIFT);
	keyState |= ((state >> 10) & 0X20);
	state = GetKeyState(VK_RMENU);
	keyState |= ((state >> 9) & 0X40);
	state = GetKeyState(VK_RWIN);
	keyState |= ((state >> 8) & 0X80);
	keyEvent.m_keyFlag = keyState;

	InsertMacroEvent(keyEvent);
}

void CMacroWindow::OnNewMacro()
{
	// ��ȡ������
	CGetTextWindow getTextWindow;
	getTextWindow.SetTitle(L"������Ҫ�½��ĺ�����");
	getTextWindow.Create(GetHWND(), NULL, WS_VISIBLE | WS_POPUP, 0);
	getTextWindow.CenterWindow();
	if (getTextWindow.ShowModal() == 0)
	{
		return;
	}

	// У��������Ƿ��Ѿ�����
	auto& macroConfig = CSettingManager::GetInstance()->m_macroCmdConfig;
	for (const auto& macro : macroConfig)
	{
		if (macro.m_name == getTextWindow.GetInputContent())
		{
			CMyMessageBox::Show(GetHWND(), L"�������Ѿ�����");
			return;
		}
	}

	// �½�һ����
	CMacroCmd macroCmd;
	macroCmd.m_name = getTextWindow.GetInputContent();
	macroConfig.push_back(macroCmd);
	UpdateMacroList(macroCmd.m_name);

	// ѡ��ú�
	SelectMacroCmd(macroCmd.m_name);
}

void CMacroWindow::OnDeleteMacro()
{
	CListUI* macroList = (CListUI*)m_PaintManager.FindControl(L"macroList");
	int selIndex = macroList->GetCurSel();
	if (selIndex < 0)
	{
		CMyMessageBox::Show(GetHWND(), L"��ѡ��һ����");
		return;
	}

	auto& macroConfig = CSettingManager::GetInstance()->m_macroCmdConfig;
	if (selIndex >= (int)macroConfig.size())
	{
		return;
	}
	
	macroConfig.erase(macroConfig.begin() + selIndex);
	std::wstring selMacro;
	if (macroConfig.size() > 0)
	{
		selMacro = macroConfig[0].m_name;
	}
	UpdateMacroList(selMacro);
	SelectMacroCmd(selMacro);
}

void CMacroWindow::OnModifyMacroEvent()
{
	CListUI* keyList = GetKeyListControl();
	int selIndex = keyList->GetCurSel();
	if (selIndex < 0)
	{
		CMyMessageBox::Show(GetHWND(), L"��ѡ��һ������");
		return;
	}	

	CMacroCmd* macro = GetSelectedMacro();
	if (macro == nullptr)
	{
		return;
	}

	if (selIndex < 0 || selIndex >= (int)macro->m_events.size())
	{
		return;
	}
	
	auto& event = macro->m_events[selIndex];
	if (event.m_type == 3)
	{
		// �������ʱ������Ϊ����ʱ
		CGetTextWindow getTextWindow;
		getTextWindow.SetTitle(L"��������ʱֵ");
		getTextWindow.Create(GetHWND(), NULL, WS_VISIBLE | WS_POPUP, 0);
		getTextWindow.CenterWindow();
		if (getTextWindow.ShowModal() == 0)
		{
			return;
		}

		int delayTime = _wtoi(getTextWindow.GetInputContent().c_str());
		if (delayTime < 0 || delayTime >= 65535)
		{
			CMyMessageBox::Show(GetHWND(), L"��ʱֵֻ����0-65535");
			return;
		}

		event.m_delayMillSec = delayTime;		
		CControlUI* contentCtrl = m_PaintManager.FindSubControlByName(keyList->GetItemAt(selIndex), L"content");
		contentCtrl->SetText((std::to_wstring(event.m_delayMillSec) + L"ms").c_str());
	}
	else if (event.m_type == 2)
	{
		// ��갴����֧���޸�
		CMyMessageBox::Show(GetHWND(), L"��갴����֧���޸ģ���ɾ���ٲ���");
		return;
	}
	else if (event.m_type == 1)
	{
		// �޸İ����¼����Ȼ�ȡһ������
		CGetVkCodeWindow getVkCodeWindow;
		getVkCodeWindow.Create(GetHWND(), NULL, WS_VISIBLE | WS_POPUP, 0);
		getVkCodeWindow.CenterWindow();
		if (getVkCodeWindow.ShowModal() == 0)
		{
			return;
		}

		event.m_vkCode = getVkCodeWindow.GetVkCode();
		CControlUI* contentCtrl = m_PaintManager.FindSubControlByName(keyList->GetItemAt(selIndex), L"content");
		contentCtrl->SetText(CMacroEventMapping::GetEventName(event).c_str());
	}	
}

void CMacroWindow::OnDeleteMacroEvent()
{
	CListUI* keyList = GetKeyListControl();
	int selIndex = keyList->GetCurSel();
	if (selIndex < 0)
	{
		CMyMessageBox::Show(GetHWND(), L"��ѡ��һ������");
		return;
	}	

	CMacroCmd* macro = GetSelectedMacro();
	if (macro == nullptr)
	{
		return;
	}
	
	if (selIndex < 0 || selIndex >= (int)macro->m_events.size())
	{
		return;
	}

	macro->m_events.erase(macro->m_events.begin() + selIndex);
	keyList->RemoveAt(selIndex);	
}

void CMacroWindow::OnRecordMacro()
{
	if (m_recordSetting.m_record)
	{
		// ����¼�ƣ��л���¼��
		m_PaintManager.FindControl(L"recordBtn")->SetText(L"��ʼ¼�ƺ�");
		m_recordSetting.m_record = false;
	}
	else
	{
		// ��¼�ƣ��л�Ϊ����¼��
		m_PaintManager.FindControl(L"recordBtn")->SetText(L"ֹͣ¼�ƺ�");
		m_recordSetting.m_record = true;
		m_recordSetting.m_insertDelay = ((COptionUI*)m_PaintManager.FindControl(L"insertDelayOption"))->IsSelected();
		m_recordSetting.m_lastKeyTime = 0;
	}
}

void CMacroWindow::OnInsertEvent()
{
	CComboUI* insertEventCombo = (CComboUI*)m_PaintManager.FindControl(L"insertEventCombo");
	int curSel = insertEventCombo->GetCurSel();
	if (curSel < 0)
	{
		return;
	}

	// ����Ҫ��ʾѡ�������
	insertEventCombo->SetText(L"");

	// �������ʱ�¼���У���¼������Ƿ񳬹�
	CMacroCmd* macroCmd = GetSelectedMacro();
	if (macroCmd == nullptr)
	{
		return;
	}

	if (curSel != INSERT_EVENT_INDEX_DELAY && macroCmd->GetEventCount()+2 > MAX_EVENT_LIST_SIZE) // 2 is down and up
	{
		CMyMessageBox::Show(GetHWND(), L"�������ѳ����������");
		return;
	}

	// ��ȡ����λ��
	int insertPos = GetKeyListControl()->GetCurSel();
	
	// ��������¼�
	if (curSel >= INSERT_EVENT_INDEX_LEFT_MOUSE && curSel <= INSERT_EVENT_INDEX_RIGHT_MOUSE)
	{
		// ����down�¼�
		CMacroEvent downEvent;
		downEvent.m_type = 2;
		downEvent.m_down = true;
		if (curSel == INSERT_EVENT_INDEX_LEFT_MOUSE)
		{
			downEvent.m_mouseKey = 1;
		}
		else if (curSel == INSERT_EVENT_INDEX_RIGHT_MOUSE)
		{
			downEvent.m_mouseKey = 3;
		}
		else if (curSel == INSERT_EVENT_INDEX_MIDDLE_MOUSE)
		{
			downEvent.m_mouseKey = 2;
		}
		InsertMacroEvent(downEvent, insertPos);
		if (insertPos != -1)
		{
			insertPos += 1;
		}

		// �ӳ�50���룬������ʱ�¼�
		CMacroEvent delayEvent;
		delayEvent.m_type = 3;
		delayEvent.m_delayMillSec = 50;
		InsertMacroEvent(delayEvent, insertPos);
		if (insertPos != -1)
		{
			insertPos += 1;
		}

		// ����up�¼�
		downEvent.m_down = false;
		InsertMacroEvent(delayEvent, insertPos);
	}
	else if (curSel == INSERT_EVENT_INDEX_KEYPAD)
	{
		// ���밴���¼����Ȼ�ȡһ������
		CGetVkCodeWindow getVkCodeWindow;
		getVkCodeWindow.Create(GetHWND(), NULL, WS_VISIBLE | WS_POPUP, 0);
		getVkCodeWindow.CenterWindow();
		if (getVkCodeWindow.ShowModal() == 0)
		{
			return;
		}

		// ����down�¼�
		CMacroEvent downEvent;
		downEvent.m_type = 1;
		downEvent.m_down = true;
		downEvent.m_vkCode = getVkCodeWindow.GetVkCode();
		InsertMacroEvent(downEvent, insertPos);
		if (insertPos != -1)
		{
			insertPos += 1;
		}

		// �ӳ�50���룬������ʱ�¼�
		CMacroEvent delayEvent;
		delayEvent.m_type = 3;
		delayEvent.m_delayMillSec = 50;
		InsertMacroEvent(delayEvent, insertPos);
		if (insertPos != -1)
		{
			insertPos += 1;
		}		

		// �ӳ�50���룬����up�¼�
		downEvent.m_down = false;
		InsertMacroEvent(downEvent, insertPos);
	}
	else if (curSel == INSERT_EVENT_INDEX_DELAY)
	{
		// ������ʱ�¼�
		CGetTextWindow getTextWindow;
		getTextWindow.SetTitle(L"��������ʱֵ");
		getTextWindow.Create(GetHWND(), NULL, WS_VISIBLE | WS_POPUP, 0);
		getTextWindow.CenterWindow();
		if (getTextWindow.ShowModal() == 0)
		{
			return;
		}

		int delayTime = _wtoi(getTextWindow.GetInputContent().c_str());
		if (delayTime < 0 || delayTime >= 65535)
		{
			CMyMessageBox::Show(GetHWND(), L"��ʱֵֻ����0-65535");
			return;
		}

		CMacroEvent delayEvent;
		delayEvent.m_type = 3;
		delayEvent.m_delayMillSec = delayTime;
		InsertMacroEvent(delayEvent, insertPos);
	}
}

void CMacroWindow::OnLoopCountTextChanged(TNotifyUI& msg)
{
	std::wstring text = msg.pSender->GetText().GetData();
	int value = _wtoi(text.c_str());
	if (value > 0)
	{
		CMacroCmd* selMacroCmd = GetSelectedMacro();
		if (selMacroCmd)
		{
			selMacroCmd->m_loopCount = value;
		}
	}
}

void CMacroWindow::OnMacroListSelectedChange()
{
	CListUI* macroList = (CListUI*)m_PaintManager.FindControl(L"macroList");
	int selIndex = macroList->GetCurSel();
	if (selIndex >= 0 && selIndex < (int)CSettingManager::GetInstance()->m_macroCmdConfig.size())
	{
		SelectMacroCmd(CSettingManager::GetInstance()->m_macroCmdConfig[selIndex].m_name);
	}
}

void CMacroWindow::InitControls()
{
	// ��ʼ���¼��б�
	CComboUI* insertEventCombo = (CComboUI*)m_PaintManager.FindControl(L"insertEventCombo");
	insertEventCombo->SetMouseWheelEnable(false);
	std::wstring eventStrs[] = {L"���", L"�Ҽ�", L"�м�", L"��ʱ", L"���̿���"};
	insertEventCombo->RemoveAll();
	for (int i = 0; i < ARRAYSIZE(eventStrs); i++)
	{
		CListLabelElementUI* pControl = new CListLabelElementUI();
		pControl->SetText(eventStrs[i].c_str());
		insertEventCombo->Add(pControl);
	}

	// ��ʼ���������б�
	const auto& macroCmdConfig = CSettingManager::GetInstance()->m_macroCmdConfig;
	if (macroCmdConfig.size() > 0)
	{
		UpdateMacroList(macroCmdConfig[0].m_name);
		SelectMacroCmd(macroCmdConfig[0].m_name);
	}
}

void CMacroWindow::UpdateMacroList(std::wstring selMacroName)
{
	CListUI* macroList = (CListUI*)m_PaintManager.FindControl(L"macroList");
	macroList->RemoveAll();
	const auto& macroCmdConfig = CSettingManager::GetInstance()->m_macroCmdConfig;
	int selIndex = -1;
	for (unsigned int i = 0; i < macroCmdConfig.size(); i++)
	{
		CListLabelElementUI* labelElementUI = new CListLabelElementUI();
		labelElementUI->SetText(macroCmdConfig[i].m_name.c_str());
		labelElementUI->SetAttribute(L"padding", L"2,2,2,2");
		macroList->Add(labelElementUI);
		if (selMacroName == macroCmdConfig[i].m_name)
		{
			selIndex = i;
		}
	}

	if (selIndex >= 0)
	{
		macroList->SelectItem(selIndex);
	}
}

void CMacroWindow::SelectMacroCmd(std::wstring macroName)
{
	// �����¿ؼ�
	CListUI* keyList = (CListUI*)m_PaintManager.FindControl(L"keyList");
	keyList->RemoveAll();

	// ��ȡ����ϸ����
	const auto& macroCmdConfig = CSettingManager::GetInstance()->m_macroCmdConfig;
	const CMacroCmd* selMacro = nullptr;
	for (const auto& macroCmd : macroCmdConfig)
	{
		if (macroCmd.m_name == macroName)
		{
			selMacro = &macroCmd;
			break;
		}
	}
	
	if (selMacro == nullptr)
	{
		return;
	}	

	// ��ʼ�������б�	
	for (unsigned int i = 0; i<selMacro->m_events.size(); i++)
	{
		const auto& event = selMacro->m_events[i];
		InsertEventListItem(event);
	}

	// ��ʼ��ѭ��������־
	if (selMacro->m_overFlag == 0)
	{
		COptionUI* optionUI = (COptionUI*)m_PaintManager.FindControl(L"loopCountRadio");
		optionUI->Selected(true);
		CEditUI* editUI = (CEditUI*)m_PaintManager.FindControl(L"loopCountEdit");
		editUI->SetText(std::to_wstring(selMacro->m_loopCount).c_str());
	}
	else if (selMacro->m_overFlag == 1)
	{
		COptionUI* optionUI = (COptionUI*)m_PaintManager.FindControl(L"releaseKeyRadio");
		optionUI->Selected(true);
	}
	else if (selMacro->m_overFlag == 2)
	{
		COptionUI* optionUI = (COptionUI*)m_PaintManager.FindControl(L"pressKeyRadio");
		optionUI->Selected(true);
	}
}

CMacroCmd* CMacroWindow::GetSelectedMacro()
{
	CListUI* macroList = (CListUI*)m_PaintManager.FindControl(L"macroList");
	int selIndex = macroList->GetCurSel();
	if (selIndex < 0)
	{
		return nullptr;
	}

	auto& macroConfigs = CSettingManager::GetInstance()->m_macroCmdConfig;
	if (selIndex >= (int)macroConfigs.size())
	{
		return nullptr;
	}

	return &macroConfigs[selIndex];
}

void CMacroWindow::InsertMacroEvent(const CMacroEvent& event, int pos)
{
	CMacroCmd* currentMacro = GetSelectedMacro();
	if (currentMacro == nullptr)
	{
		return;
	}

	if (pos == -1)
	{
		currentMacro->m_events.push_back(event);
	}
	else
	{
		currentMacro->m_events.insert(currentMacro->m_events.begin() + pos, event);
	}

	InsertEventListItem(event, pos);
}

void CMacroWindow::InsertEventListItem(const CMacroEvent& event, int pos)
{
	// ��ʱ�¼�
	if (event.m_type == 3)
	{
		CDialogBuilder builder;
		CControlUI* eventItemUI = builder.Create(L"macro_event_item.xml");

		// ����ͼ��
		CControlUI* iconControl = m_PaintManager.FindSubControlByName(eventItemUI, L"icon");
		iconControl->SetBkImage(L"file='macro_time.png'");

		// �����ı�
		CLabelUI* contentCtrl = (CLabelUI*)m_PaintManager.FindSubControlByName(eventItemUI, L"content");
		contentCtrl->SetText((std::to_wstring(event.m_delayMillSec) + L"ms").c_str());		

		GetKeyListControl()->AddAt(eventItemUI, pos);
	}
	else
	{
		// ���밴����Ϣ
		CDialogBuilder builder;
		CControlUI* eventItemUI = builder.Create(L"macro_event_item.xml");

		// ����ͼ��
		CControlUI* iconControl = m_PaintManager.FindSubControlByName(eventItemUI, L"icon");
		if (event.m_down)
		{
			iconControl->SetBkImage(L"file='macro_down.png'");
		}
		else
		{
			iconControl->SetBkImage(L"file='macro_up.png'");
		}

		// �����ı�
		CLabelUI* contentCtrl = (CLabelUI*)m_PaintManager.FindSubControlByName(eventItemUI, L"content");
		contentCtrl->SetText(CMacroEventMapping::GetEventName(event).c_str());		

		GetKeyListControl()->AddAt(eventItemUI, pos);
	}
}

CListUI* CMacroWindow::GetKeyListControl()
{
	return (CListUI*)m_PaintManager.FindControl(L"keyList");
}

void CMacroWindow::SetLoopOverFlag(int flag)
{
	CMacroCmd* currentMacroCmd = GetSelectedMacro();
	if (currentMacroCmd)
	{
		currentMacroCmd->m_overFlag = flag;
	}
}