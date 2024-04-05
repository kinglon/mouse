#pragma once

#include "ProtocalUtil.h"
#include "MouseCommManager.h"
#include "WaitingWindow.h"

class CMainWindow : public WindowImplBase, public IMouseCommCallback
{
public:
	CMainWindow();
	~CMainWindow();

public:
	UIBEGIN_MSG_MAP
		EVENT_HANDLER(DUI_MSGTYPE_CLICK, OnClickEvent)
		EVENT_HANDLER(DUI_MSGTYPE_ITEMSELECT, OnItemSelectEvent)
		EVENT_ID_HANDLER(DUI_MSGTYPE_WINDOWINIT, L"root", OnWindowInit)
	UIEND_MSG_MAP

protected: //override base
	virtual void InitWindow() override;
	virtual void OnFinalMessage(HWND hWnd) override;
	virtual CDuiString GetSkinFolder() override;
	virtual CDuiString GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName(void) const override;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;	

protected: // implement IMouseCommCallback
	virtual void RecvDataCallback(unsigned char* data, int dataLength) override;

private:
	void OnClickEvent(TNotifyUI& msg);
	void OnItemSelectEvent(TNotifyUI& msg);
	void OnWindowInit(TNotifyUI& msg);

	// ���յ���������
	void OnMouseDataArrive(const unsigned char* data, int dataLength);

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

	// �������ø����
	void SendSetting(int settingCategory, const CProtocalPackage& package);

	// �ӽ����ȡ���ݱ��浽�����ļ�
	void SaveSetting(int settingCategory);

private:
	// ʹ�����ø��¿ؼ�
	void UpdateControls(int settingCategory);

	void UpdateKeyPanel();
	void UpdatePowerPannel();
	void UpdateDpiPanel();
	void UpdateLightPanel();
	void UpdateHuibaoPanel();
	void UpdateLodPanel();
	void UpdateQudouPanel();
	void UpdateSensorPanel();

private:
	void SetSleepTime(int sleepTime);

private:
	HMENU m_keyMenu = NULL;

	// ���������˵��İ�ť
	CControlUI* m_clickedKeyBtn = nullptr;

	// �ȴ�����
	CWaitingWindow* m_waitingWindow = nullptr;
};