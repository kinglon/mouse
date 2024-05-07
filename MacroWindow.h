#pragma once

#include "SettingManager.h"

class CRecordSetting
{
public:
	// 标志是否录制
	bool m_record = false;

	// 上一个按键触发的时间，单位毫秒
	ULONGLONG m_lastKeyTime = 0;

	// 标志是否插入延时时间
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
	// 只调用一次
	void InitControls();

	// 更新宏列表
	void UpdateMacroList(std::wstring selMacroName);

	// 选择某个宏命令
	void SelectMacroCmd(std::wstring macroName);

	// 获取当前选择的宏命令
	CMacroCmd* GetSelectedMacro();

	// 插入一个宏事件（更新状态和界面），pos -1表示尾部插入
	void InsertMacroEvent(const CMacroEvent& event, int pos = -1);

	// 插入一个事件到按键列表控件
	void InsertEventListItem(const CMacroEvent& event, int pos = -1);

	// 获取按键列表控件
	CListUI* GetKeyListControl();

	// 设置循环结束标志
	void SetLoopOverFlag(int flag);

private:
	// 录制的设置
	CRecordSetting m_recordSetting;
};

