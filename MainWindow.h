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

	// 接收到鼠标的数据
	void OnMouseDataArrive(const unsigned char* data, int dataLength);

	// 通过菜单触发的命令
	void OnMenuCommand(int commandId);

	// 宏命令菜单触发
	void OnMacroMenuCommand();

	// 关闭右侧所有的设置面板
	void CloseRightSettingPannels();

	// 设置面板头背景
	void SetPanelHeadBkImage(CControlUI* control, bool open);

	// 初始化按键菜单，设置为自绘
	void InitKeyMenu();

	void OnDrawItem(LPDRAWITEMSTRUCT lpdis);

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

	// 从界面获取DPI设置
	void GetDpiSetting(int& currentDpiIndex, CMouseDpiSetting dpiSetting[6]);

	// 颜色对话框处理函数
	static UINT_PTR WINAPI ColorDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	// 按控件名称获取按键索引 1-6，未找到返回-1
	int GetKeyIndexByCtrlName(std::wstring ctrlName);

private:
	// 运行任务池
	void RunTaskPool();
	void GetBatteryFromMouse();
	void GetMouseSettingFromMouse();

	void RecvBatteryInfo(const CProtocalPackage& package);
	void RecvProtocalVersion(const CProtocalPackage& package);
	void RecvMouseSetting(const CProtocalPackage& package);

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

	// 获取控件名称尾部的数字，不是数字返回-1
	int GetDpiIndexByControlName(std::wstring controlName);
	void ClickDpiColorBtn(CButtonUI* button);
	void ClickDpiValueBtn(CButtonUI* button);
	void SwitchCurrentDpi(CButtonUI* button);

private:
	// 给鼠标配置按键
	void SetKeyToMouse(int keyNum, int keyIndex);

	// 给鼠标配置宏
	void SetMacroToMouse(int keyNum, std::wstring macroName);

	// 给鼠标按键配置DPI锁定
	void SetDpiLockToMouse(int keyNum, int dpiValue);

	// 给鼠标发送新的DPI设置
	void SetDpiSettingToMouse();

	// 给鼠标设置休眠时间
	void SetSleepTimeToMouse();

	// 给鼠标发送新的灯效设置
	void SetLightSettingToMouse();

	// 给鼠标发送新的LOD设置
	void SetLodSettingToMouse();

	// 给鼠标发送新的Sensor设置
	void SetSensorSettingToMouse();

	// 给鼠标发送新的回报率设置
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

	// 触发按键菜单的按钮
	CControlUI* m_clickedKeyBtn = nullptr;

	// 等待窗口
	CWaitingWindow* m_waitingWindow = nullptr;

	// 标识是否手动触发
	bool m_manualTrigger = true;

	// 任务池
	std::vector<int> m_taskPool;

	// 下一次获取电量的时间
	DWORD64 m_nextGetBatteryTime = 0;
};