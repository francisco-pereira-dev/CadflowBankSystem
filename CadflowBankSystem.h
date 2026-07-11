
// CadflowBankSystem.h: main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "inclua 'pch.h' antes de incluir este ficheiro para PCH"
#endif

#include "resource.h"		// principal symbols


/**
 * @class CCadflowBankSystemApp
 * @brief MFC application entry point (AppWizard-generated CWinApp).
 *
 * Owns no business logic of its own: its sole responsibility is process-level
 * setup and launching CCadflowBankSystemDlg as the application's main (and only) window.
 */
class CCadflowBankSystemApp : public CWinApp
{
public:
	CCadflowBankSystemApp();

// Replaces
public:
	/**
	 * @brief Standard MFC entry point, called once by the framework at process startup.
	 * @return BOOL Always returns FALSE here, since the app is dialog-based: closing the
	 * main dialog must end the process instead of entering a message-pump loop.
	 */
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CCadflowBankSystemApp theApp;
