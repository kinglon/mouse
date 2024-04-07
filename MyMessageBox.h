#pragma once

class CMyMessageBox : public WindowImplBase
{
public:
	CMyMessageBox();
	~CMyMessageBox();

public:
	UIBEGIN_MSG_MAP		
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK, L"okBtn", OnOk)
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK, L"cancelBtn", OnCancel)
	UIEND_MSG_MAP

public:
	// 显示一个提示框，确定返回true，取消返回false
	static bool Show(HWND hParentWnd, std::wstring message, bool showCancel=false);

public:
	void SetText(std::wstring text) { m_text = text; }

	void SetShowCancel(bool showCancel) { m_showCancel = showCancel; }

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
	std::wstring m_text;

	bool m_showCancel = false;
};

