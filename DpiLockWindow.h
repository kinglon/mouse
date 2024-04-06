#pragma once

class CDpiLockWindow : public WindowImplBase
{
public:
	CDpiLockWindow();
	~CDpiLockWindow();

public:
	UIBEGIN_MSG_MAP		
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK, L"okBtn", OnOk)
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK, L"cancelBtn", OnCancel)
	UIEND_MSG_MAP

public:
	int GetDpiValue() { return m_dpiValue; }

protected: //override base
	virtual void InitWindow() override;
	virtual void OnFinalMessage(HWND hWnd) override;
	virtual CDuiString GetSkinFolder() override;
	virtual CDuiString GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName(void) const override;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

private:
	void OnOk(TNotifyUI& msg);
	void OnCancel(TNotifyUI& msg);

private:
	int m_dpiValue = 200;
};

