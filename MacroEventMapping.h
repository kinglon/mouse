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

	// 获取VkCode
	static int GetVkCodeByKeyName(std::wstring keyName);

	// 根据Windows系统的vk码获取自己设备的按键码
	static ST_KEY_CODE* GetKeyCodeByVkCode(int vkCode);

	// 根据keyCode第3字节获取VkCode
	static int GetVkCodeByKeyCode(unsigned char keyCode);

	// 判断是否为特殊按键
	static bool IsSpecialKey(int vkCode);

private:
	static void InitVkCode2KeyNames();

private:
	static std::map<int, std::wstring> m_vkCode2KeyNames;
};

