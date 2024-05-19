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
	void SetTitle(const std::wstring& title) { m_title = title; }

	void SetVkCode(int vkCode) { m_vkCode = vkCode; }
	int GetVkCode() { return m_vkCode; }

	void SetVkCodeState(unsigned char vkCodeState) { m_vkCodeState = vkCodeState; }
	unsigned char GetVkCodeState() { return m_vkCodeState; }

	// 获取Ctrl、Shift、Alt按键是否按下的状态
	// 第0-7bit各代表left control, left shift, left alt, left win, right control, right shift, right alt, right win, 1 down, 0 up
	static unsigned char GetSpecialKeyState();

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
	void OnKey(bool down, WPARAM wParam, LPARAM lParam);

	void OnOk(TNotifyUI& msg);
	void OnCancel(TNotifyUI& msg);

private:
	std::wstring m_title;

	int m_vkCode = 0;

	unsigned char m_vkCodeState = 0;
};

