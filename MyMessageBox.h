#pragma once

class CMyMessageBox : public WindowImplBase
{
public:
	CMyMessageBox();
	~CMyMessageBox();

public:
	UIBEGIN_MSG_MAP		
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK, L"okBtn", OnOk)
	UIEND_MSG_MAP

public:
	void SetText(std::wstring text) { m_text = text; }

protected: //override base
	virtual void InitWindow() override;
	virtual void OnFinalMessage(HWND hWnd) override;
	virtual CDuiString GetSkinFolder() override;
	virtual CDuiString GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName(void) const override;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

private:
	void OnOk(TNotifyUI& msg);

private:
	std::wstring m_text;
};

