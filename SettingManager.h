#pragma once

#include <vector>
#include "KeyMapping.h"

// 灯效设置
#define LIGHT_CLOSE		0X00
#define LIGHT_BREATH	0X01
#define LIGHT_ON		0X02

// 回报率
#define HUIBAORATE_125HZ		0X00
#define HUIBAORATE_250HZ		0X01
#define HUIBAORATE_500HZ		0X02
#define HUIBAORATE_1000HZ		0X03
#define HUIBAORATE_2000HZ		0X04
#define HUIBAORATE_4000HZ		0X05
#define HUIBAORATE_8000HZ		0X06

// 鼠标LOD高度
#define LOD_1MM		0X01
#define LOD_2MM		0X02

class CMouseDpiSetting
{
public:
	CMouseDpiSetting()
	{

	}

	CMouseDpiSetting(int dpiLevel, int dpiColor)
	{
		m_dpiLevel = dpiLevel;
		m_dpiColor = dpiColor;
	}

public:
	// DPI等级
	int m_dpiLevel = 800;

	// DPI颜色(RGB)
	int m_dpiColor = 0xff0000;
};

class CMouseConfig
{
public:
	CMouseConfig()
	{
		m_dpiSetting[0] = CMouseDpiSetting(800, 0xffff0000);
		m_dpiSetting[1] = CMouseDpiSetting(1600, 0xff00ff00);
		m_dpiSetting[2] = CMouseDpiSetting(2400, 0xff0000ff);
		m_dpiSetting[3] = CMouseDpiSetting(3200, 0xffffff00);
		m_dpiSetting[4] = CMouseDpiSetting(5000, 0xff00ffff);
		m_dpiSetting[5] = CMouseDpiSetting(26000, 0xffff00ff);
	};

public:
	// 第1个按键，默认是左键
	int m_firstKey = KEY_INDEX_LEFT;

	// 第2个按键，默认是右键
	int m_secondKey = KEY_INDEX_RIGHT;

	// 第3个按键，默认是中键
	int m_thirdtKey = KEY_INDEX_MIDDLE;

	// 第4个按键，默认是后退键
	int m_fourthKey = KEY_INDEX_BACKWARD;

	// 第5个按键，默认是前进键
	int m_fifthKey = KEY_INDEX_FORWARD;

	// 第6个按键，默认是DPI切换
	int m_sixthKey = KEY_INDEX_DPISWITCH;

	// 休眠时间，默认是1分钟
	int m_sleepTime = 1;

	// 当前使用的DPI索引，从0开始
	int m_currentDpi = 2;

	// DPI设置
	CMouseDpiSetting m_dpiSetting[6];

	// 灯效设置，默认常亮
	int m_lightIndex = LIGHT_ON;

	// 回报率设置，默认4000HZ
	int m_huibaorate = HUIBAORATE_4000HZ;

	// 鼠标LOD高度
	int m_lodIndex = LOD_1MM;

	// 按键去抖时间，默认1分钟
	int m_qudouTime = 1;

	// Sensor设置，直线修正
	bool m_lineRejust = false;

	// Sensor设置，Motion Sync
	bool m_motionSync = false;

	// Sensor设置，ripple control
	bool m_rippleControl = true;
};

class CSettingManager
{
protected:
	CSettingManager();

public:
	static CSettingManager* GetInstance();

public:
	// 日志级别
	int m_logLevel = 2;  // debug

	// 鼠标配置名称列表
	std::vector<std::wstring> m_mouseConfigList;

	// 当前鼠标配置名称
	std::wstring m_currentMouseConfig;

	// 当前鼠标配置
	CMouseConfig m_mouseConfig;

public:
	// 保存所有配置
	void Save(bool needSaveMouseConfig);

	// 加载鼠标配置
	void LoadMouseConfig(const std::wstring& configName);

	// 保存鼠标配置，未指定配置名称表示保存到当前的配置
	void SaveMouseConfig(const std::wstring& configName = L"");

private:
	void Load();
};
