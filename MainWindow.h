#pragma once

class CMainWindow : public WindowImplBase
{
public:
	CMainWindow();
	~CMainWindow();

public:
	UIBEGIN_MSG_MAP
		EVENT_HANDLER(DUI_MSGTYPE_CLICK, OnClickEvent)
	UIEND_MSG_MAP

protected: //override base
	virtual void InitWindow() override;
	virtual void OnFinalMessage(HWND hWnd) override;
	virtual CDuiString GetSkinFolder() override;
	virtual CDuiString GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName(void) const override;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;	

private:
	void OnClickEvent(TNotifyUI& msg);

	// 关闭右侧所有的设置面板
	void CloseRightSettingPannels();

	// 初始化所有的控件，只调用一次
	void InitControls();
};