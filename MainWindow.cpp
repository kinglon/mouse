#include "stdafx.h"
#include "MainWindow.h"
#include "ImPath.h"
#include "ImCharset.h"
#include "Resource.h"
#include <Commdlg.h>
#include <stdlib.h>
#include <time.h>
#include <shellapi.h>
#include <sstream>
#include <shlwapi.h>
#include <set>
#include <fstream>


CMainWindow::CMainWindow()
{
	
}

CMainWindow::~CMainWindow()
{
}

void CMainWindow::InitWindow()
{
	__super::InitWindow();

	SetIcon(IDI_MOUSE);
}

void CMainWindow::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
}

CDuiString CMainWindow::GetSkinFolder()
{
	static std::wstring strSkinRootPath = L"";
	if (!strSkinRootPath.empty())
	{
		return strSkinRootPath.c_str();
	}

	if (PathFileExists((CImPath::GetSoftInstallPath() + L"resource\\").c_str()))
	{
		strSkinRootPath = L"resource\\";
		return strSkinRootPath.c_str();
	}
	else if (PathFileExists((CImPath::GetSoftInstallPath() + L"..\\resource\\").c_str()))
	{
		strSkinRootPath = L"..\\resource\\";
		return strSkinRootPath.c_str();
	}
	else
	{
		return L"";
	}
}

CDuiString CMainWindow::GetSkinFile()
{
	return L"main.xml";
}

LPCTSTR CMainWindow::GetWindowClassName() const
{
	return L"MouseMain";
}

LRESULT CMainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_NCLBUTTONDBLCLK)
	{
		// ½ûÖ¹Ë«»÷·Å´ó´°¿Ú
		return 0L;
	}
		
	return __super::HandleMessage(uMsg, wParam, lParam);
}