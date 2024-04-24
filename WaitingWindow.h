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
	// ��ѯ�Ƿ�ɹ�
	bool IsSuccess() { return m_success; }

	// ���ý��
	void SetSuccess(bool success) { m_success = success; }

	// ���õȴ�����
	void SetWaitSec(int seconds) { m_waitMillSeconds = seconds*1000; }

	// ���ñ���
	void SetCaption(std::wstring caption) { m_caption = caption; }

protected: //override base
	virtual void InitWindow() override;
	virtual void OnFinalMessage(HWND hWnd) override;
	virtual CDuiString GetSkinFolder() override;
	virtual CDuiString GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName(void) const override;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

private:
	bool m_success = false;

	int m_waitMillSeconds = 3000;

	std::wstring m_caption;
};

