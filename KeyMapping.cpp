#include "stdafx.h"
#include "KeyMapping.h"
#include "resource.h"


std::vector<CKeyItem>& CKeyMapping::GetKeys()
{
	static std::vector<CKeyItem> keys;
	if (keys.size() > 0)
	{
		return keys;
	}

	keys.push_back(CKeyItem(KEY_INDEX_LEFT, L"���", ID_KEY_LEFT));
	keys.push_back(CKeyItem(KEY_INDEX_RIGHT, L"�Ҽ�", ID_KEY_RIGHT));
	keys.push_back(CKeyItem(KEY_INDEX_MIDDLE, L"�м�", ID_KEY_MIDDLE));
	keys.push_back(CKeyItem(KEY_INDEX_FORWARD, L"ǰ��", ID_KEY_FORWARD));
	keys.push_back(CKeyItem(KEY_INDEX_BACKWARD, L"����", ID_KEY_BACKWARD));

	keys.push_back(CKeyItem(KEY_INDEX_DPIPLUS, L"DPI+", ID_DPI_DPIPLUS));
	keys.push_back(CKeyItem(KEY_INDEX_DPISUB, L"DPI-", ID_DPI_DPISUBTRACT));
	keys.push_back(CKeyItem(KEY_INDEX_DPISWITCH, L"DPI�л�", ID_DPI_DPISWITCH));

	keys.push_back(CKeyItem(KEY_INDEX_HUOLI, L"������", ID_KEY_HUOLI));
	keys.push_back(CKeyItem(KEY_INDEX_SANLIANFA, L"������", ID_KEY_LIANFA));
	keys.push_back(CKeyItem(KEY_INDEX_MACRO, L"�궨��", ID_KEY_MACRO));
	keys.push_back(CKeyItem(KEY_INDEX_COMBIN, L"��ϼ�", ID_KEY_COMBIN));

	keys.push_back(CKeyItem(KEY_INDEX_MM_PLAYER, L"������", ID_PLAYER));
	keys.push_back(CKeyItem(KEY_INDEX_MM_LAST, L"��һ��", ID_LAST_MUSIC));
	keys.push_back(CKeyItem(KEY_INDEX_MM_NEXT, L"��һ��", ID_NEXT_MUSIC));
	keys.push_back(CKeyItem(KEY_INDEX_MM_STOP, L"ֹͣ", ID_MM_STOP));
	keys.push_back(CKeyItem(KEY_INDEX_MM_PLAY, L"����/��ͣ", ID_MM_PLAY));
	keys.push_back(CKeyItem(KEY_INDEX_MM_MUSE, L"����", ID_MM_MUSE));
	keys.push_back(CKeyItem(KEY_INDEX_MM_VADD, L"����+", ID_MM_ADD_VOLUMN));
	keys.push_back(CKeyItem(KEY_INDEX_MM_VSUB, L"����-", ID_MM_SUBTRACT_VOLUMN));
	keys.push_back(CKeyItem(KEY_INDEX_MM_MAIL, L"�ʼ�", ID_MM_EMAIL));
	keys.push_back(CKeyItem(KEY_INDEX_MM_CALC, L"������", ID_MM_CALC));

	keys.push_back(CKeyItem(KEY_INDEX_COPY, L"����", ID_OFFICE_COPY));
	keys.push_back(CKeyItem(KEY_INDEX_PASTE, L"ճ��", ID_OFFICE_PASTE));
	keys.push_back(CKeyItem(KEY_INDEX_CUT, L"����", ID_OFFICE_CUT));
	keys.push_back(CKeyItem(KEY_INDEX_ALL, L"ȫѡ", ID_OFFICE_ALL_SELECT));
	keys.push_back(CKeyItem(KEY_INDEX_NEW, L"�½�", ID_OFFICE_NEW));
	keys.push_back(CKeyItem(KEY_INDEX_SAVE, L"����", ID_OFFICE_SAVE));
	keys.push_back(CKeyItem(KEY_INDEX_PRINT, L"��ӡ", ID_OFFICE_PRINT));
	keys.push_back(CKeyItem(KEY_INDEX_OPEN, L"��", ID_OFFICE_OPEN));
	keys.push_back(CKeyItem(KEY_INDEX_UNDO, L"����", ID_OFFICE_UNDO));

	keys.push_back(CKeyItem(KEY_INDEX_COMPUTER, L"�ҵĵ���", ID_WINDOW_MYCOMPUTER));
	keys.push_back(CKeyItem(KEY_INDEX_CLOSEWIN, L"�رմ���", ID_WINDOW_CLOSE));
	keys.push_back(CKeyItem(KEY_INDEX_DESKTOP, L"��ʾ����", ID_WINDOW_DESKTOP));
	keys.push_back(CKeyItem(KEY_INDEX_RUN, L"����", ID_WINDOW_RUN));
	keys.push_back(CKeyItem(KEY_INDEX_MINIMIZE, L"��С��", ID_WINDOW_MINIMIZE));
	keys.push_back(CKeyItem(KEY_INDEX_MAXMIZE, L"���", ID_WINDOW_MAXMIZE));

	keys.push_back(CKeyItem(KEY_INDEX_LIGHT, L"��Ч�л�", ID_KEY_LIGHT));
	keys.push_back(CKeyItem(KEY_INDEX_DPILOCK, L"DPI����", ID_KEY_DPILOCK));
	keys.push_back(CKeyItem(KEY_INDEX_OPENAPP, L"��APP", ID_KEY_OPENAPP));
	keys.push_back(CKeyItem(KEY_INDEX_OPENWEB, L"����ҳ", ID_KEY_OPENWEB));
	keys.push_back(CKeyItem(KEY_INDEX_ENTERTEXT, L"�����ı�", ID_KEY_ENTERTEXT));
	keys.push_back(CKeyItem(KEY_INDEX_DISABLE, L"���ð���", ID_KEY_DISABLE));

	return keys;
}

std::wstring CKeyMapping::GetKeyNameByKeyIndex(int keyIndex)
{
	std::vector<CKeyItem>& keys = GetKeys();
	for (auto& key : keys)
	{
		if (keyIndex == key.m_keyIndex)
		{
			return key.m_keyName;
		}
	}

	return L"";
}


std::wstring CKeyMapping::GetKeyNameByCommandId(int commandId)
{
	std::vector<CKeyItem>& keys = GetKeys();
	for (auto& key : keys)
	{
		if (commandId == key.m_commandId)
		{
			return key.m_keyName;
		}
	}

	return L"";
}