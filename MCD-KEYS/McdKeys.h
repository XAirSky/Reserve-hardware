// McdKeys.h : main header file for the MCDKEYS application
//

#if !defined(AFX_MCDKEYS_H__BFC302F2_4A53_4CD9_BDF3_62C698FECABF__INCLUDED_)
#define AFX_MCDKEYS_H__BFC302F2_4A53_4CD9_BDF3_62C698FECABF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMcdKeysApp:
// See McdKeys.cpp for the implementation of this class
//

class CMcdKeysApp : public CWinApp
{
public:
	CMcdKeysApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMcdKeysApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMcdKeysApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MCDKEYS_H__BFC302F2_4A53_4CD9_BDF3_62C698FECABF__INCLUDED_)
