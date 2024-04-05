#pragma once

class CWaitingWindow : public WindowImplBase
{
public:
	CWaitingWindow();
	~CWaitingWindow();

public:
	UIBEGIN_MSG_MAP		
	UIEND_MSG_MAP

public:
	// 查询是否成功
	bool IsSuccess() { return m_success; }

	// 设置结果
	void SetSuccess(bool success) { m_success = success; }

	// 设置等待秒数
	void SetWaitSec(int seconds) { m_waitSeconds = seconds; }

protected: //override base
	virtual void InitWindow() override;
	virtual void OnFinalMessage(HWND hWnd) override;
	virtual CDuiString GetSkinFolder() override;
	virtual CDuiString GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName(void) const override;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

private:
	bool m_success = false;

	int m_waitSeconds = 3;
};

