/// @file       UILabelEx.h
/// @brief      ֧���Զ����е�LabelUI, 
/// �޸��� DuiLib::CLabelUI::PaintText Ϊ�麯��, LabelUIEx�̳���CLabelUI
/// ������ virtual void PaintText(HDC hDC);


#ifndef __UI_LABEL_EX_H__
#define __UI_LABEL_EX_H__

#pragma once

namespace IcrDuiLib
{
	class UILIB_API CLabelUIEx :
		public IcrDuiLib::CLabelUI
	{
	public:
		CLabelUIEx(void);
		virtual ~CLabelUIEx(void);


		virtual void PaintText(HDC hDC);
		virtual void SetPos(RECT rc);


	private:
		Color _MakeRGB(int a, Color cl);
		Color _MakeRGB(int r, int g, int b);
	};
}


#endif

