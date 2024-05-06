#include "stdafx.h"
#include "MacroEventMapping.h"

ST_KEY_CODE st_code_data[255] =
{
	{ 0x00, 0x00, 0x00 },//						    00
	{ 0x00, 0x00, 0x00 },//VK_LBUTTON				01					������
	{ 0x00, 0x00, 0x00 },//VK_RBUTTON				02					����Ҽ� 
	{ 0x00, 0x00, 0x00 },//VK_CANCEL				03					Control-break   ����
	{ 0x00, 0x00, 0x00 },//VK_MBUTTON				04					����м� 
	{ 0x00, 0x00, 0x00 },//							05
	{ 0x00, 0x00, 0x00 },//							06
	{ 0x00, 0x00, 0x00 },//							07
	{ 0x01, 0x00, 0x2A },//VK_BACK					08					BACKSPACE	�� 
	{ 0x01, 0x00, 0x2B },//VK_TAB					09					TAB			��
	{ 0x00, 0x00, 0x00 },//							0A
	{ 0x00, 0x00, 0x00 },//							0B
	{ 0x01, 0x00, 0x9C },//VK_CLEAR					0C					CLEAR		��
	{ 0x01, 0x00, 0x28 },//VK_RETURN				0D					ENTER		��
	{ 0x00, 0x00, 0x00 },//							0E
	{ 0x00, 0x00, 0x00 },//							0F
	{ 0x01, 0x02, 0x00 },//VK_SHIFT					10					SHIFT		�� 
	{ 0x01, 0x01, 0x00 },//VK_CONTROL				11					CTRL		�� 
	{ 0x01, 0x04, 0x00 },//VK_MENU					12					ALT			�� 
	{ 0x01, 0x00, 0x48 },//VK_PAUSE					13					PAUSE		��  
	{ 0x01, 0x00, 0x82 },//VK_CAPITAL				14					CAPS LOCK   �� 
	{ 0x00, 0x00, 0x00 },//							15
	{ 0x00, 0x00, 0x00 },//							16
	{ 0x00, 0x00, 0x00 },//							17
	{ 0x00, 0x00, 0x00 },//							18
	{ 0x00, 0x00, 0x00 },//							19
	{ 0x00, 0x00, 0x00 },//							1A
	{ 0x01, 0x00, 0x29 },//VK_ESCAPE				1B						ESC			��
	{ 0x01, 0x00, 0x00 },//VK_CONVERT				1C
	{ 0x01, 0x00, 0x00 },//VK_NONCONVERT			1D
	{ 0x01, 0x00, 0x00 },//VK_ACCEPT				1E
	{ 0x01, 0x00, 0x00 },//VK_MODECHANGE			1F
	{ 0x01, 0x00, 0x2C },//VK_SPACE					20					SPACEBAR 
	{ 0x01, 0x00, 0x4B },//VK_PRIOR					21					PAGE UP		�� 
	{ 0x01, 0x00, 0x4E },//VK_NEXT					22					PAGE DOWN   �� 
	{ 0x01, 0x00, 0x4D },//VK_END					23					END			��   
	{ 0x01, 0x00, 0x4A },//VK_HOME					24					HOME		��
	{ 0x01, 0x00, 0x50 },//VK_LEFT					25					LEFT ARROW   �� 
	{ 0x01, 0x00, 0x52 },//VK_UP					26					UP   ARROW   �� 
	{ 0x01, 0x00, 0x4F },//VK_RIGHT					27					RIGHT  ARROW   �� 
	{ 0x01, 0x00, 0x51 },//VK_DOWN					28					DOWN   ARROW   ��
	{ 0x01, 0x00, 0x77 },//VK_SELECT				29					SELECT   ��   
	{ 0x00, 0x00, 0x00 },//							2A
	{ 0x01, 0x00, 0x74 },//VK_EXECUTE				2B					EXECUTE   �� 
	{ 0x01, 0x00, 0x46 },//VK_SNAPSHOT				2C					PRINT   SCREEN��
	{ 0x01, 0x00, 0x49 },//VK_INSERT				2D					INS   �� 
	{ 0x01, 0x00, 0x4C },//VK_DELETE				2E                  DEL   ��  
	{ 0x01, 0x00, 0x75 },//VK_HELP					2F					HELP��
	{ 0x01, 0x00, 0x1E },//VK_NUM_KB0				30					0 ��
	{ 0x01, 0x00, 0x1F },//VK_NUM_KB1				31					1 ��
	{ 0x01, 0x00, 0x20 },//VK_NUM_KB2				32					2 ��
	{ 0x01, 0x00, 0x21 },//VK_NUM_KB3				33					3 ��
	{ 0x01, 0x00, 0x22 },//VK_NUM_KB4				34					4 ��
	{ 0x01, 0x00, 0x23 },//VK_NUM_KB5				35					5 ��
	{ 0x01, 0x00, 0x24 },//VK_NUM_KB6				36					6 ��
	{ 0x01, 0x00, 0x25 },//VK_NUM_KB7				37					7 ��
	{ 0x01, 0x00, 0x26 },//VK_NUM_KB8				38					8 ��
	{ 0x01, 0x00, 0x27 },//VK_NUM_KB9				39					9 ��
	{ 0x00, 0x00, 0x00 },//							3A
	{ 0x00, 0x00, 0x00 },//							3B
	{ 0x00, 0x00, 0x00 },//							3C
	{ 0x00, 0x00, 0x00 },//							3D
	{ 0x00, 0x00, 0x00 },//							3E
	{ 0x00, 0x00, 0x00 },//							3F
	{ 0x00, 0x00, 0x00 },//							40
	{ 0x01, 0x00, 0x04 },//VK_NUM_KB_A				41					A ��
	{ 0x01, 0x00, 0x05 },//VK_NUM_KB_B				42					B ��
	{ 0x01, 0x00, 0x06 },//VK_NUM_KB_C				43					C ��
	{ 0x01, 0x00, 0x07 },//VK_NUM_KB_D				44					D ��
	{ 0x01, 0x00, 0x08 },//VK_NUM_KB_E				45					E ��
	{ 0x01, 0x00, 0x09 },//VK_NUM_KB_F				46					F ��
	{ 0x01, 0x00, 0x0A },//VK_NUM_KB_G				47					G ��
	{ 0x01, 0x00, 0x0B },//VK_NUM_KB_H				48					H ��
	{ 0x01, 0x00, 0x0C },//VK_NUM_KB_I				49					I ��
	{ 0x01, 0x00, 0x0D },//VK_NUM_KB_J				4A					J ��
	{ 0x01, 0x00, 0x0E },//VK_NUM_KB_K				4B					K ��
	{ 0x01, 0x00, 0x0F },//VK_NUM_KB_L				4C					L ��
	{ 0x01, 0x00, 0x10 },//VK_NUM_KB_M				4D					M ��
	{ 0x01, 0x00, 0x11 },//VK_NUM_KB_N				4E					N ��
	{ 0x01, 0x00, 0x12 },//VK_NUM_KB_O				4F					O ��
	{ 0x01, 0x00, 0x13 },//VK_NUM_KB_P				50					P ��
	{ 0x01, 0x00, 0x14 },//VK_NUM_KB_Q				51					Q ��
	{ 0x01, 0x00, 0x15 },//VK_NUM_KB_R				52					R ��
	{ 0x01, 0x00, 0x16 },//VK_NUM_KB_S				53					S ��
	{ 0x01, 0x00, 0x17 },//VK_NUM_KB_T				54					T ��
	{ 0x01, 0x00, 0x18 },//VK_NUM_KB_U				55					U ��
	{ 0x01, 0x00, 0x19 },//VK_NUM_KB_V				56					V ��
	{ 0x01, 0x00, 0x1A },//VK_NUM_KB_W				57					W ��
	{ 0x01, 0x00, 0x1B },//VK_NUM_KB_X				58					X ��
	{ 0x01, 0x00, 0x1C },//VK_NUM_KB_Y				59					Y ��
	{ 0x01, 0x00, 0x1D },//VK_NUM_KB_Z				5A					Z ��
	{ 0x01, 0x08, 0x00 },//VK_LWIN					5B					Left   Windows   ��   (Microsoft��Ȼ����)  
	{ 0x01, 0x80, 0x00 },//VK_RWIN					5C					Right   Windows   ��   (Microsoft��Ȼ����)
	{ 0x01, 0x00, 0x65 },//VK_APPS					5D					Applications   ��   (Microsoft��Ȼ����) 
	{ 0x00, 0x00, 0x00 },//							5E
	{ 0x01, 0x00, 0x00 },//VK_SLEEP       		    5F
	{ 0x01, 0x00, 0x62 },//VK_NUMPAD0				60					����С�����ϵ�   0   ��  
	{ 0x01, 0x00, 0x59 },//VK_NUMPAD1				61                  ����С�����ϵ�   1   ��
	{ 0x01, 0x00, 0x5A },//VK_NUMPAD2				62                  ����С�����ϵ�   2   ��
	{ 0x01, 0x00, 0x5B },//VK_NUMPAD3				63                  ����С�����ϵ�   3   ��
	{ 0x01, 0x00, 0x5C },//VK_NUMPAD4				64                  ����С�����ϵ�   4   ��
	{ 0x01, 0x00, 0x5D },//VK_NUMPAD5				65                  ����С�����ϵ�   5   ��  
	{ 0x01, 0x00, 0x5E },//VK_NUMPAD6				66                  ����С�����ϵ�   6   �� 
	{ 0x01, 0x00, 0x5F },//VK_NUMPAD7				67                  ����С�����ϵ�   7   ��  
	{ 0x01, 0x00, 0x60 },//VK_NUMPAD8				68                  ����С�����ϵ�   8   ��  
	{ 0x01, 0x00, 0x61 },//VK_NUMPAD9				69                  ����С�����ϵ�   9   ��
	{ 0x01, 0x00, 0x55 },//VK_MULTIPLY				6A                  Multiply   ��
	{ 0x01, 0x00, 0x57 },//VK_ADD          	  		6B                  Add�� 
	{ 0x01, 0x00, 0x9F },//VK_SEPARATOR				6C					Separator   �� 
	{ 0x01, 0x00, 0x56 },//VK_SUBTRACT				6D					Subtract   �� 
	{ 0x01, 0x00, 0x63 },//VK_DECIMAL				6E                  Decimal   �� 
	{ 0x01, 0x00, 0x54 },//VK_DIVIDE				6F					Divide   ��
	{ 0x01, 0x00, 0x3A },//VK_F1               		70                  F1   �� 
	{ 0x01, 0x00, 0x3B },//VK_F2               		71                  F2   ��  
	{ 0x01, 0x00, 0x3C },//VK_F3               		72                  F3   ��  
	{ 0x01, 0x00, 0x3D },//VK_F4               		73                  F4   ��
	{ 0x01, 0x00, 0x3E },//VK_F5               		74                  F5   ��   
	{ 0x01, 0x00, 0x3F },//VK_F6               		75                  F6   ��   
	{ 0x01, 0x00, 0x40 },//VK_F7               		76                  F7   ��   
	{ 0x01, 0x00, 0x41 },//VK_F8               		77                  F8   ��   
	{ 0x01, 0x00, 0x42 },//VK_F9               		78                  F9   ��   
	{ 0x01, 0x00, 0x43 },//VK_F10					79                  F10   ��   
	{ 0x01, 0x00, 0x44 },//VK_F11              		7A                  F11   ��   
	{ 0x01, 0x00, 0x45 },//VK_F12              		7B                  F12   ��   
	{ 0x01, 0x00, 0x68 },//VK_F13              		7C                  F13   ��   
	{ 0x01, 0x00, 0x69 },//VK_F14             		7D                  F14   ��   
	{ 0x01, 0x00, 0x6A },//VK_F15              		7E                  F15   ��   
	{ 0x01, 0x00, 0x6B },//VK_F16              		7F                  F16   ��   
	{ 0x01, 0x00, 0x6C },//VK_F17              		80                  F17   ��   
	{ 0x01, 0x00, 0x6D },//VK_F18              		81                  F18   ��   
	{ 0x01, 0x00, 0x6E },//VK_F19              		82                  F19   ��   
	{ 0x01, 0x00, 0x6F },//VK_F20              		83                  F20   ��   
	{ 0x01, 0x00, 0x70 },//VK_F21              		84                  F21   ��   
	{ 0x01, 0x00, 0x71 },//VK_F22              		85                  F22   ��   
	{ 0x01, 0x00, 0x72 },//VK_F23              		86                  F23   ��   
	{ 0x01, 0x00, 0x73 },//VK_F24              		87                  F24   �� 
	{ 0x00, 0x00, 0x00 },//							88
	{ 0x00, 0x00, 0x00 },//							89
	{ 0x00, 0x00, 0x00 },//							8A
	{ 0x00, 0x00, 0x00 },//							8B
	{ 0x00, 0x00, 0x00 },//							8C
	{ 0x00, 0x00, 0x00 },//							8D
	{ 0x00, 0x00, 0x00 },//							8E
	{ 0x00, 0x00, 0x00 },//							8F
	{ 0x01, 0x00, 0x53 },//VK_NUMLOCK				90                  NUM   LOCK   ��   
	{ 0x01, 0x00, 0x47 },//VK_SCROLL				91                  SCROLL   LOCK   ��  
	{ 0x01, 0x00, 0x86 },//VK_OEM_NEC_EQUAL			92
	{ 0x00, 0x00, 0x00 },//VK_OEM_FJ_MASSHOU   		93
	{ 0x00, 0x00, 0x00 },//VK_OEM_FJ_TOUROKU		94
	{ 0x00, 0x00, 0x00 },//VK_OEM_FJ_LOYA			95
	{ 0x00, 0x00, 0x00 },//VK_OEM_FJ_ROYA			96
	{ 0x00, 0x00, 0x00 },//							97
	{ 0x00, 0x00, 0x00 },//							98
	{ 0x00, 0x00, 0x00 },//							99
	{ 0x00, 0x00, 0x00 },//							9A
	{ 0x00, 0x00, 0x00 },//							9B
	{ 0x00, 0x00, 0x00 },//							9C
	{ 0x00, 0x00, 0x00 },//							9D
	{ 0x00, 0x00, 0x00 },//							9E
	{ 0x00, 0x00, 0x00 },//							9F
	{ 0x01, 0x02, 0x00 },//VK_LSHIFT				A0
	{ 0x01, 0x20, 0x00 },//VK_RSHIFT				A1
	{ 0x01, 0x01, 0x00 },//VK_LCONTROL				A2
	{ 0x01, 0x10, 0x00 },//VK_RCONTROL         		A3
	{ 0x01, 0x04, 0x00 },//VK_LMENU            		A4
	{ 0x01, 0x40, 0x00 },//VK_RMENU            		A5
	{ 0x01, 0x00, 0x00 },//VK_BROWSER_BACK          A6
	{ 0x01, 0x00, 0x00 },//VK_BROWSER_FORWARD       A7
	{ 0x01, 0x00, 0x00 },//VK_BROWSER_REFRESH       A8
	{ 0x01, 0x00, 0x00 },//VK_BROWSER_STOP          A9
	{ 0x01, 0x00, 0x00 },//VK_BROWSER_SEARCH        AA
	{ 0x01, 0x00, 0x00 },//VK_BROWSER_FAVORITES     AB
	{ 0x01, 0xe2, 0x00 },//VK_BROWSER_HOME          AC
	{ 0x03, 0xe2, 0x00 },//VK_VOLUME_MUTE           AD
	{ 0x03, 0xea, 0x00 },//VK_VOLUME_DOWN           AE
	{ 0x03, 0xe9, 0x00 },//VK_VOLUME_UP             AF
	{ 0x03, 0xB5, 0x00 },//VK_MEDIA_NEXT_TRACK      B0
	{ 0x03, 0xB6, 0x00 },//VK_MEDIA_PREV_TRACK      B1
	{ 0x03, 0xCC, 0x00 },//VK_MEDIA_STOP            B2
	{ 0x03, 0xcd, 0x00 },//VK_MEDIA_PLAY_PAUSE      B3
	{ 0x01, 0x00, 0x00 },//VK_LAUNCH_MAIL           B4
	{ 0x01, 0x00, 0x00 },//VK_LAUNCH_MEDIA_SELECT   B5
	{ 0x01, 0x00, 0x00 },//VK_LAUNCH_APP1           B6
	{ 0x01, 0x00, 0x00 },//VK_LAUNCH_APP2           B7
	{ 0x00, 0x00, 0x00 },//			             	B8
	{ 0x00, 0x00, 0x00 },//			             	B9
	{ 0x01, 0x00, 0x33 },//VK_OEM_1            		BA
	{ 0x01, 0x00, 0x2E },//VK_OEM_PLUS         		BB 
	{ 0x01, 0x00, 0x36 },//VK_OEM_COMMA        		BC
	{ 0x01, 0x00, 0x2D },//VK_OEM_MINUS        		BD
	{ 0x01, 0x00, 0x37 },//VK_OEM_PERIOD       		BE
	{ 0x01, 0x00, 0x38 },//VK_OEM_2            		BF
	{ 0x01, 0x00, 0x32 },//VK_OEM_3            		C0
	{ 0x00, 0x00, 0x00 },//		            		C1
	{ 0x00, 0x00, 0x00 },//		            		C2
	{ 0x00, 0x00, 0x00 },//		            		C3
	{ 0x00, 0x00, 0x00 },//		            		C4
	{ 0x00, 0x00, 0x00 },//		            		C5
	{ 0x00, 0x00, 0x00 },//		            		C6
	{ 0x00, 0x00, 0x00 },//		            		C7
	{ 0x00, 0x00, 0x00 },//		            		C8
	{ 0x00, 0x00, 0x00 },//		            		C9
	{ 0x00, 0x00, 0x00 },//		            		CA
	{ 0x00, 0x00, 0x00 },//		            		CB
	{ 0x00, 0x00, 0x00 },//		            		CC
	{ 0x00, 0x00, 0x00 },//		            		CD
	{ 0x00, 0x00, 0x00 },//		            		CE
	{ 0x00, 0x00, 0x00 },//		            		CF
	{ 0x00, 0x00, 0x00 },//		            		D0
	{ 0x00, 0x00, 0x00 },//		            		D1
	{ 0x00, 0x00, 0x00 },//		            		D2
	{ 0x00, 0x00, 0x00 },//		            		D3
	{ 0x00, 0x00, 0x00 },//		            		D4
	{ 0x00, 0x00, 0x00 },//		            		D5
	{ 0x00, 0x00, 0x00 },//		            		D6
	{ 0x00, 0x00, 0x00 },//		            		D7
	{ 0x00, 0x00, 0x00 },//		            		D8
	{ 0x00, 0x00, 0x00 },//		            		D9
	{ 0x00, 0x00, 0x00 },//		            		DA
	{ 0x01, 0x00, 0x2F },//VK_OEM_4            		DB
	{ 0x01, 0x00, 0x31 },//VK_OEM_5            		DC
	{ 0x01, 0x00, 0x30 },//VK_OEM_6            		DD
	{ 0x01, 0x00, 0x34 },//VK_OEM_7            		DE
	{ 0x01, 0x00, 0x00 },//VK_OEM_8            		DF
	{ 0x00, 0x00, 0x00 },//							E0
	{ 0x00, 0x00, 0x00 },//VK_OEM_AX           		E1
	{ 0x00, 0x00, 0x00 },//VK_OEM_102          		E2
	{ 0x00, 0x00, 0x00 },//VK_ICO_HELP         		E3 
	{ 0x00, 0x00, 0x00 },//VK_ICO_00          	 	E4 
	{ 0x00, 0x00, 0x00 },//VK_PROCESSKEY       		E5
	{ 0x00, 0x00, 0x00 },//VK_ICO_CLEAR        		E6
	{ 0x00, 0x00, 0x00 },//VK_PACKET           		E7
	{ 0x00, 0x00, 0x00 },//							E8
	{ 0x00, 0x00, 0x00 },//VK_OEM_RESET        		E9
	{ 0x00, 0x00, 0x00 },//VK_OEM_JUMP         		EA
	{ 0x00, 0x00, 0x00 },//VK_OEM_PA1          		EB
	{ 0x00, 0x00, 0x00 },//VK_OEM_PA2          		EC
	{ 0x00, 0x00, 0x00 },//VK_OEM_PA3          		ED
	{ 0x00, 0x00, 0x00 },//VK_OEM_WSCTRL       		EE
	{ 0x00, 0x00, 0x00 },//VK_OEM_CUSEL        		EF
	{ 0x00, 0x00, 0x00 },//VK_OEM_ATTN         		F0
	{ 0x00, 0x00, 0x00 },//VK_OEM_FINISH       		F1
	{ 0x00, 0x00, 0x00 },//VK_OEM_COPY         		F2
	{ 0x00, 0x00, 0x00 },//VK_OEM_AUTO         		F3
	{ 0x00, 0x00, 0x00 },//VK_OEM_BACKTAB      		F5
	{ 0x00, 0x00, 0x00 },//VK_ATTN					F6                  Attn   ��   
	{ 0x00, 0x00, 0x00 },//VK_CRSEL					F7                  CrSel   ��   
	{ 0x00, 0x00, 0x00 },//VK_EXSEL					F8                  ExSel   ��   
	{ 0x00, 0x00, 0x00 },//VK_EREOF					F9                  Erase   EOF   ��   
	{ 0x00, 0x00, 0x00 },//VK_PLAY					FA                  Play   ��   
	{ 0x00, 0x00, 0x00 },//VK_ZOOM					FB                  Zoom   ��
	{ 0x00, 0x00, 0x00 },//							FC    
	{ 0x00, 0x00, 0x00 },//							FD
	{ 0x00, 0x00, 0x00 },//VK_OEM_CLEAR				FE					Clear   ��  
	{ 0x00, 0x00, 0x00 },//							FF
};

