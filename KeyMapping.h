#pragma once

#include <vector>

// ��������
#define KEY_INDEX_BASE		0
#define KEY_INDEX_LEFT		(KEY_INDEX_BASE+1)
#define KEY_INDEX_RIGHT		(KEY_INDEX_BASE+2)
#define KEY_INDEX_MIDDLE	(KEY_INDEX_BASE+3)
#define KEY_INDEX_FORWARD	(KEY_INDEX_BASE+4)
#define KEY_INDEX_BACKWARD	(KEY_INDEX_BASE+5)
#define KEY_INDEX_DPI		(KEY_INDEX_BASE+6)
#define KEY_INDEX_DPIPLUS	(KEY_INDEX_BASE+7)
#define KEY_INDEX_DPISUB	(KEY_INDEX_BASE+8)
#define KEY_INDEX_DPISWITCH	(KEY_INDEX_BASE+9)
#define KEY_INDEX_HUOLI		(KEY_INDEX_BASE+10)
#define KEY_INDEX_SANLIANFA	(KEY_INDEX_BASE+11)
#define KEY_INDEX_MACRO		(KEY_INDEX_BASE+12)
#define KEY_INDEX_COMBIN	(KEY_INDEX_BASE+13)
#define KEY_INDEX_MM_PLAYER	(KEY_INDEX_BASE+14)
#define KEY_INDEX_MM_LAST	(KEY_INDEX_BASE+15)
#define KEY_INDEX_MM_NEXT	(KEY_INDEX_BASE+16)
#define KEY_INDEX_MM_STOP	(KEY_INDEX_BASE+17)
#define KEY_INDEX_MM_PLAY	(KEY_INDEX_BASE+18)
#define KEY_INDEX_MM_MUSE	(KEY_INDEX_BASE+19)
#define KEY_INDEX_MM_VADD	(KEY_INDEX_BASE+20)  // ����+
#define KEY_INDEX_MM_VSUB	(KEY_INDEX_BASE+21)  // ����-
#define KEY_INDEX_MM_MAIL	(KEY_INDEX_BASE+22)
#define KEY_INDEX_MM_CALC	(KEY_INDEX_BASE+23)  // ������
#define KEY_INDEX_COPY		(KEY_INDEX_BASE+24)
#define KEY_INDEX_PASTE		(KEY_INDEX_BASE+25)
#define KEY_INDEX_CUT		(KEY_INDEX_BASE+26)
#define KEY_INDEX_ALL		(KEY_INDEX_BASE+27)
#define KEY_INDEX_NEW		(KEY_INDEX_BASE+28)
#define KEY_INDEX_SAVE		(KEY_INDEX_BASE+29)
#define KEY_INDEX_PRINT		(KEY_INDEX_BASE+30)
#define KEY_INDEX_OPEN		(KEY_INDEX_BASE+31)
#define KEY_INDEX_UNDO		(KEY_INDEX_BASE+32)
#define KEY_INDEX_COMPUTER	(KEY_INDEX_BASE+33)  // �ҵĵ���
#define KEY_INDEX_CLOSEWIN	(KEY_INDEX_BASE+34)  // �رմ���
#define KEY_INDEX_DESKTOP	(KEY_INDEX_BASE+35)  // ��ʾ����
#define KEY_INDEX_RUN		(KEY_INDEX_BASE+36)
#define KEY_INDEX_MINIMIZE	(KEY_INDEX_BASE+37)
#define KEY_INDEX_MAXMIZE	(KEY_INDEX_BASE+38)
#define KEY_INDEX_LIGHT		(KEY_INDEX_BASE+39)  // ��Ч�л�
#define KEY_INDEX_DPILOCK	(KEY_INDEX_BASE+40)
#define KEY_INDEX_OPENAPP	(KEY_INDEX_BASE+41)
#define KEY_INDEX_OPENWEB	(KEY_INDEX_BASE+42)
#define KEY_INDEX_ENTERTEXT	(KEY_INDEX_BASE+43)
#define KEY_INDEX_DISABLE	(KEY_INDEX_BASE+44)

// ������������
#define KEY_CATEGORY_BASE	100
#define KEY_CATEGORY_DPI		(KEY_CATEGORY_BASE+1)
#define KEY_CATEGORY_MULTIMEDIA	(KEY_CATEGORY_BASE+2)
#define KEY_CATEGORY_OFFICE		(KEY_CATEGORY_BASE+3)
#define KEY_CATEGORY_WINDOW		(KEY_CATEGORY_BASE+4)

class CKeyItem
{
public:
	CKeyItem(int keyIndex, std::wstring keyName, int commandId)
	{
		m_keyIndex = keyIndex;
		m_keyName = keyName;
		m_commandId = commandId;
	}

public:
	// ��������
	int m_keyIndex = 0;

	// ��������
	std::wstring m_keyName;

	// �˵�����ID
	int m_commandId = 0;
};

class CKeyMapping
{
public:
	// ��ȡ����Key�б�
	static std::vector<CKeyItem>& GetKeys();

	// ����key������ȡkey����
	static std::wstring GetKeyNameByKeyIndex(int keyIndex);

	// ��������ID��ȡkey����
	static std::wstring GetKeyNameByCommandId(int commandId);

	// ����key���ƻ�ȡKey�������Ҳ�������-1
	static int GetKeyIndexByName(std::wstring name);
};

