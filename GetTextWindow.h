#pragma once

class CGetTextWindow : public WindowImplBase
{
public:
	CGetTextWindow();
	~CGetTextWindow();

public:
	UIBEGIN_MSG_MAP		
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK, L"okBtn", OnOk)
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK, L"cancelBtn", OnCancel)
	UIEND_MSG_MAP

public:
	// 设置标题
	void SetTitle(std::wstring title) { m_title = title; }

	// 获取输入内容
	std::wstring GetInputContent() { return m_inputContent; }

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
	std::wstring m_inputContent;

	std::wstring m_title;
};

