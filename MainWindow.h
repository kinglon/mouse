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

	// 接收到鼠标的数据
	void OnMouseDataArrive(const unsigned char* data, int dataLength);

	// 通过菜单触发的命令
	void OnMenuCommand(int commandId);

	// 关闭右侧所有的设置面板
	void CloseRightSettingPannels();

	// 初始化所有的控件，只调用一次
	void InitControls();	

	// 在触发控件右侧弹出按键菜单
	void PopupKeyMenu(CControlUI* fromControl);

	// 设置Option控件Check状态
	void SetOption(COptionUI* control, bool check);

	// 发送设置给鼠标
	void SendSetting(int settingCategory, const CProtocalPackage& package);

	// 从界面获取数据保存到设置文件
	void SaveSetting(int settingCategory);

private:
	// 使用配置更新控件
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

	// 触发按键菜单的按钮
	CControlUI* m_clickedKeyBtn = nullptr;

	// 等待窗口
	CWaitingWindow* m_waitingWindow = nullptr;
};