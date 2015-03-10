// McdKeysDlg.h : header file
//

#if !defined(AFX_MCDKEYSDLG_H__5469E217_AA04_447F_8854_69B6AC4430A3__INCLUDED_)
#define AFX_MCDKEYSDLG_H__5469E217_AA04_447F_8854_69B6AC4430A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMcdKeysDlg dialog

class CMcdKeysDlg : public CDialog
{
// Construction
public:
	CMcdKeysDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMcdKeysDlg)
	enum { IDD = IDD_MCDKEYS_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMcdKeysDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	CClientDC *m_d;
	CWinThread *m_th2;
	CWinThread *m_th1;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMcdKeysDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT ID);
	afx_msg void OnDestroy();
	afx_msg void OnStart();
	afx_msg void OnStop();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	
	//afx_msg void OnReleasedcaptureSlider1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MCDKEYSDLG_H__5469E217_AA04_447F_8854_69B6AC4430A3__INCLUDED_)
