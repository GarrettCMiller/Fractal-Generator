#pragma once
#include "afxwin.h"


// ParamsDlg dialog

class ParamsDlg : public CPropertyPage
{
	DECLARE_DYNAMIC(ParamsDlg)

public:
	ParamsDlg(/*CWnd* pParent = NULL*/);   // standard constructor
	virtual ~ParamsDlg();

// Dialog Data
	enum { IDD = IDD_FRACTAL_PARAMS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	UInt32 m_nMaxIterations;
};
