// McdKeys.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "McdKeys.h"
#include "McdKeysDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMcdKeysApp

BEGIN_MESSAGE_MAP(CMcdKeysApp, CWinApp)
	//{{AFX_MSG_MAP(CMcdKeysApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMcdKeysApp construction

CMcdKeysApp::CMcdKeysApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMcdKeysApp object

CMcdKeysApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMcdKeysApp initialization

BOOL CMcdKeysApp::InitInstance()
{
	AfxEnableControlContainer();


#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CMcdKeysDlg dlg;
	m_pMainWnd = &dlg;

		int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{

	}
	else if (nResponse == IDCANCEL)
	{

	}

	return FALSE;
}
