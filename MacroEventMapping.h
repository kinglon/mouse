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

	// ����Windowsϵͳ��vk���ȡ�Լ��豸�İ�����
	static ST_KEY_CODE* GetKeyCodeByVkCode(int vkCode);
};

