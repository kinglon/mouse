#pragma once

#include <vector>

class CGetOptionWindow : public WindowImplBase
{
public:
	CGetOptionWindow();
	~CGetOptionWindow();

public:
	UIBEGIN_MSG_MAP		
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK, L"okBtn", OnOk)
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK, L"cancelBtn", OnCancel)
	UIEND_MSG_MAP

public:
	// 设置标题
	void SetTitle(std::wstring title) { m_title = title; }

	// 设置选项列表
	void SetOptions(const std::vector<std::wstring>& options) { m_options = options; }
	
	// 获取选择的选项
	std::wstring GetSelectedOption() { return m_selectedOption; }

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
	std::wstring m_title;

	std::vector<std::wstring> m_options;

	std::wstring m_selectedOption;
};

