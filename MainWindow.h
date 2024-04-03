#pragma once

class CMainWindow : public WindowImplBase
{
public:
	CMainWindow();
	~CMainWindow();

public:
	UIBEGIN_MSG_MAP		
		// EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK, _T("importAccountBtn"), OnImportAccountBtn)		
	UIEND_MSG_MAP

protected: //override base
	virtual void InitWindow() override;
	virtual void OnFinalMessage(HWND hWnd) override;
	virtual CDuiString GetSkinFolder() override;
	virtual CDuiString GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName(void) const override;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;	
};