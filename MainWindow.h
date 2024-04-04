#pragma once

class CMainWindow : public WindowImplBase
{
public:
	CMainWindow();
	~CMainWindow();

public:
	UIBEGIN_MSG_MAP
		EVENT_HANDLER(DUI_MSGTYPE_CLICK, OnClickEvent)
		EVENT_ID_HANDLER(DUI_MSGTYPE_WINDOWINIT, L"root", OnWindowInit)
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

	// �����Ѿ���ɳ�ʼ�����ؼ���С���Ѿ�ȷ��
	void OnWindowInit(TNotifyUI& msg);

	// ͨ���˵�����������
	void OnMenuCommand(int commandId);

	// �ر��Ҳ����е��������
	void CloseRightSettingPannels();

	// ��ʼ�����еĿؼ���ֻ����һ��
	void InitControls();	

	// �ڴ����ؼ��Ҳ൯�������˵�
	void PopupKeyMenu(CControlUI* fromControl);

	// ����Option�ؼ�Check״̬
	void SetOption(COptionUI* control, bool check);

private:
	// ʹ�����ø��¿ؼ�
	void UpdateControls();

	void UpdateKeyPanel();
	void UpdatePowerPannel();
	void UpdateDpiPanel();
	void UpdateLightPanel();
	void UpdateHuibaoPanel();
	void UpdateLodPanel();
	void UpdateQudouPanel();
	void UpdateSensorPanel();

private:
	HMENU m_keyMenu = NULL;

	// ���������˵��İ�ť
	CControlUI* m_clickedKeyBtn = nullptr;
};