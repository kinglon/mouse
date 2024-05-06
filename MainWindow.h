#pragma once

#include "ProtocalUtil.h"
#include "MouseCommManager.h"
#include "WaitingWindow.h"
#include "SettingManager.h"

class CMainWindow : public WindowImplBase, public IMouseCommCallback
{
public:
	CMainWindow();
	~CMainWindow();

public:
	UIBEGIN_MSG_MAP
		EVENT_HANDLER(DUI_MSGTYPE_CLICK, OnClickEvent)
		EVENT_HANDLER(DUI_MSGTYPE_ITEMSELECT, OnItemSelectEvent)
		EVENT_HANDLER(DUI_MSGTYPE_SELECTCHANGED, OnSelectChangeEvent)
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
	void OnSelectChangeEvent(TNotifyUI& msg);
	void OnWindowInit(TNotifyUI& msg);
	bool OnEvent(void* event);

	// ���յ���������
	void OnMouseDataArrive(const unsigned char* data, int dataLength);

	// ͨ���˵�����������
	void OnMenuCommand(int commandId);

	// ������˵�����
	void OnMacroMenuCommand();

	// �ر��Ҳ����е��������
	void CloseRightSettingPannels();

	// �������ͷ����
	void SetPanelHeadBkImage(CControlUI* control, bool open);

	// ��ʼ�������˵�������Ϊ�Ի�
	void InitKeyMenu();

	void OnDrawItem(LPDRAWITEMSTRUCT lpdis);

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

	// �ӽ����ȡDPI����
	void GetDpiSetting(int& currentDpiIndex, CMouseDpiSetting dpiSetting[6]);

	// ��ɫ�Ի�������
	static UINT_PTR WINAPI ColorDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	// ���ؼ����ƻ�ȡ�������� 1-6��δ�ҵ�����-1
	int GetKeyIndexByCtrlName(std::wstring ctrlName);

private:
	// ���������
	void RunTaskPool();
	void GetBatteryFromMouse();
	void GetMouseSettingFromMouse();

	void RecvBatteryInfo(const CProtocalPackage& package);
	void RecvProtocalVersion(const CProtocalPackage& package);
	void RecvMouseSetting(const CProtocalPackage& package);

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

	// ��ȡ�ؼ�����β�������֣��������ַ���-1
	int GetDpiIndexByControlName(std::wstring controlName);
	void ClickDpiColorBtn(CButtonUI* button);
	void ClickDpiValueBtn(CButtonUI* button);
	void SwitchCurrentDpi(CButtonUI* button);

private:
	// ��������ð���
	void SetKeyToMouse(int keyNum, int keyIndex);

	// ��������ú�
	void SetMacroToMouse(int keyNum, std::wstring macroName);

	// ����갴������DPI����
	void SetDpiLockToMouse(int keyNum, int dpiValue);

	// ����귢���µ�DPI����
	void SetDpiSettingToMouse();

	// �������������ʱ��
	void SetSleepTimeToMouse();

	// ����귢���µĵ�Ч����
	void SetLightSettingToMouse();

	// ����귢���µ�LOD����
	void SetLodSettingToMouse();

	// ����귢���µ�Sensor����
	void SetSensorSettingToMouse();

	// ����귢���µĻر�������
	void SetHuibaoSettingToMouse();

private:
	void OnResetBtnClicked();
	void OnQudouBtnClicked();
	void OnSystemMouseBtnClicked();
	void OnMacroBtnClicked();
	void OnMatchBtnClicked();

private:
	static bool IsNumber(std::wstring text);

private:
	HMENU m_keyMenu = NULL;

	// ���������˵��İ�ť
	CControlUI* m_clickedKeyBtn = nullptr;

	// �ȴ�����
	CWaitingWindow* m_waitingWindow = nullptr;

	// ��ʶ�Ƿ��ֶ�����
	bool m_manualTrigger = true;

	// �����
	std::vector<int> m_taskPool;

	// ��һ�λ�ȡ������ʱ��
	DWORD64 m_nextGetBatteryTime = 0;
};