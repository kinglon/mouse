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
	// ��ȡ���¼�������
	static std::wstring GetEventName(const CMacroEvent& event);

	// ��ȡ��������
	static std::wstring GetKeyName(int vkCode);

	// ��ȡVkCode
	static int GetVkCodeByKeyName(std::wstring keyName);

	// ����Windowsϵͳ��vk���ȡ�Լ��豸�İ�����
	static ST_KEY_CODE* GetKeyCodeByVkCode(int vkCode);

	// ����keyCode��3�ֽڻ�ȡVkCode
	static int GetVkCodeByKeyCode(unsigned char keyCode);

	// �ж��Ƿ�Ϊ���ⰴ��
	static bool IsSpecialKey(int vkCode);

private:
	static void InitVkCode2KeyNames();

private:
	static std::map<int, std::wstring> m_vkCode2KeyNames;
};

