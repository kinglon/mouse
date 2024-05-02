#pragma once

class CGetVkCodeWindow : public WindowImplBase
{
public:
	CGetVkCodeWindow();
	~CGetVkCodeWindow();

public:
	UIBEGIN_MSG_MAP		
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK, L"okBtn", OnOk)
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK, L"cancelBtn", OnCancel)
	UIEND_MSG_MAP

public:
	// 获取输入内容
	int GetVkCode() { return m_vkCode; }

protected: //override base
	virtual void InitWindow() override;
	virtual void OnFinalMessage(HWND hWnd) override;
	virtual CDuiString GetSkinFolder() override;
	virtual CDuiString GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName(void) const override;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

private:
	void OnKey(bool down, WPARAM wParam, LPARAM lParam);

	void OnOk(TNotifyUI& msg);
	void OnCancel(TNotifyUI& msg);

private:
	int m_vkCode = 0;
};