std::wstring CMacroEventMapping::GetEventName(const CMacroEvent& event)
{
	if (event.m_type == 1)
	{
		return GetKeyName(event.m_vkCode);
	}
	else if (event.m_type == 2)
	{
		if (event.m_mouseKey == 1)
		{
			return L"L KEY";
		}
		else if (event.m_mouseKey ==2)
		{
			return L"M KEY";
		}
		else if (event.m_mouseKey == 3)
		{
			return L"R KEY";
		}
	}

	return L"";
}

std::wstring CMacroEventMapping::GetKeyName(int vkCode)
{
	static std::map<int, std::wstring> vkCode2KeyNames;
	if (vkCode2KeyNames.size() == 0)
	{
		vkCode2KeyNames[VK_BACK] = L"BACKSPACE";
		vkCode2KeyNames[VK_TAB] = L"TAB";
		vkCode2KeyNames[VK_CLEAR] = L"CLEAR";
		vkCode2KeyNames[VK_RETURN] = L"ENTER";
		vkCode2KeyNames[VK_SHIFT] = L"SHIFT";
		vkCode2KeyNames[VK_CONTROL] = L"CTRL";
		vkCode2KeyNames[VK_MENU] = L"ALT";
		vkCode2KeyNames[VK_PAUSE] = L"PAUSE";
		vkCode2KeyNames[VK_CAPITAL] = L"CAPS LOCK";
		vkCode2KeyNames[VK_ESCAPE] = L"ESC";
		vkCode2KeyNames[VK_SPACE] = L"SPACEBAR";
		vkCode2KeyNames[VK_PRIOR] = L"PAGE UP";
		vkCode2KeyNames[VK_NEXT] = L"PAGE DOWN";
		vkCode2KeyNames[VK_END] = L"END";
		vkCode2KeyNames[VK_HOME] = L"HOME";
		vkCode2KeyNames[VK_LEFT] = L"LEFT ARROW";
		vkCode2KeyNames[VK_UP] = L"UP ARROW";
		vkCode2KeyNames[VK_RIGHT] = L"RIGHT ARROW";
		vkCode2KeyNames[VK_DOWN] = L"DOWN ARROW";
		vkCode2KeyNames[VK_SELECT] = L"SELECT";
		vkCode2KeyNames[VK_EXECUTE] = L"EXECUTE";
		vkCode2KeyNames[VK_SNAPSHOT] = L"PRINT SCREEN";
		vkCode2KeyNames[VK_INSERT] = L"INS";
		vkCode2KeyNames[VK_DELETE] = L"DEL";
		vkCode2KeyNames[VK_HELP] = L"HELP";
		for (char ch = '0'; ch <= '9'; ch++)
		{
			vkCode2KeyNames[(int)ch] = std::to_wstring(ch-'0');
		}
		for (char ch = 'A'; ch <= 'Z'; ch++)
		{
			vkCode2KeyNames[(int)ch] = std::to_wstring(L'A' + ch - 'A');
		}
		vkCode2KeyNames[VK_LWIN] = L"Left Windows";
		vkCode2KeyNames[VK_RWIN] = L"Right Windows";
		vkCode2KeyNames[VK_APPS] = L"Applications";
		for (int i = VK_NUMPAD0; i <= VK_NUMPAD9; i++)
		{
			vkCode2KeyNames[i] = std::to_wstring(L'0' + i - VK_NUMPAD0);
		}
		vkCode2KeyNames[VK_MULTIPLY] = L"MULTIPLY";
		vkCode2KeyNames[VK_ADD] = L"ADD";
		vkCode2KeyNames[VK_SEPARATOR] = L"SEPARATOR";
		vkCode2KeyNames[VK_SUBTRACT] = L"SUBTRACT";
		vkCode2KeyNames[VK_DECIMAL] = L"DECIMAL";
		vkCode2KeyNames[VK_DIVIDE] = L"DIVIDE";
		for (int i = VK_F1; i <= VK_F24; i++)
		{
			vkCode2KeyNames[i] = std::wstring(L"F") + std::to_wstring(i - VK_F1 + 1);
		}
		vkCode2KeyNames[VK_NUMLOCK] = L"NUM LOCK";
		vkCode2KeyNames[VK_SCROLL] = L"SCROLL LOCK";
		vkCode2KeyNames[VK_OEM_NEC_EQUAL] = L"NEC EQUAL";
		vkCode2KeyNames[VK_LSHIFT] = L"LEFT SHIFT";
		vkCode2KeyNames[VK_RSHIFT] = L"RIGHT SHIFT";
		vkCode2KeyNames[VK_LCONTROL] = L"LEFT CTRL";
		vkCode2KeyNames[VK_RCONTROL] = L"RIGHT CTRL";
		vkCode2KeyNames[VK_LMENU] = L"LEFT ALT";
		vkCode2KeyNames[VK_RMENU] = L"RIGHT ALT";
		vkCode2KeyNames[VK_BROWSER_HOME] = L"BROWSER HOME";
		vkCode2KeyNames[VK_VOLUME_MUTE] = L"VOLUME MUTE";
		vkCode2KeyNames[VK_VOLUME_DOWN] = L"VOLUME DOWN";
		vkCode2KeyNames[VK_VOLUME_UP] = L"VOLUME UP";
		vkCode2KeyNames[VK_MEDIA_NEXT_TRACK] = L"NEXT TRACK";
		vkCode2KeyNames[VK_MEDIA_PREV_TRACK] = L"PREV TRACK";
		vkCode2KeyNames[VK_MEDIA_STOP] = L"STOP";
		vkCode2KeyNames[VK_MEDIA_PLAY_PAUSE] = L"PLAY PAUSE";
		vkCode2KeyNames[VK_OEM_1] = L"OEM 1";
		vkCode2KeyNames[VK_OEM_PLUS] = L"OEM PLUS";
		vkCode2KeyNames[VK_OEM_COMMA] = L"OEM COMMA";
		vkCode2KeyNames[VK_OEM_MINUS] = L"OEM MINUS";
		vkCode2KeyNames[VK_OEM_PERIOD] = L"OEM PERIOD";
		vkCode2KeyNames[VK_OEM_2] = L"OEM 2";
		vkCode2KeyNames[VK_OEM_3] = L"OEM 3";
		vkCode2KeyNames[VK_OEM_4] = L"OEM 4";
		vkCode2KeyNames[VK_OEM_5] = L"OEM 5";
		vkCode2KeyNames[VK_OEM_6] = L"OEM 6";
		vkCode2KeyNames[VK_OEM_7] = L"OEM 7";
	}
	
	auto& it = vkCode2KeyNames.find(vkCode);
	if (it == vkCode2KeyNames.end())
	{
		return L"";
	}
	else
	{
		return it->second;
	}
}

ST_KEY_CODE* CMacroEventMapping::GetKeyCodeByVkCode(int vkCode)
{
	if (vkCode < 0 || vkCode >= ARRAYSIZE(st_code_data))
	{
		return nullptr;
	}

	return &st_code_data[vkCode];
}