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

int CKeyMapping::GetKeyIndexByName(std::wstring name)
{
	std::vector<CKeyItem>& keys = GetKeys();
	for (auto& key : keys)
	{
		if (name == key.m_keyName)
		{
			return key.m_keyIndex;
		}
	}

	return -1;
}

std::map<int, CKeyStruct>& CKeyMapping::GetKeyStructs()
{
	static std::map<int, CKeyStruct> keyStructs;
	if (keyStructs.size() > 0)
	{
		return keyStructs;
	}

	keyStructs[KEY_INDEX_LEFT] = CKeyStruct(0X02, 0X01, 0X00);

	keyStructs[KEY_INDEX_RIGHT] = CKeyStruct(0X02, 0X02, 0X00);

	keyStructs[KEY_INDEX_MIDDLE] = CKeyStruct(0X02, 0X04, 0X00);

	keyStructs[KEY_INDEX_BACKWARD] = CKeyStruct(0X02, 0X08, 0X00);

	keyStructs[KEY_INDEX_FORWARD] = CKeyStruct(0X02, 0X10, 0X00);

	keyStructs[KEY_INDEX_DPISWITCH] = CKeyStruct(0X02, 0XFF, 0X00);

	keyStructs[KEY_INDEX_DPIPLUS] = CKeyStruct(0X02, 0XFF, 0XFF);

	keyStructs[KEY_INDEX_DPISUB] = CKeyStruct(0X02, 0XFF, 0X7F);

	CKeyStruct huoli = CKeyStruct(0X02, 0X01, 0X00);
	huoli.m_loopCount = 2;
	keyStructs[KEY_INDEX_HUOLI] = huoli;

	CKeyStruct sanlianfa = CKeyStruct(0X02, 0X01, 0X00);
	huoli.m_loopCount = 3;
	keyStructs[KEY_INDEX_SANLIANFA] = sanlianfa;

	keyStructs[KEY_INDEX_LIGHT] = CKeyStruct(0X02, 0X0F, 0XFF);

	keyStructs[KEY_INDEX_MM_PLAYER] = CKeyStruct(0X03, 0X83, 0X01);

	keyStructs[KEY_INDEX_MM_LAST] = CKeyStruct(0X03, 0XB6, 0X00);

	keyStructs[KEY_INDEX_MM_NEXT] = CKeyStruct(0X03, 0XB5, 0X00);

	keyStructs[KEY_INDEX_MM_STOP] = CKeyStruct(0X03, 0XCC, 0X00);

	keyStructs[KEY_INDEX_MM_PLAY] = CKeyStruct(0X03, 0XCD, 0X00);

	keyStructs[KEY_INDEX_MM_MUSE] = CKeyStruct(0X03, 0XE2, 0X00);

	keyStructs[KEY_INDEX_MM_VADD] = CKeyStruct(0X03, 0XE9, 0X00);

	keyStructs[KEY_INDEX_MM_VSUB] = CKeyStruct(0X03, 0XEA, 0X00);

	keyStructs[KEY_INDEX_MM_MAIL] = CKeyStruct(0X03, 0X8A, 0X01);

	keyStructs[KEY_INDEX_MM_CALC] = CKeyStruct(0X03, 0X92, 0X01);

	keyStructs[KEY_INDEX_COPY] = CKeyStruct(0X01, 0X01, 0X06);

	keyStructs[KEY_INDEX_PASTE] = CKeyStruct(0X01, 0X01, 0X19);

	keyStructs[KEY_INDEX_CUT] = CKeyStruct(0X01, 0X01, 0X1B);

	keyStructs[KEY_INDEX_ALL] = CKeyStruct(0X01, 0X01, 0X04);

	keyStructs[KEY_INDEX_NEW] = CKeyStruct(0X01, 0X01, 0X11);

	keyStructs[KEY_INDEX_SAVE] = CKeyStruct(0X01, 0X01, 0X16);

	keyStructs[KEY_INDEX_PRINT] = CKeyStruct(0X01, 0X01, 0X13);

	keyStructs[KEY_INDEX_OPEN] = CKeyStruct(0X01, 0X08, 0X08);

	keyStructs[KEY_INDEX_UNDO] = CKeyStruct(0X01, 0X01, 0X1D);

	keyStructs[KEY_INDEX_COMPUTER] = CKeyStruct(0X01, 0X08, 0X08);

	keyStructs[KEY_INDEX_CLOSEWIN] = CKeyStruct(0X01, 0X04, 0X3D);

	keyStructs[KEY_INDEX_DESKTOP] = CKeyStruct(0X01, 0X08, 0X07);

	keyStructs[KEY_INDEX_RUN] = CKeyStruct(0X01, 0X08, 0X15);

	keyStructs[KEY_INDEX_MINIMIZE] = CKeyStruct(0X01, 0X08, 0X51);

	keyStructs[KEY_INDEX_MAXMIZE] = CKeyStruct(0X01, 0X08, 0X52);

	CKeyStruct disable;
	disable.m_disable = 0x01;
	keyStructs[KEY_INDEX_DISABLE] = disable;

	keyStructs[KEY_INDEX_OPENAPP] = CKeyStruct(0X04, 0X00, 0X00);

	keyStructs[KEY_INDEX_OPENWEB] = CKeyStruct(0X05, 0X00, 0X00);

	keyStructs[KEY_INDEX_ENTERTEXT] = CKeyStruct(0X06, 0X00, 0X00);

	return keyStructs;
}