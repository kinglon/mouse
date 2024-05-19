#include "stdafx.h"
#include "MacroWindow.h"
#include "ResourceUtil.h"
#include "SettingManager.h"
#include "MacroEventMapping.h"
#include "GetTextWindow.h"
#include "MyMessageBox.h"
#include "GetVkCodeWindow.h"

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

	m_PaintManager.AddMessageFilter(this);

	InitControls();
}

void CMacroWindow::OnFinalMessage(HWND hWnd)
{
	m_PaintManager.RemoveMessageFilter(this);

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
	if (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN)
	{
		OnKey(true, wParam, lParam);
		return 0L;
	}

	if (uMsg == WM_KEYUP || uMsg == WM_SYSKEYUP)
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

LRESULT CMacroWindow::ResponseDefaultKeyEvent(WPARAM wParam)
{
	if (wParam == VK_ESCAPE)
	{
		return FALSE;
	}

	return __super::ResponseDefaultKeyEvent(wParam);
}

LRESULT CMacroWindow::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	bHandled = false;
	if (uMsg == WM_KEYDOWN && wParam == VK_TAB)
	{
		OnKey(true, wParam, lParam);
		bHandled = true;
		return 0L;
	}

	if (uMsg == WM_KEYUP && wParam == VK_TAB)
	{
		OnKey(false, wParam, lParam);
		bHandled = true;
		return 0L;
	}

	return __super::MessageHandler(uMsg, wParam, lParam, bHandled);
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

	// �ж��Ƿ�֧�ָð���
	int vkCode = (int)wParam;
	if (CMacroEventMapping::GetKeyName(vkCode).empty())
	{
		return;
	}

	// �ж��Ƿ���������
	CMacroCmd* currentMacro = GetSelectedMacro();
	if (currentMacro == nullptr)
	{
		return;
	}

	if (currentMacro->GetEventCount() + 1 > MAX_EVENT_LIST_SIZE)
	{
		CMyMessageBox::Show(GetHWND(), L"�������ѳ����������");
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
	keyEvent.m_vkCode = vkCode;
	keyEvent.m_keyFlag = CGetVkCodeWindow::GetSpecialKeyState();
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
		getVkCodeWindow.SetTitle(L"�޸İ���");
		getVkCodeWindow.SetVkCode(event.m_vkCode);
		getVkCodeWindow.SetVkCodeState(event.m_keyFlag);
		getVkCodeWindow.Create(GetHWND(), NULL, WS_VISIBLE | WS_POPUP, 0);
		getVkCodeWindow.CenterWindow();
		if (getVkCodeWindow.ShowModal() == 0)
		{
			return;
		}

		event.m_vkCode = getVkCodeWindow.GetVkCode();
		event.m_keyFlag = getVkCodeWindow.GetVkCodeState();
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
		// ��հ����б�
		GetKeyListControl()->RemoveAll();
		CMacroCmd* currentMacro = GetSelectedMacro();
		if (currentMacro)
		{
			currentMacro->m_events.clear();
		}

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
	insertEventCombo->SelectItem(-1);

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

	// ��ѡ���λ�ú������
	int insertPos = GetKeyListControl()->GetCurSel();
	if (insertPos == -1)
	{
		if (GetKeyListControl()->GetCount() == 0)
		{
			insertPos = 0;
		}
		else
		{
			CMyMessageBox::Show(GetHWND(), L"ѡ�����λ��");
			return;
		}
	}
	else
	{
		insertPos += 1;
	}

	// ��������¼�
	if (curSel >= INSERT_EVENT_INDEX_LEFT_MOUSE && curSel <= INSERT_EVENT_INDEX_MIDDLE_MOUSE)
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
		insertPos += 1;

		// �ӳ�50���룬������ʱ�¼�
		bool insertDelay = ((COptionUI*)m_PaintManager.FindControl(L"insertDelayOption"))->IsSelected();
		if (insertDelay)
		{
			CMacroEvent delayEvent;
			delayEvent.m_type = 3;
			delayEvent.m_delayMillSec = 50;
			InsertMacroEvent(delayEvent, insertPos);
			insertPos += 1;
		}

		// ����up�¼�
		downEvent.m_down = false;
		InsertMacroEvent(downEvent, insertPos);
	}
	else if (curSel == INSERT_EVENT_INDEX_KEYPAD)
	{
		// ���밴���¼����Ȼ�ȡһ������
		CGetVkCodeWindow getVkCodeWindow;
		getVkCodeWindow.SetTitle(L"���밴��");
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
		downEvent.m_keyFlag = getVkCodeWindow.GetVkCodeState();
		InsertMacroEvent(downEvent, insertPos);
		insertPos += 1;

		// �ӳ�50���룬������ʱ�¼�
		bool insertDelay = ((COptionUI*)m_PaintManager.FindControl(L"insertDelayOption"))->IsSelected();
		if (insertDelay)
		{
			CMacroEvent delayEvent;
			delayEvent.m_type = 3;
			delayEvent.m_delayMillSec = 50;
			InsertMacroEvent(delayEvent, insertPos);
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
		labelElementUI->SetFixedHeight(25);
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
	CDialogBuilder builder;
	CControlUI* eventItemUI = builder.Create(L"macro_event_item.xml");
	CControlUI* iconControl = m_PaintManager.FindSubControlByName(eventItemUI, L"icon");
	CLabelUI* contentCtrl = (CLabelUI*)m_PaintManager.FindSubControlByName(eventItemUI, L"content");

	// ��ʱ�¼�
	if (event.m_type == 3)
	{
		iconControl->SetBkImage(L"file='macro_time.png'");
		contentCtrl->SetText((std::to_wstring(event.m_delayMillSec) + L"ms").c_str());
	}
	else
	{		
		if (event.m_down)
		{
			iconControl->SetBkImage(L"file='macro_down.png'");
		}
		else
		{
			iconControl->SetBkImage(L"file='macro_up.png'");
		}		
		contentCtrl->SetText(CMacroEventMapping::GetEventName(event).c_str());		
	}

	if (pos < 0)
	{
		GetKeyListControl()->Add(eventItemUI);
	}
	else
	{
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