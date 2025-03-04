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

	keys.push_back(CKeyItem(KEY_INDEX_LEFT, L"左键", ID_KEY_LEFT));
	keys.push_back(CKeyItem(KEY_INDEX_RIGHT, L"右键", ID_KEY_RIGHT));
	keys.push_back(CKeyItem(KEY_INDEX_MIDDLE, L"中键", ID_KEY_MIDDLE));
	keys.push_back(CKeyItem(KEY_INDEX_FORWARD, L"前进", ID_KEY_FORWARD));
	keys.push_back(CKeyItem(KEY_INDEX_BACKWARD, L"后退", ID_KEY_BACKWARD));

	keys.push_back(CKeyItem(KEY_INDEX_DPIPLUS, L"DPI+", ID_DPI_DPIPLUS));
	keys.push_back(CKeyItem(KEY_INDEX_DPISUB, L"DPI-", ID_DPI_DPISUBTRACT));
	keys.push_back(CKeyItem(KEY_INDEX_DPISWITCH, L"DPI切换", ID_DPI_DPISWITCH));

	keys.push_back(CKeyItem(KEY_INDEX_HUOLI, L"火力键", ID_KEY_HUOLI));
	keys.push_back(CKeyItem(KEY_INDEX_SANLIANFA, L"三连发", ID_KEY_LIANFA));
	keys.push_back(CKeyItem(KEY_INDEX_MACRO, L"宏定义", ID_KEY_MACRO));
	keys.push_back(CKeyItem(KEY_INDEX_COMBIN, L"组合键", ID_KEY_COMBIN));

	keys.push_back(CKeyItem(KEY_INDEX_MM_PLAYER, L"播放器", ID_PLAYER));
	keys.push_back(CKeyItem(KEY_INDEX_MM_LAST, L"上一首", ID_LAST_MUSIC));
	keys.push_back(CKeyItem(KEY_INDEX_MM_NEXT, L"下一首", ID_NEXT_MUSIC));
	keys.push_back(CKeyItem(KEY_INDEX_MM_STOP, L"停止", ID_MM_STOP));
	keys.push_back(CKeyItem(KEY_INDEX_MM_PLAY, L"播放/暂停", ID_MM_PLAY));
	keys.push_back(CKeyItem(KEY_INDEX_MM_MUSE, L"静音", ID_MM_MUSE));
	keys.push_back(CKeyItem(KEY_INDEX_MM_VADD, L"音量+", ID_MM_ADD_VOLUMN));
	keys.push_back(CKeyItem(KEY_INDEX_MM_VSUB, L"音量-", ID_MM_SUBTRACT_VOLUMN));
	keys.push_back(CKeyItem(KEY_INDEX_MM_MAIL, L"邮件", ID_MM_EMAIL));
	keys.push_back(CKeyItem(KEY_INDEX_MM_CALC, L"计算器", ID_MM_CALC));

	keys.push_back(CKeyItem(KEY_INDEX_COPY, L"复制", ID_OFFICE_COPY));
	keys.push_back(CKeyItem(KEY_INDEX_PASTE, L"粘贴", ID_OFFICE_PASTE));
	keys.push_back(CKeyItem(KEY_INDEX_CUT, L"剪切", ID_OFFICE_CUT));
	keys.push_back(CKeyItem(KEY_INDEX_ALL, L"全选", ID_OFFICE_ALL_SELECT));
	keys.push_back(CKeyItem(KEY_INDEX_NEW, L"新建", ID_OFFICE_NEW));
	keys.push_back(CKeyItem(KEY_INDEX_SAVE, L"保存", ID_OFFICE_SAVE));
	keys.push_back(CKeyItem(KEY_INDEX_PRINT, L"打印", ID_OFFICE_PRINT));
	keys.push_back(CKeyItem(KEY_INDEX_OPEN, L"打开", ID_OFFICE_OPEN));
	keys.push_back(CKeyItem(KEY_INDEX_UNDO, L"撤销", ID_OFFICE_UNDO));

	keys.push_back(CKeyItem(KEY_INDEX_COMPUTER, L"我的电脑", ID_WINDOW_MYCOMPUTER));
	keys.push_back(CKeyItem(KEY_INDEX_CLOSEWIN, L"关闭窗口", ID_WINDOW_CLOSE));
	keys.push_back(CKeyItem(KEY_INDEX_DESKTOP, L"显示桌面", ID_WINDOW_DESKTOP));
	keys.push_back(CKeyItem(KEY_INDEX_RUN, L"运行", ID_WINDOW_RUN));
	keys.push_back(CKeyItem(KEY_INDEX_MINIMIZE, L"最小化", ID_WINDOW_MINIMIZE));
	keys.push_back(CKeyItem(KEY_INDEX_MAXMIZE, L"最大化", ID_WINDOW_MAXMIZE));

	keys.push_back(CKeyItem(KEY_INDEX_LIGHT, L"灯效切换", ID_KEY_LIGHT));
	keys.push_back(CKeyItem(KEY_INDEX_DPILOCK, L"DPI锁定", ID_KEY_DPILOCK));
	keys.push_back(CKeyItem(KEY_INDEX_OPENAPP, L"打开APP", ID_KEY_OPENAPP));
	keys.push_back(CKeyItem(KEY_INDEX_OPENWEB, L"打开网页", ID_KEY_OPENWEB));
	keys.push_back(CKeyItem(KEY_INDEX_ENTERTEXT, L"输入文本", ID_KEY_ENTERTEXT));
	keys.push_back(CKeyItem(KEY_INDEX_DISABLE, L"禁用按键", ID_KEY_DISABLE));

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
	sanlianfa.m_loopCount = 3;
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

	keyStructs[KEY_INDEX_OPEN] = CKeyStruct(0X01, 0X01, 0X08);

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

	keyStructs[KEY_INDEX_OPENAPP] = CKeyStruct(0X04, 0X11, 0X11);

	keyStructs[KEY_INDEX_OPENWEB] = CKeyStruct(0X05, 0X11, 0X22);

	keyStructs[KEY_INDEX_ENTERTEXT] = CKeyStruct(0X06, 0X11, 0X33);

	return keyStructs;
}