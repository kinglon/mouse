#pragma once

#include <string>
#include "SettingManager.h"

typedef struct
{
	unsigned char uc_reportID;
	unsigned char aucCode[2];
} ST_KEY_CODE;

class CMacroEventMapping
{
public:
	// 获取宏事件的名字
	static std::wstring GetEventName(const CMacroEvent& event);

	// 获取按键名字
	static std::wstring GetKeyName(int vkCode);	

	// 根据Windows系统的vk码获取自己设备的按键码
	static ST_KEY_CODE* GetKeyCodeByVkCode(int vkCode);
};

