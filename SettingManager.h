#pragma once

#include <vector>
#include "KeyMapping.h"

// ��Ч����
#define LIGHT_CLOSE		0X00
#define LIGHT_BREATH	0X01
#define LIGHT_ON		0X02

// �ر���
#define HUIBAORATE_125HZ		0X00
#define HUIBAORATE_250HZ		0X01
#define HUIBAORATE_500HZ		0X02
#define HUIBAORATE_1000HZ		0X03
#define HUIBAORATE_2000HZ		0X04
#define HUIBAORATE_4000HZ		0X05
#define HUIBAORATE_8000HZ		0X06

// ���LOD�߶�
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
	// DPI�ȼ�
	int m_dpiLevel = 800;

	// DPI��ɫ(RGB)
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
	// ��1��������Ĭ�������
	int m_firstKey = KEY_INDEX_LEFT;

	// ��2��������Ĭ�����Ҽ�
	int m_secondKey = KEY_INDEX_RIGHT;

	// ��3��������Ĭ�����м�
	int m_thirdtKey = KEY_INDEX_MIDDLE;

	// ��4��������Ĭ���Ǻ��˼�
	int m_fourthKey = KEY_INDEX_BACKWARD;

	// ��5��������Ĭ����ǰ����
	int m_fifthKey = KEY_INDEX_FORWARD;

	// ��6��������Ĭ����DPI�л�
	int m_sixthKey = KEY_INDEX_DPISWITCH;

	// ����ʱ�䣬Ĭ����1����
	int m_sleepTime = 1;

	// ��ǰʹ�õ�DPI��������0��ʼ
	int m_currentDpi = 2;

	// DPI����
	CMouseDpiSetting m_dpiSetting[6];

	// ��Ч���ã�Ĭ�ϳ���
	int m_lightIndex = LIGHT_ON;

	// �ر������ã�Ĭ��4000HZ
	int m_huibaorate = HUIBAORATE_4000HZ;

	// ���LOD�߶�
	int m_lodIndex = LOD_1MM;

	// ����ȥ��ʱ�䣬Ĭ��1����
	int m_qudouTime = 1;

	// Sensor���ã�ֱ������
	bool m_lineRejust = false;

	// Sensor���ã�Motion Sync
	bool m_motionSync = false;

	// Sensor���ã�ripple control
	bool m_rippleControl = true;
};

class CSettingManager
{
protected:
	CSettingManager();

public:
	static CSettingManager* GetInstance();

public:
	// ��־����
	int m_logLevel = 2;  // debug

	// ������������б�
	std::vector<std::wstring> m_mouseConfigList;

	// ��ǰ�����������
	std::wstring m_currentMouseConfig;

	// ��ǰ�������
	CMouseConfig m_mouseConfig;

public:
	// ������������
	void Save(bool needSaveMouseConfig);

	// �����������
	void LoadMouseConfig(const std::wstring& configName);

	// ����������ã�δָ���������Ʊ�ʾ���浽��ǰ������
	void SaveMouseConfig(const std::wstring& configName = L"");

private:
	void Load();
};
