#pragma once

#include "SettingManager.h"

class CRecordSetting
{
public:
	// ��־�Ƿ�¼��
	bool m_record = false;

	// ��һ������������ʱ�䣬��λ����
	ULONGLONG m_lastKeyTime = 0;

	// ��־�Ƿ������ʱʱ��
	bool m_insertDelay = false;
};

class CMacroWindow : public WindowImplBase
{
public:
	CMacroWindow();
	~CMacroWindow();

public:
	UIBEGIN_MSG_MAP
		EVENT_HANDLER(DUI_MSGTYPE_CLICK, OnClickEvent)
		EVENT_HANDLER(DUI_MSGTYPE_ITEMSELECT, OnItemSelectEvent)
		EVENT_HANDLER(DUI_MSGTYPE_SELECTCHANGED, OnSelectChangedEvent)
		EVENT_ID_HANDLER(DUI_MSGTYPE_TEXTCHANGED, L"loopCountEdit", OnLoopCountTextChanged)
	UIEND_MSG_MAP

protected: //override base
	virtual void InitWindow() override;
	virtual void OnFinalMessage(HWND hWnd) override;
	virtual CDuiString GetSkinFolder() override;
	virtual CDuiString GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName(void) const override;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;	
	virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam) override;
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) override;

private:
	void OnClickEvent(TNotifyUI& msg);

	void OnItemSelectEvent(TNotifyUI& msg);

	void OnSelectChangedEvent(TNotifyUI& msg);

	void OnKey(bool down, WPARAM wParam, LPARAM lParam);

	void OnNewMacro();

	void OnDeleteMacro();

	void OnModifyMacroEvent();

	void OnDeleteMacroEvent();

	void OnRecordMacro();

	void OnInsertEvent();

	void OnMacroListSelectedChange();

	void OnLoopCountTextChanged(TNotifyUI& msg);

private:
	// ֻ����һ��
	void InitControls();

	// ���º��б�
	void UpdateMacroList(std::wstring selMacroName);

	// ѡ��ĳ��������
	void SelectMacroCmd(std::wstring macroName);

	// ��ȡ��ǰѡ��ĺ�����
	CMacroCmd* GetSelectedMacro();

	// ����һ�����¼�������״̬�ͽ��棩��pos -1��ʾβ������
	void InsertMacroEvent(const CMacroEvent& event, int pos = -1);

	// ����һ���¼��������б�ؼ�
	void InsertEventListItem(const CMacroEvent& event, int pos = -1);

	// ��ȡ�����б�ؼ�
	CListUI* GetKeyListControl();

	// ����ѭ��������־
	void SetLoopOverFlag(int flag);

private:
	// ¼�Ƶ�����
	CRecordSetting m_recordSetting;
};

