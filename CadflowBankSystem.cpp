
// CadflowBankSystem.cpp: defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "CadflowBankSystem.h"
#include "CadflowBankSystemDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCadflowBankSystemApp

BEGIN_MESSAGE_MAP(CCadflowBankSystemApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CCadflowBankSystemApp builder

CCadflowBankSystemApp::CCadflowBankSystemApp()
{
	// Restart Manager support: lets Windows offer to relaunch the app after an
	// unexpected termination (e.g. Windows Update forcing a restart).
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// ALL: Add build code here,
	// Place all significant initialization in InitInstance.
}


// The only object of CCadflowBankSystemApp

CCadflowBankSystemApp theApp;


// initialization of CCadflowBankSystemApp

BOOL CCadflowBankSystemApp::InitInstance()
{
	// Required for the common-controls manifest (visual styles) to actually
	// take effect - without it, CMFCButton/CListCtrl would fall back to legacy rendering.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// ALL: you should modify this string to something appropriate,
	// such as the name of your company or organization
	SetRegistryKey(_T("Aplicacao Local Gerada pelo AppWizard"));

	// Dialog-based app: the main dialog IS the application. There is no
	// document/view or persistent main window beyond its own lifetime.
	CCadflowBankSystemDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// ALL: place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// ALL: place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Aviso: a criacao da caixa de dialogo falhou, portanto, a aplicacao vai ser encerrada inesperadamente.\n");
		TRACE(traceAppMsg, 0, "Aviso: se estiveres a usar controles do MFC na caixa de dialogo, nao sera possivel usar #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}



#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// Returning FALSE (instead of TRUE) tells the framework the dialog already
	// ran its full lifecycle and the process should exit now, rather than
	// entering CWinApp's normal message-pump loop.
	return FALSE;
}

